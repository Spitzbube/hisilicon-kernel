/******************************************************************************

  Copyright (C), 2008-2018, Hisilicon. Co., Ltd.

******************************************************************************
File Name	    : hi_gfx_comm_k.h
Version		    : version 1.0
Author		    : 
Created		    : 2013/05/28
Description	    : Describes adp file. CNcomment:���ƽ̨���� CNend\n
Function List 	: 

History       	:
Date				Author        		Modification
2013/05/28		    w00239113  		    revise by y00181162 at 2013/07/01
******************************************************************************/


#ifndef  _HI_GFX_COMM_K_H_
#define  _HI_GFX_COMM_K_H_


/***************************** SDK Version Macro Definition *********************/

/** \addtogroup 	GFX COMM */
/** @{ */  /** <!-- ��GFX COMM�� */

/** choice the sdk type */
/** CNcomment:SDK�汾 CNend */
#define  CONFIG_GFX_STB_SDK


/** @} */	/*! <!-- Macro Definition end */


/*********************************add include here******************************/

#include "hi_type.h"

#include <linux/version.h>
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 36))
#include <linux/smp_lock.h>
#endif

#if defined(CONFIG_GFX_STB_SDK)
#include "hi_debug.h"
#include "hi_module.h"
#include "hi_drv_module.h"
#include "hi_drv_dev.h"
#include "hi_drv_mmz.h"
#include "hi_drv_mem.h"
#include "drv_disp_ext.h"
#include "hi_drv_proc.h"
#include "hi_kernel_adapt.h"
#include "hi_drv_sys.h"
#elif defined(CONFIG_GFX_BVT_SDK)
#include "Sys_drv.h"
#include "hi_common.h"
#endif


/***************************** Macro Definition ******************************/

/** \addtogroup 	GFX COMM */
/** @{ */  /** <!-- ��GFX COMM�� */

/** this macro define at CFG_HI_KMOD_CFLAGS,so Makefile should include CFG_HI_KMOD_CFLAGS **/
#ifdef HI_ADVCA_FUNCTION_RELEASE
#define  CONFIG_GFX_ADVCA_RELEASE
#endif


#ifdef CONFIG_GFX_ADVCA_RELEASE
/** char disable */
/** CNcomment:charʹ�� CNend */
#define  CONFIG_GFX_COMM_STR_DISABLE
/** PROC disable */
/** CNcomment:procʹ�� CNend */
#define  CONFIG_GFX_COMM_PROC_DISABLE
/** version info disable */
/** CNcomment:�汾��Ϣʹ�� CNend */
#define  CONFIG_GFX_COMM_VERSION_DISABLE
#endif

/** close the string function */
/** CNcomment:�ر��ַ���,DEBUG����ر� CNend */
#ifdef   CONFIG_GFX_COMM_STR_DISABLE
/** LOG disable */
/** CNcomment:logʹ�� CNend */
#define  CONFIG_GFX_COMM_DEBUG_DISABLE
#endif

/** pm disable */
/** CNcomment:pmʹ�� CNend */
//#define  CONFIG_GFX_COMM_PM_DISABLE


/** register mammap operate */
/** CNcomment:�Ĵ���ӳ����� CNend */
#define HI_GFX_REG_MAP(base, size)                    ioremap_nocache((base), (size))
/** register unmap operate */
/** CNcomment:�Ĵ�����ӳ����� CNend */
#define HI_GFX_REG_UNMAP(base) 	                    iounmap((HI_VOID*)(base))

#ifdef CONFIG_GFX_COMM_STR_DISABLE
	#define SEQ_Printf(fmt...)                        {do{}while(0);}
	#define GFX_Printk(fmt...)                        {do{}while(0);}
#else 
	#define SEQ_Printf                                  PROC_PRINT
	#define GFX_Printk                                  HI_PRINT
#endif 


/** the mutex init */
/** CNcomment:�ź�����ʼ�� CNend */
#define HI_GFX_INIT_MUTEX(x)                           sema_init(x, 1)

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 36))
    #define HI_GFX_DECLARE_MUTEX(x)                        DECLARE_MUTEX(x)
