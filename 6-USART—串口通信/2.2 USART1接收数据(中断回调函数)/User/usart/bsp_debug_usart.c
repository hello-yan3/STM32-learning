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
uint8_t Uart1RX_Data;       // 串口接收的数据
uint8_t Uart1RX_Buffer[MaxSize];     // 接收数据缓冲 
uint8_t BufferSize=0;       // 接收数据数

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
  
  /* 使能串口接收断 */
	/* 只要接收到数据，就会进入中断 */
  __HAL_UART_ENABLE_IT(&UartHandle,UART_IT_RXNE);  
    
  /* 此函数用来打开中断，配置串口中断的！不是用来接收数据！ */
	/* 详见：https://blog.csdn.net/qq_45413245/article/details/104585026 */
	HAL_UART_Receive_IT(&UartHandle,&Uart1RX_Data,1);    // 打开串口中断，此时接收到的数据会自动保存在Uart1RX_Data中
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

  //抢占优先级0，子优先级0
  HAL_NVIC_SetPriority(DEBUG_USART_IRQ ,0,0);	  //  HAL_NVIC_SetPriority(USART1_IRQn ,0,0); 
	//使能USART1中断通道  
  HAL_NVIC_EnableIRQ(DEBUG_USART_IRQ );		      //  HAL_NVIC_EnableIRQ(USART1_IRQn ); 	 	   
}


/******************************中断及中断回调函数***************************************/
void  DEBUG_USART_IRQHandler(void)    // void USART1_IRQHandler(void)
{
	HAL_UART_IRQHandler(&UartHandle);
	// 此函数内部操作位：调用串口接收函数：UART_Receive_IT(huart);接收数据；
	// 之后进入中断回调函数; 
	
	// 此过程与直接读取中断标志位相同
}

// 串口中断函数讲解：https://blog.csdn.net/u010746431/article/details/97283840
// 其他讲解：
// https://blog.csdn.net/qq_45413245/article/details/104585026
// https://blog.csdn.net/qq_23250091/article/details/126257237
/* 进入中断后执行此回调函数 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	// 数据通过HAL_UART_IRQHandler(&UartHandle);中的：UART_Receive_IT(huart);函数接收；
  // 接收到数据保存至 Uart1RX_Data 中，
	if(huart->Instance == DEBUG_USART)
	{
    /* 所有接收到数据保存至缓冲区 */
    Uart1RX_Buffer[ BufferSize ] = Uart1RX_Data;
    BufferSize++;
    
    /* HAL_UART_Transmit(&UartHandle, &Uart1RX_Data, 1, 1000); */
    /* 少量数据可以使用，大量数据会出错 */
    /* HAL_UART_Transmit() 函数执行较为费时，因此在中断长期使用，会导致传输数据出错 */
    
		/* LED绿色表示接收到数据 */
	  LED_GREEN;	
    
		/* 一旦进入回调函数，串口中断就会关闭 */
		/* 为了下一次接收数据考虑，重新开启串口接收中断 */
		HAL_UART_Receive_IT(huart,&Uart1RX_Data,1);    
	}
}


/*******************************C语言重定向**************************************/
void Usart_SendString(uint8_t *str)
{
	unsigned int k=0;
  do 
  {
      HAL_UART_Transmit(&UartHandle,(uint8_t *)(str + k) ,1,1000);
      k++;
  } while(*(str + k)!='\0');
  
}

// 重定向c库函数printf到串口DEBUG_USART，重定向后可使用printf函数
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

/*********************************************END OF FILE**********************/
