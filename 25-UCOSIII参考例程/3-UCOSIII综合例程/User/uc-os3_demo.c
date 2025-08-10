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
#include "./BSP/ADC/adc.h"
#include "./BSP/AP3216C/ap3216c.h"
#include "./BSP/I2C/i2c.h"
#include "./SYSTEM/delay/delay.h"
/*uC/OS-III*********************************************************************************************/
#include "os.h"
#include "cpu.h"

/******************************************************************************************************/
/*uC/OS-III����*/

#define     Default_Tick            25       /* Ĭ��ʱ��Ƭ */

/* ��ʼ���� */
#define     START_TASK_PRIO         2         /* �������ȼ� */
#define     START_STK_SIZE          512       /* ����ջ��С(ʵ������ջ = START_STK_SIZE * 4(CPU_STK)����) */
#define     START_STK_TICK          0         /* ����ʱ��Ƭ */
OS_TCB      StartTask_TCB;                    /* ������ƿ� */
CPU_STK     StartTask_STK[START_STK_SIZE];    /* ����ջ */
void start_task(void *p_arg);                 /* ������ */
/* ע�⣺�����������ܹ�ʹ�� ��ֵΪ0��1��OS_CFG_PRIO_MAX-2 �� OS_CFG_PRIO_MAX-1 ���������ȼ� */


/* AD�ɼ����� */
#define     ADC_PRIO                4    
#define     ADC_STK_SIZE            256  
#define     ADC_STK_TICK            0
OS_TCB      ADCTask_TCB;                
CPU_STK     ADCTask_STK[ADC_STK_SIZE];                
void ADC_task(void *p_arg);               

/* I2C��ȡ�������� */
#define     I2C_PRIO                4      
#define     I2C_STK_SIZE            256     
#define     I2C_STK_TICK            0        
OS_TCB      I2CTask_TCB;                  
CPU_STK     I2CTask_STK[I2C_STK_SIZE];                     
void I2C_task(void *p_arg);                  

/* ������������ */
#define     KEY1_PRIO                5      
#define     KEY1_STK_SIZE            256 
#define     KEY1_STK_TICK            0      
OS_TCB      KEY1Task_TCB;                
CPU_STK     KEY1Task_STK[KEY1_STK_SIZE];                
void KEY1_task(void *p_arg);      

/* �����������1 */
#define     SHOW1_PRIO               3
#define     SHOW1_STK_SIZE           256   
#define     SHOW1_STK_TICK           0      
OS_TCB      SHOW1Task_TCB;                
CPU_STK     SHOW1Task_STK[SHOW1_STK_SIZE];                
void SHOW1_task(void *p_arg);       

/******************************************************************************************************/
/* �����ʱ������ǰ���� 200 ms ����һ��*/
OS_TMR   timer;                                    /* �����ʱ�� */
void     timer_cb(OS_TMR *p_tmr, void *p_arg);     /* �����ʱ���Ļص����� */
/* 
OS_CFG_TMR_TASK_RATE_HZ������ʾ��ʱ���ڣ�
#define  OS_CFG_TMR_TASK_RATE_HZ                          100u
��ʾ������ʱ���� 1/100 = 10ms

ʵ�ʶ�ʱ������ = timer_period * (1/OS_CFG_TMR_TASK_RATE_HZ)
*/
#define  timer_period    20                        

OS_Q    ADC_queue;                              /* ���ݶ��� */
OS_Q    I2C_queue;                              /* ���ݶ��� */

/* ��ֵ�ź��� */
OS_SEM KEY1_binary_sem;
OS_SEM KEY2_binary_sem;
/* �����ź�����ͳ�ư������´��� */
OS_SEM count_sem;

