/***********************************************************************************
*              Copyright 2004 - 2050, Hisilicon Tech. Co., Ltd.
*                           ALL RIGHTS RESERVED
* FileName   :  hal_demux.c
* Description:  Define interfaces of demux hardware abstract layer.
*
* History:
* Version      Date         Author        DefectNum    Description
* main\1    20090927    y00106256      NULL      Create this file.
***********************************************************************************/

#include "hi_type.h"
#include "hi_module.h"
#include "hi_reg_common.h"

#include "demux_debug.h"
#include "drv_demux_reg.h"
#include "drv_demux_define.h"
#include "drv_demux_osal.h"
#include "hal_demux.h"

//#include "basedef.h"
#include "hi_drv_struct.h"
#include "hi_drv_sys.h"
#include <asm/barrier.h>    /*wmb() */ //we added this for DTS2013091204261 
#if 1
#define DMX_COM_EQUAL(exp, act)
#else
#define DMX_COM_EQUAL(exp, act)                                                         \
    do                                                                                  \
    {                                                                                   \
        if (exp != act)                                                                 \
        {                                                                               \
            HI_ERR_DEMUX("Write register error, exp=0x%x, act=0x%x\n", exp, act);       \
        }                                                                               \
    } while (0)
#endif
/*demux �ж��ڼĴ����������ھ�����ܻ�����Ī������Ĵ�����Ҫȫ��review ���мĴ�����ͳһ�޸�
������������������ʱ��������ʱ�ȴ������������������������Ĵ��������ⵥ��: DTS2013082001104 */
struct semaphore      HALLockOQ;
struct semaphore      HALLockFQ;
HI_BOOL bSemphoreInit = HI_FALSE;

static HI_VOID DmxHalSemaphoreInit(HI_VOID)
{
    HI_INIT_MUTEX(&HALLockOQ);
    HI_INIT_MUTEX(&HALLockFQ);
}

/***********************************************************************************
* Function      : DmxHalDvbPortSetAttr
* Description   : Set Tuner Port
* Input         : PortId
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalDvbPortSetAttr(
        HI_U32                  PortId,
        HI_UNF_DMX_PORT_TYPE_E  PortType,
        HI_U32                  SyncOn,
        HI_U32                  SyncOff,
        HI_U32                  TunerInClk,
        HI_U32                  BitSelector
    )
{
    U_TS_INTERFACE  ts_interface;

    ts_interface.u32 = DMX_READ_REG(TS_INTERFACE(PortId));

    ts_interface.bits.port_sel      = 0;
    ts_interface.bits.sync_clear    = 1;
    DMX_WRITE_REG(TS_INTERFACE(PortId), ts_interface.u32);

    DMX_COM_EQUAL(ts_interface.u32, DMX_READ_REG(TS_INTERFACE(PortId)));

    msleep(2);

    switch (PortType)
    {
        case HI_UNF_DMX_PORT_TYPE_PARALLEL_BURST :
        {
            ts_interface.bits.serial_sel    = 0;
            ts_interface.bits.sync_mode     = 0;

            break;
        }

        case HI_UNF_DMX_PORT_TYPE_PARALLEL_NOSYNC_188 :
        {
            ts_interface.bits.serial_sel        = 0;
            ts_interface.bits.sync_mode         = 2;
            ts_interface.bits.nosync_fixed_204  = 0;

            break;
        }

        case HI_UNF_DMX_PORT_TYPE_PARALLEL_NOSYNC_204 :
        {
            ts_interface.bits.serial_sel        = 0;
            ts_interface.bits.sync_mode         = 2;
            ts_interface.bits.nosync_fixed_204  = 1;

            break;
        }

        case HI_UNF_DMX_PORT_TYPE_PARALLEL_NOSYNC_188_204 :
        {
            ts_interface.bits.serial_sel    = 0;
            ts_interface.bits.sync_mode     = 3;

            break;
        }

        case HI_UNF_DMX_PORT_TYPE_SERIAL :
        {
            ts_interface.bits.serial_sel    = 1;
            ts_interface.bits.sync_mode     = 0;
            ts_interface.bits.ser_2bit_mode = 0;
            ts_interface.bits.ser_2bit_rev  = 0;
            ts_interface.bits.ser_nosync    = 0;

            break;
        }

        case HI_UNF_DMX_PORT_TYPE_SERIAL2BIT :
        {
            ts_interface.bits.serial_sel    = 1;
            ts_interface.bits.sync_mode     = 0;
            ts_interface.bits.ser_2bit_mode = 1;
            ts_interface.bits.ser_2bit_rev  = 0;
            ts_interface.bits.ser_nosync    = 0;

            break;
        }

        case HI_UNF_DMX_PORT_TYPE_SERIAL_NOSYNC :
        {
            ts_interface.bits.serial_sel    = 1;
            ts_interface.bits.sync_mode     = 0;
            ts_interface.bits.ser_2bit_mode = 0;
            ts_interface.bits.ser_2bit_rev  = 0;
            ts_interface.bits.ser_nosync    = 1;

            break;
        }

        case HI_UNF_DMX_PORT_TYPE_SERIAL2BIT_NOSYNC :
        {
            ts_interface.bits.serial_sel    = 1;
            ts_interface.bits.sync_mode     = 0;
            ts_interface.bits.ser_2bit_mode = 1;
            ts_interface.bits.ser_2bit_rev  = 0;
            ts_interface.bits.ser_nosync    = 1;

            break;
        }

        case HI_UNF_DMX_PORT_TYPE_PARALLEL_VALID :
        default :
        {
            ts_interface.bits.serial_sel    = 0;
            ts_interface.bits.sync_mode     = 1;
        }
    }

    ts_interface.bits.bit_sel = BitSelector;

    ts_interface.bits.syncon_th     = SyncOn;
    ts_interface.bits.syncoff_th    = SyncOff;
    ts_interface.bits.sync_clear    = 0;

    DMX_WRITE_REG(TS_INTERFACE(PortId), ts_interface.u32);

    DMX_COM_EQUAL(ts_interface.u32, DMX_READ_REG(TS_INTERFACE(PortId)));

    ts_interface.bits.port_sel = 1;
    DMX_WRITE_REG(TS_INTERFACE(PortId), ts_interface.u32);

    DMX_COM_EQUAL(ts_interface.u32, DMX_READ_REG(TS_INTERFACE(PortId)));
}

#if defined(CHIP_TYPE_hi3716h) || defined(CHIP_TYPE_hi3716c) || defined(CHIP_TYPE_hi3716m) || defined(CHIP_TYPE_hi3712)
HI_VOID DmxHalDvbPortSetClkInPol(HI_U32 PortId, HI_BOOL Pol)
{
    U_PERI_CRG25 reg;

    reg.value = SYS_READ_REG(PERI_CRG25);

    if (PortId == 1)
    {
        reg.bits.tsi0_inclk_pctrl = Pol;
    }
    else
    {
        reg.bits.tsi1_inclk_pctrl = Pol;
    }

    SYS_WRITE_REG(PERI_CRG25, reg.value);
}
#elif defined (CHIP_TYPE_hi3716cv200es)    || defined (CHIP_TYPE_hi3716cv200) \
	|| defined (CHIP_TYPE_hi3719cv100) || defined (CHIP_TYPE_hi3718cv100)  \
	|| defined (CHIP_TYPE_hi3719mv100) || defined (CHIP_TYPE_hi3719mv100_a)\
	|| defined (CHIP_TYPE_hi3718mv100) 
HI_VOID DmxHalDvbPortSetClkInPol(HI_U32 PortId, HI_BOOL Pol)
{
    U_PERI_CRG63 PeriCrg63;

    PeriCrg63.u32 = g_pstRegCrg->PERI_CRG63.u32;

    switch (PortId)
    {
        case 1 :
            PeriCrg63.bits.pvr_tsi2_pctrl = Pol;    /*pvr_tsi2_pctrl acturally is point to tsi0*/
            break;

        case 2 :
            PeriCrg63.bits.pvr_tsi3_pctrl = Pol;
            break;
#if defined(CHIP_TYPE_hi3716cv200es) || defined(CHIP_TYPE_hi3716cv200) 
        case 3 :
            PeriCrg63.bits.pvr_tsi4_pctrl = Pol;
            break;

        case 4 :
            PeriCrg63.bits.pvr_tsi5_pctrl = Pol;
            break;
#endif
#if defined(CHIP_TYPE_hi3716cv200es)
        case 5 :
            PeriCrg63.bits.pvr_tsi6_pctrl = Pol;
            break;

        case 6 :
            PeriCrg63.bits.pvr_tsi7_pctrl = Pol;
            break;
#endif
        default :
            return;
    }

    g_pstRegCrg->PERI_CRG63.u32 = PeriCrg63.u32;
}
#elif defined(CHIP_TYPE_hi3798cv100)
HI_VOID DmxHalDvbPortSetClkInPol(HI_U32 PortId, HI_BOOL Pol)
{
}
#endif

/***********************************************************************************
* Function      : DmxHalDvbPortSetTsCountCtrl
* Description   : Set TS Count Ctrl
* Input         : PortId, option
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalDvbPortSetTsCountCtrl(const HI_U32 PortId, const HI_U32 option)
{
    U_TS_COUNT_CTRL ts_count_ctrl;

    ts_count_ctrl.u32 = DMX_READ_REG(TS_COUNT_CTRL(PortId));

    ts_count_ctrl.bits.ts_count_ctrl = option;
    DMX_WRITE_REG(TS_COUNT_CTRL(PortId), ts_count_ctrl.u32);

    DMX_COM_EQUAL(ts_count_ctrl.u32, DMX_READ_REG(TS_COUNT_CTRL(PortId)));
}

/***********************************************************************************
* Function      : DmxHalDvbPortGetTsPackCount
* Description   : Get TS Pack Counter
* Input         : PortId
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_U32 DmxHalDvbPortGetTsPackCount(HI_U32 PortId)
{
    return DMX_READ_REG(TS_COUNT(PortId));
}

/***********************************************************************************
* Function      : DmxHalDvbPortSetErrTsCountCtrl
* Description   : Set ETS Count Ctrl
* Input         : PortId, option
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalDvbPortSetErrTsCountCtrl(const HI_U32 PortId, const HI_U32 option)
{
    U_ETS_COUNT_CTRL ets_count_ctrl;

    ets_count_ctrl.u32 = DMX_READ_REG(ETS_COUNT_CTRL(PortId));

    ets_count_ctrl.bits.ets_count_ctrl = option;
    DMX_WRITE_REG(ETS_COUNT_CTRL(PortId), ets_count_ctrl.u32);

    DMX_COM_EQUAL(ets_count_ctrl.u32, DMX_READ_REG(ETS_COUNT_CTRL(PortId)));
}

/***********************************************************************************
* Function      : DmxHalDvbPortGetErrTsPackCount
* Description   : Get Error TS Pack Count
* Input         : PortId
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_U32 DmxHalDvbPortGetErrTsPackCount(HI_U32 PortId)
{
    return DMX_READ_REG(ETS_COUNT(PortId));
}

HI_VOID DmxHalIPPortSetAttr(HI_U32 PortId, HI_UNF_DMX_PORT_TYPE_E PortType, HI_U32 SyncOn, HI_U32 SyncOff)
{
    U_IP_SYNC_TH_CFG ipsync_cfg;

    ipsync_cfg.u32 = DMX_READ_REG(IP_SYNC_TH_CFG(PortId));

    switch (PortType)
    {
        case HI_UNF_DMX_PORT_TYPE_PARALLEL_NOSYNC_188 :
            ipsync_cfg.bits.ip_sync_type = 0;
            break;

        case HI_UNF_DMX_PORT_TYPE_PARALLEL_NOSYNC_204 :
            ipsync_cfg.bits.ip_sync_type = 1;
            break;

        case HI_UNF_DMX_PORT_TYPE_PARALLEL_NOSYNC_188_204 :
        case HI_UNF_DMX_PORT_TYPE_USER_DEFINED :
            ipsync_cfg.bits.ip_sync_type = 2;
            break;

    #ifdef DMX_RAM_PORT_AUTO_SCAN_SUPPORT
        case HI_UNF_DMX_PORT_TYPE_AUTO :
            ipsync_cfg.bits.ip_sync_type = 3;
            break;
    #endif

        default :
            ipsync_cfg.bits.ip_sync_type = 2;
    }

    ipsync_cfg.bits.ip_sync_th = SyncOn;
    ipsync_cfg.bits.ip_loss_th = SyncOff;

    DMX_WRITE_REG(IP_SYNC_TH_CFG(PortId), ipsync_cfg.u32);

    DMX_COM_EQUAL(ipsync_cfg.u32, DMX_READ_REG(IP_SYNC_TH_CFG(PortId)));
}

HI_VOID DmxHalIPPortSetSyncLen(HI_U32 PortId, HI_U32 SyncLen1, HI_U32 SyncLen2)
{
    U_DMX_SYNC_LEN_SET SyncSet;

    SyncSet.value = DMX_READ_REG(IP_SYNC_LEN(PortId));
    SyncSet.bits.ip_nosync_len1 = SyncLen1;
    SyncSet.bits.ip_nosync_len2 = SyncLen2;

    DMX_WRITE_REG(IP_SYNC_LEN(PortId), SyncSet.value);

    DMX_COM_EQUAL(SyncSet.value, DMX_READ_REG(IP_SYNC_LEN(PortId)));
}

#ifdef DMX_RAM_PORT_AUTO_SCAN_SUPPORT
HI_VOID DmxHalIPPortSetAutoScanRegion(HI_U32 PortId, HI_U32 len, HI_U32 step)
{
    U_DMX_SYNC_LEN_SET SyncSet;

    SyncSet.value = DMX_READ_REG(IP_SYNC_LEN(PortId));
    SyncSet.bits.ip_nosync_region   = len;
    SyncSet.bits.ip_nosync_step     = step;

    DMX_WRITE_REG(IP_SYNC_LEN(PortId), SyncSet.value);

    DMX_COM_EQUAL(SyncSet.value, DMX_READ_REG(IP_SYNC_LEN(PortId)));
}
#endif

/***********************************************************************************
* Function      : DmxHalIPPortSetTsCountCtrl
* Description   : Set TS Pack Count
* Input         : PortId
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalIPPortSetTsCountCtrl(const HI_U32 PortId, const HI_BOOL enable)
{
    U_IP_DBG_CNT_EN ip_count_en;

    ip_count_en.u32 = DMX_READ_REG(IP_DBG_CNT_EN(PortId));

    ip_count_en.bits.ip_dbg_cnt_en = enable;
    DMX_WRITE_REG(IP_DBG_CNT_EN(PortId), ip_count_en.u32);

    DMX_COM_EQUAL(ip_count_en.u32, DMX_READ_REG(IP_DBG_CNT_EN(PortId)));
}

/***********************************************************************************
* Function      : DmxHalIPPortGetTsPackCount
* Description   : Get TS Pack Counter Status
* Input         : PortId
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_U32 DmxHalIPPortGetTsPackCount(HI_U32 PortId)
{
    return DMX_READ_REG(IP_DBG_OUT1(PortId));
}

/***********************************************************************************
* Function      : DmxHalIPPortStartStream
* Description   : Start pushing stream
* Input         : PortId
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalIPPortStartStream(const HI_U32 PortId, const HI_BOOL Enable)
{
    U_IP_TS_OUT_STOP ipout_en;

    ipout_en.u32 = DMX_READ_REG(IP_TS_OUT_STOP(PortId));

    ipout_en.bits.ip_stop_en = Enable ? 0 : 1;
    DMX_WRITE_REG(IP_TS_OUT_STOP(PortId), ipout_en.u32);

    DMX_COM_EQUAL(ipout_en.u32, DMX_READ_REG(IP_TS_OUT_STOP(PortId)));
}

/***********************************************************************************
* Function      : DmxHalIPPortDescSet
* Description   : Set IP descriptors
* Input         : PortId
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalIPPortDescSet(HI_U32 PortId, HI_U32 StartAddr, HI_U32 Depth)
{
    U_IP_DESC_SIZE ipsize;

    ipsize.u32 = DMX_READ_REG(IP_DESC_SIZE(PortId));

    ipsize.bits.ip_desc_size = Depth;
    DMX_WRITE_REG(IP_DESC_SIZE(PortId), ipsize.u32);

    DMX_COM_EQUAL(ipsize.u32, DMX_READ_REG(IP_DESC_SIZE(PortId)));

    DMX_WRITE_REG(IP_DESC_SADDR(PortId), StartAddr);

    DMX_COM_EQUAL(StartAddr, DMX_READ_REG(IP_DESC_SADDR(PortId)));

    DMX_WRITE_REG(IP_TS_RATE_CFG(PortId), DMX_DEFAULT_IP_SPEED);

    DMX_COM_EQUAL(DMX_DEFAULT_IP_SPEED, DMX_READ_REG(IP_TS_RATE_CFG(PortId)));
}

/***********************************************************************************
* Function      : DmxHalIPPortDescAdd
* Description   : add descriptors
* Input         : PortId
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalIPPortDescAdd(const HI_U32 PortId, const HI_U32 DescNum)
{
    U_IP_DESC_ADD DescAdd;

    wmb();/*sync the DDR*/
    DescAdd.u32 = DMX_READ_REG(IP_DESC_ADD(PortId));

    DescAdd.bits.ip_desc_add = DescNum;
    DMX_WRITE_REG(IP_DESC_ADD(PortId), DescAdd.u32);

    DMX_COM_EQUAL(DescAdd.u32, DMX_READ_REG(IP_DESC_ADD(PortId)));
}

