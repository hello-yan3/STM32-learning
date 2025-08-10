#include "led.h"


/**************************************************************************
Function: Led flashing
Input   : time��Flicker frequency
Output  : none
�������ܣ�LED��˸
��ڲ�������˸Ƶ�� 
����  ֵ����
**************************************************************************/
void Led_Flash(u16 time)
{
	 static int temp;
	 if(0==time) LED=0;
	 else 	if(++temp==time)	LED=~LED,temp=0;
}

