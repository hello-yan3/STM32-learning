/**
  ******************************************************************************
  * @file    bsp_breathing.c
  * @author  STMicroelectronics
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ����������
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� STM32 �Ե������� 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://fire-stm32.taobao.com
  *
  ******************************************************************************
  */

#include "./breathinglight/bsp_breathing.h"

TIM_HandleTypeDef BRE_TIM;

/* LED���ȵȼ� PWM��,ָ������ ���˱�ʹ�ù���Ŀ¼�µ�python�ű�index_wave.py����*/
/* ʹ��MATLAB��ͼ��������ͼ��ʾ */
uint16_t indexWave[] = {
	2148,2249,2349,2448,2546,2643,2738,2832,
	2924,3013,3101,3186,3268,3347,3423,3496,
	3565,3631,3693,3751,3805,3854,3899,3940,
	3976,4008,4035,4057,4074,4086,4094,4095,
	4094,4086,4074,4057,4035,4008,3976,3940,
	3899,3854,3805,3751,3693,3631,3565,3496,
	3423,3347,3268,3186,3101,3013,2924,2832,
	2738,2643,2546,2448,2349,2249,2148,2048,
	1948,1847,1747,1648,1550,1453,1358,1264,
	1172,1083,995 ,910 ,828 ,749 ,673 ,600 ,
  531 ,465 ,403 ,345 ,291 ,242 ,197 ,156 ,
	120 ,88  ,61  ,39  ,22  ,10  ,2   ,0   ,
	2   ,10  ,22  ,39  ,61  ,88  ,120 ,156 ,
	197 ,242 ,291 ,345 ,403 ,465 ,531 ,600 ,
	673 ,749 ,828 ,910 ,995 ,1083,1172,1264,
	1358,1453,1550,1648,1747,1847,1948,2048
};


//����PWM���ж��ٸ�Ԫ��
uint16_t POINT_NUM = sizeof(indexWave)/sizeof(indexWave[0]);     // 128


 /**
  * @brief  ����TIM�������PWMʱ�õ���I/O
  * @param  ��
  * @retval ��
  */
void HAL_TIM_MspPostInit(TIM_HandleTypeDef* htim)
{
  GPIO_InitTypeDef GPIO_InitStruct;
    
  /* ��ʱ��ͨ���������Ŷ˿�ʱ��ʹ�� */
  BRE_TIM_GPIO_RCC_CLK_ENABLE();

  /* ��ʱ��ͨ��3��������IO��ʼ�� */
  GPIO_InitStruct.Pin = BRE_TIM_CH3_PIN; 
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(BRE_TIM_CH3_PORT, &GPIO_InitStruct);
	
	RCC_AFIO_CLK_ENABLE(); 
	AFIO_REMAP_TIM_PARTIAL(); 
}

/**
  * @brief  ������ʱ��Ӳ����ʼ������
  * @param  ��
  * @retval ��
  */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{
	 /* ������ʱ������ʱ��ʹ�� */
    BRE_TIM_RCC_CLK_ENABLE();
    
    /* ���ö�ʱ���ж����ȼ���ʹ�� */
    HAL_NVIC_SetPriority(BRE_TIM_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(BRE_TIM_IRQn);
}

/**
  * @brief  ����TIM�����PWM�źŵ�ģʽ�������ڡ�����
  * @param  ��
  * @retval ��
  */
void TIMx_Mode_Config(void)
{
	TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;
  TIM_OC_InitTypeDef sConfigOC;
  
  BRE_TIM.Instance = BRE_TIMx;
  BRE_TIM.Init.Prescaler = BRE_TIM_PRESCALER;
  BRE_TIM.Init.CounterMode = TIM_COUNTERMODE_UP;
  BRE_TIM.Init.Period = BRE_TIM_PERIOD;
  BRE_TIM.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
  HAL_TIM_Base_Init(&BRE_TIM);
	HAL_TIM_PWM_Init(&BRE_TIM);

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  HAL_TIM_ConfigClockSource(&BRE_TIM, &sClockSourceConfig);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&BRE_TIM, &sMasterConfig);
  
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW; 
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.Pulse = indexWave[0];  
  HAL_TIM_PWM_ConfigChannel(&BRE_TIM, &sConfigOC, TIM_CHANNEL);

	HAL_TIM_Base_MspInit(&BRE_TIM);
  HAL_TIM_MspPostInit(&BRE_TIM);
}


/* �жϻص����� */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  static uint8_t pwm_index = 1;			/* ����PWM��� */
	static uint8_t period_cnt = 0;		/* ���ڼ��������� */

  period_cnt++;
  /* �����������������period_class(1)�������һ��������PWM�� */
	/* ���ν����ж�ʱ��5.68889ms, ʵ�������һ��PWM��ʱ��1*5.68889ms=5.68889ms */
	/* �ۼ�128���ݵ�, �������г�: 5.68889ms*128 = 0.728178s */
  if(period_cnt >= period_class)										
  {
    /* ����PWM���޸Ķ�ʱ���ıȽϼĴ���ֵ */
    __HAL_TIM_SET_COMPARE(&BRE_TIM,TIM_CHANNEL,indexWave[pwm_index]);

    /* ��־PWM�����һ��Ԫ�� */
    pwm_index++;												
    /* ��PWM������Ѿ�������һ�飬����PWM����־ */
    if( pwm_index >=  POINT_NUM)								
    {
      pwm_index=0;								
    }
    /* �������ڼ�����־ */
    period_cnt=0;												
  }
}

/*********************************************END OF FILE**********************/
