#include "app_task.h"


// LED实例
static led_t led_left_top = {.port = LED1_GPIO_Port, .pin = LED1_Pin};
static led_t led_left_bottom = {.port = LED4_GPIO_Port, .pin = LED4_Pin};
static led_t led_right_top = {.port = LED2_GPIO_Port, .pin = LED2_Pin};
static led_t led_right_bottom = {.port = LED3_GPIO_Port, .pin = LED3_Pin};

// 遥控状态实例(连接状态)
remote_state_t remote_state = REMOTE_STATE_DISCONNECT;
// 飞行状态实例
flight_state_t flight_state = FLIGHT_STATE_IDLE;
// 遥控数据实例
remote_data_t remote_data = {.throttle = 0, .yaw = 500, .pitch = 500, .roll = 500, .fix_height = 0, .shutdown = 0};
// 欧拉角实例
euler_angle_t euler_angle;


// 电源管理任务
void power_task(void *pvParameters);
#define POWER_TASK_STACK_SIZE 128
#define POWER_TASK_PRIORITY 4
#define POWER_TASK_PERIOD 10000
TaskHandle_t power_task_handle;

// 飞行器任务
void flight_task(void *pvParameters);
#define FLIGHT_TASK_STACK_SIZE 128
#define FLIGHT_TASK_PRIORITY 3
#define FLIGHT_TASK_PERIOD 6
TaskHandle_t flight_task_handle;

// 无线通讯任务
void communicate_task(void *pvParameters);
#define COMMUNICATE_TASK_STACK_SIZE 128
#define COMMUNICATE_TASK_PRIORITY 4 
#define COMMUNICATE_TASK_PERIOD 6
TaskHandle_t communicate_task_handle;

// LED任务
void led_task(void *pvParameters);
#define LED_TASK_STACK_SIZE 128
#define LED_TASK_PRIORITY 1
#define LED_TASK_PERIOD 100
TaskHandle_t led_task_handle;

void task_entry(void)
{
    xTaskCreate(power_task, "power_task", POWER_TASK_STACK_SIZE, NULL, POWER_TASK_PRIORITY, &power_task_handle);
    xTaskCreate(flight_task, "flight_task", FLIGHT_TASK_STACK_SIZE, NULL, FLIGHT_TASK_PRIORITY, &flight_task_handle);
    xTaskCreate(communicate_task, "communicate_task", POWER_TASK_STACK_SIZE, NULL, COMMUNICATE_TASK_PRIORITY, &communicate_task_handle);
    xTaskCreate(led_task, "led_task", LED_TASK_STACK_SIZE, NULL, LED_TASK_PRIORITY, &led_task_handle);

    vTaskStartScheduler();
}

void power_task(void *pvParameters)
{
    DEBUG_PRINTF("power_task");

    while (1)
    {
        uint32_t res = ulTaskNotifyTake(pdTRUE, POWER_TASK_PERIOD);
        if (res != 0)
        {
            int_ip5305t_shutdown();
        }
        else
        {
            int_ip5305t_start();
        }
        
    }
}

void flight_task(void *pvParameters)
{
    DEBUG_PRINTF("flight_task");

    TickType_t start_time = xTaskGetTickCount();

    app_flight_init();

    uint16_t distance = 0;
    uint8_t count = 0;

    while (1)
    {
        // 根据MPU6050数据，通过姿态解算得到欧拉角
        app_flight_get_euler_angle();

        // 根据欧拉角和遥控数据，计算PID输出值
        app_flight_pid_process();

        if (flight_state == FLIGHT_STATE_FIX_HIGH)
        {
            // 24ms计算一次
            count ++;
            if (count >= 4)
            {
                count = 0;
                app_fix_high_pid_process();
            }
        }

        // 根据PID输出值，控制电机转速
        app_flight_control_motor();

        // distance = int_vl53l1_get_distance();
        // DEBUG_PRINTF("distance:%d\n", distance);

        vTaskDelayUntil(&start_time, pdMS_TO_TICKS(FLIGHT_TASK_PERIOD));
    }
}

void communicate_task(void *pvParameters)
{
    DEBUG_PRINTF("communicate_task");

    TickType_t start_time = xTaskGetTickCount();

    while (1)
    {
        uint8_t res = app_recv_data();
        app_process_connect_state(res);

        if (remote_data.shutdown == 1)
        {
            xTaskNotifyGive(power_task_handle);
        }

        app_process_flight_state();

        vTaskDelayUntil(&start_time, pdMS_TO_TICKS(COMMUNICATE_TASK_PERIOD));
    }
}

/**
 * @brief LED任务
 * 灯控状态：
 *  1. 连接状态：左上，右上亮，左下，右下灭
 *  2. 飞行状态：
 *      空闲：  左下，右下500ms闪烁
 *      正常飞：左下，右下200ms闪烁
 *      定高：  左下，右下常亮
 *      故障：  左下，右下灭
 */
void led_task(void *pvParameters)
{
    DEBUG_PRINTF("led_task");

    TickType_t start_time = xTaskGetTickCount();

    uint8_t led_count = 0;

    while (1)
    {
        led_count ++;
        if (remote_state == REMOTE_STATE_CONNECTED)
        {
            int_led_on(&led_left_top);
            int_led_on(&led_right_top);
        }
        else
        {
            int_led_off(&led_left_top);
            int_led_off(&led_right_top);
        }

        switch (flight_state)
        {
            case FLIGHT_STATE_IDLE:
            {
                if (led_count % 5 == 0)
                {
                    int_led_toggle(&led_left_bottom);
                    int_led_toggle(&led_right_bottom);
                }
                break;
            }
            case FLIGHT_STATE_NORMAL:
            {
                if (led_count % 2 == 0)
                {
                    int_led_toggle(&led_left_bottom);
                    int_led_toggle(&led_right_bottom);
                }
                break;
            }
            case FLIGHT_STATE_FIX_HIGH:
            {
                int_led_on(&led_left_bottom);
                int_led_on(&led_right_bottom);
                break;
            }
            case FLIGHT_STATE_FAULT:
            {
                int_led_off(&led_left_bottom);
                int_led_off(&led_right_bottom);
                break;
            }
            default:
            {
                int_led_off(&led_left_bottom);
                int_led_off(&led_right_bottom);
                break;
            }
        }
        if (led_count == 10) // 重置周期为最小公倍数
        {
            led_count = 0; 
        }
        vTaskDelayUntil(&start_time, pdMS_TO_TICKS(LED_TASK_PERIOD)); // 执行周期为闪烁的最小公约数
    }

}
