
/******************************************************************************
  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.
******************************************************************************
File Name     : drv_disp_hal.h
Version       : Initial Draft
Author        : Hisilicon multimedia software group
Created       : 2012/12/30
Last Modified :
Description   :
Function List :
History       :
******************************************************************************/
#ifndef __DRV_DISP_HAL_H__
#define __DRV_DISP_HAL_H__

#include "hi_drv_disp.h"
#include "drv_disp_version.h"
#include "vdp_define.h"
#include "vdp_driver.h"



#ifdef __cplusplus
 #if __cplusplus
extern "C" {
 #endif
#endif /* __cplusplus */

#define DISP_VDAC_MAX_NUMBER 6
#define DISP_VENC_SIGNAL_MAX_NUMBER 6
#define VDAC_STATE_INT 0xf000

typedef enum tagDISP_INTF_TYPE_E
{
    DISP_INTF_TYPE_VDAC  = 0,

    DISP_INTF_TYPE_BT656,
    DISP_INTF_TYPE_BT1120,
    DISP_INTF_TYPE_LCD,
    DISP_INTF_TYPE_HDMI,
    DISP_INTF_TYPE_HVENC,
    DISP_INTF_TYPE_SVENC,
    DISP_INTF_TYPE_MIRACAST,
    DISP_INTF_TYPE_BUTT
}DISP_INTF_TYPE_E;

typedef enum tagDISP_WBC_E
{
    DISP_WBC_00 = 0,
    DISP_WBC_BUTT
}DISP_WBC_E;


typedef enum tagDISP_INTERRUPT_E
{
    DISP_INTERRUPT_NONE = 0,
    DISP_INTERRUPT_D0_0_PERCENT  = 0x10,
    DISP_INTERRUPT_D0_90_PERCENT = 0x20,
    DISP_INTERRUPT_D0_RES        = 0x40,
    DISP_INTERRUPT_D0_UFINT      = 0x80,

    DISP_INTERRUPT_D1_0_PERCENT  = 0x1,
    DISP_INTERRUPT_D1_90_PERCENT = 0x2,
    DISP_INTERRUPT_D1_RES        = 0x4,
    DISP_INTERRUPT_D1_UFINT      = 0x8,

    DISP_INTERRUPT_WBCDHD_PARTFNI  = 0x10000000,

    DISP_INTERRUPT_GP1_RES       = 0x100,
    DISP_INTERRUPT_MC1_UFINT     = 0x200,

    DISP_INTERRUPT_ALL           = 0xFFFFFFFF,
}DISP_INTERRUPT_E;

typedef enum tagDISP_PLL_SOURCE_E
{
    DISP_CLOCK_SOURCE_SD0 = 0,
    DISP_CLOCK_SOURCE_HD0  , //1
    DISP_CLOCK_SOURCE_HD1, //2
    DISP_CLOCK_SOURCE_BUTT
}DISP_PLL_SOURCE_E;
typedef enum tagDISP_CHANNEL_E
{
    DISP_CHANNEL_DHD0 = 0,
    DISP_CHANNEL_DHD1  ,
    DISP_CHANNEL_BUTT
}DISP_CHANNEL_E;

typedef enum tagDISP_HDMI_DATA_E
{
    DISP_HDMI_DATA_YUV = 0,
    DISP_HDMI_DATA_RGB  ,
    DISP_HDMI_DATA_BUTT
}DISP_HDMI_DATA_E;
#define HDMI_MODE_YUV  0
#define HDMI_MODE_RGB  1
typedef struct tagDISP_INTF_S
{
    HI_BOOL bOpen; //0

    HI_BOOL bLinkVenc; //4
    DISP_VENC_E eVencId; //8

    HI_DRV_DISP_INTF_S stIf; //12

    // signal index is VDACID
    //HI_DRV_DISP_VDAC_SIGNAL_E eSignal[DISP_VENC_SIGNAL_MAX_NUMBER];
    DISP_CHANNEL_E enDispChan;
    DISP_PLL_SOURCE_E enDispPll;
    DISP_HDMI_DATA_E enHDMIDataType;
    //36
}DISP_INTF_S;

typedef struct tagDISP_HAL_ENCFMT_PARAM_S
{
    // display into
    HI_DRV_DISP_FMT_E eFmt;
    HI_DRV_DISP_STEREO_E eDispMode;
    HI_BOOL bInterlace;

    HI_RECT_S stOrgRect;
    HI_RECT_S stRefRect;
    HI_DRV_ASPECT_RATIO_S stAR;

    HI_U32    u32RefreshRate;  /* in 1/100 Hz */

    HI_DRV_COLOR_SPACE_E enColorSpace;

}DISP_HAL_ENCFMT_PARAM_S;

typedef struct tagDISP_FMT_CFG_S
{
    VDP_DISP_SYNCINFO_S stTiming; //0
    DISP_PLL_SOURCE_E enPllIndex; //72
    HI_U32 u32Pll[2];
    DISP_HAL_ENCFMT_PARAM_S stInfo;
    //144
}DISP_FMT_CFG_S;


typedef struct tagDISP_VTTHD_CFG_S
{
    HI_U32 uVt1thdFieldMode;  // 0, frame; 1, field;
    HI_U32 uVt1thd;
    HI_U32 uVt2thdFieldMode;  // 0, frame; 1, field;
    HI_U32 uVt2thd;
}DISP_VTTHD_CFG_S;


typedef struct tagDISP_CAPA_S
{
    HI_BOOL bSupport; //0
    HI_BOOL bWbc; //4
    HI_BOOL bHD; //8
    //12
}DISP_CAPA_S;


typedef struct tagDISP_INTF_SETTING_S
{
    HI_DRV_DISP_FMT_E eFmt;

    // signal index is VDACID
    HI_DRV_DISP_VDAC_SIGNAL_E eSignal[DISP_VENC_SIGNAL_MAX_NUMBER];

}DISP_INTF_SETTING_S;


#if 0
typedef struct tagDISP_VENC_SETTING_S
{
    HI_DRV_DISP_FMT_E eFmt;

    // signal index is VDACID
    HI_DRV_DISP_VDAC_SIGNAL_E eSignal[DISP_VENC_SIGNAL_MAX_NUMBER];

}DISP_VENC_SETTING_S;

typedef struct tagDISP_HDMI_SETTING_S
{
    HI_DRV_COLOR_SPACE_E eColorSpace;
    HI_U32 u32DataWidth;  /* 8/10/12 bits */

}DISP_HDMI_SETTING_S;


typedef struct tagDISP_VDAC_SETTING_S
{
    HI_DRV_DISP_VDAC_SIGNAL_E eSignal;

}DISP_VDAC_SETTING_S;
#endif


typedef struct tagDISP_LOCAL_INTF_S
{
    HI_BOOL bSupport;
    HI_BOOL bIdle;
    HI_DRV_DISPLAY_E enChan;
    //12
}DISP_LOCAL_INTF_S;

typedef struct tagDISP_LOCAL_VENC_S
{
    HI_BOOL bSupport;
    HI_BOOL bIdle;
}DISP_LOCAL_VENC_S;

typedef struct tagDISP_LOCAL_VDAC_S
{
    HI_BOOL bSupport;
    HI_BOOL bIdle;
    //8
}DISP_LOCAL_VDAC_S;

typedef struct tagDISP_LOCAL_WBC_S
{
    HI_BOOL bSupport;
    HI_BOOL bIdle;
    HI_U32 u32RefCnt;
}DISP_LOCAL_WBC_S;

typedef struct tagDISP_HAL_COLOR_S
{
    HI_DRV_COLOR_SPACE_E enInputCS;
    HI_DRV_COLOR_SPACE_E enOutputCS;

    HI_U32 u32Bright;      //bright adjust value,range[0,100],default setting 50; 
    HI_U32 u32Contrst;     //contrast adjust value,range[0,100],default setting 50;
    HI_U32 u32Hue;         //hue adjust value,range[0,100],default setting 50;
    HI_U32 u32Satur;       //saturation adjust value,range[0,100],default setting 50;
    HI_U32 u32Kr;          //red component gain adjust value for color temperature adjust,range[0,100],default setting 50;
    HI_U32 u32Kg;          //green component gain adjust value for color temperature adjust,range[0,100],default setting 50;
    HI_U32 u32Kb;          //blue component gain adjust value for color temperature adjust,range[0,100],default setting 50;

    HI_BOOL bGammaEnable;
    HI_BOOL bUseCustGammaTable; 
}DISP_HAL_COLOR_S;

typedef struct tagDISP_CH_CFG_S
{
    HI_BOOL bLinkVenc;
    DISP_VENC_E enVenc;
    HI_U32 RESERVE;
}DISP_CH_CFG_S;


typedef struct
{
	int Data_0; //0
	struct
	{
		HI_U32 Data_0; //0
		HI_U32 Data_4; //4
		//8
	} Data_81141c24[4]; //4
	//36
} Struct_81141c20;


typedef struct tagDISP_INTF_OPERATION_S
{
    /* usual */
    /* reset vdp */
    HI_S32 (*PF_ResetHardware)(HI_VOID); //0
    HI_S32 (*PF_CloseClkResetModule)(HI_VOID); //4
    
    /* Display config */
    HI_BOOL (* PF_TestChnSupport)(HI_DRV_DISPLAY_E eChn); //8
    HI_BOOL (* PF_TestChnSupportHD)(HI_DRV_DISPLAY_E eChn); //12
    HI_BOOL (* PF_TestIntfSupport)(HI_DRV_DISPLAY_E eChn, HI_DRV_DISP_INTF_ID_E eIntf); //16
    HI_BOOL (* PF_TestChnSupportCast)(HI_DRV_DISPLAY_E eChn); //20
    HI_BOOL (* PF_TestChnEncFmt)(HI_DRV_DISPLAY_E eChn, HI_DRV_DISP_FMT_E eFmt); //24
    HI_BOOL (* PF_TestChnAttach)(HI_DRV_DISPLAY_E enM, HI_DRV_DISPLAY_E enS); //28

    HI_S32 (* PF_SetChnFmt)(HI_DRV_DISPLAY_E eChn, HI_DRV_DISP_FMT_E eFmt, HI_DRV_DISP_STEREO_E enStereo); //32
    HI_S32 (* PF_SetChnTiming)(HI_DRV_DISPLAY_E eChn, HI_DRV_DISP_TIMING_S *pstTiming); //36

    HI_S32 (* PF_SetChnPixFmt)(HI_DRV_DISPLAY_E eChn, HI_DRV_PIX_FORMAT_E ePix); //40
    HI_S32 (* PF_SetChnBgColor)(HI_DRV_DISPLAY_E eChn, HI_DRV_COLOR_SPACE_E enCS, HI_DRV_DISP_COLOR_S *pstBGC); //44
    HI_S32 (* PF_SetChnColor)(HI_DRV_DISPLAY_E eChn, DISP_HAL_COLOR_S *pstColor); //48
    //HI_S32 (* PF_SetDispSignal)(HI_DRV_DISPLAY_E eChn, HI_U32 index, HI_DRV_DISP_VDAC_SIGNAL_E eSignal);

    HI_S32 (* PF_SetChnEnable)(HI_DRV_DISPLAY_E eChn, HI_BOOL bEnalbe); //52
    HI_S32 (* PF_GetChnEnable)(HI_DRV_DISPLAY_E eChn, HI_BOOL *pbEnalbe); //56
    HI_S32 (* PF_InitDacDetect)(HI_DRV_DISP_INTF_S * p); //60
    HI_S32 (* PF_SetMSChnEnable)(HI_DRV_DISPLAY_E eChnM, HI_DRV_DISPLAY_E eChnS, HI_U32 u32DelayMs, HI_BOOL bEnalbe); //64
    HI_S32 (* PF_UpdateGamma)(void/*???*/); //68
    HI_S32 (* PF_SetGammaCtrl)(void/*???*/); //72
    HI_S32 (* PF_SetGammaRWZBCtrl)(void/*???*/); //76
    
    /* interrup */
    HI_S32 (* PF_SetIntEnable)(HI_U32 u32Int, HI_BOOL bEnable); //80
    HI_S32 (* PF_SetIntDisable)(HI_U32 u32Int);

    HI_S32 (* PF_GetIntSetting)(HI_U32 *pu32IntSetting); //88

    HI_S32 (* PF_GetMaskedIntState)(HI_U32 *pu32State); //92
    HI_S32 (* PF_GetUnmaskedIntState)(HI_U32 *pu32State); //96
    HI_S32 (* PF_CleanIntState)(HI_U32 u32State); //100
    HI_U32 (* FP_GetChnIntState)(HI_DRV_DISPLAY_E enDisp, HI_U32 u32IntState); //104
    HI_U32 (* FP_GetChnBottomFlag)(HI_DRV_DISPLAY_E enDisp,  HI_BOOL *pbBtm, HI_U32 *pu32Vcnt); //108

    /* intf manage */
    HI_S32 (* PF_AcquireIntf2)(HI_DRV_DISPLAY_E enDisp, DISP_INTF_S *pstIf); //112
    HI_S32 (* PF_ReleaseIntf2)(HI_DRV_DISPLAY_E enDisp, DISP_INTF_S *pstIf); //116
    HI_S32 (* PF_ResetIntfFmt2)(HI_DRV_DISPLAY_E enDisp, DISP_INTF_S *pstIntf, HI_DRV_DISP_FMT_E eFmt, HI_DRV_DISP_TIMING_S* p); //120
    HI_S32 (* PF_SetIntfEnable2)(HI_DRV_DISPLAY_E enDisp, DISP_INTF_S *pstIntf, HI_BOOL bEnable); //124

    /* WBC manager */
    HI_S32 (* PF_AcquireWbcByChn)(HI_DRV_DISPLAY_E eChn, DISP_WBC_E *peWbc); //128
    HI_S32 (* PF_AcquireWbc)(DISP_WBC_E eWbc); //132
    HI_S32 (* PF_ReleaseWbc)(DISP_WBC_E eWbc); //136

    /* WBC */
    HI_S32 (*PF_SetWbcIORect)(DISP_WBC_E eWbc, HI_DISP_DISPLAY_INFO_S *pstDispInfo, HI_RECT_S *in, HI_RECT_S *out); //140
    HI_S32 (*PF_SetWbc3DInfo)(DISP_WBC_E eWbc, HI_DISP_DISPLAY_INFO_S *pstDispInfo, HI_RECT_S *in); //144
    HI_S32 (*PF_SetWbcColorSpace)(DISP_WBC_E eWbc, HI_DRV_COLOR_SPACE_E eSrcCS, HI_DRV_COLOR_SPACE_E eDstCS); //148
    HI_S32 (*PF_SetWbcPixFmt)(DISP_WBC_E eWbc, HI_DRV_PIX_FORMAT_E eFmt); //152
    HI_S32 (*PF_SetWbcAddr)(DISP_WBC_E eWbc, HI_DRV_VID_FRAME_ADDR_S *pstAddr); //156
    HI_S32 (*PF_SetWbcEnable)(DISP_WBC_E eWbc, HI_BOOL bEnable); //160
    HI_S32 (*PF_SetWbcLowdlyEnable)(void); //164
    HI_S32 (*PF_SetWbcPartfnsLineNum)(void); //168
    HI_S32 (*PF_SetWbcLineNumInterval)(void); //172
    HI_S32 (*PF_SetWbcLineAddr)(void); //176

    HI_S32 (*PF_UpdateWbc)(DISP_WBC_E eWbc); //180
    HI_S32 (*PF_DACIsr)(HI_U8 u8DAC); //184
    HI_S32 (*PF_SetDACDetEn)(void); //188
    HI_S32 (*PF_GetDACAttr)(Struct_81141c20* p); //192
    /*Zorder*/
    HI_S32 (*PF_CBM_MovTOP)(HI_DRV_DISPLAY_E enDisp, HI_DRV_DISP_LAYER_E enLayer); //196
    HI_S32 (*PF_VDP_RegSave)(HI_VOID); //200
    HI_S32 (*PF_VDP_RegReStore)(HI_VOID); //204

    /* date */
    HI_S32 (*PF_DATE_SetCoef)(HI_DRV_DISPLAY_E enDisp, HI_BOOL bDefault); //208
    HI_S32 (*PF_DATE_SetIRE)(void); //212
    HI_S32 (*PF_SetAllDACEn)(void); //216
    //220
}DISP_INTF_OPERATION_S;


HI_S32 DISP_HAL_Init(HI_U32 u32Base);
HI_S32 DISP_HAL_DeInit(HI_VOID);

HI_S32 DISP_HAL_GetVersion(HI_DRV_DISP_VERSION_S *pstVersion);
HI_S32 DISP_HAL_GetOperation(DISP_INTF_OPERATION_S *pstFunction);
DISP_INTF_OPERATION_S *DISP_HAL_GetOperationPtr(HI_VOID);



HI_S32 DISP_HAL_GetEncFmtPara(HI_DRV_DISP_FMT_E eFmt, DISP_HAL_ENCFMT_PARAM_S *pstFmtPara);
HI_S32 DISP_HAL_GetFmtAspectRatio(HI_DRV_DISP_FMT_E eFmt, HI_U32 *pH, HI_U32 *pV);
HI_S32 DISP_HAL_GetFmtColorSpace(HI_DRV_DISP_FMT_E eFmt, HI_DRV_COLOR_SPACE_E  *penColorSpace);



HI_S32 DISP_DEBUG_PrintPtr(HI_VOID);


#ifdef __cplusplus
 #if __cplusplus
}
 #endif
#endif /* __cplusplus */

#endif /*  __DRV_DISP_CMP_H__  */










