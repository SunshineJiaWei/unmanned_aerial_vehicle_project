#ifndef __APP_PROCESS_DATA_H
#define __APP_PROCESS_DATA_H


#include "FreeRTOS.h"
#include "task.h"

#include "int_key.h"
#include "int_joystick.h"

#include "com_debug.h"
#include "com_utils.h"


typedef struct 
{
    uint8_t fix_height;
    uint8_t shutdown;
    uint16_t throttle;
    uint16_t pitch;
    uint16_t roll;
    uint16_t yaw;
} remote_data_t;


void app_process_key_data(void);
void app_process_joystick_data(void);



#endif /* __APP_PROCESS_DATA_H */