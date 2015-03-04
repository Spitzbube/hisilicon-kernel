
/******************************************************************************
  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.
******************************************************************************
File Name     : drv_disp_hal.c
Version       : Initial Draft
Author        : Hisilicon multimedia software group
Created       : 2012/12/30
Last Modified :
Description   :
Function List :
History       :
******************************************************************************/

#include "drv_disp_com.h"
#include "drv_disp_hal.h"
#include "drv_disp_osal.h"
#include "drv_disp_da.h"
#ifndef __DISP_PLATFORM_BOOT__
#include "drv_disp_ua.h"
#include "hi_drv_sys.h"
#endif

#include "drv_disp_hal_adp.h"
#include "hi_reg_common.h"

#ifdef __cplusplus
 #if __cplusplus
extern "C" {
 #endif
#endif /* __cplusplus */

/*==========================================
     VDP / SYS / VDAC phy-address
*/
static HI_U32 s_u32VdpBaseAddr  = 0;
/*==========================================
     HAL global parameters
*/
static HI_S32 s_DispIntfFlag = 0;
static DISP_CAPA_S s_stDispCapability[HI_DRV_DISPLAY_BUTT];
static DISP_CH_CFG_S s_stDispConfig[HI_DRV_DISPLAY_BUTT];

static HI_DRV_DISP_VERSION_S s_stVersion = {0};
static DISP_INTF_OPERATION_S s_stIntfOps = {0};

/*==========================================
    HAL module
*/
static DISP_LOCAL_INTF_S s_stHalIntf[HI_DRV_DISP_INTF_ID_MAX];
//static DISP_LOCAL_VDAC_S s_stHalVEnc[DISP_VENC_MAX];
static DISP_LOCAL_VDAC_S s_stHalVDac[HI_DISP_VDAC_MAX_NUMBER];
static DISP_LOCAL_WBC_S  s_stWBC[DISP_WBC_BUTT];

#define DISP_CLOCK_SOURCE_SD0  0
#define DISP_CLOCK_SOURCE_HD0  1
#define DISP_CLOCK_SOURCE_HD1  2
DISP_MMZ_BUF_S   g_RegBackBuf;

#ifndef __DISP_PLATFORM_BOOT__
static HI_S32 HAL_WbcZmeFunc(DISP_WBC_E eWbc,
                            ALG_VZME_DRV_PARA_S *stZmeI,
                            ALG_VZME_RTL_PARA_S *stZmeO)
{
    return Chip_Specific_WbcZmeFunc(eWbc, stZmeI, stZmeO);
}
#endif

static HI_S32 HAL_Set3DMode(HI_DRV_DISPLAY_E eChn,HI_U32 u32DispId, DISP_FMT_CFG_S *pstCfg)
{
    return Chip_Specific_Set3DMode(eChn, u32DispId, pstCfg);    
}

HI_S32 DispHalGetEnFmtIndex(HI_DRV_DISP_FMT_E eFmt)
{
    switch (eFmt)
    {
        case HI_DRV_DISP_FMT_1080P_60:
            return 1;
        case HI_DRV_DISP_FMT_1080P_50:
            return 2;
        case HI_DRV_DISP_FMT_1080P_30:
            return 3;
        case HI_DRV_DISP_FMT_1080P_25:
            return 4;
        case HI_DRV_DISP_FMT_1080P_24:
            return 5;
        case HI_DRV_DISP_FMT_1080i_60:
            return 6;
        case HI_DRV_DISP_FMT_1080i_50:
            return 7;
        case HI_DRV_DISP_FMT_720P_60:
            return 8;
        case HI_DRV_DISP_FMT_720P_50:
            return 9;
        case HI_DRV_DISP_FMT_576P_50:
            return 10;
        case HI_DRV_DISP_FMT_480P_60:
            return 11;
        case HI_DRV_DISP_FMT_PAL:
        case HI_DRV_DISP_FMT_PAL_B:
        case HI_DRV_DISP_FMT_PAL_B1:
        case HI_DRV_DISP_FMT_PAL_D:
        case HI_DRV_DISP_FMT_PAL_D1:
        case HI_DRV_DISP_FMT_PAL_G:
        case HI_DRV_DISP_FMT_PAL_H:
        case HI_DRV_DISP_FMT_PAL_K:
        case HI_DRV_DISP_FMT_PAL_I:
        case HI_DRV_DISP_FMT_PAL_N:
        case HI_DRV_DISP_FMT_PAL_Nc:
        case HI_DRV_DISP_FMT_SECAM_SIN:
        case HI_DRV_DISP_FMT_SECAM_COS:
        case HI_DRV_DISP_FMT_SECAM_L:
        case HI_DRV_DISP_FMT_SECAM_B:
        case HI_DRV_DISP_FMT_SECAM_G:
        case HI_DRV_DISP_FMT_SECAM_D:
        case HI_DRV_DISP_FMT_SECAM_K:
        case HI_DRV_DISP_FMT_SECAM_H:
            return 12;


        case HI_DRV_DISP_FMT_PAL_M:
        case HI_DRV_DISP_FMT_PAL_60:
        case HI_DRV_DISP_FMT_NTSC:
        case HI_DRV_DISP_FMT_NTSC_J:
        case HI_DRV_DISP_FMT_NTSC_443:
            return 13;
        case HI_DRV_DISP_FMT_861D_640X480_60:
        case HI_DRV_DISP_FMT_VESA_800X600_60:
        case HI_DRV_DISP_FMT_VESA_1024X768_60:
        case HI_DRV_DISP_FMT_VESA_1280X720_60:
        case HI_DRV_DISP_FMT_VESA_1280X800_60:
        case HI_DRV_DISP_FMT_VESA_1280X1024_60:
        case HI_DRV_DISP_FMT_VESA_1360X768_60:
        case HI_DRV_DISP_FMT_VESA_1366X768_60:
        case HI_DRV_DISP_FMT_VESA_1400X1050_60:
        case HI_DRV_DISP_FMT_VESA_1440X900_60:
        case HI_DRV_DISP_FMT_VESA_1440X900_60_RB:
        case HI_DRV_DISP_FMT_VESA_1600X900_60_RB:
        case HI_DRV_DISP_FMT_VESA_1600X1200_60:
        case HI_DRV_DISP_FMT_VESA_1680X1050_60: /*27*/
        case HI_DRV_DISP_FMT_VESA_1680X1050_60_RB:      /*28*/
        case HI_DRV_DISP_FMT_VESA_1920X1080_60: /*29*/
        case HI_DRV_DISP_FMT_VESA_1920X1200_60: /*30*/
        case HI_DRV_DISP_FMT_VESA_1920X1440_60:  /*31*/
        case HI_DRV_DISP_FMT_VESA_2048X1152_60:/*32*/
        case HI_DRV_DISP_FMT_VESA_2560X1440_60_RB:/*33*/
        case HI_DRV_DISP_FMT_VESA_2560X1600_60_RB:/*34*/
            return (14 + eFmt - HI_DRV_DISP_FMT_861D_640X480_60);

        case HI_DRV_DISP_FMT_1080P_24_FP:
            return 36;
        case HI_DRV_DISP_FMT_720P_60_FP:
            return 37;
        case HI_DRV_DISP_FMT_720P_50_FP:
            return 38;
        case HI_DRV_DISP_FMT_1440x576i_50:
            return 39;
        case HI_DRV_DISP_FMT_1440x480i_60:
            return 40;
        default :
            return 0;
    }
    
    return 0;
}


/********************************************/
/* usual */
HI_S32 DISP_HAL_GetFmtAspectRatio(HI_DRV_DISP_FMT_E eFmt, HI_U32 *pH, HI_U32 *pV)
{
    HI_S32 index;
    DISP_FMT_CFG_S stDispFormatParam;

    index = DispHalGetEnFmtIndex(eFmt);
    if  ( HI_SUCCESS != Get_FormatCfgPara(index ,&stDispFormatParam))
        return HI_FAILURE;
    
    *pH = stDispFormatParam.stInfo.stAR.u8ARw;
    *pV = stDispFormatParam.stInfo.stAR.u8ARh;

    return HI_SUCCESS;
}

HI_S32 DISP_HAL_GetFmtColorSpace(HI_DRV_DISP_FMT_E eFmt, HI_DRV_COLOR_SPACE_E  *penColorSpace)
{
    HI_S32 index;
    DISP_FMT_CFG_S stDispFormatParam;

    index = DispHalGetEnFmtIndex(eFmt);
    if  ( HI_SUCCESS != Get_FormatCfgPara(index ,&stDispFormatParam))
        return HI_FAILURE;

    *penColorSpace = stDispFormatParam.stInfo.enColorSpace;

    return HI_SUCCESS;
}

HI_S32 DISP_HAL_GetEncFmtPara(HI_DRV_DISP_FMT_E eFmt, DISP_HAL_ENCFMT_PARAM_S *pstFmtPara)
{
    HI_S32 index;
    DISP_FMT_CFG_S stDispFormatParam;

    index = DispHalGetEnFmtIndex(eFmt);
    if  ( HI_SUCCESS != Get_FormatCfgPara(index ,&stDispFormatParam))
        return HI_FAILURE;
    
    *pstFmtPara = stDispFormatParam.stInfo;
    return HI_SUCCESS;
}

VDP_CBM_MIX_E DISP_HAL_GetMixID(HI_DRV_DISPLAY_E eChn)
{
    switch(eChn)
    {
        case HI_DRV_DISPLAY_0:
            return VDP_CBM_MIX1;
        case HI_DRV_DISPLAY_1:
            return VDP_CBM_MIX0;
        default :
            return VDP_CBM_MIX0;
    }
}

HI_U32 DISP_HAL_GetChID(HI_DRV_DISPLAY_E eChn)
{
    switch(eChn)
    {
        case HI_DRV_DISPLAY_0:
            return 1;
        case HI_DRV_DISPLAY_1:
            return 0;
        default :
            return 0;
    }
}


/*
HI_S32 DISP_HAL_GetPll(HI_DRV_DISP_FMT_E eFmt, HI_U32 *pLow, HI_U32 *pHigh)
{
    *pLow  = s_u32PllConfig[DispHalGetEnFmtIndex(eFmt)][0];
    *pHigh = s_u32PllConfig[DispHalGetEnFmtIndex(eFmt)][1];
    return HI_SUCCESS;
}
*/


/********************************************/
HI_VOID DISP_HAL_ResetDispState(HI_VOID);
HI_VOID DISP_HAL_ResetIntfState(HI_VOID);
HI_VOID DISP_HAL_ResetVDacState(HI_VOID);
HI_VOID InitWbcState(HI_VOID);

HI_S32 DISP_HAL_InitVDPState(HI_VOID)
{
    /*set the virtual addr of vdp base register.*/
    VDP_DRIVER_SetVirtualAddr(s_u32VdpBaseAddr);    

    /*reset the disp/interface/vdac status.*/
    DISP_HAL_ResetDispState();
    DISP_HAL_ResetIntfState();    
    DISP_HAL_ResetVDacState();
    
    InitWbcState();    
    return HI_SUCCESS;
}

HI_S32 PF_ResetVdpHardware(HI_VOID)
{
#if 0
    HI_U32 v;
    
    // init sysreg
    v = g_pstRegCrg->PERI_CRG54.u32;
    
    g_pstRegCrg->PERI_CRG54.u32 = 0;
    g_pstRegCrg->PERI_CRG54.u32 = DISP_CV200_ES_SYSCTRL_RESET_VALUE | DISP_CV200_ES_SYSCTRL_RESET_BIT; 

    DISP_MSLEEP(5);
    
    g_pstRegCrg->PERI_CRG54.u32 = DISP_CV200_ES_SYSCTRL_RESET_VALUE; 
#else
    /*cv200es cv200 are differentf, so there should be 2 branches.
      This is for cv200es*/
    U_PERI_CRG54 unTmpValue;

    /*open and set clock  reset */
    unTmpValue.u32 = g_pstRegCrg->PERI_CRG54.u32;

    //#define DISP_CV200_ES_SYSCTRL_RESET_VALUE 0x05F147FFUL
    //unTmpValue.u32 |= DISP_CV200_ES_SYSCTRL_RESET_VALUE; /*open and set clock*/
    DISP_ResetCRG54(&unTmpValue.u32);

    g_pstRegCrg->PERI_CRG54.u32 = unTmpValue.u32;

    DISP_MSLEEP(5);

    /*cancel reset*/
    unTmpValue.bits.vou_srst_req = 0;
    g_pstRegCrg->PERI_CRG54.u32 = unTmpValue.u32;
#endif

    // init vdp
    VDP_DRIVER_Initial();

    // init vdac
    //VDAC_DRIVER_Initial();
    VDP_VDAC_Reset();

    return HI_SUCCESS;
}

HI_S32 PF_CloseClkResetModule(HI_VOID)
{
    VDP_CloseClkResetModule();
    return HI_SUCCESS;
}

/********************************************/
/* Display config */
#define DISP_HAL_FUNCTION_START_HAERE
HI_VOID DISP_HAL_ResetDispState(HI_VOID)
{
    memset(&s_stDispCapability[0], 0, sizeof(DISP_CAPA_S) * HI_DRV_DISPLAY_BUTT);

    s_stDispCapability[HI_DRV_DISPLAY_0].bSupport = HI_TRUE;
    s_stDispCapability[HI_DRV_DISPLAY_0].bHD      = HI_TRUE;
    s_stDispCapability[HI_DRV_DISPLAY_0].bWbc    = HI_FALSE;

    s_stDispCapability[HI_DRV_DISPLAY_1].bSupport = HI_TRUE;
    s_stDispCapability[HI_DRV_DISPLAY_1].bHD      = HI_TRUE;
    s_stDispCapability[HI_DRV_DISPLAY_1].bWbc     = HI_TRUE;

    s_stDispCapability[HI_DRV_DISPLAY_2].bSupport = HI_FALSE;


    memset(&s_stDispConfig[0], 0, sizeof(DISP_CH_CFG_S) * HI_DRV_DISPLAY_BUTT);


    return;
}

HI_BOOL PF_TestChnSupport(HI_DRV_DISPLAY_E eChn)
{

    return s_stDispCapability[eChn].bSupport;
}


HI_BOOL PF_TestChnSupportHD(HI_DRV_DISPLAY_E eChn)
{

    return s_stDispCapability[eChn].bHD;
}

HI_BOOL PF_TestChnSupportCast(HI_DRV_DISPLAY_E eChn)
{

    return s_stDispCapability[eChn].bWbc;
}

HI_BOOL PF_TestIntfSupport(HI_DRV_DISPLAY_E eChn, HI_DRV_DISP_INTF_ID_E eIntf)
{
    if (HI_DRV_DISPLAY_0 == eChn)
    {
        if (  (HI_DRV_DISP_INTF_SVIDEO0 == eIntf)
            ||(HI_DRV_DISP_INTF_CVBS0   == eIntf)

            ||(HI_DRV_DISP_INTF_HDMI0  == eIntf)
            ||(HI_DRV_DISP_INTF_RGB0   == eIntf)
            ||(HI_DRV_DISP_INTF_VGA0    == eIntf)
            ||(HI_DRV_DISP_INTF_YPBPR0 == eIntf)
            )
        {
            return HI_TRUE;
        }
    }

    if (HI_DRV_DISPLAY_1 == eChn)
    {
        if (  (HI_DRV_DISP_INTF_YPBPR0 == eIntf)
            ||(HI_DRV_DISP_INTF_HDMI0  == eIntf)
            ||(HI_DRV_DISP_INTF_VGA0   == eIntf)
            ||(HI_DRV_DISP_INTF_RGB0   == eIntf)
            ||(HI_DRV_DISP_INTF_LCD0   == eIntf)
            )
        {
            return HI_TRUE;
        }
    }

    return HI_FALSE;
}

HI_BOOL PF_TestChnEncFmt(HI_DRV_DISPLAY_E eChn, HI_DRV_DISP_FMT_E eFmt)
{
    if(  (eFmt <= HI_DRV_DISP_FMT_CUSTOM))
    {
        return HI_TRUE;
    }

    return HI_FALSE;
   
}

HI_BOOL PF_TestChnAttach(HI_DRV_DISPLAY_E enM, HI_DRV_DISPLAY_E enS)
{
    if( (enM != HI_DRV_DISPLAY_1) || (enS != HI_DRV_DISPLAY_0))
    {
        return HI_FALSE;
    }

    return HI_TRUE;
}


#define DISP_HPLL_BYPASS            0x04000000ul
#define DISP_HPLL_DSM_PD            0x02000000ul
#define DISP_HPLL_VDAC_PD           0x01000000ul
#define DISP_HPLL_FOUT_POST_DIV_PD  0x00800000ul
#define DISP_HPLL_FOUT_4_PHASE_PD   0x00400000ul
#define DISP_HPLL_FOUT_VCO_PD       0x00200000ul
#define DISP_HPLL_POWERDOWN         0x00100000ul

#define DISP_HPLL_0_MASK            0xFFFFFFFFul
#define DISP_HPLL_1_MASK            0x0003FFFFul

#if 0
HI_S32 DISP_HAL_SetPll2(HI_DRV_DISP_FMT_E eFmt)
{
    HI_U32 uLow, uHigh;
    
    uLow  = s_stDispFormatParam[DispHalGetEnFmtIndex(eFmt)].u32Pll[0] & DISP_HPLL_0_MASK;
    uHigh = s_stDispFormatParam[DispHalGetEnFmtIndex(eFmt)].u32Pll[1] & DISP_HPLL_1_MASK;

    // bypass and power down
    s_pu32VdpPll0RegAddr[1] = s_pu32VdpPll0RegAddr[1] & (~DISP_HPLL_BYPASS);
    s_pu32VdpPll0RegAddr[1] = s_pu32VdpPll0RegAddr[1] | DISP_HPLL_FOUT_VCO_PD;
    s_pu32VdpPll0RegAddr[1] = s_pu32VdpPll0RegAddr[1] | DISP_HPLL_FOUT_POST_DIV_PD;
    s_pu32VdpPll0RegAddr[1] = s_pu32VdpPll0RegAddr[1] | DISP_HPLL_FOUT_4_PHASE_PD;

    // config div para
    s_pu32VdpPll0RegAddr[0] = s_pu32VdpPll0RegAddr[0] & (~DISP_HPLL_0_MASK);
    s_pu32VdpPll0RegAddr[0] = s_pu32VdpPll0RegAddr[0] | uLow;
    s_pu32VdpPll0RegAddr[1] = s_pu32VdpPll0RegAddr[1] & (~DISP_HPLL_1_MASK);
    s_pu32VdpPll0RegAddr[1] = s_pu32VdpPll0RegAddr[1] | uHigh;

    // power up
    s_pu32VdpPll0RegAddr[1] = s_pu32VdpPll0RegAddr[1] & (~DISP_HPLL_FOUT_VCO_PD);
    s_pu32VdpPll0RegAddr[1] = s_pu32VdpPll0RegAddr[1] & (~DISP_HPLL_FOUT_POST_DIV_PD);
    s_pu32VdpPll0RegAddr[1] = s_pu32VdpPll0RegAddr[1] & (~DISP_HPLL_FOUT_4_PHASE_PD);

    return HI_SUCCESS;
}
#endif

HI_S32 PF_ResetChn(HI_U32 u32DispId, HI_U32 bIntMode)
{
    VDP_DISP_CLIP_S stClipData;
    //HI_U32 u32DispId = DISP_HAL_GetChID(eChn);

    // s1 set dhd
    VDP_DHD_Reset(u32DispId);

    VDP_DISP_SetVtThdMode(u32DispId, 1, bIntMode);
    VDP_DISP_SetVtThdMode(u32DispId, 2, bIntMode);
    VDP_DISP_SetVtThdMode(u32DispId, 3, bIntMode);

    VDP_DISP_SetCscEnable(u32DispId, 0);

    // s2 set clip

    stClipData.bClipEn = 1;
    stClipData.u32ClipLow_y  = 0;
    stClipData.u32ClipLow_cb = 0;
    stClipData.u32ClipLow_cr = 0;

    stClipData.u32ClipHigh_y  = 1023;  
    stClipData.u32ClipHigh_cb = 1023;
    stClipData.u32ClipHigh_cr = 1023;

    VDP_DISP_SetClipCoef(u32DispId, VDP_DISP_INTF_LCD, stClipData);
    VDP_DISP_SetClipCoef(u32DispId, VDP_DISP_INTF_BT1120, stClipData);
    VDP_DISP_SetClipCoef(u32DispId, VDP_DISP_INTF_HDMI, stClipData);
    VDP_DISP_SetClipCoef(u32DispId, VDP_DISP_INTF_VGA, stClipData);

    stClipData.u32ClipLow_y  = 64;
    stClipData.u32ClipLow_cb = 64;
    stClipData.u32ClipLow_cr = 64;
    VDP_DISP_SetClipCoef(u32DispId, VDP_DISP_INTF_DATE, stClipData);

    VDP_DISP_SetRegUp(u32DispId);

    return HI_SUCCESS;
}

HI_S32 PF_ConfigChn(HI_DRV_DISPLAY_E eChn, DISP_CH_CFG_S *pstCfg)
{
    //HI_U32 u32DispId = DISP_HAL_GetChID(eChn);

    return HI_SUCCESS;
}


#define DISP_VTTHD_VIDEO_OFFSET 0
#define DISP_VTTHD_GFX_OFFSET 80
#define DISP_VTTHD_DISP0_TO_DISP1 32

HI_S32 PF_SetChnBaseTiming(HI_DRV_DISPLAY_E eChn, DISP_FMT_CFG_S *pstCfg)
{
    HI_S32 thd;
    VDP_DISP_RECT_S DispRect;
    VDP_LAYER_VP_E vdp_layer;
    HI_U32 u32DispId = DISP_HAL_GetChID(eChn);
   
    DISP_PRINT(">>>>>>>>>>>>>>>u32DispId=%d>>> bact=%d\n",u32DispId, pstCfg->stTiming.u32Bvact);    

    if(pstCfg->stInfo.eDispMode == DISP_STEREO_FPK)
        PF_ResetChn(u32DispId, DHD_VTXTHD_FRAME_MODE);
    else
        PF_ResetChn(u32DispId, DHD_VTXTHD_FIELD_MODE);    

    (HI_VOID)HAL_Set3DMode(eChn, u32DispId, pstCfg);
    // set dhd
    /*
      timing is like that:

      ||<--------- FRRAME 0--------->||<--------- FRRAME 1--------->||
      -----------------------------------------------------------------
      || VBB0 |     VACT0     | VFB0 || VBB01 |     VACT1    | VFB1 ||  
      -----------------------------------------------------------------
                              ^
                             /|\
                              |
                NOTICE: The 'thd' is start here, NOT between VFB0 and VBB1.
                        That means here thd is '0'.
    */
    /*
      ||<--------- FRRAME 0--------->||<--------- FRRAME 1--------->||
      -----------------------------------------------------------------
      || VBB0 |     VACT0     | VFB0 || VBB01 |     VACT1    | VFB1 ||  
      -----------------------------------------------------------------
                              |---------------|<----->|
                                                  ^   Pos1
                                                 /|\
                                                  |
                                         Here equal to 'DISP_VTTHD_VIDEO_OFFSET'
                                         And Pos1 is thd1
    */
    VDP_DISP_SetTiming(u32DispId, &(pstCfg->stTiming));
    thd = pstCfg->stTiming.u32Vfb + pstCfg->stTiming.u32Vbb + DISP_VTTHD_VIDEO_OFFSET;

    if (u32DispId == 1)
    {
        thd = thd + DISP_VTTHD_DISP0_TO_DISP1;
    }
    VDP_DISP_SetVtThd(u32DispId, 1, (HI_U32)thd);

    /*
      ||<--------- FRRAME 0--------->||<--------- FRRAME 1--------->||
      -----------------------------------------------------------------
      || VBB0 |     VACT0     | VFB0 || VBB01 |     VACT1    | VFB1 ||  
      -----------------------------------------------------------------
                                |<----------->|
                               Pos2    ^   
                                      /|\
                                       |
                                 Here equal to 'DISP_VTTHD_GFX_OFFSET'
                                 Pos2 is thd2. 
                                 If DISP_VTTHD_GFX_OFFSET is bigger tha (VFB0+VBB1),
                                 Pos2 is in VACT0.
    */

    thd = pstCfg->stTiming.u32Vbb + pstCfg->stTiming.u32Vfb - DISP_VTTHD_GFX_OFFSET;
    if (thd < 0)
    {
        /* Pos2 is in VACT0 */
        thd = thd + pstCfg->stTiming.u32Vbb + pstCfg->stTiming.u32Vfb +  pstCfg->stTiming.u32Vact;
    }

    if (u32DispId == 1)
    {
        thd = thd + DISP_VTTHD_DISP0_TO_DISP1;
    }

    VDP_DISP_SetVtThd(u32DispId, 2, (HI_U32)thd);

    // set vp
    DISP_MEMSET(&DispRect, 0, sizeof(VDP_DISP_RECT_S));
    DispRect.u32DXL  = pstCfg->stInfo.stOrgRect.s32Width;
    DispRect.u32DYL  = pstCfg->stInfo.stOrgRect.s32Height;
    DispRect.u32IWth = pstCfg->stInfo.stOrgRect.s32Width;
    DispRect.u32IHgt = pstCfg->stInfo.stOrgRect.s32Height;
    DispRect.u32OWth = pstCfg->stInfo.stOrgRect.s32Width;
    DispRect.u32OHgt = pstCfg->stInfo.stOrgRect.s32Height;
    DispRect.u32VXL  = pstCfg->stInfo.stOrgRect.s32Width;
    DispRect.u32VYL  = pstCfg->stInfo.stOrgRect.s32Height;

    Chip_Specific_DispSetChanClk(eChn,pstCfg);

    vdp_layer = (HI_DRV_DISPLAY_1 == eChn) ? VDP_LAYER_VP0 : VDP_LAYER_VP1;
    VDP_VP_SetLayerReso(vdp_layer, DispRect);
    VDP_VP_SetRegUp(vdp_layer);

    VDP_DISP_SetRegUp(u32DispId);

    return HI_SUCCESS;
}


HI_S32 PF_SetChnFmt(HI_DRV_DISPLAY_E eChn, HI_DRV_DISP_FMT_E eFmt, HI_DRV_DISP_STEREO_E enStereo)
{
    DISP_FMT_CFG_S stCfg;
    HI_S32  Ret;
    HI_S32 index;
    
       index = DispHalGetEnFmtIndex(eFmt);
       if  ( HI_SUCCESS != Get_FormatCfgPara(index ,&stCfg))
           return HI_FAILURE;

    /*select clk source*/
    if  ((stCfg.stInfo.eFmt >= HI_DRV_DISP_FMT_576P_50 )&& (stCfg.stInfo.eFmt <= HI_DRV_DISP_FMT_1440x480i_60 ))
        stCfg.enPllIndex = DISP_CLOCK_SOURCE_SD0;
    else
        stCfg.enPllIndex = DISP_CLOCK_SOURCE_HD0;
    
    stCfg.stInfo.eDispMode = enStereo;
    
    Ret = PF_SetChnBaseTiming(eChn, &stCfg);
    return Ret;
}

HI_S32 PF_SetChnTiming(HI_DRV_DISPLAY_E eChn, HI_DRV_DISP_TIMING_S *pstTiming)
{
    DISP_FMT_CFG_S stCfg;
    HI_S32 Ret;

    //pstTiming
    DISP_MEMSET(&stCfg, 0, sizeof(DISP_FMT_CFG_S));

    /*make Reg para */
    if (( pstTiming->u32VertFreq  > 12000) || ( pstTiming->u32VertFreq  < 2000))
    {
        return HI_FAILURE;
    }
    
    /*select clk source*/
    if  ((stCfg.stInfo.eFmt >= HI_DRV_DISP_FMT_576P_50 )&& (stCfg.stInfo.eFmt <= HI_DRV_DISP_FMT_1440x480i_60 ))
        stCfg.enPllIndex = DISP_CLOCK_SOURCE_SD0;
    else
        stCfg.enPllIndex = DISP_CLOCK_SOURCE_HD0;
    
    stCfg.stInfo.eFmt = HI_DRV_DISP_FMT_CUSTOM;
    stCfg.stInfo.eDispMode = DISP_STEREO_NONE;

    stCfg.stInfo.u32RefreshRate = pstTiming->u32VertFreq;

    stCfg.stInfo.stOrgRect.s32X = 0;
    stCfg.stInfo.stOrgRect.s32Y = 0;
    stCfg.stInfo.stOrgRect.s32Width = pstTiming->u32HACT;
    stCfg.stInfo.stOrgRect.s32Height = pstTiming->u32VACT;
        
    stCfg.stTiming.bSynm = 1 ;
    stCfg.stTiming.bIop = 1 ;
    stCfg.stTiming.u32Intfb = 2 ;

    stCfg.stTiming.u32Vact = pstTiming->u32VACT;
    stCfg.stTiming.u32Vbb = pstTiming->u32VBB ;
    stCfg.stTiming.u32Vfb = pstTiming->u32VFB ;

    stCfg.stTiming.u32Bvact = 1;
    stCfg.stTiming.u32Bvbb = 1 ;
    stCfg.stTiming.u32Bvfb = 1 ;

    stCfg.stTiming.u32Hact = pstTiming->u32HACT ;
    if (eChn == s_stHalIntf[HI_DRV_DISP_INTF_VGA0].enChan)
    {
        /*if VGA interface ,need date is later than sync ,so need to adjust :*/
        stCfg.stTiming.u32Hbb = pstTiming->u32HBB -7 ;
        stCfg.stTiming.u32Hfb = pstTiming->u32HFB + 7 ;
    }
    else
    {
        stCfg.stTiming.u32Hbb = pstTiming->u32HBB ;
        stCfg.stTiming.u32Hfb = pstTiming->u32HFB ;
    }


    stCfg.stTiming.u32Hpw = pstTiming->u32HPW ;
    stCfg.stTiming.u32Vpw = pstTiming->u32VPW ;

    stCfg.stTiming.u32Hmid = 1;

    stCfg.stTiming.bIdv = pstTiming->bIDV ;
    stCfg.stTiming.bIhs = pstTiming->bIHS;
    stCfg.stTiming.bIvs = pstTiming->bIVS;

    stCfg.u32Pll[0] = pstTiming->u32ClkPara0;
    stCfg.u32Pll[1] = pstTiming->u32ClkPara1;

    Ret = PF_SetChnBaseTiming(eChn, &stCfg);
    return Ret;
}

HI_S32 PF_SetChnPixFmt(HI_DRV_DISPLAY_E eChn, HI_DRV_PIX_FORMAT_E ePix)
{
//    HI_U32 u32DispId = DISP_HAL_GetChID(eChn);
    return HI_SUCCESS;
}

HI_S32 PF_SetChnBgColor(HI_DRV_DISPLAY_E eChn, HI_DRV_COLOR_SPACE_E enCS, HI_DRV_DISP_COLOR_S *pstBGC)
{
    VDP_BKG_S stBkg;
    ALG_COLOR_S stAlgC;
    DISP_DA_FUNCTION_S *pstDA;

    pstDA = DISP_DA_GetFunction();
    if (!pstDA)
    {
        return HI_FAILURE;
    }

    stAlgC.u8Red   = pstBGC->u8Red;
    stAlgC.u8Green = pstBGC->u8Green;
    stAlgC.u8Blue  = pstBGC->u8Blue;

    pstDA->PFCscRgb2Yuv(&stAlgC, &stAlgC);

    // TODO:
    stBkg.bBkType = 0;
    stBkg.u32BkgA = 0;
    stBkg.u32BkgY = ((HI_U32)stAlgC.u8Y)  << 2;
    stBkg.u32BkgU = ((HI_U32)stAlgC.u8Cb) << 2;
    stBkg.u32BkgV = ((HI_U32)stAlgC.u8Cr) << 2;
/*
    DISP_PRINT(">>>>>>>>SET BGC R=%d, G=%d, B=%d, Y=%d, PB=%d, PR=%d\n",
               stAlgC.u8Red, stAlgC.u8Green, stAlgC.u8Blue,
               stAlgC.u8Y, stAlgC.u8Cb, stAlgC.u8Cr);
*/
    if (HI_DRV_DISPLAY_1 == eChn)
    {
        VDP_CBM_SetMixerBkg(VDP_CBM_MIX0, stBkg);
        VDP_CBM_SetMixerBkg(VDP_CBM_MIXV0, stBkg);
    }
    else
    {
        VDP_CBM_SetMixerBkg(VDP_CBM_MIX1, stBkg);
    }

    return HI_SUCCESS;
}


HI_S32 PF_SetChnColor(HI_DRV_DISPLAY_E eChn, DISP_HAL_COLOR_S *pstColor)
{
    ALG_CSC_DRV_PARA_S stIn;
    ALG_CSC_RTL_PARA_S stOut;
    VDP_CSC_DC_COEF_S stCscCoef;
    VDP_CSC_COEF_S stCscCoef2;
    DISP_DA_FUNCTION_S *pstDA;
    HI_U32 u32DispId = DISP_HAL_GetChID(eChn);

    pstDA = DISP_DA_GetFunction();
    if (!pstDA)
    {
        return HI_FAILURE;
    }

    stIn.eInputCS  = pstColor->enInputCS;
    stIn.eOutputCS = pstColor->enOutputCS;
    stIn.bIsBGRIn = HI_FALSE;

    stIn.u32Bright  = pstColor->u32Bright;
    stIn.u32Contrst = pstColor->u32Contrst;
    stIn.u32Hue     = pstColor->u32Hue;
    stIn.u32Satur   = pstColor->u32Satur;
    stIn.u32Kr = pstColor->u32Kr;
    stIn.u32Kg = pstColor->u32Kg;
    stIn.u32Kb = pstColor->u32Kb;


    DISP_PRINT(">>>>>>>>PF_SetChnColor i=%d, o=%d, B=%d, C=%d, H=%d, S=%d,KR=%d,KG=%d, KB=%d\n",
               pstColor->enInputCS, pstColor->enOutputCS, 
               pstColor->u32Bright,
               pstColor->u32Contrst,
               pstColor->u32Hue,
               pstColor->u32Satur,
               pstColor->u32Kr,
               pstColor->u32Kg,
               pstColor->u32Kb);


    pstDA->pfCalcCscCoef(&stIn, &stOut);


    DISP_PRINT(">>>>>>>>PF_SetChnColor D1=%d, D2=%d, C00=%d, C11=%d, C22=%d\n",
               stOut.s32CscDcIn_1, stOut.s32CscDcIn_2, 
               stOut.s32CscCoef_00,
               stOut.s32CscCoef_11,
               stOut.s32CscCoef_22);


    stCscCoef.csc_in_dc0 = stOut.s32CscDcIn_0;
    stCscCoef.csc_in_dc1 = stOut.s32CscDcIn_1;
    stCscCoef.csc_in_dc2 = stOut.s32CscDcIn_2;

    stCscCoef.csc_out_dc0 = stOut.s32CscDcOut_0;
    stCscCoef.csc_out_dc1 = stOut.s32CscDcOut_1;
    stCscCoef.csc_out_dc2 = stOut.s32CscDcOut_2;
    VDP_DISP_SetCscDcCoef(u32DispId, stCscCoef);

    stCscCoef2.csc_coef00 = stOut.s32CscCoef_00;
    stCscCoef2.csc_coef01 = stOut.s32CscCoef_01;
    stCscCoef2.csc_coef02 = stOut.s32CscCoef_02;

    stCscCoef2.csc_coef10 = stOut.s32CscCoef_10;
    stCscCoef2.csc_coef11 = stOut.s32CscCoef_11;
    stCscCoef2.csc_coef12 = stOut.s32CscCoef_12;

    stCscCoef2.csc_coef20 = stOut.s32CscCoef_20;
    stCscCoef2.csc_coef21 = stOut.s32CscCoef_21;
    stCscCoef2.csc_coef22 = stOut.s32CscCoef_22;

    VDP_DISP_SetCscCoef(u32DispId, stCscCoef2);
    
    VDP_DISP_SetCscEnable(u32DispId, 1);

    VDP_DISP_SetRegUp(u32DispId);

    return HI_SUCCESS;
}

HI_S32 PF_SetDispSignal(HI_DRV_DISPLAY_E eChn, HI_U32 index, HI_DRV_DISP_VDAC_SIGNAL_E eSignal)
{
//    HI_U32 u32DispId = DISP_HAL_GetChID(eChn);
    
    return HI_SUCCESS;
}

HI_S32 PF_SetChnEnable(HI_DRV_DISPLAY_E eChn, HI_BOOL bEnalbe)
{
    HI_U32 u32DispId = DISP_HAL_GetChID(eChn);
    U_PERI_CRG54 unTmpValue, u32SaveValue;
#ifdef HI_DISP_BUILD_FULL
    HI_U32 t;
#endif

    VDP_DISP_SetIntfEnable(u32DispId, bEnalbe);

    VDP_DISP_SetRegUp(u32DispId);

#ifdef HI_DISP_BUILD_FULL
    HI_DRV_SYS_GetTimeStampMs(&t);
    //printk("disp=%d, t=%d, en=%d\n", u32DispId, t, bEnalbe);
#endif

    // switch sd clock divide parameters otherwis DISP0 maybe die.
    if ( (eChn == HI_DRV_DISPLAY_0) && bEnalbe )
    {
        u32SaveValue.u32 = g_pstRegCrg->PERI_CRG54.u32;

        unTmpValue.u32 = u32SaveValue.u32;

        /*set not div */
        unTmpValue.bits.vo_sd_clk_div = 3;
        g_pstRegCrg->PERI_CRG54.u32 = unTmpValue.u32;

        mdelay(5);

        /*restore value !*/
        g_pstRegCrg->PERI_CRG54.u32 = u32SaveValue.u32;
    }
    else if ( (eChn == HI_DRV_DISPLAY_1) && bEnalbe )
    {
        u32SaveValue.u32 = g_pstRegCrg->PERI_CRG54.u32;

        unTmpValue.u32 = u32SaveValue.u32;
        unTmpValue.bits.vo_hd_clk_div = 3;
        g_pstRegCrg->PERI_CRG54.u32 = unTmpValue.u32;
        
        mdelay(5);
        g_pstRegCrg->PERI_CRG54.u32 = u32SaveValue.u32;
    }


    return HI_SUCCESS;
}

HI_S32 PF_SetMSChnEnable(HI_DRV_DISPLAY_E eChnM, HI_DRV_DISPLAY_E eChnS, HI_U32 u32DelayMs, HI_BOOL bEnalbe)
{
#ifdef HI_DISP_BUILD_FULL
    HI_U32 t;
#endif
    HI_U32 u32DispIdM = DISP_HAL_GetChID(eChnM);
    HI_U32 u32DispIdS = DISP_HAL_GetChID(eChnS);
    Chip_Specific_DispSetMSChnEnable(u32DispIdM, u32DispIdS, u32DelayMs, bEnalbe);


#ifdef HI_DISP_BUILD_FULL
    HI_DRV_SYS_GetTimeStampMs(&t);
    //printk("dispM=%d, t=%d, en=%d\n", eChnM, t, bEnalbe);
#endif

    return HI_SUCCESS;
}


HI_S32 PF_GetChnEnable(HI_DRV_DISPLAY_E eChn, HI_BOOL *pbEnalbe)
{
    HI_U32 u32DispId = DISP_HAL_GetChID(eChn);
    HI_U32 bTrue;

    VDP_DISP_GetIntfEnable(u32DispId, &bTrue);

    *pbEnalbe = bTrue ? HI_TRUE : HI_FALSE;
    return HI_SUCCESS;
}









/********************************************/
/* capability */

/********************************************/
/* interrupt */
HI_S32 PF_SetIntEnable(HI_U32 u32Int, HI_BOOL bEnable)
{
    //printk("PF_SetIntEnable   int=%d, en=%d\n", u32Int, bEnable);
    if (bEnable)
    {
        VDP_DISP_SetIntMask(u32Int);
    }
    else
    {
        VDP_DISP_SetIntDisable(u32Int);
    }
    return HI_SUCCESS;
}

HI_S32 PF_GetIntSetting(HI_U32 *pu32IntSetting)
{
    VDP_DISP_GetIntMask(pu32IntSetting);
    return HI_SUCCESS;
}

HI_S32 PF_GetMaskedIntState(HI_U32 *pu32State)
{
    *pu32State = VDP_DISP_GetMaskIntSta((HI_U32)DISP_INTERRUPT_ALL);
    return HI_SUCCESS;
}

HI_S32 PF_GetUnmaskedIntState(HI_U32 *pu32State)
{

    *pu32State = VDP_DISP_GetIntSta((HI_U32)DISP_INTERRUPT_ALL);
    return HI_SUCCESS;
}


HI_S32 PF_CleanIntState(HI_U32 u32State)
{
    VDP_DISP_ClearIntSta(u32State);
    return HI_SUCCESS;
}

HI_U32 FP_GetChnIntState(HI_DRV_DISPLAY_E enDisp, HI_U32 u32IntState)
{
    switch (enDisp)
    {
        case HI_DRV_DISPLAY_1 : 

            return (HI_U32)(u32IntState & 0x0ful);

        case HI_DRV_DISPLAY_0 : 

            return (HI_U32)(u32IntState & 0xf0ul);

        default:
            return (HI_U32)0;
    }
}

HI_U32 FP_GetChnBottomFlag(HI_DRV_DISPLAY_E enDisp, HI_BOOL *pbBtm, HI_U32 *pu32Vcnt)
{
    HI_U32 u32DispId = DISP_HAL_GetChID(enDisp);
    
    if (pbBtm && pu32Vcnt)
    {
        VDP_DISP_GetVactState(u32DispId, pbBtm, pu32Vcnt);
    }

    return 0;
}






/********************************************/
/* venc manager */
#define DISP_HAL_VENC_FUNCTION_START_HAERE


VDP_DISP_INTF_E DISP_HAL_GetHalIntfIdForVenc(HI_DRV_DISPLAY_E enDisp,DISP_INTF_S *pstIf,HI_DRV_DISP_FMT_E eFmt)
{
    DISP_VENC_E enVenc = DISP_VENC_MAX;
    HI_BOOL bNeedVenc = HI_FALSE;
    VDP_DISP_INTF_E  enVDPInf = VDP_DISP_INTF_BUTT;


    switch (pstIf->stIf.eID)
    {
        case HI_DRV_DISP_INTF_YPBPR0:
        case HI_DRV_DISP_INTF_RGB0:
            /*(link to xDate   adjust by format !!)*/
            bNeedVenc = HI_TRUE;    
            if ((eFmt >= HI_DRV_DISP_FMT_PAL) && (eFmt <= HI_DRV_DISP_FMT_1440x480i_60) )
            //if ((eFmt >= HI_DRV_DISP_FMT_PAL) && (eFmt <= HI_DRV_DISP_FMT_SECAM_H) )
            {
                enVenc   = DISP_VENC_SDATE0;
                enVDPInf =   VDP_DISP_INTF_SDDATE;
            }
            else
            {
                enVenc   = DISP_VENC_HDATE0;
                enVDPInf =  VDP_DISP_INTF_HDDATE;
            }
            break;
        case HI_DRV_DISP_INTF_SVIDEO0:
        case HI_DRV_DISP_INTF_CVBS0:
            
            bNeedVenc = HI_TRUE;
            enVenc  = DISP_VENC_SDATE0;
            enVDPInf  =  VDP_DISP_INTF_SDDATE;
            break;
    case HI_DRV_DISP_INTF_VGA0:
        bNeedVenc = HI_FALSE;
        enVenc   = DISP_VENC_VGA0;
            enVDPInf  =  VDP_DISP_INTF_VGA;
            break;
        case HI_DRV_DISP_INTF_HDMI0:
        case HI_DRV_DISP_INTF_HDMI1:
        case HI_DRV_DISP_INTF_HDMI2:
            enVDPInf  =  VDP_DISP_INTF_HDMI;
            break;

        case HI_DRV_DISP_INTF_BT1120_0:
        case HI_DRV_DISP_INTF_BT1120_1:
        case HI_DRV_DISP_INTF_BT1120_2:
            enVDPInf  =  VDP_DISP_INTF_BT1120;
            break;
        case HI_DRV_DISP_INTF_LCD0:
        case HI_DRV_DISP_INTF_LCD1:
        case HI_DRV_DISP_INTF_LCD2:
            enVDPInf  =  VDP_DISP_INTF_LCD;
            break;
            
        case HI_DRV_DISP_INTF_BT656_0:
        case HI_DRV_DISP_INTF_BT656_1:
        case HI_DRV_DISP_INTF_BT656_2:
        default:
            enVDPInf  =  VDP_DISP_INTF_BUTT;
            break;
    }
    
    pstIf->bLinkVenc = bNeedVenc;
    pstIf->eVencId   = enVenc;
    return enVDPInf;
}

/********************************************/
/* VDAC */
#define DISP_HAL_VDAC_FUNCTION_START_HAERE
HI_VOID DISP_HAL_ResetVDacState(HI_VOID)
{
    HI_S32 i;

    DISP_MEMSET(&s_stHalVDac[0], 0, sizeof(DISP_LOCAL_VDAC_S)*HI_DISP_VDAC_MAX_NUMBER);

    for(i=0; i<HI_DISP_VDAC_MAX_NUMBER; i++)
    {
        s_stHalVDac[i].bSupport = HI_TRUE;
        s_stHalVDac[i].bIdle    = HI_TRUE;
    }
    
    return;
}

HI_S32 DISP_HAL_VDACIsIdle(HI_U32 uVdac)
{
    HI_U32 v0;

    v0 = uVdac & 0xff;
    
    if (v0 < HI_DISP_VDAC_MAX_NUMBER)
    {
        if (s_stHalVDac[v0].bSupport && s_stHalVDac[v0].bIdle)
        {
            return HI_TRUE;
        }
    }
    
    return HI_FALSE;
}

HI_S32 PF_AcquireVDAC(HI_U32 uVdac)
{
    HI_U32 v0;

    v0 = uVdac & 0xff;
    
    if (v0 < HI_DISP_VDAC_MAX_NUMBER)
    {
        if (s_stHalVDac[v0].bSupport && s_stHalVDac[v0].bIdle)
        {
            s_stHalVDac[v0].bIdle = HI_FALSE;
        }
        else
        {
            return HI_FAILURE;;
        }
    }
    
    return HI_SUCCESS;
}

HI_S32 PF_ReleaseVDAC(HI_U32 uVdac)
{
    HI_U32 v0;

    v0 = uVdac & 0xff;
    
    if (v0 < HI_DISP_VDAC_MAX_NUMBER)
    {
        if (s_stHalVDac[v0].bSupport && !s_stHalVDac[v0].bIdle)
        {
            s_stHalVDac[v0].bIdle = HI_TRUE;
        }
    }

    return HI_SUCCESS;
}


#define VDP_SYSCTRL_VDAC_BIT_SHIFT 20
HI_VOID VDP_VDAC_SetClk(DISP_VENC_E eDate, HI_U32 uVdac)
{
#if 0
    HI_U32 vdac_hd_sel = 1;

    /*select VDaCx clk*/
    if ( DISP_VENC_SDATE0 == eDate )
    {
        /*sdate_clk  :    sdate */
        vdac_hd_sel = ~(1 << (uVdac + VDP_SYSCTRL_VDAC_BIT_SHIFT));
        g_pstRegCrg->PERI_CRG54.u32 = g_pstRegCrg->PERI_CRG54.u32 & vdac_hd_sel;
    }
    else
    {
        /*hdate_clk  :    hdate/vga*/
        vdac_hd_sel = 1 << (uVdac + VDP_SYSCTRL_VDAC_BIT_SHIFT);
        g_pstRegCrg->PERI_CRG54.u32 = g_pstRegCrg->PERI_CRG54.u32 | vdac_hd_sel;
    }
#else
    U_PERI_CRG54 unTmpValue;

    unTmpValue.u32 = g_pstRegCrg->PERI_CRG54.u32;

    /*select VDaCx clk*/
    switch(uVdac)
    {
        case 0:
            unTmpValue.bits.vdac_ch0_clk_sel = (DISP_VENC_SDATE0 == eDate) ? 0 : 1;
            break;
        case 1:
            unTmpValue.bits.vdac_ch1_clk_sel = (DISP_VENC_SDATE0 == eDate) ? 0 : 1;
            break;
        case 2:
            unTmpValue.bits.vdac_ch2_clk_sel = (DISP_VENC_SDATE0 == eDate) ? 0 : 1;
            break;
        case 3:
            unTmpValue.bits.vdac_ch3_clk_sel = (DISP_VENC_SDATE0 == eDate) ? 0 : 1;
            break;
        default:
            break;
    }

    g_pstRegCrg->PERI_CRG54.u32 = unTmpValue.u32;
#endif
}
HI_S32 PF_AddVDacToVenc(DISP_VENC_E eVenc, HI_U32 uVdac, HI_DRV_DISP_VDAC_SIGNAL_E signal)
{
    HI_U32 v0;

    v0 = (uVdac >> 0) & 0xff;

    if (v0 < HI_DISP_VDAC_MAX_NUMBER)
    {
        VDP_VDAC_SetLink(eVenc, v0, signal);
        VDP_VDAC_SetClk(eVenc, v0);
    }
    return HI_SUCCESS;
}

HI_S32 PF_AddVDacToDisp(HI_DRV_DISPLAY_E enDisp, HI_U32 uVdac, HI_DRV_DISP_VDAC_SIGNAL_E signal)
{
    HI_U32 v0;

    v0 = (uVdac >> 0) & 0xff;
   
    return HI_SUCCESS;
}


HI_S32 DISP_HAL_SetIdleVDACDisable(HI_VOID)
{
    HI_U32 i;

    for(i=0; i<HI_DISP_VDAC_MAX_NUMBER; i++)
    {
        if (s_stHalVDac[i].bSupport && s_stHalVDac[i].bIdle)
        {
            //printk("================Vdac %d is idle and close it\n", i);
            VDP_VDAC_SetEnable(i, 0);
        }
        
    }

    return HI_SUCCESS;
}


HI_S32 PF_SetVDACEnable(HI_U32 uVdac, HI_BOOL bEnable)
{
    HI_U32 v0;
    
    v0 = (uVdac >> 0) & 0xff;
    
    if (v0 < HI_DISP_VDAC_MAX_NUMBER)
    {
        if (HI_TRUE == bEnable)
        {
            //VDP_VDAC_SetClockEnable(v0, bEnable);
            VDP_VDAC_SetEnable(v0, bEnable);
        }
        else
        {
            VDP_VDAC_SetEnable(v0, bEnable);
            //VDP_VDAC_SetClockEnable(v0, bEnable);
        }
    }

    DISP_HAL_SetIdleVDACDisable();

    return HI_SUCCESS;
}



/********************************************/
/* interface */
#define DISP_HAL_INTF_FUNCTION_START_HAERE
HI_VOID DISP_HAL_ResetIntfState(HI_VOID)
{
    DISP_MEMSET(&s_stHalIntf[0], 0, sizeof(DISP_LOCAL_INTF_S)*HI_DRV_DISP_INTF_ID_MAX);

    s_stHalIntf[HI_DRV_DISP_INTF_YPBPR0].bSupport = HI_TRUE;
    s_stHalIntf[HI_DRV_DISP_INTF_YPBPR0].bIdle    = HI_TRUE;
    s_stHalIntf[HI_DRV_DISP_INTF_YPBPR0].enChan = HI_DRV_DISPLAY_BUTT;

    s_stHalIntf[HI_DRV_DISP_INTF_SVIDEO0].bSupport = HI_FALSE;
    s_stHalIntf[HI_DRV_DISP_INTF_SVIDEO0].bIdle    = HI_TRUE;
    s_stHalIntf[HI_DRV_DISP_INTF_SVIDEO0].enChan = HI_DRV_DISPLAY_BUTT;

    s_stHalIntf[HI_DRV_DISP_INTF_CVBS0].bSupport = HI_TRUE;
    s_stHalIntf[HI_DRV_DISP_INTF_CVBS0].bIdle    = HI_TRUE;    
    s_stHalIntf[HI_DRV_DISP_INTF_CVBS0].enChan = HI_DRV_DISPLAY_BUTT;

    s_stHalIntf[HI_DRV_DISP_INTF_VGA0].bSupport = HI_TRUE;
    s_stHalIntf[HI_DRV_DISP_INTF_VGA0].bIdle    = HI_TRUE;
    s_stHalIntf[HI_DRV_DISP_INTF_VGA0].enChan = HI_DRV_DISPLAY_BUTT;

    s_stHalIntf[HI_DRV_DISP_INTF_RGB0].bSupport = HI_TRUE;
    s_stHalIntf[HI_DRV_DISP_INTF_RGB0].bIdle    = HI_TRUE;
    s_stHalIntf[HI_DRV_DISP_INTF_RGB0].enChan = HI_DRV_DISPLAY_BUTT;

    s_stHalIntf[HI_DRV_DISP_INTF_HDMI0].bSupport = HI_TRUE;
    s_stHalIntf[HI_DRV_DISP_INTF_HDMI0].bIdle    = HI_TRUE;
    s_stHalIntf[HI_DRV_DISP_INTF_HDMI0].enChan = HI_DRV_DISPLAY_BUTT;

    return;
}



HI_BOOL DISP_HAL_INTFNeedVenc(HI_DRV_DISP_INTF_ID_E eID)
{
    switch( eID)
    {
        case HI_DRV_DISP_INTF_YPBPR0:
        case HI_DRV_DISP_INTF_SVIDEO0:
        case HI_DRV_DISP_INTF_CVBS0:
        case HI_DRV_DISP_INTF_VGA0:
            return HI_TRUE;
        default:
            return HI_FALSE;
    }
}

HI_S32 PF_AcquireIntf2(HI_DRV_DISPLAY_E enDisp, DISP_INTF_S *pstIf)
{
    HI_DRV_DISP_INTF_S *pstIntf = &pstIf->stIf;
     
    if(    !PF_TestIntfSupport(enDisp, pstIntf->eID)
        || !s_stHalIntf[pstIntf->eID].bSupport 
        || !s_stHalIntf[pstIntf->eID].bIdle)
    {
    //printk("PF_AcquireIntf2*******************(%d)(%d)(%d)(%d)\n",enDisp,PF_TestIntfSupport(enDisp, pstIntf->eID),s_stHalIntf[pstIntf->eID].bSupport,s_stHalIntf[pstIntf->eID].bIdle);
        return HI_FAILURE;
    }
 //printk("PF_AcquireIntf2  ****(%d)(%d)(%d)***2\n", pstIntf->u8VDAC_Y_G, pstIntf->u8VDAC_Pb_B, pstIntf->u8VDAC_Pr_R);

    switch (pstIntf->eID)
    {
        case HI_DRV_DISP_INTF_YPBPR0:
        case HI_DRV_DISP_INTF_SVIDEO0:
        case HI_DRV_DISP_INTF_CVBS0:
        case HI_DRV_DISP_INTF_RGB0:
        case HI_DRV_DISP_INTF_VGA0:
            if (pstIntf->u8VDAC_Y_G != HI_DISP_VDAC_INVALID_ID)
            {
                if (!DISP_HAL_VDACIsIdle(pstIntf->u8VDAC_Y_G))
                {
                    return HI_FAILURE;
                }
            }
            if (pstIntf->u8VDAC_Pb_B != HI_DISP_VDAC_INVALID_ID)
            {
                if (!DISP_HAL_VDACIsIdle(pstIntf->u8VDAC_Pb_B))
                {
                    return HI_FAILURE;
                }
            }
            if (pstIntf->u8VDAC_Pr_R != HI_DISP_VDAC_INVALID_ID)
            {
                if (!DISP_HAL_VDACIsIdle(pstIntf->u8VDAC_Pr_R))
                {
                    return HI_FAILURE;
                }
            }
            //printk("PF_AcquireIntf2  ****(%d)(%d)(%d)***3\n", pstIntf->u8VDAC_Y_G, pstIntf->u8VDAC_Pb_B, pstIntf->u8VDAC_Pr_R);

        //acquire vdac
        if (pstIntf->u8VDAC_Y_G != HI_DISP_VDAC_INVALID_ID)
        {
            PF_AcquireVDAC(pstIntf->u8VDAC_Y_G);
        }

        if (pstIntf->u8VDAC_Pb_B != HI_DISP_VDAC_INVALID_ID)
        {
            PF_AcquireVDAC(pstIntf->u8VDAC_Pb_B);
        }

        if (pstIntf->u8VDAC_Pr_R != HI_DISP_VDAC_INVALID_ID)
        {
            PF_AcquireVDAC(pstIntf->u8VDAC_Pr_R);
            }
            break;
        case HI_DRV_DISP_INTF_HDMI0:
            break;
        default:
            break;
    }
    s_stHalIntf[pstIntf->eID].bIdle = HI_FALSE;
    s_stHalIntf[pstIntf->eID].enChan = enDisp;
    return HI_SUCCESS;
}

HI_S32 PF_ReleaseIntf2(HI_DRV_DISPLAY_E enDisp, DISP_INTF_S *pstIf)
{
    HI_DRV_DISP_INTF_S *pstIntf = &pstIf->stIf;
    if(    s_stHalIntf[pstIntf->eID].bSupport 
        && !s_stHalIntf[pstIntf->eID].bIdle)
    {
        switch (pstIntf->eID)
        {
            case HI_DRV_DISP_INTF_YPBPR0:
            case HI_DRV_DISP_INTF_SVIDEO0:
            case HI_DRV_DISP_INTF_CVBS0:
            case HI_DRV_DISP_INTF_RGB0:
            case HI_DRV_DISP_INTF_VGA0:
                if (pstIntf->u8VDAC_Y_G != HI_DISP_VDAC_INVALID_ID)
            {
                PF_ReleaseVDAC(pstIntf->u8VDAC_Y_G);
            }

            if (pstIntf->u8VDAC_Pb_B != HI_DISP_VDAC_INVALID_ID)
            {
                PF_ReleaseVDAC(pstIntf->u8VDAC_Pb_B);
            }

            if (pstIntf->u8VDAC_Pr_R != HI_DISP_VDAC_INVALID_ID)
            {
                PF_ReleaseVDAC(pstIntf->u8VDAC_Pr_R);
    }
                break;
            case HI_DRV_DISP_INTF_HDMI0:
                break;
            default:
                break;
        }

        s_stHalIntf[pstIntf->eID].bIdle = HI_TRUE;
    s_stHalIntf[pstIntf->eID].enChan = HI_DRV_DISPLAY_BUTT;
    }

    return HI_SUCCESS;
}



VDP_DISP_INTF_E DISP_HAL_GetHalIntfId(HI_DRV_DISP_INTF_ID_E eIntf)
{
    switch (eIntf)
    {
        case HI_DRV_DISP_INTF_VGA0:
            return VDP_DISP_INTF_VGA;
        case HI_DRV_DISP_INTF_HDMI0:
            return VDP_DISP_INTF_HDMI;
        case HI_DRV_DISP_INTF_BT1120_0:
            return VDP_DISP_INTF_BT1120;
        case HI_DRV_DISP_INTF_LCD0:
            return VDP_DISP_INTF_LCD;
        default:
            return VDP_DISP_INTF_BUTT;
    }
}

HI_S32 PF_ResetIntfFmt2(HI_DRV_DISPLAY_E enDisp, DISP_INTF_S *pstIf, HI_DRV_DISP_FMT_E eFmt)
{
    HI_DRV_DISP_INTF_S *pstIntf = &pstIf->stIf;

    VDP_DISP_INTF_E enVdpIntf;

    /*judge enVdpIntf and Venc link
    */

    enVdpIntf = DISP_HAL_GetHalIntfIdForVenc(enDisp,pstIf,eFmt);
    VDP_DATE_SetDACDET(pstIf->eVencId,eFmt);

    if (VDP_DISP_INTF_SDDATE == enVdpIntf )
    {
        /*DTS2013090205265  SDate attach to DHD1 all the time.
        */
        VDP_DISP_SetIntfMuxSel(DISP_HAL_GetChID(HI_DRV_DISPLAY_0), enVdpIntf);
    }
    else
        VDP_DISP_SetIntfMuxSel(DISP_HAL_GetChID(enDisp), enVdpIntf);
    
    if ( pstIf->bLinkVenc)
    {
        // s2 add venc to disp
#ifdef __DISP_D0_FOLLOW_D1__
        if ( (HI_DRV_DISPLAY_1 == enDisp) && (pstIf->eVencId == DISP_VENC_SDATE0) )
        {
            // nothing todo
        }
        else
        {  
            // s3 reset venc format
            VDP_DATE_ResetFmt(pstIf->eVencId, eFmt);
        }
#else

        VDP_DATE_ResetFmt(pstIf->eVencId, eFmt);
#endif
    }
        // s4 add vdac to venc
    switch (pstIf->stIf.eID)
    {
        case HI_DRV_DISP_INTF_YPBPR0:
    
            if (pstIntf->u8VDAC_Y_G != HI_DISP_VDAC_INVALID_ID)
            {
                PF_AddVDacToVenc(pstIf->eVencId, pstIntf->u8VDAC_Y_G, HI_DRV_DISP_VDAC_Y);
                VDP_VDAC_ResetFmt(pstIf->eVencId, pstIntf->u8VDAC_Y_G, eFmt);
            }

            if (pstIntf->u8VDAC_Pb_B != HI_DISP_VDAC_INVALID_ID)
            {
                PF_AddVDacToVenc(pstIf->eVencId, pstIntf->u8VDAC_Pb_B, HI_DRV_DISP_VDAC_PB);
                VDP_VDAC_ResetFmt(pstIf->eVencId, pstIntf->u8VDAC_Pb_B, eFmt);
            }

            if (pstIntf->u8VDAC_Pr_R != HI_DISP_VDAC_INVALID_ID)
            {
                PF_AddVDacToVenc(pstIf->eVencId, pstIntf->u8VDAC_Pr_R, HI_DRV_DISP_VDAC_PR);
                VDP_VDAC_ResetFmt(pstIf->eVencId, pstIntf->u8VDAC_Pr_R, eFmt);
            }
            break;
        case HI_DRV_DISP_INTF_RGB0:
            if (pstIntf->u8VDAC_Y_G != HI_DISP_VDAC_INVALID_ID)
            {
                PF_AddVDacToVenc(pstIf->eVencId, pstIntf->u8VDAC_Y_G, HI_DRV_DISP_VDAC_G);
                VDP_VDAC_ResetFmt(pstIf->eVencId, pstIntf->u8VDAC_Y_G, eFmt);
            }

            if (pstIntf->u8VDAC_Pb_B != HI_DISP_VDAC_INVALID_ID)
            {
                PF_AddVDacToVenc(pstIf->eVencId, pstIntf->u8VDAC_Pb_B, HI_DRV_DISP_VDAC_B);
                VDP_VDAC_ResetFmt(pstIf->eVencId, pstIntf->u8VDAC_Pb_B, eFmt);
            }

            if (pstIntf->u8VDAC_Pr_R != HI_DISP_VDAC_INVALID_ID)
            {
                PF_AddVDacToVenc(pstIf->eVencId, pstIntf->u8VDAC_Pr_R, HI_DRV_DISP_VDAC_R);
                VDP_VDAC_ResetFmt(pstIf->eVencId, pstIntf->u8VDAC_Pr_R, eFmt);
            }
            break;

        case HI_DRV_DISP_INTF_CVBS0:

            if (pstIntf->u8VDAC_Y_G != HI_DISP_VDAC_INVALID_ID)
            {
                PF_AddVDacToVenc(pstIf->eVencId, pstIntf->u8VDAC_Y_G, HI_DRV_DISP_VDAC_CVBS);
                VDP_VDAC_ResetFmt(pstIf->eVencId, pstIntf->u8VDAC_Y_G, eFmt);
            }
            break;

        case HI_DRV_DISP_INTF_SVIDEO0:

            if (pstIntf->u8VDAC_Y_G != HI_DISP_VDAC_INVALID_ID)
            {
                PF_AddVDacToVenc(pstIf->eVencId, pstIntf->u8VDAC_Pb_B, HI_DRV_DISP_VDAC_SV_Y);
                VDP_VDAC_ResetFmt(pstIf->eVencId, pstIntf->u8VDAC_Pb_B, eFmt);
            }

            if (pstIntf->u8VDAC_Pb_B != HI_DISP_VDAC_INVALID_ID)
            {
                PF_AddVDacToVenc(pstIf->eVencId, pstIntf->u8VDAC_Pr_R, HI_DRV_DISP_VDAC_SV_C);
                VDP_VDAC_ResetFmt(pstIf->eVencId, pstIntf->u8VDAC_Pr_R, eFmt);
            }
            break;

        case HI_DRV_DISP_INTF_VGA0:
            if (pstIntf->u8VDAC_Y_G != HI_DISP_VDAC_INVALID_ID)
            {
                PF_AddVDacToVenc(pstIf->eVencId, pstIntf->u8VDAC_Y_G, HI_DRV_DISP_VDAC_G);

                VDP_VDAC_ResetFmt(pstIf->eVencId, pstIntf->u8VDAC_Y_G, eFmt);
            }

            if (pstIntf->u8VDAC_Pb_B != HI_DISP_VDAC_INVALID_ID)
            {
                PF_AddVDacToVenc(pstIf->eVencId, pstIntf->u8VDAC_Pb_B, HI_DRV_DISP_VDAC_B);
                VDP_VDAC_ResetFmt(pstIf->eVencId, pstIntf->u8VDAC_Pb_B, eFmt);
            }

            if (pstIntf->u8VDAC_Pr_R != HI_DISP_VDAC_INVALID_ID)
            {
                PF_AddVDacToVenc(pstIf->eVencId, pstIntf->u8VDAC_Pr_R, HI_DRV_DISP_VDAC_R);
                VDP_VDAC_ResetFmt(pstIf->eVencId, pstIntf->u8VDAC_Pr_R, eFmt);
            }

            break;
        case HI_DRV_DISP_INTF_HDMI0:
            /*select hdmi corlor space 
        vdp set  yuv ,so need  hdmi judge whether  use CSC yuv to rgb;
         */
            if  ((eFmt >= HI_DRV_DISP_FMT_861D_640X480_60) && (eFmt <= HI_DRV_DISP_FMT_CUSTOM))
            { pstIf ->enHDMIDataType = DISP_HDMI_DATA_RGB; }
            else
            { pstIf ->enHDMIDataType = DISP_HDMI_DATA_YUV; }

            if  ((eFmt >= HI_DRV_DISP_FMT_PAL) && (eFmt <= HI_DRV_DISP_FMT_1440x480i_60) )
            {
                /*SD format */
                /*clk : 1:2*/
                VDP_DISP_SetHdmiClk(DISP_HAL_GetChID(enDisp),1);
            }
            else
            {
                /*clk : 1:1*/
                VDP_DISP_SetHdmiClk(DISP_HAL_GetChID(enDisp),0);
            }
            
         //printk("hal****hdmi :  disp%d---link(%d)\n",enDisp,pstIf->enHDMIDataType);
            VDP_DISP_SetHdmiMode(DISP_HAL_GetChID(enDisp), pstIf->enHDMIDataType);
            VDP_DISP_SetRegUp(DISP_HAL_GetChID(enDisp));
              break;
        case HI_DRV_DISP_INTF_BT656_0:
        case HI_DRV_DISP_INTF_BT1120_0:
        case HI_DRV_DISP_INTF_LCD0:
            break;

        default:
            return VDP_DISP_INTF_BUTT;
    }


    return HI_SUCCESS;
}

#if 0
HI_S32 PF_AddIntfToDisp(HI_DRV_DISPLAY_E enDisp, HI_DRV_DISP_INTF_ID_E eIntf)
{
    VDP_DISP_SetIntfMuxSel(DISP_HAL_GetChID(enDisp), 
                           DISP_HAL_GetHalIntfId(eIntf));
    
    return HI_SUCCESS;
}

HI_S32 PF_SetIntfColor(HI_DRV_DISP_INTF_ID_E eIntf, DISP_HAL_COLOR_S *pstColor)
{

    return HI_SUCCESS;
}
#endif

HI_S32 PF_SetIntfEnable2(HI_DRV_DISPLAY_E enDisp, DISP_INTF_S *pstIt, HI_BOOL bEnable)
{
    DISP_VENC_E enVencId = DISP_VENC_MAX;
    /* DTS2013090909501 
        switch fmt process:
        attach mode :
            step1: close HD  adn SD interface
            step2: switch format
            step3: open  HD  adn SD interface,
        no attach mode :
            step1: close HD/SD interface
            step2: switch format
            step3: open HD/SD interface,
        but in  no attach mode:
            if disp1 switch format  HDFmt -->SDFmt ,
            step1: close hdate 
            step2: switch format
            step3: open sdate,
            SDFmt -->HDFmt has the same bug;
            so should process disp  by channel;
    */

    if (pstIt->bLinkVenc)
    {
        if ( HI_DRV_DISPLAY_0 == enDisp )
            enVencId = DISP_VENC_SDATE0;
        else if  (HI_DRV_DISPLAY_1 == enDisp )
            enVencId = DISP_VENC_HDATE0;
        else
            enVencId = DISP_VENC_MAX;
            
        VDP_DATE_SetEnable(enVencId, (HI_U32)bEnable);
        VDP_DATE_SetDACDetEn(enVencId, (HI_U32)bEnable);
    }

    // set vdac link
    if (pstIt->stIf.u8VDAC_Y_G != HI_DISP_VDAC_INVALID_ID)
    {
        PF_SetVDACEnable(pstIt->stIf.u8VDAC_Y_G, bEnable);
    //printk("DispSetIntfEnable  002\n");
    }

    if (pstIt->stIf.u8VDAC_Pb_B != HI_DISP_VDAC_INVALID_ID)
    {
        PF_SetVDACEnable(pstIt->stIf.u8VDAC_Pb_B, bEnable);
    }

    if (pstIt->stIf.u8VDAC_Pr_R != HI_DISP_VDAC_INVALID_ID)
    {
        PF_SetVDACEnable(pstIt->stIf.u8VDAC_Pr_R, bEnable);
    }

    return HI_SUCCESS;
}





/*===========================================================*/
/* WBC manager */
HI_VOID InitWbcState(HI_VOID)
{
    DISP_WBC_E eID;

    for(eID = DISP_WBC_00; eID < DISP_WBC_BUTT; eID++)
    {
        s_stWBC[eID].bSupport = HI_TRUE;
        s_stWBC[eID].bIdle    = HI_TRUE;
    }

    return;
}


HI_S32 PF_AcquireWbcByChn(HI_DRV_DISPLAY_E eChn, DISP_WBC_E *peWbc)
{
    if (eChn == HI_DRV_DISPLAY_1)
    {
        if(  (s_stWBC[DISP_WBC_00].bSupport == HI_TRUE)
            &&(s_stWBC[DISP_WBC_00].bIdle == HI_TRUE) 
            )
        {
            s_stWBC[DISP_WBC_00].bIdle = HI_FALSE;
            s_stWBC[DISP_WBC_00].u32RefCnt = 1;
            *peWbc = DISP_WBC_00;
            return HI_SUCCESS;
        } else if ((s_stWBC[DISP_WBC_00].bSupport == HI_TRUE)
            &&(s_stWBC[DISP_WBC_00].bIdle == HI_FALSE)) 
        {
            s_stWBC[DISP_WBC_00].u32RefCnt++;
            *peWbc = DISP_WBC_00;
            return HI_SUCCESS;
        }
    }

    return HI_FAILURE;
}

HI_S32 PF_AcquireWbc(DISP_WBC_E eWbc)
{
    if(  (s_stWBC[eWbc].bSupport == HI_TRUE)
        &&(s_stWBC[eWbc].bIdle == HI_TRUE) 
        )
    {
        s_stWBC[eWbc].bIdle = HI_FALSE;
        s_stWBC[eWbc].u32RefCnt = 1;
        return HI_SUCCESS;
    } else if ((s_stWBC[eWbc].bSupport == HI_TRUE)
        &&(s_stWBC[eWbc].bIdle == HI_FALSE))
    {
        s_stWBC[eWbc].u32RefCnt ++;
        return HI_SUCCESS;
    }

    return HI_FAILURE;
}

HI_S32 PF_ReleaseWbc(DISP_WBC_E eWbc)
{
    if(  (s_stWBC[eWbc].bSupport == HI_TRUE)
        &&(s_stWBC[eWbc].bIdle == HI_FALSE) 
        )
    {
        if (--s_stWBC[eWbc].u32RefCnt == 0)
        s_stWBC[eWbc].bIdle = HI_TRUE;
        return HI_SUCCESS;
    }

    return HI_FAILURE;
}


static VDP_LAYER_WBC_E s_eWbcHalID[DISP_WBC_BUTT] = 
{VDP_LAYER_WBC_HD0};

/* WBC */
HI_S32 PF_SetWbcDefault(DISP_WBC_E eWbc)
{

    VDP_WBC_SetOutIntf(s_eWbcHalID[eWbc], VDP_RMODE_PROGRESSIVE);
    VDP_WBC_SetZmeInFmt(s_eWbcHalID[eWbc], VDP_PROC_FMT_SP_422);

    return HI_SUCCESS;
}

#ifndef __DISP_PLATFORM_BOOT__
static HI_S32 HAL_ReviseWbcZmeInput(DISP_WBC_E eWbc, HI_DISP_DISPLAY_INFO_S *pstDispInfo, HI_U32 *u32Width, HI_U32 *u32Height)
{    
    return Chip_Specific_ReviseWbcZmeInput(eWbc, pstDispInfo, u32Width, u32Height);        
}
#endif


HI_S32 PF_SetWbc3DInfo(DISP_WBC_E eWbc, HI_DISP_DISPLAY_INFO_S *pstDispInfo, HI_RECT_S *in)
{
    #ifndef __DISP_PLATFORM_BOOT__
    return Chip_Specific_SetWbc3DInfo(eWbc, pstDispInfo, in);    
    #else
    return HI_SUCCESS;
    #endif
}

HI_S32 PF_SetWbcIORect(DISP_WBC_E eWbc, HI_DISP_DISPLAY_INFO_S *pstDispInfo, HI_RECT_S *in, HI_RECT_S *out)
{
#ifndef __DISP_PLATFORM_BOOT__
    VDP_DISP_RECT_S stRect;
    ALG_VZME_DRV_PARA_S stZmeI;
    ALG_VZME_RTL_PARA_S stZmeO;
    HI_U32 pbBtm = 0, pu32Vcnt = 0;
    HI_BOOL bAlwayPro = HI_FALSE;
    
    DISP_MEMSET(&stRect, 0, sizeof(VDP_DISP_RECT_S));
    DISP_MEMSET(&stZmeI, 0, sizeof(ALG_VZME_DRV_PARA_S));
    DISP_MEMSET(&stZmeO, 0, sizeof(ALG_VZME_RTL_PARA_S));
    
    stRect.u32DXS = (HI_U32)in->s32X;
    stRect.u32DYS = (HI_U32)in->s32Y;
    stRect.u32DXL = (HI_U32)in->s32Width + stRect.u32DXS;
    stRect.u32DYL = (HI_U32)in->s32Height  + stRect.u32DYS;

    stRect.u32OWth = (HI_U32)out->s32Width;
    stRect.u32OHgt = (HI_U32)out->s32Height;
    VDP_WBC_SetLayerReso(s_eWbcHalID[eWbc], stRect);

    /*wbc we fix to progressive.*/
    VDP_WBC_SetOutIntf(s_eWbcHalID[eWbc], VDP_RMODE_PROGRESSIVE);

    stZmeI.u32ZmeFrmWIn = in->s32Width;
    stZmeI.u32ZmeFrmHIn = in->s32Height;    
    HAL_ReviseWbcZmeInput(eWbc, pstDispInfo, &stZmeI.u32ZmeFrmWIn, &stZmeI.u32ZmeFrmHIn);
    
    stZmeI.u32ZmeFrmWOut = (HI_U32)out->s32Width;
    stZmeI.u32ZmeFrmHOut = (HI_U32)out->s32Height;
    
    stZmeI.u8ZmeYCFmtIn  = 0; /*zme in is always 422.*/
    stZmeI.u8ZmeYCFmtOut = 1; /*zme out is 420 in wbc for encoder, 
                               other condition  422 may be neccessary,but ip not validated.*/
                                
    /*Pay attention, although we set frame width to ip, 
      but here we should pass real filed mode to alg.*/
    (HI_VOID)VDP_WBC_GetAlwaysProFlag(/*eWbc,*/ &bAlwayPro);

    if (bAlwayPro == 0)
    {                              
        if (0 == VDP_DHD_GetDispMode(0)){
            
            /*since this configure will take affect next intr,so opposite it.*/
            VDP_DISP_GetVactState(0, &pbBtm, &pu32Vcnt);       
            pbBtm = 1 - pbBtm; 
            stZmeI.bZmeFrmFmtIn  = 0;                               
            stZmeI.bZmeBFIn  = pbBtm;
        } else {        
            stZmeI.bZmeFrmFmtIn  = 1;                               
            stZmeI.bZmeBFIn  = 0;        
        }
    } else {
        /*in cv200, always progressive.*/
        stZmeI.bZmeFrmFmtIn  = 1;                               
        stZmeI.bZmeBFIn  = 0;
    }
    
    /*wbc output fix to 1, frame.*/
    stZmeI.bZmeFrmFmtOut = 1;    
    stZmeI.bZmeBFOut = 0;
    
    if (HI_SUCCESS != HAL_WbcZmeFunc(eWbc, &stZmeI, &stZmeO))
        return HI_FAILURE;
    
    VDP_WBC_SetZmeHorRatio(s_eWbcHalID[eWbc], stZmeO.u32ZmeRatioHL);
    VDP_WBC_SetZmeVerRatio(s_eWbcHalID[eWbc], stZmeO.u32ZmeRatioVL);    

    VDP_WBC_SetZmePhaseH(s_eWbcHalID[eWbc],  stZmeO.s32ZmeOffsetHL, stZmeO.s32ZmeOffsetHC);
    VDP_WBC_SetZmePhaseV(s_eWbcHalID[eWbc],  stZmeO.s32ZmeOffsetVL, stZmeO.s32ZmeOffsetVC);       
#if 1
    /* zme enable and  fir enable should not fix to a value.*/
    VDP_WBC_SetZmeEnable(s_eWbcHalID[eWbc], VDP_ZME_MODE_HORL, stZmeO.bZmeEnHL, stZmeO.bZmeMdHL);
    VDP_WBC_SetZmeEnable(s_eWbcHalID[eWbc], VDP_ZME_MODE_HORC, stZmeO.bZmeEnHC, stZmeO.bZmeMdHC);
    VDP_WBC_SetZmeEnable(s_eWbcHalID[eWbc], VDP_ZME_MODE_VERL, stZmeO.bZmeEnVL, stZmeO.bZmeMdVL);
    VDP_WBC_SetZmeEnable(s_eWbcHalID[eWbc], VDP_ZME_MODE_VERC, stZmeO.bZmeEnVC, stZmeO.bZmeMdVC);    
#else
    /* zme enable and  fir enable should not fix to a value.*/
    VDP_WBC_SetZmeEnable(s_eWbcHalID[eWbc], VDP_ZME_MODE_HORL, 0, 0);
    VDP_WBC_SetZmeEnable(s_eWbcHalID[eWbc], VDP_ZME_MODE_HORC, 0, 0);
    VDP_WBC_SetZmeEnable(s_eWbcHalID[eWbc], VDP_ZME_MODE_VERL, 0, 0);
    VDP_WBC_SetZmeEnable(s_eWbcHalID[eWbc], VDP_ZME_MODE_VERC, 0, 0);    
#endif

    /*set media fir.*/
    VDP_WBC_SetMidEnable(VDP_LAYER_WBC_HD0, VDP_ZME_MODE_HORL, stZmeO.bZmeMedHL);
    VDP_WBC_SetMidEnable(VDP_LAYER_WBC_HD0, VDP_ZME_MODE_HORC, stZmeO.bZmeMedHC);
    VDP_WBC_SetMidEnable(VDP_LAYER_WBC_HD0, VDP_ZME_MODE_VERL, stZmeO.bZmeMedVL);
    VDP_WBC_SetMidEnable(VDP_LAYER_WBC_HD0, VDP_ZME_MODE_VERC, stZmeO.bZmeMedVC);

    /*set zme order, h first or v first.*/
    VDP_WBC_SetZmeHfirOrder(VDP_LAYER_WBC_HD0, stZmeO.bZmeOrder);
    /*set v chroma zme tap.*/
    VDP_WBC_SetZmeVerTap(VDP_LAYER_WBC_HD0,VDP_ZME_MODE_VERC, stZmeO.bZmeTapVC);

    /*set  hor fir coef addr, and set updata flag.*/
    if (stZmeO.u32ZmeRatioHL) {
        VDP_WBC_SetZmeCoefAddr(VDP_LAYER_WBC_HD0, VDP_WBC_PARA_ZME_HOR, stZmeO.u32ZmeCoefAddrHL, stZmeO.u32ZmeCoefAddrHC);
        VDP_WBC_SetParaUpd(VDP_LAYER_WBC_HD0, VDP_WBC_PARA_ZME_HOR);
    }
    
    /*set  ver fir coef addr, and set updata flag.*/    
    if (stZmeO.u32ZmeRatioVL) {
        VDP_WBC_SetZmeCoefAddr(VDP_LAYER_WBC_HD0, VDP_WBC_PARA_ZME_VER, stZmeO.u32ZmeCoefAddrVL,stZmeO.u32ZmeCoefAddrVC);
        VDP_WBC_SetParaUpd(VDP_LAYER_WBC_HD0, VDP_WBC_PARA_ZME_VER);
    }


    VDP_WBC_SetZmeInFmt(VDP_LAYER_WBC_HD0, VDP_PROC_FMT_SP_422);
#endif

    return HI_SUCCESS;
}

HI_S32 PF_SetWbcColorSpace(DISP_WBC_E eWbc, HI_DRV_COLOR_SPACE_E eSrcCS, HI_DRV_COLOR_SPACE_E eDstCS)
{
    // //TODO:
    VDP_WBC_SetCscEnable(s_eWbcHalID[eWbc], 0);
    return HI_SUCCESS;
}

HI_S32 PF_SetWbcPixFmt(DISP_WBC_E eWbc, HI_DRV_PIX_FORMAT_E eFmt)
{
    switch(eFmt)
    {
        case HI_DRV_PIX_FMT_NV21:
            VDP_WBC_SetOutFmt(s_eWbcHalID[eWbc], VDP_WBC_OFMT_SP420);
            VDP_WBC_SetOutFmtUVOrder(s_eWbcHalID[eWbc], 0);
            VDP_WBC_SetZmeOutFmt(s_eWbcHalID[eWbc], VDP_PROC_FMT_SP_420);
            break;
        case HI_DRV_PIX_FMT_NV12:
            VDP_WBC_SetOutFmt(s_eWbcHalID[eWbc], VDP_WBC_OFMT_SP420);
            VDP_WBC_SetOutFmtUVOrder(s_eWbcHalID[eWbc], 1);
            VDP_WBC_SetZmeOutFmt(s_eWbcHalID[eWbc], VDP_PROC_FMT_SP_420);
            break;
        case HI_DRV_PIX_FMT_YUYV:
            VDP_WBC_SetOutFmt(s_eWbcHalID[eWbc], VDP_WBC_OFMT_PKG_YUYV);
            VDP_WBC_SetZmeOutFmt(s_eWbcHalID[eWbc], VDP_PROC_FMT_SP_422);
            break;
        case HI_DRV_PIX_FMT_UYVY:
            VDP_WBC_SetOutFmt(s_eWbcHalID[eWbc], VDP_WBC_OFMT_PKG_UYVY);
            VDP_WBC_SetZmeOutFmt(s_eWbcHalID[eWbc], VDP_PROC_FMT_SP_422);
            break;
        case HI_DRV_PIX_FMT_YVYU:
            VDP_WBC_SetOutFmt(s_eWbcHalID[eWbc], VDP_WBC_OFMT_PKG_YVYU);
            VDP_WBC_SetZmeOutFmt(s_eWbcHalID[eWbc], VDP_PROC_FMT_SP_422);
            break;
        default :
            VDP_WBC_SetOutFmt(s_eWbcHalID[eWbc], VDP_WBC_OFMT_SP420);
            VDP_WBC_SetOutFmtUVOrder(s_eWbcHalID[eWbc], 0);
            VDP_WBC_SetZmeOutFmt(s_eWbcHalID[eWbc], VDP_PROC_FMT_SP_420);
            break;
    }


    return HI_SUCCESS;
}

HI_S32 PF_SetWbcAddr(DISP_WBC_E eWbc, HI_DRV_VID_FRAME_ADDR_S *pstAddr)
{
    VDP_WBC_SetLayerAddr(s_eWbcHalID[eWbc], 
                         pstAddr->u32PhyAddr_Y, pstAddr->u32PhyAddr_C, 
                         pstAddr->u32Stride_Y, pstAddr->u32Stride_C);
    return HI_SUCCESS;
}

HI_S32 PF_SetWbcEnable(DISP_WBC_E eWbc, HI_BOOL bEnable)
{
    VDP_WBC_SetEnable(s_eWbcHalID[eWbc], (HI_U32)bEnable);
    return HI_SUCCESS;
}

HI_S32 PF_UpdateWbc(DISP_WBC_E eWbc)
{
    VDP_WBC_SetRegUp(s_eWbcHalID[eWbc]);
    return HI_SUCCESS;
}

HI_S32 PF_CBM_MovTOP(HI_DRV_DISPLAY_E enDisp, HI_DRV_DISP_LAYER_E enLayer)
{
    return Chip_Specific_CBM_MovTOP(enDisp,enLayer);
}
HI_S32 PF_VDP_RegSave(HI_VOID)
{
    HI_U32 nRet;
    nRet = DISP_OS_MMZ_Map(&g_RegBackBuf);
    if (HI_SUCCESS == nRet)
    {
        VDP_RegSave(g_RegBackBuf.u32StartVirAddr);
        DISP_OS_MMZ_UnMap(&g_RegBackBuf);
    }
    else
    {
        DISP_FATAL("save reg buffer mmz mmp fail\n");
    }
    return nRet;
}
HI_S32 PF_VDP_RegReStore(HI_VOID)
{
    HI_U32 nRet;
    nRet = DISP_OS_MMZ_Map(&g_RegBackBuf);
    if (HI_SUCCESS == nRet)
    {
        VDP_RegReStore(g_RegBackBuf.u32StartVirAddr);
        DISP_OS_MMZ_UnMap(&g_RegBackBuf);
    }
    else
    {
        DISP_FATAL("restore reg buffer mmz mmp fail\n");
    }
    return nRet;
}




//interrupt
#define DAC_B    0x1           //DAC3
#define DAC_R    0x2           //DAC1
#define DAC_G    0x4           //DAC2 
#define DAC_CVBS 0x8           //DAC0
//vdac
#define DAC_C_CTRL  0x0    
#define DAC_R_CTRL  0x1  
#define DAC_G_CTRL  0x2  
#define DAC_B_CTRL  0x3  
/*******************************************
interrupt           vo dac ctrl
DAC0    ---------->> VDAC3    0xf8cc013c
DAC1    ---------->> VDAC0    0xf8cc0130
DAC2    ---------->> VDAC1    0xf8cc0134
DAC3    ---------->> VDAC2    0xf8cc0138
********************************************/

HI_S32 DAC_PoweDown(HI_U8 u8DAC)
{
    HI_U32 s32Enable = 0;
    VDP_VDAC_GetEnable(u8DAC, &s32Enable);
    if(s32Enable)
    {
        VDP_VDAC_SetEnable(u8DAC, HI_FALSE);
    }
    return HI_SUCCESS;
}

HI_S32 DAC_PoweUp(HI_U8 u8DAC)
{
    HI_U32 s32Enable = 0;
    
    VDP_VDAC_GetEnable(u8DAC, &s32Enable);
    if(!s32Enable)
    {
        VDP_VDAC_SetEnable(u8DAC, HI_TRUE);
    }
    return HI_SUCCESS;
}
static HI_U32 g_u32RecordTime = 0;
static HI_U32 g_u32InterruptCount = 0;
#define WAKEUP_TIMER_MS 2000
#define POWERDOWN_TIMRR_MS 100
#define POWERUP_EVENT    1
#define POWERDOWN_EVENT  2
#define INTERRUPT_COUNT  8
HI_U32 GetTimerStatus(HI_VOID)
{
    HI_S32 u32Time = 0,u32TimeDiff = 0;
    DISP_OS_GetTime((HI_U32 *)&u32Time);

    u32TimeDiff = u32Time - g_u32RecordTime;
    if(u32TimeDiff >= WAKEUP_TIMER_MS)
    {
        g_u32RecordTime = u32Time;
        return POWERUP_EVENT;
    }
    else if (u32TimeDiff >= POWERDOWN_TIMRR_MS)
    {
        return POWERDOWN_EVENT;
    }
    else
    {
        //void
    }
    
    return HI_SUCCESS;
}

HI_S32 PF_DACIsr(HI_U8 u8DAC)
{
    HI_U8 u8Data,u32Event;
    //return 0;
    u8Data = u8DAC&DAC_CVBS;
    u32Event =  GetTimerStatus();
   // printk("dac:0x%x,event:0x%d\n",u8DAC,u32Event);
    if(!u8Data)
    {
        if(POWERDOWN_EVENT == u32Event)
        {
            DAC_PoweDown(DAC_B_CTRL);
        }
    }
    
    u8Data = u8DAC&DAC_R;
    if(!u8Data)
    {
        if(POWERDOWN_EVENT == u32Event)
        {
            DAC_PoweDown(DAC_R_CTRL);
        }
    }
    
    u8Data = u8DAC&DAC_G;
    if(!u8Data)
    {
        if(POWERDOWN_EVENT == u32Event)
        {
            DAC_PoweDown(DAC_G_CTRL);
        }
    }
    
    u8Data = u8DAC&DAC_B;
    if(!u8Data)
    {
        if(POWERDOWN_EVENT == u32Event)
        {
            DAC_PoweDown(DAC_C_CTRL);
        }
    }
    
    if(POWERUP_EVENT == u32Event)
    {
        DAC_PoweUp(DAC_R_CTRL);
        DAC_PoweUp(DAC_G_CTRL);
        DAC_PoweUp(DAC_B_CTRL);
        DAC_PoweUp(DAC_C_CTRL);
        if(g_u32InterruptCount++ > INTERRUPT_COUNT)
        {
            PF_CleanIntState(VDAC_STATE_INT);//clear status
        }
    }
    return HI_SUCCESS;
}

/*==============================================*/
HI_VOID DispGetVersion(HI_U32 *pu32VirBaseAddr, HI_DRV_DISP_VERSION_S * pVersion)
{
    if (pu32VirBaseAddr)
    {
        pVersion->u32VersionPartL = pu32VirBaseAddr[4];
        pVersion->u32VersionPartH = pu32VirBaseAddr[5];
    }
    else
    {
        pVersion->u32VersionPartL = 0;
        pVersion->u32VersionPartH = 0;
    }
    
    return;
}

#define DISP_VDP_CLOCK_BUS_CKEN     0x00000001UL
#define DISP_VDP_CLOCK_CKEN         0x00000002UL
#define DISP_VDP_CLOCK_CLKOUT_CKEN  0x00000400UL

//#define DISP_VDP_CLOCK_BITS_TOTAL   0x00000403UL
#define DISP_VDP_CLOCK_BITS_TOTAL  (DISP_VDP_CLOCK_BUS_CKEN \
                                    | DISP_VDP_CLOCK_CKEN \
                                    | DISP_VDP_CLOCK_CLKOUT_CKEN)


