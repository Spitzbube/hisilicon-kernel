#ifndef __DRV_AVPLAY_IOCTL_H__
#define __DRV_AVPLAY_IOCTL_H__

#include "hi_type.h"
#include "hi_unf_avplay.h"
#include "hi_drv_adec.h"
#include "hi_mpi_mem.h"
#include "hi_mpi_stat.h"
#include "hi_mpi_vdec.h"
#include "hi_mpi_adec.h"
#include "hi_mpi_demux.h"
#include "hi_mpi_sync.h"
#include "hi_mpi_win.h"
#include "hi_mpi_ao.h"
#include "hi_mpi_demux.h"
#include "hi_drv_avplay.h"

#ifdef __cplusplus
#if __cplusplus
    extern "C"{
#endif
#endif

//#define  AVPLAY_VID_THREAD

#define  AVPLAY_MAX_NUM                 16
#define  AVPLAY_MAX_WIN                 6
#define  AVPLAY_MAX_DMX_AUD_CHAN_NUM    32
#define  AVPLAY_MAX_TRACK               6

#define  AVPLAY_MAX_PORT_NUM            3    //The max num of port
#define  AVPLAY_MAX_SLAVE_FRMCHAN       2    //The max num of slave port
#define  AVPLAY_MAX_VIR_FRMCHAN         2    //The max num of virtual port

#define    AVPLAY_DFT_VID_SIZE       (5*1024*1024)
#define    AVPLAY_MIN_VID_SIZE       (512*1024)
#define    AVPLAY_MAX_VID_SIZE       (32*1024*1024)

#define    AVPLAY_TS_DFT_AUD_SIZE    (384*1024)
#define    AVPLAY_ES_DFT_AUD_SIZE    (256*1024)
#define    AVPLAY_MIN_AUD_SIZE       (192*1024)
#define    AVPLAY_MAX_AUD_SIZE       (4*1024*1024)

#define    AVPLAY_ADEC_FRAME_NUM     (8)

#define    AVPLAY_SYS_SLEEP_TIME     (10)

#define    APPLAY_EOS_BUF_MIN_LEN    (1024)
#define    AVPLAY_EOS_TIMEOUT        (200)
#define    APPLAY_EOS_STREAM_THRESHOLD (2)


/* video buffer dither waterline */
/* CNcomment: 视频缓冲管理抖动水线的百分比，0-99 */
#define    AVPLAY_ES_VID_FULL_PERCENT    85
#define    AVPLAY_ES_VID_HIGH_PERCENT    70 
#define    AVPLAY_ES_VID_LOW_PERCENT     30 
#define    AVPLAY_ES_VID_EMPTY_PERCENT   10

/* audio buffer dither waterline */
/* CNcomment: 音频缓冲管理抖动水线的百分比，0-99 */
#define    AVPLAY_ES_AUD_FULL_PERCENT    98
#define    AVPLAY_ES_AUD_HIGH_PERCENT    85
#define    AVPLAY_ES_AUD_LOW_PERCENT     5
#define    AVPLAY_ES_AUD_EMPTY_PERCENT   2

/* max delay time of adec in buffer */
#define    AVPLAY_ADEC_MAX_DELAY        1200

#define    AVPLAY_THREAD_TIMEOUT        30

typedef    HI_S32 (*AVPLAY_EVT_CB_FN)(HI_HANDLE hAvplay, HI_UNF_AVPLAY_EVENT_E EvtMsg, HI_U32 EvtPara);

typedef enum hiAVPLAY_PROC_ID_E
{
    AVPLAY_PROC_ADEC_AO,
    AVPLAY_PROC_DMX_ADEC,
    AVPLAY_PROC_VDEC_VO,
    AVPLAY_PROC_BUTT
}AVPLAY_PROC_ID_E;

typedef struct tagAVPLAY_VID_PORT_AND_WIN_S
{
    HI_HANDLE   hWindow;
    HI_HANDLE   hPort;
}AVPLAY_VID_PORT_AND_WIN_S;

typedef struct tagAVPLAY_FRC_CTRL_S
{
	/* frame rate conversion state for progressive frame : <0-drop; ==0-natrual play; >0-repeat time */
	HI_S32 s32FrmState;  
}AVPLAY_FRC_CTRL_S;

typedef struct tagAVPLAY_FRC_CFG_S
{
	HI_U32  u32InRate;     /* unit: frame */
	HI_U32  u32OutRate;  /* fresh rate */
	HI_U32  u32PlayRate;  /* play rate*/
}AVPLAY_FRC_CFG_S;

typedef struct tagAVPLAY_ALG_FRC_S
{
	HI_U32  u32InRate;     /* unit: frame */
	HI_U32  u32OutRate;  /* fresh rate */
	HI_U32  u32PlayRate;  /* play rate*/
    HI_U32  u32CurID;     /* current insert or drop position in a FRC cycle*/
	HI_U32  u32InputCount; /* input counter */
}AVPLAY_ALG_FRC_S;

typedef struct hiAVPLAY_VIDFRM_STAT_S
{
    HI_U32      SendNum;
    HI_U32      PlayNum;
    HI_U32      RepeatNum;
    HI_U32      DiscardNum;
}AVPLAY_VIDFRM_STAT_S;

typedef struct hiAVPLAY_DEBUG_INFO_S
{
    HI_U32                     AcquireAudEsNum;
    HI_U32                     AcquiredAudEsNum;
    HI_U32                     SendAudEsNum;
    HI_U32                     SendedAudEsNum;

    HI_U32                     AcquireAudFrameNum;
    HI_U32                     AcquiredAudFrameNum;
    HI_U32                     SendAudFrameNum;
    HI_U32                     SendedAudFrameNum;

    HI_U32                     AcquireVidFrameNum;
    HI_U32                     AcquiredVidFrameNum;
    AVPLAY_VIDFRM_STAT_S       MasterVidStat;
    AVPLAY_VIDFRM_STAT_S       SlaveVidStat[AVPLAY_MAX_SLAVE_FRMCHAN];
    AVPLAY_VIDFRM_STAT_S       VirVidStat[AVPLAY_MAX_VIR_FRMCHAN];

    HI_U32                     VidOverflowNum;
    HI_U32                     AudOverflowNum;

    HI_U32                     ThreadBeginTime;
    HI_U32                     ThreadEndTime;
    HI_U32                     ThreadScheTimeOutCnt;
    HI_U32                     ThreadExeTimeOutCnt;

}AVPLAY_DEBUG_INFO_S;

typedef enum hiTHREAD_PRIO_E
{
    THREAD_PRIO_REALTIME,    /*Realtime thread, only 1 permitted*/
    THREAD_PRIO_HIGH,
    THREAD_PRIO_MID,
    THREAD_PRIO_LOW,
    THREAD_PRIO_BUTT
}THREAD_PRIO_E;

#pragma pack(4)
typedef struct hiAVPLAY_S
{
    HI_UNF_AVPLAY_ATTR_S            AvplayAttr;
    HI_UNF_VCODEC_ATTR_S            VdecAttr;
    HI_UNF_AVPLAY_LOW_DELAY_ATTR_S  LowDelayAttr;
    HI_U32                          AdecType;
    
    HI_HANDLE                       hAvplay;
    HI_HANDLE                       hVdec;
    HI_HANDLE                       hAdec;
    HI_HANDLE                       hDmxVid;
    HI_HANDLE                       hDmxAud[AVPLAY_MAX_DMX_AUD_CHAN_NUM];
    HI_HANDLE                       hDmxPcr;
    HI_HANDLE                       hSync;
    
    HI_U32                          DmxVidPid;
    HI_U32                          DmxPcrPid; 
    HI_U32                          DmxAudPid[AVPLAY_MAX_DMX_AUD_CHAN_NUM]; 

    /*multi audio demux channel*/
    HI_U32                          CurDmxAudChn;
    HI_U32                          DmxAudChnNum;
    HI_UNF_ACODEC_ATTR_S            *pstAcodecAttr;
           
    /*multi video frame channel*/
    AVPLAY_VID_PORT_AND_WIN_S       MasterFrmChn;
    AVPLAY_VID_PORT_AND_WIN_S       SlaveFrmChn[AVPLAY_MAX_SLAVE_FRMCHAN];
    HI_U32                          SlaveChnNum;
    AVPLAY_VID_PORT_AND_WIN_S       VirFrmChn[AVPLAY_MAX_VIR_FRMCHAN];
    HI_U32                          VirChnNum;

    /*multi audio track channel*/
    HI_HANDLE                       hSyncTrack;
    HI_HANDLE                       hTrack[AVPLAY_MAX_TRACK];
    HI_U32                          TrackNum;

    /*frc parameters*/
    HI_BOOL                         bFrcEnable;
    AVPLAY_FRC_CFG_S                FrcParamCfg;        /* config frc param */ /*CNcomment: 配置的frc参数 */
    AVPLAY_ALG_FRC_S                FrcCalAlg;          /* frc used rate info */ /*CNcomment: frc正在使用的帧率信息 */
    AVPLAY_FRC_CTRL_S               FrcCtrlInfo;        /* frc control */ /*CNcomment: frc控制信息 */
    HI_U32                          FrcNeedPlayCnt;     /* this frame need to play time*/ /*CNcomment:该帧需要播几次 */
    HI_U32                          FrcCurPlayCnt;      /* this frame had played time*/   /*CNcomment:该帧实际播到第几次*/

    /*flush stream control*/
    HI_BOOL                         bSetEosFlag;
    HI_BOOL                         bSetAudEos;
    HI_BOOL                         bSetEosBeginTime;
    HI_U32                          u32EosBeginTime;

    /*ddp test*/
    HI_BOOL                         AudDDPMode;
    HI_U32                          LastAudPts;

    AVPLAY_EVT_CB_FN                EvtCbFunc[HI_UNF_AVPLAY_EVENT_BUTT]; 

    /*play control parameters*/
    HI_BOOL                         bSendedFrmToVirWin;          /*whether this frame has send to virtual window*/   
    HI_BOOL                         VidEnable;
    HI_BOOL                         AudEnable;
    HI_UNF_AVPLAY_STATUS_E          LstStatus;                   /* last avplay status */
    HI_UNF_AVPLAY_STATUS_E          CurStatus;                   /* current avplay status */
    HI_UNF_AVPLAY_OVERFLOW_E        OverflowProc;
    HI_BOOL                         AvplayProcContinue;          /*flag for thread continue*/
    HI_BOOL                         AvplayVidProcContinue;       /*flag for video thread continue*/
	
    HI_BOOL                         AvplayProcDataFlag[AVPLAY_PROC_BUTT];

    HI_UNF_STREAM_BUF_S             AvplayAudEsBuf;      /*adec buffer in es mode*/
    HI_UNF_ES_BUF_S                 AvplayDmxEsBuf;      /*audio denux buffer in ts mode*/
    HI_UNF_AO_FRAMEINFO_S           AvplayAudFrm;        /*audio frames get form adec*/
    SYNC_AUD_INFO_S                 AudInfo;
    SYNC_AUD_OPT_S                  AudOpt;

    VDEC_ES_BUF_S                   AvplayVidEsBuf;      /*vdec buffer in es mode*/
    HI_DRV_VIDEO_FRAME_PACKAGE_S    CurFrmPack;
    HI_DRV_VIDEO_FRAME_PACKAGE_S    LstFrmPack;
    SYNC_VID_INFO_S                 VidInfo;
    SYNC_VID_OPT_S                  VidOpt;

    HI_DRV_VDEC_FRAME_S             stIFrame;

    HI_BOOL                         bStepMode;
    HI_BOOL                         bStepPlay;

    AVPLAY_DEBUG_INFO_S             DebugInfo;
    
    HI_U32                          PreAudEsBuf;         /*audio es buffer size when EOS happens*/
    HI_U32                          PreVidEsBuf;         /*video es buffer size when EOS happens*/
    HI_U32                          PreSystime;          /*system time when EOS happens*/
    HI_U32                          PreVidEsBufWPtr;     /*position of the video es buffer write pointer*/
    HI_U32                          PreAudEsBufWPtr;     /*position of the audio es buffer write pointer*/
    HI_U32                          PreTscnt;            /*ts count when EOS happens*/
    HI_BOOL                         CurBufferEmptyState; /*current buffer state is empty or not*/

    HI_UNF_AVPLAY_BUF_STATE_E       PreVidBufState;     /*the status of video es buffer when CheckBuf*/
    HI_UNF_AVPLAY_BUF_STATE_E       PreAudBufState;     /*the status of audio es buffer when CheckBuf*/
    HI_BOOL                         VidDiscard;

    HI_U32                          EosStartTime;        /*EOS start time*/
    HI_U32                          EosDurationTime;     /*EOS duration time*/
    HI_U32                          AvgStrmBitrate;      /*Average stream bitrate */

    HI_BOOL                         bStandBy;            /*is standby or not*/

    HI_U32                          AdecDelayMs;            /*How many mseconds in ADEC buffer*/
    ADEC_SzNameINFO_S               AdecNameInfo;

    HI_U32                          u32DispOptimizeFlag;    /*this is for pvr smooth tplay*/
    
    HI_U32                          ThreadID;
    HI_BOOL                         AvplayThreadRun;
    THREAD_PRIO_E                   AvplayThreadPrio;    /*the priority level of avplay thread*/

#ifndef __KERNEL__
    pthread_t                       AvplayDataThdInst;  /* run handle of avplay thread */  
    pthread_t                       AvplayVidDataThdInst;  /* run handle of avplay thread */ 
    pthread_attr_t                  AvplayThreadAttr;   /*attribute of avplay thread*/
    pthread_mutex_t                 *pAvplayThreadMutex;     /*mutex for data safety use*/
    pthread_mutex_t                 *pAvplayVidThreadMutex;     /*mutex for data safety use*/
    pthread_mutex_t                 *pAvplayMutex;            /* mutex for interface safety use */

    pthread_t                       AvplayStatThdInst;    /* run handle of avplay thread */  
#endif
}AVPLAY_S;
#pragma pack()

typedef struct hiAVPLAY_CREATE_S
{
    HI_U32     AvplayId;
    HI_U32     AvplayPhyAddr;
    HI_UNF_AVPLAY_STREAM_TYPE_E    AvplayStreamtype;
}AVPLAY_CREATE_S;

typedef struct hiAVPLAY_USR_ADDR_S
{
    HI_U32     AvplayId;
    HI_U32     AvplayUsrAddr;    /* AVPLAY address in user model */ 
}AVPLAY_USR_ADDR_S;

typedef struct hiAVPLAY_INFO_S
{
    AVPLAY_S   *pAvplay;         /* AVPLAY pointer in kernel model */
    HI_U32     AvplayPhyAddr;    /* AVPLAY physical address */ 
    HI_U32     File;             /*avplay file handle*//* CNcomment: AVPLAY所在进程句柄 */
    HI_U32     AvplayUsrAddr;    /* AVPLAY address in user model */ 
}AVPLAY_INFO_S;

typedef struct hiAVPLAY_GLOBAL_STATE_S
{
    AVPLAY_INFO_S  AvplayInfo[AVPLAY_MAX_NUM];
    HI_U32         AvplayCount;
}AVPLAY_GLOBAL_STATE_S;


typedef enum hiIOC_AVPLAY_E
{
    IOC_AVPLAY_GET_NUM = 0,

    IOC_AVPLAY_CREATE,
    IOC_AVPLAY_DESTROY,

    IOC_AVPLAY_CHECK_ID,
    IOC_AVPLAY_SET_USRADDR,
    IOC_AVPLAY_CHECK_NUM,

    IOC_AVPLAY_SET_BUTT
}IOC_AVPLAY_E;


#define CMD_AVPLAY_CREATE            _IOWR(HI_ID_AVPLAY, IOC_AVPLAY_CREATE, AVPLAY_CREATE_S)
#define CMD_AVPLAY_DESTROY           _IOW(HI_ID_AVPLAY, IOC_AVPLAY_DESTROY, HI_U32)

#define CMD_AVPLAY_CHECK_ID       _IOWR(HI_ID_AVPLAY, IOC_AVPLAY_CHECK_ID, AVPLAY_USR_ADDR_S)
#define CMD_AVPLAY_SET_USRADDR    _IOW(HI_ID_AVPLAY, IOC_AVPLAY_SET_USRADDR, AVPLAY_USR_ADDR_S)
#define CMD_AVPLAY_CHECK_NUM      _IOWR(HI_ID_AVPLAY, IOC_AVPLAY_CHECK_NUM, HI_U32)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif


