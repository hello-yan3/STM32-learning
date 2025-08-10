#ifndef _BSP_SPI_GPIO_H
#define _BSP_SPI_GPIO_H


#include "stm32f1xx.h"


/************************* SPI 硬件相关定义 *************************/
/* SPI引脚时钟使能 */
#define SPIx_SCK_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define SPIx_MISO_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()
#define SPIx_MOSI_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()
#define W25_CS_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOA_CLK_ENABLE()

/* SPI引脚宏定义 */
#define SPIx_SCK_PIN                    GPIO_PIN_5
#define SPIx_SCK_GPIO_PORT              GPIOA

#define SPIx_MISO_PIN                   GPIO_PIN_6
#define SPIx_MISO_GPIO_PORT             GPIOA

#define SPIx_MOSI_PIN                   GPIO_PIN_7
#define SPIx_MOSI_GPIO_PORT             GPIOA

#define W25_CS_PIN                      GPIO_PIN_4
#define W25_CS_GPIO_PORT                GPIOA

/* SPI引脚功能宏定义 */
/* SPI 时钟CLK 引脚 SPI_CLK(1) SPI_CLK(0)*/
#define SPI_CLK(level)                  HAL_GPIO_WritePin(SPIx_SCK_GPIO_PORT,  SPIx_SCK_PIN,  level?GPIO_PIN_SET:GPIO_PIN_RESET)
/* SPI 主设备输出 从设备输入 MOSI 引脚 SPI_MOSI(1) SPI_MOSI(0)*/
#define SPI_MOSI(level)                 HAL_GPIO_WritePin(SPIx_MOSI_GPIO_PORT, SPIx_MOSI_PIN, level?GPIO_PIN_SET:GPIO_PIN_RESET)
/* SPI 从设备输出 主设备输入 MISO 引脚 SPI_MISO()*/
#define SPI_MISO()                      HAL_GPIO_ReadPin(SPIx_MISO_GPIO_PORT,  SPIx_MISO_PIN)
/* SPI 片选 CS 引脚 W25_CS(1) W25_CS(0)*/
#define W25_CS(level)                   HAL_GPIO_WritePin(W25_CS_GPIO_PORT,    W25_CS_PIN,    level?GPIO_PIN_SET:GPIO_PIN_RESET)
/************************* SPI 硬件相关定义结束 *************************/

/*配置SPI工作模式*/
#define CPOL  0  // 时钟极性 CPOL 0 , 空闲状态 SCK 低电平
#define CPHA  0  // 时钟相位 CPHA 0 , SCK 奇数边沿 采样 
#define MSB   1  // 高位先行


/*函数声明*/
void SPI_Init(void);
void SPI_Delay(void);
void SPI_WriteByte(uint8_t data);
uint8_t SPI_ReadByte(void);
uint8_t SPI_WriteReadByte(uint8_t data);



#endif

