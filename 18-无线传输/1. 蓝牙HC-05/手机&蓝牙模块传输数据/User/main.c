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
#include ".\key\bsp_key.h" 
#include <string.h>

// �ο�����https://blog.csdn.net/m0_56694518/article/details/131483256
// ATָ��ο����⣺https://www.bilibili.com/video/BV1b44y1U75q
// �ۺϲο���https://www.bilibili.com/video/BV1A8411d759

// ������������໥ͨ�ţ�
// ���ͣ�https://blog.csdn.net/seek97/article/details/81333701
// ��Ƶ��https://www.bilibili.com/video/BV1ii4y1s79z

// PA9 USART��TXD;  PA10 USART��RXD   ע��������ģ�齻������
// ע��ʹ��ǰ����Ҫ�� PA9/PA10 ����Դ��ڵ�TXD/RXD �ε���������������

// ��ʵ���������Ϊ��"α����"������Ҫ�����봮����ȫ��ͬ; ֻ���ʹ�ô����շ�����
// ���ڵ��������ʹ���ֻ����: ������������

// ��ɫСоƬ: ΪSPP-CAģ��, ��ATָ���������; 
// ��ɫ��оƬ: ΪHC-05ģ��, ���Գ�������AT����, AT���Է�����ATָ����ĵ�;
// ����ͨ��ģ���ϵ�/��λ ȷ��ģ������

uint8_t Uart1RX_Data;   // ���ڽ��յ�������ʱ����
uint8_t rxBuffer[50];   // �������ݻ�����
uint8_t rxIndex = 0;    // �������ݱ�־λ

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
	Key_GPIO_Config();
	
	/* �˺����������жϣ����ô����жϵģ����������������ݣ� */
	/* �����https://blog.csdn.net/qq_45413245/article/details/104585026 */
	HAL_UART_Receive_IT(&UartHandle, &Uart1RX_Data, 1);   // �򿪴����жϣ���ʱ���յ������ݻ��Զ�������Uart1RX_Data��
	
	LED_RED;   // LED��ɫδ��������/δ�յ�����
	
	uint8_t count=1;     // �������ݼ���

  while(1)
	{		
			// ���ݽ��յ������ݣ�������ͬ��ָʾ��
			switch(Uart1RX_Data)
			{
				case '1':
					LED_YELLOW;
					break;
				case '2':
					LED_PURPLE;
					break;
				case '3':
					LED_CYAN;
					break;
			}
				
			// ����յ����з�'\n'����ʾ���յ�������һ�����ݣ���ʱ�����н��յ��������ڷ��ͻ���λ��
			if (rxBuffer[rxIndex - 1] == '\n')
			{
				// ������յ�������
				// ���������������Ĵ�����룬����������ݡ���Ӧ��

				// �������ݻظ�
				UART_Transmit("Received data: ");
				UART_Transmit((char*)rxBuffer);

				// ��ս��ջ�����������
				memset(rxBuffer, 0, sizeof(rxBuffer));
				rxIndex = 0;
			}
			
			// ����û����°�������ʱ��������
			if( Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON  )
			{
				// ���ڷ�������
				// ������λ����Keil ���ı��뷽ʽ��ͬ���������Ļ�������루UTF-8 ��ȡ GBK��
				UART_Transmit("Transmit data: Hello World\n");
				printf("This is the %dth times we have received data\n\n", count);

				count++;    // ����ֵ�ۼ�
				LED_RED;    // �����
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
