 /**
  ******************************************************************************
  * @file    bsp_spi_flash.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   spi flash �ײ�Ӧ�ú���bsp 
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��STM32 F103 ������
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./nrf24l01/bsp_spi.h"

SPI_HandleTypeDef SpiHandle;

static __IO uint32_t  SPITimeout = SPIT_LONG_TIMEOUT;   

static uint16_t SPI_TIMEOUT_UserCallback(uint8_t errorCode);


/**
  * @brief  SPI ����ģʽ����
  * @param  ��
  * @retval ��
  */
void SPI_Init(void)
{
  SpiHandle.Instance               = SPIx;  
	
	SpiHandle.Init.Mode              = SPI_MODE_MASTER;            // ����SPI����ģʽ��SPI����������ģʽ
  SpiHandle.Init.Direction         = SPI_DIRECTION_2LINES;       // ����SPI��˫��ͨѶ��SPIΪ˫��˫��ȫ˫��ͨѶ
  SpiHandle.Init.DataSize          = SPI_DATASIZE_8BIT;          // ����SPI�����ݴ�С��ͨѶ����֡����8bit
	SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;    // ����SPI�����ʱ�ӷ�Ƶ���ӣ�ʹ��8��Ƶ��ʱ��Ƶ�� 36M/8 *2 = 9MHz
  SpiHandle.Init.NSS               = SPI_NSS_SOFT;               // ����SPI�����CS���ſ��Ʒ�����ʹ��������п���
  SpiHandle.Init.FirstBit          = SPI_FIRSTBIT_MSB;           // ����SPI�������ݴ��䷽��λ��ʹ��MSB(��λ����)
  SpiHandle.Init.CLKPhase          = SPI_PHASE_1EDGE;            // ����SPI��ʱ����λCPHA��ѡ���������ز�������Ӧģʽ1
  SpiHandle.Init.CLKPolarity       = SPI_POLARITY_LOW;           // ����SPI��ʱ�Ӽ���CPOL��ʱ�ӿ���ʱ�̵͵�ƽ
  SpiHandle.Init.TIMode            = SPI_TIMODE_DISABLE;         // SPI���費����SPI TI mode
  SpiHandle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE; // SPI���裬����CRCУ��ģʽ
  SpiHandle.Init.CRCPolynomial     = 7;                          // SPI���裬CRCУ�����ʽΪ7

  HAL_SPI_Init(&SpiHandle); 
  
  __HAL_SPI_ENABLE(&SpiHandle);     
}

/**
  * @brief SPI MSP Initialization 
  *        This function configures the hardware resources used in this example: 
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration  
  * @param hspi: SPI handle pointer
  * @retval None
  */
/**
   * @brief SPI MSP ��ʼ��
   * �ú������ñ�����ʹ�õ�Ӳ����Դ��
   * - ����ʱ��ʹ��
   * - �����GPIO����
   * @param hspi��SPI���ָ��
   * @retval ��
   */
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
  GPIO_InitTypeDef  GPIO_InitStruct;
  
  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* Enable GPIO TX/RX clock */
  SPIx_SCK_GPIO_CLK_ENABLE();
  SPIx_MISO_GPIO_CLK_ENABLE();
  SPIx_MOSI_GPIO_CLK_ENABLE();
  SPIx_CS_GPIO_CLK_ENABLE();
  /* Enable SPI clock */
  SPIx_CLK_ENABLE(); 
  
  /*##-2- Configure peripheral GPIO ##########################################*/  
  /* SPI SCK GPIO pin configuration  */
  GPIO_InitStruct.Pin       = SPIx_SCK_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;

  
  HAL_GPIO_Init(SPIx_SCK_GPIO_PORT, &GPIO_InitStruct);
    
  /* SPI MISO GPIO pin configuration  */
  GPIO_InitStruct.Pin = SPIx_MISO_PIN;  
  HAL_GPIO_Init(SPIx_MISO_GPIO_PORT, &GPIO_InitStruct);
  
  /* SPI MOSI GPIO pin configuration  */
  GPIO_InitStruct.Pin = SPIx_MOSI_PIN; 
  HAL_GPIO_Init(SPIx_MOSI_GPIO_PORT, &GPIO_InitStruct);   

  GPIO_InitStruct.Pin = FLASH_CS_PIN ;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  HAL_GPIO_Init(FLASH_CS_GPIO_PORT, &GPIO_InitStruct); 
	
	HAL_GPIO_WritePin(FLASH_CS_GPIO_PORT, FLASH_CS_PIN, GPIO_PIN_SET);   // Ĭ�ϸߵ�ƽ
}

/*****************************************************************/
/**
  * @brief  ʹ��SPI����һ���ֽڵ�����
  * @param  byte��Ҫ���͵�����
  * @retval ���ؽ��յ�������
  */
uint8_t SPI_SendByte(uint8_t TxData)
{
  /* ֱ��ʹ�ÿ⺯��������ȡ */
	uint8_t RxData=0;
	// SPI����ṹ�壬�������ݣ��������ݣ��������ݴ�С����������ʱ��
	if( HAL_SPI_TransmitReceive(&SpiHandle, &TxData, &RxData, 1, 10)!=HAL_OK )  
		SPI_TIMEOUT_UserCallback(1);
	return RxData;	
}

/**
  * @brief  ʹ��SPI��ȡһ���ֽڵ�����
  * @param  ��
  * @retval ���ؽ��յ������� 
  */
uint8_t SPI_ReadByte(void)
{
  return (SPI_SendByte(Dummy_Byte));
}

/*****************************************************************/
/**
  * @brief  �ȴ���ʱ�ص�����
  * @param  None.
  * @retval None.
  */
static  uint16_t SPI_TIMEOUT_UserCallback(uint8_t errorCode)
{
  /* �ȴ���ʱ��Ĵ���,���������Ϣ */
  FLASH_ERROR("SPI �ȴ���ʱ!errorCode = %d",errorCode);
  return 0;
}
   
/*********************************************END OF FILE**********************/
