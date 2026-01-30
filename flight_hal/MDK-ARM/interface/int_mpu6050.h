#ifndef __INT_MPU6050_H
#define __INT_MPU6050_H


#include <stdlib.h>

#include "i2c.h"
#include "FreeRTOS.h"
#include "task.h"

#define MPU6050_SLAVE_ADDR              0x68
#define MPU6050_ADDR_WRITE              0xD0
#define MPU6050_ADDR_READ               0xD1

#define MPU_POWER_MANAGEMENT_1          0x6B
#define MPU_POWER_MANAGEMENT_2          0x6C
#define MPU_GYROSCOPE_CONFIG            0x1B
#define MPU_ACCELEROMETER_CONFIG        0x1C
#define MPU_INTERRUPT_ENABLE            0x38
#define MPU_USER_CONTROL                0X6A
#define MPU_FIFO_ENABLE                 0X23
#define MPU_SAMPLE_RATE_DIVIDER         0x19
#define MPU_CONFIGURATION               0x1A

#define MPU_ACCEL_XOUT_H                0x3B
#define MPU_ACCEL_XOUT_L                0x3C
#define MPU_ACCEL_YOUT_H                0x3D
#define MPU_ACCEL_YOUT_L                0x3E
#define MPU_ACCEL_ZOUT_H                0x3F
#define MPU_ACCEL_ZOUT_L                0x40
#define MPU_GYRO_XOUT_H                 0x43
#define MPU_GYRO_XOUT_L                 0x44
#define MPU_GYRO_YOUT_H                 0x45
#define MPU_GYRO_YOUT_L                 0x46
#define MPU_GYRO_ZOUT_H                 0x47
#define MPU_GYRO_ZOUT_L                 0x48


typedef struct
{
    int16_t accel_x;    // 向前的加速度为正
    int16_t accel_y;    // 向左的加速度为正
    int16_t accel_z;    // 向上的加速度为正
} mpu6050_accel_t;

typedef struct
{
    int16_t gyro_x;     // 横滚角roll：向右是正
    int16_t gyro_y;     // 俯仰角pitch：向前是正
    int16_t gyro_z;     // 偏航角yaw：向左（逆时针转）是正
} mpu6050_gyro_t;

typedef struct
{
    int16_t roll;
    int16_t pitch;
    int16_t yaw;
} euler_angle_t;

typedef struct
{
    mpu6050_accel_t accel;
    mpu6050_gyro_t gyro;
} mpu6050_data_t;


void int_mpu6050_init(void);

void int_mpu6050_get_accel(mpu6050_accel_t *accel);
void int_mpu6050_get_gyro(mpu6050_gyro_t *gyro);
void int_mpu6050_get_data(mpu6050_data_t *data);


#endif /* __INT_MPU6050_H */

