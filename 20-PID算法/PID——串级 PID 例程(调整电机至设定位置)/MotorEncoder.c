/***********************************************
��˾����Ȥ�Ƽ�����ݸ�����޹�˾
Ʒ�ƣ�WHEELTEC
������wheeltec.net
�Ա����̣�shop114407458.taobao.com 
����ͨ: https://minibalance.aliexpress.com/store/4455017
�汾��5.7
�޸�ʱ�䣺2021-04-29

Brand: WHEELTEC
Website: wheeltec.net
Taobao shop: shop114407458.taobao.com 
Aliexpress: https://minibalance.aliexpress.com/store/4455017
Version:5.7
Update��2021-04-29

All rights reserved
***********************************************/

/* ������Դ��WHEELTEC */
/* ������Ϊ������������趨��λ�� */

#include "stm32f10x.h"

/*
float Velcity_Kp=80,  Velcity_Ki=10,  Velcity_Kd=0; //����ٶ�PID����
float Position_Kp=120, Position_Ki=0.1, Position_Kd=400; //���λ��PID����
*/
//�ⲿ���� extern˵���ı������������ļ�����
extern int   Encoder, CurrentPosition; //��ǰ�ٶȡ���ǰλ��
extern int   TargetVelocity, TargetCircle, CurrentPosition, Encoder,PWM; //Ŀ���ٶȡ�Ŀ��Ȧ����������������PWM���Ʊ���
extern float Velcity_Kp,  Velcity_Ki,  Velcity_Kd;  //����ٶ�PID����
extern float Position_Kp, Position_Ki, Position_Kd; //���λ��PID����
extern int   MortorRun;  //���������Ʊ�־λ


/***************************����Ϊ���ò��֣����Բ���**************************/
/***************************����Ϊ���ò��֣����Բ���**************************/
/***************************����Ϊ���ò��֣����Բ���**************************/

/*****************************���������ü���ʼ��*****************************/
/**************************************************************************
�������ܣ���������ʼ������
��ע��ʹ��TIM4 ͨ��12 ��Ϊ�������������
��ڲ�������
����  ֵ����
**************************************************************************/
void MotorEncoder_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; //����һ�����ų�ʼ���Ľṹ��  
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;//����һ����ʱ����ʼ���Ľṹ��
  TIM_ICInitTypeDef TIM_ICInitStructure; //����һ����ʱ��������ģʽ��ʼ���Ľṹ��
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //ʹ��TIM4ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //ʹ��CPIOBʱ��
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;	//TIM4_CH1��TIM4_CH2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //��������
	GPIO_Init(GPIOB, &GPIO_InitStructure);	//����GPIO_InitStructure�Ĳ�����ʼ��GPIO

	TIM_TimeBaseStructure.TIM_Period = 0xffff; //�趨�������Զ���װֵ
	TIM_TimeBaseStructure.TIM_Prescaler = 0; // Ԥ��Ƶ�� 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //ѡ��ʱ�ӷ�Ƶ������Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct�Ĳ�����ʼ����ʱ��TIM4
	
  TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising); //ʹ�ñ�����ģʽ3��CH1��CH2ͬʱ������Ϊ�ķ�Ƶ
  TIM_ICStructInit(&TIM_ICInitStructure); //��TIM_ICInitStruct �е�ÿһ��������ȱʡֵ����
	TIM_ICInitStructure.TIM_ICFilter = 10;  //�����˲�������
  TIM_ICInit(TIM4, &TIM_ICInitStructure); //��TIM_ICInitStructure������ʼ����ʱ��TIM4������ģʽ
	
	TIM_ClearFlag(TIM4, TIM_FLAG_Update);//���TIM�ĸ��±�־λ
  TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE); //�����ж�ʹ��
  TIM_SetCounter(TIM4,0); //��ʼ����ձ�������ֵ
	
	TIM_Cmd(TIM4, ENABLE); //ʹ�ܶ�ʱ��4
}

/**************************************************************************
�������ܣ���ȡTIM4��������ֵ
��ڲ�������
����  ֵ������������(���ٶ�)
**************************************************************************/
int Read_Encoder(void)
{
	int Encoder_TIM;
	Encoder_TIM=TIM4->CNT; //��ȡ����
	if(Encoder_TIM>0xefff)Encoder_TIM=Encoder_TIM-0xffff; //ת������ֵΪ�з����ֵ������0��ת��С��0��ת��
	                                                      //TIM4->CNT��ΧΪ0-0xffff����ֵΪ0��
	TIM4->CNT=0; //��ȡ����������
	return Encoder_TIM; //����ֵ
}

/**************************************************************************
�������ܣ�TIM4�жϷ�����
��ڲ�������
����  ֵ����
**************************************************************************/
void TIM4_IRQHandler(void)
{ 		    		  			    
	if(TIM4->SR&0X0001)//����ж�
	{    				   				     	    	
	}				   
	TIM4->SR&=~(1<<0);//����жϱ�־λ 	    
}

