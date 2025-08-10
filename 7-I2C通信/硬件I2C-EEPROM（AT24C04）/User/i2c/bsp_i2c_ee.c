/**
  ******************************************************************************
  * @file    bsp_i2c_ee.c
  * @author  STMicroelectronics
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   i2c EEPROM(AT24C02)应用函数bsp
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F103 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */ 

#include "./i2c/bsp_i2c_ee.h"
#include "./usart/bsp_debug_usart.h"

I2C_HandleTypeDef  I2C_Handle; 


/**
  * @brief  I2C 工作模式配置
  * @param  无
  * @retval 无
  */
static void I2C_Mode_Config(void)
{
  I2C_Handle.Instance             = I2Cx;
  
  I2C_Handle.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;  // 设置I2C寻址模式，I2C寻址模式7bit
  I2C_Handle.Init.ClockSpeed      = I2C_Speed;                // 设置I2C时钟频率，I2C时钟频率 400kHz
	I2C_Handle.Init.DutyCycle       = I2C_DUTYCYCLE_2;          // 设置I2C占空比，I2C 占空比1/2
  I2C_Handle.Init.OwnAddress1     = I2C_OWN_ADDRESS7 ;        // 设置I2C自身地址
  I2C_Handle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;  // 指定是否选择双寻址模式，即具有两个I2C地址，I2C双寻址模式禁用
	I2C_Handle.Init.OwnAddress2     = 0;                        // I2C 第二地址
  I2C_Handle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;  // I2C 广播模式禁用
  I2C_Handle.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;    // I2C 无延伸禁用

  /* Init the I2C */
  HAL_I2C_Init(&I2C_Handle);	

//  HAL_I2CEx_AnalogFilter_Config(&I2C_Handle, ENABLE);    
}

/**
  * @brief I2C MSP Initialization 
  *        This function configures the hardware resources used in this example: 
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration  
  *           - DMA configuration for transmission request by peripheral 
  *           - NVIC configuration for DMA interrupt request enable
  * @param hi2c: I2C handle pointer
  * @retval None
  */
/**
   * @brief I2C MSP 初始化
   * 该函数配置本例中使用的硬件资源：
   * - 外设时钟使能
   * - 外设的GPIO配置
   * - 外设传输请求的 DMA 配置
   * - DMA 中断请求启用的 NVIC 配置
   * @param hi2c: I2C句柄指针
   * @retval 无
   */
void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c)
{  
  GPIO_InitTypeDef  GPIO_InitStruct;
  
  /*##-1- Enable peripherals and GPIO Clocks #################################*/
	/*##-1- 启用外设和 GPIO 时钟##################################*/
  /* Enable GPIO TX/RX clock */
  I2Cx_SCL_GPIO_CLK_ENABLE();
  I2Cx_SDA_GPIO_CLK_ENABLE();
  /* Enable I2C1 clock */
  I2Cx_CLK_ENABLE(); 
  
  /*##-2- Configure peripheral GPIO ##########################################*/ 
  /*##-2- 配置外设 GPIO ########################################## ##*/	
  /* I2C TX GPIO pin configuration  */
  GPIO_InitStruct.Pin       = I2Cx_SCL_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Pull      = GPIO_NOPULL;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
  
  HAL_GPIO_Init(I2Cx_SCL_GPIO_PORT, &GPIO_InitStruct);
    
  /* I2C RX GPIO pin configuration  */
	/* I2C RX GPIO 引脚配置 */
  GPIO_InitStruct.Pin = I2Cx_SDA_PIN;   
  HAL_GPIO_Init(I2Cx_SDA_GPIO_PORT, &GPIO_InitStruct);
  
  /* Force the I2C peripheral clock reset */  
	/* 强制I2C外设时钟复位 */
	I2Cx_FORCE_RESET() ; 

	/* Release the I2C peripheral clock reset */  
	/* 释放I2C外设时钟复位 */
	I2Cx_RELEASE_RESET(); 
}

/**
  * @brief  I2C 外设(EEPROM)初始化
  * @param  无
  * @retval 无
  */
void I2C_EE_Init(void)
{
	I2C_Mode_Config();	
}

/*****************************************************************/

/**
  * @brief   从EEPROM里面读取一块数据 
  * @param   
  *		@arg pBuffer:存放从EEPROM读取的数据的缓冲区指针
  *		@arg ReadAddr:接收数据的EEPROM的地址
  *     @arg NumByteToWrite:要从EEPROM读取的字节数
  * @retval  无
  */
uint8_t I2C_EE_BufferRead(uint8_t* pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead)
{
	HAL_StatusTypeDef status = HAL_OK;
	// I2C句柄结构体，从机地址，从EEPROM何处读数据，地址大小(8位)，读取数据保存在何处，要接收的数据量，函数持续时间
	status=HAL_I2C_Mem_Read(&I2C_Handle, EEPROM_ADDRESS, ReadAddr, I2C_MEMADD_SIZE_8BIT, (uint8_t *)pBuffer, NumByteToRead, 1000);

	return status;
}

/*****************************************************************/

/**
  * @brief   写一个字节到I2C EEPROM中
  * @param   
  *		@arg pBuffer:缓冲区指针
  *		@arg WriteAddr:要写入的地址 
  * @retval  无
  */
