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


uint8_t app_recv_data(void);

#endif /* __APP_RECV_DATA_H */