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
  
#include "./tim/bsp_general_tim_IC.h"
#include "./usart/bsp_debug_usart.h"

// TIM时基结构体
TIM_HandleTypeDef  TIM_TimeBaseStructure_IC;

/**
  * @brief  配置TIM复用输出PWM时用到的I/O
  * @param  无
  * @retval 无
  */
static void TIMx_IC_GPIO_Config(void) 
{
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*开启定时器相关的GPIO外设时钟*/
	GENERAL_TIM_IC_GPIO_CLK_ENABLE();   // __HAL_RCC_GPIOA_CLK_ENABLE()

	/* 定时器功能引脚初始化 */											
	GPIO_InitStructure.Pin = GENERAL_TIM_IC_PIN;	  // GPIO_PIN_7
	GPIO_InitStructure.Mode = GPIO_MODE_AF_INPUT;   
	// 设置下拉输入,此时由于超声波模块驱动能力弱，可能无法产生高电平脉冲
	//GPIO_InitStructure.Pull = GPIO_PULLDOWN;     
	// 设置浮空输入,此时由于引脚电平不确定，可能会一直进入捕获中断函数
	GPIO_InitStructure.Pull = GPIO_NOPULL;        // 设置下拉输入
	HAL_GPIO_Init(GENERAL_TIM_IC_GPIO_PORT, &GPIO_InitStructure);	  // GPIOA
	
	/* 配置定时器中断优先级并使能 */
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
  HAL_NVIC_SetPriority(GENERAL_TIM_IRQn, 0, 0);   // TIM3_IRQn
  HAL_NVIC_EnableIRQ(GENERAL_TIM_IRQn);           // TIM3_IRQn
}


static void TIM_IC_Mode_Config(void)
{
	// 开启TIM3_CLK
	GENERAL_TIM_IC_CLK_ENABLE();    // __HAL_RCC_TIM3_CLK_ENABLE()
	
  /*--------------------时基结构体初始化-------------------------*/
	TIM_TimeBaseStructure_IC.Instance = GENERAL_TIM_IC;      // TIM3
  // 自动重装载寄存器的值，这里是CNT寄存器最大值
	TIM_TimeBaseStructure_IC.Init.Period = GENERAL_TIM_IC_PERIOD;          // (0xFFFF) 
  // 时钟预分频数
	TIM_TimeBaseStructure_IC.Init.Prescaler = GENERAL_TIM_IC_PRESCALER;	   // (72-1)
	// 采样时钟分频
	TIM_TimeBaseStructure_IC.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;    // 一般默认选择一分频即可
  // 计数器计数模式，设置为向上计数
	TIM_TimeBaseStructure_IC.Init.CounterMode=TIM_COUNTERMODE_UP;

	// 初始化定时器TIM3  同时初始化时基和PWR
	HAL_TIM_Base_Init(&TIM_TimeBaseStructure_IC);   
	HAL_TIM_IC_Init(&TIM_TimeBaseStructure_IC);

	/*-------------------时钟源初始化-------------------*/
	TIM_ClockConfigTypeDef sClockSourceConfig;
	// 时钟源选择 TIM 内部时钟源（一般程序默认选择内部时钟源即可）
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;  
  HAL_TIM_ConfigClockSource(&TIM_TimeBaseStructure_IC, &sClockSourceConfig);
	
	/*-----------------不使用主从模式--------------------*/
	// 主模式配置
	TIM_MasterConfigTypeDef sMasterConfig;
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;    // 主模式复位
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;  // 主从模式禁用
	HAL_TIMEx_MasterConfigSynchronization(&TIM_TimeBaseStructure_IC, &sMasterConfig);
	
//	// 从模式配置
//	TIM_SlaveConfigTypeDef sSlaveConfig;
//	// 从模式选择：从模式复位(即收到触发源信号，产生复位(将CNT清零))
//	sSlaveConfig.SlaveMode = TIM_SLAVEMODE_RESET;    
//	// 触发源选择：TI1FP1 是来自于通道TI1，经过滤波器后将接到捕捉比较通道IC1
//  sSlaveConfig.InputTrigger = TIM_TS_TI1FP1;       
//	// 检测到上升沿信号，进入从模式(即触发源信号为通道1上升沿信号)
//  sSlaveConfig.TriggerPolarity = TIM_INPUTCHANNELPOLARITY_RISING;  
//  sSlaveConfig.TriggerPrescaler = TIM_ICPSC_DIV1;      // 分频器不分频
//	// 输入滤波器值(滤波器作用为过滤杂波信号，不影响输入信号频率)
//  sSlaveConfig.TriggerFilter = GENERAL_TIM_ICFilter;   // 0x00        
//  HAL_TIM_SlaveConfigSynchro(&TIM_TimeBaseStructure_IC, &sSlaveConfig);
//	
//	// 主从模式作用为：CNT计数器自动清零，
//	// 通道3、4不支持主从模式，因此，只能在中断中手动清0
	
	/*--------------------输入捕获结构体初始化-------------------*/		
	// TIM输入捕获配置结构体
	TIM_IC_InitTypeDef TIM_ICInitStructure;
	// 主通道1，上升沿触发输入捕获
	TIM_ICInitStructure.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING; 
	// TI1FP1 是来自于通道TI1，经过滤波器后将接到捕捉比较通道IC1
  TIM_ICInitStructure.ICSelection = TIM_ICSELECTION_DIRECTTI;     
  // 分频器不分频
  TIM_ICInitStructure.ICPrescaler = TIM_ICPSC_DIV1; 
  // 输入滤波器值(滤波器作用为过滤杂波信号，不影响输入信号频率)	
  TIM_ICInitStructure.ICFilter = GENERAL_TIM_ICFilter; // 0x00             
	HAL_TIM_IC_ConfigChannel(&TIM_TimeBaseStructure_IC, &TIM_ICInitStructure, GENERAL_TIM_IC_CHANNELx1);   // GENERAL_TIM_IC_CHANNELx1  TIM_CHANNEL_1
	
	// 初始化后会直接进入中断，对结果带来影响，因此需要提前关闭
	__HAL_TIM_CLEAR_IT(&TIM_TimeBaseStructure_IC, TIM_IT_UPDATE);
	__HAL_TIM_CLEAR_IT(&TIM_TimeBaseStructure_IC, TIM_IT_CC2);
}

/**
  * @brief  初始化高级控制定时器定时，1s产生一次中断
  * @param  无
  * @retval 无
  */
void TIMx_IC_Configuration(void)
{
	TIMx_IC_GPIO_Config();	
	TIM_IC_Mode_Config();
}

/*********************************************END OF FILE**********************/
