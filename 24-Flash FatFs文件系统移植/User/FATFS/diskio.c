/**
 ****************************************************************************************************
 * @file        diskio.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-28
 * @brief       FATFS�ײ�(diskio) ��������
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� STM32F103������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 * �޸�˵��
 * V1.0 20200428
 * ��һ�η���
 *
 ****************************************************************************************************
 */

#include "diskio.h"		/* FatFs lower layer API */
#include "ff.h"
#include "./flash/bsp_spi_flash.h"


/* Ϊÿ���豸����һ�������� */
#define ATA			    0     // Ԥ��SD��ʹ��
#define SPI_FLASH		1     // �ⲿSPI Flash


/* ����ƫ��2MB���ⲿFlash�ļ�ϵͳ�ռ����SPI Flash����6MB�ռ� */
/*
ʹ�÷�����

BYTE *buff,		 ���ݻ����� 
DWORD sector,	 �����׵�ַ 
UINT count		 ��������(1..128)

sector+=512;
write_addr = sector<<12;    
SPI_FLASH_SectorErase(write_addr);
SPI_FLASH_BufferWrite((uint8_t *)buff,write_addr,count<<12);

SPI_FLASH һҳ��С 256B��һ������С4096B(4K)
д��ַ << 12������ַ*4096��

����Ҫд�������׵�ַΪ0/256/512��
(ADD+512)*4096 / 1024 = 2048B = 2M��
����ͨ�������2Mƫ�ƣ�
���⣬countΪ��ȡ����������*4096����ȡ�ֽ���

*/


//#define SPI_FLASH_SECTOR_SIZE   512
//#define SPI_FLASH_SECTOR_COUNT  12 * 1024 * 2   /* 25Q128, ǰ12M�ֽڸ�FATFSռ�� */
//#define SPI_FLASH_BLOCK_SIZE    8               /* ÿ��BLOCK��8������ */
//#define SPI_FLASH_FATFS_BASE    0               /* FATFS ���ⲿFLASH����ʼ��ַ ��0��ʼ */


/**
 * @brief       ��ô���״̬
 * @param       pdrv : ���̱��0~9
 * @retval      DSTATUS:FATFS�涨�ķ���ֵ
 */
DSTATUS disk_status (
    BYTE pdrv       /* ������ */
)
{
    DSTATUS status = RES_OK;
    
    switch (pdrv) {
        case ATA:	/* SD CARD */
            break;
        
        case SPI_FLASH:      
            /* SPI Flash״̬��⣺��ȡSPI Flash �豸ID */
            if(sFLASH_ID == SPI_FLASH_ReadID())
            {
                /* �豸ID��ȡ�����ȷ */
                status = RES_OK;
            }
            else
            {
                /* �豸ID��ȡ������� */
                status = STA_NOINIT;;
            }
            break;

        default:
            status = STA_NOINIT;
    }
    return status;
}

/**
 * @brief       ��ʼ������
 * @param       pdrv : ���̱��0~9
 * @retval      DSTATUS:FATFS�涨�ķ���ֵ
 */
DSTATUS disk_initialize (
    BYTE pdrv       /* ������ */
)
{
    uint16_t i;
    DSTATUS status = STA_NOINIT;	
    
    switch (pdrv) {
        case ATA:	         /* SD CARD */
            break;
      
      case SPI_FLASH:    /* SPI Flash */ 
          /* ��ʼ��SPI Flash */
          SPI_FLASH_Init();
          /* ��ʱһС��ʱ�� */
          i=500;
          while(--i);	
          /* ����SPI Flash */
          SPI_Flash_WAKEUP();
          /* ��ȡSPI FlashоƬ״̬ */
          status=disk_status(SPI_FLASH);
          break;
        
      default:
          status = STA_NOINIT;
    }
    return status;
}

