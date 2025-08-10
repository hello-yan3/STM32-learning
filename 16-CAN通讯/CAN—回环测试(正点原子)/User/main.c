/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-24
 * @brief       CAN通信 实验
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32F103开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./USMART/usmart.h"
#include "./BSP/LED/led.h"
#include "./BSP/KEY/key.h"
#include "./BSP/CAN/can.h"

/*
必看！必看！必看！

本程序的Bug！
程序运行can_init()函数时，会卡在HAL_CAN_Init()里面
就是卡在：
while ((hcan->Instance->MSR & CAN_MSR_INAK) == 0U)
while ((hcan->Instance->MSR & CAN_MSR_SLAK) != 0U)
这个循环里面

MSR 主状态寄存器，
INAK 初始化确认，由硬件置'1'，指示软件CAN模块正处于初始化模式。
SLAK 睡眠模式确认，由硬件置’1’，指示软件CAN模块正处于睡眠模式。

我的野火程序：在HAL_CAN_Init()没有上面的循环！
……
*/



void Show(uint8_t *p, uint8_t size)
{
    uint8_t i=0;
    printf("\n");
    for(i=0; i<size; i++)
    {
        printf(" %d ", p[i]);
    }
    printf("\n");
}

int main(void)
{
    uint8_t key;
    uint8_t i = 0, t = 0;
    uint8_t cnt = 0;
    uint8_t canbuf[8];
    uint8_t rxlen = 0;
    uint8_t res;
    uint8_t mode = 1; /* CAN工作模式: 0,普通模式; 1,环回模式 */

    HAL_Init();                                                            /* 初始化HAL库 */
    sys_stm32_clock_init(RCC_PLL_MUL9);                                    /* 设置时钟, 72Mhz */
    delay_init(72);                                                        /* 延时初始化 */
    usart_init(115200);                                                    /* 串口初始化为115200 */
    led_init();                                                            /* 初始化LED */
    key_init();                                                            /* 初始化按键 */
    can_init(CAN_SJW_1TQ, CAN_BS2_8TQ, CAN_BS1_9TQ, 4, CAN_MODE_LOOPBACK); /* CAN初始化, 环回模式, 波特率500Kbps */
    while (1)
    {
        key = key_scan(0);

        if (key == KEY0_PRES) /* KEY0按下,发送一次数据 */
        {
            for (i = 0; i < 8; i++)
            {
                canbuf[i] = cnt + i; /* 填充发送缓冲区 */
                
//                if (i < 4)
//                {
////                    lcd_show_xnum(30 + i * 32, 190, canbuf[i], 3, 16, 0X80, BLUE); /* 显示数据 */
//                }
//                else
//                {
////                    lcd_show_xnum(30 + (i - 4) * 32, 210, canbuf[i], 3, 16, 0X80, BLUE); /* 显示数据 */
//                }
            }
            printf("输出发送数据");
            Show(canbuf, 8);

            res = can_send_msg(0X12, canbuf, 8); /* ID = 0X12, 发送8个字节 */

            if (res)
            {
                  printf("发送失败");
//                lcd_show_string(30 + 80, 170, 200, 16, 16, "Failed", BLUE); /* 提示发送失败 */
            }
            else
            {
                  printf("发送成功");
//                lcd_show_string(30 + 80, 170, 200, 16, 16, "OK    ", BLUE); /* 提示发送成功 */
            }
        }
//        else if (key == WKUP_PRES) /* WK_UP按下，改变CAN的工作模式 */
//        {
//            mode = !mode;

//            if (mode == 0)  /* 普通模式，需要2个开发板 */
//            {
//                can_init(CAN_SJW_1TQ, CAN_BS2_8TQ, CAN_BS1_9TQ, 4, CAN_MODE_NORMAL);    /* CAN普通模式初始化, 普通模式, 波特率500Kbps */
////                lcd_show_string(30, 110, 200, 16, 16, "Nnormal Mode ", RED);
//            }
//            else            /* 回环模式,一个开发板就可以测试了. */
//            {
//                can_init(CAN_SJW_1TQ, CAN_BS2_8TQ, CAN_BS1_9TQ, 4, CAN_MODE_LOOPBACK);  /* CAN普通模式初始化, 回环模式, 波特率500Kbps */
////                lcd_show_string(30, 110, 200, 16, 16, "LoopBack Mode", RED);
//            }
//        }

//        rxlen = can_receive_msg(0X12, canbuf); /* CAN ID = 0X12, 接收数据查询 */

//        if (rxlen) /* 接收到有数据 */
//        {
////            lcd_fill(30, 270, 130, 310, WHITE); /* 清除之前的显示 */

////            for (i = 0; i < rxlen; i++)
////            {
//////                if (i < 4)
//////                {
////////                    lcd_show_xnum(30 + i * 32, 250, canbuf[i], 3, 16, 0X80, BLUE); /* 显示数据 */
//////                }
//////                else
//////                {
////////                    lcd_show_xnum(30 + (i - 4) * 32, 270, canbuf[i], 3, 16, 0X80, BLUE); /* 显示数据 */
//////                }
////            }
////              printf("输出接收数据");
////              Show(canbuf, rxlen);
//        }

        t++;
        delay_ms(10);

        if (t == 20)
        {
            LED0_TOGGLE(); /* 提示系统正在运行 */
            t = 0;
            cnt++;
//            lcd_show_xnum(30 + 48, 150, cnt, 3, 16, 0X80, BLUE); /* 显示数据 */
        }
    }
}
