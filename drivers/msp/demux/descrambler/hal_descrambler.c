/******************************************************************************
 Copyright (C), 2009-2019, Hisilicon Tech. Co., Ltd.
 ******************************************************************************
 File Name     : hal_descrambler.c
 Version       : Initial Draft
 Author        : Hisilicon multimedia software group
 Created       : 2013/04/16
 Description   :
******************************************************************************/

#include "hi_type.h"

#include "hi_unf_descrambler.h"

#include "demux_debug.h"
#include "drv_demux_config.h"
#include "drv_demux_reg.h"
#include "drv_descrambler_reg.h"
#include "hal_descrambler.h"

/***********************************************************************************
* Function      : DmxHalSetChannelCWIndex
* Description   : Set Channel CW Index
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalSetChannelCWIndex(HI_U32 ChanId, HI_U32 cwIndex)
{
    U_DMX_PID_CTRL dmx_pidctrl;

    dmx_pidctrl.u32 = DMX_READ_REG(DMX_PID_CTRL(ChanId));
    dmx_pidctrl.bits.cw_index = cwIndex;
    DMX_WRITE_REG(DMX_PID_CTRL(ChanId), dmx_pidctrl.u32);
}

/***********************************************************************************
* Function      : DmxHalSetChannelDsc
* Description   : Enable Channel Dsc
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalSetChannelDsc(HI_U32 ChanId, HI_BOOL Enable)
{
    U_DMX_PID_CTRL dmx_pidctrl;

    dmx_pidctrl.u32 = DMX_READ_REG(DMX_PID_CTRL(ChanId));
    dmx_pidctrl.bits.do_scram = Enable;
    DMX_WRITE_REG(DMX_PID_CTRL(ChanId), dmx_pidctrl.u32);
}

HI_U32 DmxHalGetOptCAType(HI_VOID)
{
    return DMX_READ_REG(CA_INFO0);
}

#ifdef DMX_DESCRAMBLER_VERSION_0

HI_U32 DmxHalGetOptDescramblerType(HI_VOID)
{
    return DMX_READ_REG(DMX_CA_DBG_0);
}

/***********************************************************************************
* Function      : DmxHalSetCAType
* Description   :
* Input         :
* Output        :
* Return        : 1: Opt set Hard ca; 0: Soft ca
* Others        :
***********************************************************************************/
HI_VOID DmxHalSetCAType(HI_U32 KeyId, HI_BOOL bAdvance)
{
    HI_U32 value;

    value = DMX_READ_REG(CW_SEL);
    if (bAdvance)
    {
        value |= (1 << KeyId);
    }
    else
    {
        value &= ~(1 << KeyId);
    }

    DMX_WRITE_REG(CW_SEL, value);
}

/***********************************************************************************
* Function      : DmxHalSetDescramblerType
* Description   :
* Input         :
* Output        :
* Return        : 1: Opt set 3.0 desc, 0: 2.0
* Others:
***********************************************************************************/
HI_VOID DmxHalSetDescramblerType(HI_U32 KeyId, HI_BOOL bHigh)
{
    HI_U32 value;

    value = DMX_READ_REG(CW_CSA3);
    if (bHigh)
    {
        value |= (1 << KeyId);
    }
    else
    {
        value &= ~(1 << KeyId);
    }

    DMX_WRITE_REG(CW_CSA3, value);
}

HI_VOID DmxHalSetCWWord(HI_U32 u32CWId, HI_U32 u32WordId, HI_U32 u32Data, HI_U32 u32EvenOdd)
{
    U_CW_SET unCwCfg;

    //firstly, config the control register, and then config cw data register, logic map
    u32CWId     = u32CWId & 0x1f;
    u32WordId   = u32WordId & 0x3;
    u32EvenOdd  = u32EvenOdd & 0x1;
    unCwCfg.u32 = DMX_READ_REG(CW_SET);
    unCwCfg.bits.cw_group_id = u32CWId;
    unCwCfg.bits.cw_word_id  = u32WordId;
    unCwCfg.bits.cw_odd_even = u32EvenOdd;
    DMX_WRITE_REG(CW_SET, unCwCfg.u32);

    DMX_WRITE_REG(DMX_CW_DATA, u32Data);
}

#endif

/***********************************************************************************
* Function      : DmxHalSetEntropyReduction
* Description   : Set the valid bits of cw
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalSetEntropyReduction(HI_U32 KeyId, HI_UNF_DMX_CA_ENTROPY_E EntropyReduction)
{
    HI_U32 Value;

    Value = DMX_READ_REG(CA_ENTROPY);
    if (HI_UNF_DMX_CA_ENTROPY_REDUCTION_CLOSE == EntropyReduction)
    {
        Value |= (1 << KeyId);
    }
    else
    {
        Value &= ~(1 << KeyId);
    }

    DMX_WRITE_REG(CA_ENTROPY, Value);
}

#ifdef DMX_DESCRAMBLER_VERSION_1
/***********************************************************************************
* Function      : DmxHalInitSpeCWOrder
* Description   : set ca type spe cw order as byte and word change
* Input         :
* Output        :
* Return        :
* Others        :
***********************************************************************************/
HI_VOID DmxHalInitSpeCWOrder(HI_VOID)
{
    HI_U32 Value;

    Value = DMX_READ_REG(DMX_CW_CTRL0);

    Value |= 0x33300;   //for spe type:byte and word change
    DMX_WRITE_REG(DMX_CW_CTRL0, Value);
}

