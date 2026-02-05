#include "M5Unified.h"

// https://docs.m5stack.com/en/arduino/m5sticks3/battery

void setup()
{
    M5.begin();
    Serial.begin(115200);

    M5.Power.setExtOutput(false); // Add: this line to disable the external power output

    M5.Lcd.setTextDatum(middle_center);
    M5.Lcd.setFont(&fonts::FreeMonoBold9pt7b); // Fix: setTextFont -> setFont
    M5.Lcd.setTextSize(1);
    M5.Lcd.setRotation(1);
}

void loop()
{
    M5.Lcd.clear();

    bool isCharging = M5.Power.isCharging();
    int vol_per = M5.Power.getBatteryLevel();
    int vol = M5.Power.getBatteryVoltage();

    M5.Lcd.setCursor(0, 30);
    M5.Lcd.printf("Charging: %s \n\n", isCharging ? "Yes" : "No");
    M5.Lcd.setCursor(0, 60);
    M5.Lcd.printf("Bat_level: %d%%", vol_per);
    M5.Lcd.setCursor(0, 90);
    M5.Lcd.printf("Bat_voltage: %d%mV", vol);
    delay(2000);
}