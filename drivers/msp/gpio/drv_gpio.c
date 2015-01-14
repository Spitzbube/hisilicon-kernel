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
#include <mach/hardware.h>
#include <asm/irq.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/miscdevice.h>

#include "hi_common.h"
#include "hi_error_mpi.h"
#include "drv_gpio_ioctl.h"
#include "hi_drv_dev.h"
#include "hi_drv_sys.h"
#include "hi_module.h"
#include "hi_drv_module.h"
#include "hi_reg_common.h"
#include "drv_gpio_ext.h"
#include "drv_gpio.h"

#define  GPIO_MAX_BUF 100
#define  GPIO_BUF_HEAD g_stGpioAttr.GpioIntBuf[g_stGpioAttr.Head]
#define  GPIO_BUF_TAIL g_stGpioAttr.GpioIntBuf[g_stGpioAttr.Tail]

#define  INC_BUF(x, len) (((x) + 1) % (len))

typedef struct
{
    HI_BOOL               GpioIntEnable;
    HI_UNF_GPIO_INTTYPE_E GpioIntType;
    HI_VOID (*GpioServer)(HI_U32 u32GpioNO);
} GPIO_INT_ATTR_S;

typedef struct
{
    HI_U32            Head;
    HI_U32            Tail;
    HI_U32            GpioIntBuf[GPIO_MAX_BUF];
    HI_U32            GpioBufLen;
    HI_U32            GpioBlockTime;
    GPIO_INT_ATTR_S   GpioIntAttr[HI_GPIO_MAX_BIT_NUM];
    wait_queue_head_t GpioIntWaitQueue;
} GPIO_ATTR_S;

static GPIO_ATTR_S g_stGpioAttr;

static HI_CHAR g_GpioIrqName[][16] = {
    "hi_gpio00_irq",
    "hi_gpio01_irq",
    "hi_gpio02_irq",
    "hi_gpio03_irq",
    "hi_gpio04_irq",
    "hi_gpio05_irq",
    "hi_gpio06_irq",
    "hi_gpio07_irq",
    "hi_gpio08_irq",
    "hi_gpio09_irq",
    "hi_gpio10_irq",
    "hi_gpio11_irq",
    "hi_gpio12_irq",
    "hi_gpio13_irq",
    "hi_gpio14_irq",
    "hi_gpio15_irq",
    "hi_gpio16_irq",
    "hi_gpio17_irq",
#if defined (CHIP_TYPE_hi3798cv100)
    "hi_gpio18_irq",
    "hi_gpio19_irq",
#endif
};

int  gpio_pm_suspend(PM_BASEDEV_S *pdev, pm_message_t state);
int  gpio_pm_resume(PM_BASEDEV_S *pdev);

static GPIO_EXT_FUNC_S g_stGpioExportFuncs =
{
    .pfnGpioDirGetBit            = HI_DRV_GPIO_GetDirBit,
    .pfnGpioDirSetBit            = HI_DRV_GPIO_SetDirBit,
    .pfnGpioReadBit              = HI_DRV_GPIO_ReadBit,
    .pfnGpioWriteBit             = HI_DRV_GPIO_WriteBit,
    .pfnGpioGetNum               = HI_DRV_GPIO_GetGpioNum,
    .pfnGpioRegisterServerFunc   = HI_DRV_GPIO_RegisterServerFunc,
    .pfnGpioUnRegisterServerFunc = HI_DRV_GPIO_UnregisterServerFunc,
    .pfnGpioSetIntType           = HI_DRV_GPIO_SetIntType,
    .pfnGpioSetIntEnable         = HI_DRV_GPIO_SetBitIntEnable,
    .pfnGpioClearGroupInt        = HI_DRV_GPIO_CearGroupInt,
    .pfnGpioClearBitInt          = HI_DRV_GPIO_ClearBitInt,
    .pfnI2cSuspend				 = gpio_pm_suspend,
    .pfnI2cResume				 = gpio_pm_resume,
};

static GPIO_GET_GPIONUM_S g_GpioNum;

static HI_S32 g_GpioUsrAddr[HI_GPIO_MAX_GROUP_NUM];
//static HI_CHIP_TYPE_E enChipType;
//static HI_CHIP_VERSION_E enChipVersion;
//static HI_U8 g_u8GpioGrpNum, g_u8GpioMaxNum;

int  gpio_pm_suspend(PM_BASEDEV_S *pdev, pm_message_t state)
{
    printk("GPIO suspend OK\n");
    return 0;
}

int  gpio_pm_resume(PM_BASEDEV_S *pdev)
{
    printk("GPIO resume OK\n");
    return 0;
}

