/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ����IO�жϷ�תLED
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
#include "./led/bsp_led.h"
#include "./key/bsp_encoder.h"
#include "./usart/bsp_debug_usart.h"

/*
��ת����������������λ�á��ٶȻ���ת�����װ�ã�
������ת����תʱ��������˿����������ת�ٶȺͷ����Ӧ�ķ����źţ���ȡ�����źŵ�Ƶ�ʺ���λ��Ϣ���ɵ�֪��ת����ٶȺͷ���

�磺ѡ��ť���ء���դ

ʵ����Ҫʹ�öŰ������ѡ�������������ѡ��PA0��PA1��Ϊѡ����������ţ�
*/



/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{
	HAL_Init();
  /* ϵͳʱ�ӳ�ʼ����72MHz */
  SystemClock_Config();
	/* LED �˿ڳ�ʼ�� */
	LED_GPIO_Config();
  	
	EXTI_Key_Config(); 
	
	DEBUG_USART_Config();
	
	printf("��ת���������ٲ���\n");
	printf("��λʱ��(1s)�ڣ���ת������ÿ��תx�㣬�ƴ���+1\n");
	printf("����ת���ٶ�Ϊ x��/s");
	
	/* �ȴ��жϣ�����ʹ���жϷ�ʽ��CPU������ѯ���� */
	while(1)                            
	{
		HAL_Delay(1000);
		printf("��ǰ��������ת�ٶ�Ϊ��%d\n\n", Encoder_Count);
		Encoder_Count=0;
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
