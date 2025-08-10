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
  
#include "./tim/Encoder.h"
#include "./usart/bsp_debug_usart.h"

// TIM时基结构体
TIM_HandleTypeDef  TIM_TimeBaseStructure;

 
/**
  * @brief  配置TIM复用输出PWM时用到的I/O
  * @param  无
  * @retval 无
  */
void HAL_TIM_Encoder_MspInit(TIM_HandleTypeDef* tim_encoderHandle)  
{
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*开启定时器相关的GPIO外设时钟*/
	GENERAL_CH_Encoder_GPIO_CLK_ENABLE();   // __HAL_RCC_GPIOA_CLK_ENABLE()

	/* 定时器功能引脚初始化 */											
	GPIO_InitStructure.Pin = GENERAL_CH1_Encoder_PIN|GENERAL_CH2_Encoder_PIN;	  // GPIO_PIN_0|GPIO_PIN_1
	GPIO_InitStructure.Mode = GPIO_MODE_AF_INPUT;    
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GENERAL_CH_Encoder_GPIO_PORT, &GPIO_InitStructure);	  // GPIOA
}

/*
 * 配置编码器接口;
 */
void TIM_Encoder_Mode_Config(void)
{
	// 开启TIM4_CLK
	GENERAL_TIM_Encoder_CLK_ENABLE();    // __HAL_RCC_TIM5_CLK_ENABLE()
	
  /*--------------------时基结构体初始化-------------------------*/
	TIM_TimeBaseStructure.Instance = GENERAL_TIM_Encoder;      // TIM4
  // 自动重装载寄存器的值，累计TIM_Period+1个频率后产生一个更新或者中断
	TIM_TimeBaseStructure.Init.Period = GENERAL_TIM_Encoder_PERIOD;          // (65536-1) 
  // 时钟预分频数
	TIM_TimeBaseStructure.Init.Prescaler = GENERAL_TIM_Encoder_PRESCALER;	   // (1-1)
	// 采样时钟分频
	TIM_TimeBaseStructure.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;    // 一般默认选择一分频即可
  // 计数器计数模式，设置为向上计数
	TIM_TimeBaseStructure.Init.CounterMode=TIM_COUNTERMODE_UP;          // 这里不起作用
	// 注意！编码器不能进行时钟初始化！
//	// 初始化定时器TIM3  同时初始化时基和PWR
//	HAL_TIM_Base_Init(&TIM_TimeBaseStructure);   
	
	/*-------------------编码器配置初始化-------------------*/
	TIM_Encoder_InitTypeDef sConfig;
	sConfig.EncoderMode = TIM_ENCODERMODE_TI12;         // 编码器接口TI1和TI2双线计数
	sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;        // 编码器上升沿触发计数
	sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;    // 输入捕获通道直连接(CubeMX初始化只能选择这个)
	sConfig.IC1Prescaler = TIM_ICPSC_DIV1;              // 一般默认选择一分频即可
	sConfig.IC1Filter = GENERAL_TIM_ICFilter;           // 编码器滤波设置
	
	sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = GENERAL_TIM_ICFilter;
	HAL_TIM_Encoder_Init(&TIM_TimeBaseStructure, &sConfig);
	
	/*-------------------主从模式配置----------------------*/
	TIM_MasterConfigTypeDef sMasterConfig;
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;    // 主模式复位
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;  // 主从模式禁用
	HAL_TIMEx_MasterConfigSynchronization(&TIM_TimeBaseStructure, &sMasterConfig);
}

/*********************************************END OF FILE**********************/
