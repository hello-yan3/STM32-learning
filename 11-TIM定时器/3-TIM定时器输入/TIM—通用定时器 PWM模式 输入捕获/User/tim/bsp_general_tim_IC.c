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

extern uint16_t CCR1;    
extern uint16_t CCR2;	
extern uint8_t  flag;	

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
	GENERAL_CH_IC_GPIO_CLK_ENABLE();   // __HAL_RCC_GPIOC_CLK_ENABLE()

	/* 定时器功能引脚初始化 */											
	GPIO_InitStructure.Pin = GENERAL_CH_IC_PIN;	  // GPIO_PIN_6
	GPIO_InitStructure.Mode = GPIO_MODE_AF_INPUT;    
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GENERAL_CH_IC_GPIO_PORT, &GPIO_InitStructure);	  // GPIOC
	
	__HAL_RCC_AFIO_CLK_ENABLE();         // 开启AFIO的时钟
	GENERAL_CH_IC_AFIO();                // IO口重映射设置
	
	/* 配置定时器中断优先级并使能 */
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
  HAL_NVIC_SetPriority(GENERAL_TIM_IRQn, 0, 0);   // TIM5_IRQn
  HAL_NVIC_EnableIRQ(GENERAL_TIM_IRQn);           // TIM5_IRQn
}

/*
 * 测量PWM波两种方法：
 * 测频法：即每个上升沿，进入一次中断，统计一个时间周期T内，进入中断次数COUNT
 * PWM周期为：COUNT/T，试用于高频信号
 * 另一种，即通过输入捕获测量周期
 * 预分频后周期为：fclk，则一个周期时间为：1/fclk
 * 统计计数器次数N，
 * 则PWM周期为：N/fclk，试用于低频信号
 * 
 * 这里使用输入捕获方法测量周期
 * 假设PWM波频率1000Hz，周期1ms
 * 设置预分频器值为：(72-1) ，即fclk：72M/72=1M，周期1us
 * ARR计数器值设置为最大：(65536-1)，则计数器值可累计最大数为65535
 * 
 * 设置TIM_Channel1读取上升沿信号，
 * 当检测到第二个上升沿信号，此时CNT值会加载到CCR1寄存器中
 * 读取CCR1寄存器，则得到计数器次数N，之后中断手动清零CNT(或者配置主从模式清零CNT)
 * 设置TIM_Channel2读取下降沿信号，
 * 当检测到下降沿信号，此时CNT值会加载到CCR2寄存器中
 * 读取CCR2寄存器，则得到计数器次数N2，此时N2/N即占空比
 * 若只需知道方波频率，则无需配置TIM_Channel2
 */
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
	
	/*-----------------主从模式配置--------------------*/
	// 主模式配置
	TIM_MasterConfigTypeDef sMasterConfig;
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;    // 主模式复位
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;  // 主从模式禁用
	HAL_TIMEx_MasterConfigSynchronization(&TIM_TimeBaseStructure_IC, &sMasterConfig);
	
	// 从模式配置
	TIM_SlaveConfigTypeDef sSlaveConfig;
	// 从模式选择：从模式复位(即收到触发源信号(开始信号)，产生复位(将CNT清零))
	sSlaveConfig.SlaveMode = TIM_SLAVEMODE_RESET;    
	// 触发源选择：TI1FP1 是来自于通道TI1，经过滤波器后将接到捕捉比较通道IC1
  sSlaveConfig.InputTrigger = TIM_TS_TI1FP1;       
	// 检测到上升沿信号，进入从模式(即触发源信号为通道1上升沿信号)
  sSlaveConfig.TriggerPolarity = TIM_INPUTCHANNELPOLARITY_RISING;  
  sSlaveConfig.TriggerPrescaler = TIM_ICPSC_DIV1;      // 分频器不分频
	// 输入滤波器值(滤波器作用为过滤杂波信号，不影响输入信号频率)
  sSlaveConfig.TriggerFilter = GENERAL_TIM_ICFilter;   // 0x00        
  HAL_TIM_SlaveConfigSynchro(&TIM_TimeBaseStructure_IC, &sSlaveConfig);
	
	// 主从模式作用为：CNT计数器自动清零，
	// 通道3、4不支持主从模式，因此，只能在中断中手动清0
	
	/*--------------------输入捕获结构体初始化-------------------*/		
	// TIM输入捕获配置结构体
	TIM_IC_InitTypeDef TIM_ICInitStructure;
	// 主通道1，上升沿触发输入捕获
	TIM_ICInitStructure.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING; 
	// TI1FP1 是来自于通道TI1，经过滤波器后将接到捕捉比较通道IC1，所以为直连模式
  TIM_ICInitStructure.ICSelection = TIM_ICSELECTION_DIRECTTI;     
  // 分频器不分频
  TIM_ICInitStructure.ICPrescaler = TIM_ICPSC_DIV1; 
  // 输入滤波器值(滤波器作用为过滤杂波信号，不影响输入信号频率)	
  TIM_ICInitStructure.ICFilter = GENERAL_TIM_ICFilter; // 0x00             
	HAL_TIM_IC_ConfigChannel(&TIM_TimeBaseStructure_IC, &TIM_ICInitStructure, GENERAL_TIM_IC_CHANNELx1);   // GENERAL_TIM_IC_CHANNELx1  TIM_CHANNEL_1
	
  // 主通道2，下降沿触发输入捕获
	TIM_ICInitStructure.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING; 
  // TI1FP2 是来自于通道TI1，经过滤波器后将接到捕捉比较通道IC2，所以为非直连模式
	TIM_ICInitStructure.ICSelection = TIM_ICSELECTION_INDIRECTTI;     
	HAL_TIM_IC_ConfigChannel(&TIM_TimeBaseStructure_IC, &TIM_ICInitStructure, GENERAL_TIM_IC_CHANNELx2);   // GENERAL_TIM_IC_CHANNELx2  TIM_CHANNEL_2
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

/*中断回调函数，输入捕获后进入此中断*/
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim == &TIM_TimeBaseStructure_IC)
	{
		// https://wenku.csdn.net/answer/9a99c796d9ff461f8adbdf04e8bb0e71
		// HAL_TIM_ACTIVE_CHANNEL_1 与 TIM_CHANNEL_1 区别
		if(htim ->Channel == GENERAL_TIM_IC_ACTIVE_CHANNELx1 )    // HAL_TIM_ACTIVE_CHANNEL_1
		{
			flag = 1;
			// 这里使用 __HAL_TIM_GET_COMPARE() 函数也可
			CCR1 = HAL_TIM_ReadCapturedValue(&TIM_TimeBaseStructure_IC,GENERAL_TIM_IC_CHANNELx1);
			CCR2 = HAL_TIM_ReadCapturedValue(&TIM_TimeBaseStructure_IC,GENERAL_TIM_IC_CHANNELx2);
		}		
	}
}
/*********************************************END OF FILE**********************/
