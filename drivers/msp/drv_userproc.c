/******************************************************************************

  Copyright (C), 2013-2023, Hisilicon Tech. Co., Ltd.

******************************************************************************
  File Name     : drv_userproc.c
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2013/07/01
  Description   : Support user proc function.
  History       :
  1.Date        : 2013/07/01
    Author      : l00185424
    Modification: Created file

******************************************************************************/

/******************************* Include Files *******************************/

/* Sys headers */
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/ioctl.h>
#include <linux/mman.h>
#include <linux/uaccess.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/list.h>
#include <asm/atomic.h>
/* Unf headers */
#include "hi_module.h"
#include "hi_debug.h"
#include "hi_common.h"
#include "hi_osal.h"
/* Drv headers */
#include "drv_userproc_ioctl.h"
#include "hi_drv_userproc.h"
#include "hi_drv_proc.h"
#include "hi_kernel_adapt.h"
#include "hi_drv_mem.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#if !(0 == HI_PROC_SUPPORT)

/***************************** Macro Definition ******************************/

#define UPROC_K_LOCK(sema) \
    do \
    { \
        if (down_interruptible(&sema)) \
        { \
            HI_ERR_UPROC("Lock fail"); \
        } \
    } while (0)
#define UPROC_K_UNLOCK(sema) do {up(&sema);} while (0)

#define FIND_DIR_BY_NAME(pszName, pstDir) \
    { \
        struct list_head* pos; \
        struct list_head* n; \
        UMP_DIR_S* pstTmp; \
        pstDir = HI_NULL; \
        /*UPROC_K_LOCK(g_stUProcParam.stSem);*/ \
        list_for_each_safe(pos, n, &g_stUProcParam.dir_list) \
        { \
            pstTmp = list_entry(pos, UMP_DIR_S, dir_node); \
            /*HI_INFO_UPROC("FIND %s CURR %s\n", pszName, pstTmp->dir_name);*/ \
            if (0 == HI_OSAL_Strncmp(pszName, pstTmp->dir_name, sizeof(pstTmp->dir_name))) \
            { \
                pstDir = pstTmp; \
                break; \
            } \
        } \
        /*UPROC_K_UNLOCK(g_stUProcParam.stSem);*/ \
    }

#define FIND_ENTRY_BY_NAME(stList, pszName, pstEntry) \
    { \
        struct list_head* pos; \
        struct list_head* n; \
        UMP_ENTRY_S* pstTmp; \
        pstEntry = HI_NULL; \
        /*UPROC_K_LOCK(g_stUProcParam.stSem);*/ \
        list_for_each_safe(pos, n, &stList) \
        { \
            pstTmp = list_entry(pos, UMP_ENTRY_S, entry_node); \
            if (0 == HI_OSAL_Strncmp(pszName, pstTmp->entry_name, sizeof(pstTmp->entry_name))) \
            { \
                pstEntry = pstTmp; \
                break; \
            } \
        } \
        /*UPROC_K_UNLOCK(g_stUProcParam.stSem);*/ \
    }

#define FIND_ENTRY_BY_ADDR(pAddr, pstEntry) \
    { \
        struct list_head* pos1; \
        struct list_head* n1; \
        struct list_head* pos2; \
        struct list_head* n2; \
        UMP_DIR_S* pstTmpDir; \
        UMP_ENTRY_S* pstTmpEntry; \
        pstEntry = HI_NULL; \
        /*UPROC_K_LOCK(g_stUProcParam.stSem);*/ \
        list_for_each_safe(pos1, n1, &g_stUProcParam.dir_list) \
        { \
            pstTmpDir = list_entry(pos1, UMP_DIR_S, dir_node); \
            list_for_each_safe(pos2, n2, &pstTmpDir->entry_list) \
            { \
                pstTmpEntry = list_entry(pos2, UMP_ENTRY_S, entry_node); \
                /*HI_INFO_UPROC("FIND %p CURR %p\n", pAddr, pstTmpEntry->stInfo.pEntry);*/ \
                if (pAddr == pstTmpEntry->stInfo.pEntry) \
                { \
                    pstEntry = pstTmpEntry; \
                    break; \
                } \
            } \
            if (HI_NULL != pstEntry) \
            { \
                break; \
            } \
        } \
        /*UPROC_K_UNLOCK(g_stUProcParam.stSem);*/ \
    }

