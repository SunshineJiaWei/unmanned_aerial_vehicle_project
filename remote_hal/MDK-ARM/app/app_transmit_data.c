#include "app_transmit_data.h"

/*
    | 帧头 | 遥控数据 | 校验和 | 
    | 3B   | 10B     |  4B   |
*/

uint8_t transmit_buf[TX_PLOAD_WIDTH];

extern remote_data_t remote_data;

uint8_t rx_vbat[RX_PLOAD_WIDTH];
recv_data_t recv_data;

void app_transmit_data(void)
{
    uint8_t *p_buf = transmit_buf;

    *p_buf++ = FRAME_HEAD_0;
    *p_buf++ = FRAME_HEAD_1;
    *p_buf++ = FRAME_HEAD_2;

    taskENTER_CRITICAL(); // 这里如果不加锁，那么关机或定高事件会被覆盖，数据竟态
    memcpy(p_buf, &remote_data, 10);
    remote_data.shutdown = 0;
    remote_data.fix_height = 0;
    taskEXIT_CRITICAL();
    p_buf += 10;

    uint32_t check_sum = 0;
    for (uint8_t i = 0; i < 13; i ++)
    {
        check_sum += transmit_buf[i];
    }
    
    memcpy(p_buf, &check_sum, 4);

    int_si24r1_tx_mode();

    uint8_t res = int_si24r1_tx_packet(transmit_buf);

    int_si24r1_rx_mode();

    if (res == 0)
    {
        // 接收电量数据：发送成功遥控数据 =》 一直接收电量数据 =》 直到接收成功
        while (int_si24r1_rx_packet((uint8_t *)&recv_data) == 1)
        {
        }
        DEBUG_PRINTF("rx_vbat: %s unlocked: %d\n", recv_data.vbat, recv_data.unlocked);
    }

    DEBUG_PRINTF(":%d,%d,%d,%d,%d,%d\n", remote_data.throttle, remote_data.yaw, remote_data.pitch, remote_data.roll, remote_data.fix_height, remote_data.shutdown);
}