#else
    #define HI_GFX_DECLARE_MUTEX(x)                        DEFINE_SEMAPHORE(x)
#endif


#if defined(CONFIG_GFX_STB_SDK)
	/** kmalloc mem */
	/** CNcomment:�ں˷����ڴ� CNend */
	#define HI_GFX_KMALLOC(module_id, size, flags)      HI_KMALLOC(ConvertID(module_id), size, flags)
	/** kfree mem */
	/** CNcomment:�ں��ͷ��ڴ� */
	#define HI_GFX_KFREE(module_id, addr)                HI_KFREE(ConvertID(module_id), addr)
	#define HI_GFX_VMALLOC(module_id, size)              HI_VMALLOC(ConvertID(module_id), size)
	#define HI_GFX_VFREE(module_id, addr)                HI_VFREE(ConvertID(module_id), addr)
    #define ConvertID(module_id) (module_id + HI_ID_TDE - HIGFX_TDE_ID)
    
	#ifdef CONFIG_GFX_COMM_DEBUG_DISABLE
	    #define HI_GFX_COMM_LOG_FATAL(module_id,fmt...)    
	    #define HI_GFX_COMM_LOG_ERROR(module_id,fmt...)    
	    #define HI_GFX_COMM_LOG_WARNING(module_id,fmt...)  
	    #define HI_GFX_COMM_LOG_INFO(module_id,fmt...)     
	#else 
	    #define HI_GFX_COMM_LOG_FATAL(module_id, fmt...)   HI_TRACE(HI_LOG_LEVEL_FATAL, ConvertID(module_id), fmt)
	    #define HI_GFX_COMM_LOG_ERROR(module_id,fmt...)    HI_TRACE(HI_LOG_LEVEL_ERROR, ConvertID(module_id), fmt)
	    #define HI_GFX_COMM_LOG_WARNING(module_id,fmt...)  HI_TRACE(HI_LOG_LEVEL_WARNING, ConvertID(module_id), fmt)
	    #define HI_GFX_COMM_LOG_INFO(module_id,fmt...)     HI_TRACE(HI_LOG_LEVEL_INFO, ConvertID(module_id), fmt)
	#endif 

	#ifndef CONFIG_GFX_COMM_PM_DISABLE
    	#define DECLARE_GFX_NODE(gfx_name,gfx_open, gfx_release, gfx_mmap, gfx_ioctl, gfx_suspend, gfx_resume) \
        static struct file_operations gfx_fops =\
        {\
            .owner   = THIS_MODULE,      \
            .unlocked_ioctl = gfx_ioctl,\
            .open    = gfx_open,        \
            .release = gfx_release,     \
            .mmap	 = gfx_mmap         \
        };\
        static PM_BASEOPS_S gfx_drvops = \
        {\
            .suspend      = gfx_suspend,\
            .resume       = gfx_resume  \
        };\
        static PM_DEVICE_S gfx_dev = \
        {\
            .name  = gfx_name,            \
            .minor = HIMEDIA_DYNAMIC_MINOR,\
            .owner = THIS_MODULE,          \
            .app_ops = &gfx_fops,          \
            .base_ops = &gfx_drvops        \
        }
    #else
        #define DECLARE_GFX_NODE(gfx_name, gfx_open, gfx_release, gfx_mmap, gfx_ioctl, gfx_suspend, gfx_resume)  \
        static struct file_operations gfx_fops =\
        {\
            .owner   = THIS_MODULE,       \
            .unlocked_ioctl = gfx_ioctl, \
            .open    = gfx_open,         \
            .release = gfx_release,      \
            .mmap	 = gfx_mmap          \
        };\
        static PM_BASEOPS_S gfx_drvops = \
        {\
            .suspend      = gfx_suspend,\
            .resume       = gfx_resume  \
        };\
        static PM_DEVICE_S gfx_dev = \
        {\
            .name  = gfx_name,            \
            .minor = HIMEDIA_DYNAMIC_MINOR,\
            .owner = THIS_MODULE,          \
            .app_ops = &gfx_fops           \
        }
 	#endif  

