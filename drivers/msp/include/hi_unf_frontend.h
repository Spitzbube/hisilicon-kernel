/******************************************************************************

  Copyright (C), 2001-2011, HiSilicon Technologies Co., Ltd.
 ******************************************************************************
 File Name     : hi_unf_frontend.h
Version       : Initial draft
Author        : HiSilicon multimedia software group
Created Date   : 2008-06-05
Last Modified by:
Description   : Application programming interfaces (APIs) of the external chip software (ECS)
Function List :
Change History:
 ******************************************************************************/
#ifndef __HI_UNF_FRONTEND_H__
#define __HI_UNF_FRONTEND_H__

#include "hi_common.h"
//#include "hi_error_mpi.h"

//#include "hi_unf_keyled.h"
//#include "hi_unf_ir.h"
//#include "hi_unf_pmoc.h"
#include "hi_unf_i2c.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#define HI_I2C_MAX_NUM_USER (15)    /** Maximum I2C channel ID*/ /** CNcomment:���I2Cͨ����*/

#define HI_UNF_DISEQC_MSG_MAX_LENGTH (6)    /** DiSEqC message length*/  /** CNcomment:DiSEqC��Ϣ����*/
#define HI_UNF_DISEQC_MAX_REPEAT_TIMES (4)  /** DiSEqC message max repeat times*/ /** CNcomment:DiSEqC��Ϣ����ظ����ʹ���*/
#define MAX_TS_LINE 11                      /** The NO of ts lines that can be configured*/ /** CNcomment:�����õ�ts�ź�������*/

//#ifdef DISEQC_SUPPORT
#define DISEQC_MAX_MOTOR_PISITION (255) /** DiSEqC motor max stored position*/ /** CNcomment:DiSEqC������洢��λ����*/
//#endif /* DISEQC_SUPPORT */
/*************************** Structure Definition ****************************/
/** \addtogroup      FRONTEND */
/** @{ */  /** <!-- [FRONTEND] */

/** Sample data, complex format*/
/** CNcomment: �ɼ����, ����ʽ*/
typedef struct  hiUNF_TUNER_SAMPLE_DATA_S
{
   HI_S32 s32DataIP;    /*sample data, i component*/    /**<CNcomment:�ɼ���ݵ�I����*/
   HI_S32 s32DataQP;    /*sample data, q component*/    /**<CNcomment:�ɼ���ݵ�Q����*/
} HI_UNF_TUNER_SAMPLE_DATA_S;

/** Sample data length*/
/** CNcomment: �����*/
typedef enum hiUNF_TUNER_SAMPLE_DATALEN_E
{
    HI_UNF_TUNER_SAMPLE_DATALEN_512,              /*sample 512 pts*/        /**<CNcomment:�ɼ�512��*/
    HI_UNF_TUNER_SAMPLE_DATALEN_1024,             /*sample 1024 pts*/       /**<CNcomment:�ɼ�1024��*/
    HI_UNF_TUNER_SAMPLE_DATALEN_2048,             /*sample 2048 pts*/       /**<CNcomment:�ɼ�2048��*/
    HI_UNF_TUNER_SAMPLE_DATALEN_BUTT              /**<Invalid value*/       /**<CNcomment:�Ƿ��߽�ֵ*/
} HI_UNF_TUNER_SAMPLE_DATALEN_E;

/** Modulation mode*/
/** CNcomment: ���Ʒ�ʽ*/
typedef enum hiUNF_QAM_TYPE_E
{
    HI_UNF_MOD_TYPE_DEFAULT,          /**<Default QAM mode. The default QAM mode is HI_UNF_MOD_TYPE_QAM_64 at present.*/                /**<CNcomment:Ĭ�ϵ�QAM����, ��ǰϵͳĬ��ΪHI_UNF_MOD_TYPE_QAM_64 */
    HI_UNF_MOD_TYPE_QAM_16 = 0x100,   /**<Enumeration corresponding to the 16QAM mode*/                                                 /**<CNcomment:16QAM��Ӧ��ö��ֵ*/
    HI_UNF_MOD_TYPE_QAM_32,           /**<Enumeration corresponding to the 32QAM mode*/                                                 /**<CNcomment:32QAM��Ӧ��ö��ֵ*/
    HI_UNF_MOD_TYPE_QAM_64,           /**<Enumeration corresponding to the 64QAM mode*/                                                 /**<CNcomment:64QAM��Ӧ��ö��ֵ*/
    HI_UNF_MOD_TYPE_QAM_128,          /**<Enumeration corresponding to the 128QAM mode*/                                                /**<CNcomment:128QAM��Ӧ��ö��ֵ*/
    HI_UNF_MOD_TYPE_QAM_256,          /**<Enumeration corresponding to the 256QAM mode*/                                                /**<CNcomment:256QAM��Ӧ��ö��ֵ*/
    HI_UNF_MOD_TYPE_QAM_512,          /**<Enumeration corresponding to the 512QAM mode*/                                                /**<CNcomment:512QAM��Ӧ��ö��ֵ*/

    HI_UNF_MOD_TYPE_BPSK = 0x200,     /**<Enumeration corresponding to the binary phase shift keying (BPSK) mode. */                    /**<CNcomment:BPSK��Ӧ��ö��ֵ*/
    HI_UNF_MOD_TYPE_QPSK = 0x300,     /**<Enumeration corresponding to the quaternary phase shift keying (QPSK) mode. */                /**<CNcomment:QPSK��Ӧ��ö��ֵ*/
    HI_UNF_MOD_TYPE_DQPSK,
    HI_UNF_MOD_TYPE_8PSK,             /**<Enumeration corresponding to the 8 phase shift keying (8PSK) mode*/                           /**<CNcomment:8PSK��Ӧ��ö��ֵ*/
    HI_UNF_MOD_TYPE_16APSK,           /**<Enumeration corresponding to the 16-Ary Amplitude and Phase Shift Keying (16APSK) mode*/      /**<CNcomment:16APSK��Ӧ��ö��ֵ*/
    HI_UNF_MOD_TYPE_32APSK,           /**<Enumeration corresponding to the 32-Ary Amplitude and Phase Shift Keying (32APSK) mode*/      /**<CNcomment:32APSK��Ӧ��ö��ֵ*/
    HI_UNF_MOD_TYPE_8VSB,             /**<Enumeration corresponding to (8VSB) mode*/                                                    /**<CNcomment:8VSB��Ӧ��ö��ֵ*/
    HI_UNF_MOD_TYPE_16VSB,            /**<Enumeration corresponding to (16VSB) mode*/                                                   /**<CNcomment:16VSB��Ӧ��ö��ֵ*/

    /**<Enumeration corresponding to the auto mode. For DVB-S/S2, if detect modulation type fail, it will return auto*/
    /**<CNcomment:�����źŵ��Ʒ�ʽ�Զ���⣬�����ʧ�ܷ���AUTO*/
    HI_UNF_MOD_TYPE_AUTO,

    HI_UNF_MOD_TYPE_BUTT              /**<Invalid Modulation mode*/       /**<CNcomment:�Ƿ��ĵ�������ö��ֵ*/
}HI_UNF_MODULATION_TYPE_E;

/** Frequency locking status of the tuner*/
/** CNcomment:TUNER��Ƶ״̬*/
typedef enum  hiUNF_TUNER_LOCK_STATUS_E
{
    HI_UNF_TUNER_SIGNAL_DROPPED = 0,       /**<The signal is not locked.*/  /**<CNcomment:�ź�δ��*/
    HI_UNF_TUNER_SIGNAL_LOCKED,            /**<The signal is locked.*/      /**<CNcomment:�ź�����*/
    HI_UNF_TUNER_SIGNAL_BUTT               /**<Invalid value*/              /**<CNcomment:�Ƿ��߽�ֵ*/
}HI_UNF_TUNER_LOCK_STATUS_E;

/** Output mode of the tuner*/
/** CNcomment:TUNER���ģʽ*/
typedef enum hiUNF_TUNER_OUTPUT_MODE_E
{
    HI_UNF_TUNER_OUTPUT_MODE_DEFAULT ,            /**<Default mode*/                       /**<CNcomment:Ĭ��ģʽ*/
    HI_UNF_TUNER_OUTPUT_MODE_PARALLEL_MODE_A,     /**<Parallel mode A*/                    /**<CNcomment:����ģʽA*/
    HI_UNF_TUNER_OUTPUT_MODE_PARALLEL_MODE_B,     /**<Parallel mode B*/                    /**<CNcomment:����ģʽB*/
    HI_UNF_TUNER_OUTPUT_MODE_SERIAL,              /**<Serial mode 74.25M*/                 /**<CNcomment:����ģ74.25M*/ 
    HI_UNF_TUNER_OUTPUT_MODE_SERIAL_50,           /**<Serial mode 50M*/                    /**<CNcomment:����ģ50M*/
    HI_UNF_TUNER_OUTPUT_MODE_SERIAL_2BIT,         /**<Serial mode witch 2 bit data line*/  /**<CNcomment:2bit����*/
    HI_UNF_TUNER_OUTPUT_MODE_BUTT                 /**<Invalid value*/                      /**<CNcomment:�Ƿ�ֵ*/
}HI_UNF_TUNER_OUPUT_MODE_E;

/** Output order*/
/** CNcomment:TUNER������� */
typedef enum hiUNF_TUNER_OUTPUT_TS_E
{
    HI_UNF_TUNER_OUTPUT_TSDAT0,        /**<data0*/                       /**<CNcomment:�����0bit*/    
    HI_UNF_TUNER_OUTPUT_TSDAT1,        /**<data1*/                       /**<CNcomment:�����1bit*/ 
    HI_UNF_TUNER_OUTPUT_TSDAT2,        /**<data2*/                       /**<CNcomment:�����2bit*/ 
    HI_UNF_TUNER_OUTPUT_TSDAT3,        /**<data3*/                       /**<CNcomment:�����3bit*/ 
    HI_UNF_TUNER_OUTPUT_TSDAT4,        /**<data4*/                       /**<CNcomment:�����4bit*/ 
    HI_UNF_TUNER_OUTPUT_TSDAT5,        /**<data5*/                       /**<CNcomment:�����5bit*/ 
    HI_UNF_TUNER_OUTPUT_TSDAT6,        /**<data6*/                       /**<CNcomment:�����6bit*/ 
    HI_UNF_TUNER_OUTPUT_TSDAT7,        /**<data7*/                       /**<CNcomment:�����7bit*/ 
    HI_UNF_TUNER_OUTPUT_TSVLD,         /**<valid*/                       /**<CNcomment:valid�ź���*/ 
    HI_UNF_TUNER_OUTPUT_TSSYNC,        /**<sync*/                        /**<CNcomment:sync�ź���*/ 
    HI_UNF_TUNER_OUTPUT_TSERR,         /**<err*/                         /**<CNcomment:err�ź���*/ 
    HI_UNF_TUNER_OUTPUT_BUTT           /**<Invalid value*/               /**<CNcomment:�Ƿ�ֵ*/
}HI_UNF_TUNER_OUTPUT_TS_E;

/** ts output port order*/
/** CNcomment:ts�ӿ�������� */
typedef struct hiUNF_TUNER_TSOUT_SET_S
{
    HI_UNF_TUNER_OUTPUT_TS_E enTSOutput[MAX_TS_LINE];   /**<ts output port order*/    /**<CNcomment:ts�ӿ��������*/
}HI_UNF_TUNER_TSOUT_SET_S;

/** Signal type of the tuner*/
/** CNcomment:TUNER�ź�����*/
typedef enum    hiTUNER_SIG_TYPE_E
{
    HI_UNF_TUNER_SIG_TYPE_CAB = 0  ,   /**<Cable signal*/       /**<CNcomment:CABLE�ź�*/
    HI_UNF_TUNER_SIG_TYPE_SAT ,        /**<Satellite signal*/   /**<CNcomment:�����ź�*/
    HI_UNF_TUNER_SIG_TYPE_DVB_T  ,     /**<Terrestrial signal*/ /**<CNcomment:�����ź�*/
    HI_UNF_TUNER_SIG_TYPE_DVB_T2  ,    /**<Terrestrial signal*/ /**<CNcomment:�����ź�*/
    HI_UNF_TUNER_SIG_TYPE_ISDB_T  ,    /**<Terrestrial signal*/ /**<CNcomment:�����ź�*/
    HI_UNF_TUNER_SIG_TYPE_ATSC_T  ,    /**<Terrestrial signal*/ /**<CNcomment:�����ź�*/
    HI_UNF_TUNER_SIG_TYPE_DTMB  ,      /**<Terrestrial signal*/ /**<CNcomment:�����ź�*/
    HI_UNF_TUNER_SIG_TYPE_BUTT         /**<Invalid value*/      /**<CNcomment:�Ƿ�ֵ*/
}HI_UNF_TUNER_SIG_TYPE_E;

/** type of TUNER device*/
/** CNcomment:TUNER�豸����*/
typedef enum    hiUNF_TUNER_DEV_TYPE_E
{
    HI_UNF_TUNER_DEV_TYPE_XG_3BL,               /**<XG_3BL*/                     /**<CNcomment:֧������Ƶͷ*/
    HI_UNF_TUNER_DEV_TYPE_CD1616,               /**<CD1616*/                     /**<CNcomment:֧��CD1616��Ƶͷ*/
    HI_UNF_TUNER_DEV_TYPE_ALPS_TDAE,            /**<ALPS_TDAE*/                  /**<CNcomment:֧��ALPS_TDAE��Ƶͷ*/
    HI_UNF_TUNER_DEV_TYPE_TDCC,                 /**<TDCC*/                       /**<CNcomment:֧��TDCC��Ƶͷ*/
    HI_UNF_TUNER_DEV_TYPE_TDA18250,             /**<TDA18250*/                   /**<CNcomment:֧��TDA18250��Ƶͷ*/
    HI_UNF_TUNER_DEV_TYPE_CD1616_DOUBLE,        /**<CD1616 with double agc*/     /**<CNcomment:֧��CD1616��˫agc��Ƶͷ*/
    HI_UNF_TUNER_DEV_TYPE_MT2081,				/**<MT2081*/                     /**<CNcomment:֧��MT2081��Ƶͷ*/
    HI_UNF_TUNER_DEV_TYPE_TMX7070X,             /**<THOMSON7070X*/               /**<CNcomment:֧��THOMSON7070X��Ƶͷ*/
    HI_UNF_TUNER_DEV_TYPE_R820C,                /**<R820C*/                      /**<CNcomment:֧��R820C��Ƶͷ*/
	HI_UNF_TUNER_DEV_TYPE_MXL203,               /**<MXL203 */                    /**<CNcomment:֧��MXL203��Ƶͷ*/
    HI_UNF_TUNER_DEV_TYPE_AV2011,               /**<AV2011*/                     /**<CNcomment:֧��AV2011��Ƶͷ*/
    HI_UNF_TUNER_DEV_TYPE_SHARP7903,            /**<SHARP7903*/                  /**<CNcomment:֧��SHARP7903��Ƶͷ*/
    HI_UNF_TUNER_DEV_TYPE_MXL101,               /**<MXL101*/                     /**<CNcomment:֧��MXL101��Ƶͷ*/
    HI_UNF_TUNER_DEV_TYPE_MXL603,               /**<MXL603*/                     /**<CNcomment:֧��MXL603��Ƶͷ*/
    HI_UNF_TUNER_DEV_TYPE_IT9170,               /**<IT9170*/                     /**<CNcomment:֧��IT9170��Ƶͷ*/
    HI_UNF_TUNER_DEV_TYPE_IT9133,               /**<IT9133*/                     /**<CNcomment:֧��IT9133��Ƶͷ*/
    HI_UNF_TUNER_DEV_TYPE_TDA6651,              /**<TDA6651*/                    /**<CNcomment:֧��TDA6651��Ƶͷ*/
    HI_UNF_TUNER_DEV_TYPE_TDA18250B,            /**<TDA18250B*/                  /**<CNcomment:֧��TDA18250B��Ƶͷ*/
    HI_UNF_TUNER_DEV_TYPE_M88TS2022,            /**<M88TS2022*/                  /**<CNcomment:֧��M88TS2022��Ƶͷ*/
    HI_UNF_TUNER_DEV_TYPE_RDA5815,              /**<RDA5815*/                    /**<CNcomment:֧��RDA5815��Ƶͷ*/
    HI_UNF_TUNER_DEV_TYPE_20,
    HI_UNF_TUNER_DEV_TYPE_21,
    HI_UNF_TUNER_DEV_TYPE_22,
    HI_UNF_TUNER_DEV_TYPE_23,
    HI_UNF_TUNER_DEV_TYPE_24,
    HI_UNF_TUNER_DEV_TYPE_25,

    HI_UNF_TUNER_DEV_TYPE_BUTT,                 /**<Invalid value*/              /**<CNcomment:�Ƿ��߽�ֵ*/
}HI_UNF_TUNER_DEV_TYPE_E ;

