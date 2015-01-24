/***********************************************************************************
 *              Copyright 2004 - 2050, Hisilicon Tech. Co., Ltd.
 *                           ALL RIGHTS RESERVED
 * FileName: hi_tuner.c
 * Description:
 *
 * History:
 * Version   Date             Author       DefectNum    Description
 * main\1    2007-08-03   w54542      NULL            Create this file.
 * main\1    2007-11-10   w54542      NULL            modify this file.
 ***********************************************************************************/
//#include <linux/config.h>

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <asm/atomic.h>
#include <linux/miscdevice.h>
#include <asm/io.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/pm.h>

#include "hi_type.h"

#include "drv_i2c_ext.h"
#include "drv_gpioi2c_ext.h"
#include "drv_gpio_ext.h"
#include "hi_drv_gpioi2c.h"
#include "hi_debug.h"


#include "hi_drv_stat.h"
#include "hi_drv_dev.h"
#include "hi_drv_reg.h"
#include "hi_kernel_adapt.h"
#include "hi_drv_proc.h"
#include "hi_drv_module.h"

#include "drv_tuner_ext.h"
#include "drv_tuner_ioctl.h"
#include "hi_drv_diseqc.h"
#include "drv_demod.h"

#include "cd1616.h"
#include "alps_tdae.h"
#include "tda18250.h"
#include "tda18250b.h"
#include "tmx7070x.h"
#include "mxl203.h"
#include "tdcc.h"
#include "xg_3bl.h"
#include "mt_2081.h"
#include "mxl603.h"
#include "r_820c.h"
#include "av2011.h"
#include "sharp_qm1d1c004x.h"
#include "RDA5815.h"
#include "M88TS2022.h"
#include "hi_unf_i2c.h"

#include "isl9492.h"
#include "mps8125.h"
#include "tda6651.h"

HI_DECLARE_MUTEX(g_TunerMutex);
static atomic_t s_atomicTunerAvailble = ATOMIC_INIT(0);

static HI_U32 s_u32InsideQamPort = 0;
static HI_U32 s_u32TunerAvailble[TUNER_NUM *10] = {0};
static HI_U32 s_u32ResetGpioNo[TUNER_NUM] = {0};
static struct stSatTunerResumeInfo
{
    TUNER_LNB_OUT_E enLNBOut;
    HI_U32 u32Continuous22K;
    HI_UNF_TUNER_SAT_ATTR_S stSatTunerAttr;
} s_stSatTunerResumeInfo[TUNER_NUM] = {{0}, {0}, {0},{0},{0}};

TUNER_OPS_S g_stTunerOps[TUNER_NUM] = {{0},{0},{0},{0},{0}};

//TUNER_LNBCONFIG_S Data_8115c680[TUNER_NUM];

HI_U32 g_xtal_clk_table[TUNER_NUM] = {XTAL_CLK, XTAL_CLK, XTAL_CLK, XTAL_CLK, XTAL_CLK};
HI_U32 g_demod_address[TUNER_NUM] = {QAM_PORT0_ADDR, QAM_PORT1_ADDR, QAM_PORT2_ADDR, QAM_PORT3_ADDR, QAM_PORT4_ADDR};
HI_U32 g_halftuner_address[TUNER_NUM] = {TUNER_PORT0_ADDR, TUNER_PORT1_ADDR, TUNER_PORT2_ADDR, TUNER_PORT3_ADDR, TUNER_PORT3_ADDR};

 HI_U32 u32Cgr57RegVal = 0;
 HI_U32 u32Cgr58RegVal = 0;
 HI_U32 u32QAMAddrRegVal = 0;
 HI_U32 u32TsCKRegVal = 0;
 HI_U32 g_u32CrgEnableFlag = 0;

 HI_U32 Data_80fb63a4 = 11700;

static HI_UNF_TUNER_LOCK_STATUS_E s_enTunerStat[TUNER_NUM] =
{
    HI_UNF_TUNER_SIGNAL_DROPPED,
    HI_UNF_TUNER_SIGNAL_DROPPED,
    HI_UNF_TUNER_SIGNAL_DROPPED,
    HI_UNF_TUNER_SIGNAL_DROPPED,
    HI_UNF_TUNER_SIGNAL_DROPPED
};

HI_BOOL g_bLockFlag = HI_FALSE;
wait_queue_head_t g_QamSpecialProcessWQ;
static struct task_struct *pThread;

static I2C_EXT_FUNC_S *s_pI2cFunc   = HI_NULL;
static GPIO_EXT_FUNC_S *s_pGpioFunc = HI_NULL;
static GPIO_I2C_EXT_FUNC_S *s_pGpioI2cFunc = HI_NULL;

/*static*/ HI_U32 Data_8115cde0; //8115cde0

typedef struct
{
    HI_UNF_TUNER_FE_LNB_CONFIG_S    stLNBConfig;/* LNB configuration */
    HI_UNF_TUNER_FE_LNB_POWER_E     enLNBPower; /* LNB power */
    HI_UNF_TUNER_FE_POLARIZATION_E  enPolar;    /* LNB polarization */
    HI_UNF_TUNER_FE_LNB_22K_E       enLNB22K;   /* LNB 22K on or off */
    HI_UNF_TUNER_SWITCH_22K_E       enSwitch22K;
    HI_UNF_TUNER_SWITCH_TONEBURST_E enToneBurst;
} TUNER_STATUS_SAT_S;

static TUNER_STATUS_SAT_S s_stMCESatPara[TUNER_NUM];

extern HI_VOID HI_DRV_SYS_GetChipVersion(HI_CHIP_TYPE_E *penChipType, HI_U32 *pu32ChipVersion);

