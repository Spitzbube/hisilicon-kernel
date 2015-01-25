/*******************************************************************************
*             Copyright 2004 - 2050, Hisilicon Tech. Co., Ltd.
*                           ALL RIGHTS RESERVED
* FileName: drv_demux_define.h
* Description: Register define for HiPVRV200
*
* History:
* Version   Date              Author    DefectNum       Description
* main\1    2009-09-27    y00106256     NULL            create this file.
*******************************************************************************/
#ifndef __DRV_DEMUX_DEFINE_H__
#define __DRV_DEMUX_DEFINE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#include <linux/list.h>
#include <linux/wait.h>

#include "hi_type.h"

#include "drv_demux_config.h"
#include "drv_demux_ext.h"

#include "hi_drv_mmz.h"
#include "hi_drv_proc.h"

#include "hi_unf_common.h"
#include "hi_unf_demux.h"
#include "hi_mpi_demux.h"

#define DMX_FQ_COM_BLKSIZE              (4 * 1024)
#define DMX_PES_MAX_SIZE                ((64 * 1024) / DMX_FQ_COM_BLKSIZE)

#define DMX_MAX_FILTER_NUM_PER_CHANNEL  32      //the FILTER number in per channel

#define DMX_REGION_FILTER_COUNT         32
#define DMX_REGION_CHANNEL_COUNT        32

#define DMX_PER_DESC_LEN                0x10
#define DMX_MAX_ERRLIST_NUM             0x100

#define DMX_CHANNEL_REGION_NUM          3
#define DMX_CHANNEL_NUM_PER_REGION      32

#define DMX_FQ_REGION_NUM               2
#define DMX_FQ_NUM_PER_REGION           32

#define DMX_OQ_REGION_NUM               4
#define DMX_OQ_NUM_PER_REGION           32

#define DMX_OQ_RSV_OFFSET   0
#define DMX_OQ_CTRL_OFFSET  1
#define DMX_OQ_EOPWR_OFFSET 2
#define DMX_OQ_SZUS_OFFSET  3
#define DMX_OQ_SADDR_OFFSET 4
#define DMX_OQ_RDWR_OFFSET  5
#define DMX_OQ_CFG_OFFSET   6
#define DMX_OQ_START_OFFSET 7

#define DMX_FQ_CTRL_OFFSET  0
#define DMX_FQ_RDVD_OFFSET  1
#define DMX_FQ_SZWR_OFFSET  2
#define DMX_FQ_START_OFFSET 3
#define DMX_FQ_ALOVF_CNT    4
#define DMX_FQ_COMMOM       0x0
#define DMX_FQ_AV_BASE      8
#define DMX_FQ_REC_BASE     2
#define DMX_FQ_SCD_BASE     5
#define DMX_OQ_DEPTH        0x3ff
#define DMX_OQ_ALOVF_CNT    0x4//almost overflow level
#define DMX_OQ_OUTINT_CNT   0x1//OQ queue continues output buf block to this number will cuase break
#define DMX_OQ_START_CNT    0x30//OQ queue continues output minimal free block count
#define DMX_OQ_STOP_PERSENT  80 //channel pid disable persent,if oq use dscs more the 80% driver will stop it
#define DMX_OQ_START_PERSENT 60//channel pid enable persent,if oq use dscs less the 60% driver will restart it
#define DMX_OQ_SOFT_ALOVF   (DMX_OQ_ALOVF_CNT*2)//software almost overflow level
#define DMX_MAX_AVFQ_DESC   0x3f0
#define DMX_MAX_BLOCK_SIZE  0xffff


#define DMX_SHIFT_16BIT     16
#define DMX_SHIFT_8BIT      8
#define DMX_BUF_INTERVAL    0x0

