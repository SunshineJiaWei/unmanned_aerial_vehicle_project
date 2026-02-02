#ifndef __COM_CONFIG_H__
#define __COM_CONFIG_H__

#include <stdint.h>


#define SAMPLING_DT      0.006f    // 时间周期

typedef enum
{
    REMOTE_STATE_CONNECTED,
    REMOTE_STATE_DISCONNECT
} remote_state_t;

typedef enum
{
    FLIGHT_STATE_IDLE,
    FLIGHT_STATE_NORMAL,
    FLIGHT_STATE_FIX_HIGH,
    FLIGHT_STATE_FAULT
} flight_state_t;

typedef enum
{
    THR_STATE_FREE,
    THE_STATE_MAX,
    THR_STATE_LEAVE_MAX,
    THR_STATE_MIN,
    THR_STATE_UNLOCK
} thr_state_t;

typedef struct 
{
    uint8_t fix_height;
    uint8_t shutdown;
    uint16_t throttle;
    uint16_t pitch;
    uint16_t roll;
    uint16_t yaw;
} remote_data_t;

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
    float roll;
    float pitch;
    float yaw;
} euler_angle_t;

typedef struct
{
    mpu6050_accel_t accel;
    mpu6050_gyro_t gyro;
} mpu6050_data_t;


#endif /* __COM_CONFIG_H */
