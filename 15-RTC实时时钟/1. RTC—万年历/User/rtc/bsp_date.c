/**
  ******************************************************************************
  * @file    bsp_date.c
  * @author  移植自linux万年历
  * @version V1.0
  * @date    2013-xx-xx
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 F103-霸道 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */

#include "./rtc/bsp_date.h"

#define FEBRUARY		  2         // 二月
#define	STARTOFTIME		1970      // 计时元年
#define SECDAY			  86400L    // 一天有多少s 
#define SECYR			(SECDAY * 365)  // 一年有多少s 
#define	leapyear(year)		((year) % 4 == 0)      // 判断闰年
#define	days_in_year(a) 	(leapyear(a) ? 366 : 365)
#define	days_in_month(a) 	(month_days[(a) - 1])  // 返回每个月有多少天

static int month_days[12] = {	31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };


/* 计算星期几 */
void GregorianDay(struct rtc_time * tm)
{
	int leapsToDate;
	int lastYear;
	int day;
	int MonthOffset[] = { 0,31,59,90,120,151,181,212,243,273,304,334 };   // 累计日期

	lastYear=tm->tm_year-1;

	/*计算从公元元年到计数的前一年之中一共经历了多少个闰年*/
	leapsToDate = lastYear/4 - lastYear/100 + lastYear/400;      

     /*如若计数的这一年为闰年，且计数的月份在2月之后，则日数加1，否则不加1*/
	if((tm->tm_year%4==0) &&
	   ((tm->tm_year%100!=0) || (tm->tm_year%400==0)) &&
	   (tm->tm_mon>2)) 
	{
		/*
		 * We are past Feb. 29 in a leap year
		 * 我们已经过了闰年的2月29日
		 */
		day=1;
	} 
	else 
	{
		day=0;
	}
	
  // 从起点开始所经历的所有时间
	day += lastYear*365 + leapsToDate + MonthOffset[tm->tm_mon-1] + tm->tm_mday; /*计算从公元元年元旦到计数日期一共有多少天*/
  // 计算得到星期
	tm->tm_wday=day%7;
}


/*
 * 将自1970-01-01 00:00:00起的公历日期转换为秒。
 * 假设输入为正常日期格式，即1980-12-31 23:59:59
 * =>年=1980，月=12，日=31，时=23，分=59，秒=59。
 */
/* tm 时间数据结构 转换为 32位 UNIX时间戳 */ 
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
	    )*24 + tm->tm_hour /* now have hours 现在有多少小时 */
	  )*60 + tm->tm_min    /* now have minutes 现在有多少分钟 */
	)*60 + tm->tm_sec;     /* finally seconds 现在有多少秒 */	 
}

/* 32位 UNIX时间戳 转换为 tm 时间数据结构 */
void to_tm(u32 tim, struct rtc_time * tm)
{
	register u32    i;
	register long   hms, day;

	day = tim / SECDAY;			/* 有多少天 */
	hms = tim % SECDAY;			/* 今天的时间，单位s */

	/* Hours, minutes, seconds are easy 时、分、秒计算获取*/
	tm->tm_hour = hms / 3600;
	tm->tm_min = (hms % 3600) / 60;
	tm->tm_sec = (hms % 3600) % 60;

	/* Number of years in days */ /*算出当前年份，起始的计数年份为1970年*/
	for (i = STARTOFTIME; day >= days_in_year(i); i++) 
	{
		day -= days_in_year(i);
	}
	tm->tm_year = i;

	/* Number of months in days left */ /*计算当前的月份*/
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

	/* Days are what is left over (+1) from all that. *//*计算当前日期*/
	tm->tm_mday = day + 1;

	/*
	 * Determine the day of week  确定星期几
	 */
	GregorianDay(tm);
	
}