#define DMX_INVALID_PORT_ID             0xffffffff
#define DMX_INVALID_DEMUX_ID            0xffffffff
#define DMX_INVALID_SYNC_HANDLE         0xffffffff
#define DMX_INVALID_CHAN_ID             0xffff
#define DMX_INVALID_FILTER_ID           0xffff
#define DMX_INVALID_KEY_ID              0xffff
#define DMX_INVALID_PID                 0x1fff
#define DMX_INVALID_BUF_ID              0xffff
#define DMX_INVALID_FQ_ID               0xffff

#define DMX_MIN_TS_BUFFER_SIZE          0x1000
#define DMX_MAX_TS_BUFFER_SIZE          0x1000000
#define DMX_TS_BUFFER_GAP               0x100

#define DMX_DISABLE                     0x0
#define DMX_ENABLE                      0x1

#define DMX_MAX_SEC_LEN                 0x1000

#define SECTION_LENGTH_FIELD_SIZE       2

#define DMX_MAX_IP_DESC_DEPTH           0xffff
#define DMX_MIN_IP_DESC_DEPTH           0x3ff
#define DMX_MAX_IP_BLOCK_SIZE           0xffff

#define DMX_MAX_LOST_TH                 0x3
#define DMX_MAX_LOCK_TH                 0x7
#define DMX_MAX_FLUSH_WAIT              0x1000

#define DMX_RAM_PORT_MIN_LEN            188
#define DMX_RAM_PORT_MAX_LEN            255

#define DMX_TS_PACKET_LEN               188
#define DMX_TS_PACKET_LEN_204           204
#define DMX_SCD_PACKET_LEN              28
#define DMX_TOTAL_SCD_FLTNUM            10
#define DMX_TOTAL_RANGE_FLTNUM          7
#define DMX_TOTAL_RANGE_FLTNUM_V200     16

#define DMX_PORT_OFFSET                 1
#define DMX_DEFAULT_INT_CNT             1

#define DMX_PES_HEADER_LENGTH           9
#define DMX_PES_HEADER_AND_PTS_LENGTH   14

#define DMX_DEFAULT_POST_TH             0

/* speed is 15 ,it means the Max speed of a IP port equal :
100 Mbps@MV300
126 Mbps@CV200
We consider this speed is enough for IP port,
user can change this value small to get more speed of IP port,
but that may cause the total speed of demux be stress
*/
#define DMX_DEFAULT_IP_SPEED              15


#define DMX_KEY_HARDONLY_FLAG           0xffffffff

#define DMX_CHECKCHN_TIMEOUT        1000    /* 1s */
#define DMX_RESETCHN_TIME1          2000    /* 2s */
#define DMX_RESETCHN_TIME2          5000    /* 2s */
#define DMX_CHECK_START_PERSENT     80      /* 80% */
#define DMX_CHN_RESET_PERSENT       20      /* 20% */

#define INVALID_PTS                 0xFFFFFFFFL

#define DMX_CSA2_TABLE_ID           0
#define DMX_SPE_TABLE_ID            1
#define DMX_OTHER_TABLE_ID          2

#define DMX_KEY_MIN_LEN             8
#define DMX_KEY_MAX_LEN             16

#define CHECKPOINTER(ptr)                                   \
    do                                                      \
    {                                                       \
        if (!(ptr))                                         \
        {                                                   \
            HI_ERR_DEMUX("pointer is null\n");             \
            return HI_ERR_DMX_NULL_PTR;                     \
        }                                                   \
    } while (0)

#define CHECKDMXID(DmxId)                                   \
    do                                                      \
    {                                                       \
        if ((DmxId) >= DMX_CNT)                             \
        {                                                   \
            HI_ERR_DEMUX("invalid demux %d\n", DmxId);     \
            return HI_ERR_DMX_INVALID_PARA;                 \
        }                                                   \
    } while (0)

#define CHECKTUNERPORTID(Id)                                \
    do                                                      \
    {                                                       \
        if ((Id) >= DMX_TUNERPORT_CNT)                      \
        {                                                   \
            HI_ERR_DEMUX("invalid tuner port %u\n", Id);   \
            return HI_ERR_DMX_INVALID_PARA;                 \
        }                                                   \
    } while (0)

