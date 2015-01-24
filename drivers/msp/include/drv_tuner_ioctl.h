#ifndef __DRV_TUNER_IOCTL_H__
#define __DRV_TUNER_IOCTL_H__

/*#include <linux/module.h>
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
#include <linux/pm.h>*/

#include "hi_type.h"
#include "hi_drv_i2c.h"
#include "hi_debug.h"
#include "hi_drv_struct.h"
#include "drv_i2c_ext.h"
#include "hi_drv_tuner.h"
#include "hi_unf_frontend.h"

//#include "common_dev.h"
//#include "common_proc.h"
//#include "common_mem.h"
//#include "common_stat.h"

#define QAM_PORT0_ADDR 0xA0
#define QAM_PORT1_ADDR 0xA2
#define QAM_PORT2_ADDR 0xA0
#define QAM_PORT3_ADDR 0xA2
#define QAM_PORT4_ADDR 0xA0
#define TUNER_PORT0_ADDR 0xC0
#define TUNER_PORT1_ADDR 0xC0
#define TUNER_PORT2_ADDR 0xC0
#define TUNER_PORT3_ADDR 0xC0
#define TUNER_PORT4_ADDR 0xC0
#define XTAL_CLK 28800

#define TUNER_NUM 5

typedef enum hiTUNER_DATA_SRC_E
{
    TUNER_DATA_SRC_ADC = 0,
    TUNER_DATA_SRC_EQU,
    TUNER_DATA_SRC_BUTT
}TUNER_DATA_SRC_E;

typedef enum hiTUNER_QAM_TYPE_E
{
    QAM_TYPE_16 = 0,
    QAM_TYPE_32,
    QAM_TYPE_64,
    QAM_TYPE_128,
    QAM_TYPE_256
}TUNER_QAM_TYPE_E;

typedef struct hiTUNER_ACC_QAM_PARAMS_S
{
    HI_U32 u32Frequency;            /* frequency kHz*/
    union
    {
        HI_U32 u32SymbolRate;           /* symbolrate Hz*/
        HI_U32 u32BandWidth;           /* BandWidth KHz*/
    } unSRBW;
    TUNER_QAM_TYPE_E enQamType;
    HI_BOOL bSI;              /*reverse*/
    HI_UNF_TUNER_FE_POLARIZATION_E enPolar; 
    int fill[3];
}TUNER_ACC_QAM_PARAMS_S;

typedef struct  hiTUNER_DATA_S
{
    HI_U32        u32Port;
    HI_U32        u32Data;
}TUNER_DATA_S, *PTR_TUNER_DATA_S;

typedef struct  hiTUNER_DATA_456_S
{
    HI_U32        u32Port;
    HI_U8         u8Data;
}TUNER_DATA_456_S, *PTR_TUNER_DATA_456_S;

typedef struct  hiTUNER_DATABUF_S
{
    HI_U32         u32Port;
    HI_U32         u32DataBuf[3];
}TUNER_DATABUF_S, *PTR_TUNER_DATABUF_S;
typedef struct  hiTUNER_SIGNALINFO_S
{
    HI_U32         u32Port;
    HI_UNF_TUNER_SIGNALINFO_S stInfo;
    //64
}TUNER_SIGNALINFO_S, *PTR_TUNER_SIGNALINFO_S;

typedef struct  hiTUNER_LNBCONFIG_S
{
    HI_U32         u32Port;
    HI_UNF_TUNER_FE_LNB_CONFIG_S stLNB;
    int fill[4];
}TUNER_LNBCONFIG_S, *PTR_TUNER_LNBCONFIG_S;

#define MAX_TP_NUM (128)

typedef struct  hiTUNER_BLINDSCAN_INITPARA_S
{
    HI_U32 u32Data;                 /*For extend*/
}TUNER_BLINDSCAN_INITPARA_S, *PTR_TUNER_BLINDSCAN_INITPARA_S;

