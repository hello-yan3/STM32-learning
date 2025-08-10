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
#include "./color_led/bsp_color_led.h"

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{
  HAL_Init();
  /* ϵͳʱ�ӳ�ʼ����72 MHz */
  SystemClock_Config();

  TIMx_Mode_Config();
  
  /* ����ͨ��PWM��� */
  HAL_TIM_PWM_Start(&COLOR_TIM, COLOR_RED_TIM_CHANNEL  );
  HAL_TIM_PWM_Start(&COLOR_TIM, COLOR_GREEN_TIM_CHANNEL);
  HAL_TIM_PWM_Start(&COLOR_TIM, COLOR_BLUE_TIM_CHANNEL );
	
  while (1)
  {
		SetRGBColor(0xff0000);  // ���
		HAL_Delay(1000);
		
		SetRGBColor(0x00ff00);  // ����
		HAL_Delay(1000);
		
		SetRGBColor(0x0000ff);  // ����
		HAL_Delay(1000);
		
		
		SetRGBColor(0xffff00);  // ǳ��
		HAL_Delay(1000);
		
		SetRGBColor(0xff00ff);  // ����
		HAL_Delay(1000);
		
		SetRGBColor(0x00ffff);  // ����
		HAL_Delay(1000);
		
		
		SetRGBColor(0x0080ff);  // ��ɫ
		HAL_Delay(1000);
		
		SetRGBColor(0x8000ff);  // ����
		HAL_Delay(1000);
		
		SetRGBColor(0x00ff80);  // ��ɫ
		HAL_Delay(1000);
		
		SetRGBColor(0x80ff00);  // ����
		HAL_Delay(1000);
		
		SetRGBColor(0xff0080);  // ��ɫ
		HAL_Delay(1000);
		
		SetRGBColor(0xff8000);  // �Ȼ�
		HAL_Delay(1000);
		
		SetRGBColor(0x8080ff);  // ����
		HAL_Delay(1000);
		
		SetRGBColor(0xff8080);  // ����
		HAL_Delay(1000);
		
		SetRGBColor(0x80ff80);  // ����
		HAL_Delay(1000);
		
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
  oscinitstruct.HSEPredivValue  = RCC_HSE_PREDIV_DIV2;
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
  if (HAL_RCC_ClockConfig(&clkinitstruct, FLASH_LATENCY_0)!= HAL_OK)
  {
    /* Initialization Error */
    while(1); 
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
