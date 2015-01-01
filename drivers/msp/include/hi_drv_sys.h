/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_drv_sys.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2007/1/31
  Description   : hi3511 system private Head File
  History       :
  1.Date        : 2007/1/31
    Author      : c42025
    Modification: Created file

******************************************************************************/
#ifndef  __HI_DRV_SYS_H__
#define  __HI_DRV_SYS_H__

#include "hi_type.h"
#include "hi_debug.h"
#include "hi_common.h"

/* Define Debug Level For SYS */
#define HI_FATAL_SYS(fmt...) HI_FATAL_PRINT(HI_ID_SYS, fmt)
#define HI_ERR_SYS(  fmt...) HI_ERR_PRINT(HI_ID_SYS, fmt)
#define HI_WARN_SYS( fmt...) HI_WARN_PRINT(HI_ID_SYS, fmt)
#define HI_INFO_SYS( fmt...) HI_INFO_PRINT(HI_ID_SYS, fmt)

extern HI_S32  HI_DRV_SYS_Init(HI_VOID);
extern HI_VOID HI_DRV_SYS_Exit(HI_VOID);

extern HI_S32  HI_DRV_SYS_KInit(HI_VOID);
extern HI_VOID HI_DRV_SYS_KExit(HI_VOID);

extern HI_VOID HI_DRV_SYS_GetChipVersion(HI_CHIP_TYPE_E *penChipType, HI_CHIP_VERSION_E *penChipVersion);
extern HI_S32  HI_DRV_SYS_GetTimeStampMs(HI_U32 *pu32TimeMs);
extern HI_S32 HI_DRV_SYS_GetDolbySupport(HI_U32 *pu32Support);
extern HI_S32 HI_DRV_SYS_GetDtsSupport(HI_U32 *pu32Support);
extern HI_S32 HI_DRV_SYS_GetRoviSupport(HI_U32 *pu32Support);

#endif /* __HI_DRV_SYS_H__ */

