/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_drv_stat.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2008/12/17
  Description   :
  History       :
  1.Date        : 2008/12/17
    Author      : z42136
    Modification: Created file

******************************************************************************/

#ifndef __HI_DRV_STAT_H__
#define __HI_DRV_STAT_H__

#include "hi_type.h"

#ifdef __cplusplus
 #if __cplusplus
extern "C" {
 #endif
#endif /* __cplusplus */

#define HI_FATAL_STAT(fmt...) HI_FATAL_PRINT(HI_ID_STAT, fmt)
#define HI_ERR_STAT(fmt...) HI_ERR_PRINT(HI_ID_STAT, fmt)
#define HI_WARN_STAT(fmt...) HI_WARN_PRINT(HI_ID_STAT, fmt)
#define HI_INFO_STAT(fmt...) HI_INFO_PRINT(HI_ID_STAT, fmt)

typedef enum
{
    STAT_ISR_AUDIO = 0x0,
    STAT_ISR_VIDEO,
    STAT_ISR_DEMUX,
    STAT_ISR_SYNC,
    STAT_ISR_VO,
    STAT_ISR_TDE,

    STAT_ISR_BUTT
}STAT_ISR_E;

typedef enum tagSTAT_EVENT
{
    STAT_EVENT_KEYIN,
    STAT_EVENT_KEYOUT,
    STAT_EVENT_ASTOP,
    STAT_EVENT_VSTOP,
    STAT_EVENT_CONNECT,
    STAT_EVENT_LOCKED,
    STAT_EVENT_ASTART,
    STAT_EVENT_VSTART,
    STAT_EVENT_CWSET,
    STAT_EVENT_STREAMIN,
    STAT_EVENT_ISTREAMGET,
    STAT_EVENT_IFRAMEOUT,
    STAT_EVENT_VPSSGETFRM,
    STAT_EVENT_VPSSOUTFRM,
    STAT_EVENT_AVPLAYGETFRM,    
	STAT_EVENT_PRESYNC,
    STAT_EVENT_BUFREADY,
    STAT_EVENT_FRAMESYNCOK,
	STAT_EVENT_VOGETFRM,
    STAT_EVENT_IFRAMEINTER,
    STAT_EVENT_BUTT
    
}STAT_EVENT_E;


typedef HI_VOID (*STAT_Event_Fun)(STAT_EVENT_E, HI_U32);

HI_S32  HI_DRV_STAT_EventFunc_Register(HI_VOID* pFunc);
HI_VOID HI_DRV_STAT_EventFunc_UnRegister(HI_VOID);
HI_S32  HI_DRV_STAT_KInit(HI_VOID);
HI_VOID HI_DRV_STAT_KExit(HI_VOID);

HI_S32  HI_DRV_STAT_Init(HI_VOID);
HI_VOID HI_DRV_STAT_Exit(HI_VOID);

/*interrupt cost time in kernel-state*/
/*CNcomment:内核态中断耗时统计*/
#if defined(HI_STAT_ISR_SUPPORTED)
HI_VOID HI_DRV_STAT_IsrReset(HI_VOID);
HI_VOID HI_DRV_STAT_IsrEnable(HI_VOID);
HI_VOID HI_DRV_STAT_IsrDisable(HI_VOID);

HI_VOID HI_DRV_STAT_IsrBegin(STAT_ISR_E isr);
HI_VOID HI_DRV_STAT_IsrEnd(STAT_ISR_E isr);
#endif

HI_VOID HI_DRV_STAT_Event(STAT_EVENT_E enEvent, HI_U32 Value);
HI_U32  HI_DRV_STAT_GetTick(HI_VOID);


#ifdef __cplusplus
 #if __cplusplus
}
 #endif
#endif /* __cplusplus */

#endif /* __HI_DRV_STAT_H__ */