/** Type of the demod device*/
/** CNcomment:demod�豸����*/
typedef enum    hiUNF_DEMOD_DEV_TYPE_E
{
    HI_UNF_DEMOD_DEV_TYPE_NONE,                /**<Not supported*/      /**<CNcomment:��֧��*/  
    HI_UNF_DEMOD_DEV_TYPE_3130I= 0x100,        /**<Internal QAM*/       /**<CNcomment:�ڲ�QAM*/
    HI_UNF_DEMOD_DEV_TYPE_3130E,               /**<External Hi3130*/    /**<CNcomment:�ⲿQAM hi3130оƬ*/
    HI_UNF_DEMOD_DEV_TYPE_J83B,                /**<suppoort j83b*/      /**<CNcomment:֧��j83b*/
    HI_UNF_DEMOD_DEV_TYPE_AVL6211,             /**<Avalink 6211*/       /**<CNcomment:֧��Avalink 6211*/
    HI_UNF_DEMOD_DEV_TYPE_MXL101,              /**<Maxlinear mxl101*/   /**<CNcomment:֧��Maxlinear mxl101*/
    HI_UNF_DEMOD_DEV_TYPE_MN88472,             /**<PANASONIC mn88472*/  /**<CNcomment:֧��PANASONIC mn88472*/
    HI_UNF_DEMOD_DEV_TYPE_IT9170,              /**<ITE it9170*/         /**<CNcomment:֧��ITE it9170*/
    HI_UNF_DEMOD_DEV_TYPE_IT9133,              /**<ITE it9133*/         /**<CNcomment:֧��ITE it9133*/
    HI_UNF_DEMOD_DEV_TYPE_3136,                /**<External Hi3136*/    /**<CNcomment:�ⲿhi3136оƬ*/
    HI_UNF_DEMOD_DEV_TYPE_3136I,               /**<Internal Hi3136*/    /**<CNcomment:�ڲ�hi3136оƬ*/
    HI_UNF_DEMOD_DEV_TYPE_0x10A,
    HI_UNF_DEMOD_DEV_TYPE_0x10B,
    HI_UNF_DEMOD_DEV_TYPE_0x10C,

    HI_UNF_DEMOD_DEV_TYPE_BUTT,                /**<Invalid value*/      /**<CNcomment:�Ƿ��߽�ֵ*/
}HI_UNF_DEMOD_DEV_TYPE_E ;

/** Defines the cable transmission signal.*/
/** CNcomment:����CABLE�����ź�*/
typedef struct  hiUNF_CAB_CONNECT_PARA_S
{
    HI_U32                      u32Freq;            /**<Frequency, in kHz*/      /**<CNcomment:Ƶ�ʣ���λ��kHz*/
    HI_U32                      u32SymbolRate ;     /**<Symbol rate, in bit/s*/  /**<CNcomment:����ʣ���λbps */
    HI_UNF_MODULATION_TYPE_E    enModType ;         /**<QAM mode*/               /**<CNcomment:QAM���Ʒ�ʽ*/
    HI_BOOL                     bReverse ;          /**<Spectrum reverse mode*/  /**<CNcomment:Ƶ�׵��ô��?ʽ*/
}HI_UNF_CAB_CONNECT_PARA_S ;

/** Guard interval of OFDM*/
/** CNcomment:���ز������µı������*/
typedef enum hiUNF_TUNER_FE_GUARD_INTV_E
{
    HI_UNF_TUNER_FE_GUARD_INTV_DEFALUT = 0 ,       /**<default guard interval mode*/    /**<CNcomment:�������Ĭ��ģʽ*/
    HI_UNF_TUNER_FE_GUARD_INTV_1_128       ,       /**<1/128*/                          /**<CNcomment:�������1/128ģʽ*/
    HI_UNF_TUNER_FE_GUARD_INTV_1_32       ,        /**<1/32*/                           /**<CNcomment:�������1/32ģʽ*/
    HI_UNF_TUNER_FE_GUARD_INTV_1_16       ,        /**<1/16*/                           /**<CNcomment:�������1/16ģʽ*/
    HI_UNF_TUNER_FE_GUARD_INTV_1_8        ,        /**<1/8*/                            /**<CNcomment:�������1/8ģʽ*/
    HI_UNF_TUNER_FE_GUARD_INTV_1_4        ,        /**<1/4*/                            /**<CNcomment:�������1/4ģʽ*/
    HI_UNF_TUNER_FE_GUARD_INTV_19_128       ,      /**<19/128*/                         /**<CNcomment:�������19/128ģʽ*/
    HI_UNF_TUNER_FE_GUARD_INTV_19_256       ,      /**<19/256*/                         /**<CNcomment:�������19/256ģʽ*/
    HI_UNF_TUNER_FE_GUARD_INTV_BUTT     ,          /**<Invalid value*/                  /**<CNcomment:�Ƿ��߽�ֵ*/
}HI_UNF_TUNER_FE_GUARD_INTV_E;

/** OFDM Mode, used in multi-carrier modulation*/
/** CNcomment:OFDMģʽ�����ڶ��ز�����ģʽ��*/
typedef enum hiUNF_TUNER_FE_FFT_E
{
    HI_UNF_TUNER_FE_FFT_DEFAULT = 0,   /**<default mode*/     /**<CNcomment:Ĭ��ģʽ*/
    HI_UNF_TUNER_FE_FFT_1K ,           /**<1k mode*/          /**<CNcomment:1kģʽ*/
    HI_UNF_TUNER_FE_FFT_2K ,           /**<2k mode*/          /**<CNcomment:2kģʽ*/
    HI_UNF_TUNER_FE_FFT_4K ,           /**<4k mode*/          /**<CNcomment:4kģʽ*/
    HI_UNF_TUNER_FE_FFT_8K ,           /**<8k mode*/          /**<CNcomment:8kģʽ*/
    HI_UNF_TUNER_FE_FFT_16K ,          /**<16k mode*/         /**<CNcomment:16kģʽ*/
    HI_UNF_TUNER_FE_FFT_32K ,          /**<32k mode*/         /**<CNcomment:32kģʽ*/
    HI_UNF_TUNER_FE_FFT_BUTT ,         /**<Invalid value*/    /**<CNcomment:�Ƿ��߽�ֵ*/
}HI_UNF_TUNER_FE_FFT_E;

/** Hierarchical modulation mode, only used in DVB-T*/
/** CNcomment:������DVB-T*/
typedef enum hiUNF_TUNER_FE_HIERARCHY_E
{
    HI_UNF_TUNER_FE_HIERARCHY_DEFAULT = 0 ,   /**<hierarchical modulation default mode*/     /**<CNcomment:Ĭ��ģʽ*/
    HI_UNF_TUNER_FE_HIERARCHY_NO,             /**<no hierarchical modulation mode*/          /**<CNcomment:���ּ���ģʽ*/
    HI_UNF_TUNER_FE_HIERARCHY_ALHPA1,         /**<hierarchical mode, alpha = 1*/             /**<CNcomment:�ּ���ģʽ, alpha = 1*/
    HI_UNF_TUNER_FE_HIERARCHY_ALHPA2,         /**<hierarchical mode, alpha = 2*/             /**<CNcomment:�ּ���ģʽ, alpha = 2*/
    HI_UNF_TUNER_FE_HIERARCHY_ALHPA4,         /**<hierarchical mode, alpha = 4*/             /**<CNcomment:�ּ���ģʽ, alpha = 4*/
    HI_UNF_TUNER_FE_HIERARCHY_BUTT,           /**<Invalid value*/                            /**<CNcomment:�Ƿ��߽�ֵ*/
}HI_UNF_TUNER_FE_HIERARCHY_E ;

/** TS Priority, only used in DVB-T*/
/** CNcomment:������DVB-T*/
typedef enum hiUNF_TUNER_TS_PRIORITY_E
{
    HI_UNF_TUNER_TS_PRIORITY_NONE = 0 ,      /**<no priority mode*/         /**<CNcomment:�����ȼ�ģʽ*/
    HI_UNF_TUNER_TS_PRIORITY_HP,             /**<high priority mode*/       /**<CNcomment:�����ȼ�ģʽ*/
    HI_UNF_TUNER_TS_PRIORITY_LP,             /**<low priority mode*/        /**<CNcomment:�����ȼ�ģʽ*/
    HI_UNF_TUNER_TS_PRIORITY_BUTT,           /**<Invalid value*/            /**<CNcomment:�Ƿ��߽�ֵ*/
}HI_UNF_TUNER_TS_PRIORITY_E ;

/** Terestrial connect param*/
/** CNcomment:�����ź���̨����*/
typedef struct  hiUNF_TER_CONNECT_PARA_S
{
    HI_U32          u32Freq     ;                    /**<freq in KHz*/           /**<CNcomment:Ƶ�ʣ���λkHz*/
    HI_U32          u32BandWidth    ;                /**<bandwidth in KHz*/      /**<CNcomment:��?��λkHz*/
    HI_UNF_MODULATION_TYPE_E enModType  ;            /**<modulation type*/       /**<CNcomment:���Ʒ�ʽ*/
    HI_BOOL                     bReverse ;          /**<Spectrum reverse mode*/  /**<CNcomment:Ƶ�׷�ת���?ʽ*/
}HI_UNF_TER_CONNECT_PARA_S ;

/** TS clock polarization*/
/** CNcomment:TSʱ�Ӽ���*/
typedef enum hiUNF_TUNER_TSCLK_POLAR_E
{
    HI_UNF_TUNER_TSCLK_POLAR_FALLING,       /**<Falling edge*/      /**<CNcomment:�½���*/
    HI_UNF_TUNER_TSCLK_POLAR_RISING,        /**<Rising edge*/       /**<CNcomment:������*/
    HI_UNF_TUNER_TSCLK_POLAR_BUTT           /**<Invalid value*/     /**<CNcomment:�Ƿ��߽�ֵ*/
} HI_UNF_TUNER_TSCLK_POLAR_E;

/** TS format*/
/** CNcomment:TS��ʽ*/
typedef enum hiUNF_TUNER_TS_FORMAT_E
{
    HI_UNF_TUNER_TS_FORMAT_TS,              /**<188*/               /**<CNcomment:188�ֽڸ�ʽ*/
    HI_UNF_TUNER_TS_FORMAT_TSP,             /**<204*/               /**<CNcomment:204�ֽڸ�ʽ*/
    HI_UNF_TUNER_TS_FORMAT_BUTT             /**<Invalid value*/     /**<CNcomment:�Ƿ��߽�ֵ*/
} HI_UNF_TUNER_TS_FORMAT_E;

/** TS serial PIN*/
/** CNcomment:����TS��ݹܽ�*/
typedef enum hiUNF_TUNER_TS_SERIAL_PIN_E
{
    HI_UNF_TUNER_TS_SERIAL_PIN_0,           /**<Serial pin 0, default*/  /**<CNcomment:����ģʽ�������ʹ�õ�0bit*/
    HI_UNF_TUNER_TS_SERIAL_PIN_7,           /**<Serial pin 7*/           /**<CNcomment:����ģʽ�������ʹ�õ�7bit*/
    HI_UNF_TUNER_TS_SERIAL_PIN_BUTT         /**<Invalid value*/          /**<CNcomment:�Ƿ��߽�ֵ*/
} HI_UNF_TUNER_TS_SERIAL_PIN_E;

/** Tuner RF AGC mode*/
/** CNcomment:agc����ģʽ*/
typedef enum hiUNF_TUNER_RFAGC_MODE_E
{
    /**< Inverted polarization, default.This setting is used for a tuner whose gain decreases with increased AGC voltage*/   /**<CNcomment:agc�������ģʽ*/
    HI_UNF_TUNER_RFAGC_INVERT, 

    /**< Normal polarization. This setting is used for a tuner whose gain increases with increased AGC voltage*/             /**<CNcomment:agc�������ģʽ*/
    HI_UNF_TUNER_RFAGC_NORMAL,
    HI_UNF_TUNER_RFAGC_BUTT                  /**<Invalid value*/                                                             /**<CNcomment:�Ƿ��߽�ֵ*/
} HI_UNF_TUNER_RFAGC_MODE_E;

/** Tuner IQ spectrum mode*/
/** CNcomment:IQģʽ*/
typedef enum hiUNF_TUNER_IQSPECTRUM_MODE_E
{
    HI_UNF_TUNER_IQSPECTRUM_NORMAL,         /**<The received signal spectrum is not inverted*/    /**<CNcomment:IQ����ת*/
    HI_UNF_TUNER_IQSPECTRUM_INVERT,         /**<The received signal spectrum is inverted*/        /**<CNcomment:IQ��ת*/
    HI_UNF_TUNER_IQSPECTRUM_BUTT            /**<Invalid value*/                                   /**<CNcomment:�Ƿ��߽�ֵ*/
} HI_UNF_TUNER_IQSPECTRUM_MODE_E;

/** DiSEqC Wave Mode*/
/** CNcomment:DiSEqCģʽ*/
typedef enum hiUNF_TUNER_DISEQCWAVE_MODE_E
{
    HI_UNF_TUNER_DISEQCWAVE_NORMAL,         /**<Waveform produced by demod*/                 /**<CNcomment:������demod����*/
    HI_UNF_TUNER_DISEQCWAVE_ENVELOPE,       /**<Waveform produced by LNB control device*/    /**<CNcomment:�����ɿ���оƬ����*/
    HI_UNF_TUNER_DISEQCWAVE_BUTT            /**<Invalid value*/                              /**<CNcomment:�Ƿ��߽�ֵ*/
} HI_UNF_TUNER_DISEQCWAVE_MODE_E;

/** LNB power supply and control device*/
/** CNcomment:LNB����Ϳ���оƬ*/
typedef enum hiUNF_LNBCTRL_DEV_TYPE_E
{
    HI_UNF_LNBCTRL_DEV_TYPE_NONE,           /**<No LNB control device*/      /**<CNcomment:�޿���оƬ*/
    HI_UNF_LNBCTRL_DEV_TYPE_MPS8125,        /**<MPS8125*/                    /**<CNcomment:MPS8125*/
    HI_UNF_LNBCTRL_DEV_TYPE_ISL9492,        /**<ISL9492*/                    /**<CNcomment:ISL9492*/
    HI_UNF_LNBCTRL_DEV_TYPE_BUTT            /**<Invalid value*/              /**<CNcomment:�Ƿ��߽�ֵ*/
} HI_UNF_LNBCTRL_DEV_TYPE_E;

