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
#include "./SYSTEM/delay/delay.h"
/*uC/OS-III*********************************************************************************************/
#include "os.h"
#include "cpu.h"

/******************************************************************************************************/
/*uC/OS-III����*/

#define    Default_Tick             0         /* Ĭ��ʱ��Ƭ */

/* START_TASK ���� ����
 * ����: �������ȼ� ����ջ��С ����ʱ��Ƭ ������ƿ� ����ջ ������
 */
#define     START_TASK_PRIO         2         /* �������ȼ� */
#define     START_STK_SIZE          256       /* ����ջ��С(ʵ������ջ = START_STK_SIZE * 4(CPU_STK)����) */
#define     START_STK_TICK          0         /* ����ʱ��Ƭ */
OS_TCB      StartTask_TCB;                    /* ������ƿ� */
CPU_STK     StartTask_STK[START_STK_SIZE];    /* ����ջ */
void start_task(void *p_arg);                 /* ������ */
/* ע�⣺�����������ܹ�ʹ�� ��ֵΪ0��1��OS_CFG_PRIO_MAX-2 �� OS_CFG_PRIO_MAX-1 ���������ȼ� */


/* TASK1 ������Ҫʵ�ֱ�����������
 * ����: �������ȼ� ����ջ��С ����ʱ��Ƭ ������ƿ� ����ջ ������
 */
#define     TASK1_PRIO              5    
#define     TASK1_STK_SIZE          128  
#define     TASK1_STK_TICK          0
OS_TCB      Task1Task_TCB;                
CPU_STK     Task1Task_STK[TASK1_STK_SIZE];                
void task1(void *p_arg);               

/* TASK2 ������Ҫʵ�ֱ�����������
 * ����: �������ȼ� ����ջ��С ����ʱ��Ƭ ������ƿ� ����ջ ������
 */
#define     TASK2_PRIO              5      
#define     TASK2_STK_SIZE          128     
#define     TASK2_STK_TICK          50        
OS_TCB      Task2Task_TCB;                  
CPU_STK     Task2Task_STK[TASK2_STK_SIZE];                     
void task2(void *p_arg);                  

/* TASK3 ������Ҫʵ������ɾ��/����������/�ָ�����
 * ����: �������ȼ� ����ջ��С ����ʱ��Ƭ ������ƿ� ����ջ ������
 */
#define     TASK3_PRIO              8      
#define     TASK3_STK_SIZE          128     
#define     TASK3_STK_TICK          0      
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
    
    /* ��ʼ��uC/OS-III */
    OSInit(&err);
    
    /* ����Start Task */
    /* �����񱻴����ú������ھ���̬ */
    OSTaskCreate(   (OS_TCB        *)&StartTask_TCB,          /* ָ��������ƿ��ָ�룬����HAL�����ṹ�� */
                    (CPU_CHAR      *)"start_task",            /* ָ����������� */
                    (OS_TASK_PTR    )start_task,              /* ָ����������ָ��(����ִ�в���д����������) */	
                    (void          *)0,                       /* ���ݸ��������Ĳ���(û�в�������NULLָ��(0)) */	
                    (OS_PRIO        )START_TASK_PRIO,         /* ������������ȼ�������ԽС�����ȼ�Խ�� */
                    (CPU_STK       *)StartTask_STK,           /* ָ������ջ����ʼ��ַ��ָ�� (����ջ���� ��� 6.)*/	
                    (CPU_STK_SIZE   )START_STK_SIZE / 10,     /* ����ջ��ʹ�þ�����(���ﶨ����90%) */
                    (CPU_STK_SIZE   )START_STK_SIZE,          /* ����ջ��С */
                    (OS_MSG_QTY     )0,                       /* ������Ƕ��Ϣ���еĴ�С(��֪����ɶ�ã���Ϊ0����ʾĬ��ֵ) */
                    (OS_TICK        )START_STK_TICK,          /* �����ʱ��Ƭ(ʱ��Ƭ������Ҫ����ǰ����һ��ʱ��Ƭ��ʾ1ms��Ĭ��ֵ0��ʾ100��ʱ��Ƭ) */
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
    
    /* �������õĽ���Ƶ������SysTick */
    cnts = (CPU_INT32U)(HAL_RCC_GetSysClockFreq() / OSCfg_TickRate_Hz);
    OS_CPU_SysTickInit(cnts);
    
    /* ����ʱ��Ƭ���� */
    OSSchedRoundRobinCfg(OS_TRUE, Default_Tick, &err);
    
    CPU_SR_ALLOC();    /* �����ٽ���֮ǰ�����ȵ��ô˺��� */
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
                    (OS_TICK        )TASK1_STK_TICK,                
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
                    (OS_TICK        )TASK2_STK_TICK,
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
                    (OS_TICK     )  TASK3_STK_TICK,
                    (void*       )  0,
                    (OS_OPT      )  (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                    (OS_ERR*     )  &err);
                                       
    CPU_CRITICAL_EXIT();            /* �˳��ٽ��� */                
                    
    /* ɾ��Start Task */
    OSTaskDel((OS_TCB *)0, &err); 
}

/* ����1���ƴ� */
void task1(void *p_arg)
{   
    OS_ERR err;
    uint16_t task1_num = 0;
    
  
    while(1)
    {
        OSSchedLock(&err);           /* ������������� */
        printf("task1���д�����%d\r\n",++task1_num);
        OSSchedUnlock(&err);         /* ������������� */       
        OSTimeDly(250, OS_OPT_TIME_DLY, &err);
    }
}

/* ����2���ƴ� */
void task2(void *p_arg)
{
    OS_ERR err;
    uint16_t task2_num = 0;
  
    while(1)
    {
        OSSchedLock(&err);           /* ������������� */
        printf("task2���д�����%d\r\n",++task2_num);
        OSSchedUnlock(&err);         /* ������������� */   
        OSTimeDly(250, OS_OPT_TIME_DLY, &err);
    }
}

/* ����3�������жϣ�����KEY0ɾ��task4���� */
void task3(void *p_arg) 
{
    OS_ERR err;  
    CPU_STK_SIZE free,used; 
  
    while(1) 
    {   
        OSSchedLock(&err);           /* ������������� */
        OSTaskStkChk (&Task3Task_TCB, &free, &used, &err);   //  ��ͳ��������Ķ�ջʹ����Ҳ��ӡ����  
                                                             // Ȼ���ʵ����������������256�ֽڸ����ǲ�����ĺ���  
        printf("SystemDatasBroadcast  used/free:%d/%d  usage:%d%%\r\n",used,free,(used*100)/(used+free));  
     
        OSTaskStkChk (&Task2Task_TCB, &free, &used, &err);  
        printf("Core_Page             used/free:%d/%d  usage:%d%%\r\n",used,free,(used*100)/(used+free));  
     
        OSTaskStkChk (&Task1Task_TCB, &free, &used, &err);  
        printf("GUIActive             used/free:%d/%d  usage:%d%%\r\n",used,free,(used*100)/(used+free));  
     
        printf("\r\n\r\n\r\n");
        
        OSSchedUnlock(&err);         /* ������������� */   
        OSTimeDly(1000, OS_OPT_TIME_DLY, &err);
    }
}  
