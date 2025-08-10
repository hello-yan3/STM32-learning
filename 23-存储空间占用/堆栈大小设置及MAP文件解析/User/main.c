/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   测试led
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
#include "./led/bsp_led.h"
#include ".\key\bsp_key.h" 

/*
00. 堆栈大小设置！！！
STM32（F407）—— 堆栈讲解
https://blog.csdn.net/CynalFly/article/details/114626136
https://blog.csdn.net/lighthear/article/details/69568396
堆栈大小设置问题
https://www.armbbs.cn/forum.php?mod=viewthread&tid=95883
STM32堆栈与uCOS堆栈
https://blog.csdn.net/qq_23922117/article/details/79400051
c++五种内存分配、堆与栈区别
https://blog.csdn.net/qq_39885372/article/details/103886195

01. 详见：STM32 MAP文件浅析
Program Size: Code=3748 RO-data=360 RW-data=16 ZI-data=1024  
* RO：Read Only 的缩写，包括只读数据（RO data）和代码（RO code）两部分内容，占用 FLASH 空间
* RW：Read Write 的缩写，包含可读写数据（RW data，有初值，且不为 0），占用FLASH（存储初值）和 RAM（读写操作）
* ZI：Zero initialized 的缩写，包含初始化为 0 的数据（ZI data），占用 RAM 空间。
* .text：相当于 RO code
* .constdata：相当于 RO data
* .bss：相当于 ZI data
* .data：相当于 RW data

1. 描述了各个文件之间的函数调用关系(Section Cross References)，如：
main.o(i.main) refers to stm32f1xx_hal.o(i.HAL_Init) for HAL_Init
main.c 调用了 stm32f1xx_hal.c 的 HAL_Init 函数
2. 描述了工程中由于未被调用而被删除的冗余程序段(Removing Unused input sections from the image)，如：
Removing stm32f1xx_ll_utils.o(i.LL_RCC_HSI_IsReady), (16 bytes).
移除 LL_RCC_HSI_IsRead() 函数
3. 映像符号表（Image Symbol Table），描述了被引用的各个符号（程序段/数据）在存储器中的存储地址、类型、大小等信息。
4. 映像内存分布图（Memory Map of the image）
5. 映像组件大小（Image component sizes）
给出了整个映像所有代码（.o）占用空间的汇总信息
      Code (inc. data)   RO Data    RW Data    ZI Data      Debug   Object Name

       148         14          0          0          0       1705   bsp_key.o
       128          8          0          0          0       1066   bsp_led.o
       172         14          0          0          0       1159   main.o
        36          8        304          0       1024        968   startup_stm32f103xe.o
       152         32          0          9          0       6413   stm32f1xx_hal.o
       272         22          0          0          0      29246   stm32f1xx_hal_cortex.o
       874         40          0          0          0       4292   stm32f1xx_hal_gpio.o
         2          0          0          0          0        574   stm32f1xx_hal_msp_template.o
      1708         84          0          0          0       4760   stm32f1xx_hal_rcc.o
        32          0          0          0          0       4486   stm32f1xx_it.o
        88         18         24          4          0     441191   system_stm32f1xx.o

Execution Region RW_IRAM1 (Exec base: 0x20000000, Load base: 0x0800100c, Size: 0x00000410, Max: 0x00010000, ABSOLUTE)
栈 起始地址：0x20000000  SRAM 最大值 64K(0x10000)

堆 + 栈 + 全局变量区域 = RAM

*/

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
    /*初始化按键*/
  Key_GPIO_Config();
	/* 轮询按键状态，若按键按下则反转LED */ 
	while(1)                            
	{	   
		if( Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON  )
		{
			/*LED1反转*/
			LED1_TOGGLE;
		}   
    
    if( Key_Scan(KEY2_GPIO_PORT,KEY2_PIN) == KEY_ON  )
		{
			/*LED2反转*/
			LED2_TOGGLE;
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
