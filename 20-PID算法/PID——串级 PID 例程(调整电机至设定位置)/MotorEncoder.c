/***********************************************
公司：轮趣科技（东莞）有限公司
品牌：WHEELTEC
官网：wheeltec.net
淘宝店铺：shop114407458.taobao.com 
速卖通: https://minibalance.aliexpress.com/store/4455017
版本：5.7
修改时间：2021-04-29

Brand: WHEELTEC
Website: wheeltec.net
Taobao shop: shop114407458.taobao.com 
Aliexpress: https://minibalance.aliexpress.com/store/4455017
Version:5.7
Update：2021-04-29

All rights reserved
***********************************************/

/* 程序来源于WHEELTEC */
/* 程序功能为：调整电机至设定的位置 */

#include "stm32f10x.h"

/*
float Velcity_Kp=80,  Velcity_Ki=10,  Velcity_Kd=0; //相关速度PID参数
float Position_Kp=120, Position_Ki=0.1, Position_Kd=400; //相关位置PID参数
*/
//外部变量 extern说明改变量已在其它文件定义
extern int   Encoder, CurrentPosition; //当前速度、当前位置
extern int   TargetVelocity, TargetCircle, CurrentPosition, Encoder,PWM; //目标速度、目标圈数、编码器读数、PWM控制变量
extern float Velcity_Kp,  Velcity_Ki,  Velcity_Kd;  //相关速度PID参数
extern float Position_Kp, Position_Ki, Position_Kd; //相关位置PID参数
extern int   MortorRun;  //允许电机控制标志位


/***************************以下为配置部分，可以不看**************************/
/***************************以下为配置部分，可以不看**************************/
/***************************以下为配置部分，可以不看**************************/

/*****************************编码器配置及初始化*****************************/
/**************************************************************************
函数功能：编码器初始化函数
备注：使用TIM4 通道12 作为编码器输出引脚
入口参数：无
返回  值：无
**************************************************************************/
void MotorEncoder_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; //定义一个引脚初始化的结构体  
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;//定义一个定时器初始化的结构体
  TIM_ICInitTypeDef TIM_ICInitStructure; //定义一个定时器编码器模式初始化的结构体
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //使能TIM4时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //使能CPIOB时钟
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;	//TIM4_CH1、TIM4_CH2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入
	GPIO_Init(GPIOB, &GPIO_InitStructure);	//根据GPIO_InitStructure的参数初始化GPIO

	TIM_TimeBaseStructure.TIM_Period = 0xffff; //设定计数器自动重装值
	TIM_TimeBaseStructure.TIM_Prescaler = 0; // 预分频器 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //选择时钟分频：不分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct的参数初始化定时器TIM4
	
  TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising); //使用编码器模式3：CH1、CH2同时计数，为四分频
  TIM_ICStructInit(&TIM_ICInitStructure); //把TIM_ICInitStruct 中的每一个参数按缺省值填入
	TIM_ICInitStructure.TIM_ICFilter = 10;  //设置滤波器长度
  TIM_ICInit(TIM4, &TIM_ICInitStructure); //根TIM_ICInitStructure参数初始化定时器TIM4编码器模式
	
	TIM_ClearFlag(TIM4, TIM_FLAG_Update);//清除TIM的更新标志位
  TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE); //更新中断使能
  TIM_SetCounter(TIM4,0); //初始化清空编码器数值
	
	TIM_Cmd(TIM4, ENABLE); //使能定时器4
}

/**************************************************************************
函数功能：读取TIM4编码器数值
入口参数：无
返回  值：编码器读数(即速度)
**************************************************************************/
int Read_Encoder(void)
{
	int Encoder_TIM;
	Encoder_TIM=TIM4->CNT; //读取计数
	if(Encoder_TIM>0xefff)Encoder_TIM=Encoder_TIM-0xffff; //转化计数值为有方向的值，大于0正转，小于0反转。
	                                                      //TIM4->CNT范围为0-0xffff，初值为0。
	TIM4->CNT=0; //读取完后计数清零
	return Encoder_TIM; //返回值
}

/**************************************************************************
函数功能：TIM4中断服务函数
入口参数：无
返回  值：无
**************************************************************************/
void TIM4_IRQHandler(void)
{ 		    		  			    
	if(TIM4->SR&0X0001)//溢出中断
	{    				   				     	    	
	}				   
	TIM4->SR&=~(1<<0);//清除中断标志位 	    
}

