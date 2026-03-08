#include "app_recv_data.h"


extern remote_data_t remote_data;
extern remote_state_t remote_state;
extern flight_state_t flight_state;
extern back_data_t back_data;

// 定高高度
uint16_t target_height;

static thr_state_t thr_state;
static uint32_t enter_max_time;
static uint32_t enter_min_time;

static uint8_t recv_buf[TX_PLOAD_WIDTH];
static uint8_t retry_count = 0;

/**
 * @brief 
 * 
 * @return uint8_t 成功接收返回0，接收失败或校验失败返回1
 */
uint8_t app_recv_data(void)
{
    memset(recv_buf, 0, TX_PLOAD_WIDTH);
    uint8_t res = int_si24r1_rx_packet(recv_buf);
    if (res)
    {
        return 1;
    }
    if (res == 0)
    {
        /*
            2.4G属于同步发送逻辑，通讯流程：
            无人机开机接收数据 =》 遥控器发送数据 =》 无人机接收到遥控数据 =》 回传电量数据
        */
        uint16_t count = 500;
        int_si24r1_tx_mode();
        // 回传 电量数据：接收成功遥控数据 =》 一直发电量数据 =》 直到发送成功
        while (int_si24r1_tx_packet((uint8_t *)&back_data) == 1 && count--)
        {
            /*
                1. 发送端发送数据包
                2. 接收端收到数据包 
                3. 接收端自动回复 ACK（硬件自动完成，无需编程）
                4. 发送端收到 ACK 
                5. STATUS 寄存器的 TX_DS 位置 
                6. 发送成功，return 0

                1. 发送端未收到 ACK
                2. 自动重发（重发次数在 SETUP_RETR 寄存器配置）
                3. 达到最大重发次数仍未收到 ACK
                4. STATUS 寄存器的 MAX_RT 位置 1
                5. 发送失败，return 1
            */
        }
        int_si24r1_rx_mode();
    }

    if (recv_buf[0] != FRAME_HEAD_0 || recv_buf[1] != FRAME_HEAD_1 || recv_buf[2] != FRAME_HEAD_2)
    {
        // DEBUG_PRINTF("frame head error, %02x, %02x, %02x\n", recv_buf[0], recv_buf[1], recv_buf[2]);
        return 1;
    }

    uint32_t csum = 0;
    for (uint8_t i = 0; i < 13; i ++)
    {
        csum += recv_buf[i];
    }

    uint32_t rsum = 0;
    uint8_t *temp = &recv_buf[13];
    memcpy(&rsum, temp, sizeof(uint32_t));

    if (csum != rsum)
    {
        DEBUG_PRINTF("check sum error, csum: %d, rsum: %d\n", csum, rsum);
        return 1;
    }


    taskENTER_CRITICAL();

    // 保存遥控数据
    uint8_t *p_data = &recv_buf[3];
    memcpy(&remote_data, p_data, 10);

    taskEXIT_CRITICAL();

    DEBUG_PRINTF(":%d,%d,%d,%d,%d,%d\n", remote_data.throttle, remote_data.yaw, remote_data.pitch, remote_data.roll, remote_data.fix_height, remote_data.shutdown);

    // if (remote_data.fix_height)
    // {
    //     DEBUG_PRINTF("fix height\n");
    // }
    // if (remote_data.shutdown)
    // {
    //     DEBUG_PRINTF("shutdown\n");
    // }

    return 0;
}

void app_process_connect_state(uint8_t res)
{
    if (res == 0)
    {
        remote_state = REMOTE_STATE_CONNECTED;
        retry_count = 0;
    }
    else
    {
        retry_count ++;
        if (retry_count >= MAX_RETRY_COUNT)
        {
            remote_state = REMOTE_STATE_DISCONNECT;
        }
    }   
}

/**
 * @brief 油门解锁
 * 
 * @return uint8_t 解锁成果：0表示成功，非0表示失败
 */
static uint8_t app_throttle_unlock(void)
{
    switch (thr_state)
    {
        case THR_STATE_FREE:
        {
            if (remote_data.throttle >= 900)
            {
                thr_state = THE_STATE_MAX;
                enter_max_time = xTaskGetTickCount();
                DEBUG_PRINTF("1");
            }
            break;
        }
        case THE_STATE_MAX:
        {
            if (remote_data.throttle < 900)
            {
                if (xTaskGetTickCount() - enter_max_time >= 1000)
                {
                    thr_state = THR_STATE_LEAVE_MAX;
                    DEBUG_PRINTF("2");
                }
                else
                {
                    thr_state = THR_STATE_FREE;
                    DEBUG_PRINTF("3");
                }
            }
            break;
        }
        case THR_STATE_LEAVE_MAX:
        {
            if (remote_data.throttle <= 100)
            {
                thr_state = THR_STATE_MIN;
                enter_min_time = xTaskGetTickCount();
                DEBUG_PRINTF("4");
            }
            break;
        }
        case THR_STATE_MIN:
        {
            if (remote_data.throttle > 100 && xTaskGetTickCount() - enter_min_time < 1000)
            {
                thr_state = THR_STATE_FREE;
                DEBUG_PRINTF("5");
            }
            else if(remote_data.throttle <= 100 && xTaskGetTickCount() - enter_min_time >= 1000)
            {
                thr_state = THR_STATE_UNLOCK;
                DEBUG_PRINTF("6");
            }
            break;
        }
        case THR_STATE_UNLOCK:
        {
            thr_state = THR_STATE_FREE;
            DEBUG_PRINTF("7");
            return 0;
            break;
        }
        default:
        {
            flight_state = FLIGHT_STATE_IDLE;
            break;
        }
    }

    return 1;
}


void app_process_flight_state(void)
{
    switch (flight_state)
    {
        case FLIGHT_STATE_IDLE:
        {
            if (!app_throttle_unlock())
            {
                flight_state = FLIGHT_STATE_NORMAL;
            }
            break;
        }
        case FLIGHT_STATE_NORMAL:
        {
            if (remote_data.fix_height)
            {
                flight_state = FLIGHT_STATE_FIX_HIGH;
                target_height = int_vl53l1_get_distance();
            }
            if (remote_state == REMOTE_STATE_DISCONNECT)
            {
                flight_state = FLIGHT_STATE_FAULT;
            }
            break;
        }
        case FLIGHT_STATE_FIX_HIGH:
        {
            if (remote_data.fix_height)
            {
                flight_state = FLIGHT_STATE_NORMAL;
            }
            if (remote_state == REMOTE_STATE_DISCONNECT)
            {
                flight_state = FLIGHT_STATE_FAULT;
            }
            break;
        }
        case FLIGHT_STATE_FAULT:
        {
            ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
            flight_state = FLIGHT_STATE_IDLE;
            break;
        }
        default:
        {
            flight_state = FLIGHT_STATE_IDLE;
            break;
        }
    }

}