static HI_BOOL DRV_GPIO_Convert(HI_U32 u32GpioNo, HI_U32 *pu32GroupNo, HI_U32 *pu32BitNo)
{
    if (u32GpioNo >= (HI_GPIO_GROUP_NUM * HI_GPIO_BIT_NUM)) //g_u8GpioMaxNum)
    {
        HI_ERR_GPIO("para u32GpioNo  is invalid.\n"); //126
        return HI_FALSE;
    }

    *pu32GroupNo = u32GpioNo / HI_GPIO_BIT_NUM;
    *pu32BitNo = u32GpioNo % HI_GPIO_BIT_NUM;

    return HI_TRUE;
}

HI_S32 HI_DRV_GPIO_SetDirBit(HI_U32 u32GpioNo, HI_U32 u32DirBit)
{
    HI_U32 GpioDirtValue;
    HI_U32 GpioUsrAddr;
    HI_U32 u32GroupNo;
    HI_U32 u32BitNo;

    if (HI_FALSE == DRV_GPIO_Convert(u32GpioNo, &u32GroupNo, &u32BitNo))
    {
        return HI_ERR_GPIO_INVALID_PARA;
    }

    if ((u32DirBit != HI_TRUE)
        && (u32DirBit != HI_FALSE)
    )
    {
        HI_ERR_GPIO("para bInput is invalid.\n");
        return HI_ERR_GPIO_INVALID_PARA;
    }

    GpioUsrAddr = g_GpioUsrAddr[u32GroupNo];

    GpioDirtValue = REG_USR_ADDR(GpioUsrAddr + HI_GPIO_DIR_REG);

    if (!u32DirBit)   /*0 output */
    {
        GpioDirtValue |= (HI_GPIO_OUTPUT << u32BitNo);
        REG_USR_ADDR(GpioUsrAddr + HI_GPIO_DIR_REG) = GpioDirtValue;
    }
    else /*1 input */
    {
        GpioDirtValue &= (~(1 << u32BitNo));
        REG_USR_ADDR(GpioUsrAddr + HI_GPIO_DIR_REG) = GpioDirtValue;
    }

    return HI_SUCCESS;
}

HI_S32 HI_DRV_GPIO_GetDirBit(HI_U32 u32GpioNo, HI_U32* pu32DirBit)
{
    HI_U32 GpioDirtValue;
    HI_U32 GpioUsrAddr;
    HI_U32 u32GroupNo;
    HI_U32 u32BitNo;

    if (HI_FALSE == DRV_GPIO_Convert(u32GpioNo, &u32GroupNo, &u32BitNo))
    {
        return HI_ERR_GPIO_INVALID_PARA;
    }

    if (!pu32DirBit)
    {
        HI_ERR_GPIO("para pu32DirBit is null.\n"); //188
        return HI_ERR_GPIO_NULL_PTR;
    }

    GpioUsrAddr = g_GpioUsrAddr[u32GroupNo];

    GpioDirtValue = REG_USR_ADDR(GpioUsrAddr + HI_GPIO_DIR_REG);

    GpioDirtValue &= (1 << u32BitNo);

    if (GpioDirtValue == 0)
    {
        *pu32DirBit = HI_TRUE;
    }
    else
    {
        *pu32DirBit = HI_FALSE;
    }

    return HI_SUCCESS;
}

HI_S32 HI_DRV_GPIO_WriteBit(HI_U32 u32GpioNo, HI_U32 u32BitValue)
{
    HI_U32 GpioDirtValue;
    HI_U32 GpioUsrAddr;
    HI_U32 u32GroupNo;
    HI_U32 u32BitNo;

    if (HI_FALSE == DRV_GPIO_Convert(u32GpioNo, &u32GroupNo, &u32BitNo))
    {
        return HI_ERR_GPIO_INVALID_PARA;
    }

    if ((u32BitValue != HI_TRUE)
        && (u32BitValue != HI_FALSE)
    )
    {
        HI_ERR_GPIO("para u32BitValue is invalid.\n");
        return HI_ERR_GPIO_INVALID_PARA;
    }

    GpioUsrAddr = g_GpioUsrAddr[u32GroupNo];

    GpioDirtValue = REG_USR_ADDR(GpioUsrAddr + HI_GPIO_DIR_REG);

    GpioDirtValue &= (1 << u32BitNo);

    GpioDirtValue = GpioDirtValue >> u32BitNo;

    if (GpioDirtValue != HI_GPIO_OUTPUT)
    {
        HI_ERR_GPIO("GPIO Direction is input, write can not operate.\n");
        return HI_ERR_GPIO_INVALID_OPT;
    }

    if (u32BitValue)
    {
        REG_USR_ADDR(GpioUsrAddr + (4 << u32BitNo)) = 1 << u32BitNo;
    }
    else
    {
        REG_USR_ADDR(GpioUsrAddr + (4 << u32BitNo)) = 0;
    }

    return HI_SUCCESS;
}