typedef struct  hiTUNER_BLINDSCAN_S
{
    HI_U32         u32Port;
    TUNER_BLINDSCAN_INITPARA_S stPara;
}TUNER_BLINDSCAN_S, *PTR_TUNER_BLINDSCAN_S;

typedef struct  hiTUNER_BLINDSCAN_PARA_S
{
    HI_U32 u32CentreFreq;           /*Blind scan centre frequency, kHz*/
    HI_U16 u16Count;                /*Channel find in this scanning */
    union
    {
        HI_UNF_TUNER_SAT_TPINFO_S astSat[MAX_TP_NUM];
    }unResult;
}TUNER_BLINDSCAN_PARA_S, *PTR_TUNER_BLINDSCAN_PARA_S;

#if 0
typedef struct  hiTUNER_BLINDSCAN_PARA_S
{
    HI_U32 u32CentreFreq;           /*Blind scan centre frequency, kHz*/
    HI_U16 u16Count;                /*Channel find in this scanning */
    HI_UNF_TUNER_SAT_TPINFO_S astSat[MAX_TP_NUM];
}TUNER_BLINDSCAN_PARA_S, *PTR_TUNER_BLINDSCAN_PARA_S;
#endif

typedef struct  hiTUNER_BLINDSCAN_INFO_S
{
    HI_U32         u32Port;
    TUNER_BLINDSCAN_PARA_S* pstPara;
}TUNER_BLINDSCAN_INFO_S, *PTR_TUNER_BLINDSCAN_INFO_S;

typedef struct  hiTUNER_TERRSCAN_INFO_S
{
    HI_U32         u32Port;
    struct
    {
    	int fill[5]; //4 = 0
    } Data_4; //4
    struct
    {
    	int fill[35]; //24 = 0
    } Data_24; //24
    //164
}TUNER_TERRSCAN_INFO_S, *PTR_TUNER_TERRSCAN_INFO_S;

typedef enum  hiTUNER_SETFUNC_MODE_E
{
    FunctMode_Demod = 0,
    FunctMode_BlindScan = 1
}TUNER_FUNC_MODE_E;

/* LNB out voltage enum */
typedef enum hiTUNER_LNB_OUT_E
{
    TUNER_LNB_OUT_0V = 0,
    TUNER_LNB_OUT_13V = 13,
    TUNER_LNB_OUT_14V = 14, /* 14 or 15V */
    TUNER_LNB_OUT_18V = 18,
    TUNER_LNB_OUT_19V = 19, /* 19 or 20V */
    TUNER_LNB_OUT_BUTT
}TUNER_LNB_OUT_E;

/* LNB out voltage struct */
typedef struct  hiTUNER_LNB_OUT_S
{
    HI_U32          u32Port;
    TUNER_LNB_OUT_E enOut;
}TUNER_LNB_OUT_S, *PTR_TUNER_LNB_OUT_S;

/* DiSEqC send message */
typedef struct  hiTUNER_DISEQC_SENDMSG_S
{
    HI_U32         u32Port;
    HI_UNF_TUNER_DISEQC_SENDMSG_S stSendMsg;
}TUNER_DISEQC_SENDMSG_S, *PTR_TUNER_DISEQC_SENDMSG_S;

typedef struct  hiTUNER_DISEQC_RECVMSG_S
{
    HI_U32         u32Port;
    HI_UNF_TUNER_DISEQC_RECVMSG_S* pstRecvMsg;  /* Read data */
}TUNER_DISEQC_RECVMSG_S, *PTR_TUNER_DISEQC_RECVMSG_S;

typedef struct hiTUNER_SET_TUNER_S
{
    HI_U32 u32Freq;
    HI_S32 s32Times;    
}TUNER_SET_TUNER_S, *PTR_TUNER_SET_TUNER_S;


typedef struct  hiTUNER_SIGNAL_S
{
    HI_UNF_TUNER_SIG_TYPE_E enSigType;
    HI_U32 u32Port;
    TUNER_ACC_QAM_PARAMS_S  stSignal;
    //int fill[3];
    //40
}TUNER_SIGNAL_S, *PTR_TUNER_SIGNAL_S;

