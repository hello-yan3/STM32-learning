#ifndef __SPI_FLASH_H
#define __SPI_FLASH_H

#include "stm32f1xx.h"
#include <stdio.h>
#include "bsp_spi_gpio.h"

/* Private typedef -----------------------------------------------------------*/
//#define  sFLASH_ID                       0xEF3015     //W25X16
//#define  sFLASH_ID                       0xEF4015	    //W25Q16
#define  sFLASH_ID                        0XEF4017     //W25Q64
//#define  sFLASH_ID                       0XEF4018    //W25Q128


//#define SPI_FLASH_PageSize            4096
#define SPI_FLASH_PageSize              256
#define SPI_FLASH_PerWritePageSize      256

/* Private define ------------------------------------------------------------*/
/*�����-��ͷ*******************************/
#define W25X_WriteEnable		      0x06   // д��ʹ��
#define W25X_WriteDisable		      0x04   // д�����
#define W25X_ReadStatusReg		    0x05   // ��״̬�Ĵ���
#define W25X_WriteStatusReg		    0x01   // д״̬�Ĵ��� 
#define W25X_ReadData			        0x03   // ������
#define W25X_FastReadData		      0x0B   // ���ٶ����� 
#define W25X_FastReadDual		      0x3B   // ����˫�߶����� 
#define W25X_PageProgram		      0x02   // ҳд��
#define W25X_BlockErase			      0xD8   // �����
#define W25X_SectorErase		      0x20   // �������� 
#define W25X_ChipErase			      0xC7   // оƬ���� 
#define W25X_PowerDown			      0xB9   // �ϵ�
#define W25X_ReleasePowerDown	    0xAB   // �ͷŶϵ� 
#define W25X_DeviceID			        0xAB   // ��ѯ�豸ID
#define W25X_ManufactDeviceID   	0x90   // �������豸ID
#define W25X_JedecDeviceID		    0x9F   // Jedec�豸ID

#define WIP_Flag                  0x01  // Write In Progress (WIP) flag д������� (WIP) ��־ 
#define Dummy_Byte                0xFF  // ��������
/*�����-��β*******************************/
     

/*�ȴ���ʱʱ��*/
#define SPIT_FLAG_TIMEOUT         ((uint32_t)0x8FFF)
#define SPIT_LONG_TIMEOUT         ((uint32_t)(10 * SPIT_FLAG_TIMEOUT))

/*��Ϣ���*/
#define FLASH_DEBUG_ON         1

#define FLASH_INFO(fmt,arg...)           printf("<<-FLASH-INFO->> "fmt"\n",##arg)
#define FLASH_ERROR(fmt,arg...)          printf("<<-FLASH-ERROR->> "fmt"\n",##arg)
#define FLASH_DEBUG(fmt,arg...)          do{\
                                          if(FLASH_DEBUG_ON)\
                                          printf("<<-FLASH-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
                                          }while(0)

/* �����ض��壨����HAL_SPI����*/
#define SPI_FLASH_CS_LOW()         do {W25_CS(0); SPI_Delay(); } while(0)  // CS�͵�ƽ, FLASH ʹ��
#define SPI_FLASH_CS_HIGH()        do {W25_CS(1); SPI_Delay(); } while(0)  // CS�ߵ�ƽ, FLASH �ض�
#define SPI_FLASH_SendByte(data)   SPI_WriteReadByte(data)			     // ʹ��SPI����һ���ֽڵ�����
#define SPI_FLASH_ReadByte()       SPI_WriteReadByte(Dummy_Byte)	
																					
/* ������������ */																																																							
void SPI_FLASH_Init(void);
void SPI_FLASH_SectorErase(uint32_t SectorAddr);
void SPI_FLASH_BulkErase(void);
void SPI_FLASH_PageWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void SPI_FLASH_BufferWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void SPI_FLASH_BufferRead(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
uint32_t SPI_FLASH_ReadID(void);
uint32_t SPI_FLASH_ReadDeviceID(void);
void SPI_Flash_PowerDown(void);
void SPI_Flash_WAKEUP(void);
void SPI_FLASH_WriteEnable(void);
void SPI_FLASH_WaitForWriteEnd(void);																																			

#endif /* __SPI_FLASH_H */

