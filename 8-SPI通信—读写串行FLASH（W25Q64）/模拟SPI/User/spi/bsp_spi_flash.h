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
/*命令定义-开头*******************************/
#define W25X_WriteEnable		      0x06   // 写入使能
#define W25X_WriteDisable		      0x04   // 写入禁用
#define W25X_ReadStatusReg		    0x05   // 读状态寄存器
#define W25X_WriteStatusReg		    0x01   // 写状态寄存器 
#define W25X_ReadData			        0x03   // 读数据
#define W25X_FastReadData		      0x0B   // 快速读数据 
#define W25X_FastReadDual		      0x3B   // 快速双线读数据 
#define W25X_PageProgram		      0x02   // 页写入
#define W25X_BlockErase			      0xD8   // 块擦除
#define W25X_SectorErase		      0x20   // 扇区擦除 
#define W25X_ChipErase			      0xC7   // 芯片擦除 
#define W25X_PowerDown			      0xB9   // 断电
#define W25X_ReleasePowerDown	    0xAB   // 释放断电 
#define W25X_DeviceID			        0xAB   // 查询设备ID
#define W25X_ManufactDeviceID   	0x90   // 制造商设备ID
#define W25X_JedecDeviceID		    0x9F   // Jedec设备ID

#define WIP_Flag                  0x01  // Write In Progress (WIP) flag 写入进行中 (WIP) 标志 
#define Dummy_Byte                0xFF  // 虚拟数据
/*命令定义-结尾*******************************/
     

/*等待超时时间*/
#define SPIT_FLAG_TIMEOUT         ((uint32_t)0x8FFF)
#define SPIT_LONG_TIMEOUT         ((uint32_t)(10 * SPIT_FLAG_TIMEOUT))

/*信息输出*/
#define FLASH_DEBUG_ON         1

#define FLASH_INFO(fmt,arg...)           printf("<<-FLASH-INFO->> "fmt"\n",##arg)
#define FLASH_ERROR(fmt,arg...)          printf("<<-FLASH-ERROR->> "fmt"\n",##arg)
#define FLASH_DEBUG(fmt,arg...)          do{\
                                          if(FLASH_DEBUG_ON)\
                                          printf("<<-FLASH-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
                                          }while(0)

/* 函数重定义（兼容HAL_SPI程序）*/
#define SPI_FLASH_CS_LOW()         do {W25_CS(0); SPI_Delay(); } while(0)  // CS低电平, FLASH 使能
#define SPI_FLASH_CS_HIGH()        do {W25_CS(1); SPI_Delay(); } while(0)  // CS高电平, FLASH 关断
#define SPI_FLASH_SendByte(data)   SPI_WriteReadByte(data)			     // 使用SPI发送一个字节的数据
#define SPI_FLASH_ReadByte()       SPI_WriteReadByte(Dummy_Byte)	
																					
/* 函数声明定义 */																																																							
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

