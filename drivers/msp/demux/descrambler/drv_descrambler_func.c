/******************************************************************************
 Copyright (C), 2009-2019, Hisilicon Tech. Co., Ltd.
 ******************************************************************************
 File Name     : drv_descrambler_func.c
 Version       : Initial Draft
 Author        : Hisilicon multimedia software group
 Created       : 2013/04/16
 Description   :
******************************************************************************/

#include <linux/kernel.h>

#include "hi_type.h"
#include "hi_module.h"
#include "hi_drv_module.h"
#include "hi_kernel_adapt.h"

#include "hi_unf_descrambler.h"

#include "demux_debug.h"
#include "drv_demux_config.h"
#include "drv_demux_define.h"
#include "drv_descrambler.h"
#include "drv_descrambler_func.h"
#include "drv_descrambler_reg.h"
#include "hal_descrambler.h"

#include "drv_advca_ext.h"

extern DMX_DEV_OSI_S *g_pDmxDevOsi;

#ifdef CHIP_TYPE_hi3716m
extern HI_BOOL Hi3716MV300Flag;
#endif

static ADVCA_EXPORT_FUNC_S *g_pAdvcaFunc = HI_NULL;

#ifdef DMX_DESCRAMBLER_VERSION_1
HI_VOID DescInitHardFlag(HI_VOID)
{
    DMX_DEV_OSI_S  *DmxDevOsi = g_pDmxDevOsi;
    U_CA_INFO0      CaInfo;

    DmxHalInitSpeCWOrder();

    DmxDevOsi->KeyCsa2HardFlag  = 0;
    DmxDevOsi->KeyCsa3HardFlag  = 0;
    DmxDevOsi->KeySpeHardFlag   = 0;
    DmxDevOsi->KeyOtherHardFlag = 0;

    CaInfo.value = DmxHalGetOptCAType();
    if (CaInfo.bits.hardonly_csa2)
    {
        DmxDevOsi->KeyCsa2HardFlag = DMX_KEY_HARDONLY_FLAG;
    }

    if (CaInfo.bits.hardonly_spe)
    {
        DmxDevOsi->KeySpeHardFlag = DMX_KEY_HARDONLY_FLAG;
    }

    if (CaInfo.bits.hardonly_csa3)
    {
        DmxDevOsi->KeyCsa3HardFlag = DMX_KEY_HARDONLY_FLAG;
    }

    if (0 == CaInfo.value)  // not handonly, open iv enable
    {
        HI_U32 DmxId;

        for (DmxId = 0; DmxId < DMX_CNT; DmxId++)
        {
            DmxHalSetDmxIvEnable(DmxId, HI_TRUE);
        }
    }
}
#endif

HI_VOID DescramblerReset(HI_U32 KeyId, DMX_KeyInfo_S *KeyInfo)
{
#ifdef CHIP_TYPE_hi3716m
    if (!Hi3716MV300Flag)
    {
        HI_U32 i;

        for (i = 0; i < DMX_KEY_MAX_LEN / sizeof(HI_U32); i++)
        {
            DmxHalSetCWWord(KeyId, i, 0, DMX_KEY_TYPE_EVEN);
            DmxHalSetCWWord(KeyId, i, 0, DMX_KEY_TYPE_ODD);
        }
    }
#else
#ifdef DMX_DESCRAMBLER_VERSION_0
    HI_U32 i;

    for (i = 0; i < DMX_KEY_MAX_LEN / sizeof(HI_U32); i++)
    {
        DmxHalSetCWWord(KeyId, i, 0, DMX_KEY_TYPE_EVEN);
        DmxHalSetCWWord(KeyId, i, 0, DMX_KEY_TYPE_ODD);
    }
#endif
#endif

#ifdef HI_DEMUX_PROC_SUPPORT
    KeyInfo->ChanCount = 0;

    memset(KeyInfo->EvenKey, 0, sizeof(KeyInfo->EvenKey));
    memset(KeyInfo->OddKey, 0, sizeof(KeyInfo->OddKey));
#endif

    KeyInfo->CaType = HI_UNF_DMX_CA_BUTT;
}

