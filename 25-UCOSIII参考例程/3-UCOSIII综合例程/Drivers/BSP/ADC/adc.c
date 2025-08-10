#include "./BSP/ADC/adc.h"

ADC_HandleTypeDef ADC_Handle;



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
    //---------------------------------------------------------------------------
    RCC_PeriphCLKInitTypeDef ADC_CLKInit;
    // 开启ADC时钟
    ADC_CLKInit.PeriphClockSelection=RCC_PERIPHCLK_ADC;			   //ADC外设时钟
    ADC_CLKInit.AdcClockSelection=RCC_ADCPCLK2_DIV6;			     //分频因子6时钟为72M/6=12MHz
    HAL_RCCEx_PeriphCLKConfig(&ADC_CLKInit);					         //设置ADC时钟
   
  	//-------------------ADC Init 结构体参数 初始化-------------------------------
    ADC_Handle.Instance=RHEOSTAT_ADC;                          //ADC1
		
    ADC_Handle.Init.DataAlign=ADC_DATAALIGN_RIGHT;             //右对齐
    ADC_Handle.Init.ScanConvMode=ADC_SCAN_DISABLE;             //非扫描模式
    ADC_Handle.Init.ContinuousConvMode=DISABLE;                //连续转换
		ADC_Handle.Init.DiscontinuousConvMode=DISABLE;             //禁止不连续转换模式
    ADC_Handle.Init.NbrOfDiscConversion=0;                     //不连续采样通道数为0
    ADC_Handle.Init.NbrOfConversion=1;                         //1个转换在规则序列中（只转换1个通道）
    ADC_Handle.Init.ExternalTrigConv=ADC_SOFTWARE_START;       //软件触发
    
		HAL_ADC_Init(&ADC_Handle);                                 //初始化 
 
    //---------------------------------------------------------------------------
		ADC_ChannelConfTypeDef ADC_Config;
    ADC_Config.Channel      = RHEOSTAT_ADC_CHANNEL;            //ADC_CHANNEL_11
    ADC_Config.Rank         = 1;                               //ADC通道1 序号1 
    // ADC使用若干个 ADC_CLK 周期对输入的电压进行采样，当前设置为采样时间 55.5 ADC时钟周期
		// 转换时间计算：
		// Tconv = 采样时间 + 12.5 个周期 = 55.5 + 12.5 = 68；ADCCLK = 12MHz
		// 转换时间 T = 68/12M = 5.667us
    ADC_Config.SamplingTime = ADC_SAMPLETIME_55CYCLES_5 ;      
    // 配置 ADC 通道转换顺序为1，第一个转换，采样时间为55.5个时钟周期
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
Output  : Battery voltage（MV）
函数功能：读取AD采集结果
入口参数: 无
返回  值：AD采集结果
**************************************************************************/
int Get_battery_volt(void)  
{ 
   	int Volt;
    HAL_ADC_Start(&ADC_Handle);                                //开启ADC
    HAL_ADC_PollForConversion(&ADC_Handle,10);                 //单次转换
	  Volt = HAL_ADC_GetValue(&ADC_Handle);	                     //获得转换结果
	  return Volt;	                                      	     //返回最近一次ADC1规则组的转换结果
}


