/**
  * 系统滴答定时器 寄存器写法
	* 程序是江科大的
  */
  
#include "stm32f1xx.h"
#include "./systick/bsp_SysTick.h"

/**
  * @brief  微秒级延时
  * @param  xus 延时时长，范围：0~233015 (0~1864135)
  * @retval 无
  */
void Delay_us(uint32_t xus)
{
	// SysTick->LOAD：当倒数计数至零时，将被重装载的值
	// SysTick->VAL ：读取时返回当前倒计数的值，写它则使之清零，
	//							  同时还会清除在 SysTick 控制及状态寄存器中的 COUNTFLAG 标志(位段16)
	// SysTick->CTRL：位段16：如果在上次读取本寄存器后， SysTick 已经计到了 0，则该位为 1。
	//                位段 2：时钟源选择位，0=AHB/8(72MHz 8分频)，1=处理器时钟AHB(72MHz 1分频)。 AHB 72MHz
	//                位段 1：1: SysTick 倒数计数到 0 时产生 SysTick 异常请求，0: 数到 0 时无动作。
	//                        也可以通过读取 COUNTFLAG 标志位来确定计数器是否递减到0
	//                位段 0：SysTick 定时器的使能位
	
//	SysTick->LOAD = 72 * xus;				//设置定时器重装值
//	SysTick->VAL = 0x00;				  	//清空当前计数值
//	// 0101B:  定时器使能，定时器到0无动作， 定时器时钟72MHz
//	SysTick->CTRL = (0x01<<0)|(0x00<<1)|(0x01<<2);				//设置时钟源为HCLK，启动定时器
// 计时到0后，CTRL & (1<<16) 位将置1
//	while(!(SysTick->CTRL & (1<<16) ));	//等待计数到0
//	// 0100B:  定时器关闭，定时器到0无动作， 定时器时钟72MHz
//	// 直接使用 &=~ 对寄存器标志位进行清零
//	SysTick->CTRL &= ~(0x01<<0);				//关闭定时器

	SysTick->LOAD = 72/8 * xus;		  //设置定时器重装值
	SysTick->VAL = 0x00;				  	//清空当前计数值
	// 0001B:  定时器使能，定时器到0无动作， 定时器时钟8MHz
	// SysTick->CTRL |= 1 << 0 ;       /* 开始倒数(正点原子) */
	SysTick->CTRL = (0x01<<0)|(0x00<<1)|(0x00<<2);				//设置时钟源为HCLK/8 (8分频)，启动定时器
	// 计时到0后，CTRL & (1<<16) 位将置1
	while(!(SysTick->CTRL & (1<<16) ));	//等待计数到0
	// 0000B:  定时器关闭，定时器到0无动作， 定时器时钟8MHz
	// 直接使用 &=~ 对寄存器标志位进行清零
	SysTick->CTRL &= ~(0x01<<0);				//关闭定时器
}

/**
  * @brief  毫秒级延时
  * @param  xms 延时时长，范围：0~4294967295
  * @retval 无
  */
void Delay_ms(uint32_t xms)
{
	while(xms--)
	{
		Delay_us(1000);
	}
}
 
/**
  * @brief  秒级延时
  * @param  xs 延时时长，范围：0~4294967295
  * @retval 无
  */
void Delay_s(uint32_t xs)
{
	while(xs--)
	{
		Delay_ms(1000);
	}
} 

/*
	这里讲述系统默认 HAL_Delay() 原理：
		
	首先，在 HAL_Init中，调用HAL_InitTick(TICK_INT_PRIORITY)函数：
	执行操作1为：HAL_SYSTICK_Config()，即设置系统定时器的SysTick->LOAD、VAL、CTRL，
	（这里将系统配置为1000Hz(LOAD寄存器)，此外这里系统定时器到达指定时间后进入中断(CTRL)，开启定时器后(CTRL)定时器不关闭;
	所以会一直计数，到达计数时间后进入中断）

	执行操作2为：HAL_NVIC_SetPriority(SysTick_IRQn(中断源), 0x0F(抢占优先级), 0U(子优先级));
	意思就是设置优先级最低；


	然后，SysTick_Handler()中断函数中，调用函数：HAL_IncTick()初始化时钟；
	HAL_IncTick()函数仅执行一个操作：uwTick += uwTickFreq;   uwTickFreq = 1;
	另外介绍 HAL_GetTick()函数，返回 uwTick 值，
		
	在HAL_Delay(time)函数中，HAL_GetTick()获得当前时间，循环，直到HAL_GetTick() + time时刻，退出循环


	补充知识！SysTick_Handler()中断函数优先级最低，任意中断均可以打断 HAL_Delay()！！
	在中断中，不可以使用系统默认 HAL_Delay()！否则会陷入死循环！
		
	因为这里的 HAL_Delay 并不是中断，所以可以中断中使用，不会卡死
*/

/*********************************************END OF FILE**********************/
