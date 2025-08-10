#include "./mpu6050/mpu6050.h"
#include "./usart/bsp_debug_usart.h"
#include "./i2c/bsp_i2c.h"


#define MPU_ERROR 	I2C_ERROR
#define MPU_INFO 		I2C_INFO

/**
  * @brief   д���ݵ�MPU6050�Ĵ���
  * @param   reg_add:�Ĵ�����ַ
	* @param	 reg_data:Ҫд�������
  * @retval  
  */
void MPU6050_WriteReg(uint8_t reg_add,uint8_t reg_dat)
{
	Sensors_I2C_WriteRegister(MPU6050_ADDRESS, reg_add, 1, &reg_dat);
}

/**
  * @brief   ��MPU6050�Ĵ�����ȡ����
  * @param   reg_add:�Ĵ�����ַ
	* @param	 Read���洢���ݵĻ�����
	* @param	 num��Ҫ��ȡ��������
  * @retval  
  */
void MPU6050_ReadData(uint8_t reg_add, uint8_t* Read, uint8_t num)
{
	Sensors_I2C_ReadRegister(MPU6050_ADDRESS, reg_add, num, Read);
}


/**
  * @brief   ��ʼ��MPU6050оƬ
  * @param   
  * @retval  
  */
void MPU6050_Init(void)
{
	//�ڳ�ʼ��֮ǰҪ��ʱһ��ʱ�䣬��û����ʱ����ϵ�����ϵ����ݿ��ܻ����
	HAL_Delay(100);
	/* 
	RA_PWR_MGMT_1 ��Դ����Ĵ���1 
	bit7: ��λ��1, �豸��λ��֮���λ�Զ�����  
	bit6: ��λ��1, ����˯��ģʽ
	bit5: bit6��0, bit5��1, ����ѭ��ģʽ, ��˯�ߺͻ��Ѽ�ѭ��, ����MPU6050_RA_PWR_MGMT_2����ѭ���������
	      bit6��0, bit5��0, ���������ɼ�
  bit3: ��λ��1, �¶ȴ�����������
  bit2~bit0 ѡ��ʱ��Դ��0x00��ʾ�ڲ�ʱ��Դ��0x01~3��ʾ������ʱ��Դ(����ȷ)
	
	RA_PWR_MGMT_2 ��Դ����Ĵ���2 ����ѭ��ģʽƵ�ʣ���ʹ��
	*/
	MPU6050_WriteReg(MPU6050_RA_PWR_MGMT_1, 0x01);	    // 0x18 0000 0001   
	/*
	SMPLRT_DIV ����Ƶ�ʷ�Ƶ���Ĵ���
	����Ƶ��=���������Ƶ��/��1+����Ƶ�ʷ�Ƶ���Ĵ���ֵ��
	ʹ���˲��������������Ƶ��1kHz����ʹ���˲��������������Ƶ��8kHz
	*/
	MPU6050_WriteReg(MPU6050_RA_SMPLRT_DIV , 0x07);	    // 0x18 0000 0111 
	/*
	CONFIG ���üĴ���
	bit3~5 �����ⲿ Frame Synchronization (FSYNC)���Ų��� ��ʹ��, ��0
	bit0~2 ���õ�ͨ�˲���ֵ, 0x06��ʾ110, ����˲�
	*/
	MPU6050_WriteReg(MPU6050_RA_CONFIG , 0x06);	        // 0x18 0000 0110
	/*
	ACCEL_CONFIG ���ٶ����üĴ���
	bit3~4 ���ü��ٶȼ����̷�Χ 0x00<<3 2g; 0x01<<3 4g; 0x02<<3 8g; 0x03<<3 16g
	bit5~7 ���ü��ٶ��Լ�, bit7��1, x�Լ�; bit6��1, y�Լ�; bit5��1, z�Լ�; 
	*/
	MPU6050_WriteReg(MPU6050_RA_ACCEL_CONFIG , 0x18);	  // 0x18 0001 1000
	/*
	GYRO_CONFIG ���������üĴ���
	bit3~4 �������������̷�Χ 0x00<<3 250��/s; 0x01<<3 500��/s; 0x02<<3 1000��/s; 0x03<<3 2000��/s
	bit5~7 �����������Լ�, bit7��1, x�Լ�; bit6��1, y�Լ�; bit5��1, z�Լ�; 
	*/
	MPU6050_WriteReg(MPU6050_RA_GYRO_CONFIG, 0x18);     // 0x18 0001 1000
	HAL_Delay(200);
}

/**
  * @brief   ��ȡMPU6050��ID
  * @param   
  * @retval  ��������1���쳣����0
  */
uint8_t MPU6050ReadID(void)
{
	uint8_t Re = 0;
	// WHO_AM_I �Ĵ��� ����оƬID
  MPU6050_ReadData(MPU6050_RA_WHO_AM_I,&Re,1);    //��������ַ
	if(Re != 0x68)
	{
		MPU_ERROR("MPU6050 dectected error!\r\n��ⲻ��MPU6050ģ�飬����ģ���뿪����Ľ���");
		return 0;
	}
	else
	{
		MPU_INFO("MPU6050 ID = %d\r\n",Re);
		return 1;
	}
}

/**
  * @brief   ��ȡMPU6050�ļ��ٶ�����
  * @param   accData: �����ļ��ٶ�����
  * @retval  
  */
void MPU6050ReadAcc(int16_t *accData)
{
    uint8_t buf[6];
	  // ACC_OUT ���ٶȼƲ���ֵ�Ĵ���
		// PS���������������6λ��
    MPU6050_ReadData(MPU6050_ACC_OUT, buf, 6);
    accData[0] = (buf[0] << 8) | buf[1];
    accData[1] = (buf[2] << 8) | buf[3];
    accData[2] = (buf[4] << 8) | buf[5];
}

/**
  * @brief   ��ȡMPU6050�ĽǼ��ٶ�����
  * @param   accData: �����Ľ��ٶ�����   
  * @retval  
  */
void MPU6050ReadGyro(int16_t *gyroData)
{
    uint8_t buf[6];
		// GYRO_OUT �����ǲ���ֵ�Ĵ���
		// PS���������������6λ��
    MPU6050_ReadData(MPU6050_GYRO_OUT,buf,6);
    gyroData[0] = (buf[0] << 8) | buf[1];
    gyroData[1] = (buf[2] << 8) | buf[3];
    gyroData[2] = (buf[4] << 8) | buf[5];
}

/**
  * @brief   ��ȡMPU6050��ԭʼ�¶�����
  * @param   
  * @retval  
  */
void MPU6050ReadTemp(int16_t *tempData)
{
	uint8_t buf[2];
  // TEMP_OUT �¶Ȳ���ֵ�Ĵ���
	MPU6050_ReadData(MPU6050_RA_TEMP_OUT_H,buf,2);     //��ȡ�¶�ֵ
  *tempData = (buf[0] << 8) | buf[1];
}

/**
  * @brief   ��ȡMPU6050���¶����ݣ�ת�������϶�
  * @param   
  * @retval  
  */
void MPU6050_ReturnTemp(float *Temperature)
{
	short temp3;
	uint8_t buf[2];
	
	MPU6050_ReadData(MPU6050_RA_TEMP_OUT_H,buf,2);     //��ȡ�¶�ֵ
  temp3= (buf[0] << 8) | buf[1];	
	*Temperature=((double) temp3/340.0)+36.53;

}
