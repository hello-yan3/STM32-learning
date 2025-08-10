/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   PWM����
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
#include "./key/bsp_key.h"
#include <stdlib.h>
#include "bsp_general_tim.h" 
#include "./tim/bsp_motor.h"

/*
�����Ҫʹ�õ��������·������
VM��������ѹ(4.5V~10V)		VCC���߼���ƽ(3.3V)		GND(0V)
STBY: H ����		    L ����
PWM : H �����ת  		L �������ת(�ƶ�)    ������PWM L H ��ʾ������͵�ƽ/�ߵ�ƽ��
IN1 IN2 ������ͬ���������ת(�ƶ�)   
IN1  IN2    O1  O2
 L    H      L   H   ��ת
 H    L      H   L   ��ת   
O1  O2  ���ӵ�� 
*/

#define Diff_value 100    // ��һ�ΰ���������仯 Diff_value��(30��)

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void) 
{ 
  HAL_Init();
	/* ��ʼ��ϵͳʱ��Ϊ72MHz */
	SystemClock_Config();
	/* ��ʼ��LED */ 
  DEBUG_USART_Config();
	Key_GPIO_Config();
  
  printf("\r\nPWM�����ʵ��\r\n");
    
	TIMx_Configuration();
	
	/* ��ʱ��ͨ��3���PWM */
	HAL_TIM_PWM_Start(&TIM_TimeBaseStructure,GENERAL_TIM_CHANNELx1);   // GENERAL_TIM_CHANNELx1  TIM_CHANNEL_3
	/* ��ʱ��ͨ��4���PWM */
//	HAL_TIM_PWM_Start(&TIM_TimeBaseStructure,GENERAL_TIM_CHANNELx2);   // GENERAL_TIM_CHANNELx2  TIM_CHANNEL_4
	
	int16_t Speed = 0;   // �����ٶ�
	int16_t Get_Speed = 0;  // ����ٶ�
	
  /* ����ѭ�� */
  while (1)
  {
		/* ����1���£��Ƕ�����30�� */
		/* ����Ƿ��а������� */
		if(Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON )  
		{	 
			Speed += Diff_value;
			if(Speed > 1000)    Speed = -1000;   
			// �Ƕȵ���
			Motor_SetSpeed(Speed);
		}
		
		/* ����2���£��Ƕȼ���30�� */
		/* ����Ƿ��а������� */
		if(Key_Scan(KEY2_GPIO_PORT,KEY2_PIN) == KEY_ON )  
		{	 
			Speed -= Diff_value;
			if(Speed < -1000)      Speed = 1000; 
			// �Ƕȵ���
			Motor_SetSpeed(Speed);
		}
		
		// ��ô�ʱCCR�Ĵ���ֵ
		Get_Speed = __HAL_TIM_GET_COMPARE(&TIM_TimeBaseStructure, GENERAL_TIM_CHANNELx1);   // GENERAL_TIM_CHANNELx1  TIM_CHANNEL_3
		printf("��ʱSpeedΪ%d \n",Speed);
		printf("��ʱCH3��CCRΪ%d \n",Get_Speed);
		printf("��ʱCH3��ռ�ձ�Ϊ%f \n",((float)Get_Speed/(GENERAL_TIM_PERIOD+1)));
		printf("\n");
		HAL_Delay(250);
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
