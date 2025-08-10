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
舵机是一种根据输入PWM信号占空比来控制输出角度的装置
输入PWM信号要求：周期为20ms，高电平宽度为0.5ms~2.5ms

舵机引脚：黑(棕)线(GND), 红线(5V), 黄(橙)线(PWM)
实验时把舵机的引脚与STM32 PB0(或PB1)引脚连接
*/

#define Diff_value 30    // 按一次按键，舵机变化 Diff_value°(30°)

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
//	/* 定时器通道4输出PWM */
//	HAL_TIM_PWM_Start(&TIM_TimeBaseStructure,GENERAL_TIM_CHANNELx2);   // GENERAL_TIM_CHANNELx2  TIM_CHANNEL_4
	
	float Angle = 0;   // 定义角度
	uint16_t Get_Angle = 0;  // 获得角度
	
  /* 无限循环 */
  while (1)
  {
		/* 按键1按下，角度增加30° */
		/* 检测是否有按键按下 */
		if(Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON )  
		{	 
			Angle += Diff_value;
			if(Angle > 180)    Angle = 0;     // 角度>180°后清零
			// 角度调整
			Servo_SetAngle(Angle);
		}
		
		/* 按键2按下，角度减少30° */
		/* 检测是否有按键按下 */
		if(Key_Scan(KEY2_GPIO_PORT,KEY2_PIN) == KEY_ON )  
		{	 
			Angle -= Diff_value;
			if(Angle < 0)      Angle = 180;   // 角度<0°后，回归180°
			// 角度调整
			Servo_SetAngle(Angle);
		}
		
		// 获得此时CCR寄存器值
		Get_Angle = __HAL_TIM_GET_COMPARE(&TIM_TimeBaseStructure, GENERAL_TIM_CHANNELx1);   // GENERAL_TIM_CHANNELx1  TIM_CHANNEL_3
		printf("此时CH3的CCR为%d \n",Get_Angle);
		printf("此时CH3的占空比为%f \n",((float)Get_Angle/(GENERAL_TIM_PERIOD+1)));
		printf("此时高电平宽度为%f \n",((float)((float)Get_Angle/(GENERAL_TIM_PERIOD+1)*20)));
		printf("此时设定角度为%f \n",((float)((float)(Get_Angle-500)/2000*180)));
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
