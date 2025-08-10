/**
 ****************************************************************************************************
 * @file        freertos.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.4
 * @date        2022-01-04
 * @brief       FreeRTOS 移植实验
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 战舰F103开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
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
/*FreeRTOS配置*/

/* START_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 任务函数
 */
#define START_TASK_PRIO 1                   /* 任务优先级 */
#define START_STK_SIZE  128                 /* 任务堆栈大小 */
TaskHandle_t            StartTask_Handler;  /* 任务句柄 */
void start_task(void *pvParameters);        /* 任务函数 */

/* TASK1 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 任务函数
 */
#define TASK1_PRIO      12                  /* 任务优先级 */
#define TASK1_STK_SIZE  128                 /* 任务堆栈大小 */
TaskHandle_t            Task1Task_Handler;  /* 任务句柄 */
void task1(void *pvParameters);             /* 任务函数 */

/* TASK2 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 任务函数
 */
#define TASK2_PRIO      8                   /* 任务优先级 */
#define TASK2_STK_SIZE  128                 /* 任务堆栈大小 */
TaskHandle_t            Task2Task_Handler;  /* 任务句柄 */
void task2(void *pvParameters);             /* 任务函数 */

/* TASK3 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK3_PRIO      6                   /* 任务优先级 */
#define TASK3_STK_SIZE  128                 /* 任务堆栈大小 */
TaskHandle_t            Task3Task_Handler;  /* 任务句柄 */
void task3(void *pvParameters);             /* 任务函数 */

List_t                  TestList;           /* 定义测试列表 */
ListItem_t              ListItem1;          /* 定义测试列表项1 */
ListItem_t              ListItem2;          /* 定义测试列表项2 */
ListItem_t              ListItem3;          /* 定义测试列表项3 */
/******************************************************************************************************/
#define is_output_the_list    0             // 是否输出列表/状态信息

/******************************************************************************************************/
QueueHandle_t  key_queue;        /* 小数据句柄 */
QueueHandle_t  big_date_queue;   /* 大数据句柄 */
char buff[80] = {"我是一个大数组，大大的数组！"};
 
QueueHandle_t  semphore_handle;          /* 二值信号量 */
QueueHandle_t  count_semphore_handle;    /* 计数信号量 */
 
EventGroupHandle_t   EventGroupHandler;       /* 事件标志组句柄 */
#define EVENTBIT_0      (1 << 0)              /* 事件位 */
#define EVENTBIT_1      (1 << 1)
#define EVENTBIT_ALL    (EVENTBIT_0 | EVENTBIT_1)
/******************************************************************************************************/
// 这里让 xQueue1/xSemaphore组成一个队列集
QueueHandle_t               xQueue1;            /* 定义队列1 */
SemaphoreHandle_t           xSemaphore;         /* 定义二值信号量 */
static QueueSetHandle_t     xQueueSet;          /* 定义队列集 */
#define QUEUESET_LENGTH     2                   /* 队列集支持的消息个数 */

/**
 * @brief       FreeRTOS例程入口函数
 * @param       无
 * @retval      无
 */
void freertos_demo(void)
{
    /* 队列的创建 */
    // xQueueCreate (  uxQueueLength,   uxItemSize  )   
    // 队列长度，队列项目的大小
    // 队列创建失败，返回值NULL；队列创建成功，返回队列句柄
    /* 
    #define    xQueueCreate         ( uxQueueLength,   uxItemSize  )   \
               xQueueGenericCreate( ( uxQueueLength ), ( uxItemSize ), (queueQUEUE_TYPE_BASE )) 
    #define queueQUEUE_TYPE_BASE              ( ( uint8_t ) 0U )   // 队列  ，这里queueQUEUE_TYPE_BASE表示队列
    */
    key_queue = xQueueCreate( 2, sizeof(uint8_t) );
    if(key_queue != NULL)           printf("key_queue队列创建成功！！\r\n");
    else                            printf("key_queue队列创建失败！！\r\n");
    
    big_date_queue = xQueueCreate( 1, sizeof(char *) );
    if(big_date_queue != NULL)      printf("big_date_queue队列创建成功！！\r\n");
    else                            printf("big_date_queue队列创建失败！！\r\n");
    
    /* 创建二值信号量 */
    /*
    #define   xSemaphoreCreateBinary( )    
              xQueueGenericCreate( 1 ,   semSEMAPHORE_QUEUE_ITEM_LENGTH  ,   queueQUEUE_TYPE_BINARY_SEMAPHORE )
    #define  semSEMAPHORE_QUEUE_ITEM_LENGTH      ( ( uint8_t ) 0U )
    // 即为创建，长度为1，消息大小为0(void)的队列；
    */
    semphore_handle = xSemaphoreCreateBinary();
    if(semphore_handle != NULL)     printf("二值信号量创建成功！！！\r\n");
    else                            printf("二值信号量创建失败！！\r\n");
    
    /* 创建计数型信号量 */
    /*
    #define   xSemaphoreCreateCounting       (  uxMaxCount    ,    uxInitialCount  )
              xQueueCreateCountingSemaphore( (  uxMaxCount  ) , (  uxInitialCount  ) ) 
    // 参数分别为：计数值的最大值限定，计数值的初始值
    */
    count_semphore_handle = xSemaphoreCreateCounting(100 , 0);  
    if(count_semphore_handle != NULL)     printf("计数型信号量创建成功！！！\r\n");
    else                                  printf("计数型信号量创建失败！！\r\n");
  
    /* 创建队列集 */
    // 先创建放入队列集中的队列/信号量    
    xQueue1 = xQueueCreate( 1, sizeof(uint8_t) );
    if(xQueue1 != NULL)           printf("xQueue1队列创建成功！！\r\n");
    else                          printf("xQueue1队列创建失败！！\r\n");
    
    xSemaphore = xSemaphoreCreateBinary();
    if(xSemaphore != NULL)     printf("队列集xSemaphore信号量创建成功！！！\r\n");
    else                       printf("队列集xSemaphore信号量创建失败！！\r\n");
    
    // 创建队列集
    // 参数为：队列集可容纳的队列数量
    xQueueSet = xQueueCreateSet(QUEUESET_LENGTH);
    if(xQueueSet != NULL)           printf("队列集创建成功！！！\r\n");
    else                            printf("队列集创建失败！！\r\n");
    /* 将队列和二值信号量添加到队列集 */
    // 用于往队列集中添加队列，要注意的时，队列在被添加到队列集之前，队列中不能有有效的消息 
    // 参数：待添加的队列句柄，队列集
    xQueueAddToSet(xQueue1,        xQueueSet);
    xQueueAddToSet(xSemaphore,     xQueueSet);
    
    /* 创建事件标志组 */
    EventGroupHandler = xEventGroupCreate();
    if(xQueueSet != NULL)           printf("事件标志组创建成功！！！\r\n");
    else                            printf("事件标志组创建失败！！\r\n");
    
    /* 创建Start Task */
    /* 当任务被创建好后，立马处于就绪态 */
    xTaskCreate((TaskFunction_t )start_task,            /* 指向任务函数的指针 */	
                (const char*    )"start_task",          /* 任务名字，最大长度configMAX_TASK_NAME_LEN */
                (uint16_t       )START_STK_SIZE,        /* 任务堆栈大小，注意字为单位 */
                (void*          )NULL,                  /* 传入给任务函数的参数 */
                (UBaseType_t    )START_TASK_PRIO,       /* 任务优先级，范围：0 ~ configMAX_PRIORITIES - 1 */
                (TaskHandle_t*  )&StartTask_Handler);   /* 任务句柄，就是任务的任务控制块 */
                
    /* 开启任务调度器 */            
    vTaskStartScheduler();
}

