/**
  ******************************************************************************
  * @file    bsp_pvd.c
  * @author  fire
  * @version V1.0
  * @date    2017-xx-xx
  * @brief   pvd�������
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F103 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */ 
  
#include "./pvd/bsp_pvd.h" 

/**
  * @brief  ����PVD.
  * @param  None
  * @retval None
  */
void PVD_Config(void)
{
	PWR_PVDTypeDef sConfigPVD;
	
	/*ʹ�� PWR ʱ�� */
	__HAL_RCC_PWR_CLK_ENABLE();
	
	/* ���� PVD �ж� */
  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
	/*�ж����ã���ռ���ȼ�0�������ȼ�Ϊ0*/
	HAL_NVIC_SetPriority(PVD_IRQn, 0 ,0);
	HAL_NVIC_EnableIRQ(PVD_IRQn);  

	/* ����PVD����5 (PVD����ѹ����ֵΪ2.6V��VDD��ѹ����2.6Vʱ����PVD�жϣ�
	   �������ݿɲ�ѯ�����ֲ��֪) ���弶������Լ���ʵ��Ӧ��Ҫ������*/
	sConfigPVD.PVDLevel = PWR_PVDLEVEL_5;     // PWR_CR_PLS_2V7 ���͵�ѹ ������2.68 �½���2.58
	/* ��ΪPVD�ж���ʵ����EXTI16���жϣ����ⲿ�жϴ�����Ϊ������/�½��� */
	sConfigPVD.Mode = PWR_PVD_MODE_IT_RISING_FALLING;     // ������/�½����ⲿ�ж϶����Լ��
	HAL_PWR_ConfigPVD(&sConfigPVD);
	/* ʹ��PVD��� */
	HAL_PWR_EnablePVD();
}

/*********************************************END OF FILE**********************/
