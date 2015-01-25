/******************************************************************************

Copyright (C), 2009-2019, Hisilicon Tech. Co., Ltd.
******************************************************************************
File Name     : drv_demux.c
Version       : Initial Draft
Author        : Hisilicon multimedia software group
Created       : 2009-12-14
Last Modified :
Description   : MPI Layer function declaration for linux
Function List :
History       :
* main\1    2009-12-14   jianglei     init.
******************************************************************************/

#include <linux/kthread.h>

#include "hi_type.h"
#include "hi_module.h"
#include "hi_drv_mmz.h"
#include "hi_drv_mem.h"
#include "hi_drv_sys.h"
#include "hi_drv_module.h"
#include "hi_drv_proc.h"
#include "hi_drv_file.h"
#include "hi_kernel_adapt.h"

#include "demux_debug.h"
#include "hi_mpi_demux.h"

#include "hi_drv_demux.h"
#include "drv_demux_ioctl.h"
#include "hal_demux.h"
#include "drv_demux_func.h"
#include "drv_demux.h"
#include "drv_demux_reg.h"
#include "drv_demux_sw.h"
#include "drv_demux_osal.h"
#include "drv_demux_ext.h"

#ifdef DMX_DESCRAMBLER_SUPPORT
#include "hi_drv_descrambler.h"
#include "drv_descrambler.h"
#endif

#include "drv_sync_ext.h"

/***************************** Macro Definition ******************************/
#define DEMUX_NAME                      "HI_DEMUX"

#define DMX_MIN_POOLBUFFER_SIZE         0x80000
#define DMX_MAX_POOLBUFFER_SIZE         0x400000
#define DMX_DEFAULT_POOLBUFFER_SIZE     CFG_HI_DEMUX_POOLBUF_SIZE

#define DMX_MIN_FQ_BLOCK_SIZE           0x1400      /*previously ,we set this equal 0x1000 (4K) ,now change it to 0x1400 (5K),for customer No:HSCP20130723010833*/
#define DMX_MAX_FQ_BLOCK_SIZE           0xffff
#define DMX_DEFAULT_FQ_BLOCK_SIZE       0x1000

#define DMX_FLTID(FilterHandle)     ((FilterHandle) & 0xff)

#define DMX_FLTHANDLE(FilterId)     ((FilterId) | 0x00000200 | (HI_ID_DEMUX << 16))

#define DMX_CHECK_FLTHANDLE(FilterHandle)                               \
    do                                                                  \
    {                                                                   \
        if (   (DMX_FLTID(FilterHandle) >= DMX_FILTER_CNT)              \
            || (((FilterHandle) & 0xffffff00) != DMX_FLTHANDLE(0)) )    \
        {                                                               \
            HI_ERR_DEMUX("Invalid FilterHandle 0x%x\n", FilterHandle); \
            return HI_ERR_DMX_INVALID_PARA;                             \
        }                                                               \
    } while (0)

#define DMX_CHECK_PCRHANDLE(PcrHandle)                                  \
    do                                                                  \
    {                                                                   \
        if ((PcrHandle) >= DMX_PCR_CHANNEL_CNT)                         \
        {                                                               \
            HI_ERR_DEMUX("Invalid PcrHandle 0x%x\n", PcrHandle);       \
            return HI_ERR_DMX_INVALID_PARA;                             \
        }                                                               \
    } while (0)

#define DMX_DEFAULT_BUF_NUM     16

/**************************** global variables ****************************/
typedef struct
{
    HI_U32                  ChanFile[DMX_CHANNEL_CNT]; /* the proccess hannle of channel */
    HI_U32                 u32TsBufProcessHandle[DMX_RAMPORT_CNT]; /* the proccess handle of ts buffer */
    HI_U32                  RecFile[DMX_CNT];
    HI_U32                 u32FilterProcessHandle[DMX_FILTER_CNT]; /* the proccess handle of filter */
    HI_U32                 u32PcrChProcessHandle[DMX_PCR_CHANNEL_CNT]; /* the proccess hanele of pcr channel */
} DMX_DEV_OSR_S;

static DMX_DEV_OSR_S g_stDmxOsr;

static DEMUX_EXPORT_FUNC_S s_DmxExportFuncs =
{
    .pfnDmxAcquireEs    = HI_DRV_DMX_AcquireEs,
    .pfnDmxReleaseEs    = HI_DRV_DMX_ReleaseEs,
};

static uint DmxPoolBufSize  = DMX_DEFAULT_POOLBUFFER_SIZE;
static uint DmxBlockSize    = DMX_MIN_FQ_BLOCK_SIZE;

module_param(DmxPoolBufSize, uint, S_IRUGO);
module_param(DmxBlockSize, uint, S_IRUGO);

extern SYNC_EXPORT_FUNC_S   *g_pSyncFunc;

/****************************** internal function *****************************/
#ifdef HI_DEMUX_PROC_SUPPORT
static HI_VOID DMX_OsrStopSaveEs(HI_VOID);
static HI_VOID DMX_OsrSaveIPTsStop(HI_VOID);
static HI_VOID DMX_OsrSaveALLTs_Stop(HI_VOID);
static HI_VOID DMX_OsrSaveDmxTs_Stop(HI_VOID);
#endif

static HI_U32 DMXParseBootargs(const HI_CHAR *buf, const HI_CHAR *str)
{
    HI_U32      ret = 0;
    HI_CHAR    *p;

    p = strstr(buf, str);
    if (p)
    {
        p += strlen(str);

        ret = simple_strtol(p, NULL, 0);
    }

    return ret;
}

static HI_VOID DMXGetExternalParameter(HI_U32 *BufSize)
{
    HI_U32 ret;

    ret = DMXParseBootargs(saved_command_line, "DmxPoolBufSize=");
    if (0 != ret)
    {
        if (ret < DMX_MIN_POOLBUFFER_SIZE)
        {
            *BufSize = DMX_MIN_POOLBUFFER_SIZE;
        }
        else if (ret > DMX_MAX_POOLBUFFER_SIZE)
        {
            *BufSize = DMX_MAX_POOLBUFFER_SIZE;
        }
        else
        {
            *BufSize = ret;
        }
    }
}

/**************************** external functions ******************************/

/*****************************************************************************
 Prototype    : HI_DRV_DMX_Init
 Description  : DEMUX module initialize function
 Input        : None
 Output       : None
 Return Value :
*****************************************************************************/
HI_S32 HI_DRV_DMX_Init(HI_VOID)
{
    HI_S32 ret;

    ret = HI_DRV_MODULE_Register(HI_ID_DEMUX, DEMUX_NAME, (HI_VOID*)&s_DmxExportFuncs);
    if (HI_SUCCESS != ret)
    {
        HI_FATAL_DEMUX("HI_DRV_MODULE_Register failed\n"); //172

        return ret;
    }

    DMXGetExternalParameter(&DmxPoolBufSize);

    ret = DMX_OsiDeviceInit(DmxPoolBufSize, DmxBlockSize);
    if (HI_SUCCESS != ret)
    {
        HI_DRV_MODULE_UnRegister(HI_ID_DEMUX);
        HI_ERR_DEMUX("DMX_OsiDeviceInit failed 0x%x\n", ret); //183

        return ret;
    }

    return HI_SUCCESS;
}

/*****************************************************************************
 Prototype    : HI_DRV_DMX_DeInit
 Description  : DEMUX module exit function
 Input        : None
 Output       : None
 Return Value :
*****************************************************************************/
HI_VOID HI_DRV_DMX_DeInit(HI_VOID)
{
#ifdef HI_DEMUX_PROC_SUPPORT
    DMX_OsrStopSaveEs();
    DMX_OsrSaveIPTsStop();
    DMX_OsrSaveALLTs_Stop();
    DMX_OsrSaveDmxTs_Stop();
#endif

    DMX_OsiDeviceDeInit();

    HI_DRV_MODULE_UnRegister(HI_ID_DEMUX);
}

/*****************************************************************************
 Prototype    : HI_DRV_DMX_Open
 Description  : demux open, called when open device node
 Input        : None
 Output       : None
 Return Value :
*****************************************************************************/
HI_S32 HI_DRV_DMX_Open(HI_VOID)
{
    return HI_DRV_MODULE_GetFunction(HI_ID_SYNC, (HI_VOID**)&g_pSyncFunc);

}

