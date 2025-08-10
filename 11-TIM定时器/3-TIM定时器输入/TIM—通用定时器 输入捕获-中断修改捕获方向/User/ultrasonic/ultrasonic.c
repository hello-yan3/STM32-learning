/**
  ******************************************************************************
  * @file    ultrasonic.c
  * @author  LJK
  * @version V1.0
  * @date    2023-xx-xx
  * @brief   超声波模块
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F103 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
	
#include "./ultrasonic/ultrasonic.h"
#include "./tim/bsp_general_tim_IC.h"
#include "./usart/bsp_debug_usart.h"

// STA的第一位/第二位表示捕获状态，(STA&0x01)!=0 捕获到一完整脉冲、(STA&0x02)!=0 捕获到上升沿；
// 0x01=BIN:0001/0x02=BIN:0010, 注意这里还不能用: (STA&0x02)==1，因为STA&0x02==0x02!=1  
uint8_t  STA;         // 捕获状态标志位
uint8_t  User_Period; // 记录捕获寄存器CCR溢出次数
uint16_t CAPTURE_VAL; // 记录捕获寄存器CCR值
uint32_t Distance;    // 超声波所测得的距离
uint32_t Time;        // 超声波收到数据花费的时间


/* 初始化PA7，从而给给触发引脚一个至少 10us 的脉冲信号 */
void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
	ultrasonic_GPIO_CLK_ENABLE();

  GPIO_InitStruct.Pin = ultrasonic_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(ultrasonic_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(ultrasonic_GPIO_Port, ultrasonic_Pin, GPIO_PIN_RESET);
}

/*
*********************************************************************************************************
*	函 数 名: Delay_us
*	功能说明: 简单的延时函数（移植与I2C）
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void Delay_us(void)
{
	uint16_t i;

	/*　
	 	下面的时间是通过逻辑分析仪测试得到的。
    工作条件：CPU主频72MHz ，MDK编译环境，1级优化

		循环次数为10时，SCL频率 = 205KHz 
		循环次数为7时，SCL频率 = 347KHz， SCL高电平时间1.5us，SCL低电平时间2.87us 
	 	循环次数为5时，SCL频率 = 421KHz， SCL高电平时间1.25us，SCL低电平时间2.375us 
	
		SCL频率200K，即一周期5us
	*/
	for (i = 0; i < 0x40; i++);
}


/**************************************************************************
Function: Pulse width reading interruption of ultrasonic echo
Input   : none
Output  : none
函数功能：超声波回波脉宽读取中断②：更新中断
入口参数: 无 
返回  值：无
**************************************************************************/	 
//定时器更新中断（计数溢出）中断处理回调函数
//每次CNT=ARR时，进入此中断
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)   //更新中断（溢出）发生时执行
{
	if(htim==&TIM_TimeBaseStructure_IC)
	{
		//DEBUG_INFO("进入定时器溢出中断回调函数");
		if( (STA&0x01)==0 )    //如果还未成功捕获
		{
			if( (STA&0x02)!=0 )         //(STA&0x02)!=0，说明成功捕获到高电平
			{
				// 之后每进入一次中断，User_Period++ (else 内容),
				// 若User_Period超过0x3F，说明高电平时间太长了，强制标记成功捕获了一次(if 内容)
				if( User_Period>=0X3F )   //高电平太长了
				{
					STA|=0x01;                   //强制标记成功捕获了一次
					CAPTURE_VAL=0XFFFF;          //标记距离为0xFFFF(最远)
				}
				else 
					User_Period++;               //进入中断次数+1
			}  
			// 捕获到高电平前什么也不做
		}
		// 成功捕获后什么也不做
	}
}

