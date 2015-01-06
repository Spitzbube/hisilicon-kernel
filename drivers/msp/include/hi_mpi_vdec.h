
/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name             :   hi_mpi_vdec.h
  Version               :   Initial Draft
  Author                :   Hisilicon multimedia software group
  Created               :   2006/06/12
  Last Modified         :
  Description           :
  Function List         :
  History               :
  1.Date                :   2006/06/12
    Author              :
Modification            :   Created file
******************************************************************************/

/******************************* Include Files *******************************/

#ifndef  __HI_MPI_VDEC_H__
#define  __HI_MPI_VDEC_H__

#include "hi_unf_avplay.h"
#include "hi_drv_video.h"
#include "hi_drv_vpss.h"
#include "hi_video_codec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

/****************************** Macro Definition *****************************/

#ifndef HI_VDEC_REG_CODEC_SUPPORT
#define HI_VDEC_REG_CODEC_SUPPORT (0)
#endif

#ifndef HI_VDEC_MJPEG_SUPPORT
#define HI_VDEC_MJPEG_SUPPORT (0)
#endif

#ifndef HI_VDEC_USERDATA_CC_SUPPORT
#define HI_VDEC_USERDATA_CC_SUPPORT (0)
#endif

/*************************** Structure Definition ****************************/

typedef struct hiVDEC_ES_BUF_S
{
    HI_U8*  pu8Addr;      /*Virtual address of the buffer user.*/
    HI_U32  u32BufSize;   /*Buffer size, in the unit of byte.*/
    HI_U64  u64Pts;       /*PTS of the data filled in a buffer.*/
    HI_BOOL bEndOfFrame;  /* End of Frame flag */
    HI_BOOL bDiscontinuous;/* Stream continue or not */
}VDEC_ES_BUF_S, *PTR_VDEC_ES_BUF_S;

typedef struct hiVDEC_STATUSINFO_S
{
    HI_U32  u32BufferSize;      /* Total buffer size, in the unit of byte.*/
    HI_U32  u32BufferAvailable; /* Available buffer, in the unit of byte.*/
    HI_U32  u32BufferUsed;      /* Used buffer, in the unit of byte.*/
    HI_U32  u32VfmwFrmNum;     
    HI_U32  u32VfmwStrmSize;
    HI_U32  u32VfmwStrmNum;     /* The un-decoded stream seg num produced by SCD */
    HI_U32  u32VfmwTotalDispFrmNum; /* total display num ( plus extra_disp ) */
    HI_U32  u32FieldFlag;       /* 0:frame 1:field */
    HI_UNF_VCODEC_FRMRATE_S stVfmwFrameRate;/* vfmw frame rate */
    HI_U32  u32StrmInBps;
    HI_U32  u32TotalDecFrmNum;
    HI_U32  u32TotalErrFrmNum;
    HI_U32  u32TotalErrStrmNum;
    HI_U32  u32FrameBufNum;     /* frame num in buffer to display */
    HI_BOOL bEndOfStream;       /* There's no enough stream in buffer to decode a frame */
    HI_BOOL bAllPortCompleteFrm;
}VDEC_STATUSINFO_S;

#if 1
typedef struct hiVDEC_FRMSTATUSINFO_S
{
    HI_U32  u32DecodedFrmNum;  /* decoded, but not ready to display.*/  
    HI_U32  u32StrmSize;       /* the un-decoded stream seg produced by SCD */
    HI_U32  u32StrmInBps;      /*average Bps */
    HI_U32  u32OutBufFrmNum;   /*decoded, and ready to display*/
}VDEC_FRMSTATUSINFO_S;
typedef struct tagVDEC_FRMSTATUSINFOWITHPORT
{
	HI_HANDLE hPort;
	VDEC_FRMSTATUSINFO_S stVdecFrmStatus;
}VDEC_FRMSTATUSINFOWITHPORT_S;
#endif

