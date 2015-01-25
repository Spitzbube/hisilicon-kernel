
/*!*********************************************************************
*
* Copyright (c) 2006 HUAWEI - All Rights Reserved
*
* @file  pvr_index.c
* @Date  2009/4/3
* @Revision v1.0
* @brief make video stream index info for trick play.
*          support multi video standards
*
* Change History:
*
* Date             Author            Change
* ====             ======            ======
* 2009/4/3         z56361            Original
*
* Dependencies:
*
************************************************************************/

#include <linux/kernel.h>
#include <linux/string.h>

#include "demux_debug.h"
#include "drv_demux_config.h"
#include "drv_demux_bitstream.h"
#include "drv_demux_index.h"

/*!@attention :define the following macro if you want PTS be set to I frames only */
//#define  PUT_PTS_ON_I_FRAME_ONLY

/*!=======================================================================
                              constants
  =======================================================================*/
#define  FIDX_MAX_CTX_NUM       DMX_REC_CNT /* maximum channel can be processed are the same as record channel */
#define  SC_SUPPORT_DATA_SIZE   16          /*! SC support data size, 16 bytes */

/*! nal unit type */
#define  FIDX_NALU_TYPE_SLICE  1
#define  FIDX_NALU_TYPE_IDR    5
//#define  FIDX_NALU_TYPE_SEI    6
#define  FIDX_NALU_TYPE_SPS    7
#define  FIDX_NALU_TYPE_PPS    8

/*! slice type */
//#define  FIDX_SLICE_TYPE_P     0
//#define  FIDX_SLICE_TYPE_B     1
//#define  FIDX_SLICE_TYPE_I     2
//#define  FIDX_SLICE_TYPE_PP    5
//#define  FIDX_SLICE_TYPE_BB    6
//#define  FIDX_SLICE_TYPE_II    7


/*!=======================================================================
                               data structures
  =======================================================================*/
/*! start code type */
typedef enum hi_SC_TYPE_E
{
    SC_TYPE_UNKNOWN = 0,
    SC_TYPE_SPS,
    SC_TYPE_PPS,
    SC_TYPE_PIC,
    SC_TYPE_SLICE,
    SC_TYPE_NONSLICE,
    SC_TYPE_BUTT
} SC_TYPE_E;

/*! start code description */
typedef struct hi_SC_INFO_S
{
    SC_TYPE_E     eScType;
    HI_S32        s32ScID;           /*! for H.264, assign PPS or SPS ID; for non-H.264, assign the byte after 00 00 01 */
    HI_S32        s32SupScID;        /*! for H.264 only, record the SPS ID for the current used PPS */
    HI_S64        s64GlobalOffset;   /*! the offset of the start code, in the global(whole) stream data space */
    HI_S32        s32OffsetInPacket; /*! the offset of the start code, in the stream data packet */
    HI_S32        s32PacketCount;    /*! the stream data packet number where the start code was found */
} SC_INFO_S;

/*! state of the instance */
typedef enum hi_CTX_STATE_E
{
    CTX_STATE_DISABLE = 0,
    CTX_STATE_ENABLE,
    CTX_STATE_BUTT
} CTX_STATE_E;


/*! context */
typedef struct hi_FIDX_CTX_S
{
    CTX_STATE_E   eCtxState;
    VIDSTD_E      eVideoStandard;  /*! video standard type */
    STRM_TYPE_E   eStrmType;       /*! stream type, ES or PES */
    HI_U32        u32PTS;          /*! current PTS, usually equals to the pts of the latest stream packet */
    SC_INFO_S     stSPS[32];       /*! start code of the sequence level parameters.
                                          H264  - sps
                                          MPEG2 - sequence header
                                          AVS   - sequence header
                                          MPEG4 - VOL or higher  */
    HI_U8         u8SpsFresh[32];  /*! one SPS can be used by one I frame only, an I frame without SPS will be treated as P or B frame */
    SC_INFO_S     stPPS[256];      /*! picture level parameter
                                          H264  - pps
                                          MPEG2 - picture header
                                          AVS   - picture header
                                          MPEG4 - VOP header  */
    HI_U8         s32SpsIdForPps[256];

    //previous 2 bytes, for start code detection, to prevent the 00 or 00 00 lost
    HI_U8         u8Prev2Bytes[2]; /*! store the latest 2 byte  */
    HI_U32        u32PrevPts;            /*! the pts of the packet where the last 2nd byte in */
    HI_S64        s64PrevGlobalOffset;   /*! the global offset of the last 2nd byte */
    HI_S32        s32PrevPacketOffset;   /*! the offset in the packet of the last 2nd byte */
    HI_S32        s32PrevPacketCount;    /*! the packet count of the packet where the last 2nd byte in */
    HI_S64        s64PrevGlobalOffset_1; /*! the global offset of the last 1st byte */
    HI_S32        s32PrevPacketOffset_1; /*! the offset in the packet of the last 1st byte */
    HI_S32        s32PrevPacketCount_1;  /*! the packet count of the packet where the last 1st byte in */

    //this SC store the latest tetected start code
    HI_S32        s32ThisSCValid;  /*! indicate the support data of this start code is ready for use */
    SC_INFO_S     stThisSC;                           /*! when a start code was found, the support data is probably not enough. If so, this start */
    HI_U8         u8ThisSCData[SC_SUPPORT_DATA_SIZE]; /*! has to be stored temporarily to wait more data */
    HI_S32        s32ThisSCDataLen;  /*! actual support data size, usually equals to SC_SUPPORT_DATA_SIZE */

    SC_INFO_S     stSEIFollowSlice; /*! record the SEI start code followed the last slice of the previous picture. Generally this SEI is the start of a new picture */

    //frame,a set of frame info probably can be generated after 'this SC' was processed
    FRAME_POS_S   stNewFramePos;     /*! frame info to be output, temporarily stored here */
    HI_S32        s32WaitFrameSize;  /*! indicate if most info of stNewFramePos has been ready, except the frame size */
    HI_S32        s32SpsID;          /*! H264: SPS ID */
    HI_S32        s32PpsID;          /*! H264: PPS ID */
    HI_U32       *Param;
}FIDX_CTX_S;


/*!=======================================================================
                           static shared data
  =======================================================================*/
