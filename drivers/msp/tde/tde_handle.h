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
    HI_S32   handle;
    HI_VOID *res;
} HI_HANDLE_MGR;

STATIC HI_HANDLE_MGR *s_pstTdeHandleList = NULL; /* Manager list of global handle */

STATIC HI_S32 s_handle = 1; /* handle=0, reserve */
STATIC HI_S32 s_s32HandleNum = 0;

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
STATIC HI_BOOL  initial_handle(HI_VOID)
{
    if (NULL == s_pstTdeHandleList)
    {
        s_pstTdeHandleList = (HI_HANDLE_MGR *) TDE_MALLOC(sizeof(HI_HANDLE_MGR));
        if (NULL == s_pstTdeHandleList)
        {
            return HI_FALSE;
        }

        /* Initialize list head */
        INIT_LIST_HEAD(&s_pstTdeHandleList->stHeader);

    }

    s_handle = 1;

    return HI_TRUE;
}
        

STATIC HI_S32  get_handle(HI_HANDLE_MGR *res, HI_S32 *handle)
{
    TDE_HANDLE_SAFE_INC_NUM();

    /* Jump over unlawful handle */
    if (TDE_MAX_HANDLE_VALUE == s_handle)
    {
        s_handle = 1;
    }

    res->handle = s_handle;
    list_add_tail(&res->stHeader, &s_pstTdeHandleList->stHeader);

    *handle = s_handle++;

    return 0;
}

/* To accelerate the speed of find, find form back to front */
STATIC HI_BOOL  query_handle(HI_S32 handle, HI_HANDLE_MGR **res)
{
    HI_HANDLE_MGR *pstHandle = list_entry(s_pstTdeHandleList->stHeader.prev, HI_HANDLE_MGR, stHeader);

    while (pstHandle != s_pstTdeHandleList)
    {
        if (pstHandle->handle == handle)
        {
            *res = pstHandle;
            return HI_TRUE;
        }

        pstHandle = list_entry(pstHandle->stHeader.prev, HI_HANDLE_MGR, stHeader);
    }

    return HI_FALSE;
}

STATIC HI_BOOL  release_handle(HI_S32 handle)
{
    HI_HANDLE_MGR *pstHandle = list_entry(s_pstTdeHandleList->stHeader.next, HI_HANDLE_MGR, stHeader);

    while (pstHandle != s_pstTdeHandleList)
    {
        if (pstHandle->handle == handle)
        {
            list_del_init(&pstHandle->stHeader);
            TDE_HANLE_SAFE_DEC_NUM();
            return HI_TRUE;
        }

        pstHandle = list_entry(pstHandle->stHeader.next, HI_HANDLE_MGR, stHeader);
    }

    return HI_FALSE;
}

STATIC HI_VOID destroy_handle(HI_VOID)
{
	/* Free head node, note: other nodes are all loaded, their resource  are responsibilited by its own module */
    if (NULL != s_pstTdeHandleList)
    {
        TDE_FREE(s_pstTdeHandleList);
        s_pstTdeHandleList = NULL;
    }

    return;
}

#ifdef __cplusplus
 #if __cplusplus
}
 #endif
#endif /* End of #ifdef __cplusplus */

#endif /* __HI_HANDLE_MGR_H__ */
