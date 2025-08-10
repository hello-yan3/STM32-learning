/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   测试led
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 F103-霸道 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx.h"
#include "./usart/bsp_debug_usart.h"
#include "./mpu6050/mpu6050.h"
#include "./led/bsp_led.h"  
#include "./i2c/bsp_i2c.h"
#include "./exti/bsp_exti.h"
#include "MPU6050DMP.h"
#include "./basic_tim/bsp_basic_tim.h"

/*
1. 官方DMP：
	 移植教程参考：https://www.bilibili.com/video/BV13N4y1y7Js  && （野火移植教程参考）
	 代码参考：https://www.bilibili.com/video/BV13N4y1y7Js  && （野火移植教程程序示例）
2. 卡尔曼滤波：
   算法教程：https://www.bilibili.com/video/BV1sL411F7fu
   代码参考：https://www.bilibili.com/video/BV15f4y1h7GM
3. 互补滤波
	 参考：https://www.bilibili.com/read/cv5772853	  && （平衡小车PDF教程）
	 代码参考：平衡小车
*/

uint8_t WAY = 1;     //获取角度的算法，1：四元数DMP   2：卡尔曼滤波   3：互补滤波 
__IO uint16_t time = 0;      // ms 计时变量 
uint16_t continue_time = 0;

/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{
  HAL_Init();
	SystemClock_Config();
	
	/* LED 端口初始化 */
	LED_GPIO_Config();
	/* 串口通信初始化 */
	DEBUG_USART_Config();
	/* 基本定时器初始化：1ms中断一次，中断后time++ */
  BASIC_TIMx_Init();
	//I2C初始化
	I2cMaster_Init();
	//PF7外部中断初始化
	EXTI_Pxy_Config();
	
	// MPU6050 DMP初始化，
	// DMP库中，除了姿态解算，INT引脚中断功能也包含在其中；
	// MPU6050 不使用DMP初始化，会对INT定时造成一定影响
	MPU6050_DMP_Init();
	MPU6050_Init();
	
	//MPU6050 DMP 初始化
	if(WAY==1)	 MPU6050_DMP_Init();
	//MPU6050硬件初始化
	else     		 MPU6050_Init();
	
	printf("ID: 0x%02x\n\n",MPU6050ReadID());
	
	time = 0;
	continue_time = 0;
	
	while(1)
	{
		// 若读取到数据
		if(EXTI_flag==1)
		{
			// 如果使用DMP解析数据
			if(WAY==1)  
			{
				MPU6050_DMP_Get_Data();
			}
			// 如果不使用DMP解析数据，通过滤波方式去除高频噪声
			else
			{
				MPU6050_Angle_Calculate();
			}			
			printf("加速度原始数据：	X方向 = %6d, Y方向 = %6d, Z方向 = %6d \n",accx, accy, accz);
			printf("角速度原始数据：	X方向 = %6d, Y方向 = %6d, Z方向 = %6d \n",gyrox, gyroy, gyroz);
			printf("俯仰角pitch = %f, 横滚角roll = %f, 偏航角yaw = %f \n",Angle_X_Final, Angle_Y_Final, Angle_Z_Final);
			printf("两次输出经过的总时间为：%d \n",continue_time);
			printf("\n");
			
			// 清空计数位
			EXTI_flag = 0;
			// 计数周期清零
			continue_time = 0;
		}
	}
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 72000000
  *            HCLK(Hz)                       = 72000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 2
  *            APB2 Prescaler                 = 1
  *            HSE Frequency(Hz)              = 8000000
  *            HSE PREDIV1                    = 2
  *            PLLMUL                         = 9
  *            Flash Latency(WS)              = 0
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef clkinitstruct = {0};
  RCC_OscInitTypeDef oscinitstruct = {0};
  
  /* Enable HSE Oscillator and activate PLL with HSE as source */
  oscinitstruct.OscillatorType  = RCC_OSCILLATORTYPE_HSE;
  oscinitstruct.HSEState        = RCC_HSE_ON;
  oscinitstruct.HSEPredivValue  = RCC_HSE_PREDIV_DIV1;
  oscinitstruct.PLL.PLLState    = RCC_PLL_ON;
  oscinitstruct.PLL.PLLSource   = RCC_PLLSOURCE_HSE;
  oscinitstruct.PLL.PLLMUL      = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&oscinitstruct)!= HAL_OK)
  {
    /* Initialization Error */
    while(1); 
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  clkinitstruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  clkinitstruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  clkinitstruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  clkinitstruct.APB2CLKDivider = RCC_HCLK_DIV1;
  clkinitstruct.APB1CLKDivider = RCC_HCLK_DIV2;  
  if (HAL_RCC_ClockConfig(&clkinitstruct, FLASH_LATENCY_2)!= HAL_OK)
  {
    /* Initialization Error */
    while(1); 
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
