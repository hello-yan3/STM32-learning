/**
  ******************************************************************************
  * @file    bsp_rtc.c
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   stm32 RTC ����
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103-�Ե� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
#include "./usart/bsp_debug_usart.h"
#include "./rtc/bsp_rtc.h"

RTC_HandleTypeDef Rtc_Handle;

/* ���жϱ�־���������ж�ʱ��1����ʱ�䱻ˢ��֮����0 */
uint8_t TimeDisplay = 0;

/**
  * @brief  RTC���ã�ѡ��RTCʱ��Դ������RTC_CLK�ķ�Ƶϵ��
  * @param  ��
  * @retval ��
  */
void RTC_CLK_Config(void)
{  
	RCC_OscInitTypeDef        RCC_OscInitStruct;
	RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;

	// ����RTC����
	Rtc_Handle.Instance = RTC;

	/*ʹ�� PWR ʱ��*/
	__HAL_RCC_RTC_ENABLE();

	/* PWR_CR:DBF��1��ʹ��RTC��RTC���ݼĴ����ͱ���SRAM�ķ��� */
	HAL_PWR_EnableBkUpAccess();

	/* ʹ��Backupʱ�� */
	__HAL_RCC_BKP_CLK_ENABLE();
	
	
	#if defined (RTC_CLOCK_SOURCE_LSI) 
	/* ʹ��LSI��ΪRTCʱ��Դ������� 
	 * Ĭ��ѡ��LSE��ΪRTC��ʱ��Դ
	 */
	/* ��ʼ��LSI */ 
	RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSI;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	RCC_OscInitStruct.LSIState = RCC_LSI_ON;
	RCC_OscInitStruct.LSEState = RCC_LSE_OFF;
	HAL_RCC_OscConfig(&RCC_OscInitStruct);
	/* ѡ��LSI��ΪRTC��ʱ��Դ */
	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
	PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
	HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);

	#elif defined (RTC_CLOCK_SOURCE_LSE)
	/* ��ʼ��LSE */ 
	RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSE;  //  ѡ�� LSE ��Ϊ RTC ʱ��Դ
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	RCC_OscInitStruct.LSEState = RCC_LSE_ON;     // LSE����
	RCC_OscInitStruct.LSIState = RCC_LSI_OFF;    // LSI�ر�
	HAL_RCC_OscConfig(&RCC_OscInitStruct);
	
	/* ѡ��LSE��ΪRTC��ʱ��Դ */
	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
	PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
	HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);

	/* Configures the External Low Speed oscillator (LSE) drive capability */
	/* �����ⲿ�������� (LSE) �������� */
	__HAL_RCC_LSE_CONFIG(RCC_LSE_ON);  //

	#endif /* RTC_CLOCK_SOURCE_LSI */

	/* ʹ��RTCʱ�� */
	__HAL_RCC_RTC_ENABLE(); 

	/* �ȴ� RTC APB �Ĵ���ͬ�� */
	HAL_RTC_WaitForSynchro(&Rtc_Handle);
	
	/*=====================��ʼ��ͬ��/�첽Ԥ��Ƶ����ֵ======================*/
	/* ����������ʱ��ck_spare = LSE/[(255+1)*(127+1)] = 1HZ */

	/* �����첽Ԥ��Ƶ����ֵ */
	Rtc_Handle.Init.AsynchPrediv = RTC_AUTO_1_SECOND;   // 1s ʱ��
	//	/* ����ͬ��Ԥ��Ƶ����ֵ */
	//	Rtc_Handle.Init.SynchPrediv  = SYNCHPREDIV;	
	Rtc_Handle.Init.OutPut   = RTC_OUTPUTSOURCE_SECOND;  // ���ж�
	/* ��RTC_InitStructure�����ݳ�ʼ��RTC�Ĵ��� */
	if (HAL_RTC_Init(&Rtc_Handle) != HAL_OK)
	{
		printf("\n\r RTC ʱ�ӳ�ʼ��ʧ�� \r\n");
	}

	/* �ж����� */
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
	
	HAL_NVIC_SetPriority(RTC_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(RTC_IRQn);

	__HAL_RTC_SECOND_ENABLE_IT(&Rtc_Handle, RTC_IT_SEC);
}


/*
 * ��������RTC_CheckAndConfig
 * ����  ����鲢����RTC
 * ����  �����ڶ�ȡRTCʱ��Ľṹ��ָ��
 * ���  ����
 * ����  ���ⲿ����
 */
