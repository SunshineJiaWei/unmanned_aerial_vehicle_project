#ifndef __APP_FLIGHT_H
#define __APP_FLIGHT_H

#include <math.h>

#include "int_mpu6050.h"
#include "int_motor.h"
#include "int_vl53l1.h"

#include "com_debug.h"
#include "com_filter.h"
#include "com_imu.h"
#include "com_pid.h"
#include "com_utils.h"

void app_flight_init(void);

void app_flight_get_euler_angle(void);

void app_flight_pid_process(void);

void app_fix_high_pid_process(void);

void app_flight_control_motor(void);


#endif /* __APP_FLIGHT_H */

