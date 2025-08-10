/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   8M����flash����
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
#include "./nrf24l01/bsp_spi.h"
#include ".\key\bsp_key.h" 
#include "./exti/bsp_exti.h"
#include "./nrf24l01/NRF24L01.h"
#include <string.h>
#include <math.h>

/*
�ֲὲ�⣺�ο���NRF24L01������2.54MM�� ��https://blog.csdn.net/weixin_43772810/article/details/123811245
��Ƶ�̳̣�https://www.bilibili.com/video/BV17E41127y3/
����ο���https://blog.csdn.net/weixin_43772810/article/details/123811245
				 https://www.bilibili.com/video/BV1194y1p7wG/
				 
���Կ���ʹ�ã�NRF24L01����תUSBģ�飬��׼��ֱ�Ӳ������Ӽ��ɡ�
�ο���Ƶ��https://www.bilibili.com/video/BV1gN411a7xW
*/

#define ARR_SIZE(a) (sizeof(a) / sizeof(a[0]))

/*
 * ��������main
 * ����  ��������
 * ����  ����
 * ���  ����
 */
int main(void)
{   
  HAL_Init();
  /* �趨ϵͳʱ��Ϊ72MHz */
  SystemClock_Config();
	
	LED_GPIO_Config();
	DEBUG_USART_Config();
	Key_GPIO_Config();
	NRF24L01_Init();
	
	uint32_t Num = 1314520, RxNum;    // Ҫ���͵����֣����յ�������
	char str[] = "Hello world! Send String!", Rxstr[32], Showstr[33];  // ���������ݣ����������ݣ�ȥ�����ݳ�����Ϣ������
	uint8_t TC = 0, RC = 0 ,NumLength = 0;    // ��ʾ�ڼ��η���/���յ����ݣ��յ������ݳ���

	printf("����ͨ�Ų���ʵ��\n\n");
	while(1)
	{
		if( Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON  )
		{
			printf("��%d�η�������\n", TC+1);
			switch(TC%3)
			{
				case 0:     // �����ַ���
        NRF24L01_SendString(str);		
				
				printf("��������Ϊ��%s\n", str);
				break;
			
				case 1:     // �����ַ�����(����'\0')
				NRF24L01_SendMunArray((uint8_t*)str, ARR_SIZE(str));
			
				printf("��������Ϊ��");
				for(uint8_t i=0; i<ARR_SIZE(str); i++)    printf(" %d ",(uint8_t)str[i]);
				printf("\n");
				break;
				
				case 2:     // ��������
				NRF24L01_SendNum(Num);
				
				printf("��������Ϊ��%d\n", Num);
				break;
			}
			TC++;
			
			EXTI_FLAG = 0;    // ò�Ʒ�������Ҳ������½����жϣ������ֶ����
			printf("\n\n");
		}
	
		if( EXTI_FLAG==1 )
		{
			printf("��%d�ν�������\n", RC+1);
			
			NumLength = NRF24L01_GetStr(Rxstr, Showstr);
			
			for (uint8_t i = 0; i < 10; i ++)
			{
				RxNum += Showstr[i] * pow(10, 9 - i);
			}
			
			printf("�յ����ݳ���Ϊ��%d\n", NumLength);
			
			printf("�޷�ֱ���жϽ��ձ������ͣ�\n");
			
			printf("�����յ�����Ϊ�ַ�������");
			printf("��������Ϊ��%s\n", Showstr);
			
			printf("�����յ�����Ϊ�ַ����飬��");
			printf("��������Ϊ��");
			for(uint8_t i=0; i<NumLength; i++)    printf(" %d ",(uint8_t)Showstr[i]);
			printf("\n");
			
			printf("�����յ�����Ϊ���֣���");
			printf("��������Ϊ��%d\n", RxNum);
			
			RC++;
			EXTI_FLAG = 0;    // ò�Ʒ�������Ҳ������½����жϣ������ֶ����

			printf("\n\n");
		}
	
		memset(Showstr, 0, sizeof(Showstr));    // ����������������
		RxNum = 0;   // ������������
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
