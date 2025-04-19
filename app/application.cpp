#include <SmingCore.h>
#include <LittleFS.h>
#include <JsonObjectStream.h>
#include <Libraries/Adafruit_NeoPixel/Adafruit_NeoPixel.h>
#include <config.h>
#include <ConfigDB/Json/Format.h>
#include <TimeKeeper.h>
#include <utils.h>

Config config("config"); // Path needs to match mount point defined in fsimage.fwfs

TimeKeeper timeKeeper("pool.ntp.org", 60 * 60); // Update time every hour
HttpServer server;

#define LED_PIN 12
#define NUM_LEDS 110
Adafruit_NeoPixel ledStrip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

/*
	05 -> Fünf nach {hour}
	10 -> Zehn nach {hour}
	15 -> Viertel {hour+1}
	20 -> Zehn vor halb {hour+1}
	25 -> Fünf vor halb {hour+1}
	30 -> Halb {hour+1}
	35 -> Fünf nach halb {hour+1}
	40 -> Zehn nach halb {hour+1}
	45 -> Viertel vor {hour+1}
	50 -> Zehn vor {hour+1}
	55 -> Fünf vor {hour+1}
 */
void displayTime(DateTime dt, uint32_t color)
{
	ledStrip.clear();

	ledStrip.fill(color, 0, 2); // "Es"
	ledStrip.fill(color, 3, 3); // "Ist"

	auto minute = (dt.Minute / 5) * 5; // round down to the next multiple of 5
	auto hour = dt.Hour;

	switch (minute)
	{
	// "Fünf"
	case 5:
	case 25:
	case 35:
	case 55:
		ledStrip.fill(color, 7, 4);
		break;
	// "Zehn"
	case 10:
	case 20:
	case 40:
	case 50:
		ledStrip.fill(color, 11, 4);
		break;
	// "Viertel"
	case 15:
	case 45:
		ledStrip.fill(color, 25, 7);
		break;
	}

	switch (minute)
	{
	// "Nach"
	case 5:
	case 10:
	case 35:
	case 40:
		ledStrip.fill(color, 18, 4);
		break;
	// "Vor"
	case 20:
	case 25:
	case 45:
	case 50:
	case 55:
		ledStrip.fill(color, 34, 3);
		break;
	}

	// "Halb"
	if (minute >= 20 && minute < 45)
	{
		ledStrip.fill(color, 40, 4);
	}

	// Unless the time is shown as "nach {hour}" it should be relative to the next full hour e.g. "viertel eins"
	if (minute > 10)
		hour += 1;

	switch (hour % 12)
	{
	case 0: // "Zwölf"
		ledStrip.fill(color, 94, 5);
		break;
	case 1:
		ledStrip.fill(color, 55, 4);
		break;
	case 2:
		ledStrip.fill(color, 62, 4);
		break;
	case 3:
		ledStrip.fill(color, 66, 4);
		break;
	case 4:
		ledStrip.fill(color, 73, 4);
		break;
	case 5:
		ledStrip.fill(color, 51, 4);
		break;
	case 6:
		ledStrip.fill(color, 77, 4);
		break;
	case 7:
		ledStrip.fill(color, 88, 6);
		break;
	case 8:
		ledStrip.fill(color, 84, 4);
		break;
	case 9:
		ledStrip.fill(color, 44, 4);
		break;
	case 10:
		ledStrip.fill(color, 99, 4);
		break;
	case 11:
		ledStrip.fill(color, 103, 3);
		break;
	}

	// "Uhr"
	if (minute == 0)
	{
		ledStrip.fill(color, 107, 3);
	}

	ledStrip.show();
}

Timer clockTimer;
void clockTimerCallback()
{
	Config::Clock clock(config);
	auto dt = DateTime(SystemClock.now());

	using Tags = Config::Clock::Animation::Tag;
	switch (clock.animation.getTag())
	{
	case Tags::Static:
	{
		auto color = hexColorToInt(clock.animation.asStatic().getColor().c_str());
		return displayTime(dt, color);
	}
	default:
		return displayTime(dt, 0xff0000);
	}
}

void onSettings(HttpRequest &request, HttpResponse &response)
{
	Config::Settings settings(config); // Client for store needs to be initialized on demand to use the latest data

	if (request.method == HTTP_POST)
	{
		debugf("Updating settings");

		IDataSourceStream *stream = request.getBodyStream();
		if (stream == nullptr)
		{
			response.code = HTTP_STATUS_BAD_REQUEST;
			return;
		}

		auto updater = settings.update();
		ConfigDB::Status ok = updater.importFromStream(ConfigDB::Json::format, *stream);
		if (!ok)
		{
			response.code = HTTP_STATUS_BAD_REQUEST;
			return;
		}
	}

	auto stream = settings.createExportStream(ConfigDB::Json::format);
	response.sendDataStream(stream.release(), MIME_JSON);
}

