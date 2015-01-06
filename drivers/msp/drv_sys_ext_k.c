/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : drv_sys_ext_k.c
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2006/02/09
  Description   :
******************************************************************************/
#include <linux/kernel.h>
#include <linux/module.h>
#include <mach/hardware.h>
#include <linux/sched.h>
#include <linux/math64.h>

#include "hi_type.h"
#include "hi_debug.h"
#include "hi_reg_common.h"
#include "hi_drv_sys.h"

#define DIV_NS_TO_MS  1000000

#if    defined(CHIP_TYPE_hi3716cv200)   \
    || defined(CHIP_TYPE_hi3718cv100)   \
    || defined(CHIP_TYPE_hi3718mv100)   \
    || defined(CHIP_TYPE_hi3719cv100)   \
    || defined(CHIP_TYPE_hi3719mv100)   \
    || defined(CHIP_TYPE_hi3719mv100_a)

#define GET_CHIP_INFO(type, version) do{                  \
    if (0x37160200 == g_pstRegSysCtrl->SC_SYSID)          \
    {                                                     \
        switch (g_pstRegPeri->PERI_SOC_FUSE.bits.chip_id) \
        {                                                 \
            case 0x8:                                     \
                type    = HI_CHIP_TYPE_HI3719C;           \
                version = HI_CHIP_VERSION_V100;           \
                break;                                    \
            case 0x10:                                    \
                type    = HI_CHIP_TYPE_HI3718C;           \
                version = HI_CHIP_VERSION_V100;           \
                break;                                    \
            case 0x1e:                                    \
            case 0x1f:                                    \
                type    = HI_CHIP_TYPE_HI3719M_A;         \
                version = HI_CHIP_VERSION_V100;           \
                break;                                    \
            default:                                      \
                type    = HI_CHIP_TYPE_HI3716C;           \
                version = HI_CHIP_VERSION_V200;           \
                break;                                    \
        }                                                 \
    }                                                     \
    else if (0x37190100 == g_pstRegSysCtrl->SC_SYSID)     \
    {                                                     \
        switch (g_pstRegPeri->PERI_SOC_FUSE.bits.chip_id) \
        {                                                 \
            case 0x4:                                     \
                type    = HI_CHIP_TYPE_HI3718M;           \
                version = HI_CHIP_VERSION_V100;           \
                break;                                    \
            default:                                      \
                type    = HI_CHIP_TYPE_HI3719M;           \
                version = HI_CHIP_VERSION_V100;           \
                break;                                    \
        }                                                 \
    }                                                     \
    else                                                  \
    {                                                     \
        HI_ERR_SYS("Unknown chip ID.\n");                 \
    }                                                     \
} while(0)

/*
 * 1:support; 0:not_support 
 */
HI_S32 HI_DRV_SYS_GetRoviSupport(HI_U32 *pu32Support)
{
#if 1
#warning TODO
#else
    *pu32Support = g_pstRegPeri->PERI_SOC_FUSE.bits.mven;
#endif

    return HI_SUCCESS;
}

#elif defined(CHIP_TYPE_hi3716cv200es)

#define GET_CHIP_INFO(type, version) do{            \
    if (0x19400200 == g_pstRegSysCtrl->SC_SYSID)    \
    {                                               \
        type    = HI_CHIP_TYPE_HI3716CES;           \
        version = HI_CHIP_VERSION_V200;             \
    }                                               \
    else                                            \
    {                                               \
        HI_ERR_SYS("Unknown chip ID.\n");           \
    }                                               \
} while(0)

/*
 * 1:support; 0:not_support 
 */
HI_S32 HI_DRV_SYS_GetRoviSupport(HI_U32 *pu32Support)
{
    *pu32Support = g_pstRegPeri->START_MODE.bits.mven;

    return HI_SUCCESS;
}
    
#else

#error "Unkown chip type!"

#endif

HI_VOID HI_DRV_SYS_GetChipVersion(HI_CHIP_TYPE_E *penChipType, HI_CHIP_VERSION_E *penChipVersion)
{
#if 1
#warning TODO
#else
    HI_CHIP_TYPE_E enChipType = HI_CHIP_TYPE_BUTT;
    HI_CHIP_VERSION_E enChipVersion = HI_CHIP_VERSION_BUTT;

    /* penChipType or penChipVersion maybe NULL, but not both */
    if (HI_NULL == penChipType && HI_NULL == penChipVersion)
    {
        HI_ERR_SYS("invalid input parameter.\n");
        return;
    }
    
    penChipType    = penChipType ? penChipType : &enChipType;
    penChipVersion = penChipVersion ? penChipVersion : &enChipVersion;

    GET_CHIP_INFO(*penChipType, *penChipVersion);
#endif
}

HI_S32 HI_DRV_SYS_GetTimeStampMs(HI_U32 *pu32TimeMs)
{
    HI_U64 u64TimeNow;

    if (HI_NULL == pu32TimeMs)
    {
        HI_ERR_SYS("null pointer error\n");
        return HI_FAILURE;
    }

    u64TimeNow = sched_clock();

    do_div(u64TimeNow, DIV_NS_TO_MS);

    *pu32TimeMs = (HI_U32)u64TimeNow;

    return HI_SUCCESS;
}

/*
 * from datasheet, the value of dolby_flag meaning: 0: support; 1: not_support .
 * but we change its meaning for return parameter : 0:not support; 1:support.
 */
HI_S32 HI_DRV_SYS_GetDolbySupport(HI_U32 *pu32Support)
{
#if 1
#warning TODO
#else
    *pu32Support = !(g_pstRegPeri->PERI_CHIP_INFO4.bits.dolby_flag);
#endif

    return HI_SUCCESS;
}

/*
 * 1:support; 0:not_support 
 */
HI_S32 HI_DRV_SYS_GetDtsSupport(HI_U32 *pu32Support)
{
#if 1
#warning TODO
#else
    *pu32Support = g_pstRegPeri->PERI_CHIP_INFO4.bits.dts_flag;
#endif

    return HI_SUCCESS;
}



HI_S32 HI_DRV_SYS_KInit(HI_VOID)
{
    return 0;
}

HI_VOID HI_DRV_SYS_KExit(HI_VOID)
{
    return ;
}

EXPORT_SYMBOL(HI_DRV_SYS_GetChipVersion);
EXPORT_SYMBOL(HI_DRV_SYS_GetTimeStampMs);
EXPORT_SYMBOL(HI_DRV_SYS_GetDolbySupport);
EXPORT_SYMBOL(HI_DRV_SYS_GetDtsSupport);
EXPORT_SYMBOL(HI_DRV_SYS_GetRoviSupport);


