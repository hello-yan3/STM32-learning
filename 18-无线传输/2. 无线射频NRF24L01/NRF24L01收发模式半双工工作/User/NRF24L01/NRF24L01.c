/**
  ******************************************************************************
  * @file    NRF24L01.c
  * @author  LJK
  * @version V1.0
  * @date    2023
  * @brief   NRF24L01����
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F103 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */

#include "./usart/bsp_debug_usart.h"
#include "./nrf24l01/NRF24L01.h"
#include <math.h>

#define COUNT_OVERFLOW    10000  // �ۼӼ�����ʱ���

#define TX_ADR_WIDTH    5     //5�ֽڵ�ַ���
#define RX_ADR_WIDTH    5     //5�ֽڵ�ַ���
#define TX_PLOAD_WIDTH  32    //32�ֽ���Ч���ݿ��
#define RX_PLOAD_WIDTH  32    //32�ֽ���Ч���ݿ��

const uint8_t TX_ADDRESS[TX_ADR_WIDTH]={0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; 
const uint8_t RX_ADDRESS[RX_ADR_WIDTH]={0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; 

// ����ʹ�ú궨����Ϊ�˳��������
#define W_SS(a)     HAL_GPIO_WritePin(FLASH_CS_GPIO_PORT, FLASH_CS_PIN, (GPIO_PinState)a)
#define W_CE(a)     HAL_GPIO_WritePin(NRF24L01_CE_GPIO_PORT, NRF24L01_CE_GPIO_PIN, (GPIO_PinState)a)         
#define R_IRQ()     HAL_GPIO_ReadPin(EXTI_INT_GPIO_PORT,EXTI_INT_GPIO_PIN)   

#define SPI_SwapByte     SPI_SendByte          // SPI�������ݺ궨��
#define NRF24L01_Get_Value_Flag()  R_IRQ()     // ��ȡ���ŵ�ƽ

/********************************************************/

 /**
  * @brief  CE���ų�ʼ��
  * @param  ��
  * @retval ��
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
	HAL_GPIO_WritePin(NRF24L01_CE_GPIO_PORT, NRF24L01_CE_GPIO_PIN, GPIO_PIN_RESET);   // Ĭ�ϵ͵�ƽ
}

/********************************************************/

/**
  * @brief  дNRF24L01�Ĵ���
  * @param  Reg, �Ĵ�����ַ
  *         Value��Ҫд�������
  * @retval ״ֵ̬
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
  * @brief  ��NRF24L01�Ĵ���
  * @param  Reg, �Ĵ�����ַ
  * @retval Value���Ĵ�������
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
  * @brief  һ��дNRF24L01�Ĵ�������ֽ�
  * @param  Reg, �Ĵ�����ַ
  *         *Buf���Ĵ���д���ֽڵĴ������
  *         Len��Ҫ�������ֽڸ���
  * @retval ״ֵ̬
  */
uint8_t NRF24L01_Write_Buf(uint8_t Reg, uint8_t *Buf, uint8_t Len)
{
	uint8_t Status, i;
	
	W_SS(0); 
	Status = SPI_SwapByte(Reg);		
	for(i = 0; i < Len; i ++)
	{
		// ��Buf++ , Bufָ��������һ�����ݣ�����Buf��ǰλ�ã�֮��*�Ե�ǰλ��ȡֵ
		SPI_SwapByte(*Buf++); 		
		// SPI_SwapByte(*Buf); 		
		// Buf++; 		
	}
	W_SS(1); 
	
	return Status;          		
}				

/**
  * @brief  һ�ζ�NRF24L01�Ĵ�������ֽ�
  * @param  Reg, �Ĵ�����ַ
  *         *Buf���Ĵ����ֽڶ�����洢������
  *         Len��Ҫ�������ֽڸ���
  * @retval ״ֵ̬
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
  * @brief  ����NRF24L01�Ƿ����
  * @note   ԭ��û��
  * @param  
  * @retval ״ֵ̬ 1�������ڣ�0������
  */
static uint8_t NRF24L01_Check(void)
{
	uint8_t check_in_buf[TX_ADR_WIDTH] = {0x11 ,0x22, 0x33, 0x44, 0x55};  	// ����������д����
	uint8_t check_out_buf[TX_ADR_WIDTH] = {0x00};

	// д���͵�ַ�Ĵ���
	NRF24L01_Write_Buf(nRF_WRITE_REG + TX_ADDR, check_in_buf, 5);
	// �����͵�ַ�Ĵ���
	NRF24L01_Read_Buf(nRF_READ_REG + TX_ADDR, check_out_buf, 5);

	for(uint8_t i=0; i<TX_ADR_WIDTH ; i++)
	{
		if(check_in_buf[i] != check_out_buf[i])     return 1;
	}
	return 0;		 //��⵽24L01
}			

/**
  * @brief  NRF24L01���߷�������
  * @param  *Buf��*BufҪ���͵��ֽڴ�ŵ�����
  * @retval ״ֵ̬
  */
uint8_t NRF24L01_SendTxBuf(uint8_t *Buf)
{
	uint8_t State=0;
	uint16_t count=0;
   
	W_CE(0);			// оƬ����ģʽ					
  NRF24L01_Write_Buf(WR_TX_PLOAD, Buf, TX_PLOAD_WIDTH);	  //  дTX��Ч����, 1~32�ֽ�
 	W_CE(1);			// оƬ�շ�ģʽ
	
	while(R_IRQ() == 1)		// �����������,��ƽ���ͣ������ж�		
	{
		count++;
		if(count > COUNT_OVERFLOW)
		{
			printf("�ۼӼ����������δ��⵽������������ź�\n");
			break;
		}
	}							
	State = NRF24L01_Read_Reg(STATUS);	   // ��ȡ״ֵ̬		
	NRF24L01_Write_Reg(nRF_WRITE_REG + STATUS, State); 		 // д1��������жϱ�־λ
	
	if(State&MAX_TX)		// �ﵽ����ʹ����ж�	 						
	{
		NRF24L01_Write_Reg(FLUSH_TX, NOP);		// ��ʱ��Ҫ�ֶ����TX FIFO�Ĵ���
		// ��δ�ﵽ����ʹ�����оƬ�Զ����TX FIFO�Ĵ���
		return MAX_TX; 
	}
	if(State & TX_OK)	 // TX��������ж�
	{
		return TX_OK;
	}
	return NOP;	
}

/**
  * @brief  ����NRF24L01���߽�������
  * @param  *Buf��*Buf���յ��Ķ���ֽ����ݴ�ŵ�����
  * @retval ״ֵ̬ 0���յ����ݣ� 1δ���յ�����
  */
uint8_t NRF24L01_GetRxBuf(uint8_t *Buf)
{
	uint8_t State;
	
	State = NRF24L01_Read_Reg(STATUS);  		   // ��ȡ״ֵ̬				 
	NRF24L01_Write_Reg(nRF_WRITE_REG + STATUS, State);  		 // д1��������жϱ�־λ

	if(State & RX_OK) 		// ������յ�����			
	{                                                       
//		W_CE(1);				// оƬ�շ�ģʽ(�Ʋ⣺�����Ƕ����ݣ����Բ���ҪW_CE(0)�޸�ģʽ����)										
		NRF24L01_Read_Buf(RD_RX_PLOAD, Buf, RX_PLOAD_WIDTH);    // ��RX��Ч����, 1~32�ֽ�
		NRF24L01_Write_Reg(FLUSH_RX, NOP);		  // ���RX FIFO �Ĵ��� 
//		W_CE(1);				// оƬ�շ�ģʽ			

		return 0;   // ���յ�����
	}	   
	return 1;     // δ���յ�����
}

				
/**
  * @brief  NRF24L01�շ����ó�ʼ��
  * @param  ��
  * @retval ��
  */
static void NRF24L01_RT_Init(void) 
{	
	W_CE(0);	

	NRF24L01_Write_Reg(nRF_WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH);    /* ѡ��ͨ��0����Ч���ݿ�� 32bytes */
	NRF24L01_Write_Reg(FLUSH_RX, NOP);					// ���RX FIFO �Ĵ��� 		
  
	// ����NRF24L01 ͨ�ŵĺ�����֪���Է��ĵ�ַ���Զ��壩��
	// ����ģ��ĵ�ַ������ͬ����ͨ�����������ͬ�ĵ�ַͨ��������ȫ��ͬ�ĵ�ַ��
	NRF24L01_Write_Buf(nRF_WRITE_REG + TX_ADDR, (uint8_t*)TX_ADDRESS, TX_ADR_WIDTH);     /* дTX�ڵ��ַ */
	NRF24L01_Write_Buf(nRF_WRITE_REG + RX_ADDR_P0, (uint8_t*)RX_ADDRESS, RX_ADR_WIDTH);  /* ����RX�ڵ��ַ,��ҪΪ��ʹ��ACK */

	NRF24L01_Write_Reg(nRF_WRITE_REG + EN_AA, 0x01);       /* ʹ��ͨ��0���Զ�Ӧ�� */
	NRF24L01_Write_Reg(nRF_WRITE_REG + EN_RXADDR, 0x01);   /* ʹ��ͨ��0�Ľ��յ�ַ */
	NRF24L01_Write_Reg(nRF_WRITE_REG + SETUP_RETR, 0x1A);  // �����Զ��ط����Զ��ط�10�Σ��ȴ�500+86 us
	NRF24L01_Write_Reg(nRF_WRITE_REG + RF_CH, 40);         /* ����RFͨ��Ƶ�� */
	NRF24L01_Write_Reg(nRF_WRITE_REG + RF_SETUP, 0x0F);    // ��Ƶģʽ���� ���໷���� ���书��0dbm �������Ŵ�������

	NRF24L01_Write_Reg(nRF_WRITE_REG + CONFIG, 0x0F);      // ���üĴ�����// �ϵ磬CRCʹ�ܣ�16λCRCУ�飬����ģʽ
	
	W_CE(1);									  
}

/**
  * @brief  NRF24L01��ʼ��
  * @param  ��
  * @retval ��
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
  * @brief  NRF24L01_SendBuf���߷�������
  * @param  *Buf��*BufҪ���͵��ֽڴ�ŵ�����
  * @retval ��
  */
void NRF24L01_SendBuf(uint8_t *Buf)
{
	W_CE(0);									
	NRF24L01_Write_Reg(nRF_WRITE_REG + CONFIG, 0x0E);    // ���뷢��ģʽ
	W_CE(1);
	
	HAL_Delay(1);
	NRF24L01_SendTxBuf(Buf);
	
	W_CE(0);
	NRF24L01_Write_Reg(nRF_WRITE_REG + CONFIG, 0x0F);    // �������ģʽ
	W_CE(1);	
}

/********************************************************************************/
/**
  * @brief  NRF24L01_SendString �����ַ���
  * @param  *String��Ҫ���͵��ַ���
  * @retval ��
  */
void NRF24L01_SendString(char *String)
{
	uint8_t SendBuf[33] = {0};
	uint8_t TSign = 0;
	
	for (uint8_t i = 0; String[i] != 0; i++)
	{
		TSign ++;   // TSign = �ַ�������
	}

	SendBuf[0] = TSign;    // �ܹ����ٸ�����
	
	for (uint8_t i = 0; i < SendBuf[0]; i ++)
	{
		SendBuf[i + 1] = String[i];   
	}

	NRF24L01_SendBuf(SendBuf);  // ��������
}

/**
  * @brief  NRF24L01_SendMunArray �����ַ�����
  * @param  *Num��Ҫ���͵��ַ�����
  * @param  len ���ַ����鳤��
  * @retval ��
  */
void NRF24L01_SendMunArray(uint8_t *Num, uint8_t len)
{
	uint8_t SendBuf[33] = {0};
	SendBuf[0] = len;    // �ܹ����ٸ�����
	
	for (uint8_t i = 0; i < SendBuf[0]; i ++)
	{
		SendBuf[i + 1] = Num[i];   
	}
	
	NRF24L01_SendBuf(SendBuf);  // ��������
}

/**
  * @brief  NRF24L01_SendNum  ����32bit����
  * @param  Num��Ҫ���͵�����
  * @retval ��
  */
void NRF24L01_SendNum(uint32_t Num)   
{
	uint8_t SendBuf[11] = {0};   // ��Ϊ32λ�����ʮλ����
	SendBuf[0] = 10;
	
	for (uint8_t i = 0; i < 10; i ++)							
	{
		SendBuf[i + 1] = Num / (uint32_t)pow(10, 9 - i) % 10;   // �˳��������ȼ���ͬ����������������
	}
	
	NRF24L01_SendBuf(SendBuf);
}

/**
  * @brief  NRF24L01_GetStr  �����ַ���
  * @param  RxBuf�����յ������ݴ�Ż�������33bytes
  * @param  ShowBuf�����������ݴ�Ż�������32bytes
  * @retval Num���ܹ��յ�����λ����
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
  * @brief  NRF24L01_GetNum  ����32bit����
  * @param  ��
  * @retval Num��Ҫ���յ�����
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
