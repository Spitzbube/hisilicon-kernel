#include <linux/version.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/kernel.h>    /* printk() */
#include <linux/slab.h>      /* kmalloc() */
#include <linux/fs.h>        /* everything... */
#include <linux/errno.h> /* error codes */
#include <linux/types.h> /* size_t */
#include <linux/fcntl.h> /* O_ACCMODE */
#include <linux/cdev.h>
#include <asm/uaccess.h> /* copy_*_user */
#include <linux/interrupt.h>
#include <linux/poll.h>
#include <linux/workqueue.h>
#include <asm/io.h>

#include "hi_module.h"
#include "hi_drv_module.h"
#include "hi_drv_mem.h"
#include "drv_tde_ext.h"

#include "hi_tde_type.h"
#include "hi_drv_tde.h"
#include "tde_osr.h"
#include "tde_osictl.h"
#include "tde_osilist.h"
#include "tde_hal.h"
#include "wmalloc.h"
#include "tde_config.h"

typedef unsigned long       HI_UL;

#define TDE_NAME    "HI_TDE"

STATIC int tde_osr_isr(int irq, void *dev_id);
STATIC void tde_tasklet_func(unsigned long int_status);

/* TDE equipment quoted count */
STATIC atomic_t g_TDECount = ATOMIC_INIT(0);


#ifdef TDE_TIME_COUNT
TDE_timeval_s g_stTimeStart;
TDE_timeval_s g_stTimeEnd;
HI_U64 g_u64TimeDiff;
#endif

#ifdef TDE_USE_SPINLOCK
static spinlock_t tde_lock;
static unsigned long tde_lockflags;
#endif
static long disablecount = 0;


DECLARE_TASKLET(tde_tasklet, tde_tasklet_func, 0);

static TDE_EXPORT_FUNC_S s_TdeExportFuncs =
{
    .pfnTdeOpen             = TdeOsiOpen,
    .pfnTdeClose            = TdeOsiClose,
    .pfnTdeBeginJob         = TdeOsiBeginJob,
    .pfnTdeEndJob           = TdeOsiEndJob,
    .pfnTdeCancelJob        = TdeOsiCancelJob,
    .pfnTdeWaitForDone      = TdeOsiWaitForDone,
    .pfnTdeWaitAllDone      = TdeOsiWaitAllDone, 
    .pfnTdeQuickCopy        = TdeOsiQuickCopy,
    .pfnTdeQuickFill        = TdeOsiQuickFill,   
    .pfnTdeQuickResize      = TdeOsiQuickResize,
    .pfnTdeQuickFlicker     = TdeOsiQuickFlicker,
    .pfnTdeBlit             = TdeOsiBlit,
    .pfnTdeMbBlit           = TdeOsiMbBlit,      
    .pfnTdeSolidDraw        = TdeOsiSolidDraw,
    .pfnTdeSetDeflickerLevel        = TdeOsiSetDeflickerLevel,
    .pfnTdeEnableRegionDeflicker    = TdeOsiEnableRegionDeflicker,
    .pfnTdeCalScaleRect     = TdeCalScaleRect
};

HI_VOID tde_cleanup_module_k(HI_VOID);

HI_S32 tde_init_module_k(HI_VOID)
{
    int ret;

    if (TdeHalInit(TDE_REG_BASEADDR) < 0)
    {
        return -1;
    }

    if (0 != request_irq(TDE_INTNUM, (irq_handler_t)tde_osr_isr,
                         IRQF_PROBE_SHARED, "hi_tde_irq", NULL))
    {
        TDE_TRACE(TDE_KERN_ERR, "request_irq for TDE failure!\n");
        TdeHalRelease();
        return -1;
    }
	
    TdeOsiListInit();

    ret = HI_GFX_MODULE_Register(HIGFX_TDE_ID, TDE_NAME,&s_TdeExportFuncs);
    if (HI_SUCCESS != ret)
    {
        TDE_TRACE(TDE_KERN_ERR, "register module failed!\n");
        tde_cleanup_module_k();
        return ret;
    }

    spin_lock_init(&tde_lock);
    
    return 0;
}

HI_VOID tde_cleanup_module_k(HI_VOID)
{
    HI_DRV_MODULE_UnRegister(HI_ID_TDE);

    TdeOsiListTerm();

    free_irq(TDE_INTNUM, NULL);
    TdeHalRelease();

    return;
}

