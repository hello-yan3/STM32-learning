/**
 ****************************************************************************************************
 * @file        freertos.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.4
 * @date        2022-01-04
 * @brief       FreeRTOS ��ֲʵ��
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� ս��F103������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include <stdlib.h>
#include <string.h>
#include "freertos_demo.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/LED/led.h"
#include "./BSP/KEY/key.h"
/*FreeRTOS*********************************************************************************************/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
/******************************************************************************************************/
/*FreeRTOS����*/

/* START_TASK ���� ����
 * ����: ������ �������ȼ� ��ջ��С ������
 */
#define START_TASK_PRIO 1                   /* �������ȼ� */
#define START_STK_SIZE  128                 /* �����ջ��С */
TaskHandle_t            StartTask_Handler;  /* ������ */
void start_task(void *pvParameters);        /* ������ */

/* TASK1 ���� ����
 * ����: ������ �������ȼ� ��ջ��С ������
 */
#define TASK1_PRIO      12                  /* �������ȼ� */
#define TASK1_STK_SIZE  128                 /* �����ջ��С */
TaskHandle_t            Task1Task_Handler;  /* ������ */
void task1(void *pvParameters);             /* ������ */

/* TASK2 ���� ����
 * ����: ������ �������ȼ� ��ջ��С ������
 */
#define TASK2_PRIO      8                   /* �������ȼ� */
#define TASK2_STK_SIZE  128                 /* �����ջ��С */
TaskHandle_t            Task2Task_Handler;  /* ������ */
void task2(void *pvParameters);             /* ������ */

/* TASK3 ���� ����
 * ����: ������ �������ȼ� ��ջ��С ��������
 */
#define TASK3_PRIO      6                   /* �������ȼ� */
#define TASK3_STK_SIZE  128                 /* �����ջ��С */
TaskHandle_t            Task3Task_Handler;  /* ������ */
void task3(void *pvParameters);             /* ������ */

List_t                  TestList;           /* ��������б� */
ListItem_t              ListItem1;          /* ��������б���1 */
ListItem_t              ListItem2;          /* ��������б���2 */
ListItem_t              ListItem3;          /* ��������б���3 */
/******************************************************************************************************/
#define is_output_the_list    0             // �Ƿ�����б�/״̬��Ϣ

/******************************************************************************************************/
QueueHandle_t  key_queue;        /* С���ݾ�� */
QueueHandle_t  big_date_queue;   /* �����ݾ�� */
char buff[80] = {"����һ�������飬�������飡"};
 
QueueHandle_t  semphore_handle;          /* ��ֵ�ź��� */
QueueHandle_t  count_semphore_handle;    /* �����ź��� */
 
EventGroupHandle_t   EventGroupHandler;       /* �¼���־���� */
#define EVENTBIT_0      (1 << 0)              /* �¼�λ */
#define EVENTBIT_1      (1 << 1)
#define EVENTBIT_ALL    (EVENTBIT_0 | EVENTBIT_1)
/******************************************************************************************************/
// ������ xQueue1/xSemaphore���һ�����м�
QueueHandle_t               xQueue1;            /* �������1 */
SemaphoreHandle_t           xSemaphore;         /* �����ֵ�ź��� */
static QueueSetHandle_t     xQueueSet;          /* ������м� */
#define QUEUESET_LENGTH     2                   /* ���м�֧�ֵ���Ϣ���� */

/**
 * @brief       FreeRTOS������ں���
 * @param       ��
 * @retval      ��
 */
void freertos_demo(void)
{
    /* ���еĴ��� */
    // xQueueCreate (  uxQueueLength,   uxItemSize  )   
    // ���г��ȣ�������Ŀ�Ĵ�С
    // ���д���ʧ�ܣ�����ֵNULL�����д����ɹ������ض��о��
    /* 
    #define    xQueueCreate         ( uxQueueLength,   uxItemSize  )   \
               xQueueGenericCreate( ( uxQueueLength ), ( uxItemSize ), (queueQUEUE_TYPE_BASE )) 
    #define queueQUEUE_TYPE_BASE              ( ( uint8_t ) 0U )   // ����  ������queueQUEUE_TYPE_BASE��ʾ����
    */
    key_queue = xQueueCreate( 2, sizeof(uint8_t) );
    if(key_queue != NULL)           printf("key_queue���д����ɹ�����\r\n");
    else                            printf("key_queue���д���ʧ�ܣ���\r\n");
    
    big_date_queue = xQueueCreate( 1, sizeof(char *) );
    if(big_date_queue != NULL)      printf("big_date_queue���д����ɹ�����\r\n");
    else                            printf("big_date_queue���д���ʧ�ܣ���\r\n");
    
    /* ������ֵ�ź��� */
    /*
    #define   xSemaphoreCreateBinary( )    
              xQueueGenericCreate( 1 ,   semSEMAPHORE_QUEUE_ITEM_LENGTH  ,   queueQUEUE_TYPE_BINARY_SEMAPHORE )
    #define  semSEMAPHORE_QUEUE_ITEM_LENGTH      ( ( uint8_t ) 0U )
    // ��Ϊ����������Ϊ1����Ϣ��СΪ0(void)�Ķ��У�
    */
    semphore_handle = xSemaphoreCreateBinary();
    if(semphore_handle != NULL)     printf("��ֵ�ź��������ɹ�������\r\n");
    else                            printf("��ֵ�ź�������ʧ�ܣ���\r\n");
    
    /* �����������ź��� */
    /*
    #define   xSemaphoreCreateCounting       (  uxMaxCount    ,    uxInitialCount  )
              xQueueCreateCountingSemaphore( (  uxMaxCount  ) , (  uxInitialCount  ) ) 
    // �����ֱ�Ϊ������ֵ�����ֵ�޶�������ֵ�ĳ�ʼֵ
    */
    count_semphore_handle = xSemaphoreCreateCounting(100 , 0);  
    if(count_semphore_handle != NULL)     printf("�������ź��������ɹ�������\r\n");
    else                                  printf("�������ź�������ʧ�ܣ���\r\n");
  
    /* �������м� */
    // �ȴ���������м��еĶ���/�ź���    
    xQueue1 = xQueueCreate( 1, sizeof(uint8_t) );
    if(xQueue1 != NULL)           printf("xQueue1���д����ɹ�����\r\n");
    else                          printf("xQueue1���д���ʧ�ܣ���\r\n");
    
    xSemaphore = xSemaphoreCreateBinary();
    if(xSemaphore != NULL)     printf("���м�xSemaphore�ź��������ɹ�������\r\n");
    else                       printf("���м�xSemaphore�ź�������ʧ�ܣ���\r\n");
    
    // �������м�
    // ����Ϊ�����м������ɵĶ�������
    xQueueSet = xQueueCreateSet(QUEUESET_LENGTH);
    if(xQueueSet != NULL)           printf("���м������ɹ�������\r\n");
    else                            printf("���м�����ʧ�ܣ���\r\n");
    /* �����кͶ�ֵ�ź�����ӵ����м� */
    // ���������м�����Ӷ��У�Ҫע���ʱ�������ڱ���ӵ����м�֮ǰ�������в�������Ч����Ϣ 
    // ����������ӵĶ��о�������м�
    xQueueAddToSet(xQueue1,        xQueueSet);
    xQueueAddToSet(xSemaphore,     xQueueSet);
    
    /* �����¼���־�� */
    EventGroupHandler = xEventGroupCreate();
    if(xQueueSet != NULL)           printf("�¼���־�鴴���ɹ�������\r\n");
    else                            printf("�¼���־�鴴��ʧ�ܣ���\r\n");
    
    /* ����Start Task */
    /* �����񱻴����ú������ھ���̬ */
    xTaskCreate((TaskFunction_t )start_task,            /* ָ����������ָ�� */	
                (const char*    )"start_task",          /* �������֣���󳤶�configMAX_TASK_NAME_LEN */
                (uint16_t       )START_STK_SIZE,        /* �����ջ��С��ע����Ϊ��λ */
                (void*          )NULL,                  /* ������������Ĳ��� */
                (UBaseType_t    )START_TASK_PRIO,       /* �������ȼ�����Χ��0 ~ configMAX_PRIORITIES - 1 */
                (TaskHandle_t*  )&StartTask_Handler);   /* �����������������������ƿ� */
                
    /* ������������� */            
    vTaskStartScheduler();
}

