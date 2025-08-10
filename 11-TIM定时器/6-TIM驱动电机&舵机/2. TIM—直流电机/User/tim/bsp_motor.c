/**
  ******************************************************************************
  * @file    bsp_motor.c
  * @author  Lacie
  * @version V1.0
  * @date    2023-xx-xx
  * @brief   ������������ӿ�
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� STM32 F103 ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */

#include "./tim/bsp_general_tim.h"
#include "./tim/bsp_motor.h"

/**
  * @brief  ��ʼ�����Ƶ����IO
  * @param  ��
  * @retval ��
  */
void Motor_GPIO_Config(void)
{				
    /*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
    GPIO_InitTypeDef  GPIO_InitStruct;

    /*����LED��ص�GPIO����ʱ��*/
    Motor_GPIO_CLK_ENABLE();
  
    /*ѡ��Ҫ���Ƶ�GPIO����*/															   
    GPIO_InitStruct.Pin = Motor1_PIN|Motor2_PIN;	

    /*�������ŵ��������Ϊ�������*/
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;  

    /*��������Ϊ����ģʽ*/
    GPIO_InitStruct.Pull  = GPIO_PULLUP;

    /*������������Ϊ���� */   
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

    /*���ÿ⺯����ʹ���������õ�GPIO_InitStructure��ʼ��GPIO*/
    HAL_GPIO_Init(Motor_GPIO_PORT, &GPIO_InitStruct);	
    
		HAL_GPIO_WritePin(Motor_GPIO_PORT, Motor1_PIN ,GPIO_PIN_SET);   // ����1Ĭ�ϸߵ�ƽ
		HAL_GPIO_WritePin(Motor_GPIO_PORT, Motor2_PIN ,GPIO_PIN_SET);
}

/**
  * @brief  ��������ٶ�
	* @param  Speed����ٶ�: ����ٶ�
	*         ��Χ: -1000~1000
  * @retval ��
  */
void Motor_SetSpeed(int16_t Speed)
{
	if (Speed >= 0)
	{
		HAL_GPIO_WritePin(Motor_GPIO_PORT, Motor1_PIN ,GPIO_PIN_SET);
		HAL_GPIO_WritePin(Motor_GPIO_PORT, Motor2_PIN ,GPIO_PIN_RESET);
		__HAL_TIM_SET_COMPARE(&TIM_TimeBaseStructure, GENERAL_TIM_CHANNELx1, Speed );  
//		__HAL_TIM_SET_COMPARE(&TIM_TimeBaseStructure, GENERAL_TIM_CHANNELx2, Speed );  
	}
	else
	{
		HAL_GPIO_WritePin(Motor_GPIO_PORT, Motor1_PIN ,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(Motor_GPIO_PORT, Motor2_PIN ,GPIO_PIN_SET);
		__HAL_TIM_SET_COMPARE(&TIM_TimeBaseStructure, GENERAL_TIM_CHANNELx1, -Speed );  
//		__HAL_TIM_SET_COMPARE(&TIM_TimeBaseStructure, GENERAL_TIM_CHANNELx2, Speed );  
	}
}
/*********************************************END OF FILE**********************/
