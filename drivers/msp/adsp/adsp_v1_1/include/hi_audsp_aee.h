/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_audsp_aee.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2013/02/28
  Description   :
  History       :
  1.Date        : 2013/02/28
    Author      : zgjie
    Modification: Created file

 *******************************************************************************/

#ifndef  __HI_AUDSP_AEE__H__
#define  __HI_AUDSP_AEE__H__

#include "hi_type.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct
{
    volatile HI_U32 Reg0;
    volatile HI_U32 Reg1;
    volatile HI_U32 Reg2;
    volatile HI_U32 Reg3;

    // todo
} HI_AEE_REGS_S;


/** @}*/  /** <!-- ==== API Declaration End ====*/

#ifdef __cplusplus
}
#endif

#endif

