/**
  ******************************************************************************
  * @file    bsp_basic_tim.c
  * @author  STMicroelectronics
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   ������ʱ����ʱ����
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F103 ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./BSP/TIMER/basic_tim.h"
#include "./BSP/LED/led.h"
#include "./SYSTEM/usart/usart.h"

TIM_HandleTypeDef htim6;    // // TIMʱ���ṹ��
TIM_HandleTypeDef htim7;    // // TIMʱ���ṹ��

/*
 * ע�⣺TIM_TimeBaseInitTypeDef�ṹ��������5����Ա��TIM6��TIM7�ļĴ�������ֻ��
 * TIM_Prescaler��TIM_Period������ʹ��TIM6��TIM7��ʱ��ֻ���ʼ����������Ա���ɣ�
 * ����������Ա��ͨ�ö�ʱ���͸߼���ʱ������.
 *-----------------------------------------------------------------------------
 * TIM_Prescaler         ����
 * TIM_CounterMode			 TIMx,x[6,7]û�У��������У�������ʱ����
 * TIM_Period            ����
 * TIM_ClockDivision     TIMx,x[6,7]û�У���������(������ʱ��)
 * TIM_RepetitionCounter TIMx,x[1,8]����(�߼���ʱ��)
 *-----------------------------------------------------------------------------
 * ��ʱʱ����㣺
 * ʱ�����ӣ�1/(72MHz/(BASIC_TIMx_PRESCALER+1)) = 1/(72M/72) = 1us
 * ���嶨ʱ�����ڣ�T_Period = 1us * 1000 =1ms
 */
void BASIC_TIM6_Init(void)
{
  htim6.Instance = BASIC_TIM6;                  // TIM6
  htim6.Init.Prescaler = BASIC_TIMx_PRESCALER;  // Ԥ��Ƶϵ����71
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;  // ������ģʽ�����ϼ�����������ʱ��ֻ��ѡ�����ϼ���
  htim6.Init.Period = BASIC_TIMx_PERIOD;        // ��ʱ����ʱ����1000 
  HAL_TIM_Base_Init(&htim6);

  HAL_TIM_Base_Start_IT(&htim6);                       /* ʹ�ܶ�ʱ��x�Ͷ�ʱ�������ж� */
}

void BASIC_TIM7_Init(void)
{
  htim7.Instance = BASIC_TIM7;                  // TIM6
  htim7.Init.Prescaler = BASIC_TIMx_PRESCALER;  // Ԥ��Ƶϵ����71
  htim7.Init.CounterMode = TIM_COUNTERMODE_UP;  // ������ģʽ�����ϼ�����������ʱ��ֻ��ѡ�����ϼ���
  htim7.Init.Period = BASIC_TIMx_PERIOD;        // ��ʱ����ʱ����1000 
  HAL_TIM_Base_Init(&htim7);
  
  HAL_TIM_Base_Start_IT(&htim7);                       /* ʹ�ܶ�ʱ��x�Ͷ�ʱ�������ж� */
}

/**
 * @brief  ʱ�����ж�����
 * @param  ��
 * @retval ��
 */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim)
{
    /* �����ж����ȼ����� */
    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
  
    if (htim->Instance == BASIC_TIM6)
    {
        BASIC_TIM6_RCC_CLK_ENABLE();                /* ʹ��TIMxʱ�� */
        HAL_NVIC_SetPriority(BASIC_TIM6_IRQ, 6, 0); /* ��ռ6�������ȼ�0 */
        HAL_NVIC_EnableIRQ(BASIC_TIM6_IRQ);         /* ����ITMx�ж� */
    }
    if (htim->Instance == BASIC_TIM7)
    {
        BASIC_TIM7_RCC_CLK_ENABLE();                /* ʹ��TIMxʱ�� */
        HAL_NVIC_SetPriority(BASIC_TIM7_IRQ, 3, 0); /* ��ռ3�������ȼ�0 */
        HAL_NVIC_EnableIRQ(BASIC_TIM7_IRQ);         /* ����ITMx�ж� */
    }
  
}

/**
 * @brief       ������ʱ��TIMX�жϷ�����
 * @param       ��
 * @retval      ��
 */
void BASIC_TIM6_INT_FUN(void)
{
    HAL_TIM_IRQHandler(&htim6);  /* ��ʱ���ص����� */
}

void BASIC_TIM7_INT_FUN(void) 
{
    HAL_TIM_IRQHandler(&htim7);  /* ��ʱ���ص����� */
}


// �жϻص�����
// �����жϺ�ִ���жϻص���������
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == BASIC_TIM6)
    {
        printf("TIM6�������У������ȼ�Ϊ6 \r\n");
    }
    if (htim->Instance == BASIC_TIM7)
    {
        printf("TIM7�������У������ȼ�Ϊ3 \r\n");
    }
}

/*********************************************END OF FILE**********************/
