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

#define HI_I2C_MAX_NUM_USER (15)    /** Maximum I2C channel ID*/ /** CNcomment:最大I2C通道号*/

#define HI_UNF_DISEQC_MSG_MAX_LENGTH (6)    /** DiSEqC message length*/  /** CNcomment:DiSEqC消息长度*/
#define HI_UNF_DISEQC_MAX_REPEAT_TIMES (4)  /** DiSEqC message max repeat times*/ /** CNcomment:DiSEqC消息最大重复发送次数*/
#define MAX_TS_LINE 11                      /** The NO of ts lines that can be configured*/ /** CNcomment:可配置的ts信号线数量*/

//#ifdef DISEQC_SUPPORT
#define DISEQC_MAX_MOTOR_PISITION (255) /** DiSEqC motor max stored position*/ /** CNcomment:DiSEqC马达最大存储星位个数*/
//#endif /* DISEQC_SUPPORT */
/*************************** Structure Definition ****************************/
/** \addtogroup      FRONTEND */
/** @{ */  /** <!-- [FRONTEND] */

/** Sample data, complex format*/
/** CNcomment: 采集数据, 复格式*/
typedef struct  hiUNF_TUNER_SAMPLE_DATA_S
{
   HI_S32 s32DataIP;    /*sample data, i component*/    /**<CNcomment:采集数据的I分量*/
   HI_S32 s32DataQP;    /*sample data, q component*/    /**<CNcomment:采集数据的Q分量*/
} HI_UNF_TUNER_SAMPLE_DATA_S;

/** Sample data length*/
/** CNcomment: 采数长度*/
typedef enum hiUNF_TUNER_SAMPLE_DATALEN_E
{
    HI_UNF_TUNER_SAMPLE_DATALEN_512,              /*sample 512 pts*/        /**<CNcomment:采集512点*/
    HI_UNF_TUNER_SAMPLE_DATALEN_1024,             /*sample 1024 pts*/       /**<CNcomment:采集1024点*/
    HI_UNF_TUNER_SAMPLE_DATALEN_2048,             /*sample 2048 pts*/       /**<CNcomment:采集2048点*/
    HI_UNF_TUNER_SAMPLE_DATALEN_BUTT              /**<Invalid value*/       /**<CNcomment:非法边界值*/
} HI_UNF_TUNER_SAMPLE_DATALEN_E;

/** Modulation mode*/
/** CNcomment: 调制方式*/
typedef enum hiUNF_QAM_TYPE_E
{
    HI_UNF_MOD_TYPE_DEFAULT,          /**<Default QAM mode. The default QAM mode is HI_UNF_MOD_TYPE_QAM_64 at present.*/                /**<CNcomment:默认的QAM类型, 当前系统默认为HI_UNF_MOD_TYPE_QAM_64 */
    HI_UNF_MOD_TYPE_QAM_16 = 0x100,   /**<Enumeration corresponding to the 16QAM mode*/                                                 /**<CNcomment:16QAM对应的枚举值*/
    HI_UNF_MOD_TYPE_QAM_32,           /**<Enumeration corresponding to the 32QAM mode*/                                                 /**<CNcomment:32QAM对应的枚举值*/
    HI_UNF_MOD_TYPE_QAM_64,           /**<Enumeration corresponding to the 64QAM mode*/                                                 /**<CNcomment:64QAM对应的枚举值*/
    HI_UNF_MOD_TYPE_QAM_128,          /**<Enumeration corresponding to the 128QAM mode*/                                                /**<CNcomment:128QAM对应的枚举值*/
    HI_UNF_MOD_TYPE_QAM_256,          /**<Enumeration corresponding to the 256QAM mode*/                                                /**<CNcomment:256QAM对应的枚举值*/
    HI_UNF_MOD_TYPE_QAM_512,          /**<Enumeration corresponding to the 512QAM mode*/                                                /**<CNcomment:512QAM对应的枚举值*/

    HI_UNF_MOD_TYPE_BPSK = 0x200,     /**<Enumeration corresponding to the binary phase shift keying (BPSK) mode. */                    /**<CNcomment:BPSK对应的枚举值*/
    HI_UNF_MOD_TYPE_QPSK = 0x300,     /**<Enumeration corresponding to the quaternary phase shift keying (QPSK) mode. */                /**<CNcomment:QPSK对应的枚举值*/
    HI_UNF_MOD_TYPE_DQPSK,
    HI_UNF_MOD_TYPE_8PSK,             /**<Enumeration corresponding to the 8 phase shift keying (8PSK) mode*/                           /**<CNcomment:8PSK对应的枚举值*/
    HI_UNF_MOD_TYPE_16APSK,           /**<Enumeration corresponding to the 16-Ary Amplitude and Phase Shift Keying (16APSK) mode*/      /**<CNcomment:16APSK对应的枚举值*/
    HI_UNF_MOD_TYPE_32APSK,           /**<Enumeration corresponding to the 32-Ary Amplitude and Phase Shift Keying (32APSK) mode*/      /**<CNcomment:32APSK对应的枚举值*/
    HI_UNF_MOD_TYPE_8VSB,             /**<Enumeration corresponding to (8VSB) mode*/                                                    /**<CNcomment:8VSB对应的枚举值*/
    HI_UNF_MOD_TYPE_16VSB,            /**<Enumeration corresponding to (16VSB) mode*/                                                   /**<CNcomment:16VSB对应的枚举值*/

    /**<Enumeration corresponding to the auto mode. For DVB-S/S2, if detect modulation type fail, it will return auto*/
    /**<CNcomment:卫星信号调制方式自动检测，如果检测失败返回AUTO*/
    HI_UNF_MOD_TYPE_AUTO,

    HI_UNF_MOD_TYPE_BUTT              /**<Invalid Modulation mode*/       /**<CNcomment:非法的调制类型枚举值*/
}HI_UNF_MODULATION_TYPE_E;

/** Frequency locking status of the tuner*/
/** CNcomment:TUNER锁频状态*/
typedef enum  hiUNF_TUNER_LOCK_STATUS_E
{
    HI_UNF_TUNER_SIGNAL_DROPPED = 0,       /**<The signal is not locked.*/  /**<CNcomment:信号未锁定*/
    HI_UNF_TUNER_SIGNAL_LOCKED,            /**<The signal is locked.*/      /**<CNcomment:信号已锁定*/
    HI_UNF_TUNER_SIGNAL_BUTT               /**<Invalid value*/              /**<CNcomment:非法边界值*/
}HI_UNF_TUNER_LOCK_STATUS_E;

/** Output mode of the tuner*/
/** CNcomment:TUNER输出模式*/
typedef enum hiUNF_TUNER_OUTPUT_MODE_E
{
    HI_UNF_TUNER_OUTPUT_MODE_DEFAULT ,            /**<Default mode*/                       /**<CNcomment:默认模式*/
    HI_UNF_TUNER_OUTPUT_MODE_PARALLEL_MODE_A,     /**<Parallel mode A*/                    /**<CNcomment:并行模式A*/
    HI_UNF_TUNER_OUTPUT_MODE_PARALLEL_MODE_B,     /**<Parallel mode B*/                    /**<CNcomment:并行模式B*/
    HI_UNF_TUNER_OUTPUT_MODE_SERIAL,              /**<Serial mode 74.25M*/                 /**<CNcomment:串行模74.25M*/ 
    HI_UNF_TUNER_OUTPUT_MODE_SERIAL_50,           /**<Serial mode 50M*/                    /**<CNcomment:串行模50M*/
    HI_UNF_TUNER_OUTPUT_MODE_SERIAL_2BIT,         /**<Serial mode witch 2 bit data line*/  /**<CNcomment:2bit串行*/
    HI_UNF_TUNER_OUTPUT_MODE_BUTT                 /**<Invalid value*/                      /**<CNcomment:非法值*/
}HI_UNF_TUNER_OUPUT_MODE_E;

/** Output order*/
/** CNcomment:TUNER输出线序 */
typedef enum hiUNF_TUNER_OUTPUT_TS_E
{
    HI_UNF_TUNER_OUTPUT_TSDAT0,        /**<data0*/                       /**<CNcomment:数据线0bit*/    
    HI_UNF_TUNER_OUTPUT_TSDAT1,        /**<data1*/                       /**<CNcomment:数据线1bit*/ 
    HI_UNF_TUNER_OUTPUT_TSDAT2,        /**<data2*/                       /**<CNcomment:数据线2bit*/ 
    HI_UNF_TUNER_OUTPUT_TSDAT3,        /**<data3*/                       /**<CNcomment:数据线3bit*/ 
    HI_UNF_TUNER_OUTPUT_TSDAT4,        /**<data4*/                       /**<CNcomment:数据线4bit*/ 
    HI_UNF_TUNER_OUTPUT_TSDAT5,        /**<data5*/                       /**<CNcomment:数据线5bit*/ 
    HI_UNF_TUNER_OUTPUT_TSDAT6,        /**<data6*/                       /**<CNcomment:数据线6bit*/ 
    HI_UNF_TUNER_OUTPUT_TSDAT7,        /**<data7*/                       /**<CNcomment:数据线7bit*/ 
    HI_UNF_TUNER_OUTPUT_TSVLD,         /**<valid*/                       /**<CNcomment:valid信号线*/ 
    HI_UNF_TUNER_OUTPUT_TSSYNC,        /**<sync*/                        /**<CNcomment:sync信号线*/ 
    HI_UNF_TUNER_OUTPUT_TSERR,         /**<err*/                         /**<CNcomment:err信号线*/ 
    HI_UNF_TUNER_OUTPUT_BUTT           /**<Invalid value*/               /**<CNcomment:非法值*/
}HI_UNF_TUNER_OUTPUT_TS_E;

/** ts output port order*/
/** CNcomment:ts接口输出线序 */
typedef struct hiUNF_TUNER_TSOUT_SET_S
{
    HI_UNF_TUNER_OUTPUT_TS_E enTSOutput[MAX_TS_LINE];   /**<ts output port order*/    /**<CNcomment:ts接口输出线序*/
}HI_UNF_TUNER_TSOUT_SET_S;

/** Signal type of the tuner*/
/** CNcomment:TUNER信号类型*/
typedef enum    hiTUNER_SIG_TYPE_E
{
    HI_UNF_TUNER_SIG_TYPE_CAB = 0  ,   /**<Cable signal*/       /**<CNcomment:CABLE信号*/
    HI_UNF_TUNER_SIG_TYPE_SAT ,        /**<Satellite signal*/   /**<CNcomment:卫星信号*/
    HI_UNF_TUNER_SIG_TYPE_DVB_T  ,     /**<Terrestrial signal*/ /**<CNcomment:地面信号*/
    HI_UNF_TUNER_SIG_TYPE_DVB_T2  ,    /**<Terrestrial signal*/ /**<CNcomment:地面信号*/
    HI_UNF_TUNER_SIG_TYPE_ISDB_T  ,    /**<Terrestrial signal*/ /**<CNcomment:地面信号*/
    HI_UNF_TUNER_SIG_TYPE_ATSC_T  ,    /**<Terrestrial signal*/ /**<CNcomment:地面信号*/
    HI_UNF_TUNER_SIG_TYPE_DTMB  ,      /**<Terrestrial signal*/ /**<CNcomment:地面信号*/
    HI_UNF_TUNER_SIG_TYPE_BUTT         /**<Invalid value*/      /**<CNcomment:非法值*/
}HI_UNF_TUNER_SIG_TYPE_E;

/** type of TUNER device*/
/** CNcomment:TUNER设备类型*/
typedef enum    hiUNF_TUNER_DEV_TYPE_E
{
    HI_UNF_TUNER_DEV_TYPE_XG_3BL,               /**<XG_3BL*/                     /**<CNcomment:支持旭光高频头*/
    HI_UNF_TUNER_DEV_TYPE_CD1616,               /**<CD1616*/                     /**<CNcomment:支持CD1616高频头*/
    HI_UNF_TUNER_DEV_TYPE_ALPS_TDAE,            /**<ALPS_TDAE*/                  /**<CNcomment:支持ALPS_TDAE高频头*/
    HI_UNF_TUNER_DEV_TYPE_TDCC,                 /**<TDCC*/                       /**<CNcomment:支持TDCC高频头*/
    HI_UNF_TUNER_DEV_TYPE_TDA18250,             /**<TDA18250*/                   /**<CNcomment:支持TDA18250高频头*/
    HI_UNF_TUNER_DEV_TYPE_CD1616_DOUBLE,        /**<CD1616 with double agc*/     /**<CNcomment:支持CD1616带双agc高频头*/
    HI_UNF_TUNER_DEV_TYPE_MT2081,				/**<MT2081*/                     /**<CNcomment:支持MT2081高频头*/
    HI_UNF_TUNER_DEV_TYPE_TMX7070X,             /**<THOMSON7070X*/               /**<CNcomment:支持THOMSON7070X高频头*/
    HI_UNF_TUNER_DEV_TYPE_R820C,                /**<R820C*/                      /**<CNcomment:支持R820C高频头*/
	HI_UNF_TUNER_DEV_TYPE_MXL203,               /**<MXL203 */                    /**<CNcomment:支持MXL203高频头*/
    HI_UNF_TUNER_DEV_TYPE_AV2011,               /**<AV2011*/                     /**<CNcomment:支持AV2011高频头*/
    HI_UNF_TUNER_DEV_TYPE_SHARP7903,            /**<SHARP7903*/                  /**<CNcomment:支持SHARP7903高频头*/
    HI_UNF_TUNER_DEV_TYPE_MXL101,               /**<MXL101*/                     /**<CNcomment:支持MXL101高频头*/
    HI_UNF_TUNER_DEV_TYPE_MXL603,               /**<MXL603*/                     /**<CNcomment:支持MXL603高频头*/
    HI_UNF_TUNER_DEV_TYPE_IT9170,               /**<IT9170*/                     /**<CNcomment:支持IT9170高频头*/
    HI_UNF_TUNER_DEV_TYPE_IT9133,               /**<IT9133*/                     /**<CNcomment:支持IT9133高频头*/
    HI_UNF_TUNER_DEV_TYPE_TDA6651,              /**<TDA6651*/                    /**<CNcomment:支持TDA6651高频头*/
    HI_UNF_TUNER_DEV_TYPE_TDA18250B,            /**<TDA18250B*/                  /**<CNcomment:支持TDA18250B高频头*/
    HI_UNF_TUNER_DEV_TYPE_M88TS2022,            /**<M88TS2022*/                  /**<CNcomment:支持M88TS2022高频头*/
    HI_UNF_TUNER_DEV_TYPE_RDA5815,              /**<RDA5815*/                    /**<CNcomment:支持RDA5815高频头*/

    HI_UNF_TUNER_DEV_TYPE_BUTT,                 /**<Invalid value*/              /**<CNcomment:非法边界值*/
}HI_UNF_TUNER_DEV_TYPE_E ;

