#ifndef __HI_DRV_MMZ_H__
#define __HI_DRV_MMZ_H__

/* add include here */
#include <linux/version.h>

#include <linux/timer.h>
#include <linux/wait.h>
#include <linux/vmalloc.h>
//#include <linux/config.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/fcntl.h>
#include <linux/mm.h>
#include <linux/miscdevice.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>
#include <linux/slab.h>
//#include <linux/devfs_fs_kernel.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/seq_file.h>
#include <linux/list.h>

#include <asm/uaccess.h>
//#include <asm/hardware.h>
#include <asm/io.h>
#include <asm/system.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/string.h>
//#include <asm/arch/arm/mach-x5hd/include/mach/hardware.h>
//#include <linux/autoconf.h>
//#include <asm/sizes.h>
#include "hi_type.h"

#ifdef __cplusplus
 #if __cplusplus
extern "C" {
 #endif
#endif /* __cplusplus */

/** @addtogroup H_MMZ */
/** @{ */

/***************************** Macro Definition ******************************/
#define MMZ_OTHERS      NULL

/*************************** Structure Definition ****************************/

/*media memory map structure*/
typedef struct hiMMZ_BUFFER_S
{
    HI_U32 u32StartVirAddr;
    HI_U32 u32StartPhyAddr;
    HI_U32 u32Size;
}MMZ_BUFFER_S;


/********************** Global Variable declaration **************************/

/******************************* API declaration *****************************/
/*alloc mmz memory, get physic address and map kernel-state address*/
/*CNcomment:申请mmz内存，得到物理地址，并做内核态地址的映射*/
HI_S32  HI_DRV_MMZ_AllocAndMap(const char *name, char *mmzzonename, HI_U32 size, int align, MMZ_BUFFER_S *psMBuf);

/*unmap kernel-state address, release mmz memory*/
/*CNcomment:解除内核态地址的映射，并释放mmz内存*/
HI_VOID HI_DRV_MMZ_UnmapAndRelease(MMZ_BUFFER_S *psMBuf);

/*Only alloc mmz memory, return physic address, but not map kernel-state address*/
/*CNcomment:只申请mmz内存，返回物理地址，不做内核态地址的映射*/
HI_S32  HI_DRV_MMZ_Alloc(const char *bufname, char *zone_name, HI_U32 size, int align, MMZ_BUFFER_S *psMBuf);

/*map kernel-state address after alloc mmz memory for cache, and flushing cache with HI_DRV_MMZ_Flush*/
/*CNcomment:申请mmz可Cache内存后，进行内核态地址的映射, 并使用HI_DRV_MMZ_Flush进行cache同步 */
HI_S32 HI_DRV_MMZ_MapCache(MMZ_BUFFER_S *psMBuf);

/*flush cache data to memory, needed to call when map memory with HI_DRV_MMZ_MapCache*/
/*CNcomment:使用HI_DRV_MMZ_MapCache时，需主动调用HI_DRV_MMZ_Flush进行cache数据同步 */
HI_S32 HI_DRV_MMZ_Flush(MMZ_BUFFER_S *psMBuf);

/*alloc mmz memory, and map kernel-state address*/
/*CNcomment:申请mmz内存后，进行内核态地址的映射*/
HI_S32  HI_DRV_MMZ_Map(MMZ_BUFFER_S *psMBuf);

/*unmap kernel-state address*/
/*CNcomment:解除内核态地址的映射*/
HI_VOID HI_DRV_MMZ_Unmap(MMZ_BUFFER_S *psMBuf);

/*release unmapped mmz memory */
/*CNcomment:解除映射后，或没有进行内核态映射的mmz内存进行释放*/
HI_VOID HI_DRV_MMZ_Release(MMZ_BUFFER_S *psMBuf);

/** @} */

HI_S32 DRV_MMZ_ModInit(HI_VOID);
HI_VOID DRV_MMZ_ModExit(HI_VOID);

#ifdef __cplusplus
 #if __cplusplus
}
 #endif
#endif /* __cplusplus */

#endif

