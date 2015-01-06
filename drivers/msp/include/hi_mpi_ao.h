/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

******************************************************************************
  File Name             :   hi_mpi_ao.h
  Version               :   Initial Draft
  Author                :   Hisilicon multimedia software group
  Created               :   2012/12/20
  Last Modified         :
  Description           :
  Function List         :
  History               :
  1.Date                :   2012/12/20
    Author              :   z40717
Modification            :   Created file
******************************************************************************/

#ifndef  __MPI_AO_H__
#define  __MPI_AO_H__

#include "hi_type.h"
#include "hi_unf_sound.h"

#ifdef __cplusplus
 #if __cplusplus
extern "C" {
 #endif
#endif /* End of #ifdef __cplusplus */

 /* the type of Adjust Audio */
typedef enum hiHI_MPI_SND_SPEEDADJUST_TYPE_E
{
 HI_MPI_AO_SND_SPEEDADJUST_SRC,     /**<samplerate convert */
 HI_MPI_AO_SND_SPEEDADJUST_PITCH,   /**<Sola speedadjust, reversed */
 HI_MPI_AO_SND_SPEEDADJUST_MUTE,    /**<mute */
 HI_MPI_AO_SND_SPEEDADJUST_BUTT 
} HI_MPI_SND_SPEEDADJUST_TYPE_E;

/******************************* MPI for UNF Sound Init *****************************/
HI_S32   HI_MPI_AO_Init(HI_VOID);
HI_S32   HI_MPI_AO_DeInit(HI_VOID);

/******************************* MPI for UNF_SND *****************************/
HI_S32   HI_MPI_AO_SND_GetDefaultOpenAttr(HI_UNF_SND_ATTR_S *pstAttr);
HI_S32   HI_MPI_AO_SND_Open(HI_UNF_SND_E enSound, const HI_UNF_SND_ATTR_S *pstAttr);
HI_S32   HI_MPI_AO_SND_Close(HI_UNF_SND_E enSound);
HI_S32   HI_MPI_AO_SND_SetMute(HI_UNF_SND_E enSound, HI_UNF_SND_OUTPUTPORT_E enOutPort, HI_BOOL bMute);
HI_S32   HI_MPI_AO_SND_GetMute(HI_UNF_SND_E enSound, HI_UNF_SND_OUTPUTPORT_E enOutPort, HI_BOOL *pbMute);
HI_S32   HI_MPI_AO_SND_SetHdmiMode(HI_UNF_SND_E enSound, HI_UNF_SND_OUTPUTPORT_E enOutPort,
                                              HI_UNF_SND_HDMI_MODE_E enHdmiMode);
HI_S32   HI_MPI_AO_SND_GetHdmiMode(HI_UNF_SND_E enSound, HI_UNF_SND_OUTPUTPORT_E enOutPort,
                                              HI_UNF_SND_HDMI_MODE_E *penHdmiMode);
HI_S32   HI_MPI_AO_SND_SetSpdifMode(HI_UNF_SND_E enSound, HI_UNF_SND_OUTPUTPORT_E enOutPort,
                                              HI_UNF_SND_SPDIF_MODE_E enSpdifMode);
HI_S32   HI_MPI_AO_SND_GetSpdifMode(HI_UNF_SND_E enSound, HI_UNF_SND_OUTPUTPORT_E enOutPort,
                                              HI_UNF_SND_SPDIF_MODE_E *penSpdifMode);
HI_S32   HI_MPI_AO_SND_SetVolume(HI_UNF_SND_E enSound, HI_UNF_SND_OUTPUTPORT_E enOutPort,
                                 const HI_UNF_SND_GAIN_ATTR_S *pstGain);
HI_S32   HI_MPI_AO_SND_GetVolume(HI_UNF_SND_E enSound, HI_UNF_SND_OUTPUTPORT_E enOutPort,
                                 HI_UNF_SND_GAIN_ATTR_S *pstGain);
HI_S32   HI_MPI_AO_SND_SetSampleRate(HI_UNF_SND_E enSound, HI_UNF_SAMPLE_RATE_E enSampleRate);
HI_S32   HI_MPI_AO_SND_GetSampleRate(HI_UNF_SND_E enSound, HI_UNF_SAMPLE_RATE_E *penSampleRate);
HI_S32   HI_MPI_AO_SND_SetTrackMode(HI_UNF_SND_E enSound, HI_UNF_SND_OUTPUTPORT_E enOutPort, HI_UNF_TRACK_MODE_E enMode);
HI_S32   HI_MPI_AO_SND_GetTrackMode(HI_UNF_SND_E enSound, HI_UNF_SND_OUTPUTPORT_E enOutPort,
                                    HI_UNF_TRACK_MODE_E *penMode);
HI_S32   HI_MPI_AO_SND_SetSmartVolume(HI_UNF_SND_E enSound, HI_UNF_SND_OUTPUTPORT_E enOutPort, HI_BOOL bSmartVolume);
HI_S32   HI_MPI_AO_SND_GetSmartVolume(HI_UNF_SND_E enSound, HI_UNF_SND_OUTPUTPORT_E enOutPort, HI_BOOL *pbSmartVolume);

HI_S32   HI_MPI_AO_SND_GetCastDefaultOpenAttr(HI_UNF_SND_CAST_ATTR_S *pstAttr);
HI_S32   HI_MPI_AO_SND_CreateCast(HI_UNF_SND_E enSound, HI_UNF_SND_CAST_ATTR_S *pstCastAttr, HI_HANDLE *phCast);
HI_S32   HI_MPI_AO_SND_DestroyCast(HI_HANDLE hCast);
HI_S32   HI_MPI_AO_SND_SetCastEnable(HI_HANDLE hCast, HI_BOOL bEnable);
HI_S32   HI_MPI_AO_SND_GetCastEnable(HI_HANDLE hCast, HI_BOOL *pbEnable);
HI_S32   HI_MPI_AO_SND_AcquireCastFrame(HI_HANDLE hCast, HI_UNF_AO_FRAMEINFO_S *pstCastFrame);
HI_S32   HI_MPI_AO_SND_ReleaseCastFrame(HI_HANDLE hCast, HI_UNF_AO_FRAMEINFO_S *pstCastFrame);


//HI_S32   HI_MPI_AO_SND_AttachTrack(HI_UNF_SND_E enSound, HI_HANDLE hTrack);   //HI_MPI_AO_TRACK_AttachSnd
//HI_S32   HI_MPI_AO_SND_DetachTrack(HI_UNF_SND_E enSound, HI_HANDLE hTrack);

/******************************* MPI Track for UNF_SND/UNF_Mixer *****************************/
HI_S32   HI_MPI_AO_Track_GetDefaultOpenAttr(HI_UNF_SND_TRACK_TYPE_E enTrackType, HI_UNF_AUDIOTRACK_ATTR_S *pstAttr);
HI_S32   HI_MPI_AO_Track_GetAttr(HI_HANDLE hTrack, HI_UNF_AUDIOTRACK_ATTR_S    *pstAttr);
HI_S32   HI_MPI_AO_Track_SetAttr(HI_HANDLE hTrack, const HI_UNF_AUDIOTRACK_ATTR_S    *pstAttr);
HI_S32   HI_MPI_AO_Track_Create(HI_UNF_SND_E enSound, const HI_UNF_AUDIOTRACK_ATTR_S *pstAttr, HI_HANDLE *phTrack);
HI_S32   HI_MPI_AO_Track_Destroy(HI_HANDLE hTrack);
HI_S32   HI_MPI_AO_Track_Start(HI_HANDLE hTrack); 
HI_S32   HI_MPI_AO_Track_Stop(HI_HANDLE hTrack);  
HI_S32   HI_MPI_AO_Track_Pause(HI_HANDLE hTrack);
HI_S32   HI_MPI_AO_Track_Flush(HI_HANDLE hTrack);
HI_S32   HI_MPI_AO_Track_SendData(HI_HANDLE hTrack, const HI_UNF_AO_FRAMEINFO_S *pstAOFrame);
HI_S32   HI_MPI_AO_Track_SetWeight(HI_HANDLE hTrack, const HI_UNF_SND_GAIN_ATTR_S *pstTrackGain);
HI_S32   HI_MPI_AO_Track_GetWeight(HI_HANDLE hTrack, HI_UNF_SND_GAIN_ATTR_S* pstTrackGain);
#define  HI_MPI_AO_Track_Resume HI_MPI_AO_Track_Start


// HI_UNF_SND_TRACK_TYPE_VIRTUAL only
HI_S32   HI_MPI_AO_Track_AcquireFrame(HI_HANDLE hTrack, HI_UNF_AO_FRAMEINFO_S *pstAOFrame);
HI_S32   HI_MPI_AO_Track_ReleaseFrame(HI_HANDLE hTrack, HI_UNF_AO_FRAMEINFO_S *pstAOFrame);

/******************************* MPI Track for MPI_AVPlay only **********************/
HI_S32   HI_MPI_AO_Track_SetEosFlag(HI_HANDLE hTrack, HI_BOOL bEosFlag);
HI_S32   HI_MPI_AO_Track_SetSpeedAdjust(HI_HANDLE hTrack, HI_S32 s32Speed, HI_MPI_SND_SPEEDADJUST_TYPE_E enType);
HI_S32   HI_MPI_AO_Track_GetDelayMs(const HI_HANDLE hTrack, HI_U32 *pDelayMs);
HI_S32   HI_MPI_AO_Track_IsBufEmpty(const HI_HANDLE hTrack, HI_BOOL *pbEmpty);

#ifdef __cplusplus
 #if __cplusplus
}
 #endif
#endif /* End of #ifdef __cplusplus */

#endif //__MPI_AO_H__
