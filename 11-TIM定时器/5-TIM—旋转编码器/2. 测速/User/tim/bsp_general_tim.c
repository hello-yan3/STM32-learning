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
#include "./usart/bsp_debug_usart.h"

/*
����ʹ��PWM�������ģ��������ź�
�ٶ�ģ������: 1000 
*/
uint16_t OC_Pulse = 1000;
// turn=1, ��ת����ı�
uint8_t turn = 0;


// TIMʱ���ṹ��
TIM_HandleTypeDef  TIM_TimeBaseStructure_OC;
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
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;    
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH; 	
	
	GPIO_InitStructure.Pin = GENERAL_CH3_PIN|GENERAL_CH4_PIN ;	  // GPIO_PIN_0 |  GPIO_PIN_1
	HAL_GPIO_Init(GENERAL_CH3_GPIO_PORT, &GPIO_InitStructure);	  // GPIOB
	
	/* �����ж� */
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
	HAL_NVIC_SetPriority(GENERAL_TIM_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(GENERAL_TIM_IRQn);
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
 */
static void TIM_Mode_Config(void)
{
	// ����TIM3_CLK
	GENERAL_TIM_CLK_ENABLE();    // __HAL_RCC_TIM3_CLK_ENABLE()
	
  /*--------------------ʱ���ṹ���ʼ��-------------------------*/
	TIM_TimeBaseStructure_OC.Instance = GENERAL_TIM;      // TIM3
  // �Զ���װ�ؼĴ�����ֵ���ۼ�TIM_Period+1��Ƶ�ʺ����һ�����»����ж�
	TIM_TimeBaseStructure_OC.Init.Period = GENERAL_TIM_PERIOD;         // (0xFFFF) 
  // ʱ��Ԥ��Ƶ��
	TIM_TimeBaseStructure_OC.Init.Prescaler = GENERAL_TIM_PRESCALER;	  // (72-1)
	// ����ʱ�ӷ�Ƶ
	TIM_TimeBaseStructure_OC.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;  // һ��Ĭ��ѡ��һ��Ƶ����
  // ����������ģʽ������Ϊ���ϼ���
	TIM_TimeBaseStructure_OC.Init.CounterMode=TIM_COUNTERMODE_UP;
	// ��ʼ����ʱ��TIM3  ͬʱ��ʼ��ʱ����PWR
	HAL_TIM_Base_Init(&TIM_TimeBaseStructure_OC);   
	HAL_TIM_OC_Init(&TIM_TimeBaseStructure_OC);

	/*-------------------ʱ��Դ��ʼ��-------------------*/
	TIM_ClockConfigTypeDef sClockSourceConfig;
	// ʱ��Դѡ�� TIM �ڲ�ʱ��Դ��һ�����Ĭ��ѡ���ڲ�ʱ��Դ���ɣ�
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;  
  HAL_TIM_ConfigClockSource(&TIM_TimeBaseStructure_OC, &sClockSourceConfig);
	
	// TIM_MasterConfigTypeDef sMasterConfig;   
	// TIM��ģʽ����DAC���ýṹ��, ���ڲ�ʹ��, ����Ͳ�������г�ʼ��������
	
	/*--------------------����ȽϽṹ���ʼ��-------------------*/		
	/** ����Ƚ�ģʽ���⣺
	����������REF��ƽָ���ǣ�������ŵ�ƽ��
	
	TIM_OCMODE_TIMING ����ģʽ��CNT=CCRʱ��REF��ƽ�������仯���������ã�
	TIM_OCMODE_ACTIVE ��Ч��ƽģʽ��CNT=CCRʱ��REF����Ч��ƽ���������ã�
  TIM_OCMODE_INACTIVE ��Ч��ƽģʽ��CNT=CCRʱ��REF����Ч��ƽ���������ã�
	TIM_OCMODE_FORCED_ACTIVE  ǿ����Ч��ƽģʽ��CCR��Ч��REFǿ��Ϊ��Ч��ƽ���������ã�
	TIM_OCMODE_FORCED_INACTIVE ǿ����Ч��ƽģʽ��CCR��Ч��REFǿ��Ϊ��Ч��ƽ���������ã�
	
	TIM_OCMODE_TOGGLE ��ƽ��תģʽ(����Ƚ�ģʽ) ��CNT=CCRʱ��REF��ƽ��ת�����ã�
	TIM_OCMODE_PWM1 PWM1ģʽ�����ϼ�����CNT < CCRʱ��REF����Ч��ƽ��CNT �� CCRʱ��REF����Ч��ƽ�����ã�
	TIM_OCMODE_PWM2 PWM2ģʽ�����ϼ�����CNT < CCRʱ��REF����Ч��ƽ��CNT �� CCRʱ��REF����Ч��ƽ�����ã�
	
	PWMģʽ���������ļ�����⣬������Ҫ����TIM_OCMODE_TOGGLEģʽ
	CNT=CCR����ƽ��ת���������жϣ���ͬʱ���� HAL_TIM_OC_DelayElapsedCallback() ����Ƚϻص�������
	��ʱ������ж�����������CCRֵ����CNT�ڵ��������õ�CCRֵǰ����ƽ�������ת��
	���ͨ�����ж������ò�ͬCCR����ʵ�ֲ�ͬƵ��/ռ�ձȵ�PWM
	
	�ɴ˿ɼ�������Ƚ�ģʽ��Ҫ����Ϊ��ʵ��ͬһ����ʱ���������ͬƵ�ʵ�PWM��
	���磺��ͨ�����ʱ��ʱСPWM�����ͨ���ֱ������ͬƵ��PWM
	*/
	
	TIM_OCInitStructure.OCMode = TIM_OCMODE_TOGGLE;  	         // ����Ϊ����Ƚ�ģʽ
	
	// PWM CCRֵ(����/�Ƚ�ֵ) ���� (CH3)
	TIM_OCInitStructure.Pulse = OC_Pulse;          
	// PWMͨ��1���������Ч��ƽ���ԣ��͵�ƽ��
	TIM_OCInitStructure.OCPolarity = TIM_OCPOLARITY_LOW;       
	//��ʼ��ͨ��3���PWM 
	HAL_TIM_OC_ConfigChannel(&TIM_TimeBaseStructure_OC,&TIM_OCInitStructure,GENERAL_TIM_CHANNELx1);   // GENERAL_TIM_CHANNELx1  TIM_CHANNEL_3
	
	// PWM CCRֵ(����/�Ƚ�ֵ) ���� (CH4)
	TIM_OCInitStructure.Pulse = 2*OC_Pulse;   
	// PWMͨ��2���������Ч��ƽ���ԣ��͵�ƽ��
	TIM_OCInitStructure.OCPolarity = TIM_OCPOLARITY_LOW;     
	//��ʼ��ͨ��4���PWM 
	HAL_TIM_OC_ConfigChannel(&TIM_TimeBaseStructure_OC,&TIM_OCInitStructure,GENERAL_TIM_CHANNELx2);   // GENERAL_TIM_CHANNELx2  TIM_CHANNEL_4
	
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
  * @brief  ��ʱ���Ƚ��ж�
  * @param  htim����ʱ�����ָ��
	*	@note 	��
  * @retval ��
  */
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
  __IO uint16_t count;
  /*��ȡ��ǰ����*/
  count = __HAL_TIM_GET_COUNTER(htim);

  /*�жϴ����жϵ����ͨ���������µıȽ���ֵ*/
	// ��������һ����ôҲ�벻����bug
	// TIM_CHANNEL_3�����϶�Ӧ0x08�����жϷ���ֵΪ0x04
	// TIM_CHANNEL_4�����϶�Ӧ0x0C�����жϷ���ֵΪ0x08
  if(htim->Channel == GENERAL_TIM_CHANNELx1-4)
  {
		if(turn ==1)
		{
			// ͨ�����ַ����仯����
			__HAL_TIM_SET_COMPARE(htim, GENERAL_TIM_CHANNELx1, count + 4*OC_Pulse);
			turn = 0;
			return;
		}
    __HAL_TIM_SET_COMPARE(htim, GENERAL_TIM_CHANNELx1, count + 2*OC_Pulse);
  }
  if(htim->Channel == GENERAL_TIM_CHANNELx2-4)
  {
    __HAL_TIM_SET_COMPARE(htim, GENERAL_TIM_CHANNELx2, count + 2*OC_Pulse);
  }
}
/*********************************************END OF FILE**********************/

