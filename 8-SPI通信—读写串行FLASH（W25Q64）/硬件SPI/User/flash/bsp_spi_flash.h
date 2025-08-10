#ifndef __SPI_FLASH_H
#define __SPI_FLASH_H

#include "stm32f1xx.h"
#include <stdio.h>

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


#define SPIx                             SPI1
#define SPIx_CLK_ENABLE()                __HAL_RCC_SPI1_CLK_ENABLE()
#define SPIx_SCK_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOA_CLK_ENABLE()
#define SPIx_MISO_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE() 
#define SPIx_MOSI_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE() 
#define SPIx_CS_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOA_CLK_ENABLE() 

#define SPIx_FORCE_RESET()               __HAL_RCC_SPI1_FORCE_RESET()
#define SPIx_RELEASE_RESET()             __HAL_RCC_SPI1_RELEASE_RESET()

/* Definition for SPIx Pins */
#define SPIx_SCK_PIN                     GPIO_PIN_5
#define SPIx_SCK_GPIO_PORT               GPIOA

#define SPIx_MISO_PIN                    GPIO_PIN_6
#define SPIx_MISO_GPIO_PORT              GPIOA

#define SPIx_MOSI_PIN                    GPIO_PIN_7
#define SPIx_MOSI_GPIO_PORT              GPIOA

#define FLASH_CS_PIN                     GPIO_PIN_4               
#define FLASH_CS_GPIO_PORT               GPIOA                     


//#define	digitalHi(p,i)			    {p->BSRR=i;}			  //设置为高电平		
//#define digitalLo(p,i)			    {p->BSRR=(uint32_t)i << 16;}	//输出低电平
//#define SPI_FLASH_CS_LOW()      digitalLo(FLASH_CS_GPIO_PORT,FLASH_CS_PIN )
//#define SPI_FLASH_CS_HIGH()     digitalHi(FLASH_CS_GPIO_PORT,FLASH_CS_PIN )

#define SPI_FLASH_CS_LOW()      HAL_GPIO_WritePin(FLASH_CS_GPIO_PORT, FLASH_CS_PIN, GPIO_PIN_RESET)
#define SPI_FLASH_CS_HIGH()     HAL_GPIO_WritePin(FLASH_CS_GPIO_PORT, FLASH_CS_PIN, GPIO_PIN_SET)


/*SPI接口定义-结尾****************************/

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

extern SPI_HandleTypeDef SpiHandle;

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
uint8_t SPI_FLASH_ReadByte(void);
uint8_t SPI_FLASH_SendByte(uint8_t byte);
void SPI_FLASH_WriteEnable(void);
void SPI_FLASH_WaitForWriteEnd(void);

#endif /* __SPI_FLASH_H */

