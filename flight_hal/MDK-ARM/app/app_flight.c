#include "app_flight.h"


extern euler_angle_t euler_angle;
static mpu6050_data_t mpu_data;
static mpu6050_gyro_t last_gyro;
static float gyro_z_sum;


void app_flight_init(void)
{
    int_mpu6050_init();
}

void app_flight_get_euler_angle(void)
{
    int_mpu6050_get_data(&mpu_data);

    // 对角速度进行一阶低通滤波，准确性要求不好，实时性要求高
    mpu_data.gyro.gyro_x = Common_Filter_LowPass(mpu_data.gyro.gyro_x, last_gyro.gyro_x);
    mpu_data.gyro.gyro_y = Common_Filter_LowPass(mpu_data.gyro.gyro_y, last_gyro.gyro_y);
    mpu_data.gyro.gyro_z = Common_Filter_LowPass(mpu_data.gyro.gyro_z, last_gyro.gyro_z);
    last_gyro = mpu_data.gyro;
    // DEBUG_PRINTF(":%d, %d, %d\n", mpu_data.gyro.gyro_x, mpu_data.gyro.gyro_y, mpu_data.gyro.gyro_z);

    // 对加速度计进行卡尔曼滤波
    mpu_data.accel.accel_x = Common_Filter_KalmanFilter(&kfs[0], mpu_data.accel.accel_x);
    mpu_data.accel.accel_y = Common_Filter_KalmanFilter(&kfs[1], mpu_data.accel.accel_y);
    mpu_data.accel.accel_z = Common_Filter_KalmanFilter(&kfs[2], mpu_data.accel.accel_z);
    // DEBUG_PRINTF(":%d, %d, %d\n", mpu_data.accel.accel_x, mpu_data.accel.accel_y, mpu_data.accel.accel_z);

    // 互补算法计算欧拉角，俯仰角和横滚角使用加速度计，偏航角使用陀螺仪
    euler_angle.pitch = atan2(mpu_data.accel.accel_x, mpu_data.accel.accel_z) * 180 / 3.141592;
    euler_angle.roll = atan2(mpu_data.accel.accel_y, mpu_data.accel.accel_z) * 180 / 3.141592;
    gyro_z_sum += (mpu_data.gyro.gyro_z * 2000.0 / 32768.0) * 0.006;
    euler_angle.yaw = gyro_z_sum;
    DEBUG_PRINTF(":%d,%d,%d\n", euler_angle.pitch, euler_angle.roll, euler_angle.yaw);
}