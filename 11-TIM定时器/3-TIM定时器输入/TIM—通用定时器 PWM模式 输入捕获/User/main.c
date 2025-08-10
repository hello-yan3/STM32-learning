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
#include <stdlib.h>
#include "bsp_general_tim_OC.h" 
#include "bsp_general_tim_IC.h" 
#include ".\key\bsp_key.h" 

/*
实验现象：
使用TIM2 输出 PWM波(输出比较) PB10
使用TIM3 测量 PWM波(输入捕获) PC6
要求：PB0与PA0连接在一起
*/
/*
参考讲解：
https://blog.csdn.net/gongyuan073/article/details/81080011
https://blog.csdn.net/weixin_44793491/article/details/126852975
https://blog.csdn.net/qq_52111026/article/details/117563000
*/

__IO uint8_t  flag;  // 中断标志
__IO uint16_t CCR1;  // CCR1寄存器值
__IO uint16_t CCR2;	// CCR2寄存器值

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
//  DEBUG_USART_Config();
	/* 初始化按键 */
	Key_GPIO_Config();
  
  printf("\r\nPWM输入捕获实验\r\n");
    
	TIMx_OC_Configuration();
	TIMx_IC_Configuration();
	
	/* 定时器2通道3输出PWM */
	HAL_TIM_PWM_Start(&TIM_TimeBaseStructure_OC,GENERAL_TIM_OC_CHANNELx1);  // GENERAL_TIM_CHANNELx1  TIM_CHANNEL_3
  /* 启动定时器通道输入捕获并开启中断 */
  HAL_TIM_IC_Start_IT(&TIM_TimeBaseStructure_IC,GENERAL_TIM_IC_CHANNELx1);	 // GENERAL_TIM_IC_CHANNELx1  TIM_CHANNEL_1
	HAL_TIM_IC_Start_IT(&TIM_TimeBaseStructure_IC,GENERAL_TIM_IC_CHANNELx2);	 // GENERAL_TIM_IC_CHANNELx1  TIM_CHANNEL_1

	uint32_t Freq;  // 频率
	float    Duty;  // 占空比	
	int16_t Pulse3 = 5000;
	
  /* 无限循环 */
  while (1)
  {
		/* 按键1按下，通道3占空比增加 */
		/* 检测是否有按键按下 */
		if(Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON )  
		{	 
			Pulse3+=200;
			if(Pulse3 > 5000)    Pulse3 = 0;   // 如果超过5000，超范围，Pulse回归下限0
			// 设置此时占空比为Pulse
			__HAL_TIM_SET_COMPARE(&TIM_TimeBaseStructure_OC, GENERAL_TIM_OC_CHANNELx1, Pulse3);   // GENERAL_TIM_CHANNELx2  TIM_CHANNEL_3
			// 获得此时占空比
		}
		
		if(flag == 1)
		{
			Freq = 72000000/(GENERAL_TIM_IC_PRESCALER+1)/(CCR1+1);  // 1/((CCR1+1)/(72000000/(GENERAL_TIM_IC_PRESCALER+1))) = 1/((CCR1+1)/(1000000))
			Duty = (float)(CCR2+1)/(CCR1+1);
			printf("此时CCR1为%d \n",CCR1);
			printf("此时CCR2为%d \n",CCR2);
			printf("此时PWM波频率为%dHz\n",Freq);
			printf("此时占空比为%f \n",Duty);
			printf("\n\n");
			flag = 0;
		}
		
		HAL_Delay(500);
	}  
}
// 1 / (CCR/1000) = 1/CCR * 1000

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
