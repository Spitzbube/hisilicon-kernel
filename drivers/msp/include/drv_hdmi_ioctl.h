#ifndef __DRV_HDMI_IOCTL_H__
#define __DRV_HDMI_IOCTL_H__

//#include "hi_common_id.h"
#include "hi_module.h"
//#include "hi_common_log.h"
#include "hi_debug.h"

#include "hi_unf_hdmi.h"
#include "hi_error_mpi.h"
//#include "hi_mpi_hiao.h"
#include "hi_drv_hdmi.h"

#ifdef __cplusplus
#if __cplusplus
	extern "C"{
#endif
#endif

/*
** HDMI IOCTL Data Structure
*/
typedef struct hiHDMI_OPEN_S
{
	HI_UNF_HDMI_ID_E                  enHdmi;
    HI_UNF_HDMI_DEFAULT_ACTION_E      enDefaultMode;
    HI_U32         u32ProcID;
}HDMI_OPEN_S;

typedef struct hiHDMI_DEINIT_S
{
    HI_U32                          NoUsed;
}HDMI_DEINIT_S;

typedef struct hiHDMI_INIT_S
{
    HI_U32                          NoUsed;
}HDMI_INIT_S;

typedef struct hiHDMI_COLSE_S
{
    HI_UNF_HDMI_ID_E                enHdmi;
    HI_U32                          NoUsed;
}HDMI_CLOSE_S;

typedef struct hiHDMI_START_S
{
    HI_UNF_HDMI_ID_E                enHdmi;
    HI_U32                          NoUsed;
}HDMI_START_S;

typedef struct hiHDMI_STOP_S
{
    HI_UNF_HDMI_ID_E                enHdmi;
    HI_U32                          NoUsed;
}HDMI_STOP_S;

typedef struct hiHDMI_POLL_EVENT_S
{
    HI_UNF_HDMI_ID_E                enHdmi;
    HI_UNF_HDMI_EVENT_TYPE_E        Event;
    HI_U32                          u32ProcID;
}HDMI_POLL_EVENT_S;

typedef struct hiHDMI_SINK_CAPABILITY_S
{
    HI_UNF_HDMI_ID_E                enHdmi;
    HI_UNF_HDMI_SINK_CAPABILITY_S   SinkCap;
    int fill[528]; //???
    //2512 = 0x9d0????
}HDMI_SINK_CAPABILITY_S;

typedef struct hiHDMI_PORT_ATTR_S
{
    HI_UNF_HDMI_ID_E                enHdmi;
    HDMI_APP_ATTR_S                 stHdmiAppAttr;
}HDMI_PORT_ATTR_S;

typedef struct hiHDMI_INFORFRAME_S
{
    HI_UNF_HDMI_ID_E                enHdmi;
    HI_UNF_HDMI_INFOFRAME_TYPE_E    enInfoFrameType;
    HI_UNF_HDMI_INFOFRAME_S         InfoFrame;
}HDMI_INFORFRAME_S;

typedef struct hiHDMI_AVMUTE_S
{
    HI_UNF_HDMI_ID_E                enHdmi;
    HI_U32                          AVMuteEnable;
}HDMI_AVMUTE_S;

typedef struct hiHDMI_VIDEOTIMING_S
{
    HI_UNF_HDMI_ID_E                enHdmi;
    HI_U32                          VideoTiming;
    HI_DRV_DISP_STEREO_E            enStereo;
}HDMI_VIDEOTIMING_S;

typedef struct hiHDMI_PREVIDEOTIMING_S
{
    HI_UNF_HDMI_ID_E                enHdmi;
    HI_U32                          VideoTiming;
}HDMI_PREVIDEOTIMING_S;

typedef struct hiHDMI_DEEPCOLOR_S
{
    HI_UNF_HDMI_ID_E                enHdmi;
    HI_UNF_HDMI_DEEP_COLOR_E        enDeepColor;
}HDMI_DEEPCOLORC_S;

typedef struct hiHDMI_SET_XVYCC_S
{
    HI_UNF_HDMI_ID_E                enHdmi;
    HI_U32                          xvYCCEnable;
}HDMI_SET_XVYCC_S;

typedef struct hiHDMI_CEC_S
{
    HI_UNF_HDMI_ID_E                enHdmi;
    HI_UNF_HDMI_CEC_CMD_S           CECCmd;
    HI_U32                          timeout;
}HDMI_CEC_S;


typedef struct
{
    HI_UNF_HDMI_ID_E                enHdmi;
    HI_UNF_HDMI_CEC_STATUS_S        stStatus;
}HDMI_CEC_STATUS;

typedef struct hiHDMI_EDID_S
{
    HI_UNF_HDMI_ID_E                enHdmi;
    HI_U8                           u8EdidValid;
    HI_U32                          u32Edidlength;
    HI_U8                           u8Edid[512];
    //524
}HDMI_EDID_S;

typedef struct hiHDMI_PLAYSTAUS_S
{
    HI_UNF_HDMI_ID_E                enHdmi;
    HI_U32                          u32PlayStaus;
}HDMI_PLAYSTAUS_S;

typedef struct hiHDMI_CEC_ENABLE_S
{
    HI_U32                          NoUsed;
}HDMI_CEC_ENABlE_S;

typedef struct hiHDMI_CEC_DISABLE_S
{
    HI_U32                          NoUsed;
}HDMI_CEC_DISABLE_S;

typedef struct hiHDMI_REGCALLBACKFUNC_S
{
    HI_UNF_HDMI_ID_E                enHdmi;
	HI_U32							u32CallBackAddr;
}HDMI_REGCALLBACKFUNC_S;

typedef struct hiHDMI_LOADKEY_S
{
	HI_UNF_HDMI_ID_E               enHdmi;
	HI_UNF_HDMI_LOAD_KEY_S         stLoadKey;
}HDMI_LOADKEY_S;

typedef struct hiHDMI_GETPROCID_S
{
	HI_UNF_HDMI_ID_E               enHdmi;
	HI_U32         u32ProcID;
}HDMI_GET_PROCID_S;

typedef struct hiHDMI_GETAOATTR_S
{
	HI_UNF_HDMI_ID_E               enHdmi;
	HDMI_AUDIO_ATTR_S              stAudAttr;
}HDMI_GETAOATTR_S;

typedef struct hiHDMI_STATUS_S
{
	HI_UNF_HDMI_ID_E               enHdmi; //????
	HI_UNF_HDMI_STATUS_S           stStatus; //????
	int fill[3];
	//24
}HDMI_STATUS_S;

typedef struct hiHDMI_DELAY_S
{
	HI_UNF_HDMI_ID_E               enHdmi; //???
	HI_UNF_HDMI_DELAY_S            stDelay; //????
	//???
}HDMI_DELAY_S;

enum hiIOCTL_HDMI_E
{
    IOCTL_HDMI_INIT = 0X01,
    IOCTL_HDMI_DEINIT, //2
    IOCTL_HDMI_OPEN, //3
    IOCTL_HDMI_CLOSE, //4
    IOCTL_HDMI_START, //5
    IOCTL_HDMI_STOP, //6
    IOCTL_HDMI_SINK_CAPABILITY, //7
    IOCTL_HDMI_POLL_EVENT, //8
    IOCTL_HDMI_GET_ATTR, //9
    IOCTL_HDMI_SET_ATTR, //0x0a
    IOCTL_HDMI_GET_INFORFRAME, //0x0b
    IOCTL_HDMI_SET_INFORFRAME, //0x0c
    IOCTL_HDMI_AVMUTE, //0x0d
    IOCTL_HDMI_VIDEO_TIMING, //0x0e
    IOCTL_HDMI_GET_DEEPCOLOR, //0x0f
    IOCTL_HDMI_SET_DEEPCOLOR, //0x10
    IOCTL_HDMI_XVYCC, //0x11
    IOCTL_HDMI_SET_CEC, //0x12
    IOCTL_HDMI_GET_CEC, //0x13
    IOCTL_HDMI_CECSTATUS, //0x14
    IOCTL_HDMI_PRE_VIDEO_TIMING, //0x15
    IOCTL_HDMI_FORCE_GET_EDID, //0x16
    IOCTL_HDMI_GET_HDMI_PLAYSTAUS, //0x17
    IOCTL_HDMI_CEC_ENABLE, //0x18
    IOCTL_HDMI_CEC_DISABLE, //0x19
    IOCTL_HDMI_REG_CALLBACK_FUNC, //0x1a
    IOCTL_HDMI_LOADKEY, //0x1b
    IOCTL_HDMI_GET_PROCID, //0x1c
    IOCTL_HDMI_RELEASE_PROCID, //0x1d
    IOCTL_HDMI_GET_AO_ATTR, //0x1e
    IOCTL_HDMI_AUDIO_CHANGE, //0x1f
    IOCTL_HDMI_GET_STATUS, //0x20
    IOCTL_HDMI_GET_DELAY, //0x21
    IOCTL_HDMI_SET_DELAY, //0x22
    IOCTL_HDMI_MAX,
};

/*
**IOCTRL Between User/Kernel Level
*/
#define CMD_HDMI_INIT                _IOWR(HI_ID_HDMI, IOCTL_HDMI_INIT,           HDMI_INIT_S)
#define CMD_HDMI_DEINIT              _IOWR(HI_ID_HDMI, IOCTL_HDMI_DEINIT,         HDMI_DEINIT_S)
#define CMD_HDMI_OPEN                _IOWR(HI_ID_HDMI, IOCTL_HDMI_OPEN,           HDMI_OPEN_S)
#define CMD_HDMI_CLOSE               _IOWR(HI_ID_HDMI, IOCTL_HDMI_CLOSE,          HDMI_CLOSE_S)
#define CMD_HDMI_START               _IOWR(HI_ID_HDMI, IOCTL_HDMI_START,          HDMI_START_S)
#define CMD_HDMI_STOP                _IOWR(HI_ID_HDMI, IOCTL_HDMI_STOP,           HDMI_STOP_S)
#define CMD_HDMI_SINK_CAPABILITY     _IOWR(HI_ID_HDMI, IOCTL_HDMI_SINK_CAPABILITY,HDMI_SINK_CAPABILITY_S)
#define CMD_HDMI_POLL_EVENT          _IOWR(HI_ID_HDMI, IOCTL_HDMI_POLL_EVENT,     HDMI_POLL_EVENT_S)
#define CMD_HDMI_GET_ATTR            _IOWR(HI_ID_HDMI, IOCTL_HDMI_GET_ATTR,       HDMI_PORT_ATTR_S)
#define CMD_HDMI_SET_ATTR            _IOWR(HI_ID_HDMI, IOCTL_HDMI_SET_ATTR,       HDMI_PORT_ATTR_S)
#define CMD_HDMI_GET_INFORFRAME      _IOWR(HI_ID_HDMI, IOCTL_HDMI_GET_INFORFRAME, HDMI_INFORFRAME_S)
#define CMD_HDMI_SET_INFORFRAME      _IOWR(HI_ID_HDMI, IOCTL_HDMI_SET_INFORFRAME, HDMI_INFORFRAME_S)
#define CMD_HDMI_SET_AVMUTE          _IOWR(HI_ID_HDMI, IOCTL_HDMI_AVMUTE,         HDMI_AVMUTE_S)
#define CMD_HDMI_VIDEO_TIMING        _IOWR(HI_ID_HDMI, IOCTL_HDMI_VIDEO_TIMING,   HDMI_VIDEOTIMING_S)
#define CMD_HDMI_GET_DEEPCOLOR       _IOWR(HI_ID_HDMI, IOCTL_HDMI_GET_DEEPCOLOR,  HDMI_DEEPCOLORC_S)
#define CMD_HDMI_SET_DEEPCOLOR       _IOWR(HI_ID_HDMI, IOCTL_HDMI_SET_DEEPCOLOR,  HDMI_DEEPCOLORC_S)
#define CMD_HDMI_SET_XVYCC           _IOWR(HI_ID_HDMI, IOCTL_HDMI_XVYCC,          HDMI_SET_XVYCC_S)
#define CMD_HDMI_GET_CEC             _IOWR(HI_ID_HDMI, IOCTL_HDMI_GET_CEC,        HDMI_CEC_S)
#define CMD_HDMI_SET_CEC             _IOWR(HI_ID_HDMI, IOCTL_HDMI_SET_CEC,        HDMI_CEC_S)
#define CMD_HDMI_CECSTATUS           _IOWR(HI_ID_HDMI, IOCTL_HDMI_CECSTATUS,      HDMI_CEC_STATUS)
#define CMD_HDMI_PREVTIMING          _IOWR(HI_ID_HDMI, IOCTL_HDMI_PRE_VIDEO_TIMING,    HDMI_PREVIDEOTIMING_S)
#define CMD_HDMI_FORCE_GET_EDID      _IOWR(HI_ID_HDMI, IOCTL_HDMI_FORCE_GET_EDID,      HDMI_EDID_S) 
#define CMD_HDMI_GET_HDMI_PLAYSTAUS  _IOWR(HI_ID_HDMI, IOCTL_HDMI_GET_HDMI_PLAYSTAUS,  HDMI_PLAYSTAUS_S) 
#define CMD_HDMI_CEC_ENABLE          _IOWR(HI_ID_HDMI, IOCTL_HDMI_CEC_ENABLE,     HDMI_CEC_ENABlE_S) 
#define CMD_HDMI_CEC_DISABLE         _IOWR(HI_ID_HDMI, IOCTL_HDMI_CEC_DISABLE,    HDMI_CEC_DISABLE_S) 
#define CMD_HDMI_REG_CALLBACK_FUNC   _IOWR(HI_ID_HDMI, IOCTL_HDMI_REG_CALLBACK_FUNC, HDMI_REGCALLBACKFUNC_S) 
#define CMD_HDMI_LOADKEY             _IOWR(HI_ID_HDMI, IOCTL_HDMI_LOADKEY, HDMI_LOADKEY_S)
#define CMD_HDMI_GET_PROCID          _IOWR(HI_ID_HDMI, IOCTL_HDMI_GET_PROCID, HDMI_GET_PROCID_S)
#define CMD_HDMI_RELEASE_PROCID      _IOWR(HI_ID_HDMI, IOCTL_HDMI_RELEASE_PROCID, HDMI_GET_PROCID_S)
#define CMD_HDMI_GET_AO_ATTR         _IOWR(HI_ID_HDMI, IOCTL_HDMI_GET_AO_ATTR, HDMI_GETAOATTR_S)
#define CMD_HDMI_AUDIO_CHANGE        _IOWR(HI_ID_HDMI, IOCTL_HDMI_AUDIO_CHANGE, HDMI_GETAOATTR_S)
#define CMD_HDMI_GET_STATUS          _IOWR(HI_ID_HDMI, IOCTL_HDMI_GET_STATUS, HDMI_STATUS_S)
#define CMD_HDMI_GET_DELAY           _IOWR(HI_ID_HDMI, IOCTL_HDMI_GET_DELAY, HDMI_DELAY_S)
#define CMD_HDMI_SET_DELAY           _IOWR(HI_ID_HDMI, IOCTL_HDMI_SET_DELAY, HDMI_DELAY_S)


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

