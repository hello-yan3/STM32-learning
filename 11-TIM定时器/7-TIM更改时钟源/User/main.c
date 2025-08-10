/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ����lcd
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
#include <stdlib.h>
#include "./tim/bsp_general_tim.h"
#include "./led/bsp_led.h"

/**
https://blog.csdn.net/shileiwu0505/article/details/124136792[1]
https://zhuanlan.zhihu.com/p/74620029[2]
https://blog.csdn.net/u010650845/article/details/81781670[3]
https://blog.csdn.net/shileiwu0505/article/details/124070312[4]
https://blog.csdn.net/u014217137/article/details/121592021[5]


�����źŹ������ࣺTRGI(�ⲿ���뵽����ʱ�����ź�) / TRGO(��ʱ�������������ʱ��������Ĵ����ź�)[2]

����Զ�ʱ������Դ����һ���Ľ��⣺
��ʱ������8�ִ���Դ��[1]
ITR0        �ڲ�����0
ITR1        �ڲ�����1
ITR2        �ڲ�����2
ITR3        �ڲ�����3
TI1F_ED     TI1���ؼ����
TI1FP1      �˲���Ķ�ʱ������1
TI2FP2      �˲���Ķ�ʱ������2
ETR1        �ⲿ��������

�����ࣺ�ڲ�ʱ��Դ��֮ǰ���г����ʹ�����֣�����������

��һ�ࣺ���Զ�ʱ����������ͨ��1��ͨ��2�������źţ���������ѡ����˲��Ժ����ɵĴ����źţ����ӵ���ģʽ���������������Ƽ������Ĺ���(TI1F...)[2]
�ڶ��࣬�������ⲿ������[ETR��]��������ѡ�񡢷�Ƶ���˲��Ժ���źţ�������������ѡ���������ӵ���ģʽ������(ETR1)[2]
�����࣬����������ʱ���Ĵ�������źţ�ͨ���ڲ���·���ӵ�����ʱ���Ĵ�����������������ӵ���ģʽ������(ITR0~ITR3)[2]

�������Ա���ʱ���ⲿ����һ�ഥ�������źţ������и���ͬ�ص㣬���Ƕ�Ҫ������������ѡ���������ӵ���ģʽ��������
�Ӷ�ʹ�ü������Ĺ����ܵ���ģʽ�������Ŀ��ƻ�Ӱ�죬������һ�㣬��ʱ�������ڴ�ģʽ��[2]

����������������������������������������������������������������������������������������

�����Ͻ���STM32ͨ�û�߼���ʱ���Ĵ�ģʽ�����¼��֣�[2][4]
1����λģʽ ��Reset mode������Ч���������źų���ʱ�����������ᱻ��λ��ͬʱ������������¼��ʹ����¼���
2������ģʽ ��Trigger mode������Ч���������źų���ʱ���Ὣ��������δʹ��״̬�ļ�����ʹ�ܼ���ü�������ʼ������ͬʱ������������¼���
3���ſ�ģʽ ��Gate mode����ʱ�����ݴ��������źŵĵ�ƽ��������ֹͣ�������ļ������ڼ�����������ֹͣʱ������������¼�����λ��ر�־λ,TIF@TIMx_SR��
4���ⲿʱ��ģʽ1 ��External clock mode 1���ڽ��ܼ�����ʱ��Դʱ���������ⲿʱ��ģʽ1������������ʱ������TI1��TI2���������壬���ӵ���ģʽ��������Ϊ�������ṩʱ�ӡ�
Ҳ����˵������������ʱ�����Դ����ź�ʱ����������һ�������ⲿʱ��ģʽ1��ģʽ��
��Ȼ����������źžͲ������������Զ�ʱ��ͨ��TI1/TI2�������źţ��������������ᵽ�����ഥ�������źŵ���һ�֣��ȷ�����������ʱ���Ĵ�������źţ���������ETR�ŵĴ��������źš�
5��������ģʽ ��encode mode��(��)

����ģʽ�ĵ����ص��Ǵ����źſ���ʹ�ܼ������Ĺ���������ģʽ�ļ������Ĺ�����Ҫ���ʹ�ܡ�[2]
�ⲿʱ��ģʽ1��ģʽ�Ƚ��ر𣬵���������ʱ��Դ���Դ����ź�ʱ����ʱ��ʱ���͹������ⲿʱ��1��ģʽ����ʱ�����źŰ�����˫��ɫ���������ź���ʱ���źš�

����ʱ���Ĺ����ܵ����������źŵ�Ӱ������ʱ�������ǹ����ڴ�ģʽ�����д�ģʽ�����ж��֣����ĳ��ʱ���ܲ��������������Ϊ������ʱ���Ĵ��������ź�ʱ����ʱ�ö�ʱ�����ǹ�������ģʽ��
���ĳ��ʱ���Ĺ����������������źŵ�Ӱ�����ƣ�ͬʱ������������ź�Ӱ�����Ʊ�ĴӶ�ʱ���������Ǵ�������˫��ɫģʽ��[2]

����������������������������������������������������������������������������������������

�����Ͻ������涨ʱ���������·�ʽ������������ź�[TRGO]��[2]

1�������ʽ�Զ�ʱ����λ����λUG@TIMx_EGR
2��ʹ�ܼ���������λCEN@TIMx_CR1
3����ʱ�������¼� (����)
4����ʱ�����񡢱Ƚ��¼�
5�������ͨ���м�ο��ź� OCxREF

����������������������������������������������������������������������������������������
ITR0~ITR3 �ο����������[1]
ETR �ο����������[5]

���䣺CubeMX��
Slave Mode:       sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
Trigger Source:   sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;

����������������������������������������������������������������������������������������
������ΪETR�ο�����ʵ�ֹ��ܣ�
ÿ����һ�ΰ�����ETR�������壬ʵ�ּ���
*/


uint32_t time = 0; // ms ��ʱ���� 

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
	LED_GPIO_Config();
  /* ��ʼ��������ʱ����ʱ��1s����һ���ж� */
	TIM_Mode_Config();
  /* ���ڳ�ʼ�� */
  DEBUG_USART_Config();
	/* ���ж�ģʽ��������ʱ�� */
  HAL_TIM_Base_Start_IT(&TIM_TimeBaseStructure);
	
	while(1)
	{  
    printf("��ǰ������ֵΪ: %d\n", __HAL_TIM_GET_COUNTER(&TIM_TimeBaseStructure) );
    printf("��ǰ�����жϴ���Ϊ: %d\n\n", time );
    HAL_Delay(1000);
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
