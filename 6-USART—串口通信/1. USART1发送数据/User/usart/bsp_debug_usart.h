#ifndef __DEBUG_USART_H
#define	__DEBUG_USART_H

#include "stm32f1xx.h"
#include <stdio.h>

//���ڲ�����
#define DEBUG_USART_BAUDRATE                    115200

//���Ŷ���
/*******************************************************/
#define DEBUG_USART                             USART1
#define DEBUG_USART_CLK_ENABLE()                __HAL_RCC_USART1_CLK_ENABLE();

#define DEBUG_USART_RX_GPIO_PORT                GPIOA
#define DEBUG_USART_RX_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOA_CLK_ENABLE()
#define DEBUG_USART_RX_PIN                      GPIO_PIN_10

#define DEBUG_USART_TX_GPIO_PORT                GPIOA
#define DEBUG_USART_TX_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOA_CLK_ENABLE()
#define DEBUG_USART_TX_PIN                      GPIO_PIN_9

/************************************************************/

extern UART_HandleTypeDef UartHandle;

void DEBUG_USART_Config(void);

void Usart_SendByte(uint8_t ch);
void Usart_SendHalfWord(uint16_t ch);
void Usart_SendWord(uint32_t ch);
void Usart_SendArray(uint8_t *array, uint16_t num);
void Usart_SendString(char *str);

int fputc(int ch, FILE *f);
int fgetc(FILE *f);
void Serial_Printf(char *format, ...);

#endif /* __USART1_H */
