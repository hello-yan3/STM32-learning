#ifndef __EXTI_H
#define	__EXTI_H

#include "stm32f1xx.h"

//Òý½Å¶¨Òå
/*******************************************************/
#define ENCODER1_INT_GPIO_PORT                GPIOA
#define ENCODER1_INT_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOA_CLK_ENABLE();
#define ENCODER1_INT_GPIO_PIN                 GPIO_PIN_0
#define ENCODER1_INT_EXTI_IRQ                 EXTI0_IRQn
#define ENCODER1_IRQHandler                   EXTI0_IRQHandler

#define ENCODER2_INT_GPIO_PORT                GPIOA
#define ENCODER2_INT_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOC_CLK_ENABLE();
#define ENCODER2_INT_GPIO_PIN                 GPIO_PIN_1
#define ENCODER2_INT_EXTI_IRQ                 EXTI1_IRQn
#define ENCODER2_IRQHandler                   EXTI1_IRQHandler

/*******************************************************/

extern int16_t Encoder_Count;

void EXTI_Key_Config(void);

#endif /* __EXTI_H */
