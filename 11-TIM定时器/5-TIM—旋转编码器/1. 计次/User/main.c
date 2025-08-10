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
#include "Encoder.h" 
#include ".\key\bsp_key.h" 
#include "./tim/bsp_general_tim.h"

/*
��ת����������������λ�á��ٶȻ���ת�����װ�ã�
������ת����תʱ��������˿����������ת�ٶȺͷ����Ӧ�ķ����źţ���ȡ�����źŵ�Ƶ�ʺ���λ��Ϣ���ɵ�֪��ת����ٶȺͷ���

�磺ѡ��ť���ء���դ

ʵ����Ҫʹ�öŰ������ѡ�������������ѡ��PB6��PB7��Ϊѡ����������ţ�
*/
/*
ʵ��ʹ��PB0��PB1�������壬ģ�����������ź�
*/

// �����ʱ����, ��λms
#define TIME     100 
// �������������������֮�����ϵ��Ϊ2��
#define RATIO    4

// �����ֵľ���ֵ
float ABS(float a)
{
	if(a<0) return -a;
	return a;
}

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
	/* ��ʼ������ */
	Key_GPIO_Config();
  
	printf("��ת���������ٲ���\n");

	/* �߼���ʱ��PWM��� */
	TIMx_Configuration();   	
	/* ��ʱ��ͨ��3���PWM */
	HAL_TIM_OC_Start_IT(&TIM_TimeBaseStructure_OC,GENERAL_TIM_CHANNELx1);   // GENERAL_TIM_CHANNELx1  TIM_CHANNEL_3
	/* ��ʱ��ͨ��4���PWM */
	HAL_TIM_OC_Start_IT(&TIM_TimeBaseStructure_OC,GENERAL_TIM_CHANNELx2);   // GENERAL_TIM_CHANNELx2  TIM_CHANNEL_4

	/* ��ʱ��5�������������� */
	TIM_Encoder_Mode_Config();   // ����������
	HAL_TIM_Encoder_Start(&TIM_TimeBaseStructure, TIM_CHANNEL_ALL);  // TIM_CHANNEL_ALL��ʾCHANNEL1��CHANNEL2ͬʱ��ѡ��

	int16_t Encoder_Count;  // CNT�Ĵ���ֵ
	float velocity;       // �ٶȼ���
	uint8_t direction;    // ��ת����
 	
  /* ����ѭ�� */
  while (1)
  {
		
		/* ����1���£������������� */
		/* ����Ƿ��а������� */
		if( Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON )  
		{	 
			OC_Pulse+=100;
			if(OC_Pulse > 2000)   OC_Pulse = 1000;  
		}
		
		/* ����2���£���������ı� */
		/* ����Ƿ��а������� */
		if( Key_Scan(KEY2_GPIO_PORT,KEY2_PIN) == KEY_ON )  
		{	 
			turn = 1;
		}
		
		// ��ת�������Ľ�������ڼ�����CNT��
		// ��ʱʱ��: 50ms
		HAL_Delay(TIME);
		// ����ʹ��__HAL_TIM_GET_COUNTER��ȡCNT��������ֵ
		Encoder_Count = (int16_t)__HAL_TIM_GET_COUNTER(&TIM_TimeBaseStructure);
		direction = __HAL_TIM_IS_TIM_COUNTING_DOWN(&TIM_TimeBaseStructure);
		// ��֪������ֵ����֪����������Ӧ�Ƕȣ����ɻ���õ��ٶ�
		// ��Ϊ�����ģ��������źţ������������PWM������
		velocity = (float)Encoder_Count/TIME;
		
		printf("��������ǰ��ת����: %d, 1��ʾ���������¼���, 0��ʾ���ϼ���\n", direction);
		printf("������������ֵΪ��%d \n",Encoder_Count);
		printf("��ǰ�������ۼƾ����Ĵ���Ϊ��%f ���� \n", ABS(velocity));
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
