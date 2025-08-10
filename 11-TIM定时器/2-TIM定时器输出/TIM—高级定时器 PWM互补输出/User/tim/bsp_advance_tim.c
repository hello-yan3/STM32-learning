/**
  ******************************************************************************
  * @file    bsp_advance_tim.c
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
  
#include "./tim/bsp_advance_tim.h"

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
	ADVANCE_OCPWM_GPIO_CLK_ENABLE();    // __HAL_RCC_GPIOC_CLK_ENABLE()
	ADVANCE_OCNPWM_GPIO_CLK_ENABLE();   // __HAL_RCC_GPIOA_CLK_ENABLE()
	ADVANCE_BKIN_GPIO_CLK_ENABLE();     // __HAL_RCC_GPIOA_CLK_ENABLE()

	/* 定时器功能引脚初始化 */															   
	GPIO_InitStructure.Pin = ADVANCE_OCPWM_PIN;	  	   	    // GPIO_PIN_6 
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;   	   	      // 推挽复用
	GPIO_InitStructure.Pull = GPIO_NOPULL; 
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH; 	
	HAL_GPIO_Init(ADVANCE_OCPWM_GPIO_PORT, &GPIO_InitStructure);	// GPIOC    

	GPIO_InitStructure.Pin = ADVANCE_OCNPWM_PIN;		   	    // GPIO_PIN_7    
	HAL_GPIO_Init(ADVANCE_OCNPWM_GPIO_PORT, &GPIO_InitStructure);	// GPIOA    
	
	GPIO_InitStructure.Pin = ADVANCE_BKIN_PIN;		   	      // GPIO_PIN_6    
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;   	   	      // 输入模式
	GPIO_InitStructure.Pull = GPIO_NOPULL; 
	HAL_GPIO_Init(ADVANCE_BKIN_GPIO_PORT, &GPIO_InitStructure);	// GPIOA   
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
 * 定义定时器周期：T_Period = 1us * 1000 =1ms
 * CCR寄存器500，即0-500ms有效电平，500ms-999ms无效电平
 * 由设置知，有效电平设置为高，
 * CK_PSC = 72M, (PSC+1) = 72, CCR = 500, (ARR+1) = 1000
 * PWM频率：	Freq = CK_PSC / (PSC + 1) / (ARR + 1) = 72M/72/1000 = 1000 (周期：1ms)
 * PWM占空比：	Duty = CCR / (ARR + 1) = 500/1000 = 0.5
 * PWM分辨率：	Reso = 1 / (ARR + 1) = 1 / 1000  (PWR最小调节量即为分辨率)
 */