HI_S32 HI_DRV_GPIO_ReadBit(HI_U32 u32GpioNo, HI_U32* pu32BitValue)
{
    HI_U32 GpioDirtValue;
    HI_U32 GpioUsrAddr;
    HI_U32 u32GroupNo;
    HI_U32 u32BitNo;

    if (HI_FALSE == DRV_GPIO_Convert(u32GpioNo, &u32GroupNo, &u32BitNo))
    {
        return HI_ERR_GPIO_INVALID_PARA;
    }

    if (!pu32BitValue)
    {
        HI_ERR_GPIO("para pu32BitValue is null.\n");
        return HI_ERR_GPIO_NULL_PTR;
    }

    GpioUsrAddr = g_GpioUsrAddr[u32GroupNo];

    GpioDirtValue = REG_USR_ADDR(GpioUsrAddr + HI_GPIO_DIR_REG);

    GpioDirtValue &= (1 << u32BitNo);

    if (GpioDirtValue != HI_GPIO_INPUT)
    {
        HI_ERR_GPIO("GPIO Direction is output,read can not operate\n");
        return HI_ERR_GPIO_INVALID_OPT;
    }

    *pu32BitValue = REG_USR_ADDR(GpioUsrAddr + (4 << u32BitNo)) >> u32BitNo;

    return HI_SUCCESS;
}

/*
u32GpioGroup: gpio group number
u32BitX:      gpio bit offset
enIntTriMode: interruput type ,please see HI_UNF_GPIO_INTTYPE_E
 */
HI_S32 HI_DRV_GPIO_SetIntType(HI_U32 u32GpioNo, HI_UNF_GPIO_INTTYPE_E enIntTriMode)
{
    HI_S32 GpioValue;
    HI_U32 GpioUsrAddr;
    HI_U32 u32GroupNo;
    HI_U32 u32BitNo;

    if (HI_FALSE == DRV_GPIO_Convert(u32GpioNo, &u32GroupNo, &u32BitNo))
    {
        return HI_ERR_GPIO_INVALID_PARA;
    }

    if (enIntTriMode >= HI_UNF_GPIO_INTTYPE_BUTT)
    {
        return HI_ERR_GPIO_INVALID_PARA;
    }

    if (enIntTriMode <= HI_UNF_GPIO_INTTYPE_UPDOWN)     /*edge trigger*/
    {
        GpioUsrAddr = g_GpioUsrAddr[u32GroupNo];
        GpioValue  = REG_USR_ADDR(GpioUsrAddr + HI_GPIO_IS_REG);
        GpioValue &= (~(1 << u32BitNo));
        REG_USR_ADDR(GpioUsrAddr + HI_GPIO_IS_REG) = GpioValue;

        GpioValue = REG_USR_ADDR(GpioUsrAddr + HI_GPIO_IBE_REG);

        if (enIntTriMode == HI_UNF_GPIO_INTTYPE_UP)
        {
            GpioValue &= (~(1 << u32BitNo));     /* single edge trigger */
            REG_USR_ADDR(GpioUsrAddr + HI_GPIO_IBE_REG) = GpioValue;

            GpioValue  = REG_USR_ADDR(GpioUsrAddr + HI_GPIO_IEV_REG);
            GpioValue |= (1 << u32BitNo);        /*  up edge*/
            REG_USR_ADDR(GpioUsrAddr + HI_GPIO_IEV_REG) = GpioValue;
        }
        else if (enIntTriMode == HI_UNF_GPIO_INTTYPE_DOWN)
        {
            GpioValue &= (~(1 << u32BitNo));     /*  single edge trigger */
            REG_USR_ADDR(GpioUsrAddr + HI_GPIO_IBE_REG) = GpioValue;

            GpioValue  = REG_USR_ADDR(GpioUsrAddr + HI_GPIO_IEV_REG);
            GpioValue &= (~(1 << u32BitNo));     /* down edge */
            REG_USR_ADDR(GpioUsrAddr + HI_GPIO_IEV_REG) = GpioValue;
        }
        else
        {
            GpioValue |= (1 << u32BitNo);        /* double edge */
            REG_USR_ADDR(GpioUsrAddr + HI_GPIO_IBE_REG) = GpioValue;
        }
    }
    else if (enIntTriMode > HI_UNF_GPIO_INTTYPE_UPDOWN) /* level trigger*/
    {
        GpioUsrAddr = g_GpioUsrAddr[u32GroupNo];
        GpioValue  = REG_USR_ADDR(GpioUsrAddr + HI_GPIO_IS_REG);
        GpioValue |= (1 << u32BitNo);
        REG_USR_ADDR(GpioUsrAddr + HI_GPIO_IS_REG) = GpioValue;

        if (enIntTriMode == HI_UNF_GPIO_INTTYPE_HIGH)
        {
            GpioValue  = REG_USR_ADDR(GpioUsrAddr + HI_GPIO_IEV_REG);
            GpioValue |= (1 << u32BitNo);    /*  high level trigger*/
            REG_USR_ADDR(GpioUsrAddr + HI_GPIO_IEV_REG) = GpioValue;
        }
        else
        {
            GpioValue  = REG_USR_ADDR(GpioUsrAddr + HI_GPIO_IEV_REG);
            GpioValue &= (~(1 << u32BitNo));   /* low level trigger */
            REG_USR_ADDR(GpioUsrAddr + HI_GPIO_IEV_REG) = GpioValue;
        }
    }

    HI_INFO_GPIO("u32GpioGroup = %d u32BitX = %d  enIntTriMode = %d \n", u32GroupNo, u32BitNo, enIntTriMode); //367

    return HI_SUCCESS;
}

