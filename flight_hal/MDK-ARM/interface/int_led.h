#ifndef __INT_LED_H
#define __INT_LED_H

#include "main.h"


typedef struct
{
    GPIO_TypeDef *port;
    uint16_t pin;
} led_t;


void int_led_on(led_t *led);
void int_led_off(led_t *led);
void int_led_toggle(led_t *led);




#endif /* __INT_LED_H */
