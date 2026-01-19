#ifndef __INT_MOTOR_H
#define __INT_MOTOR_H

#include "tim.h"
#include "com_debug.h"

typedef struct 
{
    TIM_HandleTypeDef *tim;
    uint16_t channel;
    uint16_t speed;
} motor_t;



void int_motor_set_speed(motor_t *motor);
void int_motor_start(motor_t *motor);


#endif /* __INT_MOTOR_H */

