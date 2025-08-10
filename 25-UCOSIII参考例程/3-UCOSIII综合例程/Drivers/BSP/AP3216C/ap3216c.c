/**
  ******************************************************************************
  * @file    ap3216.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   AP3216C 驱动文件
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 STM32 FLASH 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */

#include "./BSP/AP3216C/ap3216c.h"
 

/* 写寄存器的值 */
static void write_reg(uint8_t reg, uint8_t data)
{
  
  HAL_I2C_Mem_Write(&I2C_Handle, AP3216C_ADDR << 1, reg, I2C_MEMADD_SIZE_8BIT, &data, 1, 100);
  
}

/* 读寄存器的值 */
static void read_regs(uint8_t reg, uint8_t len, uint8_t *buf)
{
  
  HAL_I2C_Mem_Read(&I2C_Handle, AP3216C_ADDR << 1, reg, I2C_MEMADD_SIZE_8BIT, buf, len, 100);
  
}


/**
 * This function is convenient to getting data except including high and low data for this sensor.
 * note:after reading lower register first,reading higher add one.
 * 该功能除了包含该传感器的高低数据外，还可以方便地获取数据。
 * 注：先读低位寄存器后，读高位寄存器加一。
 */
static uint32_t read_low_and_high(uint8_t reg, uint8_t len)
{
    uint32_t data;
    uint8_t buf = 0;

    read_regs( reg, len, &buf);        // 读低字节
    data = buf;
    read_regs( reg + 1, len, &buf);    // 读高字节
    data = data + (buf << len * 8);    // 合并数据

    return data;
}


/* 函数配合ap3216c_cmd结构体阅读 */
/**
 * This function sets parameter of ap3216c sensor
 * 该函数设置ap3216c传感器的参数
 * @param cmd the parameter cmd of device   命令设备参数cmd
 * @param value for setting value in cmd register value  命令寄存器值中设置值的值
 *
 */
void ap3216c_set_param(ap3216c_cmd_t cmd, uint8_t value)
{
	  // 根据不同的系统命令，写入不同的寄存器
    switch (cmd)    // ap3216c_cmd_t命令
    {
      case AP3216C_SYSTEM_MODE:  
      {
          /* default 000,power down */
          write_reg(AP3216C_SYS_CONFIGURATION_REG, value);

          break;
      }
      case AP3216C_INT_PARAM:
      {
          write_reg(AP3216C_SYS_INT_CLEAR_MANNER_REG, value);

          break;
      }

      case AP3216C_ALS_RANGE:
      {
          uint8_t args;
        
          read_regs( AP3216C_ALS_CONFIGURATION_REG, 1, &args);
          args &= 0xcf;       // 1100 1111
          args |= value << 4; // 1111 0000
          write_reg( AP3216C_ALS_CONFIGURATION_REG, args);

          break;
      }
      case AP3216C_ALS_PERSIST:
      {
          uint8_t args = 0;

          read_regs(AP3216C_ALS_CONFIGURATION_REG, 1, &args);
          args &= 0xf0;       // 1111 0000
          args |= value;
          write_reg(AP3216C_ALS_CONFIGURATION_REG, args);

          break;
      }
      case AP3216C_ALS_LOW_THRESHOLD_L:
      {
          write_reg(AP3216C_ALS_THRESHOLD_LOW_L_REG, value);

          break;
      }
      case AP3216C_ALS_LOW_THRESHOLD_H:
      {
          write_reg(AP3216C_ALS_THRESHOLD_LOW_H_REG, value);

          break;
      }
      case AP3216C_ALS_HIGH_THRESHOLD_L:
      {
          write_reg(AP3216C_ALS_THRESHOLD_HIGH_L_REG, value);

          break;
      }
      case AP3216C_ALS_HIGH_THRESHOLD_H:
      {
          write_reg(AP3216C_ALS_THRESHOLD_HIGH_H_REG, value);

          break;
      }
      case AP3216C_PS_GAIN:
      {
          uint8_t args = 0;

          read_regs(AP3216C_PS_CONFIGURATION_REG, 1, &args);
          args &= 0xf3;    // 1111 0011
          args |= value;
          write_reg(AP3216C_PS_CONFIGURATION_REG, args);

          break;
      }
      case AP3216C_PS_PERSIST:
      {
          uint8_t args = 0;

          read_regs(AP3216C_PS_CONFIGURATION_REG, 1, &args);
          args &= 0xfc;    // 1111 1100
          args |= value;
          write_reg(AP3216C_PS_CONFIGURATION_REG, args);

          break;
      }
      case AP3216C_PS_LOW_THRESHOLD_L:
      {
          write_reg( AP3216C_PS_THRESHOLD_LOW_L_REG, value);

          break;
      }
      case AP3216C_PS_LOW_THRESHOLD_H:
      {
          write_reg( AP3216C_PS_THRESHOLD_LOW_H_REG, value);

          break;
      }
      case AP3216C_PS_HIGH_THRESHOLD_L:
      {
          write_reg( AP3216C_PS_THRESHOLD_HIGH_L_REG, value);

          break;
      }
      case AP3216C_PS_HIGH_THRESHOLD_H:
      {
          write_reg( AP3216C_PS_THRESHOLD_HIGH_H_REG, value);

          break;
      }

    default:
    {
    
    }
    }
}