/** Type of the demod device*/
/** CNcomment:demod设备类型*/
typedef enum    hiUNF_DEMOD_DEV_TYPE_E
{
    HI_UNF_DEMOD_DEV_TYPE_NONE,                /**<Not supported*/      /**<CNcomment:不支持*/  
    HI_UNF_DEMOD_DEV_TYPE_3130I= 0x100,        /**<Internal QAM*/       /**<CNcomment:内部QAM*/
    HI_UNF_DEMOD_DEV_TYPE_3130E,               /**<External Hi3130*/    /**<CNcomment:外部QAM hi3130芯片*/
    HI_UNF_DEMOD_DEV_TYPE_J83B,                /**<suppoort j83b*/      /**<CNcomment:支持j83b*/
    HI_UNF_DEMOD_DEV_TYPE_AVL6211,             /**<Avalink 6211*/       /**<CNcomment:支持Avalink 6211*/
    HI_UNF_DEMOD_DEV_TYPE_MXL101,              /**<Maxlinear mxl101*/   /**<CNcomment:支持Maxlinear mxl101*/
    HI_UNF_DEMOD_DEV_TYPE_MN88472,             /**<PANASONIC mn88472*/  /**<CNcomment:支持PANASONIC mn88472*/
    HI_UNF_DEMOD_DEV_TYPE_IT9170,              /**<ITE it9170*/         /**<CNcomment:支持ITE it9170*/
    HI_UNF_DEMOD_DEV_TYPE_IT9133,              /**<ITE it9133*/         /**<CNcomment:支持ITE it9133*/
    HI_UNF_DEMOD_DEV_TYPE_3136,                /**<External Hi3136*/    /**<CNcomment:外部hi3136芯片*/
    HI_UNF_DEMOD_DEV_TYPE_3136I,               /**<Internal Hi3136*/    /**<CNcomment:内部hi3136芯片*/

    HI_UNF_DEMOD_DEV_TYPE_BUTT,                /**<Invalid value*/      /**<CNcomment:非法边界值*/
}HI_UNF_DEMOD_DEV_TYPE_E ;

/** Defines the cable transmission signal.*/
/** CNcomment:定义CABLE传输信号*/
typedef struct  hiUNF_CAB_CONNECT_PARA_S
{
    HI_U32                      u32Freq;            /**<Frequency, in kHz*/      /**<CNcomment:频率，单位：kHz*/
    HI_U32                      u32SymbolRate ;     /**<Symbol rate, in bit/s*/  /**<CNcomment:符号率，单位bps */
    HI_UNF_MODULATION_TYPE_E    enModType ;         /**<QAM mode*/               /**<CNcomment:QAM调制方式*/
    HI_BOOL                     bReverse ;          /**<Spectrum reverse mode*/  /**<CNcomment:频谱倒置处理方式*/
}HI_UNF_CAB_CONNECT_PARA_S ;

/** Guard interval of OFDM*/
/** CNcomment:多载波调制下的保护间隔*/
typedef enum hiUNF_TUNER_FE_GUARD_INTV_E
{
    HI_UNF_TUNER_FE_GUARD_INTV_DEFALUT = 0 ,       /**<default guard interval mode*/    /**<CNcomment:保护间隔默认模式*/
    HI_UNF_TUNER_FE_GUARD_INTV_1_128       ,       /**<1/128*/                          /**<CNcomment:保护间隔1/128模式*/
    HI_UNF_TUNER_FE_GUARD_INTV_1_32       ,        /**<1/32*/                           /**<CNcomment:保护间隔1/32模式*/
    HI_UNF_TUNER_FE_GUARD_INTV_1_16       ,        /**<1/16*/                           /**<CNcomment:保护间隔1/16模式*/
    HI_UNF_TUNER_FE_GUARD_INTV_1_8        ,        /**<1/8*/                            /**<CNcomment:保护间隔1/8模式*/
    HI_UNF_TUNER_FE_GUARD_INTV_1_4        ,        /**<1/4*/                            /**<CNcomment:保护间隔1/4模式*/
    HI_UNF_TUNER_FE_GUARD_INTV_19_128       ,      /**<19/128*/                         /**<CNcomment:保护间隔19/128模式*/
    HI_UNF_TUNER_FE_GUARD_INTV_19_256       ,      /**<19/256*/                         /**<CNcomment:保护间隔19/256模式*/
    HI_UNF_TUNER_FE_GUARD_INTV_BUTT     ,          /**<Invalid value*/                  /**<CNcomment:非法边界值*/
}HI_UNF_TUNER_FE_GUARD_INTV_E;

/** OFDM Mode, used in multi-carrier modulation*/
/** CNcomment:OFDM模式，用于多载波调制模式下*/
typedef enum hiUNF_TUNER_FE_FFT_E
{
    HI_UNF_TUNER_FE_FFT_DEFAULT = 0,   /**<default mode*/     /**<CNcomment:默认模式*/
    HI_UNF_TUNER_FE_FFT_1K ,           /**<1k mode*/          /**<CNcomment:1k模式*/
    HI_UNF_TUNER_FE_FFT_2K ,           /**<2k mode*/          /**<CNcomment:2k模式*/
    HI_UNF_TUNER_FE_FFT_4K ,           /**<4k mode*/          /**<CNcomment:4k模式*/
    HI_UNF_TUNER_FE_FFT_8K ,           /**<8k mode*/          /**<CNcomment:8k模式*/
    HI_UNF_TUNER_FE_FFT_16K ,          /**<16k mode*/         /**<CNcomment:16k模式*/
    HI_UNF_TUNER_FE_FFT_32K ,          /**<32k mode*/         /**<CNcomment:32k模式*/
    HI_UNF_TUNER_FE_FFT_BUTT ,         /**<Invalid value*/    /**<CNcomment:非法边界值*/
}HI_UNF_TUNER_FE_FFT_E;

/** Hierarchical modulation mode, only used in DVB-T*/
/** CNcomment:仅用于DVB-T*/
typedef enum hiUNF_TUNER_FE_HIERARCHY_E
{
    HI_UNF_TUNER_FE_HIERARCHY_DEFAULT = 0 ,   /**<hierarchical modulation default mode*/     /**<CNcomment:默认模式*/
    HI_UNF_TUNER_FE_HIERARCHY_NO,             /**<no hierarchical modulation mode*/          /**<CNcomment:不分级别模式*/
    HI_UNF_TUNER_FE_HIERARCHY_ALHPA1,         /**<hierarchical mode, alpha = 1*/             /**<CNcomment:分级别模式, alpha = 1*/
    HI_UNF_TUNER_FE_HIERARCHY_ALHPA2,         /**<hierarchical mode, alpha = 2*/             /**<CNcomment:分级别模式, alpha = 2*/
    HI_UNF_TUNER_FE_HIERARCHY_ALHPA4,         /**<hierarchical mode, alpha = 4*/             /**<CNcomment:分级别模式, alpha = 4*/
    HI_UNF_TUNER_FE_HIERARCHY_BUTT,           /**<Invalid value*/                            /**<CNcomment:非法边界值*/
}HI_UNF_TUNER_FE_HIERARCHY_E ;

/** TS Priority, only used in DVB-T*/
/** CNcomment:仅用于DVB-T*/
typedef enum hiUNF_TUNER_TS_PRIORITY_E
{
    HI_UNF_TUNER_TS_PRIORITY_NONE = 0 ,      /**<no priority mode*/         /**<CNcomment:无优先级模式*/
    HI_UNF_TUNER_TS_PRIORITY_HP,             /**<high priority mode*/       /**<CNcomment:高优先级模式*/
    HI_UNF_TUNER_TS_PRIORITY_LP,             /**<low priority mode*/        /**<CNcomment:低优先级模式*/
    HI_UNF_TUNER_TS_PRIORITY_BUTT,           /**<Invalid value*/            /**<CNcomment:非法边界值*/
}HI_UNF_TUNER_TS_PRIORITY_E ;

/** Terestrial connect param*/
/** CNcomment:地面信号锁台参数*/
typedef struct  hiUNF_TER_CONNECT_PARA_S
{
    HI_U32          u32Freq     ;                    /**<freq in KHz*/           /**<CNcomment:频率，单位kHz*/
    HI_U32          u32BandWidth    ;                /**<bandwidth in KHz*/      /**<CNcomment:带宽，单位kHz*/
    HI_UNF_MODULATION_TYPE_E enModType  ;            /**<modulation type*/       /**<CNcomment:调制方式*/
    HI_BOOL                     bReverse ;          /**<Spectrum reverse mode*/  /**<CNcomment:频谱翻转处理方式*/
}HI_UNF_TER_CONNECT_PARA_S ;

/** TS clock polarization*/
/** CNcomment:TS时钟极性*/
typedef enum hiUNF_TUNER_TSCLK_POLAR_E
{
    HI_UNF_TUNER_TSCLK_POLAR_FALLING,       /**<Falling edge*/      /**<CNcomment:下降沿*/
    HI_UNF_TUNER_TSCLK_POLAR_RISING,        /**<Rising edge*/       /**<CNcomment:上升沿*/
    HI_UNF_TUNER_TSCLK_POLAR_BUTT           /**<Invalid value*/     /**<CNcomment:非法边界值*/
} HI_UNF_TUNER_TSCLK_POLAR_E;

/** TS format*/
/** CNcomment:TS格式*/
typedef enum hiUNF_TUNER_TS_FORMAT_E
{
    HI_UNF_TUNER_TS_FORMAT_TS,              /**<188*/               /**<CNcomment:188字节格式*/
    HI_UNF_TUNER_TS_FORMAT_TSP,             /**<204*/               /**<CNcomment:204字节格式*/
    HI_UNF_TUNER_TS_FORMAT_BUTT             /**<Invalid value*/     /**<CNcomment:非法边界值*/
} HI_UNF_TUNER_TS_FORMAT_E;

/** TS serial PIN*/
/** CNcomment:串行TS数据管脚*/
typedef enum hiUNF_TUNER_TS_SERIAL_PIN_E
{
    HI_UNF_TUNER_TS_SERIAL_PIN_0,           /**<Serial pin 0, default*/  /**<CNcomment:串行模式，数据线使用第0bit*/
    HI_UNF_TUNER_TS_SERIAL_PIN_7,           /**<Serial pin 7*/           /**<CNcomment:串行模式，数据线使用第7bit*/
    HI_UNF_TUNER_TS_SERIAL_PIN_BUTT         /**<Invalid value*/          /**<CNcomment:非法边界值*/
} HI_UNF_TUNER_TS_SERIAL_PIN_E;

/** Tuner RF AGC mode*/
/** CNcomment:agc控制模式*/
typedef enum hiUNF_TUNER_RFAGC_MODE_E
{
    /**< Inverted polarization, default.This setting is used for a tuner whose gain decreases with increased AGC voltage*/   /**<CNcomment:agc反向控制模式*/
    HI_UNF_TUNER_RFAGC_INVERT, 

    /**< Normal polarization. This setting is used for a tuner whose gain increases with increased AGC voltage*/             /**<CNcomment:agc正向控制模式*/
    HI_UNF_TUNER_RFAGC_NORMAL,
    HI_UNF_TUNER_RFAGC_BUTT                  /**<Invalid value*/                                                             /**<CNcomment:非法边界值*/
} HI_UNF_TUNER_RFAGC_MODE_E;

/** Tuner IQ spectrum mode*/
/** CNcomment:IQ模式*/
typedef enum hiUNF_TUNER_IQSPECTRUM_MODE_E
{
    HI_UNF_TUNER_IQSPECTRUM_NORMAL,         /**<The received signal spectrum is not inverted*/    /**<CNcomment:IQ不反转*/
    HI_UNF_TUNER_IQSPECTRUM_INVERT,         /**<The received signal spectrum is inverted*/        /**<CNcomment:IQ反转*/
    HI_UNF_TUNER_IQSPECTRUM_BUTT            /**<Invalid value*/                                   /**<CNcomment:非法边界值*/
} HI_UNF_TUNER_IQSPECTRUM_MODE_E;

/** DiSEqC Wave Mode*/
/** CNcomment:DiSEqC模式*/
typedef enum hiUNF_TUNER_DISEQCWAVE_MODE_E
{
    HI_UNF_TUNER_DISEQCWAVE_NORMAL,         /**<Waveform produced by demod*/                 /**<CNcomment:波形由demod产生*/
    HI_UNF_TUNER_DISEQCWAVE_ENVELOPE,       /**<Waveform produced by LNB control device*/    /**<CNcomment:波形由控制芯片产生*/
    HI_UNF_TUNER_DISEQCWAVE_BUTT            /**<Invalid value*/                              /**<CNcomment:非法边界值*/
} HI_UNF_TUNER_DISEQCWAVE_MODE_E;

/** LNB power supply and control device*/
/** CNcomment:LNB供电和控制芯片*/
typedef enum hiUNF_LNBCTRL_DEV_TYPE_E
{
    HI_UNF_LNBCTRL_DEV_TYPE_NONE,           /**<No LNB control device*/      /**<CNcomment:无控制芯片*/
    HI_UNF_LNBCTRL_DEV_TYPE_MPS8125,        /**<MPS8125*/                    /**<CNcomment:MPS8125*/
    HI_UNF_LNBCTRL_DEV_TYPE_ISL9492,        /**<ISL9492*/                    /**<CNcomment:ISL9492*/
    HI_UNF_LNBCTRL_DEV_TYPE_BUTT            /**<Invalid value*/              /**<CNcomment:非法边界值*/
} HI_UNF_LNBCTRL_DEV_TYPE_E;

/** Satellite extended attribution*/
/** CNcomment:卫星机附加属性*/
typedef struct  hiUNF_TUNER_SAT_ATTR_S
{
    HI_U32                         u32DemodClk;     /**<Demod reference clock frequency, KHz*/    /**<CNcomment:demod参考时钟频率，单位MHz*/
    HI_U16                         u16TunerMaxLPF;  /**<Tuner max LPF, MHz*/                      /**<CNcomment:tuner低通滤波器通带频率，单位MHz*/
    HI_U16                         u16TunerI2CClk;  /**<Tuner I2C clock, kHz*/                    /**<CNcomment:tuner i2c时钟频率*/
    HI_UNF_TUNER_RFAGC_MODE_E      enRFAGC;         /**<Tuner RF AGC mode*/                       /**<CNcomment:agc模式*/
    HI_UNF_TUNER_IQSPECTRUM_MODE_E enIQSpectrum;    /**<Tuner IQ spectrum mode*/                  /**<CNcomment:IQ模式*/
    HI_UNF_TUNER_TSCLK_POLAR_E     enTSClkPolar;    /**<TS clock polarization*/                   /**<CNcomment:ts时钟极性*/
    HI_UNF_TUNER_TS_FORMAT_E       enTSFormat;      /**<TS format*/                               /**<CNcomment:ts格式*/
    HI_UNF_TUNER_TS_SERIAL_PIN_E   enTSSerialPIN;   /**<TS serial PIN*/                           /**<CNcomment:ts串行数据线*/
    HI_UNF_TUNER_DISEQCWAVE_MODE_E enDiSEqCWave;    /**<DiSEqC Wave Mode*/                        /**<CNcomment:DiSEqC模式*/
    HI_UNF_LNBCTRL_DEV_TYPE_E      enLNBCtrlDev;    /**<LNB power supply and control device*/     /**<CNcomment:LNB控制芯片*/
    HI_U16                         u16LNBDevAddress;/**<LNB control device address*/              /**<CNcomment:LNB控制芯片i2c地址*/
} HI_UNF_TUNER_SAT_ATTR_S;

