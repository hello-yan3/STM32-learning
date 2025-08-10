/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   FatFs文件系统常用函数测试
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 F103-霸道 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
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
FATFS官网：
http://elm-chan.org/fsw/ff/00index_e.html
*/

/*
FATFS移植主要配置两个文件
① diskio.c 与硬件相关代码，主要实现底层读写接口函数
  主要实现函数： 
    disk_initialize		初始化磁盘驱动器 
    disk_status		获取磁盘状态
    disk_read			从磁盘驱动器读扇区
    disk_write		从磁盘驱动器写扇区
    disk_ioctl			控制设备实现指定功能，用于辅助FATFS中其他API
    get_fattime 		获取当前时间
  
② ffconf.h 包含配置参数，根据需求来配置
  主要修改参数：
  * 系统配置：
    FF_FS_TINY    配置使用FATFS为正常模式还是Tiny模式    0
    FF_FS_EXFAT   使用或禁用exFAT文件系统(使能exFAT需使能长文件名)    1
  * 功能函数配置
    FF_FS_READONLY    使能或禁止与写相关函数，即配置只读   0
    FF_USE_MKFS   使能或禁用f_mkfs函数，即是否使能格式化   1
    FF_USE_FASTSEEK   使能快速搜索功能，加快f_lseek/read/write函数执行   1
    FF_USE_LABEL    使能或禁止支持磁盘盘符读取与设置函数    1
    FF_USE_STRFUNC    设置是否支持字符串类操作    1 
    FF_STRF_ENCODE    设置字符串I/O函数读写文件字符编码    0
  * 命名空间和本地环境配置	
    FF_CODE_PAGE    设置语言类型，简体中文设置为963     963
    FF_USE_LFN    使能或禁止长文件名，取值范围(0~3)，存储地方不同    3 
  * 磁盘配置	  
    FF_VOLUMES    设置FATFS支持的逻辑设备数目    2
    FF_MAX_SS   设置最大扇区大小    4096

③ ff.c/ff.h/diskio.h/ffunicode.c/ffsystem.c ……
   FATFS 源代码，不需要修改
   (移植正点原子FATFS程序，正点原子进行了小修改，具体修改什么也没讲)
 
④ exfuns.c/fattester.c 正点原子测试文件，具体是啥也没讲，应该没用到  

*/

/*
常用开放函数：

文件操作：
f_open		打开/创建一个文件
f_close		关闭一个打开的文件
f_read 		从文件中读取数据
f_write		往文件中写数据
f_gets		读一个字符串
f_putc 		写一个字符
f_puts		写一个字符串
f_printf		写一个格式化的字符串
f_lseek		移动文件读/写指针
f_tell		获取当前读/写指针
f_size		获取文件大小

目录操作：
f_opendif		打开一个目录
f_closedir	关闭一个已经打开的目录
f_readdir		读取目录条目
f_mkdir		创建一个新目录
f_unlink		删除一个文件或目录
f_rename		重命名/移动一个文件或文件夹

卷管理：
f_mount		注册/注销一个工作区
f_mkfs		格式化，创建一个文件系统
f_getfree		获取磁盘信息以及空闲簇数量
f_setlabel	设置盘符(磁盘名字)
f_getlabel	获取盘符	
*/

/*

程序输出：

******** 这是一个SPI FLASH 文件系统实验 *******
》文件系统挂载成功，可以进行读写测试

****** 即将进行文件写入测试... ******
》打开/创建FatFs读写测试文件.txt文件成功，向文件写入数据。
》文件写入成功，写入字节数据：63
》向文件写入的数据为：
欢迎使用野火STM32开发板 今天是个好日子，新建文件系统测试文件

****** 即将进行文件读取测试... ******
》打开文件成功。
》文件读取成功,读到字节数据：63
》读取得的文件数据为：
欢迎使用野火STM32开发板 今天是个好日子，新建文件系统测试文件
 

*************** 设备信息获取 ***************
》设备总空间：      5868 KB。
》可用空间：        5856 KB。

******** 文件定位和格式化写入功能测试 ********
》文件内容：
欢迎使用野火STM32开发板 今天是个好日子，新建文件系统测试文件


********** 目录创建和重命名功能测试 **********

*************** 文件信息获取测试 **************
“testdir.txt”文件信息：
》文件大小: 64(字节)
》时间戳: 2015/01/01, 00:00
》属性: ----A

***************** 文件扫描测试 ****************
1:/TestDir/testdir.txt



*/