static FIDX_CTX_S  s_FidxIIS[FIDX_MAX_CTX_NUM];


/*! callback, used to output infomation */
static HI_VOID (*s_OutputFramePosition)(HI_U32 *Param, FRAME_POS_S *pstScInfo);

/*=======================================================================
                           function declaration
  =======================================================================*/
static HI_S32 ProcessThisSC(HI_S32 InstIdx);
static HI_S32 ProcessSC_Mpeg2(HI_S32 InstIdx);
static HI_S32 ProcessSC_Avs(HI_S32 InstIdx);
static HI_S32 ProcessSC_Mpeg4(HI_S32 InstIdx);
static HI_S32 ProcessSC_H264(HI_S32 InstIdx);


/*!=======================================================================
                             macros
  =======================================================================*/
/*! print */
#define PRINT   HI_INFO_DEMUX

/*! assertion */
#define  FIDX_ASSERT_RET( cond, else_print )                            \
do {                                                                    \
    if( !(cond) )                                                       \
    {                                                                   \
        PRINT("pvr_index.c,L%d: %s\n", __LINE__, else_print );          \
        return FIDX_ERR;                                                \
    }                                                                   \
}while(0)
#define  FIDX_ASSERT( cond, else_print )                                \
do {                                                                    \
    if( !(cond) )                                                       \
    {                                                                   \
        PRINT("pvr_index.c,L%d: %s\n", __LINE__, else_print );          \
        return;                                                         \
    }                                                                   \
}while(0)

/*! fill support data of the SC */
#define FILL_SC_DATA(ptr,len)                                           \
do {                                                                    \
    if( pstCtx->s32ThisSCDataLen + (len) >= SC_SUPPORT_DATA_SIZE )      \
    {                                                                   \
        memcpy(pstCtx->u8ThisSCData + pstCtx->s32ThisSCDataLen, ((const void *)ptr),    \
            (size_t)(SC_SUPPORT_DATA_SIZE - pstCtx->s32ThisSCDataLen));         \
        pstCtx->s32ThisSCDataLen = SC_SUPPORT_DATA_SIZE;                \
    }                                                                   \
    else                                                                \
    {                                                                   \
        memcpy(pstCtx->u8ThisSCData + pstCtx->s32ThisSCDataLen, ((const void *)ptr),    \
            ((size_t)len));                                                     \
        pstCtx->s32ThisSCDataLen += (len);                              \
    }                                                                   \
} while(0)

/*! analyze a prepared  start code */
#define ANANYSE_SC()                                                    \
do {                                                                    \
    ProcessThisSC((HI_S32)InstIdx);                                             \
    pstCtx->s32ThisSCValid = 0;                                         \
    pstCtx->s32ThisSCDataLen = 0;                                       \
} while(0)

/*! output prepared frame position infomation */
#define OUTPUT_FRAME()                                                  \
do{                                                                     \
    if( NULL != s_OutputFramePosition )                                 \
    {                                                                   \
        (HI_VOID)s_OutputFramePosition(pstCtx->Param, &pstCtx->stNewFramePos );       \
    }                                                                   \
    memset(&pstCtx->stNewFramePos, 0, sizeof(FRAME_POS_S));             \
    pstCtx->stNewFramePos.eFrameType = FIDX_FRAME_TYPE_UNKNOWN;         \
} while(0)

/*! decide if the SC is valid */
#define IS_SC_WRONG()   \
( pstCtx->s32ThisSCDataLen < 3 ||   \
(pstCtx->u8ThisSCData[0] == 0x00 && pstCtx->u8ThisSCData[1]==0x00 && pstCtx->u8ThisSCData[2]==0x01) )

/*! mpeg4 have multi-level sequence parameter, select the highest level */
#define SELECT_SPS( sps, pSelSps )                                      \
do{                                                                     \
    if( SC_TYPE_SPS == (sps).eScType &&                                 \
        (sps).s64GlobalOffset < (pSelSps)->s64GlobalOffset &&           \
        (sps).s64GlobalOffset + 64 >= (pSelSps)->s64GlobalOffset )      \
    {                                                                   \
        (pSelSps) = &(sps);                                             \
    }                                                                   \
} while (0)

#define BITS_2_BYTES(bitn)   (((bitn) + (7))>>(3))

/*!=======================================================================
                       function implementation
  =======================================================================*/

/*!***********************************************************************
    @brief global init, clear context, and register call back
 ************************************************************************/
HI_VOID FIDX_Init(HI_VOID (*OutputFramePosition)(HI_U32 *Param, FRAME_POS_S *pstScInfo))
{
    HI_S32 i;

    s_OutputFramePosition = OutputFramePosition;
    memset( s_FidxIIS, 0, FIDX_MAX_CTX_NUM*sizeof(FIDX_CTX_S) );
    for( i = 0; i < FIDX_MAX_CTX_NUM; i++ )
    {
        s_FidxIIS[i].eCtxState = CTX_STATE_DISABLE;
    }
}

/*!***********************************************************************
    @brief  open an instance
    @param[in]  VidStandard: video standard
    @return
        if success, return instance ID, 0~(FIDX_MAX_CTX_NUM-1)
        if fail, return -1
 ************************************************************************/
HI_S32  FIDX_OpenInstance(VIDSTD_E VidStandard, STRM_TYPE_E StrmType, HI_U32 *Param)
{
    HI_S32  ret = -1;
    HI_S32  i;

    FIDX_ASSERT_RET(VidStandard<VIDSTD_BUTT, "'VidStandard' out of range");
    FIDX_ASSERT_RET(StrmType<STRM_TYPE_BUTT, "'StrmType' out of range");

    /*! find an idle instance */
    for (i = 0; i < FIDX_MAX_CTX_NUM; i++)
    {
        if (s_FidxIIS[i].eCtxState != CTX_STATE_ENABLE)
        {
            memset(&s_FidxIIS[i], 0, sizeof(FIDX_CTX_S));

            s_FidxIIS[i].eCtxState          = CTX_STATE_ENABLE;
            s_FidxIIS[i].eVideoStandard     = VidStandard;
            s_FidxIIS[i].eStrmType          = StrmType;
            s_FidxIIS[i].u8Prev2Bytes[0]    = 0xff;
            s_FidxIIS[i].u8Prev2Bytes[1]    = 0xff;
            s_FidxIIS[i].Param              = Param;

            ret = i;

            break;
        }
    }

    return ret;
}

