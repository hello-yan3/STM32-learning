/**
  ******************************************************************************
  * @file    bsp_breathing.c
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

#include "./breathinglight/bsp_breathing.h"

TIM_HandleTypeDef BRE_TIM;

/* LED亮度等级 PWM表,指数曲线 ，此表使用工程目录下的python脚本index_wave.py生成*/
/* 使用MATLAB作图，亮度如图所示 */
uint16_t indexWave[] = {
1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 4,
4, 5, 5, 6, 7, 8, 9, 10, 11, 13,
15, 17, 19, 22, 25, 28, 32, 36,
41, 47, 53, 61, 69, 79, 89, 102,
116, 131, 149, 170, 193, 219, 250,   
284, 323, 367, 417, 474, 539, 613,
697, 792, 901, 1024, 1024, 901, 792,
697, 613, 539, 474, 417, 367, 323,
284, 250, 219, 193, 170, 149, 131, 
116, 102, 89, 79, 69, 61, 53, 47, 41,
36, 32, 28, 25, 22, 19, 17, 15, 13, 
11, 10, 9, 8, 7, 6, 5, 5, 4, 4, 3, 3,
2, 2, 2, 2, 1, 1, 1, 1
};


//计算PWM表有多少个元素
uint16_t POINT_NUM = sizeof(indexWave)/sizeof(indexWave[0]);    // 110


 /**
  * @brief  配置TIM复用输出PWM时用到的I/O
  * @param  无
  * @retval 无
  */
void HAL_TIM_MspPostInit(TIM_HandleTypeDef* htim)
{
  GPIO_InitTypeDef GPIO_InitStruct;
    
  /* 定时器通道功能引脚端口时钟使能 */
  BRE_TIM_GPIO_RCC_CLK_ENABLE();

  /* 定时器通道3功能引脚IO初始化 */
  GPIO_InitStruct.Pin = BRE_TIM_CH3_PIN; 
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(BRE_TIM_CH3_PORT, &GPIO_InitStruct);
	
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
    BRE_TIM_RCC_CLK_ENABLE();
    
    /* 配置定时器中断优先级并使能 */
    HAL_NVIC_SetPriority(BRE_TIM_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(BRE_TIM_IRQn);
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


/* 中断回调函数 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  static uint8_t pwm_index = 1;			/* 用于PWM查表 */
	static uint8_t period_cnt = 0;		/* 用于计算周期数 */

  period_cnt++;
  /* 若输出的周期数大于period_class(10)，输出下一种脉冲宽的PWM波 */
	/* 单次进入中断时间2.8444ms, 实际输出下一种PWM波时间10*2.8444ms=22.84ms */
	/* 累计110数据点, 单周期市场: 22.84ms*110 = 3.128889s */
  if(period_cnt >= period_class)										
  {
    /* 根据PWM表修改定时器的比较寄存器值 */
    __HAL_TIM_SET_COMPARE(&BRE_TIM,TIM_CHANNEL,indexWave[pwm_index]);

    /* 标志PWM表的下一个元素 */
    pwm_index++;												
    /* 若PWM脉冲表已经输出完成一遍，重置PWM查表标志 */
    if( pwm_index >=  POINT_NUM)								
    {
      pwm_index=0;								
    }
    /* 重置周期计数标志 */
    period_cnt=0;												
  }
}

/*********************************************END OF FILE**********************/