#define FREE_ENTRIES_IN_DIR(pstDir) \
    { \
        struct list_head* pos; \
        struct list_head* n; \
        UMP_ENTRY_S* pstEntry; \
        list_for_each_safe(pos, n, &pstDir->entry_list) \
        { \
            pstEntry = list_entry(pos, UMP_ENTRY_S, entry_node); \
            PROC_RemoveEntry(pstEntry); \
        } \
    }    

/*************************** Structure Definition ****************************/

typedef struct tagUMP_ENTRY_S
{
    HI_CHAR entry_name[MAX_PROC_NAME_LEN+1];
    struct proc_dir_entry *parent;
    HI_DRV_USRMODEPROC_ENTRY_S stInfo;
    struct list_head entry_node;
}UMP_ENTRY_S;

typedef struct tagUMP_DIR_S
{
    HI_CHAR dir_name[MAX_PROC_NAME_LEN+12];   /* '_' 1 and pid 10 */
    struct file * pstFile;
    struct proc_dir_entry *entry;
    struct proc_dir_entry *parent;
    struct list_head dir_node;
    struct list_head entry_list;
}UMP_DIR_S;

typedef struct tagUMP_PARAM_S
{
    wait_queue_head_t wq_for_read;
    wait_queue_head_t wq_for_write;
    int busy;
    HI_DRV_USRMODEPROC_CMD_S current_cmd;
    atomic_t atmOpenCnt;
    struct semaphore stSem; /* Semaphore */
    /* list of entries */
	struct list_head dir_list;
}UMP_PARAM_S;

/***************************** Global Definition *****************************/

extern struct proc_dir_entry *g_pHisi_proc;
extern struct proc_dir_entry *g_pCMPI_proc;

/***************************** Static Definition *****************************/

static UMP_PARAM_S g_stUProcParam = 
{
    .atmOpenCnt = ATOMIC_INIT(0),
    .dir_list = {&g_stUProcParam.dir_list, &g_stUProcParam.dir_list}
};

static HI_VOID PROC_RemoveDirForcibly(UMP_DIR_S *pstDir);
static HI_VOID PROC_RemoveEntry(UMP_ENTRY_S * pstEntry);

/*********************************** Code ************************************/

static int hi_usrmodeproc_open(struct inode *inode, struct file *file)
{
    HI_INFO_UPROC("Open User Mode Proc:%s,%d\n", current->comm, current->pid);

    if (atomic_inc_return(&g_stUProcParam.atmOpenCnt) == 1)
    {
        memset(&g_stUProcParam.current_cmd, 0, sizeof(HI_DRV_USRMODEPROC_CMD_S));
        init_waitqueue_head(&g_stUProcParam.wq_for_read);
        init_waitqueue_head(&g_stUProcParam.wq_for_write);
        g_stUProcParam.busy = 0;
        HI_INIT_MUTEX(&g_stUProcParam.stSem);
    }

	file->private_data = &g_stUProcParam;    
    return 0;
}

static int hi_usrmodeproc_close(struct inode *inode, struct file *file)
{
    struct list_head* pos1;
    struct list_head* n1;
    struct list_head* pos2;
    struct list_head* n2;
    UMP_DIR_S* pstTmpDir;
    UMP_ENTRY_S* pstTmpEntry;

    if (atomic_dec_return(&g_stUProcParam.atmOpenCnt) >= 0)
    {
        UPROC_K_LOCK(g_stUProcParam.stSem);
        list_for_each_safe(pos1, n1, &g_stUProcParam.dir_list)
        {
            pstTmpDir = list_entry(pos1, UMP_DIR_S, dir_node);
        
            if (file == pstTmpDir->pstFile)
            {
                PROC_RemoveDirForcibly(pstTmpDir);
            }
            else if (0 == pstTmpDir->pstFile)
            {
                list_for_each_safe(pos2, n2, &pstTmpDir->entry_list)
                {
                    pstTmpEntry = list_entry(pos2, UMP_ENTRY_S, entry_node);
                    if ((HI_VOID*)file == pstTmpEntry->stInfo.pFile)
                    {
                        PROC_RemoveEntry(pstTmpEntry);
                    }
                }
            }
        }
        UPROC_K_UNLOCK(g_stUProcParam.stSem);
    }

    HI_INFO_UPROC("Close User Mode Proc\n" );
    return 0;
}

