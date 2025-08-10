#ifndef __GENERAL_IC_TIM_H
#define	__GENERAL_IC_TIM_H

#include "stm32f1xx.h"

/*
 * ����PWM�����ַ�����
 * ��Ƶ������ÿ�������أ�����һ���жϣ�ͳ��һ��ʱ������T�ڣ������жϴ���COUNT
 * PWM����Ϊ��COUNT/T�������ڸ�Ƶ�ź�
 * ��һ�֣���ͨ�����벶���������
 * Ԥ��Ƶ������Ϊ��fclk����һ������ʱ��Ϊ��1/fclk
 * ͳ�Ƽ���������N��
 * ��PWM����Ϊ��N/fclk�������ڵ�Ƶ�ź�
 * 
 * ����ʹ�����벶�񷽷���������
 * ����PWM��Ƶ��1000Hz������1ms
 * ����Ԥ��Ƶ��ֵΪ��(72-1) ����fclk��72M/72=1M������1us
 * ARR������ֵ����Ϊ���(65536-1)���������ֵ���ۼ������Ϊ65535
 * 
 * ����TIM_Channel1��ȡ�������źţ�
 * ����⵽�ڶ����������źţ���ʱCNTֵ����ص�CCR1�Ĵ�����
 * ��ȡCCR1�Ĵ�������õ�����������N��֮���ж��ֶ�����CNT(������������ģʽ����CNT)
 * ����TIM_Channel2��ȡ�½����źţ�
 * ����⵽�½����źţ���ʱCNTֵ����ص�CCR2�Ĵ�����
 * ��ȡCCR2�Ĵ�������õ�����������N2����ʱN2/N��ռ�ձ�
 * ��ֻ��֪������Ƶ�ʣ�����������TIM_Channel2
 */
// ��ʱ��Ԥ��Ƶ
#define GENERAL_TIM_IC_PRESCALER              (72-1)
// ��ʱ������ �������Ǽ�����CNT���ֵ��
#define GENERAL_TIM_IC_PERIOD                 0xFFFF
// ��ʱ���˲�Ƶ�ʣ���0x00~0x0F��ѡ������Խ���˲�Ч��Խ��
#define GENERAL_TIM_ICFilter                  (0x00) 

/* ��ʱ�� */
#define GENERAL_TIM_IC           				      TIM8
#define GENERAL_TIM_IC_CLK_ENABLE()  			    __HAL_RCC_TIM8_CLK_ENABLE()
// ע�⣺����ģʽ���벶��PWMֻ��ʹ��TIM_CHANNEL_1��TIM_CHANNEL_2
// ����ʹ������ģʽ����Ҫ���ж����ֶ�����CNT
#define GENERAL_TIM_IC_CHANNELx1              TIM_CHANNEL_1
#define GENERAL_TIM_IC_CHANNELx2              TIM_CHANNEL_2
#define GENERAL_TIM_IC_ACTIVE_CHANNELx1       HAL_TIM_ACTIVE_CHANNEL_1
 
/* TIM3ͨ��1�������� */
#define GENERAL_TIM_IC_PIN           		      GPIO_PIN_6         
#define GENERAL_TIM_IC_GPIO_PORT     		      GPIOC                    
#define GENERAL_TIM_IC_GPIO_CLK_ENABLE() 	    __HAL_RCC_GPIOC_CLK_ENABLE()

#define GENERAL_TIM_IC_AFIO()                   

/* �ж�Դѡ�� */
#define GENERAL_TIM_IRQn                      TIM8_CC_IRQn         // TIM3_IRQn         // ͨ�ö�ʱ��һ��TIM�ж� 
#define GENERAL_TIM_IRQHANDLER                TIM8_CC_IRQHandler   // TIM3_IRQHandler   // �߼���ʱ���ж���TIM�ж�

extern TIM_HandleTypeDef TIM_TimeBaseStructure_IC;

void TIMx_IC_Configuration(void);

#endif /* __GENERAL_IC_TIM_H */