/*!***********************************************************************
    @brief   close instalce
 ************************************************************************/
HI_S32  FIDX_CloseInstance( HI_S32 InstIdx )
{
    FIDX_ASSERT_RET( InstIdx < FIDX_MAX_CTX_NUM, "InstIdx out of range");
    if( s_FidxIIS[InstIdx].eCtxState != CTX_STATE_ENABLE )
    {
        return FIDX_ERR;
    }
    else
    {
        memset( &s_FidxIIS[InstIdx], 0, sizeof(FIDX_CTX_S) );
        s_FidxIIS[InstIdx].eCtxState = CTX_STATE_DISABLE;
        return FIDX_OK;
    }
}

static HI_S32 IsPesSC(HI_U8 Code, VIDSTD_E eVidStd )
{
    HI_S32 ret = 0;

    if( eVidStd != VIDSTD_AUDIO_PES )
    {
        if( Code >= 0xe0 && Code <= 0xef )
        {
            ret = 1;
        }
    }
    else
    {
        if( Code >= 0xc0 && Code <= 0xdf )
        {
            ret = 1;
        }
    }

    return ret;
}

/*!***********************************************************************
@brief  entry of the start code process
@return
  if success return FIDX_OK, otherwise return FIDX_ERR
************************************************************************/
HI_S32 ProcessThisSC(HI_S32 InstIdx)
{
    FIDX_CTX_S *pstCtx = &s_FidxIIS[InstIdx];
    HI_S32 s32Ret = FIDX_ERR;

    switch( pstCtx->eVideoStandard )
    {
        case VIDSTD_MPEG2:
            s32Ret = ProcessSC_Mpeg2(InstIdx);
            break;
        case VIDSTD_H264:
            s32Ret = ProcessSC_H264(InstIdx);
            break;
        case VIDSTD_MPEG4:
            s32Ret = ProcessSC_Mpeg4(InstIdx);
            break;
        case VIDSTD_AVS:
            s32Ret = ProcessSC_Avs(InstIdx);
            break;
        default:
            s32Ret = FIDX_ERR;
            break;
    }

    return s32Ret;
}


/*!***********************************************************************
@brief
    process mpeg2 start code
@return
  if success return FIDX_OK, otherwise return FIDX_ERR
************************************************************************/
HI_S32 ProcessSC_Mpeg2(HI_S32 InstIdx)
{
    HI_U8 Code;
    HI_S32 s32Ret = FIDX_ERR;
    FIDX_CTX_S *pstCtx = &s_FidxIIS[InstIdx];
    SC_INFO_S   *pstSpsSC = &pstCtx->stSPS[0];
    SC_INFO_S   *pstThisSC = &pstCtx->stThisSC;
    FRAME_POS_S *pstFrmPos = &pstCtx->stNewFramePos;

    FIDX_ASSERT_RET( 0!=pstCtx->s32ThisSCValid, "ThisSC is not valid\n");
    FIDX_ASSERT_RET( !IS_SC_WRONG(), "not enough data for ThisSC\n");

    Code = pstCtx->u8ThisSCData[0];

    /*! if a frame is found, but its size have not been calculated, it is time to calc the frame size, and output this frame */
    if( pstFrmPos->eFrameType != FIDX_FRAME_TYPE_UNKNOWN )
    {
        if( 0xb3 == Code || 0x00 == Code )  /*! this means end of the formal frame found, the frame size can be calculated */
        {
            pstFrmPos->s32FrameSize = (HI_S32)(pstThisSC->s64GlobalOffset - pstFrmPos->s64GlobalOffset);
            if( pstFrmPos->s32FrameSize < 0 )
            {
                /*! just incase */
                pstFrmPos->eFrameType = FIDX_FRAME_TYPE_UNKNOWN;
            }
            else
            {
                OUTPUT_FRAME();
            }
        }
    }

    if( 0xb3 == Code )  /*! sequence header */
    {
        pstThisSC->s32ScID = (HI_S32)Code;
        pstThisSC->eScType = SC_TYPE_SPS;
        memcpy( pstSpsSC, pstThisSC, sizeof(SC_INFO_S) );
        pstCtx->u8SpsFresh[0] = 1;
        s32Ret = FIDX_ERR;
    }
    else if (0x00 == Code)  /*! picture header */
    {
        if( pstSpsSC->eScType != SC_TYPE_SPS ) /*! seqence parameters still not ready */
        {
            s32Ret = FIDX_ERR;
        }
        else
        {
            HI_U8 picture_coding_type;

            picture_coding_type = (pstCtx->u8ThisSCData[2] >> 3) & 7;
            //FIDX_ASSERT_RET(picture_coding_type>=1 && picture_coding_type<=3,
            //  "MPEG2 picture_coding_type out of range");
            if((picture_coding_type<1) || (picture_coding_type>3))
            {
                return FIDX_ERR;
            }
            if( 1 == picture_coding_type && 1 == pstCtx->u8SpsFresh[0])  /*! I frame */
            {
                pstFrmPos->eFrameType        = FIDX_FRAME_TYPE_I;
                pstFrmPos->s64GlobalOffset   = pstSpsSC->s64GlobalOffset;
                pstFrmPos->s32OffsetInPacket = pstSpsSC->s32OffsetInPacket;
                pstFrmPos->s32PacketCount    = pstSpsSC->s32PacketCount;
                pstCtx->u8SpsFresh[0] = 0;
                pstFrmPos->u32PTS            = pstCtx->u32PTS;
                pstCtx->u32PTS               = 0xffffffff;
            }
            else  /*! P or B frame */
            {
                pstFrmPos->eFrameType = (3==picture_coding_type)? FIDX_FRAME_TYPE_B: FIDX_FRAME_TYPE_P;
                pstFrmPos->s64GlobalOffset   = pstThisSC->s64GlobalOffset;
                pstFrmPos->s32OffsetInPacket = pstThisSC->s32OffsetInPacket;
                pstFrmPos->s32PacketCount    = pstThisSC->s32PacketCount;
#ifndef PUT_PTS_ON_I_FRAME_ONLY
                pstFrmPos->u32PTS            = pstCtx->u32PTS;
                pstCtx->u32PTS               = 0xffffffff;
#endif
            }
            pstFrmPos->s32FrameSize = 0; /*! frame size need the position of the next start code */
            s32Ret = FIDX_OK;
        }
    }

    return s32Ret;
}

