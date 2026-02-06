#include <M5Unified.h>
#include <IRrecv.h>
#include <IRutils.h>

const uint16_t kRecvPin = 42;
const uint16_t kCaptureBufferSize = 1024;
const uint8_t kTimeout = 50;
IRrecv irrecv(kRecvPin, kCaptureBufferSize, kTimeout, true);

void setup()
{
    auto cfg = M5.config();
    cfg.internal_spk = false; // Disable speaker amp to avoid IR RX interference
    M5.begin(cfg);

    Serial.begin(115200);

    // Display initialization
    M5.Display.setRotation(3);
    M5.Display.setFont(&fonts::FreeMonoBold9pt7b);
    M5.Display.clear();
    M5.Display.println("StickS3 IR example");

    // Enable external power output for IR receiver module
    M5.Power.setExtOutput(true, m5::ext_none);

    irrecv.enableIRIn(true); // pullup enabled
}

void loop()
{
    M5.update();

    decode_results results;
    if (irrecv.decode(&results))
    {
        Serial.print(resultToHumanReadableBasic(&results));
        Serial.printf("Type=%s  Bits=%d  Addr=0x%04lX (%ld)  Cmd=0x%02lX (%ld)  Repeat=%d\n",
                      typeToString(results.decode_type).c_str(),
                      results.bits,
                      results.address, results.address,
                      results.command, results.command,
                      results.repeat);
        Serial.println(resultToSourceCode(&results));
        Serial.println();

        M5.Display.setCursor(0, 30);
        M5.Display.printf("Type    =%s\n", typeToString(results.decode_type).c_str());
        M5.Display.printf("Bits    =%d\n", results.bits);
        M5.Display.printf("Address =0x%04lX (%ld)\n", results.address, results.address);
        M5.Display.printf("Command =0x%02lX   (%ld)\n", results.command, results.command);
        M5.Display.printf("Repeat  =%d\n", results.repeat);
    }
}