/** Polarization type*/
/** CNcomment:极化方式*/
typedef enum hiUNF_TUNER_FE_POLARIZATION_E
{
    HI_UNF_TUNER_FE_POLARIZATION_H,         /**<Horizontal Polarization*/           /**<CNcomment:水平极化*/
    HI_UNF_TUNER_FE_POLARIZATION_V,         /**<Vertical Polarization*/             /**<CNcomment:垂直极化*/
    HI_UNF_TUNER_FE_POLARIZATION_L,         /**<Left-hand circular Polarization*/   /**<CNcomment:左旋圆极化*/
    HI_UNF_TUNER_FE_POLARIZATION_R,         /**<Right-hand circular Polarization*/  /**<CNcomment:右旋圆极化*/
    HI_UNF_TUNER_FE_POLARIZATION_BUTT,      /**<Invalid value*/                     /**<CNcomment:非法边界值*/
} HI_UNF_TUNER_FE_POLARIZATION_E;

/** FEC Type*/
/** CNcomment:卫星标准*/
typedef enum hiUNF_TUNER_FE_FECTYPE_E
{
    HI_UNF_TUNER_FE_DVBS,                 /**<DVB-S*/                      /**<CNcomment:DVB-S标准*/
    HI_UNF_TUNER_FE_DVBS2,                /**<DVB-S2*/                     /**<CNcomment:DVB-S2标准*/
    HI_UNF_TUNER_FE_DIRECTV,              /**<DIRECTV*/                    /**<CNcomment:DIRECTV标准*/
    HI_UNF_TUNER_FE_BUTT                  /**<Invalid value*/              /**<CNcomment:非法边界值*/
} HI_UNF_TUNER_FE_FECTYPE_E;

/** FEC code Rate*/
/** CNcomment:FEC码率*/
typedef enum hiUNF_TUNER_FE_FECRATE_E
{
    HI_UNF_TUNER_FE_FEC_AUTO = 0,
    HI_UNF_TUNER_FE_FEC_1_2,           /**<1/2*/           /**<CNcomment:1/2码率*/
    HI_UNF_TUNER_FE_FEC_2_3,           /**<2/3*/           /**<CNcomment:2/3码率*/
    HI_UNF_TUNER_FE_FEC_3_4,           /**<3/4*/           /**<CNcomment:3/4码率*/
    HI_UNF_TUNER_FE_FEC_4_5,           /**<4/5*/           /**<CNcomment:4/5码率*/
    HI_UNF_TUNER_FE_FEC_5_6,           /**<5/6*/           /**<CNcomment:5/6码率*/
    HI_UNF_TUNER_FE_FEC_6_7,           /**<6/7*/           /**<CNcomment:6/7码率*/
    HI_UNF_TUNER_FE_FEC_7_8,           /**<7/8*/           /**<CNcomment:7/8码率*/
    HI_UNF_TUNER_FE_FEC_8_9,           /**<8/9*/           /**<CNcomment:8/9码率*/
    HI_UNF_TUNER_FE_FEC_9_10,          /**<9/10*/          /**<CNcomment:9/10码率*/
    HI_UNF_TUNER_FE_FEC_1_4,           /**<1/4*/           /**<CNcomment:1/4码率*/
    HI_UNF_TUNER_FE_FEC_1_3,           /**<1/3*/           /**<CNcomment:1/3码率*/
    HI_UNF_TUNER_FE_FEC_2_5,           /**<2/5*/           /**<CNcomment:2/5码率*/
    HI_UNF_TUNER_FE_FEC_3_5,           /**<3/5*/           /**<CNcomment:3/5码率*/
    HI_UNF_TUNER_FE_FECRATE_BUTT       /**<Invalid value*/ /**<CNcomment:非法边界值*/
} HI_UNF_TUNER_FE_FECRATE_E;

/** LNB type*/
/** CNcomment:LNB类型*/
typedef enum hiUNF_TUNER_FE_FE_LNBTYPE_E
{
    HI_UNF_TUNER_FE_LNB_SINGLE_FREQUENCY,   /**<Single LO frequency*/      /**<CNcomment:单本振*/
    HI_UNF_TUNER_FE_LNB_DUAL_FREQUENCY,     /**<Dual LO frequency*/        /**<CNcomment:双本振*/
    HI_UNF_TUNER_FE_LNB_UNICABLE,           /**<Unicable LNB */            /**<CNcomment:unicable高频头*/
    HI_UNF_TUNER_FE_LNB_TYPE_BUTT           /**<Invalid value*/            /**<CNcomment:非法边界值*/
} HI_UNF_TUNER_FE_LNB_TYPE_E;

/** LNB band type*/
/** CNcomment:卫星信号频段*/
typedef enum hiUNF_TUNER_FE_LNB_BAND_E
{
    HI_UNF_TUNER_FE_LNB_BAND_C,             /**<C */                    /**<CNcomment:C波段*/
    HI_UNF_TUNER_FE_LNB_BAND_KU,            /**<Ku */                   /**<CNcomment:Ku波段*/
    HI_UNF_TUNER_FE_LNB_BAND_BUTT           /**<Invalid value*/         /**<CNcomment:非法边界值*/
} HI_UNF_TUNER_FE_LNB_BAND_E;

/** LNB power control*/
/** CNcomment:高频头供电控制*/
typedef enum hiUNF_TUNER_FE_LNB_POWER_E
{
    HI_UNF_TUNER_FE_LNB_POWER_OFF,          /**<LNB power off*/                                                     /**<CNcomment:关断*/
    HI_UNF_TUNER_FE_LNB_POWER_ON,           /**<LNB power auto, 13V/18V, default*/                                  /**<CNcomment:默认的13/18V供电*/
    HI_UNF_TUNER_FE_LNB_POWER_ENHANCED,     /**<LNB power auto, 14V/19V, some LNB control device can support.*/     /**<CNcomment:加强供电*/
    HI_UNF_TUNER_FE_LNB_POWER_BUTT          /**<Invalid value*/                                                     /**<CNcomment:非法边界值*/
} HI_UNF_TUNER_FE_LNB_POWER_E;

/** LNB 22K tone status, for Ku band LNB*/
/** CNcomment:22k信号状态，用于Ku双本振*/
typedef enum hiUNF_TUNER_FE_LNB_22K_E
{
    HI_UNF_TUNER_FE_LNB_22K_OFF,            /**<22k off*/                    /**<CNcomment:22k信号关，选择低本振*/
    HI_UNF_TUNER_FE_LNB_22K_ON,             /**<22k on*/                     /**<CNcomment:22k信号开，选择高本振*/
    HI_UNF_TUNER_FE_LNB_22K_BUTT            /**<Invalid value*/              /**<CNcomment:非法边界值*/
} HI_UNF_TUNER_FE_LNB_22K_E;

/** 0/12V switch*/
/** CNcomment:0/12V开关*/
typedef enum hiUNF_TUNER_SWITCH_0_12V_E
{
    HI_UNF_TUNER_SWITCH_0_12V_NONE,         /**< None, default*/            /**<CNcomment:不接开关状态*/
    HI_UNF_TUNER_SWITCH_0_12V_0,            /**< 0V*/                       /**<CNcomment:0V状态*/
    HI_UNF_TUNER_SWITCH_0_12V_12,           /**< 12V*/                      /**<CNcomment:12V状态*/
    HI_UNF_TUNER_SWITCH_0_12V_BUTT          /**<Invalid value*/             /**<CNcomment:非法边界值*/
} HI_UNF_TUNER_SWITCH_0_12V_E;

/** 22KHz switch*/
/** CNcomment:22K开关*/
typedef enum hiUNF_TUNER_SWITCH_22K_E
{
    HI_UNF_TUNER_SWITCH_22K_NONE,           /**< None, default*/            /**<CNcomment:不接开关状态*/
    HI_UNF_TUNER_SWITCH_22K_0,              /**< 0*/                        /**<CNcomment:0kHz端口*/
    HI_UNF_TUNER_SWITCH_22K_22,             /**< 22KHz*/                    /**<CNcomment:22kHz端口*/
    HI_UNF_TUNER_SWITCH_22K_BUTT            /**<Invalid value*/             /**<CNcomment:非法边界值*/
} HI_UNF_TUNER_SWITCH_22K_E;

/** Tone burst switch*/
/** CNcomment:Tone burst开关*/
typedef enum hiUNF_TUNER_SWITCH_TONEBURST_E
{
    HI_UNF_TUNER_SWITCH_TONEBURST_NONE,     /**< Don't send tone burst, default*/ /**<CNcomment:不接开关状态*/
    HI_UNF_TUNER_SWITCH_TONEBURST_0,        /**< Tone burst 0*/                   /**<CNcomment:0 port*/
    HI_UNF_TUNER_SWITCH_TONEBURST_1,        /**< Tone burst 1*/                   /**<CNcomment:1 port*/
    HI_UNF_TUNER_SWITCH_TONEBURST_BUTT      /**<Invalid value*/                   /**<CNcomment:非法边界值*/
} HI_UNF_TUNER_SWITCH_TONEBURST_E;

/** Structure of the satellite transmission signal.*/
/** CNcomment:定义Satellite传输信号*/
typedef struct  hiUNF_SAT_CONNECT_PARA_S
{
    HI_U32                         u32Freq;         /**<Downlink frequency, in kHz*/ /**<CNcomment:下行频率，单位：kHz*/
    HI_U32                         u32SymbolRate;   /**<Symbol rate, in Symb/s*/     /**<CNcomment:符号率，单位：Symb/s */
    HI_UNF_TUNER_FE_POLARIZATION_E enPolar;         /**<Polarization type*/          /**<CNcomment:极化方式*/
} HI_UNF_SAT_CONNECT_PARA_S;

/** Structure of the satellite transmission signal's detailed information.*/
/** CNcomment:卫星信号详细信息*/
typedef struct  hiUNF_TUNER_SAT_SIGNALINFO_S
{
    HI_U32                         u32Freq;         /**<Downlink frequency, in kHz*/              /**<CNcomment:下行频率，单位：kHz*/
    HI_U32                         u32SymbolRate;   /**<Symbol rate, in Symb/s*/                  /**<CNcomment:符号率，单位Symb/s*/
    HI_UNF_MODULATION_TYPE_E       enModType;       /**<Modulation type*/                         /**<CNcomment:调制方式*/
    HI_UNF_TUNER_FE_POLARIZATION_E enPolar;         /**<Polarization type*/                       /**<CNcomment:极化方式*/
    HI_UNF_TUNER_FE_FECTYPE_E      enSATType;       /**<Saterllite standard, DVB-S/S2/DIRECTV*/   /**<CNcomment:卫星标准， 支持DVB-S/S2/DIRECTV*/
    HI_UNF_TUNER_FE_FECRATE_E      enFECRate;       /**<FEC rate*/                                /**<CNcomment:前向纠错码率*/
} HI_UNF_TUNER_SAT_SIGNALINFO_S;

/** PLP type of DVB-T2.*/
/** CNcomment:T2下物理层管道类型*/
typedef enum hiUNF_TUNER_T2_PLP_TYPE_E
{   
    HI_UNF_TUNER_T2_PLP_TYPE_COM=0,         /**<common type*/              /**<CNcomment:普通*/
    HI_UNF_TUNER_T2_PLP_TYPE_DAT1,          /**<data1 type*/               /**<CNcomment:数据1*/
    HI_UNF_TUNER_T2_PLP_TYPE_DAT2,          /**<data2 type*/               /**<CNcomment:数据2*/
    HI_UNF_TUNER_T2_PLP_TYPE_BUTT           /**<Invalid value*/            /**<CNcomment:非法边界值*/
} HI_UNF_TUNER_T2_PLP_TYPE_E;

/** Structure of the terrestrial transmission signal's detailed information.*/
/** CNcomment:地面信号详细信息*/
typedef struct  hiUNF_TUNER_TER_SIGNALINFO_S
{
    HI_U32                         u32Freq;         /**<Frequency, in kHz*/          /**<CNcomment:频率，单位：kHz*/
    HI_U32                         u32BandWidth;    /**<Band width, in KHz*/         /**<CNcomment:带宽，单位KHz */
    HI_UNF_MODULATION_TYPE_E       enModType;       /**<Modulation type*/            /**<CNcomment:调制方式*/
    HI_UNF_TUNER_FE_FECRATE_E      enFECRate;       /**<FEC rate*/                   /**<CNcomment:前向纠错码率*/
    HI_UNF_TUNER_FE_GUARD_INTV_E enGuardIntv;       /**<GI mode*/                    /**<CNcomment:保护间隔模式*/
    HI_UNF_TUNER_FE_FFT_E enFFTMode;                /**<FFT mode*/                   /**<CNcomment:FFT模式*/
    HI_UNF_TUNER_FE_HIERARCHY_E enHierMod;          /**<Hierarchical Modulation and alpha, only used in DVB-T*/
                                                                                     /**<CNcomment:hierachical模式和alpha值*/
    HI_UNF_TUNER_TS_PRIORITY_E enTsPriority;        /**<The TS priority, only used in DVB-T*/
                                                                                     /**<CNcomment:ts priority, 仅用于DVB-T模式下*/
} HI_UNF_TUNER_TER_SIGNALINFO_S;

/** signal information.*/
/** CNcomment:TUNER信号属性*/
typedef struct hiUNF_TUNER_SIGNALINFO_S
{
    HI_UNF_TUNER_SIG_TYPE_E enSigType;              /**<Signal transmission type*/    /**<CNcomment:信号类型*/

    union
    {
        HI_UNF_TUNER_SAT_SIGNALINFO_S stSat;        /**<Signal info of satellite*/    /**<CNcomment:卫星信号信息*/
        HI_UNF_TUNER_TER_SIGNALINFO_S stTer;        /**<Signal info of terrestrial*/  /**<CNcomment:地面信号信息*/
    } unSignalInfo;
} HI_UNF_TUNER_SIGNALINFO_S;

/** unicable multi-switch port.*/
/** CNcomment:unicable开关端口枚举*/
typedef enum hiUNF_TUNER_SATPOSITION_E
{
    HI_UNF_TUNER_SATPOSN_A,    /**<unicable switch port A*/    /**<CNcomment:端口A*/
    HI_UNF_TUNER_SATPOSN_B,    /**<unicable switch port B*/    /**<CNcomment:端口B*/
    HI_UNF_TUNER_SATPOSN_BUT   /**<Invalid value*/             /**<CNcomment:非法边界值*/
} HI_UNF_TUNER_SATPOSITION_E;

