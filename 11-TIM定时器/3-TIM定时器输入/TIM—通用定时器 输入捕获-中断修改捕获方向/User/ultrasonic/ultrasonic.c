/**
  ******************************************************************************
  * @file    ultrasonic.c
  * @author  LJK
  * @version V1.0
  * @date    2023-xx-xx
  * @brief   ������ģ��
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F103 ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
	
#include "./ultrasonic/ultrasonic.h"
#include "./tim/bsp_general_tim_IC.h"
#include "./usart/bsp_debug_usart.h"

// STA�ĵ�һλ/�ڶ�λ��ʾ����״̬��(STA&0x01)!=0 ����һ�������塢(STA&0x02)!=0 ���������أ�
// 0x01=BIN:0001/0x02=BIN:0010, ע�����ﻹ������: (STA&0x02)==1����ΪSTA&0x02==0x02!=1  
uint8_t  STA;         // ����״̬��־λ
uint8_t  User_Period; // ��¼����Ĵ���CCR�������
uint16_t CAPTURE_VAL; // ��¼����Ĵ���CCRֵ
uint32_t Distance;    // ����������õľ���
uint32_t Time;        // �������յ����ݻ��ѵ�ʱ��


/* ��ʼ��PA7���Ӷ�������������һ������ 10us �������ź� */
void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
	ultrasonic_GPIO_CLK_ENABLE();

  GPIO_InitStruct.Pin = ultrasonic_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(ultrasonic_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(ultrasonic_GPIO_Port, ultrasonic_Pin, GPIO_PIN_RESET);
}

/*
*********************************************************************************************************
*	�� �� ��: Delay_us
*	����˵��: �򵥵���ʱ��������ֲ��I2C��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void Delay_us(void)
{
	uint16_t i;

	/*��
	 	�����ʱ����ͨ���߼������ǲ��Եõ��ġ�
    ����������CPU��Ƶ72MHz ��MDK���뻷����1���Ż�

		ѭ������Ϊ10ʱ��SCLƵ�� = 205KHz 
		ѭ������Ϊ7ʱ��SCLƵ�� = 347KHz�� SCL�ߵ�ƽʱ��1.5us��SCL�͵�ƽʱ��2.87us 
	 	ѭ������Ϊ5ʱ��SCLƵ�� = 421KHz�� SCL�ߵ�ƽʱ��1.25us��SCL�͵�ƽʱ��2.375us 
	
		SCLƵ��200K����һ����5us
	*/
	for (i = 0; i < 0x40; i++);
}


/**************************************************************************
Function: Pulse width reading interruption of ultrasonic echo
Input   : none
Output  : none
�������ܣ��������ز������ȡ�жϢڣ������ж�
��ڲ���: �� 
����  ֵ����
**************************************************************************/	 
//��ʱ�������жϣ�����������жϴ���ص�����
//ÿ��CNT=ARRʱ��������ж�
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)   //�����жϣ����������ʱִ��
{
	if(htim==&TIM_TimeBaseStructure_IC)
	{
		//DEBUG_INFO("���붨ʱ������жϻص�����");
		if( (STA&0x01)==0 )    //�����δ�ɹ�����
		{
			if( (STA&0x02)!=0 )         //(STA&0x02)!=0��˵���ɹ����񵽸ߵ�ƽ
			{
				// ֮��ÿ����һ���жϣ�User_Period++ (else ����),
				// ��User_Period����0x3F��˵���ߵ�ƽʱ��̫���ˣ�ǿ�Ʊ�ǳɹ�������һ��(if ����)
				if( User_Period>=0X3F )   //�ߵ�ƽ̫����
				{
					STA|=0x01;                   //ǿ�Ʊ�ǳɹ�������һ��
					CAPTURE_VAL=0XFFFF;          //��Ǿ���Ϊ0xFFFF(��Զ)
				}
				else 
					User_Period++;               //�����жϴ���+1
			}  
			// ���񵽸ߵ�ƽǰʲôҲ����
		}
		// �ɹ������ʲôҲ����
	}
}

