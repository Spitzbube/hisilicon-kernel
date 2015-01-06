/******************************************************************************

Copyright (C), 2009-2019, Hisilicon Tech. Co., Ltd.
******************************************************************************
File Name     : hi_mpi_demux.h
Version       : Initial Draft
Author        : Hisilicon multimedia software group
Created       : 2009-12-14
Last Modified :
Description   : MPI Layer function declaration for linux
Function List :
History       :
* main\1    2009-12-14   jianglei     init.
******************************************************************************/

#ifndef __HI_MPI_DEMUX_H__
#define __HI_MPI_DEMUX_H__

#include "hi_type.h"

#include "hi_unf_demux.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*************************** Struct Definition ****************************/
typedef struct hiMPI_DMX_BUF_STATUS_S
{
    HI_U32 u32BufSize;  /*buffer size */
    HI_U32 u32UsedSize; /* buffer used size *//*CNcomment:缓冲区已使用大小 */
    HI_U32 u32BufRptr;  /*buffer read pointer *//*CNcomment:缓冲区读指针*/
    HI_U32 u32BufWptr;/*buffer written pointer *//*CNcomment:缓冲区写指针*/
} HI_MPI_DMX_BUF_STATUS_S;

typedef struct hiDMX_DATA_S
{
    HI_U8   *pAddr;
    HI_U32   u32PhyAddr;
    HI_U32   u32Len;
} DMX_DATA_S;

/******************************* API declaration *****************************/
HI_S32 HI_MPI_DMX_Init(HI_VOID);
HI_S32 HI_MPI_DMX_DeInit(HI_VOID);
HI_S32 HI_MPI_DMX_GetCapability(HI_UNF_DMX_CAPABILITY_S *pstCap);

/* Port */
HI_S32 HI_MPI_DMX_GetTSPortAttr(HI_UNF_DMX_PORT_E enPortId, HI_UNF_DMX_PORT_ATTR_S *pstAttr);
HI_S32 HI_MPI_DMX_SetTSPortAttr(HI_UNF_DMX_PORT_E enPortId, const HI_UNF_DMX_PORT_ATTR_S *pstAttr);
HI_S32 HI_MPI_DMX_AttachTSPort(HI_U32 u32DmxId, HI_UNF_DMX_PORT_E enPortId);
HI_S32 HI_MPI_DMX_DetachTSPort(HI_U32 u32DmxId);
HI_S32 HI_MPI_DMX_GetTSPortId(HI_U32 u32DmxId, HI_UNF_DMX_PORT_E *penPortId);
HI_S32 HI_MPI_DMX_GetTSPortPacketNum(HI_UNF_DMX_PORT_E enPortId, HI_UNF_DMX_PORT_PACKETNUM_S *sPortStat);


/* TS Buffer */
HI_S32 HI_MPI_DMX_CreateTSBuffer(HI_UNF_DMX_PORT_E enPortId, HI_U32 u32TsBufSize, HI_HANDLE *phTsBuffer);
HI_S32 HI_MPI_DMX_DestroyTSBuffer(HI_HANDLE hTsBuffer);
HI_S32 HI_MPI_DMX_GetTSBuffer(HI_HANDLE hTsBuffer, HI_U32 u32ReqLen,
            HI_UNF_STREAM_BUF_S *pstData, HI_U32 *pu32PhyAddr, HI_U32 u32TimeOutMs);
HI_S32 HI_MPI_DMX_PutTSBuffer(HI_HANDLE hTsBuffer, HI_U32 u32ValidDataLen, HI_U32 u32StartPos);
HI_S32 HI_MPI_DMX_ResetTSBuffer(HI_HANDLE hTsBuffer);
HI_S32 HI_MPI_DMX_GetTSBufferStatus(HI_HANDLE hTsBuffer, HI_UNF_DMX_TSBUF_STATUS_S *pStatus);
HI_S32 HI_MPI_DMX_GetTSBufferPortId(HI_HANDLE hTsBuffer, HI_UNF_DMX_PORT_E *penPortId);
HI_S32 HI_MPI_DMX_GetTSBufferHandle(HI_UNF_DMX_PORT_E enPortId, HI_HANDLE *phTsBuffer);

