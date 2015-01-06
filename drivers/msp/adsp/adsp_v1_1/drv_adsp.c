/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

******************************************************************************
  File Name     : drv_adsp_intf_k.c
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2013/04/17
  Description   :
  History       :
  1.Date        : 2013/04/17
    Author      : zgjie
    Modification: Created file

******************************************************************************/

/******************************* Include Files *******************************/

/* Sys headers */
#include <linux/version.h>
#include <linux/proc_fs.h>
#include <linux/ioport.h>
#include <linux/string.h>
#include <linux/ioctl.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/vmalloc.h>
#include <linux/interrupt.h>
#include <linux/seq_file.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <mach/hardware.h>

/* Unf headers */
#include "hi_error_mpi.h"
#include "hi_reg_common.h"

/* Drv headers */
#include "drv_adsp_private.h"
#include "drv_adsp_ext.h"
#include "hi_audsp_common.h"
#include "hi_audsp_aoe.h"

#ifdef HI_SND_DSP_HW_SUPPORT

/*kfile operation*/
 #include "kfile_ops_func.h"
 #include "dsp_elf_func.h"
 #include <asm/cacheflush.h>
#endif
#ifdef __cplusplus
 #if __cplusplus
extern "C" {
 #endif
#endif /* __cplusplus */

/***************************** Macro Definition ******************************/
#if defined (HI_SND_AOE_SWSIMULATE_SUPPORT)
extern HI_S32 AOE_SwEngineCreate(HI_VOID);
extern HI_S32 AOE_SwEngineDestory(HI_VOID);
#endif


#define ADSP_NAME "HI_ADSP"


#ifdef HI_SND_DSP_HW_SUPPORT
 #define ADSP_ELF_USE_ARRAY //defaule config
//#define ADSP_ELF_USE_FILE
 #define ADSP_ELFNAME "/mnt/HWAOE"

 #define AOE_COMBUFFER_ADDR_ALIGN (32)
 #define ELF_MAXLENGTH (2 * 1024 * 1024)
 #define WAITING_LOOP 10
#endif

/*************************** Structure Definition ****************************/

/* Global parameter */
typedef struct
{
    atomic_t                atmOpenCnt;     /* Open times */
    HI_BOOL                 bReady;         /* Init flag */
    ADSP_REGISTER_PARAM_S*  pstProcParam;   /* ADSP Proc functions */

    ADSP_EXPORT_FUNC_S stExtFunc;      /* ADSP extenal functions */
    ADSP_SETTINGS_S stADSPSetting;
} ADSP_GLOBAL_PARAM_S;

#ifdef HI_SND_DSP_HW_SUPPORT
static volatile S_ADSP_CHN_REGS_TYPE *        g_pADSPSysCrg = NULL;
#endif


/***************************** Global Definition *****************************/

/***************************** Static Definition *****************************/
#ifdef HI_SND_DSP_HW_SUPPORT
static ADSP_AOESYS_CMD_RET_E  ADSP_AOE_Ack(HI_VOID)
{
    volatile HI_U32 loop = 0;
    U_ADSP_CTRL          ADSP_SYSCTRL;

    for (loop = 0; loop < 100; loop++)
    {
        //udelay(1000);
        msleep(1);
        ADSP_SYSCTRL.u32 = g_pADSPSysCrg->ADSP_SYSCTRL.u32;
        if (ADSP_SYSCTRL.bits.aoe_cmd_done)
        {
            return (ADSP_AOESYS_CMD_RET_E)ADSP_SYSCTRL.bits.aoe_cmd_return_value;
        }
    }

    return ADSP_AOE_CMD_ERR_TIMEOUT;
}

static HI_S32 ADSP_AOE_SetCmd(ADSP_AOESYS_CMD_E newcmd)
{
    ADSP_AOESYS_CMD_RET_E Ack;
    U_ADSP_CTRL          ADSP_SYSCTRL;
    ADSP_SYSCTRL.u32 = g_pADSPSysCrg->ADSP_SYSCTRL.u32;

    switch (newcmd)
    {
    case ADSP_AOE_CMD_START:
        ADSP_SYSCTRL.bits.aoe_cmd = ADSP_AOE_CMD_START;
        break;

    case ADSP_AOE_CMD_STOP:
        ADSP_SYSCTRL.bits.aoe_cmd = ADSP_AOE_CMD_STOP;
        break;

    default:

        //HI_WARN_AO("ADSP_AOE unknow Cmd(0x%x)",newcmd);
        return HI_SUCCESS;
    }

    ADSP_SYSCTRL.bits.aoe_cmd_done = 0;
    g_pADSPSysCrg->ADSP_SYSCTRL.u32 =  ADSP_SYSCTRL.u32;

    Ack = ADSP_AOE_Ack();
    if (ADSP_AOE_CMD_DONE != Ack)
    {
        HI_ERR_ADSP("\nADSP_AOE SetCmd(0x%x) failed(0x%x)", newcmd, Ack);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static ADSP_SYSCOM_CMD_RET_E  ADSP_SYS_Ack(HI_VOID)
{
    volatile HI_U32 loop = 0;
    U_ADSP_CTRL          ADSP_SYSCTRL;

    for (loop = 0; loop < 100; loop++)
    {
        //udelay(1000);
        msleep(1);
        ADSP_SYSCTRL.u32 = g_pADSPSysCrg->ADSP_SYSCTRL.u32;
        if (ADSP_SYSCTRL.bits.sys_cmd_done)
        {
            return (ADSP_SYSCOM_CMD_RET_E)ADSP_SYSCTRL.bits.sys_cmd_return_value;
        }
    }

    return ADSP_SYSCOM_CMD_ERR_TIMEOUT;
}

static HI_S32 ADSP_SYS_SetCmd(ADSP_SYSCOM_CMD_E newcmd, HI_BOOL bBlock)
{
    ADSP_AOESYS_CMD_RET_E Ack;
    U_ADSP_CTRL          ADSP_SYSCTRL;

    ADSP_SYSCTRL.u32 = g_pADSPSysCrg->ADSP_SYSCTRL.u32;
    
    switch (newcmd)
    {
    case ADSP_SYSCOM_CMD_START:
        ADSP_SYSCTRL.bits.sys_cmd = ADSP_SYSCOM_CMD_START;
        break;

    case ADSP_SYSCOM_CMD_STOP:
        ADSP_SYSCTRL.bits.sys_cmd = ADSP_SYSCOM_CMD_STOP;
        break;

    default:

        //HI_WARN_AO("ADSP_AOE unknow Cmd(0x%x)",newcmd);
        return HI_SUCCESS;
    }

    ADSP_SYSCTRL.bits.sys_cmd_done = 0;
    g_pADSPSysCrg->ADSP_SYSCTRL.u32 =  ADSP_SYSCTRL.u32;
    if (HI_TRUE == bBlock)
    {
        Ack = ADSP_SYS_Ack();
        if (ADSP_SYSCOM_CMD_DONE != Ack)
        {
            HI_ERR_ADSP("\nADSP_SYS SetCmd(0x%x) failed(0x%x)\n", newcmd, Ack);
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

#endif

static HI_S32 ADSP_LoadFirmware(ADSP_CODEID_E u32DspCodeId)
{
    HI_S32 sRet = HI_FAILURE;

    switch (u32DspCodeId)
    {
    case ADSP_CODE_AOE:
#if defined (HI_SND_AOE_SWSIMULATE_SUPPORT)
        sRet = AOE_SwEngineCreate();
#else
        sRet = ADSP_AOE_SetCmd(ADSP_AOE_CMD_START);
#endif

        break;
    default:
        HI_ERR_ADSP("dont support DspCode(%d)\n", u32DspCodeId);
        break;
    }

    return sRet;
}

static HI_S32 ADSP_UnLoadFirmware(ADSP_CODEID_E u32DspCodeId)
{
    HI_S32 sRet = HI_SUCCESS;

    switch (u32DspCodeId)
    {
    case ADSP_CODE_AOE:
#if defined (HI_SND_AOE_SWSIMULATE_SUPPORT)
        sRet = AOE_SwEngineDestory();
#else
        sRet = ADSP_AOE_SetCmd(ADSP_AOE_CMD_STOP);
#endif
        break;
    default:
        HI_WARN_ADSP("dont support DspCode(%d)\n", u32DspCodeId);
        break;
    }

    return sRet;
}

static HI_S32 ADSP_GetFirmwareInfo(ADSP_CODEID_E u32DspCodeId, ADSP_FIRMWARE_INFO_S* pstFrm)
{
    if ((HI_NULL == pstFrm))
    {
        HI_ERR_ADSP("Bad param!\n");
        return HI_FAILURE;
    }

    // todo

    return HI_SUCCESS;
}

static HI_S32 ADSP_GetAoeFwmInfo(ADSP_CODEID_E u32DspCodeId, ADSP_FIRMWARE_AOE_INFO_S* pstInfo)
{
    HI_S32 Ret = HI_SUCCESS;

    switch (u32DspCodeId)
    {
    case ADSP_CODE_AOE:
#if defined (HI_SND_AOE_SWSIMULATE_SUPPORT)
        pstInfo->bAoeSwFlag = HI_TRUE;
#else
        pstInfo->bAoeSwFlag = HI_FALSE;
#endif
        break;
    default:
        HI_ERR_ADSP("dont support DspCode(%d)\n", u32DspCodeId);
        Ret= HI_FAILURE;
        break;
    }

    return Ret;
}

static ADSP_GLOBAL_PARAM_S s_stAdspDrv =
{
    .atmOpenCnt                    = ATOMIC_INIT(0),
    .bReady                        = HI_FALSE,

    /*
        .stDefCfg =
        {
            .enType         = HI_UNF_VCODEC_TYPE_H264,
            .enMode         = HI_UNF_VCODEC_MODE_NORMAL,
            .u32ErrCover    = 100,
            .bOrderOutput   = 0,
            .u32Priority    = 15
        },
     */
    .pstProcParam                  = HI_NULL,

    //.pDmxFunc = HI_NULL,
    //.pVfmwFunc = HI_NULL,
    //.pfnWatermark = HI_NULL,
    .stExtFunc                     =
    {
        .pfnADSP_LoadFirmware      = ADSP_LoadFirmware,
        .pfnADSP_UnLoadFirmware    = ADSP_UnLoadFirmware,
        .pfnADSP_GetFirmwareInfo   = ADSP_GetFirmwareInfo,
        .pfnADSP_GetAoeFwmInfo     = ADSP_GetAoeFwmInfo,
    }
};

/*********************************** Code ************************************/
#ifdef HI_SND_DSP_HW_SUPPORT

 #if defined ADSP_ELF_USE_ARRAY
HI_UCHAR dsp_elf_array[] = {
  #include "firmware/hififw.dat"
};
 #endif

static HI_S32 Dsp_LoadElf(HI_VOID)
{
    HI_UCHAR *pcELFBuf = HI_NULL;
    pcELFBuf = HI_VMALLOC(ADSP_ID_AOE, ELF_MAXLENGTH);
    if(HI_NULL == pcELFBuf)
    {
        HI_FATAL_ADSP("pcELFBuf HI_VMALLOC fail\n");
        return HI_FAILURE;
    }
    memset((HI_VOID*)pcELFBuf, 0, ELF_MAXLENGTH);

 #ifdef  ADSP_ELF_USE_FILE
    //1. read elf file
    struct file *fpELF;
    HI_U32 u32readsize = 0;
    fpELF = kfile_open(ADSP_ELFNAME, O_RDONLY, 0);
    if (HI_NULL == fpELF)
    {
        HI_FATAL_ADSP("ELF file %s open fail\n", ADSP_ELFNAME);
        HI_VFREE(ADSP_ID_AOE, pcELFBuf);
        return HI_FAILURE;
    }

    u32readsize = kfile_read(pcELFBuf, ELF_MAXLENGTH, fpELF);
    if (u32readsize <= 0)
    {
        HI_FATAL_ADSP("ELF file  read fail\n");
        kfile_close(fpELF);
        HI_VFREE(ADSP_ID_AOE, pcELFBuf);
        return -EACCES;
    }

    kfile_close(fpELF);
    HI_INFO_ADSP("Read ELF file size 0x%x \n", u32readsize);

 #elif defined ADSP_ELF_USE_ARRAY
    memcpy(pcELFBuf, dsp_elf_array, sizeof(dsp_elf_array));
    HI_INFO_ADSP("Read ELF array size 0x%x \n", sizeof(dsp_elf_array));
 #endif

    //2. load elf file
    //copy elf to reg and ddr
    CopyELFSection(pcELFBuf);

    //flush cache to ddr (all size)
    __cpuc_flush_dcache_area(phys_to_virt(DSP_DDR_CODE_BASEADDR), DSP_DDR_CODE_MAXSIZE);
    outer_flush_range(DSP_DDR_CODE_BASEADDR, DSP_DDR_CODE_MAXSIZE);

    //check elf copyed right
    if (CheckELFPaser(pcELFBuf) == HI_FAILURE)
    {
        HI_FATAL_ADSP("ELF Load  fail\n");
        HI_VFREE(ADSP_ID_AOE, pcELFBuf);
        return HI_FAILURE;
    }

    HI_VFREE(ADSP_ID_AOE, pcELFBuf);
    
    return HI_SUCCESS;
}

static HI_S32 Dsp_ResetBoot(HI_VOID)
{
    U_DSP0_CTRL uTmpVal;
    U_PERI_CRG52 uTmpVal1;

    //volatile HI_U32 u32RunFlagCheck = 0;
    //volatile HI_U32 u32WaitingLoop = 0;
    //HI_U32 *pdsp0_running_flag_addr = HI_NULL;
    HI_S32 s32Ret;
    //remap reg
    
    //3. reset dsp
    uTmpVal.u32 = g_pstRegPeri->DSP0_CTRL.u32;
    uTmpVal.bits.wdg1_en_dsp0 = 0;
    uTmpVal.bits.ocdhaltonreset_dsp0 = 0;
    
 #if 1  //boot from bootreg
    uTmpVal.bits.statvectorsel_dsp0 = 0;
 #else //boot from ddr
    uTmpVal.bits.statvectorsel_dsp0 = 1;
 #endif
    uTmpVal.bits.runstall_dsp0 = 0;
    g_pstRegPeri->DSP0_CTRL.u32 = uTmpVal.u32;

    uTmpVal1.u32 = g_pstRegCrg->PERI_CRG52.u32;

    uTmpVal1.u32 = 0x77;
    uTmpVal1.bits.dsp_clk_sel = 0;  //345.6M
    //uTmpVal1..bits.dsp_cken = 1;
    uTmpVal1.bits.dsp0_cken = 1;   //dsp0 clk enable
    //uTmpVal1.bits.dsp1_cken = 0;   //dsp1 clk disable
    g_pstRegCrg->PERI_CRG52.u32 = uTmpVal1.u32;

    uTmpVal1.u32 = g_pstRegCrg->PERI_CRG52.u32;
    uTmpVal1.bits.dsp_srst_req  = 1;  //dsp bus sreset
    uTmpVal1.bits.dsp0_srst_req = 1;   //dsp0 sreset
    //uTmpVal1.bits.dsp1_srst_req = 1;   //dsp0 sreset
    g_pstRegCrg->PERI_CRG52.u32 = uTmpVal1.u32;

    uTmpVal1.u32 = g_pstRegCrg->PERI_CRG52.u32;
    uTmpVal1.bits.dsp_srst_req  = 0;
    uTmpVal1.bits.dsp0_srst_req = 0;
    g_pstRegCrg->PERI_CRG52.u32 = uTmpVal1.u32;

    //4. check dsp running
 #if 1
     s32Ret = ADSP_SYS_SetCmd(ADSP_SYSCOM_CMD_START, HI_TRUE);
     if (HI_SUCCESS != s32Ret)
     {
         HI_FATAL_ADSP("Dsp start  fail: 0x%x!\n", s32Ret);
         return s32Ret;
     }
 #else
    HI_INFO_ADSP("u32RunningFlagVirtAddr=0x%x  \n", u32RunningFlagVirtAddr);
    pdsp0_running_flag_addr = (HI_U32 *)u32RunningFlagVirtAddr;

    if (HI_NULL == pdsp0_running_flag_addr)
    {
        HI_FATAL_ADSP("IoRemap Dsp0 Running Flag addr  fail\n");
        return HI_FAILURE;
    }

    while (0 == u32RunFlagCheck)
    {
        u32WaitingLoop++;
        if (*pdsp0_running_flag_addr != AOE_RUNNING_FLAG)
        {
            if (u32WaitingLoop == WAITING_LOOP)
            {
                HI_FATAL_ADSP("Dsp0 Start TimeOut \n");
                break;
            }
            else
            {
                udelay(1000);
            }
        }
        else
        {
            u32RunFlagCheck = 1;
        }
    }

    if (0 != u32RunFlagCheck)
    {
        HI_INFO_ADSP("Dsp0 running, And The Value 0x%x \n", *(volatile HI_U32 *)pdsp0_running_flag_addr);
    }
    else
    {
        HI_FATAL_ADSP("Dsp0 running Failed  \n");
        return HI_FAILURE;
    }
 #endif

    return HI_SUCCESS;
}

#endif

HI_S32 ADSP_DRV_Open(struct inode *inode, struct file  *filp)
{
    if (atomic_inc_return(&s_stAdspDrv.atmOpenCnt) == 1)
    {}

    return HI_SUCCESS;
}

HI_S32 ADSP_DRV_Release(struct inode *inode, struct file  *filp)
{
    /* Not the last close, only close the channel match with the 'filp' */
    if (atomic_dec_return(&s_stAdspDrv.atmOpenCnt) != 0)
    {}
    /* Last close */
    else
    {}

    return HI_SUCCESS;
}

HI_S32 ADSP_DRV_RegisterProc(ADSP_REGISTER_PARAM_S *pstParam)
{
    // todo

    /* Check parameters */
    if (HI_NULL == pstParam)
    {
        return HI_FAILURE;
    }

    s_stAdspDrv.pstProcParam = pstParam;

    /* Create proc */

    return HI_SUCCESS;
}

HI_VOID ADSP_DRV_UnregisterProc(HI_VOID)
{
    // todo

    /* Unregister */

    /* Clear param */
    s_stAdspDrv.pstProcParam = HI_NULL;
    return;
}

HI_S32 ADSP_HW_PowerOn(HI_VOID)
{
#ifdef HI_SND_DSP_HW_SUPPORT
    HI_S32 s32Ret;
    U_ADSP_CTRL          ADSP_SYSCTRL;

    ADSP_SYSCTRL.u32 = g_pADSPSysCrg->ADSP_SYSCTRL.u32;
    ADSP_SYSCTRL.u32 = 0;
    ADSP_SYSCTRL.bits.sys_cmd_done = 1;
    ADSP_SYSCTRL.bits.aoe_cmd_done = 1;
    ADSP_SYSCTRL.bits.ade_cmd_done = 1;
    ADSP_SYSCTRL.bits.aee_cmd_done = 1;

    g_pADSPSysCrg->ADSP_SYSCTRL.u32 =  ADSP_SYSCTRL.u32;
    //HI_FATAL_ADSP("AdspCtrlReg: 0x%x!\n", g_pADSPSysCrg->ADSP_CtrlReg.u32);

    s32Ret = Dsp_LoadElf();
    if (HI_SUCCESS != s32Ret)
    {
        HI_FATAL_ADSP("Dsp load Elf  fail: 0x%x!\n", s32Ret);
        return s32Ret;
    }

    s32Ret = Dsp_ResetBoot();
    if (HI_SUCCESS != s32Ret)
    {
        HI_FATAL_ADSP("Dsp Boot  fail: 0x%x!\n", s32Ret);
        return s32Ret;
    }

#endif


    return HI_SUCCESS;
}

HI_VOID ADSP_HW_PowerOff(HI_VOID)
{
#ifdef HI_SND_DSP_HW_SUPPORT
    U_PERI_CRG52 uTmpVal;
    uTmpVal.u32 = g_pstRegCrg->PERI_CRG52.u32;
    uTmpVal.bits.dsp0_srst_req = 1;   //dsp0 sreset
    uTmpVal.bits.dsp0_cken = 0;   //dsp0 clk disable
    g_pstRegCrg->PERI_CRG52.u32 = uTmpVal.u32;

    ADSP_SYS_SetCmd(ADSP_SYSCOM_CMD_STOP, HI_FALSE);

#endif
}

static HI_VOID ADSP_GetSettings(HI_VOID)
{
}

static HI_VOID ADSP_RestoreSettings(HI_VOID)
{
}

HI_S32 ADSP_DRV_Suspend(PM_BASEDEV_S *pdev, pm_message_t state)
{
    ADSP_GetSettings();
    ADSP_HW_PowerOff();

    // todo
    HI_FATAL_ADSP("ok\n");
    return HI_SUCCESS;
}
HI_S32 HI_DRV_ADSP_Init(HI_VOID)
{
    return ADSP_DRV_Init();
}

HI_VOID HI_DRV_ADSP_DeInit(HI_VOID)
{
    ADSP_DRV_Exit();
}

HI_S32 ADSP_DRV_Resume(PM_BASEDEV_S *pdev)
{
    ADSP_RestoreSettings();
    if (HI_SUCCESS != ADSP_HW_PowerOn())
    {
        HI_FATAL_ADSP("Resume ADSP_HW_PowerOn fail \n");
        return HI_FAILURE;
    }

    // todo
    HI_FATAL_ADSP("ok\n");
    return HI_SUCCESS;
}

HI_S32 ADSP_DRV_Init(HI_VOID)
{
    HI_S32 s32Ret;

    s32Ret = HI_DRV_MODULE_Register(HI_ID_ADSP, ADSP_NAME, (HI_VOID*)&s_stAdspDrv.stExtFunc);
    if (HI_SUCCESS != s32Ret)
    {
        HI_FATAL_ADSP("Reg module fail:%#x!\n", s32Ret);
        return s32Ret;
    }
#ifdef HI_SND_DSP_HW_SUPPORT
        g_pADSPSysCrg = (volatile S_ADSP_CHN_REGS_TYPE * )ioremap_nocache(DSP0_SHARESRAM_CHAN0_BASEADDR, sizeof(S_ADSP_CHN_REGS_TYPE));
#endif


    if (HI_SUCCESS != ADSP_HW_PowerOn())
    {
        HI_FATAL_ADSP("ADSP_HW_PowerOn fail \n");
        return HI_FAILURE;
    }
    
    return HI_SUCCESS;
}

HI_VOID ADSP_DRV_Exit(HI_VOID)
{
    ADSP_HW_PowerOff();
    
#ifdef HI_SND_DSP_HW_SUPPORT
    if(g_pADSPSysCrg)
        iounmap((HI_VOID*)g_pADSPSysCrg);
#endif

    HI_DRV_MODULE_UnRegister(HI_ID_ADSP);

    return;
}

#ifdef __cplusplus
 #if __cplusplus
}
 #endif
#endif /* End of #ifdef __cplusplus */
