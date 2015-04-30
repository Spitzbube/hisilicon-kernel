/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

******************************************************************************
  File Name             :   handle.h
  Version               :   Initial Draft
  Author                :   Hisilicon multimedia software group
  Created               :   2008/03/05
  Last Modified         :
  Description           :
  Function List         :
  History               :
  1.Date                :   2008/03/05
    Author              :   w54130
Modification            :   Created file
******************************************************************************/
#ifndef  __HI_HANDLE_MGR_H__
#define  __HI_HANDLE_MGR_H__

#include <linux/list.h>
#include <linux/mm.h>
#include "tde_define.h"

#ifdef __cplusplus
 #if __cplusplus
extern "C" {
 #endif
#endif /* End of #ifdef __cplusplus */

typedef struct hiHandleMgr
{
    struct list_head stHeader;  /* Use to organiza handle */
    spinlock_t lock; //8
    HI_S32   handle;
    HI_VOID *res; //16
} HI_HANDLE_MGR;

//STATIC HI_HANDLE_MGR *s_pstTdeHandleList = NULL; /* Manager list of global handle */

//STATIC HI_S32 s_handle = 1; /* handle=0, reserve */

#define TDE_MAX_JOB_NUM 512
#define TDE_MAX_HANDLE_VALUE 0x7fffffff

#define TDE_HANDLE_SAFE_INC_NUM() \
    do{\
        if (s_s32HandleNum >= TDE_MAX_JOB_NUM)\
        {\
            return -1;\
        }\
        else\
        {\
            s_s32HandleNum++;\
        }\
    } while(0)

#define TDE_HANLE_SAFE_DEC_NUM()\
    do{\
        if (--s_s32HandleNum < 0)\
        {\
            s_s32HandleNum = 0;\
        }\
    }while(0)

#define TDE_IS_JOB_FULL() (s_s32HandleNum == TDE_MAX_JOB_NUM)
/*STATIC*/ HI_BOOL  initial_handle(HI_VOID);
        

/*STATIC*/ HI_S32  get_handle(HI_HANDLE_MGR *res, HI_S32 *handle);

/* To accelerate the speed of find, find form back to front */
/*STATIC*/ HI_BOOL  query_handle(HI_S32 handle, HI_HANDLE_MGR **res);

/*STATIC*/ HI_BOOL  release_handle(HI_S32 handle);

/*STATIC*/ HI_VOID destroy_handle(HI_VOID);

HI_S32 get_handlenum(HI_VOID);

HI_VOID TdeFreePendingJob(HI_VOID);

#ifdef __cplusplus
 #if __cplusplus
}
 #endif
#endif /* End of #ifdef __cplusplus */

#endif /* __HI_HANDLE_MGR_H__ */
