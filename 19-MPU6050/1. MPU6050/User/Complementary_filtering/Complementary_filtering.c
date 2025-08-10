#include "./Complementary_filtering/Complementary_filtering.h"
#include "./kalman_filter/kalman.h"
#include "./mpu6050/mpu6050.h"
#include "math.h"

// ��ʱ����
#define dt        0.02   // �˲��㷨�������ڣ��ɶ�ʱ����ʱ20ms

/**************************************************************************
�������ܣ�һ�׻����˲�
��ڲ��������ٶȻ�ȡ�ĽǶȡ����ٶ�
����  ֵ��x��Ƕ�
**************************************************************************/
float Complementary_Filter_x(float angle_m, float gyro_m)
{
	 float K1 =0.7;    // �����˲�ϵ��
	
	 static float angle;
   angle = K1 * angle_m+ (1-K1) * (angle + gyro_m * dt);
	 return angle;
}

/**************************************************************************
�������ܣ�һ�׻����˲�
��ڲ��������ٶȻ�ȡ�ĽǶȡ����ٶ�
����  ֵ��y��Ƕ�
**************************************************************************/
float Complementary_Filter_y(float angle_m, float gyro_m)
{
	 float K1 =0.7;    // �����˲�ϵ�� 
	
	 static float angle;
   angle = K1 * angle_m+ (1-K1) * (angle + gyro_m * dt);
	 return angle;
}

