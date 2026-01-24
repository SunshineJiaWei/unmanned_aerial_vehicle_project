#ifndef __INT_KEY_H
#define __INT_KEY_H


#include "main.h"
#include "FreeRTOS.h"
#include "task.h"


#define LONG_PRESS_PERIOD 1000

typedef enum
{
    KEY_NONE = 0,
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_LEFT_X,
    KEY_RIGHT_X,
    KEY_RIGHT_X_LONG
} key_t;

key_t int_key_get(void);


#endif /* __INT_KEY_H *//
