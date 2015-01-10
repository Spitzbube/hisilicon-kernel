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
#include <linux/sched.h>

#include "hi_kernel_adapt.h"
#include "hi_drv_reg.h"
#include "hi_drv_dev.h"
#include "hi_drv_proc.h"
#include "hi_drv_sys.h"
#include "drv_i2c.h"
#include "drv_i2c_ioctl.h"
#include "drv_i2c_ext.h"
#include "hi_common.h"
#include "hi_reg_common.h"
#include "hi_drv_i2c.h"
#include "hi_module.h"
#include "hi_drv_module.h"
#include "hi_drv_mem.h"

#define I2C_WAIT_TIME_OUT 0x1000

#define I2C_WRITE_REG(Addr, Value) ((*(volatile HI_U32 *)(Addr)) = (Value))
#define I2C_READ_REG(Addr) (*(volatile HI_U32 *)(Addr))

struct bla
{
#if 0
	int Data_224; //224 = 0
	int Data_228; //228 = 4
	void* Data_232; //232 = 8
	void* Data_236; //236 = 12
#else
	struct semaphore Data_224;
#endif
	wait_queue_head_t I2cWaitQueue; //240
	//28?
};

//static UMAP_DEVICE_S   g_I2cRegisterData;
//static atomic_t g_I2cCount = ATOMIC_INIT(0);
HI_DECLARE_MUTEX(g_I2cMutex);

//static HI_U32 regI2CStore[HI_STD_I2C_NUM] = {0};
static HI_U32 g_I2cKernelAddr[HI_STD_I2C_NUM]; //811440c0
static struct Struct_811440e0
{
	struct
	{
		unsigned int Data_0; //0
		char fill_4; //4
		unsigned int Data_8; //8
		//12
	} Data_0[4*256]; //0
	unsigned int Data_12288; //12288
	unsigned int Data_12292; //12292
	//12296
} Data_811440e0[HI_STD_I2C_NUM]; //811440e0 -> 8115C120
static HI_U32 g_aI2cRate[HI_STD_I2C_NUM] = {0}; //8115c120
static struct bla g_astI2cShare[HI_STD_I2C_NUM]; //8115c140 -> 8115C220
static int i2cState = 0; //8115c220
static char i2cIrqStrings[HI_STD_I2C_NUM][8]; //8115c224

static HI_CHIP_TYPE_E g_enChipType;

static I2C_EXT_FUNC_S g_stI2cExtFuncs =
{
    .pfnI2cWriteConfig	= HI_DRV_I2C_WriteConfig,
    .pfnI2cWrite		= HI_DRV_I2C_Write,
    .pfnI2cRead			= HI_DRV_I2C_Read,
    .pfnI2cReadDirectly = HI_DRV_I2C_ReadDirectly,
};

#if 0
void I2C_DRV_ExchangeByte(unsigned int I2cNum, unsigned int r1)
{
	unsigned int r2 = Data_811440e0[I2cNum].Data_0[r1].Data_8;

	if (Data_811440e0[I2cNum].Data_0[r1].Data_0 == 0)
	{
		I2C_WRITE_REG((g_I2cKernelAddr[I2cNum] + I2C_COM_REB), r2);
	}
	else if (Data_811440e0[I2cNum].Data_0[r1].Data_0 == 1)
	{
		I2C_WRITE_REG((g_I2cKernelAddr[I2cNum] + I2C_TXR_REG), Data_811440e0[I2cNum].Data_0[r1].fill_4);
		I2C_WRITE_REG((g_I2cKernelAddr[I2cNum] + I2C_COM_REB), r2);
	}
	else if (Data_811440e0[I2cNum].Data_0[r1].Data_0 == 2)
	{
		I2C_WRITE_REG((g_I2cKernelAddr[I2cNum] + I2C_COM_REB), r2);
	}
}
#endif

void I2C_DRV_ReadRXR(HI_U32 I2cNum, unsigned int index)
{
	HI_INFO_I2C("Num:%d, index:%d\n", I2cNum, index); //167

	if (index > 0)
	{
		index--;
		if (Data_811440e0[I2cNum].Data_0[index].Data_0 == 2)
		{
			Data_811440e0[I2cNum].Data_0[index].fill_4 =
					I2C_READ_REG(g_I2cKernelAddr[I2cNum] + I2C_RXR_REG);
		}
	}
}

HI_S32 I2C_DRV_SetRate(HI_U32 I2cNum, HI_U32 I2cRate)
{
    HI_U32 Value = 0;
    HI_U32 SclH = 0;
    HI_U32 SclL = 0;

    //HI_CHIP_TYPE_E enChipType;
    //HI_U32 u32ChipVersion;
    HI_U32 SysClock = I2C_DFT_SYSCLK;

    if (I2cNum >= HI_STD_I2C_NUM)
    {
        HI_ERR_I2C("I2cNum(%d) is wrong, STD_I2C_NUM is %d\n", I2cNum, HI_STD_I2C_NUM); //211
        return HI_FAILURE;
    }

    g_aI2cRate[I2cNum] = I2cRate;

    /* read i2c I2C_CTRL register*/
    Value = I2C_READ_REG((g_I2cKernelAddr[I2cNum] + I2C_CTRL_REG));

    /* close all i2c  interrupt */
    I2C_WRITE_REG((g_I2cKernelAddr[I2cNum] + I2C_CTRL_REG), (Value & (~I2C_UNMASK_TOTAL)));

    //HI_DRV_SYS_GetChipVersion( &enChipType, &u32ChipVersion );
    if (HI_CHIP_TYPE_HI3716C == g_enChipType)
    {
        SysClock = I2C_3716C_SYSCLK;
    }

    SclH = (SysClock / (I2cRate * 2)) / 2 - 1;
    I2C_WRITE_REG((g_I2cKernelAddr[I2cNum] + I2C_SCL_H_REG), SclH);

    SclL = (SysClock / (I2cRate * 2)) / 2 - 1;
    I2C_WRITE_REG((g_I2cKernelAddr[I2cNum] + I2C_SCL_L_REG), SclL);

    /*enable i2c interrupt, resume original  interrupt*/
    I2C_WRITE_REG((g_I2cKernelAddr[I2cNum] + I2C_CTRL_REG), Value);

    return HI_SUCCESS;
}

HI_S32 I2C_DRV_WaitWriteEnd(HI_U32 I2cNum)
{
    HI_U32 I2cSrReg;
    HI_U32 i = 0;

    do
    {
        I2cSrReg = I2C_READ_REG((g_I2cKernelAddr[I2cNum] + I2C_SR_REG));

        if (i > I2C_WAIT_TIME_OUT)
        {
            HI_ERR_I2C("wait write data timeout!\n"); //246
            return HI_FAILURE;
        }

        i++;
    } while ((I2cSrReg & I2C_OVER_INTR) != I2C_OVER_INTR);

    if (I2cSrReg & I2C_ACK_INTR)
    {
        HI_ERR_I2C("wait write data timeout!\n"); //255
        return HI_FAILURE;
    }

    I2C_WRITE_REG((g_I2cKernelAddr[I2cNum] + I2C_ICR_REG), I2C_CLEAR_ALL);

    return HI_SUCCESS;
}