/***********************************************************************************
* Function      : DmxHalIPPortSetIntCnt
* Description   : Set ip out int level
* Input         : PortId, PackeNum
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalIPPortSetIntCnt(HI_U32 PortId, HI_U32 DescNum)
{
    U_IP_INT_CNT_CFG unIpIntCfg;

    unIpIntCfg.u32 = DMX_READ_REG(IP_INT_CNT_CFG(PortId));

    unIpIntCfg.bits.ip_int_cfg = DescNum;
    DMX_WRITE_REG(IP_INT_CNT_CFG(PortId), unIpIntCfg.u32);

    DMX_COM_EQUAL((unIpIntCfg.u32 & 0xf), (DMX_READ_REG(IP_INT_CNT_CFG(PortId)) & 0xf));
}

/***********************************************************************************
* Function      : DmxHalIPPortGetOutIntStatus
* Description   : Get desc_out int status
* Input         : PortId
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_U32 DmxHalIPPortGetOutIntStatus(HI_U32 PortId)
{
    U_IP_IINT_INT ip_int;

    ip_int.u32 = DMX_READ_REG(IP_IINT_INT(PortId));

    return ip_int.bits.ip_iint_desc_out;
}

/***********************************************************************************
* Function      : DmxHalIPPortClearOutIntStatus
* Description   : clear desc_out int status
* Input         : PortId
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalIPPortClearOutIntStatus(HI_U32 PortId)
{
    U_IP_IRAW_INT ip_raw_int;

    ip_raw_int.u32 = DMX_READ_REG(IP_IRAW_INT(PortId));

    ip_raw_int.bits.ip_iraw_desc_out = 1;
    DMX_WRITE_REG(IP_IRAW_INT(PortId), ip_raw_int.u32);
}

/***********************************************************************************
* Function      : DmxHalIPPortSetOutInt
* Description   : Set desc_out int enale
* Input         : PortId
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalIPPortSetOutInt(const HI_U32 PortId, const HI_BOOL Enable)
{
    U_IP_IENA_INT ip_int_ena;

    ip_int_ena.u32 = DMX_READ_REG(IP_IENA_INT(PortId));

    ip_int_ena.bits.ip_iena_desc_out = Enable;
    DMX_WRITE_REG(IP_IENA_INT(PortId), ip_int_ena.u32);

    DMX_COM_EQUAL(ip_int_ena.u32, DMX_READ_REG(IP_IENA_INT(PortId)));
}

/***********************************************************************************
* Function      : DmxHalIPPortDescGetRead
* Description   : Get desc read offset
* Input         : PortId
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_U32 DmxHalIPPortDescGetRead(HI_U32 PortId)
{
    U_IP_DESC_PTR ip_ptr;

    ip_ptr.u32 = DMX_READ_REG(IP_DESC_PTR(PortId));

    return ip_ptr.bits.ip_desc_rptr;
}

/***********************************************************************************
* Function      : DmxHalIPPortEnableInt
* Description   : Set IP int enable
* Input         : PortId
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalIPPortEnableInt(HI_U32 PortId)
{
    U_ENA_INT_TYPE  unEnaErr;
    U_IP_IENA_INT   unEnaIp;

    unEnaErr.u32 = DMX_READ_REG(ENA_INT_TYPE);
    unEnaIp.u32 = DMX_READ_REG(IP_IENA_INT(PortId));

    switch (PortId)
    {
        case 0:
            unEnaErr.bits.iena_ip0_all = 1;
            break;

        case 1:
            unEnaErr.bits.iena_ip1_all = 1;
            break;

        case 2:
            unEnaErr.bits.iena_ip2_all = 1;
            break;

        case 3:
            unEnaErr.bits.iena_ip3_all = 1;
            break;

    #if defined (CHIP_TYPE_hi3716cv200es)    || defined (CHIP_TYPE_hi3716cv200) 
        case 4:
            unEnaErr.bits.iena_ip4_all = 1;
            break;
    #endif
    #if defined (CHIP_TYPE_hi3719cv100) || defined (CHIP_TYPE_hi3718cv100)  \
	|| defined (CHIP_TYPE_hi3719mv100) || defined (CHIP_TYPE_hi3719mv100_a)\
	|| defined (CHIP_TYPE_hi3718mv100) 
        case 5:
            unEnaErr.bits.iena_ip5_all = 1;
            break;
    #endif

        default:
            return;
    }

    unEnaIp.bits.ip_iena_all = 1;
    DMX_WRITE_REG(ENA_INT_TYPE, unEnaErr.u32);
    DMX_WRITE_REG(IP_IENA_INT(PortId), unEnaIp.u32);
}

/***********************************************************************************
* Function      : DmxHalIPPortDisableInt
* Description   : Set IP int disable
* Input         : PortId
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalIPPortDisableInt(HI_U32 PortId)
{
    U_ENA_INT_TYPE unEnaErr;
    U_IP_IENA_INT unEnaIp;

    unEnaErr.u32 = DMX_READ_REG(ENA_INT_TYPE);
    unEnaIp.u32 = DMX_READ_REG(IP_IENA_INT(PortId));
    switch (PortId)
    {
    case 0:
        unEnaErr.bits.iena_ip0_all = 0;
        break;
    case 1:
        unEnaErr.bits.iena_ip1_all = 0;
        break;
    case 2:
        unEnaErr.bits.iena_ip2_all = 0;
        break;
    case 3:
        unEnaErr.bits.iena_ip3_all = 0;
        break;
    default:
        return;
    }

    unEnaIp.bits.ip_iena_all = 0;
    DMX_WRITE_REG(ENA_INT_TYPE, unEnaErr.u32);
    DMX_WRITE_REG(IP_IENA_INT(PortId), unEnaIp.u32);
}

/***********************************************************************************
* Function      : DmxHalSetChannelDataType
* Description   : Set Channel Data Type
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalSetChannelDataType(HI_U32 ChanId, DMX_CHAN_DATA_TYPE_E DataType)
{
    U_DMX_PID_CTRL dmx_pidctrl;

    dmx_pidctrl.u32 = DMX_READ_REG(DMX_PID_CTRL(ChanId));

    dmx_pidctrl.bits.data_type = DataType;
    DMX_WRITE_REG(DMX_PID_CTRL(ChanId), dmx_pidctrl.u32);

    DMX_COM_EQUAL(dmx_pidctrl.u32, DMX_READ_REG(DMX_PID_CTRL(ChanId)));
}

/***********************************************************************************
* Function      :  DmxHalSetChannelAFMode
* Description   :  Set Channel AF Mode
* Input         : Portid (= 0,1,2)
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalSetChannelAFMode(HI_U32 ChanId, DMX_Ch_AFMode_E eAfMode)
{
    U_DMX_PID_CTRL dmx_pidctrl;

    dmx_pidctrl.u32 = DMX_READ_REG(DMX_PID_CTRL(ChanId));
    dmx_pidctrl.bits.af_mode = eAfMode & 0x3;
    DMX_WRITE_REG(DMX_PID_CTRL(ChanId), dmx_pidctrl.u32);

    DMX_COM_EQUAL(dmx_pidctrl.u32, DMX_READ_REG(DMX_PID_CTRL(ChanId)));
}

/***********************************************************************************
* Function      : DmxHalSetChannelCRCMode
* Description   : Set Channel CRC Mode
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalSetChannelCRCMode(HI_U32 ChanId, HI_UNF_DMX_CHAN_CRC_MODE_E CrcMode)
{
    U_DMX_PID_CTRL dmx_pidctrl;

    dmx_pidctrl.u32 = DMX_READ_REG(DMX_PID_CTRL(ChanId));

    switch (CrcMode)
    {
        case HI_UNF_DMX_CHAN_CRC_MODE_FORCE_AND_DISCARD :
            dmx_pidctrl.bits.crc_mode = 1;
            break;

        case HI_UNF_DMX_CHAN_CRC_MODE_FORCE_AND_SEND :
            dmx_pidctrl.bits.crc_mode = 2;
            break;

        case HI_UNF_DMX_CHAN_CRC_MODE_BY_SYNTAX_AND_DISCARD :
            dmx_pidctrl.bits.crc_mode = 3;
            break;

        case HI_UNF_DMX_CHAN_CRC_MODE_BY_SYNTAX_AND_SEND :
            dmx_pidctrl.bits.crc_mode = 4;
            break;

        case HI_UNF_DMX_CHAN_CRC_MODE_FORBID :
        default :
            dmx_pidctrl.bits.crc_mode = 0;
    }

    DMX_WRITE_REG(DMX_PID_CTRL(ChanId), dmx_pidctrl.u32);

    DMX_COM_EQUAL(dmx_pidctrl.u32, DMX_READ_REG(DMX_PID_CTRL(ChanId)));
}

/***********************************************************************************
* Function      :  DmxHalSetChannelCCDiscon
* Description   :  Set Channel CC discontiune Mode
* Input         : Portid (= 0,1,2)  DiscardFlag =0:not discard the discontinuous CC TS packet;
1 discard the discontinuous CC TS packet
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalSetChannelCCDiscon(HI_U32 ChanId, HI_U32 DiscardFlag)
{
    U_DMX_PID_CTRL dmx_pidctrl;

    dmx_pidctrl.u32 = DMX_READ_REG(DMX_PID_CTRL(ChanId));
    dmx_pidctrl.bits.cc_discon_ctrl = DiscardFlag & 0x1;
    DMX_WRITE_REG(DMX_PID_CTRL(ChanId), dmx_pidctrl.u32);

    DMX_COM_EQUAL(dmx_pidctrl.u32, DMX_READ_REG(DMX_PID_CTRL(ChanId)));
}

/***********************************************************************************
* Function      :  DmxHalSetChannelPusiCtrl
* Description   :  Set Channel pusi control
* Input         : Portid (= 0,1,2)  PusiCtrl =0 : PUSI valid, check PUSI, start output stream when PUSI incoming
1: PUSI invalid, not check PUSI, output stream directly
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalSetChannelPusiCtrl(HI_U32 ChanId, HI_U32 PusiCtrl)
{
    U_DMX_PID_CTRL dmx_pidctrl;

    dmx_pidctrl.u32 = DMX_READ_REG(DMX_PID_CTRL(ChanId));
    dmx_pidctrl.bits.pusi_disable = PusiCtrl & 0x1;
    DMX_WRITE_REG(DMX_PID_CTRL(ChanId), dmx_pidctrl.u32);

    DMX_COM_EQUAL(dmx_pidctrl.u32, DMX_READ_REG(DMX_PID_CTRL(ChanId)));
}

/***********************************************************************************
* Function      :  DmxHalSetChannelCCRepeatCtrl
* Description   :  Set Channel CC Repeat Ctrl
* Input         : Portid (= 0,1,2)  CCRepeatCtrl =0:discard this packet; 1: reserve this packet
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalSetChannelCCRepeatCtrl(HI_U32 ChanId, HI_U32 CCRepeatCtrl)
{
    U_DMX_PID_CTRL dmx_pidctrl;

    dmx_pidctrl.u32 = DMX_READ_REG(DMX_PID_CTRL(ChanId));
    dmx_pidctrl.bits.cc_equ_rve = CCRepeatCtrl & 0x1;
    DMX_WRITE_REG(DMX_PID_CTRL(ChanId), dmx_pidctrl.u32);

    DMX_COM_EQUAL(dmx_pidctrl.u32, DMX_READ_REG(DMX_PID_CTRL(ChanId)));
}

/***********************************************************************************
* Function      :  DmxHalSetChannelTsPostMode
* Description   :  Set Channel Ts Post Mode
* Input         : Portid (= 0,1,2)  TSPost =0:not select TS_POST mode; 1: select TS_POST mode
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalSetChannelTsPostMode(HI_U32 ChanId, HI_U32 TsPost)
{
    U_DMX_PID_CTRL dmx_pidctrl;

    dmx_pidctrl.u32 = DMX_READ_REG(DMX_PID_CTRL(ChanId));
    dmx_pidctrl.bits.ts_post_mode = TsPost & 0x1;
    DMX_WRITE_REG(DMX_PID_CTRL(ChanId), dmx_pidctrl.u32);

    DMX_COM_EQUAL(dmx_pidctrl.u32, DMX_READ_REG(DMX_PID_CTRL(ChanId)));
}

/***********************************************************************************
* Function      :  DmxHalSetChannelTsPostThresh
* Description   :  Set Channel Ts Post Thresh
* Input         : Portid (= 0,1,2) Threshold
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalSetChannelTsPostThresh(HI_U32 ChanId, HI_U32 Threshold)
{
    U_DMX_PID_CTRL dmx_pidctrl;

    dmx_pidctrl.u32 = DMX_READ_REG(DMX_PID_CTRL(ChanId));
    dmx_pidctrl.bits.ts_post_threshold = Threshold & 0x3f;
    DMX_WRITE_REG(DMX_PID_CTRL(ChanId), dmx_pidctrl.u32);

    DMX_COM_EQUAL(dmx_pidctrl.u32, DMX_READ_REG(DMX_PID_CTRL(ChanId)));
}

/***********************************************************************************
* Function      :  DmxHalSetChannelAttr
* Description   :  Set Channel attr
* Input         : Portid (= 0,1,2) Threshold
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalSetChannelAttr(HI_U32 ChanId, DMX_Ch_ATTR_E echattr)
{
    U_DMX_PID_CTRL dmx_pidctrl;

    dmx_pidctrl.u32 = DMX_READ_REG(DMX_PID_CTRL(ChanId));
    if ((echattr == DMX_CH_AUDIO) || (echattr == DMX_CH_VIDEO))
    {
        dmx_pidctrl.bits.pusi_disable = 1;
    }
    else
    {
        dmx_pidctrl.bits.pusi_disable = 0;
    }

    dmx_pidctrl.bits.ch_attri = echattr & 0x7;
    DMX_WRITE_REG(DMX_PID_CTRL(ChanId), dmx_pidctrl.u32);

    DMX_COM_EQUAL(dmx_pidctrl.u32, DMX_READ_REG(DMX_PID_CTRL(ChanId)));
}

/***********************************************************************************
* Function      :  DmxHalSetChannelFltMode
* Description   :  Set Channel Filter hit Mode
* Input         : u32Chid  bEnable
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalSetChannelFltMode(HI_U32 u32Chid, HI_BOOL bEnable)
{
    U_DMX_PID_REC_BUF unPidRecTrl;

    unPidRecTrl.u32 = DMX_READ_REG(DMX_PID_REC_BUF(u32Chid));
    unPidRecTrl.bits.flt_hit_mode = bEnable;
    DMX_WRITE_REG(DMX_PID_REC_BUF(u32Chid), unPidRecTrl.u32);

    DMX_COM_EQUAL(unPidRecTrl.u32, DMX_READ_REG(DMX_PID_REC_BUF(u32Chid)));
}

/***********************************************************************************
* Function      :  DmxHalGetChannelPlayDmxid
* Description   :   Get Channel PlayDmxid
* Input         : chid (= 0,1,2)
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalGetChannelPlayDmxid(HI_U32 ChanId, HI_U32 *dmxid)
{
    U_DMX_PID_EN dmx_pid_en;

    dmx_pid_en.u32 = DMX_READ_REG(DMX_PID_EN(ChanId));
    *dmxid = (dmx_pid_en.bits.pid_play_dmx_id & 0x7) - 1;
}

/***********************************************************************************
* Function      :  DmxHalSetChannelPlayDmxid
* Description   :  Set Channel PlayDmxid
* Input         : Portid (= 0,1,2) Threshold
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalSetChannelPlayDmxid(HI_U32 ChanId, HI_U32 dmxid)
{
    U_DMX_PID_EN dmx_pid_en;

    dmx_pid_en.u32 = DMX_READ_REG(DMX_PID_EN(ChanId));
    dmx_pid_en.bits.pid_play_dmx_id = (dmxid + 1) & 0x7;
    DMX_WRITE_REG(DMX_PID_EN(ChanId), dmx_pid_en.u32);

    DMX_COM_EQUAL(dmx_pid_en.u32, DMX_READ_REG(DMX_PID_EN(ChanId)));
}

/***********************************************************************************
* Function      :  DmxHalSetChannelRecDmxid
* Description   :  Set Channel record Dmxid
* Input         : Portid (= 0,1,2) Threshold
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalSetChannelRecDmxid(HI_U32 ChanId, HI_U32 dmxid)
{
    U_DMX_PID_EN dmx_pid_en;

    dmx_pid_en.u32 = DMX_READ_REG(DMX_PID_EN(ChanId));
    dmx_pid_en.bits.pid_rec_dmx_id = (dmxid + 1) & 0x7;
    DMX_WRITE_REG(DMX_PID_EN(ChanId), dmx_pid_en.u32);

    DMX_COM_EQUAL(dmx_pid_en.u32, DMX_READ_REG(DMX_PID_EN(ChanId)));
}

/***********************************************************************************
* Function      : DmxHalSetChannelPid
* Description   : set Channel pid
* Input         : ChanId, pid
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalSetChannelPid(HI_U32 ChanId, HI_U32 pid)
{
    U_DMX_PID_VALUE dmx_pid;

    dmx_pid.u32 = DMX_READ_REG(DMX_PID_VALUE(ChanId));
    dmx_pid.bits.pid_value = pid & 0x1fff;
    DMX_WRITE_REG(DMX_PID_VALUE(ChanId), dmx_pid.u32);

    DMX_COM_EQUAL(dmx_pid.u32, DMX_READ_REG(DMX_PID_VALUE(ChanId)));
}

/***********************************************************************************
* Function      : DmxHalSetChannelRecBufId
* Description   : Set Channel Rec BufId
* Input         : ChanId
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalSetChannelRecBufId(HI_U32 ChanId, HI_U32 OQId)
{
    U_DMX_PID_REC_BUF pid_rec_buf;

    pid_rec_buf.u32 = DMX_READ_REG(DMX_PID_REC_BUF(ChanId));

    pid_rec_buf.bits.rec_buf = OQId;
    DMX_WRITE_REG(DMX_PID_REC_BUF(ChanId), pid_rec_buf.u32);

    DMX_COM_EQUAL(pid_rec_buf.u32, DMX_READ_REG(DMX_PID_REC_BUF(ChanId)));
}

/***********************************************************************************
* Function      : DmxHalSetChannelPlayBufId
* Description   : Set Channel Play BufId
* Input         : ChanId
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalSetChannelPlayBufId(HI_U32 ChanId, HI_U32 OQId)
{
    U_DMX_PID_PLAY_BUF pid_play_buf;

    pid_play_buf.u32 = DMX_READ_REG(DMX_PID_PLAY_BUF(ChanId));

    pid_play_buf.bits.play_buf = OQId;
    DMX_WRITE_REG(DMX_PID_PLAY_BUF(ChanId), pid_play_buf.u32);

    DMX_COM_EQUAL(pid_play_buf.u32, DMX_READ_REG(DMX_PID_PLAY_BUF(ChanId)));
}

/***********************************************************************************
* Function      : DmxHalSetDemuxPortId
* Description   : Set Dmx PortId
* Input         : DmxId, PortMode, PortId
* Output        :
* Return        :
* Others:
***********************************************************************************/

