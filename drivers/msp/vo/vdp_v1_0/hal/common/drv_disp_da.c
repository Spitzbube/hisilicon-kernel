
/******************************************************************************
  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.
******************************************************************************
File Name     : drv_disp_da.h
Version       : Initial Draft
Author        : Hisilicon multimedia software group
Created       : 2012/12/30
Last Modified :
Description   :
Function List :
History       :
******************************************************************************/

#include "drv_disp_com.h"
#include "drv_disp_da.h"

#ifdef __cplusplus
 #if __cplusplus
extern "C" {
 #endif
#endif /* __cplusplus */


typedef struct
{
	int fill_0[14]; //0
	//56
} Struct_81141b14;


//static DISP_ALG_VERSION_S s_stAlgCscVersion = {0};
HI_BOOL g_bDAInitFlag = HI_FALSE;
DISP_DA_FUNCTION_S g_stDAFuntion;
Struct_81141b14 Data_81141b14; //81141b14
Struct_81141acc Data_81141acc; //81141acc -> 81141B04?


HI_S32 DISP_GAMMA_Init(HI_VOID)
{
	if (HI_SUCCESS != HI_DRV_MMZ_AllocAndMap("DISP_Gamma_RGB", HI_NULL, 4992, 0, &Data_81141acc.Data_0))
	{
		DISP_FATAL("Get gmm_buf_RGB failed.\n"); //41
        return HI_FAILURE;
	}



	if (HI_SUCCESS != HI_DRV_MMZ_AllocAndMap("DISP_Gamma_YUV", HI_NULL, 4992, 0, &Data_81141acc.Data_12))
	{
		DISP_FATAL("Get gmm_buf_YUV failed.\n"); //49
		HI_DRV_MMZ_UnmapAndRelease(&Data_81141acc.Data_0);
        return HI_FAILURE;
	}



	if (HI_SUCCESS != ALG_GmmInit(&Data_81141acc))
	{
		HI_DRV_MMZ_UnmapAndRelease(&Data_81141acc.Data_0);
		HI_DRV_MMZ_UnmapAndRelease(&Data_81141acc.Data_12);
		DISP_FATAL("Init Gamma failed.\n"); //60
        return HI_FAILURE;
	}

	return HI_SUCCESS;
}

HI_S32 DISP_GAMMA_DeInit(HI_VOID)
{
	HI_DRV_MMZ_UnmapAndRelease(&Data_81141acc.Data_0);
	HI_DRV_MMZ_UnmapAndRelease(&Data_81141acc.Data_12);

	return HI_SUCCESS;
}

static HI_VOID SetGammaCtrl(int a, int b)
{
	ALG_GmmThdSet(&Data_81141acc, a, b);
}


static HI_VOID UpdateGamma(Struct_8113ea8c* a, HI_U32 b)
{
	UpdateGmmCoef(&Data_81141acc, a, b);
}


HI_S32 DISP_DA_Init(HI_DRV_DISP_VERSION_S *pstVersion)
{
    //HI_S32 nRet;

    if (!pstVersion)
    {
        DISP_ERROR("FUNC(%s) Error! Invalid input parameters!\n", __FUNCTION__); //98
        return HI_FAILURE;
    }

    if (g_bDAInitFlag)
    {
        DISP_INFO("FUNC(%s) inited!\n", __FUNCTION__); //104
        return HI_SUCCESS;
    }

    DISP_MEMSET(&g_stDAFuntion, 0, sizeof(g_stDAFuntion));
    DISP_MEMSET(&Data_81141b14, 0, sizeof(Struct_81141b14));

    if (   (pstVersion->u32VersionPartH == DISP_CV200_ES_VERSION_H)
        && (pstVersion->u32VersionPartL == DISP_CV200_ES_VERSION_L)
        )
    {
        // ����ָ�븳ֵ
        g_stDAFuntion.PFCscRgb2Yuv   = DISP_ALG_CscRgb2Yuv;
        //g_stDAFuntion.pfCalcCscCoef  = DISP_ALG_CscCoefSet;
        g_stDAFuntion.Func_4 = UpdateGamma;
        g_stDAFuntion.Func_8 = SetGammaCtrl;

    }
    else
    {
        DISP_ERROR("FUNC(%s) Error! Invalid display version!", __FUNCTION__); //126
        return HI_FAILURE;
    }
    
    if (HI_SUCCESS != DISP_GAMMA_Init())
    {
        return HI_FAILURE;
    }

	g_bDAInitFlag = HI_TRUE;

    return HI_SUCCESS;
}


HI_S32 DISP_DA_DeInit(HI_VOID)
{
    if (g_bDAInitFlag)
    {
        DISP_MEMSET(&g_stDAFuntion, 0, sizeof(g_stDAFuntion));

        g_bDAInitFlag = HI_FALSE;

        DISP_GAMMA_DeInit();
    }
    return HI_SUCCESS;    
}

DISP_DA_FUNCTION_S * DISP_DA_GetFunction(HI_VOID)
{
    if (g_bDAInitFlag)
    {
        return &g_stDAFuntion;  
    }
    else
    {
        DISP_FATAL("UA is not inited! DISP_UA_GetFunction return NULL!\n"); //168
        return HI_NULL;
    }
}










#ifdef __cplusplus
 #if __cplusplus
}
 #endif
#endif /* __cplusplus */