int tde_open(struct inode *finode, struct file  *ffile)
{
    if (1 == atomic_inc_return(&g_TDECount))
    {
        (HI_VOID)TdeHalOpen();
    }

    return 0;
}

int tde_release(struct inode *finode, struct file  *ffile)
{
    if (atomic_dec_and_test(&g_TDECount))
    {
        //todo:
        //tasklet_kill(&tde_tasklet);
    }

    if ( atomic_read(&g_TDECount) < 0 )
    {
        atomic_set(&g_TDECount, 0);
    }

    return 0;
}

long tde_ioctl(struct file  *ffile, unsigned int  cmd, unsigned long arg)
{
    void __user *argp = (void __user *)arg;


    switch (cmd)
    {
        case TDE_BEGIN_JOB:
        {
            TDE_HANDLE s32Handle;
            HI_S32 ret;
            if ((ret = TdeOsiBeginJob(&s32Handle)) < 0)
            {
                return ret;
            }

            if (copy_to_user(argp, &s32Handle, sizeof(TDE_HANDLE)))
            {
                return -EFAULT;
            }

            return 0;
        }

        case TDE_BIT_BLIT:
        {
            TDE_BITBLIT_CMD_S stBitBlt;
            TDE2_SURFACE_S *pstBackGround;
            TDE2_RECT_S *pstBackGroundRect;
            TDE2_SURFACE_S *pstForeGround;
            TDE2_RECT_S *pstForeGroundRect;
            TDE2_OPT_S *pstOpt;

            if (copy_from_user(&stBitBlt, argp, sizeof(TDE_BITBLIT_CMD_S)))
            {
                return -EFAULT;
            }

            pstBackGround       = ((stBitBlt.u32NullIndicator >> 1) & 1) ? NULL : &stBitBlt.stBackGround;
            pstBackGroundRect   = ((stBitBlt.u32NullIndicator >> 2) & 1) ? NULL : &stBitBlt.stBackGroundRect;
            pstForeGround       = ((stBitBlt.u32NullIndicator >> 3) & 1) ? NULL : &stBitBlt.stForeGround;
            pstForeGroundRect   = ((stBitBlt.u32NullIndicator >> 4) & 1) ? NULL : &stBitBlt.stForeGroundRect;
            pstOpt              = ((stBitBlt.u32NullIndicator >> 7) & 1) ? NULL : &stBitBlt.stOpt;

            return TdeOsiBlit(stBitBlt.s32Handle, pstBackGround, pstBackGroundRect,
                              pstForeGround, pstForeGroundRect, &stBitBlt.stDst, &stBitBlt.stDstRect,
                              pstOpt);
        }

        case TDE_SOLID_DRAW:
        {
            TDE_SOLIDDRAW_CMD_S stSolidDraw;
            TDE2_SURFACE_S *pstForeGround;
            TDE2_RECT_S *pstForeGroundRect;
            TDE2_FILLCOLOR_S *pstFillColor;
            TDE2_OPT_S *pstOpt;

            if (copy_from_user(&stSolidDraw, argp, sizeof(TDE_SOLIDDRAW_CMD_S)))
            {
                return -EFAULT;
            }

            pstForeGround       = ((stSolidDraw.u32NullIndicator >> 1) & 1) ? NULL : &stSolidDraw.stForeGround;
            pstForeGroundRect   = ((stSolidDraw.u32NullIndicator >> 2) & 1) ? NULL : &stSolidDraw.stForeGroundRect;
            pstFillColor        = ((stSolidDraw.u32NullIndicator >> 5) & 1) ? NULL : &stSolidDraw.stFillColor;
            pstOpt              = ((stSolidDraw.u32NullIndicator >> 6) & 1) ? NULL : &stSolidDraw.stOpt;

            return TdeOsiSolidDraw(stSolidDraw.s32Handle, pstForeGround, pstForeGroundRect,
                                   &stSolidDraw.stDst,
                                   &stSolidDraw.stDstRect, pstFillColor, pstOpt);
        }

        case TDE_QUICK_DEFLICKER:
        {
            TDE_QUICKDEFLICKER_CMD_S stDeflicker;

            if (copy_from_user(&stDeflicker, argp, sizeof(TDE_QUICKDEFLICKER_CMD_S)))
            {
                return -EFAULT;
            }

            return TdeOsiQuickFlicker(stDeflicker.s32Handle, &stDeflicker.stSrc, &stDeflicker.stSrcRect, &stDeflicker.stDst,
                                      &stDeflicker.stDstRect);
        }

        case TDE_QUICK_COPY:
        {
            TDE_QUICKCOPY_CMD_S stQuickCopy;

            if (copy_from_user(&stQuickCopy, argp, sizeof(TDE_QUICKCOPY_CMD_S)))
            {
                return -EFAULT;
            }

            return TdeOsiQuickCopy(stQuickCopy.s32Handle, &stQuickCopy.stSrc, &stQuickCopy.stSrcRect, &stQuickCopy.stDst,
                                   &stQuickCopy.stDstRect);
        }

        case TDE_QUICK_RESIZE:
        {
            TDE_QUICKRESIZE_CMD_S stQuickResize;

            if (copy_from_user(&stQuickResize, argp, sizeof(TDE_QUICKRESIZE_CMD_S)))
            {
                return -EFAULT;
            }

            return TdeOsiQuickResize(stQuickResize.s32Handle, &stQuickResize.stSrc, &stQuickResize.stSrcRect,
                                     &stQuickResize.stDst,
                                     &stQuickResize.stDstRect);
        }

        case TDE_QUICK_FILL:
        {
            TDE_QUICKFILL_CMD_S stQuickFill;

            if (copy_from_user(&stQuickFill, argp, sizeof(TDE_QUICKFILL_CMD_S)))
            {
                return -EFAULT;
            }

            return TdeOsiQuickFill(stQuickFill.s32Handle, &stQuickFill.stDst, &stQuickFill.stDstRect,
                                   stQuickFill.u32FillData);
        }

        case TDE_END_JOB:
        {
            TDE_ENDJOB_CMD_S stEndJob;

            if (copy_from_user(&stEndJob, argp, sizeof(TDE_ENDJOB_CMD_S)))
            {
                return -EFAULT;
            }

            return TdeOsiEndJob(stEndJob.s32Handle, stEndJob.bBlock, stEndJob.u32TimeOut, stEndJob.bSync, NULL, NULL);
        }

        case TDE_MB_BITBLT:
        {
            TDE_MBBITBLT_CMD_S stMbBitblt;
            if (copy_from_user(&stMbBitblt, argp, sizeof(TDE_MBBITBLT_CMD_S)))
            {
                return -EFAULT;
            }

            return TdeOsiMbBlit(stMbBitblt.s32Handle, &stMbBitblt.stMB, &stMbBitblt.stMbRect, &stMbBitblt.stDst, &stMbBitblt.stDstRect, &stMbBitblt.stMbOpt);
        }

        case TDE_WAITFORDONE:
        {
            /* AI7D02876 beg set timeout value according by instruct number */
            TDE_HANDLE s32Handle;

            if (copy_from_user(&s32Handle, argp, sizeof(TDE_HANDLE)))
            {
                return -EFAULT;
            }

            return TdeOsiWaitForDone(s32Handle, TDE_MAX_WAIT_TIMEOUT);
            /* AI7D02876 end */
        }

        case TDE_WAITALLDONE:
        {
            return TdeOsiWaitAllDone(HI_FALSE);
        }

        case TDE_RESET:
        {
            TdeOsiReset();
            return HI_SUCCESS;
        }

        case TDE_CANCEL_JOB:
        {
            TDE_HANDLE s32Handle;

            if (copy_from_user(&s32Handle, argp, sizeof(TDE_HANDLE)))
            {
                return -EFAULT;
            }

            return TdeOsiCancelJob(s32Handle);
        }
#if HI_TDE_BITMAPMASK_SUPPORT
        case TDE_BITMAP_MASKROP:
        {
            TDE_BITMAP_MASKROP_CMD_S stBmpMaskRop;
            if (copy_from_user(&stBmpMaskRop, argp, sizeof(TDE_BITMAP_MASKROP_CMD_S)))
            {
                return -EFAULT;
            }

            return TdeOsiBitmapMaskRop(stBmpMaskRop.s32Handle, 
                        &stBmpMaskRop.stBackGround, &stBmpMaskRop.stBackGroundRect, 
                        &stBmpMaskRop.stForeGround, &stBmpMaskRop.stForeGroundRect, 
                        &stBmpMaskRop.stMask, &stBmpMaskRop.stMaskRect, 
                        &stBmpMaskRop.stDst, &stBmpMaskRop.stDstRect,
                        stBmpMaskRop.enRopCode_Color, stBmpMaskRop.enRopCode_Alpha);
        }
        case TDE_BITMAP_MASKBLEND:
        {
            TDE_BITMAP_MASKBLEND_CMD_S stBmpMaskBlend;
            if (copy_from_user(&stBmpMaskBlend, argp, sizeof(TDE_BITMAP_MASKBLEND_CMD_S)))
            {
                return -EFAULT;
            }

            return TdeOsiBitmapMaskBlend(stBmpMaskBlend.s32Handle, &stBmpMaskBlend.stBackGround, &stBmpMaskBlend.stBackGroundRect, 
                        &stBmpMaskBlend.stForeGround, &stBmpMaskBlend.stForeGroundRect, &stBmpMaskBlend.stMask, &stBmpMaskBlend.stMaskRect, 
                        &stBmpMaskBlend.stDst, &stBmpMaskBlend.stDstRect, stBmpMaskBlend.u8Alpha, stBmpMaskBlend.enBlendMode);
        }
#endif
        case TDE_SET_DEFLICKERLEVEL:
        {
            TDE_DEFLICKER_LEVEL_E eDeflickerLevel;
            if (copy_from_user(&eDeflickerLevel, argp, sizeof(TDE_DEFLICKER_LEVEL_E)))
            {
                return -EFAULT;
            }

            return TdeOsiSetDeflickerLevel(eDeflickerLevel);
        }

        case TDE_GET_DEFLICKERLEVEL:
        {
            TDE_DEFLICKER_LEVEL_E eDeflickerLevel;

            if (TdeOsiGetDeflickerLevel(&eDeflickerLevel) != HI_SUCCESS)
            {
                return HI_FAILURE;
            }
            if (copy_to_user(argp, &eDeflickerLevel, sizeof(TDE_DEFLICKER_LEVEL_E)))
            {
                return -EFAULT;
            }

            return HI_SUCCESS;
        }

        case TDE_SET_ALPHATHRESHOLD_VALUE:
        {
            HI_U8 u8ThresholdValue;

            if (copy_from_user(&u8ThresholdValue, argp, sizeof(HI_U8)))
            {
                return -EFAULT;
            }

            return TdeOsiSetAlphaThresholdValue(u8ThresholdValue);
        }

        case TDE_GET_ALPHATHRESHOLD_VALUE:
        {
            HI_U8 u8ThresholdValue;

            if (TdeOsiGetAlphaThresholdValue(&u8ThresholdValue))
            {
                return HI_FAILURE;
            }

            if (copy_to_user(argp, &u8ThresholdValue, sizeof(HI_U8)))
            {
                return -EFAULT;
            }

            return HI_SUCCESS;
        }

        case TDE_SET_ALPHATHRESHOLD_STATE:
        {
            HI_BOOL bEnAlphaThreshold;

            if (copy_from_user(&bEnAlphaThreshold, argp, sizeof(HI_BOOL)))
            {
                return -EFAULT;
            }

            return TdeOsiSetAlphaThresholdState(bEnAlphaThreshold);
        }

        case TDE_GET_ALPHATHRESHOLD_STATE:
        {
            HI_BOOL bEnAlphaThreshold;

            TdeOsiGetAlphaThresholdState(&bEnAlphaThreshold);
            
            if (copy_to_user(argp, &bEnAlphaThreshold, sizeof(HI_BOOL)))
            {
                return -EFAULT;
            }

            return HI_SUCCESS;
        }
        case TDE_PATTERN_FILL:
        {
            TDE_PATTERN_FILL_CMD_S stPatternFillCmd = {0};
            TDE2_SURFACE_S *pstBackGround;
            TDE2_RECT_S *pstBackGroundRect;
            TDE2_SURFACE_S *pstForeGround;
            TDE2_RECT_S *pstForeGroundRect;
            TDE2_SURFACE_S *pstDst;
            TDE2_RECT_S *pstDstRect;
            TDE2_PATTERN_FILL_OPT_S *pstOpt;

            if (copy_from_user(&stPatternFillCmd, argp, sizeof(TDE_PATTERN_FILL_CMD_S)))
            {
                return -EFAULT;
            }

            pstBackGround       = ((stPatternFillCmd.u32NullIndicator >> 1) & 1) ? NULL : &stPatternFillCmd.stBackGround;
            pstBackGroundRect   = ((stPatternFillCmd.u32NullIndicator >> 2) & 1) ? NULL : &stPatternFillCmd.stBackGroundRect;
            pstForeGround       = ((stPatternFillCmd.u32NullIndicator >> 3) & 1) ? NULL : &stPatternFillCmd.stForeGround;
            pstForeGroundRect   = ((stPatternFillCmd.u32NullIndicator >> 4) & 1) ? NULL : &stPatternFillCmd.stForeGroundRect;
            pstDst              = ((stPatternFillCmd.u32NullIndicator >> 5) & 1) ? NULL : &stPatternFillCmd.stDst;
            pstDstRect          = ((stPatternFillCmd.u32NullIndicator >> 6) & 1) ? NULL : &stPatternFillCmd.stDstRect;
            pstOpt              = ((stPatternFillCmd.u32NullIndicator >> 7) & 1) ? NULL : &stPatternFillCmd.stOpt;

            return TdeOsiPatternFill(stPatternFillCmd.s32Handle, pstBackGround,
                pstBackGroundRect, pstForeGround, pstForeGroundRect,
                pstDst, pstDstRect, pstOpt);
        }

        case TDE_ENABLE_REGIONDEFLICKER:
        {
            HI_BOOL bRegionDeflicker;

            if (copy_from_user(&bRegionDeflicker, argp, sizeof(HI_BOOL)))
            {
                return -EFAULT;
            }

            return TdeOsiEnableRegionDeflicker(bRegionDeflicker);
        }

        default:
            return -ENOIOCTLCMD;
    }

    return 0;
}

