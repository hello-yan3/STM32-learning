/*---------------------------------------------------------------------------/
/  FatFs Functional Configurations
/---------------------------------------------------------------------------*/

#define FFCONF_DEF	86631	/* Revision ID */

/*---------------------------------------------------------------------------/
/ Function Configurations
/---------------------------------------------------------------------------*/

#define FF_FS_READONLY	0
/* This option switches read-only configuration. (0:Read/Write or 1:Read-only)  ��ѡ���л�ֻ�����á�(0:Read/Write��1:Read-only)
/  Read-only configuration removes writing API functions, f_write(), f_sync(),  ֻ������ɾ��д��API������f_write()�� f_sync()��
/  f_unlink(), f_mkdir(), f_chmod(), f_rename(), f_truncate(), f_getfree()
/  and optional writing functions as well. �Լ���ѡ��д�����ܡ�*/


#define FF_FS_MINIMIZE	0
/* This option defines minimization level to remove some basic API functions. ��ѡ�����С��������ɾ��һЩ������API������
/  
/   0: Basic functions are fully enabled.  ����������ȫ�����á�
/   1: f_stat(), f_getfree(), f_unlink(), f_mkdir(), f_truncate() and f_rename()
/      are removed. ��ɾ��
/   2: f_opendir(), f_readdir() and f_closedir() are removed in addition to 1.
/   3: f_lseek() function is removed in addition to 2. */


#define FF_USE_FIND		0
/* This option switches filtered directory read functions, f_findfirst() and
/  f_findnext(). (0:Disable, 1:Enable 2:Enable with matching altname[] too) 
/ ��ѡ���л�����Ŀ¼��ȡ���� f_findfirst() �� f_findnext()����0�����ã�1�����ã�2��ͬʱ����ƥ��� altname[]��*/


#define FF_USE_MKFS		1
/* This option switches f_mkfs() function. (0:Disable or 1:Enable) ��ѡ���л� f_mkfs() ������*/


#define FF_USE_FASTSEEK	1
/* This option switches fast seek function. (0:Disable or 1:Enable) ��ѡ����л�����Ѱ������*/


#define FF_USE_EXPAND	0
/* This option switches f_expand function. (0:Disable or 1:Enable) ��ѡ���л� f_expand ������*/


#define FF_USE_CHMOD	0
/* This option switches attribute manipulation functions, f_chmod() and f_utime(). ��ѡ���л����Բ������� f_chmod() �� f_utime()��
/  (0:Disable or 1:Enable) Also FF_FS_READONLY needs to be 0 to enable this option. ��0�����û� 1�����ã����⣬FF_FS_READONLY ��ҪΪ 0 �������ô�ѡ�*/


#define FF_USE_LABEL	1
/* This option switches volume label functions, f_getlabel() and f_setlabel(). ��ѡ���л���꺯�� f_getlabel() �� f_setlabel()��
/  (0:Disable or 1:Enable) */


#define FF_USE_FORWARD	0
/* This option switches f_forward() function. (0:Disable or 1:Enable) ��ѡ���л� f_forward() ��������0�����û� 1�����ã� */


