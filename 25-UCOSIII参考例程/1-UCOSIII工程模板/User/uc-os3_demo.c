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
/*uC/OS-III*********************************************************************************************/
#include "os.h"
#include "cpu.h"

/******************************************************************************************************/
/*uC/OS-III配置*/

/* START_TASK 任务 配置
 * 包括: 任务优先级 任务栈大小 任务控制块 任务栈 任务函数
 */
#define     START_TASK_PRIO         2         /* 任务优先级 */
#define     START_STK_SIZE          256       /* 任务栈大小 */
OS_TCB      StartTask_TCB;                    /* 任务控制块 */
CPU_STK     StartTask_STK[START_STK_SIZE];    /* 任务栈 */
void start_task(void *p_arg);                 /* 任务函数 */

/* TASK1 任务 配置
 * 包括: 任务优先级 任务栈大小 任务控制块 任务栈 任务函数
 */
#define     TASK1_PRIO              5         /* 任务优先级 */
#define     TASK1_STK_SIZE          128       /* 任务栈大小 */
OS_TCB      Task1Task_TCB;                    /* 任务控制块 */
CPU_STK     Task1Task_STK[TASK1_STK_SIZE];    /* 任务栈 */
void task1(void *p_arg);                      /* 任务函数 */

/* TASK2 任务 配置
 * 包括: 任务优先级 任务栈大小 任务控制块 任务栈 任务函数
 */
#define     TASK2_PRIO              5         /* 任务优先级 */
#define     TASK2_STK_SIZE          128       /* 任务栈大小 */
OS_TCB      Task2Task_TCB;                    /* 任务控制块 */
CPU_STK     Task2Task_STK[TASK2_STK_SIZE];    /* 任务栈 */
void task2(void *p_arg);                      /* 任务函数 */

/* TASK3 任务 配置
 * 包括: 任务优先级 任务栈大小 任务控制块 任务栈 任务函数
 */
#define     TASK3_PRIO              4
#define     TASK3_STK_SIZE          128
OS_TCB      Task3Task_TCB;    
CPU_STK     Task3Task_STK[TASK3_STK_SIZE];            
void task3(void *p_arg);

/******************************************************************************************************/

/**
 * @brief       uC/OS-III例程入口函数
 * @param       无
 * @retval      无
 */
