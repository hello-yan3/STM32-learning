#ifndef _BSP_I2C_GPIO_H
#define _BSP_I2C_GPIO_H


#include "stm32f1xx.h"


#define EEPROM_I2C_WR	0		/* д����bit */
#define EEPROM_I2C_RD	1		/* ������bit */


/* ����I2C�������ӵ�GPIO�˿�, �û�ֻ��Ҫ�޸�����4�д��뼴������ı�SCL��SDA������ */
#define I2Cx_SDA_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE() /* GPIO�˿�ʱ�� */
#define I2Cx_SCL_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE() /* GPIO�˿�ʱ�� */
#define I2Cx_SCL_PIN                    GPIO_PIN_6 	/* ���ӵ�SCLʱ���ߵ�GPIO */
#define I2Cx_SCL_GPIO_PORT              GPIOB		   	/* GPIO�˿� */
#define I2Cx_SDA_PIN                    GPIO_PIN_7 	/* ���ӵ�SCLʱ���ߵ�GPIO */
#define I2Cx_SDA_GPIO_PORT              GPIOB			  /* GPIO�˿� */



/* �����дSCL��SDA�ĺ꣬�����Ӵ���Ŀ���ֲ�ԺͿ��Ķ��� */
#define EEPROM_I2C_SCL_1()  HAL_GPIO_WritePin(I2Cx_SCL_GPIO_PORT, I2Cx_SCL_PIN, GPIO_PIN_SET)   /* SCL = 1 */	
#define EEPROM_I2C_SCL_0()  HAL_GPIO_WritePin(I2Cx_SCL_GPIO_PORT, I2Cx_SCL_PIN, GPIO_PIN_RESET) /* SCL = 0 */	

#define EEPROM_I2C_SDA_1()  HAL_GPIO_WritePin(I2Cx_SDA_GPIO_PORT, I2Cx_SDA_PIN, GPIO_PIN_SET)   /* SDA = 1 */	
#define EEPROM_I2C_SDA_0()  HAL_GPIO_WritePin(I2Cx_SDA_GPIO_PORT, I2Cx_SDA_PIN, GPIO_PIN_RESET) /* SDA = 0 */	

#define EEPROM_I2C_SDA_READ()  HAL_GPIO_ReadPin(I2Cx_SDA_GPIO_PORT, I2Cx_SDA_PIN)	/* ��SDA����״̬ */

void i2c_CfgGpio(void);
void i2c_Start(void);
void i2c_Stop(void);
void i2c_SendByte(uint8_t _ucByte);
uint8_t i2c_ReadByte(void);
uint8_t i2c_WaitAck(void);
void i2c_Ack(void);
void i2c_NAck(void);


#endif

