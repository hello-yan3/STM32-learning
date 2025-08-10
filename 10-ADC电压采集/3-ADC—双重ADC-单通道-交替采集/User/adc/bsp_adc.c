
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
  * 实验平台:野火 F103-霸道 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
#include "./adc/bsp_adc.h"

// 使用 ADC1 ADC2 同时采集一个通道

ADC_HandleTypeDef hadcx1;
ADC_HandleTypeDef hadcx2;
DMA_HandleTypeDef hdma_adcx;

void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  if(hadc->Instance==ADCx1)   // ADC1
  {
    /* 外设时钟使能 */
    ADCx1_RCC_CLK_ENABLE();   // __HAL_RCC_ADC1_CLK_ENABLE()
    
    /* AD转换通道引脚时钟使能 */
    ADC_GPIO_ClK_ENABLE();    // __HAL_RCC_GPIOC_CLK_ENABLE()
     
    /* DMA时钟使能 */
    DMAx1_RCC_CLK_ENABLE();   // __HAL_RCC_DMA1_CLK_ENABLE()
      
    /* AD转换通道引脚初始化 */
    GPIO_InitStruct.Pin = ADC_GPIO_PIN;        // GPIO_PIN_1     
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;   // 模拟输入
    HAL_GPIO_Init(ADC_GPIO, &GPIO_InitStruct); // GPIOC

    /* DMA外设初始化配置 */  
    hdma_adcx.Instance = ADC_DMAx_CHANNELn;    // DMA1_Channel1
    hdma_adcx.Init.Direction = DMA_PERIPH_TO_MEMORY;           // 外设到存储器
    hdma_adcx.Init.PeriphInc = DMA_PINC_DISABLE;               // 外设非增量模式
    hdma_adcx.Init.MemInc = DMA_MINC_ENABLE;                   // 存储器增量模式 
    hdma_adcx.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;  // 外设数据长度:32位
    hdma_adcx.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;     // 存储器数据长度:32位
    hdma_adcx.Init.Mode = DMA_CIRCULAR;                        // DMA循环传输
    hdma_adcx.Init.Priority = DMA_PRIORITY_HIGH;               // 高优先级
    HAL_DMA_Init(&hdma_adcx);
    /* 连接DMA */
    __HAL_LINKDMA(hadc,DMA_Handle,hdma_adcx);
    
		/* 不用中断，注释掉了 */
//    /* 外设中断优先级配置和使能中断 */
//    HAL_NVIC_SetPriority(ADC_DMAx_CHANNELn_IRQn, 0, 0);
//    HAL_NVIC_EnableIRQ(ADC_DMAx_CHANNELn_IRQn);

  }
  else if(hadc->Instance==ADCx2)   // ADC2
  {
    /* 外设时钟使能 */
    ADCx2_RCC_CLK_ENABLE();        // __HAL_RCC_ADC2_CLK_ENABLE()
    
    /* AD转换通道引脚时钟使能 */
    ADC_GPIO_ClK_ENABLE();         // __HAL_RCC_GPIOC_CLK_ENABLE()
      
    /* AD转换通道引脚初始化 */
    GPIO_InitStruct.Pin = ADC_GPIO_PIN;        // GPIO_PIN_1   
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;   // 模拟输入
    HAL_GPIO_Init(ADC_GPIO, &GPIO_InitStruct); // GPIOC
  }
}

void ADCx_Init(void)
{
  ADC_MultiModeTypeDef multimode;   // 多ADC结构体
  ADC_ChannelConfTypeDef sConfig;   // ADC配置结构体

  // ADC功能配置
  hadcx1.Instance = ADCx1;       // ADC1
  hadcx1.Init.ScanConvMode = ADC_SCAN_DISABLE;            //非扫描模式
  hadcx1.Init.ContinuousConvMode = ENABLE;                //连续转换
  hadcx1.Init.DiscontinuousConvMode = DISABLE;            //禁止不连续采样模式
  hadcx1.Init.ExternalTrigConv = ADC_SOFTWARE_START;      //软件触发
  hadcx1.Init.DataAlign = ADC_DATAALIGN_RIGHT;            //右对齐
  hadcx1.Init.NbrOfConversion = 1;                        //1个转换在规则序列中（只转换1个通道）
  HAL_ADC_Init(&hadcx1);  
  
  /**Configure the ADC multi-mode 
    */
  multimode.Mode = ADC_DUALMODE_INTERLSLOW;       // 配置ADC多通道采集摸索，这里使用的模式为：ADC_DUALMODE_INTERLSLOW，即交替采集模式
	// ADC1 和 ADC2 交替采集一个规则通道组（只能为一个通道）。当ADC2 触发之后，ADC1 需要等待 14 个 ADCCLK 之后才能触发。
  HAL_ADCEx_MultiModeConfigChannel(&hadcx1, &multimode);
  
  // 配置采样通道
  sConfig.Channel = ADC_CHANNEL;   // ADC_CHANNEL_11
  sConfig.Rank = 1;
	// ADC使用若干个 ADC_CLK 周期对输入的电压进行采样，当前设置为采样时间 55.5 ADC时钟周期
	// 转换时间计算：
	// Tconv = 采样时间 + 12.5 个周期 = 55.5 + 12.5 = 68；ADCCLK = 12MHz
	// 转换时间 T = 68/12M = 5.667us
  sConfig.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
  HAL_ADC_ConfigChannel(&hadcx1, &sConfig);
  
  // ADC功能配置
  hadcx2.Instance = ADCx2;       // ADC2
  hadcx2.Init.ScanConvMode = ADC_SCAN_DISABLE;            //非扫描模式
  hadcx2.Init.ContinuousConvMode = ENABLE;                //连续转换
  hadcx2.Init.DiscontinuousConvMode = DISABLE;            //禁止不连续采样模式
  hadcx2.Init.ExternalTrigConv = ADC_SOFTWARE_START;      //软件触发
  hadcx2.Init.DataAlign = ADC_DATAALIGN_RIGHT;            //右对齐
  hadcx2.Init.NbrOfConversion = 1;                        //1个转换在规则序列中（只转换1个通道）
  HAL_ADC_Init(&hadcx2);  
  
  /**Configure the ADC multi-mode 
    */
  multimode.Mode = ADC_DUALMODE_INTERLSLOW;       // 配置ADC多通道采集摸索，这里使用的模式为：ADC_DUALMODE_INTERLSLOW
  HAL_ADCEx_MultiModeConfigChannel(&hadcx2, &multimode);
  
  // 配置采样通道
  sConfig.Channel = ADC_CHANNEL;   // ADC_CHANNEL_11
  sConfig.Rank = 1;
	// ADC使用若干个 ADC_CLK 周期对输入的电压进行采样，当前设置为采样时间 55.5 ADC时钟周期
	// 转换时间计算：
	// Tconv = 采样时间 + 12.5 个周期 = 55.5 + 12.5 = 68；ADCCLK = 12MHz
	// 转换时间 T = 68/12M = 5.667us
  sConfig.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
  HAL_ADC_ConfigChannel(&hadcx2, &sConfig);
}



