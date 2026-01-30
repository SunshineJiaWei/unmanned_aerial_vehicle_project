#include "app_flight.h"


extern euler_angle_t euler_angle;
static mpu6050_data_t mpu_data;

void app_flight_init(void)
{
    int_mpu6050_init();
}

void app_flight_get_euler_angle(void)
{
    int_mpu6050_get_data(&mpu_data);

    // 角速度
    // DEBUG_PRINTF(":%d, %d, %d\n", mpu_data.gyro.gyro_x, mpu_data.gyro.gyro_y, mpu_data.gyro.gyro_z);

    // 加速度计
    DEBUG_PRINTF(":%d, %d, %d\n", mpu_data.accel.accel_x, mpu_data.accel.accel_y, mpu_data.accel.accel_z);
}