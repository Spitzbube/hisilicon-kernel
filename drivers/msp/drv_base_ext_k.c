/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : common_base.c
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2010/01/25
  Description   : common driver moudle define
  History       :
  1.Date        : 2010/01/25
    Author      : jianglei��
    Modification: Created file

******************************************************************************/
#include <linux/version.h>
#include <linux/seq_file.h>
#include "hi_type.h"
#include "hi_drv_log.h"
#include "hi_drv_dev.h"
#include "hi_drv_proc.h"
#include "hi_drv_stat.h"
#include "hi_drv_sys.h"
#include "hi_drv_mmz.h"
#include "hi_drv_module.h"


/* Use "strings hi_xx.ko | grep "SDK_VERSION"" to get the version */
/*HI_CHAR g_ModuleVersion[160] ="SDK_VERSION:["\
    MKMARCOTOSTR(SDK_VERSION)"] Build Time:["\
    __DATE__", "__TIME__"]";
*/

HI_S32 HI_DRV_CommonInit(HI_VOID)
{
    HI_S32 ret;

    printk("--> HI_DRV_CommonInit\n");

    ret = HI_DRV_MMZ_Init();
    if(HI_SUCCESS != ret)
    {
        HI_ERR_SYS("DRV_MMZ_ModInit failed:%#x!\n", ret); //42
        goto ErrorExit_MMZ;
    }

    ret = HI_DRV_DEV_Init();
    if(HI_SUCCESS != ret)
    {
        HI_ERR_SYS("CMPI_DEV_ModInit_0 failed:%#x!\n", ret); //49
        goto ErrExit_DEV;
    }

    ret = HI_DRV_LOG_KInit();
    if(HI_SUCCESS != ret)
    {
        HI_ERR_SYS("HI_DRV_LOG_KInit failed:%#x!\n", ret); //56
        goto ErrExit_LOG;
    }

    ret = HI_DRV_PROC_KInit();
    if(HI_SUCCESS != ret)
    {
        HI_ERR_SYS("HI_DRV_PROC_KInit failed:%#x!\n", ret); //63
        goto ErrExit_PROC;
    }

    ret = HI_DRV_STAT_KInit();
    if(HI_SUCCESS != ret)
    {
        HI_ERR_SYS("HI_DRV_STAT_KInit failed:%#x!\n", ret); //70
        goto ErrExit_STAT;
    }

    ret = HI_DRV_SYS_KInit();
    if(HI_SUCCESS != ret)
    {
        HI_ERR_SYS("HI_DRV_STAT_KInit failed:%#x!\n", ret); //77
        goto ErrExit_SYS;
    }


    ret = HI_DRV_MMNGR_Init(HI_KMODULE_MAX_COUNT, HI_KMODULE_MEM_MAX_COUNT);
    if(HI_SUCCESS != ret)
    {
        HI_ERR_SYS("KModuleMgr_Init failed:%#x!\n", ret); //85
        goto ErrExit_SYS;
    }

    printk("<-- HI_DRV_CommonInit\n");

    return HI_SUCCESS;

ErrExit_SYS:
    HI_DRV_STAT_KExit();

ErrExit_STAT:
    HI_DRV_PROC_KExit();

ErrExit_PROC:
    HI_DRV_LOG_KExit();

ErrExit_LOG:
    HI_DRV_DEV_Exit();

ErrExit_DEV:
	HI_DRV_MMZ_Exit();

ErrorExit_MMZ:

    return ret;
}

HI_VOID HI_DRV_CommonExit(HI_VOID)
{
    HI_DRV_MMNGR_Exit();

    HI_DRV_SYS_KExit();

    HI_DRV_STAT_KExit();

    HI_DRV_LOG_KExit();

    HI_DRV_PROC_KExit();

    HI_DRV_DEV_Exit();

    return;
}