/**
 * @brief       start_task
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           /* 进入临界区 */
  
    /* 创建任务1 */
    xTaskCreate((TaskFunction_t )task1,                 /* 任务函数 */
                (const char*    )"task1",               /* 任务名称 */
                (uint16_t       )TASK1_STK_SIZE,        /* 任务堆栈大小 */
                (void*          )NULL,                  /* 传入给任务函数的参数 */
                (UBaseType_t    )TASK1_PRIO,            /* 任务优先级 */
                (TaskHandle_t*  )&Task1Task_Handler);   /* 任务句柄 */
                
    /* 创建任务2 */
    xTaskCreate((TaskFunction_t )task2,                 /* 任务函数 */
                (const char*    )"task2",               /* 任务名称 */
                (uint16_t       )TASK2_STK_SIZE,        /* 任务堆栈大小 */
                (void*          )NULL,                  /* 传入给任务函数的参数 */
                (UBaseType_t    )TASK2_PRIO,            /* 任务优先级 */
                (TaskHandle_t*  )&Task2Task_Handler);   /* 任务句柄 */   
                
    /* 创建任务3 */
    xTaskCreate((TaskFunction_t )task3,                 /* 任务函数 */
                (const char*    )"task3",               /* 任务名称 */
                (uint16_t       )TASK3_STK_SIZE,        /* 任务堆栈大小 */
                (void*          )NULL,                  /* 传入给任务函数的参数 */
                (UBaseType_t    )TASK3_PRIO,            /* 任务优先级 */
                (TaskHandle_t*  )&Task3Task_Handler);   /* 任务句柄 */            
                
    taskEXIT_CRITICAL();            /* 退出临界区 */
                
    vTaskDelete(StartTask_Handler); /* 删除开始任务 */
}


/**
 * @brief       task2
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
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
        |         函数           |                 描述                 |
        | xQueueReceive()        | 从队列头部读取消息，并删除消息         |
        | xQueuePeek()           | 从队列头部读取消息                    |
        | xQueueReceiveFromISR() | 在中断中从队列头部读取消息，并删除消息 |
        | xQueuePeekFromISR()    | 在中断中从队列头部读取消息            |
      
        BaseType_t    xQueueReceive( QueueHandle_t   xQueue,  void *   const pvBuffer,  TickType_t   xTicksToWait )
        参数：队列句柄结构体，待读取数据地址，阻塞超时时间(portMAX_DELAY表示死等)
        返回值：pdTRUE  读取成功；pdFALSE   读取失败
        */
        err = xQueueReceive( key_queue, &key, portMAX_DELAY);
        if(err == pdTRUE)     printf("key_queue读取队列成功，数据：%d\r\n",key);
                
        /*
        获取二值信号量函数:
        BaseType_t   xSemaphoreTake( xSemaphore, xBlockTime ) 
        | xSemaphore | 要获取的信号量句柄 |
        | xBlockTime | 阻塞时间           | 
        返回值：pdTRUE  获取信号量成功；pdFALSE   超时，获取信号量失败
        */
        err = xSemaphoreTake(semphore_handle, portMAX_DELAY);   /* 获取信号量并死等 */
        if(err == pdTRUE)    printf("二值获取信号量成功\r\n");
      
        err = xQueueReceive( big_date_queue,&buf, portMAX_DELAY);
        if(err == pdTRUE)     printf("big_date_queue读取队列成功，数据：%s\r\n",buf);
        
        // 用于在任务中获取队列集中有有效消息的队列，
        // 参数分别为：队列集，队列集
        // 返回值为获取到消息的队列句柄，NULL表示获取消息失败
        activate_member = xQueueSelectFromSet(xQueueSet, portMAX_DELAY);     /* 等待队列集中的队列接收到消息 */
        if (activate_member == xQueue1)
        {
            // 注意：这里接收不能使用原始队列/二值信号量，必须使用队列集
            xQueueReceive(activate_member, &queue_recv, portMAX_DELAY);
            printf("接收到来自xQueue1的消息: %d\r\n", queue_recv);
        }
        else if (activate_member == xSemaphore)
        {
            // 注意：这里接收不能使用原始队列/二值信号量，必须使用队列集
            xSemaphoreTake(activate_member, portMAX_DELAY);
            printf("获取到二值信号量: xSemaphore\r\n");
        }
        else
            printf("队列集接收数据功能出错\r\n");
        
        // --------------------------------------------
        
        // 用于在任务中获取队列集中有有效消息的队列，
        // 参数分别为：队列集，队列集
        // 返回值为获取到消息的队列句柄，NULL表示获取消息失败
        activate_member = xQueueSelectFromSet(xQueueSet, portMAX_DELAY);     /* 等待队列集中的队列接收到消息 */
        if (activate_member == xQueue1)
        {
            // 注意：这里接收不能使用原始队列/二值信号量，必须使用队列集
            xQueueReceive(activate_member, &queue_recv, portMAX_DELAY);
            printf("接收到来自xQueue1的消息: %d\r\n", queue_recv);
        }
        else if (activate_member == xSemaphore)
        {
            // 注意：这里接收不能使用原始队列/二值信号量，必须使用队列集
            xSemaphoreTake(activate_member, portMAX_DELAY);
            printf("获取到二值信号量: xSemaphore\r\n");
        }
        else
            printf("队列集接收数据功能出错\r\n");
                        
        vTaskDelay(25);                      /* 延时1000ticks */
    }
}