/**
 * @brief       start_task
 * @param       pvParameters : �������(δ�õ�)
 * @retval      ��
 */
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           /* �����ٽ��� */
  
    /* ��������1 */
    xTaskCreate((TaskFunction_t )task1,                 /* ������ */
                (const char*    )"task1",               /* �������� */
                (uint16_t       )TASK1_STK_SIZE,        /* �����ջ��С */
                (void*          )NULL,                  /* ������������Ĳ��� */
                (UBaseType_t    )TASK1_PRIO,            /* �������ȼ� */
                (TaskHandle_t*  )&Task1Task_Handler);   /* ������ */
                
    /* ��������2 */
    xTaskCreate((TaskFunction_t )task2,                 /* ������ */
                (const char*    )"task2",               /* �������� */
                (uint16_t       )TASK2_STK_SIZE,        /* �����ջ��С */
                (void*          )NULL,                  /* ������������Ĳ��� */
                (UBaseType_t    )TASK2_PRIO,            /* �������ȼ� */
                (TaskHandle_t*  )&Task2Task_Handler);   /* ������ */   
                
    /* ��������3 */
    xTaskCreate((TaskFunction_t )task3,                 /* ������ */
                (const char*    )"task3",               /* �������� */
                (uint16_t       )TASK3_STK_SIZE,        /* �����ջ��С */
                (void*          )NULL,                  /* ������������Ĳ��� */
                (UBaseType_t    )TASK3_PRIO,            /* �������ȼ� */
                (TaskHandle_t*  )&Task3Task_Handler);   /* ������ */            
                
    taskEXIT_CRITICAL();            /* �˳��ٽ��� */
                
    vTaskDelete(StartTask_Handler); /* ɾ����ʼ���� */
}


/**
 * @brief       task2
 * @param       pvParameters : �������(δ�õ�)
 * @retval      ��
 */
void task2(void *pvParameters)
{
    uint8_t key = 0;
    char * buf;
    BaseType_t err = 0;
    QueueSetMemberHandle_t  activate_member = NULL;
    uint8_t queue_recv = 0;
  
    while(1)
    {
        /*
        |         ����           |                 ����                 |
        | xQueueReceive()        | �Ӷ���ͷ����ȡ��Ϣ����ɾ����Ϣ         |
        | xQueuePeek()           | �Ӷ���ͷ����ȡ��Ϣ                    |
        | xQueueReceiveFromISR() | ���ж��дӶ���ͷ����ȡ��Ϣ����ɾ����Ϣ |
        | xQueuePeekFromISR()    | ���ж��дӶ���ͷ����ȡ��Ϣ            |
      
        BaseType_t    xQueueReceive( QueueHandle_t   xQueue,  void *   const pvBuffer,  TickType_t   xTicksToWait )
        ���������о���ṹ�壬����ȡ���ݵ�ַ��������ʱʱ��(portMAX_DELAY��ʾ����)
        ����ֵ��pdTRUE  ��ȡ�ɹ���pdFALSE   ��ȡʧ��
        */
        err = xQueueReceive( key_queue, &key, portMAX_DELAY);
        if(err == pdTRUE)     printf("key_queue��ȡ���гɹ������ݣ�%d\r\n",key);
                
        /*
        ��ȡ��ֵ�ź�������:
        BaseType_t   xSemaphoreTake( xSemaphore, xBlockTime ) 
        | xSemaphore | Ҫ��ȡ���ź������ |
        | xBlockTime | ����ʱ��           | 
        ����ֵ��pdTRUE  ��ȡ�ź����ɹ���pdFALSE   ��ʱ����ȡ�ź���ʧ��
        */
        err = xSemaphoreTake(semphore_handle, portMAX_DELAY);   /* ��ȡ�ź��������� */
        if(err == pdTRUE)    printf("��ֵ��ȡ�ź����ɹ�\r\n");
      
        err = xQueueReceive( big_date_queue,&buf, portMAX_DELAY);
        if(err == pdTRUE)     printf("big_date_queue��ȡ���гɹ������ݣ�%s\r\n",buf);
        
        // �����������л�ȡ���м�������Ч��Ϣ�Ķ��У�
        // �����ֱ�Ϊ�����м������м�
        // ����ֵΪ��ȡ����Ϣ�Ķ��о����NULL��ʾ��ȡ��Ϣʧ��
        activate_member = xQueueSelectFromSet(xQueueSet, portMAX_DELAY);     /* �ȴ����м��еĶ��н��յ���Ϣ */
        if (activate_member == xQueue1)
        {
            // ע�⣺������ղ���ʹ��ԭʼ����/��ֵ�ź���������ʹ�ö��м�
            xQueueReceive(activate_member, &queue_recv, portMAX_DELAY);
            printf("���յ�����xQueue1����Ϣ: %d\r\n", queue_recv);
        }
        else if (activate_member == xSemaphore)
        {
            // ע�⣺������ղ���ʹ��ԭʼ����/��ֵ�ź���������ʹ�ö��м�
            xSemaphoreTake(activate_member, portMAX_DELAY);
            printf("��ȡ����ֵ�ź���: xSemaphore\r\n");
        }
        else
            printf("���м��������ݹ��ܳ���\r\n");
        
        // --------------------------------------------
        
        // �����������л�ȡ���м�������Ч��Ϣ�Ķ��У�
        // �����ֱ�Ϊ�����м������м�
        // ����ֵΪ��ȡ����Ϣ�Ķ��о����NULL��ʾ��ȡ��Ϣʧ��
        activate_member = xQueueSelectFromSet(xQueueSet, portMAX_DELAY);     /* �ȴ����м��еĶ��н��յ���Ϣ */
        if (activate_member == xQueue1)
        {
            // ע�⣺������ղ���ʹ��ԭʼ����/��ֵ�ź���������ʹ�ö��м�
            xQueueReceive(activate_member, &queue_recv, portMAX_DELAY);
            printf("���յ�����xQueue1����Ϣ: %d\r\n", queue_recv);
        }
        else if (activate_member == xSemaphore)
        {
            // ע�⣺������ղ���ʹ��ԭʼ����/��ֵ�ź���������ʹ�ö��м�
            xSemaphoreTake(activate_member, portMAX_DELAY);
            printf("��ȡ����ֵ�ź���: xSemaphore\r\n");
        }
        else
            printf("���м��������ݹ��ܳ���\r\n");
                        
        vTaskDelay(25);                      /* ��ʱ1000ticks */
    }
}