/*****************************������ʱ����ʱ*****************************/
/**************************************************************************
�������ܣ�ͨ�ö�ʱ��2��ʼ��������
��ڲ������Զ���װ��ֵ Ԥ��Ƶϵ�� Ĭ�϶�ʱʱ��Ϊ72MHZʱ�����߹�ͬ������ʱ�ж�ʱ��
����  ֵ����
��ע��
��������EncoderRead_TIM2(7199, 99); 
����ʱ����ʱ��10ms
**************************************************************************/
void EncoderRead_TIM2(u16 arr, u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStrue; //����һ����ʱ�жϵĽṹ��
	NVIC_InitTypeDef NVIC_InitStrue; //����һ���ж����ȼ���ʼ���Ľṹ��
	
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //ʹ��ͨ�ö�ʱ��2ʱ��
	
	TIM_TimeBaseInitStrue.TIM_Period=arr; //����ģʽΪ���ϼ���ʱ����ʱ����0��ʼ����������������arrʱ������ʱ�жϷ�����
	TIM_TimeBaseInitStrue.TIM_Prescaler=psc; //Ԥ��Ƶϵ��������ÿһ��������ʱ��
	TIM_TimeBaseInitStrue.TIM_CounterMode=TIM_CounterMode_Up; //����ģʽ�����ϼ���
	TIM_TimeBaseInitStrue.TIM_ClockDivision=TIM_CKD_DIV1; //һ�㲻ʹ�ã�Ĭ��TIM_CKD_DIV1
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStrue); //����TIM_TimeBaseInitStrue�Ĳ�����ʼ����ʱ��TIM2
	
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE); //ʹ��TIM2�жϣ��ж�ģʽΪ�����жϣ�TIM_IT_Update
	
	NVIC_InitStrue.NVIC_IRQChannel=TIM2_IRQn; //����TIM2�ж�
	NVIC_InitStrue.NVIC_IRQChannelCmd=ENABLE; //�ж�ʹ��
	NVIC_InitStrue.NVIC_IRQChannelPreemptionPriority=1; //��ռ���ȼ�Ϊ1����ֵԽС���ȼ�Խ�ߣ�0�����ȼ����
	NVIC_InitStrue.NVIC_IRQChannelSubPriority=1; //��Ӧ���ȼ�Ϊ1����ֵԽС���ȼ�Խ�ߣ�0�����ȼ����
	NVIC_Init(&NVIC_InitStrue); //����NVIC_InitStrue�Ĳ�����ʼ��VIC�Ĵ���������TIM2�ж�
	
	TIM_Cmd(TIM2, ENABLE); //ʹ�ܶ�ʱ��TIM2
}


/*****************************����ΪPID��������*****************************/
/*****************************����ΪPID��������*****************************/
/*****************************����ΪPID��������*****************************/

/**************************************************************************
�������ܣ��ٶȱջ�PID����(ʵ��ΪPI����)
��ڲ�����Ŀ���ٶ� ��ǰ�ٶ�
����  ֵ���ٶȿ���ֵ
����**����ʽ��ɢPID��ʽ** 

ControlVelocity+=Kp[e��k��-e(k-1)]+Ki*e(k)+Kd[e(k)-2e(k-1)+e(k-2)]
e(k)������ƫ�� 
e(k-1)������һ�ε�ƫ��  �Դ����� 
ControlVelocity�����������
�����ǵ��ٶȿ��Ʊջ�ϵͳ���棬ֻʹ��PI����
ControlVelocity+=Kp[e��k��-e(k-1)]+Ki*e(k)
**************************************************************************/
int Velocity_FeedbackControl(int TargetVelocity, int CurrentVelocity)
{
		int Bias;  //����ƫ�� 
		static int ControlVelocity, Last_bias; //�ٶȿ���ֵ���ϴε�ƫ��
		
		Bias=TargetVelocity-CurrentVelocity; //���ٶ�ƫ��
		
		// ������ô��⣬�ٶ�ƫ��*����ϵ�� = �ٶȿ���ֵ
		ControlVelocity+=Velcity_Kp*(Bias-Last_bias)+Velcity_Ki*Bias;  //����ʽPI������
                                                                   //Velcity_Kp*(Bias-Last_bias) ����Ϊ���Ƽ��ٶ�
	                                                                 //Velcity_Ki*Bias             �ٶȿ���ֵ��Bias���ϻ��ֵõ� ƫ��Խ����ٶ�Խ��
		Last_bias=Bias;	
		return ControlVelocity; //�����ٶȿ���ֵ
}