static int ump_seq_show(struct seq_file *m, void *unused)
{
    struct proc_dir_entry* pstPde = (struct proc_dir_entry *)(m->private);
    UMP_PARAM_S *proc = ((struct proc_dir_entry *)(m->private))->data;
    UMP_ENTRY_S *pstEntry = HI_NULL;
    DEFINE_WAIT(wait);

    UPROC_K_LOCK(g_stUProcParam.stSem);
    
    FIND_ENTRY_BY_ADDR(pstPde, pstEntry);
    if (HI_NULL == pstEntry)
    {
        UPROC_K_UNLOCK(g_stUProcParam.stSem);
        HI_FATAL_UPROC("Can't find entry:%p\n", pstPde);
        return -1;
    }
    else if (HI_NULL == pstEntry->stInfo.pfnShowFunc)
    {
        UPROC_K_UNLOCK(g_stUProcParam.stSem);
        HI_FATAL_UPROC("Entry don't support read\n");
        return -1;
    }

    proc->current_cmd.pEntry = &(pstEntry->stInfo);
    HI_OSAL_Strncpy(proc->current_cmd.aszCmd, HI_UPROC_READ_CMD, sizeof(proc->current_cmd.aszCmd)-1);
    UPROC_K_UNLOCK(g_stUProcParam.stSem);

    /* Wait write data over */
    prepare_to_wait(&proc->wq_for_read, &wait, TASK_INTERRUPTIBLE);
    schedule();
    finish_wait(&proc->wq_for_read, &wait);	

    /* Find it again, pstEntry may be removed when wait event */
    UPROC_K_LOCK(g_stUProcParam.stSem);
    FIND_ENTRY_BY_ADDR(pstPde, pstEntry);
    if ((HI_NULL != pstEntry) && (HI_NULL != pstEntry->stInfo.pfnShowFunc))
    {
        HI_INFO_UPROC("User Mode Proc Show entry=0x%p, proc=0x%p\n", pstPde, proc);
        PROC_PRINT(m, "%s", (HI_CHAR*)pstEntry->stInfo.stBuf.u32StartVirAddr);
    }
    UPROC_K_UNLOCK(g_stUProcParam.stSem);

    return 0;
}

static int ump_seq_open(struct inode *inode, struct file *file)
{
    UMP_PARAM_S *proc = PDE(inode)->data;
    int res;

    HI_INFO_UPROC("ump_seq_open 0x%p,%d\n", proc, proc->busy );
    
    if (proc->busy)
        return -EAGAIN;

    proc->busy = 1;

    res = single_open(file, ump_seq_show, PDE(inode));

    if( res )
        proc->busy = 0;

    return res;
}

static int ump_seq_release(struct inode *inode, struct file *file)
{
    UMP_PARAM_S *proc = PDE(inode)->data;

    HI_INFO_UPROC("ump_seq_release %d\n", proc->busy );

    proc->busy = 0;
    return single_release(inode, file);
}

static HI_S32 StripString(HI_CHAR *string, HI_U32 size)
{
    HI_CHAR * p = string;
    HI_U32      index = 0;
    
    if (!string ||  0 ==  size)
        return HI_FAILURE;
    
    /* strip '\n' as string end character */
    for (; index < size; index++)
    {
        if ( '\n' == *(p + index) )
        {
            *(p + index) = '\0';
        }
    }

    if (strlen(string))
        return HI_SUCCESS;
    else
        return HI_FAILURE;
}

static ssize_t ump_seq_write (struct file *file, const char __user *buf, size_t size, loff_t *pos)
{
    struct proc_dir_entry* pstPde = PDE(file->f_path.dentry->d_inode);
    UMP_PARAM_S *proc = pstPde->data;
    UMP_ENTRY_S *pstEntry = HI_NULL;
    DEFINE_WAIT(wait);

    UPROC_K_LOCK(g_stUProcParam.stSem);
    FIND_ENTRY_BY_ADDR(pstPde, pstEntry);
    if (HI_NULL == pstEntry ||  size > sizeof(proc->current_cmd.aszCmd) )
    {
        HI_FATAL_UPROC("invalid parameter.\n");
        UPROC_K_UNLOCK(g_stUProcParam.stSem);
        return -EINVAL;
    }
    else if (HI_NULL == pstEntry->stInfo.pfnCmdFunc)
    {
        HI_FATAL_UPROC("Entry don't support write.\n");
        UPROC_K_UNLOCK(g_stUProcParam.stSem);
        return -EINVAL;
    }

    memset(proc->current_cmd.aszCmd, 0, sizeof(proc->current_cmd.aszCmd));
    if (copy_from_user(proc->current_cmd.aszCmd, buf, size))
    {
        HI_FATAL_UPROC("get cmd failed.");
        UPROC_K_UNLOCK(g_stUProcParam.stSem);
        return -EIO;
    }
    proc->current_cmd.aszCmd[size - 1] = '\0';

    if (HI_FAILURE == StripString(proc->current_cmd.aszCmd, size))
    {
        HI_WARN_UPROC("echo string invalid.");
        UPROC_K_UNLOCK(g_stUProcParam.stSem);
        return -EINVAL;
    }
    
    proc->current_cmd.pEntry = &(pstEntry->stInfo);

    UPROC_K_UNLOCK(g_stUProcParam.stSem);
    
    /* Wait write data over */
    prepare_to_wait(&proc->wq_for_write, &wait, TASK_INTERRUPTIBLE);
    schedule();
    finish_wait(&proc->wq_for_write, &wait);	

    UPROC_K_LOCK(g_stUProcParam.stSem);
    FIND_ENTRY_BY_ADDR(pstPde, pstEntry);
    
    /* if buffer not empty , try echo to current terminal */
    if ( HI_NULL != pstEntry && pstEntry->stInfo.pfnCmdFunc )
    {   
        HI_INFO_UPROC( "ump_seq_write: proc=%p, entry=%p %d bytes\n", proc, pstPde, size);
        if (strlen( (HI_CHAR*)pstEntry->stInfo.stBuf.u32StartVirAddr))
        {
            HI_DRV_PROC_EchoHelper((HI_CHAR*)pstEntry->stInfo.stBuf.u32StartVirAddr);
        }
    }
    
    UPROC_K_UNLOCK(g_stUProcParam.stSem);
    
    return size;
}

