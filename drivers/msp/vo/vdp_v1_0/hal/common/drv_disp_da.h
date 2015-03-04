
/******************************************************************************
  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.
******************************************************************************
File Name     : drv_disp_x.h
Version       : Initial Draft
Author        : Hisilicon multimedia software group
Created       : 2012/12/30
Last Modified :
Description   :
Function List :
History       :
******************************************************************************/
#ifndef __DRV_DISP_DA_H__
#define __DRV_DISP_DA_H__

#include "hi_type.h"
#include "drv_disp_osal.h"
#include "drv_disp_version.h"
#include "drv_disp_alg_struct.h"
#include "drv_disp_alg_rgb2yuv.h"
#include "drv_disp_alg_csc.h"

#ifdef __cplusplus
 #if __cplusplus
extern "C" {
 #endif
#endif /* __cplusplus */



typedef struct tagDISP_DA_FUNCTION_S
{
    /******************* CSC ALG group  *********************/
    HI_S32 (*DISP_GetCSCAlgVersion)(DISP_ALG_VERSION_S *pstVer);
    HI_VOID (* pfCalcCscCoef)(ALG_CSC_DRV_PARA_S *pstCscDrvPara, ALG_CSC_RTL_PARA_S *pstCscRtlPara);

    HI_VOID (* PFCscRgb2Yuv)(ALG_COLOR_S *pstRgbColor, ALG_COLOR_S *pYuvColor);

}DISP_DA_FUNCTION_S;

HI_S32 DISP_DA_Init(HI_DRV_DISP_VERSION_S *pstVersion);
HI_S32 DISP_DA_DeInit(HI_VOID);

DISP_DA_FUNCTION_S *DISP_DA_GetFunction(HI_VOID);










#ifdef __cplusplus
 #if __cplusplus
}
 #endif
#endif /* __cplusplus */

#endif /*  __DRV_DISP_X_H__  */










