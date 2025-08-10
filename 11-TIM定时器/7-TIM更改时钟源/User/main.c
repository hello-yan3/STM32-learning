/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   测试lcd
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 F103-霸道 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx.h"
#include "./usart/bsp_debug_usart.h"
#include <stdlib.h>
#include "./tim/bsp_general_tim.h"
#include "./led/bsp_led.h"

/**
https://blog.csdn.net/shileiwu0505/article/details/124136792[1]
https://zhuanlan.zhihu.com/p/74620029[2]
https://blog.csdn.net/u010650845/article/details/81781670[3]
https://blog.csdn.net/shileiwu0505/article/details/124070312[4]
https://blog.csdn.net/u014217137/article/details/121592021[5]


触发信号共两大类：TRGI(外部引入到本定时器的信号) / TRGO(定时器输出给其它定时器或外设的触发信号)[2]

这里对定时器触发源进行一定的讲解：
定时器共有8种触发源：[1]
ITR0        内部触发0
ITR1        内部触发1
ITR2        内部触发2
ITR3        内部触发3
TI1F_ED     TI1边沿检测器
TI1FP1      滤波后的定时器输入1
TI2FP2      滤波后的定时器输入2
ETR1        外部触发输入

第零类：内部时钟源，之前所有程序均使用这种，不多做讲解

第一类：来自定时器自身输入通道1或通道2的输入信号，经过极性选择和滤波以后生成的触发信号，连接到从模式控制器，进而控制计数器的工作(TI1F...)[2]
第二类，来自于外部触发脚[ETR脚]经过极性选择、分频、滤波以后的信号，经过触发输入选择器，连接到从模式控制器(ETR1)[2]
第三类，来自其它定时器的触发输出信号，通过内部线路连接到本定时器的触发输入控制器而连接到从模式控制器(ITR0~ITR3)[2]

不论来自本定时器外部的哪一类触发输入信号，它们有个共同特点，就是都要经过触发输入选择器而连接到从模式控制器，
从而使得计数器的工作受到从模式控制器的控制或影响，基于这一点，定时器工作在从模式。[2]

————————————————————————————————————————————

整体上讲，STM32通用或高级定时器的从模式有如下几种：[2][4]
1、复位模式 【Reset mode】当有效触发输入信号出现时，计数器将会被复位，同时还会产生更新事件和触发事件。
2、触发模式 【Trigger mode】当有效触发输入信号出现时，会将本来处于未使能状态的计数器使能激活，让计数器开始计数，同时还会产生触发事件。
3、门控模式 【Gate mode】定时器根据触发输入信号的电平来启动或停止计数器的计数。在计数器启动或停止时都会产生触发事件并置位相关标志位,TIF@TIMx_SR。
4、外部时钟模式1 【External clock mode 1】在介绍计数器时钟源时，讲到过外部时钟模式1，即计数器的时钟来自TI1或TI2的输入脉冲，连接到从模式控制器并为计数器提供时钟。
也就是说，当计数器的时钟来自触发信号时，计数器就一定处于外部时钟模式1从模式。
当然，这个触发信号就不仅仅限于来自定时器通道TI1/TI2的输入信号，还可以是上面提到过三类触发输入信号的任一种，比方来自其它定时器的触发输出信号，或者来自ETR脚的触发输入信号。
5、编码器模式 【encode mode】(略)

触发模式的典型特点是触发信号可以使能计数器的工作，其它模式的计数器的工作需要软件使能。[2]
外部时钟模式1从模式比较特别，当计数器的时钟源来自触发信号时，此时定时器就工作在外部时钟1从模式，此时触发信号扮演着双角色，即触发信号与时钟信号。

当定时器的工作受到外来触发信号的影响或控制时，它就是工作在从模式，其中从模式可以有多种；如果某定时器能产生触发输出并作为其它定时器的触发输入信号时，此时该定时器就是工作在主模式。
如果某定时器的工作既受外来触发信号的影响或控制，同时又能输出触发信号影响或控制别的从定时器，它就是处于主从双角色模式。[2]

————————————————————————————————————————————

整体上讲，常规定时器可以如下方式产生触发输出信号[TRGO]：[2]

1、软件方式对定时器复位：置位UG@TIMx_EGR
2、使能计数器。置位CEN@TIMx_CR1
3、定时器更新事件 (常用)
4、定时器捕获、比较事件
5、各输出通道中间参考信号 OCxREF

————————————————————————————————————————————
ITR0~ITR3 参考例程详见：[1]
ETR 参考例程详见：[5]

补充：CubeMX：
Slave Mode:       sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
Trigger Source:   sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;

————————————————————————————————————————————
本程序为ETR参考程序，实现功能：
每按下一次按键，ETR输入脉冲，实现计数
*/


uint32_t time = 0; // ms 计时变量 

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void) 
{
  HAL_Init();
	/* 初始化系统时钟为72MHz */
	SystemClock_Config();
	/* 初始化LED */
	LED_GPIO_Config();
  /* 初始化基本定时器定时，1s产生一次中断 */
	TIM_Mode_Config();
  /* 串口初始化 */
  DEBUG_USART_Config();
	/* 在中断模式下启动定时器 */
  HAL_TIM_Base_Start_IT(&TIM_TimeBaseStructure);
	
	while(1)
	{  
    printf("当前计数器值为: %d\n", __HAL_TIM_GET_COUNTER(&TIM_TimeBaseStructure) );
    printf("当前进入中断次数为: %d\n\n", time );
    HAL_Delay(1000);
	}
}


/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 72000000
  *            HCLK(Hz)                       = 72000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 2
  *            APB2 Prescaler                 = 1
  *            HSE Frequency(Hz)              = 8000000
  *            HSE PREDIV1                    = 2
  *            PLLMUL                         = 9
  *            Flash Latency(WS)              = 0
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef clkinitstruct = {0};
  RCC_OscInitTypeDef oscinitstruct = {0};
  
  /* Enable HSE Oscillator and activate PLL with HSE as source */
  oscinitstruct.OscillatorType  = RCC_OSCILLATORTYPE_HSE;
  oscinitstruct.HSEState        = RCC_HSE_ON;
  oscinitstruct.HSEPredivValue  = RCC_HSE_PREDIV_DIV1;
  oscinitstruct.PLL.PLLState    = RCC_PLL_ON;
  oscinitstruct.PLL.PLLSource   = RCC_PLLSOURCE_HSE;
  oscinitstruct.PLL.PLLMUL      = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&oscinitstruct)!= HAL_OK)
  {
    /* Initialization Error */
    while(1); 
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  clkinitstruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  clkinitstruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  clkinitstruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  clkinitstruct.APB2CLKDivider = RCC_HCLK_DIV1;
  clkinitstruct.APB1CLKDivider = RCC_HCLK_DIV2;  
  if (HAL_RCC_ClockConfig(&clkinitstruct, FLASH_LATENCY_2)!= HAL_OK)
  {
    /* Initialization Error */
    while(1); 
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