#ifdef DMX_DESCRAMBLER_VERSION_0
HI_S32 DMXDescramblerCreate0(HI_U32 *KeyId, HI_UNF_DMX_DESCRAMBLER_ATTR_S *Attr)
{
    HI_S32          ret         = HI_ERR_DMX_NOFREE_KEY;
    DMX_DEV_OSI_S  *DmxDevOsi   = g_pDmxDevOsi;
    DMX_KeyInfo_S  *KeyInfo     = DmxDevOsi->DmxKeyInfo;
    HI_U32          KeyLen      = DMX_KEY_MAX_LEN / sizeof(HI_U32);
    HI_U32          IdCaOrKey   = DMX_KEY_CNT;
    HI_U32          IdFree      = DMX_KEY_CNT;
    HI_U32          Id;
    HI_U32          OptCA;
    HI_U32          OtpKey;
    HI_U32          OnlyCa;
    HI_U32          OnlyKey;
    HI_U32          and;
    HI_U32          nor;

    switch (Attr->enDescramblerType)
    {
        case HI_UNF_DMX_DESCRAMBLER_TYPE_CSA2 :
 //           KeyLen = DMX_KEY_MIN_LEN / sizeof(HI_U32);
            break;

    #ifdef DMX_DESCRAMBLER_TYPE_CSA3_SUPPORT
        case HI_UNF_DMX_DESCRAMBLER_TYPE_CSA3 :
            break;
    #endif

        default :
            HI_WARN_DEMUX("DescramblerType=%d\n", Attr->enDescramblerType);

            return HI_ERR_DMX_INVALID_PARA;
    }

    OptCA   = DmxHalGetOptCAType();
    OtpKey  = DmxHalGetOptDescramblerType();

    and     = OptCA & OtpKey;
    nor     = OptCA ^ OtpKey;
    OnlyCa  = nor & OptCA;
    OnlyKey = nor & OtpKey;

    if (0 != down_interruptible(&DmxDevOsi->lock_Key))
    {
        return HI_ERR_DMX_BUSY;
    }

    for (Id = 0; Id < DMX_KEY_CNT; Id++)
    {
        if (HI_UNF_DMX_CA_BUTT != KeyInfo[Id].CaType)
        {
            continue;
        }

        if ((HI_UNF_DMX_CA_ADVANCE == Attr->enCaType) && (HI_UNF_DMX_DESCRAMBLER_TYPE_CSA3 == Attr->enDescramblerType))
        {
            if (and & (1 << Id))
            {
                break;
            }

            if (DMX_KEY_CNT == IdCaOrKey)
            {
                if (nor & (1 << Id))
                {
                    IdCaOrKey = Id;
                }
            }
        }
        else if (HI_UNF_DMX_CA_ADVANCE == Attr->enCaType)
        {
            if (OnlyCa & (1 << Id))
            {
                break;
            }
        }
        else if (HI_UNF_DMX_DESCRAMBLER_TYPE_CSA3 == Attr->enDescramblerType)
        {
            if (OnlyKey & (1 << Id))
            {
                break;
            }
        }

        if (DMX_KEY_CNT == IdFree)
        {
            HI_U32 mask = 1 << Id;

            if ((0 == (OptCA & mask)) && (0 == (OtpKey & mask)))
            {
                IdFree = Id;
            }
        }
    }

    if (DMX_KEY_CNT == Id)
    {
        if (IdCaOrKey < DMX_KEY_CNT)
        {
            Id = IdCaOrKey;

            if (OnlyCa & (1 << Id))
            {
                DmxHalSetDescramblerType(Id, HI_TRUE);
            }
            else
            {
                DmxHalSetCAType(Id, HI_TRUE);
            }
        }
        else if (IdFree < DMX_KEY_CNT)
        {
            Id = IdFree;

            DmxHalSetCAType(Id, (HI_UNF_DMX_CA_ADVANCE == Attr->enCaType));
            DmxHalSetDescramblerType(Id, (HI_UNF_DMX_DESCRAMBLER_TYPE_CSA3 == Attr->enDescramblerType));
        }
    }

    if (Id < DMX_KEY_CNT)
    {
        DmxHalSetEntropyReduction(Id, Attr->enEntropyReduction);

        KeyInfo[Id].CaType      = Attr->enCaType;
        KeyInfo[Id].CaEntropy   = Attr->enEntropyReduction;
        KeyInfo[Id].DescType    = Attr->enDescramblerType;
        KeyInfo[Id].KeyLen      = KeyLen;

        *KeyId = Id;

        ret = HI_SUCCESS;
    }
    up(&DmxDevOsi->lock_Key);

    return ret;
}
#endif

#ifdef DMX_DESCRAMBLER_VERSION_1
static HI_U32 DescGetTableId(HI_UNF_DMX_DESCRAMBLER_TYPE_E DescType)
{
    switch (DescType)
    {
        case HI_UNF_DMX_DESCRAMBLER_TYPE_CSA2 :
            return 0;

        case HI_UNF_DMX_DESCRAMBLER_TYPE_CSA3 :
            return 1;

        case HI_UNF_DMX_DESCRAMBLER_TYPE_AES_IPTV :
        case HI_UNF_DMX_DESCRAMBLER_TYPE_AES_ECB :
        case HI_UNF_DMX_DESCRAMBLER_TYPE_AES_CI:
            return 2;

        case HI_UNF_DMX_DESCRAMBLER_TYPE_DES_CI :
        case HI_UNF_DMX_DESCRAMBLER_TYPE_DES_CBC :
        case HI_UNF_DMX_DESCRAMBLER_TYPE_AES_NS :
        case HI_UNF_DMX_DESCRAMBLER_TYPE_SMS4_NS :
        default :
            return 3;
    }
}

