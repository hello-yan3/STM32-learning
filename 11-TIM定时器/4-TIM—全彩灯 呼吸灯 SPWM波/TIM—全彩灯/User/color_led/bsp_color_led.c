/**
  ******************************************************************************
  * @file    bsp_color_light.c
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

#include "./color_led/bsp_color_led.h"

TIM_HandleTypeDef COLOR_TIM;


 /**
  * @brief  ����TIM�������PWMʱ�õ���I/O
  * @param  ��
  * @retval ��
  */
void HAL_TIM_MspPostInit(TIM_HandleTypeDef* htim)
{
  GPIO_InitTypeDef GPIO_InitStruct;
    
  /* ��ʱ��ͨ���������Ŷ˿�ʱ��ʹ�� */
	COLOR_TIM_GPIO_RCC_CLK_ENABLE();

  /* ��ʱ��ͨ��3��������IO��ʼ�� */
  GPIO_InitStruct.Pin = COLOR_RED_TIM_CH3_PIN|COLOR_GREEN_TIM_CH3_PIN|COLOR_BLUE_TIM_CH3_PIN; 
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(COLOR_TIM_GPIO_PORT, &GPIO_InitStruct);
		
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
    COLOR_TIM_RCC_CLK_ENABLE();
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
  
  COLOR_TIM.Instance = COLOR_TIMx;
  COLOR_TIM.Init.Prescaler = COLOR_TIM_PRESCALER;
  COLOR_TIM.Init.CounterMode = TIM_COUNTERMODE_UP;
  COLOR_TIM.Init.Period = COLOR_TIM_PERIOD;
  COLOR_TIM.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
  HAL_TIM_Base_Init(&COLOR_TIM);
	HAL_TIM_PWM_Init(&COLOR_TIM);

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  HAL_TIM_ConfigClockSource(&COLOR_TIM, &sClockSourceConfig);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&COLOR_TIM, &sMasterConfig);
  
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW; 
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.Pulse = 0;  
  HAL_TIM_PWM_ConfigChannel(&COLOR_TIM, &sConfigOC, COLOR_RED_TIM_CHANNEL);
	HAL_TIM_PWM_ConfigChannel(&COLOR_TIM, &sConfigOC, COLOR_GREEN_TIM_CHANNEL);
	HAL_TIM_PWM_ConfigChannel(&COLOR_TIM, &sConfigOC, COLOR_BLUE_TIM_CHANNEL); 

	HAL_TIM_Base_MspInit(&COLOR_TIM);
  HAL_TIM_MspPostInit(&COLOR_TIM);
}

/**
  * @brief  ֹͣPWM���
  * @param  ��
  * @retval ��
  */
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle)
{
	SetColorValue(0,0,0);
  COLOR_TIM_RCC_CLK_DISABLE();

	HAL_GPIO_WritePin(COLOR_RED_TIM_CH3_PORT,   COLOR_RED_TIM_CH3_PIN,   GPIO_PIN_SET);
	HAL_GPIO_WritePin(COLOR_GREEN_TIM_CH3_PORT, COLOR_GREEN_TIM_CH3_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(COLOR_BLUE_TIM_CH3_PORT,  COLOR_BLUE_TIM_CH3_PIN,  GPIO_PIN_SET);
}

/**
  * @brief  ����RGB LED����ɫ
	* @param  rgb:Ҫ����LED��ʾ����ɫֵ��ʽRGB888
  * @retval ��
  */
void SetRGBColor(uint32_t rgb)
{
	//������ɫֵ�޸Ķ�ʱ���ıȽϼĴ���ֵ
	__HAL_TIM_SET_COMPARE(&COLOR_TIM, COLOR_RED_TIM_CHANNEL,   (uint8_t)(rgb>>16));  //R
	__HAL_TIM_SET_COMPARE(&COLOR_TIM, COLOR_GREEN_TIM_CHANNEL, (uint8_t)(rgb>>8 ));  //G
	__HAL_TIM_SET_COMPARE(&COLOR_TIM, COLOR_BLUE_TIM_CHANNEL,  (uint8_t)(rgb    ));  //B
}

/**
  * @brief  ����RGB LED����ɫ
	* @param  r\g\b:Ҫ����LED��ʾ����ɫֵ
  * @retval ��
  */
void SetColorValue(uint8_t r,uint8_t g,uint8_t b)
{
	//������ɫֵ�޸Ķ�ʱ���ıȽϼĴ���ֵ
	__HAL_TIM_SET_COMPARE(&COLOR_TIM, COLOR_RED_TIM_CHANNEL,   (uint8_t)r);  //R
	__HAL_TIM_SET_COMPARE(&COLOR_TIM, COLOR_GREEN_TIM_CHANNEL, (uint8_t)g);  //G
	__HAL_TIM_SET_COMPARE(&COLOR_TIM, COLOR_BLUE_TIM_CHANNEL,  (uint8_t)b);  //B
}
/*********************************************END OF FILE**********************/
