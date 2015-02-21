#include "hi_module.h"
#include "hi_drv_module.h"
#include "drv_tde_ext.h"
#include "hifb_drv.h"
#include "hifb_comm.h"


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

static IntCallBack s_pTdeCallBack;
/* tde support which color format */
static HI_BOOL s_bTdeColFmt[HIFB_FMT_BUTT] = {
    HI_TRUE,    /* HIFB_FMT_RGB565 */ 
    HI_TRUE,    /* HIFB_FMT_RGB888 */
    HI_TRUE,    /* HIFB_FMT_KRGB444 */
    HI_TRUE,    /* HIFB_FMT_KRGB555 */
    HI_FALSE,   /* HIFB_FMT_KRGB888 */
    HI_TRUE,    /* HIFB_FMT_ARGB4444 */
    HI_TRUE,    /* HIFB_FMT_ARGB1555 */
    HI_TRUE,    /* HIFB_FMT_ARGB8888 */
    HI_TRUE,    /* HIFB_FMT_ARGB8565 */
    HI_TRUE,   /* HIFB_FMT_RGBA4444 */
    HI_TRUE,   /* HIFB_FMT_RGBA5551 */
    HI_TRUE,   /* HIFB_FMT_RGBA5658 */
    HI_TRUE,   /* HIFB_FMT_RGBA8888 */
    
    HI_TRUE,   /**< BGR565 */   
    HI_TRUE,   /**< BGR888 */   
    HI_TRUE,   /**< ABGR4444 */   
    HI_TRUE,   /**< ABGR1555 */   
    HI_TRUE,   /**< ABGR8888 */   
    HI_TRUE,   /**< ABGR8565 */      
    HI_TRUE,   /**< BGR444 16bpp */
    HI_TRUE,   /**< BGR555 16bpp */
    HI_TRUE,   /**< BGR888 32bpp */
    
    HI_TRUE,    /* HIFB_FMT_1BPP */
    HI_TRUE,    /* HIFB_FMT_2BPP */
    HI_TRUE,    /* HIFB_FMT_4BPP */
    HI_TRUE,    /* HIFB_FMT_8BPP */
    HI_TRUE,    /* HIFB_FMT_ACLUT44 */
    HI_TRUE,    /* HIFB_FMT_ACLUT88 */    
};

static TDE_EXPORT_FUNC_S *ps_TdeExportFuncs = HI_NULL;

HI_S32 HIFB_DRV_TdeOpen(HI_VOID)
{
    if(HI_NULL != ps_TdeExportFuncs)
    {
        return HI_SUCCESS;
    }
	   
    if (HI_SUCCESS != HI_DRV_MODULE_GetFunction(HI_ID_TDE, (HI_VOID**)&ps_TdeExportFuncs))
    {
    	return HI_FAILURE;
    }

    if(HI_NULL == ps_TdeExportFuncs)
    {
        HIFB_ERROR("Tde is not available!\n");
        return HI_FAILURE;
    }

    
    return HI_SUCCESS;
}

HI_S32 HIFB_DRV_TdeClose(HI_VOID)
{
	ps_TdeExportFuncs = HI_NULL;
    return HI_SUCCESS;
}

