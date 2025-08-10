/*---------------------------------------------------------------------------/
/  FatFs Functional Configurations
/---------------------------------------------------------------------------*/

#define FFCONF_DEF	86631	/* Revision ID */

/*---------------------------------------------------------------------------/
/ Function Configurations
/---------------------------------------------------------------------------*/

#define FF_FS_READONLY	0
/* This option switches read-only configuration. (0:Read/Write or 1:Read-only)  该选项切换只读配置。(0:Read/Write或1:Read-only)
/  Read-only configuration removes writing API functions, f_write(), f_sync(),  只读配置删除写入API函数，f_write()， f_sync()，
/  f_unlink(), f_mkdir(), f_chmod(), f_rename(), f_truncate(), f_getfree()
/  and optional writing functions as well. 以及可选的写作功能。*/


#define FF_FS_MINIMIZE	0
/* This option defines minimization level to remove some basic API functions. 该选项定义最小化级别以删除一些基本的API函数。
/  
/   0: Basic functions are fully enabled.  基本功能已全部启用。
/   1: f_stat(), f_getfree(), f_unlink(), f_mkdir(), f_truncate() and f_rename()
/      are removed. 被删除
/   2: f_opendir(), f_readdir() and f_closedir() are removed in addition to 1.
/   3: f_lseek() function is removed in addition to 2. */


#define FF_USE_FIND		0
/* This option switches filtered directory read functions, f_findfirst() and
/  f_findnext(). (0:Disable, 1:Enable 2:Enable with matching altname[] too) 
/ 此选项切换过滤目录读取函数 f_findfirst() 和 f_findnext()。（0：禁用，1：启用，2：同时启用匹配的 altname[]）*/


#define FF_USE_MKFS		1
/* This option switches f_mkfs() function. (0:Disable or 1:Enable) 该选项切换 f_mkfs() 函数。*/


#define FF_USE_FASTSEEK	1
/* This option switches fast seek function. (0:Disable or 1:Enable) 该选项可切换快速寻道功能*/


#define FF_USE_EXPAND	0
/* This option switches f_expand function. (0:Disable or 1:Enable) 该选项切换 f_expand 函数。*/


#define FF_USE_CHMOD	0
/* This option switches attribute manipulation functions, f_chmod() and f_utime(). 该选项切换属性操作函数 f_chmod() 和 f_utime()。
/  (0:Disable or 1:Enable) Also FF_FS_READONLY needs to be 0 to enable this option. （0：禁用或 1：启用）此外，FF_FS_READONLY 需要为 0 才能启用此选项。*/


#define FF_USE_LABEL	1
/* This option switches volume label functions, f_getlabel() and f_setlabel(). 此选项切换卷标函数 f_getlabel() 和 f_setlabel()。
/  (0:Disable or 1:Enable) */


#define FF_USE_FORWARD	0
/* This option switches f_forward() function. (0:Disable or 1:Enable) 此选项切换 f_forward() 函数。（0：禁用或 1：启用） */


#define FF_USE_STRFUNC	1
#define FF_PRINT_LLI	0
#define FF_PRINT_FLOAT	0
#define FF_STRF_ENCODE	3
/* FF_USE_STRFUNC switches string functions, f_gets(), f_putc(), f_puts() and
/  f_printf().  FF_USE_STRFUNC 切换字符串函数，f_gets()、f_putc()、f_puts() 和 f_printf()
/
/   0: Disable. FF_PRINT_LLI, FF_PRINT_FLOAT and FF_STRF_ENCODE have no effect.  禁用。FF_PRINT_LLI、FF_PRINT_FLOAT 和 FF_STRF_ENCODE 无效。
/   1: Enable without LF-CRLF conversion.  启用而不进行 LF-CRLF 转换
/   2: Enable with LF-CRLF conversion.  启用 LF-CRLF 转换。
/
/  FF_PRINT_LLI = 1 makes f_printf() support long long argument and FF_PRINT_FLOAT = 1/2
   makes f_printf() support floating point argument. These features want C99 or later.
   FF_PRINT_LLI = 1 使 f_printf() 支持 long long 参数，FF_PRINT_FLOAT = 1/2 使 f_printf() 支持浮点参数。这些功能需要 C99 或更高版本。
/  When FF_LFN_UNICODE >= 1 with LFN enabled, string functions convert the character
/  encoding in it. FF_STRF_ENCODE selects assumption of character encoding ON THE FILE
/  to be read/written via those functions.
/  当 FF_LFN_UNICODE >= 1 且启用 LFN 时，字符串函数会转换其中的字符编码。FF_STRF_ENCODE 选择通过这些函数读取/写入的文件上的字符编码假设。

/   0: ANSI/OEM in current CP
/   1: Unicode in UTF-16LE
/   2: Unicode in UTF-16BE
/   3: Unicode in UTF-8
*/


