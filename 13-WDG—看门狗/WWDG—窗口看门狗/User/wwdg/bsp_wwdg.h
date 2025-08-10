#ifndef __BSP_WWDG_H
#define	__BSP_WWDG_H


#include "stm32f1xx.h"

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
// 预分频器值
#define WWDG_PRESCALER   WWDG_PRESCALER_8
// 上窗口值
#define WWDG_WINDOW      0x50
// 计数器的值
#define WWDG_COUNTER     0x7F


void WWDG_Config(void);

extern WWDG_HandleTypeDef WWDG_Handle;

#endif /* __BSP_WWDG_H */

