/**
  ******************************************************************************
  * @file    bsp_debug_usart.c
  * @author  fire
  * @version V1.0
  * @date    2016-xx-xx
  * @brief   使用串口1，重定向c库printf函数到usart端口，中断接收模式
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 STM32 F103 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */ 
  
#include "./usart/bsp_debug_usart.h"
#include "./led/bsp_led.h" 

UART_HandleTypeDef UartHandle;

 /**
  * @brief  DEBUG_USART GPIO 配置,工作模式配置。115200 8-N-1
  * @param  无
  * @retval 无
  */  
void DEBUG_USART_Config(void)
{ 
  
  UartHandle.Instance          = DEBUG_USART;
  
  UartHandle.Init.BaudRate     = DEBUG_USART_BAUDRATE;
  UartHandle.Init.WordLength   = UART_WORDLENGTH_8B;
  UartHandle.Init.StopBits     = UART_STOPBITS_1;
  UartHandle.Init.Parity       = UART_PARITY_NONE;
  UartHandle.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
  UartHandle.Init.Mode         = UART_MODE_TX_RX;
  
  HAL_UART_Init(&UartHandle);
}


/**
  * @brief UART MSP 初始化 
  * @param huart: UART handle
  * @retval 无
  */
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{  
  GPIO_InitTypeDef  GPIO_InitStruct;
  
  DEBUG_USART_CLK_ENABLE();
	
	DEBUG_USART_RX_GPIO_CLK_ENABLE();
  DEBUG_USART_TX_GPIO_CLK_ENABLE();
  
  /**USART1 GPIO Configuration    
  PA9     ------> USART1_TX
  PA10    ------> USART1_RX 
  */
  /* 配置Tx引脚为复用功能  */
  GPIO_InitStruct.Pin = DEBUG_USART_TX_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed =  GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStruct);
  
  /* 配置Rx引脚为复用功能 */
  GPIO_InitStruct.Pin = DEBUG_USART_RX_PIN;
  GPIO_InitStruct.Mode=GPIO_MODE_AF_INPUT;	//模式要设置为复用输入模式！	
  HAL_GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStruct);  	   
}



/*******************************串口发送数据**************************************/
/******* 发送8位数据 *******/
void Usart_SendByte(uint8_t ch)
{
	/* 发送一个字节数据到USART */
	HAL_UART_Transmit(&UartHandle, &ch, 1, 1000);
		
	/* 等待发送数据寄存器为空 */
	// UART_FLAG_TXE 发送数据寄存器DR中的数据被取走，标志位置1 (发送移位寄存器可能还没发送数据)
//	while(__HAL_UART_GET_FLAG(&UartHandle, UART_FLAG_TXE ) == RESET);	
}

/******* 发送16位数据 *******/
void Usart_SendHalfWord(uint16_t ch)
{
	uint8_t temp_h, temp_l;
	
	/* 取出高八位 */
	temp_h = (uint8_t)((ch&0XFF00)>>8);
	/* 取出低八位 */
	temp_l = (uint8_t) (ch&0XFF);
	
	Usart_SendByte(temp_h);
	//	HAL_UART_Transmit(&UartHandle, &temp_h, 1, 1000);
//	while(__HAL_UART_GET_FLAG(&UartHandle, UART_FLAG_TC ) == RESET);	

	Usart_SendByte(temp_l);
	//	HAL_UART_Transmit(&UartHandle, &temp_l, 1, 1000);
//	while(__HAL_UART_GET_FLAG(&UartHandle, UART_FLAG_TC ) == RESET);	
}


/******* 发送32位数据 *******/
void Usart_SendWord(uint32_t ch)
{
	uint16_t temp_h, temp_l;
	
	/* 取出高16位 */
	temp_h = (uint16_t)((ch&0XFFFF0000)>>16);
	/* 取出低16位 */
	temp_l = (uint16_t) (ch&0X0000FFFF);
	
	Usart_SendHalfWord(temp_h);
	
	Usart_SendHalfWord(temp_l);
}

/******* 发送8位数组 *******/
void Usart_SendArray(uint8_t *array, uint16_t num)
{
	HAL_UART_Transmit(&UartHandle, array, num, 1000);	
	
//  uint8_t i;
//		
//	for(i=0; i<num; i++)
//  {
//	    /* 发送一个字节数据到USART */
// 			Usart_SendByte(array[i]);	
//	    HAL_UART_Transmit(&UartHandle, (uint8_t *)(array+i),1,1000);	
//  }
	
	/* 等待发送完成 */
	// UART_FLAG_TC 发送移位寄存器中的1字节数据已经通过TX脚一位一位的移出去后，该标志位就会被置1
//	while(__HAL_UART_GET_FLAG(&UartHandle, UART_FLAG_TC ) == RESET);	
}

/******* 发送字符串 *******/
void Usart_SendString(char *str)
{
	unsigned int k=0;
  do 
  {
      HAL_UART_Transmit(&UartHandle,(uint8_t *)(str + k) ,1,1000);
      k++;
  } while(*(str + k)!='\0');
	
  /* 等待发送完成 */
	// UART_FLAG_TC 发送移位寄存器中的1字节数据已经通过TX脚一位一位的移出去后，该标志位就会被置1
//	while(__HAL_UART_GET_FLAG(&UartHandle, UART_FLAG_TC ) == RESET);	
}


/*******************************C语言重定向**************************************/
// 重定向c库函数printf到串口DEBUG_USART，重定向后可使用printf函数
// 此函数只支持使用单个串口
int fputc(int ch, FILE *f)
{
	/* 发送一个字节数据到串口DEBUG_USART */
	HAL_UART_Transmit(&UartHandle, (uint8_t *)&ch, 1, 1000);	
	return (ch);
}

// 重定向c库函数scanf到串口DEBUG_USART，重写向后可使用scanf、getchar等函数
int fgetc(FILE *f)
{		
	int ch;
	HAL_UART_Receive(&UartHandle, (uint8_t *)&ch, 1, 1000);	
	return (ch);
}

// 重定向 sprintf() 函数，此函数支持多个串口
#include <stdarg.h>
void Serial_Printf(char *format, ...)
{
	char String[100];
	va_list arg;
	va_start(arg, format);
	vsprintf(String, format, arg);
	va_end(arg);
	Usart_SendString(String);
}
/*********************************************END OF FILE**********************/