/*****************************************************************************
 Prototype    : HI_DRV_DMX_Close
 Description  : demux close function, called when close device node
 Input        : None
 Output       : None
 Return Value :
*****************************************************************************/
HI_S32 HI_DRV_DMX_Close(HI_U32 file)
{
    HI_S32  ret;
    HI_U32  i;

    /* stop record and release the record buffer */
    for (i = 0; i < DMX_CNT; i++)
    {
        if (g_stDmxOsr.RecFile[i] == (HI_U32)file)
        {
            ret = DMX_DRV_REC_StopRecChn(i);
            if (HI_SUCCESS != ret)
            {
                HI_ERR_DEMUX("rec %u stop failed 0x%x\n", i, ret);
            }

            ret = DMX_DRV_REC_DestroyChannel(i);
            if (HI_SUCCESS != ret)
            {
                HI_ERR_DEMUX("rec %u destroy failed 0x%x\n", i, ret);
            }
        }
    }

    /*TS Buffer Deinit, the most important to delete the mmz memory */
    for (i = 0; i < DMX_RAMPORT_CNT; i++)
    {
        if (g_stDmxOsr.u32TsBufProcessHandle[i] == (HI_U32)file)
        {
            HI_DRV_DMX_DestroyTSBuffer(i);
        }
    }

    /*destroy the filters used in this process */
    for (i = 0; i < DMX_FILTER_CNT; i++)
    {
        if (g_stDmxOsr.u32FilterProcessHandle[i] == (HI_U32)file)
        {
            HI_DRV_DMX_DestroyFilter(DMX_FLTHANDLE(i));
        }
    }

#ifdef DMX_DESCRAMBLER_SUPPORT
    DmxDestroyAllDescrambler((HI_U32)file);
#endif

    /*destroy the channels used in this process */
    for (i = 0; i < DMX_CHANNEL_CNT; i++)
    {
        if (g_stDmxOsr.ChanFile[i] == (HI_U32)file)
        {
            ret = HI_DRV_DMX_DestroyChannel(DMX_CHANHANDLE(i));
            if (HI_SUCCESS != ret)
            {
                HI_ERR_DEMUX("destroy chan failed 0x%x\n", ret);
            }
        }
    }

    /*destroy the PCR channels used in this process */
    for (i = 0; i < DMX_PCR_CHANNEL_CNT; i++)
    {
        if (g_stDmxOsr.u32PcrChProcessHandle[i] == (HI_U32)file)
        {
            HI_DRV_DMX_DestroyPcrChannel(i);
        }
    }

    return HI_SUCCESS;
}

HI_S32 HI_DRV_DMX_GetPoolBufAddr(DMX_MMZ_BUF_S *PoolBuf)
{
    CHECKPOINTER(PoolBuf);

    return DMX_OsiGetPoolBufAddr(&PoolBuf->u32BufPhyAddr, &PoolBuf->u32BufSize);
}

HI_S32 HI_DRV_DMX_GetCapability(HI_UNF_DMX_CAPABILITY_S *Cap)
{
    CHECKPOINTER(Cap);

    Cap->u32TunerPortNum    = DMX_TUNERPORT_CNT;
    Cap->u32RamPortNum      = DMX_RAMPORT_CNT;
    Cap->u32DmxNum          = DMX_CNT;
    Cap->u32ChannelNum      = DMX_CHANNEL_CNT;
    Cap->u32AVChannelNum    = DMX_AV_CHANNEL_CNT;
    Cap->u32FilterNum       = DMX_FILTER_CNT;
    Cap->u32KeyNum          = DMX_KEY_CNT;
    Cap->u32RecChnNum       = DMX_REC_CNT;

    return HI_SUCCESS;
}
HI_S32 HI_DRV_DMX_SetPusi(HI_BOOL bCheckPusi)
{
    HI_BOOL bNoPusiEn = HI_TRUE;
    bNoPusiEn = (bCheckPusi == HI_TRUE)?HI_FALSE:HI_TRUE;
    DMX_OsiSetNoPusiEn(bNoPusiEn);
    return HI_SUCCESS;
}

HI_S32 HI_DRV_DMX_SetTei(HI_UNF_DMX_TEI_SET_S *pstTei)
{
    CHECKPOINTER(pstTei);
    CHECKDMXID(pstTei->u32DemuxID);
    DMX_OsiSetTei(pstTei->u32DemuxID,pstTei->bTei);   
    return HI_SUCCESS;
}

HI_S32 HI_DRV_DMX_TunerPortGetAttr(const HI_U32 PortId, HI_UNF_DMX_PORT_ATTR_S *PortAttr)
{
    CHECKTUNERPORTID(PortId);
    CHECKPOINTER(PortAttr);

    return DMX_OsiTunerPortGetAttr(PortId, PortAttr);
}

HI_S32 HI_DRV_DMX_TunerPortSetAttr(const HI_U32 PortId, const HI_UNF_DMX_PORT_ATTR_S *PortAttr)
{
    CHECKTUNERPORTID(PortId);
    CHECKPOINTER(PortAttr);

    return DMX_OsiTunerPortSetAttr(PortId, PortAttr);
}

HI_S32 HI_DRV_DMX_RamPortGetAttr(const HI_U32 PortId, HI_UNF_DMX_PORT_ATTR_S *PortAttr)
{
    CHECKRAMPORTID(PortId);
    CHECKPOINTER(PortAttr);

    return DMX_OsiRamPortGetAttr(PortId, PortAttr);
}

HI_S32 HI_DRV_DMX_RamPortSetAttr(const HI_U32 PortId, const HI_UNF_DMX_PORT_ATTR_S *PortAttr)
{
    CHECKRAMPORTID(PortId);
    CHECKPOINTER(PortAttr);

    return DMX_OsiRamPortSetAttr(PortId, PortAttr);
}

HI_S32 HI_DRV_DMX_AttachTunerPort(HI_U32 DmxId, HI_U32 PortId)
{
    CHECKDMXID(DmxId);
    CHECKTUNERPORTID(PortId);

    return DMX_OsiAttachPort(DmxId, DMX_PORT_MODE_TUNER, PortId);
}

HI_S32 HI_DRV_DMX_AttachRamPort(HI_U32 DmxId, HI_U32 PortId)
{
    CHECKDMXID(DmxId);
    CHECKRAMPORTID(PortId);

    return DMX_OsiAttachPort(DmxId, DMX_PORT_MODE_RAM, PortId);
}

HI_S32 HI_DRV_DMX_DetachPort(HI_U32 DmxId)
{
    CHECKDMXID(DmxId);

    return DMX_OsiDetachPort(DmxId);
}

HI_S32 HI_DRV_DMX_GetPortId(HI_U32 DmxId, DMX_PORT_MODE_E *PortMode, HI_U32 *PortId)
{
    CHECKDMXID(DmxId);
    CHECKPOINTER(PortMode);
    CHECKPOINTER(PortId);

    return DMX_OsiGetPortId(DmxId, PortMode, PortId);
}

HI_S32 HI_DRV_DMX_TunerPortGetPacketNum(const HI_U32 PortId, HI_U32 *TsPackCnt, HI_U32 *ErrTsPackCnt)
{
    CHECKTUNERPORTID(PortId);
    CHECKPOINTER(TsPackCnt);
    CHECKPOINTER(ErrTsPackCnt);

    return DMX_OsiTunerPortGetPacketNum(PortId, TsPackCnt, ErrTsPackCnt);
}

HI_S32 HI_DRV_DMX_RamPortGetPacketNum(const HI_U32 PortId, HI_U32 *TsPackCnt)
{
    CHECKRAMPORTID(PortId);
    CHECKPOINTER(TsPackCnt);

    return DMX_OsiRamPortGetPacketNum(PortId, TsPackCnt);
}

/*
    application ts buffer, initialized repeatly will return failure.
    repeatly initialize in the same thread, will return success in user mode.
    In this repeatly call it, imply called from different process.
    not supported used in more than one process for one port TS buffer
 */
HI_S32 HI_DRV_DMX_CreateTSBuffer(const HI_U32 PortId, const HI_U32 Size, DMX_MMZ_BUF_S *TsBuf, const HI_U32 file)
{
    HI_S32 ret;

    CHECKRAMPORTID(PortId);
    CHECKPOINTER(TsBuf);

    ret = DMX_OsiTsBufferCreate(PortId, Size, TsBuf);
    if (HI_SUCCESS == ret)
    {
        g_stDmxOsr.u32TsBufProcessHandle[PortId] = file;
    }

    return ret;
}

HI_S32 HI_DRV_DMX_DestroyTSBuffer(const HI_U32 PortId)
{
    HI_S32 ret;

    CHECKRAMPORTID(PortId);

    ret = DMX_OsiTsBufferDestroy(PortId);
    if (HI_SUCCESS == ret)
    {
        g_stDmxOsr.u32TsBufProcessHandle[PortId] = 0;
    }

    return ret;
}

