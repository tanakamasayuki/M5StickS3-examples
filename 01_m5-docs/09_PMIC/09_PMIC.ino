#include <M5Unified.h>
#include <M5PM1.h>
#include <Wire.h>

// https://docs.m5stack.com/en/arduino/m5sticks3/m5pm1

M5PM1 pm1;

void setup(void)
{
    M5.begin();

    M5.Power.setExtOutput(false); // Add: this line to disable the external power output

    M5.Display.setRotation(1);
    Serial.begin(115200);
    auto pin_num_sda = M5.getPin(m5::pin_name_t::in_i2c_sda);
    auto pin_num_scl = M5.getPin(m5::pin_name_t::in_i2c_scl);
    M5_LOGI("getPin: SDA:%u SCL:%u", pin_num_sda, pin_num_scl);
    Wire.end();
    Wire.begin(pin_num_sda, pin_num_scl, 100000U);

    // Initialize PM1
    m5pm1_err_t err = pm1.begin(&Wire, M5PM1_DEFAULT_ADDR, pin_num_sda, pin_num_scl, M5PM1_I2C_FREQ_100K);

    if (err == M5PM1_OK)
    {
        Serial.println("PM1 initialization successful");
    }
    else
    {
        Serial.printf("PM1 initialization failed, error code: %d\n", err);
    }
    M5.Display.fillScreen(BLACK);
    M5.Display.setTextSize(2);
    M5.Display.setTextColor(WHITE);
    M5.Display.setCursor(0, 10);
    M5.Display.println("Timer Power Test");
    M5.Display.println("BtnA: After 10s ON");
    M5.Display.println("BtnB: After 10s OFF");
}

void loop(void)
{
    M5.update();
    if (M5.BtnA.wasPressed())
    {
        M5.Display.fillScreen(BLACK);
        M5.Display.setCursor(0, 10);
        M5.Display.println("Shutdown");
        M5.Display.println("After 10s");
        M5.Display.println("Power ON");
        delay(1000);
        pm1.timerSet(10, M5PM1_TIM_ACTION_POWERON);
    }
    if (M5.BtnB.wasPressed())
    {
        M5.Display.fillScreen(BLACK);
        M5.Display.setCursor(0, 10);
        M5.Display.println("After 10s");
        M5.Display.println("Power OFF");
        delay(1000);
        pm1.timerSet(10, M5PM1_TIM_ACTION_POWEROFF);
    }
}