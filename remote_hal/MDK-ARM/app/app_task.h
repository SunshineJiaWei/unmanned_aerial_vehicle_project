#ifndef __APP_TASK_H__
#define __APP_TASK_H__


#include <string.h>

// 中间件层
#include "FreeRTOS.h"
#include "task.h"

// 公共层
#include "com_debug.h"

// 硬件接口层
#include "int_ip5305t.h"
#include "int_si24r1.h"
#include "int_key.h"
#include "int_joystick.h"

void task_entry(void);


#endif /* __APP_TASK_H__ */
