#include <linux/kernel.h>
#include <mach/hardware.h>
#include <asm/io.h>
#include "hi_drv_struct.h"
#include "hi_drv_module.h"
#include "hi_module.h"
#include "hi_drv_dev.h"
#include "hi_drv_proc.h"
#include "drv_pq_ext.h"
#include "drv_pq.h"
#include "hi_drv_mmz.h"
#include "hi_osal.h"
#include "hi_type.h"
#include "drv_pdm_ext.h"


#define PQ_NAME                "HI_PQ"


DECLARE_MUTEX(g_PqMutex);

#define DRV_PQ_Lock()      \
    do{         \
        if(down_interruptible(&g_PqMutex))   \
        {       \
            HI_ERR_PQ("ERR: PQ intf lock error!\n");  \
        }       \
    }while(0)

#define DRV_PQ_UnLock()      \
    do{         \
        up(&g_PqMutex);    \
    }while(0)


typedef struct tagPQ_REGISTER_PARAM_S
{
    DRV_PROC_READ_FN        rdproc;
    DRV_PROC_WRITE_FN       wtproc;
} PQ_REGISTER_PARAM_S;


PQ_EXPORT_FUNC_S   g_PqExportFuncs =
{
    .pfnPQ_GetPqParam           = PQ_DRV_GetPqParam
};

extern PQ_PARAM_S* g_pstPqParam;
extern HI_BOOL g_bLoadPqBin;
extern MMZ_BUFFER_S g_stPqBinBuf;
static HI_S32 PQ_ProcRead(struct seq_file* p, HI_VOID* v)
{
    PROC_PRINT(p, "\n -------------------------- PQ  Driver info ----------------------------------\n");
    PROC_PRINT(p, "------------ PQ Driver Version = %s\n", PQ_VERSION);
    PROC_PRINT(p, "------------ PQ Driver Bin Size = %d\n", sizeof(PQ_PARAM_S));
	
    if (HI_FALSE == g_bLoadPqBin)
    {
        PROC_PRINT(p, "------------ PQ Driver Get Flash Data : Failure\n");

    }
    else
    {
        PROC_PRINT(p, "------------ PQ Driver Get Flash Data : Success\n");
        PROC_PRINT(p, "------------ PQ Bin Info Version = %s\n", g_pstPqParam->stPQFileHeader.u8Version);
        PROC_PRINT(p, "------------ PQ Bin Info Chipname = %s\n", g_pstPqParam->stPQFileHeader.u8ChipName);
        PROC_PRINT(p, "------------ PQ Bin Info Sdkversion = %s\n", g_pstPqParam->stPQFileHeader.u8SDKVersion);
        PROC_PRINT(p, "------------ PQ Bin Info Author = %s\n", g_pstPqParam->stPQFileHeader.u8Author);
        PROC_PRINT(p, "------------ PQ Bin Info Describe = %s\n", g_pstPqParam->stPQFileHeader.u8Desc);
        PROC_PRINT(p, "------------ PQ Bin Info Time = %s\n", g_pstPqParam->stPQFileHeader.u8Time);
    }
	
    return HI_SUCCESS;
}

static HI_S32 PQ_ProcWrite(struct file* file,
                           const char __user* buf, size_t count, loff_t* ppos)
{
    return HI_SUCCESS;
}

HI_S32 PQ_DRV_Open(struct inode* finode, struct file*  ffile)
{
    return HI_SUCCESS;
}

HI_S32 PQ_DRV_Close(struct inode* finode, struct file*  ffile)
{

    return HI_SUCCESS;
}


