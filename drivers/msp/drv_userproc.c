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
#include <proc/internal.h>

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

#if 0
#define UPROC_K_LOCK(sema) \
    do \
    { \
        if (down_interruptible(&sema)) \
        { \
            HI_ERR_UPROC("Lock fail"); \
        } \
    } while (0)

#else
#define UPROC_K_LOCK(sema) do {down(&sema);} while (0)
#endif

#define UPROC_K_UNLOCK(sema) do {up(&sema);} while (0)

/*************************** Structure Definition ****************************/

/*
 * [PATCH linux v2 1/1] fs/proc: use a rb tree for the directory entries
 * http://lists.openwall.net/linux-kernel/2014/10/06/261
 */

typedef struct tagUMP_ENTRY_S
{
	unsigned int hash; //0
	struct rb_node rb_node; //4
    struct proc_dir_entry *parent; //16
    HI_DRV_USRMODEPROC_ENTRY_S stInfo; //20
#if 0
    typedef struct USRMODEPROC_ENTRY_S
    {
        HI_CHAR aszName[MAX_PROC_NAME_LEN + 1];     /* Input, entry name */
        HI_CHAR aszParent[MAX_PROC_NAME_LEN + 1];   /* Input, directory name */
        HI_PROC_SHOW_FN pfnShowFunc; //276
        HI_PROC_CMD_FN pfnCmdFunc;                  /* Input, cmd function */
        HI_VOID * pPrivData;                         /* Input, private data*/
        HI_VOID *pEntry; //288
        HI_VOID *pFile;                             /* Output, Belongs to which file */
        MMZ_BUFFER_S stBuf;                         /* Output, buffer */
    }HI_DRV_USRMODEPROC_ENTRY_S;
#endif
    HI_CHAR entry_name[MAX_PROC_NAME_LEN+1]; //308
    struct list_head entry_node;
}UMP_ENTRY_S;

typedef struct tagUMP_DIR_S
{
	unsigned int hash; //0
	struct rb_node rb_node; //4
	struct rb_root Data_16; //16
    struct file * pstFile; //20
    struct proc_dir_entry *entry; //24
    struct proc_dir_entry *parent; //28
#if 0
    struct list_head dir_node;
    struct list_head entry_list;
#endif
    HI_CHAR dir_name[MAX_PROC_NAME_LEN+12];   /* '_' 1 and pid 10 */ //32
}UMP_DIR_S;

typedef struct tagUMP_PARAM_S
{
#if 0
    int fill[5]; //0
#else
    struct semaphore stSem; /* Semaphore */
    int fill_16; //16
#endif
    wait_queue_head_t wq_for_read; //20
    wait_queue_head_t wq_for_write; //32
    int busy; //44
    HI_DRV_USRMODEPROC_CMD_S current_cmd; //48 +264
    atomic_t atmOpenCnt; //312
#if 0
    /* list of entries */
	struct list_head dir_list;
#endif
}UMP_PARAM_S;

/***************************** Global Definition *****************************/

extern struct proc_dir_entry *g_pHisi_proc;
extern struct proc_dir_entry *g_pCMPI_proc;

/***************************** Static Definition *****************************/

static struct rb_root Data_81109414; //81109414
static UMP_DIR_S *Data_81109540;
static UMP_DIR_S *Data_81109544;

static UMP_PARAM_S g_stUProcParam = 
{
    .atmOpenCnt = ATOMIC_INIT(0),
#if 0
    .dir_list = {&g_stUProcParam.dir_list, &g_stUProcParam.dir_list}
#endif
};

static HI_VOID PROC_RemoveDirForcibly(UMP_DIR_S *pstDir);
static HI_VOID PROC_RemoveEntry(UMP_DIR_S* pstDir, UMP_ENTRY_S * pstEntry);

/*********************************** Code ************************************/

