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


/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{
	 
	int16_t Acel[3];
	int16_t Gyro[3];
	float Temp;
	
  HAL_Init();
	SystemClock_Config();
	
	/* LED �˿ڳ�ʼ�� */
	LED_GPIO_Config();
	/* ����ͨ�ų�ʼ�� */
	DEBUG_USART_Config();
	//I2C��ʼ��
	I2cMaster_Init();
	//MPU6050��ʼ��
	MPU6050_Init();
	printf("%d",MPU6050ReadID());
	//���MPU6050
	if (MPU6050ReadID() == 1)
	{	
		while(1)
		{
				LED2_TOGGLE;
				MPU6050ReadAcc(Acel);
				printf("���ٶȣ�%8d%8d%8d",Acel[0],Acel[1],Acel[2]);
				MPU6050ReadGyro(Gyro);
				printf("    ������%8d%8d%8d",Gyro[0],Gyro[1],Gyro[2]);
				MPU6050_ReturnTemp(&Temp);
				printf("    �¶�%8.2f\r\n",Temp);
				HAL_Delay(1000);
			/*
				������̬����̳̣�
			  https://blog.csdn.net/weixin_44821644/article/details/116893943
			  https://blog.csdn.net/black_sneak/article/details/131253645
			  https://blog.csdn.net/qq_45801336/article/details/115611036
				https://zhuanlan.zhihu.com/p/165156300
				https://blog.csdn.net/yeppchan/article/details/83992434
				�ܶ࡭����ʱ����������
			*/
		}
	}
	else
	{
		printf("\r\nû�м�⵽MPU6050��������\r\n");
		LED_RED; 
	
		while(1);	
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
