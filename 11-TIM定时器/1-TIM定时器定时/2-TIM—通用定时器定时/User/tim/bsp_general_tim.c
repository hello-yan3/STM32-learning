/**
  ******************************************************************************
  * @file    bsp_advance_tim.c
  * @author  STMicroelectronics
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   高级控制定时器定时范例
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F103 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./tim/bsp_general_tim.h"

extern uint32_t time;

// TIM时基结构体
TIM_HandleTypeDef TIM_TimeBaseStructure;

/*
 * 注意：TIM_TimeBaseInitTypeDef结构体里面有5个成员，TIM6和TIM7的寄存器里面只有
 * TIM_Prescaler和TIM_Period，所以使用TIM6和TIM7的时候只需初始化这两个成员即可，
 * 另外三个成员是通用定时器和高级定时器才有.
 *-----------------------------------------------------------------------------
 *typedef struct
 *{ TIM_Prescaler            都有
 *	TIM_CounterMode			     TIMx,x[6,7]没有，其他都有
 *  TIM_Period               都有
 *  TIM_ClockDivision        TIMx,x[6,7]没有，其他都有
 *  TIM_RepetitionCounter    TIMx,x[1,8,15,16,17]才有
 *}TIM_TimeBaseInitTypeDef; 
 *-----------------------------------------------------------------------------
 * 定时时间计算：
 * 时钟因子：1/(72MHz/(TIMx_PRESCALER+1)) = 1/(72M/72) = 1us
 * 定义定时器周期：T_Period = 1us * 1000 =1ms
 * 重复计数器TIM_REPETITIONCOUNTER = 10，所以实际计时时间T_Period = 1ms * 10 =10ms
 */
 void TIM_Mode_Config(void)
{
	// TIM时钟源选择结构体
	TIM_ClockConfigTypeDef sClockSourceConfig;
  // TIM主模式触发DAC配置结构体
	TIM_MasterConfigTypeDef sMasterConfig;
  
  TIM_TimeBaseStructure.Instance = GENERAL_TIMx;      // TIM1
  
  // 时钟预分频数
  TIM_TimeBaseStructure.Init.Prescaler = GENERAL_TIM_PRESCALER;  // (72-1)
  // 计数器计数模式，设置为向上计数
  TIM_TimeBaseStructure.Init.CounterMode = TIM_COUNTERMODE_UP;
  // 自动重装载寄存器的值，累计TIM_Period+1个频率后产生一个更新或者中断
  TIM_TimeBaseStructure.Init.Period = GENERAL_TIM_PERIOD;        // (1000-1) 
  // 时钟分频因子 
  TIM_TimeBaseStructure.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;// 一般默认选择一分频即可
  // 初始化定时器
  HAL_TIM_Base_Init(&TIM_TimeBaseStructure);

	// 时钟源选择 TIM 内部时钟源（一般程序默认选择内部时钟源即可）
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;  
  HAL_TIM_ConfigClockSource(&TIM_TimeBaseStructure, &sClockSourceConfig);

	// 禁用主模式触发DAC功能
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&TIM_TimeBaseStructure, &sMasterConfig);
}

 /**
  * @brief  时钟与中断配置
  * @param  无
  * @retval 无
  */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{  
    /* 基本定时器外设时钟使能 */
    GENERAL_TIM_RCC_CLK_ENABLE();   // __HAL_RCC_TIM1_CLK_ENABLE()

    /* 外设中断配置 */
    HAL_NVIC_SetPriority(GENERAL_TIM_IRQn , 1, 0);   // TIM1_UP_IRQn
    HAL_NVIC_EnableIRQ(GENERAL_TIM_IRQn );           // TIM1_UP_IRQn
}

// 中断回调函数
// 进入中断后，执行中断回调函数操作
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  time++;
}

/*********************************************END OF FILE**********************/
