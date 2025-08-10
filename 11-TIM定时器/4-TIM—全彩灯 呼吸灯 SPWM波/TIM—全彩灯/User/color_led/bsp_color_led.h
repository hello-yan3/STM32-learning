#ifndef __COLOR_LED_H
#define	__COLOR_LED_H

#include "stm32f1xx.h"

extern TIM_HandleTypeDef COLOR_TIM;


/*
 * ��ʱʱ����㣺
 * ʱ�����ӣ�1/(72MHz/(TIMx_PRESCALER+1)) = 1/(72M/256) = 3.5556us
 * ���嶨ʱ�����ڣ�T_Period = 3.5556us * 500 =1.77778ms
 */
// ��ʱ��Ԥ��Ƶ
#define  COLOR_TIM_PRESCALER               (256-1)

// ��ʱ������
#define  COLOR_TIM_PERIOD                  (500-1)

/********************��ʱ��ͨ��**************************/
#define  COLOR_TIMx                        TIM3

#define  COLOR_TIM_RCC_CLK_ENABLE()        __HAL_RCC_TIM3_CLK_ENABLE()
#define  COLOR_TIM_RCC_CLK_DISABLE()       __HAL_RCC_TIM3_CLK_DISABLE()

#define  COLOR_TIM_GPIO_RCC_CLK_ENABLE()   __HAL_RCC_GPIOB_CLK_ENABLE()
#define  COLOR_TIM_GPIO_PORT               GPIOB
/************���***************/
//��Ƶ�������Ҫ��ӳ��
#define  RCC_AFIO_CLK_ENABLE()           __HAL_RCC_AFIO_CLK_ENABLE()
#define  AFIO_REMAP_TIM_PARTIAL()		     __HAL_AFIO_REMAP_TIM3_PARTIAL()

#define  COLOR_RED_TIM_CH3_PORT          GPIOB
#define  COLOR_RED_TIM_CH3_PIN           GPIO_PIN_5

#define  COLOR_RED_TIM_CHANNEL           TIM_CHANNEL_2
/************�̵�***************/
#define  COLOR_GREEN_TIM_CH3_PORT        GPIOB
#define  COLOR_GREEN_TIM_CH3_PIN         GPIO_PIN_0

#define  COLOR_GREEN_TIM_CHANNEL         TIM_CHANNEL_3
/************����***************/
#define  COLOR_BLUE_TIM_CH3_PORT         GPIOB
#define  COLOR_BLUE_TIM_CH3_PIN          GPIO_PIN_1

#define  COLOR_BLUE_TIM_CHANNEL          TIM_CHANNEL_4


void TIMx_Mode_Config(void);

void SetRGBColor(uint32_t rgb);
void SetColorValue(uint8_t r,uint8_t g,uint8_t b);

#endif /* __PWM_BREATHING_H */