/*****************************基本定时器定时*****************************/
/**************************************************************************
函数功能：通用定时器2初始化函数，
入口参数：自动重装载值 预分频系数 默认定时时钟为72MHZ时，两者共同决定定时中断时间
返回  值：无
备注：
主函数：EncoderRead_TIM2(7199, 99); 
即定时器定时：10ms
**************************************************************************/
void EncoderRead_TIM2(u16 arr, u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStrue; //定义一个定时中断的结构体
	NVIC_InitTypeDef NVIC_InitStrue; //定义一个中断优先级初始化的结构体
	
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //使能通用定时器2时钟
	
	TIM_TimeBaseInitStrue.TIM_Period=arr; //计数模式为向上计数时，定时器从0开始计数，计数超过到arr时触发定时中断服务函数
	TIM_TimeBaseInitStrue.TIM_Prescaler=psc; //预分频系数，决定每一个计数的时长
	TIM_TimeBaseInitStrue.TIM_CounterMode=TIM_CounterMode_Up; //计数模式：向上计数
	TIM_TimeBaseInitStrue.TIM_ClockDivision=TIM_CKD_DIV1; //一般不使用，默认TIM_CKD_DIV1
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStrue); //根据TIM_TimeBaseInitStrue的参数初始化定时器TIM2
	
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE); //使能TIM2中断，中断模式为更新中断：TIM_IT_Update
	
	NVIC_InitStrue.NVIC_IRQChannel=TIM2_IRQn; //属于TIM2中断
	NVIC_InitStrue.NVIC_IRQChannelCmd=ENABLE; //中断使能
	NVIC_InitStrue.NVIC_IRQChannelPreemptionPriority=1; //抢占优先级为1级，值越小优先级越高，0级优先级最高
	NVIC_InitStrue.NVIC_IRQChannelSubPriority=1; //响应优先级为1级，值越小优先级越高，0级优先级最高
	NVIC_Init(&NVIC_InitStrue); //根据NVIC_InitStrue的参数初始化VIC寄存器，设置TIM2中断
	
	TIM_Cmd(TIM2, ENABLE); //使能定时器TIM2
}


/*****************************以下为PID控制内容*****************************/
/*****************************以下为PID控制内容*****************************/
/*****************************以下为PID控制内容*****************************/

/**************************************************************************
函数功能：速度闭环PID控制(实际为PI控制)
入口参数：目标速度 当前速度
返回  值：速度控制值
根据**增量式离散PID公式** 

ControlVelocity+=Kp[e（k）-e(k-1)]+Ki*e(k)+Kd[e(k)-2e(k-1)+e(k-2)]
e(k)代表本次偏差 
e(k-1)代表上一次的偏差  以此类推 
ControlVelocity代表增量输出
在我们的速度控制闭环系统里面，只使用PI控制
ControlVelocity+=Kp[e（k）-e(k-1)]+Ki*e(k)
**************************************************************************/
int Velocity_FeedbackControl(int TargetVelocity, int CurrentVelocity)
{
		int Bias;  //本次偏差 
		static int ControlVelocity, Last_bias; //速度控制值，上次的偏差
		
		Bias=TargetVelocity-CurrentVelocity; //求速度偏差
		
		// 这里这么理解，速度偏差*比例系数 = 速度控制值
		ControlVelocity+=Velcity_Kp*(Bias-Last_bias)+Velcity_Ki*Bias;  //增量式PI控制器
                                                                   //Velcity_Kp*(Bias-Last_bias) 作用为限制加速度
	                                                                 //Velcity_Ki*Bias             速度控制值由Bias不断积分得到 偏差越大加速度越大
		Last_bias=Bias;	
		return ControlVelocity; //返回速度控制值
}