/*---------------------------------------------------------------------------/
/ Locale and Namespace Configurations
/---------------------------------------------------------------------------*/

#define FF_CODE_PAGE	936
/* This option specifies the OEM code page to be used on the target system. 此选项指定目标系统上要使用的 OEM 代码页。
/  Incorrect code page setting can cause a file open failure. 不正确的代码页设置可能会导致文件打开失败。
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
/   936 - Simplified Chinese (DBCS) 简体中文
/   949 - Korean (DBCS)
/   950 - Traditional Chinese (DBCS)
/     0 - Include all code pages above and configured by f_setcp()
*/


#define FF_USE_LFN		1
#define FF_MAX_LFN		255
/* The FF_USE_LFN switches the support for LFN (long file name). FF_USE_LFN 切换对 LFN（长文件名）的支持。
/
/   0: Disable LFN. FF_MAX_LFN has no effect.  禁用 LFN。FF_MAX_LFN 无效。
/   1: Enable LFN with static  working buffer on the BSS. Always NOT thread-safe.  在 BSS 上使用静态工作缓冲区启用 LFN。始终非线程安全的。
/   2: Enable LFN with dynamic working buffer on the STACK.  在 STACK 上使用动态工作缓冲区启用 LFN。
/   3: Enable LFN with dynamic working buffer on the HEAP.  在 HEAP 上使用动态工作缓冲区启用 LFN。
/
/  To enable the LFN, ffunicode.c needs to be added to the project. The LFN function
/  requiers certain internal working buffer occupies (FF_MAX_LFN + 1) * 2 bytes and
/  additional (FF_MAX_LFN + 44) / 15 * 32 bytes when exFAT is enabled.
/  The FF_MAX_LFN defines size of the working buffer in UTF-16 code unit and it can
/  be in range of 12 to 255. It is recommended to be set it 255 to fully support LFN
/  specification.
/  要启用 LFN，需要将 ffunicode.c 添加到项目中。LFN 函数需要一定的内部工作缓冲区占用 (FF_MAX_LFN + 1) * 2 个字节，
/  启用 exFAT 时还需要占用 (FF_MAX_LFN + 44) / 15 * 32 个字节。FF_MAX_LFN 定义 UTF-16 代码单元中工作缓冲区的大小，
/  范围为 12 到 255。建议将其设置为 255 以完全支持 LFN 规范。
/  When use stack for the working buffer, take care on stack overflow. When use heap
/  memory for the working buffer, memory management functions, ff_memalloc() and
/  ff_memfree() exemplified in ffsystem.c, need to be added to the project.
/ 使用堆栈作为工作缓冲区时，请注意堆栈溢出。
/ 使用堆内存作为工作缓冲区时，需要在工程中添加内存管理函数，ffsystem.c 中示例的 ff_memalloc() 和 ff_memfree()。
*/


#define FF_LFN_UNICODE	0
/* This option switches the character encoding on the API when LFN is enabled.
/  当 LFN 启用时，此选项切换 API 上的字符编码。
/   0: ANSI/OEM in current CP (TCHAR = char)
/   1: Unicode in UTF-16 (TCHAR = WCHAR)
/   2: Unicode in UTF-8 (TCHAR = char)
/   3: Unicode in UTF-32 (TCHAR = DWORD)
/
/  Also behavior of string I/O functions will be affected by this option.
/ 字符串 I/O 函数的行为也会受到此选项的影响。
/  When LFN is not enabled, this option has no effect. 
/ 当 LFN 未启用时，此选项无效。
*/