/*!***********************************************************************
@brief
  process the start code of AVS
@return
  if success(find the start of a frame, and can output one index record)
  return FIDX_OK, otherwise return FIDX_ERR
************************************************************************/
HI_S32 ProcessSC_Avs(HI_S32 InstIdx)
{
    HI_U8 Code;
    HI_S32 s32Ret = FIDX_ERR;
    FIDX_CTX_S *pstCtx = &s_FidxIIS[InstIdx];
    SC_INFO_S   *pstSpsSC = &pstCtx->stSPS[0];
    SC_INFO_S   *pstThisSC = &pstCtx->stThisSC;
    FRAME_POS_S *pstFrmPos = &pstCtx->stNewFramePos;

    FIDX_ASSERT_RET( 0!=pstCtx->s32ThisSCValid, "ThisSC is not valid\n");
    FIDX_ASSERT_RET( !IS_SC_WRONG(), "not enough data for ThisSC\n");

    Code = pstCtx->u8ThisSCData[0];

    /*! if a frame is found, but its size have not been calculated, it is time to calc the frame size, and output this frame */
    if( pstFrmPos->eFrameType != FIDX_FRAME_TYPE_UNKNOWN )
    {
        if( 0xb0==Code || 0xb1==Code || 0xb3==Code || 0xb6==Code )  /*! this means end of the formal frame found, the frame size can be calculated */
        {
            pstFrmPos->s32FrameSize = (HI_S32)(pstThisSC->s64GlobalOffset - pstFrmPos->s64GlobalOffset);
            if( pstFrmPos->s32FrameSize < 0 )
            {
                /*! just incase */
                pstFrmPos->eFrameType = FIDX_FRAME_TYPE_UNKNOWN;
            }
            else
            {
                OUTPUT_FRAME();
            }
        }
    }

    if( 0xb0 == Code )  /* sequence header */
    {
        pstThisSC->s32ScID = (HI_S32)Code;
        pstThisSC->eScType = SC_TYPE_SPS;
        memcpy( pstSpsSC, pstThisSC, sizeof(SC_INFO_S) );
        pstCtx->u8SpsFresh[0] = 1;
        s32Ret = FIDX_ERR;
    }
    else if (0xb3 == Code || 0xb6 == Code)  /* picture header */
    {
        if( pstSpsSC->eScType != SC_TYPE_SPS ) /*! seqence parameters still not ready */
        {
            s32Ret = FIDX_ERR;
        }
        else
        {
            if( 0xb3 == Code && 1 == pstCtx->u8SpsFresh[0] )  /*! I frame */
            {
                pstFrmPos->eFrameType        = FIDX_FRAME_TYPE_I;
                pstFrmPos->s64GlobalOffset   = pstSpsSC->s64GlobalOffset;
                pstFrmPos->s32OffsetInPacket = pstSpsSC->s32OffsetInPacket;
                pstFrmPos->s32PacketCount    = pstSpsSC->s32PacketCount;

                pstFrmPos->u32PTS            = pstCtx->u32PTS;
                pstCtx->u32PTS               = 0xffffffff;
            }
            else  /*! P or B frame. The I frame without SPS before head also treated as P frame */
            {
                HI_U8 picture_coding_type = 1;
                if( 0xb6 == Code )
                {
                    picture_coding_type = (pstCtx->u8ThisSCData[3] >> 6) & 3;
                    FIDX_ASSERT_RET(1==picture_coding_type || 2==picture_coding_type,
                        "AVS picture_coding_type out of range");
                }

                pstFrmPos->eFrameType = (1==picture_coding_type)? FIDX_FRAME_TYPE_P: FIDX_FRAME_TYPE_B;
                pstFrmPos->s64GlobalOffset   = pstThisSC->s64GlobalOffset;
                pstFrmPos->s32OffsetInPacket = pstThisSC->s32OffsetInPacket;
                pstFrmPos->s32PacketCount    = pstThisSC->s32PacketCount;

#ifndef PUT_PTS_ON_I_FRAME_ONLY
                pstFrmPos->u32PTS            = pstCtx->u32PTS;
                pstCtx->u32PTS               = 0xffffffff;
#endif
            }
            pstFrmPos->s32FrameSize = 0; /*! frame size need the position of the next start code */
            s32Ret = FIDX_OK;
        }
    }

    return s32Ret;
}

