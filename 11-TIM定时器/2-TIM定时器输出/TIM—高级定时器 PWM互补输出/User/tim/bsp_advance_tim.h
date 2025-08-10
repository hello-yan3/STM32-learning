#ifndef __ADVANCE_TIM_H
#define	__ADVANCE_TIM_H

#include "stm32f1xx.h"

/*
 * 定时时间计算：
 * 时钟因子：1/(72MHz/(TIMx_PRESCALER+1)) = 1/(72M/72) = 1us
 * 定义定时器周期：T_Period = 1us * 1000 =1ms
 * CCR寄存器500，即0-500ms有效电平，500ms-999ms无效电平
 * 由设置知，有效电平设置为高，
 * CK_PSC = 72M, (PSC+1) = 72, CCR = 500, (ARR+1) = 1000
 * PWM频率：	Freq = CK_PSC / (PSC + 1) / (ARR + 1) = 72M/72/1000 = 1000 (周期：1ms)
 * PWM占空比：	Duty = CCR / (ARR + 1) = 500/1000 = 0.5
 * PWM分辨率：	Reso = 1 / (ARR + 1) = 1 / 1000  (PWR最小调节量即为分辨率)
 */
// 定时器预分频
#define ADVANCED_TIM_PRESCALER            (72-1)
// 高级定时器重复计数寄存器值
#define ADVANCED_TIM_REPETITIONCOUNTER    (1-1)
// 定时器周期
#define ADVANCED_TIM_PERIOD               (1000-1) 
// 定时器CCR值
#define ADVANCED_TIM_CCR                  (500) 
// 定义死区时间, 这里配置的死区时间为152ns
#define DEADTIME  11

/* 定时器 */
#define ADVANCE_TIM           				     TIM8
#define ADVANCE_TIM_CLK_ENABLE()  			   __HAL_RCC_TIM8_CLK_ENABLE()
/* 定时器通道 */
#define ADVANCE_TIM_CHANNELx1              TIM_CHANNEL_1

/* TIM1通道1输出引脚 */
#define ADVANCE_OCPWM_PIN           		   GPIO_PIN_6             
#define ADVANCE_OCPWM_GPIO_PORT     		   GPIOC                    
#define ADVANCE_OCPWM_GPIO_CLK_ENABLE()  	 __HAL_RCC_GPIOC_CLK_ENABLE()

/* TIM1通道1互补输出引脚 */
#define ADVANCE_OCNPWM_PIN            		 GPIO_PIN_7              
#define ADVANCE_OCNPWM_GPIO_PORT      	 	 GPIOA                     
#define ADVANCE_OCNPWM_GPIO_CLK_ENABLE()	 __HAL_RCC_GPIOA_CLK_ENABLE()

/* TIM1刹车通道引脚 */
#define ADVANCE_BKIN_PIN            	  	 GPIO_PIN_6              
#define ADVANCE_BKIN_GPIO_PORT      	   	 GPIOA                     
#define ADVANCE_BKIN_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()

extern TIM_HandleTypeDef TIM_TimeBaseStructure;
extern TIM_OC_InitTypeDef TIM_OCInitStructure;

void TIMx_Configuration(void);

#endif /* __ADVANCE_TIM_H */

