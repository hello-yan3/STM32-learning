#ifndef _BSP_I2C_GPIO_H
#define _BSP_I2C_GPIO_H


#include "stm32f1xx.h"


#define EEPROM_I2C_WR	0		/* 写控制bit */
#define EEPROM_I2C_RD	1		/* 读控制bit */


/* 定义I2C总线连接的GPIO端口, 用户只需要修改下面4行代码即可任意改变SCL和SDA的引脚 */
#define I2Cx_SDA_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE() /* GPIO端口时钟 */
#define I2Cx_SCL_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE() /* GPIO端口时钟 */
#define I2Cx_SCL_PIN                    GPIO_PIN_6 	/* 连接到SCL时钟线的GPIO */
#define I2Cx_SCL_GPIO_PORT              GPIOB		   	/* GPIO端口 */
#define I2Cx_SDA_PIN                    GPIO_PIN_7 	/* 连接到SCL时钟线的GPIO */
#define I2Cx_SDA_GPIO_PORT              GPIOB			  /* GPIO端口 */



/* 定义读写SCL和SDA的宏，已增加代码的可移植性和可阅读性 */
#define EEPROM_I2C_SCL_1()  HAL_GPIO_WritePin(I2Cx_SCL_GPIO_PORT, I2Cx_SCL_PIN, GPIO_PIN_SET)   /* SCL = 1 */	
#define EEPROM_I2C_SCL_0()  HAL_GPIO_WritePin(I2Cx_SCL_GPIO_PORT, I2Cx_SCL_PIN, GPIO_PIN_RESET) /* SCL = 0 */	

#define EEPROM_I2C_SDA_1()  HAL_GPIO_WritePin(I2Cx_SDA_GPIO_PORT, I2Cx_SDA_PIN, GPIO_PIN_SET)   /* SDA = 1 */	
#define EEPROM_I2C_SDA_0()  HAL_GPIO_WritePin(I2Cx_SDA_GPIO_PORT, I2Cx_SDA_PIN, GPIO_PIN_RESET) /* SDA = 0 */	

#define EEPROM_I2C_SDA_READ()  HAL_GPIO_ReadPin(I2Cx_SDA_GPIO_PORT, I2Cx_SDA_PIN)	/* 读SDA口线状态 */

void i2c_CfgGpio(void);
void i2c_Start(void);
void i2c_Stop(void);
void i2c_SendByte(uint8_t _ucByte);
uint8_t i2c_ReadByte(void);
uint8_t i2c_WaitAck(void);
void i2c_Ack(void);
void i2c_NAck(void);


#endif

