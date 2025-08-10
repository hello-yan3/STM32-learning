/**
  ******************************************************************************
  * @file    bsp_rtc.c
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   stm32 RTC 驱动
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 F103-霸道 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
#include "./usart/bsp_debug_usart.h"
#include "./rtc/bsp_rtc.h"

RTC_HandleTypeDef Rtc_Handle;

/* 秒中断标志，进入秒中断时置1，当时间被刷新之后清0 */
uint8_t TimeDisplay = 0;

/**
  * @brief  RTC配置：选择RTC时钟源，设置RTC_CLK的分频系数
  * @param  无
  * @retval 无
  */
void RTC_CLK_Config(void)
{  
	RCC_OscInitTypeDef        RCC_OscInitStruct;
	RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;

	// 配置RTC外设
	Rtc_Handle.Instance = RTC;

	/*使能 PWR 时钟*/
	__HAL_RCC_RTC_ENABLE();

	/* PWR_CR:DBF置1，使能RTC、RTC备份寄存器和备份SRAM的访问 */
	HAL_PWR_EnableBkUpAccess();

	/* 使能Backup时钟 */
	__HAL_RCC_BKP_CLK_ENABLE();
	
	
	#if defined (RTC_CLOCK_SOURCE_LSI) 
	/* 使用LSI作为RTC时钟源会有误差 
	 * 默认选择LSE作为RTC的时钟源
	 */
	/* 初始化LSI */ 
	RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSI;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	RCC_OscInitStruct.LSIState = RCC_LSI_ON;
	RCC_OscInitStruct.LSEState = RCC_LSE_OFF;
	HAL_RCC_OscConfig(&RCC_OscInitStruct);
	/* 选择LSI做为RTC的时钟源 */
	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
	PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
	HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);

	#elif defined (RTC_CLOCK_SOURCE_LSE)
	/* 初始化LSE */ 
	RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSE;  //  选择 LSE 作为 RTC 时钟源
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	RCC_OscInitStruct.LSEState = RCC_LSE_ON;     // LSE开启
	RCC_OscInitStruct.LSIState = RCC_LSI_OFF;    // LSI关闭
	HAL_RCC_OscConfig(&RCC_OscInitStruct);
	
	/* 选择LSE做为RTC的时钟源 */
	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
	PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
	HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);

	/* Configures the External Low Speed oscillator (LSE) drive capability */
	/* 配置外部低速振荡器 (LSE) 驱动能力 */
	__HAL_RCC_LSE_CONFIG(RCC_LSE_ON);  //

	#endif /* RTC_CLOCK_SOURCE_LSI */

	/* 使能RTC时钟 */
	__HAL_RCC_RTC_ENABLE(); 

	/* 等待 RTC APB 寄存器同步 */
	HAL_RTC_WaitForSynchro(&Rtc_Handle);
	
	/*=====================初始化同步/异步预分频器的值======================*/
	/* 驱动日历的时钟ck_spare = LSE/[(255+1)*(127+1)] = 1HZ */

	/* 设置异步预分频器的值 */
	Rtc_Handle.Init.AsynchPrediv = RTC_AUTO_1_SECOND;   // 1s 时基
	//	/* 设置同步预分频器的值 */
	//	Rtc_Handle.Init.SynchPrediv  = SYNCHPREDIV;	
	Rtc_Handle.Init.OutPut   = RTC_OUTPUTSOURCE_SECOND;  // 秒中断
	/* 用RTC_InitStructure的内容初始化RTC寄存器 */
	if (HAL_RTC_Init(&Rtc_Handle) != HAL_OK)
	{
		printf("\n\r RTC 时钟初始化失败 \r\n");
	}

	/* 中断配置 */
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
	
	HAL_NVIC_SetPriority(RTC_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(RTC_IRQn);

	__HAL_RTC_SECOND_ENABLE_IT(&Rtc_Handle, RTC_IT_SEC);
}


/*
 * 函数名：RTC_CheckAndConfig
 * 描述  ：检查并配置RTC
 * 输入  ：用于读取RTC时间的结构体指针
 * 输出  ：无
 * 调用  ：外部调用
 */
void RTC_CheckAndConfig(struct rtc_time *tm)
{
	/*
	 * 当我们配置过RTC时间后，就往备份寄存器0写入一个数据做标记，
	 * 所以每次程序重新运行的时候，就通过检测备份寄存器0的值来判断RTC是否已经配置过，
	 * 如果配置过那就继续运行，如果没有配置过就初始化RTC，配置RTC的时间。
	 */

	/* RTC配置：选择时钟源，设置RTC_CLK的分频系数 */
	RTC_CLK_Config();

	/* 读取备份寄存器的值，如果是之前没进入过备份寄存器 
	 * 那就先设置当前时间和日期 
	 */
	if (HAL_RTCEx_BKUPRead(&Rtc_Handle,RTC_BKP_DRX) != RTC_BKP_DATA)
	{				
		/* 设置时间和日期 */
		RTC_TimeAndDate_Set(tm);
	}
	else
	{
		/* 检查是否电源复位 */
		if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST) != RESET)
		{
			printf("\r\n 发生电源复位\r\n");
		}
		/* 检查是否外部复位 */
		else if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST) != RESET)
		{
			printf("\r\n 发生外部复位\r\n");
		}
		printf("\r\n 不需要重新配置RTC\r\n");
    
		/* 使能 PWR 时钟 */
		__HAL_RCC_RTC_ENABLE();
		/* PWR_CR:DBF置1，使能RTC、RTC备份寄存器和备份SRAM的访问 */
		HAL_PWR_EnableBkUpAccess();
		/* 等待 RTC APB 寄存器同步 */
		HAL_RTC_WaitForSynchro(&Rtc_Handle);
	} 
}




