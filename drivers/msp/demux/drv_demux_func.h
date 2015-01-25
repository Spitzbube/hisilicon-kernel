/*******************************************************************************
*             Copyright 2004 - 2050, Hisilicon Tech. Co., Ltd.
*                           ALL RIGHTS RESERVED
* FileName: drv_demux_func.h
* Description: Register define for HiPVRV200
*
* History:
* Version   Date              Author    DefectNum       Description
* main\1    2009-09-29    y00106256     NULL            create this file.
*******************************************************************************/
#ifndef __DRV_DEMUX_FUNC_H__
#define __DRV_DEMUX_FUNC_H__

#include "hi_drv_dev.h"
#include "drv_demux_define.h"
#include "hi_unf_demux.h"
#include "hi_mpi_demux.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

HI_S32  DMX_OsiInit(HI_U32 PoolBufSize, HI_U32 BlockSize);
HI_S32  DMX_OsiDeInit(HI_VOID);
HI_S32  DMX_OsiGetPoolBufAddr(HI_U32 *PhyAddr, HI_U32 *BufSize);
HI_VOID DMX_OsiSetNoPusiEn(HI_BOOL bNoPusiEn);
HI_VOID DMX_OsiSetTei(HI_U32 u32DmxId,HI_BOOL bTei);

/* Port */
HI_S32  DMX_OsiTunerPortGetAttr(const HI_U32 PortId, HI_UNF_DMX_PORT_ATTR_S *PortAttr);
HI_S32  DMX_OsiTunerPortSetAttr(const HI_U32 PortId, const HI_UNF_DMX_PORT_ATTR_S *PortAttr);
HI_S32  DMX_OsiRamPortGetAttr(const HI_U32 PortId, HI_UNF_DMX_PORT_ATTR_S *PortAttr);
HI_S32  DMX_OsiRamPortSetAttr(const HI_U32 PortId, const HI_UNF_DMX_PORT_ATTR_S *PortAttr);

HI_S32  DMX_OsiTunerPortGetPacketNum(const HI_U32 PortId, HI_U32 *TsPackCnt, HI_U32 *ErrTsPackCnt);
HI_S32  DMX_OsiRamPortGetPacketNum(const HI_U32 PortId, HI_U32 *TsPackCnt);

HI_S32  DMX_OsiTsBufferCreate(const HI_U32 PortId, const HI_U32 Size, DMX_MMZ_BUF_S *TsBuf);
HI_S32  DMX_OsiTsBufferDestroy(const HI_U32 PortId);
HI_S32  DMX_OsiTsBufferGet(const HI_U32 PortId, const HI_U32 ReqLen, DMX_DATA_BUF_S *Buf, HI_U32 Timeout);
HI_S32  DMX_OsiTsBufferPut(const HI_U32 PortId, const HI_U32 DataLen, const HI_U32 StartPos);
HI_S32  DMX_OsiTsBufferReset(const HI_U32 PortId);
HI_S32  DMX_OsiTsBufferGetStatus(const HI_U32 PortId, HI_UNF_DMX_TSBUF_STATUS_S *Status);

/*SubDev*/
HI_S32  DMX_OsiAttachPort(const HI_U32 DmxId, const DMX_PORT_MODE_E PortMode, const HI_U32 PortId);
HI_S32  DMX_OsiDetachPort(const HI_U32 DmxId);
HI_S32  DMX_OsiGetPortId(const HI_U32 DmxId, DMX_PORT_MODE_E *PortMode, HI_U32 *PortId);

/* Filter */
HI_S32  DMX_OsiNewFilter(const HI_U32 DmxId, HI_U32 *FilterId);
HI_S32  DMX_OsiDeleteFilter(const HI_U32 FilterId);
HI_S32  DMX_OsiSetFilterAttr(const HI_U32 FilterId, const HI_UNF_DMX_FILTER_ATTR_S *FilterAttr);
HI_S32  DMX_OsiGetFilterAttr(const HI_U32 FilterId, HI_UNF_DMX_FILTER_ATTR_S *FilterAttr);
HI_S32  DMX_OsiAttachFilter(const HI_U32 FilterId, const HI_U32 ChanId);
HI_S32  DMX_OsiDetachFilter(const HI_U32 FilterId, const HI_U32 ChanId);
HI_S32  DMX_OsiGetFilterChannel(const HI_U32 FilterId, HI_U32 *ChanId);
HI_S32  DMX_OsiGetFreeFilterNum(const HI_U32 DmxId, HI_U32 *FreeCount);

