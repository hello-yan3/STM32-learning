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
  
#include "./BSP/TIMER/basic_tim.h"
#include "./BSP/LED/led.h"
#include "./SYSTEM/usart/usart.h"

TIM_HandleTypeDef htim6;    // // TIM时基结构体
TIM_HandleTypeDef htim7;    // // TIM时基结构体

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
void BASIC_TIM6_Init(void)
{
  htim6.Instance = BASIC_TIM6;                  // TIM6
  htim6.Init.Prescaler = BASIC_TIMx_PRESCALER;  // 预分频系数，71
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;  // 计数器模式，向上计数，基本定时器只能选择向上计数
  htim6.Init.Period = BASIC_TIMx_PERIOD;        // 定时器定时周期1000 
  HAL_TIM_Base_Init(&htim6);

  HAL_TIM_Base_Start_IT(&htim6);                       /* 使能定时器x和定时器更新中断 */
}

void BASIC_TIM7_Init(void)
{
  htim7.Instance = BASIC_TIM7;                  // TIM6
  htim7.Init.Prescaler = BASIC_TIMx_PRESCALER;  // 预分频系数，71
  htim7.Init.CounterMode = TIM_COUNTERMODE_UP;  // 计数器模式，向上计数，基本定时器只能选择向上计数
  htim7.Init.Period = BASIC_TIMx_PERIOD;        // 定时器定时周期1000 
  HAL_TIM_Base_Init(&htim7);
  
  HAL_TIM_Base_Start_IT(&htim7);                       /* 使能定时器x和定时器更新中断 */
}

/**
 * @brief  时钟与中断配置
 * @param  无
 * @retval 无
 */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim)
{
    /* 配置中断优先级分组 */
    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
  
    if (htim->Instance == BASIC_TIM6)
    {
        BASIC_TIM6_RCC_CLK_ENABLE();                /* 使能TIMx时钟 */
        HAL_NVIC_SetPriority(BASIC_TIM6_IRQ, 6, 0); /* 抢占6，子优先级0 */
        HAL_NVIC_EnableIRQ(BASIC_TIM6_IRQ);         /* 开启ITMx中断 */
    }
    if (htim->Instance == BASIC_TIM7)
    {
        BASIC_TIM7_RCC_CLK_ENABLE();                /* 使能TIMx时钟 */
        HAL_NVIC_SetPriority(BASIC_TIM7_IRQ, 3, 0); /* 抢占3，子优先级0 */
        HAL_NVIC_EnableIRQ(BASIC_TIM7_IRQ);         /* 开启ITMx中断 */
    }
  
}

/**
 * @brief       基本定时器TIMX中断服务函数
 * @param       无
 * @retval      无
 */
void BASIC_TIM6_INT_FUN(void)
{
    HAL_TIM_IRQHandler(&htim6);  /* 定时器回调函数 */
}

void BASIC_TIM7_INT_FUN(void) 
{
    HAL_TIM_IRQHandler(&htim7);  /* 定时器回调函数 */
}


// 中断回调函数
// 进入中断后，执行中断回调函数操作
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == BASIC_TIM6)
    {
        printf("TIM6正在运行！！优先级为6 \r\n");
    }
    if (htim->Instance == BASIC_TIM7)
    {
        printf("TIM7正在运行！！优先级为3 \r\n");
    }
}

/*********************************************END OF FILE**********************/