/* nearly complete */
/*static*/ HI_S32 RBTree_Insert_Dirent(int a, UMP_DIR_S * pstDir)
{
	struct rb_node **new = &Data_81109414.rb_node;
	struct rb_node *parent = NULL;
	UMP_DIR_S * tmp;
	int result;

	while (*new)
	{
		parent = *new;
		tmp = rb_entry(*new, UMP_DIR_S, rb_node);

		if (pstDir->hash == tmp->hash)
			result = HI_OSAL_Strncmp(pstDir->dir_name, tmp->dir_name, sizeof(pstDir->dir_name));
		else
			result = pstDir->hash - tmp->hash;

		if (result < 0)
			new = &(*new)->rb_left;
		else if (result > 0)
			new = &(*new)->rb_right;
		else
		{
			HI_ERR_UPROC("dirent(%s) has existed.", pstDir->dir_name); //186
			return HI_FAILURE;
		}
	}
	rb_link_node(&pstDir->rb_node, parent, new);
	rb_insert_color(&pstDir->rb_node, &Data_81109414);

	return HI_SUCCESS;
}

/* nearly complete */
/*static*/ UMP_DIR_S *RBTree_Find_Dirent(int a, const HI_CHAR* pszName)
{
	UMP_DIR_S * pstDir = NULL;
	unsigned int hash = full_name_hash(pszName, strlen(pszName)) & ~0x80000000;
	struct rb_node *node = Data_81109414.rb_node;
	int diff;

	while (node)
	{
		pstDir = rb_entry(node, UMP_DIR_S, rb_node);

		if (pstDir->hash == hash)
			diff = HI_OSAL_Strncmp(pszName, pstDir->dir_name, sizeof(pstDir->dir_name));
		else
			diff = hash - pstDir->hash;

		if (diff < 0)
			node = node->rb_left;
		else if (diff > 0)
			node = node->rb_right;
		else
			return pstDir;
	}

	return NULL;
}

static HI_S32 RBTree_Insert_Entry(int a, UMP_DIR_S * pstDir, UMP_ENTRY_S * pstEntry)
{
	struct rb_node **new = &pstDir->Data_16.rb_node;
	struct rb_node *parent = NULL;
	UMP_ENTRY_S * tmp;
	int result;

	while (*new)
	{
		parent = *new;
		tmp = rb_entry(*new, UMP_ENTRY_S, rb_node);

		if (pstEntry->hash == tmp->hash)
			result = HI_OSAL_Strncmp(pstEntry->entry_name, tmp->entry_name, sizeof(pstEntry->entry_name));
		else
			result = pstEntry->hash - tmp->hash;

		if (result < 0)
			new = &(*new)->rb_left;
		else if (result > 0)
			new = &(*new)->rb_right;
		else
		{
			HI_ERR_UPROC("entry(%s) has existed.", pstEntry->entry_name); //270
			return HI_FAILURE;
		}
	}
	rb_link_node(&pstEntry->rb_node, parent, new);
	rb_insert_color(&pstEntry->rb_node, &pstDir->Data_16);

	return HI_SUCCESS;
}

/* nearly complete */
static UMP_ENTRY_S * RBTree_Find_Entry(UMP_DIR_S *pstDir, const HI_CHAR *pszName)
{
    UMP_ENTRY_S * pstEntry = NULL;
    unsigned int hash = full_name_hash(pszName, strlen(pszName)) & ~0x80000000;
	struct rb_node *node = pstDir->Data_16.rb_node;
	int diff;

	while (node)
	{
		pstEntry = rb_entry(node, UMP_ENTRY_S, rb_node);

		if (pstEntry->hash == hash)
			diff = HI_OSAL_Strncmp(pszName, pstEntry->entry_name, sizeof(pstEntry->entry_name));
		else
			diff = hash - pstEntry->hash;

		if (diff < 0)
			node = node->rb_left;
		else if (diff > 0)
			node = node->rb_right;
		else
			return pstEntry;
	}

	return NULL;
}

