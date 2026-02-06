#include <M5PM1.h>
#include <IRrecv.h>
#include <IRutils.h>

#define I2C_SDA_PIN GPIO_NUM_47 // M5StickS3 internal SDA Pin
#define I2C_SCL_PIN GPIO_NUM_48 // M5StickS3 internal SCL Pin
M5PM1 pm1;

const uint16_t kRecvPin = 42;
const uint16_t kCaptureBufferSize = 1024;
const uint8_t kTimeout = 50;
IRrecv irrecv(kRecvPin, kCaptureBufferSize, kTimeout, true);

void setup()
{
    Serial.begin(115200);
    delay(1000);

    // IR Receiver needs 5V power & Speaker off
    pm1.begin(&Wire, M5PM1_DEFAULT_ADDR, I2C_SDA_PIN, I2C_SCL_PIN, M5PM1_I2C_FREQ_100K);
    pm1.setBoostEnable(true);
    pm1.pinMode(3, OUTPUT);
    pm1.digitalWrite(3, LOW);
    pm1.dumpPinStatus();

    irrecv.enableIRIn(true); // pullup enabled
}

void loop()
{
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
    }
}
