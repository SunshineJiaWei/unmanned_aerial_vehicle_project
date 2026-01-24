#include "app_task.h"

void app_task(void *pvParameters);
#define APP_TASK_STACK_SIZE 128
#define APP_TaSK_PRIORITY 1
TaskHandle_t app_task_handle;

void power_task(void *pvParameters);
#define POWER_TASK_STACK_SIZE 128
#define POWER_TASK_PRIORITY 4
#define POWER_TASK_PERIOD 10000
TaskHandle_t power_task_handle;

void communicate_task(void *pvParameters);
#define COMMUNICATE_TASK_STACK_SIZE 128
#define COMMUNICATE_TASK_PRIORITY 3
#define COMMUNICATE_TASK_PERIOD 6
TaskHandle_t communicate_task_handle;

void key_task(void *pvParameters);
#define KEY_TASK_STACK_SIZE 128
#define KEY_TASK_PRIORITY 2
#define KEY_TASK_PERIOD 20
TaskHandle_t key_task_handle;

void task_entry(void)
{
    // xTaskCreate(app_task, "app_task", APP_TASK_STACK_SIZE, NULL, APP_TaSK_PRIORITY, &app_task_handle);
    xTaskCreate(power_task, "power_task", POWER_TASK_STACK_SIZE, NULL, POWER_TASK_PRIORITY, &power_task_handle);
    xTaskCreate(communicate_task, "communicate_task", POWER_TASK_STACK_SIZE, NULL, COMMUNICATE_TASK_PRIORITY, &communicate_task_handle);
    xTaskCreate(key_task, "key_task", KEY_TASK_STACK_SIZE, NULL, KEY_TASK_PRIORITY, &communicate_task_handle);

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

static uint8_t si24r1_tx_buf[TX_PLOAD_WIDTH];
void communicate_task(void *pvParameters)
{
    DEBUG_PRINTF("communicate_task");

    TickType_t start_time = xTaskGetTickCount();

    while (1)
    {
        int_si24r1_tx_mode();

        char *temp = "hello flight";
        memcpy(si24r1_tx_buf, temp, strlen(temp));
        int_si24r1_tx_packet(si24r1_tx_buf);

        int_si24r1_rx_mode();

        vTaskDelayUntil(&start_time, pdMS_TO_TICKS(COMMUNICATE_TASK_PERIOD));
    }
}

void key_task(void *pvParameters)
{
    DEBUG_PRINTF("key_task");

    TickType_t start_time = xTaskGetTickCount();

    while (1)
    {
        key_t key = int_key_get();
        if (key != KEY_NONE)
        {
            DEBUG_PRINTF("key: %d", key);
        }
        vTaskDelayUntil(&start_time, pdMS_TO_TICKS(KEY_TASK_PERIOD));
    }
}
