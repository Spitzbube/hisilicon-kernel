
#ifndef __HI_DRV_AO_EXT_H__
#define __HI_DRV_AO_EXT_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

typedef HI_S32 (*FN_AIAO_todofunc)(HI_VOID);

typedef struct 
{       
    FN_AIAO_todofunc pfnAiaotodofunc;         
} AIAO_EXPORT_FUNC_S;

HI_S32 AIAO_DRV_ModInit(HI_VOID);
HI_VOID AIAO_DRV_ModExit(HI_VOID);

#if defined(HI_MCE_SUPPORT)
HI_S32  HI_DRV_AO_Init(HI_VOID);
HI_VOID HI_DRV_AO_DeInit(HI_VOID);
HI_S32  HI_DRV_AO_SND_Init(HI_VOID);
HI_S32  HI_DRV_AO_SND_DeInit(HI_VOID);
HI_S32  HI_DRV_AO_SND_GetDefaultOpenAttr(HI_UNF_SND_ATTR_S *pstAttr);
HI_S32  HI_DRV_AO_SND_Open(HI_UNF_SND_E enSound, HI_UNF_SND_ATTR_S *pstAttr);
HI_S32  HI_DRV_AO_SND_Close(HI_UNF_SND_E enSound);
HI_S32  HI_DRV_AO_SND_SetVolume(HI_UNF_SND_E enSound, HI_UNF_SND_OUTPUTPORT_E enOutPort, HI_UNF_SND_GAIN_ATTR_S stGain);
HI_S32  HI_DRV_AO_Track_GetDefaultOpenAttr(HI_UNF_SND_TRACK_TYPE_E enTrackType, HI_UNF_AUDIOTRACK_ATTR_S *pstAttr);
HI_S32  HI_DRV_AO_Track_Create(HI_UNF_SND_E enSound, HI_UNF_AUDIOTRACK_ATTR_S *pstAttr, HI_HANDLE *phTrack);
HI_S32  HI_DRV_AO_Track_Destroy(HI_HANDLE hSndTrack);
HI_S32  HI_DRV_AO_Track_Flush(HI_HANDLE hSndTrack);
HI_S32  HI_DRV_AO_Track_Start(HI_HANDLE hSndTrack);
HI_S32  HI_DRV_AO_Track_Stop(HI_HANDLE hSndTrack);
HI_S32  HI_DRV_AO_Track_GetDelayMs(HI_HANDLE hSndTrack, HI_U32 *pDelayMs);
HI_S32  HI_DRV_AO_Track_SendData(HI_HANDLE hSndTrack, HI_UNF_AO_FRAMEINFO_S *pstAOFrame);
#endif
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif//__HI_DRV_AO_EXT_H__