HI_S32 DISP_OpenClock(HI_VOID)
{
#if 0
    HI_U32 val;    
    val = g_pstRegCrg->PERI_CRG54.u32;
    
    if (  (val & DISP_CV200_ES_SYSCTRL_RESET_BIT)
        || (  (val & DISP_VDP_CLOCK_BITS_TOTAL) != DISP_VDP_CLOCK_BITS_TOTAL)
       )
    {
        /*here must set 'RESET' instead of '0', else VDP will die and never relive.*/
        g_pstRegCrg->PERI_CRG54.u32 = DISP_CV200_ES_SYSCTRL_RESET_VALUE | DISP_CV200_ES_SYSCTRL_RESET_BIT;
        DISP_MSLEEP(5);        
        g_pstRegCrg->PERI_CRG54.u32 = DISP_CV200_ES_SYSCTRL_RESET_VALUE;
    }

    return HI_SUCCESS;
#else
    /* open clock and reset vdp module*/
    U_PERI_CRG54 unTmpValue;

    unTmpValue.u32 = g_pstRegCrg->PERI_CRG54.u32;

    if (   unTmpValue.bits.vou_srst_req 
        || (!unTmpValue.bits.vo_bus_cken)
        || (!unTmpValue.bits.vo_cken)
        || (!unTmpValue.bits.vo_clkout_cken))
    {
        //unTmpValue.u32 &= (~0x77fff7ff); /*bit11 bit27 bit31 are reserved*/
        //unTmpValue.u32 |= DISP_CV200_ES_SYSCTRL_RESET_VALUE; /*open and set clock*/
        DISP_ResetCRG54(&unTmpValue.u32);
        unTmpValue.bits.vou_srst_req = 1; /*reset*/
        g_pstRegCrg->PERI_CRG54.u32 = unTmpValue.u32;
        
        DISP_MSLEEP(5);
        
        unTmpValue.bits.vou_srst_req = 0; /*cancel reset*/
        g_pstRegCrg->PERI_CRG54.u32 = unTmpValue.u32;
    }

    return HI_SUCCESS;
#endif
}

