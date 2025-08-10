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


//计算PWM表有多少个元素
uint16_t POINT_NUM = sizeof(indexWave)/sizeof(indexWave[0]);     // 128


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
  /* 若输出的周期数大于period_class(1)，输出下一种脉冲宽的PWM波 */
	/* 单次进入中断时间5.68889ms, 实际输出下一种PWM波时间1*5.68889ms=5.68889ms */
	/* 累计128数据点, 单周期市场: 5.68889ms*128 = 0.728178s */
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