/** Satellite extended attribution*/
/** CNcomment:���ǻ������*/
typedef struct  hiUNF_TUNER_SAT_ATTR_S
{
    HI_U32                         u32DemodClk;     /**<Demod reference clock frequency, KHz*/    /**<CNcomment:demod�ο�ʱ��Ƶ�ʣ���λMHz*/
    HI_U16                         u16TunerMaxLPF;  /**<Tuner max LPF, MHz*/                      /**<CNcomment:tuner��ͨ�˲���ͨ��Ƶ�ʣ���λMHz*/
    HI_U16                         u16TunerI2CClk;  /**<Tuner I2C clock, kHz*/                    /**<CNcomment:tuner i2cʱ��Ƶ��*/
    HI_UNF_TUNER_RFAGC_MODE_E      enRFAGC;         /**<Tuner RF AGC mode*/                       /**<CNcomment:agcģʽ*/
    HI_UNF_TUNER_IQSPECTRUM_MODE_E enIQSpectrum;    /**<Tuner IQ spectrum mode*/                  /**<CNcomment:IQģʽ*/
    HI_UNF_TUNER_TSCLK_POLAR_E     enTSClkPolar;    /**<TS clock polarization*/                   /**<CNcomment:tsʱ�Ӽ���*/
    HI_UNF_TUNER_TS_FORMAT_E       enTSFormat;      /**<TS format*/                               /**<CNcomment:ts��ʽ*/
    HI_UNF_TUNER_TS_SERIAL_PIN_E   enTSSerialPIN;   /**<TS serial PIN*/                           /**<CNcomment:ts���������*/
    HI_UNF_TUNER_DISEQCWAVE_MODE_E enDiSEqCWave;    /**<DiSEqC Wave Mode*/                        /**<CNcomment:DiSEqCģʽ*/
    HI_UNF_LNBCTRL_DEV_TYPE_E      enLNBCtrlDev;    /**<LNB power supply and control device*/     /**<CNcomment:LNB����оƬ*/
    HI_U16                         u16LNBDevAddress;/**<LNB control device address*/              /**<CNcomment:LNB����оƬi2c��ַ*/
} HI_UNF_TUNER_SAT_ATTR_S;

/** Polarization type*/
/** CNcomment:������ʽ*/
typedef enum hiUNF_TUNER_FE_POLARIZATION_E
{
    HI_UNF_TUNER_FE_POLARIZATION_H,         /**<Horizontal Polarization*/           /**<CNcomment:ˮƽ����*/
    HI_UNF_TUNER_FE_POLARIZATION_V,         /**<Vertical Polarization*/             /**<CNcomment:��ֱ����*/
    HI_UNF_TUNER_FE_POLARIZATION_L,         /**<Left-hand circular Polarization*/   /**<CNcomment:����Բ����*/
    HI_UNF_TUNER_FE_POLARIZATION_R,         /**<Right-hand circular Polarization*/  /**<CNcomment:����Բ����*/
    HI_UNF_TUNER_FE_POLARIZATION_BUTT,      /**<Invalid value*/                     /**<CNcomment:�Ƿ��߽�ֵ*/
} HI_UNF_TUNER_FE_POLARIZATION_E;

/** FEC Type*/
/** CNcomment:���Ǳ�׼*/
typedef enum hiUNF_TUNER_FE_FECTYPE_E
{
    HI_UNF_TUNER_FE_DVBS,                 /**<DVB-S*/                      /**<CNcomment:DVB-S��׼*/
    HI_UNF_TUNER_FE_DVBS2,                /**<DVB-S2*/                     /**<CNcomment:DVB-S2��׼*/
    HI_UNF_TUNER_FE_DIRECTV,              /**<DIRECTV*/                    /**<CNcomment:DIRECTV��׼*/
    HI_UNF_TUNER_FE_BUTT                  /**<Invalid value*/              /**<CNcomment:�Ƿ��߽�ֵ*/
} HI_UNF_TUNER_FE_FECTYPE_E;

/** FEC code Rate*/
/** CNcomment:FEC����*/
typedef enum hiUNF_TUNER_FE_FECRATE_E
{
    HI_UNF_TUNER_FE_FEC_AUTO = 0,
    HI_UNF_TUNER_FE_FEC_1_2,           /**<1/2*/           /**<CNcomment:1/2����*/
    HI_UNF_TUNER_FE_FEC_2_3,           /**<2/3*/           /**<CNcomment:2/3����*/
    HI_UNF_TUNER_FE_FEC_3_4,           /**<3/4*/           /**<CNcomment:3/4����*/
    HI_UNF_TUNER_FE_FEC_4_5,           /**<4/5*/           /**<CNcomment:4/5����*/
    HI_UNF_TUNER_FE_FEC_5_6,           /**<5/6*/           /**<CNcomment:5/6����*/
    HI_UNF_TUNER_FE_FEC_6_7,           /**<6/7*/           /**<CNcomment:6/7����*/
    HI_UNF_TUNER_FE_FEC_7_8,           /**<7/8*/           /**<CNcomment:7/8����*/
    HI_UNF_TUNER_FE_FEC_8_9,           /**<8/9*/           /**<CNcomment:8/9����*/
    HI_UNF_TUNER_FE_FEC_9_10,          /**<9/10*/          /**<CNcomment:9/10����*/
    HI_UNF_TUNER_FE_FEC_1_4,           /**<1/4*/           /**<CNcomment:1/4����*/
    HI_UNF_TUNER_FE_FEC_1_3,           /**<1/3*/           /**<CNcomment:1/3����*/
    HI_UNF_TUNER_FE_FEC_2_5,           /**<2/5*/           /**<CNcomment:2/5����*/
    HI_UNF_TUNER_FE_FEC_3_5,           /**<3/5*/           /**<CNcomment:3/5����*/
    HI_UNF_TUNER_FE_FECRATE_BUTT       /**<Invalid value*/ /**<CNcomment:�Ƿ��߽�ֵ*/
} HI_UNF_TUNER_FE_FECRATE_E;

/** LNB type*/
/** CNcomment:LNB����*/
typedef enum hiUNF_TUNER_FE_FE_LNBTYPE_E
{
    HI_UNF_TUNER_FE_LNB_SINGLE_FREQUENCY,   /**<Single LO frequency*/      /**<CNcomment:������*/
    HI_UNF_TUNER_FE_LNB_DUAL_FREQUENCY,     /**<Dual LO frequency*/        /**<CNcomment:˫����*/
    HI_UNF_TUNER_FE_LNB_UNICABLE,           /**<Unicable LNB */            /**<CNcomment:unicable��Ƶͷ*/
    HI_UNF_TUNER_FE_LNB_TYPE_BUTT           /**<Invalid value*/            /**<CNcomment:�Ƿ��߽�ֵ*/
} HI_UNF_TUNER_FE_LNB_TYPE_E;

/** LNB band type*/
/** CNcomment:�����ź�Ƶ��*/
typedef enum hiUNF_TUNER_FE_LNB_BAND_E
{
    HI_UNF_TUNER_FE_LNB_BAND_C,             /**<C */                    /**<CNcomment:C����*/
    HI_UNF_TUNER_FE_LNB_BAND_KU,            /**<Ku */                   /**<CNcomment:Ku����*/
    HI_UNF_TUNER_FE_LNB_BAND_BUTT           /**<Invalid value*/         /**<CNcomment:�Ƿ��߽�ֵ*/
} HI_UNF_TUNER_FE_LNB_BAND_E;

/** LNB power control*/
/** CNcomment:��Ƶͷ�������*/
typedef enum hiUNF_TUNER_FE_LNB_POWER_E
{
    HI_UNF_TUNER_FE_LNB_POWER_OFF,          /**<LNB power off*/                                                     /**<CNcomment:�ض�*/
    HI_UNF_TUNER_FE_LNB_POWER_ON,           /**<LNB power auto, 13V/18V, default*/                                  /**<CNcomment:Ĭ�ϵ�13/18V����*/
    HI_UNF_TUNER_FE_LNB_POWER_ENHANCED,     /**<LNB power auto, 14V/19V, some LNB control device can support.*/     /**<CNcomment:��ǿ����*/
    HI_UNF_TUNER_FE_LNB_POWER_BUTT          /**<Invalid value*/                                                     /**<CNcomment:�Ƿ��߽�ֵ*/
} HI_UNF_TUNER_FE_LNB_POWER_E;

/** LNB 22K tone status, for Ku band LNB*/
/** CNcomment:22k�ź�״̬������Ku˫����*/
typedef enum hiUNF_TUNER_FE_LNB_22K_E
{
    HI_UNF_TUNER_FE_LNB_22K_OFF,            /**<22k off*/                    /**<CNcomment:22k�źŹأ�ѡ��ͱ���*/
    HI_UNF_TUNER_FE_LNB_22K_ON,             /**<22k on*/                     /**<CNcomment:22k�źſ���ѡ��߱���*/
    HI_UNF_TUNER_FE_LNB_22K_BUTT            /**<Invalid value*/              /**<CNcomment:�Ƿ��߽�ֵ*/
} HI_UNF_TUNER_FE_LNB_22K_E;

/** 0/12V switch*/
/** CNcomment:0/12V����*/
typedef enum hiUNF_TUNER_SWITCH_0_12V_E
{
    HI_UNF_TUNER_SWITCH_0_12V_NONE,         /**< None, default*/            /**<CNcomment:���ӿ���״̬*/
    HI_UNF_TUNER_SWITCH_0_12V_0,            /**< 0V*/                       /**<CNcomment:0V״̬*/
    HI_UNF_TUNER_SWITCH_0_12V_12,           /**< 12V*/                      /**<CNcomment:12V״̬*/
    HI_UNF_TUNER_SWITCH_0_12V_BUTT          /**<Invalid value*/             /**<CNcomment:�Ƿ��߽�ֵ*/
} HI_UNF_TUNER_SWITCH_0_12V_E;

/** 22KHz switch*/
/** CNcomment:22K����*/
typedef enum hiUNF_TUNER_SWITCH_22K_E
{
    HI_UNF_TUNER_SWITCH_22K_NONE,           /**< None, default*/            /**<CNcomment:���ӿ���״̬*/
    HI_UNF_TUNER_SWITCH_22K_0,              /**< 0*/                        /**<CNcomment:0kHz�˿�*/
    HI_UNF_TUNER_SWITCH_22K_22,             /**< 22KHz*/                    /**<CNcomment:22kHz�˿�*/
    HI_UNF_TUNER_SWITCH_22K_BUTT            /**<Invalid value*/             /**<CNcomment:�Ƿ��߽�ֵ*/
} HI_UNF_TUNER_SWITCH_22K_E;

/** Tone burst switch*/
/** CNcomment:Tone burst����*/
typedef enum hiUNF_TUNER_SWITCH_TONEBURST_E
{
    HI_UNF_TUNER_SWITCH_TONEBURST_NONE,     /**< Don't send tone burst, default*/ /**<CNcomment:���ӿ���״̬*/
    HI_UNF_TUNER_SWITCH_TONEBURST_0,        /**< Tone burst 0*/                   /**<CNcomment:0 port*/
    HI_UNF_TUNER_SWITCH_TONEBURST_1,        /**< Tone burst 1*/                   /**<CNcomment:1 port*/
    HI_UNF_TUNER_SWITCH_TONEBURST_BUTT      /**<Invalid value*/                   /**<CNcomment:�Ƿ��߽�ֵ*/
} HI_UNF_TUNER_SWITCH_TONEBURST_E;

/** Structure of the satellite transmission signal.*/
/** CNcomment:����Satellite�����ź�*/
typedef struct  hiUNF_SAT_CONNECT_PARA_S
{
    HI_U32                         u32Freq;         /**<Downlink frequency, in kHz*/ /**<CNcomment:����Ƶ�ʣ���λ��kHz*/
    HI_U32                         u32SymbolRate;   /**<Symbol rate, in Symb/s*/     /**<CNcomment:����ʣ���λ��Symb/s */
    HI_UNF_TUNER_FE_POLARIZATION_E enPolar;         /**<Polarization type*/          /**<CNcomment:������ʽ*/
} HI_UNF_SAT_CONNECT_PARA_S;

/** Structure of the satellite transmission signal's detailed information.*/
/** CNcomment:�����ź���ϸ��Ϣ*/
typedef struct  hiUNF_TUNER_SAT_SIGNALINFO_S
{
    HI_U32                         u32Freq;         /**<Downlink frequency, in kHz*/              /**<CNcomment:����Ƶ�ʣ���λ��kHz*/
    HI_U32                         u32SymbolRate;   /**<Symbol rate, in Symb/s*/                  /**<CNcomment:����ʣ���λSymb/s*/
    HI_UNF_MODULATION_TYPE_E       enModType;       /**<Modulation type*/                         /**<CNcomment:���Ʒ�ʽ*/
    HI_UNF_TUNER_FE_POLARIZATION_E enPolar;         /**<Polarization type*/                       /**<CNcomment:������ʽ*/
    HI_UNF_TUNER_FE_FECTYPE_E      enSATType;       /**<Saterllite standard, DVB-S/S2/DIRECTV*/   /**<CNcomment:���Ǳ�׼�� ֧��DVB-S/S2/DIRECTV*/
    HI_UNF_TUNER_FE_FECRATE_E      enFECRate;       /**<FEC rate*/                                /**<CNcomment:ǰ���������*/
} HI_UNF_TUNER_SAT_SIGNALINFO_S;

/** PLP type of DVB-T2.*/
/** CNcomment:T2�������ܵ�����*/
typedef enum hiUNF_TUNER_T2_PLP_TYPE_E
{   
    HI_UNF_TUNER_T2_PLP_TYPE_COM=0,         /**<common type*/              /**<CNcomment:��ͨ*/
    HI_UNF_TUNER_T2_PLP_TYPE_DAT1,          /**<data1 type*/               /**<CNcomment:���1*/
    HI_UNF_TUNER_T2_PLP_TYPE_DAT2,          /**<data2 type*/               /**<CNcomment:���2*/
    HI_UNF_TUNER_T2_PLP_TYPE_BUTT           /**<Invalid value*/            /**<CNcomment:�Ƿ��߽�ֵ*/
} HI_UNF_TUNER_T2_PLP_TYPE_E;

/** Structure of the terrestrial transmission signal's detailed information.*/
/** CNcomment:�����ź���ϸ��Ϣ*/
typedef struct  hiUNF_TUNER_TER_SIGNALINFO_S
{
    HI_U32                         u32Freq;         /**<Frequency, in kHz*/          /**<CNcomment:Ƶ�ʣ���λ��kHz*/
    HI_U32                         u32BandWidth;    /**<Band width, in KHz*/         /**<CNcomment:��?��λKHz */
    HI_UNF_MODULATION_TYPE_E       enModType;       /**<Modulation type*/            /**<CNcomment:���Ʒ�ʽ*/
    HI_UNF_TUNER_FE_FECRATE_E      enFECRate;       /**<FEC rate*/                   /**<CNcomment:ǰ���������*/
    HI_UNF_TUNER_FE_GUARD_INTV_E enGuardIntv;       /**<GI mode*/                    /**<CNcomment:�������ģʽ*/
    HI_UNF_TUNER_FE_FFT_E enFFTMode;                /**<FFT mode*/                   /**<CNcomment:FFTģʽ*/
    HI_UNF_TUNER_FE_HIERARCHY_E enHierMod;          /**<Hierarchical Modulation and alpha, only used in DVB-T*/
                                                                                     /**<CNcomment:hierachicalģʽ��alphaֵ*/
    HI_UNF_TUNER_TS_PRIORITY_E enTsPriority;        /**<The TS priority, only used in DVB-T*/
                                                                                     /**<CNcomment:ts priority, ������DVB-Tģʽ��*/
} HI_UNF_TUNER_TER_SIGNALINFO_S;

