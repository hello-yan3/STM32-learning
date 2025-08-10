#include "./mpu6050/mpu6050.h"
#include "./usart/bsp_debug_usart.h"
#include "./i2c/bsp_i2c.h"


#define MPU_ERROR 	I2C_ERROR
#define MPU_INFO 		I2C_INFO

/**
  * @brief   写数据到MPU6050寄存器
  * @param   reg_add:寄存器地址
	* @param	 reg_data:要写入的数据
  * @retval  
  */
void MPU6050_WriteReg(uint8_t reg_add,uint8_t reg_dat)
{
	Sensors_I2C_WriteRegister(MPU6050_ADDRESS, reg_add, 1, &reg_dat);
}

/**
  * @brief   从MPU6050寄存器读取数据
  * @param   reg_add:寄存器地址
	* @param	 Read：存储数据的缓冲区
	* @param	 num：要读取的数据量
  * @retval  
  */
void MPU6050_ReadData(uint8_t reg_add, uint8_t* Read, uint8_t num)
{
	Sensors_I2C_ReadRegister(MPU6050_ADDRESS, reg_add, num, Read);
}


/**
  * @brief   初始化MPU6050芯片
  * @param   
  * @retval  
  */
void MPU6050_Init(void)
{
	//在初始化之前要延时一段时间，若没有延时，则断电后再上电数据可能会出错
	HAL_Delay(100);
	/* 
	RA_PWR_MGMT_1 电源管理寄存器1 
	bit7: 该位置1, 设备复位，之后该位自动清零  
	bit6: 该位置1, 进入睡眠模式
	bit5: bit6置0, bit5置1, 进入循环模式, 在睡眠和唤醒间循环, 根据MPU6050_RA_PWR_MGMT_2进行循环间隔配置
	      bit6置0, bit5置0, 正常连续采集
  bit3: 该位置1, 温度传感器不工作
  bit2~bit0 选择时钟源，0x00表示内部时钟源，0x01~3表示陀螺仪时钟源(更精确)
	
	RA_PWR_MGMT_2 电源管理寄存器2 配置循环模式频率，不使用
	*/
	MPU6050_WriteReg(MPU6050_RA_PWR_MGMT_1, 0x01);	    // 0x18 0000 0001   
	/*
	SMPLRT_DIV 采样频率分频器寄存器
	采样频率=陀螺仪输出频率/（1+采样频率分频器寄存器值）
	使用滤波器，陀螺仪输出频率1kHz，不使用滤波器，陀螺仪输出频率8kHz
	*/
	MPU6050_WriteReg(MPU6050_RA_SMPLRT_DIV , 0x07);	    // 0x18 0000 0111 
	/*
	CONFIG 配置寄存器
	bit3~5 配置外部 Frame Synchronization (FSYNC)引脚采样 不使用, 置0
	bit0~2 配置低通滤波器值, 0x06表示110, 最大滤波
	*/
	MPU6050_WriteReg(MPU6050_RA_CONFIG , 0x06);	        // 0x18 0000 0110
	/*
	ACCEL_CONFIG 加速度配置寄存器
	bit3~4 配置加速度计量程范围 0x00<<3 2g; 0x01<<3 4g; 0x02<<3 8g; 0x03<<3 16g
	bit5~7 配置加速度自检, bit7置1, x自检; bit6置1, y自检; bit5置1, z自检; 
	*/
	MPU6050_WriteReg(MPU6050_RA_ACCEL_CONFIG , 0x18);	  // 0x18 0001 1000
	/*
	GYRO_CONFIG 陀螺仪配置寄存器
	bit3~4 配置陀螺仪量程范围 0x00<<3 250°/s; 0x01<<3 500°/s; 0x02<<3 1000°/s; 0x03<<3 2000°/s
	bit5~7 配置陀螺仪自检, bit7置1, x自检; bit6置1, y自检; bit5置1, z自检; 
	*/
	MPU6050_WriteReg(MPU6050_RA_GYRO_CONFIG, 0x18);     // 0x18 0001 1000
	HAL_Delay(200);
}

/**
  * @brief   读取MPU6050的ID
  * @param   
  * @retval  正常返回1，异常返回0
  */
uint8_t MPU6050ReadID(void)
{
	uint8_t Re = 0;
	// WHO_AM_I 寄存器 返回芯片ID
  MPU6050_ReadData(MPU6050_RA_WHO_AM_I,&Re,1);    //读器件地址
	if(Re != 0x68)
	{
		MPU_ERROR("MPU6050 dectected error!\r\n检测不到MPU6050模块，请检查模块与开发板的接线");
		return 0;
	}
	else
	{
		MPU_INFO("MPU6050 ID = %d\r\n",Re);
		return 1;
	}
}

/**
  * @brief   读取MPU6050的加速度数据
  * @param   accData: 读到的加速度数据
  * @retval  
  */
void MPU6050ReadAcc(int16_t *accData)
{
    uint8_t buf[6];
	  // ACC_OUT 加速度计测量值寄存器
		// PS：这里可以连续读6位吗？
    MPU6050_ReadData(MPU6050_ACC_OUT, buf, 6);
    accData[0] = (buf[0] << 8) | buf[1];
    accData[1] = (buf[2] << 8) | buf[3];
    accData[2] = (buf[4] << 8) | buf[5];
}

/**
  * @brief   读取MPU6050的角加速度数据
  * @param   accData: 读到的角速度数据   
  * @retval  
  */
void MPU6050ReadGyro(int16_t *gyroData)
{
    uint8_t buf[6];
		// GYRO_OUT 陀螺仪测量值寄存器
		// PS：这里可以连续读6位吗？
    MPU6050_ReadData(MPU6050_GYRO_OUT,buf,6);
    gyroData[0] = (buf[0] << 8) | buf[1];
    gyroData[1] = (buf[2] << 8) | buf[3];
    gyroData[2] = (buf[4] << 8) | buf[5];
}

/**
  * @brief   读取MPU6050的原始温度数据
  * @param   
  * @retval  
  */
void MPU6050ReadTemp(int16_t *tempData)
{
	uint8_t buf[2];
  // TEMP_OUT 温度测量值寄存器
	MPU6050_ReadData(MPU6050_RA_TEMP_OUT_H,buf,2);     //读取温度值
  *tempData = (buf[0] << 8) | buf[1];
}

/**
  * @brief   读取MPU6050的温度数据，转化成摄氏度
  * @param   
  * @retval  
  */
void MPU6050_ReturnTemp(float *Temperature)
{
	short temp3;
	uint8_t buf[2];
	
	MPU6050_ReadData(MPU6050_RA_TEMP_OUT_H,buf,2);     //读取温度值
  temp3= (buf[0] << 8) | buf[1];	
	*Temperature=((double) temp3/340.0)+36.53;

}