HI_S32 PQ_DRV_ProcessGetCmd(PQ_IO_S* arg)
{
    HI_U32 u32PqCmd = arg->u32PqCmd;

    switch (u32PqCmd)
    {
        case PQ_CMD_VIRTUAL_OPTION_DISP0:
        case PQ_CMD_VIRTUAL_OPTION_DISP1:
        case PQ_CMD_VIRTUAL_OPTION_DISP2:
        {
            PQ_DRV_GetOption(arg);
            break;
        }
        case PQ_CMD_VIRTUAL_DEI_CTRL:
        case PQ_CMD_VIRTUAL_DEI_GLOBAL_MOTION_CTRL:
        case PQ_CMD_VIRTUAL_DEI_DIR_CHECK:
        case PQ_CMD_VIRTUAL_DEI_DIR_MULTI:
        case PQ_CMD_VIRTUAL_DEI_INTP_SCALE_RATIO:
        case PQ_CMD_VIRTUAL_DEI_INTP_CTRL:
        case PQ_CMD_VIRTUAL_DEI_JITTER_MOTION:
        case PQ_CMD_VIRTUAL_DEI_FIELD_MOTION:
        case PQ_CMD_VIRTUAL_DEI_MOTION_RATIO_CURVE:
        case PQ_CMD_VIRTUAL_DEI_IIR_MOTION_CURVE:
        case PQ_CMD_VIRTUAL_DEI_REC_MODE:
        case PQ_CMD_VIRTUAL_DEI_HIST_MOTION:
        case PQ_CMD_VIRTUAL_DEI_MOR_FLT:
        case PQ_CMD_VIRTUAL_DEI_OPTM_MODULE:
        case PQ_CMD_VIRTUAL_DEI_COMB_CHK:
        case PQ_CMD_VIRTUAL_DEI_CRS_CLR:
        {
            PQ_DRV_GetDeiParam(arg);
            break;
        }
        case PQ_CMD_VIRTUAL_FMD_CTRL:
        case PQ_CMD_VIRTUAL_FMD_HISTBIN:
        case PQ_CMD_VIRTUAL_FMD_PCCTHD:
        case PQ_CMD_VIRTUAL_FMD_PCCBLK:
        case PQ_CMD_VIRTUAL_FMD_UMTHD:
        case PQ_CMD_VIRTUAL_FMD_ITDIFF:
        case PQ_CMD_VIRTUAL_FMD_LAST:
        {
            PQ_DRV_GetFmdParam(arg);
            break;
        }
        case PQ_CMD_VIRTUAL_DNR_CTRL:
        case PQ_CMD_VIRTUAL_DNR_DB_FILTER:
        case PQ_CMD_VIRTUAL_DNR_DR_FILTER:
        case PQ_CMD_VIRTUAL_DNR_INFO:
        {
            PQ_DRV_GetDnrParam(arg);
            break;
        }
        case PQ_CMD_VIRTUAL_SHARP_VHD_LUMA:
        case PQ_CMD_VIRTUAL_SHARP_VSD_LUMA:
        case PQ_CMD_VIRTUAL_SHARP_STR_LUMA:
        case PQ_CMD_VIRTUAL_SHARP_GP0_LUMA:
        case PQ_CMD_VIRTUAL_SHARP_GP1_LUMA:
        case PQ_CMD_VIRTUAL_SHARP_VHD_CHROMA:
        case PQ_CMD_VIRTUAL_SHARP_VSD_CHROMA:
        case PQ_CMD_VIRTUAL_SHARP_STR_CHROMA:
        case PQ_CMD_VIRTUAL_SHARP_GP0_CHROMA:
        case PQ_CMD_VIRTUAL_SHARP_GP1_CHROMA:
        {
            PQ_DRV_GetSharpParam(arg);
            break;
        }
        case PQ_CMD_VIRTUAL_CSC_VDP_V0:
        case PQ_CMD_VIRTUAL_CSC_VDP_V1:
        case PQ_CMD_VIRTUAL_CSC_VDP_V3:
        case PQ_CMD_VIRTUAL_CSC_VDP_V4:
        case PQ_CMD_VIRTUAL_CSC_VDP_G0:
        case PQ_CMD_VIRTUAL_CSC_VDP_G1:
        case PQ_CMD_VIRTUAL_CSC_VDP_WBC_DHD0:
        case PQ_CMD_VIRTUAL_CSC_VDP_INTER_DHD0:
        case PQ_CMD_VIRTUAL_CSC_VDP_INTER_DHD1:
        {
            PQ_DRV_GetCscParam(arg);
            break;
        }
        case PQ_CMD_VIRTUAL_ACC_MOD_GENTLE:
        case PQ_CMD_VIRTUAL_ACC_MOD_MIDDLE:
        case PQ_CMD_VIRTUAL_ACC_MOD_STRONG:
        case PQ_CMD_VIRTUAL_ACC_CTRL:
        {
            PQ_DRV_GetAccParam(arg);
            break;
        }
        case PQ_CMD_VIRTUAL_ACM_MOD_BLUE:
        case PQ_CMD_VIRTUAL_ACM_MOD_GREEN:
        case PQ_CMD_VIRTUAL_ACM_MOD_BG:
        case PQ_CMD_VIRTUAL_ACM_MOD_SKIN:
        case PQ_CMD_VIRTUAL_ACM_MOD_VIVID:
        case PQ_CMD_VIRTUAL_ACM_CTRL:
        {
            PQ_DRV_GetAcmParam(arg);
            break;
        }
        case PQ_CMD_VIRTUAL_GAMMA_RGB_MOD0:
        case PQ_CMD_VIRTUAL_GAMMA_RGB_MOD1:
        case PQ_CMD_VIRTUAL_GAMMA_RGB_MOD2:
        case PQ_CMD_VIRTUAL_GAMMA_RGB_MOD3:
        case PQ_CMD_VIRTUAL_GAMMA_YUV_MOD0:
        case PQ_CMD_VIRTUAL_GAMMA_YUV_MOD1:
        case PQ_CMD_VIRTUAL_GAMMA_YUV_MOD2:
        case PQ_CMD_VIRTUAL_GAMMA_YUV_MOD3:
        {
            PQ_DRV_GetGammaParam(arg);
            break;
        }
        case PQ_CMD_VIRTUAL_GAMMA_CTRL_DISPLAY0:
        case PQ_CMD_VIRTUAL_GAMMA_CTRL_DISPLAY1:
        {
            PQ_DRV_GetGammaCtrlParam(arg);
            break;
        }
        case PQ_CMD_VIRTUAL_DITHER_COEF:
        {
            PQ_DRV_GetDitherParam(arg);
            break;
        }
        case PQ_CMD_VIRTUAL_BIN_EXPORT:
        {
            arg->u_Data.u32PqDriverBinPhyAddr = g_stPqBinBuf.u32StartPhyAddr;
            break;

        }
        default:
        {
            HI_TRACE(HI_LOG_LEVEL_ERROR, HI_ID_PQ, "Error,PQ_DRV_ReadBin,unknown address = %x\n", u32PqCmd);
            return HI_FAILURE;
        }
    }
    return HI_SUCCESS;
}



