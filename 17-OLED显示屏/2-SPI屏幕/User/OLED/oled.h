#ifndef __OLED_H
#define __OLED_H 

#include "stm32f1xx.h"

//���Ŷ���
//����ʹ�õ���"��ȱ"�����SPI��û��MISO
//���SPIʹ���������ž���ʵ��
/*******************************************************/
// SCLK��SPIЭ���SCLK���ţ���ʱ˿ӡҲ���ע��D0��SCL��SCK��
#define SCL_PIN                  GPIO_PIN_12              
#define SCL_GPIO_PORT            GPIOG                    
#define SCL_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOG_CLK_ENABLE()

// SDA��SPIЭ���MOSI���ţ���ʱ˿ӡҲ���ע��D1, DIN��SDA��
#define SDA_PIN                  GPIO_PIN_5          
#define SDA_GPIO_PORT            GPIOD                   
#define SDA_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOD_CLK_ENABLE()

// RES��RESΪOLED��λ���ţ��͵�ƽ��λ��
#define RES_PIN                  GPIO_PIN_4         
#define RES_GPIO_PORT            GPIOD                      
#define RES_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOD_CLK_ENABLE()

// DC��DCΪOLED�� D/C#E ���ţ����ݺ�����Ŀ������ţ��ߵ�ƽд���ݣ�
#define DC_PIN                  GPIO_PIN_15         
#define DC_GPIO_PORT            GPIOD                      
#define DC_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOD_CLK_ENABLE()

// CS��CSΪOLED�� CS ���ţ���Ƭѡ����
#define CS_PIN                  GPIO_PIN_1         
#define CS_GPIO_PORT            GPIOD                      
#define CS_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOD_CLK_ENABLE()

/************************************************************/

//-----------------OLED�˿ڶ���---------------- 
#define OLED_SCLK_Clr()  HAL_GPIO_WritePin(SCL_GPIO_PORT, SCL_PIN, GPIO_PIN_RESET)
#define OLED_SCLK_Set()  HAL_GPIO_WritePin(SCL_GPIO_PORT, SCL_PIN, GPIO_PIN_SET)

#define OLED_SDIN_Clr()  HAL_GPIO_WritePin(SDA_GPIO_PORT, SDA_PIN, GPIO_PIN_RESET)
#define OLED_SDIN_Set()  HAL_GPIO_WritePin(SDA_GPIO_PORT, SDA_PIN, GPIO_PIN_SET)

#define OLED_RES_Clr()   HAL_GPIO_WritePin(RES_GPIO_PORT, RES_PIN, GPIO_PIN_RESET)
#define OLED_RES_Set()   HAL_GPIO_WritePin(RES_GPIO_PORT, RES_PIN, GPIO_PIN_SET)

#define OLED_DC_Clr()    HAL_GPIO_WritePin(DC_GPIO_PORT,  DC_PIN,  GPIO_PIN_RESET)
#define OLED_DC_Set()    HAL_GPIO_WritePin(DC_GPIO_PORT,  DC_PIN,  GPIO_PIN_SET)
 
#define OLED_CS_Clr()    HAL_GPIO_WritePin(CS_GPIO_PORT,  CS_PIN,  GPIO_PIN_RESET)
#define OLED_CS_Set()    HAL_GPIO_WritePin(CS_GPIO_PORT,  CS_PIN,  GPIO_PIN_SET) 
 
#define OLED_CMD  0	//д����
#define OLED_DATA 1	//д����

typedef   uint8_t     u8;
typedef   uint16_t    u16;
typedef   uint32_t    u32;

void OLED_SPI_Init(void);
void OLED_ClearPoint(u8 x,u8 y);
void OLED_ColorTurn(u8 i);
void OLED_DisplayTurn(u8 i);
void OLED_WR_Byte(u8 dat,u8 cmd);
void OLED_DisPlay_On(void);
void OLED_DisPlay_Off(void);
void OLED_Refresh(void);
void OLED_Clear(void);
void OLED_DrawPoint(u8 x,u8 y);
void OLED_DrawLine(u8 x1,u8 y1,u8 x2,u8 y2);
void OLED_DrawCircle(u8 x,u8 y,u8 r);
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size1);
void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 size1);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size1);
void OLED_ShowChinese(u8 x,u8 y,u8 num,u8 size1);
void OLED_ScrollDisplay(u8 num,u8 space);
void OLED_WR_BP(u8 x,u8 y);
void OLED_ShowPicture(u8 x0,u8 y0,u8 x1,u8 y1,u8 BMP[]);
void OLED_Init(void);
void OLED_Show(void);









#endif

