#include <SmingCore.h>
#include <Libraries/Adafruit_NeoPixel/Adafruit_NeoPixel.h>

#define LED_PIN 12

class Screen
{
public:
    Screen(int width, int height, Config *config);
    ~Screen();

    void setRenderFn(Delegate<uint32_t(uint8_t x, uint8_t y, uint8_t t)> fn);
    void setRefreshRate(uint8_t refreshRate);

private:
    Config *config;

    Adafruit_NeoPixel *ledStrip;
    Timer *refreshTimer;

    int width, height;
    uint8_t t;
    Delegate<uint32_t(uint8_t x, uint8_t y, uint8_t t)> renderFn;
    void updateScreen();
};

Screen::Screen(int width, int height, Config *config)
{
    this->config = config;

    this->width = width;
    this->height = height;

    int numLeds = width * height;
    this->ledStrip = new Adafruit_NeoPixel(numLeds, LED_PIN, NEO_GRB + NEO_KHZ800);
    this->ledStrip->begin();
    this->ledStrip->clear();

    this->refreshTimer = new Timer();
    this->refreshTimer->setCallback(std::bind(&Screen::updateScreen, this));
}

Screen::~Screen()
{
    this->refreshTimer->stop();
    delete this->refreshTimer;
    delete this->ledStrip;
}

void Screen::setRefreshRate(uint8_t refreshRate)
{
    this->refreshTimer->stop();
    if (refreshRate > 0)
    {
        this->refreshTimer->setIntervalMs(1000 / refreshRate);
        this->refreshTimer->start();
    }
}

void Screen::setRenderFn(Delegate<uint32_t(uint8_t x, uint8_t y, uint8_t t)> fn)
{
    this->renderFn = fn;
}

void Screen::updateScreen()
{
    if (this->renderFn == nullptr)
        return;

    Config::Settings generalSettings(*this->config);

    this->t++;
    for (int y = 0; y < this->height; y++)
    {
        int yOffset = this->width * y;
        for (int x = 0; x < this->width; x++)
        {
            uint32_t color = this->renderFn(x, y, t);
            ledStrip->setPixelColor(yOffset + x, color);
        }
    }

    uint8_t brightness = generalSettings.getMaxBrightness();

    // Night mode, automatically dim the brightness by the specified percentage
    auto currentTime = DateTime(SystemClock.now()).format("%H:%M");
    uint8_t dimmingAmount = generalSettings.nightMode.getDimmingAmount();
    if (dimmingAmount > 0 && currentTime >= generalSettings.nightMode.getStartTime() && currentTime < generalSettings.nightMode.getEndTime())
    {
        brightness = (uint8_t)(brightness * (dimmingAmount / 255.0f));
    }

    ledStrip->setBrightness(brightness);
    ledStrip->show();
}