/**
 * This function gets parameter of ap3216c sensor
 * 该函数获取ap3216c传感器的参数
 * @param cmd the parameter cmd of device  命令设备参数cmd
 * @param value to get value in cmd register  命令寄存器值中设置值的值
 *
 */
void ap3216c_get_param(ap3216c_cmd_t cmd, uint8_t *value)
{
    switch (cmd)
    {
    case AP3216C_SYSTEM_MODE:
    {
        read_regs( AP3216C_SYS_CONFIGURATION_REG, 1, value);

        break;
    }
    case AP3216C_INT_PARAM:
    {
        read_regs( AP3216C_SYS_INT_CLEAR_MANNER_REG, 1, value);

        break;
    }
    case AP3216C_ALS_RANGE:
    {
        uint8_t temp;

        read_regs( AP3216C_ALS_CONFIGURATION_REG, 1, value);
        temp = (*value & 0xff) >> 4;   // 1111 1111 (取高4位)

        *value = temp;

        break;
    }
    case AP3216C_ALS_PERSIST:
    {
        uint8_t temp;

        read_regs( AP3216C_ALS_CONFIGURATION_REG, 1, value);
        temp = *value & 0x0f;   // 0000 1111 (取低4位)

        *value = temp;

        break;
    }
    case AP3216C_ALS_LOW_THRESHOLD_L:
    {
        read_regs( AP3216C_ALS_THRESHOLD_LOW_L_REG, 1, value);

        break;
    }
    case AP3216C_ALS_LOW_THRESHOLD_H:
    {
        read_regs( AP3216C_ALS_THRESHOLD_LOW_H_REG, 1, value);

        break;
    }
    case AP3216C_ALS_HIGH_THRESHOLD_L:
    {
        read_regs( AP3216C_ALS_THRESHOLD_HIGH_L_REG, 1, value);

        break;
    }
    case AP3216C_ALS_HIGH_THRESHOLD_H:
    {
        read_regs( AP3216C_ALS_THRESHOLD_HIGH_H_REG, 1, value);

        break;
    }
    case AP3216C_PS_GAIN:
    {
        uint8_t temp;

        read_regs( AP3216C_PS_CONFIGURATION_REG, 1, &temp);

        *value = (temp & 0xc) >> 2;   // 1100 

        break;
    }
    case AP3216C_PS_PERSIST:
    {
        uint8_t temp;

        read_regs( AP3216C_PS_CONFIGURATION_REG, 1, &temp);

        *value = temp & 0x3;          // 0011

        break;
    }
    case AP3216C_PS_LOW_THRESHOLD_L:
    {
        read_regs( AP3216C_PS_THRESHOLD_LOW_L_REG, 1, value);

        break;
    }
    case AP3216C_PS_LOW_THRESHOLD_H:
    {
        read_regs( AP3216C_PS_THRESHOLD_LOW_H_REG, 1, value);
        break;
    }
    case AP3216C_PS_HIGH_THRESHOLD_L:
    {
        read_regs( AP3216C_PS_THRESHOLD_HIGH_L_REG, 1, value);

        break;
    }
    case AP3216C_PS_HIGH_THRESHOLD_H:
    {
        read_regs( AP3216C_PS_THRESHOLD_HIGH_H_REG, 1, value);

        break;
    }

    default:
    {
         
    }
    } 
}



