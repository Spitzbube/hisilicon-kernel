#ifndef TDE_BOOT
#include <linux/version.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/kernel.h>    /* printk() */
#include <linux/slab.h>      /* kmalloc() */
#include <linux/fs.h>        /* everything... */
#include <linux/errno.h> /* error codes */
#include <linux/types.h> /* size_t */
#include <linux/fcntl.h> /* O_ACCMODE */
#include <linux/cdev.h>
#include <asm/uaccess.h> /* copy_*_user */
#include <linux/interrupt.h>
#include <linux/poll.h>
#include <linux/workqueue.h>
#include <asm/io.h>
#include "tde_proc.h"
#else
#include "tde_osilist.h"
#endif
#include "tde_adp.h"
#include "tde_config.h"
#include "hi_type.h"

#define MKSTR(exp) # exp
#define MKMARCOTOSTR(exp) MKSTR(exp)
char* g_pszTdeMmzName = NULL;
module_param(g_pszTdeMmzName, charp, S_IRUGO);
MODULE_PARM_DESC(g_pszTdeMmzName, "The mmz name for tde.");

HI_U32 g_u32TdeTmpBuf = 0;
module_param(g_u32TdeTmpBuf, uint, S_IRUGO);

MODULE_PARM_DESC(g_u32TdeTmpBuf, "TDE Tmp buffer.");
#ifndef TDE_BOOT
extern int tde_init_module_k(void);
extern void tde_cleanup_module_k(void);
extern int tde_open(struct inode *finode, struct file  *ffile);
extern int tde_release(struct inode *finode, struct file  *ffile);

extern long tde_ioctl(struct file  *ffile, unsigned int  cmd, unsigned long arg); 
#ifdef CONFIG_TDE_PM_ENABLE
extern int tde_pm_suspend(PM_BASEDEV_S *pdev, pm_message_t state);
extern int tde_pm_resume(PM_BASEDEV_S *pdev);
#endif
DECLARE_GFX_NODE("hi_tde",tde_open, tde_release, NULL, tde_ioctl, tde_pm_suspend, tde_pm_resume);
#endif
#ifdef TDE_BOOT
int  tde_osr_init(void)
{

    if (TdeHalInit(TDE_REG_BASEADDR) < 0)
    {
        return -1;
    }

    TdeOsiListInit();

    TDE_TRACE(TDE_KERN_DEBUG, "init tde successful!\n");
    return 0;
}
void tde_osr_deinit(void)
{
    TdeOsiListTerm();

    TdeHalRelease();

 
    return;
}
#endif
#ifndef TDE_BOOT
HI_S32  TDE_DRV_ModInit(HI_VOID)
{
    int ret = 0;
#ifndef HI_MCE_SUPPORT
    ret = tde_init_module_k();
    if (0 != ret)
    {
        return -1;
    }    
#endif
#ifdef CONFIG_TDE_PM_ENABLE
    /* register tde device */
    /*ret =*/ HI_GFX_PM_Register();
    if(0!=ret)
    {
        return -1;    
    }
#endif
#ifndef CONFIG_TDE_PROC_DISABLE
{
   GFX_PROC_ITEM_S pProcItem = {tde_read_proc,tde_write_proc,NULL};
   HI_GFX_PROC_AddModule("tde", &pProcItem, NULL);
}    
#endif

#ifndef CONFIG_TDE_VERSION_DISABLE
   HI_GFX_ShowVersionK(HIGFX_TDE_ID);
#endif
    return 0;
}

HI_VOID  TDE_DRV_ModExit(HI_VOID)
{
#ifndef CONFIG_TDE_PROC_DISABLE
    HI_GFX_PROC_RemoveModule("tde");
#endif

#ifndef HI_MCE_SUPPORT
    tde_cleanup_module_k();
#endif
#ifdef CONFIG_TDE_PM_ENABLE
    /* cleanup_module is never called if registering failed */
    HI_GFX_PM_UnRegister();
#endif
}
#endif
#ifdef MODULE
module_init(TDE_DRV_ModInit);
module_exit(TDE_DRV_ModExit);
#endif
#ifndef TDE_BOOT
#ifndef HI_ADVCA_FUNCTION_RELEASE
MODULE_AUTHOR(AUTHOR);
MODULE_DESCRIPTION(DESCRIPTION);
MODULE_LICENSE(MPP_LICENSE);
//MODULE_VERSION(VERSION);
#else
MODULE_AUTHOR("");
MODULE_DESCRIPTION("");
MODULE_LICENSE(MPP_LICENSE);
MODULE_VERSION("");
#endif
#endif
