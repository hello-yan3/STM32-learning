/**
  ******************************************************************************
  * @file    bsp_exti.c
  * @author  fire
  * @version V1.0
  * @date    2016-xx-xx
  * @brief   I/O���ж�Ӧ��bsp
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F103 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./key/bsp_exti.h"
#include "./led/bsp_led.h"


 /**
  * @brief  ���� PA0 Ϊ���жϿڣ��������ж����ȼ�
  * @param  ��
  * @retval ��
  */
void EXTI_Key_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure; 

    /*��������GPIO�ڵ�ʱ��*/
    KEY1_INT_GPIO_CLK_ENABLE();
    KEY2_INT_GPIO_CLK_ENABLE();

    /* ѡ�񰴼�1������ */ 
    GPIO_InitStructure.Pin = KEY1_INT_GPIO_PIN;
    /* ��������Ϊ����ģʽ */ 
    GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING;	    // �����ؼ���ж�
//	  GPIO_InitStructure.Mode = GPIO_MODE_IT_FALLING;	    // �½��ؼ���ж�
    /* �������Ų�����Ҳ������ */
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    /* ʹ������Ľṹ���ʼ������ */
    HAL_GPIO_Init(KEY1_INT_GPIO_PORT, &GPIO_InitStructure); 
    /* ���� EXTI �ж�Դ ��key1 ���š������ж����ȼ�*/
    HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);  // HAL_NVIC_SetPriority(KEY1_INT_EXTI_IRQ, 0, 0);
    /* ʹ���ж� */
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);          // HAL_NVIC_EnableIRQ(KEY1_INT_EXTI_IRQ);

    /* ѡ�񰴼�2������ */ 
    GPIO_InitStructure.Pin = KEY2_INT_GPIO_PIN;  
    /* ����������������ͬ */
    HAL_GPIO_Init(KEY2_INT_GPIO_PORT, &GPIO_InitStructure);    

		/* �����ж����ȼ����� */
		HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
    /* ���� EXTI �ж�Դ ��key1 ���š������ж����ȼ�*/
    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);  // HAL_NVIC_SetPriority(KEY2_INT_EXTI_IRQ, 0, 0);
    /* ʹ���ж� */
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);          // HAL_NVIC_EnableIRQ(KEY2_INT_EXTI_IRQ);
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)  // �ص�������������λ�ö��弴��
{
    if(GPIO_Pin == KEY1_INT_GPIO_PIN)
        LED1_TOGGLE;
		
	else if(GPIO_Pin == KEY2_INT_GPIO_PIN)
        LED2_TOGGLE;
}
	
/*********************************************END OF FILE**********************/
