/**
  ******************************************************************************
  * @file    bsp_basic_tim.c
  * @author  STMicroelectronics
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   基本定时器定时范例
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F103 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./basic_tim/bsp_basic_tim.h"

extern __IO uint16_t time ; // ms 计时变量
TIM_HandleTypeDef htimx;    // // TIM时基结构体

/*
 * 注意：TIM_TimeBaseInitTypeDef结构体里面有5个成员，TIM6和TIM7的寄存器里面只有
 * TIM_Prescaler和TIM_Period，所以使用TIM6和TIM7的时候只需初始化这两个成员即可，
 * 另外三个成员是通用定时器和高级定时器才有.
 *-----------------------------------------------------------------------------
 * TIM_Prescaler         都有
 * TIM_CounterMode			 TIMx,x[6,7]没有，其他都有（基本定时器）
 * TIM_Period            都有
 * TIM_ClockDivision     TIMx,x[6,7]没有，其他都有(基本定时器)
 * TIM_RepetitionCounter TIMx,x[1,8]才有(高级定时器)
 *-----------------------------------------------------------------------------
 * 定时时间计算：
 * 时钟因子：1/(72MHz/(BASIC_TIMx_PRESCALER+1)) = 1/(72M/72) = 1us
 * 定义定时器周期：T_Period = 1us * 1000 =1ms
 */
void BASIC_TIMx_Init(void)
{
	// 基本定时器，拥有定时中断、主模式触发DAC的功能
	// TIM_MasterConfigTypeDef，与主模式触发DAC相关
	// 这里只用于定时，用不到该功能，
	// 因此可不对此结构体进行配置
	TIM_MasterConfigTypeDef sMasterConfig;        // 此结构体及其配置可直接注释

  htimx.Instance = BASIC_TIMx;                  // TIM6
  htimx.Init.Prescaler = BASIC_TIMx_PRESCALER;  // 预分频系数，71
  htimx.Init.CounterMode = TIM_COUNTERMODE_UP;  // 计数器模式，向上计数，基本定时器只能选择向上计数
  htimx.Init.Period = BASIC_TIMx_PERIOD;        // 定时器定时周期1000 
  HAL_TIM_Base_Init(&htimx);

	// 禁用主模式触发DAC功能，可直接注释
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&htimx, &sMasterConfig);
	
	/* 在中断模式下启动定时器 */
	HAL_TIM_Base_Start_IT(&htimx);
}

/**
 * @brief  时钟与中断配置
 * @param  无
 * @retval 无
 */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{
   /* 基本定时器外设时钟使能 */
    BASIC_TIM_RCC_CLK_ENABLE();                 // __HAL_RCC_TIM6_CLK_ENABLE()

		/* 配置中断优先级分组 */
		HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
    /* 外设中断配置 */
    HAL_NVIC_SetPriority(BASIC_TIM_IRQ, 1, 0);  // TIM6_IRQn
    HAL_NVIC_EnableIRQ(BASIC_TIM_IRQ);          // TIM6_IRQn
}

// 中断回调函数
// 进入中断后，执行中断回调函数操作
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  time++;
}

/*********************************************END OF FILE**********************/
