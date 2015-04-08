#ifndef TDE_BOOT
#include <linux/list.h>
#include <linux/mm.h>
#else
#include "list.h"
#endif
#include <linux/vmalloc.h>
#include "tde_osilist.h"
#include "tde_define.h"
#include "tde_handle.h"
#ifdef __cplusplus
 #if __cplusplus
extern "C" {
 #endif  /* __cplusplus */
#endif  /* __cplusplus */

STATIC HI_HANDLE_MGR *s_pstTdeHandleList = NULL; /* Manager list of global handle */

STATIC HI_S32 s_handle = 1; /* handle=0, reserve */
#ifndef TDE_BOOT
STATIC spinlock_t s_handlelock;
#endif

 HI_BOOL  initial_handle(HI_VOID)
{
    if (NULL == s_pstTdeHandleList)
    {
        s_pstTdeHandleList = (HI_HANDLE_MGR *) vmalloc(sizeof(HI_HANDLE_MGR));
        if (NULL == s_pstTdeHandleList)
        {
            return HI_FALSE;
        }

        /* Initialize list head */
        #ifndef TDE_BOOT
        INIT_LIST_HEAD(&s_pstTdeHandleList->stHeader);
        spin_lock_init(&s_pstTdeHandleList->lock);
        spin_lock_init(&s_handlelock);
        #endif
    }

    s_handle = 1;

    return HI_TRUE;
}


 HI_S32  get_handle(HI_HANDLE_MGR *res, HI_S32 *handle)
{
#ifndef TDE_BOOT
    HI_SIZE_T lockflags;
    HI_SIZE_T handlockflags;
#endif
    TDE_LOCK(&s_handlelock, handlockflags);
    /* Jump over unlawful handle */
    if (TDE_MAX_HANDLE_VALUE == s_handle)
    {
        s_handle = 1;
    }
    res->handle = s_handle;
    *handle = s_handle++;
    TDE_UNLOCK(&s_handlelock, handlockflags);
    TDE_LOCK(&s_pstTdeHandleList->lock,lockflags);
    list_add_tail(&res->stHeader, &s_pstTdeHandleList->stHeader);
    TDE_UNLOCK(&s_pstTdeHandleList->lock,lockflags);
}

/* To accelerate the speed of find, find form back to front */
 HI_BOOL  query_handle(HI_S32 handle, HI_HANDLE_MGR **res)
{
#ifndef TDE_BOOT
    HI_SIZE_T lockflags;
#endif
    HI_HANDLE_MGR *pstHandle =NULL;
    TDE_LOCK(&s_pstTdeHandleList->lock,lockflags);
    pstHandle = list_entry(s_pstTdeHandleList->stHeader.prev, HI_HANDLE_MGR, stHeader);
    while (pstHandle != s_pstTdeHandleList)
    {
        if (pstHandle->handle == handle)
        {
            *res = pstHandle;
            TDE_UNLOCK(&s_pstTdeHandleList->lock,lockflags);
            return HI_TRUE;
        }
        pstHandle = list_entry(pstHandle->stHeader.prev, HI_HANDLE_MGR, stHeader);

    }
    TDE_UNLOCK(&s_pstTdeHandleList->lock,lockflags);
    return HI_FALSE;
}

 HI_BOOL  release_handle(HI_S32 handle)
{
#ifndef TDE_BOOT
    HI_SIZE_T lockflags;
#endif
    HI_HANDLE_MGR *pstHandle = NULL;
    TDE_LOCK(&s_pstTdeHandleList->lock,lockflags);
    pstHandle = list_entry(s_pstTdeHandleList->stHeader.next, HI_HANDLE_MGR, stHeader);
    while (pstHandle != s_pstTdeHandleList)
    {
        if (pstHandle->handle == handle)
        {
            list_del_init(&pstHandle->stHeader);
            TDE_UNLOCK(&s_pstTdeHandleList->lock,lockflags);
            return HI_TRUE;
        }
        pstHandle = list_entry(pstHandle->stHeader.next, HI_HANDLE_MGR, stHeader);
    }
    TDE_UNLOCK(&s_pstTdeHandleList->lock,lockflags);
    return HI_FALSE;
}

 HI_VOID destroy_handle(HI_VOID)
{
	/* Free head node, note: other nodes are all loaded, their resource  are responsibilited by its own module */
    if (NULL != s_pstTdeHandleList)
    {
        vfree(s_pstTdeHandleList);
        s_pstTdeHandleList = NULL;
    }

    return;
}

#ifndef TDE_BOOT
HI_VOID TdeFreePendingJob(HI_VOID)
{
    HI_HANDLE_MGR *pstHandle;
    HI_HANDLE_MGR *pstSave;
    TDE_SWJOB_S *pstJob;
    pid_t pid;
    HI_SIZE_T lockflags;

    pid = current->tgid;

    TDE_LOCK(&s_pstTdeHandleList->lock,lockflags);
    pstHandle = list_entry(s_pstTdeHandleList->stHeader.next, HI_HANDLE_MGR, stHeader);
    if(NULL==pstHandle)
    {
        TDE_TRACE(TDE_KERN_ERR, "No pending job!!\n");
        TDE_UNLOCK(&s_pstTdeHandleList->lock,lockflags);
        return;
    }
    while (pstHandle != s_pstTdeHandleList)
    {
        pstSave = pstHandle;
        pstJob = (TDE_SWJOB_S *)pstSave->res;
        /* FIXME debug */
        if(NULL==pstJob)
        {
            TDE_TRACE(TDE_KERN_ERR, "ERR:pstJob Null Pointer!!!\n");
            TDE_UNLOCK(&s_pstTdeHandleList->lock,lockflags);
            return;
        }
        /* FIXME END */
        pstHandle = list_entry(pstHandle->stHeader.next, HI_HANDLE_MGR, stHeader);
        if(NULL==pstHandle)
        {
            TDE_TRACE(TDE_KERN_ERR, "ERR: pstHandle Null Pointer!!\n");
            TDE_UNLOCK(&s_pstTdeHandleList->lock,lockflags);
            return;
        }
        /** free when it is not submitted */
        if ((!pstJob->bSubmitted)&&(pid==pstJob->pid))
        {
            /** free handle resource */
            TdeOsiListFreeSerialCmd(pstJob->pstFirstCmd, pstJob->pstTailNode);
            list_del_init(&pstSave->stHeader);
            TDE_FREE(pstSave);
        }

    }
    TDE_UNLOCK(&s_pstTdeHandleList->lock,lockflags);
    return;
}
#endif


HI_S32 get_handlenum(HI_VOID)
{
#warning TODO: get_handlenum
	return s_s32HandleNum;
}

#ifdef __cplusplus
 #if __cplusplus
}
 #endif /* __cplusplus */
#endif  /* __cplusplus */