/** signal information.*/
/** CNcomment:TUNER�ź�����*/
typedef struct hiUNF_TUNER_SIGNALINFO_S
{
    HI_UNF_TUNER_SIG_TYPE_E enSigType;              /**<Signal transmission type*/    /**<CNcomment:�ź�����*/

    union
    {
        HI_UNF_TUNER_SAT_SIGNALINFO_S stSat;        /**<Signal info of satellite*/    /**<CNcomment:�����ź���Ϣ*/
        HI_UNF_TUNER_TER_SIGNALINFO_S stTer;        /**<Signal info of terrestrial*/  /**<CNcomment:�����ź���Ϣ*/
    } unSignalInfo;
    int fill[6];
    //60
} HI_UNF_TUNER_SIGNALINFO_S;

/** unicable multi-switch port.*/
/** CNcomment:unicable���ض˿�ö��*/
typedef enum hiUNF_TUNER_SATPOSITION_E
{
    HI_UNF_TUNER_SATPOSN_A,    /**<unicable switch port A*/    /**<CNcomment:�˿�A*/
    HI_UNF_TUNER_SATPOSN_B,    /**<unicable switch port B*/    /**<CNcomment:�˿�B*/
    HI_UNF_TUNER_SATPOSN_BUT   /**<Invalid value*/             /**<CNcomment:�Ƿ��߽�ֵ*/
} HI_UNF_TUNER_SATPOSITION_E;

/** LNB configurating parameters*/
/** CNcomment:LNB���ò���*/
typedef struct hiUNF_TUNER_FE_LNB_CONFIG_S
{
    HI_UNF_TUNER_FE_LNB_TYPE_E  enLNBType;      /**<LNB type*/                              /**<CNcomment:LNB����*/
    HI_U32                      u32LowLO;       /**< Low Local Oscillator Frequency, MHz */ /**<CNcomment:LNB�ͱ���Ƶ�ʣ���λMHz*/
    HI_U32                      u32HighLO;      /**< High Local Oscillator Frequency, MHz*/ /**<CNcomment:LNB�߱���Ƶ�ʣ���λMHz*/
    HI_UNF_TUNER_FE_LNB_BAND_E  enLNBBand;      /**< LNB band, C or Ku */                   /**<CNcomment:LNB���Σ�C��Ku*/
    HI_U8 u8UNIC_SCRNO;                         /**< SCR number, 0-7 */                     /**<CNcomment:SCR��ţ�ȡֵΪ0-7*/
    HI_U32 u32UNICIFFreqMHz;                    /**< SCR IF frequency, unit MHz */          /**<CNcomment:SCR��ƵƵ�ʣ� ��λMHz*/
    HI_UNF_TUNER_SATPOSITION_E enSatPosn;       /**< unicable multi-switch port */          /**<CNcomment:unicable���ض˿ں�*/
} HI_UNF_TUNER_FE_LNB_CONFIG_S;

/** tuner attribution.*/
/** CNcomment:TUNER����*/
typedef struct  hiTUNER_ATTR_S
{
    HI_UNF_TUNER_SIG_TYPE_E	enSigType ;         /**<Signal type*/                             /**<CNcomment:�ź�����*/
    HI_UNF_TUNER_DEV_TYPE_E	enTunerDevType;     /**<Tuner type*/                                  /**<CNcomment:TUNER����*/
    HI_U32	u32TunerAddr;                                      /*The i2c address of tuner, if demod type is 3130I,3130E or J83B, you cannot set it*/
    HI_UNF_DEMOD_DEV_TYPE_E	enDemodDevType;     /**<QAM type*/                                    /**<CNcomment:QAM����*/
    HI_U32	u32DemodAddr;                                   /*The i2c address of demod, if demod type is 3130I,3130E or J83B, you cannot set it*/
    HI_UNF_TUNER_OUPUT_MODE_E	enOutputMode ;      /**<Output mode of transport streams (TSs)*/  /**<CNcomment:TS�����ģʽ*/
    HI_U8	enI2cChannel;       /**<I2C channel used by the tuner*/                               /**<CNcomment:TUNERʹ�õ�I2Cͨ��*/
    HI_U32	u32ResetGpioNo;  /**<Demod reset GPIO NO.*/                    /**<CNcomment:��λʹ�õ�gpio�ܽź�*/
} HI_UNF_TUNER_ATTR_S ;

/** Frequency locking parameters of the tuner*/
/** CNcomment:TUNER��Ƶ����*/
typedef struct  hiUNF_TUNER_CONNECT_PARA_S
{
    HI_UNF_TUNER_SIG_TYPE_E enSigType ;        /**<Signal type*/                      /**<CNcomment:�ź�����*/

    union
    {
        HI_UNF_CAB_CONNECT_PARA_S   stCab ;   /**<Cable transmission signal*/         /**<CNcomment:CABLE�����ź�*/
        HI_UNF_TER_CONNECT_PARA_S stTer ;       /**<Terrestrial transmission signal*/ /**<CNcomment:���洫���ź�*/
        HI_UNF_SAT_CONNECT_PARA_S stSat;        /**<Satellite transmission signal*/   /**<CNcomment:���Ǵ����ź�*/
    } unConnectPara;
} HI_UNF_TUNER_CONNECT_PARA_S ;

/** Frequency locking status and parameters of the tuner*/
/** CNcomment:TUNER��Ƶ״̬����Ƶ����*/
typedef struct  hiUNF_TUNER_STATUS_S
{
    HI_UNF_TUNER_LOCK_STATUS_E  enLockStatus ;  /**<Frequency locking status*/               /**<CNcomment:��Ƶ״̬*/
    HI_UNF_TUNER_CONNECT_PARA_S stConnectPara;  /**<Actual frequency locking parameters*/    /**<CNcomment:ʵ����Ƶ����*/
} HI_UNF_TUNER_STATUS_S ;

/** Tuner blind scan type*/
/** CNcomment:TUNERäɨ��ʽ*/
typedef enum hiUNF_TUNER_BLINDSCAN_MODE_E
{
    HI_UNF_TUNER_BLINDSCAN_MODE_AUTO = 0,       /**<Blind scan automatically*/  /**<CNcomment:�Զ�ɨ��*/
    HI_UNF_TUNER_BLINDSCAN_MODE_MANUAL,         /**<Blind scan manually*/       /**<CNcomment:�ֶ�ɨ��*/
    HI_UNF_TUNER_BLINDSCAN_MODE_BUTT            /**<Invalid value*/             /**<CNcomment:�Ƿ��߽�ֵ*/
} HI_UNF_TUNER_BLINDSCAN_MODE_E;

/** Definition of blind scan event type*/
/** CNcomment:TUNERäɨ�¼�*/
typedef enum hiUNF_TUNER_BLINDSCAN_EVT_E
{
    HI_UNF_TUNER_BLINDSCAN_EVT_STATUS,          /**<New status*/        /**<CNcomment:״̬�仯*/
    HI_UNF_TUNER_BLINDSCAN_EVT_PROGRESS,        /**<New Porgress */     /**<CNcomment:��ȱ仯*/
    HI_UNF_TUNER_BLINDSCAN_EVT_NEWRESULT,       /**<Find new channel*/  /**<CNcomment:��Ƶ��*/
    HI_UNF_TUNER_BLINDSCAN_EVT_BUTT             /**<Invalid value*/     /**<CNcomment:�Ƿ��߽�ֵ*/
} HI_UNF_TUNER_BLINDSCAN_EVT_E;

/** Definition of tuner blind scan status*/
/** CNcomment:TUNERäɨ״̬*/
typedef enum hiUNF_TUNER_BLINDSCAN_STATUS_E
{
    HI_UNF_TUNER_BLINDSCAN_STATUS_IDLE,         /**<Idel*/              /**<CNcomment:����*/
    HI_UNF_TUNER_BLINDSCAN_STATUS_SCANNING,     /**<Scanning*/          /**<CNcomment:ɨ����*/
    HI_UNF_TUNER_BLINDSCAN_STATUS_FINISH,       /**<Finish*/            /**<CNcomment:�ɹ����*/
    HI_UNF_TUNER_BLINDSCAN_STATUS_QUIT,         /**<User quit*/         /**<CNcomment:�û��˳�*/
    HI_UNF_TUNER_BLINDSCAN_STATUS_FAIL,         /**<Scan fail*/         /**<CNcomment:ɨ��ʧ��*/
    HI_UNF_TUNER_BLINDSCAN_STATUS_BUTT          /**<Invalid value*/     /**<CNcomment:�Ƿ��߽�ֵ*/
} HI_UNF_TUNER_BLINDSCAN_STATUS_E;

/** Structure of satellite TP*/
/** CNcomment:TUNERɨ��TP��Ϣ*/
typedef struct  hiUNF_TUNER_SAT_TPINFO_S
{
    HI_U32                         u32Freq;         /**<Downlink frequency, in kHz*/ /**<CNcomment:����Ƶ�ʣ���λ��kHz*/
    HI_U32                         u32SymbolRate;   /**<Symbol rate, in bit/s*/      /**<CNcomment:����ʣ���λbps */
    HI_UNF_TUNER_FE_POLARIZATION_E enPolar;         /**<Polarization type*/          /**<CNcomment:������ʽ*/
    HI_U8 cbs_reliablity;                           /**<TP reliability*/             /**<CNcomment:TP�Ŀɿ���*/
} HI_UNF_TUNER_SAT_TPINFO_S;

/** Notify structure of tuner blind scan */
/** CNcomment:TUNERäɨ֪ͨ��Ϣ*/
typedef union hiUNF_TUNER_BLINDSCAN_NOTIFY_U
{
    HI_UNF_TUNER_BLINDSCAN_STATUS_E* penStatus;             /**<Scanning status*/            /**<CNcomment:äɨ״̬*/
    HI_U16*                          pu16ProgressPercent;   /**<Scanning progress*/          /**<CNcomment:äɨ���*/
    HI_UNF_TUNER_SAT_TPINFO_S* pstResult;                   /**<Scanning result*/            /**<CNcomment:äɨ���*/
} HI_UNF_TUNER_BLINDSCAN_NOTIFY_U;

/** Parameter of the satellite tuner blind scan */
/** CNcomment:����TUNERäɨ����*/
typedef struct hiUNF_TUNER_SAT_BLINDSCAN_PARA_S
{
    /**<LNB Polarization type, only take effect in manual blind scan mode*/
    /**<CNcomment:LNB������ʽ���Զ�ɨ��ģʽ������Ч*/
    HI_UNF_TUNER_FE_POLARIZATION_E enPolar;
    
    /**<LNB 22K signal status, for Ku band LNB which has dual LO, 22K ON will select high LO and 22K off select low LO,
        only take effect in manual blind scan mode*/
    /**<CNcomment:LNB 22K״̬������Ku����˫����LNB��ONѡ��߱���OFFѡ��ͱ����Զ�ɨ��ģʽ������Ч*/
    HI_UNF_TUNER_FE_LNB_22K_E      enLNB22K;

    /**<Blind scan start IF, in kHz, only take effect in manual blind scan mode */
    /**<CNcomment:äɨ��ʼƵ��(��Ƶ)����λ��kHz���Զ�ɨ��ģʽ������Ч*/
    HI_U32                         u32StartFreq;   

    /**<Blind scan stop IF, in kHz, only take effect in manual blind scan mode */
    /**<CNcomment:äɨ����Ƶ��(��Ƶ)����λ��kHz���Զ�ɨ��ģʽ������Ч*/
    HI_U32                         u32StopFreq; 

    /**<The execution of the blind scan may change the 13/18V or 22K status. 
        If you use any DiSEqC device which need send command when 13/18V or 22K status change,
        you should registe a callback here. Otherwise, you can set NULL here.*/
    /**<CNcomment:äɨ��̿��ܻ��л�������ʽ��22K�����������ĳЩDiSEqC�豸��Ҫ����13/18V��22K�ģ�
        ��ע������ص������û���ã���ɴ�NULL */
    HI_VOID (*pfnDISEQCSet)(HI_U32 u32TunerId, HI_UNF_TUNER_FE_POLARIZATION_E enPolar,
                            HI_UNF_TUNER_FE_LNB_22K_E enLNB22K);

    /**<Callback when scan status change, scan progress change or find new channel.*/
    /**<CNcomment:ɨ��״̬���Ȱٷֱȷ���仯ʱ�������µ�Ƶ��ʱ�ص�*/
    HI_VOID (*pfnEVTNotify)(HI_U32 u32TunerId, HI_UNF_TUNER_BLINDSCAN_EVT_E enEVT, HI_UNF_TUNER_BLINDSCAN_NOTIFY_U * punNotify);
} HI_UNF_TUNER_SAT_BLINDSCAN_PARA_S;

/** Parameter of the tuner blind scan */
/** CNcomment:TUNERäɨ����*/
typedef struct hiUNF_TUNER_BLINDSCAN_PARA_S
{
    HI_UNF_TUNER_BLINDSCAN_MODE_E enMode;            /**<Scanning mode*/                /**<CNcomment:äɨģʽ*/
    union
    {
        HI_UNF_TUNER_SAT_BLINDSCAN_PARA_S stSat;    /**<Scanning parameter*/            /**<CNcomment:äɨ����*/
    } unScanPara;
} HI_UNF_TUNER_BLINDSCAN_PARA_S;


/** DiSEqC Level*/
/** CNcomment:DiSEqC�豸�汾*/
typedef enum hiUNF_TUNER_DISEQC_LEVEL_E
{
    HI_UNF_TUNER_DISEQC_LEVEL_1_X,      /**<1.x, one way*/                  /**<CNcomment:1.x������ */
    HI_UNF_TUNER_DISEQC_LEVEL_2_X,      /**<2.x, two way, support reply*/   /**<CNcomment:2.x��˫��֧��Reply*/
    HI_UNF_TUNER_DISEQC_LEVEL_BUTT      /**<Invalid value*/                 /**<CNcomment:�Ƿ��߽�ֵ*/
} HI_UNF_TUNER_DISEQC_LEVEL_E;

/** Receive status of DiSEqC reply massage*/
/** CNcomment:DiSEqC��Ϣ����״̬*/
typedef enum hiUNF_TUNER_DISEQC_RECV_STATUS_E
{
    HI_UNF_TUNER_DISEQC_RECV_OK,        /**<Receive successfully*/          /**<CNcomment:���ճɹ�*/
    HI_UNF_TUNER_DISEQC_RECV_UNSUPPORT, /**<Device don't support reply*/    /**<CNcomment:�豸��֧�ֻش�*/    
    HI_UNF_TUNER_DISEQC_RECV_TIMEOUT,   /**<Receive timeout*/               /**<CNcomment:���ճ�ʱ*/
    HI_UNF_TUNER_DISEQC_RECV_ERROR,     /**<Receive fail*/                  /**<CNcomment:���ճ���*/
    HI_UNF_TUNER_DISEQC_RECV_BUTT       /**<Invalid value*/                 /**<CNcomment:�Ƿ��߽�ֵ*/
} HI_UNF_TUNER_DISEQC_RECV_STATUS_E;