void RTC_CheckAndConfig(struct rtc_time *tm)
{
	/*
	 * ���������ù�RTCʱ��󣬾������ݼĴ���0д��һ����������ǣ�
	 * ����ÿ�γ����������е�ʱ�򣬾�ͨ����ⱸ�ݼĴ���0��ֵ���ж�RTC�Ƿ��Ѿ����ù���
	 * ������ù��Ǿͼ������У����û�����ù��ͳ�ʼ��RTC������RTC��ʱ�䡣
	 */

	/* RTC���ã�ѡ��ʱ��Դ������RTC_CLK�ķ�Ƶϵ�� */
	RTC_CLK_Config();

	/* ��ȡ���ݼĴ�����ֵ�������֮ǰû��������ݼĴ��� 
	 * �Ǿ������õ�ǰʱ������� 
	 */
	if (HAL_RTCEx_BKUPRead(&Rtc_Handle,RTC_BKP_DRX) != RTC_BKP_DATA)
	{				
		/* ����ʱ������� */
		RTC_TimeAndDate_Set(tm);
	}
	else
	{
		/* ����Ƿ��Դ��λ */
		if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST) != RESET)
		{
			printf("\r\n ������Դ��λ\r\n");
		}
		/* ����Ƿ��ⲿ��λ */
		else if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST) != RESET)
		{
			printf("\r\n �����ⲿ��λ\r\n");
		}
		printf("\r\n ����Ҫ��������RTC\r\n");
    
		/* ʹ�� PWR ʱ�� */
		__HAL_RCC_RTC_ENABLE();
		/* PWR_CR:DBF��1��ʹ��RTC��RTC���ݼĴ����ͱ���SRAM�ķ��� */
		HAL_PWR_EnableBkUpAccess();
		/* �ȴ� RTC APB �Ĵ���ͬ�� */
		HAL_RTC_WaitForSynchro(&Rtc_Handle);
	} 
}




/**
  * @brief  ����ʱ�������
  * @param  ��
  * @retval ��
  */
void RTC_TimeAndDate_Set(struct rtc_time *tm)
{
		/* ��̫�Ƽ�����bug */
		/* bugԭ�����https://www.cnblogs.com/xiaobaibai2021/p/15716909.html */
	
//  RTC_DateTypeDef  RTC_DateStructure;
//  RTC_TimeTypeDef  RTC_TimeStructure;
//	
//	/* �����ڼ���ʱ��� - ʱ�� */
//	to_tm(mktimev(tm)-TIME_ZOOM, tm);
//	/* �������� */
//	GregorianDay(tm);
//	
//	// ��ʼ��ʱ��
//	RTC_TimeStructure.Hours = tm->tm_hour;         
//	RTC_TimeStructure.Minutes = tm->tm_min;      
//	RTC_TimeStructure.Seconds = tm->tm_sec;      
//  HAL_RTC_SetTime(&Rtc_Handle,&RTC_TimeStructure, RTC_FORMAT_BIN);   // RTC_FORMAT_BIN ��ʾ��ʽΪ 10����
//	
//  // ��ʼ������	
//	RTC_DateStructure.Date = tm->tm_mday;         
//	RTC_DateStructure.Month = tm->tm_mon;         
//	RTC_DateStructure.Year = tm->tm_year-2000;     
//	RTC_DateStructure.WeekDay = tm->tm_wday;       	
//  HAL_RTC_SetDate(&Rtc_Handle,&RTC_DateStructure, RTC_FORMAT_BIN);

	// ֱ��������Ĵ���д��ֵ
	RTC_WriteTimeCounter(&Rtc_Handle, mktimev(tm)-TIME_ZOOM);
	// �򱸷ݼĴ���д���־
  HAL_RTCEx_BKUPWrite(&Rtc_Handle,RTC_BKP_DRX, RTC_BKP_DATA);
}

/**
  * @brief  ��ʾʱ�������
  * @param  ��
  * @retval ��
  */
void RTC_TimeAndDate_Get(struct rtc_time *tm)
{
		/* ��̫�Ƽ�����bug */
	  /* bugԭ�����https://www.cnblogs.com/xiaobaibai2021/p/15716909.html */
	
//	RTC_TimeTypeDef RTC_TimeStructure;
//	RTC_DateTypeDef RTC_DateStructure;
//	
//	// ��ȡ����
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
	// ֱ��������Ĵ�����ȡֵ
	uint32_t TimeVar;
	TimeVar = RTC_ReadTimeCounter(&Rtc_Handle);
	to_tm(TimeVar, tm);
}


/*
 * ��������Time_Display
 * ����  ����ʾ��ǰʱ��ֵ
 * ����  ��-TimeVar RTC����ֵ����λΪ s
 * ���  ����
 * ����  ���ڲ�����
 */	
