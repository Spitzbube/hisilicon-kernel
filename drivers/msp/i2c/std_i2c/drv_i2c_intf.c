/*  extdrv/interface/i2c/hi_i2c.c
 *
 * Copyright (c) 2006 Hisilicon Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program;
 *
 * History:
 *      19-April-2006 create this file
 */
#include <linux/device.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/delay.h>
#include <linux/poll.h>
#include <mach/hardware.h>
#include <linux/interrupt.h>
#include <linux/slab.h>

#include "hi_drv_dev.h"
#include "hi_drv_proc.h"
#include "drv_i2c.h"
#include "drv_i2c_ext.h"
#include "hi_drv_i2c.h"
#include "drv_i2c_ioctl.h"
#include "hi_common.h"
#include "drv_gpioi2c_ext.h"
#include "hi_drv_module.h"
#include "hi_drv_mem.h"

#define I2C_WRITE_REG(Addr, Value) ((*(volatile HI_U32 *)(Addr)) = (Value))
#define I2C_READ_REG(Addr) (*(volatile HI_U32 *)(Addr))

extern int     i2c_pm_resume(PM_BASEDEV_S *pdev);
extern int     i2c_pm_suspend(PM_BASEDEV_S *pdev, pm_message_t state);

extern HI_S32  I2C_Ioctl(struct file *file, unsigned int cmd, unsigned long arg);

static UMAP_DEVICE_S g_I2cRegisterData;

static GPIO_I2C_EXT_FUNC_S* g_pstGpioI2cExtFuncs = HI_NULL;

extern HI_U32 g_aI2cRate[HI_STD_I2C_NUM];
#ifdef HI_GPIOI2C_SUPPORT
extern I2C_GPIO_S g_stI2cGpio[HI_I2C_MAX_NUM];
#endif

HI_S32 I2C_ProcRead(struct seq_file *p, HI_VOID *v)
{
	HI_U8 ii = 0;
    PROC_PRINT(p, "---------Hisilicon Standard I2C Info---------\n");
	PROC_PRINT(p, "No.            Rate\n");

	for (ii = 0; ii < HI_STD_I2C_NUM; ii++)
	{
		PROC_PRINT(p, "%d             %d\n", ii, g_aI2cRate[ii]);
	}

#ifdef HI_GPIOI2C_SUPPORT
	PROC_PRINT(p, "---------Hisilicon GPIO simulate I2C Info---------\n");
	PROC_PRINT(p, "No.            SCL_IO       SDA_IO\n");
	for (ii = 0; ii < HI_I2C_MAX_NUM; ii++)
	{
		if (g_stI2cGpio[ii].bUsed)
		{
			PROC_PRINT(p, "%d                %d           %d\n", g_stI2cGpio[ii].I2cNum,
							g_stI2cGpio[ii].u32SCLGpioNo, g_stI2cGpio[ii].u32SDAGpioNo);
		}		
	}
#endif

    return HI_SUCCESS;
}


