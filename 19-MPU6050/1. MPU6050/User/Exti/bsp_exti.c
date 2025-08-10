/**
  ******************************************************************************
  * @file    bsp_exti.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   I/O���ж�Ӧ��bsp
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� ָ���� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./exti/bsp_exti.h"
#include "./led/bsp_led.h"
#include "./usart/bsp_debug_usart.h"

extern __IO uint16_t time; // ms ��ʱ���� 
extern uint16_t continue_time;

 /**
  * @brief  ���� PF7 Ϊ���жϿڣ��������ж����ȼ�
  * @param  ��
  * @retval ��
  */
void EXTI_Pxy_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure; 

    /*��������GPIO�ڵ�ʱ��*/
    EXTI_INT_GPIO_CLK_ENABLE();
 	  __HAL_RCC_AFIO_CLK_ENABLE();

    /* ѡ�񰴼�1������ */ 
    GPIO_InitStructure.Pin = EXTI_GPIO_PIN;
    /* ��������Ϊ����ģʽ */ 
    GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING;	    // �����ؼ���ж�
//	  GPIO_InitStructure.Mode = GPIO_MODE_IT_FALLING;	    // �½��ؼ���ж�
    /* �������Ų�����Ҳ������ */
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    /* ʹ������Ľṹ���ʼ������ */
    HAL_GPIO_Init(EXTI_GPIO_PORT, &GPIO_InitStructure); 
	
		/* �����ж����ȼ����� */
		HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
    /* ���� EXTI �ж�Դ ��key1 ���š������ж����ȼ�*/
    HAL_NVIC_SetPriority(EXTI_IRQ, 0, 0);  // HAL_NVIC_SetPriority(KEY1_INT_EXTI_IRQ, 0, 0);
    /* ʹ���ж� */
    HAL_NVIC_EnableIRQ(EXTI_IRQ);          // HAL_NVIC_EnableIRQ(KEY1_INT_EXTI_IRQ);
}

// �жϱ�־λ���������жϴ���>countʱ���жϱ�־λ��1
// ע�⣺�����ж�ʱ������������йأ�
// ��DMP��ʼ���еģ�mpu_set_sample_rate(DEFAULT_MPU_HZ);  
// Ӧ����MPU6050��ʼ���еģ�MPU6050_WriteReg(MPU6050_RA_SMPLRT_DIV , 0x07);	�����޸���仰û��Ч����

// count �������ã����뼸���жϻص����������������
uint8_t EXTI_flag;
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)  // �ص�������������λ�ö��弴��
{	
	static uint8_t count=0;     
	if(GPIO_Pin == EXTI_GPIO_PIN)
	{
		LED2_TOGGLE;
		count++;
		printf("��%d�ν����жϺ�������������ʱ�䣺%d \r\n", count ,time);
		continue_time += time;
		time = 0;
	}
	if(count>=10)  
	{
		EXTI_flag = 1;
		count = 0;
	}
}

/*********************************************END OF FILE**********************/