/** Structure of the DiSEqC send massage */
/** CNcomment:DiSEqC������Ϣ�ṹ*/
typedef struct hiUNF_TUNER_DISEQC_SENDMSG_S
{
    HI_UNF_TUNER_DISEQC_LEVEL_E     enLevel;        /**<Device level*/                         /**<CNcomment:�����汾*/
    HI_UNF_TUNER_SWITCH_TONEBURST_E enToneBurst;    /**<Tone Burst */                          /**<CNcomment:tone�ź�״̬*/
    HI_U8                           au8Msg[HI_UNF_DISEQC_MSG_MAX_LENGTH]; /**<Message data*/   /**<CNcomment:��Ϣ��*/
    HI_U8                           u8Length;       /**<Message length*/                       /**<CNcomment:��Ϣ����*/
    HI_U8                           u8RepeatTimes;  /**<Message repeat times*/                 /**<CNcomment:�ش�����*/
} HI_UNF_TUNER_DISEQC_SENDMSG_S;

/** Structure of the DiSEqC reply massage */
/** CNcomment:DiSEqC������Ϣ�ṹ*/
typedef struct hiUNF_TUNER_DISEQC_RECVMSG_S
{
    HI_UNF_TUNER_DISEQC_RECV_STATUS_E enStatus;     /**<Recieve status*/                                 /**<CNcomment:����״̬*/
    HI_U8                             au8Msg[HI_UNF_DISEQC_MSG_MAX_LENGTH]; /**<Recieve message data*/   /**<CNcomment:������ݻ���*/
    HI_U8                             u8Length;     /**<Recieve message length*/                         /**<CNcomment:������ݳ���*/
} HI_UNF_TUNER_DISEQC_RECVMSG_S;

/** DiSEqC Switch port*/
/** CNcomment:DiSEqC���ض˿�ö��*/
typedef enum hiUNF_TUNER_DISEQC_SWITCH_PORT_E
{
    HI_UNF_TUNER_DISEQC_SWITCH_NONE = 0,         /**<none*/                                 /**<CNcomment:���ӿ���*/
    HI_UNF_TUNER_DISEQC_SWITCH_PORT_1,           /**<port1*/                                /**<CNcomment:�˿�1*/
    HI_UNF_TUNER_DISEQC_SWITCH_PORT_2,           /**<port2*/                                /**<CNcomment:�˿�2*/
    HI_UNF_TUNER_DISEQC_SWITCH_PORT_3,           /**<port3*/                                /**<CNcomment:�˿�3*/
    HI_UNF_TUNER_DISEQC_SWITCH_PORT_4,           /**<port4*/                                /**<CNcomment:�˿�4*/
    HI_UNF_TUNER_DISEQC_SWITCH_PORT_5,           /**<port5*/                                /**<CNcomment:�˿�5*/
    HI_UNF_TUNER_DISEQC_SWITCH_PORT_6,           /**<port6*/                                /**<CNcomment:�˿�6*/
    HI_UNF_TUNER_DISEQC_SWITCH_PORT_7,           /**<port7*/                                /**<CNcomment:�˿�7*/
    HI_UNF_TUNER_DISEQC_SWITCH_PORT_8,           /**<port8*/                                /**<CNcomment:�˿�8*/
    HI_UNF_TUNER_DISEQC_SWITCH_PORT_9,           /**<port9*/                                /**<CNcomment:�˿�9*/
    HI_UNF_TUNER_DISEQC_SWITCH_PORT_10,          /**<port10*/                               /**<CNcomment:�˿�10*/
    HI_UNF_TUNER_DISEQC_SWITCH_PORT_11,          /**<port11*/                               /**<CNcomment:�˿�11*/
    HI_UNF_TUNER_DISEQC_SWITCH_PORT_12,          /**<port12*/                               /**<CNcomment:�˿�12*/
    HI_UNF_TUNER_DISEQC_SWITCH_PORT_13,          /**<port13*/                               /**<CNcomment:�˿�13*/   
    HI_UNF_TUNER_DISEQC_SWITCH_PORT_14,          /**<port14*/                               /**<CNcomment:�˿�14*/
    HI_UNF_TUNER_DISEQC_SWITCH_PORT_15,          /**<port15*/                               /**<CNcomment:�˿�15*/
    HI_UNF_TUNER_DISEQC_SWITCH_PORT_16,          /**<port16*/                               /**<CNcomment:�˿�16*/
    HI_UNF_TUNER_DISEQC_SWITCH_PORT_BUTT         /**<Invalid value*/                        /**<CNcomment:�Ƿ��߽�ֵ*/
} HI_UNF_TUNER_DISEQC_SWITCH_PORT_E;

/** Parameter for DiSEqC 1.0/2.0 switch
   Some DiSEqC device need set polarization(13/18V) and 22K, you can set them here. */
/** CNcomment:DiSEqC 1.0/2.0 ���ز���
   ��ЩDiSEqC�豸��Ҫ���ü�����ʽ��22K�ģ�������������豸����Ҫ���������� */
typedef struct hiUNF_TUNER_DISEQC_SWITCH4PORT_S
{
    HI_UNF_TUNER_DISEQC_LEVEL_E       enLevel;  /**<DiSEqC device level*/           /**<CNcomment:�����汾*/
    HI_UNF_TUNER_DISEQC_SWITCH_PORT_E enPort;   /**<DiSEqC switch port*/            /**<CNcomment:ѡͨ�˿ں�*/
    HI_UNF_TUNER_FE_POLARIZATION_E    enPolar;  /**<Polarization type */            /**<CNcomment:������ʽ*/
    HI_UNF_TUNER_FE_LNB_22K_E         enLNB22K; /**<22K status*/                    /**<CNcomment:22k״̬*/
} HI_UNF_TUNER_DISEQC_SWITCH4PORT_S;

/** Parameter for DiSEqC 1.1/2.1 switch */
/** CNcomment:DiSEqC 1.1/2.1 ���ز��� */
typedef struct hiUNF_TUNER_DISEQC_SWITCH16PORT_S
{
    HI_UNF_TUNER_DISEQC_LEVEL_E       enLevel;  /**<DiSEqC device level*/            /**<CNcomment:�����汾*/
    HI_UNF_TUNER_DISEQC_SWITCH_PORT_E enPort;   /**<DiSEqC switch port*/             /**<CNcomment:ѡͨ�˿ں�*/
} HI_UNF_TUNER_DISEQC_SWITCH16PORT_S;

/** DiSEqC motor limit setting*/
/** CNcomment:DiSEqC��Ｋ������*/
typedef enum hiUNF_TUNER_DISEQC_LIMIT_E
{
    HI_UNF_TUNER_DISEQC_LIMIT_OFF,              /**<Disable Limits*/                /**<CNcomment:������*/
    HI_UNF_TUNER_DISEQC_LIMIT_EAST,             /**<Set East Limit*/                /**<CNcomment:��������*/
    HI_UNF_TUNER_DISEQC_LIMIT_WEST,             /**<Set West Limit*/                /**<CNcomment:��������*/
    HI_UNF_TUNER_DISEQC_LIMIT_BUTT              /**<Invalid value*/                 /**<CNcomment:�Ƿ��߽�ֵ*/
} HI_UNF_TUNER_DISEQC_LIMIT_E;

/** Difinition of DiSEqC motor move direction*/
/** CNcomment:DiSEqC����ƶ�����*/
typedef enum hiUNF_TUNER_DISEQC_MOVE_DIR_E
{
    HI_UNF_TUNER_DISEQC_MOVE_DIR_EAST,          /**<Move east*/                     /**<CNcomment:���ƶ�*/
    HI_UNF_TUNER_DISEQC_MOVE_DIR_WEST,          /**<Move west*/                     /**<CNcomment:�����ƶ�*/
    HI_UNF_TUNER_DISEQC_MOVE_DIR_BUTT           /**<Invalid value*/                 /**<CNcomment:�Ƿ��߽�ֵ*/
} HI_UNF_TUNER_DISEQC_MOVE_DIR_E;

/** Difinition of DiSEqC motor move type*/
/** CNcomment:DiSEqC����ƶ���ʽ*/
typedef enum hiUNF_TUNER_DISEQC_MOVE_TYPE_E
{
    HI_UNF_TUNER_DISEQC_MOVE_STEP_SLOW,         /**<1 step one time, default*/      /**<CNcomment:�����ƶ�*/
    HI_UNF_TUNER_DISEQC_MOVE_STEP_FAST,         /**<5 step one time*/               /**<CNcomment:�����ƶ�*/
    HI_UNF_TUNER_DISEQC_MOVE_CONTINUE,          /**<Continuous moving*/             /**<CNcomment:�����ƶ�*/
    HI_UNF_TUNER_DISEQC_MOVE_TYPE_BUTT          /**<Invalid value*/                 /**<CNcomment:�Ƿ��߽�ֵ*/
} HI_UNF_TUNER_DISEQC_MOVE_TYPE_E;

/** Parameter for DiSEqC motor store position*/
/** CNcomment:���ߴ洢λ�ò���*/
typedef struct hiUNF_TUNER_DISEQC_POSITION_S
{
    HI_UNF_TUNER_DISEQC_LEVEL_E enLevel;        /**<DiSEqC device level*/               /**<CNcomment:�����汾*/
    HI_U32                      u32Pos;         /**<Index of position, 0-255*/          /**<CNcomment:λ�����*/
} HI_UNF_TUNER_DISEQC_POSITION_S;

/** Parameter for DiSEqC motor limit setting*/
/** CNcomment:����Limit���ò���*/
typedef struct hiUNF_TUNER_DISEQC_LIMIT_S
{
    HI_UNF_TUNER_DISEQC_LEVEL_E enLevel;        /**<DiSEqC device level*/               /**<CNcomment:�����汾*/
    HI_UNF_TUNER_DISEQC_LIMIT_E enLimit;        /**<Limit setting*/                     /**<CNcomment:�����趨*/
} HI_UNF_TUNER_DISEQC_LIMIT_S;

/** Parameter for DiSEqC motor moving*/
/** CNcomment:DiSEqC����ƶ�����*/
typedef struct hiUNF_TUNER_DISEQC_MOVE_S
{
    HI_UNF_TUNER_DISEQC_LEVEL_E     enLevel;    /**<DiSEqC device level*/           /**<CNcomment:�����汾*/
    HI_UNF_TUNER_DISEQC_MOVE_DIR_E  enDir;      /**<Moving direction*/              /**<CNcomment:�ƶ�����*/
    HI_UNF_TUNER_DISEQC_MOVE_TYPE_E enType;     /**<Moving type*/                   /**<CNcomment:�ƶ�����(���٣����٣�����)*/
} HI_UNF_TUNER_DISEQC_MOVE_S;

/** Parameter for DiSEqC motor recalculate*/
/** CNcomment:DiSEqC�����ؼ������*/
typedef struct hiUNF_TUNER_DISEQC_RECALCULATE_S
{
    HI_UNF_TUNER_DISEQC_LEVEL_E enLevel;        /**<DiSEqC device level*/           /**<CNcomment:�����汾*/
    HI_U8                       u8Para1;        /**<Parameter 1*/                   /**<CNcomment:����1*/
    HI_U8                       u8Para2;        /**<Parameter 2*/                   /**<CNcomment:����2*/
    HI_U8                       u8Para3;        /**<Parameter 3*/                   /**<CNcomment:����3*/
    HI_U8                       u8Reserve;      /**<Reserve*/                       /**<CNcomment:��������*/
} HI_UNF_TUNER_DISEQC_RECALCULATE_S;

/** Parameter for USALS*/
/** CNcomment:USALS ����*/
typedef struct hiUNF_TUNER_DISEQC_USALS_PARA_S
{
    HI_U16 u16LocalLongitude;   /**<local longitude, is 10*longitude, in param, E:0-1800, W:1800-3600(3600-longtitude)*/
                                /**<CNcomment:���ؾ��ȣ���λ0.1�ȣ�����ȡֵ��Χ0-1800������ȡֵ��Χ1800-3600��ֵΪ3600-����ֵ*/
    HI_U16 u16LocalLatitude;    /**<local latitude, is 10*latitude, in param N:0-900, S:900-1800(1800-latitude)*/
                                /**<CNcomment:����γ�ȣ���λ0.1�ȣ���γȡֵ��Χ0-900����γȡֵ��Χ900-1800��ֵΪ1800-γ��ֵ*/
    HI_U16 u16SatLongitude;     /**<sat longitude, is 10*longitude, in param, E:0-1800, W:1800-3600(3600-longtitude)*/
                                /**<CNcomment:���Ǿ��ȣ���λ0.1�ȣ�����ȡֵ��Χ0-1800������ȡֵ��Χ1800-3600��ֵΪ3600-����ֵ*/
    HI_U16 u16Angular;          /**<calculate result, out param*/
                                /**<CNcomment:�������������*/
} HI_UNF_TUNER_DISEQC_USALS_PARA_S;

/** Parameter for USALS goto angular*/
/** CNcomment:USALS�ǶȲ���*/
typedef struct hiUNF_TUNER_DISEQC_USALS_ANGULAR_S
{
    HI_UNF_TUNER_DISEQC_LEVEL_E enLevel;        /**<DiSEqC device level*/                                       /**<CNcomment:�����汾*/
    HI_U16                      u16Angular;     /**<Angular, calculated by HI_UNF_TUNER_DISEQC_CalcAngular()*/  /**<CNcomment:�Ƕ�ֵ����ͨ����HI_UNF_TUNER_DISEQC_CalcAngular����õ�*/
} HI_UNF_TUNER_DISEQC_USALS_ANGULAR_S;
//#endif /* DISEQC_SUPPORT */

/** @} */  /** <!-- ==== Structure Definition end ==== */