HI_S32 HIFB_DRV_TdeSupportFmt(HIFB_COLOR_FMT_E fmt)
{
	if(HIFB_FMT_BUTT <= fmt)
	{
		return HI_FALSE;
	}
	return s_bTdeColFmt[fmt];
}
TDE2_COLOR_FMT_E HIFB_DRV_ConvFmt(HIFB_COLOR_FMT_E Fmt)
{
    switch (Fmt)
    {
        case HIFB_FMT_RGB565:
            return TDE2_COLOR_FMT_RGB565;
        case HIFB_FMT_RGB888:
            return TDE2_COLOR_FMT_RGB888;
        case HIFB_FMT_KRGB444:
            return TDE2_COLOR_FMT_RGB444;
        case HIFB_FMT_KRGB555:
            return TDE2_COLOR_FMT_RGB555;
        /*HIFB_FMT_KRGB888 32bpp;TDE2_COLOR_FMT_RGB888:24bpp.so transform HIFB_FMT_KRGB888 to 
        TDE2_COLOR_FMT_ARGB8888 ,but ignore alpha data in process*/
        case HIFB_FMT_KRGB888:
            return TDE2_COLOR_FMT_ARGB8888;
        case HIFB_FMT_ARGB4444:
            return TDE2_COLOR_FMT_ARGB4444;
        case HIFB_FMT_ARGB1555:
            return TDE2_COLOR_FMT_ARGB1555;
        case HIFB_FMT_ARGB8888:
            return TDE2_COLOR_FMT_ARGB8888;
        case HIFB_FMT_ARGB8565:
            return TDE2_COLOR_FMT_ARGB8565;
            
        case HIFB_FMT_BGR565: 
            return TDE2_COLOR_FMT_BGR565;           
        case HIFB_FMT_ABGR1555:
            return TDE2_COLOR_FMT_ABGR1555;  
        case HIFB_FMT_ABGR4444:
            return TDE2_COLOR_FMT_ABGR4444;  
        case HIFB_FMT_KBGR444:
            return TDE2_COLOR_FMT_BGR444;  
        case HIFB_FMT_KBGR555:
            return TDE2_COLOR_FMT_BGR555;  
        case HIFB_FMT_BGR888: 
            return TDE2_COLOR_FMT_BGR888;    
        case HIFB_FMT_ABGR8888:
            return TDE2_COLOR_FMT_ABGR8888;  
        case HIFB_FMT_ABGR8565:
            return TDE2_COLOR_FMT_ABGR8565;  
        case HIFB_FMT_KBGR888:  
            return TDE2_COLOR_FMT_ABGR8888;  
            
        case HIFB_FMT_1BPP:
            return TDE2_COLOR_FMT_CLUT1;
        case HIFB_FMT_2BPP:
            return TDE2_COLOR_FMT_CLUT2;
        case HIFB_FMT_4BPP:
            return TDE2_COLOR_FMT_CLUT4;
        case HIFB_FMT_8BPP:
            return TDE2_COLOR_FMT_CLUT8;
        case HIFB_FMT_ACLUT44:
            return TDE2_COLOR_FMT_ACLUT44;
        case HIFB_FMT_ACLUT88:
            return TDE2_COLOR_FMT_ACLUT88;
        default:
            return TDE2_COLOR_FMT_BUTT;
            
    }
}

HI_BOOL HIFB_IsTdeColorFmtClut(TDE2_COLOR_FMT_E enColorFmt)
{
	if (enColorFmt >= TDE2_COLOR_FMT_CLUT1
		&& enColorFmt <= TDE2_COLOR_FMT_ACLUT88)
	{
		return HI_TRUE;
	}
	
	return HI_FALSE;
}

