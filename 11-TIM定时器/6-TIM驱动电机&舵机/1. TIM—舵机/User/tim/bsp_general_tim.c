/**
  ******************************************************************************
  * @file    bsp_GENERAL_tim.c
  * @author  STMicroelectronics
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   高级控制定时器定时范例
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F103 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./tim/bsp_general_tim.h"

// TIM时基结构体
TIM_HandleTypeDef  TIM_TimeBaseStructure;
// TIM输出比较配置结构体
TIM_OC_InitTypeDef TIM_OCInitStructure;


/**
  * @brief  配置TIM复用输出PWM时用到的I/O
  * @param  无
  * @retval 无
  */
static void TIMx_GPIO_Config(void) 
{
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*开启定时器相关的GPIO外设时钟*/
	GENERAL_CH3_GPIO_CLK_ENABLE();   // __HAL_RCC_GPIOB_CLK_ENABLE()
	GENERAL_CH4_GPIO_CLK_ENABLE();   // __HAL_RCC_GPIOB_CLK_ENABLE()

	/* 定时器功能引脚初始化 */															   
	GPIO_InitStructure.Pin = GENERAL_CH3_PIN | GENERAL_CH4_PIN;	  // GPIO_PIN_0 |  GPIO_PIN_1
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;    
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH; 	
	HAL_GPIO_Init(GENERAL_CH3_GPIO_PORT, &GPIO_InitStructure);	  // GPIOB
}

/*
 * 注意：TIM_TimeBaseInitTypeDef结构体里面有5个成员，TIM6和TIM7的寄存器里面只有
 * TIM_Prescaler和TIM_Period，所以使用TIM6和TIM7的时候只需初始化这两个成员即可，
 * 另外三个成员是通用定时器和高级定时器才有.
 *-----------------------------------------------------------------------------
 * TIM_Prescaler         都有
 * TIM_CounterMode			 TIMx,x[6,7]没有，其他都有（基本定时器）
 * TIM_Period            都有
 * TIM_ClockDivision     TIMx,x[6,7]没有，其他都有(基本定时器)
 * TIM_RepetitionCounter TIMx,x[1,8]才有(高级定时器)
 *-----------------------------------------------------------------------------
 * 定时时间计算：
 * 时钟因子：1/(72MHz/(TIMx_PRESCALER+1)) = 1/(72M/72) = 1us
 * 定义定时器周期：T_Period = 1us * 20000 =20ms
 * CCR寄存器0，
 * 由设置知，有效电平设置为高，
 * CK_PSC = 72M, (PSC+1) = 72, CCR = 0, (ARR+1) = 20000
 * PWM频率：	Freq = CK_PSC / (PSC + 1) / (ARR + 1) = 72M/72/20000 = 50 (周期：20ms)
 * PWM占空比：	Duty = CCR / (ARR + 1) = 0/20000 = 0
 * PWM分辨率：	Reso = 1 / (ARR + 1) = 1 / 20000  (PWR最小调节量即为分辨率)
 */
static void TIM_Mode_Config(void)
{
	// 开启TIM3_CLK
	GENERAL_TIM_CLK_ENABLE();    // __HAL_RCC_TIM3_CLK_ENABLE()
	
  /*--------------------时基结构体初始化-------------------------*/
	TIM_TimeBaseStructure.Instance = GENERAL_TIM;      // TIM8
  // 自动重装载寄存器的值，累计TIM_Period+1个频率后产生一个更新或者中断
	TIM_TimeBaseStructure.Init.Period = GENERAL_TIM_PERIOD;           // (20000-1) 
  // 时钟预分频数
	TIM_TimeBaseStructure.Init.Prescaler = GENERAL_TIM_PRESCALER;	    // (72-1)
	// 采样时钟分频
	TIM_TimeBaseStructure.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;  // 一般默认选择一分频即可
  // 计数器计数模式，设置为向上计数
	TIM_TimeBaseStructure.Init.CounterMode=TIM_COUNTERMODE_UP;
	// 初始化定时器TIM3  同时初始化时基和PWR
	HAL_TIM_Base_Init(&TIM_TimeBaseStructure);   
	HAL_TIM_PWM_Init(&TIM_TimeBaseStructure);

	/*-------------------时钟源初始化-------------------*/
	TIM_ClockConfigTypeDef sClockSourceConfig;
	// 时钟源选择 TIM 内部时钟源（一般程序默认选择内部时钟源即可）
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;  
  HAL_TIM_ConfigClockSource(&TIM_TimeBaseStructure, &sClockSourceConfig);
	
	// TIM_MasterConfigTypeDef sMasterConfig;   
	// TIM主模式触发DAC配置结构体, 由于不使用, 这里就不对其进行初始化配置了
	
	/*--------------------输出比较结构体初始化-------------------*/		
	// PWM1模式，向上计数：CNT < CCR时，REF置有效电平，CNT ≥ CCR时，REF置无效电平
	TIM_OCInitStructure.OCMode = TIM_OCMODE_PWM1;  	           // 配置为PWM模式1
	// PWM CCR值(捕获/比较值) 设置 (CH3)
	TIM_OCInitStructure.Pulse = GENERAL_TIM_CH3_CCR;           // 500
	// PWM通道1输出引脚有效电平极性（高电平）
	TIM_OCInitStructure.OCPolarity = TIM_OCPOLARITY_HIGH;       

	//初始化通道3输出PWM 
	HAL_TIM_PWM_ConfigChannel(&TIM_TimeBaseStructure,&TIM_OCInitStructure,GENERAL_TIM_CHANNELx1);   // GENERAL_TIM_CHANNELx1  TIM_CHANNEL_3
	
//	// PWM CCR值(捕获/比较值) 设置 (CH4)
//	TIM_OCInitStructure.Pulse = GENERAL_TIM_CH4_CCR;           // 500
//	//初始化通道4输出PWM 
//	HAL_TIM_PWM_ConfigChannel(&TIM_TimeBaseStructure,&TIM_OCInitStructure,GENERAL_TIM_CHANNELx2);   // GENERAL_TIM_CHANNELx2  TIM_CHANNEL_4
}

/**
  * @brief  初始化高级控制定时器定时，1s产生一次中断
  * @param  无
  * @retval 无
  */
void TIMx_Configuration(void)
{
	TIMx_GPIO_Config();	

	TIM_Mode_Config();
}

/**
  * @brief  调整舵机角度
	* @param  Angle: 舵机角度
	*         范围: 0~180°
  * @retval 无
  */
void Servo_SetAngle(float Angle)
{
	// GENERAL_TIM_CHANNELx1  TIM_CHANNEL_3
	__HAL_TIM_SET_COMPARE(&TIM_TimeBaseStructure, GENERAL_TIM_CHANNELx1, (uint16_t)(Angle / 180 * 2000 + 500) );  
//	__HAL_TIM_SET_COMPARE(&TIM_TimeBaseStructure, GENERAL_TIM_CHANNELx2, (uint16_t)(Angle / 180 * 2000 + 500) );  
}

/*********************************************END OF FILE**********************/
