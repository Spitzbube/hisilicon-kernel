#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/file.h>
#include <linux/major.h>
#include <asm/uaccess.h>
#include "hi_type.h"
#include "hi_drv_proc.h"
#include "hi_osal.h"
#include "hi_debug.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* End of #ifdef __cplusplus */

static  DRV_PROC_INTFPARAM  *procIntfParam = NULL;

HI_S32 HI_DRV_PROC_RegisterParam(DRV_PROC_INTFPARAM *param)
{
	if (NULL == param)
	{
		//printk("CMPI_PROC_Register param err! \n");
		return HI_FAILURE;
	}
	if( (param->addModulefun == NULL) || 
		(param->rmvModulefun == NULL))
	{	
		//printk("CMPI_PROC_Register param err! \n");
		return HI_FAILURE;
	}
	procIntfParam = param;
    return HI_SUCCESS;
}

HI_VOID HI_DRV_PROC_UnRegisterParam(HI_VOID)
{
	procIntfParam = NULL;
	return;
}

DRV_PROC_ITEM_S* HI_DRV_PROC_AddModule(HI_CHAR *entry_name ,DRV_PROC_EX_S* pfnOpt, HI_VOID * data)
{
	if(procIntfParam){
		if(procIntfParam->addModulefun){
			return procIntfParam->addModulefun(entry_name, pfnOpt, data);
		}
	}
	return NULL;
}

HI_VOID HI_DRV_PROC_RemoveModule(char *entry_name)
{
	if(procIntfParam){
		if(procIntfParam->rmvModulefun){
			procIntfParam->rmvModulefun(entry_name);
		}
	}
	return;
}


HI_S32 HI_DRV_PROC_KInit(void)
{
	procIntfParam = NULL;
    return HI_SUCCESS;
}

HI_VOID HI_DRV_PROC_KExit(void)
{
	if(procIntfParam){
		HI_DRV_PROC_UnRegisterParam();
	}
    return ;
}

/*
 * echo string to current terminal display(serial console or tty).
 * this implement implicit that current task file handle '0' must be terminal device file. 
 * otherwise do nothing.
 */
HI_VOID HI_DRV_PROC_EchoHelper(const HI_CHAR *string)
{
#define DEFAULT_ECHO_DEVICE_HANDLE (0)

    struct kstat stat;
    HI_S32 ret;

    if (!string)
        return;
    
    ret = vfs_fstat(DEFAULT_ECHO_DEVICE_HANDLE, &stat);
    if (ret)
    {
        HI_PRINT("Default echo device handle(%u) invalid!\n", DEFAULT_ECHO_DEVICE_HANDLE);
        return;
    }

    /* echo device must be chrdev and major number must be TTYAUX_MAJOR or UNIX98_PTY_SLAVE_MAJOR */
    if ( S_ISCHR(stat.mode) && (MAJOR(stat.rdev) == TTYAUX_MAJOR || MAJOR(stat.rdev) == UNIX98_PTY_SLAVE_MAJOR) )
    {
        struct file *file = fget(DEFAULT_ECHO_DEVICE_HANDLE);
        if (file)
        {
		mm_segment_t stOld_FS = {0};
		/* file pos is invalid for chrdev */
		loff_t pos = 0;

		set_fs(KERNEL_DS);
		ret = vfs_write(file, string, strlen(string), &pos);
		if (ret < 0)
		{
			HI_PRINT("write to echo device failed(%d)!\n", ret);
		}
		set_fs(stOld_FS);
			
		fput(file);
	}
    }
    else
    {
        HI_PRINT("Default echo device is invalid!\n");
    }
}

/*
 * HI_DRV_PROC_EchoHelper extent interface support Variable parameter  
 */
HI_VOID HI_DRV_PROC_EchoHelperVargs(HI_CHAR *buf, HI_U32 size, const HI_CHAR * fmt, ...)
{
       va_list args;

       if (!buf)
            return;

       va_start(args, fmt);
       HI_OSAL_Vsnprintf(buf, size, fmt, args);
       va_end(args);

       HI_DRV_PROC_EchoHelper(buf);
       
}

#ifndef MODULE
EXPORT_SYMBOL(HI_DRV_PROC_RegisterParam);
EXPORT_SYMBOL(HI_DRV_PROC_UnRegisterParam);
#endif
EXPORT_SYMBOL(HI_DRV_PROC_AddModule);
EXPORT_SYMBOL(HI_DRV_PROC_RemoveModule);
EXPORT_SYMBOL(HI_DRV_PROC_EchoHelper);
EXPORT_SYMBOL(HI_DRV_PROC_EchoHelperVargs);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