#if defined (CHIP_TYPE_hi3716cv200es)    || defined (CHIP_TYPE_hi3716cv200) \
	|| defined (CHIP_TYPE_hi3719cv100) || defined (CHIP_TYPE_hi3718cv100)  \
	|| defined (CHIP_TYPE_hi3719mv100) || defined (CHIP_TYPE_hi3719mv100_a)\
	|| defined (CHIP_TYPE_hi3718mv100) 
HI_VOID DmxHalSetDemuxPortId(HI_U32 DmxId, DMX_PORT_MODE_E PortMode, HI_U32 PortId)
{
    HI_U32          Id = 0;

    if (DMX_PORT_MODE_TUNER == PortMode)
    {
        Id = PortId + DMX_PORT_OFFSET;
    }
    else if (DMX_PORT_MODE_RAM == PortMode)
    {
        Id = DMX_RAM_PORT_OFFSET + PortId + DMX_PORT_OFFSET;
    }

    if (DmxId < 4)
    {
        U_SWITCH_CFG0   SwitchCfg0;

        SwitchCfg0.u32 = DMX_READ_REG(SWITCH_CFG0);

        switch (DmxId)
        {
            case 0:
                SwitchCfg0.bits.switch_cfg1 = Id;
                break;

            case 1:
                SwitchCfg0.bits.switch_cfg2 = Id;
                break;

            case 2:
                SwitchCfg0.bits.switch_cfg3 = Id;
                break;

            case 3:
            default :
                SwitchCfg0.bits.switch_cfg4 = Id;
                break;
        }

        DMX_WRITE_REG(SWITCH_CFG0, SwitchCfg0.u32);
    }
    else
    {
        U_SWITCH_CFG1   SwitchCfg1;

        SwitchCfg1.u32 = DMX_READ_REG(SWITCH_CFG1);

        switch (DmxId)
        {
            case 4:
                SwitchCfg1.bits.switch_cfg5 = Id;
                break;

            case 5:
                SwitchCfg1.bits.switch_cfg6 = Id;
                break;

            case 6:
                SwitchCfg1.bits.switch_cfg7 = Id;
                break;

            default:
                break;
        }

        DMX_WRITE_REG(SWITCH_CFG1, SwitchCfg1.u32);
    }
}
#else
HI_VOID DmxHalSetDemuxPortId(HI_U32 DmxId, DMX_PORT_MODE_E PortMode, HI_U32 PortId)
{
    HI_U32          Id = 0;
    U_SWITCH_CFG    SwitchCfg;

    if (DMX_PORT_MODE_TUNER == PortMode)
    {
        Id = PortId + DMX_PORT_OFFSET;
    }
    else if (DMX_PORT_MODE_RAM == PortMode)
    {
        Id = DMX_RAM_PORT_OFFSET + PortId + DMX_PORT_OFFSET;
    }

    SwitchCfg.u32 = DMX_READ_REG(SWITCH_CFG);

    switch (DmxId)
    {
        case 0:
            SwitchCfg.bits.switch_cfg1 = Id;
            break;

        case 1:
            SwitchCfg.bits.switch_cfg2 = Id;
            break;

        case 2:
            SwitchCfg.bits.switch_cfg3 = Id;
            break;

        case 3:
            SwitchCfg.bits.switch_cfg4 = Id;
            break;

        case 4:
            SwitchCfg.bits.switch_cfg5 = Id;
            break;

        default:
            break;
    }

    DMX_WRITE_REG(SWITCH_CFG, SwitchCfg.u32);

    DMX_COM_EQUAL(SwitchCfg.u32, DMX_READ_REG(SWITCH_CFG));
}
#endif

/***********************************************************************************
* Function      :  DmxHalSetDataFakeMod
* Description   :  Set Data Fake Mode
* Input         : bFakeEn
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalSetDataFakeMod(HI_BOOL bFakeEn)
{
#if defined (CHIP_TYPE_hi3716cv200es)    || defined (CHIP_TYPE_hi3716cv200) \
	|| defined (CHIP_TYPE_hi3719cv100) || defined (CHIP_TYPE_hi3718cv100)  \
	|| defined (CHIP_TYPE_hi3719mv100) || defined (CHIP_TYPE_hi3719mv100_a)\
	|| defined (CHIP_TYPE_hi3718mv100) 
    U_SWITCH_FAKE_EN switch_fake_en;

    switch_fake_en.u32 = DMX_READ_REG(SWITCH_FAKE_EN);
    switch_fake_en.bits.switch_fake_en = bFakeEn;
    DMX_WRITE_REG(SWITCH_FAKE_EN, switch_fake_en.u32);
#else
    U_SWITCH_CFG switch_cfg;

    switch_cfg.u32 = DMX_READ_REG(SWITCH_CFG);
    switch_cfg.bits.switch_fake_en = bFakeEn;
    DMX_WRITE_REG(SWITCH_CFG, switch_cfg.u32);
#endif
}

/***********************************************************************************
* Function      : DmxHalSetRecType
* Description   : Set Rec Type
* Input         : DmxId, RecType
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalSetRecType(HI_U32 DmxId, DMX_REC_TYPE_E RecType)
{
    U_DMX_CTRL_FUNC dmx_ctrl_func;

    DmxId += DMX_SW_HW_OFFSET;

    dmx_ctrl_func.u32 = DMX_READ_REG(DMX_CTRL_FUNC);
    switch (DmxId)
    {
        case 1:
            dmx_ctrl_func.bits.dmx1_rec_ctrl = RecType;
            break;

        case 2:
            dmx_ctrl_func.bits.dmx2_rec_ctrl = RecType;
            break;

        case 3:
            dmx_ctrl_func.bits.dmx3_rec_ctrl = RecType;
            break;

        case 4:
            dmx_ctrl_func.bits.dmx4_rec_ctrl = RecType;
            break;

        case 5:
            dmx_ctrl_func.bits.dmx5_rec_ctrl = RecType;
            break;

    #if defined (CHIP_TYPE_hi3716cv200es)    || defined (CHIP_TYPE_hi3716cv200) \
	|| defined (CHIP_TYPE_hi3719cv100) || defined (CHIP_TYPE_hi3718cv100)  \
	|| defined (CHIP_TYPE_hi3719mv100) || defined (CHIP_TYPE_hi3719mv100_a)\
	|| defined (CHIP_TYPE_hi3718mv100) 
        case 6:
            dmx_ctrl_func.bits.dmx6_rec_ctrl = RecType;
            break;

        case 7:
            dmx_ctrl_func.bits.dmx7_rec_ctrl = RecType;
            break;
    #endif

        default:
            dmx_ctrl_func.bits.dmx1_rec_ctrl = RecType;
    }

    DMX_WRITE_REG(DMX_CTRL_FUNC, dmx_ctrl_func.u32);

    DMX_COM_EQUAL(dmx_ctrl_func.u32, DMX_READ_REG(DMX_CTRL_FUNC));
}

/***********************************************************************************
* Function      : DmxHalFlushChannel
* Description   : Flush Channel
* Input         : ChanId, FlushType
* Output        :
* Return        :
* Others:       :
***********************************************************************************/
HI_VOID DmxHalFlushChannel(HI_U32 ChanId, DMX_FLUSH_TYPE_E FlushType)
{
    U_DMX_GLB_FLUSH glb_flush;

    glb_flush.u32 = DMX_READ_REG(DMX_GLB_FLUSH);

    glb_flush.bits.flush_ch     = ChanId;
    glb_flush.bits.flush_type   = FlushType;
    glb_flush.bits.flush_cmd    = 1;
    DMX_WRITE_REG(DMX_GLB_FLUSH, glb_flush.u32);
}

/***********************************************************************************
* Function      :  DmxHalIsFlushChannelDone
* Description   :  Is flush channel done
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_BOOL DmxHalIsFlushChannelDone(HI_VOID)
{
    U_DMX_GLB_FLUSH glb_flush;

    glb_flush.u32 = DMX_READ_REG(DMX_GLB_FLUSH);

    return glb_flush.bits.flush_done ? HI_TRUE : HI_FALSE;
}

/***********************************************************************************
* Function      : DmxHalSetTsRecBufId
* Description   : Set Ts Record Buf Id
* Input         : DmxId, OqId
* Output        :
* Return        :
* Others:       :
***********************************************************************************/
HI_VOID DmxHalSetTsRecBufId(HI_U32 DmxId, HI_U32 OqId)
{
    U_DMX_GLB_CTRL2 glb_ctrl2;

    glb_ctrl2.u32 = DMX_READ_REG(DMX_GLB_CTRL2(DmxId));

    glb_ctrl2.bits.dmx_tsrec_buf = OqId;
    DMX_WRITE_REG(DMX_GLB_CTRL2(DmxId), glb_ctrl2.u32);

    DMX_COM_EQUAL(glb_ctrl2.u32, DMX_READ_REG(DMX_GLB_CTRL2(DmxId)));
}

/***********************************************************************************
* Function      : DmxHalSetSpsRefRecCh
* Description   : Set Sps Reference Channel
* Input         : DmxId, ChanId
* Output        :
* Return        :
* Others:       :
***********************************************************************************/
HI_VOID DmxHalSetSpsRefRecCh(HI_U32 DmxId, HI_U32 ChanId)
{
    U_DMX_GLB_CTRL3 glb_ctrl3;

    glb_ctrl3.u32 = DMX_READ_REG(DMX_GLB_CTRL3(DmxId));
    glb_ctrl3.bits.dmx_spsrec_pusi_ch = ChanId;
    DMX_WRITE_REG(DMX_GLB_CTRL3(DmxId), glb_ctrl3.u32);

    DMX_COM_EQUAL(glb_ctrl3.u32, DMX_READ_REG(DMX_GLB_CTRL3(DmxId)));
}

/***********************************************************************************
* Function      :  DmxHalSetSpsPauseType
* Description   : Set Sps Reference Channel
* Input         : dmxid(= 1,2,3,4,5)type
* Output        :
* Return        :
* Others:       :type  0: after sps_ctrl valid, when find out the spx_num pes header in base channel, pause the play
1:after sps_ctrl valid, when find out ts tail in base channel, puase the timeshift
***********************************************************************************/
HI_VOID DmxHalSetSpsPauseType(HI_U32 DmxId, HI_U32 type)
{
    U_DMX_GLB_CTRL3 glb_ctrl3;

    glb_ctrl3.u32 = DMX_READ_REG(DMX_GLB_CTRL3(DmxId));
    glb_ctrl3.bits.dmx_sps_type = type & 0x1;
    DMX_WRITE_REG(DMX_GLB_CTRL3(DmxId), glb_ctrl3.u32);

    DMX_COM_EQUAL(glb_ctrl3.u32, DMX_READ_REG(DMX_GLB_CTRL3(DmxId)));
}

