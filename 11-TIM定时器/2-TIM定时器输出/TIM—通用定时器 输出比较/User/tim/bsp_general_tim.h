#ifndef __GENERAL_TIM_H
#define	__GENERAL_TIM_H

#include "stm32f1xx.h"

/*
 * ��ʱʱ����㣺
 * ʱ�����ӣ�1/(72MHz/(TIMx_PRESCALER+1)) = 1/(72M/72) = 1us
 * TIM_CHANNEL_3:
 * CCR1:2000��CCR2:8000��CCR3:7000��CCR4:3000
 *      2ms        8ms        7ms        3ms
 * TIM_CHANNEL_4:
 * CCR1:2500��CCR2:2500��CCR3:2500��CCR4:2500
 *      2.5ms      2.5ms      2.5ms      2.5ms    
 */
// ��ʱ��Ԥ��Ƶ
#define GENERAL_TIM_PRESCALER             (72-1)
// ��ʱ�����ڣ�������Ϊ���ֵ
#define GENERAL_TIM_PERIOD                (0xFFFF) 
// ��ʱ��CCRֵ��������������������

/* ��ʱ�� */
#define GENERAL_TIM           				    TIM3
#define GENERAL_TIM_CLK_ENABLE()  			  __HAL_RCC_TIM3_CLK_ENABLE()
#define GENERAL_TIM_IRQn		              TIM3_IRQn
#define GENERAL_TIM_IRQHandler            TIM3_IRQHandler
 
/* TIM3ͨ��3������� */
#define GENERAL_CH3_PIN           		    GPIO_PIN_0             
#define GENERAL_CH3_GPIO_PORT     		    GPIOB                    
#define GENERAL_CH3_GPIO_CLK_ENABLE() 	  __HAL_RCC_GPIOB_CLK_ENABLE()
#define GENERAL_TIM_CHANNELx1             TIM_CHANNEL_3

/* TIM3ͨ��4������� */
#define GENERAL_CH4_PIN            		    GPIO_PIN_1              
#define GENERAL_CH4_GPIO_PORT      		    GPIOB                    
#define GENERAL_CH4_GPIO_CLK_ENABLE()	    __HAL_RCC_GPIOB_CLK_ENABLE()
#define GENERAL_TIM_CHANNELx2             TIM_CHANNEL_4

extern TIM_HandleTypeDef TIM_TimeBaseStructure;

void TIMx_Configuration(void);

#endif /* __GENERAL_TIM_H */