HI_S32 HIFB_DRV_Blit(HIFB_BUFFER_S *pSrcImg, HIFB_BUFFER_S *pDstImg,  HIFB_BLIT_OPT_S *pstOpt, HI_BOOL bRefreshScreen)
{
    HI_S32 s32Ret;
    TDE2_SURFACE_S stSrcSur = {0};
    TDE2_SURFACE_S stDstSur = {0};
    TDE2_RECT_S stSrcRect;
    TDE2_RECT_S stDstRect;
    TDE_HANDLE handle;
    TDE2_OPT_S stOpt = {0};    
    TDE_DEFLICKER_LEVEL_E enTdeDflkLevel;

    if(NULL == ps_TdeExportFuncs)
    {
        HIFB_ERROR("Tde is not available!\n");
        return HI_FAILURE;
    }

    /** confing src*/
    stSrcSur.u32PhyAddr = pSrcImg->stCanvas.u32PhyAddr;
    stSrcSur.u32Width = pSrcImg->stCanvas.u32Width;
    stSrcSur.u32Height = pSrcImg->stCanvas.u32Height;
    stSrcSur.u32Stride = pSrcImg->stCanvas.u32Pitch;
    stSrcSur.bAlphaMax255 = HI_TRUE;
    stSrcSur.bYCbCrClut = HI_FALSE;
    stSrcSur.enColorFmt = HIFB_DRV_ConvFmt(pSrcImg->stCanvas.enFmt);
    stSrcSur.u8Alpha0 = pstOpt->stAlpha.u8Alpha0;
    stSrcSur.u8Alpha1 = pstOpt->stAlpha.u8Alpha1;
    if (!((stSrcSur.u8Alpha0 == 0) && (stSrcSur.u8Alpha1 == 0)))
    {
        stSrcSur.bAlphaExt1555 = HI_TRUE;
    }

    stSrcRect.s32Xpos = pSrcImg->UpdateRect.x;
    stSrcRect.s32Ypos = pSrcImg->UpdateRect.y;
    stSrcRect.u32Width = pSrcImg->UpdateRect.w;
    stSrcRect.u32Height = pSrcImg->UpdateRect.h;

    /** confing dst*/
    stDstSur.u32PhyAddr = pDstImg->stCanvas.u32PhyAddr;
    stDstSur.u32Width = pDstImg->stCanvas.u32Width;
    stDstSur.u32Height = pDstImg->stCanvas.u32Height;
    stDstSur.u32Stride = pDstImg->stCanvas.u32Pitch;
    stDstSur.bAlphaMax255 = HI_TRUE;
    stDstSur.bYCbCrClut = HI_FALSE;
    stDstSur.enColorFmt = HIFB_DRV_ConvFmt(pDstImg->stCanvas.enFmt);
    stDstSur.u8Alpha0 = pstOpt->stAlpha.u8Alpha0;
    stDstSur.u8Alpha1 = pstOpt->stAlpha.u8Alpha1;

    stDstRect.s32Xpos = pDstImg->UpdateRect.x;
    stDstRect.s32Ypos = pDstImg->UpdateRect.y;
    stDstRect.u32Width = pDstImg->UpdateRect.w;
    stDstRect.u32Height = pDstImg->UpdateRect.h;


    stOpt.bResize = pstOpt->bScale;
    //if (((HI_U32)stSrcSur.enColorFmt >= (HI_U32)HIFB_FMT_1BPP) && ((HI_U32)stSrcSur.enColorFmt <= (HI_U32)HIFB_FMT_ACLUT88))
    if (HIFB_IsTdeColorFmtClut(stSrcSur.enColorFmt))
    {
        stOpt.bClutReload = HI_TRUE;
        stSrcSur.pu8ClutPhyAddr = (HI_U8 *)pstOpt->u32CmapAddr;
        stDstSur.pu8ClutPhyAddr = (HI_U8 *)pstOpt->u32CmapAddr;
    }

    switch(pstOpt->enAntiflickerLevel)
    {
        case HIFB_LAYER_ANTIFLICKER_NONE:
        {
            stOpt.enDeflickerMode = TDE2_DEFLICKER_MODE_NONE;
            enTdeDflkLevel = TDE_DEFLICKER_BUTT;
            break;
        }
        case HIFB_LAYER_ANTIFLICKER_LOW:
        {
            stOpt.enDeflickerMode = TDE2_DEFLICKER_MODE_BOTH;
            enTdeDflkLevel = TDE_DEFLICKER_LOW;
            break;
        }
        case HIFB_LAYER_ANTIFLICKER_MIDDLE:
        {
            stOpt.enDeflickerMode = TDE2_DEFLICKER_MODE_BOTH;
            enTdeDflkLevel = TDE_DEFLICKER_MIDDLE;
            break;
        }
        case HIFB_LAYER_ANTIFLICKER_HIGH:
        {
            stOpt.enDeflickerMode = TDE2_DEFLICKER_MODE_BOTH;
            enTdeDflkLevel = TDE_DEFLICKER_HIGH;
            break;
        }
        case HIFB_LAYER_ANTIFLICKER_AUTO:
        {
            stOpt.enDeflickerMode = TDE2_DEFLICKER_MODE_BOTH;
            enTdeDflkLevel = TDE_DEFLICKER_AUTO;
            break;
        }
        default:
		{
			stOpt.enDeflickerMode = TDE2_DEFLICKER_MODE_NONE;
			enTdeDflkLevel = TDE_DEFLICKER_BUTT;
            break;
		}

    }

    if(TDE_DEFLICKER_BUTT != enTdeDflkLevel)
    {
        ps_TdeExportFuncs->pfnTdeSetDeflickerLevel(enTdeDflkLevel);
    }

    if (pstOpt->stCKey.bKeyEnable)
    {
        //if (((HIFB_COLOR_FMT_E)(stSrcSur.enColorFmt) >= HIFB_FMT_1BPP) && ((HIFB_COLOR_FMT_E)(stSrcSur.enColorFmt) <= HIFB_FMT_ACLUT88))
		if (HIFB_IsTdeColorFmtClut(stSrcSur.enColorFmt))
        {
            stOpt.enColorKeyMode = TDE2_COLORKEY_MODE_FOREGROUND;
            stOpt.unColorKeyValue.struCkClut.stAlpha.bCompIgnore = HI_TRUE;
            stOpt.unColorKeyValue.struCkClut.stClut.bCompOut = pstOpt->stCKey.u32KeyMode;
            stOpt.unColorKeyValue.struCkClut.stClut.u8CompMax = pstOpt->stCKey.u8BlueMax;
            stOpt.unColorKeyValue.struCkClut.stClut.u8CompMin = pstOpt->stCKey.u8BlueMin;
            stOpt.unColorKeyValue.struCkClut.stClut.u8CompMask = 0xff;
        }
        else
        {
            stOpt.enColorKeyMode = TDE2_COLORKEY_MODE_FOREGROUND;
            stOpt.unColorKeyValue.struCkARGB.stAlpha.bCompIgnore = HI_TRUE;
            stOpt.unColorKeyValue.struCkARGB.stRed.u8CompMax = pstOpt->stCKey.u8RedMax;
            stOpt.unColorKeyValue.struCkARGB.stRed.u8CompMin = pstOpt->stCKey.u8RedMin;
            stOpt.unColorKeyValue.struCkARGB.stRed.bCompOut = pstOpt->stCKey.u32KeyMode;
            stOpt.unColorKeyValue.struCkARGB.stRed.u8CompMask = 0xff;
            
            stOpt.unColorKeyValue.struCkARGB.stGreen.u8CompMax = pstOpt->stCKey.u8GreenMax;
            stOpt.unColorKeyValue.struCkARGB.stGreen.u8CompMin = pstOpt->stCKey.u8GreenMin;
            stOpt.unColorKeyValue.struCkARGB.stGreen.bCompOut = pstOpt->stCKey.u32KeyMode;
            stOpt.unColorKeyValue.struCkARGB.stGreen.u8CompMask = 0xff;
            
            stOpt.unColorKeyValue.struCkARGB.stBlue.u8CompMax = pstOpt->stCKey.u8BlueMax;
            stOpt.unColorKeyValue.struCkARGB.stBlue.u8CompMin = pstOpt->stCKey.u8BlueMin;
            stOpt.unColorKeyValue.struCkARGB.stBlue.bCompOut = pstOpt->stCKey.u32KeyMode;
            stOpt.unColorKeyValue.struCkARGB.stBlue.u8CompMask = 0xff;
        }
    }
    stOpt.u8GlobalAlpha = 255;
    if (pstOpt->stAlpha.bAlphaEnable)
    {
        stOpt.enAluCmd = TDE2_ALUCMD_BLEND;
        stOpt.u8GlobalAlpha = pstOpt->stAlpha.u8GlobalAlpha;
        stOpt.enOutAlphaFrom = TDE2_OUTALPHA_FROM_NORM;
        stOpt.stBlendOpt.bGlobalAlphaEnable = HI_TRUE;
        stOpt.stBlendOpt.bPixelAlphaEnable = HI_TRUE;
        stOpt.stBlendOpt.bSrc1AlphaPremulti = HI_TRUE;
        stOpt.stBlendOpt.bSrc2AlphaPremulti = HI_TRUE;
        stOpt.stBlendOpt.eBlendCmd = TDE2_BLENDCMD_SRCOVER;

    }
    else
    {
        stOpt.enOutAlphaFrom = TDE2_OUTALPHA_FROM_FOREGROUND;
    }

    s32Ret = ps_TdeExportFuncs->pfnTdeEnableRegionDeflicker(pstOpt->bRegionDeflicker);
    if (s32Ret != HI_SUCCESS)
    {
        HIFB_ERROR("enable region deflicker failed!\n");
        return s32Ret;
    }

    if (pstOpt->stClip.bClip)
    {
        stOpt.enClipMode = pstOpt->stClip.bInRegionClip ? TDE2_CLIPMODE_INSIDE : TDE2_CLIPMODE_OUTSIDE;
        stOpt.stClipRect.s32Xpos = pstOpt->stClip.stClipRect.x;
        stOpt.stClipRect.s32Ypos = pstOpt->stClip.stClipRect.y;
        stOpt.stClipRect.u32Width = pstOpt->stClip.stClipRect.w;
        stOpt.stClipRect.u32Height = pstOpt->stClip.stClipRect.h;
    }

    s32Ret = ps_TdeExportFuncs->pfnTdeBeginJob(&handle);
    if(s32Ret != HI_SUCCESS)
    {
        HIFB_ERROR("begin job failed\n");
        return s32Ret;
    }


    //printk("%d, sw:%d sh:%d sstride %d dw:%d dh:%d dstride %d \n" ,__LINE__, stSrcSur.u32Width, stSrcSur.u32Height, stSrcSur.u32Stride,stDstSur.u32Width, stDstSur.u32Height,stDstSur.u32Stride);
    //printk("%d, srcx %d srcy %d srcw %d srch %d dstx %d dsty %d dstw %d dsth %d\n",__LINE__,stSrcRect.s32Xpos,stSrcRect.s32Ypos,stSrcRect.u32Width,stSrcRect.u32Height,stDstRect.s32Xpos,stDstRect.s32Ypos,stDstRect.u32Width,stDstRect.u32Height);
    //printk("+++++++srcAddr 0x%x DstAddr 0x%x \n",stSrcSur.pu8ClutPhyAddr,stDstSur.pu8ClutPhyAddr);

    s32Ret = ps_TdeExportFuncs->pfnTdeBlit(handle, &stDstSur, &stDstRect, &stSrcSur, &stSrcRect, &stDstSur, \
                     &stDstRect, &stOpt);
    if(s32Ret != HI_SUCCESS)
    {
        HIFB_ERROR("tde blit failed\n");    
        ps_TdeExportFuncs->pfnTdeCancelJob(handle);
        return s32Ret;
    }

    if (pstOpt->bCallBack)
    {
        s32Ret = ps_TdeExportFuncs->pfnTdeEndJob(handle, pstOpt->bBlock, 100, HI_FALSE, \
            (TDE_FUNC_CB)pstOpt->pfnCallBack, pstOpt->pParam);
    }
    else
    {
        s32Ret = ps_TdeExportFuncs->pfnTdeEndJob(handle, pstOpt->bBlock, 100, HI_FALSE, \
            HI_NULL, HI_NULL);
    }
    if(s32Ret != HI_SUCCESS)
    {
        HIFB_ERROR("end job failed\n"); 
        ps_TdeExportFuncs->pfnTdeCancelJob(handle);
        return s32Ret;
    }
    
    if (pstOpt->bRegionDeflicker)
    {
        s32Ret = ps_TdeExportFuncs->pfnTdeEnableRegionDeflicker(HI_FALSE);
        if (s32Ret != HI_SUCCESS)
        {
            HIFB_ERROR("disable region deflicker failed!\n");
            return s32Ret;
        }
    } 
    return handle;
}

