#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "stm32f1xx.h"

void SysTick_Init(void);
void Delay_10us(__IO uint32_t nTime);
#define Delay_ms(x) Delay_10us(100*x)	   //单位ms
#define Delay_s(x)  Delay_ms(1000*x)	 //单位ms

#endif /* __SYSTICK_H */
