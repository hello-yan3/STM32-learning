/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   FatFs�ļ�ϵͳ���ú�������
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103-�Ե� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx.h"
#include "./usart/bsp_debug_usart.h"
#include "ff.h"
#include "string.h"
#include "./flash/bsp_spi_flash.h"

/* 
FATFS������
http://elm-chan.org/fsw/ff/00index_e.html
*/

/*
FATFS��ֲ��Ҫ���������ļ�
�� diskio.c ��Ӳ����ش��룬��Ҫʵ�ֵײ��д�ӿں���
  ��Ҫʵ�ֺ����� 
    disk_initialize		��ʼ������������ 
    disk_status		��ȡ����״̬
    disk_read			�Ӵ���������������
    disk_write		�Ӵ���������д����
    disk_ioctl			�����豸ʵ��ָ�����ܣ����ڸ���FATFS������API
    get_fattime 		��ȡ��ǰʱ��
  
�� ffconf.h �������ò�������������������
  ��Ҫ�޸Ĳ�����
  * ϵͳ���ã�
    FF_FS_TINY    ����ʹ��FATFSΪ����ģʽ����Tinyģʽ    0
    FF_FS_EXFAT   ʹ�û����exFAT�ļ�ϵͳ(ʹ��exFAT��ʹ�ܳ��ļ���)    1
  * ���ܺ�������
    FF_FS_READONLY    ʹ�ܻ��ֹ��д��غ�����������ֻ��   0
    FF_USE_MKFS   ʹ�ܻ����f_mkfs���������Ƿ�ʹ�ܸ�ʽ��   1
    FF_USE_FASTSEEK   ʹ�ܿ����������ܣ��ӿ�f_lseek/read/write����ִ��   1
    FF_USE_LABEL    ʹ�ܻ��ֹ֧�ִ����̷���ȡ�����ú���    1
    FF_USE_STRFUNC    �����Ƿ�֧���ַ��������    1 
    FF_STRF_ENCODE    �����ַ���I/O������д�ļ��ַ�����    0
  * �����ռ�ͱ��ػ�������	
    FF_CODE_PAGE    �����������ͣ�������������Ϊ963     963
    FF_USE_LFN    ʹ�ܻ��ֹ���ļ�����ȡֵ��Χ(0~3)���洢�ط���ͬ    3 
  * ��������	  
    FF_VOLUMES    ����FATFS֧�ֵ��߼��豸��Ŀ    2
    FF_MAX_SS   �������������С    4096

�� ff.c/ff.h/diskio.h/ffunicode.c/ffsystem.c ����
   FATFS Դ���룬����Ҫ�޸�
   (��ֲ����ԭ��FATFS��������ԭ�ӽ�����С�޸ģ������޸�ʲôҲû��)
 
�� exfuns.c/fattester.c ����ԭ�Ӳ����ļ���������ɶҲû����Ӧ��û�õ�  

*/

/*
���ÿ��ź�����

�ļ�������
f_open		��/����һ���ļ�
f_close		�ر�һ���򿪵��ļ�
f_read 		���ļ��ж�ȡ����
f_write		���ļ���д����
f_gets		��һ���ַ���
f_putc 		дһ���ַ�
f_puts		дһ���ַ���
f_printf		дһ����ʽ�����ַ���
f_lseek		�ƶ��ļ���/дָ��
f_tell		��ȡ��ǰ��/дָ��
f_size		��ȡ�ļ���С

Ŀ¼������
f_opendif		��һ��Ŀ¼
f_closedir	�ر�һ���Ѿ��򿪵�Ŀ¼
f_readdir		��ȡĿ¼��Ŀ
f_mkdir		����һ����Ŀ¼
f_unlink		ɾ��һ���ļ���Ŀ¼
f_rename		������/�ƶ�һ���ļ����ļ���

�����
f_mount		ע��/ע��һ��������
f_mkfs		��ʽ��������һ���ļ�ϵͳ
f_getfree		��ȡ������Ϣ�Լ����д�����
f_setlabel	�����̷�(��������)
f_getlabel	��ȡ�̷�	
*/

