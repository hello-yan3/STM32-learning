#include "./adc/bsp_adc.h"
#include "./tim/bsp_general_tim.h"
#include "./usart/bsp_debug_usart.h"

__IO uint16_t ADC_ConvertedValue;
DMA_HandleTypeDef hdma_adcx;
ADC_HandleTypeDef ADC_Handle;
ADC_ChannelConfTypeDef ADC_Config;


// RHEOSTAT  变阻器
static void Rheostat_ADC_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RHEOSTAT_ADC_CLK_ENABLE();         // __HAL_RCC_ADC1_CLK_ENABLE(); 
    // 使能 GPIO 时钟
    RHEOSTAT_ADC_GPIO_CLK_ENABLE();    // __HAL_RCC_GPIOC_CLK_ENABLE()
          
    // 配置 IO
    GPIO_InitStructure.Pin = RHEOSTAT_ADC_GPIO_PIN;   // GPIO_PIN_1
    GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;	      // 模拟输入
    GPIO_InitStructure.Pull = GPIO_NOPULL ;           // 不上拉不下拉
    HAL_GPIO_Init(RHEOSTAT_ADC_GPIO_PORT, &GPIO_InitStructure);		   // GPIOC
}

static void Rheostat_ADC_Mode_Config(void)
{
    // ------------------DMA Init 结构体参数 初始化--------------------------
    // 开启DMA时钟
    RHEOSTAT_ADC_DMA_CLK_ENABLE();     // __HAL_RCC_DMA1_CLK_ENABLE();
    // 数据传输通道
    hdma_adcx.Instance = RHEOSTAT_ADC_DMA_STREAM;    // DMA1_Channel1
  
    hdma_adcx.Init.Direction=DMA_PERIPH_TO_MEMORY;             //外设到存储器
    hdma_adcx.Init.PeriphInc=DMA_PINC_DISABLE;                 //外设非增量模式
    hdma_adcx.Init.MemInc=DMA_MINC_ENABLE;                     //存储器增量模式 
    hdma_adcx.Init.PeriphDataAlignment=DMA_PDATAALIGN_HALFWORD;//外设数据长度:16位
    hdma_adcx.Init.MemDataAlignment=DMA_MDATAALIGN_HALFWORD;   //存储器数据长度:16位
    hdma_adcx.Init.Mode= DMA_CIRCULAR;                         //DMA循环传输
    hdma_adcx.Init.Priority=DMA_PRIORITY_MEDIUM;               //中等优先级

    //初始化DMA流，流相当于一个大的管道，管道里面有很多通道
    HAL_DMA_Init(&hdma_adcx); 

    __HAL_LINKDMA( &ADC_Handle,DMA_Handle,hdma_adcx);
  
		HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
	
    //---------------------------------------------------------------------------
    RCC_PeriphCLKInitTypeDef ADC_CLKInit;
    // 开启ADC时钟
    ADC_CLKInit.PeriphClockSelection=RCC_PERIPHCLK_ADC;			   //ADC外设时钟
    ADC_CLKInit.AdcClockSelection=RCC_ADCPCLK2_DIV6;			     //分频因子6时钟为72M/6=12MHz
    HAL_RCCEx_PeriphCLKConfig(&ADC_CLKInit);					         //设置ADC时钟
   
  	//-------------------ADC Init 结构体参数 初始化-------------------------------
    ADC_Handle.Instance=RHEOSTAT_ADC;                          //ADC1
		
    ADC_Handle.Init.DataAlign=ADC_DATAALIGN_RIGHT;             //右对齐
    ADC_Handle.Init.ScanConvMode=DISABLE;                      //非扫描模式
    ADC_Handle.Init.ContinuousConvMode=DISABLE;                //禁止连续转换
		ADC_Handle.Init.DiscontinuousConvMode=DISABLE;             //禁止不连续转换模式 
    ADC_Handle.Init.NbrOfDiscConversion=0;                     //不连续采样通道数为0
    ADC_Handle.Init.NbrOfConversion=1;                         //1个转换在规则序列中（只转换1个通道）
    
    ADC_Handle.Init.ExternalTrigConv=ADC_EXTERNALTRIGCONV_T3_TRGO;       //定时器3触发
    
		HAL_ADC_Init(&ADC_Handle);                                 //初始化 
 
    //---------------------------------------------------------------------------
    ADC_Config.Channel      = RHEOSTAT_ADC_CHANNEL;            //ADC_CHANNEL_11
    ADC_Config.Rank         = 1;                               //ADC通道1 序号1 
    // ADC使用若干个 ADC_CLK 周期对输入的电压进行采样，当前设置为采样时间 55.5 ADC时钟周期
		// 转换时间计算：
		// Tconv = 采样时间 + 12.5 个周期 = 55.5 + 12.5 = 68；ADCCLK = 12MHz
		// 转换时间 T = 68/12M = 5.667us
    ADC_Config.SamplingTime = ADC_SAMPLETIME_55CYCLES_5 ;      
    // 配置 ADC 通道转换顺序为1，第一个转换，采样时间为55.5个时钟周期
    HAL_ADC_ConfigChannel(&ADC_Handle, &ADC_Config);

    // HAL_ADC_Start_DMA(&ADC_Handle, (uint32_t*)&ADC_ConvertedValue, 1);   // 建议写 main 函数
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

// 回调函数
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{	
	printf("进入中断"); 
	printf("\r\n The current AD value = 0x%04X ", ADC_ConvertedValue); 
}