/**************************************************************************
Function: Pulse width reading interruption of ultrasonic echo
Input   : none
Output  : none
函数功能：超声波回波脉宽读取中断①：捕获中断
入口参数: 无 
返回  值：无
**************************************************************************/
//进入定时器3中断后，在定时器3中断里判断出是捕获中断，然后进入此回调函数
//成功捕获到数据，进入此中断
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)//捕获中断发生时执行
{
	if(htim==&TIM_TimeBaseStructure_IC)
	{
		DEBUG_INFO("进入捕获中断回调函数");
		DEBUG_VAR("STA1", STA); 
		if( (STA&0x01)==0 )   //还未成功捕获
		{
			//开始捕获,第一次捕获上升沿
			DEBUG_INFO("上升沿");
			if( (STA&0x02)==0 )          
			{
				User_Period=0;      //STA清空（进入定时器更新中断次数清空）
				CAPTURE_VAL=0;      //VAL捕获值清空
				STA|=0x02;          //标记捕获到了上升沿

				//配置tim前一定要先关闭tim，配置完以后再使能
				__HAL_TIM_DISABLE(&TIM_TimeBaseStructure_IC);        //关闭定时器
				__HAL_TIM_SET_COUNTER(&TIM_TimeBaseStructure_IC,0);  //计数器CNT置0
				
				TIM_RESET_CAPTUREPOLARITY(&TIM_TimeBaseStructure_IC,GENERAL_TIM_IC_CHANNELx1);   //一定要先清除原来的设置！！
				TIM_SET_CAPTUREPOLARITY(&TIM_TimeBaseStructure_IC,GENERAL_TIM_IC_CHANNELx1,TIM_ICPOLARITY_FALLING);//配置定时器通道设置为下降沿捕获
				
				__HAL_TIM_ENABLE(&TIM_TimeBaseStructure_IC);//使能定时器
			}   
			
			//捕获到一个下降沿，捕获结束   
			else if( (STA&0x02)!=0 )     
			{   
				DEBUG_INFO("下降沿");
				STA|=0x01;    //标记成功捕获到一次高电平脉宽
				CAPTURE_VAL=HAL_TIM_ReadCapturedValue(&TIM_TimeBaseStructure_IC,GENERAL_TIM_IC_CHANNELx1);//获取当前的捕获值
				
				//配置tim前一定要先关闭tim，配置完以后再使能
				__HAL_TIM_DISABLE(&TIM_TimeBaseStructure_IC);
				
				TIM_RESET_CAPTUREPOLARITY(&TIM_TimeBaseStructure_IC,GENERAL_TIM_IC_CHANNELx1);    //一定要先清除原来的设置！！
				TIM_SET_CAPTUREPOLARITY(&TIM_TimeBaseStructure_IC,GENERAL_TIM_IC_CHANNELx1,TIM_ICPOLARITY_RISING);//配置定时器通道设置为上升沿捕获
				
				__HAL_TIM_ENABLE(&TIM_TimeBaseStructure_IC);//使能定时器
			}		
		}
		// 成功捕获后什么也不做
	}
}

/**************************************************************************
Function: Ultrasonic receiving echo function
Input   : none
Output  : none
函数功能：超声波接收回波函数
入口参数: 无 
返回  值：无
**************************************************************************/	 	
void Read_Distane(void)        
{ 
	STA=0;			        //开启捕获			
  
	/* 给触发引脚一个至少 10us 的脉冲信号， */
	HAL_GPIO_WritePin(ultrasonic_GPIO_Port, ultrasonic_Pin, GPIO_PIN_SET);    
	Delay_us();    // 延时一段时间
	HAL_GPIO_WritePin(ultrasonic_GPIO_Port, ultrasonic_Pin, GPIO_PIN_RESET);    

	while( (STA&0x01)==0 );   //如果还未成功捕获，就一直在这个循环里面卡着，直到成功捕获

	// 得到定时器总计数值 = 捕获次数 * 单次捕获花费时间 = 捕获次数 / 单位时间内捕获次数           
	Time = (uint32_t)((float)( User_Period*(GENERAL_TIM_IC_PERIOD+1) + (CAPTURE_VAL+1) ) / ( 72000000/(GENERAL_TIM_IC_PRESCALER+1) ) *1000) ;     // 单位: ms
	// 计算获得距离，公式:s(路程)=v(速度)*t(时间)
	Distance = (340/2) * Time;    // 单位: mm
	
	// 输出计算细节
	DEBUG_VAR("User_Period", User_Period); 
	DEBUG_VAR("CAPTURE_VAL", User_Period); 
	DEBUG_VAR("User_Period*(GENERAL_TIM_IC_PERIOD+1) + (CAPTURE_VAL+1)", User_Period*(GENERAL_TIM_IC_PERIOD+1) + (CAPTURE_VAL+1) ); 
	DEBUG_VAR("72000000/(GENERAL_TIM_IC_PRESCALER+1)", 72000000/(GENERAL_TIM_IC_PRESCALER+1)  ); 
	
	printf("超声波收到数据花费的时间为%dms \n",Time);
	printf("超声波所测得的距离为%dmm \n",Distance);
	printf("\n\n");

//	// 计算原始程序
//	Distance=TIM3CH3_CAPTURE_STA&0X3F;   
//	Distance*=GENERAL_TIM_IC_PERIOD;		//溢出时间总和
//	Distance+=TIM3CH3_CAPTURE_VAL;		  //得到总的高电平时间
//	Distance=Distance*170/1000;         //时间*声速/2（来回） 一个计数0.001ms
	
	STA=0;			        //开启下一次捕获			
}

