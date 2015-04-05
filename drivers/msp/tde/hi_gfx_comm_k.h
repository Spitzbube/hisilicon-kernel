/**
 \file
 \brief Describes adp file. CNcomment:驱动跨平台适配
 \author Shenzhen Hisilicon Co., Ltd.
 \date 2008-2018
 \version 1.0
 \author w00239113
 \date 2013-5-28
 */
#ifndef __HI_GFX_COMMON__H__
#define __HI_GFX_COMMON__H__

/*************************** MACRO Definition ****************************/

//#define  CONFIG_GFX_BVT_SDK              // choice the sdk type
#define  CONFIG_GFX_STB_SDK           // choice the sdk type
//#define  CONFIG_GFX_COMM_DEBUG_DISABLE     //LOG disable.
//#define  CONFIG_GFX_COMM_PROC_DISABLE    //PROC disable.
//#define  CONFIG_GFX_COMM_PM_DISABLE        //pm disable.
//#define  CONFIG_GFX_COMM_STR_DISABLE       //char disable
//#define  CONFIG_GFX_COMM_VERSION_DISABLE   //version info disable.


#include "hi_type.h"

#include <linux/version.h>

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 36))
#include <linux/smp_lock.h>
#endif

#ifdef CONFIG_GFX_STB_SDK
#include "hi_debug.h"
#include "hi_module.h"
#include "hi_drv_module.h"
#include "hi_drv_dev.h"
#include "hi_drv_mmz.h"
#include "hi_drv_mem.h"
#include "drv_disp_ext.h"
#include "hi_drv_proc.h"
#include "hi_drv_mem.h"
#include "hi_kernel_adapt.h"
#include "hi_drv_sys.h"
#endif
//#else  CONFIG_GFX_BVT_SDK
#if 0
#include "Sys_drv.h"
#include "hi_common.h"
#endif

#ifdef   CONFIG_GFX_COMM_STR_DISABLE
#define  CONFIG_GFX_COMM_DEBUG_DISABLE       //关闭字符串功能,DEBUG关闭必须关闭.
#endif



extern unsigned long long sched_clock(void);

/*************************** Structure Definition ****************************/


typedef struct struGFX_PROC_ITEM
{
	HI_S32 (*fnRead)(struct seq_file *, HI_VOID *);
	HI_S32 (*fnWrite)(struct file * file,  const char __user * buf, size_t count, loff_t *ppos);
	HI_S32 (*fnIoctl)(struct seq_file *, HI_U32 cmd, HI_U32 arg);
}GFX_PROC_ITEM_S;


typedef enum
{
	HIGFX_TDE_ID = 0,
	HIGFX_JPGDEC_ID,
	HIGFX_JPGENC_ID,
	HIGFX_FB_ID,
	HIGFX_PNG_ID,
	HIGFX_BUTT_ID,
}HIGFX_MODE_ID_E;

typedef enum
{
    HIGFX_CHIP_TYPE_HI3716MV100 = 0,    /**<HI3716MV100 */
    HIGFX_CHIP_TYPE_HI3716MV200,        /**<HI3716MV200 */
    HIGFX_CHIP_TYPE_HI3716MV300,        /**<HI3716MV300 */
    HIGFX_CHIP_TYPE_HI3716H,            /**<HI3716H */
    HIGFX_CHIP_TYPE_HI3716CV100,        /**<HI3716C */

    HIGFX_CHIP_TYPE_HI3720,             /**<HI3720 */
    HIGFX_CHIP_TYPE_HI3712V300,         /**<X6V300 */
    HIGFX_CHIP_TYPE_HI3715,             /**<HI3715 */

    HIGFX_CHIP_TYPE_HI3716CV200ES,      /**<HI3716CV200ES */
    HIGFX_CHIP_TYPE_HI3716CV200,        /**<HI3716CV200 */
    HIGFX_CHIP_TYPE_HI3719MV100,        /**<HI3719MV100 */
    HIGFX_CHIP_TYPE_HI3718CV100,        /**<HI3718CV100 */
    HIGFX_CHIP_TYPE_HI3719CV100,        /**<HI3719CV100 */
    HIGFX_CHIP_TYPE_HI3719MV100_A,      /**<HI3719MV100_A */

    HIGFX_CHIP_TYPE_HI3531 = 100,       /**<HI3531 */
    HIGFX_CHIP_TYPE_HI3521,             /**<HI3521 */
    HIGFX_CHIP_TYPE_HI3518,             /**<HI3518 */
    HIGFX_CHIP_TYPE_HI3520A,            /**<HI3520A */
    HIGFX_CHIP_TYPE_HI3520D,            /**<HI3520D */
    HIGFX_CHIP_TYPE_HI3535,             /**<HI3535 */

    HIGFX_CHIP_TYPE_BUTT = 400          /**<Invalid Chip*/
}
HIGFX_CHIP_TYPE_E;

