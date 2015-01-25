/*******************************************************************************
*             Copyright 2004 - 2050, Hisilicon Tech. Co., Ltd.
*                           ALL RIGHTS RESERVED
* FileName: hal_demux.h
* Description: Register define for HiPVRV200
*
* History:
* Version   Date              Author    DefectNum       Description
* main\1    2009-09-29    y00106256     NULL            create this file.
*******************************************************************************/
#ifndef __HAL_DEMUX_H__
#define __HAL_DEMUX_H__

#include "drv_demux_define.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    DMX_CHAN_DATA_TYPE_SEC = 0,
    DMX_CHAN_DATA_TYPE_PES = 1,
} DMX_CHAN_DATA_TYPE_E;

typedef enum
{
    DMX_REC_TYPE_NONE       = 0,
    DMX_REC_TYPE_DESCRAM_TS = 1,
    DMX_REC_TYPE_PES        = 2,
    DMX_REC_TYPE_SCRAM_TS   = 3,
    DMX_REC_TYPE_ALL_TS     = 4,
    DMX_REC_TYPE_UNDEF
} DMX_REC_TYPE_E;

HI_VOID DmxHalDvbPortSetAttr(
        HI_U32                  PortId,
        HI_UNF_DMX_PORT_TYPE_E  PortType,
        HI_U32                  SyncOn,
        HI_U32                  SyncOff,
        HI_U32                  TunerInClk,
        HI_U32                  BitSelector
    );

HI_VOID DmxHalDvbPortSetClkInPol(HI_U32 PortId, HI_BOOL bClkInPol);
HI_VOID DmxHalDvbPortSetTsCountCtrl(const HI_U32 PortId, const HI_U32 option);
HI_U32  DmxHalDvbPortGetTsPackCount(HI_U32 PortId);
HI_VOID DmxHalDvbPortSetErrTsCountCtrl(const HI_U32 PortId, const HI_U32 option);
HI_U32  DmxHalDvbPortGetErrTsPackCount(HI_U32 PortId);

HI_VOID DmxHalIPPortSetAttr(HI_U32 PortId, HI_UNF_DMX_PORT_TYPE_E PortType, HI_U32 SyncOn, HI_U32 SyncOff);
HI_VOID DmxHalIPPortSetSyncLen(HI_U32 PortId, HI_U32 SyncLen1, HI_U32 SyncLen2);

#ifdef DMX_RAM_PORT_AUTO_SCAN_SUPPORT
HI_VOID DmxHalIPPortSetAutoScanRegion(HI_U32 PortId, HI_U32 len, HI_U32 step);
#endif

HI_VOID DmxHalIPPortSetTsCountCtrl(const HI_U32 PortId, const HI_BOOL enable);
HI_U32  DmxHalIPPortGetTsPackCount(HI_U32 PortId);
HI_VOID DmxHalIPPortStartStream(const HI_U32 PortId, const HI_BOOL Enable);
HI_VOID DmxHalIPPortDescSet(HI_U32 PortId, HI_U32 StartAddr, HI_U32 Depth);
HI_VOID DmxHalIPPortDescAdd(const HI_U32 PortId, const HI_U32 DescNum);

HI_VOID DmxHalSetChannelDataType(HI_U32 ChanId, DMX_CHAN_DATA_TYPE_E DataType);
HI_VOID DmxHalSetChannelAFMode(HI_U32 ChanId, DMX_Ch_AFMode_E eAfMode);
HI_VOID DmxHalSetChannelCRCMode(HI_U32 ChanId, HI_UNF_DMX_CHAN_CRC_MODE_E CrcMode);
HI_VOID DmxHalSetChannelCCDiscon(HI_U32 ChanId, HI_U32 DiscardFlag);
HI_VOID DmxHalSetChannelPusiCtrl(HI_U32 ChanId, HI_U32 PusiCtrl);
HI_VOID DmxHalSetChannelCCRepeatCtrl(HI_U32 ChanId, HI_U32 CCRepeatCtrl);
HI_VOID DmxHalSetChannelTsPostMode(HI_U32 ChanId, HI_U32 TsPost);
HI_VOID DmxHalSetChannelTsPostThresh(HI_U32 ChanId, HI_U32 Threshold);
HI_VOID DmxHalSetChannelAttr(HI_U32 ChanId, DMX_Ch_ATTR_E echattr);
HI_VOID DmxHalSetChannelFltMode(HI_U32 ChanId, HI_BOOL bEnable);
HI_VOID DmxHalGetChannelPlayDmxid(HI_U32 ChanId, HI_U32 *dmxid);
HI_VOID DmxHalSetChannelPlayDmxid(HI_U32 ChanId, HI_U32 dmxid);
HI_VOID DmxHalSetChannelRecDmxid(HI_U32 ChanId, HI_U32 dmxid);
HI_VOID DmxHalSetChannelPid(HI_U32 ChanId, HI_U32 pid);
HI_VOID DmxHalSetChannelRecBufId(HI_U32 ChanId, HI_U32 bufid);
HI_VOID DmxHalSetChannelPlayBufId(HI_U32 ChanId, HI_U32 bufid);
HI_VOID DmxHalSetDemuxPortId(HI_U32 DmxId, DMX_PORT_MODE_E PortMode, HI_U32 PortId);
HI_VOID DmxHalSetDataFakeMod(HI_BOOL bFakeEn);
HI_VOID DmxHalSetRecType(HI_U32 DmxId, DMX_REC_TYPE_E RecType);
HI_VOID DmxHalFlushChannel(HI_U32 ChanId, DMX_FLUSH_TYPE_E FlushType);
HI_BOOL DmxHalIsFlushChannelDone(HI_VOID);

