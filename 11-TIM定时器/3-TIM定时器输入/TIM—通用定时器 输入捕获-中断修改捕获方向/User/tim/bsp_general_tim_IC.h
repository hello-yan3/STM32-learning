#ifndef __GENERAL_IC_TIM_H
#define	__GENERAL_IC_TIM_H

#include "stm32f1xx.h"

// 定时器预分频
#define GENERAL_TIM_IC_PRESCALER              (72-1)
// 定时器周期 （这里是计数器CNT最大值）
#define GENERAL_TIM_IC_PERIOD                 0xFFFF
// 定时器滤波频率，在0x00~0x0F中选择，数字越大滤波效果越好
#define GENERAL_TIM_ICFilter                  (0x00) 

/* 定时器 */
#define GENERAL_TIM_IC           				      TIM3
#define GENERAL_TIM_IC_CLK_ENABLE()  			    __HAL_RCC_TIM3_CLK_ENABLE()
// 不使用主从模式，在中断中手动清零CNT
#define GENERAL_TIM_IC_CHANNELx1              TIM_CHANNEL_2
 
/* TIM3通道1输入引脚 */
#define GENERAL_TIM_IC_PIN           		      GPIO_PIN_7         
#define GENERAL_TIM_IC_GPIO_PORT     		      GPIOA                    
#define GENERAL_TIM_IC_GPIO_CLK_ENABLE() 	    __HAL_RCC_GPIOA_CLK_ENABLE()

#define GENERAL_TIM_IC_AFIO()                   

/* 中断源选择 */
#define GENERAL_TIM_IRQn                      TIM3_IRQn       
#define GENERAL_TIM_IRQHANDLER                TIM3_IRQHandler  
// 对于高级定时器，需要同时开始TIM8_UP_IRQn(定时器更新中断)和TIM8_CC_IRQn(定时器输入捕获/输出比较)中断

extern TIM_HandleTypeDef TIM_TimeBaseStructure_IC;

void TIMx_IC_Configuration(void);

#endif /* __GENERAL_IC_TIM_H */