/*

���������

******** ����һ��SPI FLASH �ļ�ϵͳʵ�� *******
���ļ�ϵͳ���سɹ������Խ��ж�д����

****** ���������ļ�д�����... ******
����/����FatFs��д�����ļ�.txt�ļ��ɹ������ļ�д�����ݡ�
���ļ�д��ɹ���д���ֽ����ݣ�63
�����ļ�д�������Ϊ��
��ӭʹ��Ұ��STM32������ �����Ǹ������ӣ��½��ļ�ϵͳ�����ļ�

****** ���������ļ���ȡ����... ******
�����ļ��ɹ���
���ļ���ȡ�ɹ�,�����ֽ����ݣ�63
����ȡ�õ��ļ�����Ϊ��
��ӭʹ��Ұ��STM32������ �����Ǹ������ӣ��½��ļ�ϵͳ�����ļ�
 

*************** �豸��Ϣ��ȡ ***************
���豸�ܿռ䣺      5868 KB��
�����ÿռ䣺        5856 KB��

******** �ļ���λ�͸�ʽ��д�빦�ܲ��� ********
���ļ����ݣ�
��ӭʹ��Ұ��STM32������ �����Ǹ������ӣ��½��ļ�ϵͳ�����ļ�


********** Ŀ¼���������������ܲ��� **********

*************** �ļ���Ϣ��ȡ���� **************
��testdir.txt���ļ���Ϣ��
���ļ���С: 64(�ֽ�)
��ʱ���: 2015/01/01, 00:00
������: ----A

***************** �ļ�ɨ����� ****************
1:/TestDir/testdir.txt



*/

FATFS fs;													/* FatFs�ļ�ϵͳ���� */
FIL fnew;													/* �ļ����� */
FRESULT res_flash;                /* �ļ�������� */
UINT fnum;            					  /* �ļ��ɹ���д���� */
char fpath[100];                  /* ���浱ǰɨ��·�� */
char readbuffer[512];    

BYTE work[FF_MAX_SS];//��ʽ���豸������
BYTE WriteBuffer[] = "��ӭʹ��Ұ��STM32������ �����Ǹ������ӣ��½��ļ�ϵͳ�����ļ�\r\n"; 
BYTE ReadBuffer[1024]={0};        /* �������� */

