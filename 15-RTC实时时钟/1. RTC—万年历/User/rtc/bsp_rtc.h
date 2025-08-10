#ifndef __RTC_H
#define	__RTC_H

#include "stm32f1xx.h"
#include "./rtc/bsp_date.h"
#include "./rtc/bsp_calendar.h"

extern RTC_HandleTypeDef Rtc_Handle;
extern uint8_t TimeDisplay ;

// ʱ��Դ�궨��
#define RTC_CLOCK_SOURCE_LSE      
//#define RTC_CLOCK_SOURCE_LSI  

// ������Ĵ����궨��
#define RTC_BKP_DRX          RTC_BKP_DR1
// д�뵽���ݼĴ��������ݺ궨��
#define RTC_BKP_DATA         0xA5A5

void RTC_CLK_Config(void);
void RTC_CheckAndConfig(struct rtc_time *tm);

void RTC_TimeAndDate_Set(struct rtc_time *tm);
void RTC_TimeAndDate_Get(struct rtc_time *tm);
void Time_Display(struct rtc_time *tm);
void Time_Regulate_Get(struct rtc_time *tm);


#endif /* __XXX_H */
