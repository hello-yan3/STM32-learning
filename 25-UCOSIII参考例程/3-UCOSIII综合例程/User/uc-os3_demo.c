/**
 ****************************************************************************************************
 * @file        uc-os3_demo.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-04-13
 * @brief       uC/OS-III 移植实验
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 探索者 F407开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include "uc-os3_demo.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/LED/led.h"
#include "./BSP/KEY/key.h"
#include "./BSP/ADC/adc.h"
#include "./BSP/AP3216C/ap3216c.h"
#include "./BSP/I2C/i2c.h"
#include "./SYSTEM/delay/delay.h"
/*uC/OS-III*********************************************************************************************/
#include "os.h"
#include "cpu.h"

/******************************************************************************************************/
/*uC/OS-III配置*/

#define     Default_Tick            25       /* 默认时间片 */

/* 开始任务 */
#define     START_TASK_PRIO         2         /* 任务优先级 */
#define     START_STK_SIZE          512       /* 任务栈大小(实际任务栈 = START_STK_SIZE * 4(CPU_STK)类型) */
#define     START_STK_TICK          0         /* 任务时间片 */
OS_TCB      StartTask_TCB;                    /* 任务控制块 */
CPU_STK     StartTask_STK[START_STK_SIZE];    /* 任务栈 */
void start_task(void *p_arg);                 /* 任务函数 */
/* 注意：创建的任务不能够使用 数值为0、1、OS_CFG_PRIO_MAX-2 和 OS_CFG_PRIO_MAX-1 的任务优先级 */


/* AD采集任务 */
#define     ADC_PRIO                4    
#define     ADC_STK_SIZE            256  
#define     ADC_STK_TICK            0
OS_TCB      ADCTask_TCB;                
CPU_STK     ADCTask_STK[ADC_STK_SIZE];                
void ADC_task(void *p_arg);               

/* I2C获取数据任务 */
#define     I2C_PRIO                4      
#define     I2C_STK_SIZE            256     
#define     I2C_STK_TICK            0        
OS_TCB      I2CTask_TCB;                  
CPU_STK     I2CTask_STK[I2C_STK_SIZE];                     
void I2C_task(void *p_arg);                  

/* 按键按下任务 */
#define     KEY1_PRIO                5      
#define     KEY1_STK_SIZE            256 
#define     KEY1_STK_TICK            0      
OS_TCB      KEY1Task_TCB;                
CPU_STK     KEY1Task_STK[KEY1_STK_SIZE];                
void KEY1_task(void *p_arg);      

/* 数据输出任务1 */
#define     SHOW1_PRIO               3
#define     SHOW1_STK_SIZE           256   
#define     SHOW1_STK_TICK           0      
OS_TCB      SHOW1Task_TCB;                
CPU_STK     SHOW1Task_STK[SHOW1_STK_SIZE];                
void SHOW1_task(void *p_arg);       

/******************************************************************************************************/
/* 软件定时器，当前设置 200 ms 进入一次*/
OS_TMR   timer;                                    /* 软件定时器 */
void     timer_cb(OS_TMR *p_tmr, void *p_arg);     /* 软件定时器的回调函数 */
/* 
OS_CFG_TMR_TASK_RATE_HZ变量表示定时周期，
#define  OS_CFG_TMR_TASK_RATE_HZ                          100u
表示单个定时周期 1/100 = 10ms

实际定时器周期 = timer_period * (1/OS_CFG_TMR_TASK_RATE_HZ)
*/
#define  timer_period    20                        

OS_Q    ADC_queue;                              /* 数据队列 */
OS_Q    I2C_queue;                              /* 数据队列 */

/* 二值信号量 */
OS_SEM KEY1_binary_sem;
OS_SEM KEY2_binary_sem;
/* 计数信号量，统计按键按下次数 */
OS_SEM count_sem;

/* 任务标志组 */
OS_FLAG_GRP flag;
#define FLAG_BIT0    (1 << 0)
#define FLAG_BIT1    (1 << 1)
/******************************************************************************************************/
/**
 * @brief       uC/OS-III例程入口函数
 * @param       无
 * @retval      无
 */
