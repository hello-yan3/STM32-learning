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
  
#include "./key/bsp_encoder.h" 
#include "./led/bsp_led.h"

int16_t Encoder_Count;

 /**
  * @brief  ���� PA0 Ϊ���жϿڣ��������ж����ȼ�
  * @param  ��
  * @retval ��
  */
void EXTI_Key_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure; 

    /*��������GPIO�ڵ�ʱ��*/
    ENCODER1_INT_GPIO_CLK_ENABLE();
    ENCODER2_INT_GPIO_CLK_ENABLE(); 

    /* ѡ�񰴼�1������ */ 
    GPIO_InitStructure.Pin = ENCODER1_INT_GPIO_PIN;
    /* ��������Ϊ����ģʽ */ 
    GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING;	    // �����ؼ���ж�
//	  GPIO_InitStructure.Mode = GPIO_MODE_IT_FALLING;	    // �½��ؼ���ж�
    /* �������Ų�����Ҳ������ */
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    /* ʹ������Ľṹ���ʼ������ */
    HAL_GPIO_Init(ENCODER1_INT_GPIO_PORT, &GPIO_InitStructure); 
    /* ���� EXTI �ж�Դ ��key1 ���š������ж����ȼ�*/
		HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
    HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);  // HAL_NVIC_SetPriority(ENCODER1_INT_EXTI_IRQ, 0, 0);
    /* ʹ���ж� */
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);          // HAL_NVIC_EnableIRQ(ENCODER1_INT_EXTI_IRQ);

    /* ѡ�񰴼�2������ */ 
    GPIO_InitStructure.Pin = ENCODER2_INT_GPIO_PIN;  
    /* ����������������ͬ */
    HAL_GPIO_Init(ENCODER2_INT_GPIO_PORT, &GPIO_InitStructure);    

		/* �����ж����ȼ����� */
		HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
    /* ���� EXTI �ж�Դ ��key1 ���š������ж����ȼ�*/
    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 1);  // HAL_NVIC_SetPriority(ENCODER2_INT_EXTI_IRQ, 0, 0);
    /* ʹ���ж� */
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);          // HAL_NVIC_EnableIRQ(ENCODER2_INT_EXTI_IRQ);
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)  // �ص�������������λ�ö��弴��
{
		// ENCODER1(PA0)�����أ���ʱENCODER2(PA1)�͵�ƽ������ת��ENCODER2(PA1)�ߵ�ƽ����ת
    if(GPIO_Pin == ENCODER1_INT_GPIO_PIN)
		{
				if(HAL_GPIO_ReadPin(ENCODER2_INT_GPIO_PORT, ENCODER2_INT_GPIO_PIN) == GPIO_PIN_RESET)
				{
						Encoder_Count ++;
				}
				else if(HAL_GPIO_ReadPin(ENCODER2_INT_GPIO_PORT, ENCODER2_INT_GPIO_PIN) == GPIO_PIN_SET)
				{
						Encoder_Count --;
				}
		}
		// ENCODER2(PA1)�����أ���ʱENCODER1(PA0)�ߵ�ƽ������ת��ENCODER1(PA0)�͵�ƽ����ת
		else if(GPIO_Pin == ENCODER2_INT_GPIO_PIN)
    {	
				if(HAL_GPIO_ReadPin(ENCODER1_INT_GPIO_PORT, ENCODER1_INT_GPIO_PIN) == GPIO_PIN_SET)
				{
						Encoder_Count ++;
				}
				else if(HAL_GPIO_ReadPin(ENCODER1_INT_GPIO_PORT, ENCODER1_INT_GPIO_PIN) == GPIO_PIN_RESET)
				{
						Encoder_Count --;
				}
		}
}
	
/*********************************************END OF FILE**********************/
