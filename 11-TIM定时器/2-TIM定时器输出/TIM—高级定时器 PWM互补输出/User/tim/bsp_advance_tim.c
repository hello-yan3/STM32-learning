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
  
#include "./tim/bsp_advance_tim.h"

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
	ADVANCE_OCPWM_GPIO_CLK_ENABLE();    // __HAL_RCC_GPIOC_CLK_ENABLE()
	ADVANCE_OCNPWM_GPIO_CLK_ENABLE();   // __HAL_RCC_GPIOA_CLK_ENABLE()
	ADVANCE_BKIN_GPIO_CLK_ENABLE();     // __HAL_RCC_GPIOA_CLK_ENABLE()

	/* ��ʱ���������ų�ʼ�� */															   
	GPIO_InitStructure.Pin = ADVANCE_OCPWM_PIN;	  	   	    // GPIO_PIN_6 
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;   	   	      // ���츴��
	GPIO_InitStructure.Pull = GPIO_NOPULL; 
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH; 	
	HAL_GPIO_Init(ADVANCE_OCPWM_GPIO_PORT, &GPIO_InitStructure);	// GPIOC    

	GPIO_InitStructure.Pin = ADVANCE_OCNPWM_PIN;		   	    // GPIO_PIN_7    
	HAL_GPIO_Init(ADVANCE_OCNPWM_GPIO_PORT, &GPIO_InitStructure);	// GPIOA    
	
	GPIO_InitStructure.Pin = ADVANCE_BKIN_PIN;		   	      // GPIO_PIN_6    
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;   	   	      // ����ģʽ
	GPIO_InitStructure.Pull = GPIO_NOPULL; 
	HAL_GPIO_Init(ADVANCE_BKIN_GPIO_PORT, &GPIO_InitStructure);	// GPIOA   
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
static void TIM_Mode_Config(void)
{
	// TIM����ʱ�����ýṹ�壬ֻ�и߼���ʱ������
	TIM_BreakDeadTimeConfigTypeDef TIM_BDTRInitStructure;   
	// TIMʱ��Դѡ��Ĵ���
	TIM_ClockConfigTypeDef sClockSourceConfig;
	// ����TIMx_CLK,x[1,8] 
	ADVANCE_TIM_CLK_ENABLE();     // __HAL_RCC_TIM8_CLK_ENABLE()
	
  /*--------------------ʱ���ṹ���ʼ��-------------------------*/
	TIM_TimeBaseStructure.Instance = ADVANCE_TIM;      // TIM8
  // �Զ���װ�ؼĴ�����ֵ���ۼ�TIM_Period+1��Ƶ�ʺ����һ�����»����ж�
	TIM_TimeBaseStructure.Init.Period = ADVANCED_TIM_PERIOD;          // (1000-1) 
  // ʱ��Ԥ��Ƶ��
	TIM_TimeBaseStructure.Init.Prescaler = ADVANCED_TIM_PRESCALER;	  // (72-1)
	// ����ʱ�ӷ�Ƶ
	TIM_TimeBaseStructure.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;  // һ��Ĭ��ѡ��һ��Ƶ����
  // ����������ģʽ������Ϊ���ϼ���
	TIM_TimeBaseStructure.Init.CounterMode=TIM_COUNTERMODE_UP;
  // �ظ���������ֵ��ֻ�и߼���ʱ������
	TIM_TimeBaseStructure.Init.RepetitionCounter=ADVANCED_TIM_REPETITIONCOUNTER;	 // �ظ�����(1-1)��
	// ��ʼ����ʱ��TIMx, x[1,8]  ͬʱ��ʼ��ʱ����PWR
	HAL_TIM_Base_Init(&TIM_TimeBaseStructure);   
	HAL_TIM_PWM_Init(&TIM_TimeBaseStructure);
	
	/*-------------------ʱ��Դ��ʼ��-------------------*/
	// ʱ��Դѡ�� TIM �ڲ�ʱ��Դ��һ�����Ĭ��ѡ���ڲ�ʱ��Դ���ɣ�
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;  
  HAL_TIM_ConfigClockSource(&TIM_TimeBaseStructure, &sClockSourceConfig);
	
	// TIM_MasterConfigTypeDef sMasterConfig;   
	// TIM��ģʽ����DAC���ýṹ��, ���ڲ�ʹ��, ����Ͳ�������г�ʼ��������
	
	/*--------------------����ȽϽṹ���ʼ��-------------------*/		
	// PWM1ģʽ�����ϼ�����CNT < CCRʱ��REF����Ч��ƽ��CNT �� CCRʱ��REF����Ч��ƽ
	// PWM2ģʽ�����ϼ�����CNT < CCRʱ��REF����Ч��ƽ��CNT �� CCRʱ��REF����Ч��ƽ
	TIM_OCInitStructure.OCMode = TIM_OCMODE_PWM1;  	            // ����ΪPWMģʽ1
	// PWM CCRֵ(����/�Ƚ�ֵ) ���� 
	TIM_OCInitStructure.Pulse = ADVANCED_TIM_CCR;               // 500
	// PWMͨ��1���������Ч��ƽ���ԣ��ߵ�ƽ��
	TIM_OCInitStructure.OCPolarity = TIM_OCPOLARITY_HIGH;       
	// PWMͨ��1�������������Ч��ƽ���ԣ��ߵ�ƽ��
	TIM_OCInitStructure.OCNPolarity = TIM_OCNPOLARITY_HIGH;    
	// ʵ�ⷢ�֣��ı�����Ե�ƽ�����Ӱ�죬ԭ��δ֪
	// ����(ɲ��)״̬�ڼ䣬PWMͨ��1������ż��ԣ��ߵ�ƽ��
	TIM_OCInitStructure.OCIdleState = TIM_OCIDLESTATE_SET;    
	// ����(ɲ��)״̬�ڼ䣬PWM����ͨ��1������ż��ԣ��͵�ƽ��
	TIM_OCInitStructure.OCNIdleState = TIM_OCNIDLESTATE_RESET; 
	//��ʼ��ͨ��1���PWM 
	HAL_TIM_PWM_ConfigChannel(&TIM_TimeBaseStructure,&TIM_OCInitStructure,ADVANCE_TIM_CHANNELx1);

	/*-------------------ɲ���������ṹ���ʼ��-------------------*/
	/* �Զ����ʹ�ܣ���·������ʱ����������� */
	// �ο���https://blog.csdn.net/yaoji1234/article/details/103937394
	// https://blog.csdn.net/weixin_42653531/article/details/88745288
	// ����ģʽ�µĹر�״̬ѡ�����趨 BDTR �Ĵ��� OSSR λ��ֵ��
	TIM_BDTRInitStructure.OffStateRunMode = TIM_OSSR_ENABLE;
	// ����ģʽ�µĹر�״̬ѡ�����趨 BDTR �Ĵ��� OSSI λ��ֵ��
	TIM_BDTRInitStructure.OffStateIDLEMode = TIM_OSSI_ENABLE;
	// �����������ã�BDTR �Ĵ��� LOCK[1:0]λ��ֵ��
	TIM_BDTRInitStructure.LockLevel = TIM_LOCKLEVEL_1;
	// ����Ƚ��ź�����ʱ�����ã�������μ���ɲο� BDTR:UTG[7:0]������
	// �������õ�����ʱ��Ϊ152ns
	TIM_BDTRInitStructure.DeadTime = DEADTIME;
	// BKE ɲ������ʹ��
	TIM_BDTRInitStructure.BreakState = TIM_BREAK_ENABLE;  
	// BKE ɲ�����ܼ���ѡ���������õ͵�ƽ��Ч��������ż�⵽�ߵ�ƽ���ֹͣPWM���������������κβ���
	TIM_BDTRInitStructure.BreakPolarity = TIM_BREAKPOLARITY_LOW; 
	// ɲ���ֶ��ָ�(��ֹ�Զ��ָ�)
	TIM_BDTRInitStructure.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
	HAL_TIMEx_ConfigBreakDeadTime(&TIM_TimeBaseStructure, &TIM_BDTRInitStructure);
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

/*********************************************END OF FILE**********************/