/** LNB configurating parameters*/
/** CNcomment:LNB配置参数*/
typedef struct hiUNF_TUNER_FE_LNB_CONFIG_S
{
    HI_UNF_TUNER_FE_LNB_TYPE_E  enLNBType;      /**<LNB type*/                              /**<CNcomment:LNB类型*/
    HI_U32                      u32LowLO;       /**< Low Local Oscillator Frequency, MHz */ /**<CNcomment:LNB低本振频率，单位MHz*/
    HI_U32                      u32HighLO;      /**< High Local Oscillator Frequency, MHz*/ /**<CNcomment:LNB高本振频率，单位MHz*/
    HI_UNF_TUNER_FE_LNB_BAND_E  enLNBBand;      /**< LNB band, C or Ku */                   /**<CNcomment:LNB波段：C或Ku*/
    HI_U8 u8UNIC_SCRNO;                         /**< SCR number, 0-7 */                     /**<CNcomment:SCR序号，取值为0-7*/
    HI_U32 u32UNICIFFreqMHz;                    /**< SCR IF frequency, unit MHz */          /**<CNcomment:SCR中频频率， 单位MHz*/
    HI_UNF_TUNER_SATPOSITION_E enSatPosn;       /**< unicable multi-switch port */          /**<CNcomment:unicable开关端口号*/
} HI_UNF_TUNER_FE_LNB_CONFIG_S;

/** tuner attribution.*/
/** CNcomment:TUNER属性*/
typedef struct  hiTUNER_ATTR_S
{
    HI_UNF_TUNER_SIG_TYPE_E	enSigType ;         /**<Signal type*/                             /**<CNcomment:信号类型*/
    HI_UNF_TUNER_DEV_TYPE_E	enTunerDevType;     /**<Tuner type*/                                  /**<CNcomment:TUNER类型*/
    HI_U32	u32TunerAddr;                                      /*The i2c address of tuner, if demod type is 3130I,3130E or J83B, you cannot set it*/
    HI_UNF_DEMOD_DEV_TYPE_E	enDemodDevType;     /**<QAM type*/                                    /**<CNcomment:QAM类型*/
    HI_U32	u32DemodAddr;                                   /*The i2c address of demod, if demod type is 3130I,3130E or J83B, you cannot set it*/
    HI_UNF_TUNER_OUPUT_MODE_E	enOutputMode ;      /**<Output mode of transport streams (TSs)*/  /**<CNcomment:TS流输出模式*/
    HI_U8	enI2cChannel;       /**<I2C channel used by the tuner*/                               /**<CNcomment:TUNER使用的I2C通道*/
    HI_U32	u32ResetGpioNo;  /**<Demod reset GPIO NO.*/                    /**<CNcomment:复位使用的gpio管脚号*/
} HI_UNF_TUNER_ATTR_S ;

/** Frequency locking parameters of the tuner*/
/** CNcomment:TUNER锁频参数*/
typedef struct  hiUNF_TUNER_CONNECT_PARA_S
{
    HI_UNF_TUNER_SIG_TYPE_E enSigType ;        /**<Signal type*/                      /**<CNcomment:信号类型*/

    union
    {
        HI_UNF_CAB_CONNECT_PARA_S   stCab ;   /**<Cable transmission signal*/         /**<CNcomment:CABLE传输信号*/
        HI_UNF_TER_CONNECT_PARA_S stTer ;       /**<Terrestrial transmission signal*/ /**<CNcomment:地面传输信号*/
        HI_UNF_SAT_CONNECT_PARA_S stSat;        /**<Satellite transmission signal*/   /**<CNcomment:卫星传输信号*/
    } unConnectPara;
} HI_UNF_TUNER_CONNECT_PARA_S ;

/** Frequency locking status and parameters of the tuner*/
/** CNcomment:TUNER锁频状态和锁频参数*/
typedef struct  hiUNF_TUNER_STATUS_S
{
    HI_UNF_TUNER_LOCK_STATUS_E  enLockStatus ;  /**<Frequency locking status*/               /**<CNcomment:锁频状态*/
    HI_UNF_TUNER_CONNECT_PARA_S stConnectPara;  /**<Actual frequency locking parameters*/    /**<CNcomment:实际锁频参数*/
} HI_UNF_TUNER_STATUS_S ;

/** Tuner blind scan type*/
/** CNcomment:TUNER盲扫方式*/
typedef enum hiUNF_TUNER_BLINDSCAN_MODE_E
{
    HI_UNF_TUNER_BLINDSCAN_MODE_AUTO = 0,       /**<Blind scan automatically*/  /**<CNcomment:自动扫描*/
    HI_UNF_TUNER_BLINDSCAN_MODE_MANUAL,         /**<Blind scan manually*/       /**<CNcomment:手动扫描*/
    HI_UNF_TUNER_BLINDSCAN_MODE_BUTT            /**<Invalid value*/             /**<CNcomment:非法边界值*/
} HI_UNF_TUNER_BLINDSCAN_MODE_E;

/** Definition of blind scan event type*/
/** CNcomment:TUNER盲扫事件*/
typedef enum hiUNF_TUNER_BLINDSCAN_EVT_E
{
    HI_UNF_TUNER_BLINDSCAN_EVT_STATUS,          /**<New status*/        /**<CNcomment:状态变化*/
    HI_UNF_TUNER_BLINDSCAN_EVT_PROGRESS,        /**<New Porgress */     /**<CNcomment:进度变化*/
    HI_UNF_TUNER_BLINDSCAN_EVT_NEWRESULT,       /**<Find new channel*/  /**<CNcomment:新频点*/
    HI_UNF_TUNER_BLINDSCAN_EVT_BUTT             /**<Invalid value*/     /**<CNcomment:非法边界值*/
} HI_UNF_TUNER_BLINDSCAN_EVT_E;

/** Definition of tuner blind scan status*/
/** CNcomment:TUNER盲扫状态*/
typedef enum hiUNF_TUNER_BLINDSCAN_STATUS_E
{
    HI_UNF_TUNER_BLINDSCAN_STATUS_IDLE,         /**<Idel*/              /**<CNcomment:空闲*/
    HI_UNF_TUNER_BLINDSCAN_STATUS_SCANNING,     /**<Scanning*/          /**<CNcomment:扫描中*/
    HI_UNF_TUNER_BLINDSCAN_STATUS_FINISH,       /**<Finish*/            /**<CNcomment:成功完成*/
    HI_UNF_TUNER_BLINDSCAN_STATUS_QUIT,         /**<User quit*/         /**<CNcomment:用户退出*/
    HI_UNF_TUNER_BLINDSCAN_STATUS_FAIL,         /**<Scan fail*/         /**<CNcomment:扫描失败*/
    HI_UNF_TUNER_BLINDSCAN_STATUS_BUTT          /**<Invalid value*/     /**<CNcomment:非法边界值*/
} HI_UNF_TUNER_BLINDSCAN_STATUS_E;

/** Structure of satellite TP*/
/** CNcomment:TUNER扫出TP信息*/
typedef struct  hiUNF_TUNER_SAT_TPINFO_S
{
    HI_U32                         u32Freq;         /**<Downlink frequency, in kHz*/ /**<CNcomment:下行频率，单位：kHz*/
    HI_U32                         u32SymbolRate;   /**<Symbol rate, in bit/s*/      /**<CNcomment:符号率，单位bps */
    HI_UNF_TUNER_FE_POLARIZATION_E enPolar;         /**<Polarization type*/          /**<CNcomment:极化方式*/
    HI_U8 cbs_reliablity;                           /**<TP reliability*/             /**<CNcomment:TP的可靠度*/
} HI_UNF_TUNER_SAT_TPINFO_S;

/** Notify structure of tuner blind scan */
/** CNcomment:TUNER盲扫通知信息*/
typedef union hiUNF_TUNER_BLINDSCAN_NOTIFY_U
{
    HI_UNF_TUNER_BLINDSCAN_STATUS_E* penStatus;             /**<Scanning status*/            /**<CNcomment:盲扫状态*/
    HI_U16*                          pu16ProgressPercent;   /**<Scanning progress*/          /**<CNcomment:盲扫进度*/
    HI_UNF_TUNER_SAT_TPINFO_S* pstResult;                   /**<Scanning result*/            /**<CNcomment:盲扫结果*/
} HI_UNF_TUNER_BLINDSCAN_NOTIFY_U;

/** Parameter of the satellite tuner blind scan */
/** CNcomment:卫星TUNER盲扫参数*/
typedef struct hiUNF_TUNER_SAT_BLINDSCAN_PARA_S
{
    /**<LNB Polarization type, only take effect in manual blind scan mode*/
    /**<CNcomment:LNB极化方式，自动扫描模式设置无效*/
    HI_UNF_TUNER_FE_POLARIZATION_E enPolar;
    
    /**<LNB 22K signal status, for Ku band LNB which has dual LO, 22K ON will select high LO and 22K off select low LO,
        only take effect in manual blind scan mode*/
    /**<CNcomment:LNB 22K状态，对于Ku波段双本振LNB，ON选择高本振，OFF选择低本振，自动扫描模式设置无效*/
    HI_UNF_TUNER_FE_LNB_22K_E      enLNB22K;

    /**<Blind scan start IF, in kHz, only take effect in manual blind scan mode */
    /**<CNcomment:盲扫起始频率(中频)，单位：kHz，自动扫描模式设置无效*/
    HI_U32                         u32StartFreq;   

    /**<Blind scan stop IF, in kHz, only take effect in manual blind scan mode */
    /**<CNcomment:盲扫结束频率(中频)，单位：kHz，自动扫描模式设置无效*/
    HI_U32                         u32StopFreq; 

    /**<The execution of the blind scan may change the 13/18V or 22K status. 
        If you use any DiSEqC device which need send command when 13/18V or 22K status change,
        you should registe a callback here. Otherwise, you can set NULL here.*/
    /**<CNcomment:盲扫过程可能会切换极化方式和22K，如果你用了某些DiSEqC设备需要设置13/18V和22K的，
        请注册这个回调，如果没有用，请可传NULL */
    HI_VOID (*pfnDISEQCSet)(HI_U32 u32TunerId, HI_UNF_TUNER_FE_POLARIZATION_E enPolar,
                            HI_UNF_TUNER_FE_LNB_22K_E enLNB22K);

    /**<Callback when scan status change, scan progress change or find new channel.*/
    /**<CNcomment:扫描状态或进度百分比发生变化时、发现新的频点时回调*/
    HI_VOID (*pfnEVTNotify)(HI_U32 u32TunerId, HI_UNF_TUNER_BLINDSCAN_EVT_E enEVT, HI_UNF_TUNER_BLINDSCAN_NOTIFY_U * punNotify);
} HI_UNF_TUNER_SAT_BLINDSCAN_PARA_S;

/** Parameter of the tuner blind scan */
/** CNcomment:TUNER盲扫参数*/
typedef struct hiUNF_TUNER_BLINDSCAN_PARA_S
{
    HI_UNF_TUNER_BLINDSCAN_MODE_E enMode;            /**<Scanning mode*/                /**<CNcomment:盲扫模式*/
    union
    {
        HI_UNF_TUNER_SAT_BLINDSCAN_PARA_S stSat;    /**<Scanning parameter*/            /**<CNcomment:盲扫参数*/
    } unScanPara;
} HI_UNF_TUNER_BLINDSCAN_PARA_S;


/** DiSEqC Level*/
/** CNcomment:DiSEqC设备版本*/
typedef enum hiUNF_TUNER_DISEQC_LEVEL_E
{
    HI_UNF_TUNER_DISEQC_LEVEL_1_X,      /**<1.x, one way*/                  /**<CNcomment:1.x，单向 */
    HI_UNF_TUNER_DISEQC_LEVEL_2_X,      /**<2.x, two way, support reply*/   /**<CNcomment:2.x，双向，支持Reply*/
    HI_UNF_TUNER_DISEQC_LEVEL_BUTT      /**<Invalid value*/                 /**<CNcomment:非法边界值*/
} HI_UNF_TUNER_DISEQC_LEVEL_E;

/** Receive status of DiSEqC reply massage*/
/** CNcomment:DiSEqC消息接收状态*/
typedef enum hiUNF_TUNER_DISEQC_RECV_STATUS_E
{
    HI_UNF_TUNER_DISEQC_RECV_OK,        /**<Receive successfully*/          /**<CNcomment:接收成功*/
    HI_UNF_TUNER_DISEQC_RECV_UNSUPPORT, /**<Device don't support reply*/    /**<CNcomment:设备不支持回传*/    
    HI_UNF_TUNER_DISEQC_RECV_TIMEOUT,   /**<Receive timeout*/               /**<CNcomment:接收超时*/
    HI_UNF_TUNER_DISEQC_RECV_ERROR,     /**<Receive fail*/                  /**<CNcomment:接收出错*/
    HI_UNF_TUNER_DISEQC_RECV_BUTT       /**<Invalid value*/                 /**<CNcomment:非法边界值*/
} HI_UNF_TUNER_DISEQC_RECV_STATUS_E;

/** Structure of the DiSEqC send massage */
/** CNcomment:DiSEqC发送消息结构*/
typedef struct hiUNF_TUNER_DISEQC_SENDMSG_S
{
    HI_UNF_TUNER_DISEQC_LEVEL_E     enLevel;        /**<Device level*/                         /**<CNcomment:器件版本*/
    HI_UNF_TUNER_SWITCH_TONEBURST_E enToneBurst;    /**<Tone Burst */                          /**<CNcomment:tone信号状态*/
    HI_U8                           au8Msg[HI_UNF_DISEQC_MSG_MAX_LENGTH]; /**<Message data*/   /**<CNcomment:消息字*/
    HI_U8                           u8Length;       /**<Message length*/                       /**<CNcomment:信息长度*/
    HI_U8                           u8RepeatTimes;  /**<Message repeat times*/                 /**<CNcomment:重传次数*/
} HI_UNF_TUNER_DISEQC_SENDMSG_S;

/** Structure of the DiSEqC reply massage */
/** CNcomment:DiSEqC接收消息结构*/
typedef struct hiUNF_TUNER_DISEQC_RECVMSG_S
{
    HI_UNF_TUNER_DISEQC_RECV_STATUS_E enStatus;     /**<Recieve status*/                                 /**<CNcomment:接收状态*/
    HI_U8                             au8Msg[HI_UNF_DISEQC_MSG_MAX_LENGTH]; /**<Recieve message data*/   /**<CNcomment:接收数据缓存*/
    HI_U8                             u8Length;     /**<Recieve message length*/                         /**<CNcomment:接收数据长度*/
} HI_UNF_TUNER_DISEQC_RECVMSG_S;