/***********************************************************************************
* Function      : DmxHalSetFilter
* Description   : Set filter attr
* Input         : FilterNum, Depth, Content, Mask
* Output        :
* Return        :
* Others:       :
***********************************************************************************/
HI_VOID DmxHalSetFilter(HI_U32 FilterId, HI_U32 Depth, HI_U8 Content, HI_BOOL bReverse, HI_U8 Mask)
{
    U_DMX_FILTER dmx_filter_crtl;

    dmx_filter_crtl.u32 = 0;

    dmx_filter_crtl.bits.wdata_mask     = Mask;
    dmx_filter_crtl.bits.wdata_content  = Content;
    dmx_filter_crtl.bits.wdata_mode     = bReverse;
    DMX_WRITE_REG(DMX_FILTERxy(FilterId, Depth), dmx_filter_crtl.u32);
}

/***********************************************************************************
* Function      : DmxHalClearOq
* Description   : Clear Oq
* Input         : OQId
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalClearOq(HI_U32 OqId, DMX_OQ_CLEAR_TYPE_E ClearType)
{
    U_CLEAR_CHANNEL clear_ch;

    clear_ch.u32 = DMX_READ_REG(CLR_CHN_CMD);

    clear_ch.bits.clear_chn   = OqId;
    clear_ch.bits.clear_type  = ClearType;
    clear_ch.bits.clear_start = 1;
    DMX_WRITE_REG(CLR_CHN_CMD, clear_ch.u32);
}

/***********************************************************************************
* Function      : DmxHalIsClearOqDone
* Description   :
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_BOOL DmxHalIsClearOqDone(HI_VOID)
{
    HI_BOOL         ret = HI_FALSE;
    U_IRAW_CLR_CHN  clear_ch;

    clear_ch.u32 = DMX_READ_REG(RAW_CLR_CHN);
    if (clear_ch.bits.iraw_clr_chn)
    {
        DMX_WRITE_REG(RAW_CLR_CHN, clear_ch.u32);

        ret = HI_TRUE;
    }

    return ret;
}

/***********************************************************************************
* Function      :  DmxHalEnableAllPVRInt
* Description   :    Get eop int status
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalEnableAllPVRInt(HI_VOID)
{
    DMX_WRITE_REG(ENA_PVR_INT, 1);
}

/***********************************************************************************
* Function      :  DmxHalDisableAllPVRInt
* Description   :    Get eop int status
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalDisableAllPVRInt(HI_VOID)
{
    DMX_WRITE_REG(ENA_PVR_INT, 0);

    DMX_COM_EQUAL(0, DMX_READ_REG(ENA_PVR_INT));
}

#ifdef HI_DEMUX_PROC_SUPPORT
/***********************************************************************************
* Function      : DmxHalFQEnableAllOverflowInt
* Description   : enable fq overflow int
* Input         :
* Output        :
* Return        :
* Others        :
***********************************************************************************/
HI_VOID DmxHalFQEnableAllOverflowInt(HI_VOID)
{
    U_ENA_INT_TYPE unEnaErr;

    unEnaErr.u32 = DMX_READ_REG(ENA_INT_TYPE);

    unEnaErr.bits.iena_chn_i = 0x3;
    DMX_WRITE_REG(ENA_INT_TYPE, unEnaErr.u32);
}

/***********************************************************************************
* Function      : DmxHalFQGetAllOverflowIntStatus
* Description   : Get fq overflow int status
* Input         :
* Output        :
* Return        :
* Others        :
***********************************************************************************/
HI_U32 DmxHalFQGetAllOverflowIntStatus(HI_VOID)
{
    U_INT_STA_TYPE unIntSta;

    unIntSta.u32 = DMX_READ_REG(INT_STA_TYPE);

    return unIntSta.bits.iint_chn_i;
}

HI_U32 DmxHalFQGetOverflowIntStatus(HI_U32 offset)
{
    return DMX_READ_REG(INT_FQ_CHN_0(offset));
}

HI_VOID DmxHalFQClearOverflowInt(HI_U32 FqId)
{
    HI_U32  offset  = FqId >> 5;
    HI_U32  bit     = FqId & 0x1f;
    HI_U32  value;

    value = 1 << bit;
    DMX_WRITE_REG(RAW_FQ_CHN_0(offset), value);
}

HI_VOID DmxHalFQSetOverflowInt(HI_U32 FqId, HI_BOOL Enable)
{
    HI_U32  offset  = FqId >> 5;
    HI_U32  bit     = FqId & 0x1F;
    HI_U32  value;

    value = DMX_READ_REG(ENA_FQ_CHN_0(offset));

    if (Enable)
    {
        value |= 1 << bit;
    }
    else
    {
        value &= ~(1 << bit);
    }
    DMX_WRITE_REG(ENA_FQ_CHN_0(offset), value);
}

HI_BOOL DmxHalFQIsEnableOverflowInt(HI_U32 FqId)
{
    HI_U32  offset  = FqId >> 5;
    HI_U32  bit     = FqId & 0x1F;
    HI_U32  value;

    value = DMX_READ_REG(ENA_FQ_CHN_0(offset));

    return (value & (1 << bit)) ? HI_TRUE : HI_FALSE;
}
#endif

/***********************************************************************************
* Function      : DmxHalOQGetAllOverflowIntStatus
* Description   : Get oq overflow int status
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_U32 DmxHalOQGetAllOverflowIntStatus(HI_VOID)
{
    U_INT_STA_TYPE unIntSta;

    unIntSta.u32 = DMX_READ_REG(INT_STA_TYPE);

    return unIntSta.bits.iint_chn_o;
}

/***********************************************************************************
* Function      : DmxHalOQGetAllEopIntStatus
* Description   :
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_U32 DmxHalOQGetAllEopIntStatus(HI_VOID)
{
    U_INT_STA_TYPE unIntSta;

    unIntSta.u32 = DMX_READ_REG(INT_STA_TYPE);

    return unIntSta.bits.iint_eop_o;
}

/***********************************************************************************
* Function      :  DmxHalEnableAllChEopInt
* Description   :    Set channel overflow  int enable
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalEnableAllChEopInt(HI_VOID)
{
    U_ENA_INT_TYPE unEnaErr;

    unEnaErr.u32 = DMX_READ_REG(ENA_INT_TYPE);
    unEnaErr.bits.iena_eop_o = 0xf;
    DMX_WRITE_REG(ENA_INT_TYPE, unEnaErr.u32);
}

/***********************************************************************************
* Function      :  DmxHalEnableAllChEnqueInt
* Description   :    Set channel overflow  int enable
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalEnableAllChEnqueInt(HI_VOID)
{
    U_ENA_INT_TYPE unEnaErr;

    unEnaErr.u32 = DMX_READ_REG(ENA_INT_TYPE);
    unEnaErr.bits.iena_desc_o = 0xf;
    DMX_WRITE_REG(ENA_INT_TYPE, unEnaErr.u32);
}

/***********************************************************************************
* Function      :  DmxHalEnableFQOutqueInt
* Description   :    Set channel overflow  int enable
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalEnableFQOutqueInt(HI_VOID)
{
    U_ENA_INT_TYPE unEnaErr;

    unEnaErr.u32 = DMX_READ_REG(ENA_INT_TYPE);
    unEnaErr.bits.iena_desc_i = 0x3;
    DMX_WRITE_REG(ENA_INT_TYPE, unEnaErr.u32);
}

/***********************************************************************************
* Function      :  DmxHalDisableFQOutqueInt
* Description   :    Set IP  int disable
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalDisableFQOutqueInt(HI_VOID)
{
    U_ENA_INT_TYPE unEnaErr;

    unEnaErr.u32 = DMX_READ_REG(ENA_INT_TYPE);
    unEnaErr.bits.iena_desc_i = 0;
    DMX_WRITE_REG(ENA_INT_TYPE, unEnaErr.u32);
}

/***********************************************************************************
* Function      :  DmxHalSetFlushMaxWaitTime
* Description   :    Set play dmx2buf bp mode
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalSetFlushMaxWaitTime(HI_U32 u32MaxTime)
{
    U_CLR_WAIT_TIME unWaitTime;

    unWaitTime.u32 = DMX_READ_REG(CLR_WAIT_TIME);
    unWaitTime.bits.clr_wait_time = u32MaxTime & 0xffff;
    DMX_WRITE_REG(CLR_WAIT_TIME, unWaitTime.u32);
}

/***********************************************************************************
* Function      :  DmxHalSetScdFilter
* Description   :    set SCD filter content
* Input         : u32FltId u8Content
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_S32 DmxHalSetScdFilter(HI_U32 u32FltId, HI_U8 u8Content)
{
    U_SCD_FLT filter;
    HI_U32 offset = 0;

    if (u32FltId >= 10)
    {
        return HI_FAILURE;
    }

    offset = u32FltId >> 2;

    offset <<= 2;

    u32FltId -= offset;

    filter.u32 = DMX_READ_REG(SCD_FLT0_3 + offset);

    switch (u32FltId)
    {
    case 0:
        filter.bits.flt0 = u8Content;
        break;
    case 1:
        filter.bits.flt1 = u8Content;
        break;
    case 2:
        filter.bits.flt2 = u8Content;
        break;
    case 3:
        filter.bits.flt3 = u8Content;
        break;
    default:
        return HI_FAILURE;
    }

    DMX_WRITE_REG(SCD_FLT0_3 + offset, filter.u32);

    DMX_COM_EQUAL(filter.u32, DMX_READ_REG(SCD_FLT0_3 + offset));

    return HI_SUCCESS;
}

/***********************************************************************************
* Function      :  DmxHalSetScdRangeFilter
* Description   :    set SCD filter range
* Input         : u32FltId u8High u8Low
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_S32 DmxHalSetScdRangeFilter(HI_U32 u32FltId, HI_U8 u8High, HI_U8 u8Low)
{
    U_SCD_FLT filter;
    HI_U32 offset = 0;

    if (u32FltId >= DMX_TOTAL_RANGE_FLTNUM)
    {
        return HI_FAILURE;
    }

    /*calculate the offset of each scd filter*/
    switch (u32FltId)
    {
    case 1:
    case 2:
        offset = 4;
        break;
    case 3:
    case 4:
        offset = 8;
        break;
    case 5:
    case 6:
        offset = 12;
        break;
    case 0:
    default:
        offset = 0;
        break;
    }

    filter.u32 = DMX_READ_REG(SCD_FLT8_11 + offset);

    switch ((u32FltId + 1) % 2)
    {
    case 0:
        filter.bits.flt0 = u8Low;
        filter.bits.flt1 = u8High;
        break;
    case 1:
        filter.bits.flt2 = u8Low;
        filter.bits.flt3 = u8High;
        break;
    default:
        return HI_FAILURE;
    }

    DMX_WRITE_REG(SCD_FLT8_11 + offset, filter.u32);

    DMX_COM_EQUAL(filter.u32, DMX_READ_REG(SCD_FLT8_11 + offset));

    return HI_SUCCESS;
}

/***********************************************************************************
* Function      : DmxHalSetScdNewRangeFilter
* Description   : set SCD new filter range
* Input         : FilterId, High, Low, Mask, Negate
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalSetScdNewRangeFilter(HI_U32 FilterId, HI_U8 High, HI_U8 Low, HI_U8 Mask, HI_BOOL Negate)
{
    U_SCD_FLT_V200  filter;
    HI_U32          new_flt_neg;

    filter.u32 = DMX_READ_REG(SCD_NEW_FLTSET(FilterId));
    filter.bits.byte_h  = High;
    filter.bits.byte_l  = Low;
    filter.bits.mask    = Mask;

    DMX_WRITE_REG(SCD_NEW_FLTSET(FilterId), filter.u32);

    DMX_COM_EQUAL(filter.u32, DMX_READ_REG(SCD_NEW_FLTSET(FilterId)));

    new_flt_neg = DMX_READ_REG(SCD_NEW_FLT_NEG);
    if (Negate)
    {
        new_flt_neg |= (1 << FilterId);
    }
    else
    {
        new_flt_neg &= ~(1 << FilterId);
    }

    DMX_WRITE_REG(SCD_NEW_FLT_NEG, new_flt_neg);

    DMX_COM_EQUAL(new_flt_neg, DMX_READ_REG(SCD_NEW_FLT_NEG));
}

/***********************************************************************************
* Function      : DmxHalChooseScdFilter
* Description   : Choose Scd Filter
* Input         : ScdId, FilterId
* Output        :
* Return        :
* Others        :
***********************************************************************************/
HI_VOID DmxHalChooseScdFilter(HI_U32 ScdId, HI_U32 FilterId)
{
    U_SCD_SET scd_set;

    scd_set.u32 = DMX_READ_REG(SCD_SETa(ScdId));

    //Note: for 3716MV200, SCD's byte_flt_en(10bit) is opened always for all SCD.
    scd_set.bits.flt_en0 |= (1 << FilterId);

    DMX_WRITE_REG(SCD_SETa(ScdId), scd_set.u32);

    DMX_COM_EQUAL(scd_set.u32, DMX_READ_REG(SCD_SETa(ScdId)));
}

/***********************************************************************************
* Function      : DmxHalScdFilterClear
* Description   : Clear Scd Filter
* Input         : ScdId
* Output        :
* Return        :
* Others        :
***********************************************************************************/
HI_VOID DmxHalScdFilterClear(HI_U32 ScdId)
{
    U_SCD_SET scd_set;

    scd_set.u32 = DMX_READ_REG(SCD_SETa(ScdId));

    scd_set.bits.flt_en0 = 0;

    DMX_WRITE_REG(SCD_SETa(ScdId), scd_set.u32);

    DMX_COM_EQUAL(scd_set.u32, DMX_READ_REG(SCD_SETa(ScdId)));
}

/***********************************************************************************
* Function      : DmxHalChooseScdRangeFilter
* Description   : Choose Scd Range Filter
* Input         : ScdId, FilterId
* Output        :
* Return        :
* Others        :
***********************************************************************************/
HI_VOID DmxHalChooseScdRangeFilter(HI_U32 ScdId, HI_U32 FilterId)
{
    U_SCD_SET scd_set;

    scd_set.u32 = DMX_READ_REG(SCD_SETa(ScdId));

    if (FilterId < 2)
    {
        scd_set.bits.flt_en1 |= (1 << FilterId);
    }
    else
    {
        scd_set.bits.flt_en2 |= (1 << (FilterId - 2));
    }

    DMX_WRITE_REG(SCD_SETa(ScdId), scd_set.u32);

    DMX_COM_EQUAL(scd_set.u32, DMX_READ_REG(SCD_SETa(ScdId)));
}

/***********************************************************************************
* Function      : DmxHalScdRangeFilterClear
* Description   : Clear Scd Range Filter
* Input         : ScdId
* Output        :
* Return        :
* Others        :
***********************************************************************************/
HI_VOID DmxHalScdRangeFilterClear(HI_U32 ScdId)
{
    U_SCD_SET scd_set;

    scd_set.u32 = DMX_READ_REG(SCD_SETa(ScdId));

    scd_set.bits.flt_en1 = 0;
    scd_set.bits.flt_en2 = 0;

    DMX_WRITE_REG(SCD_SETa(ScdId), scd_set.u32);

    DMX_COM_EQUAL(scd_set.u32, DMX_READ_REG(SCD_SETa(ScdId)));
}

/***********************************************************************************
* Function      : DmxHalChooseScdNewRangeFilter
* Description   : Choose Scd New Range Filter
* Input         : ScdId, FilterId
* Output        :
* Return        :
* Others        :
***********************************************************************************/
HI_VOID DmxHalChooseScdNewRangeFilter(HI_U32 ScdId, HI_U32 FilterId)
{
    HI_U32 flt_en;

    flt_en = DMX_READ_REG(SCD_NEW_FLTEN(ScdId));

    flt_en |= (1 << FilterId);
    DMX_WRITE_REG(SCD_NEW_FLTEN(ScdId), flt_en);

    DMX_COM_EQUAL(flt_en, DMX_READ_REG(SCD_NEW_FLTEN(ScdId)));
}