// Channel
HI_S32  DMX_OsiCreateChannel(HI_U32 DmxId, HI_UNF_DMX_CHAN_ATTR_S *ChanAttr, DMX_MMZ_BUF_S *ChanBuf, HI_U32 *ChanId);
HI_S32  DMX_OsiDestroyChannel(HI_U32 ChanId);
HI_S32  DMX_OsiOpenChannel(HI_U32 ChanId);
HI_S32  DMX_OsiCloseChannel(HI_U32 ChanId);
HI_S32  DMX_OsiGetChannelAttr(HI_U32 ChanId, HI_UNF_DMX_CHAN_ATTR_S *ChanAttr);
HI_S32  DMX_OsiSetChannelAttr(HI_U32 ChanId, HI_UNF_DMX_CHAN_ATTR_S *ChanAttr);
HI_S32  DMX_OsiGetChannelPid(HI_U32 ChanId, HI_U32 *Pid);
HI_S32  DMX_OsiSetChannelPid(HI_U32 ChanId, HI_U32 Pid);
HI_S32  DMX_OsiGetChannelStatus(HI_U32 ChanId, HI_UNF_DMX_CHAN_STATUS_E *ChanStatus);
HI_S32  DMX_OsiGetFreeChannelNum(HI_U32 u32DmxId, HI_U32 *pu32FreeCount);
HI_S32  DMX_OsiGetChannelScrambleFlag(HI_U32 u32ChannelId, HI_UNF_DMX_SCRAMBLED_FLAG_E *penScrambleFlag);
HI_S32  DMX_OsiSetChannelEosFlag(HI_U32 ChanId);
HI_U32  DMX_OsiGetChnDataFlag(HI_U32 u32ChanId);
#ifdef DMX_USE_ECM
HI_S32  DMX_OsiGetChannelSwFlag(HI_U32 u32ChannelId, HI_U32* pu32SwFlag);
HI_S32  DMX_OsiGetChannelSwBufAddr(HI_U32 u32ChannelId, MMZ_BUFFER_S* pstSwBuf);
#endif
HI_S32  DMX_OsiResetChannel(HI_U32 u32ChId, DMX_FLUSH_TYPE_E eFlushType);
HI_S32  DMX_OsiGetChannelTsCnt(HI_U32 u32ChannelId, HI_U32* pu32TsCnt);
HI_S32  DMX_OsiSetChannelCCRepeat(HI_U32 ChanId, HI_UNF_DMX_CHAN_CC_REPEAT_SET_S * pstChCCReaptSet);


/* get channel ID by pid, return HI_ERR_DMX_NOMATCH_CHN when failed to find it */
HI_S32  DMX_OsiGetChannelId(HI_U32 u32DmxId, HI_U32 u32Pid, HI_U32 *pu32ChannelId);
HI_S32  DMX_OsiSelectDataFlag(HI_U32 *pu32WatchCh, HI_U32 u32WatchNum, HI_U32 *pu32Flag, HI_U32 u32TimeOutMs);

HI_S32  DMX_OsiReadDataRequset(HI_U32 u32ChId, HI_U32 u32AcqNum,
                               HI_U32 *pu32AcqedNum, DMX_UserMsg_S* psMsgList, HI_U32 u32TimeOutMs);
HI_S32  DMX_OsiReleaseReadData(HI_U32 u32ChId, HI_U32 u32RelNum, DMX_UserMsg_S* psMsgList);

HI_S32  DMX_OsiReadEsRequset(HI_U32 ChanId, DMX_Stream_S *EsData);
HI_S32  DMX_OsiReleaseReadEs(HI_U32 ChanId, DMX_Stream_S *EsData);

HI_S32  DMX_OsiGetChanBufStatus(HI_U32 ChanId, HI_MPI_DMX_BUF_STATUS_S *BufStatus);

/* get whether all the channel(96 channenels) channel data exist or not, returned by three u32 flag
    per-bit present one channel, just only return common channel, except the audio-video channel
 */
HI_S32  DMX_OsiGetAllDataFlag(HI_U32 *pu32Flag, HI_U32 u32TimeOutMs);

/* PCR */
HI_S32  DMX_OsiPcrChannelCreate(const HI_U32 DmxId, HI_U32 *PcrId);
HI_S32  DMX_OsiPcrChannelDestroy(const HI_U32 PcrId);
HI_S32  DMX_OsiPcrChannelSetPid(const HI_U32 PcrId, const HI_U32 PcrPid);
HI_S32  DMX_OsiPcrChannelGetPid(const HI_U32 PcrId, HI_U32 *PcrPid);
HI_S32  DMX_OsiPcrChannelGetClock(const HI_U32 PcrId, HI_U64 *PcrValue, HI_U64 *ScrValue);
HI_S32  DMX_OsiPcrChannelAttachSync(const HI_U32 PcrId, const HI_U32 SyncHadle);
HI_S32  DMX_OsiPcrChannelDetachSync(const HI_U32 PcrId);