typedef struct hiVDEC_EVENT_S
{    
    HI_BOOL  bNewFrame;
    HI_BOOL  bNewSeq;
    HI_BOOL  bNewUserData;

    HI_BOOL  bFirstValidPts;
    HI_U32   u32FirstValidPts;
    HI_BOOL  bSecondValidPts;
    HI_U32   u32SecondValidPts;
    
    HI_BOOL  bNormChange;
    HI_UNF_NORMCHANGE_PARAM_S stNormChangeParam;

    HI_BOOL  bFramePackingChange;
    HI_UNF_VIDEO_FRAME_PACKING_TYPE_E enFramePackingType;

    HI_BOOL  bIFrameErr;
	HI_BOOL  bUnSupportStream;
} VDEC_EVENT_S;

typedef enum hiVDEC_DISCARD_MODE_E
{
    VDEC_DISCARD_STOP,      /* Don't discard frame, or stop when current status is discarding */
    VDEC_DISCARD_START,     /* Start to discard frame until mode switch to VDEC_DISCARD_STOP */
    VDEC_DISCARD_BY_NUM,    /* Discard appointed number frames */
    VDEC_DISCARD_BUTT
}VDEC_DISCARD_MODE_E;

typedef enum hiVDEC_PORT_ABILITY_E
{
	VDEC_PORT_HD, 
	VDEC_PORT_SD,
	VDEC_PORT_STR,/*virtual window*/
	VDEC_PORT_BUTT
}VDEC_PORT_ABILITY_E;
typedef struct hiVDEC_PORT_CFG_S
{
    HI_HANDLE *phPort;
    VDEC_PORT_ABILITY_E ePortAbility;	
}VDEC_PORT_CFG_S;

typedef struct hiVDEC_DISCARD_FRAME_S
{
    VDEC_DISCARD_MODE_E enMode; /* Discard mode */
    HI_U32              u32Num; /* Discard number, usable when mode is VDEC_DISCARD_BY_NUM */
}VDEC_DISCARD_FRAME_S;

//add by l00225186
typedef int (*PFN_VDEC_Chan_VOAcqFrame)(HI_HANDLE, HI_DRV_VIDEO_FRAME_S*);
typedef int (*PFN_VDEC_Chan_VORlsFrame)(HI_HANDLE, HI_DRV_VIDEO_FRAME_S*);
typedef int (*PFN_VDEC_Chan_VOChangeWinInfo)(HI_HANDLE,HI_DRV_WIN_PRIV_INFO_S*);
//add by l00225186
typedef struct tagVDEC_PORT_PARAM_S
{
   /*提供给VO,收帧，释放帧，处理窗口信息改变的函数*/
  PFN_VDEC_Chan_VOAcqFrame pfVOAcqFrame;
  PFN_VDEC_Chan_VORlsFrame pfVORlsFrame;
  PFN_VDEC_Chan_VOChangeWinInfo pfVOSendWinInfo;
}VDEC_PORT_PARAM_S;
typedef struct tagVDEC_PORT_PARAM_WITHPORT_S
{
  HI_HANDLE hPort;
  VDEC_PORT_PARAM_S stVdecPortParam;
}VDEC_PORT_PARAM_WITHPORT_S;
typedef enum hiVDEC_PORT_TYPE_E
{
    VDEC_PORT_TYPE_MASTER,
    VDEC_PORT_TYPE_SLAVE,
    VDEC_PORT_TYPE_VIRTUAL,
    VDEC_PORT_TYPE_BUTT
}VDEC_PORT_TYPE_E;

typedef struct tagVDEC_PORT_TYPE_WITHPORT_S
{
  HI_HANDLE hPort;
  VDEC_PORT_TYPE_E enPortType;
}VDEC_PORT_TYPE_WITHPORT_S;