HI_S32 I2C_DRV_WaitRead(HI_U32 I2cNum)
{
    HI_U32 I2cSrReg;
    HI_U32 i = 0;

    do
    {
        I2cSrReg = I2C_READ_REG((g_I2cKernelAddr[I2cNum] + I2C_SR_REG));

        if (i > I2C_WAIT_TIME_OUT)
        {
            HI_ERR_I2C("wait Read data timeout!\n");
            return HI_FAILURE;
        }

        i++;
    } while ((I2cSrReg & I2C_RECEIVE_INTR) != I2C_RECEIVE_INTR);

    return HI_SUCCESS;
}

/*
add by Jiang Lei 2010-08-24
I2C write finished acknowledgement function
it use to e2prom device ,make sure it finished write operation.
i2c master start next write operation must waiting when it acknowledge e2prom write cycle finished.
 */
HI_S32 I2C_DRV_WriteConfig(HI_U32 I2cNum, HI_U8 I2cDevAddr)
{
    HI_U32 i = 0;
    HI_U32 j = 0;
    HI_U32 I2cSrReg;

#warning I2C_DRV_WriteConfig: TODO

    do
    {
        I2C_WRITE_REG((g_I2cKernelAddr[I2cNum] + I2C_TXR_REG), (I2cDevAddr & WRITE_OPERATION));
        I2C_WRITE_REG((g_I2cKernelAddr[I2cNum] + I2C_COM_REB), (I2C_WRITE | I2C_START));

        j = 0;
        do
        {
            I2cSrReg = I2C_READ_REG((g_I2cKernelAddr[I2cNum] + I2C_SR_REG));

            if (j > I2C_WAIT_TIME_OUT)
            {
                HI_ERR_I2C("wait write data timeout!\n"); //309
                return HI_FAILURE;
            }

            j++;
        } while ((I2cSrReg & I2C_OVER_INTR) != I2C_OVER_INTR);

        I2cSrReg = I2C_READ_REG((g_I2cKernelAddr[I2cNum] + I2C_SR_REG));
        I2C_WRITE_REG((g_I2cKernelAddr[I2cNum] + I2C_ICR_REG), I2C_CLEAR_ALL);

        i++;

        if (i > 0x200000) //I2C_WAIT_TIME_OUT)
        {
            HI_ERR_I2C("wait write ack ok timeout!\n"); //323
            return HI_FAILURE;
        }
    } while ((I2cSrReg & I2C_ACK_INTR));

    return HI_SUCCESS;
}

static void blaaa(HI_U32 I2cNum, int index, unsigned int a, HI_U8 b, unsigned int c)
{
    Data_811440e0[I2cNum].Data_0[index].Data_0 = a;
    Data_811440e0[I2cNum].Data_0[index].fill_4 = b;
    Data_811440e0[I2cNum].Data_0[index].Data_8 = c;
}

static void blaa1(HI_U32 I2cNum, int index, unsigned int a, unsigned int c)
{
    Data_811440e0[I2cNum].Data_0[index].Data_0 = a;
    Data_811440e0[I2cNum].Data_0[index].Data_8 = c;
}

static void blaa2(unsigned int I2cNum, unsigned int r1)
{
	unsigned int r2 = Data_811440e0[I2cNum].Data_0[r1].Data_8;

	if (/*r3*/Data_811440e0[I2cNum].Data_0[r1].Data_0 == 0)
	{
		//8061d900
		I2C_WRITE_REG((g_I2cKernelAddr[I2cNum] + I2C_COM_REB), r2);
	}
	else if (/*r3*/Data_811440e0[I2cNum].Data_0[r1].Data_0 == 1)
	{
		//8061db00
		I2C_WRITE_REG((g_I2cKernelAddr[I2cNum] + I2C_TXR_REG), Data_811440e0[I2cNum].Data_0[r1].fill_4);
		I2C_WRITE_REG((g_I2cKernelAddr[I2cNum] + I2C_COM_REB), r2);
	}
	else if (/*r3*/Data_811440e0[I2cNum].Data_0[r1].Data_0 == 2)
	{
		//8061dab0
		I2C_WRITE_REG((g_I2cKernelAddr[I2cNum] + I2C_COM_REB), r2);
	}
}

HI_S32 I2C_DRV_Write(HI_U32 I2cNum, HI_U8 I2cDevAddr, HI_U32 I2cRegAddr, HI_U32 I2cRegAddrByteNum, HI_U8 *pData,
                     HI_U32 DataLen, HI_BOOL bWithStop)
{
    HI_U32 i;

    //    unsigned long   IntFlag;
    HI_U32 RegAddr;

    //local_irq_save(IntFlag);

    /*  clear interrupt flag*/
    I2C_WRITE_REG((g_I2cKernelAddr[I2cNum] + I2C_ICR_REG), I2C_CLEAR_ALL);

    /* send devide address */
    I2C_WRITE_REG((g_I2cKernelAddr[I2cNum] + I2C_TXR_REG), (I2cDevAddr & WRITE_OPERATION));
    I2C_WRITE_REG((g_I2cKernelAddr[I2cNum] + I2C_COM_REB), (I2C_WRITE | I2C_START));

    if (I2C_DRV_WaitWriteEnd(I2cNum))
    {
        //local_irq_restore(IntFlag);
        //HI_ERR_I2C("wait write data timeout!%s, %d\n", __func__, __LINE__);
        return HI_ERR_I2C_WRITE_TIMEOUT;
    }

    /* send register address which will need to write */
    for (i = 0; i < I2cRegAddrByteNum; i++)
    {
        RegAddr = I2cRegAddr >> ((I2cRegAddrByteNum - i - 1) * 8);
        I2C_WRITE_REG((g_I2cKernelAddr[I2cNum] + I2C_TXR_REG), RegAddr);

        I2C_WRITE_REG((g_I2cKernelAddr[I2cNum] + I2C_COM_REB), I2C_WRITE);

        if (I2C_DRV_WaitWriteEnd(I2cNum))
        {
            //local_irq_restore(IntFlag);
            //HI_ERR_I2C("wait write data timeout!%s, %d\n", __func__, __LINE__);
            return HI_ERR_I2C_WRITE_TIMEOUT;
        }
    }

    /* send data */
    for (i = 0; i < DataLen; i++)
    {
        I2C_WRITE_REG((g_I2cKernelAddr[I2cNum] + I2C_TXR_REG), (*(pData + i)));
        I2C_WRITE_REG((g_I2cKernelAddr[I2cNum] + I2C_COM_REB), I2C_WRITE);

        if (I2C_DRV_WaitWriteEnd(I2cNum))
        {
            //local_irq_restore(IntFlag);
            //HI_ERR_I2C("wait write data timeout!%s, %d\n", __func__, __LINE__);
            return HI_ERR_I2C_WRITE_TIMEOUT;
        }
    }

    if (bWithStop)
    {
        /*   send stop flag bit*/
        I2C_WRITE_REG((g_I2cKernelAddr[I2cNum] + I2C_COM_REB), I2C_STOP);
        if (I2C_DRV_WaitWriteEnd(I2cNum))
        {
            //local_irq_restore(IntFlag);
            //HI_ERR_I2C("wait write data timeout!%s, %d\n", __func__, __LINE__);
            return HI_ERR_I2C_WRITE_TIMEOUT;
        }
    }

    //local_irq_restore(IntFlag);

    return HI_SUCCESS;
}

