 /**
  ******************************************************************************
  * @file    bsp_spi_flash.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   spi flash 底层应用函数bsp 
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火STM32 F103 开发板
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./nrf24l01/bsp_spi.h"

SPI_HandleTypeDef SpiHandle;

static __IO uint32_t  SPITimeout = SPIT_LONG_TIMEOUT;   

static uint16_t SPI_TIMEOUT_UserCallback(uint8_t errorCode);


/**
  * @brief  SPI 工作模式配置
  * @param  无
  * @retval 无
  */
void SPI_Init(void)
{
  SpiHandle.Instance               = SPIx;  
	
	SpiHandle.Init.Mode              = SPI_MODE_MASTER;            // 设置SPI工作模式，SPI工作在主机模式
  SpiHandle.Init.Direction         = SPI_DIRECTION_2LINES;       // 设置SPI单双向通讯，SPI为双线双向全双工通讯
  SpiHandle.Init.DataSize          = SPI_DATASIZE_8BIT;          // 设置SPI的数据大小，通讯数据帧长度8bit
	SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;    // 设置SPI外设的时钟分频因子，使用8分频，时钟频率 36M/8 *2 = 9MHz
  SpiHandle.Init.NSS               = SPI_NSS_SOFT;               // 设置SPI外设的CS引脚控制方法，使用软件进行控制
  SpiHandle.Init.FirstBit          = SPI_FIRSTBIT_MSB;           // 设置SPI外设数据传输方向位，使用MSB(高位先行)
  SpiHandle.Init.CLKPhase          = SPI_PHASE_1EDGE;            // 设置SPI的时钟相位CPHA，选择奇数边沿采样，对应模式1
  SpiHandle.Init.CLKPolarity       = SPI_POLARITY_LOW;           // 设置SPI的时钟极性CPOL，时钟空闲时刻低电平
  SpiHandle.Init.TIMode            = SPI_TIMODE_DISABLE;         // SPI外设不启用SPI TI mode
  SpiHandle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE; // SPI外设，禁用CRC校验模式
  SpiHandle.Init.CRCPolynomial     = 7;                          // SPI外设，CRC校验多项式为7

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
   * @brief SPI MSP 初始化
   * 该函数配置本例中使用的硬件资源：
   * - 外设时钟使能
   * - 外设的GPIO配置
   * @param hspi：SPI句柄指针
   * @retval 无
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
	
	HAL_GPIO_WritePin(FLASH_CS_GPIO_PORT, FLASH_CS_PIN, GPIO_PIN_SET);   // 默认高电平
}

/*****************************************************************/
/**
  * @brief  使用SPI发送一个字节的数据
  * @param  byte：要发送的数据
  * @retval 返回接收到的数据
  */
uint8_t SPI_SendByte(uint8_t TxData)
{
  /* 直接使用库函数方法读取 */
	uint8_t RxData=0;
	// SPI句柄结构体，发送数据，接收数据，发送数据大小，函数持续时间
	if( HAL_SPI_TransmitReceive(&SpiHandle, &TxData, &RxData, 1, 10)!=HAL_OK )  
		SPI_TIMEOUT_UserCallback(1);
	return RxData;	
}

/**
  * @brief  使用SPI读取一个字节的数据
  * @param  无
  * @retval 返回接收到的数据 
  */
uint8_t SPI_ReadByte(void)
{
  return (SPI_SendByte(Dummy_Byte));
}

/*****************************************************************/
/**
  * @brief  等待超时回调函数
  * @param  None.
  * @retval None.
  */
static  uint16_t SPI_TIMEOUT_UserCallback(uint8_t errorCode)
{
  /* 等待超时后的处理,输出错误信息 */
  FLASH_ERROR("SPI 等待超时!errorCode = %d",errorCode);
  return 0;
}
   
/*********************************************END OF FILE**********************/
