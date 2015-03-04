/******************************************************************************

  Copyright (C), 2001-2011, Huawei Tech. Co., Ltd.

******************************************************************************
  File Name     : hifb_main_k.c
  Version       : Initial Draft
  Author        : h180450
  Created       : 2013/6/9
  Last Modified :
  Description   : framebuffer export function for mce
  History       :
 
******************************************************************************/

#include <linux/ctype.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/delay.h>

#include <linux/slab.h>
#include <linux/mm.h>

#include <linux/fb.h>
#include <asm/uaccess.h>

#include <asm/types.h>
#include <asm/stat.h>
#include <asm/fcntl.h>

#include <linux/interrupt.h>
#include "hi_module.h"
#include "hi_drv_module.h"

#include "drv_hifb_ext.h"
#include "drv_pdm_ext.h"
#include "hifb_p.h"
#define HIFB_NAME "HI_FB"
#define LOGO_LAYE_ADDR 0xff127400 //0xf8cc7000
#define SD_LAYE_ADDR   0xff127800 //0xf8cc8000
#define WBC_LAYE_ADDR  0xff127800 //0xf8cca800
#define NEW_ADDR	   0xff129400

#ifndef HI_ADVCA_FUNCTION_RELEASE
#define HIFB_PRINT_INFO  printk
#else
#define HIFB_PRINT_INFO(x...)
#endif

HI_VOID HifbSetLogoLayerEnable(HI_BOOL bEnable);

static volatile HI_U32 *g_u32LogoCtrlReg = HI_NULL;
static volatile HI_U32 *g_u32SDCtrlReg   = HI_NULL;
static volatile HI_U32 *g_u32WBCCtrlReg  = HI_NULL;
static volatile HI_U32 *g_u32Data_811437bc = HI_NULL;

static HIFB_EXPORT_FUNC_S s_HifbExportFuncs =
{
    .pfnHifbSetLogoLayerEnable             = HifbSetLogoLayerEnable,
#warning TODO: HIFB_EXPORT_FUNC_S
};

HI_VOID HifbSetLogoLayerEnable(HI_BOOL bEnable)
{
    PDM_EXPORT_FUNC_S *ps_PdmExportFuncs = HI_NULL;
	
    if (bEnable) 
    {
        *g_u32LogoCtrlReg |= (0x1 << 31);
        *g_u32SDCtrlReg   |= (0x1 << 31);
        *g_u32WBCCtrlReg  |= (0x1 << 31);
        *g_u32Data_811437bc |= (0x1 << 31);

        *(g_u32LogoCtrlReg + 1) = 0x1;
        *(g_u32SDCtrlReg + 1)   = 0x1;
        *(g_u32WBCCtrlReg + 1)  = 0x1;
        *(g_u32Data_811437bc +1) = 0x1;
    }
    else
    {
        *g_u32LogoCtrlReg &= ~(0x1 << 31);
        *g_u32SDCtrlReg   &= ~(0x1 << 31);
        *g_u32WBCCtrlReg  &= ~(0x1 << 31);
        *g_u32Data_811437bc &= ~(0x1 << 31);

        *(g_u32LogoCtrlReg + 1) = 0x1;
        *(g_u32SDCtrlReg + 1)   = 0x1;
        *(g_u32WBCCtrlReg + 1)  = 0x1;
        *(g_u32Data_811437bc + 1) = 0x1;

        msleep(40);

        /* free the reserve memory*/
        if (HI_SUCCESS != HI_DRV_MODULE_GetFunction(HI_ID_PDM, (HI_VOID**)&ps_PdmExportFuncs))
        {
        	return;
        }
		
        if(HI_NULL != ps_PdmExportFuncs)
        {
            ps_PdmExportFuncs->pfnPDM_ReleaseReserveMem(DISPLAY_BUFFER_HD);
            ps_PdmExportFuncs->pfnPDM_ReleaseReserveMem(DISPLAY_BUFFER_SD);
            ps_PdmExportFuncs->pfnPDM_ReleaseReserveMem(OPTM_GFX_WBC2_BUFFER);
            ps_PdmExportFuncs->pfnPDM_ReleaseReserveMem(HIFB_ZME_COEF_BUFFER);
        }
    }

    return;
}

HI_S32 hifb_init_module_k(HI_VOID)
{
    HI_S32 ret;

    g_u32LogoCtrlReg = (HI_U32*)ioremap_nocache(LOGO_LAYE_ADDR, 8);
	g_u32SDCtrlReg   = (HI_U32*)ioremap_nocache(SD_LAYE_ADDR, 8);
	g_u32WBCCtrlReg  = (HI_U32*)ioremap_nocache(WBC_LAYE_ADDR, 8);
	g_u32Data_811437bc = (HI_U32*)ioremap_nocache(NEW_ADDR, 8);
    ret = HI_GFX_MODULE_Register(HIGFX_FB_ID, HIFB_NAME, &s_HifbExportFuncs); 
    if (HI_SUCCESS != ret)
    {
        HIFB_PRINT_INFO("HI_DRV_MODULE_Register failed\n");
        hifb_cleanup_module_k();
        return ret;
    }
    return HI_SUCCESS;
}

HI_VOID hifb_cleanup_module_k(HI_VOID)
{
    HI_GFX_MODULE_UnRegister(HIGFX_FB_ID);
    iounmap(g_u32LogoCtrlReg);
	iounmap(g_u32SDCtrlReg);
	iounmap(g_u32WBCCtrlReg);
	iounmap(g_u32Data_811437bc);
}
