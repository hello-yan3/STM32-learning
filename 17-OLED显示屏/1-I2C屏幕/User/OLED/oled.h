#ifndef __OLED_H
#define __OLED_H 

#include "stm32f1xx.h"

//引脚定义
//这里使用的是软件I2C，所以任意引脚均可
/*******************************************************/
// SCL，I2C协议的SCL引脚（有时丝印也会标注：D0）
#define SCL_PIN                  GPIO_PIN_12              
#define SCL_GPIO_PORT            GPIOG                    
#define SCL_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOG_CLK_ENABLE()

// SDA，I2C协议的SDA引脚（有时丝印也会标注：D1, DIN）
#define SDA_PIN                  GPIO_PIN_5          
#define SDA_GPIO_PORT            GPIOD                   
#define SDA_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOD_CLK_ENABLE()

// RES，RES为OLED复位引脚（低电平复位），对于5引脚OLED显示屏，使用前需要先复位
#define RES_PIN                  GPIO_PIN_4         
#define RES_GPIO_PORT            GPIOD                      
#define RES_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOD_CLK_ENABLE()

/************************************************************/

//-----------------OLED端口定义---------------- 
#define OLED_SCL_Clr()  HAL_GPIO_WritePin(SCL_GPIO_PORT, SCL_PIN, GPIO_PIN_RESET)
#define OLED_SCL_Set()  HAL_GPIO_WritePin(SCL_GPIO_PORT, SCL_PIN, GPIO_PIN_SET)

#define OLED_SDA_Clr()  HAL_GPIO_WritePin(SDA_GPIO_PORT, SDA_PIN, GPIO_PIN_RESET)
#define OLED_SDA_Set()  HAL_GPIO_WritePin(SDA_GPIO_PORT, SDA_PIN, GPIO_PIN_SET)

#define OLED_RES_Clr()  HAL_GPIO_WritePin(RES_GPIO_PORT, RES_PIN, GPIO_PIN_RESET)
#define OLED_RES_Set()  HAL_GPIO_WritePin(RES_GPIO_PORT, RES_PIN, GPIO_PIN_SET)
 
#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据

// OLED显示屏地址为  0x78
#define OLED_ADDR   0x78

typedef   uint8_t     u8;
typedef   uint16_t    u16;
typedef   uint32_t    u32;

void OLED_I2C_Init(void);
void OLED_ClearPoint(u8 x,u8 y);
void OLED_ColorTurn(u8 i);
void OLED_DisplayTurn(u8 i);
void OLED_I2C_Start(void);
void OLED_I2C_Stop(void);
void OLED_I2C_WaitAck(void);
void OLED_Send_Byte(u8 dat);
void OLED_WR_Byte(u8 dat,u8 mode);
void OLED_DisPlay_On(void);
void OLED_DisPlay_Off(void);
void OLED_Refresh(void);
void OLED_Clear(void);
void OLED_DrawPoint(u8 x,u8 y,u8 t);
void OLED_DrawLine(u8 x1,u8 y1,u8 x2,u8 y2,u8 mode);
void OLED_DrawCircle(u8 x,u8 y,u8 r);
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size1,u8 mode);
void OLED_ShowChar6x8(u8 x,u8 y,u8 chr,u8 mode);
void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 size1,u8 mode);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size1,u8 mode);
void OLED_ShowChinese(u8 x,u8 y,u8 num,u8 size1,u8 mode);
void OLED_ScrollDisplay(u8 num,u8 space,u8 mode);
void OLED_ShowPicture(u8 x,u8 y,u8 sizex,u8 sizey,u8 BMP[],u8 mode);
void OLED_Init(void);
void OLED_Show(void);

#endif

