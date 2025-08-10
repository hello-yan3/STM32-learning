/**
  ******************************************************************************
  * @file    NRF24L01.c
  * @author  LJK
  * @version V1.0
  * @date    2023
  * @brief   NRF24L01驱动
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F103 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */

#include "./usart/bsp_debug_usart.h"
#include "./nrf24l01/NRF24L01.h"
#include <math.h>

#define COUNT_OVERFLOW    10000  // 累加计数超时溢出

#define TX_ADR_WIDTH    5     //5字节地址宽度
#define RX_ADR_WIDTH    5     //5字节地址宽度
#define TX_PLOAD_WIDTH  32    //32字节有效数据宽度
#define RX_PLOAD_WIDTH  32    //32字节有效数据宽度

const uint8_t TX_ADDRESS[TX_ADR_WIDTH]={0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; 
const uint8_t RX_ADDRESS[RX_ADR_WIDTH]={0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; 

// 这里使用宏定义是为了程序兼容性
#define W_SS(a)     HAL_GPIO_WritePin(FLASH_CS_GPIO_PORT, FLASH_CS_PIN, (GPIO_PinState)a)
#define W_CE(a)     HAL_GPIO_WritePin(NRF24L01_CE_GPIO_PORT, NRF24L01_CE_GPIO_PIN, (GPIO_PinState)a)         
#define R_IRQ()     HAL_GPIO_ReadPin(EXTI_INT_GPIO_PORT,EXTI_INT_GPIO_PIN)   

#define SPI_SwapByte     SPI_SendByte          // SPI发送数据宏定义
#define NRF24L01_Get_Value_Flag()  R_IRQ()     // 获取引脚电平

/********************************************************/

 /**
  * @brief  CE引脚初始化
  * @param  无
  * @retval 无
  */
static void NRF24L01_Pin_Init(void)	
{
	GPIO_InitTypeDef  GPIO_InitStruct;
  
  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  NRF24L01_CE_GPIO_CLK_ENABLE();

  /*##-2- Configure peripheral GPIO ##########################################*/  
  GPIO_InitStruct.Pin       = NRF24L01_CE_GPIO_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
  
  HAL_GPIO_Init(NRF24L01_CE_GPIO_PORT, &GPIO_InitStruct);	
	HAL_GPIO_WritePin(NRF24L01_CE_GPIO_PORT, NRF24L01_CE_GPIO_PIN, GPIO_PIN_RESET);   // 默认低电平
}

/********************************************************/

/**
  * @brief  写NRF24L01寄存器
  * @param  Reg, 寄存器地址
  *         Value：要写入的数据
  * @retval 状态值
  */
uint8_t NRF24L01_Write_Reg(uint8_t Reg, uint8_t Value)
{
	uint8_t Status;

	W_SS(0);                  
  Status = SPI_SwapByte(Reg);
	SPI_SwapByte(Value);	
	W_SS(1);                 

	return Status;
}

/**
  * @brief  读NRF24L01寄存器
  * @param  Reg, 寄存器地址
  * @retval Value，寄存器数据
  */
uint8_t NRF24L01_Read_Reg(uint8_t Reg)
{
 	uint8_t Value;

	W_SS(0);              
  SPI_SwapByte(Reg);		
	Value = SPI_SwapByte(NOP);
	W_SS(1);             	

	return Value;
}

/**
  * @brief  一次写NRF24L01寄存器多个字节
  * @param  Reg, 寄存器地址
  *         *Buf：寄存器写入字节的存放数组
  *         Len：要读出的字节个数
  * @retval 状态值
  */
uint8_t NRF24L01_Write_Buf(uint8_t Reg, uint8_t *Buf, uint8_t Len)
{
	uint8_t Status, i;
	
	W_SS(0); 
	Status = SPI_SwapByte(Reg);		
	for(i = 0; i < Len; i ++)
	{
		// 先Buf++ , Buf指针移至下一个数据，返回Buf当前位置，之后*对当前位置取值
		SPI_SwapByte(*Buf++); 		
		// SPI_SwapByte(*Buf); 		
		// Buf++; 		
	}
	W_SS(1); 
	
	return Status;          		
}				

/**
  * @brief  一次读NRF24L01寄存器多个字节
  * @param  Reg, 寄存器地址
  *         *Buf：寄存器字节读出后存储的数组
  *         Len：要读出的字节个数
  * @retval 状态值
  */
uint8_t NRF24L01_Read_Buf(uint8_t Reg, uint8_t *Buf, uint8_t Len)
{
	uint8_t Status, i;
	
	W_SS(0);                    	
	Status =SPI_SwapByte(Reg);		
 	for(i = 0; i < Len; i ++)
	{
		Buf[i] = SPI_SwapByte(NOP);	
	}
	W_SS(1); 
	
	return Status;        			
}

/********************************************************/

/**
  * @brief  检验NRF24L01是否存在
  * @note   原理没懂
  * @param  
  * @retval 状态值 1：不存在，0：存在
  */
static uint8_t NRF24L01_Check(void)
{
	uint8_t check_in_buf[TX_ADR_WIDTH] = {0x11 ,0x22, 0x33, 0x44, 0x55};  	// 这个内容随便写都行
	uint8_t check_out_buf[TX_ADR_WIDTH] = {0x00};

	// 写发送地址寄存器
	NRF24L01_Write_Buf(nRF_WRITE_REG + TX_ADDR, check_in_buf, 5);
	// 读发送地址寄存器
	NRF24L01_Read_Buf(nRF_READ_REG + TX_ADDR, check_out_buf, 5);

	for(uint8_t i=0; i<TX_ADR_WIDTH ; i++)
	{
		if(check_in_buf[i] != check_out_buf[i])     return 1;
	}
	return 0;		 //检测到24L01
}			

/**
  * @brief  NRF24L01无线发送数据
  * @param  *Buf：*Buf要发送的字节存放的数组
  * @retval 状态值
  */
uint8_t NRF24L01_SendTxBuf(uint8_t *Buf)
{
	uint8_t State=0;
	uint16_t count=0;
   
	W_CE(0);			// 芯片待机模式					
  NRF24L01_Write_Buf(WR_TX_PLOAD, Buf, TX_PLOAD_WIDTH);	  //  写TX有效数据, 1~32字节
 	W_CE(1);			// 芯片收发模式
	
	while(R_IRQ() == 1)		// 发送完成数据,电平拉低，进入中断		
	{
		count++;
		if(count > COUNT_OVERFLOW)
		{
			printf("累加计数器溢出，未检测到发送完成数据信号\n");
			break;
		}
	}							
	State = NRF24L01_Read_Reg(STATUS);	   // 读取状态值		
	NRF24L01_Write_Reg(nRF_WRITE_REG + STATUS, State); 		 // 写1清除接收中断标志位
	
	if(State&MAX_TX)		// 达到最大发送次数中断	 						
	{
		NRF24L01_Write_Reg(FLUSH_TX, NOP);		// 此时需要手动清除TX FIFO寄存器
		// 若未达到最大发送次数，芯片自动清除TX FIFO寄存器
		return MAX_TX; 
	}
	if(State & TX_OK)	 // TX发送完成中断
	{
		return TX_OK;
	}
	return NOP;	
}

/**
  * @brief  读出NRF24L01无线接收数据
  * @param  *Buf：*Buf接收到的多个字节数据存放的数组
  * @retval 状态值 0接收到数据； 1未接收到数据
  */
uint8_t NRF24L01_GetRxBuf(uint8_t *Buf)
{
	uint8_t State;
	
	State = NRF24L01_Read_Reg(STATUS);  		   // 读取状态值				 
	NRF24L01_Write_Reg(nRF_WRITE_REG + STATUS, State);  		 // 写1清除接收中断标志位

	if(State & RX_OK) 		// 如果接收到数据			
	{                                                       
//		W_CE(1);				// 芯片收发模式(推测：这里是读数据，所以不需要W_CE(0)修改模式命令)										
		NRF24L01_Read_Buf(RD_RX_PLOAD, Buf, RX_PLOAD_WIDTH);    // 读RX有效数据, 1~32字节
		NRF24L01_Write_Reg(FLUSH_RX, NOP);		  // 清除RX FIFO 寄存器 
//		W_CE(1);				// 芯片收发模式			

		return 0;   // 接收到数据
	}	   
	return 1;     // 未接收到数据
}

				
/**
  * @brief  NRF24L01收发配置初始化
  * @param  无
  * @retval 无
  */
static void NRF24L01_RT_Init(void) 
{	
	W_CE(0);	

	NRF24L01_Write_Reg(nRF_WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH);    /* 选择通道0的有效数据宽度 32bytes */
	NRF24L01_Write_Reg(FLUSH_RX, NOP);					// 清除RX FIFO 寄存器 		
  
	// 两个NRF24L01 通信的核心是知道对方的地址（自定义），
	// 两个模块的地址可以相同，但通常情况下允许不同的地址通道设置完全相同的地址。
	NRF24L01_Write_Buf(nRF_WRITE_REG + TX_ADDR, (uint8_t*)TX_ADDRESS, TX_ADR_WIDTH);     /* 写TX节点地址 */
	NRF24L01_Write_Buf(nRF_WRITE_REG + RX_ADDR_P0, (uint8_t*)RX_ADDRESS, RX_ADR_WIDTH);  /* 设置RX节点地址,主要为了使能ACK */

	NRF24L01_Write_Reg(nRF_WRITE_REG + EN_AA, 0x01);       /* 使能通道0的自动应答 */
	NRF24L01_Write_Reg(nRF_WRITE_REG + EN_RXADDR, 0x01);   /* 使能通道0的接收地址 */
	NRF24L01_Write_Reg(nRF_WRITE_REG + SETUP_RETR, 0x1A);  // 设置自动重发，自动重发10次，等待500+86 us
	NRF24L01_Write_Reg(nRF_WRITE_REG + RF_CH, 40);         /* 设置RF通信频率 */
	NRF24L01_Write_Reg(nRF_WRITE_REG + RF_SETUP, 0x0F);    // 射频模式配置 锁相环禁用 发射功率0dbm 低噪声放大器开启

	NRF24L01_Write_Reg(nRF_WRITE_REG + CONFIG, 0x0F);      // 配置寄存器：// 上电，CRC使能，16位CRC校验，接收模式
	
	W_CE(1);									  
}

/**
  * @brief  NRF24L01初始化
  * @param  无
  * @retval 无
  */
void NRF24L01_Init(void)
{
	EXTI_Key_Config();
	SPI_Init();
	NRF24L01_Pin_Init();
	while(NRF24L01_Check());
	NRF24L01_RT_Init();
}

/**
  * @brief  NRF24L01_SendBuf无线发送数据
  * @param  *Buf：*Buf要发送的字节存放的数组
  * @retval 无
  */
void NRF24L01_SendBuf(uint8_t *Buf)
{
	W_CE(0);									
	NRF24L01_Write_Reg(nRF_WRITE_REG + CONFIG, 0x0E);    // 进入发射模式
	W_CE(1);
	
	HAL_Delay(1);
	NRF24L01_SendTxBuf(Buf);
	
	W_CE(0);
	NRF24L01_Write_Reg(nRF_WRITE_REG + CONFIG, 0x0F);    // 进入接收模式
	W_CE(1);	
}

/********************************************************************************/
/**
  * @brief  NRF24L01_SendString 发送字符串
  * @param  *String：要发送的字符串
  * @retval 无
  */
void NRF24L01_SendString(char *String)
{
	uint8_t SendBuf[33] = {0};
	uint8_t TSign = 0;
	
	for (uint8_t i = 0; String[i] != 0; i++)
	{
		TSign ++;   // TSign = 字符串个数
	}

	SendBuf[0] = TSign;    // 总共多少个数据
	
	for (uint8_t i = 0; i < SendBuf[0]; i ++)
	{
		SendBuf[i + 1] = String[i];   
	}

	NRF24L01_SendBuf(SendBuf);  // 发送数据
}

/**
  * @brief  NRF24L01_SendMunArray 发送字符数组
  * @param  *Num：要发送的字符数组
  * @param  len ：字符数组长度
  * @retval 无
  */
void NRF24L01_SendMunArray(uint8_t *Num, uint8_t len)
{
	uint8_t SendBuf[33] = {0};
	SendBuf[0] = len;    // 总共多少个数据
	
	for (uint8_t i = 0; i < SendBuf[0]; i ++)
	{
		SendBuf[i + 1] = Num[i];   
	}
	
	NRF24L01_SendBuf(SendBuf);  // 发送数据
}

/**
  * @brief  NRF24L01_SendNum  发送32bit数字
  * @param  Num：要发送的数字
  * @retval 无
  */
void NRF24L01_SendNum(uint32_t Num)   
{
	uint8_t SendBuf[11] = {0};   // 因为32位数最多十位数字
	SendBuf[0] = 10;
	
	for (uint8_t i = 0; i < 10; i ++)							
	{
		SendBuf[i + 1] = Num / (uint32_t)pow(10, 9 - i) % 10;   // 乘除和余优先级相同，从左到右正常运算
	}
	
	NRF24L01_SendBuf(SendBuf);
}

/**
  * @brief  NRF24L01_GetStr  接收字符串
  * @param  RxBuf：接收到的数据存放缓冲区，33bytes
  * @param  ShowBuf：解析后数据存放缓冲区，32bytes
  * @retval Num：总共收到多少位数据
  */
uint8_t NRF24L01_GetStr(char *RxBuf, char *ShowBuf)
{
	uint8_t Num = 0;
	
	NRF24L01_GetRxBuf((uint8_t*)RxBuf); 
	
	for (uint8_t i = 0; i < 32; i ++)
	{
		ShowBuf[i] = RxBuf[i + 1];
	}
	
	Num = RxBuf[0];
	
	return Num;
}

/**
  * @brief  NRF24L01_GetNum  接收32bit数字
  * @param  无
  * @retval Num：要接收的数字
  */
uint32_t NRF24L01_GetNum(void)
{
	uint32_t Num = 0;
	uint8_t Buf[33] = {0};
	
	NRF24L01_GetRxBuf(Buf);
	
	for (uint8_t i = 0; i < 10; i ++)
	{
		Num += Buf[i + 1] * pow(10, 9 - i);
	}
	
	return Num;
}

/********************************************************************************/
