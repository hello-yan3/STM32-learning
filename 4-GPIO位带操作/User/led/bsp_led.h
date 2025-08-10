#ifndef __LED_H
#define	__LED_H

#include "stm32f1xx.h"
#include "./sys/sys.h"

//���Ŷ���
/*******************************************************/
//R ��ɫ��
#define LED1_PIN                  GPIO_PIN_5                
#define LED1_GPIO_PORT            GPIOB                     
#define LED1_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOB_CLK_ENABLE()

//G ��ɫ��
#define LED2_PIN                  GPIO_PIN_0               
#define LED2_GPIO_PORT            GPIOB                      
#define LED2_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOB_CLK_ENABLE()

//B ��ɫ��
#define LED3_PIN                  GPIO_PIN_1              
#define LED3_GPIO_PORT            GPIOB                       
#define LED3_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOB_CLK_ENABLE()

/************************************************************/


///** ����LED������ĺ꣬
//	* LED�͵�ƽ��������ON=0��OFF=1
//	* ��LED�ߵ�ƽ�����Ѻ����ó�ON=1 ��OFF=0 ����
//	*/
//#define ON  GPIO_PIN_RESET
//#define OFF GPIO_PIN_SET

///* ���κ꣬��������������һ��ʹ�� */
//#define LED1(a)	HAL_GPIO_WritePin(LED1_GPIO_PORT, LED1_PIN, a)


//#define LED2(a)	HAL_GPIO_WritePin(LED2_GPIO_PORT, LED2_PIN, a)


//#define LED3(a)	HAL_GPIO_WritePin(LED3_GPIO_PORT, LED3_PIN, a)

// ����ʹ��λ����������LED


///* ֱ�Ӳ����Ĵ����ķ�������IO */
//#define	digitalHi(p,i)			{p->BSRR=i;}			  //����Ϊ�ߵ�ƽ		
//#define digitalLo(p,i)			{p->BSRR=(uint32_t)i << 16;}				//����͵�ƽ
//#define digitalToggle(p,i)		{p->ODR ^=i;}			//�����ת״̬

#define ON  1
#define OFF 0

#define LED1(a)	       PBout(5)=a
#define LED2(a)	       PBout(0)=a
#define LED3(a)	       PBout(1)=a


/* �������IO�ĺ� */
#define LED1_TOGGLE		 PBout(5) = ((~PBout(5))&0x01)    // ODR�Ĵ����ɶ���д��ȡ��PBout(5)ֵ��ȡ����֮�󸳸�ODR�Ĵ���
#define LED1_OFF	  	 LED1(OFF)
#define LED1_ON		  	 LED1(ON)

#define LED2_TOGGLE	 	 PBout(0) = ((~PBout(0))&0x01)    // ODR�Ĵ����ɶ���д��ȡ��PBout(5)ֵ��ȡ����֮�󸳸�ODR�Ĵ���
#define LED2_OFF	   	 LED2(OFF)
#define LED2_ON		  	 LED2(ON)

#define LED3_TOGGLE		 PBout(1) = ((~PBout(1))&0x01)    // ODR�Ĵ����ɶ���д��ȡ��PBout(5)ֵ��ȡ����֮�󸳸�ODR�Ĵ���
#define LED3_OFF	  	 LED3(OFF)
#define LED3_ON		  	 LED3(ON)



/* ������ɫ������߼��÷�ʹ��PWM�ɻ��ȫ����ɫ,��Ч������ */

//��
#define LED_RED  \
					LED1_ON;\
					LED2_OFF;\
					LED3_OFF;

//��
#define LED_GREEN		\
					LED1_OFF;\
					LED2_ON;\
					LED3_OFF;

//��
#define LED_BLUE	\
					LED1_OFF;\
					LED2_OFF;\
					LED3_ON;

					
//��(��+��)					
#define LED_YELLOW	\
					LED1_ON;\
					LED2_ON;\
					LED3_OFF;
//��(��+��)
#define LED_PURPLE	\
					LED1_ON;\
					LED2_OFF;\
					LED3_ON;

//��(��+��)
#define LED_CYAN \
					LED1_OFF;\
					LED2_ON;\
					LED3_ON;
					
//��(��+��+��)
#define LED_WHITE	\
					LED1_ON;\
					LED2_ON;\
					LED3_ON;
					
//��(ȫ���ر�)
#define LED_RGBOFF	\
					LED1_OFF;\
					LED2_OFF;\
					LED3_OFF;
					

void LED_GPIO_Config(void);

#endif /* __LED_H */
