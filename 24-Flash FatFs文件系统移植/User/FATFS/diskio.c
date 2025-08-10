/**
 ****************************************************************************************************
 * @file        diskio.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-28
 * @brief       FATFS底层(diskio) 驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32F103开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20200428
 * 第一次发布
 *
 ****************************************************************************************************
 */

#include "diskio.h"		/* FatFs lower layer API */
#include "ff.h"
#include "./flash/bsp_spi_flash.h"


/* 为每个设备定义一个物理编号 */
#define ATA			    0     // 预留SD卡使用
#define SPI_FLASH		1     // 外部SPI Flash


/* 扇区偏移2MB，外部Flash文件系统空间放在SPI Flash后面6MB空间 */
/*
使用方法：

BYTE *buff,		 数据缓存区 
DWORD sector,	 扇区首地址 
UINT count		 扇区个数(1..128)

sector+=512;
write_addr = sector<<12;    
SPI_FLASH_SectorErase(write_addr);
SPI_FLASH_BufferWrite((uint8_t *)buff,write_addr,count<<12);

SPI_FLASH 一页大小 256B，一扇区大小4096B(4K)
写地址 << 12，即地址*4096，

假设要写入扇区首地址为0/256/512，
(ADD+512)*4096 / 1024 = 2048B = 2M！
所以通过此完成2M偏移！
此外，count为读取扇区个数，*4096即读取字节数

*/


//#define SPI_FLASH_SECTOR_SIZE   512
//#define SPI_FLASH_SECTOR_COUNT  12 * 1024 * 2   /* 25Q128, 前12M字节给FATFS占用 */
//#define SPI_FLASH_BLOCK_SIZE    8               /* 每个BLOCK有8个扇区 */
//#define SPI_FLASH_FATFS_BASE    0               /* FATFS 在外部FLASH的起始地址 从0开始 */


/**
 * @brief       获得磁盘状态
 * @param       pdrv : 磁盘编号0~9
 * @retval      DSTATUS:FATFS规定的返回值
 */
DSTATUS disk_status (
    BYTE pdrv       /* 物理编号 */
)
{
    DSTATUS status = RES_OK;
    
    switch (pdrv) {
        case ATA:	/* SD CARD */
            break;
        
        case SPI_FLASH:      
            /* SPI Flash状态检测：读取SPI Flash 设备ID */
            if(sFLASH_ID == SPI_FLASH_ReadID())
            {
                /* 设备ID读取结果正确 */
                status = RES_OK;
            }
            else
            {
                /* 设备ID读取结果错误 */
                status = STA_NOINIT;;
            }
            break;

        default:
            status = STA_NOINIT;
    }
    return status;
}

/**
 * @brief       初始化磁盘
 * @param       pdrv : 磁盘编号0~9
 * @retval      DSTATUS:FATFS规定的返回值
 */
DSTATUS disk_initialize (
    BYTE pdrv       /* 物理编号 */
)
{
    uint16_t i;
    DSTATUS status = STA_NOINIT;	
    
    switch (pdrv) {
        case ATA:	         /* SD CARD */
            break;
      
      case SPI_FLASH:    /* SPI Flash */ 
          /* 初始化SPI Flash */
          SPI_FLASH_Init();
          /* 延时一小段时间 */
          i=500;
          while(--i);	
          /* 唤醒SPI Flash */
          SPI_Flash_WAKEUP();
          /* 获取SPI Flash芯片状态 */
          status=disk_status(SPI_FLASH);
          break;
        
      default:
          status = STA_NOINIT;
    }
    return status;
}

/**
 * @brief       读扇区
 * @param       pdrv   : 磁盘编号0~9
 * @param       buff   : 数据接收缓冲首地址
 * @param       sector : 扇区地址
 * @param       count  : 需要读取的扇区数
 * @retval      DRESULT:FATFS规定的返回值
 */
DRESULT disk_read (
    BYTE pdrv,		/* 设备物理编号(0..) */
    BYTE *buff,		/* 数据缓存区 */
    DWORD sector,	/* 扇区首地址 */
    UINT count		/* 扇区个数(1..128) */
)
{
    DRESULT status = RES_PARERR;
    
    switch (pdrv) {
        case ATA:	/* SD CARD */
            break;
        
        case SPI_FLASH:
            /* 扇区偏移2MB，外部Flash文件系统空间放在SPI Flash后面6MB空间 */
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
 * @brief       写扇区
 * @param       pdrv   : 磁盘编号0~9
 * @param       buff   : 发送数据缓存区首地址
 * @param       sector : 扇区地址
 * @param       count  : 需要写入的扇区数
 * @retval      DRESULT:FATFS规定的返回值
 */
DRESULT disk_write (
    BYTE pdrv,			  /* 设备物理编号(0..) */
    const BYTE *buff,	/* 欲写入数据的缓存区 */
    DWORD sector,		  /* 扇区首地址 */
    UINT count			  /* 扇区个数(1..128) */
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
          /* 扇区偏移2MB，外部Flash文件系统空间放在SPI Flash后面6MB空间 */
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
 * @brief       获取其他控制参数
 * @param       pdrv   : 磁盘编号0~9
 * @param       ctrl   : 控制代码
 * @param       buff   : 发送/接收缓冲区指针
 * @retval      DRESULT:FATFS规定的返回值
 */
DRESULT disk_ioctl (
    BYTE pdrv,		/* 物理编号 */
    BYTE cmd,		  /* 控制指令 */
    void *buff		/* 写入或者读取数据地址指针 */
)
{
    DRESULT status = RES_PARERR;
    switch (pdrv) {
        case ATA:	/* SD CARD */
            break;
      
      case SPI_FLASH:
          switch (cmd) {
              /* 扇区数量：1536*4096/1024/1024=6(MB) */
              case GET_SECTOR_COUNT:
                *(DWORD * )buff = 1536;		
              break;
              /* 扇区大小  */
              case GET_SECTOR_SIZE :
                *(WORD * )buff = 4096;
              break;
              /* 同时擦除扇区个数 */
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
/* 返回当前时间戳 */
    return	  ((DWORD)(2015 - 1980) << 25)	/* Year 2015 */
        | ((DWORD)1 << 21)				/* Month 1 */
        | ((DWORD)1 << 16)				/* Mday 1 */
        | ((DWORD)0 << 11)				/* Hour 0 */
        | ((DWORD)0 << 5)				  /* Min 0 */
        | ((DWORD)0 >> 1);				/* Sec 0 */
}

