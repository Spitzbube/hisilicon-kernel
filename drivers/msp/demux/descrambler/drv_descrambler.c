/******************************************************************************
 Copyright (C), 2009-2019, Hisilicon Tech. Co., Ltd.
 ******************************************************************************
 File Name     : drv_descrambler.c
 Version       : Initial Draft
 Author        : Hisilicon multimedia software group
 Created       : 2013/04/16
 Description   :
******************************************************************************/

#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/major.h>
#include <asm/types.h>
#include <linux/fs.h>
#include <asm/atomic.h>

#include "hi_type.h"
#include "hi_drv_stat.h"
#include "hi_kernel_adapt.h"

#include "hi_unf_descrambler.h"
#include "hi_drv_descrambler.h"

#include "demux_debug.h"
#include "drv_demux_ioctl.h"
#include "drv_demux_config.h"
#include "drv_demux_define.h"
#include "drv_descrambler.h"
#include "drv_descrambler_func.h"
#include "drv_descrambler_ioctl.h"

#define DMX_KEYID(KeyHandle)    ((KeyHandle) & 0xff)

#define DMX_KEYHANDLE(KeyId)    ((KeyId) | 0x00000300 | (HI_ID_DEMUX << 16))

#define DMX_CHECK_KEYHANDLE(KeyHandle)                                  \
    do                                                                  \
    {                                                                   \
        if (   (DMX_KEYID(KeyHandle) >= DMX_KEY_CNT)                    \
            || (((KeyHandle) & 0xffffff00) != DMX_KEYHANDLE(0)) )       \
        {                                                               \
            HI_WARN_DEMUX("Invalid KeyHandle 0x%x\n", KeyHandle);       \
            return HI_ERR_DMX_INVALID_PARA;                             \
        }                                                               \
    } while (0)

typedef struct
{
    HI_U32  KeyFile[DMX_KEY_CNT];
} DMX_DEV_DSCR_S;

static DMX_DEV_DSCR_S   s_DmxDscrFile;

HI_S32 DMXKeyIoctl(struct file *file, HI_U32 cmd, HI_VOID *arg)
{
    HI_S32 ret = HI_FAILURE;

    switch (cmd)
    {
        case CMD_DEMUX_KEYS_NEW:
        {
            DMX_NewKey_S *Param = (DMX_NewKey_S*)arg;

            ret = HI_DRV_DMX_CreateDescrambler(Param->DmxId, &Param->DesramblerAttr, &Param->KeyHandle, (HI_U32)file);

            break;
        }

        case CMD_DEMUX_KEYS_DEL:
        {
            ret = HI_DRV_DMX_DestroyDescrambler(*(HI_HANDLE*)arg);

            break;
        }

        case CMD_DEMUX_KEYS_SET_EVEN:
        {
            DMX_KeySet_S *Param = (DMX_KeySet_S*)arg;

            ret = HI_DRV_DMX_SetDescramblerEvenKey(Param->KeyHandle, Param->Key);

            HI_DRV_STAT_Event(STAT_EVENT_CWSET, 0);

            break;
        }

        case CMD_DEMUX_KEYS_SET_ODD:
        {
            DMX_KeySet_S *Param = (DMX_KeySet_S*)arg;

            ret = HI_DRV_DMX_SetDescramblerOddKey(Param->KeyHandle, Param->Key);

            break;
        }

#ifdef DMX_DESCRAMBLER_VERSION_1
        case CMD_DEMUX_KEYS_SET_IVEVEN:
        {
            DMX_KeySet_S *Param = (DMX_KeySet_S*)arg;

            ret = HI_DRV_DMX_SetDescramblerEvenIVKey(Param->KeyHandle, Param->Key);

            break;
        }

        case CMD_DEMUX_KEYS_SET_IVODD:
        {
            DMX_KeySet_S *Param = (DMX_KeySet_S*)arg;

            ret = HI_DRV_DMX_SetDescramblerOddIVKey(Param->KeyHandle, Param->Key);

            break;
        }
#endif

        case CMD_DEMUX_KEYS_ATTACH:
        {
            DMX_KeyAttach_S *Param = (DMX_KeyAttach_S*)arg;

            ret = HI_DRV_DMX_AttachDescrambler(Param->KeyHandle, Param->ChanHandle);

            break;
        }

        case CMD_DEMUX_KEYS_DETACH:
        {
            DMX_KeyAttach_S *Param = (DMX_KeyAttach_S*)arg;

            ret = HI_DRV_DMX_DetachDescrambler(Param->KeyHandle, Param->ChanHandle);

            break;
        }

        case CMD_DEMUX_KEYS_GET_ID:
        {
            DMX_KeyAttach_S *Param = (DMX_KeyAttach_S*)arg;

            ret = HI_DRV_DMX_GetDescramblerKeyHandle(Param->ChanHandle, &Param->KeyHandle);

            break;
        }

        case CMD_DEMUX_KEYS_GET_FREE:
        {
            DMX_FreeKeyGet_S *Param = (DMX_FreeKeyGet_S*)arg;

            ret = HI_DRV_DMX_GetFreeDescramblerKeyCount(Param->DmxId, &Param->FreeCount);

            break;
        }

        default:
        {
            HI_WARN_DEMUX("unknown cmd: 0x%x\n", cmd);
        }
    }

    return ret;
}

HI_VOID DmxDestroyAllDescrambler(HI_U32 file)
{
    HI_U32 i;

    for (i = 0; i < DMX_KEY_CNT; i++)
    {
        if (s_DmxDscrFile.KeyFile[i] == file)
        {
            HI_DRV_DMX_DestroyDescrambler(DMX_KEYHANDLE(i));
        }
    }
}