HI_S32 DISP_CloseClock(HI_VOID)
{
#if 0
    g_pstRegCrg->PERI_CRG54.u32 = 0;
    return HI_SUCCESS;
#else
    U_PERI_CRG54 unTmpValue;

    unTmpValue.u32 = g_pstRegCrg->PERI_CRG54.u32;

    /*Close Clock*/
    //unTmpValue.u32 = 0;
    unTmpValue.bits.vo_bus_cken  = 0;
    unTmpValue.bits.vo_cken      = 0;
    unTmpValue.bits.vou_srst_req = 1; /*set reset*/

    g_pstRegCrg->PERI_CRG54.u32 = unTmpValue.u32;
    return HI_SUCCESS;
#endif
}

HI_S32 PF_DATE_SetCoef(HI_DRV_DISPLAY_E enDisp, HI_BOOL bDefault)
{

    if (HI_DRV_DISPLAY_0 != enDisp)
        return HI_FAILURE;

    if (bDefault)
        VDP_DATE_SetSrcCoef(DISP_VENC_SDATE0, u32aSDDATEDefSrcCoef);
    else
        VDP_DATE_SetSrcCoef(DISP_VENC_SDATE0, u32aSDDATESrcCoef);

    return HI_SUCCESS;
}


HI_S32 DISP_HAL_Init(HI_U32 u32Base)
{
    HI_S32 nRet;
    
    // s1 if input version
    if(s_DispIntfFlag > 0)
    {
        return HI_SUCCESS;
    }

    // s1.2 open vdp clock. 
    nRet = DISP_OpenClock();
    if (nRet != HI_SUCCESS)
    {
        return nRet;
    }

    // s2 if input Base
    // s2.1 get virual address
    s_u32VdpBaseAddr = (HI_U32)DISP_IOADDRESS(u32Base);
        
    if (!s_u32VdpBaseAddr)
    {
        return HI_FAILURE;
    }
#ifndef __DISP_PLATFORM_BOOT__
    nRet = DISP_OS_MMZ_Alloc("VDP_RegBackBuff", HI_NULL, 0x10000, 16, &g_RegBackBuf);
    if (nRet)
    {
        DISP_FATAL("Alloc MMZ VDP_RegBackBuff failed\n");
        return nRet;
    }
#endif
    DispGetVersion((HI_U32 *)s_u32VdpBaseAddr, &s_stVersion);
    //printk(">>>>>>>>>>>Base=%x, vir=%x, vh=%x, vl=%x\n", 
    //      u32Base, RegVirAddr, s_stVersion.u32VersionPartH, s_stVersion.u32VersionPartL);

    // s2.3 initial ops
    DISP_MEMSET(&s_stIntfOps, 0, sizeof(DISP_INTF_OPERATION_S));

    if (  (s_stVersion.u32VersionPartH == DISP_CV200_ES_VERSION_H)
        &&(s_stVersion.u32VersionPartL == DISP_CV200_ES_VERSION_L)
        )
    {
        nRet = DISP_HAL_InitVDPState();
        if (nRet)
        {
            DISP_FATAL("Hal init vdp state failed\n");
            return nRet;
        }

        //printk(">>>>>>>>>>> DISP_X5HD2_MPW_VERSION \n");
        /* reset hardware */
        s_stIntfOps.PF_ResetHardware  = PF_ResetVdpHardware;
        s_stIntfOps.PF_CloseClkResetModule  = PF_CloseClkResetModule;

        /* display */
        /* capability */
        s_stIntfOps.PF_TestChnSupport     = PF_TestChnSupport;
        s_stIntfOps.PF_TestChnSupportHD   = PF_TestChnSupportHD;
        s_stIntfOps.PF_TestIntfSupport    = PF_TestIntfSupport;
        s_stIntfOps.PF_TestChnSupportCast = PF_TestChnSupportCast;
        s_stIntfOps.PF_TestChnEncFmt      = PF_TestChnEncFmt;
        s_stIntfOps.PF_TestChnAttach      = PF_TestChnAttach;

        //s_stIntfOps.PF_ResetChn  = PF_ResetChn;
        //s_stIntfOps.PF_ConfigChn = PF_ConfigChn;
        s_stIntfOps.PF_SetChnFmt = PF_SetChnFmt;
        s_stIntfOps.PF_SetChnTiming = PF_SetChnTiming;

        s_stIntfOps.PF_SetChnPixFmt  = PF_SetChnPixFmt;
        s_stIntfOps.PF_SetChnBgColor = PF_SetChnBgColor;
        s_stIntfOps.PF_SetChnColor   = PF_SetChnColor;
        s_stIntfOps.PF_SetDispSignal = PF_SetDispSignal;

        s_stIntfOps.PF_SetChnEnable  = PF_SetChnEnable;
        s_stIntfOps.PF_GetChnEnable  = PF_GetChnEnable;
        s_stIntfOps.PF_SetMSChnEnable = PF_SetMSChnEnable;

        /* interrypt */
        s_stIntfOps.PF_SetIntEnable = PF_SetIntEnable;
        s_stIntfOps.PF_GetMaskedIntState = PF_GetMaskedIntState;
        s_stIntfOps.PF_GetIntSetting = PF_GetIntSetting;
        s_stIntfOps.PF_GetUnmaskedIntState = PF_GetUnmaskedIntState;
        s_stIntfOps.PF_CleanIntState = PF_CleanIntState;
        s_stIntfOps.FP_GetChnIntState = FP_GetChnIntState;
        s_stIntfOps.FP_GetChnBottomFlag = FP_GetChnBottomFlag;

        /* interface manager */
        s_stIntfOps.PF_AcquireIntf2   = PF_AcquireIntf2;
        s_stIntfOps.PF_ReleaseIntf2   = PF_ReleaseIntf2;
        s_stIntfOps.PF_ResetIntfFmt2  = PF_ResetIntfFmt2;
        s_stIntfOps.PF_SetIntfEnable2 = PF_SetIntfEnable2;

        /* WBC manager */
        s_stIntfOps.PF_AcquireWbcByChn = PF_AcquireWbcByChn;
        s_stIntfOps.PF_AcquireWbc = PF_AcquireWbc;
        s_stIntfOps.PF_ReleaseWbc = PF_ReleaseWbc;

        /* WBC */
        s_stIntfOps.PF_SetWbcIORect = PF_SetWbcIORect;
        s_stIntfOps.PF_SetWbc3DInfo = PF_SetWbc3DInfo;
        s_stIntfOps.PF_SetWbcColorSpace = PF_SetWbcColorSpace;
        s_stIntfOps.PF_SetWbcPixFmt = PF_SetWbcPixFmt;
        s_stIntfOps.PF_SetWbcAddr = PF_SetWbcAddr;
        s_stIntfOps.PF_SetWbcEnable = PF_SetWbcEnable;
        s_stIntfOps.PF_UpdateWbc= PF_UpdateWbc;
        s_stIntfOps.PF_DACIsr = PF_DACIsr;
        /*set CBM Zorder*/
        s_stIntfOps.PF_CBM_MovTOP = PF_CBM_MovTOP;
        s_stIntfOps.PF_VDP_RegSave = PF_VDP_RegSave;
        s_stIntfOps.PF_VDP_RegReStore = PF_VDP_RegReStore;
        s_stIntfOps.PF_DATE_SetCoef = PF_DATE_SetCoef;

    }
    else
    {
        DISP_ERROR("Not support version : %x %x\n", 
            s_stVersion.u32VersionPartH, s_stVersion.u32VersionPartL);
    }

    s_DispIntfFlag++;

    return HI_SUCCESS;
}

