/******************************************************************************
Copyright (C), 2001-2021, Hisilicon Tech. Co., Ltd.
******************************************************************************
File Name     : hi_audsp_ade.h
Version       : Initial Draft
Author        : Hisilicon multimedia software group
Created       : 2013/01/28
Last Modified :
Description   : define audio common data structure
  History       :
  1.Date        : 2013/01/28
    Author      : zgjie
    Modification: Created file
******************************************************************************/

/**
 * \file
 * \brief Describes the information about the audio filter.
 */

/* TODO:

 */

#ifndef __HISI_AUDIO_DSP_ADEC_H__
#define __HISI_AUDIO_DSP_ADEC_H__

#include "hi_type.h"
#include "hi_audsp_common.h"

#ifdef __cplusplus
 #if __cplusplus
extern "C" {
 #endif
#endif /* __cplusplus */

/***************************** Macro Definition ******************************/
#define ADE_COM_REG_BASE (DSP0_SHARESRAM_BASEADDR + DSP0_SHARESRAM_ADE_OFFSET)  
#define ADE_CHN_REG_BASE (ADE_COM_REG_BASE + ADE_CHN_REG_OFFSET)
#define ADE_COM_REG_OFFSET 0x0000
#define ADE_CHN_REG_OFFSET sizeof(HI_ADE_COM_REGS_S)
#define ADE_COM_REG_BANDSIZE sizeof(HI_ADE_COM_REGS_S)
#define ADE_CHN_REG_BANDSIZE sizeof(HI_ADE_CHN_REGS_S)

//#define HI_ADE_NUMBER_MAX 0x8

#define HI_ADE_MAX_INSTANCE 4

/* ADE Channel Definition */
typedef enum
{
    ADE_CHAN_0 = 0x00,
    ADE_CHAN_1 = 0x01,
    ADE_CHAN_2 = 0x02,
    ADE_CHAN_3 = 0x03,
    ADE_CHAN_4 = 0x04,
    ADE_CHAN_5 = 0x05,
    ADE_CHAN_6 = 0x06,
    ADE_CHAN_7 = 0x07,
    ADE_CHAN_BUTT = 0x08,
} ADE_CHAN_ID_E;

/* ADE Channel Definition */
typedef enum
{
    ADE_CMD_OPEN_DECODER  = 0x00,
    ADE_CMD_CLOSE_DECODER = 0x01,
    ADE_CMD_START_DECODER = 0x02,
    ADE_CMD_STOP_DECODER   = 0x03,
    ADE_CMD_IOCTRL_DECODER = 0x04,
    ADE_CMD_BUTT = 0xff,
} ADE_CMD_E;

typedef enum
{
    ADE_CMD_DONE = 0,
    ADE_CMD_ERR_OPEN_DECODER_FAIL  = 1,
    ADE_CMD_ERR_CLOSE_DECODER_FAIL = 2,
    ADE_CMD_ERR_ATTACH_INBUF_FAIL = 3,
    ADE_CMD_ERR_DETACH_INBUF_FAIL = 4,
    ADE_CMD_ERR_START_FAIL = 5,
    ADE_CMD_ERR_STOP_FAIL   = 6,
    ADE_CMD_ERR_IOCTRL_FAIL = 7,
    ADE_CMD_ERR_UNKNOWCMD = 8,
    ADE_CMD_ERR_TIMEOUT = 9,

    ADE_CMD_ERR_BUTT
} ADE_CMD_RET_E;

typedef enum
{
    ADE_SAMPLE_RATE_UNKNOWN = 0,
    ADE_SAMPLE_RATE_8K,
    ADE_SAMPLE_RATE_11K,
    ADE_SAMPLE_RATE_12K,
    ADE_SAMPLE_RATE_16K,
    ADE_SAMPLE_RATE_22K,
    ADE_SAMPLE_RATE_24K,
    ADE_SAMPLE_RATE_32K,
    ADE_SAMPLE_RATE_44K,
    ADE_SAMPLE_RATE_48K,
    ADE_SAMPLE_RATE_88K,
    ADE_SAMPLE_RATE_96K,
    ADE_SAMPLE_RATE_176K,
    ADE_SAMPLE_RATE_192K,
} HI_ADE_SAMPLE_RATE_E;

typedef enum
{
    ADE_BIT_DEPTH_08 = 0,
    ADE_BIT_DEPTH_16,
    ADE_BIT_DEPTH_24,
} HI_ADE_BITDEPTH_E;

/************************************ Struct *************************************/

// Define the union U_ADE_OUT_INFO_S
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int OutSampleRate         : 4; // [6..0]
        unsigned int OrgSampleRate         : 4; // [7]
        unsigned int OutChannels           : 4; // [7]
        unsigned int OrgChannels           : 4; // [7]
        unsigned int BitPerSample          : 2; // [14..8]
        unsigned int AckEos                : 1; // [14..8]
        unsigned int Reserved_0            : 13; // [31..23]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_ADE_OUT_INFO_S;

typedef struct
{
    HI_U32 u32BufDataPhyAddr;         /*buffer physical addr*/
    HI_U32 u32BufDataVirAddr;         /*buffer kernel virtual addr*/
    HI_U32 u32BufPaddingSize;         /*buffer padding length*/
    HI_U32 u32BufSize;                /*buffer length*/
    HI_U32 u32Boundary;               /* pts read pointers wrap point */
} HI_ADE_INBUF_ATTR_S;

typedef struct
{
    HI_U32 u32UnitHeadSize;
    HI_U32 u32PeriodBufSize;        /* u32PeriodBufSize >= sizeof(HI_ADE_FRAME_UNIT_S) + u32PrivateDecInfoSize + u32PcmDataSize + u32RawDataSize */
    HI_U32 u32PeriodNumber;         /* 2/4/8 */
} HI_ADE_OUTBUF_ATTR_S;

typedef struct
{
    U_ADE_OUT_INFO_S unPcmAttr;
    U_ADE_OUT_INFO_S unLbrAttr;
    U_ADE_OUT_INFO_S unHbrAttr;

    HI_U32 u32PcmOutBytes;
    HI_U32 u32LbrOutBytes;
    HI_U32 u32HbrOutBytes;

    HI_U32 u32BitRate;
    HI_U32 u32CurnPtsReadPos;
    HI_U32 u32PrivateDecInfoSize;
    HI_U32 u32Reserved[7];     /* align to 16*sizof(HI_U32) */
} HI_ADE_FRAME_UNIT_S;

#if 1

/* dsp hw decoder */
/**Define the error codes of an HAD codec.*/
typedef enum
{
    HI_ADE_ErrorNone = 0,

    HI_ADE_ErrorInsufficientResources = (HI_S32) 0x80001000, /**<The device fails to be created due to insufficient resources.*/ /**<CNcomment:资源不够，创建设备失败 */

    HI_ADE_ErrorInvalidParameter = (HI_S32) 0x80001001, /**<The input parameter is invalid.*/ /**<CNcomment:输入参数非法 */

    HI_ADE_ErrorStreamCorrupt = (HI_S32) 0x80001002, /**<The decoding fails due to incorrect input streams.*/ /**<CNcomment:输入码流错误，解码失败 */

    HI_ADE_ErrorNotEnoughData = (HI_S32) 0x80001003, /**<The decoding ends due to insufficient streams.*/ /**<CNcomment:输入码流不够，退出解码 */

    HI_ADE_ErrorDecodeMode = (HI_S32) 0x80001004, /**<The decoding mode is not supported.*/ /**<CNcomment:解码模式不支持 */

    HI_ADE_ErrorNotSupportCodec = (HI_S32) 0x80001005,/**<The codec is not supported.*/

    HI_ADE_ErrorMax = 0x9FFFFFFF
} HI_ADE_ERRORTYPE_E;

typedef enum
{
    HI_ADE_DEC_MODE_RAWPCM = 0, /**<PCM decoding mode*/ /**<CNcomment:PCM 解码模式 */
    HI_ADE_DEC_MODE_THRU, /**<SPIDF61937 passthrough decoding mode only, such as AC3/DTS */ /**<CNcomment:透传解码模式 */
    HI_ADE_DEC_MODE_SIMUL, /**<PCM and passthrough decoding mode*/ /**<CNcomment:PCM + 透传解码模式 */
    HI_ADE_DEC_MODE_BUTT = 0x7FFFFFFF
} HI_ADE_DEC_MODE_E;

typedef struct
{
    HI_U32            u32CodecID;
    HI_ADE_DEC_MODE_E enDecMode;/**<IN.  */
    HI_VOID *         pCodecPrivateData;
    HI_U32            u32CodecPrivateParamsSize;
} HI_ADE_OPEN_PARAM_S;

typedef struct
{
    HI_U32   u32Cmd;
    HI_VOID* pPrivateSetConfigData;
    HI_U32   u32PrivateSetConfigDataSize;
} HI_ADE_IOCTRL_PARAM_S;

typedef struct
{
    HI_U8   *pu8Data;        /**<Data pointer*/
    HI_U32   u32Size;        /**<Data size*/
    HI_BOOL  bEosFlag;
} HI_ADE_STREAM_BUF_S;

typedef struct
{
    /* input */
    HI_U32 *pu32Outbuf;  /* pcm -> lbr -> hbr  */
    HI_U32  u32OutbufSize;

    /* output */
    HI_U32 u32StreamSampleRate;
    HI_U32 u32StreamChannels;
    HI_U32 u32StreamBitRate;

    HI_U32 u32PcmSampleRate;
    HI_U32 u32LbrSampleRate;
    HI_U32 u32HbrSampleRate;

    HI_U32 u32PcmChannels;
    HI_U32 u32LbrChannels;/* 2 */
    HI_U32 u32HbrChannels;/* 2(DDP) or 8 */

    HI_U32 u32PcmBitDepth;/* 16 or 24 */
    HI_U32 u32LbrBitDepth;/* 16 */
    HI_U32 u32HbrBitDepth;/* 16 */

    HI_U32 u32PcmBytesPerFrame;
    HI_U32 u32LbrBytesPerFrame;
    HI_U32 u32HbrBytesPerFrame;

    HI_BOOL bAckEosFlag;

    HI_U32 u32BsByteLeftAtDecoder; /* for PTS */
} HI_ADE_FRMAE_BUF_S;

typedef struct
{
    /* input */
    HI_U32 *pu32PrivateBuf;
    HI_U32  u32PrivateBufSize;

    /* output */
    HI_U32 u32PrivateOutBytes;
} HI_ADE_FRMAE_PRIV_INFO_S;

/**
 *Definition of the HAD decoder. CNcomment:HAD 解码器设备定义
 *
 */
typedef struct
{
    HI_U32 enCodecID;

    HI_ADE_ERRORTYPE_E (*DecInit)(HI_HANDLE * phDecoder,
                                  HI_ADE_OPEN_PARAM_S * pstOpenParam);

    HI_ADE_ERRORTYPE_E (*DecDeInit)(HI_HANDLE hDecoder );

    HI_ADE_ERRORTYPE_E (*DecSetConfig)(HI_HANDLE hDecoder, HI_ADE_IOCTRL_PARAM_S * pstConfigStructure);

    HI_ADE_ERRORTYPE_E (*DecGetMaxPcmOutSize)(HI_HANDLE hDecoder,
                                              HI_U32 * pu32OutSizes);

    HI_ADE_ERRORTYPE_E (*DecGetMaxBitsOutSize)(HI_HANDLE hDecoder,
                                               HI_U32 * pu32OutSizes);

    HI_ADE_ERRORTYPE_E (*DecInquiryBytesLeftInputBuffer)(HI_HANDLE hDecoder,
                                                         HI_U32 * pu32BytesLeft);

    HI_ADE_ERRORTYPE_E (*DecGetInputBuffer)(HI_HANDLE hDecoder,
                                            HI_U32 u32RequestSize, HI_ADE_STREAM_BUF_S * pstStream);

    HI_ADE_ERRORTYPE_E (*DecPutInputBuffer)(HI_HANDLE hDecoder,
                                            HI_ADE_STREAM_BUF_S * pstStream);

    HI_ADE_ERRORTYPE_E (*DecDecodeFrame)(HI_HANDLE hDecoder,
                                         HI_ADE_FRMAE_BUF_S * pstAOut, HI_ADE_FRMAE_PRIV_INFO_S * pstPrivateInfo);
} HI_ADE_DECODER_S;

#endif

static inline HI_U32  ADE_GET_Channels(U_ADE_OUT_INFO_S AudioInfo)
{
    return (HI_U32)(AudioInfo.bits.OutChannels);
}

static inline HI_U32  ADE_GET_OrgChannels(U_ADE_OUT_INFO_S AudioInfo)
{
    return (HI_U32)(AudioInfo.bits.OrgChannels);
}

static inline HI_U32  ADE_GET_BitPerSample(U_ADE_OUT_INFO_S AudioInfo)
{
    HI_U32 Bit = 16;
    HI_ADE_BITDEPTH_E enBitDepth = (HI_ADE_BITDEPTH_E)(AudioInfo.bits.BitPerSample);

    switch (enBitDepth)
    {
    case ADE_BIT_DEPTH_08:
        Bit = 8;
        break;
    case ADE_BIT_DEPTH_16:
        Bit = 16;
        break;
    case ADE_BIT_DEPTH_24:
        Bit = 24;
        break;
    }

    return Bit;
}

static inline HI_BOOL  ADE_GET_AckEos(U_ADE_OUT_INFO_S AudioInfo)
{
    if (AudioInfo.bits.AckEos)
    {
        return HI_TRUE;
    }
    else
    {
        return HI_FALSE;
    }
}

static inline HI_U32  ADE_GET_SampleRate(U_ADE_OUT_INFO_S AudioInfo)
{
    HI_U32 Rate = 0;
    HI_ADE_SAMPLE_RATE_E enRate = (HI_ADE_SAMPLE_RATE_E)(AudioInfo.bits.OutSampleRate);

    switch (enRate)
    {
    case ADE_SAMPLE_RATE_8K:
        Rate = 8000;
        break;
    case ADE_SAMPLE_RATE_11K:
        Rate = 8000;
        break;
    case ADE_SAMPLE_RATE_12K:
        Rate = 12000;
        break;
    case ADE_SAMPLE_RATE_16K:
        Rate = 16000;
        break;
    case ADE_SAMPLE_RATE_22K:
        Rate = 22050;
        break;
    case ADE_SAMPLE_RATE_24K:
        Rate = 24000;
        break;
    case ADE_SAMPLE_RATE_32K:
        Rate = 32000;
        break;
    case ADE_SAMPLE_RATE_44K:
        Rate = 44100;
        break;
    case ADE_SAMPLE_RATE_48K:
        Rate = 48000;
        break;
    case ADE_SAMPLE_RATE_88K:
        Rate = 88200;
        break;
    case ADE_SAMPLE_RATE_96K:
        Rate = 96000;
        break;
    case ADE_SAMPLE_RATE_176K:
        Rate = 176400;
        break;
    case ADE_SAMPLE_RATE_192K:
        Rate = 192000;
        break;
    default:
        Rate = 0;
    }

    return Rate;
}

static inline HI_U32  ADE_GET_OrgSampleRate(U_ADE_OUT_INFO_S AudioInfo)
{
    HI_U32 Rate = 0;
    HI_ADE_SAMPLE_RATE_E enRate = (HI_ADE_SAMPLE_RATE_E)(AudioInfo.bits.OrgSampleRate);

    switch (enRate)
    {
    case ADE_SAMPLE_RATE_8K:
        Rate = 8000;
        break;
    case ADE_SAMPLE_RATE_11K:
        Rate = 8000;
        break;
    case ADE_SAMPLE_RATE_12K:
        Rate = 12000;
        break;
    case ADE_SAMPLE_RATE_16K:
        Rate = 16000;
        break;
    case ADE_SAMPLE_RATE_22K:
        Rate = 22050;
        break;
    case ADE_SAMPLE_RATE_24K:
        Rate = 24000;
        break;
    case ADE_SAMPLE_RATE_32K:
        Rate = 32000;
        break;
    case ADE_SAMPLE_RATE_44K:
        Rate = 44100;
        break;
    case ADE_SAMPLE_RATE_48K:
        Rate = 48000;
        break;
    case ADE_SAMPLE_RATE_88K:
        Rate = 88200;
        break;
    case ADE_SAMPLE_RATE_96K:
        Rate = 96000;
        break;
    case ADE_SAMPLE_RATE_176K:
        Rate = 176400;
        break;
    case ADE_SAMPLE_RATE_192K:
        Rate = 192000;
        break;
    default:
        Rate = 0;
    }

    return Rate;
}

/* Define the union U_ADE_STATUS0 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int aver_ms               : 8; /* [7..0]  */
        unsigned int peak_ms               : 8; /* [15..8]  */
        unsigned int last_ms               : 8; /* [23..16]  */
        unsigned int Reserved_0            : 7; /* [30..24]  */
        unsigned int ena_profile           : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} U_ADE_STATUS0;