#define FF_LFN_BUF		255
#define FF_SFN_BUF		12
/* This set of options defines size of file name members in the FILINFO structure
/  which is used to read out directory items. These values should be suffcient for
/  the file names to read. The maximum possible length of the read file name depends
/  on character encoding. When LFN is not enabled, these options have no effect. 
这组选项定义用于读取目录项的 FILINFO 结构中的文件名成员的大小。
这些值应该足以读取文件名。读取文件名的最大可能长度取决于字符编码。当未启用 LFN 时，这些选项不起作用。
*/


#define FF_FS_RPATH		0
/* This option configures support for relative path.
此选项配置对相对路径的支持。
/
/   0: Disable relative path and remove related functions.  禁用相对路径并删除相关功能。
/   1: Enable relative path. f_chdir() and f_chdrive() are available.  启用相对路径。f_chdir() 和 f_chdrive() 可用。
/   2: f_getcwd() function is available in addition to 1. 除 1 之外，还有 f_getcwd() 函数可用。
*/


/*---------------------------------------------------------------------------/
/ Drive/Volume Configurations
/---------------------------------------------------------------------------*/

#define FF_VOLUMES		2   /* 支持2个磁盘 */
/* Number of volumes (logical drives) to be used. (1-10) 要使用的卷（逻辑驱动器）的数量。*/


#define FF_STR_VOLUME_ID	0
#define FF_VOLUME_STRS		"RAM","NAND","CF","SD","SD2","USB","USB2","USB3"
/* FF_STR_VOLUME_ID switches support for volume ID in arbitrary strings. FF_STR_VOLUME_ID 开关支持任意字符串中的卷 ID。
/  When FF_STR_VOLUME_ID is set to 1 or 2, arbitrary strings can be used as drive
/  number in the path name. FF_VOLUME_STRS defines the volume ID strings for each
/  logical drives. Number of items must not be less than FF_VOLUMES. Valid
/  characters for the volume ID strings are A-Z, a-z and 0-9, however, they are
/  compared in case-insensitive. If FF_STR_VOLUME_ID >= 1 and FF_VOLUME_STRS is
/  not defined, a user defined volume string table needs to be defined as:
/  FF_STR_VOLUME_ID 开关支持任意字符串中的卷 ID。当 FF_STR_VOLUME_ID 设置为 1 或 2 时，
  任意字符串可用作路径名中的驱动器号。FF_VOLUME_STRS 定义每个逻辑驱动器的卷 ID 字符串。
  项目数不得少于 FF_VOLUMES。卷 ID 字符串的有效字符为 A-Z、a-z 和 0-9，但它们不区分大小写。
  如果 FF_STR_VOLUME_ID >= 1 且未定义 FF_VOLUME_STRS，则需要定义用户定义的卷字符串表，如下所示：
/  const char* VolumeStr[FF_VOLUMES] = {"ram","flash","sd","usb",...
*/


#define FF_MULTI_PARTITION	0
/* This option switches support for multiple volumes on the physical drive.  此选项切换对物理驱动器上的多个卷的支持。
/  By default (0), each logical drive number is bound to the same physical drive
/  number and only an FAT volume found on the physical drive will be mounted.
/  When this function is enabled (1), each logical drive number can be bound to
/  arbitrary physical drive and partition listed in the VolToPart[]. Also f_fdisk()
/  funciton will be available.
   默认情况下 (0)，每个逻辑驱动器号都绑定到相同的物理驱动器号，并且只会安装物理驱动器上找到的 FAT 卷。
   启用此功能 (1) 后，每个逻辑驱动器号都可以绑定到 VolToPart[] 中列出的任意物理驱动器和分区。此外，f_fdisk() 函数也将可用。
*/