HI_S32 HI_DRV_DMX_GetTSBuffer(const HI_U32 PortId, const HI_U32 ReqLen, DMX_DATA_BUF_S *Data, const HI_U32 TimeoutMs)
{
    CHECKRAMPORTID(PortId);
    CHECKPOINTER(Data);

    return DMX_OsiTsBufferGet(PortId, ReqLen, Data, TimeoutMs);
}

HI_S32 HI_DRV_DMX_PutTSBuffer(const HI_U32 PortId, const HI_U32 DataLen, const HI_U32 StartPos)
{
    CHECKRAMPORTID(PortId);

    return DMX_OsiTsBufferPut(PortId, DataLen, StartPos);
}

HI_S32 HI_DRV_DMX_ResetTSBuffer(const HI_U32 PortId)
{
    CHECKRAMPORTID(PortId);

    return DMX_OsiTsBufferReset(PortId);
}

HI_S32 HI_DRV_DMX_GetTSBufferStatus(const HI_U32 PortId, HI_UNF_DMX_TSBUF_STATUS_S *Status)
{
    CHECKRAMPORTID(PortId);
    CHECKPOINTER(Status);

    return DMX_OsiTsBufferGetStatus(PortId, Status);
}

HI_S32 HI_DRV_DMX_CreateChannel(
        HI_U32                  u32DmxId,
        HI_UNF_DMX_CHAN_ATTR_S *pstChAttr,
        HI_HANDLE              *phChannel,
        DMX_MMZ_BUF_S          *pstChBuf,
        HI_U32                  file
    )
{
    HI_S32  ret;
    HI_U32  ChanId;

    CHECKDMXID(u32DmxId);
    CHECKPOINTER(pstChAttr);
    CHECKPOINTER(phChannel);
    CHECKPOINTER(pstChBuf);

    ret = DMX_OsiCreateChannel(u32DmxId, pstChAttr, pstChBuf, &ChanId);
    if (HI_SUCCESS == ret)
    {
        g_stDmxOsr.ChanFile[ChanId] = (HI_U32)file;

        *phChannel = DMX_CHANHANDLE(ChanId);
    }

    return ret;
}

HI_S32 HI_DRV_DMX_DestroyChannel(HI_HANDLE hChannel)
{
    HI_S32  ret;
    HI_U32  ChanId;

    DMX_CHECK_CHANHANDLE(hChannel);

    ChanId = DMX_CHANID(hChannel);

    ret = DMX_OsiDestroyChannel(ChanId);
    if (HI_SUCCESS == ret)
    {
        g_stDmxOsr.ChanFile[ChanId] = 0;
    }

    return ret;
}

HI_S32 HI_DRV_DMX_GetChannelAttr(HI_HANDLE hChannel, HI_UNF_DMX_CHAN_ATTR_S *pstChAttr)
{
    DMX_CHECK_CHANHANDLE(hChannel);

    return DMX_OsiGetChannelAttr(DMX_CHANID(hChannel), pstChAttr);
}

HI_S32 HI_DRV_DMX_SetChannelAttr(HI_HANDLE hChannel, HI_UNF_DMX_CHAN_ATTR_S *pstChAttr)
{
    DMX_CHECK_CHANHANDLE(hChannel);

    return DMX_OsiSetChannelAttr(DMX_CHANID(hChannel), pstChAttr);
}

HI_S32 HI_DRV_DMX_SetChannelPID(HI_HANDLE hChannel, HI_U32 u32Pid)
{
    DMX_CHECK_CHANHANDLE(hChannel);

    return DMX_OsiSetChannelPid(DMX_CHANID(hChannel), u32Pid);
}

HI_S32 HI_DRV_DMX_GetChannelPID(HI_HANDLE hChannel, HI_U32 *pu32Pid)
{
    DMX_CHECK_CHANHANDLE(hChannel);
    CHECKPOINTER(pu32Pid);

    return DMX_OsiGetChannelPid(DMX_CHANID(hChannel), pu32Pid);
}

HI_S32 HI_DRV_DMX_OpenChannel(HI_HANDLE hChannel)
{
    DMX_CHECK_CHANHANDLE(hChannel);

    return DMX_OsiOpenChannel(DMX_CHANID(hChannel));
}

HI_S32 HI_DRV_DMX_CloseChannel(HI_HANDLE hChannel)
{
    DMX_CHECK_CHANHANDLE(hChannel);

    return DMX_OsiCloseChannel(DMX_CHANID(hChannel));
}

HI_S32 HI_DRV_DMX_GetChannelStatus(HI_HANDLE hChannel, HI_UNF_DMX_CHAN_STATUS_S *pstStatus)
{
    DMX_CHECK_CHANHANDLE(hChannel);

    return DMX_OsiGetChannelStatus(DMX_CHANID(hChannel), &pstStatus->enChanStatus);
}

HI_S32 HI_DRV_DMX_GetChannelHandle(HI_U32 DmxId, HI_U32 Pid, HI_HANDLE *ChanHandle)
{
    HI_S32  ret;
    HI_U32  ChanId;

    CHECKDMXID(DmxId);
    CHECKPOINTER(ChanHandle);

    ret = DMX_OsiGetChannelId(DmxId, Pid, &ChanId);
    if (HI_SUCCESS == ret)
    {
        *ChanHandle = DMX_CHANHANDLE(ChanId);
    }

    return ret;
}

HI_S32 HI_DRV_DMX_GetFreeChannelCount (HI_U32 DmxId, HI_U32 *FreeCount)
{
    CHECKDMXID(DmxId);
    CHECKPOINTER(FreeCount);

    return DMX_OsiGetFreeChannelNum(DmxId, FreeCount);
}

HI_S32 HI_DRV_DMX_GetScrambledFlag(HI_HANDLE hChannel, HI_UNF_DMX_SCRAMBLED_FLAG_E *ScrambleFlag)
{
    DMX_CHECK_CHANHANDLE(hChannel);
    CHECKPOINTER(ScrambleFlag);

    return DMX_OsiGetChannelScrambleFlag(DMX_CHANID(hChannel), ScrambleFlag);
}

HI_S32 HI_DRV_DMX_SetChannelEosFlag(HI_HANDLE hChannel)
{
    DMX_CHECK_CHANHANDLE(hChannel);

    return DMX_OsiSetChannelEosFlag(DMX_CHANID(hChannel));
}

#ifdef DMX_USE_ECM
HI_S32 DMX_OsrGetChannelSwFlag(HI_HANDLE hChannel, HI_U32 *pu32SwFlag)
{
    DMX_CHECK_CHANHANDLE(hChannel);
    CHECKPOINTER(pu32SwFlag);

    return DMX_OsiGetChannelSwFlag(DMX_CHANID(hChannel), pu32SwFlag);
}

HI_S32 DMX_OsrGetChannelSwBufAddr(HI_HANDLE hChannel, DMX_MMZ_BUF_S* pstSwBuf)
{
    HI_S32 ret;
    MMZ_BUFFER_S stChnSwBuf;

    DMX_CHECK_CHANHANDLE(hChannel);
    CHECKPOINTER(pstSwBuf);

    ret = DMX_OsiGetChannelSwBufAddr(DMX_CHANID(hChannel), &stChnSwBuf);
    if (HI_SUCCESS != ret)
    {
        HI_ERR_DEMUX("DMX_OsiGetChannelSwBufAddr failed:ChId=%d\n", DMX_CHANID(hChannel));
        return ret;
    }
    pstSwBuf->u32BufPhyAddr = stChnSwBuf.u32StartPhyAddr;
    pstSwBuf->u32BufKerVirAddr = stChnSwBuf.u32StartVirAddr;
    pstSwBuf->u32BufSize = stChnSwBuf.u32Size;

    return HI_SUCCESS;

}
#endif
HI_S32 HI_DRV_DMX_GetChannelTsCount(HI_HANDLE hChannel, HI_U32 *pu32TsCnt)
{
    DMX_CHECK_CHANHANDLE(hChannel);
    CHECKPOINTER(pu32TsCnt);

    return DMX_OsiGetChannelTsCnt(DMX_CHANID(hChannel), pu32TsCnt);
}

HI_S32 HI_DRV_DMX_SetChannelCCRepeat(HI_HANDLE hChannel, HI_UNF_DMX_CHAN_CC_REPEAT_SET_S *pstChCCReaptSet)
{
    DMX_CHECK_CHANHANDLE(hChannel);
    return DMX_OsiSetChannelCCRepeat(DMX_CHANID(hChannel),pstChCCReaptSet);
}


