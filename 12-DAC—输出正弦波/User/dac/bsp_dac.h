#ifndef __DACx_H
#define	__DACx_H


#include "stm32f1xx.h"

#define POINT_NUM 32

#define DACx                             DAC
#define DACx_CLK_ENABLE()                __HAL_RCC_DAC_CLK_ENABLE()
#define DACx_CHANNEL_GPIO_CLK_DISABLE()  __HAL_RCC_DAC_CLK_DISABLE()
#define DACx_CHANNEL                     DAC_CHANNEL_2


#define DACx_CHANNEL_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOA_CLK_ENABLE()
#define DACx_CHANNEL_PIN                 GPIO_PIN_5
#define DACx_CHANNEL_GPIO_PORT           GPIOA

#define DACx_DMA_CLK_ENABLE()            __HAL_RCC_DMA2_CLK_ENABLE()
#define DACx_DMAx_CHANNELn               DMA2_Channel4    
#define DMA_HANDLE                       DMA_Handle2                      

#define DAC_TIM_CLK_ENABLE()             __HAL_RCC_TIM6_CLK_ENABLE()
#define DAC_TIM                          TIM6
#define TIM_PRESCALER                    (72-1)
#define TIM_PERIOD                       (1000-1)
/*
 * ��ʱ���ڣ�
 * ʱ�����ӣ�1/(72MHz/(BASIC_TIMx_PRESCALER+1)) = 1/(72M/72) = 1us
 * ���嶨ʱ�����ڣ�T_Period = 1us * 1000 =1ms
 * �������ڣ�����: 32
 * ���Ҳ����ڣ�32ms�����Ҳ�Ƶ�ʣ�31.25
 */


void DAC_Config(void);

extern uint16_t Sine12bit[POINT_NUM]; 

extern TIM_HandleTypeDef TIM_Time;
extern DMA_HandleTypeDef DMA_InitStructure;
extern DAC_HandleTypeDef  DAC_InitStructure;
#endif /* __DACx_H */

