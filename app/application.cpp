#include <SmingCore.h>
#include <LittleFS.h>
#include <JsonObjectStream.h>
#include <Libraries/Adafruit_NeoPixel/Adafruit_NeoPixel.h>
#include <config.h>
#include <ConfigDB/Json/Format.h>
#include <TimeKeeper.h>
#include <Screen.h>
#include <Wordclock.h>

Config config("config"); // Path needs to match mount point defined in fsimage.fwfs

void onSettings(HttpRequest &request, HttpResponse &response)
{
	Config::Settings generalSettings(config); // Client for store needs to be initialized on demand to use the latest data

	if (request.method == HTTP_POST)
	{
		IDataSourceStream *requestBodyStream = request.getBodyStream();
		if (requestBodyStream == nullptr)
		{
			response.code = HTTP_STATUS_BAD_REQUEST;
			return;
		}

		auto updater = generalSettings.update();
		ConfigDB::Status ok = updater.importFromStream(ConfigDB::Json::format, *requestBodyStream);
		if (!ok)
		{
			response.code = HTTP_STATUS_BAD_REQUEST;
			return;
		}
	}

	auto exportStream = generalSettings.createExportStream(ConfigDB::Json::format);
	response.sendDataStream(exportStream.release(), MIME_JSON);
}

void onClock(HttpRequest &request, HttpResponse &response)
{
	Config::Clock clockSettings(config);

	if (request.method == HTTP_POST)
	{
		IDataSourceStream *requestBodyStream = request.getBodyStream();
		if (requestBodyStream == nullptr)
		{
			response.code = HTTP_STATUS_BAD_REQUEST;
			return;
		}

		auto updater = clockSettings.update();
		ConfigDB::Status ok = clockSettings.importFromStream(ConfigDB::Json::format, *requestBodyStream);
		if (!ok)
		{
			response.code = HTTP_STATUS_BAD_REQUEST;
			return;
		}
	}

	auto exportStream = clockSettings.createExportStream(ConfigDB::Json::format);
	response.sendDataStream(exportStream.release(), MIME_JSON);
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

TimeKeeper timeKeeper("pool.ntp.org", 60 * 60); // Update time every hour
HttpServer server;

void startWebServer()
{
	server.listen(80);
	server.paths.set("/", onIndex);
	server.paths.set("/api/clock", onClock);
	server.paths.set("/api/settings", onSettings);
	server.paths.set("/api/settings/wifi", onUpdateWifi);
	server.paths.setDefault(onFile);
	server.setBodyParser(MIME_JSON, bodyToStringParser);

	Serial << endl
		   << _F("=== WEB SERVER STARTED ===") << endl
		   << WifiStation.getIP() << endl
		   << _F("==========================") << endl
		   << endl;
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

Screen screen(11, 10, 1);
Wordclock wordclock(11, 10, &config);

void init()
{
	Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
	Serial.systemDebugOutput(true); // Enable debug output to serial

	mountFileSystem();

	// Initialize all config files (will use defaults defined in config.cfgdb as fallback)
	Config::Clock clockSettings(config);
	Config::Settings generalSettings(config);
	clockSettings.exportToFile(ConfigDB::Json::format, "config/clock.json");
	generalSettings.exportToFile(ConfigDB::Json::format, "config/settings.json");

	timeKeeper.setTimeZone(generalSettings.getTimezone());
	screen.setRenderFn([](uint8_t x, uint8_t y, uint8_t t) -> uint32_t
					   { return wordclock.getColor(x, y); });

	WifiStation.enable(true);
	WifiAccessPoint.config("Wordclock", "", WifiAuthMode::AUTH_OPEN);
	WifiEvents.onStationGotIP(onWifiConnect);
	WifiEvents.onStationDisconnect(onWifiDisconnect);
	System.onReady(startWebServer);
}