STATIC int tde_osr_isr(int irq, void *dev_id)
{
    HI_U32 int_status;
    
#ifdef TDE_TIME_COUNT
    (HI_VOID)TDE_gettimeofday(&g_stTimeStart);
#endif
    int_status = TdeHalCtlIntStats();

    /* AI7D02547 Interrupt handling while suspend to die */
    if(int_status & 0x80000000)
    {
        TDE_TRACE(TDE_KERN_DEBUG, "tde interrupts coredump!\n");
        TdeHalCtlReset();
        return IRQ_HANDLED;
    }
    
    TDE_TRACE(TDE_KERN_DEBUG, "tde register int status: 0x%x!\n", (HI_U32)int_status);

    tde_tasklet.data = tde_tasklet.data |((HI_UL)int_status);
    
    tasklet_schedule(&tde_tasklet);

    return IRQ_HANDLED;
}

STATIC void tde_tasklet_func(unsigned long int_status)
{
    tde_osr_disableirq();
    tde_tasklet.data &= (~int_status);
    tde_osr_enableirq();

#ifdef TDE_TIME_COUNT
    (HI_VOID)TDE_gettimeofday(&g_stTimeEnd);

    g_u64TimeDiff = (g_stTimeEnd.tv_sec - g_stTimeStart.tv_sec)*1000000
         + (g_stTimeEnd.tv_usec - g_stTimeStart.tv_usec);
    TDE_TRACE(TDE_KERN_DEBUG, "tde int status: 0x%x, g_u64TimeDiff:%d!\n", (HI_U32)int_status, (HI_U32)g_u64TimeDiff);
#endif

#if HI_TDE_SQ_SUPPORT
    if(int_status&TDE_DRV_INT_SUSPEND_LINE_AQ)
    {
        TdeOsiListSuspLineProc(TDE_LIST_AQ);
    }
    
    if(int_status&TDE_DRV_INT_SUSPEND_LINE_SQ)
    {
        TdeOsiListSuspLineProc(TDE_LIST_SQ);
    }
#endif
    if(int_status&TDE_DRV_INT_NODE_COMP_AQ)
    {
        TdeOsiListNodeComp(TDE_LIST_AQ);
    }
#if HI_TDE_SQ_SUPPORT
    if(int_status&TDE_DRV_INT_HEAD_UPDATE_SQ) /*AI7D02874*/
    {
        TdeOsiListSqUpdateProc();
    }
    
    if(int_status&TDE_DRV_INT_NODE_COMP_SQ)
    {
        TdeOsiListNodeComp(TDE_LIST_SQ);
    }
#endif
    if(int_status&TDE_DRV_INT_lIST_COMP_AQ)
    {
        //TdeOsiListCompProc(TDE_LIST_AQ);
    }
#if HI_TDE_SQ_SUPPORT    
    if(int_status&TDE_DRV_INT_lIST_COMP_SQ)
    {
        TdeOsiListCompProc(TDE_LIST_SQ);
    }
#endif
}

