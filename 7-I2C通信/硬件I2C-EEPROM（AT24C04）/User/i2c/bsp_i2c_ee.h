#ifndef __I2C_EE_H
#define	__I2C_EE_H

#include "stm32f1xx.h"

/* AT24C01/02ÿҳ��8���ֽ� */
#define EEPROM_PAGESIZE 	   8
/* AT24C04/08A/16Aÿҳ��16���ֽ� */
//#define EEPROM_PAGESIZE           16			
/* AT24C01�豸��ַ */
#define EEPROM_ADDRESS        0xA0       // 7λ�豸��ַ��ע���ַ��������

/* �����ַֻҪ��STM32��ҵ�I2C������ַ��һ������ */
#define I2C_OWN_ADDRESS7      0X0A   

/* STM32 I2C ����ģʽ */
#define I2C_Speed              400000  

/* 
 * AT24C02 2kb = 2048bit = 2048/8 B = 256 B
 * 32 pages of 8 bytes each
 *
 * Device Address
 * 1 0 1 0 A2 A1 A0 R/W
 * 1 0 1 0 0  0  0  0 = 0XA0
 * 1 0 1 0 0  0  0  1 = 0XA1 
 */

/* AT24C01/02ÿҳ��8���ֽ� 
 * AT24C04/08A/16Aÿҳ��16���ֽ� 
 */

#define I2Cx                             I2C1
#define I2Cx_CLK_ENABLE()                __HAL_RCC_I2C1_CLK_ENABLE()
#define I2Cx_SDA_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE()
#define I2Cx_SCL_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE() 

#define I2Cx_FORCE_RESET()               __HAL_RCC_I2C1_FORCE_RESET()
#define I2Cx_RELEASE_RESET()             __HAL_RCC_I2C1_RELEASE_RESET()

/* Definition for I2Cx Pins */
#define I2Cx_SCL_PIN                    GPIO_PIN_6
#define I2Cx_SCL_GPIO_PORT              GPIOB
#define I2Cx_SDA_PIN                    GPIO_PIN_7
#define I2Cx_SDA_GPIO_PORT              GPIOB


/*�ȴ���ʱʱ��*/
#define I2CT_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define I2CT_LONG_TIMEOUT         ((uint32_t)(10 * I2CT_FLAG_TIMEOUT))

#define I2Cx_TIMEOUT_MAX                300
/* Maximum number of trials for HAL_I2C_IsDeviceReady() function */
/*HAL_I2C_IsDeviceReady() ����������Դ���*/
#define EEPROM_MAX_TRIALS               300

/*��Ϣ���*/
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
 * AT24C02 2kb, 256bytes, ��32page, ÿҳ8bytes
 *																				
 * I2C Device Address
 * 1 0 1 0 A2 A1 A0 R/W
 * 1 0 1 0 0  0  0  0 = 0XA0
 * 1 0 1 0 0  0  0  1 = 0XA1 
 */
/* EEPROM Addresses defines */
/* (A2 A1 A0��ѹ��ͬʱ��I2C��ַ��ĿǰA2 A1 A0�ӵأ����Ե�ַ0xA0) */
#define EEPROM_Block0_ADDRESS 0xA0   
#define EEPROM_Block1_ADDRESS 0xA2   
#define EEPROM_Block2_ADDRESS 0xA4   
#define EEPROM_Block3_ADDRESS 0xA6   

extern I2C_HandleTypeDef  I2C_Handle; 
																					
void I2C_EE_Init(void);
			 							  
void I2C_EE_BufferWrite(uint8_t* pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite);
uint8_t I2C_EE_ByteWrite(uint8_t* pBuffer, uint8_t WriteAddr);
uint8_t I2C_EE_PageWrite(uint8_t* pBuffer, uint8_t WriteAddr, uint8_t NumByteToWrite);
uint8_t I2C_EE_BufferRead(uint8_t* pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead);

#endif /* __I2C_EE_H */
