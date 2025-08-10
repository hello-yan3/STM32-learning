/**
  ******************************************************************************
  * @file    bsp_date.c
  * @author  ��ֲ��linux������
  * @version V1.0
  * @date    2013-xx-xx
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103-�Ե� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */

#include "./rtc/bsp_date.h"

#define FEBRUARY		  2         // ����
#define	STARTOFTIME		1970      // ��ʱԪ��
#define SECDAY			  86400L    // һ���ж���s 
#define SECYR			(SECDAY * 365)  // һ���ж���s 
#define	leapyear(year)		((year) % 4 == 0)      // �ж�����
#define	days_in_year(a) 	(leapyear(a) ? 366 : 365)
#define	days_in_month(a) 	(month_days[(a) - 1])  // ����ÿ�����ж�����

static int month_days[12] = {	31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };


/* �������ڼ� */
void GregorianDay(struct rtc_time * tm)
{
	int leapsToDate;
	int lastYear;
	int day;
	int MonthOffset[] = { 0,31,59,90,120,151,181,212,243,273,304,334 };   // �ۼ�����

	lastYear=tm->tm_year-1;

	/*����ӹ�ԪԪ�굽������ǰһ��֮��һ�������˶��ٸ�����*/
	leapsToDate = lastYear/4 - lastYear/100 + lastYear/400;      

     /*������������һ��Ϊ���꣬�Ҽ������·���2��֮����������1�����򲻼�1*/
	if((tm->tm_year%4==0) &&
	   ((tm->tm_year%100!=0) || (tm->tm_year%400==0)) &&
	   (tm->tm_mon>2)) 
	{
		/*
		 * We are past Feb. 29 in a leap year
		 * �����Ѿ����������2��29��
		 */
		day=1;
	} 
	else 
	{
		day=0;
	}
	
  // ����㿪ʼ������������ʱ��
	day += lastYear*365 + leapsToDate + MonthOffset[tm->tm_mon-1] + tm->tm_mday; /*����ӹ�ԪԪ��Ԫ������������һ���ж�����*/
  // ����õ�����
	tm->tm_wday=day%7;
}


/*
 * ����1970-01-01 00:00:00��Ĺ�������ת��Ϊ�롣
 * ��������Ϊ�������ڸ�ʽ����1980-12-31 23:59:59
 * =>��=1980����=12����=31��ʱ=23����=59����=59��
 */
/* tm ʱ�����ݽṹ ת��Ϊ 32λ UNIXʱ��� */ 
u32 mktimev(struct rtc_time *tm)
{
	if (0 >= (int) (tm->tm_mon -= 2)) 
	{	/* 1..12 -> 11,12,1..10 */
		tm->tm_mon += 12;		/* Puts Feb last since it has leap day */
		tm->tm_year -= 1;
	}

	return (((
		(u32) (tm->tm_year/4 - tm->tm_year/100 + tm->tm_year/400 + 367*tm->tm_mon/12 + tm->tm_mday) +
			tm->tm_year*365 - 719499 
	    )*24 + tm->tm_hour /* now have hours �����ж���Сʱ */
	  )*60 + tm->tm_min    /* now have minutes �����ж��ٷ��� */
	)*60 + tm->tm_sec;     /* finally seconds �����ж����� */	 
}

/* 32λ UNIXʱ��� ת��Ϊ tm ʱ�����ݽṹ */
void to_tm(u32 tim, struct rtc_time * tm)
{
	register u32    i;
	register long   hms, day;

	day = tim / SECDAY;			/* �ж����� */
	hms = tim % SECDAY;			/* �����ʱ�䣬��λs */

	/* Hours, minutes, seconds are easy ʱ���֡�������ȡ*/
	tm->tm_hour = hms / 3600;
	tm->tm_min = (hms % 3600) / 60;
	tm->tm_sec = (hms % 3600) % 60;

	/* Number of years in days */ /*�����ǰ��ݣ���ʼ�ļ������Ϊ1970��*/
	for (i = STARTOFTIME; day >= days_in_year(i); i++) 
	{
		day -= days_in_year(i);
	}
	tm->tm_year = i;

	/* Number of months in days left */ /*���㵱ǰ���·�*/
	if (leapyear(tm->tm_year)) 
	{
		days_in_month(FEBRUARY) = 29;
	}
	for (i = 1; day >= days_in_month(i); i++) 
	{
		day -= days_in_month(i);
	}
	days_in_month(FEBRUARY) = 28;
	tm->tm_mon = i;

	/* Days are what is left over (+1) from all that. *//*���㵱ǰ����*/
	tm->tm_mday = day + 1;

	/*
	 * Determine the day of week  ȷ�����ڼ�
	 */
	GregorianDay(tm);
	
}

