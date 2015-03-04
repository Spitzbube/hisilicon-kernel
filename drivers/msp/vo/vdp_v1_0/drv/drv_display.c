
/******************************************************************************
  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.
******************************************************************************
File Name     : drv_display.h
Version       : Initial Draft
Author        : Hisilicon multimedia software group
Created       : 2012/12/30
Last Modified :
Description   :
Function List :
History       :
******************************************************************************/

#include "drv_display.h"
#include "drv_disp_com.h"
#include "drv_disp_priv.h"
#include "drv_disp_hal.h"
#include "drv_disp_da.h"

#ifdef __DISP_PLATFORM_BOOT__
#include "hi_drv_hdmi.h"
#endif

#ifdef HI_DISP_BUILD_FULL
#include "drv_disp_isr.h"
#include "drv_disp_ua.h"
#include "drv_disp_cast.h"
#include "hi_drv_module.h"
#include "drv_hdmi_ext.h"
#include "hi_drv_sys.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/******************************************************************************
    global object
******************************************************************************/
static volatile HI_S32 s_s32DisplayGlobalFlag = DISP_DEVICE_STATE_CLOSE;
static DISP_DEV_S s_stDisplayDevice;

/* OPTM ISR handle */
HI_U32 g_DispIrqHandle = 0;

#ifdef __DISP_PLATFORM_BOOT__

#else
static HDMI_EXPORT_FUNC_S* s_pstHDMIFunc;
#endif


/******************************************************************************
    local function and macro
******************************************************************************/
#define DEF_DRV_DISP_INTER_FUNCTION_AND_MACRO_START_HERE

#define DispCheckDeviceState()    \
    {                                \
        if (DISP_DEVICE_STATE_OPEN != s_s32DisplayGlobalFlag)  \
        {                            \
            DISP_ERROR("DISP ERROR! DISP is not inited in %s!\n", __FUNCTION__); \
            return HI_ERR_DISP_NO_INIT;  \
        }                             \
    }

#define DispCheckCastHandleValid(handle)    \
    {                                \
        if ( ((handle >> 16) != HI_ID_DISP)      \
             || (((handle & 0xffff) != HI_DRV_DISPLAY_0)    \
                 &&((handle & 0xffff) != HI_DRV_DISPLAY_1)    \
                 && ((handle & 0xffff) != HI_DRV_DISPLAY_2)) )  \
        {                                                  \
            DISP_ERROR("DISP ERROR! bad cast handle in  %s!\n", __FUNCTION__); \
            return HI_ERR_DISP_INVALID_PARA;  \
        }                             \
    }


#define DispCheckNullPointer(ptr) \
    {                                \
        if (!ptr)                    \
        {                            \
            DISP_ERROR("DISP ERROR! Input null pointer in %s!\n", __FUNCTION__); \
            return HI_ERR_DISP_NULL_PTR;  \
        }                             \
    }

#if 0

#define DispCheckID(id)    \
    {                                \
        if ( (id > HI_DRV_DISPLAY_BUTT) || (id > HI_DRV_DISPLAY_BUTT))  \
        {                            \
            DISP_ERROR("DISP ERROR! Invalid display in %s!\n", __FUNCTION__); \
            return HI_ERR_DISP_INVALID_PARA;  \
        }                             \
    }
#endif

#define DispGetPointerByID(id, ptr)    \
    {                                      \
        if (id >= HI_DRV_DISPLAY_BUTT)     \
        {                                     \
            DISP_ERROR("DISP ERROR! Invalid display in %s!\n", __FUNCTION__); \
            return HI_ERR_DISP_INVALID_PARA;  \
        }                                     \
        ptr = &s_stDisplayDevice.stDisp[id - HI_DRV_DISPLAY_0]; \
    }

#define DispGetPointerByIDNoReturn(id, ptr)    \
    {                                      \
        if (id >= HI_DRV_DISPLAY_BUTT)     \
        {                                     \
            DISP_ERROR("DISP ERROR! Invalid display in %s!\n", __FUNCTION__); \
        }                                     \
        ptr = &s_stDisplayDevice.stDisp[id - HI_DRV_DISPLAY_0]; \
    }


#define DispShouldBeOpened(id) \
    {                              \
        if(!DISP_IsOpened(id))     \
        {                          \
            DISP_ERROR("DISP ERROR! Display is not opened!\n"); \
            return HI_ERR_DISP_NOT_OPEN;  \
        }                          \
    }

HI_VOID DispSetHardwareState(HI_VOID)
{
    s_stDisplayDevice.bHwReseted = HI_TRUE;
    return;
}

HI_VOID DispClearHardwareState(HI_VOID)
{
    s_stDisplayDevice.bHwReseted = HI_FALSE;
    return;
}


HI_VOID DispResetHardware(HI_VOID)
{
    if (!s_stDisplayDevice.bHwReseted)
    {
        s_stDisplayDevice.stIntfOpt.PF_ResetHardware();
        s_stDisplayDevice.bHwReseted = HI_TRUE;

        DISP_WARN("========DispResetHardware=======\n");
    }
}
HI_VOID DispCloseClkResetModule(HI_VOID)
{
    s_stDisplayDevice.stIntfOpt.PF_CloseClkResetModule();    
}


#ifndef __DISP_PLATFORM_BOOT__
HI_S32 DispGetHdmiFunction(void)
{
    HI_S32 nRet = HI_SUCCESS;

    s_pstHDMIFunc = HI_NULL;
    nRet = HI_DRV_MODULE_GetFunction(HI_ID_HDMI, (HI_VOID**)&s_pstHDMIFunc);
    
    if ((nRet != HI_SUCCESS) || (s_pstHDMIFunc == HI_NULL))
    {
        DISP_ERROR("DISP_get HDMI func failed!");
        return HI_FAILURE;
    }
    return nRet;
}
#endif

HI_S32 DispSearchCastHandle(HI_HANDLE* cast_ptr, HI_DRV_DISPLAY_E penDisp)
{
    DISP_S* pstDisp;

    if (DISP_IsOpened(penDisp))
    {
        DispGetPointerByID(penDisp, pstDisp);

        if (pstDisp->Cast_ptr != 0)
        {
            *cast_ptr = pstDisp->Cast_ptr;
            return HI_SUCCESS;
        }
    }

    return HI_FAILURE;
}

HI_S32 DispCheckMaster(HI_DRV_DISPLAY_E enDisp, HI_BOOL bDetach)
{
    if (enDisp != HI_DRV_DISPLAY_0)
    {
        DISP_ERROR("DISP ERROR! Not support display %d as master!\n", (HI_S32)enDisp);
        return HI_ERR_DISP_INVALID_PARA;
    }

    if (bDetach)
    {
        // if detach, check  'eMaster is enDisp'
        if (s_stDisplayDevice.stAttchDisp.eMaster != enDisp)
        {
            DISP_ERROR("DISP ERROR! Display %d is not master!\n", (HI_S32)enDisp);
            return HI_ERR_DISP_INVALID_PARA;
        }
    }
    else
    {
        // if attach, check  'eMaster is exist'; only support one master diaplay.
        if (s_stDisplayDevice.stAttchDisp.eMaster != HI_DRV_DISPLAY_BUTT)
        {
            DISP_ERROR("DISP ERROR! Display master has existed!\n");
            return HI_ERR_DISP_INVALID_OPT;
        }
    }

    return HI_SUCCESS;
}

HI_S32 DispCheckSlace(HI_DRV_DISPLAY_E enDisp, HI_BOOL bDetach)
{
    if (enDisp != HI_DRV_DISPLAY_1)
    {
        DISP_ERROR("DISP ERROR! Not support display %d as slave!\n", (HI_S32)enDisp);
        return HI_ERR_DISP_INVALID_PARA;
    }

    if (bDetach)
    {
        // if detach, check  'eSlave is enDisp'
        if (s_stDisplayDevice.stAttchDisp.eSlave != enDisp)
        {
            DISP_ERROR("DISP ERROR! Display %d is not slave!\n", (HI_S32)enDisp);
            return HI_ERR_DISP_INVALID_PARA;
        }
    }
    else
    {
        // if attach, check  'eSlave is exist'; only support one slave diaplay.
        if (s_stDisplayDevice.stAttchDisp.eSlave != HI_DRV_DISPLAY_BUTT)
        {
            DISP_ERROR("DISP ERROR! Display slave has existed!\n");
            return HI_ERR_DISP_INVALID_OPT;
        }
    }

    return HI_SUCCESS;
}


HI_BOOL DispFmtIsStandDefinition(HI_DRV_DISP_FMT_E enEncFmt)
{
    if ((enEncFmt >= HI_DRV_DISP_FMT_PAL) && (enEncFmt <= HI_DRV_DISP_FMT_1440x480i_60) )
    {
        return HI_TRUE;
    }
    else
    {
        return HI_FALSE;
    }
}

HI_DRV_DISP_FMT_E DispTransferFormat(HI_DRV_DISPLAY_E enDisp, HI_DRV_DISP_FMT_E enEncFmt)
{
    if (HI_DRV_DISPLAY_1 == enDisp)
    {
        if (   (HI_DRV_DISP_FMT_PAL <= enEncFmt)
               && (HI_DRV_DISP_FMT_PAL_Nc >= enEncFmt) )
        {
            return HI_DRV_DISP_FMT_1440x576i_50;
        }

        if (   (HI_DRV_DISP_FMT_SECAM_SIN <= enEncFmt)
               && (HI_DRV_DISP_FMT_SECAM_H >= enEncFmt) )
        {
            return HI_DRV_DISP_FMT_1440x576i_50;
        }

        if (   (HI_DRV_DISP_FMT_PAL_M <= enEncFmt)
               && (HI_DRV_DISP_FMT_NTSC_443 >= enEncFmt) )
        {
            return HI_DRV_DISP_FMT_1440x480i_60;
        }

        return enEncFmt;
    }
    else
    {
        return enEncFmt;
    }
}

HI_S32 DispSetMasterAndSlace(HI_DRV_DISPLAY_E enMaster, HI_DRV_DISPLAY_E enSlave)
{
    s_stDisplayDevice.stAttchDisp.eMaster = enMaster;
    s_stDisplayDevice.stAttchDisp.eSlave  = enSlave;

    return HI_SUCCESS;
}

