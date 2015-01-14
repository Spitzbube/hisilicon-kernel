/***********************************************************************************
*              Copyright 2004 - 2050, Hisilicon Tech. Co., Ltd.
*                           ALL RIGHTS RESERVED
* FileName: hi_gpio.c
* Description:
*
* History:
* Version   Date                Author       DefectNum    Description
* main\1    2007-10-16      w54542      NULL           Create this file.
***********************************************************************************/
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/string.h>
#include <linux/poll.h>
#include <linux/interrupt.h>
#include <linux/cdev.h>
#include <linux/ioctl.h>
#include <linux/pm.h>
#include <mach/hardware.h>
#include <asm/irq.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/miscdevice.h>

#include "hi_kernel_adapt.h"
#include "hi_common.h"
#include "hi_error_mpi.h"
#include "drv_gpio.h"
#include "drv_gpio_ioctl.h"
#include "drv_gpio_ext.h"
#include "hi_drv_dev.h"

static struct semaphore g_GpioSemIntf;   // kf39117 g_GpioSemIntf;

static long hi_gpio_ioctl(struct file *filp, HI_U32 cmd, unsigned long arg)
{
    long Ret = HI_SUCCESS;

    HI_VOID __user *argp = (HI_VOID __user*)arg;
    GPIO_DATA_S GPIOData;
    GPIO_INT_S GpioIntValue;

    Ret = down_interruptible(&g_GpioSemIntf);
    if (Ret)
    {
        HI_INFO_GPIO("Semaphore lock is  error. \n"); //51
        return HI_FAILURE;
    }

    switch (cmd)
    {
    case CMD_GPIO_SET_INT_TYPE:
    {
        if (copy_from_user(&GPIOData, argp, sizeof(GPIO_DATA_S)))
        {
            HI_INFO_GPIO("copy data from user fail!\n");
            Ret = HI_FAILURE;
            break;
        }

        Ret = HI_DRV_GPIO_SetIntType(GPIOData.u32GpioNo, GPIOData.enIntType);
        break;
    }

    case CMD_GPIO_SET_INT_ENABLE:
    {
        if (copy_from_user(&GPIOData, argp, sizeof(GPIO_DATA_S)))
        {
            HI_INFO_GPIO("copy data from user fail!\n");
            Ret = HI_FAILURE;
            break;
        }

        if (HI_TRUE == GPIOData.bEnable)
        {
            Ret = HI_DRV_GPIO_ClearBitInt(GPIOData.u32GpioNo);
        }

        Ret |= HI_DRV_GPIO_SetBitIntEnable(GPIOData.u32GpioNo, GPIOData.bEnable);

        break;
    }

    case CMD_GPIO_GET_INT:
    {
        if (copy_from_user(&GpioIntValue, argp, sizeof(GPIO_INT_S)))
        {
            HI_INFO_GPIO("copy data from user fail!\n");
            Ret = HI_FAILURE;
            break;
        }

        Ret = DRV_GPIO_QueryInt (&GpioIntValue);
        if (HI_SUCCESS == Ret)
        {
            if (copy_to_user(argp, &GpioIntValue, sizeof(GPIO_INT_S)))
            {
                HI_INFO_GPIO("copy data to user fail!\n");
                Ret = HI_FAILURE;
            }
        }
        else
        {
            Ret = HI_ERR_GPIO_FAILED_GETINT;
        }

        break;
    }

    case CMD_GPIO_GET_GPIONUM:
    {
        GPIO_GET_GPIONUM_S GpioNum;
        HI_DRV_GPIO_GetGpioNum(&GpioNum);

        if (copy_to_user(argp, &GpioNum, sizeof(GPIO_GET_GPIONUM_S)))
        {
            HI_INFO_GPIO("copy data to user fail!\n");
            Ret = HI_FAILURE;
        }

        Ret = HI_SUCCESS;
        break;
    }

	case CMD_GPIO_SET_OUTPUTTYPE:
	{
		GPIO_OUTPUT_TYPE_S stOutputType;

		if (copy_from_user(&stOutputType, argp, sizeof(GPIO_OUTPUT_TYPE_S)))
        {
            HI_INFO_GPIO("copy data from user fail!\n");
            Ret = HI_FAILURE;
        }

        Ret = DRV_GPIO_SetOutputType(stOutputType.u32GpioNo, stOutputType.enOutputType);
		break;
	}

	case CMD_GPIO_GET_OUTPUTTYPE:
	{
		GPIO_OUTPUT_TYPE_S stOutputType;
		
		if (copy_from_user(&stOutputType, argp, sizeof(GPIO_OUTPUT_TYPE_S)))
        {
            HI_INFO_GPIO("copy data from user fail!\n");
            Ret = HI_FAILURE;
        }

		Ret = DRV_GPIO_GetOutputType(stOutputType.u32GpioNo, &stOutputType.enOutputType);

        if (copy_to_user(argp, &stOutputType, sizeof(GPIO_OUTPUT_TYPE_S)))
        {
            HI_INFO_GPIO("copy data to user fail!\n");
            Ret = HI_FAILURE;
        }

		break;
	}

    default:
    {
        up(&g_GpioSemIntf);
        return -ENOIOCTLCMD;
    }
    }

    up(&g_GpioSemIntf);
    return Ret;
}