#define FF_USE_STRFUNC	1
#define FF_PRINT_LLI	0
#define FF_PRINT_FLOAT	0
#define FF_STRF_ENCODE	3
/* FF_USE_STRFUNC switches string functions, f_gets(), f_putc(), f_puts() and
/  f_printf().  FF_USE_STRFUNC �л��ַ���������f_gets()��f_putc()��f_puts() �� f_printf()
/
/   0: Disable. FF_PRINT_LLI, FF_PRINT_FLOAT and FF_STRF_ENCODE have no effect.  ���á�FF_PRINT_LLI��FF_PRINT_FLOAT �� FF_STRF_ENCODE ��Ч��
/   1: Enable without LF-CRLF conversion.  ���ö������� LF-CRLF ת��
/   2: Enable with LF-CRLF conversion.  ���� LF-CRLF ת����
/
/  FF_PRINT_LLI = 1 makes f_printf() support long long argument and FF_PRINT_FLOAT = 1/2
   makes f_printf() support floating point argument. These features want C99 or later.
   FF_PRINT_LLI = 1 ʹ f_printf() ֧�� long long ������FF_PRINT_FLOAT = 1/2 ʹ f_printf() ֧�ָ����������Щ������Ҫ C99 ����߰汾��
/  When FF_LFN_UNICODE >= 1 with LFN enabled, string functions convert the character
/  encoding in it. FF_STRF_ENCODE selects assumption of character encoding ON THE FILE
/  to be read/written via those functions.
/  �� FF_LFN_UNICODE >= 1 ������ LFN ʱ���ַ���������ת�����е��ַ����롣FF_STRF_ENCODE ѡ��ͨ����Щ������ȡ/д����ļ��ϵ��ַ�������衣

/   0: ANSI/OEM in current CP
/   1: Unicode in UTF-16LE
/   2: Unicode in UTF-16BE
/   3: Unicode in UTF-8
*/


/*---------------------------------------------------------------------------/
/ Locale and Namespace Configurations
/---------------------------------------------------------------------------*/

#define FF_CODE_PAGE	936
/* This option specifies the OEM code page to be used on the target system. ��ѡ��ָ��Ŀ��ϵͳ��Ҫʹ�õ� OEM ����ҳ��
/  Incorrect code page setting can cause a file open failure. ����ȷ�Ĵ���ҳ���ÿ��ܻᵼ���ļ���ʧ�ܡ�
/
/   437 - U.S.
/   720 - Arabic
/   737 - Greek
/   771 - KBL
/   775 - Baltic
/   850 - Latin 1
/   852 - Latin 2
/   855 - Cyrillic
/   857 - Turkish
/   860 - Portuguese
/   861 - Icelandic
/   862 - Hebrew
/   863 - Canadian French
/   864 - Arabic
/   865 - Nordic
/   866 - Russian
/   869 - Greek 2
/   932 - Japanese (DBCS)
/   936 - Simplified Chinese (DBCS) ��������
/   949 - Korean (DBCS)
/   950 - Traditional Chinese (DBCS)
/     0 - Include all code pages above and configured by f_setcp()
*/


#define FF_USE_LFN		1
#define FF_MAX_LFN		255
/* The FF_USE_LFN switches the support for LFN (long file name). FF_USE_LFN �л��� LFN�����ļ�������֧�֡�
/
/   0: Disable LFN. FF_MAX_LFN has no effect.  ���� LFN��FF_MAX_LFN ��Ч��
/   1: Enable LFN with static  working buffer on the BSS. Always NOT thread-safe.  �� BSS ��ʹ�þ�̬�������������� LFN��ʼ�շ��̰߳�ȫ�ġ�
/   2: Enable LFN with dynamic working buffer on the STACK.  �� STACK ��ʹ�ö�̬�������������� LFN��
/   3: Enable LFN with dynamic working buffer on the HEAP.  �� HEAP ��ʹ�ö�̬�������������� LFN��
/
/  To enable the LFN, ffunicode.c needs to be added to the project. The LFN function
/  requiers certain internal working buffer occupies (FF_MAX_LFN + 1) * 2 bytes and
/  additional (FF_MAX_LFN + 44) / 15 * 32 bytes when exFAT is enabled.
/  The FF_MAX_LFN defines size of the working buffer in UTF-16 code unit and it can
/  be in range of 12 to 255. It is recommended to be set it 255 to fully support LFN
/  specification.
/  Ҫ���� LFN����Ҫ�� ffunicode.c ��ӵ���Ŀ�С�LFN ������Ҫһ�����ڲ�����������ռ�� (FF_MAX_LFN + 1) * 2 ���ֽڣ�
/  ���� exFAT ʱ����Ҫռ�� (FF_MAX_LFN + 44) / 15 * 32 ���ֽڡ�FF_MAX_LFN ���� UTF-16 ���뵥Ԫ�й����������Ĵ�С��
/  ��ΧΪ 12 �� 255�����齫������Ϊ 255 ����ȫ֧�� LFN �淶��
/  When use stack for the working buffer, take care on stack overflow. When use heap
/  memory for the working buffer, memory management functions, ff_memalloc() and
/  ff_memfree() exemplified in ffsystem.c, need to be added to the project.
/ ʹ�ö�ջ��Ϊ����������ʱ����ע���ջ�����
/ ʹ�ö��ڴ���Ϊ����������ʱ����Ҫ�ڹ���������ڴ��������ffsystem.c ��ʾ���� ff_memalloc() �� ff_memfree()��
*/