HI_S32 DescCreate(HI_U32 KeyId, HI_U32 CaType, HI_U32 DescType, HI_U32 CaEntropy)
{
    DMX_DEV_OSI_S  *DmxDevOsi               = g_pDmxDevOsi;
    HI_U8           key[DMX_KEY_MAX_LEN]    = {0};
    HI_U32          TabId;
    HI_U32         *HardFlag;

    TabId = DescGetTableId(DescType);
    switch (TabId)
    {
        case 0 :
            HardFlag = &DmxDevOsi->KeyCsa2HardFlag;
            break;

        case 1 :
            HardFlag = &DmxDevOsi->KeyCsa3HardFlag;
            break;

        case 2 :
            HardFlag = &DmxDevOsi->KeySpeHardFlag;
            break;

        default :
            HardFlag = &DmxDevOsi->KeyOtherHardFlag;
    }

    if (HI_UNF_DMX_CA_NORMAL == CaType)
    {
        if (*HardFlag & (1 << KeyId))
        {
            return HI_FAILURE;
        }
    }
    else
    {
        *HardFlag |= (1 << KeyId);
    }

    if (HI_UNF_DMX_DESCRAMBLER_TYPE_CSA2 == DescType)
    {
        DmxHalSetEntropyReduction(KeyId, CaEntropy);
    }

    DMX_OsiDescramblerSetIVKey(KeyId, DMX_KEY_TYPE_EVEN, key);
    DMX_OsiDescramblerSetIVKey(KeyId, DMX_KEY_TYPE_ODD, key);

    return HI_SUCCESS;
}

