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
	//							  同时还会清除在 SysTick 控制及状态寄存器中的 COUNTFLAG 标志
	// SysTick->CTRL：位段16：如果在上次读取本寄存器后， SysTick 已经计到了 0，则该位为 1。
	//                位段 2：时钟源选择位，0=AHB/8，1=处理器时钟 AHB。 AHB 72MHz
	//                位段 1：1: SysTick 倒数计数到 0 时产生 SysTick 异常请求，0: 数到 0 时无动作。
	//                        也可以通过读取 COUNTFLAG 标志位来确定计数器是否递减到0
	//                位段 0：SysTick 定时器的使能位
	
//	SysTick->LOAD = 72 * xus;				//设置定时器重装值
//	SysTick->VAL = 0x00;				  	//清空当前计数值
//	// 0101B:  定时器使能，定时器到0无动作， 定时器时钟72MHz
//	SysTick->CTRL = (0x01<<0)|(0x00<<1)|(0x01<<2);				//设置时钟源为HCLK，启动定时器
//	while(!(SysTick->CTRL & (1<<16) ));	//等待计数到0
//	// 0100B:  定时器关闭，定时器到0无动作， 定时器时钟72MHz
//	// 直接使用 &=~ 对寄存器标志位进行清零
//	SysTick->CTRL &= ~(0x01<<0);				//关闭定时器

	SysTick->LOAD = 72/8 * xus;		  //设置定时器重装值
	SysTick->VAL = 0x00;				  	//清空当前计数值
	// 0001B:  定时器使能，定时器到0无动作， 定时器时钟8MHz
	SysTick->CTRL = (0x01<<0)|(0x00<<1)|(0x00<<2);				//设置时钟源为HCLK，启动定时器
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

/*********************************************END OF FILE**********************/
