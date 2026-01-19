#include "app_task.h"

void app_task(void *pvParameters);
#define APP_TASK_STACK_SIZE 128
#define APP_TaSK_PRIORITY 1
TaskHandle_t app_task_handle;

void power_task(void *pvParameters);
#define POWER_TASK_STACK_SIZE 128
#define POWER_TASK_PRIORITY 4
TaskHandle_t power_task_handle;

void task_entry(void)
{
    xTaskCreate(app_task, "app_task", APP_TASK_STACK_SIZE, NULL, APP_TaSK_PRIORITY, &app_task_handle);
    xTaskCreate(power_task, "power_task", POWER_TASK_STACK_SIZE, NULL, POWER_TASK_PRIORITY, &power_task_handle);

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
        vTaskDelayUntil(&start_time, pdMS_TO_TICKS(10000));
        
        int_ip5305t_start();

    }
}
