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
  
#include "./exti/bsp_exti.h"
#include "./led/bsp_led.h"
#include "./usart/bsp_debug_usart.h"

// 中断标志位，EXTI_FLAG = 1，表示进入中断
uint8_t EXTI_FLAG = 0;

 /**
  * @brief  配置 PA0 为线中断口，并设置中断优先级
	* @note   IRQ默认高电平，低电平触发中断；因此这里选择：下降沿进入中断
  * @param  无
  * @retval 无
  */
void EXTI_Key_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure; 

    /*开启按键GPIO口的时钟*/
    EXTI_INT_GPIO_CLK_ENABLE();

    /* 选择按键1的引脚 */ 
    GPIO_InitStructure.Pin = EXTI_INT_GPIO_PIN;
    /* 设置引脚为输入模式 */ 
//    GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING;	    // 上升沿检测中断
	  GPIO_InitStructure.Mode = GPIO_MODE_IT_FALLING;	    // 下降沿检测中断
    /* 设置引脚不上拉也不下拉 */
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    /* 使用上面的结构体初始化按键 */
    HAL_GPIO_Init(EXTI_INT_GPIO_PORT, &GPIO_InitStructure); 
	
		/* 配置中断优先级分组 */
		HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
    /* 配置 EXTI 中断源 到key1 引脚、配置中断优先级*/
    HAL_NVIC_SetPriority(EXTI_INT_EXTI_IRQ, 0, 0);  // HAL_NVIC_SetPriority(KEY1_INT_EXTI_IRQ, 0, 0);
    /* 使能中断 */
    HAL_NVIC_EnableIRQ(EXTI_INT_EXTI_IRQ);          // HAL_NVIC_EnableIRQ(KEY1_INT_EXTI_IRQ);
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)  // 回调函数程序任意位置定义即可
{
    if(GPIO_Pin == EXTI_INT_GPIO_PIN)
        EXTI_FLAG = 1;  
}
	
/*********************************************END OF FILE**********************/