HI_VOID DmxHalSetTsRecBufId(HI_U32 DmxId, HI_U32 OqId);
HI_VOID DmxHalSetSpsRefRecCh(HI_U32 DmxId, HI_U32 ChanId);
HI_VOID DmxHalSetSpsPauseType(HI_U32 u32DmxId, HI_U32 type);
HI_VOID DmxHalSetFilter(HI_U32 FilterId, HI_U32 Depth, HI_U8 Content, HI_BOOL bReverse, HI_U8 Mask);
HI_VOID DmxHalAttachFilter(HI_U32 FilterId, HI_U32 ChanId);
HI_VOID DmxHalDetachFilter(HI_U32 FilterId, HI_U32 ChanId);

HI_VOID DmxHalClearOq(HI_U32 OqId, DMX_OQ_CLEAR_TYPE_E ClearType);
HI_BOOL DmxHalIsClearOqDone(HI_VOID);

HI_VOID DmxHalEnableAllPVRInt(HI_VOID);
HI_VOID DmxHalDisableAllPVRInt(HI_VOID);

HI_VOID DmxHalIPPortAutoClearBP(HI_VOID);
HI_BOOL DmxHalGetIPBPStatus(HI_U32 PortId);
HI_VOID DmxHalClrIPBPStatus(HI_U32 PortId);
HI_VOID DmxHalClrIPFqBPStatus(HI_U32 PortId, HI_U32 FQId);

HI_U32  DmxHalIPPortGetOutIntStatus(HI_U32 u32PortId);
HI_VOID DmxHalIPPortClearOutIntStatus(HI_U32 u32PortId);
HI_VOID DmxHalIPPortSetOutInt(const HI_U32 PortId, const HI_BOOL Enable);
HI_U32  DmxHalIPPortDescGetRead(HI_U32 PortId);
HI_VOID DmxHalIPPortEnableInt(HI_U32 PortId);
HI_VOID DmxHalIPPortDisableInt(HI_U32 PortId);

HI_U32  DmxHalOQGetAllEopIntStatus(HI_VOID);
HI_VOID DmxHalEnableAllChEopInt(HI_VOID);
HI_VOID DmxHalEnableAllChEnqueInt(HI_VOID);

HI_VOID DmxHalEnableFQOutqueInt(HI_VOID);
HI_VOID DmxHalDisableFQOutqueInt(HI_VOID);
HI_VOID DmxHalSetFlushMaxWaitTime(HI_U32 u32MaxTime);
HI_S32  DmxHalSetScdFilter(HI_U32 u32FltId, HI_U8 u8Content);
HI_S32  DmxHalSetScdRangeFilter(HI_U32 u32FltId, HI_U8 u8High, HI_U8 u8Low);
HI_VOID DmxHalSetScdNewRangeFilter(HI_U32 FilterId, HI_U8 High, HI_U8 Low, HI_U8 Mask, HI_BOOL Negate);

HI_VOID DmxHalChooseScdFilter(HI_U32 ScdId, HI_U32 FilterId);
HI_VOID DmxHalScdFilterClear(HI_U32 ScdId);
HI_VOID DmxHalChooseScdRangeFilter(HI_U32 ScdId, HI_U32 FilterId);
HI_VOID DmxHalScdRangeFilterClear(HI_U32 ScdId);
HI_VOID DmxHalChooseScdNewRangeFilter(HI_U32 ScdId, HI_U32 FilterId);
HI_VOID DmxHalScdNewRangeFilterClear(HI_U32 ScdId);