static HI_S32 PROC_AddDir(const HI_CHAR* pszName, const HI_CHAR* pszParent, struct file *pstFile)
{
    UMP_DIR_S * pstDir;
    UMP_DIR_S * pstDirFind = HI_NULL;

    /* Check parameter */
    if ((HI_NULL == pszName) || (strlen(pszName) == 0) || (strlen(pszName) > MAX_PROC_NAME_LEN))
    {
        HI_ERR_UPROC("Invalid name\n");
        return HI_FAILURE;
    }

    /* Alloc directory resource */
    pstDir = HI_VMALLOC(HI_ID_PROC, sizeof(UMP_DIR_S));
    if (HI_NULL == pstDir)
    {
        HI_FATAL_UPROC("vmalloc fail\n");
        return HI_FAILURE;
    }

    /* Make directory name */
    HI_OSAL_Snprintf(pstDir->dir_name, sizeof(pstDir->dir_name), "%s_%d", pszName, current->pid);

    /* Find directory node, if exist, return success directlly */
    FIND_DIR_BY_NAME(pstDir->dir_name, pstDirFind);
    if (HI_NULL != pstDirFind)
    {
        HI_WARN_UPROC("Dir %s exist\n", pstDir->dir_name);
        HI_VFREE(HI_ID_PROC, pstDir);
        return HI_SUCCESS;
    }

    /* Make proc directory */
    pstDir->entry = proc_mkdir(pstDir->dir_name, g_pHisi_proc);
    if (HI_NULL == pstDir->entry)
    {
        HI_VFREE(HI_ID_PROC, pstDir);
        HI_FATAL_UPROC("proc_mkdir fail\n");
        return HI_FAILURE;
    }
    HI_INFO_UPROC("Proc add dir %s, file=%p, entry=0x%p\n", pstDir->dir_name, pstFile, pstDir->entry);

    /* Init other parameter */
    pstDir->parent = HI_NULL;
    pstDir->pstFile = pstFile;
    INIT_LIST_HEAD(&pstDir->entry_list);

    /* Add directory to list */
    list_add(&pstDir->dir_node, &g_stUProcParam.dir_list);
 
    return HI_SUCCESS;
}

static HI_S32 PROC_RemoveDir(UMP_DIR_S * pstDir)
{
    /* Check parameter */
    if (HI_NULL == pstDir)
    {
        HI_ERR_UPROC("Invalid name\n");
        return HI_FAILURE;
    }
    
    /* If there are entries in this directory, remove fail */
    if (!(list_empty(&pstDir->entry_list)))
    {
        HI_ERR_UPROC("dir %s non-null\n", pstDir->dir_name);
        return HI_FAILURE;
    }

    /* Remove proc directory */
    remove_proc_entry(pstDir->dir_name, g_pHisi_proc);
    HI_INFO_UPROC("Proc remove dir %s\n", pstDir->dir_name);

    /* Remove directory from list */
    list_del(&pstDir->dir_node);

    /* Free directory resource */
    HI_VFREE(HI_ID_PROC, pstDir);

    return HI_SUCCESS;
}