/*static*/ UMP_ENTRY_S * RBTree_Find_Proc_Entry(struct proc_dir_entry* pstPde)
{
    UMP_DIR_S * pstDir;
    UMP_ENTRY_S * pstEntry;

	if ((pstPde == HI_NULL) || !strlen(pstPde->parent->name) || !strlen(pstPde->name))
	{
		HI_ERR_UPROC("invalid proc entry."); //295
		return HI_NULL;
	}

	pstDir = RBTree_Find_Dirent(0, pstPde->parent->name);
	if (pstDir == HI_NULL)
	{
		HI_ERR_UPROC("Can't find dirent:%p\n", pstPde->parent->name); //302
		return HI_NULL;
	}

	pstEntry = RBTree_Find_Entry(pstDir, pstPde->name);
	if (pstEntry == HI_NULL)
	{
		HI_ERR_UPROC("Can't find entry:%p\n", pstPde->name);
		return HI_NULL;
	}

	return pstEntry;
}

/* complete */
static int hi_usrmodeproc_open(struct inode *inode, struct file *file)
{
    HI_INFO_UPROC("Open User Mode Proc:%s,%d\n", current->comm, current->pid); //321

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

/* todo */
static int hi_usrmodeproc_close(struct inode *inode, struct file *file)
{
#if 0
    struct list_head* pos1;
    struct list_head* n1;
    struct list_head* pos2;
    struct list_head* n2;
#endif
    UMP_DIR_S* pstTmpDir;
    UMP_ENTRY_S* pstTmpEntry;
    struct rb_node *r7;
    struct rb_node *r0;

    if (atomic_dec_return(&g_stUProcParam.atmOpenCnt) >= 0)
    {
        UPROC_K_LOCK(g_stUProcParam.stSem);
//        list_for_each_safe(pos1, n1, &g_stUProcParam.dir_list)
        while (1)
        {
        	//8053d90c
        	r7 = rb_first(&Data_81109414);

        	if (r7 == NULL)
        	{
        		//->8053d994
        		break;
        	}
        	//8053d930
label_8053d930:
        	pstTmpDir = rb_entry(r7, UMP_DIR_S, rb_node);
            if (file == pstTmpDir->pstFile)
            {
            	//->8053d9cc
                PROC_RemoveDirForcibly(pstTmpDir);
                //->8053d90c
            }
            else
            {
				if (0 == pstTmpDir->pstFile)
				{
					//8053d948
					do
            		{
                		//8053d94c
            			r0 = rb_first(&pstTmpDir->Data_16);
            			if (r0 != NULL)
            			{
							//8053d95c
							pstTmpEntry = rb_entry(r0, UMP_ENTRY_S, rb_node);
							if ((HI_VOID*)file == pstTmpEntry->stInfo.pFile)
							{
								//8053d988
								PROC_RemoveEntry(pstTmpDir, pstTmpEntry);
								//->8053d94c
							}
							else
							{
								//8053d96c
								while ((r0 = rb_next(r0)) != NULL)
								{
									//8053d97c
									pstTmpEntry = rb_entry(r0, UMP_ENTRY_S, rb_node);
									if ((HI_VOID*)file == pstTmpEntry->stInfo.pFile)
									{
										//8053d988
										PROC_RemoveEntry(pstTmpDir, pstTmpEntry);
										//->8053d94c
										break;
									}
								}
							}
            			} //if (r0 != NULL)
            		}
					while (r0 != NULL);
				} //if (0 == pstTmpDir->pstFile)
				//8053d920
				r7 = rb_next(r7);
	        	if (r7 == NULL)
	        	{
	        		//->8053d994
	        		break;
	        	}
	        	//->8053d930
	        	goto label_8053d930;
            }
        }
        //8053d994
        UPROC_K_UNLOCK(g_stUProcParam.stSem);
    }
    //8053d9a0
    HI_INFO_UPROC("Close User Mode Proc\n" ); //374
    return 0;
}

/* complete */
static int ump_seq_show(struct seq_file *m, void *unused)
{
	int res = 0;
    struct proc_dir_entry* pstPde = (struct proc_dir_entry *)(m->private);
    UMP_PARAM_S *proc = pstPde->data;
    UMP_ENTRY_S *pstEntry = HI_NULL;
    DEFINE_WAIT(wait);

    UPROC_K_LOCK(g_stUProcParam.stSem);
    
    pstEntry = RBTree_Find_Proc_Entry(pstPde);
    if (HI_NULL == pstEntry)
    {
        HI_ERR_UPROC("Can't find entry:%p\n", pstPde->name); //392
        res = -1;
        goto end;
    }
    else if (HI_NULL == pstEntry->stInfo.pfnShowFunc)
    {
        HI_ERR_UPROC("Entry don't support read\n"); //398
        res = -1;
        goto end;
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


    pstEntry = RBTree_Find_Proc_Entry(pstPde);
    if (HI_NULL == pstEntry)
    {
        HI_ERR_UPROC("Can't find entry:%p\n", pstPde->name); //419
        res = -1;
        goto end;
    }
    if (HI_NULL != pstEntry->stInfo.pfnShowFunc)
    {
		HI_INFO_UPROC("User Mode Proc Show entry=0x%p, proc=0x%p\n", pstPde, proc); //425
		PROC_PRINT(m, "%s", (HI_CHAR*)pstEntry->stInfo.stBuf.u32StartVirAddr);
    }
end:
    UPROC_K_UNLOCK(g_stUProcParam.stSem);

    return res;
}

/* complete */
static int ump_seq_open(struct inode *inode, struct file *file)
{
    UMP_PARAM_S *proc = __PDE_DATA(inode);
	int res;

    HI_INFO_UPROC("ump_seq_open 0x%p,%d\n", proc, proc->busy ); //443
    
    if (proc->busy)
        return -EAGAIN;

    proc->busy = 1;

    res = single_open(file, ump_seq_show, PDE(inode));

    if( res )
        proc->busy = 0;

    return res;
}

/* nearly complete */
static int ump_seq_release(struct inode *inode, struct file *file)
{
    UMP_PARAM_S *proc = __PDE_DATA(inode);

    HI_INFO_UPROC("ump_seq_release %d\n", proc->busy ); //462

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
    UMP_PARAM_S *proc = /*__PDE_DATA(file->f_path.dentry->d_inode);*/ pstPde->data;
    UMP_ENTRY_S *pstEntry = HI_NULL;
    DEFINE_WAIT(wait);

    UPROC_K_LOCK(g_stUProcParam.stSem);
    pstEntry = RBTree_Find_Proc_Entry(pstPde);
    if (HI_NULL == pstEntry ||  size > sizeof(proc->current_cmd.aszCmd) )
    {
        HI_ERR_UPROC("Can't find entry:%p\n", pstPde->name); //505
        size = -1;
        goto end;
    }
    else if (HI_NULL == pstEntry->stInfo.pfnCmdFunc)
    {
        HI_ERR_UPROC("Entry don't support write.\n"); //511
        size = -1;
        goto end;
    }

    memset(proc->current_cmd.aszCmd, 0, sizeof(proc->current_cmd.aszCmd));
    if (copy_from_user(proc->current_cmd.aszCmd, buf, size))
    {
        HI_ERR_UPROC("get cmd failed."); //519
        size = -EIO;
        goto end;
    }
    proc->current_cmd.aszCmd[(size > 1)? size-1: 0] = '\0';

    if (HI_FAILURE == StripString(proc->current_cmd.aszCmd, size))
    {
        HI_WARN_UPROC("echo string invalid."); //527
        UPROC_K_UNLOCK(g_stUProcParam.stSem);
        return -EINVAL;
        //size = -EINVAL;
        //goto end;
    }
    
    proc->current_cmd.pEntry = &(pstEntry->stInfo);

    UPROC_K_UNLOCK(g_stUProcParam.stSem);
    
    /* Wait write data over */
    prepare_to_wait(&proc->wq_for_write, &wait, TASK_INTERRUPTIBLE);
    schedule();
    finish_wait(&proc->wq_for_write, &wait);	

    UPROC_K_LOCK(g_stUProcParam.stSem);
    //FIND_ENTRY_BY_ADDR(pstPde, pstEntry);
    pstEntry = RBTree_Find_Proc_Entry(pstPde);
    
    /* if buffer not empty , try echo to current terminal */
    if ( HI_NULL != pstEntry && pstEntry->stInfo.pfnCmdFunc )
    {   
        HI_INFO_UPROC( "ump_seq_write: proc=%p, entry=%p %d bytes\n", proc, pstPde, size); //548
        if (strlen( (HI_CHAR*)pstEntry->stInfo.stBuf.u32StartVirAddr))
        {
            HI_DRV_PROC_EchoHelper((HI_CHAR*)pstEntry->stInfo.stBuf.u32StartVirAddr);
        }
    }
    
end:
    UPROC_K_UNLOCK(g_stUProcParam.stSem);
    
    return size;
}

/*static*/ UMP_DIR_S * PROC_AddDir(const HI_CHAR* pszName, const HI_CHAR* pszParent, struct file *pstFile)
{
    UMP_DIR_S * pstDir;
    UMP_DIR_S * pstDirFind = HI_NULL;

    /* Check parameter */
    if ((HI_NULL == pszName) || (strlen(pszName) == 0) || (strlen(pszName) > MAX_PROC_NAME_LEN))
    {
        HI_ERR_UPROC("Invalid name\n"); //573
        return HI_NULL;
    }

    /* Find directory node, if exist, return success directlly */
    pstDirFind = RBTree_Find_Dirent(0, pszName);
    if (HI_NULL != pstDirFind)
    {
        HI_INFO_UPROC("Dir %s exist\n", pszName); //581
        return pstDirFind;
    }

    /* Alloc directory resource */
    pstDir = (UMP_DIR_S*)kmalloc(sizeof(UMP_DIR_S), GFP_KERNEL);
    if (HI_NULL == pstDir)
    {
        HI_ERR_UPROC("kmalloc fail\n"); //589
        return HI_NULL;
    }
    /* Make directory name */
    HI_OSAL_Snprintf(pstDir->dir_name, sizeof(pstDir->dir_name), "%s", pszName);

    pstDir->hash = full_name_hash(pszName, strlen(pszName)) & ~0x80000000;
    pstDir->Data_16.rb_node = HI_NULL;
    pstDir->parent = HI_NULL;
    pstDir->pstFile = pstFile;

    /* Make proc directory */
    pstDir->entry = proc_mkdir(pstDir->dir_name, g_pHisi_proc);
    if (HI_NULL == pstDir->entry)
    {
    	HI_ERR_UPROC("proc_mkdir fail\n"); //604
        kfree(pstDir);
        return HI_NULL;
    }
    HI_INFO_UPROC("Proc add dir %s, file=%p, entry=0x%p\n", pstDir->dir_name, pstFile, pstDir->entry); //608

    /**/
    if (HI_SUCCESS != RBTree_Insert_Dirent(0, pstDir))
    {
        HI_ERR_UPROC("Insert new dirent failed.\n"); //613
        remove_proc_entry(pstDir->dir_name, g_pHisi_proc);
        kfree(pstDir);
        return HI_NULL;
    }

    return pstDir;
}

/*static*/ HI_S32 PROC_RemoveDir(UMP_DIR_S * pstDir)
{
    /* Check parameter */
    if (HI_NULL == pstDir)
    {
        HI_ERR_UPROC("Invalid name\n"); //633
        return HI_FAILURE;
    }
    
    /* If there are entries in this directory, remove fail */
    if (pstDir->Data_16.rb_node) //!(list_empty(&pstDir->entry_list)))
    {
        HI_ERR_UPROC("dir %s non-null\n", pstDir->dir_name); //640
        return HI_FAILURE;
    }

    /* Remove proc directory */
    remove_proc_entry(pstDir->dir_name, g_pHisi_proc);

    HI_INFO_UPROC("Proc remove dir %s\n", pstDir->dir_name); //647

#if 0
    /* Remove directory from list */
    list_del(&pstDir->dir_node);

    /* Free directory resource */
    HI_VFREE(HI_ID_PROC, pstDir);
#else
    rb_erase(&pstDir->rb_node, &Data_81109414);
    kfree(pstDir);
#endif

    return HI_SUCCESS;
}

/*static*/ HI_S32 PROC_RemoveDirByName(const HI_CHAR* pszName)
{
    HI_CHAR aszDir[MAX_PROC_NAME_LEN+12];
    UMP_DIR_S * pstDir = HI_NULL;

    /* Check parameter */
    if ((HI_NULL == pszName) || (strlen(pszName) == 0) ||  (strlen(pszName) > MAX_PROC_NAME_LEN))
    {
        HI_ERR_UPROC("Invalid name\n"); //666
        return HI_FAILURE;
    }
    
    /* Make directory name */
    HI_OSAL_Snprintf(aszDir, sizeof(aszDir), "%s", pszName);

    /* Find directory node */
    pstDir = RBTree_Find_Dirent(0, aszDir);
    if (HI_NULL == pstDir)
    {
        HI_ERR_UPROC("Find dir %s fail\n", aszDir); //677
        return HI_FAILURE;
    }

    return PROC_RemoveDir(pstDir);
}

/* complete */
static UMP_DIR_S * PROC_AddPrivateDir(const HI_CHAR* pszName, struct proc_dir_entry *pstEntry)
{
    UMP_DIR_S * pstDir = 0;

    /* Check parameter */
    if ((HI_NULL == pszName) || (strlen(pszName) > MAX_PROC_NAME_LEN) || (HI_NULL == pstEntry))
    {
        return pstDir;
    }

    /* Alloc directory resource */
    pstDir = (UMP_DIR_S*)kmalloc(sizeof(UMP_DIR_S), GFP_KERNEL);

    if (HI_NULL == pstDir)
    {
        HI_ERR_UPROC("kmalloc fail\n"); //698
        return pstDir;
    }

    /* Init other parameter */
    HI_OSAL_Strncpy(pstDir->dir_name, pszName, sizeof(pstDir->dir_name)-1);

    pstDir->hash = full_name_hash(pszName, strlen(pszName)) & ~0x80000000;
    pstDir->Data_16.rb_node = NULL;
    pstDir->entry = pstEntry;
    pstDir->parent = HI_NULL;
    pstDir->pstFile = HI_NULL;

    /* Add directory to list */
    if (HI_SUCCESS != RBTree_Insert_Dirent(0, pstDir))
    {
    	HI_ERR_UPROC("Insert new dirent failed.\n"); //713
    	kfree(pstDir);
    	pstDir = 0;
    }

    return pstDir;
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
    pstDir = RBTree_Find_Dirent(0, pszName);
    if (HI_NULL == pstDir)
    {
        HI_ERR_UPROC("Find dir %s fail\n", pszName); //742
        return HI_FAILURE;
    }

#if 0
    /* Remove directory from list */
    list_del(&pstDir->dir_node);

    /* Free directory resource */
    HI_VFREE(HI_ID_PROC, pstDir);
#else
    rb_erase(&pstDir->rb_node, &Data_81109414);
    kfree(pstDir);
#endif

    return HI_SUCCESS;
}

/*static*/ struct file_operations ump_seq_fops = {
        .open = ump_seq_open,
        .read = seq_read,
        .write   = ump_seq_write,
        .llseek = seq_lseek,
        .release = ump_seq_release,
};

/* nearly complete */
/*static*/ UMP_ENTRY_S* PROC_AddEntry(const HI_DRV_USRMODEPROC_ENTRY_S* pstParam, HI_BOOL bUsrMode)
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
        HI_ERR_UPROC("Invalid name\n"); //775
        return HI_NULL;
    }

    /* Make parent directory name */
    if (pstParam->aszParent[0] == 0)
    {
    	strlcpy(aszDir, "hisi", sizeof(aszDir)-1);
    }
    else
    {
        HI_OSAL_Snprintf(aszDir, sizeof(aszDir), "%s", pstParam->aszParent);
    }

    /* Find directory node, if don't exist, return fail */
    pstDir = RBTree_Find_Dirent(0, aszDir);
    if (HI_NULL == pstDir)
    {
        HI_ERR_UPROC("Dir %s don't exist\n", pstParam->aszParent); //793
        return HI_NULL;
    }

    /* Find entry in the directory, if exist, return success directlly */
    pstEntry = RBTree_Find_Entry(pstDir, pstParam->aszParent);
    if (HI_NULL != pstEntry)
    {
        HI_INFO_UPROC("Entry %s exist\n", pstParam->aszName); //801
        return HI_NULL; //pstEntry;
    }

    /* Alloc entry resource */
    //pstEntry = HI_VMALLOC(HI_ID_PROC, sizeof(UMP_ENTRY_S));
    pstEntry = (UMP_ENTRY_S*)kmalloc(sizeof(UMP_ENTRY_S), GFP_KERNEL);

    if (HI_NULL == pstEntry)
    {
        HI_ERR_UPROC("kmalloc fail\n"); //809
        return HI_NULL;
    }
    memset(pstEntry, 0, sizeof(UMP_ENTRY_S));

    /* Create proc entry */
    pstEntry->stInfo.pEntry = proc_create(pstParam->aszName, 0/*umode_t mode*/, pstDir->entry, &ump_seq_fops);
    if (HI_NULL == pstEntry->stInfo.pEntry)
    {
//        HI_VFREE(HI_ID_PROC, pstEntry);
        HI_FATAL_UPROC("create_proc_entry fail\n"); //818
#if 0
        kfree(pstEntry);
        return HI_NULL;
#else
        goto error;
#endif
    }
    HI_INFO_UPROC("Proc add entry %s, file=%p, entry=0x%p\n", pstParam->aszName, pstParam->pFile, pstEntry->stInfo.pEntry); //822

    /* Init other parameter */
    HI_OSAL_Strncpy(pstEntry->entry_name, pstParam->aszName, sizeof(pstEntry->entry_name)-1);

    pstEntry->hash = full_name_hash(pstParam->aszName, strlen(pstParam->aszName)) & ~0x80000000;
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
            HI_FATAL_UPROC("Alloc MMZ fail:%#x\n", s32Ret); //841
            remove_proc_entry(pstEntry->entry_name, pstEntry->parent);
#if 0
            kfree(pstEntry);
            return HI_NULL;
#else
            goto error;
#endif
        }
        memset((HI_VOID*)pstEntry->stInfo.stBuf.u32StartVirAddr, 0, pstEntry->stInfo.stBuf.u32Size);
    }
    else
    {
        pstEntry->stInfo.stBuf.u32Size = 0;
        pstEntry->stInfo.stBuf.u32StartPhyAddr = 0;
        pstEntry->stInfo.stBuf.u32StartVirAddr = 0;
    }

    s32Ret = RBTree_Insert_Entry(0, pstDir, pstEntry);
    if (HI_SUCCESS != s32Ret)
    {
		HI_ERR_UPROC("Insert new file entry failed.\n"); //856
        remove_proc_entry(pstEntry->entry_name, pstEntry->parent);
#if 0
        kfree(pstEntry);
        return HI_NULL;
#else
        goto error;
#endif
    }

    return pstEntry;