void onUpdateColor(HttpRequest &request, HttpResponse &response)
{
	Config::Clock clock(config);
	auto updater = clock.update();
	if (!updater)
	{
		response.code = HTTP_STATUS_CONFLICT;
		return;
	}

	StaticJsonDocument<128> root;
	if (request.getBodyStream() == nullptr || !Json::deserialize(root, request.getBodyStream()) || !root.containsKey("color"))
	{
		response.code = HTTP_STATUS_BAD_REQUEST;
		return;
	}

	auto animation = updater.animation.asStatic();
	animation.setName("static");
	animation.setColor(root["color"]);
}

void onUpdateWifi(HttpRequest &request, HttpResponse &response)
{
	debugf("Updating wifi config");

	if (request.method != HTTP_POST || request.getBodyStream() == nullptr)
	{
		response.code = HTTP_STATUS_BAD_REQUEST;
		return;
	}

	StaticJsonDocument<256> config;
	if (!Json::deserialize(config, request.getBodyStream()))
	{
		debugf("Received invalid json while trying to update wifi config");
		return;
	}

	if (!config.containsKey("SSID") || !config.containsKey("PW"))
		return;

	String ssid = String(config["SSID"]);
	String password = String(config["PW"]);

	debugf("Updating wifi config to ssid %s", ssid);

	WifiStation.config(ssid, password);
	WifiStation.connect();
}

void onIndex(HttpRequest &request, HttpResponse &response)
{
	response.sendFile("index.html", false);
}

void onFile(HttpRequest &request, HttpResponse &response)
{
	String file = request.uri.getRelativePath();

	// Config files should not be cached
	if (!file.startsWith("config/"))
	{
		response.setCache(86400, true);
	}

	response.sendFile(file, false);
}

void startWebServer()
{
	server.listen(80);
	server.paths.set("/", onIndex);
	server.paths.set("/api/settings", onSettings);
	server.paths.set("/api/settings/color", onUpdateColor);
	server.paths.set("/api/settings/wifi", onUpdateWifi);
	server.paths.setDefault(onFile);
	server.setBodyParser(MIME_JSON, bodyToStringParser);

	Serial << endl
		   << _F("=== WEB SERVER STARTED ===") << endl
		   << WifiStation.getIP() << endl
		   << _F("==========================") << endl
		   << endl;
}

bool mountFileSystem()
{
	// Mount ifs read only file system with frontend
	auto part = Storage::findDefaultPartition(Storage::Partition::SubType::Data::fwfs);
	auto fs = IFS::createFirmwareFilesystem(part);
	if (fs == nullptr)
	{
		debugf("Failed to create filesystem");
		return false;
	}

	int res = fs->mount();
	if (res != FS_OK)
	{
		debugf("Failed to mount file system %s", fs->getErrorString(res).c_str());
		delete fs;
		return false;
	}

	fileSetFileSystem(fs);

	// Add second LFS based file system to store config in
	part = Storage::findDefaultPartition(Storage::Partition::SubType::Data::littlefs);
	auto lfs = IFS::createLfsFilesystem(part);
	if (lfs == nullptr)
	{
		debugf("Failed to create LFS filesystem");
		return false;
	}

	res = lfs->mount();
	if (res != FS_OK)
	{
		debugf("Failed to mount LFS file system %s", lfs->getErrorString(res).c_str());
		delete lfs;
		return false;
	}
	fs->setVolume(0, lfs);

	return true;
}

void onWifiConnect(IpAddress ip, IpAddress netmask, IpAddress gateway)
{
	timeKeeper.enableAutoQuery(true);

	if (WifiAccessPoint.isEnabled())
	{
		debugf("Shutting down own Wifi AP");
		WifiAccessPoint.enable(false);
	}
}

void onWifiDisconnect(const String &ssid, MacAddress mac, WifiDisconnectReason reason)
{
	timeKeeper.enableAutoQuery(false);

	if (!WifiAccessPoint.isEnabled())
	{
		debugf("Starting own Wifi AP");
		WifiAccessPoint.enable(true);
	}
}

void init()
{
	Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
	Serial.systemDebugOutput(true); // Enable debug output to serial

	mountFileSystem();

	WifiStation.enable(true);
	WifiAccessPoint.config("Wordclock", "", WifiAuthMode::AUTH_OPEN);
	WifiEvents.onStationGotIP(onWifiConnect);
	WifiEvents.onStationDisconnect(onWifiDisconnect);

	ledStrip.begin();
	ledStrip.clear();

	// Initialize all config files (will use defaults defined in config.cfgdb as fallback)
	Config::Clock clock(config);
	Config::Settings settings(config);
	clock.exportToFile(ConfigDB::Json::format, "config/clock.json");
	settings.exportToFile(ConfigDB::Json::format, "config/settings.json");

	timeKeeper.setTimeZone(settings.getTimezone());

	System.onReady(startWebServer);
	clockTimer.initializeMs(1000, clockTimerCallback).start();
}