/**
 * @brief       task3
 * @param       pvParameters : �������(δ�õ�)
 * @retval      ��
 */
void task3(void *pvParameters)
{
    uint8_t key = 0;
    char * buf;
    BaseType_t   err = 0;
    buf = &buff[0]; /* buf = &buff[0] */
    uint8_t count = 0;
    
    uint8_t key1_count1 = 0;
    uint8_t key2_count1 = 0;
  
    EventBits_t event_bit = 0;
    
    while (1)
    {
        key = key_scan(0);
        
        switch (key)
        {
            case KEY1_PRES:                  
            {
                printf("\n///////// **************��%d�ΰ��°���1(key1)\r\n", ++key1_count1); 
                /*
                | xQueueSend()               | �����е�β��д����Ϣ                                |
                | xQueueSendToBack()         | ͬ xQueueSend()                                    |
                | xQueueSendToFront()        | �����е�ͷ��д����Ϣ                                |
                | xQueueOverwrite()          | ��д������Ϣ��ֻ���ڶ��г���Ϊ 1  �������            |
                | xQueueSendFromISR()        | ���ж��������е�β��д����Ϣ                         |
                | xQueueSendToBackFromISR()  | ͬ xQueueSendFromISR()                             |
                | xQueueSendToFrontFromISR() | ���ж��������е�ͷ��д����Ϣ                         |
                | xQueueOverwriteFromISR()   | ���ж��и�д������Ϣ��ֻ���ڶ��г���Ϊ 1  �������    |
              
                #define  xQueueSend(  xQueue,   pvItemToQueue,   xTicksToWait  )  \
                         xQueueGenericSend( ( xQueue ), ( pvItemToQueue ), ( xTicksToWait ), queueSEND_TO_BACK )
                ���ʾ�Ϊ���ã�xQueueGenericSend()����
                ���������о���ṹ�壬��д�����ݵ�ַ��������ʱʱ��(portMAX_DELAY��ʾ����)��д�뷽ʽ��д�����β��/ͷ��/����д�룩
                ����ֵ��pdTRUE  д��ɹ���errQUEUE_FULL  д��ʧ��
              
                ע�⣺��FreeRTOS�У�д����У���������ʱ��������������˲���ʱ�������ݻᵼ�³���������
                �������Ϊ������ʱ����Ϊ0(֮�������ݻ���ʾ�����ǰ�)�����߸�д������Ϣ��
                */
                err = xQueueSend( big_date_queue, &buf, portMAX_DELAY );
                if(err == pdTRUE)   printf("big_date_queue111�ɹ���������\r\n");        
                else                printf("big_date_queue111����ʧ��\r\n");      

                err = xQueueSend( key_queue, &count, portMAX_DELAY );
                if(err == pdTRUE)   printf("key_queue111�ɹ���������\r\n");  
                else                printf("key_queue111����ʧ��\r\n");                      
                
//                err = xQueueSend( big_date_queue, &buf, 1 );
//                if(err == pdTRUE)   printf("big_date_queue222�ɹ���������\r\n");        
//                else                printf("big_date_queue222����ʧ��\r\n");      

//                err = xQueueSend( key_queue, &count, 1 );
//                if(err == pdTRUE)   printf("key_queue222�ɹ���������\r\n");  
//                else                printf("key_queue222����ʧ��\r\n");        
                
                /*
                �ͷŶ�ֵ�ź���
                #define                    xSemaphoreGive (  xSemaphore  )      
                      xQueueGenericSend( ( QueueHandle_t ) ( xSemaphore )  ,   NULL  ,   semGIVE_BLOCK_TIME  ,   queueSEND_TO_BACK )
                xSemaphore Ϊ ��ֵ�ź������
                #define   semGIVE_BLOCK_TIME                  ( ( TickType_t ) 0U )
                ����ͷŶ�ֵ�ź������Ῠ����������ʱ��0��
                �ͷż����ź����������ź�����+1���ﵽ�����ź��������ֵ���ź������������ֵ(ͬ�����Ῠ��)��
                */
                xSemaphoreGive(count_semphore_handle);      /* �ͷż����ź��� */ 
                if(err == pdPASS)     printf("�����ź����ͷųɹ�����\r\n");
                 
                err = xSemaphoreGive(semphore_handle);      /* �ͷŶ�ֵ�ź��� */ 
                if(err == pdPASS)     printf("��ֵ�ź����ͷųɹ�����\r\n");
                else                  printf("��ֵ�ź����ͷ�ʧ�ܣ���\r\n");
                 
                /* ���м��ͷ�/д�룬ֱ�Ӷ���Ӧ�Ķ���/�ź����������� */
                err = xSemaphoreGive(xSemaphore);      /* �ͷŶ��м��еĶ�ֵ�ź��� */ 
                if(err == pdPASS)     printf("xSemaphore�ź����ͷųɹ�����\r\n");
                else                  printf("xSemaphore�ź����ͷ�ʧ�ܣ���\r\n");
                 
                err = xQueueSend( xQueue1, &count, portMAX_DELAY );   /* �ͷŶ��м��еĶ��� */ 
                if(err == pdTRUE)   printf("xQueue1�ɹ���������\r\n");        
                else                printf("xQueue1����ʧ��\r\n");       
                
                // �����¼���־λAPI���������¼���־���bit0λ��1 
                event_bit = xEventGroupSetBits((EventGroupHandle_t  )EventGroupHandler, /* ���������¼���־���� */
                                               (EventBits_t         )EVENTBIT_0);       /* �����õ�bitλ */
                printf("�¼���־���bit0λ��1����ʱ�¼���־��Ϊ%02X\r\n", event_bit);      
                break;
            }
            case KEY2_PRES:               
            {
                printf("\n///////// **************��%d�ΰ��°���2(key2)\r\n", ++key2_count1); 
                /*
                #define     uxSemaphoreGetCount( xSemaphore )     uxQueueMessagesWaiting( ( QueueHandle_t ) ( xSemaphore ) )
                �˺������ڻ�ȡ�ź�����ǰ����ֵ��С��xSemaphore  �ź������
                ����ֵ����������ʾ��ǰ�ź����ļ���ֵ��С
                ��ȡ�����ź������ź�����-1������ź���Ϊ0�����򽫻����ȣ�
                */
                err = xSemaphoreTake(count_semphore_handle,  portMAX_DELAY);    /* ��ȡ�ź��������� */
                if(err == pdTRUE)     printf("�����ź�����ȡ�ɹ��������ź����ļ���ֵΪ��%d\r\n",(int)uxSemaphoreGetCount(count_semphore_handle));
                
                // �����¼���־λAPI���������¼���־���bit1λ��1 
                event_bit = xEventGroupSetBits((EventGroupHandle_t  )EventGroupHandler, /* ���������¼���־���� */
                                               (EventBits_t         )EVENTBIT_1);       /* �����õ�bitλ */
                printf("�¼���־���bit0λ��1����ʱ�¼���־��Ϊ%02X\r\n", event_bit);      
                break;
            }
            default:
            {
                break;
            }
        }
        
        // LED0_TOGGLE();                                                  /* LED0��˸ */
        count++;
        vTaskDelay(10);
    }
}