/************************************************************************
@brief
  process the start code of MPEG4
@return
  if success(find the start of a frame, and can output one index record)
  return FIDX_OK, otherwise return FIDX_ERR
************************************************************************/
HI_S32 ProcessSC_Mpeg4(HI_S32 InstIdx)
{
    HI_U8 Code;
    HI_S32 s32Ret = FIDX_ERR;
    FIDX_CTX_S *pstCtx = &s_FidxIIS[InstIdx];
    SC_INFO_S   *pstSpsSC = &pstCtx->stSPS[0];
    SC_INFO_S   *pstThisSC = &pstCtx->stThisSC;
    FRAME_POS_S *pstFrmPos = &pstCtx->stNewFramePos;

    FIDX_ASSERT_RET( 0!=pstCtx->s32ThisSCValid, "ThisSC is not valid\n");
    FIDX_ASSERT_RET( !IS_SC_WRONG(), "not enough data for ThisSC\n");

    Code = pstCtx->u8ThisSCData[0];

    /*! if a frame is found, but its size have not been calculated, it is time to calc the frame size, and output this frame */
    if( pstFrmPos->eFrameType != FIDX_FRAME_TYPE_UNKNOWN )
    {
        if( 0xb2 != Code )  /*! this means end of the formal frame found, the frame size can be calculated */
        {
            pstFrmPos->s32FrameSize = (HI_S32)(pstThisSC->s64GlobalOffset - pstFrmPos->s64GlobalOffset);
            if( pstFrmPos->s32FrameSize < 0 )
            {
                 /*! just incase */
                pstFrmPos->eFrameType = FIDX_FRAME_TYPE_UNKNOWN;
            }
            else
            {
                OUTPUT_FRAME();
            }
        }
    }

    /*! distribution of the sequence level parameters:pstCtx->u8ThisSCData[x]
       [0]: B0, Visual_Object_Sequence
       [1]: b5, Visual_Object
       [2]: 00~1f, Video_Object
       [3]: 20~2f, Video_Object_Layer
    */
    if( 0xb0 == Code )  /* Visual_Object_Sequence */
    {
        pstThisSC->s32ScID = (HI_S32)Code;
        pstThisSC->eScType = SC_TYPE_SPS;
        memcpy( &pstSpsSC[0], pstThisSC, sizeof(SC_INFO_S) );
        s32Ret = FIDX_ERR;
    }
    else if ( 0xb5 == Code )  /* b5, Visual_Object */
    {
        pstThisSC->s32ScID = (HI_S32)Code;
        pstThisSC->eScType = SC_TYPE_SPS;
        memcpy( &pstSpsSC[1], pstThisSC, sizeof(SC_INFO_S) );
        s32Ret = FIDX_ERR;
    }
    else if ( Code <= 0x1f )  /* Video_Object: 0x00 ~ 0x1f */
    {
        pstThisSC->s32ScID = (HI_S32)Code;
        pstThisSC->eScType = SC_TYPE_SPS;
        memcpy( &pstSpsSC[2], pstThisSC, sizeof(SC_INFO_S) );
        s32Ret = FIDX_ERR;
    }
    else if ( Code >= 0x20 && Code <= 0x2f )  /* Video_Object_Layer */
    {
        pstThisSC->s32ScID = (HI_S32)Code;
        pstThisSC->eScType = SC_TYPE_SPS;
        memcpy( &pstSpsSC[3], pstThisSC, sizeof(SC_INFO_S) );
        pstCtx->u8SpsFresh[3] = 1;
        s32Ret = FIDX_ERR;
    }
    else if (0xb6 == Code)  /* VOP: picture header */
    {
        if( pstSpsSC[3].eScType != SC_TYPE_SPS ) /*! seqence parameters still not ready */
        {
            s32Ret = FIDX_ERR;
        }
        else
        {
            HI_U8 picture_coding_type;
            SC_INFO_S *pstLatestSps;

            /*! some mpeg4 streams have no paramaters above VOL, or those parameters are only transmittered at very low frequency */
            pstLatestSps = &pstSpsSC[3];  /*! select VOL as the highest sequence parameter by default, if have higher, overload it */
            SELECT_SPS(pstSpsSC[2], pstLatestSps);
            SELECT_SPS(pstSpsSC[1], pstLatestSps);
            SELECT_SPS(pstSpsSC[0], pstLatestSps);

            picture_coding_type = (pstCtx->u8ThisSCData[1] >> 6) & 3;
            FIDX_ASSERT_RET(picture_coding_type<=2,
                "MPEG4 picture_coding_type out of range");

            if( 0 == picture_coding_type && 1== pstCtx->u8SpsFresh[3])  /*! I frame */
            {
                pstFrmPos->eFrameType        = FIDX_FRAME_TYPE_I;
                pstFrmPos->s64GlobalOffset   = pstLatestSps->s64GlobalOffset;
                pstFrmPos->s32OffsetInPacket = pstLatestSps->s32OffsetInPacket;
                pstFrmPos->s32PacketCount    = pstLatestSps->s32PacketCount;
                pstCtx->u8SpsFresh[3] = 0;

                pstFrmPos->u32PTS            = pstCtx->u32PTS;
                pstCtx->u32PTS               = 0xffffffff;
            }
            else /*! P or B frame. The I frame without SPS before head also treated as P frame */
            {
                pstFrmPos->eFrameType = (2==picture_coding_type)? FIDX_FRAME_TYPE_B: FIDX_FRAME_TYPE_P;
                pstFrmPos->s64GlobalOffset   = pstThisSC->s64GlobalOffset;
                pstFrmPos->s32OffsetInPacket = pstThisSC->s32OffsetInPacket;
                pstFrmPos->s32PacketCount    = pstThisSC->s32PacketCount;

#ifndef PUT_PTS_ON_I_FRAME_ONLY
                pstFrmPos->u32PTS            = pstCtx->u32PTS;
                pstCtx->u32PTS               = 0xffffffff;
#endif
            }
            pstFrmPos->s32FrameSize = 0; /*! frame size need the position of the next start code */
            s32Ret = FIDX_OK;
        }
    }

    return s32Ret;
}

static HI_U32 simple_ue_v( HI_U32 Code, HI_U32 *BitNum )
{
    HI_U32 Val;
    HI_U32 zeros;
    zeros = (HI_S32)PvrZerosMS_32(Code);
    if (zeros < 16)
    {
        *BitNum = (zeros<<1) + 1;
        Val = (Code>>(32-(*BitNum))) - 1;
    }
    else
    {
        *BitNum = 0;
        Val = 0xffffffff;
    }

    return Val;
}

static HI_S32  ParseSPS( HI_U8 *pu8Data, HI_S32 s32DataLen, HI_S32 *p32SpsID )
{
    HI_U32  u32Code, u32Val, u32BitNum;

    u32Code = ((HI_U32)pu8Data[4]<<24) | ((HI_U32)pu8Data[5]<<16) |
        ((HI_U32)pu8Data[6]<<8) | ((HI_U32)pu8Data[7]);

    u32Val = simple_ue_v( u32Code, &u32BitNum );
    FIDX_ASSERT_RET( u32Val<=31, "SPS ID out of range!" );
    FIDX_ASSERT_RET( ((u32BitNum+7)>>3)<=(HI_U32)(s32DataLen-4), "data NOT enough for SPS" );

    *p32SpsID = (HI_S32)u32Val;
    return FIDX_OK;
}