/***********************************************************************************
* Function      : DmxHalScdNewRangeFilterClear
* Description   : Clear Scd New Range Filter
* Input         : ScdId
* Output        :
* Return        :
* Others        :
***********************************************************************************/
HI_VOID DmxHalScdNewRangeFilterClear(HI_U32 ScdId)
{
    HI_U32 flt_en;

    flt_en = DMX_READ_REG(SCD_NEW_FLTEN(ScdId));

    flt_en &= 0xFFFF0000;
    DMX_WRITE_REG(SCD_NEW_FLTEN(ScdId), flt_en);

    DMX_COM_EQUAL(flt_en, DMX_READ_REG(SCD_NEW_FLTEN(ScdId)));
}

/***********************************************************************************
* Function      : DmxHalEnablePesSCD
* Description   : enbale pes
* Input         : ScdId
* Output        :
* Return        :
* Others        :
***********************************************************************************/
HI_VOID DmxHalEnablePesSCD(HI_U32 ScdId)
{
    U_SCD_SET scd_set;

    scd_set.u32 = DMX_READ_REG(SCD_SETa(ScdId));

    scd_set.bits.pes_en = 1;

    DMX_WRITE_REG(SCD_SETa(ScdId), scd_set.u32);

    DMX_COM_EQUAL(scd_set.u32, DMX_READ_REG(SCD_SETa(ScdId)));
}

/***********************************************************************************
* Function      : DmxHalDisablePesSCD
* Description   : disbale pes
* Input         : ScdId
* Output        :
* Return        :
* Others        :
***********************************************************************************/
HI_VOID DmxHalDisablePesSCD(HI_U32 ScdId)
{
    U_SCD_SET scd_set;

    scd_set.u32 = DMX_READ_REG(SCD_SETa(ScdId));

    scd_set.bits.pes_en = 0;

    DMX_WRITE_REG(SCD_SETa(ScdId), scd_set.u32);

    DMX_COM_EQUAL(scd_set.u32, DMX_READ_REG(SCD_SETa(ScdId)));
}

/***********************************************************************************
* Function      : DmxHalEnableEsSCD
* Description   : enbale es
* Input         : ScdId
* Output        :
* Return        :
* Others        :
***********************************************************************************/
HI_VOID DmxHalEnableEsSCD(HI_U32 ScdId)
{
    U_SCD_SET scd_set;

    scd_set.u32 = DMX_READ_REG(SCD_SETa(ScdId));

    scd_set.bits.esscd_en = 1;

    DMX_WRITE_REG(SCD_SETa(ScdId), scd_set.u32);

    DMX_COM_EQUAL(scd_set.u32, DMX_READ_REG(SCD_SETa(ScdId)));
}

/***********************************************************************************
* Function      : DmxHalDisableEsSCD
* Description   : disbale es
* Input         : ScdId
* Output        :
* Return        :
* Others        :
***********************************************************************************/
HI_VOID DmxHalDisableEsSCD(HI_U32 ScdId)
{
    U_SCD_SET scd_set;

    scd_set.u32 = DMX_READ_REG(SCD_SETa(ScdId));

    scd_set.bits.esscd_en = 0;

    DMX_WRITE_REG(SCD_SETa(ScdId), scd_set.u32);

    DMX_COM_EQUAL(scd_set.u32, DMX_READ_REG(SCD_SETa(ScdId)));
}

/***********************************************************************************
* Function      : DmxHalEnableMp4SCD
* Description   : enbale m4_short_en
* Input         : ScdId
* Output        :
* Return        :
* Others        :
***********************************************************************************/
HI_VOID DmxHalEnableMp4SCD(HI_U32 ScdId)
{
    U_SCD_SET scd_set;

    scd_set.u32 = DMX_READ_REG(SCD_SETa(ScdId));

    scd_set.bits.m4_short_en = 1;

    DMX_WRITE_REG(SCD_SETa(ScdId), scd_set.u32);

    DMX_COM_EQUAL(scd_set.u32, DMX_READ_REG(SCD_SETa(ScdId)));
}

/***********************************************************************************
* Function      : DmxHalDisableMp4SCD
* Description   : disbale m4_short_en
* Input         : ScdId
* Output        :
* Return        :
* Others        :
***********************************************************************************/
HI_VOID DmxHalDisableMp4SCD(HI_U32 ScdId)
{
    U_SCD_SET scd_set;

    scd_set.u32 = DMX_READ_REG(SCD_SETa(ScdId));

    scd_set.bits.m4_short_en = 0;

    DMX_WRITE_REG(SCD_SETa(ScdId), scd_set.u32);

    DMX_COM_EQUAL(scd_set.u32, DMX_READ_REG(SCD_SETa(ScdId)));
}

/***********************************************************************************
* Function      : DmxHalSetSCDAttachChannel
* Description   : set SCD channel
* Input         : ScdId ChanId
* Output        :
* Return        :
* Others        :
***********************************************************************************/
HI_VOID DmxHalSetSCDAttachChannel(HI_U32 ScdId, HI_U32 ChanId)
{
    U_SCD_SET scd_set;

    scd_set.u32 = DMX_READ_REG(SCD_SETa(ScdId));

    scd_set.bits.scd_ch = ChanId;

    DMX_WRITE_REG(SCD_SETa(ScdId), scd_set.u32);

    DMX_COM_EQUAL(scd_set.u32, DMX_READ_REG(SCD_SETa(ScdId)));
}

/***********************************************************************************
* Function      : DmxHalAllocSCDBufferId
* Description   : set SCD buffer id
* Input         : ScdId, OqId
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalAllocSCDBufferId(HI_U32 ScdId, HI_U32 OqId)
{
    U_SCD_BUF scd_buf;

    scd_buf.u32 = DMX_READ_REG(SCD_BUF0(ScdId));

    scd_buf.bits.buf_num = OqId & 0x7f;

    DMX_WRITE_REG(SCD_BUF0(ScdId), scd_buf.u32);

    DMX_COM_EQUAL(scd_buf.u32, DMX_READ_REG(SCD_BUF0(ScdId)));
}

/***********************************************************************************
* Function      :  DmxHalSetFlushIPPort
* Description   :  Set Flush IP Port
* Input         : u32PortId
* Output        :
* Return        :
* Others:       :
***********************************************************************************/
HI_VOID DmxHalSetFlushIPPort(HI_U32 PortId)
{
    U_IP_CLRCHN_REQ ip_flush;

    ip_flush.u32 = DMX_READ_REG(IP_CLRCHN_REQ(PortId));

    ip_flush.bits.ip_clrchn_req = 1;
    DMX_WRITE_REG(IP_CLRCHN_REQ(PortId), ip_flush.u32);
}