FATFS fs;													/* FatFs文件系统对象 */
FIL fnew;													/* 文件对象 */
FRESULT res_flash;                /* 文件操作结果 */
UINT fnum;            					  /* 文件成功读写数量 */
char fpath[100];                  /* 保存当前扫描路径 */
char readbuffer[512];    

BYTE work[FF_MAX_SS];//格式化设备工作区
BYTE WriteBuffer[] = "欢迎使用野火STM32开发板 今天是个好日子，新建文件系统测试文件\r\n"; 
BYTE ReadBuffer[1024]={0};        /* 读缓冲区 */

/* FatFs多项功能测试 */
static FRESULT miscellaneous(void)
{
  DIR dir;
  FATFS *pfs;
  DWORD fre_clust, fre_sect, tot_sect;
  
  printf("\n*************** 设备信息获取 ***************\r\n");
  /* 获取设备信息和空簇大小 */
  res_flash = f_getfree("1:", &fre_clust, &pfs);

  /* 计算得到总的扇区个数和空扇区个数 */
  tot_sect = (pfs->n_fatent - 2) * pfs->csize;
  fre_sect = fre_clust * pfs->csize;

  /* 打印信息(4096 字节/扇区) */
  printf("》设备总空间：%10lu KB。\n》可用空间：  %10lu KB。\n", tot_sect *4, fre_sect *4);
  
  printf("\n******** 文件定位和格式化写入功能测试 ********\r\n");
  res_flash = f_open(&fnew, "1:FatFs读写测试文件.txt",
                            FA_OPEN_ALWAYS|FA_WRITE|FA_READ );
	if ( res_flash == FR_OK )
	{
    /*  文件定位 */
    res_flash = f_lseek(&fnew,f_size(&fnew));
    if (res_flash == FR_OK)
    {
      /* 格式化写入，参数格式类似printf函数 */
      f_printf(&fnew,"\n在原来文件新添加一行内容\n");
      f_printf(&fnew,"》设备总空间：%10lu KB。\n》可用空间：  %10lu KB。\n", tot_sect *4, fre_sect *4);
      /*  文件定位到文件起始位置 */
      res_flash = f_lseek(&fnew,0);
      /* 读取文件所有内容到缓存区 */
      res_flash = f_read(&fnew,readbuffer,f_size(&fnew),&fnum);
      if(res_flash == FR_OK)
      {
        printf("》文件内容：\n%s\n",readbuffer);
      }
    }
    f_close(&fnew);    
    
    printf("\n********** 目录创建和重命名功能测试 **********\r\n");
    /* 尝试打开目录 */
    res_flash=f_opendir(&dir,"1:TestDir");
    if(res_flash!=FR_OK)
    {
      /* 打开目录失败，就创建目录 */
      res_flash=f_mkdir("1:TestDir");
    }
    else
    {
      /* 如果目录已经存在，关闭它 */
      res_flash=f_closedir(&dir);
      /* 删除文件 */
      f_unlink("1:TestDir/testdir.txt");
    }
    if(res_flash==FR_OK)
    {
      /* 重命名并移动文件 */
      res_flash=f_rename("1:FatFs读写测试文件.txt","1:TestDir/testdir.txt");      
    } 
	}
  else
  {
    printf("!! 打开文件失败：%d\n",res_flash);
    printf("!! 或许需要再次运行“FatFs移植与读写测试”工程\n");
  }
  return res_flash;
}


