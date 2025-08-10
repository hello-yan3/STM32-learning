#ifndef __I2C_EE_H
#define	__I2C_EE_H

#include "stm32f1xx.h"
#include "./i2c/bsp_i2c_gpio.h" 

/* 
 * AT24C02 2kb = 2048bit = 2048/8 B = 256 B
 * 32 pages of 8 bytes each
 *
 * Device Address
 * 1 0 1 0 A2 A1 A0 R/W
 * 1 0 1 0 0  0  0  0 = 0XA0
 * 1 0 1 0 0  0  0  1 = 0XA1 
 */

/* AT24C01/02每页有8个字节 
 * AT24C04/08A/16A每页有16个字节 
 */
 
/* AT24C01/02每页有8个字节 */
#define EEPROM_PAGE_SIZE 	   8
/* AT24C04/08A/16A每页有16个字节 */
//#define EEPROM_PAGESIZE           16			
/* AT24C01设备地址 */
#define EEPROM_DEV_ADDR		 0xA0	  	
/* AT24C01设备总容量 */
#define EEPROM_SIZE				  256			 

uint8_t i2c_CheckDevice(uint8_t _Address);
uint8_t ee_CheckOk(void);
uint8_t ee_ReadBytes(uint8_t *_pReadBuf, uint16_t _usAddress, uint16_t _usSize);
uint8_t ee_WriteBytes(uint8_t *_pWriteBuf, uint16_t _usAddress, uint16_t _usSize);
uint8_t ee_Test(void);


/*等待超时时间*/
#define I2CT_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define I2CT_LONG_TIMEOUT         ((uint32_t)(10 * I2CT_FLAG_TIMEOUT))

#define I2Cx_TIMEOUT_MAX                300

/*信息输出*/
#define EEPROM_DEBUG_ON         0

#define EEPROM_INFO(fmt,arg...)           printf("<<-EEPROM-INFO->> "fmt"\n",##arg)
#define EEPROM_ERROR(fmt,arg...)          printf("<<-EEPROM-ERROR->> "fmt"\n",##arg)
#define EEPROM_DEBUG(fmt,arg...)          do{\
                                          if(EEPROM_DEBUG_ON)\
                                          printf("<<-EEPROM-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
                                          }while(0)

/* 
 * AT24C02 2kb = 2048bit = 2048/8 B = 256 B   
 * 32 pages of 8 bytes each
 * AT24C02 2kb, 256bytes, 共32page, 每页8bytes
 *																				
 * I2C Device Address
 * 1 0 1 0 A2 A1 A0 R/W
 * 1 0 1 0 0  0  0  0 = 0XA0
 * 1 0 1 0 0  0  0  1 = 0XA1 
 */
/* EEPROM Addresses defines */
/* (A2 A1 A0电压不同时的I2C地址，目前A2 A1 A0接地，所以地址0xA0) */
#define EEPROM_Block0_ADDRESS 0xA0   
#define EEPROM_Block1_ADDRESS 0xA2   
#define EEPROM_Block2_ADDRESS 0xA4   
#define EEPROM_Block3_ADDRESS 0xA6   

#endif /* __I2C_EE_H */
