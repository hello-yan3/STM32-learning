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
  
#include "./tim/bsp_general_tim_IC.h"
#include "./usart/bsp_debug_usart.h"

// TIMʱ���ṹ��
TIM_HandleTypeDef  TIM_TimeBaseStructure_IC;

/**
  * @brief  ����TIM�������PWMʱ�õ���I/O
  * @param  ��
  * @retval ��
  */
static void TIMx_IC_GPIO_Config(void) 
{
	/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*������ʱ����ص�GPIO����ʱ��*/
	GENERAL_TIM_IC_GPIO_CLK_ENABLE();   // __HAL_RCC_GPIOA_CLK_ENABLE()

	/* ��ʱ���������ų�ʼ�� */											
	GPIO_InitStructure.Pin = GENERAL_TIM_IC_PIN;	  // GPIO_PIN_7
	GPIO_InitStructure.Mode = GPIO_MODE_AF_INPUT;   
	// ������������,��ʱ���ڳ�����ģ�������������������޷������ߵ�ƽ����
	//GPIO_InitStructure.Pull = GPIO_PULLDOWN;     
	// ���ø�������,��ʱ�������ŵ�ƽ��ȷ�������ܻ�һֱ���벶���жϺ���
	GPIO_InitStructure.Pull = GPIO_NOPULL;        // ������������
	HAL_GPIO_Init(GENERAL_TIM_IC_GPIO_PORT, &GPIO_InitStructure);	  // GPIOA
	
	/* ���ö�ʱ���ж����ȼ���ʹ�� */
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
  HAL_NVIC_SetPriority(GENERAL_TIM_IRQn, 0, 0);   // TIM3_IRQn
  HAL_NVIC_EnableIRQ(GENERAL_TIM_IRQn);           // TIM3_IRQn
}


static void TIM_IC_Mode_Config(void)
{
	// ����TIM3_CLK
	GENERAL_TIM_IC_CLK_ENABLE();    // __HAL_RCC_TIM3_CLK_ENABLE()
	
  /*--------------------ʱ���ṹ���ʼ��-------------------------*/
	TIM_TimeBaseStructure_IC.Instance = GENERAL_TIM_IC;      // TIM3
  // �Զ���װ�ؼĴ�����ֵ��������CNT�Ĵ������ֵ
	TIM_TimeBaseStructure_IC.Init.Period = GENERAL_TIM_IC_PERIOD;          // (0xFFFF) 
  // ʱ��Ԥ��Ƶ��
	TIM_TimeBaseStructure_IC.Init.Prescaler = GENERAL_TIM_IC_PRESCALER;	   // (72-1)
	// ����ʱ�ӷ�Ƶ
	TIM_TimeBaseStructure_IC.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;    // һ��Ĭ��ѡ��һ��Ƶ����
  // ����������ģʽ������Ϊ���ϼ���
	TIM_TimeBaseStructure_IC.Init.CounterMode=TIM_COUNTERMODE_UP;

	// ��ʼ����ʱ��TIM3  ͬʱ��ʼ��ʱ����PWR
	HAL_TIM_Base_Init(&TIM_TimeBaseStructure_IC);   
	HAL_TIM_IC_Init(&TIM_TimeBaseStructure_IC);

	/*-------------------ʱ��Դ��ʼ��-------------------*/
	TIM_ClockConfigTypeDef sClockSourceConfig;
	// ʱ��Դѡ�� TIM �ڲ�ʱ��Դ��һ�����Ĭ��ѡ���ڲ�ʱ��Դ���ɣ�
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;  
  HAL_TIM_ConfigClockSource(&TIM_TimeBaseStructure_IC, &sClockSourceConfig);
	
	/*-----------------��ʹ������ģʽ--------------------*/
	// ��ģʽ����
	TIM_MasterConfigTypeDef sMasterConfig;
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;    // ��ģʽ��λ
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;  // ����ģʽ����
	HAL_TIMEx_MasterConfigSynchronization(&TIM_TimeBaseStructure_IC, &sMasterConfig);
	
//	// ��ģʽ����
//	TIM_SlaveConfigTypeDef sSlaveConfig;
//	// ��ģʽѡ�񣺴�ģʽ��λ(���յ�����Դ�źţ�������λ(��CNT����))
//	sSlaveConfig.SlaveMode = TIM_SLAVEMODE_RESET;    
//	// ����Դѡ��TI1FP1 ��������ͨ��TI1�������˲����󽫽ӵ���׽�Ƚ�ͨ��IC1
//  sSlaveConfig.InputTrigger = TIM_TS_TI1FP1;       
//	// ��⵽�������źţ������ģʽ(������Դ�ź�Ϊͨ��1�������ź�)
//  sSlaveConfig.TriggerPolarity = TIM_INPUTCHANNELPOLARITY_RISING;  
//  sSlaveConfig.TriggerPrescaler = TIM_ICPSC_DIV1;      // ��Ƶ������Ƶ
//	// �����˲���ֵ(�˲�������Ϊ�����Ӳ��źţ���Ӱ�������ź�Ƶ��)
//  sSlaveConfig.TriggerFilter = GENERAL_TIM_ICFilter;   // 0x00        
//  HAL_TIM_SlaveConfigSynchro(&TIM_TimeBaseStructure_IC, &sSlaveConfig);
//	
//	// ����ģʽ����Ϊ��CNT�������Զ����㣬
//	// ͨ��3��4��֧������ģʽ����ˣ�ֻ�����ж����ֶ���0
	
	/*--------------------���벶��ṹ���ʼ��-------------------*/		
	// TIM���벶�����ýṹ��
	TIM_IC_InitTypeDef TIM_ICInitStructure;
	// ��ͨ��1�������ش������벶��
	TIM_ICInitStructure.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING; 
	// TI1FP1 ��������ͨ��TI1�������˲����󽫽ӵ���׽�Ƚ�ͨ��IC1
  TIM_ICInitStructure.ICSelection = TIM_ICSELECTION_DIRECTTI;     
  // ��Ƶ������Ƶ
  TIM_ICInitStructure.ICPrescaler = TIM_ICPSC_DIV1; 
  // �����˲���ֵ(�˲�������Ϊ�����Ӳ��źţ���Ӱ�������ź�Ƶ��)	
  TIM_ICInitStructure.ICFilter = GENERAL_TIM_ICFilter; // 0x00             
	HAL_TIM_IC_ConfigChannel(&TIM_TimeBaseStructure_IC, &TIM_ICInitStructure, GENERAL_TIM_IC_CHANNELx1);   // GENERAL_TIM_IC_CHANNELx1  TIM_CHANNEL_1
	
	// ��ʼ�����ֱ�ӽ����жϣ��Խ������Ӱ�죬�����Ҫ��ǰ�ر�
	__HAL_TIM_CLEAR_IT(&TIM_TimeBaseStructure_IC, TIM_IT_UPDATE);
	__HAL_TIM_CLEAR_IT(&TIM_TimeBaseStructure_IC, TIM_IT_CC2);
}

/**
  * @brief  ��ʼ���߼����ƶ�ʱ����ʱ��1s����һ���ж�
  * @param  ��
  * @retval ��
  */
void TIMx_IC_Configuration(void)
{
	TIMx_IC_GPIO_Config();	
	TIM_IC_Mode_Config();
}

/*********************************************END OF FILE**********************/