#elif defined(CONFIG_GFX_BVT_SDK)
	/** kmalloc mem */
	/** CNcomment:�ں˷����ڴ� CNend */
   	#define HI_GFX_KMALLOC(module_id, size, flags)       kmalloc(size, flags)
    #define HI_GFX_KFREE(module_id, addr)                 kfree(addr)
    #define HI_GFX_VMALLOC(module_id, size)               vmalloc(size)
    #define HI_GFX_VFREE(module_id, addr)                 vfree(addr)
    #define MMB_ADDR_INVALID (~0)

	/** kfree mem */
	/** CNcomment:�ں��ͷ��ڴ� CNend */
	#define ConvertID(module_id) (module_id + HI_ID_TDE - HIGFX_TDE_ID)
	
	#ifdef CONFIG_GFX_COMM_DEBUG_DISABLE
	    #define HI_GFX_COMM_LOG_FATAL(module_id,fmt...)    
	    #define HI_GFX_COMM_LOG_ERROR(module_id,fmt...)    
	    #define HI_GFX_COMM_LOG_WARNING(module_id,fmt...)  
	    #define HI_GFX_COMM_LOG_INFO(module_id,fmt...)     

	#else 
	    #define HI_GFX_COMM_LOG_FATAL(module_id, fmt...)   HI_TRACE(HI_LOG_LEVEL_FATAL,    ConvertID(module_id), fmt)
	    #define HI_GFX_COMM_LOG_ERROR(module_id,fmt...)    HI_TRACE(HI_LOG_LEVEL_ERROR,    ConvertID(module_id), fmt)
	    #define HI_GFX_COMM_LOG_WARNING(module_id,fmt...)  HI_TRACE(HI_LOG_LEVEL_WARNING,  ConvertID(module_id), fmt)
	    #define HI_GFX_COMM_LOG_INFO(module_id,fmt...)     HI_TRACE(HI_LOG_LEVEL_INFO,     ConvertID(module_id), fmt)
    #endif

	#define DECLARE_GFX_NODE(gfx_name,gfx_open, gfx_release, gfx_mmap, gfx_ioctl, gfx_suspend, gfx_resume) \
    struct file_operations gfx_fops =\
    {\
        .owner   = THIS_MODULE,      \
        .unlocked_ioctl = gfx_ioctl,\
        .open    = gfx_open,        \
        .release = gfx_release,     \
        .mmap	 = gfx_mmap         \
    };\
    static struct miscdevice gfx_dev =\
    {\
        MISC_DYNAMIC_MINOR,\
        gfx_name,         \
        &gfx_fops          \
    }

#else


#endif


/** @} */	/*! <!-- Macro Definition end */


/*************************** Enum Definition ****************************/

/** \addtogroup 	 GFX COMM */
/** @{ */  /** <!-- ��GFX COMM�� */


/** enum of the chip type */
/** CNcomment:оƬ����ö�� CNend */
typedef enum tagHIGFX_CHIP_TYPE_E
{
    HIGFX_CHIP_TYPE_HI3716MV100 = 0,   /**< HI3716MV100 */
    HIGFX_CHIP_TYPE_HI3716MV200,       /**< HI3716MV200 */
    HIGFX_CHIP_TYPE_HI3716MV300,       /**< HI3716MV300 */
    HIGFX_CHIP_TYPE_HI3716H,           /**< HI3716H     */
    HIGFX_CHIP_TYPE_HI3716CV100,       /**< HI3716C     */

    HIGFX_CHIP_TYPE_HI3720,            /**< HI3720       */
    HIGFX_CHIP_TYPE_HI3712V300,        /**< X6V300       */
    HIGFX_CHIP_TYPE_HI3715,            /**< HI3715       */

    HIGFX_CHIP_TYPE_HI3716CV200ES,     /**< S40V200      */
    HIGFX_CHIP_TYPE_HI3716CV200,       /**< HI3716CV200  */    
    HIGFX_CHIP_TYPE_HI3719MV100,       /**< HI3719MV100  */

    HIGFX_CHIP_TYPE_HI3531      = 100, /**< HI3531       */
    HIGFX_CHIP_TYPE_HI3521,            /**< HI3521       */
    HIGFX_CHIP_TYPE_HI3518,            /**< HI3518       */
    HIGFX_CHIP_TYPE_HI3520A,           /**< HI3520A      */
    HIGFX_CHIP_TYPE_HI3520D,           /**< HI3520D      */
    HIGFX_CHIP_TYPE_HI3535,            /**< HI3535       */

    HIGFX_CHIP_TYPE_BUTT        = 400  /**< Invalid Chip */
    
}HIGFX_CHIP_TYPE_E;