HI_S32 HI_DRV_DMX_CreateFilter(HI_U32 DmxId, HI_UNF_DMX_FILTER_ATTR_S *FilterAttr, HI_HANDLE *Filter, HI_U32 file)
{
    HI_S32  ret;
    HI_U32  FilterId;

    CHECKDMXID(DmxId);
    CHECKPOINTER(FilterAttr);
    CHECKPOINTER(Filter);

    ret = DMX_OsiNewFilter(DmxId, &FilterId);
    if (HI_SUCCESS != ret)
    {
        return ret;
    }

    ret = DMX_OsiSetFilterAttr(FilterId, FilterAttr);
    if (HI_SUCCESS != ret)
    {
        DMX_OsiDeleteFilter(FilterId);

        return ret;
    }

    *Filter = DMX_FLTHANDLE(FilterId);

    g_stDmxOsr.u32FilterProcessHandle[FilterId] = file;

    return HI_SUCCESS;
}

HI_S32 HI_DRV_DMX_DestroyFilter(HI_HANDLE Filter)
{
    HI_S32 ret;

    DMX_CHECK_FLTHANDLE(Filter);

    ret = DMX_OsiDeleteFilter(DMX_FLTID(Filter));
    if (HI_SUCCESS == ret)
    {
        g_stDmxOsr.u32FilterProcessHandle[DMX_FLTID(Filter)] = 0;
    }

    return ret;
}

HI_S32 HI_DRV_DMX_DestroyAllFilter(HI_HANDLE Channel)
{
    HI_S32 ret = HI_SUCCESS;
    HI_U32 FilterId;

    DMX_CHECK_CHANHANDLE(Channel);

    for (FilterId = 0; FilterId < DMX_FILTER_CNT; FilterId++)
    {
        HI_U32 ChanId;

        ret = DMX_OsiGetFilterChannel(FilterId, &ChanId);
        if (HI_SUCCESS == ret)
        {
            if (ChanId == DMX_CHANID(Channel))
            {
                ret = DMX_OsiDeleteFilter(FilterId);
                if (HI_SUCCESS != ret)
                {
                    HI_ERR_DEMUX("DMX_OsiDeleteFilter failed:0x%x\n", ret);
                    return ret;
                }

                g_stDmxOsr.u32FilterProcessHandle[FilterId] = 0;
            }
        }
    }

    return HI_SUCCESS;
}

HI_S32 HI_DRV_DMX_SetFilterAttr(HI_HANDLE Filter, HI_UNF_DMX_FILTER_ATTR_S *FilterAttr)
{
    DMX_CHECK_FLTHANDLE(Filter);
    CHECKPOINTER(FilterAttr);

    return DMX_OsiSetFilterAttr(DMX_FLTID(Filter), FilterAttr);
}

HI_S32 HI_DRV_DMX_GetFilterAttr(HI_HANDLE Filter, HI_UNF_DMX_FILTER_ATTR_S *FilterAttr)
{
    DMX_CHECK_FLTHANDLE(Filter);
    CHECKPOINTER(FilterAttr);

    return DMX_OsiGetFilterAttr(DMX_FLTID(Filter), FilterAttr);
}

HI_S32 HI_DRV_DMX_AttachFilter(HI_HANDLE Filter, HI_HANDLE Channel)
{
    DMX_CHECK_FLTHANDLE(Filter);
    DMX_CHECK_CHANHANDLE(Channel);

    return DMX_OsiAttachFilter(DMX_FLTID(Filter), DMX_CHANID(Channel));
}

HI_S32 HI_DRV_DMX_DetachFilter(HI_HANDLE Filter, HI_HANDLE Channel)
{
    DMX_CHECK_FLTHANDLE(Filter);
    DMX_CHECK_CHANHANDLE(Channel);

    return DMX_OsiDetachFilter(DMX_FLTID(Filter), DMX_CHANID(Channel));
}

HI_S32 HI_DRV_DMX_GetFilterChannelHandle(HI_HANDLE Filter, HI_HANDLE *Channel)
{
    HI_S32 ret;
    HI_U32 ChanId;

    DMX_CHECK_FLTHANDLE(Filter);
    CHECKPOINTER(Channel);

    ret = DMX_OsiGetFilterChannel(DMX_FLTID(Filter), &ChanId);
    if (HI_SUCCESS == ret)
    {
        *Channel = DMX_CHANHANDLE(ChanId);
    }

    return ret;
}

HI_S32 HI_DRV_DMX_GetFreeFilterCount(HI_U32 DmxId, HI_U32 *FreeCount)
{
    CHECKDMXID(DmxId);
    CHECKPOINTER(FreeCount);

    return DMX_OsiGetFreeFilterNum(DmxId, FreeCount);
}

HI_S32 HI_DRV_DMX_GetDataHandle(HI_U32 *pu32Flag, HI_U32 u32TimeOutMs)
{
    CHECKPOINTER(pu32Flag);

    return DMX_OsiSelectDataFlag(HI_NULL, DMX_CHANNEL_CNT, pu32Flag, u32TimeOutMs);
}

HI_S32 HI_DRV_DMX_SelectDataHandle(HI_U32 *pu32WatchChannel, HI_U32 u32WatchNum, HI_U32 *pu32Flag, HI_U32 u32TimeOutMs)
{
    HI_U32  i;
    HI_U32  Chan[DMX_CHANNEL_CNT];

    CHECKPOINTER(pu32WatchChannel);
    CHECKPOINTER(pu32Flag);

    if (0 == u32WatchNum)
    {
        HI_ERR_DEMUX("u32WatchNum == 0!\n");
        return HI_ERR_DMX_INVALID_PARA;
    }

    for (i = 0; i < u32WatchNum; i++)
    {
        DMX_CHECK_CHANHANDLE(pu32WatchChannel[i]);

        Chan[i] = DMX_CHANID(pu32WatchChannel[i]);
    }

    return DMX_OsiSelectDataFlag(Chan, u32WatchNum, pu32Flag, u32TimeOutMs);
}

/*pstBuf is the address of user mode, need to copy out buf info
   pu8Buf in pstBuf need to return physical address to user mode
 */
HI_S32 HI_DRV_DMX_AcquireBuf(HI_HANDLE hChannel, HI_U32 u32AcquireNum,
                          HI_U32 *pu32AcquiredNum, HI_UNF_DMX_DATA_S *pstBuf,
                          HI_U32 u32TimeOutMs)
{
    HI_S32  ret;
    HI_U32  i;
    HI_U32  ChanId;
    DMX_UserMsg_S stReqBufTmp[DMX_DEFAULT_BUF_NUM];
    HI_UNF_DMX_DATA_S stUsrBuf;
    DMX_UserMsg_S *pstBufTmp;
    #ifdef DMX_USE_ECM
    HI_U32 u32SwFlag;
    #endif

    DMX_CHECK_CHANHANDLE(hChannel);

    ChanId = DMX_CHANID(hChannel);

    if (u32AcquireNum > DMX_DEFAULT_BUF_NUM)
    {
        pstBufTmp = HI_VMALLOC(HI_ID_DEMUX, sizeof(DMX_UserMsg_S) * u32AcquireNum);
        if (NULL == pstBufTmp)
        {
            HI_FATAL_DEMUX("malloc failed.\n");
            return HI_FAILURE;
        }
    }
    else
    {
        pstBufTmp = stReqBufTmp;
    }

    *pu32AcquiredNum = 0;

    #ifdef DMX_USE_ECM
    DMX_OsiGetChannelSwFlag(ChanId, &u32SwFlag);
    if (u32SwFlag)
    {
        ret  = HI_DMX_SwReadDataRequest(ChanId, u32AcquireNum, pu32AcquiredNum, pstBufTmp, u32TimeOutMs);
        if (HI_SUCCESS != ret)
        {
            if (u32AcquireNum > DMX_DEFAULT_BUF_NUM)
            {
                HI_VFREE(HI_ID_DEMUX, pstBufTmp);
            }
            HI_WARN_DEMUX("HI_DMX_SwReadDataRequest failed:%x.\n",ret);
            return ret;
        }

    }
    else
    {
    #endif
        ret = DMX_OsiReadDataRequset(ChanId, u32AcquireNum, pu32AcquiredNum, pstBufTmp, u32TimeOutMs);
        if (HI_SUCCESS != ret)
        {
            if (u32AcquireNum > DMX_DEFAULT_BUF_NUM)
            {
                HI_VFREE(HI_ID_DEMUX, pstBufTmp);
            }
            HI_WARN_DEMUX("DMX_OsiReadDataRequset failed:%x.\n",ret);
            return ret;
        }
    #ifdef DMX_USE_ECM
    }
    #endif

    for (i = 0; i < *pu32AcquiredNum; i++)
    {
        stUsrBuf.pu8Data = (HI_U8*)pstBufTmp[i].u32BufStartAddr;
        stUsrBuf.u32Size = pstBufTmp[i].u32MsgLen;
        if (DMX_OsiGetChType(ChanId) == HI_UNF_DMX_CHAN_TYPE_PES)
        {
            stUsrBuf.enDataType = pstBufTmp[i].enDataType;
        }
        else
        {
            stUsrBuf.enDataType = HI_UNF_DMX_DATA_TYPE_WHOLE;
        }

        if (copy_to_user(&pstBuf[i], &stUsrBuf, sizeof(HI_UNF_DMX_DATA_S)))
        {
            HI_ERR_DEMUX("copy data buf to usr failed\n");
            if (u32AcquireNum > DMX_DEFAULT_BUF_NUM)
            {
                HI_VFREE(HI_ID_DEMUX, pstBufTmp);
            }

            return HI_FAILURE;
        }
    }

    if (u32AcquireNum > DMX_DEFAULT_BUF_NUM)
    {
        HI_VFREE(HI_ID_DEMUX, pstBufTmp);
    }

    return HI_SUCCESS;
}

