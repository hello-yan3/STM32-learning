#ifndef __BASIC_TIM_H
#define	__BASIC_TIM_H

#include "stm32f1xx.h"

// ���嶨ʱ��Ԥ��Ƶ����ʱ��ʵ��ʱ��Ƶ��Ϊ��72MHz/��BASIC_TIMx_PRESCALER+1��
#define BASIC_TIMx_PRESCALER           (72-1) 
// ���嶨ʱ�����ڣ�����ʱ����ʼ������(BASIC_TIMx_PERIOD+1)ֵʱ���¶�ʱ�������ɶ�Ӧ�¼����ж�
#define BASIC_TIMx_PERIOD              (1000-1) 
/*
 * ��ʱʱ����㣺
 * ʱ�����ӣ�1/(72MHz/(BASIC_TIMx_PRESCALER+1)) = 1/(72M/72) = 1us
 * ���嶨ʱ�����ڣ�T_Period = 1us * 1000 =1ms
 */

#define USE_TIM6

#ifdef  USE_TIM6  // ʹ�û�����ʱ��TIM6
#define BASIC_TIMx                     TIM6
#define BASIC_TIM_RCC_CLK_ENABLE()     __HAL_RCC_TIM6_CLK_ENABLE()
#define BASIC_TIM_RCC_CLK_DISABLE()    __HAL_RCC_TIM6_CLK_DISABLE()
#define BASIC_TIM_IRQ                  TIM6_IRQn
#define BASIC_TIM_INT_FUN              TIM6_IRQHandler

#else  // ʹ�û�����ʱ��TIM7

#define BASIC_TIMx                     TIM7
#define BASIC_TIM_RCC_CLK_ENABLE()     __HAL_RCC_TIM7_CLK_ENABLE()
#define BASIC_TIM_RCC_CLK_DISABLE()    __HAL_RCC_TIM7_CLK_DISABLE()
#define BASIC_TIM_IRQ                  TIM7_IRQn
#define BASIC_TIM_INT_FUN              TIM7_IRQHandler

#endif

extern TIM_HandleTypeDef htimx;

void BASIC_TIMx_Init(void);

#endif /* __BASIC_TIM_H */

