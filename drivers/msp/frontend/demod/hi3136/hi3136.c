/***********************************************************************************
*              Copyright 2012 - 2080, Hisilicon Tech. Co., Ltd.
*                           ALL RIGHTS RESERVED
* FileName: hi3136.c
* Description:
*
* History:
* Version   Date             Author     DefectNum        Description
* main\1    2012-08-08   j207467    NULL                Create this file.
***********************************************************************************/

#include <linux/delay.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <linux/time.h>
#include <linux/module.h>
#include <asm/io.h>
#include <linux/kthread.h>

#include "hi_type.h"
#include "hi_drv_i2c.h"
#include "hi_drv_gpio.h"
#include "hi_debug.h"
#include "hi_error_mpi.h"
//#include " hi_unf_ecs_type.h"
#include "hi_drv_proc.h"
#include "hi_drv_sys.h"

//#include "common_proc.h"
//#include "tuner_common_pilot.h"
#include "drv_demod.h"
#include "av2011.h"
#include "sharp_qm1d1c004x.h"
#include "M88TS2022.h"
#include "RDA5815.h"
//#define TUNER_NUM 5

HI3136_CFG_ITEM_S hi3136_cfg[TUNER_NUM] = {0};
HI_U32 u32LPF_KHz[TUNER_NUM] = {0};
static HI_S32 hi3136_setlpf(HI_U32 u32TunerPort, HI_U32 u32SymbolRate/*unit BAUD ,neither K, nor M*/);

typedef enum tagHI3136_LOCK_STATUS_E
{
    UNLOCKED = 0,
    AGC_LOCKED,
    CBS_LOCKED,
    TR_LOCKED,
    SYNC_LOCKED,
    CR_LOCKED,
    FEC_LOCKED
} HI3136_LOCK_STATUS_E;

/*HI_U32 hi3136_I2C_Check(u32TunerPort)
{
    return HI_SUCCESS;
}*/

static HI_VOID hi3136_HotReset_CHIP(HI_U32 u32TunerPort)
{
    HI_U8 u8Temp = 0;

    qam_read_byte(u32TunerPort, RSTN_CTRL, &u8Temp);
    u8Temp &= 0xfd;
    qam_write_byte(u32TunerPort, RSTN_CTRL, u8Temp);
    u8Temp |= 0x02;
    qam_write_byte(u32TunerPort, RSTN_CTRL, u8Temp);
}

HI_VOID hi3136_config_i2c_out(HI_U32 u32TunerPort, HI_BOOL bTuner)
{
    if (HI_UNF_DEMOD_DEV_TYPE_3136I == g_stTunerOps[u32TunerPort].enDemodDevType)
    {
        return;
    }

    if (bTuner)
    {
        (HI_VOID)qam_write_bit(u32TunerPort, TUNER_SEL, 0, 1); /*set I2C_repeater_en 1*/
    }
}

static HI_VOID hi3136_config_IQ_swap(HI_U32 u32TunerPort, HI_BOOL bSwap)  /*TR module S2 IQ swap*/
{
    if (bSwap)
    {
        (HI_VOID)qam_write_bit(u32TunerPort, AGC_CTRL, 3, 1);
    }
    else
    {
        (HI_VOID)qam_write_bit(u32TunerPort, AGC_CTRL, 3, 0);
    }
}

HI_BOOL hi3136_Set_Ts_Out_Enable(HI_U32 u32TunerPort,HI_BOOL bTS_ON)
{
    //HI_U8 u8Temp;

    if(bTS_ON)
    {
        qam_write_bit(u32TunerPort, TS_CTRL0, 1, 1);
    }
    else
    {
        qam_write_bit(u32TunerPort, TS_CTRL0, 1, 0);
    }

    return HI_SUCCESS;
}

HI3136_LOCK_STATUS_E hi3136_Read_Lock_Status(HI_U32 u32TunerPort)  //设置状态寄存器，
{
    HI_U8 u8Temp = 0;
    HI3136_LOCK_STATUS_E lockstatus;
    HI_S32 s32Ret = 0;
    lockstatus = UNLOCKED;

    s32Ret = qam_read_byte(u32TunerPort, LOCK_FLAG, &u8Temp);
    if(HI_SUCCESS != s32Ret)
    {
        return s32Ret;
    }
    if (u8Temp & (1 << 5))
    {
        lockstatus = FEC_LOCKED;
    }
    else if (u8Temp & (1 << 4))
    {
        lockstatus = CR_LOCKED;
    }
    else if (u8Temp & (1 << 3))
    {
        lockstatus = SYNC_LOCKED;
    }
    else if (u8Temp & (1 << 2))
    {
        lockstatus = TR_LOCKED;
    }
    else if (u8Temp & (1 << 1))
    {
        lockstatus = CBS_LOCKED;
    }
    else if (u8Temp & 1)
    {
        lockstatus = AGC_LOCKED;
    }
    else
    {
        lockstatus = UNLOCKED;
    }

    return lockstatus;
}

#if 0
HI_S32 hi3136_get_signal_strength(HI_U32 u32TunerPort, HI_U32 *pu32SignalStrength)
{
    HI_U8 au8RegVal[2] = {0};

    HI_TUNER_CHECKPOINTER( pu32SignalStrength);

    qam_read_byte(u32TunerPort, AGC_CTRL_L, &au8RegVal[1]);
    qam_read_byte(u32TunerPort, AGC_CTRL_H, &au8RegVal[0]);

    pu32SignalStrength[1] = ((au8RegVal[0] & 0x0f) << 8) + au8RegVal[1];

    return HI_SUCCESS;
}
#endif

HI_VOID hi3136_read_tuner_reg(HI_U32 u32TunerPort, HI_U8 RegAddress, HI_U8 *RegData)
{
    //TODO: read  tuner register
    HI_U8 enI2cChannel = 0;
    HI_U8 u8DevAddr = 0;
    HI_S32 s32Ret = 0;
    u8DevAddr = g_stTunerOps[u32TunerPort].u32TunerAddress;
    enI2cChannel = g_stTunerOps[u32TunerPort].enI2cChannel;

    hi3136_config_i2c_out(u32TunerPort, 1);

    if (4 >= enI2cChannel)
    {
        s32Ret = HI_DRV_I2C_Read(enI2cChannel, u8DevAddr, RegAddress, 1, RegData, 1);
	 if(HI_SUCCESS != s32Ret)
        {
            return;
        } 
    }
}

HI_S32 hi3136_get_signal_strength(HI_U32 u32TunerPort, HI_U32 *pu32SignalStrength)
{
    HI_U8 au8RegVal[2] = {0};
    HI_U8 u8BBGain = 0, u8RFGain = 0;

    HI_TUNER_CHECKPOINTER( pu32SignalStrength);

    qam_read_byte(u32TunerPort, AGC_CTRL_L, &au8RegVal[0]);
    qam_read_byte(u32TunerPort, AGC_CTRL_H, &au8RegVal[1]);

    if (au8RegVal[1] >> 7)
    {
        pu32SignalStrength[1] = ((au8RegVal[1] & 0x0f) << 8) + au8RegVal[0];
    }
    else
    {
        HI_INFO_TUNER("AGC not locked!\n");
        return HI_FAILURE;
    }

    if (HI_UNF_TUNER_DEV_TYPE_M88TS2022 == g_stTunerOps[u32TunerPort].enTunerDevType)
    {
        hi3136_read_tuner_reg(u32TunerPort, 0x21, &u8BBGain);
        hi3136_read_tuner_reg(u32TunerPort, 0x3d, &u8RFGain);
        pu32SignalStrength[0] = u8BBGain;
        pu32SignalStrength[2] = u8RFGain;
    }

    return HI_SUCCESS;
}

HI_S32 hi3136_get_snr(HI_U32 u32TunerPort, HI_U32* pu32SNR)
{
    HI_U8 au8RegVal[2] = {0};
    HI_U32 u32SNR = 0;
    HI_U8 u8Times = 0;

    HI_TUNER_CHECKPOINTER( pu32SNR);

    do
    {
        qam_read_byte(u32TunerPort, NOISE_POW_CPU_H, &au8RegVal[0]);
        u8Times++;
    } while(!(au8RegVal[0] & (1 << 7)) && (u8Times < 10));

    if (!(au8RegVal[0] & (1 << 7)) && (10 == u8Times))
    {
        return HI_FAILURE;
    }
    qam_read_byte(u32TunerPort, NOISE_POW_CPU_L, &au8RegVal[1]);
    u32SNR = ((au8RegVal[0] & 0x7f) << 8) + au8RegVal[1];

    *pu32SNR = u32SNR;

    return HI_SUCCESS;
}

HI_S32 hi3136_get_ber(HI_U32 u32TunerPort, HI_U32 *pu32ber)
{
    HI_U8   u8Temp = 0;
    HI_U8   temp = 0;
    HI_U8   temp1 = 0;
    HI_U8   temp2 = 0;
    HI_U32  ErrorRate = 0;
    HI_U32  a = 0;
    HI_U32  b = 0;
    HI_U32  total_num, temp3;

    if(pu32ber == HI_NULL)
    {
        return HI_FAILURE;
    }
    if (hi3136_Read_Lock_Status(u32TunerPort) >= SYNC_LOCKED)
    {
        qam_read_byte(u32TunerPort, SEAR_RESULT, &u8Temp);
        temp = (u8Temp & 0x01);     //DVBS OR DIRECT TV
        qam_read_byte(u32TunerPort, LOCK_FLAG, &u8Temp);
        if(u8Temp & (1 << 3))
        {
            temp = 2;     //sync_ok
        }

        switch(temp)
        {
        case 0:
            {   //DVBS

                qam_read_byte(u32TunerPort, BER_CTRL, &u8Temp);
                temp1 = (u8Temp & 0x70) >> 4;
                switch(temp1)
                {
                case 0 :
                    a = 0x10;
                    break;
                case 1 :
                    a = 0x40;
                    break;
                case 2 :
                    a = 0x100;
                    break;
                case 3 :
                    a = 0x400;
                    break;
                case 4 :
                    a = 0x1000;
                    break;
                case 5 :
                    a = 0x4000;
                    break;
                case 6 :
                    a = 0x10000;
                    break;
                default:
                    a = 0x40000;
                }
                total_num = a * 204 * 8;
                qam_read_byte(u32TunerPort, FEC_BER_L, &temp2);
                qam_read_byte(u32TunerPort, FEC_BER_H, &temp1);

                temp3 = (temp1 << 8) + temp2;
                ErrorRate = temp3 / total_num * 1000000000;
            }
            break;

        case 1:
            {   //DIRECT TV
                qam_read_byte(u32TunerPort, BER_CTRL, &u8Temp);
                temp1 = (u8Temp & 0x70) >> 4;
                switch(temp1)
                {
                case 0 :
                    a = 0x10;
                    break;
                case 1 :
                    a = 0x40;
                    break;
                case 2 :
                    a = 0x100;
                    break;
                case 3 :
                    a = 0x400;
                    break;
                case 4 :
                    a = 0x1000;
                    break;
                case 5 :
                    a = 0x4000;
                    break;
                case 6 :
                    a = 0x10000;
                    break;
                default:
                    a = 0x40000;
                }
                total_num = a * 146 * 8;
                qam_read_byte(u32TunerPort, FEC_BER_L, &temp2);
                qam_read_byte(u32TunerPort, FEC_BER_H, &temp1);
                temp3 = (temp1 << 8) + temp2;
                ErrorRate = temp3 / total_num * 1000000000;
            }
            break;

        case 2:
            {   //DVBS2
                qam_read_byte(u32TunerPort, CR_PLS_CODE, &u8Temp);
                temp = ((u8Temp & 0x7c) >> 2);
                temp1 = ((u8Temp & 0x02) >> 1);
                if(temp1 == 0) /*long frame*/
                {
                    switch(temp)
                    {
                    case 1 :
                        b = 16200;
                        break;
                    case 2 :
                        b = 21600;
                        break;
                    case 3 :
                        b = 25920;
                        break;
                    case 4 :
                        b = 32400;
                        break;
                    case 5 :
                    case 12:
                        b = 38880;
                        break;
                    case 6 :
                    case 13:
                    case 18:
                        b = 43200;
                        break;
                    case 7 :
                    case 14:
                    case 19:
                    case 24:
                        b = 48600;
                        break;
                    case 8 :
                    case 20:
                    case 25:
                        b = 51840;
                        break;
                    case 9 :
                    case 15:
                    case 21:
                    case 26:
                        b = 54000;
                        break;
                    case 10:
                    case 16:
                    case 22:
                    case 27:
                        b = 57600;
                        break;
                    case 11:
                    case 17:
                    case 23:
                    case 28:
                        b = 58320;
                        break;
                    default:
                        b = 583200;
                    }
                }
                else  /*short frame*/
                {
                    switch (temp)
                    {
                    case 1 :
                        b = 3240;
                        break;
                    case 2 :
                        b = 5400;
                        break;
                    case 3 :
                        b = 6480;
                        break;
                    case 4 :
                        b = 7200;
                        break;
                    case 5 :
                    case 12:
                        b = 9720;
                        break;
                    case 6 :
                    case 13:
                    case 18:
                        b = 10800;
                        break;
                    case 7 :
                    case 14:
                    case 19:
                    case 24:
                        b = 11880;
                        break;
                    case 8 :
                    case 20:
                    case 25:
                        b = 12600;
                        break;
                    case 9 :
                    case 15:
                    case 21:
                    case 26:
                        b = 13320;
                        break;
                    case 10:
                    case 16:
                    case 22:
                    case 27:
                        b = 14400;
                        break;
                    default:
                        b = 14400;
                    }
                }

                qam_read_byte(u32TunerPort, BER_CTRL, &u8Temp);
                temp1 = (u8Temp & 0x70) >> 4;
                switch(temp1)
                {
                case 0 :
                    a = 0x4;
                    break;
                case 1 :
                    a = 0x10;
                    break;
                case 2 :
                    a = 0x80;
                    break;
                case 3 :
                    a = 0x100;
                    break;
                case 4 :
                    a = 0x800;
                    break;
                case 5 :
                    a = 0x1000;
                    break;
                case 6 :
                    a = 0x8000;
                    break;
                default:
                    a = 0xFFFF;
                    break;
                }
                total_num = a * b;
                qam_read_byte(u32TunerPort, FEC_BER_L, &temp2);
                qam_read_byte(u32TunerPort, FEC_BER_H, &temp1);
                temp3 = (temp1 << 8) + temp2;
                ErrorRate = temp3 / total_num * 1000000000;
                break;
            }
        }

        *pu32ber=ErrorRate;
    }

    return HI_SUCCESS;
}