HI_S32 HI_DRV_DMX_ReleaseBuf(HI_HANDLE hChannel, HI_U32 u32ReleaseNum, HI_UNF_DMX_DATA_S *pstBuf)
{
    HI_S32 ret;
    HI_U32              ChanId;
    HI_U32 i;
    HI_UNF_DMX_DATA_S stUsrBuf;
    DMX_UserMsg_S *pstBufTmp;
    DMX_UserMsg_S stReqBufTmp[DMX_DEFAULT_BUF_NUM];
    #ifdef DMX_USE_ECM
    HI_U32 u32SwFlag;
    #endif

    DMX_CHECK_CHANHANDLE(hChannel);
    CHECKPOINTER(pstBuf);

    if (0 == u32ReleaseNum)
    {
        HI_ERR_DEMUX("u32ReleaseNum == 0!\n");
        return HI_ERR_DMX_INVALID_PARA;
    }

    if (u32ReleaseNum > DMX_DEFAULT_BUF_NUM)
    {
        pstBufTmp = HI_VMALLOC(HI_ID_DEMUX, sizeof(DMX_UserMsg_S) * u32ReleaseNum);
    }
    else
    {
        pstBufTmp = stReqBufTmp;
    }

    if (NULL == pstBufTmp)
    {
        HI_FATAL_DEMUX("malloc failed u32ReleaseNum:%x.\n", u32ReleaseNum);
        return HI_FAILURE;
    }

    ChanId = DMX_CHANID(hChannel);

    for (i = 0; i < u32ReleaseNum; i++)
    {
        if (copy_from_user(&stUsrBuf, &pstBuf[i], sizeof(HI_UNF_DMX_DATA_S)))
        {
            HI_ERR_DEMUX("copy data buf to usr failed\n");
            if (u32ReleaseNum > DMX_DEFAULT_BUF_NUM)
            {
                HI_VFREE(HI_ID_DEMUX, pstBufTmp);
            }

            return HI_FAILURE;
        }

        pstBufTmp[i].u32BufStartAddr = (HI_U32)stUsrBuf.pu8Data;
        pstBufTmp[i].u32MsgLen = stUsrBuf.u32Size;
    }

#ifdef DMX_USE_ECM
    DMX_OsiGetChannelSwFlag(ChanId, &u32SwFlag);
    if (u32SwFlag)
    {
        HI_S32 s32Ret;
        s32Ret  = HI_DMX_SwReleaseReadData(ChanId, u32ReleaseNum, pstBufTmp);
        if (HI_SUCCESS != s32Ret)
        {
            HI_ERR_DEMUX(" sw channel %d release error:%x!\n", ChanId, s32Ret);
            if (u32ReleaseNum > DMX_DEFAULT_BUF_NUM)
            {
                HI_VFREE(HI_ID_DEMUX, pstBufTmp);
            }

            return s32Ret;
        }

    }
    else
    {
#endif
        ret = DMX_OsiReleaseReadData(ChanId, u32ReleaseNum, pstBufTmp);
        if (HI_SUCCESS != ret)
        {
            HI_ERR_DEMUX("DMX_OsiReleaseReadData failed: ChId=%d, ret=0x%x\n", ChanId, ret);
            if (u32ReleaseNum > DMX_DEFAULT_BUF_NUM)
            {
                HI_VFREE(HI_ID_DEMUX, pstBufTmp);
            }

            return ret;
        }
     #ifdef DMX_USE_ECM
     }
     #endif

    if (u32ReleaseNum > DMX_DEFAULT_BUF_NUM)
    {
        HI_VFREE(HI_ID_DEMUX, pstBufTmp);
    }

    return HI_SUCCESS;
}

HI_S32 HI_DRV_DMX_CreatePcrChannel(const HI_U32 DmxId, HI_U32 *PcrHandle, const HI_U32 file)
{
    HI_S32 ret;

    CHECKDMXID(DmxId);
    CHECKPOINTER(PcrHandle);

    ret = DMX_OsiPcrChannelCreate(DmxId, PcrHandle);
    if (HI_SUCCESS == ret)
    {
        g_stDmxOsr.u32PcrChProcessHandle[*PcrHandle] = (HI_U32)file;
    }

    return ret;
}

HI_S32 HI_DRV_DMX_DestroyPcrChannel(const HI_U32 PcrHandle)
{
    HI_S32 ret;

    DMX_CHECK_PCRHANDLE(PcrHandle);

    ret = DMX_OsiPcrChannelDestroy(PcrHandle);
    if (HI_SUCCESS == ret)
    {
        g_stDmxOsr.u32PcrChProcessHandle[PcrHandle] = 0;
    }

    return ret;
}

HI_S32 HI_DRV_DMX_PcrPidSet(const HI_U32 PcrHandle, const HI_U32 PcrPid)
{
    DMX_CHECK_PCRHANDLE(PcrHandle);

    return DMX_OsiPcrChannelSetPid(PcrHandle, PcrPid);
}

HI_S32 HI_DRV_DMX_PcrPidGet(const HI_U32 PcrHandle, HI_U32 *PcrPid)
{
    DMX_CHECK_PCRHANDLE(PcrHandle);
    CHECKPOINTER(PcrPid);

    return DMX_OsiPcrChannelGetPid(PcrHandle, PcrPid);
}

HI_S32 HI_DRV_DMX_PcrScrGet(const HI_U32 PcrHandle, HI_U64 *PcrValue, HI_U64 *ScrValue)
{
    DMX_CHECK_PCRHANDLE(PcrHandle);
    CHECKPOINTER(PcrValue);
    CHECKPOINTER(ScrValue);

    return DMX_OsiPcrChannelGetClock(PcrHandle, PcrValue, ScrValue);
}

HI_S32 HI_DRV_DMX_PcrSyncAttach(const HI_U32 PcrHandle, const HI_U32 SyncHandle)
{
    DMX_CHECK_PCRHANDLE(PcrHandle);

    return DMX_OsiPcrChannelAttachSync(PcrHandle, SyncHandle);
}

HI_S32 HI_DRV_DMX_PcrSyncDetach(const HI_U32 PcrHandle)
{
    DMX_CHECK_PCRHANDLE(PcrHandle);

    return DMX_OsiPcrChannelDetachSync(PcrHandle);
}

HI_S32 HI_DRV_DMX_GetPESBufferStatus(HI_HANDLE ChanHandle, HI_MPI_DMX_BUF_STATUS_S *BufStatus)
{
    DMX_CHECK_CHANHANDLE(ChanHandle);
    CHECKPOINTER(BufStatus);

    return DMX_OsiGetChanBufStatus(DMX_CHANID(ChanHandle), BufStatus);
}

HI_S32 HI_DRV_DMX_AcquireEs(HI_HANDLE hChannel, DMX_Stream_S *pEsBuf)
{
    DMX_CHECK_CHANHANDLE(hChannel); //1079
    CHECKPOINTER(pEsBuf); //1080

    return DMX_OsiReadEsRequset(DMX_CHANID(hChannel), pEsBuf);
}

HI_S32 HI_DRV_DMX_ReleaseEs(HI_HANDLE hChannel, DMX_Stream_S *pEsBuf)
{
    DMX_CHECK_CHANHANDLE(hChannel); //1087
    CHECKPOINTER(pEsBuf); //1088

    return DMX_OsiReleaseReadEs(DMX_CHANID(hChannel), pEsBuf);
}

