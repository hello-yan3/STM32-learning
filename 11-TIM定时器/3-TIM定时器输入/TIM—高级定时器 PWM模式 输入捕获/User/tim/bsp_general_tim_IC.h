#ifndef __GENERAL_IC_TIM_H
#define	__GENERAL_IC_TIM_H

#include "stm32f1xx.h"

/*
 * 测量PWM波两种方法：
 * 测频法：即每个上升沿，进入一次中断，统计一个时间周期T内，进入中断次数COUNT
 * PWM周期为：COUNT/T，试用于高频信号
 * 另一种，即通过输入捕获测量周期
 * 预分频后周期为：fclk，则一个周期时间为：1/fclk
 * 统计计数器次数N，
 * 则PWM周期为：N/fclk，试用于低频信号
 * 
 * 这里使用输入捕获方法测量周期
 * 假设PWM波频率1000Hz，周期1ms
 * 设置预分频器值为：(72-1) ，即fclk：72M/72=1M，周期1us
 * ARR计数器值设置为最大：(65536-1)，则计数器值可累计最大数为65535
 * 
 * 设置TIM_Channel1读取上升沿信号，
 * 当检测到第二个上升沿信号，此时CNT值会加载到CCR1寄存器中
 * 读取CCR1寄存器，则得到计数器次数N，之后中断手动清零CNT(或者配置主从模式清零CNT)
 * 设置TIM_Channel2读取下降沿信号，
 * 当检测到下降沿信号，此时CNT值会加载到CCR2寄存器中
 * 读取CCR2寄存器，则得到计数器次数N2，此时N2/N即占空比
 * 若只需知道方波频率，则无需配置TIM_Channel2
 */
// 定时器预分频
#define GENERAL_TIM_IC_PRESCALER              (72-1)
// 定时器周期 （这里是计数器CNT最大值）
#define GENERAL_TIM_IC_PERIOD                 0xFFFF
// 定时器滤波频率，在0x00~0x0F中选择，数字越大滤波效果越好
#define GENERAL_TIM_ICFilter                  (0x00) 

/* 定时器 */
#define GENERAL_TIM_IC           				      TIM8
#define GENERAL_TIM_IC_CLK_ENABLE()  			    __HAL_RCC_TIM8_CLK_ENABLE()
// 注意：主从模式输入捕获PWM只能使用TIM_CHANNEL_1和TIM_CHANNEL_2
// 若不使用主从模式，需要在中断中手动清零CNT
#define GENERAL_TIM_IC_CHANNELx1              TIM_CHANNEL_1
#define GENERAL_TIM_IC_CHANNELx2              TIM_CHANNEL_2
#define GENERAL_TIM_IC_ACTIVE_CHANNELx1       HAL_TIM_ACTIVE_CHANNEL_1
 
/* TIM3通道1输入引脚 */
#define GENERAL_TIM_IC_PIN           		      GPIO_PIN_6         
#define GENERAL_TIM_IC_GPIO_PORT     		      GPIOC                    
#define GENERAL_TIM_IC_GPIO_CLK_ENABLE() 	    __HAL_RCC_GPIOC_CLK_ENABLE()

#define GENERAL_TIM_IC_AFIO()                   

/* 中断源选择 */
#define GENERAL_TIM_IRQn                      TIM8_CC_IRQn         // TIM3_IRQn         // 通用定时器一种TIM中断 
#define GENERAL_TIM_IRQHANDLER                TIM8_CC_IRQHandler   // TIM3_IRQHandler   // 高级定时器有多种TIM中断

extern TIM_HandleTypeDef TIM_TimeBaseStructure_IC;

void TIMx_IC_Configuration(void);

#endif /* __GENERAL_IC_TIM_H */

