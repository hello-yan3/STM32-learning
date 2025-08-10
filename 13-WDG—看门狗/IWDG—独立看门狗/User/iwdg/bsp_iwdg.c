/**
  ******************************************************************************
  * @file    bsp_iwdg.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   独立看门狗驱动
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F103 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */


#include "./iwdg/bsp_iwdg.h"  

IWDG_HandleTypeDef IWDG_Handle;
/*
 * 设置 IWDG 的超时时间
 * 
 *      IWDG_PRESCALER可以是[4,8,16,32,64,128,256]
 * IWDG_PRESCALER:预分频器值，取值如下：
 *     @arg IWDG_PRESCALER_4: IWDG prescaler set to 4
 *     @arg IWDG_PRESCALER_8: IWDG prescaler set to 8
 *     @arg IWDG_PRESCALER_16: IWDG prescaler set to 16
 *     @arg IWDG_PRESCALER_32: IWDG prescaler set to 32
 *     @arg IWDG_PRESCALER_64: IWDG prescaler set to 64
 *     @arg IWDG_PRESCALER_128: IWDG prescaler set to 128
 *     @arg IWDG_PRESCALER_256: IWDG prescaler set to 256
 *
 *		独立看门狗使用LSI作为时钟。
 *		LSI 的频率一般在 30~60KHZ 之间，根据温度和工作场合会有一定的漂移，我
 *		们一般取 40KHZ，所以独立看门狗的定时时间并一定非常精确，只适用于对时间精度
 *		要求比较低的场合。
 *
 * IWDG_RELOAD:自动重装载计数器值，取值范围为：0-0XFFF
 *
 * Tout = IWDG_PRESCALER/40 * IWDG_RELOAD (s)
 * 单个周期：1/(40k/IWDG_PRESCALER) = 1/(40k/64) = 1.6ms
 * 计数时间：1.6ms * 625  = 1s
 */

void IWDG_Config(void)
{	
	IWDG_Handle.Instance = IWDG;
  IWDG_Handle.Init.Prescaler = IWDG_PRESCALER;
  IWDG_Handle.Init.Reload = IWDG_RELOAD;
  HAL_IWDG_Init(&IWDG_Handle);
}


/*********************************************END OF FILE**********************/
