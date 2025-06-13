#include <SmingCore.h>
#include <config.h>
#include <utils.h>

class Wordclock
{
public:
    Wordclock(int width, int height, Config *config);

    uint32_t getColor(uint8_t x, uint8_t y);

private:
    int width, height;
    Config *config;

    bool maskTime(DateTime dt, uint8_t x, uint8_t y);
};

Wordclock::Wordclock(int width, int height, Config *config)
{
    this->width = width;
    this->height = height;
    this->config = config;
}

uint32_t Wordclock::getColor(uint8_t x, uint8_t y)
{
    auto dt = DateTime(SystemClock.now());
    bool enablePixel = maskTime(dt, x, y);
    if (!enablePixel)
        return 0;

    Config::Clock clockSettings(*this->config);

    using Tags = Config::Clock::Animation::Tag;
    switch (clockSettings.animation.getTag())
    {
    case Tags::Static:
    {
        auto color = hexColorToInt(clockSettings.animation.asStatic().getColor().c_str());
        return color;
    }
    default:
        return 0xff0000;
    }
}

/*
    Check if the led for the given coordinates should be light at the given time

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
bool Wordclock::maskTime(DateTime dt, uint8_t x, uint8_t y)
{
    if (y == 0 && x < 2) // "Es"
        return true;

    if (y == 0 && x >= 3 && x < 6) // "Ist"
        return true;

    int minute = (dt.Minute / 5) * 5; // round down to the next multiple of 5

    if (y == 0 && x >= 7 && (minute == 5 || minute == 25 || minute == 35 || minute == 55)) // "Fünf"
        return true;

    if (y == 1 && x < 4 && (minute == 10 || minute == 20 || minute == 40 || minute == 50)) // "Zehn"
        return true;

    if (y == 1 && x >= 7 && (minute == 5 || minute == 10 || minute == 35 || minute == 40)) // "Nach"
        return true;

    if (y == 2 && x >= 3 && x < 10 && (minute == 15 || minute == 45)) // "Viertel"
        return true;

    if (y == 3 && x >= 1 && x < 4 && (minute == 20 || minute == 25 || minute == 45 || minute == 50 || minute == 55)) // "Vor"
        return true;

    if (y == 3 && x >= 7 && (minute >= 20 && minute < 45)) // "Halb"
        return true;

    // Unless the time is shown as "nach {hour}" it should be relative to the next full hour e.g. "viertel eins"
    int hour = (minute > 10 ? dt.Hour + 1 : dt.Hour) % 12;

    if (y == 4 && x < 4 && hour == 9) // "Neun"
        return true;

    if (y == 4 && x >= 7 && hour == 5) // "Fünf"
        return true;

    if (y == 5 && x < 4 && hour == 1) // "Eins"
        return true;

    if (y == 5 && x >= 7 && hour == 2) // "Zwei"
        return true;

    if (y == 6 && x < 4 && hour == 3) // "Drei"
        return true;

    if (y == 6 && x >= 7 && hour == 4) // "Vier"
        return true;

    if (y == 7 && x < 5 && hour == 6) // "Sechs"
        return true;

    if (y == 7 && x >= 7 && hour == 8) // "Acht"
        return true;

    if (y == 8 && x < 6 && hour == 7) // "Sieben"
        return true;

    if (y == 8 && x >= 6 && hour == 0) // "Zwölf"
        return true;

    if (y == 9 && x < 4 && hour == 10) // "Zehn"
        return true;

    if (y == 9 && x >= 4 && x < 7 && hour == 11) // "Elf"
        return true;

    if (y == 9 && x >= 8) // "Uhr"
        return true;

    return false;
}