#define CHECKRAMPORTID(Id)                                  \
    do                                                      \
    {                                                       \
        if ((Id) >= DMX_RAMPORT_CNT)                        \
        {                                                   \
            HI_ERR_DEMUX("invalid ram port %u\n", Id);     \
            return HI_ERR_DMX_INVALID_PARA;                 \
        }                                                   \
    } while (0)

#define DMXINC(a, size) \
    if ((++a) >= (size)) { \
        (a) = 0;\
    } \

typedef enum hiDMX_Ch_AFMode_E
{
    DMX_AF_SEND                   = 0,                         /*send AF data*/
    DMX_AF_DISCARD                = 1,                         /*discard AF data*/
    DMX_UNMEANING_AF_DISCARD      = 2                          /*send meaningful AF data only*/
}DMX_Ch_AFMode_E;

typedef enum hiDMX_Ch_ATTR_E
{
    DMX_CH_GENERAL = 0,
    DMX_CH_GENERAL_NOPL,
    DMX_CH_PES_NOCHECK = 2,
    DMX_CH_PES = 3,
    DMX_CH_AUDIO = 6,
    DMX_CH_VIDEO,
    DMX_CH_UNDEF
}DMX_Ch_ATTR_E;

typedef enum
{
    DMX_FLUSH_TYPE_REC_PLAY = 0x0,
    DMX_FLUSH_TYPE_PLAY     = 0x1,
    DMX_FLUSH_TYPE_REC      = 0x2,
    DMX_FLUSH_TYPE_UNDEF    = 0x3
} DMX_FLUSH_TYPE_E;

typedef enum
{
    DMX_OQ_CLEAR_TYPE_PLAY  = 0,
    DMX_OQ_CLEAR_TYPE_REC   = 1,
    DMX_OQ_CLEAR_TYPE_SCD   = 2,
    DMX_OQ_CLEAR_TYPE_UNDEF
} DMX_OQ_CLEAR_TYPE_E;

typedef enum
{
    DMX_REC_STATUS_STOP,
    DMX_REC_STATUS_START
} DMX_REC_STATUS_E;

typedef enum
{
    DMX_OQ_MODE_UNUSED,
    DMX_OQ_MODE_PLAY,
    DMX_OQ_MODE_REC,
    DMX_OQ_MODE_SCD,
    DMX_OQ_MODE_UNDEF
} DMX_OQ_MODE_E;

/****************************************************************************/

typedef struct
{
    HI_UNF_DMX_PORT_TYPE_E  PortType;
    HI_U32                  SyncLockTh;
    HI_U32                  SyncLostTh;

    /*
    ** whether Tuner input clock inverting or not.
    ** 0: in-phase(default)
    ** 1: inverting
    */
    HI_U32  TunerInClk;
    /* port-line sequence select:
    ** parallel:
    **    0: mean cdata[7] is the significant bit(default)
    **    1: mean cdata[0] is the significant bit
    ** serial:
    **    0: mean cdata[0] is the data line (default)
    **    1: mean cdata[7] is the data line
    */
    HI_U32  BitSelector;
} DMX_TunerPort_Info_S;

typedef struct hiDMX_UserMsg_S
{
    HI_U32                  u32MsgLen;
    HI_U32                  u32BufStartAddr;
    HI_UNF_DMX_DATA_TYPE_E  enDataType;  /**< the data packet type */
} DMX_UserMsg_S;

typedef struct
{
    HI_U32 u32UsedFlag;
    DMX_UserMsg_S psMsg;
} DMX_ERRMSG_S;