HI_S32 DispCheckReadyForOpen(HI_DRV_DISPLAY_E enDisp)
{
    DISP_S* pstDisp;
    DispGetPointerByID(enDisp, pstDisp);
    
    if (HI_DRV_DISP_FMT_BUTT == pstDisp->stSetting.enFormat)
    {
        DISP_WARN("WARNING! Fmt is not set!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/*==========================================================================
    interface control
*/
HI_VOID DispCleanIntf(DISP_INTF_S* pstIntf)
{
    DISP_MEMSET(pstIntf, 0, sizeof(DISP_INTF_S));

    pstIntf->bOpen = HI_FALSE;
    pstIntf->bLinkVenc = HI_FALSE;
    pstIntf->eVencId = DISP_VENC_MAX;

    /*
        for(i=0; i<DISP_VENC_SIGNAL_MAX_NUMBER; i++)
        {
            pstIntf->eSignal[i] = HI_DRV_DISP_VDAC_NONE;
        }
    */
    pstIntf->stIf.eID = HI_DRV_DISP_INTF_ID_MAX;
    pstIntf->stIf.u8VDAC_Y_G  = HI_DISP_VDAC_INVALID_ID;
    pstIntf->stIf.u8VDAC_Pb_B = HI_DISP_VDAC_INVALID_ID;
    pstIntf->stIf.u8VDAC_Pr_R = HI_DISP_VDAC_INVALID_ID;
    pstIntf->stIf.bDacSync = HI_TRUE;

    return;
}

HI_VOID DispCleanAllIntf(DISP_S* pstDisp)
{
    HI_S32 i;

    for (i = 0; i < (HI_S32)HI_DRV_DISP_INTF_ID_MAX; i++)
    {
        DispCleanIntf(&pstDisp->stSetting.stIntf[i]);
    }

    return;
}


/*if dac is busy ,release the  interface ,in order to used for new config is valid!*/
HI_S32 DispPrepareVDAC( HI_U32 u32DacId)
{
    HI_U32 i, j;
    DISP_S* pstDisp;
    DISP_INTF_S* pstIt;

    DISP_INTF_OPERATION_S* pfOpt = DISP_HAL_GetOperationPtr();
    DispCheckNullPointer(pfOpt);
    DispCheckNullPointer(pfOpt->PF_ReleaseIntf2);

    for ( i = HI_DRV_DISPLAY_0; i < HI_DRV_DISPLAY_2 ; i++)
    {
        DispGetPointerByID((HI_DRV_DISPLAY_E) i, pstDisp);
        //printk("check  dac *********(%d)\n",u32DacId);

        for ( j = HI_DRV_DISP_INTF_YPBPR0; j <= HI_DRV_DISP_INTF_VGA0 ; j++)
        {
            pstIt = &pstDisp->stSetting.stIntf[j];
            //printk("*disp%d*[%d]***(%d)(%d)(%d)***(%d)\n",i,j,pstIt->stIf.u8VDAC_Y_G,pstIt->stIf.u8VDAC_Pb_B,pstIt->stIf.u8VDAC_Pr_R,pstDisp->stSetting.stIntf[i].bOpen);

            if ( (pstIt->stIf.u8VDAC_Y_G == u32DacId)
                 || (pstIt->stIf.u8VDAC_Pb_B == u32DacId)
                 || (pstIt->stIf.u8VDAC_Pr_R == u32DacId)
               )
            {
                // s1 release vdac
                //printk("del **disp%d*[%d]***(%d)(%d)(%d)***(%d)\n", i, j, pstIt->stIf.u8VDAC_Y_G, pstIt->stIf.u8VDAC_Pb_B, pstIt->stIf.u8VDAC_Pr_R, pstDisp->stSetting.stIntf[j].bOpen);
                pfOpt->PF_ReleaseIntf2(i, pstIt);
                /* clean */

                DispCleanIntf(pstIt);
                continue;
            }
        }
    }

    return HI_SUCCESS;
}
#ifdef __DISP_PLATFORM_BOOT__
HI_S32 DispPrepareHDMI( HI_DRV_DISP_INTF_ID_E enHDMIId)
{
    return HI_SUCCESS;
}
#else
HI_S32 DispPrepareHDMI( HI_DRV_DISP_INTF_ID_E enHDMIId)
{
    HI_U32 i;
    DISP_S* pstDisp;
    DISP_INTF_S* pstIt;

    DISP_INTF_OPERATION_S* pfOpt = DISP_HAL_GetOperationPtr();
    DispCheckNullPointer(pfOpt);
    DispCheckNullPointer(pfOpt->PF_ReleaseIntf2);

    for ( i = HI_DRV_DISPLAY_0; i < HI_DRV_DISPLAY_2 ; i++)
    {
        DispGetPointerByID((HI_DRV_DISPLAY_E) i, pstDisp);
        pstIt = &pstDisp->stSetting.stIntf[enHDMIId];

        if (!DispGetHdmiFunction()) {
            if (s_pstHDMIFunc->pfnHdmiPreFormat && s_pstHDMIFunc->pfnHdmiSetFormat)
            {
               s_pstHDMIFunc->pfnHdmiPreFormat(HI_UNF_HDMI_ID_0, pstDisp->stSetting.enFormat);
            }
        }
        
        pfOpt->PF_ReleaseIntf2(i, pstIt);
        DispCleanIntf(pstIt);
        continue;
    }

    return HI_SUCCESS;
}

#endif
#define DEF_DRV_DISP_INTERFACE_CONTROL_START_HERE

HI_S32 DispCheckIntfValid(HI_DRV_DISP_INTF_S* pstIntf)
{
    if (pstIntf->eID >= HI_DRV_DISP_INTF_ID_MAX)
    {
        return HI_FAILURE;
    }

    if (  (pstIntf->eID == HI_DRV_DISP_INTF_YPBPR0)
          || (pstIntf->eID == HI_DRV_DISP_INTF_VGA0)
          || (pstIntf->eID == HI_DRV_DISP_INTF_RGB0)
       )
    {
        if (  (pstIntf->u8VDAC_Y_G  >= HI_DISP_VDAC_MAX_NUMBER)
              || (pstIntf->u8VDAC_Pb_B >= HI_DISP_VDAC_MAX_NUMBER)
              || (pstIntf->u8VDAC_Pr_R >= HI_DISP_VDAC_MAX_NUMBER)
           )
        {
            return HI_FAILURE;
        }
    }

    if (pstIntf->eID == HI_DRV_DISP_INTF_SVIDEO0)
    {
        if (  (pstIntf->u8VDAC_Y_G  >= HI_DISP_VDAC_MAX_NUMBER)
              || (pstIntf->u8VDAC_Pb_B >= HI_DISP_VDAC_MAX_NUMBER)
              || (pstIntf->u8VDAC_Pr_R != HI_DISP_VDAC_INVALID_ID)
           )
        {
            return HI_FAILURE;
        }
    }

    if (pstIntf->eID == HI_DRV_DISP_INTF_CVBS0)
    {
        if (  (pstIntf->u8VDAC_Y_G  >= HI_DISP_VDAC_MAX_NUMBER)
              || (pstIntf->u8VDAC_Pb_B != HI_DISP_VDAC_INVALID_ID)
              || (pstIntf->u8VDAC_Pr_R != HI_DISP_VDAC_INVALID_ID)
           )
        {
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

HI_S32 DispPrepareInft(HI_DRV_DISP_INTF_S* pstIntf)
{
    switch (pstIntf->eID)
    {
        case HI_DRV_DISP_INTF_YPBPR0:
        case HI_DRV_DISP_INTF_VGA0:
        case HI_DRV_DISP_INTF_RGB0:
            DispPrepareVDAC(pstIntf->u8VDAC_Y_G);
            DispPrepareVDAC(pstIntf->u8VDAC_Pb_B);
            DispPrepareVDAC(pstIntf->u8VDAC_Pr_R);
            break;

        case HI_DRV_DISP_INTF_SVIDEO0:
            DispPrepareVDAC(pstIntf->u8VDAC_Y_G);
            DispPrepareVDAC(pstIntf->u8VDAC_Pb_B);
            break;

        case HI_DRV_DISP_INTF_CVBS0:
            DispPrepareVDAC(pstIntf->u8VDAC_Y_G);
            break;

        case HI_DRV_DISP_INTF_HDMI0:
        case HI_DRV_DISP_INTF_HDMI1:
        case HI_DRV_DISP_INTF_HDMI2:
            DispPrepareHDMI(pstIntf->eID);
            break;

        default:
            break;
    }

    return HI_SUCCESS;
}



HI_BOOL DispCheckIntfExist(HI_DRV_DISPLAY_E enDisp, HI_DRV_DISP_INTF_S* pstIntf)
{
    DISP_S* pstDisp;
    HI_DRV_DISP_INTF_S* pstIntf2;

    DispGetPointerByIDNoReturn(enDisp, pstDisp);

    pstIntf2 = &pstDisp->stSetting.stIntf[pstIntf->eID].stIf;

    if (pstIntf->eID >= HI_DRV_DISP_INTF_ID_MAX)
        return HI_FALSE;

    if (  (HI_TRUE == pstDisp->stSetting.stIntf[pstIntf->eID].bOpen)
          && (pstIntf->eID         == pstIntf2->eID)
          && (pstIntf->u8VDAC_Y_G  == pstIntf2->u8VDAC_Y_G)
          && (pstIntf->u8VDAC_Pb_B == pstIntf2->u8VDAC_Pb_B)
          && (pstIntf->u8VDAC_Pr_R == pstIntf2->u8VDAC_Pr_R)
       )
    {
        return HI_TRUE;
    }
    else
    {
        return HI_FALSE;
    }
}

HI_BOOL DispCheckIntfExistByType(HI_DRV_DISPLAY_E enDisp, HI_DRV_DISP_INTF_S* pstIntf)
{
    DISP_S* pstDisp;

    DispGetPointerByIDNoReturn(enDisp, pstDisp);

    if (HI_TRUE == pstDisp->stSetting.stIntf[pstIntf->eID].bOpen)
    {
        return HI_TRUE;
    }
    else
    {
        return HI_FALSE;
    }
}

DISP_INTF_S* DispGetIntfPtr(DISP_S* pstDisp, HI_DRV_DISP_INTF_ID_E eID)
{
    return &pstDisp->stSetting.stIntf[eID];
}

HI_S32 DispAddIntf(DISP_S* pstDisp, HI_DRV_DISP_INTF_S* pstIntf)
{
    DISP_INTF_S* pstIt = &pstDisp->stSetting.stIntf[pstIntf->eID];
    DISP_INTF_OPERATION_S* pfOpt = DISP_HAL_GetOperationPtr();
    HI_DRV_DISP_INTF_S stBackup;
    HI_BOOL bBkFlag = HI_FALSE;
    HI_S32 nRet = HI_SUCCESS;
    DispCheckNullPointer(pfOpt);

    //printk("DispAddIntf ***0**add (%d)----%d (%d)(%d)(%d)\n",pstDisp->enDisp,pstIntf->eID,pstIntf->u8VDAC_Y_G,pstIntf->u8VDAC_Pb_B,pstIntf->u8VDAC_Pr_R);
    /* if intf exist, release firstly */
    if (pstIt->bOpen)
    {
        bBkFlag = HI_TRUE;

        stBackup = pstIt->stIf;

        // s1 release vdac
        DispCheckNullPointer(pfOpt->PF_ReleaseIntf2);
        pfOpt->PF_ReleaseIntf2(pstDisp->enDisp, pstIt);
    }

    /* clean */
    DispCleanIntf(pstIt);

    DISP_PRINT("DispAddIntf  pstIntf->u8VDAC_Y_G = %d\n", pstIntf->u8VDAC_Y_G);

    pstIt->stIf = *pstIntf;
        DispCheckNullPointer(pfOpt->PF_AcquireIntf2);
    nRet = pfOpt->PF_AcquireIntf2(pstDisp->enDisp, pstIt);

    if (nRet)
    {
        DISP_ERROR("DISP %d acquire  (%d) failed\n", pstDisp->enDisp, pstIt->stIf.eID);
        goto __SET_BACKUP__;
    }
#ifdef __DISP_PLATFORM_BOOT__	
#else
    if ((pstIt->stIf.eID >= HI_DRV_DISP_INTF_HDMI0 ) && (pstIt->stIf.eID <= HI_DRV_DISP_INTF_HDMI2 ) && (!DispGetHdmiFunction()))
    {
        if (s_pstHDMIFunc->pfnHdmiPreFormat && s_pstHDMIFunc->pfnHdmiSetFormat)
        {
            s_pstHDMIFunc->pfnHdmiSetFormat(HI_UNF_HDMI_ID_0, pstDisp->stSetting.enFormat, pstDisp->stSetting.eDispMode);
        }
    }
#endif
    pstIt->bOpen = HI_TRUE;

    return HI_SUCCESS;

__SET_BACKUP__:

    if (bBkFlag == HI_TRUE)
    {
        DispAddIntf(pstDisp, &stBackup);
    }

    return nRet;
}

#define  DISPLAY_DEFAULT_VIRT_SCREEN_WIDTH   1280
#define  DISPLAY_DEFAULT_VIRT_SCREEN_HEIGHT  720

/*==========================================================================
    video encoding
*/
HI_S32 DispProduceDisplayInfo(DISP_S* pstDisp, HI_DISP_DISPLAY_INFO_S* pstInfo)
{
    DISP_HAL_ENCFMT_PARAM_S stFmt;
    HI_DRV_DISP_FMT_E eFmt; 
    HI_S32 nRet;

    if (!pstDisp)
    {
        DISP_ERROR("Found null pointer in %s\n", __FUNCTION__);
        return HI_FAILURE;
    }
    
    eFmt = pstDisp->stSetting.enFormat;
    if (eFmt < HI_DRV_DISP_FMT_CUSTOM)
    {
        nRet = DISP_HAL_GetEncFmtPara(eFmt, &stFmt);

        pstInfo->bIsMaster = pstDisp->bIsMaster;
        pstInfo->bIsSlave  = pstDisp->bIsSlave;
        //printk("id=%d, bm=%d, bs=%d\n", pstDisp->enDisp, pstInfo->bIsMaster,  pstInfo->bIsSlave);
        pstInfo->enAttachedDisp = pstDisp->enAttachedDisp;


        pstInfo->eDispMode = pstDisp->stSetting.eDispMode;
        pstInfo->bRightEyeFirst = pstDisp->stSetting.bRightEyeFirst;
        pstInfo->bInterlace = stFmt.bInterlace;

        pstInfo->stVirtaulScreen = pstDisp->stSetting.stVirtaulScreen;


        if ((pstInfo->stVirtaulScreen.s32Width == 0)
            || (pstInfo->stVirtaulScreen.s32Height == 0))
        {
            pstInfo->stVirtaulScreen.s32Width = DISPLAY_DEFAULT_VIRT_SCREEN_WIDTH;
            pstInfo->stVirtaulScreen.s32Height = DISPLAY_DEFAULT_VIRT_SCREEN_HEIGHT;
        }

        pstInfo->stOffsetInfo    = pstDisp->stSetting.stOffsetInfo;
        pstInfo->stFmtResolution = stFmt.stRefRect;
        pstInfo->stPixelFmtResolution = stFmt.stOrgRect;


        pstDisp->stSetting.stColor.enInCS = stFmt.enColorSpace;
        pstInfo->eColorSpace = stFmt.enColorSpace;
        //printk(">>>>>>>>>>>>>>>> 002 cs =%d\n",  pstInfo->eColorSpace);

        if (!pstDisp->stSetting.bCustomRatio)
        {
            pstInfo->stAR = stFmt.stAR;
        }
        else
        {
            pstInfo->stAR.u8ARh = pstDisp->stSetting.u32CustomRatioHeight;
            pstInfo->stAR.u8ARw = pstDisp->stSetting.u32CustomRatioWidth;
        }

        pstInfo->u32RefreshRate = stFmt.u32RefreshRate;

        pstInfo->u32Bright  = pstDisp->stSetting.stColor.u32Bright;
        pstInfo->u32Contrst = pstDisp->stSetting.stColor.u32Contrst;
        pstInfo->u32Hue     = pstDisp->stSetting.stColor.u32Hue;
        pstInfo->u32Satur   = pstDisp->stSetting.stColor.u32Satur;
        pstInfo->u32Kr      = pstDisp->stSetting.stColor.u32Kr;
        pstInfo->u32Kg      = pstDisp->stSetting.stColor.u32Kg;
        pstInfo->u32Kb      = pstDisp->stSetting.stColor.u32Kb;
    }
    else if ((eFmt == HI_DRV_DISP_FMT_CUSTOM) && (pstDisp->stSetting.bCustomTimingIsSet))
    {
        pstInfo->bIsMaster = pstDisp->bIsMaster;
        pstInfo->bIsSlave  = pstDisp->bIsSlave;
        pstInfo->enAttachedDisp = pstDisp->enAttachedDisp;


        pstInfo->eDispMode = pstDisp->stSetting.eDispMode;
        pstInfo->bRightEyeFirst = pstDisp->stSetting.bRightEyeFirst;

        pstInfo->bInterlace = pstDisp->stSetting.stCustomTimg.bInterlace;


        pstInfo->stVirtaulScreen = pstDisp->stSetting.stVirtaulScreen;

#if 1

        if ((pstInfo->stVirtaulScreen.s32Width == 0)
            || (pstInfo->stVirtaulScreen.s32Height == 0))
        {
            pstInfo->stVirtaulScreen.s32Width = DISPLAY_DEFAULT_VIRT_SCREEN_WIDTH;
            pstInfo->stVirtaulScreen.s32Height = DISPLAY_DEFAULT_VIRT_SCREEN_HEIGHT;
        }

        pstInfo->stOffsetInfo    = pstDisp->stSetting.stOffsetInfo;
#if 0
        pstInfo->stFmtResolution = stFmt.stRefRect;
        pstInfo->stPixelFmtResolution = stFmt.stOrgRect;
#endif
        pstInfo->stPixelFmtResolution.s32X = 0;
        pstInfo->stPixelFmtResolution.s32Y = 0;
        pstInfo->stPixelFmtResolution.s32Width = pstDisp->stSetting.stCustomTimg.u32HACT;
        pstInfo->stPixelFmtResolution.s32Height = pstDisp->stSetting.stCustomTimg.u32VACT;

        pstInfo->stFmtResolution.s32X = 0;
        pstInfo->stFmtResolution.s32Y = 0;

        pstInfo->stFmtResolution.s32Width = pstDisp->stSetting.stCustomTimg.u32HACT;
        pstInfo->stFmtResolution.s32Height = pstDisp->stSetting.stCustomTimg.u32VACT;
#else
        pstInfo->bUseAdjRect = pstDisp->stSetting.bAdjRect;
        pstInfo->stAdjRect   = pstDisp->stSetting.stUsingAdjRect;

        pstInfo->stOrgRect.s32X = 0;
        pstInfo->stOrgRect.s32Y = 0;
        pstInfo->stOrgRect.s32Width = pstDisp->stSetting.stCustomTimg.u32HACT;
        pstInfo->stOrgRect.s32Height = pstDisp->stSetting.stCustomTimg.u32VACT;

        pstInfo->stRefRect.s32X = 0;
        pstInfo->stRefRect.s32Y = 0;

        pstInfo->stRefRect.s32Width = pstDisp->stSetting.stCustomTimg.u32HACT;
        pstInfo->stRefRect.s32Height = pstDisp->stSetting.stCustomTimg.u32VACT;
#endif
        /**/
        /*set  DHDx  in Color Space*/
        pstDisp->stSetting.stColor.enInCS = HI_DRV_CS_BT709_RGB_FULL;
        pstInfo->eColorSpace = HI_DRV_CS_BT709_RGB_FULL;

        pstInfo->stAR.u8ARh = pstDisp->stSetting.stCustomTimg.u32AspectRatioH;
        pstInfo->stAR.u8ARw = pstDisp->stSetting.stCustomTimg.u32AspectRatioW;

        /*set  Rate*/
        pstInfo->u32RefreshRate = pstDisp->stSetting.stCustomTimg.u32VertFreq;

        pstInfo->u32Bright  = pstDisp->stSetting.stColor.u32Bright;
        pstInfo->u32Contrst = pstDisp->stSetting.stColor.u32Contrst;
        pstInfo->u32Hue     = pstDisp->stSetting.stColor.u32Hue;
        pstInfo->u32Satur   = pstDisp->stSetting.stColor.u32Satur;
        pstInfo->u32Kr      = pstDisp->stSetting.stColor.u32Kr;
        pstInfo->u32Kg      = pstDisp->stSetting.stColor.u32Kg;
        pstInfo->u32Kb      = pstDisp->stSetting.stColor.u32Kb;
    }
    else
    {
        DISP_WARN("Invalid display encoding format now\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_VOID DispInitCSC(HI_DRV_DISPLAY_E enDisp, HI_DRV_DISP_COLOR_SETTING_S* pstColor)
{
    // s1 CSC

    DISP_MEMSET(pstColor, 0, sizeof(HI_DRV_DISP_COLOR_SETTING_S));

    pstColor->enInCS  = HI_DRV_CS_DEFAULT;
    pstColor->enOutCS = HI_DRV_CS_DEFAULT;

    pstColor->u32Bright  = DISP_DEFAULT_BRIGHT;
    pstColor->u32Contrst = DISP_DEFAULT_CONTRAST;
    pstColor->u32Satur   = DISP_DEFAULT_SATURATION;
    pstColor->u32Hue     = DISP_DEFAULT_HUE;

    pstColor->u32Kr      = DISP_DEFAULT_KR;
    pstColor->u32Kg      = DISP_DEFAULT_KG;
    pstColor->u32Kb      = DISP_DEFAULT_KB;

    // s2 Gamma
    pstColor->bGammaEnable       = HI_FALSE;
    pstColor->bUseCustGammaTable = HI_FALSE;

    pstColor->pReserve = HI_NULL;
    pstColor->u32Reserve = 0;
    return;
}


HI_VOID DispGetTestInitParam(HI_DRV_DISPLAY_E enDisp, HI_DRV_DISP_SETTING_S* pstSetting)
{
    if (enDisp == HI_DRV_DISPLAY_0)
    {
        pstSetting->bIsMaster = HI_TRUE;
        pstSetting->enAttachedDisp = HI_DRV_DISPLAY_1;

        pstSetting->enFormat = HI_DRV_DISP_FMT_1080i_50;
        pstSetting->stIntf[HI_DRV_DISP_INTF_YPBPR0].eID = HI_DRV_DISP_INTF_YPBPR0;
        pstSetting->stIntf[HI_DRV_DISP_INTF_YPBPR0].u8VDAC_Y_G  = 1;
        pstSetting->stIntf[HI_DRV_DISP_INTF_YPBPR0].u8VDAC_Pb_B = 2;
        pstSetting->stIntf[HI_DRV_DISP_INTF_YPBPR0].u8VDAC_Pr_R = 0;
    }

    if (enDisp == HI_DRV_DISPLAY_1)
    {
        pstSetting->bIsSlave = HI_TRUE;
        pstSetting->enAttachedDisp = HI_DRV_DISPLAY_0;

        pstSetting->enFormat = HI_DRV_DISP_FMT_PAL;
        pstSetting->stIntf[HI_DRV_DISP_INTF_CVBS0].eID = HI_DRV_DISP_INTF_CVBS0;
        pstSetting->stIntf[HI_DRV_DISP_INTF_CVBS0].u8VDAC_Y_G  = 3;
        pstSetting->stIntf[HI_DRV_DISP_INTF_CVBS0].u8VDAC_Pb_B = HI_DISP_VDAC_INVALID_ID;
        pstSetting->stIntf[HI_DRV_DISP_INTF_CVBS0].u8VDAC_Pr_R = HI_DISP_VDAC_INVALID_ID;
    }

    return;
}

extern HI_S32 DispGetInitParam(HI_DRV_DISPLAY_E enDisp, HI_DRV_DISP_INIT_PARAM_S* pstSetting);

HI_S32 DispGetInitParamPriv(HI_DRV_DISPLAY_E enDisp, HI_DRV_DISP_SETTING_S* pstSetting)
{
    HI_DRV_DISP_INTF_ID_E enIf;
    HI_DRV_DISP_INIT_PARAM_S stInitParam;

    DISP_MEMSET(pstSetting, 0, sizeof(HI_DRV_DISP_SETTING_S));

    if (enDisp > HI_DRV_DISPLAY_1)
    {
        return HI_FAILURE;
    }


    // s1 get init param form PDM

    // s2 check para, if(ok){return;} else {get default param;}
    pstSetting->u32BootVersion = 0xfffffffful;

	
    pstSetting->bGetPDMParam = HI_FALSE;

    pstSetting->bIsMaster = HI_FALSE;
    pstSetting->bIsSlave  = HI_FALSE;
    pstSetting->enAttachedDisp = HI_DRV_DISPLAY_BUTT;

    /* output format */
    pstSetting->eDispMode = DISP_STEREO_NONE;

    pstSetting->enFormat  = HI_DRV_DISP_FMT_BUTT;
    pstSetting->bIsMaster = HI_FALSE;
    pstSetting->bIsSlave  = HI_FALSE;
    pstSetting->enAttachedDisp = HI_DRV_DISPLAY_BUTT;


    /* about color */
    DispInitCSC(enDisp, &pstSetting->stColor);

    /* background color */
    pstSetting->stBgColor.u8Red   = DISP_DEFAULT_COLOR_RED;
    pstSetting->stBgColor.u8Green = DISP_DEFAULT_COLOR_GREEN;
    pstSetting->stBgColor.u8Blue  = DISP_DEFAULT_COLOR_BLUE;

    /*zorder */
    pstSetting->enLayer[0] = HI_DRV_DISP_LAYER_GFX;
    pstSetting->enLayer[1] = HI_DRV_DISP_LAYER_VIDEO;

    /* interface setting */
    for (enIf = HI_DRV_DISP_INTF_YPBPR0; enIf < HI_DRV_DISP_INTF_ID_MAX; enIf++)
    {
        pstSetting->stIntf[enIf].eID = HI_DRV_DISP_INTF_ID_MAX;
        pstSetting->stIntf[enIf].u8VDAC_Y_G  = HI_DISP_VDAC_INVALID_ID;
        pstSetting->stIntf[enIf].u8VDAC_Pb_B = HI_DISP_VDAC_INVALID_ID;
        pstSetting->stIntf[enIf].u8VDAC_Pr_R = HI_DISP_VDAC_INVALID_ID;
    }

    pstSetting->u32LayerNumber = 0;
    //HI_DRV_DISP_LAYER_E enLayer[HI_DRV_DISP_LAYER_BUTT]; /* Z-order is from bottom to top */


    pstSetting->bCustomRatio = HI_FALSE;
    pstSetting->u32CustomRatioWidth  = 0;
    pstSetting->u32CustomRatioHeight = 0;

    pstSetting->stVirtaulScreen.s32X      = 0;
    pstSetting->stVirtaulScreen.s32Y      = 0;
    pstSetting->stVirtaulScreen.s32Width  = 1280;
    pstSetting->stVirtaulScreen.s32Height = 720;

    pstSetting->stOffsetInfo.u32Left     = 0;
    pstSetting->stOffsetInfo.u32Right    = 0;
    pstSetting->stOffsetInfo.u32Top      = 0;
    pstSetting->stOffsetInfo.u32Bottom   = 0;

    //pstSetting->u32Reseve;
    //pstSetting->pRevData;
    //DispGetTestInitParam(enDisp, pstSetting);
    //DispGetInitParam(enDisp, pstSetting);
    if ( DispGetInitParam(enDisp, &stInitParam) == HI_SUCCESS)
    {    
        pstSetting->bGetPDMParam = HI_TRUE;
        pstSetting->u32BootVersion = stInitParam.u32Version;


        pstSetting->bIsMaster = stInitParam.bIsMaster;
        pstSetting->bIsSlave  = stInitParam.bIsSlave;
        pstSetting->enAttachedDisp = stInitParam.enAttachedDisp;
		

        pstSetting->enFormat = DispTransferFormat(enDisp, stInitParam.enFormat);		
        pstSetting->stColor.u32Bright = stInitParam.u32Brightness;
        pstSetting->stColor.u32Contrst = stInitParam.u32Contrast;
        pstSetting->stColor.u32Satur = stInitParam.u32Saturation;
        pstSetting->stColor.u32Hue = stInitParam.u32HuePlus;
        pstSetting->stColor.bGammaEnable = stInitParam.bGammaEnable;
        pstSetting->stBgColor = stInitParam.stBgColor;

        pstSetting->bCustomRatio = stInitParam.bCustomRatio;
        pstSetting->u32CustomRatioWidth  = stInitParam.u32CustomRatioWidth;
        pstSetting->u32CustomRatioHeight = stInitParam.u32CustomRatioHeight;

        pstSetting->stVirtaulScreen.s32X      = 0;
        pstSetting->stVirtaulScreen.s32Y      = 0;
        pstSetting->stVirtaulScreen.s32Width  = stInitParam.u32VirtScreenWidth;
        pstSetting->stVirtaulScreen.s32Height = stInitParam.u32VirtScreenHeight;

        pstSetting->stOffsetInfo              = stInitParam.stOffsetInfo;

        for (enIf = HI_DRV_DISP_INTF_YPBPR0; enIf < HI_DRV_DISP_INTF_ID_MAX; enIf++)
        {
            if (stInitParam.stIntf[enIf].eID != HI_DRV_DISP_INTF_ID_MAX)
            {
                pstSetting->stIntf[enIf] = stInitParam.stIntf[enIf];
                DISP_PRINT(">>>>>>>>>> intf %d id=%d\n", enIf,
                           stInitParam.stIntf[enIf].eID);
            }
        }

        //stInitParam.stDispTiming;
    }

    return HI_SUCCESS;
}


HI_VOID DispParserInitParam(DISP_S* pstDisp, HI_DRV_DISP_SETTING_S* pstSetting)
{
    DISP_INTF_OPERATION_S* pstIntfOpt;
    DISP_SETTING_S* pstS = &pstDisp->stSetting;
    HI_S32 t = 0;

    pstIntfOpt = DISP_HAL_GetOperationPtr();
    //DISP_ASSERT(pstIntfOpt);
    pstS->u32Version = DISP_DRVIER_VERSION;
    pstS->u32BootVersion = pstSetting->u32BootVersion;
    //pstS->bSelfStart = pstSetting->bSelfStart;
    pstS->bGetPDMParam = pstSetting->bGetPDMParam;

    pstS->eDispMode = pstSetting->eDispMode;
    pstS->bRightEyeFirst = HI_FALSE;
    pstS->enFormat  = pstSetting->enFormat;
    //pstS->bFmtChanged = HI_FALSE;  // TODO

    pstS->stCustomTimg = pstSetting->stCustomTimg;

    if (pstS->enFormat == HI_DRV_DISP_FMT_CUSTOM)
    {
        pstS->bCustomTimingIsSet = HI_TRUE;
    }
    else
    {
        pstS->bCustomTimingIsSet = HI_FALSE;
    }

    pstS->bCustomTimingChange = HI_FALSE;

    /* about color */
    pstS->stColor = pstSetting->stColor;

    /* background color */
    pstS->stBgColor = pstSetting->stBgColor;

    /* interface setting */
    DispCleanAllIntf(pstDisp);

    pstS->u32LayerNumber = 0;
    //HI_DRV_DISP_LAYER_E enLayer[HI_DRV_DISP_LAYER_BUTT]; /* Z-order is from bottom to top */

    pstS->bCustomRatio = pstSetting->bCustomRatio;

    if (pstS->bCustomRatio)
    {
        pstS->u32CustomRatioWidth = pstSetting->u32CustomRatioWidth;
        pstS->u32CustomRatioHeight = pstSetting->u32CustomRatioHeight;
    }

    pstS->u32Reseve = 0;
    pstS->pRevData  = HI_NULL;
    pstS->stVirtaulScreen = pstSetting->stVirtaulScreen;
    pstS->stOffsetInfo    = pstSetting->stOffsetInfo;

    for (t = 0; t < HI_DRV_DISP_INTF_ID_MAX; t++)
    {
        if (pstSetting->stIntf[t].eID < HI_DRV_DISP_INTF_ID_MAX)
        {
            //todo
            DispAddIntf(pstDisp, &pstSetting->stIntf[t]);
        }
    }

    /* for attach display */
    pstDisp->bIsMaster = pstSetting->bIsMaster;
    pstDisp->bIsSlave  = pstSetting->bIsSlave;
    pstDisp->enAttachedDisp = pstSetting->enAttachedDisp;

#if 0
    DISP_PRINT("FOLLOW INFO: DISP %d, M=%d,S=%d,ATT=%d......\n",
               pstDisp->enDisp,
               pstDisp->bIsMaster,
               pstDisp->bIsSlave,
               pstDisp->enAttachedDisp);
#endif

#ifdef HI_DISP_BUILD_FULL

    if (pstS->bGetPDMParam)
    {
        HI_BOOL bOutput;

        // todo
        if (( !pstIntfOpt) || (!pstIntfOpt->PF_GetChnEnable) )
        {
            DISP_ERROR(" %s has null ptr!\n", __FUNCTION__);
            return ;
        }
        pstIntfOpt->PF_GetChnEnable(pstDisp->enDisp, &bOutput);

        if (bOutput)
        {
            pstDisp->bEnable = HI_TRUE;
            DISP_PRINT("DISP %d is working......\n", pstDisp->enDisp);

            // VDP is working, not to reset.
            DispSetHardwareState();
        }
    }

#endif

    return;
}

HI_VOID DispInitDisplay(HI_DRV_DISPLAY_E enDisp)
{
    DISP_S* pstDisp;
    HI_DRV_DISP_SETTING_S stDefSetting;

    DispGetPointerByIDNoReturn(enDisp, pstDisp);

    DISP_MEMSET(pstDisp, 0, sizeof(DISP_S));

    // s1 set id
    pstDisp->enDisp = enDisp;

    // s2 get base parameters
    if (DispGetInitParamPriv(enDisp, &stDefSetting))
    {
        pstDisp->bSupport = HI_FALSE;
        DISP_PRINT("DispGetInitParam  failed\n");
        return;
    }

    pstDisp->bSupport = HI_TRUE;

    //component operation
    pstDisp->pstIntfOpt = &s_stDisplayDevice.stIntfOpt;

    DispParserInitParam(pstDisp, &stDefSetting);

    pstDisp->eState  = DISP_PRIV_STATE_DISABLE;
    pstDisp->bOpen = HI_FALSE;;

    DispProduceDisplayInfo(pstDisp, &pstDisp->stDispInfo);
    pstDisp->hCast = HI_NULL;
    
    return;
}


HI_VOID DispDeInitDisplay(HI_DRV_DISPLAY_E enDisp)
{
    DISP_S* pstDisp;
    HI_S32 t;

    DispGetPointerByIDNoReturn(enDisp, pstDisp);

#ifdef HI_DISP_BUILD_FULL

    if (pstDisp->hCast)
    {
        DISP_CastSetEnable(pstDisp->hCast, HI_FALSE);

        DISP_CastDestroy(pstDisp->hCast);
    }

#endif

    for (t = 0; t < HI_DRV_DISP_INTF_ID_MAX; t++)
    {
        if (pstDisp->stSetting.stIntf[t].bOpen)
        {
            DISP_INTF_OPERATION_S* pfOpt = DISP_HAL_GetOperationPtr();
            if (pfOpt)
            {
				if ( pfOpt->PF_ReleaseIntf2) 
            		pfOpt->PF_ReleaseIntf2(pstDisp->enDisp, &pstDisp->stSetting.stIntf[t]);
            }
            // s3 set intf
            DispCleanIntf(&pstDisp->stSetting.stIntf[t]);
        }
    }

    return;
}

HI_S32 DispSetFormat(HI_DRV_DISPLAY_E eDisp, HI_DRV_DISP_FMT_E eFmt,HI_DRV_DISP_STEREO_E enStereo)
{
    DISP_S* pstDisp;
    DISP_INTF_OPERATION_S* pfOpt = DISP_HAL_GetOperationPtr();
    HI_U32 u;
    HI_S32 nRet;

    DispGetPointerByID(eDisp, pstDisp);
    DispCheckNullPointer(pfOpt);
    DispCheckNullPointer(pfOpt->PF_ResetIntfFmt2);

    if (HI_DRV_DISP_FMT_CUSTOM != eFmt)
    {
        // s1 set channel
        nRet = pstDisp->pstIntfOpt->PF_SetChnFmt(pstDisp->enDisp, eFmt, enStereo);

        // s2 set interface  if necessarily
        for (u = 0; u < HI_DRV_DISP_INTF_ID_MAX; u++)
        {
            if (pstDisp->stSetting.stIntf[u].bOpen)
            {
                //DispSetIntfFmt(pstDisp, u, eFmt);
                //pstDisp->pstIntfOpt->PF_ResetIntfFmt(enIntfId, enEncFmt);
                pfOpt->PF_ResetIntfFmt2(pstDisp->enDisp, &pstDisp->stSetting.stIntf[u], eFmt);
            }
        }

        // s3 produce
        /*set corlor space */
        //pstDisp->stSetting.stColor.enInCS = HI_DRV_CS_BT709_YUV_LIMITED;
        //pstDisp->stSetting.stColor.enOutCS = HI_DRV_CS_BT709_RGB_LIMITED;


        DispProduceDisplayInfo(pstDisp, &pstDisp->stDispInfo);
        //printk("xxxxxxxxxxxxx eFmt = %d\n", eFmt);
    }
    else
    {
        if (!pstDisp->stSetting.bCustomTimingIsSet)
        {
            DISP_ERROR("User customer format without setting timming\n");
            return HI_ERR_DISP_INVALID_PARA;
        }
        else
        {
            // s1 set channel
            nRet = pstDisp->pstIntfOpt->PF_SetChnTiming(pstDisp->enDisp, &pstDisp->stSetting.stCustomTimg);
        }

        for (u = 0; u < HI_DRV_DISP_INTF_ID_MAX; u++)
        {
            if (pstDisp->stSetting.stIntf[u].bOpen)
            {
                pfOpt->PF_ResetIntfFmt2(pstDisp->enDisp, &pstDisp->stSetting.stIntf[u], eFmt);
                //DISP_AddIntf(eDisp,&pstDisp->stSetting.stIntf[u]);
            }
        }

        /*set corlor space */

        DispProduceDisplayInfo(pstDisp, &pstDisp->stDispInfo);
    }

    return HI_SUCCESS;
}


HI_S32 DispSetColor(DISP_S* pstDisp)
{
#if 1
    HI_S32 nRet;
    DISP_HAL_COLOR_S stColor;
    HI_DRV_DISP_COLOR_SETTING_S* pstC = &pstDisp->stSetting.stColor;


    stColor.enInputCS  = pstC->enInCS;
    stColor.enOutputCS = HI_DRV_CS_BT709_RGB_FULL;
    //printk("in(%d),out ***(%d)\n",stColor.enInputCS,stColor.enOutputCS );
    stColor.u32Bright  = 50;
    stColor.u32Contrst = 50;
    stColor.u32Hue     = 50;
    stColor.u32Satur   = 50;
    stColor.u32Kr      = 50;
    stColor.u32Kg      = 50;
    stColor.u32Kb      = 50;
    stColor.bGammaEnable = pstC->bGammaEnable;


    // s1 set channel
    nRet = pstDisp->pstIntfOpt->PF_SetChnColor(pstDisp->enDisp, &stColor);

    // s2 set interface if necessarily
#if 0

    for (u = 0; u < HI_DRV_DISP_INTF_ID_MAX; u++)
    {
        if (pstDisp->stSetting.stIntf[u].bOpen)
        {
            nRet = pstDisp->pstIntfOpt->PF_SetIntfColor(pstDisp->stIntf[u].stIf.eID, pstColor);
        }
    }

#endif
#endif

    // s3 set bgc
    /*
        DISP_PRINT("====DispSetColor R=%d, G=%d, B=%d\n", pstDisp->stSetting.stBgColor.u8Red,
                                                          pstDisp->stSetting.stBgColor.u8Green,
                                                          pstDisp->stSetting.stBgColor.u8Blue);
    */
    pstDisp->pstIntfOpt->PF_SetChnBgColor(pstDisp->enDisp,
                                          pstDisp->stSetting.stColor.enOutCS,
                                          &pstDisp->stSetting.stBgColor);


    return HI_SUCCESS;
}

HI_S32 DispSetIntfEnable(DISP_S* pstDisp, HI_BOOL bEnable)
{
    DISP_INTF_OPERATION_S* pfOpt = DISP_HAL_GetOperationPtr();
    HI_U32 u;

    DispCheckNullPointer(pfOpt);
    DispCheckNullPointer(pfOpt->PF_SetIntfEnable2);
    // s1 set interface if necessarily
    for (u = 0; u < HI_DRV_DISP_INTF_ID_MAX; u++)
    {
        if (pstDisp->stSetting.stIntf[u].bOpen)
        {
            //DispSetIntfLink(pstDisp, u);
            pfOpt->PF_SetIntfEnable2(pstDisp->enDisp, &pstDisp->stSetting.stIntf[u], bEnable);
        }
    }

    return HI_SUCCESS;
}

HI_S32 DispSetEnable(DISP_S* pstDisp, HI_BOOL bEnable)
{
    DISP_INTF_OPERATION_S* pfOpt = DISP_HAL_GetOperationPtr();
    HI_U32 u;
    HI_S32 nRet;
    DispCheckNullPointer(pfOpt);
    DispCheckNullPointer(pfOpt->PF_SetIntfEnable2);
    DispCheckNullPointer(pfOpt->PF_SetChnEnable);

    // s1 set interface if necessarily
    for (u = 0; u < HI_DRV_DISP_INTF_ID_MAX; u++)
    {
        if (pstDisp->stSetting.stIntf[u].bOpen)
        {
            //DispSetIntfLink(pstDisp, u);
            pfOpt->PF_SetIntfEnable2(pstDisp->enDisp, &pstDisp->stSetting.stIntf[u], bEnable);
        }
    }

    // s2 set channel
    nRet = pfOpt->PF_SetChnEnable(pstDisp->enDisp, bEnable);

    return HI_SUCCESS;
}

/******************************************************************************
    display function
*****************************************************************************/
#define DEF_DRV_DISP_API_FUNCTION_START_HERE

HI_S32 DISP_GetInitFlag(HI_BOOL* pbInited)
{
    DispCheckNullPointer(pbInited);

    *pbInited = (s_s32DisplayGlobalFlag == DISP_DEVICE_STATE_OPEN) ? HI_TRUE : HI_FALSE;

    return HI_SUCCESS;
}

HI_S32 DISP_GetVersion(HI_DRV_DISP_VERSION_S* pstVersion)
{
    DispCheckNullPointer(pstVersion);

    // check whether display is inited.
    DispCheckDeviceState();

    // return version
    DISP_HAL_GetVersion(pstVersion);

    return HI_SUCCESS;
}

HI_BOOL DISP_IsOpened(HI_DRV_DISPLAY_E enDisp)
{
    // s1 check input parameters
    DispCheckDeviceState();

    DispCheckID(enDisp);

    // s2 return display OPEN state
    return s_stDisplayDevice.stDisp[enDisp - HI_DRV_DISPLAY_0].bOpen;
}

HI_BOOL DISP_IsFollowed(HI_DRV_DISPLAY_E enDisp)
{
    // s1 check input parameters
    DispCheckDeviceState();

    DispCheckID(enDisp);

    // s2 return display OPEN state
    return s_stDisplayDevice.stDisp[enDisp - HI_DRV_DISPLAY_0].bIsMaster;
}

HI_S32 DISP_Init(HI_VOID)
{
    HI_DRV_DISPLAY_E eDisp;
    HI_DRV_DISP_VERSION_S stDispVersion;
    HI_S32 nRet;
    
    if (s_s32DisplayGlobalFlag != DISP_DEVICE_STATE_CLOSE)
    {
        DISP_INFO("DISPLAY has been inited");
        return HI_SUCCESS;
    }



    DISP_MEMSET(&s_stDisplayDevice, 0, sizeof(DISP_DEV_S));

    // s1 get interface operation
    nRet = DISP_HAL_Init(DISP_BASE_ADDRESS);
    if (nRet)
    {
        DISP_ERROR("DISP_HAL_Init failed!");
        goto __ERR_EXIT__;
    }

    nRet = DISP_HAL_GetOperation(&s_stDisplayDevice.stIntfOpt);
    nRet = DISP_HAL_GetVersion(&stDispVersion);

    // s1.1 init alg
    nRet = DISP_DA_Init(&stDispVersion);
    if (nRet)
    {
        DISP_ERROR("DISP_DA_Init failed!");
        goto __ERR_EXIT__;
    }


#ifdef HI_DISP_BUILD_FULL
    // s1.2 init alg
    nRet = DISP_UA_Init(&stDispVersion);

    if (nRet)
    {
        DISP_ERROR("DISP_UA_Init failed!");
        goto __ERR_EXIT__;
    }

#endif

    // s2 inited display
    for (eDisp = HI_DRV_DISPLAY_0; eDisp < HI_DRV_DISPLAY_2; eDisp++)
    {
        DispInitDisplay(eDisp);
    }

#ifdef HI_DISP_BUILD_FULL

    nRet = DISP_ISR_Init();
    if (request_irq(DISP_INT_NUMBER, (irq_handler_t)DISP_ISR_Main, IRQF_SHARED, "hi_vdp_irq", &g_DispIrqHandle) != 0)
    {
        DISP_ERROR("DISP registe IRQ failed!\n");
        nRet = HI_FAILURE;
        goto __ERR_EXIT__;
    }

#endif

    DispResetHardware();

    s_s32DisplayGlobalFlag = DISP_DEVICE_STATE_OPEN;

    for (eDisp = HI_DRV_DISPLAY_0; eDisp < HI_DRV_DISPLAY_2; eDisp++)
    {
        DISP_S* pstDisp = HI_NULL;
        DispGetPointerByID(eDisp, pstDisp);
        
#ifdef __DISP_PLATFORM_BOOT__
        DISP_Open(eDisp);

        if (pstDisp->stSetting.stIntf[HI_DRV_DISP_INTF_HDMI0].bOpen)
        {
            HI_DRV_DISP_FMT_E enEncFmt = HI_DRV_DISP_FMT_BUTT;
            HI_DRV_DISP_STEREO_MODE_E enStereo;
            DISP_GetFormat(eDisp, &enStereo, &enEncFmt);

            HI_DRV_HDMI_Init();

            if (HI_SUCCESS != HI_DRV_HDMI_Open(enEncFmt))
            {
                DISP_PRINT("HI_UNF_HDMI_Open Err \n");
            }

            HI_DRV_HDMI_Start();
        }

#else

        if ( pstDisp->bEnable )
        {
            DISP_Open(eDisp);
            
#ifndef HI_MCE_SUPPORT
            if ((pstDisp->stSetting.stIntf[HI_DRV_DISP_INTF_HDMI0].bOpen) && (!DispGetHdmiFunction()))
            {                
                    if (s_pstHDMIFunc->pfnHdmiInit && s_pstHDMIFunc->pfnHdmiOpen)
                    {
                        DISP_PRINT(">>>>>>>>>>>>>>>>>>>>>>>>>>>> disp init hdmi\n");                
                        s_pstHDMIFunc->pfnHdmiInit();
                        s_pstHDMIFunc->pfnHdmiOpen(HI_UNF_HDMI_ID_0);
                    }                
            }
#endif
        }

#endif
    }

    //DISP_PRINT("DISP_Init 004\n");

    return HI_SUCCESS;

__ERR_EXIT__:
#ifdef HI_DISP_BUILD_FULL
    DISP_UA_DeInit();
#endif

    DISP_HAL_DeInit();

    return nRet;
}

HI_S32 DISP_DeInit(HI_VOID)
{
    HI_DRV_DISPLAY_E eDisp;

    DISP_PRINT("DISP_DeInit 001\n");

    if (DISP_DEVICE_STATE_CLOSE == s_s32DisplayGlobalFlag)
    {
        DISP_INFO("DISPLAY has NOT inited");
        return HI_SUCCESS;
    }

    DISP_PRINT("DISP_DeInit 002\n");

    // s2 inited display
    for (eDisp = HI_DRV_DISPLAY_0; eDisp < HI_DRV_DISPLAY_2; eDisp++)
    {
        DISP_Close(eDisp);
        DispDeInitDisplay(eDisp);
    }

    DISP_PRINT("DISP_DeInit 003\n");
    //DISP_MSLEEP(40);
    //s_stDisplayDevice.stIntfOpt.PF_ResetHardware();

#ifdef HI_DISP_BUILD_FULL
    free_irq(DISP_INT_NUMBER, &g_DispIrqHandle);

    DISP_ISR_DeInit();

    DISP_PRINT("DISP_DeInit 004\n");


    DISP_UA_DeInit();
#endif

    DISP_DA_DeInit();


    DISP_HAL_DeInit();

    DISP_MEMSET(&s_stDisplayDevice, 0, sizeof(DISP_DEV_S));

    DISP_PRINT("DISP_DeInit 004\n");


    s_s32DisplayGlobalFlag = DISP_DEVICE_STATE_CLOSE;

    return HI_SUCCESS;
}


HI_S32 DISP_Suspend(HI_VOID)
{
#ifndef __DISP_PLATFORM_BOOT__
    DISP_S* pstDisp;
    HI_DRV_DISPLAY_E enD;
    DISP_INTF_OPERATION_S* pfHal = DISP_HAL_GetOperationPtr();
    //DISP_INTF_OPERATION_S *pfOpt = DISP_HAL_GetOperationPtr();

    DispCheckDeviceState();
    DispCheckNullPointer(pfHal);
    DispCheckNullPointer(pfHal->PF_VDP_RegSave);

    if (DISP_DEVICE_STATE_OPEN == s_s32DisplayGlobalFlag)
    {
        for (enD = HI_DRV_DISPLAY_0; enD < HI_DRV_DISPLAY_BUTT; enD++)
        {
            DispGetPointerByIDNoReturn(enD, pstDisp);

            pstDisp->bStateBackup = pstDisp->bEnable;

            if (pstDisp->bEnable == HI_TRUE)
            {
                DISP_SetEnable(enD, HI_FALSE);
            }
        }

        DISP_ISR_Suspend();

        /*save VDP reg*/
        if (HI_SUCCESS != pfHal->PF_VDP_RegSave())
        {
            DISP_ERROR("Display save registers for suspend failed!\n");
             return HI_FAILURE;
        }
        /*dts:DTS2013080709083, we should reset the module and close all the clk.*/
        DispCloseClkResetModule();

        DispClearHardwareState();

        s_s32DisplayGlobalFlag = DISP_DEVICE_STATE_SUSPEND;
    }

#endif
    return HI_SUCCESS;
}

HI_S32 DISP_Resume(HI_VOID)
{
#ifndef __DISP_PLATFORM_BOOT__
    DISP_S* pstDisp;
    HI_DRV_DISPLAY_E enD;
    DISP_INTF_OPERATION_S* pfHal = DISP_HAL_GetOperationPtr();
    DispCheckNullPointer(pfHal);
    DispCheckNullPointer(pfHal->PF_VDP_RegReStore);

    if (DISP_DEVICE_STATE_SUSPEND == s_s32DisplayGlobalFlag)
    {
        //
        DispResetHardware();
        /*restore VDP reg*/
        
        if (HI_SUCCESS != pfHal->PF_VDP_RegReStore())
        {
           DISP_ERROR("Display restore registers for resume failed!\n");
            return HI_FAILURE;
        }

        DISP_ISR_Resume();

        s_s32DisplayGlobalFlag = DISP_DEVICE_STATE_OPEN;

        for (enD = HI_DRV_DISPLAY_0; enD < HI_DRV_DISPLAY_BUTT; enD++)
        {
            DispGetPointerByIDNoReturn(enD, pstDisp);

            if (pstDisp->bStateBackup == HI_TRUE)
            {
                DISP_SetEnable(enD, HI_TRUE);
            }
        }
    }

#endif
    return HI_SUCCESS;
}


HI_S32 DISP_Attach(HI_DRV_DISPLAY_E enMaster, HI_DRV_DISPLAY_E enSlave)
{
    DISP_S* pstM, *pstS;
    DISP_INTF_OPERATION_S* pfOpt = DISP_HAL_GetOperationPtr();

    // s1 check input parameters
    DispCheckDeviceState();

    DispCheckNullPointer(pfOpt);
    DispCheckNullPointer(pfOpt->PF_TestChnAttach);
    if ( !pfOpt->PF_TestChnAttach(enMaster, enSlave) )
    {
        DISP_ERROR("Display NOT support %d attach to %d!\n", enSlave, enMaster);
        return HI_ERR_DISP_INVALID_OPT;
    }

    DispGetPointerByID(enMaster, pstM);
    DispGetPointerByID(enSlave, pstS);

    if (pstM->bIsMaster && (pstM->enAttachedDisp == enSlave))
    {
        DISP_INFO("Display has been ATTACHED!\n");
        return HI_SUCCESS;
    }

    if (pstM->bIsMaster || pstM->bIsSlave || pstS->bIsMaster || pstS->bIsSlave)
    {
        DISP_ERROR("Display has been opened!\n");
        return HI_ERR_DISP_INVALID_OPT;
    }

    pstM->bIsMaster = HI_TRUE;
    pstM->bIsSlave  = HI_FALSE;
    pstM->enAttachedDisp = enSlave;

    pstS->bIsMaster = HI_FALSE;
    pstS->bIsSlave  = HI_TRUE;
    pstS->enAttachedDisp = enMaster;

    return HI_SUCCESS;
}

HI_S32 DISP_Detach(HI_DRV_DISPLAY_E enMaster, HI_DRV_DISPLAY_E enSlave)
{
    DISP_S* pstM, *pstS;

    // s1 check input parameters
    DispCheckDeviceState();

    // s2 set detach
    DispGetPointerByID(enMaster, pstM);
    DispGetPointerByID(enSlave, pstS);

    if (!pstM->bIsMaster || (pstM->enAttachedDisp != enSlave))
    {
        DISP_INFO("Display has NOT been ATTACHED!\n");
        return HI_SUCCESS;
    }

    pstM->bIsMaster = HI_FALSE;
    pstM->enAttachedDisp = HI_DRV_DISPLAY_BUTT;

    pstS->bIsSlave = HI_FALSE;
    pstS->enAttachedDisp = HI_DRV_DISPLAY_BUTT;

    return HI_SUCCESS;
}

HI_VOID debug_DISP_Callback(HI_HANDLE hDst, const HI_DRV_DISP_CALLBACK_INFO_S* pstInfo)
{
    //printk("event = %d, ", pstInfo->eEventType);
    return;
}


#ifdef HI_DISP_BUILD_FULL
#define  DISPLAY0_BUS_UNDERFLOW_INT 0x00000080UL
#define  DISPLAY1_BUS_UNDERFLOW_INT 0x00000008UL
HI_VOID DISP_CB_PreProcess(HI_HANDLE hHandle, const HI_DRV_DISP_CALLBACK_INFO_S* pstInfo)
{
    DISP_S* pstDisp = (DISP_S*)hHandle;
    DISP_INTF_OPERATION_S* pfHal = DISP_HAL_GetOperationPtr();
    HI_U32 uIntState;
    HI_U8 u8DacReg;
    if ((!pfHal) || (!pfHal->PF_GetUnmaskedIntState) || (!pfHal->PF_CleanIntState) )
    {
        DISP_ERROR("has null ptr!\n");
        return ;
    }

    pfHal->PF_GetUnmaskedIntState(&uIntState);

    if ((pstDisp->enDisp == HI_DRV_DISPLAY_0) && (uIntState & DISPLAY0_BUS_UNDERFLOW_INT) )
    {
        pstDisp->u32Underflow++;
        pfHal->PF_CleanIntState(DISPLAY0_BUS_UNDERFLOW_INT);
    }

    if ((pstDisp->enDisp == HI_DRV_DISPLAY_1) && (uIntState & DISPLAY1_BUS_UNDERFLOW_INT) )
    {
        pstDisp->u32Underflow++;
        pfHal->PF_CleanIntState(DISPLAY1_BUS_UNDERFLOW_INT);
    }

   // printk("---State:0x%x\n",uIntState);
    u8DacReg = (uIntState & VDAC_STATE_INT)>>12; 
    pfHal->PF_DACIsr(u8DacReg);// shielding function, subsequent to continue to use this functio
    // display state change
    switch (pstDisp->eState)
    {
        case DISP_PRIV_STATE_DISABLE:
            if (pstDisp->bEnable)
            {
                DISP_ISR_SetEvent(pstDisp->enDisp, HI_DRV_DISP_C_OPEN);
                DISP_ISR_SetDispInfo(pstDisp->enDisp, &pstDisp->stDispInfo);
                pstDisp->eState = DISP_PRIV_STATE_WILL_ENABLE;
                DISP_PRINT("DISP_CB_PreProcess001 id=%d,en=%d\n", pstDisp->enDisp, pstDisp->bEnable);
            }

            break;

        case DISP_PRIV_STATE_WILL_ENABLE:
            DISP_ISR_SetEvent(pstDisp->enDisp, HI_DRV_DISP_C_VT_INT);
            DISP_ISR_SetDispInfo(pstDisp->enDisp, &pstDisp->stDispInfo);
            pstDisp->eState = DISP_PRIV_STATE_ENABLE;

            DISP_PRINT("DISP_CB_PreProcess002 id=%d,en=%d\n", pstDisp->enDisp, pstDisp->bEnable);

            break;

        case DISP_PRIV_STATE_ENABLE:
            if (!pstDisp->bEnable)
            {
                DISP_ISR_SetEvent(pstDisp->enDisp, HI_DRV_DISP_C_PREPARE_CLOSE);
                pstDisp->eState = DISP_PRIV_STATE_WILL_DISABLE;
                DISP_PRINT("DISP_CB_PreProcess003 id=%d,en=%d\n", pstDisp->enDisp, pstDisp->bEnable);
            }
            else
            {
                if (pstDisp->bDispSettingChange)
                {
                    DISP_PRINT("DISP_CB_PreProcess0031 id=%d,en=%d\n", pstDisp->enDisp, pstDisp->bEnable);
                    DispProduceDisplayInfo(pstDisp, &pstDisp->stDispInfo);
                    DISP_ISR_SetDispInfo(pstDisp->enDisp, &pstDisp->stDispInfo);
                    DISP_ISR_SetEvent(pstDisp->enDisp, HI_DRV_DISP_C_DISPLAY_SETTING_CHANGE);
                    pstDisp->bDispSettingChange = HI_FALSE;
                }
                else
                {
                    DISP_ISR_SetEvent(pstDisp->enDisp, HI_DRV_DISP_C_VT_INT);
                    //DISP_ISR_SetDispInfo(pstDisp->enDisp, &pstDisp->stDispInfo);
                }
            }

            break;

        case DISP_PRIV_STATE_WILL_DISABLE:
            DISP_ISR_SetEvent(pstDisp->enDisp, HI_DRV_DISP_C_EVET_NONE);
            pstDisp->eState = DISP_PRIV_STATE_DISABLE;

            DISP_PRINT("DISP_CB_PreProcess004 id=%d, en=%d\n", pstDisp->enDisp,  pstDisp->bEnable);

            break;

        default :
            break;

    }

    //    printk("event = %d, ", pstDisp->eEventType);
    return;
}
#endif

HI_S32 DISP_Open(HI_DRV_DISPLAY_E enDisp)
{
    DISP_S* pstDisp;
    HI_S32 nRet = HI_SUCCESS;
#ifdef HI_DISP_BUILD_FULL
    HI_DRV_DISP_CALLBACK_S stCB;
#endif

    // s1 check input parameters
    DispCheckDeviceState();

    DispCheckID(enDisp);

    // s2 check whether display opened
    if (DISP_IsOpened(enDisp))
    {
        DISP_INFO("Display has been opened!\n");
        return HI_SUCCESS;
    }

    if(HI_SUCCESS != DispCheckReadyForOpen(enDisp))
    {
        DISP_INFO("format has not  been set!\n");
        return HI_FAILURE;
    }
    // s2.0 reset hardware
    //DispResetHardware();

    // s2.1 get display channel
    DispGetPointerByID(enDisp, pstDisp);
    if (( !pstDisp) || (!pstDisp->pstIntfOpt) ||  (!pstDisp->pstIntfOpt->PF_TestChnSupport))
    {
        DISP_ERROR(" %s has null ptr!\n", __FUNCTION__);
        return HI_ERR_DISP_NULL_PTR;
    }

    if (!pstDisp->pstIntfOpt->PF_TestChnSupport(enDisp))
    {
        DISP_ERROR("DISP ERROR! This version does not support display %d\n", (HI_S32)enDisp);
        return HI_ERR_DISP_INVALID_OPT;
    }


    // s3 check whether necessory attributes are configed
#ifdef HI_DISP_BUILD_FULL
    // s3.2 open display channel isr
    DISP_ISR_OpenChn(enDisp);

    // set display info for first time
    DISP_ISR_SetDispInfo(enDisp, &pstDisp->stDispInfo);

    // register display callback
    stCB.hDst = (HI_HANDLE)pstDisp;
    stCB.pfDISP_Callback = DISP_CB_PreProcess;
    nRet = DISP_ISR_RegCallback(enDisp, HI_DRV_DISP_C_INTPOS_0_PERCENT, &stCB);
    DISP_ASSERT(!nRet);
    //DISP_PRINT(" DISP_ISR_OpenChn = 0x%x\n", nRet);
#endif
    // s5 Product ask for that display must be enabled at the same time.
    
    DISP_SetLayerZorder(enDisp, HI_DRV_DISP_LAYER_GFX, HI_DRV_DISP_ZORDER_MOVETOP);

    DISP_SetEnable(enDisp, HI_TRUE);

    // s4 set open state
    pstDisp->bOpen = HI_TRUE;

    return nRet;
}

HI_VOID DispReleaseIntf(HI_DRV_DISPLAY_E enDisp)
{
    DISP_S* pstDisp;
    HI_DRV_DISP_INTF_ID_E i;

    DispGetPointerByIDNoReturn(enDisp, pstDisp);

    for (i = 0; i < (HI_S32)HI_DRV_DISP_INTF_ID_MAX; i++)
    {
        if (pstDisp->stSetting.stIntf[i].bOpen)
        {
            DISP_INTF_OPERATION_S* pfOpt = DISP_HAL_GetOperationPtr();

            // s1 release vdac
            if (pfOpt && pfOpt->PF_ReleaseIntf2)
            pfOpt->PF_ReleaseIntf2(pstDisp->enDisp, &pstDisp->stSetting.stIntf[i]);

            // s3 set intf
            DispCleanIntf(&pstDisp->stSetting.stIntf[i]);
        }
    }

    return;
}

HI_S32 DISP_Close(HI_DRV_DISPLAY_E enDisp)
{
    DISP_S* pstDisp;
  
#ifdef HI_DISP_BUILD_FULL
    HI_DRV_DISP_CALLBACK_S stCB;
    HI_S32 nRet;
#endif

    // s1 check input parameters
    DispCheckDeviceState();

    DispCheckID(enDisp);

    // s2 check whether display opened
    if (!DISP_IsOpened(enDisp))
    {
        DISP_INFO("Display is not opened!\n");
        return HI_SUCCESS;
    }

    // s4 set close state
    DispGetPointerByID(enDisp, pstDisp);

    // s3 Product ask for that display must be enabled at the same time.
    DISP_SetEnable(enDisp, HI_FALSE);    
#ifdef HI_DISP_BUILD_FULL
    stCB.hDst = (HI_HANDLE)pstDisp;
    stCB.pfDISP_Callback = DISP_CB_PreProcess;
    nRet = DISP_ISR_UnRegCallback(enDisp, HI_DRV_DISP_C_INTPOS_0_PERCENT, &stCB);
    DISP_ASSERT(!nRet);

    // s4.2 delete isr
    DISP_ISR_CloseChn(enDisp);
#endif

    pstDisp->bOpen = HI_FALSE;

    return HI_SUCCESS;
}


HI_S32 DispGetVactTime(DISP_S* pstDisp)
{
    HI_S32 vtime;

    if (pstDisp->stDispInfo.u32RefreshRate)
    {
        vtime = (1000 * 100) / pstDisp->stDispInfo.u32RefreshRate;
    }
    else
    {
        vtime = 50;
    }

    if (vtime > 50)
    {
        vtime = 50;
    }
    else if (vtime < 20)
    {
        vtime = 20;
    }

    return vtime;
}


HI_S32 DISP_SetEnable(HI_DRV_DISPLAY_E enDisp, HI_BOOL bEnable)
{
    DISP_S* pstDisp;
    DISP_S* pstDispS;
    HI_S32 nRet, u;
    DISP_INTF_OPERATION_S* pfOpt = DISP_HAL_GetOperationPtr();

    DispCheckDeviceState();

    // s1 check input parameters
    DispCheckID(enDisp);
    DispGetPointerByID(enDisp, pstDisp);

    // s2 check state
    if (bEnable == pstDisp->bEnable)
    {
        DISP_PRINT(" DISP Set enable return!\n");
        return HI_SUCCESS;
    }

    //printk("Set enable 0002  DISP=%d, IS M= %d\n", enDisp, pstDisp->bIsMaster);

    if (pstDisp->bIsSlave)
    {
        return DISP_SetEnable(pstDisp->enAttachedDisp, bEnable);
    }

    if (pstDisp->bIsMaster)
    {
        DispGetPointerByID(pstDisp->enAttachedDisp, pstDispS);
        DispCheckNullPointer(pfOpt);
        DispCheckNullPointer(pfOpt->PF_SetMSChnEnable);

        // s3 if Enable, set all config
        if (bEnable)
        {
            nRet = DispCheckReadyForOpen(enDisp);

            if (nRet)
            {
                return nRet;
            }

            nRet = DispCheckReadyForOpen(pstDisp->enAttachedDisp);

            if (nRet)
            {
                return nRet;
            }

            // s1 set FMT
            nRet = DispSetFormat(enDisp, pstDisp->stSetting.enFormat, pstDisp->stSetting.eDispMode);

            if (nRet)
            {
                DISP_ERROR("Set format failed\n");
                return nRet;
            }

            // s2 set CSC
            nRet = DispSetColor(pstDisp);

            if (nRet)
            {
                DISP_ERROR("Set color failed\n");
                return nRet;
            }


            // s1 set FMT
            nRet = DispSetFormat(pstDisp->enAttachedDisp, pstDispS->stSetting.enFormat,pstDispS->stSetting.eDispMode);

            if (nRet)
            {
                DISP_ERROR("Set format failed\n");
                return nRet;
            }

            // s2 set CSC
            nRet = DispSetColor(pstDispS);

            if (nRet)
            {
                DISP_ERROR("Set color failed\n");
                return nRet;
            }

            // s3 set enable
            pstDisp->eState = DISP_PRIV_STATE_DISABLE;
            DispSetIntfEnable(pstDisp, bEnable);

            pstDispS->eState = DISP_PRIV_STATE_DISABLE;
            DispSetIntfEnable(pstDispS, bEnable);

            pfOpt->PF_SetMSChnEnable(pstDisp->enDisp, pstDisp->enAttachedDisp, 0, bEnable);

#ifndef __DISP_PLATFORM_BOOT__
            HI_DRV_SYS_GetTimeStampMs(&pstDisp->u32StartTime);
#endif

            pstDisp->bEnable = bEnable;
            pstDispS->bEnable = bEnable;
        }
        else
        {
            HI_S32 vtime;

            // addtional 2ms delay for safe
            vtime = DispGetVactTime(pstDisp) + 2;

            // s1 set state and wait ISR Process
            pstDisp->bEnable = bEnable;
            pstDispS->bEnable = bEnable;

            DISP_MSLEEP(2 * vtime);

            u = 0;

            while (pstDisp->eState != DISP_PRIV_STATE_DISABLE)
            {
                DISP_MSLEEP(vtime);
                u++;

                if (u > DISP_SET_TIMEOUT_THRESHOLD)
                {
                    DISP_WARN("Set enable timeout\n");
                    break;
                }
            }

            // s2 set disable
            //DispSetEnable(pstDisp, bEnable);
            //DispSetEnable(pstDispS, bEnable);
            DispSetIntfEnable(pstDisp, bEnable);
            DispSetIntfEnable(pstDispS, bEnable);

            pfOpt->PF_SetMSChnEnable(pstDisp->enDisp, pstDisp->enAttachedDisp, vtime, bEnable);

#ifndef __DISP_PLATFORM_BOOT__
            HI_DRV_SYS_GetTimeStampMs(&pstDisp->u32StartTime);
#endif
            //printk(">>>>>>>>>>>> close disp%d =0x%x\n", pstDisp->enDisp,pstDisp->u32StartTime);
        }
    }
    else
    {
        // s3 if Enable, set all config
        if (bEnable)
        {
            nRet = DispCheckReadyForOpen(enDisp);

            if (nRet)
            {
                return nRet;
            }

            // s1 set FMT
            nRet = DispSetFormat(enDisp, pstDisp->stSetting.enFormat, pstDisp->stSetting.eDispMode);

            if (nRet)
            {
                DISP_ERROR("Set format failed\n");
                return nRet;
            }

            // s2 set CSC
            nRet = DispSetColor(pstDisp);

            if (nRet)
            {
                DISP_ERROR("Set color failed\n");
                return nRet;
            }

            // s3 set enable
            pstDisp->eState = DISP_PRIV_STATE_DISABLE;
            DispSetEnable(pstDisp, bEnable);

#ifndef __DISP_PLATFORM_BOOT__
            HI_DRV_SYS_GetTimeStampMs(&pstDisp->u32StartTime);
#endif
            pstDisp->bEnable = bEnable;

            //printk("disp%d =0x%x, ", pstDisp->enDisp,pstDisp->u32StartTime);
        }
        else
        {
            HI_S32 vtime;

            // addtional 2ms delay for safe
            vtime = DispGetVactTime(pstDisp) + 2;

            // s1 set state and wait ISR Process
            pstDisp->bEnable = bEnable;

            DISP_MSLEEP(2 * vtime);

            u = 0;

            while (pstDisp->eState != DISP_PRIV_STATE_DISABLE)
            {
                DISP_MSLEEP(vtime);
                u++;

                if (u > DISP_SET_TIMEOUT_THRESHOLD)
                {
                    DISP_WARN("Set enable timeout\n");
                    break;
                }
            }

            // s2 set disable
            DispSetEnable(pstDisp, bEnable);

            // s3 wait vdp diable really
            DISP_MSLEEP(vtime);
        }
    }

    return HI_SUCCESS;
}


HI_S32 DISP_GetEnable(HI_DRV_DISPLAY_E enDisp, HI_BOOL* pbEnable)
{
    DISP_S* pstDisp;

    DispCheckDeviceState();

    // s1 check input parameters
    DispCheckID(enDisp);
    DispCheckNullPointer(pbEnable);

    // s2 check whether display opened
    DispShouldBeOpened(enDisp);

    // s3 get ENABLE state and return
    DispGetPointerByID(enDisp, pstDisp);

    *pbEnable = pstDisp->bEnable;

    return HI_SUCCESS;
}

#ifndef __DISP_PLATFORM_BOOT__
//#define DISP_DEBUG_TEST_SET_FORMAT_TIME 1
#endif
HI_S32 DISP_ReviseIntfRGB_VGA(HI_DRV_DISPLAY_E enDisp, HI_DRV_DISP_FMT_E enEncFmt)
{
    DISP_S* pstDisp;
    DISP_INTF_S stDispIntfRGB;
    DISP_INTF_S stDispIntfVGA;

    DispGetPointerByID(enDisp, pstDisp);

    stDispIntfRGB = pstDisp->stSetting.stIntf[HI_DRV_DISP_INTF_RGB0];
    stDispIntfVGA = pstDisp->stSetting.stIntf[HI_DRV_DISP_INTF_VGA0];

    if  ((enEncFmt >= HI_DRV_DISP_FMT_861D_640X480_60) && (enEncFmt <= HI_DRV_DISP_FMT_CUSTOM))
    {
        /*VESA or Custom FMT*/
        if (stDispIntfRGB.bOpen)
        {
            DISP_DelIntf(enDisp, &stDispIntfRGB.stIf);
            stDispIntfRGB.stIf.eID = HI_DRV_DISP_INTF_VGA0;
            DISP_AddIntf(enDisp, &stDispIntfRGB.stIf);
        }
    }
    else
    {
        if (stDispIntfVGA.bOpen)
        {
            DISP_DelIntf(enDisp, &stDispIntfVGA.stIf);
            stDispIntfVGA.stIf.eID = HI_DRV_DISP_INTF_RGB0;
            DISP_AddIntf(enDisp, &stDispIntfVGA.stIf);
        }
    }
    return HI_SUCCESS;
}
HI_S32 DISP_SetFormat(HI_DRV_DISPLAY_E enDisp, HI_DRV_DISP_STEREO_MODE_E enStereo, HI_DRV_DISP_FMT_E enEncFmt)
{
    DISP_S* pstDisp;
    HI_DRV_DISP_FMT_E enEncFmt2;
    HI_S32 nRet;

#ifdef DISP_DEBUG_TEST_SET_FORMAT_TIME
    struct timeval tv;
    HI_U32 t2, t1, t0;
#endif

    // s1 check input parameters
    DispCheckDeviceState();
    DispCheckID(enDisp);
    // s2 get pointer
    DispGetPointerByID(enDisp, pstDisp);
    enEncFmt2 = DispTransferFormat(enDisp, enEncFmt);

    //printk("DISP_SetFormat  disp(%d) :mode (%d) fmt(%d)\n",enDisp,enStereo,enEncFmt);
    if (enEncFmt2 > HI_DRV_DISP_FMT_CUSTOM)
    {
        DISP_ERROR("Display fmt is invalid\n");
        return HI_ERR_DISP_INVALID_PARA;
    }

    if (((HI_DRV_DISPLAY_0 == enDisp) && (pstDisp->bIsSlave))
        && ((enEncFmt2 > HI_DRV_DISP_FMT_SECAM_H) || (enEncFmt2 < HI_DRV_DISP_FMT_PAL))
       )
    {
        DISP_ERROR("attach mode  disp0 not support fmt(%d)\n", enEncFmt2);
        return HI_ERR_DISP_INVALID_PARA;
    }

    if ( (enEncFmt2 == pstDisp->stSetting.enFormat)
         && (enStereo == pstDisp->stSetting.eDispMode)
         && (enEncFmt2 < HI_DRV_DISP_FMT_CUSTOM) )
    {
        DISP_PRINT(" DISP Set same format and return!\n");
        return HI_SUCCESS;
    }

    if (!pstDisp->pstIntfOpt->PF_TestChnEncFmt(pstDisp->enDisp, enEncFmt2))
    {
        DISP_ERROR("Display %d does not support fmt %d\n", (HI_S32)enDisp, (HI_S32)enEncFmt2);
        return HI_ERR_DISP_INVALID_PARA;
    }

    // If hd work at follow-mode, set display0 sd format
    if (   (enDisp == HI_DRV_DISPLAY_1) && pstDisp->bIsMaster
           && DispFmtIsStandDefinition(enEncFmt))
    {
        DISP_SetFormat(pstDisp->enAttachedDisp, HI_DRV_DISP_STEREO_NONE, enEncFmt);
    }

    pstDisp->stSetting.eDispMode = enStereo;
    pstDisp->stSetting.enFormat = enEncFmt2;

    // s4 if display is enabled, disable it and enable it,
    //    and new format will work.
    if (pstDisp->bEnable)
    {
#ifndef __DISP_PLATFORM_BOOT__

        //if ((enDisp == HI_DRV_DISPLAY_1) && (s_pstHDMIFunc))
       
	if (pstDisp->stSetting.stIntf[HI_DRV_DISP_INTF_HDMI0].bOpen && !DispGetHdmiFunction())
        {
            if (s_pstHDMIFunc->pfnHdmiPreFormat && s_pstHDMIFunc->pfnHdmiSetFormat)
            {
                s_pstHDMIFunc->pfnHdmiPreFormat(HI_UNF_HDMI_ID_0, enEncFmt);
            }
        }

#endif

#ifdef DISP_DEBUG_TEST_SET_FORMAT_TIME
        HI_DRV_SYS_GetTimeStampMs(&t0);
#endif

        nRet = DISP_SetEnable(enDisp, HI_FALSE);
        /*
        if interface is RGB mode ;
        when fmt switch to VESA or Custom Fmt :
        	1:change to VGA interface!
        	2:HDMI Data tpye use RGB ;TV FMT use YUV!
        */

        DISP_ReviseIntfRGB_VGA(enDisp, pstDisp->stSetting.enFormat);

#ifdef DISP_DEBUG_TEST_SET_FORMAT_TIME
        HI_DRV_SYS_GetTimeStampMs(&t1);
#endif

        if (enDisp == HI_DRV_DISPLAY_0)
        {
            // DTS2013060905670 : if time between setdisable and set enable is
            // less than 160ms, the screen on TV linked in CVBS flicker.
            // Increase time interval, flicker disappear.
            // set diable use time 60ms
            //DISP_MSLEEP(400);
            DISP_MSLEEP(500);
        }

        nRet = DISP_SetEnable(enDisp, HI_TRUE);

#ifdef DISP_DEBUG_TEST_SET_FORMAT_TIME
        HI_DRV_SYS_GetTimeStampMs(&t2);
        DISP_FATAL("disable use time=%d, enable use time=%d ms\n", t1 - t0, t2 - t1);
#endif

#ifndef __DISP_PLATFORM_BOOT__

        if (pstDisp->stSetting.stIntf[HI_DRV_DISP_INTF_HDMI0].bOpen && !DispGetHdmiFunction())
        {
            if (s_pstHDMIFunc->pfnHdmiPreFormat && s_pstHDMIFunc->pfnHdmiSetFormat)
            {
                s_pstHDMIFunc->pfnHdmiSetFormat(HI_UNF_HDMI_ID_0, enEncFmt, enStereo);
            }
        }

#endif
    }
    
    return HI_SUCCESS;
}

HI_S32 DISP_GetFormat(HI_DRV_DISPLAY_E enDisp, HI_DRV_DISP_STEREO_MODE_E* penStereo, HI_DRV_DISP_FMT_E* penEncFmt)
{
    DISP_S* pstDisp;

    // s1 check input parameters
    DispCheckDeviceState();

    DispCheckID(enDisp);
    DispCheckNullPointer(penStereo);
    DispCheckNullPointer(penEncFmt);

    // s2 get pointer
    DispGetPointerByID(enDisp, pstDisp);

    // s3 if display is not enabled, set format and return
    *penStereo = pstDisp->stSetting.eDispMode;
    *penEncFmt = pstDisp->stSetting.enFormat;

    return HI_SUCCESS;
}

HI_S32 DISP_SetRightEyeFirst(HI_DRV_DISPLAY_E enDisp, HI_BOOL bEnable)
{
    DISP_S* pstDisp;

    DispCheckDeviceState();

    // s1 check input parameters
    DispCheckID(enDisp);

    // s3 check state
    DispGetPointerByID(enDisp, pstDisp);

    // s4 if display is enabled, disable it and enable it,
    //    and new format will work.
    pstDisp->bDispSettingChange = HI_FALSE;
    pstDisp->stSetting.bRightEyeFirst = bEnable;
    pstDisp->bDispSettingChange = HI_TRUE;

    return HI_SUCCESS;
}

HI_S32 DISP_SetVirtScreen(HI_DRV_DISPLAY_E enDisp, HI_RECT_S virtscreen)
{
    DISP_S *pstDisp = HI_NULL, *pstDisp_attach = HI_NULL;

    DispCheckDeviceState();
    DispCheckID(enDisp);
    DispGetPointerByID(enDisp, pstDisp);


    if ((virtscreen.s32Height < 480)
        || (virtscreen.s32Height > 3840)
        || (virtscreen.s32Width < 480)
        || (virtscreen.s32Width > 3840))
    {
        DISP_ERROR("screen window too small ,must be within [480,3840].\n");
        return HI_ERR_DISP_INVALID_PARA;
    }

    if (pstDisp->bIsMaster)
    {   
        pstDisp->bDispSettingChange = HI_FALSE;
        pstDisp->stSetting.stVirtaulScreen.s32Width = virtscreen.s32Width & 0xFFFFFFFCul;
        pstDisp->stSetting.stVirtaulScreen.s32Height = virtscreen.s32Height & 0xFFFFFFFEul;
        pstDisp->bDispSettingChange = HI_TRUE;

        DispGetPointerByID(pstDisp->enAttachedDisp, pstDisp_attach);
        if(pstDisp_attach->bIsSlave)
        {
            pstDisp_attach->bDispSettingChange = HI_FALSE;
            pstDisp_attach->stSetting.stVirtaulScreen.s32Width = virtscreen.s32Width & 0xFFFFFFFCul;
            pstDisp_attach->stSetting.stVirtaulScreen.s32Height = virtscreen.s32Height & 0xFFFFFFFEul;
            pstDisp_attach->bDispSettingChange = HI_TRUE;
        }
    }
    else if (pstDisp->bIsSlave)
    {
        /*non same-source, vscreen can be set.*/
        if (pstDisp->enAttachedDisp >= HI_DRV_DISPLAY_BUTT)
        {
            pstDisp->bDispSettingChange = HI_FALSE;
            pstDisp->stSetting.stVirtaulScreen.s32Width = virtscreen.s32Width & 0xFFFFFFFCul;
            pstDisp->stSetting.stVirtaulScreen.s32Height = virtscreen.s32Height & 0xFFFFFFFEul;
            pstDisp->bDispSettingChange = HI_TRUE;            
        }
        else
        {
            DispGetPointerByID(pstDisp->enAttachedDisp, pstDisp_attach);        
            if (!pstDisp_attach->bIsMaster)
            {
                pstDisp->bDispSettingChange = HI_FALSE;
                pstDisp->stSetting.stVirtaulScreen.s32Width = virtscreen.s32Width & 0xFFFFFFFCul;
                pstDisp->stSetting.stVirtaulScreen.s32Height = virtscreen.s32Height & 0xFFFFFFFEul;
                pstDisp->bDispSettingChange = HI_TRUE;
            }
            else
            {
                return HI_FAILURE;
            }
        }
    } else {
        pstDisp->bDispSettingChange = HI_FALSE;
        pstDisp->stSetting.stVirtaulScreen.s32Width = virtscreen.s32Width & 0xFFFFFFFCul;
        pstDisp->stSetting.stVirtaulScreen.s32Height = virtscreen.s32Height & 0xFFFFFFFEul;
        pstDisp->bDispSettingChange = HI_TRUE;
    }
    
    return HI_SUCCESS;
}

HI_S32 DISP_GetVirtScreen(HI_DRV_DISPLAY_E enDisp, HI_RECT_S* virtscreen)
{
    DISP_S* pstDisp;

    DispCheckDeviceState();
    DispCheckID(enDisp);
    DispGetPointerByID(enDisp, pstDisp);

    *virtscreen = pstDisp->stSetting.stVirtaulScreen;

    return HI_SUCCESS;
}

#define HI_DRV_DISP_OFFSET_MAX 200
#define HI_DRV_DISP_OFFSET_HORIZONTAL_ALIGN 0xFFFFFFFEul
#define HI_DRV_DISP_OFFSET_VERTICAL_ALIGN   0xFFFFFFFCul

HI_S32 DISP_SetScreenOffset(HI_DRV_DISPLAY_E enDisp, HI_DRV_DISP_OFFSET_S *pstScreenOffset)
{
    DISP_S* pstDisp;

    DispCheckDeviceState();
    DispCheckID(enDisp);
    DispGetPointerByID(enDisp, pstDisp);
    DispCheckNullPointer(pstScreenOffset);

    if (  (pstScreenOffset->u32Bottom > HI_DRV_DISP_OFFSET_MAX)
        ||(pstScreenOffset->u32Left   > HI_DRV_DISP_OFFSET_MAX)
        ||(pstScreenOffset->u32Right  > HI_DRV_DISP_OFFSET_MAX)
        ||(pstScreenOffset->u32Top    > HI_DRV_DISP_OFFSET_MAX))
    {
        DISP_ERROR("screen offset must less then 200: %d,%d,%d,%d.\n",
                   pstScreenOffset->u32Left, pstScreenOffset->u32Right,
                   pstScreenOffset->u32Top, pstScreenOffset->u32Bottom);
        return HI_ERR_DISP_INVALID_PARA;
    }

    /*
        if ((screenoffset.u32Bottom & 0x3)
            ||(screenoffset.u32Left & 0x3)
            ||(screenoffset.u32Right & 0x3)
            ||(screenoffset.u32Top   & 0x3))
        {
            DISP_ERROR("screen offset not aligned:%d,%d,%d,%d.\n", screenoffset.u32Bottom,
                                                               screenoffset.u32Left,
                                                               screenoffset.u32Right,
                                                               screenoffset.u32Top);
            return HI_ERR_DISP_INVALID_PARA;
        }
    */
    pstDisp->bDispSettingChange = HI_FALSE;
    pstDisp->stSetting.stOffsetInfo.u32Left   = pstScreenOffset->u32Left   & HI_DRV_DISP_OFFSET_HORIZONTAL_ALIGN;
    pstDisp->stSetting.stOffsetInfo.u32Right  = pstScreenOffset->u32Right  & HI_DRV_DISP_OFFSET_HORIZONTAL_ALIGN;
    pstDisp->stSetting.stOffsetInfo.u32Top    = pstScreenOffset->u32Top    & HI_DRV_DISP_OFFSET_VERTICAL_ALIGN;
    pstDisp->stSetting.stOffsetInfo.u32Bottom = pstScreenOffset->u32Bottom & HI_DRV_DISP_OFFSET_VERTICAL_ALIGN;
    pstDisp->bDispSettingChange = HI_TRUE;

    return HI_SUCCESS;
}

HI_S32 DISP_GetScreenOffset(HI_DRV_DISPLAY_E enDisp, HI_DRV_DISP_OFFSET_S* pstScreenOffset)
{
    DISP_S* pstDisp;

    DispCheckDeviceState();
    DispCheckID(enDisp);
    DispGetPointerByID(enDisp, pstDisp);
    DispCheckNullPointer(pstScreenOffset);

    *pstScreenOffset = pstDisp->stSetting.stOffsetInfo;
    return HI_SUCCESS;
}

//set aspect ratio
HI_S32 DISP_SetAspectRatio(HI_DRV_DISPLAY_E enDisp, HI_U32 u32Ratio_h, HI_U32 u32Ratio_v)
{
    DISP_S* pstDisp;

    DispCheckDeviceState();

    // s1 check input parameters
    DispCheckID(enDisp);

    // s3 check state
    DispGetPointerByID(enDisp, pstDisp);

    // s4 if display is enabled, disable it and enable it,
    //    and new format will work.
    pstDisp->bDispSettingChange = HI_FALSE;

    if (u32Ratio_h && u32Ratio_v)
    {
        pstDisp->stSetting.bCustomRatio = HI_TRUE;
        pstDisp->stSetting.u32CustomRatioWidth  = u32Ratio_h;
        pstDisp->stSetting.u32CustomRatioHeight = u32Ratio_v;
    }
    else
    {
        pstDisp->stSetting.bCustomRatio = HI_FALSE;
    }

    pstDisp->bDispSettingChange = HI_TRUE;

    return HI_SUCCESS;
}

HI_S32 DISP_GetAspectRatio(HI_DRV_DISPLAY_E enDisp, HI_U32* pu32Ratio_h, HI_U32* pu32Ratio_v)
{
    DISP_S* pstDisp;

    DispCheckDeviceState();

    // s1 check input parameters
    DispCheckID(enDisp);
    DispCheckNullPointer(pu32Ratio_h);
    DispCheckNullPointer(pu32Ratio_v);

    // s3 check state
    DispGetPointerByID(enDisp, pstDisp);

    // s4 if display is enabled, disable it and enable it,
    //    and new format will work.
    if (pstDisp->stSetting.bCustomRatio)
    {
        *pu32Ratio_h = pstDisp->stSetting.u32CustomRatioWidth;
        *pu32Ratio_v = pstDisp->stSetting.u32CustomRatioHeight;
    }
    else
    {
        *pu32Ratio_h = 0;
        *pu32Ratio_v = 0;
    }

    return HI_SUCCESS;
}

HI_S32 DISPCheckCustomTiming(HI_DRV_DISP_TIMING_S* pstTiming)
{
    if ((pstTiming->bIDV != HI_TRUE)
        && (pstTiming->bIDV != HI_FALSE)
       )
    {
        DISP_ERROR("para pstTiming->IDV is invalid.\n");
        return HI_ERR_DISP_INVALID_PARA;
    }

    if ((pstTiming->bIHS != HI_TRUE)
        && (pstTiming->bIHS != HI_FALSE)
       )
    {
        DISP_ERROR("para pstTiming->IHS is invalid.\n");
        return HI_ERR_DISP_INVALID_PARA;
    }

    if ((pstTiming->bIVS != HI_TRUE)
        && (pstTiming->bIVS != HI_FALSE)
       )
    {
        DISP_ERROR("para pstTiming->IVS is invalid.\n");
        return HI_ERR_DISP_INVALID_PARA;
    }

    if ((pstTiming->bClkReversal != HI_TRUE)
        && (pstTiming->bClkReversal != HI_FALSE)
       )
    {
        DISP_ERROR("para pstTiming->ClockReversal is invalid.\n");
        return HI_ERR_DISP_INVALID_PARA;
    }

    if (pstTiming->u32DataWidth > 24)
    {
        DISP_ERROR("para pstTiming->DataWidth is invalid.\n");
        return HI_ERR_DISP_INVALID_PARA;
    }

    if (pstTiming->eDataFmt >= HI_DRV_DISP_INTF_DATA_FMT_BUTT)
    {
        DISP_ERROR("para pstTiming->ItfFormat is invalid.\n");
        return HI_ERR_DISP_INVALID_PARA;
    }

    if ((pstTiming->bDitherEnable != HI_TRUE)
        && (pstTiming->bDitherEnable != HI_FALSE)
       )
    {
        DISP_ERROR("para pstTiming->DitherEnable is invalid.\n");
        return HI_ERR_DISP_INVALID_PARA;
    }

    if ( (pstTiming->u32AspectRatioW >= (pstTiming->u32AspectRatioH * 16))
         || ( pstTiming->u32AspectRatioH >= (pstTiming->u32AspectRatioW * 16))
       )
    {
        DISP_ERROR(" DISP Set Custom Timing AspectRatio err!\n");
        return HI_ERR_DISP_INVALID_PARA;
    }

    if ( (pstTiming->u32VertFreq <= 2000) || ( pstTiming->u32VertFreq >= 12000)
       )
    {
        DISP_ERROR(" para pstTiming->u32VertFreq err!\n");
        return HI_ERR_DISP_INVALID_PARA;
    }

    return HI_SUCCESS;

}

HI_S32 DISPCheckCustomTimingIsSet(HI_DRV_DISP_TIMING_S* pstTiming, DISP_SETTING_S* pstSetting)
{
    HI_S32 nRet = HI_FAILURE;
    if (pstSetting->bCustomTimingIsSet)
    {
        if (!memcmp(pstTiming, &(pstSetting->stCustomTimg), sizeof(HI_DRV_DISP_TIMING_S)))
        { nRet = HI_SUCCESS; }
    }
    return nRet;
}
#if 1
#define debug_printf
#define err_printf printk



#define FBDIV_D_MAX       240
#define FBDIV_D_MIN       19
#if 0
#define FBDIV_I_MAX       240
#define FBDIV_I_MIN       19

#else
#define FBDIV_I_MAX       2400
#define FBDIV_I_MIN       16
#endif
/*    pll_refdiv [17:12]  max=64*/
#define REFDIV_I_MAX       (1<<6)
#define REFDIV_I_MIN       1

/*KHz*/
/*20M~600M*/
#define PIX_CLK_MAX      240000
#define PIX_CLK_MIN       20000

/*KHz*/
/*800M~24000M*/
#define FOUTVCO_MAX       2400000
#define FOUTVCO_MIN       800000

/*KHz*/
/*0.5M~800M*/
#define FREF_MIN        500
#define FREF_MAX       800000
#define FREF_CLK       24000

#define FRAC_MAX       (1<<23)
#define PIX_DIFF_MAX       (3)

HI_S32 DispCheck_FOUTVCO(TEMP_PARA_S* pstTmpPara, HI_U32 u32FREF)
{
    HI_U32 i, TmpFBDIV = 0, u32Tmpm;

    if ((pstTmpPara->u32TmpFOUTVCO < FOUTVCO_MAX) && (pstTmpPara->u32TmpFOUTVCO > FOUTVCO_MIN))
    {
        //debug_printf("Check FOUTVCO(%d)\n", pstTmpPara->u32TmpFOUTVCO);
        /*FOUTVCO  =m* FBDIV */
        for ( i = REFDIV_I_MIN; i < REFDIV_I_MAX;  i++ )
        {
            /*div m */
            if ( 0 == u32FREF % i )
            {
                u32Tmpm =  u32FREF / i ;
                // debug_printf("try :FCO (%d),m(%d)", pstTmpPara->u32TmpFOUTVCO, u32Tmpm);
                if (0 == pstTmpPara->u32TmpFOUTVCO % u32Tmpm)
                {
                    TmpFBDIV = pstTmpPara->u32TmpFOUTVCO / u32Tmpm;
                    if ((TmpFBDIV < FBDIV_I_MAX) && (TmpFBDIV >  FBDIV_I_MIN)  )
                    {
                        pstTmpPara->u32TmpM = u32Tmpm;
                        pstTmpPara->u32TmpFBDIV = TmpFBDIV;
                        return HI_SUCCESS;
                    }
                }
            }
        }
    }

    return HI_FAILURE;
}

HI_S32 DispGetPllPSTDIV(TEMP_PARA_S stTmpPara, PLL_PARA_S* pstPllPara)
{
    HI_U32 i;

    /*: set PSTDIV1 PSTDIV2 */
    for ( i = 1; i <= 7; i++ )
    {
        if ((!(stTmpPara.u32TmpN % i)) && (i <= 7) && ( stTmpPara.u32TmpN / i <= 7))
        {
            pstPllPara->u32PSTDIV1 = i;
            pstPllPara->u32PSTDIV2 = stTmpPara.u32TmpN / i;
            pstPllPara->u32REFDIV = pstPllPara->u32FREF / stTmpPara.u32TmpM;
            return HI_SUCCESS;
        }
    }

    return HI_FAILURE;
}
/*PixClk  XKHz*/
HI_S32 MakePixClkParaInteger(HI_U32 u32TestPixClk, PLL_PARA_S* pstPllPara)
{
    //HI_S32  nRet;
    HI_U32 i, j;
    TEMP_PARA_S stTmpPara;
    /*  (0~7)* (0~7)*/
    HI_U32 nPara[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 12, 14, 15, 16, 18, 20, 21, 24, 25, 28, 30, 35, 36, 42, 49};

    /*1: Make  FOUTVCO ,n, FBDIV */
    for ( i = 0; i < (sizeof(nPara) / sizeof(HI_U32)); i++ )
    {
        stTmpPara.u32TmpFOUTVCO = u32TestPixClk * 2 * 2 * nPara[i];

        if (HI_SUCCESS == DispCheck_FOUTVCO(&stTmpPara, pstPllPara->u32FREF))
        {
            stTmpPara.u32TmpN = nPara[i];
            pstPllPara->u32FOUTVCO = stTmpPara.u32TmpFOUTVCO;
            pstPllPara->u32FBDIV = stTmpPara.u32TmpFBDIV;

            /*2:get PSTDIV1 PSTDIV2 value*/
            for ( j = 1; j <= 7; j++ )
            {
                if ((!(stTmpPara.u32TmpN % j)) && (j <= 7) && ( stTmpPara.u32TmpN / j <= 7))
                {
                    pstPllPara->u32PSTDIV1 = j;
                    pstPllPara->u32PSTDIV2 = stTmpPara.u32TmpN / j;
                    pstPllPara->u32REFDIV = pstPllPara->u32FREF / stTmpPara.u32TmpM;
                    return HI_SUCCESS;
                }
            }
        }
    }

    return HI_FAILURE;
}


HI_S32 MakeReg(PLL_PARA_S* pstPllPara)
{
    pstPllPara->u32REG1 = (HI_U32)(pstPllPara->u32PSTDIV1 << 28) + (HI_U32)(pstPllPara->u32PSTDIV2 << 24) + (HI_U32)(pstPllPara->u32FRAC);
    pstPllPara->u32REG2 = (HI_U32)( 1 << 25) + (HI_U32)(pstPllPara->u32REFDIV << 12) + (HI_U32)(pstPllPara->u32FBDIV);
    return HI_SUCCESS;
}

HI_S32 DispCheckCustomTimingPllPara(HI_U32 u32TestPixClk, REG_PARA_S* pstRegPara)
{
    HI_S32 nRet;
    PLL_PARA_S stPllPara;

    HI_U32 i, TestOk, TmpDiff, TmpClkAdd, TmpClkDec, u32CheckPixClk = 0;
    //HI_U32 all_h, all_v, all_pix, pixclk;

    memset(&stPllPara, 0, sizeof(PLL_PARA_S));
    stPllPara.u32FREF = 24000;
    nRet = MakePixClkParaInteger(u32TestPixClk, &stPllPara);

    if (HI_SUCCESS != nRet)
    {
        TestOk = 0;
        TmpDiff = (u32TestPixClk * PIX_DIFF_MAX) / 10000;

        for (i = 0; i < TmpDiff; i++)
        {
            TmpClkAdd = u32TestPixClk + i;
            TmpClkDec  = u32TestPixClk - i;

            if (HI_SUCCESS == MakePixClkParaInteger(TmpClkAdd, &stPllPara))
            {
                u32CheckPixClk = TmpClkAdd;
                TestOk = 1;
                break;
            }

            if (HI_SUCCESS == MakePixClkParaInteger(TmpClkDec, &stPllPara))
            {
                u32CheckPixClk = TmpClkDec;
                TestOk = 1;
                break;
            }
        }
    }
    else
    { TestOk = 1; }

    if (!TestOk)
    {
        return HI_FAILURE;
    }

    MakeReg(&stPllPara);
    pstRegPara->u32ClkPara0 = stPllPara.u32REG1;
    pstRegPara->u32ClkPara1 = stPllPara.u32REG2;
    pstRegPara->u32CalcPixFreq =  u32CheckPixClk;
    // s2 set calculate pll reg para

    return HI_SUCCESS;
}
HI_S32 DISP_GetTimingReg(HI_DRV_DISPLAY_E eChn, HI_DRV_DISP_TIMING_S* pstTiming)
{
    HI_S32 Ret;
    /*VESA and CUSTOM  format need calculate Reg value!*/
    // s1 make pixclk
    REG_PARA_S stRegPara;

    if ((pstTiming->u32ClkPara0) || (pstTiming->u32ClkPara1))
    {
        DISP_ERROR("used custom clk reg value! (0x%x)(0x%x)\n", pstTiming->u32ClkPara0, pstTiming->u32ClkPara1);
        return HI_SUCCESS;
    }

    if ( (pstTiming->u32VertFreq / 100) > 120)
    {
        DISP_ERROR("freshRate  out of rang  (0~120)!!\n");
        return HI_FAILURE;
    }

    if ( 0 == pstTiming->u32PixFreq )
    {
        pstTiming->u32PixFreq =  (( pstTiming->u32HBB + pstTiming->u32HACT + pstTiming->u32HFB)
                                  * ( pstTiming->u32VBB + pstTiming->u32VACT + pstTiming->u32VFB)
                                  * (pstTiming->u32VertFreq / 100)) / 1000;
    }

    if ((20000 > pstTiming->u32PixFreq) || (600000 < pstTiming->u32PixFreq))
    {
        DISP_ERROR("u32PixClk (%d)out of rang  (20000~600000)!!\n", pstTiming->u32PixFreq);
        return HI_FAILURE;
    }

    DISP_MEMSET(&stRegPara, 0, sizeof(REG_PARA_S));
    Ret = DispCheckCustomTimingPllPara(pstTiming->u32PixFreq, &stRegPara);

    if (HI_SUCCESS != Ret)
    {
        DISP_ERROR("make  REG  err!!\n");
        return HI_FAILURE;
    }

    pstTiming->u32ClkPara0 = stRegPara.u32ClkPara0;
    pstTiming->u32ClkPara1 = stRegPara.u32ClkPara1;
    DISP_INFO("u32PixClk (%d)reg(0x%x)(0x%x)!!\n", pstTiming->u32PixFreq, pstTiming->u32ClkPara0, pstTiming->u32ClkPara1);
    return HI_SUCCESS;
}

#endif

HI_S32 DISP_SetCustomTiming(HI_DRV_DISPLAY_E enDisp, HI_DRV_DISP_TIMING_S* pstTiming)
{
    DISP_S* pstDisp;
    HI_S32 nRet;
    // s1 check input parameters

    DispCheckDeviceState();

    DispCheckID(enDisp);
    // s2 get pointer
    DispGetPointerByID(enDisp, pstDisp);
    DispCheckNullPointer(pstTiming);

    //todo check color setting

    nRet = DISPCheckCustomTiming(pstTiming);
    if (HI_SUCCESS != nRet)
    {
        DISP_ERROR("Check Custom Timing Para err! (%d)\n", nRet);
        return nRet;
    }

    nRet = DISPCheckCustomTimingIsSet(pstTiming, &(pstDisp->stSetting));
    if (HI_SUCCESS == nRet)
    {
        DISP_ERROR(" Custom Timing set yet! (%d)\n", nRet);
        return nRet;
    }
    nRet = DISP_GetTimingReg( enDisp, pstTiming);

    if (HI_SUCCESS != nRet)
    {
        DISP_ERROR("Get Custom Timing Reg Para err! (%d)\n", nRet);
        return nRet;
    }

    pstDisp->stSetting.stCustomTimg = *pstTiming;

    pstDisp->stSetting.bCustomTimingIsSet = HI_TRUE;
    pstDisp->stSetting.bCustomTimingChange  = HI_TRUE;

    return HI_SUCCESS;
}
HI_S32 DISP_GetCustomTiming(HI_DRV_DISPLAY_E enDisp, HI_DRV_DISP_TIMING_S* pstTiming)
{
    DISP_S* pstDisp;

    // s1 check input parameters
    DispCheckDeviceState();

    DispCheckID(enDisp);
    // s2 get pointer
    DispGetPointerByID(enDisp, pstDisp);
    DispCheckNullPointer(pstTiming);

    *pstTiming = pstDisp->stSetting.stCustomTimg;

    return HI_SUCCESS;
}

HI_S32 DISP_SetBGColor(HI_DRV_DISPLAY_E enDisp, HI_DRV_DISP_COLOR_S* pstBGColor)
{
    DISP_S* pstDisp;
    HI_S32 Ret = HI_SUCCESS;

    // s1 check input parameters
    DispCheckDeviceState();

    DispCheckID(enDisp);

    // s2 get pointer
    DispGetPointerByID(enDisp, pstDisp);
    DispCheckNullPointer(pstBGColor);

    //todo check color setting
    /*
        DISP_PRINT("xxxxxxDISP_SetBGColor R=%d,G=%d,B=%d\n",
                     pstBGColor->u8Red,
                     pstBGColor->u8Green, pstBGColor->u8Blue);
    */
    pstDisp->stSetting.stBgColor = *pstBGColor;

    if (pstDisp->bEnable)
    {
        Ret = DispSetColor(pstDisp);
        if (Ret != HI_SUCCESS)
        { goto __SET_BGCOLOR__; }
    }

__SET_BGCOLOR__:
    if (pstDisp->bIsMaster)
    {
        //DISP_PRINT("DISP_SetColor, attech  disp ID = %d\n", pstDisp->enAttachedDisp);
        Ret = DISP_SetBGColor(pstDisp->enAttachedDisp, pstBGColor);
        if (Ret != HI_SUCCESS)
        { goto __SET_BGCOLOR_EXIT_; }
    }

__SET_BGCOLOR_EXIT_:
    return Ret;
}
HI_S32 DISP_GetBGColor(HI_DRV_DISPLAY_E enDisp, HI_DRV_DISP_COLOR_S* pstBGColor)
{
    DISP_S* pstDisp;

    // s1 check input parameters
    DispCheckDeviceState();

    DispCheckID(enDisp);

    // s2 get pointer
    DispGetPointerByID(enDisp, pstDisp);
    DispCheckNullPointer(pstBGColor);

    //todo check color setting

    *pstBGColor = pstDisp->stSetting.stBgColor;

    return HI_SUCCESS;
}

HI_S32 DISP_SetLayerZorder(HI_DRV_DISPLAY_E enDisp, HI_DRV_DISP_LAYER_E enLayer, HI_DRV_DISP_ZORDER_E enZFlag)
{
    HI_DRV_DISP_LAYER_E enUpLayer;
    DISP_S *pstDisp = HI_NULL;
    DispGetPointerByID(enDisp, pstDisp);

    if ((HI_DRV_DISP_ZORDER_MOVETOP == enZFlag) || (HI_DRV_DISP_ZORDER_MOVEUP == enZFlag))
    {
        /*if  move top!*/
        enUpLayer = (enLayer == HI_DRV_DISP_LAYER_VIDEO) ? HI_DRV_DISP_LAYER_VIDEO : HI_DRV_DISP_LAYER_GFX;
    }
    else
    {
         /*if  move bottom!*/
         enUpLayer = (enLayer == HI_DRV_DISP_LAYER_VIDEO)?HI_DRV_DISP_LAYER_GFX:HI_DRV_DISP_LAYER_VIDEO;
    }

    if (enUpLayer == HI_DRV_DISP_LAYER_VIDEO)
    {
        pstDisp->stSetting.enLayer[0] = HI_DRV_DISP_LAYER_GFX;
        pstDisp->stSetting.enLayer[1] = HI_DRV_DISP_LAYER_VIDEO;
    }
    else
    {
        pstDisp->stSetting.enLayer[0] = HI_DRV_DISP_LAYER_VIDEO;
        pstDisp->stSetting.enLayer[1] = HI_DRV_DISP_LAYER_GFX;
    }
     
    pstDisp->pstIntfOpt->PF_CBM_MovTOP(enDisp,enUpLayer);
    
    /*if attach mode ,set Attached disp too!*/
    if (pstDisp->bIsMaster)
        DISP_SetLayerZorder(pstDisp->enAttachedDisp,enLayer,enZFlag);

    return HI_SUCCESS;
}

HI_S32 DISP_GetLayerZorder(HI_DRV_DISPLAY_E enDisp, HI_DRV_DISP_LAYER_E enLayer, HI_U32 *pu32Zorder)
{
    DISP_S *pstDisp = HI_NULL;
    HI_U32 index,i;
    DispGetPointerByID(enDisp, pstDisp);
    
    index = 0;
    for (i =0; i < HI_DRV_DISP_LAYER_BUTT ; i++)
    {
        if (enLayer == pstDisp->stSetting.enLayer[i])
        {
            index = i;
            break;
        }
    }
	
    *pu32Zorder = index;
    
    return HI_SUCCESS;
}

HI_S32 DISP_TestMacrovisionSupport(HI_DRV_DISPLAY_E enDisp, HI_BOOL* pbSupport)
{

    return HI_SUCCESS;
}

#ifdef HI_DISP_BUILD_FULL
//snapshot
HI_S32 DISP_AcquireSnapshot(HI_DRV_DISPLAY_E enDisp, HI_DRV_VIDEO_FRAME_S* pstSnapShotFrame, HI_HANDLE *snapshotHandleOut)
{
    DISP_CAST_S* pstCast;
    HI_S32 Ret;
    HI_S32 i = 0;
    DISP_S* pstDisp;

    // s1 check input parameters
    DispCheckDeviceState();
    DispCheckID(enDisp);

    DispGetPointerByID(enDisp, pstDisp);
    DispCheckNullPointer(pstSnapShotFrame);
    if (!pstDisp->bOpen || !pstDisp->bEnable)
    {
        DISP_ERROR("Disp not open, cannot snapshot!\n");
        return HI_ERR_DISP_NOT_OPEN;
    }

    if (!pstDisp->pstIntfOpt->PF_TestChnSupportCast(pstDisp->enDisp))
    {
        DISP_ERROR("Disp %d not support cast!\n", (HI_S32)enDisp);
        return HI_ERR_DISP_INVALID_OPT;
    }

    /* reget pstCast for user may call cast_destroy. */
    pstCast = (DISP_CAST_S*)pstDisp->Cast_ptr;

    if (pstCast)
    {
        /* notify cast to shedule wbc. */
        pstCast->bScheduleWbc = HI_TRUE;;
        pstCast->u32Ref++;
    retry:

        if (pstCast->bEnable)
        {
            msleep(40);

            if (!pstCast->bScheduleWbcStatus && ++i < 3)
            {
                goto retry;
            }

            if (!pstCast->bScheduleWbcStatus)
            {
                Ret = HI_ERR_DISP_TIMEOUT;
                goto out;
            }
        }
    }

    Ret = DISP_Acquire_Snapshot(enDisp, snapshotHandleOut, pstSnapShotFrame);
    
out:    
    pstCast = (DISP_CAST_S*)pstDisp->Cast_ptr;
    if (pstCast)
    {
        pstCast->bScheduleWbc = HI_FALSE;
        pstCast->bScheduleWbcStatus = HI_FALSE;
        /* In order to decrease pstCast->u32Ref. Maybe do some destroy actually */
        DISP_DestroyCast(pstDisp->hCast);
    }

    return Ret;
}
HI_S32 DISP_ReleaseSnapshot(HI_DRV_DISPLAY_E enDisp, HI_DRV_VIDEO_FRAME_S* pstSnapShotFrame, HI_HANDLE snapshotHandle)
{
    HI_S32 Ret;
    DISP_S* pstDisp;

    DispCheckDeviceState();
    DispCheckID(enDisp);
    DispGetPointerByID(enDisp, pstDisp);
    DispCheckNullPointer(pstSnapShotFrame);
    if (!pstDisp->bOpen || !pstDisp->bEnable)
    {
        DISP_ERROR("Disp not open, cannot snapshot!\n");
        return HI_ERR_DISP_NOT_OPEN;
    }

    if (!pstDisp->pstIntfOpt->PF_TestChnSupportCast(pstDisp->enDisp))
    {
        DISP_ERROR("Disp %d not support cast!\n", (HI_S32)enDisp);
        return HI_ERR_DISP_INVALID_OPT;
    }

    Ret = DISP_Release_Snapshot(enDisp, snapshotHandle, pstSnapShotFrame);
    return Ret;
}


HI_S32 DISP_DestroySnapshot(HI_HANDLE hSnapshot)
{
    HI_S32 nRet;
    
    DispCheckDeviceState();
    
    if (!hSnapshot)
        return HI_ERR_DISP_NULL_PTR;
    
    nRet = DISP_SnapshotDestroy(hSnapshot);
    
    return nRet;    
}


//miracast
HI_S32 DISP_CreateCast(HI_DRV_DISPLAY_E enDisp, HI_DRV_DISP_CAST_CFG_S* pstCfg, HI_HANDLE* phCast)
{
    DISP_S* pstDisp;
    HI_S32 nRet;
    HI_HANDLE cast_ptr = HI_NULL;
    HI_DISP_DISPLAY_INFO_S pstInfo;

    /* check input parameters*/
    DispCheckDeviceState();
    DispCheckID(enDisp);
    DispCheckNullPointer(pstCfg);
    DispCheckNullPointer(phCast);

    /*check open status.*/
    DispShouldBeOpened(enDisp);


    /*check cast support .*/
    DispGetPointerByID(enDisp, pstDisp);

    if (!pstDisp->pstIntfOpt->PF_TestChnSupportCast(pstDisp->enDisp))
    {
        DISP_ERROR("Disp %d not support cast!\n", (HI_S32)enDisp);
        return HI_ERR_DISP_INVALID_OPT;
    }

    DISP_GetDisplayInfo(enDisp,  &pstInfo);

    nRet = DISP_CastCreate(enDisp, &pstInfo, pstCfg, &cast_ptr);

    if (!nRet)
    {
        /*hcast is a user handle containing of mod id info and display channel.
         *for there is 1:1 relationship bettween display channel and cast handle.*/
        pstDisp->hCast      = (HI_ID_DISP << 16) | enDisp;
        *phCast              = pstDisp->hCast;

        /*this is a cast instance, we can get it from DISP_S struct definition.*/
        pstDisp->Cast_ptr   = cast_ptr;
    }

    DISP_WARN("DISP_CreateCast  pstDisp->hCast = 0x%x, cast_ptr:0x%x\n", (HI_U32)pstDisp->hCast, pstDisp->Cast_ptr);

    return nRet;
}

HI_S32 DISP_DestroyCast(HI_HANDLE hCast)
{
    HI_DRV_DISPLAY_E enDisp;
    DISP_S* pstDisp;
    HI_S32 nRet;
    HI_HANDLE cast_ptr = 0;
    DISP_CAST_S* pstCast;

    // s1 check input parameters
    DispCheckDeviceState();
    DispCheckCastHandleValid(hCast);

    // s2 TODO: search display
    enDisp  =  hCast & 0xff;
    nRet = DispSearchCastHandle(&cast_ptr, enDisp);

    if (nRet)
    {
        DISP_ERROR("DISP cast not exist!\n");
        return HI_ERR_DISP_NOT_EXIST;
    }

    // s3 check whether display opened
    DispShouldBeOpened(enDisp);
    // s4 get pointer
    DispGetPointerByID(enDisp, pstDisp);

    pstCast = (DISP_CAST_S*)cast_ptr;

    if (--pstCast->u32Ref > 0)
    { 
        return HI_SUCCESS;
    }

    // s5 destroy cast
    nRet = DISP_CastDestroy(cast_ptr);

    pstDisp->hCast      = HI_NULL;
    pstDisp->Cast_ptr   = HI_NULL;

    return nRet;
}

HI_S32 DISP_SetCastEnable(HI_HANDLE hCast, HI_BOOL bEnable)
{
    HI_DRV_DISPLAY_E enDisp;
    HI_S32 nRet;
    HI_HANDLE cast_ptr = 0;

    // s1 check input parameters
    DispCheckDeviceState();
    DispCheckCastHandleValid(hCast);
    DISP_WARN("DISP_SetCastEnable  hCast = 0x%x\n", (HI_U32)hCast);

    // s2 TODO: search display
    enDisp  =  (HI_DRV_DISPLAY_E)(hCast & 0xff);
    // s3 check whether display opened
    DispShouldBeOpened(enDisp);
    
    nRet = DispSearchCastHandle(&cast_ptr, enDisp);
    if (nRet)
    {
        DISP_ERROR("DISP cast not exist!\n");
        return HI_ERR_DISP_NOT_EXIST;
    }
    
    nRet = DISP_CastSetEnable(cast_ptr, bEnable);
    return nRet;
}


HI_S32 DISP_GetCastEnable(HI_HANDLE hCast, HI_BOOL* pbEnable)
{
    HI_DRV_DISPLAY_E enDisp;
    HI_S32 nRet;
    HI_HANDLE cast_ptr = 0;

    // s1 check input parameters
    DispCheckDeviceState();
    DispCheckCastHandleValid(hCast);
    DispCheckNullPointer(pbEnable);

    // s2 TODO: search display
    enDisp  = (HI_DRV_DISPLAY_E)(hCast & 0xff);
    // s3 check whether display opened
    DispShouldBeOpened(enDisp);
    
    nRet = DispSearchCastHandle(&cast_ptr, enDisp);
    if (nRet)
    {
        DISP_ERROR("DISP cast not exist!\n");
        return HI_ERR_DISP_NOT_EXIST;
    }

    nRet = DISP_CastGetEnable(cast_ptr, pbEnable);
    return nRet;
}

HI_S32 DISP_AcquireCastFrame(HI_HANDLE hCast, HI_DRV_VIDEO_FRAME_S* pstCastFrame)
{
    HI_DRV_DISPLAY_E enDisp;
    HI_S32 nRet;
    HI_HANDLE cast_ptr = 0;

    // s1 check input parameters
    DispCheckDeviceState();
    DispCheckCastHandleValid(hCast);

    // s2 TODO: search display
    enDisp  = (HI_DRV_DISPLAY_E)( hCast & 0xff);
    // s3 check whether display opened
    DispShouldBeOpened(enDisp);
    
    nRet = DispSearchCastHandle(&cast_ptr, enDisp);
    if (nRet)
    {
        DISP_ERROR("DISP cast not exist!\n");
        return HI_ERR_DISP_NOT_EXIST;
    }

    nRet = DISP_CastAcquireFrame(cast_ptr, pstCastFrame);
    return nRet;
}

HI_S32 DISP_ReleaseCastFrame(HI_HANDLE hCast, HI_DRV_VIDEO_FRAME_S* pstCastFrame)
{
    HI_DRV_DISPLAY_E enDisp;
    HI_S32 nRet;
    HI_HANDLE cast_ptr = 0;

    // s1 check input parameters
    DispCheckDeviceState();
    DispCheckCastHandleValid(hCast);
    DispCheckNullPointer(pstCastFrame);

    // s2 TODO: search display
    enDisp  = (HI_DRV_DISPLAY_E)(hCast & 0xff);
    // s3 check whether display opened
    DispShouldBeOpened(enDisp);
    
    nRet = DispSearchCastHandle(&cast_ptr, enDisp);
    if (nRet)
    {
        DISP_ERROR("DISP cast not exist!\n");
        return HI_ERR_DISP_NOT_EXIST;
    }

    nRet = DISP_CastReleaseFrame(cast_ptr, pstCastFrame);
    return nRet;
}

HI_S32 DISP_External_Attach(HI_HANDLE hCast, HI_HANDLE hsink)
{
    HI_DRV_DISPLAY_E enDisp;
    HI_S32 nRet;
    HI_HANDLE cast_ptr = 0;

    // s1 check input parameters
    DispCheckDeviceState();
    DispCheckCastHandleValid(hCast);

    // s2 TODO: search display
    enDisp  =  (HI_DRV_DISPLAY_E)(hCast & 0xff);
    // s3 check whether display opened
    DispShouldBeOpened(enDisp);
    
    nRet = DispSearchCastHandle(&cast_ptr, enDisp);
    if (nRet)
    {
        DISP_ERROR("DISP cast not exist!\n");
        return HI_ERR_DISP_NOT_EXIST;
    }
    
    nRet = DISP_Cast_AttachSink(cast_ptr, hsink);
    return nRet;
}

HI_S32 DISP_External_DeAttach(HI_HANDLE hCast, HI_HANDLE hsink)
{
    HI_DRV_DISPLAY_E enDisp;
    HI_S32 nRet;
    HI_HANDLE cast_ptr = 0;

    // s1 check input parameters
    DispCheckDeviceState();
    DispCheckCastHandleValid(hCast);

    // s2 TODO: search display
    enDisp  =  (HI_DRV_DISPLAY_E)(hCast & 0xff);    
    //s3 check whether display opened
    DispShouldBeOpened(enDisp);
    
    nRet = DispSearchCastHandle(&cast_ptr, enDisp);
    if (nRet)
    {
        DISP_ERROR("DISP cast not exist!\n");
        return HI_ERR_DISP_NOT_EXIST;
    }

    nRet = DISP_Cast_DeAttachSink(cast_ptr, hsink);
    return nRet;
}

/*currently, this function only called by venc to change cast resolution. */
HI_S32 DRV_DISP_SetCastAttr(HI_HANDLE hCast, HI_DRV_DISP_Cast_Attr_S *castAttr)
{
    HI_DRV_DISPLAY_E enDisp;
    HI_S32 nRet;
    HI_HANDLE cast_ptr = 0;

    DispCheckDeviceState();
    DispCheckCastHandleValid(hCast);

    enDisp  = (HI_DRV_DISPLAY_E)(hCast & 0xff);
    DispShouldBeOpened(enDisp);
    
    nRet = DispSearchCastHandle(&cast_ptr, enDisp);
    if (nRet)
    {
        DISP_ERROR("DISP cast not exist!\n");
        return HI_ERR_DISP_NOT_EXIST;
    }   
    
    nRet = DISP_Cast_SetAttr(cast_ptr, castAttr);
    return nRet;
}

HI_S32 DRV_DISP_GetCastAttr(HI_HANDLE hCast, HI_DRV_DISP_Cast_Attr_S *castAttr)
{
    HI_DRV_DISPLAY_E enDisp;
    HI_S32 nRet;
    HI_HANDLE cast_ptr = 0;

    DispCheckDeviceState();
    DispCheckCastHandleValid(hCast);

    enDisp  =  (HI_DRV_DISPLAY_E)(hCast & 0xff);
    DispShouldBeOpened(enDisp);
    
    nRet = DispSearchCastHandle(&cast_ptr, enDisp);
    if (nRet)
    {
        DISP_ERROR("DISP cast not exist!\n");
        return HI_ERR_DISP_NOT_EXIST;
    }
    
    nRet = DISP_Cast_GetAttr(cast_ptr, castAttr);
    return nRet;
}
#endif



HI_S32 DISP_SetColor(HI_DRV_DISPLAY_E enDisp, HI_DRV_DISP_COLOR_SETTING_S* pstCS)
{
    DISP_S* pstDisp;

    // s1 check input parameters
    DispCheckDeviceState();

    DispCheckID(enDisp);

    // s2 get pointer
    DispGetPointerByID(enDisp, pstDisp);
    DispCheckNullPointer(pstCS);

    //todo check color setting

    pstDisp->stSetting.stColor = *pstCS;

    //DISP_PRINT("DISP_SetColor, id=%d, en=%d\n", enDisp, pstDisp->bEnable);
    if (pstDisp->bEnable)
    {
        pstDisp->bDispSettingChange = HI_TRUE;
    }

    if (pstDisp->bIsMaster)
    {
        //DISP_PRINT("DISP_SetColor, attech  disp ID = %d\n", pstDisp->enAttachedDisp);
        DISP_SetColor(pstDisp->enAttachedDisp, pstCS);
    }

    return HI_SUCCESS;
}

HI_S32 DISP_GetColor(HI_DRV_DISPLAY_E enDisp, HI_DRV_DISP_COLOR_SETTING_S* pstCS)
{
    DISP_S* pstDisp;

    // s1 check input parameters
    DispCheckDeviceState();

    DispCheckID(enDisp);

    // s2 get pointer
    DispGetPointerByID(enDisp, pstDisp);
    DispCheckNullPointer(pstCS);

    *pstCS = pstDisp->stSetting.stColor;

    return HI_SUCCESS;
}



HI_S32 DISP_SetMacrovisionCustomer(HI_DRV_DISPLAY_E enDisp, HI_VOID* pData)
{

    return HI_SUCCESS;
}

HI_S32 DISP_SetMacrovision(HI_DRV_DISPLAY_E enDisp, HI_DRV_DISP_MACROVISION_E enMode)
{

    return HI_SUCCESS;
}

HI_S32 DISP_GetMacrovision(HI_DRV_DISPLAY_E enDisp, HI_DRV_DISP_MACROVISION_E* penMode)
{

    return HI_SUCCESS;
}

//cgms-a
HI_S32 DISP_SetCGMS_A(HI_DRV_DISPLAY_E enDisp, HI_DRV_DISP_CGMSA_CFG_S* pstCfg)
{

    return HI_SUCCESS;
}


//vbi
HI_S32 DISP_CreateVBIChannel(HI_DRV_DISPLAY_E enDisp, HI_DRV_DISP_VBI_CFG_S* pstCfg, HI_HANDLE* phVbi)
{

    return HI_SUCCESS;
}

HI_S32 DISP_DestroyVBIChannel(HI_HANDLE hVbi)
{

    return HI_SUCCESS;
}

HI_S32 DISP_SendVbiData(HI_HANDLE hVbi, HI_DRV_DISP_VBI_DATA_S* pstVbiData)
{

    return HI_SUCCESS;
}

HI_S32 DISP_SetWss(HI_HANDLE hVbi, HI_DRV_DISP_WSS_DATA_S* pstWssData)
{

    return HI_SUCCESS;
}


//may be deleted
HI_S32 DISP_SetHdmiIntf(HI_DRV_DISPLAY_E enDisp, HI_DRV_DISP_HDMI_S* pstCfg)
{

    return HI_SUCCESS;
}

HI_S32 DISP_GetHdmiIntf(HI_DRV_DISPLAY_E enDisp, HI_DRV_DISP_HDMI_S* pstCfg)
{

    return HI_SUCCESS;
}

HI_S32 DISP_SetSetting(HI_DRV_DISPLAY_E enDisp, DISP_SETTING_S* pstSetting)
{

    return HI_SUCCESS;
}

HI_S32 DISP_GetSetting(HI_DRV_DISPLAY_E enDisp, DISP_SETTING_S* pstSetting)
{

    return HI_SUCCESS;
}


HI_S32 DISP_AddIntf(HI_DRV_DISPLAY_E enDisp, HI_DRV_DISP_INTF_S* pstIntf)
{
    DISP_S* pstDisp;
    HI_S32 nRet;
    DISP_INTF_OPERATION_S* pfOpt = DISP_HAL_GetOperationPtr();

    // s1 check input parameters
    DispCheckDeviceState();

    DispCheckID(enDisp);
    DispCheckNullPointer(pstIntf);
    //printk("add****DISP_AddIntf**************** (%d)(%d)(%d)\n",pstIntf->eID,pstIntf->u8VDAC_Pb_B,pstIntf->u8VDAC_Pr_R);
    nRet = DispCheckIntfValid(pstIntf);

    if (nRet)
    {
        DISP_ERROR("Invalid intf parameters in %s!\n", __FUNCTION__);
        return HI_ERR_DISP_INVALID_PARA;
    }

    // s2 get pointer
    DispGetPointerByID(enDisp, pstDisp);

    /*judge VGA mode*/
    if ((pstIntf->eID == HI_DRV_DISP_INTF_RGB0) && (pstDisp->stSetting.enFormat >= HI_DRV_DISP_FMT_861D_640X480_60)
        && (pstDisp->stSetting.enFormat <= HI_DRV_DISP_FMT_CUSTOM))
    {
        pstIntf->eID = HI_DRV_DISP_INTF_VGA0;
    }

    // s3.2 check whether eIntf exists
    if (DispCheckIntfExist(enDisp, pstIntf))
    {
        return HI_SUCCESS;
    }

    DispPrepareInft(pstIntf);

    // s3.1 check whether eIntf is supported
    nRet = DispAddIntf(pstDisp, pstIntf);

    if ( pstDisp->bEnable && !nRet)
    {
        //printk(">>>>> xb1\n");
        DispCheckNullPointer(pfOpt);
        DispCheckNullPointer(pfOpt->PF_ResetIntfFmt2);
        DispCheckNullPointer(pfOpt->PF_SetIntfEnable2);
        pfOpt->PF_ResetIntfFmt2(pstDisp->enDisp,
                                DispGetIntfPtr(pstDisp, pstIntf->eID),
                                pstDisp->stSetting.enFormat);

        pfOpt->PF_SetIntfEnable2(pstDisp->enDisp,
                                 DispGetIntfPtr(pstDisp, pstIntf->eID),
                                 HI_TRUE);
    }

    return nRet;
}

HI_S32 DISP_DelIntf(HI_DRV_DISPLAY_E enDisp, HI_DRV_DISP_INTF_S* pstIntf)
{
    DISP_S* pstDisp;
    DISP_INTF_OPERATION_S* pfOpt = DISP_HAL_GetOperationPtr();
    DISP_INTF_S* pstIf;

    // s1 check input parameters
    DispCheckDeviceState();

    DispCheckID(enDisp);
    DispCheckNullPointer(pstIntf);

    // s2 get pointer
    DispGetPointerByID(enDisp, pstDisp);

    // s3 check if eIntf exists
    if (!DispCheckIntfExistByType(enDisp, pstIntf))
    {
        return HI_FAILURE;
    }

    pstIf = DispGetIntfPtr(pstDisp, pstIntf->eID);

    DispCheckNullPointer(pfOpt);
    DispCheckNullPointer(pfOpt->PF_SetIntfEnable2);
    DispCheckNullPointer(pfOpt->PF_ReleaseIntf2);
    // s4 set intf disable
    pfOpt->PF_SetIntfEnable2(pstDisp->enDisp, pstIf, HI_FALSE);

    // s5 release intf
    pfOpt->PF_ReleaseIntf2(pstDisp->enDisp, pstIf);

    DispCleanIntf(pstIf);

    if (HI_DRV_DISP_INTF_HDMI0 == pstIntf->eID)
    {
        DispPrepareHDMI(pstIntf->eID);
    }

    return HI_SUCCESS;
}


HI_S32 DISP_GetSlave(HI_DRV_DISPLAY_E enDisp, HI_DRV_DISPLAY_E* penSlave)
{
    DISP_S* pstDisp;

    // s1 check input parameters
    DispCheckDeviceState();

    DispCheckID(enDisp);
    DispCheckNullPointer(penSlave);

    // s2 get pointer
    DispGetPointerByID(enDisp, pstDisp);

    // s3 check if eIntf exists
    if (!pstDisp->bIsMaster)
    {
        return HI_FAILURE;
    }

    *penSlave = pstDisp->enAttachedDisp;

    return HI_SUCCESS;
}

HI_S32 DISP_GetMaster(HI_DRV_DISPLAY_E enDisp, HI_DRV_DISPLAY_E* penMaster)
{
    DISP_S* pstDisp;

    // s1 check input parameters
    DispCheckDeviceState();

    DispCheckID(enDisp);
    DispCheckNullPointer(penMaster);

    // s2 get pointer
    DispGetPointerByID(enDisp, pstDisp);

    // s3 check if eIntf exists
    if (!pstDisp->bIsSlave)
    {
        return HI_FAILURE;
    }

    *penMaster = pstDisp->enAttachedDisp;

    return HI_SUCCESS;
}


HI_S32 DISP_GetDisplayInfo(HI_DRV_DISPLAY_E enDisp, HI_DISP_DISPLAY_INFO_S* pstInfo)
{
    DISP_S* pstDisp;
    DISP_INTF_OPERATION_S* pfOpt = DISP_HAL_GetOperationPtr();
    HI_BOOL bBtm;
    HI_U32 vcnt;

    //DISP_HAL_ENCFMT_PARAM_S stFmt;
    //HI_S32 nRet;

    DispCheckDeviceState();

    // s1 check input parameters
    DispCheckID(enDisp);
    DispCheckNullPointer(pstInfo);

    // s2 check whether display opened
    DispShouldBeOpened(enDisp);

    // s3 get ENABLE state and return
    DispGetPointerByID(enDisp, pstDisp);

    if ( (pstDisp->stSetting.enFormat < HI_DRV_DISP_FMT_BUTT) && pfOpt )
    {
        *pstInfo = pstDisp->stDispInfo;
        DispCheckNullPointer(pfOpt->FP_GetChnBottomFlag);

        pfOpt->FP_GetChnBottomFlag(enDisp, &bBtm, &vcnt);

        pstInfo->bIsBottomField = bBtm;
        pstInfo->u32Vline = vcnt;
    }
    else
    {
        DISP_ERROR("Display %d info not available!\n", enDisp);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}


#ifdef HI_DISP_BUILD_FULL
HI_S32 DISP_RegCallback(HI_DRV_DISPLAY_E enDisp, HI_DRV_DISP_CALLBACK_TYPE_E eType,
                        HI_DRV_DISP_CALLBACK_S* pstCB)
{
    //    DISP_S *pstDisp;
    //    DISP_HAL_ENCFMT_PARAM_S stFmt;
    HI_S32 nRet;

    DispCheckDeviceState();

    // s1 check input parameters
    DispCheckID(enDisp);
    DispCheckNullPointer(pstCB);

    // s2 check whether display opened
    DispShouldBeOpened(enDisp);

    if (!pstCB->pfDISP_Callback)
    {
        DISP_ERROR("Callback function is null!\n");
        return HI_ERR_DISP_INVALID_PARA;
    }

    nRet = DISP_ISR_RegCallback(enDisp, eType, pstCB);

    return nRet;
}

HI_S32 DISP_UnRegCallback(HI_DRV_DISPLAY_E enDisp, HI_DRV_DISP_CALLBACK_TYPE_E eType,
                          HI_DRV_DISP_CALLBACK_S* pstCB)
{
    //    DISP_S *pstDisp;
    //    DISP_HAL_ENCFMT_PARAM_S stFmt;
    HI_S32 nRet;

    DispCheckDeviceState();

    // s1 check input parameters
    DispCheckID(enDisp);
    DispCheckNullPointer(pstCB);

    // s2 check whether display opened
    //DispShouldBeOpened(enDisp);

    if (!pstCB->pfDISP_Callback)
    {
        DISP_ERROR("Callback function is null!\n");
        return HI_ERR_DISP_INVALID_PARA;
    }

    nRet = DISP_ISR_UnRegCallback(enDisp, eType, pstCB);

    return nRet;
}

HI_VOID DISP_GetCastInfor(DISP_CAST_S *pstCast_info, DISP_Cast_PROC_INFO_S *pstCastProc)
{
    HI_U32 i = 0;

    if (pstCast_info)
    {
        pstCastProc->u32CastEmptyBufferNum = 0;
        pstCastProc->u32CastFullBufferNum = 0;
        pstCastProc->u32CastWriteBufferNum = 0;
        pstCastProc->u32CastUsedBufferNum  = 0;
        
        for (i = 0; i < pstCast_info->stBP.u32BufNum; i++)
        {
            switch (pstCast_info->stBP.pstBufQueue[i].enState)
            {
                case BUF_STATE_EMPTY:
                    pstCastProc->u32CastEmptyBufferNum ++;
                    break;                    
                case BUF_STATE_FULL:
                    pstCastProc->u32CastFullBufferNum ++;
                    break;
                case BUF_STATE_WRITING:        
                    pstCastProc->u32CastWriteBufferNum ++;
                    break; 
                case BUF_STATE_READING:
                case BUF_STATE_DONE:
                    pstCastProc->u32CastUsedBufferNum ++;
                    break;
                default:
                    break;
            }            
        }

        pstCastProc->bEnable = pstCast_info->bEnable;
        pstCastProc->bLowDelay = pstCast_info->bLowDelay;
        pstCastProc->bUserAllocate = pstCast_info->stConfig.bUserAlloc;
        pstCastProc->u32OutResolutionWidth  = pstCast_info->stAttr.stOut.s32Width;
        pstCastProc->u32OutResolutionHeight = pstCast_info->stAttr.stOut.s32Height;
        pstCastProc->u32CastOutFrameRate = pstCast_info->stDispInfo.u32RefreshRate;
        pstCastProc->u32TotalBufNum = pstCast_info->stBP.u32BufNum;
        pstCastProc->u32BufSize  = pstCast_info->stBP.u32BufSize;
        pstCastProc->u32BufStride = pstCast_info->stBP.u32BufStride;
            
        pstCastProc->u32CastAcquireTryCnt = pstCast_info->u32CastAcquireTryCnt;
        pstCastProc->u32CastAcquireOkCnt = pstCast_info->u32CastAcquireOkCnt;
        pstCastProc->u32CastReleaseTryCnt = pstCast_info->u32CastReleaseTryCnt;
        pstCastProc->u32CastReleaseOkCnt = pstCast_info->u32CastReleaseOkCnt;        
        
        for (i = 0; i < pstCastProc->u32TotalBufNum; i++)
        {               
            pstCastProc->enState[i] = (HI_U32)pstCast_info->stBP.pstBufQueue[i].enState;
            pstCastProc->u32FrameIndex[i] = pstCast_info->stBP.pstBufQueue[i].stFrame.u32FrameIndex;
        }        
    }

    return;
    
}


HI_S32 DISP_GetProcInto(HI_DRV_DISPLAY_E enDisp, DISP_PROC_INFO_S* pstInfo)
{
    DISP_S* pstDisp;
    HI_S32 i;

    DispCheckDeviceState();    
    DispCheckID(enDisp);
    DispCheckNullPointer(pstInfo);
    DispShouldBeOpened(enDisp);
    DispGetPointerByID(enDisp, pstDisp);
    
    /*the channel such as display0,or 1 enable or not.*/
    pstInfo->bEnable         = pstDisp->bEnable;
    pstInfo->eFmt            = pstDisp->stSetting.enFormat;
    pstInfo->eDispMode       = pstDisp->stSetting.eDispMode;
    pstInfo->bRightEyeFirst  = pstDisp->stSetting.bRightEyeFirst;
    pstInfo->stVirtaulScreen = pstDisp->stSetting.stVirtaulScreen;
    pstInfo->stOffsetInfo    = pstDisp->stSetting.stOffsetInfo;

    /*get the aspect setting.*/
    pstInfo->bCustAspectRatio = pstDisp->stSetting.bCustomRatio;
    pstInfo->u32AR_w       = pstDisp->stDispInfo.stAR.u8ARw;
    pstInfo->u32AR_h       = pstDisp->stDispInfo.stAR.u8ARh;
    
    /*get the csc space transfer.*/
    pstInfo->eDispColorSpace = pstDisp->stDispInfo.eColorSpace;

    /*get the display csc setting.*/
    pstInfo->u32Bright = pstDisp->stDispInfo.u32Bright;
    pstInfo->u32Hue = pstDisp->stDispInfo.u32Hue;
    pstInfo->u32Satur = pstDisp->stDispInfo.u32Satur;
    pstInfo->u32Contrst = pstDisp->stDispInfo.u32Contrst;

    /*get the background setting.*/
    pstInfo->stBgColor = pstDisp->stSetting.stBgColor; 
    
    /*get the zorder, which one is on the top or bottom.*/
    for (i = 0; i < HI_DRV_DISP_LAYER_BUTT; i++)
    {
        pstInfo->enLayer[i] = pstDisp->stSetting.enLayer[i];
    }

    /*get the master or slave role, and attached layer.*/
    pstInfo->bMaster = pstDisp->bIsMaster;
    pstInfo->bSlave  = pstDisp->bIsSlave;
    pstInfo->enAttachedDisp = pstDisp->enAttachedDisp;

    /*get the unflow times.*/
    pstInfo->u32Underflow = pstDisp->u32Underflow;

    /*FIXME: i don't know what does it means.*/
    pstInfo->u32StartTime = pstDisp->u32StartTime;    
    pstInfo->stTiming   = pstDisp->stSetting.stCustomTimg;        
    pstInfo->stColorSetting  = pstDisp->stSetting.stColor;

    /*get the cast information.*/
    pstInfo->pstCastInfor = pstDisp->Cast_ptr;

    DISP_GetCastInfor((DISP_CAST_S*)pstDisp->Cast_ptr, &pstInfo->stCastInfor);
    
    
    
    pstInfo->u32IntfNumber = 0;
    for (i = 0; i < HI_DRV_DISP_INTF_ID_MAX; i++)
    {
        if (pstDisp->stSetting.stIntf[i].bOpen)
        {
            pstInfo->stIntf[pstInfo->u32IntfNumber] = pstDisp->stSetting.stIntf[i].stIf;
            pstInfo->u32Link[pstInfo->u32IntfNumber] = pstDisp->stSetting.stIntf[i].eVencId;
            pstInfo->u32IntfNumber++;
        }
    }    

    return HI_SUCCESS;
}



#endif


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


