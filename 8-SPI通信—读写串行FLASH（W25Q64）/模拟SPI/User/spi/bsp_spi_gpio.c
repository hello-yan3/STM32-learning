/**
  ******************************************************************************
  * @file    bsp_i2c_ee.c
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   用gpio模拟i2c总线, 适用于STM32系列CPU。该模块不包括应用层命令帧，仅包括I2C总线基本操作函数。
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 F103-霸道 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 


#include "bsp_spi_gpio.h" 
#include "stm32f1xx.h"

/*
* 函数名：void SPI_Init(void)
* 输入参数：
* 输出参数：无
* 返回值：无
* 函数作用：初始化 SPI 的四根引脚
*/
void SPI_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	/* 启用外设和GPIO时钟 */
	SPIx_SCK_GPIO_CLK_ENABLE();
	SPIx_MISO_GPIO_CLK_ENABLE();
	SPIx_MOSI_GPIO_CLK_ENABLE();
	W25_CS_GPIO_CLK_ENABLE();
	
	/* GPIO引脚配置 */
	GPIO_InitStruct.Pin = SPIx_SCK_PIN | W25_CS_PIN | SPIx_MOSI_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(SPIx_SCK_GPIO_PORT, &GPIO_InitStruct); // SCK CS MOSI 为推挽输出
	
	GPIO_InitStruct.Pin = SPIx_MISO_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(SPIx_MISO_GPIO_PORT, &GPIO_InitStruct); // MISO 为输入
	
	W25_CS(1);          // CS 初始化为高电平
	if(CPOL==0)         // 时钟极性 CPOL 0 , 空闲状态 SCK 低电平
		SPI_CLK(0); 
	else if(CPOL==1)    // 时钟极性 CPOL 1 , 空闲状态 SCK 高电平
		SPI_CLK(1);   
}

/*
*********************************************************************************************************
*	函 数 名: spi_Delay
*	功能说明: SPI总线位延迟
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void SPI_Delay(void)
{
	uint8_t i;

	/*　
	 	下面的时间是通过逻辑分析仪测试得到的。
    工作条件：CPU主频72MHz ，MDK编译环境，1级优化
  
		循环次数为10时，SCK频率 = 205KHz 
		循环次数为7时，SCK频率 = 347KHz， SCK高电平时间1.5us，SCK低电平时间2.87us 
	 	循环次数为5时，SCK频率 = 421KHz， SCK高电平时间1.25us，SCK低电平时间2.375us 
	*/
	for (i = 0; i < 5; i++);
}

/*
参考讲解：
STM32通信模拟SPI：https://blog.csdn.net/thisway_diy/article/details/117064534
软件模拟SPI接口程序代码（4种模式）：https://blog.csdn.net/zwj695535100/article/details/107303648/
*/

/* 以下程序均为 MSB first */
/* 在SPI_WriteReadByte()中实现了四种SPI时序 */

/*
 * 函数名：void SPI_WriteByte(uint8_t data)
 * 输入参数：data -> 要写的数据
 * 输出参数：无
 * 返回值：无
 * 函数作用：模拟 SPI 写一个字节
 */
void SPI_WriteByte(uint8_t data)
{
	uint8_t i = 0;
	uint8_t temp = 0;
	for(i=0; i<8; i++) 
	{
		temp = ((data&0x80)==0x80)? 1:0;
		data = data<<1;
		SPI_CLK(0); //CPOL=0
		SPI_MOSI(temp);
		SPI_Delay();
		SPI_CLK(1); //CPHA=0
		SPI_Delay(); 
	}
	SPI_CLK(0); 
}

/*
 * 函数名：uint8_t SPI_ReadByte(void)
 * 输入参数：
 * 输出参数：无
 * 返回值：读到的数据
 * 函数作用：模拟 SPI 读一个字节
 */