/***********************************************************************************
* Function      : DMXDescramblerCreate1
* Description   : create a new descrambler
* Input         :
* Output        : KeyId
* Return        : HI_SUCCESS
*                 HI_FAILURE
* Others:
***********************************************************************************/
HI_S32 DMXDescramblerCreate1(HI_U32 *KeyId, HI_UNF_DMX_DESCRAMBLER_ATTR_S *Attr)
{
    HI_S32          ret         = HI_ERR_DMX_NOFREE_KEY;
    DMX_DEV_OSI_S  *DmxDevOsi   = g_pDmxDevOsi;
    DMX_KeyInfo_S  *KeyInfo     = DmxDevOsi->DmxKeyInfo;
    HI_U32          KeyLen      = DMX_KEY_MAX_LEN / sizeof(HI_U32);
    U_CA_INFO0      CaInfo;
    HI_U32          i;

    CaInfo.value = DmxHalGetOptCAType();

    switch (Attr->enDescramblerType)
    {
        case HI_UNF_DMX_DESCRAMBLER_TYPE_CSA2 :
            if (CaInfo.bits.dis_csa2)
            {
                HI_WARN_DEMUX("disable CSA2\n");

                return HI_ERR_DMX_NOT_SUPPORT;
            }

            if ((HI_UNF_DMX_CA_NORMAL == Attr->enCaType) && (DMX_KEY_HARDONLY_FLAG == DmxDevOsi->KeyCsa2HardFlag))
            {
                return HI_ERR_DMX_NOT_SUPPORT;
            }

            //KeyLen = DMX_KEY_MIN_LEN / sizeof(HI_U32);

            break;

        case HI_UNF_DMX_DESCRAMBLER_TYPE_CSA3 :
        #ifdef DMX_DESCRAMBLER_TYPE_CSA3_SUPPORT
            if (CaInfo.bits.dis_csa3)
            {
                HI_WARN_DEMUX("disable CSA3\n");

                return HI_ERR_DMX_NOT_SUPPORT;
            }

            if ((HI_UNF_DMX_CA_NORMAL == Attr->enCaType) && (DMX_KEY_HARDONLY_FLAG == DmxDevOsi->KeyCsa3HardFlag))
            {
                return HI_ERR_DMX_NOT_SUPPORT;
            }
        #else
            return HI_ERR_DMX_NOT_SUPPORT;
        #endif
            break;

        case HI_UNF_DMX_DESCRAMBLER_TYPE_AES_IPTV :
        case HI_UNF_DMX_DESCRAMBLER_TYPE_AES_ECB :
        case HI_UNF_DMX_DESCRAMBLER_TYPE_AES_CI :
        #ifdef DMX_DESCRAMBLER_TYPE_SPE_SUPPORT
            if (CaInfo.bits.dis_spe)
            {
                HI_WARN_DEMUX("disable SPE\n");

                return HI_ERR_DMX_NOT_SUPPORT;
            }

            if ((HI_UNF_DMX_CA_NORMAL == Attr->enCaType) && (DMX_KEY_HARDONLY_FLAG == DmxDevOsi->KeySpeHardFlag))
            {
                return HI_ERR_DMX_NOT_SUPPORT;
            }
        #else
            return HI_ERR_DMX_NOT_SUPPORT;
        #endif
            break;

        case HI_UNF_DMX_DESCRAMBLER_TYPE_DES_CI :
        case HI_UNF_DMX_DESCRAMBLER_TYPE_DES_CBC :
        {
        #ifdef DMX_DESCRAMBLER_TYPE_DES_SUPPORT
            if (CaInfo.bits.dis_des)
            {
                HI_WARN_DEMUX("disable DES\n");

                return HI_ERR_DMX_NOT_SUPPORT;
            }

            if (   (HI_UNF_DMX_CA_NORMAL == Attr->enCaType)
                && (CaInfo.bits.hardonly_des || (DMX_KEY_HARDONLY_FLAG == DmxDevOsi->KeyOtherHardFlag)) )
            {
                return HI_ERR_DMX_NOT_SUPPORT;
            }

            //KeyLen = DMX_KEY_MIN_LEN / sizeof(HI_U32);

            break;
        #else
            return HI_ERR_DMX_NOT_SUPPORT;
        #endif
        }

        case HI_UNF_DMX_DESCRAMBLER_TYPE_DES_IPTV :
        {
        #ifdef DMX_DESCRAMBLER_TYPE_DES_IPTV_SUPPORT
            if (   (HI_UNF_DMX_CA_NORMAL == Attr->enCaType)
                && (DMX_KEY_HARDONLY_FLAG == DmxDevOsi->KeyOtherHardFlag) )
            {
                return HI_ERR_DMX_NOT_SUPPORT;
            }

            break;
        #else
            return HI_ERR_DMX_NOT_SUPPORT;
        #endif
        }

        case HI_UNF_DMX_DESCRAMBLER_TYPE_AES_NS :
        #ifdef DMX_DESCRAMBLER_TYPE_AES_NS_SUPPORT
            if (CaInfo.bits.dis_novel)
            {
                HI_WARN_DEMUX("disable novel\n");

                return HI_ERR_DMX_NOT_SUPPORT;
            }

            if (   (HI_UNF_DMX_CA_NORMAL == Attr->enCaType)
                && (CaInfo.bits.hardonly_novel || (DMX_KEY_HARDONLY_FLAG == DmxDevOsi->KeyOtherHardFlag)) )
            {
                return HI_ERR_DMX_NOT_SUPPORT;
            }
        #else
            return HI_ERR_DMX_NOT_SUPPORT;
        #endif
            break;

        case HI_UNF_DMX_DESCRAMBLER_TYPE_SMS4_NS :
        #ifdef DMX_DESCRAMBLER_TYPE_SMS4_NS_SUPPORT
            if (CaInfo.bits.dis_novel)
            {
                HI_WARN_DEMUX("disable novel\n");

                return HI_ERR_DMX_NOT_SUPPORT;
            }

            if (   (HI_UNF_DMX_CA_NORMAL == Attr->enCaType)
                && (CaInfo.bits.hardonly_novel || (DMX_KEY_HARDONLY_FLAG == DmxDevOsi->KeyOtherHardFlag)) )
            {
                return HI_ERR_DMX_NOT_SUPPORT;
            }
        #else
            return HI_ERR_DMX_NOT_SUPPORT;
        #endif
            break;

        case HI_UNF_DMX_DESCRAMBLER_TYPE_SMS4_IPTV :
        #ifdef DMX_DESCRAMBLER_TYPE_SMS4_IPTV_SUPPORT
            if (CaInfo.bits.dis_others)
            {
                HI_WARN_DEMUX("disable others\n");

                return HI_ERR_DMX_NOT_SUPPORT;
            }

            if (   (HI_UNF_DMX_CA_NORMAL == Attr->enCaType)
                && (CaInfo.bits.hardonly_others || (DMX_KEY_HARDONLY_FLAG == DmxDevOsi->KeyOtherHardFlag)) )
            {
                return HI_ERR_DMX_NOT_SUPPORT;
            }
        #else
            return HI_ERR_DMX_NOT_SUPPORT;
        #endif
            break;

        case HI_UNF_DMX_DESCRAMBLER_TYPE_SMS4_ECB :
        #ifdef DMX_DESCRAMBLER_TYPE_SMS4_ECB_SUPPORT
            if (CaInfo.bits.dis_others)
            {
                HI_WARN_DEMUX("disable others\n");

                return HI_ERR_DMX_NOT_SUPPORT;
            }

            if (   (HI_UNF_DMX_CA_NORMAL == Attr->enCaType)
                && (CaInfo.bits.hardonly_others || (DMX_KEY_HARDONLY_FLAG == DmxDevOsi->KeyOtherHardFlag)) )
            {
                return HI_ERR_DMX_NOT_SUPPORT;
            }
        #else
            return HI_ERR_DMX_NOT_SUPPORT;
        #endif
            break;

        case HI_UNF_DMX_DESCRAMBLER_TYPE_SMS4_CBC :
        #ifdef DMX_DESCRAMBLER_TYPE_SMS4_CBC_SUPPORT
            if (CaInfo.bits.dis_others)
            {
                HI_WARN_DEMUX("disable others\n");

                return HI_ERR_DMX_NOT_SUPPORT;
            }

            if (   (HI_UNF_DMX_CA_NORMAL == Attr->enCaType)
                && (CaInfo.bits.hardonly_others || (DMX_KEY_HARDONLY_FLAG == DmxDevOsi->KeyOtherHardFlag)) )
            {
                return HI_ERR_DMX_NOT_SUPPORT;
            }
        #else
            return HI_ERR_DMX_NOT_SUPPORT;
        #endif
            break;

        case HI_UNF_DMX_DESCRAMBLER_TYPE_AES_CBC :
        #ifdef DMX_DESCRAMBLER_TYPE_AES_CBC_SUPPORT
            if (CaInfo.bits.dis_others)
            {
                HI_WARN_DEMUX("disable others\n");

                return HI_ERR_DMX_NOT_SUPPORT;
            }

            if (   (HI_UNF_DMX_CA_NORMAL == Attr->enCaType)
                && (CaInfo.bits.hardonly_others || (DMX_KEY_HARDONLY_FLAG == DmxDevOsi->KeyOtherHardFlag)) )
            {
                return HI_ERR_DMX_NOT_SUPPORT;
            }
        #else
            return HI_ERR_DMX_NOT_SUPPORT;
        #endif
            break;

        case HI_UNF_DMX_DESCRAMBLER_TYPE_TDES_IPTV :
        case HI_UNF_DMX_DESCRAMBLER_TYPE_TDES_ECB :
        case HI_UNF_DMX_DESCRAMBLER_TYPE_TDES_CBC :
        {
        #ifdef DMX_DESCRAMBLER_TYPE_TDES_SUPPORT
            if (CaInfo.bits.dis_tdes)
            {
                HI_WARN_DEMUX("disable tdes\n");

                return HI_ERR_DMX_NOT_SUPPORT;
            }

            if (   (HI_UNF_DMX_CA_NORMAL == Attr->enCaType)
                && (CaInfo.bits.hardonly_others || (DMX_KEY_HARDONLY_FLAG == DmxDevOsi->KeyOtherHardFlag)) )
            {
                return HI_ERR_DMX_NOT_SUPPORT;
            }

            break;
        #else
            return HI_ERR_DMX_NOT_SUPPORT;
        #endif
        }

        default :
            HI_WARN_DEMUX("Invalid DescramblerType %d\n", Attr->enDescramblerType);

            return HI_ERR_DMX_INVALID_PARA;
    }

    if (0 == down_interruptible(&DmxDevOsi->lock_Key))
    {
        for (i = 0; i < DMX_KEY_CNT; i++)
        {
            if (HI_UNF_DMX_CA_BUTT != KeyInfo[i].CaType)
            {
                continue;
            }

            if (HI_SUCCESS == DescCreate(i, Attr->enCaType, Attr->enDescramblerType, Attr->enEntropyReduction))
            {
                KeyInfo[i].CaType       = Attr->enCaType;
                KeyInfo[i].CaEntropy    = Attr->enEntropyReduction;
                KeyInfo[i].DescType     = Attr->enDescramblerType;
                KeyInfo[i].KeyLen       = KeyLen;

                *KeyId = i;

                ret = HI_SUCCESS;

                break;
            }
        }
        up(&DmxDevOsi->lock_Key);
    }
    else
    {
        ret = HI_ERR_DMX_BUSY;
    }

    return ret;
}
#endif