static HI_S32 PROC_RemoveDirByName(const HI_CHAR* pszName)
{
    HI_CHAR aszDir[MAX_PROC_NAME_LEN+12];
    UMP_DIR_S * pstDir = HI_NULL;

    /* Check parameter */
    if ((HI_NULL == pszName) || (strlen(pszName) == 0) ||  (strlen(pszName) > MAX_PROC_NAME_LEN))
    {
        HI_ERR_UPROC("Invalid name\n");
        return HI_FAILURE;
    }
    
    /* Make directory name */
    HI_OSAL_Snprintf(aszDir, sizeof(aszDir), "%s_%d", pszName, current->pid);

    /* Find directory node */
    FIND_DIR_BY_NAME(aszDir, pstDir);
    if (HI_NULL == pstDir)
    {
        HI_ERR_UPROC("Find dir %s fail\n", pszName);
        return HI_FAILURE;
    }

    return PROC_RemoveDir(pstDir);
}

static HI_S32 PROC_AddPrivateDir(const HI_CHAR* pszName, struct proc_dir_entry *pstEntry)
{
    UMP_DIR_S * pstDir;

    /* Check parameter */
    if ((HI_NULL == pszName) || (strlen(pszName) > MAX_PROC_NAME_LEN) || (HI_NULL == pstEntry))
    {
        return HI_FAILURE;
    }

    /* Alloc directory resource */
    pstDir = HI_VMALLOC(HI_ID_PROC, sizeof(UMP_DIR_S));
    if (HI_NULL == pstDir)
    {
        HI_FATAL_UPROC("vmalloc fail\n");
        return HI_FAILURE;
    }

    /* Init other parameter */
    HI_OSAL_Strncpy(pstDir->dir_name, pszName, sizeof(pstDir->dir_name)-1);
    pstDir->entry = pstEntry;
    pstDir->parent = HI_NULL;
    pstDir->pstFile = HI_NULL;
    INIT_LIST_HEAD(&pstDir->entry_list);

    /* Add directory to list */
    list_add(&pstDir->dir_node, &g_stUProcParam.dir_list);

    return HI_SUCCESS;
}

static HI_S32 PROC_RemovePrivateDir(const HI_CHAR* pszName)
{
    UMP_DIR_S * pstDir = HI_NULL;

    /* Check parameter */
    if (HI_NULL == pszName)
    {
        return HI_FAILURE;
    }

    /* Find directory node */
    FIND_DIR_BY_NAME(pszName, pstDir);
    if (HI_NULL == pstDir)
    {
        HI_ERR_UPROC("Find dir %s fail\n", pszName);
        return HI_FAILURE;
    }

    /* Remove directory from list */
    list_del(&pstDir->dir_node);

    /* Free directory resource */
    HI_VFREE(HI_ID_PROC, pstDir);
    return HI_SUCCESS;
}

