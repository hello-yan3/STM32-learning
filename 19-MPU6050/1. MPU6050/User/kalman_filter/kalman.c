/**
  ******************************************************************************
  * @file    kalman.c
  * @author  https://www.bilibili.com/video/BV15f4y1h7GM/
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   I/O线中断应用bsp
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 指南者 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */


#include "./kalman_filter/kalman.h"
#include "./mpu6050/mpu6050.h"
#include "math.h"

// 定时周期
#define delta_T        0.02   // 滤波算法计算周期，由定时器定时20ms

/**************************************************************************
函数功能：获取x轴角度简易卡尔曼滤波
入口参数：加速度获取的角度、角速度
返回  值：x轴角速度
**************************************************************************/
void Kalman_Filter_X(float Accel,float Gyro) //卡尔曼函数		
{
	float dt      = delta_T;		//滤波算法计算周期，由定时器定时确定
	float Q_angle = 0.001;	  	//角度数据置信度，角度噪声的协方差
	float Q_gyro  = 0.003;			//角速度数据置信度，角速度噪声的协方差  
	float R_angle = 0.5;				//加速度计测量噪声的协方差
	char  C_0     = 1;					//H矩阵值
	
	float Q_bias, Angle_err;	//Q_bias:陀螺仪的偏差  Angle_err:角度偏量 
	float PCt_0, PCt_1, E;		//计算的过程量
	float K_0, K_1, t_0, t_1;	//卡尔曼增益  K_0:用于计算最优估计值  K_1:用于计算最优估计值的偏差 t_0/1:中间变量
	float P[4] ={0,0,0,0};	//过程协方差矩阵的微分矩阵，中间变量
	float PP[2][2] = { { 1, 0 },{ 0, 1 } };//过程协方差矩阵P
	
	//步骤一，先验估计
	//公式：X(k|k-1) = AX(k-1|k-1) + BU(k)
	//X = (Angle,Q_bias)
	//A(1,1) = 1,A(1,2) = -dt
	//A(2,1) = 0,A(2,2) = 1
	Angle_X_Final += (Gyro - Q_bias) * dt; //状态方程,角度值等于上次最优角度加角速度减零漂后积分
	
	//步骤二，计算过程协方差矩阵的微分矩阵
	//公式：P(k|k-1)=AP(k-1|k-1)A^T + Q 
	//Q(1,1) = cov(Angle,Angle)	Q(1,2) = cov(Q_bias,Angle)
	//Q(2,1) = cov(Angle,Q_bias)	Q(2,2) = cov(Q_bias,Q_bias)
	P[0]= Q_angle - PP[0][1] - PP[1][0];
	P[1]= -PP[1][1];// 先验估计误差协方差
	P[2]= -PP[1][1];
	P[3]= Q_gyro;
	PP[0][0] += P[0] * dt;   
	PP[0][1] += P[1] * dt;   
	PP[1][0] += P[2] * dt;
	PP[1][1] += P[3] * dt;	
	Angle_err = Accel - Angle_X_Final;	//Z(k)先验估计 计算角度偏差
	
	//步骤三，计算卡尔曼增益
	//公式：Kg(k)= P(k|k-1)H^T/(HP(k|k-1)H^T+R)
	//Kg = (K_0,K_1) 对应Angle,Q_bias增益
	//H = (1,0)	可由z=HX+v求出z:Accel
	PCt_0 = C_0 * PP[0][0];
	PCt_1 = C_0 * PP[1][0];
	E = R_angle + C_0 * PCt_0;
	K_0 = PCt_0 / E;
	K_1 = PCt_1 / E;
	
	//步骤四，后验估计误差协方差
	//公式：P(k|k)=(I-Kg(k)H)P(k|k-1)
	//也可写为：P(k|k)=P(k|k-1)-Kg(k)HP(k|k-1)
	t_0 = PCt_0;
	t_1 = C_0 * PP[0][1];
	PP[0][0] -= K_0 * t_0;		
	PP[0][1] -= K_0 * t_1;
	PP[1][0] -= K_1 * t_0;
	PP[1][1] -= K_1 * t_1;
	
	//步骤五，计算最优角速度值
	//公式：X(k|k)= X(k|k-1)+Kg(k)(Z(k)-X(k|k-1))
	Angle_X_Final += K_0 * Angle_err;	 //后验估计，给出最优估计值
	Q_bias        += K_1 * Angle_err;	 //后验估计，跟新最优估计值偏差
	GyroX         = Gyro - Q_bias;	   //最优角速度
}

