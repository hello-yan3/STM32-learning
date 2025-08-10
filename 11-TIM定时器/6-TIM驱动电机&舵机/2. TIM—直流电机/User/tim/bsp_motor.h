#ifndef __MOTOR_H
#define	__MOTOR_H

#include "stm32f1xx.h"

/*******************���Ŷ���********************/
#define Motor_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()
#define Motor1_PIN                  GPIO_PIN_4
#define Motor2_PIN                  GPIO_PIN_5
#define Motor_GPIO_PORT             GPIOA       

// ������ʼ��
void Motor_GPIO_Config(void);
void Motor_SetSpeed(int16_t Speed);
	
#endif /* __MOTOR_H */