/**
 * @brief       ������
 * @param       pdrv   : ���̱��0~9
 * @param       buff   : ���ݽ��ջ����׵�ַ
 * @param       sector : ������ַ
 * @param       count  : ��Ҫ��ȡ��������
 * @retval      DRESULT:FATFS�涨�ķ���ֵ
 */
DRESULT disk_read (
    BYTE pdrv,		/* �豸������(0..) */
    BYTE *buff,		/* ���ݻ����� */
    DWORD sector,	/* �����׵�ַ */
    UINT count		/* ��������(1..128) */
)
{
    DRESULT status = RES_PARERR;
    
    switch (pdrv) {
        case ATA:	/* SD CARD */
            break;
        
        case SPI_FLASH:
            /* ����ƫ��2MB���ⲿFlash�ļ�ϵͳ�ռ����SPI Flash����6MB�ռ� */
            sector+=512;      
            SPI_FLASH_BufferRead(buff, sector <<12, count<<12);
            status = RES_OK;
        break;
        
        default:
            status = RES_PARERR;
    }
    return status;
}

/**
 * @brief       д����
 * @param       pdrv   : ���̱��0~9
 * @param       buff   : �������ݻ������׵�ַ
 * @param       sector : ������ַ
 * @param       count  : ��Ҫд���������
 * @retval      DRESULT:FATFS�涨�ķ���ֵ
 */
DRESULT disk_write (
    BYTE pdrv,			  /* �豸������(0..) */
    const BYTE *buff,	/* ��д�����ݵĻ����� */
    DWORD sector,		  /* �����׵�ַ */
    UINT count			  /* ��������(1..128) */
)
{
    uint32_t write_addr; 
    DRESULT status = RES_PARERR;
    if (!count) {
        return RES_PARERR;		/* Check parameter */
    }

    switch (pdrv) {
        case ATA:	/* SD CARD */      
        break;

      case SPI_FLASH:
          /* ����ƫ��2MB���ⲿFlash�ļ�ϵͳ�ռ����SPI Flash����6MB�ռ� */
          sector+=512;
          write_addr = sector<<12;    
          SPI_FLASH_SectorErase(write_addr);
          SPI_FLASH_BufferWrite((uint8_t *)buff,write_addr,count<<12);
          status = RES_OK;
      break;
      
      default:
          status = RES_PARERR;
    }
    return status;
}

/**
 * @brief       ��ȡ�������Ʋ���
 * @param       pdrv   : ���̱��0~9
 * @param       ctrl   : ���ƴ���
 * @param       buff   : ����/���ջ�����ָ��
 * @retval      DRESULT:FATFS�涨�ķ���ֵ
 */
DRESULT disk_ioctl (
    BYTE pdrv,		/* ������ */
    BYTE cmd,		  /* ����ָ�� */
    void *buff		/* д����߶�ȡ���ݵ�ַָ�� */
)
{
    DRESULT status = RES_PARERR;
    switch (pdrv) {
        case ATA:	/* SD CARD */
            break;
      
      case SPI_FLASH:
          switch (cmd) {
              /* ����������1536*4096/1024/1024=6(MB) */
              case GET_SECTOR_COUNT:
                *(DWORD * )buff = 1536;		
              break;
              /* ������С  */
              case GET_SECTOR_SIZE :
                *(WORD * )buff = 4096;
              break;
              /* ͬʱ������������ */
              case GET_BLOCK_SIZE :
                *(DWORD * )buff = 1;
              break;        
          }
          status = RES_OK;
      break;
      
      default:
          status = RES_PARERR;
    }
    return status;
}


__weak DWORD get_fattime(void) {
/* ���ص�ǰʱ��� */
    return	  ((DWORD)(2015 - 1980) << 25)	/* Year 2015 */
        | ((DWORD)1 << 21)				/* Month 1 */
        | ((DWORD)1 << 16)				/* Mday 1 */
        | ((DWORD)0 << 11)				/* Hour 0 */
        | ((DWORD)0 << 5)				  /* Min 0 */
        | ((DWORD)0 >> 1);				/* Sec 0 */
}

