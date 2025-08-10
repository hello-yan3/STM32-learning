/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2020-xx-xx
  * @brief   TIM���߼���ʱ���������
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103-���� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdlib.h>
#include "./usart/bsp_debug_usart.h"
#include ".\key\bsp_key.h" 
#include "./tim/bsp_general_tim_IC.h"
#include "./ultrasonic/ultrasonic.h"


/*
���벶��-�ж��޸Ĳ����򡪡������ڲ����ߵ�ƽ/�͵�ƽ����ʱ��

ʵ������
����ͨ��TIM���벶��ʵ�ֳ�������๦��
��������Trig: PC6
��Ӧ����Echo: PA7
ģ���ͺ�Ϊ HC-SR04��
����ԭ��
����������һ������ 10us �������źţ���ʱģ��ᷢ�����������ز����Ż��ɵ͵�ƽ��Ϊ�ߵ�ƽ��
��ģ����ܵ��������ĳ��������ز��ھͻ�Ӹߵ�ƽ���䵽�͵�ƽ��
ͨ�������ߵ�ƽ�ĳ���ʱ�䣬ͨ�����㹫ʽ�Ϳ���������롣

*/

/*
https://zhuanlan.zhihu.com/p/145635309
https://blog.csdn.net/as480133937/article/details/99407485
*/

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

  /* ��ʼ������ */
  DEBUG_USART_Config();
  /* ��ʼ������ */
	Key_GPIO_Config();
	
  printf("\r\nPWM���벶��ʵ��\r\n");
	MX_GPIO_Init();
	TIMx_IC_Configuration();

	// ������ʱ���ж�
	HAL_TIM_IC_Start_IT(&TIM_TimeBaseStructure_IC,GENERAL_TIM_IC_CHANNELx1);    // GENERAL_TIM_IC_CHANNELx1  TIM_CHANNEL_1
  HAL_TIM_Base_Start_IT(&TIM_TimeBaseStructure_IC);         // �����ж϶�Ҫ��                  

  while(1)
  {   
		/* ����1���£����г�������� */
		if(Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON )  
		{	 
			Read_Distane();  
		}

		HAL_Delay(100);
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
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