error:
	kfree(pstEntry);
	return HI_NULL;
}

static HI_VOID PROC_RemoveEntry(UMP_DIR_S* pstDir, UMP_ENTRY_S * pstEntry)
{
    /* Check parameter */
    if ((HI_NULL == pstDir) || (HI_NULL == pstEntry))
    {
        return;
    }

    /* Remove proc entry */
    remove_proc_entry(pstEntry->entry_name, pstEntry->parent);
    HI_INFO_UPROC("Proc remove entry %s\n", pstEntry->entry_name); //882

    /* Free MMZ */
    if (0 != pstEntry->stInfo.stBuf.u32Size)
    {
        HI_DRV_MMZ_UnmapAndRelease(&(pstEntry->stInfo.stBuf));
    }
    
#if 0
    /* Remove entry from list */
    list_del(&pstEntry->entry_node);
#endif
    rb_erase(&pstEntry->rb_node, &pstDir->Data_16);

    /* If current command belongs to this entry, clear it. */
    if (g_stUProcParam.current_cmd.pEntry == (HI_VOID*)&(pstEntry->stInfo))
    {
        g_stUProcParam.current_cmd.pEntry = HI_NULL;
        memset(&g_stUProcParam.current_cmd, 0, sizeof(g_stUProcParam.current_cmd));
    }

    /* Free resource */
    //HI_VFREE(HI_ID_PROC, pstEntry);
    kfree(pstEntry);

    return;
}

