#include <M5Unified.h>
#include <M5PM1.h>
#include <Wire.h>
#include "SparkFun_BMI270_Arduino_Library.h"
#include "driver/rtc_io.h"

// https://docs.m5stack.com/en/arduino/m5sticks3/m5pm1

BMI270 imu;
M5PM1 pm1;

void setup(void)
{
    M5.begin();
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
        pm1.irqClearGpioAll();
        pm1.irqClearSysAll();
        pm1.irqClearBtnAll();

        pm1.irqSetGpioMaskAll(M5PM1_IRQ_MASK_ENABLE);
        pm1.irqSetSysMaskAll(M5PM1_IRQ_MASK_ENABLE);
        pm1.irqSetBtnMaskAll(M5PM1_IRQ_MASK_ENABLE);

        pm1.irqSetGpioMask(M5PM1_IRQ_GPIO4, M5PM1_IRQ_MASK_DISABLE);
        pm1.gpioSetMode(M5PM1_GPIO_NUM_4, M5PM1_GPIO_MODE_INPUT);
        pm1.gpioSetPull(M5PM1_GPIO_NUM_4, M5PM1_GPIO_PULL_UP);

        pm1.gpioSetMode(M5PM1_GPIO_NUM_1, M5PM1_GPIO_MODE_OUTPUT);
        pm1.gpioSetDrive(M5PM1_GPIO_NUM_1, M5PM1_GPIO_DRIVE_PUSHPULL);
        pm1.gpioSetFunc(M5PM1_GPIO_NUM_1, M5PM1_GPIO_FUNC_IRQ);
    }
    else
    {
        Serial.printf("PM1 initialization failed, error code: %d\n", err);
    }

    // Check if sensor is connected and initialize
    // Address is optional (defaults to 0x68)
    while (imu.beginI2C(BMI2_I2C_PRIM_ADDR) != BMI2_OK)
    {
        Serial.println("Error: BMI270 not connected, check wiring and I2C address!");
        delay(1000);
    }
    imu.disableFeature(BMI2_ANY_MOTION);
    Serial.println("BMI270 initialization successful");

    M5.Display.setFont(&fonts::FreeMonoBold9pt7b);
    M5.Display.fillScreen(BLACK);
    M5.Display.setTextColor(WHITE);
    M5.Display.setCursor(0, 10);
    M5.Display.println("IMU IRQ Test");
    M5.Display.println("Press BtnA to Sleep");
    M5.Display.println("Shake to wake up");
}

void ARDUINO_ISR_ATTR pm1_irq_handler()
{
    Serial.println("PM1 IRQ triggered");
    M5.Display.println("PM1 IRQ triggered");
}

void loop(void)
{
    M5.update();
    if (M5.BtnA.wasPressed())
    {
        pm1.irqClearGpioAll();
        pm1.irqClearSysAll();
        pm1.irqClearBtnAll();
        int8_t ret = imu.enableFeature(BMI2_ANY_MOTION);
        // Optional
        // bmi2_sens_config config;
        // config.type = BMI2_ANY_MOTION;
        // config.cfg.any_motion.threshold = 0xE0;// 1LSB equals to 0.48mg. Default is 83mg. Lower is more sensitive
        // config.cfg.any_motion.duration = 0x0A; // 1LSB equals 20ms. Default is 100ms.
        // imu.setConfig(config);
        //
        bmi2_int_pin_config intPinConfig;
        intPinConfig.pin_type = BMI2_INT1;
        intPinConfig.int_latch = BMI2_INT_NON_LATCH;
        intPinConfig.pin_cfg[0].lvl = BMI2_INT_ACTIVE_LOW;
        intPinConfig.pin_cfg[0].od = BMI2_INT_PUSH_PULL;
        intPinConfig.pin_cfg[0].output_en = BMI2_INT_OUTPUT_ENABLE;
        intPinConfig.pin_cfg[0].input_en = BMI2_INT_INPUT_DISABLE;
        ret |= imu.setInterruptPinConfig(intPinConfig);
        ret |= imu.mapInterruptToPin(BMI2_ANY_MOTION_INT, BMI2_INT1);
        if (!ret)
        {
            Serial.println("BMI270 AnyMotionInterrupt enabled successfully");
        }
        else
        {
            Serial.println("Failed to enable BMI270 AnyMotionInterrupt");
        }

        M5.Display.fillScreen(BLACK);
        M5.Display.setCursor(0, 10);
        M5.Display.println("Now Shake!");

        // Choose either of the two pieces of code below.
        pinMode(GPIO_NUM_13, INPUT_PULLUP);
        attachInterrupt(GPIO_NUM_13, pm1_irq_handler, FALLING);

        // esp_sleep_enable_ext0_wakeup(GPIO_NUM_13, 0);  // 0 = Low
        // rtc_gpio_pullup_en(GPIO_NUM_13);
        // Serial.println("Going to sleep now");
        // esp_deep_sleep_start();
    }
}