HI_S32 HI_DRV_DMX_CreateRecChn(
        HI_UNF_DMX_REC_ATTR_S  *RecAttr,
        HI_HANDLE              *RecHandle,
        HI_U32                 *RecBufPhyAddr,
        HI_U32                 *RecBufSize,
        HI_U32                  file
    )
{
    HI_S32  ret;
    HI_U32  RecId;
	DMX_REC_TIMESTAMP_MODE_E enRecTimeStamp = DMX_REC_TIMESTAMP_NONE;

    CHECKPOINTER(RecAttr);
    CHECKPOINTER(RecHandle);
    CHECKPOINTER(RecBufPhyAddr);
    CHECKPOINTER(RecBufSize);

    ret = DMX_DRV_REC_CreateChannel(RecAttr, enRecTimeStamp,&RecId, RecBufPhyAddr, RecBufSize);
    if (HI_SUCCESS == ret)
    {
        g_stDmxOsr.RecFile[RecId] = file;

        *RecHandle = DMX_RECHANDLE(RecId);
    }

    return ret;
}

HI_S32 HI_DRV_DMX_DestroyRecChn(HI_HANDLE RecHandle)
{
    HI_S32  ret;
    HI_U32  RecId;

    DMX_CHECK_RECHANDLE(RecHandle);

    RecId = DMX_RECID(RecHandle);

    ret = DMX_DRV_REC_DestroyChannel(RecId);
    if (HI_SUCCESS == ret)
    {
        g_stDmxOsr.RecFile[RecId] = 0;
    }

    return ret;
}

HI_S32 HI_DRV_DMX_AddRecPid(HI_HANDLE RecHandle, HI_U32 Pid, HI_HANDLE *ChanHandle, HI_U32 file)
{
    HI_S32  ret;
    HI_U32  ChanId;

    DMX_CHECK_RECHANDLE(RecHandle);
    CHECKPOINTER(ChanHandle);

    ret = DMX_DRV_REC_AddRecPid(DMX_RECID(RecHandle), Pid, &ChanId);
    if (HI_SUCCESS == ret)
    {
        *ChanHandle = DMX_CHANHANDLE(ChanId);

        g_stDmxOsr.ChanFile[ChanId] = (HI_U32)file;
    }

    return ret;
}

HI_S32 HI_DRV_DMX_DelRecPid(HI_HANDLE RecHandle, HI_HANDLE ChanHandle)
{
    HI_S32  ret;
    HI_U32  ChanId;

    DMX_CHECK_RECHANDLE(RecHandle);
    DMX_CHECK_CHANHANDLE(ChanHandle);

    ChanId = DMX_CHANID(ChanHandle);

    ret = DMX_DRV_REC_DelRecPid(DMX_RECID(RecHandle), ChanId);
    if (HI_SUCCESS == ret)
    {
        g_stDmxOsr.ChanFile[ChanId] = 0;
    }

    return ret;
}

HI_S32 HI_DRV_DMX_DelAllRecPid(HI_HANDLE RecHandle)
{
    DMX_CHECK_RECHANDLE(RecHandle);

    return DMX_DRV_REC_DelAllRecPid(DMX_RECID(RecHandle));
}

HI_S32 HI_DRV_DMX_AddExcludeRecPid(HI_HANDLE RecHandle, HI_U32 Pid)
{
    DMX_CHECK_RECHANDLE(RecHandle);

    return DMX_DRV_REC_AddExcludeRecPid(DMX_RECID(RecHandle), Pid);
}

HI_S32 HI_DRV_DMX_DelExcludeRecPid(HI_HANDLE RecHandle, HI_U32 Pid)
{
    DMX_CHECK_RECHANDLE(RecHandle);

    return DMX_DRV_REC_DelExcludeRecPid(DMX_RECID(RecHandle), Pid);
}

HI_S32 HI_DRV_DMX_DelAllExcludeRecPid(HI_HANDLE RecHandle)
{
    DMX_CHECK_RECHANDLE(RecHandle);

    return DMX_DRV_REC_DelAllExcludeRecPid(DMX_RECID(RecHandle));
}

HI_S32 HI_DRV_DMX_StartRecChn(HI_HANDLE RecHandle)
{
    DMX_CHECK_RECHANDLE(RecHandle);

    return DMX_DRV_REC_StartRecChn(DMX_RECID(RecHandle));
}

HI_S32 HI_DRV_DMX_StopRecChn(HI_HANDLE RecHandle)
{
    DMX_CHECK_RECHANDLE(RecHandle);

    return DMX_DRV_REC_StopRecChn(DMX_RECID(RecHandle));
}

HI_S32 HI_DRV_DMX_AcquireRecData(HI_HANDLE RecHandle, HI_UNF_DMX_REC_DATA_S *RecData, HI_U32 Timeout)
{
    HI_S32  ret;
    HI_U32  PhyAddr;
    HI_U32  KerAddr;
    HI_U32  Len;

    DMX_CHECK_RECHANDLE(RecHandle);
    CHECKPOINTER(RecData);

    ret = DMX_DRV_REC_AcquireRecData(DMX_RECID(RecHandle), &PhyAddr, &KerAddr, &Len, Timeout);
    if (HI_SUCCESS == ret)
    {
        RecData->u32DataPhyAddr = PhyAddr;
        RecData->u32Len         = Len;
    }

    return ret;
}

HI_S32 HI_DRV_DMX_ReleaseRecData(HI_HANDLE RecHandle, const HI_UNF_DMX_REC_DATA_S *RecData)
{
    DMX_CHECK_RECHANDLE(RecHandle);
    CHECKPOINTER(RecData);

    return DMX_DRV_REC_ReleaseRecData(DMX_RECID(RecHandle), RecData->u32DataPhyAddr, RecData->u32Len);
}

HI_S32 HI_DRV_DMX_AcquireRecIndex(HI_HANDLE RecHandle, HI_UNF_DMX_REC_INDEX_S *RecIndex, HI_U32 Timeout)
{
    DMX_CHECK_RECHANDLE(RecHandle);
    CHECKPOINTER(RecIndex);

    return DMX_DRV_REC_AcquireRecIndex(DMX_RECID(RecHandle), RecIndex, Timeout);
}

HI_S32 HI_DRV_DMX_GetRecBufferStatus(HI_HANDLE RecHandle, HI_UNF_DMX_RECBUF_STATUS_S *BufStatus)
{
    DMX_CHECK_RECHANDLE(RecHandle);
    CHECKPOINTER(BufStatus);

    return DMX_DRV_REC_GetRecBufferStatus(DMX_RECID(RecHandle), BufStatus);
}

HI_S32 HI_DRV_DMX_AcquireScdData(HI_HANDLE RecHandle, HI_UNF_DMX_REC_DATA_S *RecData, HI_U32 Timeout)
{
    HI_S32  ret;
    HI_U32  PhyAddr;
    HI_U32  KerAddr;
    HI_U32  Len;

    DMX_CHECK_RECHANDLE(RecHandle);
    CHECKPOINTER(RecData);

    ret = DMX_DRV_REC_AcquireScdData(DMX_RECID(RecHandle), &PhyAddr, &KerAddr, &Len, Timeout);
    if (HI_SUCCESS == ret)
    {
        RecData->u32DataPhyAddr = PhyAddr;
        RecData->u32Len         = Len;
    }

    return ret;
}

HI_S32 HI_DRV_DMX_ReleaseScdData(HI_HANDLE RecHandle, const HI_UNF_DMX_REC_DATA_S *RecData)
{
    DMX_CHECK_RECHANDLE(RecHandle);
    CHECKPOINTER(RecData);

    return DMX_DRV_REC_ReleaseScdData(DMX_RECID(RecHandle), RecData->u32DataPhyAddr, RecData->u32Len);
}

#ifdef HI_DEMUX_PROC_SUPPORT
#define DMX_FILE_NAME_LEN   (256)

HI_DECLARE_MUTEX(SaveEsMutex);
static struct file *DmxEsHandle[DMX_CHANNEL_CNT];
static HI_U32       SaveEsFlag         = 0;

HI_VOID DMX_OsrSaveEs(HI_U32 type, HI_U8 *buf, HI_U32 len,HI_U32 chnid)
{
    if (len)
    {
		if (0 == down_interruptible(&SaveEsMutex))
		{
            if (SaveEsFlag && ((HI_UNF_DMX_CHAN_TYPE_VID == type) || (HI_UNF_DMX_CHAN_TYPE_AUD == type)))
            {
                if (HI_NULL == DmxEsHandle[chnid])
                {
                    HI_CHAR str[DMX_FILE_NAME_LEN]  = {0};
                    HI_CHAR path[DMX_FILE_NAME_LEN] = {0};

                    if (HI_SUCCESS != HI_DRV_FILE_GetStorePath(path, DMX_FILE_NAME_LEN))
                    {
                        HI_ERR_DEMUX("get path failed\n");

                        return;
                    }
                    if (HI_UNF_DMX_CHAN_TYPE_VID == type)
                    {
                        snprintf(str, sizeof(str),"%s/dmx_vid_%u.es", path, chnid);
                    }
                    else
                    {
                        snprintf(str,sizeof(str), "%s/dmx_aud_%u.es", path, chnid);
                    }                

                    DmxEsHandle[chnid] = HI_DRV_FILE_Open(str, 1);
                    if (!DmxEsHandle[chnid])
                    {
                        HI_ERR_DEMUX("open %s error\n", str);

                        return;
                    }
                }
                HI_DRV_FILE_Write(DmxEsHandle[chnid], buf, len);
            } 
    		up(&SaveEsMutex);
		}       

    }
}

