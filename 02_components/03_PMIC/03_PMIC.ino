#include <M5PM1.h>

/*
--- PM1 Information ---
Device ID     : 0x50
Device Model  : 0x20
HW Version    : 0x05
SW Version    : 0x4F
Power default : CHG=ON  DCDC=ON  LDO=ON  5V=OFF
Power CHG off : CHG=OFF DCDC=ON  LDO=ON  5V=OFF
Power 5V off  : CHG=OFF DCDC=ON  LDO=ON  5V=ON
Power default : CHG=ON  DCDC=ON  LDO=ON  5V=OFF
Vref          : 3340 mV (3.340 V)
VBAT          : 4104 mV (4.104 V)
VIN           : 5032 mV (5.032 V)
5V IN/OUT     : 5012 mV (5.012 V)
Temp          : 37
*/

#define I2C_SDA_PIN GPIO_NUM_47 // M5StickS3 internal SDA Pin
#define I2C_SCL_PIN GPIO_NUM_48 // M5StickS3 internal SCL Pin

M5PM1 pm1;

void printPowerConfig()
{
    uint8_t cfg = 0;
    if (pm1.getPowerConfig(&cfg) != M5PM1_OK)
    {
        Serial.printf("Get power config failed");
        return;
    }
    Serial.printf("CHG=%s DCDC=%s LDO=%s 5V=%s\n",
                  (cfg & (1 << 0)) ? "ON " : "OFF",
                  (cfg & (1 << 1)) ? "ON " : "OFF",
                  (cfg & (1 << 2)) ? "ON " : "OFF",
                  (cfg & (1 << 3)) ? "ON " : "OFF");
}

void setup()
{
    Serial.begin(115200);
    delay(1000);

    m5pm1_err_t err = pm1.begin(&Wire, M5PM1_DEFAULT_ADDR, I2C_SDA_PIN, I2C_SCL_PIN, M5PM1_I2C_FREQ_100K);
    if (err == M5PM1_OK)
    {
        Serial.println("PM1 initialization successful");
    }
    else
    {
        Serial.printf("PM1 initialization failed, error code: %d\n", err);
    }
}

void loop()
{
    Serial.printf("\n--- PM1 Information ---\n");

    uint8_t id = 0;
    if (pm1.getDeviceId(&id) == M5PM1_OK)
    {
        Serial.printf("Device ID     : 0x%02X\n", id);
    }

    uint8_t model = 0;
    if (pm1.getDeviceModel(&model) == M5PM1_OK)
    {
        Serial.printf("Device Model  : 0x%02X\n", model);
    }

    uint8_t hw = 0;
    if (pm1.getHwVersion(&hw) == M5PM1_OK)
    {
        Serial.printf("HW Version    : 0x%02X\n", hw);
    }

    uint8_t sw = 0;
    if (pm1.getSwVersion(&sw) == M5PM1_OK)
    {
        Serial.printf("SW Version    : 0x%02X\n", sw);
    }

    // Power configuration
    Serial.printf("Power default : ");
    printPowerConfig();

    pm1.setChargeEnable(false);
    Serial.printf("Power CHG off : ");
    printPowerConfig();

    // When using ESP32, always keep DCDC and LDO ON
    // pm1.setDCDCEnable(false);
    // pm1.setLDOEnable(false);

    pm1.setBoostEnable(true);
    Serial.printf("Power 5V off  : ");
    printPowerConfig();
    delay(1000);

    Serial.printf("Power default : ");
    pm1.setChargeEnable(true);
    pm1.setBoostEnable(false);
    printPowerConfig();

    // Voltages
    uint16_t mv = 0;
    if (pm1.readVref(&mv) == M5PM1_OK)
    {
        Serial.printf("Vref          : %u mV (%.3f V)\n", mv, mv / 1000.0f);
    }
    if (pm1.readVbat(&mv) == M5PM1_OK)
    {
        Serial.printf("VBAT          : %u mV (%.3f V)\n", mv, mv / 1000.0f);
    }
    if (pm1.readVin(&mv) == M5PM1_OK)
    {
        Serial.printf("VIN           : %u mV (%.3f V)\n", mv, mv / 1000.0f);
    }
    if (pm1.read5VInOut(&mv) == M5PM1_OK)
    {
        Serial.printf("5V IN/OUT     : %u mV (%.3f V)\n", mv, mv / 1000.0f);
    }

    // Temperature
    uint16_t temp = 0;
    if (pm1.readTemperature(&temp) == M5PM1_OK)
    {
        Serial.printf("Temp          : %u\n", temp);
    }

    delay(5000);
}
