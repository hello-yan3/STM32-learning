#ifndef __ADVANCE_TIM_H
#define	__ADVANCE_TIM_H

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
#define ADVANCED_TIM_PRESCALER            (72-1)
// �߼���ʱ���ظ������Ĵ���ֵ
#define ADVANCED_TIM_REPETITIONCOUNTER    (1-1)
// ��ʱ������
#define ADVANCED_TIM_PERIOD               (1000-1) 
// ��ʱ��CCRֵ
#define ADVANCED_TIM_CCR                  (500) 
// ��������ʱ��, �������õ�����ʱ��Ϊ152ns
#define DEADTIME  11

/* ��ʱ�� */
#define ADVANCE_TIM           				     TIM8
#define ADVANCE_TIM_CLK_ENABLE()  			   __HAL_RCC_TIM8_CLK_ENABLE()
/* ��ʱ��ͨ�� */
#define ADVANCE_TIM_CHANNELx1              TIM_CHANNEL_1

/* TIM1ͨ��1������� */
#define ADVANCE_OCPWM_PIN           		   GPIO_PIN_6             
#define ADVANCE_OCPWM_GPIO_PORT     		   GPIOC                    
#define ADVANCE_OCPWM_GPIO_CLK_ENABLE()  	 __HAL_RCC_GPIOC_CLK_ENABLE()

/* TIM1ͨ��1����������� */
#define ADVANCE_OCNPWM_PIN            		 GPIO_PIN_7              
#define ADVANCE_OCNPWM_GPIO_PORT      	 	 GPIOA                     
#define ADVANCE_OCNPWM_GPIO_CLK_ENABLE()	 __HAL_RCC_GPIOA_CLK_ENABLE()

/* TIM1ɲ��ͨ������ */
#define ADVANCE_BKIN_PIN            	  	 GPIO_PIN_6              
#define ADVANCE_BKIN_GPIO_PORT      	   	 GPIOA                     
#define ADVANCE_BKIN_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()

extern TIM_HandleTypeDef TIM_TimeBaseStructure;
extern TIM_OC_InitTypeDef TIM_OCInitStructure;

void TIMx_Configuration(void);

#endif /* __ADVANCE_TIM_H */