typedef struct hiTUNER_TP_VERIFY_PARAMS_S
{
    HI_U32 *pu32Frequency;            /**<frequency kHz*/                            /**<CNcomment:Ƶ�ʣ���λkHz*/
    HI_U32 *pu32SymbolRate;           /**<symbolrate kBaud*/                         /**<CNcomment:����ʣ���λkBaud*/
    HI_UNF_TUNER_FE_POLARIZATION_E enPolar; /**<TP polarization*/                    /**<CNcomment:TP�ļ�����ʽ*/
    HI_U8 cbs_reliablity;                           /**<TP reliability*/             /**<CNcomment:TP�Ŀɿ���*/
    HI_S32 CBS_TH;                                   /**<blindscan threshold*/        /**<CNcomment:äɨ��ֵ*/
    HI_U8 fs_grade;
    HI_U32 *fec_ok_cnt;                             /**<TP number of fec ok*/        /**<CNcomment:fec ok��TP����*/
    HI_U32 *fec_no_ok_cnt;                          /**<TP number of fec  not ok*/   /**<CNcomment:fec not ok��TP����*/
}TUNER_TP_VERIFY_PARAMS_S;

typedef struct  hiTUNER_TP_VERIFY_INFO_S
{
    HI_U32 u32Port;
    TUNER_TP_VERIFY_PARAMS_S  stTPVerifyPrm;
}TUNER_TP_VERIFY_INFO_S, *PTR_TUNER_TP_VERIFY_INFO_S;

typedef struct hiTUNER_SAMPLE_DATA_PARAM_S
{
    TUNER_DATA_SRC_E enDataSrc;
    HI_U32 u32DataLen;
    HI_UNF_TUNER_SAMPLE_DATA_S *pstData;
}TUNER_SAMPLE_DATA_PARAM_S;

typedef enum hiTUNER_QAMINSIDE_E
{
    TUNER_QAMINSIDE_IN = 0,
    TUNER_QAMINSIDE_OUT,
    TUNER_QAMINSIDE_INVALID,
}TUNER_QAMINSIDE_E;

typedef struct tag_TunerAttr_S
{
    HI_UNF_TUNER_DEV_TYPE_E enTunerDevType;     /**<Tuner type*/                              /**<CNcomment:TUNER����*/
    HI_U32 u32TunerAddr;
} HI_TunerAttr_S;

typedef struct tag_DemodAttr_S
{
    HI_UNF_DEMOD_DEV_TYPE_E enDemodDevType;     /**<QAM type*/                                /**<CNcomment:QAM����*/
    HI_U32 u32DemodAddr;
    int fill;
} HI_DemodAttr_S;

typedef struct hiTUNER_I2C_DATA_S
{       
    HI_U8* pu8SendBuf;
    HI_U32 u32SendLength;
    HI_U8* pu8ReceiveBuf;
    HI_U32 u32ReceiveLength;
} TUNER_I2C_DATA_S, *PTR_TUNER_I2C_DATA_S; 

typedef struct  hiAGC_TEST_S 
{
    HI_U32      u32Port;
    HI_U32		u32Agc1;
    HI_U32		u32Agc2;
    HI_BOOL     bLockFlag; 
    HI_BOOL		bAgcLockFlag;
    HI_U8 		u8BagcCtrl12;
	HI_U32		u32Count;
}AGC_TEST_S;

