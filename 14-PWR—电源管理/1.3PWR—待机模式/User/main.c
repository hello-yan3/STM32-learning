/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ����led
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
#include "./usart/bsp_debug_usart.h"
#include ".\key\bsp_key.h" 
  


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
	LED_GPIO_Config();	
	/* ��ʼ�����Դ��ڣ�һ��Ϊ����1 */
	DEBUG_USART_Config();	
	/*��ʼ������������Ҫ�ж�,����ʼ��KEY2���ɣ�ֻ���ڻ��ѵ�PA0���Ų���Ҫ������ʼ��*/
	Key_GPIO_Config(); 
	
	printf("\r\n ��ӭʹ��Ұ��  STM32 F103 �����塣\r\n");
	printf("Ұ��F103 ����ģʽ����\r\n");
	
	printf("ʵ��˵����\r\n");

	printf("1.�������У��̵Ʊ�ʾ���θ�λ���ϵ�����Ÿ�λ����Ʊ�ʾ�����������״̬�����Ʊ�ʾ�����Ǵ������ѵĸ�λ\r\n");
	printf("2.����KEY2�����󣬻�������ģʽ\r\n");
	printf("3.�ڴ���ģʽ�£���KEY1�����ɻ��ѣ����Ѻ�ϵͳ����и�λ�������ͷ��ʼִ��\r\n");
	printf("4.��ͨ�����WU��־λȷ����λ��Դ\r\n");
	
	printf("5.�ڴ���״̬�£�DAP�������޷���STM32���س�����Ҫ���Ѻ��������");

	//��⸴λ��Դ
	if(__HAL_PWR_GET_FLAG(PWR_FLAG_SB) == SET)
	{
		__HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
		LED_BLUE;
		printf("\r\n �������Ѹ�λ \r\n");
	}
	else
	{
		LED_GREEN;
		printf("\r\n �Ǵ������Ѹ�λ \r\n");
	}
	
  while(1)
  {			
		// K2 ���������������ģʽ
		if(KEY2_LongPress())
		{
			printf("\r\n �����������ģʽ���������ģʽ��ɰ�KEY1���ѣ����Ѻ����и�λ�������ͷ��ʼִ��\r\n");
			LED_RED;	
			HAL_Delay(1000);
			
			/* ʹ�� PWR ʱ�� */
			__HAL_RCC_PWR_CLK_ENABLE();
			/*���WU״̬λ*/
			__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
			
			/* ʹ��WKUP���ŵĻ��ѹ��� ��ʹ��PA0*/
			HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);
			
			//��ͣ�δ�ʱ�ӣ���ֹͨ���δ�ʱ���жϻ���
			HAL_SuspendTick();			
			/* �������ģʽ */
			HAL_PWR_EnterSTANDBYMode();
		}
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
