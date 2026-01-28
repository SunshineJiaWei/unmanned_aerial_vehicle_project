#include "app_recv_data.h"

extern remote_data_t remote_data;
extern remote_state_t remote_state;
extern flight_state_t flight_state;

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

    uint8_t res = int_si24r1_rx_packet(recv_buf);
    if (res)
    {
        return 1;
    }

    if (recv_buf[0] != FRAME_HEAD_0 || recv_buf[1] != FRAME_HEAD_1 || recv_buf[2] != FRAME_HEAD_2)
    {
        DEBUG_PRINTF("frame head error, %02x, %02x, %02x\n", recv_buf[0], recv_buf[1], recv_buf[2]);
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
                }
                else
                {
                    thr_state = THR_STATE_FREE;
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
            }
            break;
        }
        case THR_STATE_MIN:
        {
            if (remote_data.throttle > 100 && xTaskGetTickCount() - enter_min_time < 1000)
            {
                thr_state = THR_STATE_FREE;
            }
            else if(remote_data.throttle <= 100 && xTaskGetTickCount() - enter_min_time >= 1000)
            {
                thr_state = THR_STATE_UNLOCK;
            }
            break;
        }
        case THR_STATE_UNLOCK:
        {
            thr_state = THR_STATE_FREE;
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