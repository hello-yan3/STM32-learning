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
#include "./systick/bsp_SysTick.h"

// �ο����⣺https://blog.csdn.net/qq_45607873/article/details/124654107
// https://www.cnblogs.com/nevel/p/6357349.html

/*
~~ϵͳ��ʱ����һ������ʵ����ʱ����~~
~~HAL_Delay();  ��ʹ��ϵͳ��ʱ����ʱ; ~~

�������������Ϲ�㣡��
�������ǰ�HAL_Delay()ԭ���Լ�ʵ����һ�飡��
����û��Ҫ������������˷�ʱ�䣡��
��������ʱ�䲻�翴�� HAL_Delay() ��ʱԭ����

�������翴����Ĵ���д������HAL_Delay() ԭ���⣡��
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
  /* ����SysTick Ϊ10us�ж�һ��,ʱ�䵽�󴥷���ʱ�жϣ�
	 *����stm32f1xx_it.c�ļ���SysTick_Handler����ͨ�����жϴ�����ʱ
	 */
	// ϵͳ��ʱ����ʼ��������ϵͳ��ʱ��ʱ��Ƶ��
	SysTick_Init(); 
	while(1)                            
	{
	  LED_RED; 
		// ��ʱ1s
	  Delay_s(1);  	     
	  LED_GREEN;
		// ��ʱ1000ms = 1s
	  Delay_ms(1000);		 
	  LED_BLUE;
		// ��ʱ100000*10us = 1000ms = 1s
	  Delay_10us(100000); 
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
