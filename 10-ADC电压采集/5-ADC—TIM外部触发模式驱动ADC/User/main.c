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
#include "./led/bsp_led.h" 
#include "./adc/bsp_adc.h"
#include "./tim/bsp_general_tim.h"

// https://blog.csdn.net/m0_46659414/article/details/107882775
// https://blog.csdn.net/Master_0_/article/details/125612622
// https://blog.csdn.net/lljss1980/article/details/107885891
// https://blog.csdn.net/lljss1980/article/details/107885891
// 见了鬼了，和网上一模一样的配置，结果就是ADC完全无法工作……见了鬼了
// 网上例程，基于STM32G系列、STM32F4系列，没有基于F103的，难道是这个原因？

// 全局变量，用于保存转换计算后的电压值 	 
float ADC_Vol; 

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{  
    HAL_Init(); 
    /* 配置系统时钟为72MHz */
    SystemClock_Config();

    /* 初始化USART1 配置模式为 115200 8-N-1 */
    DEBUG_USART_Config();

		TIM_Mode_Config();
	
    Rheostat_Init();
		// 执行 ADC 自动自校准，注意必须在ADC启动前使用
		HAL_ADCEx_Calibration_Start(&ADC_Handle);   
		
	  // 参数分别为：句柄结构体，数据存放地址，数据长度(多通道的通道数)
	  HAL_ADC_Start_DMA(&ADC_Handle, (uint32_t*)&ADC_ConvertedValue, 1);
    while (1)
    {
        ADC_Vol =(float) ADC_ConvertedValue/4096*(float)3.3; // 读取转换的AD值
        printf("\r\n The current AD value = 0x%04X ", ADC_ConvertedValue); 
        printf("\r\n The current AD value = %f V \r\n",ADC_Vol);     
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