HI_S32 I2C_ProcWrite(struct file * file, const char __user * buf, size_t count, loff_t *ppos)
{
    HI_U32 u32I2cNo = 0, u32DevAddr = 0, u32RegAddr = 0, u32Val = 0;
    HI_S32 s32Ret = HI_SUCCESS;
    char acInput[256];
	char tmp[256];
    char *p  = HI_NULL;
    HI_U32 i = 0;
    HI_U8 SendData[32] = {0};
    HI_U8 u8SendCount = 0;
    HI_U8 u8ArgCount = 0;
    HI_BOOL bCommondErr = HI_FALSE;
	HI_BOOL bSetRate = HI_FALSE;
	HI_U32 u32Rate = 0;

    if (count >= sizeof(acInput))
    {
        HI_PRINT("commond line is too long, please try it in 256 Bytes\n");
        return HI_FAILURE;
    }

    memset(acInput, 0, sizeof(acInput));
    if (copy_from_user(acInput, buf, count))
    {
        //printk("copy from user failed\n");
        return HI_FAILURE;
    }
	
	acInput[255] = '\0';

	/*begin deal with set rate cmd*/
	p = acInput;
	u8ArgCount = 0;
	for (i = 0; i < count && i < strlen(acInput); i++)
    {
        if ((acInput[i] == ' ') || (acInput[i] == '\n'))
        {
        	memset(tmp, 0, sizeof(tmp));
			memcpy(tmp, p, i);
			tmp[255] = '\0';
			u8ArgCount++;
			p = &acInput[i+1];

			if (1 == u8ArgCount)
			{
				if (0 == strncmp(tmp, "SetRate", strlen("SetRate")))
				{
					bSetRate = HI_TRUE;
				}
				else
				{
					break;
				}
			}
			else if (2 == u8ArgCount)
			{
				u32I2cNo = simple_strtol(tmp, NULL, 10);
			}
			else if (3 == u8ArgCount)
			{
				u32Rate = simple_strtol(tmp, NULL, 10);
				break;
			}
        }
    }

	if (bSetRate)
	{
		if (u32I2cNo >= HI_STD_I2C_NUM)
		{
			HI_PRINT("I2c NO.%d not support rate setting!\n", u32I2cNo);
		}
		else
		{
			I2C_DRV_SetRate(u32I2cNo, u32Rate);
		}
		
		return count;
	}
	/*end deal with set rate cmd*/
	
    p = acInput;
    u8ArgCount = 0;

    for (i = 0; i < count && i < strlen(acInput); i++)
    {
        if ((acInput[i] == ' ') || (acInput[i] == '\n'))
        {
            u8ArgCount++;
        }
    }

    do
    {
        if (u8ArgCount < 3)
        {
            bCommondErr = HI_TRUE;
            break;
        }

        u32I2cNo   = (HI_U32)simple_strtoul(p, &p, 16);
        u32DevAddr = (HI_U32)simple_strtoul(p + 1, &p, 16);
        u32RegAddr = (HI_U32)simple_strtoul(p + 1, &p, 16);

        if (u8ArgCount >= 4)
        {
            u32Val = (HI_U32)simple_strtoul(p + 1, &p, 16);

            if (u8ArgCount > 4)
            {
                if (u32Val != (u8ArgCount - 4))
                {
                    bCommondErr = HI_TRUE;
                    break;
                }

                if ((u8ArgCount - 4) > sizeof(SendData))
                {
                    bCommondErr = HI_TRUE;
                    break;
                }

                for (i = 0; i < (u8ArgCount - 4); i++)
                {
                    SendData[i] = (HI_U8)simple_strtoul(p + 1, &p, 16);
                }

                u8SendCount = u8ArgCount - 4;
            }
            else
            {
                SendData[0] = u32Val;
                u8SendCount = 1;
            }
        }

        if ((u32I2cNo >= HI_STD_I2C_NUM) && (g_pstGpioI2cExtFuncs == HI_NULL))
        {
            s32Ret = HI_DRV_MODULE_GetFunction(HI_ID_GPIO_I2C, (HI_VOID * *)&g_pstGpioI2cExtFuncs);
            if ((HI_SUCCESS != s32Ret) || (HI_NULL == g_pstGpioI2cExtFuncs))
            {
                HI_PRINT("GPIO_I2C Function ERR: ret:0x%08x, func:0x%08x\n", s32Ret,
                       (HI_U32)g_pstGpioI2cExtFuncs);
                break;
            }
        }

        if (u8ArgCount == 3)
        {
            HI_PRINT("Read: u32I2cNo=%d, u32DevAddr=0x%x, u32RegAddr=0x%x\n", u32I2cNo, u32DevAddr, u32RegAddr);
            if (u32I2cNo < HI_STD_I2C_NUM)
            {
                s32Ret = HI_DRV_I2C_Read(u32I2cNo, (HI_U8)u32DevAddr, u32RegAddr, 1, (HI_U8 *)&u32Val, 1);
                if (HI_SUCCESS != s32Ret)
                {
                    HI_ERR_I2C("Read failed(Ret:0x%x)\n\n", s32Ret);
                }
                else
                {
                    HI_PRINT("0x%x\n", u32Val);
                }
            }
            else
            {
                if (g_pstGpioI2cExtFuncs && g_pstGpioI2cExtFuncs->pfnGpioI2cIsUsed)
                {
                    HI_BOOL bUsed = HI_FALSE;

                    s32Ret = g_pstGpioI2cExtFuncs->pfnGpioI2cIsUsed(u32I2cNo, &bUsed);

                    if ((HI_SUCCESS == s32Ret) && (HI_TRUE == bUsed))
                    {
                        if (g_pstGpioI2cExtFuncs->pfnGpioI2cReadExt)
                        {
                            s32Ret = g_pstGpioI2cExtFuncs->pfnGpioI2cReadExt(u32I2cNo, u32DevAddr, u32RegAddr, 1,
                                                                             (unsigned char *)&u32Val, 1);
                            if (HI_SUCCESS != s32Ret)
                            {
                                HI_ERR_I2C("Read failed(Ret:0x%x)\n\n", s32Ret);
                            }
                            else
                            {
                                HI_PRINT("0x%x\n", u32Val);
                            }
                        }
                    }
                    else
                    {
                        HI_INFO_I2C("Read failed(I2cNum=%d not valid)! \n", u32I2cNo);
                    }
                }
                else
                {
                    HI_INFO_I2C("Read failed(I2cNum=%d not valid)! \n", u32I2cNo);
                }
            }
        }
        else
        {
            HI_PRINT("Write: u32I2cNo=%d, u32DevAddr=0x%x, u32RegAddr=0x%x, write number=0x%x\n", u32I2cNo,
                   u32DevAddr, u32RegAddr, u8SendCount);
            if (u32I2cNo < HI_STD_I2C_NUM)
            {
                s32Ret = HI_DRV_I2C_Write(u32I2cNo, (HI_U8)u32DevAddr, u32RegAddr, 1, SendData, u8SendCount);
                if (HI_SUCCESS != s32Ret)
                {
                    HI_ERR_I2C("Write failed(Ret:0x%x)", s32Ret);
                }
            }
            else
            {
                if (g_pstGpioI2cExtFuncs && g_pstGpioI2cExtFuncs->pfnGpioI2cIsUsed)
                {
                    HI_BOOL bUsed = HI_FALSE;

                    s32Ret = g_pstGpioI2cExtFuncs->pfnGpioI2cIsUsed(u32I2cNo, &bUsed);

                    if ((HI_SUCCESS == s32Ret) && (HI_TRUE == bUsed))
                    {
                        if (g_pstGpioI2cExtFuncs->pfnGpioI2cWriteExt)
                        {
                            s32Ret = g_pstGpioI2cExtFuncs->pfnGpioI2cWriteExt(u32I2cNo, u32DevAddr, u32RegAddr, 1,
                                                                              SendData, u8SendCount);
                            if (HI_SUCCESS != s32Ret)
                            {
                                HI_ERR_I2C("Write failed(Ret:0x%x)\n\n", s32Ret);
                            }
                        }
                    }
                    else
                    {
                        HI_INFO_I2C("Write failed(I2cNum=%d not valid)! \n", u32I2cNo);
                    }
                }
                else
                {
                    HI_INFO_I2C("Write failed(I2cNum=%d not valid)! \n", u32I2cNo);
                }
            }
        }
    } while (0);

    if (bCommondErr)
    {
        HI_PRINT("\nUsage:\n");
        HI_PRINT("Read data: echo 'bus' 'device address' 'Register address' > /proc/msp/i2c\n");
        HI_PRINT("Write data: echo 'bus' 'device address' 'Register address' 'data' > /proc/msp/i2c\n");
        HI_PRINT("or:echo 'bus' 'device address' 'Register address' 'data number n(n<=32)' 'data1' ...'datan > /proc/msp/i2c\n");
        HI_PRINT("such as: echo 4 a0 5d 2 b c > /proc/msp/i2c \n");
		HI_PRINT("Set Standard i2c rate: echo SetRate 'bus' 'rate' > /proc/msp/i2c .such as :echo SetRate 1 100000  > /proc/msp/i2c\n");
		HI_PRINT("Look over i2c info: cat /proc/msp/i2c\n");
    }

    return count;
}

