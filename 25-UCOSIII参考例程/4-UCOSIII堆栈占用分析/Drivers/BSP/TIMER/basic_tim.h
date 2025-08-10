#ifndef __BASIC_TIM_H
#define	__BASIC_TIM_H

#include "stm32f1xx.h"

// ���嶨ʱ��Ԥ��Ƶ����ʱ��ʵ��ʱ��Ƶ��Ϊ��72MHz/��BASIC_TIMx_PRESCALER+1��
#define BASIC_TIMx_PRESCALER           (7200-1) 
// ���嶨ʱ�����ڣ�����ʱ����ʼ������(BASIC_TIMx_PERIOD+1)ֵʱ���¶�ʱ�������ɶ�Ӧ�¼����ж�
#define BASIC_TIMx_PERIOD              (10000-1) 
/*
 * ��ʱʱ����㣺
 * ʱ�����ӣ�1/(72MHz/(BASIC_TIMx_PRESCALER+1)) = 1/(72M/7200) = 100us
 * ���嶨ʱ�����ڣ�T_Period = 100us * 10000 =1s
 */


#define BASIC_TIM6                      TIM6
#define BASIC_TIM6_RCC_CLK_ENABLE()     __HAL_RCC_TIM6_CLK_ENABLE()
#define BASIC_TIM6_RCC_CLK_DISABLE()    __HAL_RCC_TIM6_CLK_DISABLE()
#define BASIC_TIM6_IRQ                  TIM6_IRQn
#define BASIC_TIM6_INT_FUN              TIM6_IRQHandler

#define BASIC_TIM7                      TIM7
#define BASIC_TIM7_RCC_CLK_ENABLE()     __HAL_RCC_TIM7_CLK_ENABLE()
#define BASIC_TIM7_RCC_CLK_DISABLE()    __HAL_RCC_TIM7_CLK_DISABLE()
#define BASIC_TIM7_IRQ                  TIM7_IRQn
#define BASIC_TIM7_INT_FUN              TIM7_IRQHandler

extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim7;

void BASIC_TIM6_Init(void);
void BASIC_TIM7_Init(void);

#endif /* __BASIC_TIM_H */

