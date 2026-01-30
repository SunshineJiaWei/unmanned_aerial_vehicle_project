#ifndef __APP_FLIGHT_H
#define __APP_FLIGHT_H


#include "int_mpu6050.h"
#include "com_debug.h"


void app_flight_init(void);

void app_flight_get_euler_angle(void);



#endif /* __APP_FLIGHT_H */