/**
  ******************************************************************************
  * @file    ap3216.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   AP3216C �����ļ�
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� STM32 FLASH ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */

#include "./BSP/AP3216C/ap3216c.h"
 

/* д�Ĵ�����ֵ */
static void write_reg(uint8_t reg, uint8_t data)
{
  
  HAL_I2C_Mem_Write(&I2C_Handle, AP3216C_ADDR << 1, reg, I2C_MEMADD_SIZE_8BIT, &data, 1, 100);
  
}

/* ���Ĵ�����ֵ */
static void read_regs(uint8_t reg, uint8_t len, uint8_t *buf)
{
  
  HAL_I2C_Mem_Read(&I2C_Handle, AP3216C_ADDR << 1, reg, I2C_MEMADD_SIZE_8BIT, buf, len, 100);
  
}


/**
 * This function is convenient to getting data except including high and low data for this sensor.
 * note:after reading lower register first,reading higher add one.
 * �ù��ܳ��˰����ô������ĸߵ������⣬�����Է���ػ�ȡ���ݡ�
 * ע���ȶ���λ�Ĵ����󣬶���λ�Ĵ�����һ��
 */
static uint32_t read_low_and_high(uint8_t reg, uint8_t len)
{
    uint32_t data;
    uint8_t buf = 0;

    read_regs( reg, len, &buf);        // �����ֽ�
    data = buf;
    read_regs( reg + 1, len, &buf);    // �����ֽ�
    data = data + (buf << len * 8);    // �ϲ�����

    return data;
}


/* �������ap3216c_cmd�ṹ���Ķ� */
/**
 * This function sets parameter of ap3216c sensor
 * �ú�������ap3216c�������Ĳ���
 * @param cmd the parameter cmd of device   �����豸����cmd
 * @param value for setting value in cmd register value  ����Ĵ���ֵ������ֵ��ֵ
 *
 */
