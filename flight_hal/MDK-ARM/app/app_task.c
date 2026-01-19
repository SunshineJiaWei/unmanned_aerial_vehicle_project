#include "app_task.h"

#define temp_speed 0
static motor_t motor_left_top = {.tim = &htim3, .channel = TIM_CHANNEL_1, .speed = temp_speed};
static motor_t motor_left_bottom = {.tim = &htim4, .channel = TIM_CHANNEL_4, .speed = temp_speed};
static motor_t motor_right_top = {.tim = &htim2, .channel = TIM_CHANNEL_2, .speed = temp_speed};
static motor_t motor_right_bottom = {.tim = &htim1, .channel = TIM_CHANNEL_3, .speed = temp_speed};


void app_task(void *pvParameters);
#define APP_TASK_STACK_SIZE 128
#define APP_TaSK_PRIORITY 1
TaskHandle_t app_task_handle;

void power_task(void *pvParameters);
#define POWER_TASK_STACK_SIZE 128
#define POWER_TASK_PRIORITY 4
#define POWER_TASK_PERIOD 10000
TaskHandle_t power_task_handle;

void flight_task(void *pvParameters);
#define FLIGHT_TASK_STACK_SIZE 128
#define FLIGHT_TASK_PRIORITY 3
#define FLIGHT_TASK_PERIOD 6
TaskHandle_t flight_task_handle;

void task_entry(void)
{
    xTaskCreate(app_task, "app_task", APP_TASK_STACK_SIZE, NULL, APP_TaSK_PRIORITY, &app_task_handle);
    xTaskCreate(power_task, "power_task", POWER_TASK_STACK_SIZE, NULL, POWER_TASK_PRIORITY, &power_task_handle);
    xTaskCreate(flight_task, "flight_task", FLIGHT_TASK_STACK_SIZE, NULL, FLIGHT_TASK_PRIORITY, &flight_task_handle);

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
        int_motor_set_speed(&motor_left_top);
        int_motor_set_speed(&motor_left_bottom);
        int_motor_set_speed(&motor_right_top);
        int_motor_set_speed(&motor_right_bottom);

        int_motor_start(&motor_left_top);
        int_motor_start(&motor_left_bottom);
        int_motor_start(&motor_right_top);
        int_motor_start(&motor_right_bottom);

        vTaskDelayUntil(&start_time, pdMS_TO_TICKS(FLIGHT_TASK_PERIOD));
        
    }
}
