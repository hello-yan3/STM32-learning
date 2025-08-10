/**
 ****************************************************************************************************
 * @file        main.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2021-11-04
 * @brief       �ڴ���� ʵ��
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

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/KEY/key.h"
#include "./BSP/ADC/adc.h"
#include "./BSP/AP3216C/ap3216c.h"
#include "./BSP/I2C/i2c.h"
#include "uc-os3_demo.h"


/*
  ���徲̬���� time��
  �����ʱ��  һֱ time++��֮�� ++ ������ݣ�������������
  
  ����1����  ������ֵ�ź�1�������ڽ��յ��źź�ִ��AD�ɼ����ɼ���ɺ�AD��ɱ�־��1��˳����һ�� LED �ƣ� printf һ��ʱ�䣡
  ����2����  ������ֵ�ź�2�������ڽ��յ��źź�ִ��I2C��ȡ���ݣ��ɼ���ɺ�I2C��־��1��˳����һ�� LED �ƣ� printf һ��ʱ�䣡
  ����1&2���� �����ź�++

  ����ȫ��ʹ�ö��б��棡���� I2C&AD ʹ�� ������Ƕ����

  ��� ������־ ����Ϊ1��
  �������е����ݣ�printf�����˳�� printf һ��ʱ�䣡
*/

int main(void)
{
    HAL_Init();                         /* ��ʼ��HAL�� */
    sys_stm32_clock_init(RCC_PLL_MUL9); /* ����ʱ��, 72Mhz */
    delay_init(72);                     /* ��ʱ��ʼ�� */
    usart_init(115200);                 /* ���ڳ�ʼ��Ϊ115200 */
    led_init();                         /* ��ʼ��LED */
    key_init();                         /* ��ʼ������ */
  
    /*********** ADC ***********/
    Rheostat_Init();
    // ִ�� ADC �Զ���У׼��ע�������ADC����ǰʹ��
    HAL_ADCEx_Calibration_Start(&ADC_Handle);   
  
    /*********** I2C ***********/
    ap3216c_init();
    
    printf("��������ʼ��������UCOSIII�ں�\r\n");
  
    /*********** UCOSIII ***********/
    uc_os3_demo();                      /* ���� uC/OS-III ���� */
}