#define FF_MIN_SS		512
#define FF_MAX_SS		4096
/* This set of options configures the range of sector size to be supported. (512,
/  1024, 2048 or 4096) Always set both 512 for most systems, generic memory card and
/  harddisk, but a larger value may be required for on-board flash memory and some
/  type of optical media. When FF_MAX_SS is larger than FF_MIN_SS, FatFs is configured
/  for variable sector size mode and disk_ioctl() function needs to implement
/  GET_SECTOR_SIZE command. 
这组选项配置了要支持的扇区大小范围。（512、1024、2048 或 4096）对于大多数系统、通用存储卡和硬盘，始终将两者设置为 512，
但对于板载闪存和某些类型的光学介质，可能需要更大的值。
当 FF_MAX_SS 大于 FF_MIN_SS 时，FatFs 配置为可变扇区大小模式，并且 disk_ioctl() 函数需要执行 GET_SECTOR_SIZE 命令。

*/


#define FF_LBA64		0
/* This option switches support for 64-bit LBA. (0:Disable or 1:Enable)  此选项切换对 64 位 LBA 的支持。（0：禁用或 1：启用）
/  To enable the 64-bit LBA, also exFAT needs to be enabled. (FF_FS_EXFAT == 1)要启用 64 位 LBA，还需要启用 exFAT。（FF_FS_EXFAT == 1） */


#define FF_MIN_GPT		0x10000000
/* Minimum number of sectors to switch GPT as partitioning format in f_mkfs and
/  f_fdisk function. 0x100000000 max. This option has no effect when FF_LBA64 == 0.
在 f_mkfs 和 f_fdisk 函数中切换 GPT 作为分区格式的最小扇区数。最大 0x100000000。当 FF_LBA64 == 0 时，此选项无效。
*/


#define FF_USE_TRIM		0
/* This option switches support for ATA-TRIM. (0:Disable or 1:Enable)  此选项切换对 ATA-TRIM 的支持。（0：禁用或 1：启用）
/  To enable Trim function, also CTRL_TRIM command should be implemented to the
/  disk_ioctl() function. 
此选项切换对 ATA-TRIM 的支持。（0：禁用或 1：启用）要启用 Trim 功能，还应将 CTRL_TRIM 命令实施到 disk_ioctl() 函数中。
*/



/*---------------------------------------------------------------------------/
/ System Configurations
/---------------------------------------------------------------------------*/

#define FF_FS_TINY		0
/* This option switches tiny buffer configuration. (0:Normal or 1:Tiny)  此选项切换微小缓冲区配置。（0：正常或 1：微小）
/  At the tiny configuration, size of file object (FIL) is shrinked FF_MAX_SS bytes. 在微小配置下，文件对象（FIL）的大小缩小了 FF_MAX_SS 字节。
/  Instead of private sector buffer eliminated from the file object, common sector
/  buffer in the filesystem object (FATFS) is used for the file data transfer. 
不使用从文件对象中删除的私有扇区缓冲区，而是使用文件系统对象 (FATFS) 中的公共扇区缓冲区进行文件数据传输。
*/


#define FF_FS_EXFAT		1
/* This option switches support for exFAT filesystem. (0:Disable or 1:Enable)  此选项切换对 exFAT 文件系统的支持。（0：禁用或 1：启用）
/  To enable exFAT, also LFN needs to be enabled. (FF_USE_LFN >= 1) 要启用 exFAT，还需要启用 LFN。（FF_USE_LFN >= 1）请注意，启用 exFAT 会放弃 ANSI C（C89）兼容性。
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
  选项 FF_FS_NORTC 可切换时间戳功能。如果系统没有任何 RTC 功能或不需要有效时间戳，请设置 FF_FS_NORTC = 1 以禁用时间戳功能。
  FatFs 修改的每个对象都将具有由 FF_NORTC_MON、FF_NORTC_MDAY 和 FF_NORTC_YEAR 以本地时间定义的固定时间戳。
  要启用时间戳功能（FF_FS_NORTC = 0），需要在项目中添加 get_fattime() 函数以从实时时钟读取当前时间。
  FF_NORTC_MON、FF_NORTC_MDAY 和 FF_NORTC_YEAR 无效。这些选项在只读配置（FF_FS_READONLY = 1）中无效
*/


