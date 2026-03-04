#ifndef __INT_VL53L1_H
#define __INT_VL53L1_H


#include "main.h"
#include "FreeRTOS.h"
#include "task.h"

#include "vl53l1_platform.h"
#include "VL53L1X_api.h"
#include "VL53L1X_calibration.h"

void int_vl53l1_init(void);

uint16_t int_vl53l1_get_distance(void);


#endif /* __INT_VL53L1_H */