/******************************* API Declaration *****************************/
/** \addtogroup      FRONTEND */
/** @{ */  /** <!-- [FRONTEND] */
/*---TUNER---*/
/** 
\brief Initializes the tuner.
CNcomment:\brief ��ʼ��TUNER�豸��CNend

\attention \n
The error code HI_SUCCESS is returned if this API is called repeatedly.
CNcomment:֧���ظ���ʼ��TUNER�豸����ʱֱ�ӷ��سɹ���CNend
\param N/A                                    CNcomment:�� CNend
\retval ::HI_SUCCESS Success                  CNcomment:�ɹ� CNend
\retval ::HI_FAILURE  Calling this API fails. CNcomment:APIϵͳ����ʧ�� CNend

\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_Init(HI_VOID);


/** 
\brief Deinitializes the tuner.
CNcomment:\brief ȥ��ʼ��TUNER�豸��CNend

\attention \n
The error code HI_SUCCESS is returned if this API is called repeatedly.
CNcomment:֧���ظ�ȥ��ʼ��TUNER�豸����ʱֱ�ӷ��سɹ���CNend
\param  N/A                    CNcomment:�� CNend
\retval ::HI_SUCCESS Success   CNcomment:�ɹ� CNend
\retval ::HI_FAILURE  Calling this API fails. CNcomment:APIϵͳ����ʧ�� CNend

\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_DeInit(HI_VOID);

/**
\brief  Obtains the default attributes of the tuner. 
CNcomment:\brief  ��ȡTUNER��Ĭ�����ԡ�CNend
\attention \n
This API is available only after the tuner is initialized.
CNcomment:�˽ӿ���TUNER�豸��ʼ�������ʹ�á�CNend
\param[in] u32tunerId   tuner port ID. The port ID can be 0-2.             CNcomment:TUNER�˿ںţ�ȡֵΪ0-2��CNend
\param[out] pstTunerAttr The default attributes of the tuner are returned. CNcomment:���ص�ǰTUNER��Ĭ�����ԡ�CNend
\retval ::HI_SUCCESS Success.									   CNcomment: �ɹ� CNend
\retval ::HI_FAILURE Calling this API fails.                                               CNcomment: APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_TUNER_NOT_INIT  The Frontend module is not initialized. CNcomment: ģ��û�г�ʼ��CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:����PORT ����Ƿ� CNend
\retval ::HI_ERR_TUNER_INVALID_POINT The input parameter is invalid. CNcomment:����PORT ����Ƿ� CNend
\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_GetDeftAttr(HI_U32  u32tunerId , HI_UNF_TUNER_ATTR_S *pstTunerAttr );


/** 
\brief Sets the current attributes of the tuner.
CNcomment:\brief ����TUNER��ǰ���ԡ�CNend
\attention \n
N/A
\param[in] u32tunerId   tuner port ID. The port ID can be 0-2.      CNcomment:TUNER�˿ںţ�ȡֵΪ0-2 CNend
\param[in] pstTunerAttr tuner attributes                            CNcomment:TUNER�����ԡ�CNend
\retval ::HI_SUCCESS Success                                        CNcomment:�ɹ� CNend
\retval ::HI_FAILURE  Calling this API fails.                       CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_TUNER_NOT_OPEN  The TUNER module is not opened. CNcomment: ģ��û�д�CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_INVALID_POINT The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_INVALID_PARA The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_FAILED_SELECTI2CCHANNEL The input I2C channel is invalid. CNcomment:����I2C ͨ���Ƿ� CNend
\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_SetAttr(HI_U32  u32tunerId , const HI_UNF_TUNER_ATTR_S *pstTunerAttr );


/** 
\brief While using satellite channel, sets the current attributes of the tuner.
CNcomment:\brief ��������TUNER�������ԡ�CNend
\attention \n
N/A
\param[in] u32tunerId   tuner port ID. The port ID can be 0-2.      CNcomment:����TUNER�˿ںţ�ȡֵΪ0-2 CNend
\param[in] pstSatTunerAttr tuner attributes                         CNcomment:����TUNER�ĸ������ԡ�CNend
\retval ::HI_SUCCESS Success                                        CNcomment:�ɹ� CNend
\retval ::HI_FAILURE  Calling this API fails.                       CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_TUNER_NOT_OPEN  The TUNER module is not opened. CNcomment: ģ��û�д�CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_INVALID_POINT The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_INVALID_PARA The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_FAILED_SETSATATTR Tuner TUNER_SETSATATTR_CMD error. CNcomment:��������ʧ�� CNend
\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_SetSatAttr(HI_U32	u32tunerId , const HI_UNF_TUNER_SAT_ATTR_S *pstSatTunerAttr);

/** 
\brief Obtains the attributes of the tuner.
CNcomment:\brief ��ȡTUNER�����ԡ�CNend
\attention \n
N/A
This API is available only after the tuner is initialized.
CNcomment:�˽ӿ���TUNER�豸��ʼ�������ʹ�á�CNend

\param[in] u32tunerId  tuner port ID. The port ID can be 0-2.                CNcomment:TUNER�˿ںţ�ȡֵΪ0-2 CNend
\param[out] pstTunerAttr   The current attributes of the tuner are returned. CNcomment:���ص�ǰTUNER�����ԡ�CNend
\retval ::HI_SUCCESS Success                                                 CNcomment:�ɹ� CNend
\retval ::HI_FAILURE  Calling this API fails.                                CNcomment: APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_TUNER_NOT_INIT  The TUNER module is not initialized. CNcomment: ģ��û�г�ʼ��CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_INVALID_POINT The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_GetAttr(HI_U32  u32tunerId , HI_UNF_TUNER_ATTR_S *pstTunerAttr );


/** 
\brief Starts the tuner.
CNcomment:\brief ��TUNER�豸��CNend

\attention \n
The error code HI_SUCCESS is returned if this API is called repeatedly.
CNcomment:֧���ظ���TUNER�豸����ʱ��ֱ�ӷ��سɹ���CNend

\param[in] u32tunerId   tuner port ID. The port ID can be 0-2.      CNcomment:TUNER�˿ںţ�ȡֵΪ0-2 CNend
\retval ::HI_SUCCESS Success                                        CNcomment:�ɹ� CNend
\retval ::HI_FAILURE Calling this API fails.                        CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_TUNER_NOT_INIT  The TUNER module is not initialized. CNcomment: ģ��û�г�ʼ��CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_FAILED_INIT  Opening The TUNER module  fails. CNcomment:���豸ʧ��CNend
\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_Open (HI_U32    u32tunerId);


/** 
\brief Stops the tuner.
CNcomment:\brief �ر�TUNER�豸��CNend

\attention \n
The error code HI_SUCCESS is returned if this API is called repeatedly.
CNcomment:֧���ظ��ر�TUNER�豸����ʱ��ֱ�ӷ��سɹ���CNend

\param[in] u32tunerId  tuner port ID. The port ID can be 0-2.     CNcomment:TUNER�˿ںţ�ȡֵΪ0-2 CNend
\retval ::HI_SUCCESS Success                                      CNcomment:�ɹ� CNend
\retval ::HI_FAILURE  Calling this API fails.                     CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_TUNER_NOT_INIT  The TUNER module is not initialized. CNcomment: ģ��û�г�ʼ��CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:���� ����Ƿ� CNend

\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_Close(HI_U32    u32tunerId);


/** 
\brief Locks the frequency of the tuner.
CNcomment:\brief TUNER��Ƶ��CNend
\attention \n
N/A
\param[in] u32tunerId  tuner port ID. The port ID can be 0-2.             CNcomment:TUNER�˿ںţ�ȡֵΪ0-2 CNend
\param[in] pstConnectPara   Information about the frequency of the tuner  CNcomment:TUNER��Ƶ����Ϣ��CNend
\param[in] u32TimeOut  Wait timeout (in ms) when the frequency of the tuner \n
is locked. The value 0 indicates no wait, and any other value indicates the \n
maximum wait period.  
                                                                          CNcomment:TUNER����ȴ�ʱʱ�䣬0Ϊ���ȴ�����Ϊ��ȴ�ʱ�䣬��λms��CNend
\retval ::HI_SUCCESS Success                                              CNcomment:�ɹ� CNend
\retval ::HI_FAILURE  Calling this API fails.                             CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_TUNER_NOT_OPEN  The TUNER module is not opened. CNcomment: ģ��û�д�CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_INVALID_POINT The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_INVALID_PARA The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_FAILED_CONNECT The signal is locked unsuccessfully . CNcomment:��Ƶ��ʧ��CNend

\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_Connect(HI_U32  u32tunerId , const HI_UNF_TUNER_CONNECT_PARA_S  *pstConnectPara,HI_U32 u32TimeOut);


/**
\brief  set TS out.CNcomment:����TS��� CNend
\attention \n
\Please call this API to set TS out. \n
\There are 12 signals in ts interface, ts_dat[0..7], ts_sync, ts_vld, ts_err, ts_clk.\n 
\There're 12 pins in HI3136 or HI3130v200 chip, and each pin can be set to output ts_dat[0..7], ts_sync,\n
\ts_vld or ts_err, but ts_clk is binded to one pin, cannot be setted.\n
CNcomment:ts�ӿڹ���12���ź��ߣ��ֱ���ts_dat[0..7], ts_sync, ts_vld, ts_err, ts_clk��HI3136/HI3130V200 оƬ����12��ts�ܽţ�ÿ���ܽſ���\n
���óɳ�ts_clk֮�������һ���ܽţ�ts_clk�ǹ̶��ģ���֧�����á�CNend

\param[in] u32tunerId  tuner port ID. The port ID can be 0-2.             CNcomment:TUNER�˿ںţ�ȡֵΪ0-2 CNend
\param[in] pstTSOUT   pointer of ts pin.                                  CNcomment:ָ�����ͣ�ts�ܽŶ��壬��μ�::HI_UNF_TUNER_TSOUT_SET_S CNend
\retval ::HI_SUCCESS                                                      CNcomment: success.�ɹ� CNend
\retval ::HI_FAILURE  Calling this API fails.                             CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_TUNER_NOT_OPEN  The TUNER module is not opened. CNcomment: ģ��û�д�CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_INVALID_PARA The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_FAILED_SETTSOUT Setting tsout fails. CNcomment:����ts out ʧ�� CNend
\see \n
none.CNcomment:�� CNend
*/
HI_S32 HI_UNF_TUNER_SetTSOUT(HI_U32 u32TunerId, HI_UNF_TUNER_TSOUT_SET_S *pstTSOUT);

/** 
\brief Obtains the frequency locking status and parameters of the tuner.
CNcomment:\brief ��ȡTUNER��Ƶ״̬����Ƶ����CNend
\attention \n
N/A
\param[in] u32tunerId tuner port ID. The port ID can be 0-2.             CNcomment:TUNER�˿ںţ�ȡֵΪ0-2 CNend
\param[in] u32TimeOut frequency locking timeout, unit: ms, frequency locking time is related to strength of signal CNcomment:��Ƶ�ĳ�ʱʱ�䣬��λ�Ǻ��룬��Ƶʱ����ź�ǿ���й�ϵ��CNend
               if you want to get the state of locking, the u32TimeOut should be set 100ms at least;
               if the u32TimeOut is 0, just config register without locking state, return HI_SUCCESS
               CNcomment: �����õ���״̬����Ƶ��ʱ��С����Ϊ100ms��
                          ���ʱʱ��Ϊ0��ֻ���üĴ��������ж���״̬������HI_SUCCESS��CNend
\param[out] pstTunerStatus: The current frequency locking status and parameters of the tuner are returned. Note: This parameter is valid only when HI_SUCCESS is returned.  CNcomment: ���ص�ǰTUNER����Ƶ״̬����Ƶ���� ע�⣺�˲���ֻ�ں���سɹ�������²������塣CNend
\retval ::HI_SUCCESS Success                    CNcomment:�ɹ� CNend
\retval ::HI_FAILURE  Calling this API fails.  CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_TUNER_NOT_OPEN  The TUNER module is not opened. CNcomment: ģ��û�д�CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_INVALID_POINT The input parameter is invalid. CNcomment:���� ����Ƿ� CNend

\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_GetStatus(HI_U32    u32tunerId , HI_UNF_TUNER_STATUS_S  *pstTunerStatus);


/** 
\brief Obtains the current bit error rate (BER) of the tuner. The BER is expressed by using the scientific notation.
CNcomment:\brief ��ȡ��ǰTUNER �������ʣ��ÿ�ѧ�����ʾ��CNend
\attention \n
N/A
\param[in] u32tunerId tuner port ID. The port ID can be 0-2.             CNcomment:TUNER�˿ںţ�ȡֵΪ0-2 CNend
\param[out] pu32BER  Pointer to the current BER of the tuner The error bit rate consists of three elements. Their definitions are as follows:
                      Their definitions are as follows:
                    pu32BER[0]: integral part of the base number of the BER
                    pu32BER[1]: decimal part of the base number of the BER x 1000
                    pu32BER[2]: absolute value of the exponential of the BER
                    For example, if the BER is 2.156E-7, the values of the three elements are
                    2, 156, and 7 respectively.
                    The three elements are valid only when HI_SUCCESS is returned.
                    Otherwise, the application layer sets a large value as required, for example, 0.5 (the values of the three elements are 5, 0, and 1 respectively).
                         0.5 (the values of the three elements are 5, 0, and 1 respectively).
CNcomment:\param[out] pu32BER  ָ��ǰTUNER �����ʵ�ָ�롣��ָ��ָ��һ�������Ԫ�ص����飬���Ԫ�غ�������:
                              pu32BER[0]:�����ʵ���������
                              pu32BER[1]:�����ʵ����С��ֳ���1000
                              pu32BER[2]:������ָ���ȡ���ֵ
                              ����:������Ϊ2.156E-7����ô���Ԫ�ص�ȡֵ�ֱ�Ϊ 
                              2��156��7 
                              �����Ԫ��ֻ�ں���سɹ�������²������壬
                              ����Ӧ�ò�ɸ��ʵ�������������һ���ϴ�ֵ(����0.5�������Ԫ�طֱ�Ϊ5��0��1)��CNend
\retval ::HI_SUCCESS Success                  CNcomment:�ɹ� CNend
\retval ::HI_FAILURE Calling this API fails.  CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_TUNER_NOT_OPEN  The TUNER module is not opened. CNcomment: ģ��û�д�CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_INVALID_POINT The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_NOT_CONNECT The signal is droped. CNcomment:�ź�δ�� CNend

\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_GetBER(HI_U32   u32tunerId , HI_U32 *pu32BER);


/** 
\brief Obtains the current signal-to-noise ratio (SNR) of the tuner.
CNcomment:\brief ��ȡ��ǰTUNER ������ȡ�CNend
\attention \n
N/A
\param[in] u32tunerId  tuner port ID. The port ID can be 0-2.         CNcomment:TUNER�˿ںţ�ȡֵΪ0-2 CNend
\param[out] pu32SNR  Pointer to the current SNR of the tuner
                    This parameter is valid only when HI_SUCCESS is returned.
                   Otherwise, the application layer sets the parameter to a small value.
CNcomment:\param[out] pu32SNR  ָ��ǰTUNER����ȵ�ָ�롣
                              �˲���ֻ�ں���سɹ�������²������壬
                              ����Ӧ�ò�ɽ���ֵ����Ϊ��Сֵ��CNend
\retval ::HI_SUCCESS Success                   CNcomment:�ɹ� CNend
\retval ::HI_FAILURE  Calling this API fails.  CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_TUNER_NOT_OPEN  The TUNER module is not opened. CNcomment: ģ��û�д�CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_INVALID_POINT The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_NOT_CONNECT The signal is droped. CNcomment:�ź�δ�� CNend
\retval ::HI_ERR_TUNER_FAILED_GETSTATUS Getting status fails. CNcomment:��ȡ�ŵ���״̬ʧ�� CNend

\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_GetSNR(HI_U32   u32tunerId , HI_U32 *pu32SNR );             /* range : 0-255  */


