#ifndef __GENERAL_IC_TIM_H
#define	__GENERAL_IC_TIM_H

#include "stm32f1xx.h"


// 定时器预分频
#define GENERAL_TIM_Encoder_PRESCALER              (0)       // 不分频，编码器时钟直接驱动计数器
// 编码器计数值
// 初始值设为0，向上计数则为1，向下计数则为计数器最大值
#define GENERAL_TIM_Encoder_PERIOD                 0xFFFF    // uint16_t 65535 → int16_t -1
// 定时器滤波频率，在0x00~0x0F中选择，数字越大滤波效果越好
#define GENERAL_TIM_ICFilter                  (0x00)    

#define GENERAL_TIM_Encoder           				      TIM4
#define GENERAL_TIM_Encoder_CLK_ENABLE()  			    __HAL_RCC_TIM4_CLK_ENABLE()
//// Encoder 编码器接口只可以接入 TIM_CHANNEL_1 和 TIM_CHANNEL_2
//#define GENERAL_TIM_IC_CHANNELx1              TIM_CHANNEL_1
//#define GENERAL_TIM_IC_CHANNELx2              TIM_CHANNEL_2

/* TIM5通道1输入引脚 */
#define GENERAL_CH1_Encoder_PIN           		      GPIO_PIN_6           
#define GENERAL_CH2_Encoder_PIN           		      GPIO_PIN_7      
#define GENERAL_CH_Encoder_GPIO_PORT     		        GPIOB                    
#define GENERAL_CH_Encoder_GPIO_CLK_ENABLE() 	      __HAL_RCC_GPIOB_CLK_ENABLE()

extern TIM_HandleTypeDef TIM_TimeBaseStructure;

void TIM_Encoder_Mode_Config(void);

#endif /* __GENERAL_IC_TIM_H */

