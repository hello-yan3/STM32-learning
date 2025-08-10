/**
 ****************************************************************************************************
 * @file        main.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-28
 * @brief       �ڴ���� ʵ��
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� STM32F103������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
//#include "./USMART/usmart.h"
#include "./BSP/LED/led.h"
#include "./BSP/KEY/key.h"
#include "./MALLOC/malloc.h"
#include <stdlib.h>

#define SRAMBANK    1       /* ����֧�ֵ�SRAM����. */
const char *SRAM_NAME_BUF[SRAMBANK] = {" SRAMIN "};

/* �� ���ǹ����ڴ����֪ʶ���� */
/* https://zhuanlan.zhihu.com/p/531609461 */
/*
C�����Դ�malloc���⣺ʹ�� malloc ���������Ժ󣬻�����ܶ���ڴ���Ƭ��
��ˣ�Ϊ�����ڴ���Ƭ���⣬��Ҫʹ���ڴ����

�������ʹ�� ����ԭ�� �� mymalloc ����

��Ƭ�������û��ǲ��� malloc ������

�������� ��ЩС��Ŀ�����û���Լ���**�ڴ����ʽ**���Ǳ�Ҫ��������ʹ�ã�ͬʱΪ�˽�Լ�ڴ棬���԰�heap���óɺ�С����һ������ܵ��õ�C�⺯�����õ�����
����ܲ���ϵͳ������ϵͳ�����Ƶ��ڴ��������ʹ���ʹ��**����ϵͳ**��malloc�ӿں����� ���������ЩС��Ŀ��Ҳ�ǿ��Բ��õġ�
*/

/*
�� ����ԭ�ӵģ�malloc.c �ӿں���
1. my_mem_init(0);  ��ʼ��
2. mymalloc(0, size);    �ڴ���䣬ʹ�÷�����malloc()��ͬ
3. myfree(0, *ptr);      �ڴ��ͷţ�ʹ�÷�����free()��ͬ

* ���þͶ��ˣ���Ҫ����������*
*/

/*
�� ��ʵ��������Ϊ��������������Ϊ��
ϵͳ�Դ��� malloc() �����ݶ����� �� ����
����ԭ�ӵ� mymalloc() �����ݶ����� ջ ����
*/

/*
�� �ڴ�ռ�ñȽ�
��ʹ��ϵͳ�Դ� malloc()  (ע�͵���������ԭ������ڴ���䣬����malloc.cȫ��ע��)
Program Size: Code=7400 RO-data=400 RW-data=28 ZI-data=1900  
��ʹ������ԭ�� mymalloc()  (ע�͵�#include <stdlib.h>)
Program Size: Code=7344 RO-data=412 RW-data=48 ZI-data=12784  
EMMMM������������
���� �ҵ������� ���� C���Դ���
*/

/*
��
Code        Flash           ����
RO-Data     Flash           ֻ�����ݣ�һ����ָconst ���ε�����
RW-Data     Flash and SRAM  ��ֵΪ��O�Ŀɶ���д����
Zl-Data     SRAM            ��ֵΪ0�Ŀɶ���д����

*/
int main(void)
{
//    uint8_t paddr[20];      /* ���P Addr:+p��ַ��ASCIIֵ */
//    uint16_t memused = 0;
//    uint8_t key;
    uint16_t i = 0;
    uint8_t *p = 0;
//    uint8_t sramx = 0;      /* Ĭ��Ϊ�ڲ�sram */

    HAL_Init();                         /* ��ʼ��HAL�� */
    sys_stm32_clock_init(RCC_PLL_MUL9); /* ����ʱ��, 72Mhz */
    delay_init(72);                     /* ��ʱ��ʼ�� */
    usart_init(115200);                 /* ���ڳ�ʼ��Ϊ115200 */
//    usmart_dev.init(72);                /* ��ʼ��USMART */
    led_init();                         /* ��ʼ��LED */
    key_init();                         /* ��ʼ������ */

    /****************************************************/
  
    my_mem_init(SRAMIN);                /* ��ʼ���ڲ�SRAM�ڴ�� */

    printf("ʹ���ڴ���� �Զ����ڴ���亯�� ����\n");
  
    p = mymalloc(0, 256*sizeof(char));  /* ����2K�ֽ�,��д������ */
    for(i=0; i<256; i++)
    {
        p[i] = (i%10)+48;
    }
    p[i] = 0;
    
    printf("���ݵ�ַΪ%p\n", p);
    printf("�������Ϊ%s\n", p);
    
    myfree(0, p);   /* �ͷ��ڴ� */ 
     
    /****************************************************/
    
    printf("ʹ��C���Կ� �ڴ���亯�� ����\n");
    
    p = malloc(256*sizeof(char));  /* ����2K�ֽ�,��д������ */
    for(i=0; i<256; i++)
    {
        p[i] = (i%10)+48;
    }
    p[i] = 0;
    
    printf("���ݵ�ַΪ%p\n", p);
    printf("�������Ϊ%s\n", p);
    
    free(p);   /* �ͷ��ڴ� */
    
    while (1)
    {
//        key = key_scan(0);      /* ��֧������ */
//        
//        switch (key)
//        {
//            case KEY2_PRES:     /* KEY0���� */

//                p = mymalloc(sramx, 2048);  /* ����2K�ֽ�,��д������ */

//                if (p != NULL)
//                {
//                    sprintf((char *)p, "Memory Malloc Test%03d", i);        /* ��pд��һЩ���� */
//                    printf("д�����ݣ�%s\n", p);   /* ��ʾp������ */
//                    printf("д���ַ��%p\n", p);   /* ��ʾp������ */
//                }

//                break;

//            case KEY1_PRES:         /* KEY1���� */
//                myfree(sramx, p);   /* �ͷ��ڴ� */
//                p = 0;              /* ָ��յ�ַ */
//                break;
//        }
//        
//        delay_ms(10);
//        i++;

//        if ((i % 50) == 0)
//        {
//            memused = my_mem_perused(SRAMIN);
//            sprintf((char *)paddr, "%d.%01d%%", memused / 10, memused % 10);
//            printf("�ڲ��ڴ�ʹ���ʣ�%p\n\n\n\n", p);   /* ��ʾp������ */

//            LED0_TOGGLE();  /* LED0��˸ */
//        }       
    }
}





























