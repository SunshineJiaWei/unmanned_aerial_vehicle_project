#include "com_pid.h"


void com_pid_calc(pid_t *pid)
{
    // 比例部分
    pid->err = pid->measure_val - pid->desire_val;

    // 积分部分
    pid->integral_err += pid->err;

    // 微分部分
    float derivative_err = 0;
    if (pid->last_err == 0)
    {
        // 首次计算，微分不影响计算结果
        pid->last_err = pid->err;
    }
    derivative_err = pid->err - pid->last_err;

    pid->last_err = pid->err;
    pid->output = (pid->kp * pid->err) + (pid->ki * pid->integral_err * SAMPLING_DT) + (pid->kd * derivative_err / SAMPLING_DT);
}

void com_pid_cascade(pid_t *out_pid, pid_t *in_pid)
{
    // 1、计算外环pid输出值作为内环pid的期望值；2、计算内环pid输出值作为外环pid的测量值
    // 为什么外环的输出值就是角速度：这里使用反推法；总结：外环的输出值与内环的测量值有关
    // 答：因为内环的测量值是角速度，根据PID模型可知：期望值和测量值物理单位必须相同，所以外环的输出值就是角速度。
    com_pid_calc(out_pid);
    in_pid->desire_val = out_pid->output;
    com_pid_calc(in_pid);
}
  