#define FF_LFN_UNICODE	0
/* This option switches the character encoding on the API when LFN is enabled.
/  �� LFN ����ʱ����ѡ���л� API �ϵ��ַ����롣
/   0: ANSI/OEM in current CP (TCHAR = char)
/   1: Unicode in UTF-16 (TCHAR = WCHAR)
/   2: Unicode in UTF-8 (TCHAR = char)
/   3: Unicode in UTF-32 (TCHAR = DWORD)
/
/  Also behavior of string I/O functions will be affected by this option.
/ �ַ��� I/O ��������ΪҲ���ܵ���ѡ���Ӱ�졣
/  When LFN is not enabled, this option has no effect. 
/ �� LFN δ����ʱ����ѡ����Ч��
*/


#define FF_LFN_BUF		255
#define FF_SFN_BUF		12
/* This set of options defines size of file name members in the FILINFO structure
/  which is used to read out directory items. These values should be suffcient for
/  the file names to read. The maximum possible length of the read file name depends
/  on character encoding. When LFN is not enabled, these options have no effect. 
����ѡ������ڶ�ȡĿ¼��� FILINFO �ṹ�е��ļ�����Ա�Ĵ�С��
��ЩֵӦ�����Զ�ȡ�ļ�������ȡ�ļ����������ܳ���ȡ�����ַ����롣��δ���� LFN ʱ����Щѡ������á�
*/


#define FF_FS_RPATH		0
/* This option configures support for relative path.
��ѡ�����ö����·����֧�֡�
/
/   0: Disable relative path and remove related functions.  �������·����ɾ����ع��ܡ�
/   1: Enable relative path. f_chdir() and f_chdrive() are available.  �������·����f_chdir() �� f_chdrive() ���á�
/   2: f_getcwd() function is available in addition to 1. �� 1 ֮�⣬���� f_getcwd() �������á�
*/


/*---------------------------------------------------------------------------/
/ Drive/Volume Configurations
/---------------------------------------------------------------------------*/

#define FF_VOLUMES		2   /* ֧��2������ */
/* Number of volumes (logical drives) to be used. (1-10) Ҫʹ�õľ��߼�����������������*/


#define FF_STR_VOLUME_ID	0
#define FF_VOLUME_STRS		"RAM","NAND","CF","SD","SD2","USB","USB2","USB3"
/* FF_STR_VOLUME_ID switches support for volume ID in arbitrary strings. FF_STR_VOLUME_ID ����֧�������ַ����еľ� ID��
/  When FF_STR_VOLUME_ID is set to 1 or 2, arbitrary strings can be used as drive
/  number in the path name. FF_VOLUME_STRS defines the volume ID strings for each
/  logical drives. Number of items must not be less than FF_VOLUMES. Valid
/  characters for the volume ID strings are A-Z, a-z and 0-9, however, they are
/  compared in case-insensitive. If FF_STR_VOLUME_ID >= 1 and FF_VOLUME_STRS is
/  not defined, a user defined volume string table needs to be defined as:
/  FF_STR_VOLUME_ID ����֧�������ַ����еľ� ID���� FF_STR_VOLUME_ID ����Ϊ 1 �� 2 ʱ��
  �����ַ���������·�����е��������š�FF_VOLUME_STRS ����ÿ���߼��������ľ� ID �ַ�����
  ��Ŀ���������� FF_VOLUMES���� ID �ַ�������Ч�ַ�Ϊ A-Z��a-z �� 0-9�������ǲ����ִ�Сд��
  ��� FF_STR_VOLUME_ID >= 1 ��δ���� FF_VOLUME_STRS������Ҫ�����û�����ľ��ַ�����������ʾ��
/  const char* VolumeStr[FF_VOLUMES] = {"ram","flash","sd","usb",...
*/


