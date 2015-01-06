/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

******************************************************************************
  File Name     : mpi_priv_adec.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2006/03/10
  Description   :
  History       :
  1.Date        : 2006/03/10
    Author      : f47391
    Modification: Created file

******************************************************************************/

#ifndef __MPI_PRIV_ADEC_H__
#define __MPI_PRIV_ADEC_H__

#include "hi_type.h"
#include "hi_unf_sound.h"
#include "hi_mpi_adec.h"
#include "hi_mpi_mem.h"
#include "hi_module.h"
#ifdef __cplusplus
 #if __cplusplus
extern "C" {
 #endif
#endif /* End of #ifdef __cplusplus */
typedef enum hiADEC_ATTR
{
	ADEC_ATTR_CODECID = 0,
	ADEC_ATTR_WORKSTATE, 
	ADEC_ATTR_INBUFSIZE,
	ADEC_ATTR_OUTBUFNUM,
	ADEC_ATTR_DECOPENPARAM,
    ADEC_ATTR_EosStateFlag,
    ADEC_ATTR_ALLATTR
}hiADEC_ATTR;
#ifndef HI_ADEC_MAX_INSTANCE
#define ADEC_INSTANCE_MAXNUM 2     /* max decoder instance */
#else
#define ADEC_INSTANCE_MAXNUM HI_ADEC_MAX_INSTANCE 
#endif

#define ADEC_MAX_INPUT_BLOCK_SIZE 0x10000   /* max input data size of decoder */
#define ADEC_MAX_VOLUME 100 /* 32 */

#define ADEC_MAX_CHANNLES 8

/* Max frame number can be used in ADEC */
#define ADEC_MAX_WORK_BUFFER_NUMBER 40
#define ADEC_DEFAULT_WORKINGBUF_NUM 30

#define ADEC_MAX_INPUT_BUFFER_SIZE (16 * 1024 * 1024)
#define ADEC_MIN_INPUT_BUFFER_SIZE (8 * 1024)
#define ADEC_DEFAULT_INPUT_BUFFER_SIZE (128 * 1024)

/* global vars for pts process 							*/
#define     ADEC_MAX_STORED_PTS_NUM 2048
/* note: ADEC_MAX_STORED_PACKET_NUM is not less than  ADEC_MAX_STORED_PTS_NUM */
#define     ADEC_MAX_STORED_PACKET_NUM (ADEC_MAX_STORED_PTS_NUM)

/* max adec consume bytes for one frame */
#define ADEC_MAX_FRAME_CONSUME_THD   (1024*128)

#define CHECK_ADEC_HANDLE(hAdec) \
    do                                                  \
    {                                                   \
        if ((HI_S32)(hAdec) >= ADEC_INSTANCE_MAXNUM)                                  \
        {                                               \
            HI_ERR_ADEC("  invalid Adec handle =0x%x!\n", hAdec);          \
            return HI_FAILURE;              \
        }                                               \
    } while (0)

#define CHECK_ADEC_NULL_PTR(ptr) \
    do                                                  \
    {                                                   \
        if (NULL == ptr)                             \
        {                                               \
            HI_ERR_ADEC("invalid NULL poiner!\n");          \
            return HI_FAILURE;                  \
        }                                               \
    } while (0)

#define CHECK_NULL_PTR_NORET(ptr) \
    do                                                  \
    {                                                   \
        if (NULL == ptr)                             \
        {                                               \
            HI_ERR_ADEC("invalid NULL poiner!\n");          \
            return;                  \
        }                                               \
    } while (0)

#define CHECK_ADEC_STATE_NOMUTE(bEnable) \
    do                                                  \
    {                                                   \
        if (HI_FALSE == bEnable)                             \
        {                                               \
            HI_ERR_ADEC(" adec state invalid\n");          \
            return HI_FAILURE; \
        }                                               \
    } while (0)    

#define CHECK_ADEC_STATE(bEnable, adecMutex) \
    do                                                  \
    {                                                   \
        if (HI_FALSE == bEnable)                             \
        {                                               \
            HI_ERR_ADEC(" adec state invalid\n");          \
            ADEC_UNLOCK(adecMutex);\
            return HI_FAILURE; \
        }                                               \
    } while (0)


#define CHECK_ADEC_STATEARG2(bEnable, adecMutex1,adecMutex2) \
		do                                                  \
{                                                   \
		if (HI_FALSE == bEnable)                             \
		{                                               \
				HI_ERR_ADEC(" adec state invalid\n");          \
				ADEC_UNLOCK(adecMutex1);\
				ADEC_UNLOCK(adecMutex2);\
				return HI_FAILURE; \
		}                                               \
} while (0)



#define CHECK_ADEC_OUTBUF_NUMBER(number) \
    do                                                  \
    {                                                   \
        if (number >= ADEC_MAX_WORK_BUFFER_NUMBER)                             \
        {                                               \
            HI_ERR_ADEC("invalid output No.(%d)!\n",number);          \
            return HI_FAILURE;                  \
        }                                               \
    } while (0)

#define CHECK_ADEC_OUTBUF_NUMBER_NORET(number) \
    do                                                  \
    {                                                   \
        if (number >= ADEC_MAX_WORK_BUFFER_NUMBER)                             \
        {                                               \
            HI_ERR_ADEC("invalid output No.(%d)!\n",number);         \
            return;                  \
        }                                               \
    } while (0)

#define CHECK_ADEC_VOLUME(volume) \
    do                                                  \
    {                                                   \
        if (volume > ADEC_MAX_VOLUME)                    \
        {                                               \
            HI_WARN_AO("invalid ADEC Volume(%d)\n", volume);   \
            return ADEC_MAX_VOLUME;               \
        }                                               \
    } while (0)

#define  HI_MPI_ADEC_RetUserErr(DrvErrCode, adecMutex)  \
    do                                                  \
    {                                                   \
        if (HI_SUCCESS != DrvErrCode)                   \
        {                                               \
            HI_ERR_ADEC(" ErrCode =0x%x\n",DrvErrCode); \
            ADEC_UNLOCK(adecMutex); \
            return DrvErrCode; \
        }                                               \
    } while (0)


#define  HI_MPI_ADEC_RetUserErrARG2(DrvErrCode, adecMutex1,adecMutex2)  \
		do                                                  \
{                                                   \
		if (HI_SUCCESS != DrvErrCode)                   \
		{                                               \
				HI_ERR_ADEC(" ErrCode =0x%x\n",DrvErrCode); \
				ADEC_UNLOCK(adecMutex1); \
				ADEC_UNLOCK(adecMutex2); \
				return DrvErrCode; \
		}                                               \
} while (0)

#define  HI_MPI_ADEC_RetUserErr2(DrvErrCode, adecMutex) \
    do                                                  \
    {                                                   \
        HI_ERR_ADEC(" ErrCode =0x%x\n",DrvErrCode); \
        ADEC_UNLOCK(adecMutex); \
        return DrvErrCode; \
    } while (0)		

#define  HI_MPI_ADEC_RetUserErr2ARG2(DrvErrCode, adecMutex1,adecMutex2) \
		do                                                  \
{                                                   \
		HI_ERR_ADEC(" ErrCode =0x%x\n",DrvErrCode); \
		ADEC_UNLOCK(adecMutex1); \
		ADEC_UNLOCK(adecMutex2); \
		return DrvErrCode; \
} while (0)


typedef enum
{
    ADEC_CMD_CTRL_START = 0,
    ADEC_CMD_CTRL_STOP,
    ADEC_CMD_CTRL_BUTT
} ADEC_CMD_CTRL_E;     


typedef enum
{
    ADEC_CMD_PROC_SAVE_PCM = 0,
    ADEC_CMD_PROC_SAVE_ES,
    ADEC_CMD_PROC_BUTT
} ADEC_CMD_SAVE_E;     


/* Buffer 												*/
typedef struct hiADEC_OUTPUTBUF_S
{
    HI_BOOL  bFlag;     /* Buffer Flag 				*/
    HI_U32   u32PTS;      /* Play Time Stamp may be interpolated	*/
    HI_U32   u32OrgPTS;  /* original Play Time Stamp 			*/
    HI_U32  *OutBuf;     /* Output Buffer 			*/
    HI_S32 * ps32PcmOutBuf;
    HI_S32 * ps32BitsOutBuf;       /* NULL, if decoder dont suppourt iec 61937 */
    HI_U32   u32PcmOutSamplesPerFrame;
    HI_U32   u32BitsOutBytesPerFrame;
    HI_U32   u32OutChannels;
    HI_U32   u32OutSampleRate;
    HI_BOOL  bInterleaved;
    HI_U32   u32BitPerSample; /**< Bit per sample */
} ADEC_OUTPUTBUF_S;

typedef struct hiADEC_PTS_S
{
    HI_U32 u32PtsMs;        /* Play Time Stamp 			         */
    HI_U32 u32BegPos;       /* Stream offset address of PTS 	 */
    HI_U32 u32EndPos;    /* Stream offset end address of PTS  */
} ADEC_PTS;

typedef struct hiADEC_PACKET_S
{
    HI_BOOL bPacketEosFlag;        /* patket EOS 			         */
    HI_S32  s32BegPos;       /* Stream offset address of patket 	 */
    HI_U32  u32EndPos;       /* Stream offset end address of patket  */
} ADEC_PACKET;

typedef struct hiADEC_STREAM_BUFFER_S
{
    HI_U8   *pu8Data;               /*buffer addr*/
    HI_U32   u32BufSize;                /*buffer length*/
    HI_S32   s32BufReadPos;               /*buffer s32BufReadPos ptr*/
    HI_U32   u32BufWritePos;              /*buffer u32BufWritePos ptr*/
    HI_U32   u32BufFree;               /*buffer u32BufFree length*/

    //HI_U32   u32BufLeftBytes;               /*buffer continuous unread area length*/
    HI_U32 u32BufPaddingSize;

    //#ifdef ADEC_MMZ_BUF_SUPPORT
    HI_MMZ_BUF_S sAdecInMMzBuf;
    //#endif
    HI_U32   u32StreamWritePos;   
    HI_U32   u32Boundary;	/* pts read pointers wrap point */
} ADEC_STREAM_BUFFER_S;

typedef struct hiADEC_PROC_ITEM_S
{
    HI_BOOL        bAdecWorkEnable;
    HI_CHAR        szCodecType[32];
    HI_CHAR        szCodecDescription[32];
    HI_U32           u32CodecID;
    HI_BOOL         enFmt;        
    HI_UNF_SAMPLE_RATE_E enSampleRate;
    HI_UNF_BIT_DEPTH_E   enBitWidth;
    HI_U32          u32PcmSamplesPerFrame;
    HI_U32           u32FramnNm;
    HI_U32           u32ErrFrameNum;
    HI_U32           u32BufSize;
    HI_S32           s32BufRead;
    HI_U32           u32BufWrite;
    HI_U32           u32FrameSize;
    HI_U32           u32FrameRead;
    HI_U32           u32FrameWrite;
    HI_U32           u32PtsLost;
    HI_U32           u32Volume;
	HI_U32           u32OutChannels;
	HI_U32           u32BitsOutBytesPerFrame;
    HI_U32           u32DbgGetBufCount_Try;
    HI_U32           u32DbgGetBufCount;
    HI_U32           u32DbgPutBufCount_Try;
    HI_U32           u32DbgPutBufCount;
    HI_U32           u32DbgReceiveFrameCount_Try;
    HI_U32           u32DbgReceiveFrameCount;
    HI_U32           u32DbgSendStraemCount_Try;
    HI_U32           u32DbgSendStraemCount;
    HI_U32           u32DbgTryDecodeCount;
    HI_U32           u32FrameConsumedBytes;
    HI_U32           u32LastCorrectFrameNum;
    HI_U32           u32ThreadId;

    ADEC_CMD_CTRL_E    enPcmCtrlState;
    ADEC_CMD_CTRL_E    enEsCtrlState;
    HI_U32                  u32SavePcmCnt;
    HI_U32                  u32SaveEsCnt;
    HI_CHAR              filePath[512];   

    HI_U32           ThreadBeginTime;
    HI_U32           ThreadEndTime;
    HI_U32           ThreadScheTimeOutCnt;
    HI_U32           ThreadExeTimeOutCnt;
} ADEC_PROC_ITEM_S;

#define ADEC_LOCK_DECLARE(p_mutex) ;                 \
    static pthread_mutex_t p_mutex = PTHREAD_MUTEX_INITIALIZER;

#define ADEC_LOCK_INIT(p_mutex) ;                 \
    (void)pthread_mutex_init(p_mutex, NULL)

#define ADEC_LOCK_DESTROY(p_mutex) \
    (void)pthread_mutex_destroy(p_mutex)

#define ADEC_LOCK(p_mutex) \
    (void)pthread_mutex_lock(p_mutex)

#define ADEC_UNLOCK(p_mutex) \
    (void)pthread_mutex_unlock(p_mutex)

#define DRV_ADEC_DEVICE_NAME "hi_adec"

/* 'IOC_TYPE_ADEC' means ADEC magic macro */
#define     DRV_ADEC_PROC_INIT _IOW(HI_ID_ADEC, 0, ADEC_PROC_ITEM_S *)
#define     DRV_ADEC_PROC_EXIT _IO(HI_ID_ADEC, 1)

/*Define Debug Level For HI_ID_AO                     */
 #define HI_FATAL_ADEC(fmt...) \
    HI_FATAL_PRINT(HI_ID_ADEC, fmt)

 #define HI_ERR_ADEC(fmt...) \
    HI_ERR_PRINT(HI_ID_ADEC, fmt)

 #define HI_WARN_ADEC(fmt...) \
    HI_WARN_PRINT(HI_ID_ADEC, fmt)

 #define HI_INFO_ADEC(fmt...) \
    HI_INFO_PRINT(HI_ID_ADEC, fmt)

HI_S32	ADEC_Init(const HI_CHAR* pszCodecNameTable[]);
HI_S32	ADEC_deInit(HI_VOID);
HI_S32	ADEC_Reset(HI_HANDLE hAdec);
HI_S32	ADEC_Open(HI_HANDLE *phAdec);
HI_S32	ADEC_Close(HI_HANDLE hAdec);
HI_S32 ADEC_GetAttr(HI_HANDLE hAdec, HI_U32 u32Command, void  *pstAttr);
HI_S32	ADEC_GetBuffer (HI_HANDLE hAdec, HI_U32 u32RequestSize, HI_UNF_STREAM_BUF_S *pstStream1,
                                HI_UNF_STREAM_BUF_S *pstStream2);
HI_S32	ADEC_GetBufferStatus (HI_HANDLE hAdec, ADEC_BUFSTATUS_S *ptsBufStatus);
HI_S32	ADEC_GetDebugInfo(HI_HANDLE hAdec, ADEC_DEBUGINFO_S *pstDebuginfo);
HI_S32	ADEC_GetStatusInfo(HI_HANDLE hAdec, ADEC_STATUSINFO_S *pstStatusinfo);
HI_S32	ADEC_GetStreamInfo(HI_HANDLE hAdec, ADEC_STREAMINFO_S * pstStreaminfo);
HI_S32  ADEC_GetHaSzNameInfo(HI_HANDLE hAdec,ADEC_SzNameINFO_S *pHaSznameInfo);

HI_S32 ADEC_GetAnalysisPcmData(HI_HANDLE hAdec);
HI_S32 ADEC_GetAudSpectrum(HI_U16 *pSpectrum ,HI_U32 u32BandNum);



HI_S32	ADEC_PutBuffer (HI_HANDLE hAdec, const HI_UNF_STREAM_BUF_S *pstStream1, const HI_UNF_STREAM_BUF_S *pstStream2,
                                HI_U32 u32PtsMs);
HI_S32 ADEC_ReceiveFrame (HI_HANDLE hAdec, HI_UNF_AO_FRAMEINFO_S *pstAOFrame, ADEC_EXTFRAMEINFO_S *pstExtInfo);
HI_S32 ADEC_TryReceiveFrame (HI_HANDLE hAdec, HI_UNF_AO_FRAMEINFO_S *pstAOFrame, ADEC_EXTFRAMEINFO_S *pstExtInfo);
HI_S32	ADEC_RegisterDeoder(const char *pszDecoderDllName);
HI_S32	ADEC_ShowRegisterDeoder(HI_VOID);
HI_S32 ADEC_FoundSupportDeoder(HA_FORMAT_E enFormat,HI_U32 *penDstCodecID);

HI_S32	ADEC_ReleaseFrame(HI_HANDLE hAdec, const HI_UNF_AO_FRAMEINFO_S *pstAOFrame);
HI_S32	ADEC_SendStream (HI_HANDLE hAdec, const HI_UNF_STREAM_BUF_S *pstStream, HI_U32 u32PtsMs);
HI_S32 ADEC_SetAttr(HI_HANDLE hAdec,HI_U32 command  ,void  *pstParam);
HI_S32	ADEC_SetConfigDeoder(HI_HANDLE hAdec, HI_VOID *pstConfigStructure);

HI_VOID	ADEC_DbgCountTryGetBuffer(HI_HANDLE hAdec);
HI_VOID	ADEC_DbgCountGetBuffer (HI_HANDLE hAdec);
HI_VOID	ADEC_DbgCountTryReceiveFrame (HI_HANDLE hAdec);
HI_VOID	ADEC_DbgCountReceiveFrame (HI_HANDLE hAdec);
HI_VOID	ADEC_DbgCountTrySendStream (HI_HANDLE hAdec);
HI_VOID	ADEC_DbgCountSendStream (HI_HANDLE hAdec);
HI_VOID	ADEC_DbgCountTryPutBuffer(HI_HANDLE hAdec);
HI_VOID	ADEC_DbgCountPutBuffer(HI_HANDLE hAdec);
HI_S32	ADEC_SetEosFlag(HI_HANDLE hAdec);
HI_S32   ADEC_SetCodecCmd(HI_HANDLE hAdec, HI_VOID *pstCodecCmd);
HI_S32   ADEC_GetDelayMs(HI_HANDLE hAdec,HI_U32 *pDelayMs);


#ifdef __cplusplus
 #if __cplusplus
}
 #endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __MPI_PRIV_ADEC_H__ */
