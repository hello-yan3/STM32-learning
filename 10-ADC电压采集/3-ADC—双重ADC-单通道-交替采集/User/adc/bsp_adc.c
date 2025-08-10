
/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ADC
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103-�Ե� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
#include "./adc/bsp_adc.h"

// ʹ�� ADC1 ADC2 ͬʱ�ɼ�һ��ͨ��

ADC_HandleTypeDef hadcx1;
ADC_HandleTypeDef hadcx2;
DMA_HandleTypeDef hdma_adcx;

void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  if(hadc->Instance==ADCx1)   // ADC1
  {
    /* ����ʱ��ʹ�� */
    ADCx1_RCC_CLK_ENABLE();   // __HAL_RCC_ADC1_CLK_ENABLE()
    
    /* ADת��ͨ������ʱ��ʹ�� */
    ADC_GPIO_ClK_ENABLE();    // __HAL_RCC_GPIOC_CLK_ENABLE()
     
    /* DMAʱ��ʹ�� */
    DMAx1_RCC_CLK_ENABLE();   // __HAL_RCC_DMA1_CLK_ENABLE()
      
    /* ADת��ͨ�����ų�ʼ�� */
    GPIO_InitStruct.Pin = ADC_GPIO_PIN;        // GPIO_PIN_1     
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;   // ģ������
    HAL_GPIO_Init(ADC_GPIO, &GPIO_InitStruct); // GPIOC

    /* DMA�����ʼ������ */  
    hdma_adcx.Instance = ADC_DMAx_CHANNELn;    // DMA1_Channel1
    hdma_adcx.Init.Direction = DMA_PERIPH_TO_MEMORY;           // ���赽�洢��
    hdma_adcx.Init.PeriphInc = DMA_PINC_DISABLE;               // ���������ģʽ
    hdma_adcx.Init.MemInc = DMA_MINC_ENABLE;                   // �洢������ģʽ 
    hdma_adcx.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;  // �������ݳ���:32λ
    hdma_adcx.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;     // �洢�����ݳ���:32λ
    hdma_adcx.Init.Mode = DMA_CIRCULAR;                        // DMAѭ������
    hdma_adcx.Init.Priority = DMA_PRIORITY_HIGH;               // �����ȼ�
    HAL_DMA_Init(&hdma_adcx);
    /* ����DMA */
    __HAL_LINKDMA(hadc,DMA_Handle,hdma_adcx);
    
		/* �����жϣ�ע�͵��� */
//    /* �����ж����ȼ����ú�ʹ���ж� */
//    HAL_NVIC_SetPriority(ADC_DMAx_CHANNELn_IRQn, 0, 0);
//    HAL_NVIC_EnableIRQ(ADC_DMAx_CHANNELn_IRQn);

  }
  else if(hadc->Instance==ADCx2)   // ADC2
  {
    /* ����ʱ��ʹ�� */
    ADCx2_RCC_CLK_ENABLE();        // __HAL_RCC_ADC2_CLK_ENABLE()
    
    /* ADת��ͨ������ʱ��ʹ�� */
    ADC_GPIO_ClK_ENABLE();         // __HAL_RCC_GPIOC_CLK_ENABLE()
      
    /* ADת��ͨ�����ų�ʼ�� */
    GPIO_InitStruct.Pin = ADC_GPIO_PIN;        // GPIO_PIN_1   
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;   // ģ������
    HAL_GPIO_Init(ADC_GPIO, &GPIO_InitStruct); // GPIOC
  }
}

void ADCx_Init(void)
{
  ADC_MultiModeTypeDef multimode;   // ��ADC�ṹ��
  ADC_ChannelConfTypeDef sConfig;   // ADC���ýṹ��

  // ADC��������
  hadcx1.Instance = ADCx1;       // ADC1
  hadcx1.Init.ScanConvMode = ADC_SCAN_DISABLE;            //��ɨ��ģʽ
  hadcx1.Init.ContinuousConvMode = ENABLE;                //����ת��
  hadcx1.Init.DiscontinuousConvMode = DISABLE;            //��ֹ����������ģʽ
  hadcx1.Init.ExternalTrigConv = ADC_SOFTWARE_START;      //�������
  hadcx1.Init.DataAlign = ADC_DATAALIGN_RIGHT;            //�Ҷ���
  hadcx1.Init.NbrOfConversion = 1;                        //1��ת���ڹ��������У�ֻת��1��ͨ����
  HAL_ADC_Init(&hadcx1);  
  
  /**Configure the ADC multi-mode 
    */
  multimode.Mode = ADC_DUALMODE_INTERLSLOW;       // ����ADC��ͨ���ɼ�����������ʹ�õ�ģʽΪ��ADC_DUALMODE_INTERLSLOW��������ɼ�ģʽ
	// ADC1 �� ADC2 ����ɼ�һ������ͨ���飨ֻ��Ϊһ��ͨ��������ADC2 ����֮��ADC1 ��Ҫ�ȴ� 14 �� ADCCLK ֮����ܴ�����
  HAL_ADCEx_MultiModeConfigChannel(&hadcx1, &multimode);
  
  // ���ò���ͨ��
  sConfig.Channel = ADC_CHANNEL;   // ADC_CHANNEL_11
  sConfig.Rank = 1;
	// ADCʹ�����ɸ� ADC_CLK ���ڶ�����ĵ�ѹ���в�������ǰ����Ϊ����ʱ�� 55.5 ADCʱ������
	// ת��ʱ����㣺
	// Tconv = ����ʱ�� + 12.5 ������ = 55.5 + 12.5 = 68��ADCCLK = 12MHz
	// ת��ʱ�� T = 68/12M = 5.667us
  sConfig.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
  HAL_ADC_ConfigChannel(&hadcx1, &sConfig);
  
  // ADC��������
  hadcx2.Instance = ADCx2;       // ADC2
  hadcx2.Init.ScanConvMode = ADC_SCAN_DISABLE;            //��ɨ��ģʽ
  hadcx2.Init.ContinuousConvMode = ENABLE;                //����ת��
  hadcx2.Init.DiscontinuousConvMode = DISABLE;            //��ֹ����������ģʽ
  hadcx2.Init.ExternalTrigConv = ADC_SOFTWARE_START;      //�������
  hadcx2.Init.DataAlign = ADC_DATAALIGN_RIGHT;            //�Ҷ���
  hadcx2.Init.NbrOfConversion = 1;                        //1��ת���ڹ��������У�ֻת��1��ͨ����
  HAL_ADC_Init(&hadcx2);  
  
  /**Configure the ADC multi-mode 
    */
  multimode.Mode = ADC_DUALMODE_INTERLSLOW;       // ����ADC��ͨ���ɼ�����������ʹ�õ�ģʽΪ��ADC_DUALMODE_INTERLSLOW
  HAL_ADCEx_MultiModeConfigChannel(&hadcx2, &multimode);
  
  // ���ò���ͨ��
  sConfig.Channel = ADC_CHANNEL;   // ADC_CHANNEL_11
  sConfig.Rank = 1;
	// ADCʹ�����ɸ� ADC_CLK ���ڶ�����ĵ�ѹ���в�������ǰ����Ϊ����ʱ�� 55.5 ADCʱ������
	// ת��ʱ����㣺
	// Tconv = ����ʱ�� + 12.5 ������ = 55.5 + 12.5 = 68��ADCCLK = 12MHz
	// ת��ʱ�� T = 68/12M = 5.667us
  sConfig.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
  HAL_ADC_ConfigChannel(&hadcx2, &sConfig);
}



