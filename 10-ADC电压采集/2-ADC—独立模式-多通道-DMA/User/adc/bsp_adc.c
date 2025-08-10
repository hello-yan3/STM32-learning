#include "./adc/bsp_adc.h"

// ADC1 同时转换多通道
__IO uint16_t ADC_ConvertedValue[NOFCHANEL]={0};
DMA_HandleTypeDef hdma_adcx;
ADC_HandleTypeDef ADC_Handle;
ADC_ChannelConfTypeDef ADC_Config;

static void Rheostat_ADC_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RHEOSTAT_ADC_CLK_ENABLE();         // __HAL_RCC_ADC1_CLK_ENABLE(); 
    // 使能 GPIO 时钟
    RHEOSTAT_ADC_GPIO_CLK_ENABLE();    // __HAL_RCC_GPIOC_CLK_ENABLE()
          
    // 配置 IO
    GPIO_InitStructure.Pin =ADC_PIN1|     // GPIO_PIN_0
                            ADC_PIN2|     // GPIO_PIN_1
                            ADC_PIN3|     // GPIO_PIN_2
                            ADC_PIN4|     // GPIO_PIN_3
                            ADC_PIN5|     // GPIO_PIN_4
                            ADC_PIN6;     // GPIO_PIN_5
    GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;	 // 模拟输入
    GPIO_InitStructure.Pull = GPIO_NOPULL ;      //不上拉不下拉
    HAL_GPIO_Init(RHEOSTAT_ADC_GPIO_PORT, &GPIO_InitStructure);	   // GPIOC	
}

static void Rheostat_ADC_Mode_Config(void)
{
	// ------------------DMA Init 结构体参数 初始化--------------------------
	// 开启DMA时钟
	RHEOSTAT_ADC_DMA_CLK_ENABLE();     // __HAL_RCC_DMA1_CLK_ENABLE();
	// 数据传输通道
	hdma_adcx.Instance = RHEOSTAT_ADC_DMA_STREAM;  // DMA1_Channel1

	hdma_adcx.Init.Direction=DMA_PERIPH_TO_MEMORY;              //外设到存储器
	hdma_adcx.Init.PeriphInc=DMA_PINC_DISABLE;                  //外设非增量模式
	hdma_adcx.Init.MemInc=DMA_MINC_ENABLE;                      //存储器增量模式 
	hdma_adcx.Init.PeriphDataAlignment=DMA_PDATAALIGN_HALFWORD; //外设数据长度:16位
	hdma_adcx.Init.MemDataAlignment=DMA_MDATAALIGN_HALFWORD;    //存储器数据长度:16位
	hdma_adcx.Init.Mode= DMA_CIRCULAR;                          //DMA循环传输
	hdma_adcx.Init.Priority=DMA_PRIORITY_MEDIUM;                //中等优先级

	HAL_DMA_Init(&hdma_adcx); 

	__HAL_LINKDMA( &ADC_Handle,DMA_Handle,hdma_adcx);

	//---------------------------------------------------------------------------
	RCC_PeriphCLKInitTypeDef ADC_CLKInit;
	// 开启ADC时钟
	ADC_CLKInit.PeriphClockSelection=RCC_PERIPHCLK_ADC;	   //ADC外设时钟
	ADC_CLKInit.AdcClockSelection=RCC_ADCPCLK2_DIV6;			 //分频因子6时钟为72M/6=12MHz
	HAL_RCCEx_PeriphCLKConfig(&ADC_CLKInit);					     //设置ADC时钟

	//-------------------ADC Init 结构体参数 初始化-------------------------------
	ADC_Handle.Instance=RHEOSTAT_ADC;                           //ADC1
	
	ADC_Handle.Init.DataAlign=ADC_DATAALIGN_RIGHT;              //右对齐
	ADC_Handle.Init.ScanConvMode=ENABLE;                        //扫描模式
	ADC_Handle.Init.ContinuousConvMode=ENABLE;                  //连续转换
	ADC_Handle.Init.DiscontinuousConvMode=DISABLE;              //禁止不连续采样模式
	ADC_Handle.Init.NbrOfDiscConversion=0;                      //不连续采样通道数为0
	ADC_Handle.Init.NbrOfConversion=NOFCHANEL	;                 //6个转换在规则序列中（转换6个通道）
	ADC_Handle.Init.ExternalTrigConv=ADC_SOFTWARE_START;        //软件触发
	
	HAL_ADC_Init(&ADC_Handle);                                

	//---------------------------------------------------------------------------
  // ADC使用若干个 ADC_CLK 周期对输入的电压进行采样，当前设置为采样时间 55.5 ADC时钟周期
	// 转换时间计算：
	// Tconv = 采样时间 + 12.5 个周期 = 1.5 + 12.5 = 14；ADCCLK = 12MHz
	// 转换时间 T = 14/12M = 1.667us
	ADC_Config.SamplingTime = ADC_SAMPLETIME_1CYCLE_5; 
	
	ADC_Config.Channel      = ADC_CHANNEL1;  
	ADC_Config.Rank         = 1;

	// 配置 ADC 通道1转换顺序为1，第一个转换，采样时间为1.5个时钟周期
	HAL_ADC_ConfigChannel(&ADC_Handle, &ADC_Config);

	//---------------------------------------------------------------------------
	ADC_Config.Channel      = ADC_CHANNEL2;
	ADC_Config.Rank         = 2;
	// 配置 ADC 通道2转换顺序为2，第二个转换，采样时间为1.5个时钟周期
	HAL_ADC_ConfigChannel(&ADC_Handle, &ADC_Config);   

	//---------------------------------------------------------------------------
	ADC_Config.Channel      = ADC_CHANNEL3;
	ADC_Config.Rank         = 3;
	// 配置 ADC 通道3转换顺序为3，第三个转换，采样时间为1.5个时钟周期
	HAL_ADC_ConfigChannel(&ADC_Handle, &ADC_Config);   
	//---------------------------------------------------------------------------
	ADC_Config.Channel      = ADC_CHANNEL4;
	ADC_Config.Rank         = 4;
	// 配置 ADC 通道4转换顺序为4，第四个转换，采样时间为1.5个时钟周期
	HAL_ADC_ConfigChannel(&ADC_Handle, &ADC_Config);   
	//---------------------------------------------------------------------------
	ADC_Config.Channel      = ADC_CHANNEL5;
	ADC_Config.Rank         = 5;
	// 配置 ADC 通道5转换顺序为5，第五个转换，采样时间为1.5个时钟周期
	HAL_ADC_ConfigChannel(&ADC_Handle, &ADC_Config);   
	//---------------------------------------------------------------------------
	ADC_Config.Channel      = ADC_CHANNEL6;
	ADC_Config.Rank         = 6;
	// 配置 ADC 通道6转换顺序为6，第六个转换，采样时间为1.5个时钟周期
	HAL_ADC_ConfigChannel(&ADC_Handle, &ADC_Config);       
	
	// HAL_ADC_Start_DMA(&ADC_Handle, (uint32_t*)&ADC_ConvertedValue, 6);    // 建议写 main 函数
}



void Rheostat_Init(void)
{
	Rheostat_ADC_GPIO_Config();
	Rheostat_ADC_Mode_Config();
}