/**
 * This function is only used to set threshold without filtering times
 * 该功能仅用于设置阈值，不用于过滤次数
 *
 * @param cmd first register , and other cmd count by it.
 * 命令第一个寄存器，其他命令计数由它决定
 * @param threshold threshold and filtering times of als threshold
 * 阈值 als 阈值 阈值 和 过滤次数 阈值
 */
static void set_als_threshold(ap3216c_cmd_t cmd, ap3216c_threshold_t threshold)
{
    uint8_t Resolution;
    double DB;

    /* 读光照强度的范围 */
    ap3216c_get_param(AP3216C_ALS_RANGE, &Resolution);

    if (Resolution == AP3216C_ALS_RANGE_20661)     // 光照强度范围 0 - 20661
    {
        DB = 0.35;    // 光照强度的分辨率
    }
    else if (Resolution == AP3216C_ALS_RANGE_5162)     // 光照强度范围 0 - 5162
    {
        DB = 0.0788;    // 光照强度的分辨率
    }
    else if (Resolution == AP3216C_ALS_RANGE_1291)     // 光照强度范围 0 - 1291
    {
        DB = 0.0197;    // 光照强度的分辨率
    }
    else if (Resolution == AP3216C_ALS_RANGE_323)     // 光照强度范围 0 - 323
    {
        DB = 0.0049;    // 光照强度的分辨率
    }
  
    threshold.min /= DB;    // 根据不同的分辨率来设置
    threshold.max /= DB;
      
    ap3216c_set_param(cmd, (threshold.min & 0xff));
    ap3216c_set_param((ap3216c_cmd_t)(cmd + 1), (threshold.min >> 8));
    ap3216c_set_param((ap3216c_cmd_t)(cmd + 2), (threshold.max & 0xff));
    ap3216c_set_param((ap3216c_cmd_t)(cmd + 3), threshold.max >> 8);
}

static void set_ps_threshold(ap3216c_cmd_t cmd, ap3216c_threshold_t threshold)
{
    if (threshold.min > 1020)    // 大于1020 时需要设置低字节的低两位
      ap3216c_set_param(cmd, (threshold.min - 1020 & 0x03));
    
    ap3216c_set_param((ap3216c_cmd_t)(cmd + 1), threshold.min/4);    // 设置高字节参数
    
    if (threshold.max > 1020)    // 大于1020 时需要设置低字节的低两位
      ap3216c_set_param((ap3216c_cmd_t)(cmd + 2), (threshold.max - 1020 & 0x03));
    
    ap3216c_set_param((ap3216c_cmd_t)(cmd + 3), threshold.max/4);    // 设置高字节参数
}

/* 中断配置及初始化 */
static void ap3216c_int_init(void)
{ 
    ap3216c_threshold_t threshold;    // 设置报警值阈值结构体
  
    threshold.min = 100;     // 光照下限产生报警
    threshold.max = 1000;    // 光照上限产生报警
    threshold.noises_time = 1;    // ALS 达到阈值后持续 threshold.noises_time * 4 个周期后开始报警， threshold.noises_time 最大为； 15
    set_als_threshold(AP3216C_ALS_LOW_THRESHOLD_L, threshold);    // 设置 ALS 的报警值
  
  
    threshold.min = 120;    // PS 低于 200 产生报警
    threshold.max = 600;    // PS 高于 500 产生报警
    set_ps_threshold(AP3216C_PS_LOW_THRESHOLD_L, threshold);     // 设置 PS 的报警值
}

/**
 * This function initializes ap3216c registered device driver
 * 该函数初始化ap3216c注册的设备驱动程序
 * @param no
 * @return the ap3216c device.
 */