/** DiSEqC Switch port*/
/** CNcomment:DiSEqC开关端口枚举*/
typedef enum hiUNF_TUNER_DISEQC_SWITCH_PORT_E
{
    HI_UNF_TUNER_DISEQC_SWITCH_NONE = 0,         /**<none*/                                 /**<CNcomment:不接开关*/
    HI_UNF_TUNER_DISEQC_SWITCH_PORT_1,           /**<port1*/                                /**<CNcomment:端口1*/
    HI_UNF_TUNER_DISEQC_SWITCH_PORT_2,           /**<port2*/                                /**<CNcomment:端口2*/
    HI_UNF_TUNER_DISEQC_SWITCH_PORT_3,           /**<port3*/                                /**<CNcomment:端口3*/
    HI_UNF_TUNER_DISEQC_SWITCH_PORT_4,           /**<port4*/                                /**<CNcomment:端口4*/
    HI_UNF_TUNER_DISEQC_SWITCH_PORT_5,           /**<port5*/                                /**<CNcomment:端口5*/
    HI_UNF_TUNER_DISEQC_SWITCH_PORT_6,           /**<port6*/                                /**<CNcomment:端口6*/
    HI_UNF_TUNER_DISEQC_SWITCH_PORT_7,           /**<port7*/                                /**<CNcomment:端口7*/
    HI_UNF_TUNER_DISEQC_SWITCH_PORT_8,           /**<port8*/                                /**<CNcomment:端口8*/
    HI_UNF_TUNER_DISEQC_SWITCH_PORT_9,           /**<port9*/                                /**<CNcomment:端口9*/
    HI_UNF_TUNER_DISEQC_SWITCH_PORT_10,          /**<port10*/                               /**<CNcomment:端口10*/
    HI_UNF_TUNER_DISEQC_SWITCH_PORT_11,          /**<port11*/                               /**<CNcomment:端口11*/
    HI_UNF_TUNER_DISEQC_SWITCH_PORT_12,          /**<port12*/                               /**<CNcomment:端口12*/
    HI_UNF_TUNER_DISEQC_SWITCH_PORT_13,          /**<port13*/                               /**<CNcomment:端口13*/   
    HI_UNF_TUNER_DISEQC_SWITCH_PORT_14,          /**<port14*/                               /**<CNcomment:端口14*/
    HI_UNF_TUNER_DISEQC_SWITCH_PORT_15,          /**<port15*/                               /**<CNcomment:端口15*/
    HI_UNF_TUNER_DISEQC_SWITCH_PORT_16,          /**<port16*/                               /**<CNcomment:端口16*/
    HI_UNF_TUNER_DISEQC_SWITCH_PORT_BUTT         /**<Invalid value*/                        /**<CNcomment:非法边界值*/
} HI_UNF_TUNER_DISEQC_SWITCH_PORT_E;

/** Parameter for DiSEqC 1.0/2.0 switch
   Some DiSEqC device need set polarization(13/18V) and 22K, you can set them here. */
/** CNcomment:DiSEqC 1.0/2.0 开关参数
   有些DiSEqC设备需要设置极化方式和22K的，如果用了这种设备，需要在这里设置 */
typedef struct hiUNF_TUNER_DISEQC_SWITCH4PORT_S
{
    HI_UNF_TUNER_DISEQC_LEVEL_E       enLevel;  /**<DiSEqC device level*/           /**<CNcomment:器件版本*/
    HI_UNF_TUNER_DISEQC_SWITCH_PORT_E enPort;   /**<DiSEqC switch port*/            /**<CNcomment:选通端口号*/
    HI_UNF_TUNER_FE_POLARIZATION_E    enPolar;  /**<Polarization type */            /**<CNcomment:极化方式*/
    HI_UNF_TUNER_FE_LNB_22K_E         enLNB22K; /**<22K status*/                    /**<CNcomment:22k状态*/
} HI_UNF_TUNER_DISEQC_SWITCH4PORT_S;

/** Parameter for DiSEqC 1.1/2.1 switch */
/** CNcomment:DiSEqC 1.1/2.1 开关参数 */
typedef struct hiUNF_TUNER_DISEQC_SWITCH16PORT_S
{
    HI_UNF_TUNER_DISEQC_LEVEL_E       enLevel;  /**<DiSEqC device level*/            /**<CNcomment:器件版本*/
    HI_UNF_TUNER_DISEQC_SWITCH_PORT_E enPort;   /**<DiSEqC switch port*/             /**<CNcomment:选通端口号*/
} HI_UNF_TUNER_DISEQC_SWITCH16PORT_S;

/** DiSEqC motor limit setting*/
/** CNcomment:DiSEqC马达极限设置*/
typedef enum hiUNF_TUNER_DISEQC_LIMIT_E
{
    HI_UNF_TUNER_DISEQC_LIMIT_OFF,              /**<Disable Limits*/                /**<CNcomment:无限制*/
    HI_UNF_TUNER_DISEQC_LIMIT_EAST,             /**<Set East Limit*/                /**<CNcomment:东向限制*/
    HI_UNF_TUNER_DISEQC_LIMIT_WEST,             /**<Set West Limit*/                /**<CNcomment:西向限制*/
    HI_UNF_TUNER_DISEQC_LIMIT_BUTT              /**<Invalid value*/                 /**<CNcomment:非法边界值*/
} HI_UNF_TUNER_DISEQC_LIMIT_E;

/** Difinition of DiSEqC motor move direction*/
/** CNcomment:DiSEqC马达移动方向*/
typedef enum hiUNF_TUNER_DISEQC_MOVE_DIR_E
{
    HI_UNF_TUNER_DISEQC_MOVE_DIR_EAST,          /**<Move east*/                     /**<CNcomment:向东移动*/
    HI_UNF_TUNER_DISEQC_MOVE_DIR_WEST,          /**<Move west*/                     /**<CNcomment:向西移动*/
    HI_UNF_TUNER_DISEQC_MOVE_DIR_BUTT           /**<Invalid value*/                 /**<CNcomment:非法边界值*/
} HI_UNF_TUNER_DISEQC_MOVE_DIR_E;

/** Difinition of DiSEqC motor move type*/
/** CNcomment:DiSEqC马达移动方式*/
typedef enum hiUNF_TUNER_DISEQC_MOVE_TYPE_E
{
    HI_UNF_TUNER_DISEQC_MOVE_STEP_SLOW,         /**<1 step one time, default*/      /**<CNcomment:缓慢移动*/
    HI_UNF_TUNER_DISEQC_MOVE_STEP_FAST,         /**<5 step one time*/               /**<CNcomment:快速移动*/
    HI_UNF_TUNER_DISEQC_MOVE_CONTINUE,          /**<Continuous moving*/             /**<CNcomment:连续移动*/
    HI_UNF_TUNER_DISEQC_MOVE_TYPE_BUTT          /**<Invalid value*/                 /**<CNcomment:非法边界值*/
} HI_UNF_TUNER_DISEQC_MOVE_TYPE_E;

/** Parameter for DiSEqC motor store position*/
/** CNcomment:天线存储位置参数*/
typedef struct hiUNF_TUNER_DISEQC_POSITION_S
{
    HI_UNF_TUNER_DISEQC_LEVEL_E enLevel;        /**<DiSEqC device level*/               /**<CNcomment:器件版本*/
    HI_U32                      u32Pos;         /**<Index of position, 0-255*/          /**<CNcomment:位置序号*/
} HI_UNF_TUNER_DISEQC_POSITION_S;

/** Parameter for DiSEqC motor limit setting*/
/** CNcomment:天线Limit设置参数*/
typedef struct hiUNF_TUNER_DISEQC_LIMIT_S
{
    HI_UNF_TUNER_DISEQC_LEVEL_E enLevel;        /**<DiSEqC device level*/               /**<CNcomment:器件版本*/
    HI_UNF_TUNER_DISEQC_LIMIT_E enLimit;        /**<Limit setting*/                     /**<CNcomment:限制设定*/
} HI_UNF_TUNER_DISEQC_LIMIT_S;

/** Parameter for DiSEqC motor moving*/
/** CNcomment:DiSEqC马达移动参数*/
typedef struct hiUNF_TUNER_DISEQC_MOVE_S
{
    HI_UNF_TUNER_DISEQC_LEVEL_E     enLevel;    /**<DiSEqC device level*/           /**<CNcomment:器件版本*/
    HI_UNF_TUNER_DISEQC_MOVE_DIR_E  enDir;      /**<Moving direction*/              /**<CNcomment:移动方向*/
    HI_UNF_TUNER_DISEQC_MOVE_TYPE_E enType;     /**<Moving type*/                   /**<CNcomment:移动类型(慢速，快速，连续)*/
} HI_UNF_TUNER_DISEQC_MOVE_S;

/** Parameter for DiSEqC motor recalculate*/
/** CNcomment:DiSEqC天线重计算参数*/
typedef struct hiUNF_TUNER_DISEQC_RECALCULATE_S
{
    HI_UNF_TUNER_DISEQC_LEVEL_E enLevel;        /**<DiSEqC device level*/           /**<CNcomment:器件版本*/
    HI_U8                       u8Para1;        /**<Parameter 1*/                   /**<CNcomment:参数1*/
    HI_U8                       u8Para2;        /**<Parameter 2*/                   /**<CNcomment:参数2*/
    HI_U8                       u8Para3;        /**<Parameter 3*/                   /**<CNcomment:参数3*/
    HI_U8                       u8Reserve;      /**<Reserve*/                       /**<CNcomment:保留参数*/
} HI_UNF_TUNER_DISEQC_RECALCULATE_S;

/** Parameter for USALS*/
/** CNcomment:USALS 参数*/
typedef struct hiUNF_TUNER_DISEQC_USALS_PARA_S
{
    HI_U16 u16LocalLongitude;   /**<local longitude, is 10*longitude, in param, E:0-1800, W:1800-3600(3600-longtitude)*/
                                /**<CNcomment:本地经度，单位0.1度，东经取值范围0-1800，西经取值范围1800-3600，值为3600-经度值*/
    HI_U16 u16LocalLatitude;    /**<local latitude, is 10*latitude, in param N:0-900, S:900-1800(1800-latitude)*/
                                /**<CNcomment:本地纬度，单位0.1度，北纬取值范围0-900，南纬取值范围900-1800，值为1800-纬度值*/
    HI_U16 u16SatLongitude;     /**<sat longitude, is 10*longitude, in param, E:0-1800, W:1800-3600(3600-longtitude)*/
                                /**<CNcomment:卫星经度，单位0.1度，东经取值范围0-1800，西经取值范围1800-3600，值为3600-经度值*/
    HI_U16 u16Angular;          /**<calculate result, out param*/
                                /**<CNcomment:计算结果，输出参数*/
} HI_UNF_TUNER_DISEQC_USALS_PARA_S;

/** Parameter for USALS goto angular*/
/** CNcomment:USALS角度参数*/
typedef struct hiUNF_TUNER_DISEQC_USALS_ANGULAR_S
{
    HI_UNF_TUNER_DISEQC_LEVEL_E enLevel;        /**<DiSEqC device level*/                                       /**<CNcomment:器件版本*/
    HI_U16                      u16Angular;     /**<Angular, calculated by HI_UNF_TUNER_DISEQC_CalcAngular()*/  /**<CNcomment:角度值，可通过函数HI_UNF_TUNER_DISEQC_CalcAngular计算得到*/
} HI_UNF_TUNER_DISEQC_USALS_ANGULAR_S;
//#endif /* DISEQC_SUPPORT */

/** @} */  /** <!-- ==== Structure Definition end ==== */


