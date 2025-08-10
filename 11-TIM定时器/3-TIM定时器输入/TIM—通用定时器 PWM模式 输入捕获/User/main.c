/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   PWM����
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103-�Ե� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
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
ʵ������
ʹ��TIM2 ��� PWM��(����Ƚ�) PB10
ʹ��TIM3 ���� PWM��(���벶��) PC6
Ҫ��PB0��PA0������һ��
*/
/*
�ο����⣺
https://blog.csdn.net/gongyuan073/article/details/81080011
https://blog.csdn.net/weixin_44793491/article/details/126852975
https://blog.csdn.net/qq_52111026/article/details/117563000
*/

__IO uint8_t  flag;  // �жϱ�־
__IO uint16_t CCR1;  // CCR1�Ĵ���ֵ
__IO uint16_t CCR2;	// CCR2�Ĵ���ֵ

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void) 
{ 
  HAL_Init();
	/* ��ʼ��ϵͳʱ��Ϊ72MHz */
	SystemClock_Config();
	/* ��ʼ��LED */ 
//  DEBUG_USART_Config();
	/* ��ʼ������ */
	Key_GPIO_Config();
  
  printf("\r\nPWM���벶��ʵ��\r\n");
    
	TIMx_OC_Configuration();
	TIMx_IC_Configuration();
	
	/* ��ʱ��2ͨ��3���PWM */
	HAL_TIM_PWM_Start(&TIM_TimeBaseStructure_OC,GENERAL_TIM_OC_CHANNELx1);  // GENERAL_TIM_CHANNELx1  TIM_CHANNEL_3
  /* ������ʱ��ͨ�����벶�񲢿����ж� */
  HAL_TIM_IC_Start_IT(&TIM_TimeBaseStructure_IC,GENERAL_TIM_IC_CHANNELx1);	 // GENERAL_TIM_IC_CHANNELx1  TIM_CHANNEL_1
	HAL_TIM_IC_Start_IT(&TIM_TimeBaseStructure_IC,GENERAL_TIM_IC_CHANNELx2);	 // GENERAL_TIM_IC_CHANNELx1  TIM_CHANNEL_1

	uint32_t Freq;  // Ƶ��
	float    Duty;  // ռ�ձ�	
	int16_t Pulse3 = 5000;
	
  /* ����ѭ�� */
  while (1)
  {
		/* ����1���£�ͨ��3ռ�ձ����� */
		/* ����Ƿ��а������� */
		if(Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON )  
		{	 
			Pulse3+=200;
			if(Pulse3 > 5000)    Pulse3 = 0;   // �������5000������Χ��Pulse�ع�����0
			// ���ô�ʱռ�ձ�ΪPulse
			__HAL_TIM_SET_COMPARE(&TIM_TimeBaseStructure_OC, GENERAL_TIM_OC_CHANNELx1, Pulse3);   // GENERAL_TIM_CHANNELx2  TIM_CHANNEL_3
			// ��ô�ʱռ�ձ�
		}
		
		if(flag == 1)
		{
			Freq = 72000000/(GENERAL_TIM_IC_PRESCALER+1)/(CCR1+1);  // 1/((CCR1+1)/(72000000/(GENERAL_TIM_IC_PRESCALER+1))) = 1/((CCR1+1)/(1000000))
			Duty = (float)(CCR2+1)/(CCR1+1);
			printf("��ʱCCR1Ϊ%d \n",CCR1);
			printf("��ʱCCR2Ϊ%d \n",CCR2);
			printf("��ʱPWM��Ƶ��Ϊ%dHz\n",Freq);
			printf("��ʱռ�ձ�Ϊ%f \n",Duty);
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