/**************************************************************************
Function: Pulse width reading interruption of ultrasonic echo
Input   : none
Output  : none
�������ܣ��������ز������ȡ�жϢ٣������ж�
��ڲ���: �� 
����  ֵ����
**************************************************************************/
//���붨ʱ��3�жϺ��ڶ�ʱ��3�ж����жϳ��ǲ����жϣ�Ȼ�����˻ص�����
//�ɹ��������ݣ�������ж�
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)//�����жϷ���ʱִ��
{
	if(htim==&TIM_TimeBaseStructure_IC)
	{
		DEBUG_INFO("���벶���жϻص�����");
		DEBUG_VAR("STA1", STA); 
		if( (STA&0x01)==0 )   //��δ�ɹ�����
		{
			//��ʼ����,��һ�β���������
			DEBUG_INFO("������");
			if( (STA&0x02)==0 )          
			{
				User_Period=0;      //STA��գ����붨ʱ�������жϴ�����գ�
				CAPTURE_VAL=0;      //VAL����ֵ���
				STA|=0x02;          //��ǲ�����������

				//����timǰһ��Ҫ�ȹر�tim���������Ժ���ʹ��
				__HAL_TIM_DISABLE(&TIM_TimeBaseStructure_IC);        //�رն�ʱ��
				__HAL_TIM_SET_COUNTER(&TIM_TimeBaseStructure_IC,0);  //������CNT��0
				
				TIM_RESET_CAPTUREPOLARITY(&TIM_TimeBaseStructure_IC,GENERAL_TIM_IC_CHANNELx1);   //һ��Ҫ�����ԭ�������ã���
				TIM_SET_CAPTUREPOLARITY(&TIM_TimeBaseStructure_IC,GENERAL_TIM_IC_CHANNELx1,TIM_ICPOLARITY_FALLING);//���ö�ʱ��ͨ������Ϊ�½��ز���
				
				__HAL_TIM_ENABLE(&TIM_TimeBaseStructure_IC);//ʹ�ܶ�ʱ��
			}   
			
			//����һ���½��أ��������   
			else if( (STA&0x02)!=0 )     
			{   
				DEBUG_INFO("�½���");
				STA|=0x01;    //��ǳɹ�����һ�θߵ�ƽ����
				CAPTURE_VAL=HAL_TIM_ReadCapturedValue(&TIM_TimeBaseStructure_IC,GENERAL_TIM_IC_CHANNELx1);//��ȡ��ǰ�Ĳ���ֵ
				
				//����timǰһ��Ҫ�ȹر�tim���������Ժ���ʹ��
				__HAL_TIM_DISABLE(&TIM_TimeBaseStructure_IC);
				
				TIM_RESET_CAPTUREPOLARITY(&TIM_TimeBaseStructure_IC,GENERAL_TIM_IC_CHANNELx1);    //һ��Ҫ�����ԭ�������ã���
				TIM_SET_CAPTUREPOLARITY(&TIM_TimeBaseStructure_IC,GENERAL_TIM_IC_CHANNELx1,TIM_ICPOLARITY_RISING);//���ö�ʱ��ͨ������Ϊ�����ز���
				
				__HAL_TIM_ENABLE(&TIM_TimeBaseStructure_IC);//ʹ�ܶ�ʱ��
			}		
		}
		// �ɹ������ʲôҲ����
	}
}

/**************************************************************************
Function: Ultrasonic receiving echo function
Input   : none
Output  : none
�������ܣ����������ջز�����
��ڲ���: �� 
����  ֵ����
**************************************************************************/	 	
void Read_Distane(void)        
{ 
	STA=0;			        //��������			
  
	/* ����������һ������ 10us �������źţ� */
	HAL_GPIO_WritePin(ultrasonic_GPIO_Port, ultrasonic_Pin, GPIO_PIN_SET);    
	Delay_us();    // ��ʱһ��ʱ��
	HAL_GPIO_WritePin(ultrasonic_GPIO_Port, ultrasonic_Pin, GPIO_PIN_RESET);    

	while( (STA&0x01)==0 );   //�����δ�ɹ����񣬾�һֱ�����ѭ�����濨�ţ�ֱ���ɹ�����

	// �õ���ʱ���ܼ���ֵ = ������� * ���β��񻨷�ʱ�� = ������� / ��λʱ���ڲ������           
	Time = (uint32_t)((float)( User_Period*(GENERAL_TIM_IC_PERIOD+1) + (CAPTURE_VAL+1) ) / ( 72000000/(GENERAL_TIM_IC_PRESCALER+1) ) *1000) ;     // ��λ: ms
	// �����þ��룬��ʽ:s(·��)=v(�ٶ�)*t(ʱ��)
	Distance = (340/2) * Time;    // ��λ: mm
	
	// �������ϸ��
	DEBUG_VAR("User_Period", User_Period); 
	DEBUG_VAR("CAPTURE_VAL", User_Period); 
	DEBUG_VAR("User_Period*(GENERAL_TIM_IC_PERIOD+1) + (CAPTURE_VAL+1)", User_Period*(GENERAL_TIM_IC_PERIOD+1) + (CAPTURE_VAL+1) ); 
	DEBUG_VAR("72000000/(GENERAL_TIM_IC_PRESCALER+1)", 72000000/(GENERAL_TIM_IC_PRESCALER+1)  ); 
	
	printf("�������յ����ݻ��ѵ�ʱ��Ϊ%dms \n",Time);
	printf("����������õľ���Ϊ%dmm \n",Distance);
	printf("\n\n");

//	// ����ԭʼ����
//	Distance=TIM3CH3_CAPTURE_STA&0X3F;   
//	Distance*=GENERAL_TIM_IC_PERIOD;		//���ʱ���ܺ�
//	Distance+=TIM3CH3_CAPTURE_VAL;		  //�õ��ܵĸߵ�ƽʱ��
//	Distance=Distance*170/1000;         //ʱ��*����/2�����أ� һ������0.001ms
	
	STA=0;			        //������һ�β���			
}

