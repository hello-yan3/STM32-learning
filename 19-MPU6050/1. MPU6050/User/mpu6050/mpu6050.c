#include "./mpu6050/mpu6050.h"
#include "./usart/bsp_debug_usart.h"
#include "./i2c/bsp_i2c.h"
#include "math.h"
#include "main.h"
#include "./kalman_filter/kalman.h"
#include "./Complementary_filtering/Complementary_filtering.h"

int16_t accx,accy,accz;	  	// 加速度传感器原始数据 
int16_t gyrox,gyroy,gyroz;	// 陀螺仪原始数据 
float temperature;	  		  // 陀螺仪温度数据
float Angle_X_Final; 		  	// 解算后俯仰角(Pitch)
float Angle_Y_Final; 		  	// 解算后横滚角(Roll)
float Angle_Z_Final; 		  	// 解算后偏航角(Yaw)
float GyroX,GyroY;				  // 解算后的最优角速度值

#define rad2deg     ((float)180/3.14)
#define deg2rad     ((float)3.14/180)

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
	e.p 内部时钟源 1kHz/(0x07+1) = 125Hz  （即采样率 125Hz）	
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
	return Re;
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
    MPU6050_ReadData(MPU6050_ACC_OUT, buf, 6);
    accData[0] = (buf[0] << 8) | buf[1];
    accData[1] = (buf[2] << 8) | buf[3];
    accData[2] = (buf[4] << 8) | buf[5];
}

/**
  * @brief   读取MPU6050的角速度数据
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


/**
  * @brief   MPU6050角度计算
  * @param   
  * @retval  
  */
void MPU6050_Angle_Calculate(void)
{
	// 如果使用DMP方法，该函数不执行
	if(WAY==1)  return;
	
	int16_t Acel[3];   // 加速度原始数据保存
	int16_t Gyro[3];   // 角速度原始数据保存
	float Accel_x,Accel_y,Accel_z;   // 加速度数据
	float Gyro_x, Gyro_y, Gyro_z ;   // 角速度数据
	float Angle_x_temp;  		//由加速度计算的x倾斜角度
	float Angle_y_temp;  		//由加速度计算的y倾斜角度
	
	MPU6050ReadAcc(Acel);   // 加速度原始数据获取
	MPU6050ReadGyro(Gyro);  // 角速度原始数据获取
	
	// 原始数据保存
	accx  = Acel[0];
	accy  = Acel[1];
	accz  = Acel[2];
	gyrox = Gyro[0];
	gyroy = Gyro[1];
	gyroz = Gyro[2];
	
	// 获得未滤波的 加速度/角速度 数据
	Accel_x = (float)accx/ACC_LSB; 
	Accel_y = (float)accy/ACC_LSB;
	Accel_z = (float)accz/ACC_LSB;
	Gyro_x = (float)gyrox/GYRO_LSB;
	Gyro_y = (float)gyroy/GYRO_LSB;
	Gyro_z = (float)gyroz/GYRO_LSB;
	
	//加速度反正切公式计算三个轴和水平面坐标系之间的夹角
	Angle_x_temp=(atan(Accel_y/Accel_z))*rad2deg;
	Angle_y_temp=(atan(Accel_x/Accel_z))*rad2deg;

	if(WAY==2)
	{
		Kalman_Filter_X(Angle_x_temp,Gyro_x);  //卡尔曼滤波计算X倾角
		Kalman_Filter_Y(Angle_y_temp,Gyro_y);  //卡尔曼滤波计算Y倾角			
	}
	else if(WAY==3)
	{
		Angle_X_Final = Complementary_Filter_x(Angle_x_temp,Gyro_x);    //一阶互补滤波计算X倾角
		Angle_Y_Final = Complementary_Filter_y(Angle_y_temp,Gyro_y);    //一阶互补滤波计算Y倾角	
	}
}
 
