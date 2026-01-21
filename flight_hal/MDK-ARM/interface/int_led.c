#include "int_led.h"


void int_led_on(led_t *led)
{
    HAL_GPIO_WritePin(led->port, led->pin, GPIO_PIN_RESET);
}

void int_led_off(led_t *led)
{
    HAL_GPIO_WritePin(led->port, led->pin, GPIO_PIN_SET);
}

void int_led_toggle(led_t *led)
{
    HAL_GPIO_TogglePin(led->port, led->pin);
}