/* tde wait for start  */
int tde_pm_suspend(PM_BASEDEV_S *pdev, pm_message_t state)
{
    TdeOsiWaitAllDone(HI_FALSE);

    TDE_TRACE(TDE_KERN_INFO, "tde suspend!\n");

    return 0;
}

/* wait for resume */
int tde_pm_resume(PM_BASEDEV_S *pdev)
{
    TDE_TRACE(TDE_KERN_INFO, "tde resume!\n");

    TdeHalResumeInit();

    return 0;
}


/*****************************************************************************
 Prototype       : TdeOsiOpen
 Description     : open TDE equipment
 Input           : I_VOID  **
 Output          : None
 Return Value    : 
 Global Variable   
    Read Only    : 
    Read & Write : 
  History         
  1.Date         : 2008/5/26
    Author       : wming
    Modification : Created function

*****************************************************************************/
HI_S32 TdeOsiOpen(HI_VOID)
{
    return tde_open(NULL, NULL);
}

/*****************************************************************************
 Prototype       : TdeOsiClose
 Description     : close TDE equipment
 Input           : I_VOID  **
 Output          : None
 Return Value    : 
 Global Variable   
    Read Only    : 
    Read & Write : 
  History         
  1.Date         : 2008/5/26
    Author       : wming
    Modification : Created function

*****************************************************************************/
HI_S32 TdeOsiClose(HI_VOID)
{
    return tde_release(NULL, NULL);
}

