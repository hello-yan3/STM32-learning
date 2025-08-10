/**
  ******************************************************************************
  * @file    bsp_key.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   ����Ӧ��bsp��ɨ��ģʽ��
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F103 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */ 

  

#include ".\key\bsp_key.h" 

/**
  * @brief  ���ð����õ���I/O��
  * @param  ��
  * @retval ��
  */

void Key_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	
    /*��������GPIO�ڵ�ʱ��*/
    KEY1_GPIO_CLK_ENABLE();
    KEY2_GPIO_CLK_ENABLE();
	
    /*ѡ�񰴼�������*/	
    GPIO_InitStructure.Pin = KEY2_PIN; 
    /*��������Ϊ����ģʽ*/
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT; 
    /*�������Ų�����Ҳ������*/
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    /*ʹ������Ľṹ���ʼ������*/
    HAL_GPIO_Init(KEY2_GPIO_PORT, &GPIO_InitStructure);

}



/**
  * @brief   ����Ƿ��а�������
  * @param   ����Ķ˿ںͶ˿�λ
  *		@arg GPIOx: x�����ǣ�A...G�� 
  *		@arg GPIO_PIN ������GPIO_PIN_x��x������1...16��
  * @retval  ������״̬
  *		@arg KEY_ON:��������
  *		@arg KEY_OFF:����û����
  */

uint8_t Key_Scan(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin)
{			
	/*����Ƿ��а������� */
	if(HAL_GPIO_ReadPin(GPIOx,GPIO_Pin) == KEY_ON )  
	{	 
		/*�ȴ������ͷ� */
		while(HAL_GPIO_ReadPin(GPIOx,GPIO_Pin) == KEY_ON);   
		return 	KEY_ON;	 
	}
	else
		return KEY_OFF;
}


/**
  * @brief  ���ڼ�ⰴ���Ƿ񱻳�ʱ�䰴��
  * @param  ��
  * @retval 1 ����������ʱ�䰴��  0 ������û�б���ʱ�䰴��
  */
uint8_t KEY2_LongPress(void)
{			
	uint8_t downCnt =0;	//��¼���µĴ���
	uint8_t upCnt =0;	  //��¼�ɿ��Ĵ���			

	while(1)					//��ѭ������return����
	{	
		HAL_Delay(20);	//�ӳ�һ��ʱ���ټ��

		if(HAL_GPIO_ReadPin( KEY2_GPIO_PORT, KEY2_PIN ) == SET)	//��⵽���°���
		{
			downCnt++;	//��¼���´���
			upCnt=0;  	//��������ͷż�¼

			if(downCnt>=100)	//����ʱ���㹻
			{
				return 1; 		//��⵽������ʱ�䳤����
			}
		}
		else 
		{
			upCnt++; 			//��¼�ͷŴ���
			if(upCnt>5)			//������⵽�ͷų���5��
			{
				return 0;		//����ʱ��̫�̣����ǰ�����������
			}
		}
	}
}

/*********************************************END OF FILE**********************/