typedef struct tagVDEC_PORT_ATTR_WITHHANDLE_S
{
    HI_HANDLE                   hPort;
    HI_DRV_VPSS_PORT_CFG_S      stPortCfg;
}VDEC_PORT_ATTR_WITHHANDLE_S;

/****************************** API Declaration ******************************/

HI_S32 HI_MPI_VDEC_Init(HI_VOID);
HI_S32 HI_MPI_VDEC_DeInit(HI_VOID);
HI_S32 HI_MPI_VDEC_RegisterVcodecLib(const HI_CHAR *pszCodecDllName);
HI_S32 HI_MPI_VDEC_AllocChan(HI_HANDLE *phHandle, const HI_UNF_AVPLAY_OPEN_OPT_S *pstMaxCapbility);
HI_S32 HI_MPI_VDEC_FreeChan(HI_HANDLE hVdec);
HI_S32 HI_MPI_VDEC_SetChanAttr(HI_HANDLE hVdec, const HI_UNF_VCODEC_ATTR_S *pstAttr);
HI_S32 HI_MPI_VDEC_GetChanAttr(HI_HANDLE hVdec, HI_UNF_VCODEC_ATTR_S *pstAttr);
HI_S32 HI_MPI_VDEC_ChanBufferInit(HI_HANDLE hVdec, HI_U32 u32BufSize, HI_HANDLE hDmxVidChn);
HI_S32 HI_MPI_VDEC_ChanBufferDeInit(HI_HANDLE hVdec);
HI_S32 HI_MPI_VDEC_ResetChan(HI_HANDLE hVdec);
HI_S32 HI_MPI_VDEC_ChanStart(HI_HANDLE hVdec);
HI_S32 HI_MPI_VDEC_ChanStop(HI_HANDLE hVdec);
HI_S32 HI_MPI_VDEC_ChanGetBuffer(HI_HANDLE hVdec, HI_U32 u32RequestSize, VDEC_ES_BUF_S *pstBuf);
HI_S32 HI_MPI_VDEC_ChanPutBuffer(HI_HANDLE hVdec, const VDEC_ES_BUF_S *pstBuf);
HI_S32 HI_MPI_VDEC_ChanRecvFrm(HI_HANDLE hVdec, HI_DRV_VIDEO_FRAME_S* pstFrameInfo);
HI_S32 HI_MPI_VDEC_ChanRlsFrm(HI_HANDLE hVdec, HI_DRV_VIDEO_FRAME_S* pstFrameInfo);
HI_S32 HI_MPI_VDEC_ChanIFrameDecode(HI_HANDLE hVdec, HI_UNF_AVPLAY_I_FRAME_S *pstIFrameStream,
                                    HI_DRV_VIDEO_FRAME_S *pstVoFrameInfo, HI_BOOL bCapture);
