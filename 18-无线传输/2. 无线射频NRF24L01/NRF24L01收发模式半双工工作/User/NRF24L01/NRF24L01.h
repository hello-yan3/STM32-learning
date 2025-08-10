#ifndef __NRF24L01_H
#define	__NRF24L01_H

#include "stm32f1xx.h"
#include "./nrf24l01/bsp_spi.h"
#include "./exti/bsp_exti.h"

/* 引脚功能说明：
 * 
 * CE：Chip Enable，芯片使能，在发送和接收过程中都要将这个引脚拉高。
 * IRQ：低电平触发，当状态寄存器中 TX_DS、RX_DR 或 MAX_RT 为高时触发中断，当 MCU 给中断源写 1 时，中断引脚被禁止。默认状态下所有的中断源是被禁止的。
 * 除CE和IRQ外就是标准的SPI信号
 * 
 * 引脚定义：
 *（注意同时兼容野火霸道和野火拂晓开发板）
 * IRQ_Pin    PB13    在bsp_exti.h中修改
 * CE_Pin     PC9     在NRF24L01.h中修改
 * CSN_Pin    PB10    在bsp_spi.h中修改
 * SCK_Pin    PA5     在bsp_spi.h中修改
 * MOSI_Pin   PA7     在bsp_spi.h中修改  
 * MISO_Pin   PA6     在bsp_spi.h中修改 
 */
 
/* CE引脚定义 */ 
#define NRF24L01_CE_GPIO_PORT                GPIOC
#define NRF24L01_CE_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOC_CLK_ENABLE()
#define NRF24L01_CE_GPIO_PIN                 GPIO_PIN_9

/**********  NRF24L01寄存器操作命令  ***********/
#define nRF_READ_REG        0x00     // 读配置寄存器,低5位为寄存器地址，使用时需要 (REG + 寄存器地址)
#define nRF_WRITE_REG       0x20     // 写配置命令,低5位为寄存器地址，使用时需要 (REG + 寄存器地址)
#define RD_RX_PLOAD     0x61         // 读RX有效数据, 1~32字节
#define WR_TX_PLOAD     0xA0         // 写TX有效数据, 1~32字节
#define FLUSH_TX        0xE1         // 清除TX FIFO寄存器.发射模式下用
#define FLUSH_RX        0xE2         // 清除RX FIFO寄存器.接收模式下用
#define REUSE_TX_PL     0xE3         // 重新使用上一包数据,CE为高,数据包被不断发送.
#define NOP             0xFF         // 空操作,可以用来读状态寄存器
/**********  NRF24L01寄存器地址   *************/
#define CONFIG          0x00         // 配置寄存器地址;  bit0:1接收模式,0发射模式;  bit1:电选择;  bit2:CRC模式;  bit3:CRC使能;
                                     //                 bit4:中断MAX_RT(达到最大重发次数中断)使能;  bit5:中断TX_DS使能;  bit6:中断RX_DR使能    
#define EN_AA           0x01         // 使能自动应答功能  bit0~5,对应通道0~5
#define EN_RXADDR       0x02         // 接收地址允许,bit0~5,对应通道0~5
#define SETUP_AW        0x03         // 设置地址宽度(所有数据通道): bit1,0:00,3字节; 01,4字节; 02,5字节;
#define SETUP_RETR      0x04         // 建立自动重发; bit3:0,自动重发计数器; bit7:4,自动重发延时 250*x+86us
#define RF_CH           0x05         // 射频通道, bit6:0,工作通道频率;
#define RF_SETUP        0x06         // 射频寄存器; bit3:传输速率(0:1Mbps,1:2Mbps); bit2:1,发射功率; bit0:低噪声放大器增益
#define STATUS          0x07         // 状态寄存器;bit0:TX FIFO满标志; bit3:1,接收数据通道号(最大:6); bit4,达到最多次重发
                                     //           bit5:数据发送完成中断; bit6:接收数据中断;
#define OBSERVE_TX      0x08         // 发送检测寄存器,bit7:4,数据包丢失计数器; bit3:0,重发计数器
#define CD              0x09         // 载波检测寄存器,bit0,载波检测;
#define RX_ADDR_P0      0x0A 				 // 数据通道0接收地址,最大长度5个字节,低字节在前
#define RX_ADDR_P1      0x0B 				 // 数据通道1接收地址,最大长度5个字节,低字节在前
#define RX_ADDR_P2      0x0C 				 // 数据通道2接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等;
#define RX_ADDR_P3      0x0D				 // 数据通道3接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等;
#define RX_ADDR_P4      0x0E 				 // 数据通道4接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等;
#define RX_ADDR_P5      0x0F 				 // 数据通道5接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等;
#define TX_ADDR         0x10         // 发送地址(低字节在前),ShockBurstTM模式下,RX_ADDR_P0与此地址相等
#define RX_PW_P0        0x11  			 // 接收数据通道0有效数据宽度(1~32字节),设置为0则非法
#define RX_PW_P1        0x12  			 // 接收数据通道1有效数据宽度(1~32字节),设置为0则非法
#define RX_PW_P2        0x13  		 	 // 接收数据通道2有效数据宽度(1~32字节),设置为0则非法
#define RX_PW_P3        0x14 				 // 接收数据通道3有效数据宽度(1~32字节),设置为0则非法
#define RX_PW_P4        0x15 				 // 接收数据通道4有效数据宽度(1~32字节),设置为0则非法
#define RX_PW_P5        0x16 				 // 接收数据通道5有效数据宽度(1~32字节),设置为0则非法
#define FIFO_STATUS     0x17         // FIFO状态寄存器; bit0,RX FIFO寄存器空标志; bit1,RX FIFO满标志; bit2,3,保留
                                     //                bit4,TX FIFO空标志; bit5,TX FIFO满标志; bit6,1,循环发送上一数据包.0,不循环;

/******   STATUS寄存器bit位定义      *******/
#define TX_FULL   0x00               // FIFO 寄存器满标志
#define MAX_TX  	0x10  	           // 达到最大发送次数中断
#define TX_OK   	0x20  	           // TX发送完成中断
#define RX_OK   	0x40  	           // 接收到数据中断

 
uint8_t NRF24L01_Write_Reg(uint8_t Reg, uint8_t Value);
uint8_t NRF24L01_Read_Reg(uint8_t Reg);
uint8_t NRF24L01_Write_Buf(uint8_t Reg, uint8_t *Buf, uint8_t Len);
uint8_t NRF24L01_Read_Buf(uint8_t Reg, uint8_t *Buf, uint8_t Len);
uint8_t NRF24L01_SendTxBuf(uint8_t *Buf);
uint8_t NRF24L01_GetRxBuf(uint8_t *Buf);
void NRF24L01_Init(void);
void NRF24L01_SendString(char *String);
void NRF24L01_SendMunArray(uint8_t *Num, uint8_t len);
void NRF24L01_SendNum(uint32_t Num);
uint8_t NRF24L01_GetStr(char *RxBuf, char *ShowBuf);
uint32_t NRF24L01_GetNum(void);
 
#endif /* __NRF24L01_H */