HI_S32 HI_DRV_GPIO_SetBitIntEnable(HI_U32 u32GpioNo, HI_BOOL bEnable)
{
    HI_S32 GpioValue;
    HI_U32 GpioUsrAddr;
    HI_U32 u32GroupNo;
    HI_U32 u32BitNo;

    if (HI_FALSE == DRV_GPIO_Convert(u32GpioNo, &u32GroupNo, &u32BitNo))
    {
        return HI_ERR_GPIO_INVALID_PARA;
    }

    GpioUsrAddr = g_GpioUsrAddr[u32GroupNo];

    GpioValue = REG_USR_ADDR(GpioUsrAddr + HI_GPIO_IE_REG);

    if (bEnable)   /* not marsk  */
    {
        GpioValue |= (1 << u32BitNo);
        g_stGpioAttr.GpioIntAttr[u32GpioNo].GpioIntEnable = HI_TRUE;
    }
    else           /*  marsk  */
    {
        GpioValue &= (~(1 << u32BitNo));
        g_stGpioAttr.GpioIntAttr[u32GpioNo].GpioIntEnable = HI_FALSE;
    }

    REG_USR_ADDR(GpioUsrAddr + HI_GPIO_IE_REG) = GpioValue;

    HI_INFO_GPIO("u32GpioGroup = %d u32BitX = %d  bEnable = %d \n", u32GroupNo, u32BitNo, bEnable); //401

    return HI_SUCCESS;
}

HI_S32 HI_DRV_GPIO_SetGroupIntEnable(HI_U32 u32GroupNo, HI_BOOL bEnable)
{
    HI_S32 GpioValue;
    HI_U32 GpioUsrAddr;

    if (u32GroupNo >= g_GpioNum.u8GpioGrpNum)
    {
        HI_ERR_GPIO("invalid parameter(%u).\n", u32GroupNo);
        return HI_ERR_GPIO_INVALID_PARA;
    }

    GpioUsrAddr = g_GpioUsrAddr[u32GroupNo];

    GpioValue = REG_USR_ADDR(GpioUsrAddr + HI_GPIO_IE_REG);

    if (bEnable)   /* enable one group*/
    {
        GpioValue |= 0xff;
    }
    else           /* disable one group */
    {
        GpioValue = 0;
    }

    REG_USR_ADDR(GpioUsrAddr + HI_GPIO_IE_REG) = GpioValue;

    HI_INFO_GPIO("u32GpioGroup = %d u32BitX = %d  bEnable = %d \n", u32GroupNo, bEnable);

    return HI_SUCCESS;
}

