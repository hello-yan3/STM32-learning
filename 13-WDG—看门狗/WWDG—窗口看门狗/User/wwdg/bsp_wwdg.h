#ifndef __BSP_WWDG_H
#define	__BSP_WWDG_H


#include "stm32f1xx.h"

/* brief��
 * ���ڿ��Ź������ϴ���ֵ���´���ֵ��
 * ���������趨�ĳ�ʼֵ��ʼ���µݼ������������´���ֵ�м�ι��
 * ��ǰι�����Ӻ�ι��������и�λ
 * WWDG ���ú���
 * WWDG_COUNTER  ���ݼ���ʱ���ĳ�ʼֵ�� ȡֵ��ΧΪ��0x7f~0x40
 * WWDG_WINDOW   ���ϴ���ֵ��ȡֵ��ΧΪ��0x7f~0x40
 * WWDG_PRESCALER��Ԥ��Ƶ��ֵ��ȡֵ������
 *      @arg WWDG_Prescaler_1: WWDG counter clock frequency = (PCLK1(36MHZ)/4096)/1
 *      @arg WWDG_Prescaler_2: WWDG counter clock frequency = (PCLK1(36MHZ)/4096)/2
 *      @arg WWDG_Prescaler_4: WWDG counter clock frequency = (PCLK1(36MHZ)/4096)/4
 *      @arg WWDG_Prescaler_8: WWDG counter clock frequency = (PCLK1(36MHZ)/4096)/8
 * ��ʱʱ����㣺1 / WWDG counter clock frequency 
 * һ������������ WWDG_PRESCALER Ԥ��Ƶ�� ���
 * ʱ�����ӣ�  Period = 1/((PCLK1(36MHZ)/4096)/WWDG_PRESCALER) = 1/((PCLK1(36MHZ)/4096)/8) = 0.91ms
 * �´��ڼ�ʱ��(WWDG_COUNTER - 0x40) * Period = (0x7F - 0x40) * 0.91ms
 * �ϴ��ڼ�ʱ��(WWDG_COUNTER - WWDG_WINDOW) * Period = (0x7F - 0x50) * 0.91ms
 */
// Ԥ��Ƶ��ֵ
#define WWDG_PRESCALER   WWDG_PRESCALER_8
// �ϴ���ֵ
#define WWDG_WINDOW      0x50
// ��������ֵ
#define WWDG_COUNTER     0x7F


void WWDG_Config(void);

extern WWDG_HandleTypeDef WWDG_Handle;

#endif /* __BSP_WWDG_H */