/* Define the union U_ADE_STATUS1 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int overflow_cnt_0        : 16; /* [15..0]  */
        unsigned int overflow_cnt_1        : 8; /* [23..16]  */
        unsigned int overflow_cnt_2        : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} U_ADE_STATUS1;

/* Define the union U_ADE_CTRL */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int cmd                   : 4; /* [3..0]  */
        unsigned int cmd_done              : 1; /* [4]  */
        unsigned int cmd_return_value      : 4; /* [8..5]  */
        unsigned int Reserved_2            : 23; /* [31..9]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} U_ADE_CTRL;

/* Define the union U_ADE_ATTR */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int msgpool_size          : 16; /* [15..0]  */
        unsigned int Reserved_5            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} U_ADE_ATTR;

/* Define the union U_IP_BUF_SIZE */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int buff_size             : 24; /* [23..0]  */
        unsigned int buff_eos_flag         : 1; /* [24]  */
        unsigned int Reserved_6            : 7; /* [31..25]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} U_IP_BUF_SIZE;

/* Define the union U_OP_BUF_SIZE */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int buff_size             : 24; /* [23..0]  */
        unsigned int period_num            : 5;  /* [27..24]  */
        unsigned int Reserved_9            : 3; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} U_OP_BUF_SIZE;

/* Define the union U_STATUS0 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int frame_cnt             : 16; /* [15..0]  */
        unsigned int Reserved_12           : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} U_STATUS0;