/* check tuner port whether it's in valid range */
static HI_S32 tuner_check_port(HI_U32 u32TunerPort)
{
    if (u32TunerPort >= TUNER_NUM)
    {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/* register tuner standard interface function */
HI_S32 tuner_select_type(HI_U32 u32TunerPort, HI_TunerAttr_S *pstTunerAttr,
                         HI_DemodAttr_S *pstDemodAttr, HI_U32 u32ResetGpioNo)
{
	HI_U8 u8ChipReset = 0;
    HI_U8 u8DataTmp = 0;
    HI_U32 u32RegVal = 0;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHIP_TYPE_E enChipType = HI_CHIP_TYPE_BUTT;
    HI_U32 u32ChipVersion = 0;
#if defined (CHIP_TYPE_hi3716cv200es) || defined (CHIP_TYPE_hi3716cv200) || defined (CHIP_TYPE_hi3798cv100)
    U_PERI_CRG58 unTmpPeriCrg58;
    unTmpPeriCrg58.u32 = g_pstRegCrg->PERI_CRG58.u32;
#endif
    UNUSED(s32Ret);
    UNUSED(enChipType);
    UNUSED(u32ChipVersion);
    UNUSED(u32RegVal);
    UNUSED(u8DataTmp);

	
   

    if (HI_SUCCESS != tuner_check_port(u32TunerPort))
    {
        return HI_FAILURE;
    }

    g_stTunerOps[u32TunerPort].enTunerDevType = pstTunerAttr->enTunerDevType;
    g_stTunerOps[u32TunerPort].enDemodDevType = pstDemodAttr->enDemodDevType;
    s_u32ResetGpioNo[u32TunerPort] = u32ResetGpioNo;

    g_stTunerOps[u32TunerPort].u32TunerAddress = pstTunerAttr->u32TunerAddr;
    g_stTunerOps[u32TunerPort].u32DemodAddress = pstDemodAttr->u32DemodAddr;

    g_stTunerOps[u32TunerPort].recalculate_signal_strength = NULL;
    g_stTunerOps[u32TunerPort].manage_after_chipreset = NULL;
    g_stTunerOps[u32TunerPort].tuner_resume = NULL;

    Data_8115cde0 = u32ResetGpioNo;

    switch (g_stTunerOps[u32TunerPort].enDemodDevType)
    {
#if defined(DEMOD_DEV_TYPE_HI3130)
        case HI_UNF_DEMOD_DEV_TYPE_3130E:
        	//806296b8
            {
            	tuner_chip_reset(u32ResetGpioNo);

          qam_read_byte(u32TunerPort, MCTRL_1_ADDR, &u8DataTmp);

             if ((u8DataTmp >> 5) == 5)
             {
            	 //80629bc8
                    hi3130v200_set_hard_reg_value(u32TunerPort);
                    s_u32InsideQamPort = u32TunerPort;
                    g_stTunerOps[u32TunerPort].tuner_connect = hi3130v200_connect;
                    g_stTunerOps[u32TunerPort].tuner_get_status = hi3130v200_get_status;
                    g_stTunerOps[u32TunerPort].tuner_get_ber = hi3130v200_get_ber;
                    g_stTunerOps[u32TunerPort].tuner_get_snr = hi3130v200_get_snr;
                    g_stTunerOps[u32TunerPort].tuner_set_ts_type = hi3130v200_set_ts_type;
                    g_stTunerOps[u32TunerPort].tuner_get_signal_strength = hi3130v200_get_signal_strength;
                    g_stTunerOps[u32TunerPort].tuner_test_single_agc = hi3130v200_test_single_agc;
                    g_stTunerOps[u32TunerPort].tuner_get_freq_symb_offset = hi3130v200_get_freq_symb_offset;
                    g_stTunerOps[u32TunerPort].manage_after_chipreset = hi3130v200_manage_after_chipreset;
                    g_stTunerOps[u32TunerPort].tuner_get_registers = hi3130v200_get_registers;
                    g_stTunerOps[u32TunerPort].tuner_get_rs = hi3130v200_get_rs;
                    g_stTunerOps[u32TunerPort].tuner_connect_timeout = hi3130v200_connect_timeout;
                    g_stTunerOps[u32TunerPort].tuner_set_ts_out = hi3130v200_set_ts_out;
                    HI_INFO_TUNER("tuner port %d's demod type is hi3130v200 outside\n", u32TunerPort); //197

            }
         else
         {
        	 //806296e0
                    s_u32InsideQamPort = u32TunerPort;
                    g_stTunerOps[u32TunerPort].tuner_connect = hi3130_connect;
                    g_stTunerOps[u32TunerPort].tuner_get_status = hi3130_get_status;
                    g_stTunerOps[u32TunerPort].tuner_get_ber = hi3130_get_ber;
                    g_stTunerOps[u32TunerPort].tuner_get_snr = hi3130_get_snr;
                    g_stTunerOps[u32TunerPort].tuner_set_ts_type = hi3130_set_ts_type;
                    g_stTunerOps[u32TunerPort].tuner_get_signal_strength = hi3130_get_signal_strength;
                    g_stTunerOps[u32TunerPort].tuner_test_single_agc = hi3130_test_single_agc;
                    g_stTunerOps[u32TunerPort].manage_after_chipreset = hi3130_manage_after_chipreset;
                    g_stTunerOps[u32TunerPort].tuner_get_registers = hi3130_get_registers;
                    g_stTunerOps[u32TunerPort].tuner_get_rs = hi3130_get_rs;
                    g_stTunerOps[u32TunerPort].tuner_connect_timeout = hi3130_connect_timeout;
                    g_stTunerOps[u32TunerPort].tuner_get_freq_symb_offset = hi3130_get_freq_symb_offset;
                    HI_INFO_TUNER("tuner port %d's demod type is hi3130 outside\n", u32TunerPort); //216
         }
                break;
            }
#endif

#if defined(DEMOD_DEV_TYPE_X5HDQAM) || defined(DEMOD_DEV_TYPE_X5HDV2QAM)
        case HI_UNF_DEMOD_DEV_TYPE_3130I:
            {
                qam_read_byte(u32TunerPort, MCTRL_1_ADDR, &u8DataTmp);

            if ((u8DataTmp >> 5) == 1 || (u8DataTmp >> 5) == 3 || (u8DataTmp >> 5) == 4 || (u8DataTmp >> 5)  == 6 || (u8DataTmp >> 5)  == 7)
            {
                if((u8DataTmp >> 5) == 1)
                {
                    HI_INFO_TUNER("tuner port %d's demod type is x5hdqammv200 inside\n", u32TunerPort);
                }
                else if((u8DataTmp >> 5) == 3)
                {
                    HI_INFO_TUNER("tuner port %d's demod type is x5hdqammv300 inside\n", u32TunerPort);
                }
                else if((u8DataTmp >> 5) == 4)
                {
                    HI_INFO_TUNER("tuner port %d's demod type is s40v100qam inside\n", u32TunerPort);
                }
                else if((u8DataTmp >> 5) == 6)
                {
                    HI_INFO_TUNER("tuner port %d's demod type is s40v200qam inside\n", u32TunerPort);
                }
          else if((u8DataTmp >> 5) == 7)
                {
                    HI_INFO_TUNER("tuner port %d's demod type is hi3716cv200qam inside\n", u32TunerPort);
                }

                /*config adc clk 28.8MHz*/
#if defined (CHIP_TYPE_hi3716cv200es)
                /*config qamadc_clk_sel 28.8MHz,  PERI_CRG58[9:8]:10*/
                unTmpPeriCrg58.bits.qamadc_clk_sel = 0x2;//PERI_CRG58[9:8]:10
                 /*config qamctrl_clk_sel 28.8MHz , PERI_CRG58[11:10]:10; \
                config qamctrl_clk_div for the same frequence, PERI_CRG58[12]:0*/
                unTmpPeriCrg58.bits.qamctrl_clk_sel = 0x2;//PERI_CRG58[11:10]:10
                unTmpPeriCrg58.bits.qamctrl_clk_div = 0;//PERI_CRG58[12]:0

                g_pstRegCrg->PERI_CRG58.u32 = unTmpPeriCrg58.u32;

#elif defined (CHIP_TYPE_hi3716cv200)
                /*config qamadc_clk_sel 28.8MHz,  PERI_CRG58[9:8]:00*/
                unTmpPeriCrg58.bits.qamadc_clk_sel = 0;//PERI_CRG58[9:8]:00
                /*config qamctrl_clk_sel 25MHz , PERI_CRG58[11:10]:00; \
                            config qamctrl_clk_div for the same frequence, PERI_CRG58[12]:0*/
                unTmpPeriCrg58.bits.qamctrl_clk_sel = 0;//PERI_CRG58[11:10]:00
                unTmpPeriCrg58.bits.qamctrl_clk_div = 0;//PERI_CRG58[12]:0
                g_pstRegCrg->PERI_CRG58.u32 = unTmpPeriCrg58.u32;

#elif defined (CHIP_TYPE_hi3798cv100)
                /*config qamadc_clk_sel 28.8MHz,  PERI_CRG58[9:8]:00*/
                unTmpPeriCrg58.bits.qamadc_clk_sel = 0;//PERI_CRG58[9:8]:00
                /*config qamctrl_clk_sel 25MHz , PERI_CRG58[11:10]:00; \
                            config qamctrl_clk_div for the same frequence, PERI_CRG58[12]:0*/
                unTmpPeriCrg58.bits.qamctrl_clk_sel = 0;//PERI_CRG58[11:10]:00
                unTmpPeriCrg58.bits.qamctrl_clk_div = 0;//PERI_CRG58[12]:0
                g_pstRegCrg->PERI_CRG58.u32 = unTmpPeriCrg58.u32;

#endif

#if defined (CHIP_TYPE_hi3716cv200)
                {
                       g_stTunerOps[u32TunerPort].tuner_connect = hi3716cv200_connect;
                    g_stTunerOps[u32TunerPort].tuner_get_status = hi3716cv200_get_status;
                    g_stTunerOps[u32TunerPort].tuner_get_ber = hi3716cv200_get_ber;
                    g_stTunerOps[u32TunerPort].tuner_get_snr = hi3716cv200_get_snr;
                    g_stTunerOps[u32TunerPort].tuner_set_ts_type = hi3716cv200_set_ts_type;
                    g_stTunerOps[u32TunerPort].tuner_get_signal_strength = hi3716cv200_get_signal_strength;
                    g_stTunerOps[u32TunerPort].tuner_test_single_agc = hi3716cv200_test_single_agc;
                    g_stTunerOps[u32TunerPort].tuner_get_freq_symb_offset = hi3716cv200_get_freq_symb_offset;
                    g_stTunerOps[u32TunerPort].manage_after_chipreset = hi3716cv200_manage_after_chipreset;
                    g_stTunerOps[u32TunerPort].tuner_get_registers = hi3716cv200_get_registers;
                    g_stTunerOps[u32TunerPort].tuner_get_rs = hi3716cv200_get_rs;
                    g_stTunerOps[u32TunerPort].tuner_connect_timeout = hi3716cv200_connect_timeout;
            }
#elif defined (CHIP_TYPE_hi3716cv200es)
                {
                    g_stTunerOps[u32TunerPort].tuner_connect = x5hdqamv200_connect;
                    g_stTunerOps[u32TunerPort].tuner_get_status = x5hdqamv200_get_status;
                    g_stTunerOps[u32TunerPort].tuner_get_ber = x5hdqamv200_get_ber;
                    g_stTunerOps[u32TunerPort].tuner_get_snr = x5hdqamv200_get_snr;
                    g_stTunerOps[u32TunerPort].tuner_set_ts_type = x5hdqamv200_set_ts_type;
                    g_stTunerOps[u32TunerPort].tuner_get_signal_strength = x5hdqamv200_get_signal_strength;
                    g_stTunerOps[u32TunerPort].tuner_test_single_agc = x5hdqamv200_test_single_agc;
                    g_stTunerOps[u32TunerPort].tuner_get_freq_symb_offset = x5hdqamv200_get_freq_symb_offset;
                    g_stTunerOps[u32TunerPort].manage_after_chipreset = x5hdqamv200_manage_after_chipreset;
                    g_stTunerOps[u32TunerPort].tuner_get_registers = x5hdqamv200_get_registers;
                    g_stTunerOps[u32TunerPort].tuner_get_rs = x5hdqamv200_get_rs;
                    g_stTunerOps[u32TunerPort].tuner_connect_timeout = x5hdqamv200_connect_timeout;

                }
#endif

            }
             else
            {
#if defined(DEMOD_DEV_TYPE_X5HDQAM)
                        s_u32InsideQamPort = u32TunerPort;
                        g_stTunerOps[u32TunerPort].tuner_connect = x5hdqam_connect;
                        g_stTunerOps[u32TunerPort].tuner_get_status = x5hdqam_get_status;
                        g_stTunerOps[u32TunerPort].tuner_get_ber = x5hdqam_get_ber;
                        g_stTunerOps[u32TunerPort].tuner_get_snr = x5hdqam_get_snr;
                        g_stTunerOps[u32TunerPort].tuner_set_ts_type = x5hdqam_set_ts_type;
                        g_stTunerOps[u32TunerPort].tuner_get_signal_strength = x5hdqam_get_signal_strength;
                        g_stTunerOps[u32TunerPort].tuner_test_single_agc = x5hdqam_test_single_agc;
                        g_stTunerOps[u32TunerPort].tuner_get_freq_symb_offset = x5hdqam_get_freq_symb_offset;
                        g_stTunerOps[u32TunerPort].manage_after_chipreset = x5hdqam_manage_after_chipreset;
                        g_stTunerOps[u32TunerPort].tuner_get_registers = x5hdqam_get_registers;
                        g_stTunerOps[u32TunerPort].tuner_get_rs = x5hdqam_get_rs;
                        g_stTunerOps[u32TunerPort].tuner_connect_timeout = x5hdqam_connect_timeout;
                        HI_INFO_TUNER("tuner port %d's demod type is x5hdqam inside\n", u32TunerPort);
#endif
                }

                break;
            }
#endif
#if defined(DEMOD_DEV_TYPE_J83B)
        case HI_UNF_DEMOD_DEV_TYPE_J83B:
        	//806294d8
           {
        	   tuner_chip_reset(u32ResetGpioNo);

            qam_read_byte(u32TunerPort, MCTRL_1_ADDR, &u8DataTmp);

            if((u8DataTmp >> 5) == 1)
            {
                HI_INFO_TUNER("tuner port %d's demod type is mv200qam_J83B\n", u32TunerPort); //347
            }
            else if((u8DataTmp >> 5) == 3)
            {
                HI_INFO_TUNER("tuner port %d's demod type is mv300qam_J83B\n", u32TunerPort); //351
            }
            else if((u8DataTmp >> 5) == 4)
            {
                HI_INFO_TUNER("tuner port %d's demod type is s40v100qam_j83b\n", u32TunerPort); //355
            }
         else if((u8DataTmp >> 5) == 5)
            {
                HI_INFO_TUNER("tuner port %d's demod type is hi3130v200qam_j83b\n", u32TunerPort); //359
            }

            else if((u8DataTmp >> 5) == 6)
            {
                HI_INFO_TUNER("tuner port %d's demod type is s40v200qam_j83b\n", u32TunerPort); //364
            }
         else if((u8DataTmp >> 5) == 7)
            {
                HI_INFO_TUNER("tuner port %d's demod type is hi3716cv200qam_j83b\n", u32TunerPort); //368
            }

        if((u8DataTmp >> 5) == 5)
        {
 #if defined(DEMOD_DEV_TYPE_HI3130)
            hi3130j83b_set_hard_reg_value(u32TunerPort);

            g_stTunerOps[u32TunerPort].tuner_connect = hi3130j83b_connect;
            g_stTunerOps[u32TunerPort].tuner_get_status = hi3130j83b_get_status;
            g_stTunerOps[u32TunerPort].tuner_get_ber = hi3130j83b_get_ber;
            g_stTunerOps[u32TunerPort].tuner_get_snr = hi3130j83b_get_snr;
            g_stTunerOps[u32TunerPort].tuner_set_ts_type = hi3130j83b_set_ts_type;
            g_stTunerOps[u32TunerPort].tuner_get_signal_strength = hi3130j83b_get_signal_strength;
            g_stTunerOps[u32TunerPort].tuner_test_single_agc = hi3130j83b_test_single_agc;
            g_stTunerOps[u32TunerPort].manage_after_chipreset = hi3130j83b_manage_after_chipreset;
            g_stTunerOps[u32TunerPort].tuner_get_registers = hi3130j83b_get_registers;
            g_stTunerOps[u32TunerPort].tuner_get_rs = hi3130j83b_get_rs;
            g_stTunerOps[u32TunerPort].tuner_get_freq_symb_offset = hi3130j83b_get_freq_symb_offset;
            g_stTunerOps[u32TunerPort].tuner_connect_timeout = hi3130j83b_connect_timeout;
            g_stTunerOps[u32TunerPort].tuner_set_ts_out = hi3130j83b_set_ts_out;
		 
            HI_INFO_TUNER("tuner port %d's demod type is hi3130v200_j83b outside\n", u32TunerPort); //390
 #endif
        }
        else
        {
            /*config adc clk 25MHz*/
#if defined  (CHIP_TYPE_hi3716cv200es)

            /*config qamadc_clk_sel 25MHz,  PERI_CRG58[9:8]:01*/
            unTmpPeriCrg58.bits.qamadc_clk_sel = 0x01;
            /*config qamctrl_clk_sel 25MHz , PERI_CRG58[11:10]:01; \
            config qamctrl_clk_div for the same frequence, PERI_CRG58[12]:0*/
            unTmpPeriCrg58.bits.qamctrl_clk_sel = 0x01;
            unTmpPeriCrg58.bits.qamctrl_clk_div = 0;
            g_pstRegCrg->PERI_CRG58.u32 = unTmpPeriCrg58.u32;

#elif defined (CHIP_TYPE_hi3716cv200)

            /*config qamadc_clk_sel 25MHz,  PERI_CRG58[9:8]:01*/
            unTmpPeriCrg58.bits.qamadc_clk_sel = 0x01;
            /*config qamctrl_clk_sel 25MHz , PERI_CRG58[11:10]:01; \
            config qamctrl_clk_div for the same frequence, PERI_CRG58[12]:0*/
            unTmpPeriCrg58.bits.qamctrl_clk_sel = 0x01;
            unTmpPeriCrg58.bits.qamctrl_clk_div = 0;
            g_pstRegCrg->PERI_CRG58.u32 = unTmpPeriCrg58.u32;

#elif defined (CHIP_TYPE_hi3798cv100)
			unTmpPeriCrg58.bits.bit7 = 1;
			unTmpPeriCrg58.bits.bit10 = 1;
			unTmpPeriCrg58.bits.bit13 = 0;
			g_pstRegCrg->PERI_CRG58.u32 = unTmpPeriCrg58.u32;
#endif
#if defined  (CHIP_TYPE_hi3716cv200)
                          {
                                 g_stTunerOps[u32TunerPort].tuner_connect = hi3716cv200_j83b_connect;
                        g_stTunerOps[u32TunerPort].tuner_get_status = hi3716cv200_j83b_get_status;
                        g_stTunerOps[u32TunerPort].tuner_get_ber = hi3716cv200_j83b_get_ber;
                        g_stTunerOps[u32TunerPort].tuner_get_snr = hi3716cv200_j83b_get_snr;
                        g_stTunerOps[u32TunerPort].tuner_set_ts_type = hi3716cv200_j83b_set_ts_type;
                        g_stTunerOps[u32TunerPort].tuner_get_signal_strength = hi3716cv200_j83b_get_signal_strength;
                        g_stTunerOps[u32TunerPort].tuner_test_single_agc = hi3716cv200_j83b_test_single_agc;
                        g_stTunerOps[u32TunerPort].manage_after_chipreset = hi3716cv200_j83b_manage_after_chipreset;
                        g_stTunerOps[u32TunerPort].tuner_get_registers = hi3716cv200_j83b_get_registers;
                        g_stTunerOps[u32TunerPort].tuner_get_rs = hi3716cv200_j83b_get_rs;
                        g_stTunerOps[u32TunerPort].tuner_get_freq_symb_offset = hi3716cv200_j83b_get_freq_symb_offset;
                        g_stTunerOps[u32TunerPort].tuner_connect_timeout = hi3716cv200_j83b_connect_timeout;
                        HI_INFO_TUNER("tuner port %d's demod type is j83b inside,demod Version:%d\n", u32TunerPort,(u8DataTmp >> 5));
                          }
#else
                          {
                        g_stTunerOps[u32TunerPort].tuner_connect = j83b_connect;
                        g_stTunerOps[u32TunerPort].tuner_get_status = j83b_get_status;
                        g_stTunerOps[u32TunerPort].tuner_get_ber = j83b_get_ber;
                        g_stTunerOps[u32TunerPort].tuner_get_snr = j83b_get_snr;
                        g_stTunerOps[u32TunerPort].tuner_set_ts_type = j83b_set_ts_type;
                        g_stTunerOps[u32TunerPort].tuner_get_signal_strength = j83b_get_signal_strength;
                        g_stTunerOps[u32TunerPort].tuner_test_single_agc = j83b_test_single_agc;
                        g_stTunerOps[u32TunerPort].manage_after_chipreset = j83b_manage_after_chipreset;
                        g_stTunerOps[u32TunerPort].tuner_get_registers = j83b_get_registers;
                        g_stTunerOps[u32TunerPort].tuner_get_rs = j83b_get_rs;
                        g_stTunerOps[u32TunerPort].tuner_get_freq_symb_offset = j83b_get_freq_symb_offset;
                        g_stTunerOps[u32TunerPort].tuner_connect_timeout = j83b_connect_timeout;
                        HI_INFO_TUNER("tuner port %d's demod type is j83b inside,demod Version:%d\n", u32TunerPort,(u8DataTmp >> 5)); //441
                          }
#endif
        }
            break;
            }
#endif

#if defined(DEMOD_DEV_TYPE_AVL6211)
        case HI_UNF_DEMOD_DEV_TYPE_AVL6211:
            {
                u8ChipReset = 1;
                g_stTunerOps[u32TunerPort].tuner_init = avl6211_init;
                g_stTunerOps[u32TunerPort].tuner_set_sat_attr = avl6211_set_sat_attr;
                g_stTunerOps[u32TunerPort].tuner_connect = avl6211_connect;
                g_stTunerOps[u32TunerPort].tuner_get_status = avl6211_get_status;
                g_stTunerOps[u32TunerPort].tuner_get_ber = avl6211_get_ber;
                g_stTunerOps[u32TunerPort].tuner_get_snr = avl6211_get_snr;
                g_stTunerOps[u32TunerPort].tuner_get_signal_strength  = avl6211_get_signal_strength;
                g_stTunerOps[u32TunerPort].tuner_get_freq_symb_offset = avl6211_get_freq_symb_offset;
                g_stTunerOps[u32TunerPort].tuner_get_signal_info = avl6211_get_signal_info;
                g_stTunerOps[u32TunerPort].tuner_set_ts_type = avl6211_set_ts_type;
                g_stTunerOps[u32TunerPort].tuner_blindscan_init   = avl6211_blindscan_init;
                g_stTunerOps[u32TunerPort].tuner_blindscan_action = avl6211_blindscan_action;
                g_stTunerOps[u32TunerPort].tuner_standby = avl6211_standby;
                g_stTunerOps[u32TunerPort].tuner_setfuncmode = avl6211_set_funcmode;
                break;
            }
#endif

#if defined(DEMOD_DEV_TYPE_HI3136)
        case HI_UNF_DEMOD_DEV_TYPE_3136:
                u8ChipReset = 1;/*externel demod need chip reset, internel do not need.*/

        case HI_UNF_DEMOD_DEV_TYPE_3136I:
            {
                g_stTunerOps[u32TunerPort].tuner_init = hi3136_init;
                g_stTunerOps[u32TunerPort].tuner_set_sat_attr = hi3136_set_sat_attr;
                g_stTunerOps[u32TunerPort].tuner_connect = hi3136_connect;
                g_stTunerOps[u32TunerPort].tuner_get_status = hi3136_get_status;
                g_stTunerOps[u32TunerPort].tuner_get_ber = hi3136_get_ber;
                g_stTunerOps[u32TunerPort].tuner_get_snr = hi3136_get_snr;
                g_stTunerOps[u32TunerPort].tuner_get_signal_strength  = hi3136_get_signal_strength;
                g_stTunerOps[u32TunerPort].tuner_get_freq_symb_offset = hi3136_get_freq_symb_offset;
                g_stTunerOps[u32TunerPort].tuner_get_signal_info = hi3136_get_signal_info;
                g_stTunerOps[u32TunerPort].tuner_set_ts_type = hi3136_set_ts_type;
                g_stTunerOps[u32TunerPort].tuner_blindscan_init   = hi3136_blindscan_init;
                g_stTunerOps[u32TunerPort].tuner_blindscan_action = hi3136_blindscan_action;
                /*g_stTunerOps[u32TunerPort].tuner_standby = hi3136_standby;
                g_stTunerOps[u32TunerPort].tuner_disable = hi3136_disable;*/
                g_stTunerOps[u32TunerPort].tuner_setfuncmode = hi3136_set_funcmode;
                g_stTunerOps[u32TunerPort].tuner_tp_verify = hi3136_tp_verify;
                g_stTunerOps[u32TunerPort].tuner_set_ts_out = hi3136_set_ts_out;
                g_stTunerOps[u32TunerPort].tuner_data_sample = hi3136_data_sample;
                break;
            }
#endif

#if defined(DEMOD_DEV_TYPE_MXL101)
        case HI_UNF_DEMOD_DEV_TYPE_MXL101:
            {
                u8ChipReset = 1;
                g_stTunerOps[u32TunerPort].tuner_init = mxl101_init;
                g_stTunerOps[u32TunerPort].tuner_connect = mxl101_connect;
                g_stTunerOps[u32TunerPort].tuner_get_status = mxl101_get_status;
                g_stTunerOps[u32TunerPort].tuner_get_ber = mxl101_get_ber;
                g_stTunerOps[u32TunerPort].tuner_get_snr = mxl101_get_snr;
                g_stTunerOps[u32TunerPort].tuner_get_signal_strength  = mxl101_get_signal_strength;
                g_stTunerOps[u32TunerPort].tuner_get_freq_symb_offset = mxl101_get_freq_symb_offset;
                g_stTunerOps[u32TunerPort].tuner_set_ts_type = mxl101_set_ts_type;
                g_stTunerOps[u32TunerPort].tuner_get_signal_info = mxl101_get_signal_info;
                break;
            }
#endif

#if defined(DEMOD_DEV_TYPE_MN88472)
        case HI_UNF_DEMOD_DEV_TYPE_MN88472:
            {
                u8ChipReset = 1;
                g_stTunerOps[u32TunerPort].tuner_init = MN88472_Init;
                g_stTunerOps[u32TunerPort].tuner_connect = MN88472_Connect;
                g_stTunerOps[u32TunerPort].tuner_get_status = MN88472_GetStatus;
                g_stTunerOps[u32TunerPort].tuner_get_ber = MN88472_GetBer;
                g_stTunerOps[u32TunerPort].tuner_get_snr = MN88472_GetSnr;
                g_stTunerOps[u32TunerPort].tuner_get_signal_strength  = MN88472_GetSignalStrength;
                g_stTunerOps[u32TunerPort].tuner_get_freq_symb_offset = MN88472_get_freq_symb_offset;
                g_stTunerOps[u32TunerPort].tuner_set_ts_type = MN88472_SetTsType;
                g_stTunerOps[u32TunerPort].tuner_get_signal_info = MN88472_get_signal_info;
                g_stTunerOps[u32TunerPort].tuner_setplpid = MN88472_setplpid;
                g_stTunerOps[u32TunerPort].tuner_get_plp_num = MN88472_get_plp_num;
                g_stTunerOps[u32TunerPort].tuner_get_current_plp_type = MN88472_get_current_plp_type;
                g_stTunerOps[u32TunerPort].tuner_connect_timeout = MN88472_connect_timeout;
                break;
            }
#endif

#if defined(DEMOD_DEV_TYPE_IT9133)
        case HI_UNF_DEMOD_DEV_TYPE_IT9133:
            {
                u8ChipReset = 1;
                g_stTunerOps[u32TunerPort].tuner_init = it9133_init;
                g_stTunerOps[u32TunerPort].tuner_connect = it9133_connect;
                g_stTunerOps[u32TunerPort].tuner_get_status = it9133_get_status;
                g_stTunerOps[u32TunerPort].tuner_get_ber = it9133_get_ber;
                g_stTunerOps[u32TunerPort].tuner_get_snr = it9133_get_snr;
                g_stTunerOps[u32TunerPort].tuner_get_signal_strength  = it9133_get_signal_strength;
                g_stTunerOps[u32TunerPort].tuner_get_freq_symb_offset = it9133_get_freq_symb_offset;
                g_stTunerOps[u32TunerPort].tuner_set_ts_type = it9133_set_ts_type;
                g_stTunerOps[u32TunerPort].tuner_get_signal_info = it9133_get_signal_info;
                break;
            }
#endif
#if defined(DEMOD_DEV_TYPE_IT9170)
        case HI_UNF_DEMOD_DEV_TYPE_IT9170:
            {
                u8ChipReset = 1;
                //ptrTunerAttr = &stTerTunerAttr;
                //unEXTTunerAttr.stTerAttr = stTerTunerAttr;
                g_stTunerOps[u32TunerPort].tuner_init = it9170_init;
                g_stTunerOps[u32TunerPort].tuner_connect = it9170_connect;
                g_stTunerOps[u32TunerPort].tuner_get_status = it9170_get_status;
                g_stTunerOps[u32TunerPort].tuner_get_ber = it9170_get_ber;
                g_stTunerOps[u32TunerPort].tuner_get_snr = it9170_get_snr;
                g_stTunerOps[u32TunerPort].tuner_get_signal_strength  = it9170_get_signal_strength;
                g_stTunerOps[u32TunerPort].tuner_get_freq_symb_offset = it9170_get_freq_symb_offset;
                g_stTunerOps[u32TunerPort].tuner_set_ts_type = it9170_set_ts_type;
                g_stTunerOps[u32TunerPort].tuner_get_signal_info = it9170_get_signal_info;
                break;
            }
#endif
#if defined(DEMOD_DEV_TYPE_CXD2837)
        case HI_UNF_DEMOD_DEV_TYPE_0x10B: //cxd2837
        {
        	//80629610
            u8ChipReset = 1;
        	g_stTunerOps[u32TunerPort].tuner_init = cxd2837_init;
            g_stTunerOps[u32TunerPort].tuner_connect = cxd2837_connect;
            g_stTunerOps[u32TunerPort].tuner_get_status = cxd2837_get_status;
            g_stTunerOps[u32TunerPort].tuner_get_ber = cxd2837_get_ber;
            g_stTunerOps[u32TunerPort].tuner_get_snr = cxd2837_get_snr;
            g_stTunerOps[u32TunerPort].tuner_get_signal_strength  = cxd2837_get_signal_strength;
            g_stTunerOps[u32TunerPort].tuner_get_freq_symb_offset = cxd2837_get_freq_symb_offset;
            g_stTunerOps[u32TunerPort].tuner_set_ts_type = cxd2837_set_ts_type;
            g_stTunerOps[u32TunerPort].tuner_get_signal_info = cxd2837_get_signal_info;
            g_stTunerOps[u32TunerPort].tuner_setplpid = cxd2837_setplpid;
            g_stTunerOps[u32TunerPort].tuner_get_plp_num = cxd2837_get_plp_num;
            g_stTunerOps[u32TunerPort].tuner_get_current_plp_type = cxd2837_get_current_plp_type;
            g_stTunerOps[u32TunerPort].tuner_connect_timeout = cxd2837_connect_timeout;
            //->80629420
        	break;
        }
#endif
#if defined(DEMOD_DEV_TYPE_HI3137)
        case HI_UNF_DEMOD_DEV_TYPE_0x10C: //hi3137
        {
        	//80629310
            u8ChipReset = 1;
            g_stTunerOps[u32TunerPort].tuner_init = hi3137_init;
            g_stTunerOps[u32TunerPort].tuner_connect = hi3137_connect;
            g_stTunerOps[u32TunerPort].tuner_get_status = hi3137_get_status;
            g_stTunerOps[u32TunerPort].tuner_get_ber = hi3137_get_ber;
            g_stTunerOps[u32TunerPort].tuner_get_snr = hi3137_get_snr;
            g_stTunerOps[u32TunerPort].tuner_get_signal_strength  = hi3137_get_signal_strength;
            g_stTunerOps[u32TunerPort].tuner_get_freq_symb_offset = hi3137_get_freq_symb_offset;
            g_stTunerOps[u32TunerPort].tuner_set_ts_type = hi3137_set_ts_type;
            g_stTunerOps[u32TunerPort].tuner_get_signal_info = hi3137_get_signal_info;
            g_stTunerOps[u32TunerPort].tuner_setplpid = hi3137_setplpid;
            g_stTunerOps[u32TunerPort].Func_412 = hi3137_tp_scan;
            g_stTunerOps[u32TunerPort].Func_456 = hi3137_set_common_plp_id;
            g_stTunerOps[u32TunerPort].Func_460 = hi3137_set_common_plp_combination;
            g_stTunerOps[u32TunerPort].tuner_get_plp_num = hi3137_get_plp_num;
            g_stTunerOps[u32TunerPort].tuner_get_current_plp_type = hi3137_get_current_plp_type;
            g_stTunerOps[u32TunerPort].Func_472 = hi3137_get_plp_id;
            g_stTunerOps[u32TunerPort].Func_476 = hi3137_get_plp_group_id;
            g_stTunerOps[u32TunerPort].tuner_connect_timeout = hi3137_connect_timeout;
            g_stTunerOps[u32TunerPort].tuner_set_ts_out = hi3137_set_ts_out;
            g_stTunerOps[u32TunerPort].Func_396 = hi3137_set_ter_attr;
            g_stTunerOps[u32TunerPort].Func_480 = hi3137_set_antena_power;
            g_stTunerOps[u32TunerPort].tuner_standby = hi3137_standby;
            //->80629420
        	break;
        }
#endif

        default:
        	HI_ERR_TUNER("demod type error:%d", g_stTunerOps[u32TunerPort].enDemodDevType); //653
            return HI_FAILURE;
    }
#if defined(DEMOD_DEV_TYPE_IT9170) || defined(DEMOD_DEV_TYPE_IT9133)||defined(DEMOD_DEV_TYPE_MN88472)||defined(DEMOD_DEV_TYPE_MXL101)||defined(DEMOD_DEV_TYPE_HI3136)||defined(DEMOD_DEV_TYPE_AVL6211) \
     || defined(DDEMOD_DEV_TYPE_HI3137) || defined(DEMOD_DEV_TYPE_CXD2837)
    if (u8ChipReset)
    {
        tuner_chip_reset(u32ResetGpioNo);
    }
#endif
    //80629428
    /* Added begin:l00185424 2011-12-20 For AVL6211 */
    if (g_stTunerOps[u32TunerPort].tuner_init)
    {
        g_stTunerOps[u32TunerPort].tuner_init(u32TunerPort, g_stTunerOps[u32TunerPort].enI2cChannel, g_stTunerOps[u32TunerPort].enTunerDevType);
        //s_stTunerResumeInfo[u32TunerPort].unResumeData = unEXTTunerAttr;
    }
    /* Added begin:l00185424 2011-11-28 For AVL6211 */

    switch (g_stTunerOps[u32TunerPort].enTunerDevType)
    {
#if 1
#if defined(TUNER_DEV_TYPE_CD1616)
        case HI_UNF_TUNER_DEV_TYPE_CD1616: //80629b88
            {
                g_stTunerOps[u32TunerPort].set_tuner = cd1616_set_tuner;
                HI_INFO_TUNER("tuner port %d's halftuner type is cd1616\n", u32TunerPort); //679
                break;
            }
#endif
#if defined(TUNER_DEV_TYPE_CD1616_LF_GIH_4)
        case HI_UNF_TUNER_DEV_TYPE_CD1616_DOUBLE: //80629a5c
            {
                g_stTunerOps[u32TunerPort].set_tuner = cd1616_double_set_tuner;
                HI_INFO_TUNER("tuner port %d's halftuner type is cd1616_double\n", u32TunerPort); //687
                break;
            }
#endif
#if defined(TUNER_DEV_TYPE_ALPS_TDAE)
        case HI_UNF_TUNER_DEV_TYPE_ALPS_TDAE: //80629b48
            {
                g_stTunerOps[u32TunerPort].set_tuner = alps_tdae_set_tuner;
                HI_INFO_TUNER("tuner port %d's halftuner type is alps_tdae\n", u32TunerPort); //695
                break;
            }
#endif
#endif

#if defined(TUNER_DEV_TYPE_TDA18250)
        case HI_UNF_TUNER_DEV_TYPE_TDA18250: //80629a9c
            {
                s32Ret = tda18250_init_tuner(u32TunerPort);
                if(HI_FAILURE == s32Ret)
                {
                    HI_INFO_TUNER("tda18250 init error\n"); //707
                    return HI_FAILURE;
                }
                g_stTunerOps[u32TunerPort].set_tuner = tda18250_set_tuner;
                g_stTunerOps[u32TunerPort].tuner_resume = tda18250_tuner_resume;
                g_stTunerOps[u32TunerPort].tuner_get_signal_strength = tda18250_get_signal_strength;
                HI_INFO_TUNER("tuner port %d's halftuner type is tda18250\n", u32TunerPort); //713
                break;
            }
#endif

#if defined(TUNER_DEV_TYPE_TMX7070X)
        case HI_UNF_TUNER_DEV_TYPE_TMX7070X: //806299c4
            {
                g_stTunerOps[u32TunerPort].set_tuner = tmx7070x_set_tuner;
                HI_INFO_TUNER("tuner port %d's halftuner type is tmx7070x\n", u32TunerPort); //722
                break;
            }
#endif
#if defined(TUNER_DEV_TYPE_TDCC)
        case HI_UNF_TUNER_DEV_TYPE_TDCC: //80629b08
            {
                g_stTunerOps[u32TunerPort].set_tuner = tdcc_set_tuner;
                HI_INFO_TUNER("tuner port %d's halftuner type is tdcc\n", u32TunerPort); //730
                break;
            }
#endif
#if defined(TUNER_DEV_TYPE_XG_3BL)
        case HI_UNF_TUNER_DEV_TYPE_XG_3BL:
            {
                g_stTunerOps[u32TunerPort].set_tuner = xg_3bl_set_tuner;
                HI_INFO_TUNER("tuner port %d's halftuner type is xg3bl\n", u32TunerPort);
                break;
            }
#endif
#if defined(TUNER_DEV_TYPE_MT2081)
        case HI_UNF_TUNER_DEV_TYPE_MT2081: //80629a04
            {
                g_stTunerOps[u32TunerPort].set_tuner = mt2081_set_tuner;
                g_stTunerOps[u32TunerPort].tuner_resume = mt2081_tuner_resume;
                g_stTunerOps[u32TunerPort].tuner_get_signal_strength = mt2081_get_signal_strength;
                HI_INFO_TUNER("tuner port %d's halftuner type is mt2081\n", u32TunerPort); //748
                break;
            }
#endif

#if defined(TUNER_DEV_TYPE_R820C)
        case HI_UNF_TUNER_DEV_TYPE_R820C: //80629958
            {
                s32Ret = r820c_init_tuner(u32TunerPort);
                if(HI_FAILURE == s32Ret)
                {
                    HI_INFO_TUNER("r820c_init_tuner error\n"); //759
                    return HI_FAILURE;
                }
                g_stTunerOps[u32TunerPort].set_tuner = r820c_set_tuner;
                g_stTunerOps[u32TunerPort].tuner_resume = r820c_tuner_resume;
                g_stTunerOps[u32TunerPort].recalculate_signal_strength = r820c_get_signal_strength;
                HI_INFO_TUNER("tuner port %d's halftuner type is r820c\n", u32TunerPort); //765
                break;
            }
#endif

#if defined(TUNER_DEV_TYPE_MXL203)
        case HI_UNF_TUNER_DEV_TYPE_MXL203: //80629918
        {
            g_stTunerOps[u32TunerPort].set_tuner = mxl203_set_tuner;
            HI_INFO_TUNER("tuner port %d's halftuner type is mxl203\n", u32TunerPort); //774
            break;
        }
#endif

#if defined(TUNER_DEV_TYPE_TDA18250B)
        case HI_UNF_TUNER_DEV_TYPE_TDA18250B: //806297d0
        {
            s32Ret = tda18250b_init_tuner(u32TunerPort);
            if(HI_FAILURE == s32Ret)
            {
                HI_ERR_TUNER("tda18250B init error\n"); //785
                return HI_FAILURE;
            }
            g_stTunerOps[u32TunerPort].set_tuner = tda18250b_set_tuner;
            g_stTunerOps[u32TunerPort].tuner_resume = tda18250b_tuner_resume;
            g_stTunerOps[u32TunerPort].tuner_get_signal_strength = tda18250b_get_signal_strength;
            HI_INFO_TUNER("tuner port %d's halftuner type is tda18250B\n", u32TunerPort); //791
            break;
        }
#endif

#if defined(TUNER_DEV_TYPE_AV2011)
        case HI_UNF_TUNER_DEV_TYPE_AV2011:
            {
#if defined(DEMOD_DEV_TYPE_HI3136)
                g_stTunerOps[u32TunerPort].set_tuner = av2011_set_tuner;
#endif
                HI_INFO_TUNER("tuner port %d's halftuner type is AV2011\n", u32TunerPort);
                break;
            }
#endif

#if defined(TUNER_DEV_TYPE_SHARP7903)
        case HI_UNF_TUNER_DEV_TYPE_SHARP7903:
            {
#if defined(DEMOD_DEV_TYPE_HI3136)
                g_stTunerOps[u32TunerPort].set_tuner = sharp7903_set_tuner;
#endif
                HI_INFO_TUNER("tuner port %d's halftuner type is SHARP7903\n", u32TunerPort);
                break;
            }
#endif

#if defined(TUNER_DEV_TYPE_RDA5815)
        case HI_UNF_TUNER_DEV_TYPE_RDA5815:
            {
#if defined(DEMOD_DEV_TYPE_HI3136)
                g_stTunerOps[u32TunerPort].set_tuner = RDA5815_set_tuner;
#endif
                HI_INFO_TUNER("tuner port %d's halftuner type is RDA5815\n", u32TunerPort);
                break;
            }
#endif

#if defined(TUNER_DEV_TYPE_M88TS2022)
        case HI_UNF_TUNER_DEV_TYPE_M88TS2022:
            {
#if defined(DEMOD_DEV_TYPE_HI3136)
                g_stTunerOps[u32TunerPort].set_tuner = M88TS2022_set_tuner;
#endif
                HI_INFO_TUNER("tuner port %d's halftuner type is M88TS2022\n", u32TunerPort);
                break;
            }
#endif

        case HI_UNF_TUNER_DEV_TYPE_MXL101: //806298c0
            {
                HI_INFO_TUNER("tuner port %d's halftuner type is MXL101\n", u32TunerPort); //842
                break;
            }

#if defined(TUNER_DEV_TYPE_MXL603)
        case HI_UNF_TUNER_DEV_TYPE_MXL603:
            {
                g_stTunerOps[u32TunerPort].set_tuner = mxl603_set_tuner;
                g_stTunerOps[u32TunerPort].tuner_resume = mxl603_tuner_resume;
                g_stTunerOps[u32TunerPort].tuner_get_signal_strength = mxl603_get_signal_strength;
                HI_INFO_TUNER("tuner port %d's halftuner type is mxl603\n", u32TunerPort); //853
                break;
            }
#endif
        case HI_UNF_TUNER_DEV_TYPE_IT9170: //806298ec
            {
                HI_INFO_TUNER("tuner port %d's halftuner type is IT9170\n", u32TunerPort); //883
                break;
            }
        case HI_UNF_TUNER_DEV_TYPE_IT9133: //8062983c
            {
                HI_INFO_TUNER("tuner port %d's halftuner type is IT9133\n", u32TunerPort); //888
                break;
            }
#if defined(TUNER_DEV_TYPE_TDA6651)
        case HI_UNF_TUNER_DEV_TYPE_TDA6651:
            {
                g_stTunerOps[u32TunerPort].set_tuner = tda6651_set_tuner;

                HI_INFO_TUNER("tuner port %d's halftuner type is tda6651\n", u32TunerPort);
                break;
            }
#endif
        case HI_UNF_TUNER_DEV_TYPE_20: //806297a4
        case HI_UNF_TUNER_DEV_TYPE_25: //806297a4
        {
        	HI_INFO_TUNER("tuner port %d's halftuner type is MXL254\n", u32TunerPort); //903
        	break;
        }

        default:
            {
                //g_stTunerOps[u32TunerPort].set_tuner = cd1616_set_tuner;
                //HI_INFO_TUNER("tuner port %d's halftuner type(default) is cd1616\n", u32TunerPort);
                //break;
            }
    }

    return HI_SUCCESS;
}

HI_S32 tuner_set_sat_attr(HI_U32 u32TunerPort, HI_UNF_TUNER_SAT_ATTR_S *pstSatTunerAttr)
{
    s_stSatTunerResumeInfo[u32TunerPort].stSatTunerAttr = *pstSatTunerAttr;

    if (g_stTunerOps[u32TunerPort].tuner_set_sat_attr)
    {
        g_stTunerOps[u32TunerPort].tuner_set_sat_attr(u32TunerPort, pstSatTunerAttr);
    }

    /* For DVB-S/S2 demod, need configurate LNB control device */
    switch (g_stTunerOps[u32TunerPort].enDemodDevType)
    {
#if defined(DEMOD_DEV_TYPE_AVL6211)
        case HI_UNF_DEMOD_DEV_TYPE_AVL6211:
            {
                g_stTunerOps[u32TunerPort].tuner_set_lnb_out = HI_NULL;
                g_stTunerOps[u32TunerPort].tuner_send_continuous_22K = avl6211_send_continuous_22K;
                g_stTunerOps[u32TunerPort].tuner_send_tone = avl6211_send_tone;
                g_stTunerOps[u32TunerPort].tuner_DiSEqC_send_msg = avl6211_DiSEqC_send_msg;
                g_stTunerOps[u32TunerPort].tuner_DiSEqC_recv_msg = avl6211_DiSEqC_recv_msg;

                /* Which device send wave */
                switch (pstSatTunerAttr->enDiSEqCWave)
                {
                        /* Demod send wave, LNB control only supply power */
                    case HI_UNF_TUNER_DISEQCWAVE_NORMAL:
                    default:
                        {
                            switch (pstSatTunerAttr->enLNBCtrlDev)
                            {
#if defined(LNB_CTRL_MPS8125)
                                case HI_UNF_LNBCTRL_DEV_TYPE_MPS8125:
                                    g_stTunerOps[u32TunerPort].tuner_lnbctrl_dev_init = mps8125_init;
                                    g_stTunerOps[u32TunerPort].tuner_lnbctrl_dev_standby = mps8125_standby;
                                    g_stTunerOps[u32TunerPort].tuner_set_lnb_out = mps8125_set_lnb_out;
                                    break;
#endif

#if defined(LNB_CTRL_ISL9492)
                                case HI_UNF_LNBCTRL_DEV_TYPE_ISL9492:
                                    g_stTunerOps[u32TunerPort].tuner_lnbctrl_dev_init = isl9492_init;
                                    g_stTunerOps[u32TunerPort].tuner_lnbctrl_dev_standby = isl9492_standby;
                                    g_stTunerOps[u32TunerPort].tuner_set_lnb_out = isl9492_set_lnb_out;
                                    break;
#endif

                                case HI_UNF_LNBCTRL_DEV_TYPE_NONE:
                                default:
                                    HI_INFO_TUNER("LNB ctrl chiptype error!\n");
                                    return HI_FAILURE;
                            }

                            break;
                        }

                        /* LNB control device send wave */
                    case HI_UNF_TUNER_DISEQCWAVE_ENVELOPE:
                        {
                            switch (pstSatTunerAttr->enLNBCtrlDev)
                            {
#if defined(LNB_CTRL_MPS8125)
                                case HI_UNF_LNBCTRL_DEV_TYPE_MPS8125:
                                    g_stTunerOps[u32TunerPort].tuner_lnbctrl_dev_init = mps8125_init;
                                    g_stTunerOps[u32TunerPort].tuner_lnbctrl_dev_standby = mps8125_standby;
                                    g_stTunerOps[u32TunerPort].tuner_set_lnb_out = mps8125_set_lnb_out;
                                    g_stTunerOps[u32TunerPort].tuner_send_continuous_22K = mps8125_send_continuous_22K;
                                    g_stTunerOps[u32TunerPort].tuner_send_tone = mps8125_send_tone;
                                    g_stTunerOps[u32TunerPort].tuner_DiSEqC_send_msg = mps8125_DiSEqC_send_msg;
                                    g_stTunerOps[u32TunerPort].tuner_DiSEqC_recv_msg = mps8125_DiSEqC_recv_msg;
                                    break;
#endif

#if defined(LNB_CTRL_ISL9492)
                                case HI_UNF_LNBCTRL_DEV_TYPE_ISL9492:
                                    g_stTunerOps[u32TunerPort].tuner_lnbctrl_dev_init = isl9492_init;
                                    g_stTunerOps[u32TunerPort].tuner_lnbctrl_dev_standby = isl9492_standby;
                                    g_stTunerOps[u32TunerPort].tuner_set_lnb_out = isl9492_set_lnb_out;
                                    g_stTunerOps[u32TunerPort].tuner_send_continuous_22K = isl9492_send_continuous_22K;
                                    g_stTunerOps[u32TunerPort].tuner_send_tone = isl9492_send_tone;
                                    g_stTunerOps[u32TunerPort].tuner_DiSEqC_send_msg = isl9492_DiSEqC_send_msg;
                                    g_stTunerOps[u32TunerPort].tuner_DiSEqC_recv_msg = isl9492_DiSEqC_recv_msg;
                                    break;
#endif

                                case HI_UNF_LNBCTRL_DEV_TYPE_NONE:
                                default:
                                    HI_INFO_TUNER("LNB ctrl chiptype error!\n");
                                    return HI_FAILURE;
                            }

                            break;
                        }
                }

                break;
            }
#endif

#if defined(DEMOD_DEV_TYPE_HI3136)
        case HI_UNF_DEMOD_DEV_TYPE_3136:
        case HI_UNF_DEMOD_DEV_TYPE_3136I:
            {
                g_stTunerOps[u32TunerPort].tuner_set_lnb_out = HI_NULL;
                g_stTunerOps[u32TunerPort].tuner_send_continuous_22K = hi3136_send_continuous_22K;
                g_stTunerOps[u32TunerPort].tuner_send_tone = hi3136_send_tone;
                g_stTunerOps[u32TunerPort].tuner_DiSEqC_send_msg = hi3136_DiSEqC_send_msg;
                /*g_stTunerOps[u32TunerPort].tuner_DiSEqC_recv_msg = hi3136_DiSEqC_recv_msg;*/

                /* Which device send wave */
                switch (pstSatTunerAttr->enDiSEqCWave)
                {
                        /* Demod send wave, LNB control only supply power */
                    case HI_UNF_TUNER_DISEQCWAVE_NORMAL:
                    default:
                        {
                            switch (pstSatTunerAttr->enLNBCtrlDev)
                            {
#if defined(LNB_CTRL_MPS8125)
                                case HI_UNF_LNBCTRL_DEV_TYPE_MPS8125:
                                    g_stTunerOps[u32TunerPort].tuner_lnbctrl_dev_init = mps8125_init;
                                    g_stTunerOps[u32TunerPort].tuner_lnbctrl_dev_standby = mps8125_standby;
                                    g_stTunerOps[u32TunerPort].tuner_set_lnb_out = mps8125_set_lnb_out;
                                    break;
#endif

#if defined(LNB_CTRL_ISL9492)
                                case HI_UNF_LNBCTRL_DEV_TYPE_ISL9492:
                                    g_stTunerOps[u32TunerPort].tuner_lnbctrl_dev_init = isl9492_init;
                                    g_stTunerOps[u32TunerPort].tuner_lnbctrl_dev_standby = isl9492_standby;
                                    g_stTunerOps[u32TunerPort].tuner_set_lnb_out = isl9492_set_lnb_out;
                                    break;
#endif

                                case HI_UNF_LNBCTRL_DEV_TYPE_NONE:
                                default:
                                    HI_INFO_TUNER("LNB ctrl chiptype error!\n");
                                    return HI_FAILURE;
                            }

                            break;
                        }

                        /* LNB control device send wave */
                    case HI_UNF_TUNER_DISEQCWAVE_ENVELOPE:
                        {
                            switch (pstSatTunerAttr->enLNBCtrlDev)
                            {
#if defined(LNB_CTRL_MPS8125)
                                case HI_UNF_LNBCTRL_DEV_TYPE_MPS8125:
                                    g_stTunerOps[u32TunerPort].tuner_lnbctrl_dev_init = mps8125_init;
                                    g_stTunerOps[u32TunerPort].tuner_lnbctrl_dev_standby = mps8125_standby;
                                    g_stTunerOps[u32TunerPort].tuner_set_lnb_out = mps8125_set_lnb_out;
                                    g_stTunerOps[u32TunerPort].tuner_send_continuous_22K = mps8125_send_continuous_22K;
                                    g_stTunerOps[u32TunerPort].tuner_send_tone = mps8125_send_tone;
                                    g_stTunerOps[u32TunerPort].tuner_DiSEqC_send_msg = mps8125_DiSEqC_send_msg;
                                    g_stTunerOps[u32TunerPort].tuner_DiSEqC_recv_msg = mps8125_DiSEqC_recv_msg;
                                    break;
#endif

#if defined(LNB_CTRL_ISL9492)
                                case HI_UNF_LNBCTRL_DEV_TYPE_ISL9492:
                                    g_stTunerOps[u32TunerPort].tuner_lnbctrl_dev_init = isl9492_init;
                                    g_stTunerOps[u32TunerPort].tuner_lnbctrl_dev_standby = isl9492_standby;
                                    g_stTunerOps[u32TunerPort].tuner_set_lnb_out = isl9492_set_lnb_out;
                                    g_stTunerOps[u32TunerPort].tuner_send_continuous_22K = isl9492_send_continuous_22K;
                                    g_stTunerOps[u32TunerPort].tuner_send_tone = isl9492_send_tone;
                                    g_stTunerOps[u32TunerPort].tuner_DiSEqC_send_msg = isl9492_DiSEqC_send_msg;
                                    g_stTunerOps[u32TunerPort].tuner_DiSEqC_recv_msg = isl9492_DiSEqC_recv_msg;
                                    break;
#endif

                                case HI_UNF_LNBCTRL_DEV_TYPE_NONE:
                                default:
                                    HI_INFO_TUNER("LNB ctrl chiptype error!\n");
                                    return HI_FAILURE;
                            }

                            break;
                        }
                }

                break;
            }
#endif
        default:
            {
                HI_INFO_TUNER("Demod type error! Not sat demod!\n"); //1104
                return HI_FAILURE;
            }
    }
#if defined(DEMOD_DEV_TYPE_AVL6211) || defined(DEMOD_DEV_TYPE_HI3136)
    if (g_stTunerOps[u32TunerPort].tuner_lnbctrl_dev_init)
    {
        g_stTunerOps[u32TunerPort].tuner_lnbctrl_dev_init(u32TunerPort,
                                                          g_stTunerOps[u32TunerPort].enI2cChannel,
                                                          pstSatTunerAttr->u16LNBDevAddress, g_stTunerOps[u32TunerPort].enDemodDevType);
    }
#endif
    return HI_SUCCESS;
}

HI_S32 tuner_set_ter_attr(HI_U32 u32TunerPort, HI_UNF_TUNER_SAT_ATTR_S *pstSatTunerAttr)
{
//    s_stSatTunerResumeInfo[u32TunerPort].stSatTunerAttr = *pstSatTunerAttr;

    if (!g_stTunerOps[u32TunerPort].Func_396)
    {
    	HI_INFO_TUNER("tuner service function ptr is NULL!\n"); //1125
        return -EFAULT; //-EINVAL; //HI_FAILURE;
    }



    return g_stTunerOps[u32TunerPort].Func_396(u32TunerPort, pstSatTunerAttr);
}

HI_S32 tuner_select_i2c(HI_U32 u32TunerPort, HI_U8 enI2cChannel)
{
    if (enI2cChannel >= /*HI_UNF_I2C_CHANNEL_MAX*/16)
    {
        HI_INFO_TUNER( "invalid i2c channel: %d\n", enI2cChannel); //1138
        return -EINVAL;
    }
    HI_INFO_TUNER("tuner port %d's i2c channel is %d\n", u32TunerPort, enI2cChannel); //1141

    g_stTunerOps[u32TunerPort].enI2cChannel = enI2cChannel;

    return HI_SUCCESS;
}


HI_S32 tuner_osr_connect(PTR_TUNER_SIGNAL_S pstTunerSignal)
{
    HI_U32 u32TunerPort = 0;
    HI_S32 s32Ret = 0;

    HI_DRV_STAT_Event(STAT_EVENT_CONNECT, 0);

    /* get tuner port  number and if tuner port is not supported , return fault */
    u32TunerPort = pstTunerSignal->u32Port;
    if (HI_SUCCESS != tuner_check_port(u32TunerPort))
    {
        HI_INFO_TUNER( "invalid tuner port: %d\n", u32TunerPort); //1160
        return HI_FAILURE;
    }

    g_stTunerOps[u32TunerPort].enSigType = pstTunerSignal->enSigType;
    g_stTunerOps[u32TunerPort].stCurrPara.u32Frequency = pstTunerSignal->stSignal.u32Frequency;
    g_stTunerOps[u32TunerPort].stCurrPara.unSRBW.u32SymbolRate = pstTunerSignal->stSignal.unSRBW.u32SymbolRate;
    g_stTunerOps[u32TunerPort].stCurrPara.enQamType = pstTunerSignal->stSignal.enQamType;
    g_stTunerOps[u32TunerPort].stCurrPara.bSI = pstTunerSignal->stSignal.bSI;

    /* if tuner connect operation is not setted, return fault */
    if (!g_stTunerOps[u32TunerPort].tuner_connect) /* check if tuner conect is supported */
    {
        HI_INFO_TUNER( "tuner service function ptr is NULL!\n"); //1173
        return HI_FAILURE;
    }

    /* call tuner connect operation */
    s32Ret = g_stTunerOps[u32TunerPort].tuner_connect(u32TunerPort, &(pstTunerSignal->stSignal));
    if (HI_SUCCESS != s32Ret)
    {
        HI_INFO_TUNER( "\n tuner_connect failed \n"); //1181
        return s32Ret;
    }

    return HI_SUCCESS;
}


HI_S32 tuner_osr_getstatus(PTR_TUNER_DATA_S pstTunerDataStr)
{
    HI_U32 u32TunerPort = 0;
    HI_S32 s32Ret = 0;

    /* get tuner port  number and if tuner port is not supported , return fault */
    u32TunerPort = pstTunerDataStr->u32Port;
    if (HI_SUCCESS != tuner_check_port(u32TunerPort))
    {
        return HI_FAILURE;
    }

    /* if tuner get status operation is not setted, return fault */
    if (!g_stTunerOps[u32TunerPort].tuner_get_status) /* check if tuner get status is supported */
    {
        HI_INFO_TUNER( "tuner service function ptr is NULL!\n"); //1204
        return -EFAULT;
    }

    /* get tuner lock status */
    s32Ret = g_stTunerOps[u32TunerPort].tuner_get_status(u32TunerPort,
                                    (HI_UNF_TUNER_LOCK_STATUS_E *)&pstTunerDataStr->u32Data);
    if (HI_SUCCESS != s32Ret)
    {
        HI_INFO_TUNER( "\n tuner_get_status failed \n"); //1213
        return s32Ret;
    }

    s_enTunerStat[u32TunerPort] = pstTunerDataStr->u32Data;

    if (HI_UNF_TUNER_SIGNAL_LOCKED == pstTunerDataStr->u32Data)
    {
        HI_DRV_STAT_Event(STAT_EVENT_LOCKED, 0);
    }

    return HI_SUCCESS;
}

HI_S32 tuner_osr_getsignalstrength(PTR_TUNER_DATABUF_S pstTunerDataBuf)
{
    HI_U32 u32TunerPort = 0;
    HI_S32 s32Ret = 0;

    /* get tuner port  number and if tuner port is not supported , return fault */
    u32TunerPort = pstTunerDataBuf->u32Port;
    if (HI_SUCCESS != tuner_check_port(u32TunerPort))
    {
        return HI_FAILURE;
    }

    /* if tuner get singal strength operation is not setted, return fault */
    if (!g_stTunerOps[u32TunerPort].tuner_get_signal_strength) /* check if tuner get strength is supported */
    {
        HI_INFO_TUNER( "tuner service function ptr is NULL!\n"); //1242
        return -EFAULT;
    }

    /* call signal strength */
    s32Ret = g_stTunerOps[u32TunerPort].tuner_get_signal_strength(u32TunerPort, pstTunerDataBuf->u32DataBuf);
    if (HI_SUCCESS != s32Ret)
    {
        HI_INFO_TUNER( "tuner_get_signal_strength failed!\n"); //1250
    }

    return s32Ret;
}


HI_S32 tuner_osr_getber(PTR_TUNER_DATABUF_S pstTunerDataBuf)
{
    HI_U32 u32TunerPort = 0;
    HI_S32 s32Ret = 0;

    /* get tuner port  number and if tuner port is not supported , return fault */
    u32TunerPort = pstTunerDataBuf->u32Port;
    if (HI_SUCCESS != tuner_check_port(u32TunerPort))
    {
        return HI_FAILURE;
    }

    /* if tuner get ber operation is not setted, return fault */
    if (!g_stTunerOps[u32TunerPort].tuner_get_ber) /* check if tuner get ber is supported */
    {
        HI_INFO_TUNER( "tuner service function ptr is NULL!\n"); //1272
        return -EFAULT;
    }

    /* tuner get ber */
    s32Ret = g_stTunerOps[u32TunerPort].tuner_get_ber(u32TunerPort, pstTunerDataBuf->u32DataBuf);
    if (HI_SUCCESS != s32Ret)
    {
        HI_INFO_TUNER( "tuner_get_ber failed!\n"); //1280
    }

    return s32Ret;
}

HI_S32 tuner_osr_getsnr(PTR_TUNER_DATA_S pstTunerDataStr)
{
    HI_U32 u32TunerPort = 0;
    HI_S32 s32Ret = 0;

    /* get tuner port  number and if tuner port is not supported , return fault */
    u32TunerPort = pstTunerDataStr->u32Port;
    if (HI_SUCCESS != tuner_check_port(u32TunerPort))
    {
        return HI_FAILURE;
    }

    /* if tuner get snr operation is not setted, return fault */
    if (!g_stTunerOps[u32TunerPort].tuner_get_snr) /* check if tuner snr ber is supported */
    {
        HI_INFO_TUNER( "tuner service function ptr is NULL!\n"); //1301
        return -EFAULT;
    }

    /* get snr */
    s32Ret = g_stTunerOps[u32TunerPort].tuner_get_snr(u32TunerPort, &pstTunerDataStr->u32Data);
    if (HI_SUCCESS != s32Ret)
    {
        HI_INFO_TUNER( "tuner_get_snr failed!\n"); //1309
    }

    return s32Ret;
}


HI_S32 tuner_osr_settstype(PTR_TUNER_DATA_S pstTunerDataStr)
{
    HI_UNF_TUNER_OUPUT_MODE_E enTsType;
    HI_U32 u32TunerPort = 0;
    HI_S32 s32Ret = 0;

    /* get tuner port  number and if tuner port is not supported , return fault */
    u32TunerPort = pstTunerDataStr->u32Port;
    if (HI_SUCCESS != tuner_check_port(u32TunerPort))
    {
        HI_INFO_TUNER( "invalid tuner port: %d\n", u32TunerPort); //1326
        return HI_FAILURE;
    }

    /* if tuner get snr operation is not setted, return fault */
    if (!g_stTunerOps[u32TunerPort].tuner_set_ts_type) /* check if tuner set ts type is supported */
    {
        HI_INFO_TUNER( "tuner service function ptr is NULL!\n"); //1333
        return -EFAULT;
    }

    enTsType = (HI_UNF_TUNER_OUPUT_MODE_E)pstTunerDataStr->u32Data;

    /* set ts type */
    s32Ret = g_stTunerOps[u32TunerPort].tuner_set_ts_type(u32TunerPort, enTsType);
    if (HI_SUCCESS != s32Ret)
    {
        HI_INFO_TUNER( "tuner_set_ts_type failed!\n"); //1343
    }

    return s32Ret;
}

HI_S32 tuner_osr_selecttype(PTR_TUNER_DATABUF_S pstTunerDataBuf)
{
    HI_TunerAttr_S *pstTuner_Attr = HI_NULL;
    HI_DemodAttr_S *pstDemod_Attr = HI_NULL;
    HI_U32 u32TunerPort = 0;
    HI_S32 s32Ret = 0;
    HI_U32 u32ResetGpioNo = 0xFF;

    u32TunerPort = pstTunerDataBuf->u32Port;
    if (HI_SUCCESS != tuner_check_port(u32TunerPort))
    {
        HI_INFO_TUNER( "invalid tuner port: %d\n", u32TunerPort); //1360
        return HI_FAILURE;
    }

    pstTuner_Attr = (HI_TunerAttr_S *)pstTunerDataBuf->u32DataBuf[1];
    pstDemod_Attr = (HI_DemodAttr_S *)pstTunerDataBuf->u32DataBuf[0];
    u32ResetGpioNo = pstTunerDataBuf->u32DataBuf[2];

    /* Modified begin: l00185424, for outer demod configure, 2012-01-19 */
    /* in terms of g_tuner_type, register tuner service function */
    s32Ret = tuner_select_type(u32TunerPort, pstTuner_Attr, pstDemod_Attr, u32ResetGpioNo);
    /* Modified end: l00185424, for outer demod configure, 2012-01-19 */
    if (HI_SUCCESS != s32Ret)
    {
        HI_INFO_TUNER( "tuner_select_type failed!\n"); //1374
    }

    return s32Ret;
}

HI_S32 tuner_osr_selecti2cchannel(PTR_TUNER_DATA_S pstTunerDataStr)
{
    HI_U32 u32TunerPort = 0;
    HI_U8 enI2cChannel = /*HI_UNF_I2C_CHANNEL_MAX*/16;

    u32TunerPort = pstTunerDataStr->u32Port;
    if (HI_SUCCESS != tuner_check_port(u32TunerPort))
    {
        HI_INFO_TUNER( "invalid tuner port: %d\n", u32TunerPort); //1388
        return HI_FAILURE;
    }

    enI2cChannel = (HI_U8)pstTunerDataStr->u32Data;
    if (HI_SUCCESS != tuner_select_i2c(u32TunerPort, enI2cChannel))
    {
        HI_INFO_TUNER( "tuner_select_i2c failed!\n"); //1395
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 tuner_test_single_agc(AGC_TEST_S *pstAgcTest)
{
    HI_U32 u32TunerPort = 0;

    /* get tuner port  number and if tuner port is not supported , return fault */
    u32TunerPort = pstAgcTest->u32Port;
    if (HI_SUCCESS != tuner_check_port(u32TunerPort))
    {
        return HI_FAILURE;
    }

    /* if tuner connect operation is not setted, return fault */
    if (!g_stTunerOps[u32TunerPort].tuner_test_single_agc) /* check if tuner conect is supported */
    {
        HI_INFO_TUNER( "tuner service function ptr is NULL!\n"); //1416
        return -EFAULT;
    }
    g_stTunerOps[u32TunerPort].tuner_test_single_agc( u32TunerPort, pstAgcTest );

    return HI_SUCCESS;
}

HI_S32 tuner_get_freq_symb_offset(TUNER_DATABUF_S *pstTunerDataBuf)
{
    HI_U32 u32TunerPort = 0;
    HI_S32 s32Ret = 0;
    HI_U8 u8DataTmp = 0;

    /* get tuner port  number and if tuner port is not supported , return fault */
    u32TunerPort = pstTunerDataBuf->u32Port;
    if (HI_SUCCESS != tuner_check_port(u32TunerPort))
    {
        return HI_FAILURE;
    }

    /* if tuner get freq and symb offset operation is not setted, return fault */
    if (!g_stTunerOps[u32TunerPort].tuner_get_freq_symb_offset) /* check if tuner get freq and symb offset is supported */
    {
        HI_INFO_TUNER( "tuner service function ptr is NULL!\n"); //1440
        return -EFAULT;
    }

    /* tuner get freq and symb offset */
    s32Ret = g_stTunerOps[u32TunerPort].tuner_get_freq_symb_offset(u32TunerPort,
            &pstTunerDataBuf->u32DataBuf[0], &pstTunerDataBuf->u32DataBuf[1]);
    if (HI_SUCCESS != s32Ret)
    {
        HI_INFO_TUNER( "tuner_get_freq_symb_offset failed!\n"); //1449
        return s32Ret;
    }

    switch (g_stTunerOps[u32TunerPort].enDemodDevType)
    {
    case HI_UNF_DEMOD_DEV_TYPE_3130I:
    case HI_UNF_DEMOD_DEV_TYPE_3130E:
    case HI_UNF_DEMOD_DEV_TYPE_J83B:
    s32Ret = qam_read_byte(u32TunerPort, BS_STAT_1_ADDR, &u8DataTmp);
    if (HI_SUCCESS != s32Ret)
    {
        HI_INFO_TUNER( "qam_read_byte BS_STAT_1_ADDR failed!\n"); //1461
        return s32Ret;
    }

    pstTunerDataBuf->u32DataBuf[2] = u8DataTmp;
        break;
        /* For avl6211, get offset in tuner_get_freq_symb_offset */
    case HI_UNF_DEMOD_DEV_TYPE_AVL6211:
        break;

    case HI_UNF_DEMOD_DEV_TYPE_MXL101:
    case HI_UNF_DEMOD_DEV_TYPE_3136:
    case HI_UNF_DEMOD_DEV_TYPE_3136I:
        break;

    case HI_UNF_DEMOD_DEV_TYPE_MN88472:
    case HI_UNF_DEMOD_DEV_TYPE_IT9170:
    case HI_UNF_DEMOD_DEV_TYPE_IT9133:
        break;

    case HI_UNF_DEMOD_DEV_TYPE_NONE:
    case HI_UNF_DEMOD_DEV_TYPE_BUTT:
    default:
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 tuner_osr_connect_timeout(TUNER_DATABUF_S *pstDataBuf)
{
    HI_U32 u32TunerPort = 0;
    HI_U32 u32TimeOut = 0;

    u32TimeOut = pstDataBuf->u32DataBuf[0];

    /* get tuner port  number and if tuner port is not supported , return fault */
    u32TunerPort = pstDataBuf->u32Port;
    if (HI_SUCCESS != tuner_check_port(u32TunerPort))
    {
        return HI_FAILURE;
    }

    /* if tuner_connect_timeout operation is not setted, return fault */
    if (!g_stTunerOps[u32TunerPort].tuner_connect_timeout) /* check if tuner_connect_timeout is supported */
    {
        HI_INFO_TUNER( "tuner service function ptr is NULL!\n"); //1507
        return -EFAULT;
    }

    /* tuner_connect_timeout */
    g_stTunerOps[u32TunerPort].tuner_connect_timeout(u32TimeOut);

    return HI_SUCCESS;
}

HI_S32 tuner_get_signal_info(TUNER_SIGNALINFO_S *pstSignal)
{
    HI_U32 u32TunerPort = 0;
    HI_S32 s32Ret = 0;

    /* get tuner port  number and if tuner port is not supported , return fault */
    u32TunerPort = pstSignal->u32Port;
    if (HI_SUCCESS != tuner_check_port(u32TunerPort))
    {
        HI_INFO_TUNER( "Invalid Tuner Port!\n"); //1526
        return HI_FAILURE;
    }

    /* if tuner blind scan operation is not setted, return fault */
    if (!g_stTunerOps[u32TunerPort].tuner_get_signal_info) /* check if tuner get signal info is supported */
    {
        HI_INFO_TUNER( "tuner service function ptr is NULL!\n"); //1533
        return -EFAULT;
    }

    s32Ret = g_stTunerOps[u32TunerPort].tuner_get_signal_info(u32TunerPort, &(pstSignal->stInfo));
    if (HI_SUCCESS != s32Ret)
    {
        HI_INFO_TUNER( "tuner_get_signal_info failed!\n"); //1540
        return s32Ret;
    }

    return HI_SUCCESS;
}

HI_S32 tuner_blindscan_init(TUNER_BLINDSCAN_S *pstBlindScan)
{
    HI_U32 u32TunerPort = 0;
    HI_S32 s32Ret = 0;

    /* get tuner port  number and if tuner port is not supported , return fault */
    u32TunerPort = pstBlindScan->u32Port;
    if (HI_SUCCESS != tuner_check_port(u32TunerPort))
    {
        HI_INFO_TUNER( "Invalid Tuner Port!\n"); //1556
        return HI_FAILURE;
    }

    /* if tuner blind scan operation is not setted, return fault */
    if (!g_stTunerOps[u32TunerPort].tuner_blindscan_init) /* check if tuner get freq and symb offset is supported */
    {
        HI_INFO_TUNER( "tuner service function ptr is NULL!\n"); //1563
        return -EFAULT;
    }

    s32Ret = g_stTunerOps[u32TunerPort].tuner_blindscan_init(u32TunerPort, &(pstBlindScan->stPara));
    if (HI_SUCCESS != s32Ret)
    {
        HI_INFO_TUNER( "tuner_blindscan_init failed!\n"); //1570
        return s32Ret;
    }

    return HI_SUCCESS;
}

HI_S32 tuner_blindscan_action(TUNER_BLINDSCAN_INFO_S *pstInfo)
{
    HI_U32 u32TunerPort = 0;
    HI_S32 s32Ret = 0;

    /* get tuner port  number and if tuner port is not supported , return fault */
    u32TunerPort = pstInfo->u32Port;
    if (HI_SUCCESS != tuner_check_port(u32TunerPort))
    {
        HI_INFO_TUNER( "Invalid Tuner Port!\n"); //1586
        return HI_FAILURE;
    }

    /* if tuner blind scan operation is not setted, return fault */
    if (!g_stTunerOps[u32TunerPort].tuner_blindscan_action) /* check if tuner get freq and symb offset is supported */
    {
        HI_INFO_TUNER( "tuner service function ptr is NULL!\n"); //1593
        return -EFAULT;
    }

    /* tuner blind scan */
    s32Ret = g_stTunerOps[u32TunerPort].tuner_blindscan_action(u32TunerPort, pstInfo->pstPara);
    if (HI_SUCCESS != s32Ret)
    {
        HI_INFO_TUNER( "tuner_blindscan_action failed!\n"); //1601
        return s32Ret;
    }

    return HI_SUCCESS;
}

HI_S32 tuner_terscan_action(PTR_TUNER_TERRSCAN_INFO_S pstInfo)
{
    HI_U32 u32TunerPort = 0;
    HI_S32 s32Ret = 0;

    /* get tuner port  number and if tuner port is not supported , return fault */
    u32TunerPort = pstInfo->u32Port;
    if (HI_SUCCESS != tuner_check_port(u32TunerPort))
    {
        HI_INFO_TUNER( "Invalid Tuner Port!\n"); //1617
        return HI_FAILURE;
    }

    /* if tuner blind scan operation is not setted, return fault */
    if (!g_stTunerOps[u32TunerPort].Func_412) /* check if tuner get freq and symb offset is supported */
    {
        HI_INFO_TUNER( "tuner service function ptr is NULL!\n"); //1624
        return -EFAULT;
    }

    /* tuner blind scan */
    s32Ret = g_stTunerOps[u32TunerPort].Func_412(u32TunerPort, &(pstInfo->Data_4), &(pstInfo->Data_24));
    if (HI_SUCCESS != s32Ret)
    {
        HI_INFO_TUNER( "tuner_terscan_action failed!\n"); //1632
        return s32Ret;
    }

    return HI_SUCCESS;
}

HI_S32 tuner_set_lnb_out(TUNER_LNB_OUT_S *pstLNBOut)
{
    HI_U32 u32TunerPort = 0;
    HI_S32 s32Ret = 0;

    /* get tuner port  number and if tuner port is not supported , return fault */
    u32TunerPort = pstLNBOut->u32Port;
    if (HI_SUCCESS != tuner_check_port(u32TunerPort))
    {
        HI_INFO_TUNER( "Invalid Tuner Port!\n"); //1648
        return HI_FAILURE;
    }

    /* if tuner blind scan operation is not setted, return fault */
    if (!g_stTunerOps[u32TunerPort].tuner_set_lnb_out)
    {
        HI_INFO_TUNER( "tuner service function ptr is NULL!\n"); //1655
        return -EFAULT;
    }

    s32Ret = g_stTunerOps[u32TunerPort].tuner_set_lnb_out(u32TunerPort, pstLNBOut->enOut);
    if (HI_SUCCESS != s32Ret)
    {
        HI_INFO_TUNER( "tuner_set_lnb_out failed!\n"); //1662
        return s32Ret;
    }

    s_stSatTunerResumeInfo[u32TunerPort].enLNBOut = pstLNBOut->enOut;
    return HI_SUCCESS;
}

HI_S32 tuner_send_continuous_22K(TUNER_DATA_S *pstLNB22K)
{
    HI_U32 u32TunerPort = 0;
    HI_S32 s32Ret = 0;

    /* get tuner port  number and if tuner port is not supported , return fault */
    u32TunerPort = pstLNB22K->u32Port;
    if (HI_SUCCESS != tuner_check_port(u32TunerPort))
    {
        HI_INFO_TUNER( "Invalid Tuner Port!\n"); //1679
        return HI_FAILURE;
    }

    /* if tuner blind scan operation is not setted, return fault */
    if (!g_stTunerOps[u32TunerPort].tuner_send_continuous_22K)
    {
        HI_INFO_TUNER( "tuner service function ptr is NULL!\n"); //1686
        return -EFAULT;
    }

    s32Ret = g_stTunerOps[u32TunerPort].tuner_send_continuous_22K(u32TunerPort, pstLNB22K->u32Data);
    if (HI_SUCCESS != s32Ret)
    {
        HI_INFO_TUNER( "tuner_send_continuous_22K failed!\n"); //1693
        return s32Ret;
    }
    s_stSatTunerResumeInfo[u32TunerPort].u32Continuous22K = pstLNB22K->u32Data;

    return HI_SUCCESS;
}

HI_S32 tuner_send_tone(TUNER_DATA_S *pstTone)
{
    HI_U32 u32TunerPort = 0;
    HI_S32 s32Ret = 0;

    /* get tuner port  number and if tuner port is not supported , return fault */
    u32TunerPort = pstTone->u32Port;
    if (HI_SUCCESS != tuner_check_port(u32TunerPort))
    {
        HI_INFO_TUNER( "Invalid Tuner Port!\n"); //1710
        return HI_FAILURE;
    }

    /* if tuner blind scan operation is not setted, return fault */
    if (!g_stTunerOps[u32TunerPort].tuner_send_tone)
    {
        HI_INFO_TUNER( "tuner service function ptr is NULL!\n"); //1717
        return -EFAULT;
    }

    s32Ret = g_stTunerOps[u32TunerPort].tuner_send_tone(u32TunerPort, pstTone->u32Data);
    if (HI_SUCCESS != s32Ret)
    {
        HI_INFO_TUNER( "tuner_send_tone failed!\n"); //1724
        return s32Ret;
    }

    return HI_SUCCESS;
}

HI_S32 tuner_DiSEqC_send_msg(TUNER_DISEQC_SENDMSG_S *pstPara)
{
    HI_U32 u32TunerPort = 0;
    HI_S32 s32Ret = 0;

    /* get tuner port  number and if tuner port is not supported , return fault */
    u32TunerPort = pstPara->u32Port;
    if (HI_SUCCESS != tuner_check_port(u32TunerPort))
    {
        HI_INFO_TUNER( "Invalid Tuner Port!\n"); //1740
        return HI_FAILURE;
    }

    /* if tuner unsupport DiSEqC, return fault */
    if (!g_stTunerOps[u32TunerPort].tuner_DiSEqC_send_msg) /* check if tuner get freq and symb offset is supported */
    {
        HI_INFO_TUNER( "tuner service function ptr is NULL!\n"); //1747
        return -EFAULT;
    }

    s32Ret = g_stTunerOps[u32TunerPort].tuner_DiSEqC_send_msg(u32TunerPort, &(pstPara->stSendMsg));
    if (HI_SUCCESS != s32Ret)
    {
        HI_INFO_TUNER( "tuner_DiSEqC_send_msg failed!\n"); //1754
        return s32Ret;
    }

    return HI_SUCCESS;
}

HI_S32 tuner_DiSEqC_recv_msg(TUNER_DISEQC_RECVMSG_S *pstPara)
{
    HI_U32 u32TunerPort = 0;
    HI_S32 s32Ret = 0;

    /* get tuner port  number and if tuner port is not supported , return fault */
    u32TunerPort = pstPara->u32Port;
    if (HI_SUCCESS != tuner_check_port(u32TunerPort))
    {
        HI_INFO_TUNER( "Invalid Tuner Port!\n"); //1770
        return HI_FAILURE;
    }

    /* if tuner unsupport DiSEqC, return fault */
    if (!g_stTunerOps[u32TunerPort].tuner_DiSEqC_recv_msg) /* check if tuner get freq and symb offset is supported */
    {
        HI_INFO_TUNER( "tuner service function ptr is NULL!\n"); //1777
        return -EFAULT;
    }

    s32Ret = g_stTunerOps[u32TunerPort].tuner_DiSEqC_recv_msg(u32TunerPort, pstPara->pstRecvMsg);
    if (HI_SUCCESS != s32Ret)
    {
        HI_INFO_TUNER( "tuner_DiSEqC_recv_msg failed!\n"); //1784
        return s32Ret;
    }

    return HI_SUCCESS;
}

HI_S32 tuner_standby(TUNER_DATA_S *pstPara)
{
    HI_U32 u32TunerPort = 0;
    HI_S32 s32Ret = 0;

    /* get tuner port  number and if tuner port is not supported , return fault */
    u32TunerPort = pstPara->u32Port;
    if (HI_SUCCESS != tuner_check_port(u32TunerPort))
    {
        HI_INFO_TUNER( "Invalid Tuner Port!\n"); //1800
        return HI_FAILURE;
    }

    /* if tuner blind scan operation is not setted, return fault */
    if (!g_stTunerOps[u32TunerPort].tuner_standby)
    {
        HI_INFO_TUNER( "tuner service function ptr is NULL!\n"); //1807
        return -EFAULT;
    }

    s32Ret = g_stTunerOps[u32TunerPort].tuner_standby(u32TunerPort, pstPara->u32Data);
    if (HI_SUCCESS != s32Ret)
    {
        HI_INFO_TUNER( "tuner_standby failed!\n"); //1814
        return s32Ret;
    }

    /* If board has LNB control device, standby here */
    if (g_stTunerOps[u32TunerPort].tuner_lnbctrl_dev_standby)
    {
        s32Ret = g_stTunerOps[u32TunerPort].tuner_lnbctrl_dev_standby(u32TunerPort, pstPara->u32Data);
        if (HI_SUCCESS != s32Ret)
        {
            HI_INFO_TUNER( "tuner_standby failed!\n"); //1824
            return s32Ret;
        }
    }

    return HI_SUCCESS;
}

#if 0
HI_S32 tuner_disable(TUNER_DATA_S *pstPara)
{
    HI_U32 u32TunerPort = 0;
    HI_S32 s32Ret = 0;

    /* get tuner port  number and if tuner port is not supported , return fault */
    u32TunerPort = pstPara->u32Port;
    if (HI_SUCCESS != tuner_check_port(u32TunerPort))
    {
        HI_INFO_TUNER( "Invalid Tuner Port!\n");
        return HI_FAILURE;
    }

    /* if tuner blind scan operation is not setted, return fault */
    if (!g_stTunerOps[u32TunerPort].tuner_disable)
    {
        HI_INFO_TUNER( "tuner service function ptr is NULL!\n");
        return -EFAULT;
    }

    s32Ret = g_stTunerOps[u32TunerPort].tuner_disable(u32TunerPort, pstPara->u32Data);
    if (HI_SUCCESS != s32Ret)
    {
        HI_INFO_TUNER( "tuner_disable failed!\n");
        return s32Ret;
    }

    return HI_SUCCESS;
}
#endif

HI_S32 tuner_setfuncmode(TUNER_DATA_S *pstPara)
{
    HI_U32 u32TunerPort = 0;
    HI_S32 s32Ret = 0;
    TUNER_FUNC_MODE_E enFuncMode;

    /* get tuner port  number and if tuner port is not supported , return fault */
    u32TunerPort = pstPara->u32Port;
    if (HI_SUCCESS != tuner_check_port(u32TunerPort))
    {
        HI_INFO_TUNER( "Invalid Tuner Port!\n"); //1874
        return HI_FAILURE;
    }

    /* if tuner set func mode operation is not setted, return fault */
    if (!g_stTunerOps[u32TunerPort].tuner_setfuncmode)
    {
        HI_INFO_TUNER( "tuner service function ptr is NULL!\n"); //1881
        return -EFAULT;
    }

    enFuncMode = pstPara->u32Data;
    s32Ret = g_stTunerOps[u32TunerPort].tuner_setfuncmode(u32TunerPort, enFuncMode);
    if (HI_SUCCESS != s32Ret)
    {
        HI_INFO_TUNER( "tuner_setfuncmode failed!\n"); //1889
        return s32Ret;
    }

    return HI_SUCCESS;
}

HI_S32 tuner_setplpid(PTR_TUNER_DATABUF_S pstPara)
{
    HI_U32 u32TunerPort = 0;
    HI_S32 s32Ret = 0;
    HI_U8 u8PLPID = 0;
    HI_U32 u32Data = 0;

    /* get tuner port  number and if tuner port is not supported , return fault */
    u32TunerPort = pstPara->u32Port;
    if (HI_SUCCESS != tuner_check_port(u32TunerPort))
    {
        HI_INFO_TUNER( "Invalid Tuner Port!\n"); //1906
        return HI_FAILURE;
    }

    /* if tuner set plp no operation is not setted, return fault */
    if (!g_stTunerOps[u32TunerPort].tuner_setplpid)
    {
        HI_INFO_TUNER( "tuner service function ptr is NULL!\n"); //1913
        return -EFAULT;
    }

    u8PLPID = (pstPara->u32DataBuf[0]) & 0xFF;
    u32Data = pstPara->u32DataBuf[1];
    s32Ret = g_stTunerOps[u32TunerPort].tuner_setplpid(u32TunerPort, u8PLPID, u32Data);
    if (HI_SUCCESS != s32Ret)
    {
        HI_INFO_TUNER( "tuner_setplpid failed!\n"); //1922
        return s32Ret;
    }

    return HI_SUCCESS;
}

HI_S32 tuner_set_common_plp_id(PTR_TUNER_DATA_456_S pstTunerDataStr)
{
    HI_U32 u32TunerPort = 0;
    HI_S32 s32Ret = 0;
    HI_U8 u8Data = 0;


    u32TunerPort = pstTunerDataStr->u32Port;
    if (HI_SUCCESS != tuner_check_port(u32TunerPort))
    {
    	HI_ERR_TUNER("Invalid Tuner Port!\n"); //1939
        return HI_FAILURE;
    }

    if (!g_stTunerOps[u32TunerPort].Func_456)
    {
        HI_ERR_TUNER( "tuner service function ptr is NULL!\n"); //1946
        return -EFAULT;
    }

    u8Data = pstTunerDataStr->u8Data; //(pstPara->u32Data) & 0xFF;
    s32Ret = g_stTunerOps[u32TunerPort].Func_456(u32TunerPort, u8Data);
    if (HI_SUCCESS != s32Ret)
    {
        HI_ERR_TUNER("tuner_set_common_plp_id failed!\n"); //1954
        return s32Ret;
    }

    return s32Ret;
}

HI_S32 tuner_set_common_plp_combination(PTR_TUNER_DATA_S pstTunerDataStr)
{
    HI_U32 u32TunerPort = 0;
    HI_S32 s32Ret = 0;
    HI_U8 u8Data = 0;

    u32TunerPort = pstTunerDataStr->u32Port;
    if (HI_SUCCESS != tuner_check_port(u32TunerPort))
    {
    	HI_ERR_TUNER("Invalid Tuner Port!\n"); //1971
        return HI_FAILURE;
    }


    if (!g_stTunerOps[u32TunerPort].Func_460)
    {
        HI_ERR_TUNER( "tuner service function ptr is NULL!\n"); //1978
        return -EFAULT;
    }

    u8Data = (pstTunerDataStr->u32Data) & 0xFF;
    s32Ret = g_stTunerOps[u32TunerPort].Func_460(u32TunerPort, u8Data);
    if (HI_SUCCESS != s32Ret)
    {
        HI_ERR_TUNER("tuner_set_common_plp_combination failed!\n"); //1986
        return s32Ret;
    }

    return s32Ret;
}

HI_S32 tuner_osr_getplpnum(PTR_TUNER_DATA_S pstTunerDataStr)
{
    HI_U32 u32TunerPort = 0;
    HI_S32 s32Ret = 0;

    /* get tuner port  number and if tuner port is not supported , return fault */
    u32TunerPort = pstTunerDataStr->u32Port;
    if (HI_SUCCESS != tuner_check_port(u32TunerPort))
    {
        return HI_FAILURE;
    }

    /* if tuner get status operation is not setted, return fault */
    if (!g_stTunerOps[u32TunerPort].tuner_get_plp_num) /* check if tuner get status is supported */
    {
        HI_INFO_TUNER( "tuner service function ptr is NULL!\n"); //2008
        return -EFAULT;
    }

    /* get tuner lock status */
    s32Ret = g_stTunerOps[u32TunerPort].tuner_get_plp_num(u32TunerPort,
                                    (HI_U8 *)&pstTunerDataStr->u32Data);
    if (HI_SUCCESS != s32Ret)
    {
        HI_INFO_TUNER( "\n tuner_get_status failed \n"); //2017
        return s32Ret;
    }

    return HI_SUCCESS;
}

HI_S32 tuner_osr_getcurplptype(PTR_TUNER_DATA_S pstTunerDataStr)
{
    HI_U32 u32TunerPort = 0;
    HI_S32 s32Ret = 0;

    /* get tuner port  number and if tuner port is not supported , return fault */
    u32TunerPort = pstTunerDataStr->u32Port;
    if (HI_SUCCESS != tuner_check_port(u32TunerPort))
    {
        return HI_FAILURE;
    }

    /* if tuner get status operation is not setted, return fault */
    if (!g_stTunerOps[u32TunerPort].tuner_get_current_plp_type) /* check if tuner_get_current_plp_type is supported */
    {
        HI_INFO_TUNER( "tuner service function ptr is NULL!\n"); //2039
        return -EFAULT;
    }

    /* tuner_get_current_plp_type */
    s32Ret = g_stTunerOps[u32TunerPort].tuner_get_current_plp_type(u32TunerPort,
                                    (HI_UNF_TUNER_T2_PLP_TYPE_E *)&pstTunerDataStr->u32Data);
    if (HI_SUCCESS != s32Ret)
    {
        HI_INFO_TUNER( "\n tuner_get_status failed \n"); //2047
        return s32Ret;
    }

    return HI_SUCCESS;
}

HI_S32 tuner_osr_getplpid(PTR_TUNER_DATA_S pstTunerDataStr)
{
    HI_U32 u32TunerPort = 0;
    HI_S32 s32Ret = 0;
    HI_U8 u8Data = 0;

    u32TunerPort = pstTunerDataStr->u32Port;
    if (HI_SUCCESS != tuner_check_port(u32TunerPort))
    {
//    	HI_ERR_TUNER("Invalid Tuner Port!\n"); //
        return HI_FAILURE;
    }


    if (!g_stTunerOps[u32TunerPort].Func_472)
    {
        HI_INFO_TUNER( "tuner service function ptr is NULL!\n"); //2069
        return -EFAULT;
    }

    s32Ret = g_stTunerOps[u32TunerPort].Func_472(u32TunerPort, &(pstTunerDataStr->u32Data));
    if (HI_SUCCESS != s32Ret)
    {
        HI_INFO_TUNER("\n tuner_get_plp_id failed \n"); //2077
        return s32Ret;
    }

    return s32Ret;
}

HI_S32 tuner_osr_getgroupplpid(PTR_TUNER_DATA_S pstTunerDataStr)
{
    HI_U32 u32TunerPort = 0;
    HI_S32 s32Ret = 0;
    HI_U8 u8Data = 0;

    u32TunerPort = pstTunerDataStr->u32Port;
    if (HI_SUCCESS != tuner_check_port(u32TunerPort))
    {
//    	HI_ERR_TUNER("Invalid Tuner Port!\n"); //
        return HI_FAILURE;
    }


    if (!g_stTunerOps[u32TunerPort].Func_476)
    {
        HI_INFO_TUNER( "tuner service function ptr is NULL!\n"); //2099
        return -EFAULT;
    }

    s32Ret = g_stTunerOps[u32TunerPort].Func_476(u32TunerPort, &(pstTunerDataStr->u32Data));
    if (HI_SUCCESS != s32Ret)
    {
        HI_INFO_TUNER("\n tuner_get_group_plp_id failed \n"); //2107
        return s32Ret;
    }

    return s32Ret;
}

HI_S32 tuner_osr_setantennapower(PTR_TUNER_DATA_S pstTunerDataStr)
{
    HI_U32 u32TunerPort = 0;
    HI_S32 s32Ret = 0;
    HI_U32 u32Data = 0;


    u32TunerPort = pstTunerDataStr->u32Port;
    if (HI_SUCCESS != tuner_check_port(u32TunerPort))
    {
//    	HI_ERR_TUNER("Invalid Tuner Port!\n"); //
        return HI_FAILURE;
    }

    if (!g_stTunerOps[u32TunerPort].Func_480)
    {
        HI_ERR_TUNER( "tuner service function ptr is NULL!\n"); //2129
        return -EFAULT;
    }

    u32Data = pstTunerDataStr->u32Data;
    s32Ret = g_stTunerOps[u32TunerPort].Func_480(u32TunerPort, u32Data);
    if (HI_SUCCESS != s32Ret)
    {
        HI_ERR_TUNER("\n tuner_set_antena_power failed \n"); //2138
        return s32Ret;
    }

    return s32Ret;
}

HI_S32 tuner_osr_settsout(PTR_TUNER_DATA_S pstTunerDataStr)
{
    HI_U32 u32TunerPort = 0;
    HI_S32 s32Ret = 0;

    /* get tuner port  number and if tuner port is not supported , return fault */
    u32TunerPort = pstTunerDataStr->u32Port;
    if (HI_SUCCESS != tuner_check_port(u32TunerPort))
    {
        return HI_FAILURE;
    }

    /* if tuner get status operation is not setted, return fault */
    if (!g_stTunerOps[u32TunerPort].tuner_set_ts_out) /* check if tuner_get_current_plp_type is supported */
    {
        HI_ERR_TUNER( "tuner service function ptr is NULL!\n"); //2160
        return -EFAULT;
    }

    /* tuner_set_ts_out */
    s32Ret = g_stTunerOps[u32TunerPort].tuner_set_ts_out(u32TunerPort,
                                    (HI_UNF_TUNER_TSOUT_SET_S *)(pstTunerDataStr->u32Data));
    if (HI_SUCCESS != s32Ret)
    {
        HI_ERR_TUNER( "\n tuner_get_status failed \n"); //2169
        return s32Ret;
    }

    return HI_SUCCESS;
}

HI_S32 tuner_osr_tpverify(TUNER_TP_VERIFY_INFO_S *pstTunerTPVerifyInfo)
{
    HI_U32 u32TunerPort = 0;
    HI_S32 s32Ret = 0;

    /* get tuner port  number and if tuner port is not supported , return fault */
    u32TunerPort = pstTunerTPVerifyInfo->u32Port;
    if (HI_SUCCESS != tuner_check_port(u32TunerPort))
    {
        return HI_FAILURE;
    }

    /* if tuner get status operation is not setted, return fault */
    if (!g_stTunerOps[u32TunerPort].tuner_tp_verify) /* check if tuner_get_current_plp_type is supported */
    {
        HI_ERR_TUNER( "tuner service function ptr is NULL!\n"); //2191
        return -EFAULT;
    }

    /* tuner_get_current_plp_type */
    s32Ret = g_stTunerOps[u32TunerPort].tuner_tp_verify(u32TunerPort,
                                    &(pstTunerTPVerifyInfo->stTPVerifyPrm));
    if (HI_SUCCESS != s32Ret)
    {
        HI_ERR_TUNER( "\n tuner_tp_verify failed \n"); //2200
        return s32Ret;
    }

    return HI_SUCCESS;
}

HI_S32 tuner_osr_setsatattr(PTR_TUNER_DATA_S pstTunerDataStr)
{
    HI_U32 u32TunerPort = 0;
    HI_S32 s32Ret = 0;
    HI_UNF_TUNER_SAT_ATTR_S *pstSatAttr;

    /* get tuner port  number and if tuner port is not supported , return fault */
    u32TunerPort = pstTunerDataStr->u32Port;
    if (HI_SUCCESS != tuner_check_port(u32TunerPort))
    {
        return HI_FAILURE;
    }

    pstSatAttr = (HI_UNF_TUNER_SAT_ATTR_S *)pstTunerDataStr->u32Data;

    s32Ret = tuner_set_sat_attr(u32TunerPort, pstSatAttr);
    /* Modified end: l00185424, for outer demod configure, 2012-01-19 */
    if (HI_SUCCESS != s32Ret)
    {
        HI_INFO_TUNER( "tuner_select_type failed!\n"); //2226
    }

    return s32Ret;
}

HI_S32 tuner_osr_setterattr(PTR_TUNER_DATA_S pstTunerDataStr)
{
    HI_U32 u32TunerPort = 0;
    HI_S32 s32Ret = 0;
    HI_UNF_TUNER_SAT_ATTR_S *pstSatAttr;

    /* get tuner port  number and if tuner port is not supported , return fault */
    u32TunerPort = pstTunerDataStr->u32Port;
    if (HI_SUCCESS != tuner_check_port(u32TunerPort))
    {
        return HI_FAILURE;
    }

    pstSatAttr = (HI_UNF_TUNER_SAT_ATTR_S *)pstTunerDataStr->u32Data;

    s32Ret = tuner_set_ter_attr(u32TunerPort, pstSatAttr);
    /* Modified end: l00185424, for outer demod configure, 2012-01-19 */
    if (HI_SUCCESS != s32Ret)
    {
        HI_ERR_TUNER("tuner_osr_setterattr failed!\n"); //2251
    }

    return s32Ret;
}

HI_S32 tuner_osr_sampledata(PTR_TUNER_DATA_S pstTunerDataStr)
{
    HI_U32 u32TunerPort = 0;
    HI_S32 s32Ret = 0;
    TUNER_SAMPLE_DATA_PARAM_S *pstSampleParam = 0;

    /* get tuner port  number and if tuner port is not supported , return fault */
    u32TunerPort = pstTunerDataStr->u32Port;
    if (HI_SUCCESS != tuner_check_port(u32TunerPort))
    {
        return HI_FAILURE;
    }

    pstSampleParam = (TUNER_SAMPLE_DATA_PARAM_S *)(pstTunerDataStr->u32Data);

    /* if tuner get status operation is not setted, return fault */
    if (!g_stTunerOps[u32TunerPort].tuner_data_sample) /* check if tuner_get_current_plp_type is supported */
    {
        HI_ERR_TUNER( "tuner service function ptr is NULL!\n"); //2275
        return -EFAULT;
    }

    /* tuner_get_current_plp_type */
    s32Ret = g_stTunerOps[u32TunerPort].tuner_data_sample(u32TunerPort, pstSampleParam->enDataSrc, pstSampleParam->u32DataLen, \
                                    pstSampleParam->pstData);
    if (HI_SUCCESS != s32Ret)
    {
        HI_ERR_TUNER( "\n tuner_data_sample failed \n"); //2284
        return s32Ret;
    }

    return s32Ret;
}
/*
 *    ioctl method.  enable user to contrl tuner
 *    parameter:
 *        TUNER_CONNECT_CMD:  tuner connect
 *        TUNER_GET_STATUS_CMD:  get the status of tuner
 *        TUNER_GET_SIGNALSTRENGTH_CMD:  get signal strength from tuner
 *        TUNER_GET_BER_CMD:   get BER from tuner
 *        TUNER_GET_SNR_CMD:   get SNR from tuner
 *        TUNER_SET_TSTYPE_CMD: set ts stream format
 *       TUNER_SELECT_TYPE_CMD: select tuner type
 *
 */
HI_S32 tuner_ioctl(struct inode *inode, struct file *file, HI_U32 cmd, HI_VOID *arg)
{
    HI_S32 s32Ret = HI_FAILURE;

    if (HI_NULL == arg)
    {
        HI_INFO_TUNER("copy data from user error: cmd=%d\n", _IOC_NR(cmd)); //2308
        return HI_FAILURE;
    }

    switch (cmd)
    {
        case TUNER_CONNECT_CMD:
        case TUNER_CONNECTUNBLOCK_CMD:
            {
                TUNER_SIGNAL_S *pstPara = (TUNER_SIGNAL_S *)arg;
                s32Ret = tuner_osr_connect(pstPara);
                break;
            }
        case TUNER_GET_STATUS_CMD:
            {
                TUNER_DATA_S *pstPara = (TUNER_DATA_S *)arg;
                s32Ret = tuner_osr_getstatus(pstPara);
                break;
            }
        case TUNER_GET_SIGNALSTRENGTH_CMD:
            {
                TUNER_DATABUF_S *pstPara = (TUNER_DATABUF_S *)arg;
                s32Ret = tuner_osr_getsignalstrength(pstPara);
                break;
            }
        case TUNER_GET_BER_CMD:
            {
                TUNER_DATABUF_S *pstPara = (TUNER_DATABUF_S *)arg;
                s32Ret = tuner_osr_getber(pstPara);
                break;
            }
        case TUNER_GET_SNR_CMD:
            {
                TUNER_DATA_S *pstPara = (TUNER_DATA_S *)arg;
                s32Ret = tuner_osr_getsnr(pstPara);
                break;
            }
        case TUNER_SET_TSTYPE_CMD:
            {
                TUNER_DATA_S *pstPara = (TUNER_DATA_S *)arg;
                s32Ret = tuner_osr_settstype(pstPara);
                break;
            }
        case TUNER_SELECT_TYPE_CMD:
            {
                TUNER_DATABUF_S *pstPara = (TUNER_DATABUF_S *)arg;
                s32Ret = tuner_osr_selecttype(pstPara);
                break;
            }
        case TUNER_SELECT_I2C_CMD:
            {
                TUNER_DATA_S *pstPara = (TUNER_DATA_S *)arg;
                s32Ret = tuner_osr_selecti2cchannel(pstPara);
                break;
            }
        case TUNER_TEST_SINGLE_AGC:
            {
                AGC_TEST_S *pstPara = (AGC_TEST_S *)arg;
                s32Ret = tuner_test_single_agc(pstPara);
                break;
            }
        case TUNER_GET_FREQ_SYMB_OFFSET:
            {
                TUNER_DATABUF_S *pstPara = (TUNER_DATABUF_S *)arg;
                s32Ret = tuner_get_freq_symb_offset(pstPara);
                break;
            }
        case TUNER_CONNECT_TIMEOUT_CMD:
            {
                TUNER_DATABUF_S *pstPara = (TUNER_DATABUF_S *)arg;
                s32Ret = tuner_osr_connect_timeout(pstPara);
                break;
            }
        case TUNER_GET_SIGANLINFO_CMD:
        {
            TUNER_SIGNALINFO_S *pstPara = (TUNER_SIGNALINFO_S *)arg;
            s32Ret = tuner_get_signal_info(pstPara);
            break;
        }
        case TUNER_BLINDSCAN_INIT_CMD:
        {
            TUNER_BLINDSCAN_S *pstPara = (TUNER_BLINDSCAN_S *)arg;
            s32Ret = tuner_blindscan_init(pstPara);
            break;
        }
        case TUNER_BLINDSCAN_ACTION_CMD:
        {
            TUNER_BLINDSCAN_INFO_S *pstPara = (TUNER_BLINDSCAN_INFO_S *)arg;
            s32Ret = tuner_blindscan_action(pstPara);
            break;
        }
        case TUNER_SET_LNBOUT_CMD:
        {
            TUNER_LNB_OUT_S *pstPara = (TUNER_LNB_OUT_S *)arg;
            s32Ret = tuner_set_lnb_out(pstPara);
            break;
        }
        case TUNER_SEND_CONTINUOUS_22K_CMD:
        {
            TUNER_DATA_S *pstPara = (TUNER_DATA_S *)arg;
            s32Ret = tuner_send_continuous_22K(pstPara);
            break;
        }
        case TUNER_SEND_TONE_CMD:
        {
            TUNER_DATA_S *pstPara = (TUNER_DATA_S *)arg;
            s32Ret = tuner_send_tone(pstPara);
            break;
        }
        case TUNER_DISEQC_SEND_MSG_CMD:
        {
            TUNER_DISEQC_SENDMSG_S *pstPara = (TUNER_DISEQC_SENDMSG_S *)arg;
            s32Ret = tuner_DiSEqC_send_msg(pstPara);
            break;
        }
        case TUNER_DISEQC_RECV_MSG_CMD:
        {
            TUNER_DISEQC_RECVMSG_S *pstPara = (TUNER_DISEQC_RECVMSG_S *)arg;
            s32Ret = tuner_DiSEqC_recv_msg(pstPara);
            break;
        }
        case TUNER_STANDBY_CMD:
        {
            TUNER_DATA_S *pstPara = (TUNER_DATA_S *)arg;
            s32Ret = tuner_standby(pstPara);
            break;
        }
        /*case TUNER_DISABLE_CMD:
        {
            TUNER_DATA_S *pstPara = (TUNER_DATA_S *)arg;
            s32Ret = tuner_disable(pstPara);
            break;
        }*/
        case TUNER_SETFUNCMODE_CMD:
        {
            TUNER_DATA_S *pstPara = (TUNER_DATA_S *)arg;
            s32Ret = tuner_setfuncmode(pstPara);
            break;
        }
        case TUNER_SETPLPNO_CMD:
        {
        	TUNER_DATABUF_S *pstPara = (TUNER_DATABUF_S *)arg;
            s32Ret = tuner_setplpid(pstPara);
            break;
        }
        case TUNER_GETPLPNUM_CMD:
        {
            TUNER_DATA_S *pstPara = (TUNER_DATA_S *)arg;
            s32Ret = tuner_osr_getplpnum(pstPara);
            break;
        }
        case TUNER_GETCURPLPTYPE_CMD:
        {
            TUNER_DATA_S *pstPara = (TUNER_DATA_S *)arg;
            s32Ret = tuner_osr_getcurplptype(pstPara);
            break;
        }
        case TUNER_SETTSOUT_CMD:
        {
            TUNER_DATA_S *pstPara = (TUNER_DATA_S *)arg;
            s32Ret = tuner_osr_settsout(pstPara);
            break;
        }
        case TUNER_TPVERIFY_CMD:
        {
            TUNER_TP_VERIFY_INFO_S *pstPara = (TUNER_TP_VERIFY_INFO_S *)arg;
            s32Ret = tuner_osr_tpverify(pstPara);
            break;
        }
        case TUNER_SETSATATTR_CMD:
        {
            TUNER_DATA_S *pstPara = (TUNER_DATA_S *)arg;
        	if (g_stTunerOps[pstPara->u32Port].enDemodDevType == 0x10c)
        	{
        		s32Ret = tuner_osr_setterattr(pstPara);
        	}
        	else
        	{
        		s32Ret = tuner_osr_setsatattr(pstPara);
        	}
            break;
        }
        case TUNER_SAMPLE_DATA_CMD:
        {
            TUNER_DATA_S *pstPara = (TUNER_DATA_S *)arg;
            s32Ret = tuner_osr_sampledata(pstPara);
            break;
        }

        case TUNER_SET_COMMONPLPID_CMD:
        {
            TUNER_DATA_S *pstPara = (TUNER_DATA_S *)arg;
            s32Ret = tuner_set_common_plp_id(pstPara);
        	break;
        }

        case TUNER_SET_COMMONPLPCOMBINATION_CMD:
        {
            TUNER_DATA_S *pstPara = (TUNER_DATA_S *)arg;
            s32Ret = tuner_set_common_plp_combination(pstPara);
        	break;
        }

        case TUNER_GET_PLPID_CMD:
        {
            TUNER_DATA_S *pstPara = (TUNER_DATA_S *)arg;
            s32Ret = tuner_osr_getplpid(pstPara);
        	break;
        }

        case TUNER_GET_GROUPPLPID_CMD:
        {
            TUNER_DATA_S *pstPara = (TUNER_DATA_S *)arg;
            s32Ret = tuner_osr_getgroupplpid(pstPara);
        	break;
        }

        case TUNER_SET_ANTENNAPOWER_CMD:
        {
            TUNER_DATA_S *pstPara = (TUNER_DATA_S *)arg;
        	s32Ret = tuner_osr_setantennapower(pstPara);
        	break;
        }

        case TUNER_TERSCAN_ACTION_CMD:
        {
        	TUNER_TERRSCAN_INFO_S *pstPara = (TUNER_TERRSCAN_INFO_S *)arg;
            s32Ret = tuner_terscan_action(pstPara);
            break;
        }

        default:
        {
            s32Ret= -EFAULT;
            break;
        }
    }
    return s32Ret;
}

long hi_tuner_ioctl(struct file *filp, HI_U32 cmd, unsigned long arg)
{
    HI_S32 s32Ret;

    s32Ret = down_interruptible(&g_TunerMutex);

    if (HI_TUNER_IOC_MAGIC != _IOC_TYPE(cmd))
    {
        up(&g_TunerMutex);
        return -ENOTTY;
    }

    s32Ret = HI_DRV_UserCopy(filp->f_dentry->d_inode, filp, cmd, arg, tuner_ioctl);
    up(&g_TunerMutex);
    return s32Ret;
}

/*static*/ HI_VOID hi3796c_enable_adc(HI_VOID)
{
	unsigned i;
	U_PERI_QAM_ADC0 unTmpPeriQamADC0;
	U_PERI_QAM_ADC1 unTmpPeriQamADC1;

	unTmpPeriQamADC0.u32 = g_pstRegPeri->PERI_QAM_ADC0.u32;
	unTmpPeriQamADC1.u32 = g_pstRegPeri->PERI_QAM_ADC1.u32;

	unTmpPeriQamADC0.bits.selof = 0;
	unTmpPeriQamADC0.bits.opm = 0;

	g_pstRegPeri->PERI_QAM_ADC0.u32 = unTmpPeriQamADC0.u32;
	msleep(1);

	unTmpPeriQamADC1.bits.i2c_resetz = 0;
	g_pstRegPeri->PERI_QAM_ADC1.u32 = unTmpPeriQamADC1.u32;
	msleep(1);

	unTmpPeriQamADC1.bits.i2c_resetz = 1;
	g_pstRegPeri->PERI_QAM_ADC1.u32 = unTmpPeriQamADC1.u32;
	msleep(1);

	unTmpPeriQamADC0.bits.opm = 3;
	g_pstRegPeri->PERI_QAM_ADC0.u32 = unTmpPeriQamADC0.u32;

	for (i = 30; !g_pstRegPeri->PERI_QAM_ADC0.bits.adcrdy; i--)
	{
		if (i == 0)
		{
			HI_ERR_TUNER("ADC is not ready\n"); //2659
			break;
		}

		msleep(1);
	}

	unTmpPeriQamADC0.bits.selof = 0;
	unTmpPeriQamADC0.bits.bctrl = 9;
	g_pstRegPeri->PERI_QAM_ADC0.u32 = unTmpPeriQamADC0.u32;

	unTmpPeriQamADC1.bits.enavcmin = 1;
	g_pstRegPeri->PERI_QAM_ADC1.u32 = unTmpPeriQamADC1.u32;
}

/*static*/ HI_S32 hi3796c_enable_crg(HI_VOID)
{
    U_PERI_CRG57 unTmpPeriCrg57;
    U_PERI_CRG58 unTmpPeriCrg58;
    U_PERI_QAM    unTmpPeriQam;
    U_PERI_0x864 unTmpPeri_0x864;

    unTmpPeri_0x864.u32 = g_pstRegPeri->PERI_0x864.u32;

    unTmpPeri_0x864.bits.bit2 = 0;
    unTmpPeri_0x864.bits.bit0 = 1;

    g_pstRegPeri->PERI_0x864.u32 = unTmpPeri_0x864.u32;

    unTmpPeriCrg57.u32 = g_pstRegCrg->PERI_CRG57.u32;
    unTmpPeriCrg58.u32 = g_pstRegCrg->PERI_CRG58.u32;
    unTmpPeriQam.u32 = g_pstRegPeri->PERI_QAM.u32;

    {

    /*select QAM TSCLK source 150MHz,PERI_CRG57[19:18]:01 ; QAM TSCLK 16 div,PERI_CRG57[23:20] :0111,*/
   unTmpPeriCrg57.bits.qam_ts_clk_sel =  0x01;
   unTmpPeriCrg57.bits.qam_ts_clk_div =0x7;
    /*TS_CLK_OUT config  the opposite phase,PERI_CRG57[16]:1;*/
       /*ads_clk config the same phase,PERI_CRG57[17]:0*/
   unTmpPeriCrg57.bits.qam_tsout_pctrl = 0x1;
   unTmpPeriCrg57.bits.qam_adsclk_pctrl = 0;
    /*config qam_*_cken open, PERI_CRG57[6:0]:1111111*/
   unTmpPeriCrg57.bits.qam_2x_cken = 1;
   unTmpPeriCrg57.bits.qam_1x_cken = 1;

   unTmpPeriCrg57.bits.qam_j83b_cken = 1;
   unTmpPeriCrg57.bits.qam_j83ac_cken = 1;
   unTmpPeriCrg57.bits.qam_ads_cken = 1;

   unTmpPeriCrg57.bits.qam_ts_cken = 1;
   unTmpPeriCrg57.bits.qam_tsout_cken = 1;

   /*don't reset qam_ads_srst_req and qam_ts_srst_req, PERI_CRG57[13:12]:00*/
   unTmpPeriCrg57.bits.qam_ads_srst_req = 0;
   unTmpPeriCrg57.bits.qam_ts_srst_req = 0;
      /*don't reset qam_2x_srst_req and qam_1x_srst_req, PERI_CRG57[9:8]:00*/
   unTmpPeriCrg57.bits.qam_2x_srst_req = 0;
   unTmpPeriCrg57.bits.qam_1x_srst_req = 0;
   unTmpPeriCrg57.bits.qam2x_clk_sel = 0;

#if 0
   /*config adc clk 28.8MHz*/
    /*config qamadc_clk_sel 28.8MHz,  PERI_CRG58[9:8]:00*/
   unTmpPeriCrg58.bits.qamadc_clk_sel = 0x00;

    /*config qamctrl_clk_sel 28.8MHz , PERI_CRG58[11:10]:00; \
               config qamctrl_clk_div for the same frequence, PERI_CRG58[12]:0*/
   unTmpPeriCrg58.bits.qamctrl_clk_sel = 0x00;
   unTmpPeriCrg58.bits.qamctrl_clk_div = 0;
#else
   unTmpPeriCrg58.bits.bit14 = 0x00;
   unTmpPeriCrg58.bits.bit7 = 0x00;
   unTmpPeriCrg58.bits.bit10 = 0x00;
   unTmpPeriCrg58.bits.bit13 = 0x00;
#endif

   unTmpPeriQam.bits.qam_i2c_devaddr =0x50;
  }
   g_pstRegCrg->PERI_CRG57.u32 = unTmpPeriCrg57.u32;
   g_pstRegCrg->PERI_CRG58.u32 = unTmpPeriCrg58.u32;
   g_pstRegPeri->PERI_QAM.u32 = unTmpPeriQam.u32;

   return HI_SUCCESS;
}

HI_VOID hi3796c_disable_adc(HI_VOID)
{
	U_PERI_QAM_ADC0 unTmpPeriQamADC0;

    unTmpPeriQamADC0.u32 = g_pstRegPeri->PERI_QAM_ADC0.u32;
    {
    	unTmpPeriQamADC0.bits.opm = 0;
    }
    g_pstRegPeri->PERI_QAM_ADC0.u32 = unTmpPeriQamADC0.u32;
}

HI_S32 hi3796c_disable_crg(HI_VOID)
{
    U_PERI_CRG57 unTmpPeriCrg57;

   if (g_u32CrgEnableFlag )
   {
	   g_u32CrgEnableFlag = 0;

	   unTmpPeriCrg57.u32 = g_pstRegCrg->PERI_CRG57.u32;

	   unTmpPeriCrg57.bits.qam_2x_cken = 0;
	   unTmpPeriCrg57.bits.qam_1x_cken = 0;
	   unTmpPeriCrg57.bits.qam_j83b_cken = 0;
	   unTmpPeriCrg57.bits.qam_j83ac_cken = 0;
	   unTmpPeriCrg57.bits.qam_ads_cken = 0;
	   unTmpPeriCrg57.bits.qam_ts_cken = 0;
	   unTmpPeriCrg57.bits.qam_tsout_cken = 0;

	   g_pstRegCrg->PERI_CRG57.u32 = unTmpPeriCrg57.u32;
   }

   return HI_SUCCESS;
}

HI_S32 tuner_geti2cFunc(HI_VOID)
{
	HI_S32 s32Ret = HI_SUCCESS;

	s_pI2cFunc = HI_NULL;
    s32Ret = HI_DRV_MODULE_GetFunction(HI_ID_I2C, (HI_VOID * *)&s_pI2cFunc);
    if ((HI_SUCCESS != s32Ret) || !s_pI2cFunc ||
    		!s_pI2cFunc->pfnI2cWrite || !s_pI2cFunc->pfnI2cRead ||
    		!s_pI2cFunc->pfnI2cReadDirectly || !s_pI2cFunc->pfnI2cWriteNostop)
    {
    	HI_FATAL_TUNER("I2C not found\n"); //2950
        return HI_FAILURE;
    }

    s_pGpioFunc = HI_NULL;
    s32Ret = HI_DRV_MODULE_GetFunction(HI_ID_GPIO, (HI_VOID**)&s_pGpioFunc);
    if ((HI_SUCCESS != s32Ret) || !s_pGpioFunc ||
    		!s_pGpioFunc->pfnGpioDirSetBit || !s_pGpioFunc->pfnGpioWriteBit)
    {
    	HI_FATAL_TUNER("GPIO not found\n"); //2959
        return HI_FAILURE;
    }

    s_pGpioI2cFunc = HI_NULL;
    s32Ret = HI_DRV_MODULE_GetFunction(HI_ID_GPIO_I2C, (HI_VOID**)&s_pGpioI2cFunc);
    if ((HI_SUCCESS != s32Ret) || !s_pGpioI2cFunc ||
    		!s_pGpioI2cFunc->pfnGpioI2cWriteExt || !s_pGpioI2cFunc->pfnGpioI2cReadExt ||
    		!s_pGpioI2cFunc->pfnGpioI2cReadExtDiRectly)
    {
        HI_FATAL_TUNER("GPIO_I2C not found\n"); //2968
        return HI_FAILURE;
    }

	return HI_SUCCESS;
}

static HI_VOID tuner_enable_crg(HI_VOID)
{
#if defined (CHIP_TYPE_hi3716cv200es) || defined (CHIP_TYPE_hi3716cv200)
     U_PERI_CRG57 unTmpPeriCrg57;
     U_PERI_CRG58 unTmpPeriCrg58;
     U_PERI_QAM    unTmpPeriQam;
#endif
    g_u32CrgEnableFlag= 1;
   
#if defined (CHIP_TYPE_hi3716cv200es)

     unTmpPeriCrg57.u32 = g_pstRegCrg->PERI_CRG57.u32;
     unTmpPeriCrg58.u32 = g_pstRegCrg->PERI_CRG58.u32;
     unTmpPeriQam.u32 = g_pstRegPeri->PERI_QAM.u32;
    {

    /*select QAM TSCLK source 150MHz,PERI_CRG57[19:18]:01 ; QAM TSCLK 16 div,PERI_CRG57[23:20] :0111,*/
    unTmpPeriCrg57.bits.qam_ts_clk_sel =  0x01;
    unTmpPeriCrg57.bits.qam_ts_clk_div =0x7;
     /*TS_CLK_OUT config  the opposite phase,PERI_CRG57[16]:1;*/
        /*ads_clk config the same phase,PERI_CRG57[17]:0*/
    unTmpPeriCrg57.bits.qam_tsout_pctrl = 0x1;

    unTmpPeriCrg57.bits.qam_adsclk_pctrl = 0;
     /*config qam_*_cken open, PERI_CRG57[6:0]:1111111*/
    unTmpPeriCrg57.bits.qam_2x_cken = 1;
    unTmpPeriCrg57.bits.qam_1x_cken = 1;

    unTmpPeriCrg57.bits.qam_j83b_cken = 1;
    unTmpPeriCrg57.bits.qam_j83ac_cken = 1;
    unTmpPeriCrg57.bits.qam_ads_cken = 1;

    unTmpPeriCrg57.bits.qam_ts_cken = 1;
    unTmpPeriCrg57.bits.qam_tsout_cken = 1;
       /*don't reset qam_ads_srst_req and qam_ts_srst_req, PERI_CRG57[13:12]:00*/
    unTmpPeriCrg57.bits.qam_ads_srst_req = 0;
    unTmpPeriCrg57.bits.qam_ts_srst_req = 0;
        /*don't reset qam_2x_srst_req and qam_1x_srst_req, PERI_CRG57[9:8]:00*/
    unTmpPeriCrg57.bits.qam_2x_srst_req = 0;
    unTmpPeriCrg57.bits.qam_1x_srst_req = 0;

    /*config adc clk 28.8MHz*/
    /*config qamadc_clk_sel 28.8MHz,  PERI_CRG58[9:8]:10*/

    unTmpPeriCrg58.bits.qamadc_clk_sel = 0x2;
        /*config qamctrl_clk_sel 28.8MHz , PERI_CRG58[11:10]:10; \
                config qamctrl_clk_div for the same frequence, PERI_CRG58[12]:0*/
    unTmpPeriCrg58.bits.qamctrl_clk_sel = 0x2;
    unTmpPeriCrg58.bits.qamctrl_clk_div = 0;

     unTmpPeriQam.bits.qam_i2c_devaddr =0x50;

    }
    g_pstRegCrg->PERI_CRG57.u32 = unTmpPeriCrg57.u32;
    g_pstRegCrg->PERI_CRG58.u32 = unTmpPeriCrg58.u32;
    g_pstRegPeri->PERI_QAM.u32 = unTmpPeriQam.u32;

#elif defined(CHIP_TYPE_hi3716cv200)

     unTmpPeriCrg57.u32 = g_pstRegCrg->PERI_CRG57.u32;
     unTmpPeriCrg58.u32 = g_pstRegCrg->PERI_CRG58.u32;
     unTmpPeriQam.u32 = g_pstRegPeri->PERI_QAM.u32;

     {

     /*select QAM TSCLK source 150MHz,PERI_CRG57[19:18]:01 ; QAM TSCLK 16 div,PERI_CRG57[23:20] :0111,*/
    unTmpPeriCrg57.bits.qam_ts_clk_sel =  0x01;
    unTmpPeriCrg57.bits.qam_ts_clk_div =0x7;
     /*TS_CLK_OUT config  the opposite phase,PERI_CRG57[16]:1;*/
        /*ads_clk config the same phase,PERI_CRG57[17]:0*/
    unTmpPeriCrg57.bits.qam_tsout_pctrl = 0x1;
    unTmpPeriCrg57.bits.qam_adsclk_pctrl = 0;
     /*config qam_*_cken open, PERI_CRG57[6:0]:1111111*/
    unTmpPeriCrg57.bits.qam_2x_cken = 1;
    unTmpPeriCrg57.bits.qam_1x_cken = 1;

    unTmpPeriCrg57.bits.qam_j83b_cken = 1;
    unTmpPeriCrg57.bits.qam_j83ac_cken = 1;
    unTmpPeriCrg57.bits.qam_ads_cken = 1;

    unTmpPeriCrg57.bits.qam_ts_cken = 1;
    unTmpPeriCrg57.bits.qam_tsout_cken = 1;

    /*don't reset qam_ads_srst_req and qam_ts_srst_req, PERI_CRG57[13:12]:00*/
    unTmpPeriCrg57.bits.qam_ads_srst_req = 0;
    unTmpPeriCrg57.bits.qam_ts_srst_req = 0;
       /*don't reset qam_2x_srst_req and qam_1x_srst_req, PERI_CRG57[9:8]:00*/
    unTmpPeriCrg57.bits.qam_2x_srst_req = 0;
    unTmpPeriCrg57.bits.qam_1x_srst_req = 0;

    /*config adc clk 28.8MHz*/
     /*config qamadc_clk_sel 28.8MHz,  PERI_CRG58[9:8]:00*/
    unTmpPeriCrg58.bits.qamadc_clk_sel = 0x00;

     /*config qamctrl_clk_sel 28.8MHz , PERI_CRG58[11:10]:00; \
                config qamctrl_clk_div for the same frequence, PERI_CRG58[12]:0*/
    unTmpPeriCrg58.bits.qamctrl_clk_sel = 0x00;
    unTmpPeriCrg58.bits.qamctrl_clk_div = 0;

    unTmpPeriQam.bits.qam_i2c_devaddr =0x50;
   }
    g_pstRegCrg->PERI_CRG57.u32 = unTmpPeriCrg57.u32;
    g_pstRegCrg->PERI_CRG58.u32 = unTmpPeriCrg58.u32;
    g_pstRegPeri->PERI_QAM.u32 = unTmpPeriQam.u32;

#elif defined (CHIP_TYPE_hi3798cv100)

    hi3796c_enable_crg();
    hi3796c_enable_adc();

  #endif
}

static HI_VOID tuner_disable_crg(HI_VOID)
{
#if defined (CHIP_TYPE_hi3716cv200es) || defined (CHIP_TYPE_hi3716cv200)
   U_PERI_CRG57 unTmpPeriCrg57;
#endif
#if defined (CHIP_TYPE_hi3798cv100)
	U_PERI_QAM_ADC0 unTmpPeriQamADC0;
#endif
   if (g_u32CrgEnableFlag )
   {
	   g_u32CrgEnableFlag = 0;
   }
#if defined (CHIP_TYPE_hi3716cv200es)||defined (CHIP_TYPE_hi3716cv200)

    unTmpPeriCrg57.u32 = g_pstRegCrg->PERI_CRG57.u32;
    {
     /*config qam_*_cken open, PERI_CRG57[6:0]:0000000*/
    unTmpPeriCrg57.bits.qam_2x_cken = 0;
    unTmpPeriCrg57.bits.qam_1x_cken = 0;

    unTmpPeriCrg57.bits.qam_j83b_cken = 0;
    unTmpPeriCrg57.bits.qam_j83ac_cken = 0;
    unTmpPeriCrg57.bits.qam_ads_cken = 0;

    unTmpPeriCrg57.bits.qam_ts_cken = 0;
    unTmpPeriCrg57.bits.qam_tsout_cken = 0;

   }
    g_pstRegCrg->PERI_CRG57.u32 = unTmpPeriCrg57.u32;

#elif defined (CHIP_TYPE_hi3798cv100)

    unTmpPeriQamADC0.u32 = g_pstRegPeri->PERI_QAM_ADC0.u32;
    {
    	unTmpPeriQamADC0.bits.opm = 0;
    }
    g_pstRegPeri->PERI_QAM_ADC0.u32 = unTmpPeriQamADC0.u32;

#endif
}

HI_S32 tuner_suspend (PM_BASEDEV_S *pdev, pm_message_t state)
{
    tuner_disable_crg();
    printk("TUNER suspend OK\n");
    return HI_SUCCESS;
}

HI_S32 tuner_resume(PM_BASEDEV_S *pdev)
{
    HI_S32 i;
    //TUNER_DATA_S stPara;
    //HI_U32 u32TunerPort = 0;
    //HI_U32 u32RegVal =0;

#if defined (CHIP_TYPE_hi3798cv100)
    g_u32CrgEnableFlag= 1;

    hi3796c_enable_crg();
    hi3796c_enable_adc();
#else
    tuner_enable_crg();
#endif

    if (tuner_geti2cFunc())
    {
    	HI_INFO_TUNER("tuner_geti2cFunc err\n"); //2998
    	return HI_FAILURE;
    }

    for (i = 0; i < TUNER_NUM; i++)
    {
        switch (g_stTunerOps[i].enDemodDevType)
        {
        case HI_UNF_DEMOD_DEV_TYPE_NONE:
        case HI_UNF_DEMOD_DEV_TYPE_BUTT:
        default:
            break;

        case HI_UNF_DEMOD_DEV_TYPE_3130I:
        case HI_UNF_DEMOD_DEV_TYPE_3130E:
        case HI_UNF_DEMOD_DEV_TYPE_J83B:
            if( g_stTunerOps[i].tuner_resume )
            {
                g_stTunerOps[i].tuner_resume (i);
            }
            break;

#if 0
        case HI_UNF_DEMOD_DEV_TYPE_AVL6211:
            break;

        case HI_UNF_DEMOD_DEV_TYPE_MXL101:
            break;
#endif

        case HI_UNF_DEMOD_DEV_TYPE_MN88472:
        case HI_UNF_DEMOD_DEV_TYPE_3136:
            tuner_chip_reset(s_u32ResetGpioNo[i]);
         if (g_stTunerOps[i].tuner_init)
            {
                g_stTunerOps[i].tuner_init(i, g_stTunerOps[i].enI2cChannel, g_stTunerOps[i].enTunerDevType);
            }

            if (g_stTunerOps[i].tuner_set_sat_attr)
            {
                g_stTunerOps[i].tuner_set_sat_attr(i, &(s_stSatTunerResumeInfo[i].stSatTunerAttr));
            }

            if (g_stTunerOps[i].tuner_lnbctrl_dev_init)
            {
                g_stTunerOps[i].tuner_lnbctrl_dev_init(i, g_stTunerOps[i].enI2cChannel,
                                                                  s_stSatTunerResumeInfo[i].stSatTunerAttr.u16LNBDevAddress, g_stTunerOps[i].enDemodDevType);
            }

            if (g_stTunerOps[i].tuner_set_lnb_out)
            {
                g_stTunerOps[i].tuner_set_lnb_out(i, s_stSatTunerResumeInfo[i].enLNBOut);
            }

            if (g_stTunerOps[i].tuner_send_continuous_22K)
            {
                g_stTunerOps[i].tuner_send_continuous_22K(i, s_stSatTunerResumeInfo[i].u32Continuous22K);
            }

            /*if (g_stTunerOps[i].tuner_init)
                g_stTunerOps[i].tuner_init(i,
                                         g_stTunerOps[i].enI2cChannel,
                                         g_stTunerOps[i].enTunerDevType,
                                         &(s_stTunerResumeInfo[i].unResumeData));*/
            break;
        case HI_UNF_DEMOD_DEV_TYPE_3136I:
            if (g_stTunerOps[i].tuner_init)
            {
                g_stTunerOps[i].tuner_init(i, g_stTunerOps[i].enI2cChannel, g_stTunerOps[i].enTunerDevType);
            }

            if (g_stTunerOps[i].tuner_set_sat_attr)
            {
                g_stTunerOps[i].tuner_set_sat_attr(i, &(s_stSatTunerResumeInfo[i].stSatTunerAttr));
            }

            if (g_stTunerOps[i].tuner_lnbctrl_dev_init)
            {
                g_stTunerOps[i].tuner_lnbctrl_dev_init(i, g_stTunerOps[i].enI2cChannel,
                                                                  s_stSatTunerResumeInfo[i].stSatTunerAttr.u16LNBDevAddress, g_stTunerOps[i].enDemodDevType);
            }

            if (g_stTunerOps[i].tuner_set_lnb_out)
            {
                g_stTunerOps[i].tuner_set_lnb_out(i, s_stSatTunerResumeInfo[i].enLNBOut);
            }

            if (g_stTunerOps[i].tuner_send_continuous_22K)
            {
                g_stTunerOps[i].tuner_send_continuous_22K(i, s_stSatTunerResumeInfo[i].u32Continuous22K);
            }

            /*if (g_stTunerOps[i].tuner_init)
                g_stTunerOps[i].tuner_init(i,
                                         g_stTunerOps[i].enI2cChannel,
                                         g_stTunerOps[i].enTunerDevType,
                                         &(s_stTunerResumeInfo[i].unResumeData));*/
            break;
        }
    }

    for (i = 0; i < TUNER_NUM; i++)
    {
        if ((HI_UNF_TUNER_SIGNAL_LOCKED == s_enTunerStat[i]) && (g_stTunerOps[i].tuner_connect))
        {
            g_stTunerOps[i].tuner_connect(i, &g_stTunerOps[i].stCurrPara);
            HI_INFO_TUNER(" tuner port [%d] connected! \n", i);
        }
    }

    printk("TUNER resume OK\n");
    return HI_SUCCESS;
}

HI_S32 tuner_proc_read_reg(struct seq_file * p, HI_VOID * v)
{
    HI_U32 u32TunerPort;

    PROC_PRINT(p,"---------Hisilicon TUNER Info---------\n");

    for ( u32TunerPort = 0; u32TunerPort < TUNER_NUM; u32TunerPort++ )
    {
        if ( HI_NULL == g_stTunerOps[u32TunerPort].tuner_connect )
        {
            continue;
        }

        if (g_stTunerOps[u32TunerPort].tuner_get_registers)
        {
            g_stTunerOps[u32TunerPort].tuner_get_registers(u32TunerPort, p);
        }
    }
    return HI_SUCCESS;
}

HI_S32 tuner_proc_read(struct seq_file *p, HI_VOID *v)
{

    HI_U32 u32TunerPort;
    HI_U32 au32SignalStrength[3] = {0};
    HI_U32 au32Tmp[3] = {0};
    HI_U32 u32SNR = 0;
    HI_U32 u32Ber = 0;
    HI_U64 u64TmpBer = 0;
    HI_U32 au32Rs[3] = {0};
    HI_CHAR acQamType[8];
    HI_CHAR acTunerType[20];
    HI_CHAR acDemodType[20];
    HI_CHAR acTunerLock[10];
    HI_CHAR acFECType[10];
    HI_CHAR acFECRate[10];
    HI_CHAR acFFTMode[10];
    HI_CHAR acGI[10];
    HI_CHAR acHierAlpha[10];
    HI_CHAR acTSPrio[10];
    HI_UNF_TUNER_LOCK_STATUS_E enTunerStatus = HI_UNF_TUNER_SIGNAL_DROPPED;
    HI_UNF_TUNER_SIGNALINFO_S stSignalInfo = {0};

    PROC_PRINT(p,"---------Hisilicon TUNER Info---------\n");
    PROC_PRINT(p,
            "Port: LockStat    I2CChannel    Frequency(KHz)    SymbRate    QamMode    DemodType     TunerType\n");

#warning tuner_proc_read: TODO

    for ( u32TunerPort = 0; u32TunerPort < TUNER_NUM; u32TunerPort++ )
    {
        if ( HI_NULL == g_stTunerOps[u32TunerPort].tuner_connect )
        {
            continue;
        }

        /* init proc variable */
        enTunerStatus = HI_UNF_TUNER_SIGNAL_DROPPED;
        strncpy(acTunerLock, "unlocked", sizeof( acTunerLock ) );
        acTunerLock[sizeof( acTunerLock ) - 1] = '\0';

        strncpy(acQamType, "QAM_INVALID ", sizeof( acQamType ) );
        acQamType[sizeof( acQamType ) - 1] = '\0';

        u32SNR = 0;
        u32Ber = 0;
        au32SignalStrength[2] = 0;
        au32SignalStrength[1] = 0;
        au32SignalStrength[0] = 0;

        strncpy(acTunerType, "unknown TUNER", sizeof( acTunerType ) );
        acTunerType[sizeof( acTunerType ) - 1] = '\0';
        strncpy(acDemodType, "unknown demod", sizeof( acDemodType ) );
        acDemodType[sizeof( acDemodType ) - 1] = '\0';


        /* get proc info */
        if (g_stTunerOps[u32TunerPort].tuner_get_status)
        g_stTunerOps[u32TunerPort].tuner_get_status(u32TunerPort, &enTunerStatus);

        if ( HI_UNF_TUNER_SIGNAL_LOCKED == enTunerStatus )
        {
            strncpy(acTunerLock, "locked", sizeof( acTunerLock ) );
            acTunerLock[sizeof( acTunerLock ) - 1] = '\0';
            if (g_stTunerOps[u32TunerPort].tuner_get_ber)
            {
                g_stTunerOps[u32TunerPort].tuner_get_ber(u32TunerPort, au32Tmp);
            }
            if (g_stTunerOps[u32TunerPort].tuner_get_snr)
            {
                g_stTunerOps[u32TunerPort].tuner_get_snr(u32TunerPort, &u32SNR);
            }
            if (g_stTunerOps[u32TunerPort].tuner_get_signal_strength)
            {
                g_stTunerOps[u32TunerPort].tuner_get_signal_strength(u32TunerPort, au32SignalStrength);
            }
            if (g_stTunerOps[u32TunerPort].tuner_get_signal_info)
            {
                g_stTunerOps[u32TunerPort].tuner_get_signal_info(u32TunerPort, &stSignalInfo);
            }

            if ((HI_UNF_DEMOD_DEV_TYPE_3130I == g_stTunerOps[u32TunerPort].enDemodDevType)
                || (HI_UNF_DEMOD_DEV_TYPE_3130E == g_stTunerOps[u32TunerPort].enDemodDevType)
                || (HI_UNF_DEMOD_DEV_TYPE_J83B == g_stTunerOps[u32TunerPort].enDemodDevType))
            {
                u32Ber = (au32Tmp[0] << 16 | au32Tmp[1] << 8 | au32Tmp[2]);
                u64TmpBer = (HI_U64)u32Ber;
                u32Ber = (HI_U32)((u64TmpBer / 8) * 10000000 / (1 << 20));
                au32Tmp[0] = u32Ber;
                au32Tmp[1] = 0;
                au32Tmp[2] = 7;
            }
        }

        /* Modulate type */
        switch (g_stTunerOps[u32TunerPort].enDemodDevType)
        {
            case HI_UNF_DEMOD_DEV_TYPE_NONE:
            case HI_UNF_DEMOD_DEV_TYPE_BUTT:
            default:
                break;

            /* For cable */
            case HI_UNF_DEMOD_DEV_TYPE_3130I:
            case HI_UNF_DEMOD_DEV_TYPE_3130E:
            case HI_UNF_DEMOD_DEV_TYPE_J83B:
                switch(g_stTunerOps[u32TunerPort].stCurrPara.enQamType)
                {
                    case QAM_TYPE_16:
                        strncpy(acQamType, "QAM_16 ", sizeof( acQamType ) );
                        break;
                    case QAM_TYPE_32:
                        strncpy(acQamType, "QAM_32 ", sizeof( acQamType ) );
                        break;
                    case QAM_TYPE_64:
                        strncpy(acQamType, "QAM_64 ", sizeof( acQamType ) );
                        break;
                    case QAM_TYPE_128:
                        strncpy(acQamType, "QAM_128", sizeof( acQamType ) );
                        break;
                    case QAM_TYPE_256:
                        strncpy(acQamType, "QAM_256", sizeof( acQamType ) );
                        break;
                    default :
                        strncpy(acQamType, "QAM_INVALID ", sizeof( acQamType ) );
                }
                break;

            /* For satellite */
            case HI_UNF_DEMOD_DEV_TYPE_AVL6211:
            case HI_UNF_DEMOD_DEV_TYPE_3136:
            case HI_UNF_DEMOD_DEV_TYPE_3136I:
                switch (stSignalInfo.unSignalInfo.stSat.enModType)
                {
                case HI_UNF_MOD_TYPE_QPSK:
                    strncpy(acQamType, "QPSK ", sizeof(acQamType));
                    break;
                case HI_UNF_MOD_TYPE_8PSK:
                    strncpy(acQamType, "8PSK ", sizeof(acQamType));
                    break;
                case HI_UNF_MOD_TYPE_16APSK:
                    strncpy(acQamType, "16APSK ", sizeof(acQamType));
                    break;
                case HI_UNF_MOD_TYPE_32APSK:
                    strncpy(acQamType, "32APSK", sizeof(acQamType));
                    break;
                case HI_UNF_MOD_TYPE_DEFAULT:
                case HI_UNF_MOD_TYPE_QAM_16:
                case HI_UNF_MOD_TYPE_QAM_32:
                case HI_UNF_MOD_TYPE_QAM_64:
                case HI_UNF_MOD_TYPE_QAM_128:
                case HI_UNF_MOD_TYPE_QAM_256:
                case HI_UNF_MOD_TYPE_QAM_512:
                default:
                    strncpy(acQamType, "QAM_INVALID ", sizeof(acQamType));
                }
                break;
            /*for terrestrial*/
            case HI_UNF_DEMOD_DEV_TYPE_MXL101:
                switch (stSignalInfo.unSignalInfo.stTer.enModType)
                {
                case HI_UNF_MOD_TYPE_QPSK:
                    strncpy(acQamType, "QPSK ", sizeof(acQamType));
                    break;
                case HI_UNF_MOD_TYPE_QAM_16:
                    strncpy(acQamType, "QAM_16 ", sizeof(acQamType));
                    break;
                case HI_UNF_MOD_TYPE_QAM_64:
                    strncpy(acQamType, "QAM_64 ", sizeof(acQamType));
                    break;
                default:
                    strncpy(acQamType, "QAM_INVALID ", sizeof(acQamType));
                }
                break;

            case HI_UNF_DEMOD_DEV_TYPE_MN88472:
                switch (stSignalInfo.unSignalInfo.stTer.enModType)
                {
                case HI_UNF_MOD_TYPE_QPSK:
                    strncpy(acQamType, "QPSK ", sizeof(acQamType));
                    break;
                case HI_UNF_MOD_TYPE_QAM_16:
                    strncpy(acQamType, "QAM_16 ", sizeof(acQamType));
                    break;
                case HI_UNF_MOD_TYPE_QAM_64:
                    strncpy(acQamType, "QAM_64 ", sizeof(acQamType));
                    break;
                case HI_UNF_MOD_TYPE_QAM_256:
                    strncpy(acQamType, "QAM_256 ", sizeof(acQamType));
                    break;
                default:
                    strncpy(acQamType, "QAM_INVALID ", sizeof(acQamType));
                }
                break;

            case HI_UNF_DEMOD_DEV_TYPE_IT9170:
                switch (stSignalInfo.unSignalInfo.stTer.enModType)
                {
                case HI_UNF_MOD_TYPE_QPSK:
                    strncpy(acQamType, "QPSK ", sizeof(acQamType));
                    break;
                case HI_UNF_MOD_TYPE_DQPSK:
                    strncpy(acQamType, "DQPSK ", sizeof(acQamType));
                    break;
                case HI_UNF_MOD_TYPE_QAM_16:
                    strncpy(acQamType, "QAM_16 ", sizeof(acQamType));
                    break;
                case HI_UNF_MOD_TYPE_QAM_64:
                    strncpy(acQamType, "QAM_64 ", sizeof(acQamType));
                    break;
                default:
                    strncpy(acQamType, "QAM_INVALID ", sizeof(acQamType));
                }
                break;

            case HI_UNF_DEMOD_DEV_TYPE_IT9133:
                switch (stSignalInfo.unSignalInfo.stTer.enModType)
                {
                case HI_UNF_MOD_TYPE_QPSK:
                    strncpy(acQamType, "QPSK ", sizeof(acQamType));
                    break;
                case HI_UNF_MOD_TYPE_QAM_16:
                    strncpy(acQamType, "QAM_16 ", sizeof(acQamType));
                    break;
                case HI_UNF_MOD_TYPE_QAM_64:
                    strncpy(acQamType, "QAM_64 ", sizeof(acQamType));
                    break;
                default:
                    strncpy(acQamType, "QAM_INVALID ", sizeof(acQamType));
                }
                break;
        }
        acQamType[sizeof( acQamType ) - 1] = '\0';


        switch( g_stTunerOps[u32TunerPort].enDemodDevType )
        {
            case HI_UNF_DEMOD_DEV_TYPE_3130I:
                strncpy(acDemodType, "3130_inside", sizeof( acDemodType ) );
                break;

            case HI_UNF_DEMOD_DEV_TYPE_3130E:
                strncpy(acDemodType, "3130_outside", sizeof( acDemodType ) );
                break;
            case HI_UNF_DEMOD_DEV_TYPE_J83B:
                strncpy(acDemodType, "j83b", sizeof(acDemodType));
                break;
            case HI_UNF_DEMOD_DEV_TYPE_AVL6211:
                strncpy(acDemodType, "avl6211", sizeof(acDemodType));
                break;
            case HI_UNF_DEMOD_DEV_TYPE_MXL101:
                strncpy(acDemodType, "mxl101", sizeof(acDemodType));
                break;
            case HI_UNF_DEMOD_DEV_TYPE_MN88472:
                strncpy(acDemodType, "mn88472", sizeof(acDemodType));
                break;
            case HI_UNF_DEMOD_DEV_TYPE_IT9170:
                strncpy(acDemodType, "it9170", sizeof(acDemodType));
                break;
            case HI_UNF_DEMOD_DEV_TYPE_IT9133:
                strncpy(acDemodType, "it9133", sizeof(acDemodType));
                break;
            case HI_UNF_DEMOD_DEV_TYPE_3136:
                strncpy(acDemodType, "hi3136e", sizeof(acDemodType));
                break;
            case HI_UNF_DEMOD_DEV_TYPE_3136I:
                strncpy(acDemodType, "hi3136i", sizeof(acDemodType));
                break;
            default:
                strncpy(acDemodType, "unknown demod", sizeof( acDemodType ) );
                break;
        }
        acDemodType[sizeof( acDemodType ) - 1] = '\0';



        switch(g_stTunerOps[u32TunerPort].enTunerDevType)
        {
            case HI_UNF_TUNER_DEV_TYPE_CD1616:
                strncpy(acTunerType, "CD1616", sizeof( acTunerType ) );
                break;
            case HI_UNF_TUNER_DEV_TYPE_CD1616_DOUBLE:
                strncpy(acTunerType, "CD1616_DOUBLE", sizeof( acTunerType ) );
                break;
            case HI_UNF_TUNER_DEV_TYPE_ALPS_TDAE:
                strncpy(acTunerType, "ALPS_TDAE", sizeof( acTunerType ) );
                break;
            case HI_UNF_TUNER_DEV_TYPE_TDCC:
                strncpy(acTunerType, "TDCC", sizeof( acTunerType ) );
                break;
            case HI_UNF_TUNER_DEV_TYPE_TDA18250:
                strncpy(acTunerType, "TDA18250", sizeof( acTunerType ) );
                break;
            case HI_UNF_TUNER_DEV_TYPE_TDA18250B:
                strncpy(acTunerType, "TDA18250B", sizeof( acTunerType ) );
                break;
            case HI_UNF_TUNER_DEV_TYPE_TMX7070X:
                strncpy(acTunerType, "TMX7070X", sizeof( acTunerType ) );
                break;
            case HI_UNF_TUNER_DEV_TYPE_XG_3BL:
                strncpy(acTunerType, "XG_3BL", sizeof( acTunerType ) );
                break;
            case HI_UNF_TUNER_DEV_TYPE_MT2081:
                strncpy(acTunerType, "MT2081", sizeof( acTunerType ) );
                break;
            case HI_UNF_TUNER_DEV_TYPE_R820C:
                strncpy(acTunerType, "R820C", sizeof( acTunerType ) );
                break;
            case HI_UNF_TUNER_DEV_TYPE_MXL203:
                strncpy(acTunerType, "MXL203", sizeof( acTunerType ) );
                break;
            case HI_UNF_TUNER_DEV_TYPE_AV2011:
                strncpy(acTunerType, "AV2011", sizeof(acTunerType));
                break;
            case HI_UNF_TUNER_DEV_TYPE_SHARP7903:
                strncpy(acTunerType, "SHARP7903", sizeof(acTunerType));
                break;
            case HI_UNF_TUNER_DEV_TYPE_M88TS2022:
                strncpy(acTunerType, "M88TS2022", sizeof(acTunerType));
                break;
            case HI_UNF_TUNER_DEV_TYPE_RDA5815:
                strncpy(acTunerType, "RDA5815", sizeof(acTunerType));
                break;
            case HI_UNF_TUNER_DEV_TYPE_MXL101:
                strncpy(acTunerType, "MXL101", sizeof(acTunerType));
                break;
            case HI_UNF_TUNER_DEV_TYPE_MXL603:
                strncpy(acTunerType, "MXL603", sizeof(acTunerType));
                break;
            case HI_UNF_TUNER_DEV_TYPE_IT9170:
                strncpy(acTunerType, "IT9170", sizeof(acTunerType));
                break;
            case HI_UNF_TUNER_DEV_TYPE_IT9133:
                strncpy(acTunerType, "IT9133", sizeof(acTunerType));
                break;
            case HI_UNF_TUNER_DEV_TYPE_TDA6651:
                strncpy(acTunerType, "TDA6651", sizeof(acTunerType));
                break;
            default :
                strncpy(acTunerType, "unknown TUNER", sizeof( acTunerType ) );
                break;
        }
        acTunerType[sizeof( acTunerType ) - 1] = '\0';

        PROC_PRINT(p,
                " %d:   %s     %5d        %8d   %13d  %10s    %s  %11s\n",
                u32TunerPort,
                acTunerLock,
                g_stTunerOps[u32TunerPort].enI2cChannel,
                g_stTunerOps[u32TunerPort].stCurrPara.u32Frequency,
                        g_stTunerOps[u32TunerPort].stCurrPara.unSRBW.u32SymbolRate,
                acQamType,
                acDemodType,
                acTunerType
                );

        PROC_PRINT(p, "\nBER:%d.%d*(E-%d),  SNR:%d,  SignalStrength:%d\n", au32Tmp[0], au32Tmp[1], au32Tmp[2], u32SNR, au32SignalStrength[1]);

        if (g_stTunerOps[u32TunerPort].tuner_get_rs)
        {
            g_stTunerOps[u32TunerPort].tuner_get_rs(u32TunerPort, au32Rs);
            PROC_PRINT(p, "\nall_rs_package:%-d,  corrected_rs_package:%d,  error_rs_package:%d\n\n", au32Rs[0],
                       au32Rs[1], au32Rs[2]);
        }

        /* For DVB-S/S2, print FEC type and rate */
        if (HI_UNF_TUNER_SIG_TYPE_SAT == stSignalInfo.enSigType)
        {
            switch (stSignalInfo.unSignalInfo.stSat.enSATType)
            {
            case HI_UNF_TUNER_FE_DVBS:
                strncpy(acFECType, "DVBS", sizeof(acFECType));
                break;
            case HI_UNF_TUNER_FE_DVBS2:
                strncpy(acFECType, "DVBS2", sizeof(acFECType));
                break;
            case HI_UNF_TUNER_FE_DIRECTV:
                strncpy(acFECType, "DIRECTV", sizeof(acFECType));
                break;
            case HI_UNF_TUNER_FE_BUTT:
            default:
                strncpy(acFECType, "Unknown", sizeof(acFECType));
                break;
            }

            switch (stSignalInfo.unSignalInfo.stSat.enFECRate)
            {
            case HI_UNF_TUNER_FE_FEC_1_2:
                strncpy(acFECRate, "1/2", sizeof(acFECRate));
                break;
            case HI_UNF_TUNER_FE_FEC_1_3:
                strncpy(acFECRate, "1/3", sizeof(acFECRate));
                break;
            case HI_UNF_TUNER_FE_FEC_2_3:
                strncpy(acFECRate, "2/3", sizeof(acFECRate));
                break;
            case HI_UNF_TUNER_FE_FEC_3_4:
                strncpy(acFECRate, "3/4", sizeof(acFECRate));
                break;
            case HI_UNF_TUNER_FE_FEC_4_5:
                strncpy(acFECRate, "4/5", sizeof(acFECRate));
                break;
            case HI_UNF_TUNER_FE_FEC_5_6:
                strncpy(acFECRate, "5/6", sizeof(acFECRate));
                break;
            case HI_UNF_TUNER_FE_FEC_6_7:
                strncpy(acFECRate, "6/7", sizeof(acFECRate));
                break;
            case HI_UNF_TUNER_FE_FEC_7_8:
                strncpy(acFECRate, "7/8", sizeof(acFECRate));
                break;
            case HI_UNF_TUNER_FE_FEC_8_9:
                strncpy(acFECRate, "8/9", sizeof(acFECRate));
                break;
            case HI_UNF_TUNER_FE_FEC_9_10:
                strncpy(acFECRate, "9/10", sizeof(acFECRate));
                break;
            case HI_UNF_TUNER_FE_FEC_1_4:
                strncpy(acFECRate, "1/4", sizeof(acFECRate));
                break;
            case HI_UNF_TUNER_FE_FEC_2_5:
                strncpy(acFECRate, "2/5", sizeof(acFECRate));
                break;
            case HI_UNF_TUNER_FE_FEC_3_5:
                strncpy(acFECRate, "3/5", sizeof(acFECRate));
                break;
            case HI_UNF_TUNER_FE_FECRATE_BUTT:
            case HI_UNF_TUNER_FE_FEC_AUTO:
            default:
                strncpy(acFECRate, "AUTO", sizeof(acFECRate));
                break;
            }

            PROC_PRINT(p, "\nFEC type:%s,  FEC rate:%s\n", acFECType, acFECRate);
        }

        /* For DVB-T/T2, print FEC type and rate */
        if (HI_UNF_TUNER_SIG_TYPE_DVB_T <= stSignalInfo.enSigType
            && HI_UNF_TUNER_SIG_TYPE_DTMB >= stSignalInfo.enSigType)
        {
            switch (stSignalInfo.unSignalInfo.stTer.enFECRate)
            {
            case HI_UNF_TUNER_FE_FEC_1_2:
                strncpy(acFECRate, "1/2", sizeof(acFECRate));
                break;
            case HI_UNF_TUNER_FE_FEC_2_3:
                strncpy(acFECRate, "1/2", sizeof(acFECRate));
                break;
            case HI_UNF_TUNER_FE_FEC_3_4:
                strncpy(acFECRate, "3/4", sizeof(acFECRate));
                break;
            case HI_UNF_TUNER_FE_FEC_3_5:
                strncpy(acFECRate, "3/5", sizeof(acFECRate));
                break;
            case HI_UNF_TUNER_FE_FEC_5_6:
                strncpy(acFECRate, "5/6", sizeof(acFECRate));
                break;
            case HI_UNF_TUNER_FE_FEC_7_8:
                strncpy(acFECRate, "7/8", sizeof(acFECRate));
                break;
            default:
                strncpy(acFECRate, "INVALID", sizeof(acFECRate));
                break;
            }

            switch (stSignalInfo.unSignalInfo.stTer.enFFTMode)
            {
            case HI_UNF_TUNER_FE_FFT_1K:
                strncpy(acFFTMode, "1K", sizeof(acFFTMode));
                break;
            case HI_UNF_TUNER_FE_FFT_2K:
                strncpy(acFFTMode, "2K", sizeof(acFFTMode));
                break;
            case HI_UNF_TUNER_FE_FFT_4K:
                strncpy(acFFTMode, "4K", sizeof(acFFTMode));
                break;
            case HI_UNF_TUNER_FE_FFT_8K:
                strncpy(acFFTMode, "8K", sizeof(acFFTMode));
                break;
            case HI_UNF_TUNER_FE_FFT_16K:
                strncpy(acFFTMode, "16K", sizeof(acFFTMode));
                break;
            case HI_UNF_TUNER_FE_FFT_32K:
                strncpy(acFFTMode, "32K", sizeof(acFFTMode));
                break;
            default:
                strncpy(acFFTMode, "INVALID", sizeof(acFFTMode));
                break;
            }

            switch (stSignalInfo.unSignalInfo.stTer.enGuardIntv)
            {
            case HI_UNF_TUNER_FE_GUARD_INTV_1_128:
                strncpy(acGI, "1/128", sizeof(acGI));
                break;
            case HI_UNF_TUNER_FE_GUARD_INTV_1_32:
                strncpy(acGI, "1/32", sizeof(acGI));
                break;
            case HI_UNF_TUNER_FE_GUARD_INTV_1_16:
                strncpy(acGI, "1/16", sizeof(acGI));
                break;
            case HI_UNF_TUNER_FE_GUARD_INTV_1_8:
                strncpy(acGI, "1/8", sizeof(acGI));
                break;
            case HI_UNF_TUNER_FE_GUARD_INTV_1_4:
                strncpy(acGI, "1/4", sizeof(acGI));
                break;
            case HI_UNF_TUNER_FE_GUARD_INTV_19_128:
                strncpy(acGI, "19/128", sizeof(acGI));
                break;
            case HI_UNF_TUNER_FE_GUARD_INTV_19_256:
                strncpy(acGI, "19/256", sizeof(acGI));
                break;
            default:
                strncpy(acGI, "INVALID", sizeof(acGI));
                break;
            }

            switch (stSignalInfo.unSignalInfo.stTer.enHierMod)
            {
            case HI_UNF_TUNER_FE_HIERARCHY_NO:
                strncpy(acHierAlpha, "NONE", sizeof(acHierAlpha));
                break;
            case HI_UNF_TUNER_FE_HIERARCHY_ALHPA1:
                strncpy(acHierAlpha, "alpha1", sizeof(acHierAlpha));
                break;
            case HI_UNF_TUNER_FE_HIERARCHY_ALHPA2:
                strncpy(acHierAlpha, "alpha2", sizeof(acHierAlpha));
                break;
            case HI_UNF_TUNER_FE_HIERARCHY_ALHPA4:
                strncpy(acHierAlpha, "alpha4", sizeof(acHierAlpha));
                break;
            default:
                strncpy(acHierAlpha, "INVALID", sizeof(acHierAlpha));
                break;
            }

            switch (stSignalInfo.unSignalInfo.stTer.enTsPriority)
            {
            case HI_UNF_TUNER_TS_PRIORITY_NONE:
                strncpy(acTSPrio, "NONE", sizeof(acTSPrio));
                break;
            case HI_UNF_TUNER_TS_PRIORITY_HP:
                strncpy(acTSPrio, "HP", sizeof(acTSPrio));
                break;
            case HI_UNF_TUNER_TS_PRIORITY_LP:
                strncpy(acTSPrio, "LP", sizeof(acTSPrio));
                break;
            default:
                strncpy(acTSPrio, "INVALID", sizeof(acTSPrio));
                break;
            }

            PROC_PRINT(p, "\nFEC rate:%s, FFT Mode:%s, GI: %s, HierMod: %s, TSPrio: %s\n", acFECRate, acFFTMode, acGI, acHierAlpha, acTSPrio);
        }
    }

    return HI_SUCCESS;
}

#if defined (CHIP_TYPE_hi3716cv200es) || defined (CHIP_TYPE_hi3716cv200)
/*RAM_SIZE * 32BIT*/
#define  RAM_SIZE  8192

HI_VOID  PsisiPrintBuf(HI_U8* pu8Buf, HI_U32 u32Len)
{
    struct file *file_filp = NULL;
    mm_segment_t old_fs;

    if(file_filp== NULL)
    {
        file_filp = filp_open("collect_data.bin", O_RDWR | O_CREAT, 0644);
    }
    if((file_filp== NULL) ||(IS_ERR(file_filp)))
    {
        HI_ERR_TUNER("error occured while opening file collect_data.bin, exiting...\n");
        return;
    }

    old_fs = get_fs();
    set_fs(KERNEL_DS);
    file_filp->f_op->write(file_filp, (char *)pu8Buf, u32Len, &file_filp->f_pos);
    set_fs(old_fs);
    HI_INFO_TUNER("file is closing !\n");
   // if(file_filp != NULL)
    //{
        filp_close(file_filp, NULL);
        file_filp= NULL;
    //}
     HI_INFO_TUNER("file end !\n");
     return;
}

HI_S32 qam_ram_collect_data(struct file * file, const char __user * buf, size_t count, loff_t *ppos)
{
    HI_U8 u8RegVal = 0;
    HI_U32 u32DataType = 0;
    HI_U32 u32Flag = 0;
    HI_PCHAR p   = NULL;
    HI_PCHAR org = NULL;
    HI_PCHAR pcDataBuf = NULL;
    HI_S32 s32Ret = HI_SUCCESS;
    p = (char *)kmalloc(sizeof(char)*64,GFP_KERNEL);
    if (copy_from_user(p, buf, count))
    {
        HI_ERR_TUNER("copy from user failed\n");
        return HI_FAILURE;
    }

    pcDataBuf = (HI_PCHAR)kmalloc(sizeof(char)*RAM_SIZE*4,GFP_KERNEL);

    org = p;
    u32Flag = (HI_U32)simple_strtoul(p, &p, 10);
    u32DataType = (HI_U32)simple_strtoul(p+1, &p, 10);

    HI_INFO_TUNER("count = %d\n,u32Flag = %d\n, u32DataType = %d\n",count, u32Flag, u32DataType);

    /*collect data type*/
    if((4 == count || 3 == count))
    {
      //disable auto_reset
      qam_write_bit(s_u32InsideQamPort, MCTRL_6_ADDR, 6, 0); /*disable*/
        switch(u32DataType)
        {
            case(0):
            {
                HI_INFO_TUNER("TR loop filter output data\n");
                qam_read_byte(s_u32InsideQamPort, DATA_COLLECT_1_ADDR, &u8RegVal);
                u8RegVal = u8RegVal & (~(0x7 << 4));
                qam_write_byte(s_u32InsideQamPort, DATA_COLLECT_1_ADDR, u8RegVal);
                break;
            }
            case(1):
            {
                HI_INFO_TUNER("CR loop filter output data\n");
                qam_read_byte(s_u32InsideQamPort, DATA_COLLECT_1_ADDR, &u8RegVal);
                u8RegVal = (u8RegVal & (~(0x7 << 4))) | (0x1 << 4);
                qam_write_byte(s_u32InsideQamPort, DATA_COLLECT_1_ADDR, u8RegVal);
                break;
            }
            case(2):
            {
                HI_INFO_TUNER("CR status data\n");
                qam_read_byte(s_u32InsideQamPort, DATA_COLLECT_1_ADDR, &u8RegVal);
                u8RegVal = (u8RegVal & (~(0x7 << 4))) | (0x2 << 4);
                qam_write_byte(s_u32InsideQamPort, DATA_COLLECT_1_ADDR, u8RegVal);
                break;
            }
            case(3):
            {
                HI_INFO_TUNER("CR PED data\n");
                qam_read_byte(s_u32InsideQamPort, DATA_COLLECT_1_ADDR, &u8RegVal);
                u8RegVal = (u8RegVal & (~(0x7 << 4))) | (0x3 << 4);
                qam_write_byte(s_u32InsideQamPort, DATA_COLLECT_1_ADDR, u8RegVal);
                break;
            }
            case(4):
            {
                HI_INFO_TUNER("before hard judge,after filter phase\n");
                qam_read_byte(s_u32InsideQamPort, DATA_COLLECT_1_ADDR, &u8RegVal);
                u8RegVal = (u8RegVal & (~(0x7 << 4))) | (0x4 << 4);
                qam_write_byte(s_u32InsideQamPort, DATA_COLLECT_1_ADDR, u8RegVal);
                break;
            }
            case(5):
            {
                HI_INFO_TUNER("DFE and FFE data\n");
                qam_read_byte(s_u32InsideQamPort, DATA_COLLECT_1_ADDR, &u8RegVal);
                u8RegVal = (u8RegVal & (~(0x7 << 4))) | (0x5 << 4);
                qam_write_byte(s_u32InsideQamPort, DATA_COLLECT_1_ADDR, u8RegVal);
                break;
            }
            case(6):
            {
                HI_INFO_TUNER("MF data\n");
                qam_read_byte(s_u32InsideQamPort, DATA_COLLECT_1_ADDR, &u8RegVal);
                u8RegVal = (u8RegVal & (~(0x7 << 4))) | (0x6 << 4);
                qam_write_byte(s_u32InsideQamPort, DATA_COLLECT_1_ADDR, u8RegVal);
                break;
            }
            case(7):
            {
                qam_read_byte(s_u32InsideQamPort, DATA_COLLECT_1_ADDR, &u8RegVal);
                u8RegVal = (u8RegVal & (~(0x7 << 4))) | (0x7 << 4);
                qam_write_byte(s_u32InsideQamPort, DATA_COLLECT_1_ADDR, u8RegVal);
                break;
            }
            default:
            {
                HI_INFO_TUNER("collect data type error\n");
                break;
            }
        }

        /*write enable*/
        qam_read_byte(s_u32InsideQamPort, DATA_COLLECT_0_ADDR, &u8RegVal);
        /*select RAM collect data*/
        if(1 == u32Flag)
        {
            HI_INFO_TUNER("u32Flag = %d\n", u32Flag);
            u8RegVal = u8RegVal | (1 << 5) ;
        }
        else if(0 == u32Flag)
        {
            u8RegVal = u8RegVal & ( ~(1 << 5)) ;
        }
        else
        {
            HI_INFO_TUNER("select reg2ram_j83b_sel error\n");
        }
        /*disable ram read*/
        u8RegVal = u8RegVal & (~(1 << 6));
        /*enable ram write*/
        u8RegVal = u8RegVal & (~(1 << 7));
        qam_write_byte(s_u32InsideQamPort, DATA_COLLECT_0_ADDR, u8RegVal);
        qam_read_byte(s_u32InsideQamPort, DATA_COLLECT_0_ADDR, &u8RegVal);
        u8RegVal = u8RegVal | (1 << 7);
        qam_write_byte(s_u32InsideQamPort, DATA_COLLECT_0_ADDR, u8RegVal);
     //enable autoreset
        qam_write_bit(s_u32InsideQamPort, MCTRL_6_ADDR, 6, 1);
    }

    /*read stauts*/
    else if(1 == count)
    {
    //disable autoreset
        qam_write_bit(s_u32InsideQamPort, MCTRL_6_ADDR, 6, 0);
        qam_read_byte(s_u32InsideQamPort, DATA_COLLECT_1_ADDR, &u8RegVal);
        HI_INFO_TUNER("*************read stauts = 0x%x\n",u8RegVal);
        if(u8RegVal & 0x1)
        {
            HI_INFO_TUNER("reading RAM\n");
            qam_read_byte(s_u32InsideQamPort, DATA_COLLECT_0_ADDR, &u8RegVal);
            u8RegVal = u8RegVal | (0x1 << 6);
            qam_write_byte(s_u32InsideQamPort, DATA_COLLECT_0_ADDR, u8RegVal);
			
           
            if(HI_STD_I2C_NUM >g_stTunerOps[s_u32InsideQamPort].enI2cChannel)
            {
                s32Ret = HI_DRV_I2C_Read(g_stTunerOps[s_u32InsideQamPort].enI2cChannel,
                     g_stTunerOps[s_u32InsideQamPort].u32DemodAddress, 0x00, 1, pcDataBuf, RAM_SIZE*4);
                if(HI_SUCCESS != s32Ret)
                {
	                 return s32Ret;
                }
            }
            else
            {
                s32Ret = HI_DRV_GPIOI2C_ReadExt(g_stTunerOps[s_u32InsideQamPort].enI2cChannel,
                     g_stTunerOps[s_u32InsideQamPort].u32DemodAddress, 0x00, 1, pcDataBuf, RAM_SIZE*4);
                if(HI_SUCCESS != s32Ret)
                {
	                 return s32Ret;
                }
            }	
            if(HI_SUCCESS != s32Ret)
            {
                HI_ERR_TUNER("HI_I2C_Read fail\n");
                kfree(org);
                kfree(pcDataBuf);
                org = NULL;
                p = NULL;
                pcDataBuf = NULL;
          //enable autoreset
                qam_write_bit(s_u32InsideQamPort, MCTRL_6_ADDR, 6, 1);
                return HI_FAILURE;
            }


            /*clean read flag and select RAM flag*/
            qam_read_byte(s_u32InsideQamPort, DATA_COLLECT_0_ADDR, &u8RegVal);
            u8RegVal = u8RegVal & (~(0x7 << 5));
            qam_write_byte(s_u32InsideQamPort, DATA_COLLECT_0_ADDR, u8RegVal);
            qam_read_byte(s_u32InsideQamPort, DATA_COLLECT_1_ADDR, &u8RegVal);
            u8RegVal = u8RegVal & (~0x1);
            qam_write_byte(s_u32InsideQamPort, DATA_COLLECT_1_ADDR, u8RegVal);

            /*save data to file*/
            PsisiPrintBuf(pcDataBuf, RAM_SIZE*4);
        }
        else
        {
            HI_WARN_TUNER("status 0\n");
        }

     //enable autoreset
        qam_write_bit(s_u32InsideQamPort, MCTRL_6_ADDR, 6, 1);
    }

    /*force to write read status*/
    else if(2 == count)
    {
        qam_read_byte(s_u32InsideQamPort, DATA_COLLECT_1_ADDR, &u8RegVal);
        u8RegVal = u8RegVal | 0x1;
        qam_write_byte(s_u32InsideQamPort, DATA_COLLECT_1_ADDR, u8RegVal);
    }
    else
    {
        HI_ERR_TUNER("parameter error\n");
    }
    kfree(org);
    kfree(pcDataBuf);
    org = NULL;
    p = NULL;
    pcDataBuf = NULL;
    return count;
}
#endif

//atomic_t Data_8115cd24 = ATOMIC_INIT(0);

HI_S32 hi_tuner_open(struct inode *inode, struct file *filp)
{
	HI_S32 s32Ret;

    s32Ret = down_interruptible(&g_TunerMutex);

    if (atomic_inc_return(&/*Data_8115cd24*/s_atomicTunerAvailble) == 1)
	{
	    g_u32CrgEnableFlag= 1;

	    hi3796c_enable_crg();
	    hi3796c_enable_adc();

	    s32Ret = tuner_geti2cFunc();
	    if (s32Ret)
	    {
	    	HI_ERR_TUNER("tuner_geti2cFunc err\n"); //4012
	    }
	}

	up(&g_TunerMutex);

	return s32Ret;

#if 0
    HI_U32  i = 0;
    for(i=0;i<TUNER_NUM*10;i++)
    {
        if(0 != s_u32TunerAvailble[i])
        {           
            break;
        }
    }
    if(i == TUNER_NUM*10)
    {
        tuner_enable_crg();  
    
    }
    
    for(i=0;i<TUNER_NUM*10;i++)
    {
        if(0 == s_u32TunerAvailble[i])
        {
            s_u32TunerAvailble[i] = (HI_U32)filp;
            break;
        }
    }
    if(i == TUNER_NUM*10)
    {
        HI_ERR_TUNER("progress NUM is  full\n");
        return HI_FAILURE;
    }
    
    return HI_SUCCESS;
#endif
}

HI_S32 hi_tuner_release(struct inode *inode, struct file *filp)
{
	HI_S32 s32Ret;

    s32Ret = down_interruptible(&g_TunerMutex);

//    if (atomic_dec_return(&Data_8115cd24) == 0)
    if (atomic_dec_and_test(&s_atomicTunerAvailble))
	{
    	tuner_disable_crg();
	}

	up(&g_TunerMutex);

#if 0
   HI_U32 i = 0;
   HI_S32    Ret = 0;
   Ret =down_interruptible(&g_TunerMutex);

   for(i=0;i<TUNER_NUM*10;i++)
   {
       if((HI_U32)filp == s_u32TunerAvailble[i])
       {
           s_u32TunerAvailble[i] = 0;
           break;
       }
   }
   
   for(i=0;i<TUNER_NUM*10;i++)
   {
       if(0 != s_u32TunerAvailble[i])
       {
          
           break;
       }
   }
   if(i ==TUNER_NUM*10)
   {
       tuner_disable_crg();
     
   }
   if (atomic_dec_and_test(&s_atomicTunerAvailble))
   {
   }

   up(&g_TunerMutex);
#endif
    
    return HI_SUCCESS;
}

HI_VOID reset_special_process_flag(HI_BOOL flag)
{
    g_bLockFlag = flag;
    return;
}

HI_S32 x5hdqam_special_process(void * arg)
{
    HI_U8 u8LockRegVal = 0;
    HI_U32 u32TunerPort = 0;

    while(!kthread_should_stop())
    {
        wait_event_interruptible_timeout( g_QamSpecialProcessWQ, g_bLockFlag, msecs_to_jiffies( 100 ) );

        if ( g_bLockFlag )
        {
            msleep_interruptible( 150 );

            qam_read_byte(s_u32InsideQamPort, MCTRL_11_ADDR, &u8LockRegVal);
            if ( 0xf8 != u8LockRegVal )
            {
                if ( QAM_TYPE_256 == g_stTunerOps[u32TunerPort].u32CurrQamMode )
                {
                    qam_write_byte(s_u32InsideQamPort, EQU_CTRL_3_ADDR, 0x45);
                }

                /* reset eq */
                qam_write_byte(s_u32InsideQamPort, MCTRL_1_ADDR, 0x08);
                qam_write_byte(s_u32InsideQamPort, MCTRL_1_ADDR, 0x00);

                qam_write_byte(s_u32InsideQamPort, DEPHASE_CTRL_ADDR, 0x0b);

            }
            else
            {
                if ( QAM_TYPE_256 == g_stTunerOps[u32TunerPort].u32CurrQamMode )
                {
                    qam_write_byte(s_u32InsideQamPort, EQU_CTRL_3_ADDR, 0x33);
                }
                qam_write_byte(s_u32InsideQamPort, DEPHASE_CTRL_ADDR, 0x0b);
                g_bLockFlag = HI_FALSE;
            }
        }

    }

    return 0;
}

HI_S32 HI_DRV_TUNER_Init(HI_VOID)
{
    HI_U32 i;
    HI_CHIP_TYPE_E enChipType;
    HI_U32 u32ChipVersion = 0;

   // tuner_enable_crg();

#if 0
    HI_DRV_SYS_GetChipVersion( &enChipType, &u32ChipVersion );

    /*if (HI_CHIP_TYPE_HI3712 == enChipType)
    {
        SocSpiGpio_ModInit();
    }*/
#endif

    g_stTunerOps[0].u32TunerAddress = TUNER_PORT0_ADDR;
    g_stTunerOps[1].u32TunerAddress = TUNER_PORT1_ADDR;
    g_stTunerOps[2].u32TunerAddress = TUNER_PORT2_ADDR;
    g_stTunerOps[3].u32TunerAddress = TUNER_PORT3_ADDR;
    g_stTunerOps[4].u32TunerAddress = TUNER_PORT4_ADDR;
	
    g_stTunerOps[0].u32DemodAddress = QAM_PORT0_ADDR;
    g_stTunerOps[1].u32DemodAddress = QAM_PORT1_ADDR;
    g_stTunerOps[2].u32DemodAddress = QAM_PORT2_ADDR;
    g_stTunerOps[3].u32DemodAddress = QAM_PORT3_ADDR;
    g_stTunerOps[4].u32DemodAddress = QAM_PORT4_ADDR;
    /* init all tuner ops as none operation */
    for (i = 0; i < TUNER_NUM; i++)
    {
        g_stTunerOps[i].u32XtalClk = XTAL_CLK;
        g_stTunerOps[i].u32CurrQamMode = QAM_TYPE_64;
        g_stTunerOps[i].u8AdcType = QAM_AD_INSIDE;
        g_stTunerOps[i].u8AgcOutputSel = QAM_AGC_CMOS_OUTPUT;
        g_stTunerOps[i].u8AdcDataFmt = 1;
        g_stTunerOps[i].enTsType = HI_UNF_TUNER_OUTPUT_MODE_PARALLEL_MODE_A;

        g_stTunerOps[i].tuner_connect = NULL;
        g_stTunerOps[i].tuner_get_status = NULL;
        g_stTunerOps[i].tuner_get_ber = NULL;
        g_stTunerOps[i].tuner_get_snr = NULL;
        g_stTunerOps[i].tuner_get_signal_strength = NULL;
        g_stTunerOps[i].tuner_get_snr = NULL;
        g_stTunerOps[i].tuner_set_ts_type = NULL;
        g_stTunerOps[i].set_tuner   = NULL;
        g_stTunerOps[i].tuner_test_single_agc = NULL;
        g_stTunerOps[i].enI2cChannel = i;
        g_stTunerOps[i].tuner_resume = NULL;
        g_stTunerOps[i].tuner_get_rs = NULL;
        g_stTunerOps[i].tuner_get_registers = NULL;
        g_stTunerOps[i].tuner_connect_timeout = NULL;

        /* Added begin:l00185424 2011-11-28 For DVB-S/S2 */
        g_stTunerOps[i].tuner_init = NULL;
        g_stTunerOps[i].tuner_set_sat_attr = NULL;
        g_stTunerOps[i].tuner_get_signal_info = NULL;
        g_stTunerOps[i].tuner_blindscan_init   = NULL;
        g_stTunerOps[i].tuner_blindscan_action = NULL;
        g_stTunerOps[i].tuner_lnbctrl_dev_init = NULL;
        g_stTunerOps[i].tuner_lnbctrl_dev_standby = NULL;
        g_stTunerOps[i].tuner_set_lnb_out = NULL;
        g_stTunerOps[i].tuner_send_continuous_22K = NULL;
        g_stTunerOps[i].tuner_send_tone = NULL;
        g_stTunerOps[i].tuner_DiSEqC_send_msg = NULL;
        g_stTunerOps[i].tuner_DiSEqC_recv_msg = NULL;
        g_stTunerOps[i].tuner_setfuncmode = NULL;
        g_stTunerOps[i].tuner_setplpid = NULL;
        g_stTunerOps[i].tuner_standby = NULL;
        g_stTunerOps[i].tuner_tp_verify = NULL;
        g_stTunerOps[i].tuner_set_ts_out = NULL;
        g_stTunerOps[i].tuner_data_sample = NULL;
        /* Added end:l00185424 2011-11-28 For DVB-S/S2 */
    }

    init_waitqueue_head(&g_QamSpecialProcessWQ);
    pThread = kthread_create(x5hdqam_special_process, NULL, "x5hdqam_sop");
    wake_up_process(pThread);

    return HI_SUCCESS;
}

HI_VOID  HI_DRV_TUNER_DeInit(HI_VOID)
{
    kthread_stop(pThread);

    return;
}

/*
 * config to decide if i2c can be switch to control tuner
 */
HI_VOID qam_config_i2c_out(HI_U32 u32TunerPort, HI_BOOL bTuner)
{
    if (bTuner)
    {
        (HI_VOID)qam_write_bit(u32TunerPort, MCTRL_7_ADDR, 7, 1); /*set I2C_repeater_en 1*/
    }
}


HI_S32 tuner_i2c_receive_data(HI_U32 u32I2cChannel, HI_U8 u8DevAddress, PTR_TUNER_I2C_DATA_S pstDataStruct)
{
    HI_U32  u32I2cNo = 0;
    HI_U32  u32DevAddr = 0;
    HI_U32  u32RegAddr = 0;
    HI_U32  u32RegNum = 0;
    HI_U32  u32DataLen = 0;
    HI_U8   *pu8Data = NULL;
    HI_S32  s32Ret = HI_SUCCESS;

    u32I2cNo = u32I2cChannel;
    if (HI_I2C_MAX_NUM_USER < u32I2cNo)
    {
        return HI_FAILURE;
    }

    u32DevAddr = u8DevAddress;
    u32RegAddr = *(pstDataStruct->pu8SendBuf);
    u32RegNum = pstDataStruct->u32SendLength;

    u32DataLen = pstDataStruct->u32ReceiveLength;
    pu8Data = pstDataStruct->pu8ReceiveBuf;
#if defined (CHIP_TYPE_hi3716cv200es)||defined (CHIP_TYPE_hi3716cv200)
    if(HI_STD_I2C_NUM > u32I2cNo)
    {
        s32Ret = HI_DRV_I2C_Read(u32I2cNo, (HI_U8)u32DevAddr, u32RegAddr, u32RegNum, pu8Data, u32DataLen);
        if(HI_SUCCESS != s32Ret)
        {
            return s32Ret;
        }
    }
    else
    {
        s32Ret = HI_DRV_GPIOI2C_ReadExt(u32I2cNo, (HI_U8)u32DevAddr, (HI_U8)u32RegAddr, u32RegNum, pu8Data, u32DataLen);
        if(HI_SUCCESS != s32Ret)
        {
            return s32Ret;
        }
    }
#elif defined (CHIP_TYPE_hi3798cv100)
    if(HI_STD_I2C_NUM > u32I2cNo)
    {
        s32Ret = s_pI2cFunc->pfnI2cRead(u32I2cNo, (HI_U8)u32DevAddr, u32RegAddr, u32RegNum, pu8Data, u32DataLen);
        if(HI_SUCCESS != s32Ret)
        {
            return s32Ret;
        }
    }
    else
    {
        s32Ret = s_pGpioI2cFunc->pfnGpioI2cReadExt(u32I2cNo, (HI_U8)u32DevAddr, (HI_U8)u32RegAddr, u32RegNum, pu8Data, u32DataLen);
        if(HI_SUCCESS != s32Ret)
        {
            return s32Ret;
        }
    }
#else
    if (4/*HI_UNF_I2C_CHANNEL_QAM*/ >= u32I2cNo)
    {
        s32Ret = HI_DRV_I2C_Read(u32I2cNo, (HI_U8)u32DevAddr, u32RegAddr, u32RegNum, pu8Data, u32DataLen);
        if(HI_SUCCESS != s32Ret)
        {
            return s32Ret;
        }
    }
#endif
    /*else
    {
        s32Ret = gpio_i2c_read_ext(u32I2cNo, (HI_U8)u32DevAddr, (HI_U8)u32RegAddr, u32RegNum, pu8Data, u32DataLen);
        if(HI_SUCCESS != s32Ret)
        {
            return s32Ret;
        }
    }*/

    return HI_SUCCESS;
}


HI_S32 qam_read_byte(HI_U32 u32TunerPort, HI_U8 u8RegAddr, HI_U8 *pu8RegVal)
{

    TUNER_I2C_DATA_S stI2cdataStr = {0};
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U8 u8Data[4];
    HI_U8 u8SendData[4];

    u8Data[0] = 0;
    u8SendData[0] = 0;

#warning qam_read_byte: TODO

    if (HI_NULL == pu8RegVal)
    {
        HI_ERR_TUNER( "pointer is null\n"); //4254
        return HI_ERR_TUNER_INVALID_POINT;
    }

    if (tuner_check_port(u32TunerPort))
    {
        HI_ERR_TUNER( "invalid parameter TunerPort:%d\n", u32TunerPort ); //4260
    	return HI_FAILURE;
    }

    u8SendData[0] = u8RegAddr;

    stI2cdataStr.pu8ReceiveBuf = &u8Data[0];
    stI2cdataStr.u32ReceiveLength = 1;
    stI2cdataStr.pu8SendBuf = &u8SendData[0]; //&u8RegAddr;
    stI2cdataStr.u32SendLength = 1;

    if (HI_UNF_DEMOD_DEV_TYPE_3136I == g_stTunerOps[u32TunerPort].enDemodDevType)
    {
        //printk("%s, %d, HI3136I_I2C_CHAN = %d, %x\n", __func__, __LINE__, HI3136I_I2C_CHAN, g_stTunerOps[u32TunerPort].u32DemodAddress);
        s32Ret = tuner_i2c_receive_data(HI3136I_I2C_CHAN,
        		g_stTunerOps[u32TunerPort].u32DemodAddress, &stI2cdataStr);
    }
    else
    {
        s32Ret = tuner_i2c_receive_data(g_stTunerOps[u32TunerPort].enI2cChannel,
        		g_stTunerOps[u32TunerPort].u32DemodAddress, &stI2cdataStr);
    }
    if (HI_SUCCESS == s32Ret)
    {
        *pu8RegVal = u8Data[0];
    }

    return s32Ret;
}

HI_S32 qam_read_bit(HI_U32 u32TunerPort, HI_U8 u8RegAddr, HI_U8 u8BitNum, HI_U8 *pu8BitVal)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U8  u8Tmp = 0;

    if (HI_NULL == pu8BitVal)
    {
        HI_INFO_TUNER( "pointer(pu8BitVal) is null\n"); //4297
        return HI_ERR_TUNER_INVALID_POINT;
    }

    s32Ret = qam_read_byte(u32TunerPort, u8RegAddr, &u8Tmp);
    if (HI_SUCCESS != s32Ret)
    {
        return s32Ret;
    }

    if ((1 << u8BitNum) == (u8Tmp & (1 << u8BitNum)))
    {
        *pu8BitVal = 1;
    }
    else
    {
        *pu8BitVal = 0;
    }

    return HI_SUCCESS;
}


HI_S32 tuner_i2c_send_data(HI_U32 u32I2cChannel, HI_U8 u8DevAddress, PTR_TUNER_I2C_DATA_S pstDataStruct)
{
    HI_U32  u32I2cNo = 0;
    HI_U32  u32DevAddr = 0;
    HI_U32  u32RegAddr = 0;
    HI_U32  u32DataLen = 0;
    HI_U32  u32RegNum = 0;
    HI_U8   *pData = NULL;
    HI_S32  s32Ret = HI_SUCCESS;

    u32I2cNo = u32I2cChannel;
    if (HI_I2C_MAX_NUM_USER < u32I2cNo)
    {
        return HI_FAILURE;
    }

    u32DevAddr = u8DevAddress;
    u32RegAddr = *(pstDataStruct->pu8SendBuf);
    u32RegNum  = 1;

    pData      = pstDataStruct->pu8SendBuf + 1;
    u32DataLen = pstDataStruct->u32SendLength - 1;

#if defined (CHIP_TYPE_hi3716cv200es)||defined (CHIP_TYPE_hi3716cv200)
    if(HI_STD_I2C_NUM >u32I2cNo)
    {
        s32Ret = HI_DRV_I2C_Write(u32I2cNo, (HI_U8)u32DevAddr, u32RegAddr, u32RegNum, pData, u32DataLen);
        if(HI_SUCCESS != s32Ret)
        {
            return s32Ret;
        }
    }
    else
    {
        s32Ret = HI_DRV_GPIOI2C_WriteExt(u32I2cNo,(HI_U8)u32DevAddr, (HI_U8)u32RegAddr, u32RegNum, pData, u32DataLen);
        if(HI_SUCCESS != s32Ret)
        {
            return s32Ret;
        }
    }	
#elif defined (CHIP_TYPE_hi3798cv100)
    if(HI_STD_I2C_NUM > u32I2cNo)
    {
        s32Ret = s_pI2cFunc->pfnI2cWrite(u32I2cNo, (HI_U8)u32DevAddr, u32RegAddr, u32RegNum, pData, u32DataLen);
        if(HI_SUCCESS != s32Ret)
        {
            return s32Ret;
        }
    }
    else
    {
        s32Ret = s_pGpioI2cFunc->pfnGpioI2cWriteExt(u32I2cNo, (HI_U8)u32DevAddr, (HI_U8)u32RegAddr, u32RegNum, pData, u32DataLen);
        if(HI_SUCCESS != s32Ret)
        {
            return s32Ret;
        }
    }
#else
    if (4/*HI_UNF_I2C_CHANNEL_QAM*/ >= u32I2cNo)
    {
        s32Ret = HI_DRV_I2C_Write(u32I2cNo, (HI_U8)u32DevAddr, u32RegAddr, u32RegNum, pData, u32DataLen);
        if(HI_SUCCESS != s32Ret)
        {
            return s32Ret;
        }
    }

    /*else
    {
        s32Ret = gpio_i2c_write_ext(u32I2cNo, (HI_U8)u32DevAddr, (HI_U8)u32RegAddr, u32RegNum, pData, u32DataLen);
        if(HI_SUCCESS != s32Ret)
        {
            return s32Ret;
        }
    }*/
   #endif
    return HI_SUCCESS;
}

HI_VOID tuner_mdelay(HI_U32 u32MS)
{
    ktime_t wait;

    wait = ns_to_ktime(u32MS * 1000000);//ns
    set_current_state(TASK_UNINTERRUPTIBLE);
    schedule_hrtimeout(&wait, HRTIMER_MODE_REL);
}

HI_S32 qam_write_byte(HI_U32 u32TunerPort, HI_U8 u8RegAddr, HI_U8 u8RegVal)
{

    TUNER_I2C_DATA_S stI2cDataStr = {0};
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U8 au8SendData[2] = {0};

#warning qam_write_byte: TODO

    if (tuner_check_port(u32TunerPort))
    {
        HI_ERR_TUNER( "invalid parameter TunerPort:%d\n", u32TunerPort ); //4404
    	return HI_FAILURE;
    }

    au8SendData[0] = u8RegAddr;
    au8SendData[1] = u8RegVal;

    stI2cDataStr.pu8ReceiveBuf = HI_NULL;
    stI2cDataStr.u32ReceiveLength = 0;
    stI2cDataStr.pu8SendBuf = au8SendData;
    stI2cDataStr.u32SendLength = 2;

    if (HI_UNF_DEMOD_DEV_TYPE_3136I == g_stTunerOps[u32TunerPort].enDemodDevType)
    {
        //printk("%s, %d, HI3136I_I2C_CHAN = %d, %x\n", __func__, __LINE__, HI3136I_I2C_CHAN, g_stTunerOps[u32TunerPort].u32DemodAddress);
        s32Ret = tuner_i2c_send_data(HI3136I_I2C_CHAN,
            g_stTunerOps[u32TunerPort].u32DemodAddress, &stI2cDataStr);
    }
    else
    {
        s32Ret = tuner_i2c_send_data(g_stTunerOps[u32TunerPort].enI2cChannel,
            g_stTunerOps[u32TunerPort].u32DemodAddress, &stI2cDataStr);
    }

    return s32Ret;
}

HI_S32 qam_write_bit(HI_U32 u32TunerPort, HI_U8 u8RegAddr, HI_U8 u8BitNum, HI_U8 u8BitVal)
{
    HI_U8  u8Data  = 0;
    HI_S32 s32Ret = HI_SUCCESS;

    if (u8BitVal > 1)
    {
        HI_INFO_TUNER( "invalid parameter(u8BitVal>1):%d\n", u8BitVal); //4438
        return HI_FAILURE;
    }

    if (u8BitNum > 7)
    {
        HI_INFO_TUNER( "invalid parameter(u8BitNum>7):%d\n", u8BitNum); //4444
        return HI_FAILURE;
    }

    s32Ret = qam_read_byte(u32TunerPort, u8RegAddr, &u8Data);
    if (HI_SUCCESS != s32Ret)
    {
        return s32Ret;
    }

    if (u8BitVal)
    {
        u8Data |= (1 << u8BitNum);
    }
    else
    {
        u8Data &= ~(1 << u8BitNum);
    }

    s32Ret = qam_write_byte(u32TunerPort, u8RegAddr, u8Data);

    return s32Ret;
}

#if 0
HI_S32 tuner_base_ioctl(unsigned int cmd, HI_VOID *arg)
{
    HI_S32 s32Ret;

    if (HI_NULL == arg)
    {
        HI_INFO_TUNER("data(arg) error: cmd=%d\n", cmd);
        return HI_FAILURE;
    }

    s32Ret = down_interruptible(&g_TunerMutex);
    switch (cmd)
    {
        case TUNER_CONNECT_CMD:
            {
                s32Ret = tuner_osr_connect((TUNER_SIGNAL_S *)arg);
                break;
            }
        case TUNER_GET_STATUS_CMD:
            {
                s32Ret = tuner_osr_getstatus((TUNER_DATA_S *)arg);
                break;
            }
        case TUNER_SELECT_TYPE_CMD:
            {
                s32Ret = tuner_osr_selecttype((TUNER_DATABUF_S *)arg);
                break;
            }
        case TUNER_SELECT_I2C_CMD:
            {
                s32Ret = tuner_osr_selecti2cchannel((TUNER_DATA_S *)arg);
                break;
            }
        case TUNER_SET_TSTYPE_CMD:
            {
                s32Ret = tuner_osr_settstype((TUNER_DATA_S *)arg);
                break;
            }
        case TUNER_CONNECT_TIMEOUT_CMD:
            {
                s32Ret = tuner_osr_connect_timeout((TUNER_DATABUF_S *)arg);
                break;
            }

        default:
            {
                HI_INFO_TUNER(" Invalid Ioctl command: 0x%x\n", cmd);
                up(&g_TunerMutex);
                return -ENOIOCTLCMD;
            }
    }

    up(&g_TunerMutex);
    return s32Ret;
}
#endif

HI_S32 tuner_chip_reset(HI_U32 u32ResetGpioNo)
{
    HI_U32 u32Ret = HI_FAILURE;
    u32Ret  = s_pGpioFunc->pfnGpioDirSetBit(u32ResetGpioNo, HI_FALSE);
    //u32Ret  = HI_DRV_GPIO_SetDirBit(u32ResetGpioNo, HI_FALSE);
    u32Ret |= s_pGpioFunc->pfnGpioWriteBit(u32ResetGpioNo, 0);
    //u32Ret |= HI_DRV_GPIO_WriteBit(u32ResetGpioNo, 0);
    msleep(100);
    u32Ret |= s_pGpioFunc->pfnGpioWriteBit(u32ResetGpioNo, 1);
//    u32Ret |= HI_DRV_GPIO_WriteBit(u32ResetGpioNo, 1);
    if (HI_SUCCESS != u32Ret)
    {
        HI_INFO_TUNER("Demod reset failed !\n"); //4535
        return u32Ret;
    }

    msleep(50);
    return HI_SUCCESS;
}

HI_S32 HI_DRV_TUNER_SetLNBPower(HI_U32 u32TunerId, HI_UNF_TUNER_FE_LNB_POWER_E enLNBPower)
{
    TUNER_LNB_OUT_S stLNBOut;
    HI_S32 s32Ret;

    if (TUNER_NUM <= u32TunerId)
    {
        HI_ERR_TUNER("Input parameter(u32TunerId) invalid,invalid tunerId is: %d\n", u32TunerId); //4550
        return HI_ERR_TUNER_INVALID_PORT;
    }

    if (HI_UNF_TUNER_FE_LNB_POWER_BUTT <= enLNBPower)
    {
        HI_ERR_TUNER("Input parameter(pstLNB) invalid\n"); //4556
        return HI_ERR_TUNER_INVALID_PARA;
    }

    stLNBOut.u32Port = u32TunerId;

    switch (enLNBPower)
    {
    case HI_UNF_TUNER_FE_LNB_POWER_ON:
        stLNBOut.enOut = TUNER_LNB_OUT_18V;
        break;

    case HI_UNF_TUNER_FE_LNB_POWER_ENHANCED:
        stLNBOut.enOut = TUNER_LNB_OUT_19V;
        break;

    case HI_UNF_TUNER_FE_LNB_POWER_OFF:
        stLNBOut.enOut = TUNER_LNB_OUT_0V;
        break;

    default:
        return HI_ERR_TUNER_INVALID_PARA;
    }

    s32Ret = down_interruptible(&g_TunerMutex);
    /* if tuner blind scan operation is not setted, return fault */
    if (!g_stTunerOps[u32TunerId].tuner_set_lnb_out)
    {
        HI_INFO_TUNER( "tuner service function ptr is NULL!\n"); //4584
        up(&g_TunerMutex);
        return HI_FAILURE;
    }

    s32Ret = g_stTunerOps[u32TunerId].tuner_set_lnb_out(u32TunerId, stLNBOut.enOut);
    up(&g_TunerMutex);
    if (HI_SUCCESS != s32Ret)
    {
        HI_INFO_TUNER( "tuner_set_lnb_out failed!\n"); //4593
        return s32Ret;
    }

    s_stMCESatPara[u32TunerId].enLNBPower = enLNBPower;

    return HI_SUCCESS;
}

HI_S32 HI_DRV_TUNER_GetLNBPower(HI_U32    u32tunerId, HI_UNF_TUNER_FE_LNB_POWER_E*     enLNBPower)
{
    if(TUNER_NUM <= u32tunerId)
    {
        HI_ERR_TUNER("Input parameter(u32tunerId)invalid,invalid tunerId is: %d\n",u32tunerId); //4606
        return HI_ERR_TUNER_INVALID_PORT;
    }

    if (!enLNBPower)
    {
    	HI_ERR_TUNER("Input parameter(enLNBPower) invalid\n"); //4612
        return HI_ERR_TUNER_INVALID_POINT;
    }

    *enLNBPower = s_stMCESatPara[u32tunerId].enLNBPower;

	return HI_SUCCESS;
}

typedef struct
{
    HI_UNF_TUNER_DISEQC_SWITCH4PORT_S stPort1_0;
    HI_UNF_TUNER_DISEQC_SWITCH16PORT_S stPort1_1;
} DISEQC_STATUS_S;

static HI_UNF_TUNER_SWITCH_22K_E s_enSwitch22K[TUNER_NUM] =
{
    HI_UNF_TUNER_SWITCH_22K_NONE,
    HI_UNF_TUNER_SWITCH_22K_NONE,
    HI_UNF_TUNER_SWITCH_22K_NONE,
    HI_UNF_TUNER_SWITCH_22K_NONE,
    HI_UNF_TUNER_SWITCH_22K_NONE
};

static DISEQC_STATUS_S s_stDiSEqCStatus[TUNER_NUM] =
{
    {
        .stPort1_0 =
        {
            HI_UNF_TUNER_DISEQC_LEVEL_1_X,
            HI_UNF_TUNER_DISEQC_SWITCH_NONE,
            HI_UNF_TUNER_FE_POLARIZATION_H,
            HI_UNF_TUNER_FE_LNB_22K_OFF
        },
        .stPort1_1 =
        {
            HI_UNF_TUNER_DISEQC_LEVEL_1_X,
            HI_UNF_TUNER_DISEQC_SWITCH_NONE
        }
    },
    {
        .stPort1_0 =
        {
            HI_UNF_TUNER_DISEQC_LEVEL_1_X,
            HI_UNF_TUNER_DISEQC_SWITCH_NONE,
            HI_UNF_TUNER_FE_POLARIZATION_H,
            HI_UNF_TUNER_FE_LNB_22K_OFF
        },
        .stPort1_1 =
        {
            HI_UNF_TUNER_DISEQC_LEVEL_1_X,
            HI_UNF_TUNER_DISEQC_SWITCH_NONE
        }
    },
    {
        .stPort1_0 =
        {
            HI_UNF_TUNER_DISEQC_LEVEL_1_X,
            HI_UNF_TUNER_DISEQC_SWITCH_NONE,
            HI_UNF_TUNER_FE_POLARIZATION_H,
            HI_UNF_TUNER_FE_LNB_22K_OFF
        },
        .stPort1_1 =
        {
            HI_UNF_TUNER_DISEQC_LEVEL_1_X,
            HI_UNF_TUNER_DISEQC_SWITCH_NONE
        }
    },
    {
        .stPort1_0 =
        {
            HI_UNF_TUNER_DISEQC_LEVEL_1_X,
            HI_UNF_TUNER_DISEQC_SWITCH_NONE,
            HI_UNF_TUNER_FE_POLARIZATION_H,
            HI_UNF_TUNER_FE_LNB_22K_OFF
        },
        .stPort1_1 =
        {
            HI_UNF_TUNER_DISEQC_LEVEL_1_X,
            HI_UNF_TUNER_DISEQC_SWITCH_NONE
        }
    },
    {
        .stPort1_0 =
        {
            HI_UNF_TUNER_DISEQC_LEVEL_1_X,
            HI_UNF_TUNER_DISEQC_SWITCH_NONE,
            HI_UNF_TUNER_FE_POLARIZATION_H,
            HI_UNF_TUNER_FE_LNB_22K_OFF
        },
        .stPort1_1 =
        {
            HI_UNF_TUNER_DISEQC_LEVEL_1_X,
            HI_UNF_TUNER_DISEQC_SWITCH_NONE
        }
    }
};

static HI_S32 TUNER_DISEQC_Send22K(HI_U32 u32TunerId, HI_BOOL bStatus)
{
    TUNER_DATA_S stTunerData;
    HI_S32 s32Ret = HI_FAILURE;

    stTunerData.u32Port = u32TunerId;
    stTunerData.u32Data = bStatus ? 1 : 0;

    s32Ret = down_interruptible(&g_TunerMutex);
    /* if tuner blind scan operation is not setted, return fault */
    if (!g_stTunerOps[u32TunerId].tuner_send_continuous_22K)
    {
        HI_INFO_TUNER( "tuner service function ptr is NULL!\n"); //4721
        up(&g_TunerMutex);
        return HI_FAILURE;
    }

    s32Ret = g_stTunerOps[u32TunerId].tuner_send_continuous_22K(u32TunerId, stTunerData.u32Data);
    up(&g_TunerMutex);
    if (HI_SUCCESS != s32Ret)
    {
        HI_ERR_TUNER("Set continuous 22K fail.\n"); //4730
        return HI_ERR_TUNER_FAILED_DISEQC;
    }

    return HI_SUCCESS;
}

static HI_S32 TUNER_DISEQC_Stop22K(HI_U32 u32TunerId)
{
    HI_S32 s32Ret = HI_FAILURE;

    s32Ret = TUNER_DISEQC_Send22K(u32TunerId, HI_FALSE);
    if (HI_SUCCESS != s32Ret)
    {
        HI_ERR_TUNER("Stop 22K fail.\n"); //4744
        return HI_ERR_TUNER_FAILED_SWITCH;
    }

    return HI_SUCCESS;
}

static HI_S32 TUNER_DISEQC_Resume22K(HI_U32 u32TunerId)
{
    /* Resume */
    if (HI_UNF_TUNER_SWITCH_22K_22 == s_enSwitch22K[u32TunerId])
    {
        return TUNER_DISEQC_Send22K(u32TunerId, HI_TRUE);
    }
    else
    {
        return HI_SUCCESS;
    }
}

HI_S32 HI_DRV_TUNER_Switch22K(HI_U32 u32TunerId, HI_UNF_TUNER_SWITCH_22K_E enPort)
{
    if (TUNER_NUM <= u32TunerId)
    {
        HI_ERR_TUNER("Input parameter(u32TunerId) invalid,invalid tunerId is: %d\n", u32TunerId); //4768
        return HI_ERR_TUNER_INVALID_PORT;
    }

    if (HI_UNF_TUNER_SWITCH_22K_BUTT <= enPort)
    {
        HI_ERR_TUNER("Input parameter(enPort) invalid\n"); //4774
        return HI_ERR_TUNER_INVALID_PARA;
    }

    /* Save */
    s_enSwitch22K[u32TunerId] = enPort;

    if (HI_UNF_TUNER_SWITCH_22K_0 == enPort)
    {
        return TUNER_DISEQC_Send22K(u32TunerId, HI_FALSE);
    }
    else if (HI_UNF_TUNER_SWITCH_22K_22 == enPort)
    {
        return TUNER_DISEQC_Send22K(u32TunerId, HI_TRUE);
    }
    else
    {
        return HI_SUCCESS;
    }
}

#define DISEQC_DELAY_TIME_MS (15)

HI_S32 TUNER_DISEQC_SendRecvMessage(HI_U32 u32TunerId,
                                           const HI_UNF_TUNER_DISEQC_SENDMSG_S * pstSendMsg,
                                           HI_UNF_TUNER_DISEQC_RECVMSG_S * pstRecvMsg)
{
    TUNER_DISEQC_SENDMSG_S stSend;
    TUNER_DISEQC_RECVMSG_S stRecv;
    TUNER_DATA_S stTunerData = {0};
    HI_S32 s32Ret = HI_FAILURE;
    HI_U32 u32RepeatTimes;
    HI_U32 u32RepeatTime = 0;
    HI_BOOL bSendTone = HI_FALSE;

    if (TUNER_NUM <= u32TunerId)
    {
        HI_ERR_TUNER("Input parameter(u32TunerId) invalid,invalid tunerId is: %d\n", u32TunerId); //4811
        return HI_ERR_TUNER_INVALID_PORT;
    }

    if (HI_NULL == pstSendMsg)
    {
        HI_ERR_TUNER("Input parameter(pstSendMsg) invalid\n");
        return HI_ERR_TUNER_INVALID_POINT;
    }

    if (HI_UNF_TUNER_DISEQC_LEVEL_BUTT <= pstSendMsg->enLevel)
    {
        HI_ERR_TUNER("Input parameter(enLevel) invalid\n"); //4823
        return HI_ERR_TUNER_INVALID_PARA;
    }

    if (HI_UNF_TUNER_SWITCH_TONEBURST_BUTT <= pstSendMsg->enToneBurst)
    {
        HI_ERR_TUNER("Input parameter(enToneBurst) invalid\n");
        return HI_ERR_TUNER_INVALID_PARA;
    }

    if (HI_UNF_DISEQC_MSG_MAX_LENGTH < pstSendMsg->u8Length)
    {
        HI_ERR_TUNER("Input parameter(u8Length) invalid\n");
        return HI_ERR_TUNER_INVALID_PARA;
    }

    if (HI_UNF_DISEQC_MAX_REPEAT_TIMES < pstSendMsg->u8RepeatTimes)
    {
        HI_ERR_TUNER("Input parameter(u8RepeatTimes) invalid\n");
        return HI_ERR_TUNER_INVALID_PARA;
    }

    /* pstRecvMsg can be NULL */

    /* Handle tone burst */
    stTunerData.u32Port = u32TunerId;
    switch (pstSendMsg->enToneBurst)
    {
    case HI_UNF_TUNER_SWITCH_TONEBURST_NONE:
        bSendTone = HI_FALSE;
        break;

    case HI_UNF_TUNER_SWITCH_TONEBURST_0:
        bSendTone = HI_TRUE;
        stTunerData.u32Data = 0;
        break;

    case HI_UNF_TUNER_SWITCH_TONEBURST_1:
        bSendTone = HI_TRUE;
        stTunerData.u32Data = 1;
        break;
    default:
        HI_ERR_TUNER("Input parameter invalid!\n");
        return HI_ERR_TUNER_INVALID_PARA;
    }

    /* Stop continuous 22K */
    (HI_VOID)TUNER_DISEQC_Stop22K(u32TunerId);
    msleep(DISEQC_DELAY_TIME_MS);

    /* Send command */
    stSend.u32Port = u32TunerId;
    stSend.stSendMsg = *pstSendMsg;
    u32RepeatTimes = pstSendMsg->u8RepeatTimes;
    //while (/*u32RepeatTimes >= 0*/1)
    for(;;)
    {
        /* Handle repeat */
        if (u32RepeatTime == 1)
        {
            stSend.stSendMsg.au8Msg[0] += 1;
        }

        /* Send command */
        s32Ret = down_interruptible(&g_TunerMutex);
        /* if tuner unsupport DiSEqC, return fault */
        if (!g_stTunerOps[u32TunerId].tuner_DiSEqC_send_msg) /* check if tuner get freq and symb offset is supported */
        {
            HI_INFO_TUNER( "tuner service function ptr is NULL!\n"); //4891
            up(&g_TunerMutex);
            return HI_FAILURE;
        }

        s32Ret = g_stTunerOps[u32TunerId].tuner_DiSEqC_send_msg(u32TunerId, &(stSend.stSendMsg));
        up(&g_TunerMutex);
        if (HI_SUCCESS != s32Ret)
        {
            HI_ERR_TUNER("Send DiSEqC message fail.\n"); //4900
            return HI_ERR_TUNER_FAILED_DISEQC;
        }

        /* After send command, delay 15ms */
        msleep(DISEQC_DELAY_TIME_MS);

        /* Send tone */
        if (bSendTone)
        {
            /* if tuner blind scan operation is not setted, return fault */
            s32Ret = down_interruptible(&g_TunerMutex);
            if (!g_stTunerOps[u32TunerId].tuner_send_tone)
            {
                HI_INFO_TUNER( "tuner service function ptr is NULL!\n"); //4914
                up(&g_TunerMutex);
                return HI_FAILURE;
            }

            s32Ret = g_stTunerOps[u32TunerId].tuner_send_tone(u32TunerId, stTunerData.u32Data);
            up(&g_TunerMutex);
            if (HI_SUCCESS != s32Ret)
            {
                HI_ERR_TUNER("Send tone fail.\n"); //4923
                return HI_ERR_TUNER_FAILED_DISEQC;
            }

            /* After send tone, delay 15ms */
            msleep(DISEQC_DELAY_TIME_MS);
        }

        if (u32RepeatTimes == 0)
        {
            break;
        }

        u32RepeatTimes--;
        u32RepeatTime++;
    }

    /* Recv msessage */
    stRecv.u32Port = u32TunerId;
    stRecv.pstRecvMsg = pstRecvMsg;
    if (HI_NULL != pstRecvMsg)
    {
        if (HI_UNF_TUNER_DISEQC_LEVEL_2_X == pstSendMsg->enLevel)
        {
            /* if tuner unsupport DiSEqC, return fault */
            s32Ret = down_interruptible(&g_TunerMutex);
            if (!g_stTunerOps[u32TunerId].tuner_DiSEqC_recv_msg) /* check if tuner get freq and symb offset is supported */
            {
                HI_INFO_TUNER( "tuner service function ptr is NULL!\n"); //4951
                up(&g_TunerMutex);
                return HI_FAILURE;
            }

            s32Ret = g_stTunerOps[u32TunerId].tuner_DiSEqC_recv_msg(u32TunerId, stRecv.pstRecvMsg);
            up(&g_TunerMutex);
            if (HI_SUCCESS != s32Ret)
            {
                HI_ERR_TUNER("Recv DiSEqC message fail.\n"); //4960
                return HI_ERR_TUNER_FAILED_DISEQC;
            }

            msleep(DISEQC_DELAY_TIME_MS);
        }
        else
        {
            stRecv.pstRecvMsg->enStatus = HI_UNF_TUNER_DISEQC_RECV_UNSUPPORT;
            stRecv.pstRecvMsg->u8Length = 0;
        }
    }

    (HI_VOID)TUNER_DISEQC_Resume22K(u32TunerId);

    return HI_SUCCESS;
}

static HI_S32 DISEQC_SendCmd1_0(HI_U32 u32TunerId, const HI_UNF_TUNER_DISEQC_SWITCH4PORT_S* pstPara)
{
    HI_UNF_TUNER_DISEQC_SENDMSG_S stSendMsg;
    //HI_UNF_TUNER_DISEQC_RECVMSG_S stRecvMsg;
    HI_S32 s32Ret;
    HI_U8 u8PortGroupBits;
    HI_U8 u8CMD;
    HI_U8 u8Framing;
    HI_U8 u8Polar;
    HI_U8 u8LOFreq;

    /* Polarization */
    if ((HI_UNF_TUNER_FE_POLARIZATION_V == pstPara->enPolar)
       || (HI_UNF_TUNER_FE_POLARIZATION_R == pstPara->enPolar))
    {
        u8Polar = POLARISATION_V;
    }
    else
    {
        u8Polar = POLARISATION_H;
    }

    /* LO, 22K */
    u8LOFreq = (HI_UNF_TUNER_FE_LNB_22K_ON == pstPara->enLNB22K) ? LO_FREQUENCY_H : LO_FREQUENCY_L;

    /* Init parameter */
    memset(&stSendMsg, 0, sizeof(stSendMsg));
    //memset(&stRecvMsg, 0, sizeof(stRecvMsg));
    stSendMsg.enLevel = pstPara->enLevel;
    //stSendMsg.enToneBurst = TUNER_DISEQC_GetToneBurstStatus(u32TunerId);
    stSendMsg.u8Length = 4;
    stSendMsg.u8RepeatTimes = 0;

    u8Framing = (pstPara->enLevel == HI_UNF_TUNER_DISEQC_LEVEL_2_X) ? MASTER_REPLY_FIRST : MASTER_NOREPLY_FIRST;

    /* Send WRITE N0(0x38) command */
    u8PortGroupBits = PORT_GROUP_CLR_BITS | (((HI_U8)(pstPara->enPort - 1)) << 2) | u8Polar | u8LOFreq;
    u8CMD = CMD_WRITE_N0;
    FORMAT_DISEQC_CMD_VALUE(stSendMsg.au8Msg, u8Framing, DEVICE_ANY_LNB_SW_SMATV, u8CMD, &u8PortGroupBits, 1);

    s32Ret = TUNER_DISEQC_SendRecvMessage(u32TunerId, &stSendMsg, NULL/*&stRecvMsg*/);
    if (HI_SUCCESS != s32Ret)
    {
        HI_ERR_TUNER("Send WRITE N0 fail.\n"); //5021
        return s32Ret;
    }

    /* If support level 2.x, handle received message here. */

    return HI_SUCCESS;
}

static HI_S32 DISEQC_SendCmd1_1(HI_U32 u32TunerId, const HI_UNF_TUNER_DISEQC_SWITCH16PORT_S* pstPara)
{
    HI_UNF_TUNER_DISEQC_SENDMSG_S stSendMsg;
    //HI_UNF_TUNER_DISEQC_RECVMSG_S stRecvMsg;
    HI_S32 s32Ret;
    HI_U8 u8PortGroupBits;
    HI_U8 u8CMD;
    HI_U8 u8Framing;

    /* Init parameter */
    memset(&stSendMsg, 0, sizeof(stSendMsg));
    //memset(&stRecvMsg, 0, sizeof(stRecvMsg));
    stSendMsg.enLevel = pstPara->enLevel;
    //stSendMsg.enToneBurst = TUNER_DISEQC_GetToneBurstStatus(u32TunerId);
    stSendMsg.u8Length = 4;
    stSendMsg.u8RepeatTimes = 0;

    u8Framing = (pstPara->enLevel == HI_UNF_TUNER_DISEQC_LEVEL_2_X) ? MASTER_REPLY_FIRST : MASTER_NOREPLY_FIRST;

    /* Send WRITE N1(0x39) command */
    u8PortGroupBits = PORT_GROUP_CLR_BITS | (pstPara->enPort - 1);
    u8CMD = CMD_WRITE_N1;
    FORMAT_DISEQC_CMD_VALUE(stSendMsg.au8Msg, u8Framing, DEVICE_ANY_LNB_SW_SMATV, u8CMD, &u8PortGroupBits, 1);

    s32Ret = TUNER_DISEQC_SendRecvMessage(u32TunerId, &stSendMsg, NULL/*&stRecvMsg*/);
    if (HI_SUCCESS != s32Ret)
    {
        HI_ERR_TUNER("Send WRITE N1 fail.\n"); //5057
        return s32Ret;
    }

    /* If support level 2.x, handle received message here. */

    return HI_SUCCESS;
}

HI_S32 HI_DRV_TUNER_DISEQC_Switch4Port(HI_U32 u32TunerId, const HI_UNF_TUNER_DISEQC_SWITCH4PORT_S* pstPara)
{
    HI_S32 s32Ret;

    if (TUNER_NUM <= u32TunerId)
    {
        HI_ERR_TUNER("Input parameter(u32TunerId) invalid: %d\n", u32TunerId); //5072
        return HI_ERR_TUNER_INVALID_PORT;
    }

    if (HI_NULL == pstPara)
    {
        HI_ERR_TUNER("Input parameter(pstPara) invalid\n");
        return HI_ERR_TUNER_INVALID_POINT;
    }

    if (HI_UNF_TUNER_DISEQC_LEVEL_BUTT <= pstPara->enLevel)
    {
        HI_ERR_TUNER("DiSEqC level invalid: %d\n", pstPara->enLevel);
        return HI_ERR_TUNER_INVALID_PARA;
    }

    if (HI_UNF_TUNER_DISEQC_SWITCH_PORT_5 <= pstPara->enPort)
    {
        HI_ERR_TUNER("Switch port invalid: %d\n", pstPara->enPort);
        return HI_ERR_TUNER_INVALID_PARA;
    }

    /* Save port parameter */
    s_stDiSEqCStatus[u32TunerId].stPort1_0 = *pstPara;

    /* If NONE, only save. */
    if (HI_UNF_TUNER_DISEQC_SWITCH_NONE == pstPara->enPort)
    {
        return HI_SUCCESS;
    }

    /* If use 4port device, other parameter must be valid. */
    if (HI_UNF_TUNER_FE_POLARIZATION_BUTT <= pstPara->enPolar)
    {
        HI_ERR_TUNER("Polarization invalid: %d\n", pstPara->enPolar);
        return HI_ERR_TUNER_INVALID_PARA;
    }

    if (HI_UNF_TUNER_FE_LNB_22K_BUTT <= pstPara->enLNB22K)
    {
        HI_ERR_TUNER("LNB 22K invalid: %d\n", pstPara->enLNB22K);
        return HI_ERR_TUNER_INVALID_PARA;
    }

    /* Switch 1.0 */
    s32Ret = DISEQC_SendCmd1_0(u32TunerId, pstPara);
    if (HI_SUCCESS != s32Ret)
    {
        HI_ERR_TUNER("Send cmd 1.0 fail.\n"); //5120
        return s32Ret;
    }

    /*
     * If has switch 1.1, set it here.
     * Support Tuner - DiSEqC1.0 - DiSEqC1.1 - Other switch - LNB cascaded.
     */
    if (HI_UNF_TUNER_DISEQC_SWITCH_NONE != s_stDiSEqCStatus[u32TunerId].stPort1_1.enPort)
    {
        msleep(DISEQC_DELAY_BETWEEN_CMD_MS);
        s32Ret = DISEQC_SendCmd1_1(u32TunerId, &(s_stDiSEqCStatus[u32TunerId].stPort1_1));
        if (HI_SUCCESS != s32Ret)
        {
            HI_ERR_TUNER("Send cmd 1.1 fail.\n"); //5134
            return s32Ret;
        }
    }

    return HI_SUCCESS;
}

HI_S32 HI_DRV_TUNER_DISEQC_Switch16Port(HI_U32 u32TunerId, const HI_UNF_TUNER_DISEQC_SWITCH16PORT_S* pstPara)
{
    HI_S32 s32Ret;

    if (TUNER_NUM <= u32TunerId)
    {
        HI_ERR_TUNER("Input parameter(u32TunerId) invalid: %d\n", u32TunerId); //5148
        return HI_ERR_TUNER_INVALID_PORT;
    }

    if (HI_NULL == pstPara)
    {
        HI_ERR_TUNER("Input parameter(pstPara) invalid\n");
        return HI_ERR_TUNER_INVALID_POINT;
    }

    if (HI_UNF_TUNER_DISEQC_LEVEL_BUTT <= pstPara->enLevel)
    {
        HI_ERR_TUNER("DiSEqC level invalid: %d\n", pstPara->enLevel);
        return HI_ERR_TUNER_INVALID_PARA;
    }

    if (HI_UNF_TUNER_DISEQC_SWITCH_PORT_BUTT <= pstPara->enPort)
    {
        HI_ERR_TUNER("Switch port invalid: %d\n", pstPara->enPort);
        return HI_ERR_TUNER_INVALID_PARA;
    }

    /* Save port parameter */
    s_stDiSEqCStatus[u32TunerId].stPort1_1 = *pstPara;

    /* If NONE, return. */
    if (HI_UNF_TUNER_DISEQC_SWITCH_NONE == pstPara->enPort)
    {
        return HI_SUCCESS;
    }

    /* If have 1.0 switch, set if first */
    if (HI_UNF_TUNER_DISEQC_SWITCH_NONE != s_stDiSEqCStatus[u32TunerId].stPort1_0.enPort)
    {
        s32Ret = DISEQC_SendCmd1_0(u32TunerId, &(s_stDiSEqCStatus[u32TunerId].stPort1_0));
        if (HI_SUCCESS != s32Ret)
        {
            HI_ERR_TUNER("Send cmd 1.0 fail.\n"); //5185
            return s32Ret;
        }

        msleep(DISEQC_DELAY_BETWEEN_CMD_MS);
    }

    /* Switch 1.1 */
    s32Ret = DISEQC_SendCmd1_1(u32TunerId, pstPara);
    if (HI_SUCCESS != s32Ret)
    {
        HI_ERR_TUNER("Send cmd 1.1 fail.\n"); //5196
        return s32Ret;
    }

    /*
     * If have 1.0 switch, repeat.
     * Support Tuner - DiSEqC1.1 - DiSEqC1.0 - Other switch - LNB cascaded.
     */
    if (HI_UNF_TUNER_DISEQC_SWITCH_NONE != s_stDiSEqCStatus[u32TunerId].stPort1_0.enPort)
    {
        msleep(DISEQC_DELAY_BETWEEN_CMD_MS);
        s32Ret = DISEQC_SendCmd1_0(u32TunerId, &(s_stDiSEqCStatus[u32TunerId].stPort1_0));
        if (HI_SUCCESS != s32Ret)
        {
            HI_ERR_TUNER("Send cmd 1.0 fail.\n"); //5210
            return s32Ret;
        }
    }

    return HI_SUCCESS;
}

HI_S32 HI_DRV_TUNER_DISEQC_CalcAngular(HI_U32  u32tunerId , HI_VOID *p )
{
    return HI_SUCCESS;
}

HI_S32 HI_DRV_TUNER_GetDeftAttr(HI_U32  u32tunerId , HI_UNF_TUNER_ATTR_S *pstTunerAttr )
{
    return HI_SUCCESS;
}

HI_S32 HI_DRV_TUNER_SetAttr(HI_U32  u32tunerId , const HI_UNF_TUNER_ATTR_S *pstTunerAttr )
{
    HI_S32 s32Ret = 0;
    TUNER_DATA_S stTunerData = {0};
    TUNER_DATABUF_S stDataBuf = {0};
    HI_TunerAttr_S stTuner_Attr = {0};
    HI_DemodAttr_S stDemod_Attr = {0};

    if(TUNER_NUM <= u32tunerId)
    {
        HI_ERR_TUNER("Input parameter(u32tunerId) invalid,invalid tunerId is: %d\n",u32tunerId); //5896
        return HI_ERR_TUNER_INVALID_PORT;
    }

    if(HI_NULL == pstTunerAttr)
    {
        HI_ERR_TUNER("Input parameter(pstConnectPara) invalid\n");
        return HI_ERR_TUNER_INVALID_POINT;
    }

    stTunerData.u32Port = u32tunerId;
    stTunerData.u32Data = (HI_U32)pstTunerAttr->enI2cChannel;

    s32Ret = down_interruptible(&g_TunerMutex);

    s32Ret = tuner_osr_selecti2cchannel(&stTunerData);
    if (HI_SUCCESS != s32Ret)
    {
        HI_ERR_TUNER("tuner_osr_selecti2cchannel err.\n");
        up(&g_TunerMutex);
        return HI_FAILURE;
    }

    stDataBuf.u32Port = u32tunerId;
    stTuner_Attr.enTunerDevType = pstTunerAttr->enTunerDevType;
    stTuner_Attr.u32TunerAddr = pstTunerAttr->u32TunerAddr;
    stDemod_Attr.enDemodDevType = pstTunerAttr->enDemodDevType;
    stDemod_Attr.u32DemodAddr = pstTunerAttr->u32DemodAddr;
    stDataBuf.u32DataBuf[0] = (HI_U32)&stDemod_Attr;
    stDataBuf.u32DataBuf[1] = (HI_U32)&stTuner_Attr;
    stDataBuf.u32DataBuf[2] = pstTunerAttr->u32ResetGpioNo;

    s32Ret = tuner_osr_selecttype(&stDataBuf);
    if (HI_SUCCESS != s32Ret)
    {
        HI_ERR_TUNER("tuner_osr_selecttype err.\n"); //5931
        up(&g_TunerMutex);
        return HI_FAILURE;
    }

    stTunerData.u32Port = u32tunerId;
    stTunerData.u32Data = pstTunerAttr->enOutputMode;

    s32Ret = tuner_osr_settstype(&stTunerData);
    if (HI_SUCCESS != s32Ret)
    {
        HI_ERR_TUNER("tuner_osr_settstype err \n"); //5942
        up(&g_TunerMutex);
        return HI_FAILURE;
    }

    up(&g_TunerMutex);
    return HI_SUCCESS;
}

HI_S32 HI_DRV_TUNER_SetSatAttr(HI_U32   u32tunerId , const HI_UNF_TUNER_SAT_ATTR_S *pstSatTunerAttr)
{
    HI_S32 s32Ret;

    if(TUNER_NUM <= u32tunerId)
    {
        HI_ERR_TUNER("Input parameter(u32tunerId)invalid,invalid tunerId is: %d\n",u32tunerId); //5957
        return HI_ERR_TUNER_INVALID_PORT;
    }

    if(HI_NULL == pstSatTunerAttr)
    {
        HI_ERR_TUNER("Input parameter(pstTunerAttr)invalid\n");
        return HI_ERR_TUNER_INVALID_POINT;
    }

    if (HI_UNF_TUNER_RFAGC_BUTT <= pstSatTunerAttr->enRFAGC)
    {
        HI_ERR_TUNER("Input parameter(enRFAGC)invalid\n");
        return HI_ERR_TUNER_INVALID_PARA;
    }
    if (HI_UNF_TUNER_IQSPECTRUM_BUTT <= pstSatTunerAttr->enIQSpectrum)
    {
        HI_ERR_TUNER("Input parameter(enIQSpectrum)invalid\n");
        return HI_ERR_TUNER_INVALID_PARA;
    }
    if (HI_UNF_TUNER_TSCLK_POLAR_BUTT <= pstSatTunerAttr->enTSClkPolar)
    {
        HI_ERR_TUNER("Input parameter(enTSClkPolar)invalid\n");
        return HI_ERR_TUNER_INVALID_PARA;
    }
    if (HI_UNF_TUNER_TS_FORMAT_BUTT <= pstSatTunerAttr->enTSFormat)
    {
        HI_ERR_TUNER("Input parameter(enTSFormat)invalid\n");
        return HI_ERR_TUNER_INVALID_PARA;
    }
    if (HI_UNF_TUNER_TS_SERIAL_PIN_BUTT <= pstSatTunerAttr->enTSSerialPIN)
    {
        HI_ERR_TUNER("Input parameter(enTSSerialPIN)invalid\n");
        return HI_ERR_TUNER_INVALID_PARA;
    }
    if (HI_UNF_TUNER_DISEQCWAVE_BUTT <= pstSatTunerAttr->enDiSEqCWave)
    {
        HI_ERR_TUNER("Input parameter(enDiSEqCWave)invalid\n");
        return HI_ERR_TUNER_INVALID_PARA;
    }
    if (HI_UNF_LNBCTRL_DEV_TYPE_BUTT <= pstSatTunerAttr->enLNBCtrlDev)
    {
        HI_ERR_TUNER("Input parameter(enLNBCtrlDev)invalid\n");
        return HI_ERR_TUNER_INVALID_PARA;
    }

    s32Ret = down_interruptible(&g_TunerMutex);
    s32Ret = tuner_set_sat_attr(u32tunerId, (HI_UNF_TUNER_SAT_ATTR_S *)pstSatTunerAttr);
    up(&g_TunerMutex);
    if (HI_SUCCESS != s32Ret)
    {
        HI_ERR_TUNER("Tuner TUNER_SETSATATTR_CMD error\n"); //6008
        return HI_ERR_TUNER_FAILED_SETSATATTR;
    }

    return HI_SUCCESS;
}

HI_S32 HI_DRV_TUNER_SetTsOut(HI_U32    u32tunerId, HI_UNF_TUNER_TSOUT_SET_S * p)
{
	HI_S32 s32Ret;

	if (!g_stTunerOps[u32tunerId].tuner_set_ts_out)
	{
		HI_ERR_TUNER( "tuner service function ptr is NULL!\n"); //6029
		return -EFAULT;
	}

	/* tuner_set_ts_out */
	s32Ret = g_stTunerOps[u32tunerId].tuner_set_ts_out(0/*u32tunerId*/, p);
	if (HI_SUCCESS != s32Ret)
	{
		HI_ERR_TUNER( "\n tuner_get_status failed \n"); //6037
		return s32Ret;
	}

	return HI_SUCCESS;
}

HI_S32 HI_DRV_TUNER_Open (HI_U32    u32tunerId)
{
#if defined (CHIP_TYPE_hi3798cv100)
    g_u32CrgEnableFlag= 1;

    hi3796c_enable_crg();
    hi3796c_enable_adc();
#else
    tuner_enable_crg();
#endif

    if (tuner_geti2cFunc())
    {
    	HI_ERR_TUNER("tuner_geti2cFunc err\n"); //6052
    	return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 HI_DRV_TUNER_Close(HI_U32    u32tunerId)
{
    tuner_disable_crg();
    return HI_SUCCESS;
}

HI_S32 HI_DRV_TUNER_GetRs(HI_U32  u32tunerId , HI_VOID *p)
{
	if (!p)
	{
		return -14;
	}

    return HI_SUCCESS;
}

HI_S32 HI_DRV_TUNER_SetSwitchFreq(HI_U32 u32Data)
{
	Data_80fb63a4 = u32Data;

    return HI_SUCCESS;
}

HI_S32 HI_DRV_TUNER_GetSwitchFreq(HI_U32* pData)
{
	*pData = Data_80fb63a4;

    return HI_SUCCESS;
}

HI_S32 HI_DRV_TUNER_SetLoopthrough(HI_VOID)
{
    return HI_SUCCESS;
}

HI_S32 HI_DRV_TUNER_CancelLoopthrough(HI_VOID)
{
    return HI_SUCCESS;
}


#if 0
HI_S32 HI_DRV_TUNER_Connect(HI_U32  u32tunerId , const HI_UNF_TUNER_CONNECT_PARA_S  *pstConnectPara,HI_U32 u32TimeOut)
{
    TUNER_ACC_QAM_PARAMS_S tunerPara = {0};
    TUNER_SIGNAL_S tunerSignal = {0};
    TUNER_DATA_S tunerData = {0};
    HI_U32 u32TimeSpan = 0;
    HI_S32 s32Ret = 0;

    if(TUNER_NUM <= u32tunerId)
    {
        HI_ERR_TUNER("Input parameter(u32tunerId) invalid,invalid tunerId is: %d\n",u32tunerId);
        return HI_ERR_TUNER_INVALID_PORT;
    }

    if(HI_NULL == pstConnectPara)
    {
        HI_ERR_TUNER("Input parameter(pstConnectPara) invalid\n");
        return HI_ERR_TUNER_INVALID_POINT;
    }

    tunerPara.u32Frequency  = pstConnectPara->unConnectPara.stCab.u32Freq;
    tunerPara.unSRBW.u32SymbolRate = pstConnectPara->unConnectPara.stCab.u32SymbolRate;
    tunerPara.bSI = pstConnectPara->unConnectPara.stCab.bReverse;
    switch (pstConnectPara->unConnectPara.stCab.enModType)
    {
    case HI_UNF_MOD_TYPE_QAM_16:
        tunerPara.enQamType = QAM_TYPE_16;
        break;
    case HI_UNF_MOD_TYPE_QAM_32:
        tunerPara.enQamType = QAM_TYPE_32;
        break;
    case HI_UNF_MOD_TYPE_QAM_64:
    default:
        tunerPara.enQamType = QAM_TYPE_64;
        break;
    case HI_UNF_MOD_TYPE_QAM_128:
        tunerPara.enQamType = QAM_TYPE_128;
        break;
    case HI_UNF_MOD_TYPE_QAM_256:
        tunerPara.enQamType = QAM_TYPE_256;
        break;
    }

    if ((tunerPara.u32Frequency < 45000) || (tunerPara.u32Frequency > 858000))
    {
        HI_ERR_TUNER("Input parameter(pSignal.u32frequency) invalid freq = %d\n", tunerPara.u32Frequency);
        return HI_FAILURE;
    }

    if ((tunerPara.unSRBW.u32SymbolRate < 2600000) || (tunerPara.unSRBW.u32SymbolRate > 7200000))
    {
        HI_ERR_TUNER("Input parameter(pSignal.u32symbolRate = %d) invalid, \n", tunerPara.unSRBW.u32SymbolRate);
        return HI_FAILURE;
    }

    if ((0 != tunerPara.bSI) && (1 != tunerPara.bSI))
    {
        HI_ERR_TUNER("Input parameter(pSignal.u32bSI) error\n");

        //return -1;
    }

    tunerSignal.u32Port  = u32tunerId;
    tunerSignal.stSignal = tunerPara;
    s32Ret = tuner_osr_connect(&tunerSignal);
    if (HI_SUCCESS != s32Ret)
    {
        HI_ERR_TUNER("tuner_osr_connect err\n");
        return HI_FAILURE;
    }

    if (0 == u32TimeOut)
    {
        return 0;
    }

    tunerData.u32Data = 0;
    tunerData.u32Port = u32tunerId;
    while (u32TimeSpan < u32TimeOut)
    {
        s32Ret = down_interruptible(&g_TunerMutex);
        s32Ret = tuner_osr_getstatus(&tunerData);
        up(&g_TunerMutex);
        if (HI_SUCCESS != s32Ret)
        {
            HI_ERR_TUNER("tuner_ioctl TUNER_GET_STATUS_CMD err\n");
            return -1;
        }

        HI_ERR_TUNER("tuner_ioctl TUNER_GET_STATUS_CMD ok\n");
        if (HI_UNF_TUNER_SIGNAL_LOCKED == tunerData.u32Data)
        {
            return HI_SUCCESS;
        }
        else
        {
            msleep(50);
            u32TimeSpan += 50;
        }
    }

    HI_ERR_TUNER("connect time out\n");
    return HI_FAILURE;
}
#endif

#define SAT_C_MIN_KHZ (3000000)
#define SAT_C_MAX_KHZ (4200000)
#define SAT_KU_MIN_KHZ (10600000)
#define SAT_KU_MAX_KHZ (12750000)
#define SAT_DOWNLINK_FREQ_KU_MID (11700)
#define SAT_SYMBOLRATE_MAX (60000000)//(45000000)

HI_S32 HI_DRV_TUNER_SetLNBConfig( HI_U32 u32TunerId, const HI_UNF_TUNER_FE_LNB_CONFIG_S *pstLNB)
{
    HI_S32 s32Ret = 0;

    if (TUNER_NUM <= u32TunerId)
    {
        HI_ERR_TUNER("Input parameter(u32TunerId) invalid,invalid tunerId is: %d\n", u32TunerId); //6186
        return HI_ERR_TUNER_INVALID_PORT;
    }

    if (HI_NULL == pstLNB)
    {
        HI_ERR_TUNER("Input parameter(pstLNB) invalid\n");
        return HI_ERR_TUNER_INVALID_POINT;
    }

    if (HI_UNF_TUNER_FE_LNB_TYPE_BUTT <= pstLNB->enLNBType)
    {
        HI_ERR_TUNER("Input parameter(pstLNB->enLNBType) invalid\n");
        return HI_ERR_TUNER_INVALID_PARA;
    }

    if (HI_UNF_TUNER_FE_LNB_BAND_BUTT <= pstLNB->enLNBBand)
    {
        HI_ERR_TUNER("Input parameter(pstLNB->enLNBBand) invalid\n");
        return HI_ERR_TUNER_INVALID_PARA;
    }

    if (HI_UNF_TUNER_FE_LNB_SINGLE_FREQUENCY == pstLNB->enLNBType)
    {
        if (pstLNB->u32LowLO != pstLNB->u32HighLO)
        {
            HI_ERR_TUNER("Input parameter invalid\n");
            return HI_ERR_TUNER_INVALID_PARA;
        }
    }

    if (pstLNB->u32LowLO > pstLNB->u32HighLO)
    {
        HI_ERR_TUNER("Input parameter invalid, lowLO is bigger than highLO\n");
        return HI_ERR_TUNER_INVALID_PARA;
    }

    if (HI_UNF_TUNER_FE_LNB_BAND_C == pstLNB->enLNBBand)
    {
        if ((pstLNB->u32LowLO > 7500) || (pstLNB->u32HighLO > 7500))
        {
            HI_ERR_TUNER("Invalid LO freq\n"); //6227
            return HI_ERR_TUNER_INVALID_PARA;
        }
    }
    else
    {
        if ((pstLNB->u32LowLO <= 7500) || (pstLNB->u32HighLO <= 7500))
        {
            HI_ERR_TUNER("Invalid LO freq\n"); //6235
            return HI_ERR_TUNER_INVALID_PARA;
        }
    }

    s32Ret = down_interruptible(&g_TunerMutex);
    s_stMCESatPara[u32TunerId].stLNBConfig = *pstLNB;
    up(&g_TunerMutex);

    return HI_SUCCESS;
}

HI_S32 HI_DRV_TUNER_GetLNBConfig(HI_U32  u32tunerId, HI_UNF_TUNER_FE_LNB_CONFIG_S* pstLNB)
{
	if (!pstLNB)
	{
		HI_ERR_TUNER("Input parameter(pstLNB) invalid\n"); //6251

		return 0x3804a;
	}

//	*pstLNB = Data_8115c680[u32tunerId].stLNB;
	*pstLNB = s_stMCESatPara[u32tunerId].stLNBConfig;

    return HI_SUCCESS;
}

/* Convert downlink frequency to IF, calculate LNB 22K status synchronously, for connect */
/*Downlink freq dominates the course, i.e, the freq band is decided by the downlink freq*/
static HI_VOID TUNER_DownlinkFreqToIF(HI_UNF_TUNER_FE_LNB_CONFIG_S* pstLNBConfig,
                                      HI_UNF_TUNER_FE_POLARIZATION_E enPolar, HI_U32 u32DownlinkFreq,
                                      HI_U32* pu32IF, HI_UNF_TUNER_FE_LNB_22K_E *penLNB22K)
{
    /* Default */
    if (HI_NULL != penLNB22K)
    {
        *penLNB22K = HI_UNF_TUNER_FE_LNB_22K_OFF;
    }

    if ((SAT_C_MIN_KHZ <= u32DownlinkFreq)
            && (SAT_C_MAX_KHZ >= u32DownlinkFreq))
    {
        pstLNBConfig->enLNBBand = HI_UNF_TUNER_FE_LNB_BAND_C;
    }
    else if ((SAT_KU_MIN_KHZ <= u32DownlinkFreq)
            && (SAT_KU_MAX_KHZ >= u32DownlinkFreq))
    {
        pstLNBConfig->enLNBBand = HI_UNF_TUNER_FE_LNB_BAND_KU;
    }
    else
    {
        HI_ERR_TUNER("Error freq!\n"); //6283
        return;
    }

    switch (pstLNBConfig->enLNBBand)
    {
        /* C band, IF = LO - downlink frequency */
    case HI_UNF_TUNER_FE_LNB_BAND_C:
        /* Single LO */
        if ((HI_UNF_TUNER_FE_LNB_SINGLE_FREQUENCY == pstLNBConfig->enLNBType)
           || (pstLNBConfig->u32HighLO == pstLNBConfig->u32LowLO))
        {
            *pu32IF = pstLNBConfig->u32LowLO * 1000 - u32DownlinkFreq;
        }
        /* Dual LO */
        else
        {
            /* V/R polarization, use high LO */
            if ((HI_UNF_TUNER_FE_POLARIZATION_V == enPolar) || (HI_UNF_TUNER_FE_POLARIZATION_R == enPolar))
            {
                *pu32IF = pstLNBConfig->u32HighLO * 1000 - u32DownlinkFreq;
            }
            /* H/L polarization, use low LO */
            else
            {
                *pu32IF = pstLNBConfig->u32LowLO * 1000 - u32DownlinkFreq;
            }
        }

        break;

    /* Ku band, IF = downlink frequency - LO */
    case HI_UNF_TUNER_FE_LNB_BAND_KU:

        /* Single LO */
        if ((HI_UNF_TUNER_FE_LNB_SINGLE_FREQUENCY == pstLNBConfig->enLNBType)
           || (pstLNBConfig->u32HighLO == pstLNBConfig->u32LowLO))
        {
            *pu32IF = u32DownlinkFreq - pstLNBConfig->u32LowLO * 1000;
        }
        /* Dual LO */
        else
        {
            /* downlink frequency >= 11700MHz, use high LO */
            if ((u32DownlinkFreq >= SAT_DOWNLINK_FREQ_KU_MID * 1000))
            {
                *pu32IF = u32DownlinkFreq - pstLNBConfig->u32HighLO * 1000;

                /* Ku dual LO LNB use 22K select high LO */
                if (HI_NULL != penLNB22K)
                {
                    *penLNB22K = HI_UNF_TUNER_FE_LNB_22K_ON;
                }
            }
            /* downlink frequency < 11700MHz, use low LO */
            else
            {
                *pu32IF = u32DownlinkFreq - pstLNBConfig->u32LowLO * 1000;
            }
        }

        break;

    default:
        break;
    }
}

static HI_S32 TUNER_SetLNBOutAnd22K(HI_U32 u32TunerId,
                                    HI_UNF_TUNER_FE_POLARIZATION_E enPolar, HI_UNF_TUNER_FE_LNB_22K_E enLNB22K)
{
    TUNER_LNB_OUT_S stLNBOut;
    HI_S32 s32Ret;

    s32Ret = down_interruptible(&g_TunerMutex);
    switch (s_stMCESatPara[u32TunerId].enLNBPower)
    {
    /* 0V */
    case HI_UNF_TUNER_FE_LNB_POWER_OFF:
        stLNBOut.enOut = TUNER_LNB_OUT_0V;
        break;

    /* 13V/18V */
    case HI_UNF_TUNER_FE_LNB_POWER_ON:
        if ((HI_UNF_TUNER_FE_POLARIZATION_V == enPolar) || (HI_UNF_TUNER_FE_POLARIZATION_R == enPolar))
        {
            stLNBOut.enOut = TUNER_LNB_OUT_13V;
        }
        else
        {
            stLNBOut.enOut = TUNER_LNB_OUT_18V;
        }

        break;

        /* 14V/19V */
    case HI_UNF_TUNER_FE_LNB_POWER_ENHANCED:
        if ((HI_UNF_TUNER_FE_POLARIZATION_V == enPolar) || (HI_UNF_TUNER_FE_POLARIZATION_R == enPolar))
        {
            stLNBOut.enOut = TUNER_LNB_OUT_14V;
        }
        else
        {
            stLNBOut.enOut = TUNER_LNB_OUT_19V;
        }
        break;

    default:
        up(&g_TunerMutex);
        return HI_ERR_TUNER_INVALID_PARA;
    }

    stLNBOut.u32Port = u32TunerId;
    /* if tuner blind scan operation is not setted, return fault */
    if (!g_stTunerOps[u32TunerId].tuner_set_lnb_out)
    {
        HI_INFO_TUNER( "tuner service function ptr is NULL!\n"); //6399
        up(&g_TunerMutex);
        return HI_FAILURE;
    }

    s32Ret = g_stTunerOps[u32TunerId].tuner_set_lnb_out(u32TunerId, stLNBOut.enOut);
    if (HI_SUCCESS != s32Ret)
    {
        HI_INFO_TUNER( "tuner_set_lnb_out failed!\n"); //6407
        up(&g_TunerMutex);
        return s32Ret;
    }

    /* Save polarization status */
    s_stMCESatPara[u32TunerId].enPolar = enPolar;

    /* 22K signal control. If LNB power off, can't send 22K signal. */
    if (HI_UNF_TUNER_FE_LNB_POWER_OFF != s_stMCESatPara[u32TunerId].enLNBPower)
    {
        /* If has 22K switch, 22K controlled by switch.
           If hasn't 22K switch, 22K controlled by tuner lock or blind scan.
         */
        if (HI_UNF_TUNER_SWITCH_22K_NONE == s_stMCESatPara[u32TunerId].enSwitch22K)
        {
            /* if tuner blind scan operation is not setted, return fault */
            if (!g_stTunerOps[u32TunerId].tuner_send_continuous_22K)
            {
                HI_INFO_TUNER( "tuner service function ptr is NULL!\n"); //6426
                up(&g_TunerMutex);
                return HI_FAILURE;
            }

            s32Ret = g_stTunerOps[u32TunerId].tuner_send_continuous_22K(u32TunerId, enLNB22K);
            if (HI_SUCCESS != s32Ret)
            {
                HI_ERR_TUNER("Set continuous 22K fail.\n"); //6434
                up(&g_TunerMutex);
                return HI_ERR_TUNER_FAILED_LNBCTRL;
            }
        }
    }

    /* Save polarization status */
    s_stMCESatPara[u32TunerId].enLNB22K = enLNB22K;
    up(&g_TunerMutex);
    return HI_SUCCESS;
}

HI_S32 HI_DRV_TUNER_Connect(HI_U32  u32tunerId , const HI_UNF_TUNER_CONNECT_PARA_S  *pstConnectPara,HI_U32 u32TimeOut)
{
    HI_S32 s32Ret = 0;
    HI_U32 u32TimeSpan = 0;
    TUNER_ACC_QAM_PARAMS_S stSignal = {0};
    HI_UNF_TUNER_LOCK_STATUS_E enTunerStatus = 0;
    HI_UNF_TUNER_FE_LNB_22K_E enLNB22K = 0;

    if (HI_SUCCESS != tuner_check_port(u32tunerId))
    {
        HI_ERR_TUNER( "invalid tuner port: %d\n", u32tunerId); //6457
        return HI_FAILURE;
    }

    /* Cable */
    if (HI_UNF_TUNER_SIG_TYPE_CAB == pstConnectPara->enSigType)
    {
        stSignal.u32Frequency = pstConnectPara->unConnectPara.stCab.u32Freq;
        stSignal.unSRBW.u32SymbolRate = pstConnectPara->unConnectPara.stCab.u32SymbolRate;
        stSignal.bSI = pstConnectPara->unConnectPara.stCab.bReverse;

        switch(pstConnectPara->unConnectPara.stCab.enModType)
        {
            case HI_UNF_MOD_TYPE_QAM_16:
                stSignal.enQamType = QAM_TYPE_16;
                 break;

            case HI_UNF_MOD_TYPE_QAM_32:
                stSignal.enQamType = QAM_TYPE_32;
                 break;

            case HI_UNF_MOD_TYPE_QAM_64:
            case HI_UNF_MOD_TYPE_DEFAULT:
                stSignal.enQamType = QAM_TYPE_64;
                 break;

            case HI_UNF_MOD_TYPE_QAM_128:
                stSignal.enQamType = QAM_TYPE_128;
                 break;

            case HI_UNF_MOD_TYPE_QAM_256:
                stSignal.enQamType = QAM_TYPE_256;
                 break;
            default:
                HI_ERR_TUNER("Tuner HI_UNF_SET_eqamType error:%d\n",pstConnectPara->unConnectPara.stCab.enModType); //6491
                return HI_FAILURE;
        }

        /* if tuner connect operation is not setted, return fault */
        s32Ret = down_interruptible(&g_TunerMutex);
        if (!g_stTunerOps[u32tunerId].tuner_connect) /* check if tuner conect is supported */
        {
            HI_ERR_TUNER( "tuner service function ptr is NULL!\n"); //6499
            up(&g_TunerMutex);
            return HI_FAILURE;
        }

        /* call tuner connect operation */
        s32Ret = g_stTunerOps[u32tunerId].tuner_connect(u32tunerId, &stSignal);
        up(&g_TunerMutex);
        if (HI_SUCCESS != s32Ret)
        {
            HI_ERR_TUNER( "\n tuner_connect failed \n"); //6509
            return s32Ret;
        }

        while (u32TimeSpan < u32TimeOut)
        {
            s32Ret = down_interruptible(&g_TunerMutex);
            if (g_stTunerOps[u32tunerId].tuner_get_status) /* check if tuner get status is supported */
            {
                /* get tuner lock status */
                s32Ret = g_stTunerOps[u32tunerId].tuner_get_status(u32tunerId, &enTunerStatus);
                up(&g_TunerMutex);
                if (HI_SUCCESS != s32Ret)
                {
                    HI_ERR_TUNER( "\n tuner_get_status failed \n"); //6523
                    return s32Ret;
                }
            }

            if(HI_UNF_TUNER_SIGNAL_LOCKED == enTunerStatus)
            {
                return HI_SUCCESS;
            }
            else
            {
                mdelay(10);
                u32TimeSpan += 10;
            }
        }

    }
    /* Satellite */
    else if (HI_UNF_TUNER_SIG_TYPE_SAT == pstConnectPara->enSigType)
    {
        if (!((SAT_C_MIN_KHZ <= pstConnectPara->unConnectPara.stSat.u32Freq)
                && (SAT_C_MAX_KHZ >= pstConnectPara->unConnectPara.stSat.u32Freq))
            && !((SAT_KU_MIN_KHZ <= pstConnectPara->unConnectPara.stSat.u32Freq)
                && (SAT_KU_MAX_KHZ >= pstConnectPara->unConnectPara.stSat.u32Freq)))
        {
            HI_ERR_TUNER("Input parameter(u32Freq) invalid: %d\n",
                                pstConnectPara->unConnectPara.stSat.u32Freq); //6549
            return HI_ERR_TUNER_INVALID_PARA;
        }

        if (SAT_SYMBOLRATE_MAX < pstConnectPara->unConnectPara.stSat.u32SymbolRate)
        {
            HI_ERR_TUNER("Input parameter(u32SymbolRate) invalid: %d\n",
                                pstConnectPara->unConnectPara.stSat.u32SymbolRate); //6556
            return HI_ERR_TUNER_INVALID_PARA;
        }

        if (HI_UNF_TUNER_FE_POLARIZATION_BUTT <= pstConnectPara->unConnectPara.stSat.enPolar)
        {
            HI_ERR_TUNER("Input parameter(enPolar) invalid: %d\n",
                                pstConnectPara->unConnectPara.stSat.enPolar); //6563
            return HI_ERR_TUNER_INVALID_PARA;

        }

        /* Convert downlink frequency to IF */
        TUNER_DownlinkFreqToIF(&(s_stMCESatPara[u32tunerId].stLNBConfig), pstConnectPara->unConnectPara.stSat.enPolar,
                               pstConnectPara->unConnectPara.stSat.u32Freq, &(stSignal.u32Frequency), &enLNB22K);

        stSignal.unSRBW.u32SymbolRate = pstConnectPara->unConnectPara.stSat.u32SymbolRate;
        stSignal.enPolar = pstConnectPara->unConnectPara.stSat.enPolar;

         /* LNB power and 22K signal switch */
        s32Ret = TUNER_SetLNBOutAnd22K(u32tunerId, pstConnectPara->unConnectPara.stSat.enPolar, enLNB22K);
        if (HI_SUCCESS != s32Ret)
        {
            HI_ERR_TUNER("TUNER_SetLNBOutAnd22K fail.\n"); //6579
        }

        /* if tuner connect operation is not setted, return fault */
        s32Ret = down_interruptible(&g_TunerMutex);
        if (!g_stTunerOps[u32tunerId].tuner_connect) /* check if tuner conect is supported */
        {
            HI_ERR_TUNER( "tuner service function ptr is NULL!\n"); //6586
            up(&g_TunerMutex);
            return HI_FAILURE;
        }

        /* call tuner connect operation */
        s32Ret = g_stTunerOps[u32tunerId].tuner_connect(u32tunerId, &stSignal);
        up(&g_TunerMutex);
        if (HI_SUCCESS != s32Ret)
        {
            HI_ERR_TUNER( "\n tuner_connect failed \n"); //6596
            return s32Ret;
        }

        if (0 == u32TimeOut)
        {
            return HI_SUCCESS;
        }

        while (u32TimeSpan < u32TimeOut)
        {
            s32Ret = down_interruptible(&g_TunerMutex);
            if (g_stTunerOps[u32tunerId].tuner_get_status) /* check if tuner get status is supported */
            {
                /* get tuner lock status */
                s32Ret = g_stTunerOps[u32tunerId].tuner_get_status(u32tunerId, &enTunerStatus);
                up(&g_TunerMutex);
                if (HI_SUCCESS != s32Ret)
                {
                    HI_ERR_TUNER( "\n tuner_get_status failed \n"); //6615
                    return s32Ret;
                }
            }

            if (HI_UNF_TUNER_SIGNAL_LOCKED == enTunerStatus)
            {
                HI_ERR_TUNER("Tuner LOCKED!\n"); //6622
                return HI_SUCCESS;
            }
            else
            {
                mdelay(10);
                u32TimeSpan += 10;
            }
        }

        return HI_SUCCESS;
    }

    return HI_SUCCESS;
}

HI_S32 HI_DRV_TUNER_GetBer(HI_U32 u32tunerId , HI_U32*  pBerData)
{
	HI_S32 s32Ret = HI_SUCCESS;
	TUNER_DATABUF_S data;

	if (!pBerData)
	{
		return -14;
	}

	memset(&data, 0, sizeof(TUNER_DATABUF_S));

	data.u32Port = u32tunerId;

	s32Ret = tuner_osr_getber(&data);
	if (s32Ret != HI_SUCCESS)
	{
		return HI_FAILURE;
	}

	memcpy(pBerData, data.u32DataBuf, sizeof(data.u32DataBuf));

    return s32Ret;
}

HI_S32 HI_DRV_TUNER_GetSnr(HI_U32 u32tunerId , HI_U32*  pSnrData)
{
	HI_S32 s32Ret = HI_SUCCESS;
	TUNER_DATA_S data;

	if (!pSnrData)
	{
		return -14;
	}

	memset(&data, 0, sizeof(TUNER_DATA_S));

	data.u32Port = u32tunerId;

	s32Ret = tuner_osr_getsnr(&data);
	if (s32Ret != HI_SUCCESS)
	{
		return HI_FAILURE;
	}

	*pSnrData = data.u32Data;

    return s32Ret;
}

HI_S32 HI_DRV_TUNER_GetRssi(HI_U32 u32tunerId , HI_U32*  pRssiData)
{
	HI_S32 s32Ret = HI_SUCCESS;
	TUNER_DATABUF_S data;

	if (!pRssiData)
	{
		return -14;
	}

	memset(&data, 0, sizeof(TUNER_DATABUF_S));

	data.u32Port = u32tunerId;

	s32Ret = tuner_osr_getsignalstrength(&data);

	memcpy(pRssiData, data.u32DataBuf, sizeof(data.u32DataBuf));

    return s32Ret;
}

HI_S32 HI_DRV_TUNER_GetFreqSymbOffset(HI_U32 u32tunerId , HI_U32*  pFreqSymbOffsetData)
{
	HI_S32 s32Ret = HI_SUCCESS;
	TUNER_DATABUF_S data;

	if (!pFreqSymbOffsetData)
	{
		return -14;
	}

	memset(&data, 0, sizeof(TUNER_DATABUF_S));

	data.u32Port = u32tunerId;

	s32Ret = tuner_get_freq_symb_offset(&data);

	memcpy(pFreqSymbOffsetData, data.u32DataBuf, sizeof(data.u32DataBuf));

    return s32Ret;
}

HI_S32 HI_DRV_TUNER_GetStatus(HI_U32    u32tunerId , HI_UNF_TUNER_STATUS_S  *pstTunerStatus)
{
	HI_S32 s32Ret = HI_SUCCESS;
	TUNER_DATA_S data;

	if (!pstTunerStatus)
	{
		return -14;
	}

	memset(&data, 0, sizeof(TUNER_DATA_S));

	data.u32Port = u32tunerId;

	s32Ret = tuner_osr_getstatus(&data);
	if (s32Ret != HI_SUCCESS)
	{
		return HI_FAILURE;
	}

	pstTunerStatus->enLockStatus = (HI_UNF_TUNER_LOCK_STATUS_E)(data.u32Data);

    return s32Ret;
}

#ifndef MODULE
EXPORT_SYMBOL(hi_tuner_open);
EXPORT_SYMBOL(hi_tuner_ioctl);
EXPORT_SYMBOL(hi_tuner_release);
EXPORT_SYMBOL(tuner_suspend);
EXPORT_SYMBOL(tuner_resume);
EXPORT_SYMBOL(tuner_proc_read);
EXPORT_SYMBOL(tuner_proc_read_reg);
#else
/*not as parameter in the kernel temporarily*/
module_param_array(g_xtal_clk_table,uint,NULL,S_IRUGO);
module_param_array(g_demod_address,uint,NULL,S_IRUGO);
module_param_array(g_halftuner_address,uint,NULL,S_IRUGO);
#endif

EXPORT_SYMBOL(HI_DRV_TUNER_Init);
EXPORT_SYMBOL(HI_DRV_TUNER_DeInit);

EXPORT_SYMBOL(HI_DRV_TUNER_GetDeftAttr);
EXPORT_SYMBOL(HI_DRV_TUNER_GetRs);
EXPORT_SYMBOL(HI_DRV_TUNER_SetSwitchFreq);
EXPORT_SYMBOL(HI_DRV_TUNER_GetSwitchFreq);
EXPORT_SYMBOL(HI_DRV_TUNER_SetLoopthrough);
EXPORT_SYMBOL(HI_DRV_TUNER_CancelLoopthrough);
EXPORT_SYMBOL(HI_DRV_TUNER_GetLNBConfig);
EXPORT_SYMBOL(HI_DRV_TUNER_DISEQC_CalcAngular);
EXPORT_SYMBOL(HI_DRV_TUNER_SetAttr);
EXPORT_SYMBOL(HI_DRV_TUNER_SetSatAttr);

EXPORT_SYMBOL(HI_DRV_TUNER_Open);
EXPORT_SYMBOL(HI_DRV_TUNER_Close);

EXPORT_SYMBOL(HI_DRV_TUNER_Connect);

EXPORT_SYMBOL(HI_DRV_TUNER_GetBer);
EXPORT_SYMBOL(HI_DRV_TUNER_GetSnr);
EXPORT_SYMBOL(HI_DRV_TUNER_GetRssi);
EXPORT_SYMBOL(HI_DRV_TUNER_GetFreqSymbOffset);

EXPORT_SYMBOL(HI_DRV_TUNER_GetStatus);
#if defined (CHIP_TYPE_hi3716cv200es)||defined (CHIP_TYPE_hi3716cv200)
EXPORT_SYMBOL(qam_ram_collect_data);
#endif

EXPORT_SYMBOL(HI_DRV_TUNER_SetLNBConfig);
EXPORT_SYMBOL(HI_DRV_TUNER_SetLNBPower);
EXPORT_SYMBOL(HI_DRV_TUNER_Switch22K);
EXPORT_SYMBOL(HI_DRV_TUNER_DISEQC_Switch4Port);
EXPORT_SYMBOL(HI_DRV_TUNER_DISEQC_Switch16Port);

