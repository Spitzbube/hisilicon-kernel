#ifndef __HDMI_EXT_H__
#define __HDMI_EXT_H__
#include "hi_unf_hdmi.h"
#include "hi_drv_hdmi.h"
#include "hi_drv_disp.h"
#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

HI_S32 HDMI_DRV_ModInit(HI_VOID);
HI_VOID HDMI_DRV_ModExit(HI_VOID);


typedef HI_S32  (*FN_HDMI_Init)(HI_VOID);
typedef HI_VOID (*FN_HDMI_Deinit)(HI_VOID);
typedef HI_S32  (*FN_HDMI_Open)(HI_UNF_HDMI_ID_E enHdmi);
typedef HI_S32  (*FN_HDMI_Close)(HI_UNF_HDMI_ID_E enHdmi);

typedef HI_S32  (*FN_HDMI_GetPlayStus)(HI_UNF_HDMI_ID_E enHdmi, HI_U32 *pu32Stutus);
typedef HI_S32  (*FN_HDMI_GetAoAttr)(HI_UNF_HDMI_ID_E enHdmi, HDMI_AUDIO_ATTR_S *pstHDMIAOAttr);
typedef HI_S32  (*FN_HDMI_GetSinkCapability)(HI_UNF_HDMI_ID_E enHdmi, HI_UNF_HDMI_SINK_CAPABILITY_S *pstSinkCap);
typedef HI_S32  (*FN_HDMI_GetAudioCapability)(HI_UNF_HDMI_ID_E enHdmi, HDMI_AUDIO_CAPABILITY_S *pstAudCap);
typedef HI_S32  (*FN_HDMI_AudioChange)(HI_UNF_HDMI_ID_E enHdmi, HDMI_AUDIO_ATTR_S *pstHDMIAOAttr);

typedef HI_S32  (*FN_HDMI_PreFormat)(HI_UNF_HDMI_ID_E enHdmi, HI_DRV_DISP_FMT_E enEncodingFormat);
typedef HI_S32  (*FN_HDMI_SetFormat)(HI_UNF_HDMI_ID_E enHdmi, HI_DRV_DISP_FMT_E enFmt, HI_DRV_DISP_STEREO_E enStereo);
//typedef HI_S32  (*FN_HDMI_Set3DMode)(HI_UNF_HDMI_ID_E enHdmi, HI_BOOL b3DEnable,HI_U8 u83Dmode);


typedef struct
{
    FN_HDMI_Init                pfnHdmiInit;
    FN_HDMI_Deinit              pfnHdmiDeinit;
    FN_HDMI_Open                pfnHdmiOpen;
    FN_HDMI_Close               pfnHdmiClose;
    FN_HDMI_GetPlayStus         pfnHdmiGetPlayStus;
    FN_HDMI_GetAoAttr           pfnHdmiGetAoAttr;
    FN_HDMI_GetSinkCapability   pfnHdmiGetSinkCapability;
    FN_HDMI_GetAudioCapability  pfnHdmiGetAudioCapability;
    FN_HDMI_AudioChange         pfnHdmiAudioChange;
    FN_HDMI_PreFormat           pfnHdmiPreFormat;
    FN_HDMI_SetFormat           pfnHdmiSetFormat;
//  FN_HDMI_Set3DMode           pfnHdmiSet3DMode;
}HDMI_EXPORT_FUNC_S;




#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __DISP_EXT_H__ */