#define FF_MULTI_PARTITION	0
/* This option switches support for multiple volumes on the physical drive.  ��ѡ���л��������������ϵĶ�����֧�֡�
/  By default (0), each logical drive number is bound to the same physical drive
/  number and only an FAT volume found on the physical drive will be mounted.
/  When this function is enabled (1), each logical drive number can be bound to
/  arbitrary physical drive and partition listed in the VolToPart[]. Also f_fdisk()
/  funciton will be available.
   Ĭ������� (0)��ÿ���߼��������Ŷ��󶨵���ͬ�������������ţ�����ֻ�ᰲװ�������������ҵ��� FAT ��
   ���ô˹��� (1) ��ÿ���߼��������Ŷ����԰󶨵� VolToPart[] ���г������������������ͷ��������⣬f_fdisk() ����Ҳ�����á�
*/


#define FF_MIN_SS		512
#define FF_MAX_SS		4096
/* This set of options configures the range of sector size to be supported. (512,
/  1024, 2048 or 4096) Always set both 512 for most systems, generic memory card and
/  harddisk, but a larger value may be required for on-board flash memory and some
/  type of optical media. When FF_MAX_SS is larger than FF_MIN_SS, FatFs is configured
/  for variable sector size mode and disk_ioctl() function needs to implement
/  GET_SECTOR_SIZE command. 
����ѡ��������Ҫ֧�ֵ�������С��Χ����512��1024��2048 �� 4096�����ڴ����ϵͳ��ͨ�ô洢����Ӳ�̣�ʼ�ս���������Ϊ 512��
�����ڰ��������ĳЩ���͵Ĺ�ѧ���ʣ�������Ҫ�����ֵ��
�� FF_MAX_SS ���� FF_MIN_SS ʱ��FatFs ����Ϊ�ɱ�������Сģʽ������ disk_ioctl() ������Ҫִ�� GET_SECTOR_SIZE ���

*/


#define FF_LBA64		0
/* This option switches support for 64-bit LBA. (0:Disable or 1:Enable)  ��ѡ���л��� 64 λ LBA ��֧�֡���0�����û� 1�����ã�
/  To enable the 64-bit LBA, also exFAT needs to be enabled. (FF_FS_EXFAT == 1)Ҫ���� 64 λ LBA������Ҫ���� exFAT����FF_FS_EXFAT == 1�� */


#define FF_MIN_GPT		0x10000000
/* Minimum number of sectors to switch GPT as partitioning format in f_mkfs and
/  f_fdisk function. 0x100000000 max. This option has no effect when FF_LBA64 == 0.
�� f_mkfs �� f_fdisk �������л� GPT ��Ϊ������ʽ����С����������� 0x100000000���� FF_LBA64 == 0 ʱ����ѡ����Ч��
*/


#define FF_USE_TRIM		0
/* This option switches support for ATA-TRIM. (0:Disable or 1:Enable)  ��ѡ���л��� ATA-TRIM ��֧�֡���0�����û� 1�����ã�
/  To enable Trim function, also CTRL_TRIM command should be implemented to the
/  disk_ioctl() function. 
��ѡ���л��� ATA-TRIM ��֧�֡���0�����û� 1�����ã�Ҫ���� Trim ���ܣ���Ӧ�� CTRL_TRIM ����ʵʩ�� disk_ioctl() �����С�
*/