void Time_Display(struct rtc_time *tm)
{
		static uint32_t FirstDisplay = 1;
		uint32_t TimeVar;
		uint8_t str[200]; // �ַ����ݴ�  	

		/* ��ȡ��ǰʱ�� */
		RTC_TimeAndDate_Get(tm);
		/* ���32λUNIXʱ��� */
		TimeVar = mktimev(tm);
		/* �ѱ�׼ʱ��ת��Ϊ����ʱ�� */
		to_tm(TimeVar+TIME_ZOOM, tm);
	
	  if((!tm->tm_hour && !tm->tm_min && !tm->tm_sec)  || (FirstDisplay))
	  {
	      
	      GetChinaCalendar((u16)tm->tm_year, (u8)tm->tm_mon, (u8)tm->tm_mday, str);	
					printf("\r\n ����������%0.2d%0.2d,%0.2d,%0.2d", str[0], str[1], str[2],  str[3]);
	
	      GetChinaCalendarStr((u16)tm->tm_year,(u8)tm->tm_mon,(u8)tm->tm_mday,str);
					printf("\r\n ����ũ����%s\r\n", str);
	
	     if(GetJieQiStr((u16)tm->tm_year, (u8)tm->tm_mon, (u8)tm->tm_mday, str))
					printf("\r\n ����ũ����%s\r\n", str);
	
	      FirstDisplay = 0;
	  }	 	  	

	  /* ���ʱ���������ʱ�� */
	  printf(" UNIXʱ��� = %d ��ǰʱ��Ϊ: %d��(%s��) %d�� %d�� (����%s)  %0.2d:%0.2d:%0.2d\r",TimeVar,
	                    tm->tm_year, zodiac_sign[(tm->tm_year-3)%12], tm->tm_mon, tm->tm_mday, 
	                    WEEK_STR[tm->tm_wday], tm->tm_hour, 
	                    tm->tm_min, tm->tm_sec);
}

/*
 * ��������Time_Regulate_Get
 * ����  �������û�ʹ�ô������õ�ʱ�䣬
 *         �Ա����ת����ʱ����洢��RTC �����Ĵ����С�
 * ����  �����ڶ�ȡRTCʱ��Ľṹ��ָ��
 * ע��  ���ڴ��ڵ�����������ʱ������������Ҫ�ӻس�
 */
void Time_Regulate_Get(struct rtc_time *tm)
{
	  uint32_t temp_num = 0;
		uint8_t day_max=0 ;

	  printf("\r\n=========================����ʱ��==================");
		
	  do 
	  {
			printf("\r\n  ���������(Please Set Years),��Χ[1970~2038]�������ַ�����ӻس�:");
			scanf("%d",&temp_num);
			if(temp_num <1970 || temp_num >2038)
			{
				printf("\r\n �������������:%d��������Ҫ��",temp_num);
					  
			}
			else
			{	  
				printf("\n\r  ��ݱ�����Ϊ: %d\n\r", temp_num);

				tm->tm_year = temp_num;
				break;
			}
	  }while(1);


	 do 
	  {
			printf("\r\n  �������·�(Please Set Months):��Χ[1~12]�������ַ�����ӻس�:");
			scanf("%d",&temp_num);
			if(temp_num <1 || temp_num >12)
			{
				printf("\r\n �������������:%d��������Ҫ��",temp_num);
					  
			}
			else
			{	  
				printf("\n\r  �·ݱ�����Ϊ: %d\n\r", temp_num);

				tm->tm_mon = temp_num;
				break;
			}
	  }while(1);
		
		/*�����·ݼ����������*/
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
				     /*��������*/
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
			printf("\r\n  ����������(Please Set Months),��Χ[1~%d]�������ַ�����ӻس�:",day_max);
			scanf("%d",&temp_num);
			
			if(temp_num <1 || temp_num >day_max)
			{
				printf("\r\n �������������:%d��������Ҫ��",temp_num);
			}
			else
			{
				printf("\n\r  ���ڱ�����Ϊ: %d\n\r", temp_num);

				tm->tm_mday = temp_num;
				break;
			}
	  }while(1);
		
		do 
	  {				
			printf("\r\n  ������ʱ��(Please Set Hours),��Χ[0~23]�������ַ�����ӻس�:");
			scanf("%d",&temp_num);
			
			if( temp_num >23)
			{
				printf("\r\n �������������:%d��������Ҫ��",temp_num);
			}
			else
			{
				printf("\n\r  ʱ�ӱ�����Ϊ: %d\n\r", temp_num);

				tm->tm_hour = temp_num;
				break;
			}
	  }while(1);

		do 
	  {				
			printf("\r\n  ���������(Please Set Minutes),��Χ[0~59]�������ַ�����ӻس�:");
			scanf("%d",&temp_num);
			
			if( temp_num >59)
			{
				printf("\r\n �������������:%d��������Ҫ��",temp_num);
			}
			else
			{
				printf("\n\r  ���ӱ�����Ϊ: %d\n\r", temp_num);

				tm->tm_min = temp_num;
				break;
			}
	  }while(1);

		do 
	  {				
			printf("\r\n  ����������(Please Set Seconds),��Χ[0~59]�������ַ�����ӻس�:");
			scanf("%d",&temp_num);
			
			if( temp_num >59)
			{
				printf("\r\n �������������:%d��������Ҫ��",temp_num);
			}
			else
			{
				printf("\n\r  ���ӱ�����Ϊ: %d\n\r", temp_num);

				tm->tm_sec = temp_num;
				break;
			}
	  }while(1);

}

/**********************************END OF FILE*************************************/