static PM_BASEOPS_S i2c_drvops =
{
    .probe        = NULL,
    .remove       = NULL,
    .shutdown     = NULL,
    .prepare      = NULL,
    .complete     = NULL,
    .suspend      = i2c_pm_suspend,
    .suspend_late = NULL,
    .resume_early = NULL,
    .resume       = i2c_pm_resume,
};

/*****************************************************************************/

static long I2C_INTF_Ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    long ret = HI_FAILURE;
    I2C_DATA_S I2cData;
    void __user *argp = (void __user*)arg;

    switch (cmd)
    {
    /* if I2C channel num < 4 ,when execute ioctl select I2C_Ioctl,otherwise selet HI_DRV_GPIOI2C_Ioctl,  the same macro will compatible i2c and gpio-i2c*/
    case CMD_I2C_WRITE:
    case CMD_I2C_READ:
        {
            if (copy_from_user(&I2cData, argp, sizeof(I2C_DATA_S)))
            {
                HI_INFO_I2C("copy data from user fail!\n");
                ret = HI_ERR_I2C_COPY_DATA_ERR;
                return ret;
            }

            if (HI_STD_I2C_NUM <= I2cData.I2cNum)
            {
                if (I2cData.I2cNum >= HI_I2C_MAX_NUM)
                {
                    HI_INFO_I2C("i2c chanenl (NO:%d) is invalid !\n", I2cData.I2cNum);
                    return HI_ERR_I2C_INVALID_PARA;
                }

                if (g_pstGpioI2cExtFuncs && g_pstGpioI2cExtFuncs->pfnGpioI2cIOCtl)
                {
                    ret = g_pstGpioI2cExtFuncs->pfnGpioI2cIOCtl(file, cmd, arg);
                }
            }
            else
            {
                ret = I2C_Ioctl(file, cmd, arg);
            }

            break;
        }

    case CMD_I2C_SET_RATE:
        {
            ret = I2C_Ioctl(file, cmd, arg);
            break;
        }

    case CMD_I2C_CONFIG:
    case CMD_I2C_DESTROY:
        {
            if (g_pstGpioI2cExtFuncs && g_pstGpioI2cExtFuncs->pfnGpioI2cIOCtl)
            {
                ret = g_pstGpioI2cExtFuncs->pfnGpioI2cIOCtl(file, cmd, arg);
            }

            break;
        }

    default:
        {
            ret = -ENOIOCTLCMD;
            break;
        }
    }

    return ret;
}