/*---------------------------------------------------------------------------/
/ System Configurations
/---------------------------------------------------------------------------*/

#define FF_FS_TINY		0
/* This option switches tiny buffer configuration. (0:Normal or 1:Tiny)  ��ѡ���л�΢С���������á���0�������� 1��΢С��
/  At the tiny configuration, size of file object (FIL) is shrinked FF_MAX_SS bytes. ��΢С�����£��ļ�����FIL���Ĵ�С��С�� FF_MAX_SS �ֽڡ�
/  Instead of private sector buffer eliminated from the file object, common sector
/  buffer in the filesystem object (FATFS) is used for the file data transfer. 
��ʹ�ô��ļ�������ɾ����˽������������������ʹ���ļ�ϵͳ���� (FATFS) �еĹ������������������ļ����ݴ��䡣
*/


#define FF_FS_EXFAT		1
/* This option switches support for exFAT filesystem. (0:Disable or 1:Enable)  ��ѡ���л��� exFAT �ļ�ϵͳ��֧�֡���0�����û� 1�����ã�
/  To enable exFAT, also LFN needs to be enabled. (FF_USE_LFN >= 1) Ҫ���� exFAT������Ҫ���� LFN����FF_USE_LFN >= 1����ע�⣬���� exFAT ����� ANSI C��C89�������ԡ�
/  Note that enabling exFAT discards ANSI C (C89) compatibility. */


#define FF_FS_NORTC		0
#define FF_NORTC_MON	1
#define FF_NORTC_MDAY	1
#define FF_NORTC_YEAR	2020
/* The option FF_FS_NORTC switches timestamp functiton. If the system does not have
/  any RTC function or valid timestamp is not needed, set FF_FS_NORTC = 1 to disable
/  the timestamp function. Every object modified by FatFs will have a fixed timestamp
/  defined by FF_NORTC_MON, FF_NORTC_MDAY and FF_NORTC_YEAR in local time.
/  To enable timestamp function (FF_FS_NORTC = 0), get_fattime() function need to be
/  added to the project to read current time form real-time clock. FF_NORTC_MON,
/  FF_NORTC_MDAY and FF_NORTC_YEAR have no effect.
/  These options have no effect in read-only configuration (FF_FS_READONLY = 1). 
  ѡ�� FF_FS_NORTC ���л�ʱ������ܡ����ϵͳû���κ� RTC ���ܻ���Ҫ��Чʱ����������� FF_FS_NORTC = 1 �Խ���ʱ������ܡ�
  FatFs �޸ĵ�ÿ�����󶼽������� FF_NORTC_MON��FF_NORTC_MDAY �� FF_NORTC_YEAR �Ա���ʱ�䶨��Ĺ̶�ʱ�����
  Ҫ����ʱ������ܣ�FF_FS_NORTC = 0������Ҫ����Ŀ����� get_fattime() �����Դ�ʵʱʱ�Ӷ�ȡ��ǰʱ�䡣
  FF_NORTC_MON��FF_NORTC_MDAY �� FF_NORTC_YEAR ��Ч����Щѡ����ֻ�����ã�FF_FS_READONLY = 1������Ч
*/


#define FF_FS_NOFSINFO	0
/* If you need to know correct free space on the FAT32 volume, set bit 0 of this
/  option, and f_getfree() function at first time after volume mount will force
/  a full FAT scan. Bit 1 controls the use of last allocated cluster number.
/  �������Ҫ֪�� FAT32 ���ϵ���ȷ���ÿռ䣬�����ô�ѡ���λ 0����װ���һ��ʹ�� f_getfree() ������ǿ�ƽ��������� FAT ɨ�衣λ 1 ����������Ĵغŵ�ʹ�á�
/  bit0=0: Use free cluster count in the FSINFO if available. bit0=0��������ã���ʹ�� FSINFO �еĿ��д�����
/  bit0=1: Do not trust free cluster count in the FSINFO. bit0=1�������� FSINFO �еĿ��д�����
/  bit1=0: Use last allocated cluster number in the FSINFO if available. bit1=0��������ã���ʹ�� FSINFO ��������Ĵغš�
/  bit1=1: Do not trust last allocated cluster number in the FSINFO. bit1=1�������� FSINFO ��������Ĵغš�
*/


