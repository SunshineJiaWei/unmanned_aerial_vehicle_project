#ifndef __APP_TRANSMIT_DATA_H
#define __APP_TRANSMIT_DATA_H


#include <string.h>

#include "FreeRTOS.h"
#include "task.h"

#include "int_si24r1.h"
#include "app_process_data.h"

#include "com_debug.h"

#define FRAME_HEAD_0 0xAA
#define FRAME_HEAD_1 0x55
#define FRAME_HEAD_2 0x00

// #define FRAME_HEAD_0 's'
// #define FRAME_HEAD_1 'g'
// #define FRAME_HEAD_2 'g'

void app_transmit_data(void);

#endif /* __APP_TRANSMIT_DATA_H */