/******************************* API Declaration *****************************/
/** \addtogroup      FRONTEND */
/** @{ */  /** <!-- [FRONTEND] */
/*---TUNER---*/
/** 
\brief Initializes the tuner.
CNcomment:\brief 初始化TUNER设备。CNend

\attention \n
The error code HI_SUCCESS is returned if this API is called repeatedly.
CNcomment:支持重复初始化TUNER设备，此时直接返回成功。CNend
\param N/A                                    CNcomment:无 CNend
\retval ::HI_SUCCESS Success                  CNcomment:成功 CNend
\retval ::HI_FAILURE  Calling this API fails. CNcomment:API系统调用失败 CNend

\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_Init(HI_VOID);


/** 
\brief Deinitializes the tuner.
CNcomment:\brief 去初始化TUNER设备。CNend

\attention \n
The error code HI_SUCCESS is returned if this API is called repeatedly.
CNcomment:支持重复去初始化TUNER设备，此时直接返回成功。CNend
\param  N/A                    CNcomment:无 CNend
\retval ::HI_SUCCESS Success   CNcomment:成功 CNend
\retval ::HI_FAILURE  Calling this API fails. CNcomment:API系统调用失败 CNend

\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_DeInit(HI_VOID);

/**
\brief  Obtains the default attributes of the tuner. 
CNcomment:\brief  获取TUNER的默认属性。CNend
\attention \n
This API is available only after the tuner is initialized.
CNcomment:此接口在TUNER设备初始化后才能使用。CNend
\param[in] u32tunerId   tuner port ID. The port ID can be 0-2.             CNcomment:TUNER端口号，取值为0-2。CNend
\param[out] pstTunerAttr The default attributes of the tuner are returned. CNcomment:返回当前TUNER的默认属性。CNend
\retval ::HI_SUCCESS Success.									   CNcomment: 成功 CNend
\retval ::HI_FAILURE Calling this API fails.                                               CNcomment: API系统调用失败 CNend
\retval ::HI_ERR_TUNER_NOT_INIT  The Frontend module is not initialized. CNcomment: 模块没有初始化CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:输入PORT 参数非法 CNend
\retval ::HI_ERR_TUNER_INVALID_POINT The input parameter is invalid. CNcomment:输入PORT 参数非法 CNend
\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_GetDeftAttr(HI_U32  u32tunerId , HI_UNF_TUNER_ATTR_S *pstTunerAttr );


/** 
\brief Sets the current attributes of the tuner.
CNcomment:\brief 设置TUNER当前属性。CNend
\attention \n
N/A
\param[in] u32tunerId   tuner port ID. The port ID can be 0-2.      CNcomment:TUNER端口号，取值为0-2 CNend
\param[in] pstTunerAttr tuner attributes                            CNcomment:TUNER的属性。CNend
\retval ::HI_SUCCESS Success                                        CNcomment:成功 CNend
\retval ::HI_FAILURE  Calling this API fails.                       CNcomment:API系统调用失败 CNend
\retval ::HI_ERR_TUNER_NOT_OPEN  The TUNER module is not opened. CNcomment: 模块没有打开CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_INVALID_POINT The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_INVALID_PARA The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_FAILED_SELECTI2CCHANNEL The input I2C channel is invalid. CNcomment:输入I2C 通道非法 CNend
\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_SetAttr(HI_U32  u32tunerId , const HI_UNF_TUNER_ATTR_S *pstTunerAttr );


/** 
\brief While using satellite channel, sets the current attributes of the tuner.
CNcomment:\brief 设置卫星TUNER附加属性。CNend
\attention \n
N/A
\param[in] u32tunerId   tuner port ID. The port ID can be 0-2.      CNcomment:卫星TUNER端口号，取值为0-2 CNend
\param[in] pstSatTunerAttr tuner attributes                         CNcomment:卫星TUNER的附加属性。CNend
\retval ::HI_SUCCESS Success                                        CNcomment:成功 CNend
\retval ::HI_FAILURE  Calling this API fails.                       CNcomment:API系统调用失败 CNend
\retval ::HI_ERR_TUNER_NOT_OPEN  The TUNER module is not opened. CNcomment: 模块没有打开CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_INVALID_POINT The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_INVALID_PARA The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_FAILED_SETSATATTR Tuner TUNER_SETSATATTR_CMD error. CNcomment:设置属性失败 CNend
\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_SetSatAttr(HI_U32	u32tunerId , const HI_UNF_TUNER_SAT_ATTR_S *pstSatTunerAttr);

/** 
\brief Obtains the attributes of the tuner.
CNcomment:\brief 获取TUNER的属性。CNend
\attention \n
N/A
This API is available only after the tuner is initialized.
CNcomment:此接口在TUNER设备初始化后才能使用。CNend

\param[in] u32tunerId  tuner port ID. The port ID can be 0-2.                CNcomment:TUNER端口号，取值为0-2 CNend
\param[out] pstTunerAttr   The current attributes of the tuner are returned. CNcomment:返回当前TUNER的属性。CNend
\retval ::HI_SUCCESS Success                                                 CNcomment:成功 CNend
\retval ::HI_FAILURE  Calling this API fails.                                CNcomment: API系统调用失败 CNend
\retval ::HI_ERR_TUNER_NOT_INIT  The TUNER module is not initialized. CNcomment: 模块没有初始化CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_INVALID_POINT The input parameter is invalid. CNcomment:输入 参数非法 CNend
\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_GetAttr(HI_U32  u32tunerId , HI_UNF_TUNER_ATTR_S *pstTunerAttr );


/** 
\brief Starts the tuner.
CNcomment:\brief 打开TUNER设备。CNend

\attention \n
The error code HI_SUCCESS is returned if this API is called repeatedly.
CNcomment:支持重复打开TUNER设备。此时，直接返回成功。CNend

\param[in] u32tunerId   tuner port ID. The port ID can be 0-2.      CNcomment:TUNER端口号，取值为0-2 CNend
\retval ::HI_SUCCESS Success                                        CNcomment:成功 CNend
\retval ::HI_FAILURE Calling this API fails.                        CNcomment:API系统调用失败 CNend
\retval ::HI_ERR_TUNER_NOT_INIT  The TUNER module is not initialized. CNcomment: 模块没有初始化CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_FAILED_INIT  Opening The TUNER module  fails. CNcomment:打开设备失败CNend
\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_Open (HI_U32    u32tunerId);


/** 
\brief Stops the tuner.
CNcomment:\brief 关闭TUNER设备。CNend

\attention \n
The error code HI_SUCCESS is returned if this API is called repeatedly.
CNcomment:支持重复关闭TUNER设备。此时，直接返回成功。CNend

\param[in] u32tunerId  tuner port ID. The port ID can be 0-2.     CNcomment:TUNER端口号，取值为0-2 CNend
\retval ::HI_SUCCESS Success                                      CNcomment:成功 CNend
\retval ::HI_FAILURE  Calling this API fails.                     CNcomment:API系统调用失败 CNend
\retval ::HI_ERR_TUNER_NOT_INIT  The TUNER module is not initialized. CNcomment: 模块没有初始化CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:输入 参数非法 CNend

\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_Close(HI_U32    u32tunerId);


/** 
\brief Locks the frequency of the tuner.
CNcomment:\brief TUNER锁频。CNend
\attention \n
N/A
\param[in] u32tunerId  tuner port ID. The port ID can be 0-2.             CNcomment:TUNER端口号，取值为0-2 CNend
\param[in] pstConnectPara   Information about the frequency of the tuner  CNcomment:TUNER的频点信息。CNend
\param[in] u32TimeOut  Wait timeout (in ms) when the frequency of the tuner \n
is locked. The value 0 indicates no wait, and any other value indicates the \n
maximum wait period.  
                                                                          CNcomment:TUNER的锁定等待超时时间，0为不等待，其他为最长等待时间，单位ms。CNend
\retval ::HI_SUCCESS Success                                              CNcomment:成功 CNend
\retval ::HI_FAILURE  Calling this API fails.                             CNcomment:API系统调用失败 CNend
\retval ::HI_ERR_TUNER_NOT_OPEN  The TUNER module is not opened. CNcomment: 模块没有打开CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_INVALID_POINT The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_INVALID_PARA The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_FAILED_CONNECT The signal is locked unsuccessfully . CNcomment:锁定频点失败CNend

\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_Connect(HI_U32  u32tunerId , const HI_UNF_TUNER_CONNECT_PARA_S  *pstConnectPara,HI_U32 u32TimeOut);


/**
\brief  set TS out.CNcomment:设置TS输出 CNend
\attention \n
\Please call this API to set TS out. \n
\There are 12 signals in ts interface, ts_dat[0..7], ts_sync, ts_vld, ts_err, ts_clk.\n 
\There're 12 pins in HI3136 or HI3130v200 chip, and each pin can be set to output ts_dat[0..7], ts_sync,\n
\ts_vld or ts_err, but ts_clk is binded to one pin, cannot be setted.\n
CNcomment:ts接口共有12根信号线，分别是ts_dat[0..7], ts_sync, ts_vld, ts_err, ts_clk，HI3136/HI3130V200 芯片上有12个ts管脚，每个管脚可以\n
配置成除ts_clk之外的任意一个管脚，ts_clk是固定的，不支持配置。CNend

\param[in] u32tunerId  tuner port ID. The port ID can be 0-2.             CNcomment:TUNER端口号，取值为0-2 CNend
\param[in] pstTSOUT   pointer of ts pin.                                  CNcomment:指针类型，ts管脚定义，请参见::HI_UNF_TUNER_TSOUT_SET_S CNend
\retval ::HI_SUCCESS                                                      CNcomment: success.成功 CNend
\retval ::HI_FAILURE  Calling this API fails.                             CNcomment:API系统调用失败 CNend
\retval ::HI_ERR_TUNER_NOT_OPEN  The TUNER module is not opened. CNcomment: 模块没有打开CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_INVALID_PARA The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_FAILED_SETTSOUT Setting tsout fails. CNcomment:设置ts out 失败 CNend
\see \n
none.CNcomment:无 CNend
*/
HI_S32 HI_UNF_TUNER_SetTSOUT(HI_U32 u32TunerId, HI_UNF_TUNER_TSOUT_SET_S *pstTSOUT);

/** 
\brief Obtains the frequency locking status and parameters of the tuner.
CNcomment:\brief 获取TUNER锁频状态和锁频参数。CNend
\attention \n
N/A
\param[in] u32tunerId tuner port ID. The port ID can be 0-2.             CNcomment:TUNER端口号，取值为0-2 CNend
\param[in] u32TimeOut frequency locking timeout, unit: ms, frequency locking time is related to strength of signal CNcomment:锁频的超时时间，单位是毫秒，锁频时间和信号强度有关系。CNend
               if you want to get the state of locking, the u32TimeOut should be set 100ms at least;
               if the u32TimeOut is 0, just config register without locking state, return HI_SUCCESS
               CNcomment: 如果想得到锁定状态，锁频超时最小设置为100ms；
                          如果超时时间为0，只配置寄存器，不判定锁定状态，返回HI_SUCCESS。CNend
\param[out] pstTunerStatus: The current frequency locking status and parameters of the tuner are returned. Note: This parameter is valid only when HI_SUCCESS is returned.  CNcomment: 返回当前TUNER的锁频状态和锁频参数。 注意：此参数只在函数返回成功的情况下才有意义。CNend
\retval ::HI_SUCCESS Success                    CNcomment:成功 CNend
\retval ::HI_FAILURE  Calling this API fails.  CNcomment:API系统调用失败 CNend
\retval ::HI_ERR_TUNER_NOT_OPEN  The TUNER module is not opened. CNcomment: 模块没有打开CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_INVALID_POINT The input parameter is invalid. CNcomment:输入 参数非法 CNend

\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_GetStatus(HI_U32    u32tunerId , HI_UNF_TUNER_STATUS_S  *pstTunerStatus);


/** 
\brief Obtains the current bit error rate (BER) of the tuner. The BER is expressed by using the scientific notation.
CNcomment:\brief 获取当前TUNER 的误码率，用科学计数法表示。CNend
\attention \n
N/A
\param[in] u32tunerId tuner port ID. The port ID can be 0-2.             CNcomment:TUNER端口号，取值为0-2 CNend
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
CNcomment:\param[out] pu32BER  指向当前TUNER 误码率的指针。该指针指向一个包含三个元素的数组，三个元素含义如下:
                              pu32BER[0]:误码率底数的整数部分
                              pu32BER[1]:误码率底数的小数部分乘以1000
                              pu32BER[2]:误码率指数部分取绝对值
                              例如:误码率为2.156E-7，那么三个元素的取值分别为 
                              2、156和7 
                              这三个元素只在函数返回成功的情况下才有意义，
                              否则应用层可根据实际情况自行设置一个较大值(比如0.5，即三个元素分别为5、0、1)。CNend
\retval ::HI_SUCCESS Success                  CNcomment:成功 CNend
\retval ::HI_FAILURE Calling this API fails.  CNcomment:API系统调用失败 CNend
\retval ::HI_ERR_TUNER_NOT_OPEN  The TUNER module is not opened. CNcomment: 模块没有打开CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_INVALID_POINT The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_NOT_CONNECT The signal is droped. CNcomment:信号未锁定 CNend

\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_GetBER(HI_U32   u32tunerId , HI_U32 *pu32BER);


/** 
\brief Obtains the current signal-to-noise ratio (SNR) of the tuner.
CNcomment:\brief 获取当前TUNER 的信噪比。CNend
\attention \n
N/A
\param[in] u32tunerId  tuner port ID. The port ID can be 0-2.         CNcomment:TUNER端口号，取值为0-2 CNend
\param[out] pu32SNR  Pointer to the current SNR of the tuner
                    This parameter is valid only when HI_SUCCESS is returned.
                   Otherwise, the application layer sets the parameter to a small value.
CNcomment:\param[out] pu32SNR  指向当前TUNER信噪比的指针。
                              此参数只在函数返回成功的情况下才有意义，
                              否则，应用层可将此值设置为最小值。CNend
\retval ::HI_SUCCESS Success                   CNcomment:成功 CNend
\retval ::HI_FAILURE  Calling this API fails.  CNcomment:API系统调用失败 CNend
\retval ::HI_ERR_TUNER_NOT_OPEN  The TUNER module is not opened. CNcomment: 模块没有打开CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_INVALID_POINT The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_NOT_CONNECT The signal is droped. CNcomment:信号未锁定 CNend
\retval ::HI_ERR_TUNER_FAILED_GETSTATUS Getting status fails. CNcomment:获取信道锁定状态失败 CNend

\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_GetSNR(HI_U32   u32tunerId , HI_U32 *pu32SNR );             /* range : 0-255  */