HI_VOID DmxHalEnablePesSCD(HI_U32 ScdId);
HI_VOID DmxHalDisablePesSCD(HI_U32 ScdId);
HI_VOID DmxHalEnableEsSCD(HI_U32 ScdId);
HI_VOID DmxHalDisableEsSCD(HI_U32 ScdId);
HI_VOID DmxHalEnableMp4SCD(HI_U32 ScdId);
HI_VOID DmxHalDisableMp4SCD(HI_U32 ScdId);
HI_VOID DmxHalSetSCDAttachChannel(HI_U32 ScdId, HI_U32 ChanId);
HI_VOID DmxHalAllocSCDBufferId(HI_U32 ScdId, HI_U32 OqId);

HI_VOID DmxHalSetFlushIPPort(HI_U32 PortId);

HI_VOID DmxHalGetChannelTSScrambleFlag(HI_U32 u32Chid, HI_BOOL  *pEnable);
HI_VOID DmxHalGetChannelPesScrambleFlag(HI_U32 u32Chid, HI_BOOL  *pEnable);

HI_VOID DmxHalSetPcrDmxId(const HI_U32 PcrId, const HI_U32 DmxId);
HI_VOID DmxHalSetPcrPid(const HI_U32 PcrId, const HI_U32 PcrPid);
HI_VOID DmxHalGetPcrValue(const HI_U32 PcrId, HI_U64 *PcrVal);
HI_VOID DmxHalGetScrValue(const HI_U32 PcrId, HI_U64 *ScrVal);

/*---------------------------test IP-----------------------------*/

HI_U32  DmxHalGetTotalTeiIntStatus(HI_VOID);
HI_U32  DmxHalGetTotalPcrIntStatus(HI_VOID);
HI_U32  DmxHalGetTotalDiscIntStatus(HI_VOID);
HI_U32  DmxHalGetTotalCrcIntStatus(HI_VOID);
HI_U32  DmxHalGetTotalPenLenIntStatus(HI_VOID);
HI_U32  DmxHalGetPcrIntStatus(HI_VOID);
HI_VOID DmxHalClrPcrIntStatus(const HI_U32 PcrId);
HI_VOID DmxHalSetPcrIntEnable(const HI_U32 PcrId, const HI_BOOL Enable);
HI_VOID DmxHalGetTeiIntInfo(HI_U32* pu32DmxId, HI_U32* pu32ChanId);
HI_VOID DmxHalClrTeiIntStatus(HI_VOID);
HI_U32  DmxHalGetDiscIntStatus(HI_U32 RegionNum);
HI_VOID DmxHalClearDiscIntStatus(HI_U32 ChanId);
HI_U32  DmxHalGetCrcIntStatus(HI_U32 RegionNum);
HI_VOID DmxHalClearCrcIntStatus(HI_U32 ChanId);
HI_U32  DmxHalGetPesLenIntStatus(HI_U32 RegionNum);
HI_VOID DmxHalClearPesLenIntStatus(HI_U32 ChanId);
HI_VOID DmxHalFlushScdBuf(HI_U32 u32ScdId);
HI_VOID DmxHalClrScdCnt(HI_U32 u32ScdId);
HI_VOID DmxHalConfigHardware(HI_VOID);
HI_VOID DmxHalIPPortSetIntCnt(HI_U32 PortId, HI_U32 DescNum);

HI_VOID DmxHalEnableOQOutDInt(HI_U32 u32OQId);
HI_VOID DmxHalDisableOQOutDInt(HI_U32 u32OQId);
HI_BOOL DmxHalGetOQEopIntStatus(HI_U32 u32OQId);
HI_VOID DmxHalClearOQEopIntStatus(HI_U32 u32OQId);
HI_VOID DmxHalEnableOQEopInt(HI_U32 u32OQId);
HI_VOID DmxHalDisableOQEopInt(HI_U32 u32OQId);

#ifdef HI_DEMUX_PROC_SUPPORT
HI_VOID DmxHalFQEnableAllOverflowInt(HI_VOID);
HI_U32  DmxHalFQGetAllOverflowIntStatus(HI_VOID);
HI_U32  DmxHalFQGetOverflowIntStatus(HI_U32 offset);
HI_VOID DmxHalFQClearOverflowInt(HI_U32 FqId);
HI_VOID DmxHalFQSetOverflowInt(HI_U32 FqId, HI_BOOL Enable);
HI_BOOL DmxHalFQIsEnableOverflowInt(HI_U32 FqId);
#endif

HI_U32  DmxHalOQGetAllOverflowIntStatus(HI_VOID);
HI_BOOL DmxHalOQGetOverflowIntStatus(HI_U32 OQId);
HI_VOID DmxHalOQClearOverflowInt(HI_U32 OQId);
HI_VOID DmxHalOQEnableOverflowInt(HI_U32 OQId);
HI_VOID DmxHalOQDisableOverflowInt(HI_U32 OQId);

