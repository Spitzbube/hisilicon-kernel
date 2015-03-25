#include "drv_virtual.h"
#include "drv_win_priv.h"
#include "hi_drv_sys.h"
#include "drv_venc_ext.h"
#include "hi_drv_module.h"

#ifdef __cplusplus
 #if __cplusplus
extern "C" {
 #endif
#endif /* __cplusplus */

HI_S32 VIR_BUFFER_Init(VIR_BUFFER_S *pstBuffer)
{
    memset(pstBuffer->stBufArray,0,
                 sizeof(HI_DRV_VIDEO_FRAME_S)*DEF_VIR_BUFFER_LENGTH);

    pstBuffer->u32Head = 0 ;
    pstBuffer->u32Tail = 0 ;

    pstBuffer->enType = MUTUAL_TYPE_SINKACTIVE;

    return HI_SUCCESS;    
}
HI_S32 VIR_BUFFER_DeInit(VIR_BUFFER_S *pstBuffer)
{
    pstBuffer->u32Head = 0 ;
    pstBuffer->u32Tail = 0 ;

    pstBuffer->enType = MUTUAL_TYPE_BUTT;

    return HI_SUCCESS;
}
HI_S32 VIR_BUFFER_SetType(VIR_BUFFER_S *pstBuffer, MUTUAL_TYPE_E enType)
{
    if (pstBuffer->enType != MUTUAL_TYPE_BUTT)
    {
        WIN_ERROR("Virtual Window buffer type can't be changed.\n");
        return HI_FAILURE;
    }
    
    pstBuffer->enType = enType;

    return HI_SUCCESS;
}

HI_S32 VIR_BUFFER_GetFrm(VIR_BUFFER_S *pstBuffer,HI_DRV_VIDEO_FRAME_S *pstFrm)
{
    MUTUAL_TYPE_E enType;
    HI_S32 s32Ret;
    HI_U32 u32Head;
    HI_U32 u32Tail;
    HI_DRV_VIDEO_FRAME_S *pstArrayFrm;
    
    enType = pstBuffer->enType;

    switch (enType)
    {
        case MUTUAL_TYPE_SRCACTIVE:
            WIN_ERROR("MUTUAL_TYPE_SRCACTIVE Can't be supported.\n");
            s32Ret = HI_FAILURE;
            break;
        case MUTUAL_TYPE_SINKACTIVE:
            u32Head = pstBuffer->u32Head;
            u32Tail = pstBuffer->u32Tail;
            if (u32Tail != u32Head)
            {
                pstArrayFrm = &(pstBuffer->stBufArray[u32Head]);
                memcpy(pstFrm, pstArrayFrm, sizeof(HI_DRV_VIDEO_FRAME_S));
                memset(pstArrayFrm, 0, sizeof(HI_DRV_VIDEO_FRAME_S));
                pstBuffer->u32Head = (pstBuffer->u32Head + 1)%DEF_VIR_BUFFER_LENGTH;

                s32Ret = HI_SUCCESS;
            }
            else
            {
                s32Ret = HI_FAILURE;
            }
            break;
        default:
            WIN_ERROR("MUTUAL TYPE Can't be supported.\n");
            s32Ret = HI_FAILURE;
            break;
    }
    
    return s32Ret;
    
}

HI_S32 VIR_BUFFER_AddFrm(VIR_BUFFER_S *pstBuffer,HI_DRV_VIDEO_FRAME_S *pstFrm)
{   
    MUTUAL_TYPE_E enType;
    HI_S32 s32Ret;
    HI_U32 u32Head;
    HI_U32 u32Tail;
    HI_DRV_VIDEO_FRAME_S *pstArrayFrm;
    
    enType = pstBuffer->enType;
    
    switch (enType)
    {
        case MUTUAL_TYPE_SRCACTIVE:
            WIN_ERROR("MUTUAL_TYPE_SRCACTIVE Can't be supported.\n");
            s32Ret = HI_FAILURE;
            break;
        case MUTUAL_TYPE_SINKACTIVE:
            u32Head = pstBuffer->u32Head;
            u32Tail = pstBuffer->u32Tail;
            if ((u32Tail + 1)%DEF_VIR_BUFFER_LENGTH != u32Head)
            {
                pstArrayFrm = &(pstBuffer->stBufArray[u32Tail]);
                memcpy(pstArrayFrm,pstFrm,sizeof(HI_DRV_VIDEO_FRAME_S));
                pstBuffer->u32Tail = (pstBuffer->u32Tail + 1)%DEF_VIR_BUFFER_LENGTH;
                s32Ret = HI_SUCCESS;
            }
            else
            {
                s32Ret = HI_FAILURE;
            }
            break;
        default:
            WIN_ERROR("MUTUAL TYPE Can't be supported.\n");
            s32Ret = HI_FAILURE;
            break;
    }
    
    return s32Ret;
}

HI_S32 WIN_VIR_Create(HI_DRV_WIN_ATTR_S *pWinAttr, VIRTUAL_S **ppstVirWin)
{   
    VIRTUAL_S *pstVirWin;
    
    pstVirWin = (VIRTUAL_S *)DISP_MALLOC(sizeof(VIRTUAL_S));
    if (!pstVirWin)
    {
        WIN_ERROR("Malloc VIRTUAL_S failed in %s!\n", __FUNCTION__);
        return HI_ERR_VO_MALLOC_FAILED;
    }

    DISP_MEMSET(pstVirWin, 0, sizeof(VIRTUAL_S));

    pstVirWin->stAttrBuf = *pWinAttr;

    pstVirWin->stAttrBuf.stCropRect.u32BottomOffset = 0;
    pstVirWin->stAttrBuf.stCropRect.u32LeftOffset   = 0;
    pstVirWin->stAttrBuf.stCropRect.u32RightOffset  = 0;
    pstVirWin->stAttrBuf.stCropRect.u32TopOffset    = 0;
        
    atomic_set(&pstVirWin->bNewAttrFlag, 1);

    VIR_BUFFER_Init(&(pstVirWin->stBuffer));
    
    pstVirWin->enBufType = MUTUAL_TYPE_SINKACTIVE;

    *ppstVirWin = pstVirWin;

    
    return HI_SUCCESS;
}

HI_S32 WIN_VIR_Destroy(VIRTUAL_S *pstVirWin)
{

     WinCheckNullPointer(pstVirWin);
     
     VIR_BUFFER_DeInit(&(pstVirWin->stBuffer));

     DISP_FREE(pstVirWin);
     
     return HI_SUCCESS;
}

HI_S32 WIN_VIR_SendAttrToSource(VIRTUAL_S *pstVirWin)
{
    HI_DRV_WIN_PRIV_INFO_S stWinPriv;

    memset(&stWinPriv,0,sizeof(HI_DRV_WIN_PRIV_INFO_S));

    if (pstVirWin->stSrcInfo.pfSendWinInfo != HI_NULL)
    {
        //stWinPriv.ePixFmt = pstVirWin->stAttrBuf.enDataFormat;
        stWinPriv.ePixFmt = HI_DRV_PIX_FMT_NV21;
        
        stWinPriv.u32MaxRate =  30;
        stWinPriv.stOutRect.s32X = 0;
        stWinPriv.stOutRect.s32Y = 0;
        if ((pstVirWin->u32Width == 0 && pstVirWin->u32Height == 0)
           || pstVirWin->u32Width > 1920 
           || pstVirWin->u32Height > 1088)
        {   
            stWinPriv.stOutRect.s32Height = 1080;
            stWinPriv.stOutRect.s32Width = 1920;
        }
        else
        {
            stWinPriv.stOutRect.s32Height = pstVirWin->u32Height;
            stWinPriv.stOutRect.s32Width = pstVirWin->u32Width;
        }
        stWinPriv.stCropRect = pstVirWin->stAttrBuf.stCropRect;
        stWinPriv.stInRect = pstVirWin->stAttrBuf.stInRect;
        stWinPriv.bUseCropRect = pstVirWin->stAttrBuf.bUseCropRect;
        
        stWinPriv.stScreen = stWinPriv.stOutRect;
        
        stWinPriv.stScreenAR.u8ARh = 0;
        stWinPriv.stScreenAR.u8ARw = 0;
        
        stWinPriv.stCustmAR = pstVirWin->stAttrBuf.stCustmAR;
        stWinPriv.enARCvrs = pstVirWin->stAttrBuf.enARCvrs;

        
        
        pstVirWin->stSrcInfo.pfSendWinInfo(pstVirWin->stSrcInfo.hSrc,&stWinPriv);
    }
    else
    {

    }
    return HI_SUCCESS;
}
HI_S32 WIN_VIR_SetAttr(VIRTUAL_S *pstVirWin,HI_DRV_WIN_ATTR_S *pWinAttr)
{
    atomic_set(&pstVirWin->bNewAttrFlag, 0);
        
    pstVirWin->stAttrBuf = *pWinAttr;

    atomic_set(&pstVirWin->bNewAttrFlag, 1);

    
    WIN_VIR_SendAttrToSource(pstVirWin);
    
    return HI_SUCCESS;
}
HI_S32 WIN_VIR_SetSize(VIRTUAL_S *pstVirWin,HI_U32 u32Width,HI_U32 u32Height)
{
    
    if (u32Width > 1920 || u32Height > 1088
       || u32Width < 64 || u32Height < 64)
    {
        WIN_ERROR("Set Virtual Win Invalid H %d and W %d ,only support 64*64 ~ 1280*720.\n",
                    u32Height,u32Width);
        return HI_FAILURE;
    }
    pstVirWin->u32Width = u32Width;
    pstVirWin->u32Height = u32Height;
    
    WIN_VIR_SendAttrToSource(pstVirWin);

    return HI_SUCCESS;
}

HI_S32 WIN_VIR_GetFrm(VIRTUAL_S *pstVirWin,HI_DRV_VIDEO_FRAME_S *pstFrm)
{
    HI_S32  s32Ret = HI_FAILURE;

    switch(pstVirWin->enBufType)
    {
        case MUTUAL_TYPE_SRCACTIVE:
            WIN_ERROR("MUTUAL_TYPE_SRCACTIVE Can't be supported.\n");
            break;
        case MUTUAL_TYPE_SINKACTIVE:
            s32Ret = VIR_BUFFER_GetFrm(&(pstVirWin->stBuffer), pstFrm);
            break;
        default:
            WIN_ERROR("MUTUAL TYPE Can't be supported.\n");
            break;
    }

    return s32Ret;
}
HI_S32 WIN_VIR_RelFrm(VIRTUAL_S *pstVirWin,HI_DRV_VIDEO_FRAME_S *pstFrm)
{
    HI_S32  s32Ret;
    HI_HANDLE hSrc;
    
    hSrc = pstVirWin->stSrcInfo.hSrc;

    WinCheckNullPointer(pstVirWin->stSrcInfo.pfRlsFrame);
    
    s32Ret = pstVirWin->stSrcInfo.pfRlsFrame(hSrc,pstFrm);
    
    return s32Ret;

}

/*AVPLAY put in frame*/
HI_S32 WIN_VIR_AddNewFrm(VIRTUAL_S *pstVirWin,HI_DRV_VIDEO_FRAME_S *pstFrm)
{
    HI_S32  s32Ret;
    HI_HANDLE hSink;
    HI_HANDLE hSrc;
    FN_VENC_PUT_FRAME pfnQueueFrm;

    if (pstVirWin->bEnable == HI_TRUE)
    {
        switch(pstVirWin->enBufType)
        {
            case MUTUAL_TYPE_SRCACTIVE:
                hSink = pstVirWin->hSink;
                WinCheckNullPointer(pstVirWin->pfnQueueFrm);
                pfnQueueFrm = (FN_VENC_PUT_FRAME)pstVirWin->pfnQueueFrm;
                
                s32Ret = pfnQueueFrm(hSink,pstFrm);

                break;
            case MUTUAL_TYPE_SINKACTIVE:
                s32Ret = VIR_BUFFER_AddFrm(&(pstVirWin->stBuffer), pstFrm);
           
                break;
            default:
                WIN_ERROR("MUTUAL TYPE Can't be supported.\n");
                return HI_FAILURE;
                break;
        }
    }
    else
    {
        s32Ret = HI_FAILURE;
    }
    
    if (HI_FAILURE == s32Ret)
    {
        hSrc = pstVirWin->stSrcInfo.hSrc;
        WinCheckNullPointer(pstVirWin->stSrcInfo.pfRlsFrame);
        s32Ret = pstVirWin->stSrcInfo.pfRlsFrame(hSrc, pstFrm);
        s32Ret = HI_SUCCESS;
    }

    return s32Ret;
}


HI_S32 WIN_VIR_AddUlsFrm(VIRTUAL_S *pstVirWin,HI_DRV_VIDEO_FRAME_S *pstFrm)
{
    HI_S32  s32Ret;
    HI_HANDLE hSrc;

    hSrc = pstVirWin->stSrcInfo.hSrc;
    WinCheckNullPointer(pstVirWin->stSrcInfo.pfRlsFrame);
    s32Ret = pstVirWin->stSrcInfo.pfRlsFrame(hSrc, pstFrm);
       
    return s32Ret;
}


HI_BOOL WinCheckVirtual(HI_U32 u32WinIndex)
{
    if (WIN_INDEX_PREFIX != WinGetPrefix(u32WinIndex))
    {
        WIN_ERROR("Invalid Handle.\n");
        return HI_FALSE;
    }

    if (WIN_INDEX_VIRTUAL_CHANNEL == WinGetDispId(u32WinIndex))
    {
        return HI_TRUE;
    }
    else
    {
        return HI_FALSE;
    }    
}

HI_S32 WIN_VIR_DetachSink(VIRTUAL_S *pstVirWin,HI_HANDLE hSink)
{
    HI_MOD_ID_E enModID;
    HI_S32 s32Ret;
    
    enModID = (HI_MOD_ID_E)((hSink & 0xff0000) >> 16);

    if (hSink == pstVirWin->hSink)
    {
        pstVirWin->hSink = HI_INVALID_HANDLE;
        pstVirWin->pfnQueueFrm = HI_NULL;
        pstVirWin->pfnDequeueFrame = HI_NULL;

        pstVirWin->enBufType = MUTUAL_TYPE_SINKACTIVE;
        pstVirWin->stBuffer.enType = MUTUAL_TYPE_SINKACTIVE;
        s32Ret = HI_SUCCESS;
    }
    else
    {
        WIN_ERROR("Invalid Detach Handle.\n");
        s32Ret = HI_FAILURE;
    }

    
    return s32Ret;
    
}

HI_S32 WIN_VIR_AttachSink(VIRTUAL_S *pstVirWin,HI_HANDLE hSink)
{
    HI_MOD_ID_E enModID;
    VENC_EXPORT_FUNC_S *pstVenFunc;
    HI_S32 s32Ret;
    
    enModID = (HI_MOD_ID_E)((hSink & 0xff0000) >> 16);
    
    if ( HI_ID_VENC == enModID )
    {
        s32Ret = HI_DRV_MODULE_GetFunction(enModID,(HI_VOID**)&(pstVenFunc));

        if (HI_SUCCESS != s32Ret)
        {
            WIN_ERROR("get venc func error when in virtual win.\n");
            return HI_FAILURE;
        }

        pstVirWin->hSink = hSink;
        pstVirWin->pfnQueueFrm = pstVenFunc->pfnVencQueueFrame;
        pstVirWin->pfnDequeueFrame = HI_NULL;
        pstVirWin->enBufType = MUTUAL_TYPE_SRCACTIVE;
        pstVirWin->stBuffer.enType = MUTUAL_TYPE_SRCACTIVE;
        s32Ret = HI_SUCCESS;
    }
    else
    {
       s32Ret = HI_FAILURE;
    }

    return s32Ret;
    
}
#ifdef __cplusplus
 #if __cplusplus
}
 #endif
#endif /* __cplusplus */
