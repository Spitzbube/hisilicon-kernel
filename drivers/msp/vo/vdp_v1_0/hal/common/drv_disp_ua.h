
/******************************************************************************
  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.
******************************************************************************
File Name     : drv_disp_ua.h
Version       : Initial Draft
Author        : Hisilicon multimedia software group
Created       : 2012/12/30
Last Modified :
Description   :
Function List :
History       :
******************************************************************************/
#ifndef __DRV_DISP_UA_H__
#define __DRV_DISP_UA_H__

#include "hi_type.h"
#include "drv_disp_osal.h"
#include "drv_disp_version.h"
#include "drv_disp_alg_struct.h"
#include "drv_disp_alg_zme.h"
#include "drv_disp_alg_csc.h"


#ifdef __cplusplus
 #if __cplusplus
extern "C" {
 #endif
#endif /* __cplusplus */



typedef struct tagDISP_UA_FUNCTION_S
{
    /******************* CSC ALG group  *********************/
    //HI_S32 (*DISP_GetCSCAlgVersion)(DISP_ALG_VERSION_S *pstVer);
    HI_VOID (* pfCalcCscCoef)(ALG_CSC_DRV_PARA_S *pstCscDrvPara, ALG_CSC_RTL_PARA_S *pstCscRtlPara);

    HI_VOID (* pfVZmeVdpHQSet)(ALG_VZME_DRV_PARA_S *pstZmeDrvPara, ALG_VZME_RTL_PARA_S *pstZmeRtlPara);
    HI_VOID (* pfVZmeVdpSQSet)(ALG_VZME_DRV_PARA_S *pstZmeDrvPara, ALG_VZME_RTL_PARA_S *pstZmeRtlPara);
    HI_VOID (* pfVZmeVdpSQSetSeperateAddr)(ALG_VZME_DRV_PARA_S *pstZmeDrvPara, ALG_VZME_RTL_PARA_S *pstZmeRtlPara);
}DISP_UA_FUNCTION_S;

HI_S32 DISP_UA_Init(HI_DRV_DISP_VERSION_S *pstVersion);
HI_S32 DISP_UA_DeInit(HI_VOID);

DISP_UA_FUNCTION_S *DISP_UA_GetFunction(HI_VOID);




#ifdef __cplusplus
 #if __cplusplus
}
 #endif
#endif /* __cplusplus */

#endif /*  __DRV_DISP_UA_H__  */










