/******************************************************************************
  Copyright (C), 2013-2023, Hisilicon Tech. Co., Ltd.
******************************************************************************
  File Name     : drv_reg_common.c
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2013/08/26
  Description   :
******************************************************************************/

#include <linux/kernel.h>
#include <linux/module.h>
#include <mach/hardware.h>

#include "hi_type.h"
#include "hi_reg_common.h"

volatile HI_REG_SYSCTRL_S   *g_pstRegSysCtrl    = (HI_REG_SYSCTRL_S*)IO_ADDRESS(HI_SYS_BASE_ADDR);
volatile HI_REG_PERI_S      *g_pstRegPeri       = (HI_REG_PERI_S*)IO_ADDRESS(HI_PERI_BASE_ADDR);
volatile HI_REG_IO_S        *g_pstRegIO         = (HI_REG_IO_S*)IO_ADDRESS(HI_IO_BASE_ADDR);
volatile HI_REG_CRG_S       *g_pstRegCrg        = (HI_REG_CRG_S*)IO_ADDRESS(HI_CRG_BASE_ADDR);

EXPORT_SYMBOL(g_pstRegSysCtrl);
EXPORT_SYMBOL(g_pstRegPeri);
EXPORT_SYMBOL(g_pstRegIO);
EXPORT_SYMBOL(g_pstRegCrg);