HI_S32 PQ_DRV_ProcessSetCmd(PQ_IO_S* arg)
{
    HI_U32 u32PqCmd = arg->u32PqCmd;
    switch (u32PqCmd)
    {
        case PQ_CMD_VIRTUAL_OPTION_DISP0:
        case PQ_CMD_VIRTUAL_OPTION_DISP1:
        case PQ_CMD_VIRTUAL_OPTION_DISP2:
        {
            PQ_DRV_SetOption(arg);
            break;
        }
        case PQ_CMD_VIRTUAL_DEI_CTRL:
        case PQ_CMD_VIRTUAL_DEI_GLOBAL_MOTION_CTRL:
        case PQ_CMD_VIRTUAL_DEI_DIR_CHECK:
        case PQ_CMD_VIRTUAL_DEI_DIR_MULTI:
        case PQ_CMD_VIRTUAL_DEI_INTP_SCALE_RATIO:
        case PQ_CMD_VIRTUAL_DEI_INTP_CTRL:
        case PQ_CMD_VIRTUAL_DEI_JITTER_MOTION:
        case PQ_CMD_VIRTUAL_DEI_FIELD_MOTION:
        case PQ_CMD_VIRTUAL_DEI_MOTION_RATIO_CURVE:
        case PQ_CMD_VIRTUAL_DEI_IIR_MOTION_CURVE:
        case PQ_CMD_VIRTUAL_DEI_REC_MODE:
        case PQ_CMD_VIRTUAL_DEI_HIST_MOTION:
        case PQ_CMD_VIRTUAL_DEI_MOR_FLT:
        case PQ_CMD_VIRTUAL_DEI_OPTM_MODULE:
        case PQ_CMD_VIRTUAL_DEI_COMB_CHK:
        case PQ_CMD_VIRTUAL_DEI_CRS_CLR:
        {
            PQ_DRV_SetDeiParam(arg);
            break;
        }
        case PQ_CMD_VIRTUAL_FMD_CTRL:
        case PQ_CMD_VIRTUAL_FMD_HISTBIN:
        case PQ_CMD_VIRTUAL_FMD_PCCTHD:
        case PQ_CMD_VIRTUAL_FMD_PCCBLK:
        case PQ_CMD_VIRTUAL_FMD_UMTHD:
        case PQ_CMD_VIRTUAL_FMD_ITDIFF:
        case PQ_CMD_VIRTUAL_FMD_LAST:
        {
            PQ_DRV_SetFmdParam(arg);
            break;
        }
        case PQ_CMD_VIRTUAL_DNR_CTRL:
        case PQ_CMD_VIRTUAL_DNR_DB_FILTER:
        case PQ_CMD_VIRTUAL_DNR_DR_FILTER:
        case PQ_CMD_VIRTUAL_DNR_INFO:
        {
            PQ_DRV_SetDnrParam(arg);
            break;
        }
        case PQ_CMD_VIRTUAL_SHARP_VHD_LUMA:
        case PQ_CMD_VIRTUAL_SHARP_VSD_LUMA:
        case PQ_CMD_VIRTUAL_SHARP_STR_LUMA:
        case PQ_CMD_VIRTUAL_SHARP_VHD_CHROMA:
        case PQ_CMD_VIRTUAL_SHARP_VSD_CHROMA:
        case PQ_CMD_VIRTUAL_SHARP_STR_CHROMA:
        {
            PQ_DRV_SetVpssSharpParam(arg);
            break;
        }
        case PQ_CMD_VIRTUAL_SHARP_GP0_LUMA:
        case PQ_CMD_VIRTUAL_SHARP_GP1_LUMA:
        case PQ_CMD_VIRTUAL_SHARP_GP0_CHROMA:
        case PQ_CMD_VIRTUAL_SHARP_GP1_CHROMA:
        {
            PQ_DRV_SetGfxSharpParam(arg);
            break;
        }
        case PQ_CMD_VIRTUAL_CSC_VDP_V0:
        case PQ_CMD_VIRTUAL_CSC_VDP_V1:
        case PQ_CMD_VIRTUAL_CSC_VDP_V3:
        case PQ_CMD_VIRTUAL_CSC_VDP_V4:
        {
            PQ_DRV_SetVoCscParam(arg);
            break;
        }
        case PQ_CMD_VIRTUAL_CSC_VDP_G0:
        case PQ_CMD_VIRTUAL_CSC_VDP_G1:
        {
            PQ_DRV_SetGfxCscParam(arg);
            break;
        }
        case PQ_CMD_VIRTUAL_CSC_VDP_WBC_DHD0:
        case PQ_CMD_VIRTUAL_CSC_VDP_INTER_DHD0:
        case PQ_CMD_VIRTUAL_CSC_VDP_INTER_DHD1:
        {
            PQ_DRV_SetDispCscParam(arg);
            break;
        }
        case PQ_CMD_VIRTUAL_ACC_MOD_GENTLE:
        case PQ_CMD_VIRTUAL_ACC_MOD_MIDDLE:
        case PQ_CMD_VIRTUAL_ACC_MOD_STRONG:
        case PQ_CMD_VIRTUAL_ACC_CTRL:
        {
            PQ_DRV_SetAccParam(arg);
            break;
        }
        case PQ_CMD_VIRTUAL_ACM_MOD_BLUE:
        case PQ_CMD_VIRTUAL_ACM_MOD_GREEN:
        case PQ_CMD_VIRTUAL_ACM_MOD_BG:
        case PQ_CMD_VIRTUAL_ACM_MOD_SKIN:
        case PQ_CMD_VIRTUAL_ACM_MOD_VIVID:
        case PQ_CMD_VIRTUAL_ACM_CTRL:
        {
            PQ_DRV_SetAcmParam(arg);
            break;
        }
        case PQ_CMD_VIRTUAL_GAMMA_RGB_MOD0:
        case PQ_CMD_VIRTUAL_GAMMA_RGB_MOD1:
        case PQ_CMD_VIRTUAL_GAMMA_RGB_MOD2:
        case PQ_CMD_VIRTUAL_GAMMA_RGB_MOD3:
        case PQ_CMD_VIRTUAL_GAMMA_YUV_MOD0:
        case PQ_CMD_VIRTUAL_GAMMA_YUV_MOD1:
        case PQ_CMD_VIRTUAL_GAMMA_YUV_MOD2:
        case PQ_CMD_VIRTUAL_GAMMA_YUV_MOD3:
        {
            PQ_DRV_SetGammaParam(arg);
            break;
        }
        case PQ_CMD_VIRTUAL_GAMMA_CTRL_DISPLAY0:
        case PQ_CMD_VIRTUAL_GAMMA_CTRL_DISPLAY1:
        {
            PQ_DRV_SetGammaCtrlParam(arg);
            break;
        }
        case PQ_CMD_VIRTUAL_DITHER_COEF:
        {
            PQ_DRV_SetDitherParam(arg);
            break;
        }
        case PQ_CMD_VIRTUAL_BIN_IMPORT:
        {
            arg->u_Data.u32PqDriverBinPhyAddr = g_stPqBinBuf.u32StartPhyAddr;
            break;
        }
        case PQ_CMD_VIRTUAL_BIN_FIXED:
        {
            arg->u_Data.u32PqDriverBinPhyAddr = g_stPqBinBuf.u32StartPhyAddr;
            break;

        }
        default:
        {
            HI_TRACE(HI_LOG_LEVEL_ERROR, HI_ID_PQ, "Error,PQ_DRV_ReadBin,unknown address = %x\n", u32PqCmd);
            return HI_FAILURE;
        }
    }
    return HI_SUCCESS;
}
HI_S32 PQ_DRV_ProcessCmd(unsigned int cmd, HI_VOID* arg)
{
    HI_S32 s32Ret = HI_SUCCESS;

    if (HI_FALSE == g_bLoadPqBin)
    {
        HI_ERR_PQ("\nPq driver get flash data fail  \r\n");
        return HI_FAILURE;
    }

    switch (cmd)
    {
        case CMD_IOC_SET_PQ:
        {
            s32Ret = PQ_DRV_ProcessSetCmd((PQ_IO_S*) arg);
            break;
        }
        case CMD_IOC_GET_PQ:
        {
            s32Ret = PQ_DRV_ProcessGetCmd((PQ_IO_S*) arg);
            break;
        }
        default:
        {
            HI_ERR_PQ("\nPq Ioctrl unknown command = %x \r\n", cmd);
            break;
        }

    }

    return s32Ret;
}

