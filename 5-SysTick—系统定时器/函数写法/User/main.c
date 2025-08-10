/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   测试IO中断翻转LED
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
#include "./led/bsp_led.h"
#include "./systick/bsp_SysTick.h"

// 参考讲解：https://blog.csdn.net/qq_45607873/article/details/124654107
// https://www.cnblogs.com/nevel/p/6357349.html

/*
~~系统定时器，一般用于实现延时函数~~
~~HAL_Delay();  即使用系统定时器延时; ~~

！！这个程序纯属瞎搞！！
！！就是把HAL_Delay()原理自己实现了一遍！！
！！没必要在这个程序上浪费时间！！
！！有这时间不如看懂 HAL_Delay() 延时原理！！

！！不如看后面寄存器写法，对HAL_Delay() 原理讲解！！
*/

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
  HAL_Init();
  /* 系统时钟初始化成72MHz */
  SystemClock_Config();
	/* LED 端口初始化 */
	LED_GPIO_Config();
  /* 配置SysTick 为10us中断一次,时间到后触发定时中断，
	 *进入stm32f1xx_it.c文件的SysTick_Handler处理，通过数中断次数计时
	 */
	// 系统定时器初始化并配置系统定时器时钟频率
	SysTick_Init(); 
	while(1)                            
	{
	  LED_RED; 
		// 延时1s
	  Delay_s(1);  	     
	  LED_GREEN;
		// 延时1000ms = 1s
	  Delay_ms(1000);		 
	  LED_BLUE;
		// 延时100000*10us = 1000ms = 1s
	  Delay_10us(100000); 
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