/* �����־�� */
OS_FLAG_GRP flag;
#define FLAG_BIT0    (1 << 0)
#define FLAG_BIT1    (1 << 1)
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
    
    printf("��ʼ���񴴽����\r\n");                
                    
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
    
    /* �������� */
    /* ���о���ṹ��  �� ��Ϣ���еĴ�С */
    OSQCreate (&ADC_queue , "ADC_queue" , 1 , &err);
    OSQCreate (&I2C_queue , "I2C_queue" , 1 , &err);
  
    /* ����һ���¼���־�� */
    /* �¼���־�����ṹ��  �� �¼���־��ĳ�ʼֵ */
    OSFlagCreate (&flag , "flag" , 0 , &err);
    
    /* �����ź��� */
    /* �ź�������ṹ��  ��  �ź�����Դ���ĳ�ʼֵ */
    OSSemCreate (&KEY1_binary_sem , "KEY1_binary_sem" , 0 , &err);
    OSSemCreate (&KEY2_binary_sem , "KEY2_binary_sem" , 0 , &err);  
    OSSemCreate (&count_sem , "count_sem" , 0 , &err);  
  
    /* ���������ʱ�� */
    OSTmrCreate (   &timer,    // �����ʱ������ṹ��
                    "timer", 
                    0,             // �����ʱ���Ŀ�����ʱʱ�䣬ע�ⵥ�ζ�ʱ���������0
                    timer_period,  // ���ڶ�ʱ���Ķ�ʱ����ʱ�䣬ע�����ڶ�ʱ���������0 
                    OS_OPT_TMR_PERIODIC,             // OS_OPT_TMR_ONE_SHOT�����ζ�ʱ���� OS_OPT_TMR_PERIODIC�����ڶ�ʱ����
                    (OS_TMR_CALLBACK_PTR)timer_cb,   // ָ��ʱ�ص�������ָ��
                    0,         // ��ʱ�ص�������ڲ���
                    &err    );
  
    printf("���С��źš���ʱ���ȴ������\r\n");      
  
    /* ����ʱ��Ƭ���� */
    OSSchedRoundRobinCfg(OS_TRUE, Default_Tick, &err);
    
    CPU_SR_ALLOC();    /* �����ٽ���֮ǰ�����ȵ��ô˺��� */
    CPU_CRITICAL_ENTER();           /* �����ٽ��� */
    
    /* ����ADCTask */
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
    
    /* ����I2CTask */    
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
                        
    /* ����KEY1Task */
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
                                     
    /* ����SHOW1Task */
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
            
                                       
    printf("���񴴽����\r\n");     
  
    // ��ʼ�����ʱ��
    OSTmrStart (&timer , &err);
    if(err != OS_ERR_NONE)
        printf("ִ�ж�ʱ����������Ϊ%d", err);
    
    printf("��ʱ����ʼ\r\n");           
                    
    CPU_CRITICAL_EXIT();            /* �˳��ٽ��� */                
                    
    /* ɾ��Start Task */
    OSTaskDel((OS_TCB *)0, &err); 
}

/* ����1��ADC */
void ADC_task(void *p_arg)
{   
    OS_ERR err;
    uint16_t ADC_ConvertedValue;
    OS_SEM_CTR cnt = 0;

    while(1)
    {
        /* ���źţ�  �ź�������ṹ�壬 ���ȴ�ʱ��(0 ��ʾһֱ��) ���������� ����ֵ���ź�������ֵ */
        OSSemPend (&KEY1_binary_sem , 0 , OS_OPT_PEND_BLOCKING , 0 , &err);  /* ��ȡ��ֵ�ź��� */
      
        OSSchedLock(&err);           /* ������������� */
        printf("��ʼ��ȡADCֵ\r\n");
        ADC_ConvertedValue = Get_battery_volt();
        OSSchedUnlock(&err);         /* ������������� */     
        
        /* д���У�  ���о���ṹ�壬 �����д���ֵ ��д��ֵ��С�� д�뷽ʽ */
        OSQPost (&ADC_queue, &ADC_ConvertedValue , sizeof(ADC_ConvertedValue) , OS_OPT_POST_FIFO , &err);
        OSSchedLock(&err);           /* ������������� */
        printf("ADC����д���\r\n");
        OSSchedUnlock(&err);         /* ������������� */    
      
        /* д�¼���־֡��  �¼���־�����ṹ�壬 �¼���־����һλд�룬 ����һλ�Ĳ��� */
        OSFlagPost (&flag , FLAG_BIT0 , OS_OPT_POST_FLAG_SET , &err);        /* �ͷŶ�ֵ�ź��� */
        OSSchedLock(&err);           /* ������������� */ 
        printf("ADC�¼���־֡д���\r\n");
        OSSchedUnlock(&err);         /* ������������� */   
        LED0(1);   // ��ʾAD�ɼ����
      
        cnt = OSSemPend (&count_sem , 0 , OS_OPT_PEND_BLOCKING , 0 ,  &err);
        OSSchedLock(&err);           /* ������������� */ 
        printf("ִ�����ADC task����ʱ�ź���Ϊ%d\r\n", cnt);
        OSSchedUnlock(&err);         /* ������������� */   
                    
        OSTimeDly(10, OS_OPT_TIME_DLY, &err);
    }
}