HI_S32 PQ_Ioctl(struct inode* inode, struct file* file, unsigned int cmd, HI_VOID* arg)
{
    HI_S32          Ret = HI_SUCCESS;

    DRV_PQ_Lock();

    Ret = PQ_DRV_ProcessCmd(cmd, arg);

    DRV_PQ_UnLock();

    return Ret;
}

static long PQ_DRV_Ioctl(struct file* ffile, unsigned int cmd, unsigned long arg)
{
    HI_S32 Ret;

    Ret = HI_DRV_UserCopy(ffile->f_dentry->d_inode, ffile, cmd, arg, PQ_Ioctl);

    return Ret;
}

HI_S32 PQ_Suspend(PM_BASEDEV_S* pdev, pm_message_t state)
{
    return HI_SUCCESS;
}

HI_S32 PQ_Resume(PM_BASEDEV_S* pdev)
{
    return HI_SUCCESS;
}

static PQ_REGISTER_PARAM_S g_PQProcPara =
{
    .rdproc = PQ_ProcRead,
    .wtproc = PQ_ProcWrite,
};

static UMAP_DEVICE_S g_PQRegisterData;


static struct file_operations g_PQFops =
{
    .owner          =    THIS_MODULE,
    .open           =     PQ_DRV_Open,
    .unlocked_ioctl =    PQ_DRV_Ioctl,
    .release        =  PQ_DRV_Close,
};

