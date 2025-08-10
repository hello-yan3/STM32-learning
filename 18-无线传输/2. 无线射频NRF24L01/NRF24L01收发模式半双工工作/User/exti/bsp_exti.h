#ifndef __EXTI_H
#define	__EXTI_H

#include "stm32f1xx.h"

//Òý½Å¶¨Òå
/*******************************************************/
#define EXTI_INT_GPIO_PORT                GPIOB
#define EXTI_INT_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOB_CLK_ENABLE()
#define EXTI_INT_GPIO_PIN                 GPIO_PIN_13
#define EXTI_INT_EXTI_IRQ                 EXTI15_10_IRQn
#define EXTI_IRQHandler                   EXTI15_10_IRQHandler
/*******************************************************/

extern uint8_t EXTI_FLAG;

void EXTI_Key_Config(void);

#endif /* __EXTI_H */
