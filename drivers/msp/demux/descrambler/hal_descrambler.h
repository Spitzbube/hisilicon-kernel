/******************************************************************************
 Copyright (C), 2009-2019, Hisilicon Tech. Co., Ltd.
 ******************************************************************************
 File Name     : hal_descrambler.h
 Version       : Initial Draft
 Author        : Hisilicon multimedia software group
 Created       : 2013/04/16
 Description   :
******************************************************************************/

#ifndef __HAL_DESCRAMBLER_H__
#define __HAL_DESCRAMBLER_H__

#include "drv_demux_define.h"
#include "drv_descrambler.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

HI_VOID DmxHalSetChannelCWIndex(HI_U32 ChanId, HI_U32 cwIndex);
HI_VOID DmxHalSetChannelDsc(HI_U32 ChanId, HI_BOOL Enable);

HI_VOID DmxHalSetEntropyReduction(HI_U32 KeyId, HI_UNF_DMX_CA_ENTROPY_E EntropyReduction);

HI_U32  DmxHalGetOptCAType(HI_VOID);
HI_U32  DmxHalGetOptDescramblerType(HI_VOID);
HI_VOID DmxHalSetCAType(HI_U32 u32KeyId, HI_BOOL bAdvance);
HI_VOID DmxHalSetDescramblerType(HI_U32 KeyId, HI_BOOL bHigh);
HI_VOID DmxHalSetCWWord(HI_U32 u32CWId, HI_U32 u32WordId, HI_U32 u32Data, HI_U32 u32EvenOdd);

#ifdef DMX_DESCRAMBLER_VERSION_1
HI_VOID DmxHalInitSpeCWOrder(HI_VOID);
HI_VOID DmxHalSetCWWord1(
        HI_U32                          GroupId,
        HI_U32                          WordId,
        HI_U32                          Key,
        DMX_KEY_TYPE_E                  KeyType,
        DMX_CW_TYPE                     CWType,
        HI_UNF_DMX_DESCRAMBLER_TYPE_E   DescType
    );
HI_VOID DmxHalSetChanCwTabId(HI_U32 ChanId, HI_U32 TabId);
HI_VOID DmxHalSetDmxIvEnable(HI_U32 DmxId, HI_BOOL Enable);
HI_VOID DmxHalSetCSA3Reset(HI_BOOL Enable);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  // __HAL_DESCRAMBLER_H__

