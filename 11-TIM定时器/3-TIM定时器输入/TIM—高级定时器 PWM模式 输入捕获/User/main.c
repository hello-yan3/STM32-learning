/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2020-xx-xx
  * @brief   TIM���߼���ʱ���������
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103-���� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdlib.h>
#include "./usart/bsp_debug_usart.h"
#include ".\key\bsp_key.h" 
#include "./tim/bsp_general_tim_IC.h"
#include "./tim/bsp_general_tim_OC.h"

/*
ʵ������
ʹ��TIM3 ��� PWM��(����Ƚ�) PA7
ʹ��TIM8 ���� PWM��(���벶��) PC6
Ҫ��PA7��PC6������һ��
*/
/*
�ο����⣺
https://blog.csdn.net/gongyuan073/article/details/81080011
https://blog.csdn.net/weixin_44793491/article/details/126852975
https://blog.csdn.net/qq_52111026/article/details/117563000
*/

__IO uint16_t CCR1;  // CCR1�Ĵ���ֵ
__IO uint16_t CCR2;	 // CCR2�Ĵ���ֵ


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

  /* ��ʼ������ */
  DEBUG_USART_Config();
  /* ��ʼ������ */
	Key_GPIO_Config();
	
  printf("\r\nPWM���벶��ʵ��\r\n");
	TIMx_OC_Configuration();
	TIMx_IC_Configuration();

	/* ��ʱ��3ͨ��2���PWM */
	HAL_TIM_PWM_Start(&TIM_TimeBaseStructure_OC,GENERAL_TIM_OC_CHANNELx1);  // GENERAL_TIM_CHANNELx1  TIM_CHANNEL_2
	/* ������ʱ��ͨ�����벶�񲢿����ж� */
  HAL_TIM_IC_Start_IT(&TIM_TimeBaseStructure_IC,GENERAL_TIM_IC_CHANNELx1);	 // GENERAL_TIM_IC_CHANNELx1  TIM_CHANNEL_1
	HAL_TIM_IC_Start_IT(&TIM_TimeBaseStructure_IC,GENERAL_TIM_IC_CHANNELx2);	 // GENERAL_TIM_IC_CHANNELx2  TIM_CHANNEL_2
	
	float   Freq;  // Ƶ��
	float   Duty;  // ռ�ձ�	
	int16_t Pulse = GENERAL_TIM_OC_CH_CCR;
  
  while(1)
  {   
		/* ����1���£�ͨ��2ռ�ձ����� */
		if(Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON )  
		{	 
			Pulse+=200;
			if(Pulse > 5000)    Pulse = 0;   // �������5000������Χ��Pulse�ع�����0
			// ���ô�ʱռ�ձ�ΪPulse
			__HAL_TIM_SET_COMPARE(&TIM_TimeBaseStructure_OC, GENERAL_TIM_OC_CHANNELx1, Pulse);   // GENERAL_TIM_CHANNELx2  TIM_CHANNEL_3
			// ��ô�ʱռ�ձ�
		}

		/* ����1���£�ͨ��2ռ�ձ����� */
		if(Key_Scan(KEY2_GPIO_PORT,KEY2_PIN) == KEY_ON )  
		{	 
			Pulse-=200;
			if(Pulse < 0)    Pulse = 5000;   // �������0������Χ��Pulse�ع�����5000
			// ���ô�ʱռ�ձ�ΪPulse
			__HAL_TIM_SET_COMPARE(&TIM_TimeBaseStructure_OC, GENERAL_TIM_OC_CHANNELx1, Pulse);   // GENERAL_TIM_CHANNELx2  TIM_CHANNEL_3
			// ��ô�ʱռ�ձ�
		}
		
		Freq = (float)72000000/(GENERAL_TIM_IC_PRESCALER+1)/(CCR1+1);  // 1/((CCR1+1)/(72000000/(GENERAL_TIM_IC_PRESCALER+1))) = 1/((CCR1+1)/(1000000))
		Duty = (float)(CCR2+1)/(CCR1+1);
		printf("��ʱCCR1Ϊ%d \n",CCR1);
		printf("��ʱCCR2Ϊ%d \n",CCR2);
		printf("��ʱPWM��Ƶ��Ϊ%fHz\n",Freq);
		printf("��ʱռ�ձ�Ϊ%f \n",Duty);
		printf("\n\n");

		HAL_Delay(500);
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
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