/** 
\brief Obtains the current signal strength of the tuner.
CNcomment:\brief 获取当前TUNER的信号强度。CNend
\attention \n
N/A
\param[in] u32tunerId  tuner port ID. The port ID can be 0-2.         CNcomment:TUNER端口号，取值为0-2 CNend
\param[out] pu32SignalStrength  Pointer to the current signal strength of the tuner
                            For cable signal, the value of *pu32SignalStrength ranges from 20~120.
                            For satellite signal, the value is 15~130, unit in dBuv.
                  This parameter is valid only when HI_SUCCESS is returned.
                  Otherwise, the application layer sets the parameter to a small value.
CNcomment:\param[out] pu32SignalStrength  指向当前TUNER信号强度的指针。
                            对于Cable信号，*pu32SignalStrength的取值范围为20~120
                            对于Satellite信号，*pu32SignalStrength的取值范围为15~130，单位为dBuv
                            此参数只在函数返回成功的情况下才有意义，
                            否则，应用层可将此值设置为最小值。CNend
\retval ::HI_SUCCESS Success                  CNcomment:成功 CNend
\retval ::HI_FAILURE  Calling this API fails. CNcomment:API系统调用失败 CNend
\retval ::HI_ERR_TUNER_NOT_OPEN  The TUNER module is not opened. CNcomment: 模块没有打开CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_INVALID_POINT The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_NOT_CONNECT The signal is droped. CNcomment:信号未锁定 CNend
\retval ::HI_ERR_TUNER_FAILED_GETSTATUS Getting status fails. CNcomment:获取信道锁定状态失败 CNend
\retval ::HI_ERR_TUNER_FAILED_GETSIGNALSTRENGTH Calling this CMD fails. CNcomment:ioctl系统调用失败 CNend

\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_GetSignalStrength(HI_U32   u32tunerId , HI_U32 *pu32SignalStrength );

/**
\brief Obtains the current signal quality of the tuner, returns a percentage value. 
CNcomment:\brief 获取当前TUNER的信号质量，返回百分比( 此接口不支持DVB_C 和J83B制式)。CNend
\attention \n
N/A
\param[in] u32TunerId  Tuner port ID. The port ID can be 0-2.         CNcomment:TUNER端口号，取值为0-2 CNend
\param[out] pu32SignalQuality   Output pointer.                       CNcomment:指向信号质量的指针。CNend
\retval ::HI_SUCCESS   Success                                        CNcomment:成功 CNend
\retval ::HI_FAILURE   Calling this API fails.                        CNcomment:API系统调用失败 CNend
\retval ::HI_ERR_TUNER_NOT_OPEN  The TUNER module is not opened. CNcomment: 模块没有打开CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_INVALID_POINT The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_NOT_CONNECT The signal is droped. CNcomment:信号未锁定 CNend
\retval ::HI_ERR_TUNER_FAILED_GETSTATUS Getting status fails. CNcomment:获取信道锁定状态失败 CNend
\retval ::HI_ERR_TUNER_FAILED_GETSIGNALQUALITY Calling this CMD fails. CNcomment:ioctl系统调用失败 CNend

\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_GetSignalQuality(HI_U32 u32TunerId, HI_U32 *pu32SignalQuality);

/**
\brief Obtains the actual frequency and symbol rate of the current tuner for cable and satellite
signal, for terrestrial signal, symbol rate means nothing, ignore it. 
CNcomment:\brief 获取当前TUNER的实际频率和符号率。CNend
\attention \n
N/A
\param[in] u32TunerId  Tuner port ID. The port ID can be 0-2.         CNcomment:TUNER端口号，取值为0-2 CNend
\param[out] pu32Freq   Pointer to the current frequency of the tuner. CNcomment:指向当前TUNER频点的指针。CNend
\param[out] pu32Symb   Points to the current symbol rate.             CNcomment:指向当前符号率 CNend
\retval ::HI_SUCCESS Success                                          CNcomment:成功 CNend
\retval ::HI_FAILURE  Calling this API fails.                         CNcomment:API系统调用失败 CNend
\retval ::HI_ERR_TUNER_NOT_OPEN  The TUNER module is not opened. CNcomment: 模块没有打开CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_INVALID_POINT The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_NOT_CONNECT The signal is droped. CNcomment:信号未锁定 CNend
\retval ::HI_ERR_TUNER_FAILED_GETSTATUS Getting status fails. CNcomment:获取信道锁定状态失败 CNend
\retval ::HI_ERR_TUNER_FAILED_GETSIGNALSTRENGTH  Calling this CMD fails. CNcomment:ioctl系统调用失败 CNend

\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_GetRealFreqSymb( HI_U32 u32TunerId, HI_U32 *pu32Freq, HI_U32 *pu32Symb );

/**
\brief Obtains current signal information of the TUNER, used in satellite and terrestrial, not necessary for cable. 
CNcomment:\brief 获取当前TUNER的信号信息。CNend
\attention \n
N/A
\param[in] u32TunerId  Tuner port ID. The port ID can be 0-2.         CNcomment:TUNER端口号，取值为0-2 CNend
\param[out] pstSignalInfo Pointer to a signal info structure.         CNcomment:指向信号信息结构体的指针。CNend
\retval ::HI_SUCCESS   Success                                        CNcomment:成功 CNend
\retval ::HI_FAILURE   Calling this API fails.                        CNcomment:API系统调用失败 CNend
\retval ::HI_ERR_TUNER_NOT_OPEN  The TUNER module is not opened. CNcomment: 模块没有打开CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_INVALID_POINT The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_NOT_CONNECT The signal is droped. CNcomment:信号未锁定 CNend
\retval ::HI_ERR_TUNER_FAILED_GETSTATUS Getting status fails. CNcomment:获取信道锁定状态失败 CNend
\retval ::HI_ERR_TUNER_FAILED_GETSIGNALINFO  Calling this CMD fails. CNcomment:ioctl系统调用失败 CNend

\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_GetSignalInfo(HI_U32 u32TunerId, HI_UNF_TUNER_SIGNALINFO_S *pstSignalInfo);

/**
\brief Sets the LNB parameter. 
CNcomment:\brief 设置LNB参数。CNend
\attention \n
N/A
\param[in] u32TunerId  Tuner port ID. The port ID can be 0-2.         CNcomment:TUNER端口号，取值为0-2 CNend
\param[in] pstLNB      Pointer to a LNB parameter structure.          CNcomment:指向LNB参数结构体的指针。CNend
\retval ::HI_SUCCESS   Success                                        CNcomment:成功 CNend
\retval ::HI_FAILURE   Calling this API fails.                        CNcomment:API系统调用失败 CNend
\retval ::HI_ERR_TUNER_NOT_OPEN  The TUNER module is not opened. CNcomment: 模块没有打开CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_INVALID_POINT The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_INVALID_PARA The input parameter is invalid. CNcomment:输入 参数非法 CNend
\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_SetLNBConfig(HI_U32 u32TunerId, const HI_UNF_TUNER_FE_LNB_CONFIG_S *pstLNB);

/**
\brief Sets the LNB power. 
CNcomment:\brief 设置LNB供电。CNend
\attention \n
N/A
\param[in] u32TunerId  Tuner port ID. The port ID can be 0-2.         CNcomment:TUNER端口号，取值为0-2 CNend
\param[in] enLNBPower  The enumeration of the LNB power type.         CNcomment:LNB供电方式枚举值。CNend
\retval ::HI_SUCCESS   Success                                        CNcomment:成功 CNend
\retval ::HI_FAILURE   Calling this API fails.                        CNcomment:API系统调用失败 CNend
\retval ::HI_ERR_TUNER_NOT_OPEN  The TUNER module is not opened. CNcomment: 模块没有打开CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_INVALID_POINT The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_INVALID_PARA The input parameter is invalid. CNcomment:输入 参数非法 CNend

\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_SetLNBPower(HI_U32 u32TunerId, HI_UNF_TUNER_FE_LNB_POWER_E enLNBPower);

/**
\brief Sets PLP ID, only used in DVB-T2. 
CNcomment:\brief 设置物理层管道ID。CNend
\attention \n
N/A
\param[in] u32TunerId  Tuner port ID. The port ID can be 0-2.         CNcomment:TUNER端口号，取值为0-2 CNend
\param[in] u8PLPID  The PLP ID.                                       CNcomment:物理层管道ID。CNend
\retval ::HI_SUCCESS   Success                                        CNcomment:成功 CNend
\retval ::HI_FAILURE   Calling this API fails.                        CNcomment:API系统调用失败 CNend
\retval ::HI_ERR_TUNER_NOT_OPEN  The TUNER module is not opened. CNcomment: 模块没有打开CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_FAILED_SETPLPID  Calling this CMD fails. CNcomment:ioctl系统调用失败 CNend

\see \n
N/A
*/

HI_S32 HI_UNF_TUNER_SetPLPID(HI_U32 u32TunerId, HI_U8 u8PLPID);

/**
\brief Gets PLP number, only used in DVB-T2. 
CNcomment:\brief 获取物理层管道数量。CNend
\attention \n
N/A
\param[in] u32TunerId  Tuner port ID. The port ID can be 0-2.         CNcomment:TUNER端口号，取值为0-2 CNend
\param[out] pu8PLPNum  The PLP number.                                CNcomment:物理层管道数量。CNend
\retval ::HI_SUCCESS   Success                                        CNcomment:成功 CNend
\retval ::HI_FAILURE   Calling this API fails.                        CNcomment:API系统调用失败 CNend
\retval ::HI_ERR_TUNER_NOT_OPEN  The TUNER module is not opened. CNcomment: 模块没有打开CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_INVALID_POINT The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_FAILED_GETPLPNUM  Calling this CMD fails. CNcomment:ioctl系统调用失败 CNend

\see \n
N/A
*/

HI_S32 HI_UNF_TUNER_GetPLPNum(HI_U32 u32TunerId, HI_U8 *pu8PLPNum);


/**
\brief Gets current PLP type, only used in DVB-T2. 
CNcomment:\brief 获取当前物理层管道类型。CNend
\attention \n
N/A
\param[in] u32TunerId  Tuner port ID. The port ID can be 0-2.         CNcomment:TUNER端口号，取值为0-2 CNend
\param[out] penPLPType  The PLP type.                                 CNcomment:物理层管道类型。CNend
\retval ::HI_SUCCESS   Success                                        CNcomment:成功 CNend
\retval ::HI_FAILURE   Calling this API fails.                        CNcomment:API系统调用失败 CNend
\retval ::HI_ERR_TUNER_NOT_OPEN  The TUNER module is not opened. CNcomment: 模块没有打开CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_INVALID_POINT The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_FAILED_GETPLPTYPE  Calling this CMD fails. CNcomment:ioctl系统调用失败 CNend

\see \n
N/A
*/

HI_S32 HI_UNF_TUNER_GetCurrentPLPType(HI_U32 u32TunerId, HI_UNF_TUNER_T2_PLP_TYPE_E *penPLPType);


/**
\brief Starts blind scan. 
CNcomment:\brief 开始盲扫。CNend
\attention \n
N/A
\param[in] u32TunerId  Tuner port ID. The port ID can be 0-2.         CNcomment:TUNER端口号，取值为0-2 CNend
\param[in] pstPara     The pointer to the blind scan parameter.       CNcomment:指向盲扫参数的指针。CNend
\retval ::HI_SUCCESS   Success                                        CNcomment:成功 CNend
\retval ::HI_FAILURE   Calling this API fails.                        CNcomment:API系统调用失败 CNend
\retval ::HI_ERR_TUNER_NOT_OPEN  The TUNER module is not opened. CNcomment: 模块没有打开CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_INVALID_POINT The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_INVALID_PARA The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_FAILED_BLINDSCAN Blind scan busy. CNcomment:盲扫正在应用 CNend

\see Please refer to definition of HI_UNF_TUNER_BLINDSCAN_PARA_S.
N/A
*/
HI_S32 HI_UNF_TUNER_BlindScanStart(HI_U32 u32TunerId, const HI_UNF_TUNER_BLINDSCAN_PARA_S *pstPara);

/**
\brief Stops blind scan. 
CNcomment:\brief 停止盲扫。CNend
\attention \n
N/A
\param[in] u32TunerId  Tuner port ID. The port ID can be 0-2.         CNcomment:TUNER端口号，取值为0-2 CNend
\retval ::HI_SUCCESS   Success                                        CNcomment:成功 CNend
\retval ::HI_FAILURE   Calling this API fails.                        CNcomment:API系统调用失败 CNend
\retval ::HI_ERR_TUNER_NOT_OPEN  The TUNER module is not opened. CNcomment: 模块没有打开CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:输入 参数非法 CNend

\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_BlindScanStop(HI_U32 u32TunerId);

/**
\brief TUNER standby. 
CNcomment:\brief TUNER待机(  此接口不支持DVB_C 和J83B制式，此两种制式待机请参见pmoc 待机接口)  CNend
\attention \n
N/A
\param[in] u32TunerId  Tuner port ID. The port ID can be 0-2.         CNcomment:TUNER端口号，取值为0-2 CNend
\retval ::HI_SUCCESS   Success                                        CNcomment:成功 CNend
\retval ::HI_FAILURE   Calling this API fails.                        CNcomment:API系统调用失败 CNend
\retval ::HI_ERR_TUNER_NOT_OPEN  The TUNER module is not opened. CNcomment: 模块没有打开CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_FAILED_STANDBY  Calling this CMD fails. CNcomment:ioctl系统调用失败 CNend

\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_Standby(HI_U32 u32TunerId);

/**
\brief Wakes up TUNER. 
CNcomment:\brief TUNER唤醒(  此接口不支持DVB_C 和J83B制式，此两种制式唤醒请参见pmoc 唤醒唤醒接口) CNend
\attention \n
N/A
\param[in] u32TunerId  Tuner port ID. The port ID can be 0-2.         CNcomment:TUNER端口号，取值为0-2 CNend
\retval ::HI_SUCCESS   Success                                        CNcomment:成功 CNend
\retval ::HI_FAILURE   Calling this API fails.                        CNcomment:API系统调用失败 CNend
\retval ::HI_ERR_TUNER_NOT_OPEN  The TUNER module is not opened. CNcomment: 模块没有打开CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_FAILED_WAKEUP  Calling this CMD fails. CNcomment:ioctl系统调用失败 CNend
\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_WakeUp( HI_U32 u32TunerId);

/**
\brief Sends and receives DiSEqC message, only the devices supporting DiSEqC 2.x support receive message. 
CNcomment:\brief 发送接收DiSEqC消息，仅支持DiSEqC 2.x的设备支持接收消息。CNend
\attention \n
N/A
\param[in] u32TunerId  Tuner port ID. The port ID can be 0-2.         CNcomment:TUNER端口号, 取值范围0-2 CNend
\param[in] pstSendMsg  The pointer to a sending message structure.    CNcomment:指向发送消息结构体的指针。CNend
\param[out] pstRecvMsg The pointer to a receiving message structure.If your device is DiSEqC 1.x, you can pass NULL here.
CNcomment:\param[out] pstRecvMsg 指向接收消息结构体的指针。如果是DiSEqC 1.x设备，这里可以传NULL。CNend
\retval ::HI_SUCCESS   Success                                        CNcomment:成功 CNend
\retval ::HI_FAILURE   Calling this API fails.                        CNcomment:API系统调用失败 CNend
\retval ::HI_ERR_TUNER_NOT_OPEN  The TUNER module is not opened. CNcomment: 模块没有打开CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_INVALID_POINT The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_INVALID_PARA The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_FAILED_DISEQC Send DiSEqC message fail. CNcomment:发送消息失败 CNend
\retval ::HI_ERR_TUNER_FAILED_DISEQC Recv DiSEqC message fail. CNcomment:接收消息失败 CNend

\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_DISEQC_SendRecvMessage(HI_U32 u32TunerId,
                                           const HI_UNF_TUNER_DISEQC_SENDMSG_S * pstSendMsg,
                                           HI_UNF_TUNER_DISEQC_RECVMSG_S * pstRecvMsg);

/**
\brief Sets 0/12V switch. Don't support now. 
CNcomment:\brief 设置0/12V开关状态，暂不支持。CNend
\attention \n
N/A
\param[in] u32TunerId  Tuner port ID. The port ID can be 0-2.         CNcomment:TUNER端口号，取值为0-2 CNend
\param[in] enPort      The enumeration of the switch port.            CNcomment:开关枚举值。CNend
\retval ::HI_SUCCESS   Success                                        CNcomment:成功 CNend
\retval ::HI_FAILURE   Calling this API fails.                        CNcomment:API系统调用失败 CNend
\retval ::HI_ERR_TUNER_NOT_OPEN  The TUNER module is not opened. CNcomment: 模块没有打开CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_INVALID_PARA The input parameter is invalid. CNcomment:输入 参数非法 CNend
\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_Switch012V(HI_U32 u32TunerId, HI_UNF_TUNER_SWITCH_0_12V_E enPort);

/**
\brief Sets 22KHz switch. 
CNcomment:\brief 设置22KHz开关状态。CNend
\attention \n
N/A
\param[in] u32TunerId  Tuner port ID. The port ID can be 0-2.         CNcomment:TUNER端口号，取值为0-2 CNend
\param[in] enPort      The enumeration of the switch port.            CNcomment:开关枚举值。CNend
\retval ::HI_SUCCESS   Success                                        CNcomment:成功 CNend
\retval ::HI_FAILURE   Calling this API fails.                        CNcomment:API系统调用失败 CNend
\retval ::HI_ERR_TUNER_NOT_OPEN  The TUNER module is not opened. CNcomment: 模块没有打开CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_INVALID_PARA The input parameter is invalid. CNcomment:输入 参数非法 CNend
\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_Switch22K(HI_U32 u32TunerId, HI_UNF_TUNER_SWITCH_22K_E enPort);

/**
\brief Sets tone burst switch. 
CNcomment:\brief 设置Tone burst开关状态。CNend
\attention \n
N/A
\param[in] u32TunerId  Tuner port ID. The port ID can be 0-2.         CNcomment:TUNER端口号，取值为0-2 CNend
\param[in] enStatus    The enumeration of the switch port.            CNcomment:开关枚举值。CNend
\retval ::HI_SUCCESS   Success                                        CNcomment:成功 CNend
\retval ::HI_FAILURE   Calling this API fails.                        CNcomment:API系统调用失败 CNend
\retval ::HI_ERR_TUNER_NOT_OPEN  The TUNER module is not opened. CNcomment: 模块没有打开CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_INVALID_PARA The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_FAILED_DISEQC  Calling this CMD fails. CNcomment:ioctl系统调用失败 CNend
\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_SwitchToneBurst(HI_U32 u32TunerId, HI_UNF_TUNER_SWITCH_TONEBURST_E enStatus);

//#ifdef DISEQC_SUPPORT
/**
\brief Sets DiSEqC 1.0/2.0 switch, at most 4 port.
CNcomment:\brief 设置DiSEqC 1.0/2.0开关，至多4口。CNend
\attention \n
N/A
\param[in] u32TunerId  Tuner port ID. The port ID can be 0-2.         CNcomment:TUNER端口号，取值为0-2 CNend
\param[in] pstPara     The pointer to a switch parameter structure.   CNcomment:指向开关参数的指针。CNend
\retval ::HI_SUCCESS   Success                                        CNcomment:成功 CNend
\retval ::HI_FAILURE   Calling this API fails.                        CNcomment:API系统调用失败 CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_INVALID_POINT The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_INVALID_PARA The input parameter is invalid. CNcomment:输入 参数非法 CNend
\see Please refer to definition of HI_UNF_TUNER_DISEQC_SWITCH4PORT_S.
N/A
*/
HI_S32 HI_UNF_TUNER_DISEQC_Switch4Port(HI_U32 u32TunerId, const HI_UNF_TUNER_DISEQC_SWITCH4PORT_S* pstPara);