static HI_S32 DMX_OsrStartSaveEs(HI_VOID)
{
    HI_CHAR path[DMX_FILE_NAME_LEN] = {0};

    if (SaveEsFlag)
    {
        return HI_SUCCESS;
    }
	

    if (HI_SUCCESS != HI_DRV_FILE_GetStorePath(path, DMX_FILE_NAME_LEN))
    {
        HI_ERR_DEMUX("get path failed\n");

        return HI_FAILURE;
    }
	if (0 == down_interruptible(&SaveEsMutex))
	{
        memset(DmxEsHandle,0,DMX_CHANNEL_CNT*sizeof(struct file *));
        SaveEsFlag = 1;
    	up(&SaveEsMutex);
    	return HI_SUCCESS;
	}
    return HI_FAILURE;
}

static HI_VOID DMX_OsrStopSaveEs(HI_VOID)
{
    HI_S32 i;
	if (0 == down_interruptible(&SaveEsMutex))
	{
        for ( i = 0 ; i < DMX_CHANNEL_CNT ; i++ )
        {
            if (DmxEsHandle[i])
            {
                HI_DRV_FILE_Close(DmxEsHandle[i]);
                DmxEsHandle[i] = HI_NULL;
            }
        }
    	
    	SaveEsFlag = 0;
    	up(&SaveEsMutex);
	}
    
}

HI_DECLARE_MUTEX(DmxRamPortTsMutex);

static struct file *DmxRamPortTsHandle  = HI_NULL;
static HI_U32       DmxRamPortID         = 0;

HI_VOID DMX_OsrSaveIPTs(HI_U8 *buf, HI_U32 len,HI_U32 u32PortID)
{
	if (0 == down_interruptible(&DmxRamPortTsMutex))
    {	
        if (DmxRamPortTsHandle && (DmxRamPortID - HI_UNF_DMX_PORT_RAM_0) == u32PortID)
        {
            HI_DRV_FILE_Write(DmxRamPortTsHandle, buf, len);
        }
    	up(&DmxRamPortTsMutex);
	}
}

static HI_S32 DMX_OsrSaveIPTsStart(HI_U32 u32PortID)
{
    char str[DMX_FILE_NAME_LEN];

    if ((u32PortID < HI_UNF_DMX_PORT_RAM_0) || (u32PortID  > (HI_UNF_DMX_PORT_RAM_0 + DMX_RAMPORT_CNT)))
    {
        HI_ERR_DEMUX("invalid port id:%d\n",u32PortID);
        return HI_FAILURE;
    }

    if (HI_SUCCESS != HI_DRV_FILE_GetStorePath(str, DMX_FILE_NAME_LEN))
    {
        HI_ERR_DEMUX("get path failed\n");

        return HI_FAILURE;
    }
    DmxRamPortID = u32PortID;

 if (0 == down_interruptible(&DmxRamPortTsMutex))
    {
        if (DmxRamPortTsHandle == HI_NULL)
        {
            snprintf(str, sizeof(str),"%s/dmx_ram_%u.ts", str, u32PortID);

            DmxRamPortTsHandle = HI_DRV_FILE_Open(str, 1);
            if (!DmxRamPortTsHandle)
            {
                HI_ERR_DEMUX("open %s error\n", str);
    			up(&DmxRamPortTsMutex);
                return HI_FAILURE;
            }
        }
    	up(&DmxRamPortTsMutex);
	}

    return HI_SUCCESS;
}

static HI_VOID DMX_OsrSaveIPTsStop(HI_VOID)
{
if (0 == down_interruptible(&DmxRamPortTsMutex))
    {
    if (DmxRamPortTsHandle)
    {
        HI_DRV_FILE_Close(DmxRamPortTsHandle);
        DmxRamPortTsHandle = HI_NULL;
        DmxRamPortID = 0;
    }
	 up(&DmxRamPortTsMutex);
	}
}

#define DMX_ALLTS_DMXID (4)
#define DMX_DMXTS_DMXID (4)

#define DMX_INVALID_REC_ID  0xFFFF

static struct file         *DmxAllTsHandle  = HI_NULL;
static struct task_struct  *DmxAllTsThread  = HI_NULL;
static HI_U32               DmxAllTsRecId   = DMX_INVALID_REC_ID;

static HI_S32 HI_DMX_SaveAllTS_Routine(HI_VOID *arg)
{
    while (1)
    {
        HI_U32  RecId   = DmxAllTsRecId;
        HI_U32  PhyAddr;
        HI_U32  KerAddr;
        HI_U32  Len;

        if (kthread_should_stop())
        {
            break;
        }

        if (DMX_INVALID_REC_ID == RecId)
        {
            continue;
        }

        if (HI_SUCCESS == DMX_DRV_REC_AcquireRecData(RecId, &PhyAddr, &KerAddr, &Len, 500))
        {
            if (DmxAllTsHandle)
            {
                HI_DRV_FILE_Write(DmxAllTsHandle, (HI_S8*)KerAddr, Len);
            }

            DMX_DRV_REC_ReleaseRecData(RecId, PhyAddr, Len);

        }

        yield();
    }

    return 0;
}

static HI_S32 DMX_OsrSaveALLTs_Start(HI_U32 PortId)
{
    HI_S32                  ret;
    DMX_PORT_MODE_E         PortMode;
    HI_UNF_DMX_REC_ATTR_S   RecAttr;
    HI_U32                  PhyAddr;
    HI_U32                  BufSize;
    HI_CHAR                 FileName[DMX_FILE_NAME_LEN] = {0};
	DMX_REC_TIMESTAMP_MODE_E enRecTimeStamp = DMX_REC_TIMESTAMP_NONE;

    ret = HI_DRV_FILE_GetStorePath(FileName, DMX_FILE_NAME_LEN);
    if (HI_SUCCESS != ret)
    {
        HI_ERR_DEMUX("get path failed\n");

        return ret;
    }


    if (HI_NULL != DmxAllTsHandle)
    {
        HI_ERR_DEMUX("already started\n");

        return HI_FAILURE;
    }

    snprintf(FileName, sizeof(FileName),"%s/dmx_allts_%u.ts", FileName, PortId);

    DmxAllTsHandle = HI_DRV_FILE_Open(FileName, 1);
    if (!DmxAllTsHandle)
    {
        HI_ERR_DEMUX("open %s error\n", FileName);

        goto exit;
    }

    if (PortId >= HI_UNF_DMX_PORT_RAM_0)
    {
        PortMode = DMX_PORT_MODE_RAM;

        PortId -= HI_UNF_DMX_PORT_RAM_0;
    }
    else
    {
        PortMode = DMX_PORT_MODE_TUNER;
    }

    ret = DMX_OsiAttachPort(DMX_ALLTS_DMXID, PortMode, PortId);
    if (HI_SUCCESS != ret)
    {
        HI_ERR_DEMUX("AttachPort failed 0x%x\n", ret);

        goto exit;
    }

    RecAttr.u32DmxId        = DMX_ALLTS_DMXID;
    RecAttr.u32RecBufSize   = 0x400000;
    RecAttr.enRecType       = HI_UNF_DMX_REC_TYPE_ALL_PID;

    ret = DMX_DRV_REC_CreateChannel(&RecAttr, enRecTimeStamp,&DmxAllTsRecId, &PhyAddr, &BufSize);
    if (HI_SUCCESS != ret)
    {
        HI_ERR_DEMUX("open rec failed 0x%x\n", ret);

        goto exit;
    }

    ret = DMX_DRV_REC_StartRecChn(DmxAllTsRecId);
    if (HI_SUCCESS != ret)
    {
        HI_ERR_DEMUX("start rec failed 0x%x\n", ret);

        goto exit;
    }

    DmxAllTsThread = kthread_create(HI_DMX_SaveAllTS_Routine, HI_NULL, "SaveAllTs");
    if (!DmxAllTsThread)
    {
        HI_ERR_DEMUX("create kthread failed\n");

        goto exit;
    }

    wake_up_process(DmxAllTsThread);


    return HI_SUCCESS;

exit :
    if (DMX_INVALID_REC_ID != DmxAllTsRecId)
    {
        DMX_DRV_REC_StopRecChn(DmxAllTsRecId);

        DMX_DRV_REC_DestroyChannel(DmxAllTsRecId);

        DmxAllTsRecId = DMX_INVALID_REC_ID;
    }

    DMX_OsiDetachPort(DMX_ALLTS_DMXID);

    if (DmxAllTsHandle)
    {
        HI_DRV_FILE_Close(DmxAllTsHandle);
        DmxAllTsHandle = HI_NULL;
    }

    return HI_FAILURE;
}

