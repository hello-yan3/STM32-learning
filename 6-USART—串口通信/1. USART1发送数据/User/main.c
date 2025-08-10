/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   �����жϽ��ղ���
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
#include "./led/bsp_led.h"  

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
	   
  /*��ʼ��USART ����ģʽΪ 115200 8-N-1���жϽ���*/
  DEBUG_USART_Config();
  LED_GPIO_Config();
	
	/*����printf��������Ϊ�ض�����fputc��printf�����ݻ����������*/
	printf("��ӭʹ��Ұ�𿪷���\n");	
	printf("�Զ��庯�����������һ�������жϽ��ջ���ʵ��\n" );	

	printf("\n����8λ����ʵ��\n");	
	Usart_SendByte(0x32);           // 0x32 ASCLL '2'
	
	printf("\n����16λ����ʵ��\n");	
	Usart_SendHalfWord(0x4242);     // 0x42 ASCLL 'B'
	
	printf("\n����32λ����ʵ��\n");	
	Usart_SendWord(0x70707070);     // 0x70 ASCLL 'p'
	
	printf("\n����8λ����ʵ��\n");	
	uint8_t array[10] = {65, 66, 67, 68, 69, 70, 71, 72, 73, 74};   // 65 'A' ......  74 'J'
	Usart_SendArray(array, 10);

	printf("\n�����ַ���ʵ��\n");	
	char str[] = "hello world!\n";
  Usart_SendString(str); 
	
	Serial_Printf("\nʵ��%d���", 1);
	
  while(1)
	{		
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
