/******************** (C) COPYRIGHT 2009 www.armjishu.com ************************
* File Name          : date.h
* Author             : www.armjishu.com Team
* Version            : V1.0
* Date               : 12/1/2009
* Description        : 日期相关函数
*******************************************************************************/
#ifndef __DATE_H
#define __DATE_H

#include "stm32f1xx.h"

typedef uint32_t  u32;
typedef uint16_t  u16;
typedef uint8_t   u8;

struct rtc_time {
	uint8_t  tm_sec;   // 秒
	uint8_t  tm_min;   // 分
	uint8_t  tm_hour;  // 时
	uint8_t  tm_mday;  // 日
	uint8_t  tm_mon;   // 月
	uint16_t tm_year;  // 年
	uint8_t  tm_wday;  // 星期
};
    
//北京时间的时区秒数差
#define TIME_ZOOM						(8*60*60)

void GregorianDay(struct rtc_time * tm);
uint32_t mktimev(struct rtc_time *tm);
void to_tm(uint32_t tim, struct rtc_time * tm);
#endif 
