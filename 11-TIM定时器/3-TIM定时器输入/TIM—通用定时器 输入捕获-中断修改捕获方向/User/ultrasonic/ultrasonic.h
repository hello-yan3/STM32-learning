#ifndef ULTRASONIC_H
#define	ULTRASONIC_H

#include "stm32f1xx.h"

#define ultrasonic_Pin                   GPIO_PIN_6
#define ultrasonic_GPIO_Port             GPIOC
#define ultrasonic_GPIO_CLK_ENABLE() 	   __HAL_RCC_GPIOC_CLK_ENABLE()


#define USER_DEBUG    0u              /* 调试信息是否输出 */
/* 调试信息输出 */
#if  USER_DEBUG
#define DEBUG_INFO(fmt)             printf(fmt "\n");            // 调试输出信息
#define DEBUG_VAR(fmt, num)         printf(fmt " = %d\n", num);  // 调试输出信息      
/* 调试信息不输出 */
#else             
#define DEBUG_INFO(fmt)              
#define DEBUG_VAR(fmt, num)	
#endif

extern uint8_t  STA;         // 0未捕获/1捕获到上升沿/2捕获到下降沿
extern uint8_t  User_Period; // 记录捕获寄存器CCR溢出次数
extern uint16_t CAPTURE_VAL; // 记录捕获寄存器CCR值
extern uint32_t Distance;    // 超声波所测得的距离
extern uint32_t Time;        // 超声波收到数据花费的时间

void MX_GPIO_Init(void);
void Read_Distane(void);  


#endif /* ULTRASONIC_H */
