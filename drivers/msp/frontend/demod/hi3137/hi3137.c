
#include <linux/delay.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <linux/time.h>
#include <linux/module.h>
#include <asm/io.h>
#include <linux/kthread.h>
#include <linux/pm.h>

#include "hi_type.h"
#include "hi_drv_i2c.h"
#include "hi_drv_gpio.h"
#include "hi_debug.h"
#include "hi_error_mpi.h"
//#include " hi_unf_ecs_type.h"
#include "hi_drv_proc.h"
#include "hi_drv_sys.h"
#include "drv_demod.h"


HI_S32 hi3137_init(HI_U32 u32TunerPort, HI_U8 enI2cChannel, HI_UNF_TUNER_DEV_TYPE_E enTunerDevType)
{

}

HI_S32 hi3137_connect(HI_U32 u32TunerPort, TUNER_ACC_QAM_PARAMS_S *pstChannel)
{
}

HI_S32 hi3137_get_status (HI_U32 u32TunerPort, HI_UNF_TUNER_LOCK_STATUS_E  *penTunerStatus)
{
}

HI_S32 hi3137_get_ber(HI_U32 u32TunerPort, HI_U32 *pu32ber)
{
}

HI_S32 hi3137_get_snr(HI_U32 u32TunerPort, HI_U32* pu32SNR)
{
}

HI_S32 hi3137_get_signal_strength(HI_U32 u32TunerPort, HI_U32 *pu32SignalStrength)
{
}

HI_S32 hi3137_set_ts_type(HI_U32 u32TunerPort, HI_UNF_TUNER_OUPUT_MODE_E enTsType)
{
}

HI_S32 hi3137_get_freq_symb_offset(HI_U32 u32TunerPort, HI_U32 * pu32Freq, HI_U32 * pu32Symb )
{
}

HI_S32 hi3137_get_signal_info(HI_U32 u32TunerPort, HI_UNF_TUNER_SIGNALINFO_S *pstInfo)
{

}

HI_S32 hi3137_setplpid(HI_U32 u32TunerPort, HI_U8 u8PLPID)
{

}

HI_S32 hi3137_get_plp_num(HI_U32 u32TunerPort, HI_U8 *pu8PLPNum)
{

}

HI_S32 hi3137_get_current_plp_type(HI_U32 u32TunerPort, HI_UNF_TUNER_T2_PLP_TYPE_E *penPLPType)
{

}

HI_VOID hi3137_connect_timeout(HI_U32 u32ConnectTimeout)
{

}

HI_S32 hi3137_set_ts_out(HI_U32 u32TunerPort, HI_UNF_TUNER_TSOUT_SET_S *pstTSOut)
{

}

HI_S32 hi3137_standby(HI_U32 u32TunerPort, HI_U32 u32Standby)
{

}

HI_S32 hi3137_get_plp_id(HI_U32 u32TunerPort)
{

}

HI_S32 hi3137_get_plp_group_id(HI_U32 u32TunerPort)
{

}

HI_S32 hi3137_tp_scan(HI_U32 u32TunerPort)
{

}

HI_S32 hi3137_set_common_plp_id(HI_U32 u32TunerPort)
{

}

HI_S32 hi3137_set_common_plp_combination(HI_U32 u32TunerPort)
{

}

HI_S32 hi3137_set_ter_attr(HI_U32 u32TunerPort)
{

}

HI_S32 hi3137_set_antena_power(HI_U32 u32TunerPort)
{

}