static struct file_operations hi_gpio_fops =
{
    .read			= NULL,
    .write			= NULL,
    .open			= DRV_GPIO_Open,
    .release		= DRV_GPIO_Close,
    .unlocked_ioctl = hi_gpio_ioctl,
};

extern HI_S32 gpio_pm_suspend (PM_BASEDEV_S *pdev, pm_message_t state);
extern HI_S32 gpio_pm_resume  (PM_BASEDEV_S *pdev);

static PM_BASEOPS_S gpio_baseOps =
{
    .probe        = NULL,
    .remove       = NULL,
    .shutdown     = NULL,
    .prepare      = NULL,
    .complete     = NULL,
    .suspend      = gpio_pm_suspend,
    .suspend_late = NULL,
    .resume_early = NULL,
    .resume       = gpio_pm_resume
};

static UMAP_DEVICE_S g_GpioRegisterData;

HI_S32 GPIO_DRV_ModInit(HI_VOID)
{
    HI_INIT_MUTEX(&g_GpioSemIntf);

#ifndef HI_MCE_SUPPORT
 #ifndef HI_KEYLED_CT1642_KERNEL_SUPPORT
//    HI_DRV_GPIO_Init();
 #endif
#endif

    snprintf(g_GpioRegisterData.devfs_name, sizeof(g_GpioRegisterData.devfs_name), UMAP_DEVNAME_GPIO);
    g_GpioRegisterData.minor = UMAP_MIN_MINOR_GPIO;
    g_GpioRegisterData.owner = THIS_MODULE;
    g_GpioRegisterData.fops   = &hi_gpio_fops;
    g_GpioRegisterData.drvops = &gpio_baseOps;
    if (HI_DRV_DEV_Register(&g_GpioRegisterData) < 0)
    {
        HI_INFO_GPIO("HI_DRV_DEV_Register err ! \n"); //221
        goto err0;
    }

#ifdef MODULE
    HI_PRINT("Load hi_gpio.ko success.\t(%s)\n", VERSION_STRING);
#endif

    return HI_SUCCESS;

err0:
#ifndef HI_MCE_SUPPORT
 #ifndef HI_KEYLED_CT1642_KERNEL_SUPPORT
//    HI_DRV_GPIO_DeInit();
 #endif
#endif
    return -EFAULT;
}

HI_VOID GPIO_DRV_ModExit(HI_VOID)
{
    HI_DRV_DEV_UnRegister(&g_GpioRegisterData);

#ifndef HI_MCE_SUPPORT
 #ifndef HI_KEYLED_CT1642_KERNEL_SUPPORT
//    HI_DRV_GPIO_DeInit();
 #endif
#endif

#ifdef MODULE
    HI_PRINT("remove hi_gpio.ko ok!\n");
#endif
    return;
}

#ifdef MODULE
module_init(GPIO_DRV_ModInit);
module_exit(GPIO_DRV_ModExit);
#endif

MODULE_LICENSE("GPL");
