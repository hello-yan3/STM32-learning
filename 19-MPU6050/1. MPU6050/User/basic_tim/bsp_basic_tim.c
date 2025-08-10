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
  
#include "./basic_tim/bsp_basic_tim.h"

extern __IO uint16_t time ; // ms ��ʱ����
TIM_HandleTypeDef htimx;    // // TIMʱ���ṹ��

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
void BASIC_TIMx_Init(void)
{
	// ������ʱ����ӵ�ж�ʱ�жϡ���ģʽ����DAC�Ĺ���
	// TIM_MasterConfigTypeDef������ģʽ����DAC���
	// ����ֻ���ڶ�ʱ���ò����ù��ܣ�
	// ��˿ɲ��Դ˽ṹ���������
	TIM_MasterConfigTypeDef sMasterConfig;        // �˽ṹ�弰�����ÿ�ֱ��ע��

  htimx.Instance = BASIC_TIMx;                  // TIM6
  htimx.Init.Prescaler = BASIC_TIMx_PRESCALER;  // Ԥ��Ƶϵ����71
  htimx.Init.CounterMode = TIM_COUNTERMODE_UP;  // ������ģʽ�����ϼ�����������ʱ��ֻ��ѡ�����ϼ���
  htimx.Init.Period = BASIC_TIMx_PERIOD;        // ��ʱ����ʱ����1000 
  HAL_TIM_Base_Init(&htimx);

	// ������ģʽ����DAC���ܣ���ֱ��ע��
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&htimx, &sMasterConfig);
	
	/* ���ж�ģʽ��������ʱ�� */
	HAL_TIM_Base_Start_IT(&htimx);
}

/**
 * @brief  ʱ�����ж�����
 * @param  ��
 * @retval ��
 */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{
   /* ������ʱ������ʱ��ʹ�� */
    BASIC_TIM_RCC_CLK_ENABLE();                 // __HAL_RCC_TIM6_CLK_ENABLE()

		/* �����ж����ȼ����� */
		HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
    /* �����ж����� */
    HAL_NVIC_SetPriority(BASIC_TIM_IRQ, 1, 0);  // TIM6_IRQn
    HAL_NVIC_EnableIRQ(BASIC_TIM_IRQ);          // TIM6_IRQn
}

// �жϻص�����
// �����жϺ�ִ���жϻص���������
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  time++;
}

/*********************************************END OF FILE**********************/