static PM_BASEOPS_S g_PQDrvOps =
{
    .probe        = NULL,
    .remove       = NULL,
    .shutdown     = NULL,
    .prepare      = NULL,
    .complete     = NULL,
    .suspend      = PQ_Suspend,
    .suspend_late = NULL,
    .resume_early = NULL,
    .resume       = PQ_Resume,
};

#if 0
HI_S32 HI_DRV_PQ_Init(HI_VOID)
{
    HI_S32  s32Ret;

    g_bLoadPqBin = HI_FALSE;
    g_pstPqParam	= HI_NULL;
    s32Ret = HI_DRV_MODULE_Register(HI_ID_PQ, PQ_NAME, (HI_VOID*)&g_PqExportFuncs);

    if (HI_SUCCESS != s32Ret)
    {
        HI_FATAL_PQ("ERR: HI_DRV_MODULE_Register!\n"); //181
        return s32Ret;
    }

    s32Ret = HI_DRV_MMZ_AllocAndMap("PQ_FLASH_BIN", HI_NULL, sizeof(PQ_PARAM_S), 0, (MMZ_BUFFER_S*)(&g_stPqBinBuf));
    if (HI_SUCCESS != s32Ret)
    {
        HI_FATAL_PQ("ERR: Pqdriver mmz memory failed!\n"); //188
        g_bLoadPqBin = HI_FALSE;
        return s32Ret;
    }

    g_pstPqParam = (PQ_PARAM_S*)g_stPqBinBuf.u32StartVirAddr;
    //HI_INFO_PQ("\r\ng_stPqBinBuf.u32StartVirAddr = %x,g_stPqBinBuf.u32StartVirAddr = %x\r\n", g_stPqBinBuf.u32StartPhyAddr, g_stPqBinBuf.u32StartVirAddr);
    HI_INFO_PQ("\ng_stPqBinBuf.u32StartVirAddr = 0x%x,g_stPqBinBuf.u32StartVirAddr = 0x%x\n", g_stPqBinBuf.u32StartPhyAddr, g_stPqBinBuf.u32StartVirAddr); //194
    s32Ret = PQ_DRV_GetFlashPqBin(g_pstPqParam);

    if (HI_SUCCESS != s32Ret)
    {
        HI_WARN_PQ("ERR: PQ_DRV_GetFlashPqBin failed!\n");
        g_bLoadPqBin = HI_FALSE;
        return s32Ret;
    }

    g_bLoadPqBin = HI_TRUE;
    return s32Ret;
}
#endif

