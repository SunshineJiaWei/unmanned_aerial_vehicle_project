#include "app_flight.h"


extern euler_angle_t euler_angle;
extern remote_data_t remote_data;
extern flight_state_t flight_state;
extern uint16_t target_height;
extern TaskHandle_t communicate_task_handle;

static mpu6050_data_t mpu_data;
static mpu6050_gyro_t last_gyro;
// static float gyro_z_sum;

// 电机实例
static motor_t motor_left_top = {.tim = &htim3, .channel = TIM_CHANNEL_1, .speed = 0};
static motor_t motor_left_bottom = {.tim = &htim4, .channel = TIM_CHANNEL_4, .speed = 0};
static motor_t motor_right_top = {.tim = &htim2, .channel = TIM_CHANNEL_2, .speed = 0};
static motor_t motor_right_bottom = {.tim = &htim1, .channel = TIM_CHANNEL_3, .speed = 0};

static motor_speed_t last_motor_speed;

// 俯仰角
pid_t pitch_pid = {.kp = -7.00, .ki = 0.00, .kd = 0.00};
pid_t gyro_y_pid = {.kp = 3.00, .ki = 0.00, .kd = 0.50};

// 横滚角
pid_t roll_pid = {.kp = -7.00, .ki = 0.00, .kd = 0.00};
pid_t gyro_x_pid = {.kp = 3.00, .ki = 0.00, .kd = 0.50};

// 偏航角
pid_t yaw_pid = {.kp = -3.00, .ki = 0.00, .kd = 0.00};
pid_t gyro_z_pid = {.kp = -5.00, .ki = 0.00, .kd = 0.00};

// 定高pid
pid_t altitude_pid = {.kp = -0.60, .ki = 0.00, .kd = -0.20};

static void app_motor_init(void)
{
    int_motor_start(&motor_left_top);
    int_motor_start(&motor_left_bottom);
    int_motor_start(&motor_right_top);
    int_motor_start(&motor_right_bottom);
}

void app_flight_init(void)
{
    int_mpu6050_init();
    int_vl53l1_init();
    app_motor_init();
}

void app_flight_get_euler_angle(void)
{
    int_mpu6050_get_data(&mpu_data);

    // 对角速度进行一阶低通滤波，准确性要求不好，实时性要求高
    mpu_data.gyro.gyro_x = Common_Filter_LowPass(mpu_data.gyro.gyro_x, last_gyro.gyro_x);
    mpu_data.gyro.gyro_y = Common_Filter_LowPass(mpu_data.gyro.gyro_y, last_gyro.gyro_y);
    mpu_data.gyro.gyro_z = Common_Filter_LowPass(mpu_data.gyro.gyro_z, last_gyro.gyro_z);
    last_gyro = mpu_data.gyro;
    // DEBUG_PRINTF(":%d, %d, %d\n", mpu_data.gyro.gyro_x, mpu_data.gyro.gyro_y, mpu_data.gyro.gyro_z);

    // 对加速度计进行卡尔曼滤波
    mpu_data.accel.accel_x = Common_Filter_KalmanFilter(&kfs[0], mpu_data.accel.accel_x);
    mpu_data.accel.accel_y = Common_Filter_KalmanFilter(&kfs[1], mpu_data.accel.accel_y);
    mpu_data.accel.accel_z = Common_Filter_KalmanFilter(&kfs[2], mpu_data.accel.accel_z);
    // DEBUG_PRINTF(":%d, %d, %d\n", mpu_data.accel.accel_x, mpu_data.accel.accel_y, mpu_data.accel.accel_z);

    // 互补算法计算欧拉角，俯仰角和横滚角使用加速度计，偏航角使用陀螺仪
    // euler_angle.pitch = atan2(mpu_data.accel.accel_x, mpu_data.accel.accel_z) * 180 / 3.141592;
    // euler_angle.roll = atan2(mpu_data.accel.accel_y, mpu_data.accel.accel_z) * 180 / 3.141592;
    // gyro_z_sum += (mpu_data.gyro.gyro_z * 2000.0 / 32768.0) * 0.006;
    // euler_angle.yaw = gyro_z_sum;
    // DEBUG_PRINTF(":%.2f,%.2f,%.2f\n", euler_angle.pitch, euler_angle.roll, euler_angle.yaw);

    // 四元数姿态解算
    Common_IMU_GetEulerAngle(&mpu_data, &euler_angle, SAMPLING_DT);
    // DEBUG_PRINTF(":%.2f,%.2f,%.2f\n", euler_angle.pitch, euler_angle.roll, euler_angle.yaw);
}

