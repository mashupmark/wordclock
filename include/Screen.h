#include <SmingCore.h>
#include <Libraries/Adafruit_NeoPixel/Adafruit_NeoPixel.h>

#define LED_PIN 12

class Screen
{
public:
    Screen(int width, int height, uint8_t refreshRate);
    ~Screen();

    void setRenderFn(Delegate<uint32_t(uint8_t x, uint8_t y, uint8_t t)> fn);

private:
    Adafruit_NeoPixel *ledStrip;
    Timer *refreshTimer;

    int width, height;
    uint8_t t;
    Delegate<uint32_t(uint8_t x, uint8_t y, uint8_t t)> renderFn;

    void updateScreen();
};

Screen::Screen(int width, int height, uint8_t refreshRate)
{
    this->width = width;
    this->height = height;

    int numLeds = width * height;
    this->ledStrip = new Adafruit_NeoPixel(numLeds, LED_PIN, NEO_GRB + NEO_KHZ800);
    this->ledStrip->begin();
    this->ledStrip->clear();

    this->refreshTimer = new Timer();
    this->refreshTimer->initializeMs(1000 / refreshRate, std::bind(&Screen::updateScreen, this)).start();
}

Screen::~Screen()
{
    this->refreshTimer->stop();
    delete this->refreshTimer;
    delete this->ledStrip;
}

void Screen::setRenderFn(Delegate<uint32_t(uint8_t x, uint8_t y, uint8_t t)> fn)
{
    this->renderFn = fn;
}

void Screen::updateScreen()
{
    if (this->renderFn == nullptr)
        return;

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

    ledStrip->setBrightness(255);
    ledStrip->show();
}