/**
  ******************************************************************************
  * @file    bsp_debug_usart.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   485驱动
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 F103-霸道 STM32  开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
#include "./485/bsp_485.h"
#include "./led/bsp_led.h"  

UART_HandleTypeDef Uart2_Handle;

static void Delay(__IO uint32_t nCount)	 //简单的延时函数
{
	for(; nCount != 0; nCount--);
}

/// 配置USART接收中断
static void NVIC_Configuration(void)
{	
	/* 配置抢占优先级的分组 */
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_0);
	/*中断设置，抢占优先级0，子优先级为0*/
	HAL_NVIC_SetPriority(_485_INT_IRQ, 1 ,1);
	HAL_NVIC_EnableIRQ(_485_INT_IRQ);
}
/*
 * 函数名：_485_Config
 * 描述  ：USART GPIO 配置,工作模式配置
 * 输入  ：无
 * 输出  : 无
 * 调用  ：外部调用
 */
void _485_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
  
  _485_USART_RX_GPIO_CLK_ENABLE();
  _485_USART_TX_GPIO_CLK_ENABLE();
  _485_RE_GPIO_CLK_ENABLE();
  
  /* 使能 UART 时钟 */
  _485_USART_CLK_ENABLE();

  /**USART2 GPIO Configuration    
  PD5    ------> USART2_TX
  PD6    ------> USART2_RX 
  */
  /* 配置Tx引脚为复用功能  */
  GPIO_InitStruct.Pin = _485_USART_TX_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(_485_USART_TX_GPIO_PORT, &GPIO_InitStruct);
  
  /* 配置Rx引脚为复用功能 */
  GPIO_InitStruct.Pin = _485_USART_RX_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_INPUT;
  HAL_GPIO_Init(_485_USART_RX_GPIO_PORT, &GPIO_InitStruct); 
  
 /* 485收发控制管脚 */
  GPIO_InitStruct.Pin = _485_RE_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(_485_RE_GPIO_PORT, &GPIO_InitStruct);
  
  /* 配置串485_USART 模式 */
  Uart2_Handle.Instance = _485_USART;
	
  Uart2_Handle.Init.BaudRate = _485_USART_BAUDRATE;
  Uart2_Handle.Init.WordLength = UART_WORDLENGTH_8B;
  Uart2_Handle.Init.StopBits = UART_STOPBITS_1;
  Uart2_Handle.Init.Parity = UART_PARITY_NONE;
  Uart2_Handle.Init.Mode = UART_MODE_TX_RX;
  Uart2_Handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  Uart2_Handle.Init.OverSampling = UART_OVERSAMPLING_16;
  HAL_UART_Init(&Uart2_Handle);

  /*串口1中断初始化 */
  NVIC_Configuration();
  /*配置串口接收中断 */
  __HAL_UART_ENABLE_IT(&Uart2_Handle,UART_IT_RXNE);
  //默认进入接收模式
  HAL_GPIO_WritePin(_485_RE_GPIO_PORT,_485_RE_PIN,GPIO_PIN_RESET);
}

/***************** 发送/接收步骤与普通串口完全相同 **********************/

/***************** 发送一个字符 **********************/
//使用单字节数据发送前要使能发送引脚，发送后要使能接收引脚。
void _485_SendByte(uint8_t ch)
{
	/* 发送一个字节数据到USART1 */
    HAL_UART_Transmit(&Uart2_Handle, (uint8_t*)&ch, 1, 0xFFFF);	
}

/*****************  发送指定长度的字符串 **********************/
void _485_SendStr_length(uint8_t *str, uint16_t strlen)
{
	_485_TX_EN()	;//	使能发送数据	
	HAL_UART_Transmit(&Uart2_Handle, (uint8_t*)str, strlen, 0xFFFF);	
	
	/*加短暂延时，保证485发送数据完毕*/
	Delay(0xFFF);
		
	_485_RX_EN()	;//	使能接收数据
	
//	unsigned int k=0;

//	_485_TX_EN()	;//	使能发送数据	
//    do 
//    {
//        _485_SendByte( *(str + k) );
//        k++;
//    } while(k < strlen);
//		
//	/*加短暂延时，保证485发送数据完毕*/
//	Delay(0xFFF);
//		
//	_485_RX_EN()	;//	使能接收数据
}

/*****************  发送字符串 **********************/
void _485_SendString(uint8_t *str)
{
	unsigned int k=0;
	_485_TX_EN()	;//	使能发送数据
	
	do 
	{
			HAL_UART_Transmit(&Uart2_Handle, (uint8_t *)(str + k), 1, 0xFFFF);	
			k++;
	} while(*(str + k)!='\0');
	
	/*加短暂延时，保证485发送数据完毕*/
	Delay(0xFFF);
		
	_485_RX_EN()	;//	使能接收数据
}



/***************** 中断接收数据 **********************/
#define UART_BUFF_SIZE      1024

__IO			uint16_t uart_p = 0;           // 接收数据个数
uint8_t	  uart_buff[UART_BUFF_SIZE];     // 接收数据缓冲区

// 进入中断函数
// 因为这里没有HAL_UART_Receive_IT(&UartHandle,(uint8_t*)RxBuff,BUFFSIZE); (从中断中接收数据)
// 所以无法使用之前串口接收数据部分
void bsp_485_IRQHandler(void)
{
    if(uart_p<UART_BUFF_SIZE)
    {
        if(__HAL_UART_GET_FLAG( &Uart2_Handle, UART_FLAG_RXNE ) != RESET)
        {
					  // 在中断中使用普通函数接收
            HAL_UART_Receive (&Uart2_Handle, (uint8_t *)(&uart_buff[uart_p]),1 , 1000);
            uart_p++;	
        }
    }
		else
		{
				clean_rebuff();        // 数据太多溢出
		}
	
	// 不是从中断中启动，所以这句话没啥用
	HAL_UART_IRQHandler(&Uart2_Handle);
}

//获取接收到的数据和长度
char *get_rebuff(uint16_t *len) 
{
    *len = uart_p;
    return (char *)&uart_buff;
}

//清空缓冲区
void clean_rebuff(void) 
{
    uint16_t i=UART_BUFF_SIZE+1;
    uart_p = 0;
		while(i)
			uart_buff[--i]=0;
}


