/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   串口中断接收测试
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
#include ".\key\bsp_key.h" 
#include <string.h>

// 参考程序：https://blog.csdn.net/m0_56694518/article/details/131483256
// AT指令参考讲解：https://www.bilibili.com/video/BV1b44y1U75q
// 综合参考：https://www.bilibili.com/video/BV1A8411d759

// 两个蓝牙间的相互通信：
// 博客：https://blog.csdn.net/seek97/article/details/81333701
// 视频：https://www.bilibili.com/video/BV1ii4y1s79z

// PA9 USART的TXD;  PA10 USART的RXD   注意与蓝牙模块交叉连接
// 注意使用前，需要把 PA9/PA10 与调试串口的TXD/RXD 拔掉，否则会产生干扰

// 其实这里的蓝牙为："伪蓝牙"，其主要操作与串口完全相同; 只需会使用串口收发即可
// 串口调试软件，使用手机软件: 蓝牙调试助手

// 蓝色小芯片: 为SPP-CA模块, 无AT指令，其他正常; 
// 蓝色大芯片: 为HC-05模块, 可以长按进入AT调试, AT调试方法及AT指令见文档;
// 可以通过模块上电/复位 确定模块名称

uint8_t Uart1RX_Data;   // 串口接收的数据临时保存
uint8_t rxBuffer[50];   // 接收数据缓冲区
uint8_t rxIndex = 0;    // 接收数据标志位

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
  HAL_Init();        
  /* 配置系统时钟为72MHz */ 
  SystemClock_Config();
	   
  /*初始化USART 配置模式为 115200 8-N-1，中断接收*/
  DEBUG_USART_Config();
  LED_GPIO_Config();
	Key_GPIO_Config();
	
	/* 此函数用来打开中断，配置串口中断的！不是用来接收数据！ */
	/* 详见：https://blog.csdn.net/qq_45413245/article/details/104585026 */
	HAL_UART_Receive_IT(&UartHandle, &Uart1RX_Data, 1);   // 打开串口中断，此时接收到的数据会自动保存在Uart1RX_Data中
	
	LED_RED;   // LED红色未建立连接/未收到数据
	
	uint8_t count=1;     // 发送数据计数

  while(1)
	{		
			// 根据接收到的数据，点亮不同的指示灯
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
				
			// 如果收到换行符'\n'，表示接收到完整的一行数据，此时将所有接收到的数据在发送回上位机
			if (rxBuffer[rxIndex - 1] == '\n')
			{
				// 处理接收到的数据
				// 在这里可以添加您的处理代码，例如解析数据、响应等

				// 发送数据回复
				UART_Transmit("Received data: ");
				UART_Transmit((char*)rxBuffer);

				// 清空接收缓冲区和索引
				memset(rxBuffer, 0, sizeof(rxBuffer));
				rxIndex = 0;
			}
			
			// 如果用户按下按键，此时发送数据
			if( Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON  )
			{
				// 串口发送数据
				// 串口上位机与Keil 中文编码方式不同，发送中文会出现乱码（UTF-8 读取 GBK）
				UART_Transmit("Transmit data: Hello World\n");
				printf("This is the %dth times we have received data\n\n", count);

				count++;    // 计数值累加
				LED_RED;    // 亮红灯
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