void ap3216c_init(void)
{
    I2C_Mode_Config();

    /* reset ap3216c */
    ap3216c_set_param(AP3216C_SYSTEM_MODE, AP3216C_MODE_SW_RESET);  // 断电复位模式
    HAL_Delay(100);
    ap3216c_set_param(AP3216C_SYSTEM_MODE, AP3216C_MODE_ALS_AND_PS);  // ALS 和 PS+IR 功能已激活
    HAL_Delay(100); // delay at least  100ms

    /* 配置中断脚 和 中断数据	*/
    AP_INT_Config();
    ap3216c_int_init();
}



/**
 * This function reads status register by ap3216c sensor measurement
 * 该函数通过ap3216c传感器测量读取状态寄存器
 * @param no
 * @return status register value.
 * 状态寄存器值。
 */

uint8_t ap3216c_get_IntStatus(void)
{
    uint8_t IntStatus;
    
    /* 读中断状态寄存器 */
    read_regs(AP3216C_SYS_INT_STATUS_REG, 1, &IntStatus);
    // IntStatus 第 0 位表示 ALS 中断，第 1 位表示 PS 中断。
    
    return IntStatus;    // 返回状态
}

/**
 * This function reads light by ap3216c sensor measurement
 * 该功能通过ap3216c传感器测量读取光线
 * @param no
 * @return the ambient light converted to float data.
 * 环境光转换为浮点数据。
 */
float ap3216c_read_ambient_light(void)
{
    float brightness = 0.0; // default error data
    uint32_t read_data;
    uint8_t temp;

    read_data = read_low_and_high(AP3216C_ALS_DATA_L_REG, 1);

    ap3216c_get_param(AP3216C_ALS_RANGE, &temp);
  
    if (temp == AP3216C_ALS_RANGE_20661)
    {
        brightness = 0.35 * read_data; //sensor ambient light converse to reality 
    }
    else if (temp == AP3216C_ALS_RANGE_5162)
    {
        brightness = 0.0788 * read_data; //sensor ambient light converse to reality
    }
    else if (temp == AP3216C_ALS_RANGE_1291)
    {
        brightness = 0.0197 * read_data; //sensor ambient light converse to reality
    }
    else if (temp == AP3216C_ALS_RANGE_323)
    {
        brightness = 0.0049 * read_data; //sensor ambient light converse to reality
    }

    return brightness;
}

/**
 * This function reads proximity by ap3216c sensor measurement
 * 此功能通过 ap3216c 传感器测量读取接近度
 * @param no
 * @return the proximity data.
 * 接近数据。
 */
uint16_t ap3216c_read_ps_data(void)
{
    uint16_t proximity = 0;

    uint32_t read_data;
    read_data = read_low_and_high(AP3216C_PS_DATA_L_REG, 1); //read two data

		// 如果IR_OF红外溢出标志被置 1，则表明 PS 数据在高强度红外光下无效。
    if (1 == ((read_data >> 6) & 0x01 || (read_data >> 14) & 0x01))
    {
       return proximity = 55555;    // 红外过高（IR），PS无效 返回一个 55555 的无效数据
    }
		// 低字节寄存器低4位 + 高字节寄存器低6位
    proximity = (read_data & 0x000f) + (((read_data >> 8) & 0x3f) << 4); //sensor proximity converse to reality

    proximity |= read_data & 0x8000;    // 取最高位，0 表示物体远离，1 表示物体靠近
    
    return proximity;    // proximity 后十位是数据位，最高位为状态位
}

/**
 * This function reads ir by ap3216c sensor measurement
 * 该功能通过 ap3216c 传感器测量读取 IR 值。
 * @param no
 * @return the ir data.
 * ir数据。
 */
uint16_t ap3216c_read_ir_data(void)
{
    uint16_t proximity = 0;

    uint32_t read_data;
    read_data = read_low_and_high(AP3216C_IR_DATA_L_REG, 1); //read two data
		// 低字节寄存器低2位 + 高字节寄存器
    proximity = (read_data & 0x0003) + (((read_data >> 8) & 0xFF)<<2) ; //sensor proximity converse to reality

    return proximity;
}
