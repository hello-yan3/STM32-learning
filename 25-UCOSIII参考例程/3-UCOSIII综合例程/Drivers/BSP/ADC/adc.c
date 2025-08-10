#include "./BSP/ADC/adc.h"

ADC_HandleTypeDef ADC_Handle;



// RHEOSTAT  ������
static void Rheostat_ADC_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RHEOSTAT_ADC_CLK_ENABLE();         // __HAL_RCC_ADC1_CLK_ENABLE(); 
    // ʹ�� GPIO ʱ��
    RHEOSTAT_ADC_GPIO_CLK_ENABLE();    // __HAL_RCC_GPIOC_CLK_ENABLE()
          
    // ���� IO
    GPIO_InitStructure.Pin = RHEOSTAT_ADC_GPIO_PIN;   // GPIO_PIN_1
    GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;	      // ģ������
    GPIO_InitStructure.Pull = GPIO_NOPULL ;           // ������������
    HAL_GPIO_Init(RHEOSTAT_ADC_GPIO_PORT, &GPIO_InitStructure);		   // GPIOC
}

static void Rheostat_ADC_Mode_Config(void)
{
    //---------------------------------------------------------------------------
    RCC_PeriphCLKInitTypeDef ADC_CLKInit;
    // ����ADCʱ��
    ADC_CLKInit.PeriphClockSelection=RCC_PERIPHCLK_ADC;			   //ADC����ʱ��
    ADC_CLKInit.AdcClockSelection=RCC_ADCPCLK2_DIV6;			     //��Ƶ����6ʱ��Ϊ72M/6=12MHz
    HAL_RCCEx_PeriphCLKConfig(&ADC_CLKInit);					         //����ADCʱ��
   
  	//-------------------ADC Init �ṹ����� ��ʼ��-------------------------------
    ADC_Handle.Instance=RHEOSTAT_ADC;                          //ADC1
		
    ADC_Handle.Init.DataAlign=ADC_DATAALIGN_RIGHT;             //�Ҷ���
    ADC_Handle.Init.ScanConvMode=ADC_SCAN_DISABLE;             //��ɨ��ģʽ
    ADC_Handle.Init.ContinuousConvMode=DISABLE;                //����ת��
		ADC_Handle.Init.DiscontinuousConvMode=DISABLE;             //��ֹ������ת��ģʽ
    ADC_Handle.Init.NbrOfDiscConversion=0;                     //����������ͨ����Ϊ0
    ADC_Handle.Init.NbrOfConversion=1;                         //1��ת���ڹ��������У�ֻת��1��ͨ����
    ADC_Handle.Init.ExternalTrigConv=ADC_SOFTWARE_START;       //�������
    
		HAL_ADC_Init(&ADC_Handle);                                 //��ʼ�� 
 
    //---------------------------------------------------------------------------
		ADC_ChannelConfTypeDef ADC_Config;
    ADC_Config.Channel      = RHEOSTAT_ADC_CHANNEL;            //ADC_CHANNEL_11
    ADC_Config.Rank         = 1;                               //ADCͨ��1 ���1 
    // ADCʹ�����ɸ� ADC_CLK ���ڶ�����ĵ�ѹ���в�������ǰ����Ϊ����ʱ�� 55.5 ADCʱ������
		// ת��ʱ����㣺
		// Tconv = ����ʱ�� + 12.5 ������ = 55.5 + 12.5 = 68��ADCCLK = 12MHz
		// ת��ʱ�� T = 68/12M = 5.667us
    ADC_Config.SamplingTime = ADC_SAMPLETIME_55CYCLES_5 ;      
    // ���� ADC ͨ��ת��˳��Ϊ1����һ��ת��������ʱ��Ϊ55.5��ʱ������
    HAL_ADC_ConfigChannel(&ADC_Handle, &ADC_Config);
}

void Rheostat_Init(void)
{
	Rheostat_ADC_GPIO_Config();
	Rheostat_ADC_Mode_Config();
}



/**************************************************************************
Function: Read battery voltage
Input   : none
Output  : Battery voltage��MV��
�������ܣ���ȡAD�ɼ����
��ڲ���: ��
����  ֵ��AD�ɼ����
**************************************************************************/
int Get_battery_volt(void)  
{ 
   	int Volt;
    HAL_ADC_Start(&ADC_Handle);                                //����ADC
    HAL_ADC_PollForConversion(&ADC_Handle,10);                 //����ת��
	  Volt = HAL_ADC_GetValue(&ADC_Handle);	                     //���ת�����
	  return Volt;	                                      	     //�������һ��ADC1�������ת�����
}