HI_S32 I2C_Open(struct inode *inode, struct file *filp)
{
    HI_S32 s32Ret = HI_SUCCESS;

    s32Ret = HI_DRV_MODULE_GetFunction(HI_ID_GPIO_I2C, (HI_VOID * *)&g_pstGpioI2cExtFuncs);
    if ((HI_SUCCESS != s32Ret) || (HI_NULL == g_pstGpioI2cExtFuncs))
    {
        HI_INFO_I2C("Get GPIO_I2C Function ERR: ret:0x%08x, func:0x%08x\n", s32Ret, g_pstGpioI2cExtFuncs);
    }

    return HI_SUCCESS;
}

HI_S32 I2C_Close(struct inode *inode, struct file *filp)
{
    return HI_SUCCESS;
}

static struct file_operations I2C_FOPS =
{
	owner: THIS_MODULE,
	open: I2C_Open,
	unlocked_ioctl: I2C_INTF_Ioctl,
	release: I2C_Close,
};

HI_S32 I2C_DRV_ModInit(HI_VOID)
{
    DRV_PROC_ITEM_S  *pProcItem;

    /* pre */

    //#ifndef HI_MCE_SUPPORT
    HI_DRV_I2C_Init(); /*HI_MCE_SUPPORT=yes or HI_MCE_SUPPORT=no, all will call HI_DRV_I2C_Init, make sure HI_I2C_Open call it */
    //#endif

    /* I2C driver register */
    snprintf(g_I2cRegisterData.devfs_name, sizeof(g_I2cRegisterData.devfs_name), UMAP_DEVNAME_I2C);
    g_I2cRegisterData.minor = UMAP_MIN_MINOR_I2C;
    g_I2cRegisterData.owner = THIS_MODULE;
    g_I2cRegisterData.fops   = &I2C_FOPS;
    g_I2cRegisterData.drvops = &i2c_drvops;
    if (HI_DRV_DEV_Register(&g_I2cRegisterData) < 0)
    {
        HI_FATAL_I2C("register I2C failed.\n");
        return HI_FAILURE;
    }

    /* register i2c PROC funtion*/
    pProcItem = HI_DRV_PROC_AddModule(HI_MOD_I2C, HI_NULL, HI_NULL);
    if (!pProcItem)
    {
        HI_INFO_I2C("add I2C proc failed.\n");
        HI_DRV_DEV_UnRegister(&g_I2cRegisterData);
        return HI_FAILURE;
    }

    pProcItem->read  = I2C_ProcRead;
    pProcItem->write = I2C_ProcWrite;

#ifdef MODULE
    HI_PRINT("Load hi_i2c.ko success.  \t(%s)\n", VERSION_STRING);
#endif
    return 0;
}

HI_VOID I2C_DRV_ModExit(HI_VOID)
{
    HI_DRV_PROC_RemoveModule(HI_MOD_I2C);
    HI_DRV_DEV_UnRegister(&g_I2cRegisterData);

    //#ifndef HI_MCE_SUPPORT
    HI_DRV_I2C_DeInit();
    //#endif

    return;
}

#ifdef MODULE
module_init(I2C_DRV_ModInit);
module_exit(I2C_DRV_ModExit);
#endif

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("HISILICON");
