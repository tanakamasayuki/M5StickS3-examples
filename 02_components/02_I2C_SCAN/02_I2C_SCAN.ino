#include <Wire.h>

/*
Scanning for I2C devices ...
I2C device found at address 0x18
I2C device found at address 0x68
I2C device found at address 0x6E
*/

#define I2C_SDA_PIN GPIO_NUM_47 // M5StickS3 internal SDA Pin
#define I2C_SCL_PIN GPIO_NUM_48 // M5StickS3 internal SCL Pin

void setup()
{
    Serial.begin(115200);
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
}

void loop()
{
    byte error, address;
    int nDevices = 0;

    delay(5000);

    Serial.println("Scanning for I2C devices ...");
    for (address = 0x01; address < 0x7f; address++)
    {
        Wire.beginTransmission(address);
        error = Wire.endTransmission();
        if (error == 0)
        {
            Serial.printf("I2C device found at address 0x%02X\n", address);
            nDevices++;
        }
        else if (error != 2)
        {
            Serial.printf("Error %d at address 0x%02X\n", error, address);
        }
    }
    if (nDevices == 0)
    {
        Serial.println("No I2C devices found");
    }
}