/**
 * @brief       task3
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
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
                printf("\n///////// **************第%d次按下按键1(key1)\r\n", ++key1_count1); 
                /*
                | xQueueSend()               | 往队列的尾部写入消息                                |
                | xQueueSendToBack()         | 同 xQueueSend()                                    |
                | xQueueSendToFront()        | 往队列的头部写入消息                                |
                | xQueueOverwrite()          | 覆写队列消息（只用于队列长度为 1  的情况）            |
                | xQueueSendFromISR()        | 在中断中往队列的尾部写入消息                         |
                | xQueueSendToBackFromISR()  | 同 xQueueSendFromISR()                             |
                | xQueueSendToFrontFromISR() | 在中断中往队列的头部写入消息                         |
                | xQueueOverwriteFromISR()   | 在中断中覆写队列消息（只用于队列长度为 1  的情况）    |
              
                #define  xQueueSend(  xQueue,   pvItemToQueue,   xTicksToWait  )  \
                         xQueueGenericSend( ( xQueue ), ( pvItemToQueue ), ( xTicksToWait ), queueSEND_TO_BACK )
                本质均为调用：xQueueGenericSend()函数
                参数：队列句柄结构体，待写入数据地址，阻塞超时时间(portMAX_DELAY表示死等)，写入方式（写入队列尾部/头部/覆盖写入）
                返回值：pdTRUE  写入成功；errQUEUE_FULL  写入失败
              
                注意：在FreeRTOS中，写入队列：当队列满时，可阻塞！（因此不及时接收数据会导致程序卡死！）
                解决方法为：发送时间设为0(之后发送数据会显示发送是吧)，或者覆写队列消息；
                */
                err = xQueueSend( big_date_queue, &buf, portMAX_DELAY );
                if(err == pdTRUE)   printf("big_date_queue111成功发送数据\r\n");        
                else                printf("big_date_queue111发送失败\r\n");      

                err = xQueueSend( key_queue, &count, portMAX_DELAY );
                if(err == pdTRUE)   printf("key_queue111成功发送数据\r\n");  
                else                printf("key_queue111发送失败\r\n");                      
                
//                err = xQueueSend( big_date_queue, &buf, 1 );
//                if(err == pdTRUE)   printf("big_date_queue222成功发送数据\r\n");        
//                else                printf("big_date_queue222发送失败\r\n");      

//                err = xQueueSend( key_queue, &count, 1 );
//                if(err == pdTRUE)   printf("key_queue222成功发送数据\r\n");  
//                else                printf("key_queue222发送失败\r\n");        
                
                /*
                释放二值信号量
                #define                    xSemaphoreGive (  xSemaphore  )      
                      xQueueGenericSend( ( QueueHandle_t ) ( xSemaphore )  ,   NULL  ,   semGIVE_BLOCK_TIME  ,   queueSEND_TO_BACK )
                xSemaphore 为 二值信号量句柄
                #define   semGIVE_BLOCK_TIME                  ( ( TickType_t ) 0U )
                多次释放二值信号量不会卡死！（阻塞时间0）
                释放计数信号量，计数信号量将+1，达到计数信号量的最大值后，信号量保持在最大值(同样不会卡死)；
                */
                xSemaphoreGive(count_semphore_handle);      /* 释放计数信号量 */ 
                if(err == pdPASS)     printf("计数信号量释放成功！！\r\n");
                 
                err = xSemaphoreGive(semphore_handle);      /* 释放二值信号量 */ 
                if(err == pdPASS)     printf("二值信号量释放成功！！\r\n");
                else                  printf("二值信号量释放失败！！\r\n");
                 
                /* 队列集释放/写入，直接对相应的队列/信号量操作即可 */
                err = xSemaphoreGive(xSemaphore);      /* 释放队列集中的二值信号量 */ 
                if(err == pdPASS)     printf("xSemaphore信号量释放成功！！\r\n");
                else                  printf("xSemaphore信号量释放失败！！\r\n");
                 
                err = xQueueSend( xQueue1, &count, portMAX_DELAY );   /* 释放队列集中的队列 */ 
                if(err == pdTRUE)   printf("xQueue1成功发送数据\r\n");        
                else                printf("xQueue1发送失败\r\n");       
                
                // 设置事件标志位API函数，将事件标志组的bit0位置1 
                event_bit = xEventGroupSetBits((EventGroupHandle_t  )EventGroupHandler, /* 待操作的事件标志组句柄 */
                                               (EventBits_t         )EVENTBIT_0);       /* 待设置的bit位 */
                printf("事件标志组的bit0位置1，此时事件标志组为%02X\r\n", event_bit);      
                break;
            }
            case KEY2_PRES:               
            {
                printf("\n///////// **************第%d次按下按键2(key2)\r\n", ++key2_count1); 
                /*
                #define     uxSemaphoreGetCount( xSemaphore )     uxQueueMessagesWaiting( ( QueueHandle_t ) ( xSemaphore ) )
                此函数用于获取信号量当前计数值大小，xSemaphore  信号量句柄
                返回值：整数，表示当前信号量的计数值大小
                获取计数信号量，信号量将-1；如果信号量为0，程序将会死等！
                */
                err = xSemaphoreTake(count_semphore_handle,  portMAX_DELAY);    /* 获取信号量并死等 */
                if(err == pdTRUE)     printf("计数信号量获取成功！计数信号量的计数值为：%d\r\n",(int)uxSemaphoreGetCount(count_semphore_handle));
                
                // 设置事件标志位API函数，将事件标志组的bit1位置1 
                event_bit = xEventGroupSetBits((EventGroupHandle_t  )EventGroupHandler, /* 待操作的事件标志组句柄 */
                                               (EventBits_t         )EVENTBIT_1);       /* 待设置的bit位 */
                printf("事件标志组的bit0位置1，此时事件标志组为%02X\r\n", event_bit);      
                break;
            }
            default:
            {
                break;
            }
        }
        
        // LED0_TOGGLE();                                                  /* LED0闪烁 */
        count++;
        vTaskDelay(10);
    }
}


