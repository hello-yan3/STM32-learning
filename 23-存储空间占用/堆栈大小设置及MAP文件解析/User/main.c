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
#include "./led/bsp_led.h"
#include ".\key\bsp_key.h" 

/*
00. ��ջ��С���ã�����
STM32��F407������ ��ջ����
https://blog.csdn.net/CynalFly/article/details/114626136
https://blog.csdn.net/lighthear/article/details/69568396
��ջ��С��������
https://www.armbbs.cn/forum.php?mod=viewthread&tid=95883
STM32��ջ��uCOS��ջ
https://blog.csdn.net/qq_23922117/article/details/79400051
c++�����ڴ���䡢����ջ����
https://blog.csdn.net/qq_39885372/article/details/103886195

01. �����STM32 MAP�ļ�ǳ��
Program Size: Code=3748 RO-data=360 RW-data=16 ZI-data=1024  
* RO��Read Only ����д������ֻ�����ݣ�RO data���ʹ��루RO code�����������ݣ�ռ�� FLASH �ռ�
* RW��Read Write ����д�������ɶ�д���ݣ�RW data���г�ֵ���Ҳ�Ϊ 0����ռ��FLASH���洢��ֵ���� RAM����д������
* ZI��Zero initialized ����д��������ʼ��Ϊ 0 �����ݣ�ZI data����ռ�� RAM �ռ䡣
* .text���൱�� RO code
* .constdata���൱�� RO data
* .bss���൱�� ZI data
* .data���൱�� RW data

1. �����˸����ļ�֮��ĺ������ù�ϵ(Section Cross References)���磺
main.o(i.main) refers to stm32f1xx_hal.o(i.HAL_Init) for HAL_Init
main.c ������ stm32f1xx_hal.c �� HAL_Init ����
2. �����˹���������δ�����ö���ɾ������������(Removing Unused input sections from the image)���磺
Removing stm32f1xx_ll_utils.o(i.LL_RCC_HSI_IsReady), (16 bytes).
�Ƴ� LL_RCC_HSI_IsRead() ����
3. ӳ����ű�Image Symbol Table���������˱����õĸ������ţ������/���ݣ��ڴ洢���еĴ洢��ַ�����͡���С����Ϣ��
4. ӳ���ڴ�ֲ�ͼ��Memory Map of the image��
5. ӳ�������С��Image component sizes��
����������ӳ�����д��루.o��ռ�ÿռ�Ļ�����Ϣ
      Code (inc. data)   RO Data    RW Data    ZI Data      Debug   Object Name

       148         14          0          0          0       1705   bsp_key.o
       128          8          0          0          0       1066   bsp_led.o
       172         14          0          0          0       1159   main.o
        36          8        304          0       1024        968   startup_stm32f103xe.o
       152         32          0          9          0       6413   stm32f1xx_hal.o
       272         22          0          0          0      29246   stm32f1xx_hal_cortex.o
       874         40          0          0          0       4292   stm32f1xx_hal_gpio.o
         2          0          0          0          0        574   stm32f1xx_hal_msp_template.o
      1708         84          0          0          0       4760   stm32f1xx_hal_rcc.o
        32          0          0          0          0       4486   stm32f1xx_it.o
        88         18         24          4          0     441191   system_stm32f1xx.o

Execution Region RW_IRAM1 (Exec base: 0x20000000, Load base: 0x0800100c, Size: 0x00000410, Max: 0x00010000, ABSOLUTE)
ջ ��ʼ��ַ��0x20000000  SRAM ���ֵ 64K(0x10000)

�� + ջ + ȫ�ֱ������� = RAM

*/

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{
  HAL_Init();
  /* ϵͳʱ�ӳ�ʼ����72MHz */
  SystemClock_Config();
	/* LED �˿ڳ�ʼ�� */
	LED_GPIO_Config();	 
    /*��ʼ������*/
  Key_GPIO_Config();
	/* ��ѯ����״̬��������������תLED */ 
	while(1)                            
	{	   
		if( Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON  )
		{
			/*LED1��ת*/
			LED1_TOGGLE;
		}   
    
    if( Key_Scan(KEY2_GPIO_PORT,KEY2_PIN) == KEY_ON  )
		{
			/*LED2��ת*/
			LED2_TOGGLE;
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
