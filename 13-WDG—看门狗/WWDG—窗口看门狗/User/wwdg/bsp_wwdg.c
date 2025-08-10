#include "bsp_wwdg.h"  
#include "./led/bsp_led.h"  

WWDG_HandleTypeDef WWDG_Handle;

// WWDG 中断优先级初始化
static void WWDG_NVIC_Config(void)
{
  HAL_NVIC_SetPriority(WWDG_IRQn,0,0);
	HAL_NVIC_EnableIRQ(WWDG_IRQn);
}

/* brief：
 * 窗口看门狗，有上窗口值和下窗口值，
 * 计数器从设定的初始值开始向下递减，必须在上下窗口值中间喂狗
 * 提前喂狗或延后喂狗都会进行复位
 * WWDG 配置函数
 * WWDG_COUNTER  ：递减计时器的初始值， 取值范围为：0x7f~0x40
 * WWDG_WINDOW   ：上窗口值，取值范围为：0x7f~0x40
 * WWDG_PRESCALER：预分频器值，取值可以是
 *      @arg WWDG_Prescaler_1: WWDG counter clock frequency = (PCLK1(36MHZ)/4096)/1
 *      @arg WWDG_Prescaler_2: WWDG counter clock frequency = (PCLK1(36MHZ)/4096)/2
 *      @arg WWDG_Prescaler_4: WWDG counter clock frequency = (PCLK1(36MHZ)/4096)/4
 *      @arg WWDG_Prescaler_8: WWDG counter clock frequency = (PCLK1(36MHZ)/4096)/8
 * 计时时间计算：1 / WWDG counter clock frequency 
 * 一个计数周期由 WWDG_PRESCALER 预分频器 获得
 * 时钟因子：  Period = 1/((PCLK1(36MHZ)/4096)/WWDG_PRESCALER) = 1/((PCLK1(36MHZ)/4096)/8) = 0.91ms
 * 下窗口计时：(WWDG_COUNTER - 0x40) * Period = (0x7F - 0x40) * 0.91ms
 * 上窗口计时：(WWDG_COUNTER - WWDG_WINDOW) * Period = (0x7F - 0x50) * 0.91ms
 */
void WWDG_Config()
{	
	// 开启 WWDG 时钟
	__HAL_RCC_WWDG_CLK_ENABLE();
	
	// 配置WWDG句柄即寄存器基地址
	WWDG_Handle.Instance = WWDG;
	
	// 设置预分频器值
	WWDG_Handle.Init.Prescaler = WWDG_PRESCALER;
	// 设置上窗口值
	WWDG_Handle.Init.Window  = WWDG_WINDOW;	
	// 设置计数器的值
	WWDG_Handle.Init.Counter = WWDG_COUNTER;
	
	// 使能提前唤醒中断
	WWDG_Handle.Init.EWIMode = WWDG_EWI_ENABLE;
	// 初始化WWDG
	HAL_WWDG_Init(&WWDG_Handle);	
	
	// 配置WWDG中断优先级
	WWDG_NVIC_Config();
}

/* 窗口看门狗回调函数 */
void HAL_WWDG_EarlyWakeupCallback(WWDG_HandleTypeDef* hwwdg)
{
	//黄灯亮，点亮LED只是示意性的操作，
	//真正使用的时候，这里应该是做最重要的事情
	LED_YELLOW; 
}


/*********************************************END OF FILE**********************/