static UMP_ENTRY_S* PROC_AddEntry(const HI_DRV_USRMODEPROC_ENTRY_S* pstParam, HI_BOOL bUsrMode)
{
    HI_S32 s32Ret;
    UMP_ENTRY_S * pstEntry = HI_NULL;
    UMP_DIR_S * pstDir = HI_NULL;
    HI_CHAR aszDir[MAX_PROC_NAME_LEN+12];
    HI_CHAR aszMMZName[32];
    HI_U32 u32EntryLen;
    
    /* Check parameter */
    if (HI_NULL == pstParam)
    {
        return HI_NULL;
    }
    u32EntryLen = strlen(pstParam->aszName);
    if ((0 == u32EntryLen) || (u32EntryLen > MAX_PROC_NAME_LEN))
    {
        HI_ERR_UPROC("Invalid name\n");
        return HI_NULL;
    }

    /* Make parent directory name */
    if (0 == strlen(pstParam->aszParent))
    {
        HI_OSAL_Strncpy(aszDir, "hisi", sizeof(aszDir)-1);
    }
    else if (0 == HI_OSAL_Strncmp("msp", pstParam->aszParent, 4))
    {
        HI_OSAL_Strncpy(aszDir, pstParam->aszParent, sizeof(aszDir)-1);
    }
    else
    {
        HI_OSAL_Snprintf(aszDir, sizeof(aszDir), "%s_%d", pstParam->aszParent, current->pid);
    }

    /* Find directory node, if don't exist, return fail */
    FIND_DIR_BY_NAME(aszDir, pstDir);
    if (HI_NULL == pstDir)
    {
        HI_ERR_UPROC("Dir %s don't exist\n", pstParam->aszParent);
        return HI_NULL;
    }

    /* Find entry in the directory, if exist, return success directlly */
    FIND_ENTRY_BY_NAME(pstDir->entry_list, pstParam->aszName, pstEntry);
    if (HI_NULL != pstEntry)
    {
        HI_WARN_UPROC("Entry %s exist\n", pstParam->aszName);
        return pstEntry;
    }

    /* Alloc entry resource */
    pstEntry = HI_VMALLOC(HI_ID_PROC, sizeof(UMP_ENTRY_S));
    if (HI_NULL == pstEntry)
    {
        HI_FATAL_UPROC("vmalloc fail\n");
        return HI_NULL;
    }
    memset(pstEntry, 0, sizeof(UMP_ENTRY_S));

    /* Create proc entry */
    pstEntry->stInfo.pEntry = create_proc_entry(pstParam->aszName, 0, pstDir->entry);
    if (HI_NULL == pstEntry->stInfo.pEntry)
    {
        HI_VFREE(HI_ID_PROC, pstEntry);
        HI_FATAL_UPROC("create_proc_entry fail\n");
        return HI_NULL;
    }
    HI_INFO_UPROC("Proc add entry %s, file=%p, entry=0x%p\n", pstParam->aszName, pstParam->pFile, pstEntry->stInfo.pEntry);

    /* Init other parameter */
    HI_OSAL_Strncpy(pstEntry->entry_name, pstParam->aszName, sizeof(pstEntry->entry_name)-1);
    pstEntry->parent = pstDir->entry;
    pstEntry->stInfo.pFile = pstParam->pFile;
    pstEntry->stInfo.pfnShowFunc = pstParam->pfnShowFunc;
    pstEntry->stInfo.pfnCmdFunc = pstParam->pfnCmdFunc;
    pstEntry->stInfo.pPrivData   = pstParam->pPrivData;

    /* pfnShowFunc need mmz */
    if ( bUsrMode )
    {
        /* Alloc MMZ */
        HI_OSAL_Snprintf(aszMMZName, sizeof(aszMMZName), "CMN_Uproc_%s", pstEntry->entry_name);
        s32Ret = HI_DRV_MMZ_AllocAndMap(aszMMZName, NULL, HI_PROC_BUFFER_SIZE, 0, &(pstEntry->stInfo.stBuf));
        if (HI_SUCCESS != s32Ret)
        {
            remove_proc_entry(pstEntry->entry_name, pstEntry->parent);
            HI_VFREE(HI_ID_PROC, pstEntry);
            HI_FATAL_UPROC("Alloc MMZ fail:%#x\n", s32Ret);
            return HI_NULL;
        }
        memset((HI_VOID*)pstEntry->stInfo.stBuf.u32StartVirAddr, 0, pstEntry->stInfo.stBuf.u32Size);
    }
    else
    {
        pstEntry->stInfo.stBuf.u32Size = 0;
        pstEntry->stInfo.stBuf.u32StartPhyAddr = 0;
        pstEntry->stInfo.stBuf.u32StartVirAddr = 0;
    }
    
    /* Add entry to list */
    list_add(&pstEntry->entry_node, &pstDir->entry_list);

    return pstEntry;
}

static HI_VOID PROC_RemoveEntry(UMP_ENTRY_S * pstEntry)
{
    /* Check parameter */
    if (HI_NULL == pstEntry)
    {
        return;
    }

    /* Remove proc entry */
    remove_proc_entry(pstEntry->entry_name, pstEntry->parent);
    HI_INFO_UPROC("Proc remove entry %s\n", pstEntry->entry_name);

    /* Free MMZ */
    if (0 != pstEntry->stInfo.stBuf.u32Size)
    {
        HI_DRV_MMZ_UnmapAndRelease(&(pstEntry->stInfo.stBuf));
    }
    
    /* Remove entry from list */
    list_del(&pstEntry->entry_node);

    /* If current command belongs to this entry, clear it. */
    if (g_stUProcParam.current_cmd.pEntry == (HI_VOID*)&(pstEntry->stInfo))
    {
        g_stUProcParam.current_cmd.pEntry = HI_NULL;
        memset(&g_stUProcParam.current_cmd, 0, sizeof(g_stUProcParam.current_cmd));
    }

    /* Free resource */
    HI_VFREE(HI_ID_PROC, pstEntry);

    return;
}

