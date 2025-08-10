/**
  ******************************************************************************
  * @file    bsp_color_light.c
  * @author  STMicroelectronics
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   呼吸灯驱动
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 STM32 霸道开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://fire-stm32.taobao.com
  *
  ******************************************************************************
  */

#include "./color_led/bsp_color_led.h"

TIM_HandleTypeDef COLOR_TIM;


 /**
  * @brief  配置TIM复用输出PWM时用到的I/O
  * @param  无
  * @retval 无
  */
void HAL_TIM_MspPostInit(TIM_HandleTypeDef* htim)
{
  GPIO_InitTypeDef GPIO_InitStruct;
    
  /* 定时器通道功能引脚端口时钟使能 */
	COLOR_TIM_GPIO_RCC_CLK_ENABLE();

  /* 定时器通道3功能引脚IO初始化 */
  GPIO_InitStruct.Pin = COLOR_RED_TIM_CH3_PIN|COLOR_GREEN_TIM_CH3_PIN|COLOR_BLUE_TIM_CH3_PIN; 
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(COLOR_TIM_GPIO_PORT, &GPIO_InitStruct);
		
	RCC_AFIO_CLK_ENABLE(); 
	AFIO_REMAP_TIM_PARTIAL(); 

}

/**
  * @brief  基本定时器硬件初始化配置
  * @param  无
  * @retval 无
  */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{
	 /* 基本定时器外设时钟使能 */
    COLOR_TIM_RCC_CLK_ENABLE();
}
 
/**
  * @brief  配置TIM输出的PWM信号的模式，如周期、极性
  * @param  无
  * @retval 无
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
  * @brief  停止PWM输出
  * @param  无
  * @retval 无
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
  * @brief  设置RGB LED的颜色
	* @param  rgb:要设置LED显示的颜色值格式RGB888
  * @retval 无
  */
void SetRGBColor(uint32_t rgb)
{
	//根据颜色值修改定时器的比较寄存器值
	__HAL_TIM_SET_COMPARE(&COLOR_TIM, COLOR_RED_TIM_CHANNEL,   (uint8_t)(rgb>>16));  //R
	__HAL_TIM_SET_COMPARE(&COLOR_TIM, COLOR_GREEN_TIM_CHANNEL, (uint8_t)(rgb>>8 ));  //G
	__HAL_TIM_SET_COMPARE(&COLOR_TIM, COLOR_BLUE_TIM_CHANNEL,  (uint8_t)(rgb    ));  //B
}

/**
  * @brief  设置RGB LED的颜色
	* @param  r\g\b:要设置LED显示的颜色值
  * @retval 无
  */
void SetColorValue(uint8_t r,uint8_t g,uint8_t b)
{
	//根据颜色值修改定时器的比较寄存器值
	__HAL_TIM_SET_COMPARE(&COLOR_TIM, COLOR_RED_TIM_CHANNEL,   (uint8_t)r);  //R
	__HAL_TIM_SET_COMPARE(&COLOR_TIM, COLOR_GREEN_TIM_CHANNEL, (uint8_t)g);  //G
	__HAL_TIM_SET_COMPARE(&COLOR_TIM, COLOR_BLUE_TIM_CHANNEL,  (uint8_t)b);  //B
}
/*********************************************END OF FILE**********************/
