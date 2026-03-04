#ifndef __COM_PID_H
#define __COM_PID_H


#include "com_config.h"

/**
 * @brief kp、ki、kd需要在初始化时确定，如果CPU性能强，推荐使用double精度更高
 * 
 */
typedef struct
{
    float kp;               // 比例：值越大响应越快
    float ki;               // 积分：消除稳态误差
    float kd;               // 微分：预判误差，解决过调震荡
    float err;              // 误差
    float last_err;         // 上一次误差
    float integral_err;     // 积分累计误差
    float desire_val;       // 期望值
    float measure_val;      // 测量值
    float output;           // 输出值
} pid_t;

// 单环PID计算
void com_pid_calc(pid_t *pid);
// 串级PID计算，外环角度，内环角速度
void com_pid_cascade(pid_t *out_pid, pid_t *in_pid);


#endif /* __COM_PID_H */
