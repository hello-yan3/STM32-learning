#include "./adc/bsp_adc.h"

// ADC1 ͬʱת����ͨ��
__IO uint16_t ADC_ConvertedValue[NOFCHANEL]={0};
DMA_HandleTypeDef hdma_adcx;
ADC_HandleTypeDef ADC_Handle;
ADC_ChannelConfTypeDef ADC_Config;

static void Rheostat_ADC_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RHEOSTAT_ADC_CLK_ENABLE();         // __HAL_RCC_ADC1_CLK_ENABLE(); 
    // ʹ�� GPIO ʱ��
    RHEOSTAT_ADC_GPIO_CLK_ENABLE();    // __HAL_RCC_GPIOC_CLK_ENABLE()
          
    // ���� IO
    GPIO_InitStructure.Pin =ADC_PIN1|     // GPIO_PIN_0
                            ADC_PIN2|     // GPIO_PIN_1
                            ADC_PIN3|     // GPIO_PIN_2
                            ADC_PIN4|     // GPIO_PIN_3
                            ADC_PIN5|     // GPIO_PIN_4
                            ADC_PIN6;     // GPIO_PIN_5
    GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;	 // ģ������
    GPIO_InitStructure.Pull = GPIO_NOPULL ;      //������������
    HAL_GPIO_Init(RHEOSTAT_ADC_GPIO_PORT, &GPIO_InitStructure);	   // GPIOC	
}

static void Rheostat_ADC_Mode_Config(void)
{
	// ------------------DMA Init �ṹ����� ��ʼ��--------------------------
	// ����DMAʱ��
	RHEOSTAT_ADC_DMA_CLK_ENABLE();     // __HAL_RCC_DMA1_CLK_ENABLE();
	// ���ݴ���ͨ��
	hdma_adcx.Instance = RHEOSTAT_ADC_DMA_STREAM;  // DMA1_Channel1

	hdma_adcx.Init.Direction=DMA_PERIPH_TO_MEMORY;              //���赽�洢��
	hdma_adcx.Init.PeriphInc=DMA_PINC_DISABLE;                  //���������ģʽ
	hdma_adcx.Init.MemInc=DMA_MINC_ENABLE;                      //�洢������ģʽ 
	hdma_adcx.Init.PeriphDataAlignment=DMA_PDATAALIGN_HALFWORD; //�������ݳ���:16λ
	hdma_adcx.Init.MemDataAlignment=DMA_MDATAALIGN_HALFWORD;    //�洢�����ݳ���:16λ
	hdma_adcx.Init.Mode= DMA_CIRCULAR;                          //DMAѭ������
	hdma_adcx.Init.Priority=DMA_PRIORITY_MEDIUM;                //�е����ȼ�

	HAL_DMA_Init(&hdma_adcx); 

	__HAL_LINKDMA( &ADC_Handle,DMA_Handle,hdma_adcx);

	//---------------------------------------------------------------------------
	RCC_PeriphCLKInitTypeDef ADC_CLKInit;
	// ����ADCʱ��
	ADC_CLKInit.PeriphClockSelection=RCC_PERIPHCLK_ADC;	   //ADC����ʱ��
	ADC_CLKInit.AdcClockSelection=RCC_ADCPCLK2_DIV6;			 //��Ƶ����6ʱ��Ϊ72M/6=12MHz
	HAL_RCCEx_PeriphCLKConfig(&ADC_CLKInit);					     //����ADCʱ��

	//-------------------ADC Init �ṹ����� ��ʼ��-------------------------------
	ADC_Handle.Instance=RHEOSTAT_ADC;                           //ADC1
	
	ADC_Handle.Init.DataAlign=ADC_DATAALIGN_RIGHT;              //�Ҷ���
	ADC_Handle.Init.ScanConvMode=ENABLE;                        //ɨ��ģʽ
	ADC_Handle.Init.ContinuousConvMode=ENABLE;                  //����ת��
	ADC_Handle.Init.DiscontinuousConvMode=DISABLE;              //��ֹ����������ģʽ
	ADC_Handle.Init.NbrOfDiscConversion=0;                      //����������ͨ����Ϊ0
	ADC_Handle.Init.NbrOfConversion=NOFCHANEL	;                 //6��ת���ڹ��������У�ת��6��ͨ����
	ADC_Handle.Init.ExternalTrigConv=ADC_SOFTWARE_START;        //�������
	
	HAL_ADC_Init(&ADC_Handle);                                

	//---------------------------------------------------------------------------
  // ADCʹ�����ɸ� ADC_CLK ���ڶ�����ĵ�ѹ���в�������ǰ����Ϊ����ʱ�� 55.5 ADCʱ������
	// ת��ʱ����㣺
	// Tconv = ����ʱ�� + 12.5 ������ = 1.5 + 12.5 = 14��ADCCLK = 12MHz
	// ת��ʱ�� T = 14/12M = 1.667us
	ADC_Config.SamplingTime = ADC_SAMPLETIME_1CYCLE_5; 
	
	ADC_Config.Channel      = ADC_CHANNEL1;  
	ADC_Config.Rank         = 1;

	// ���� ADC ͨ��1ת��˳��Ϊ1����һ��ת��������ʱ��Ϊ1.5��ʱ������
	HAL_ADC_ConfigChannel(&ADC_Handle, &ADC_Config);

	//---------------------------------------------------------------------------
	ADC_Config.Channel      = ADC_CHANNEL2;
	ADC_Config.Rank         = 2;
	// ���� ADC ͨ��2ת��˳��Ϊ2���ڶ���ת��������ʱ��Ϊ1.5��ʱ������
	HAL_ADC_ConfigChannel(&ADC_Handle, &ADC_Config);   

	//---------------------------------------------------------------------------
	ADC_Config.Channel      = ADC_CHANNEL3;
	ADC_Config.Rank         = 3;
	// ���� ADC ͨ��3ת��˳��Ϊ3��������ת��������ʱ��Ϊ1.5��ʱ������
	HAL_ADC_ConfigChannel(&ADC_Handle, &ADC_Config);   
	//---------------------------------------------------------------------------
	ADC_Config.Channel      = ADC_CHANNEL4;
	ADC_Config.Rank         = 4;
	// ���� ADC ͨ��4ת��˳��Ϊ4�����ĸ�ת��������ʱ��Ϊ1.5��ʱ������
	HAL_ADC_ConfigChannel(&ADC_Handle, &ADC_Config);   
	//---------------------------------------------------------------------------
	ADC_Config.Channel      = ADC_CHANNEL5;
	ADC_Config.Rank         = 5;
	// ���� ADC ͨ��5ת��˳��Ϊ5�������ת��������ʱ��Ϊ1.5��ʱ������
	HAL_ADC_ConfigChannel(&ADC_Handle, &ADC_Config);   
	//---------------------------------------------------------------------------
	ADC_Config.Channel      = ADC_CHANNEL6;
	ADC_Config.Rank         = 6;
	// ���� ADC ͨ��6ת��˳��Ϊ6��������ת��������ʱ��Ϊ1.5��ʱ������
	HAL_ADC_ConfigChannel(&ADC_Handle, &ADC_Config);       
	
	// HAL_ADC_Start_DMA(&ADC_Handle, (uint32_t*)&ADC_ConvertedValue, 6);    // ����д main ����
}



void Rheostat_Init(void)
{
	Rheostat_ADC_GPIO_Config();
	Rheostat_ADC_Mode_Config();
}

