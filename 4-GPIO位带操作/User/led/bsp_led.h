#ifndef __LED_H
#define	__LED_H

#include "stm32f1xx.h"
#include "./sys/sys.h"

//引脚定义
/*******************************************************/
//R 红色灯
#define LED1_PIN                  GPIO_PIN_5                
#define LED1_GPIO_PORT            GPIOB                     
#define LED1_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOB_CLK_ENABLE()

//G 绿色灯
#define LED2_PIN                  GPIO_PIN_0               
#define LED2_GPIO_PORT            GPIOB                      
#define LED2_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOB_CLK_ENABLE()

//B 蓝色灯
#define LED3_PIN                  GPIO_PIN_1              
#define LED3_GPIO_PORT            GPIOB                       
#define LED3_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOB_CLK_ENABLE()

/************************************************************/


///** 控制LED灯亮灭的宏，
//	* LED低电平亮，设置ON=0，OFF=1
//	* 若LED高电平亮，把宏设置成ON=1 ，OFF=0 即可
//	*/
//#define ON  GPIO_PIN_RESET
//#define OFF GPIO_PIN_SET

///* 带参宏，可以像内联函数一样使用 */
//#define LED1(a)	HAL_GPIO_WritePin(LED1_GPIO_PORT, LED1_PIN, a)


//#define LED2(a)	HAL_GPIO_WritePin(LED2_GPIO_PORT, LED2_PIN, a)


//#define LED3(a)	HAL_GPIO_WritePin(LED3_GPIO_PORT, LED3_PIN, a)

// 这里使用位带操作控制LED


///* 直接操作寄存器的方法控制IO */
//#define	digitalHi(p,i)			{p->BSRR=i;}			  //设置为高电平		
//#define digitalLo(p,i)			{p->BSRR=(uint32_t)i << 16;}				//输出低电平
//#define digitalToggle(p,i)		{p->ODR ^=i;}			//输出反转状态

#define ON  1
#define OFF 0

#define LED1(a)	       PBout(5)=a
#define LED2(a)	       PBout(0)=a
#define LED3(a)	       PBout(1)=a


/* 定义控制IO的宏 */
#define LED1_TOGGLE		 PBout(5) = ((~PBout(5))&0x01)    // ODR寄存器可读可写，取出PBout(5)值，取反，之后赋给ODR寄存器
#define LED1_OFF	  	 LED1(OFF)
#define LED1_ON		  	 LED1(ON)

#define LED2_TOGGLE	 	 PBout(0) = ((~PBout(0))&0x01)    // ODR寄存器可读可写，取出PBout(5)值，取反，之后赋给ODR寄存器
#define LED2_OFF	   	 LED2(OFF)
#define LED2_ON		  	 LED2(ON)

#define LED3_TOGGLE		 PBout(1) = ((~PBout(1))&0x01)    // ODR寄存器可读可写，取出PBout(5)值，取反，之后赋给ODR寄存器
#define LED3_OFF	  	 LED3(OFF)
#define LED3_ON		  	 LED3(ON)



/* 基本混色，后面高级用法使用PWM可混出全彩颜色,且效果更好 */

//红
#define LED_RED  \
					LED1_ON;\
					LED2_OFF;\
					LED3_OFF;

//绿
#define LED_GREEN		\
					LED1_OFF;\
					LED2_ON;\
					LED3_OFF;

//蓝
#define LED_BLUE	\
					LED1_OFF;\
					LED2_OFF;\
					LED3_ON;

					
//黄(红+绿)					
#define LED_YELLOW	\
					LED1_ON;\
					LED2_ON;\
					LED3_OFF;
//紫(红+蓝)
#define LED_PURPLE	\
					LED1_ON;\
					LED2_OFF;\
					LED3_ON;

//青(绿+蓝)
#define LED_CYAN \
					LED1_OFF;\
					LED2_ON;\
					LED3_ON;
					
//白(红+绿+蓝)
#define LED_WHITE	\
					LED1_ON;\
					LED2_ON;\
					LED3_ON;
					
//黑(全部关闭)
#define LED_RGBOFF	\
					LED1_OFF;\
					LED2_OFF;\
					LED3_OFF;
					

void LED_GPIO_Config(void);

#endif /* __LED_H */