static HI_BOOL hi3136_Set_BS_Mode(HI_U32 u32TunerPort, HI_BOOL bs)   //blind sacn mode
{
    HI_U8 temp1 = 0;
    HI_U8 temp2 = 0;
    HI_U8  u8Temp = 0;

    qam_read_byte(u32TunerPort, CBS_CTRL_RDADDR, &u8Temp);

    temp1 = u8Temp | 0x80;//defalult blind scan mode
    temp2 = u8Temp & 0x7f;

    if(bs == 1)
    {
        qam_write_byte(u32TunerPort, CBS_CTRL_RDADDR, temp1);
    }
    else if(bs == 0)
    {
        qam_write_byte(u32TunerPort, CBS_CTRL_RDADDR, temp2);
    }

    return HI_SUCCESS;
}

/* 0 Power off, 1 power on */
HI_S32 hi3136_lnbctrl_power(HI_U32 u32TunerPort, HI_U8 u8Power)
{
    HI_U8 u8Temp = 0 ;

    if (HI_UNF_DEMOD_DEV_TYPE_3136I == g_stTunerOps[u32TunerPort].enDemodDevType)
    {
#if defined(LNB_CTRL_GPIO)
        if (u8Power)
        {
            HI_DRV_GPIO_WriteBit(HI_LNB_EN_GPIO_NO, 1);
        }
        else
        {
            HI_DRV_GPIO_WriteBit(HI_LNB_EN_GPIO_NO, 0);
        }

        return HI_SUCCESS;
#endif
    }

    if (u8Power)
    {
        qam_read_byte(u32TunerPort, DSEC_EN, &u8Temp);
        u8Temp |= 0x80;
        qam_write_byte(u32TunerPort, DSEC_EN, u8Temp);
    }
    else
    {
        qam_read_byte(u32TunerPort, DSEC_EN, &u8Temp);
        u8Temp &= 0x7f;
        qam_write_byte(u32TunerPort, DSEC_EN, u8Temp);
    }

    return HI_SUCCESS;
}

HI_S32 hi3136_set_lnb_out(HI_U32 u32TunerPort, TUNER_LNB_OUT_E enOut)
{
    switch (enOut)
    {
    case TUNER_LNB_OUT_0V:
    case TUNER_LNB_OUT_BUTT:
    default:
        return HI_FAILURE;

    case TUNER_LNB_OUT_13V:
    case TUNER_LNB_OUT_14V:
        if (HI_UNF_DEMOD_DEV_TYPE_3136I == g_stTunerOps[u32TunerPort].enDemodDevType)
        {
#if defined(LNB_CTRL_GPIO)
            HI_DRV_GPIO_WriteBit(HI_LNB_HV_SEL_GPIO_NO, 0);
            return HI_SUCCESS;
#endif
        }

        qam_write_bit(u32TunerPort, TX_CTRL, 0, 0);
        break;

    case TUNER_LNB_OUT_18V:
    case TUNER_LNB_OUT_19V:
        if (HI_UNF_DEMOD_DEV_TYPE_3136I == g_stTunerOps[u32TunerPort].enDemodDevType)
        {
#if defined(LNB_CTRL_GPIO)
            HI_DRV_GPIO_WriteBit(HI_LNB_HV_SEL_GPIO_NO, 1);
            return HI_SUCCESS;
#endif
        }

        qam_write_bit(u32TunerPort, TX_CTRL, 0, 1);
        break;
    }

    return HI_SUCCESS;
}

HI_S32 hi3136_send_continuous_22K(HI_U32 u32TunerPort,HI_BOOL b22k_on) // 设置22k,
{
    HI_U8   u8Temp = 0;
    HI_U32  diseqc_Ratio = 0;

    //diseqc_Ratio = (HI3136_SYSCLK_SAMPLE / 22) & 0xffff;
    diseqc_Ratio = (hi3136_cfg[u32TunerPort].u32CLK_DEMO_kHz / 22) & 0xffff;

    qam_write_byte(u32TunerPort, DSEC_RATIO_L, (HI_U8)(diseqc_Ratio & 0xff));
    qam_write_byte(u32TunerPort, DSEC_RATIO_H, (HI_U8)((diseqc_Ratio & 0xff00) >> 8));

    qam_read_byte(u32TunerPort, TX_CTRL, &u8Temp);
    u8Temp &= 0xff;
    if(b22k_on)
    {
        u8Temp &= 0xf1;
        u8Temp |= (1 << 1);
    }
    else
    {
        u8Temp &= ~(1 << 1);
    }

    msleep_interruptible(10);
    qam_write_byte(u32TunerPort, TX_CTRL, u8Temp);

    qam_read_byte(u32TunerPort, TX_CTRL, &u8Temp);
    return HI_SUCCESS;
}

static HI_S32 hi3136_set_symrate(HI_U32 u32TunerPort, HI_U32 u32SymRate)  /*u32SymRate unit hz*/
{
    /*HI_U32 u32Temp;
    HI_U8  u8Temp = 0;  */
    HI_S32 Center_shift_Value = 0;

    if(u32SymRate == 0)
        return HI_FAILURE;

    u32SymRate = (u32SymRate / 1000) & 0xffff;
    qam_write_byte(u32TunerPort, FS_L, (HI_U8)(u32SymRate & 0xff));
    qam_write_byte(u32TunerPort, FS_H, (HI_U8)((u32SymRate & 0xff00) >> 8));

    if(u32SymRate <= 5000)
    {
        Center_shift_Value = 5000;
    }

    if(hi3136_cfg[u32TunerPort].enIQSpectrum)
    {
        Center_shift_Value = -Center_shift_Value;
    }

    Center_shift_Value &= 0xffff;
    qam_write_byte(u32TunerPort, CENT_FREQ_L, (HI_U8)(Center_shift_Value&0xff));
    qam_write_byte(u32TunerPort, CENT_FREQ_H, (HI_U8)(Center_shift_Value>>8)&0xff);


    return HI_SUCCESS;
}

