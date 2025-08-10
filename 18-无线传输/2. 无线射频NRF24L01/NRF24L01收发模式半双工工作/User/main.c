/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   8M串行flash测试
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
#include "./led/bsp_led.h"  
#include "./nrf24l01/bsp_spi.h"
#include ".\key\bsp_key.h" 
#include "./exti/bsp_exti.h"
#include "./nrf24l01/NRF24L01.h"
#include <string.h>
#include <math.h>

/*
手册讲解：参考：NRF24L01（插针2.54MM） 、https://blog.csdn.net/weixin_43772810/article/details/123811245
视频教程：https://www.bilibili.com/video/BV17E41127y3/
代码参考：https://blog.csdn.net/weixin_43772810/article/details/123811245
				 https://www.bilibili.com/video/BV1194y1p7wG/
				 
调试可以使用：NRF24L01串口转USB模块，标准版直接插入连接即可。
参考视频：https://www.bilibili.com/video/BV1gN411a7xW
*/

#define ARR_SIZE(a) (sizeof(a) / sizeof(a[0]))

/*
 * 函数名：main
 * 描述  ：主函数
 * 输入  ：无
 * 输出  ：无
 */
int main(void)
{   
  HAL_Init();
  /* 设定系统时钟为72MHz */
  SystemClock_Config();
	
	LED_GPIO_Config();
	DEBUG_USART_Config();
	Key_GPIO_Config();
	NRF24L01_Init();
	
	uint32_t Num = 1314520, RxNum;    // 要发送的数字，接收到的数据
	char str[] = "Hello world! Send String!", Rxstr[32], Showstr[33];  // 待发送数据，待接收数据，去除数据长度信息后数据
	uint8_t TC = 0, RC = 0 ,NumLength = 0;    // 表示第几次发送/接收到数据，收到的数据长度

	printf("无线通信测试实验\n\n");
	while(1)
	{
		if( Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON  )
		{
			printf("第%d次发送数据\n", TC+1);
			switch(TC%3)
			{
				case 0:     // 发送字符串
        NRF24L01_SendString(str);		
				
				printf("发送数据为：%s\n", str);
				break;
			
				case 1:     // 发送字符数组(包含'\0')
				NRF24L01_SendMunArray((uint8_t*)str, ARR_SIZE(str));
			
				printf("发送数据为：");
				for(uint8_t i=0; i<ARR_SIZE(str); i++)    printf(" %d ",(uint8_t)str[i]);
				printf("\n");
				break;
				
				case 2:     // 发送数字
				NRF24L01_SendNum(Num);
				
				printf("发送数据为：%d\n", Num);
				break;
			}
			TC++;
			
			EXTI_FLAG = 0;    // 貌似发送数据也会产生下降沿中断，这里手动清除
			printf("\n\n");
		}
	
		if( EXTI_FLAG==1 )
		{
			printf("第%d次接收数据\n", RC+1);
			
			NumLength = NRF24L01_GetStr(Rxstr, Showstr);
			
			for (uint8_t i = 0; i < 10; i ++)
			{
				RxNum += Showstr[i] * pow(10, 9 - i);
			}
			
			printf("收到数据长度为：%d\n", NumLength);
			
			printf("无法直接判断接收变量类型，\n");
			
			printf("假设收到数据为字符串，则：");
			printf("接收数据为：%s\n", Showstr);
			
			printf("假设收到数据为字符数组，则：");
			printf("接收数据为：");
			for(uint8_t i=0; i<NumLength; i++)    printf(" %d ",(uint8_t)Showstr[i]);
			printf("\n");
			
			printf("假设收到数据为数字，则：");
			printf("接收数据为：%d\n", RxNum);
			
			RC++;
			EXTI_FLAG = 0;    // 貌似发送数据也会产生下降沿中断，这里手动清除

			printf("\n\n");
		}
	
		memset(Showstr, 0, sizeof(Showstr));    // 接收数据数组清零
		RxNum = 0;   // 接收数字清零
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
