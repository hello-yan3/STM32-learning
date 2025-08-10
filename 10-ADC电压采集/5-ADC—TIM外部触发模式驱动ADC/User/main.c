/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ����lcd
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
#include "./led/bsp_led.h" 
#include "./adc/bsp_adc.h"
#include "./tim/bsp_general_tim.h"

// https://blog.csdn.net/m0_46659414/article/details/107882775
// https://blog.csdn.net/Master_0_/article/details/125612622
// https://blog.csdn.net/lljss1980/article/details/107885891
// https://blog.csdn.net/lljss1980/article/details/107885891
// ���˹��ˣ�������һģһ�������ã��������ADC��ȫ�޷������������˹���
// �������̣�����STM32Gϵ�С�STM32F4ϵ�У�û�л���F103�ģ��ѵ������ԭ��

// ȫ�ֱ��������ڱ���ת�������ĵ�ѹֵ 	 
float ADC_Vol; 

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{  
    HAL_Init(); 
    /* ����ϵͳʱ��Ϊ72MHz */
    SystemClock_Config();

    /* ��ʼ��USART1 ����ģʽΪ 115200 8-N-1 */
    DEBUG_USART_Config();

		TIM_Mode_Config();
	
    Rheostat_Init();
		// ִ�� ADC �Զ���У׼��ע�������ADC����ǰʹ��
		HAL_ADCEx_Calibration_Start(&ADC_Handle);   
		
	  // �����ֱ�Ϊ������ṹ�壬���ݴ�ŵ�ַ�����ݳ���(��ͨ����ͨ����)
	  HAL_ADC_Start_DMA(&ADC_Handle, (uint32_t*)&ADC_ConvertedValue, 1);
    while (1)
    {
        ADC_Vol =(float) ADC_ConvertedValue/4096*(float)3.3; // ��ȡת����ADֵ
        printf("\r\n The current AD value = 0x%04X ", ADC_ConvertedValue); 
        printf("\r\n The current AD value = %f V \r\n",ADC_Vol);     
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