/********************** Global Variable declaration **************************/


/******************************* API declaration *****************************/

#define HI_GFX_REG_MAP(base, size)                  ioremap_nocache((base), (size))
#define HI_GFX_REG_UNMAP(base) 	                    iounmap((HI_VOID*)(base))

#ifdef CONFIG_GFX_COMM_STR_DISABLE
#define SEQ_Printf
#define GFX_Printk
#else
#define SEQ_Printf seq_printf
#define GFX_Printk printk
#endif

#define HI_INIT_MUTEX(x)  sema_init(x, 1)
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 36))
    #define HI_DECLARE_MUTEX(x) DECLARE_MUTEX(x)
#else
    #define HI_DECLARE_MUTEX(x) DEFINE_SEMAPHORE(x)
#endif

static inline  HI_VOID HI_GFX_ShowVersionK(HIGFX_MODE_ID_E ModID)
{
#ifndef CONFIG_GFX_COMM_VERSION_DISABLE
    HI_CHAR MouleName[6][10] = {"tde","jpegdec","jpegenc","fb","png"};
    HI_CHAR Version[160] ="SDK_VERSION:["MKMARCOTOSTR(SDK_VERSION)"] Build Time:["\
	__DATE__", "__TIME__"]";

    if (ModID >= HIGFX_BUTT_ID)
    	return;
    GFX_Printk("Load hi_%s.ko success.\t\t(%s)\n", MouleName[ModID],Version);
	return;
#endif
}
static inline HI_S32 HI_GFX_GetTimeStamp(HI_U32 *pu32TimeMs, HI_U32 *pu32TimeUs)
{
    HI_U64 u64TimeNow;
    HI_U64 ns;

    if(HI_NULL == pu32TimeMs)
	{
		return HI_FAILURE;
	}

	u64TimeNow = sched_clock();

	*pu32TimeMs = (HI_U32)iter_div_u64_rem(u64TimeNow,1000000,&ns);

	return HI_SUCCESS;
}


#ifdef CONFIG_GFX_STB_SDK
	#define HI_GFX_KMALLOC(module_id, size, flags)      HI_KMALLOC(ConvertID(modle_id), size, flags)
        #define HI_GFX_KFREE(module_id, addr)             HI_KFREE(ConvertID(modle_id), addr)
	#define HI_GFX_VMALLOC(module_id, size)             HI_VMALLOC(ConvertID(modle_id), size)
	#define HI_GFX_VFREE(module_id, addr)               HI_VFREE(ConvertID(modle_id), addr)

        #define ConvertID(modle_id) (modle_id + HI_ID_TDE - HIGFX_TDE_ID)



	#ifdef CONFIG_GFX_COMM_DEBUG_DISABLE
	    #define HI_GFX_COMM_LOG_FATAL(modle_id,fmt...)
	    #define HI_GFX_COMM_LOG_ERROR(modle_id,fmt...)
	    #define HI_GFX_COMM_LOG_WARNING(modle_id,fmt...)
	    #define HI_GFX_COMM_LOG_INFO(modle_id,fmt...)
	#else
	    #define HI_GFX_COMM_LOG_FATAL(modle_id, fmt...)   HI_TRACE(HI_LOG_LEVEL_FATAL, ConvertID(modle_id), fmt)
	    #define HI_GFX_COMM_LOG_ERROR(modle_id,fmt...)    HI_TRACE(HI_LOG_LEVEL_ERROR, ConvertID(modle_id), fmt)
	    #define HI_GFX_COMM_LOG_WARNING(modle_id,fmt...)  HI_TRACE(HI_LOG_LEVEL_WARNING, ConvertID(modle_id), fmt)
	    #define HI_GFX_COMM_LOG_INFO(modle_id,fmt...)     HI_TRACE(HI_LOG_LEVEL_INFO, ConvertID(modle_id), fmt)
	#endif