/** enum of the module ID */
/** CNcomment:ÿ��ģ���ID�� CNend */
typedef enum tagHIGFX_MODE_ID_E
{

	HIGFX_TDE_ID      = 0,    /**< TDE ID         */
	HIGFX_JPGDEC_ID,          /**< JPEG DECODE ID */
	HIGFX_JPGENC_ID,          /**< JPEG_ENCODE ID */
	HIGFX_FB_ID,              /**<  FRAMEBUFFER ID */
	HIGFX_PNG_ID,             /**< PNG ID          */
	HIGFX_BUTT_ID,
	
}HIGFX_MODE_ID_E;


/** @} */  /*! <!-- enum Definition end */

/*************************** Structure Definition ****************************/


/** \addtogroup 	 GFX COMM */
/** @{ */  /** <!-- ��GFX COMM�� */


/** Structure of proc item */
/** CNcomment:proc��غ������ CNend */
typedef struct struGFX_PROC_ITEM
{
	HI_S32 (*fnRead)(struct seq_file *, HI_VOID *);
	HI_S32 (*fnWrite)(struct file * file,  const char __user * buf, size_t count, loff_t *ppos);
	HI_S32 (*fnIoctl)(struct seq_file *, HI_U32 cmd, HI_U32 arg);
}GFX_PROC_ITEM_S;


/** @} */  /*! <!-- Structure Definition end */


/********************** Global Variable declaration **************************/

extern unsigned long long sched_clock(void);

/******************************* API declaration *****************************/

/** \addtogroup 	 GFX COMM */
/** @{ */  /** <!-- ��GFX COMM�� */


/** 
\brief show sdk version. CNcomment:��ʾģ��汾�� CNend\n
\attention \n
if you want to show module version when insmod ko,call this function.\n
CNcomment:��Ҫ��ʾģ��汾�ŵ�ʱ����øýӿ� CNend\n

\param[in]	ModID. CNcomment:ģ��ID CNend

\retval ::NA

\see \n
::HI_GFX_ShowVersionK
*/
static inline HI_VOID HI_GFX_ShowVersionK(HIGFX_MODE_ID_E ModID)
{
	#if !defined(CONFIG_GFX_COMM_VERSION_DISABLE) && !defined(CONFIG_GFX_COMM_DEBUG_DISABLE)
	
        HI_CHAR MouleName[7][10] = {"tde","jpegdec","jpegenc","fb","png", "higo", "gfx2d"};
        HI_CHAR Version[160] ="SDK_VERSION:["MKMARCOTOSTR(SDK_VERSION)"] Build Time:["\
		__DATE__", "__TIME__"]";

    	if (ModID >= HIGFX_BUTT_ID)
    		return;
		
		GFX_Printk("Load hi_%s.ko success.\t\t(%s)\n", MouleName[ModID],Version);
		
		return;
		
	#endif
}

/** 
\brief get time function. CNcomment:��ȡʱ�亯�� CNend\n
\attention \n

\param[in]	ModID. CNcomment:ģ��ID CNend\n

\retval ::HI_SUCCESS
\retval ::HI_FAILURE

\see \n
::HI_GFX_GetTimeStamp
*/
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
     
    
/** 
\brief free the mem that has alloced. CNcomment:�ͷŷ������ڴ� CNend
\attention \n

\param[in]	u32Phyaddr. CNcomment:�����ַ CNend\n

\retval ::HI_SUCCESS
\retval ::HI_FAILURE

\see \n
::HI_GFX_FreeMem
*/
static inline HI_VOID HI_GFX_FreeMem(HI_U32 u32Phyaddr)
{
        MMZ_BUFFER_S stBuffer;                             
        stBuffer.u32StartPhyAddr = u32Phyaddr;               
        HI_DRV_MMZ_Release(&stBuffer);                    
}