#define FF_FS_NOFSINFO	0
/* If you need to know correct free space on the FAT32 volume, set bit 0 of this
/  option, and f_getfree() function at first time after volume mount will force
/  a full FAT scan. Bit 1 controls the use of last allocated cluster number.
/  如果您需要知道 FAT32 卷上的正确可用空间，请设置此选项的位 0，卷安装后第一次使用 f_getfree() 函数将强制进行完整的 FAT 扫描。位 1 控制最后分配的簇号的使用。
/  bit0=0: Use free cluster count in the FSINFO if available. bit0=0：如果可用，则使用 FSINFO 中的空闲簇数。
/  bit0=1: Do not trust free cluster count in the FSINFO. bit0=1：不信任 FSINFO 中的空闲簇数。
/  bit1=0: Use last allocated cluster number in the FSINFO if available. bit1=0：如果可用，则使用 FSINFO 中最后分配的簇号。
/  bit1=1: Do not trust last allocated cluster number in the FSINFO. bit1=1：不信任 FSINFO 中最后分配的簇号。
*/


#define FF_FS_LOCK		0
/* The option FF_FS_LOCK switches file lock function to control duplicated file open
/  and illegal operation to open objects. This option must be 0 when FF_FS_READONLY
/  is 1.  选项 FF_FS_LOCK 切换文件锁定功能以控制重复打开文件和对打开对象的非法操作。当 FF_FS_READONLY 为 1 时，此选项必须为 0。
/
/  0:  Disable file lock function. To avoid volume corruption, application program
/      should avoid illegal open, remove and rename to the open objects.  禁用文件锁定功能。为避免卷损坏，应用程序应避免对打开的对象进行非法打开、删除和重命名。
/  >0: Enable file lock function. The value defines how many files/sub-directories
/      can be opened simultaneously under file lock control. Note that the file
/      lock control is independent of re-entrancy. 
启用文件锁定功能。该值定义在文件锁定控制下可以同时打开多少个文件/子目录。请注意，文件锁定控制与重入无关。
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
/ 选项 FF_FS_REENTRANT 可切换 FatFs 模块本身的可重入性（线程安全）。
  请注意，无论此选项如何，对不同卷的文件访问始终是可重入的，而卷控制函数 f_mount()、f_mkfs() 和 f_fdisk() 函数始终不可重入。
  只有对同一卷的文件/目录访问受此函数控制。
/   0: Disable re-entrancy. FF_FS_TIMEOUT and FF_SYNC_t have no effect.  禁用重入。FF_FS_TIMEOUT 和 FF_SYNC_t 无效。
/   1: Enable re-entrancy. Also user provided synchronization handlers,
/      ff_req_grant(), ff_rel_grant(), ff_del_syncobj() and ff_cre_syncobj()
/      function, must be added to the project. Samples are available in
/      option/syscall.c.  
启用重入。此外，用户提供的同步处理程序 ff_req_grant()、ff_rel_grant()、ff_del_syncobj() 和 ff_cre_syncobj() 函数也必须添加到项目中。示例可在 option/syscall.c 中找到。
/
/  The FF_FS_TIMEOUT defines timeout period in unit of time tick.
/  The FF_SYNC_t defines O/S dependent sync object type. e.g. HANDLE, ID, OS_EVENT*,
/  SemaphoreHandle_t and etc. A header file for O/S definitions needs to be
/  included somewhere in the scope of ff.h. 
/ FF_FS_TIMEOUT 以时间单位定义超时时间。
FF_SYNC_t 定义依赖于操作系统的同步对象类型。
例如 HANDLE、ID、OS_EVENT*、SemaphoreHandle_t 等。
需要在 ff.h 范围内的某个位置包含操作系统定义的头文件。
*/



/*--- End of configuration options ---*/
