#include "./SysTick/bsp_SysTick.h"

void get_tick_count(unsigned long *ms)
{
  *ms = HAL_GetTick();
}

/*********************************************END OF FILE**********************/