/***********************************************************************************
* Function      :  DmxHalGetChannelTSScrambleFlag
* Description   :   Get Channel CRC Mode
* Input         : chid (= 0,1,2)
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalGetChannelTSScrambleFlag(HI_U32 ChanId, HI_BOOL  *pEnable)
{
    U_CH_HIS ch_sramble;

    ch_sramble.u32 = DMX_READ_REG(CH_HIS(ChanId));
    *pEnable = (ch_sramble.bits.ts_scr_flag & 0x1);
}

/***********************************************************************************
* Function      :  DmxHalGetChannelPesScrambleFlag
* Description   :   Get Channel CRC Mode
* Input         : chid (= 0,1,2)
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalGetChannelPesScrambleFlag(HI_U32 ChanId, HI_BOOL  *pEnable)
{
    U_CH_HIS ch_sramble;

    ch_sramble.u32 = DMX_READ_REG(CH_HIS(ChanId));
    *pEnable = (ch_sramble.bits.pes_scr_flag & 0x1);
}

/***********************************************************************************
* Function      : DmxHalClrAutoIPBP
* Description   : IP Auto Clear BP
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalIPPortAutoClearBP(HI_VOID)
{
    U_IP_BP_CLR_CFG reg;

    reg.value = DMX_READ_REG(IP_BP_CLR_CFG);

    reg.bits.ipaful_clr_ena = 0;
    DMX_WRITE_REG(IP_BP_CLR_CFG, reg.value);

    DMX_COM_EQUAL(reg.value, DMX_READ_REG(IP_BP_CLR_CFG));
}

/***********************************************************************************
* Function      : DmxHalGetIPBPStatus
* Description   : Get IP BP Status
* Input         : PortId
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_BOOL DmxHalGetIPBPStatus(HI_U32 PortId)
{
    HI_U32 status32, status40,regaddr;
    IP_BP_FQ_STA(PortId, 0,regaddr);
    status32 = DMX_READ_REG(regaddr);
    IP_BP_FQ_STA(PortId, 1,regaddr);
    status40 = DMX_READ_REG(regaddr) & 0xff;

    return (status32 || status40);
}

/***********************************************************************************
* Function      : DmxHalClrIPBPStatus
* Description   : clear IP BP Status
* Input         : PortId
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalClrIPBPStatus(HI_U32 PortId)
{
    HI_U32 regaddr;
    IP_BP_FQ_STA(PortId, 0,regaddr);
    DMX_WRITE_REG(regaddr, 0);
    IP_BP_FQ_STA(PortId, 1,regaddr);
    DMX_WRITE_REG(regaddr, 0);
}

/***********************************************************************************
* Function      : DmxHalClrIPFqBPStatus
* Description   : clear IP BP Status for one fq
* Input         : PortId, FQId
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalClrIPFqBPStatus(HI_U32 PortId, HI_U32 FQId)
{
    HI_U32  offset  = FQId >> 5;
    HI_U32  bit     = FQId & 0x1F;
    HI_U32  value;
    HI_U32 regaddr;

    IP_BP_FQ_STA(PortId, offset,regaddr);
    value = DMX_READ_REG(regaddr);

    value &= ~(1 << bit);
    DMX_WRITE_REG(regaddr, value);
}

/***********************************************************************************
* Function      : DmxHalSetPcrDmxId
* Description   : Set PCR DmxId
* Input         : PcrId, DmxId
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalSetPcrDmxId(const HI_U32 PcrId, const HI_U32 DmxId)
{
    U_DMX_PCR_SET PcrSet;

    PcrSet.u32 = DMX_READ_REG(DMX_PCR_SET(PcrId));

    PcrSet.bits.pcr_dmx_id = DmxId + 1;
    DMX_WRITE_REG(DMX_PCR_SET(PcrId), PcrSet.u32);

    DMX_COM_EQUAL(PcrSet.u32, DMX_READ_REG(DMX_PCR_SET(PcrId)));
}

/***********************************************************************************
* Function      : DmxHalSetPcrPid
* Description   : Set PCR Pid
* Input         : PcrId, PcrPid
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalSetPcrPid(const HI_U32 PcrId, const HI_U32 PcrPid)
{
    U_DMX_PCR_SET PcrSet;

    PcrSet.u32 = DMX_READ_REG(DMX_PCR_SET(PcrId));

    PcrSet.bits.pcr_pid = PcrPid & 0x1fff;
    DMX_WRITE_REG(DMX_PCR_SET(PcrId), PcrSet.u32);

    DMX_COM_EQUAL(PcrSet.u32, DMX_READ_REG(DMX_PCR_SET(PcrId)));
}

/***********************************************************************************
* Function      : DmxHalGetPcrValue
* Description   : Get PCR value
* Input         : PcrId, PcrVal
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalGetPcrValue(const HI_U32 PcrId, HI_U64 *PcrVal)
{
    HI_U64 value0 = 0, value1 = 0;

    //PCR value(extend bit and high bit)register 0,0x3F00+0x4*j
    //pcr_extra_8_0 16:8    RO  0x000   PCR extend bit
    //reserved       7:1    RO  0x00    reserved
    //pcr_base_32      0    RO  0x0     PCR_base the highest bit
    //PCR value(low 32 bits) register 1 0x3F04+0x4*j
    //pcr_base_31_0 31:0    RO  0x00000000  PCR_base low 32 bits

    value0 = DMX_READ_REG(DMX_CH_PCR_VALUE0(PcrId));
    value1 = DMX_READ_REG(DMX_CH_PCR_VALUE1(PcrId));

    //*PcrVal = ((((value0&0xff00)>>7) +(value0&0x1))<<32) + value1;
    *PcrVal = ((value0 & 0x1) << 32) + value1;
}

/***********************************************************************************
* Function      : DmxHalGetScrValue
* Description   : Get SCR value
* Input         : PcrId, ScrVal
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalGetScrValue(const HI_U32 PcrId, HI_U64 *ScrVal)
{
    HI_U64 value0 = 0, value1 = 0;

    //when PCR incoming SCRvalue(extend bit and high bit) register 0    0x3F08+0x4*j
    //scr_extra_8_0 16:8    RO  0x000   SCR extend bit
    //reserved  7:1 RO  0x00    reserved
    //scr_base_32   0   RO  0x0 SCR_base the highest bit

    //when PCR incoming SCR value(low 32 bits) register 1   0x3F0C+0x4*j
    //scr_base_31_0 31:0    RO  0x00000000  SCR_base low 32 bits

    value0 = DMX_READ_REG(DMX_CH_SCR_VALUE0(PcrId));
    value1 = DMX_READ_REG(DMX_CH_SCR_VALUE1(PcrId));

    //*ScrVal = ((((value0&0xff00)>>7) +(value0&0x1))<<32) + value1;
    *ScrVal = ((value0 & 0x1) << 32) + value1;
}

/***********************************************************************************
* Function      :  DmxHalGetTotalTeiIntStatus
* Description   :    Get Total Sync Status
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_U32 DmxHalGetTotalTeiIntStatus(HI_VOID)
{
    U_PVR_INT_SCAN unPvrIntSta;

    unPvrIntSta.u32 = DMX_READ_REG(PVR_INT_SCAN);
    return unPvrIntSta.bits.total_int_err;
}

/***********************************************************************************
* Function      :  DmxHalGetTotalPcrIntStatus
* Description   :    Get Total pcr int  Status
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_U32 DmxHalGetTotalPcrIntStatus(HI_VOID)
{
    U_PVR_INT_SCAN unPvrIntSta;

    unPvrIntSta.u32 = DMX_READ_REG(PVR_INT_SCAN);
    return unPvrIntSta.bits.total_int_pcr;
}

/***********************************************************************************
* Function      :  DmxHalGetTotalDiscIntStatus
* Description   :    Get Total Disc int Status
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_U32 DmxHalGetTotalDiscIntStatus(HI_VOID)
{
    U_PVR_INT_SCAN unPvrIntSta;

    unPvrIntSta.u32 = DMX_READ_REG(PVR_INT_SCAN);
    return unPvrIntSta.bits.total_int_disc;
}

/***********************************************************************************
* Function      :  DmxHalGetTotalCrcIntStatus
* Description   :    Get Total crc int Status
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_U32 DmxHalGetTotalCrcIntStatus(HI_VOID)
{
    U_PVR_INT_SCAN unPvrIntSta;

    unPvrIntSta.u32 = DMX_READ_REG(PVR_INT_SCAN);
    return unPvrIntSta.bits.total_int_fltcrc;
}

/***********************************************************************************
* Function      :  DmxHalGetTotalPenLenIntStatus
* Description   :    Get Total pes len int Status
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_U32 DmxHalGetTotalPenLenIntStatus(HI_VOID)
{
    U_PVR_INT_SCAN unPvrIntSta;

    unPvrIntSta.u32 = DMX_READ_REG(PVR_INT_SCAN);
    return unPvrIntSta.bits.total_int_peslen;
}

/***********************************************************************************
* Function      : DmxHalGetPcrIntStatus
* Description   : Get Pcr Int Status
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_U32 DmxHalGetPcrIntStatus(HI_VOID)
{
    U_STA_PCR_ARRI PcrIntSta;

    PcrIntSta.u32 = DMX_READ_REG(STA_PCR_ARRI);

    return PcrIntSta.bits.sta_pcr;
}

/***********************************************************************************
* Function      : DmxHalClrPcrIntStatus
* Description   : Clean Pcr Int
* Input         : PcrId
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalClrPcrIntStatus(const HI_U32 PcrId)
{
    DMX_WRITE_REG(RAW_PCR_ARRI, (1 << PcrId));
}

/***********************************************************************************
* Function      : DmxHalSetPcrIntEnable
* Description   : Set Pcr Int Enable
* Input         : PcrId, Enable
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalSetPcrIntEnable(const HI_U32 PcrId, const HI_BOOL Enable)
{
    U_ENA_PCR_ARRI  EnaPcrAttr;

    EnaPcrAttr.u32 = DMX_READ_REG(ENA_PCR_ARRI);

    if (Enable)
    {
        EnaPcrAttr.bits.ena_pcr |= (1 << PcrId);
    }
    else
    {
        EnaPcrAttr.bits.ena_pcr &= ~(1 << PcrId);
    }

    DMX_WRITE_REG(ENA_PCR_ARRI, EnaPcrAttr.u32);
}

/***********************************************************************************
* Function      :  DmxHalGetTeiIntInfo
* Description   :    Get Tei Int Info
* Input         :
* Output        :   pu32DmxId , pu32ChanId
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalGetTeiIntInfo(HI_U32* pu32DmxId, HI_U32* pu32ChanId)
{
    if (pu32DmxId && pu32ChanId)
    {
        U_STA_TEI unTeiIntSta;

        unTeiIntSta.u32 = DMX_READ_REG(STA_TEI);

        *pu32DmxId  = unTeiIntSta.bits.tei_dmx;
        *pu32ChanId = unTeiIntSta.bits.tei_ch;
    }
}

/***********************************************************************************
* Function      :  DmxHalClrTeiIntStatus
* Description   :    Clr Tei Int Status
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalClrTeiIntStatus(HI_VOID)
{
    DMX_WRITE_REG(RAW_TEI, 1);
}

/***********************************************************************************
* Function      : DmxHalGetDiscIntStatus
* Description   : Get disc int status
* Input         : RegionNum
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_U32 DmxHalGetDiscIntStatus(HI_U32 RegionNum)
{
    return DMX_READ_REG(STA_DISC0(RegionNum));
}

/***********************************************************************************
* Function      : DmxHalClearDiscIntStatus
* Description   : Clr Disc int status
* Input         : ChanId
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalClearDiscIntStatus(HI_U32 ChanId)
{
    HI_U32  offset  = ChanId >> 5;
    HI_U32  bit     = ChanId & 0x1f;
    HI_U32  value   = (1 << bit);

    DMX_WRITE_REG(RAW_DISC0(offset), value);
}

/***********************************************************************************
* Function      : DmxHalGetCrcIntStatus
* Description   : Get Crc int status
* Input         : RegionNum
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_U32 DmxHalGetCrcIntStatus(HI_U32 RegionNum)
{
    return DMX_READ_REG(STA_FLTCRC0(RegionNum));
}

/***********************************************************************************
* Function      : DmxHalClearCrcIntStatus
* Description   : Clr Crc int status
* Input         : ChanId
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalClearCrcIntStatus(HI_U32 ChanId)
{
    HI_U32  offset  = ChanId >> 5;
    HI_U32  bit     = ChanId & 0x1f;
    HI_U32  value   = (1 << bit);

    DMX_WRITE_REG(RAW_FLTCRC0(offset), value);
}

/***********************************************************************************
* Function      : DmxHalGetPesLenIntStatus
* Description   : Get Pes Len int status
* Input         : RegionNum
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_U32 DmxHalGetPesLenIntStatus(HI_U32 RegionNum)
{
    return DMX_READ_REG(STA_PES_LEN0(RegionNum));
}

/***********************************************************************************
* Function      : DmxHalClearPesLenIntStatus
* Description   : Clr Pes Len int status
* Input         : ChanId
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalClearPesLenIntStatus(HI_U32 ChanId)
{
    HI_U32  offset  = ChanId >> 5;
    HI_U32  bit     = ChanId & 0x1f;
    HI_U32  value   = (1 << bit);

    DMX_WRITE_REG(RAW_PES_LEN0(offset), value);
}

/***********************************************************************************
* Function      :  DmxHalFlushScdBuf
* Description   :    Flush Scd Buf
* Input         : u32ScdId
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalFlushScdBuf(HI_U32 u32ScdId)
{
    U_SCD_CLRBUF unScdClrBuf;

    unScdClrBuf.bits.buf_id = u32ScdId & 0x7;
    DMX_WRITE_REG(SCD_CLRBUF, unScdClrBuf.u32);
}

/***********************************************************************************
* Function      : DmxHalClrScdCnt
* Description   : clear Scd cnt
* Input         : ScdId
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalClrScdCnt(HI_U32 ScdId)
{
    DMX_WRITE_REG(TS_CNT0_L(ScdId - 1), 0);
    DMX_WRITE_REG(TS_CNT0_H(ScdId - 1), 0);
}

#if defined(CHIP_TYPE_hi3716h) || defined(CHIP_TYPE_hi3716c)

HI_VOID DmxHalConfigHardware(HI_VOID)
{
    U_PERI_CRG25 reg;

    reg.value = SYS_READ_REG(PERI_CRG25);

    reg.bits.pvr_srst_req = 1;
    SYS_WRITE_REG(PERI_CRG25, reg.value);

    reg.bits.tsi_clkout_cken    = 1;
    reg.bits.tsi2_cken          = 1;
    reg.bits.tsi1_cken          = 1;
    reg.bits.tsi0_cken          = 1;
    reg.bits.dmx_clk_sel        = 1;
    reg.bits.dmx_cken           = 1;
    reg.bits.pvr_hcken          = 1;
    reg.bits.pvr_srst_req       = 0;

    SYS_WRITE_REG(PERI_CRG25, reg.value);
}

#elif defined(CHIP_TYPE_hi3716m)

HI_VOID DmxHalConfigHardware(HI_VOID)
{
    HI_CHIP_TYPE_E      ChipType;
    HI_CHIP_VERSION_E   Version;

    HI_DRV_SYS_GetChipVersion(&ChipType, &Version);
    if ((HI_CHIP_TYPE_HI3716M == ChipType) && (HI_CHIP_VERSION_V300 == Version))
    {
        U_PERI_CRG25_V300 reg;

        reg.value = SYS_READ_REG(PERI_CRG25);

        reg.bits.pvr_srst_req = 1;
        SYS_WRITE_REG(PERI_CRG25, reg.value);

    #ifdef CFG_HI_DEMUX_CSA3_SUPPORT
        reg.bits.dmx_clk_sel = 1;   // use 171MHz

        DmxHalSetCSA3Reset(0);      // clear csa3.0 reset bit
    #else
        reg.bits.dmx_clk_sel = 0;   // use 198MHz

        DmxHalSetCSA3Reset(1);      // reset csa3.0
    #endif

        reg.bits.tsi2_cken      = 1;
        reg.bits.tsi1_cken      = 1;
        reg.bits.tsi0_cken      = 1;
        reg.bits.dmx_cken       = 1;
        reg.bits.pvr_hcken      = 1;
        reg.bits.pvr_srst_req   = 0;

        SYS_WRITE_REG(PERI_CRG25, reg.value);
    }
    else
    {
        U_PERI_CRG25 reg;

        reg.value = SYS_READ_REG(PERI_CRG25);

        reg.bits.pvr_srst_req = 1;
        SYS_WRITE_REG(PERI_CRG25, reg.value);

        reg.bits.tsi_clkout_cken    = 1;
        reg.bits.tsi2_cken          = 1;
        reg.bits.tsi1_cken          = 1;
        reg.bits.tsi0_cken          = 1;
        reg.bits.dmx_clk_sel        = 0;
        reg.bits.dmx_cken           = 1;
        reg.bits.pvr_hcken          = 1;
        reg.bits.pvr_srst_req       = 0;

        SYS_WRITE_REG(PERI_CRG25, reg.value);
    }
}

#elif defined(CHIP_TYPE_hi3712) || defined(CHIP_TYPE_hi3715)

HI_VOID DmxHalConfigHardware(HI_VOID)
{
    U_PERI_CRG25 reg;

    reg.value = SYS_READ_REG(PERI_CRG25);

    reg.bits.pvr_srst_req = 1;
    SYS_WRITE_REG(PERI_CRG25, reg.value);

    reg.bits.dmx_clk_sel    = 0;
    reg.bits.tsi1_cken      = 1;
    reg.bits.tsi0_cken      = 1;
    reg.bits.dmx_cken       = 1;
    reg.bits.pvr_hcken      = 1;
    reg.bits.pvr_srst_req   = 0;

    SYS_WRITE_REG(PERI_CRG25, reg.value);
}

#elif defined (CHIP_TYPE_hi3716cv200es)    || defined (CHIP_TYPE_hi3716cv200) \
	|| defined (CHIP_TYPE_hi3719cv100) || defined (CHIP_TYPE_hi3718cv100)  \
	|| defined (CHIP_TYPE_hi3719mv100) || defined (CHIP_TYPE_hi3719mv100_a)\
	|| defined (CHIP_TYPE_hi3718mv100)  
HI_VOID DmxHalConfigHardware(HI_VOID)
{
    U_PERI_CRG63 PeriCrg63;
    U_PERI_CRG64 PeriCrg64;

    PeriCrg63.u32 = g_pstRegCrg->PERI_CRG63.u32;

    // reset demux
    PeriCrg63.bits.pvr_srst_req = 1;
    g_pstRegCrg->PERI_CRG63.u32 = PeriCrg63.u32;

    udelay(1);

    PeriCrg63.bits.pvr_bus_cken     = 1;
    PeriCrg63.bits.pvr_dmx_cken     = 1;
    PeriCrg63.bits.pvr_27m_cken     = 1;
    PeriCrg63.bits.pvr_tsi1_cken    = 1;
    PeriCrg63.bits.pvr_tsi2_cken    = 1;
#if defined(CHIP_TYPE_hi3716cv200es) || defined(CHIP_TYPE_hi3716cv200)   
    PeriCrg63.bits.pvr_tsi3_cken    = 1;
    PeriCrg63.bits.pvr_tsi4_cken    = 1;
    PeriCrg63.bits.pvr_tsi5_cken    = 1;
#endif
#ifdef CHIP_TYPE_hi3716cv200es
    PeriCrg63.bits.pvr_tsi6_cken    = 1;
    PeriCrg63.bits.pvr_tsi7_cken    = 1;
#endif
    PeriCrg63.bits.pvr_ts0_cken     = 1;
    PeriCrg63.bits.pvr_ts1_cken     = 1;
    PeriCrg63.bits.pvr_tsout0_cken  = 1;
    PeriCrg63.bits.pvr_tsout1_cken  = 1;
    PeriCrg63.bits.pvr_srst_req     = 0;

    g_pstRegCrg->PERI_CRG63.u32 = PeriCrg63.u32;

    PeriCrg64.u32 = g_pstRegCrg->PERI_CRG64.u32;

    PeriCrg64.bits.pvr_ts0_clk_sel  = 1;
    PeriCrg64.bits.pvr_ts1_clk_sel  = 1;

    g_pstRegCrg->PERI_CRG64.u32 = PeriCrg64.u32;
}

#elif defined(CHIP_TYPE_hi3798cv100)

HI_VOID DmxHalConfigHardware(HI_VOID)
{
}

#endif

/***********************************************************************************
* Function      : DmxHalAttachFilter
* Description   : Attach Filter to channel
* Input         : FilterId, ChanId
* Output        :
* Return        :
* Others:       :
***********************************************************************************/
HI_VOID DmxHalAttachFilter(HI_U32 FilterId, HI_U32 ChanId)
{
    HI_U32  filter_en = 1 << (FilterId & 0x1F);
#ifndef DMX_REGION_SUPPORT
    HI_U32  value;
    HI_U32  offset;
    HI_U32  i;

    value = DMX_READ_REG(DMX_FILTER_EN(ChanId));
    for (i = 0; i < 32; i++)
    {
        if (0 == (value & (1 << i)))
        {
            break;
        }
    }

    value = DMX_READ_REG(DMX_FILTER_ID(ChanId, i));

    offset = (i & 0x3) * 8;

    value &= ~(0xFF << offset);
    value |= FilterId << offset;

    DMX_WRITE_REG(DMX_FILTER_ID(ChanId, i), value);

    filter_en = (1 << i);
#endif
    filter_en |= DMX_READ_REG(DMX_FILTER_EN(ChanId));

    DMX_WRITE_REG(DMX_FILTER_EN(ChanId), filter_en);

    DMX_COM_EQUAL(filter_en, DMX_READ_REG(DMX_FILTER_EN(ChanId)));
}

/***********************************************************************************
* Function      : DmxHalDetachFilter
* Description   : Detach Filter from channel
* Input         : FilterId, ChanId
* Output        :
* Return        :
* Others:       :
***********************************************************************************/
HI_VOID DmxHalDetachFilter(HI_U32 FilterId, HI_U32 ChanId)
{
    HI_U32  filter_en = ~(1 << (FilterId & 0x1F));
#ifndef DMX_REGION_SUPPORT
    HI_U32  value;
    HI_U32  i;

    for (i = 0; i < 8; i++)
    {
        HI_U32 j;

        value = DMX_READ_REG(DMX_FILTER_ID(ChanId, i * 4));

        for (j = 0; j < 4; j++)
        {
            HI_U32 Id = (value >> (j * 8)) & 0xff;

            if (Id == FilterId)
            {
                break;
            }
        }

        if (j < 4)
        {
            filter_en = ~(1 << (i * 4 + j));

            break;
        }
    }
#endif
    filter_en &= DMX_READ_REG(DMX_FILTER_EN(ChanId));

    DMX_WRITE_REG(DMX_FILTER_EN(ChanId), filter_en);

    DMX_COM_EQUAL(filter_en, DMX_READ_REG(DMX_FILTER_EN(ChanId)));
}

