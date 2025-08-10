#include "bsp_wwdg.h"  
#include "./led/bsp_led.h"  

WWDG_HandleTypeDef WWDG_Handle;

// WWDG �ж����ȼ���ʼ��
static void WWDG_NVIC_Config(void)
{
  HAL_NVIC_SetPriority(WWDG_IRQn,0,0);
	HAL_NVIC_EnableIRQ(WWDG_IRQn);
}

/* brief��
 * ���ڿ��Ź������ϴ���ֵ���´���ֵ��
 * ���������趨�ĳ�ʼֵ��ʼ���µݼ������������´���ֵ�м�ι��
 * ��ǰι�����Ӻ�ι��������и�λ
 * WWDG ���ú���
 * WWDG_COUNTER  ���ݼ���ʱ���ĳ�ʼֵ�� ȡֵ��ΧΪ��0x7f~0x40
 * WWDG_WINDOW   ���ϴ���ֵ��ȡֵ��ΧΪ��0x7f~0x40
 * WWDG_PRESCALER��Ԥ��Ƶ��ֵ��ȡֵ������
 *      @arg WWDG_Prescaler_1: WWDG counter clock frequency = (PCLK1(36MHZ)/4096)/1
 *      @arg WWDG_Prescaler_2: WWDG counter clock frequency = (PCLK1(36MHZ)/4096)/2
 *      @arg WWDG_Prescaler_4: WWDG counter clock frequency = (PCLK1(36MHZ)/4096)/4
 *      @arg WWDG_Prescaler_8: WWDG counter clock frequency = (PCLK1(36MHZ)/4096)/8
 * ��ʱʱ����㣺1 / WWDG counter clock frequency 
 * һ������������ WWDG_PRESCALER Ԥ��Ƶ�� ���
 * ʱ�����ӣ�  Period = 1/((PCLK1(36MHZ)/4096)/WWDG_PRESCALER) = 1/((PCLK1(36MHZ)/4096)/8) = 0.91ms
 * �´��ڼ�ʱ��(WWDG_COUNTER - 0x40) * Period = (0x7F - 0x40) * 0.91ms
 * �ϴ��ڼ�ʱ��(WWDG_COUNTER - WWDG_WINDOW) * Period = (0x7F - 0x50) * 0.91ms
 */
void WWDG_Config()
{	
	// ���� WWDG ʱ��
	__HAL_RCC_WWDG_CLK_ENABLE();
	
	// ����WWDG������Ĵ�������ַ
	WWDG_Handle.Instance = WWDG;
	
	// ����Ԥ��Ƶ��ֵ
	WWDG_Handle.Init.Prescaler = WWDG_PRESCALER;
	// �����ϴ���ֵ
	WWDG_Handle.Init.Window  = WWDG_WINDOW;	
	// ���ü�������ֵ
	WWDG_Handle.Init.Counter = WWDG_COUNTER;
	
	// ʹ����ǰ�����ж�
	WWDG_Handle.Init.EWIMode = WWDG_EWI_ENABLE;
	// ��ʼ��WWDG
	HAL_WWDG_Init(&WWDG_Handle);	
	
	// ����WWDG�ж����ȼ�
	WWDG_NVIC_Config();
}

/* ���ڿ��Ź��ص����� */
void HAL_WWDG_EarlyWakeupCallback(WWDG_HandleTypeDef* hwwdg)
{
	//�Ƶ���������LEDֻ��ʾ���ԵĲ�����
	//����ʹ�õ�ʱ������Ӧ����������Ҫ������
	LED_YELLOW; 
}


/*********************************************END OF FILE**********************/