void uc_os3_demo(void)
{
    OS_ERR err;
    
    /* 关闭所有中断 */
    CPU_IntDis();
    
    /* 初始化uC/OS-III */
    OSInit(&err);
    
    /* 创建Start Task */
    OSTaskCreate(   (OS_TCB        *)&StartTask_TCB,          /* 指向任务控制块的指针，类似HAL库句柄结构体 */
                    (CPU_CHAR      *)"start_task",            /* 指向任务的名字 */
                    (OS_TASK_PTR    )start_task,              /* 指向任务函数的指针(任务执行操作写在任务函数内) */	
                    (void          *)0,                       /* 传递给任务函数的参数(没有参数就是NULL指针(0)) */	
                    (OS_PRIO        )START_TASK_PRIO,         /* 任务的任务优先级，数字越小，优先级越高 */
                    (CPU_STK       *)StartTask_STK,           /* 指向任务栈的起始地址的指针 (任务栈解释 详见 6.)*/	
                    (CPU_STK_SIZE   )START_STK_SIZE / 10,     /* 任务栈的使用警戒线(这里定义是90%) */
                    (CPU_STK_SIZE   )START_STK_SIZE,          /* 任务栈大小 */
                    (OS_MSG_QTY     )0,                       /* 任务内嵌消息队列的大小(不知道有啥用，设为0，表示默认值) */
                    (OS_TICK        )0,                       /* 任务的时间片(时间片调度需要，当前设置一个时间片表示1ms，默认值0表示100个时间片) */
                    (void          *)0,                       /* 指向用户扩展内存的指针(不知道有啥用，设为0，表示默认值) */
                    (OS_OPT         )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),   /* 任务选项共有5个，STK_CHK 表示 允许对任务进行堆栈检查，STK_CLR 表示 需要清除任务堆栈 */
                    (OS_ERR        *)&err);                   /* 指向接收错误代码变量指针(相当于错误码，0表示没有错误) */
    
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
    /* 进入临界区之前必须先调用此函数 */
    CPU_SR_ALLOC();
    
    /* 根据配置的节拍频率配置SysTick */
    /* HAL_RCC_GetSysClockFreq()=72 000 000(72MHz)     OSCfg_TickRate_Hz = 1000(时间片1ms) */
    cnts = (CPU_INT32U)(HAL_RCC_GetSysClockFreq() / OSCfg_TickRate_Hz);
    OS_CPU_SysTickInit(cnts);
    
    /* 开启时间片调度，时间片设为默认值 */
    OSSchedRoundRobinCfg(OS_TRUE, 0, &err);
    
    /* 临界段代码也叫做临界区，是指那些必须完整运行，不能被打断的代码段 */
    CPU_CRITICAL_ENTER();           /* 进入临界区 */
    
    /* 创建Task1 */
    OSTaskCreate(   (OS_TCB        *)&Task1Task_TCB,       
                    (CPU_CHAR      *)"task1",              
                    (OS_TASK_PTR    )task1,                
                    (void          *)0,                    
                    (OS_PRIO        )TASK1_PRIO,           
                    (CPU_STK       *)Task1Task_STK,        
                    (CPU_STK_SIZE   )TASK1_STK_SIZE / 10,  
                    (CPU_STK_SIZE   )TASK1_STK_SIZE,       
                    (OS_MSG_QTY     )0,                    
                    (OS_TICK        )0,                    
                    (void          *)0,                    
                    (OS_OPT         )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), 
                    (OS_ERR        *)&err);           
    
    /* 创建Task2 */     
    OSTaskCreate(   (OS_TCB        *)&Task2Task_TCB,
                    (CPU_CHAR      *)"task2",
                    (OS_TASK_PTR    )task2,
                    (void          *)0,
                    (OS_PRIO        )TASK2_PRIO,
                    (CPU_STK       *)Task2Task_STK,
                    (CPU_STK_SIZE   )TASK2_STK_SIZE / 10,
                    (CPU_STK_SIZE   )TASK2_STK_SIZE,
                    (OS_MSG_QTY     )0,
                    (OS_TICK        )0,
                    (void          *)0,
                    (OS_OPT         )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                    (OS_ERR        *)&err);
                    
    
                    
    /* 创建task3 */
    OSTaskCreate(   (OS_TCB*     )  &Task3Task_TCB,
                    (CPU_CHAR*   )  "task3",
                    (OS_TASK_PTR )  task3,
                    (void*       )  0,
                    (OS_PRIO     )  TASK3_PRIO,
                    (CPU_STK*    )  Task3Task_STK,
                    (CPU_STK_SIZE)  TASK3_STK_SIZE / 10,
                    (CPU_STK_SIZE)  TASK3_STK_SIZE,
                    (OS_MSG_QTY  )  0,
                    (OS_TICK     )  0,
                    (void*       )  0,
                    (OS_OPT      )  (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                    (OS_ERR*     )  &err);
                                
    CPU_CRITICAL_EXIT();            /* 退出临界区 */                
                    
    /* 删除Start Task */
    OSTaskDel((OS_TCB *)0, &err); 
}

/* 任务1 */
void task1(void *p_arg)
{
    OS_ERR err;
    
    while(1)
    {
        
    } 
}

/* 任务2 */
void task2(void *p_arg)
{
    OS_ERR err;
    
    while(1)
    {
    }
}

/* 任务3 */
void task3(void *p_arg) 
{
    OS_ERR  err;
  
    while(1) 
    {   
    }
}  