uint8_t SPI_ReadByte(void) 
{
	uint8_t i = 0;
	uint8_t read_data = 0x00;
	for(i=0; i<8; i++) 
	{
		read_data = read_data << 1;
		SPI_CLK(0);
		SPI_Delay();
		SPI_CLK(1);
		SPI_Delay();
		if(SPI_MISO()==1) 
		{
			read_data = read_data + 1; 
		} 
	}
	SPI_CLK(0);
	return read_data;
}

/*
 * 函数名：uint8_t SPI_WriteReadByte(uint8_t data)
 * 输入参数：data -> 要写的一个字节数据
 * 输出参数：无
 * 返回值：读到的数据
 * 函数作用：模拟 SPI 读写一个字节
 */

#if (CPHA==0)
uint8_t SPI_WriteReadByte(uint8_t data)
{
	uint8_t i = 0;
	uint8_t temp = 0;
	uint8_t read_data = 0x00;
	for(i=0;i<8;i++) 
	{
		/* 数据处理 */
		temp = ((data&0x80)==0x80)? 1:0;
		data = data<<1;
		read_data = read_data<<1;
		
		/* 执行第一步 */
		/* CPHA==0，跳变前先写数据 */
		if(CPOL==0)
		{
			SPI_CLK(0);
			SPI_MOSI(temp);		 // 写数据
		}
		else if(CPOL==1)
		{
			SPI_CLK(1);
			SPI_MOSI(temp);		 // 写数据
		}
		
		SPI_Delay();	
		
		/* 执行第二步 */
		/* CPHA==0，跳变后在读数据 */
		if(CPOL==0)
		{
			SPI_CLK(1);
		  SPI_Delay();
			if(SPI_MISO()==1) 
			{
				read_data = read_data + 1;    // 读数据
			} 
		}
		else if(CPOL==1)
		{
			SPI_CLK(0);
			SPI_Delay();
			if(SPI_MISO()==1) 
			{
				read_data = read_data + 1;    // 读数据  
			} 
		}
	}
	
	/* 恢复至默认状态 */
	if(CPOL==0)
		SPI_CLK(0);
	
	else if(CPOL==1)
		SPI_CLK(1);
	
	return read_data;
}

#elif (CPHA==1)
uint8_t SPI_WriteReadByte(uint8_t data)
{
	uint8_t i = 0;
	uint8_t temp = 0;
	uint8_t read_data = 0x00;

	/* 执行第零步 */
	/* CPHA==1，先电压跳变 */
	if(CPOL==0)
		SPI_CLK(0);
	else if(CPOL==1)
		SPI_CLK(1);
		
	SPI_Delay();	

	for(i=0;i<8;i++) 
	{
		/* 数据处理 */
		temp = ((data&0x80)==0x80)? 1:0;
		data = data<<1;
		read_data = read_data<<1;
	
		/* 执行第一步 */
		/* CPHA==1，第二个跳变前先写数据 */
		if(CPOL==0)
		{
			SPI_CLK(1);
			SPI_MOSI(temp);		 // 写数据
	
		}
		else if(CPOL==1)
		{
			SPI_CLK(0);
			SPI_MOSI(temp);		 // 写数据
		}
		
		SPI_Delay();	
		
		/* 执行第二步 */
		/* CPHA==1，第二个跳变后在读数据 */
		if(CPOL==0)
		{
			SPI_CLK(0);
			SPI_Delay();	
			if(SPI_MISO()==1) 
			{
				read_data = read_data + 1;    // 读数据
			} 
		}
		else if(CPOL==1)
		{
			SPI_CLK(1);
			SPI_Delay();	
			if(SPI_MISO()==1) 
			{
				read_data = read_data + 1;    // 读数据  
			} 
		}
	}
	
	/* 恢复至默认状态 */
	if(CPOL==0)
		SPI_CLK(0);
	
	else if(CPOL==1)
		SPI_CLK(1);
	
	return read_data;
}


#endif


