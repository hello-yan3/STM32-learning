/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2020-xx-xx
  * @brief   TIM—高级定时器互补输出
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 F103-拂晓 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdlib.h>
#include "./usart/bsp_debug_usart.h"
#include ".\key\bsp_key.h" 
#include "./tim/bsp_general_tim_IC.h"
#include "./ultrasonic/ultrasonic.h"


/*
输入捕获-中断修改捕获方向——常用于测量高电平/低电平持续时间

实验现象：
这里通过TIM输入捕获，实现超声波测距功能
触发引脚Trig: PC6
响应引脚Echo: PA7
模块型号为 HC-SR04：
工作原理：
给触发引脚一个至少 10us 的脉冲信号，此时模块会发出超声波，回波引脚会由低电平变为高电平，
若模块接受到弹回来的超声波，回波口就会从高电平跳变到低电平，
通过测量高电平的持续时间，通过计算公式就可以算出距离。

*/

/*
https://zhuanlan.zhihu.com/p/145635309
https://blog.csdn.net/as480133937/article/details/99407485
*/

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void) 
{
	HAL_Init();
  /* 初始化系统时钟为72MHz */
  SystemClock_Config();

  /* 初始化串口 */
  DEBUG_USART_Config();
  /* 初始化按键 */
	Key_GPIO_Config();
	
  printf("\r\nPWM输入捕获实验\r\n");
	MX_GPIO_Init();
	TIMx_IC_Configuration();

	// 开启定时器中断
	HAL_TIM_IC_Start_IT(&TIM_TimeBaseStructure_IC,GENERAL_TIM_IC_CHANNELx1);    // GENERAL_TIM_IC_CHANNELx1  TIM_CHANNEL_1
  HAL_TIM_Base_Start_IT(&TIM_TimeBaseStructure_IC);         // 两个中断都要打开                  

  while(1)
  {   
		/* 按键1按下，进行超声波测距 */
		if(Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON )  
		{	 
			Read_Distane();  
		}

		HAL_Delay(100);
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
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
