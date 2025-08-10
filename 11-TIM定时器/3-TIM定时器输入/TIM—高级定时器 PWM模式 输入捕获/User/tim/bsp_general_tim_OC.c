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
  
#include "./tim/bsp_general_tim_OC.h"

// TIMʱ���ṹ��
TIM_HandleTypeDef  TIM_TimeBaseStructure_OC; 


/**
  * @brief  ����TIM�������PWMʱ�õ���I/O
  * @param  ��
  * @retval ��
  */
static void TIMx_OC_GPIO_Config(void) 
{
	/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*������ʱ����ص�GPIO����ʱ��*/
	GENERAL_TIM_OC_GPIO_CLK_ENABLE();   // __HAL_RCC_GPIOA_CLK_ENABLE()
	
	/* ��ʱ���������ų�ʼ�� */															   
	GPIO_InitStructure.Pin = GENERAL_TIM_OC_PIN;	  // GPIO_PIN_7
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;    
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH; 	
	HAL_GPIO_Init(GENERAL_TIM_OC_GPIO_PORT, &GPIO_InitStructure);	  // GPIOB
}

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
 * ʱ�����ӣ�1/(72MHz/(TIMx_PRESCALER+1)) = 1/(72M/72) = 1us
 * ���嶨ʱ�����ڣ�T_Period = 1us * 1000 =1ms
 * CCR�Ĵ���500����0-500ms��Ч��ƽ��500ms-999ms��Ч��ƽ
 * ������֪����Ч��ƽ����Ϊ�ߣ�
 * CK_PSC = 72M, (PSC+1) = 72, CCR = 500, (ARR+1) = 1000
 * PWMƵ�ʣ�	Freq = CK_PSC / (PSC + 1) / (ARR + 1) = 72M/72/1000 = 1000 (���ڣ�1ms)
 * PWMռ�ձȣ�	Duty = CCR / (ARR + 1) = 500/1000 = 0.5
 * PWM�ֱ��ʣ�	Reso = 1 / (ARR + 1) = 1 / 1000  (PWR��С��������Ϊ�ֱ���)
 */
static void TIM_OC_Mode_Config(void)
{
	// ����TIM2_CLK
	GENERAL_TIM_OC_CLK_ENABLE();    // __HAL_RCC_TIM3_CLK_ENABLE()
	
  /*--------------------ʱ���ṹ���ʼ��-------------------------*/
	TIM_TimeBaseStructure_OC.Instance = GENERAL_TIM_OC;      // TIM3
  // �Զ���װ�ؼĴ�����ֵ���ۼ�TIM_Period+1��Ƶ�ʺ����һ�����»����ж�
	TIM_TimeBaseStructure_OC.Init.Period = GENERAL_TIM_OC_PERIOD;          // (5000-1) 
  // ʱ��Ԥ��Ƶ��
	TIM_TimeBaseStructure_OC.Init.Prescaler = GENERAL_TIM_OC_PRESCALER;	   // (72-1)
	// ����ʱ�ӷ�Ƶ
	TIM_TimeBaseStructure_OC.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;    // һ��Ĭ��ѡ��һ��Ƶ����
  // ����������ģʽ������Ϊ���ϼ���
	TIM_TimeBaseStructure_OC.Init.CounterMode=TIM_COUNTERMODE_UP;
	// ��ʼ����ʱ��TIM3  ͬʱ��ʼ��ʱ����PWR
	HAL_TIM_Base_Init(&TIM_TimeBaseStructure_OC);   
	HAL_TIM_PWM_Init(&TIM_TimeBaseStructure_OC);
	
	/*-------------------ʱ��Դ��ʼ��-------------------*/
	TIM_ClockConfigTypeDef sClockSourceConfig;
	// ʱ��Դѡ�� TIM �ڲ�ʱ��Դ��һ�����Ĭ��ѡ���ڲ�ʱ��Դ���ɣ�
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;  
  HAL_TIM_ConfigClockSource(&TIM_TimeBaseStructure_OC, &sClockSourceConfig);
	
	// TIM_MasterConfigTypeDef sMasterConfig;   
	// TIM��ģʽ����DAC���ýṹ��, ���ڲ�ʹ��, ����Ͳ�������г�ʼ��������
	
	/*--------------------����ȽϽṹ���ʼ��-------------------*/		
	// TIM����Ƚ����ýṹ��
	TIM_OC_InitTypeDef TIM_OCInitStructure;
	// PWM1ģʽ�����ϼ�����CNT < CCRʱ��REF����Ч��ƽ��CNT �� CCRʱ��REF����Ч��ƽ
	TIM_OCInitStructure.OCMode = TIM_OCMODE_PWM1;  	           // ����ΪPWMģʽ1
	// PWM CCRֵ(����/�Ƚ�ֵ) ���� 
	TIM_OCInitStructure.Pulse = GENERAL_TIM_OC_CH_CCR;        // 2500
	// PWMͨ��1���������Ч��ƽ���ԣ��ߵ�ƽ��
	TIM_OCInitStructure.OCPolarity = TIM_OCPOLARITY_HIGH;  
	//���ÿ������ģʽ
	TIM_OCInitStructure.OCFastMode = TIM_OCFAST_DISABLE;

	//��ʼ��ͨ��3���PWM 
	HAL_TIM_PWM_ConfigChannel(&TIM_TimeBaseStructure_OC,&TIM_OCInitStructure,GENERAL_TIM_OC_CHANNELx1);   // GENERAL_TIM_CHANNELx1  TIM_CHANNEL_3
}

/**
  * @brief  ��ʼ���߼����ƶ�ʱ����ʱ��1s����һ���ж�
  * @param  ��
  * @retval ��
  */
void TIMx_OC_Configuration(void)
{
	TIMx_OC_GPIO_Config();	

	TIM_OC_Mode_Config();
}

/*********************************************END OF FILE**********************/