HI_S32 HI_DRV_PQ_DeInit(HI_VOID)
{
    HI_DRV_MODULE_UnRegister(HI_ID_PQ);
    HI_DRV_MMZ_UnmapAndRelease((MMZ_BUFFER_S*)(&g_stPqBinBuf));
    return HI_SUCCESS;
}

HI_S32 PQ_DRV_ModInit(HI_VOID)
{
    HI_CHAR             ProcName[16];
    DRV_PROC_ITEM_S*     pProcItem = HI_NULL;

#ifndef HI_MCE_SUPPORT
#if 0
    HI_DRV_PQ_Init();
#endif
#endif

    HI_OSAL_Snprintf(ProcName, sizeof(ProcName), "%s", HI_MOD_PQ);

    pProcItem = HI_DRV_PROC_AddModule(ProcName, HI_NULL, HI_NULL);
    if (HI_NULL != pProcItem)
    {
        pProcItem->read = g_PQProcPara.rdproc;
        pProcItem->write = g_PQProcPara.wtproc;
    }

    HI_OSAL_Snprintf(g_PQRegisterData.devfs_name, sizeof(g_PQRegisterData.devfs_name), UMAP_DEVNAME_PQ);
    g_PQRegisterData.fops = &g_PQFops;
    g_PQRegisterData.minor = UMAP_MIN_MINOR_PQ;
    g_PQRegisterData.owner  = THIS_MODULE;
    g_PQRegisterData.drvops = &g_PQDrvOps;

    if (HI_DRV_DEV_Register(&g_PQRegisterData) < 0)
    {
        HI_FATAL_PQ("register PQ failed.\n");
        return HI_FAILURE;
    }

#ifdef MODULE
    HI_PRINT("Load hi_pq.ko success.\t\t(%s)\n", VERSION_STRING);
#endif

    return  HI_SUCCESS;
}

HI_VOID PQ_DRV_ModExit(HI_VOID)
{
    HI_CHAR             ProcName[16];

    HI_DRV_DEV_UnRegister(&g_PQRegisterData);

    HI_OSAL_Snprintf(ProcName, sizeof(ProcName), "%s", HI_MOD_PQ);
    HI_DRV_PROC_RemoveModule(ProcName);

#ifndef HI_MCE_SUPPORT
    HI_DRV_PQ_DeInit();
#endif

#ifdef MODULE
    HI_PRINT("Unload hi_pq.ko success.\t(%s)\n", VERSION_STRING);
#endif

    return;
}

#ifdef MODULE
module_init(PQ_DRV_ModInit);
module_exit(PQ_DRV_ModExit);

#endif


MODULE_AUTHOR("HISILICON");
MODULE_LICENSE("GPL");