/** 
\brief Obtains the current signal strength of the tuner.
CNcomment:\brief ��ȡ��ǰTUNER���ź�ǿ�ȡ�CNend
\attention \n
N/A
\param[in] u32tunerId  tuner port ID. The port ID can be 0-2.         CNcomment:TUNER�˿ںţ�ȡֵΪ0-2 CNend
\param[out] pu32SignalStrength  Pointer to the current signal strength of the tuner
                            For cable signal, the value of *pu32SignalStrength ranges from 20~120.
                            For satellite signal, the value is 15~130, unit in dBuv.
                  This parameter is valid only when HI_SUCCESS is returned.
                  Otherwise, the application layer sets the parameter to a small value.
CNcomment:\param[out] pu32SignalStrength  ָ��ǰTUNER�ź�ǿ�ȵ�ָ�롣
                            ����Cable�źţ�*pu32SignalStrength��ȡֵ��ΧΪ20~120
                            ����Satellite�źţ�*pu32SignalStrength��ȡֵ��ΧΪ15~130����λΪdBuv
                            �˲���ֻ�ں���سɹ�������²������壬
                            ����Ӧ�ò�ɽ���ֵ����Ϊ��Сֵ��CNend
\retval ::HI_SUCCESS Success                  CNcomment:�ɹ� CNend
\retval ::HI_FAILURE  Calling this API fails. CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_TUNER_NOT_OPEN  The TUNER module is not opened. CNcomment: ģ��û�д�CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_INVALID_POINT The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_NOT_CONNECT The signal is droped. CNcomment:�ź�δ�� CNend
\retval ::HI_ERR_TUNER_FAILED_GETSTATUS Getting status fails. CNcomment:��ȡ�ŵ���״̬ʧ�� CNend
\retval ::HI_ERR_TUNER_FAILED_GETSIGNALSTRENGTH Calling this CMD fails. CNcomment:ioctlϵͳ����ʧ�� CNend

\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_GetSignalStrength(HI_U32   u32tunerId , HI_U32 *pu32SignalStrength );

/**
\brief Obtains the current signal quality of the tuner, returns a percentage value. 
CNcomment:\brief ��ȡ��ǰTUNER���ź����������ذٷֱ�( �˽ӿڲ�֧��DVB_C ��J83B��ʽ)��CNend
\attention \n
N/A
\param[in] u32TunerId  Tuner port ID. The port ID can be 0-2.         CNcomment:TUNER�˿ںţ�ȡֵΪ0-2 CNend
\param[out] pu32SignalQuality   Output pointer.                       CNcomment:ָ���ź�������ָ�롣CNend
\retval ::HI_SUCCESS   Success                                        CNcomment:�ɹ� CNend
\retval ::HI_FAILURE   Calling this API fails.                        CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_TUNER_NOT_OPEN  The TUNER module is not opened. CNcomment: ģ��û�д�CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_INVALID_POINT The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_NOT_CONNECT The signal is droped. CNcomment:�ź�δ�� CNend
\retval ::HI_ERR_TUNER_FAILED_GETSTATUS Getting status fails. CNcomment:��ȡ�ŵ���״̬ʧ�� CNend
\retval ::HI_ERR_TUNER_FAILED_GETSIGNALQUALITY Calling this CMD fails. CNcomment:ioctlϵͳ����ʧ�� CNend

\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_GetSignalQuality(HI_U32 u32TunerId, HI_U32 *pu32SignalQuality);

/**
\brief Obtains the actual frequency and symbol rate of the current tuner for cable and satellite
signal, for terrestrial signal, symbol rate means nothing, ignore it. 
CNcomment:\brief ��ȡ��ǰTUNER��ʵ��Ƶ�ʺͷ���ʡ�CNend
\attention \n
N/A
\param[in] u32TunerId  Tuner port ID. The port ID can be 0-2.         CNcomment:TUNER�˿ںţ�ȡֵΪ0-2 CNend
\param[out] pu32Freq   Pointer to the current frequency of the tuner. CNcomment:ָ��ǰTUNERƵ���ָ�롣CNend
\param[out] pu32Symb   Points to the current symbol rate.             CNcomment:ָ��ǰ����� CNend
\retval ::HI_SUCCESS Success                                          CNcomment:�ɹ� CNend
\retval ::HI_FAILURE  Calling this API fails.                         CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_TUNER_NOT_OPEN  The TUNER module is not opened. CNcomment: ģ��û�д�CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_INVALID_POINT The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_NOT_CONNECT The signal is droped. CNcomment:�ź�δ�� CNend
\retval ::HI_ERR_TUNER_FAILED_GETSTATUS Getting status fails. CNcomment:��ȡ�ŵ���״̬ʧ�� CNend
\retval ::HI_ERR_TUNER_FAILED_GETSIGNALSTRENGTH  Calling this CMD fails. CNcomment:ioctlϵͳ����ʧ�� CNend

\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_GetRealFreqSymb( HI_U32 u32TunerId, HI_U32 *pu32Freq, HI_U32 *pu32Symb );

/**
\brief Obtains current signal information of the TUNER, used in satellite and terrestrial, not necessary for cable. 
CNcomment:\brief ��ȡ��ǰTUNER���ź���Ϣ��CNend
\attention \n
N/A
\param[in] u32TunerId  Tuner port ID. The port ID can be 0-2.         CNcomment:TUNER�˿ںţ�ȡֵΪ0-2 CNend
\param[out] pstSignalInfo Pointer to a signal info structure.         CNcomment:ָ���ź���Ϣ�ṹ���ָ�롣CNend
\retval ::HI_SUCCESS   Success                                        CNcomment:�ɹ� CNend
\retval ::HI_FAILURE   Calling this API fails.                        CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_TUNER_NOT_OPEN  The TUNER module is not opened. CNcomment: ģ��û�д�CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_INVALID_POINT The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_NOT_CONNECT The signal is droped. CNcomment:�ź�δ�� CNend
\retval ::HI_ERR_TUNER_FAILED_GETSTATUS Getting status fails. CNcomment:��ȡ�ŵ���״̬ʧ�� CNend
\retval ::HI_ERR_TUNER_FAILED_GETSIGNALINFO  Calling this CMD fails. CNcomment:ioctlϵͳ����ʧ�� CNend

\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_GetSignalInfo(HI_U32 u32TunerId, HI_UNF_TUNER_SIGNALINFO_S *pstSignalInfo);

/**
\brief Sets the LNB parameter. 
CNcomment:\brief ����LNB����CNend
\attention \n
N/A
\param[in] u32TunerId  Tuner port ID. The port ID can be 0-2.         CNcomment:TUNER�˿ںţ�ȡֵΪ0-2 CNend
\param[in] pstLNB      Pointer to a LNB parameter structure.          CNcomment:ָ��LNB����ṹ���ָ�롣CNend
\retval ::HI_SUCCESS   Success                                        CNcomment:�ɹ� CNend
\retval ::HI_FAILURE   Calling this API fails.                        CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_TUNER_NOT_OPEN  The TUNER module is not opened. CNcomment: ģ��û�д�CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_INVALID_POINT The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_INVALID_PARA The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_SetLNBConfig(HI_U32 u32TunerId, const HI_UNF_TUNER_FE_LNB_CONFIG_S *pstLNB);

/**
\brief Sets the LNB power. 
CNcomment:\brief ����LNB���硣CNend
\attention \n
N/A
\param[in] u32TunerId  Tuner port ID. The port ID can be 0-2.         CNcomment:TUNER�˿ںţ�ȡֵΪ0-2 CNend
\param[in] enLNBPower  The enumeration of the LNB power type.         CNcomment:LNB���緽ʽö��ֵ��CNend
\retval ::HI_SUCCESS   Success                                        CNcomment:�ɹ� CNend
\retval ::HI_FAILURE   Calling this API fails.                        CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_TUNER_NOT_OPEN  The TUNER module is not opened. CNcomment: ģ��û�д�CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_INVALID_POINT The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_INVALID_PARA The input parameter is invalid. CNcomment:���� ����Ƿ� CNend

\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_SetLNBPower(HI_U32 u32TunerId, HI_UNF_TUNER_FE_LNB_POWER_E enLNBPower);

/**
\brief Sets PLP ID, only used in DVB-T2. 
CNcomment:\brief ���������ܵ�ID��CNend
\attention \n
N/A
\param[in] u32TunerId  Tuner port ID. The port ID can be 0-2.         CNcomment:TUNER�˿ںţ�ȡֵΪ0-2 CNend
\param[in] u8PLPID  The PLP ID.                                       CNcomment:�����ܵ�ID��CNend
\retval ::HI_SUCCESS   Success                                        CNcomment:�ɹ� CNend
\retval ::HI_FAILURE   Calling this API fails.                        CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_TUNER_NOT_OPEN  The TUNER module is not opened. CNcomment: ģ��û�д�CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_FAILED_SETPLPID  Calling this CMD fails. CNcomment:ioctlϵͳ����ʧ�� CNend

\see \n
N/A
*/

HI_S32 HI_UNF_TUNER_SetPLPID(HI_U32 u32TunerId, HI_U8 u8PLPID);

/**
\brief Gets PLP number, only used in DVB-T2. 
CNcomment:\brief ��ȡ�����ܵ�������CNend
\attention \n
N/A
\param[in] u32TunerId  Tuner port ID. The port ID can be 0-2.         CNcomment:TUNER�˿ںţ�ȡֵΪ0-2 CNend
\param[out] pu8PLPNum  The PLP number.                                CNcomment:�����ܵ�������CNend
\retval ::HI_SUCCESS   Success                                        CNcomment:�ɹ� CNend
\retval ::HI_FAILURE   Calling this API fails.                        CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_TUNER_NOT_OPEN  The TUNER module is not opened. CNcomment: ģ��û�д�CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_INVALID_POINT The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_FAILED_GETPLPNUM  Calling this CMD fails. CNcomment:ioctlϵͳ����ʧ�� CNend

\see \n
N/A
*/

HI_S32 HI_UNF_TUNER_GetPLPNum(HI_U32 u32TunerId, HI_U8 *pu8PLPNum);


/**
\brief Gets current PLP type, only used in DVB-T2. 
CNcomment:\brief ��ȡ��ǰ�����ܵ����͡�CNend
\attention \n
N/A
\param[in] u32TunerId  Tuner port ID. The port ID can be 0-2.         CNcomment:TUNER�˿ںţ�ȡֵΪ0-2 CNend
\param[out] penPLPType  The PLP type.                                 CNcomment:�����ܵ����͡�CNend
\retval ::HI_SUCCESS   Success                                        CNcomment:�ɹ� CNend
\retval ::HI_FAILURE   Calling this API fails.                        CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_TUNER_NOT_OPEN  The TUNER module is not opened. CNcomment: ģ��û�д�CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_INVALID_POINT The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_FAILED_GETPLPTYPE  Calling this CMD fails. CNcomment:ioctlϵͳ����ʧ�� CNend

\see \n
N/A
*/

HI_S32 HI_UNF_TUNER_GetCurrentPLPType(HI_U32 u32TunerId, HI_UNF_TUNER_T2_PLP_TYPE_E *penPLPType);


/**
\brief Starts blind scan. 
CNcomment:\brief ��ʼäɨ��CNend
\attention \n
N/A
\param[in] u32TunerId  Tuner port ID. The port ID can be 0-2.         CNcomment:TUNER�˿ںţ�ȡֵΪ0-2 CNend
\param[in] pstPara     The pointer to the blind scan parameter.       CNcomment:ָ��äɨ�����ָ�롣CNend
\retval ::HI_SUCCESS   Success                                        CNcomment:�ɹ� CNend
\retval ::HI_FAILURE   Calling this API fails.                        CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_TUNER_NOT_OPEN  The TUNER module is not opened. CNcomment: ģ��û�д�CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_INVALID_POINT The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_INVALID_PARA The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_FAILED_BLINDSCAN Blind scan busy. CNcomment:äɨ����Ӧ�� CNend

\see Please refer to definition of HI_UNF_TUNER_BLINDSCAN_PARA_S.
N/A
*/
HI_S32 HI_UNF_TUNER_BlindScanStart(HI_U32 u32TunerId, const HI_UNF_TUNER_BLINDSCAN_PARA_S *pstPara);

/**
\brief Stops blind scan. 
CNcomment:\brief ֹͣäɨ��CNend
\attention \n
N/A
\param[in] u32TunerId  Tuner port ID. The port ID can be 0-2.         CNcomment:TUNER�˿ںţ�ȡֵΪ0-2 CNend
\retval ::HI_SUCCESS   Success                                        CNcomment:�ɹ� CNend
\retval ::HI_FAILURE   Calling this API fails.                        CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_TUNER_NOT_OPEN  The TUNER module is not opened. CNcomment: ģ��û�д�CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:���� ����Ƿ� CNend

\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_BlindScanStop(HI_U32 u32TunerId);

/**
\brief TUNER standby. 
CNcomment:\brief TUNER���(  �˽ӿڲ�֧��DVB_C ��J83B��ʽ����������ʽ�����μ�pmoc ���ӿ�)  CNend
\attention \n
N/A
\param[in] u32TunerId  Tuner port ID. The port ID can be 0-2.         CNcomment:TUNER�˿ںţ�ȡֵΪ0-2 CNend
\retval ::HI_SUCCESS   Success                                        CNcomment:�ɹ� CNend
\retval ::HI_FAILURE   Calling this API fails.                        CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_TUNER_NOT_OPEN  The TUNER module is not opened. CNcomment: ģ��û�д�CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_FAILED_STANDBY  Calling this CMD fails. CNcomment:ioctlϵͳ����ʧ�� CNend

\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_Standby(HI_U32 u32TunerId);

/**
\brief Wakes up TUNER. 
CNcomment:\brief TUNER����(  �˽ӿڲ�֧��DVB_C ��J83B��ʽ����������ʽ������μ�pmoc ���ѻ��ѽӿ�) CNend
\attention \n
N/A
\param[in] u32TunerId  Tuner port ID. The port ID can be 0-2.         CNcomment:TUNER�˿ںţ�ȡֵΪ0-2 CNend
\retval ::HI_SUCCESS   Success                                        CNcomment:�ɹ� CNend
\retval ::HI_FAILURE   Calling this API fails.                        CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_TUNER_NOT_OPEN  The TUNER module is not opened. CNcomment: ģ��û�д�CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_FAILED_WAKEUP  Calling this CMD fails. CNcomment:ioctlϵͳ����ʧ�� CNend
\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_WakeUp( HI_U32 u32TunerId);

/**
\brief Sends and receives DiSEqC message, only the devices supporting DiSEqC 2.x support receive message. 
CNcomment:\brief ���ͽ���DiSEqC��Ϣ����֧��DiSEqC 2.x���豸֧�ֽ�����Ϣ��CNend
\attention \n
N/A
\param[in] u32TunerId  Tuner port ID. The port ID can be 0-2.         CNcomment:TUNER�˿ں�, ȡֵ��Χ0-2 CNend
\param[in] pstSendMsg  The pointer to a sending message structure.    CNcomment:ָ������Ϣ�ṹ���ָ�롣CNend
\param[out] pstRecvMsg The pointer to a receiving message structure.If your device is DiSEqC 1.x, you can pass NULL here.
CNcomment:\param[out] pstRecvMsg ָ�������Ϣ�ṹ���ָ�롣�����DiSEqC 1.x�豸��������Դ�NULL��CNend
\retval ::HI_SUCCESS   Success                                        CNcomment:�ɹ� CNend
\retval ::HI_FAILURE   Calling this API fails.                        CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_TUNER_NOT_OPEN  The TUNER module is not opened. CNcomment: ģ��û�д�CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_INVALID_POINT The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_INVALID_PARA The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_FAILED_DISEQC Send DiSEqC message fail. CNcomment:������Ϣʧ�� CNend
\retval ::HI_ERR_TUNER_FAILED_DISEQC Recv DiSEqC message fail. CNcomment:������Ϣʧ�� CNend

\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_DISEQC_SendRecvMessage(HI_U32 u32TunerId,
                                           const HI_UNF_TUNER_DISEQC_SENDMSG_S * pstSendMsg,
                                           HI_UNF_TUNER_DISEQC_RECVMSG_S * pstRecvMsg);

/**
\brief Sets 0/12V switch. Don't support now. 
CNcomment:\brief ����0/12V����״̬���ݲ�֧�֡�CNend
\attention \n
N/A
\param[in] u32TunerId  Tuner port ID. The port ID can be 0-2.         CNcomment:TUNER�˿ںţ�ȡֵΪ0-2 CNend
\param[in] enPort      The enumeration of the switch port.            CNcomment:����ö��ֵ��CNend
\retval ::HI_SUCCESS   Success                                        CNcomment:�ɹ� CNend
\retval ::HI_FAILURE   Calling this API fails.                        CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_TUNER_NOT_OPEN  The TUNER module is not opened. CNcomment: ģ��û�д�CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_INVALID_PARA The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_Switch012V(HI_U32 u32TunerId, HI_UNF_TUNER_SWITCH_0_12V_E enPort);

/**
\brief Sets 22KHz switch. 
CNcomment:\brief ����22KHz����״̬��CNend
\attention \n
N/A
\param[in] u32TunerId  Tuner port ID. The port ID can be 0-2.         CNcomment:TUNER�˿ںţ�ȡֵΪ0-2 CNend
\param[in] enPort      The enumeration of the switch port.            CNcomment:����ö��ֵ��CNend
\retval ::HI_SUCCESS   Success                                        CNcomment:�ɹ� CNend
\retval ::HI_FAILURE   Calling this API fails.                        CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_TUNER_NOT_OPEN  The TUNER module is not opened. CNcomment: ģ��û�д�CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_INVALID_PARA The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_Switch22K(HI_U32 u32TunerId, HI_UNF_TUNER_SWITCH_22K_E enPort);

/**
\brief Sets tone burst switch. 
CNcomment:\brief ����Tone burst����״̬��CNend
\attention \n
N/A
\param[in] u32TunerId  Tuner port ID. The port ID can be 0-2.         CNcomment:TUNER�˿ںţ�ȡֵΪ0-2 CNend
\param[in] enStatus    The enumeration of the switch port.            CNcomment:����ö��ֵ��CNend
\retval ::HI_SUCCESS   Success                                        CNcomment:�ɹ� CNend
\retval ::HI_FAILURE   Calling this API fails.                        CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_TUNER_NOT_OPEN  The TUNER module is not opened. CNcomment: ģ��û�д�CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_INVALID_PARA The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_FAILED_DISEQC  Calling this CMD fails. CNcomment:ioctlϵͳ����ʧ�� CNend
\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_SwitchToneBurst(HI_U32 u32TunerId, HI_UNF_TUNER_SWITCH_TONEBURST_E enStatus);

//#ifdef DISEQC_SUPPORT
/**
\brief Sets DiSEqC 1.0/2.0 switch, at most 4 port.
CNcomment:\brief ����DiSEqC 1.0/2.0���أ�����4�ڡ�CNend
\attention \n
N/A
\param[in] u32TunerId  Tuner port ID. The port ID can be 0-2.         CNcomment:TUNER�˿ںţ�ȡֵΪ0-2 CNend
\param[in] pstPara     The pointer to a switch parameter structure.   CNcomment:ָ�򿪹ز����ָ�롣CNend
\retval ::HI_SUCCESS   Success                                        CNcomment:�ɹ� CNend
\retval ::HI_FAILURE   Calling this API fails.                        CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_INVALID_POINT The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_INVALID_PARA The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\see Please refer to definition of HI_UNF_TUNER_DISEQC_SWITCH4PORT_S.
N/A
*/
HI_S32 HI_UNF_TUNER_DISEQC_Switch4Port(HI_U32 u32TunerId, const HI_UNF_TUNER_DISEQC_SWITCH4PORT_S* pstPara);