uint8_t I2C_EE_ByteWrite(uint8_t* pBuffer, uint8_t WriteAddr)
{
	HAL_StatusTypeDef status = HAL_OK;

	/* I2C寄存器模式写入 */
	// I2C句柄结构体，从机地址，从EEPROM何处写入数据，地址大小(8位)，写入数据保存在何处，要接收的数据量，函数持续时间
	status = HAL_I2C_Mem_Write(&I2C_Handle, EEPROM_ADDRESS, (uint16_t)WriteAddr, I2C_MEMADD_SIZE_8BIT, pBuffer, 1, 100); 

//	/* Check the communication status */
//	if(status != HAL_OK)
//	{
//	/* Execute user timeout callback */
//	//I2Cx_Error(Addr);
//	}
//	/*******************以下内容为检测语句，实测可以整段注释*********************/
//	// 检查I2C标志位，是否可以进行下一次数据传输
//	while (HAL_I2C_GetState(&I2C_Handle) != HAL_I2C_STATE_READY)
//	{
//	}

//	/* Check if the EEPROM is ready for a new operation */
//	/* 检查 EEPROM 是否已准备好进行新操作 */
//	while (HAL_I2C_IsDeviceReady(&I2C_Handle, EEPROM_ADDRESS, EEPROM_MAX_TRIALS, I2Cx_TIMEOUT_MAX) == HAL_TIMEOUT);

//	/* Wait for the end of the transfer */
//	/* 等待传输结束 */
//	while (HAL_I2C_GetState(&I2C_Handle) != HAL_I2C_STATE_READY)
//	{
//	}

	return status;
}


/**
  * @brief   在EEPROM的一个写循环中可以写多个字节，但一次写入的字节数
  *          不能超过EEPROM页的大小，AT24C02每页有8个字节
  * @param   
  *		@arg pBuffer:缓冲区指针
  *		@arg WriteAddr:要写入的地址 
  *     @arg NumByteToWrite:写的字节数
  * @retval  无
  */
uint8_t I2C_EE_PageWrite(uint8_t* pBuffer, uint8_t WriteAddr, uint8_t NumByteToWrite)
{
	HAL_StatusTypeDef status = HAL_OK;
	/* Write EEPROM_PAGESIZE */
	// I2C句柄结构体，从机地址，从EEPROM何处写入数据，地址大小(8位)，写入数据保存在何处，要接收的数据量，函数持续时间
	status=HAL_I2C_Mem_Write(&I2C_Handle, EEPROM_ADDRESS,WriteAddr, I2C_MEMADD_SIZE_8BIT, (uint8_t*)(pBuffer),NumByteToWrite, 100);

//	/*******************以下内容为检测语句，实测可以整段注释*********************/
//	// 检查I2C标志位，是否可以进行下一次数据传输
//	while (HAL_I2C_GetState(&I2C_Handle) != HAL_I2C_STATE_READY)
//	{
//	}

//	/* Check if the EEPROM is ready for a new operation */
//	/* 检查 EEPROM 是否已准备好进行新操作 */
//	while (HAL_I2C_IsDeviceReady(&I2C_Handle, EEPROM_ADDRESS, EEPROM_MAX_TRIALS, I2Cx_TIMEOUT_MAX) == HAL_TIMEOUT);

//	/* Wait for the end of the transfer */
//	/* 等待传输结束 */
//	while (HAL_I2C_GetState(&I2C_Handle) != HAL_I2C_STATE_READY)
//	{
//	}

	return status;
}

/*****************************************************************/

/**
  * @brief   将缓冲区中的数据写到I2C EEPROM中
  * @param   
  *		@arg pBuffer:缓冲区指针 
  *		@arg WriteAddr:写地址
  *     @arg NumByteToWrite:写的字节数
  * @retval  无
  */
void I2C_EE_BufferWrite(uint8_t* pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite)
{
  uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0;

  Addr = WriteAddr % EEPROM_PAGESIZE;
  count = EEPROM_PAGESIZE - Addr;
  NumOfPage =  NumByteToWrite / EEPROM_PAGESIZE;
  NumOfSingle = NumByteToWrite % EEPROM_PAGESIZE;
 
  /* If WriteAddr is I2C_PageSize aligned 如果 写入地址 与 I2C页地址 对齐 */
  if(Addr == 0) 
  {
    /* If NumByteToWrite(写入字节数) < I2C_PageSize(I2C页数大小) */
    if(NumOfPage == 0) 
    {
      I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
    }
    /* If NumByteToWrite(写入字节数) > I2C_PageSize(I2C页数大小) */
    else  
    {
      while(NumOfPage--)
      {
        I2C_EE_PageWrite(pBuffer, WriteAddr, EEPROM_PAGESIZE); 
        WriteAddr +=  EEPROM_PAGESIZE;
        pBuffer += EEPROM_PAGESIZE;
      }

      if(NumOfSingle!=0)
      {
        I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
      }
    }
  }
  /* If WriteAddr is not I2C_PageSize aligned 如果 写入地址 与 I2C页地址 没有对齐 */
  else 
  {
    /* If NumByteToWrite(写入字节数) < I2C_PageSize(I2C页数大小) */
    if(NumOfPage== 0) 
    {
      I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
    }
    /* If NumByteToWrite(写入字节数) > I2C_PageSize(I2C页数大小) */
    else
    {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / EEPROM_PAGESIZE;
      NumOfSingle = NumByteToWrite % EEPROM_PAGESIZE;	
      
      if(count != 0)
      {  
        I2C_EE_PageWrite(pBuffer, WriteAddr, count);
        WriteAddr += count;
        pBuffer += count;
      } 
      
      while(NumOfPage--)
      {
        I2C_EE_PageWrite(pBuffer, WriteAddr, EEPROM_PAGESIZE);
        WriteAddr +=  EEPROM_PAGESIZE;
        pBuffer += EEPROM_PAGESIZE;  
      }
      if(NumOfSingle != 0)
      {
        I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle); 
      }
    }
  }  
}

/*********************************************END OF FILE**********************/
