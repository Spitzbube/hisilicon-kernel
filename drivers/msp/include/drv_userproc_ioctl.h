/******************************************************************************

  Copyright (C), 2013-2023, Hisilicon Tech. Co., Ltd.

******************************************************************************
  File Name     : drv_userproc_ioctl.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2013/07/12
  Description   : Support user proc function.
  History       :
  1.Date        : 2013/07/12
    Author      : l00185424
    Modification: Created file

******************************************************************************/

#ifndef __DRV_USERPROC_IOCTL_H__
#define __DRV_USERPROC_IOCTL_H__

/******************************* Include Files *******************************/

/* add include here */
#include "hi_type.h"
#ifdef __KERNEL__
#include "hi_drv_mmz.h"
#endif
#include "hi_drv_userproc.h"

#ifdef __cplusplus
 #if __cplusplus
extern "C" {
 #endif
#endif /* __cplusplus */

/***************************** Macro Definition ******************************/

#define HI_UPROC_READ_CMD "__read"

/*************************** Structure Definition ****************************/

#ifndef __KERNEL__
typedef struct hiMMZ_BUFFER_S
{
    HI_U32 u32StartVirAddr;
    HI_U32 u32StartPhyAddr;
    HI_U32 u32Size;
}MMZ_BUFFER_S;
#endif

typedef struct USRMODEPROC_ENTRY_S
{
    HI_CHAR aszName[MAX_PROC_NAME_LEN + 1];     /* Input, entry name */
    HI_CHAR aszParent[MAX_PROC_NAME_LEN + 1];   /* Input, directory name */
    HI_PROC_SHOW_FN pfnShowFunc;                /* Input, show function */
    HI_PROC_CMD_FN pfnCmdFunc;                  /* Input, cmd function */
    HI_VOID * pPrivData;                         /* Input, private data*/
    HI_VOID *pEntry;                            /* Output, entry pointer */
    HI_VOID *pFile;                             /* Output, Belongs to which file */
    MMZ_BUFFER_S stBuf;                         /* Output, buffer */
}HI_DRV_USRMODEPROC_ENTRY_S;

typedef struct USRMODEPROC_CMD_S
{
    HI_VOID     *pEntry;                        /* The type is HI_DRV_USRMODEPROC_ENTRY_S* */
    HI_S32      s32Write;
    HI_CHAR     aszCmd[MAX_PROC_CMD_LEN + 1];
}HI_DRV_USRMODEPROC_CMD_S;

typedef struct USRMODEPROC_CMDINFO_S
{
    HI_DRV_USRMODEPROC_CMD_S stCmd;
    HI_DRV_USRMODEPROC_ENTRY_S stEntry;
}HI_DRV_USRMODEPROC_CMDINFO_S;

typedef char HI_PROC_DIR_NAME_S[MAX_PROC_NAME_LEN+1];

/********************** Global Variable declaration **************************/

/******************************* API declaration *****************************/

#define	UMPIOC_ADD_ENTRY		           _IOWR(HI_ID_PROC, 1, HI_DRV_USRMODEPROC_ENTRY_S)
#define	UMPIOC_REMOVE_ENTRY		    _IOW(HI_ID_PROC, 2, HI_DRV_USRMODEPROC_ENTRY_S)
#define	UMPIOC_ADD_DIR		           _IOW(HI_ID_PROC, 3, HI_PROC_DIR_NAME_S)
#define	UMPIOC_REMOVE_DIR                 _IOW(HI_ID_PROC, 4, HI_PROC_DIR_NAME_S)
#define	UMPIOC_GETCMD   		           _IOR(HI_ID_PROC, 5, HI_DRV_USRMODEPROC_CMDINFO_S)
#define	UMPIOC_WAKE_READ_TASK   	     _IOW(HI_ID_PROC, 6, HI_S32)
#define	UMPIOC_WAKE_WRITE_TASK        _IOW(HI_ID_PROC, 7, HI_S32)

#ifdef __cplusplus
 #if __cplusplus
}
 #endif
#endif /* __cplusplus */

#endif /* __DRV_USERPROC_IOCTL_H__ */