void app_flight_pid_process(void)
{
    // 俯仰角
    // 外环：
    // 目标角度
    // 限幅：raw遥控器数据为0-1000，默认居中500。飞机俯仰角最高+-10°
    pitch_pid.desire_val = (remote_data.pitch - 500) / 50.0f;
    // 测量角度
    pitch_pid.measure_val = euler_angle.pitch;

    // 内环：
    // 测量角速度
    gyro_y_pid.measure_val = (mpu_data.gyro.gyro_y * 2000.0f / 32768.0f);

    com_pid_cascade(&pitch_pid, &gyro_y_pid);

    // DEBUG_PRINTF(":%.2f, %.2f\n", gyro_y_pid.err, gyro_y_pid.output);

    // 横滚角
    // 外环：
    // 目标角度
    // 限幅：raw遥控器数据为0-1000，默认居中500。飞机俯仰角最高+-10°
    roll_pid.desire_val = (remote_data.roll - 500) / 50.0f;
    // 测量角度
    roll_pid.measure_val = euler_angle.roll;

    // 内环：
    // 测量角速度
    gyro_x_pid.measure_val = (mpu_data.gyro.gyro_x * 2000.0f / 32768.0f);

    com_pid_cascade(&roll_pid, &gyro_x_pid);

    // 偏航角
    // 外环：
    // 目标角度
    // 限幅：raw遥控器数据为0-1000，默认居中500。飞机俯仰角最高+-10°
    yaw_pid.desire_val = (remote_data.yaw - 500) / 50.0f;
    // 测量角度
    yaw_pid.measure_val = euler_angle.yaw;

    // 内环：
    // 测量角速度
    gyro_z_pid.measure_val = (mpu_data.gyro.gyro_z * 2000.0f / 32768.0f);

    com_pid_cascade(&yaw_pid, &gyro_z_pid);
}

void app_fix_high_pid_process(void)
{
    altitude_pid.desire_val = target_height;
    altitude_pid.measure_val = int_vl53l1_get_distance();
    com_pid_calc(&altitude_pid);
}

