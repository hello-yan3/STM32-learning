/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-28
 * @brief       内存管理 实验
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32F103开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
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

#define SRAMBANK    1       /* 定义支持的SRAM块数. */
const char *SRAM_NAME_BUF[SRAMBANK] = {" SRAMIN "};

/* ② 还是关于内存管理知识讲解 */
/* https://zhuanlan.zhihu.com/p/531609461 */
/*
C语言自带malloc问题：使用 malloc 函数多了以后，会产生很多的内存碎片；
因此，为避免内存碎片问题，需要使用内存管理；

因此这里使用 正点原子 的 mymalloc 函数

单片机领域，用还是不用 malloc 函数？

如果跑裸机 做些小项目，如果没有自己的**内存管理方式**不是必要都不建议使用，同时为了节约内存，可以把heap设置成很小（留一点给可能调用的C库函数会用到）。
如果跑操作系统，操作系统有完善的内存管理，可以痛快的使用**操作系统**的malloc接口函数。 但是如果做些小项目，也是可以不用的。
*/

/*
③ 正点原子的：malloc.c 接口函数
1. my_mem_init(0);  初始化
2. mymalloc(0, size);    内存分配，使用方法与malloc()相同
3. myfree(0, *ptr);      内存释放，使用方法与free()相同

* 会用就对了，不要深究这个！！！*
*/

/*
④ 其实，个人认为，二者最大的区别为：
系统自带的 malloc() ，数据定义在 堆 区！
正点原子的 mymalloc() ，数据定义在 栈 区！
*/

/*
⑤ 内存占用比较
仅使用系统自带 malloc()  (注释掉所有正点原子相关内存分配，比如malloc.c全部注释)
Program Size: Code=7400 RO-data=400 RW-data=28 ZI-data=1900  
仅使用正点原子 mymalloc()  (注释掉#include <stdlib.h>)
Program Size: Code=7344 RO-data=412 RW-data=48 ZI-data=12784  
EMMMM……啊？……
所以 我的评价是 不如 C库自带。
*/

/*
⑥
Code        Flash           代码
RO-Data     Flash           只读数据，一般是指const 修饰的数据
RW-Data     Flash and SRAM  初值为非O的可读可写数据
Zl-Data     SRAM            初值为0的可读可写数据

*/
int main(void)
{
//    uint8_t paddr[20];      /* 存放P Addr:+p地址的ASCII值 */
//    uint16_t memused = 0;
//    uint8_t key;
    uint16_t i = 0;
    uint8_t *p = 0;
//    uint8_t sramx = 0;      /* 默认为内部sram */

    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(RCC_PLL_MUL9); /* 设置时钟, 72Mhz */
    delay_init(72);                     /* 延时初始化 */
    usart_init(115200);                 /* 串口初始化为115200 */
//    usmart_dev.init(72);                /* 初始化USMART */
    led_init();                         /* 初始化LED */
    key_init();                         /* 初始化按键 */

    /****************************************************/
  
    my_mem_init(SRAMIN);                /* 初始化内部SRAM内存池 */

    printf("使用内存管理 自定义内存分配函数 试验\n");
  
    p = mymalloc(0, 256*sizeof(char));  /* 申请2K字节,并写入内容 */
    for(i=0; i<256; i++)
    {
        p[i] = (i%10)+48;
    }
    p[i] = 0;
    
    printf("数据地址为%p\n", p);
    printf("输出数据为%s\n", p);
    
    myfree(0, p);   /* 释放内存 */ 
     
    /****************************************************/
    
    printf("使用C语言库 内存分配函数 试验\n");
    
    p = malloc(256*sizeof(char));  /* 申请2K字节,并写入内容 */
    for(i=0; i<256; i++)
    {
        p[i] = (i%10)+48;
    }
    p[i] = 0;
    
    printf("数据地址为%p\n", p);
    printf("输出数据为%s\n", p);
    
    free(p);   /* 释放内存 */
    
    while (1)
    {
//        key = key_scan(0);      /* 不支持连按 */
//        
//        switch (key)
//        {
//            case KEY2_PRES:     /* KEY0按下 */

//                p = mymalloc(sramx, 2048);  /* 申请2K字节,并写入内容 */

//                if (p != NULL)
//                {
//                    sprintf((char *)p, "Memory Malloc Test%03d", i);        /* 向p写入一些内容 */
//                    printf("写入内容：%s\n", p);   /* 显示p的内容 */
//                    printf("写入地址：%p\n", p);   /* 显示p的内容 */
//                }

//                break;

//            case KEY1_PRES:         /* KEY1按下 */
//                myfree(sramx, p);   /* 释放内存 */
//                p = 0;              /* 指向空地址 */
//                break;
//        }
//        
//        delay_ms(10);
//        i++;

//        if ((i % 50) == 0)
//        {
//            memused = my_mem_perused(SRAMIN);
//            sprintf((char *)paddr, "%d.%01d%%", memused / 10, memused % 10);
//            printf("内部内存使用率：%p\n\n\n\n", p);   /* 显示p的内容 */

//            LED0_TOGGLE();  /* LED0闪烁 */
//        }       
    }
}





