HI_S32 I2C_DRV_WriteIsr(HI_U32 I2cNum, HI_U8 I2cDevAddr, HI_U32 I2cRegAddr, HI_U32 I2cRegAddrByteNum, HI_U8 *pData,
                     HI_U32 DataLen, HI_BOOL bWithStop)
{
    HI_U32 i;

    HI_U32 RegAddr;
    int r1;
    int r3;
    int r2;
    unsigned int timeout;
    long s32Ret;

#warning I2C_DRV_WriteIsr: TODO

    /*  clear interrupt flag*/
    I2C_WRITE_REG((g_I2cKernelAddr[I2cNum] + I2C_ICR_REG), I2C_CLEAR_ALL);

    memset(&Data_811440e0[I2cNum], 0, sizeof(struct Struct_811440e0));

    /* send devide address */
    blaaa(I2cNum, Data_811440e0[I2cNum].Data_12288, 1, I2cDevAddr & /*~1*/WRITE_OPERATION, 10);
    Data_811440e0[I2cNum].Data_12288++;

    /* send register address which will need to write */
    for (i = 0; i < I2cRegAddrByteNum; i++)
    {
        RegAddr = I2cRegAddr >> ((I2cRegAddrByteNum - i - 1) * 8);

        blaaa(I2cNum, Data_811440e0[I2cNum].Data_12288, 1, RegAddr, 2);
        Data_811440e0[I2cNum].Data_12288++;
    }

    /* send data */
    for (i = 0; i < DataLen; i++)
    {
        blaaa(I2cNum, Data_811440e0[I2cNum].Data_12288, 1, *(pData + i), 2);
        Data_811440e0[I2cNum].Data_12288++;
    }

    if (bWithStop)
    {
        /*   send stop flag bit*/
    	blaa1(I2cNum, Data_811440e0[I2cNum].Data_12288, 0, 1);
    	Data_811440e0[I2cNum].Data_12288++;
    }

    HI_INFO_I2C("index:%d, len:%d\n",
    		Data_811440e0[I2cNum].Data_12292,
    		Data_811440e0[I2cNum].Data_12288); //881

#if 0
	r1 = Data_811440e0[I2cNum].Data_12288++;
	//r3 = Data_811440e0[I2cNum].Data_0[r1].Data_0;
	r2 = Data_811440e0[I2cNum].Data_0[r1].Data_8;

	if (/*r3*/Data_811440e0[I2cNum].Data_0[r1].Data_0 == 0)
	{
		//8061d900
		I2C_WRITE_REG((g_I2cKernelAddr[I2cNum] + I2C_COM_REB), r2);
	}
	else if (/*r3*/Data_811440e0[I2cNum].Data_0[r1].Data_0 == 1)
	{
		//8061db00
		I2C_WRITE_REG((g_I2cKernelAddr[I2cNum] + I2C_TXR_REG), Data_811440e0[I2cNum].Data_0[r1].fill_4);
		I2C_WRITE_REG((g_I2cKernelAddr[I2cNum] + I2C_COM_REB), r2);
	}
	else if (/*r3*/Data_811440e0[I2cNum].Data_0[r1].Data_0 == 2)
	{
		//8061dab0
		I2C_WRITE_REG((g_I2cKernelAddr[I2cNum] + I2C_COM_REB), r2);
	}
#else
	blaa2(I2cNum, Data_811440e0[I2cNum].Data_12288++);
#endif
	//8061d90c
	timeout = (Data_811440e0[I2cNum].Data_12288 * 800000) / g_aI2cRate[I2cNum];

	if (timeout < 10)
	{
		timeout = 10;
	}

	HI_INFO_I2C("timeout:%d\n", timeout); //891

	//timeout = timeout * 1000 / 1000;

	s32Ret = wait_event_interruptible_timeout(g_astI2cShare[I2cNum].I2cWaitQueue,
			Data_811440e0[I2cNum].Data_12292 == Data_811440e0[I2cNum].Data_12288,
			timeout * 1000 / 1000/*timeout*/);
	//8061da68
	HI_INFO_I2C("s32Ret:%d\n", s32Ret); //897

	if (s32Ret <= 0)
	{
		//8061db44
		HI_ERR_I2C("%s, index:%d, len:%d\n", __FUNCTION__,
				Data_811440e0[I2cNum].Data_12292,
				Data_811440e0[I2cNum].Data_12288); //904
		return 0x80440009;
	}

    return HI_SUCCESS;
}

