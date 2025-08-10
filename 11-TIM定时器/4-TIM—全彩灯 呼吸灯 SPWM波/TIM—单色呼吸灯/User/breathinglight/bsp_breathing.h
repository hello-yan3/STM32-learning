#ifndef __PWM_BREATHING_H
#define	__PWM_BREATHING_H

#include "stm32f1xx.h"

extern TIM_HandleTypeDef BRE_TIM;
extern  uint16_t indexWave[];

/*PWM表中的点数*/
extern uint16_t  POINT_NUM	;

// 计数周期
#define period_class  10

/*
 * 定时时间计算：
 * 时钟因子：1/(72MHz/(TIMx_PRESCALER+1)) = 1/(72M/200) = 2.7778us
 * 定义定时器周期：T_Period = 2.7778us * 1024 =2.8444ms
 */
// 定时器预分频
#define BRE_TIM_PRESCALER               (200-1)

// 定时器周期
#define BRE_TIM_PERIOD                  (1024-1)


#define RED_LIGHT 		1
#define GREEN_LIGHT 	2
#define BLUE_LIGHT		3

/*要使用什么颜色的呼吸灯，可选RED_LIGHT、GREEN_LIGHT、BLUE_LIGHT*/
#define LIGHT_COLOR 	RED_LIGHT
/********************定时器通道**************************/

#if  LIGHT_COLOR == RED_LIGHT 
/************红灯***************/
	#define  BRE_TIMx                        TIM3

	#define  BRE_TIM_RCC_CLK_ENABLE()        __HAL_RCC_TIM3_CLK_ENABLE()
	#define  BRE_TIM_RCC_CLK_DISABLE()       __HAL_RCC_TIM3_CLK_DISABLE()
	#define  BRE_TIM_GPIO_RCC_CLK_ENABLE()   __HAL_RCC_GPIOB_CLK_ENABLE()

	//红灯的引脚需要重映射
	#define  RCC_AFIO_CLK_ENABLE()           __HAL_RCC_AFIO_CLK_ENABLE()
	#define  AFIO_REMAP_TIM_PARTIAL()		     __HAL_AFIO_REMAP_TIM3_PARTIAL()
 
	#define  BRE_TIM_CH3_PORT                GPIOB
	#define  BRE_TIM_CH3_PIN                 GPIO_PIN_5
	
	#define  TIM_CHANNEL                     TIM_CHANNEL_2


	#define  BRE_TIM_IRQn                    TIM3_IRQn            
	#define  BRE_TIM_IRQHandler              TIM3_IRQHandler

#elif LIGHT_COLOR == GREEN_LIGHT
/************绿灯***************/
	#define  BRE_TIMx                        TIM3

	#define  BRE_TIM_RCC_CLK_ENABLE()        __HAL_RCC_TIM3_CLK_ENABLE()
	#define  BRE_TIM_RCC_CLK_DISABLE()       __HAL_RCC_TIM3_CLK_DISABLE()
	#define  BRE_TIM_GPIO_RCC_CLK_ENABLE()   __HAL_RCC_GPIOB_CLK_ENABLE()

	//绿灯不需要重映射
	#define  RCC_AFIO_CLK_ENABLE()          
	#define  AFIO_REMAP_TIM_PARTIAL()		   

	#define  BRE_TIM_CH3_PORT                GPIOB
	#define  BRE_TIM_CH3_PIN                 GPIO_PIN_0
	
	#define  TIM_CHANNEL                		 TIM_CHANNEL_3

	#define  BRE_TIM_IRQn                    TIM3_IRQn            
	#define  BRE_TIM_IRQHandler              TIM3_IRQHandler

#elif LIGHT_COLOR == BLUE_LIGHT
/************蓝灯***************/
	#define   BRE_TIMx                       TIM3

	#define  BRE_TIM_RCC_CLK_ENABLE()        __HAL_RCC_TIM3_CLK_ENABLE()
	#define  BRE_TIM_RCC_CLK_DISABLE()       __HAL_RCC_TIM3_CLK_DISABLE()
	#define  BRE_TIM_GPIO_RCC_CLK_ENABLE()   __HAL_RCC_GPIOB_CLK_ENABLE()

	//蓝灯不需要重映射
	#define  RCC_AFIO_CLK_ENABLE()          
	#define  AFIO_REMAP_TIM_PARTIAL()		   

	#define  BRE_TIM_CH3_PORT                GPIOB
	#define  BRE_TIM_CH3_PIN                 GPIO_PIN_1
	
	#define  TIM_CHANNEL                		 TIM_CHANNEL_4

	#define  BRE_TIM_IRQn                    TIM3_IRQn            
	#define  BRE_TIM_IRQHandler              TIM3_IRQHandler

#endif


void TIMx_Mode_Config(void);

#endif /* __PWM_BREATHING_H */
