/**
  ******************************************************************************
  * @file    bsp_i2c_ee.c
  * @author  STMicroelectronics
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   i2c EEPROM(AT24C02)Ӧ�ú���bsp
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F103 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */ 

#include "./i2c/bsp_i2c_ee.h"
#include "./usart/bsp_debug_usart.h"

I2C_HandleTypeDef  I2C_Handle; 


/**
  * @brief  I2C ����ģʽ����
  * @param  ��
  * @retval ��
  */
static void I2C_Mode_Config(void)
{
  I2C_Handle.Instance             = I2Cx;
  
  I2C_Handle.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;  // ����I2CѰַģʽ��I2CѰַģʽ7bit
  I2C_Handle.Init.ClockSpeed      = I2C_Speed;                // ����I2Cʱ��Ƶ�ʣ�I2Cʱ��Ƶ�� 400kHz
	I2C_Handle.Init.DutyCycle       = I2C_DUTYCYCLE_2;          // ����I2Cռ�ձȣ�I2C ռ�ձ�1/2
  I2C_Handle.Init.OwnAddress1     = I2C_OWN_ADDRESS7 ;        // ����I2C�����ַ
  I2C_Handle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;  // ָ���Ƿ�ѡ��˫Ѱַģʽ������������I2C��ַ��I2C˫Ѱַģʽ����
	I2C_Handle.Init.OwnAddress2     = 0;                        // I2C �ڶ���ַ
  I2C_Handle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;  // I2C �㲥ģʽ����
  I2C_Handle.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;    // I2C ���������

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
   * @brief I2C MSP ��ʼ��
   * �ú������ñ�����ʹ�õ�Ӳ����Դ��
   * - ����ʱ��ʹ��
   * - �����GPIO����
   * - ���贫������� DMA ����
   * - DMA �ж��������õ� NVIC ����
   * @param hi2c: I2C���ָ��
   * @retval ��
   */
void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c)
{  
  GPIO_InitTypeDef  GPIO_InitStruct;
  
  /*##-1- Enable peripherals and GPIO Clocks #################################*/
	/*##-1- ��������� GPIO ʱ��##################################*/
  /* Enable GPIO TX/RX clock */
  I2Cx_SCL_GPIO_CLK_ENABLE();
  I2Cx_SDA_GPIO_CLK_ENABLE();
  /* Enable I2C1 clock */
  I2Cx_CLK_ENABLE(); 
  
  /*##-2- Configure peripheral GPIO ##########################################*/ 
  /*##-2- �������� GPIO ########################################## ##*/	
  /* I2C TX GPIO pin configuration  */
  GPIO_InitStruct.Pin       = I2Cx_SCL_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Pull      = GPIO_NOPULL;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
  
  HAL_GPIO_Init(I2Cx_SCL_GPIO_PORT, &GPIO_InitStruct);
    
  /* I2C RX GPIO pin configuration  */
	/* I2C RX GPIO �������� */
  GPIO_InitStruct.Pin = I2Cx_SDA_PIN;   
  HAL_GPIO_Init(I2Cx_SDA_GPIO_PORT, &GPIO_InitStruct);
  
  /* Force the I2C peripheral clock reset */  
	/* ǿ��I2C����ʱ�Ӹ�λ */
	I2Cx_FORCE_RESET() ; 

	/* Release the I2C peripheral clock reset */  
	/* �ͷ�I2C����ʱ�Ӹ�λ */
	I2Cx_RELEASE_RESET(); 
}

/**
  * @brief  I2C ����(EEPROM)��ʼ��
  * @param  ��
  * @retval ��
  */
void I2C_EE_Init(void)
{
	I2C_Mode_Config();	
}

/*****************************************************************/

/**
  * @brief   ��EEPROM�����ȡһ������ 
  * @param   
  *		@arg pBuffer:��Ŵ�EEPROM��ȡ�����ݵĻ�����ָ��
  *		@arg ReadAddr:�������ݵ�EEPROM�ĵ�ַ
  *     @arg NumByteToWrite:Ҫ��EEPROM��ȡ���ֽ���
  * @retval  ��
  */
uint8_t I2C_EE_BufferRead(uint8_t* pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead)
{
	HAL_StatusTypeDef status = HAL_OK;
	// I2C����ṹ�壬�ӻ���ַ����EEPROM�δ������ݣ���ַ��С(8λ)����ȡ���ݱ����ںδ���Ҫ���յ�����������������ʱ��
	status=HAL_I2C_Mem_Read(&I2C_Handle, EEPROM_ADDRESS, ReadAddr, I2C_MEMADD_SIZE_8BIT, (uint8_t *)pBuffer, NumByteToRead, 1000);

	return status;
}

/*****************************************************************/

/**
  * @brief   дһ���ֽڵ�I2C EEPROM��
  * @param   
  *		@arg pBuffer:������ָ��
  *		@arg WriteAddr:Ҫд��ĵ�ַ 
  * @retval  ��
  */
