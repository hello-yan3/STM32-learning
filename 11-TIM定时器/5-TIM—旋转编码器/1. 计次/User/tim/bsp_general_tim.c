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
#include "./usart/bsp_debug_usart.h"

/*
这里使用PWM输出脉冲模拟编码器信号
假定模拟脉冲: 1000 
*/
uint16_t OC_Pulse = 1000;
// turn=1, 旋转方向改变
uint8_t turn = 0;


// TIM时基结构体
TIM_HandleTypeDef  TIM_TimeBaseStructure_OC;
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
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;    
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH; 	
	
	GPIO_InitStructure.Pin = GENERAL_CH3_PIN|GENERAL_CH4_PIN ;	  // GPIO_PIN_0 |  GPIO_PIN_1
	HAL_GPIO_Init(GENERAL_CH3_GPIO_PORT, &GPIO_InitStructure);	  // GPIOB
	
	/* 设置中断 */
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
	HAL_NVIC_SetPriority(GENERAL_TIM_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(GENERAL_TIM_IRQn);
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
 */
static void TIM_Mode_Config(void)
{
	// 开启TIM3_CLK
	GENERAL_TIM_CLK_ENABLE();    // __HAL_RCC_TIM3_CLK_ENABLE()
	
  /*--------------------时基结构体初始化-------------------------*/
	TIM_TimeBaseStructure_OC.Instance = GENERAL_TIM;      // TIM3
  // 自动重装载寄存器的值，累计TIM_Period+1个频率后产生一个更新或者中断
	TIM_TimeBaseStructure_OC.Init.Period = GENERAL_TIM_PERIOD;         // (0xFFFF) 
  // 时钟预分频数
	TIM_TimeBaseStructure_OC.Init.Prescaler = GENERAL_TIM_PRESCALER;	  // (72-1)
	// 采样时钟分频
	TIM_TimeBaseStructure_OC.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;  // 一般默认选择一分频即可
  // 计数器计数模式，设置为向上计数
	TIM_TimeBaseStructure_OC.Init.CounterMode=TIM_COUNTERMODE_UP;
	// 初始化定时器TIM3  同时初始化时基和PWR
	HAL_TIM_Base_Init(&TIM_TimeBaseStructure_OC);   
	HAL_TIM_OC_Init(&TIM_TimeBaseStructure_OC);

	/*-------------------时钟源初始化-------------------*/
	TIM_ClockConfigTypeDef sClockSourceConfig;
	// 时钟源选择 TIM 内部时钟源（一般程序默认选择内部时钟源即可）
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;  
  HAL_TIM_ConfigClockSource(&TIM_TimeBaseStructure_OC, &sClockSourceConfig);
	
	// TIM_MasterConfigTypeDef sMasterConfig;   
	// TIM主模式触发DAC配置结构体, 由于不使用, 这里就不对其进行初始化配置了
	
	/*--------------------输出比较结构体初始化-------------------*/		
	/** 输出比较模式讲解：
	以下所讲述REF电平指的是，输出引脚电平；
	
	TIM_OCMODE_TIMING 冻结模式，CNT=CCR时，REF电平不发生变化，几乎不用；
	TIM_OCMODE_ACTIVE 有效电平模式，CNT=CCR时，REF置有效电平，几乎不用；
  TIM_OCMODE_INACTIVE 无效电平模式，CNT=CCR时，REF置无效电平，几乎不用；
	TIM_OCMODE_FORCED_ACTIVE  强制有效电平模式，CCR无效，REF强制为有效电平，几乎不用；
	TIM_OCMODE_FORCED_INACTIVE 强制无效电平模式，CCR无效，REF强制为无效电平，几乎不用；
	
	TIM_OCMODE_TOGGLE 电平翻转模式(输出比较模式) ，CNT=CCR时，REF电平翻转，常用；
	TIM_OCMODE_PWM1 PWM1模式，向上计数：CNT < CCR时，REF置有效电平，CNT ≥ CCR时，REF置无效电平，常用；
	TIM_OCMODE_PWM2 PWM2模式，向上计数：CNT < CCR时，REF置无效电平，CNT ≥ CCR时，REF置有效电平，常用；
	
	PWM模式将在其他文件中详解，这里主要讲述TIM_OCMODE_TOGGLE模式
	CNT=CCR，电平翻转，若开启中断，则同时进入 HAL_TIM_OC_DelayElapsedCallback() 输出比较回调函数；
	此时如果在中断中重新设置CCR值，则CNT在到达新设置的CCR值前，电平会继续翻转，
	因此通过在中断中设置不同CCR，可实现不同频率/占空比的PWM
	
	由此可见，输出比较模式主要作用为：实现同一个定时器，输出不同频率的PWM，
	诸如：单通道输出时大时小PWM，或多通道分别输出不同频率PWM
	*/
	
	TIM_OCInitStructure.OCMode = TIM_OCMODE_TOGGLE;  	         // 配置为输出比较模式
	
	// PWM CCR值(捕获/比较值) 设置 (CH3)
	TIM_OCInitStructure.Pulse = OC_Pulse;          
	// PWM通道1输出引脚有效电平极性（低电平）
	TIM_OCInitStructure.OCPolarity = TIM_OCPOLARITY_LOW;       
	//初始化通道3输出PWM 
	HAL_TIM_OC_ConfigChannel(&TIM_TimeBaseStructure_OC,&TIM_OCInitStructure,GENERAL_TIM_CHANNELx1);   // GENERAL_TIM_CHANNELx1  TIM_CHANNEL_3
	
	// PWM CCR值(捕获/比较值) 设置 (CH4)
	TIM_OCInitStructure.Pulse = 2*OC_Pulse;   
	// PWM通道2输出引脚有效电平极性（低电平）
	TIM_OCInitStructure.OCPolarity = TIM_OCPOLARITY_LOW;     
	//初始化通道4输出PWM 
	HAL_TIM_OC_ConfigChannel(&TIM_TimeBaseStructure_OC,&TIM_OCInitStructure,GENERAL_TIM_CHANNELx2);   // GENERAL_TIM_CHANNELx2  TIM_CHANNEL_4
	
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
  * @brief  定时器比较中断
  * @param  htim：定时器句柄指针
	*	@note 	无
  * @retval 无
  */
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
  __IO uint16_t count;
  /*获取当前计数*/
  count = __HAL_TIM_GET_COUNTER(htim);

  /*判断触发中断的输出通道并设置新的比较数值*/
	// 这里遇到一个怎么也想不懂的bug
	// TIM_CHANNEL_3理论上对应0x08，但中断返回值为0x04
	// TIM_CHANNEL_4理论上对应0x0C，但中断返回值为0x08
  if(htim->Channel == GENERAL_TIM_CHANNELx1-4)
  {
		if(turn ==1)
		{
			// 通过这种方法变化方向
			__HAL_TIM_SET_COMPARE(htim, GENERAL_TIM_CHANNELx1, count + 4*OC_Pulse);
			turn = 0;
			return;
		}
    __HAL_TIM_SET_COMPARE(htim, GENERAL_TIM_CHANNELx1, count + 2*OC_Pulse);
  }
  if(htim->Channel == GENERAL_TIM_CHANNELx2-4)
  {
    __HAL_TIM_SET_COMPARE(htim, GENERAL_TIM_CHANNELx2, count + 2*OC_Pulse);
  }
}
/*********************************************END OF FILE**********************/

