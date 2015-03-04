
/******************************************************************************
  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.
******************************************************************************
File Name     : drv_disp_ua.c
Version       : Initial Draft
Author        : Hisilicon multimedia software group
Created       : 2012/12/30
Last Modified :
Description   :
Function List :
History       :
******************************************************************************/


#include "drv_disp_com.h"
#include "drv_disp_ua.h"


#ifdef __cplusplus
 #if __cplusplus
extern "C" {
 #endif
#endif /* __cplusplus */

//static DISP_ALG_VERSION_S s_stAlgCscVersion = {0};
HI_BOOL g_bUAInitFlag = HI_FALSE;
DISP_UA_FUNCTION_S g_stUAFuntion;
ALG_VZME_MEM_S g_stVZMEInstance;

HI_VOID UA_VZmeVdpHQSet(ALG_VZME_DRV_PARA_S *pstZmeDrvPara, ALG_VZME_RTL_PARA_S *pstZmeRtlPara)
{
    ALG_VZmeVdpHQSet(&g_stVZMEInstance, pstZmeDrvPara, pstZmeRtlPara);

    return;
}


HI_VOID UA_VZmeVdpSQSet(ALG_VZME_DRV_PARA_S *pstZmeDrvPara, ALG_VZME_RTL_PARA_S *pstZmeRtlPara)
{
    ALG_VZmeVdpSQSet(&g_stVZMEInstance, pstZmeDrvPara, pstZmeRtlPara);
    return;
}

HI_VOID UA_VZmeVdpSQSetSeparateAddr(ALG_VZME_DRV_PARA_S *pstZmeDrvPara, ALG_VZME_RTL_PARA_S *pstZmeRtlPara)
{
    ALG_VZmeVdpSQSetSptAddr(&g_stVZMEInstance, pstZmeDrvPara, pstZmeRtlPara);
    return;
}

HI_S32 DISP_UA_Init(HI_DRV_DISP_VERSION_S *pstVersion)
{
    HI_S32 nRet;

    if (!pstVersion)
    {
        DISP_ERROR("FUNC(%s) Error! Invalid input parameters!\n", __FUNCTION__);
        return HI_FAILURE;
    }

    if (g_bUAInitFlag)
    {
        DISP_INFO("FUNC(%s) inited!\n", __FUNCTION__);
        return HI_SUCCESS;
    }

    DISP_MEMSET(&g_stUAFuntion, 0, sizeof(g_stUAFuntion));

    if (   (pstVersion->u32VersionPartH == DISP_CV200_ES_VERSION_H)
        && (pstVersion->u32VersionPartL == DISP_CV200_ES_VERSION_L)
        )
    {
        // 准备工作，包括资源申请
        nRet = ALG_VZmeVdpComnInit(&g_stVZMEInstance);
        if (nRet)
        {
            DISP_ERROR("ALG_VZmeVdpComnInit failed!\n");
            return HI_FAILURE;
        }

        // 函数指针赋值
        g_stUAFuntion.pfCalcCscCoef  = DISP_ALG_CscCoefSet;

        g_stUAFuntion.pfVZmeVdpHQSet = UA_VZmeVdpHQSet;
        g_stUAFuntion.pfVZmeVdpSQSet = UA_VZmeVdpSQSet;
        g_stUAFuntion.pfVZmeVdpSQSetSeperateAddr = UA_VZmeVdpSQSetSeparateAddr;
    }
    else
    {
        DISP_ERROR("FUNC(%s) Error! Invalid display version!", __FUNCTION__);
        return HI_FAILURE;
    }
    

    g_bUAInitFlag = HI_TRUE;

    return HI_SUCCESS;
}


HI_S32 DISP_UA_DeInit(HI_VOID)
{
    if (g_bUAInitFlag)
    {
        // 释放资源
        ALG_VZmeVdpComnDeInit(&g_stVZMEInstance);

        DISP_MEMSET(&g_stUAFuntion, 0, sizeof(g_stUAFuntion));

        g_bUAInitFlag = HI_FALSE;
    }
    return HI_SUCCESS;    
}

DISP_UA_FUNCTION_S * DISP_UA_GetFunction(HI_VOID)
{
    if (g_bUAInitFlag)
    {
        return &g_stUAFuntion;  
    }
    else
    {
        DISP_FATAL("UA is not inited! DISP_UA_GetFunction return NULL!\n");
        return HI_NULL;
    }
}


#ifdef __cplusplus
 #if __cplusplus
}
 #endif
#endif /* __cplusplus */