typedef struct
{
#ifdef DMX_REGION_SUPPORT
    HI_U32      DmxId;
#endif
    HI_U32      ChanId;
    HI_U32      FilterId;
    HI_U32      Depth;
    HI_U8       Match[DMX_FILTER_MAX_DEPTH];
    HI_U8       Mask[DMX_FILTER_MAX_DEPTH];
    HI_U8       Negate[DMX_FILTER_MAX_DEPTH];
} DMX_FilterInfo_S;

typedef struct
{
    HI_U32  CaType;
    HI_U32  CaEntropy;
    HI_U32  DescType;
    HI_U32  KeyLen;

#ifdef HI_DEMUX_PROC_SUPPORT
    HI_U32  ChanCount;
    HI_U32  EvenKey[DMX_KEY_MAX_LEN / sizeof(HI_U32)];
    HI_U32  OddKey[DMX_KEY_MAX_LEN / sizeof(HI_U32)];
#endif
} DMX_KeyInfo_S;

typedef struct
{
    DMX_OQ_MODE_E enOQBufMode;
    HI_U32          u32OQId;
    HI_U32          u32AttachId;
    HI_U32          u32OQVirAddr;
    HI_U32          u32OQPhyAddr;
    HI_U32          u32OQDepth;
    HI_U32          u32FQId;
    HI_U32          u32ProcsBlk;
    HI_U32          u32ProcsOffset;
    HI_U32          u32ReleaseBlk;
    HI_U32          u32ReleaseOffset;
    HI_U32              OqWakeUp;
    wait_queue_head_t   OqWaitQueue;
    wait_queue_head_t *pWatchWaitQueue;
} DMX_OQ_Info_S;

/*
Passing decode parameters
*/
typedef struct hi_Disp_Control_t
{
    HI_U32          u32DispTime;
    HI_U32          u32DispEnableFlag;       
    HI_U32          u32DispFrameDistance;   
    HI_U32          u32DistanceBeforeFirstFrame;
    HI_U32          u32GopNum;
} Disp_Control_t;

typedef struct
{
    HI_U32                          DmxId;
    HI_U32                          ChanId;
    HI_U32                          ChanPid;
    HI_U32                          FilterCount;
    HI_U32                          ChanBufSize;
    HI_UNF_DMX_CHAN_TYPE_E          ChanType;
    HI_UNF_DMX_CHAN_CRC_MODE_E      ChanCrcMode;
    HI_UNF_DMX_CHAN_OUTPUT_MODE_E   ChanOutMode;
    HI_UNF_DMX_CHAN_STATUS_E        ChanStatus;
    HI_U32                          KeyId;
    HI_U32                          ChanOqId;
    HI_U32                          u32TotolAcq;
    HI_U32                          u32HitAcq;
    HI_U32                          u32Release;
    HI_U32                          u32PesBlkCnt;
    HI_U32                          u32PesLength;
    HI_U32                          u32ProcsOffset;         /* for pes */
    HI_U32                          LastPts;                /* the last parsed PTS*/
    HI_U32                          u32AcqTime;             /* the last get time */
    HI_U32                          u32AcqTimeInterval;     /* the interval from this time to previous time, in millisecond */
    HI_U32                          u32RelTime;             /* the last release time */
    HI_U32                          u32RelTimeInterval;     /* the interval from this time to previous time in milliseond */
    HI_U32                          u32ChnResetLock;
#ifdef DMX_USE_ECM
    HI_U32                          u32SwFlag;
#endif
    HI_BOOL                         ChanEosFlag;
    Disp_Control_t                  stLastControl;
} DMX_ChanInfo_S;

typedef struct
{
    DMX_PORT_MODE_E             PortMode;
    HI_U32                      PortId;

#ifdef DMX_REGION_SUPPORT
    HI_U32                      DmxChanCount;
    HI_U32                      DmxFilterCount;
#endif
} DMX_Sub_DevInfo_S;

