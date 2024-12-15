#include <SmingCore.h>
#include <JsonObjectStream.h>
#include <Libraries/Adafruit_NeoPixel/Adafruit_NeoPixel.h>

// Default WiFi credentials, please provide the actual ones via the CLI when building the final binary e.g.: `make WIFI_SSID=test WIFI_PWD=1234`
#ifndef WIFI_SSID
#define WIFI_SSID "PleaseEnterSSID"
#define WIFI_PWD "PleaseEnterPass"
#endif

HttpServer server;

#define LED_PIN 12
#define NUM_LEDS 110
Adafruit_NeoPixel ledStrip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
Timer ledTimer;

uint8_t currentLedIndex = 0;
void shiftLed()
{
	ledStrip.clear();

	ledStrip.setPixelColor(currentLedIndex, 0x00ff00);
	currentLedIndex++;
	if (currentLedIndex >= NUM_LEDS)
		currentLedIndex = 0;

	ledStrip.show();
}

void onIndex(HttpRequest &request, HttpResponse &response)
{
	TemplateFileStream *tmpl = new TemplateFileStream("index.html");
	response.sendNamedStream(tmpl); // this template object will be deleted automatically
}

void onFile(HttpRequest &request, HttpResponse &response)
{
	String file = request.uri.getRelativePath();

	if (file[0] == '.')
		response.code = HTTP_STATUS_FORBIDDEN;
	else
	{
		response.setCache(86400, true); // It's important to use cache for better performance.
		response.sendFile(file);
	}
}

void onPing(HttpRequest &request, HttpResponse &response)
{
	JsonObjectStream *stream = new JsonObjectStream();
	JsonObject json = stream->getRoot();

	JsonObject data = json.createNestedObject("data");
	data[F("message")] = F("pong");

	response.sendDataStream(stream, MIME_JSON);
}

void startWebServer()
{
	server.listen(80);
	server.paths.set("/", onIndex);
	server.paths.set("/api/ping", onPing);
	server.paths.setDefault(onFile);

	Serial << endl
		   << _F("=== WEB SERVER STARTED ===") << endl
		   << WifiStation.getIP() << endl
		   << _F("==========================") << endl
		   << endl;
}

void gotIP(IpAddress ip, IpAddress netmask, IpAddress gateway)
{
	startWebServer();
}

void init()
{
	spiffs_mount(); // Mount file system, in order to work with files

	Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
	Serial.systemDebugOutput(true); // Enable debug output to serial

	WifiStation.enable(true);
	WifiStation.config(WIFI_SSID, WIFI_PWD);
	WifiAccessPoint.enable(false);

	// Run our method when station was connected to AP
	WifiEvents.onStationGotIP(gotIP);

	ledStrip.begin();
	ledTimer.initializeMs(500, shiftLed).start();
}