#define FF_FS_LOCK		0
/* The option FF_FS_LOCK switches file lock function to control duplicated file open
/  and illegal operation to open objects. This option must be 0 when FF_FS_READONLY
/  is 1.  ѡ�� FF_FS_LOCK �л��ļ����������Կ����ظ����ļ��ͶԴ򿪶���ķǷ��������� FF_FS_READONLY Ϊ 1 ʱ����ѡ�����Ϊ 0��
/
/  0:  Disable file lock function. To avoid volume corruption, application program
/      should avoid illegal open, remove and rename to the open objects.  �����ļ��������ܡ�Ϊ������𻵣�Ӧ�ó���Ӧ����Դ򿪵Ķ�����зǷ��򿪡�ɾ������������
/  >0: Enable file lock function. The value defines how many files/sub-directories
/      can be opened simultaneously under file lock control. Note that the file
/      lock control is independent of re-entrancy. 
�����ļ��������ܡ���ֵ�������ļ����������¿���ͬʱ�򿪶��ٸ��ļ�/��Ŀ¼����ע�⣬�ļ����������������޹ء�
*/


/* #include <somertos.h>	// O/S definitions */
#define FF_FS_REENTRANT	0
#define FF_FS_TIMEOUT	1000
#define FF_SYNC_t		HANDLE
/* The option FF_FS_REENTRANT switches the re-entrancy (thread safe) of the FatFs
/  module itself. Note that regardless of this option, file access to different
/  volume is always re-entrant and volume control functions, f_mount(), f_mkfs()
/  and f_fdisk() function, are always not re-entrant. Only file/directory access
/  to the same volume is under control of this function.
/ ѡ�� FF_FS_REENTRANT ���л� FatFs ģ�鱾��Ŀ������ԣ��̰߳�ȫ����
  ��ע�⣬���۴�ѡ����Σ��Բ�ͬ����ļ�����ʼ���ǿ�����ģ�������ƺ��� f_mount()��f_mkfs() �� f_fdisk() ����ʼ�ղ������롣
  ֻ�ж�ͬһ����ļ�/Ŀ¼�����ܴ˺������ơ�
/   0: Disable re-entrancy. FF_FS_TIMEOUT and FF_SYNC_t have no effect.  �������롣FF_FS_TIMEOUT �� FF_SYNC_t ��Ч��
/   1: Enable re-entrancy. Also user provided synchronization handlers,
/      ff_req_grant(), ff_rel_grant(), ff_del_syncobj() and ff_cre_syncobj()
/      function, must be added to the project. Samples are available in
/      option/syscall.c.  
�������롣���⣬�û��ṩ��ͬ��������� ff_req_grant()��ff_rel_grant()��ff_del_syncobj() �� ff_cre_syncobj() ����Ҳ������ӵ���Ŀ�С�ʾ������ option/syscall.c ���ҵ���
/
/  The FF_FS_TIMEOUT defines timeout period in unit of time tick.
/  The FF_SYNC_t defines O/S dependent sync object type. e.g. HANDLE, ID, OS_EVENT*,
/  SemaphoreHandle_t and etc. A header file for O/S definitions needs to be
/  included somewhere in the scope of ff.h. 
/ FF_FS_TIMEOUT ��ʱ�䵥λ���峬ʱʱ�䡣
FF_SYNC_t ���������ڲ���ϵͳ��ͬ���������͡�
���� HANDLE��ID��OS_EVENT*��SemaphoreHandle_t �ȡ�
��Ҫ�� ff.h ��Χ�ڵ�ĳ��λ�ð�������ϵͳ�����ͷ�ļ���
*/



/*--- End of configuration options ---*/
