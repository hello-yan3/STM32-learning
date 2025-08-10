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
#include "./tim/bsp_motor.h"

/*
电机需要使用电机驱动电路来操作
VM—驱动电压(4.5V~10V)		VCC—逻辑电平(3.3V)		GND(0V)
STBY: H 工作		    L 待机
PWM : H 电机旋转  		L 电机不旋转(制动)    （这里PWM L H 表示纯输出低电平/高电平）
IN1 IN2 极性相同，电机不旋转(制动)   
IN1  IN2    O1  O2
 L    H      L   H   反转
 H    L      H   L   正转   
O1  O2  连接电机 
*/

#define Diff_value 100    // 按一次按键，舵机变化 Diff_value°(30°)

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
//	HAL_TIM_PWM_Start(&TIM_TimeBaseStructure,GENERAL_TIM_CHANNELx2);   // GENERAL_TIM_CHANNELx2  TIM_CHANNEL_4
	
	int16_t Speed = 0;   // 定义速度
	int16_t Get_Speed = 0;  // 获得速度
	
  /* 无限循环 */
  while (1)
  {
		/* 按键1按下，角度增加30° */
		/* 检测是否有按键按下 */
		if(Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON )  
		{	 
			Speed += Diff_value;
			if(Speed > 1000)    Speed = -1000;   
			// 角度调整
			Motor_SetSpeed(Speed);
		}
		
		/* 按键2按下，角度减少30° */
		/* 检测是否有按键按下 */
		if(Key_Scan(KEY2_GPIO_PORT,KEY2_PIN) == KEY_ON )  
		{	 
			Speed -= Diff_value;
			if(Speed < -1000)      Speed = 1000; 
			// 角度调整
			Motor_SetSpeed(Speed);
		}
		
		// 获得此时CCR寄存器值
		Get_Speed = __HAL_TIM_GET_COMPARE(&TIM_TimeBaseStructure, GENERAL_TIM_CHANNELx1);   // GENERAL_TIM_CHANNELx1  TIM_CHANNEL_3
		printf("此时Speed为%d \n",Speed);
		printf("此时CH3的CCR为%d \n",Get_Speed);
		printf("此时CH3的占空比为%f \n",((float)Get_Speed/(GENERAL_TIM_PERIOD+1)));
		printf("\n");
		HAL_Delay(250);
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
