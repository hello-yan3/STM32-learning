/******************** (C) COPYRIGHT 2009 www.armjishu.com ************************
* File Name          : date.h
* Author             : www.armjishu.com Team
* Version            : V1.0
* Date               : 12/1/2009
* Description        : ������غ���
*******************************************************************************/
#ifndef __DATE_H
#define __DATE_H

#include "stm32f1xx.h"

typedef uint32_t  u32;
typedef uint16_t  u16;
typedef uint8_t   u8;

struct rtc_time {
	uint8_t  tm_sec;   // ��
	uint8_t  tm_min;   // ��
	uint8_t  tm_hour;  // ʱ
	uint8_t  tm_mday;  // ��
	uint8_t  tm_mon;   // ��
	uint16_t tm_year;  // ��
	uint8_t  tm_wday;  // ����
};
    
//����ʱ���ʱ��������
#define TIME_ZOOM						(8*60*60)

void GregorianDay(struct rtc_time * tm);
uint32_t mktimev(struct rtc_time *tm);
void to_tm(uint32_t tim, struct rtc_time * tm);
#endif 
