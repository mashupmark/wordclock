#include <SmingCore.h>
#include <JsonObjectStream.h>
#include <Libraries/Adafruit_NeoPixel/Adafruit_NeoPixel.h>

NtpClient ntpClient("pool.ntp.org", 60 * 60 * 1000); // Update system clock every hour
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
	auto dt = DateTime(SystemClock.now());
	if (dt.Minute % 5 != 0 && dt.Second < 1) // Save some performance as this only needs to run for the first second of every 5th minute
		return;

	displayTime(dt, 0xff0000);
}

void onIndex(HttpRequest &request, HttpResponse &response)
{
	response.sendFile("index.html", false);
}

void onFile(HttpRequest &request, HttpResponse &response)
{
	String file = request.uri.getRelativePath();

	response.setCache(86400, true);
	response.sendFile(file, false);
}

void onUpdateWifi(HttpRequest &request, HttpResponse &response)
{
	debugf("Trying to update wifi config");

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

void startWebServer()
{
	server.listen(80);
	server.paths.set("/", onIndex);
	server.paths.set("/api/wifi", onUpdateWifi);
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
	return true;
}

void onWifiConnect(IpAddress ip, IpAddress netmask, IpAddress gateway)
{
	if (WifiAccessPoint.isEnabled())
	{
		debugf("Shutting down own Wifi AP");
		WifiAccessPoint.enable(false);
	}
}

void onWifiDisconnect(const String &ssid, MacAddress mac, WifiDisconnectReason reason)
{
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

	System.onReady(startWebServer);

	SystemClock.setTimeZone(1); // ToDo: Use dynamic timezone instead of hardcoded offset (see: https://github.com/SmingHub/Sming/blob/5.2.0/samples/SystemClock_NTP/app/NtpClientDemo.cpp)
	clockTimer.initializeMs(1000, clockTimerCallback).start();
}