/* ����2��I2C */
void I2C_task(void *p_arg)
{
    OS_ERR err;
    float ALS;
    OS_SEM_CTR cnt = 0;

    while(1)
    {
        /* ���źţ�  �ź�������ṹ�壬 ���ȴ�ʱ��(0 ��ʾһֱ��) ���������� ����ֵ���ź�������ֵ */
        OSSemPend (&KEY2_binary_sem , 0 , OS_OPT_PEND_BLOCKING , 0 , &err);  /* ��ȡ��ֵ�ź��� */
      
        OSSchedLock(&err);           /* ������������� */
        printf("��ʼ��ȡI2Cֵ\r\n");
        ALS = ap3216c_read_ambient_light();
        OSSchedUnlock(&err);         /* ������������� */       
      
        /* д���У�  ���о���ṹ�壬 �����д���ֵ ��д��ֵ��С�� д�뷽ʽ */
        OSQPost (&I2C_queue, &ALS , sizeof(ALS) , OS_OPT_POST_FIFO , &err);
        OSSchedLock(&err);           /* ������������� */
        printf("I2C����д���\r\n");
        OSSchedUnlock(&err);         /* ������������� */   
      
        /* д�¼���־֡��  �¼���־�����ṹ�壬 �¼���־����һλд�룬 ����һλ�Ĳ��� */
        OSFlagPost (&flag , FLAG_BIT1 , OS_OPT_POST_FLAG_SET , &err);        /* �ͷŶ�ֵ�ź��� */
        OSSchedLock(&err);           /* ������������� */ 
        printf("I2C�¼���־֡д���\r\n");
        OSSchedUnlock(&err);         /* ������������� */   
        LED1(1);  // ��ʾI2C�������
      
        cnt = OSSemPend (&count_sem , 0 , OS_OPT_PEND_BLOCKING , 0 ,  &err);
        printf("ִ��I2C task����ʱ�ź���Ϊ%d\r\n", cnt);
        OSTimeDly(10, OS_OPT_TIME_DLY, &err);

    }
}

