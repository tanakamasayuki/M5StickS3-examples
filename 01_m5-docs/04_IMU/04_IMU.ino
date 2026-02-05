#include <M5Unified.h>

void setup(void)
{
    M5.begin();
    M5.Power.setExtOutput(false);              // Add: this line to disable the external power output
    M5.Lcd.setFont(&fonts::FreeMonoBold9pt7b); // Fix: setTextFont -> setFont
}

void loop(void)
{
    auto imu_update = M5.Imu.update();
    if (imu_update)
    {
        M5.Lcd.setCursor(0, 5);
        M5.Lcd.clear();

        auto ImuData = M5.Imu.getImuData();

        // The ImuData obtained by getImuData can be used as follows.
        /* Add: Commented out
        ImuData.accel.x;     // accel x-axis value.
        ImuData.accel.y;     // accel y-axis value.
        ImuData.accel.z;     // accel z-axis value.
        ImuData.accel.value; // accel 3values array [0]=x / [1]=y / [2]=z.

        ImuData.gyro.x;     // gyro x-axis value.
        ImuData.gyro.y;     // gyro y-axis value.
        ImuData.gyro.z;     // gyro z-axis value.
        ImuData.gyro.value; // gyro 3values array [0]=x / [1]=y / [2]=z.
        */

        M5.Lcd.printf("IMU:\n\n");
        M5.Lcd.printf(" ax:%6.2f\n ay:%6.2f\n az:%6.2f\r\n", ImuData.accel.x, ImuData.accel.y, ImuData.accel.z);
        M5.Lcd.println();
        M5.Lcd.printf(" gx:%6.2f\n gy:%6.2f\n gz:%6.2f\r\n", ImuData.gyro.x, ImuData.gyro.y, ImuData.gyro.z);
    }
    delay(500);
}