/* FatFs����ܲ��� */
static FRESULT miscellaneous(void)
{
  DIR dir;
  FATFS *pfs;
  DWORD fre_clust, fre_sect, tot_sect;
  
  printf("\n*************** �豸��Ϣ��ȡ ***************\r\n");
  /* ��ȡ�豸��Ϣ�Ϳմش�С */
  res_flash = f_getfree("1:", &fre_clust, &pfs);

  /* ����õ��ܵ����������Ϳ��������� */
  tot_sect = (pfs->n_fatent - 2) * pfs->csize;
  fre_sect = fre_clust * pfs->csize;

  /* ��ӡ��Ϣ(4096 �ֽ�/����) */
  printf("���豸�ܿռ䣺%10lu KB��\n�����ÿռ䣺  %10lu KB��\n", tot_sect *4, fre_sect *4);
  
  printf("\n******** �ļ���λ�͸�ʽ��д�빦�ܲ��� ********\r\n");
  res_flash = f_open(&fnew, "1:FatFs��д�����ļ�.txt",
                            FA_OPEN_ALWAYS|FA_WRITE|FA_READ );
	if ( res_flash == FR_OK )
	{
    /*  �ļ���λ */
    res_flash = f_lseek(&fnew,f_size(&fnew));
    if (res_flash == FR_OK)
    {
      /* ��ʽ��д�룬������ʽ����printf���� */
      f_printf(&fnew,"\n��ԭ���ļ������һ������\n");
      f_printf(&fnew,"���豸�ܿռ䣺%10lu KB��\n�����ÿռ䣺  %10lu KB��\n", tot_sect *4, fre_sect *4);
      /*  �ļ���λ���ļ���ʼλ�� */
      res_flash = f_lseek(&fnew,0);
      /* ��ȡ�ļ��������ݵ������� */
      res_flash = f_read(&fnew,readbuffer,f_size(&fnew),&fnum);
      if(res_flash == FR_OK)
      {
        printf("���ļ����ݣ�\n%s\n",readbuffer);
      }
    }
    f_close(&fnew);    
    
    printf("\n********** Ŀ¼���������������ܲ��� **********\r\n");
    /* ���Դ�Ŀ¼ */
    res_flash=f_opendir(&dir,"1:TestDir");
    if(res_flash!=FR_OK)
    {
      /* ��Ŀ¼ʧ�ܣ��ʹ���Ŀ¼ */
      res_flash=f_mkdir("1:TestDir");
    }
    else
    {
      /* ���Ŀ¼�Ѿ����ڣ��ر��� */
      res_flash=f_closedir(&dir);
      /* ɾ���ļ� */
      f_unlink("1:TestDir/testdir.txt");
    }
    if(res_flash==FR_OK)
    {
      /* ���������ƶ��ļ� */
      res_flash=f_rename("1:FatFs��д�����ļ�.txt","1:TestDir/testdir.txt");      
    } 
	}
  else
  {
    printf("!! ���ļ�ʧ�ܣ�%d\n",res_flash);
    printf("!! ������Ҫ�ٴ����С�FatFs��ֲ���д���ԡ�����\n");
  }
  return res_flash;
}


FILINFO fno;
/**
  * �ļ���Ϣ��ȡ
  */
static FRESULT file_check(void)
{ 
  /* ��ȡ�ļ���Ϣ */
  res_flash=f_stat("1:TestDir/testdir.txt",&fno);
  if(res_flash==FR_OK)
  {
    printf("��testdir.txt���ļ���Ϣ��\n");
    printf("���ļ���С: %ld(�ֽ�)\n", fno.fsize);
    printf("��ʱ���: %u/%02u/%02u, %02u:%02u\n",
           (fno.fdate >> 9) + 1980, fno.fdate >> 5 & 15, fno.fdate & 31,fno.ftime >> 11, fno.ftime >> 5 & 63);
    printf("������: %c%c%c%c%c\n\n",
           (fno.fattrib & AM_DIR) ? 'D' : '-',      // ��һ��Ŀ¼
           (fno.fattrib & AM_RDO) ? 'R' : '-',      // ֻ���ļ�
           (fno.fattrib & AM_HID) ? 'H' : '-',      // �����ļ�
           (fno.fattrib & AM_SYS) ? 'S' : '-',      // ϵͳ�ļ�
           (fno.fattrib & AM_ARC) ? 'A' : '-');     // �����ļ�
  }
  return res_flash;
}


/**
  * @brief  scan_files �ݹ�ɨ��FatFs�ڵ��ļ�
  * @param  path:��ʼɨ��·��
  * @retval result:�ļ�ϵͳ�ķ���ֵ
  */