static void TIM_Mode_Config(void)
{
	// TIM死区时间配置结构体，只有高级定时器才有
	TIM_BreakDeadTimeConfigTypeDef TIM_BDTRInitStructure;   
	// TIM时钟源选择寄存器
	TIM_ClockConfigTypeDef sClockSourceConfig;
	// 开启TIMx_CLK,x[1,8] 
	ADVANCE_TIM_CLK_ENABLE();     // __HAL_RCC_TIM8_CLK_ENABLE()
	
  /*--------------------时基结构体初始化-------------------------*/
	TIM_TimeBaseStructure.Instance = ADVANCE_TIM;      // TIM8
  // 自动重装载寄存器的值，累计TIM_Period+1个频率后产生一个更新或者中断
	TIM_TimeBaseStructure.Init.Period = ADVANCED_TIM_PERIOD;          // (1000-1) 
  // 时钟预分频数
	TIM_TimeBaseStructure.Init.Prescaler = ADVANCED_TIM_PRESCALER;	  // (72-1)
	// 采样时钟分频
	TIM_TimeBaseStructure.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;  // 一般默认选择一分频即可
  // 计数器计数模式，设置为向上计数
	TIM_TimeBaseStructure.Init.CounterMode=TIM_COUNTERMODE_UP;
  // 重复计数器的值，只有高级定时器才有
	TIM_TimeBaseStructure.Init.RepetitionCounter=ADVANCED_TIM_REPETITIONCOUNTER;	 // 重复计数(1-1)次
	// 初始化定时器TIMx, x[1,8]  同时初始化时基和PWR
	HAL_TIM_Base_Init(&TIM_TimeBaseStructure);   
	HAL_TIM_PWM_Init(&TIM_TimeBaseStructure);
	
	/*-------------------时钟源初始化-------------------*/
	// 时钟源选择 TIM 内部时钟源（一般程序默认选择内部时钟源即可）
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;  
  HAL_TIM_ConfigClockSource(&TIM_TimeBaseStructure, &sClockSourceConfig);
	
	// TIM_MasterConfigTypeDef sMasterConfig;   
	// TIM主模式触发DAC配置结构体, 由于不使用, 这里就不对其进行初始化配置了
	
	/*--------------------输出比较结构体初始化-------------------*/		
	// PWM1模式，向上计数：CNT < CCR时，REF置有效电平，CNT ≥ CCR时，REF置无效电平
	// PWM2模式，向上计数：CNT < CCR时，REF置无效电平，CNT ≥ CCR时，REF置有效电平
	TIM_OCInitStructure.OCMode = TIM_OCMODE_PWM1;  	            // 配置为PWM模式1
	// PWM CCR值(捕获/比较值) 设置 
	TIM_OCInitStructure.Pulse = ADVANCED_TIM_CCR;               // 500
	// PWM通道1输出引脚有效电平极性（高电平）
	TIM_OCInitStructure.OCPolarity = TIM_OCPOLARITY_HIGH;       
	// PWM通道1互补输出引脚有效电平极性（高电平）
	TIM_OCInitStructure.OCNPolarity = TIM_OCNPOLARITY_HIGH;    
	// 实测发现，改变参数对电平输出无影响，原因未知
	// 空闲(刹车)状态期间，PWM通道1输出引脚极性（高电平）
	TIM_OCInitStructure.OCIdleState = TIM_OCIDLESTATE_SET;    
	// 空闲(刹车)状态期间，PWM互补通道1输出引脚极性（低电平）
	TIM_OCInitStructure.OCNIdleState = TIM_OCNIDLESTATE_RESET; 
	//初始化通道1输出PWM 
	HAL_TIM_PWM_ConfigChannel(&TIM_TimeBaseStructure,&TIM_OCInitStructure,ADVANCE_TIM_CHANNELx1);

	/*-------------------刹车和死区结构体初始化-------------------*/
	/* 自动输出使能，断路、死区时间和锁定配置 */
	// 参考：https://blog.csdn.net/yaoji1234/article/details/103937394
	// https://blog.csdn.net/weixin_42653531/article/details/88745288
	// 运行模式下的关闭状态选择，它设定 BDTR 寄存器 OSSR 位的值。
	TIM_BDTRInitStructure.OffStateRunMode = TIM_OSSR_ENABLE;
	// 空闲模式下的关闭状态选择，它设定 BDTR 寄存器 OSSI 位的值。
	TIM_BDTRInitStructure.OffStateIDLEMode = TIM_OSSI_ENABLE;
	// 锁定级别配置，BDTR 寄存器 LOCK[1:0]位的值。
	TIM_BDTRInitStructure.LockLevel = TIM_LOCKLEVEL_1;
	// 输出比较信号死区时间配置，具体如何计算可参考 BDTR:UTG[7:0]的描述
	// 这里配置的死区时间为152ns
	TIM_BDTRInitStructure.DeadTime = DEADTIME;
	// BKE 刹车功能使能
	TIM_BDTRInitStructure.BreakState = TIM_BREAK_ENABLE;  
	// BKE 刹车功能极性选择，这里设置低电平有效，如果引脚检测到高电平则会停止PWM的输出，不会产生任何波形
	TIM_BDTRInitStructure.BreakPolarity = TIM_BREAKPOLARITY_LOW; 
	// 刹车手动恢复(禁止自动恢复)
	TIM_BDTRInitStructure.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
	HAL_TIMEx_ConfigBreakDeadTime(&TIM_TimeBaseStructure, &TIM_BDTRInitStructure);
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

/*********************************************END OF FILE**********************/