/**
\brief Sets DiSEqC 1.1/2.1 switch, supports 8in1, 16in1 switches. 
CNcomment:\brief ����DiSEqC 1.1/2.1���أ�֧��8�ڣ�16�ڿ��ء�CNend
\attention \n
N/A
\param[in] u32TunerId  Tuner port ID. The port ID can be 0-2.         CNcomment:TUNER�˿ںţ�ȡֵΪ0-2 CNend
\param[in] pstPara     The pointer to a switch parameter structure.   CNcomment:ָ�򿪹ز����ָ�롣CNend
\retval ::HI_SUCCESS   Success                                        CNcomment:�ɹ� CNend
\retval ::HI_FAILURE   Calling this API fails.                        CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_INVALID_POINT The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_INVALID_PARA The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_DISEQC_Switch16Port(HI_U32 u32TunerId, const HI_UNF_TUNER_DISEQC_SWITCH16PORT_S* pstPara);

/**
\brief Lets the DiSEqC motor store current position. 
CNcomment:\brief DiSEqC���洢��ǰλ�á�CNend
\attention \n
N/A
\param[in] u32TunerId  Tuner port ID. The port ID can be 0-2.         CNcomment:TUNER�˿ںţ�ȡֵΪ0-2 CNend
\param[in] pstPara     The pointer to a position parameter structure. CNcomment:ָ��λ�ò����ָ�롣CNend
\retval ::HI_SUCCESS   Success                                        CNcomment:�ɹ� CNend
\retval ::HI_FAILURE   Calling this API fails.                        CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_INVALID_POINT The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_INVALID_PARA The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_DISEQC_StorePos(HI_U32 u32TunerId, const HI_UNF_TUNER_DISEQC_POSITION_S *pstPara);

/**
\brief Lets the DiSEqC motor move to stored position. 
CNcomment:\brief DiSEqC���ת�����洢λ�á�CNend
\attention \n
N/A
\param[in] u32TunerId  Tuner port ID. The port ID can be 0-2.         CNcomment:TUNER�˿ںţ�ȡֵΪ0-2 CNend
\param[in] pstPara     The pointer to a position parameter structure. CNcomment:ָ��λ�ò����ָ�롣CNend
\retval ::HI_SUCCESS   Success                                        CNcomment:�ɹ� CNend
\retval ::HI_FAILURE   Calling this API fails.                        CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_INVALID_POINT The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_INVALID_PARA The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_DISEQC_GotoPos(HI_U32 u32TunerId, const HI_UNF_TUNER_DISEQC_POSITION_S *pstPara);

/**
\brief Enables or disables the DiSEqC motor's limit setting. 
CNcomment:\brief �򿪡��ر�DiSEqC���Ȩ�����á�CNend
\attention \n
N/A
\param[in] u32TunerId  Tuner port ID. The port ID can be 0-2.         CNcomment:TUNER�˿ںţ�ȡֵΪ0-2 CNend
\param[in] pstPara     The pointer to a limit parameter structure.    CNcomment:ָ��limit�����ָ�롣CNend
\retval ::HI_SUCCESS   Success                                        CNcomment:�ɹ� CNend
\retval ::HI_FAILURE   Calling this API fails.                        CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_INVALID_POINT The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_INVALID_PARA The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_DISEQC_SetLimit(HI_U32 u32TunerId, const HI_UNF_TUNER_DISEQC_LIMIT_S* pstPara);

/**
\brief Drives DiSEqC motor. 
CNcomment:\brief ��DiSEqC����ƶ���CNend
\attention \n
N/A
\param[in] u32TunerId  Tuner port ID. The port ID can be 0-2.         CNcomment:TUNER�˿ںţ�ȡֵΪ0-2 CNend
\param[in] pstPara     The pointer to a movement parameter structure. CNcomment:ָ���ƶ������ָ�롣CNend
\retval ::HI_SUCCESS   Success                                        CNcomment:�ɹ� CNend
\retval ::HI_FAILURE   Calling this API fails.                        CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_INVALID_POINT The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_INVALID_PARA The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_DISEQC_Move(HI_U32 u32TunerId, const HI_UNF_TUNER_DISEQC_MOVE_S* pstPara);

/**
\brief Halts DiSEqC motor. 
CNcomment:\brief ֹͣDiSEqC����ƶ���CNend
\attention \n
N/A
\param[in] u32TunerId  Tuner port ID. The port ID can be 0-2.         CNcomment:TUNER�˿ںţ�ȡֵΪ0-2 CNend
\param[in] enLevel     The command level of the DiSEqC motor.         CNcomment:���֧�ֵ�����ȼ���CNend
\retval ::HI_SUCCESS   Success                                        CNcomment:�ɹ� CNend
\retval ::HI_FAILURE   Calling this API fails.                        CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_INVALID_PARA The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_DISEQC_Stop(HI_U32 u32TunerId, const HI_UNF_TUNER_DISEQC_LEVEL_E enLevel);

/**
\brief Lets the DiSEqC motor recalculate its stored positions. 
CNcomment:\brief ���¼������洢λ�á�CNend
\attention \n
N/A
\param[in] u32TunerId  Tuner port ID. The port ID can be 0-2.         CNcomment:TUNER�˿ںţ�ȡֵΪ0-2 CNend
\param[in] pstPara     The pointer to a recaulculate parameter structure. 
                                                                      CNcomment:ָ���ؼ�������ָ�롣CNend
\retval ::HI_SUCCESS   Success                                        CNcomment:�ɹ� CNend
\retval ::HI_FAILURE   Calling this API fails.                        CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_INVALID_POINT The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_INVALID_PARA The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_DISEQC_Recalculate(HI_U32 u32TunerId, const HI_UNF_TUNER_DISEQC_RECALCULATE_S* pstPara);

/**
\brief Calculates the angular, basing on site-longitude, site-latitude and satellite-longitude. 
CNcomment:\brief USALS��ݵ��ؾ�γ�ȡ����Ǿ��ȼ������ǽǶȡ�CNend
\attention \n
N/A
\param[in] u32TunerId  Tuner port ID. The port ID can be 0-2.         CNcomment:TUNER�˿ںţ�ȡֵΪ0-2 CNend
\param[in/out] pstPara The pointer to a USALS parameter structure.    CNcomment:ָ��USALS��������ָ�롣CNend
\retval ::HI_SUCCESS   Success                                        CNcomment:�ɹ� CNend
\retval ::HI_FAILURE   Calling this API fails.                        CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_INVALID_POINT The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_INVALID_PARA The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_DISEQC_CalcAngular(HI_U32 u32TunerId, HI_UNF_TUNER_DISEQC_USALS_PARA_S* pstPara);

/**
\brief Gotos the indicated angular. 
CNcomment:\brief USALS�豸ת��ĳ�Ƕȡ�CNend
\attention \n
N/A
\param[in] u32TunerId  Tuner port ID. The port ID can be 0-2.         CNcomment:TUNER�˿ںţ�ȡֵΪ0-2 CNend
\param[in] pstPara     The pointer to a USALS angular structure.      CNcomment:ָ��USALS�ǶȲ����ָ�롣CNend
\retval ::HI_SUCCESS   Success                                        CNcomment:�ɹ� CNend
\retval ::HI_FAILURE   Calling this API fails.                        CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_INVALID_POINT The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_INVALID_PARA The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_DISEQC_GotoAngular(HI_U32 u32TunerId, const HI_UNF_TUNER_DISEQC_USALS_ANGULAR_S* pstPara);

/**
\brief Resets DiSEqC device. 
CNcomment:\brief ��λDiSEqC�豸��CNend
\attention \n
N/A
\param[in] u32TunerId  Tuner port ID. The port ID can be 0-2.         CNcomment:TUNER�˿ںţ�ȡֵΪ0-2 CNend
\param[in] enLevel     The command level of the DiSEqC device.        CNcomment:�豸֧�ֵ�����ȼ���CNend
\retval ::HI_SUCCESS   Success                                        CNcomment:�ɹ� CNend
\retval ::HI_FAILURE   Calling this API fails.                        CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_INVALID_PARA The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_DISEQC_Reset(HI_U32 u32TunerId, HI_UNF_TUNER_DISEQC_LEVEL_E enLevel);

/**
\brief DiSEqC device standby. 
CNcomment:\brief DiSEqC�豸���CNend
\attention \n
N/A
\param[in] u32TunerId  Tuner port ID. The port ID can be 0-2.         CNcomment:TUNER�˿ںţ�ȡֵΪ0-2 CNend
\param[in] enLevel     The command level of the DiSEqC device.        CNcomment:�豸֧�ֵ�����ȼ���CNend
\retval ::HI_SUCCESS   Success                                        CNcomment:�ɹ� CNend
\retval ::HI_FAILURE   Calling this API fails.                        CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_INVALID_PARA The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_DISEQC_Standby(HI_U32 u32TunerId, HI_UNF_TUNER_DISEQC_LEVEL_E enLevel);

/**
\brief Wakes up DiSEqC device. 
CNcomment:\brief ����DiSEqC�豸��CNend
\attention \n
N/A
\param[in] u32TunerId  Tuner port ID. The port ID can be 0-2.         CNcomment:TUNER�˿ںţ�ȡֵΪ0-2 CNend
\param[in] enLevel     The command level of the DiSEqC device.        CNcomment:�豸֧�ֵ�����ȼ���CNend
\retval ::HI_SUCCESS   Success                                        CNcomment:�ɹ� CNend
\retval ::HI_FAILURE   Calling this API fails.                        CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_INVALID_PARA The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_DISEQC_WakeUp(HI_U32 u32TunerId, HI_UNF_TUNER_DISEQC_LEVEL_E enLevel);

/**
\brief Acquire data to display constellation.
CNcomment:\brief ��ȡ����ͼ��ݡ�CNend
\attention \n
N/A
\param[in]  u32TunerId    Tuner port ID. The port ID can be 0-2.             CNcomment:TUNER�˿ںţ�ȡֵΪ0-2 CNend
\param[in]  enDataLen     The sample data length, can be 512, 1024 or 2048.  CNcomment:�ɼ�����ݳ���, ֧��512��1024��2048 3�֡�CNend
\param[out] pstData       The buffer to store data.                          CNcomment:��ݴ洢��CNend
\retval ::HI_SUCCESS   Success                                               CNcomment:�ɹ� CNend
\retval ::HI_FAILURE   Calling this API fails.                               CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_TUNER_NOT_OPEN  The TUNER module is not opened. CNcomment: ģ��û�д�CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_INVALID_PARA The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_FAILED_SAMPLEDATA Tuner sample data fail. CNcomment:��ȡ���ʧ�� CNend
\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_GetConstellationData(HI_U32 u32TunerId, HI_UNF_TUNER_SAMPLE_DATALEN_E enDataLen, HI_UNF_TUNER_SAMPLE_DATA_S *pstData);

/**
\brief Acquire data to display spectrum.
CNcomment:\brief ��ȡƵ��ͼ��ݡ�CNend
\attention \n
N/A
\param[in]  u32TunerId    Tuner port ID. The port ID can be 0-2.             CNcomment:TUNER�˿ںţ�ȡֵΪ0-2 CNend
\param[in]  enDataLen     The sample data length, can be 512, 1024 or 2048.  CNcomment:�ɼ�����ݳ���, ֧��512��1024��2048 3�֡�CNend
\param[out] pu32Data      The buffer to store data.                          CNcomment:��ݴ洢��CNend
\retval ::HI_SUCCESS   Success                                               CNcomment:�ɹ� CNend
\retval ::HI_FAILURE   Calling this API fails.                               CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_TUNER_NOT_OPEN  The TUNER module is not opened. CNcomment: ģ��û�д�CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_INVALID_PARA The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_FAILED_SAMPLEDATA Tuner sample data fail. CNcomment:��ȡ���ʧ�� CNend
\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_GetSpectrumData(HI_U32 u32TunerId, HI_UNF_TUNER_SAMPLE_DATALEN_E enDataLen, HI_U32 *pu32Data);

/**
\brief Get the upper limit time according to the connect parameter.
CNcomment:\brief �����Ƶ�����ȡ�����Ƶʱ�䡣CNend
\attention \n
N/A
\param[in]  u32TunerId    Tuner port ID. The port ID can be 0-2.             CNcomment:TUNER�˿ںţ�ȡֵΪ0-2 CNend
\param[in]  pstConnectPara     The pointer to store the connect parameter.   CNcomment:��Ƶ����ָ�롣CNend
\param[out] pu32TimeOutMs      The upper limit time.                         CNcomment:�����̨ʱ�䡣CNend
\retval ::HI_SUCCESS   Success                                               CNcomment:�ɹ� CNend
\retval ::HI_FAILURE   Calling this API fails.                               CNcomment:APIϵͳ����ʧ�� CNend
\retval ::HI_ERR_TUNER_NOT_OPEN  The TUNER module is not opened. CNcomment: ģ��û�д�CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_INVALID_POINT The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\retval ::HI_ERR_TUNER_INVALID_PARA The input parameter is invalid. CNcomment:���� ����Ƿ� CNend
\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_GetDefaultTimeout(HI_U32  u32tunerId, const HI_UNF_TUNER_CONNECT_PARA_S  *pstConnectPara, HI_U32 *pu32TimeOutMs);
/** @} */  /** <!-- ==== API declaration end ==== */
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HI_UNF_ECS_TYPE_H__ */

