#include "M5Unified.h"

// https://docs.m5stack.com/en/arduino/m5sticks3/wakeup

void setup(void)
{
    M5.begin();

    M5.Power.setExtOutput(false); // Add: this line to disable the external power output

    M5.Display.setTextDatum(middle_center);
    M5.Display.setFont(&fonts::FreeMonoBoldOblique9pt7b); // Fix: setTextFont -> setFont
    M5.Display.setRotation(1);

    M5.Display.drawString("BtnA to Sleep 5s", M5.Display.width() / 2, M5.Display.height() / 2);
}

void loop(void)
{
    M5.update();

    if (M5.BtnA.wasPressed())
    {
        M5.Display.clear();
        M5.Power.timerSleep(5); // sec
    }
}