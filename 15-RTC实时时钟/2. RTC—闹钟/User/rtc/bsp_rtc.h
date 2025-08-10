
#ifndef __RTC_H
#define	__RTC_H

#include "stm32f1xx.h"

////�Ƿ�ʹ��LCD��ʾ
//#define USE_LCD_DISPLAY

extern RTC_HandleTypeDef Rtc_Handle;
extern __IO uint8_t Alarmflag ;
// ʱ��Դ�궨��
#define RTC_CLOCK_SOURCE_LSE      
//#define RCC_OSCILLATORTYPE_LSI  
 
#define HOURS                     12          // 0~23
#define MINUTES                   12          // 0~59
#define SECONDS                   50          // 0~59


// ���ں궨��  ����WEEKDAYΪ��Ч�궨�壬ֻҪ���������գ�HAL����Զ������ WEEKDAY
#define WEEKDAY                   6           // 1~7
#define DATE                      24        // 1~31
#define MONTH                     12        // 1~12
#define YEAR                      18        // 0~99


// ������غ궨��
#define ALARM_HOURS             12         // 0~23
#define ALARM_MINUTES           13          // 0~59
#define ALARM_SECONDS           0       // 0~59

// ������Ĵ����궨��
#define RTC_BKP_DRX          RTC_BKP_DR1
// д�뵽���ݼĴ��������ݺ궨��
#define RTC_BKP_DATA         0X3522 
                                  
void RTC_CalendarConfig(void);
void RTC_TimeAndDate_Show(void);
void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc);
void RTC_AlarmSet(void);

#endif /* __XXX_H */
