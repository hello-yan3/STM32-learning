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
#include "./tim/bsp_general_tim_OC.h"

/*
实验现象：
使用TIM3 输出 PWM波(输出比较) PA7
使用TIM8 测量 PWM波(输入捕获) PC6
要求：PA7与PC6连接在一起
*/
/*
参考讲解：
https://blog.csdn.net/gongyuan073/article/details/81080011
https://blog.csdn.net/weixin_44793491/article/details/126852975
https://blog.csdn.net/qq_52111026/article/details/117563000
*/

__IO uint16_t CCR1;  // CCR1寄存器值
__IO uint16_t CCR2;	 // CCR2寄存器值


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
	TIMx_OC_Configuration();
	TIMx_IC_Configuration();

	/* 定时器3通道2输出PWM */
	HAL_TIM_PWM_Start(&TIM_TimeBaseStructure_OC,GENERAL_TIM_OC_CHANNELx1);  // GENERAL_TIM_CHANNELx1  TIM_CHANNEL_2
	/* 启动定时器通道输入捕获并开启中断 */
  HAL_TIM_IC_Start_IT(&TIM_TimeBaseStructure_IC,GENERAL_TIM_IC_CHANNELx1);	 // GENERAL_TIM_IC_CHANNELx1  TIM_CHANNEL_1
	HAL_TIM_IC_Start_IT(&TIM_TimeBaseStructure_IC,GENERAL_TIM_IC_CHANNELx2);	 // GENERAL_TIM_IC_CHANNELx2  TIM_CHANNEL_2
	
	float   Freq;  // 频率
	float   Duty;  // 占空比	
	int16_t Pulse = GENERAL_TIM_OC_CH_CCR;
  
  while(1)
  {   
		/* 按键1按下，通道2占空比增加 */
		if(Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON )  
		{	 
			Pulse+=200;
			if(Pulse > 5000)    Pulse = 0;   // 如果超过5000，超范围，Pulse回归下限0
			// 设置此时占空比为Pulse
			__HAL_TIM_SET_COMPARE(&TIM_TimeBaseStructure_OC, GENERAL_TIM_OC_CHANNELx1, Pulse);   // GENERAL_TIM_CHANNELx2  TIM_CHANNEL_3
			// 获得此时占空比
		}

		/* 按键1按下，通道2占空比增加 */
		if(Key_Scan(KEY2_GPIO_PORT,KEY2_PIN) == KEY_ON )  
		{	 
			Pulse-=200;
			if(Pulse < 0)    Pulse = 5000;   // 如果超过0，超范围，Pulse回归上限5000
			// 设置此时占空比为Pulse
			__HAL_TIM_SET_COMPARE(&TIM_TimeBaseStructure_OC, GENERAL_TIM_OC_CHANNELx1, Pulse);   // GENERAL_TIM_CHANNELx2  TIM_CHANNEL_3
			// 获得此时占空比
		}
		
		Freq = (float)72000000/(GENERAL_TIM_IC_PRESCALER+1)/(CCR1+1);  // 1/((CCR1+1)/(72000000/(GENERAL_TIM_IC_PRESCALER+1))) = 1/((CCR1+1)/(1000000))
		Duty = (float)(CCR2+1)/(CCR1+1);
		printf("此时CCR1为%d \n",CCR1);
		printf("此时CCR2为%d \n",CCR2);
		printf("此时PWM波频率为%fHz\n",Freq);
		printf("此时占空比为%f \n",Duty);
		printf("\n\n");

		HAL_Delay(500);
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
