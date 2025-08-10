#ifndef __SYSTICK_H
#define __SYSTICK_H

/* 本文件定义延时/获得时间函数 */
#include "stm32f1xx.h"

#define mdelay   HAL_Delay

void get_tick_count(unsigned long *ms);

#endif /* __SYSTICK_H */
