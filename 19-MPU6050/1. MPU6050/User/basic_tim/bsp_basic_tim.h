#ifndef __BASIC_TIM_H
#define	__BASIC_TIM_H

#include "stm32f1xx.h"

// 定义定时器预分频，定时器实际时钟频率为：72MHz/（BASIC_TIMx_PRESCALER+1）
#define BASIC_TIMx_PRESCALER           (72-1) 
// 定义定时器周期，当定时器开始计数到(BASIC_TIMx_PERIOD+1)值时更新定时器并生成对应事件和中断
#define BASIC_TIMx_PERIOD              (1000-1) 
/*
 * 定时时间计算：
 * 时钟因子：1/(72MHz/(BASIC_TIMx_PRESCALER+1)) = 1/(72M/72) = 1us
 * 定义定时器周期：T_Period = 1us * 1000 =1ms
 */

#define USE_TIM6

#ifdef  USE_TIM6  // 使用基本定时器TIM6
#define BASIC_TIMx                     TIM6
#define BASIC_TIM_RCC_CLK_ENABLE()     __HAL_RCC_TIM6_CLK_ENABLE()
#define BASIC_TIM_RCC_CLK_DISABLE()    __HAL_RCC_TIM6_CLK_DISABLE()
#define BASIC_TIM_IRQ                  TIM6_IRQn
#define BASIC_TIM_INT_FUN              TIM6_IRQHandler

#else  // 使用基本定时器TIM7

#define BASIC_TIMx                     TIM7
#define BASIC_TIM_RCC_CLK_ENABLE()     __HAL_RCC_TIM7_CLK_ENABLE()
#define BASIC_TIM_RCC_CLK_DISABLE()    __HAL_RCC_TIM7_CLK_DISABLE()
#define BASIC_TIM_IRQ                  TIM7_IRQn
#define BASIC_TIM_INT_FUN              TIM7_IRQHandler

#endif

extern TIM_HandleTypeDef htimx;

void BASIC_TIMx_Init(void);

#endif /* __BASIC_TIM_H */

