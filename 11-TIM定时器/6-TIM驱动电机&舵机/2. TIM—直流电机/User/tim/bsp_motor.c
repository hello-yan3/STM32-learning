/**
  ******************************************************************************
  * @file    bsp_motor.c
  * @author  Lacie
  * @version V1.0
  * @date    2023-xx-xx
  * @brief   电机驱动函数接口
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 STM32 F103 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */

#include "./tim/bsp_general_tim.h"
#include "./tim/bsp_motor.h"

/**
  * @brief  初始化控制电机的IO
  * @param  无
  * @retval 无
  */
void Motor_GPIO_Config(void)
{				
    /*定义一个GPIO_InitTypeDef类型的结构体*/
    GPIO_InitTypeDef  GPIO_InitStruct;

    /*开启LED相关的GPIO外设时钟*/
    Motor_GPIO_CLK_ENABLE();
  
    /*选择要控制的GPIO引脚*/															   
    GPIO_InitStruct.Pin = Motor1_PIN|Motor2_PIN;	

    /*设置引脚的输出类型为推挽输出*/
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;  

    /*设置引脚为上拉模式*/
    GPIO_InitStruct.Pull  = GPIO_PULLUP;

    /*设置引脚速率为高速 */   
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

    /*调用库函数，使用上面配置的GPIO_InitStructure初始化GPIO*/
    HAL_GPIO_Init(Motor_GPIO_PORT, &GPIO_InitStruct);	
    
		HAL_GPIO_WritePin(Motor_GPIO_PORT, Motor1_PIN ,GPIO_PIN_SET);   // 引脚1默认高电平
		HAL_GPIO_WritePin(Motor_GPIO_PORT, Motor2_PIN ,GPIO_PIN_SET);
}

/**
  * @brief  调整电机速度
	* @param  Speed电机速度: 电机速度
	*         范围: -1000~1000
  * @retval 无
  */
void Motor_SetSpeed(int16_t Speed)
{
	if (Speed >= 0)
	{
		HAL_GPIO_WritePin(Motor_GPIO_PORT, Motor1_PIN ,GPIO_PIN_SET);
		HAL_GPIO_WritePin(Motor_GPIO_PORT, Motor2_PIN ,GPIO_PIN_RESET);
		__HAL_TIM_SET_COMPARE(&TIM_TimeBaseStructure, GENERAL_TIM_CHANNELx1, Speed );  
//		__HAL_TIM_SET_COMPARE(&TIM_TimeBaseStructure, GENERAL_TIM_CHANNELx2, Speed );  
	}
	else
	{
		HAL_GPIO_WritePin(Motor_GPIO_PORT, Motor1_PIN ,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(Motor_GPIO_PORT, Motor2_PIN ,GPIO_PIN_SET);
		__HAL_TIM_SET_COMPARE(&TIM_TimeBaseStructure, GENERAL_TIM_CHANNELx1, -Speed );  
//		__HAL_TIM_SET_COMPARE(&TIM_TimeBaseStructure, GENERAL_TIM_CHANNELx2, Speed );  
	}
}
/*********************************************END OF FILE**********************/