//==============================================================================

/* Define the global struct */
typedef struct
{
    volatile unsigned int  ADE_MAGIC;
    volatile unsigned int  ADE_VERSION;
    volatile U_ADE_STATUS0 ADE_STATUS0;
    volatile U_ADE_STATUS1 ADE_STATUS1;
    volatile unsigned int  ADE_RESERVED[12];
} HI_ADE_COM_REGS_S;

typedef struct
{
    volatile U_ADE_CTRL    ADE_CTRL;
    volatile unsigned int  ADE_MSGPOOL_ADDR;
    volatile unsigned int  CHAN_RESERVED0;
    volatile U_ADE_ATTR    ADE_ATTR;
    volatile unsigned int  IP_BUF_ADDR;
    volatile U_IP_BUF_SIZE IP_BUF_SIZE;
    volatile unsigned int  IP_BUF_WPTR;
    volatile unsigned int  IP_BUF_RPTR;
    volatile unsigned int  OP_BUF_ADDR;
    volatile U_OP_BUF_SIZE OP_BUF_SIZE;
    volatile unsigned int  OP_BUF_WPTR;
    volatile unsigned int  OP_BUF_RPTR;
    volatile U_STATUS0     STATUS0;
    volatile unsigned int  IP_PTS_READPOS;
    volatile unsigned int  IP_PTS_BOUNDARY;
    volatile unsigned int  CHAN_RESERVED1;
} HI_ADE_CHN_REGS_S;

/** @} */  /** <!-- ==== Structure Definition end ==== */

#ifdef __cplusplus
 #if __cplusplus
}
 #endif
#endif /* __cplusplus */

#endif /* #ifndef __HISI_AUDIO_DSP_ADEC_H__ */
