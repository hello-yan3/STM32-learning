/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   PWM测试
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
#include "Encoder.h" 
#include ".\key\bsp_key.h" 
#include "./tim/bsp_general_tim.h"

/*
旋转编码器，用来测量位置、速度或旋转方向的装置，
当其旋转轴旋转时，其输出端可以输出与旋转速度和方向对应的方波信号，读取方波信号的频率和相位信息即可得知旋转轴的速度和方向

如：选择按钮开关、光栅

实验需要使用杜邦线外接选择编码器，这里选择PB6、PB7作为选择编码器引脚：
*/
/*
实验使用PB0、PB1产生脉冲，模拟编码器输出信号
*/

// 定义计时周期, 单位ms
#define TIME     100 
// 方波与编码器计数周期之间比例系数为2，
#define RATIO    4

// 求数字的绝对值
float ABS(float a)
{
	if(a<0) return -a;
	return a;
}

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
	/* 初始化LED */ 
  DEBUG_USART_Config();
	/* 初始化按键 */
	Key_GPIO_Config();
  
	printf("旋转编码器测速测试\n");

	/* 高级定时器PWM输出 */
	TIMx_Configuration();   	
	/* 定时器通道3输出PWM */
	HAL_TIM_OC_Start_IT(&TIM_TimeBaseStructure_OC,GENERAL_TIM_CHANNELx1);   // GENERAL_TIM_CHANNELx1  TIM_CHANNEL_3
	/* 定时器通道4输出PWM */
	HAL_TIM_OC_Start_IT(&TIM_TimeBaseStructure_OC,GENERAL_TIM_CHANNELx2);   // GENERAL_TIM_CHANNELx2  TIM_CHANNEL_4

	/* 定时器5启动编码器输入 */
	TIM_Encoder_Mode_Config();   // 编码器配置
	HAL_TIM_Encoder_Start(&TIM_TimeBaseStructure, TIM_CHANNEL_ALL);  // TIM_CHANNEL_ALL表示CHANNEL1和CHANNEL2同时被选择

	int16_t Encoder_Count;  // CNT寄存器值
	float velocity;       // 速度计算
	uint8_t direction;    // 旋转方向
 	
  /* 无限循环 */
  while (1)
  {
		
		/* 按键1按下，方波周期增加 */
		/* 检测是否有按键按下 */
		if( Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON )  
		{	 
			OC_Pulse+=100;
			if(OC_Pulse > 2000)   OC_Pulse = 1000;  
		}
		
		/* 按键2按下，方波方向改变 */
		/* 检测是否有按键按下 */
		if( Key_Scan(KEY2_GPIO_PORT,KEY2_PIN) == KEY_ON )  
		{	 
			turn = 1;
		}
		
		// 旋转编码器的结果保存在计数器CNT里
		// 计时时间: 50ms
		HAL_Delay(TIME);
		// 这里使用__HAL_TIM_GET_COUNTER获取CNT计数器的值
		Encoder_Count = (int16_t)__HAL_TIM_GET_COUNTER(&TIM_TimeBaseStructure);
		direction = __HAL_TIM_IS_TIM_COUNTING_DOWN(&TIM_TimeBaseStructure);
		// 已知编码器值，又知编码器所对应角度，即可换算得到速度
		// 因为是软件模拟编码器信号，所以这里计算PWM波周期
		velocity = (float)Encoder_Count/TIME;
		
		printf("编码器当前旋转方向: %d, 1表示计数器向下计数, 0表示向上计数\n", direction);
		printf("编码器计数器值为：%d \n",Encoder_Count);
		printf("当前编码器累计经过的次数为：%f 方波 \n", ABS(velocity));
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