HI_S32 HIFB_DRV_SetTdeCallBack(IntCallBack pTdeCallBack)
{
   s_pTdeCallBack = pTdeCallBack;
   return HI_SUCCESS;    
}


HI_S32 HIFB_DRV_ClearRect(HIFB_SURFACE_S* pDstImg, HIFB_BLIT_OPT_S* pstOpt)
{
    HI_S32 s32Ret;
    TDE2_SURFACE_S TDESurface = {0};
    TDE2_RECT_S Rect;
    TDE_HANDLE s32Handle;

    if(NULL == ps_TdeExportFuncs)
    {
        HIFB_ERROR("Tde is not available!\n");
        return HI_FAILURE;
    }
    
    /** confing dst*/
    TDESurface.u32PhyAddr = pDstImg->u32PhyAddr;
    TDESurface.u32Width = pDstImg->u32Width;
    TDESurface.u32Height = pDstImg->u32Height;
    TDESurface.u32Stride = pDstImg->u32Pitch;
    TDESurface.bAlphaMax255 = HI_TRUE;
    TDESurface.bYCbCrClut = HI_FALSE;
    TDESurface.enColorFmt = HIFB_DRV_ConvFmt(pDstImg->enFmt);
    TDESurface.u8Alpha0 = pstOpt->stAlpha.u8Alpha0;
    TDESurface.u8Alpha1 = pstOpt->stAlpha.u8Alpha1;

    Rect.s32Xpos = 0;
    Rect.s32Ypos = 0;
    Rect.u32Width = pDstImg->u32Width;
    Rect.u32Height = pDstImg->u32Height;

    s32Ret = ps_TdeExportFuncs->pfnTdeBeginJob(&s32Handle);
    if(s32Ret != HI_SUCCESS)
    {
        HIFB_ERROR("begin job failed\n");
        return s32Ret;
    }

    //printk("line:%d, phyaddr:0x%x, w:%d, h:%d, stride:%d\n", __LINE__, TDESurface.u32PhyAddr, TDESurface.u32Width, TDESurface.u32Height, TDESurface.u32Stride);

    s32Ret = ps_TdeExportFuncs->pfnTdeQuickFill(s32Handle, &TDESurface, &Rect, 0x0);
    if(s32Ret != HI_SUCCESS)
    {
        HIFB_ERROR("tde quick fill failed s32Ret:0x%x\n", s32Ret);    
        ps_TdeExportFuncs->pfnTdeCancelJob(s32Handle);
        return s32Ret;
    }

    s32Ret = ps_TdeExportFuncs->pfnTdeEndJob(s32Handle, pstOpt->bBlock, 100, HI_FALSE, HI_NULL, HI_NULL);
    if(s32Ret != HI_SUCCESS)
    {
        HIFB_ERROR("end job failed ret:%d\n", s32Ret); 
        ps_TdeExportFuncs->pfnTdeCancelJob(s32Handle);
        return s32Ret;
    }
    
    return s32Handle;
}

