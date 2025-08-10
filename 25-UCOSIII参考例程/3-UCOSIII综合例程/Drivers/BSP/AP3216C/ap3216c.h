#ifndef __DRV_AP3216C_H__
#define __DRV_AP3216C_H__

#include "stm32f1xx.h"
#include "./BSP/I2C/i2c.h"


#define AP3216C_ADDR 0x1e    /*0x3c=0x1e<<1*/ // AP3216C 的 7 位I2C地址


//System Register
#define AP3216C_SYS_CONFIGURATION_REG 0x00 //default
#define AP3216C_SYS_INT_STATUS_REG 0x01
#define AP3216C_SYS_INT_CLEAR_MANNER_REG 0x02
#define AP3216C_IR_DATA_L_REG 0x0A
#define AP3216C_IR_DATA_H_REG 0x0B
#define AP3216C_ALS_DATA_L_REG 0x0C
#define AP3216C_ALS_DATA_H_REG 0x0D
#define AP3216C_PS_DATA_L_REG 0x0E
#define AP3216C_PS_DATA_H_REG 0x0F

//ALS Register
#define AP3216C_ALS_CONFIGURATION_REG 0x10 //range 5:4,persist 3:0
#define AP3216C_ALS_CALIBRATION_REG 0x19
#define AP3216C_ALS_THRESHOLD_LOW_L_REG 0x1A  //bit 7:0
#define AP3216C_ALS_THRESHOLD_LOW_H_REG 0x1B  //bit 15:8
#define AP3216C_ALS_THRESHOLD_HIGH_L_REG 0x1C //bit 7:0
#define AP3216C_ALS_THRESHOLD_HIGH_H_REG 0x1D //bit 15:8

//PS Register
#define AP3216C_PS_CONFIGURATION_REG 0x20
#define AP3216C_PS_LED_DRIVER_REG 0x21
#define AP3216C_PS_INT_FORM_REG 0x22
#define AP3216C_PS_MEAN_TIME_REG 0x23
#define AP3216C_PS_LED_WAITING_TIME_REG 0x24
#define AP3216C_PS_CALIBRATION_L_REG 0x28
#define AP3216C_PS_CALIBRATION_H_REG 0x29
#define AP3216C_PS_THRESHOLD_LOW_L_REG 0x2A  //bit 1:0
#define AP3216C_PS_THRESHOLD_LOW_H_REG 0x2B  //bit 9:2
#define AP3216C_PS_THRESHOLD_HIGH_L_REG 0x2C //bit 1:0
#define AP3216C_PS_THRESHOLD_HIGH_H_REG 0x2D //bit 9:2


enum ap3216c_mode_value
{
    AP3216C_MODE_POWER_DOWN,      //Power down (Default)  // 断电模式
    AP3216C_MODE_ALS,             //ALS function active
    AP3216C_MODE_PS,              //PS+IR function active
    AP3216C_MODE_ALS_AND_PS,      //ALS and PS+IR functions active  // ALS 和 PS+IR 功能已激活
    AP3216C_MODE_SW_RESET,        //SW reset     // 复位模式
    AP3216C_MODE_ALS_ONCE,        //ALS function once
    AP3216C_MODE_PS_ONCE,         //PS+IR function once
    AP3216C_MODE_ALS_AND_PS_ONCE, //ALS and PS+IR functions once
};

enum ap3216c_int_clear_manner
{
    AP3216C_INT_CLEAR_MANNER_BY_READING,  //INT is automatically cleared by reading data registers(Default)
    AP3216C_ALS_CLEAR_MANNER_BY_SOFTWARE, //Software clear after writing 1 into address 0x01 each bit
};

enum als_range
{
    AP3216C_ALS_RANGE_20661, //Resolution = 0.35 lux/count(default).
    AP3216C_ALS_RANGE_5162,  //Resolution = 0.0788 lux/count.
    AP3216C_ALS_RANGE_1291,  //Resolution = 0.0197 lux/count.
    AP3216C_ALS_RANGE_323,   //Resolution = 0.0049 lux/count
};
typedef enum als_range als_range_t;

enum ps_gain
{
    AP3216C_PS_GAIN1, //detection distance *1.
    AP3216C_PS_GAIN2, //detection distance *2 (default).
    AP3216C_PS_GAIN4, //detection distance *4.
    AP3216C_PS_GAIN8, //detection distance *8.
};
typedef enum als_gain ps_gain_t;

