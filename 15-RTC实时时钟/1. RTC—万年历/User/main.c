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
  * 实验平台:野火 F103-霸道 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
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


// 设置当前时间
#define HOURS                     10          // 0~23  时
#define MINUTES                   30          // 0~59  分
#define SECONDS                   30          // 0~59  秒

// 日期宏定义  其中WEEKDAY为无效宏定义，只要给出年月日，HAL库会自动计算出 WEEKDAY
#define WEEKDAY                   5         // 1~7   
#define DATE                      22        // 1~31  天
#define MONTH                     10        // 1~12  月
#define YEAR                      2018      // 20xx  年

// N = 2^32/365/24/60/60 = 136 年

/*时间结构体，默认时间2000-01-01 00:00:00*/
struct rtc_time systmtime=
{
	SECONDS, MINUTES, HOURS,
	DATE, MONTH, YEAR, WEEKDAY
};

extern uint8_t TimeDisplay ;

/* 掉电后，RTC日期异常原因详见：https://www.cnblogs.com/xiaobaibai2021/p/15716909.html */
/* 解决方法1：日期写入备份寄存器，详见：https://www.cnblogs.com/dreamrj/p/14046598.html */
/* 解决方法2：使用：RTC_WriteTimeCounter()和RTC_ReadTimeCounter()两个函数 */
/* 注意：此函数在原库中为静态函数无法使用 */    
/* 这里手动修改源文档，使用该函数 */


/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
	HAL_Init();
  /* 系统时钟初始化成72MHz */
  SystemClock_Config();
  /* LED 端口初始化 */
  LED_GPIO_Config();
  /* 串口初始化 */
  DEBUG_USART_Config();

  printf("\n\r这是一个RTC日历实验 \r\n");

	/* 检查并配置RTC */
	RTC_CheckAndConfig(&systmtime);

	while(1)
	{
		/* 每过1s 更新一次时间*/
		if (TimeDisplay == 1)
		{
			/* 当前时间 */
			Time_Display(&systmtime); 		  
			TimeDisplay = 0;
		}
		
		//按下按键，通过串口修改时间
		if( Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON  )
		{
			struct rtc_time set_time;

			/*使用串口接收设置的时间，输入数字时注意末尾要加回车*/
			Time_Regulate_Get(&set_time); 
			/*用接收到的时间设置RTC*/
			RTC_CLK_Config();    // RTC需要重新配置
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
