#include "int_motor.h"

/**
 * @brief 设置电机转速
 * 通过改变占空比来实现修改转速
 * 
 * @param motor 
 */
void int_motor_set_speed(motor_t *motor)
{
    __HAL_TIM_SET_COMPARE(motor->tim, motor->channel, motor->speed);
}

/**
 * @brief 启动电机，传入电机结构体
 * 
 * @param motor 
 */
void int_motor_start(motor_t *motor)
{
    if (motor->speed > 1000)
    {
        DEBUG_PRINTF("beyond the maximum speed range");
        return;
    }
    HAL_TIM_PWM_Start(motor->tim, motor->channel);
}