static HI_S32 PROC_RemoveEntryByName(const HI_CHAR* pszName, const HI_CHAR* pszParent)
{
    UMP_ENTRY_S * pstEntry = HI_NULL;
    UMP_DIR_S * pstDir = HI_NULL;
    HI_CHAR aszDir[MAX_PROC_NAME_LEN+12];

    /* Check parameter */
    if ((HI_NULL == pszName) || (strlen(pszName) > MAX_PROC_NAME_LEN))
    {
        HI_ERR_UPROC("Invalid name\n");
        return HI_FAILURE;
    }
    if ((HI_NULL == pszParent) || (strlen(pszParent) > MAX_PROC_NAME_LEN))
    {
        HI_ERR_UPROC("Invalid parent name\n");
        return HI_FAILURE;
    }

    /* Make parent directory name */
    if (0 == strlen(pszParent))
    {
        HI_OSAL_Strncpy(aszDir, "hisi", sizeof(aszDir)-1);
    }
    else if (0 == HI_OSAL_Strncmp("msp", pszParent, 4))
    {
        HI_OSAL_Strncpy(aszDir, pszParent, sizeof(aszDir)-1);
    }
    else
    {
        HI_OSAL_Snprintf(aszDir, sizeof(aszDir), "%s_%d", pszParent, current->pid);
    }

    /* Find directory node, if don't exist, return fail */
    FIND_DIR_BY_NAME(aszDir, pstDir);
    if (HI_NULL == pstDir)
    {
        HI_ERR_UPROC("Dir %s don't exist\n", pszParent);
        return HI_FAILURE;
    }

    /* Find entry in the directory, if don't exist, return fail */
    FIND_ENTRY_BY_NAME(pstDir->entry_list, pszName, pstEntry);
    if (HI_NULL == pstEntry)
    {
        HI_WARN_UPROC("Entry %s don't exist\n", pszName);
        return HI_FAILURE;
    }

    /* Remove entry */
    PROC_RemoveEntry(pstEntry);

    return HI_SUCCESS;
}

static HI_VOID PROC_RemoveDirForcibly(UMP_DIR_S *pstDir)
{
    /* Check parameter */
    if (HI_NULL == pstDir)
    {
        return;
    }
    
    HI_INFO_UPROC("Proc remove dir: %s\n", pstDir->dir_name); \

    /* Free entries */
    if (!(list_empty(&pstDir->entry_list)))
    {
        FREE_ENTRIES_IN_DIR(pstDir);
    }

    PROC_RemoveDir(pstDir);
}

static struct file_operations ump_seq_fops = {
        .open = ump_seq_open,
        .read = seq_read,
        .write   = ump_seq_write,
        .llseek = seq_lseek,
        .release = ump_seq_release,
};

