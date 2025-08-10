/**
  ******************************************************************************
  * @file    bsp_debug_usart.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   can�������ػ�ģʽ��
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F103-�Ե� ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 

#include "./can/bsp_can.h"

CAN_HandleTypeDef Can_Handle; /* CANx��� */
CanTxMsgTypeDef TxMessage;		/* ���Ͳ������ */
CanRxMsgTypeDef RxMessage;		/* ���ղ������ */

extern __IO uint32_t flag;		 //���ڱ�־�Ƿ���յ����ݣ����жϺ����и�ֵ

/*
 * ��������CAN_GPIO_Config
 * ����  ��CAN��GPIO ����
 * ����  ����
 * ���  ����
 * ����  ���ڲ�����
 */
static void CAN_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* ʹ������ʱ�� */
	CAN_TX_GPIO_CLK_ENABLE();
	CAN_RX_GPIO_CLK_ENABLE();	
	
	__HAL_RCC_AFIO_CLK_ENABLE();

	/* ����CAN�������� */
	GPIO_InitStructure.Pin = CAN_TX_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Speed =GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStructure.Pull  = GPIO_PULLUP;

	HAL_GPIO_Init(CAN_TX_GPIO_PORT, &GPIO_InitStructure);

	/* ����CAN�������� */
	GPIO_InitStructure.Pin = CAN_RX_PIN ;
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	HAL_GPIO_Init(CAN_RX_GPIO_PORT, &GPIO_InitStructure);
	
	__HAL_AFIO_REMAP_CAN1_2();
}

/*
 * ��������CAN_NVIC_Config
 * ����  ��CAN��NVIC ����,��1���ȼ��飬0��0���ȼ�
 * ����  ����
 * ���  ����
 * ����  ���ڲ�����
 */
static void CAN_NVIC_Config(void)
{
   /* ������ռ���ȼ��ķ��� */
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_1);
	/*�ж����ã���ռ���ȼ�0�������ȼ�Ϊ0*/
	HAL_NVIC_SetPriority(CAN_RX_IRQ, 0 ,0);
	HAL_NVIC_EnableIRQ(CAN_RX_IRQ);
  
//  /* ʹ���жϽ��� */
//  __HAL_CAN_ENABLE_IT(&Can_Handle, CAN_FIFO0); /* FIFO0��Ϣ�Һ��ж����� */
  // �����Ǿ������ HAL_CAN_Receive_IT(&Can_Handle, CAN_FIFO0); �У�������
}

/*
 * ��������CAN_Mode_Config
 * ����  ��CAN��ģʽ ����
 * ����  ����
 * ���  ����
 * ����  ���ڲ�����
 */
/**
 * @brief       CAN��ʼ��������ԭ�� ע�ͣ�
 * @param       tsjw    : ����ͬ����Ծʱ�䵥Ԫ.��Χ: 1~3;
 * @param       tbs2    : ʱ���2��ʱ�䵥Ԫ.��Χ: 1~8;
 * @param       tbs1    : ʱ���1��ʱ�䵥Ԫ.��Χ: 1~16;
 * @param       brp     : �����ʷ�Ƶ��.��Χ: 1~1024;
 *   @note      ����4������, �ں����ڲ����1, ����, �κ�һ�����������ܵ���0
 *              CAN����APB1����, ������ʱ��Ƶ��Ϊ Fpclk1 = PCLK1 = 36Mhz
 *              tq     = brp * tpclk1;
 *              ������ = Fpclk1 / ((tbs1 + tbs2 + 1) * brp);
 *              �������� can_init(1, 8, 9, 4, 1), ��CAN������Ϊ:
 *              36M / ((8 + 9 + 1) * 4) = 500Kbps
 *
 * @param       mode    : CAN_MODE_NORMAL,  ��ͨģʽ;
                          CAN_MODE_LOOPBACK,�ػ�ģʽ;
 * @retval      0,  ��ʼ���ɹ�; ����, ��ʼ��ʧ��;
 */
