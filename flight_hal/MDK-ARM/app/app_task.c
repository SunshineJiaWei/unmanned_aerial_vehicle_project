#include "app_task.h"

#define temp_speed 0
// 电机实例
static motor_t motor_left_top = {.tim = &htim3, .channel = TIM_CHANNEL_1, .speed = temp_speed};
static motor_t motor_left_bottom = {.tim = &htim4, .channel = TIM_CHANNEL_4, .speed = temp_speed};
static motor_t motor_right_top = {.tim = &htim2, .channel = TIM_CHANNEL_2, .speed = temp_speed};
static motor_t motor_right_bottom = {.tim = &htim1, .channel = TIM_CHANNEL_3, .speed = temp_speed};

// LED实例
static led_t led_left_top = {.port = LED1_GPIO_Port, .pin = LED1_Pin};
static led_t led_left_bottom = {.port = LED4_GPIO_Port, .pin = LED4_Pin};
static led_t led_right_top = {.port = LED2_GPIO_Port, .pin = LED2_Pin};
static led_t led_right_bottom = {.port = LED3_GPIO_Port, .pin = LED3_Pin};

// 遥控状态实例(连接状态)
remote_state_t remote_state = REMOTE_STATE_DISCONNECT;
// 飞行状态实例
static flight_state_t flight_state = FLIGHT_STATE_IDLE;


void app_task(void *pvParameters);
#define APP_TASK_STACK_SIZE 128
#define APP_TaSK_PRIORITY 1
TaskHandle_t app_task_handle;

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
#define COMMUNICATE_TASK_PRIORITY 2
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
    // xTaskCreate(app_task, "app_task", APP_TASK_STACK_SIZE, NULL, APP_TaSK_PRIORITY, &app_task_handle);
    xTaskCreate(power_task, "power_task", POWER_TASK_STACK_SIZE, NULL, POWER_TASK_PRIORITY, &power_task_handle);
    xTaskCreate(flight_task, "flight_task", FLIGHT_TASK_STACK_SIZE, NULL, FLIGHT_TASK_PRIORITY, &flight_task_handle);
    xTaskCreate(communicate_task, "communicate_task", POWER_TASK_STACK_SIZE, NULL, COMMUNICATE_TASK_PRIORITY, &communicate_task_handle);
    xTaskCreate(led_task, "led_task", LED_TASK_STACK_SIZE, NULL, LED_TASK_PRIORITY, &led_task_handle);

    vTaskStartScheduler();
}

void app_task(void *pvParameters)
{

    while (1)
    {
        DEBUG_PRINTF("app_task");
        vTaskDelay(1000);

    }

}

void power_task(void *pvParameters)
{
    DEBUG_PRINTF("power_task");

    TickType_t start_time = xTaskGetTickCount();

    while (1)
    {
        vTaskDelayUntil(&start_time, pdMS_TO_TICKS(POWER_TASK_PERIOD));
        
        int_ip5305t_start();
    }
}

void flight_task(void *pvParameters)
{
    DEBUG_PRINTF("flight_task");

    TickType_t start_time = xTaskGetTickCount();

    while (1)
    {
        // int_motor_set_speed(&motor_left_top);
        // int_motor_set_speed(&motor_left_bottom);
        // int_motor_set_speed(&motor_right_top);
        // int_motor_set_speed(&motor_right_bottom);

        // int_motor_start(&motor_left_top);
        // int_motor_start(&motor_left_bottom);
        // int_motor_start(&motor_right_top);
        // int_motor_start(&motor_right_bottom);

        vTaskDelayUntil(&start_time, pdMS_TO_TICKS(FLIGHT_TASK_PERIOD));
        
    }
}

static uint8_t si24r1_rx_buf[TX_PLOAD_WIDTH];
void communicate_task(void *pvParameters)
{
    DEBUG_PRINTF("communicate_task");

    TickType_t start_time = xTaskGetTickCount();

    while (1)
    {
        uint8_t res = app_recv_data();
        app_process_connect_state(res);

        vTaskDelayUntil(&start_time, pdMS_TO_TICKS(COMMUNICATE_TASK_PERIOD));
    }
}

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
            led_count == 0; 
        }
        vTaskDelayUntil(&start_time, pdMS_TO_TICKS(LED_TASK_PERIOD)); // 执行周期为闪烁的最小公约数
    }

}