/**
 * @brief       task1
 * @param       pvParameters : �������(δ�õ�)
 * @retval      ��
 */
void task1(void *pvParameters)
{
    #if is_output_the_list 
    /* 1. �б�/�б��� */   
    printf("/*************1. �б�/�б���ʵ��**************/\r\n");
    /* 
    �б��� FreeRTOS �е�һ�����ݽṹ�������Ϻ������е����ƣ��б��������� FreeRTOS�е����� 
    �б�����Ǵ�����б��е���Ŀ 
    �б��൱�������б����൱�ڽڵ㣬FreeRTOS �е��б���һ��˫�������� 
    */

    /*
    �б�
    typedef struct xLIST
    {
        listFIRST_LIST_INTEGRITY_CHECK_VALUE                         У��ֵ 
        volatile       UBaseType_t            uxNumberOfItems;       �б��е��б������� 
        ListItem_t   *   configLIST_VOLATILE          pxIndex        ���ڱ����б����ָ��
        MiniListItem_t                        xListEnd               ĩβ�б��� 
        listSECOND_LIST_INTEGRITY_CHECK_VALUE                        У��ֵ 
    } List_t;
  
    // �б������б������ڴ�����ݵĵط�
    struct xLIST_ITEM
    {
        listFIRST_LIST_ITEM_INTEGRITY_CHECK_VALUE                       ���ڼ���б��������������(У��ֵ)
        configLIST_VOLATILE       TickType_t       xItemValue           �б����ֵ  
        struct xLIST_ITEM   *   configLIST_VOLATILE     pxNext          ��һ���б���   
        struct xLIST_ITEM   *   configLIST_VOLATILE     pxPrevious      ��һ���б���   
        void   *   pvOwner                                              �б����ӵ����  
        struct xLIST   *   configLIST_VOLATILE     pxContainer;         �б��������б�  
        listSECOND_LIST_ITEM_INTEGRITY_CHECK_VALUE                      ���ڼ���б��������������  
    };
    typedef struct     xLIST_ITEM     ListItem_t;   
    
    // �����б���Ҳ���б���������б�������ڱ���б��ĩβ�͹������������б��е��б��� 
    struct xMINI_LIST_ITEM
    {
        listFIRST_LIST_ITEM_INTEGRITY_CHECK_VALUE                        ���ڼ������������(У��ֵ)
        configLIST_VOLATILE       TickType_t       xItemValue;           �б����ֵ  
        struct xLIST_ITEM   *   configLIST_VOLATILE     pxNext;          ��һ���б��� 
        struct xLIST_ITEM   *   configLIST_VOLATILE     pxPrevious;      ��һ���б��� 
    };
    typedef struct     xMINI_LIST_ITEM     MiniListItem_t;   
    
    | **����**                                                      | **����**                                                     |
    | ------------------------------------------------------------  | ------------------------------------------------------------ |
    | void vListInitialise (List_t * const pxList)                  | ��ʼ���б�                                                    |
    |                                                                 ��ʼ��ʱ���б���ֻ�� xListEnd����� pxIndex ָ�� xListEnd       |
    |                                                                 ��ʼ��ʱ���б���ֻ�� xListEnd�������һ������һ���б��Ϊ xListEnd ���� |
    | void vListInitialiseItem ( ListItem_t * const pxItem )        | ��ʼ���б���                                                 |
    | void vListInsertEnd ( List_t * const pxList ,  ListItem_t * const pxNewListItem )    |   ���������б���б�����뵽�б� pxIndex ָ��ָ����б���ǰ�棬��һ������Ĳ��뷽��   |
    | void vListInsert ( List_t * const pxList ,  ListItem_t * const pxNewListItem )       |   ���������б���б�����б���ֵ���������������ز��뵽�б���    |
    | UBaseType_t uxListRemove( ListItem_t * const pxItemToRemove ) | �б��Ƴ��б��� �� ����ֵΪ  �б�ʣ���б��������         |
    
    */
  
    /* ��һ������ʼ���б���б��� */
    vListInitialise(&TestList);                 /* ��ʼ���б� */
    vListInitialiseItem(&ListItem1);            /* ��ʼ���б���1 */
    vListInitialiseItem(&ListItem2);            /* ��ʼ���б���2 */
    vListInitialiseItem(&ListItem3);            /* ��ʼ���б���3 */
    ListItem1.xItemValue = 40;
    ListItem2.xItemValue = 60;
    ListItem3.xItemValue = 50;
    
    /* �ڶ�������ӡ�б�������б���ĵ�ַ */
    printf("/**************�ڶ�������ӡ�б���б���ĵ�ַ**************/\r\n");
    printf("��ʱ�б�˳��pxIndex->xListEnd��xListEnd -> xListEnd  \r\n");
    
    printf("��Ŀ\t\t\t��ַ\r\n");
    printf("TestList\t\t0x%p\t\r\n", &TestList);
    printf("TestList->pxIndex\t0x%p\t\r\n", TestList.pxIndex);
    printf("TestList->xListEnd\t0x%p\t\r\n", (&TestList.xListEnd));
    printf("ListItem1\t\t0x%p\t\r\n", &ListItem1);
    printf("ListItem2\t\t0x%p\t\r\n", &ListItem2);
    printf("ListItem3\t\t0x%p\t\r\n", &ListItem3);
    printf("/**************************����***************************/\r\n\r\n");
    vTaskDelay(10);
    
    /* ���������б���1�����б� */
    printf("/*****************���������б���1�����б�******************/\r\n");
    vListInsert((List_t*    )&TestList,         /* �б� */
                (ListItem_t*)&ListItem1);       /* �б��� */
    printf("��ʱ�б�˳��xListEnd -> ListItem1 -> xListEnd  \r\n");
    
    printf("��Ŀ\t\t\t\t��ַ\r\n");
    printf("TestList->xListEnd->pxNext\t0x%p\r\n", (TestList.xListEnd.pxNext));
    printf("ListItem1->pxNext\t\t0x%p\r\n", (ListItem1.pxNext));
    printf("TestList->xListEnd->pxPrevious\t0x%p\r\n", (TestList.xListEnd.pxPrevious));
    printf("ListItem1->pxPrevious\t\t0x%p\r\n", (ListItem1.pxPrevious));
    printf("/**************************����***************************/\r\n\r\n");
    vTaskDelay(10);
    
    /* ���Ĳ����б���2�����б� */
    printf("/*****************���Ĳ����б���2�����б�******************/\r\n");
    vListInsert((List_t*    )&TestList,         /* �б� */
                (ListItem_t*)&ListItem2);       /* �б��� */
    printf("��ʱ�б�˳��xListEnd -> ListItem1 -> ListItem2 -> xListEnd  \r\n");
    
    printf("��Ŀ\t\t\t\t��ַ\r\n");
    printf("TestList->xListEnd->pxNext\t0x%p\r\n", (TestList.xListEnd.pxNext));
    printf("ListItem1->pxNext\t\t0x%p\r\n", (ListItem1.pxNext));
    printf("ListItem2->pxNext\t\t0x%p\r\n", (ListItem2.pxNext));
    printf("TestList->xListEnd->pxPrevious\t0x%p\r\n", (TestList.xListEnd.pxPrevious));
    printf("ListItem1->pxPrevious\t\t0x%p\r\n", (ListItem1.pxPrevious));
    printf("ListItem2->pxPrevious\t\t0x%p\r\n", (ListItem2.pxPrevious));
    printf("/**************************����***************************/\r\n\r\n");
    vTaskDelay(10);
    
    /* ���岽���б���3�����б� */
    printf("/*****************���岽���б���3�����б�******************/\r\n");
    vListInsert((List_t*    )&TestList,         /* �б� */
                (ListItem_t*)&ListItem3);       /* �б��� */
    printf("��ʱ�б�˳��xListEnd -> ListItem1 -> ListItem3 -> ListItem2 -> xListEnd  \r\n");
    
    printf("��Ŀ\t\t\t\t��ַ\r\n");
    printf("TestList->xListEnd->pxNext\t0x%p\r\n", (TestList.xListEnd.pxNext));
    printf("ListItem1->pxNext\t\t0x%p\r\n", (ListItem1.pxNext));
    printf("ListItem2->pxNext\t\t0x%p\r\n", (ListItem2.pxNext));
    printf("ListItem3->pxNext\t\t0x%p\r\n", (ListItem3.pxNext));
    printf("TestList->xListEnd->pxPrevious\t0x%p\r\n", (TestList.xListEnd.pxPrevious));
    printf("ListItem1->pxPrevious\t\t0x%p\r\n", (ListItem1.pxPrevious));
    printf("ListItem2->pxPrevious\t\t0x%p\r\n", (ListItem2.pxPrevious));
    printf("ListItem3->pxPrevious\t\t0x%p\r\n", (ListItem3.pxPrevious));
    printf("/**************************����***************************/\r\n\r\n");
    vTaskDelay(10);
    
    /* ���������Ƴ��б���2 */
    printf("/*******************���������Ƴ��б���2********************/\r\n");
    uxListRemove((ListItem_t*   )&ListItem2);   /* �Ƴ��б��� */
    printf("��ʱ�б�˳��xListEnd -> ListItem1 -> ListItem3 -> xListEnd \r\n");
    
    printf("��Ŀ\t\t\t\t��ַ\r\n");
    printf("TestList->xListEnd->pxNext\t0x%p\r\n", (TestList.xListEnd.pxNext));
    printf("ListItem1->pxNext\t\t0x%p\r\n", (ListItem1.pxNext));
    printf("ListItem3->pxNext\t\t0x%p\r\n", (ListItem3.pxNext));
    printf("TestList->xListEnd->pxPrevious\t0x%p\r\n", (TestList.xListEnd.pxPrevious));
    printf("ListItem1->pxPrevious\t\t0x%p\r\n", (ListItem1.pxPrevious));
    printf("ListItem3->pxPrevious\t\t0x%p\r\n", (ListItem3.pxPrevious));
    printf("/**************************����***************************/\r\n\r\n");
    vTaskDelay(10);
    
    /* ���߲����б�ĩβ����б���2 */
    printf("/****************���߲����б�ĩβ����б���2****************/\r\n");
    vListInsertEnd((List_t*     )&TestList,     /* �б� */
                   (ListItem_t* )&ListItem2);   /* �б��� */
    printf("��ʱ�б�˳��pxIndex->xListEnd��xListEnd -> ListItem1 -> ListItem3 -> ListItem2 -> xListEnd \r\n");
    printf("��Ŀ\t\t\t\t��ַ\r\n");
    
    printf("TestList->pxIndex\t\t0x%p\r\n", TestList.pxIndex);
    printf("TestList->xListEnd->pxNext\t0x%p\r\n", (TestList.xListEnd.pxNext));
    printf("ListItem1->pxNext\t\t0x%p\r\n", (ListItem1.pxNext));
    printf("ListItem2->pxNext\t\t0x%p\r\n", (ListItem2.pxNext));
    printf("ListItem3->pxNext\t\t0x%p\r\n", (ListItem3.pxNext));
    printf("TestList->xListEnd->pxPrevious\t0x%p\r\n", (TestList.xListEnd.pxPrevious));
    printf("ListItem1->pxPrevious\t\t0x%p\r\n", (ListItem1.pxPrevious));
    printf("ListItem2->pxPrevious\t\t0x%p\r\n", (ListItem2.pxPrevious));
    printf("ListItem3->pxPrevious\t\t0x%p\r\n", (ListItem3.pxPrevious));
    printf("/************************ʵ�����***************************/\r\n\r\n");
    vTaskDelay(10);
    
    while (key_scan(0) != KEY1_PRES)
    {
        vTaskDelay(5);
    }
    
    /**************************************************************************************************************/
    
    /* 2. FreeRTOS�������API���� */   
    printf("/*************2. FreeRTOS�������API����**************/\r\n");
    
    uint32_t        i               = 0;
    UBaseType_t     task_num        = 0;
    TaskStatus_t    *status_array   = NULL;
    TaskHandle_t    task_handle     = NULL;
    TaskStatus_t    *task_info      = NULL;
    eTaskState      task_state      = eInvalid;
    char            *task_state_str = NULL;
    char            *task_info_buf  = NULL;
    UBaseType_t     task_stack_min  = 0;    
    
    
    /* ��һ��������uxTaskGetSystemState()��ʹ�� */
    printf("/********��һ��������uxTaskGetSystemState()��ʹ��**********/\r\n");

    /* UBaseType_t uxTaskGetNumberOfTasks( void ) */
    /* �˺������ڻ�ȡϵͳ��������������� �� ����ֵ ϵͳ����������� */
    task_num = uxTaskGetNumberOfTasks();                            /* ��ȡϵͳ�������� */
    
    status_array = malloc(task_num * sizeof(TaskStatus_t));
    
    /*
    UBaseType_t  uxTaskGetSystemState(   TaskStatus_t * const pxTaskStatusArray,
                                         const UBaseType_t uxArraySize,
                                         configRUN_TIME_COUNTER_TYPE * const pulTotalRunTime  )
    | xTaskStatusArray | ָ��TaskStatus_t �ṹ�������׵�ַ         |
    | uxArraySize      | ������Ϣ�������С                        |
    | pulTotalRunTime  | ϵͳ������ʱ�䣬ΪNULL ��ʡ��������ʱ��ֵ |       
    �˺������ڻ�ȡϵͳ���������������״̬��Ϣ��ʹ�øú����轫�� configUSE_TRACE_FACILITY �� 1    
    */
    task_num = uxTaskGetSystemState((TaskStatus_t* )status_array,   /* ����״̬��Ϣbuffer */
                                    (UBaseType_t   )task_num,       /* buffer��С */
                                    (uint32_t*     )NULL);          /* ����ȡ��������ʱ����Ϣ */
                                    
    printf("������\t\t���ȼ�\t\t������\r\n");
    for (i=0; i<task_num; i++)
    {
        printf("%s\t%s%ld\t\t%ld\r\n",
                status_array[i].pcTaskName,
                strlen(status_array[i].pcTaskName) > 7 ? "": "\t",
                status_array[i].uxCurrentPriority,
                status_array[i].xTaskNumber);
    }
    
    free(status_array);

    printf("/**************************����***************************/\r\n");
    vTaskDelay(10);
    
    
    /* �ڶ���������vTaskGetInfo()��ʹ�� */
    printf("/************�ڶ���������vTaskGetInfo()��ʹ��**************/\r\n");
    task_info = malloc(sizeof(TaskStatus_t));
    
    /* TaskHandle_t xTaskGetHandle(const char * pcNameToQuery);  */
    /* �˺�������ͨ����������ȡ������ �� ʹ�øú����轫�� INCLUDE_xTaskGetHandle �� 1 */
    /* �β�  pcNameToQuery  ������  ��  ����ֵ  TaskHandle  ������  */
    task_handle = xTaskGetHandle("task1");                          /* ��ȡ������ */
    vTaskGetInfo((TaskHandle_t  )task_handle,                       /* ������ */
                 (TaskStatus_t* )task_info,                         /* ������Ϣbuffer */
                 (BaseType_t    )pdTRUE,                            /* ����ͳ�������ջ��ʷ��Сֵ */
                 (eTaskState    )eInvalid);                         /* ��ȡ��������״̬ */
    /*
    void vTaskGetInfo(  TaskHandle_t  xTask,
                        TaskStatus_t *  pxTaskStatus,
                        BaseType_t  xGetFreeStackSpace,
                        eTaskState  eState  )  
    �˺������ڻ�ȡָ���ĵ��������״̬��Ϣ��ʹ�øú����轫�� configUSE_TRACE_FACILITY  �� 1 
    | xTask              | ָ����ȡ��Ϣ������ľ��                                     |
    | pxTaskStatus       | ����������Ϣ�ı���                                           |
    | xGetFreeStackSpace | ����ջ��ʷʣ����Сֵ��  ��Ϊ��pdFALSE�� ������������裬  ��Ϊ��pdTRUE��������ʷʣ����С��ջ |
    | eState             | ����״̬����ֱ�Ӹ�ֵ�������ȡ���롰eInvalid��                 |
    typedef enum
    {   
      eRunning = 0,    ����̬   
      eReady,          ����̬  
      eBlocked,        ����̬  
      eSuspended,      ��̬  
      eDeleted,        ����ɾ��  
      eInvalid,        ��Ч   
    } eTaskState;
    */
    printf("������:\t\t\t%s\r\n", task_info->pcTaskName);
    printf("������:\t\t\t%ld\r\n", task_info->xTaskNumber);
    printf("����׳̬:\t\t\t%d\r\n", task_info->eCurrentState);
    printf("����ǰ���ȼ�:\t\t%ld\r\n", task_info->uxCurrentPriority);
    printf("��������ȼ�:\t\t%ld\r\n", task_info->uxBasePriority);
    printf("�����ջ����ַ:\t\t0x%p\r\n", task_info->pxStackBase);
    printf("�����ջ��ʷʣ����Сֵ:\t\t%d\r\n", task_info->usStackHighWaterMark);
    free(task_info);
    
    printf("/**************************����***************************/\r\n");
    vTaskDelay(10);
    
    
    /* ������������eTaskGetState()��ʹ��  */
    printf("/***********������������eTaskGetState()��ʹ��*************/\r\n");  
    
    task_state_str = malloc(10);
    
    /* TaskHandle_t xTaskGetHandle(const char * pcNameToQuery);  */
    /* �˺�������ͨ����������ȡ������ �� ʹ�øú����轫�� INCLUDE_xTaskGetHandle �� 1 */
    /* �β�  pcNameToQuery  ������  ��  ����ֵ  TaskHandle  ������  */
    task_handle = xTaskGetHandle("task1");
    
    /* eTaskState    eTaskGetState(TaskHandle_t xTask);  */
    /* �˺������ڲ�ѯĳ�����������״̬��ʹ�ô˺����轫�� INCLUDE_eTaskGetState ��1  */
    /* �β�  xTask  ����ȡ״̬�����������  ��  ����ֵ  eTaskState  ����״̬  */
    /*
    typedef enum
    {   
      eRunning = 0,    ����̬   
      eReady,          ����̬  
      eBlocked,        ����̬  
      eSuspended,      ��̬  
      eDeleted,        ����ɾ��  
      eInvalid,        ��Ч   
    } eTaskState;
    */
    task_state = eTaskGetState(task_handle);                        /* ��ȡ��������״̬ */
    sprintf(task_state_str, task_state == eRunning ? "Runing" :
                            task_state == eReady ? "Ready" :
                            task_state == eBlocked ? "Blocked" :
                            task_state == eSuspended ? "Suspended" :
                            task_state == eDeleted ? "Deleted" :
                            task_state == eInvalid ? "Invalid" :
                                                     "");
    printf("����״ֵ̬: %d����Ӧ״̬Ϊ: %s\r\n", task_state, task_state_str);
    free(task_state_str);
    
    printf("/**************************����***************************/\r\n");
    vTaskDelay(10);
    
    
    /* ���Ĳ�������vTaskList()��ʹ�� */
    printf("/*************���Ĳ�������vTaskList()��ʹ��*************/\r\n");
    task_info_buf = malloc(250);       
    // ע��һ�£����ﲻҪ���������ļ��У�����ѵĴ�С
    // Stack_Size      EQU     0x00000400
    // Heap_Size       EQU     0x00000200    
    /*
    void   vTaskList(char * pcWriteBuffer)
    �˺��������ԡ���񡱵���ʽ��ȡϵͳ���������Ϣ ��ʹ�ô˺����轫�� configUSE_TRACE_FACILITY ��configUSE_STATS_FORMATTING_FUNCTIONS ��1 
    �βΣ�pcWriteBuffer  ����������Ϣ�Ļ���ָ��
    Name        �� ���������ʱ��������������֡�
    State       �� �����׳̬��Ϣ�� B ������̬�� R �Ǿ���̬�� S �ǹ���̬�� D ��ɾ��̬
    Priority    �� �������ȼ���
    Stack       �� �����ջ�ġ���ˮλ�ߡ������Ƕ�ջ��ʷ��Сʣ���С��
    Num         �� �����ţ���������Ψһ�ģ����������ʹ��ͬһ����������ʱ�����ͨ���˱���������֡� 
    */
    vTaskList(task_info_buf);                                       /* ��ȡ�����������Ϣ */ 
    printf("������\t״̬\t���ȼ�\tʣ��ջ\t�������\r\n");
    printf("%s\r\n", task_info_buf);
    
    free(task_info_buf);
    
    /* TaskHandle_t xTaskGetHandle(const char * pcNameToQuery);  */
    /* �˺�������ͨ����������ȡ������ �� ʹ�øú����轫�� INCLUDE_xTaskGetHandle �� 1 */
    /* �β�  pcNameToQuery  ������  ��  ����ֵ  TaskHandle  ������  */
    task_handle = xTaskGetHandle("task1");
    /* ��ȡ���������ջ��ʷʣ����Сֵ */
    task_stack_min = uxTaskGetStackHighWaterMark( task_handle );
    printf("task1��ʷʣ����С��ջΪ%ld\r\n",task_stack_min);
    
    printf("/************************ʵ�����***************************/\r\n");
    
    while (1)
    {
        char    *runtime_info   = NULL;
        runtime_info = malloc(100);
      
        /*
        Void    vTaskGetRunTimeStats( char * pcWriteBuffer ) 
        �˺�������ͳ�����������ʱ����Ϣ��
        �β�  pcWriteBuffer  ������������ʱ����Ϣ�Ļ���ָ��
      
        ʹ�ô˺����轫�� configGENERATE_RUN_TIME_STAT ��configUSE_STATS_FORMATTING_FUNCTIONS ��1 
        ����Ҫʵ��2���궨�壺
        portCONFIGURE_TIMER_FOR_RUNTIME_STATE() �����ڳ�ʼ������������������ʱ��ͳ�Ƶ�ʱ����ʱ����ע�⣺���ʱ����ʱ���ļ�ʱ���������ϵͳʱ�ӽ��ľ��ȵ�10��100����
        portGET_RUN_TIME_COUNTER_VALUE()�����ڻ�ȡ�ù���ʱ��Ӳ����ʱ�������ļ���ֵ ��

        Task����������     Abs Time������ʵ�����е���ʱ�䣨����ʱ�䣩     % Time��ռ�ܴ���ʱ��İٷֱ�
        */
        vTaskGetRunTimeStats(runtime_info);             /* ��ȡ��������ʱ����Ϣ */
      
        printf("������\t����ʱ��\t������ռ�ٷֱ�\r\n");
        printf("%s\r\n", runtime_info);
      
        free(runtime_info);
        
        //--------------�ڴ������� 
        LED1_TOGGLE();

        uint8_t * buf = NULL;
        buf = pvPortMalloc(30);             /* �����ڴ� */
        if(buf != NULL)   printf("�����ڴ�ɹ���\r\n");
        
        printf("ʣ��Ŀ����ڴ��СΪ��%d\r\n", xPortGetFreeHeapSize());
      
        vPortFree(buf);                     /* �ͷ��ڴ� */
        printf("�ͷ��ڴ棡��\r\n");
        
        printf("ʣ��Ŀ����ڴ��СΪ��%d\r\n", xPortGetFreeHeapSize());
        //--------------  
          
        for(uint8_t i=0; i<30; i++)
            vTaskDelay(1000);
    }
    

    #else

    EventBits_t event_bit = 0;
    while (1)
    {   
//        LED1_TOGGLE();

        // ����ֵ�������¼����е��¼���־λ
        event_bit = xEventGroupWaitBits((EventGroupHandle_t )EventGroupHandler,              /* �ȴ����¼���־���� */
                ( EventBits_t  )EVENTBIT_0 | EVENTBIT_1,        /* �ȴ����¼���־λ���������߼���ȴ�����¼���־λ */
                ( BaseType_t   )pdTRUE,                         /* �ɹ��ȴ����¼���־λ������¼����ж�Ӧ���¼���־λ��pdTRUE�����uxBitsToWaitForָ��λ��pdFALSE������� */
                ( BaseType_t   )pdTRUE,                         /* �ȴ� uxBitsToWaitFor �е������¼���־λ��pdTRUE���ȴ���λ��ȫ��Ϊ1��pdFALSE���ȴ���λ��ĳ��Ϊ1 */
                ( TickType_t   )portMAX_DELAY);                 /* �ȴ�ʱ�䣬portMAX_DELAY��ʾ���� */
      
        printf("�¼���־���еı�־λΪ%02X\r\n", event_bit);
        vTaskDelay(10);
    } 
    
    #endif
}