/**************************************************************************
函数功能：位置式PID控制器
入口参数：目标圈数(位置) 当前位置
返回  值：速度控制值
根据**位置式离散PID公式** 

pwm=Kp*e(k)+Ki*∑e(k)+Kd[e（k）-e(k-1)]
e(k)代表本次偏差 
e(k-1)代表上一次的偏差  
∑e(k)代表e(k)以及之前的偏差的累积和;其中k为1,2,,k;
pwm代表输出
**************************************************************************/
int Position_FeedbackControl(float Circle, int CurrentPosition)
{
		float TargetPosition,Bias, ControlVelocity;     //目标位置，位置偏差，速度控制值
		static float Last_bias, Integral_Bias;          //上次的偏差，偏差累计
		
	  TargetPosition=Circle*1040*1.04; //目标位置=目标圈数*1040
	                              //10ms读取一次编码器(即100HZ)，电机减速比为20，霍尔编码器精度13，AB双相组合得到4倍频，
	                              //则转1圈编码器读数为20*13*4=1040，电机转速=Encoder*100/1040r/s 使用定时器2
	                              //1.04是误差系数，电机本身存在误差，可根据实际情况调整该系数以提高控制精度
	
		Bias=TargetPosition-CurrentPosition; //求位置偏差
	  Integral_Bias+=Bias;
    if(Integral_Bias> 970) Integral_Bias= 970;	//积分限幅 防止到达目标位置后过冲
	  if(Integral_Bias<-970) Integral_Bias=-970;	//积分限幅 防止到达目标位置后过冲
	
		ControlVelocity=Position_Kp*Bias+Position_Ki*Integral_Bias+Position_Kd*(Bias-Last_bias);  //增量式PI控制器
	                                                                                            //Position_Kp*Bias 偏差越大速度越大
	                                                                                            //Position_Ki*Integral_Bias 减小稳态误差
	                                                                                            //Position_Kd*(Bias-Last_bias) 限制速度

		Last_bias=Bias;	
		return ControlVelocity;    //返回速度控制值
}

/**************************************************************************
函数功能：速度(PWM)限幅
入口参数：PWM_P:位置环输出的PWM值 TargetVelocity:目标速度(速度限制值)
返回  值：无
**************************************************************************/
int PWM_Restrict(int PWM_P, int TargetVelocity)
{
    if     (PWM_P>+TargetVelocity*76) PWM_P=+TargetVelocity*76;
	  else if(PWM_P<-TargetVelocity*76) PWM_P=-TargetVelocity*76;
	  else PWM_P=PWM_P;
	  return PWM_P;
}

/**************************************************************************
函数功能：TIM2中断服务函数 定时读取编码器数值并清零 10ms进入一次
入口参数：无
返回  值：无
**************************************************************************/
void TIM2_IRQHandler()
{
	static int PWM_P, PWM_V, TimeCount;  //速度位置串级PID控制变量PWM_P、PWM_V, 计时变量TimeCount; 静态变量，函数调用结束后其值依然存在
  if(TIM_GetITStatus(TIM2, TIM_IT_Update)==1) //当发生中断时状态寄存器(TIMx_SR)的bit0会被硬件置1
	{
	  Encoder=Read_Encoder();   //读取当前编码器读数，即速度(Encoder/10ms)
		CurrentPosition+=Encoder; //编码器读数(速度)积分得到位置(编码器读数，需要换算为圈数)
		
		if(MortorRun) //如果按键按下，运行电机控制程序
			{
				TimeCount++; //计时
				if(TimeCount<=600)				TargetCircle=15,TargetVelocity=40;     //前6秒=600*10ms  目标位置=15*1040 目标速度=40(/10ms)
				else if(600<TimeCount)		TargetCircle=30,TargetVelocity=20; 		 //6秒后  目标位置=30*1040 目标速度=20(/10ms)

				// 目标位置, 现在位置 → 速度控制值
				PWM_P=Position_FeedbackControl(TargetCircle, CurrentPosition); //位置闭环控制
				PWM_P=PWM_Restrict(PWM_P,TargetVelocity); //限幅位置环输出的PWM

				// 这个转换原理？
        PWM_V=PWM_P/76; //PWM值转换为速度值
				// 目标速度, 现在速度 → 速度控制值
				PWM=Velocity_FeedbackControl(PWM_V, Encoder); //速度环闭环控制 相当于位置环的输出为速度环的输入，形成串级PID
				// 目标速度 如何转换为 PWM？

				SetPWM(PWM); //设置PWM
			}
		  else PWM=0,SetPWM(PWM); //如果按键再次按下，电机停止
			
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update); //状态寄存器(TIMx_SR)的bit0置0
	}
}

/*
物理过程理解：

PWM → 黑箱控制过程 → 速度(编码器读数)
根据速度Bias 逐渐获得 PWM Bias				
至于速度Bias和PWM Bias的关系，就由 PID系数 确定！！！

PWM → 黑箱控制过程 → 速度(编码器读数) → 位置
根据位置Bias 逐渐获得 PWM Bias				
至于位置Bias和PWM Bias的关系，就由 PID系数 确定！！！


*/