static void CAN_Mode_Config(void)
{
	/************************CANͨ�Ų�������**********************************/
	/* ʹ��CANʱ�� */
	CAN_CLK_ENABLE();

	Can_Handle.Instance = CANx;
	Can_Handle.pTxMsg = &TxMessage;
	Can_Handle.pRxMsg = &RxMessage;
	
	/* CAN���Ƶ�Ԫ��ʼ�� */
	
	/* TTCM ʱ�䴥��ģʽ��
		 �ڴ�ģʽ�£�CAN ʹ�����ڲ���ʱ������ʱ����������������� CAN_RDTxR��CAN_TDTxR �Ĵ����С�
	   �ڲ���ʱ����ÿ�� CAN λʱ���ۼӣ��ڽ��պͷ��͵�֡��ʼλ��������������ʱ�����*/
	Can_Handle.Init.TTCM=DISABLE;			   //MCR-TTCM  �ر�ʱ�䴥��ͨ��ģʽʹ��
	
	/* ABOM �Զ����߹���
	   ���ڵ��⵽�����ʹ������մ��󳬹�һ��ֵʱ�����Զ���������״̬��������״̬�У�CAN ���ܽ��ջ��ͱ��ġ�
		 ��������״̬��ʱ�򣬿���������ƻָ�����ֱ��ʹ������Զ����߹����ܣ��������ʵ���ʱ���Զ��ָ���*/
	Can_Handle.Init.ABOM=ENABLE;			   //MCR-ABOM  �Զ����߹��� 
	
	/* AWUM �Զ����ѣ�
	   CAN �������ʹ���������͹��ĵ�˯��ģʽ��
		 ���ʹ��������Զ����ѹ��ܣ��� CAN ��⵽���߻��ʱ�򣬻��Զ����ѡ�*/	
	Can_Handle.Init.AWUM=ENABLE;			   //MCR-AWUM  ʹ���Զ�����ģʽ
	
	/* NART �Զ��ش�
	   ����������ܺ󣬵����ķ���ʧ��ʱ���Զ��ش����ɹ�Ϊֹ��
		 ����ʹ��������ܣ����۷��ͽ����Σ���Ϣֻ����һ��*/		
	Can_Handle.Init.NART=DISABLE;			   //MCR-NART  ��ֹ�����Զ��ش�	  DISABLE-�Զ��ش�
	
	/* RFLM ����ģʽ
	   �����󣬵����� FIFO ���ʱ���ᶪ����һ�����յı��ġ�
		 ��������������һ�����յ��ı��ĻḲ��ԭ���ġ�*/			
	Can_Handle.Init.RFLM=DISABLE;			   //MCR-RFLM  ����FIFO ����ģʽ  DISABLE-���ʱ�±��ĻḲ��ԭ�б���  
	
	/* TXFP ���ķ������ȼ����ж�����
	   �� CAN ����ķ����������ж�������ͱ���ʱ��
		 �����ܿ��Կ������Ǹ��ݱ��ĵ� ID ���ȼ����Ǳ��Ĵ�������˳�������͡�*/			
	Can_Handle.Init.TXFP=DISABLE;			   //MCR-TXFP  ����FIFO���ȼ� DISABLE-���ȼ�ȡ���ڱ��ı�ʾ�� 
	
	/*
	#define CAN_MODE_NORMAL              // Normal mode   ����ģʽ
	#define CAN_MODE_LOOPBACK            // Loopback mode �ػ�ģʽ�������ⷢ�Ͳ�������գ����Իػ���
	#define CAN_MODE_SILENT              // Silent mode   ��Ĭģʽ�������ⷢ�Ϳ�������գ����Իػ���
	#define CAN_MODE_SILENT_LOOPBACK     // Loopback combined with silent mode  ��Ĭ�ػ�ģʽ����֧���Իػ���
	*/
	Can_Handle.Init.Mode = CAN_MODE_LOOPBACK;  //�ػ�ģʽ
	
	Can_Handle.Init.SJW=CAN_SJW_2TQ;		   //BTR-SJW ����ͬ����Ծ��� 2��ʱ�䵥Ԫ

	/* ss=1 bs1=5 bs2=3 λʱ����Ϊ(1+5+3) �����ʼ�Ϊʱ������tq*(1+3+5)  */
	Can_Handle.Init.BS1=CAN_BS1_5TQ;		   //BTR-TS1 ʱ���1 ռ����5��ʱ�䵥Ԫ
	Can_Handle.Init.BS2=CAN_BS2_3TQ;		   //BTR-TS1 ʱ���2 ռ����3��ʱ�䵥Ԫ	

	/* CAN Baudrate = 1 MBps (1MBps��Ϊstm32��CAN�������) (CAN ʱ��Ƶ��Ϊ APB 1 = 36 MHz) */
	// BTR-BRP �����ʷ�Ƶ��  ������ʱ�䵥Ԫ��ʱ�䳤�� 36/(1+5+3)/4=1 Mbps
	// CAN����APB1���ߣ�CLK = 36MHz��4��Ƶ����ʱ��Ϊ9Mhz��ʱ����9Tq��������1 Mbps
	Can_Handle.Init.Prescaler =4;		   
	
	HAL_CAN_Init(&Can_Handle);
}

