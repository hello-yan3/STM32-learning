/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2021-11-04
 * @brief       内存管理 实验
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 探索者 F407开发板
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
#include "./BSP/LED/led.h"
#include "./BSP/KEY/key.h"
#include "./BSP/ADC/adc.h"
#include "./BSP/AP3216C/ap3216c.h"
#include "./BSP/I2C/i2c.h"
#include "uc-os3_demo.h"


/*
  定义静态变量 time，
  软件定时器  一直 time++，之后将 ++ 后的数据，保存至队列中
  
  按键1按下  产生二值信号1，函数在接收到信号后，执行AD采集！采集完成后，AD完成标志置1，顺便亮一下 LED 灯， printf 一下时间！
  按键2按下  产生二值信号2，函数在接收到信号后，执行I2C获取数据！采集完成后，I2C标志置1，顺便亮一下 LED 灯， printf 一下时间！
  按键1&2按下 计数信号++

  数据全部使用队列保存！其中 I2C&AD 使用 任务内嵌队列

  如果 两个标志 均设为1，
  将队列中的数据，printf输出，顺便 printf 一下时间！
*/

int main(void)
{
    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(RCC_PLL_MUL9); /* 设置时钟, 72Mhz */
    delay_init(72);                     /* 延时初始化 */
    usart_init(115200);                 /* 串口初始化为115200 */
    led_init();                         /* 初始化LED */
    key_init();                         /* 初始化按键 */
  
    /*********** ADC ***********/
    Rheostat_Init();
    // 执行 ADC 自动自校准，注意必须在ADC启动前使用
    HAL_ADCEx_Calibration_Start(&ADC_Handle);   
  
    /*********** I2C ***********/
    ap3216c_init();
    
    printf("完成外设初始化，进入UCOSIII内核\r\n");
  
    /*********** UCOSIII ***********/
    uc_os3_demo();                      /* 运行 uC/OS-III 例程 */
}