/* Channel */
/*Buffer for video and audio channel should be attached,it's meaningless to config when apply them *//*CNcomment: 音视频通道的buffer需要绑定，在申请时配置没有意义*/
HI_S32 HI_MPI_DMX_GetPortMode(HI_U32 u32DmxId, HI_UNF_DMX_PORT_MODE_E *penPortMod);
HI_S32 HI_MPI_DMX_GetChannelDefaultAttr(HI_UNF_DMX_CHAN_ATTR_S *pstChAttr);
HI_S32 HI_MPI_DMX_CreateChannel(HI_U32 u32DmxId, const HI_UNF_DMX_CHAN_ATTR_S *pstChAttr,
            HI_HANDLE *phChannel);
HI_S32 HI_MPI_DMX_DestroyChannel(HI_HANDLE hChannel);
HI_S32 HI_MPI_DMX_GetChannelAttr(HI_HANDLE hChannel, HI_UNF_DMX_CHAN_ATTR_S *pstChAttr);
HI_S32 HI_MPI_DMX_SetChannelAttr(HI_HANDLE hChannel, const HI_UNF_DMX_CHAN_ATTR_S *pstChAttr);
HI_S32 HI_MPI_DMX_SetChannelPID(HI_HANDLE hChannel, HI_U32 u32Pid);
HI_S32 HI_MPI_DMX_GetChannelPID(HI_HANDLE hChannel, HI_U32 *pu32Pid);
HI_S32 HI_MPI_DMX_OpenChannel(HI_HANDLE hChannel);
HI_S32 HI_MPI_DMX_CloseChannel(HI_HANDLE hChannel);
HI_S32 HI_MPI_DMX_GetChannelStatus(HI_HANDLE hChannel, HI_UNF_DMX_CHAN_STATUS_S *pstStatus);
HI_S32 HI_MPI_DMX_GetChannelHandle(HI_U32 u32DmxId, HI_U32 u32Pid, HI_HANDLE *phChannel);
HI_S32 HI_MPI_DMX_GetFreeChannelCount(HI_U32 u32DmxId, HI_U32 *pu32FreeCount);
HI_S32 HI_MPI_DMX_GetScrambledFlag(HI_HANDLE hChannel, HI_UNF_DMX_SCRAMBLED_FLAG_E *penScrambleFlag);
HI_S32 HI_MPI_DMX_SetChannelEosFlag(HI_HANDLE hChannel);
HI_S32 HI_MPI_DMX_GetChannelTsCount(HI_HANDLE hChannel, HI_U32 *pu32TsCount);


/* Filter */
HI_S32 HI_MPI_DMX_CreateFilter(HI_U32   u32DmxId, const HI_UNF_DMX_FILTER_ATTR_S  *pstFilterAttr,
            HI_HANDLE *phFilter);
HI_S32 HI_MPI_DMX_DestroyFilter(HI_HANDLE hFilter);
HI_S32 HI_MPI_DMX_DeleteAllFilter(HI_HANDLE hChannel);
HI_S32 HI_MPI_DMX_SetFilterAttr(HI_HANDLE hFilter, const HI_UNF_DMX_FILTER_ATTR_S *pstFilterAttr);
HI_S32 HI_MPI_DMX_GetFilterAttr(HI_HANDLE hFilter, HI_UNF_DMX_FILTER_ATTR_S *pstFilterAttr );
HI_S32 HI_MPI_DMX_AttachFilter(HI_HANDLE hFilter, HI_HANDLE hChannel);
HI_S32 HI_MPI_DMX_DetachFilter(HI_HANDLE hFilter, HI_HANDLE hChannel);
HI_S32 HI_MPI_DMX_GetFilterChannelHandle(HI_HANDLE hFilter, HI_HANDLE *phChannel);
HI_S32 HI_MPI_DMX_GetFreeFilterCount(HI_U32 u32DmxId ,  HI_U32 * pu32FreeCount);