HI_S32 DISP_HAL_DeInit(HI_VOID)
{
#ifndef __DISP_PLATFORM_BOOT__
    DISP_OS_MMZ_Release( &g_RegBackBuf);
#endif
    DISP_CloseClock();    
    s_DispIntfFlag = 0;
    return HI_SUCCESS;
}

HI_S32 DISP_HAL_GetOperation(DISP_INTF_OPERATION_S *pstFunction)
{
    if(s_DispIntfFlag < 0)
    {
        DISP_ERROR("DISP_INTF Not inited\n");
        return HI_FAILURE;
    }
    
    memcpy(pstFunction, &s_stIntfOps, sizeof(DISP_INTF_OPERATION_S));

    return HI_SUCCESS;
}

DISP_INTF_OPERATION_S *DISP_HAL_GetOperationPtr(HI_VOID)
{
    if(s_DispIntfFlag < 0)
    {
        DISP_ERROR("DISP_INTF Not inited\n");
        return HI_NULL;
    }

    return &s_stIntfOps;
}

HI_S32 DISP_HAL_GetVersion(HI_DRV_DISP_VERSION_S *pstVersion)
{
    if(s_DispIntfFlag < 0)
    {
        DISP_ERROR("DISP_INTF Not inited\n");
        return HI_FAILURE;
    }

    if (pstVersion)
    {
        *pstVersion = s_stVersion;
        return HI_SUCCESS;
    }
    else
    {
        return HI_FAILURE;
    }
}


HI_S32 DISP_DEBUG_PrintPtr(HI_VOID)
{
    HI_S32 i;
    HI_U32 *ptrf;

    ptrf = (HI_U32 *) & s_stIntfOps;

    for(i=0; i<(sizeof(s_stIntfOps)/sizeof(HI_U32)); i++)
    {
        DISP_WARN("i=%d, v=0x%x\n", i, ptrf[i]);
    }


    return HI_SUCCESS;
}




#ifdef __cplusplus
 #if __cplusplus
}
 #endif
#endif /* __cplusplus */


