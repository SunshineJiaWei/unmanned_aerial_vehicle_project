#ifndef __APP_RECV_DATA_H
#define __APP_RECV_DATA_H


#include <string.h>

#include "FreeRTOS.h"
#include "task.h"

#include "com_config.h"
#include "com_debug.h"
#include "int_si24r1.h"

#define FRAME_HEAD_0 0xAA
#define FRAME_HEAD_1 0x55
#define FRAME_HEAD_2 0x00

#define MAX_RETRY_COUNT 10


uint8_t app_recv_data(void);
void app_process_connect_state(uint8_t res);
void app_process_flight_state(void);

#endif /* __APP_RECV_DATA_H */