/**
 * @brief       task1
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void task1(void *pvParameters)
{
    #if is_output_the_list 
    /* 1. 列表/列表项 */   
    printf("/*************1. 列表/列表项实验**************/\r\n");
    /* 
    列表是 FreeRTOS 中的一个数据结构，概念上和链表有点类似，列表被用来跟踪 FreeRTOS中的任务。 
    列表项就是存放在列表中的项目 
    列表相当于链表，列表项相当于节点，FreeRTOS 中的列表是一个双向环形链表 
    */

    /*
    列表：
    typedef struct xLIST
    {
        listFIRST_LIST_INTEGRITY_CHECK_VALUE                         校验值 
        volatile       UBaseType_t            uxNumberOfItems;       列表中的列表项数量 
        ListItem_t   *   configLIST_VOLATILE          pxIndex        用于遍历列表项的指针
        MiniListItem_t                        xListEnd               末尾列表项 
        listSECOND_LIST_INTEGRITY_CHECK_VALUE                        校验值 
    } List_t;
  
    // 列表项是列表中用于存放数据的地方
    struct xLIST_ITEM
    {
        listFIRST_LIST_ITEM_INTEGRITY_CHECK_VALUE                       用于检测列表项的数据完整性(校验值)
        configLIST_VOLATILE       TickType_t       xItemValue           列表项的值  
        struct xLIST_ITEM   *   configLIST_VOLATILE     pxNext          下一个列表项   
        struct xLIST_ITEM   *   configLIST_VOLATILE     pxPrevious      上一个列表项   
        void   *   pvOwner                                              列表项的拥有者  
        struct xLIST   *   configLIST_VOLATILE     pxContainer;         列表项所在列表  
        listSECOND_LIST_ITEM_INTEGRITY_CHECK_VALUE                      用于检测列表项的数据完整性  
    };
    typedef struct     xLIST_ITEM     ListItem_t;   
    
    // 迷你列表项也是列表项，但迷你列表项仅用于标记列表的末尾和挂载其他插入列表中的列表项 
    struct xMINI_LIST_ITEM
    {
        listFIRST_LIST_ITEM_INTEGRITY_CHECK_VALUE                        用于检测数据完整性(校验值)
        configLIST_VOLATILE       TickType_t       xItemValue;           列表项的值  
        struct xLIST_ITEM   *   configLIST_VOLATILE     pxNext;          上一个列表项 
        struct xLIST_ITEM   *   configLIST_VOLATILE     pxPrevious;      下一个列表项 
    };
    typedef struct     xMINI_LIST_ITEM     MiniListItem_t;   
    
    | **函数**                                                      | **描述**                                                     |
    | ------------------------------------------------------------  | ------------------------------------------------------------ |
    | void vListInitialise (List_t * const pxList)                  | 初始化列表                                                    |
    |                                                                 初始化时，列表中只有 xListEnd，因此 pxIndex 指向 xListEnd       |
    |                                                                 初始化时，列表中只有 xListEnd，因此上一个和下一个列表项都为 xListEnd 本身 |
    | void vListInitialiseItem ( ListItem_t * const pxItem )        | 初始化列表项                                                 |
    | void vListInsertEnd ( List_t * const pxList ,  ListItem_t * const pxNewListItem )    |   将待插入列表的列表项插入到列表 pxIndex 指针指向的列表项前面，是一种无序的插入方法   |
    | void vListInsert ( List_t * const pxList ,  ListItem_t * const pxNewListItem )       |   将待插入列表的列表项按照列表项值升序进行排序，有序地插入到列表中    |
    | UBaseType_t uxListRemove( ListItem_t * const pxItemToRemove ) | 列表移除列表项 ， 返回值为  列表剩余列表项的数量         |
    
    */
  
    /* 第一步：初始化列表和列表项 */
    vListInitialise(&TestList);                 /* 初始化列表 */
    vListInitialiseItem(&ListItem1);            /* 初始化列表项1 */
    vListInitialiseItem(&ListItem2);            /* 初始化列表项2 */
    vListInitialiseItem(&ListItem3);            /* 初始化列表项3 */
    ListItem1.xItemValue = 40;
    ListItem2.xItemValue = 60;
    ListItem3.xItemValue = 50;
    
    /* 第二步：打印列表和其他列表项的地址 */
    printf("/**************第二步：打印列表和列表项的地址**************/\r\n");
    printf("此时列表顺序：pxIndex->xListEnd，xListEnd -> xListEnd  \r\n");
    
    printf("项目\t\t\t地址\r\n");
    printf("TestList\t\t0x%p\t\r\n", &TestList);
    printf("TestList->pxIndex\t0x%p\t\r\n", TestList.pxIndex);
    printf("TestList->xListEnd\t0x%p\t\r\n", (&TestList.xListEnd));
    printf("ListItem1\t\t0x%p\t\r\n", &ListItem1);
    printf("ListItem2\t\t0x%p\t\r\n", &ListItem2);
    printf("ListItem3\t\t0x%p\t\r\n", &ListItem3);
    printf("/**************************结束***************************/\r\n\r\n");
    vTaskDelay(10);
    
    /* 第三步：列表项1插入列表 */
    printf("/*****************第三步：列表项1插入列表******************/\r\n");
    vListInsert((List_t*    )&TestList,         /* 列表 */
                (ListItem_t*)&ListItem1);       /* 列表项 */
    printf("此时列表顺序：xListEnd -> ListItem1 -> xListEnd  \r\n");
    
    printf("项目\t\t\t\t地址\r\n");
    printf("TestList->xListEnd->pxNext\t0x%p\r\n", (TestList.xListEnd.pxNext));
    printf("ListItem1->pxNext\t\t0x%p\r\n", (ListItem1.pxNext));
    printf("TestList->xListEnd->pxPrevious\t0x%p\r\n", (TestList.xListEnd.pxPrevious));
    printf("ListItem1->pxPrevious\t\t0x%p\r\n", (ListItem1.pxPrevious));
    printf("/**************************结束***************************/\r\n\r\n");
    vTaskDelay(10);
    
    /* 第四步：列表项2插入列表 */
    printf("/*****************第四步：列表项2插入列表******************/\r\n");
    vListInsert((List_t*    )&TestList,         /* 列表 */
                (ListItem_t*)&ListItem2);       /* 列表项 */
    printf("此时列表顺序：xListEnd -> ListItem1 -> ListItem2 -> xListEnd  \r\n");
    
    printf("项目\t\t\t\t地址\r\n");
    printf("TestList->xListEnd->pxNext\t0x%p\r\n", (TestList.xListEnd.pxNext));
    printf("ListItem1->pxNext\t\t0x%p\r\n", (ListItem1.pxNext));
    printf("ListItem2->pxNext\t\t0x%p\r\n", (ListItem2.pxNext));
    printf("TestList->xListEnd->pxPrevious\t0x%p\r\n", (TestList.xListEnd.pxPrevious));
    printf("ListItem1->pxPrevious\t\t0x%p\r\n", (ListItem1.pxPrevious));
    printf("ListItem2->pxPrevious\t\t0x%p\r\n", (ListItem2.pxPrevious));
    printf("/**************************结束***************************/\r\n\r\n");
    vTaskDelay(10);
    
    /* 第五步：列表项3插入列表 */
    printf("/*****************第五步：列表项3插入列表******************/\r\n");
    vListInsert((List_t*    )&TestList,         /* 列表 */
                (ListItem_t*)&ListItem3);       /* 列表项 */
    printf("此时列表顺序：xListEnd -> ListItem1 -> ListItem3 -> ListItem2 -> xListEnd  \r\n");
    
    printf("项目\t\t\t\t地址\r\n");
    printf("TestList->xListEnd->pxNext\t0x%p\r\n", (TestList.xListEnd.pxNext));
    printf("ListItem1->pxNext\t\t0x%p\r\n", (ListItem1.pxNext));
    printf("ListItem2->pxNext\t\t0x%p\r\n", (ListItem2.pxNext));
    printf("ListItem3->pxNext\t\t0x%p\r\n", (ListItem3.pxNext));
    printf("TestList->xListEnd->pxPrevious\t0x%p\r\n", (TestList.xListEnd.pxPrevious));
    printf("ListItem1->pxPrevious\t\t0x%p\r\n", (ListItem1.pxPrevious));
    printf("ListItem2->pxPrevious\t\t0x%p\r\n", (ListItem2.pxPrevious));
    printf("ListItem3->pxPrevious\t\t0x%p\r\n", (ListItem3.pxPrevious));
    printf("/**************************结束***************************/\r\n\r\n");
    vTaskDelay(10);
    
    /* 第六步：移除列表项2 */
    printf("/*******************第六步：移除列表项2********************/\r\n");
    uxListRemove((ListItem_t*   )&ListItem2);   /* 移除列表项 */
    printf("此时列表顺序：xListEnd -> ListItem1 -> ListItem3 -> xListEnd \r\n");
    
    printf("项目\t\t\t\t地址\r\n");
    printf("TestList->xListEnd->pxNext\t0x%p\r\n", (TestList.xListEnd.pxNext));
    printf("ListItem1->pxNext\t\t0x%p\r\n", (ListItem1.pxNext));
    printf("ListItem3->pxNext\t\t0x%p\r\n", (ListItem3.pxNext));
    printf("TestList->xListEnd->pxPrevious\t0x%p\r\n", (TestList.xListEnd.pxPrevious));
    printf("ListItem1->pxPrevious\t\t0x%p\r\n", (ListItem1.pxPrevious));
    printf("ListItem3->pxPrevious\t\t0x%p\r\n", (ListItem3.pxPrevious));
    printf("/**************************结束***************************/\r\n\r\n");
    vTaskDelay(10);
    
    /* 第七步：列表末尾添加列表项2 */
    printf("/****************第七步：列表末尾添加列表项2****************/\r\n");
    vListInsertEnd((List_t*     )&TestList,     /* 列表 */
                   (ListItem_t* )&ListItem2);   /* 列表项 */
    printf("此时列表顺序：pxIndex->xListEnd，xListEnd -> ListItem1 -> ListItem3 -> ListItem2 -> xListEnd \r\n");
    printf("项目\t\t\t\t地址\r\n");
    
    printf("TestList->pxIndex\t\t0x%p\r\n", TestList.pxIndex);
    printf("TestList->xListEnd->pxNext\t0x%p\r\n", (TestList.xListEnd.pxNext));
    printf("ListItem1->pxNext\t\t0x%p\r\n", (ListItem1.pxNext));
    printf("ListItem2->pxNext\t\t0x%p\r\n", (ListItem2.pxNext));
    printf("ListItem3->pxNext\t\t0x%p\r\n", (ListItem3.pxNext));
    printf("TestList->xListEnd->pxPrevious\t0x%p\r\n", (TestList.xListEnd.pxPrevious));
    printf("ListItem1->pxPrevious\t\t0x%p\r\n", (ListItem1.pxPrevious));
    printf("ListItem2->pxPrevious\t\t0x%p\r\n", (ListItem2.pxPrevious));
    printf("ListItem3->pxPrevious\t\t0x%p\r\n", (ListItem3.pxPrevious));
    printf("/************************实验结束***************************/\r\n\r\n");
    vTaskDelay(10);
    
    while (key_scan(0) != KEY1_PRES)
    {
        vTaskDelay(5);
    }
    
    /**************************************************************************************************************/
    
    /* 2. FreeRTOS任务相关API函数 */   
    printf("/*************2. FreeRTOS任务相关API函数**************/\r\n");
    
    uint32_t        i               = 0;
    UBaseType_t     task_num        = 0;
    TaskStatus_t    *status_array   = NULL;
    TaskHandle_t    task_handle     = NULL;
    TaskStatus_t    *task_info      = NULL;
    eTaskState      task_state      = eInvalid;
    char            *task_state_str = NULL;
    char            *task_info_buf  = NULL;
    UBaseType_t     task_stack_min  = 0;    
    
    
    /* 第一步：函数uxTaskGetSystemState()的使用 */
    printf("/********第一步：函数uxTaskGetSystemState()的使用**********/\r\n");

    /* UBaseType_t uxTaskGetNumberOfTasks( void ) */
    /* 此函数用于获取系统中任务的任务数量 ， 返回值 系统中任务的数量 */
    task_num = uxTaskGetNumberOfTasks();                            /* 获取系统任务数量 */
    
    status_array = malloc(task_num * sizeof(TaskStatus_t));
    
    /*
    UBaseType_t  uxTaskGetSystemState(   TaskStatus_t * const pxTaskStatusArray,
                                         const UBaseType_t uxArraySize,
                                         configRUN_TIME_COUNTER_TYPE * const pulTotalRunTime  )
    | xTaskStatusArray | 指向TaskStatus_t 结构体数组首地址         |
    | uxArraySize      | 接收信息的数组大小                        |
    | pulTotalRunTime  | 系统总运行时间，为NULL 则省略总运行时间值 |       
    此函数用于获取系统中所有任务的任务状态信息，使用该函数需将宏 configUSE_TRACE_FACILITY 置 1    
    */
    task_num = uxTaskGetSystemState((TaskStatus_t* )status_array,   /* 任务状态信息buffer */
                                    (UBaseType_t   )task_num,       /* buffer大小 */
                                    (uint32_t*     )NULL);          /* 不获取任务运行时间信息 */
                                    
    printf("任务名\t\t优先级\t\t任务编号\r\n");
    for (i=0; i<task_num; i++)
    {
        printf("%s\t%s%ld\t\t%ld\r\n",
                status_array[i].pcTaskName,
                strlen(status_array[i].pcTaskName) > 7 ? "": "\t",
                status_array[i].uxCurrentPriority,
                status_array[i].xTaskNumber);
    }
    
    free(status_array);

    printf("/**************************结束***************************/\r\n");
    vTaskDelay(10);
    
    
    /* 第二步：函数vTaskGetInfo()的使用 */
    printf("/************第二步：函数vTaskGetInfo()的使用**************/\r\n");
    task_info = malloc(sizeof(TaskStatus_t));
    
    /* TaskHandle_t xTaskGetHandle(const char * pcNameToQuery);  */
    /* 此函数用于通过任务名获取任务句柄 ， 使用该函数需将宏 INCLUDE_xTaskGetHandle 置 1 */
    /* 形参  pcNameToQuery  任务名  ，  返回值  TaskHandle  任务句柄  */
    task_handle = xTaskGetHandle("task1");                          /* 获取任务句柄 */
    vTaskGetInfo((TaskHandle_t  )task_handle,                       /* 任务句柄 */
                 (TaskStatus_t* )task_info,                         /* 任务信息buffer */
                 (BaseType_t    )pdTRUE,                            /* 允许统计任务堆栈历史最小值 */
                 (eTaskState    )eInvalid);                         /* 获取任务运行状态 */
    /*
    void vTaskGetInfo(  TaskHandle_t  xTask,
                        TaskStatus_t *  pxTaskStatus,
                        BaseType_t  xGetFreeStackSpace,
                        eTaskState  eState  )  
    此函数用于获取指定的单个任务的状态信息，使用该函数需将宏 configUSE_TRACE_FACILITY  置 1 
    | xTask              | 指定获取信息的任务的句柄                                     |
    | pxTaskStatus       | 接收任务信息的变量                                           |
    | xGetFreeStackSpace | 任务栈历史剩余最小值，  当为“pdFALSE” 则跳过这个步骤，  当为“pdTRUE”则检查历史剩余最小堆栈 |
    | eState             | 任务状态，可直接赋值，如想获取代入“eInvalid”                 |
    typedef enum
    {   
      eRunning = 0,    运行态   
      eReady,          就绪态  
      eBlocked,        阻塞态  
      eSuspended,      起态  
      eDeleted,        任务被删除  
      eInvalid,        无效   
    } eTaskState;
    */
    printf("任务名:\t\t\t%s\r\n", task_info->pcTaskName);
    printf("任务编号:\t\t\t%ld\r\n", task_info->xTaskNumber);
    printf("任务壮态:\t\t\t%d\r\n", task_info->eCurrentState);
    printf("任务当前优先级:\t\t%ld\r\n", task_info->uxCurrentPriority);
    printf("任务基优先级:\t\t%ld\r\n", task_info->uxBasePriority);
    printf("任务堆栈基地址:\t\t0x%p\r\n", task_info->pxStackBase);
    printf("任务堆栈历史剩余最小值:\t\t%d\r\n", task_info->usStackHighWaterMark);
    free(task_info);
    
    printf("/**************************结束***************************/\r\n");
    vTaskDelay(10);
    
    
    /* 第三步：函数eTaskGetState()的使用  */
    printf("/***********第三步：函数eTaskGetState()的使用*************/\r\n");  
    
    task_state_str = malloc(10);
    
    /* TaskHandle_t xTaskGetHandle(const char * pcNameToQuery);  */
    /* 此函数用于通过任务名获取任务句柄 ， 使用该函数需将宏 INCLUDE_xTaskGetHandle 置 1 */
    /* 形参  pcNameToQuery  任务名  ，  返回值  TaskHandle  任务句柄  */
    task_handle = xTaskGetHandle("task1");
    
    /* eTaskState    eTaskGetState(TaskHandle_t xTask);  */
    /* 此函数用于查询某个任务的运行状态，使用此函数需将宏 INCLUDE_eTaskGetState 置1  */
    /* 形参  xTask  待获取状态任务的任务句柄  ，  返回值  eTaskState  任务状态  */
    /*
    typedef enum
    {   
      eRunning = 0,    运行态   
      eReady,          就绪态  
      eBlocked,        阻塞态  
      eSuspended,      起态  
      eDeleted,        任务被删除  
      eInvalid,        无效   
    } eTaskState;
    */
    task_state = eTaskGetState(task_handle);                        /* 获取任务运行状态 */
    sprintf(task_state_str, task_state == eRunning ? "Runing" :
                            task_state == eReady ? "Ready" :
                            task_state == eBlocked ? "Blocked" :
                            task_state == eSuspended ? "Suspended" :
                            task_state == eDeleted ? "Deleted" :
                            task_state == eInvalid ? "Invalid" :
                                                     "");
    printf("任务状态值: %d，对应状态为: %s\r\n", task_state, task_state_str);
    free(task_state_str);
    
    printf("/**************************结束***************************/\r\n");
    vTaskDelay(10);
    
    
    /* 第四步：函数vTaskList()的使用 */
    printf("/*************第四步：函数vTaskList()的使用*************/\r\n");
    task_info_buf = malloc(250);       
    // 注意一下，这里不要超过启动文件中，定义堆的大小
    // Stack_Size      EQU     0x00000400
    // Heap_Size       EQU     0x00000200    
    /*
    void   vTaskList(char * pcWriteBuffer)
    此函数用于以“表格”的形式获取系统中任务的信息 ；使用此函数需将宏 configUSE_TRACE_FACILITY 和configUSE_STATS_FORMATTING_FUNCTIONS 置1 
    形参：pcWriteBuffer  接收任务信息的缓存指针
    Name        ： 创建任务的时候给任务分配的名字。
    State       ： 任务的壮态信息， B 是阻塞态， R 是就绪态， S 是挂起态， D 是删除态
    Priority    ： 任务优先级。
    Stack       ： 任务堆栈的“高水位线”，就是堆栈历史最小剩余大小。
    Num         ： 任务编号，这个编号是唯一的，当多个任务使用同一个任务名的时候可以通过此编号来做区分。 
    */
    vTaskList(task_info_buf);                                       /* 获取所有任务的信息 */ 
    printf("任务名\t状态\t优先级\t剩余栈\t任务序号\r\n");
    printf("%s\r\n", task_info_buf);
    
    free(task_info_buf);
    
    /* TaskHandle_t xTaskGetHandle(const char * pcNameToQuery);  */
    /* 此函数用于通过任务名获取任务句柄 ， 使用该函数需将宏 INCLUDE_xTaskGetHandle 置 1 */
    /* 形参  pcNameToQuery  任务名  ，  返回值  TaskHandle  任务句柄  */
    task_handle = xTaskGetHandle("task1");
    /* 获取任务的任务栈历史剩余最小值 */
    task_stack_min = uxTaskGetStackHighWaterMark( task_handle );
    printf("task1历史剩余最小堆栈为%ld\r\n",task_stack_min);
    
    printf("/************************实验结束***************************/\r\n");
    
    while (1)
    {
        char    *runtime_info   = NULL;
        runtime_info = malloc(100);
      
        /*
        Void    vTaskGetRunTimeStats( char * pcWriteBuffer ) 
        此函数用于统计任务的运行时间信息，
        形参  pcWriteBuffer  接收任务运行时间信息的缓存指针
      
        使用此函数需将宏 configGENERATE_RUN_TIME_STAT 、configUSE_STATS_FORMATTING_FUNCTIONS 置1 
        还需要实现2个宏定义：
        portCONFIGURE_TIMER_FOR_RUNTIME_STATE() ：用于初始化用于配置任务运行时间统计的时基定时器；注意：这个时基定时器的计时精度需高于系统时钟节拍精度的10至100倍！
        portGET_RUN_TIME_COUNTER_VALUE()：用于获取该功能时基硬件定时器计数的计数值 。

        Task：任务名称     Abs Time：任务实际运行的总时间（绝对时间）     % Time：占总处理时间的百分比
        */
        vTaskGetRunTimeStats(runtime_info);             /* 获取任务运行时间信息 */
      
        printf("任务名\t运行时间\t运行所占百分比\r\n");
        printf("%s\r\n", runtime_info);
      
        free(runtime_info);
        
        //--------------内存管理测试 
        LED1_TOGGLE();

        uint8_t * buf = NULL;
        buf = pvPortMalloc(30);             /* 申请内存 */
        if(buf != NULL)   printf("申请内存成功！\r\n");
        
        printf("剩余的空闲内存大小为：%d\r\n", xPortGetFreeHeapSize());
      
        vPortFree(buf);                     /* 释放内存 */
        printf("释放内存！！\r\n");
        
        printf("剩余的空闲内存大小为：%d\r\n", xPortGetFreeHeapSize());
        //--------------  
          
        for(uint8_t i=0; i<30; i++)
            vTaskDelay(1000);
    }
    

    #else

    EventBits_t event_bit = 0;
    while (1)
    {   
//        LED1_TOGGLE();

        // 返回值：返回事件组中的事件标志位
        event_bit = xEventGroupWaitBits((EventGroupHandle_t )EventGroupHandler,              /* 等待的事件标志组句柄 */
                ( EventBits_t  )EVENTBIT_0 | EVENTBIT_1,        /* 等待的事件标志位，可以用逻辑或等待多个事件标志位 */
                ( BaseType_t   )pdTRUE,                         /* 成功等待到事件标志位后，清除事件组中对应的事件标志位，pdTRUE：清除uxBitsToWaitFor指定位；pdFALSE：不清除 */
                ( BaseType_t   )pdTRUE,                         /* 等待 uxBitsToWaitFor 中的所有事件标志位，pdTRUE：等待的位，全部为1；pdFALSE：等待的位，某个为1 */
                ( TickType_t   )portMAX_DELAY);                 /* 等待时间，portMAX_DELAY表示死等 */
      
        printf("事件标志组中的标志位为%02X\r\n", event_bit);
        vTaskDelay(10);
    } 
    
    #endif
}


