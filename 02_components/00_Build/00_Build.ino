#include <Arduino.h>
#include <EspHelperBoardSettings.h>

/*
=== ESP32 Arduino Board Settings ===
USB CDC On Boot                         : Enabled
CPU Frequency                           : 240MHz (WiFi)
Core Debug Level                        : Debug
USB DFU On Boot                         : Disabled
Events Run On                           : Core 1
Flash Mode                              : QIO 80MHz
Flash Size                              : 8MB (64Mb)
Arduino Runs On                         : Core 1
USB Firmware MSC On Boot                : Disabled
PSRAM                                   : Enabled (OPI)
Partition Scheme                        : 8MB (APP 3.2MB x2, SPIFFS 1.5MB, NVS 20KB, OTADATA 8KB)
USB Mode                                : Hardware CDC and JTAG
=== end ===
*/

void setup()
{
    Serial.begin(115200);
    delay(1000);
    EspHelperBoardSettings::dump();
}

void loop()
{
    delay(1);
}