/**************************************************************************
�������ܣ�λ��ʽPID������
��ڲ�����Ŀ��Ȧ��(λ��) ��ǰλ��
����  ֵ���ٶȿ���ֵ
����**λ��ʽ��ɢPID��ʽ** 

pwm=Kp*e(k)+Ki*��e(k)+Kd[e��k��-e(k-1)]
e(k)������ƫ�� 
e(k-1)������һ�ε�ƫ��  
��e(k)����e(k)�Լ�֮ǰ��ƫ����ۻ���;����kΪ1,2,,k;
pwm�������
**************************************************************************/
int Position_FeedbackControl(float Circle, int CurrentPosition)
{
		float TargetPosition,Bias, ControlVelocity;     //Ŀ��λ�ã�λ��ƫ��ٶȿ���ֵ
		static float Last_bias, Integral_Bias;          //�ϴε�ƫ�ƫ���ۼ�
		
	  TargetPosition=Circle*1040*1.04; //Ŀ��λ��=Ŀ��Ȧ��*1040
	                              //10ms��ȡһ�α�����(��100HZ)��������ٱ�Ϊ20����������������13��AB˫����ϵõ�4��Ƶ��
	                              //��ת1Ȧ����������Ϊ20*13*4=1040�����ת��=Encoder*100/1040r/s ʹ�ö�ʱ��2
	                              //1.04�����ϵ�����������������ɸ���ʵ�����������ϵ������߿��ƾ���
	
		Bias=TargetPosition-CurrentPosition; //��λ��ƫ��
	  Integral_Bias+=Bias;
    if(Integral_Bias> 970) Integral_Bias= 970;	//�����޷� ��ֹ����Ŀ��λ�ú����
	  if(Integral_Bias<-970) Integral_Bias=-970;	//�����޷� ��ֹ����Ŀ��λ�ú����
	
		ControlVelocity=Position_Kp*Bias+Position_Ki*Integral_Bias+Position_Kd*(Bias-Last_bias);  //����ʽPI������
	                                                                                            //Position_Kp*Bias ƫ��Խ���ٶ�Խ��
	                                                                                            //Position_Ki*Integral_Bias ��С��̬���
	                                                                                            //Position_Kd*(Bias-Last_bias) �����ٶ�

		Last_bias=Bias;	
		return ControlVelocity;    //�����ٶȿ���ֵ
}

/**************************************************************************
�������ܣ��ٶ�(PWM)�޷�
��ڲ�����PWM_P:λ�û������PWMֵ TargetVelocity:Ŀ���ٶ�(�ٶ�����ֵ)
����  ֵ����
**************************************************************************/
int PWM_Restrict(int PWM_P, int TargetVelocity)
{
    if     (PWM_P>+TargetVelocity*76) PWM_P=+TargetVelocity*76;
	  else if(PWM_P<-TargetVelocity*76) PWM_P=-TargetVelocity*76;
	  else PWM_P=PWM_P;
	  return PWM_P;
}

/**************************************************************************
�������ܣ�TIM2�жϷ����� ��ʱ��ȡ��������ֵ������ 10ms����һ��
��ڲ�������
����  ֵ����
**************************************************************************/
void TIM2_IRQHandler()
{
	static int PWM_P, PWM_V, TimeCount;  //�ٶ�λ�ô���PID���Ʊ���PWM_P��PWM_V, ��ʱ����TimeCount; ��̬�������������ý�������ֵ��Ȼ����
  if(TIM_GetITStatus(TIM2, TIM_IT_Update)==1) //�������ж�ʱ״̬�Ĵ���(TIMx_SR)��bit0�ᱻӲ����1
	{
	  Encoder=Read_Encoder();   //��ȡ��ǰ���������������ٶ�(Encoder/10ms)
		CurrentPosition+=Encoder; //����������(�ٶ�)���ֵõ�λ��(��������������Ҫ����ΪȦ��)
		
		if(MortorRun) //����������£����е�����Ƴ���
			{
				TimeCount++; //��ʱ
				if(TimeCount<=600)				TargetCircle=15,TargetVelocity=40;     //ǰ6��=600*10ms  Ŀ��λ��=15*1040 Ŀ���ٶ�=40(/10ms)
				else if(600<TimeCount)		TargetCircle=30,TargetVelocity=20; 		 //6���  Ŀ��λ��=30*1040 Ŀ���ٶ�=20(/10ms)

				// Ŀ��λ��, ����λ�� �� �ٶȿ���ֵ
				PWM_P=Position_FeedbackControl(TargetCircle, CurrentPosition); //λ�ñջ�����
				PWM_P=PWM_Restrict(PWM_P,TargetVelocity); //�޷�λ�û������PWM

				// ���ת��ԭ��
        PWM_V=PWM_P/76; //PWMֵת��Ϊ�ٶ�ֵ
				// Ŀ���ٶ�, �����ٶ� �� �ٶȿ���ֵ
				PWM=Velocity_FeedbackControl(PWM_V, Encoder); //�ٶȻ��ջ����� �൱��λ�û������Ϊ�ٶȻ������룬�γɴ���PID
				// Ŀ���ٶ� ���ת��Ϊ PWM��

				SetPWM(PWM); //����PWM
			}
		  else PWM=0,SetPWM(PWM); //��������ٴΰ��£����ֹͣ
			
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update); //״̬�Ĵ���(TIMx_SR)��bit0��0
	}
}

/*
���������⣺

PWM �� ������ƹ��� �� �ٶ�(����������)
�����ٶ�Bias �𽥻�� PWM Bias				
�����ٶ�Bias��PWM Bias�Ĺ�ϵ������ PIDϵ�� ȷ��������

PWM �� ������ƹ��� �� �ٶ�(����������) �� λ��
����λ��Bias �𽥻�� PWM Bias				
����λ��Bias��PWM Bias�Ĺ�ϵ������ PIDϵ�� ȷ��������


*/