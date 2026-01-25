#ifndef __INT_JOYSTICK_H
#define __INT_JOYSTICK_H


#include "adc.h"


#define ADC1_CHANNEL_NUM 4


typedef struct
{
    int16_t throttle;   // 油门
    int16_t yaw;        // 顺逆时针旋转
    int16_t pitch;      // 前后
    int16_t roll;       // 左右
} joystick_t;
 

void int_joystick_init(void);
void int_joystick_get(joystick_t *joystick);

#endif /* __INT_JOYSTICK_H */