static FRESULT scan_files (char* path) 
{ 
  FRESULT res; 		//�����ڵݹ���̱��޸ĵı���������ȫ�ֱ���	
  FILINFO fno; 
  DIR dir; 
  int i;            
  char *fn;        // �ļ���	
	
#if _USE_LFN 
  /* ���ļ���֧�� */
  /* ����������Ҫ2���ֽڱ���һ�����֡�*/
  static char lfn[_MAX_LFN*2 + 1]; 	
  fno.lfname = lfn; 
  fno.lfsize = sizeof(lfn); 
#endif 
  //��Ŀ¼
  res = f_opendir(&dir, path); 
  if (res == FR_OK) 
	{ 
    i = strlen(path); 
    for (;;) 
		{ 
      //��ȡĿ¼�µ����ݣ��ٶ����Զ�����һ���ļ�
      res = f_readdir(&dir, &fno); 								
      //Ϊ��ʱ��ʾ������Ŀ��ȡ��ϣ�����
      if (res != FR_OK || fno.fname[0] == 0) break; 	
#if _USE_LFN 
      fn = *fno.lfname ? fno.lfname : fno.fname; 
#else 
      fn = fno.fname; 
#endif 
      //���ʾ��ǰĿ¼������			
      if (*fn == '.') continue; 	
      //Ŀ¼���ݹ��ȡ      
      if (fno.fattrib & AM_DIR)         
			{ 			
        //�ϳ�����Ŀ¼��        
        sprintf(&path[i], "/%s", fn); 		
        //�ݹ����         
        res = scan_files(path);	
        path[i] = 0;         
        //��ʧ�ܣ�����ѭ��        
        if (res != FR_OK) 
					break; 
      } 
			else 
			{ 
				printf("%s/%s\r\n", path, fn);								//����ļ���	
        /* ������������ȡ�ض���ʽ���ļ�·�� */        
      }//else
    } //for
  } 
  return res; 
}


/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{
  HAL_Init();        
  /* ����ϵͳʱ��Ϊ72 MHz */ 
  SystemClock_Config();
  /*��ʼ��USART ����ģʽΪ 115200 8-N-1���жϽ���*/
  DEBUG_USART_Config();
  
  printf("******** ����һ��SPI FLASH �ļ�ϵͳʵ�� *******\r\n");
  
  //���ⲿSPI Flash�����ļ�ϵͳ���ļ�ϵͳ����ʱ���SPI�豸��ʼ��
  res_flash = f_mount(&fs,"1:",1);
	
  /*----------------------- ��ʽ������ -----------------*/  
	/* ���û���ļ�ϵͳ�͸�ʽ�����������ļ�ϵͳ */
	if(res_flash == FR_NO_FILESYSTEM)
	{
		printf("��FLASH��û���ļ�ϵͳ���������и�ʽ��...\r\n");
    /* ��ʽ�� */
		res_flash=f_mkfs("1:", 0, work, sizeof(work));						
		
		if(res_flash == FR_OK)
		{
			printf("��FLASH�ѳɹ���ʽ���ļ�ϵͳ��\r\n");
      /* ��ʽ������ȡ������ */
			res_flash = f_mount(NULL,"1:",1);			
      /* ���¹���	*/			
			res_flash = f_mount(&fs,"1:",1);
		}
		else
		{
			printf("������ʽ��ʧ�ܡ�����\r\n");
			while(1);
		}
	}
  else if(res_flash!=FR_OK)
  {
    printf("�����ⲿFlash�����ļ�ϵͳʧ�ܡ�(%d)\r\n",res_flash);
    printf("������ SPI����д����FLASH ���̲��ԣ�����������ڸ�����f_mount�����if���ǰ��ʱ�����һ�� res_flash = FR_NO_FILESYSTEM; ������ֱ��ִ�и�ʽ������\r\n");
		while(1);
  }
  else
  {
    printf("���ļ�ϵͳ���سɹ������Խ��ж�д����\r\n");
  }  
  
/*----------------------- �ļ�ϵͳ���ԣ�д���� -------------------*/
	/* ���ļ���ÿ�ζ����½�����ʽ�򿪣�����Ϊ��д */
	printf("\r\n****** ���������ļ�д�����... ******\r\n");	
  // 1:��ʾ ������ ������0����������1��SPI_FLASH ���������Ϊ 1
	res_flash = f_open(&fnew, "1:FatFs��д�����ļ�.txt",FA_OPEN_ALWAYS | FA_WRITE );
	if ( res_flash == FR_OK )
	{
		printf("����/����FatFs��д�����ļ�.txt�ļ��ɹ������ļ�д�����ݡ�\r\n");
    /* ��ָ���洢������д�뵽�ļ��� */
		res_flash=f_write(&fnew,WriteBuffer,sizeof(WriteBuffer),&fnum);
    if(res_flash==FR_OK)
    {
      printf("���ļ�д��ɹ���д���ֽ����ݣ�%d\n",fnum);
      printf("�����ļ�д�������Ϊ��\r\n%s\r\n",WriteBuffer);
    }
    else
    {
      printf("�����ļ�д��ʧ�ܣ�(%d)\n",res_flash);
    }    
		/* ���ٶ�д���ر��ļ� */
    f_close(&fnew);
	}
	else
	{	
		printf("������/�����ļ�ʧ�ܡ�\r\n");
	}
	
/*------------------- �ļ�ϵͳ���ԣ������� --------------------------*/
	printf("****** ���������ļ���ȡ����... ******\r\n");
  // 1:��ʾ ������ ������0����������1��SPI_FLASH ���������Ϊ 1
  res_flash = f_open(&fnew, "1:FatFs��д�����ļ�.txt",FA_OPEN_ALWAYS | FA_READ); 	 
	if(res_flash == FR_OK)
	{

		printf("�����ļ��ɹ���\r\n");
		res_flash = f_read(&fnew, ReadBuffer, sizeof(ReadBuffer), &fnum); 
    if(res_flash==FR_OK)
    {
      printf("���ļ���ȡ�ɹ�,�����ֽ����ݣ�%d\r\n",fnum);
      printf("����ȡ�õ��ļ�����Ϊ��\r\n%s \r\n", ReadBuffer);	
    }
    else
    {
      printf("�����ļ���ȡʧ�ܣ�(%d)\n",res_flash);
    }		
	}
	else
	{
		printf("�������ļ�ʧ�ܡ�\r\n");
	}
	/* ���ٶ�д���ر��ļ� */
	f_close(&fnew);	  
  
/*-----------------------------------------------------------------------*/
/*-------------------------����Ϳ������ˡ���-----------------------------*/ 
  
  /* FatFs����ܲ��� */
  res_flash = miscellaneous();

  
  printf("\n*************** �ļ���Ϣ��ȡ���� **************\r\n");
  res_flash = file_check();

  
  printf("***************** �ļ�ɨ����� ****************\r\n");
  strcpy(fpath,"1:");
  scan_files(fpath);
  
  
	/* ����ʹ���ļ�ϵͳ��ȡ�������ļ�ϵͳ */
	f_mount(NULL,"1:",1);
  
  /* ������ɣ�ͣ�� */
	while(1)
	{ 
	}
}