HI_VOID DmxDescramblerResume(HI_VOID)
{
    HI_U32 i;

    for (i = 0; i < DMX_KEY_CNT; i++)
    {
        DMX_KeyInfo_S *KeyInfo = &g_pDmxDevOsi->DmxKeyInfo[i];

        if (HI_UNF_DMX_CA_BUTT != KeyInfo->CaType)
        {
        #ifdef DMX_DESCRAMBLER_VERSION_0
            HI_U32  OptCA   = DmxHalGetOptCAType();
            HI_U32  OtpKey  = DmxHalGetOptDescramblerType();
        #endif

    #ifdef CHIP_TYPE_hi3716m
            if (Hi3716MV300Flag)
            {
                DescCreate(i, KeyInfo->CaType, KeyInfo->DescType, KeyInfo->CaEntropy);
            }
            else
            {
                if (0 == (OptCA & (1 << i)))
                {
                    DmxHalSetCAType(i, (HI_UNF_DMX_CA_ADVANCE == KeyInfo->CaType));
                }

                if (0 == (OtpKey & (1 << i)))
                {
                    DmxHalSetDescramblerType(i, (HI_UNF_DMX_DESCRAMBLER_TYPE_CSA3 == KeyInfo->DescType));
                }

                DmxHalSetEntropyReduction(i, KeyInfo->CaEntropy);
            }
    #else
        #ifdef DMX_DESCRAMBLER_VERSION_0
            if (0 == (OptCA & (1 << i)))
            {
                DmxHalSetCAType(i, (HI_UNF_DMX_CA_ADVANCE == KeyInfo->CaType));
            }

            if (0 == (OtpKey & (1 << i)))
            {
                DmxHalSetDescramblerType(i, (HI_UNF_DMX_DESCRAMBLER_TYPE_CSA3 == KeyInfo->DescType));
            }

            DmxHalSetEntropyReduction(i, KeyInfo->CaEntropy);
        #endif

        #ifdef DMX_DESCRAMBLER_VERSION_1
            DescCreate(i, KeyInfo->CaType, KeyInfo->DescType, KeyInfo->CaEntropy);
        #endif
    #endif

        #ifdef HI_DEMUX_PROC_SUPPORT
            KeyInfo->ChanCount = 0;

            DMX_OsiDescramblerSetKey(i, DMX_KEY_TYPE_EVEN, (HI_U8*)KeyInfo->EvenKey);
            DMX_OsiDescramblerSetKey(i, DMX_KEY_TYPE_ODD, (HI_U8*)KeyInfo->OddKey);
        #endif
        }
    }
}

