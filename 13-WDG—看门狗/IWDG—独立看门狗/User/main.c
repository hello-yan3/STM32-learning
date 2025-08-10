/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   测试led
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
#include ".\key\bsp_key.h" 
#include "./iwdg/bsp_iwdg.h"  
#include "./usart/bsp_debug_usart.h"

/*
程序现象：

间隔1s，RED_LED点亮，
若喂狗(按键)，则LED变为绿灯，持续喂狗(按键)，则保持绿灯
否则，RED_LED熄灭-点亮闪烁
*/


/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{ 
  HAL_Init();
  /* 系统时钟初始化成72 MHz */
  SystemClock_Config();
	/* LED 端口初始化 */
	LED_GPIO_Config();	
	/*初始化USART 配置模式为 115200 8-N-1，中断接收*/
  DEBUG_USART_Config();	
  
  /*初始化按键*/
  Key_GPIO_Config();
	
	HAL_Delay(1000);
	LED_RED;
	
	// 看门狗放在要监控程序前初始化
	IWDG_Config();        	// IWDG 1s 超时溢

  // 如果要被监控的程序正常执行，那么执行完毕之后就会执行喂狗程序，
	// 如果程序跑飞了那程序就会超时，到达不了喂狗的程序,此时就会产生系统复位
	while(1)                            
	{	   
		if( Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON )
		{
			/*喂狗，如果不喂狗，系统则会复位，LD1则会灭一次，如果在1s
			  时间内准时喂狗的话，则LED1会常亮 */
			HAL_IWDG_Refresh(&IWDG_Handle);           // 看门狗喂狗
      
      LED_GREEN;
		}   
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