#ifdef HI_DEMUX_PROC_SUPPORT
HI_S32 DMXKeyProcRead(struct seq_file *p, HI_VOID *v)
{
    HI_U32 KeyId;

    PROC_PRINT(p, "Id ChanCnt EvenKey                             OddKey\n");

    for (KeyId = 0; KeyId < DMX_KEY_CNT; KeyId++)
    {
        DMX_KeyInfo_S *KeyInfo;

        KeyInfo = DMX_OsiGetKeyProc(KeyId);
        if (!KeyInfo)
        {
            continue;
        }

        PROC_PRINT(p, "%2u   %2u    %08x %08x %08x %08x %08x %08x %08x %08x\n",
                KeyId,
                KeyInfo->ChanCount,
                KeyInfo->EvenKey[0],
                KeyInfo->EvenKey[1],
                KeyInfo->EvenKey[2],
                KeyInfo->EvenKey[3],
                KeyInfo->OddKey[0],
                KeyInfo->OddKey[1],
                KeyInfo->OddKey[2],
                KeyInfo->OddKey[3]
            );
    }

    return HI_SUCCESS;
}
#endif

HI_S32 HI_DRV_DMX_CreateDescrambler(HI_U32 DmxId, HI_UNF_DMX_DESCRAMBLER_ATTR_S *DescAttr, HI_HANDLE *KeyHandle, HI_U32 file)
{
    HI_S32 ret;
    HI_U32 KeyId;

    CHECKDMXID(DmxId);
    CHECKPOINTER(DescAttr);
    CHECKPOINTER(KeyHandle);

    ret = DMX_OsiDescramblerCreate(&KeyId, DescAttr);
    if (HI_SUCCESS == ret)
    {
        s_DmxDscrFile.KeyFile[KeyId] = (HI_U32)file;

        *KeyHandle = DMX_KEYHANDLE(KeyId);
    }

    return ret;
}

HI_S32 HI_DRV_DMX_DestroyDescrambler(HI_HANDLE KeyHandle)
{
    HI_S32 ret;

    DMX_CHECK_KEYHANDLE(KeyHandle);

    ret = DMX_OsiDescramblerDestroy(DMX_KEYID(KeyHandle));
    if (HI_SUCCESS == ret)
    {
        s_DmxDscrFile.KeyFile[DMX_KEYID(KeyHandle)] = 0;
    }

    return ret;
}

HI_S32 HI_DRV_DMX_SetDescramblerEvenKey(HI_HANDLE KeyHandle, HI_U8 *Key)
{
    DMX_CHECK_KEYHANDLE(KeyHandle);

    return DMX_OsiDescramblerSetKey(DMX_KEYID(KeyHandle), DMX_KEY_TYPE_EVEN, Key);
}

HI_S32 HI_DRV_DMX_SetDescramblerOddKey(HI_HANDLE KeyHandle, HI_U8 *Key)
{
    DMX_CHECK_KEYHANDLE(KeyHandle);

    return DMX_OsiDescramblerSetKey(DMX_KEYID(KeyHandle), DMX_KEY_TYPE_ODD, Key);
}

#ifdef DMX_DESCRAMBLER_VERSION_1
HI_S32 HI_DRV_DMX_SetDescramblerEvenIVKey(HI_HANDLE KeyHandle, HI_U8 *Key)
{
    DMX_CHECK_KEYHANDLE(KeyHandle);

    return DMX_OsiDescramblerSetIVKey(DMX_KEYID(KeyHandle), DMX_KEY_TYPE_EVEN, Key);
}

HI_S32 HI_DRV_DMX_SetDescramblerOddIVKey(HI_HANDLE KeyHandle, HI_U8 *Key)
{
    DMX_CHECK_KEYHANDLE(KeyHandle);

    return DMX_OsiDescramblerSetIVKey(DMX_KEYID(KeyHandle), DMX_KEY_TYPE_ODD, Key);
}
#endif

HI_S32 HI_DRV_DMX_AttachDescrambler(HI_HANDLE KeyHandle, HI_HANDLE ChanHandle)
{
    DMX_CHECK_KEYHANDLE(KeyHandle);
    DMX_CHECK_CHANHANDLE(ChanHandle);

    return DMX_OsiDescramblerAttach(DMX_KEYID(KeyHandle), DMX_CHANID(ChanHandle));
}

HI_S32 HI_DRV_DMX_DetachDescrambler(HI_HANDLE KeyHandle, HI_HANDLE ChanHandle)
{
    DMX_CHECK_KEYHANDLE(KeyHandle);
    DMX_CHECK_CHANHANDLE(ChanHandle);

    return DMX_OsiDescramblerDetach(DMX_KEYID(KeyHandle), DMX_CHANID(ChanHandle));
}

HI_S32 HI_DRV_DMX_GetDescramblerKeyHandle(HI_HANDLE ChanHandle, HI_HANDLE *KeyHandle)
{
    HI_S32  ret;
    HI_U32  KeyId;

    CHECKPOINTER(KeyHandle);
    DMX_CHECK_CHANHANDLE(ChanHandle);

    ret = DMX_OsiDescramblerGetKeyId(DMX_CHANID(ChanHandle), &KeyId);
    if (HI_SUCCESS == ret)
    {
        *KeyHandle = DMX_KEYHANDLE(KeyId);
    }

    return ret;
}

HI_S32 HI_DRV_DMX_GetFreeDescramblerKeyCount(HI_U32 DmxId, HI_U32 *FreeCount)
{
    CHECKDMXID(DmxId);
    CHECKPOINTER(FreeCount);

    return DMX_OsiDescramblerGetFreeKeyNum(FreeCount);
}