/***********************************************************************************
* Function      :  DmxHalEnableOQOutDInt
* Description   :    Set fix int enale
* Input         : u32FQId
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalEnableOQOutDInt(HI_U32 OQId)
{
    HI_U32  offset  = OQId >> 5;
    HI_U32  bit     = OQId & 0x1F;
    HI_U32  value;

    value  = DMX_READ_REG(ENA_OQ_DESC_0(offset));

    value |= 1 << bit;
    DMX_WRITE_REG(ENA_OQ_DESC_0(offset), value);
}

/***********************************************************************************
* Function      :  DmxHalDisableOQOutDInt
* Description   :    Set fix int enale
* Input         : u32OQId
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalDisableOQOutDInt(HI_U32 OQId)
{
    HI_U32  offset  = OQId >> 5;
    HI_U32  bit     = OQId & 0x1F;
    HI_U32  value;

    value  = DMX_READ_REG(ENA_OQ_DESC_0(offset));

    value &= ~(1 << bit);
    DMX_WRITE_REG(ENA_OQ_DESC_0(offset), value);
}

/***********************************************************************************
* Function      :  DmxHalGetOQEopIntStatus
* Description   :    Get fix int status
* Input         : u32FQId
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_BOOL DmxHalGetOQEopIntStatus(HI_U32 OQId)
{
    HI_U32  offset  = OQId >> 5;
    HI_U32  bit     = OQId & 0x1f;
    HI_U32  value;

    value = DMX_READ_REG(INT_OQ_EOP_0(offset));

    return (value & (1 << bit)) ? HI_TRUE : HI_FALSE;
}

/***********************************************************************************
* Function      :  DmxHalGetOQEopIntStatus
* Description   :    Get eop int status
* Input         : u32FQId
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalClearOQEopIntStatus(HI_U32 OQId)
{
    HI_U32  offset  = OQId >> 5;
    HI_U32  bit     = OQId & 0x1f;
    HI_U32  value;

    value = 1 << bit;
    DMX_WRITE_REG(RAW_OQ_EOP_0(offset), value);
}

/***********************************************************************************
* Function      :  DmxHalEnableOQEopInt
* Description   :    Set fix int enale
* Input         : u32FQId
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalEnableOQEopInt(HI_U32 OQId)
{
    HI_U32  offset  = OQId >> 5;
    HI_U32  bit     = OQId & 0x1f;
    HI_U32  value;

    value  = DMX_READ_REG(ENA_OQ_EOP_0(offset));

    value |= (1 << bit);
    DMX_WRITE_REG(ENA_OQ_EOP_0(offset), value);
}

/***********************************************************************************
* Function      :  DmxHalDisableOQEopInt
* Description   :    Set fix int enale
* Input         : u32OQId
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalDisableOQEopInt(HI_U32 OQId)
{
    HI_U32  offset  = OQId >> 5;
    HI_U32  bit     = OQId & 0x1f;
    HI_U32  value;

    value  = DMX_READ_REG(ENA_OQ_EOP_0(offset));

    value &= ~(1 << bit);
    DMX_WRITE_REG(ENA_OQ_EOP_0(offset), value);
}

HI_BOOL DmxHalOQGetOverflowIntStatus(HI_U32 OQId)
{
    HI_U32  offset  = OQId >> 5;
    HI_U32  bit     = OQId & 0x1f;
    HI_U32  value;

    value = DMX_READ_REG(INT_OQ_CHN_0(offset));

    return (value & (1 << bit)) ? HI_TRUE : HI_FALSE;
}

HI_VOID DmxHalOQClearOverflowInt(HI_U32 OQId)
{
    HI_U32  offset  = OQId >> 5;
    HI_U32  bit     = OQId & 0x1f;
    HI_U32  value;

    value = 1 << bit;
    DMX_WRITE_REG(RAW_OQ_CHN_0(offset), value);
}

/***********************************************************************************
* Function      :  DmxHalOQEnableOverflowInt
* Description   :    Set fix int enale
* Input         : u32FQId
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalOQEnableOverflowInt(HI_U32 OQId)
{
    HI_U32  offset  = OQId >> 5;
    HI_U32  bit     = OQId & 0x1f;
    HI_U32  value;

    value  = DMX_READ_REG(ENA_OQ_CHN_0(offset));

    value |= 1 << bit;
    DMX_WRITE_REG(ENA_OQ_CHN_0(offset), value);
}

/***********************************************************************************
* Function      :  DmxHalOQDisableOverflowInt
* Description   :    Set fix int enale
* Input         : u32OQId
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalOQDisableOverflowInt(HI_U32 OQId)
{
    HI_U32  offset  = OQId >> 5;
    HI_U32  bit     = OQId & 0x1f;
    HI_U32  value;

    value  = DMX_READ_REG(ENA_OQ_CHN_0(offset));

    value &= ~(1 << bit);
    DMX_WRITE_REG(ENA_OQ_CHN_0(offset), value);
}

HI_U32 DmxHalOQGetAllOutputIntStatus(HI_VOID)
{
    U_INT_STA_TYPE unIntSta;

    unIntSta.u32 = DMX_READ_REG(INT_STA_TYPE);

    return unIntSta.bits.iint_desc_o;
}

HI_U32 DmxHalOQGetOutputIntStatus(HI_U32 OqRegionId)
{
    return DMX_READ_REG(INT_OQ_DESC_0(OqRegionId));
}

HI_VOID DmxHalOQEnableOutputInt(HI_U32 OQId, HI_BOOL Enable)
{
    HI_U32  offset  = OQId >> 5;
    HI_U32  bit     = OQId & 0x1f;
    HI_U32  value;

    // clear raw int
    value = 1 << bit;
    DMX_WRITE_REG(RAW_OQ_DESC_0(offset), value);

    value = DMX_READ_REG(ENA_OQ_DESC_0(offset));

    if (Enable)
    {
        value |= 1 << bit;
    }
    else
    {
        value &= ~(1 << bit);
    }
    DMX_WRITE_REG(ENA_OQ_DESC_0(offset), value);
}

/***********************************************************************************
* Function      :  DmxHalEnableFQOvflErrInt
* Description   :    Set fix int enale
* Input         : u32FQId
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalEnableFQOvflErrInt(HI_VOID)
{
    U_ENA_INT_TYPE unEnaErr;

    unEnaErr.u32 = DMX_READ_REG(ENA_INT_TYPE);
    unEnaErr.bits.iena_fq_ovfl_err = 1;
    DMX_WRITE_REG(ENA_INT_TYPE, unEnaErr.u32);
}

/***********************************************************************************
* Function      :  DmxHalDisableFQOvflErrInt
* Description   :    Set fix int enale
* Input         : u32FQId
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalDisableFQOvflErrInt(HI_VOID)
{
    U_ENA_INT_TYPE unEnaErr;

    unEnaErr.u32 = DMX_READ_REG(ENA_INT_TYPE);
    unEnaErr.bits.iena_fq_ovfl_err = 0;
    DMX_WRITE_REG(ENA_INT_TYPE, unEnaErr.u32);
}

/***********************************************************************************
* Function      :  DmxHalEnableFQOvflErrInt
* Description   :    Set fix int enale
* Input         : u32FQId
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalEnableOQOvflErrInt(HI_VOID)
{
    U_ENA_INT_TYPE unEnaErr;

    unEnaErr.u32 = DMX_READ_REG(ENA_INT_TYPE);
    unEnaErr.bits.iena_oq_ovfl_err = 1;
    DMX_WRITE_REG(ENA_INT_TYPE, unEnaErr.u32);
}

/***********************************************************************************
* Function      :  DmxHalDisableOQOvflErrInt
* Description   :    Set fix int enale
* Input         : u32FQId
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalDisableOQOvflErrInt(HI_VOID)
{
    U_ENA_INT_TYPE unEnaErr;

    unEnaErr.u32 = DMX_READ_REG(ENA_INT_TYPE);
    unEnaErr.bits.iena_oq_ovfl_err = 0;
    DMX_WRITE_REG(ENA_INT_TYPE, unEnaErr.u32);
}

/***********************************************************************************
* Function      :  DmxHalEnableFQCfgErrInt
* Description   :    Set fix int enale
* Input         : u32FQId
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalEnableFQCfgErrInt(HI_VOID)
{
    U_ENA_INT_TYPE unEnaErr;

    unEnaErr.u32 = DMX_READ_REG(ENA_INT_TYPE);
    unEnaErr.bits.iena_fq_cfg_err = 1;
    DMX_WRITE_REG(ENA_INT_TYPE, unEnaErr.u32);
}

/***********************************************************************************
* Function      :  DmxHalDisableFQCfgErrInt
* Description   :    Set fix int enale
* Input         : u32FQId
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalDisableFQCfgErrInt(HI_VOID)
{
    U_ENA_INT_TYPE unEnaErr;

    unEnaErr.u32 = DMX_READ_REG(ENA_INT_TYPE);
    unEnaErr.bits.iena_fq_cfg_err = 0;
    DMX_WRITE_REG(ENA_INT_TYPE, unEnaErr.u32);
}

/***********************************************************************************
* Function      :  DmxHalEnableFQDescErrInt
* Description   :    Set fix int enale
* Input         : u32FQId
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalEnableFQDescErrInt(HI_VOID)
{
    U_ENA_INT_TYPE unEnaErr;

    unEnaErr.u32 = DMX_READ_REG(ENA_INT_TYPE);
    unEnaErr.bits.iena_fq_desc_err = 1;
    DMX_WRITE_REG(ENA_INT_TYPE, unEnaErr.u32);
}

/***********************************************************************************
* Function      :  DmxHalDisableFQDescErrInt
* Description   :    Set fix int enale
* Input         : u32FQId
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalDisableFQDescErrInt(HI_VOID)
{
    U_ENA_INT_TYPE unEnaErr;

    unEnaErr.u32 = DMX_READ_REG(ENA_INT_TYPE);
    unEnaErr.bits.iena_fq_desc_err = 0;
    DMX_WRITE_REG(ENA_INT_TYPE, unEnaErr.u32);
}

/***********************************************************************************
* Function      :  DmxHalEnableAllDavInt
* Description   :    Enable All  Int
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalEnableAllDavInt(HI_VOID)
{
    U_ENA_INT_ALL ena_all_int;

    ena_all_int.u32 = DMX_READ_REG(ENA_INT_ALL);
    ena_all_int.bits.iena_all = 1;
    DMX_WRITE_REG(ENA_INT_ALL, ena_all_int.u32);
}

/***********************************************************************************
* Function      :  DmxHalEnableOQRecive
* Description   :    Set fix int enale
* Input         : u32OQId
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalEnableOQRecive(HI_U32 OQId)
{
    HI_U32  offset  = OQId >> 5;
    HI_U32  bit     = OQId & 0x1f;
    HI_U32  value;
    if( bSemphoreInit == HI_FALSE)
    {
        DmxHalSemaphoreInit();
        bSemphoreInit = HI_TRUE;
    }
    if (0 == down_interruptible(&HALLockOQ))
    {
        value  = DMX_READ_REG(OQ_ENA_0(offset));
        value |= 1 << bit;
        DMX_WRITE_REG(OQ_ENA_0(offset), value);
        up(&HALLockOQ);
    }
}

/***********************************************************************************
* Function      :  DmxHalDisableOQRecive
* Description   :    Set fix int enale
* Input         : u32OQId
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalDisableOQRecive(HI_U32 OQId)
{
    HI_U32  offset  = OQId >> 5;
    HI_U32  bit     = OQId & 0x1f;
    HI_U32  value;
    if( bSemphoreInit == HI_FALSE)
    {
        DmxHalSemaphoreInit();
        bSemphoreInit = HI_TRUE;
    }
    if (0 == down_interruptible(&HALLockOQ))
    {
        value  = DMX_READ_REG(OQ_ENA_0(offset));
        value &= ~(1 << bit);
        DMX_WRITE_REG(OQ_ENA_0(offset), value);
        up(&HALLockOQ);
    }
}

HI_BOOL DmxHalGetOQEnableStatus(HI_U32 OQId)
{
    HI_U32  offset  = OQId >> 5;
    HI_U32  bit     = OQId & 0x1f;
    HI_U32  value;

    value  = DMX_READ_REG(OQ_ENA_0(offset));
    value &= 1 << bit;
    if (value)
    {
        return HI_TRUE;
    }
    else
    {
        return HI_FALSE;
    }
}

/***********************************************************************************
* Function      :  DmxHalFQEnableRecive
* Description   :    Set fix int enale
* Input         : u32FQId
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalFQEnableRecive(HI_U32 FQId, HI_BOOL Enable)
{
    HI_U32  offset  = FQId >> 5;
    HI_U32  bit     = FQId & 0x1f;
    HI_U32  value;

    if( bSemphoreInit == HI_FALSE)
    {
        DmxHalSemaphoreInit();
        bSemphoreInit = HI_TRUE;
    }
    if (0 == down_interruptible(&HALLockFQ))
    {
        value = DMX_READ_REG(FQ_ENA_0(offset));

        if (Enable)
        {
            value |= 1 << bit;
        }
        else
        {
            value &= ~(1 << bit);
        }
        DMX_WRITE_REG(FQ_ENA_0(offset), value);
        up(&HALLockFQ);
    }
}

HI_S32 DmxHalGetInitStatus(HI_VOID)
{
    HI_S32 ret      = HI_FAILURE;
    HI_U32 FqStatus = DMX_READ_REG(FQ_INIT_DONE) & DMX_MASK_BIT_0;
    HI_U32 OqStatus = DMX_READ_REG(OQ_INIT_DONE) & DMX_MASK_BIT_0;

    if (!FqStatus && !OqStatus)
    {
        ret = HI_SUCCESS;
    }

    return ret;
}

/***********************************************************************************
* Function      :  DmxHalSetFQWORDx
* Description   :    Set fix int enale
* Input         : u32FQId,u32Data
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalSetFQWORDx(HI_U32 FQId, HI_U32 Offset, HI_U32 Value)
{
    wmb();/*sync the DDR*/
    switch (Offset)
    {
        case DMX_FQ_CTRL_OFFSET:
        {
            DMX_WRITE_REG(ADDR_FQ_WORD0(FQId), Value);
            break;
        }

        case DMX_FQ_RDVD_OFFSET:
        {
            DMX_WRITE_REG(ADDR_FQ_WORD1(FQId), Value);
            break;
        }

        case DMX_FQ_SZWR_OFFSET:
        {
            DMX_WRITE_REG(ADDR_FQ_WORD2(FQId), Value);
            break;
        }

        case DMX_FQ_START_OFFSET:
        {
            DMX_WRITE_REG(ADDR_FQ_WORD3(FQId), Value);
            break;
        }

        default:
        {
            break;
        }
    }
}

/***********************************************************************************
* Function      :  DmxHalGetFQWORDx
* Description   :    Set fix int enale
* Input         : u32FQId,u32Data
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalGetFQWORDx(HI_U32 FQId, HI_U32 Offset, HI_U32 *pu32Data)
{
    switch (Offset)
    {
        case DMX_FQ_CTRL_OFFSET:
            *pu32Data = DMX_READ_REG(ADDR_FQ_WORD0(FQId));
            break;

        case DMX_FQ_RDVD_OFFSET:
            *pu32Data = DMX_READ_REG(ADDR_FQ_WORD1(FQId));
            break;

        case DMX_FQ_SZWR_OFFSET:
            *pu32Data = DMX_READ_REG(ADDR_FQ_WORD2(FQId));
            break;

        case DMX_FQ_START_OFFSET:
            *pu32Data = DMX_READ_REG(ADDR_FQ_WORD3(FQId));
            break;

        default:
        {
            break;
        }
    }
}

/***********************************************************************************
* Function      :  DmxHalSetFQWritePtr
* Description   :    Set fix int enale
* Input         : u32FQId,u32Data
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalSetFQWritePtr(HI_U32 FQId, HI_U32 WritePtr)
{
    wmb();/*sync the DDR*/
    DMX_WRITE_REG(ADDR_FQ_WORD2(FQId), WritePtr);
}

/***********************************************************************************
* Function      : DmxHalGetFQWritePtr
* Description   : Set fix int enale
* Input         : FQId
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_U32 DmxHalGetFQWritePtr(HI_U32 FQId)
{
    return DMX_READ_REG(ADDR_FQ_WORD2(FQId)) & 0xffff;
}

/***********************************************************************************
* Function      : DmxHalGetFQReadPtr
* Description   : Set fix int enale
* Input         : FQId
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_U32 DmxHalGetFQReadPtr(HI_U32 FQId)
{
    return DMX_READ_REG(ADDR_FQ_WORD1(FQId)) & 0xffff;
}

/***********************************************************************************
* Function      :  DmxHalSetOQWORDx
* Description   :    Set fix int enale
* Input         : u32FQId,u32Data
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalSetOQWORDx(HI_U32 OQId, HI_U32 Offset, HI_U32 Value)
{
    wmb();/*sync the DDR*/
    switch (Offset)
    {
        case DMX_OQ_RSV_OFFSET:
            DMX_WRITE_REG(ADDR_OQ_WORD0(OQId), Value);
            break;

        case DMX_OQ_CTRL_OFFSET:
            DMX_WRITE_REG(ADDR_OQ_WORD1(OQId), Value);
            break;

        case DMX_OQ_EOPWR_OFFSET:
            DMX_WRITE_REG(ADDR_OQ_WORD2(OQId), Value);
            break;

        case DMX_OQ_SZUS_OFFSET:
            DMX_WRITE_REG(ADDR_OQ_WORD3(OQId), Value);
            break;

        case DMX_OQ_SADDR_OFFSET:
            DMX_WRITE_REG(ADDR_OQ_WORD4(OQId), Value);
            break;

        case DMX_OQ_RDWR_OFFSET:
            DMX_WRITE_REG(ADDR_OQ_WORD5(OQId), Value);
            break;

        case DMX_OQ_CFG_OFFSET:
            DMX_WRITE_REG(ADDR_OQ_WORD6(OQId), Value);
            break;

        case DMX_OQ_START_OFFSET:
            DMX_WRITE_REG(ADDR_OQ_WORD7(OQId), Value);
            break;

        default:
            break;
    }
}