// ap3216c_cmd ap3216c 命令
enum ap3216c_cmd
{
    AP3216C_SYSTEM_MODE,          //system  Configuration(Default : 000) 系统配置
    AP3216C_INT_PARAM,            //INT Clear Manner(Default : 0) 中断复位管理
    AP3216C_ALS_RANGE,            //ALS dynamic range(Default : 00) ALS(光强) 动态范围
    AP3216C_ALS_PERSIST,          //ALS persist(Default : 0000) ALS 存留
    AP3216C_ALS_CALIBRATION,      //ALS window loss calibration(Default : 0x40)ALS 窗口损失校准
    AP3216C_ALS_LOW_THRESHOLD_L,  //Lower byte of low interrupt threshold for ALS(Default : 0x00)  ALS 低中断阈值的低字节
    AP3216C_ALS_LOW_THRESHOLD_H,  //Higher byte of low interrupt threshold for ALS(Default : 0x00) ALS 低中断阈值的高字节
    AP3216C_ALS_HIGH_THRESHOLD_L, //Lower byte of high interrupt threshold for ALS (Default : 0xFF) ALS 高中断阈值的低字节
    AP3216C_ALS_HIGH_THRESHOLD_H, //Higher byte of high interrupt threshold for ALS(Default : 0xFF) ALS 高中断阈值的高字节
    AP3216C_PS_INTEGRATED_TIME,   //PS or IR Integrated time select(Default : 0000) PS 或 IR 集成时间选择
    AP3216C_PS_GAIN,              //PS gain (Default : 01) PS(接近)增益
    AP3216C_PS_PERSIST,           //Interrupt filter(Default : 01) 中断过滤器
    AP3216C_PS_LED_CONTROL,       //LED pulse(Default : 01)LED脉冲
    AP3216C_PS_LED_DRIVER_RATIO,  //LED driver ratio(Default : 11)LED驱动比
    AP3216C_PS_INT_MODE,          //PS INT Mode(Default : 0x01)PS 中断 模式
    AP3216C_PS_MEAN_TIME,         //PS mean time(Default : 0x00)PS平均时间
    AP3216C_PS_WAITING_TIME,      //PS LED Waiting(Default : 0x00)PS LED 等待
    AP3216C_PS_CALIBRATION_L,     //PS Calibration L(Default : 0x00)PS 低位校准
    AP3216C_PS_CALIBRATION_H,     //PS Calibration H(Default : 0x00)PS 高位校准 
    AP3216C_PS_LOW_THRESHOLD_L,   //PS Low Threshold L(Default :0x00)PS 低阈值 低位
    AP3216C_PS_LOW_THRESHOLD_H,   //PS Low Threshold H(Default :0x00)PS 低阈值 高位
    AP3216C_PS_HIGH_THRESHOLD_L,  //PS high Threshold L(Default :0xff)PS 高阈值 低位
    AP3216C_PS_HIGH_THRESHOLD_H,  //PS high Threshold H(Default :0xff)PS 高阈值 高位
};
typedef enum ap3216c_cmd ap3216c_cmd_t;

/* intrrupt parameters of ap3216c on ps or als */
/* ps 或 als 上 ap3216c 的中断参数 */
struct ap3216c_threshold
{
	  /* als 16 位，ps 10 位可用（低字节：0-1 位，高字节：8-15 位 */
    uint16_t min;        /* als 16 bits, ps 10 bits available(low byte ：0-1 bit and High Byte ：8-15 bit ) */
    uint16_t max;        /* als 16 bits, ps 10 bits available(low byte ：0-1 bit and High Byte ：8-15 bit ) */
    uint8_t noises_time; /* filter special noises trigger interrupt 过滤特殊噪音触发中断 */
};
typedef struct ap3216c_threshold ap3216c_threshold_t;


uint8_t ap3216c_get_IntStatus(void);
void ap3216c_init(void);
float ap3216c_read_ambient_light(void);
uint16_t ap3216c_read_ps_data(void);
uint16_t ap3216c_read_ir_data(void);
void ap3216c_set_param(ap3216c_cmd_t cmd, uint8_t value);
void ap3216c_get_param(ap3216c_cmd_t cmd, uint8_t *value);

#endif /*__DRV_AP3216C_H__ */