typedef struct
{
    HI_UNF_DMX_PORT_TYPE_E  PortType;
    HI_U32                  SyncLockTh;
    HI_U32                  SyncLostTh;
    HI_U32                  MinLen;
    HI_U32                  MaxLen;

    HI_U32                  DescPhyAddr;
    HI_U32                  DescKerAddr;
    HI_U32                  DescDepth;
    HI_U32                  DescWrite;
    HI_U32                 *DescCurrAddr;

    HI_U32                  PhyAddr;
    HI_U32                  KerAddr;
    HI_U32                  BufSize;
    HI_U32                  Read;
    HI_U32                  Write;

    HI_U32                  ReqAddr;    // physical address
    HI_U32                  ReqLen;

    HI_U32                  WaitLen;
    HI_BOOL                 WakeUp;
    wait_queue_head_t       WaitQueue;

    spinlock_t              LockRamPort;

    HI_U32                  GetCount;
    HI_U32                  GetValidCount;
    HI_U32                  PutCount;
} DMX_RamPort_Info_S;


typedef struct
{
    HI_U32              u32IsUsed;
    HI_U32              u32BufVirAddr;
    HI_U32              u32BufPhyAddr;
    HI_U32              u32BufSize;
    HI_U32              u32BlockSize;
    HI_U32              u32FQVirAddr;
    HI_U32              u32FQPhyAddr;
    HI_U32              u32FQDepth;
#ifdef HI_DEMUX_PROC_SUPPORT
    HI_U32              FqOverflowCount;
#endif
    struct semaphore    LockFq;
} DMX_FQ_Info_S;


typedef struct
{
    HI_U32  DmxId;
    HI_U32  PcrPid;
    HI_U32  SyncHandle;
    HI_U64  PcrValue;
    HI_U64  ScrValue;
} DMX_PCR_Info_S;

/**recorded Ts time stamp mode*/
/**CNcomment: ¼��TS��ʱ������ģʽ*/
typedef enum hiDMX_REC_TIMESTAMP_MODE_E
{
    DMX_REC_TIMESTAMP_NONE,               /**<No time stamp added before each recoreded  ts packet*/  /**<CNcomment: ����ÿ��¼�Ƶ�TS ��ǰ��ʱ��� */
	DMX_REC_TIMESTAMP_ZERO, 				 /**<Use 4 byte 0  added before each recoreded  ts packet*/  /**<CNcomment: ��ÿ��¼�Ƶ�TS ��ǰ��4�ֽ�ʱ���������Ϊ0 */
    DMX_REC_TIMESTAMP_HIGH32BIT_SCR,      /**<Use high 32 bit of SCR_base (4 byte)  added before each recoreded  ts packet*/  /**<CNcomment: ��ÿ��¼�Ƶ�TS ��ǰ��4�ֽ�ʱ���������ΪSCR_BASE �ĸ�32bit */ 
    DMX_REC_TIMESTAMP_LOW32BIT_SCR, 		 /**<Use low 32 bit of SCR_base (4 byte)  added before each recoreded  ts packet*/  /**<CNcomment: ��ÿ��¼�Ƶ�TS ��ǰ��4�ֽ�ʱ���������ΪSCR_BASE �ĵ�32bit */ 
} DMX_REC_TIMESTAMP_MODE_E;

typedef struct
{
    HI_U32                      DmxId;
    HI_UNF_DMX_REC_TYPE_E       RecType;
    HI_BOOL                     Descramed;
    HI_UNF_DMX_REC_INDEX_TYPE_E IndexType;
    HI_UNF_VCODEC_TYPE_E        VCodecType;
    HI_U32                      IndexPid;
    DMX_REC_STATUS_E            RecStatus;
    HI_U32                      RecFqId;
    HI_U32                      ScdFqId;
    HI_U32                      RecOqId;
    HI_U32                      ScdOqId;
    HI_U32                      ScdId;
    HI_U32                      PicParser;
    HI_U32                      FirstFrameMs;
    HI_UNF_DMX_REC_INDEX_S      LastFrameInfo;
    HI_U32                      AddUpMs;
	DMX_REC_TIMESTAMP_MODE_E    enRecTimeStamp;
    struct semaphore            LockRec;
} DMX_RecInfo_S;