/* standard tuner operation */
typedef struct _TUNER_OPS_S
{
    HI_UNF_TUNER_SIG_TYPE_E enSigType; //256 = 0
    HI_U8 enI2cChannel; //260 = 4
    HI_UNF_TUNER_DEV_TYPE_E   enTunerDevType; //264 = 8
    HI_UNF_DEMOD_DEV_TYPE_E	  enDemodDevType; //268 = 12
	TUNER_ACC_QAM_PARAMS_S    stCurrPara; //272 = 16
//	int fill1[3]; // = 36
	HI_UNF_TUNER_OUPUT_MODE_E      enTsType; //304 = 48
	
	HI_U32	u32DemodAddress; //308 = 52
	HI_U32	u32XtalClk;
	HI_U32  u32TunerAddress; //316 = 60
	HI_U32  u32CurrQamMode;
	HI_U8   u8AdcType;
	HI_U8   u8AgcOutputSel;
	HI_U8   u8AdcDataFmt;
	
    HI_S32 (*tuner_connect)(HI_U32 u32TunerPort, TUNER_ACC_QAM_PARAMS_S * pstChannel); //328 = 72
    HI_S32 (*tuner_get_status)(HI_U32 u32TunerPort, HI_UNF_TUNER_LOCK_STATUS_E * penTunerStatus); //332 = 76
    HI_S32 (*tuner_get_ber)(HI_U32 u32TunerPort, HI_U32 * pu32BERa); //336 = 80
    HI_S32 (*tuner_get_snr)(HI_U32 u32TunerPort, HI_U32 * pu32SignalStrength); //340 = 84
    HI_S32 (*tuner_get_signal_strength)(HI_U32 u32TunerPort, HI_U32 * pu32strength); //344 = 88
    HI_S32 (*tuner_set_ts_type)(HI_U32 u32TunerPort, HI_UNF_TUNER_OUPUT_MODE_E enTsType); //348 = 92
    HI_S32 (*set_tuner)(HI_U32 u32TunerPort, HI_U8 enI2cChannel, HI_U32 u32RF); //352 = 96
	HI_S32 (*tuner_get_freq_symb_offset)(HI_U32 u32TunerPort, HI_U32 * pu32Freq, HI_U32 * pu32Symb ); //356 = 100
	HI_S32  (*tuner_get_rs)(HI_U32 u32TunerPort, HI_U32 *pu32Rs); //360 = 104
    HI_VOID (*manage_after_chipreset)(HI_U32 u32TunerPort); //364 = 108
    HI_VOID (*recalculate_signal_strength)(HI_U32 u32TunerPort, HI_U32 * pu32Strength); //368 = 112
    HI_VOID (*tuner_test_single_agc)( HI_U32 u32TunerPort, AGC_TEST_S * pstAgcTest );	/* just for test */ //372 = 116
	HI_VOID (*tuner_resume)( HI_U32 u32TunerPort ); //376 = 120
	HI_VOID (*tuner_get_registers)(HI_U32 u32TunerPort, void *p); //380 = 124
	HI_VOID (*tuner_connect_timeout)(HI_U32 u32ConnectTimeout); //384 = 128
    /* Added begin:l00185424 2011-11-28 For DVB-S/S2 */
    HI_S32 (*tuner_init)(HI_U32 u32TunerPort, HI_U8 enI2cChannel, HI_UNF_TUNER_DEV_TYPE_E enTunerDevType); //388 = 132
    HI_S32 (*tuner_set_sat_attr)(HI_U32 u32TunerPort, HI_UNF_TUNER_SAT_ATTR_S *pstSatTunerAttr); //392 = 136
    HI_S32 (*Func_396/*hi3137_set_ter_attr*/)(HI_U32 u32TunerPort, HI_UNF_TUNER_SAT_ATTR_S *pstSatTunerAttr); //396?????
    HI_S32 (*tuner_get_signal_info)(HI_U32 u32TunerPort, HI_UNF_TUNER_SIGNALINFO_S *pstInfo); //400 = 144
    HI_S32 (*tuner_blindscan_init)(HI_U32 u32TunerPort, TUNER_BLINDSCAN_INITPARA_S *pstPara); //404 = 148
    HI_S32 (*tuner_blindscan_action)(HI_U32 u32TunerPort, TUNER_BLINDSCAN_PARA_S *pstPara); //408 = 152
    HI_S32 (*Func_412/*hi3137_tp_scan*/)(HI_U32 u32TunerPort, void* a, void* b); //412
    HI_S32 (*tuner_lnbctrl_dev_init)(HI_U32 u32TunerPort, HI_U32 u32I2CNum, HI_U8 u8DevAddr, 
                            HI_UNF_DEMOD_DEV_TYPE_E enDemodDevType); //416
    HI_S32 (*tuner_lnbctrl_dev_standby)(HI_U32 u32TunerPort, HI_U32 u32Standby); //420 = 164
    HI_S32 (*tuner_set_lnb_out)(HI_U32 u32TunerPort, TUNER_LNB_OUT_E enOut); //424 = 168
    HI_S32 (*tuner_send_continuous_22K)(HI_U32 u32TunerPort, HI_U32 u32Continuous22K); //428 = 172
    HI_S32 (*tuner_send_tone)(HI_U32 u32TunerPort, HI_U32 u32Tone); //432 = 176
    HI_S32 (*tuner_DiSEqC_send_msg)(HI_U32 u32TunerPort, HI_UNF_TUNER_DISEQC_SENDMSG_S *pstSendMsg); //436 = 180
    HI_S32 (*tuner_DiSEqC_recv_msg)(HI_U32 u32TunerPort, HI_UNF_TUNER_DISEQC_RECVMSG_S *pstRecvMsg); //440 = 184
    HI_S32 (*tuner_standby)(HI_U32 u32TunerPort, HI_U32 u32Standby); //444 = 188
    HI_S32 (*tuner_setfuncmode)(HI_U32 u32TunerPort, TUNER_FUNC_MODE_E enFuncMode); //448 = 192
    HI_S32 (*tuner_setplpid)(HI_U32 u32TunerPort, HI_U8 u8PLPID, HI_U32 u32Data); //452 = 196
    HI_S32 (*Func_456/*hi3137_set_common_plp_id*/)(HI_U32 u32TunerPort, HI_U8 u8Data); //456 = 200
    HI_S32 (*Func_460/*hi3137_set_common_plp_combination*/)(HI_U32 u32TunerPort, HI_U8 u8Data); //460
    HI_S32 (*tuner_get_plp_num)(HI_U32 u32TunerPort, HI_U8 *pu8PLPNum); //464 = 208
    HI_S32 (*tuner_get_current_plp_type)(HI_U32 u32TunerPort, HI_UNF_TUNER_T2_PLP_TYPE_E *penPLPType); //468 = 212
    HI_S32 (*Func_472/*hi3137_get_plp_id*/)(HI_U32 u32TunerPort, HI_U32* pu32Data); //472
    HI_S32 (*Func_476/*hi3137_get_plp_group_id*/)(HI_U32 u32TunerPort, HI_U32* pu32Data); //476
    HI_S32 (*Func_480/*hi3137_set_antena_power*/)(HI_U32 u32TunerPort, HI_U32 u32Data); //480
    HI_S32 (*tuner_tp_verify)(HI_U32 u32TunerPort, TUNER_TP_VERIFY_PARAMS_S * pstChannel); //484 = 228
    HI_S32 (*tuner_set_ts_out)(HI_U32 u32TunerPort, HI_UNF_TUNER_TSOUT_SET_S *pstTSOut); //488 = 232
    HI_S32 (*tuner_data_sample)(HI_U32 u32TunerPort, TUNER_DATA_SRC_E enDataSrc, HI_U32 u32DataLen, HI_UNF_TUNER_SAMPLE_DATA_S *pstData); //492
    /* Added end:l00185424 2011-11-28 For DVB-S/S2 */
} TUNER_OPS_S;

