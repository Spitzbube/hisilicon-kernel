#include <linux/module.h>
#include "hi_debug.h"
#include "hi_type.h"
#include "hi_unf_i2c.h"
#include "drv_tuner_ext.h"
#include "drv_demod.h"
#include "MN_DMD_driver.h"
#include "MN_DMD_device.h"
#include "MN88472.h"
#include "MN_I2C.h"
#include <mach/platform.h>

#define MAX_TUNER (3)
extern DMD_PARAMETER_t g_stMN88472Demod[];

HI_S32 MN88472_Init(HI_U32 u32TunerPort, HI_U8 enI2cChannel, HI_UNF_TUNER_DEV_TYPE_E enTunerDevType)
{
    DMD_PARAMETER_t * pstParaMeter;
    HI_U32 i;

    /* Check tuner port and init. */
    if (u32TunerPort >= MAX_TUNER)
    {
        HI_ERR_TUNER("Bad u32TunerPort\n");
        return HI_FAILURE;
    }

    pstParaMeter = g_stMN88472Demod + u32TunerPort;
    pstParaMeter->userdata = g_stTunerOps + u32TunerPort;

    pstParaMeter->devid = u32TunerPort;
  
    /* Initialize Parameter Variable */
    for(i=0;i<DMD_INFORMATION_MAX;i++)
    {
        pstParaMeter->info[i] = 0;		//NOT_DEFINED
    }
  
    pstParaMeter->funit = DMD_E_KHZ;


    if( DMD_E_OK != DMD_init(pstParaMeter))
    {
        return HI_FAILURE;
    }
    return HI_SUCCESS;
    
}

HI_S32 MN88472_Connect(HI_U32 u32TunerPort,TUNER_ACC_QAM_PARAMS_S *pstChannel)
{
    DMD_PARAMETER_t * pstParaMeter;
    HI_U8 enI2cChan;
    DMD_ERROR_t enRet;
    DMD_u32_t     i2caddr;

    HI_TUNER_CHECKPOINTER( pstChannel);
    /* Check tuner port and init. */
    if (u32TunerPort >= MAX_TUNER)
    {
        HI_ERR_TUNER("Bad u32TunerPort\n");
        return HI_FAILURE;
    }

    pstParaMeter = g_stMN88472Demod + u32TunerPort;
    enI2cChan = g_stTunerOps[u32TunerPort].enI2cChannel;
    i2caddr = g_stTunerOps[u32TunerPort].u32DemodAddress;

    pstParaMeter->freq = pstChannel->u32Frequency;

    switch(g_stTunerOps[u32TunerPort].enSigType)
    {
    case HI_UNF_TUNER_SIG_TYPE_CAB:
        pstParaMeter->system = DMD_E_DVBC;
        break;
    case HI_UNF_TUNER_SIG_TYPE_DVB_T:
        pstParaMeter->system = DMD_E_DVBT;
        break;
    case HI_UNF_TUNER_SIG_TYPE_DVB_T2:
        pstParaMeter->system = DMD_E_DVBT2;
        break;
    default:
    	HI_ERR_TUNER("Signal type not supported!\n");
       return HI_FAILURE;
    }

    if (HI_UNF_TUNER_SIG_TYPE_CAB != g_stTunerOps[u32TunerPort].enSigType)
    {
        switch(pstChannel->unSRBW.u32BandWidth)
        {
            case 5000:
                pstParaMeter->bw = DMD_E_BW_5MHZ;
                break;
            case 6000:
                pstParaMeter->bw = DMD_E_BW_6MHZ;
                break;
            case 7000:
                pstParaMeter->bw = DMD_E_BW_7MHZ;
                break;
            case 8000:
                pstParaMeter->bw = DMD_E_BW_8MHZ;
                break;
            case 1700:
                pstParaMeter->bw = DMD_E_BW_1_7MHZ;
                break;
            default:
                return HI_FAILURE;
               
        }
    }    

#if 1
    enRet =  g_stTunerOps[u32TunerPort].set_tuner(u32TunerPort, enI2cChan, pstParaMeter->freq);
    if (HI_SUCCESS != enRet)
    {
        HI_ERR_TUNER("MN88472 set tuner not ok\n");
        return enRet;
    }
    HI_ERR_TUNER("MN88472 set tuner ok\n");
#endif

    /*set ts interface to parallel or serial*/
    if(DMD_TS_PARALLEL == pstParaMeter->tstype)
    {
        if (DMD_E_ERROR == DMD_I2C_Write(enI2cChan, DMD_BANK_1st + i2caddr, DMD_TSSET1, 0))
        {
            return HI_FAILURE;
        }

        if (DMD_E_ERROR == DMD_I2C_Write(enI2cChan, DMD_BANK_2nd + i2caddr, DMD_FIFOSET, 0xE3))
        {
            return HI_FAILURE;
        }
        
    }
    else
    {
        if (DMD_E_ERROR == DMD_I2C_Write(enI2cChan, DMD_BANK_1st + i2caddr, DMD_TSSET1, 1))
        {
            return HI_FAILURE;
        }

        if (DMD_E_ERROR == DMD_I2C_Write(enI2cChan, DMD_BANK_2nd + i2caddr, DMD_FIFOSET, 0xE3))
        {
            return HI_FAILURE;
        }
    }
	
    enRet = DMD_set_system(pstParaMeter);
    if (DMD_E_OK != enRet)
    {
        return HI_FAILURE;
    }
    
    DMD_device_post_tune(pstParaMeter);
   
    return HI_SUCCESS;
}

