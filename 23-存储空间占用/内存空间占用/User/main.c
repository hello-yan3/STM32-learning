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
#include <stdlib.h>
#include <string.h>


int a = 0;    // 全局初始化区
char *p1;     // 全局未初始化区
const int e1 = 12;   // 常量区

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
  const int e = 12;   // 常量区
  int b;     // 栈
  char s[] = "abc";     // 栈
  char *p2;             // 栈
  char *p3 = "123456";  // 123456\0在常量区，p3在栈上。
  static int c =0;      // 全局（静态）初始化区
  p1 = (char *)malloc(10);   
  p2 = (char *)malloc(20);
  // 分配得来得10和20字节的区域就在堆区。
  strcpy(p1, "123456");    // 123456\0放在常量区，编译器可能会将它与p3所指向的"123456"优化成一个地方。
  
  /**----------------------------------------------------**/
  
  HAL_Init();        
  /* 配置系统时钟为72MHz */ 
  SystemClock_Config();
	   
  /*初始化USART 配置模式为 115200 8-N-1，中断接收*/
  DEBUG_USART_Config();
  LED_GPIO_Config();
	
  /**----------------------------------------------------**/
  
  printf("a变量的地址在%p  \r\n", &a);         // SRAM 全局变量区       a变量的地址在20000010  
  printf("&p1变量的地址在%p  \r\n", &p1);      // SRAM 全局变量区      &p1变量的地址在20000014  
  printf("*p1变量的地址在%p  \r\n", p1);       // p1指针指向的地址     *p1变量的地址在20000070  
  printf("e1变量的地址在%p  \r\n", &e1);       // FLASH中的文字常量区   e1变量的地址在08002414  

  printf("e变量的地址在%p  \r\n", &e);         // SRAM 栈区            e变量的地址在20000660  
  printf("b变量的地址在%p  \r\n", &b);         // SRAM 栈区            e变量的地址在2000065c (0x60-0x04 = 0x5c，栈上数据按顺序排布，栈向下生长) 
  printf("&s变量的地址在%p  \r\n", &s);        // SRAM 栈区           &s变量的地址在20000658 (下面栈区地址同理)
  printf("s[]变量的地址在%p  \r\n", s);        // SRAM 栈区          s[]变量的地址在20000658  
  printf("&p2变量的地址在%p  \r\n", &p2);      // SRAM 栈区          &p2变量的地址在20000654  
  printf("*p2变量的地址在%p  \r\n", p2);       // p2指针指向的地址     *p2变量的地址在20000080  
  printf("&p3变量的地址在%p  \r\n", &p3);      // SRAM 栈区           &p3变量的地址在20000650  
  printf("*p3变量的地址在%p  \r\n", p3);       // p3指针指向的地址(p3指向文字常量区)     *p3变量的地址在0800221c  
  printf("123456 变量的地址在%p  \r\n", "123456");   // FLASH中的文字常量区   123456 变量的地址在0800221c  
  printf("c变量的地址在%p  \r\n", &c);         // SRAM 全局变量区       c变量的地址在20000018 (全局变量区数据也是按顺序排布存储的)
  
  printf("&p1变量的地址在%p  \r\n", &p1);      // SRAM 全局变量区      &p1变量的地址在20000014  
  printf("*p1变量的地址在%p  \r\n", p1);       // p1指针指向的地址     *p1变量的地址在20000070 (其实就是在堆区)
  printf("&p2变量的地址在%p  \r\n", &p2);      // SRAM 栈区            &p2变量的地址在20000654  
  printf("*p2变量的地址在%p  \r\n", p2);       // p2指针指向的地址     *p2变量的地址在20000080  (其实就是在堆区) 

  printf("123456  变量的地址在%p  \r\n", "123456");   // FLASH中的文字常量区   123456 变量的地址在0800221c (编译器优化，与上面的 123456 是同一个地址) 
  printf("&p1变量的地址在%p  \r\n", &p1);      // SRAM 全局变量区      &p1变量的地址在20000014  
  printf("p1变量的地址在%p  \r\n", p1);        // p1指针指向的地址      p1变量的地址在20000070   (p1还是在堆区) 
  
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
