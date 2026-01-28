#include "app_process_data.h"


// 摇杆实例
static joystick_t joystick;

// 遥控数据实例
remote_data_t remote_data;

// 俯仰角、横滚角偏移量
static int16_t key_pitch_offset = 0;   // 往前飞的正
static int16_t key_roll_offset = 0;    // 往右飞是正

// 摇杆校准偏移量
static int16_t thr_offset, pit_offset, rol_offset, yaw_offset;

// 摇杆校准标志位
static volatile uint8_t is_calibrating;

static void calculate_joystick_error(void)
{
    int16_t thr_sum = 0;
    int16_t pitch_sum = 0;
    int16_t roll_sum = 0;
    int16_t yaw_sum = 0;

    key_pitch_offset = 0;
    key_roll_offset = 0;

    for (uint8_t i = 0; i < 10; i ++)
    {
        app_process_joystick_data();
        thr_sum += joystick.throttle - 0;
        pitch_sum += joystick.pitch - 500;
        roll_sum += joystick.roll - 500;
        yaw_sum += joystick.yaw - 500;
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    thr_offset += thr_sum / 10;
    pit_offset += pitch_sum / 10;
    rol_offset += roll_sum / 10;
    yaw_offset += yaw_sum / 10;
}


void app_process_key_data(void)
{
    key_t key = int_key_get();

    switch (key)
    {
        case KEY_UP:
        {
            key_pitch_offset += 10;
            break;
        }
        case KEY_DOWN:
        {
            key_pitch_offset -= 10;
            break;
        }
        case KEY_LEFT:
        {
            key_roll_offset -= 10;
            break;
        }
        case KEY_RIGHT:
        {
            key_roll_offset += 10;
            break;
        }
        case KEY_LEFT_X:
        {
            remote_data.shutdown = 1;
            break;
        }
        case KEY_RIGHT_X:
        {
            remote_data.fix_height = 1;
            break;
        }
        case KEY_RIGHT_X_LONG:
        {
            // 校准摇杆
            // 这里后续可以优化：
            //  1、按键任务长按时，仅设置标志位，在摇杆任务中检测标志位进行校准
            //  2、这样消除了并发冲突（重入问题），逻辑也更清晰
            //  3、需要使用状态机非阻塞，以及保证不会递归调用
            // 目前这种写法是由线程安全问题的，出现资源竟态
            calculate_joystick_error();
            // is_calibrating = 1;
            break;
        }
        default:
        {
            break;
        }
    }
}

void app_process_joystick_data(void)
{
    // if (is_calibrating)
    // {
    //     is_calibrating = 0;
    //     calculate_joystick_error();
    // }

    taskENTER_CRITICAL();

    int_joystick_get(&joystick);

    // 反相+限幅
    joystick.throttle = 1000 - (joystick.throttle * 1000 / 4095);
    joystick.pitch = 1000 - (joystick.pitch * 1000 / 4095);
    joystick.roll = 1000 - (joystick.roll * 1000 / 4095);
    joystick.yaw = 1000 - (joystick.yaw * 1000 / 4095);

    joystick.pitch += key_pitch_offset;
    joystick.roll += key_roll_offset;

    joystick.throttle = COM_LIMIT_JOYSTICK(joystick.throttle - thr_offset);
    joystick.pitch = COM_LIMIT_JOYSTICK(joystick.pitch - pit_offset);
    joystick.roll = COM_LIMIT_JOYSTICK(joystick.roll - rol_offset);
    joystick.yaw = COM_LIMIT_JOYSTICK(joystick.yaw - yaw_offset);

    taskEXIT_CRITICAL();


    DEBUG_PRINTF(":%d,%d,%d,%d\n", joystick.throttle, joystick.yaw, joystick.pitch, joystick.roll);
}  
