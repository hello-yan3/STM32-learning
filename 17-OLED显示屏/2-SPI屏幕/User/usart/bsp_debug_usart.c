/**
  ******************************************************************************
  * @file    bsp_debug_usart.c
  * @author  fire
  * @version V1.0
  * @date    2016-xx-xx
  * @brief   ʹ�ô���1���ض���c��printf������usart�˿ڣ��жϽ���ģʽ
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� STM32 F103 ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */ 
  
#include "./usart/bsp_debug_usart.h"
#include "./led/bsp_led.h" 

UART_HandleTypeDef UartHandle;

 /**
  * @brief  DEBUG_USART GPIO ����,����ģʽ���á�115200 8-N-1
  * @param  ��
  * @retval ��
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
  * @brief UART MSP ��ʼ�� 
  * @param huart: UART handle
  * @retval ��
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
  /* ����Tx����Ϊ���ù���  */
  GPIO_InitStruct.Pin = DEBUG_USART_TX_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed =  GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStruct);
  
  /* ����Rx����Ϊ���ù��� */
  GPIO_InitStruct.Pin = DEBUG_USART_RX_PIN;
  GPIO_InitStruct.Mode=GPIO_MODE_AF_INPUT;	//ģʽҪ����Ϊ��������ģʽ��	
  HAL_GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStruct);  	   
}



/*******************************���ڷ�������**************************************/
/******* ����8λ���� *******/
void Usart_SendByte(uint8_t ch)
{
	/* ����һ���ֽ����ݵ�USART */
	HAL_UART_Transmit(&UartHandle, &ch, 1, 1000);
		
	/* �ȴ��������ݼĴ���Ϊ�� */
	// UART_FLAG_TXE �������ݼĴ���DR�е����ݱ�ȡ�ߣ���־λ��1 (������λ�Ĵ������ܻ�û��������)
//	while(__HAL_UART_GET_FLAG(&UartHandle, UART_FLAG_TXE ) == RESET);	
}

/******* ����16λ���� *******/
void Usart_SendHalfWord(uint16_t ch)
{
	uint8_t temp_h, temp_l;
	
	/* ȡ���߰�λ */
	temp_h = (uint8_t)((ch&0XFF00)>>8);
	/* ȡ���Ͱ�λ */
	temp_l = (uint8_t) (ch&0XFF);
	
	Usart_SendByte(temp_h);
	//	HAL_UART_Transmit(&UartHandle, &temp_h, 1, 1000);
//	while(__HAL_UART_GET_FLAG(&UartHandle, UART_FLAG_TC ) == RESET);	

	Usart_SendByte(temp_l);
	//	HAL_UART_Transmit(&UartHandle, &temp_l, 1, 1000);
//	while(__HAL_UART_GET_FLAG(&UartHandle, UART_FLAG_TC ) == RESET);	
}


/******* ����32λ���� *******/
void Usart_SendWord(uint32_t ch)
{
	uint16_t temp_h, temp_l;
	
	/* ȡ����16λ */
	temp_h = (uint16_t)((ch&0XFFFF0000)>>16);
	/* ȡ����16λ */
	temp_l = (uint16_t) (ch&0X0000FFFF);
	
	Usart_SendHalfWord(temp_h);
	
	Usart_SendHalfWord(temp_l);
}

/******* ����8λ���� *******/
void Usart_SendArray(uint8_t *array, uint16_t num)
{
	HAL_UART_Transmit(&UartHandle, array, num, 1000);	
	
//  uint8_t i;
//		
//	for(i=0; i<num; i++)
//  {
//	    /* ����һ���ֽ����ݵ�USART */
// 			Usart_SendByte(array[i]);	
//	    HAL_UART_Transmit(&UartHandle, (uint8_t *)(array+i),1,1000);	
//  }
	
	/* �ȴ�������� */
	// UART_FLAG_TC ������λ�Ĵ����е�1�ֽ������Ѿ�ͨ��TX��һλһλ���Ƴ�ȥ�󣬸ñ�־λ�ͻᱻ��1
//	while(__HAL_UART_GET_FLAG(&UartHandle, UART_FLAG_TC ) == RESET);	
}

/******* �����ַ��� *******/
void Usart_SendString(char *str)
{
	unsigned int k=0;
  do 
  {
      HAL_UART_Transmit(&UartHandle,(uint8_t *)(str + k) ,1,1000);
      k++;
  } while(*(str + k)!='\0');
	
  /* �ȴ�������� */
	// UART_FLAG_TC ������λ�Ĵ����е�1�ֽ������Ѿ�ͨ��TX��һλһλ���Ƴ�ȥ�󣬸ñ�־λ�ͻᱻ��1
//	while(__HAL_UART_GET_FLAG(&UartHandle, UART_FLAG_TC ) == RESET);	
}


/*******************************C�����ض���**************************************/
// �ض���c�⺯��printf������DEBUG_USART���ض�����ʹ��printf����
// �˺���ֻ֧��ʹ�õ�������
int fputc(int ch, FILE *f)
{
	/* ����һ���ֽ����ݵ�����DEBUG_USART */
	HAL_UART_Transmit(&UartHandle, (uint8_t *)&ch, 1, 1000);	
	return (ch);
}

// �ض���c�⺯��scanf������DEBUG_USART����д����ʹ��scanf��getchar�Ⱥ���
int fgetc(FILE *f)
{		
	int ch;
	HAL_UART_Receive(&UartHandle, (uint8_t *)&ch, 1, 1000);	
	return (ch);
}

// �ض��� sprintf() �������˺���֧�ֶ������
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
