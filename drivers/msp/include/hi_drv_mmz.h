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
/*CNcomment:����mmz�ڴ棬�õ������ַ�������ں�̬��ַ��ӳ��*/
HI_S32  HI_DRV_MMZ_AllocAndMap(const char *name, char *mmzzonename, HI_U32 size, int align, MMZ_BUFFER_S *psMBuf);

/*unmap kernel-state address, release mmz memory*/
/*CNcomment:����ں�̬��ַ��ӳ�䣬���ͷ�mmz�ڴ�*/
HI_VOID HI_DRV_MMZ_UnmapAndRelease(MMZ_BUFFER_S *psMBuf);

/*Only alloc mmz memory, return physic address, but not map kernel-state address*/
/*CNcomment:ֻ����mmz�ڴ棬���������ַ�������ں�̬��ַ��ӳ��*/
HI_S32  HI_DRV_MMZ_Alloc(const char *bufname, char *zone_name, HI_U32 size, int align, MMZ_BUFFER_S *psMBuf);

/*map kernel-state address after alloc mmz memory for cache, and flushing cache with HI_DRV_MMZ_Flush*/
/*CNcomment:����mmz��Cache�ڴ�󣬽����ں�̬��ַ��ӳ��, ��ʹ��HI_DRV_MMZ_Flush����cacheͬ�� */
HI_S32 HI_DRV_MMZ_MapCache(MMZ_BUFFER_S *psMBuf);

/*flush cache data to memory, needed to call when map memory with HI_DRV_MMZ_MapCache*/
/*CNcomment:ʹ��HI_DRV_MMZ_MapCacheʱ������������HI_DRV_MMZ_Flush����cache���ͬ�� */
HI_S32 HI_DRV_MMZ_Flush(MMZ_BUFFER_S *psMBuf);

/*alloc mmz memory, and map kernel-state address*/
/*CNcomment:����mmz�ڴ�󣬽����ں�̬��ַ��ӳ��*/
HI_S32  HI_DRV_MMZ_Map(MMZ_BUFFER_S *psMBuf);

/*unmap kernel-state address*/
/*CNcomment:����ں�̬��ַ��ӳ��*/
HI_VOID HI_DRV_MMZ_Unmap(MMZ_BUFFER_S *psMBuf);

/*release unmapped mmz memory */
/*CNcomment:���ӳ��󣬻�û�н����ں�̬ӳ���mmz�ڴ�����ͷ�*/
HI_VOID HI_DRV_MMZ_Release(MMZ_BUFFER_S *psMBuf);

/** @} */

int HI_DRV_MMZ_Init(void);
void HI_DRV_MMZ_Exit(void);

HI_S32 DRV_MMZ_ModInit(HI_VOID);
HI_VOID DRV_MMZ_ModExit(HI_VOID);

#ifdef __cplusplus
 #if __cplusplus
}
 #endif
#endif /* __cplusplus */

#endif