int I2C_DRV_Read(HI_U32 I2cNum, HI_U8 I2cDevAddr, HI_BOOL bSendSlave, HI_U32 I2cRegAddr, HI_U32 I2cRegAddrByteNum,
                 HI_U8 *pData, HI_U32 DataLen)
{
    HI_U32 dataTmp = 0xff;
    HI_U32 i;

    //    unsigned long   IntFlag;
    HI_U32 RegAddr;

#warning I2C_DRV_Read: TODO
    //local_irq_save(IntFlag);

    if (bSendSlave)
    {
        /* clear interrupt flag*/
        I2C_WRITE_REG((g_I2cKernelAddr[I2cNum] + I2C_ICR_REG), I2C_CLEAR_ALL);

        /* send devide address*/
        I2C_WRITE_REG((g_I2cKernelAddr[I2cNum] + I2C_TXR_REG), (I2cDevAddr & WRITE_OPERATION));
        I2C_WRITE_REG((g_I2cKernelAddr[I2cNum] + I2C_COM_REB), (I2C_WRITE | I2C_START));

        if (I2C_DRV_WaitWriteEnd(I2cNum))
        {
            //local_irq_restore(IntFlag);
            //HI_ERR_I2C("wait write data timeout!\n");
            return HI_ERR_I2C_WRITE_TIMEOUT;
        }
    }

    /* send register address which will need to write*/
    for (i = 0; i < I2cRegAddrByteNum; i++)
    {
        RegAddr = I2cRegAddr >> ((I2cRegAddrByteNum - i - 1) * 8);
        I2C_WRITE_REG((g_I2cKernelAddr[I2cNum] + I2C_TXR_REG), RegAddr);

        I2C_WRITE_REG((g_I2cKernelAddr[I2cNum] + I2C_COM_REB), I2C_WRITE);

        if (I2C_DRV_WaitWriteEnd(I2cNum))
        {
            //local_irq_restore(IntFlag);
            //HI_ERR_I2C("wait write data timeout!\n");
            return HI_ERR_I2C_WRITE_TIMEOUT;
        }
    }

    /* send register address which will need to read */
    I2C_WRITE_REG((g_I2cKernelAddr[I2cNum] + I2C_TXR_REG), (I2cDevAddr | READ_OPERATION));
    I2C_WRITE_REG((g_I2cKernelAddr[I2cNum] + I2C_COM_REB), I2C_WRITE | I2C_START);

    if (I2C_DRV_WaitWriteEnd(I2cNum))
    {
        //local_irq_restore(IntFlag);
        return HI_ERR_I2C_WRITE_TIMEOUT;
    }

    /* repetitivily read data */
    for (i = 0; i < DataLen; i++)
    {
        /*  the last byte don't need send ACK*/
        if (i == (DataLen - 1))
        {
            I2C_WRITE_REG((g_I2cKernelAddr[I2cNum] + I2C_COM_REB), (I2C_READ | (~I2C_SEND_ACK)));
        }
        /*  if i2c master receive data will send ACK*/
        else
        {
            I2C_WRITE_REG((g_I2cKernelAddr[I2cNum] + I2C_COM_REB), I2C_READ);
        }

        if (I2C_DRV_WaitRead(I2cNum))
        {
            //local_irq_restore(IntFlag);
            //HI_ERR_I2C("wait read data timeout!\n");
            return HI_ERR_I2C_READ_TIMEOUT;
        }

        dataTmp = I2C_READ_REG((g_I2cKernelAddr[I2cNum] + I2C_RXR_REG));
        *(pData + i) = dataTmp & 0xff;

        if (I2C_DRV_WaitWriteEnd(I2cNum))
        {
            //local_irq_restore(IntFlag);
            //HI_ERR_I2C("wait write data timeout!\n");
            return HI_ERR_I2C_WRITE_TIMEOUT;
        }
    }

    /* send stop flag bit*/
    I2C_WRITE_REG((g_I2cKernelAddr[I2cNum] + I2C_COM_REB), I2C_STOP);
    if (I2C_DRV_WaitWriteEnd(I2cNum))
    {
        //local_irq_restore(IntFlag);
        //HI_ERR_I2C("wait write data timeout!\n");
        return HI_ERR_I2C_WRITE_TIMEOUT;
    }

    //local_irq_restore(IntFlag);

    return HI_SUCCESS;
}

int I2C_DRV_ReadIsr(HI_U32 I2cNum, HI_U8 I2cDevAddr, HI_BOOL bSendSlave, HI_U32 I2cRegAddr, HI_U32 I2cRegAddrByteNum,
                 HI_U8 *pData, HI_U32 DataLen)
{
    HI_U32 dataTmp = 0xff;
    HI_U32 i;

    //    unsigned long   IntFlag;
    HI_U32 RegAddr;
    unsigned int timeout;
    int s32Ret;

#warning I2C_DRV_ReadIsr: TODO
    //local_irq_save(IntFlag);

    I2C_WRITE_REG((g_I2cKernelAddr[I2cNum] + I2C_ICR_REG), I2C_CLEAR_ALL);

    memset(&Data_811440e0[I2cNum], 0, sizeof(struct Struct_811440e0));

    if (bSendSlave)
    {
        /* send devide address*/
        blaaa(I2cNum, Data_811440e0[I2cNum].Data_12288, 1, I2cDevAddr & WRITE_OPERATION, 10);
        Data_811440e0[I2cNum].Data_12288++;
    }

    /* send register address which will need to write*/
    for (i = 0; i < I2cRegAddrByteNum; i++)
    {
        RegAddr = I2cRegAddr >> ((I2cRegAddrByteNum - i - 1) * 8);

        blaaa(I2cNum, Data_811440e0[I2cNum].Data_12288, 1, RegAddr, 2);
        Data_811440e0[I2cNum].Data_12288++;
    }

    /* send register address which will need to read */
    blaaa(I2cNum, Data_811440e0[I2cNum].Data_12288, 1, I2cDevAddr | READ_OPERATION, 10);
    Data_811440e0[I2cNum].Data_12288++;

    /* repetitivily read data */
    for (i = 0; i < DataLen; i++)
    {
        /*  the last byte don't need send ACK*/
#if 0
        if (i == (DataLen - 1))
        {
            I2C_WRITE_REG((g_I2cKernelAddr[I2cNum] + I2C_COM_REB), (I2C_READ | (~I2C_SEND_ACK)));
        }
        /*  if i2c master receive data will send ACK*/
        else
        {
            I2C_WRITE_REG((g_I2cKernelAddr[I2cNum] + I2C_COM_REB), I2C_READ);
        }
#else
        blaa1(I2cNum, Data_811440e0[I2cNum].Data_12288, 2,
        		(i == (DataLen - 1))? (I2C_READ | (~I2C_SEND_ACK)): I2C_READ);
        Data_811440e0[I2cNum].Data_12288++;
#endif

#if 0
        if (I2C_DRV_WaitRead(I2cNum))
        {
            //local_irq_restore(IntFlag);
            //HI_ERR_I2C("wait read data timeout!\n");
            return HI_ERR_I2C_READ_TIMEOUT;
        }

        dataTmp = I2C_READ_REG((g_I2cKernelAddr[I2cNum] + I2C_RXR_REG));
        *(pData + i) = dataTmp & 0xff;

        if (I2C_DRV_WaitWriteEnd(I2cNum))
        {
            //local_irq_restore(IntFlag);
            //HI_ERR_I2C("wait write data timeout!\n");
            return HI_ERR_I2C_WRITE_TIMEOUT;
        }
#endif
    }

    /* send stop flag bit*/
#if 0
    I2C_WRITE_REG((g_I2cKernelAddr[I2cNum] + I2C_COM_REB), I2C_STOP);
    if (I2C_DRV_WaitWriteEnd(I2cNum))
    {
        //local_irq_restore(IntFlag);
        //HI_ERR_I2C("wait write data timeout!\n");
        return HI_ERR_I2C_WRITE_TIMEOUT;
    }
#endif
    blaa1(I2cNum, Data_811440e0[I2cNum].Data_12288, 0, I2C_STOP);
    Data_811440e0[I2cNum].Data_12288++;

    HI_INFO_I2C("index:%d, len:%d\n",
    		Data_811440e0[I2cNum].Data_12292,
    		Data_811440e0[I2cNum].Data_12288); //982

	blaa2(I2cNum, Data_811440e0[I2cNum].Data_12288++);

	timeout = (Data_811440e0[I2cNum].Data_12288 * 800000) / g_aI2cRate[I2cNum];

	if (timeout < 10)
	{
		timeout = 10;
	}

	HI_INFO_I2C("timeout:%d\n", timeout);

	//timeout = timeout * 1000 / 1000;

	s32Ret = wait_event_interruptible_timeout(g_astI2cShare[I2cNum].I2cWaitQueue,
			Data_811440e0[I2cNum].Data_12292 == Data_811440e0[I2cNum].Data_12288,
			timeout * 1000 / 1000/*timeout*/);

	HI_INFO_I2C("s32Ret:%d\n", s32Ret);

	if (s32Ret <= 0)
	{
		HI_ERR_I2C("%s, index:%d, len:%d\n", __FUNCTION__,
				Data_811440e0[I2cNum].Data_12292,
				Data_811440e0[I2cNum].Data_12288);
		return 0x80440009;
	}

    return HI_SUCCESS;
}