/***********************************************************************************
* Function      :   DMX_OsiNewDescrambler
* Description   :  apply a new descrambler
* Input         :
* Output        :  pKeyId
* Return        :  HI_SUCCESS:     success
*                  HI_FAILURE:
* Others:
***********************************************************************************/
HI_S32 DMX_OsiDescramblerCreate(HI_U32 *KeyId, const HI_UNF_DMX_DESCRAMBLER_ATTR_S *DescAttr)
{
    HI_UNF_DMX_DESCRAMBLER_ATTR_S Attr;

    if ((HI_UNF_DMX_CA_NORMAL != DescAttr->enCaType) && (HI_UNF_DMX_CA_ADVANCE != DescAttr->enCaType))
    {
        HI_WARN_DEMUX("CaType=%d\n", DescAttr->enCaType);

        return HI_ERR_DMX_INVALID_PARA;
    }

    Attr.enCaType           = DescAttr->enCaType;
    Attr.enDescramblerType  = DescAttr->enDescramblerType;
    Attr.enEntropyReduction = HI_UNF_DMX_CA_ENTROPY_REDUCTION_CLOSE;

    if (HI_UNF_DMX_DESCRAMBLER_TYPE_CSA2 == DescAttr->enDescramblerType)
    {
        if (   (HI_UNF_DMX_CA_ENTROPY_REDUCTION_CLOSE   != DescAttr->enEntropyReduction)
            && (HI_UNF_DMX_CA_ENTROPY_REDUCTION_OPEN    != DescAttr->enEntropyReduction) )
        {
            HI_WARN_DEMUX("EntropyReduction=%d\n", DescAttr->enEntropyReduction);

            return HI_ERR_DMX_INVALID_PARA;
        }

        Attr.enEntropyReduction = DescAttr->enEntropyReduction;
    }

#ifdef CHIP_TYPE_hi3716m
    if (Hi3716MV300Flag)
    {
        return DMXDescramblerCreate1(KeyId, &Attr);
    }
    else
    {
        return DMXDescramblerCreate0(KeyId, &Attr);
    }
#endif

#ifdef DMX_DESCRAMBLER_VERSION_0
    return DMXDescramblerCreate0(KeyId, &Attr);
#endif

#ifdef DMX_DESCRAMBLER_VERSION_1
    return DMXDescramblerCreate1(KeyId, &Attr);
#endif
}

/***********************************************************************************
* Function      : DMX_OsiDescramblerDestroy
* Description   : destroy a Descrambler
* Input         : KeyId
* Output        :
* Return        : HI_SUCCESS
*                 HI_FAILURE
* Others:
***********************************************************************************/
HI_S32 DMX_OsiDescramblerDestroy(HI_U32 KeyId)
{
    HI_S32          ret         = HI_ERR_DMX_INVALID_PARA;
    DMX_DEV_OSI_S  *DmxDevOsi   = g_pDmxDevOsi;
    DMX_KeyInfo_S  *KeyInfo     = &DmxDevOsi->DmxKeyInfo[KeyId];
    DMX_ChanInfo_S *ChanInfo    = DmxDevOsi->DmxChanInfo;
    HI_U32          i;

    if (HI_UNF_DMX_CA_BUTT != KeyInfo->CaType)
    {
        for (i = 0; i < DMX_CHANNEL_CNT; i++)
        {
            if (ChanInfo[i].KeyId == KeyId)
            {
                DmxHalSetChannelCWIndex(ChanInfo[i].ChanId, 0);

                DmxHalSetChannelDsc(ChanInfo[i].ChanId, HI_FALSE);
            }
        }

        ret = down_interruptible(&DmxDevOsi->lock_Key);
        DescramblerReset(KeyId, KeyInfo);
        up(&DmxDevOsi->lock_Key);

        ret = HI_SUCCESS;
    }

    return ret;
}

HI_S32 DMX_OsiDescramblerSetKey(HI_U32 KeyId, DMX_KEY_TYPE_E KeyType, HI_U8 *Key)
{
    DMX_KeyInfo_S *KeyInfo = &g_pDmxDevOsi->DmxKeyInfo[KeyId];
    HI_U32 i;
    DRV_ADVCA_EXTFUNC_PARAM_S stAdvcaFuncParam = {0};

    if (HI_UNF_DMX_CA_BUTT == KeyInfo->CaType)
    {
        return HI_ERR_DMX_INVALID_PARA;
    }

#ifdef HI_DEMUX_PROC_SUPPORT
    for (i = 0; i < KeyInfo->KeyLen; i++)
    {
        if (DMX_KEY_TYPE_EVEN == KeyType)
        {
            KeyInfo->EvenKey[i] = ((HI_U32*)Key)[i];
        }
        else
        {
            KeyInfo->OddKey[i] = ((HI_U32*)Key)[i];
        }
    }
#endif

    if (HI_UNF_DMX_CA_NORMAL == KeyInfo->CaType)
    {
        for (i = 0; i < KeyInfo->KeyLen; i++)
        {
    #ifdef CHIP_TYPE_hi3716m
            if (Hi3716MV300Flag)
            {
                DmxHalSetCWWord1(KeyId, i, ((HI_U32*)Key)[i], KeyType, DMX_KEY_CW, KeyInfo->DescType);
            }
            else
            {
                DmxHalSetCWWord(KeyId, i, ((HI_U32*)Key)[i], KeyType);
            }
    #else
        #ifdef DMX_DESCRAMBLER_VERSION_0
            DmxHalSetCWWord(KeyId, i, ((HI_U32*)Key)[i], KeyType);
        #endif

        #ifdef DMX_DESCRAMBLER_VERSION_1
            DmxHalSetCWWord1(KeyId, i, ((HI_U32*)Key)[i], KeyType, DMX_KEY_CW, KeyInfo->DescType);
        #endif
    #endif
        }
    }
    else
    {
        /*change as get the function pointer every time when setting CW*/
        HI_S32 ret;

        ret = HI_DRV_MODULE_GetFunction(HI_ID_CA, (HI_VOID**)&g_pAdvcaFunc);
        if (HI_SUCCESS != ret)
        {
            return HI_ERR_DMX_NOT_SUPPORT;
        }

        if (g_pAdvcaFunc && g_pAdvcaFunc->pfnAdvcaCrypto)
        {
            memset(&stAdvcaFuncParam, 0, sizeof(stAdvcaFuncParam));
            if (HI_UNF_DMX_DESCRAMBLER_TYPE_CSA2 == KeyInfo->DescType)
            {
                stAdvcaFuncParam.enCAType = HI_UNF_CIPHER_CA_TYPE_CSA2;
            }
            else if (HI_UNF_DMX_DESCRAMBLER_TYPE_CSA3 == KeyInfo->DescType)
            {
                stAdvcaFuncParam.enCAType = HI_UNF_CIPHER_CA_TYPE_CSA3;
            }
            else
            {
                stAdvcaFuncParam.enCAType = HI_UNF_CIPHER_CA_TYPE_SP;
            }

            stAdvcaFuncParam.AddrID = KeyId;
            stAdvcaFuncParam.EvenOrOdd = KeyType;
            stAdvcaFuncParam.pu8Data = (HI_U8 *)Key;
            stAdvcaFuncParam.bIsDeCrypt = HI_TRUE;
            stAdvcaFuncParam.enTarget = DRV_ADVCA_CA_TARGET_DEMUX;
            return (g_pAdvcaFunc->pfnAdvcaCrypto)(stAdvcaFuncParam);
        }
    }

    return HI_SUCCESS;
}

