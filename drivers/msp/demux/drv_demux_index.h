
/***********************************************************************
*
* Copyright (c) 2006 HUAWEI - All Rights Reserved
*
* File: $pvr_index.h$
* Date: $2009/4/3$
* Revision: $v1.0$
* Purpose: make video stream index info for trick play.
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


#ifndef __FIDX_EXT_HEADER__
#define __FIDX_EXT_HEADER__

#include "hi_type.h"

#include "hi_unf_demux.h"

/************************************************************************/
/* constant                                                             */
/************************************************************************/
#define  FIDX_VERSION  20090804

#define  FIDX_OK       0
#define  FIDX_ERR      -1


/************************************************************************/
/* struct & enum                                                        */
/************************************************************************/
/* video standard type */
typedef enum hi_VIDSTD_E
{
	VIDSTD_MPEG2,
	VIDSTD_MPEG4,
	VIDSTD_AVS,
    VIDSTD_H264,
    VIDSTD_VC1,
    VIDSTD_H263,
    VIDSTD_DIVX3,
	VIDSTD_AUDIO_PES,
    VIDSTD_BUTT
}VIDSTD_E;

/* stream data type */
typedef enum hi_STRM_TYPE_E
{
    STRM_TYPE_ES = 0,
	STRM_TYPE_PES,
	STRM_TYPE_BUTT
}STRM_TYPE_E;

/* frame type */
typedef enum hi_FIDX_FRAME_TYPE_E
{
    FIDX_FRAME_TYPE_UNKNOWN = 0,
    FIDX_FRAME_TYPE_I,
	FIDX_FRAME_TYPE_P,
	FIDX_FRAME_TYPE_B,
	FIDX_FRAME_TYPE_PESH,
	FIDX_FRAME_TYPE_BUTT
} FIDX_FRAME_TYPE_E;

/* descriptor for frame information */
typedef struct hiFRAME_POS_S
{
	FIDX_FRAME_TYPE_E  eFrameType;
	HI_U32        u32PTS;
	HI_S64        s64GlobalOffset;
	HI_S32        s32OffsetInPacket;
	HI_S32        s32PacketCount;
	HI_S32        s32FrameSize;
	HI_U32        u32Reservd;
}FRAME_POS_S;

/* pvr index's SCD descriptor                                               */
typedef struct hiFINDEX_SCD_S
{
    HI_U8   u8IndexType;             /* type of index(pts,sc,pause,ts) */
    HI_U8   u8StartCode;             /* type of start code, 1Byte after 000001 */
    HI_U16  u16Reservd;    
    HI_U32  u32PtsMs;
	
    HI_S64  s64GlobalOffset;        /* start code offset in global buffer */
    HI_U8   au8DataAfterSC[8];      /* 1~8 Byte next to SC */
    
} FINDEX_SCD_S;

/*!***********************************************************************/
/*! interface functions definition                                       */
/*!***********************************************************************/

/*!***********************************************************************
@brief
	global init, clear residual information, and register call back.
 ************************************************************************/
HI_VOID FIDX_Init(HI_VOID (*OutputFramePosition)(HI_U32 *Param, FRAME_POS_S *pstScInfo));

/*!***********************************************************************
@brief
    open an instance
@param	
	VidStandard:  video standard type
@return
    if success return the instance ID, between 0 and (FIDX_MAX_CTX_NUM-1)
	otherwise return -1
 ************************************************************************/
HI_S32  FIDX_OpenInstance(VIDSTD_E VidStandard, STRM_TYPE_E StrmType, HI_U32 *Param);

/************************************************************************
@brief
    close specified instance
@param
	InstIdx:  the ID of the instance to be closed
 ************************************************************************/
HI_S32  FIDX_CloseInstance( HI_S32 InstIdx );


/*!***********************************************************************
@brief
    feed start code to FIDX.
    there are 2 method to feed necessary information to FIDX:
	1. feed stream directly. Call FIDX_MakeFrameIndex()
	2. feed start code. In this method, the start code must be scanned outside,
	   This call this function to create index.
 ************************************************************************/
HI_VOID  FIDX_FeedStartCode(
    HI_S32 InstIdx,                    /*! instance ID */
    const FINDEX_SCD_S *pstSC );      /* SCD descriptor */

/*!***********************************************************************
@brief
    to see if the SC is usful for index making
@return
    if the SC is usful return FIDX_OK, otherwise return FIDX_ERR
 ************************************************************************/
HI_S32  FIDX_IsSCUseful(HI_S32 InstIdx, HI_U8 u8StartCode);

#endif