/*****************************************************************************
 Prototype    :
 Description  : I2C  mudole suspend function
 Input        : None
 Output       : None
 Return Value : None
*****************************************************************************/
struct i2c_pm_Info
{
    unsigned int rsclh;
    unsigned int rscll;
};
//static int i2cState = 0; //8115c220
static struct i2c_pm_Info pmStatus[HI_I2C_MAX_NUM];

/*
static void  i2c_pm_reset(void)
{
    int i;
    i2cState = 0;
    for(i = 0; i < HI_I2C_MAX_NUM; i++)
    {
        if (i > HI_UNF_I2C_CHANNEL_QAM)
        {
            break;
        }
        pmStatus[i].rsclh = I2C_DFT_RATE;
        pmStatus[i].rscll = I2C_DFT_RATE;
    }
    return;
}
 */

/* beacuse this mudule have opened in  tuner/e2prom ModeuleInit, so relational opened operation register need to  store */
int  i2c_pm_suspend(PM_BASEDEV_S *pdev, pm_message_t state)
{
    int i;
    int ret;

    ret = down_trylock(&g_I2cMutex);
    if (ret)
    {
        HI_INFO_I2C("lock err!\n");
        return -1;
    }

    // 1

    // 2
    for (i = 0; i < HI_STD_I2C_NUM; i++)
    {
        /* disable all i2c interrupt */
        I2C_WRITE_REG((g_I2cKernelAddr[i] + I2C_CTRL_REG), 0x0);

        /* clear all i2c interrupt*/
        I2C_WRITE_REG((g_I2cKernelAddr[i] + I2C_ICR_REG), I2C_CLEAR_ALL);

        /*store  I2C_SCL_H and  I2C_SCL_L  register*/
        pmStatus[i].rsclh = I2C_READ_REG(g_I2cKernelAddr[i] + I2C_SCL_H_REG);
        pmStatus[i].rscll = I2C_READ_REG(g_I2cKernelAddr[i] + I2C_SCL_L_REG);
    }

#if  defined (CHIP_TYPE_hi3716cv200es) || defined (CHIP_TYPE_hi3716cv200) \
	|| defined (CHIP_TYPE_hi3719cv100) || defined (CHIP_TYPE_hi3718cv100)  \
	|| defined (CHIP_TYPE_hi3719mv100) || defined (CHIP_TYPE_hi3719mv100_a)\
	|| defined (CHIP_TYPE_hi3718mv100)
    regI2CStore[0] = g_pstRegCrg->PERI_CRG27.u32;
#endif


    up(&g_I2cMutex);
    HI_INFO_I2C("i2c_pm_suspend ok \n");
    return 0;
}

int  i2c_pm_resume(PM_BASEDEV_S *pdev)
{
    int i;
    int ret;

    ret = down_trylock(&g_I2cMutex);
    if (ret)
    {
        HI_INFO_I2C("lock err!\n"); //1107
        return -1;
    }

#if  defined (CHIP_TYPE_hi3716cv200es) || defined (CHIP_TYPE_hi3716cv200) \
	|| defined (CHIP_TYPE_hi3719cv100) || defined (CHIP_TYPE_hi3718cv100)  \
	|| defined (CHIP_TYPE_hi3719mv100) || defined (CHIP_TYPE_hi3719mv100_a)\
	|| defined (CHIP_TYPE_hi3718mv100)
    g_pstRegCrg->PERI_CRG27.u32 = regI2CStore[0];
#endif

    // 1
    // 2
    for (i = 0; i < HI_STD_I2C_NUM; i++)
    {
        /*disable all i2c interrupt*/
        I2C_WRITE_REG((g_I2cKernelAddr[i] + I2C_CTRL_REG), 0x0);

        /*resume previous store register before suspend*/
        I2C_WRITE_REG((g_I2cKernelAddr[i] + I2C_SCL_H_REG), pmStatus[i].rsclh);
        I2C_WRITE_REG((g_I2cKernelAddr[i] + I2C_SCL_L_REG), pmStatus[i].rscll);

        /*  config scl clk rate*/
        I2C_DRV_SetRate(i, I2C_DFT_RATE);

        /*clear all i2c interrupt*/
        I2C_WRITE_REG((g_I2cKernelAddr[i] + I2C_ICR_REG), I2C_CLEAR_ALL);

        /*enable relative interrupt*/
        I2C_WRITE_REG((g_I2cKernelAddr[i] + I2C_CTRL_REG), (I2C_ENABLE | I2C_UNMASK_TOTAL | I2C_UNMASK_ALL));
    }

    //i2c_pm_reset();
    up(&g_I2cMutex);
    HI_INFO_I2C("i2c_pm_resume ok \n");
    return 0;
}

void I2C_DRV_Read_sonyIsr(HI_U32 I2cNum)
{
	memset(&Data_811440e0[I2cNum], 0, 12296);
}


void ByteTransferOver(HI_U32 I2cNum)
{
	//unsigned int lr, ip;
	int reg;

	if (I2cNum >= HI_STD_I2C_NUM)
	{
		return;
	}

	//ip = Data_811440e0[I2cNum].Data_12288;
	//lr = Data_811440e0[I2cNum].Data_12292;
	if (Data_811440e0[I2cNum].Data_12292/*lr*/ < /*ip*/Data_811440e0[I2cNum].Data_12288)
	{
		//8061c6ec
		I2C_DRV_ReadRXR(I2cNum, Data_811440e0[I2cNum].Data_12292/*lr*/);

		reg = Data_811440e0[I2cNum].Data_0[Data_811440e0[I2cNum].Data_12292].Data_8;
		if (Data_811440e0[I2cNum].Data_0[Data_811440e0[I2cNum].Data_12292].Data_0 == 0)
		{
			//8061c740
			I2C_WRITE_REG(g_I2cKernelAddr[I2cNum] + I2C_COM_REB, reg);
		}
		else if (Data_811440e0[I2cNum].Data_0[Data_811440e0[I2cNum].Data_12292].Data_0 == 1)
		{
			//8061c7bc
			I2C_WRITE_REG(g_I2cKernelAddr[I2cNum] + I2C_TXR_REG,
					Data_811440e0[I2cNum].Data_0[Data_811440e0[I2cNum].Data_12292].fill_4);
			I2C_WRITE_REG(g_I2cKernelAddr[I2cNum] + I2C_COM_REB, reg);
		}
		else if (Data_811440e0[I2cNum].Data_0[Data_811440e0[I2cNum].Data_12292].Data_0 == 2)
		{
			//8061c740
			I2C_WRITE_REG(g_I2cKernelAddr[I2cNum] + I2C_COM_REB, reg);
		}
		//8061c728
		Data_811440e0[I2cNum].Data_12292++;
	}
	else
	{
		if ((Data_811440e0[I2cNum].Data_0[/*ip*/Data_811440e0[I2cNum].Data_12288-1].Data_8 & 4))
		{
			//8061c764
			HI_INFO_I2C("wakeup:index:%d, Len:%d\n",
					Data_811440e0[I2cNum].Data_12292/*lr*/,
					Data_811440e0[I2cNum].Data_12288/*ip*/); //1166

			I2C_DRV_ReadRXR(I2cNum, Data_811440e0[I2cNum].Data_12292);

			wake_up_interruptible(&g_astI2cShare[I2cNum].I2cWaitQueue);
		}
		else
		{
			HI_INFO_I2C("wakeup:index:%d, Len:%d\n",
					Data_811440e0[I2cNum].Data_12292/*lr*/,
					Data_811440e0[I2cNum].Data_12288/*ip*/); //1173

			wake_up_interruptible(&g_astI2cShare[I2cNum].I2cWaitQueue);
		}
	}
}