/**
  * @brief  设置时间和日期
  * @param  无
  * @retval 无
  */
void RTC_TimeAndDate_Set(struct rtc_time *tm)
{
		/* 不太推荐，有bug */
		/* bug原因见：https://www.cnblogs.com/xiaobaibai2021/p/15716909.html */
	
//  RTC_DateTypeDef  RTC_DateStructure;
//  RTC_TimeTypeDef  RTC_TimeStructure;
//	
//	/* 由日期计算时间戳 - 时差 */
//	to_tm(mktimev(tm)-TIME_ZOOM, tm);
//	/* 计算星期 */
//	GregorianDay(tm);
//	
//	// 初始化时间
//	RTC_TimeStructure.Hours = tm->tm_hour;         
//	RTC_TimeStructure.Minutes = tm->tm_min;      
//	RTC_TimeStructure.Seconds = tm->tm_sec;      
//  HAL_RTC_SetTime(&Rtc_Handle,&RTC_TimeStructure, RTC_FORMAT_BIN);   // RTC_FORMAT_BIN 表示格式为 10进制
//	
//  // 初始化日期	
//	RTC_DateStructure.Date = tm->tm_mday;         
//	RTC_DateStructure.Month = tm->tm_mon;         
//	RTC_DateStructure.Year = tm->tm_year-2000;     
//	RTC_DateStructure.WeekDay = tm->tm_wday;       	
//  HAL_RTC_SetDate(&Rtc_Handle,&RTC_DateStructure, RTC_FORMAT_BIN);

	// 直接向计数寄存器写入值
	RTC_WriteTimeCounter(&Rtc_Handle, mktimev(tm)-TIME_ZOOM);
	// 向备份寄存器写入标志
  HAL_RTCEx_BKUPWrite(&Rtc_Handle,RTC_BKP_DRX, RTC_BKP_DATA);
}

/**
  * @brief  显示时间和日期
  * @param  无
  * @retval 无
  */
void RTC_TimeAndDate_Get(struct rtc_time *tm)
{
		/* 不太推荐，有bug */
	  /* bug原因见：https://www.cnblogs.com/xiaobaibai2021/p/15716909.html */
	
//	RTC_TimeTypeDef RTC_TimeStructure;
//	RTC_DateTypeDef RTC_DateStructure;
//	
//	// 获取日历
//	HAL_RTC_GetTime(&Rtc_Handle, &RTC_TimeStructure, RTC_FORMAT_BIN);
//	HAL_RTC_GetDate(&Rtc_Handle, &RTC_DateStructure, RTC_FORMAT_BIN);
//	
//	tm->tm_year = RTC_DateStructure.Year+2000;
//	tm->tm_mon  = RTC_DateStructure.Month;
//	tm->tm_mday = RTC_DateStructure.Date;
//	tm->tm_wday = RTC_DateStructure.WeekDay;
//	tm->tm_hour = RTC_TimeStructure.Hours;
//	tm->tm_min  = RTC_TimeStructure.Minutes;
//	tm->tm_sec  = RTC_TimeStructure.Seconds;
	// 直接向计数寄存器读取值
	uint32_t TimeVar;
	TimeVar = RTC_ReadTimeCounter(&Rtc_Handle);
	to_tm(TimeVar, tm);
}


/*
 * 函数名：Time_Display
 * 描述  ：显示当前时间值
 * 输入  ：-TimeVar RTC计数值，单位为 s
 * 输出  ：无
 * 调用  ：内部调用
 */	