/***********************************************************************************
* Function      : DmxHalSetCWWord1
* Description   : Set CW Word
* Input         :
* Output        :
* Return        :
* Others        :
***********************************************************************************/
HI_VOID DmxHalSetCWWord1(
        HI_U32                          GroupId,
        HI_U32                          WordId,
        HI_U32                          Key,
        DMX_KEY_TYPE_E                  KeyType,
        DMX_CW_TYPE                     CWType,
        HI_UNF_DMX_DESCRAMBLER_TYPE_E   DescType
    )
{
    U_CW_SET1   reg;
    HI_U32      type;

    switch (DescType)
    {
        case HI_UNF_DMX_DESCRAMBLER_TYPE_CSA3 :
            type = 0x10;
            break;

        case HI_UNF_DMX_DESCRAMBLER_TYPE_AES_IPTV :
            type = 0x20;
            break;

        case HI_UNF_DMX_DESCRAMBLER_TYPE_AES_ECB :
            type = 0x21;
            break;

        case HI_UNF_DMX_DESCRAMBLER_TYPE_AES_CI :
            type = 0x22;
            break;

        case HI_UNF_DMX_DESCRAMBLER_TYPE_DES_IPTV :
            type = 0x30;
            break;

        case HI_UNF_DMX_DESCRAMBLER_TYPE_DES_CI :
            type = 0x32;
            break;

        case HI_UNF_DMX_DESCRAMBLER_TYPE_DES_CBC :
            type = 0x33;
            break;

        case HI_UNF_DMX_DESCRAMBLER_TYPE_AES_NS :
            type = 0x40;
            break;

        case HI_UNF_DMX_DESCRAMBLER_TYPE_SMS4_NS :
            type = 0x41;
            break;

        case HI_UNF_DMX_DESCRAMBLER_TYPE_SMS4_IPTV :
            type = 0x50;
            break;

        case HI_UNF_DMX_DESCRAMBLER_TYPE_SMS4_ECB :
            type = 0x51;
            break;

        case HI_UNF_DMX_DESCRAMBLER_TYPE_SMS4_CBC :
            type = 0x53;
            break;

        case HI_UNF_DMX_DESCRAMBLER_TYPE_AES_CBC :
            type = 0x63;
            break;

        case HI_UNF_DMX_DESCRAMBLER_TYPE_TDES_IPTV :
            type = 0x70;
            break;

        case HI_UNF_DMX_DESCRAMBLER_TYPE_TDES_ECB :
            type = 0x71;
            break;

        case HI_UNF_DMX_DESCRAMBLER_TYPE_TDES_CBC :
            type = 0x73;
            break;

        case HI_UNF_DMX_DESCRAMBLER_TYPE_CSA2 :
        default :
            type = 0;
    }

    //firstly, config the control register, and then config cw data register, logic map

    reg.value = DMX_READ_REG(CW_SET);

    reg.bits.cw_word_id     = WordId;
    reg.bits.cw_odd_even    = (DMX_KEY_TYPE_ODD == KeyType) ? 1 : 0;
    reg.bits.cw_group_id    = GroupId;
    reg.bits.cw_type        = type;
    reg.bits.cw_iv_sel      = (DMX_KEY_IV == CWType) ? 1 : 0;

    DMX_WRITE_REG(CW_SET, reg.value);

    DMX_WRITE_REG(DMX_CW_DATA, Key);
}

/***********************************************************************************
* Function      :  DmxHalSetChanCwTabId
* Description   :
* Input         :
* Output        :
* Return        :
* Others        :
***********************************************************************************/
HI_VOID DmxHalSetChanCwTabId(HI_U32 ChanId, HI_U32 TabId)
{
    HI_U32 value;
    HI_U32 offset = ChanId & 0xF;

    value = DMX_READ_REG(CHAN_CW_TAB_ID(ChanId));

    value &= ~(0x3 << (offset * 2));
    value |= TabId << (offset * 2);

    DMX_WRITE_REG(CHAN_CW_TAB_ID(ChanId), value);
}

/***********************************************************************************
* Function      : DmxHalSetDmxIvEnable
* Description   :
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalSetDmxIvEnable(HI_U32 DmxId, HI_BOOL Enable)
{
    U_DMX_CW_SET stDmxCWSet;

    stDmxCWSet.u32 = DMX_READ_REG(DMX_CW_SET(DmxId));
    if (Enable != 0)
    {
        stDmxCWSet.bits.cw_iv_en = 1;
    }
    else
    {
        stDmxCWSet.bits.cw_iv_en = 0;
    }
    DMX_WRITE_REG(DMX_CW_SET(DmxId), stDmxCWSet.u32);
}

/***********************************************************************************
* Function      :  DmxHalSetCSA3Reset
* Description   :
* Input         :  Enable 1 : set pvr csa3.0 soft reset
*                         0 : clear pvr csa3.0 soft reset
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalSetCSA3Reset(HI_BOOL Enable)
{
    U_PVR_CSA3_RST reg;

    reg.value = DMX_READ_REG(PVR_CSA3_RST);

    reg.bits.pvr_csa3_soft_rst = Enable;
    DMX_WRITE_REG(PVR_CSA3_RST, reg.value);
}
#endif

