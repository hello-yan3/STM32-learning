#include "MPU6050DMP.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include <math.h>
#include "./usart/bsp_debug_usart.h"
#include "./mpu6050/mpu6050.h"

// 读取到的四元数数据为30位整数，需要将整数转换为小数
#define Q30				(float)(2^30)  

// 角度弧度转换	
#define rad2deg     (180.0/3.14)
#define deg2rad     (3.14/180.0)

#define DEFAULT_MPU_HZ  (200)      // 设置采样率（其实就是修改INT引脚脉冲频率）
// 采样周期 = 1/DEFAULT_MPU_HZ = 5ms

// 传感器方向矩阵
static signed char gyro_orientation[9] = {-1, 0, 0,
                                           0,-1, 0,
                                           0, 0, 1};

static unsigned short inv_row_2_scale(const signed char *row)
{
    unsigned short b;

    if (row[0] > 0)
        b = 0;
    else if (row[0] < 0)
        b = 4;
    else if (row[1] > 0)
        b = 1;
    else if (row[1] < 0)
        b = 5;
    else if (row[2] > 0)
        b = 2;
    else if (row[2] < 0)
        b = 6;
    else
        b = 7;		// error
    return b;
}

// 将由0、+1和-1组成的方向矩阵转换为标量表示。
static unsigned short inv_orientation_matrix_to_scalar(const signed char *mtx)
{
    unsigned short scalar;
	
    /*
       XYZ  010_001_000 Identity Matrix
       XZY  001_010_000
       YXZ  010_000_001
       YZX  000_010_001
       ZXY  001_000_010
       ZYX  000_001_010
     */
    scalar = inv_row_2_scale(mtx);
    scalar |= inv_row_2_scale(mtx + 3) << 3;
    scalar |= inv_row_2_scale(mtx + 6) << 6;

    return scalar;
}

//自检函数
static int8_t run_self_test(void)
{
    int result;
    long gyro[3], accel[3];

#if defined (MPU6500) || defined (MPU9250)
    result = mpu_run_6500_self_test(gyro, accel, 0);
#elif defined (MPU6050) || defined (MPU9150)
    result = mpu_run_self_test(gyro, accel);
#endif
	
    if (result == 0x7) {
		/* Push the calibrated data to the MPL library.
		 * 将校准后的数据推送到MPL库。
		 * MPL expects biases in hardware units << 16, but self test returns biases in g's << 16.
		 * MPL预计硬件单元的偏差<<16，但自检返回g<<16的偏差。
		 */
		unsigned short accel_sens;
		float gyro_sens;

		mpu_get_accel_sens(&accel_sens);
		accel[0] *= accel_sens;
		accel[1] *= accel_sens;
		accel[2] *= accel_sens;
		mpu_get_gyro_sens(&gyro_sens);
		gyro[0] = (long) (gyro[0] * gyro_sens);
		gyro[1] = (long) (gyro[1] * gyro_sens);
		gyro[2] = (long) (gyro[2] * gyro_sens);
    }
		
    else {
			if (!(result & 0x1))
					printf("Gyro failed.\n");
			if (!(result & 0x2))
					printf("Accel failed.\n");
			if (!(result & 0x4))
					printf("Compass failed.\n");
			return -1;
			}
		
		return 0;
}

/************************************************************/
// DMP库初始化
int8_t MPU6050_DMP_Init(void)
{
	int result=0;
	struct int_param_s int_param;
	
	// 移植于参考例程里面的测试例程 main() 函数
	// 注意，程序源码里面，inv开头相关程序都不需要移植
	
	//mpu初始化
	mpu_init(&int_param);	    
	//设置传感器
  mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL);     
	//设置FIFO
  mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL);  
	//设置采样率
  mpu_set_sample_rate(DEFAULT_MPU_HZ);  
	//加载dmp固件
	dmp_load_motion_driver_firmware();    
	//设置陀螺仪方向
	dmp_set_orientation(inv_orientation_matrix_to_scalar(gyro_orientation));
	//注册要在点击事件上执行的函数。
	//dmp_register_tap_cb(tap_cb);    //这里没有用到
	//注册要在 Android 方向事件上执行的函数。	
	//dmp_register_android_orient_cb(android_orient_cb);    //这里没有用到
	//设置dmp功能
	dmp_enable_feature(DMP_FEATURE_6X_LP_QUAT | DMP_FEATURE_TAP |
										 DMP_FEATURE_ANDROID_ORIENT | DMP_FEATURE_SEND_RAW_ACCEL |
									   DMP_FEATURE_SEND_CAL_GYRO | DMP_FEATURE_GYRO_CAL);
	//设置输出速率
	dmp_set_fifo_rate(DEFAULT_MPU_HZ);
	//自检，自检时需要将MPU6050平放
	result = run_self_test();
  //使能dmp
	mpu_set_dmp_state(1);
	
	//返回结果（自检经常不通过，因此将自检结果单独返回）
	return result;
}

// DMP库初始化
int8_t MPU6050_DMP_Get_Data(void)
{
	float q0 = 0.0f;
	float q1 = 0.0f;
	float q2 = 0.0f;
	float q3 = 0.0f;
	short gyro[3];
	short accel[3];
	long quat[4];
  unsigned long timestamp;
	short sensors;
	unsigned char more;
	
	//读取FIFO中的数据，读取正常返回0
	//参数：陀螺仪数据、加速度记数据、四元数数据、时间间隔(ms)、数据掩码(表示哪些数据收到)、剩余数据包
	if(dmp_read_fifo(gyro, accel, quat, &timestamp, &sensors, &more))
	{
		return -1;
	}
	
	// 若读取到加速度数据
	if(sensors & INV_XYZ_ACCEL)
	{
		// 保存加速度原始数据（因为量程范围未知，暂不进行转换）
		accx = accel[0];
		accy = accel[1];
		accz = accel[2];
	}
	// 若读取到角速度数据
	if(sensors & INV_XYZ_GYRO)
	{
	  // 保存角速度原始数据（因为量程范围未知，暂不进行转换）
		gyrox = gyro[0];
		gyroy = gyro[1];
		gyroz = gyro[2];
	}
	
	// 若读取到四元数数据
	if(sensors & INV_WXYZ_QUAT)
	{
		q0 = quat[0] / Q30;
		q1 = quat[1] / Q30;
		q2 = quat[2] / Q30;
		q3 = quat[3] / Q30;
		
		Angle_X_Final  = asin(-2*q1*q3 + 2*q0*q2) * rad2deg;
		Angle_Y_Final  = atan2(2*q2*q3 + 2*q0*q1, -2*q1*q1 - 2*q2*q2 + 1) * rad2deg;
		Angle_Z_Final  = atan2(2*(q1*q2 + q0*q3), q0*q0 + q1*q1 - q2*q2 - q3*q3) * rad2deg;
	}
	else return -2;    // 未读到数据，返回错误值
	
	return 0;
}