/* Data receive */
HI_S32  HI_MPI_DMX_GetDataHandle(HI_HANDLE *phChannel, HI_U32 *pu32ChNum,
            HI_U32 u32TimeOutMs);

HI_S32  HI_MPI_DMX_SelectDataHandle(HI_HANDLE *phWatchChannel, HI_U32 u32WatchNum,
            HI_HANDLE *phDataChannel, HI_U32 *pu32ChNum, HI_U32 u32TimeOutMs);

HI_S32  HI_MPI_DMX_AcquireBuf(HI_HANDLE hChannel, HI_U32 u32AcquireNum,
            HI_U32 * pu32AcquiredNum, HI_UNF_DMX_DATA_S *pstBuf,
            HI_U32 u32TimeOutMs);
HI_S32  HI_MPI_DMX_ReleaseBuf(HI_HANDLE hChannel, HI_U32 u32ReleaseNum,
            HI_UNF_DMX_DATA_S *pstBuf);

/* PCR */
HI_S32 HI_MPI_DMX_CreatePcrChannel(HI_U32 u32DmxId, HI_U32 *pu32PcrChId);
HI_S32 HI_MPI_DMX_DestroyPcrChannel(HI_U32 u32PcrChId);
HI_S32 HI_MPI_DMX_PcrPidSet(HI_U32 pu32PcrChId, HI_U32 u32Pid);
HI_S32 HI_MPI_DMX_PcrPidGet(HI_U32 pu32PcrChId, HI_U32 *pu32Pid);
HI_S32 HI_MPI_DMX_PcrScrGet(HI_U32 pu32PcrChId, HI_U64 *pu64PcrMs,HI_U64 *pu64ScrMs);
HI_S32 HI_MPI_DMX_PcrSyncAttach(HI_U32 u32PcrChId, HI_U32 u32SyncHandle);
HI_S32 HI_MPI_DMX_PcrSyncDetach(HI_U32 u32PcrChId);

/*Only video and audio channel are enable to attach PES Buffer *//*CNcomment:   只有音视频通道允许绑定和解绑定PES Buffer*/
HI_S32 HI_MPI_DMX_GetPESBufferStatus(HI_HANDLE hChannel, HI_MPI_DMX_BUF_STATUS_S *pBufStat);

/*Be used to send stream to user status decoder *//*CNcomment:   用于给用户态的音频解码送码流*/
HI_S32 HI_MPI_DMX_AcquireEs(HI_HANDLE hChannel, HI_UNF_ES_BUF_S *pAudioEsBuf);
HI_S32 HI_MPI_DMX_ReleaseEs(HI_HANDLE hChannel,const HI_UNF_ES_BUF_S *pAudioEsBuf);

HI_S32 HI_MPI_DMX_CreateRecChn(HI_UNF_DMX_REC_ATTR_S *pstRecAttr, HI_HANDLE *phRecChn);
HI_S32 HI_MPI_DMX_DestroyRecChn(HI_HANDLE hRecChn);

HI_S32 HI_MPI_DMX_AddRecPid(HI_HANDLE hRecChn, HI_U32 u32Pid, HI_HANDLE *phChannel);
HI_S32 HI_MPI_DMX_DelRecPid(HI_HANDLE hRecChn, HI_HANDLE hChannel);
HI_S32 HI_MPI_DMX_DelAllRecPid(HI_HANDLE hRecChn);

HI_S32 HI_MPI_DMX_AddExcludeRecPid(HI_HANDLE hRecChn, HI_U32 u32Pid);
HI_S32 HI_MPI_DMX_DelExcludeRecPid(HI_HANDLE hRecChn, HI_U32 u32Pid);
HI_S32 HI_MPI_DMX_DelAllExcludeRecPid(HI_HANDLE hRecChn);

HI_S32 HI_MPI_DMX_StartRecChn(HI_HANDLE hRecChn);
HI_S32 HI_MPI_DMX_StopRecChn(HI_HANDLE hRecChn);

