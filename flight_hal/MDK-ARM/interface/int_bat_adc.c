#include "int_bat_adc.h"


void int_bat_adc_init(void)
{
    HAL_GPIO_WritePin(BAT_ADC_EN_GPIO_Port, BAT_ADC_EN_Pin, GPIO_PIN_RESET);
    HAL_ADC_Start(&hadc1);
}

float int_bat_adc_read(void)
{
    uint32_t adc_value = HAL_ADC_GetValue(&hadc1);
    float voltage = ((adc_value * 3.3) / 4095.0) * 2; // Assuming Vref is 3.3V
    return voltage;
}