void app_flight_control_motor(void)
{
    switch (flight_state)
    {
        case FLIGHT_STATE_IDLE:
        {
            motor_left_top.speed = 0;
            motor_left_bottom.speed = 0;
            motor_right_top.speed = 0;
            motor_right_bottom.speed = 0;
            break;
        }
        case FLIGHT_STATE_NORMAL:
        {
            // 这里+-与极性相关，或者修改pid系数正负也可以实现极性修改，如果极性错误，则会加速下降，无法实现负反馈控制
            // 外环只控制角速度，内环的角速度控制电机转速
            // 偏航角：是对角线为一组，俯仰角：是前面为一组，横滚角：是侧面为一组
            // 这里z轴对油门的影响比例不那么重要，限制其对油门的输出值
            motor_left_top.speed = remote_data.throttle + gyro_y_pid.output - gyro_x_pid.output + COM_LIMIT_RANGE(gyro_z_pid.output, -100, 100);
            motor_left_bottom.speed = remote_data.throttle - gyro_y_pid.output - gyro_x_pid.output - COM_LIMIT_RANGE(gyro_z_pid.output, -100, 100);
            motor_right_top.speed = remote_data.throttle + gyro_y_pid.output + gyro_x_pid.output - COM_LIMIT_RANGE(gyro_z_pid.output, -100, 100);
            motor_right_bottom.speed = remote_data.throttle - gyro_y_pid.output + gyro_x_pid.output + COM_LIMIT_RANGE(gyro_z_pid.output, -100, 100);
            break;
        }
        case FLIGHT_STATE_FIX_HIGH:
        {
            motor_left_top.speed = remote_data.throttle + gyro_y_pid.output - gyro_x_pid.output + COM_LIMIT_RANGE(gyro_z_pid.output, -100, 100) + altitude_pid.output;
            motor_left_bottom.speed = remote_data.throttle - gyro_y_pid.output - gyro_x_pid.output - COM_LIMIT_RANGE(gyro_z_pid.output, -100, 100) + altitude_pid.output;
            motor_right_top.speed = remote_data.throttle + gyro_y_pid.output + gyro_x_pid.output - COM_LIMIT_RANGE(gyro_z_pid.output, -100, 100) + altitude_pid.output;
            motor_right_bottom.speed = remote_data.throttle - gyro_y_pid.output + gyro_x_pid.output + COM_LIMIT_RANGE(gyro_z_pid.output, -100, 100) + altitude_pid.output;
            break;
        }
        case FLIGHT_STATE_FAULT:
        {
            motor_left_top.speed -= 2;
            motor_left_bottom.speed -= 2;
            motor_right_top.speed -= 2;
            motor_right_bottom.speed -= 2;
            if (motor_left_top.speed <= 0 && motor_left_bottom.speed <= 0 && motor_right_top.speed <= 0 && motor_right_bottom.speed <= 0)
            {
                xTaskNotifyGive(communicate_task_handle);
            }
            break;
        }
        
        default:
        {

            break;
        }
    }

    // 电机斜率输出限制，防止突变
    const uint16_t max_change = 15; // 这个参数需要调到一个合理值，综合考虑响应速度和电机电流瞬间过大
    // 但是限制了这里，原本那套pid参数就不适用了需要重新调整
    motor_left_top.speed = com_limit_speed_change(motor_left_top.speed, &last_motor_speed.left_top, max_change);
    motor_left_bottom.speed = com_limit_speed_change(motor_left_bottom.speed, &last_motor_speed.left_bottom, max_change);
    motor_right_top.speed = com_limit_speed_change(motor_right_top.speed, &last_motor_speed.right_top, max_change);
    motor_right_bottom.speed = com_limit_speed_change(motor_right_bottom.speed, &last_motor_speed.right_bottom, max_change);

    // 限制电机转速上限
    motor_left_top.speed = COM_LIMIT_THROTTLE(motor_left_top.speed);
    motor_left_bottom.speed = COM_LIMIT_THROTTLE(motor_left_bottom.speed);
    motor_right_top.speed = COM_LIMIT_THROTTLE(motor_right_top.speed);
    motor_right_bottom.speed = COM_LIMIT_THROTTLE(motor_right_bottom.speed);

    DEBUG_PRINTF(":%d,%d,%d,%d\n", motor_left_top.speed, motor_left_bottom.speed, motor_right_top.speed, motor_right_bottom.speed);

    if (remote_data.throttle <= 50)
    {
        // 保护逻辑
        motor_left_top.speed = 0;
        motor_left_bottom.speed = 0;
        motor_right_top.speed = 0;
        motor_right_bottom.speed = 0;

        last_motor_speed.left_top = 0;
        last_motor_speed.left_bottom = 0;
        last_motor_speed.right_top = 0;
        last_motor_speed.right_bottom = 0;
    }

    int_motor_set_speed(&motor_left_top);
    int_motor_set_speed(&motor_left_bottom);
    int_motor_set_speed(&motor_right_top);
    int_motor_set_speed(&motor_right_bottom);
}