static irqreturn_t I2C_Isr(int irq, void *dev_id)
{
	HI_U32 reg;
	int i;

#warning TODO: I2C_Isr
	if (irq == 70) i = 0;
	else if (irq == 71) i = 1;
	else if (irq == 72) i = 2;
	else if (irq == 73) i = 3;
	else if (irq == 76) i = 4;
	else if (irq == 75) i = 5;
	else if (irq == 74) i = 6;
	else if (irq == 89) i = 7;
	else return IRQ_HANDLED;

	reg = I2C_READ_REG(g_I2cKernelAddr[i] + I2C_SR_REG);
	if ((reg & 5) == 1)
	{
		ByteTransferOver(i);
	}

	reg = I2C_READ_REG(g_I2cKernelAddr[i] + I2C_ICR_REG);
	reg |= 5;
	I2C_WRITE_REG(g_I2cKernelAddr[i] + I2C_ICR_REG, reg);

	return IRQ_HANDLED;
}

static int getIrq(int a)
{
	switch (a)
	{
	case 4: return 76;
	case 6: return 74;
	case 7: return 89;
	default: return 70+a;
	}
}

/*****************************************************************************/
static HI_VOID HI_DRV_I2C_Open(HI_VOID)
{
    HI_S32 Ret;
    HI_U32 i;

#warning TODO: HI_DRV_I2C_Open

    if (1 == i2cState)
    {
        return;
    }

    Ret = down_interruptible(&g_I2cMutex);
    if (Ret)
    {
        HI_INFO_I2C("lock g_I2cMutex error.\n"); //1226
        return;
    }

    for (i = 0; i < HI_STD_I2C_NUM; i++)
    {
        /*disable all i2c interrupt*/
        I2C_WRITE_REG((g_I2cKernelAddr[i] + I2C_CTRL_REG), 0x0);

        /*  config scl clk rate*/
        I2C_DRV_SetRate(i, I2C_DFT_RATE);

        /*clear all i2c interrupt*/
        I2C_WRITE_REG((g_I2cKernelAddr[i] + I2C_ICR_REG), I2C_CLEAR_ALL);

#if 0
        /*enable relative interrupt*/
        I2C_WRITE_REG((g_I2cKernelAddr[i] + I2C_CTRL_REG), (I2C_ENABLE | I2C_UNMASK_TOTAL | I2C_UNMASK_ALL));
#else
        I2C_WRITE_REG((g_I2cKernelAddr[i] + I2C_CTRL_REG), 0x185);
#endif

#if 0
    	g_astI2cShare[i].Data_224 = 0;
    	g_astI2cShare[i].Data_228 = 1;
    	g_astI2cShare[i].Data_236 = g_astI2cShare[i].Data_232 = &g_astI2cShare[i].Data_232;
#else
#warning TODO
    	g_astI2cShare[i].Data_224.lock.raw_lock.slock = 0;
    	g_astI2cShare[i].Data_224.count = 1;
    	INIT_LIST_HEAD(&g_astI2cShare[i].Data_224.wait_list);
#endif
    	init_waitqueue_head(&g_astI2cShare[i].I2cWaitQueue);

    	memset(i2cIrqStrings[i], 0, 8);
    	snprintf(i2cIrqStrings[i], 8, "hi_i2c%d_irq", i);

    	Ret = request_irq(getIrq(i), I2C_Isr, 0x20, i2cIrqStrings[i], 0);
    	if (Ret)
    	{
            HI_ERR_I2C("register i2c %d Isr failed 0x%x.\n", i, Ret); //1255
    	}
    }

    i2cState = 1;

    up(&g_I2cMutex);
    return;
}

HI_S32 I2C_Ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    HI_S32 Ret = 0;
    HI_U8  *pData = NULL;
    I2C_DATA_S I2cData;
    I2C_RATE_S I2cRate;
    void __user *argp = (void __user*)arg;

    Ret = down_interruptible(&g_I2cMutex);
    if (Ret)
    {
        HI_INFO_I2C("lock g_I2cMutex error.\n");
        return HI_FAILURE;
    }

    switch (cmd)
    {
    case CMD_I2C_WRITE:
        {
            if (copy_from_user(&I2cData, argp, sizeof(I2C_DATA_S)))
            {
                HI_INFO_I2C("copy data from user fail!\n");
                Ret = HI_ERR_I2C_COPY_DATA_ERR;
                break;
            }

            pData = HI_KMALLOC(HI_ID_I2C, I2cData.DataLen, GFP_KERNEL);
            if (!pData)
            {
                HI_ERR_I2C("i2c kmalloc fail!\n");
                Ret = HI_ERR_I2C_MALLOC_ERR;
                break;
            }

            if (copy_from_user(pData, I2cData.pData, I2cData.DataLen))
            {
                HI_INFO_I2C("copy data from user fail!\n");
                HI_KFREE(HI_ID_I2C, pData);
                Ret = HI_ERR_I2C_COPY_DATA_ERR;
                break;
            }

            Ret = I2C_DRV_Write(I2cData.I2cNum, I2cData.I2cDevAddr, I2cData.I2cRegAddr, I2cData.I2cRegCount, pData,
                                I2cData.DataLen, HI_TRUE);
            HI_KFREE(HI_ID_I2C, pData);
            break;
        }

    case CMD_I2C_READ:
        {
            if (copy_from_user(&I2cData, argp, sizeof(I2C_DATA_S)))
            {
                HI_INFO_I2C("copy data from user fail!\n");
                Ret = HI_ERR_I2C_COPY_DATA_ERR;
                break;
            }

            pData = HI_KMALLOC(HI_ID_I2C, I2cData.DataLen, GFP_KERNEL);
            if (!pData)
            {
                HI_ERR_I2C("i2c kmalloc fail!\n");
                Ret = HI_ERR_I2C_MALLOC_ERR;
                break;
            }

            Ret = I2C_DRV_Read(I2cData.I2cNum, I2cData.I2cDevAddr, HI_TRUE, I2cData.I2cRegAddr, I2cData.I2cRegCount,
                               pData, I2cData.DataLen);
            if (HI_SUCCESS == Ret)
            {
                if (copy_to_user(I2cData.pData, pData, I2cData.DataLen))
                {
                    HI_INFO_I2C("copy data to user fail!\n");
                    Ret = HI_ERR_I2C_COPY_DATA_ERR;
                }
            }

            HI_KFREE(HI_ID_I2C, pData);
            break;
        }

    case  CMD_I2C_SET_RATE:
        {
            if (copy_from_user(&I2cRate, argp, sizeof(I2C_RATE_S)))
            {
                HI_INFO_I2C("copy data from user fail!\n");
                Ret = HI_FAILURE;
                break;
            }

            I2C_DRV_SetRate(I2cRate.I2cNum, I2cRate.I2cRate);
            Ret = HI_SUCCESS;
            break;
        }
    default:
        {
            up(&g_I2cMutex);
            return -ENOIOCTLCMD;
        }
    }

    up(&g_I2cMutex);
    return Ret;
}

