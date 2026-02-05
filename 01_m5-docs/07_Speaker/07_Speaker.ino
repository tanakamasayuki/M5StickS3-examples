#include "M5Unified.h"

void setup()
{
    M5.begin();
    M5.Lcd.setRotation(1);
    M5.Lcd.setTextDatum(middle_center);
    M5.Lcd.setFont(&fonts::FreeMonoBold9pt7b); // Fix: setTextFont -> setFont
    M5.Lcd.clear();
    M5.Lcd.drawString("Speaker", M5.Lcd.width() / 2, M5.Lcd.height() / 2);
    delay(100);
}

void loop()
{
    M5.Speaker.tone(7000, 100); // frequency, duration
    delay(1000);
    M5.Speaker.tone(4000, 200); // frequency, duration
    delay(1000);
}