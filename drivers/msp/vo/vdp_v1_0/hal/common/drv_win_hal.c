
/******************************************************************************
  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.
******************************************************************************
File Name     : drv_win_hal.c
Version       : Initial Draft
Author        : Hisilicon multimedia software  group
Created       : 2012/12/30
Last Modified :
Description   :
Function List :
History       :
******************************************************************************/

#include "drv_win_hal.h"
#include "drv_disp_ua.h"
#include "drv_disp_da.h"
#include "drv_win_priv.h"
#include "drv_win_hal_adp.h"

#ifdef __cplusplus
 #if __cplusplus
extern "C" {
 #endif
#endif /* __cplusplus */

static HI_S32 s_bVideoSurfaceFlag = -1;
static VIDEO_LAYER_FUNCTIONG_S s_stVieoLayerFunc;
static VIDEO_LAYER_S s_stVideoLayer[DEF_VIDEO_LAYER_MAX_NUMBER];
static VIDEO_LAYER_CAPABILITY_S s_stVideoLayerCap[DEF_VIDEO_LAYER_MAX_NUMBER];

static HI_S32 HAL_SetDispMode(HI_U32 u32id, HI_DRV_DISP_STEREO_MODE_E eMode)
{
    return Chip_Specific_SetDispMode(u32id, eMode);    
}

HI_S32 GetCapability(HI_U32 eLayer, VIDEO_LAYER_CAPABILITY_S *pstSurf)
{
    if ( (eLayer < DEF_VIDEO_LAYER_MAX_NUMBER) && pstSurf)
    {
        *pstSurf = s_stVideoLayerCap[eLayer];
        return HI_SUCCESS;
    }
    else
    {
        return HI_FAILURE;
    }
}

HI_S32 AcquireLayer(HI_U32 eLayer)
{
    if (   (!s_stVideoLayerCap[eLayer].bSupport) 
        || (s_stVideoLayer[eLayer].bWorking)
        )
    {
        return HI_FAILURE;
    }

    s_stVideoLayer[eLayer].bWorking = HI_TRUE;

    return HI_SUCCESS;
}


HI_S32 AcquireLayerByDisplay(HI_DRV_DISPLAY_E eDisp, HI_U32 *peLayer)
{
    HI_U32 eId;
    
    if (HI_DRV_DISPLAY_1 == eDisp)
    {
        for(eId = VDP_RM_LAYER_VID0; eId < VDP_RM_LAYER_VID2; eId++)
        {
            if( s_stVideoLayerCap[eId].bSupport && !s_stVideoLayer[eId].bWorking)
            {
                *peLayer = eId;
                s_stVideoLayer[eId].bWorking = HI_TRUE;
                //printk(">>>>>>>>>>>  VIDEO LAYER %d Start....\n", eId);
                return HI_SUCCESS;
            }
        }
    }
    
    if (HI_DRV_DISPLAY_0 == eDisp)
    {
        for(eId = VDP_RM_LAYER_VID3; eId <=  VDP_RM_LAYER_VID4; eId++)
        {
            if( s_stVideoLayerCap[eId].bSupport && !s_stVideoLayer[eId].bWorking)
            {
                *peLayer = eId;
                s_stVideoLayer[eId].bWorking = HI_TRUE;
                return HI_SUCCESS;
            }
        }
    }

    return HI_FAILURE;
}


HI_S32 ReleaseLayer(HI_U32 eLayer)
{
    if (!s_stVideoLayerCap[eLayer].bSupport) 
    {
        return HI_FAILURE;
    }

    s_stVideoLayer[eLayer].bWorking = HI_FALSE;

    return HI_SUCCESS;
}


HI_S32 SetEnable(HI_U32 eLayer, HI_BOOL bEnable)
{
    VDP_VID_SetLayerEnable(eLayer, bEnable);
    return HI_SUCCESS;
}


HI_S32 Update(HI_U32 eLayer)
{
    VDP_VID_SetRegUp(eLayer);

    if ( (eLayer == VDP_RM_LAYER_VID0) || (eLayer == VDP_RM_LAYER_VID1) )
    {
        VDP_VP_SetRegUp(VDP_LAYER_VP0);
    }
    
    return HI_SUCCESS;
}

HI_S32 SetDefault(HI_U32 eLayer)
{
    HI_U32 uHid;
    VDP_BKG_S vidBkg;
    VIDEO_LAYER_CAPABILITY_S video_capbility;

    memset((void*)&video_capbility, 0, sizeof(VIDEO_LAYER_CAPABILITY_S));

    memset((void*)&vidBkg, 0, sizeof(VDP_BKG_S));
    uHid = eLayer;
    
    VDP_VID_SetLayerEnable(uHid, HI_FALSE);
    VDP_VID_SetInDataFmt(uHid, VDP_VID_IFMT_SP_420); 

    (HI_VOID)GetCapability(eLayer, &video_capbility);
    
    if (video_capbility.bZme)
        VDP_VID_SetReadMode(uHid, VDP_RMODE_PROGRESSIVE, VDP_RMODE_PROGRESSIVE);    
    else
        VDP_VID_SetReadMode(uHid, VDP_RMODE_INTERFACE, VDP_RMODE_INTERFACE);
        
    VDP_VID_SetMuteEnable(uHid, HI_FALSE);
    VDP_VID_SetFlipEnable(uHid, HI_FALSE);
    VDP_VID_SetIfirMode(uHid, VDP_IFIRMODE_6TAPFIR);
    VDP_VID_SetLayerGalpha(uHid, 0xff);

    vidBkg.u32BkgU = 0x11;
    vidBkg.u32BkgV = 0x11;
    vidBkg.u32BkgY = 0x11;
    vidBkg.u32BkgA = 0xff;
    VDP_VID_SetLayerBkg(uHid, vidBkg);

    return HI_SUCCESS;
}

HI_S32 SetAllLayerDefault(HI_VOID)
{
    SetDefault(VDP_RM_LAYER_VID0);
    SetDefault(VDP_RM_LAYER_VID1);
    //SetDefault(VDP_RM_LAYER_VID2);
    SetDefault(VDP_RM_LAYER_VID3);
    SetDefault(VDP_RM_LAYER_VID4);

    return HI_SUCCESS;
}



HI_S32 WIN_HAL_SetZme(HI_U32 eLayer, HI_RECT_S *in, HI_RECT_S *disp, HI_RECT_S *video)
{
//    DISP_UA_FUNCTION_S *pfZme;

    return HI_SUCCESS;
}

HI_S32 SetIORect(HI_U32 u32LayerId, HI_RECT_S *in, HI_RECT_S *disp, HI_RECT_S *video)
{
    VDP_RECT_S stFrmVRect;
    VDP_RECT_S stVRect;
    HI_U32 u32Ratio;


    stFrmVRect.u32X = 0;
    stFrmVRect.u32Y = 0;
    stFrmVRect.u32Wth = in->s32Width;
    stFrmVRect.u32Hgt = in->s32Height;

    stVRect.u32X = video->s32X;
    stVRect.u32Y = video->s32Y;
    stVRect.u32Wth = video->s32Width;
    stVRect.u32Hgt = video->s32Height;
    
    //VDP_VID_ZME_DEFAULT();

    VDP_VID_SetInReso(u32LayerId, stFrmVRect);
    VDP_VID_SetOutReso(u32LayerId,  stVRect);
    VDP_VID_SetVideoPos(u32LayerId, stVRect);
    VDP_VID_SetDispPos(u32LayerId,  stVRect);

    u32Ratio = (stFrmVRect.u32Wth << 20) / stVRect.u32Wth;
    VDP_VID_SetZmeHorRatio(u32LayerId, u32Ratio);

    u32Ratio = (stFrmVRect.u32Hgt << 12) / stVRect.u32Hgt;
    VDP_VID_SetZmeVerRatio(u32LayerId, u32Ratio);

    VDP_VID_SetZmeEnable(u32LayerId, VDP_ZME_MODE_HOR, 1);
    VDP_VID_SetZmeEnable(u32LayerId, VDP_ZME_MODE_VER, 1);

    return HI_SUCCESS;
}


HI_S32 WinHalSetColor_MPW(HI_U32 u32LayerId, HI_DRV_DISP_COLOR_SETTING_S *pstColor)
{
    ALG_CSC_DRV_PARA_S stIn;
    ALG_CSC_RTL_PARA_S stOut;
    VDP_CSC_DC_COEF_S stCscCoef;
    VDP_CSC_COEF_S stCscCoef2;
    DISP_UA_FUNCTION_S *pstuUA;

    pstuUA = DISP_UA_GetFunction();
    if (!pstuUA)
    {
        return HI_FAILURE;
    }

    stIn.eInputCS  = pstColor->enInCS;
    stIn.eOutputCS = pstColor->enOutCS;
    stIn.bIsBGRIn = HI_FALSE;
    
    stIn.u32Bright  = pstColor->u32Bright;
    stIn.u32Contrst = pstColor->u32Contrst;
    stIn.u32Hue     = pstColor->u32Hue;
    stIn.u32Satur   = pstColor->u32Satur;
    stIn.u32Kr = pstColor->u32Kr;
    stIn.u32Kg = pstColor->u32Kg;
    stIn.u32Kb = pstColor->u32Kb;

/*
    DISP_PRINT(">>>>>>>>WinHalSetColor_MPW i=%d, o=%d, B=%d, C=%d, H=%d, S=%d,KR=%d,KG=%d, KB=%d\n",
               pstColor->enInCS, pstColor->enOutCS, 
               pstColor->u32Bright,
               pstColor->u32Contrst,
               pstColor->u32Hue,
               pstColor->u32Satur,
               pstColor->u32Kr,
               pstColor->u32Kg,
               pstColor->u32Kb);
*/

    pstuUA->pfCalcCscCoef(&stIn, &stOut);

/*
    DISP_PRINT(">>>>>>>>WinHalSetColor_MPW D1=%d, D2=%d, C00=%d, C11=%d, C22=%d\n",
               stOut.s32CscDcIn_1, stOut.s32CscDcIn_2, 
               stOut.s32CscCoef_00,
               stOut.s32CscCoef_11,
               stOut.s32CscCoef_22);
*/

    stCscCoef.csc_in_dc0 = stOut.s32CscDcIn_0;
    stCscCoef.csc_in_dc1 = stOut.s32CscDcIn_1;
    stCscCoef.csc_in_dc2 = stOut.s32CscDcIn_2;

    stCscCoef.csc_out_dc0 = stOut.s32CscDcOut_0;
    stCscCoef.csc_out_dc1 = stOut.s32CscDcOut_1;
    stCscCoef.csc_out_dc2 = stOut.s32CscDcOut_2;
    VDP_VID_SetCscDcCoef(u32LayerId, stCscCoef);

    stCscCoef2.csc_coef00 = stOut.s32CscCoef_00;
    stCscCoef2.csc_coef01 = stOut.s32CscCoef_01;
    stCscCoef2.csc_coef02 = stOut.s32CscCoef_02;

    stCscCoef2.csc_coef10 = stOut.s32CscCoef_10;
    stCscCoef2.csc_coef11 = stOut.s32CscCoef_11;
    stCscCoef2.csc_coef12 = stOut.s32CscCoef_12;

    stCscCoef2.csc_coef20 = stOut.s32CscCoef_20;
    stCscCoef2.csc_coef21 = stOut.s32CscCoef_21;
    stCscCoef2.csc_coef22 = stOut.s32CscCoef_22;

    VDP_VID_SetCscCoef(u32LayerId, stCscCoef2);
    
    VDP_VID_SetCscEnable(u32LayerId, 1);

    return HI_SUCCESS;
}


HI_S32 SetPixFmt(HI_U32 u32LayerId, HI_DRV_PIX_FORMAT_E eFmt)
{
    if (eFmt == HI_DRV_PIX_FMT_NV12)
    {
        VDP_VID_SetInDataFmt(u32LayerId, VDP_VID_IFMT_SP_420);
        VDP_VID_SetInDataUVOrder(u32LayerId, 1);
        VDP_VID_SetZmeInFmt(u32LayerId, VDP_PROC_FMT_SP_420);
    }
    else if (eFmt == HI_DRV_PIX_FMT_NV21)
    {
        VDP_VID_SetInDataFmt(u32LayerId, VDP_VID_IFMT_SP_420);
        VDP_VID_SetInDataUVOrder(u32LayerId, 0);
        VDP_VID_SetZmeInFmt(u32LayerId, VDP_PROC_FMT_SP_420);
    }
    else if (eFmt == HI_DRV_PIX_FMT_NV16)
    {
        VDP_VID_SetInDataFmt(u32LayerId, VDP_VID_IFMT_SP_422);
        VDP_VID_SetInDataUVOrder(u32LayerId, 1);
        VDP_VID_SetZmeInFmt(u32LayerId, VDP_PROC_FMT_SP_422);
    }
    else if (eFmt == HI_DRV_PIX_FMT_NV61)
    {
        VDP_VID_SetInDataFmt(u32LayerId, VDP_VID_IFMT_SP_422);
        VDP_VID_SetInDataUVOrder(u32LayerId, 0);
        VDP_VID_SetZmeInFmt(u32LayerId, VDP_PROC_FMT_SP_422);
    }
    else if (eFmt == HI_DRV_PIX_FMT_YUYV)
    {
        VDP_VID_SetInDataFmt(u32LayerId, VDP_VID_IFMT_PKG_YUYV);
        //VDP_VID_SetInDataUVOrder(u32LayerId, 1);
        VDP_VID_SetZmeInFmt(u32LayerId, VDP_PROC_FMT_SP_422);
    }
    else if (eFmt == HI_DRV_PIX_FMT_YVYU)
    {
        VDP_VID_SetInDataFmt(u32LayerId, VDP_VID_IFMT_PKG_YVYU);
        //VDP_VID_SetInDataUVOrder(u32LayerId, 1);
        VDP_VID_SetZmeInFmt(u32LayerId, VDP_PROC_FMT_SP_422);
    }
    else if (eFmt == HI_DRV_PIX_FMT_UYVY)
    {
        VDP_VID_SetInDataFmt(u32LayerId, VDP_VID_IFMT_PKG_UYVY);
        //VDP_VID_SetInDataUVOrder(u32LayerId, 1);
        VDP_VID_SetZmeInFmt(u32LayerId, VDP_PROC_FMT_SP_422);
    }

    else
    {
        WIN_FATAL(">>>>>>>>>>>>> Error! not support vid format!\n");
    }

    return HI_SUCCESS;
}


HI_S32 SetAddr(HI_U32 u32LayerId, HI_DRV_VID_FRAME_ADDR_S *pstAddr, HI_U32 u32Num)
{
    VDP_VID_SetLayerAddr(u32LayerId, 0, pstAddr->u32PhyAddr_Y, pstAddr->u32PhyAddr_C, 
                         pstAddr->u32Stride_Y, pstAddr->u32Stride_C);

    VDP_VID_SetLayerAddr(u32LayerId, 1, pstAddr->u32PhyAddr_Y, pstAddr->u32PhyAddr_C, 
                         pstAddr->u32Stride_Y, pstAddr->u32Stride_C);

    return HI_SUCCESS;
}

HI_S32 WinHalSetAddr_MPW(HI_U32 u32LayerId, WIN_HAL_PARA_S *pstPara, HI_S32 s32exl)
{
    return Chip_Specific_WinHalSetAddr(u32LayerId, pstPara, s32exl);
}

HI_S32 WinHalSetPixFmt_MPW(HI_U32 u32LayerId, WIN_HAL_PARA_S *pstPara)
{
    HI_DRV_PIX_FORMAT_E eFmt = HI_DRV_PIX_BUTT;
    
    if (!pstPara)
    {
        DISP_FATAL_RETURN();
    }

    /*s40v2 defined null. i don't know is this a better way or not?
     * i think capablity-callback supplied for drv-level,not hal-level,
     * so i give a different implement and same interface instead of  capability,
     * to be reviewed by others.*/
     
    /*since dcmp treated as pixel fmt, so dcmp set here.
      closed first,because compressed stream and non-compressed will switch sometimes.*/
    VDP_VID_SetDcmpEnable(u32LayerId, 0);
    eFmt =  pstPara->pstFrame->ePixFormat;
    if (eFmt == HI_DRV_PIX_FMT_NV12)
    {
        VDP_VID_SetInDataFmt(u32LayerId, VDP_VID_IFMT_SP_420);
        VDP_VID_SetInDataUVOrder(u32LayerId, 1);
        VDP_VID_SetZmeInFmt(u32LayerId, VDP_PROC_FMT_SP_420);
    }
    else if (eFmt == HI_DRV_PIX_FMT_NV21)
    {
        VDP_VID_SetInDataFmt(u32LayerId, VDP_VID_IFMT_SP_420);
        VDP_VID_SetInDataUVOrder(u32LayerId, 0);
        VDP_VID_SetZmeInFmt(u32LayerId, VDP_PROC_FMT_SP_420);
    }
    else if (eFmt == HI_DRV_PIX_FMT_NV16)
    {
        VDP_VID_SetInDataFmt(u32LayerId, VDP_VID_IFMT_SP_422);
        VDP_VID_SetInDataUVOrder(u32LayerId, 1);
        VDP_VID_SetZmeInFmt(u32LayerId, VDP_PROC_FMT_SP_422);
    }
    else if (eFmt == HI_DRV_PIX_FMT_NV61)
    {
        VDP_VID_SetInDataFmt(u32LayerId, VDP_VID_IFMT_SP_422);
        VDP_VID_SetInDataUVOrder(u32LayerId, 0);
        VDP_VID_SetZmeInFmt(u32LayerId, VDP_PROC_FMT_SP_422);
    }
    else if (eFmt == HI_DRV_PIX_FMT_YUYV)
    {
        VDP_VID_SetInDataFmt(u32LayerId, VDP_VID_IFMT_PKG_YUYV);
        //VDP_VID_SetInDataUVOrder(u32LayerId, 1);
        VDP_VID_SetZmeInFmt(u32LayerId, VDP_PROC_FMT_SP_422);
    }
    else if (eFmt == HI_DRV_PIX_FMT_YVYU)
    {
        VDP_VID_SetInDataFmt(u32LayerId, VDP_VID_IFMT_PKG_YVYU);
        //VDP_VID_SetInDataUVOrder(u32LayerId, 1);
        VDP_VID_SetZmeInFmt(u32LayerId, VDP_PROC_FMT_SP_422);
    }
    else if (eFmt == HI_DRV_PIX_FMT_UYVY)
    {
        VDP_VID_SetInDataFmt(u32LayerId, VDP_VID_IFMT_PKG_UYVY);
        //VDP_VID_SetInDataUVOrder(u32LayerId, 1);
        VDP_VID_SetZmeInFmt(u32LayerId, VDP_PROC_FMT_SP_422);
    }
    else if (eFmt == HI_DRV_PIX_FMT_NV21_CMP)
    {
        VDP_VID_SetInDataFmt(u32LayerId, VDP_VID_IFMT_SP_420);
        VDP_VID_SetInDataUVOrder(u32LayerId, 0);
        VDP_VID_SetZmeInFmt(u32LayerId, VDP_PROC_FMT_SP_420);
        
        /*turn on dcmp.*/
        VDP_VID_SetDcmpEnable(u32LayerId, 1);         
    } 
    else if (eFmt == HI_DRV_PIX_FMT_NV12_CMP)
    {
        VDP_VID_SetInDataFmt(u32LayerId, VDP_VID_IFMT_SP_420);
        VDP_VID_SetInDataUVOrder(u32LayerId, 1);
        VDP_VID_SetZmeInFmt(u32LayerId, VDP_PROC_FMT_SP_420);

        /*turn on dcmp.*/
        VDP_VID_SetDcmpEnable(u32LayerId, 1);
    }
    else {
        WIN_FATAL(">>>>>>>>>>>>> Error! not support vid format!\n");
    }
        

    return HI_SUCCESS;
}

HI_S32 TranPixFmtToAlg(HI_DRV_PIX_FORMAT_E enFmt)
{
    switch (enFmt)
    {
        case HI_DRV_PIX_FMT_NV12:
        case HI_DRV_PIX_FMT_NV21:
        case HI_DRV_PIX_FMT_YVU420:
        case HI_DRV_PIX_FMT_NV12_CMP:
        case HI_DRV_PIX_FMT_NV21_CMP:
        case HI_DRV_PIX_FMT_NV12_TILE:
        case HI_DRV_PIX_FMT_NV21_TILE:
        case HI_DRV_PIX_FMT_NV12_TILE_CMP:
        case HI_DRV_PIX_FMT_NV21_TILE_CMP:
            return 1;

        case HI_DRV_PIX_FMT_NV16:
        case HI_DRV_PIX_FMT_NV61:
        case HI_DRV_PIX_FMT_NV16_2X1:
        case HI_DRV_PIX_FMT_NV61_2X1:
        case HI_DRV_PIX_FMT_YUYV:
        case HI_DRV_PIX_FMT_YYUV:
        case HI_DRV_PIX_FMT_YVYU:
        case HI_DRV_PIX_FMT_UYVY:
        case HI_DRV_PIX_FMT_VYUY:
        case HI_DRV_PIX_FMT_YUV422P:
        case HI_DRV_PIX_FMT_NV16_CMP:
        case HI_DRV_PIX_FMT_NV61_CMP:
        case HI_DRV_PIX_FMT_NV16_2X1_CMP:
        case HI_DRV_PIX_FMT_NV61_2X1_CMP:
            return 0;

        case HI_DRV_PIX_FMT_NV24_CMP:
        case HI_DRV_PIX_FMT_NV42_CMP:
        case HI_DRV_PIX_FMT_NV24:
        case HI_DRV_PIX_FMT_NV42:
            return 2;

        default:
            return 1;
    }

}

HI_S32 Get3DOutRect(HI_DRV_DISP_STEREO_E en3DMode, HI_RECT_S *pstOutRect, HI_RECT_S *pstReviseOutRect)
{
    *pstReviseOutRect = *pstOutRect; 
    if(en3DMode >= DISP_STEREO_BUTT)
    {
        WIN_ERROR("3d mode error!\n");
        return HI_FAILURE;
    }
    
    if (en3DMode == DISP_STEREO_FPK)
    {
      //  *pstReviseOutRect = stOutRect;
    }
    else if (en3DMode == DISP_STEREO_SBS_HALF)
    {
        pstReviseOutRect->s32Width = (pstOutRect->s32Width/2) & HI_WIN_OUT_RECT_WIDTH_ALIGN;
    }
    else if (en3DMode == DISP_STEREO_TAB)
    {
        pstReviseOutRect->s32Height = (pstOutRect->s32Height/2) & HI_WIN_OUT_RECT_HEIGHT_ALIGN;
    }
    else //no 3d mode
    {
        //WIN_ERROR("3d mode no support!\n");
    }

    return HI_SUCCESS;
}

HI_S32 ReviseWinOutRect(HI_RECT_S *pstInRect, HI_RECT_S *pstOutRect,HI_RECT_S stScreen, HI_DRV_DISP_OFFSET_S stOffsetRect)
{
    HI_RECT_S stInRect = *pstInRect;
    HI_RECT_S stOutRect = *pstOutRect;
    HI_DRV_DISP_OFFSET_S offset = stOffsetRect;
    HI_S32 s32VaildWidth,s32VaildHeight;
    HI_S32 ix = 0, iy = 0, iw = 0, ih = 0;
    HI_S32 ox =0, oy = 0, ow = 0, oh = 0;
    HI_BOOL bDispFlag = HI_FALSE;
   
    if(stOutRect.s32X < (HI_S32)offset.u32Left)
    {
        s32VaildWidth = stOutRect.s32Width  + stOutRect.s32X - offset.u32Left ; 

        if(s32VaildWidth < WIN_INRECT_MIN_WIDTH)
        {
            ow = WIN_INRECT_MIN_WIDTH;
            ox = offset.u32Left;
            iw = WIN_INRECT_MIN_WIDTH;
            ix = stInRect.s32Width - iw;
            bDispFlag = HI_TRUE;
        }
        else
        {   // >screen size
            ow = stScreen.s32Width - offset.u32Left - offset.u32Right;
            if(s32VaildWidth < ow) // no full screan
            {
                 ow = s32VaildWidth;
            }
            ox = offset.u32Left;
            iw = stInRect.s32Width * ow/stOutRect.s32Width;
            ix = stInRect.s32X + stInRect.s32Width - iw;
        }
    }
    else if (stOutRect.s32X < (stScreen.s32Width - offset.u32Right - WIN_INRECT_MIN_WIDTH))
    {
        s32VaildWidth = stOutRect.s32Width;

        if(s32VaildWidth < WIN_INRECT_MIN_WIDTH)
        {
            ow = WIN_INRECT_MIN_WIDTH;
            ox = stOutRect.s32X;
            iw = WIN_INRECT_MIN_WIDTH;
            ix = stInRect.s32X;
            bDispFlag = HI_TRUE;
        }
        else
        {
        
            ox = stOutRect.s32X;
            ix = stInRect.s32X;
            if((s32VaildWidth + ox) > (stScreen.s32Width - offset.u32Right))
            {
                ow = stScreen.s32Width - offset.u32Right - stOutRect.s32X;
                iw = stInRect.s32Width*ow/stOutRect.s32Width;
            }
            else
            {
               ow = stOutRect.s32Width;
               iw = stInRect.s32Width;
            }
        }
    }
    else 
    {
        ow = WIN_INRECT_MIN_WIDTH;
        ox = stScreen.s32Width - offset.u32Right - ow;
        iw = WIN_INRECT_MIN_WIDTH;
        ix = stInRect.s32X;
        bDispFlag = HI_TRUE;
    }
 
   
    if(stOutRect.s32Y < (HI_S32)offset.u32Top)
    {
        s32VaildHeight = stOutRect.s32Height + stOutRect.s32Y - offset.u32Top;
        if(s32VaildHeight < WIN_INRECT_MIN_HEIGHT)
        {
            oy = offset.u32Top;
            oh = WIN_INRECT_MIN_HEIGHT;
            ih = WIN_INRECT_MIN_HEIGHT;
            iy = stInRect.s32Height - ih;
            bDispFlag = HI_TRUE;
        }
        else
        {
            oy = offset.u32Top;
            oh = stScreen.s32Height - offset.u32Top - offset.u32Bottom;
            if(s32VaildHeight < oh)
            {
                oh = s32VaildHeight;
            }
            ih = stInRect.s32Height*oh/stOutRect.s32Height;
            iy = stInRect.s32Y + stInRect.s32Height - ih;
        }
    }
    else if (stOutRect.s32Y < (stScreen.s32Height - offset.u32Bottom - WIN_INRECT_MIN_HEIGHT))
    {   
        s32VaildHeight = stOutRect.s32Height;

        if(s32VaildHeight < WIN_INRECT_MIN_HEIGHT)
        {
            oy = stOutRect.s32Y;
            oh = WIN_INRECT_MIN_HEIGHT;
            ih = WIN_INRECT_MIN_HEIGHT;
            iy = stInRect.s32Y;
            bDispFlag = HI_TRUE;
        }
        else 
        {
            oy = stOutRect.s32Y;
            iy = stInRect.s32Y;
            if((s32VaildHeight + oy) > (stScreen.s32Height - offset.u32Bottom))
            {
                oh = stScreen.s32Height - stOutRect.s32Y - offset.u32Bottom;
                ih = stInRect.s32Height * oh/stOutRect.s32Height;
            }
            else
            {
                oh = stOutRect.s32Height;
                ih = stInRect.s32Height;
            }
         }
    }
    else
    {
        oh = WIN_INRECT_MIN_HEIGHT;
        oy = stScreen.s32Height- offset.u32Bottom - oh;
        ih = WIN_INRECT_MIN_HEIGHT;
        iy = stInRect.s32Y;
        bDispFlag = HI_TRUE;
    }

  
    pstInRect->s32X = ix & HI_WIN_IN_RECT_X_ALIGN;
    pstInRect->s32Y = iy & HI_WIN_IN_RECT_Y_ALIGN;
    pstInRect->s32Width = iw & HI_WIN_IN_RECT_WIDTH_ALIGN;
    pstInRect->s32Height = ih & HI_WIN_IN_RECT_HEIGHT_ALIGN;
#if 0 

    pstOutRect->s32X = ox & HI_WIN_OUT_RECT_X_ALIGN;
    pstOutRect->s32Y = oy & HI_WIN_OUT_RECT_Y_ALIGN;
    pstOutRect->s32Width = ow & HI_WIN_OUT_RECT_WIDTH_ALIGN;
    pstOutRect->s32Height = oh & HI_WIN_OUT_RECT_HEIGHT_ALIGN;
#else
    /*if there is different in ALIGN between inRect and OutRect ,may be reflect on ZME !*/
    pstOutRect->s32X = ox & HI_WIN_IN_RECT_X_ALIGN;
    pstOutRect->s32Y = oy & HI_WIN_IN_RECT_Y_ALIGN;
    pstOutRect->s32Width = ow & HI_WIN_IN_RECT_WIDTH_ALIGN;
    pstOutRect->s32Height = oh & HI_WIN_IN_RECT_HEIGHT_ALIGN;
#endif
    
    return bDispFlag;
}

HI_S32 zmeCoefAddr[5][5] = {
    {0, 0, 0, 0,10},
    {0, 0, 0, 0,10},
    {0, 0, 0, 0,10},
    {0, 0, 0, 0,10},
    {0, 0, 0, 0,10},
};

HI_S32 WinHalSetRect_MPW(HI_U32 u32LayerId, WIN_HAL_PARA_S *pstPara, HI_S32 s32exl)
{
    ALG_VZME_DRV_PARA_S stZmeI;
    ALG_VZME_RTL_PARA_S stZmeO;
    HI_RECT_S stIntmp,stIntmp1, stVideo, stDisp,stOutRect;
    
    HI_DRV_VIDEO_PRIVATE_S *pFrmPriv = (HI_DRV_VIDEO_PRIVATE_S *)&pstPara->pstFrame->u32Priv[0];    

    memset((void*)&stZmeI, 0, sizeof(ALG_VZME_DRV_PARA_S));
    memset((void*)&stZmeO, 0, sizeof(ALG_VZME_RTL_PARA_S));   
    
    stIntmp = pstPara->stIn;
    stIntmp.s32Height = stIntmp.s32Height/s32exl;

    /*since cv200 and s40v2 will accept differe width when crop
     *(orginal width should be set in cv200, but cropped width will be set in s40v2.)
     *,so we pass  both stInOrigin and stIn to VDP_VID_SetInReso2, make a choice by hal.
     */
    stIntmp1 = pstPara->stInOrigin;
    stVideo = pstPara->stVideo;
    stDisp = pstPara->stDisp;

    /*when pal and ntsc 1440 width, all the zme width set should be 2 size.*/
    if (pstPara->pstDispInfo->stPixelFmtResolution.s32Width
        == pstPara->pstDispInfo->stFmtResolution.s32Width *2)
    {
        stVideo.s32Width *=2;
        stVideo.s32X *=2;
        stDisp.s32Width  *=2;       
        stDisp.s32X  *=2;       
    }
    
    VDP_VID_SetInReso2(u32LayerId, &stIntmp, &stIntmp1);
    VDP_VID_SetOutReso2(u32LayerId,  &stVideo);
    VDP_VID_SetVideoPos2(u32LayerId, &stVideo);
    VDP_VID_SetDispPos2(u32LayerId,  &stDisp);

    stZmeI.u32ZmeFrmWIn = stIntmp.s32Width;
    stZmeI.u32ZmeFrmHIn = stIntmp.s32Height;
    
    Get3DOutRect(pstPara->en3Dmode,&stVideo, &stOutRect);
    stZmeI.u32ZmeFrmWOut = stOutRect.s32Width;
    stZmeI.u32ZmeFrmHOut = stOutRect.s32Height;

    stZmeI.u8ZmeYCFmtIn  = TranPixFmtToAlg(pstPara->pstFrame->ePixFormat);

    stZmeI.u8ZmeYCFmtOut = 0; // X5HD2 MPW FIX '0'(422)
    stZmeI.bZmeFrmFmtIn  = 1;
    stZmeI.bZmeFrmFmtOut = (pstPara->pstDispInfo->bInterlace == HI_TRUE) ? 0 : 1;
    
    stZmeI.bZmeBFIn  = 0;
    stZmeI.bZmeBFOut = 0;
    
    if (HI_SUCCESS != Chip_Specific_LayerZmeFunc(u32LayerId, &stZmeI, &stZmeO))
        return HI_FAILURE;
    
    VDP_VID_SetZmeHorRatio(u32LayerId, stZmeO.u32ZmeRatioHL);
    VDP_VID_SetZmeVerRatio(u32LayerId, stZmeO.u32ZmeRatioVL);

    VDP_VID_SetZmePhaseH(u32LayerId, stZmeO.s32ZmeOffsetHL, stZmeO.s32ZmeOffsetHC);
    VDP_VID_SetZmePhaseV(u32LayerId, stZmeO.s32ZmeOffsetVL, stZmeO.s32ZmeOffsetVC);
    VDP_VID_SetZmePhaseVB(u32LayerId, stZmeO.s32ZmeOffsetVLBtm, stZmeO.s32ZmeOffsetVCBtm);

    VDP_VID_SetZmeMidEnable2(u32LayerId, stZmeO.bZmeMedHL);
    VDP_VID_SetZmeHfirOrder(u32LayerId, stZmeO.bZmeOrder);
    VDP_VID_SetZmeVchTap(u32LayerId, stZmeO.bZmeTapVC);

    if (stZmeO.bZmeMdHL || stZmeO.bZmeMdHC)
    {
        VDP_VID_SetZmeCoefAddr(u32LayerId, VDP_VID_PARA_ZME_HOR, stZmeO.u32ZmeCoefAddrHL, stZmeO.u32ZmeCoefAddrHC);
        VDP_VID_SetParaUpd(u32LayerId,VDP_VID_PARA_ZME_HOR);
    }

    if (stZmeO.bZmeMdVL || stZmeO.bZmeMdVC)
    {
        VDP_VID_SetZmeCoefAddr(u32LayerId, VDP_VID_PARA_ZME_VER, stZmeO.u32ZmeCoefAddrVL, stZmeO.u32ZmeCoefAddrVC);
        VDP_VID_SetParaUpd(u32LayerId,VDP_VID_PARA_ZME_VER);
    }
    
    if (  (pstPara->pstDispInfo->stPixelFmtResolution.s32Width  == stVideo.s32Width)
        &&(pstPara->pstDispInfo->stPixelFmtResolution.s32Height == stVideo.s32Height)
        &&(pFrmPriv->u32Fidelity > 0)
        )
    {
        // for fidelity output
        VDP_VID_SetZmeFirEnable2(u32LayerId, 
                                 stZmeO.bZmeMdHL,
                                 stZmeO.bZmeMdHC,
                                 HI_FALSE,
                                 HI_FALSE);
    }
    else
    {
        // for normal output

        /*as a result of problem */
        if ((zmeCoefAddr[u32LayerId][0] != stZmeO.u32ZmeCoefAddrHL)
            || (zmeCoefAddr[u32LayerId][1] != stZmeO.u32ZmeCoefAddrHC)
            || (zmeCoefAddr[u32LayerId][2] != stZmeO.u32ZmeCoefAddrVL)
            || (zmeCoefAddr[u32LayerId][3] != stZmeO.u32ZmeCoefAddrVC)
            || (zmeCoefAddr[u32LayerId][4] < 5) )
        {
            if (zmeCoefAddr[u32LayerId][4] >= 5)
                zmeCoefAddr[u32LayerId][4] = 0;
            
            VDP_VID_SetZmeFirEnable2(u32LayerId, 0, 0, 0,0);            
            zmeCoefAddr[u32LayerId][0] = stZmeO.u32ZmeCoefAddrHL;
            zmeCoefAddr[u32LayerId][1] = stZmeO.u32ZmeCoefAddrHC;
            zmeCoefAddr[u32LayerId][2] = stZmeO.u32ZmeCoefAddrVL;
            zmeCoefAddr[u32LayerId][3] = stZmeO.u32ZmeCoefAddrVC;
            zmeCoefAddr[u32LayerId][4] ++;
        } else {        
           VDP_VID_SetZmeFirEnable2(u32LayerId, 
                                 stZmeO.bZmeMdHL,
                                 stZmeO.bZmeMdHC,
                                 stZmeO.bZmeMdVL,
                                 stZmeO.bZmeMdVC);
           zmeCoefAddr[u32LayerId][4]  = 10;
        }
    }

    VDP_VID_SetZmeInFmt(u32LayerId, VDP_PROC_FMT_SP_420);
    VDP_VID_SetZmeOutFmt(u32LayerId, VDP_PROC_FMT_SP_422);

    VDP_VID_SetZmeEnable2(u32LayerId, stZmeO.bZmeEnVL);

    return HI_SUCCESS;
}

HI_S32 WinHalGetExtrLineParam(HI_U32 u32LayerId, WIN_HAL_PARA_S *pstPara)
{
    HI_S32 s32exl = 1;
    HI_S32 s32HeightIn, s32HeightOut;
    
    s32HeightIn = pstPara->stIn.s32Height;
    s32HeightOut = pstPara->stVideo.s32Height;
    
    while((s32HeightIn / s32exl)  > (s32HeightOut * VIDEO_ZOOM_IN_VERTICAL_MAX))
    {
        s32exl = s32exl * 2;
    }
    
    return s32exl;
}


HI_S32 WinHalSetFrame_MPW(HI_U32 u32LayerId, WIN_HAL_PARA_S *pstPara)
{
    HI_S32 s32exl;

    /*
     *since we support picture moved out of screen, so we should
     *give a revise to the window , both inrect and outrect.
     */
    (HI_VOID)ReviseWinOutRect(&pstPara->stIn, &pstPara->stVideo,
                     pstPara->pstDispInfo->stFmtResolution, 
                     pstPara->pstDispInfo->stOffsetInfo);
    
    pstPara->stDisp = pstPara->stVideo;    
    if(HAL_SetDispMode(u32LayerId, pstPara->en3Dmode) )
    {
        return HI_FAILURE;
    }

    s32exl = WinHalGetExtrLineParam(u32LayerId, pstPara);
    if (WinHalSetAddr_MPW(u32LayerId, pstPara, s32exl))
    {
        return HI_FAILURE;
    }

    if( WinHalSetPixFmt_MPW(u32LayerId, pstPara) )
    {
        return HI_FAILURE;
    }

    if (pstPara->bZmeUpdate)
    {
        if( WinHalSetRect_MPW(u32LayerId, pstPara, s32exl) )
        {
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

VDP_CBM_MIX_E GetMixerID(VDP_LAYER_VID_E eLayer)
{
    if ( (VDP_LAYER_VID0 <= eLayer) && (VDP_LAYER_VID1 >= eLayer))
    {
        return VDP_CBM_MIXV0;
    }
    else if ( (VDP_LAYER_VID3 <= eLayer) && (VDP_LAYER_VID4 >= eLayer))
    {
        return VDP_CBM_MIXV1;
    }
    else
    {
        return VDP_CBM_MIX_BUTT;
    }
}



HI_S32 MovUp(HI_U32 eLayer)
{
    VDP_CBM_MIX_E eMixId;
    HI_U32 nMaxLayer;
    VDP_LAYER_VID_E eLayerHalId;
    HI_U32 MixArray[VDP_LAYER_VID_BUTT+1];
    HI_U32 i, index;

    // get eLayer Id
    eLayerHalId = (VDP_LAYER_VID_E)eLayer;
    // get mixv id
    eMixId = GetMixerID(eLayerHalId);


//printk("in id=%d, halid=%d\n", eLayer, eLayerHalId);
    // get mixv setting
    nMaxLayer = Chip_Specific_GetMixvMaxNumvber(eMixId);

//printk("eMixId=%d, nMaxLayer=%d\n", eMixId, nMaxLayer);
    if (nMaxLayer <= 1)
    {
        return HI_SUCCESS;
    }

    // get eLayer prio
    for(i=0; i<nMaxLayer; i++)
    {
        Chip_Specific_CBM_GetMixvPrio(eMixId, i, &MixArray[i]);
        //printk("prio=%d, id=%d\n", i, MixArray[i]);
    }

    // get eLayer index
    index = nMaxLayer;
    for(i=0; i<nMaxLayer; i++)
    {
    	//printk("i=%d, id=%d\n", i, MixArray[i]);
        if (MixArray[i] == (HI_U32)eLayerHalId)
        {
            index = i;
            break;
        }
    }

  	//printk("index=%d\n", index);

    // not found or just single layer work
    if (index >= (nMaxLayer-1))
    {
        return HI_SUCCESS;
    }

    // change mixv order
    MixArray[index]= MixArray[index+1];
    MixArray[index+1] = eLayerHalId;

    // set mixv setting
    Chip_Specific_CBM_SetMixvPrio(eMixId, MixArray, nMaxLayer);

    return HI_SUCCESS;
}


HI_S32 MovTop(HI_U32 eLayer)
{
    VDP_CBM_MIX_E eMixId;
    HI_U32 nMaxLayer;
    VDP_LAYER_VID_E eLayerHalId;
    HI_U32 MixArray[VDP_LAYER_VID_BUTT+1];
    HI_U32 i, index;

    // get eLayer Id
    eLayerHalId = (VDP_LAYER_VID_E)eLayer;

    // get mixv id
    eMixId = GetMixerID(eLayerHalId);

    // get mixv setting
    nMaxLayer = Chip_Specific_GetMixvMaxNumvber(eMixId);
    if (nMaxLayer <= 1)
    {
        return HI_SUCCESS;
    }

    // get eLayer prio
    for(i=0; i<nMaxLayer; i++)
    {
        Chip_Specific_CBM_GetMixvPrio(eMixId, i, &MixArray[i]);
    }

    // get eLayer index
    index = nMaxLayer;
    for(i=0; i<nMaxLayer; i++)
    {
        if (MixArray[i] == (HI_U32)eLayerHalId)
        {
            index = i;
            break;
        }
    }

    // not found or just single layer work
    if (index >= (nMaxLayer-1))
    {
        return HI_SUCCESS;
    }

    // change mixv order
    for(i=index; i<(nMaxLayer-1); i++)
    {
        MixArray[i]= MixArray[i+1];
    }
    MixArray[i] = eLayerHalId;

    // set mixv setting
    Chip_Specific_CBM_SetMixvPrio(eMixId, MixArray, nMaxLayer);
    return HI_SUCCESS;
}


HI_S32 MovDown(HI_U32 eLayer)
{
    VDP_CBM_MIX_E eMixId;
    HI_U32 nMaxLayer;
    VDP_LAYER_VID_E eLayerHalId;
    HI_U32 MixArray[VDP_LAYER_VID_BUTT+1];
    HI_U32 i, index;

    // get eLayer Id
    eLayerHalId = (VDP_LAYER_VID_E)eLayer;

    // get mixv id
    eMixId = GetMixerID(eLayerHalId);

    // get mixv setting
    nMaxLayer = Chip_Specific_GetMixvMaxNumvber(eMixId);
    if (nMaxLayer <= 1)
    {
        return HI_SUCCESS;
    }

    // get eLayer prio
    for(i=0; i<nMaxLayer; i++)
    {
        Chip_Specific_CBM_GetMixvPrio(eMixId, i, &MixArray[i]);
    }

    // get eLayer index
    index = nMaxLayer;
    for(i=0; i<nMaxLayer; i++)
    {
        if (MixArray[i] == (HI_U32)eLayerHalId)
        {
            index = i;
            break;
        }
    }

    // not found
    if (index >= nMaxLayer)
    {
        return HI_SUCCESS;
    }

    // layer at bottom
    if (!index)
    {
        return HI_SUCCESS;
    }

    // change mixv order
    MixArray[index]= MixArray[index-1];
    MixArray[index - 1] = eLayerHalId;

    // set mixv setting
    Chip_Specific_CBM_SetMixvPrio(eMixId, MixArray, nMaxLayer);
    return HI_SUCCESS;
}


HI_S32 MovBottom(HI_U32 eLayer)
{
    VDP_CBM_MIX_E eMixId;
    HI_U32 nMaxLayer;
    VDP_LAYER_VID_E eLayerHalId;
    HI_U32 MixArray[VDP_LAYER_VID_BUTT+1];
    HI_U32 i, index;

    // get eLayer Id
    eLayerHalId = (VDP_LAYER_VID_E)eLayer;

    // get mixv id
    eMixId = GetMixerID(eLayerHalId);

    // get mixv setting
    nMaxLayer = Chip_Specific_GetMixvMaxNumvber(eMixId);
    if (nMaxLayer <= 1)
    {
        return HI_SUCCESS;
    }

    // get eLayer prio
    for(i=0; i<nMaxLayer; i++)
    {
        Chip_Specific_CBM_GetMixvPrio(eMixId, i, &MixArray[i]);
    }

    // get eLayer index
    index = nMaxLayer;
    for(i=0; i<nMaxLayer; i++)
    {
        if (MixArray[i] == (HI_U32)eLayerHalId)
        {
            index = i;
            break;
        }
    }

    // not found
    if (index >= nMaxLayer)
    {
        return HI_SUCCESS;
    }

    // layer at bottom
    if (!index)
    {
        return HI_SUCCESS;
    }

    // change mixv order
    for(i=index; i>0; i--)
    {
        MixArray[i]= MixArray[i-1];
    }
    MixArray[0] = eLayerHalId;

    // set mixv setting
    Chip_Specific_CBM_SetMixvPrio(eMixId, MixArray, nMaxLayer);
    return HI_SUCCESS;
}

HI_S32 GetZorder(HI_U32 eLayer, HI_U32 *pZOrder)
{
    VDP_CBM_MIX_E eMixId;
    HI_U32 nMaxLayer;
    VDP_LAYER_VID_E eLayerHalId;
    HI_U32 MixArray[VDP_LAYER_VID_BUTT+1];
    HI_U32 i, index;

    // get eLayer Id
    eLayerHalId = (VDP_LAYER_VID_E)eLayer;

//printk("in id=%d, halid=%d\n", eLayer, eLayerHalId);

    // get mixv id
    eMixId = GetMixerID(eLayerHalId);

    // get mixv setting
    nMaxLayer = Chip_Specific_GetMixvMaxNumvber(eMixId);

//printk("eMixId=%d, nMaxLayer=%d\n", eMixId, nMaxLayer);
    if (nMaxLayer <= 1)
    {
        *pZOrder = 0;
        return HI_SUCCESS;
    }


    // get eLayer prio
    for(i=0; i<nMaxLayer; i++)
    {
        Chip_Specific_CBM_GetMixvPrio(eMixId, i, &MixArray[i]);
        //printk("prio=%d, id=%d\n", i, MixArray[i]);
    }

    // get eLayer index
    index = nMaxLayer;
    for(i=0; i<nMaxLayer; i++)
    {
        if (MixArray[i] == (HI_U32)eLayerHalId)
        {
            index = i;
            break;
        }
    }

    // not found
    if (index >= nMaxLayer)
    {
        *pZOrder = 0xfffffffful;
    }
    else
    {
        *pZOrder = index;
    }

    return HI_SUCCESS;
}



HI_S32 SetACC(HI_U32 eLayer, VIDEO_LAYER_ACC_MODE_E eMode)
{

    return HI_SUCCESS;
}


HI_S32 SetACM(HI_U32 eLayer, VIDEO_LAYER_ACM_MODE_E eMode)
{

    return HI_SUCCESS;
}


HI_S32 SetDebug(HI_U32 eLayer, HI_BOOL bEnable)
{

    return HI_SUCCESS;
}

HI_S32 VideoLayer_Init(HI_DRV_DISP_VERSION_S *pstVersion)
{
    if (s_bVideoSurfaceFlag >= 0)
    {
        return HI_SUCCESS;
    }

    // s1 init videolayer
    DISP_MEMSET(&s_stVieoLayerFunc, 0, sizeof(VIDEO_LAYER_FUNCTIONG_S));
    DISP_MEMSET(&s_stVideoLayer, 0, sizeof(VIDEO_LAYER_S) * DEF_VIDEO_LAYER_MAX_NUMBER);


    // s2 init function pointer
    if (   (pstVersion->u32VersionPartH == DISP_CV200_ES_VERSION_H)
        && (pstVersion->u32VersionPartL == DISP_CV200_ES_VERSION_L)
        )
    {
        // s2.1 set function pointer
        s_stVieoLayerFunc.PF_GetCapability  = GetCapability;
        s_stVieoLayerFunc.PF_AcquireLayer   = AcquireLayer;
        s_stVieoLayerFunc.PF_AcquireLayerByDisplay = AcquireLayerByDisplay;
        s_stVieoLayerFunc.PF_ReleaseLayer   = ReleaseLayer;    
        s_stVieoLayerFunc.PF_SetEnable      = SetEnable;       
        s_stVieoLayerFunc.PF_Update         = Update;          
        s_stVieoLayerFunc.PF_SetDefault     = SetDefault;
        s_stVieoLayerFunc.PF_SetAllLayerDefault = SetAllLayerDefault;
        //s_stVieoLayerFunc.PF_SetFramePara   = SetFramePara;    
        s_stVieoLayerFunc.PF_SetDispMode    = HAL_SetDispMode;     
        s_stVieoLayerFunc.PF_SetIORect      = SetIORect;       
        s_stVieoLayerFunc.PF_SetColor  = WinHalSetColor_MPW;   
        s_stVieoLayerFunc.PF_SetPixFmt      = SetPixFmt;       
        s_stVieoLayerFunc.PF_SetAddr        = SetAddr;         
        s_stVieoLayerFunc.PF_MovUp          = MovUp;           
        s_stVieoLayerFunc.PF_MovTop         = MovTop;          
        s_stVieoLayerFunc.PF_MovDown        = MovDown;         
        s_stVieoLayerFunc.PF_MovBottom      = MovBottom;  
        s_stVieoLayerFunc.PF_GetZorder      = GetZorder;  
        s_stVieoLayerFunc.PF_SetACC         = SetACC;          
        s_stVieoLayerFunc.PF_SetACM         = SetACM;          
        s_stVieoLayerFunc.PF_SetDebug       = SetDebug;        


        s_stVieoLayerFunc.PF_SetFramePara   = WinHalSetFrame_MPW;
        s_stVieoLayerFunc.PF_Get3DOutRect   = Get3DOutRect;

        // s2.2 init videolayer capbility
        Chip_Specific_SetLayerCapability(s_stVideoLayerCap);

        // s2.3 init hardware

    }
    else
    {
        WIN_ERROR("Not support version : %x %x\n", 
                   pstVersion->u32VersionPartH, pstVersion->u32VersionPartL);

        return HI_FAILURE;
    }

    s_bVideoSurfaceFlag++;
    
    return HI_SUCCESS;
}


HI_S32 VideoLayer_DeInit(HI_VOID)
{
    if (s_bVideoSurfaceFlag < 0)
    {
        return HI_SUCCESS;
    }

    s_bVideoSurfaceFlag--;
    
    return HI_SUCCESS;
}

HI_S32 VideoLayer_GetFunction(VIDEO_LAYER_FUNCTIONG_S *pstFunc)
{
    if (s_bVideoSurfaceFlag < 0)
    {
        WIN_ERROR("Video layer NOT INIT\n");
        return HI_FAILURE;
    }


    if (!pstFunc)
    {
        WIN_ERROR("NULL Pointer\n");
        return HI_FAILURE;
    }

    *pstFunc = s_stVieoLayerFunc;

    return HI_SUCCESS;
}

VIDEO_LAYER_FUNCTIONG_S *VideoLayer_GetFunctionPtr(HI_VOID)
{
    if (s_bVideoSurfaceFlag < 0)
    {
        WIN_ERROR("Video layer NOT INIT\n");
        return HI_NULL;
    }

    return &s_stVieoLayerFunc;
}




#ifdef __cplusplus
 #if __cplusplus
}
 #endif
#endif /* __cplusplus */
