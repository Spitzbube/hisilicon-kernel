/******************************************************************************
  Copyright (C), 2004-2050, Hisilicon Tech. Co., Ltd.
******************************************************************************
  File Name     : hi_unf_sound.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2009/1/22
  Last Modified :
  Description   : header file for audio and video output control
  Function List :
  History       :
  1.Date        :
  Author        : z67193
  Modification  : Created file
******************************************************************************/
/** 
 * \file
 * \brief Describes the information about the SOUND (SND) module. CNcomment:提供SOUND的相关信息 CNend
 */
 
#ifndef  __HI_UNF_SND_H__
#define  __HI_UNF_SND_H__

#include "hi_unf_common.h"
//#include "hi_debug.h"

#ifdef __cplusplus
 #if __cplusplus
extern "C" {
 #endif
#endif /* __cplusplus */

/********************************Macro Definition********************************/
/** \addtogroup      SOUND */
/** @{ */  /** <!-- 【SOUND】 */

/**Maximum sound outputport*/
/**CNcomment:最大sound输出端口*/
#define HI_UNF_SND_OUTPUTPORT_MAX 8

/** @} */  /** <!-- ==== Macro Definition end ==== */


/*************************** Structure Definition ****************************/
/** \addtogroup      SOUND */
/** @{ */  /** <!--  【SOUND】 */

/**Defines the ID of the audio output (AO) device.*/
/**CNcomment:定义音频输出设备号*/
typedef enum hiUNF_SND_E
{
    HI_UNF_SND_0,           /**<AO device 0*/ /**<CNcomment:音频输出设备0 */
    HI_UNF_SND_1,           /**<AO device 1*/ /**<CNcomment:音频输出设备1 */
    HI_UNF_SND_2,           /**<AO device 2*/ /**<CNcomment:音频输出设备2 */
    HI_UNF_SND_BUTT    
} HI_UNF_SND_E;

/**Audio volume attribute*/
/**CNcomment:音频音量属性*/
typedef struct hiHI_UNF_SND_GAIN_ATTR_S
{
    HI_BOOL bLinearMode; /**< gain type of volume*/ /**<CNcomment:音量模式 */
    HI_S32  s32Gain; /**<Linear gain(bLinearMode is HI_TRUE) , ranging from 0 to 100*/ /**<CNcomment:线性音量: 0~100 */
                     /**<Decibel gain(bLinearMode is HI_FALSE) , ranging from -70dB to 0dB */ /**<CNcomment: dB音量:-70~0*/
} HI_UNF_SND_GAIN_ATTR_S;

/**Audio Track Type: Master, Slave, Virtual channel*/
/**CNcomment:音频Track类型:主通道 辅通道 虚拟通道*/
typedef enum hiHI_UNF_SND_TRACK_TYPE_E
{
    HI_UNF_SND_TRACK_TYPE_MASTER = 0,     
    HI_UNF_SND_TRACK_TYPE_SLAVE,      
    HI_UNF_SND_TRACK_TYPE_VIRTUAL,      
    HI_UNF_SND_TRACK_TYPE_BUTT 
} HI_UNF_SND_TRACK_TYPE_E;

/**Audio output attribute */
/**CNcomment:音频输出属性*/
typedef struct hiHI_UNF_AUDIOTRACK_ATTR_S
{
    HI_UNF_SND_TRACK_TYPE_E   enTrackType;    /**<Track Type*/ /**<CNcomment:Track类型*/
    HI_U32                  u32FadeinMs;      /**<Fade in time(unit:ms)*/ /**<CNcomment:淡入时间(单位: ms)*/
    HI_U32                  u32FadeoutMs;     /**<Fade out time(unit:ms)*/ /**<CNcomment:淡出时间(单位: ms)*/
    HI_U32                    u32OutputBufSize;  /**<Track output buffer size*/ /**<CNcomment:Track输出缓存大小*/
    HI_U32                  u32BufLevelMs;    /**<Output buffer data size control(ms)*/ /**<CNcomment:输出缓存中数据量控制(ms)*/
} HI_UNF_AUDIOTRACK_ATTR_S;

/**Audio outputport: DAC0,I2S0,SPDIF0,HDMI0,ARC0,CAPTURE0*/
/**CNcomment:音频输出端口:DAC0,I2S0,SPDIF0,HDMI0,ARC0,CAPTURE0*/
typedef enum hiUNF_SND_OUTPUTPORT_E
{
    HI_UNF_SND_OUTPUTPORT_DAC0 = 0,

    HI_UNF_SND_OUTPUTPORT_I2S0,

    HI_UNF_SND_OUTPUTPORT_I2S1,

    HI_UNF_SND_OUTPUTPORT_SPDIF0,

    HI_UNF_SND_OUTPUTPORT_HDMI0,

    HI_UNF_SND_OUTPUTPORT_ARC0,

    HI_UNF_SND_OUTPUTPORT_ALL = 0x7fff,

    HI_UNF_SND_OUTPUTPORT_BUTT,
} HI_UNF_SND_OUTPUTPORT_E;

/**Defines internal Audio DAC outport attribute */
/**CNcomment:定义内置音频DAC输出端口属性*/
typedef struct hiUNF_SND_DAC_ATTR_S
{
    HI_VOID* pPara;
} HI_UNF_SND_DAC_ATTR_S;


/**Defines  Audio I2S outport attribute */
/**CNcomment:定义音频I2S输出端口属性*/
typedef struct hiUNF_SND_I2S_ATTR_S
{
    HI_UNF_I2S_ATTR_S  stAttr;
}  HI_UNF_SND_I2S_ATTR_S;

/**Defines  S/PDIF outport attribute */
/**CNcomment:定义S/PDIF输出端口属性*/
typedef struct hiUNF_SND_SPDIF_ATTR_S
{
    HI_VOID* pPara;
}  HI_UNF_SND_SPDIF_ATTR_S;

/**Defines  HDMI Audio outport attribute */
/**CNcomment:定义HDMI音频输出端口属性*/
typedef struct hiUNF_SND_HDMI_ATTR_S
{
    HI_VOID* pPara;
} HI_UNF_SND_HDMI_ATTR_S;

/**Defines  HDMI ARC outport attribute */
/**CNcomment:定义HDMI音频回传通道端口属性*/
typedef struct hiUNF_SND_ARC_ATTR_S
{
    HI_VOID* pPara;
} HI_UNF_SND_ARC_ATTR_S;


/**Defines  Audio outport attribute */
/**CNcomment:定义音频输出端口属性*/
typedef struct hiUNF_SND_OUTPORT_S
{
    HI_UNF_SND_OUTPUTPORT_E enOutPort;
    union
    {
        HI_UNF_SND_DAC_ATTR_S stDacAttr;
        HI_UNF_SND_I2S_ATTR_S stI2sAttr;
        HI_UNF_SND_SPDIF_ATTR_S stSpdifAttr;
        HI_UNF_SND_HDMI_ATTR_S stHDMIAttr;
        HI_UNF_SND_ARC_ATTR_S stArcAttr;
    } unAttr;
} HI_UNF_SND_OUTPORT_S;

/**Defines  Audio Sound device attribute */
/**CNcomment:定义Sound设备属性*/
typedef struct hiHI_UNF_SND_ATTR_S
{
    HI_U32                    u32PortNum;  /**<Outport number attached sound*/ /**<CNcomment:绑定到Sound设备的输出端口数*/
    HI_UNF_SND_OUTPORT_S stOutport[HI_UNF_SND_OUTPUTPORT_MAX];  /**<Outports attached sound*/ /**<CNcomment:绑定到Sound设备的输出端口*/
    HI_UNF_SAMPLE_RATE_E    enSampleRate;       /**<Sound samplerate*/ /**<CNcomment:Sound设备输出采样率*/
    HI_U32              u32MasterOutputBufSize; /**<Sound master channel buffer size*/ /**<CNcomment:Sound设备主输出通道缓存大小*/
    HI_U32              u32SlaveOutputBufSize;  /**<Sound slave channel buffer size*/ /**<CNcomment:Sound设备从输出通道缓存大小*/
} HI_UNF_SND_ATTR_S;

/**define SND CAST config  struct */
/**CNcomment:定义声音共享配置结构体*/
typedef struct hiUNF_SND_CAST_ATTR_S
{
    HI_U32  u32PcmFrameMaxNum;     /**<Max frame of the PCM data at cast buffer*/ /**<CNcomment: 最大可缓存帧数*/
    HI_U32  u32PcmSamplesPerFrame; /**<Number of sample of the PCM data*/ /**<CNcomment: PCM数据采样点数量*/
} HI_UNF_SND_CAST_ATTR_S;

/**HDMI MODE:AUTO,LPCM,RAW,HBR2LBR*/
/**CNcomment:HDMI 模式:AUTO,LPCM,RAW,HBR2LBR*/
typedef enum hiHI_UNF_SND_HDMI_MODE_E
{
    HI_UNF_SND_HDMI_MODE_LPCM = 0,       /**<HDMI LCPM2.0*/ /**<CNcomment: 立体声pcm*/
    HI_UNF_SND_HDMI_MODE_RAW,            /**<HDMI Pass-through.*/ /**<CNcomment: HDMI透传*/
    HI_UNF_SND_HDMI_MODE_HBR2LBR,        /**<HDMI Pass-through force high-bitrate to low-bitrate.*/ /**<CNcomment: 蓝光次世代音频降规格输出*/
    HI_UNF_SND_HDMI_MODE_AUTO,           /**<automatically match according to the EDID of HDMI */ /**<CNcomment: 根据HDMI EDID能力自动匹配*/
    HI_UNF_SND_HDMI_MODE_BUTT
} HI_UNF_SND_HDMI_MODE_E;

/**SPDIF MODE:LPCM,RAW*/
/**CNcomment:SPDIF 模式:LPCM,RAW*/
typedef enum hiHI_UNF_SND_SPDIF_MODE_E
{
    HI_UNF_SND_SPDIF_MODE_LPCM,           /**<SPDIF LCPM2.0*/ /**<CNcomment: 立体声pcm*/
    HI_UNF_SND_SPDIF_MODE_RAW,            /**<SPDIF Pass-through.*/ /**<CNcomment: SPDIF透传*/
    HI_UNF_SND_SPDIF_MODE_BUTT
} HI_UNF_SND_SPDIF_MODE_E;
/** @} */  /** <!-- ==== Structure Definition end ==== */


/******************************* API declaration *****************************/
/** \addtogroup      SOUND */
/** @{ */  /** <!--  【SOUND】 */

/** 
\brief Initializes an AO device. CNcomment:初始化音频输出设备 CNend
\attention \n
Before calling the SND module, you must call this application programming interface (API). CNcomment:调用SND模块要求首先调用该接口 CNend
\param N/A
\retval ::HI_SUCCESS Success CNcomment:成功 CNend
\see \n
N/A
*/
HI_S32 HI_UNF_SND_Init(HI_VOID);

/** 
\brief Deinitializes an AO device. CNcomment:去初始化音频输出设备 CNend
\attention \n
N/A
\param N/A
\retval ::HI_SUCCESS Success CNcomment:成功 CNend 
\see \n
N/A
*/
HI_S32 HI_UNF_SND_DeInit(HI_VOID);

/** 
\brief Obtains the default configured parameters of an AO device. CNcomment:获取音频输出设备默认设置参数 CNend
\attention \n
\param[in] enSound     ID of an AO device CNcomment:音频输出设备号 CNend
\param[out] pstAttr     Audio attributes CNcomment:音频属性 CNend
\retval ::HI_SUCCESS Success CNcomment:成功 CNend
\retval ::HI_ERR_AO_INVALID_ID	    The parameter enSound is invalid. CNcomment:无效Sound ID CNend
\retval ::HI_ERR_AO_NULL_PTR           The pointer is null. CNcomment:指针参数为空 CNend
\see \n
N/A
*/
HI_S32 HI_UNF_SND_GetDefaultOpenAttr(HI_UNF_SND_E enSound, HI_UNF_SND_ATTR_S *pstAttr);

/** 
\brief Starts an AO device. CNcomment:打开音频输出设备 CNend
\attention \n
N/A
\param[in] enSound     ID of an AO device CNcomment:音频输出设备号 CNend
\param[in] pstAttr     Attribute of an AO device CNcomment:音频输出设备参数 CNend
\retval ::HI_SUCCESS Success CNcomment:成功 CNend
\retval ::HI_FAILURE FAILURE CNcomment:失败 CNend
\retval ::HI_ERR_AO_INVALID_PARA	    The parameter is invalid. CNcomment:无效的参数 CNend
\retval ::HI_ERR_AO_INVALID_ID	    The parameter enSound is invalid. CNcomment:无效Sound ID CNend
\retval ::HI_ERR_AO_NULL_PTR               The pointer is null. CNcomment:指针参数为空 CNend
\see \n
N/A
*/
HI_S32 HI_UNF_SND_Open(HI_UNF_SND_E enSound, const HI_UNF_SND_ATTR_S *pstAttr);

/** 
\brief Destroys a AO SND instance. CNcomment:销毁音频输出Sound实例 CNend
\attention \n
An instance cannot be destroyed repeatedly. CNcomment:不支持重复销毁 CNend
\param[in] enSound     ID of an AO device CNcomment:音频输出设备号 CNend
\retval ::HI_SUCCESS Success CNcomment:成功 CNend
\retval ::HI_ERR_AO_INVALID_ID	    The parameter enSound is invalid. CNcomment:无效Sound ID CNend
\see \n
N/A
*/
HI_S32 HI_UNF_SND_Close(HI_UNF_SND_E enSound);


/** 
\brief Sets the mute status of  AO ports. CNcomment:音频输出静音开关设置 CNend
\attention \n
N/A
\param[in] enSound 
\param[in] enOutPort CNcomment:sound输出端口 CNend
\param[in] bMute
\retval ::HI_SUCCESS Success CNcomment:成功 CNend
\retval ::HI_FAILURE FAILURE CNcomment:失败 CNend
\retval ::HI_ERR_AO_SOUND_NOT_OPEN    Sound device is not opened. CNcomment:Sound设备未打开 CNend
\retval ::HI_ERR_AO_INVALID_PARA	    The parameter is invalid. CNcomment:无效的参数 CNend
\retval ::HI_ERR_AO_INVALID_ID	    The parameter enSound is invalid. CNcomment:无效Sound ID CNend
\see \n
N/A
*/
HI_S32   HI_UNF_SND_SetMute(HI_UNF_SND_E enSound, HI_UNF_SND_OUTPUTPORT_E enOutPort, HI_BOOL bMute);

/** 
\brief Obtains the mute status of AO ports. CNcomment:获取音频输出的静音开关状态 CNend
\attention \n
N/A
\param[in] enSound CNcomment:
\param[in] enOutPort CNcomment:sound输出端口 CNend
\param[out] pbMute CNcomment: 
\retval ::HI_SUCCESS Success CNcomment:成功 CNend
\retval ::HI_FAILURE FAILURE CNcomment:失败 CNend
\retval ::HI_ERR_AO_SOUND_NOT_OPEN    Sound device is not opened. CNcomment:Sound设备未打开 CNend
\retval ::HI_ERR_AO_NULL_PTR           The pointer is null. CNcomment:指针参数为空 CNend
\retval ::HI_ERR_AO_INVALID_PARA	    The parameter is invalid. CNcomment:无效的参数 CNend
\retval ::HI_ERR_AO_INVALID_ID	    The parameter enSound is invalid. CNcomment:无效Sound ID CNend
\see \n
N/A
*/
HI_S32   HI_UNF_SND_GetMute(HI_UNF_SND_E enSound, HI_UNF_SND_OUTPUTPORT_E enOutPort, HI_BOOL *pbMute);

/** 
\brief Sets the output mode of the HDMI(Auto/PCM/RAW/HBR2LBR).
\attention \n
\param[in] enSound CNcomment:音频输出设备号  CNend
\param[in] enOutPort  Audio OutputPort   CNcomment:音频输出端口 CNend
\param[in] enHdmiMode HDMI mode CNcomment:HDMI模式CNend
\retval ::HI_SUCCESS Success CNcomment:成功 CNend
\retval ::HI_FAILURE FAILURE CNcomment:失败 CNend
\retval ::HI_ERR_AO_SOUND_NOT_OPEN    Sound device is not opened. CNcomment:Sound设备未打开 CNend
\retval ::HI_ERR_AO_INVALID_ID	    The parameter enSound is invalid. CNcomment:无效Sound ID CNend
\see \n
N/A
*/
HI_S32   HI_UNF_SND_SetHdmiMode(HI_UNF_SND_E enSound, HI_UNF_SND_OUTPUTPORT_E enOutPort, HI_UNF_SND_HDMI_MODE_E enHdmiMode);

/** 
\brief Gets the output mode of the HDMI.
\attention \n
\param[in] enSound CNcomment:音频输出设备号  CNend
\param[in] enOutPort  Audio OutputPort   CNcomment:音频输出端口 CNend
\param[out] Pointer to the obtained HDMI mode CNcomment:获取到的HDMI模式的指针CNend
\retval ::HI_SUCCESS Success CNcomment:成功 CNend
\retval ::HI_FAILURE FAILURE CNcomment:失败 CNend
\retval ::HI_ERR_AO_SOUND_NOT_OPEN    Sound device is not opened. CNcomment:Sound设备未打开 CNend
\retval ::HI_ERR_AO_INVALID_ID	    The parameter enSound is invalid. CNcomment:无效Sound ID CNend
\see \n
N/A
*/
HI_S32   HI_UNF_SND_GetHdmiMode(HI_UNF_SND_E enSound, HI_UNF_SND_OUTPUTPORT_E enOutPort, HI_UNF_SND_HDMI_MODE_E *penHdmiMode);

/** 
\brief Sets the output mode of the SPDIF(PCM/RAW).
\attention \n
\param[in] enSound CNcomment:音频输出设备号  CNend
\param[in] enOutPort  Audio OutputPort   CNcomment:音频输出端口 CNend
\param[in] enHdmiMode SPDIF mode CNcomment:SPDIF模式CNend
\retval ::HI_SUCCESS Success CNcomment:成功 CNend
\retval ::HI_FAILURE FAILURE CNcomment:失败 CNend
\retval ::HI_ERR_AO_SOUND_NOT_OPEN    Sound device is not opened. CNcomment:Sound设备未打开 CNend
\retval ::HI_ERR_AO_INVALID_ID    The parameter enSound is invalid. CNcomment:无效Sound ID CNend
\see \n
N/A
*/                                               
HI_S32   HI_UNF_SND_SetSpdifMode(HI_UNF_SND_E enSound, HI_UNF_SND_OUTPUTPORT_E enOutPort, HI_UNF_SND_SPDIF_MODE_E enSpdifMode);

/** 
\brief Gets the output mode of the SPDIF.
\attention \n
\param[in] enSound CNcomment:音频输出设备号  CNend
\param[in] enOutPort  Audio OutputPort   CNcomment:音频输出端口 CNend
\param[out] Pointer to the obtained SPDIF mode CNcomment:获取到的SPDIF模式的指针CNend
\retval ::HI_SUCCESS Success CNcomment:成功 CNend
\retval ::HI_FAILURE FAILURE CNcomment:失败 CNend
\retval ::HI_ERR_AO_SOUND_NOT_OPEN    Sound device is not opened. CNcomment:Sound设备未打开 CNend
\retval ::HI_ERR_AO_INVALID_ID    The parameter enSound is invalid. CNcomment:无效Sound ID CNend
\retval ::HI_ERR_AO_NULL_PTR               The pointer is null. CNcomment:指针参数为空 CNend
\see \n
N/A
*/                                               
HI_S32   HI_UNF_SND_GetSpdifMode(HI_UNF_SND_E enSound, HI_UNF_SND_OUTPUTPORT_E enOutPort, HI_UNF_SND_SPDIF_MODE_E *penSpdifMode);


/**
 \brief Sets the output volume value. CNcomment:设置输出音量 CNend
 \attention \n
If s32Gain is set to a value greater than 100 or 0dB, then return failure. CNcomment:如果s32Gain设置大于100或0dB，返回失败 CNend
 \param[in] enSound        ID of an AO device CNcomment:音频输出设备号 CNend
 \param[in] enOutPort  Audio OutputPort     CNcomment:音频输出端口 CNend
 \param[in] pstGain     Volume value CNcomment:设置的音量值 CNend
 \retval ::HI_SUCCESS Success CNcomment:成功 CNend
 \retval ::HI_FAILURE FAILURE CNcomment:失败 CNend
 \retval ::HI_ERR_AO_SOUND_NOT_OPEN    Sound device is not opened. CNcomment:Sound设备未打开 CNend
 \retval ::HI_ERR_AO_INVALID_PARA	    The parameter is invalid. CNcomment:无效的参数 CNend
 \retval ::HI_ERR_SND_INVALID_ID	    The parameter enSound is invalid. CNcomment:无效Sound ID CNend
 \retval ::HI_ERR_AO_NULL_PTR               The pointer is null. CNcomment:指针参数为空 CNend
 \see \n
N/A
 */
HI_S32   HI_UNF_SND_SetVolume(HI_UNF_SND_E enSound, HI_UNF_SND_OUTPUTPORT_E enOutPort, const HI_UNF_SND_GAIN_ATTR_S *pstGain);

/** 
\brief Obtains the output volume value. CNcomment:获取输出音量 CNend
\attention \n
The default linear volume value is 100 and abslute volume is 0dB. CNcomment:查询的默认音量值为100(线性音量) and 0dB(绝对音量) CNend
\param[in] enSound         ID of an AO device CNcomment:音频输出设备号 CNend
 \param[in] enOutPort  Audio OutputPort     CNcomment:音频输出端口 CNend
 \param[out] pstGain    Pointer to the obtained volume value CNcomment:指针类型，获取到的音量值 CNend
\retval ::HI_SUCCESS Success CNcomment:成功 CNend
\retval ::HI_FAILURE FAILURE CNcomment:失败 CNend
\retval ::HI_ERR_AO_SOUND_NOT_OPEN    Sound device is not opened. CNcomment:Sound设备未打开 CNend
\retval ::HI_ERR_AO_INVALID_PARA	    The parameter is invalid. CNcomment:无效的参数 CNend
\retval ::HI_ERR_AO_INVALID_ID	    The parameter enSound is invalid. CNcomment:无效Sound ID CNend
\retval ::HI_ERR_AO_NULL_PTR               The pointer is null. CNcomment:指针参数为空 CNend
\see \n
N/A
*/
HI_S32   HI_UNF_SND_GetVolume(HI_UNF_SND_E enSound, HI_UNF_SND_OUTPUTPORT_E enOutPort, HI_UNF_SND_GAIN_ATTR_S *pstGain);

/** 
\brief Sets the sampling rate during audio output. CNcomment:设置音频输出时的采样率 CNend
\attention \n
At present, the sampling rate cannot be set, and it is fixed at 48 kHz. The streams that are not sampled at 48 kHz are resampled at 48 kHz.
CNcomment:目前输出采样率默认为48k(实际输出采样率为48K~192K)，支持从8K到192K码流输入，因最大支持6倍重采样，\n
因此当设定输出采样率为192K时(实际输出采样率为192K)，播小于32K的码流会出错(此时不影响其它采样率的码流切换)\n
然而当设定输出采样率为8K时(实际输出采样率为8K~192K)，此时不能通过HDMI输出小于32K的码流(HDMI不支持) CNend
\param[in] enSound          ID of an AO device CNcomment:音频输出设备号 CNend
\param[in] enSampleRate    Audio sampling rate. For details, see the description of ::HI_UNF_SAMPLE_RATE_E. CNcomment:音频采样率。请参见::HI_UNF_SAMPLE_RATE_E CNend
\retval ::HI_SUCCESS Success CNcomment:成功 CNend
\retval ::HI_ERR_AO_SOUND_NOT_OPEN    Sound device is not opened. CNcomment:Sound设备未打开 CNend
\retval ::HI_ERR_AO_INVALID_PARA	    The parameter is invalid. CNcomment:无效的参数 CNend
\see \n
N/A
*/
HI_S32 HI_UNF_SND_SetSampleRate(HI_UNF_SND_E enSound, HI_UNF_SAMPLE_RATE_E enSampleRate);

/** 
\brief Obtains the sampling rate during audio output. CNcomment:获取音频输出时的采样率 CNend
\attention \n
The 48 kHz sampling rate is returned by default. CNcomment:此接口默认返回48kHz采样率 CNend
\param[in] enSound           ID of an AO device CNcomment:音频输出设备号 CNend
\param[out] penSampleRate   Pointer to the type of the audio sampling rate CNcomment:指针类型，音频采样率的类型 CNend
\retval ::HI_SUCCESS Success CNcomment:成功 CNend
\retval ::HI_ERR_AO_SOUND_NOT_OPEN    Sound device is not opened. CNcomment:Sound设备未打开 CNend
\retval ::HI_ERR_AO_NULL_PTR           The pointer is null. CNcomment:指针参数为空 CNend
\retval ::HI_ERR_AO_INVALID_PARA	    The parameter is invalid. CNcomment:无效的参数 CNend
\see \n
N/A
*/
HI_S32 HI_UNF_SND_GetSampleRate(HI_UNF_SND_E enSound, HI_UNF_SAMPLE_RATE_E *penSampleRate);

/** 
\brief Sets the smart volume for audio output. CNcomment:音频输出进行智能音量处理处理开关设置 CNend
\attention \n
1. The smart volume is disabled by default.\n 
2. The smart volume is valid only for the master audio.\n 
3. The smart volume is enabled only when the program is switched. 
CNcomment:1. 默认关闭该智能音量\n
2. 智能音量仅对主音有效\n
3. 智能音量仅在切换节目时触发 CNend
\param[in] enSound     ID of an AO device CNcomment:音频输出设备号 CNend
\param[in] enOutPort  CNcomment:sound输出端口 CNend
\param[in] bSmartVolume     Smart volume enable, HI_TRUE: enabled; HI_FALSE: disabled CNcomment:是否打开智能音量。HI_TRUE：打开；HI_FALSE：关闭 CNend
\retval ::HI_SUCCESS Success CNcomment:成功 CNend
\retval ::HI_ERR_AO_SOUND_NOT_OPEN    Sound device is not opened. CNcomment:Sound设备未打开 CNend
\retval ::HI_ERR_AO_INVALID_PARA	    The parameter is invalid. CNcomment:无效的参数 CNend
\see \n
N/A
*/
HI_S32   HI_UNF_SND_SetSmartVolume(HI_UNF_SND_E enSound, HI_UNF_SND_OUTPUTPORT_E enOutPort, HI_BOOL bSmartVolume);

/** 
\brief Obtains the status of the smart volume for audio output. CNcomment:获取音频输出智能音量开关状态 CNend
\attention \n
\param[in] enSound     ID of an AO device CNcomment:音频输出设备号 CNend
\param[in] enOutPort  CNcomment:sound输出端口 CNend
\param[out] pbSmartVolume     Pointer to the enable status of the smart volume CNcomment:指针类型，是否打开智能音量 CNend
\retval ::HI_SUCCESS Success CNcomment:成功 CNend
\retval ::HI_ERR_AO_SOUND_NOT_OPEN    Sound device is not opened. CNcomment:Sound设备未打开 CNend
\retval ::HI_ERR_AO_INVALID_PARA	    The parameter is invalid. CNcomment:无效的参数 CNend
\retval ::HI_ERR_AO_NULL_PTR               The pointer is null. CNcomment:指针参数为空 CNend
\see \n
N/A
*/
HI_S32   HI_UNF_SND_GetSmartVolume(HI_UNF_SND_E enSound, HI_UNF_SND_OUTPUTPORT_E eOutPort, HI_BOOL *pbSmartVolume);

/** 
\brief Set the AO track mode. CNcomment:设置音频输出声道模式 CNend
\attention \n
N/A
\param[in] enSound     ID of an AO device CNcomment:音频输出设备号 CNend
\param[in] enOutPort   CNcomment:sound输出端口 CNend
\param[in] enMode     Audio track mode. For details, see the description of ::HI_UNF_TRACK_MODE_E. CNcomment:音频声道模式，请参见::HI_UNF_TRACK_MODE_E CNend
\retval ::HI_SUCCESS Success CNcomment:成功 CNend
\retval ::HI_ERR_AO_SOUND_NOT_OPEN    Sound device is not opened. CNcomment:Sound设备未打开 CNend
\retval ::HI_ERR_AO_INVALID_PARA	    The parameter is invalid. CNcomment:无效的参数 CNend
\see \n
N/A
*/
HI_S32 HI_UNF_SND_SetTrackMode(HI_UNF_SND_E enSound, HI_UNF_SND_OUTPUTPORT_E enOutPort, HI_UNF_TRACK_MODE_E enMode);

/** 
\brief Obtains the AO track mode. CNcomment:获取音频输出声道模式 CNend
\attention \n
N/A
\param[in] enSound     ID of an AO device CNcomment:音频输出设备号 CNend
\param[in] enOutPort   CNcomment:sound输出端口 CNend
\param[out] penMode   Pointer to the AO track mode. For details, see the description of ::HI_UNF_TRACK_MODE_E.
CNcomment:指针类型，音频声道模式。请参见::HI_UNF_TRACK_MODE_E CNend
\retval ::HI_SUCCESS Success CNcomment:成功 CNend
\retval ::HI_ERR_AO_SOUND_NOT_OPEN    Sound device is not opened. CNcomment:Sound设备未打开 CNend
\retval ::HI_ERR_AO_NULL_PTR           The pointer is null. CNcomment:指针参数为空 CNend
\retval ::HI_ERR_AO_INVALID_PARA    The parameter is invalid. CNcomment:无效的参数 CNend
\see \n
N/A
*/
HI_S32 HI_UNF_SND_GetTrackMode(HI_UNF_SND_E enSound, HI_UNF_SND_OUTPUTPORT_E enOutPort, HI_UNF_TRACK_MODE_E *penMode);

/** 
\brief Attaches the SND module to an audio/video player (AVPLAY). CNcomment:绑定音频输出Sound和AV（Audio Video）播放器 CNend
\attention \n
Before calling this API, you must create a player and ensure that the player has no output. CNcomment:调用此接口前必须先创建播放器，对应这路播放器没有输出 CNend
\param[in] hTrack             Instance handle of an AVPLAY CNcomment:Track 实例句柄 CNend
\param[in] hSource           Instance handle of an AVPLAY CNcomment:AV播放器播放实例句柄 CNend  
\retval ::HI_SUCCESS Success CNcomment:成功 CNend
\retval ::HI_ERR_AO_SOUND_NOT_OPEN    Sound device is not opened. CNcomment:Sound设备未打开 CNend
\retval ::HI_ERR_AO_INVALID_PARA	        The parameter is invalid. CNcomment:无效的参数 CNend
\see \n
N/A
*/

HI_S32   HI_UNF_SND_Attach(HI_HANDLE hTrack, HI_HANDLE hSource);

/** 
\brief Detaches the SND module from an AVPLAY. CNcomment:解除Track和AV播放器绑定 CNend
\attention \n
N/A
\param[in] hTrack             Instance handle of an AVPLAY CNcomment:Track 实例句柄 CNend 
\param[in] hSource    Instance handle of an AVPLAY CNcomment:AV播放器播放实例句柄 CNend
\retval ::HI_SUCCESS Success CNcomment:成功 CNend
\retval ::HI_ERR_AO_SOUND_NOT_OPEN    Sound device is not opened. CNcomment:Sound设备未打开 CNend
\retval ::HI_ERR_AO_INVALID_PARA        The parameter is invalid. CNcomment:无效的参数 CNend
\see \n
N/A
*/

HI_S32 HI_UNF_SND_Detach(HI_HANDLE hTrack, HI_HANDLE hSource);

/** 
\brief Sets the mixing weight of an audio player. CNcomment:设置音频Track 混音权重 CNend
\attention \n
The output volumes of two players are calculated as follows: (volume x weight 1 + volume x weight 2)/100. The formula of calculating the output volumes of multiple players is similar.
CNcomment:两个Track 输出音量的计算方法为：（设置的音量%权重1+设置的音量%权重2）/100，多个播放器的计算方法与此类似 CNend
\param[in] hTrack              ID of an AO device CNcomment:音频输出Track CNend
\param[in] stMixWeightGain   the MixWeight Gain, ranging from 0 to 100. 0: minimum value; 100: maximum value CNcomment:权重，范围为0～100。0：最小值；100：最大值 CNend
\retval ::HI_SUCCESS Success CNcomment:成功 CNend
\retval ::HI_ERR_AO_SOUND_NOT_OPEN    Sound device is not opened. CNcomment:Sound设备未打开 CNend
\retval ::HI_ERR_AO_INVALID_PARA	        The parameter is invalid. CNcomment:无效的参数 CNend
\retval ::HI_ERR_AO_NULL_PTR           The pointer is null. CNcomment:指针参数为空 CNend
\see \n
N/A
*/

HI_S32  HI_UNF_SND_SetTrackWeight(HI_HANDLE hTrack, const HI_UNF_SND_GAIN_ATTR_S *pstMixWeightGain);

/** 
\brief Obtains the mixing weight of an audio player. CNcomment:获取音频播放器混音权重 CNend
\attention \n

\param[in] hTrack              ID of an AO device CNcomment:音频输出Track CNend
\param[in] pstMixWeightGain     Pointer to the MixWeight Gain CNcomment:指针类型，权重属性 CNend
\retval ::HI_SUCCESS Success CNcomment:成功 CNend
\retval ::HI_ERR_AO_SOUND_NOT_OPEN    Sound device is not opened. CNcomment:Sound设备未打开 CNend
\retval ::HI_ERR_AO_NULL_PTR               The pointer is null. CNcomment:指针参数为空 CNend
\retval ::HI_ERR_AO_INVALID_PARA	        The parameter is invalid. CNcomment:无效的参数 CNend
\see \n
N/A
*/
HI_S32   HI_UNF_SND_GetTrackWeight(HI_HANDLE hTrack, HI_UNF_SND_GAIN_ATTR_S *pstMixWeightGain);

/**
 \brief Obtains the default configured parameters of an AO Track. CNcomment:获取音频输出Track默认设置参数 CNend
 \attention \n
 \param[in] enTrackType              Track Type CNcomment:Track类型 CNend
 \param[out] pstAttr     Audio attributes CNcomment:音频属性 CNend
 \retval ::HI_SUCCESS Success CNcomment:成功 CNend
 \retval ::HI_FAILURE  Failure  CNcomment:失败 CNend
 \retval ::HI_ERR_AO_NULL_PTR               The pointer is null. CNcomment:指针参数为空 CNend
 \see \n
N/A
 */
HI_S32   HI_UNF_SND_GetDefaultTrackAttr(HI_UNF_SND_TRACK_TYPE_E enTrackType, HI_UNF_AUDIOTRACK_ATTR_S *pstAttr);

/** 
\brief Create a Track. CNcomment:创建一路Track CNend
\attention \n
Create 8 output tracks(master/slave track) and 6 virtual tracks at the most. only create 1 master track on every sound. 
CNcomment:最多可创建8路输出track(master/slave), 6路虚拟track，每个sound只能创建一路master track CNend
\param[in] enSound     ID of an AO device CNcomment:音频输出设备号 CNend
\param[in] pTrackAttr  Track attributes CNcomment:指针类型，Track  属性 CNend
\param[out] phTrack   Pointer to the handle of the created Track CNcomment:指针类型，创建的Track 句柄 CNend
\retval ::HI_SUCCESS Success CNcomment:成功 CNend
\retval ::HI_FAILURE  Failure  CNcomment:失败 CNend
\retval ::HI_ERR_AO_SOUND_NOT_OPEN    Sound device is not opened. CNcomment:Sound设备未打开 CNend
\retval ::HI_ERR_AO_INVALID_PARA The parameter is invalid. CNcomment:无效的参数 CNend
\retval ::HI_ERR_AO_NULL_PTR           The pointer is null. CNcomment:指针参数为空 CNend
\see \n
N/A
*/
HI_S32   HI_UNF_SND_CreateTrack(HI_UNF_SND_E enSound,const HI_UNF_AUDIOTRACK_ATTR_S *pTrackAttr,HI_HANDLE *phTrack);

/** 
\brief Destroy a Track. CNcomment:销毁一路Track CNend
\attention \n
\param[in] hTrack   the handle of the Track CNcomment:Track 句柄 CNend
\retval ::HI_SUCCESS Success CNcomment:成功 CNend
\retval ::HI_ERR_AO_SOUND_NOT_OPEN    Sound device is not opened. CNcomment:Sound设备未打开 CNend
\retval ::HI_ERR_AO_INVALID_PARA The parameter is invalid. CNcomment:无效的参数 CNend
\see \n
N/A
*/
HI_S32   HI_UNF_SND_DestroyTrack(HI_HANDLE hTrack);

/** 
\brief set the attribute of  a track, reversed. CNcomment:设置某一路track的属性， 预留 CNend
\attention \n
\param[in] hTrack   the handle of the Track CNcomment:Track 句柄 CNend
\param[in] stTrackAttr   the attribute of the Track CNcomment:Track 属性 CNend
\retval ::HI_SUCCESS Success CNcomment:成功 CNend
\retval ::HI_ERR_AO_SOUND_NOT_OPEN    Sound device is not opened. CNcomment:Sound设备未打开 CNend
\retval ::HI_ERR_AO_INVALID_ID The parameter is invalid. CNcomment:无效的参数 CNend
\retval ::HI_ERR_AO_INVALID_PARA The parameter is invalid. CNcomment:无效的参数 CNend
\retval ::HI_ERR_AO_NULL_PTR           The pointer is null. CNcomment:指针参数为空 CNend
\see \n
N/A
*/
HI_S32   HI_UNF_SND_SetTrackAttr(HI_HANDLE hTrack, const HI_UNF_AUDIOTRACK_ATTR_S *pstTrackAttr);

/** 
\brief get the attribute of  a track, reversed. CNcomment:获取某一路track的属性， 预留 CNend
\attention \n
\param[in] hTrack   the handle of the Track CNcomment:Track 句柄 CNend
\param[in] pstTrackAttr   the attribute of the Track CNcomment:Track 属性 CNend
\retval ::HI_SUCCESS Success CNcomment:成功 CNend
\retval ::HI_ERR_AO_SOUND_NOT_OPEN    Sound device is not opened. CNcomment:Sound设备未打开 CNend
\retval ::HI_ERR_AO_INVALID_ID The parameter is invalid. CNcomment:无效的参数 CNend
\retval ::HI_ERR_AO_INVALID_PARA The parameter is invalid. CNcomment:无效的参数 CNend
\retval ::HI_ERR_AO_NULL_PTR           The pointer is null. CNcomment:指针参数为空 CNend
\see \n
N/A
*/
HI_S32   HI_UNF_SND_GetTrackAttr(HI_HANDLE hTrack, HI_UNF_AUDIOTRACK_ATTR_S *pstTrackAttr);

/** 
\brief Acquire the audio frame from the track. CNcomment:获取某一路track的音频帧 CNend
\attention \n
\param[in] hTrack   the handle of the Track CNcomment:Track 句柄 CNend
\param[out] pstAOFrame the audio frame  CNcomment:Track 音频帧 CNend
\retval ::HI_SUCCESS Success CNcomment:成功 CNend
\retval ::HI_ERR_AO_INVALID_ID The parameter is invalid. CNcomment:无效的参数 CNend
\retval ::HI_ERR_AO_INVALID_PARA   The parameter is invalid. CNcomment:无效的参数 CNend
\retval ::HI_ERR_AO_NULL_PTR           The pointer is null. CNcomment:指针参数为空 CNend
\see \n
N/A
*/
HI_S32   HI_UNF_SND_AcquireTrackFrame(HI_HANDLE hTrack, HI_UNF_AO_FRAMEINFO_S *pstAOFrame);

/** 
\brief Release the audio frame . CNcomment:释放track 音频帧 CNend
\attention \n
\param[in] hTrack   the handle of the Track CNcomment:Track 句柄 CNend
\param[in] pstAOFrame the audio frame  CNcomment:Track 音频帧 CNend
\retval ::HI_SUCCESS Success CNcomment:成功 CNend
\retval ::HI_ERR_AO_INVALID_ID	The parameter is invalid. CNcomment:无效的参数 CNend
\retval ::HI_ERR_AO_INVALID_PARA	The parameter is invalid. CNcomment:无效的参数 CNend
\retval ::HI_ERR_AO_NULL_PTR           The pointer is null. CNcomment:指针参数为空 CNend
\see \n
N/A
*/
HI_S32  HI_UNF_SND_ReleaseTrackFrame(HI_HANDLE hTrack, HI_UNF_AO_FRAMEINFO_S *pstAOFrame);

/** 
\brief  Transmits data to a slave track directly. CNcomment:直接将数据送到slave track CNend
\attention \n
1. If HI_ERR_AO_OUT_BUF_FULL is returned, you need to transmit the data that fails to be transmitted last time to ensure the audio continuity.
2. For the PCM data, the restrictions are as follows:
    s32BitPerSample must be set to 16.
    bInterleaved must be set to HI_TRUE. Only interlaced mode is supported.
    u32Channels can be set to 1 or 2.
    u32PtsMs can be ignored.
    ps32PcmBuffer indicates the PCM data pointer.
    ps32BitsBuffer can be ignored.
    u32PcmSamplesPerFrame indicates the number of audio sampling. The data length (in byte) is calculated as follows: u32PcmSamplesPerFrame x u32Channels x sizeof(HI_u16)
    u32BitsBytesPerFrame can be ignored.
    u32FrameIndex can be ignored.
CNcomment:1 如果返回HI_ERR_AO_OUT_BUF_FULL，需要调度者继续送上次失败数据，才能保证声音的连续
2 PCM 数据格式在混音器的限制如下
    s32BitPerSample: 必须为16
    bInterleaved: 必须为HI_TRUE, 仅支持交织模式
    u32Channels: 1 或2
    u32PtsMs: 忽略该参数
    ps32PcmBuffer: PCM 数据指针
    ps32BitsBuffer: 忽略该参数
    u32PcmSamplesPerFrame: 音频样点数, 数据长度(unit:Bytes): u32PcmSamplesPerFrame*u32Channels*sizeof(HI_u16)
    u32BitsBytesPerFrame: 忽略该参数
    u32FrameIndex: 忽略该参数 CNend
\param[in] hTrack   Track handle CNcomment:Track 句柄 CNend
\param[out] pstAOFrame   Information about the audio data CNcomment:音频数据信息 CNend
\retval ::HI_SUCCESS Success CNcomment:成功 CNend
\retval ::HI_ERR_AO_NULL_PTR	The parameter is invalid. CNcomment:无效的参数 CNend
\retval ::HI_ERR_AO_INVALID_PARA	The parameter is invalid. CNcomment:无效的参数 CNend
\retval ::HI_ERR_AO_OUT_BUF_FULL  Data fails to be transmitted because the mixer buffer is full. CNcomment:混音缓冲区数据满，送数据失败 CNend
\see \n
N/A
*/
HI_S32	 HI_UNF_SND_SendTrackData(HI_HANDLE hTrack, const HI_UNF_AO_FRAMEINFO_S *pstAOFrame);


/**
\brief create screen share channel. CNcomment: 获取共享通道设置默认属性 CNend
\attention \n
none. CNcomment:无
\param[in] enSound      display channel.CNcomment:播放通路 CNend
\param[out] pstAttr      handle of default attr  .CNcomment:设置默认属性句柄 CNend
\retval ::HI_SUCCESS CNcomment: success.成功 CNend
\retval ::HI_ERR_AO_SOUND_NOT_OPEN    Sound device is not opened. CNcomment:Sound设备未打开 CNend
\retval ::HI_ERR_AO_NULL_PTR          Input pointer is NULL.CNcomment:输入指针为空 CNend
\retval ::HI_ERR_AO_INVALID_PARA   invalid input parameter.CNcomment:输入参数非法 CNend
\retval ::HI_ERR_AO_INVALID_OPT    invalid operation. CNcomment:操作非法 CNend
\see \n
none.CNcomment:无 CNend
*/
HI_S32   HI_UNF_SND_GetDefaultCastAttr(HI_UNF_SND_E enSound, HI_UNF_SND_CAST_ATTR_S *pstAttr);

/**
\brief create screen share channel. CNcomment: 创建声音共享通道 CNend
\attention \n
none. CNcomment:无
\param[in] enSound      display channel.CNcomment:播放通路 CNend
\param[in] pstAttr      pointer of parameter .CNcomment:指针,属性参数 CNend
\param[out] phCast      handle of screen share .CNcomment:声音共享句柄 CNend
\retval ::HI_SUCCESS CNcomment: success.成功 CNend
\retval ::HI_ERR_AO_SOUND_NOT_OPEN    Sound device is not opened. CNcomment:Sound设备未打开 CNend
\retval ::HI_ERR_AO_NULL_PTR          Input pointer is NULL.CNcomment:输入指针为空 CNend
\retval ::HI_ERR_AO_INVALID_PARA   invalid input parameter.CNcomment:输入参数非法 CNend
\retval ::HI_ERR_AO_INVALID_OPT    invalid operation. CNcomment:操作非法 CNend
\see \n
none.CNcomment:无 CNend
*/
HI_S32 HI_UNF_SND_CreateCast(HI_UNF_SND_E enSound, HI_UNF_SND_CAST_ATTR_S *pstAttr, HI_HANDLE *phCast);

/**
\brief destroy screen share channel. CNcomment: 销毁声音共享通道 CNend
\attention \n
none. CNcomment:无 CNend
\param[in] phCast      handle of screen share .CNcomment:声音共享句柄 CNend
\retval ::HI_SUCCESS CNcomment: success.成功 CNend
\retval ::HI_ERR_AO_SOUND_NOT_OPEN    Sound device is not opened. CNcomment:Sound设备未打开 CNend
\retval ::HI_ERR_AO_NULL_PTR          Input pointer is NULL.CNcomment:输入指针为空 CNend
\retval ::HI_ERR_AO_INVALID_PARA   invalid input parameter.CNcomment:输入参数非法 CNend
\retval ::HI_ERR_AO_INVALID_OPT    invalid operation. CNcomment:操作非法 CNend
\see \n
none.CNcomment:无 CNend
*/
HI_S32 HI_UNF_SND_DestroyCast(HI_HANDLE hCast);

/**
\brief enable screen share. CNcomment: 使能声音共享功能 CNend
\attention \n
none. CNcomment:无
\param[in] phCast      handle of screen share .CNcomment:声音共享句柄
\param[in] bEnable      enable screen share .CNcomment:使能声音共享 CNend
\retval ::HI_SUCCESS CNcomment: success.成功 CNend
\retval ::HI_ERR_AO_SOUND_NOT_OPEN    Sound device is not opened. CNcomment:Sound设备未打开 CNend
\retval ::HI_ERR_AO_NULL_PTR          Input pointer is NULL.CNcomment:输入指针为空 CNend
\retval ::HI_ERR_AO_INVALID_PARA   invalid input parameter.CNcomment:输入参数非法 CNend
\retval ::HI_ERR_AO_INVALID_OPT    invalid operation. CNcomment:操作非法 CNend
\see \n
none.CNcomment:无 CNend
*/
HI_S32 HI_UNF_SND_SetCastEnable(HI_HANDLE hCast, HI_BOOL bEnable);

/**
\brief get enable flag of screen share. CNcomment: 获取声音共享是否使能 CNend
\attention \n
none. CNcomment:无 CNend
\param[in] phCast      handle of screen share .CNcomment:声音共享句柄 CNend
\param[out] bEnable     flag .CNcomment:标志 CNend
\retval ::HI_SUCCESS CNcomment: success.成功 CNend
\retval ::HI_ERR_AO_SOUND_NOT_OPEN    Sound device is not opened. CNcomment:Sound设备未打开 CNend
\retval ::HI_ERR_AO_NULL_PTR          Input pointer is NULL.CNcomment:输入指针为空 CNend
\retval ::HI_ERR_AO_INVALID_PARA   invalid input parameter.CNcomment:输入参数非法 CNend
\retval ::HI_ERR_AO_INVALID_OPT    invalid operation. CNcomment:操作非法 CNend
\see \n
none.CNcomment:无 CNend
*/
HI_S32 HI_UNF_SND_GetCastEnable(HI_HANDLE hCast, HI_BOOL *pbEnable);

/**
\brief get frame info of snd cast. CNcomment: 获取声音共享帧信息 CNend
\attention \n
Cast pcm data format  s32BitPerSample(16), u32Channels(2),bInterleaved(HI_TRUE), u32SampleRate(same as SND).
\param[in] hCast      handle of screen share .CNcomment:声音共享句柄 CNend
\param[out] pstCastFrame        frame info.CNcomment:帧信息 CNend
\param[in] u32TimeoutMs         acquire timeout.CNcomment:获取超时 CNend
\retval ::HI_SUCCESS CNcomment: success.成功 CNend
\retval ::HI_ERR_AO_SOUND_NOT_OPEN    Sound device is not opened. CNcomment:Sound设备未打开 CNend
\retval ::HI_ERR_AO_NULL_PTR          Input pointer is NULL.CNcomment:输入指针为空 CNend
\retval ::HI_ERR_V_INVALID_PARA   invalid input parameter.CNcomment:输入参数非法 CNend
\retval ::HI_ERR_AO_INVALID_OPT    invalid operation. CNcomment:操作非法 CNend
\retval ::HI_ERR_AO_CAST_TIMEOUT   no enough data.CNcomment:数据不够 CNend
\see \n
none.CNcomment:无 CNend
*/
HI_S32 HI_UNF_SND_AcquireCastFrame(HI_HANDLE hCast, HI_UNF_AO_FRAMEINFO_S *pstCastFrame, HI_U32 u32TimeoutMs);

/**
\brief release frame info of screen share. CNcomment: 释放声音共享帧信息 CNend
\attention \n
none. CNcomment:无 CNend
\param[in] hCast      handle of screen share .CNcomment:声音共享句柄 CNend
\param[in] pstCastFrame     frame info.CNcomment:帧信息 CNend
\param[in] u32TimeoutMs    release timeout.CNcomment:释放超时 CNend
\retval ::HI_SUCCESS CNcomment: success.成功 CNend
\retval ::HI_ERR_AO_SOUND_NOT_OPEN    Sound device is not opened. CNcomment:Sound设备未打开 CNend
\retval ::HI_ERR_AO_NULL_PTR          Input pointer is NULL.CNcomment:输入指针为空 CNend
\retval ::HI_ERR_AO_INVALID_PARA   invalid input parameter.CNcomment:输入参数非法 CNend
\retval ::HI_ERR_AO_INVALID_OPT    invalid operation. CNcomment:操作非法 CNend
\see \n
none.CNcomment:无 CNend
*/
HI_S32 HI_UNF_SND_ReleaseCastFrame(HI_HANDLE hCast, HI_UNF_AO_FRAMEINFO_S *pstCastFrame);

/** @} */  /** <!-- ==== API declaration end ==== */

#ifdef __cplusplus
 #if __cplusplus
}
 #endif
#endif /* __cplusplus */

#endif  /*__HI_UNF_SND_H__*/

