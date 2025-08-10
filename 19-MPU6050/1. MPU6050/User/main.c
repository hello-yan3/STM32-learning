/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ����led
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103-�Ե� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
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
1. �ٷ�DMP��
	 ��ֲ�̳̲ο���https://www.bilibili.com/video/BV13N4y1y7Js  && ��Ұ����ֲ�̳̲ο���
	 ����ο���https://www.bilibili.com/video/BV13N4y1y7Js  && ��Ұ����ֲ�̳̳���ʾ����
2. �������˲���
   �㷨�̳̣�https://www.bilibili.com/video/BV1sL411F7fu
   ����ο���https://www.bilibili.com/video/BV15f4y1h7GM
3. �����˲�
	 �ο���https://www.bilibili.com/read/cv5772853	  && ��ƽ��С��PDF�̳̣�
	 ����ο���ƽ��С��
*/

uint8_t WAY = 1;     //��ȡ�Ƕȵ��㷨��1����Ԫ��DMP   2���������˲�   3�������˲� 
__IO uint16_t time = 0;      // ms ��ʱ���� 
uint16_t continue_time = 0;

/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{
  HAL_Init();
	SystemClock_Config();
	
	/* LED �˿ڳ�ʼ�� */
	LED_GPIO_Config();
	/* ����ͨ�ų�ʼ�� */
	DEBUG_USART_Config();
	/* ������ʱ����ʼ����1ms�ж�һ�Σ��жϺ�time++ */
  BASIC_TIMx_Init();
	//I2C��ʼ��
	I2cMaster_Init();
	//PF7�ⲿ�жϳ�ʼ��
	EXTI_Pxy_Config();
	
	// MPU6050 DMP��ʼ����
	// DMP���У�������̬���㣬INT�����жϹ���Ҳ���������У�
	// MPU6050 ��ʹ��DMP��ʼ�������INT��ʱ���һ��Ӱ��
	MPU6050_DMP_Init();
	MPU6050_Init();
	
	//MPU6050 DMP ��ʼ��
	if(WAY==1)	 MPU6050_DMP_Init();
	//MPU6050Ӳ����ʼ��
	else     		 MPU6050_Init();
	
	printf("ID: 0x%02x\n\n",MPU6050ReadID());
	
	time = 0;
	continue_time = 0;
	
	while(1)
	{
		// ����ȡ������
		if(EXTI_flag==1)
		{
			// ���ʹ��DMP��������
			if(WAY==1)  
			{
				MPU6050_DMP_Get_Data();
			}
			// �����ʹ��DMP�������ݣ�ͨ���˲���ʽȥ����Ƶ����
			else
			{
				MPU6050_Angle_Calculate();
			}			
			printf("���ٶ�ԭʼ���ݣ�	X���� = %6d, Y���� = %6d, Z���� = %6d \n",accx, accy, accz);
			printf("���ٶ�ԭʼ���ݣ�	X���� = %6d, Y���� = %6d, Z���� = %6d \n",gyrox, gyroy, gyroz);
			printf("������pitch = %f, �����roll = %f, ƫ����yaw = %f \n",Angle_X_Final, Angle_Y_Final, Angle_Z_Final);
			printf("���������������ʱ��Ϊ��%d \n",continue_time);
			printf("\n");
			
			// ��ռ���λ
			EXTI_flag = 0;
			// ������������
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
