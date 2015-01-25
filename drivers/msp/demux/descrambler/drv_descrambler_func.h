/******************************************************************************
 Copyright (C), 2009-2019, Hisilicon Tech. Co., Ltd.
 ******************************************************************************
 File Name     : drv_descrambler_func.h
 Version       : Initial Draft
 Author        : Hisilicon multimedia software group
 Created       : 2013/04/16
 Description   :
******************************************************************************/

#ifndef __DRV_DESCRAMBLER_FUNC_H__
#define __DRV_DESCRAMBLER_FUNC_H__

#include "hi_unf_descrambler.h"

#include "drv_demux_define.h"

#ifdef __cplusplus
extern "C" {
#endif

HI_VOID DescramblerReset(HI_U32 KeyId, DMX_KeyInfo_S *KeyInfo);
HI_VOID DescInitHardFlag(HI_VOID);
HI_VOID DmxDescramblerResume(HI_VOID);

HI_S32  DMX_OsiDescramblerCreate(HI_U32 *KeyId, const HI_UNF_DMX_DESCRAMBLER_ATTR_S *DescAttr);
HI_S32  DMX_OsiDescramblerDestroy(HI_U32 KeyId);
HI_S32  DMX_OsiDescramblerSetKey(HI_U32 KeyId, HI_U32 KeyType, HI_U8 *Key);
HI_S32  DMX_OsiDescramblerSetIVKey(HI_U32 KeyId, HI_U32 KeyType, HI_U8 *Key);
HI_S32  DMX_OsiDescramblerAttach(HI_U32 KeyId, HI_U32 ChanId);
HI_S32  DMX_OsiDescramblerDetach(HI_U32 KeyId, HI_U32 ChanId);
HI_S32  DMX_OsiDescramblerGetFreeKeyNum(HI_U32 *FreeCount);
HI_S32  DMX_OsiDescramblerGetKeyId(HI_U32 ChanId, HI_U32 *KeyId);

#ifdef HI_DEMUX_PROC_SUPPORT
DMX_KeyInfo_S* DMX_OsiGetKeyProc(HI_U32 KeyId);
#endif

#ifdef __cplusplus
}
#endif

#endif  // __DRV_DESCRAMBLER_FUNC_H__

