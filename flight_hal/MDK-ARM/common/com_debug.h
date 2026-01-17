#ifndef __COM_DEBUG_H
#define __COM_DEBUG_H

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "usart.h"


#define DEBUG_LOG_ENABLE 1

#if DEBUG_LOG_ENABLE
    // 排除路径前缀
    #define __FILE_NAME__ (strrchr(__FILE__, '/') ? (strrchr(__FILE__, '/') + 1):__FILE__)
    #define __FILE_NAME (strrchr(__FILE_NAME__, '\\') ? (strrchr(__FILE_NAME__, '\\') + 1):__FILE_NAME__)

    #define DEBUG_PRINTF(format, ...) printf("[%s:%d] " format, __FILE_NAME, __LINE__, ##__VA_ARGS__)
#else
    #define DEBUG_PRINTF(format, ...) do {} while (0) // 防止优化
#endif /* DEBUG_LOG_ENABLE */

#endif /* __COM_DEBUG_H */

