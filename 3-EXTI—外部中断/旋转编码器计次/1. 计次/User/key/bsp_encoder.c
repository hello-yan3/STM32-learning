/**
  ******************************************************************************
  * @file    bsp_exti.c
  * @author  fire
  * @version V1.0
  * @date    2016-xx-xx
  * @brief   I/O线中断应用bsp
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F103 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./key/bsp_encoder.h" 
#include "./led/bsp_led.h"

int16_t Encoder_Count;

 /**
  * @brief  配置 PA0 为线中断口，并设置中断优先级
  * @param  无
  * @retval 无
  */
void EXTI_Key_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure; 

    /*开启按键GPIO口的时钟*/
    ENCODER1_INT_GPIO_CLK_ENABLE();
    ENCODER2_INT_GPIO_CLK_ENABLE(); 

    /* 选择按键1的引脚 */ 
    GPIO_InitStructure.Pin = ENCODER1_INT_GPIO_PIN;
    /* 设置引脚为输入模式 */ 
    GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING;	    // 上升沿检测中断
//	  GPIO_InitStructure.Mode = GPIO_MODE_IT_FALLING;	    // 下降沿检测中断
    /* 设置引脚不上拉也不下拉 */
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    /* 使用上面的结构体初始化按键 */
    HAL_GPIO_Init(ENCODER1_INT_GPIO_PORT, &GPIO_InitStructure); 
    /* 配置 EXTI 中断源 到key1 引脚、配置中断优先级*/
		HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
    HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);  // HAL_NVIC_SetPriority(ENCODER1_INT_EXTI_IRQ, 0, 0);
    /* 使能中断 */
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);          // HAL_NVIC_EnableIRQ(ENCODER1_INT_EXTI_IRQ);

    /* 选择按键2的引脚 */ 
    GPIO_InitStructure.Pin = ENCODER2_INT_GPIO_PIN;  
    /* 其他配置与上面相同 */
    HAL_GPIO_Init(ENCODER2_INT_GPIO_PORT, &GPIO_InitStructure);    

		/* 配置中断优先级分组 */
		HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
    /* 配置 EXTI 中断源 到key1 引脚、配置中断优先级*/
    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 1);  // HAL_NVIC_SetPriority(ENCODER2_INT_EXTI_IRQ, 0, 0);
    /* 使能中断 */
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);          // HAL_NVIC_EnableIRQ(ENCODER2_INT_EXTI_IRQ);
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)  // 回调函数程序任意位置定义即可
{
		// ENCODER1(PA0)上升沿，此时ENCODER2(PA1)低电平代表正转，ENCODER2(PA1)高电平代表反转
    if(GPIO_Pin == ENCODER1_INT_GPIO_PIN)
		{
				if(HAL_GPIO_ReadPin(ENCODER2_INT_GPIO_PORT, ENCODER2_INT_GPIO_PIN) == GPIO_PIN_RESET)
				{
						Encoder_Count ++;
				}
				else if(HAL_GPIO_ReadPin(ENCODER2_INT_GPIO_PORT, ENCODER2_INT_GPIO_PIN) == GPIO_PIN_SET)
				{
						Encoder_Count --;
				}
		}
		// ENCODER2(PA1)上升沿，此时ENCODER1(PA0)高电平代表正转，ENCODER1(PA0)低电平代表反转
		else if(GPIO_Pin == ENCODER2_INT_GPIO_PIN)
    {	
				if(HAL_GPIO_ReadPin(ENCODER1_INT_GPIO_PORT, ENCODER1_INT_GPIO_PIN) == GPIO_PIN_SET)
				{
						Encoder_Count ++;
				}
				else if(HAL_GPIO_ReadPin(ENCODER1_INT_GPIO_PORT, ENCODER1_INT_GPIO_PIN) == GPIO_PIN_RESET)
				{
						Encoder_Count --;
				}
		}
}
	
/*********************************************END OF FILE**********************/