/*
 * ��������CAN_Filter_Config
 * ����  ��CAN�Ĺ����� ����
 * ����  ����
 * ���  ����
 * ����  ���ڲ�����
 */
static void CAN_Filter_Config(void)
{
	CAN_FilterConfTypeDef  CAN_FilterInitStructure;

	/* CANɸѡ����ʼ�� */
	// STM32F103 ϵ��оƬ���� 14 ��ɸѡ���飺0-13 ��
	CAN_FilterInitStructure.FilterNumber=0;						//ɸѡ����0
	CAN_FilterInitStructure.FilterMode=CAN_FILTERMODE_IDMASK; 	//����������ģʽ
	
	/* ʹ��ɸѡ�������ձ�־�����ݽ��бȶ�ɸѡ����չID�������µľ����������ǵĻ��������FIFO0�� */
	/* ����32λ����� EXTID[28:0]�� IDE �� RTR λ��һ�� 31 λ */
	/* ����16λ����� STDID[10:0]�� RTR�� IDE �� EXTID[17:15]��һ�� 16 λ */
	CAN_FilterInitStructure.FilterScale=CAN_FILTERSCALE_32BIT;	//ɸѡ��λ��Ϊ����32λ��
	
	CAN_FilterInitStructure.FilterIdHigh= ((((uint32_t)0x1314<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF0000)>>16;		//Ҫɸѡ��ID��λ 
	CAN_FilterInitStructure.FilterIdLow=   (((uint32_t)0x1314<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF;  //Ҫɸѡ��ID��λ
	/* 0x1314����3λ, ��ʾ��չID | RTR λ(��������֡��ң��֡) | IDE λ(���ֱ�׼��ʽ����չ��ʽ)  */
	/* CAN_RTR_DATA(����֡) |  CAN_ID_EXT(��չ֡��ʽ) */
	
	CAN_FilterInitStructure.FilterMaskIdHigh= 0xFFFF;			//ɸѡ����16λÿλ����ƥ��
	CAN_FilterInitStructure.FilterMaskIdLow=  0xFFFF;			//ɸѡ����16λÿλ����ƥ��
	
	CAN_FilterInitStructure.FilterFIFOAssignment=CAN_FILTER_FIFO0 ;	//ɸѡ����������FIFO0
	CAN_FilterInitStructure.FilterActivation=ENABLE;			//ʹ��ɸѡ��
	HAL_CAN_ConfigFilter(&Can_Handle,&CAN_FilterInitStructure);
}

/*
 * ��������CAN_Config
 * ����  ����������CAN�Ĺ���
 * ����  ����
 * ���  ����
 * ����  ���ⲿ����
 */
void CAN_Config(void)
{
  CAN_GPIO_Config();
  CAN_NVIC_Config();
  CAN_Mode_Config();
  CAN_Filter_Config();
  Init_RxMes(); 
  HAL_CAN_Receive_IT(&Can_Handle, CAN_FIFO0); 	// ʹ��CAN�����ж�
}


/**
  * @brief  ��ʼ�� Rx Message���ݽṹ��
  * @param  RxMessage: ָ��Ҫ��ʼ�������ݽṹ��
  * @retval None
  */
void Init_RxMes(void)
{
  uint8_t ubCounter = 0;

  /*�ѽ��սṹ������*/
  Can_Handle.pRxMsg->StdId = 0x00;
  Can_Handle.pRxMsg->ExtId = 0x00;
  Can_Handle.pRxMsg->IDE = CAN_ID_STD;
  Can_Handle.pRxMsg->DLC = 0;
	// FMI ָ�������д洢���ʼ��������Ĺ�������������
	// ����0 ��ʾʹ�� ɸѡ����0
  Can_Handle.pRxMsg->FMI = 0;  
	// ָ������ FIFO ���
  // Can_Handle.pRxMsg->FIFONumber = CAN_FIFO0;  	
  for (ubCounter = 0; ubCounter < 8; ubCounter++)
  {
    Can_Handle.pRxMsg->Data[ubCounter] = 0x00;
  }
}


/*
 * ��������CAN_SetMsg
 * ����  ��CANͨ�ű�����������,����һ����������Ϊ0-7�����ݰ�
 * ����  �����ͱ��Ľṹ��
 * ���  ����
 * ����  ���ⲿ����
 */	 
void CAN_SetMsg(void)
{	  
  uint8_t ubCounter = 0;
  Can_Handle.pTxMsg->StdId=0x00;						   //�����ʼ����׼IDû��
  Can_Handle.pTxMsg->ExtId=0x1314;				  	 //ʹ�õ���չID
  Can_Handle.pTxMsg->IDE=CAN_ID_EXT;				   //��չģʽ
  Can_Handle.pTxMsg->RTR=CAN_RTR_DATA;				 //���͵�������
  Can_Handle.pTxMsg->DLC=8;						      	 //���ݳ���Ϊ8�ֽ�
	
  /*����Ҫ���͵�����0-7*/
  for (ubCounter = 0; ubCounter < 8; ubCounter++)
  {
    Can_Handle.pTxMsg->Data[ubCounter] = ubCounter;
  }
}

/*
bit3~bit31 EXID ( ����:  bit21~bit31 STID)
bit2: IDE
bit1: RTR
*/


void CAN_RX_IRQHandler(void)
{
	HAL_CAN_IRQHandler(&Can_Handle);
}

void CAN2_TX_IRQHandler(void)
{
	HAL_CAN_IRQHandler(&Can_Handle);
}

/**
  * @brief  CAN��������ж�(������) 
  * @param  hcan: CAN���ָ��
  * @retval ��
  */
void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan)
{
	/* �Ƚ�ID�Ƿ�Ϊ0x1314 */ 
	if((hcan->pRxMsg->ExtId==0x1314) && (hcan->pRxMsg->IDE==CAN_ID_EXT) && (hcan->pRxMsg->DLC==8) )
	{
		flag = 1; //���ճɹ�  
	}
	else
	{
		flag = 0; //����ʧ��
	}
	/* ׼���жϽ��� */
	HAL_CAN_Receive_IT(&Can_Handle, CAN_FIFO0);
}
/**
  * @brief  CAN����ص�����
  * @param  hcan: CAN���ָ��
  * @retval ��
  */
void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan)
{
	printf("\r\nCAN����\r\n");
}


/**************************END OF FILE************************************/