#ifndef CONFIG_GFX_COMM_PM_DISABLE
    #define DECLARE_GFX_NODE(gfx_name,gfx_open, gfx_release, gfx_ioctl, gfx_suspend, gfx_resume) \
        static struct file_operations gfx_fops =\
        {\
            .owner   = THIS_MODULE,\
            .unlocked_ioctl = gfx_ioctl,\
            .open    = gfx_open,\
            .release = gfx_release\
        };\
        static PM_BASEOPS_S gfx_drvops = {\
            .suspend      = gfx_suspend,\
            .resume       = gfx_resume\
        };\
        static PM_DEVICE_S gfx_dev = {\
            .name  = gfx_name,\
            .minor = HIMEDIA_DYNAMIC_MINOR,\
            .owner = THIS_MODULE,\
            .app_ops = &gfx_fops,\
            .base_ops = &gfx_drvops,\
        };
    #else
      #define DECLARE_GFX_NODE(gfx_name, gfx_open, gfx_release, gfx_ioctl, gfx_suspend, gfx_resume)  \
        static struct file_operations gfx_fops =\
        {\
            .owner   = THIS_MODULE,\
            .unlocked_ioctl = gfx_ioctl,\
            .open    = gfx_open,\
            .release = gfx_release\
        };\
        static PM_BASEOPS_S gfx_drvops = {\
            .suspend      = gfx_suspend,\
            .resume       = gfx_resume\
        };\
             PM_DEVICE_S gfx_dev = {\
            .name  = gfx_name,\
            .minor = HIMEDIA_DYNAMIC_MINOR,\
            .owner = THIS_MODULE,\
            .app_ops = &gfx_fops\
        };
 #endif


/*CNcomment:释放mmz内存，得到物理地址，并做内核态地址的映射*/
static inline HI_VOID HI_GFX_FreeMem(HI_U32 u32Phyaddr)
{
        MMZ_BUFFER_S stBuffer;
        stBuffer.u32StartPhyAddr = u32Phyaddr;
        HI_DRV_MMZ_Release(&stBuffer);
}
/*CNcomment:申请mmz内存，得到物理地址，并做内核态地址的映射*/
static inline HI_U32 HI_GFX_AllocMem(HI_CHAR *pName, HI_CHAR* pZoneName, HI_U32 u32LayerSize)
{
    int addr;
    MMZ_BUFFER_S stBuffer;

    if ((u32LayerSize == 0) || (u32LayerSize > 0x40000000))
    {
        return 0;
    }
    /* three time mem mangent*/
    if(HI_SUCCESS == HI_DRV_MMZ_Alloc(pName, pZoneName, u32LayerSize, 16, &stBuffer))
    {
        addr = stBuffer.u32StartPhyAddr;
    }
    else if(HI_SUCCESS == HI_DRV_MMZ_Alloc(pName, "graphics", u32LayerSize, 16, &stBuffer))
    {
        addr = stBuffer.u32StartPhyAddr;
    }
    else if(HI_SUCCESS == HI_DRV_MMZ_Alloc(pName, NULL, u32LayerSize, 16, &stBuffer))
    {
        addr = stBuffer.u32StartPhyAddr;
    }
    else
	{
        addr = 0;
	}
	return addr;
}