typedef struct hiDMX_DEV_OSI_S
{
    DMX_TunerPort_Info_S    TunerPortInfo[DMX_TUNERPORT_CNT];
    DMX_RamPort_Info_S      RamPortInfo[DMX_RAMPORT_CNT];
    DMX_Sub_DevInfo_S       SubDevInfo[DMX_CNT];
    DMX_ChanInfo_S          DmxChanInfo[DMX_CHANNEL_CNT];
    DMX_FilterInfo_S        DmxFilterInfo[DMX_FILTER_CNT];
    DMX_KeyInfo_S           DmxKeyInfo[DMX_KEY_CNT];
    DMX_PCR_Info_S          DmxPcrInfo[DMX_PCR_CHANNEL_CNT];
    DMX_FQ_Info_S           DmxFqInfo[DMX_FQ_CNT];
    DMX_OQ_Info_S           DmxOqInfo[DMX_OQ_CNT];
    DMX_RecInfo_S           DmxRecInfo[DMX_CNT];
    HI_U32                  KeyCsa2HardFlag;
    HI_U32                  KeyCsa3HardFlag;
    HI_U32                  KeySpeHardFlag;
    HI_U32                  KeyOtherHardFlag;
    HI_U32                  AVChanCount;
    wait_queue_head_t       DmxWaitQueue;
    struct semaphore        lock_Channel;
    struct semaphore        lock_AVChan;
    struct semaphore        lock_Filter;
    struct semaphore        lock_Key;
    struct semaphore        lock_OqBuf;
    spinlock_t              splock_OqBuf;
    int 					Data_30604; //30604
} DMX_DEV_OSI_S;

/*
[127:96]:FQStartAddr(32bit);
[96:64]:{FQSize(16bit),FQWPtr(16bit)};
[63:32]:{FQVal(16bit),FQRPtr(16bit)}
[31:0]:{FQAlovfl_TH(8bit),FQIntCfg(4bit),FQIntCnt(4bit),FQUse(16bit)}
*/
//not used now
typedef struct hi_FQ_Header_t
{
    HI_SIZE_T FQStartAddr:32;
    HI_SIZE_T FQSize:16;
    HI_SIZE_T FQWPtr:16;
    HI_SIZE_T FQVal:16;
    HI_SIZE_T FQRPtr:16;
    HI_SIZE_T FQAlovfl_TH:16;
    HI_SIZE_T FQIntCfg:16;
    HI_SIZE_T FQIntCnt:16;
    HI_SIZE_T FQUse:16;
} FQ_HeaderInfor_t;

typedef struct
{
    HI_U32 start_addr;
    HI_U32 buflen;/*rsv(16)+buflen(16)*/
} FQ_DescInfo_S;

/*
[255:224]:BQSAddr(32bit)
[223:192]:{Rsv(2bit),BQIntCfg(4bit),BQSize(10bit),BQAlovfl_TH(8bit),BQCfg(8bit)};
[191:160]:{Rsv(6bit),BQRPtr(10bit),BQIntCnt(4bit),Rsv(2bit),BQWPtr(10bit)}
[159:128]:BBSAddr(32bit)
[127:96]:{BBSize(16bit),BQUse(16bit)}
[96:64]:{BBEopAddr(16bit),BBWaddr(16bit)}
[63:32]:{WResByte(24bit),PVRCtrl(8bit)}
[31:0]:{EopResByte(24bit),Rsv(8bit)}

*/
//not used now
typedef struct hi_OQ_Header_t
{
    HI_SIZE_T OQSAddr:32;
    HI_SIZE_T Rsv1:2;
    HI_SIZE_T OQIntCfg:4;
    HI_SIZE_T OQSize:10;
    HI_SIZE_T OQAlovfl_TH:8;
    HI_SIZE_T OQCfg:8;
    HI_SIZE_T Rsv2:6;
    HI_SIZE_T OQRPtr:10;
    HI_SIZE_T OQIntCnt:4;
    HI_SIZE_T Rsv3:2;
    HI_SIZE_T OQWPtr:10;
    HI_SIZE_T OBSAddr:32;
    HI_SIZE_T OBSize:16;
    HI_SIZE_T OQUse:16;
    HI_SIZE_T OBEopAddr:16;
    HI_SIZE_T OBWaddr:16;
    HI_SIZE_T WResByte:24;
    HI_SIZE_T PVRCtrl:8;
    HI_SIZE_T EopResByte:24;
    HI_SIZE_T Rsv4:8;
} OQ_HeaderInfor_t;

