/**
 ****************************************************************************************************
 * @file        uc-os3_demo.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-04-13
 * @brief       uC/OS-III ��ֲʵ��
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� ̽���� F407������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
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
/*uC/OS-III����*/

/* START_TASK ���� ����
 * ����: �������ȼ� ����ջ��С ������ƿ� ����ջ ������
 */
#define     START_TASK_PRIO         2         /* �������ȼ� */
#define     START_STK_SIZE          256       /* ����ջ��С */
OS_TCB      StartTask_TCB;                    /* ������ƿ� */
CPU_STK     StartTask_STK[START_STK_SIZE];    /* ����ջ */
void start_task(void *p_arg);                 /* ������ */

/* TASK1 ���� ����
 * ����: �������ȼ� ����ջ��С ������ƿ� ����ջ ������
 */
#define     TASK1_PRIO              5         /* �������ȼ� */
#define     TASK1_STK_SIZE          128       /* ����ջ��С */
OS_TCB      Task1Task_TCB;                    /* ������ƿ� */
CPU_STK     Task1Task_STK[TASK1_STK_SIZE];    /* ����ջ */
void task1(void *p_arg);                      /* ������ */

/* TASK2 ���� ����
 * ����: �������ȼ� ����ջ��С ������ƿ� ����ջ ������
 */
#define     TASK2_PRIO              5         /* �������ȼ� */
#define     TASK2_STK_SIZE          128       /* ����ջ��С */
OS_TCB      Task2Task_TCB;                    /* ������ƿ� */
CPU_STK     Task2Task_STK[TASK2_STK_SIZE];    /* ����ջ */
void task2(void *p_arg);                      /* ������ */

/* TASK3 ���� ����
 * ����: �������ȼ� ����ջ��С ������ƿ� ����ջ ������
 */
#define     TASK3_PRIO              4
#define     TASK3_STK_SIZE          128
OS_TCB      Task3Task_TCB;    
CPU_STK     Task3Task_STK[TASK3_STK_SIZE];            
void task3(void *p_arg);

/******************************************************************************************************/

/**
 * @brief       uC/OS-III������ں���
 * @param       ��
 * @retval      ��
 */
void uc_os3_demo(void)
{
    OS_ERR err;
    
    /* �ر������ж� */
    CPU_IntDis();
    
    /* ��ʼ��uC/OS-III */
    OSInit(&err);
    
    /* ����Start Task */
    OSTaskCreate(   (OS_TCB        *)&StartTask_TCB,          /* ָ��������ƿ��ָ�룬����HAL�����ṹ�� */
                    (CPU_CHAR      *)"start_task",            /* ָ����������� */
                    (OS_TASK_PTR    )start_task,              /* ָ����������ָ��(����ִ�в���д����������) */	
                    (void          *)0,                       /* ���ݸ��������Ĳ���(û�в�������NULLָ��(0)) */	
                    (OS_PRIO        )START_TASK_PRIO,         /* ������������ȼ�������ԽС�����ȼ�Խ�� */
                    (CPU_STK       *)StartTask_STK,           /* ָ������ջ����ʼ��ַ��ָ�� (����ջ���� ��� 6.)*/	
                    (CPU_STK_SIZE   )START_STK_SIZE / 10,     /* ����ջ��ʹ�þ�����(���ﶨ����90%) */
                    (CPU_STK_SIZE   )START_STK_SIZE,          /* ����ջ��С */
                    (OS_MSG_QTY     )0,                       /* ������Ƕ��Ϣ���еĴ�С(��֪����ɶ�ã���Ϊ0����ʾĬ��ֵ) */
                    (OS_TICK        )0,                       /* �����ʱ��Ƭ(ʱ��Ƭ������Ҫ����ǰ����һ��ʱ��Ƭ��ʾ1ms��Ĭ��ֵ0��ʾ100��ʱ��Ƭ) */
                    (void          *)0,                       /* ָ���û���չ�ڴ��ָ��(��֪����ɶ�ã���Ϊ0����ʾĬ��ֵ) */
                    (OS_OPT         )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),   /* ����ѡ���5����STK_CHK ��ʾ �����������ж�ջ��飬STK_CLR ��ʾ ��Ҫ��������ջ */
                    (OS_ERR        *)&err);                   /* ָ����մ���������ָ��(�൱�ڴ����룬0��ʾû�д���) */
    
    /* ��ʼ������� */
    OSStart(&err);
}

/**
 * @brief       start_task
 * @param       p_arg : �������(δ�õ�)
 * @retval      ��
 */
void start_task(void *p_arg)
{
    OS_ERR err;
    CPU_INT32U cnts;
    
    /* ��ʼ��CPU�� */
    CPU_Init();
    /* �����ٽ���֮ǰ�����ȵ��ô˺��� */
    CPU_SR_ALLOC();
    
    /* �������õĽ���Ƶ������SysTick */
    /* HAL_RCC_GetSysClockFreq()=72 000 000(72MHz)     OSCfg_TickRate_Hz = 1000(ʱ��Ƭ1ms) */
    cnts = (CPU_INT32U)(HAL_RCC_GetSysClockFreq() / OSCfg_TickRate_Hz);
    OS_CPU_SysTickInit(cnts);
    
    /* ����ʱ��Ƭ���ȣ�ʱ��Ƭ��ΪĬ��ֵ */
    OSSchedRoundRobinCfg(OS_TRUE, 0, &err);
    
    /* �ٽ�δ���Ҳ�����ٽ�������ָ��Щ�����������У����ܱ���ϵĴ���� */
    CPU_CRITICAL_ENTER();           /* �����ٽ��� */
    
    /* ����Task1 */
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
    
    /* ����Task2 */     
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
                    
    
                    
    /* ����task3 */
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
                                
    CPU_CRITICAL_EXIT();            /* �˳��ٽ��� */                
                    
    /* ɾ��Start Task */
    OSTaskDel((OS_TCB *)0, &err); 
}

/* ����1 */
void task1(void *p_arg)
{
    OS_ERR err;
    
    while(1)
    {
        
    } 
}

/* ����2 */
void task2(void *p_arg)
{
    OS_ERR err;
    
    while(1)
    {
    }
}

/* ����3 */
void task3(void *p_arg) 
{
    OS_ERR  err;
  
    while(1) 
    {   
    }
}  