HI_S32 HI_MPI_DMX_AcquireRecData(HI_HANDLE hRecChn, HI_UNF_DMX_REC_DATA_S *pstRecData, HI_U32 u32TimeoutMs);
HI_S32 HI_MPI_DMX_ReleaseRecData(HI_HANDLE hRecChn, const HI_UNF_DMX_REC_DATA_S *pstRecData);

HI_S32 HI_MPI_DMX_AcquireRecIndex(HI_HANDLE hRecChn, HI_UNF_DMX_REC_INDEX_S *pstRecIndex, HI_U32 u32TimeoutMs);

HI_S32 HI_MPI_DMX_GetRecBufferStatus(HI_HANDLE hRecChn, HI_UNF_DMX_RECBUF_STATUS_S *pstBufStatus);

/***********************recoder type***************/
typedef enum hiMPI_DMX_RECORD_TYPE_E
{
    HI_MPI_DMX_RECORD_NONE = 0,
    HI_MPI_DMX_RECORD_DESCRAM_TS,
    HI_MPI_DMX_RECORD_PES,
    HI_MPI_DMX_RECORD_SCRAM_TS,
    HI_MPI_DMX_RECORD_ALL_TS,

    HI_MPI_DMX_RECORD_TYPE_BUTT
} HI_MPI_DMX_RECORD_TYPE_E;

typedef enum
{
    HI_MPI_DMX_REC_INDEX_TYPE_NONE,
    HI_MPI_DMX_REC_INDEX_TYPE_VIDEO,
    HI_MPI_DMX_REC_INDEX_TYPE_AUDIO,
    HI_MPI_DMX_REC_INDEX_TYPE_BUTT
} HI_MPI_DMX_REC_INDEX_TYPE_E;

/* Sequence of DMX_IDX_DATA_S 's member can not change,must match the sequence defined by hardware*/
/* CNcomment:DMX_IDX_DATA_S 中各成员的顺序不能改变，必须与硬件规定的顺序保持一致 */
typedef struct hiDMX_IDX_DATA_S
{
    HI_U32 u32Chn_Ovflag_IdxType_Flags;
    HI_U32 u32ScType_Byte12AfterSc_OffsetInTs;
    HI_U32 u32TsCntLo32;
    HI_U32 u32TsCntHi8_Byte345AfterSc;
    HI_U32 u32ScCode_Byte678AfterSc;
    HI_U32 u32SrcClk;
    HI_U32 u32BackPacetNum;/*Back package number*//* CNcomment:回退包计数*/
} DMX_IDX_DATA_S;

HI_S32 HI_MPI_DMX_StartRecord(HI_U32 u32DmxId,
                               HI_MPI_DMX_REC_INDEX_TYPE_E enIndexType,
                               HI_U32 u32IndexPid,
                               HI_UNF_VCODEC_TYPE_E enVCodecType,
                               HI_MPI_DMX_RECORD_TYPE_E enRecType,
                               HI_U32 u32ScdBufSize, HI_U32 u32TsBufSize);
HI_S32 HI_MPI_DMX_StopRecord(HI_U32 u32DmxId);
HI_S32 HI_MPI_DMX_GetRECBufferStatus(HI_U32 u32DmxId, HI_MPI_DMX_BUF_STATUS_S *pBufStat);

HI_S32 HI_MPI_DMX_AcquireRecTsBuf(HI_U32 u32DmxId, DMX_DATA_S *pstBuf, HI_BOOL bDoCipher, HI_U32 u32TimeoutMs);
HI_S32 HI_MPI_DMX_ReleaseRecTsBuf(HI_U32 u32DmxId, const DMX_DATA_S *pstBuf);
HI_S32 HI_MPI_DMX_AcquireRecScdBuf(HI_U32 u32DmxId, DMX_DATA_S *pstBuf, HI_U32 u32TimeoutMs);
HI_S32 HI_MPI_DMX_ReleaseRecScdBuf(HI_U32 u32DmxId, const DMX_DATA_S *pstBuf);

HI_S32 HI_MPI_DMX_Invoke(HI_UNF_DMX_INVOKE_TYPE_E enCmd, const HI_VOID *pCmdPara);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  // __HI_MPI_DEMUX_H__

