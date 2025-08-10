/**
  ******************************************************************************
  * @file    bsp_exti.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   I/O线中断应用bsp
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 指南者 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./exti/bsp_exti.h"
#include "./led/bsp_led.h"
#include "./usart/bsp_debug_usart.h"

extern __IO uint16_t time; // ms 计时变量 
extern uint16_t continue_time;

 /**
  * @brief  配置 PF7 为线中断口，并设置中断优先级
  * @param  无
  * @retval 无
  */
void EXTI_Pxy_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure; 

    /*开启按键GPIO口的时钟*/
    EXTI_INT_GPIO_CLK_ENABLE();
 	  __HAL_RCC_AFIO_CLK_ENABLE();

    /* 选择按键1的引脚 */ 
    GPIO_InitStructure.Pin = EXTI_GPIO_PIN;
    /* 设置引脚为输入模式 */ 
    GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING;	    // 上升沿检测中断
//	  GPIO_InitStructure.Mode = GPIO_MODE_IT_FALLING;	    // 下降沿检测中断
    /* 设置引脚不上拉也不下拉 */
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    /* 使用上面的结构体初始化按键 */
    HAL_GPIO_Init(EXTI_GPIO_PORT, &GPIO_InitStructure); 
	
		/* 配置中断优先级分组 */
		HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
    /* 配置 EXTI 中断源 到key1 引脚、配置中断优先级*/
    HAL_NVIC_SetPriority(EXTI_IRQ, 0, 0);  // HAL_NVIC_SetPriority(KEY1_INT_EXTI_IRQ, 0, 0);
    /* 使能中断 */
    HAL_NVIC_EnableIRQ(EXTI_IRQ);          // HAL_NVIC_EnableIRQ(KEY1_INT_EXTI_IRQ);
}

// 中断标志位，当进入中断次数>count时，中断标志位置1
// 注意：进入中断时间间隔与采样率有关，
// 即DMP初始化中的：mpu_set_sample_rate(DEFAULT_MPU_HZ);  
// 应该是MPU6050初始化中的：MPU6050_WriteReg(MPU6050_RA_SMPLRT_DIV , 0x07);	但是修改这句话没有效果，

// count 变量作用：进入几次中断回调函数后进入主程序
uint8_t EXTI_flag;
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)  // 回调函数程序任意位置定义即可
{	
	static uint8_t count=0;     
	if(GPIO_Pin == EXTI_GPIO_PIN)
	{
		LED2_TOGGLE;
		count++;
		printf("第%d次进入中断函数所经历过的时间：%d \r\n", count ,time);
		continue_time += time;
		time = 0;
	}
	if(count>=10)  
	{
		EXTI_flag = 1;
		count = 0;
	}
}

/*********************************************END OF FILE**********************/
