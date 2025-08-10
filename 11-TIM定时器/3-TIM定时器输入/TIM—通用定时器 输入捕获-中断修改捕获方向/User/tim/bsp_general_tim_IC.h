#ifndef __GENERAL_IC_TIM_H
#define	__GENERAL_IC_TIM_H

#include "stm32f1xx.h"

// ��ʱ��Ԥ��Ƶ
#define GENERAL_TIM_IC_PRESCALER              (72-1)
// ��ʱ������ �������Ǽ�����CNT���ֵ��
#define GENERAL_TIM_IC_PERIOD                 0xFFFF
// ��ʱ���˲�Ƶ�ʣ���0x00~0x0F��ѡ������Խ���˲�Ч��Խ��
#define GENERAL_TIM_ICFilter                  (0x00) 

/* ��ʱ�� */
#define GENERAL_TIM_IC           				      TIM3
#define GENERAL_TIM_IC_CLK_ENABLE()  			    __HAL_RCC_TIM3_CLK_ENABLE()
// ��ʹ������ģʽ�����ж����ֶ�����CNT
#define GENERAL_TIM_IC_CHANNELx1              TIM_CHANNEL_2
 
/* TIM3ͨ��1�������� */
#define GENERAL_TIM_IC_PIN           		      GPIO_PIN_7         
#define GENERAL_TIM_IC_GPIO_PORT     		      GPIOA                    
#define GENERAL_TIM_IC_GPIO_CLK_ENABLE() 	    __HAL_RCC_GPIOA_CLK_ENABLE()

#define GENERAL_TIM_IC_AFIO()                   

/* �ж�Դѡ�� */
#define GENERAL_TIM_IRQn                      TIM3_IRQn       
#define GENERAL_TIM_IRQHANDLER                TIM3_IRQHandler  
// ���ڸ߼���ʱ������Ҫͬʱ��ʼTIM8_UP_IRQn(��ʱ�������ж�)��TIM8_CC_IRQn(��ʱ�����벶��/����Ƚ�)�ж�

extern TIM_HandleTypeDef TIM_TimeBaseStructure_IC;

void TIMx_IC_Configuration(void);

#endif /* __GENERAL_IC_TIM_H */