/***********************************************************************************
* Function      :  DmxHalGetOQWORDx
* Description   :    Set fix int enale
* Input         : OQId,u32Data
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalGetOQWORDx(HI_U32 OQId, HI_U32 Offset, HI_U32 *Value)
{
    switch (Offset)
    {
        case DMX_OQ_RSV_OFFSET:
            *Value = DMX_READ_REG(ADDR_OQ_WORD0(OQId));
            break;

        case DMX_OQ_CTRL_OFFSET:
            *Value = DMX_READ_REG(ADDR_OQ_WORD1(OQId));
            break;

        case DMX_OQ_EOPWR_OFFSET:
            *Value = DMX_READ_REG(ADDR_OQ_WORD2(OQId));
            break;

        case DMX_OQ_SZUS_OFFSET:
            *Value = DMX_READ_REG(ADDR_OQ_WORD3(OQId));
            break;

        case DMX_OQ_SADDR_OFFSET:
            *Value = DMX_READ_REG(ADDR_OQ_WORD4(OQId));
            break;

        case DMX_OQ_RDWR_OFFSET:
            *Value = DMX_READ_REG(ADDR_OQ_WORD5(OQId));
            break;

        case DMX_OQ_CFG_OFFSET:
            *Value = DMX_READ_REG(ADDR_OQ_WORD6(OQId));
            break;

        case DMX_OQ_START_OFFSET:
            *Value = DMX_READ_REG(ADDR_OQ_WORD7(OQId));
            break;

        default:
            break;
    }
}

//set oq description word mask bit, set bit to 1 for the needless bit
HI_VOID DxmHalSetOQRegMask(HI_U32 u32MaskValue)
{
    DMX_WRITE_REG(OQ_WR_MASK, u32MaskValue & 0xf);
}

HI_U32 DxmHalGetOQRegMask(HI_VOID)
{
    return DMX_READ_REG(OQ_WR_MASK);
}

/***********************************************************************************
* Function      :  DmxHalSetOQReadPtr
* Description   :    Set fix int enale
* Input         : u32OQId,u32ReadPtr
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalSetOQReadPtr(HI_U32 OQId, HI_U32 ReadPtr)
{
    HI_U32 u32MaskValue;

    //wmb();/*sync the DDR*/

    ReadPtr = (ReadPtr & DMX_OQ_DEPTH) << 16;

    if (DmxHalGetOQEnableStatus(OQId) == HI_FALSE)
    {
        u32MaskValue = DxmHalGetOQRegMask();
        DxmHalSetOQRegMask(0x3); //shield the needless bits, prevent from effecting other bits
        DMX_WRITE_REG(ADDR_OQ_WORD5(OQId), ReadPtr);
        DxmHalSetOQRegMask(u32MaskValue);
    }
    else
    {
        DMX_WRITE_REG(ADDR_OQ_WORD5(OQId), ReadPtr);
    }
}

/***********************************************************************************
* Function      :  DmxHalAttachIPBPFQ
* Description   :  Set Ip Back Push OQ and enable
* Input         : u32IPNum,BufferId
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalAttachIPBPFQ(HI_U32 PortId, HI_U32 FQId)
{
    HI_U32  offset  = FQId >> 5;
    HI_U32  bit     = FQId & 0x1f;
    HI_U32  value;

    value = DMX_READ_REG(IP_BP_FQ_CFG(PortId, offset));

    value |= 1 << bit;
    DMX_WRITE_REG(IP_BP_FQ_CFG(PortId, offset), value);
}

/***********************************************************************************
* Function      :  DmxHalDetachIPBPFQ
* Description   :  Set Ip Back Push OQ and enable
* Input         : u32IPNum,BufferId
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalDetachIPBPFQ(HI_U32 PortId, HI_U32 FQId)
{
    HI_U32  offset  = FQId >> 5;
    HI_U32  bit     = FQId & 0x1f;
    HI_U32  value;

    value = DMX_READ_REG(IP_BP_FQ_CFG(PortId, offset));

    value &= ~(1 << bit);
    DMX_WRITE_REG(IP_BP_FQ_CFG(PortId, offset), value);
}

/***********************************************************************************
* Function      :  DmxHalSetRecTsCounter
* Description   :  Set Record Ts Counter and enable
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalSetRecTsCounter(HI_U32 u32DmxId, HI_U32 u32OqId)
{
    U_REC_TSCNT_CFG_0 unTsCntCfg0;
    U_REC_TSCNT_CFG_1 unTsCntCfg1;
    HI_U32 u32TsCounter;

    u32TsCounter = u32DmxId;

    unTsCntCfg0.u32 = DMX_READ_REG(REC_TSCNT_CFG_0);
    unTsCntCfg1.u32 = DMX_READ_REG(REC_TSCNT_CFG_1);
    switch (u32TsCounter)
    {
    case 0:
        unTsCntCfg0.bits.tscnt0_oqid = u32OqId & 0x7f;
        unTsCntCfg0.bits.tscnt0_ena = 1;
        break;
    case 1:
        unTsCntCfg0.bits.tscnt1_oqid = u32OqId & 0x7f;
        unTsCntCfg0.bits.tscnt1_ena = 1;
        break;
    case 2:
        unTsCntCfg0.bits.tscnt2_oqid = u32OqId & 0x7f;
        unTsCntCfg0.bits.tscnt2_ena = 1;
        break;
    case 3:
        unTsCntCfg0.bits.tscnt3_oqid = u32OqId & 0x7f;
        unTsCntCfg0.bits.tscnt3_ena = 1;
        break;
    case 4:
        unTsCntCfg1.bits.tscnt4_oqid = u32OqId & 0x7f;
        unTsCntCfg1.bits.tscnt4_ena = 1;
        break;
    case 5:
        unTsCntCfg1.bits.tscnt5_oqid = u32OqId & 0x7f;
        unTsCntCfg1.bits.tscnt5_ena = 1;
        break;
    default:
        return;
    }

    DMX_WRITE_REG(REC_TSCNT_CFG_0, unTsCntCfg0.u32);
    DMX_WRITE_REG(REC_TSCNT_CFG_1, unTsCntCfg1.u32);
}

/***********************************************************************************
* Function      :  DmxHalSetRecTsCntReplace
* Description   :  Set Record Ts Counter replace
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalSetRecTsCntReplace(HI_U32 u32DmxId)
{
    U_SCD_TSCNT_ENA unTsCntRpl;
    HI_U32 u32TsCounter;

    u32TsCounter = u32DmxId;

    unTsCntRpl.u32 = DMX_READ_REG(SCD_TSCNT_ENA);
    switch (u32TsCounter)
    {
    case 0:
        unTsCntRpl.bits.tscnt0_rep_ena = 1;
        break;
    case 1:
        unTsCntRpl.bits.tscnt1_rep_ena = 1;
        break;
    case 2:
        unTsCntRpl.bits.tscnt2_rep_ena = 1;
        break;
    case 3:
        unTsCntRpl.bits.tscnt3_rep_ena = 1;
        break;
    case 4:
        unTsCntRpl.bits.tscnt4_rep_ena = 1;
        break;
    case 5:
        unTsCntRpl.bits.tscnt5_rep_ena = 1;
        break;
    default:
        return;
    }

    DMX_WRITE_REG(SCD_TSCNT_ENA, unTsCntRpl.u32);
}

/***********************************************************************************
* Function      :  DmxHalResetOqCounter
* Description   :  Get Oq Counter
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalResetOqCounter(HI_U32 OQId)
{
    DMX_WRITE_REG(ADDR_INT_CNT(OQId), 0);
}

/***********************************************************************************
* Function      :  DmxHalGetChannelCounter
* Description   :  Get Oq Counter
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_U32 DmxHalGetChannelCounter(HI_U32 ChanId)
{
    return DMX_READ_REG(CHANNEL_TS_COUNT(ChanId));
}

/***********************************************************************************
* Function      :  DmxHalResetOqCounter
* Description   :  Get Oq Counter
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalResetChannelCounter(HI_U32 ChanId)
{
    DMX_WRITE_REG(CHANNEL_TS_COUNT(ChanId), 0);
}

/***********************************************************************************
* Function      :  DmxHalFilterSetSecStuffCtrl
* Description   :
* Input         :  Enable: 1 - receive the stuff not 0xff
*                          0 - do not receive the stuff not 0xff
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalFilterSetSecStuffCtrl(HI_BOOL Enable)
{
    U_DMX_FILTER_CTRL flt_ctrl;

    flt_ctrl.value = DMX_READ_REG(DMX_FILTER_CTRL);
    if (Enable)
    {
        flt_ctrl.bits.sec_stuff_nopusi_e = 1;
    }
    else
    {
        flt_ctrl.bits.sec_stuff_nopusi_e = 0;
    }

    DMX_WRITE_REG(DMX_FILTER_CTRL, flt_ctrl.value);
    DMX_COM_EQUAL(flt_ctrl.value, DMX_READ_REG(DMX_FILTER_CTRL));
}

/***********************************************************************************
* Function      :  DmxHalSetTei
* Description   :
* Input         :
*
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalSetTei(HI_U32   u32DemuxID,HI_BOOL bCheckTei)
{
    U_DMX_GLB_CTRL1 glb_cfg1;

    glb_cfg1.value = DMX_READ_REG(DMX_GLB_CTRL1(u32DemuxID));/*(0x3B00 + ((DmxId) << 4))*/
    if (bCheckTei)
    {
        glb_cfg1.bits.dmx_tei_ctrl = 1;
    }
    else
    {
        glb_cfg1.bits.dmx_tei_ctrl = 0;
    }
    DMX_WRITE_REG(DMX_GLB_CTRL1(u32DemuxID), glb_cfg1.value);
    DMX_COM_EQUAL(glb_cfg1.value, DMX_READ_REG(DMX_GLB_CTRL1(u32DemuxID)));
}

#ifdef DMX_FILTER_DEPTH_SUPPORT
/***********************************************************************************
* Function      : DmxHalFilterEnableDepth
* Description   : enable filter depth
* Input         :
* Output        :
* Return        :
* Others:       :
***********************************************************************************/
HI_VOID DmxHalFilterEnableDepth(HI_VOID)
{
    U_DMX_FILTER_CTRL flt_ctrl;

    flt_ctrl.value = DMX_READ_REG(DMX_FILTER_CTRL);

    flt_ctrl.bits.minlen_discard_by_flt = 1;

    DMX_WRITE_REG(DMX_FILTER_CTRL, flt_ctrl.value);

    DMX_COM_EQUAL(flt_ctrl.value, DMX_READ_REG(DMX_FILTER_CTRL));
}

/***********************************************************************************
* Function      : DmxHalFilterSetDepth
* Description   : Set filter depth
* Input         : FilterId, Depth
* Output        :
* Return        :
* Others:       :
***********************************************************************************/
HI_VOID DmxHalFilterSetDepth(HI_U32 FilterId, HI_U32 Depth)
{
    U_DMX_FILTER_NUM flt_num;

    flt_num.value = DMX_READ_REG(DMX_FLT_NUM(FilterId));

    flt_num.bits.flt_depth = Depth;

    DMX_WRITE_REG(DMX_FLT_NUM(FilterId), flt_num.value);

    DMX_COM_EQUAL(flt_num.value, DMX_READ_REG(DMX_FLT_NUM(FilterId)));
}
#endif

#ifdef DMX_REC_EXCLUDE_PID_SUPPORT
/***********************************************************************************
* Function      : DmxHalEnableAllRecExcludePid
* Description   : Enable all rec exclude pid function
* Input         :
* Output        :
* Return        :
* Others:       :
***********************************************************************************/
HI_VOID DmxHalEnableAllRecExcludePid(HI_U32 DmxID)
{
    U_DMX_GLB_CTRL1 glb_cfg1;

    glb_cfg1.value = DMX_READ_REG(DMX_GLB_CTRL1(DmxID));
    glb_cfg1.bits.dmx_allrec_neg_en = 1;
    DMX_WRITE_REG(DMX_GLB_CTRL1(DmxID), glb_cfg1.value);
    DMX_COM_EQUAL(glb_cfg1.value, DMX_READ_REG(DMX_GLB_CTRL1(DmxID)));
}

/***********************************************************************************
* Function      : DmxHalDisableAllRecExcludePid
* Description   : Disable all rec exclude pid function
* Input         :
* Output        :
* Return        :
* Others:       :
***********************************************************************************/
HI_VOID DmxHalDisableAllRecExcludePid(HI_U32 DmxID)
{
    U_DMX_GLB_CTRL1 glb_cfg1;

    glb_cfg1.value = DMX_READ_REG(DMX_GLB_CTRL1(DmxID));
    glb_cfg1.bits.dmx_allrec_neg_en = 0;
    DMX_WRITE_REG(DMX_GLB_CTRL1(DmxID), glb_cfg1.value);
    DMX_COM_EQUAL(glb_cfg1.value, DMX_READ_REG(DMX_GLB_CTRL1(DmxID)));
}

/***********************************************************************************
* Function      : DmxHalGetAllRecExcludePid
* Description   : Get all rec exclude pid
* Input         :
* Output        :
* Return        :
* Others:       :
***********************************************************************************/
HI_VOID DmxHalGetAllRecExcludePid(HI_U32 RecCfgID, HI_U32* DmxID, HI_U32* PID)
{
    U_STA_ALLREC_CFG allrec_cfg;

    allrec_cfg.value = DMX_READ_REG(ALLREC_CFG0_29(RecCfgID));

    *DmxID = allrec_cfg.bits.recdel_pid_dmxid;
    *PID   = allrec_cfg.bits.recdel_pid_value;
}

/***********************************************************************************
* Function      : DmxHalSetAllRecExcludePid
* Description   : Set filter depth
* Input         : FilterId, Depth
* Output        :
* Return        :
* Others:       :
***********************************************************************************/
HI_VOID DmxHalSetAllRecExcludePid(HI_U32 RecCfgID, HI_U32 DmxID, HI_U32 PID)
{
    U_STA_ALLREC_CFG allrec_cfg;

    allrec_cfg.value = DMX_READ_REG(ALLREC_CFG0_29(RecCfgID));

    allrec_cfg.bits.recdel_pid_dmxid = DmxID;
    allrec_cfg.bits.recdel_pid_value = PID;

    DMX_WRITE_REG(ALLREC_CFG0_29(RecCfgID), allrec_cfg.value);

    DMX_COM_EQUAL(allrec_cfg.value, DMX_READ_REG(ALLREC_CFG0_29(RecCfgID)));
}

#endif

#ifdef DMX_REC_TIME_STAMP_SUPPORT    /*only hi3719 support this */

/***********************************************************************************
* Function      : DmxHalConfigRecTsTimeStamp
* Description   : Config Record Ts time stamp
* Input         : DmxID,enRecTimeStamp
* Output        :
* Return        :
* Others:
***********************************************************************************/
HI_VOID DmxHalConfigRecTsTimeStamp(HI_U32 DmxID, DMX_REC_TIMESTAMP_MODE_E enRecTimeStamp)
{
    U_TIMESTAMP_CTRL timestamp_ctrl;
    timestamp_ctrl.value = DMX_READ_REG(TIMESTAMP_CTRL);
    timestamp_ctrl.value |= ((enRecTimeStamp << DmxID) && (0x3fff));
    DMX_WRITE_REG(TIMESTAMP_CTRL, timestamp_ctrl.value);
}
#endif