void tde_osr_disableirq(void)
{
#ifdef TDE_USE_SPINLOCK
    spin_lock_irqsave(&tde_lock, tde_lockflags);
#else
    tasklet_disable(&tde_tasklet);
#endif
    disablecount++;    
    //printk(KERN_ERR "+++++++disable %d+++++++++++\n", disablecount);
}

void tde_osr_enableirq(void)
{
    disablecount--;    
    //printk(KERN_ERR "--------enable %d--------\n", disablecount);    
#ifdef TDE_USE_SPINLOCK
    spin_unlock_irqrestore(&tde_lock, tde_lockflags);
#else
    tasklet_enable(&tde_tasklet);
#endif
}

HI_BOOL tde_osr_isirqdisabled(void)
{
    return (disablecount > 0) ? HI_TRUE : HI_FALSE;
}

typedef HI_VOID (* TDE_WQ_CB) (HI_U32);

typedef struct 
{
    HI_U32 Count;
    TDE_WQ_CB pWQCB;
    struct work_struct work;
}TDEFREEWQ_S;

void tde_osr_freevmem(struct work_struct *work)
{
    TDEFREEWQ_S *pWQueueInfo = container_of(work, TDEFREEWQ_S, work);
    TDE_TRACE(TDE_KERN_DEBUG, "Count = %d\n", pWQueueInfo->Count);
    pWQueueInfo->pWQCB(pWQueueInfo->Count);
    tde_osr_disableirq();
    TDE_FREE(pWQueueInfo);
    tde_osr_enableirq();
}