static long hi_usrmodeproc_ioctl(struct file *file,
				unsigned int cmd, unsigned long arg)
{
    HI_S32 s32Ret = HI_SUCCESS;
    UMP_PARAM_S *proc = file->private_data;
    HI_DRV_USRMODEPROC_ENTRY_S ump_entry;
    HI_CHAR aszName[MAX_PROC_NAME_LEN+1];
    UMP_ENTRY_S* pstEntry;
    HI_DRV_USRMODEPROC_CMDINFO_S *pstCmdInfo;

    switch(cmd)
    {
        case UMPIOC_ADD_ENTRY:
            if (copy_from_user(&ump_entry, (void __user *)arg, sizeof(ump_entry)))
            {
                s32Ret = HI_FAILURE;
                break;
            }
            ump_entry.aszName[sizeof(ump_entry.aszName)-1] = 0;
            ump_entry.aszParent[sizeof(ump_entry.aszParent)-1] = 0;
            ump_entry.pFile = (HI_VOID*)file;
            
            pstEntry = PROC_AddEntry(&ump_entry, HI_TRUE);
            if (HI_NULL == pstEntry)
            {
                s32Ret = HI_FAILURE;
                break;
            }
            
            ((struct proc_dir_entry *)pstEntry->stInfo.pEntry)->proc_fops = &ump_seq_fops;
            ((struct proc_dir_entry *)pstEntry->stInfo.pEntry)->data = proc;
            break;

        case UMPIOC_REMOVE_ENTRY:
            if (copy_from_user(&ump_entry, (void __user *)arg, sizeof(ump_entry)))
            {
                s32Ret = HI_FAILURE;
                break;
            }
            ump_entry.aszName[sizeof(ump_entry.aszName)-1] = 0;
            ump_entry.aszParent[sizeof(ump_entry.aszParent)-1] = 0;

            UPROC_K_LOCK(g_stUProcParam.stSem);
            /* Removed by name now, can be removed by ump_entry.pEntry  */
            s32Ret = PROC_RemoveEntryByName((HI_CHAR*)ump_entry.aszName, (HI_CHAR*)ump_entry.aszParent);
            UPROC_K_UNLOCK(g_stUProcParam.stSem);
            break;

        case UMPIOC_ADD_DIR:
        {
            HI_CHAR *ptrDirName = aszName;
            if (copy_from_user(ptrDirName, (void __user *)arg, sizeof(HI_PROC_DIR_NAME_S)))
            {
                s32Ret = HI_FAILURE;
                break;
            }
            aszName[sizeof(aszName)-1] = 0;

            UPROC_K_LOCK(g_stUProcParam.stSem);
            s32Ret = PROC_AddDir(aszName, HI_NULL, file);
            UPROC_K_UNLOCK(g_stUProcParam.stSem);
            break;
        }
        case UMPIOC_REMOVE_DIR:
        {
            HI_CHAR *ptrDirName = aszName;
            if (copy_from_user(ptrDirName, (void __user *)arg, sizeof(HI_PROC_DIR_NAME_S)))
            {
                s32Ret = HI_FAILURE;
                break;
            }
            aszName[sizeof(aszName)-1] = 0;

            UPROC_K_LOCK(g_stUProcParam.stSem);
            s32Ret = PROC_RemoveDirByName(aszName);
            UPROC_K_UNLOCK(g_stUProcParam.stSem);
            break;
        }
        case UMPIOC_GETCMD:
            pstCmdInfo = (HI_DRV_USRMODEPROC_CMDINFO_S*)arg;

            UPROC_K_LOCK(g_stUProcParam.stSem);
                /* If there is a command */
            if ((strlen(proc->current_cmd.aszCmd) > 0) && 
                /* and it must belong to a entry */
                (HI_NULL != proc->current_cmd.pEntry) && 
                /* and the entry must belong to this file(this process). */
                ((HI_VOID*)file == ((HI_DRV_USRMODEPROC_ENTRY_S*)proc->current_cmd.pEntry)->pFile))
            {
                if (copy_to_user((void __user *)&(pstCmdInfo->stCmd), 
                    &(proc->current_cmd), sizeof(HI_DRV_USRMODEPROC_CMD_S)))
                {
                    UPROC_K_UNLOCK(g_stUProcParam.stSem);
                    return -EFAULT;
                }
                if (copy_to_user((void __user *)&(pstCmdInfo->stEntry), 
                    proc->current_cmd.pEntry, sizeof(HI_DRV_USRMODEPROC_ENTRY_S)))
                {
                    UPROC_K_UNLOCK(g_stUProcParam.stSem);
                    return -EFAULT;
                }

                memset(proc->current_cmd.aszCmd, 0, sizeof(proc->current_cmd.aszCmd));
            }
            UPROC_K_UNLOCK(g_stUProcParam.stSem);
            break;

        case UMPIOC_WAKE_READ_TASK:
            wake_up_interruptible(&(proc->wq_for_read));
            break;
            
         case UMPIOC_WAKE_WRITE_TASK:
            wake_up_interruptible(&(proc->wq_for_write));
            break;
            
        default:
            s32Ret = HI_FAILURE;
            break;
    }
    
    return s32Ret;
}


static struct file_operations hi_usrmodeproc_fops =
{
    .open    = hi_usrmodeproc_open,
    .release   = hi_usrmodeproc_close,
    .unlocked_ioctl = hi_usrmodeproc_ioctl,
};

static struct miscdevice hi_usrmodeproc_dev =
{
    MISC_DYNAMIC_MINOR,
    HI_USERPROC_DEVNAME,
    &hi_usrmodeproc_fops
};

HI_S32 USRPROC_DRV_ModInit(HI_VOID)
{
    HI_S32 ret;

    ret = misc_register(&hi_usrmodeproc_dev);
    if (ret)
    {
        HI_ERR_UPROC("%s device register failed\n", HI_USERPROC_DEVNAME);
    }

    PROC_AddPrivateDir("hisi", g_pHisi_proc);
    PROC_AddPrivateDir("msp", g_pCMPI_proc);
    return ret;
}

HI_VOID USRPROC_DRV_ModExit(HI_VOID)
{
    PROC_RemovePrivateDir("msp");
    PROC_RemovePrivateDir("hisi");
    misc_deregister(&hi_usrmodeproc_dev);
}

MODULE_AUTHOR("Yan Haifeng");
MODULE_DESCRIPTION("Hisilicon User Mode Proc Driver");
MODULE_LICENSE("GPL");

#endif

#ifdef __cplusplus
 #if __cplusplus
}
 #endif
#endif /* End of #ifdef __cplusplus */