FILINFO fno;
/**
  * 文件信息获取
  */
static FRESULT file_check(void)
{ 
  /* 获取文件信息 */
  res_flash=f_stat("1:TestDir/testdir.txt",&fno);
  if(res_flash==FR_OK)
  {
    printf("“testdir.txt”文件信息：\n");
    printf("》文件大小: %ld(字节)\n", fno.fsize);
    printf("》时间戳: %u/%02u/%02u, %02u:%02u\n",
           (fno.fdate >> 9) + 1980, fno.fdate >> 5 & 15, fno.fdate & 31,fno.ftime >> 11, fno.ftime >> 5 & 63);
    printf("》属性: %c%c%c%c%c\n\n",
           (fno.fattrib & AM_DIR) ? 'D' : '-',      // 是一个目录
           (fno.fattrib & AM_RDO) ? 'R' : '-',      // 只读文件
           (fno.fattrib & AM_HID) ? 'H' : '-',      // 隐藏文件
           (fno.fattrib & AM_SYS) ? 'S' : '-',      // 系统文件
           (fno.fattrib & AM_ARC) ? 'A' : '-');     // 档案文件
  }
  return res_flash;
}


/**
  * @brief  scan_files 递归扫描FatFs内的文件
  * @param  path:初始扫描路径
  * @retval result:文件系统的返回值
  */
static FRESULT scan_files (char* path) 
{ 
  FRESULT res; 		//部分在递归过程被修改的变量，不用全局变量	
  FILINFO fno; 
  DIR dir; 
  int i;            
  char *fn;        // 文件名	
	
#if _USE_LFN 
  /* 长文件名支持 */
  /* 简体中文需要2个字节保存一个“字”*/
  static char lfn[_MAX_LFN*2 + 1]; 	
  fno.lfname = lfn; 
  fno.lfsize = sizeof(lfn); 
#endif 
  //打开目录
  res = f_opendir(&dir, path); 
  if (res == FR_OK) 
	{ 
    i = strlen(path); 
    for (;;) 
		{ 
      //读取目录下的内容，再读会自动读下一个文件
      res = f_readdir(&dir, &fno); 								
      //为空时表示所有项目读取完毕，跳出
      if (res != FR_OK || fno.fname[0] == 0) break; 	
#if _USE_LFN 
      fn = *fno.lfname ? fno.lfname : fno.fname; 
#else 
      fn = fno.fname; 
#endif 
      //点表示当前目录，跳过			
      if (*fn == '.') continue; 	
      //目录，递归读取      
      if (fno.fattrib & AM_DIR)         
			{ 			
        //合成完整目录名        
        sprintf(&path[i], "/%s", fn); 		
        //递归遍历         
        res = scan_files(path);	
        path[i] = 0;         
        //打开失败，跳出循环        
        if (res != FR_OK) 
					break; 
      } 
			else 
			{ 
				printf("%s/%s\r\n", path, fn);								//输出文件名	
        /* 可以在这里提取特定格式的文件路径 */        
      }//else
    } //for
  } 
  return res; 
}