#ifdef DMX_DESCRAMBLER_VERSION_1
HI_S32 DMX_OsiDescramblerSetIVKey(HI_U32 KeyId, DMX_KEY_TYPE_E KeyType, HI_U8 *Key)
{
    DMX_KeyInfo_S  *KeyInfo = &g_pDmxDevOsi->DmxKeyInfo[KeyId];
    HI_U32          i;

    if (HI_UNF_DMX_CA_BUTT == KeyInfo->CaType)
    {
        return HI_ERR_DMX_INVALID_PARA;
    }

    if (HI_UNF_DMX_DESCRAMBLER_TYPE_AES_CI == KeyInfo->DescType)    //change iv cw order, AES CI+ request
    {
        HI_U32  len = KeyInfo->KeyLen * sizeof(HI_U32);
        HI_U8   tmp[DMX_KEY_MAX_LEN];

        memcpy(tmp, Key, len);

        for (i = 0 ; i < len ; i++)
        {
            Key[i] = tmp[len - i - 1];
        }
    }

    for (i = 0; i < KeyInfo->KeyLen; i++)
    {
        DmxHalSetCWWord1(KeyId, i, ((HI_U32*)Key)[i], KeyType, DMX_KEY_IV, KeyInfo->DescType);
    }

    return HI_SUCCESS;
}
#endif

/***********************************************************************************
* Function      : DMX_OsiDescramblerAttach
* Description   : attcg  Descrambler to channel
* Input         : KeyId, ChanId
* Output        :
* Return        : HI_SUCCESS
*                 HI_FAILURE
* Others        :
***********************************************************************************/
HI_S32 DMX_OsiDescramblerAttach(HI_U32 KeyId, HI_U32 ChanId)
{
    HI_S32          ret;
    DMX_DEV_OSI_S  *DmxDevOsi   = g_pDmxDevOsi;
    DMX_ChanInfo_S *ChanInfo    = &DmxDevOsi->DmxChanInfo[ChanId];
    DMX_KeyInfo_S  *KeyInfo     = &DmxDevOsi->DmxKeyInfo[KeyId];

    if ((HI_UNF_DMX_CA_BUTT == KeyInfo->CaType) || (DMX_INVALID_CHAN_ID == ChanInfo->ChanId))
    {
        return HI_ERR_DMX_INVALID_PARA;
    }

    if (DMX_INVALID_KEY_ID != ChanInfo->KeyId)
    {
        return HI_ERR_DMX_ATTACHED_KEY;
    }

    ret = down_interruptible(&DmxDevOsi->lock_Key);
    ChanInfo->KeyId = KeyId;

#ifdef HI_DEMUX_PROC_SUPPORT
    ++KeyInfo->ChanCount;
#endif

    up(&DmxDevOsi->lock_Key);

#ifdef CHIP_TYPE_hi3716m
    if (Hi3716MV300Flag)
    {
        DmxHalSetChannelCWIndex(ChanId, KeyId);
        DmxHalSetChanCwTabId(ChanId, DescGetTableId(KeyInfo->DescType));
    }
    else
    {
        DmxHalSetChannelCWIndex(ChanId, KeyId);
    }
#else
    #ifdef DMX_DESCRAMBLER_VERSION_0
    DmxHalSetChannelCWIndex(ChanId, KeyId);
    #endif

    #ifdef DMX_DESCRAMBLER_VERSION_1
    DmxHalSetChannelCWIndex(ChanId, KeyId);
    DmxHalSetChanCwTabId(ChanId, DescGetTableId(KeyInfo->DescType));
    #endif
#endif

    DmxHalSetChannelDsc(ChanId, HI_TRUE);

    return HI_SUCCESS;
}