void tde_osr_hsr(void* pstFunc, void* data)
{
    TDEFREEWQ_S *pstWQ = NULL;

    pstWQ = TDE_MALLOC(sizeof(TDEFREEWQ_S));
    if(HI_NULL == pstWQ)
    {
        TDE_TRACE(TDE_KERN_INFO, "Malloc TDEFREEWQ_S failed!\n");
        return;
    }
    pstWQ->Count = (HI_U32)data;
    pstWQ->pWQCB = (TDE_WQ_CB)pstFunc;
    INIT_WORK(&pstWQ->work, tde_osr_freevmem);
    schedule_work(&pstWQ->work);
    return;
}

#ifndef MODULE
EXPORT_SYMBOL(tde_pm_suspend);
EXPORT_SYMBOL(tde_pm_resume);
#endif
EXPORT_SYMBOL(TdeOsiOpen);
EXPORT_SYMBOL(TdeOsiClose);
EXPORT_SYMBOL(tde_osr_enableirq);
EXPORT_SYMBOL(tde_osr_disableirq);
EXPORT_SYMBOL(tde_ioctl);
EXPORT_SYMBOL(tde_open);
EXPORT_SYMBOL(tde_release);
EXPORT_SYMBOL(tde_init_module_k);
EXPORT_SYMBOL(tde_cleanup_module_k);