static inline HI_VOID *HI_GFX_Map(HI_U32 u32PhyAddr)
{
    MMZ_BUFFER_S stBuffer;

    stBuffer.u32StartPhyAddr = u32PhyAddr;
    if(HI_SUCCESS == HI_DRV_MMZ_Map(&stBuffer))
     {
         return ((unsigned char *)stBuffer.u32StartVirAddr);
     }
     else
     {
         return HI_NULL;
     }
}

static inline HI_VOID *HI_GFX_MapCached(HI_U32 u32PhyAddr)
{
    MMZ_BUFFER_S stBuffer;
    stBuffer.u32StartPhyAddr = u32PhyAddr;
    if(HI_SUCCESS == HI_DRV_MMZ_MapCache(&stBuffer))
    {
        return ((unsigned char *)stBuffer.u32StartVirAddr);
    }
    else
    {
        return HI_NULL;
    }
}
static inline HI_S32 HI_GFX_Unmap(HI_VOID *pViraddr)
{
    MMZ_BUFFER_S stBuffer;
    stBuffer.u32StartVirAddr = (HI_U32)pViraddr;
    HI_DRV_MMZ_Unmap(&stBuffer);

    return HI_SUCCESS;
}


static inline HI_S32 HI_GFX_PROC_AddModule(HI_CHAR * pEntry_name, GFX_PROC_ITEM_S* pProcItem, HI_VOID *pData)
{
#ifndef  CONFIG_GFX_COMM_PROC_DISABLE
	DRV_PROC_EX_S stProcItem;
	stProcItem.fnIoctl =  pProcItem->fnIoctl;
	stProcItem.fnRead = pProcItem->fnRead;
	stProcItem.fnWrite= pProcItem->fnWrite;
	HI_DRV_PROC_AddModule(pEntry_name, &stProcItem, pData);
#endif
	return 0;

}

static inline HI_VOID HI_GFX_PROC_RemoveModule(HI_CHAR *pEntry_name)
{
#ifndef  CONFIG_GFX_COMM_PROC_DISABLE
	HI_DRV_PROC_RemoveModule(pEntry_name);
#endif
	return;
}

static inline HI_S32 HI_GFX_MODULE_Register(HI_U32 u32ModuleID, const HI_CHAR * pszModuleName, HI_VOID *pData)
{
	return HI_DRV_MODULE_Register(ConvertID(u32ModuleID), pszModuleName, pData);
}

static inline HI_S32 HI_GFX_MODULE_UnRegister(HI_U32 u32ModuleID)
{
	return HI_DRV_MODULE_UnRegister(ConvertID(u32ModuleID));
}

#define HI_GFX_PM_Register()  HI_DRV_PM_Register(&gfx_dev);