/*
#define is_output_the_list 0       
if    is_output_the_list==1    则输出各种列表状态信息
if    is_output_the_list==0    不输出相关信息
is_output_the_list==0   实验
按2下 按键1，再按2下 按键2;
按2下 按键1，再按3下 按键2;
输出信息：
---------------------------------------------------------------------------
key_queue队列创建成功！！
big_date_queue队列创建成功！！
二值信号量创建成功！！！
计数型信号量创建成功！！！
xQueue1队列创建成功！！
队列集xSemaphore信号量创建成功！！！
队列集创建成功！！！
事件标志组创建成功！！！

///////// **************第1次按下按键1(key1)
big_date_queue111成功发送数据
key_queue读取队列成功，数据：248
key_queue111成功发送数据
计数信号量释放成功！！
二值获取信号量成功
big_date_queue读取队列成功，数据：我是一个大数组，大大的数组！
二值信号量释放成功！！
获取到二值信号量: xSemaphore
xSemaphore信号量释放成功！！
接收到来自xQueue1的消息: 248
xQueue1成功发送数据
事件标志组的bit0位置1，此时事件标志组为01

///////// **************第2次按下按键1(key1)
big_date_queue111成功发送数据
key_queue读取队列成功，数据：208
key_queue111成功发送数据
计数信号量释放成功！！
二值获取信号量成功
big_date_queue读取队列成功，数据：我是一个大数组，大大的数组！
二值信号量释放成功！！
获取到二值信号量: xSemaphore
xSemaphore信号量释放成功！！
接收到来自xQueue1的消息: 208
xQueue1成功发送数据
事件标志组的bit0位置1，此时事件标志组为01

///////// **************第1次按下按键2(key2)
计数信号量获取成功！计数信号量的计数值为：1
事件标志组中的标志位为03
事件标志组的bit0位置1，此时事件标志组为00

///////// **************第2次按下按键2(key2)
计数信号量获取成功！计数信号量的计数值为：0
事件标志组的bit0位置1，此时事件标志组为02

///////// **************第3次按下按键1(key1)
big_date_queue111成功发送数据
key_queue读取队列成功，数据：113
key_queue111成功发送数据
计数信号量释放成功！！
二值获取信号量成功
big_date_queue读取队列成功，数据：我是一个大数组，大大的数组！
二值信号量释放成功！！
获取到二值信号量: xSemaphore
xSemaphore信号量释放成功！！
接收到来自xQueue1的消息: 113
xQueue1成功发送数据
事件标志组中的标志位为03
事件标志组的bit0位置1，此时事件标志组为00

///////// **************第4次按下按键1(key1)
big_date_queue111成功发送数据
key_queue读取队列成功，数据：22
key_queue111成功发送数据
计数信号量释放成功！！
二值获取信号量成功
big_date_queue读取队列成功，数据：我是一个大数组，大大的数组！
二值信号量释放成功！！
获取到二值信号量: xSemaphore
xSemaphore信号量释放成功！！
接收到来自xQueue1的消息: 22
xQueue1成功发送数据
事件标志组的bit0位置1，此时事件标志组为01

///////// **************第3次按下按键2(key2)
计数信号量获取成功！计数信号量的计数值为：1
事件标志组中的标志位为03
事件标志组的bit0位置1，此时事件标志组为00

///////// **************第4次按下按键2(key2)
计数信号量获取成功！计数信号量的计数值为：0
事件标志组的bit0位置1，此时事件标志组为02

///////// **************第5次按下按键2(key2)


！！！之后系统卡死！


*/