/***********************************************************************************
* Function      : DMX_OsiDescramblerDetach
* Description   : dettach Descrambler from a channel
* Input         : KeyId, ChanId
* Output        :
* Return        : HI_SUCCESS
*                 HI_FAILURE
* Others        :
***********************************************************************************/
HI_S32 DMX_OsiDescramblerDetach(HI_U32 KeyId, HI_U32 ChanId)
{
    HI_S32          ret;
    DMX_DEV_OSI_S  *DmxDevOsi   = g_pDmxDevOsi;
    DMX_ChanInfo_S *ChanInfo    = &DmxDevOsi->DmxChanInfo[ChanId];
    DMX_KeyInfo_S  *KeyInfo     = &DmxDevOsi->DmxKeyInfo[KeyId];

    if (HI_UNF_DMX_CA_BUTT == KeyInfo->CaType)
    {
        HI_WARN_DEMUX("key %d has not been create yet\n", KeyId);

        return HI_ERR_DMX_INVALID_PARA;
    }

    if (DMX_INVALID_KEY_ID == ChanInfo->KeyId)
    {
        HI_WARN_DEMUX("Channel %d has not attached any descrambler\n", ChanId);

        return HI_ERR_DMX_NOATTACH_KEY;
    }

    if (ChanInfo->KeyId != KeyId)
    {
        HI_WARN_DEMUX("Detach Wrong Key from channel %d\n", ChanId);

        return HI_ERR_DMX_UNMATCH_KEY;
    }

    ret = down_interruptible(&DmxDevOsi->lock_Key);
    ChanInfo->KeyId = DMX_INVALID_KEY_ID;

#ifdef HI_DEMUX_PROC_SUPPORT
    --KeyInfo->ChanCount;
#endif

    up(&DmxDevOsi->lock_Key);

    DmxHalSetChannelCWIndex(ChanId, 0);

#ifdef CHIP_TYPE_hi3716m
    if (Hi3716MV300Flag)
    {
        DmxHalSetChanCwTabId(ChanId, 0);
    }
#else
    #ifdef DMX_DESCRAMBLER_VERSION_1
    DmxHalSetChanCwTabId(ChanId, 0);
    #endif
#endif

    DmxHalSetChannelDsc(ChanId, HI_FALSE);

    return HI_SUCCESS;
}

/***********************************************************************************
* Function      : DMX_OsiDescramblerGetFreeKeyNum
* Description   : get free Descramber Num
* Input         :
* Output        : FreeCount
* Return        : HI_SUCCESS
*                 HI_FAILURE
* Others        :
***********************************************************************************/
HI_S32 DMX_OsiDescramblerGetFreeKeyNum(HI_U32 *FreeCount)
{
    DMX_DEV_OSI_S  *DmxDevOsi   = g_pDmxDevOsi;
    DMX_KeyInfo_S  *KeyInfo     = DmxDevOsi->DmxKeyInfo;
    HI_U32          i;

    *FreeCount = 0;

    if (0 == down_interruptible(&DmxDevOsi->lock_Key))
    {
        for (i = 0; i < DMX_KEY_CNT; i++)
        {
            if (HI_UNF_DMX_CA_BUTT == KeyInfo[i].CaType)
            {
                ++(*FreeCount);
            }
        }
        up(&DmxDevOsi->lock_Key);

        return HI_SUCCESS;
    }

    return HI_ERR_DMX_BUSY;
}

/***********************************************************************************
* Function      : DMX_OsiDescramblerGetKeyId
* Description   : get key
* Input         : ChanId
* Output        : KeyId
* Return        : HI_SUCCESS
*                 HI_FAILURE
* Others        :
***********************************************************************************/
HI_S32 DMX_OsiDescramblerGetKeyId(HI_U32 ChanId, HI_U32 *KeyId)
{
    HI_S32          ret         = HI_ERR_DMX_INVALID_PARA;
    DMX_DEV_OSI_S  *DmxDevOsi   = g_pDmxDevOsi;
    DMX_ChanInfo_S *ChanInfo    = &DmxDevOsi->DmxChanInfo[ChanId];

    if (DMX_INVALID_CHAN_ID == ChanInfo->ChanId)
    {
        return ret;
    }

    ret = HI_ERR_DMX_NOATTACH_KEY;

    if (0 == down_interruptible(&DmxDevOsi->lock_Key))
    {
        if (ChanInfo->KeyId < DMX_KEY_CNT)
        {
            *KeyId = ChanInfo->KeyId;

            ret = HI_SUCCESS;
        }

        up(&DmxDevOsi->lock_Key);
    }
    else
    {
        ret = HI_ERR_DMX_BUSY;
    }

    return ret;
}

#ifdef HI_DEMUX_PROC_SUPPORT
DMX_KeyInfo_S* DMX_OsiGetKeyProc(HI_U32 KeyId)
{
    DMX_KeyInfo_S *KeyInfo;

    KeyInfo = &g_pDmxDevOsi->DmxKeyInfo[KeyId];
    if (HI_UNF_DMX_CA_BUTT == KeyInfo->CaType)
    {
        KeyInfo = HI_NULL;
    }

    return KeyInfo;
}
#endif