extern TUNER_OPS_S g_stTunerOps[TUNER_NUM];

/*---- TUNER COMMAND----*/
#define HI_TUNER_IOC_MAGIC 't'
#define TUNER_CONNECT_CMD _IOW(HI_TUNER_IOC_MAGIC, 1, TUNER_SIGNAL_S)
#define TUNER_GET_STATUS_CMD _IOWR(HI_TUNER_IOC_MAGIC, 2, TUNER_DATA_S)
#define TUNER_GET_SIGNALSTRENGTH_CMD _IOWR(HI_TUNER_IOC_MAGIC, 3, TUNER_DATABUF_S)
#define TUNER_GET_BER_CMD _IOWR(HI_TUNER_IOC_MAGIC, 4, TUNER_DATABUF_S)
#define TUNER_GET_SNR_CMD _IOWR(HI_TUNER_IOC_MAGIC, 5, TUNER_DATA_S)
#define TUNER_SET_TSTYPE_CMD _IOW(HI_TUNER_IOC_MAGIC, 6, TUNER_DATA_S)
#define TUNER_SELECT_TYPE_CMD _IOW(HI_TUNER_IOC_MAGIC, 7, TUNER_DATABUF_S)
#define TUNER_SELECT_I2C_CMD _IOW(HI_TUNER_IOC_MAGIC, 8, TUNER_DATA_S)
#define TUNER_SELECT_RW_CMD _IOWR(HI_TUNER_IOC_MAGIC, 9, TUNER_RegRW_S)
#define TUNER_SET_TUNER_CMD _IOWR(HI_TUNER_IOC_MAGIC, 10, TUNER_SET_TUNER_S)
#define TUNER_LOW_CONS_CMD _IO(HI_TUNER_IOC_MAGIC, 11)
#define TUNER_NORMAL_MODE_CMD _IO(HI_TUNER_IOC_MAGIC, 12)
#define TUNER_SET_QAMINSIDE_CMD _IOR(HI_TUNER_IOC_MAGIC, 13, TUNER_QAMINSIDE_E)
#define TUNER_CONNECTUNBLOCK_CMD _IOW(HI_TUNER_IOC_MAGIC, 14, TUNER_SIGNAL_S)
#define TUNER_SELECT_SYMBOLRATE_CMD _IOR(HI_TUNER_IOC_MAGIC, 15, TUNER_DATA_S)
#define TUNER_CHECK_VALID_I2CADDR _IOW(HI_TUNER_IOC_MAGIC, 16, TUNER_DATA_S) 
#define TUNER_TEST_SINGLE_AGC _IOWR(HI_TUNER_IOC_MAGIC, 17, AGC_TEST_S) 
#define TUNER_GET_FREQ_SYMB_OFFSET _IOWR(HI_TUNER_IOC_MAGIC, 18, TUNER_DATABUF_S)
#define TUNER_CONNECT_TIMEOUT_CMD _IOWR(HI_TUNER_IOC_MAGIC, 19, TUNER_DATABUF_S)
#define TUNER_GET_SIGANLQUALITY_CMD _IOWR(HI_TUNER_IOC_MAGIC, 20, TUNER_DATABUF_S)
#define TUNER_GET_SIGANLINFO_CMD _IOWR(HI_TUNER_IOC_MAGIC, 21, TUNER_SIGNALINFO_S)
#define TUNER_BLINDSCAN_INIT_CMD _IOWR(HI_TUNER_IOC_MAGIC, 22, TUNER_BLINDSCAN_S)
#define TUNER_BLINDSCAN_ACTION_CMD _IOWR(HI_TUNER_IOC_MAGIC, 23, TUNER_BLINDSCAN_INFO_S)
#define TUNER_SET_LNBOUT_CMD _IOWR(HI_TUNER_IOC_MAGIC, 24, TUNER_LNB_OUT_S)
#define TUNER_SEND_CONTINUOUS_22K_CMD _IOWR(HI_TUNER_IOC_MAGIC, 25, TUNER_DATA_S)
#define TUNER_SEND_TONE_CMD _IOWR(HI_TUNER_IOC_MAGIC, 26, TUNER_DATA_S)
#define TUNER_DISEQC_SEND_MSG_CMD _IOWR(HI_TUNER_IOC_MAGIC, 27, TUNER_DISEQC_SENDMSG_S)
#define TUNER_DISEQC_RECV_MSG_CMD _IOWR(HI_TUNER_IOC_MAGIC, 28, TUNER_DISEQC_RECVMSG_S)
#define TUNER_STANDBY_CMD _IOWR(HI_TUNER_IOC_MAGIC, 29, TUNER_DATA_S)
#define TUNER_DISABLE_CMD _IOWR(HI_TUNER_IOC_MAGIC, 30, TUNER_DATA_S)
#define TUNER_SETFUNCMODE_CMD _IOW(HI_TUNER_IOC_MAGIC, 31, TUNER_DATA_S) 
#define TUNER_SETPLPNO_CMD _IOW(HI_TUNER_IOC_MAGIC, 32, TUNER_DATABUF_S) //TUNER_DATA_S)
#define TUNER_GETPLPNUM_CMD _IOWR(HI_TUNER_IOC_MAGIC, 33, TUNER_DATA_S)
#define TUNER_GETCURPLPTYPE_CMD _IOWR(HI_TUNER_IOC_MAGIC, 34, TUNER_DATA_S)
#define TUNER_SETTSOUT_CMD _IOWR(HI_TUNER_IOC_MAGIC, 35, TUNER_DATA_S)
#define TUNER_TPVERIFY_CMD _IOWR(HI_TUNER_IOC_MAGIC, 36, TUNER_TP_VERIFY_INFO_S)
#define TUNER_SETSATATTR_CMD _IOWR(HI_TUNER_IOC_MAGIC, 37, TUNER_DATA_S)
#define TUNER_SAMPLE_DATA_CMD _IOWR(HI_TUNER_IOC_MAGIC, 38, TUNER_DATA_S)
#define TUNER_SET_COMMONPLPID_CMD _IOW(HI_TUNER_IOC_MAGIC, 39, TUNER_DATA_S)
#define TUNER_SET_COMMONPLPCOMBINATION_CMD _IOW(HI_TUNER_IOC_MAGIC, 40, TUNER_DATA_S)
#define TUNER_GET_PLPID_CMD _IOWR(HI_TUNER_IOC_MAGIC, 41, TUNER_DATA_S)
#define TUNER_GET_GROUPPLPID_CMD _IOWR(HI_TUNER_IOC_MAGIC, 42, TUNER_DATA_S)
#define TUNER_SET_ANTENNAPOWER_CMD _IOW(HI_TUNER_IOC_MAGIC, 43, TUNER_DATA_S)
#define TUNER_TERSCAN_ACTION_CMD _IOWR(HI_TUNER_IOC_MAGIC, 44, TUNER_TERRSCAN_INFO_S)

/*
#define TUNER_BASE_CONNECT_CMD _IOW(HI_TUNER_IOC_MAGIC, 20, TUNER_SIGNAL_S)
#define TUNER_BASE_SELECT_TYPE_CMD _IOW(HI_TUNER_IOC_MAGIC, 21, TUNER_DATABUF_S)
#define TUNER_BASE_SELECT_I2C_CMD _IOW(HI_TUNER_IOC_MAGIC, 22, TUNER_DATA_S)
#define TUNER_BASE_SET_TSTYPE_CMD _IOW(HI_TUNER_IOC_MAGIC, 23, TUNER_DATA_S)
*/

extern HI_S32 tuner_i2c_send_data(HI_U32 u32I2cChannel, HI_U8 u8DevAddress, PTR_TUNER_I2C_DATA_S pstDataStruct);
extern HI_S32 tuner_i2c_receive_data(HI_U32 u32I2cChannel, HI_U8 u8DevAddress, PTR_TUNER_I2C_DATA_S pstDataStruct);
extern HI_VOID reset_special_process_flag(HI_BOOL flag);
extern HI_VOID tuner_mdelay(HI_U32 u32MS);

#endif

