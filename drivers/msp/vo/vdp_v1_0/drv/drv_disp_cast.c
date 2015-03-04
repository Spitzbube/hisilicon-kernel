
/******************************************************************************
  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.
******************************************************************************
File Name     : drv_disp_cast.h
Version       : Initial Draft
Author        : Hisilicon multimedia software group
Created       : 2012/12/30
Last Modified :
Description   :
Function List :
History       :
******************************************************************************/

#include "drv_disp_com.h"
#include "drv_disp_cast.h"
#include "drv_venc_ext.h"
#include "hi_drv_module.h"
#include "drv_disp_priv.h"
#include "drv_display.h"
#ifdef __cplusplus
 #if __cplusplus
extern "C" {
 #endif
#endif /* __cplusplus */

static HI_S32 CastCheckCfg(HI_DRV_DISP_CAST_CFG_S * pstCfg, HI_DISP_DISPLAY_INFO_S *pstInfo)
{
    if (  (pstCfg->u32Width < DISP_CAST_MIN_W)
        ||(pstCfg->u32Width > DISP_CAST_MAX_W)
        ||(pstCfg->u32Height < DISP_CAST_MIN_H)
        ||(pstCfg->u32Height > DISP_CAST_MAX_H)
        ||( (pstCfg->u32Width & 0x1)  != 0)
        ||( (pstCfg->u32Height & 0x3) != 0)
        )
    {
        DISP_ERROR("Cast w= %d or h=%d invalid\n", pstCfg->u32Width, pstCfg->u32Height);
        return HI_FAILURE;
    }    

    if (  (pstCfg->eFormat != HI_DRV_PIX_FMT_NV21)
        &&(pstCfg->eFormat != HI_DRV_PIX_FMT_NV12)
        &&(pstCfg->eFormat != HI_DRV_PIX_FMT_YUYV)
        &&(pstCfg->eFormat != HI_DRV_PIX_FMT_YVYU)
        &&(pstCfg->eFormat != HI_DRV_PIX_FMT_UYVY)
        )
    {
        DISP_ERROR("Cast pixfmt = %d invalid\n", pstCfg->eFormat);
        return HI_FAILURE;
    }

    if (  (pstCfg->u32BufNumber < DISP_CAST_BUFFER_MIN_NUMBER) 
        ||(pstCfg->u32BufNumber > DISP_CAST_BUFFER_MAX_NUMBER) )
    {
        DISP_ERROR("Cast u32BufNumber =%d invalid\n", pstCfg->u32BufNumber);
        return HI_FAILURE;
    }

    if(pstCfg->bUserAlloc) 
    {
        DISP_ERROR("Cast not support User Alloc memory\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static HI_S32 CastSetFrameDemoPartA(HI_HANDLE cast_ptr, HI_DRV_DISP_CAST_CFG_S *pstCfg, DISP_CAST_ATTR_S *pstAttr)
{
    HI_DRV_VIDEO_FRAME_S *pstFrame;
    DISP_CAST_PRIV_FRAME_S *pstPriv;

    // set frame demo
    DISP_MEMSET(pstAttr, 0, sizeof(DISP_CAST_ATTR_S));

    pstAttr->stOut.s32Width  = (HI_S32)pstCfg->u32Width;
    pstAttr->stOut.s32Height = (HI_S32)pstCfg->u32Height;

    pstFrame = &pstAttr->stFrameDemo;
    pstFrame->eFrmType = HI_DRV_FT_NOT_STEREO;
    pstFrame->ePixFormat = pstCfg->eFormat;
    
    pstFrame->bProgressive = HI_TRUE;
    pstFrame->u32Width  = pstCfg->u32Width;
    pstFrame->u32Height = pstCfg->u32Height;
    pstFrame->stDispRect = pstAttr->stOut;

    pstPriv = (DISP_CAST_PRIV_FRAME_S *)&(pstFrame->u32Priv[0]);

    pstPriv->cast_ptr = cast_ptr;
    pstPriv->stPrivInfo.u32PlayTime = 1;

    return HI_SUCCESS;
}

#define Cast_ERROR_DEAL(a, flag)  do{                    \
        if (HI_SUCCESS !=   (a))                    \
        {               \
            return HI_FAILURE;\
        }\
    }while(0);

static HI_S32 Cast_GetFrame(DISP_CAST_S *pstCast, 
                     HI_U32 *u32BufId, 
                     HI_U32 buf_type,
                     HI_DRV_VIDEO_FRAME_S *frameInfo)
{
    if(buf_type == 1)
    {
        Cast_ERROR_DEAL(BP_GetFullBuf(&pstCast->stBP, u32BufId), 0);
        Cast_ERROR_DEAL(BP_DelFullBuf(&pstCast->stBP, *u32BufId), 1);   
        Cast_ERROR_DEAL(BP_GetFrame(&pstCast->stBP, *u32BufId, frameInfo), 1);
    } else {    
        Cast_ERROR_DEAL(BP_GetEmptyBuf(&pstCast->stBP, u32BufId), 0);
        Cast_ERROR_DEAL(BP_DelEmptyBuf(&pstCast->stBP, *u32BufId), 1);
        Cast_ERROR_DEAL(BP_GetFrame(&pstCast->stBP, *u32BufId, frameInfo), 1);        
    }
    return HI_SUCCESS;
}

HI_S32 DISP_CastCreate(HI_DRV_DISPLAY_E enDisp,
                       HI_DISP_DISPLAY_INFO_S *pstInfo,
                       HI_DRV_DISP_CAST_CFG_S *pstCfg,
                       HI_HANDLE *cast_ptr)
{
    HI_DRV_DISP_CALLBACK_S stCB1;
    DISP_CAST_S *pstCast;
    BUF_ALLOC_S stAlloc;
    HI_S32 nRet;

    // check cfg
    if( CastCheckCfg(pstCfg, pstInfo) )
    {
        DISP_ERROR("Cast config invalid!\n");
        return HI_ERR_DISP_INVALID_PARA;
    }

    // alloc mem
    pstCast = (DISP_CAST_S *)DISP_MALLOC( sizeof(DISP_CAST_S) );
    if (!pstCast)
    {
        DISP_ERROR("Cast malloc failed!\n");
        return HI_ERR_DISP_INVALID_PARA;
    }

    DISP_MEMSET(pstCast, 0, sizeof(DISP_CAST_S));

    // set attr
    CastSetFrameDemoPartA((HI_HANDLE)pstCast, pstCfg, &pstCast->stAttr);

    // get hal operation
    nRet = DISP_HAL_GetOperation(&pstCast->stIntfOpt);
    if (nRet)
    {
        DISP_ERROR("Cast get hal operation failed!\n");
        goto __ERR_EXIT__;
    }

    // get wbclayer
    nRet = pstCast->stIntfOpt.PF_AcquireWbcByChn(enDisp, &pstCast->eWBC);
    if (nRet)
    {
        DISP_ERROR("Cast get wbc layer failed!\n");
        goto __ERR_EXIT__;
    }

    // create buffer
    stAlloc.bFbAllocMem  = !pstCfg->bUserAlloc;
    stAlloc.eDataFormat  = pstCfg->eFormat;
    stAlloc.u32BufWidth  = pstCfg->u32Width;
    stAlloc.u32BufHeight = pstCfg->u32Height;
    stAlloc.u32BufStride = pstCfg->u32BufStride;
    stAlloc.u32BufSize = pstCfg->u32BufSize;
    nRet = BP_Create(pstCfg->u32BufNumber, &stAlloc, &pstCast->stBP);
    if (nRet)
    {
        DISP_ERROR("Cast alloc buffer failed!\n");
        goto __ERR_EXIT__;
    }

    // register callback
    stCB1.hDst = (HI_HANDLE)pstCast;
    stCB1.pfDISP_Callback = DISP_CastCBWork;
    nRet = DISP_ISR_RegCallback(enDisp, HI_DRV_DISP_C_INTPOS_0_PERCENT, &stCB1);
    if (nRet)
    {
        DISP_ERROR("Cast register work callback failed!\n");
        goto __ERR_EXIT__;
    }

    pstCast->u32LastCfgBufId= 0;
    pstCast->u32LastFrameBufId = 0;

    
    pstCast->bToGetDispInfo = HI_TRUE;
    pstCast->bOpen = HI_TRUE;
    pstCast->eDisp = enDisp;
	pstCast->u32Ref = 1;
    *cast_ptr = (HI_HANDLE)pstCast;

    DISP_PRINT("DISP_CastCreate ok\n");

    return HI_SUCCESS;

__ERR_EXIT__:
    DISP_ISR_UnRegCallback(enDisp, HI_DRV_DISP_C_INTPOS_0_PERCENT, &stCB1);

    BP_Destroy(&pstCast->stBP);

    DISP_FREE(pstCast);
    return nRet;
}

HI_S32 DISP_CastDestroy(HI_HANDLE cast_ptr)
{
    HI_DRV_DISP_CALLBACK_S stCB1;
    DISP_CAST_S *pstCast;
    HI_S32 nRet;

    pstCast = (DISP_CAST_S *)cast_ptr;

    // set disable
    pstCast->bEnable = HI_FALSE;
    /*release  buffer and stop intr func is asynchronus,
      so we should wait. after intr stop, then release the mmz mem,
       to avoid writing after release.*/
    msleep(100);
    
    stCB1.hDst = (HI_HANDLE)cast_ptr;
    stCB1.pfDISP_Callback = DISP_CastCBWork;
    nRet = DISP_ISR_UnRegCallback(pstCast->eDisp, HI_DRV_DISP_C_INTPOS_0_PERCENT, &stCB1);

    msleep(60);

    // destroy buffer
    BP_Destroy(&pstCast->stBP);

    pstCast->stIntfOpt.PF_ReleaseWbc(pstCast->eWBC);

    // free mem
    DISP_FREE(pstCast);

    return HI_SUCCESS;
}


HI_S32 DISP_CastSetEnable(HI_HANDLE cast_ptr, HI_BOOL bEnable)
{
    DISP_CAST_S *pstCast;

    pstCast = (DISP_CAST_S *)cast_ptr;

    pstCast->bEnable = bEnable;

    DISP_PRINT("DISP_CastSetEnable  bEnable = 0x%x\n", (HI_U32)bEnable);

    return HI_SUCCESS;
}

HI_S32 DISP_CastGetEnable(HI_HANDLE cast_ptr, HI_BOOL *pbEnable)
{
    DISP_CAST_S *pstCast;

    pstCast = (DISP_CAST_S *)cast_ptr;

    *pbEnable = pstCast->bEnable;

    return HI_SUCCESS;
}

HI_S32	DispCastSetFramePTS(HI_DRV_VIDEO_FRAME_S *pstCastFrame,MIRA_GET_PTS_E pts_flag)
{
    DISP_CAST_PRIV_FRAME_S *pstPrivFrame;

    pstPrivFrame = (DISP_CAST_PRIV_FRAME_S *)&(pstCastFrame->u32Priv[0]);

    if(!pts_flag) {        
        DISP_OS_GetTime(&pstPrivFrame->u32Pts0);
        pstCastFrame->u32Pts    = pstPrivFrame->u32Pts0;
        pstCastFrame->u32SrcPts = pstPrivFrame->u32Pts0;       
    }
    else {        
        DISP_OS_GetTime(&pstPrivFrame->u32Pts1);        
    }
    

    return HI_SUCCESS;}


static HI_S32 Cast_AcquireFrame(DISP_CAST_S *pstCast, HI_DRV_VIDEO_FRAME_S *pstCastFrame)
{
    HI_S32 nRet;
    HI_U32 u32BufId = 0;
    nRet = Cast_GetFrame(pstCast, &u32BufId, 1, pstCastFrame);      
    if (nRet)
    {
        DISP_WARN("Cast get id failed!\n");
        return nRet;
    }   

    DispCastSetFramePTS(pstCastFrame, MIRA_SET_AQUIRE_PTS);
    
    return HI_SUCCESS;
}

static HI_S32 Cast_PushFrameToBackMod(DISP_CAST_S *pstCast)
{
    HI_S32 nRet = HI_SUCCESS;
    HI_DRV_VIDEO_FRAME_S pstCastFrame;
    FN_VENC_PUT_FRAME  venc_queue_pfn  = NULL;
    
    /*if this is null, means get frame though unf api, not attach mode.*/
    if (pstCast->attach_pairs[0].pfnQueueFrm) {
        pstCast->u32CastAcquireTryCnt ++;        
        if (Cast_AcquireFrame(pstCast, &pstCastFrame))
           return HI_FAILURE;        
        venc_queue_pfn = pstCast->attach_pairs[0].pfnQueueFrm;
        nRet = venc_queue_pfn(pstCast->attach_pairs[0].hSink, &pstCastFrame);
        
        if (nRet == HI_SUCCESS)
            pstCast->u32CastAcquireOkCnt ++;
    }
    
    return nRet;
}

HI_S32 DISP_CastAcquireFrame(HI_HANDLE cast_ptr, HI_DRV_VIDEO_FRAME_S *pstCastFrame)
{
    DISP_CAST_S *pstCast;
    HI_S32 Ret = 0;

    pstCast = (DISP_CAST_S *)cast_ptr;

    if(pstCast->attach_pairs[0].pfnQueueFrm)
    {
        DISP_ERROR("attach mode is going on ,you can't acquire through unf api.");
        return HI_FAILURE;
    }
    
    Ret = Cast_AcquireFrame(pstCast, pstCastFrame);
    
    pstCast->u32CastAcquireTryCnt ++;
    if (HI_SUCCESS == Ret)     
        pstCast->u32CastAcquireOkCnt ++;
    
#if 0
       printk("cast acquire bufid=0x%x, w=%d,h=%d,index=%d, y=0x%x, c=0x%x\n", 
       u32BufId,
       pstCastFrame->u32Width, 
       pstCastFrame->u32Height,
       pstCastFrame->u32FrmCnt,
       pstCastFrame->stBufAddr[0].u32PhyAddr_Y,
       pstCastFrame->stBufAddr[0].u32PhyAddr_C
       );
#endif

   return Ret;    
}


HI_S32 DispCastCheckFrame(HI_HANDLE cast_ptr, HI_DRV_VIDEO_FRAME_S *pstCastFrame)
{
    DISP_CAST_PRIV_FRAME_S *pstPrivFrame;

    pstPrivFrame = (DISP_CAST_PRIV_FRAME_S *)&(pstCastFrame->u32Priv[0]);

    if (pstPrivFrame->cast_ptr == cast_ptr)
    {
        return HI_SUCCESS;
    }
    else
    {
        return HI_FAILURE;
    }
}


HI_S32 DispCastGetBufId(HI_HANDLE cast_ptr, 
                        HI_DRV_VIDEO_FRAME_S *pstCastFrame, 
                        HI_U32 *pu32BufId)
{
    DISP_CAST_PRIV_FRAME_S *pstPrivFrame;

    pstPrivFrame = (DISP_CAST_PRIV_FRAME_S *)&(pstCastFrame->u32Priv[0]);

    if (pstPrivFrame->cast_ptr != cast_ptr)
    {
        DISP_PRINT("PRIV=[%x][%x]\n", 
            pstPrivFrame->cast_ptr, 
            pstPrivFrame->stPrivInfo.u32BufferID);
        
        return HI_FAILURE;
    }
    
    *pu32BufId = pstPrivFrame->stPrivInfo.u32BufferID;
    
    return HI_SUCCESS;
}

HI_S32 DISP_CastReleaseFrame(HI_HANDLE cast_ptr, HI_DRV_VIDEO_FRAME_S *pstCastFrame)
{
    DISP_CAST_S *pstCast;
    HI_U32 u32BufId;
    HI_S32 nRet = HI_SUCCESS;

    
    pstCast = (DISP_CAST_S *)cast_ptr;

    pstCast->u32CastReleaseTryCnt ++;

    nRet = DispCastGetBufId(cast_ptr, pstCastFrame, &u32BufId);
    if (nRet)
    {
        DISP_WARN("Cast release frame invalid!\n");
        return nRet;
    }
    
    nRet = BP_AddEmptyBuf(&pstCast->stBP, u32BufId);
    if (nRet)
    {
        DISP_ERROR("Cast release frame failed!\n");
        return nRet;
    }

    nRet = BP_SetBufEmpty(&pstCast->stBP, u32BufId);
    if (nRet)
    {
        DISP_ERROR("Cast release frame failed!\n");
        return nRet;
    }

    pstCast->u32CastReleaseOkCnt ++;
    return HI_SUCCESS;
}

HI_S32 DispSetFrameDemoPartB(HI_HANDLE cast_ptr, 
                             HI_U32 u32Rate, 
                             HI_DRV_COLOR_SPACE_E eColorSpace, 
                             HI_DRV_VIDEO_FRAME_S *pstFrame)
{
    DISP_CAST_S *pstCast;
    DISP_CAST_PRIV_FRAME_S *pstPriv;

    pstCast = (DISP_CAST_S *)cast_ptr;
    pstPriv = (DISP_CAST_PRIV_FRAME_S *)&(pstFrame->u32Priv[0]);

    //venc and vpss and others,they want a rate * 1000;
    pstFrame->u32FrameRate = u32Rate * 10;
    pstPriv->stPrivInfo.eColorSpace = eColorSpace;

    return HI_SUCCESS;
}

HI_VOID DISP_CastCBSetDispMode(HI_HANDLE cast_ptr, 
                               const HI_DRV_DISP_CALLBACK_INFO_S *pstInfo)
{
    DISP_CAST_S *pstCast;
    HI_U32 Rate;

    pstCast = (DISP_CAST_S *)cast_ptr;

    // set display info
    pstCast->stDispInfo = pstInfo->stDispInfo;

    pstCast->stAttr.stIn = pstInfo->stDispInfo.stPixelFmtResolution;
    pstCast->stAttr.bInterlace = pstInfo->stDispInfo.bInterlace;
    pstCast->stAttr.eInColorSpace = pstInfo->stDispInfo.eColorSpace;
    //Todo
    pstCast->stAttr.eOutColorSpace = pstCast->stAttr.eInColorSpace;

    Rate = pstInfo->stDispInfo.u32RefreshRate;
    pstCast->stAttr.u32InRate = Rate;

    pstCast->u32Periods = 1;
    while(Rate > DISP_CAST_MAX_FRAME_RATE)
    {
        pstCast->u32Periods = pstCast->u32Periods << 1;
        Rate = Rate >> 1;
    }
    
    pstCast->stAttr.u32OutRate = Rate;
    DispSetFrameDemoPartB(cast_ptr, Rate, pstCast->stAttr.eOutColorSpace, &pstCast->stAttr.stFrameDemo);

    DISP_PRINT("CAST: iw=%d, ih=%d, ow=%d, oh=%d, or=%d\n ",  
                pstCast->stAttr.stIn.s32Width,
                pstCast->stAttr.stIn.s32Height,
                pstCast->stAttr.stOut.s32Width,
                pstCast->stAttr.stOut.s32Height,
                Rate);
    return ;
}


HI_S32 DispCastSetFrameInfo(DISP_CAST_S *pstCast, HI_DRV_VIDEO_FRAME_S *pstCurFrame)
{
    DISP_CAST_PRIV_FRAME_S *pstPrivFrame;

    pstCast->stAttr.stFrameDemo.u32FrameIndex = pstCast->u32FrameCnt;
    pstCast->stAttr.stFrameDemo.stBufAddr[0] = pstCurFrame->stBufAddr[0];
    
    pstPrivFrame = (DISP_CAST_PRIV_FRAME_S *)&(pstCast->stAttr.stFrameDemo.u32Priv[0]);
    pstPrivFrame->stPrivInfo.u32FrmCnt   = pstCast->u32FrameCnt;
    pstPrivFrame->stPrivInfo.u32BufferID = pstCast->u32LastCfgBufId;   

    DISP_OS_GetTime(&pstCast->stAttr.stFrameDemo.u32Pts);
    return HI_SUCCESS;
}


HI_S32 DispCastSendTask(DISP_CAST_S *pstCast)
{
    DISP_CAST_ATTR_S *pstAttr;
    pstAttr = &pstCast->stAttr;
    
    // config pixformat
    pstCast->stIntfOpt.PF_SetWbcPixFmt(pstCast->eWBC, pstAttr->stFrameDemo.ePixFormat);
    
    pstCast->stIntfOpt.PF_SetWbcIORect(pstCast->eWBC, &pstCast->stDispInfo, &pstAttr->stIn, &pstAttr->stOut);
    
    pstCast->stIntfOpt.PF_SetWbc3DInfo(pstCast->eWBC, &pstCast->stDispInfo, &pstAttr->stIn);

    // config csc
    pstCast->stIntfOpt.PF_SetWbcColorSpace(pstCast->eWBC, pstAttr->eInColorSpace, pstAttr->eOutColorSpace);

    // config addr
    pstCast->stIntfOpt.PF_SetWbcAddr(pstCast->eWBC, &(pstAttr->stFrameDemo.stBufAddr[0]));

    // set enable
    pstCast->stIntfOpt.PF_SetWbcEnable(pstCast->eWBC, HI_TRUE);
    pstCast->stIntfOpt.PF_UpdateWbc(pstCast->eWBC);

    return HI_SUCCESS;
}


HI_VOID DISP_CastCBWork(HI_HANDLE cast_ptr, const HI_DRV_DISP_CALLBACK_INFO_S *pstInfo)
{
    HI_DRV_VIDEO_FRAME_S stCurFrame;
    DISP_CAST_S *pstCast;
    HI_U32 u32BufId;
    HI_S32 nRet;
    DISP_CAST_PRIV_FRAME_S *pstPrivFrame = NULL;

    pstCast = (DISP_CAST_S *)cast_ptr;

    if ( (pstInfo->eEventType == HI_DRV_DISP_C_PREPARE_CLOSE)
        ||(pstInfo->eEventType == HI_DRV_DISP_C_PREPARE_TO_PEND)
       )
    {
        pstCast->bMasked  = HI_TRUE;
        pstCast->bToGetDispInfo = HI_TRUE;
    }
    else
    {
        pstCast->bMasked  = HI_FALSE;
    }

    if (pstInfo->eEventType == HI_DRV_DISP_C_OPEN)
    {
        pstCast->bToGetDispInfo = HI_TRUE;
    }

    if (pstCast->bToGetDispInfo)
    {
        DISP_CastCBSetDispMode(cast_ptr, pstInfo);

        pstCast->bToGetDispInfo = HI_FALSE;
    }

	if (pstCast->bScheduleWbc) {
        pstCast->stIntfOpt.PF_SetWbcEnable(pstCast->eWBC, HI_FALSE);
        pstCast->stIntfOpt.PF_UpdateWbc(pstCast->eWBC);
		pstCast->bScheduleWbcStatus = HI_TRUE;
 		return ;
	}

    // check state
    if(!pstCast->bEnable || pstCast->bMasked)
    {
        // set enable
        pstCast->stIntfOpt.PF_SetWbcEnable(pstCast->eWBC, HI_FALSE);
        pstCast->stIntfOpt.PF_UpdateWbc(pstCast->eWBC);

        //printk("[z:%d,%d,%d] ", pstCast->bEnable, pstCast->bMasked, pstCast->bDispSet);
        return;
    }

    if (pstInfo->eEventType == HI_DRV_DISP_C_VT_INT)
    {
        // set enable
        pstCast->stIntfOpt.PF_SetWbcEnable(pstCast->eWBC, HI_FALSE);
        pstCast->stIntfOpt.PF_UpdateWbc(pstCast->eWBC);


        // put frame
        if (pstCast->u32LastFrameBufId)
        {
            //printk("L=%d,", pstCast->u32LastFrameBufId);
            nRet = BP_AddFullBuf(&pstCast->stBP, pstCast->u32LastFrameBufId);
            if(nRet)
            {
                DISP_ERROR("Cast ADD buf failed!\n");
                return;
            }
            pstCast->u32LastFrameBufId = 0;
        }

        /*we call this every intr,but will take no affect in unf api mode,
         * judged in the func body. 
         */
        Cast_PushFrameToBackMod(pstCast);
        
        /*update the ptr,because we have got new born frame from full buf, so
         *we should put a new empty node to wbc to be born.
         */
        if (pstCast->u32LastCfgBufId)
        {
            pstCast->u32LastFrameBufId = pstCast->u32LastCfgBufId;
            pstCast->u32LastCfgBufId = 0;
        }      

        /*since we may get 25pfs in 50hz fmt, so a interval is necessary.*/
        pstCast->u32TaskCount++;        
        if ((pstCast->u32TaskCount % pstCast->u32Periods) != 0)
        {
            return;
        }
        
        nRet = Cast_GetFrame(pstCast, &u32BufId, 0, &stCurFrame);
        if (nRet)
        {
            //DISP_ERROR("Cast get empty id failed!\n");
            return;
        }
        
        pstCast->u32LastCfgBufId = u32BufId;
        pstCast->u32FrameCnt++;

        DispCastSetFrameInfo(pstCast, &stCurFrame);

        DispCastSetFramePTS(&pstCast->stAttr.stFrameDemo, MIRA_SET_CREATE_PTS);
        
        pstPrivFrame = (DISP_CAST_PRIV_FRAME_S *)&(pstCast->stAttr.stFrameDemo.u32Priv[0]);        
        pstPrivFrame->u32Pts0 += 100000 / pstCast->stDispInfo.u32RefreshRate;
            
        pstCast->stAttr.stFrameDemo.u32Pts    = pstPrivFrame->u32Pts0;
        pstCast->stAttr.stFrameDemo.u32SrcPts = pstPrivFrame->u32Pts0;
        
        DispCastSendTask(pstCast);
        
        nRet = BP_SetFrame(&pstCast->stBP, pstCast->u32LastCfgBufId, &pstCast->stAttr.stFrameDemo);
        DISP_ASSERT(!nRet);
    }

    return;
}

HI_S32 DISP_Cast_AttachSink(HI_HANDLE cast_ptr, HI_HANDLE hSink)
{
    HI_MOD_ID_E enModID;
    DISP_CAST_S *pstCast;
    HI_S32 s32Ret;
    VENC_EXPORT_FUNC_S *pstVenFunc = HI_NULL;
    HI_U32  attach_index  = 0;

    pstCast = (DISP_CAST_S *)cast_ptr; 
    enModID = (HI_MOD_ID_E)((hSink & 0xff0000) >> 16);
    
    if ( HI_ID_VENC == enModID ) {
        s32Ret = HI_DRV_MODULE_GetFunction(enModID,(HI_VOID**)&(pstVenFunc));        
        if (HI_SUCCESS != s32Ret) {
            DISP_ERROR("Get null venc ptr when cast.\n");
            return HI_ERR_DISP_NULL_PTR;
        }
        
        for (attach_index = 0; attach_index < DISPLAY_ATTACH_CNT_MAX; attach_index++) {
            if (pstCast->attach_pairs[attach_index].hSink == hSink) {
                return HI_SUCCESS;
            }            
        }
        
        for (attach_index = 0; attach_index < DISPLAY_ATTACH_CNT_MAX; attach_index++) {
            if (pstCast->attach_pairs[attach_index].hSink == 0) {                
                pstCast->attach_pairs[attach_index].hSink = hSink;
                pstCast->attach_pairs[attach_index].pfnQueueFrm = pstVenFunc->pfnVencQueueFrame;
                pstCast->attach_pairs[attach_index].pfnDequeueFrame = HI_NULL;  
                break;
            }            
        }

        if (attach_index == DISPLAY_ATTACH_CNT_MAX) {       
            s32Ret = HI_FAILURE;
        }
        else{
            s32Ret = HI_SUCCESS;
        }
    } else {
       s32Ret = HI_FAILURE;
    }
    return s32Ret;    
}

HI_S32 DISP_Cast_DeAttachSink(HI_HANDLE cast_ptr, HI_HANDLE hSink)
{
    HI_MOD_ID_E enModID;
    DISP_CAST_S *pstCast;
    HI_S32 s32Ret;
    HI_U32  attach_index  = 0;

    pstCast = (DISP_CAST_S *)cast_ptr; 
    enModID = (HI_MOD_ID_E)((hSink & 0xff0000) >> 16);    
    
    if ( HI_ID_VENC == enModID ) {
        
        for (attach_index = 0; attach_index < DISPLAY_ATTACH_CNT_MAX; attach_index++) {
            if (pstCast->attach_pairs[attach_index].hSink == hSink) {                
                pstCast->attach_pairs[attach_index].hSink = 0;
                pstCast->attach_pairs[attach_index].pfnQueueFrm = HI_NULL;
                pstCast->attach_pairs[attach_index].pfnDequeueFrame = HI_NULL;
                 break;
            }           
        }
        
        if (attach_index == DISPLAY_ATTACH_CNT_MAX)        
            s32Ret = HI_FAILURE;
        else
            s32Ret = HI_SUCCESS;        
    } else {
       s32Ret = HI_FAILURE;
    }

    /*as a result of asynchrounous bettween Cast and venc,
     *we should wait ,because if venc detach and return, cast may keep 
     * writing  to venc. 
     */
    msleep(40);
    
    return s32Ret;    
}

HI_S32 DISP_Cast_SetAttr(HI_HANDLE cast_ptr, HI_DRV_DISP_Cast_Attr_S *castAttr)
{   
    return HI_ERR_DISP_NOT_SUPPORT;    
}

HI_S32 DISP_Cast_GetAttr(HI_HANDLE cast_ptr, HI_DRV_DISP_Cast_Attr_S *castAttr)
{   
    return HI_ERR_DISP_NOT_SUPPORT;    
}


#define SNAPSHOT_MAGIC 0x534e4150 /* ASCII code of "SNAP" */
static HI_U32 s_u32FrameCnt = 1;

HI_S32 DISP_Acquire_Snapshot(HI_DRV_DISPLAY_E enDisp, HI_HANDLE *snapshotHandle, HI_DRV_VIDEO_FRAME_S *pstFrame)
{
    BUF_ALLOC_S stAlloc;
    HI_S32 ret;
    HI_DRV_VIDEO_FRAME_S* pstVideoFrame = pstFrame;
    DISP_INTF_OPERATION_S stFunc;
    DISP_WBC_E eWBC;
    HI_DISP_DISPLAY_INFO_S stInfo;
    HI_U32 u32BufId = 0;
    DISP_SNAPSHOT_PRIV_FRAME_S *pstPriv;
    HI_U32 u32Pts;
    DISP_SNAPSHOT_S *pstSnapshot = HI_NULL;
    HI_BOOL  bBufAlloc = 0;
    
    *snapshotHandle  = 0;
    
    pstSnapshot = (DISP_SNAPSHOT_S*)DISP_MALLOC(sizeof(DISP_SNAPSHOT_S));   
    if (!pstSnapshot) 
        return HI_FAILURE;    

    DISP_MEMSET(pstSnapshot, 0, sizeof(DISP_SNAPSHOT_S));    
    ret = DISP_HAL_GetOperation(&stFunc);
    if (ret) {
        goto __ERR_EXIT__;
    }
    
    ret = stFunc.PF_AcquireWbcByChn(enDisp, &eWBC);
    if (ret) {
        if (eWBC >= DISP_WBC_BUTT)
            goto __ERR_EXIT__;
    }
   
    (HI_VOID)DISP_GetDisplayInfo( enDisp, &stInfo);
    stAlloc.bFbAllocMem = HI_TRUE;
    stAlloc.eDataFormat = HI_DRV_PIX_FMT_NV21;
    stAlloc.u32BufHeight = stInfo.stFmtResolution.s32Height;
    stAlloc.u32BufWidth = stInfo.stFmtResolution.s32Width;
    stAlloc.u32BufStride = 0;
    ret = BP_Create(1, &stAlloc, &pstSnapshot->stBP);
    if (ret) 
    {
        goto __ERR_EXIT__;
    }    

    bBufAlloc = 1;
    ret = BP_GetEmptyBuf(&pstSnapshot->stBP, &u32BufId);
    if (ret) 
    {
        goto __ERR_EXIT__;
    }    

    ret = BP_DelEmptyBuf(&pstSnapshot->stBP, u32BufId);
    if (ret) 
    {
        goto __ERR_EXIT__;
    }    

    ret = BP_GetFrame(&pstSnapshot->stBP, u32BufId, pstVideoFrame); 
    if (ret) 
    {
        goto __ERR_EXIT__;
    }        

    pstVideoFrame->u32Width  = stInfo.stFmtResolution.s32Width;
    pstVideoFrame->u32Height = stInfo.stFmtResolution.s32Height;
    pstVideoFrame->u32AspectWidth = 16;
    pstVideoFrame->u32AspectHeight = 9;
    pstVideoFrame->u32FrameRate = 0;
    pstVideoFrame->ePixFormat = HI_DRV_PIX_FMT_NV21;
    pstVideoFrame->bProgressive = HI_TRUE;
    pstVideoFrame->enFieldMode = HI_DRV_FIELD_ALL;
    pstVideoFrame->bTopFieldFirst = 0;
    pstVideoFrame->stDispRect = stInfo.stFmtResolution;
    pstVideoFrame->eFrmType = HI_DRV_FT_NOT_STEREO;
    pstVideoFrame->u32FrameIndex = 0;
    memset(pstVideoFrame->u32Priv, 0, sizeof(pstVideoFrame->u32Priv));
    // config pixformat 
    stFunc.PF_SetWbcPixFmt(eWBC, HI_DRV_PIX_FMT_NV21);

    /*FIXME:  ourrect  why be stInfo.stFmtResolution? error?*/
    stFunc.PF_SetWbcIORect(eWBC, &stInfo, &stInfo.stPixelFmtResolution, &stInfo.stFmtResolution);

    stFunc.PF_SetWbc3DInfo(eWBC, &stInfo, &stInfo.stPixelFmtResolution);

    // config csc
    stFunc.PF_SetWbcColorSpace(eWBC, stInfo.eColorSpace, stInfo.eColorSpace);

    // config addr
    stFunc.PF_SetWbcAddr(eWBC, &(pstVideoFrame->stBufAddr[0]));

    // set enable FIXME!!!
    stFunc.PF_SetWbcEnable(eWBC, HI_TRUE);
    stFunc.PF_UpdateWbc(eWBC);

    msleep(60);

    /* set pts. */
    DISP_OS_GetTime(&u32Pts);
    pstFrame->u32Pts    = u32Pts;
    pstFrame->u32SrcPts = u32Pts;       

    /* construct some private info. */
    pstPriv = (DISP_SNAPSHOT_PRIV_FRAME_S*)&(pstFrame->u32Priv[0]);
    pstPriv->stPrivInfo.u32FrmCnt = s_u32FrameCnt ++;
    pstPriv->u32BPAddr = (HI_U32)&(pstSnapshot->stBP);
    pstPriv->u32Magic = SNAPSHOT_MAGIC;

    pstSnapshot->bWork = 1;
    
    *snapshotHandle  = (HI_HANDLE)pstSnapshot;
    return HI_SUCCESS;
    
__ERR_EXIT__:    
    if (bBufAlloc)
        BP_Destroy(&pstSnapshot->stBP);
    
    DISP_FREE(pstSnapshot);
    return HI_FAILURE;
}

HI_S32 DISP_Release_Snapshot(HI_DRV_DISPLAY_E enDisp, HI_HANDLE snapshotHandle, HI_DRV_VIDEO_FRAME_S *pstFrame)
{
    DISP_SNAPSHOT_PRIV_FRAME_S *pstPriv; 
    DISP_SNAPSHOT_S *pstSnapshot = HI_NULL;
    
    pstPriv = (DISP_SNAPSHOT_PRIV_FRAME_S *)&(pstFrame->u32Priv[0]);
    if (pstPriv->u32Magic != SNAPSHOT_MAGIC)
        return HI_ERR_DISP_INVALID_PARA;

    if (!snapshotHandle)
        return HI_ERR_DISP_NULL_PTR; 

    pstSnapshot = (DISP_SNAPSHOT_S *)snapshotHandle;
    
     if (pstSnapshot->bWork == 0)
        return HI_SUCCESS;
    
    pstSnapshot->bWork = 0;    
    BP_Destroy(&pstSnapshot->stBP);
    DISP_FREE(pstSnapshot);
    return 0;
}


HI_S32 DISP_SnapshotDestroy(HI_HANDLE snapshot_ptr)
{
    DISP_SNAPSHOT_S *pstSnapshot = HI_NULL;

    

    pstSnapshot = (DISP_SNAPSHOT_S *)snapshot_ptr;    
    if (!pstSnapshot)
        return HI_ERR_DISP_NULL_PTR; 

    if (pstSnapshot->bWork == 0)
        return HI_SUCCESS;

    /*when we destroy, wbc may be going on,so we wait.*/
    msleep(40);    
    
    pstSnapshot->bWork = 0; 
    BP_Destroy(&pstSnapshot->stBP);    
    DISP_FREE(pstSnapshot);

    return HI_SUCCESS;
}



#ifdef __cplusplus
 #if __cplusplus
}
 #endif
#endif /* __cplusplus */





