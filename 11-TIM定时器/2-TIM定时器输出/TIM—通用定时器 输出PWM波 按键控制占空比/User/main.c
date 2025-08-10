/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   PWM测试
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
#include "./key/bsp_key.h"
#include <stdlib.h>
#include "bsp_general_tim.h" 

/*
实验现象：
按键，灯泡逐渐变亮，之后熄灭，在逐渐变亮
同时，串口输出占空比
*/

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
  DEBUG_USART_Config();
	Key_GPIO_Config();
  
  printf("\r\nPWM波输出实验\r\n");
    
	TIMx_Configuration();
	
	/* 定时器通道3输出PWM */
	HAL_TIM_PWM_Start(&TIM_TimeBaseStructure,GENERAL_TIM_CHANNELx1);   // GENERAL_TIM_CHANNELx1  TIM_CHANNEL_3
	/* 定时器通道4输出PWM */
	HAL_TIM_PWM_Start(&TIM_TimeBaseStructure,GENERAL_TIM_CHANNELx2);   // GENERAL_TIM_CHANNELx2  TIM_CHANNEL_4
	
	int16_t Pulse3=500;
	int16_t Get_Pulse3;
	int16_t Pulse4=500;
	int16_t Get_Pulse4;

  /* 无限循环 */
  while (1)
  {
		/* 按键1按下，通道3占空比增加 */
		/* 检测是否有按键按下 */
		if( Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON )  
		{	 
			Pulse3-=20;
			if(Pulse3 < 0)    Pulse3 = 1000;   // 如果减到负数，超范围，Pulse回归上限1000
			// 设置此时占空比为Pulse
			__HAL_TIM_SET_COMPARE(&TIM_TimeBaseStructure, GENERAL_TIM_CHANNELx1, Pulse3);   // GENERAL_TIM_CHANNELx1  TIM_CHANNEL_3
		}
		
		/* 按键2按下，通道4占空比增加 */
		/* 检测是否有按键按下 */
		if( Key_Scan(KEY2_GPIO_PORT,KEY2_PIN) == KEY_ON )  
		{	 
			Pulse4+=20;
			if(Pulse4 > 1000)    Pulse4 = 0;   // 如果超过1000，超范围，Pulse回归下限0
			// 设置此时占空比为Pulse
			__HAL_TIM_SET_COMPARE(&TIM_TimeBaseStructure, GENERAL_TIM_CHANNELx2, Pulse4);   // GENERAL_TIM_CHANNELx2  TIM_CHANNEL_4
			// 获得此时占空比
		}
		// 获得此时CCR寄存器值
		Get_Pulse3 = __HAL_TIM_GET_COMPARE(&TIM_TimeBaseStructure, GENERAL_TIM_CHANNELx1);   // GENERAL_TIM_CHANNELx1  TIM_CHANNEL_3
		printf("此时CH3的CCR为%d \n",Get_Pulse3);
		printf("此时CH3的占空比为%f \n",((float)Get_Pulse3/(GENERAL_TIM_PERIOD+1)));
		HAL_Delay(100);
		Get_Pulse4 = __HAL_TIM_GET_COMPARE(&TIM_TimeBaseStructure, GENERAL_TIM_CHANNELx2);   // GENERAL_TIM_CHANNELx2  TIM_CHANNEL_4
		printf("此时CH4的CCR为%d \n",Get_Pulse4);
		printf("此时CH4的占空比为%f \n",((float)Get_Pulse4/(GENERAL_TIM_PERIOD+1)));
		
		printf("\n\n");
		HAL_Delay(100);
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