#define HI_GFX_PM_UnRegister()  HI_DRV_PM_UnRegister(&gfx_dev);
static inline HI_VOID HI_GFX_SYS_GetChipVersion(HIGFX_CHIP_TYPE_E *penChipType)
{
    HI_CHIP_TYPE_E    ChipType;
    HI_CHIP_VERSION_E ChipVersion;

    HI_DRV_SYS_GetChipVersion(&ChipType, &ChipVersion);

    if ((HI_CHIP_TYPE_HI3716M == ChipType) && (ChipVersion == HI_CHIP_VERSION_V100))
        *penChipType = HIGFX_CHIP_TYPE_HI3716MV100;

    if ((HI_CHIP_TYPE_HI3716M == ChipType) && (ChipVersion == HI_CHIP_VERSION_V200))
        *penChipType = HIGFX_CHIP_TYPE_HI3716MV200;
    if ((HI_CHIP_TYPE_HI3716M == ChipType) && (ChipVersion == HI_CHIP_VERSION_V300))
        *penChipType = HIGFX_CHIP_TYPE_HI3716MV300;
    if (HIGFX_CHIP_TYPE_HI3716H == ChipType)
        *penChipType = HIGFX_CHIP_TYPE_HI3716H;
    if ((HI_CHIP_TYPE_HI3716C == ChipType) && (ChipVersion == HI_CHIP_VERSION_V100))
        *penChipType = HIGFX_CHIP_TYPE_HI3716CV100;
   if (HI_CHIP_TYPE_HI3720 == ChipType)
        *penChipType = HIGFX_CHIP_TYPE_HI3720;
   if ((HI_CHIP_TYPE_HI3712 == ChipType) && (ChipVersion == HI_CHIP_VERSION_V300))
        *penChipType = HIGFX_CHIP_TYPE_HI3712V300;
   if (HI_CHIP_TYPE_HI3715 == ChipType)
        *penChipType = HIGFX_CHIP_TYPE_HI3715;
   if ((HI_CHIP_TYPE_HI3716CES == ChipType) && (ChipVersion == HI_CHIP_VERSION_V200))
        *penChipType = HIGFX_CHIP_TYPE_HI3716CV200ES;
   if ((HI_CHIP_TYPE_HI3716C == ChipType) && (ChipVersion == HI_CHIP_VERSION_V200))
        *penChipType = HIGFX_CHIP_TYPE_HI3716CV200;
   if ((HI_CHIP_TYPE_HI3718C == ChipType) && (ChipVersion == HI_CHIP_VERSION_V100))
        *penChipType = HIGFX_CHIP_TYPE_HI3718CV100;
   if ((HI_CHIP_TYPE_HI3719C == ChipType) && (ChipVersion == HI_CHIP_VERSION_V100))
        *penChipType = HIGFX_CHIP_TYPE_HI3719CV100;
   if ((HI_CHIP_TYPE_HI3719M_A == ChipType) && (ChipVersion == HI_CHIP_VERSION_V100))
        *penChipType = HIGFX_CHIP_TYPE_HI3719MV100_A;
    return ;

}
    #endif
//#else  if CONFIG_GFX_BVT_SDK
#if 0
    #define HI_GFX_KMALLOC(module_id, size, flags)      kmalloc(size, flags)
    #define HI_GFX_KFREE(module_id, addr)               kfree(addr)
    #define HI_GFX_VMALLOC(module_id, size)             vmalloc(size)
    #define HI_GFX_VFREE(module_id, addr)               vfree(addr)
    #define MMB_ADDR_INVALID (~0)

	#ifdef CONFIG_GFX_COMM_DEBUG_DISABLE
	    #define HI_GFX_COMM_LOG_FATAL(modle_id,fmt...)
	    #define HI_GFX_COMM_LOG_ERROR(modle_id,fmt...)
	    #define HI_GFX_COMM_LOG_WARNING(modle_id,fmt...)
	    #define HI_GFX_COMM_LOG_INFO(modle_id,fmt...)

	#else
        static HI_S32 ConvertID(modle_id)
	    {
	        HI_S32 ModleID;
	        switch(modle_id)
	        {
	            case HIGFX_TDE_ID:
	                ModleID = HI_ID_TDE;
	                break;
	            case HIGFX_JPGDEC_ID:
	                ModleID = HI_ID_JPEGD;
	                break;
	            default:
	                ModleID = HI_ID_FB;
	                break;
	        }
	        return ModleID;
	    }
	    #define HI_GFX_COMM_LOG_FATAL(modle_id, fmt...)   HI_TRACE(HI_LOG_LEVEL_FATAL,    ConvertID(modle_id), fmt)
	    #define HI_GFX_COMM_LOG_ERROR(modle_id,fmt...)    HI_TRACE(HI_LOG_LEVEL_ERROR,    ConvertID(modle_id), fmt)
	    #define HI_GFX_COMM_LOG_WARNING(modle_id,fmt...)  HI_TRACE(HI_LOG_LEVEL_WARNING,  ConvertID(modle_id), fmt)
	    #define HI_GFX_COMM_LOG_INFO(modle_id,fmt...)     HI_TRACE(HI_LOG_LEVEL_INFO,     ConvertID(modle_id), fmt)
    #endif