void ap3216c_set_param(ap3216c_cmd_t cmd, uint8_t value)
{
	  // ���ݲ�ͬ��ϵͳ���д�벻ͬ�ļĴ���
    switch (cmd)    // ap3216c_cmd_t����
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
 * �ú�����ȡap3216c�������Ĳ���
 * @param cmd the parameter cmd of device  �����豸����cmd
 * @param value to get value in cmd register  ����Ĵ���ֵ������ֵ��ֵ
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
        temp = (*value & 0xff) >> 4;   // 1111 1111 (ȡ��4λ)

        *value = temp;

        break;
    }
    case AP3216C_ALS_PERSIST:
    {
        uint8_t temp;

        read_regs( AP3216C_ALS_CONFIGURATION_REG, 1, value);
        temp = *value & 0x0f;   // 0000 1111 (ȡ��4λ)

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
 * �ù��ܽ�����������ֵ�������ڹ��˴���
 *
 * @param cmd first register , and other cmd count by it.
 * �����һ���Ĵ������������������������
 * @param threshold threshold and filtering times of als threshold
 * ��ֵ als ��ֵ ��ֵ �� ���˴��� ��ֵ
 */
static void set_als_threshold(ap3216c_cmd_t cmd, ap3216c_threshold_t threshold)
{
    uint8_t Resolution;
    double DB;

    /* ������ǿ�ȵķ�Χ */
    ap3216c_get_param(AP3216C_ALS_RANGE, &Resolution);

    if (Resolution == AP3216C_ALS_RANGE_20661)     // ����ǿ�ȷ�Χ 0 - 20661
    {
        DB = 0.35;    // ����ǿ�ȵķֱ���
    }
    else if (Resolution == AP3216C_ALS_RANGE_5162)     // ����ǿ�ȷ�Χ 0 - 5162
    {
        DB = 0.0788;    // ����ǿ�ȵķֱ���
    }
    else if (Resolution == AP3216C_ALS_RANGE_1291)     // ����ǿ�ȷ�Χ 0 - 1291
    {
        DB = 0.0197;    // ����ǿ�ȵķֱ���
    }
    else if (Resolution == AP3216C_ALS_RANGE_323)     // ����ǿ�ȷ�Χ 0 - 323
    {
        DB = 0.0049;    // ����ǿ�ȵķֱ���
    }
  
    threshold.min /= DB;    // ���ݲ�ͬ�ķֱ���������
    threshold.max /= DB;
      
    ap3216c_set_param(cmd, (threshold.min & 0xff));
    ap3216c_set_param((ap3216c_cmd_t)(cmd + 1), (threshold.min >> 8));
    ap3216c_set_param((ap3216c_cmd_t)(cmd + 2), (threshold.max & 0xff));
    ap3216c_set_param((ap3216c_cmd_t)(cmd + 3), threshold.max >> 8);
}

static void set_ps_threshold(ap3216c_cmd_t cmd, ap3216c_threshold_t threshold)
{
    if (threshold.min > 1020)    // ����1020 ʱ��Ҫ���õ��ֽڵĵ���λ
      ap3216c_set_param(cmd, (threshold.min - 1020 & 0x03));
    
    ap3216c_set_param((ap3216c_cmd_t)(cmd + 1), threshold.min/4);    // ���ø��ֽڲ���
    
    if (threshold.max > 1020)    // ����1020 ʱ��Ҫ���õ��ֽڵĵ���λ
      ap3216c_set_param((ap3216c_cmd_t)(cmd + 2), (threshold.max - 1020 & 0x03));
    
    ap3216c_set_param((ap3216c_cmd_t)(cmd + 3), threshold.max/4);    // ���ø��ֽڲ���
}

/* �ж����ü���ʼ�� */
static void ap3216c_int_init(void)
{ 
    ap3216c_threshold_t threshold;    // ���ñ���ֵ��ֵ�ṹ��
  
    threshold.min = 100;     // �������޲�������
    threshold.max = 1000;    // �������޲�������
    threshold.noises_time = 1;    // ALS �ﵽ��ֵ����� threshold.noises_time * 4 �����ں�ʼ������ threshold.noises_time ���Ϊ�� 15
    set_als_threshold(AP3216C_ALS_LOW_THRESHOLD_L, threshold);    // ���� ALS �ı���ֵ
  
  
    threshold.min = 120;    // PS ���� 200 ��������
    threshold.max = 600;    // PS ���� 500 ��������
    set_ps_threshold(AP3216C_PS_LOW_THRESHOLD_L, threshold);     // ���� PS �ı���ֵ
}

/**
 * This function initializes ap3216c registered device driver
 * �ú�����ʼ��ap3216cע����豸��������
 * @param no
 * @return the ap3216c device.
 */
void ap3216c_init(void)
{
    I2C_Mode_Config();

    /* reset ap3216c */
    ap3216c_set_param(AP3216C_SYSTEM_MODE, AP3216C_MODE_SW_RESET);  // �ϵ縴λģʽ
    HAL_Delay(100);
    ap3216c_set_param(AP3216C_SYSTEM_MODE, AP3216C_MODE_ALS_AND_PS);  // ALS �� PS+IR �����Ѽ���
    HAL_Delay(100); // delay at least  100ms

    /* �����жϽ� �� �ж�����	*/
    AP_INT_Config();
    ap3216c_int_init();
}



/**
 * This function reads status register by ap3216c sensor measurement
 * �ú���ͨ��ap3216c������������ȡ״̬�Ĵ���
 * @param no
 * @return status register value.
 * ״̬�Ĵ���ֵ��
 */

uint8_t ap3216c_get_IntStatus(void)
{
    uint8_t IntStatus;
    
    /* ���ж�״̬�Ĵ��� */
    read_regs(AP3216C_SYS_INT_STATUS_REG, 1, &IntStatus);
    // IntStatus �� 0 λ��ʾ ALS �жϣ��� 1 λ��ʾ PS �жϡ�
    
    return IntStatus;    // ����״̬
}

/**
 * This function reads light by ap3216c sensor measurement
 * �ù���ͨ��ap3216c������������ȡ����
 * @param no
 * @return the ambient light converted to float data.
 * ������ת��Ϊ�������ݡ�
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
 * �˹���ͨ�� ap3216c ������������ȡ�ӽ���
 * @param no
 * @return the proximity data.
 * �ӽ����ݡ�
 */
uint16_t ap3216c_read_ps_data(void)
{
    uint16_t proximity = 0;

    uint32_t read_data;
    read_data = read_low_and_high(AP3216C_PS_DATA_L_REG, 1); //read two data

		// ���IR_OF���������־���� 1������� PS �����ڸ�ǿ�Ⱥ��������Ч��
    if (1 == ((read_data >> 6) & 0x01 || (read_data >> 14) & 0x01))
    {
       return proximity = 55555;    // ������ߣ�IR����PS��Ч ����һ�� 55555 ����Ч����
    }
		// ���ֽڼĴ�����4λ + ���ֽڼĴ�����6λ
    proximity = (read_data & 0x000f) + (((read_data >> 8) & 0x3f) << 4); //sensor proximity converse to reality

    proximity |= read_data & 0x8000;    // ȡ���λ��0 ��ʾ����Զ�룬1 ��ʾ���忿��
    
    return proximity;    // proximity ��ʮλ������λ�����λΪ״̬λ
}

/**
 * This function reads ir by ap3216c sensor measurement
 * �ù���ͨ�� ap3216c ������������ȡ IR ֵ��
 * @param no
 * @return the ir data.
 * ir���ݡ�
 */
uint16_t ap3216c_read_ir_data(void)
{
    uint16_t proximity = 0;

    uint32_t read_data;
    read_data = read_low_and_high(AP3216C_IR_DATA_L_REG, 1); //read two data
		// ���ֽڼĴ�����2λ + ���ֽڼĴ���
    proximity = (read_data & 0x0003) + (((read_data >> 8) & 0xFF)<<2) ; //sensor proximity converse to reality

    return proximity;
}
