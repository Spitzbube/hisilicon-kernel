/******************************************************************************

  Copyright (C), 2010-2020, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : drv_global.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2013/7/20
  Description   :
  History       :
  Date          : 2013/7/20
  Author        : y00229039
  Modification  :
*******************************************************************************/

#ifndef __DRV_GLOBAL_H__
#define __DRV_GLOBAL_H__

#ifdef __cplusplus
 #if __cplusplus
extern "C" {
 #endif
#endif /* __cplusplus */

#include "hi_error_mpi.h"
//#include "mpi_priv_hdmi.h"
#include "hi_drv_hdmi.h"
#include "hi_drv_disp.h"
#include "hi_unf_hdmi.h"
#include "drv_hdmi.h"

//g_stHdmiCommParam  

typedef struct
{
    HI_BOOL     bOpenGreenChannel;        /* Green Channel */
    //HI_BOOL     bOnlySetAttr;             /*boot => mce : we only need set default param to attr(no config) and create thread*/
    HI_BOOL     bOpenedInBoot;             /*boot => mce : we only need set default param to attr(no config) and create thread*/
    //HI_BOOL     bReadEDIDOk;
    HI_U32      enDefaultMode;            /*init parameter*//*CNcomment: 初始化参数 */
    struct task_struct  *kThreadTimer;    /*timer thread*//*CNcomment:定时器线程 */
    HI_BOOL     kThreadTimerStop;
    struct task_struct  *kCECRouter;      /*CEC thread*//*CNcomment: CEC线程 */
#if 0 /*--NO MODIFY : COMMENT BY CODINGPARTNER--*/
    HI_BOOL     bHdmiStarted;             /*HDMI Start Flag*//*CNcomment:HDMI 接口是否启动标志 */
    HI_BOOL     bHdmiExit;                /*HDMI exit flag*//*CNcomment:HDMI 模块退出标志 */
#endif /*--NO MODIFY : COMMENT BY CODINGPARTNER--*/
    HI_UNF_HDMI_VIDEO_MODE_E enVidInMode; /*reservation,please setting VIDEO_MODE_YCBCR422 mode*//*CNcomment:保留，请配置为VIDEO_MODE_YCBCR422 */
}HDMI_COMM_ATTR_S;

//void DRV_Get_CommAttr(HDMI_COMM_ATTR_S **pstCommAttr);
HDMI_COMM_ATTR_S *DRV_Get_CommAttr(HI_VOID);




void DRV_PrintCommAttr(HI_VOID);

HI_UNF_HDMI_VIDEO_MODE_E DRV_Get_VIDMode(HI_VOID);
void DRV_Set_VIDMode(HI_UNF_HDMI_VIDEO_MODE_E enVInMode);


HI_S32 DRV_Get_IsGreenChannel(HI_VOID);
void DRV_Set_GreenChannel(HI_BOOL bGreen);

HI_S32 DRV_Get_IsOpenedInBoot(HI_VOID);
void DRV_Set_OpenedInBoot(HI_BOOL bOpened);

HI_S32 DRV_Get_DefHDMIMode(HI_VOID);
void DRV_Set_DefHDMIMode(HI_S32 defMod);

HI_S32 DRV_Get_IsThreadStoped(HI_VOID);
void DRV_Set_ThreadStop(HI_BOOL bStop);
//g_stHdmiCommParam end

#if 1 /*--NO MODIFY : COMMENT BY CODINGPARTNER--*/
#define MAX_PROCESS_NUM 10
#define PROC_EVENT_NUM 5

typedef struct
{
    HI_U32      bUsed;
    HI_U32      CurEventNo;
    HI_U32      Event[PROC_EVENT_NUM];
    HI_U32      u32ProcHandle;
}HDMI_PROC_EVENT_S;

typedef struct
{
    HI_BOOL            bOpen;
    HI_BOOL            bStart;
    HDMI_PROC_EVENT_S  eventList[MAX_PROCESS_NUM];
    //HI_U32           Event[5];        /*Current Event Array, sequence will be change */
    HDMI_ATTR_S        stHDMIAttr;          /*HDMI implement parameter*//*CNcomment:HDMI 运行参数 */
    HI_BOOL            ForceUpdateFlag;
    HI_BOOL            partUpdateFlag;
    HI_UNF_HDMI_AVI_INFOFRAME_VER2_S   stAVIInfoFrame;
    HI_UNF_HDMI_AUD_INFOFRAME_VER1_S   stAUDInfoFrame;

    HI_BOOL                            bCECStart;
    HI_U8                              u8CECCheckCount;
    HI_UNF_HDMI_CEC_STATUS_S           stCECStatus;
}HDMI_CHN_ATTR_S;
#endif /*--NO MODIFY : COMMENT BY CODINGPARTNER--*/

HDMI_CHN_ATTR_S *DRV_Get_ChnAttr(HI_VOID);

HI_U32 DRV_HDMI_SetDefaultAttr(HI_VOID);
HDMI_ATTR_S *DRV_Get_HDMIAttr(HI_UNF_HDMI_ID_E enHdmi);
HDMI_APP_ATTR_S   *DRV_Get_AppAttr(HI_UNF_HDMI_ID_E enHdmi);
HDMI_VIDEO_ATTR_S *DRV_Get_VideoAttr(HI_UNF_HDMI_ID_E enHdmi);
HDMI_AUDIO_ATTR_S *DRV_Get_AudioAttr(HI_UNF_HDMI_ID_E enHdmi);

HI_BOOL DRV_Get_IsNeedForceUpdate(HI_UNF_HDMI_ID_E enHdmi);
void DRV_Set_ForceUpdateFlag(HI_UNF_HDMI_ID_E enHdmi,HI_BOOL bupdate);

HI_BOOL DRV_Get_IsNeedPartUpdate(HI_UNF_HDMI_ID_E enHdmi);
void DRV_Set_PartUpdateFlag(HI_UNF_HDMI_ID_E enHdmi,HI_BOOL bupdate);

HDMI_PROC_EVENT_S *DRV_Get_EventList(HI_UNF_HDMI_ID_E enHdmi);
HI_UNF_HDMI_CEC_STATUS_S *DRV_Get_CecStatus(HI_UNF_HDMI_ID_E enHdmi);

HI_UNF_HDMI_AVI_INFOFRAME_VER2_S *DRV_Get_AviInfoFrm(HI_UNF_HDMI_ID_E enHdmi);
HI_UNF_HDMI_AUD_INFOFRAME_VER1_S *DRV_Get_AudInfoFrm(HI_UNF_HDMI_ID_E enHdmi);

HI_BOOL DRV_Get_IsChnOpened(HI_UNF_HDMI_ID_E enHdmi);
void DRV_Set_ChnOpen(HI_UNF_HDMI_ID_E enHdmi,HI_BOOL bChnOpen);

HI_BOOL DRV_Get_IsChnStart(HI_UNF_HDMI_ID_E enHdmi);
void DRV_Set_ChnStart(HI_UNF_HDMI_ID_E enHdmi,HI_BOOL bChnStart);

HI_BOOL DRV_Get_IsCECStart(HI_UNF_HDMI_ID_E enHdmi);
void DRV_Set_CECStart(HI_UNF_HDMI_ID_E enHdmi,HI_BOOL bCecStart);



#ifdef __cplusplus
 #if __cplusplus
}
 #endif
#endif /* __cplusplus */

#endif  /* __DRV_GLOBAL_H__ */

/*------------------------------------END-------------------------------------*/