static HI_S32  ParsePPS( HI_U8 *pu8Data, HI_S32 s32DataLen, HI_S32 *ps32SPSID, HI_S32 *ps32PPSID )
{
    HI_S32 s32Code;
    HI_U32 u32Val, u32BitNum;
    PVR_BS Bs;

    PvrBsInit(&Bs, pu8Data, s32DataLen);
    PvrBsSkip(&Bs,8);

    /* pic_parameter_set_id */
    s32Code = PvrBsShow(&Bs, 32);
    u32Val = simple_ue_v((HI_U32)s32Code,&u32BitNum );
    FIDX_ASSERT_RET( u32Val<=255, "PPS ID out of range!" );
    FIDX_ASSERT_RET((HI_S32)BITS_2_BYTES((HI_U32)PvrBsPos(&Bs))<= s32DataLen, "data NOT enough for PPS" );
    *ps32PPSID = (HI_S32)u32Val;
    PvrBsSkip(&Bs, (HI_S32)u32BitNum );

    /* seq_parameter_set_id */
    s32Code = PvrBsShow(&Bs, 32);
    u32Val = simple_ue_v( (HI_U32)s32Code, &u32BitNum );
    FIDX_ASSERT_RET( u32Val<=31, "SPS ID out of range!" );
    FIDX_ASSERT_RET( (HI_S32)BITS_2_BYTES((HI_U32)PvrBsPos(&Bs))<=s32DataLen, "data NOT enough for SPS" );
    *ps32SPSID = (HI_S32)u32Val;

    return FIDX_OK;
}

static HI_S32  ParseSliceHeader( HI_U8 *pu8Data, HI_S32 s32DataLen, HI_S32 *ps32FirstMBInSlice,
    HI_S32 *ps32SliceType, HI_S32 *ps32PPSId )
{
    HI_S32  s32Code;
    HI_U32  u32Val, u32BitNum;
    PVR_BS Bs;

    PvrBsInit(&Bs, pu8Data, s32DataLen);
    PvrBsSkip(&Bs,8);

    /* first_mb_in_slice */
    s32Code = PvrBsShow( &Bs, 32 );
    u32Val = simple_ue_v((HI_U32)s32Code, &u32BitNum );
    FIDX_ASSERT_RET( u32Val<=8192, "'first_mb_in_slice' out of range!" );
    FIDX_ASSERT_RET( (HI_S32)BITS_2_BYTES((HI_U32)PvrBsPos(&Bs))<=s32DataLen, "data NOT enough for 'first_mb_in_slice'" );
    *ps32FirstMBInSlice = (HI_S32)u32Val;
    PvrBsSkip(&Bs, (HI_S32)u32BitNum );

    /* slice_type */
    s32Code = PvrBsShow( &Bs, 32 );
    u32Val = simple_ue_v((HI_U32)s32Code, &u32BitNum );
    FIDX_ASSERT_RET( (u32Val<=2 || (u32Val>=5&&u32Val<=7) ), "'slice_type' out of range!" );
    FIDX_ASSERT_RET( (HI_S32)BITS_2_BYTES((HI_U32)PvrBsPos(&Bs))<=s32DataLen, "data NOT enough for 'slice_type'" );
    *ps32SliceType = (HI_S32)u32Val;
    PvrBsSkip(&Bs, (HI_S32)u32BitNum );

    /* pic_parameter_set_id */
    s32Code = PvrBsShow( &Bs, 32 );
    u32Val = simple_ue_v( (HI_U32)s32Code, &u32BitNum );
    FIDX_ASSERT_RET( (u32Val<=255), "'pic_parameter_set_id' out of range!" );
    FIDX_ASSERT_RET( (HI_S32)BITS_2_BYTES((HI_U32)PvrBsPos(&Bs))<=s32DataLen, "data NOT enough for 'pic_parameter_set_id'" );
    *ps32PPSId = (HI_S32)u32Val;

    return FIDX_OK;
}

/*!
@brief
  For H264, produce frame information.
  This function is to be called when frame border is found. The frame information produced will be stored into 'stNewFramePos'
*/

static HI_VOID H264MakeFrame( FIDX_CTX_S *pstCtx )
{
    SC_INFO_S   *pstSpsSC = &pstCtx->stSPS[0];
    SC_INFO_S   *pstEndSC = &pstCtx->stSEIFollowSlice;
    SC_INFO_S   *pstThisSC = &pstCtx->stThisSC;
    FRAME_POS_S *pstFrmPos = &pstCtx->stNewFramePos;

    if( FIDX_FRAME_TYPE_I == pstFrmPos->eFrameType && 1 == pstCtx->u8SpsFresh[pstCtx->s32SpsID] )
    {
        /*! SPS should be treaded as the start of the I frame following the SPS. */
        pstFrmPos->s64GlobalOffset = pstSpsSC[pstCtx->s32SpsID].s64GlobalOffset;
        pstFrmPos->s32OffsetInPacket = pstSpsSC[pstCtx->s32SpsID].s32OffsetInPacket;
        pstFrmPos->s32PacketCount = pstSpsSC[pstCtx->s32SpsID].s32PacketCount;
        pstCtx->u8SpsFresh[pstCtx->s32SpsID] = 0;
        if( SC_TYPE_NONSLICE != pstEndSC->eScType )
        {
            pstFrmPos->s32FrameSize = (HI_S32)(pstThisSC->s64GlobalOffset -
                            pstCtx->stSPS[pstCtx->s32SpsID].s64GlobalOffset);
        }
        else
        {
            pstFrmPos->s32FrameSize = (HI_S32)(pstEndSC->s64GlobalOffset -
                            pstCtx->stSPS[pstCtx->s32SpsID].s64GlobalOffset);
        }
    }
    else
    {
        if( FIDX_FRAME_TYPE_I == pstFrmPos->eFrameType && 1 != pstCtx->u8SpsFresh[pstCtx->s32SpsID] )
        {
            pstFrmPos->eFrameType = FIDX_FRAME_TYPE_P; /*! The I frame have no fresh SPS before head should be treaded as P frame */
        }
        if( SC_TYPE_NONSLICE != pstEndSC->eScType )
        {
            pstFrmPos->s32FrameSize = (HI_S32)(pstThisSC->s64GlobalOffset - pstFrmPos->s64GlobalOffset);
        }
        else
        {
            pstFrmPos->s32FrameSize = (HI_S32)(pstEndSC->s64GlobalOffset - pstFrmPos->s64GlobalOffset);
        }
    }

#ifdef PUT_PTS_ON_I_FRAME_ONLY
    /*! If This frame is P or B frame, do NOT consume PTS to prevent the following I frame have a PTS equals to -1 */
    if ( (FIDX_FRAME_TYPE_I != pstFrmPos->eFrameType) )
    {
        /* if the PTS has been used by current frame(and no new PTS was got later), restore the used PTS */
        if (0xffffffff == pstCtx->u32PTS)
        {
            pstCtx->u32PTS = pstFrmPos->u32PTS;
        }

        /* P or B frame have PTS equals to -1 */
        pstFrmPos->u32PTS = 0xffffffff;
    }
#endif
    return;
}

