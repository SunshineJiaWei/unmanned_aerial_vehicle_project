#include "app_task.h"


// 电源管理任务
void power_task(void *pvParameters);
#define POWER_TASK_STACK_SIZE 128
#define POWER_TASK_PRIORITY 4
#define POWER_TASK_PERIOD 10000
TaskHandle_t power_task_handle;

// 无线通讯任务
void communicate_task(void *pvParameters);
#define COMMUNICATE_TASK_STACK_SIZE 128
#define COMMUNICATE_TASK_PRIORITY 3
#define COMMUNICATE_TASK_PERIOD 6
TaskHandle_t communicate_task_handle;

// 按键任务
void key_task(void *pvParameters);
#define KEY_TASK_STACK_SIZE 128
#define KEY_TASK_PRIORITY 2
#define KEY_TASK_PERIOD 20
TaskHandle_t key_task_handle;

// 摇杆任务
void joystick_task(void *pvParameters);
#define JOYSTICK_TASK_STACK_SIZE 128
#define JOYSTICK_TASK_PRIORITY 2
#define JOYSTICK_TASK_PERIOD 20
TaskHandle_t joystick_task_handle;

void task_entry(void)
{
    xTaskCreate(power_task, "power_task", POWER_TASK_STACK_SIZE, NULL, POWER_TASK_PRIORITY, &power_task_handle);
    xTaskCreate(communicate_task, "communicate_task", POWER_TASK_STACK_SIZE, NULL, COMMUNICATE_TASK_PRIORITY, &communicate_task_handle);
    xTaskCreate(key_task, "key_task", KEY_TASK_STACK_SIZE, NULL, KEY_TASK_PRIORITY, &communicate_task_handle);
    xTaskCreate(joystick_task, "joystick_task", JOYSTICK_TASK_STACK_SIZE, NULL, JOYSTICK_TASK_PRIORITY, &joystick_task_handle);

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
    DEBUG_PRINTF("power_task\n");

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
    DEBUG_PRINTF("communicate_task\n");

    TickType_t start_time = xTaskGetTickCount();

    while (1)
    {
        app_transmit_data();

        // int_si24r1_tx_mode();
        // si24r1_tx_buf[0] = 'h';
        // si24r1_tx_buf[1] = 'e';
        // si24r1_tx_buf[2] = 'l';
        // si24r1_tx_buf[3] = 'l';
        // si24r1_tx_buf[4] = 'o';


        // int_si24r1_tx_packet(si24r1_tx_buf);

        // int_si24r1_rx_mode();

        vTaskDelayUntil(&start_time, pdMS_TO_TICKS(COMMUNICATE_TASK_PERIOD));
    }
}

void key_task(void *pvParameters)
{
    DEBUG_PRINTF("key_task\n");

    TickType_t start_time = xTaskGetTickCount();

    while (1)
    {
        app_process_key_data();

        vTaskDelayUntil(&start_time, pdMS_TO_TICKS(KEY_TASK_PERIOD));
    }
}


void joystick_task(void *pvParameters)
{
    DEBUG_PRINTF("joystick_task\n");

    TickType_t start_time = xTaskGetTickCount();

    int_joystick_init();

    while (1)
    {
        app_process_joystick_data();

        vTaskDelayUntil(&start_time, pdMS_TO_TICKS(JOYSTICK_TASK_PERIOD));
    }
}