/**
\brief Sets DiSEqC 1.1/2.1 switch, supports 8in1, 16in1 switches. 
CNcomment:\brief 设置DiSEqC 1.1/2.1开关，支持8口，16口开关。CNend
\attention \n
N/A
\param[in] u32TunerId  Tuner port ID. The port ID can be 0-2.         CNcomment:TUNER端口号，取值为0-2 CNend
\param[in] pstPara     The pointer to a switch parameter structure.   CNcomment:指向开关参数的指针。CNend
\retval ::HI_SUCCESS   Success                                        CNcomment:成功 CNend
\retval ::HI_FAILURE   Calling this API fails.                        CNcomment:API系统调用失败 CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_INVALID_POINT The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_INVALID_PARA The input parameter is invalid. CNcomment:输入 参数非法 CNend
\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_DISEQC_Switch16Port(HI_U32 u32TunerId, const HI_UNF_TUNER_DISEQC_SWITCH16PORT_S* pstPara);

/**
\brief Lets the DiSEqC motor store current position. 
CNcomment:\brief DiSEqC马达存储当前位置。CNend
\attention \n
N/A
\param[in] u32TunerId  Tuner port ID. The port ID can be 0-2.         CNcomment:TUNER端口号，取值为0-2 CNend
\param[in] pstPara     The pointer to a position parameter structure. CNcomment:指向位置参数的指针。CNend
\retval ::HI_SUCCESS   Success                                        CNcomment:成功 CNend
\retval ::HI_FAILURE   Calling this API fails.                        CNcomment:API系统调用失败 CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_INVALID_POINT The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_INVALID_PARA The input parameter is invalid. CNcomment:输入 参数非法 CNend
\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_DISEQC_StorePos(HI_U32 u32TunerId, const HI_UNF_TUNER_DISEQC_POSITION_S *pstPara);

/**
\brief Lets the DiSEqC motor move to stored position. 
CNcomment:\brief DiSEqC马达转动至存储位置。CNend
\attention \n
N/A
\param[in] u32TunerId  Tuner port ID. The port ID can be 0-2.         CNcomment:TUNER端口号，取值为0-2 CNend
\param[in] pstPara     The pointer to a position parameter structure. CNcomment:指向位置参数的指针。CNend
\retval ::HI_SUCCESS   Success                                        CNcomment:成功 CNend
\retval ::HI_FAILURE   Calling this API fails.                        CNcomment:API系统调用失败 CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_INVALID_POINT The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_INVALID_PARA The input parameter is invalid. CNcomment:输入 参数非法 CNend
\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_DISEQC_GotoPos(HI_U32 u32TunerId, const HI_UNF_TUNER_DISEQC_POSITION_S *pstPara);

/**
\brief Enables or disables the DiSEqC motor's limit setting. 
CNcomment:\brief 打开、关闭DiSEqC马达权限设置。CNend
\attention \n
N/A
\param[in] u32TunerId  Tuner port ID. The port ID can be 0-2.         CNcomment:TUNER端口号，取值为0-2 CNend
\param[in] pstPara     The pointer to a limit parameter structure.    CNcomment:指向limit参数的指针。CNend
\retval ::HI_SUCCESS   Success                                        CNcomment:成功 CNend
\retval ::HI_FAILURE   Calling this API fails.                        CNcomment:API系统调用失败 CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_INVALID_POINT The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_INVALID_PARA The input parameter is invalid. CNcomment:输入 参数非法 CNend
\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_DISEQC_SetLimit(HI_U32 u32TunerId, const HI_UNF_TUNER_DISEQC_LIMIT_S* pstPara);

/**
\brief Drives DiSEqC motor. 
CNcomment:\brief 驱动DiSEqC马达移动。CNend
\attention \n
N/A
\param[in] u32TunerId  Tuner port ID. The port ID can be 0-2.         CNcomment:TUNER端口号，取值为0-2 CNend
\param[in] pstPara     The pointer to a movement parameter structure. CNcomment:指向移动参数的指针。CNend
\retval ::HI_SUCCESS   Success                                        CNcomment:成功 CNend
\retval ::HI_FAILURE   Calling this API fails.                        CNcomment:API系统调用失败 CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_INVALID_POINT The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_INVALID_PARA The input parameter is invalid. CNcomment:输入 参数非法 CNend
\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_DISEQC_Move(HI_U32 u32TunerId, const HI_UNF_TUNER_DISEQC_MOVE_S* pstPara);

/**
\brief Halts DiSEqC motor. 
CNcomment:\brief 停止DiSEqC马达移动。CNend
\attention \n
N/A
\param[in] u32TunerId  Tuner port ID. The port ID can be 0-2.         CNcomment:TUNER端口号，取值为0-2 CNend
\param[in] enLevel     The command level of the DiSEqC motor.         CNcomment:马达支持的命令等级。CNend
\retval ::HI_SUCCESS   Success                                        CNcomment:成功 CNend
\retval ::HI_FAILURE   Calling this API fails.                        CNcomment:API系统调用失败 CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_INVALID_PARA The input parameter is invalid. CNcomment:输入 参数非法 CNend
\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_DISEQC_Stop(HI_U32 u32TunerId, const HI_UNF_TUNER_DISEQC_LEVEL_E enLevel);

/**
\brief Lets the DiSEqC motor recalculate its stored positions. 
CNcomment:\brief 重新计算马达存储位置。CNend
\attention \n
N/A
\param[in] u32TunerId  Tuner port ID. The port ID can be 0-2.         CNcomment:TUNER端口号，取值为0-2 CNend
\param[in] pstPara     The pointer to a recaulculate parameter structure. 
                                                                      CNcomment:指向重计算参数的指针。CNend
\retval ::HI_SUCCESS   Success                                        CNcomment:成功 CNend
\retval ::HI_FAILURE   Calling this API fails.                        CNcomment:API系统调用失败 CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_INVALID_POINT The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_INVALID_PARA The input parameter is invalid. CNcomment:输入 参数非法 CNend
\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_DISEQC_Recalculate(HI_U32 u32TunerId, const HI_UNF_TUNER_DISEQC_RECALCULATE_S* pstPara);

/**
\brief Calculates the angular, basing on site-longitude, site-latitude and satellite-longitude. 
CNcomment:\brief USALS根据当地经纬度、卫星经度计算卫星角度。CNend
\attention \n
N/A
\param[in] u32TunerId  Tuner port ID. The port ID can be 0-2.         CNcomment:TUNER端口号，取值为0-2 CNend
\param[in/out] pstPara The pointer to a USALS parameter structure.    CNcomment:指向USALS计算参数的指针。CNend
\retval ::HI_SUCCESS   Success                                        CNcomment:成功 CNend
\retval ::HI_FAILURE   Calling this API fails.                        CNcomment:API系统调用失败 CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_INVALID_POINT The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_INVALID_PARA The input parameter is invalid. CNcomment:输入 参数非法 CNend
\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_DISEQC_CalcAngular(HI_U32 u32TunerId, HI_UNF_TUNER_DISEQC_USALS_PARA_S* pstPara);

/**
\brief Gotos the indicated angular. 
CNcomment:\brief USALS设备转至某角度。CNend
\attention \n
N/A
\param[in] u32TunerId  Tuner port ID. The port ID can be 0-2.         CNcomment:TUNER端口号，取值为0-2 CNend
\param[in] pstPara     The pointer to a USALS angular structure.      CNcomment:指向USALS角度参数的指针。CNend
\retval ::HI_SUCCESS   Success                                        CNcomment:成功 CNend
\retval ::HI_FAILURE   Calling this API fails.                        CNcomment:API系统调用失败 CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_INVALID_POINT The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_INVALID_PARA The input parameter is invalid. CNcomment:输入 参数非法 CNend
\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_DISEQC_GotoAngular(HI_U32 u32TunerId, const HI_UNF_TUNER_DISEQC_USALS_ANGULAR_S* pstPara);

/**
\brief Resets DiSEqC device. 
CNcomment:\brief 复位DiSEqC设备。CNend
\attention \n
N/A
\param[in] u32TunerId  Tuner port ID. The port ID can be 0-2.         CNcomment:TUNER端口号，取值为0-2 CNend
\param[in] enLevel     The command level of the DiSEqC device.        CNcomment:设备支持的命令等级。CNend
\retval ::HI_SUCCESS   Success                                        CNcomment:成功 CNend
\retval ::HI_FAILURE   Calling this API fails.                        CNcomment:API系统调用失败 CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_INVALID_PARA The input parameter is invalid. CNcomment:输入 参数非法 CNend
\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_DISEQC_Reset(HI_U32 u32TunerId, HI_UNF_TUNER_DISEQC_LEVEL_E enLevel);

/**
\brief DiSEqC device standby. 
CNcomment:\brief DiSEqC设备待机。CNend
\attention \n
N/A
\param[in] u32TunerId  Tuner port ID. The port ID can be 0-2.         CNcomment:TUNER端口号，取值为0-2 CNend
\param[in] enLevel     The command level of the DiSEqC device.        CNcomment:设备支持的命令等级。CNend
\retval ::HI_SUCCESS   Success                                        CNcomment:成功 CNend
\retval ::HI_FAILURE   Calling this API fails.                        CNcomment:API系统调用失败 CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_INVALID_PARA The input parameter is invalid. CNcomment:输入 参数非法 CNend
\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_DISEQC_Standby(HI_U32 u32TunerId, HI_UNF_TUNER_DISEQC_LEVEL_E enLevel);

/**
\brief Wakes up DiSEqC device. 
CNcomment:\brief 唤醒DiSEqC设备。CNend
\attention \n
N/A
\param[in] u32TunerId  Tuner port ID. The port ID can be 0-2.         CNcomment:TUNER端口号，取值为0-2 CNend
\param[in] enLevel     The command level of the DiSEqC device.        CNcomment:设备支持的命令等级。CNend
\retval ::HI_SUCCESS   Success                                        CNcomment:成功 CNend
\retval ::HI_FAILURE   Calling this API fails.                        CNcomment:API系统调用失败 CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_INVALID_PARA The input parameter is invalid. CNcomment:输入 参数非法 CNend
\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_DISEQC_WakeUp(HI_U32 u32TunerId, HI_UNF_TUNER_DISEQC_LEVEL_E enLevel);

/**
\brief Acquire data to display constellation.
CNcomment:\brief 获取星座图数据。CNend
\attention \n
N/A
\param[in]  u32TunerId    Tuner port ID. The port ID can be 0-2.             CNcomment:TUNER端口号，取值为0-2 CNend
\param[in]  enDataLen     The sample data length, can be 512, 1024 or 2048.  CNcomment:采集的数据长度, 支持512，1024和2048 3种。CNend
\param[out] pstData       The buffer to store data.                          CNcomment:数据存储区。CNend
\retval ::HI_SUCCESS   Success                                               CNcomment:成功 CNend
\retval ::HI_FAILURE   Calling this API fails.                               CNcomment:API系统调用失败 CNend
\retval ::HI_ERR_TUNER_NOT_OPEN  The TUNER module is not opened. CNcomment: 模块没有打开CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_INVALID_PARA The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_FAILED_SAMPLEDATA Tuner sample data fail. CNcomment:获取数据失败 CNend
\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_GetConstellationData(HI_U32 u32TunerId, HI_UNF_TUNER_SAMPLE_DATALEN_E enDataLen, HI_UNF_TUNER_SAMPLE_DATA_S *pstData);

/**
\brief Acquire data to display spectrum.
CNcomment:\brief 获取频谱图数据。CNend
\attention \n
N/A
\param[in]  u32TunerId    Tuner port ID. The port ID can be 0-2.             CNcomment:TUNER端口号，取值为0-2 CNend
\param[in]  enDataLen     The sample data length, can be 512, 1024 or 2048.  CNcomment:采集的数据长度, 支持512，1024和2048 3种。CNend
\param[out] pu32Data      The buffer to store data.                          CNcomment:数据存储区。CNend
\retval ::HI_SUCCESS   Success                                               CNcomment:成功 CNend
\retval ::HI_FAILURE   Calling this API fails.                               CNcomment:API系统调用失败 CNend
\retval ::HI_ERR_TUNER_NOT_OPEN  The TUNER module is not opened. CNcomment: 模块没有打开CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_INVALID_PARA The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_FAILED_SAMPLEDATA Tuner sample data fail. CNcomment:获取数据失败 CNend
\see \n
N/A
*/
HI_S32 HI_UNF_TUNER_GetSpectrumData(HI_U32 u32TunerId, HI_UNF_TUNER_SAMPLE_DATALEN_E enDataLen, HI_U32 *pu32Data);

/**
\brief Get the upper limit time according to the connect parameter.
CNcomment:\brief 根据锁频参数获取最大锁频时间。CNend
\attention \n
N/A
\param[in]  u32TunerId    Tuner port ID. The port ID can be 0-2.             CNcomment:TUNER端口号，取值为0-2 CNend
\param[in]  pstConnectPara     The pointer to store the connect parameter.   CNcomment:锁频参数指针。CNend
\param[out] pu32TimeOutMs      The upper limit time.                         CNcomment:最大锁台时间。CNend
\retval ::HI_SUCCESS   Success                                               CNcomment:成功 CNend
\retval ::HI_FAILURE   Calling this API fails.                               CNcomment:API系统调用失败 CNend
\retval ::HI_ERR_TUNER_NOT_OPEN  The TUNER module is not opened. CNcomment: 模块没有打开CNend
\retval ::HI_ERR_TUNER_INVALID_PORT  The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_INVALID_POINT The input parameter is invalid. CNcomment:输入 参数非法 CNend
\retval ::HI_ERR_TUNER_INVALID_PARA The input parameter is invalid. CNcomment:输入 参数非法 CNend
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

