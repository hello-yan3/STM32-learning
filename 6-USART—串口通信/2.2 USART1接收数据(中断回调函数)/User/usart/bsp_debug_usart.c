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
uint8_t Uart1RX_Data;       // ���ڽ��յ�����
uint8_t Uart1RX_Buffer[MaxSize];     // �������ݻ��� 
uint8_t BufferSize=0;       // ����������

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
  
  /* ʹ�ܴ��ڽ��ն� */
	/* ֻҪ���յ����ݣ��ͻ�����ж� */
  __HAL_UART_ENABLE_IT(&UartHandle,UART_IT_RXNE);  
    
  /* �˺����������жϣ����ô����жϵģ����������������ݣ� */
	/* �����https://blog.csdn.net/qq_45413245/article/details/104585026 */
	HAL_UART_Receive_IT(&UartHandle,&Uart1RX_Data,1);    // �򿪴����жϣ���ʱ���յ������ݻ��Զ�������Uart1RX_Data��
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

  //��ռ���ȼ�0�������ȼ�0
  HAL_NVIC_SetPriority(DEBUG_USART_IRQ ,0,0);	  //  HAL_NVIC_SetPriority(USART1_IRQn ,0,0); 
	//ʹ��USART1�ж�ͨ��  
  HAL_NVIC_EnableIRQ(DEBUG_USART_IRQ );		      //  HAL_NVIC_EnableIRQ(USART1_IRQn ); 	 	   
}


/******************************�жϼ��жϻص�����***************************************/
void  DEBUG_USART_IRQHandler(void)    // void USART1_IRQHandler(void)
{
	HAL_UART_IRQHandler(&UartHandle);
	// �˺����ڲ�����λ�����ô��ڽ��պ�����UART_Receive_IT(huart);�������ݣ�
	// ֮������жϻص�����; 
	
	// �˹�����ֱ�Ӷ�ȡ�жϱ�־λ��ͬ
}

// �����жϺ������⣺https://blog.csdn.net/u010746431/article/details/97283840
// �������⣺
// https://blog.csdn.net/qq_45413245/article/details/104585026
// https://blog.csdn.net/qq_23250091/article/details/126257237
/* �����жϺ�ִ�д˻ص����� */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	// ����ͨ��HAL_UART_IRQHandler(&UartHandle);�еģ�UART_Receive_IT(huart);�������գ�
  // ���յ����ݱ����� Uart1RX_Data �У�
	if(huart->Instance == DEBUG_USART)
	{
    /* ���н��յ����ݱ����������� */
    Uart1RX_Buffer[ BufferSize ] = Uart1RX_Data;
    BufferSize++;
    
    /* HAL_UART_Transmit(&UartHandle, &Uart1RX_Data, 1, 1000); */
    /* �������ݿ���ʹ�ã��������ݻ���� */
    /* HAL_UART_Transmit() ����ִ�н�Ϊ��ʱ��������жϳ���ʹ�ã��ᵼ�´������ݳ��� */
    
		/* LED��ɫ��ʾ���յ����� */
	  LED_GREEN;	
    
		/* һ������ص������������жϾͻ�ر� */
		/* Ϊ����һ�ν������ݿ��ǣ����¿������ڽ����ж� */
		HAL_UART_Receive_IT(huart,&Uart1RX_Data,1);    
	}
}


/*******************************C�����ض���**************************************/
void Usart_SendString(uint8_t *str)
{
	unsigned int k=0;
  do 
  {
      HAL_UART_Transmit(&UartHandle,(uint8_t *)(str + k) ,1,1000);
      k++;
  } while(*(str + k)!='\0');
  
}

// �ض���c�⺯��printf������DEBUG_USART���ض�����ʹ��printf����
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

/*********************************************END OF FILE**********************/