/*static*/ HI_S32 PROC_RemoveEntryByName(const HI_CHAR* pszName, const HI_CHAR* pszParent)
{
    UMP_ENTRY_S * pstEntry = HI_NULL;
    UMP_DIR_S * pstDir = HI_NULL;
    HI_CHAR aszDir[MAX_PROC_NAME_LEN+12];

    /* Check parameter */
    if ((HI_NULL == pszName) || (strlen(pszName) > MAX_PROC_NAME_LEN))
    {
        HI_ERR_UPROC("Invalid name\n"); //915
        return HI_FAILURE;
    }
    if ((HI_NULL == pszParent) || (strlen(pszParent) > MAX_PROC_NAME_LEN))
    {
        HI_ERR_UPROC("Invalid parent name\n"); //920
        return HI_FAILURE;
    }

    /* Make parent directory name */
    if (pszParent[0] == 0)
    {
    	strlcpy(aszDir, "hisi", sizeof(aszDir)-1);
    }
    else
    {
        HI_OSAL_Snprintf(aszDir, sizeof(aszDir), "%s", pszParent);
    }

    /* Find directory node, if don't exist, return fail */
    pstDir = RBTree_Find_Dirent(0, aszDir);
    if (HI_NULL == pstDir)
    {
        HI_ERR_UPROC("Dir %s don't exist\n", pszParent); //938
        return HI_FAILURE;
    }

    /* Find entry in the directory, if don't exist, return fail */
    pstEntry = RBTree_Find_Entry(pstDir, pszName);
    if (HI_NULL == pstEntry)
    {
        HI_WARN_UPROC("Entry %s don't exist\n", pszName); //946
        return HI_FAILURE;
    }

    /* Remove entry */
    PROC_RemoveEntry(pstDir, pstEntry);

    return HI_SUCCESS;
}