HI_S32 HI_DRV_I2C_Init(HI_VOID)
{
    //HI_CHIP_TYPE_E enChipType;
    //HI_U32 u32ChipVersion = 0;
    HI_U32 u32RegVal = 0;
    HI_S32 s32Ret = HI_SUCCESS;

#warning TODO: HI_DRV_I2C_Init

    s32Ret = HI_DRV_MODULE_Register(HI_ID_I2C, "HI_I2C", (HI_VOID *)&g_stI2cExtFuncs);
    if (HI_SUCCESS != s32Ret)
    {
        HI_INFO_I2C(" GPIO Module register failed 0x%x.\n", s32Ret); //1411
        return HI_FAILURE;
    }

    g_I2cKernelAddr[0] = IO_ADDRESS(I2C0_PHY_ADDR);
    g_I2cKernelAddr[1] = IO_ADDRESS(I2C1_PHY_ADDR);
    g_I2cKernelAddr[2] = IO_ADDRESS(I2C2_PHY_ADDR);
    g_I2cKernelAddr[3] = IO_ADDRESS(I2C3_PHY_ADDR);
    g_I2cKernelAddr[4] = IO_ADDRESS(I2C4_PHY_ADDR);
#if  defined (CHIP_TYPE_hi3716cv200es) || defined (CHIP_TYPE_hi3716cv200) \
	|| defined (CHIP_TYPE_hi3719cv100) || defined (CHIP_TYPE_hi3718cv100)  \
	|| defined (CHIP_TYPE_hi3719mv100) || defined (CHIP_TYPE_hi3719mv100_a)\
	|| defined (CHIP_TYPE_hi3718mv100) || defined (CHIP_TYPE_hi3798cv100)
    g_I2cKernelAddr[5] = IO_ADDRESS(I2CQAM_PHY_ADDR);
#endif

#if defined (CHIP_TYPE_hi3798cv100)
    g_I2cKernelAddr[6] = IO_ADDRESS(0xF8B1C000);
    g_I2cKernelAddr[7] = IO_ADDRESS(0xF8B1D000);
#endif

#if  defined (CHIP_TYPE_hi3716cv200es) || defined (CHIP_TYPE_hi3716cv200) \
	|| defined (CHIP_TYPE_hi3719cv100) || defined (CHIP_TYPE_hi3718cv100)  \
	|| defined (CHIP_TYPE_hi3719mv100) || defined (CHIP_TYPE_hi3719mv100_a)\
	|| defined (CHIP_TYPE_hi3718mv100) 
    u32RegVal  = g_pstRegCrg->PERI_CRG27.u32;
    u32RegVal &= ~0x222222;
    u32RegVal |= 0x111111;
    g_pstRegCrg->PERI_CRG27.u32 = u32RegVal;
#elif  defined (CHIP_TYPE_hi3798cv100)
    u32RegVal  = g_pstRegCrg->PERI_CRG27.u32;
    u32RegVal &= ~0x78F78F;
    u32RegVal |=  0x00078F;
    g_pstRegCrg->PERI_CRG27.u32 = u32RegVal;
#endif
    HI_DRV_I2C_Open();

    return 0;
}

HI_VOID HI_DRV_I2C_DeInit(HI_VOID)
{
    HI_U32 u32RegVal;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 i;

    s32Ret = HI_DRV_MODULE_UnRegister(HI_ID_I2C);
    if (HI_SUCCESS != s32Ret)
    {
        HI_INFO_I2C(" GPIO Module unregister failed 0x%x.\n", s32Ret); //1470
    }


#if  defined (CHIP_TYPE_hi3716cv200es) || defined (CHIP_TYPE_hi3716cv200) \
	|| defined (CHIP_TYPE_hi3719cv100) || defined (CHIP_TYPE_hi3718cv100)  \
	|| defined (CHIP_TYPE_hi3719mv100) || defined (CHIP_TYPE_hi3719mv100_a)\
	|| defined (CHIP_TYPE_hi3718mv100) 
    u32RegVal  = g_pstRegCrg->PERI_CRG27.u32;
    u32RegVal |= 0x222222;
    g_pstRegCrg->PERI_CRG27.u32 = u32RegVal;
#elif defined (CHIP_TYPE_hi3798cv100)
    u32RegVal  = g_pstRegCrg->PERI_CRG27.u32;
    u32RegVal &= ~0x78F78F;
    u32RegVal |=  0x78F000;
    g_pstRegCrg->PERI_CRG27.u32 = u32RegVal;

    for (i = 0; i < HI_STD_I2C_NUM; i++)
    {
        free_irq(getIrq(i), 0);
    }
#endif

    i2cState = 0;

    return;
}

HI_S32 HI_DRV_I2C_WriteConfig(HI_U32 I2cNum, HI_U8 I2cDevAddr)
{
    HI_S32 Ret;

    if (I2cNum >= HI_STD_I2C_NUM)
    {
        HI_ERR_I2C("I2cNum(%d) is wrong, STD_I2C_NUM is %d\n", I2cNum, HI_STD_I2C_NUM); //1507
        return HI_FAILURE;
    }

#if 0
    Ret = down_interruptible(&g_I2cMutex);
#else
    Ret = down_interruptible(&g_astI2cShare[I2cNum].Data_224);
#endif
    if (Ret)
    {
        HI_INFO_I2C("lock g_I2cMutex error.\n"); //1514
        return HI_FAILURE;
    }

    Ret = I2C_DRV_WriteConfig(I2cNum, I2cDevAddr);

#if 0
    up(&g_I2cMutex);
#else
    up(&g_astI2cShare[I2cNum].Data_224);
#endif

    return Ret;
}