/** 
\brief alloc the mem that need. CNcomment:������Ҫ���ڴ� CNend\n
\attention \n

\param[in]	pName.        CNcomment:ģ����   CNend\n
\param[in]	pZoneName.
\param[in]	u32LayerSize. CNcomment:�ڴ��С CNend\n

\retval ::HI_SUCCESS
\retval ::HI_FAILURE

\see \n
::HI_GFX_AllocMem
*/
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
    {
        *penChipType = HIGFX_CHIP_TYPE_HI3716MV100;
		return;
    }
    if ((HI_CHIP_TYPE_HI3716M == ChipType) && (ChipVersion == HI_CHIP_VERSION_V200))
	{
        *penChipType = HIGFX_CHIP_TYPE_HI3716MV200;
		return;
    }
    if ((HI_CHIP_TYPE_HI3716M == ChipType) && (ChipVersion == HI_CHIP_VERSION_V300))
	{
        *penChipType = HIGFX_CHIP_TYPE_HI3716MV300;
		return;
    }
    if (HI_CHIP_TYPE_HI3716H == ChipType)
	{
        *penChipType = HIGFX_CHIP_TYPE_HI3716H;
		return;
    }
    if ((HI_CHIP_TYPE_HI3716C == ChipType) && (ChipVersion == HI_CHIP_VERSION_V100))
    {
        *penChipType = HIGFX_CHIP_TYPE_HI3716CV100;
		return;
    }
   if (HI_CHIP_TYPE_HI3720 == ChipType)
   {
        *penChipType = HIGFX_CHIP_TYPE_HI3720;
		return;
   	}
   if ((HI_CHIP_TYPE_HI3712 == ChipType) && (ChipVersion == HI_CHIP_VERSION_V300))
   {
        *penChipType = HIGFX_CHIP_TYPE_HI3712V300;
		return;
   }
   if (HI_CHIP_TYPE_HI3715 == ChipType)
   {
        *penChipType = HIGFX_CHIP_TYPE_HI3715;
		return;
   }
   if ((HI_CHIP_TYPE_HI3716CES == ChipType) && (HI_CHIP_VERSION_V200 == ChipVersion))
   {
        *penChipType = HIGFX_CHIP_TYPE_HI3716CV200ES;
		return;
   }
   if ((HI_CHIP_TYPE_HI3716C == ChipType) && (ChipVersion == HI_CHIP_VERSION_V200))
   {
        *penChipType = HIGFX_CHIP_TYPE_HI3716CV200;
		return;
   	}
   
}

#elif defined(CONFIG_GFX_BVT_SDK) 


/** 
\brief free the mem that has alloced. CNcomment:�ͷŷ������ڴ� CNend\n
\attention \n

\param[in]	u32Phyaddr. CNcomment:�����ַ CNend\n

\retval ::HI_SUCCESS
\retval ::HI_FAILURE

\see \n
::HI_GFX_FreeMem
*/
static HI_VOID HI_GFX_FreeMem(HI_U32 u32Phyaddr)
{
    delete_mmb(u32Phyaddr);                
}

/** 
\brief alloc the mem that need. CNcomment:������Ҫ���ڴ� CNend\n
\attention \n

\param[in]	pName.        CNcomment:ģ����   CNend\n
\param[in]	pZoneName.
\param[in]	u32LayerSize. CNcomment:�ڴ��С CNend\n

\retval ::HI_SUCCESS
\retval ::HI_FAILURE

\see \n
::HI_GFX_AllocMem
*/
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
	return 0;
	
}

static HI_VOID HI_GFX_PROC_RemoveModule(HI_CHAR *pEntry_name)
{
    #ifndef  CONFIG_GFX_COMM_PROC_DISABLE
	  CMPI_RemoveProc(pEntry_name);
    #endif
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

/** @} */  /*! <!-- API declaration end */


#endif /*_HI_GFX_COMM_K_H_ */
