/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   RTC
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
#include ".\key\bsp_key.h" 
#include "./rtc/bsp_rtc.h"
#include "./rtc/bsp_date.h"
#include "./rtc/bsp_calendar.h"


// ���õ�ǰʱ��
#define HOURS                     10          // 0~23  ʱ
#define MINUTES                   30          // 0~59  ��
#define SECONDS                   30          // 0~59  ��

// ���ں궨��  ����WEEKDAYΪ��Ч�궨�壬ֻҪ���������գ�HAL����Զ������ WEEKDAY
#define WEEKDAY                   5         // 1~7   
#define DATE                      22        // 1~31  ��
#define MONTH                     10        // 1~12  ��
#define YEAR                      2018      // 20xx  ��

// N = 2^32/365/24/60/60 = 136 ��

/*ʱ��ṹ�壬Ĭ��ʱ��2000-01-01 00:00:00*/
struct rtc_time systmtime=
{
	SECONDS, MINUTES, HOURS,
	DATE, MONTH, YEAR, WEEKDAY
};

extern uint8_t TimeDisplay ;

/* �����RTC�����쳣ԭ�������https://www.cnblogs.com/xiaobaibai2021/p/15716909.html */
/* �������1������д�뱸�ݼĴ����������https://www.cnblogs.com/dreamrj/p/14046598.html */
/* �������2��ʹ�ã�RTC_WriteTimeCounter()��RTC_ReadTimeCounter()�������� */
/* ע�⣺�˺�����ԭ����Ϊ��̬�����޷�ʹ�� */    
/* �����ֶ��޸�Դ�ĵ���ʹ�øú��� */


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
  /* ���ڳ�ʼ�� */
  DEBUG_USART_Config();

  printf("\n\r����һ��RTC����ʵ�� \r\n");

	/* ��鲢����RTC */
	RTC_CheckAndConfig(&systmtime);

	while(1)
	{
		/* ÿ��1s ����һ��ʱ��*/
		if (TimeDisplay == 1)
		{
			/* ��ǰʱ�� */
			Time_Display(&systmtime); 		  
			TimeDisplay = 0;
		}
		
		//���°�����ͨ�������޸�ʱ��
		if( Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON  )
		{
			struct rtc_time set_time;

			/*ʹ�ô��ڽ������õ�ʱ�䣬��������ʱע��ĩβҪ�ӻس�*/
			Time_Regulate_Get(&set_time); 
			/*�ý��յ���ʱ������RTC*/
			RTC_CLK_Config();    // RTC��Ҫ��������
			RTC_TimeAndDate_Set(&set_time);
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