/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 72000000
  *            HCLK(Hz)                       = 72000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 2
  *            APB2 Prescaler                 = 1
  *            HSE Frequency(Hz)              = 8000000
  *            HSE PREDIV1                    = 2
  *            PLLMUL                         = 9
  *            Flash Latency(WS)              = 0
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef clkinitstruct = {0};
  RCC_OscInitTypeDef oscinitstruct = {0};
  
  /* Enable HSE Oscillator and activate PLL with HSE as source */
  oscinitstruct.OscillatorType  = RCC_OSCILLATORTYPE_HSE;
  oscinitstruct.HSEState        = RCC_HSE_ON;
  oscinitstruct.HSEPredivValue  = RCC_HSE_PREDIV_DIV2;
  oscinitstruct.PLL.PLLState    = RCC_PLL_ON;
  oscinitstruct.PLL.PLLSource   = RCC_PLLSOURCE_HSE;
  oscinitstruct.PLL.PLLMUL      = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&oscinitstruct)!= HAL_OK)
  {
    /* Initialization Error */
    while(1); 
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  clkinitstruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  clkinitstruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  clkinitstruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  clkinitstruct.APB2CLKDivider = RCC_HCLK_DIV1;
  clkinitstruct.APB1CLKDivider = RCC_HCLK_DIV2;  
  if (HAL_RCC_ClockConfig(&clkinitstruct, FLASH_LATENCY_0)!= HAL_OK)
  {
    /* Initialization Error */
    while(1); 
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