static HI_VOID PROC_RemoveDirForcibly(UMP_DIR_S *pstDir)
{
    /* Check parameter */
    if (HI_NULL == pstDir)
    {
        return;
    }
    
    HI_INFO_UPROC("Proc remove dir: %s\n", pstDir->dir_name); //967

    while (pstDir->Data_16.rb_node != 0)
    {
    	struct rb_node *node = rb_first(&pstDir->Data_16);
    	UMP_ENTRY_S* pstEntry = rb_entry(node, UMP_ENTRY_S, rb_node);

    	PROC_RemoveEntry(pstDir, pstEntry);
    }

    PROC_RemoveDir(pstDir);
}

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
            
            UPROC_K_LOCK(g_stUProcParam.stSem);
            pstEntry = PROC_AddEntry(&ump_entry, HI_TRUE);
            UPROC_K_UNLOCK(g_stUProcParam.stSem);

            if (HI_NULL == pstEntry)
            {
                s32Ret = HI_FAILURE;
                break;
            }
            
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
            s32Ret = (PROC_AddDir(aszName, HI_NULL, file) != NULL)? HI_SUCCESS: HI_FAILURE;
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
        HI_ERR_UPROC("%s device register failed\n", HI_USERPROC_DEVNAME); //1146
    }

    Data_81109544 = PROC_AddPrivateDir("hisi", g_pHisi_proc);
    if (Data_81109544 == NULL)
    {
        HI_ERR_UPROC("add 'hisi' directory failed.\n"); //1152
        return HI_FAILURE;
    }

    Data_81109540 = PROC_AddPrivateDir("msp", g_pCMPI_proc);
    if (Data_81109540 == NULL)
    {
        HI_ERR_UPROC("add 'msp' directory failed.\n"); //1160
        return HI_FAILURE;
    }

    return ret;
}

HI_VOID USRPROC_DRV_ModExit(HI_VOID)
{
	if (Data_81109544)
	{
		PROC_RemovePrivateDir(Data_81109544->dir_name);
		Data_81109544 = 0;
	}

	if (Data_81109540)
	{
		PROC_RemovePrivateDir(Data_81109540->dir_name);
		Data_81109540 = 0;
	}

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