/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
  HAL_Init();        
  /* 配置系统时钟为72 MHz */ 
  SystemClock_Config();
  /*初始化USART 配置模式为 115200 8-N-1，中断接收*/
  DEBUG_USART_Config();
  
  printf("******** 这是一个SPI FLASH 文件系统实验 *******\r\n");
  
  //在外部SPI Flash挂载文件系统，文件系统挂载时会对SPI设备初始化
  res_flash = f_mount(&fs,"1:",1);
	
  /*----------------------- 格式化测试 -----------------*/  
	/* 如果没有文件系统就格式化创建创建文件系统 */
	if(res_flash == FR_NO_FILESYSTEM)
	{
		printf("》FLASH还没有文件系统，即将进行格式化...\r\n");
    /* 格式化 */
		res_flash=f_mkfs("1:", 0, work, sizeof(work));						
		
		if(res_flash == FR_OK)
		{
			printf("》FLASH已成功格式化文件系统。\r\n");
      /* 格式化后，先取消挂载 */
			res_flash = f_mount(NULL,"1:",1);			
      /* 重新挂载	*/			
			res_flash = f_mount(&fs,"1:",1);
		}
		else
		{
			printf("《《格式化失败。》》\r\n");
			while(1);
		}
	}
  else if(res_flash!=FR_OK)
  {
    printf("！！外部Flash挂载文件系统失败。(%d)\r\n",res_flash);
    printf("请下载 SPI—读写串行FLASH 例程测试，如果正常，在该例程f_mount语句下if语句前临时多添加一句 res_flash = FR_NO_FILESYSTEM; 让重新直接执行格式化流程\r\n");
		while(1);
  }
  else
  {
    printf("》文件系统挂载成功，可以进行读写测试\r\n");
  }  
  
/*----------------------- 文件系统测试：写测试 -------------------*/
	/* 打开文件，每次都以新建的形式打开，属性为可写 */
	printf("\r\n****** 即将进行文件写入测试... ******\r\n");	
  // 1:表示 挂载在 驱动器0还是驱动器1，SPI_FLASH 驱动器编号为 1
	res_flash = f_open(&fnew, "1:FatFs读写测试文件.txt",FA_OPEN_ALWAYS | FA_WRITE );
	if ( res_flash == FR_OK )
	{
		printf("》打开/创建FatFs读写测试文件.txt文件成功，向文件写入数据。\r\n");
    /* 将指定存储区内容写入到文件内 */
		res_flash=f_write(&fnew,WriteBuffer,sizeof(WriteBuffer),&fnum);
    if(res_flash==FR_OK)
    {
      printf("》文件写入成功，写入字节数据：%d\n",fnum);
      printf("》向文件写入的数据为：\r\n%s\r\n",WriteBuffer);
    }
    else
    {
      printf("！！文件写入失败：(%d)\n",res_flash);
    }    
		/* 不再读写，关闭文件 */
    f_close(&fnew);
	}
	else
	{	
		printf("！！打开/创建文件失败。\r\n");
	}
	
/*------------------- 文件系统测试：读测试 --------------------------*/
	printf("****** 即将进行文件读取测试... ******\r\n");
  // 1:表示 挂载在 驱动器0还是驱动器1，SPI_FLASH 驱动器编号为 1
  res_flash = f_open(&fnew, "1:FatFs读写测试文件.txt",FA_OPEN_ALWAYS | FA_READ); 	 
	if(res_flash == FR_OK)
	{

		printf("》打开文件成功。\r\n");
		res_flash = f_read(&fnew, ReadBuffer, sizeof(ReadBuffer), &fnum); 
    if(res_flash==FR_OK)
    {
      printf("》文件读取成功,读到字节数据：%d\r\n",fnum);
      printf("》读取得的文件数据为：\r\n%s \r\n", ReadBuffer);	
    }
    else
    {
      printf("！！文件读取失败：(%d)\n",res_flash);
    }		
	}
	else
	{
		printf("！！打开文件失败。\r\n");
	}
	/* 不再读写，关闭文件 */
	f_close(&fnew);	  
  
/*-----------------------------------------------------------------------*/
/*-------------------------后面就看不懂了……-----------------------------*/ 
  
  /* FatFs多项功能测试 */
  res_flash = miscellaneous();

  
  printf("\n*************** 文件信息获取测试 **************\r\n");
  res_flash = file_check();

  
  printf("***************** 文件扫描测试 ****************\r\n");
  strcpy(fpath,"1:");
  scan_files(fpath);
  
  
	/* 不再使用文件系统，取消挂载文件系统 */
	f_mount(NULL,"1:",1);
  
  /* 操作完成，停机 */
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
