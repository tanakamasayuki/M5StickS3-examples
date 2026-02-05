#include "M5Unified.h"
#include "M5GFX.h"

// https://docs.m5stack.com/en/arduino/m5sticks3/button

static int32_t w;
static int32_t h;
static bool drawed = false;

void setup()
{
    auto cfg = M5.config();
    M5.begin(cfg);

    M5.Power.setExtOutput(false); // Add: this line to disable the external power output

    M5.Lcd.setRotation(1);
    w = M5.Lcd.width();
    h = M5.Lcd.height();
    M5.Lcd.setFont(&fonts::FreeMonoBold12pt7b);
    M5.Lcd.drawString("Button Released", 0, 0);
}

void loop()
{
    M5.update();
    if (M5.BtnA.isPressed() || M5.BtnB.isPressed())
    {
        if (!drawed)
        {
            M5.Lcd.clear();
        }
        M5.Lcd.drawString("Button  Detail:", 0, 0);
        if (M5.BtnA.isPressed())
        {
            M5.Lcd.drawString("ButtonA Pressed", 0, 30);
        }
        else if (M5.BtnB.isPressed())
        {
            M5.Lcd.drawString("ButtonB Pressed", 0, 60);
        }
        drawed = true;
    }
    else if (drawed)
    {
        drawed = false;
        M5.Lcd.clear();
        M5.Lcd.drawString("Button Released", 0, 0);
    }
    delay(1); // vTaskDelay -> delay
}