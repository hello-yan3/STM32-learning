#include "./Complementary_filtering/Complementary_filtering.h"
#include "./kalman_filter/kalman.h"
#include "./mpu6050/mpu6050.h"
#include "math.h"

// 定时周期
#define dt        0.02   // 滤波算法计算周期，由定时器定时20ms

/**************************************************************************
函数功能：一阶互补滤波
入口参数：加速度获取的角度、角速度
返回  值：x轴角度
**************************************************************************/
float Complementary_Filter_x(float angle_m, float gyro_m)
{
	 float K1 =0.7;    // 互补滤波系数
	
	 static float angle;
   angle = K1 * angle_m+ (1-K1) * (angle + gyro_m * dt);
	 return angle;
}

/**************************************************************************
函数功能：一阶互补滤波
入口参数：加速度获取的角度、角速度
返回  值：y轴角度
**************************************************************************/
float Complementary_Filter_y(float angle_m, float gyro_m)
{
	 float K1 =0.7;    // 互补滤波系数 
	
	 static float angle;
   angle = K1 * angle_m+ (1-K1) * (angle + gyro_m * dt);
	 return angle;
}

