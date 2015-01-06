/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : mpi_priv_avplay.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2009/12/21
  Description   :
  History       :
  1.Date        : 2009/12/21
    Author      : w58735
    Modification: Created file

******************************************************************************/
#ifndef __HI_DRV_AVPLAY_H__
#define __HI_DRV_AVPLAY_H__

#include "hi_type.h"
#include "hi_debug.h"

#ifdef __cplusplus
#if __cplusplus
    extern "C"{
#endif
#endif

#define HI_FATAL_AVPLAY(fmt...) \
            HI_FATAL_PRINT(HI_ID_AVPLAY, fmt)

#define HI_ERR_AVPLAY(fmt...) \
            HI_ERR_PRINT(HI_ID_AVPLAY, fmt)

#define HI_WARN_AVPLAY(fmt...) \
            HI_WARN_PRINT(HI_ID_AVPLAY, fmt)

#define HI_INFO_AVPLAY(fmt...) \
            HI_INFO_PRINT(HI_ID_AVPLAY, fmt)


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