HI_S32 HI_MPI_VDEC_ChanIFrameRelease(HI_HANDLE hVdec, HI_DRV_VIDEO_FRAME_S *pstVoFrameInfo);
HI_S32 HI_MPI_VDEC_GetChanStatusInfo(HI_HANDLE hVdec, VDEC_STATUSINFO_S *pstStatusInfo);
HI_S32 HI_MPI_VDEC_GetChanStreamInfo(HI_HANDLE hVdec, HI_UNF_VCODEC_STREAMINFO_S *pstStreamInfo);
HI_S32 HI_MPI_VDEC_CheckNewEvent(HI_HANDLE hVdec, VDEC_EVENT_S *pstNewEvent);
HI_S32 HI_MPI_VDEC_ReadNewFrame(HI_HANDLE hVdec, HI_DRV_VIDEO_FRAME_S *pstNewFrame);
HI_S32 HI_MPI_VDEC_ChanRecvUsrData(HI_HANDLE hVdec, HI_UNF_VIDEO_USERDATA_S *pstUsrData);
HI_S32 HI_MPI_VDEC_SetChanFrmRate(HI_HANDLE hVdec, HI_UNF_AVPLAY_FRMRATE_PARAM_S *pstFrmRate);
HI_S32 HI_MPI_VDEC_GetChanFrmRate(HI_HANDLE hVdec, HI_UNF_AVPLAY_FRMRATE_PARAM_S *pstFrmRate);
HI_S32 HI_MPI_VDEC_SetEosFlag(HI_HANDLE hVdec);
HI_S32 HI_MPI_VDEC_DiscardFrame(HI_HANDLE hVdec, VDEC_DISCARD_FRAME_S* pstParam);
HI_S32 HI_MPI_VDEC_CreatePort(HI_HANDLE hVdec, HI_HANDLE *phPort, VDEC_PORT_ABILITY_E ePortAbility);
HI_S32 HI_MPI_VDEC_EnablePort(HI_HANDLE hVdec,HI_HANDLE hPort);
HI_S32 HI_MPI_VDEC_DisablePort(HI_HANDLE hVdec,HI_HANDLE hPort);
//HI_S32 HI_MPI_VDEC_SetMainPort(HI_HANDLE hVdec,HI_HANDLE hPort);  
HI_S32 HI_MPI_VDEC_SetPortType(HI_HANDLE hVdec, HI_HANDLE hPort, VDEC_PORT_TYPE_E enPortType);

HI_S32 HI_MPI_VDEC_GetPortAttr(HI_HANDLE hVdec, HI_HANDLE hPort, HI_DRV_VPSS_PORT_CFG_S *pstPortCfg);

HI_S32 HI_MPI_VDEC_SetPortAttr(HI_HANDLE hVdec, HI_HANDLE hPort, HI_DRV_VPSS_PORT_CFG_S *pstPortCfg);

HI_S32 HI_MPI_VDEC_CancleMainPort(HI_HANDLE hVdec,HI_HANDLE hPort);
HI_S32 HI_MPI_VDEC_DestroyPort(HI_HANDLE hVdec,HI_HANDLE hPort);
HI_S32 HI_MPI_VDEC_GetPortParam(HI_HANDLE hVdec, HI_HANDLE hPort, VDEC_PORT_PARAM_S *pstParam);
HI_S32 HI_MPI_VDEC_ReceiveFrame(HI_HANDLE hVdec, HI_DRV_VIDEO_FRAME_PACKAGE_S *pFrmPack);
HI_S32 HI_MPI_VDEC_GetChanFrmStatusInfo(HI_HANDLE hVdec, HI_HANDLE  hPort,VDEC_FRMSTATUSINFO_S *pstVdecFrmStatus);
HI_S32 HI_MPI_VDEC_SetChanFrmPackType(HI_HANDLE hVdec, HI_UNF_VIDEO_FRAME_PACKING_TYPE_E *pFrmPackingType);
HI_S32 HI_MPI_VDEC_GetChanFrmPackType(HI_HANDLE hVdec, HI_UNF_VIDEO_FRAME_PACKING_TYPE_E *pFrmPackingType);
HI_S32 HI_MPI_VDEC_AcqUserData(HI_HANDLE hVdec, 
                HI_UNF_VIDEO_USERDATA_S* pstUserData, HI_UNF_VIDEO_USERDATA_TYPE_E* penType);
HI_S32 HI_MPI_VDEC_RlsUserData(HI_HANDLE hVdec, HI_UNF_VIDEO_USERDATA_S* pstUserData);
HI_S32 HI_MPI_VDEC_Invoke(HI_HANDLE hVdec, HI_CODEC_VIDEO_CMD_S* pstParam);
HI_S32 HI_MPI_VDEC_GetChanOpenParam(HI_HANDLE hVdec, HI_UNF_AVPLAY_OPEN_OPT_S *pstOpenPara);

HI_S32 HI_MPI_VDEC_ReleaseFrame(HI_HANDLE hPort, HI_DRV_VIDEO_FRAME_S *pVideoFrame);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif//__HI_MPI_VDEC_H__

