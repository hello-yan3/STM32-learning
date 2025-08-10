#ifndef __GENERAL_TIM_H
#define	__GENERAL_TIM_H

#include "stm32f1xx.h"


extern uint16_t OC_Pulse;
extern uint8_t turn;

// 定时器预分频
#define GENERAL_TIM_PRESCALER             (72-1)
// 定时器周期，这里设为最大值
#define GENERAL_TIM_PERIOD                (0xFFFF) 
// 定时器CCR值，在主函数中数组声明

/* 定时器 */
#define GENERAL_TIM           				    TIM3
#define GENERAL_TIM_CLK_ENABLE()  			  __HAL_RCC_TIM3_CLK_ENABLE()
#define GENERAL_TIM_IRQn		              TIM3_IRQn
#define GENERAL_TIM_IRQHandler            TIM3_IRQHandler
 
/* TIM3通道3输出引脚 */
#define GENERAL_CH3_PIN           		    GPIO_PIN_0             
#define GENERAL_CH3_GPIO_PORT     		    GPIOB                    
#define GENERAL_CH3_GPIO_CLK_ENABLE() 	  __HAL_RCC_GPIOB_CLK_ENABLE()
#define GENERAL_TIM_CHANNELx1             TIM_CHANNEL_3

/* TIM3通道4输出引脚 */
#define GENERAL_CH4_PIN            		    GPIO_PIN_1              
#define GENERAL_CH4_GPIO_PORT      		    GPIOB                    
#define GENERAL_CH4_GPIO_CLK_ENABLE()	    __HAL_RCC_GPIOB_CLK_ENABLE()
#define GENERAL_TIM_CHANNELx2             TIM_CHANNEL_4

extern TIM_HandleTypeDef TIM_TimeBaseStructure_OC;

void TIMx_Configuration(void);

#endif /* __GENERAL_TIM_H */

