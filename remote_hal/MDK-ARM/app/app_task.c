#include "app_task.h"

void app_task(void *pvParameters);
#define APP_TASK_STACK_SIZE 128
#define APP_TaSK_PRIORITY 1
TaskHandle_t app_task_handle;

void task2(void *pvParameters);
#define TASK2_STACK_SIZE 128
#define TaSK2_PRIORITY 1
TaskHandle_t task2_handle;

void task_entry(void)
{
    xTaskCreate(app_task, "app_task", APP_TASK_STACK_SIZE, NULL, APP_TaSK_PRIORITY, &app_task_handle);
    xTaskCreate(task2, "app_task", TASK2_STACK_SIZE, NULL, TaSK2_PRIORITY, &task2_handle);

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

void task2(void *pvParameters)

{

    while (1)

    {
        DEBUG_PRINTF("task2");
        vTaskDelay(900);

    }

}
