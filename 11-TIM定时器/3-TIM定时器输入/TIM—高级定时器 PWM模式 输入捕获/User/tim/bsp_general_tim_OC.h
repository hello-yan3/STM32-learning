#ifndef __GENERAL_OC_TIM_H
#define	__GENERAL_OC_TIM_H

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
#define GENERAL_TIM_OC_PRESCALER              (72-1)
// 定时器周期
#define GENERAL_TIM_OC_PERIOD                 (5000-1) 
// 定时器CCR值
#define GENERAL_TIM_OC_CH_CCR                 (2500) 

/* 定时器 */
#define GENERAL_TIM_OC           				      TIM3
#define GENERAL_TIM_OC_CLK_ENABLE()  			    __HAL_RCC_TIM3_CLK_ENABLE()
#define GENERAL_TIM_OC_CHANNELx1              TIM_CHANNEL_2
 
/* TIM3通道3输出引脚 */
#define GENERAL_TIM_OC_PIN           		      GPIO_PIN_7            
#define GENERAL_TIM_OC_GPIO_PORT     		      GPIOA                    
#define GENERAL_TIM_OC_GPIO_CLK_ENABLE() 	    __HAL_RCC_GPIOA_CLK_ENABLE()
// 定义引脚重映射，不需要的话将后面清空
#define GENERAL_TIM_OC_AFIO()                   

extern TIM_HandleTypeDef TIM_TimeBaseStructure_OC;

void TIMx_OC_Configuration(void);

#endif /*__GENERAL_OC_TIM_H */

