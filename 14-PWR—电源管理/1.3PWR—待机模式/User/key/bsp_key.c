/**
  ******************************************************************************
  * @file    bsp_key.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   按键应用bsp（扫描模式）
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F103 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */ 

  

#include ".\key\bsp_key.h" 

/**
  * @brief  配置按键用到的I/O口
  * @param  无
  * @retval 无
  */

void Key_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	
    /*开启按键GPIO口的时钟*/
    KEY1_GPIO_CLK_ENABLE();
    KEY2_GPIO_CLK_ENABLE();
	
    /*选择按键的引脚*/	
    GPIO_InitStructure.Pin = KEY2_PIN; 
    /*设置引脚为输入模式*/
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT; 
    /*设置引脚不上拉也不下拉*/
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    /*使用上面的结构体初始化按键*/
    HAL_GPIO_Init(KEY2_GPIO_PORT, &GPIO_InitStructure);

}



/**
  * @brief   检测是否有按键按下
  * @param   具体的端口和端口位
  *		@arg GPIOx: x可以是（A...G） 
  *		@arg GPIO_PIN 可以是GPIO_PIN_x（x可以是1...16）
  * @retval  按键的状态
  *		@arg KEY_ON:按键按下
  *		@arg KEY_OFF:按键没按下
  */

uint8_t Key_Scan(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin)
{			
	/*检测是否有按键按下 */
	if(HAL_GPIO_ReadPin(GPIOx,GPIO_Pin) == KEY_ON )  
	{	 
		/*等待按键释放 */
		while(HAL_GPIO_ReadPin(GPIOx,GPIO_Pin) == KEY_ON);   
		return 	KEY_ON;	 
	}
	else
		return KEY_OFF;
}


/**
  * @brief  用于检测按键是否被长时间按下
  * @param  无
  * @retval 1 ：按键被长时间按下  0 ：按键没有被长时间按下
  */
uint8_t KEY2_LongPress(void)
{			
	uint8_t downCnt =0;	//记录按下的次数
	uint8_t upCnt =0;	  //记录松开的次数			

	while(1)					//死循环，由return结束
	{	
		HAL_Delay(20);	//延迟一段时间再检测

		if(HAL_GPIO_ReadPin( KEY2_GPIO_PORT, KEY2_PIN ) == SET)	//检测到按下按键
		{
			downCnt++;	//记录按下次数
			upCnt=0;  	//清除按键释放记录

			if(downCnt>=100)	//按下时间足够
			{
				return 1; 		//检测到按键被时间长按下
			}
		}
		else 
		{
			upCnt++; 			//记录释放次数
			if(upCnt>5)			//连续检测到释放超过5次
			{
				return 0;		//按下时间太短，不是按键长按操作
			}
		}
	}
}

/*********************************************END OF FILE**********************/