/* ����3��KEY1 */
void KEY1_task(void *p_arg)
{
    OS_ERR  err;
    uint8_t key = 0;
  
    while(1)
    {
        key = key_scan(0);
      
        if(key == KEY1_PRES)
        {
            /* д�ź�����  ��ֵ�ź�������� д�뷽ʽ(ֻ��һ��д��) */
            OSSemPost (&KEY1_binary_sem , OS_OPT_POST_1 , &err);                 /* �ͷŶ�ֵ�ź��� */
            OSSchedLock(&err);           /* ������������� */
            printf("KEY1���£��ɹ�д��\r\n");
            OSSchedUnlock(&err);         /* ������������� */       

        }
        else if(key == KEY2_PRES)
        {
            /* д�ź�����  ��ֵ�ź�������� д�뷽ʽ(ֻ��һ��д��) */
            OSSemPost (&KEY2_binary_sem , OS_OPT_POST_1 , &err);                 /* �ͷŶ�ֵ�ź��� */
            OSSchedLock(&err);           /* ������������� */
            printf("KEY2���£��ɹ�д��\r\n");
            OSSchedUnlock(&err);         /* ������������� */       
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
        /* ���¼���־�� */
        OSFlagPend (&flag ,   /* �ĸ��¼���־�� */
                    FLAG_BIT0 | FLAG_BIT1 ,   /* �¼���־λ */
                    0 ,      /* �������ȴ�ʱ�� */
                    /* ����ѡ� �ȴ���flags���е�����ָ��λ����1�����ȴ���ָ��λ��,��0��Ӧλ����־�鲻��������ʱ�������� */
                    OS_OPT_PEND_FLAG_SET_ALL | OS_OPT_PEND_FLAG_CONSUME | OS_OPT_PEND_BLOCKING ,  
                    0,
                    &err);
        
        /* ���о���ṹ�壬�������ʱ��(0��ʾ����)������������Ϣ��С��   ����ֵ��ָ����Ϣָ�� */
        ADC_ConvertedValue = OSQPend (&ADC_queue, 0 , OS_OPT_PEND_BLOCKING , &size , 0 , &err);      
        ALS = OSQPend (&I2C_queue, 0 , OS_OPT_PEND_BLOCKING , &size , 0 , &err);  
        
        OSSchedLock(&err);           /* ������������� */
        printf("���յ����ݣ��ɼ�����AD��ѹΪ%f, ����ǿ��Ϊ%f\r\n", (float)(*ADC_ConvertedValue)/4096*3.3, *ALS);
        OSSchedUnlock(&err);         /* ������������� */    
        
        LED0(0);
        LED1(0);
      
        OSTimeDly(10, OS_OPT_TIME_DLY, &err);
    }

}

/* ע�⣡ ��ʱ������Ҳ�����ȼ���
   �������ȼ� �� //#define  OS_CFG_TMR_TASK_PRIO   ((OS_PRIO)(OS_CFG_PRIO_MAX-3u)) ȷ����
*/
void  timer_cb(OS_TMR *p_tmr, void *p_arg)
{
    OS_ERR  err;
    if(p_tmr == &timer)
    {
         /* �ͷ��ź�����OS_OPT_POST_ALL �����еȴ����ź����������ź� */
         OSSemPost (&count_sem , OS_OPT_POST_ALL , &err);
         if(err != OS_ERR_NONE)
            printf("ִ�ж�ʱ����������Ϊ%d", err);
         LED2_TOGGLE();
    }
}


/*
������ݣ�
*****************************************
�Ȱ�����1���ٰ�����2
*****************************************
��������ʼ��������UCOSIII�ں�
��ʼ���񴴽����
���С��źš���ʱ���ȴ������
���񴴽����
��ʱ����ʼ

��ʼ��ȡADCֵ
ADC����д���
ADC�¼���־֡д���
ִ�����ADC task����ʱ�ź���Ϊ70
KEY1���£��ɹ�д��

��ʼ��ȡI2Cֵ
I2C����д���
���յ����ݣ��ɼ�����AD��ѹΪ2.123730, ����ǿ��Ϊ25.549999
I2C�¼���־֡д���
ִ��I2C task����ʱ�ź���Ϊ173
KEY2���£��ɹ�д��(�����)    

PS: ���Ȱ�KEY2�ڰ�KEY1������̵�
ԭ��SHOW1_task()����LED0/LED1����ADC_task/I2C_task����ر�

*************��ʱ��LED0******************
*****************************************
���������ΰ���1
*****************************************
��������ʼ��������UCOSIII�ں�
��ʼ���񴴽����
���С��źš���ʱ���ȴ������
���񴴽����
��ʱ����ʼ

��ʼ��ȡADCֵ
ADC����д���
ADC�¼���־֡д���
ִ�����ADC task����ʱ�ź���Ϊ25
KEY1���£��ɹ�д��

��ʼ��ȡADCֵ
ADC����д���
ADC�¼���־֡д���
ִ�����ADC task����ʱ�ź���Ϊ71
KEY1���£��ɹ�д��

*/
