#ifndef __GENERAL_OC_TIM_H
#define	__GENERAL_OC_TIM_H

#include "stm32f1xx.h"

/*
 * ��ʱʱ����㣺
 * ʱ�����ӣ�1/(72MHz/(TIMx_PRESCALER+1)) = 1/(72M/72) = 1us
 * ���嶨ʱ�����ڣ�T_Period = 1us * 1000 =1ms
 * CCR�Ĵ���500����0-500ms��Ч��ƽ��500ms-999ms��Ч��ƽ
 * ������֪����Ч��ƽ����Ϊ�ߣ�
 * CK_PSC = 72M, (PSC+1) = 72, CCR = 500, (ARR+1) = 1000
 * PWMƵ�ʣ�	Freq = CK_PSC / (PSC + 1) / (ARR + 1) = 72M/72/1000 = 1000 (���ڣ�1ms)
 * PWMռ�ձȣ�	Duty = CCR / (ARR + 1) = 500/1000 = 0.5
 * PWM�ֱ��ʣ�	Reso = 1 / (ARR + 1) = 1 / 1000  (PWR��С��������Ϊ�ֱ���)
 */
// ��ʱ��Ԥ��Ƶ
#define GENERAL_TIM_OC_PRESCALER              (72-1)
// ��ʱ������
#define GENERAL_TIM_OC_PERIOD                 (5000-1) 
// ��ʱ��CCRֵ
#define GENERAL_TIM_OC_CH_CCR                 (2500) 

/* ��ʱ�� */
#define GENERAL_TIM_OC           				      TIM3
#define GENERAL_TIM_OC_CLK_ENABLE()  			    __HAL_RCC_TIM3_CLK_ENABLE()
#define GENERAL_TIM_OC_CHANNELx1              TIM_CHANNEL_2
 
/* TIM3ͨ��3������� */
#define GENERAL_TIM_OC_PIN           		      GPIO_PIN_7            
#define GENERAL_TIM_OC_GPIO_PORT     		      GPIOA                    
#define GENERAL_TIM_OC_GPIO_CLK_ENABLE() 	    __HAL_RCC_GPIOA_CLK_ENABLE()
// ����������ӳ�䣬����Ҫ�Ļ����������
#define GENERAL_TIM_OC_AFIO()                   

extern TIM_HandleTypeDef TIM_TimeBaseStructure_OC;

void TIMx_OC_Configuration(void);

#endif /*__GENERAL_OC_TIM_H */