HI_S32 DMX_DRV_REC_CreateChannel(HI_UNF_DMX_REC_ATTR_S *RecAttr, DMX_REC_TIMESTAMP_MODE_E enRecTimeStamp,HI_U32 *RecId, HI_U32 *BufPhyAddr, HI_U32 *BufSize);
HI_S32  DMX_DRV_REC_DestroyChannel(HI_U32 RecId);
HI_S32  DMX_DRV_REC_AddRecPid(HI_U32 RecId, HI_U32 Pid, HI_U32 *ChanId);
HI_S32  DMX_DRV_REC_DelRecPid(HI_U32 RecId, HI_U32 ChanId);
HI_S32  DMX_DRV_REC_DelAllRecPid(HI_U32 RecId);
HI_S32  DMX_DRV_REC_AddExcludeRecPid(HI_U32 RecId, HI_U32 Pid);
HI_S32  DMX_DRV_REC_DelExcludeRecPid(HI_U32 RecId, HI_U32 Pid);
HI_S32  DMX_DRV_REC_DelAllExcludeRecPid(HI_U32 RecId);
HI_S32  DMX_DRV_REC_StartRecChn(HI_U32 RecId);
HI_S32  DMX_DRV_REC_StopRecChn(HI_U32 RecId);
HI_S32  DMX_DRV_REC_AcquireRecData(HI_U32 RecId, HI_U32 *PhyAddr, HI_U32 *KerAddr, HI_U32 *Len, HI_U32 Timeout);
HI_S32  DMX_DRV_REC_ReleaseRecData(HI_U32 RecId, HI_U32 PhyAddr, HI_U32 Len);
HI_S32  DMX_DRV_REC_AcquireRecIndex(HI_U32 RecId, HI_UNF_DMX_REC_INDEX_S *RecIndex, HI_U32 Timeout);
HI_S32  DMX_DRV_REC_GetRecBufferStatus(HI_U32 RecId, HI_UNF_DMX_RECBUF_STATUS_S *BufStatus);

HI_S32  DMX_DRV_REC_AcquireScdData(HI_U32 RecId, HI_U32 *PhyAddr, HI_U32 *KerAddr, HI_U32 *Len, HI_U32 Timeout);
HI_S32  DMX_DRV_REC_ReleaseScdData(HI_U32 RecId, HI_U32 PhyAddr, HI_U32 Len);

#ifdef HI_DEMUX_PROC_SUPPORT

typedef struct
{
    HI_U32  PhyAddr;
    HI_U32  BufSize;
    HI_U32  UsedSize;
    HI_U32  Read;
    HI_U32  Write;
    HI_U32  GetCount;
    HI_U32  GetValidCount;
    HI_U32  PutCount;
} DMX_Proc_RamPort_BufInfo_S;

typedef struct
{
    HI_U32  DescDepth;
    HI_U32  DescRead;
    HI_U32  DescWrite;
    HI_U32  BlockSize;
    HI_U32  Overflow;
} DMX_Proc_ChanBuf_S;

typedef struct
{
    HI_UNF_DMX_REC_TYPE_E   RecType;
    HI_BOOL                 Descramed;
    HI_U32                  BlockCnt;
    HI_U32                  BlockSize;
    HI_U32                  BufRead;
    HI_U32                  BufWrite;
    HI_U32                  RecStatus;
    HI_U32                  Overflow;
} DMX_Proc_Rec_BufInfo_S;

typedef struct
{
    HI_UNF_DMX_REC_INDEX_TYPE_E IndexType;
    HI_U32                      IndexPid;
    HI_U32                      BlockCnt;
    HI_U32                      BlockSize;
    HI_U32                      BufRead;
    HI_U32                      BufWrite;
    HI_U32                  Overflow;
} DMX_Proc_RecScd_BufInfo_S;

HI_S32  DMX_OsiRamPortGetBufInfo(HI_U32 PortId, DMX_Proc_RamPort_BufInfo_S *BufInfo);
HI_S32  DMX_OsiGetChanBufProc(HI_U32 ChanId, DMX_Proc_ChanBuf_S *BufInfo);
HI_S32  DMX_OsiGetDmxRecProc(HI_U32 RecId, DMX_Proc_Rec_BufInfo_S *RecBufInfo);
HI_S32  DMX_OsiGetDmxRecScdProc(HI_U32 RecId, DMX_Proc_RecScd_BufInfo_S *ScdBufInfo);
DMX_ChanInfo_S*     DMX_OsiGetChannelProc(HI_U32 ChanId);
DMX_FilterInfo_S*   DMX_OsiGetFilterProc(HI_U32 FilterId);
DMX_PCR_Info_S*     DMX_OsiGetPcrChannelProc(const HI_U32 PcrId);

HI_S32  DMX_OsiSaveDmxTs_Start(HI_U32 u32DmxId, HI_U32 u32RecDmxId);
HI_S32  DMX_OsiSaveDmxTs_Stop(HI_U32 u32RecDmx);

#endif

HI_S32  DMX_OsiDeviceInit(HI_U32 PoolBufSize, HI_U32 BlockSize);
HI_VOID DMX_OsiDeviceDeInit(HI_VOID);
HI_U32  DMX_OsiGetChType(HI_U32 u32ChId);

HI_S32  DMX_OsiSuspend(PM_BASEDEV_S *himd, pm_message_t state);
HI_S32  DMX_OsiResume(PM_BASEDEV_S *himd);


//#define DMX_DBG_FPGA_TEST       1

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  // __DRV_DEMUX_FUNC_H__

