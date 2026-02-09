#include "M5Unified.h"

// https://docs.m5stack.com/en/arduino/m5sticks3/display

void draw_function(LovyanGFX *gfx)
{
    int x = rand() % gfx->width();
    int y = rand() % gfx->height();
    int r = (gfx->width() >> 4) + 2;
    uint16_t c = rand();
    gfx->fillRect(x - r, y - r, r * 2, r * 2, c);
}

void setup()
{
    auto cfg = M5.config();
    M5.begin(cfg);

    M5.Power.setExtOutput(false); // Add: this line to disable the external power output

    // Delete: setTextSize

    M5.Display.clear(TFT_WHITE);
}

void loop()
{
    int x = rand() % M5.Display.width();
    int y = rand() % M5.Display.height();
    int r = (M5.Display.width() >> 4) + 2;
    uint16_t c = rand();
    M5.Display.fillCircle(x, y, r, c);
    draw_function(&M5.Display);
    delay(100); // 1000 -> 100
}