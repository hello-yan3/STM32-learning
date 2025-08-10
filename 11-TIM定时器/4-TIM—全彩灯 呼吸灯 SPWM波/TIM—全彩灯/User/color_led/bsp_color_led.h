#ifndef __COLOR_LED_H
#define	__COLOR_LED_H

#include "stm32f1xx.h"

extern TIM_HandleTypeDef COLOR_TIM;


/*
 * 定时时间计算：
 * 时钟因子：1/(72MHz/(TIMx_PRESCALER+1)) = 1/(72M/256) = 3.5556us
 * 定义定时器周期：T_Period = 3.5556us * 500 =1.77778ms
 */
// 定时器预分频
#define  COLOR_TIM_PRESCALER               (256-1)

// 定时器周期
#define  COLOR_TIM_PERIOD                  (500-1)

/********************定时器通道**************************/
#define  COLOR_TIMx                        TIM3

#define  COLOR_TIM_RCC_CLK_ENABLE()        __HAL_RCC_TIM3_CLK_ENABLE()
#define  COLOR_TIM_RCC_CLK_DISABLE()       __HAL_RCC_TIM3_CLK_DISABLE()

#define  COLOR_TIM_GPIO_RCC_CLK_ENABLE()   __HAL_RCC_GPIOB_CLK_ENABLE()
#define  COLOR_TIM_GPIO_PORT               GPIOB
/************红灯***************/
//红灯的引脚需要重映射
#define  RCC_AFIO_CLK_ENABLE()           __HAL_RCC_AFIO_CLK_ENABLE()
#define  AFIO_REMAP_TIM_PARTIAL()		     __HAL_AFIO_REMAP_TIM3_PARTIAL()

#define  COLOR_RED_TIM_CH3_PORT          GPIOB
#define  COLOR_RED_TIM_CH3_PIN           GPIO_PIN_5

#define  COLOR_RED_TIM_CHANNEL           TIM_CHANNEL_2
/************绿灯***************/
#define  COLOR_GREEN_TIM_CH3_PORT        GPIOB
#define  COLOR_GREEN_TIM_CH3_PIN         GPIO_PIN_0

#define  COLOR_GREEN_TIM_CHANNEL         TIM_CHANNEL_3
/************蓝灯***************/
#define  COLOR_BLUE_TIM_CH3_PORT         GPIOB
#define  COLOR_BLUE_TIM_CH3_PIN          GPIO_PIN_1

#define  COLOR_BLUE_TIM_CHANNEL          TIM_CHANNEL_4


void TIMx_Mode_Config(void);

void SetRGBColor(uint32_t rgb);
void SetColorValue(uint8_t r,uint8_t g,uint8_t b);

#endif /* __PWM_BREATHING_H */
