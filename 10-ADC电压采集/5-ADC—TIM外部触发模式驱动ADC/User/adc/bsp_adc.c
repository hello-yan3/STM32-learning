#include "./adc/bsp_adc.h"
#include "./tim/bsp_general_tim.h"
#include "./usart/bsp_debug_usart.h"

__IO uint16_t ADC_ConvertedValue;
DMA_HandleTypeDef hdma_adcx;
ADC_HandleTypeDef ADC_Handle;
ADC_ChannelConfTypeDef ADC_Config;


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
    // ------------------DMA Init �ṹ����� ��ʼ��--------------------------
    // ����DMAʱ��
    RHEOSTAT_ADC_DMA_CLK_ENABLE();     // __HAL_RCC_DMA1_CLK_ENABLE();
    // ���ݴ���ͨ��
    hdma_adcx.Instance = RHEOSTAT_ADC_DMA_STREAM;    // DMA1_Channel1
  
    hdma_adcx.Init.Direction=DMA_PERIPH_TO_MEMORY;             //���赽�洢��
    hdma_adcx.Init.PeriphInc=DMA_PINC_DISABLE;                 //���������ģʽ
    hdma_adcx.Init.MemInc=DMA_MINC_ENABLE;                     //�洢������ģʽ 
    hdma_adcx.Init.PeriphDataAlignment=DMA_PDATAALIGN_HALFWORD;//�������ݳ���:16λ
    hdma_adcx.Init.MemDataAlignment=DMA_MDATAALIGN_HALFWORD;   //�洢�����ݳ���:16λ
    hdma_adcx.Init.Mode= DMA_CIRCULAR;                         //DMAѭ������
    hdma_adcx.Init.Priority=DMA_PRIORITY_MEDIUM;               //�е����ȼ�

    //��ʼ��DMA�������൱��һ����Ĺܵ����ܵ������кܶ�ͨ��
    HAL_DMA_Init(&hdma_adcx); 

    __HAL_LINKDMA( &ADC_Handle,DMA_Handle,hdma_adcx);
  
		HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
	
    //---------------------------------------------------------------------------
    RCC_PeriphCLKInitTypeDef ADC_CLKInit;
    // ����ADCʱ��
    ADC_CLKInit.PeriphClockSelection=RCC_PERIPHCLK_ADC;			   //ADC����ʱ��
    ADC_CLKInit.AdcClockSelection=RCC_ADCPCLK2_DIV6;			     //��Ƶ����6ʱ��Ϊ72M/6=12MHz
    HAL_RCCEx_PeriphCLKConfig(&ADC_CLKInit);					         //����ADCʱ��
   
  	//-------------------ADC Init �ṹ����� ��ʼ��-------------------------------
    ADC_Handle.Instance=RHEOSTAT_ADC;                          //ADC1
		
    ADC_Handle.Init.DataAlign=ADC_DATAALIGN_RIGHT;             //�Ҷ���
    ADC_Handle.Init.ScanConvMode=DISABLE;                      //��ɨ��ģʽ
    ADC_Handle.Init.ContinuousConvMode=DISABLE;                //��ֹ����ת��
		ADC_Handle.Init.DiscontinuousConvMode=DISABLE;             //��ֹ������ת��ģʽ 
    ADC_Handle.Init.NbrOfDiscConversion=0;                     //����������ͨ����Ϊ0
    ADC_Handle.Init.NbrOfConversion=1;                         //1��ת���ڹ��������У�ֻת��1��ͨ����
    
    ADC_Handle.Init.ExternalTrigConv=ADC_EXTERNALTRIGCONV_T3_TRGO;       //��ʱ��3����
    
		HAL_ADC_Init(&ADC_Handle);                                 //��ʼ�� 
 
    //---------------------------------------------------------------------------
    ADC_Config.Channel      = RHEOSTAT_ADC_CHANNEL;            //ADC_CHANNEL_11
    ADC_Config.Rank         = 1;                               //ADCͨ��1 ���1 
    // ADCʹ�����ɸ� ADC_CLK ���ڶ�����ĵ�ѹ���в�������ǰ����Ϊ����ʱ�� 55.5 ADCʱ������
		// ת��ʱ����㣺
		// Tconv = ����ʱ�� + 12.5 ������ = 55.5 + 12.5 = 68��ADCCLK = 12MHz
		// ת��ʱ�� T = 68/12M = 5.667us
    ADC_Config.SamplingTime = ADC_SAMPLETIME_55CYCLES_5 ;      
    // ���� ADC ͨ��ת��˳��Ϊ1����һ��ת��������ʱ��Ϊ55.5��ʱ������
    HAL_ADC_ConfigChannel(&ADC_Handle, &ADC_Config);

    // HAL_ADC_Start_DMA(&ADC_Handle, (uint32_t*)&ADC_ConvertedValue, 1);   // ����д main ����
}



void Rheostat_Init(void)
{
	Rheostat_ADC_GPIO_Config();
	Rheostat_ADC_Mode_Config();
}


void DMA1_Channel1_IRQHandler(void)
{

  HAL_DMA_IRQHandler(&hdma_adcx);

}

// �ص�����
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{	
	printf("�����ж�"); 
	printf("\r\n The current AD value = 0x%04X ", ADC_ConvertedValue); 
}
