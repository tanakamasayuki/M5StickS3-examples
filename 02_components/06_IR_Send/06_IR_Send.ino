#include <M5PM1.h>
#include <IRsend.h>

#define I2C_SDA_PIN GPIO_NUM_47 // M5StickS3 internal SDA Pin
#define I2C_SCL_PIN GPIO_NUM_48 // M5StickS3 internal SCL Pin
M5PM1 pm1;

const uint16_t kIrLed = 46;
IRsend irsend(kIrLed);

void setup()
{
    Serial.begin(115200);
    pinMode(kIrLed, OUTPUT);
    delay(1000);

    // IR Send needs 5V power
    pm1.begin(&Wire, M5PM1_DEFAULT_ADDR, I2C_SDA_PIN, I2C_SCL_PIN, M5PM1_I2C_FREQ_100K);
    pm1.setBoostEnable(true);
}

void loop()
{
    static uint16_t address = 0x0010;
    static uint8_t command = 0x00;
    address += 1;
    command += 2;
    uint64_t send = irsend.encodeNEC(address, command);
    irsend.sendNEC(send);
    Serial.printf("Sent NEC Address: 0x%04X Command: 0x%02X\n", address, command);

    delay(1000);
}