/*
#define is_output_the_list 0       
if    is_output_the_list==1    ����������б�״̬��Ϣ
if    is_output_the_list==0    ����������Ϣ
is_output_the_list==0   ʵ��
��2�� ����1���ٰ�2�� ����2;
��2�� ����1���ٰ�3�� ����2;
�����Ϣ��
---------------------------------------------------------------------------
key_queue���д����ɹ�����
big_date_queue���д����ɹ�����
��ֵ�ź��������ɹ�������
�������ź��������ɹ�������
xQueue1���д����ɹ�����
���м�xSemaphore�ź��������ɹ�������
���м������ɹ�������
�¼���־�鴴���ɹ�������

///////// **************��1�ΰ��°���1(key1)
big_date_queue111�ɹ���������
key_queue��ȡ���гɹ������ݣ�248
key_queue111�ɹ���������
�����ź����ͷųɹ�����
��ֵ��ȡ�ź����ɹ�
big_date_queue��ȡ���гɹ������ݣ�����һ�������飬�������飡
��ֵ�ź����ͷųɹ�����
��ȡ����ֵ�ź���: xSemaphore
xSemaphore�ź����ͷųɹ�����
���յ�����xQueue1����Ϣ: 248
xQueue1�ɹ���������
�¼���־���bit0λ��1����ʱ�¼���־��Ϊ01

///////// **************��2�ΰ��°���1(key1)
big_date_queue111�ɹ���������
key_queue��ȡ���гɹ������ݣ�208
key_queue111�ɹ���������
�����ź����ͷųɹ�����
��ֵ��ȡ�ź����ɹ�
big_date_queue��ȡ���гɹ������ݣ�����һ�������飬�������飡
��ֵ�ź����ͷųɹ�����
��ȡ����ֵ�ź���: xSemaphore
xSemaphore�ź����ͷųɹ�����
���յ�����xQueue1����Ϣ: 208
xQueue1�ɹ���������
�¼���־���bit0λ��1����ʱ�¼���־��Ϊ01

///////// **************��1�ΰ��°���2(key2)
�����ź�����ȡ�ɹ��������ź����ļ���ֵΪ��1
�¼���־���еı�־λΪ03
�¼���־���bit0λ��1����ʱ�¼���־��Ϊ00

///////// **************��2�ΰ��°���2(key2)
�����ź�����ȡ�ɹ��������ź����ļ���ֵΪ��0
�¼���־���bit0λ��1����ʱ�¼���־��Ϊ02

///////// **************��3�ΰ��°���1(key1)
big_date_queue111�ɹ���������
key_queue��ȡ���гɹ������ݣ�113
key_queue111�ɹ���������
�����ź����ͷųɹ�����
��ֵ��ȡ�ź����ɹ�
big_date_queue��ȡ���гɹ������ݣ�����һ�������飬�������飡
��ֵ�ź����ͷųɹ�����
��ȡ����ֵ�ź���: xSemaphore
xSemaphore�ź����ͷųɹ�����
���յ�����xQueue1����Ϣ: 113
xQueue1�ɹ���������
�¼���־���еı�־λΪ03
�¼���־���bit0λ��1����ʱ�¼���־��Ϊ00

///////// **************��4�ΰ��°���1(key1)
big_date_queue111�ɹ���������
key_queue��ȡ���гɹ������ݣ�22
key_queue111�ɹ���������
�����ź����ͷųɹ�����
��ֵ��ȡ�ź����ɹ�
big_date_queue��ȡ���гɹ������ݣ�����һ�������飬�������飡
��ֵ�ź����ͷųɹ�����
��ȡ����ֵ�ź���: xSemaphore
xSemaphore�ź����ͷųɹ�����
���յ�����xQueue1����Ϣ: 22
xQueue1�ɹ���������
�¼���־���bit0λ��1����ʱ�¼���־��Ϊ01

///////// **************��3�ΰ��°���2(key2)
�����ź�����ȡ�ɹ��������ź����ļ���ֵΪ��1
�¼���־���еı�־λΪ03
�¼���־���bit0λ��1����ʱ�¼���־��Ϊ00

///////// **************��4�ΰ��°���2(key2)
�����ź�����ȡ�ɹ��������ź����ļ���ֵΪ��0
�¼���־���bit0λ��1����ʱ�¼���־��Ϊ02

///////// **************��5�ΰ��°���2(key2)


������֮��ϵͳ������


*/