HI_S32 HI_DRV_I2C_Write(HI_U32 I2cNum, HI_U8 I2cDevAddr, HI_U32 I2cRegAddr, HI_U32 I2cRegAddrByteNum, HI_U8 *pData,
                        HI_U32 DataLen)
{
    HI_S32 Ret;

    if (I2cNum >= HI_STD_I2C_NUM)
    {
        HI_ERR_I2C("I2cNum(%d) is wrong, STD_I2C_NUM is %d\n", I2cNum, HI_STD_I2C_NUM); //1532
        return HI_FAILURE;
    }

#if 0
    Ret = down_interruptible(&g_I2cMutex);
#else
    Ret = down_interruptible(&g_astI2cShare[I2cNum].Data_224);
#endif
    if (Ret)
    {
        HI_INFO_I2C("lock g_I2cMutex error.\n"); //1539
        return HI_FAILURE;
    }

    Ret = I2C_DRV_WriteIsr(I2cNum, I2cDevAddr, I2cRegAddr, I2cRegAddrByteNum, pData, DataLen, HI_TRUE);
    HI_INFO_I2C("Ret=0x%x, I2cNum=%d, DevAddr=0x%x, RegAddr=0x%x, Num=%d, Len=%d, data0=0x%x\n", Ret, I2cNum,
                I2cDevAddr, I2cRegAddr, I2cRegAddrByteNum, DataLen, pData[0]); //1549

#if 0
    up(&g_I2cMutex);
#else
    up(&g_astI2cShare[I2cNum].Data_224);
#endif

    return Ret;
}

HI_S32 HI_DRV_I2C_Write_NoSTOP(HI_U32 I2cNum, HI_U8 I2cDevAddr, HI_U32 I2cRegAddr, HI_U32 I2cRegAddrByteNum,
                               HI_U8 *pData, HI_U32 DataLen)
{
    HI_S32 Ret;

    if (I2cNum >= HI_STD_I2C_NUM)
    {
        HI_ERR_I2C("I2cNum(%d) is wrong, STD_I2C_NUM is %d\n", I2cNum, HI_STD_I2C_NUM); //1563
        return HI_FAILURE;
    }

#if 0
    Ret = down_interruptible(&g_I2cMutex);
#else
    Ret = down_interruptible(&g_astI2cShare[I2cNum].Data_224);
#endif
    if (Ret)
    {
        HI_INFO_I2C("lock g_I2cMutex error.\n"); //1570
        return HI_FAILURE;
    }

    Ret = I2C_DRV_WriteIsr(I2cNum, I2cDevAddr, I2cRegAddr, I2cRegAddrByteNum, pData, DataLen, HI_FALSE);
    HI_INFO_I2C("Ret=0x%x, I2cNum=%d, DevAddr=0x%x, RegAddr=0x%x, Num=%d, Len=%d, data0=0x%x\n", Ret, I2cNum,
                I2cDevAddr, I2cRegAddr, I2cRegAddrByteNum, DataLen, pData[0]); //1580

#if 0
    up(&g_I2cMutex);
#else
    up(&g_astI2cShare[I2cNum].Data_224);
#endif

    return Ret;
}

HI_S32 HI_DRV_I2C_Read(HI_U32 I2cNum, HI_U8 I2cDevAddr, HI_U32 I2cRegAddr, HI_U32 I2cRegAddrByteNum, HI_U8 *pData,
                       HI_U32 DataLen)
{
    HI_S32 Ret;

    if (I2cNum >= HI_STD_I2C_NUM)
    {
        HI_ERR_I2C("I2cNum(%d) is wrong, STD_I2C_NUM is %d\n", I2cNum, HI_STD_I2C_NUM); //1594
        return HI_FAILURE;
    }

#if 0
    Ret = down_interruptible(&g_I2cMutex);
#else
    Ret = down_interruptible(&g_astI2cShare[I2cNum].Data_224);
#endif
    if (Ret)
    {
        HI_INFO_I2C("lock g_I2cMutex error.\n"); //1601
        return HI_FAILURE;
    }

    Ret = I2C_DRV_ReadIsr(I2cNum, I2cDevAddr, HI_TRUE, I2cRegAddr, I2cRegAddrByteNum, pData, DataLen);
    HI_INFO_I2C("Ret=0x%x, I2cNum=%d, DevAddr=0x%x, RegAddr=0x%x, Num=%d, Len=%d\n", Ret, I2cNum, I2cDevAddr,
                I2cRegAddr, I2cRegAddrByteNum, DataLen); //1611

#if 0
    up(&g_I2cMutex);
#else
    up(&g_astI2cShare[I2cNum].Data_224);
#endif

    return Ret;
}

/* Added begin: l00185424 20120131, for avl6211 demod */
/* Some I2C needn't send slave address before read */
HI_S32 HI_DRV_I2C_ReadDirectly(HI_U32 I2cNum, HI_U8 I2cDevAddr, HI_U32 I2cRegAddr, HI_U32 I2cRegAddrByteNum,
                               HI_U8 *pData, HI_U32 DataLen)
{
    HI_S32 Ret;

    if (I2cNum >= HI_STD_I2C_NUM)
    {
        HI_ERR_I2C("I2cNum(%d) is wrong, STD_I2C_NUM is %d\n", I2cNum, HI_STD_I2C_NUM); //1673
        return HI_FAILURE;
    }

#if 0
    Ret = down_interruptible(&g_I2cMutex);
#else
    Ret = down_interruptible(&g_astI2cShare[I2cNum].Data_224);
#endif
    if (Ret)
    {
        HI_INFO_I2C("lock g_I2cMutex error.\n"); //1680
        return HI_FAILURE;
    }

    Ret = I2C_DRV_ReadIsr(I2cNum, I2cDevAddr, HI_FALSE, I2cRegAddr, I2cRegAddrByteNum, pData, DataLen);
    HI_INFO_I2C("Ret=0x%x, I2cNum=%d, DevAddr=0x%x, RegAddr=0x%x, Num=%d, Len=%d\n", Ret, I2cNum, I2cDevAddr,
                I2cRegAddr, I2cRegAddrByteNum, DataLen); //1690

#if 0
    up(&g_I2cMutex);
#else
    up(&g_astI2cShare[I2cNum].Data_224);
#endif

    return Ret;
}

HI_S32 HI_DRV_I2C_SetRate(HI_U32 I2cNum, HI_U32 I2cRate)
{
	return I2C_DRV_SetRate(I2cNum, I2cRate);
}

/* Added end: l00185424 20120131, for avl6211 demod */

#ifndef MODULE
EXPORT_SYMBOL(I2C_Ioctl);
EXPORT_SYMBOL(i2c_pm_suspend);
EXPORT_SYMBOL(i2c_pm_resume);
#endif

EXPORT_SYMBOL(HI_DRV_I2C_Init);
EXPORT_SYMBOL(HI_DRV_I2C_DeInit);

#if 1
EXPORT_SYMBOL(HI_DRV_I2C_WriteConfig);
EXPORT_SYMBOL(HI_DRV_I2C_Write);
EXPORT_SYMBOL(HI_DRV_I2C_Read);
EXPORT_SYMBOL(HI_DRV_I2C_ReadDirectly);
EXPORT_SYMBOL(HI_DRV_I2C_Write_NoSTOP);
#endif