HI_S32 hi3136_get_signal_info(HI_U32 u32TunerPort, HI_UNF_TUNER_SIGNALINFO_S *pstInfo)
{

    HI_U8   temp1 = 0;
    HI_U8   temp2 = 0;
    HI_U8   temp3 = 0;
    HI_U8   u8Temp = 0;
    HI_U8   u8Temp1 = 0;

    pstInfo->enSigType = HI_UNF_TUNER_SIG_TYPE_SAT;
    qam_read_byte(u32TunerPort, TX_CTRL, &u8Temp);

    if (hi3136_Read_Lock_Status(u32TunerPort) == FEC_LOCKED)
    {
        qam_read_byte(u32TunerPort, SEAR_RESULT, &u8Temp);
        temp1 = u8Temp & 0x01;
        qam_read_byte(u32TunerPort, LOCK_FLAG, &u8Temp1);
        if(u8Temp1 & 0x08)
        {
            temp1 = 2;    //sync_ok
        }
        switch(temp1)
        {
        case 0:
            {
                pstInfo->unSignalInfo.stSat.enSATType = HI_UNF_TUNER_FE_DVBS;
                pstInfo->unSignalInfo.stSat.enModType = HI_UNF_MOD_TYPE_QPSK;

                temp2 = ((u8Temp & 0x0e) >> 1);
                switch(temp2)
                {
                case 0:
                    {
                        pstInfo->unSignalInfo.stSat.enFECRate = HI_UNF_TUNER_FE_FEC_1_2;
                        break;
                    }
                case 1:
                    {
                        pstInfo->unSignalInfo.stSat.enFECRate = HI_UNF_TUNER_FE_FEC_2_3;
                        break;
                    }
                case 2:
                    {
                        pstInfo->unSignalInfo.stSat.enFECRate = HI_UNF_TUNER_FE_FEC_3_4;
                        break;
                    }
                case 3:
                    {
                        pstInfo->unSignalInfo.stSat.enFECRate = HI_UNF_TUNER_FE_FEC_5_6;
                        break;
                    }
                case 4:
                    {
                        pstInfo->unSignalInfo.stSat.enFECRate = HI_UNF_TUNER_FE_FEC_7_8;
                        break;
                    }
                default:
                    {
                        pstInfo->unSignalInfo.stSat.enFECRate = HI_UNF_TUNER_FE_FECRATE_BUTT;
                        break;
                    }
                }
                break;
            }

        case 1:
            {
                pstInfo->unSignalInfo.stSat.enSATType = HI_UNF_TUNER_FE_DIRECTV;
                pstInfo->unSignalInfo.stSat.enModType = HI_UNF_MOD_TYPE_QPSK;

                temp2 = ((u8Temp & 0x0e) >> 1);
                switch(temp2)
                {
                case 0:
                    {
                        pstInfo->unSignalInfo.stSat.enFECRate = HI_UNF_TUNER_FE_FEC_1_2;
                        break;
                    }
                case 1:
                    {
                        pstInfo->unSignalInfo.stSat.enFECRate = HI_UNF_TUNER_FE_FEC_2_3;
                        break;
                    }
                case 5:
                    {
                        pstInfo->unSignalInfo.stSat.enFECRate = HI_UNF_TUNER_FE_FEC_6_7;
                        break;
                    }
                default:
                    {
                        pstInfo->unSignalInfo.stSat.enFECRate = HI_UNF_TUNER_FE_FECRATE_BUTT;
                        break;
                    }
                }
                break;
            }

        case 2:
            {
                pstInfo->unSignalInfo.stSat.enSATType = HI_UNF_TUNER_FE_DVBS2;

                qam_read_byte(u32TunerPort, CR_PLS_CODE, &u8Temp);
                temp3 = ((u8Temp & 0x7c) >> 2);
                switch (temp3)
                {
                case 1:
                    {
                        pstInfo->unSignalInfo.stSat.enModType = HI_UNF_MOD_TYPE_QPSK;
                        pstInfo->unSignalInfo.stSat.enFECRate = HI_UNF_TUNER_FE_FEC_1_4;
                        break;
                    }
                case 2:
                    {
                        pstInfo->unSignalInfo.stSat.enModType = HI_UNF_MOD_TYPE_QPSK;
                        pstInfo->unSignalInfo.stSat.enFECRate = HI_UNF_TUNER_FE_FEC_1_3;
                        break;
                    }
                case 3:
                    {
                        pstInfo->unSignalInfo.stSat.enModType = HI_UNF_MOD_TYPE_QPSK;
                        pstInfo->unSignalInfo.stSat.enFECRate = HI_UNF_TUNER_FE_FEC_2_5;
                        break;
                    }
                case 4:
                    {
                        pstInfo->unSignalInfo.stSat.enModType = HI_UNF_MOD_TYPE_QPSK;
                        pstInfo->unSignalInfo.stSat.enFECRate = HI_UNF_TUNER_FE_FEC_1_2;
                        break;
                    }
                case 5:
                    {
                        pstInfo->unSignalInfo.stSat.enModType = HI_UNF_MOD_TYPE_QPSK;
                        pstInfo->unSignalInfo.stSat.enFECRate = HI_UNF_TUNER_FE_FEC_3_5;
                        break;
                    }
                case 6:
                    {
                        pstInfo->unSignalInfo.stSat.enModType = HI_UNF_MOD_TYPE_QPSK;
                        pstInfo->unSignalInfo.stSat.enFECRate = HI_UNF_TUNER_FE_FEC_2_3;
                        break;
                    }
                case 7:
                    {
                        pstInfo->unSignalInfo.stSat.enModType = HI_UNF_MOD_TYPE_QPSK;
                        pstInfo->unSignalInfo.stSat.enFECRate = HI_UNF_TUNER_FE_FEC_3_4;
                        break;
                    }
                case 8:
                    {
                        pstInfo->unSignalInfo.stSat.enModType = HI_UNF_MOD_TYPE_QPSK;
                        pstInfo->unSignalInfo.stSat.enFECRate = HI_UNF_TUNER_FE_FEC_4_5;
                        break;
                    }
                case 9:
                    {
                        pstInfo->unSignalInfo.stSat.enModType = HI_UNF_MOD_TYPE_QPSK;
                        pstInfo->unSignalInfo.stSat.enFECRate = HI_UNF_TUNER_FE_FEC_5_6;
                        break;
                    }
                case 10:
                    {
                        pstInfo->unSignalInfo.stSat.enModType = HI_UNF_MOD_TYPE_QPSK;
                        pstInfo->unSignalInfo.stSat.enFECRate = HI_UNF_TUNER_FE_FEC_8_9;
                        break;
                    }
                case 11:
                    {
                        pstInfo->unSignalInfo.stSat.enModType = HI_UNF_MOD_TYPE_QPSK;
                        pstInfo->unSignalInfo.stSat.enFECRate = HI_UNF_TUNER_FE_FEC_9_10;
                        break;
                    }
                case 12:
                    {
                        pstInfo->unSignalInfo.stSat.enModType = HI_UNF_MOD_TYPE_8PSK;
                        pstInfo->unSignalInfo.stSat.enFECRate = HI_UNF_TUNER_FE_FEC_3_5;
                        break;
                    }
                case 13:
                    {
                        pstInfo->unSignalInfo.stSat.enModType = HI_UNF_MOD_TYPE_8PSK;
                        pstInfo->unSignalInfo.stSat.enFECRate = HI_UNF_TUNER_FE_FEC_2_3;
                        break;
                    }
                case 14:
                    {
                        pstInfo->unSignalInfo.stSat.enModType = HI_UNF_MOD_TYPE_8PSK;
                        pstInfo->unSignalInfo.stSat.enFECRate = HI_UNF_TUNER_FE_FEC_3_4;
                        break;
                    }
                case 15:
                    {
                        pstInfo->unSignalInfo.stSat.enModType = HI_UNF_MOD_TYPE_8PSK;
                        pstInfo->unSignalInfo.stSat.enFECRate = HI_UNF_TUNER_FE_FEC_5_6;
                        break;
                    }
                case 16:
                    {
                        pstInfo->unSignalInfo.stSat.enModType = HI_UNF_MOD_TYPE_8PSK;
                        pstInfo->unSignalInfo.stSat.enFECRate = HI_UNF_TUNER_FE_FEC_8_9;
                        break;
                    }
                case 17:
                    {
                        pstInfo->unSignalInfo.stSat.enModType = HI_UNF_MOD_TYPE_8PSK;
                        pstInfo->unSignalInfo.stSat.enFECRate = HI_UNF_TUNER_FE_FEC_9_10;
                        break;
                    }
                case 18:
                    {
                        pstInfo->unSignalInfo.stSat.enModType = HI_UNF_MOD_TYPE_16APSK;
                        pstInfo->unSignalInfo.stSat.enFECRate = HI_UNF_TUNER_FE_FEC_2_3;
                        break;
                    }
                case 19:
                    {
                        pstInfo->unSignalInfo.stSat.enModType = HI_UNF_MOD_TYPE_16APSK;
                        pstInfo->unSignalInfo.stSat.enFECRate = HI_UNF_TUNER_FE_FEC_3_4;
                        break;
                    }
                case 20:
                    {
                        pstInfo->unSignalInfo.stSat.enModType = HI_UNF_MOD_TYPE_16APSK;
                        pstInfo->unSignalInfo.stSat.enFECRate = HI_UNF_TUNER_FE_FEC_4_5;
                        break;
                    }
                case 21:
                    {
                        pstInfo->unSignalInfo.stSat.enModType = HI_UNF_MOD_TYPE_16APSK;
                        pstInfo->unSignalInfo.stSat.enFECRate = HI_UNF_TUNER_FE_FEC_5_6;
                        break;
                    }
                case 22:
                    {
                        pstInfo->unSignalInfo.stSat.enModType = HI_UNF_MOD_TYPE_16APSK;
                        pstInfo->unSignalInfo.stSat.enFECRate = HI_UNF_TUNER_FE_FEC_8_9;
                        break;
                    }
                case 23:
                    {
                        pstInfo->unSignalInfo.stSat.enModType = HI_UNF_MOD_TYPE_16APSK;
                        pstInfo->unSignalInfo.stSat.enFECRate = HI_UNF_TUNER_FE_FEC_9_10;
                        break;
                    }
                case 24:
                    {
                        pstInfo->unSignalInfo.stSat.enModType = HI_UNF_MOD_TYPE_32APSK;
                        pstInfo->unSignalInfo.stSat.enFECRate = HI_UNF_TUNER_FE_FEC_3_4;
                        break;
                    }
                case 25:
                    {
                        pstInfo->unSignalInfo.stSat.enModType = HI_UNF_MOD_TYPE_32APSK;
                        pstInfo->unSignalInfo.stSat.enFECRate = HI_UNF_TUNER_FE_FEC_4_5;
                        break;
                    }
                case 26:
                    {
                        pstInfo->unSignalInfo.stSat.enModType = HI_UNF_MOD_TYPE_32APSK;
                        pstInfo->unSignalInfo.stSat.enFECRate = HI_UNF_TUNER_FE_FEC_5_6;
                        break;
                    }
                case 27:
                    {
                        pstInfo->unSignalInfo.stSat.enModType = HI_UNF_MOD_TYPE_32APSK;
                        pstInfo->unSignalInfo.stSat.enFECRate = HI_UNF_TUNER_FE_FEC_8_9;
                        break;
                    }
                case 28:
                    {
                        pstInfo->unSignalInfo.stSat.enModType = HI_UNF_MOD_TYPE_32APSK;
                        pstInfo->unSignalInfo.stSat.enFECRate = HI_UNF_TUNER_FE_FEC_9_10;
                        break;
                    }
                default:
                    {
                        pstInfo->unSignalInfo.stSat.enModType = HI_UNF_MOD_TYPE_BUTT;
                        pstInfo->unSignalInfo.stSat.enFECRate = HI_UNF_TUNER_FE_FECRATE_BUTT;
                        break;
                    }
                }
                break;
            }

        default:
            {
                pstInfo->unSignalInfo.stSat.enSATType = HI_UNF_TUNER_FE_BUTT;
                pstInfo->unSignalInfo.stSat.enModType = HI_UNF_MOD_TYPE_BUTT;
                pstInfo->unSignalInfo.stSat.enFECRate = HI_UNF_TUNER_FE_FECRATE_BUTT;
            }
        }
    }
    else
    {
        HI_INFO_TUNER("Signal drop!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 hi3136_lock_TP(HI_U32 u32TunerPort, TUNER_ACC_QAM_PARAMS_S *tp)
{
    HI_U8 temp = 0;
    HI_U8 temp1 = 0;	 
    HI_S16 center_shift_KHz = 0;
    HI_S32 s32FuncRet = HI_SUCCESS;

    hi3136_Set_BS_Mode(u32TunerPort, 0);

    hi3136_set_symrate(u32TunerPort, tp->unSRBW.u32SymbolRate);
    qam_read_byte(u32TunerPort, CENT_FREQ_L, &temp1);
    qam_read_byte(u32TunerPort, CENT_FREQ_H, &temp);

    center_shift_KHz = (HI_S16)((temp << 8) + temp1);

    if(hi3136_cfg[u32TunerPort].enIQSpectrum)
    {
        center_shift_KHz = -center_shift_KHz;
    }

    hi3136_setlpf(u32TunerPort, tp->unSRBW.u32SymbolRate + abs(center_shift_KHz));
    if (g_stTunerOps[u32TunerPort].set_tuner)
    {
        s32FuncRet = g_stTunerOps[u32TunerPort].set_tuner(u32TunerPort, g_stTunerOps[u32TunerPort].enI2cChannel, tp->u32Frequency -center_shift_KHz + 500);
        if (HI_SUCCESS != s32FuncRet)
        {
            HI_ERR_TUNER( "set tuner error, s32FuncRet = %d\n", s32FuncRet);
            return s32FuncRet;
        }
    }

    hi3136_HotReset_CHIP(u32TunerPort);
    mdelay(20);

    return HI_SUCCESS;
}

static HI_VOID hi3136_change_iterNum(HI_U32 u32TunerPort)
{
    HI_U8 s2_sync_bit = 0, temp1 = 0, temp2 = 0, temp3 = 0, u8Temp = 0, M= 0;
    HI_U32 fs_cur = 0;
    HI_U16 ldpc_iter_num = 0;
    HI_U32 fec_clk, ldpc_iter_lmt = 0;
    HI_U8 fec_freq_l=0, fec_freq_m=0, fec_freq_h = 0;

    qam_read_bit(u32TunerPort, LOCK_FLAG, 3, &s2_sync_bit);
    if(s2_sync_bit)
    {
        if(HI_UNF_DEMOD_DEV_TYPE_3136 == g_stTunerOps[u32TunerPort].enDemodDevType)
        {
            qam_read_byte(u32TunerPort, CLK_FEC_L, &fec_freq_l);
            qam_read_byte(u32TunerPort, CLK_FEC_M, &fec_freq_m);
            qam_read_byte(u32TunerPort, CLK_FEC_H, &fec_freq_h);

            fec_clk = fec_freq_l + fec_freq_m * 256 + fec_freq_h*65536;
        }
        else
        {
            fec_clk = 187500;
        }

        qam_read_byte(u32TunerPort, FS_L, &temp1);
        qam_read_byte(u32TunerPort, FS_H, &temp2);
        fs_cur = temp1 + temp2 * 256;

        qam_read_byte(u32TunerPort, CR_PLS_CODE, &u8Temp);
        temp3 = ((u8Temp & 0x7c) >> 2);
        if(temp3 > 0 && temp3 <= 11)
            M = 2;
        else if(temp3 > 11 && temp3 <= 17)
            M = 3;
        else if(temp3 > 17 && temp3 <= 23)
            M = 4;
        else
            M = 5;

        ldpc_iter_lmt = ((648 * fec_clk) / (450 * 3 * 4032)) - 1;

        if(ldpc_iter_lmt > LDPC_ITER_45M)
            ldpc_iter_lmt = LDPC_ITER_45M;

        ldpc_iter_num = (HI_U16)((((4500000 / fs_cur) * ldpc_iter_lmt) / (M * 100)) * 3);

        if(ldpc_iter_num > LDPC_ITER_MAX)
            ldpc_iter_num = LDPC_ITER_MAX;

        qam_write_byte(u32TunerPort, LDPC_ITER_ADDR, ldpc_iter_num | 0xc0);
    }
}

HI_S32 hi3136_lock_TP_BS(HI_U32 u32TunerPort, TUNER_ACC_QAM_PARAMS_S *tp)
{
    HI_U8  j=0, u8Temp=0, temp_TPnum=0, tempL=0, tempH=0, fs_grade=0, read_reliable=0, fec_ok_bit = 0, old_bw_max=0;
    HI_U32 fs_cur, time_cnt=0, fec_ok_wait=0, max_reliablity=0, reliablity, cbs_fs_sel = 0;
    HI_S16 fc_true=0, center_shift_KHz=0, cbs_fc=0, cbs_fc_sel = 0;
    HI_S32 s32FuncRet = HI_SUCCESS;

    qam_read_byte (u32TunerPort, 0x58, &old_bw_max);
    qam_write_byte(u32TunerPort, 0x58, 0x12);
    qam_write_byte(u32TunerPort, LDPC_ITER_ADDR, 0x50);
    qam_write_byte(u32TunerPort, 0xa1, 0x05);

    qam_write_byte(u32TunerPort, CBS_CTRL_RDADDR, 0xc0);    //bs with symbrate known

    fs_cur = tp->unSRBW.u32SymbolRate;
    hi3136_set_symrate(u32TunerPort, fs_cur);
    qam_read_byte(u32TunerPort, CENT_FREQ_L, &tempL);
    qam_read_byte(u32TunerPort, CENT_FREQ_H, &tempH);
    fc_true = (HI_S16) (tempL + (tempH<<8));

    center_shift_KHz = (hi3136_cfg[u32TunerPort].enIQSpectrum) ? (-fc_true) : fc_true;

    //fs_grade判断， unit Hz
    if         (fs_cur< 2000000)                    fs_grade=0;
    else if (fs_cur>=2000000 && fs_cur<3000000)     fs_grade=1;
    else if (fs_cur>=3000000 && fs_cur<4900000)     fs_grade=2;
    else if (fs_cur>=4900000 && fs_cur<8000000)     fs_grade=3;
    else if (fs_cur>=8000000 && fs_cur<15000000)    fs_grade=4;
    else                                            fs_grade=5;

    hi3136_setlpf(u32TunerPort, fs_cur / 10 * 7 + abs(center_shift_KHz) * 1000 + 5000000);
    if (g_stTunerOps[u32TunerPort].set_tuner)
    {
        s32FuncRet = g_stTunerOps[u32TunerPort].set_tuner(u32TunerPort, g_stTunerOps[u32TunerPort].enI2cChannel, tp->u32Frequency - (HI_S32)(center_shift_KHz));
        if (s32FuncRet != HI_SUCCESS)
        {
            HI_ERR_TUNER( "set tuner error\n");
            return s32FuncRet;
        }
    }

    mdelay(20);
    hi3136_HotReset_CHIP(u32TunerPort);

    time_cnt = 0;
    do
    {
        mdelay(2);
        time_cnt += 2;
        qam_read_bit(u32TunerPort, CBS_FINISH, 0, &u8Temp);
    } while(!u8Temp && time_cnt < 300);

    temp_TPnum = 0;
    if(u8Temp)  //cbs_finish 标志位1表示已完成
    {
        qam_read_byte(u32TunerPort, CBS_R2_NUM, &tempL);
        temp_TPnum = tempL & 0x1f;
    }

    if(temp_TPnum > 0)
    {
        max_reliablity = 0;
        qam_read_byte(u32TunerPort, CBS_CTRL_RDADDR, &u8Temp);
        for(j = 0; j < temp_TPnum; j++)     //已知Fs Fc情况下，读盲扫信息参数，同时找最大综合可靠度
        {
            qam_write_byte(u32TunerPort, CBS_CTRL_RDADDR, (u8Temp & 0xe0) | j);

            qam_read_byte(u32TunerPort, CBS_RELIABLITY, &read_reliable);
            qam_read_byte(u32TunerPort, CBS_FC_L, &tempL);
            qam_read_byte(u32TunerPort, CBS_FC_H, &tempH);
            cbs_fc = (HI_S16)(tempL + (tempH << 8));
            reliablity = abs(cbs_fc) / 64 + read_reliable;      //计算综合可靠度 ? 溢出

            if(reliablity > max_reliablity)
            {
                max_reliablity = reliablity;
                qam_read_byte(u32TunerPort, CBS_FS_L, &tempL);
                qam_read_byte(u32TunerPort, CBS_FS_H, &tempH);
                cbs_fs_sel = tempL + (tempH<<8);
                cbs_fc_sel = cbs_fc;
            }
        }

        //将盲扫的结果置入配置寄存器，再关闭盲扫，复位
        hi3136_set_symrate(u32TunerPort, cbs_fs_sel * 1000);
        fc_true += cbs_fc_sel;
        //printk("%s, %d, cbs_fc_sel=%d\n", __func__, __LINE__,  cbs_fc_sel);


        qam_write_byte(u32TunerPort, CENT_FREQ_L, fc_true & 0xff);
        qam_write_byte(u32TunerPort, CENT_FREQ_H, (fc_true >> 8) & 0xff);

        qam_write_byte(u32TunerPort, CBS_CTRL_RDADDR, 0x00);    //关闭盲扫，Ram地址为0
        hi3136_setlpf(u32TunerPort, cbs_fs_sel * 1000 / 10 * 7 + abs(fc_true) * 1000 + 3500000);    //中心准确已知，余量小些，加3.5M

        mdelay(10); //没有盲扫，对TUNER输出不稳定的抵抗力强，可以少等待。另外tuner仅改带宽，应该很快稳定。
        hi3136_HotReset_CHIP(u32TunerPort);

        fec_ok_wait = (320 - fs_grade * 20) * 2 + 600;
        time_cnt = 0;
        do      //等待FEC-OK
        {
            mdelay(2);
            time_cnt += 2;
            qam_read_bit(u32TunerPort, LOCK_FLAG, 5, &fec_ok_bit);
            if(fec_ok_bit)
            {
                hi3136_change_iterNum(u32TunerPort);
            }
        } while(!fec_ok_bit && (time_cnt < fec_ok_wait));
    }
    else
    {
        qam_write_byte(u32TunerPort, 0x58, old_bw_max);
        hi3136_HotReset_CHIP(u32TunerPort);
        if (HI_UNF_DEMOD_DEV_TYPE_3136I == g_stTunerOps[u32TunerPort].enDemodDevType)
        {
            qam_write_byte(u32TunerPort, CBS_CTRL_RDADDR, 0xc0);
        }
        return HI_SUCCESS;
    }

    qam_write_byte(u32TunerPort, 0x58, old_bw_max);
    if (HI_UNF_DEMOD_DEV_TYPE_3136I == g_stTunerOps[u32TunerPort].enDemodDevType)
    {
        qam_write_byte(u32TunerPort, CBS_CTRL_RDADDR, 0xc0);
    }
    return HI_SUCCESS;
}


HI_U32 hi3136_BS_one_Window(HI_U32 u32TunerPort, HI_U32 Fcenter/*MHz*/, HI_U32 Lpf_BW_window/*kHz*/, HI_UNF_TUNER_SAT_TPINFO_S *pstChannel)
{
    HI_U32  TP_Num = 0, time_cnt;
    HI_U8   i=0;
    HI_U8   u8Temp=0, tempL=0, tempH=0, reliablity=0;
    HI_S16  Fc_offset_Bs=0;
    HI_S32  s32FuncRet = HI_SUCCESS;
    HI_U32  Fs_Bs=0;

    qam_write_byte(u32TunerPort, CBS_CTRL_RDADDR, 0x80);

    hi3136_setlpf(u32TunerPort, Lpf_BW_window * 1000);
    if (g_stTunerOps[u32TunerPort].set_tuner)
    {
        s32FuncRet = g_stTunerOps[u32TunerPort].set_tuner(u32TunerPort, g_stTunerOps[u32TunerPort].enI2cChannel, Fcenter * 1000);
        if (HI_SUCCESS != s32FuncRet)
        {
            HI_ERR_TUNER( "set tuner error\n");
            return s32FuncRet;
        }
    }

    tuner_mdelay(12);
    hi3136_HotReset_CHIP(u32TunerPort);

    time_cnt = 0;
    do
    {
        tuner_mdelay(5);
        time_cnt += 5;
        qam_read_bit(u32TunerPort, CBS_FINISH, 0, &u8Temp);
    } while(!u8Temp && (time_cnt < 300));       //等待读取CBS_FINISH 操作,当前延时10000ms，需确认延时

    if(u8Temp)  //cbs_finish 标志位1表示已完成
    {
        qam_read_byte(u32TunerPort, CBS_R2_NUM, &tempL);
        TP_Num = tempL & 0x1f;
    }

    if(TP_Num != 0)
    {
        qam_read_byte(u32TunerPort, CBS_CTRL_RDADDR, &u8Temp);
        for(i = 0; i < TP_Num; i++)
        {
            qam_write_byte(u32TunerPort, CBS_CTRL_RDADDR, (u8Temp & 0xe0) | i);
            qam_read_byte(u32TunerPort, CBS_FC_L, &tempL);
            qam_read_byte(u32TunerPort, CBS_FC_H, &tempH);
            Fc_offset_Bs = (HI_S16)(tempL + (tempH << 8));

            if(hi3136_cfg[u32TunerPort].enIQSpectrum)
            {
                Fc_offset_Bs = -Fc_offset_Bs;  //具体根据实际硬件配置来
            }

            pstChannel->u32Freq = (Fcenter * 1000) + Fc_offset_Bs;

            qam_read_byte(u32TunerPort, CBS_FS_L, &tempL);
            qam_read_byte(u32TunerPort, CBS_FS_H, &tempH);
            Fs_Bs = tempL + (tempH << 8);
            pstChannel->u32SymbolRate = Fs_Bs * 1000;//符号率

            qam_read_byte(u32TunerPort, CBS_RELIABLITY, &reliablity);
            pstChannel->cbs_reliablity = reliablity;    //cbs 结果可靠度，越大越可靠
            pstChannel++;
        }
    }
    else
    {
        return 0;
    }

    return TP_Num;
}

HI_S32 hi3136_set_funcmode(HI_U32 u32TunerPort, TUNER_FUNC_MODE_E enFuncMode)
{
    return HI_SUCCESS;
}

HI_S32 hi3136_blindscan_init(HI_U32 u32TunerPort, TUNER_BLINDSCAN_INITPARA_S *pstPara)
{
    HI_TUNER_CHECKPOINTER( pstPara);

    /* Check tuner port and init. */
    if (u32TunerPort >= TUNER_NUM)
    {
        HI_INFO_TUNER("Bad u32TunerPort\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}
HI_S32 hi3136_blindscan_action(HI_U32 u32TunerPort, TUNER_BLINDSCAN_PARA_S *pstPara)
{
    HI_U8 u8Temp=0;

    pstPara->u16Count = hi3136_BS_one_Window(u32TunerPort, pstPara->u32CentreFreq, HI3136_BS_WINDOW_STEP_KHZ, pstPara->unResult.astSat);

    qam_read_byte(u32TunerPort, CBS_CTRL_RDADDR, &u8Temp);
    u8Temp &= 0xe0;
    qam_write_byte(u32TunerPort, CBS_CTRL_RDADDR, u8Temp);//将cbs_rd_addr 清零

    return HI_SUCCESS;
}

static HI_S32 hi3136_setlpf(HI_U32 u32TunerPort, HI_U32 u32SymbolRate/*unit BAUD ,neither K, nor M*/)
{
    u32LPF_KHz[u32TunerPort] = (u32SymbolRate * 75 / 10000000 + 40) * 100;

    if (u32LPF_KHz[u32TunerPort] >= hi3136_cfg[u32TunerPort].u16TunerLPF_100Khz * 100)
    {
        u32LPF_KHz[u32TunerPort] = hi3136_cfg[u32TunerPort].u16TunerLPF_100Khz * 100;
    }
    return HI_SUCCESS;
}

HI_S32 hi3136_init(HI_U32 u32TunerPort, HI_U8 enI2cChannel, HI_UNF_TUNER_DEV_TYPE_E enTunerDevType)
{
    if (u32TunerPort >= TUNER_NUM)
    {
        return HI_FAILURE;
    }

    if(HI_UNF_DEMOD_DEV_TYPE_3136 == g_stTunerOps[u32TunerPort].enDemodDevType)
    {
        qam_write_byte(u32TunerPort, ADC_CTRL1, 0x7a);//adc power down
        qam_write_byte(u32TunerPort, ADC_CTRL1, 0x4a);//adc power on
    }

    qam_write_byte(u32TunerPort, TS_CTRL4, 0x8);
    qam_write_byte(u32TunerPort, TS_CTRL0, 0x12);

    /* Init tuner fuctions */
    switch (enTunerDevType)
    {
#if defined(TUNER_DEV_TYPE_AV2011)
    case HI_UNF_TUNER_DEV_TYPE_AV2011:
        av2011_init_tuner(u32TunerPort);
        break;
#endif

#if defined(TUNER_DEV_TYPE_SHARP7903)
    case HI_UNF_TUNER_DEV_TYPE_SHARP7903:
        sharp7903_init_tuner(u32TunerPort);
        break;
#endif

#if defined(TUNER_DEV_TYPE_M88TS2022)
    case HI_UNF_TUNER_DEV_TYPE_M88TS2022:
        M88TS2022_init_tuner(u32TunerPort);
        break;
#endif

#if defined(TUNER_DEV_TYPE_RDA5815)
    case HI_UNF_TUNER_DEV_TYPE_RDA5815:
        RDA5815_init_tuner(u32TunerPort);
        break;
#endif

    default:
        HI_ERR_TUNER("Tuner type not supported!\n");
        return HI_FAILURE;
    }
 #if defined(TUNER_DEV_TYPE_RDA5815)  ||  defined(TUNER_DEV_TYPE_M88TS2022) ||defined(TUNER_DEV_TYPE_SHARP7903) || defined(TUNER_DEV_TYPE_AV2011)
    qam_write_byte(u32TunerPort, 0x22, 0x1e);      //reset waiting time
    qam_write_byte(u32TunerPort, 0x8c, 0x85 | (0<<5) | (1<<3));      //fec not search direcTV,
    qam_write_byte(u32TunerPort, 0x8d, 0x5e);      //fec not search direcTV,
    qam_write_byte(u32TunerPort, 0xa1, 0x05);
#endif
    return HI_SUCCESS;
}

HI_S32 hi3136_set_sat_attr(HI_U32 u32TunerPort, HI_UNF_TUNER_SAT_ATTR_S *pstSatTunerAttr)
{
    HI_UNF_TUNER_SAT_ATTR_S* pstAttr = HI_NULL;

    HI_TUNER_CHECKPOINTER( pstSatTunerAttr);

    if (u32TunerPort >= TUNER_NUM)
    {
        return HI_FAILURE;
    }

    pstAttr = (HI_UNF_TUNER_SAT_ATTR_S*)pstSatTunerAttr;

    hi3136_cfg[u32TunerPort].u16TunerLPF_100Khz = pstAttr->u16TunerMaxLPF * 10;
    hi3136_cfg[u32TunerPort].enIQSpectrum = pstAttr->enIQSpectrum;

    //修改agc_inverse
    switch (pstAttr->enRFAGC)
    {
    case HI_UNF_TUNER_RFAGC_INVERT:
    default:
        qam_write_bit(u32TunerPort, AGC_CTRL, 1, 1);
        break;
    case HI_UNF_TUNER_RFAGC_NORMAL:
        qam_write_bit(u32TunerPort, AGC_CTRL, 1, 0);
        break;
    }

    if(HI_UNF_DEMOD_DEV_TYPE_3136 == g_stTunerOps[u32TunerPort].enDemodDevType)
    {
        switch (pstAttr->u32DemodClk)
        {
        case 13500:
            qam_write_bit(u32TunerPort, ADC_CTRL2, 4, 1);
            //set PLL1
            qam_write_bit(u32TunerPort, PLL_CTRL1, 7, 1);
            qam_write_byte(u32TunerPort, PLL_CTRL3, 0x37);
            qam_write_bit(u32TunerPort, PLL_CTRL1, 7, 0);
            //set PLL2
            qam_write_bit(u32TunerPort, PLL_CTRL5, 7, 1);
            qam_write_byte(u32TunerPort, PLL_CTRL8, 0x52);
            qam_write_bit(u32TunerPort, PLL_CTRL5, 7, 0);
            mdelay(5);
            qam_write_byte(u32TunerPort, PLL_CTRL5, 0x4);
            qam_write_byte(u32TunerPort, PLL_CTRL10, 0xe);

            qam_write_bit(u32TunerPort, ADC_CTRL2, 4, 0);

            qam_write_byte(u32TunerPort, CLK_DEMO_L, 0x98);
            qam_write_byte(u32TunerPort, CLK_DEMO_M, 0xe5);
            qam_write_byte(u32TunerPort, CLK_DEMO_H, 0x1);
            hi3136_cfg[u32TunerPort].u32CLK_DEMO_kHz = 124312;

            qam_write_byte(u32TunerPort, CLK_FEC_L, 0x65);
            qam_write_byte(u32TunerPort, CLK_FEC_M, 0xd8);
            qam_write_byte(u32TunerPort, CLK_FEC_H, 0x2);
            break;
        case 16000:
            qam_write_bit(u32TunerPort, ADC_CTRL2, 4, 1);
            //set PLL1
            qam_write_bit(u32TunerPort, PLL_CTRL1, 7, 1);
            qam_write_byte(u32TunerPort, PLL_CTRL3, 0x2e);
            qam_write_bit(u32TunerPort, PLL_CTRL1, 7, 0);
            //set PLL2
            qam_write_bit(u32TunerPort, PLL_CTRL5, 7, 1);
            qam_write_byte(u32TunerPort, PLL_CTRL8, 0x45);
            qam_write_bit(u32TunerPort, PLL_CTRL5, 7, 0);
            mdelay(5);
            qam_write_byte(u32TunerPort, PLL_CTRL5, 0x4);
            qam_write_byte(u32TunerPort, PLL_CTRL10, 0xe);

            qam_write_bit(u32TunerPort, ADC_CTRL2, 4, 0);

            qam_write_byte(u32TunerPort, CLK_DEMO_L, 0xc5);
            qam_write_byte(u32TunerPort, CLK_DEMO_M, 0xe1);
            qam_write_byte(u32TunerPort, CLK_DEMO_H, 0x1);
            hi3136_cfg[u32TunerPort].u32CLK_DEMO_kHz = 123333;

            qam_write_byte(u32TunerPort, CLK_FEC_L, 0xdd);
            qam_write_byte(u32TunerPort, CLK_FEC_M, 0xd7);
            qam_write_byte(u32TunerPort, CLK_FEC_H, 0x2);
            break;
        case 24000:
            qam_write_bit(u32TunerPort, ADC_CTRL2, 4, 1);
            //set PLL1
            qam_write_bit(u32TunerPort, PLL_CTRL1, 7, 1);
            qam_write_byte(u32TunerPort, PLL_CTRL3, 0x1f);
            qam_write_bit(u32TunerPort, PLL_CTRL1, 7, 0);
            //set PLL2
            qam_write_bit(u32TunerPort, PLL_CTRL5, 7, 1);
            qam_write_byte(u32TunerPort, PLL_CTRL8, 0x2e);
            qam_write_bit(u32TunerPort, PLL_CTRL5, 7, 0);
            mdelay(5);
            qam_write_byte(u32TunerPort, PLL_CTRL5, 0x4);
            qam_write_byte(u32TunerPort, PLL_CTRL10, 0xe);

            qam_write_bit(u32TunerPort, ADC_CTRL2, 4, 0);

            qam_write_byte(u32TunerPort, CLK_DEMO_L, 0x48);
            qam_write_byte(u32TunerPort, CLK_DEMO_M, 0xe8);
            qam_write_byte(u32TunerPort, CLK_DEMO_H, 0x1);
            hi3136_cfg[u32TunerPort].u32CLK_DEMO_kHz = 125000;

            qam_write_byte(u32TunerPort, CLK_FEC_L, 0x6c);
            qam_write_byte(u32TunerPort, CLK_FEC_M, 0xdc);
            qam_write_byte(u32TunerPort, CLK_FEC_H, 0x2);
            break;
        case 27000:
            qam_write_bit(u32TunerPort, ADC_CTRL2, 4, 1);
            //set PLL1
            qam_write_bit(u32TunerPort, PLL_CTRL1, 7, 1);
            qam_write_byte(u32TunerPort, PLL_CTRL3, 0x1b);
            qam_write_bit(u32TunerPort, PLL_CTRL1, 7, 0);
            //set PLL2
            qam_write_bit(u32TunerPort, PLL_CTRL5, 7, 1);
            qam_write_byte(u32TunerPort, PLL_CTRL8, 0x28);
            qam_write_bit(u32TunerPort, PLL_CTRL5, 7, 0);
            mdelay(5);
            qam_write_byte(u32TunerPort, PLL_CTRL5, 0x4);
            qam_write_byte(u32TunerPort, PLL_CTRL10, 0xe);

            qam_write_bit(u32TunerPort, ADC_CTRL2, 4, 0);

            qam_write_byte(u32TunerPort, CLK_DEMO_L, 0x1);
            qam_write_byte(u32TunerPort, CLK_DEMO_M, 0xdf);
            qam_write_byte(u32TunerPort, CLK_DEMO_H, 0x1);
            hi3136_cfg[u32TunerPort].u32CLK_DEMO_kHz = 122625;

            qam_write_byte(u32TunerPort, CLK_FEC_L, 0x81);
            qam_write_byte(u32TunerPort, CLK_FEC_M, 0xce);
            qam_write_byte(u32TunerPort, CLK_FEC_H, 0x2);
            break;
        }
    }
    else
    {
        hi3136_cfg[u32TunerPort].u32CLK_DEMO_kHz = 125000;
    }

    return HI_SUCCESS;
}

HI_S32 hi3136_connect(HI_U32 u32TunerPort, TUNER_ACC_QAM_PARAMS_S *pstChannel)
{
    HI_S32 s32FuncRet = HI_SUCCESS;
    
    HI_TUNER_CHECKPOINTER( pstChannel);

    s32FuncRet = hi3136_lock_TP_BS(u32TunerPort, pstChannel); //改成开邋BS
    if (HI_SUCCESS != s32FuncRet)
    {
        HI_ERR_TUNER( "set tuner error\n");
        return s32FuncRet;
    }
    if(u32TunerPort < TUNER_NUM)
    {
          hi3136_config_IQ_swap(u32TunerPort, hi3136_cfg[u32TunerPort].enIQSpectrum);     /*配置IQ*/

          hi3136_set_ts_type(u32TunerPort, g_stTunerOps[u32TunerPort].enTsType);
    }
    else
    {
         HI_ERR_TUNER( "u32TunerPort >  TUNER_NUM error\n");
         return HI_FAILURE;
    }

     return HI_SUCCESS;
}

HI_S32 hi3136_get_status (HI_U32 u32TunerPort, HI_UNF_TUNER_LOCK_STATUS_E  *penTunerStatus)
{
    HI_TUNER_CHECKPOINTER( penTunerStatus);

    if (hi3136_Read_Lock_Status(u32TunerPort) == FEC_LOCKED)
    {
        *penTunerStatus = HI_UNF_TUNER_SIGNAL_LOCKED;
    }
    else
    {
        *penTunerStatus = HI_UNF_TUNER_SIGNAL_DROPPED;
    }

    return HI_SUCCESS;
}

HI_S32 hi3136_set_ts_type(HI_U32 u32TunerPort, HI_UNF_TUNER_OUPUT_MODE_E enTsType)
{
    HI_U8 u8Temp=0;

    /*ts type control*/
    switch (enTsType)
    {
    case HI_UNF_TUNER_OUTPUT_MODE_PARALLEL_MODE_A:
    case HI_UNF_TUNER_OUTPUT_MODE_PARALLEL_MODE_B:
    case HI_UNF_TUNER_OUTPUT_MODE_DEFAULT:
        {
            qam_read_byte(u32TunerPort, TS_PARALL_CTRL, &u8Temp);
            u8Temp = u8Temp & 0xe0;
            u8Temp = u8Temp | (1 << 5) | 0x0a;
            qam_write_byte(u32TunerPort, TS_PARALL_CTRL, u8Temp);/*config ts_out_10bit*/
            //qam_write_byte(u32TunerPort, TS_OUT98_SEL, 0x98);/*config ts_out_9&8bit*/
            //qam_write_byte(u32TunerPort, TS_OUT76_SEL, 0x76 );/*config ts_out_7&6bit*/
            //qam_write_byte(u32TunerPort, TS_OUT54_SEL, 0x54 );/*config ts_out_5&4bit*/
            //qam_write_byte(u32TunerPort, TS_OUT32_SEL, 0x32 );/*config ts_out_3&2bit*/
            //qam_write_byte(u32TunerPort, TS_OUT10_SEL, 0x10 );/*config ts_out_1&0bit*/
            //qam_write_bit(u32TunerPort,  TS_CTRL0, 1, 1);
            break;
        }
    case HI_UNF_TUNER_OUTPUT_MODE_SERIAL:
        {
            qam_write_bit(u32TunerPort, TS_PARALL_CTRL, 5, 0);/*SERIAL MODE*/
            //qam_write_byte(u32TunerPort,TS_OUT98_SEL, 0x0f);/*config ts_out_9&8bit*/
            //qam_write_byte(u32TunerPort,TS_OUT76_SEL,0x76 );/*config ts_out_7&6bit   在串行配置具体可调整*/
            //qam_write_bit(u32TunerPort, TS_CTRL0, 1, 1);
            break;
        }
    case HI_UNF_TUNER_OUTPUT_MODE_SERIAL_2BIT:
        {
            qam_read_byte(u32TunerPort, TS_PARALL_CTRL, &u8Temp);
            u8Temp = u8Temp | (1 << 5) | (1 << 4);
            qam_write_byte(u32TunerPort, TS_PARALL_CTRL, u8Temp);
            //qam_write_byte(u32TunerPort,TS_OUT98_SEL, 0x01);/*config ts_out_9&8bit*/
            //qam_write_byte(u32TunerPort,TS_OUT76_SEL,0x76 );/*config ts_out_7&6bit   在串行配置具体可调整*/
            //qam_write_bit(u32TunerPort, TS_CTRL0, 1, 1);
            break;
        }
    default:
        break;
    }

    g_stTunerOps[u32TunerPort].enTsType = enTsType;

    return HI_SUCCESS;
}

HI_S32 hi3136_DiSEqC_send_msg(HI_U32 u32TunerPort, HI_UNF_TUNER_DISEQC_SENDMSG_S *pstSendMsg)
{
    HI_U8 i = 0;
    HI_U8 u8Temp = 0;
    HI_U8     diseqc_mode = 0;
    HI_U32    diseqc_Ratio = 0;

    qam_read_byte(u32TunerPort, TX_CTRL, &diseqc_mode);
    //diseqc_Ratio = (HI3136_SYSCLK_SAMPLE / 22) & 0xffff;
    diseqc_Ratio = (hi3136_cfg[u32TunerPort].u32CLK_DEMO_kHz / 22) & 0xffff;
    //printk("diseqc_mode = 0x%x, diseqc_Ratio = 0x%x\n", diseqc_mode, diseqc_Ratio);
    qam_write_byte(u32TunerPort, DSEC_RATIO_L, (HI_U8)(diseqc_Ratio & 0xff));
    qam_write_byte(u32TunerPort, DSEC_RATIO_H, (HI_U8)((diseqc_Ratio & 0xff00) >> 8)); //设置22K 时钟


    u8Temp = diseqc_mode & 0x01;   /*diseqc 处于空闲状态，即使diseqc module 复位，屏蔽也可用*/
    qam_write_byte(u32TunerPort, TX_CTRL, u8Temp);


    for (i = 0; i < pstSendMsg->u8Length; i++)
    {
        //printk("i = %d, pstSendMsg->au8Msg[i] = 0x%x\n", i, pstSendMsg->au8Msg[i]);
        qam_write_byte(u32TunerPort, DSEC_DATA, pstSendMsg->au8Msg[i]);
        qam_write_byte(u32TunerPort, DSEC_ADDR, i);
    }

    u8Temp =  (diseqc_mode & 0x01) | (1 << 3) | (((pstSendMsg->u8Length - 1) & 7) << 4);
    //printk("u8Temp = 0x%x\n", u8Temp);
    qam_write_byte(u32TunerPort,TX_CTRL,u8Temp); /*发送num+1个字节,并发送完整diseqc信息*/

    msleep_interruptible(60);   //至少延时30ms

    return HI_SUCCESS;
}

HI_S32 hi3136_tp_verify(HI_U32 u32TunerPort, TUNER_TP_VERIFY_PARAMS_S * pstChannel)
{
    HI_U8 j = 0, m = 0, u8Temp=0, tempL=0, tempH=0, temp_TPnum=0, max_finded = 0, max_reliablity = 0, /*great_reliable = 0,*/ tim_ok_bit = 0, s2_sync_bit = 0, cr_ok_bit = 0, fec_ok_bit = 0,read_reliable=0;
    HI_U32 i=0, fs_cur=0, cbs_fs_sel = 0, time_cnt=0, tim_ok_wait=0, fec_wait_s1=0, fec_ok_wait=0, /*fec_wait_2,*/ fec_wait_all=0, reliablity_temp1=0, reliablity_temp2=0, reliablity_temp3=0, quit_point = 0, fs_grade=0;
    HI_S16 cbs_fc, cbs_fc_sel = 0;
    HI_S32 s32FuncRet = HI_SUCCESS;
    HI_U32 scan_time = 0, scan_wait_s1=0, scan_wait_s2_8=0, scan_wait_s2_q=0, scan_ok_wait=0, pre_reliablity=0, fec_num_all=0, fec_num_cnt = 0, try_enough = 0, tp_all, u32temp=0;
    HI_U8 s2_sync_old = 0, scan_ok_bit = 0, scan_ok_tmp = 0/*, reliable_th*/;

    static HI_U32 hist_cnt=0, hist_fs[3200]={0}, hist_if[3200]={0}, hist_time[3200]={0}, hist_ok[3200]={0}, hist_num=0, low_cn_cnt=0, may_sfu=0, may_sfu_old=0, may_sfu_tmp=0, eq_fs_oked=0;
    HI_U32 if_cur=0, fs_kHz=0, fec_ok_cnt=0, fec_no_ok_cnt=0, point_tmp=0, hist_is_ok=0, hist_no_ok=0, fs_dlt=0, if_dlt=0, equal_fs=0, equal_if=0, eq_fs_no_cnt=0, eq_fs_no_cnt2=0, time_msec=0, time_start=0, time1=0, time2=0, scan_state_old = 0;
    HI_U32 chance_2a=0, chance_2=0, /*try_st1_waited = 0,*/ scan_st1_wait = 0, scan_state=0, fec_num_true=0, s2_fec_resync=0, /*special_fs,*/ find_up, s2_tryed=0;
    HI_S16 cr_cn = 0, cn_th = 230, cn_drop=0;
    //HI_S16 s2_cn_vec[32] = {-300, -235, -124, -30, 100, 223, 310, 403, 468, 518, 620, 642, 550, 662, 791, 935, 1069, 1098, 897, 1021, 1103, 1161, 1289, 1313, 1273, 1364, 1428, 1569, 1605};
    HI_U8 average_num = 6, sum_num = 0, cr_ok_err = 0, low_cn = 0, try_one = 0, try_one_old = 0, modcod = 0, scan_num, fec_num=0, /*freq_inverse,*/ old_rst_wait=0, old_bw_max=0, bw_max_set=0, s1_high_cn=0;
    HI_S32 sum_fc, sum_fs=0, sum_reliable=0, mean_fc = 0, mean_fs = 0, mean_reliable = 0, freq_tr=0, /*freq_loop,*/ s32temp=0;
    HI_S16 s16temp=0, freq_err=0;


    (HI_VOID)HI_DRV_SYS_GetTimeStampMs(&time_start);

    if_cur = *(pstChannel->pu32Frequency);
    fs_cur = *(pstChannel->pu32SymbolRate); //hz
    pre_reliablity = (pstChannel->cbs_reliablity);
    fec_ok_cnt = *(pstChannel->fec_ok_cnt);
    fec_no_ok_cnt =*(pstChannel->fec_no_ok_cnt);
    tp_all = (pstChannel->CBS_TH);      //notice: not match, changed by zouzhiyong

    fs_kHz = fs_cur / 1000;
    if (fs_kHz <  2000)           fs_grade = 0;
    else if (fs_kHz < 3000)       fs_grade = 1;
    else if (fs_kHz < 4900)       fs_grade = 2;
    else if (fs_kHz < 8000)       fs_grade = 3;
    else if (fs_kHz <15000)       fs_grade = 4;
    else                          fs_grade = 5;

    if(fs_kHz < 2000)
        pre_reliablity += pre_reliablity/2;

    if(fs_kHz < 990 || pre_reliablity < 20 - fs_grade)
        return HI_SUCCESS;


    bw_max_set = 0x12;
    cn_drop = 180;
    scan_num = 1;
    fec_num = 2;
    fec_num_all = 3;
    tim_ok_wait = (50 - fs_grade * 5);
    fec_wait_s1 = 90 + 340000/fs_kHz;
    scan_wait_s1 = 110 + 170000/fs_kHz;  //not include state=0 time
    if     (fs_kHz < 1600)            scan_wait_s2_q = 100 + 80000/fs_kHz;
    else if(fs_kHz < 2200)            scan_wait_s2_q = 90 + 80000/fs_kHz;
    else if(fs_kHz < 3000)            scan_wait_s2_q = 80 + 80000/fs_kHz;
    else if(fs_kHz < 5000)            scan_wait_s2_q = 60 + 70000/fs_kHz;
    else if(fs_kHz < 10000)           scan_wait_s2_q = 60 + 70000/fs_kHz;
    else if(fs_kHz < 20000)           scan_wait_s2_q = 50 + 60000/fs_kHz;
    else                              scan_wait_s2_q = 40 + 50000/fs_kHz;


    if     (fs_kHz < 1600)            scan_wait_s2_8 = 90 + 60000/fs_kHz;  //other 220000/fs_khz
    else if(fs_kHz < 2200)            scan_wait_s2_8 = 90 + 60000/fs_kHz;
    else if(fs_kHz < 3000)            scan_wait_s2_8 = 90 + 60000/fs_kHz;
    else if(fs_kHz < 5000)            scan_wait_s2_8 = 80 + 50000/fs_kHz;
    else if(fs_kHz < 10000)           scan_wait_s2_8 = 70 + 50000/fs_kHz;
    else if(fs_kHz < 20000)           scan_wait_s2_8 = 60 + 50000/fs_kHz;
    else                              scan_wait_s2_8 = 40 + 50000/fs_kHz;


    //histroy analysis, begin
    if((fec_ok_cnt == 0)  && (fec_no_ok_cnt == 0))
    {
        hist_cnt += 1;
        may_sfu_old = may_sfu;
        may_sfu = 0;
        low_cn_cnt = 0;

        //discard too old histroy, and move histroy
        point_tmp = 0;
        if(hist_num > 0)
            for(i = 0; i < hist_num; i++)
            {
                if(hist_time[i] + 8 >= hist_cnt)
                {
                    hist_if[point_tmp] = hist_if[i];
                    hist_fs[point_tmp] = hist_fs[i];
                    hist_time[point_tmp] = hist_time[i];
                    hist_ok[point_tmp] = hist_ok[i];
                    point_tmp +=1;
                }
            }
        hist_num = point_tmp;
    }


    //analyse same tp
    point_tmp = 0;
    eq_fs_no_cnt = 0;
    eq_fs_no_cnt2 = 0;
    eq_fs_oked  = 0;
    hist_is_ok = 0;
    hist_no_ok = 0;

    if(hist_num > 0)
        for(i = 0; i < hist_num; i++)
        {
                fs_dlt = abs((HI_S32)hist_fs[i] - (HI_S32)fs_kHz)*1000/fs_kHz;
                if_dlt = abs((HI_S32)hist_if[i] - (HI_S32)if_cur);

                if(fs_dlt < 4)
                {
                    equal_fs = 1;
                    if(hist_time[i] + 4 >= hist_cnt && eq_fs_oked==0)
                    {
                        eq_fs_no_cnt = hist_ok[i]  ?  0  :  (eq_fs_no_cnt + 1);   //only for sfu
                        eq_fs_no_cnt2= hist_ok[i]  ?  0  :  (eq_fs_no_cnt2 + (hist_time[i]== hist_cnt ? 1 : 0));   //only for sfu, current band fs equal
                        eq_fs_oked = hist_ok[i];
                    }
                }
                else
                    equal_fs = 0;

                if(if_dlt < (fs_kHz*15/100+2000))
                    equal_if = 1;
                else
                    equal_if = 0;

                if(equal_if && equal_fs)
                {
                    if(!hist_is_ok && hist_ok[i])
                        hist_is_ok = 1;
                    else if(!hist_no_ok && !hist_ok[i])
                        hist_no_ok = 1;
                }
        }

    if(hist_is_ok)
        hist_no_ok = 0;

    if(fec_ok_cnt >= 2)
    {
        may_sfu = 0;
        may_sfu_old = 0;
    }
    //histroy analysis, end

    //sfu discard
    if(tp_all > 120)
        may_sfu_tmp = (low_cn_cnt > 8);
    else if(tp_all > 85)
        may_sfu_tmp = (low_cn_cnt > 12);
    else if(tp_all > 60 || may_sfu_old)
        may_sfu_tmp = (low_cn_cnt > 14) && (eq_fs_no_cnt > 10);
    else if(tp_all > 48)
        may_sfu_tmp = (low_cn_cnt > 20) && (eq_fs_no_cnt > 12);
    else
        may_sfu_tmp = (low_cn_cnt > 30) && (eq_fs_no_cnt > 15);

    may_sfu = may_sfu ||  ((fec_ok_cnt <= 1) && may_sfu_tmp);
    u32temp =   eq_fs_no_cnt/6 + eq_fs_no_cnt2/3;
    reliablity_temp1 = pre_reliablity + 10 - fs_grade * 2;
    reliablity_temp2 = (reliablity_temp1 > low_cn_cnt/2)   ?   (reliablity_temp1 - low_cn_cnt/2)  :  0;
    reliablity_temp3 = (reliablity_temp2 > u32temp)   ?   (reliablity_temp2 - u32temp)  :  0;

    mean_reliable = pre_reliablity;  //temp, only for right print
    i = 0;
    m = 0;
    j = 0;

    if((may_sfu && reliablity_temp3  < 50)  || (fec_ok_cnt==2 && low_cn_cnt > 40 && reliablity_temp3 < 25))
    {
        //printk("goto return_proc\n");
        goto return_proc;
    }

    if(may_sfu)
    {
        scan_num = 1;
        fec_num = 1;
        cn_drop = 80;
    }

    qam_write_byte(u32TunerPort, LDPC_ITER_ADDR, 0x50);

    hi3136_setlpf(u32TunerPort, (fs_cur)/10*9);
    if (g_stTunerOps[u32TunerPort].set_tuner)
    {
        s32FuncRet = g_stTunerOps[u32TunerPort].set_tuner(u32TunerPort, g_stTunerOps[u32TunerPort].enI2cChannel, *(pstChannel->pu32Frequency) );
        if (HI_SUCCESS != s32FuncRet)
            return s32FuncRet;
    }

    tuner_mdelay(8);


    if        (fs_kHz < 1800)           average_num = 4;
    else if(fs_kHz < 2800)           average_num = 5;
    else if(fs_kHz < 4100)           average_num = 5;
    else if(fs_kHz < 6000)           average_num = 7;
    else if(fs_kHz < 8200)           average_num = 8;
    else if(fs_kHz <14000)          average_num = 10;
    else if(fs_kHz <21000)          average_num = 12;
    else                                         average_num = 14;

    qam_read_byte (u32TunerPort, 0x22, &old_rst_wait);
    qam_write_byte(u32TunerPort, 0x22, 0xc0);
    qam_read_byte (u32TunerPort, 0x58, &old_bw_max);

    for (i = 0; i < 3; i++)
    {
        sum_num = 0;
        sum_fc = 0;
        sum_fs = 0;
        sum_reliable = 0;
        low_cn = 0;
        cr_ok_err = 0;
        try_one = 0;
        try_one_old = 0;
        s2_sync_bit= 0;
        s2_sync_old = 0;
        scan_ok_bit = 0;
        scan_ok_tmp = 0;
        quit_point = 0;

        qam_write_byte(u32TunerPort, 0x58, bw_max_set);

        for(m=0; m<average_num; m++) //多次盲扫求载波误差均值
        {
            //set key register
            if((m == 0) || (try_one != try_one_old))
            {
                try_one_old = try_one;
                qam_write_byte(u32TunerPort, CBS_CTRL_RDADDR, 0xc0);    //已知频点及符号率的盲扫
                hi3136_set_symrate(u32TunerPort, fs_cur );
                qam_write_byte(u32TunerPort, CENT_FREQ_L, 0x00);    //hi3136_set_symrate中含载波中心偏移，但盲扫确认时不应加偏移
                qam_write_byte(u32TunerPort, CENT_FREQ_H, 0x00);
            }

            //reset system or module
            if(m==0)
                hi3136_HotReset_CHIP(u32TunerPort);
            else
            {
                qam_write_byte(u32TunerPort, 0x20, 0x81);   //reset module except agc and fsk
                qam_write_byte(u32TunerPort, 0x20, 0xff);        //remove reset
            }

            //wait cbs finish
            time_cnt = 0;
            do      //等待盲扫完成
            {
                //tuner_mdelay(2);
                //time_cnt += 2;
                tuner_mdelay(5);
                time_cnt += 5;
                qam_read_bit(u32TunerPort, CBS_FINISH, 0, &u8Temp);
            }
            while(!u8Temp && (time_cnt < 300));

            //read TP num
            temp_TPnum = 0;
            if(u8Temp)  //cbs_finish 标志位1表示已完成，再读TP个数
            {
                qam_read_byte(u32TunerPort, CBS_R2_NUM, &tempL);
                temp_TPnum = tempL & 0x1f;
            }

            if(temp_TPnum != 0) //TP>=1 即盲扫成功，并开始已知Fs Fc进行盲扫，读取相应的TP数，找可靠度最高的TP
            {
                max_finded = 0;
                max_reliablity = 0;

                //find max reliable
                for(j = 0; j < temp_TPnum; j++)     //已知Fs Fc情况下，盲扫信息参数
                {
                    qam_write_byte(u32TunerPort, CBS_CTRL_RDADDR, 0xc0 | j);    //低5位指定了要读哪个TP
                    qam_read_byte(u32TunerPort, CBS_RELIABLITY, &read_reliable);
                    qam_read_byte(u32TunerPort, CBS_FC_L, &tempL);
                    qam_read_byte(u32TunerPort, CBS_FC_H, &tempH);
                    cbs_fc = (HI_S16) (tempL + (tempH<<8));
                    if((abs(cbs_fc) < (fs_kHz*25/100+700))  &&  (read_reliable > max_reliablity))   //前提：载波误差小于0.25倍符号率????
                    {
                        max_finded = 1;
                        max_reliablity = read_reliable;
                        qam_read_byte(u32TunerPort, CBS_FS_L, &tempL);
                        qam_read_byte(u32TunerPort, CBS_FS_H, &tempH);
                        cbs_fs_sel = tempL + (tempH<<8);
                        cbs_fc_sel = cbs_fc;
                    }
                }

                //average cbs_fs and reliablity
                if(max_finded)
                {
                    sum_num +=1;
                    sum_fc += (HI_S32)cbs_fc_sel;
                    sum_fs += cbs_fs_sel;
                    max_reliablity += (((fs_kHz > 2000) || max_reliablity > 160)  ?  0 : max_reliablity/2);
                    sum_reliable += max_reliablity;
                    mean_fc = sum_fc/sum_num;
                    mean_fs = sum_fs/sum_num;
                    mean_reliable = sum_reliable/sum_num;
                    //printk("%s, %d, cbs_fc_sel=%d, sum_fc=%d, reliable=%d, sum_num=%d  \n", __func__, __LINE__, cbs_fc_sel, sum_fc,max_reliablity, sum_num);
                }

                //try once to find wether is dvb-s2 (check s2_sync_bit)
                if(max_finded && !try_one && (fs_kHz >= 3000 || may_sfu))
                {
                    try_one = 1;
                    qam_write_byte(u32TunerPort, CBS_CTRL_RDADDR, 0x00);    //关闭盲扫，Ram地址为0
                    hi3136_set_symrate(u32TunerPort, mean_fs*1000);
                    qam_write_byte(u32TunerPort, CENT_FREQ_L, mean_fc & 0xff);
                    qam_write_byte(u32TunerPort, CENT_FREQ_H, (mean_fc>>8) & 0xff);
                    hi3136_HotReset_CHIP(u32TunerPort);      //reset man_ctrl state

                    //wait for timing ok
                    time_cnt = 0;
                    do      //等待定时OK
                    {
                        //tuner_mdelay(2);
                        //time_cnt +=2;
                        tuner_mdelay(5);
                        time_cnt += 5;
                        qam_read_bit(u32TunerPort, LOCK_FLAG, 2, &tim_ok_bit);
                    }while(!tim_ok_bit && (time_cnt < tim_ok_wait));

                    if(tim_ok_bit)
                    {
                        time_cnt = 0;
                        do      //等待SCAN-OK
                        {
                            tuner_mdelay(5);
                            time_cnt += 5;
                            qam_read_byte(u32TunerPort, 0x5b, &u8Temp);   //scan_state
                            s2_sync_bit  = (u8Temp>>0) & 0x1;
                            scan_ok_tmp= (u8Temp>>1) & 0x1;

                            if(!scan_ok_bit && scan_ok_tmp)
                            {
                                scan_ok_bit = scan_ok_tmp;
                                scan_time = time_cnt;
                            }

                            // read CN and compare CN
                            u32temp = may_sfu ? 180000 : 220000;
                            if(time_cnt > u32temp/fs_kHz)
                            {
                                qam_read_byte(u32TunerPort, 0x5a, &u8Temp);
                                cr_cn = (u8Temp>=128)   ?   (u8Temp - 256)  :  u8Temp;
                                cr_cn *= 1875/100;      //unit=0.01 dB

                                if(cr_cn < (cn_th - 400) && !s2_sync_bit)
                                {
                                    quit_point = 1;
                                    low_cn = 1;
                                    low_cn_cnt += 1;
                                }
                                break;
                            }
                        }while(time_cnt < 20000);
                    }
                    else
                    {
                        quit_point = 2;
                        low_cn = 1;
                        low_cn_cnt +=1;
                    }
                }
            }
            if(low_cn || s2_sync_bit || scan_ok_bit)
                break;
        }

        if(!low_cn && m==average_num && sum_num < average_num*3/4)
        {
             quit_point = 3;
             low_cn = 1;
             low_cn_cnt +=1;
        }

        (HI_VOID)HI_DRV_SYS_GetTimeStampMs(&time1);
        time_msec =  time1 - time_start;
        //printk("%s, %d, average finish : sfu=%d, eq_fs_no_cnt=%d, hist_is_ok=%d, hist_no_ok=%d, cbs_fc_sel=%d, mean_fc=%d, sum_num=%d, cr_cn_st1=%d, cn_th=%d, low_cn=%d, m=%d, try_one=%d,  tim_ok_bit=%d, time=%d \n", __func__, __LINE__, may_sfu, eq_fs_no_cnt, hist_is_ok, hist_no_ok, cbs_fc_sel, mean_fc, sum_num, cr_cn_st1, cn_th, low_cn, m, try_one, tim_ok_bit, time_msec);

        if(!low_cn)
        {
            //if is dvb-s2, needn't restart system,   else reset fs and fc, then restart
            if(!s2_sync_bit && !scan_ok_bit)
            {
                qam_write_byte(u32TunerPort, CBS_CTRL_RDADDR, 0x00);    //关闭盲扫，Ram地址为0
                hi3136_set_symrate(u32TunerPort, mean_fs*1000);
                qam_write_byte(u32TunerPort, CENT_FREQ_L, mean_fc & 0xff);
                qam_write_byte(u32TunerPort, CENT_FREQ_H, (mean_fc>>8) & 0xff);
                hi3136_HotReset_CHIP(u32TunerPort);      //reset man_ctrl state
            }

            for(m=0; m<scan_num; m++)
            {
                if(m>0)
                {
                    qam_write_byte(u32TunerPort, 0x58, bw_max_set);
                    hi3136_HotReset_CHIP(u32TunerPort);      //reset man_ctrl state
                    s2_sync_bit= 0;
                    s2_sync_old = 0;
                    scan_ok_bit = 0;
                    scan_ok_tmp = 0;
                }

                time_cnt = 0;
                do      //等待定时OK
                {
                    //tuner_mdelay(2);
                    //time_cnt +=2;
                    tuner_mdelay(5);
                    time_cnt += 5;
                    qam_read_bit(u32TunerPort, LOCK_FLAG, 2, &tim_ok_bit);
                }while(!tim_ok_bit && (time_cnt < (tim_ok_wait + 10)));  //add 10ms for agc


                if(tim_ok_bit)
                {
                    s2_tryed = 0;
                    scan_state_old = 0;
                    scan_st1_wait = 0;
                    time_cnt = 0;
                    do      //等待SCAN-OK
                    {
                        tuner_mdelay(5);
                        time_cnt += 5;
                        qam_read_byte(u32TunerPort, 0x5b, &u8Temp);   //scan_state
                        scan_state = (u8Temp>>6) & 0x3;
                        s2_sync_bit  = (u8Temp>>0) & 0x1;
                        scan_ok_tmp= (u8Temp>>1) & 0x1;

                        if(!scan_ok_bit && scan_ok_tmp)
                        {
                            scan_ok_bit = scan_ok_tmp;
                            scan_time = time_cnt;
                        }

                        //read modcod
                        if(s2_sync_bit)
                        {
                            qam_read_byte(u32TunerPort, 0x5c, &u8Temp); //read modcod of pls_code
                            modcod = (u8Temp>>2) & 0x1f;
                        }

                        //compare CN
                        qam_read_byte(u32TunerPort, 0x5a, &u8Temp);
                        cr_cn = (u8Temp>=128)   ?   (u8Temp - 256)  :  u8Temp;
                        cr_cn *= 1875/100;      //unit=0.01 dB

                        if(cr_cn < (cn_th - 400) && !s2_sync_bit && scan_state>0)
                        {
                            quit_point = 8;
                            low_cn = 1;
                            low_cn_cnt += 1;
                            break;
                        }

                        if(cr_cn > 800 && !s2_sync_bit && scan_state>0)
                        {
                            s1_high_cn = 1;
                            scan_num = 5;
                        }

                        if(scan_state==0 && scan_state_old!=0)
                        {
                            if((!s2_sync_bit || s2_tryed) && !s1_high_cn)
                            {
                                quit_point = 9;
                                break;
                            }
                            s2_tryed = 1;
                        }
                        scan_state_old = scan_state;

                        if(scan_state>0)
                            scan_st1_wait += 5;
                        else
                             scan_st1_wait = 0;

                        scan_ok_wait = s2_sync_bit   ?   (modcod >=12 ? scan_wait_s2_8 : scan_wait_s2_q)   :   scan_wait_s1;
                        scan_ok_wait = s2_sync_bit && (modcod<=3 || modcod==12 || modcod==13 || modcod==18  || modcod==24)  ?  scan_ok_wait * 15/10  :  scan_ok_wait;
                    }while(!scan_ok_bit && (time_cnt < 1500) && (scan_st1_wait < scan_ok_wait));

                    if(!scan_ok_bit)
                        scan_time = time_cnt;

                    if(low_cn)
                        break;

                    s2_sync_old = s2_sync_bit;

                    qam_write_byte(u32TunerPort, 0x58, old_bw_max);

                    fec_num_true = (i==0)  ?  fec_num  :  1;
                    s2_fec_resync = 0;
                    if(scan_ok_bit)
                        //wait for fec ok,  give one chance, defined by fec_num
                        for(j=0; j<fec_num_true; j++)
                        {
                            time_cnt = 0;
                            do      //等待FEC-OK
                            {
                                tuner_mdelay(10);
                                time_cnt += 10;
                                fec_wait_all += 10;
                                qam_read_byte(u32TunerPort, LOCK_FLAG, &u8Temp);
                                s2_sync_bit= (u8Temp>>3) & 0x1;
                                cr_ok_bit    = (u8Temp>>4) & 0x1;
                                fec_ok_bit  = (u8Temp>>5) & 0x1;

                                if(j>0  && !cr_ok_bit)
                                {
                                    quit_point = 4;
                                    cr_ok_err = 1;
                                    break;
                                }

                                // get modcod
                                if(s2_sync_bit)
                                {
                                    if(!s2_sync_old)
                                    {
                                        time_cnt = 0;
                                        s2_fec_resync = 1;
                                    }
                                    s2_sync_old = s2_sync_bit;
                                }

                                //read and compare CN
                                qam_read_byte(u32TunerPort, 0x5a, &u8Temp);
                                cr_cn = (u8Temp>=128)   ?   (u8Temp - 256)  :  u8Temp;
                                cr_cn *= 1875/100;

                                //printk("%s, %d, fec_loop=%d, cr_cn=%d, cn_th=%d, s2_syn=%d, modcod=%d\n", __func__, __LINE__,j, cr_cn, cn_th, s2_sync_bit, modcod);
                                u32temp = may_sfu ? 140000 : 200000;
                                if((cr_cn < (cn_th - cn_drop)) && !s2_sync_bit  &&  (j>0 || time_cnt > (20+u32temp/fs_kHz)))
                                {
                                    quit_point = 5;
                                    low_cn = 1;
                                    low_cn_cnt += 1;
                                }
                                if(cr_ok_err || low_cn)
                                    break;
                                fec_ok_wait = (s2_sync_bit  ?  fec_wait_s1/2  :  fec_wait_s1)   +   (j==0 && s2_fec_resync  ?  (100 + 220000/fs_kHz)  :  0);
                            }while(!fec_ok_bit && (time_cnt < fec_ok_wait));

                            fec_num_cnt += 1;
                            if(fec_num_cnt >= fec_num_all && !fec_ok_bit)
                            {
                                quit_point = 6;
                                try_enough = 1;
                            }

                            if(cr_ok_err || fec_ok_bit || low_cn || try_enough)
                                break;
                            else
                            {
                                qam_write_byte(u32TunerPort, 0x20, 0x9f); //reset fec and outp
                                qam_write_byte(u32TunerPort, 0x20, 0xff);
                            }
                        }
                }
                else
                {
                    quit_point = 7;
                    low_cn = 1;
                    low_cn_cnt += 1;
                }
/*
                qam_read_byte(u32TunerPort, 0x66, &tempL);
                qam_read_byte(u32TunerPort, 0x67, &tempH);
                s16temp = tempL + (tempH<<8);
                s32temp = (HI_S32)(s16temp);
                freq_tr = (s32temp * (HI_S32)(fs_kHz))/65536;
                qam_read_byte(u32TunerPort, 0x68, &tempL);
                qam_read_byte(u32TunerPort, 0x69, &tempH);
                freq_loop = tempL + (tempH<<8);
*/
            //printk("%s, %d, freq_tuner=%d, mean_fc=%d, freq_tr=%d, freq_loop=%d \n", __func__, __LINE__,if_cur/1000, mean_fc, freq_tr, freq_loop);
            //printk("%s, %d, freq=%d, sym=%d, loop=%d, scan_lp=%d, fec_lp=%d, reliable=%d, low_cn=%d, sfu=%d, hist_no=%d, hist_ok=%d, scan_ok=%d, fec_ok=%d , scan_time=%d,  time=%d, quit_point=%d\n", __func__, __LINE__,if_cur, fs_kHz, i, m, j, mean_reliable, low_cn, may_sfu, hist_no_ok, hist_is_ok, scan_ok_bit, fec_ok_bit, scan_time, time_cnt,quit_point);

            if(cr_ok_err || fec_ok_bit || low_cn || try_enough)
                break;
            }
        }

        find_up = fec_ok_cnt > 0  ?  10  :  0;
        if       (fs_kHz <1500)        chance_2a = mean_reliable > (52 + find_up + low_cn_cnt*15/10);
        else if(fs_kHz <2000)        chance_2a = mean_reliable > (47 + find_up + low_cn_cnt*13/10);
        else if(fs_kHz <3000)        chance_2a = mean_reliable > (44 + find_up + low_cn_cnt*11/10);
        else if(fs_kHz <5000)        chance_2a = mean_reliable > (42 + find_up + low_cn_cnt);
        else                                 chance_2a = mean_reliable > (40 + find_up + low_cn_cnt);

        chance_2 = cr_ok_err || (!scan_ok_bit && chance_2a);
        if(fec_ok_bit || ((i==0) && !hist_is_ok && !s1_high_cn && (low_cn  || try_enough ||may_sfu  || !chance_2)) || ((i==1) && (!s1_high_cn ||try_enough))) //不满足再试条件
            break;
     }


    qam_write_byte(u32TunerPort, 0x22, old_rst_wait);
    qam_write_byte(u32TunerPort, 0x58, old_bw_max);

return_proc:

//renew histroy
    if(hist_num<3200)
    {
        hist_fs[hist_num] = fs_kHz;
        hist_if[hist_num] = if_cur;
        hist_ok[hist_num] = fec_ok_bit;
        hist_time[hist_num] = hist_cnt;
        hist_num += 1;
    }


    if(!s2_sync_bit && !scan_ok_bit && !low_cn)
        low_cn_cnt += 1;

   if(fec_ok_bit)
    {
        //read spectrum inverse bit,  read frequency error,  renew the if frequency
/*        if(s2_sync_bit)
            qam_read_bit(u32TunerPort, 0x5d, 0, &freq_inverse);      //spectrum inverse for dvb-s2
        else
            qam_read_bit(u32TunerPort, 0x8f, 2, &freq_inverse);     //spectrum inverse for dvb-s1
*/
        qam_read_byte(u32TunerPort, 0x66, &tempL);
        qam_read_byte(u32TunerPort, 0x67, &tempH);

        s16temp = tempL + (tempH<<8);
        s32temp = (HI_S32)(s16temp);
        freq_tr = (s32temp * (HI_S32)(fs_kHz))/65536;
        freq_err = mean_fc  + freq_tr;

        if(hi3136_cfg[u32TunerPort].enIQSpectrum)
            freq_err = -freq_err;  //具体根据实际硬件配置来
        *(pstChannel->pu32Frequency)  += freq_err;
        *(pstChannel->pu32SymbolRate) = mean_fs*1000;
        if_cur = *(pstChannel->pu32Frequency);
        //printk("%s, %d, if=%d, mean_fc=%d, freq_tr=%d, freq_err=%d, freq_inverse=%d, new_freq=%d, new_fs=%d\n", __func__, __LINE__,if_cur, mean_fc, freq_tr, freq_err, freq_inverse, *(pstChannel->pu32Frequency), *(pstChannel->pu32SymbolRate));
    }

    (HI_VOID)HI_DRV_SYS_GetTimeStampMs(&time2);
    time_msec =  time2 - time_start;
    //printk("if=%8d,fs=%5d,lp=%d,scanlp=%d,feclp=%d,reliab=%3d,low_cn=%d,hno=%d,hok=%d,scan_ok=%d,  fec_ok= %d , scan_t=%3d,fec_t=%4d,time=%4d,quit=%d,eqfsno=%2d,s2=%d,mc=%2d,cn=%3d\n",if_cur, fs_kHz, i, m, fec_num_cnt, mean_reliable, low_cn, hist_no_ok, hist_is_ok, scan_ok_bit, fec_ok_bit, scan_time, fec_wait_all, time_msec,quit_point, eq_fs_no_cnt, s2_sync_bit, modcod, cr_cn/10);


   if(fec_ok_bit)
    {
        *(pstChannel->fec_ok_cnt) += 1;
        return HI_SUCCESS;
   }
    else
    {
        *(pstChannel->fec_no_ok_cnt) += 1;
        return HI_SUCCESS;
    }
}

HI_S32 hi3136_set_ts_out(HI_U32 u32TunerPort, HI_UNF_TUNER_TSOUT_SET_S *pstTSOut)
{
    HI_U8 u8Temp = 0, i=0;
    HI_TUNER_CHECKPOINTER( pstTSOut);

    /* Check tuner port and init. */
    if (u32TunerPort >= TUNER_NUM)
    {
        HI_INFO_TUNER("Bad u32TunerPort\n");
        return HI_FAILURE;
    }

    for (i = 0; i < MAX_TS_LINE; i++)
    {
        if (HI_UNF_TUNER_OUTPUT_BUTT <= pstTSOut->enTSOutput[i])
        {
            HI_INFO_TUNER("Error pin define!\n");
            return HI_FAILURE;
        }
    }

    qam_read_byte(u32TunerPort, TS_PARALL_CTRL, &u8Temp);
    u8Temp = u8Temp & 0xf0;
    u8Temp = u8Temp | (pstTSOut->enTSOutput[10] & 0xf);
    qam_write_byte(u32TunerPort, TS_PARALL_CTRL, u8Temp);

    qam_write_byte(u32TunerPort, TS_OUT98_SEL, ((pstTSOut->enTSOutput[9]& 0xf)<<4) | pstTSOut->enTSOutput[8]);/*config ts_out_9&8bit*/
    qam_write_byte(u32TunerPort, TS_OUT76_SEL, ((pstTSOut->enTSOutput[7]& 0xf)<<4) | pstTSOut->enTSOutput[6]);/*config ts_out_7&6bit*/
    qam_write_byte(u32TunerPort, TS_OUT54_SEL, ((pstTSOut->enTSOutput[5]& 0xf)<<4) | pstTSOut->enTSOutput[4]);/*config ts_out_5&4bit*/
    qam_write_byte(u32TunerPort, TS_OUT32_SEL, ((pstTSOut->enTSOutput[3]& 0xf)<<4) | pstTSOut->enTSOutput[2]);/*config ts_out_3&2bit*/
    qam_write_byte(u32TunerPort, TS_OUT10_SEL, ((pstTSOut->enTSOutput[1]& 0xf)<<4) | pstTSOut->enTSOutput[0]);/*config ts_out_1&0bit*/

    return HI_SUCCESS;
}


HI_S32 hi3136_get_freq_symb_offset(HI_U32 u32TunerPort, HI_U32 * pu32Freq, HI_U32 * pu32Symb )
{
    HI_U8 u8Temp_L = 0, u8Temp_H = 0;
    HI_S32 s32Temp = 0;

    HI_TUNER_CHECKPOINTER( pu32Freq);
    HI_TUNER_CHECKPOINTER( pu32Symb);

    qam_read_byte(u32TunerPort, FS_OFFSET_FC_L, &u8Temp_L);
    qam_read_byte(u32TunerPort, FS_OFFSET_FC_H, &u8Temp_H);
    s32Temp = u8Temp_H * 256 + u8Temp_L;
    if (s32Temp >= 32768)
    {
        s32Temp -= 65536;
    }
    *pu32Symb = s32Temp * hi3136_cfg[u32TunerPort].u32CLK_DEMO_kHz/(1<<22);

    qam_read_byte(u32TunerPort, FREQ_OFFSET_FC_L, &u8Temp_L);
    qam_read_byte(u32TunerPort, FREQ_OFFSET_FC_H, &u8Temp_H);
    s32Temp = u8Temp_H * 256 + u8Temp_L;
    if (s32Temp >= 32768)
    {
        s32Temp -= 65536;
    }
    *pu32Freq = s32Temp * hi3136_cfg[u32TunerPort].u32CLK_DEMO_kHz/(1<<17) ;

    return HI_SUCCESS;
}

HI_S32 hi3136_send_tone(HI_U32 u32TunerPort, HI_U32 u32Tone)
{
    return HI_SUCCESS;
}

//sample data of AD or EQU
HI_S32 hi3136_data_sample(HI_U32 u32TunerPort, TUNER_DATA_SRC_E enDataSrc, HI_U32 u32DataLen, HI_UNF_TUNER_SAMPLE_DATA_S *pstData)
{
    HI_U8 agc_ctrl =0 , u8Temp = 0, data_l =0, data_m=0, data_h =0;
    HI_U16 u16TimeOut = 0;
    HI_U32 i=0, u32Temp = 0;

    //config enable ctrl
    qam_write_byte(u32TunerPort, 0xe0, 0);
    qam_write_byte(u32TunerPort, 0xe1, 0);
    qam_write_byte(u32TunerPort, 0xe2, 1);
    qam_write_byte(u32TunerPort, 0xe3, 0);
    qam_write_byte(u32TunerPort, 0xe4, 0);

    //config trig ctrl
    qam_write_byte(u32TunerPort, 0xe5, 0);
    qam_write_byte(u32TunerPort, 0xe6, 0);
    qam_write_byte(u32TunerPort, 0xe7, 1);
    qam_write_byte(u32TunerPort, 0xeb, 0);
    qam_write_byte(u32TunerPort, 0xec, 0);

    //config sample num
    qam_write_byte(u32TunerPort, 0xe8, (u32DataLen & 0xff));
    qam_write_byte(u32TunerPort, 0xe9, (u32DataLen & 0xff00) >> 8);

    switch (enDataSrc)
    {
    case TUNER_DATA_SRC_ADC:
        qam_write_byte(u32TunerPort, 0x2f, 0x30);  //select la_sel to adc, for FFT
        break;
    case TUNER_DATA_SRC_EQU:
        qam_write_byte(u32TunerPort, 0x2f, 0x7c);  //select la_sel to equ, for display constellation
        break;
    default:
        HI_INFO_TUNER("Sample data source error!\n");
        return HI_FAILURE;
    }

    //select low 32bit data from vector
    for (i = 0; i < 0x10; i++)
    {
        qam_write_byte(u32TunerPort, 0xf0 + i, i);
    }

    //judge whether agc ok, if not, return
#if 0
    u16TimeOut = 0;
    do
    {
        msleep_interruptible(2);
        qam_read_byte(u32TunerPort, AGC_CTRL_L, &agc_ctrl[0]);
        qam_read_byte(u32TunerPort, AGC_CTRL_H, &agc_ctrl[1]);
        u16TimeOut += 2;
    } while((agc_ctrl[1] < 128) && (u16TimeOut < 10));
#endif

    qam_read_byte(u32TunerPort, AGC_CTRL_H, &agc_ctrl);
    if (!(agc_ctrl & (1 << 7)))
    {
        HI_INFO_TUNER("AGC not ok!\n");
        return HI_FAILURE;
    }

    //set catch_data_start
    //first reset logic
    qam_write_byte(u32TunerPort, 0xea, 0);
    tuner_mdelay(5);
    qam_write_byte(u32TunerPort, 0xea, 0x20);

    //wait for catch data finished
    u16TimeOut = 0;
    do
    {
        tuner_mdelay(2);
        qam_read_byte(u32TunerPort, 0xea, &u8Temp);
        u16TimeOut += 2;
    } while(!(u8Temp & (1 << 6)) && (u16TimeOut < 20));

    //read sample data
    qam_write_byte(u32TunerPort, 0xed, 0);
    qam_write_byte(u32TunerPort, 0xee, 0);

    if (TUNER_DATA_SRC_ADC == enDataSrc)
    {
        for (i = 0; i < u32DataLen; i++)
        {
            qam_read_byte(u32TunerPort, 0xef, &data_l);
            qam_read_byte(u32TunerPort, 0xef, &data_m);
            qam_read_byte(u32TunerPort, 0xef, &data_h);
            qam_read_byte(u32TunerPort, 0xef, &u8Temp);

            pstData[i].s32DataIP = ((data_m & 0x01) << 8) + data_l - 256;
            u32Temp = ((data_h & 0x03) << 8) + data_m;
            pstData[i].s32DataQP = (u32Temp >> 1) - 256;
        }
    }
    else
    {
        for (i = 0; i < u32DataLen; i++)
        {
            qam_read_byte(u32TunerPort, 0xef, &data_l);
            qam_read_byte(u32TunerPort, 0xef, &data_m);
            qam_read_byte(u32TunerPort, 0xef, &data_h);
            qam_read_byte(u32TunerPort, 0xef, &u8Temp);

            u32Temp = ((data_m & 0x0f) << 8) + data_l;
            pstData[i].s32DataQP = (u32Temp >> 4);
            if(pstData[i].s32DataQP >= 128)
            {
                pstData[i].s32DataQP -= 256;
            }

            u32Temp = ((data_h & 0x0f) << 8) + data_m;
            pstData[i].s32DataIP = (u32Temp >> 4);
            if(pstData[i].s32DataIP >= 128)
            {
                pstData[i].s32DataIP -= 256;
            }
        }
    }

    return HI_SUCCESS;
}

