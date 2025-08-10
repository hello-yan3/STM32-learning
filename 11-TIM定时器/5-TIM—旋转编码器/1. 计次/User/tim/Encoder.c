/**
  ******************************************************************************
  * @file    bsp_GENERAL_tim.c
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
  
#include "./tim/Encoder.h"
#include "./usart/bsp_debug_usart.h"

// TIMʱ���ṹ��
TIM_HandleTypeDef  TIM_TimeBaseStructure;

 
/**
  * @brief  ����TIM�������PWMʱ�õ���I/O
  * @param  ��
  * @retval ��
  */
void HAL_TIM_Encoder_MspInit(TIM_HandleTypeDef* tim_encoderHandle)  
{
	/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*������ʱ����ص�GPIO����ʱ��*/
	GENERAL_CH_Encoder_GPIO_CLK_ENABLE();   // __HAL_RCC_GPIOA_CLK_ENABLE()

	/* ��ʱ���������ų�ʼ�� */											
	GPIO_InitStructure.Pin = GENERAL_CH1_Encoder_PIN|GENERAL_CH2_Encoder_PIN;	  // GPIO_PIN_0|GPIO_PIN_1
	GPIO_InitStructure.Mode = GPIO_MODE_AF_INPUT;    
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GENERAL_CH_Encoder_GPIO_PORT, &GPIO_InitStructure);	  // GPIOA
}

/*
 * ���ñ������ӿ�;
 */
void TIM_Encoder_Mode_Config(void)
{
	// ����TIM4_CLK
	GENERAL_TIM_Encoder_CLK_ENABLE();    // __HAL_RCC_TIM5_CLK_ENABLE()
	
  /*--------------------ʱ���ṹ���ʼ��-------------------------*/
	TIM_TimeBaseStructure.Instance = GENERAL_TIM_Encoder;      // TIM4
  // �Զ���װ�ؼĴ�����ֵ���ۼ�TIM_Period+1��Ƶ�ʺ����һ�����»����ж�
	TIM_TimeBaseStructure.Init.Period = GENERAL_TIM_Encoder_PERIOD;          // (65536-1) 
  // ʱ��Ԥ��Ƶ��
	TIM_TimeBaseStructure.Init.Prescaler = GENERAL_TIM_Encoder_PRESCALER;	   // (1-1)
	// ����ʱ�ӷ�Ƶ
	TIM_TimeBaseStructure.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;    // һ��Ĭ��ѡ��һ��Ƶ����
  // ����������ģʽ������Ϊ���ϼ���
	TIM_TimeBaseStructure.Init.CounterMode=TIM_COUNTERMODE_UP;          // ���ﲻ������
	// ע�⣡���������ܽ���ʱ�ӳ�ʼ����
//	// ��ʼ����ʱ��TIM3  ͬʱ��ʼ��ʱ����PWR
//	HAL_TIM_Base_Init(&TIM_TimeBaseStructure);   
	
	/*-------------------���������ó�ʼ��-------------------*/
	TIM_Encoder_InitTypeDef sConfig;
	sConfig.EncoderMode = TIM_ENCODERMODE_TI12;         // �������ӿ�TI1��TI2˫�߼���
	sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;        // �����������ش�������
	sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;    // ���벶��ͨ��ֱ����(CubeMX��ʼ��ֻ��ѡ�����)
	sConfig.IC1Prescaler = TIM_ICPSC_DIV1;              // һ��Ĭ��ѡ��һ��Ƶ����
	sConfig.IC1Filter = GENERAL_TIM_ICFilter;           // �������˲�����
	
	sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = GENERAL_TIM_ICFilter;
	HAL_TIM_Encoder_Init(&TIM_TimeBaseStructure, &sConfig);
	
	/*-------------------����ģʽ����----------------------*/
	TIM_MasterConfigTypeDef sMasterConfig;
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;    // ��ģʽ��λ
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;  // ����ģʽ����
	HAL_TIMEx_MasterConfigSynchronization(&TIM_TimeBaseStructure, &sMasterConfig);
}

/*********************************************END OF FILE**********************/