/*
OQ QUEUE DESCRIBTOR
*/
typedef struct hi_OQ_Desc_t
{
    HI_U32 start_addr;
    HI_U32 cactrl_buflen;/*cactrl(16)+buflen(16)*/
    HI_U32 pvrctrl_datalen;/*rsv(8)+pvrctrl(8)+datalen(16)*/
    HI_U32 carsv;
} OQ_DescInfo_S;


/********************************************************
the common bit mask definition
*********************************************************/
#define DMX_MASK_BIT_0      0x00000001UL        /* bit0 */
#define DMX_MASK_BIT_1      0x00000002UL        /* bit1 */
#define DMX_MASK_BIT_2      0x00000004UL        /* bit2 */
#define DMX_MASK_BIT_3      0x00000008UL        /* bit3 */
#define DMX_MASK_BIT_4      0x00000010UL        /* bit4 */
#define DMX_MASK_BIT_5      0x00000020UL        /* bit5 */
#define DMX_MASK_BIT_6      0x00000040UL        /* bit6 */
#define DMX_MASK_BIT_7      0x00000080UL        /* bit7 */
#define DMX_MASK_BIT_8      0x00000100UL        /* bit8 */
#define DMX_MASK_BIT_9      0x00000200UL        /* bit9 */
#define DMX_MASK_BIT_10     0x00000400UL        /* bit10 */
#define DMX_MASK_BIT_11     0x00000800UL        /* bit11 */
#define DMX_MASK_BIT_12     0x00001000UL        /* bit12 */
#define DMX_MASK_BIT_13     0x00002000UL        /* bit13 */
#define DMX_MASK_BIT_14     0x00004000UL        /* bit14 */
#define DMX_MASK_BIT_15     0x00008000UL        /* bit15 */
#define DMX_MASK_BIT_16     0x00010000UL        /* bit16 */
#define DMX_MASK_BIT_17     0x00020000UL        /* bit17 */
#define DMX_MASK_BIT_18     0x00040000UL        /* bit18 */
#define DMX_MASK_BIT_19     0x00080000UL        /* bit19 */
#define DMX_MASK_BIT_20     0x00100000UL        /* bit20 */
#define DMX_MASK_BIT_21     0x00200000UL        /* bit21 */
#define DMX_MASK_BIT_22     0x00400000UL        /* bit22 */
#define DMX_MASK_BIT_23     0x00800000UL        /* bit23 */
#define DMX_MASK_BIT_24     0x01000000UL        /* bit24 */
#define DMX_MASK_BIT_25     0x02000000UL        /* bit25 */
#define DMX_MASK_BIT_26     0x04000000UL        /* bit26 */
#define DMX_MASK_BIT_27     0x08000000UL        /* bit27 */
#define DMX_MASK_BIT_28     0x10000000UL        /* bit28 */
#define DMX_MASK_BIT_29     0x20000000UL        /* bit29 */
#define DMX_MASK_BIT_30     0x40000000UL        /* bit30 */
#define DMX_MASK_BIT_31     0x80000000UL        /* bit31 */

#define DMX_MASK_INT_ALL               0xffffffff

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  // __DRV_DEMUX_DEFINE_H__