/*show version num*/
/*CNcomment:申请mmz内存，得到物理地址，并做内核态地址的映射*/
#define DECLARE_GFX_NODE(gfx_name,gfx_open, gfx_release, gfx_ioctl, gfx_suspend, gfx_resume) \
do  \
{\
    struct file_operations gfx_fops =\
    {\
        .owner   = THIS_MODULE,\
        .unlocked_ioctl = gfx_ioctl,\
        .open    = gfx_open,\
        .release = gfx_release,\
    };\
    static struct miscdevice gfx_dev =\
    {\
        MISC_DYNAMIC_MINOR,\
        gfx_name,\
        &gfx_fops,\
    };\
}\
while (0);


static HI_VOID HI_GFX_FreeMem(HI_U32 u32Phyaddr)
{
    delete_mmb(u32Phyaddr);
}

static HI_U32 HI_GFX_AllocMem(HI_CHAR *pName, HI_CHAR* pZoneName, HI_U32 u32LayerSize)
{
    int addr;

    if ((u32LayerSize == 0) || (u32LayerSize > 0x40000000))
    {
        return 0;
    }
        int addr;


    if (MMB_ADDR_INVALID != (addr = new_mmb(pName,  u32LayerSize, 16, pZoneName)))
    {
		return addr;
    }
    else  if (MMB_ADDR_INVALID != (addr = new_mmb(pName,  u32LayerSize, 16, "graphics")))
    {
		return addr;
    }
    else if (MMB_ADDR_INVALID != (addr = new_mmb(pName,  u32LayerSize, 16, NULL))
    {
		return addr;
    }
    else
	{
		return MMB_ADDR_INVALID;
	}


}


static HI_VOID *HI_GFX_Map(HI_U32 u32PhyAddr)
{
    return (unsigned char *)remap_mmb(u32PhyAddr);
}

static HI_VOID *HI_GFX_MapCached(HI_U32 u32PhyAddr)
{
    return (unsigned char *)remap_mmb_cached(u32PhyAddr);
}
static HI_S32 HI_GFX_Unmap(HI_VOID *pViraddr)
{
    unmap_mmb(pViraddr);
    return HI_SUCCESS;
}

static HI_S32 HI_GFX_PROC_AddModule(HI_CHAR * pEntry_name, GFX_PROC_ITEM_S* pProcItem, HI_VOID *pData)
{
#ifndef  CONFIG_GFX_COMM_PROC_DISABLE
    CMPI_PROC_ITEM_S *pProcItem;
    pProcItem = CMPI_CreateProc(pEntry_name, pProcItem->fnRead, pData);
    pProcItem->write = pProcItem->fnWrite
    pProcItem->pData = pData;
#endif
	return;

}

static HI_VOID HI_GFX_PROC_RemoveModule(HI_CHAR *pEntry_name)
{
#ifndef  CONFIG_GFX_COMM_PROC_DISABLE
	CMPI_RemoveProc(pEntry_name);
#endif
	return;
}

static HI_S32 HI_GFX_MODULE_Register(HI_U32 u32ModuleID, const HI_CHAR * pszModuleName,HI_VOID *pData)
{
	//return HI_MODULE_Register((u32ModuleID + HI_ID_TDE - GFX_TDE_ID), pszModuleName);
}

static HI_S32 HI_GFX_MODULE_UnRegister(HI_U32 u32ModuleID)
{
	//return HI_MODULE_UnRegister((u32ModuleID + HI_ID_TDE - GFX_TDE_ID));
}

static HI_S32 HI_GFX_PM_Register()
{
	return misc_register(&gfx_dev);
}

static HI_S32 HI_GFX_PM_UnRegister()
{
	return misc_deregister(&gfx_dev);
}

static HI_VOID HI_GFX_SYS_GetChipVersion(HIGFX_CHIP_TYPE_E *penChipType)
{
    HI_U32 Version;

    penChipType = HIGFX_CHIP_TYPE_BUTT;
    Version = SYS_DRV_GetChipVersion();
    switch (Version)
    {
        case HI3535_V100;
            *penChipType = HIGFX_CHIP_TYPE_HI3535;
            break;
        default
            *penChipType = HIGFX_CHIP_TYPE_BUTT;
    }
    return ;

}

#else
#endif
#endif