/*!***********************************************************************
@brief
  process H.264 start code
@return
if success(find the start of a frame, and can output one index record)
  return FIDX_OK, otherwise return FIDX_ERR
************************************************************************/
HI_S32 ProcessSC_H264(HI_S32 InstIdx)
{
    HI_S32 s32Ret = FIDX_ERR;
    HI_S32 NalUnitType;
    HI_S32 s32SpsID, s32PpsID, s32FirstMb, s32SliceType;
    FIDX_CTX_S *pstCtx = &s_FidxIIS[InstIdx];
    SC_INFO_S   *pstSpsSC = &pstCtx->stSPS[0];
    SC_INFO_S   *pstPpsSC = &pstCtx->stPPS[0];
    SC_INFO_S   *pstEndSC = &pstCtx->stSEIFollowSlice;
    SC_INFO_S   *pstThisSC = &pstCtx->stThisSC;
    FRAME_POS_S *pstFrmPos = &pstCtx->stNewFramePos;
    HI_U8       *pu8Data = &pstCtx->u8ThisSCData[0];
    static  HI_S32  SlcType2FrmType[10] = {2,3,1,0,0,2,3,1,0,0};

    FIDX_ASSERT_RET( 0!=pstCtx->s32ThisSCValid, "ThisSC is not valid\n");
    FIDX_ASSERT_RET( !IS_SC_WRONG(), "not enough data for ThisSC\n");

    if(0 != (pu8Data[0] & 0x80) )
    {
        return FIDX_ERR;
    }
    NalUnitType = (HI_S32)(pu8Data[0] & 31);

    /*! to find the frame border, the position of the start code after the last slice of this frame is needed */
    if( FIDX_NALU_TYPE_IDR != NalUnitType && FIDX_NALU_TYPE_SLICE != NalUnitType && SC_TYPE_UNKNOWN == pstEndSC->eScType )
    {
        memcpy( pstEndSC, pstThisSC, sizeof(SC_INFO_S) );
        pstEndSC->eScType = SC_TYPE_NONSLICE;
    }

    if( FIDX_NALU_TYPE_SPS == NalUnitType )
    {
        if( FIDX_OK == (s32Ret=ParseSPS( pu8Data, pstCtx->s32ThisSCDataLen, &s32SpsID )) )
        {
            /*! if the SPS with the same ID as the former is found, the end of the last frame is also found */
            if( FIDX_FRAME_TYPE_UNKNOWN != pstFrmPos->eFrameType && pstCtx->s32SpsID == s32SpsID )
            {
                H264MakeFrame(pstCtx);
                OUTPUT_FRAME();
            }
            pstThisSC->eScType = SC_TYPE_SPS;
            pstThisSC->s32ScID = s32SpsID;
            memcpy( &pstSpsSC[s32SpsID], pstThisSC, sizeof(SC_INFO_S) );
            pstCtx->u8SpsFresh[s32SpsID] = 1;
        }
    }
    else if ( FIDX_NALU_TYPE_PPS == NalUnitType )
    {
        if( FIDX_OK == (s32Ret=ParsePPS( pu8Data, pstCtx->s32ThisSCDataLen, &s32SpsID, &s32PpsID)) )
        {
            /*! check the if the SPS for this PPS have been parsed OK */
            if( SC_TYPE_SPS == pstCtx->stSPS[s32SpsID].eScType )
            {
                /*! if the PPS with the same ID as the former is found, the end of the last frame is also found */
                if( FIDX_FRAME_TYPE_UNKNOWN != pstFrmPos->eFrameType && pstCtx->s32PpsID == s32PpsID )
                {
                    H264MakeFrame(pstCtx);
                    OUTPUT_FRAME();
                }
                pstThisSC->eScType = SC_TYPE_PPS;
                pstThisSC->s32ScID = s32PpsID;
                pstThisSC->s32SupScID = s32SpsID;
                memcpy( &pstPpsSC[s32PpsID], pstThisSC, sizeof(SC_INFO_S) );
            }
            else
            {
                s32Ret = FIDX_ERR;
            }
        }
    }
    else if ( FIDX_NALU_TYPE_IDR == NalUnitType || FIDX_NALU_TYPE_SLICE == NalUnitType )
    {
        if( FIDX_OK == (s32Ret=ParseSliceHeader(pu8Data, pstCtx->s32ThisSCDataLen, &s32FirstMb, &s32SliceType, &s32PpsID)) )
        {
            /*! check if the SPS and PPS for this slice are parsed OK */
            if( SC_TYPE_PPS == pstPpsSC[s32PpsID].eScType &&
                SC_TYPE_SPS == pstSpsSC[pstPpsSC[s32PpsID].s32SupScID].eScType )
            {
                pstThisSC->eScType = SC_TYPE_SLICE;  /*! mark this SC is a slice SC*/
            }

            /*! find frame border, output one frame information if necessary */
            if( FIDX_FRAME_TYPE_UNKNOWN != pstFrmPos->eFrameType )
            {
                if( (0==s32FirstMb || s32PpsID!=pstCtx->s32PpsID) )
                {
                    /*! find frame border, the end of the last frame found. */
                    H264MakeFrame(pstCtx);
                    OUTPUT_FRAME();
                }
                else
                {
                    /*! This SC is still belongs to the last frame. It's slice type is ok to fresh the frame type */
                    HI_S32 s32FrmType = SlcType2FrmType[s32SliceType];
                    pstFrmPos->eFrameType = (s32FrmType > pstFrmPos->eFrameType)? s32FrmType: pstFrmPos->eFrameType;
                    pstThisSC->eScType = SC_TYPE_UNKNOWN;  /* mark ThisSC is not valid */
                }
            }

            /*! if ThisSC belongs to the next new frame, create new frame information here */
            if( FIDX_FRAME_TYPE_UNKNOWN == pstFrmPos->eFrameType && SC_TYPE_SLICE == pstThisSC->eScType )
            {
                if( SC_TYPE_NONSLICE != pstEndSC->eScType )
                {
                    pstFrmPos->s64GlobalOffset = pstThisSC->s64GlobalOffset;
                    pstFrmPos->s32OffsetInPacket = pstThisSC->s32OffsetInPacket;
                    pstFrmPos->s32PacketCount = pstThisSC->s32PacketCount;
                }
                else
                {
                    pstFrmPos->s64GlobalOffset = pstEndSC->s64GlobalOffset;
                    pstFrmPos->s32OffsetInPacket = pstEndSC->s32OffsetInPacket;
                    pstFrmPos->s32PacketCount = pstEndSC->s32PacketCount;
                }
                pstFrmPos->eFrameType = (FIDX_FRAME_TYPE_E)SlcType2FrmType[s32SliceType];
                pstFrmPos->s32FrameSize = 0;

                pstFrmPos->u32PTS = pstCtx->u32PTS;
                pstCtx->u32PTS = 0xffffffff;

                pstCtx->s32PpsID = s32PpsID;
                pstCtx->s32SpsID = pstPpsSC[s32PpsID].s32SupScID;
                pstEndSC->eScType = SC_TYPE_UNKNOWN;
            }
        }
        pstEndSC->eScType = SC_TYPE_UNKNOWN;  /*! clear SEI when slice found to ensure the SEI is the one following the slice */
    }

    return s32Ret;
}

