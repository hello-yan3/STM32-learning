/**
  ******************************************************************************
  * @file    bsp_xxx.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   adc1 应用bsp / DMA 模式
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火STM32 霸道 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
  
#include "./dac/bsp_dac.h"

DAC_HandleTypeDef DAC_InitStructure;
DMA_HandleTypeDef DMA_InitStructure;
TIM_HandleTypeDef TIM_Time;


/* 波形数据 ---------------------------------------------------------*/
/*
电压数据计算：(Vref 电压3.3V)
3.3V → 2^12(4096)
0V   → 0
*/

uint16_t Sine12bit[POINT_NUM] = {
	2048	, 2460	, 2856	, 3218	, 3532	, 3786	, 3969	, 4072	,
	4093	, 4031	, 3887	, 3668	, 3382	, 3042	,	2661	, 2255	, 
	1841	, 1435	, 1054	, 714		, 428		, 209		, 65		, 3			,
	24		, 127		, 310		, 564		, 878		, 1240	, 1636	, 2048
};

/*
uint16_t Sine12bit[POINT_NUM] = {
	2148,2249,2349,2448,2546,2643,2738,2832,
	2924,3013,3101,3186,3268,3347,3423,3496,
	3565,3631,3693,3751,3805,3854,3899,3940,
	3976,4008,4035,4057,4074,4086,4094,4095,
	4094,4086,4074,4057,4035,4008,3976,3940,
	3899,3854,3805,3751,3693,3631,3565,3496,
	3423,3347,3268,3186,3101,3013,2924,2832,
	2738,2643,2546,2448,2349,2249,2148,2048,
	1948,1847,1747,1648,1550,1453,1358,1264,
	1172,1083,995 ,910 ,828 ,749 ,673 ,600 ,
  531 ,465 ,403 ,345 ,291 ,242 ,197 ,156 ,
	120 ,88  ,61  ,39  ,22  ,10  ,2   ,0   ,
	2   ,10  ,22  ,39  ,61  ,88  ,120 ,156 ,
	197 ,242 ,291 ,345 ,403 ,465 ,531 ,600 ,
	673 ,749 ,828 ,910 ,995 ,1083,1172,1264,
	1358,1453,1550,1648,1747,1847,1948,2048
};
*/

/**
  * @brief  使能DAC的时钟，初始化GPIO
  * @param  无
  * @retval 无
  */
void HAL_DAC_MspInit(DAC_HandleTypeDef*  DAC_InitStructure)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  
  /* DMA控制器时钟使能 */
  DACx_DMA_CLK_ENABLE();    // __HAL_RCC_DMA2_CLK_ENABLE() 
  
  /* DAC通道引脚端口时钟使能 */
  DACx_CHANNEL_GPIO_CLK_ENABLE();  // __HAL_RCC_GPIOA_CLK_ENABLE()
  
  /* DAC通道引脚配置 */
  GPIO_InitStruct.Pin = DACx_CHANNEL_PIN;    // GPIO_PIN_5
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;   // 注意DAC输出，引脚要配置为模拟模式！
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(DACx_CHANNEL_GPIO_PORT, &GPIO_InitStruct);   // GPIOA
  
  /* DMA控制器初始化 */  
  DMA_InitStructure.Instance = DACx_DMAx_CHANNELn;            // DMA2_Channel4，DMA通道由手册确定   
  DMA_InitStructure.Init.Direction = DMA_MEMORY_TO_PERIPH;    // 存储器→外设
  DMA_InitStructure.Init.PeriphInc = DMA_PINC_DISABLE;        // 外设地址不自增
  DMA_InitStructure.Init.MemInc = DMA_MINC_ENABLE;            // 存储器地址自增
  DMA_InitStructure.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;  // 外设数据长度:   16bits
  DMA_InitStructure.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;     // 存储器数据长度: 16bits
  DMA_InitStructure.Init.Mode = DMA_CIRCULAR;  // DMA 循环数据传输
  DMA_InitStructure.Init.Priority = DMA_PRIORITY_HIGH;  // DMA高优先级
  HAL_DMA_Init(&DMA_InitStructure);
	
  __HAL_LINKDMA( DAC_InitStructure,DMA_HANDLE,DMA_InitStructure); 
}


/**
  * @brief  使能DAC配置
  * @param  无
  * @retval 无
  */
 void DAC_Config(void)
{
  DAC_ChannelConfTypeDef sConfig;
  TIM_MasterConfigTypeDef sMasterConfig;
  
  /* DAC外设时钟使能 */
  DACx_CLK_ENABLE();     //  __HAL_RCC_DAC_CLK_ENABLE()
  
  /* DAC初始化 */
  DAC_InitStructure.Instance = DACx;    // DAC   
  HAL_DAC_Init(& DAC_InitStructure);

  /* DAC通道输出配置 */
	/* DAC支持 TIM2/TIM4/TIM5/TIM6/TIM7/TIM8/软件触发 */
	/* 这里选择触发源 */
  sConfig.DAC_Trigger = DAC_TRIGGER_T6_TRGO;    // TIM6 TRGO 生成DAC
	/* 指定DAC通道输出缓冲区是启用还是禁用。 */
	/* 使能了 DAC 的输出缓冲后可以减小输出阻抗，适合直接驱动一些外部负载。 */
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;    // 禁用
  HAL_DAC_ConfigChannel(& DAC_InitStructure, &sConfig, DACx_CHANNEL);   // DAC_CHANNEL_2
  
	// 使用TIM6
  DAC_TIM_CLK_ENABLE();     // __HAL_RCC_TIM6_CLK_ENABLE()
	
  /* 初始化定时器，用于触发DAC更新 */
  TIM_Time.Instance = DAC_TIM;    // TIM6
  TIM_Time.Init.Prescaler = TIM_PRESCALER;   // 预分频器 (72-1)
  TIM_Time.Init.CounterMode = TIM_COUNTERMODE_UP;
  TIM_Time.Init.Period = TIM_PERIOD;         // 计数器  (1000-1)
  HAL_TIM_Base_Init(&TIM_Time);
  
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;    // 选择TRGO更新事件(DAC事件)，否则TIM无驱动无反应
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&TIM_Time, &sMasterConfig);
}

