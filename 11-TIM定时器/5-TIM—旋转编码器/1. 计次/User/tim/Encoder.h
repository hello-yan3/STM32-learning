#ifndef __GENERAL_IC_TIM_H
#define	__GENERAL_IC_TIM_H

#include "stm32f1xx.h"


// ��ʱ��Ԥ��Ƶ
#define GENERAL_TIM_Encoder_PRESCALER              (0)       // ����Ƶ��������ʱ��ֱ������������
// ����������ֵ
// ��ʼֵ��Ϊ0�����ϼ�����Ϊ1�����¼�����Ϊ���������ֵ
#define GENERAL_TIM_Encoder_PERIOD                 0xFFFF    // uint16_t 65535 �� int16_t -1
// ��ʱ���˲�Ƶ�ʣ���0x00~0x0F��ѡ������Խ���˲�Ч��Խ��
#define GENERAL_TIM_ICFilter                  (0x00)    

#define GENERAL_TIM_Encoder           				      TIM4
#define GENERAL_TIM_Encoder_CLK_ENABLE()  			    __HAL_RCC_TIM4_CLK_ENABLE()
//// Encoder �������ӿ�ֻ���Խ��� TIM_CHANNEL_1 �� TIM_CHANNEL_2
//#define GENERAL_TIM_IC_CHANNELx1              TIM_CHANNEL_1
//#define GENERAL_TIM_IC_CHANNELx2              TIM_CHANNEL_2

/* TIM5ͨ��1�������� */
#define GENERAL_CH1_Encoder_PIN           		      GPIO_PIN_6           
#define GENERAL_CH2_Encoder_PIN           		      GPIO_PIN_7      
#define GENERAL_CH_Encoder_GPIO_PORT     		        GPIOB                    
#define GENERAL_CH_Encoder_GPIO_CLK_ENABLE() 	      __HAL_RCC_GPIOB_CLK_ENABLE()

extern TIM_HandleTypeDef TIM_TimeBaseStructure;

void TIM_Encoder_Mode_Config(void);

#endif /* __GENERAL_IC_TIM_H */

