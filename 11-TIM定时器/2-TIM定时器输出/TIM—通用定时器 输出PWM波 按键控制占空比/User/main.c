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
#include "./key/bsp_key.h"
#include <stdlib.h>
#include "bsp_general_tim.h" 

/*
ʵ������
�����������𽥱�����֮��Ϩ�����𽥱���
ͬʱ���������ռ�ձ�
*/

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
  DEBUG_USART_Config();
	Key_GPIO_Config();
  
  printf("\r\nPWM�����ʵ��\r\n");
    
	TIMx_Configuration();
	
	/* ��ʱ��ͨ��3���PWM */
	HAL_TIM_PWM_Start(&TIM_TimeBaseStructure,GENERAL_TIM_CHANNELx1);   // GENERAL_TIM_CHANNELx1  TIM_CHANNEL_3
	/* ��ʱ��ͨ��4���PWM */
	HAL_TIM_PWM_Start(&TIM_TimeBaseStructure,GENERAL_TIM_CHANNELx2);   // GENERAL_TIM_CHANNELx2  TIM_CHANNEL_4
	
	int16_t Pulse3=500;
	int16_t Get_Pulse3;
	int16_t Pulse4=500;
	int16_t Get_Pulse4;

  /* ����ѭ�� */
  while (1)
  {
		/* ����1���£�ͨ��3ռ�ձ����� */
		/* ����Ƿ��а������� */
		if( Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON )  
		{	 
			Pulse3-=20;
			if(Pulse3 < 0)    Pulse3 = 1000;   // �����������������Χ��Pulse�ع�����1000
			// ���ô�ʱռ�ձ�ΪPulse
			__HAL_TIM_SET_COMPARE(&TIM_TimeBaseStructure, GENERAL_TIM_CHANNELx1, Pulse3);   // GENERAL_TIM_CHANNELx1  TIM_CHANNEL_3
		}
		
		/* ����2���£�ͨ��4ռ�ձ����� */
		/* ����Ƿ��а������� */
		if( Key_Scan(KEY2_GPIO_PORT,KEY2_PIN) == KEY_ON )  
		{	 
			Pulse4+=20;
			if(Pulse4 > 1000)    Pulse4 = 0;   // �������1000������Χ��Pulse�ع�����0
			// ���ô�ʱռ�ձ�ΪPulse
			__HAL_TIM_SET_COMPARE(&TIM_TimeBaseStructure, GENERAL_TIM_CHANNELx2, Pulse4);   // GENERAL_TIM_CHANNELx2  TIM_CHANNEL_4
			// ��ô�ʱռ�ձ�
		}
		// ��ô�ʱCCR�Ĵ���ֵ
		Get_Pulse3 = __HAL_TIM_GET_COMPARE(&TIM_TimeBaseStructure, GENERAL_TIM_CHANNELx1);   // GENERAL_TIM_CHANNELx1  TIM_CHANNEL_3
		printf("��ʱCH3��CCRΪ%d \n",Get_Pulse3);
		printf("��ʱCH3��ռ�ձ�Ϊ%f \n",((float)Get_Pulse3/(GENERAL_TIM_PERIOD+1)));
		HAL_Delay(100);
		Get_Pulse4 = __HAL_TIM_GET_COMPARE(&TIM_TimeBaseStructure, GENERAL_TIM_CHANNELx2);   // GENERAL_TIM_CHANNELx2  TIM_CHANNEL_4
		printf("��ʱCH4��CCRΪ%d \n",Get_Pulse4);
		printf("��ʱCH4��ռ�ձ�Ϊ%f \n",((float)Get_Pulse4/(GENERAL_TIM_PERIOD+1)));
		
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