void uc_os3_demo(void)
{
    OS_ERR err;
    
    /* 初始化uC/OS-III */
    OSInit(&err);
    
    /* 创建Start Task */
    /* 当任务被创建好后，立马处于就绪态 */
    OSTaskCreate(   (OS_TCB        *)&StartTask_TCB,          /* 指向任务控制块的指针，类似HAL库句柄结构体 */
                    (CPU_CHAR      *)"start_task",            /* 指向任务的名字 */
                    (OS_TASK_PTR    )start_task,              /* 指向任务函数的指针(任务执行操作写在任务函数内) */	
                    (void          *)0,                       /* 传递给任务函数的参数(没有参数就是NULL指针(0)) */	
                    (OS_PRIO        )START_TASK_PRIO,         /* 任务的任务优先级，数字越小，优先级越高 */
                    (CPU_STK       *)StartTask_STK,           /* 指向任务栈的起始地址的指针 (任务栈解释 详见 6.)*/	
                    (CPU_STK_SIZE   )START_STK_SIZE / 10,     /* 任务栈的使用警戒线(这里定义是90%) */
                    (CPU_STK_SIZE   )START_STK_SIZE,          /* 任务栈大小 */
                    (OS_MSG_QTY     )0,                       /* 任务内嵌消息队列的大小(不知道有啥用，设为0，表示默认值) */
                    (OS_TICK        )START_STK_TICK,          /* 任务的时间片(时间片调度需要，当前设置一个时间片表示1ms，默认值0表示100个时间片) */
                    (void          *)0,                       /* 指向用户扩展内存的指针(不知道有啥用，设为0，表示默认值) */
                    (OS_OPT         )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),   /* 任务选项共有5个，STK_CHK 表示 允许对任务进行堆栈检查，STK_CLR 表示 需要清除任务堆栈 */
                    (OS_ERR        *)&err);                   /* 指向接收错误代码变量指针(相当于错误码，0表示没有错误) */
    
    printf("开始任务创建完毕\r\n");                
                    
    /* 开始任务调度 */
    OSStart(&err);
}

/**
 * @brief       start_task
 * @param       p_arg : 传入参数(未用到)
 * @retval      无
 */