uint8_t I2C_EE_ByteWrite(uint8_t* pBuffer, uint8_t WriteAddr)
{
	HAL_StatusTypeDef status = HAL_OK;

	/* I2C�Ĵ���ģʽд�� */
	// I2C����ṹ�壬�ӻ���ַ����EEPROM�δ�д�����ݣ���ַ��С(8λ)��д�����ݱ����ںδ���Ҫ���յ�����������������ʱ��
	status = HAL_I2C_Mem_Write(&I2C_Handle, EEPROM_ADDRESS, (uint16_t)WriteAddr, I2C_MEMADD_SIZE_8BIT, pBuffer, 1, 100); 

//	/* Check the communication status */
//	if(status != HAL_OK)
//	{
//	/* Execute user timeout callback */
//	//I2Cx_Error(Addr);
//	}
//	/*******************��������Ϊ�����䣬ʵ���������ע��*********************/
//	// ���I2C��־λ���Ƿ���Խ�����һ�����ݴ���
//	while (HAL_I2C_GetState(&I2C_Handle) != HAL_I2C_STATE_READY)
//	{
//	}

//	/* Check if the EEPROM is ready for a new operation */
//	/* ��� EEPROM �Ƿ���׼���ý����²��� */
//	while (HAL_I2C_IsDeviceReady(&I2C_Handle, EEPROM_ADDRESS, EEPROM_MAX_TRIALS, I2Cx_TIMEOUT_MAX) == HAL_TIMEOUT);

//	/* Wait for the end of the transfer */
//	/* �ȴ�������� */
//	while (HAL_I2C_GetState(&I2C_Handle) != HAL_I2C_STATE_READY)
//	{
//	}

	return status;
}


/**
  * @brief   ��EEPROM��һ��дѭ���п���д����ֽڣ���һ��д����ֽ���
  *          ���ܳ���EEPROMҳ�Ĵ�С��AT24C02ÿҳ��8���ֽ�
  * @param   
  *		@arg pBuffer:������ָ��
  *		@arg WriteAddr:Ҫд��ĵ�ַ 
  *     @arg NumByteToWrite:д���ֽ���
  * @retval  ��
  */
uint8_t I2C_EE_PageWrite(uint8_t* pBuffer, uint8_t WriteAddr, uint8_t NumByteToWrite)
{
	HAL_StatusTypeDef status = HAL_OK;
	/* Write EEPROM_PAGESIZE */
	// I2C����ṹ�壬�ӻ���ַ����EEPROM�δ�д�����ݣ���ַ��С(8λ)��д�����ݱ����ںδ���Ҫ���յ�����������������ʱ��
	status=HAL_I2C_Mem_Write(&I2C_Handle, EEPROM_ADDRESS,WriteAddr, I2C_MEMADD_SIZE_8BIT, (uint8_t*)(pBuffer),NumByteToWrite, 100);

//	/*******************��������Ϊ�����䣬ʵ���������ע��*********************/
//	// ���I2C��־λ���Ƿ���Խ�����һ�����ݴ���
//	while (HAL_I2C_GetState(&I2C_Handle) != HAL_I2C_STATE_READY)
//	{
//	}

//	/* Check if the EEPROM is ready for a new operation */
//	/* ��� EEPROM �Ƿ���׼���ý����²��� */
//	while (HAL_I2C_IsDeviceReady(&I2C_Handle, EEPROM_ADDRESS, EEPROM_MAX_TRIALS, I2Cx_TIMEOUT_MAX) == HAL_TIMEOUT);

//	/* Wait for the end of the transfer */
//	/* �ȴ�������� */
//	while (HAL_I2C_GetState(&I2C_Handle) != HAL_I2C_STATE_READY)
//	{
//	}

	return status;
}

/*****************************************************************/

/**
  * @brief   ���������е�����д��I2C EEPROM��
  * @param   
  *		@arg pBuffer:������ָ�� 
  *		@arg WriteAddr:д��ַ
  *     @arg NumByteToWrite:д���ֽ���
  * @retval  ��
  */
void I2C_EE_BufferWrite(uint8_t* pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite)
{
  uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0;

  Addr = WriteAddr % EEPROM_PAGESIZE;
  count = EEPROM_PAGESIZE - Addr;
  NumOfPage =  NumByteToWrite / EEPROM_PAGESIZE;
  NumOfSingle = NumByteToWrite % EEPROM_PAGESIZE;
 
  /* If WriteAddr is I2C_PageSize aligned ��� д���ַ �� I2Cҳ��ַ ���� */
  if(Addr == 0) 
  {
    /* If NumByteToWrite(д���ֽ���) < I2C_PageSize(I2Cҳ����С) */
    if(NumOfPage == 0) 
    {
      I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
    }
    /* If NumByteToWrite(д���ֽ���) > I2C_PageSize(I2Cҳ����С) */
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
  /* If WriteAddr is not I2C_PageSize aligned ��� д���ַ �� I2Cҳ��ַ û�ж��� */
  else 
  {
    /* If NumByteToWrite(д���ֽ���) < I2C_PageSize(I2Cҳ����С) */
    if(NumOfPage== 0) 
    {
      I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
    }
    /* If NumByteToWrite(д���ֽ���) > I2C_PageSize(I2Cҳ����С) */
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
