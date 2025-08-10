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
#include <stdlib.h>
#include <string.h>


int a = 0;    // ȫ�ֳ�ʼ����
char *p1;     // ȫ��δ��ʼ����
const int e1 = 12;   // ������

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{
  const int e = 12;   // ������
  int b;     // ջ
  char s[] = "abc";     // ջ
  char *p2;             // ջ
  char *p3 = "123456";  // 123456\0�ڳ�������p3��ջ�ϡ�
  static int c =0;      // ȫ�֣���̬����ʼ����
  p1 = (char *)malloc(10);   
  p2 = (char *)malloc(20);
  // ���������10��20�ֽڵ�������ڶ�����
  strcpy(p1, "123456");    // 123456\0���ڳ����������������ܻὫ����p3��ָ���"123456"�Ż���һ���ط���
  
  /**----------------------------------------------------**/
  
  HAL_Init();        
  /* ����ϵͳʱ��Ϊ72MHz */ 
  SystemClock_Config();
	   
  /*��ʼ��USART ����ģʽΪ 115200 8-N-1���жϽ���*/
  DEBUG_USART_Config();
  LED_GPIO_Config();
	
  /**----------------------------------------------------**/
  
  printf("a�����ĵ�ַ��%p  \r\n", &a);         // SRAM ȫ�ֱ�����       a�����ĵ�ַ��20000010  
  printf("&p1�����ĵ�ַ��%p  \r\n", &p1);      // SRAM ȫ�ֱ�����      &p1�����ĵ�ַ��20000014  
  printf("*p1�����ĵ�ַ��%p  \r\n", p1);       // p1ָ��ָ��ĵ�ַ     *p1�����ĵ�ַ��20000070  
  printf("e1�����ĵ�ַ��%p  \r\n", &e1);       // FLASH�е����ֳ�����   e1�����ĵ�ַ��08002414  

  printf("e�����ĵ�ַ��%p  \r\n", &e);         // SRAM ջ��            e�����ĵ�ַ��20000660  
  printf("b�����ĵ�ַ��%p  \r\n", &b);         // SRAM ջ��            e�����ĵ�ַ��2000065c (0x60-0x04 = 0x5c��ջ�����ݰ�˳���Ų���ջ��������) 
  printf("&s�����ĵ�ַ��%p  \r\n", &s);        // SRAM ջ��           &s�����ĵ�ַ��20000658 (����ջ����ַͬ��)
  printf("s[]�����ĵ�ַ��%p  \r\n", s);        // SRAM ջ��          s[]�����ĵ�ַ��20000658  
  printf("&p2�����ĵ�ַ��%p  \r\n", &p2);      // SRAM ջ��          &p2�����ĵ�ַ��20000654  
  printf("*p2�����ĵ�ַ��%p  \r\n", p2);       // p2ָ��ָ��ĵ�ַ     *p2�����ĵ�ַ��20000080  
  printf("&p3�����ĵ�ַ��%p  \r\n", &p3);      // SRAM ջ��           &p3�����ĵ�ַ��20000650  
  printf("*p3�����ĵ�ַ��%p  \r\n", p3);       // p3ָ��ָ��ĵ�ַ(p3ָ�����ֳ�����)     *p3�����ĵ�ַ��0800221c  
  printf("123456 �����ĵ�ַ��%p  \r\n", "123456");   // FLASH�е����ֳ�����   123456 �����ĵ�ַ��0800221c  
  printf("c�����ĵ�ַ��%p  \r\n", &c);         // SRAM ȫ�ֱ�����       c�����ĵ�ַ��20000018 (ȫ�ֱ���������Ҳ�ǰ�˳���Ų��洢��)
  
  printf("&p1�����ĵ�ַ��%p  \r\n", &p1);      // SRAM ȫ�ֱ�����      &p1�����ĵ�ַ��20000014  
  printf("*p1�����ĵ�ַ��%p  \r\n", p1);       // p1ָ��ָ��ĵ�ַ     *p1�����ĵ�ַ��20000070 (��ʵ�����ڶ���)
  printf("&p2�����ĵ�ַ��%p  \r\n", &p2);      // SRAM ջ��            &p2�����ĵ�ַ��20000654  
  printf("*p2�����ĵ�ַ��%p  \r\n", p2);       // p2ָ��ָ��ĵ�ַ     *p2�����ĵ�ַ��20000080  (��ʵ�����ڶ���) 

  printf("123456  �����ĵ�ַ��%p  \r\n", "123456");   // FLASH�е����ֳ�����   123456 �����ĵ�ַ��0800221c (�������Ż���������� 123456 ��ͬһ����ַ) 
  printf("&p1�����ĵ�ַ��%p  \r\n", &p1);      // SRAM ȫ�ֱ�����      &p1�����ĵ�ַ��20000014  
  printf("p1�����ĵ�ַ��%p  \r\n", p1);        // p1ָ��ָ��ĵ�ַ      p1�����ĵ�ַ��20000070   (p1�����ڶ���) 
  
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
