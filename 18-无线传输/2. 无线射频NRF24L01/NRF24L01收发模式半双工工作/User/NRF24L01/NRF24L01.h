#ifndef __NRF24L01_H
#define	__NRF24L01_H

#include "stm32f1xx.h"
#include "./nrf24l01/bsp_spi.h"
#include "./exti/bsp_exti.h"

/* ���Ź���˵����
 * 
 * CE��Chip Enable��оƬʹ�ܣ��ڷ��ͺͽ��չ����ж�Ҫ������������ߡ�
 * IRQ���͵�ƽ��������״̬�Ĵ����� TX_DS��RX_DR �� MAX_RT Ϊ��ʱ�����жϣ��� MCU ���ж�Դд 1 ʱ���ж����ű���ֹ��Ĭ��״̬�����е��ж�Դ�Ǳ���ֹ�ġ�
 * ��CE��IRQ����Ǳ�׼��SPI�ź�
 * 
 * ���Ŷ��壺
 *��ע��ͬʱ����Ұ��Ե���Ұ����������壩
 * IRQ_Pin    PB13    ��bsp_exti.h���޸�
 * CE_Pin     PC9     ��NRF24L01.h���޸�
 * CSN_Pin    PB10    ��bsp_spi.h���޸�
 * SCK_Pin    PA5     ��bsp_spi.h���޸�
 * MOSI_Pin   PA7     ��bsp_spi.h���޸�  
 * MISO_Pin   PA6     ��bsp_spi.h���޸� 
 */
 
/* CE���Ŷ��� */ 
#define NRF24L01_CE_GPIO_PORT                GPIOC
#define NRF24L01_CE_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOC_CLK_ENABLE()
#define NRF24L01_CE_GPIO_PIN                 GPIO_PIN_9

/**********  NRF24L01�Ĵ�����������  ***********/
#define nRF_READ_REG        0x00     // �����üĴ���,��5λΪ�Ĵ�����ַ��ʹ��ʱ��Ҫ (REG + �Ĵ�����ַ)
#define nRF_WRITE_REG       0x20     // д��������,��5λΪ�Ĵ�����ַ��ʹ��ʱ��Ҫ (REG + �Ĵ�����ַ)
#define RD_RX_PLOAD     0x61         // ��RX��Ч����, 1~32�ֽ�
#define WR_TX_PLOAD     0xA0         // дTX��Ч����, 1~32�ֽ�
#define FLUSH_TX        0xE1         // ���TX FIFO�Ĵ���.����ģʽ����
#define FLUSH_RX        0xE2         // ���RX FIFO�Ĵ���.����ģʽ����
#define REUSE_TX_PL     0xE3         // ����ʹ����һ������,CEΪ��,���ݰ������Ϸ���.
#define NOP             0xFF         // �ղ���,����������״̬�Ĵ���
/**********  NRF24L01�Ĵ�����ַ   *************/
#define CONFIG          0x00         // ���üĴ�����ַ;  bit0:1����ģʽ,0����ģʽ;  bit1:��ѡ��;  bit2:CRCģʽ;  bit3:CRCʹ��;
                                     //                 bit4:�ж�MAX_RT(�ﵽ����ط������ж�)ʹ��;  bit5:�ж�TX_DSʹ��;  bit6:�ж�RX_DRʹ��    
#define EN_AA           0x01         // ʹ���Զ�Ӧ����  bit0~5,��Ӧͨ��0~5
#define EN_RXADDR       0x02         // ���յ�ַ����,bit0~5,��Ӧͨ��0~5
#define SETUP_AW        0x03         // ���õ�ַ���(��������ͨ��): bit1,0:00,3�ֽ�; 01,4�ֽ�; 02,5�ֽ�;
#define SETUP_RETR      0x04         // �����Զ��ط�; bit3:0,�Զ��ط�������; bit7:4,�Զ��ط���ʱ 250*x+86us
#define RF_CH           0x05         // ��Ƶͨ��, bit6:0,����ͨ��Ƶ��;
#define RF_SETUP        0x06         // ��Ƶ�Ĵ���; bit3:��������(0:1Mbps,1:2Mbps); bit2:1,���书��; bit0:�������Ŵ�������
#define STATUS          0x07         // ״̬�Ĵ���;bit0:TX FIFO����־; bit3:1,��������ͨ����(���:6); bit4,�ﵽ�����ط�
                                     //           bit5:���ݷ�������ж�; bit6:���������ж�;
#define OBSERVE_TX      0x08         // ���ͼ��Ĵ���,bit7:4,���ݰ���ʧ������; bit3:0,�ط�������
#define CD              0x09         // �ز����Ĵ���,bit0,�ز����;
#define RX_ADDR_P0      0x0A 				 // ����ͨ��0���յ�ַ,��󳤶�5���ֽ�,���ֽ���ǰ
#define RX_ADDR_P1      0x0B 				 // ����ͨ��1���յ�ַ,��󳤶�5���ֽ�,���ֽ���ǰ
#define RX_ADDR_P2      0x0C 				 // ����ͨ��2���յ�ַ,����ֽڿ�����,���ֽ�,����ͬRX_ADDR_P1[39:8]���;
#define RX_ADDR_P3      0x0D				 // ����ͨ��3���յ�ַ,����ֽڿ�����,���ֽ�,����ͬRX_ADDR_P1[39:8]���;
#define RX_ADDR_P4      0x0E 				 // ����ͨ��4���յ�ַ,����ֽڿ�����,���ֽ�,����ͬRX_ADDR_P1[39:8]���;
#define RX_ADDR_P5      0x0F 				 // ����ͨ��5���յ�ַ,����ֽڿ�����,���ֽ�,����ͬRX_ADDR_P1[39:8]���;
#define TX_ADDR         0x10         // ���͵�ַ(���ֽ���ǰ),ShockBurstTMģʽ��,RX_ADDR_P0��˵�ַ���
#define RX_PW_P0        0x11  			 // ��������ͨ��0��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define RX_PW_P1        0x12  			 // ��������ͨ��1��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define RX_PW_P2        0x13  		 	 // ��������ͨ��2��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define RX_PW_P3        0x14 				 // ��������ͨ��3��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define RX_PW_P4        0x15 				 // ��������ͨ��4��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define RX_PW_P5        0x16 				 // ��������ͨ��5��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define FIFO_STATUS     0x17         // FIFO״̬�Ĵ���; bit0,RX FIFO�Ĵ����ձ�־; bit1,RX FIFO����־; bit2,3,����
                                     //                bit4,TX FIFO�ձ�־; bit5,TX FIFO����־; bit6,1,ѭ��������һ���ݰ�.0,��ѭ��;

/******   STATUS�Ĵ���bitλ����      *******/
#define TX_FULL   0x00               // FIFO �Ĵ�������־
#define MAX_TX  	0x10  	           // �ﵽ����ʹ����ж�
#define TX_OK   	0x20  	           // TX��������ж�
#define RX_OK   	0x40  	           // ���յ������ж�

 
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

