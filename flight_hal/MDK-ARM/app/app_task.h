#ifndef __APP_TASK_H__
#define __APP_TASK_H__


// 应用层 =》 中间件层 =》 硬件接口层 =》 HAL驱动层

// 中间件层
#include "FreeRTOS.h"
#include "task.h"

// 通用层
#include "com_debug.h"
#include "com_config.h"

// 硬件接口层
#include "int_ip5305t.h"
#include "int_motor.h"
#include "int_led.h"
#include "int_si24r1.h"

// 应用层
#include "app_recv_data.h"

void task_entry(void);


#endif /* __APP_TASK_H__ */