HI_VOID MN88472_connect_timeout(HI_U32 u32ConnectTimeout)
{
    return;
}
HI_S32 MN88472_GetStatus (HI_U32 u32TunerPort, HI_UNF_TUNER_LOCK_STATUS_E  *penTunerStatus)
{
    DMD_PARAMETER_t * pstParaMeter;
    //DMD_ERROR_t enRet;

    HI_TUNER_CHECKPOINTER( penTunerStatus);
    /* Check tuner port and init. */
    if (u32TunerPort >= MAX_TUNER)
    {
        HI_ERR_TUNER("Bad u32TunerPort\n");
        return HI_FAILURE;
    }

    pstParaMeter = g_stMN88472Demod + u32TunerPort;

    *penTunerStatus = HI_UNF_TUNER_SIGNAL_DROPPED;
    DMD_get_info( pstParaMeter , DMD_E_INFO_LOCK);
    if( pstParaMeter->info[DMD_E_INFO_LOCK] == DMD_E_LOCKED )
    {
        *penTunerStatus = HI_UNF_TUNER_SIGNAL_LOCKED;
    }
    return HI_SUCCESS;
}

HI_S32 MN88472_SetTsType(HI_U32 u32TunerPort, HI_UNF_TUNER_OUPUT_MODE_E enTsType)
{
    DMD_PARAMETER_t * pstParaMeter;
    
    /* Check tuner port and init. */
    if (u32TunerPort >= MAX_TUNER)
    {
        HI_ERR_TUNER("Bad u32TunerPort\n");
        return HI_FAILURE;
    }
    
    pstParaMeter = g_stMN88472Demod + u32TunerPort;
    g_stTunerOps[u32TunerPort].enTsType = enTsType;
    HI_INFO_TUNER("The stream type of UNF Layer is %d.\n",enTsType);

    switch(enTsType)
    {
        case HI_UNF_TUNER_OUTPUT_MODE_PARALLEL_MODE_A:
        case HI_UNF_TUNER_OUTPUT_MODE_PARALLEL_MODE_B:
        case HI_UNF_TUNER_OUTPUT_MODE_DEFAULT:
            pstParaMeter->tstype = DMD_TS_PARALLEL;
            break;
        case HI_UNF_TUNER_OUTPUT_MODE_SERIAL:
            pstParaMeter->tstype = DMD_TS_SERIAL;
            break;
        default:
            return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/*获取误码率，pu32ber处存放错误数，pu32ber+1处存放收到数据总数*/
HI_S32 MN88472_GetBer(HI_U32 u32TunerPort, HI_U32 *pu32ber)
{
    DMD_PARAMETER_t * pstParaMeter;

    HI_TUNER_CHECKPOINTER( pu32ber);
    /* Check tuner port and init. */
    if (u32TunerPort >= MAX_TUNER)
    {
        HI_ERR_TUNER("Bad u32TunerPort\n");
        return HI_FAILURE;
    }

    pstParaMeter = g_stMN88472Demod + u32TunerPort;
    if (DMD_E_OK != DMD_BER(pstParaMeter, pu32ber, pu32ber + 1))
    {
        return HI_FAILURE;
    }

    if (pu32ber[1])
    {
        pu32ber[0] = pu32ber[0] * 10000000 / pu32ber[1];
        pu32ber[1] = 0;
        pu32ber[2] = 7;
    }
    else
    {
        pu32ber[0] = 0;
        pu32ber[1] = 0;
        pu32ber[2] = 0;
    }
    
    return HI_SUCCESS;
}

/*获取信噪比，结果已经转换为dB*/
HI_S32 MN88472_GetSnr(HI_U32 u32TunerPort, HI_U32* pu32SNR)
{
    DMD_PARAMETER_t * pstParaMeter;
    HI_U32 ulCntInt = 0;
    HI_U32 ulCntDec = 0;

    HI_TUNER_CHECKPOINTER( pu32SNR);
    /* Check tuner port and init. */
    if (u32TunerPort >= MAX_TUNER)
    {
        HI_ERR_TUNER("Bad u32TunerPort\n");
        return HI_FAILURE;
    }
    
    pstParaMeter = g_stMN88472Demod + u32TunerPort;
    
    if (DMD_E_OK != DMD_CNR(pstParaMeter, &ulCntInt, &ulCntDec))
    {
       return HI_FAILURE;
    }
    /*只取整数部分*/
    *pu32SNR = ulCntInt;
    return HI_SUCCESS;
}

HI_S32 MN88472_GetSignalStrength(HI_U32 u32TunerPort, HI_U32 *pu32SignalStrength)
{
    DMD_PARAMETER_t * pstParaMeter;

    HI_TUNER_CHECKPOINTER( pu32SignalStrength);
    /* Check tuner port and init. */
    if (u32TunerPort >= MAX_TUNER)
    {
        HI_ERR_TUNER("Bad u32TunerPort\n");
        return HI_FAILURE;
    }

    pstParaMeter = g_stMN88472Demod + u32TunerPort;
    pu32SignalStrength[1] = DMD_AGC(pstParaMeter);
    return HI_SUCCESS;
}

HI_S32 MN88472_get_signal_info(HI_U32 u32TunerPort, HI_UNF_TUNER_SIGNALINFO_S *pstInfo)
{
    DMD_PARAMETER_t* pstParaMeter;
    //HI_U32 i = 0;

    HI_TUNER_CHECKPOINTER( pstInfo);
    /* Check tuner port and init. */
    if (u32TunerPort >= MAX_TUNER)
    {
        HI_ERR_TUNER("Bad u32TunerPort\n");
        return HI_FAILURE;
    }
    
    pstParaMeter = g_stMN88472Demod + u32TunerPort;
    pstInfo->enSigType = g_stTunerOps[u32TunerPort].enSigType;
        
    if (HI_UNF_TUNER_SIG_TYPE_DVB_T2 == pstInfo->enSigType)
    {
        /*get the constellation*/
        DMD_get_info(pstParaMeter, DMD_E_INFO_DVBT2_DAT_PLP_MOD);
        switch (pstParaMeter->info[DMD_E_INFO_DVBT2_DAT_PLP_MOD])
        {
        case 0:
	     pstInfo->unSignalInfo.stTer.enModType = HI_UNF_MOD_TYPE_QPSK;
            break;
        case 1:
	     pstInfo->unSignalInfo.stTer.enModType = HI_UNF_MOD_TYPE_QAM_16;
            break;
        case 2:
	     pstInfo->unSignalInfo.stTer.enModType = HI_UNF_MOD_TYPE_QAM_64;
            break;
        case 3:
	     pstInfo->unSignalInfo.stTer.enModType = HI_UNF_MOD_TYPE_QAM_256;
            break;
        default:
            return HI_FAILURE;	
        }
        
        /*get fft size*/
        DMD_get_info(pstParaMeter, DMD_E_INFO_DVBT2_MODE);
        switch (pstParaMeter->info[DMD_E_INFO_DVBT2_MODE])
        {
        case 0:
	     pstInfo->unSignalInfo.stTer.enFFTMode = HI_UNF_TUNER_FE_FFT_1K;
            break;
        case 1:
	     pstInfo->unSignalInfo.stTer.enFFTMode = HI_UNF_TUNER_FE_FFT_2K;
            break;
        case 2:
	     pstInfo->unSignalInfo.stTer.enFFTMode = HI_UNF_TUNER_FE_FFT_4K;
            break;
        case 3:
	     pstInfo->unSignalInfo.stTer.enFFTMode = HI_UNF_TUNER_FE_FFT_8K;
            break;
        case 4:
	     pstInfo->unSignalInfo.stTer.enFFTMode = HI_UNF_TUNER_FE_FFT_16K;
            break;
        case 5:
	     pstInfo->unSignalInfo.stTer.enFFTMode = HI_UNF_TUNER_FE_FFT_32K;
            break;
        default:
            return HI_FAILURE;	
        }

        /*get guard interval*/
        DMD_get_info(pstParaMeter, DMD_E_INFO_DVBT2_GI);
        switch (pstParaMeter->info[DMD_E_INFO_DVBT2_GI])
        {
        case 0:
	     pstInfo->unSignalInfo.stTer.enGuardIntv = HI_UNF_TUNER_FE_GUARD_INTV_1_32;
            break;
        case 1:
	     pstInfo->unSignalInfo.stTer.enGuardIntv = HI_UNF_TUNER_FE_GUARD_INTV_1_16;
            break;
        case 2:
	     pstInfo->unSignalInfo.stTer.enGuardIntv = HI_UNF_TUNER_FE_GUARD_INTV_1_8;
            break;
        case 3:
	     pstInfo->unSignalInfo.stTer.enGuardIntv = HI_UNF_TUNER_FE_GUARD_INTV_1_4;
            break;
        case 4:
	     pstInfo->unSignalInfo.stTer.enGuardIntv = HI_UNF_TUNER_FE_GUARD_INTV_1_32;
            break;
        case 5:
	     pstInfo->unSignalInfo.stTer.enGuardIntv = HI_UNF_TUNER_FE_GUARD_INTV_1_128;
            break;
        case 6:
	     pstInfo->unSignalInfo.stTer.enGuardIntv = HI_UNF_TUNER_FE_GUARD_INTV_19_128;
            break;
        case 7:
	     pstInfo->unSignalInfo.stTer.enGuardIntv = HI_UNF_TUNER_FE_GUARD_INTV_19_256;
            break;
        default:
            return HI_FAILURE;	
        }

        /*get fec*/
        DMD_get_info(pstParaMeter, DMD_E_INFO_DVBT2_L1_COD);
        switch (pstParaMeter->info[DMD_E_INFO_DVBT2_L1_COD])
        {
        case 0:
	     pstInfo->unSignalInfo.stTer.enFECRate = HI_UNF_TUNER_FE_FEC_1_2;
            break;
        case 1:
	     pstInfo->unSignalInfo.stTer.enFECRate = HI_UNF_TUNER_FE_FEC_3_5;
            break;
        case 2:
	     pstInfo->unSignalInfo.stTer.enFECRate = HI_UNF_TUNER_FE_FEC_2_3;
            break;
        case 3:
	     pstInfo->unSignalInfo.stTer.enFECRate = HI_UNF_TUNER_FE_FEC_3_4;
            break;
        case 4:
	     pstInfo->unSignalInfo.stTer.enFECRate = HI_UNF_TUNER_FE_FEC_4_5;
            break;
        case 5:
	     pstInfo->unSignalInfo.stTer.enFECRate = HI_UNF_TUNER_FE_FEC_5_6;
            break;
        default:
            return HI_FAILURE;	
        }        
    }
    else if (HI_UNF_TUNER_SIG_TYPE_DVB_T == pstInfo->enSigType)
    {
        /*get the constellation*/
        DMD_get_info(pstParaMeter, DMD_E_INFO_DVBT_CONSTELLATION);
        switch (pstParaMeter->info[DMD_E_INFO_DVBT_CONSTELLATION])
        {
        case 0:
	     pstInfo->unSignalInfo.stTer.enModType = HI_UNF_MOD_TYPE_QPSK;
            break;
        case 1:
	     pstInfo->unSignalInfo.stTer.enModType = HI_UNF_MOD_TYPE_QAM_16;
            break;
        case 2:
	     pstInfo->unSignalInfo.stTer.enModType = HI_UNF_MOD_TYPE_QAM_64;
            break;
        default:
            return HI_FAILURE;	
        }
        
        /*get fft size*/
        DMD_get_info(pstParaMeter, DMD_E_INFO_DVBT_MODE);
        switch (pstParaMeter->info[DMD_E_INFO_DVBT_MODE])
        {
         case 0:
 	    pstInfo->unSignalInfo.stTer.enFFTMode = HI_UNF_TUNER_FE_FFT_2K;
             break;
         case 1:
 	    pstInfo->unSignalInfo.stTer.enFFTMode = HI_UNF_TUNER_FE_FFT_8K;
             break;
         default:
             return HI_FAILURE;	
        }

        /*get guard interval*/
        DMD_get_info(pstParaMeter, DMD_E_INFO_DVBT_GI);
        switch (pstParaMeter->info[DMD_E_INFO_DVBT_GI])
        {
        case 0:
            pstInfo->unSignalInfo.stTer.enGuardIntv = HI_UNF_TUNER_FE_GUARD_INTV_1_32;
            break;
        case 1:
            pstInfo->unSignalInfo.stTer.enGuardIntv = HI_UNF_TUNER_FE_GUARD_INTV_1_16;
            break;
        case 2:
            pstInfo->unSignalInfo.stTer.enGuardIntv = HI_UNF_TUNER_FE_GUARD_INTV_1_8;
            break;
        case 3:
            pstInfo->unSignalInfo.stTer.enGuardIntv = HI_UNF_TUNER_FE_GUARD_INTV_1_4;
            break;
        default:
            return HI_FAILURE;	
        }

        /*get priority*/    
        DMD_get_info(pstParaMeter, DMD_E_INFO_DVBT_HIERARCHY_SELECT);
        switch (pstParaMeter->info[DMD_E_INFO_DVBT_HIERARCHY_SELECT])
        {
            case 0:
                pstInfo->unSignalInfo.stTer.enTsPriority = HI_UNF_TUNER_TS_PRIORITY_LP;
                break;
            case 1:
                pstInfo->unSignalInfo.stTer.enTsPriority = HI_UNF_TUNER_TS_PRIORITY_HP;
                break;
            default:
                return HI_FAILURE;	
        }
       
        /*get alpha*/
        DMD_get_info(pstParaMeter, DMD_E_INFO_DVBT_HIERARCHY);
        switch (pstParaMeter->info[DMD_E_INFO_DVBT_HIERARCHY])
        {
        case 0:
            pstInfo->unSignalInfo.stTer.enTsPriority = HI_UNF_TUNER_TS_PRIORITY_NONE;
            pstInfo->unSignalInfo.stTer.enHierMod = HI_UNF_TUNER_FE_HIERARCHY_NO;
            break;
        case 1:
            pstInfo->unSignalInfo.stTer.enHierMod = HI_UNF_TUNER_FE_HIERARCHY_ALHPA1;
            break;
        case 2:
            pstInfo->unSignalInfo.stTer.enHierMod = HI_UNF_TUNER_FE_HIERARCHY_ALHPA2;
            break;
        case 3:
            pstInfo->unSignalInfo.stTer.enHierMod = HI_UNF_TUNER_FE_HIERARCHY_ALHPA4;
            break;
        default:
            return HI_FAILURE;	
        }
       
        /*get fec*/
        if (HI_UNF_TUNER_TS_PRIORITY_HP == pstInfo->unSignalInfo.stTer.enTsPriority)
        {
            DMD_get_info(pstParaMeter, DMD_E_INFO_DVBT_HP_CODERATE);

            switch (pstParaMeter->info[DMD_E_INFO_DVBT_HP_CODERATE])
            {
            case 0:
                pstInfo->unSignalInfo.stTer.enFECRate = HI_UNF_TUNER_FE_FEC_1_2;
                break;
            case 1:
                pstInfo->unSignalInfo.stTer.enFECRate = HI_UNF_TUNER_FE_FEC_2_3;
                break;
            case 2:
                pstInfo->unSignalInfo.stTer.enFECRate = HI_UNF_TUNER_FE_FEC_3_4;
                break;
            case 3:
                pstInfo->unSignalInfo.stTer.enFECRate = HI_UNF_TUNER_FE_FEC_5_6;
                break;
            case 4:
                pstInfo->unSignalInfo.stTer.enFECRate = HI_UNF_TUNER_FE_FEC_7_8;
                break;
            default:
                return HI_FAILURE;	
            }
        }
        else
        {
            DMD_get_info(pstParaMeter, DMD_E_INFO_DVBT_LP_CODERATE);

            switch (pstParaMeter->info[DMD_E_INFO_DVBT_LP_CODERATE])
            {
            case 0:
                pstInfo->unSignalInfo.stTer.enFECRate = HI_UNF_TUNER_FE_FEC_1_2;
                break;
            case 1:
                pstInfo->unSignalInfo.stTer.enFECRate = HI_UNF_TUNER_FE_FEC_2_3;
                break;
            case 2:
                pstInfo->unSignalInfo.stTer.enFECRate = HI_UNF_TUNER_FE_FEC_3_4;
                break;
            case 3:
                pstInfo->unSignalInfo.stTer.enFECRate = HI_UNF_TUNER_FE_FEC_5_6;
                break;
            case 4:
                pstInfo->unSignalInfo.stTer.enFECRate = HI_UNF_TUNER_FE_FEC_7_8;
                break;
            default:
                return HI_FAILURE;	
            }
        }
    }
    else
    {
        return HI_FAILURE;	
    }
    
    return HI_SUCCESS;
}

/*only used in DVB-T2*/
HI_S32 MN88472_get_plp_num(HI_U32 u32TunerPort, HI_U8 *pu8PLPNum)
{
    DMD_PARAMETER_t* pstParaMeter;

    HI_TUNER_CHECKPOINTER( pu8PLPNum);
    /* Check tuner port and init. */
    if (u32TunerPort >= MAX_TUNER)
    {
        HI_ERR_TUNER("Bad u32TunerPort\n");
        return HI_FAILURE;
    }

    pstParaMeter = g_stMN88472Demod + u32TunerPort;

    if (HI_UNF_TUNER_SIG_TYPE_DVB_T2 == g_stTunerOps[u32TunerPort].enSigType)
    {
        /*get the number of PLPs*/
        DMD_get_info( pstParaMeter, DMD_E_INFO_DVBT2_NUM_PLP );
        *pu8PLPNum = pstParaMeter->info[DMD_E_INFO_DVBT2_NUM_PLP];
        return HI_SUCCESS;
    }

    return HI_FAILURE;
}

/*only used in DVB-T2*/
HI_S32 MN88472_get_current_plp_type(HI_U32 u32TunerPort, HI_UNF_TUNER_T2_PLP_TYPE_E *penPLPType)
{
    DMD_PARAMETER_t* pstParaMeter;

    HI_TUNER_CHECKPOINTER( penPLPType);
    /* Check tuner port and init. */
    if (u32TunerPort >= MAX_TUNER)
    {
        HI_ERR_TUNER("Bad u32TunerPort\n");
        return HI_FAILURE;
    }

    pstParaMeter = g_stMN88472Demod + u32TunerPort;
    if (HI_UNF_TUNER_SIG_TYPE_DVB_T2 == g_stTunerOps[u32TunerPort].enSigType)
    {
        DMD_wait(300);
        DMD_get_info(pstParaMeter, DMD_E_INFO_DVBT2_DAT_PLP_TYPE);
        *penPLPType = pstParaMeter->info[DMD_E_INFO_DVBT2_DAT_PLP_TYPE];
        return HI_SUCCESS;
    }

    return HI_FAILURE;
}

/*only used in DVB-T2*/
HI_S32 MN88472_setplpid(HI_U32 u32TunerPort, HI_U8 u8PLPID)
{
    DMD_PARAMETER_t* pstParaMeter;

    /* Check tuner port. */
    if (u32TunerPort >= MAX_TUNER)
    {
        HI_ERR_TUNER("Bad u32TunerPort\n");
        return HI_FAILURE;
    }

    pstParaMeter = g_stMN88472Demod + u32TunerPort;
    
    DMD_set_info( pstParaMeter , DMD_E_INFO_DVBT2_SELECTED_PLP , u8PLPID & 0xFF );
}

HI_S32 MN88472_get_freq_symb_offset(HI_U32 u32TunerPort, HI_U32 * pu32Freq, HI_U32 * pu32Symb)
{
    /*guarantee the pointer is not NULL*/
    HI_TUNER_CHECKPOINTER( pu32Freq);
    HI_TUNER_CHECKPOINTER( pu32Symb);

    /* Check tuner port and init. */
    if (u32TunerPort >= MAX_TUNER)
    {
        HI_ERR_TUNER("Bad u32TunerPort\n");
        return HI_FAILURE;
    }
    
    *pu32Freq = 0;
    *pu32Symb = 0;
    
    return HI_SUCCESS;
}
#if 0
HI_VOID MN88472_TunerResume(HI_VOID)
{

    DMD_PARAMETER_t * pstParaMeter;

    /*本来应该要根据u32TunerPort来选择哪个tuner的，但是目前这个接口无法知道u32TunerPort,
    目前的应用暂时只有一个tuner，就先用0,后续此接口要考虑多tuner的情况*/
    pstParaMeter = g_stMN88472Demod;
    *((volatile unsigned int *)(IO_ADDRESS(0x10203160))) = 0;
    DMD_init(pstParaMeter);
    DMD_device_set_sig_type(pstParaMeter);
    return;
}
#endif