HI_VOID HIFB_DRV_WaitAllTdeDone(HI_BOOL bSync)
{
    if(NULL == ps_TdeExportFuncs)
    {
        HIFB_ERROR("Tde is not available!\n");
        //return -1;
        return;
    }
    ps_TdeExportFuncs->pfnTdeWaitAllDone(HI_FALSE);
}


HI_S32 HIFB_DRV_CalScaleRect(const TDE2_RECT_S* pstSrcRect, const TDE2_RECT_S* pstDstRect,
                                TDE2_RECT_S* pstRectInSrc, TDE2_RECT_S* pstRectInDst)
{
    if(NULL == ps_TdeExportFuncs)
    {
        HIFB_ERROR("Tde is not available!\n");
        return HI_FAILURE;
    }
    return ps_TdeExportFuncs->pfnTdeCalScaleRect(pstSrcRect, pstDstRect, pstRectInSrc, pstRectInDst);
}

HI_S32 HIFB_DRV_WaitForDone(TDE_HANDLE s32Handle, HI_U32 u32TimeOut)
{
    if(NULL == ps_TdeExportFuncs)
    {
        HIFB_ERROR("Tde is not available!\n");
        return HI_FAILURE;
    }
    return ps_TdeExportFuncs->pfnTdeWaitForDone(s32Handle, u32TimeOut);
}

HI_VOID HIFB_DRV_GetTdeOps(HIFB_DRV_TDEOPS_S *Ops)
{
	Ops->HIFB_DRV_Blit           = HIFB_DRV_Blit;
	Ops->HIFB_DRV_CalScaleRect   = HIFB_DRV_CalScaleRect;
	Ops->HIFB_DRV_ClearRect      = HIFB_DRV_ClearRect;
	Ops->HIFB_DRV_SetTdeCallBack = HIFB_DRV_SetTdeCallBack;
	Ops->HIFB_DRV_TdeClose       = HIFB_DRV_TdeClose;
	Ops->HIFB_DRV_TdeOpen        = HIFB_DRV_TdeOpen;
	Ops->HIFB_DRV_TdeSupportFmt  = HIFB_DRV_TdeSupportFmt;
	Ops->HIFB_DRV_WaitAllTdeDone = HIFB_DRV_WaitAllTdeDone;
	Ops->HIFB_DRV_WaitForDone    = HIFB_DRV_WaitForDone;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

