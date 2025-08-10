/**
  ******************************************************************************
  * @file    bsp_pvd.c
  * @author  fire
  * @version V1.0
  * @date    2017-xx-xx
  * @brief   pvd监控配置
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F103 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */ 
  
#include "./pvd/bsp_pvd.h" 

/**
  * @brief  配置PVD.
  * @param  None
  * @retval None
  */
void PVD_Config(void)
{
	PWR_PVDTypeDef sConfigPVD;
	
	/*使能 PWR 时钟 */
	__HAL_RCC_PWR_CLK_ENABLE();
	
	/* 配置 PVD 中断 */
  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
	/*中断设置，抢占优先级0，子优先级为0*/
	HAL_NVIC_SetPriority(PVD_IRQn, 0 ,0);
	HAL_NVIC_EnableIRQ(PVD_IRQn);  

	/* 配置PVD级别5 (PVD检测电压的阈值为2.6V，VDD电压低于2.6V时产生PVD中断，
	   具体数据可查询数据手册获知) 具体级别根据自己的实际应用要求配置*/
	sConfigPVD.PVDLevel = PWR_PVDLEVEL_5;     // PWR_CR_PLS_2V7 典型电压 上升沿2.68 下降沿2.58
	/* 因为PVD中断其实就是EXTI16线中断，而外部中断触发分为上升沿/下降沿 */
	sConfigPVD.Mode = PWR_PVD_MODE_IT_RISING_FALLING;     // 上升沿/下降沿外部中断都可以监测
	HAL_PWR_ConfigPVD(&sConfigPVD);
	/* 使能PVD输出 */
	HAL_PWR_EnablePVD();
}

/*********************************************END OF FILE**********************/
