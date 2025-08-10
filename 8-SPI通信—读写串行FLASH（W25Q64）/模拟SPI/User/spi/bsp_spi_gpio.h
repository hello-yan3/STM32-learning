#ifndef _BSP_SPI_GPIO_H
#define _BSP_SPI_GPIO_H


#include "stm32f1xx.h"


/************************* SPI Ӳ����ض��� *************************/
/* SPI����ʱ��ʹ�� */
#define SPIx_SCK_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define SPIx_MISO_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()
#define SPIx_MOSI_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()
#define W25_CS_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOA_CLK_ENABLE()

/* SPI���ź궨�� */
#define SPIx_SCK_PIN                    GPIO_PIN_5
#define SPIx_SCK_GPIO_PORT              GPIOA

#define SPIx_MISO_PIN                   GPIO_PIN_6
#define SPIx_MISO_GPIO_PORT             GPIOA

#define SPIx_MOSI_PIN                   GPIO_PIN_7
#define SPIx_MOSI_GPIO_PORT             GPIOA

#define W25_CS_PIN                      GPIO_PIN_4
#define W25_CS_GPIO_PORT                GPIOA

/* SPI���Ź��ܺ궨�� */
/* SPI ʱ��CLK ���� SPI_CLK(1) SPI_CLK(0)*/
#define SPI_CLK(level)                  HAL_GPIO_WritePin(SPIx_SCK_GPIO_PORT,  SPIx_SCK_PIN,  level?GPIO_PIN_SET:GPIO_PIN_RESET)
/* SPI ���豸��� ���豸���� MOSI ���� SPI_MOSI(1) SPI_MOSI(0)*/
#define SPI_MOSI(level)                 HAL_GPIO_WritePin(SPIx_MOSI_GPIO_PORT, SPIx_MOSI_PIN, level?GPIO_PIN_SET:GPIO_PIN_RESET)
/* SPI ���豸��� ���豸���� MISO ���� SPI_MISO()*/
#define SPI_MISO()                      HAL_GPIO_ReadPin(SPIx_MISO_GPIO_PORT,  SPIx_MISO_PIN)
/* SPI Ƭѡ CS ���� W25_CS(1) W25_CS(0)*/
#define W25_CS(level)                   HAL_GPIO_WritePin(W25_CS_GPIO_PORT,    W25_CS_PIN,    level?GPIO_PIN_SET:GPIO_PIN_RESET)
/************************* SPI Ӳ����ض������ *************************/

/*����SPI����ģʽ*/
#define CPOL  0  // ʱ�Ӽ��� CPOL 0 , ����״̬ SCK �͵�ƽ
#define CPHA  0  // ʱ����λ CPHA 0 , SCK �������� ���� 
#define MSB   1  // ��λ����


/*��������*/
void SPI_Init(void);
void SPI_Delay(void);
void SPI_WriteByte(uint8_t data);
uint8_t SPI_ReadByte(void);
uint8_t SPI_WriteReadByte(uint8_t data);



#endif

