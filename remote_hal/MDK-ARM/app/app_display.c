#include "app_display.h"


extern remote_data_t remote_data;
extern uint8_t rx_vbat[RX_PLOAD_WIDTH];
extern recv_data_t recv_data;

void app_display_show_bar(uint8_t x, uint8_t y, uint8_t count)
{
    if (count < 13)
    {
        OLED_Show_CH(x, y, 12 + count, 12, 1);
    }
}

/**
 * @brief 初始化显示模块
 *
 */
void app_display_init(void)
{
    OLED_Init();
}

/**
 * @brief 循环执行刷写屏幕
 *
 */
void app_display_show(void)
{
    // 将显示内容写入到缓存
    // (1) X坐标  (2) Y坐标 (3)字符串 (4) 字体大小 (5) 模式(字白底黑)
    // OLED_ShowString(0, 0, "Hello World", 12, 1);

    // 第一行 => 标题  尚硅谷嵌入式
    // for (uint8_t i = 0; i < 7; i++)
    // {
    //     OLED_Show_CH(LINE1_BEGIN + 12 * i, Y0, i, 12, 1);
    // }
    for (uint8_t i = 0; i < 7; i++)
    {
        OLED_Show_CH(22 + 12 * i, Y0, 27 + i, 12, 1);
    }

    // 第二行 => 展示2.4G通信的信道
    for (uint8_t i = 0; i < 2; i++)
    {
        OLED_Show_CH(12 * i, Y1, 34 + i, 12, 1);
    }
    OLED_ShowString(LINE2_BEGIN, Y1, ":", 12, 1);
    uint8_t buff[3] = {0};
    sprintf((char *)buff, "%03d", TX_RX_CHANNEL);
    OLED_ShowString(33, Y1, buff, 12, 1);

    // 显示回传电池电压
    for (uint8_t i = 0; i < 2; i++)
    {
        OLED_Show_CH(62 + 12 * i, Y1, 36 + i, 12, 1);
    }
    OLED_ShowString(88, Y1, ":", 12, 1);
    OLED_ShowString(95, Y1, recv_data.vbat, 12, 1);

    uint8_t count = 0;
    // 第三行 => 展示遥控数据  THR  ROL
    OLED_ShowString(LINE3_BEGIN, Y2, "THR:", 12, 1);
    if (remote_data.throttle > 500)
    {
        count = (remote_data.throttle - 500) / 41;
        // 前一个汉字字符 涂满
        app_display_show_bar(BAR1_BEGING, Y2, 12);
        app_display_show_bar(BAR2_BEGING, Y2, count);
    }
    else
    {
        count = (remote_data.throttle) / 41;
        // 只图前一个字符  后一个图空白
        app_display_show_bar(BAR1_BEGING, Y2, count);
        app_display_show_bar(BAR2_BEGING, Y2, 0);
    }

    OLED_ShowString(LINE3_BEGIN2, Y2, "ROL:", 12, 1);

    if (remote_data.roll > 500)
    {
        count = (remote_data.roll - 500) / 41;
        // 前一个汉字字符 涂满
        app_display_show_bar(BAR1_BEGING2, Y2, 12);
        app_display_show_bar(BAR2_BEGING2, Y2, count);
    }
    else
    {
        count = (remote_data.roll) / 41;
        // 只图前一个字符  后一个图空白
        app_display_show_bar(BAR1_BEGING2, Y2, count);
        app_display_show_bar(BAR2_BEGING2, Y2, 0);
    }

    // 第四行 =>  YAW PIT
    OLED_ShowString(LINE4_BEGIN, Y3, "YAW:", 12, 1);
    if (remote_data.yaw > 500)
    {
        count = (remote_data.yaw - 500) / 41;
        // 前一个汉字字符 涂满
        app_display_show_bar(BAR1_BEGING, Y3, 12);
        app_display_show_bar(BAR2_BEGING, Y3, count);
    }
    else
    {
        count = (remote_data.yaw) / 41;
        // 只图前一个字符  后一个图空白
        app_display_show_bar(BAR1_BEGING, Y3, count);
        app_display_show_bar(BAR2_BEGING, Y3, 0);
    }

    OLED_ShowString(LINE4_BEGIN2, Y3, "PIT:", 12, 1);

    if (remote_data.pitch > 500)
    {
        count = (remote_data.pitch - 500) / 41;
        // 前一个汉字字符 涂满
        app_display_show_bar(BAR1_BEGING2, Y3, 12);
        app_display_show_bar(BAR2_BEGING2, Y3, count);
    }
    else
    {
        count = (remote_data.pitch) / 41;
        // 只图前一个字符  后一个图空白
        app_display_show_bar(BAR1_BEGING2, Y3, count);
        app_display_show_bar(BAR2_BEGING2, Y3, 0);
    }

    // 显示油门解锁
    OLED_ShowString(18, 50, "unlocked:", 12, 1);
    if (recv_data.unlocked)
    {
        OLED_ShowString(24+66, 50, "on ", 12, 1);
    }
    else
    {
        OLED_ShowString(24+66, 50, "off", 12, 1);
    }
    

    // 调用刷写才能真的显示
    OLED_Refresh_Gram();
}