static HI_VOID DMX_OsrSaveALLTs_Stop(HI_VOID)
{
    if (DmxAllTsHandle)
    {
        kthread_stop(DmxAllTsThread);
        DmxAllTsThread = HI_NULL;

        DMX_DRV_REC_StopRecChn(DmxAllTsRecId);

        DMX_DRV_REC_DestroyChannel(DmxAllTsRecId);

        DmxAllTsRecId = DMX_INVALID_REC_ID;

        DMX_OsiDetachPort(DMX_ALLTS_DMXID);

        HI_DRV_FILE_Close(DmxAllTsHandle);
        DmxAllTsHandle = HI_NULL;

    }
}

static struct task_struct  *DmxRecTsThread  = HI_NULL;
static struct file         *DmxRecTsHandle  = HI_NULL;
static HI_U32               DmxTsRecId      = DMX_INVALID_REC_ID;

static HI_S32 HI_DMX_SaveDmxTS_Routine(HI_VOID *arg)
{
    while (1)
    {
        HI_U32  RecId   = DmxTsRecId;
        HI_U32  PhyAddr;
        HI_U32  KerAddr;
        HI_U32  Len;

        if (kthread_should_stop())
        {
            break;
        }

        if (DMX_INVALID_REC_ID == RecId)
        {
            continue;
        }

        if (HI_SUCCESS == DMX_DRV_REC_AcquireRecData(RecId, &PhyAddr, &KerAddr, &Len, 500))
        {
            if (DmxRecTsHandle)
            {
                HI_DRV_FILE_Write(DmxRecTsHandle, (HI_S8*)KerAddr, Len);
            }

            DMX_DRV_REC_ReleaseRecData(RecId, PhyAddr, Len);
        }

        yield();
    }

    return 0;
}

static HI_S32 DMX_OsrSaveDmxTs_Start(HI_U32 DmxId)
{
    HI_S32                  ret;
    HI_UNF_DMX_REC_ATTR_S   RecAttr;
    HI_U32                  PhyAddr;
    HI_U32                  BufSize;
    HI_CHAR                 FileName[DMX_FILE_NAME_LEN] = {0};
	DMX_REC_TIMESTAMP_MODE_E enRecTimeStamp = DMX_REC_TIMESTAMP_NONE;

    ret = HI_DRV_FILE_GetStorePath(FileName, DMX_FILE_NAME_LEN);
    if (HI_SUCCESS != ret)
    {
        HI_ERR_DEMUX("get path failed\n");

        return HI_FAILURE;
    }
    
    if (HI_NULL != DmxRecTsHandle)
    {
        HI_ERR_DEMUX("already started\n");

        return HI_FAILURE;
    }

    ret = DMX_OsiSaveDmxTs_Start(DmxId, DMX_DMXTS_DMXID);
    if (HI_SUCCESS != ret)
    {
        HI_ERR_DEMUX("ts start failed\n");

        return HI_FAILURE;
    }

    snprintf(FileName,sizeof(FileName), "%s/dmx_rects_%u.ts", FileName, DmxId);

    DmxRecTsHandle = HI_DRV_FILE_Open(FileName, 1);
    if (!DmxRecTsHandle)
    {
        HI_ERR_DEMUX("open %s error\n", FileName);

        goto exit;
    }

    RecAttr.u32DmxId        = DMX_DMXTS_DMXID;
    RecAttr.enIndexType     = HI_UNF_DMX_REC_INDEX_TYPE_NONE;
    RecAttr.u32IndexSrcPid  = DMX_INVALID_PID;
    RecAttr.enVCodecType    = HI_UNF_VCODEC_TYPE_MPEG2;
    RecAttr.enRecType       = HI_UNF_DMX_REC_TYPE_SELECT_PID;
    RecAttr.u32RecBufSize   = 0x400000;

    ret = DMX_DRV_REC_CreateChannel(&RecAttr, enRecTimeStamp,&DmxTsRecId, &PhyAddr, &BufSize);
    if (HI_SUCCESS != ret)
    {
        HI_ERR_DEMUX("open rec failed 0x%x\n", ret);

        goto exit;
    }

    ret = DMX_DRV_REC_StartRecChn(DmxTsRecId);
    if (HI_SUCCESS != ret)
    {
        HI_ERR_DEMUX("start rec failed 0x%x\n", ret);

        goto exit;
    }

    DmxRecTsThread = kthread_create(HI_DMX_SaveDmxTS_Routine, HI_NULL, "SaveDmxTs");
    if (HI_NULL == DmxRecTsThread)
    {
        HI_ERR_DEMUX("create kthread failed\n");

        goto exit;
    }

    wake_up_process(DmxRecTsThread);

    return HI_SUCCESS;

exit :
    if (DMX_INVALID_REC_ID != DmxTsRecId)
    {
        DMX_DRV_REC_StopRecChn(DmxTsRecId);

        DMX_DRV_REC_DestroyChannel(DmxTsRecId);

        DmxTsRecId = DMX_INVALID_REC_ID;
    }

    if (DmxRecTsHandle)
    {
        HI_DRV_FILE_Close(DmxRecTsHandle);
        DmxRecTsHandle = HI_NULL;
    }

    DMX_OsiSaveDmxTs_Stop(DMX_DMXTS_DMXID);

    return HI_FAILURE;
}

static HI_VOID DMX_OsrSaveDmxTs_Stop(HI_VOID)
{
    if (DmxRecTsHandle)
    {
        kthread_stop(DmxRecTsThread);
        DmxRecTsThread = HI_NULL;

        DMX_DRV_REC_StopRecChn(DmxTsRecId);

        DMX_DRV_REC_DestroyChannel(DmxTsRecId);

        DmxTsRecId = DMX_INVALID_REC_ID;

        DMX_OsiSaveDmxTs_Stop(DMX_DMXTS_DMXID);

        HI_DRV_FILE_Close(DmxRecTsHandle);
        DmxRecTsHandle = HI_NULL;
    }

}

/* control function entry by proc file system */
HI_S32 DMX_OsrDebugCtrl(HI_U32 cmd,DMX_DEBUG_CMD_CTRl cmdctrl,HI_U32 param)
{
    HI_S32 ret;
    switch (cmd)
    {
        case DMX_DEBUG_CMD_SAVE_ES:
        {
            if (DMX_DEBUG_CMD_START == cmdctrl)
            {
                
                ret = DMX_OsrStartSaveEs();
                if (HI_SUCCESS == ret)
                {
                    printk("begin save es\n");
                }
            }
            else
            {
                printk("stop save es\n");
                DMX_OsrStopSaveEs();
            }

            break;
        }

        case DMX_DEBUG_CMD_SAVE_ALLTS:
        {
            if (DMX_DEBUG_CMD_STOP == cmdctrl)
            {
                printk("stop allts save\n");
                DMX_OsrSaveALLTs_Stop();
            }
            else
            {
                ret = DMX_OsrSaveALLTs_Start(param);
                if (HI_SUCCESS == ret)
                {
                    printk("begin save allts of port:%d\n",param);
                }                
            }

            break;
        }

        case DMX_DEBUG_CMD_SAVE_IPTS:
        {
            if (DMX_DEBUG_CMD_START == cmdctrl)
            {
                ret = DMX_OsrSaveIPTsStart(param);
                if (HI_SUCCESS == ret)
                {
                    printk("begin save ram port:%d ts\n",param);
                }
            }
            else
            {
                printk("stop save ip port ts\n");
                DMX_OsrSaveIPTsStop();
            }

            break;
        }

        case DMX_DEBUG_CMD_SAVE_DMXTS:
        {
            if (DMX_DEBUG_CMD_STOP == cmdctrl)
            {
                printk("stop save dmx ts\n");
                DMX_OsrSaveDmxTs_Stop();
            }
            else
            {
                ret = DMX_OsrSaveDmxTs_Start(param);
                if (HI_SUCCESS == ret)
                {
                    printk("begine save dmx:%d ts\n",param);
                }
            }

            break;
        }

        default:
            return HI_FAILURE;
    }

    return HI_SUCCESS;
}

#endif