/*!***********************************************************************
@brief
    feed start code to FIDX.
    there are 2 method to feed necessary information to FIDX:
    1. feed stream directly. Call FIDX_MakeFrameIndex()
    2. feed start code. In this method, the start code must be scanned outside,
       This call this function to create index.
 ************************************************************************/
HI_VOID  FIDX_FeedStartCode(HI_S32 InstIdx, const FINDEX_SCD_S *pstSC)
{
    FIDX_CTX_S *pstCtx;
    FRAME_POS_S PesFrame;

    pstCtx = &s_FidxIIS[InstIdx];

    /* Get SC info */
    pstCtx->stThisSC.s32ScID = pstSC->u8StartCode;
    pstCtx->stThisSC.s64GlobalOffset = pstSC->s64GlobalOffset;
    pstCtx->stThisSC.s32OffsetInPacket = 0;
    pstCtx->stThisSC.s32PacketCount = 0;

    /* fill SC data */
    pstCtx->u8ThisSCData[0] = pstSC->u8StartCode;
    memcpy(pstCtx->u8ThisSCData+1, pstSC->au8DataAfterSC, 8);
    pstCtx->s32ThisSCDataLen = 9;
    pstCtx->s32ThisSCValid = 1;

    /*! if this SC is a PES SC, output it here simply, otherwise process it according to the video standard */
    if( 1 == IsPesSC(pstSC->u8StartCode, pstCtx->eVideoStandard) )
    {
        /*! the PTS after PES SC is valid, record it */
        pstCtx->u32PTS  = pstSC->u32PtsMs;

        /*! report the PES position */
        memset( &PesFrame, 0, sizeof(FRAME_POS_S) );
        PesFrame.eFrameType = FIDX_FRAME_TYPE_PESH;
        PesFrame.s64GlobalOffset = pstCtx->stThisSC.s64GlobalOffset;
        PesFrame.s32OffsetInPacket = pstCtx->stThisSC.s32OffsetInPacket;
        PesFrame.s32PacketCount = pstCtx->stThisSC.s32PacketCount;
        PesFrame.u32PTS = pstSC->u32PtsMs;
        if( NULL != s_OutputFramePosition )
        {
            (HI_VOID)s_OutputFramePosition( pstCtx->Param, &PesFrame );
        }

        pstCtx->s32ThisSCValid = 0;
        pstCtx->s32ThisSCDataLen = 0;
    }
    else
    {
        ANANYSE_SC();
    }

    return;
}

#if 0
/*!***********************************************************************
@brief
    to see if the SC is usful for index making
@return
    if the SC is usful return FIDX_OK, otherwise return FIDX_ERR
 ************************************************************************/
HI_S32  FIDX_IsSCUseful(HI_S32 InstIdx, HI_U8 u8StartCode)
{
    HI_S32 s32Ret = FIDX_OK;
    FIDX_CTX_S *pstCtx = &s_FidxIIS[InstIdx];

    /*! filter the SC according to the video standard */
    switch( pstCtx->eVideoStandard )
    {
    case VIDSTD_MPEG2:
        if ((u8StartCode != 0x00) && (u8StartCode != 0xb3))
        {
            s32Ret = FIDX_ERR;
        }
        break;

    case VIDSTD_H264:
        if ((u8StartCode & 0x80) != 0)
        {
            s32Ret = FIDX_ERR;
        }
        else
        {
            u8StartCode &= 0x1f;
            if ( (u8StartCode != FIDX_NALU_TYPE_SLICE) && (u8StartCode != FIDX_NALU_TYPE_IDR) &&
                 (u8StartCode != FIDX_NALU_TYPE_SPS) && (u8StartCode != FIDX_NALU_TYPE_PPS) )
            {
                s32Ret = FIDX_ERR;
            }
        }
        break;

    case VIDSTD_MPEG4:
        if ( (u8StartCode != 0xb0) && (u8StartCode != 0xb5) &&  /* visual_object_sequense & visual_object */
             (u8StartCode > 0x2f) && /* video_object(00~1f) & video_object_layer(20~2f) */
             (u8StartCode != 0xb6) ) /* video_object_plane */
        {
            s32Ret = FIDX_ERR;
        }
        break;

    case VIDSTD_AVS:
        if ((u8StartCode != 0x00) && (u8StartCode != 0xb3))
        {
            s32Ret = FIDX_ERR;
        }
        break;
    default:
        s32Ret = FIDX_ERR;
        break;
    }

    return s32Ret;
}
#endif