HI_U32  DmxHalOQGetAllOutputIntStatus(HI_VOID);
HI_U32  DmxHalOQGetOutputIntStatus(HI_U32 OqRegionId);
HI_VOID DmxHalOQEnableOutputInt(HI_U32 OQId, HI_BOOL Enable);

HI_VOID DxmHalSetOQRegMask(HI_U32 u32MaskValue);
HI_U32  DxmHalGetOQRegMask(HI_VOID);
HI_VOID DmxHalEnableFQOvflErrInt(HI_VOID);
HI_VOID DmxHalDisableFQOvflErrInt(HI_VOID);
HI_VOID DmxHalEnableOQOvflErrInt(HI_VOID);
HI_VOID DmxHalDisableOQOvflErrInt(HI_VOID);
HI_VOID DmxHalEnableFQCfgErrInt(HI_VOID);
HI_VOID DmxHalDisableFQCfgErrInt(HI_VOID);
HI_VOID DmxHalEnableFQDescErrInt(HI_VOID);
HI_VOID DmxHalDisableFQDescErrInt(HI_VOID);
HI_VOID DmxHalEnableAllDavInt(HI_VOID);
HI_VOID DmxHalEnableOQRecive(HI_U32 u32OQId);
HI_VOID DmxHalDisableOQRecive(HI_U32 u32OQId);
HI_BOOL DmxHalGetOQEnableStatus(HI_U32 u32OQId);
HI_VOID DmxHalFQEnableRecive(HI_U32 FQId, HI_BOOL Enable);
HI_S32  DmxHalGetInitStatus(HI_VOID);
HI_VOID DmxHalSetFQWORDx(HI_U32 u32FQId, HI_U32 u32Offset, HI_U32 u32Data);
HI_VOID DmxHalGetFQWORDx(HI_U32 u32FQId, HI_U32 u32Offset, HI_U32 *pu32Data);
HI_VOID DmxHalSetFQWritePtr(HI_U32 u32FQId, HI_U32 u32WritePtr);
HI_U32  DmxHalGetFQWritePtr(HI_U32 FQId);
HI_U32  DmxHalGetFQReadPtr(HI_U32 FQId);
HI_VOID DmxHalSetOQWORDx(HI_U32 u32OQId, HI_U32 u32Offset, HI_U32 u32Data);
HI_VOID DmxHalGetOQWORDx(HI_U32 u32OQId, HI_U32 u32Offset, HI_U32 *pu32Data);
HI_VOID DmxHalSetOQReadPtr(HI_U32 u32OQId, HI_U32 u32ReadPtr);

HI_VOID DmxHalAttachIPBPFQ(HI_U32 PortId, HI_U32 FQId);
HI_VOID DmxHalDetachIPBPFQ(HI_U32 PortId, HI_U32 FQId);

HI_VOID DmxHalSetRecTsCounter(HI_U32 u32DmxId, HI_U32 u32OqId);
HI_VOID DmxHalSetRecTsCntReplace(HI_U32 u32DmxId);
HI_VOID DmxHalResetOqCounter(HI_U32 u32OqId);
HI_U32  DmxHalGetChannelCounter(HI_U32 u32ChId);
HI_VOID DmxHalResetChannelCounter(HI_U32 u32ChId);

HI_VOID DmxHalFilterSetSecStuffCtrl(HI_BOOL Enable);
HI_VOID DmxHalSetTei(HI_U32   u32DemuxID,HI_BOOL bCheckTei);

#ifdef DMX_FILTER_DEPTH_SUPPORT
HI_VOID DmxHalFilterEnableDepth(HI_VOID);
HI_VOID DmxHalFilterSetDepth(HI_U32 FilterId, HI_U32 Depth);
#endif

#ifdef DMX_REC_EXCLUDE_PID_SUPPORT
HI_VOID DmxHalEnableAllRecExcludePid(HI_U32 DmxID);
HI_VOID DmxHalDisableAllRecExcludePid(HI_U32 DmxID);
HI_VOID DmxHalGetAllRecExcludePid(HI_U32 RecCfgID, HI_U32* DmxID, HI_U32* PID);
HI_VOID DmxHalSetAllRecExcludePid(HI_U32 RecCfgID, HI_U32 DmxID, HI_U32 PID);
#endif


#ifdef DMX_REC_TIME_STAMP_SUPPORT    /*only hi3719 support this */
HI_VOID DmxHalConfigRecTsTimeStamp(HI_U32 DmxID, DMX_REC_TIMESTAMP_MODE_E enRecTimeStamp);
#endif

#ifdef __cplusplus
}
#endif

#endif