void start_task(void *p_arg)
{
    OS_ERR err;
    CPU_INT32U cnts;
    
    /* 初始化CPU库 */
    CPU_Init();
    
    /* 根据配置的节拍频率配置SysTick */
    cnts = (CPU_INT32U)(HAL_RCC_GetSysClockFreq() / OSCfg_TickRate_Hz);
    OS_CPU_SysTickInit(cnts);
    
    /* 创建队列 */
    /* 队列句柄结构体  ， 消息队列的大小 */
    OSQCreate (&ADC_queue , "ADC_queue" , 1 , &err);
    OSQCreate (&I2C_queue , "I2C_queue" , 1 , &err);
  
    /* 创建一个事件标志组 */
    /* 事件标志组句柄结构体  ， 事件标志组的初始值 */
    OSFlagCreate (&flag , "flag" , 0 , &err);
    
    /* 创建信号量 */
    /* 信号量句柄结构体  ，  信号量资源数的初始值 */
    OSSemCreate (&KEY1_binary_sem , "KEY1_binary_sem" , 0 , &err);
    OSSemCreate (&KEY2_binary_sem , "KEY2_binary_sem" , 0 , &err);  
    OSSemCreate (&count_sem , "count_sem" , 0 , &err);  
  
    /* 创建软件定时器 */
    OSTmrCreate (   &timer,    // 软件定时器句柄结构体
                    "timer", 
                    0,             // 软件定时器的开启延时时间，注意单次定时器必须大于0
                    timer_period,  // 周期定时器的定时周期时间，注意周期定时器必须大于0 
                    OS_OPT_TMR_PERIODIC,             // OS_OPT_TMR_ONE_SHOT（单次定时器） OS_OPT_TMR_PERIODIC（周期定时器）
                    (OS_TMR_CALLBACK_PTR)timer_cb,   // 指向超时回调函数的指针
                    0,         // 超时回调函数入口参数
                    &err    );
  
    printf("队列、信号、定时器等创建完毕\r\n");      
  
    /* 开启时间片调度 */
    OSSchedRoundRobinCfg(OS_TRUE, Default_Tick, &err);
    
    CPU_SR_ALLOC();    /* 进入临界区之前必须先调用此函数 */
    CPU_CRITICAL_ENTER();           /* 进入临界区 */
    
    /* 创建ADCTask */
    OSTaskCreate(   (OS_TCB        *)&ADCTask_TCB,   
                    (CPU_CHAR      *)"ADCTask",          
                    (OS_TASK_PTR    )ADC_task,            
                    (void          *)0,               
                    (OS_PRIO        )ADC_PRIO,        
                    (CPU_STK       *)ADCTask_STK,      
                    (CPU_STK_SIZE   )ADC_STK_SIZE / 10,
                    (CPU_STK_SIZE   )ADC_STK_SIZE,  
                    (OS_MSG_QTY     )0,                 
                    (OS_TICK        )ADC_STK_TICK,                
                    (void          *)0,                 
                    (OS_OPT         )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),  
                    (OS_ERR        *)&err);              
    
    /* 创建I2CTask */    
    OSTaskCreate(   (OS_TCB        *)&I2CTask_TCB,
                    (CPU_CHAR      *)"I2CTask",
                    (OS_TASK_PTR    )I2C_task,
                    (void          *)0,
                    (OS_PRIO        )I2C_PRIO,
                    (CPU_STK       *)I2CTask_STK,
                    (CPU_STK_SIZE   )I2C_STK_SIZE / 10,
                    (CPU_STK_SIZE   )I2C_STK_SIZE,
                    (OS_MSG_QTY     )0,
                    (OS_TICK        )I2C_STK_TICK,
                    (void          *)0,
                    (OS_OPT         )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                    (OS_ERR        *)&err);
                        
    /* 创建KEY1Task */
    OSTaskCreate(   (OS_TCB*     )  &KEY1Task_TCB,
                    (CPU_CHAR*   )  "KEY1Task",
                    (OS_TASK_PTR )  KEY1_task,
                    (void*       )  0,
                    (OS_PRIO     )  KEY1_PRIO,
                    (CPU_STK*    )  KEY1Task_STK,
                    (CPU_STK_SIZE)  KEY1_STK_SIZE / 10,
                    (CPU_STK_SIZE)  KEY1_STK_SIZE,
                    (OS_MSG_QTY  )  0,
                    (OS_TICK     )  KEY1_STK_TICK,
                    (void*       )  0,
                    (OS_OPT      )  (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                    (OS_ERR*     )  &err);
                                     
    /* 创建SHOW1Task */
    OSTaskCreate(   (OS_TCB*     )  &SHOW1Task_TCB,
                    (CPU_CHAR*   )  "SHOW1Task",
                    (OS_TASK_PTR )  SHOW1_task,
                    (void*       )  0,
                    (OS_PRIO     )  SHOW1_PRIO,
                    (CPU_STK*    )  SHOW1Task_STK,
                    (CPU_STK_SIZE)  SHOW1_STK_SIZE / 10,
                    (CPU_STK_SIZE)  SHOW1_STK_SIZE,
                    (OS_MSG_QTY  )  0,
                    (OS_TICK     )  SHOW1_STK_TICK,
                    (void*       )  0,
                    (OS_OPT      )  (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                    (OS_ERR*     )  &err);
            
                                       
    printf("任务创建完毕\r\n");     
  
    // 开始软件定时器
    OSTmrStart (&timer , &err);
    if(err != OS_ERR_NONE)
        printf("执行定时器，错误码为%d", err);
    
    printf("定时器开始\r\n");           
                    
    CPU_CRITICAL_EXIT();            /* 退出临界区 */                
                    
    /* 删除Start Task */
    OSTaskDel((OS_TCB *)0, &err); 
}

/* 任务1，ADC */
void ADC_task(void *p_arg)
{   
    OS_ERR err;
    uint16_t ADC_ConvertedValue;
    OS_SEM_CTR cnt = 0;

    while(1)
    {
        /* 读信号：  信号量句柄结构体， 最大等待时间(0 表示一直等) ，阻塞任务； 返回值：信号量计数值 */
        OSSemPend (&KEY1_binary_sem , 0 , OS_OPT_PEND_BLOCKING , 0 , &err);  /* 获取二值信号量 */
      
        OSSchedLock(&err);           /* 任务调度锁上锁 */
        printf("开始读取ADC值\r\n");
        ADC_ConvertedValue = Get_battery_volt();
        OSSchedUnlock(&err);         /* 任务调度锁解锁 */     
        
        /* 写队列：  队列句柄结构体， 向队列写入的值 ，写入值大小， 写入方式 */
        OSQPost (&ADC_queue, &ADC_ConvertedValue , sizeof(ADC_ConvertedValue) , OS_OPT_POST_FIFO , &err);
        OSSchedLock(&err);           /* 任务调度锁上锁 */
        printf("ADC队列写完毕\r\n");
        OSSchedUnlock(&err);         /* 任务调度锁解锁 */    
      
        /* 写事件标志帧：  事件标志组句柄结构体， 事件标志组哪一位写入， 对这一位的操作 */
        OSFlagPost (&flag , FLAG_BIT0 , OS_OPT_POST_FLAG_SET , &err);        /* 释放二值信号量 */
        OSSchedLock(&err);           /* 任务调度锁上锁 */ 
        printf("ADC事件标志帧写完毕\r\n");
        OSSchedUnlock(&err);         /* 任务调度锁解锁 */   
        LED0(1);   // 表示AD采集完成
      
        cnt = OSSemPend (&count_sem , 0 , OS_OPT_PEND_BLOCKING , 0 ,  &err);
        OSSchedLock(&err);           /* 任务调度锁上锁 */ 
        printf("执行完成ADC task，记时信号量为%d\r\n", cnt);
        OSSchedUnlock(&err);         /* 任务调度锁解锁 */   
                    
        OSTimeDly(10, OS_OPT_TIME_DLY, &err);
    }
}

/* 任务2，I2C */
void I2C_task(void *p_arg)
{
    OS_ERR err;
    float ALS;
    OS_SEM_CTR cnt = 0;

    while(1)
    {
        /* 读信号：  信号量句柄结构体， 最大等待时间(0 表示一直等) ，阻塞任务； 返回值：信号量计数值 */
        OSSemPend (&KEY2_binary_sem , 0 , OS_OPT_PEND_BLOCKING , 0 , &err);  /* 获取二值信号量 */
      
        OSSchedLock(&err);           /* 任务调度锁上锁 */
        printf("开始读取I2C值\r\n");
        ALS = ap3216c_read_ambient_light();
        OSSchedUnlock(&err);         /* 任务调度锁解锁 */       
      
        /* 写队列：  队列句柄结构体， 向队列写入的值 ，写入值大小， 写入方式 */
        OSQPost (&I2C_queue, &ALS , sizeof(ALS) , OS_OPT_POST_FIFO , &err);
        OSSchedLock(&err);           /* 任务调度锁上锁 */
        printf("I2C队列写完毕\r\n");
        OSSchedUnlock(&err);         /* 任务调度锁解锁 */   
      
        /* 写事件标志帧：  事件标志组句柄结构体， 事件标志组哪一位写入， 对这一位的操作 */
        OSFlagPost (&flag , FLAG_BIT1 , OS_OPT_POST_FLAG_SET , &err);        /* 释放二值信号量 */
        OSSchedLock(&err);           /* 任务调度锁上锁 */ 
        printf("I2C事件标志帧写完毕\r\n");
        OSSchedUnlock(&err);         /* 任务调度锁解锁 */   
        LED1(1);  // 表示I2C传输完成
      
        cnt = OSSemPend (&count_sem , 0 , OS_OPT_PEND_BLOCKING , 0 ,  &err);
        printf("执行I2C task，记时信号量为%d\r\n", cnt);
        OSTimeDly(10, OS_OPT_TIME_DLY, &err);

    }
}

/* 任务3，KEY1 */
void KEY1_task(void *p_arg)
{
    OS_ERR  err;
    uint8_t key = 0;
  
    while(1)
    {
        key = key_scan(0);
      
        if(key == KEY1_PRES)
        {
            /* 写信号量：  二值信号量句柄， 写入方式(只给一个写入) */
            OSSemPost (&KEY1_binary_sem , OS_OPT_POST_1 , &err);                 /* 释放二值信号量 */
            OSSchedLock(&err);           /* 任务调度锁上锁 */
            printf("KEY1按下，成功写入\r\n");
            OSSchedUnlock(&err);         /* 任务调度锁解锁 */       

        }
        else if(key == KEY2_PRES)
        {
            /* 写信号量：  二值信号量句柄， 写入方式(只给一个写入) */
            OSSemPost (&KEY2_binary_sem , OS_OPT_POST_1 , &err);                 /* 释放二值信号量 */
            OSSchedLock(&err);           /* 任务调度锁上锁 */
            printf("KEY2按下，成功写入\r\n");
            OSSchedUnlock(&err);         /* 任务调度锁解锁 */       
        }
        OSTimeDly(10, OS_OPT_TIME_DLY, &err);
    }
}

void SHOW1_task(void *p_arg)
{
    OS_ERR  err;
    uint16_t* ADC_ConvertedValue;
    float* ALS;
    OS_MSG_SIZE size = 0;
    
    while(1)
    {
        /* 读事件标志组 */
        OSFlagPend (&flag ,   /* 哪个事件标志组 */
                    FLAG_BIT0 | FLAG_BIT1 ,   /* 事件标志位 */
                    0 ,      /* 任务挂起等待时间 */
                    /* 函数选项： 等待“flags”中的所有指定位被置1，当等待到指定位后,清0对应位，标志组不满足条件时挂起任务 */
                    OS_OPT_PEND_FLAG_SET_ALL | OS_OPT_PEND_FLAG_CONSUME | OS_OPT_PEND_BLOCKING ,  
                    0,
                    &err);
        
        /* 队列句柄结构体，最大允许时间(0表示死等)，阻塞任务，消息大小，   返回值：指向消息指针 */
        ADC_ConvertedValue = OSQPend (&ADC_queue, 0 , OS_OPT_PEND_BLOCKING , &size , 0 , &err);      
        ALS = OSQPend (&I2C_queue, 0 , OS_OPT_PEND_BLOCKING , &size , 0 , &err);  
        
        OSSchedLock(&err);           /* 任务调度锁上锁 */
        printf("接收到数据，采集到的AD电压为%f, 光照强度为%f\r\n", (float)(*ADC_ConvertedValue)/4096*3.3, *ALS);
        OSSchedUnlock(&err);         /* 任务调度锁解锁 */    
        
        LED0(0);
        LED1(0);
      
        OSTimeDly(10, OS_OPT_TIME_DLY, &err);
    }

}

/* 注意！ 定时器任务！也有优先级！
   任务优先级 由 //#define  OS_CFG_TMR_TASK_PRIO   ((OS_PRIO)(OS_CFG_PRIO_MAX-3u)) 确定！
*/
void  timer_cb(OS_TMR *p_tmr, void *p_arg)
{
    OS_ERR  err;
    if(p_tmr == &timer)
    {
         /* 释放信号量，OS_OPT_POST_ALL 给所有等待该信号量的任务发信号 */
         OSSemPost (&count_sem , OS_OPT_POST_ALL , &err);
         if(err != OS_ERR_NONE)
            printf("执行定时器，错误码为%d", err);
         LED2_TOGGLE();
    }
}


/*
输出内容：
*****************************************
先按按键1，再按按键2
*****************************************
完成外设初始化，进入UCOSIII内核
开始任务创建完毕
队列、信号、定时器等创建完毕
任务创建完毕
定时器开始

开始读取ADC值
ADC队列写完毕
ADC事件标志帧写完毕
执行完成ADC task，记时信号量为70
KEY1按下，成功写入

开始读取I2C值
I2C队列写完毕
接收到数据，采集到的AD电压为2.123730, 光照强度为25.549999
I2C事件标志帧写完毕
执行I2C task，记时信号量为173
KEY2按下，成功写入(亮红灯)    

PS: 若先按KEY2在按KEY1则会亮绿灯
原因：SHOW1_task()点亮LED0/LED1后，在ADC_task/I2C_task立马关闭

*************此时亮LED0******************
*****************************************
连续按两次按键1
*****************************************
完成外设初始化，进入UCOSIII内核
开始任务创建完毕
队列、信号、定时器等创建完毕
任务创建完毕
定时器开始

开始读取ADC值
ADC队列写完毕
ADC事件标志帧写完毕
执行完成ADC task，记时信号量为25
KEY1按下，成功写入

开始读取ADC值
ADC队列写完毕
ADC事件标志帧写完毕
执行完成ADC task，记时信号量为71
KEY1按下，成功写入

*/
