#ifndef __IWDG_H
#define	__IWDG_H

#include "stm32f1xx.h"

// IWDG_Handle.Init.Prescaler 预分频器值
#define IWDG_PRESCALER   IWDG_PRESCALER_64
// IWDG_Handle.Init.Reload 计数器值
#define IWDG_RELOAD      625
// IWDG 的超时时间 = Tout = IWDG_PRESCALER/40 * IWDG_RELOAD (s) = 64 /40 *625 = 1000ms =1s

void IWDG_Config(void);

extern IWDG_HandleTypeDef IWDG_Handle;


#endif /* __IWDG_H */