/**************************************************************************
函数功能：获取y轴角度简易卡尔曼滤波
入口参数：加速度获取的角度、角速度
返回  值：y轴角速度
**************************************************************************/
void Kalman_Filter_Y(float Accel,float Gyro) 		
{
	float dt      = delta_T;	//滤波算法计算周期，由定时器定时确定
	float Q_angle = 0.001;		//角度数据置信度，角度噪声的协方差
	float Q_gyro  = 0.003;		//角速度数据置信度，角速度噪声的协方差  
	float R_angle = 0.5;			//加速度计测量噪声的协方差
	char  C_0     = 1;				//H矩阵值
	
	float Q_bias, Angle_err;	//Q_bias:陀螺仪的偏差  Angle_err:角度偏量 
	float PCt_0, PCt_1, E;		//计算的过程量
	float K_0, K_1, t_0, t_1;	//卡尔曼增益  K_0:用于计算最优估计值  K_1:用于计算最优估计值的偏差 t_0/1:中间变量
	float P[4] ={0,0,0,0};	//过程协方差矩阵的微分矩阵，中间变量
	float PP[2][2] = { { 1, 0 },{ 0, 1 } };//过程协方差矩阵P
	
	//步骤一，先验估计
	//公式：X(k|k-1) = AX(k-1|k-1) + BU(k)
	//X = (Angle,Q_bias)
	//A(1,1) = 1,A(1,2) = -dt
	//A(2,1) = 0,A(2,2) = 1
	Angle_Y_Final += (Gyro - Q_bias) * dt; //状态方程,角度值等于上次最优角度加角速度减零漂后积分
	
	//步骤二，计算过程协方差矩阵的微分矩阵
	//公式：P(k|k-1)=AP(k-1|k-1)A^T + Q 
	//Q(1,1) = cov(Angle,Angle)	Q(1,2) = cov(Q_bias,Angle)
	//Q(2,1) = cov(Angle,Q_bias)	Q(2,2) = cov(Q_bias,Q_bias)
	P[0]=Q_angle - PP[0][1] - PP[1][0]; 
	P[1]=-PP[1][1];// 先验估计误差协方差
	P[2]=-PP[1][1];
	P[3]=Q_gyro;	
	PP[0][0] += P[0] * dt; 
	PP[0][1] += P[1] * dt;  
	PP[1][0] += P[2] * dt;
	PP[1][1] += P[3] * dt;	
	Angle_err = Accel - Angle_Y_Final;			//Z(k)先验估计 计算角度偏差
	
	//步骤三，计算卡尔曼增益
	//公式：Kg(k)= P(k|k-1)H^T/(HP(k|k-1)H^T+R)
	//Kg = (K_0,K_1) 对应Angle,Q_bias增益
	//H = (1,0)	可由z=HX+v求出z:Accel
	PCt_0 = C_0 * PP[0][0];
	PCt_1 = C_0 * PP[1][0];	
	E = R_angle + C_0 * PCt_0;	
	K_0 = PCt_0 / E;
	K_1 = PCt_1 / E;	
		
	//步骤四，后验估计误差协方差
	//公式：P(k|k)=(I-Kg(k)H)P(k|k-1)
	//也可写为：P(k|k)=P(k|k-1)-Kg(k)HP(k|k-1)
	t_0 = PCt_0;
	t_1 = C_0 * PP[0][1];
	PP[0][0] -= K_0 * t_0;		
	PP[0][1] -= K_0 * t_1;
	PP[1][0] -= K_1 * t_0;
	PP[1][1] -= K_1 * t_1;		
	
	//步骤五，计算最优角速度值
	//公式：X(k|k)= X(k|k-1)+Kg(k)(Z(k)-X(k|k-1))
	Angle_Y_Final	+= K_0 * Angle_err;	 //后验估计，给出最优估计值
	Q_bias	+= K_1 * Angle_err;	 	 //后验估计，跟新最优估计值偏差
	GyroY   = Gyro - Q_bias;	     //最优角速度	 
}

