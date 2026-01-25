#include "int_joystick.h"

uint16_t adc_buf[ADC1_CHANNEL_NUM] = {0};

void int_joystick_init(void)
{
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_buf, ADC1_CHANNEL_NUM);
}

void int_joystick_get(joystick_t *joystick)
{
    joystick->throttle = adc_buf[0];
    joystick->yaw = adc_buf[1];
    joystick->pitch = adc_buf[2];
    joystick->roll = adc_buf[3];
}