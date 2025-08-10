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
  
#include "./tim/bsp_general_tim.h"

// TIMʱ���ṹ��
TIM_HandleTypeDef  TIM_TimeBaseStructure;
// TIM����Ƚ����ýṹ��
TIM_OC_InitTypeDef TIM_OCInitStructure;


/**
  * @brief  ����TIM�������PWMʱ�õ���I/O
  * @param  ��
  * @retval ��
  */
static void TIMx_GPIO_Config(void) 
{
	/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*������ʱ����ص�GPIO����ʱ��*/
	GENERAL_CH3_GPIO_CLK_ENABLE();   // __HAL_RCC_GPIOB_CLK_ENABLE()
	GENERAL_CH4_GPIO_CLK_ENABLE();   // __HAL_RCC_GPIOB_CLK_ENABLE()

	/* ��ʱ���������ų�ʼ�� */															   
	GPIO_InitStructure.Pin = GENERAL_CH3_PIN | GENERAL_CH4_PIN;	  // GPIO_PIN_0 |  GPIO_PIN_1
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;    
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH; 	
	HAL_GPIO_Init(GENERAL_CH3_GPIO_PORT, &GPIO_InitStructure);	  // GPIOB
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
 * ���嶨ʱ�����ڣ�T_Period = 1us * 20000 =20ms
 * CCR�Ĵ���0��
 * ������֪����Ч��ƽ����Ϊ�ߣ�
 * CK_PSC = 72M, (PSC+1) = 72, CCR = 0, (ARR+1) = 20000
 * PWMƵ�ʣ�	Freq = CK_PSC / (PSC + 1) / (ARR + 1) = 72M/72/20000 = 50 (���ڣ�20ms)
 * PWMռ�ձȣ�	Duty = CCR / (ARR + 1) = 0/20000 = 0
 * PWM�ֱ��ʣ�	Reso = 1 / (ARR + 1) = 1 / 20000  (PWR��С��������Ϊ�ֱ���)
 */
static void TIM_Mode_Config(void)
{
	// ����TIM3_CLK
	GENERAL_TIM_CLK_ENABLE();    // __HAL_RCC_TIM3_CLK_ENABLE()
	
  /*--------------------ʱ���ṹ���ʼ��-------------------------*/
	TIM_TimeBaseStructure.Instance = GENERAL_TIM;      // TIM8
  // �Զ���װ�ؼĴ�����ֵ���ۼ�TIM_Period+1��Ƶ�ʺ����һ�����»����ж�
	TIM_TimeBaseStructure.Init.Period = GENERAL_TIM_PERIOD;           // (20000-1) 
  // ʱ��Ԥ��Ƶ��
	TIM_TimeBaseStructure.Init.Prescaler = GENERAL_TIM_PRESCALER;	    // (72-1)
	// ����ʱ�ӷ�Ƶ
	TIM_TimeBaseStructure.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;  // һ��Ĭ��ѡ��һ��Ƶ����
  // ����������ģʽ������Ϊ���ϼ���
	TIM_TimeBaseStructure.Init.CounterMode=TIM_COUNTERMODE_UP;
	// ��ʼ����ʱ��TIM3  ͬʱ��ʼ��ʱ����PWR
	HAL_TIM_Base_Init(&TIM_TimeBaseStructure);   
	HAL_TIM_PWM_Init(&TIM_TimeBaseStructure);

	/*-------------------ʱ��Դ��ʼ��-------------------*/
	TIM_ClockConfigTypeDef sClockSourceConfig;
	// ʱ��Դѡ�� TIM �ڲ�ʱ��Դ��һ�����Ĭ��ѡ���ڲ�ʱ��Դ���ɣ�
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;  
  HAL_TIM_ConfigClockSource(&TIM_TimeBaseStructure, &sClockSourceConfig);
	
	// TIM_MasterConfigTypeDef sMasterConfig;   
	// TIM��ģʽ����DAC���ýṹ��, ���ڲ�ʹ��, ����Ͳ�������г�ʼ��������
	
	/*--------------------����ȽϽṹ���ʼ��-------------------*/		
	// PWM1ģʽ�����ϼ�����CNT < CCRʱ��REF����Ч��ƽ��CNT �� CCRʱ��REF����Ч��ƽ
	TIM_OCInitStructure.OCMode = TIM_OCMODE_PWM1;  	           // ����ΪPWMģʽ1
	// PWM CCRֵ(����/�Ƚ�ֵ) ���� (CH3)
	TIM_OCInitStructure.Pulse = GENERAL_TIM_CH3_CCR;           // 500
	// PWMͨ��1���������Ч��ƽ���ԣ��ߵ�ƽ��
	TIM_OCInitStructure.OCPolarity = TIM_OCPOLARITY_HIGH;       

	//��ʼ��ͨ��3���PWM 
	HAL_TIM_PWM_ConfigChannel(&TIM_TimeBaseStructure,&TIM_OCInitStructure,GENERAL_TIM_CHANNELx1);   // GENERAL_TIM_CHANNELx1  TIM_CHANNEL_3
	
//	// PWM CCRֵ(����/�Ƚ�ֵ) ���� (CH4)
//	TIM_OCInitStructure.Pulse = GENERAL_TIM_CH4_CCR;           // 500
//	//��ʼ��ͨ��4���PWM 
//	HAL_TIM_PWM_ConfigChannel(&TIM_TimeBaseStructure,&TIM_OCInitStructure,GENERAL_TIM_CHANNELx2);   // GENERAL_TIM_CHANNELx2  TIM_CHANNEL_4
}

/**
  * @brief  ��ʼ���߼����ƶ�ʱ����ʱ��1s����һ���ж�
  * @param  ��
  * @retval ��
  */
void TIMx_Configuration(void)
{
	TIMx_GPIO_Config();	

	TIM_Mode_Config();
}

/**
  * @brief  ��������Ƕ�
	* @param  Angle: ����Ƕ�
	*         ��Χ: 0~180��
  * @retval ��
  */
void Servo_SetAngle(float Angle)
{
	// GENERAL_TIM_CHANNELx1  TIM_CHANNEL_3
	__HAL_TIM_SET_COMPARE(&TIM_TimeBaseStructure, GENERAL_TIM_CHANNELx1, (uint16_t)(Angle / 180 * 2000 + 500) );  
//	__HAL_TIM_SET_COMPARE(&TIM_TimeBaseStructure, GENERAL_TIM_CHANNELx2, (uint16_t)(Angle / 180 * 2000 + 500) );  
}

/*********************************************END OF FILE**********************/
