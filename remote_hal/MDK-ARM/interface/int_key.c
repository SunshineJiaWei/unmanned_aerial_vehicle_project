#include "int_key.h"

// 这里按键消抖判断，后续可以优化成：使用freertos的软件定时器，周期性的调用按键扫描函数
key_t int_key_get(void)
{
    if (HAL_GPIO_ReadPin(KEY_UP_GPIO_Port, KEY_UP_Pin) == GPIO_PIN_RESET)
    {
        vTaskDelay(pdMS_TO_TICKS(5));
        while (HAL_GPIO_ReadPin(KEY_UP_GPIO_Port, KEY_UP_Pin) == GPIO_PIN_RESET)
        {
            vTaskDelay(pdMS_TO_TICKS(1)); 
            // 在阻塞式判断中，需要增加延时，否则会阻塞低优先级任务
        }

        return KEY_UP;
    }
    else if (HAL_GPIO_ReadPin(KEY_DOWN_GPIO_Port, KEY_DOWN_Pin) == GPIO_PIN_RESET)
    {
        vTaskDelay(pdMS_TO_TICKS(5));
        while (HAL_GPIO_ReadPin(KEY_DOWN_GPIO_Port, KEY_DOWN_Pin) == GPIO_PIN_RESET)
        {
            vTaskDelay(pdMS_TO_TICKS(1));
        }

        return KEY_DOWN;
    }
    else if (HAL_GPIO_ReadPin(KEY_LEFT_GPIO_Port, KEY_LEFT_Pin) == GPIO_PIN_RESET)
    {
        vTaskDelay(pdMS_TO_TICKS(5));
        while (HAL_GPIO_ReadPin(KEY_LEFT_GPIO_Port, KEY_LEFT_Pin) == GPIO_PIN_RESET)
        {
            vTaskDelay(pdMS_TO_TICKS(1));
        }

        return KEY_LEFT;
    }
    else if (HAL_GPIO_ReadPin(KEY_RIGHT_GPIO_Port, KEY_RIGHT_Pin) == GPIO_PIN_RESET)
    {
        vTaskDelay(pdMS_TO_TICKS(5));
        while (HAL_GPIO_ReadPin(KEY_RIGHT_GPIO_Port, KEY_RIGHT_Pin) == GPIO_PIN_RESET)
        {
            vTaskDelay(pdMS_TO_TICKS(1));
        }

        return KEY_RIGHT;
    }
    else if (HAL_GPIO_ReadPin(KEY_LEFT_X_GPIO_Port, KEY_LEFT_X_Pin) == GPIO_PIN_RESET)
    {
        vTaskDelay(pdMS_TO_TICKS(5));
        while (HAL_GPIO_ReadPin(KEY_LEFT_X_GPIO_Port, KEY_LEFT_X_Pin) == GPIO_PIN_RESET)
        {
            vTaskDelay(pdMS_TO_TICKS(1));
        }

        return KEY_LEFT_X;
    }
    else if (HAL_GPIO_ReadPin(KEY_RIGHT_X_GPIO_Port, KEY_RIGHT_X_Pin) == GPIO_PIN_RESET)
    {
        TickType_t press_tick = xTaskGetTickCount();
        vTaskDelay(pdMS_TO_TICKS(5));
        while (HAL_GPIO_ReadPin(KEY_RIGHT_X_GPIO_Port, KEY_RIGHT_X_Pin) == GPIO_PIN_RESET)
        {
            vTaskDelay(pdMS_TO_TICKS(1));
        }

        TickType_t loose_tick = xTaskGetTickCount();
        if (loose_tick - press_tick > pdMS_TO_TICKS(LONG_PRESS_PERIOD))
        {
            return KEY_RIGHT_X_LONG;
        }

        return KEY_RIGHT_X;
    }
 
    return KEY_NONE;
}