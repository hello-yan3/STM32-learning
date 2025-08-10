#ifndef ULTRASONIC_H
#define	ULTRASONIC_H

#include "stm32f1xx.h"

#define ultrasonic_Pin                   GPIO_PIN_6
#define ultrasonic_GPIO_Port             GPIOC
#define ultrasonic_GPIO_CLK_ENABLE() 	   __HAL_RCC_GPIOC_CLK_ENABLE()


#define USER_DEBUG    0u              /* ������Ϣ�Ƿ���� */
/* ������Ϣ��� */
#if  USER_DEBUG
#define DEBUG_INFO(fmt)             printf(fmt "\n");            // ���������Ϣ
#define DEBUG_VAR(fmt, num)         printf(fmt " = %d\n", num);  // ���������Ϣ      
/* ������Ϣ����� */
#else             
#define DEBUG_INFO(fmt)              
#define DEBUG_VAR(fmt, num)	
#endif

extern uint8_t  STA;         // 0δ����/1����������/2�����½���
extern uint8_t  User_Period; // ��¼����Ĵ���CCR�������
extern uint16_t CAPTURE_VAL; // ��¼����Ĵ���CCRֵ
extern uint32_t Distance;    // ����������õľ���
extern uint32_t Time;        // �������յ����ݻ��ѵ�ʱ��

void MX_GPIO_Init(void);
void Read_Distane(void);  


#endif /* ULTRASONIC_H */
