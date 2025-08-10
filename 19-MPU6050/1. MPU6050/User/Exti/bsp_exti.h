#ifndef __EXTI_H
#define	__EXTI_H


#include "stm32f1xx.h"


/*�ⲿ�ж�EXIT��غ궨��*/
#define						  EXTI_INT_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOF_CLK_ENABLE()
#define             EXTI_GPIO_PORT                       GPIOF   
#define             EXTI_GPIO_PIN                        GPIO_PIN_7
#define             EXTI_IRQ                             EXTI9_5_IRQn
#define             EXTI_INT_FUNCTION                    EXTI9_5_IRQHandler

// ������Ҫ�Ƿ���DMP������жϿ���/�رպ���
#define ENABLE_INV_INTERRUPTS   HAL_NVIC_EnableIRQ(EXTI_IRQ);
#define DISABLE_INV_INTERRUPTS  HAL_NVIC_DisableIRQ(EXTI_IRQ)

extern uint8_t EXTI_flag;
void EXTI_Pxy_Config(void);

#endif /* __EXTI_H */
