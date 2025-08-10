/**
  ******************************************************************************
  * @file    bsp_advance_tim.c
  * @author  STMicroelectronics
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   �߼����ƶ�ʱ����ʱ����
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F103 ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./tim/bsp_general_tim.h"

extern uint32_t time;

// TIMʱ���ṹ��
TIM_HandleTypeDef TIM_TimeBaseStructure;

/*
 * ע�⣺TIM_TimeBaseInitTypeDef�ṹ��������5����Ա��TIM6��TIM7�ļĴ�������ֻ��
 * TIM_Prescaler��TIM_Period������ʹ��TIM6��TIM7��ʱ��ֻ���ʼ����������Ա���ɣ�
 * ����������Ա��ͨ�ö�ʱ���͸߼���ʱ������.
 *-----------------------------------------------------------------------------
 *typedef struct
 *{ TIM_Prescaler            ����
 *	TIM_CounterMode			     TIMx,x[6,7]û�У���������
 *  TIM_Period               ����
 *  TIM_ClockDivision        TIMx,x[6,7]û�У���������
 *  TIM_RepetitionCounter    TIMx,x[1,8,15,16,17]����
 *}TIM_TimeBaseInitTypeDef; 
 *-----------------------------------------------------------------------------
 * ��ʱʱ����㣺
 * ʱ�����ӣ�1/(72MHz/(TIMx_PRESCALER+1)) = 1/(72M/72) = 1us
 * ���嶨ʱ�����ڣ�T_Period = 1us * 1000 =1ms
 * �ظ�������TIM_REPETITIONCOUNTER = 10������ʵ�ʼ�ʱʱ��T_Period = 1ms * 10 =10ms
 */
 void TIM_Mode_Config(void)
{
	// TIMʱ��Դѡ��ṹ��
	TIM_ClockConfigTypeDef sClockSourceConfig;
  // TIM��ģʽ����DAC���ýṹ��
	TIM_MasterConfigTypeDef sMasterConfig;
  
  TIM_TimeBaseStructure.Instance = GENERAL_TIMx;      // TIM1
  
  // ʱ��Ԥ��Ƶ��
  TIM_TimeBaseStructure.Init.Prescaler = GENERAL_TIM_PRESCALER;  // (72-1)
  // ����������ģʽ������Ϊ���ϼ���
  TIM_TimeBaseStructure.Init.CounterMode = TIM_COUNTERMODE_UP;
  // �Զ���װ�ؼĴ�����ֵ���ۼ�TIM_Period+1��Ƶ�ʺ����һ�����»����ж�
  TIM_TimeBaseStructure.Init.Period = GENERAL_TIM_PERIOD;        // (1000-1) 
  // ʱ�ӷ�Ƶ���� 
  TIM_TimeBaseStructure.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;// һ��Ĭ��ѡ��һ��Ƶ����
  // ��ʼ����ʱ��
  HAL_TIM_Base_Init(&TIM_TimeBaseStructure);

	// ʱ��Դѡ�� TIM �ڲ�ʱ��Դ��һ�����Ĭ��ѡ���ڲ�ʱ��Դ���ɣ�
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;  
  HAL_TIM_ConfigClockSource(&TIM_TimeBaseStructure, &sClockSourceConfig);

	// ������ģʽ����DAC����
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&TIM_TimeBaseStructure, &sMasterConfig);
}

 /**
  * @brief  ʱ�����ж�����
  * @param  ��
  * @retval ��
  */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{  
    /* ������ʱ������ʱ��ʹ�� */
    GENERAL_TIM_RCC_CLK_ENABLE();   // __HAL_RCC_TIM1_CLK_ENABLE()

    /* �����ж����� */
    HAL_NVIC_SetPriority(GENERAL_TIM_IRQn , 1, 0);   // TIM1_UP_IRQn
    HAL_NVIC_EnableIRQ(GENERAL_TIM_IRQn );           // TIM1_UP_IRQn
}

// �жϻص�����
// �����жϺ�ִ���жϻص���������
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  time++;
}

/*********************************************END OF FILE**********************/
