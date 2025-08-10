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
  
#include "./exti/bsp_exti.h"
#include "./led/bsp_led.h"
#include "./usart/bsp_debug_usart.h"

// �жϱ�־λ��EXTI_FLAG = 1����ʾ�����ж�
uint8_t EXTI_FLAG = 0;

 /**
  * @brief  ���� PA0 Ϊ���жϿڣ��������ж����ȼ�
	* @note   IRQĬ�ϸߵ�ƽ���͵�ƽ�����жϣ��������ѡ���½��ؽ����ж�
  * @param  ��
  * @retval ��
  */
void EXTI_Key_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure; 

    /*��������GPIO�ڵ�ʱ��*/
    EXTI_INT_GPIO_CLK_ENABLE();

    /* ѡ�񰴼�1������ */ 
    GPIO_InitStructure.Pin = EXTI_INT_GPIO_PIN;
    /* ��������Ϊ����ģʽ */ 
//    GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING;	    // �����ؼ���ж�
	  GPIO_InitStructure.Mode = GPIO_MODE_IT_FALLING;	    // �½��ؼ���ж�
    /* �������Ų�����Ҳ������ */
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    /* ʹ������Ľṹ���ʼ������ */
    HAL_GPIO_Init(EXTI_INT_GPIO_PORT, &GPIO_InitStructure); 
	
		/* �����ж����ȼ����� */
		HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
    /* ���� EXTI �ж�Դ ��key1 ���š������ж����ȼ�*/
    HAL_NVIC_SetPriority(EXTI_INT_EXTI_IRQ, 0, 0);  // HAL_NVIC_SetPriority(KEY1_INT_EXTI_IRQ, 0, 0);
    /* ʹ���ж� */
    HAL_NVIC_EnableIRQ(EXTI_INT_EXTI_IRQ);          // HAL_NVIC_EnableIRQ(KEY1_INT_EXTI_IRQ);
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)  // �ص�������������λ�ö��弴��
{
    if(GPIO_Pin == EXTI_INT_GPIO_PIN)
        EXTI_FLAG = 1;  
}
	
/*********************************************END OF FILE**********************/
