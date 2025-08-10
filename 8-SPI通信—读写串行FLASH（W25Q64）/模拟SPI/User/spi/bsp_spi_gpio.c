/**
  ******************************************************************************
  * @file    bsp_i2c_ee.c
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ��gpioģ��i2c����, ������STM32ϵ��CPU����ģ�鲻����Ӧ�ò�����֡��������I2C���߻�������������
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103-�Ե� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 


#include "bsp_spi_gpio.h" 
#include "stm32f1xx.h"

/*
* ��������void SPI_Init(void)
* ���������
* �����������
* ����ֵ����
* �������ã���ʼ�� SPI ���ĸ�����
*/
void SPI_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	/* ���������GPIOʱ�� */
	SPIx_SCK_GPIO_CLK_ENABLE();
	SPIx_MISO_GPIO_CLK_ENABLE();
	SPIx_MOSI_GPIO_CLK_ENABLE();
	W25_CS_GPIO_CLK_ENABLE();
	
	/* GPIO�������� */
	GPIO_InitStruct.Pin = SPIx_SCK_PIN | W25_CS_PIN | SPIx_MOSI_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(SPIx_SCK_GPIO_PORT, &GPIO_InitStruct); // SCK CS MOSI Ϊ�������
	
	GPIO_InitStruct.Pin = SPIx_MISO_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(SPIx_MISO_GPIO_PORT, &GPIO_InitStruct); // MISO Ϊ����
	
	W25_CS(1);          // CS ��ʼ��Ϊ�ߵ�ƽ
	if(CPOL==0)         // ʱ�Ӽ��� CPOL 0 , ����״̬ SCK �͵�ƽ
		SPI_CLK(0); 
	else if(CPOL==1)    // ʱ�Ӽ��� CPOL 1 , ����״̬ SCK �ߵ�ƽ
		SPI_CLK(1);   
}

/*
*********************************************************************************************************
*	�� �� ��: spi_Delay
*	����˵��: SPI����λ�ӳ�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void SPI_Delay(void)
{
	uint8_t i;

	/*��
	 	�����ʱ����ͨ���߼������ǲ��Եõ��ġ�
    ����������CPU��Ƶ72MHz ��MDK���뻷����1���Ż�
  
		ѭ������Ϊ10ʱ��SCKƵ�� = 205KHz 
		ѭ������Ϊ7ʱ��SCKƵ�� = 347KHz�� SCK�ߵ�ƽʱ��1.5us��SCK�͵�ƽʱ��2.87us 
	 	ѭ������Ϊ5ʱ��SCKƵ�� = 421KHz�� SCK�ߵ�ƽʱ��1.25us��SCK�͵�ƽʱ��2.375us 
	*/
	for (i = 0; i < 5; i++);
}

/*
�ο����⣺
STM32ͨ��ģ��SPI��https://blog.csdn.net/thisway_diy/article/details/117064534
���ģ��SPI�ӿڳ�����루4��ģʽ����https://blog.csdn.net/zwj695535100/article/details/107303648/
*/

/* ���³����Ϊ MSB first */
/* ��SPI_WriteReadByte()��ʵ��������SPIʱ�� */

/*
 * ��������void SPI_WriteByte(uint8_t data)
 * ���������data -> Ҫд������
 * �����������
 * ����ֵ����
 * �������ã�ģ�� SPI дһ���ֽ�
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
 * ��������uint8_t SPI_ReadByte(void)
 * ���������
 * �����������
 * ����ֵ������������
 * �������ã�ģ�� SPI ��һ���ֽ�
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
 * ��������uint8_t SPI_WriteReadByte(uint8_t data)
 * ���������data -> Ҫд��һ���ֽ�����
 * �����������
 * ����ֵ������������
 * �������ã�ģ�� SPI ��дһ���ֽ�
 */

#if (CPHA==0)
uint8_t SPI_WriteReadByte(uint8_t data)
{
	uint8_t i = 0;
	uint8_t temp = 0;
	uint8_t read_data = 0x00;
	for(i=0;i<8;i++) 
	{
		/* ���ݴ��� */
		temp = ((data&0x80)==0x80)? 1:0;
		data = data<<1;
		read_data = read_data<<1;
		
		/* ִ�е�һ�� */
		/* CPHA==0������ǰ��д���� */
		if(CPOL==0)
		{
			SPI_CLK(0);
			SPI_MOSI(temp);		 // д����
		}
		else if(CPOL==1)
		{
			SPI_CLK(1);
			SPI_MOSI(temp);		 // д����
		}
		
		SPI_Delay();	
		
		/* ִ�еڶ��� */
		/* CPHA==0��������ڶ����� */
		if(CPOL==0)
		{
			SPI_CLK(1);
		  SPI_Delay();
			if(SPI_MISO()==1) 
			{
				read_data = read_data + 1;    // ������
			} 
		}
		else if(CPOL==1)
		{
			SPI_CLK(0);
			SPI_Delay();
			if(SPI_MISO()==1) 
			{
				read_data = read_data + 1;    // ������  
			} 
		}
	}
	
	/* �ָ���Ĭ��״̬ */
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

	/* ִ�е��㲽 */
	/* CPHA==1���ȵ�ѹ���� */
	if(CPOL==0)
		SPI_CLK(0);
	else if(CPOL==1)
		SPI_CLK(1);
		
	SPI_Delay();	

	for(i=0;i<8;i++) 
	{
		/* ���ݴ��� */
		temp = ((data&0x80)==0x80)? 1:0;
		data = data<<1;
		read_data = read_data<<1;
	
		/* ִ�е�һ�� */
		/* CPHA==1���ڶ�������ǰ��д���� */
		if(CPOL==0)
		{
			SPI_CLK(1);
			SPI_MOSI(temp);		 // д����
	
		}
		else if(CPOL==1)
		{
			SPI_CLK(0);
			SPI_MOSI(temp);		 // д����
		}
		
		SPI_Delay();	
		
		/* ִ�еڶ��� */
		/* CPHA==1���ڶ���������ڶ����� */
		if(CPOL==0)
		{
			SPI_CLK(0);
			SPI_Delay();	
			if(SPI_MISO()==1) 
			{
				read_data = read_data + 1;    // ������
			} 
		}
		else if(CPOL==1)
		{
			SPI_CLK(1);
			SPI_Delay();	
			if(SPI_MISO()==1) 
			{
				read_data = read_data + 1;    // ������  
			} 
		}
	}
	
	/* �ָ���Ĭ��״̬ */
	if(CPOL==0)
		SPI_CLK(0);
	
	else if(CPOL==1)
		SPI_CLK(1);
	
	return read_data;
}


#endif