HI_S32 HI_DRV_GPIO_SetAllIntEnable(HI_BOOL bEnable)
{
    HI_S32 Ret;
    HI_U32 i = 0;

    for (i = 0; i < g_GpioNum.u8GpioGrpNum; i++)
    {
        Ret = HI_DRV_GPIO_SetGroupIntEnable(i, bEnable);
        if (HI_SUCCESS != Ret)
        {            
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

HI_S32 DRV_GPIO_QueryInt (GPIO_INT_S * pGpioIntValue)
{
    HI_S32 Ret = 0;
    HI_U32 GpioIntNum;

    g_stGpioAttr.GpioBlockTime = pGpioIntValue->u32TimeoutMs;

    while ((g_stGpioAttr.Head) == (g_stGpioAttr.Tail))
    {
        if (0xffffffff == g_stGpioAttr.GpioBlockTime)
        {
            Ret = wait_event_interruptible(g_stGpioAttr.GpioIntWaitQueue, (g_stGpioAttr.Head != g_stGpioAttr.Tail));
            if (Ret < 0)
            {
                HI_INFO_GPIO("wait data err.\n");
                return -ERESTARTSYS;
            }
        }
        else
        {
            Ret = wait_event_interruptible_timeout(g_stGpioAttr.GpioIntWaitQueue, (g_stGpioAttr.Head
                                                                                   != g_stGpioAttr.Tail),
                                                   (long)(g_stGpioAttr.GpioBlockTime * HZ / 1000));
            if (Ret < 0)
            {
                HI_INFO_GPIO("wait data err.\n");
                return -ERESTARTSYS;
            }
            else if (0 == Ret)
            {
                HI_INFO_GPIO("wait data timeout.\n"); //484
                return HI_ERR_GPIO_GETINT_TIMEOUT;
            }
        }
    }

    if ((g_stGpioAttr.Head) != (g_stGpioAttr.Tail))
    {
        GpioIntNum = GPIO_BUF_TAIL;
        g_stGpioAttr.Tail = INC_BUF(g_stGpioAttr.Tail, g_stGpioAttr.GpioBufLen);

        pGpioIntValue->u32GpioNo = GpioIntNum;
    }

    return HI_SUCCESS;
}

HI_S32 DRV_GPIO_Open(struct inode *inode, struct file *filp)
{
	g_stGpioAttr.Head = 0;
	g_stGpioAttr.Tail = 0;
	return HI_SUCCESS;
}

HI_S32 DRV_GPIO_Close(struct inode *inode, struct file *filp)
{
	g_stGpioAttr.Head = 0;
	g_stGpioAttr.Tail = 0;
	return HI_SUCCESS;
}

HI_S32 DRV_GPIO_SetOutputType(HI_U32 u32GpioNo, HI_UNF_GPIO_OUTPUTTYPE_E  enOutputType)
{
#if defined (CHIP_TYPE_hi3716cv200) || defined (CHIP_TYPE_hi3719cv100) \
	|| defined (CHIP_TYPE_hi3718cv100) || defined (CHIP_TYPE_hi3719mv100) \
	|| defined (CHIP_TYPE_hi3719mv100_a) || defined (CHIP_TYPE_hi3718mv100)

    HI_U32 u32Value = 0;
	HI_U32 u32Bit = 0;

	if ((u32GpioNo < 5 * 8 + 0)
		|| (u32GpioNo > 5 * 8 + 7)
		|| (u32GpioNo == 5 * 8 + 5)
		|| (u32GpioNo == 5 * 8 + 6))
	{
		return HI_ERR_GPIO_NOT_SUPPORT;
	}

	u32Value = g_pstRegSysCtrl->SC_GPIO_OD_CTRL.u32;

	u32Bit = u32GpioNo % 8;
	u32Value &= ~(1 << u32Bit);
	u32Value |= enOutputType << u32Bit;

	g_pstRegSysCtrl->SC_GPIO_OD_CTRL.u32 = u32Value;
	
	return HI_SUCCESS;
	
#elif defined (CHIP_TYPE_hi3798cv100)

	return HI_SUCCESS;

#else
	return HI_ERR_GPIO_NOT_SUPPORT;
#endif
}

HI_S32 DRV_GPIO_GetOutputType(HI_U32 u32GpioNo, HI_UNF_GPIO_OUTPUTTYPE_E  *penOutputType)
{
#if defined (CHIP_TYPE_hi3716cv200) || defined (CHIP_TYPE_hi3719cv100) \
	|| defined (CHIP_TYPE_hi3718cv100) || defined (CHIP_TYPE_hi3719mv100) \
	|| defined (CHIP_TYPE_hi3719mv100_a) || defined (CHIP_TYPE_hi3718mv100)

	HI_U32 u32Value = 0;
	HI_U32 u32Bit = 0;

	if ((u32GpioNo < 5 * 8 + 0)
		|| (u32GpioNo > 5 * 8 + 7)
		|| (u32GpioNo == 5 * 8 + 5)
		|| (u32GpioNo == 5 * 8 + 6))
	{
		return HI_ERR_GPIO_NOT_SUPPORT;
	}	

	u32Value = g_pstRegSysCtrl->SC_GPIO_OD_CTRL.u32;

	u32Bit = u32GpioNo % 8;
	
	u32Value = (u32Value >> u32Bit) & 0x1;
	*penOutputType = (HI_UNF_GPIO_OUTPUTTYPE_E)u32Value;

	
	return HI_SUCCESS;
	
#elif defined (CHIP_TYPE_hi3798cv100)

	return HI_SUCCESS;

#else
	return HI_ERR_GPIO_NOT_SUPPORT;
#endif
}

HI_S32 HI_DRV_GPIO_RegisterServerFunc(HI_U32 u32GpioNo, HI_VOID (*func)(HI_U32))
{
    if (u32GpioNo >= g_GpioNum.u8GpioMaxNum)
    {
        HI_INFO_GPIO(" para gpio number out of range , u32GpioNum = %d\n", u32GpioNo); //598
        return HI_FAILURE;
    }

    if (HI_NULL == func)
    {
        HI_INFO_GPIO(" register func para is null, u32GpioNum%d_%d \n", u32GpioNo);
        return HI_FAILURE;
    }

    if (HI_NULL != g_stGpioAttr.GpioIntAttr[u32GpioNo].GpioServer)
    {
        HI_INFO_GPIO(" GPIO %d had registered gpio server pragram \n", u32GpioNo);
        return HI_FAILURE;
    }

    g_stGpioAttr.GpioIntAttr[u32GpioNo].GpioServer = func;
    HI_INFO_GPIO("gpio %d finished register gpio server function \n", u32GpioNo); //615

    return HI_SUCCESS;
}

HI_S32 HI_DRV_GPIO_UnregisterServerFunc(HI_U32 u32GpioNo)
{
    if (u32GpioNo >= g_GpioNum.u8GpioMaxNum)
    {
        HI_INFO_GPIO(" para gpio number out of range , u32GpioNum = %d\n", u32GpioNo); //624
        return HI_FAILURE;
    }

    g_stGpioAttr.GpioIntAttr[u32GpioNo].GpioServer = HI_NULL;

    return HI_SUCCESS;
}

HI_S32 HI_DRV_GPIO_ClearBitInt(HI_U32 u32GpioNo)
{
    HI_U32 GpioUsrAddr;
    HI_U32 RegVal;
    HI_U32 u32GroupNo;
    HI_U32 u32BitNo;

    if (HI_FALSE == DRV_GPIO_Convert(u32GpioNo, &u32GroupNo, &u32BitNo))
    {
        return HI_ERR_GPIO_INVALID_PARA;
    }

    GpioUsrAddr = g_GpioUsrAddr[u32GroupNo];
    RegVal  = REG_USR_ADDR((GpioUsrAddr + HI_GPIO_IC_REG));
    RegVal |= (0x1 << u32BitNo);

    REG_USR_ADDR((GpioUsrAddr + HI_GPIO_IC_REG)) = RegVal;
    return HI_SUCCESS;
}

HI_S32 HI_DRV_GPIO_CearGroupInt(HI_U32 u32GpioGroup)
{
    HI_U32 GpioUsrAddr;

    if (u32GpioGroup >= g_GpioNum.u8GpioGrpNum)
    {
        HI_ERR_GPIO("invalid parameter(%u).\n", u32GpioGroup); //659
        return HI_ERR_GPIO_INVALID_PARA;
    }

    GpioUsrAddr = g_GpioUsrAddr[u32GpioGroup];
    REG_USR_ADDR((GpioUsrAddr + HI_GPIO_IC_REG)) = 0xff;

    return HI_SUCCESS;
}

HI_S32 GPIO_ClearAllInt(HI_VOID)
{
    HI_S32 Ret;
    HI_U32 i = 0;

    for (i = 0; i < g_GpioNum.u8GpioGrpNum; i++)
    {
        Ret = HI_DRV_GPIO_CearGroupInt(i);
        if (HI_SUCCESS != Ret)
        {
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

irqreturn_t GPIO_Isr(HI_S32 irq, HI_VOID * dev_id)
{
    HI_S32 IntValue;
    HI_U32 GpioUsrAddr;
    HI_U32 u32GpioGroup;
    HI_U32 i;
    HI_U32 u32GpioNum;

    u32GpioGroup = irq - HI_GPIO_IRQ_NO;
    if (u32GpioGroup >= g_GpioNum.u8GpioGrpNum)
    {
        HI_ERR_GPIO("Error irq(%u).\n", u32GpioGroup); //697
        return IRQ_HANDLED;
    }

    GpioUsrAddr = g_GpioUsrAddr[u32GpioGroup];
    IntValue = REG_USR_ADDR(GpioUsrAddr + HI_GPIO_MIS_REG);

    for (i = 0; i < 8; i++)
    {
        if (IntValue & (1 << i)) /* check each bit */
        {
            u32GpioNum = u32GpioGroup * 8 + i;

            if (HI_TRUE == g_stGpioAttr.GpioIntAttr[u32GpioNum].GpioIntEnable)
            {
                if (HI_NULL != g_stGpioAttr.GpioIntAttr[u32GpioNum].GpioServer)
                {
                    g_stGpioAttr.GpioIntAttr[u32GpioNum].GpioServer(u32GpioNum);
                    HI_INFO_GPIO(" ----- execute gpio[%d].GpioServer \n", u32GpioNum);
                }

                HI_INFO_GPIO("############  Get GPIO INT: GPIO%d_%d. \n", u32GpioGroup, i);

                GPIO_BUF_HEAD = u32GpioNum;

                g_stGpioAttr.Head = INC_BUF(g_stGpioAttr.Head, g_stGpioAttr.GpioBufLen);
                wake_up_interruptible(&(g_stGpioAttr.GpioIntWaitQueue));
            }

            REG_USR_ADDR((GpioUsrAddr + HI_GPIO_IC_REG)) = (0X1 << i);    /* clear one bit interruput flag*/
            break;
        }
    }

    return IRQ_HANDLED;
}

HI_S32 HI_DRV_GPIO_GetGpioNum(GPIO_GET_GPIONUM_S* GpioNum)
{
    if (HI_NULL == GpioNum)
    {
        HI_ERR_GPIO("invalid parameter\n"); //738
        return HI_ERR_GPIO_NULL_PTR;
    }

    GpioNum->u8GpioGrpNum = HI_GPIO_GROUP_NUM; //g_u8GpioGrpNum;
    GpioNum->u8GpioMaxNum = HI_GPIO_GROUP_NUM * HI_GPIO_BIT_NUM; //g_u8GpioMaxNum;

    return HI_SUCCESS;
}

HI_S32 HI_DRV_GPIO_Init(void)
{
    HI_S32 ret;
    HI_U32 i;

    ret = HI_DRV_MODULE_Register(HI_ID_GPIO, "HI_GPIO", (HI_VOID *)&g_stGpioExportFuncs);
    if (HI_SUCCESS != ret)
    {
        HI_INFO_GPIO(" GPIO Module register failed 0x%x.\n", ret); //756
        return HI_FAILURE;
    }

#if 0
    HI_DRV_SYS_GetChipVersion( &enChipType, &enChipVersion );
    if ((HI_CHIP_TYPE_HI3716M == enChipType) && (HI_CHIP_VERSION_V300 == enChipVersion))
    {
        g_u8GpioGrpNum = HI_MV300_GPIO_GROUP_NUM;
    }
    else
    {
        g_u8GpioGrpNum = HI_GPIO_GROUP_NUM;
    }

    g_u8GpioMaxNum = g_u8GpioGrpNum * HI_GPIO_BIT_NUM;
#endif

    g_GpioUsrAddr[0] = IO_ADDRESS(HI_GPIO_0_ADDR);
    g_GpioUsrAddr[1] = IO_ADDRESS(HI_GPIO_1_ADDR);
    g_GpioUsrAddr[2] = IO_ADDRESS(HI_GPIO_2_ADDR);
    g_GpioUsrAddr[3] = IO_ADDRESS(HI_GPIO_3_ADDR);
    g_GpioUsrAddr[4] = IO_ADDRESS(HI_GPIO_4_ADDR);
    g_GpioUsrAddr[5] = IO_ADDRESS(HI_GPIO_5_ADDR);
    g_GpioUsrAddr[6] = IO_ADDRESS(HI_GPIO_6_ADDR);
    g_GpioUsrAddr[7] = IO_ADDRESS(HI_GPIO_7_ADDR);
    g_GpioUsrAddr[8] = IO_ADDRESS(HI_GPIO_8_ADDR);
    g_GpioUsrAddr[9] = IO_ADDRESS(HI_GPIO_9_ADDR);

    g_GpioUsrAddr[10] = IO_ADDRESS(HI_GPIO_10_ADDR);
    g_GpioUsrAddr[11] = IO_ADDRESS(HI_GPIO_11_ADDR);
    g_GpioUsrAddr[12] = IO_ADDRESS(HI_GPIO_12_ADDR);

#if defined (CHIP_TYPE_hi3716cv200es) || defined (CHIP_TYPE_hi3716cv200) \
	|| defined (CHIP_TYPE_hi3719cv100) || defined (CHIP_TYPE_hi3718cv100)  \
	|| defined (CHIP_TYPE_hi3719mv100) || defined (CHIP_TYPE_hi3719mv100_a)\
	|| defined (CHIP_TYPE_hi3718mv100) || defined (CHIP_TYPE_hi3798cv100)
    g_GpioUsrAddr[13] = IO_ADDRESS(HI_GPIO_13_ADDR);
    g_GpioUsrAddr[14] = IO_ADDRESS(HI_GPIO_14_ADDR);
    g_GpioUsrAddr[15] = IO_ADDRESS(HI_GPIO_15_ADDR);
    g_GpioUsrAddr[16] = IO_ADDRESS(HI_GPIO_16_ADDR);
    g_GpioUsrAddr[17] = IO_ADDRESS(HI_GPIO_17_ADDR);
#endif
    
#if defined (CHIP_TYPE_hi3798cv100)
    g_GpioUsrAddr[18] = IO_ADDRESS(HI_GPIO_18_ADDR);
    g_GpioUsrAddr[19] = IO_ADDRESS(HI_GPIO_19_ADDR);
#endif

    g_stGpioAttr.Head = 0;
    g_stGpioAttr.Tail = 0;
    g_stGpioAttr.GpioBufLen = GPIO_MAX_BUF;
    g_stGpioAttr.GpioBlockTime = 0xffffffff;

    ret = HI_DRV_GPIO_GetGpioNum(&g_GpioNum);

    for (i = 1; i < g_GpioNum.u8GpioMaxNum; i++)
    {
        g_stGpioAttr.GpioIntAttr[i].GpioIntEnable = HI_FALSE;
        g_stGpioAttr.GpioIntAttr[i].GpioIntType = HI_UNF_GPIO_INTTYPE_DOWN;
        g_stGpioAttr.GpioIntAttr[i].GpioServer = HI_NULL;
    }

    init_waitqueue_head(&g_stGpioAttr.GpioIntWaitQueue);

    ret = HI_DRV_GPIO_SetAllIntEnable(HI_FALSE);
    if (HI_SUCCESS != ret)
    {
        HI_INFO_GPIO(" GPIO disable INT failed 0x%x.\n", ret); //833
        return HI_FAILURE;
    }

    ret = GPIO_ClearAllInt();
    if (HI_SUCCESS != ret)
    {
        HI_INFO_GPIO(" GPIO clear all INT failed 0x%x.\n", ret);
        return HI_FAILURE;
    }

    for (i = 0; i < g_GpioNum.u8GpioGrpNum; i++)
    {
    	if ((i == 0) || (i == 15) || (i == 16) || (i == 17))
    	{
    		continue;
    	}

        ret = request_irq(HI_GPIO_IRQ_NO + i, GPIO_Isr, IRQF_DISABLED, g_GpioIrqName[i], NULL);
        if (HI_SUCCESS != ret)
        {
            HI_INFO_GPIO("register GPIO INT failed 0x%x, irq:%d.\n", ret, HI_GPIO_IRQ_NO + i); //856
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

void  HI_DRV_GPIO_DeInit(void)
{
    HI_U32 i;
    HI_S32 Ret;

    Ret = HI_DRV_GPIO_SetAllIntEnable(HI_FALSE);  //disable all group interruput
    if (HI_SUCCESS != Ret)
    {
        HI_INFO_GPIO(" GPIO disable INT failed 0x%x.\n", Ret); //882
    }

    for (i = 1; i < g_GpioNum.u8GpioMaxNum; i++)
    {
        g_stGpioAttr.GpioIntAttr[i].GpioIntEnable = HI_FALSE;
        g_stGpioAttr.GpioIntAttr[i].GpioIntType = HI_UNF_GPIO_INTTYPE_DOWN;
        g_stGpioAttr.GpioIntAttr[i].GpioServer = HI_NULL;
    }

    for (i = 0; i < g_GpioNum.u8GpioGrpNum; i++)
    {
    	if ((i == 0) || (i == 15) || (i == 16) || (i == 17))
    	{
    		continue;
    	}

        free_irq(HI_GPIO_IRQ_NO + i, NULL);
        HI_INFO_GPIO("free_irq(%d)\n", HI_GPIO_IRQ_NO + i);
    }

    Ret = HI_DRV_MODULE_UnRegister(HI_ID_GPIO);
    if (HI_SUCCESS != Ret)
    {
        HI_INFO_GPIO(" GPIO Module unregister failed 0x%x.\n", Ret);
    }

    return;
}

#ifdef MODULE
EXPORT_SYMBOL(HI_DRV_GPIO_Init);
EXPORT_SYMBOL(HI_DRV_GPIO_DeInit);
#endif

EXPORT_SYMBOL(HI_DRV_GPIO_GetDirBit);
EXPORT_SYMBOL(HI_DRV_GPIO_SetDirBit);
EXPORT_SYMBOL(HI_DRV_GPIO_WriteBit);
EXPORT_SYMBOL(HI_DRV_GPIO_ReadBit);
EXPORT_SYMBOL(HI_DRV_GPIO_GetGpioNum);
EXPORT_SYMBOL(HI_DRV_GPIO_RegisterServerFunc);
EXPORT_SYMBOL(HI_DRV_GPIO_UnregisterServerFunc);
EXPORT_SYMBOL(HI_DRV_GPIO_SetIntType);
EXPORT_SYMBOL(HI_DRV_GPIO_SetBitIntEnable);
EXPORT_SYMBOL(HI_DRV_GPIO_CearGroupInt);
EXPORT_SYMBOL(HI_DRV_GPIO_ClearBitInt);
