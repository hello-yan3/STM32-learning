#ifndef __ADVANCE_TIM_H
#define	__ADVANCE_TIM_H

#include "stm32f1xx.h"

// ��ʱ��Ԥ��Ƶ
#define GENERAL_TIM_PRESCALER            (72-1)
// �߼���ʱ���ظ������Ĵ���ֵ
#define GENERAL_TIM_REPETITIONCOUNTER    (10-1)
// ��ʱ������
#define GENERAL_TIM_PERIOD               (1000-1) 


#define GENERAL_TIMx                     TIM2
#define GENERAL_TIM_RCC_CLK_ENABLE()     __HAL_RCC_TIM2_CLK_ENABLE()
#define GENERAL_TIM_RCC_CLK_DISABLE()    __HAL_RCC_TIM2_CLK_DISABLE()

#define GENERAL_TIM_IRQn                 TIM2_IRQn
#define GENERAL_TIM_IRQHandler           TIM2_IRQHandler


extern TIM_HandleTypeDef TIM_TimeBaseStructure;

void TIM_Mode_Config(void);

#endif /* __ADVANCE_TIM_H */