void Time_Display(struct rtc_time *tm)
{
		static uint32_t FirstDisplay = 1;
		uint32_t TimeVar;
		uint8_t str[200]; // 字符串暂存  	

		/* 获取当前时间 */
		RTC_TimeAndDate_Get(tm);
		/* 获得32位UNIX时间戳 */
		TimeVar = mktimev(tm);
		/* 把标准时间转换为北京时间 */
		to_tm(TimeVar+TIME_ZOOM, tm);
	
	  if((!tm->tm_hour && !tm->tm_min && !tm->tm_sec)  || (FirstDisplay))
	  {
	      
	      GetChinaCalendar((u16)tm->tm_year, (u8)tm->tm_mon, (u8)tm->tm_mday, str);	
					printf("\r\n 今天新历：%0.2d%0.2d,%0.2d,%0.2d", str[0], str[1], str[2],  str[3]);
	
	      GetChinaCalendarStr((u16)tm->tm_year,(u8)tm->tm_mon,(u8)tm->tm_mday,str);
					printf("\r\n 今天农历：%s\r\n", str);
	
	     if(GetJieQiStr((u16)tm->tm_year, (u8)tm->tm_mon, (u8)tm->tm_mday, str))
					printf("\r\n 今天农历：%s\r\n", str);
	
	      FirstDisplay = 0;
	  }	 	  	

	  /* 输出时间戳，公历时间 */
	  printf(" UNIX时间戳 = %d 当前时间为: %d年(%s年) %d月 %d日 (星期%s)  %0.2d:%0.2d:%0.2d\r",TimeVar,
	                    tm->tm_year, zodiac_sign[(tm->tm_year-3)%12], tm->tm_mon, tm->tm_mday, 
	                    WEEK_STR[tm->tm_wday], tm->tm_hour, 
	                    tm->tm_min, tm->tm_sec);
}

/*
 * 函数名：Time_Regulate_Get
 * 描述  ：保存用户使用串口设置的时间，
 *         以便后面转化成时间戳存储到RTC 计数寄存器中。
 * 输入  ：用于读取RTC时间的结构体指针
 * 注意  ：在串口调试助手输入时，输入完数字要加回车
 */
void Time_Regulate_Get(struct rtc_time *tm)
{
	  uint32_t temp_num = 0;
		uint8_t day_max=0 ;

	  printf("\r\n=========================设置时间==================");
		
	  do 
	  {
			printf("\r\n  请输入年份(Please Set Years),范围[1970~2038]，输入字符后请加回车:");
			scanf("%d",&temp_num);
			if(temp_num <1970 || temp_num >2038)
			{
				printf("\r\n 您输入的数字是:%d，不符合要求",temp_num);
					  
			}
			else
			{	  
				printf("\n\r  年份被设置为: %d\n\r", temp_num);

				tm->tm_year = temp_num;
				break;
			}
	  }while(1);


	 do 
	  {
			printf("\r\n  请输入月份(Please Set Months):范围[1~12]，输入字符后请加回车:");
			scanf("%d",&temp_num);
			if(temp_num <1 || temp_num >12)
			{
				printf("\r\n 您输入的数字是:%d，不符合要求",temp_num);
					  
			}
			else
			{	  
				printf("\n\r  月份被设置为: %d\n\r", temp_num);

				tm->tm_mon = temp_num;
				break;
			}
	  }while(1);
		
		/*根据月份计算最大日期*/
		switch(tm->tm_mon)
			{
				case 1:
				case 3:
				case 5:
				case 7:
				case 8:
				case 10:
				case 12:					
						day_max = 31;
					break;
				
				case 4:
				case 6:
				case 9:
				case 11:
						day_max = 30;
					break;
				
				case 2:					
				     /*计算闰年*/
						if((tm->tm_year%4==0) &&
							 ((tm->tm_year%100!=0) || (tm->tm_year%400==0)) &&
							 (tm->tm_mon>2)) 
								{
									day_max = 29;
								} else 
								{
									day_max = 28;
								}
					break;			
			}

		do 
	  {				
			printf("\r\n  请输入日期(Please Set Months),范围[1~%d]，输入字符后请加回车:",day_max);
			scanf("%d",&temp_num);
			
			if(temp_num <1 || temp_num >day_max)
			{
				printf("\r\n 您输入的数字是:%d，不符合要求",temp_num);
			}
			else
			{
				printf("\n\r  日期被设置为: %d\n\r", temp_num);

				tm->tm_mday = temp_num;
				break;
			}
	  }while(1);
		
		do 
	  {				
			printf("\r\n  请输入时钟(Please Set Hours),范围[0~23]，输入字符后请加回车:");
			scanf("%d",&temp_num);
			
			if( temp_num >23)
			{
				printf("\r\n 您输入的数字是:%d，不符合要求",temp_num);
			}
			else
			{
				printf("\n\r  时钟被设置为: %d\n\r", temp_num);

				tm->tm_hour = temp_num;
				break;
			}
	  }while(1);

		do 
	  {				
			printf("\r\n  请输入分钟(Please Set Minutes),范围[0~59]，输入字符后请加回车:");
			scanf("%d",&temp_num);
			
			if( temp_num >59)
			{
				printf("\r\n 您输入的数字是:%d，不符合要求",temp_num);
			}
			else
			{
				printf("\n\r  分钟被设置为: %d\n\r", temp_num);

				tm->tm_min = temp_num;
				break;
			}
	  }while(1);

		do 
	  {				
			printf("\r\n  请输入秒钟(Please Set Seconds),范围[0~59]，输入字符后请加回车:");
			scanf("%d",&temp_num);
			
			if( temp_num >59)
			{
				printf("\r\n 您输入的数字是:%d，不符合要求",temp_num);
			}
			else
			{
				printf("\n\r  秒钟被设置为: %d\n\r", temp_num);

				tm->tm_sec = temp_num;
				break;
			}
	  }while(1);

}

/**********************************END OF FILE*************************************/
