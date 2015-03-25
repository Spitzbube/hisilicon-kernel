
/******************************************************************************
  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.
******************************************************************************
File Name     : drv_window.h
Version       : Initial Draft
Author        : Hisilicon multimedia software group
Created       : 2012/12/30
Last Modified :
Description   :
Function List :
History       :
******************************************************************************/

#include "drv_display.h"

#include "drv_window.h"
#include "drv_win_priv.h"
#include "hi_drv_sys.h"
#include "drv_vdec_ext.h"
#include "drv_disp_hal.h"




#ifdef __cplusplus
 #if __cplusplus
extern "C" {
 #endif
#endif /* __cplusplus */

/******************************************************************************
    global object
******************************************************************************/
static volatile HI_S32 s_s32WindowGlobalFlag = WIN_DEVICE_STATE_CLOSE;
static DISPLAY_WINDOW_S stDispWindow;
static VIRTUAL_WINDOW_S stVirWindow;
VIRTUAL_S *WinGetVirWindow(HI_U32 u32WinIndex)
{
    
    if (!stVirWindow.u32WinNumber)
    {
        WIN_WARN("Not found this window!\n");
        return HI_NULL;
    }

    if ( WinGetPrefix(u32WinIndex) != WIN_INDEX_PREFIX)
    {
        WIN_ERROR("Invalid window index = 0x%x\n", u32WinIndex);
        return HI_NULL;
    }

    if (  WinGetDispId(u32WinIndex) != WIN_INDEX_VIRTUAL_CHANNEL)
    {
        WIN_ERROR("Invalid window index = 0x%x\n", u32WinIndex);
        return HI_NULL;
    }

    if ( WinGetId(u32WinIndex) >= WIN_VIRTAUL_MAX_NUMBER)
    {
        WIN_ERROR("Invalid window index = 0x%x\n", u32WinIndex);
        return HI_NULL;
    }

    return stVirWindow.pstWinArray[WinGetId(u32WinIndex)];
}

HI_VOID ISR_CallbackForDispModeChange(HI_HANDLE hDst, const HI_DRV_DISP_CALLBACK_INFO_S *pstInfo);
HI_VOID ISR_CallbackForWinProcess(HI_HANDLE hDst, const HI_DRV_DISP_CALLBACK_INFO_S *pstInfo);

/******************************************************************************
    Win device function
******************************************************************************/

HI_S32 Check_WinFunc(WINDOW_S *pWin)
{
    if (!pWin)
        return HI_FAILURE;

    if ((!pWin->stVLayerFunc.PF_ReleaseLayer)
        ||(!pWin->stVLayerFunc.PF_SetEnable)
        ||(!pWin->stVLayerFunc.PF_Update)
        ||(!pWin->stVLayerFunc.PF_AcquireLayerByDisplay)
        ||(!pWin->stVLayerFunc.PF_MovBottom)
        ||(!pWin->stVLayerFunc.PF_MovTop)
        ||(!pWin->stVLayerFunc.PF_AcquireLayerByDisplay)
        )
    {
        return HI_FAILURE;
        
    }
    else
    {
        return HI_SUCCESS;
    }
}

HI_VOID ISR_CallbackForWinManage(HI_HANDLE hDst, const HI_DRV_DISP_CALLBACK_INFO_S *pstInfo)
{
    HI_U32 i = 0,j = 0;
    WINDOW_S *pWin[WINDOW_MAX_NUMBER];
    HI_U32 u32WinRange[WINDOW_MAX_NUMBER];
    HI_U32 u32WinOrder[WINDOW_MAX_NUMBER];
    HI_U32 u32MaxWinRange = 0;
    HI_U32 u32MaxWinNum = 0;
    HI_U32 u32MainWinIndex;
    VIDEO_LAYER_CAPABILITY_S  stVideoLayerCap;
    HI_DRV_VIDEO_FRAME_S  *pConfigFrame;

    memset(u32WinRange,0,sizeof(HI_U32)*WINDOW_MAX_NUMBER);
    memset(pWin,0,sizeof(WINDOW_S*)*WINDOW_MAX_NUMBER);
    memset(u32WinOrder,0,sizeof(HI_U32)*WINDOW_MAX_NUMBER);
    
    for ( i = 0 ; i < WINDOW_MAX_NUMBER; i++)
    {
        if (stDispWindow.pstWinArray[(HI_U32)hDst][i])
        {
            pWin[j] = stDispWindow.pstWinArray[(HI_U32)hDst][i];
            /*check window opt is right */
            if (HI_SUCCESS == Check_WinFunc(pWin[j]))
            {
                 pConfigFrame = WinBuf_GetDisplayedFrame(&pWin[j]->stBuffer.stWinBP);
                 if (pConfigFrame)
                 {
                    /*save window Zorder*/
                    pWin[j]->stVLayerFunc.PF_GetZorder(pWin[j]->u32VideoLayer,&u32WinOrder[i]);
                    
                    /*save window size*/
                    u32WinRange[i] = (pConfigFrame->u32Width) * (pConfigFrame->u32Height);
                    j++;
                }
             }
        }
    }

    u32MaxWinNum = j;
       /* if no window return */
    if ( u32MaxWinNum<= 0)
    {
        return ;
    }

    /*find  largest window as MainWindow*/
    u32MaxWinRange = u32WinRange[0];
    u32MainWinIndex = 0;
    for ( i = 0 ; i < u32MaxWinNum; i++)
    {
        if (u32WinRange[i] > u32MaxWinRange)
        {
            u32MaxWinRange = u32WinRange[i];
            u32MainWinIndex = i ;
        }
    }

    /*1: judge whether need to change window video  layer*/
    pWin[u32MainWinIndex]->stVLayerFunc.PF_GetCapability(pWin[u32MainWinIndex]->u32VideoLayer,&stVideoLayerCap);
    if ((!stVideoLayerCap.bZme ) && (u32WinRange[u32MainWinIndex]))
    {    
        /*if main window is not ZME layer,we need change layer*/
        if (1 == u32MaxWinNum)
         {
                
            /*when  change  one layer to other layer , we should close old layer*/
                pWin[u32MainWinIndex]->stVLayerFunc.PF_SetEnable(pWin[u32MainWinIndex]->u32VideoLayer,HI_FALSE);
                pWin[u32MainWinIndex]->stVLayerFunc.PF_Update(pWin[u32MainWinIndex]->u32VideoLayer);
         }
        
        /*2: change  layer */ 
        /*release all layer*/
        for ( i = 0 ; i < u32MaxWinNum; i++)
        {
            if (pWin[i])
            {
                    pWin[i]->stVLayerFunc.PF_ReleaseLayer(pWin[i]->u32VideoLayer);
            }
        }

        /*make sure that main window Acquire ZME layer */
          //printk("manage window  change max (%d)main  (%d) layer(%d)\n ",u32MaxWinNum,u32MainWinIndex,(pWin[u32MainWinIndex]->u32VideoLayer));
         pWin[u32MainWinIndex]->stVLayerFunc.PF_AcquireLayerByDisplay( (HI_DRV_DISPLAY_E) hDst,&(pWin[u32MainWinIndex]->u32VideoLayer));

         for ( i = 0 ; i < u32MaxWinNum; i++)
        {
            if ((u32MainWinIndex != i ) && pWin[i])
                pWin[i]->stVLayerFunc.PF_AcquireLayerByDisplay((HI_DRV_DISPLAY_E)  hDst,&(pWin[i]->u32VideoLayer));
        }

         /*resume  window zorder,because we only have two layers ,so we can this*/
         if (u32WinOrder[u32MainWinIndex] == 0)
            pWin[u32MainWinIndex]->stVLayerFunc.PF_MovBottom(pWin[u32MainWinIndex]->u32VideoLayer);
         else
            pWin[u32MainWinIndex]->stVLayerFunc.PF_MovTop(pWin[u32MainWinIndex]->u32VideoLayer);
         
         pWin[u32MainWinIndex]->stVLayerFunc.PF_Update(pWin[u32MainWinIndex]->u32VideoLayer);

    }

}

HI_S32 WinRegWinManageCallback(HI_DRV_DISPLAY_E enDisp)
{
    HI_DRV_DISP_CALLBACK_S stCB;
    HI_S32 nRet= HI_SUCCESS;    

    stCB.hDst  = (HI_HANDLE)enDisp;
    stCB.pfDISP_Callback = ISR_CallbackForWinManage;
    nRet = DISP_RegCallback(enDisp, HI_DRV_DISP_C_INTPOS_0_PERCENT, &stCB);
    if (nRet)
    {
        WIN_ERROR("WIN register callback failed in %s!\n", __FUNCTION__);
    }

    return nRet;
}
HI_S32 WinUnRegWinManageCallback(HI_DRV_DISPLAY_E enDisp)
{
    HI_DRV_DISP_CALLBACK_S stCB;
    HI_S32 nRet= HI_SUCCESS;    

    stCB.hDst  = (HI_HANDLE)enDisp;
    stCB.pfDISP_Callback = ISR_CallbackForWinManage;
    nRet = DISP_UnRegCallback(enDisp, HI_DRV_DISP_C_INTPOS_0_PERCENT, &stCB);
    if (nRet)
    {
        WIN_ERROR("WIN unregister callback failed in %s!\n", __FUNCTION__);
    }
    return nRet;
}

HI_S32 WIN_DestroyStillFrame(HI_DRV_VIDEO_FRAME_S *pstReleaseFrame)
{
    HI_U32 i;
    WIN_RELEASE_FRM_S* pstWinRelFrame = &stDispWindow.stWinRelFrame;
    
    for ( i =0; i < MAX_RELEASE_NO; i++)
    {
        if (!pstWinRelFrame->pstNeedRelFrmNode[i] )
        {
            pstWinRelFrame->pstNeedRelFrmNode[i] = pstReleaseFrame;
            pstWinRelFrame->enThreadEvent= EVENT_RELEASE;
            //printk(" wake up !\n");
            wake_up(&pstWinRelFrame->stWaitQueHead);
            return HI_SUCCESS;   
        }
    }
    
    WIN_ERROR("Release still frame failed ,buff is full. %s!\n", __FUNCTION__); 
    return HI_FAILURE;   
}
HI_S32 WinReleaseFrameThreadProcess(HI_VOID* pArg)
{   
    HI_U32 i;
    WIN_RELEASE_FRM_S* pstWinRelFrame = pArg;

    /*if stop refush release frame buffer*/
    while(!kthread_should_stop() )
    {
        for ( i =0; i < MAX_RELEASE_NO; i++)
        {
            if (pstWinRelFrame->pstNeedRelFrmNode[i]  )
            {
                if ( pstWinRelFrame->pstNeedRelFrmNode[i] ->bStillFrame)
                {
                    WinReleaseStillFrame(pstWinRelFrame->pstNeedRelFrmNode[i]);
                    pstWinRelFrame->pstNeedRelFrmNode[i] = HI_NULL;
                }
            }
        }

        //printk("run  --%d!\n",pstWinRelFrame->enThreadEvent);
        pstWinRelFrame->enThreadEvent = EVENT_BUTT;
        //wait_event_timeout(stDispWindow.stQueueHead.queue_head, stDispWindow.enThreadEvent,HZ);
        wait_event(pstWinRelFrame->stWaitQueHead, (EVENT_RELEASE == pstWinRelFrame->enThreadEvent));
    }
    
    return HI_SUCCESS;
}
HI_S32 WinCreatReleaseFrameThread(HI_VOID )
{
    WIN_RELEASE_FRM_S* pstWinRelFrame = &stDispWindow.stWinRelFrame;
    
    memset(pstWinRelFrame,0,sizeof(WIN_RELEASE_FRM_S));
    pstWinRelFrame->hThread =  kthread_create(WinReleaseFrameThreadProcess, (HI_VOID *)(&stDispWindow.stWinRelFrame), "HI_WIN_ReleaseFrameProcess");
    
    init_waitqueue_head( &(pstWinRelFrame->stWaitQueHead) );
    if (HI_NULL == pstWinRelFrame->hThread)
    {
        WIN_FATAL("Can not create thread.\n");
        return HI_FAILURE;
    }
    wake_up_process(pstWinRelFrame->hThread);
    return HI_SUCCESS;
}

HI_S32 WinDestroyReleaseFrameThread(HI_VOID )
{
    HI_S32 s32Ret;
    HI_S32 s32Times = 0;
    WIN_RELEASE_FRM_S* pstWinRelFrame = &stDispWindow.stWinRelFrame;

    /*reflush release buffer */
    pstWinRelFrame->enThreadEvent = EVENT_RELEASE;
    wake_up(&pstWinRelFrame->stWaitQueHead);

    for (s32Times = 0 ; s32Times < 10; s32Times++)
    {
         if (EVENT_BUTT == pstWinRelFrame->enThreadEvent) 
         {
            pstWinRelFrame->enThreadEvent = EVENT_RELEASE;
            s32Ret = kthread_stop(pstWinRelFrame->hThread);
             
            if (s32Ret != HI_SUCCESS)
            {
                WIN_FATAL("Destory Thread Error.\n");
            }
            return HI_SUCCESS;
        }
         msleep(100);
    }
    
    return HI_FAILURE;
}

HI_S32 WIN_Init(HI_VOID)
{
    HI_BOOL bDispInitFlag;
    HI_S32 s32Ret;
    HI_DRV_DISP_VERSION_S stVerison;
    
    if (WIN_DEVICE_STATE_CLOSE != s_s32WindowGlobalFlag)
    {
        WIN_INFO("VO has been inited!\n");
        return HI_SUCCESS;
    }

    DISP_GetInitFlag(&bDispInitFlag);
    if (HI_TRUE != bDispInitFlag)
    {
        WIN_ERROR("Display is not inited!\n");
        return HI_ERR_VO_DEPEND_DEVICE_NOT_READY;
    }

    if (HI_SUCCESS != BP_CreateBlackFrame())
    {
        WIN_ERROR("Create Black Frame failed!\n");
        return HI_ERR_VO_MALLOC_FAILED;
    }

    DISP_MEMSET(&stDispWindow, 0, sizeof(DISPLAY_WINDOW_S));
    
    DISP_GetVersion(&stVerison);
    VideoLayer_Init(&stVerison);

    DISP_MEMSET(&stVirWindow, 0, sizeof(VIRTUAL_WINDOW_S));
    
    WinRegWinManageCallback(HI_DRV_DISPLAY_0);
    WinRegWinManageCallback(HI_DRV_DISPLAY_1);
    
    s32Ret = WinCreatReleaseFrameThread();
    if (HI_SUCCESS != s32Ret)
    {
        WIN_ERROR("win Create Release Frame Thread failed!\n");
        return HI_ERR_VO_MALLOC_FAILED;
    }
    s_s32WindowGlobalFlag = WIN_DEVICE_STATE_OPEN;

    return HI_SUCCESS;
}


HI_S32 WIN_DeInit(HI_VOID)
{
    HI_S32 i,j,s32Ret;

    if (WIN_DEVICE_STATE_CLOSE == s_s32WindowGlobalFlag)
    {
        WIN_INFO("VO is not inited!\n");
        return HI_SUCCESS;
    }
    /*close all the windows.*/
    for(i=0; i<HI_DRV_DISPLAY_BUTT; i++)
    {
        for(j=0; j<WINDOW_MAX_NUMBER; j++)
        {
            if (stDispWindow.pstWinArray[i][j])
            {
                WIN_Destroy(stDispWindow.pstWinArray[i][j]->u32Index);
                stDispWindow.pstWinArray[i][j] = HI_NULL;
            }
        }
    }
    stDispWindow.u32WinNumber = 0;

    
    /*close all the virtual windows.*/
    for(i=0; i<HI_DRV_DISPLAY_BUTT; i++)
    {
        if (stVirWindow.pstWinArray[i])
        {
            WIN_VIR_Destroy(stVirWindow.pstWinArray[i]);
            stVirWindow.pstWinArray[i] = HI_NULL;
        }
    }
    
    stVirWindow.u32WinNumber = 0;  

    WinUnRegWinManageCallback(HI_DRV_DISPLAY_1);
    WinUnRegWinManageCallback(HI_DRV_DISPLAY_0);
    
    s32Ret = WinDestroyReleaseFrameThread();
    if (HI_SUCCESS != s32Ret)
    {
        WIN_ERROR("win Destroy Release Frame Thread failed!\n");
        //return HI_ERR_VO_MALLOC_FAILED;
    }
    VideoLayer_DeInit();
    BP_DestroyBlackFrame();  
    s_s32WindowGlobalFlag = WIN_DEVICE_STATE_CLOSE;
    
    WIN_INFO("VO has been DEinited!\n");
    return HI_SUCCESS;
}

HI_S32 WIN_Suspend(HI_VOID)
{
    WinCheckDeviceOpen();

    s_s32WindowGlobalFlag = WIN_DEVICE_STATE_SUSPEND;

    return HI_SUCCESS;
}

HI_S32 WIN_Resume(HI_VOID)
{

    if (s_s32WindowGlobalFlag == WIN_DEVICE_STATE_SUSPEND)
    {
        VIDEO_LAYER_FUNCTIONG_S *pF = VideoLayer_GetFunctionPtr();

        s_s32WindowGlobalFlag = WIN_DEVICE_STATE_OPEN;
        if (pF)
            pF->PF_SetAllLayerDefault();
    }

    return HI_SUCCESS;
}

HI_S32 WinTestAddWindow(HI_VOID)
{
    if (stDispWindow.u32WinNumber < WINDOW_MAX_NUMBER)
    {
        return HI_SUCCESS;
    }
    else
    {
        return HI_FAILURE;
    }
}

HI_U32 WinGetPrefix(HI_U32 u32WinIndex)
{
    return (HI_U32)(u32WinIndex & WIN_INDEX_PREFIX_MASK);
}

HI_U32 WinGetDispId(HI_U32 u32WinIndex)
{
    return (HI_U32)((u32WinIndex >> WIN_INDEX_DISPID_SHIFT_NUMBER) & WIN_INDEX_DISPID_MASK);
}

HI_U32 WinGetId(HI_U32 u32WinIndex)
{
    return (HI_U32)(u32WinIndex & WINDOW_INDEX_NUMBER_MASK);
}



HI_U32 WinMakeVirIndex(HI_U32 u32WinIndex)
{
    /*
     *  0x12               34                        56             78
     *       WIN_INDEX_PREFIX  WIN_INDEX_VIRTUAL_CHANNEL   u32WinIndex  
     *
     */
    return (HI_U32)(   WIN_INDEX_PREFIX
                     | ( ( WIN_INDEX_VIRTUAL_CHANNEL& WIN_INDEX_DISPID_MASK) \
                          << WIN_INDEX_DISPID_SHIFT_NUMBER
                       )
                     |(u32WinIndex& WINDOW_INDEX_NUMBER_MASK)
                    );
}

HI_U32 WinMakeIndex(HI_DRV_DISPLAY_E enDisp, HI_U32 u32WinIndex)
{
    return (HI_U32)(   (WIN_INDEX_PREFIX)
                     | ( ( (HI_U32)enDisp & WIN_INDEX_DISPID_MASK) \
                          << WIN_INDEX_DISPID_SHIFT_NUMBER
                       )
                     |(u32WinIndex& WINDOW_INDEX_NUMBER_MASK)
                    );
}

HI_U32 WinGetIndex(HI_HANDLE hWin, HI_DRV_DISPLAY_E *enDisp, HI_U32 *u32WinIndex)
{
    
    *enDisp = (hWin & 0xff00) >> WIN_INDEX_DISPID_SHIFT_NUMBER;
    *u32WinIndex = hWin & 0xff;

    return HI_SUCCESS;
}

HI_S32 WinAddVirWindow(VIRTUAL_S *pstWin)
{
    HI_S32 i;
    
    for(i=0; i<WIN_VIRTAUL_MAX_NUMBER; i++)
    {
        if (!stVirWindow.pstWinArray[i])
        {
            pstWin->u32Index =  WinMakeVirIndex((HI_U32)i);
            stVirWindow.pstWinArray[i] = pstWin;
            stVirWindow.u32WinNumber++;

            return HI_SUCCESS;
        }
    }
    return HI_FAILURE;
}
HI_S32 WinDelVirWindow(HI_U32 u32WinIndex)
{
    if ( WinGetPrefix(u32WinIndex) != WIN_INDEX_PREFIX)
    {
        WIN_ERROR("Invalid window index = 0x%x\n", u32WinIndex);
        return HI_FAILURE;
    }

    if ( WinGetDispId(u32WinIndex) != WIN_INDEX_VIRTUAL_CHANNEL)
    {
        WIN_ERROR("Invalid window index = 0x%x\n", u32WinIndex);
        return HI_FAILURE;
    }

    if ( WinGetId(u32WinIndex) >= WIN_VIRTAUL_MAX_NUMBER)
    {
        WIN_ERROR("Invalid window index = 0x%x\n", u32WinIndex);
        return HI_FAILURE;
    }

    if (!stVirWindow.u32WinNumber)
    {
        WIN_ERROR("Not found this window!\n");
        return HI_FAILURE;
    }

    if (stVirWindow.pstWinArray[WinGetId(u32WinIndex)])
    {
        stVirWindow.pstWinArray[WinGetId(u32WinIndex)] = HI_NULL;
        stVirWindow.u32WinNumber--;
    }
    else
    {
        WIN_ERROR("Not found this window!\n");
    }

    return HI_SUCCESS;
}


HI_S32 WinAddWindow(HI_DRV_DISPLAY_E enDisp, WINDOW_S *pstWin)
{
    HI_S32 i;

    if (enDisp >= HI_DRV_DISPLAY_BUTT)
    {
        return HI_FAILURE;
    }

    for(i=0; i<WINDOW_MAX_NUMBER; i++)
    {
        if (!stDispWindow.pstWinArray[(HI_U32)enDisp][i])
        {
            pstWin->u32Index =  WinMakeIndex(enDisp, (HI_U32)i);

            stDispWindow.pstWinArray[(HI_U32)enDisp][i] = pstWin;
            stDispWindow.u32WinNumber++;
            
            return HI_SUCCESS;
        }
    }    

    return HI_FAILURE;
}



HI_S32 WinDelWindow(HI_U32 u32WinIndex)
{
    if ( WinGetPrefix(u32WinIndex) != WIN_INDEX_PREFIX)
    {
        WIN_ERROR("Invalid window index = 0x%x\n", u32WinIndex);
        return HI_FAILURE;
    }

    if (  WinGetDispId(u32WinIndex) >= HI_DRV_DISPLAY_BUTT)
    {
        WIN_ERROR("Invalid window index = 0x%x\n", u32WinIndex);
        return HI_FAILURE;
    }

    if ( WinGetId(u32WinIndex) >= WINDOW_MAX_NUMBER)
    {
        WIN_ERROR("Invalid window index = 0x%x\n", u32WinIndex);
        return HI_FAILURE;
    }

    if (!stDispWindow.u32WinNumber)
    {
        WIN_ERROR("Not found this window!\n");
        return HI_FAILURE;
    }

    if (stDispWindow.pstWinArray[WinGetDispId(u32WinIndex)][WinGetId(u32WinIndex)])
    {
        stDispWindow.pstWinArray[WinGetDispId(u32WinIndex)][WinGetId(u32WinIndex)] = HI_NULL;
        stDispWindow.u32WinNumber--;
    }
    else
    {
        WIN_ERROR("Not found this window!\n");
    }

    return HI_SUCCESS;
}


WINDOW_S *WinGetWindow(HI_U32 u32WinIndex)
{
    if (!stDispWindow.u32WinNumber)
    {
        WIN_WARN("Not found this window!\n");
        return HI_NULL;
    }
    
    if ( WinGetPrefix(u32WinIndex) != WIN_INDEX_PREFIX)
    {
        WIN_ERROR("Invalid window index = 0x%x\n", u32WinIndex);
        return HI_NULL;
    }

    if (  WinGetDispId(u32WinIndex) >= HI_DRV_DISPLAY_BUTT)
    {
        WIN_WARN("Invalid window index = 0x%x\n", u32WinIndex);
        return HI_NULL;
    }

    if ( WinGetId(u32WinIndex) >= WINDOW_MAX_NUMBER)
    {
        WIN_ERROR("Invalid window index = 0x%x\n", u32WinIndex);
        return HI_NULL;
    }

    return stDispWindow.pstWinArray[WinGetDispId(u32WinIndex)][WinGetId(u32WinIndex)];
}



/******************************************************************************
    internal function
******************************************************************************/
HI_U32 WinParamAlignUp(HI_U32 x, HI_U32 a)
{
    if (!a)
    {
        return x;
    }
    else
    {
        return ( (( x + (a-1) ) / a ) * a);
    }
}

HI_U32 WinParamAlignDown(HI_U32 x, HI_U32 a)
{
    if (!a)
    {
        return x;
    }
    else
    {
        return (( x / a ) * a);
    }
}

HI_VOID window_revise(HI_RECT_S *stToBeRevisedRect_tmp, const HI_RECT_S *tmp_virtscreen)
{
    /*give a basic  process of width and height.*/
    if (stToBeRevisedRect_tmp->s32Width < WIN_OUTRECT_MIN_WIDTH)
        stToBeRevisedRect_tmp->s32Width = WIN_OUTRECT_MIN_WIDTH;
    else if(stToBeRevisedRect_tmp->s32Width > tmp_virtscreen->s32Width)
        stToBeRevisedRect_tmp->s32Width =  tmp_virtscreen->s32Width;

    if (stToBeRevisedRect_tmp->s32Height < WIN_OUTRECT_MIN_HEIGHT)
        stToBeRevisedRect_tmp->s32Height = WIN_OUTRECT_MIN_HEIGHT;
    else if(stToBeRevisedRect_tmp->s32Height > tmp_virtscreen->s32Height)
        stToBeRevisedRect_tmp->s32Height =  tmp_virtscreen->s32Height;   
    
    return ;
}

/*this func is for both graphics and video  virtual screen deal, it's a common function.*/
HI_S32 Win_ReviseOutRect(const HI_RECT_S *tmp_virtscreen, 
                         const HI_DRV_DISP_OFFSET_S *stOffsetInfo,
                         const HI_RECT_S *stFmtResolution,
                         HI_RECT_S *stToBeRevisedRect, 
                         HI_RECT_S *stRevisedRect)
{    
    HI_S32 width_ratio = 0,  height_ratio = 0;
    HI_U32 zmeDestWidth = 0, zmeDestHeight = 0;
    HI_DRV_DISP_OFFSET_S tmp_offsetInfo;

    HI_RECT_S stToBeRevisedRect_tmp = *stToBeRevisedRect;

    tmp_offsetInfo = *stOffsetInfo;
    
    /*for browse mode, revise in virt screen .*/
    window_revise(&stToBeRevisedRect_tmp,tmp_virtscreen);
    
    zmeDestWidth = (stFmtResolution->s32Width - tmp_offsetInfo.u32Left - tmp_offsetInfo.u32Right);
    zmeDestHeight = (stFmtResolution->s32Height - tmp_offsetInfo.u32Top - tmp_offsetInfo.u32Bottom);
    
    
    /*pay attention ,we must care about that  u32 overflow.....*/
    width_ratio  = zmeDestWidth  * 100 /(tmp_virtscreen->s32Width);
    height_ratio = zmeDestHeight * 100 /(tmp_virtscreen->s32Height);    

    if (tmp_virtscreen->s32Width != stToBeRevisedRect_tmp.s32Width)
    {
        stRevisedRect->s32Width = (stToBeRevisedRect_tmp.s32Width * width_ratio) / 100;         
    } else {
        stRevisedRect->s32Width = zmeDestWidth;
    }
    
    if (tmp_virtscreen->s32Height != stToBeRevisedRect_tmp.s32Height)
    {
        stRevisedRect->s32Height = (stToBeRevisedRect_tmp.s32Height * height_ratio) / 100;      
    } else {
        stRevisedRect->s32Height = zmeDestHeight;
    }
    
        
    stRevisedRect->s32X = (stToBeRevisedRect_tmp.s32X * width_ratio) /100 + tmp_offsetInfo.u32Left;
    stRevisedRect->s32Y= (stToBeRevisedRect_tmp.s32Y * height_ratio) /100 + tmp_offsetInfo.u32Top;
    
    stRevisedRect->s32X = WinParamAlignUp(stRevisedRect->s32X, 2);
    stRevisedRect->s32Y = WinParamAlignUp(stRevisedRect->s32Y, 2);
    stRevisedRect->s32Width  = WinParamAlignUp(stRevisedRect->s32Width, 2);
    stRevisedRect->s32Height = WinParamAlignUp(stRevisedRect->s32Height, 2);

    /*for browse mode, revise in virt screen .*/
    window_revise(stRevisedRect,stFmtResolution);    
    return HI_SUCCESS;
}


HI_S32 WinBufferReset(WIN_BUFFER_S *pstBuffer);
HI_VOID ISR_WinReleaseUSLFrame(WINDOW_S *pstWin);

HI_S32 WinCreateDisplayWindow(HI_DRV_WIN_ATTR_S *pWinAttr, WINDOW_S **ppstWin, HI_U32 u32BufNum)
{
    WINDOW_S *pstWin;
    HI_S32 nRet;

    if(WinTestAddWindow())
    {
        WIN_ERROR("Reach max window number,can not create!\n");
        return HI_ERR_VO_CREATE_ERR;
    }
 
    pstWin = (WINDOW_S *)DISP_MALLOC(sizeof(WINDOW_S));
    if (!pstWin)
    {
        WIN_ERROR("Malloc WINDOW_S failed in %s!\n", __FUNCTION__);
        return HI_ERR_VO_CREATE_ERR;
    }

    DISP_MEMSET(pstWin, 0, sizeof(WINDOW_S));
    
    /* attribute */
    pstWin->bEnable = HI_FALSE;
    pstWin->bMasked = HI_FALSE;

    pstWin->enState = WIN_STATE_WORK;
    //pstWin->enStateBackup = pstWin->enState;
    pstWin->bUpState      = HI_FALSE;

    pstWin->enDisp = pWinAttr->enDisp;
    pstWin->enType = HI_DRV_WIN_ACTIVE_SINGLE;

    pstWin->stCfg.stAttrBuf = *pWinAttr;
    atomic_set(&pstWin->stCfg.bNewAttrFlag, 1);

    pstWin->stCfg.enFrameCS = HI_DRV_CS_UNKNOWN;
    pstWin->stCfg.u32Fidelity = 0;
    pstWin->stCfg.enOutCS   = HI_DRV_CS_UNKNOWN;

    nRet = VideoLayer_GetFunction(&pstWin->stVLayerFunc);
    if (nRet)
    {
        WIN_ERROR("VideoLayer_GetFunction failed in %s!\n", __FUNCTION__);
        goto __ERR_GET_FUNC__;
    }
    
    /*get free layer.*/
    nRet = pstWin->stVLayerFunc.PF_AcquireLayerByDisplay(pstWin->enDisp, &pstWin->u32VideoLayer);
    if (nRet)
    {
        WIN_ERROR("PF_AcquireLayerByDisplay failed in %s!\n", __FUNCTION__);
        goto __ERR_GET_FUNC__;
    }

    nRet = pstWin->stVLayerFunc.PF_SetDefault(pstWin->u32VideoLayer);
    if (nRet)
    {
        WIN_ERROR("PF_SetDefault failed in %s!\n", __FUNCTION__);
        goto __ERR_GET_FUNC__;
    }
    
    pstWin->stVLayerFunc.PF_MovTop(pstWin->u32VideoLayer);
    
    nRet = WinBuf_Create(u32BufNum, WIN_BUF_MEM_SRC_SUPPLY, HI_NULL, &pstWin->stBuffer.stWinBP);
    if(nRet)
    {
        WIN_ERROR("Create buffer pool failed\n");
        goto __ERR_GET_FUNC__;
    }

    WinBufferReset(&pstWin->stBuffer);

    // initial reset
    pstWin->bReset = HI_FALSE;
    pstWin->bConfigedBlackFrame = HI_FALSE;

    // initial quickmode
    pstWin->bQuickMode = HI_FALSE;

    // initial stepmode flag
    pstWin->bStepMode = HI_FALSE;

    //init delay info
    pstWin->stDelayInfo.u32DispRate = 5000;
    pstWin->stDelayInfo.bTBMatch = HI_TRUE;
    pstWin->stDelayInfo.u32DisplayTime = 20;
    pstWin->bInInterrupt = HI_FALSE;

    *ppstWin = pstWin;

    return HI_SUCCESS;


__ERR_GET_FUNC__:

    DISP_FREE(pstWin);

    return HI_ERR_VO_CREATE_ERR;
}

HI_S32 WinDestroyDisplayWindow(WINDOW_S *pstWin)
{
    HI_DRV_VIDEO_FRAME_S *pstFrame;
    
    WinBuf_RlsAndUpdateUsingFrame(&pstWin->stBuffer.stWinBP);    
    ISR_WinReleaseUSLFrame(pstWin);

    // flush frame in full buffer pool
    pstFrame = WinBuf_GetDisplayedFrame(&pstWin->stBuffer.stWinBP);
    WinBuf_FlushWaitingFrame(&pstWin->stBuffer.stWinBP, pstFrame);

    // release current frame
    WinBuf_ForceReleaseFrame(&pstWin->stBuffer.stWinBP, pstFrame);

    // s1 derstoy buffer    
    WinBuf_Destroy(&pstWin->stBuffer.stWinBP);

    pstWin->stVLayerFunc.PF_ReleaseLayer(pstWin->u32VideoLayer);
    DISP_FREE(pstWin);    

    return HI_SUCCESS;
}

HI_S32 WinRegCallback(WINDOW_S *pstWin)
{
    HI_DRV_DISP_CALLBACK_S stCB;
    HI_S32 nRet= HI_SUCCESS;    

    stCB.hDst  = (HI_HANDLE)pstWin;
    stCB.pfDISP_Callback = ISR_CallbackForWinProcess;
    nRet = DISP_RegCallback(WinGetDispID(pstWin), HI_DRV_DISP_C_INTPOS_0_PERCENT, &stCB);
    if (nRet)
    {
        WIN_ERROR("WIN register callback failed in %s!\n", __FUNCTION__);
        return HI_ERR_VO_CREATE_ERR;
    }

    return HI_SUCCESS;
}

HI_S32 WinUnRegCallback(WINDOW_S *pstWin)
{
    HI_DRV_DISP_CALLBACK_S stCB;
    HI_S32 nRet= HI_SUCCESS;    

    stCB.hDst  = (HI_HANDLE)pstWin;
    stCB.pfDISP_Callback = ISR_CallbackForWinProcess;
    nRet = DISP_UnRegCallback(WinGetDispID(pstWin), HI_DRV_DISP_C_INTPOS_0_PERCENT, &stCB);

    return HI_SUCCESS;
}


HI_VOID WinSetBlackFrameFlag(WINDOW_S *pstWin)
{
    if (pstWin)
    {
        pstWin->bConfigedBlackFrame = HI_TRUE;
    }
}

HI_VOID WinClearBlackFrameFlag(WINDOW_S *pstWin)
{
    if (pstWin)
    {
        pstWin->bConfigedBlackFrame = HI_FALSE;
    }
}

HI_BOOL WinTestBlackFrameFlag(WINDOW_S *pstWin)
{
    if (pstWin)
    {
        return pstWin->bConfigedBlackFrame;
    }
    return HI_FALSE;
}

HI_S32 WinCheckFixedAttr(HI_DRV_WIN_ATTR_S *pOldAttr, HI_DRV_WIN_ATTR_S *pNewAttr)
{
    if (  (pOldAttr->enDisp != pNewAttr->enDisp)
        ||(pOldAttr->bVirtual != pNewAttr->bVirtual)
        )
    {
        return HI_FAILURE;
    }

    if (pOldAttr->bVirtual)
    {
        if (  (pOldAttr->bUserAllocBuffer != pNewAttr->bUserAllocBuffer)
            ||(pOldAttr->u32BufNumber != pNewAttr->u32BufNumber)
            )
        {
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

HI_S32 WinCheckAttr(HI_DRV_WIN_ATTR_S *pstAttr)
{  
    HI_RECT_S virtual_screen;
    HI_S32 ret = 0;
    
    if (pstAttr->enDisp > HI_DRV_DISPLAY_1)
    {
        WIN_FATAL("WIN only support HI_DRV_DISPLAY_0!\n");
        return HI_ERR_VO_INVALID_PARA;
    }

    ret = DISP_GetVirtScreen(pstAttr->enDisp, &virtual_screen);
    if (ret != HI_SUCCESS)
    {
        HI_ERR_WIN("Get Virtual SCREEN error!\n");
        return HI_FAILURE;
    }

    if (pstAttr->bVirtual == HI_TRUE)
    {
        if (   (pstAttr->stOutRect.s32Height > WIN_VIRTUAL_OUTRECT_MAX_HEIGHT)
            || (pstAttr->stOutRect.s32Width  > WIN_VIRTUAL_OUTRECT_MAX_WIDTH))
        {
            HI_ERR_WIN("Virtual win outrect is larger max width and height!\n");
            return HI_FAILURE;
        }
    }
    else
    {
        if (   (pstAttr->stOutRect.s32Height > virtual_screen.s32Height)
            || (pstAttr->stOutRect.s32Width > virtual_screen.s32Width))
        {
            HI_ERR_WIN("Win outrect is larger than virtual screen!\n");
            return HI_FAILURE;
        }
    }

    if ( ((pstAttr->stOutRect.s32Height == 0)
              && (pstAttr->stOutRect.s32Width != 0))
         || ((pstAttr->stOutRect.s32Height != 0)
              && (pstAttr->stOutRect.s32Width == 0)))
    {
        HI_ERR_WIN("win outrect error, one of w/h is zero.!\n");
        return HI_FAILURE;        
    } 
    else if ((!(pstAttr->stOutRect.s32Height | pstAttr->stOutRect.s32Width))
         && (pstAttr->stOutRect.s32X | pstAttr->stOutRect.s32Y))
    {
        HI_ERR_WIN("when w/h is zero, x/y should be zero too.!\n");
        return HI_FAILURE;        
    }
    else if((pstAttr->stOutRect.s32Height | pstAttr->stOutRect.s32Width)
        && ((pstAttr->stOutRect.s32Width   < WIN_OUTRECT_MIN_WIDTH)
            || (pstAttr->stOutRect.s32Height  < WIN_OUTRECT_MIN_HEIGHT))            
            )
    {
        WIN_FATAL("The Min WIN OutRect supported is 64*64 !\n");
        return HI_ERR_VO_INVALID_PARA;
    }
    
    if (   ( pstAttr->stCustmAR.u8ARw > (pstAttr->stCustmAR.u8ARh * WIN_MAX_ASPECT_RATIO) )
        || ( (pstAttr->stCustmAR.u8ARw * WIN_MAX_ASPECT_RATIO) < pstAttr->stCustmAR.u8ARh)
        )
    {
        HI_ERR_WIN("bUserDefAspectRatio  error!\n");
        return HI_FAILURE;
    }

    if ( pstAttr->bUseCropRect)
    {
        if(    (pstAttr->stCropRect.u32TopOffset    > WIN_CROPRECT_MAX_OFFSET_TOP)
            || (pstAttr->stCropRect.u32LeftOffset   > WIN_CROPRECT_MAX_OFFSET_LEFT)
            || (pstAttr->stCropRect.u32BottomOffset > WIN_CROPRECT_MAX_OFFSET_BOTTOM)
            || (pstAttr->stCropRect.u32RightOffset  > WIN_CROPRECT_MAX_OFFSET_RIGHT)
           )
        {
            WIN_FATAL("WIN CropRec support less than 128!\n");
            return HI_ERR_VO_INVALID_PARA;
        }
    }
    else
    {
        if ( !pstAttr->stInRect.s32Height || !pstAttr->stInRect.s32Width)
        {
            DISP_MEMSET(&pstAttr->stInRect, 0, sizeof(HI_RECT_S));
        }
        else if(   (pstAttr->stInRect.s32Width   < WIN_FRAME_MIN_WIDTH)
                || (pstAttr->stInRect.s32Height  < WIN_FRAME_MIN_HEIGHT)
                || (pstAttr->stInRect.s32Width   > WIN_FRAME_MAX_WIDTH)
                || (pstAttr->stInRect.s32Height  > WIN_FRAME_MAX_HEIGHT)
                )
        {
            WIN_FATAL("WIN InRect support 64*64 ~ 2560*1600!\n");
            return HI_ERR_VO_INVALID_PARA;
        }
    }
    

    /* may change when window lives */
    pstAttr->stInRect.s32X = pstAttr->stInRect.s32X & HI_WIN_IN_RECT_X_ALIGN;
    pstAttr->stInRect.s32Y = pstAttr->stInRect.s32Y & HI_WIN_IN_RECT_Y_ALIGN;
    pstAttr->stInRect.s32Width = pstAttr->stInRect.s32Width & HI_WIN_IN_RECT_WIDTH_ALIGN;
    pstAttr->stInRect.s32Height = pstAttr->stInRect.s32Height & HI_WIN_IN_RECT_HEIGHT_ALIGN;

    pstAttr->stCropRect.u32LeftOffset   = pstAttr->stCropRect.u32LeftOffset   & HI_WIN_IN_RECT_X_ALIGN;
    pstAttr->stCropRect.u32RightOffset  = pstAttr->stCropRect.u32RightOffset  & HI_WIN_IN_RECT_X_ALIGN;
    pstAttr->stCropRect.u32TopOffset    = pstAttr->stCropRect.u32TopOffset    & HI_WIN_IN_RECT_Y_ALIGN;
    pstAttr->stCropRect.u32BottomOffset = pstAttr->stCropRect.u32BottomOffset & HI_WIN_IN_RECT_Y_ALIGN;

    pstAttr->stOutRect.s32X = pstAttr->stOutRect.s32X & HI_WIN_OUT_RECT_X_ALIGN;
    pstAttr->stOutRect.s32Y = pstAttr->stOutRect.s32Y & HI_WIN_OUT_RECT_Y_ALIGN;
    pstAttr->stOutRect.s32Width  = pstAttr->stOutRect.s32Width  & HI_WIN_OUT_RECT_WIDTH_ALIGN;
    pstAttr->stOutRect.s32Height = pstAttr->stOutRect.s32Height & HI_WIN_OUT_RECT_HEIGHT_ALIGN;
    

    return HI_SUCCESS;
}


HI_S32 WinCheckSourceInfo(HI_DRV_WIN_SRC_INFO_S *pstSrc)
{
    if (!pstSrc->pfAcqFrame && !pstSrc->pfRlsFrame && !pstSrc->pfSendWinInfo)
    {
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}


HI_S32 WinSendAttrToSource(WINDOW_S *pstWin, HI_DISP_DISPLAY_INFO_S *pstDispInfo)
{
    if (pstWin->stCfg.stSource.pfSendWinInfo)
    {
        HI_DRV_WIN_PRIV_INFO_S stInfo;
        HI_DRV_WIN_ATTR_S *pstAttr;
        
        DISP_MEMSET(&stInfo, 0, sizeof(HI_DRV_WIN_PRIV_INFO_S));

        pstAttr = &pstWin->stUsingAttr;        
        
        stInfo.ePixFmt      = HI_DRV_PIX_FMT_NV21;
        stInfo.bUseCropRect = pstAttr->bUseCropRect;
        stInfo.stInRect     = pstAttr->stInRect;        
        stInfo.stCropRect   = pstAttr->stCropRect;

        if (!pstAttr->stOutRect.s32Width || !pstAttr->stOutRect.s32Height)        
            stInfo.stOutRect = pstDispInfo->stFmtResolution;        
        else        
            stInfo.stOutRect = pstAttr->stOutRect;
        

        stInfo.stScreenAR      = pstDispInfo->stAR;
        
        stInfo.stCustmAR    = pstAttr->stCustmAR;
        stInfo.enARCvrs     = pstAttr->enARCvrs;
        
        
        stInfo.bUseExtBuf   = pstAttr->bUserAllocBuffer;

        stInfo.u32MaxRate  = (pstDispInfo->u32RefreshRate > WIN_TRANSFER_CODE_MAX_FRAME_RATE) ?
                 (pstDispInfo->u32RefreshRate / 2 ) : pstDispInfo->u32RefreshRate;

        stInfo.bInterlaced = pstDispInfo->bInterlace;
        
       // stInfo.stScreen    = pstDispInfo->stFmtResolution; 
        if ((pstDispInfo->eDispMode != DISP_STEREO_NONE)
            &&(pstDispInfo->eDispMode < DISP_STEREO_BUTT))
        {
            stInfo.bIn3DMode = HI_TRUE;
        }
        else
        {
            stInfo.bIn3DMode = HI_FALSE;
        }
        
        /*when 3d mode ,set screen and OutRect as one eys*/
        pstWin->stVLayerFunc.PF_Get3DOutRect(pstDispInfo->eDispMode, &pstDispInfo->stFmtResolution, &stInfo.stScreen);
        pstWin->stVLayerFunc.PF_Get3DOutRect(pstDispInfo->eDispMode, &stInfo.stOutRect, &stInfo.stOutRect);
        
        DISP_PRINT(">>>>>>>>>>>>>>>>>>>>>>>>>>>will send info to source .............\n");
        pstWin->stCfg.stSource.pfSendWinInfo(pstWin->stCfg.stSource.hSrc, &stInfo);
    }

    return HI_SUCCESS;
}


HI_S32 WinCalcDispRectBaseRefandRel(HI_RECT_S *pRef, HI_RECT_S *pRel, 
                                             HI_RECT_S *pI, HI_RECT_S *pO)
{
    pO->s32X      = (pI->s32X * pRel->s32Width) / pRef->s32Width;
    pO->s32Width  = (pI->s32Width * pRel->s32Width) / pRef->s32Width;
    pO->s32Y      = (pI->s32Y* pRel->s32Height) / pRef->s32Height;
    pO->s32Height = (pI->s32Height* pRel->s32Height) / pRef->s32Height;

    pO->s32X      = pO->s32X & HI_WIN_OUT_RECT_X_ALIGN;
    pO->s32Width  = pO->s32Width & HI_WIN_OUT_RECT_WIDTH_ALIGN;
    pO->s32Y      = pO->s32Y & HI_WIN_OUT_RECT_Y_ALIGN;
    pO->s32Height = pO->s32Height & HI_WIN_OUT_RECT_HEIGHT_ALIGN;

    if ( pO->s32Width && (pO->s32Width < WIN_OUTRECT_MIN_WIDTH))
    {
        pO->s32Width = WIN_OUTRECT_MIN_WIDTH;
    }

    if ( pO->s32Height && (pO->s32Height < WIN_OUTRECT_MIN_HEIGHT))
    {
        pO->s32Height = WIN_OUTRECT_MIN_HEIGHT;
    }

    return HI_SUCCESS;
}

HI_S32 WinCalcCropRectBaseRefandRel(HI_RECT_S *pRef, HI_RECT_S *pRel, 
                                              HI_DRV_CROP_RECT_S *pI, 
                                              HI_DRV_CROP_RECT_S *pO)
{
    pO->u32LeftOffset   = (pI->u32LeftOffset * pRel->s32Width) / pRef->s32Width;
    pO->u32RightOffset  = (pI->u32RightOffset * pRel->s32Width) / pRef->s32Width;
    pO->u32TopOffset    = (pI->u32TopOffset * pRel->s32Height) / pRef->s32Height;
    pO->u32BottomOffset = (pI->u32BottomOffset * pRel->s32Height) / pRef->s32Height;

    pO->u32LeftOffset   = pO->u32LeftOffset   & HI_WIN_IN_RECT_X_ALIGN;
    pO->u32RightOffset  = pO->u32RightOffset  & HI_WIN_IN_RECT_X_ALIGN;
    pO->u32TopOffset    = pO->u32TopOffset    & HI_WIN_IN_RECT_Y_ALIGN;
    pO->u32BottomOffset = pO->u32BottomOffset & HI_WIN_IN_RECT_Y_ALIGN;

    return HI_SUCCESS;
}

HI_S32 WinGetSlaveWinAttr(HI_DRV_WIN_ATTR_S *pWinAttr, 
                               HI_DRV_WIN_ATTR_S *pSlvWinAttr)
{
    HI_S32 nRet;

    DISP_MEMSET(pSlvWinAttr, 0, sizeof(HI_DRV_WIN_ATTR_S));

    // s1 get slave display    
    nRet = DISP_GetSlave(pWinAttr->enDisp, &pSlvWinAttr->enDisp);
    if (nRet)
    {
        WIN_ERROR("Get slave Display failed\n");
        return nRet;
    }

    if (!DISP_IsOpened(pSlvWinAttr->enDisp) )
    {
        WIN_ERROR("Slave Display is not open\n");
        return HI_FAILURE;
    }

    // s2 get master and slave display info
    pSlvWinAttr->bVirtual = HI_FALSE;

    /* may change when window lives */
    pSlvWinAttr->stCustmAR = pWinAttr->stCustmAR;
    pSlvWinAttr->enARCvrs  = pWinAttr->enARCvrs;
    
    pSlvWinAttr->bUseCropRect = pWinAttr->bUseCropRect;
    pSlvWinAttr->stInRect   = pWinAttr->stInRect;
    
    pSlvWinAttr->stCropRect = pWinAttr->stCropRect;
    pSlvWinAttr->stOutRect  = pWinAttr->stOutRect;

    return HI_SUCCESS;
}


HI_S32 WinGetSlaveWinAttr2(WINDOW_S *pstWin,
                                 HI_DRV_WIN_ATTR_S *pWinAttr, 
                                 HI_DRV_WIN_ATTR_S *pSlvWinAttr)
{
    /* may change when window lives */
    pSlvWinAttr->stCustmAR = pWinAttr->stCustmAR;
    pSlvWinAttr->enARCvrs  = pWinAttr->enARCvrs;
 
    pSlvWinAttr->bUseCropRect = pWinAttr->bUseCropRect;
    pSlvWinAttr->stInRect   = pWinAttr->stInRect;
    
    pSlvWinAttr->stCropRect = pWinAttr->stCropRect;
    pSlvWinAttr->stOutRect  = pWinAttr->stOutRect;

    return HI_SUCCESS;
}



HI_S32 WinTestZero(volatile HI_U32 *pLock, HI_U32 u32MaxTimeIn10ms)
{
    volatile HI_U32 nLockState;
    HI_U32 u = 0;

    while(u < u32MaxTimeIn10ms)
    {
        nLockState = *pLock;
        if (!nLockState)
        {
            return HI_SUCCESS;
        }

        DISP_MSLEEP(10);
        u++;
    }

    return HI_ERR_VO_TIMEOUT;
}


HI_S32 WinCheckFrame(HI_DRV_VIDEO_FRAME_S *pFrameInfo)
{
    HI_DRV_VIDEO_PRIVATE_S *pstPriv = (HI_DRV_VIDEO_PRIVATE_S*)&(pFrameInfo->u32Priv[0]);

    pstPriv->u32PlayTime = 1;

    if (pFrameInfo->eFrmType >  HI_DRV_FT_BUTT)
    {
        WIN_FATAL("Q Frame type error : %d\n", pFrameInfo->eFrmType);
        return HI_ERR_VO_INVALID_PARA;
    }

    if (!(    (HI_DRV_PIX_FMT_NV12 == pFrameInfo->ePixFormat)
            || (HI_DRV_PIX_FMT_NV21 == pFrameInfo->ePixFormat)
          )
       )
    {
        WIN_FATAL("Q Frame pixformat error : %d\n", pFrameInfo->ePixFormat);
        return HI_ERR_VO_INVALID_PARA;
    }
   
    if (    (pFrameInfo->u32Width < WIN_FRAME_MIN_WIDTH)
         || (pFrameInfo->u32Width > WIN_FRAME_MAX_WIDTH)
         || (pFrameInfo->u32Height < WIN_FRAME_MIN_HEIGHT)
         || (pFrameInfo->u32Height > WIN_FRAME_MAX_HEIGHT)
        )
    {
        WIN_FATAL("Q Frame resolution error : w=%d,h=%d\n", 
                     pFrameInfo->u32Width, pFrameInfo->u32Height);
        return HI_ERR_VO_INVALID_PARA;
    }

    pFrameInfo->stDispRect.s32X = 0;
    pFrameInfo->stDispRect.s32Y = 0;
    pFrameInfo->stDispRect.s32Width  = pFrameInfo->u32Width;
    pFrameInfo->stDispRect.s32Height = pFrameInfo->u32Height;

/*
    if (   (pFrameInfo->stDispRect.s32X < 0)
        || (pFrameInfo->stDispRect.s32Width <  0)
        || ((pFrameInfo->stDispRect.s32Width + pFrameInfo->stDispRect.s32X) >  pFrameInfo->u32Width)
        || (pFrameInfo->stDispRect.s32Y < 0)
        || (pFrameInfo->stDispRect.s32Height < 0)
        || ((pFrameInfo->stDispRect.s32Height + pFrameInfo->stDispRect.s32Y) >  pFrameInfo->u32Height)
        )
    {
        return HI_ERR_VO_INVALID_PARA;
    }

    if (  (pFrameInfo->stDispAR.u8ARh > (pFrameInfo->stDispAR.u8ARw * WIN_MAX_ASPECT_RATIO))
        ||(pFrameInfo->stDispAR.u8ARw > (pFrameInfo->stDispAR.u8ARh  * WIN_MAX_ASPECT_RATIO))
        )
    {
        return HI_ERR_VO_INVALID_PARA;
    }

    if (pFrameInfo->u32FrameRate >  WIN_MAX_FRAME_RATE)
    {
        return HI_ERR_VO_INVALID_PARA;
    }

    if (pFrameInfo->eColorSpace >  HI_DRV_CS_SMPT240M)
    {
        return HI_ERR_VO_INVALID_PARA;
    }

    if (pFrameInfo->u32PlayTime >  WIN_MAX_FRAME_PLAY_TIME)
    {
        return HI_ERR_VO_INVALID_PARA;
    }

    // stBufAddr[1] is right eye for stereo video 
    if (    (pFrameInfo->stBufAddr[0].u32Stride_Y < pFrameInfo->stDispRect.s32Width)
        ||  (pFrameInfo->stBufAddr[0].u32Stride_C < pFrameInfo->stDispRect.s32Width)
        )
    {
        return HI_ERR_VO_INVALID_PARA;
    }
*/

    return HI_SUCCESS;
}

/* window buffer manager */
HI_S32 WinBufferReset(WIN_BUFFER_S *pstBuffer)
{
    
    DISP_MEMSET(&pstBuffer->stUselessFrame, 0, 
                sizeof(HI_DRV_VIDEO_FRAME_S)*WIN_USELESS_FRAME_MAX_NUMBER);

    pstBuffer->u32ULSRdPtr = 0;
    pstBuffer->u32ULSWtPtr = 0;

    pstBuffer->u32ULSIn  = 0;
    pstBuffer->u32ULSOut = 0;
    pstBuffer->u32UnderLoad= 0;

    return HI_SUCCESS;
}

HI_S32 WinBufferPutULSFrame(WIN_BUFFER_S *pstBuffer, HI_DRV_VIDEO_FRAME_S *pstFrame)
{
    HI_U32 WP1;

    WP1 = (pstBuffer->u32ULSWtPtr + 1) % WIN_USELESS_FRAME_MAX_NUMBER;

    if (WP1 == pstBuffer->u32ULSRdPtr)
    {
        WIN_ERROR("usl full\n");
        return HI_FAILURE;
    }

    pstBuffer->stUselessFrame[pstBuffer->u32ULSWtPtr] = *pstFrame;

    pstBuffer->u32ULSWtPtr = WP1;
    pstBuffer->u32ULSIn++;

    return HI_SUCCESS;
}

HI_S32 WinBufferGetULSFrame(WIN_BUFFER_S *pstBuffer, HI_DRV_VIDEO_FRAME_S *pstFrame)
{
    if (pstBuffer->u32ULSWtPtr == pstBuffer->u32ULSRdPtr)
    {
        return HI_FAILURE;
    }

    *pstFrame = pstBuffer->stUselessFrame[pstBuffer->u32ULSRdPtr];
    pstBuffer->u32ULSRdPtr = (pstBuffer->u32ULSRdPtr + 1) % WIN_USELESS_FRAME_MAX_NUMBER;
    pstBuffer->u32ULSOut++;

    return HI_SUCCESS;
}



/******************************************************************************
    apply function
******************************************************************************/
HI_S32 WIN_Create(HI_DRV_WIN_ATTR_S *pWinAttr, HI_HANDLE *phWin)
{
    HI_S32 nRet = HI_SUCCESS;
    WINDOW_S *pWindow = HI_NULL;
    
    WinCheckDeviceOpen();
    WinCheckNullPointer(pWinAttr);
    WinCheckNullPointer(phWin);

    // s1 check attribute
    nRet = WinCheckAttr(pWinAttr);
    if (nRet)
    {
        WIN_ERROR("WinAttr is invalid!\n");
        return nRet;
    }
    
    if (pWinAttr->bVirtual != HI_TRUE)
    {
        //HI_DRV_DISPLAY_E enSlave;
        HI_DRV_WIN_ATTR_S stSlvWinAttr;
        WINDOW_S *pSlaveWindow = HI_NULL;
        
        // if this is a display window that will be shown on screen
        if (DISP_IsOpened(pWinAttr->enDisp) != HI_TRUE)
        {
            WIN_ERROR("DISP is not opened!\n");
            return HI_ERR_DISP_NOT_EXIST;
        }

        // s2.1 create window
        nRet = WinCreateDisplayWindow(pWinAttr, &pWindow, WIN_IN_FB_DEFAULT_NUMBER);
        if (nRet)
        {
            goto __ERR_RET__;
        }

        nRet = WinRegCallback(pWindow);
        if (nRet)
        {
            goto __ERR_RET_DESTROY__;
        }

        if ( DISP_IsFollowed(pWinAttr->enDisp))
        {
            nRet = WinGetSlaveWinAttr(pWinAttr, &stSlvWinAttr);
            if (nRet)
            {
                WIN_ERROR("WinAttr is invalid!\n");
                goto __ERR_RET_UNREG_CB__;
            }

            nRet = WinCreateDisplayWindow(&stSlvWinAttr, &pSlaveWindow, WIN_IN_FB_DEFAULT_NUMBER+4);
            if (nRet)
            {
                goto __ERR_RET_UNREG_CB__;
            }
            
            nRet = WinRegCallback(pSlaveWindow);
            if (nRet)
            {
                goto __ERR_RET_DESTROY_SL__;
            }

            pSlaveWindow->enType = HI_DRV_WIN_ACTIVE_SLAVE ;
            WinAddWindow(pSlaveWindow->enDisp, pSlaveWindow);            
            pWindow->hSlvWin = (HI_HANDLE)(pSlaveWindow->u32Index);
            pSlaveWindow->pstMstWin = (HI_HANDLE)pWindow;
            pWindow->enType = HI_DRV_WIN_ACTIVE_MAIN_AND_SLAVE;
        } else {
            pWindow->enType = HI_DRV_WIN_ACTIVE_SINGLE;
        }
        
        WinAddWindow(pWindow->enDisp, pWindow);
        *phWin = (HI_HANDLE)(pWindow->u32Index);
        return HI_SUCCESS;    

__ERR_RET_DESTROY_SL__:
        WinDestroyDisplayWindow(pSlaveWindow);

__ERR_RET_UNREG_CB__:
        WinUnRegCallback(pWindow);

__ERR_RET_DESTROY__:
        WinDestroyDisplayWindow(pWindow);

__ERR_RET__:
        return nRet;
    }
    else
    {
        VIRTUAL_S *pstVirWindow;

        nRet = WIN_VIR_Create(pWinAttr, &pstVirWindow);
        if (nRet)        
            return nRet;

        pstVirWindow->enType = HI_DRV_WIN_VITUAL_SINGLE;

        // add to virtual window array, if win_deinit, auto destory it
        nRet = WinAddVirWindow(pstVirWindow);
        if (nRet) 
        {
            WIN_VIR_Destroy(pstVirWindow);
            return HI_FAILURE;
        }
        else
        {
            *phWin = (HI_HANDLE)(pstVirWindow->u32Index);
            return HI_SUCCESS;
        }
        
    }
}

HI_S32 WIN_Destroy(HI_HANDLE hWin)
{
    WINDOW_S *pstWin;
    HI_S32 nRet = HI_SUCCESS;
    HI_BOOL bVirtual;

    WinCheckDeviceOpen();
    bVirtual = WinCheckVirtual(hWin);

    if (!bVirtual)
    {
        WinCheckWindow(hWin, pstWin);
        if (pstWin->bEnable == HI_TRUE)
        {
            nRet = WIN_SetEnable(hWin, HI_FALSE);
        }

        if (pstWin->enType != HI_DRV_WIN_VITUAL_SINGLE)
        {

            if (pstWin->hSlvWin)
            {
                WIN_Destroy(pstWin->hSlvWin);
            }


            WinUnRegCallback(pstWin);
            WinDelWindow(pstWin->u32Index);
            WinDestroyDisplayWindow(pstWin);
        }
    }
    else
    {
        VIRTUAL_S *pstVirWin;
        
        WinCheckVirWindow(hWin, pstVirWin);
        
        WinDelVirWindow(hWin);

        nRet = WIN_VIR_Destroy(pstVirWin);
    }
    
    return HI_SUCCESS;
}

HI_S32 WIN_SetAttr(HI_HANDLE hWin, HI_DRV_WIN_ATTR_S *pWinAttr)
{
    WINDOW_S *pstWin;
    HI_DRV_WIN_ATTR_S stSlvWinAttr;
    HI_DISP_DISPLAY_INFO_S stDispInfo;
    HI_S32 nRet = HI_SUCCESS;
    HI_S32 t;
    HI_BOOL bVirtual;
    
    WinCheckDeviceOpen();
    WinCheckNullPointer(pWinAttr);

    bVirtual = WinCheckVirtual(hWin);
    if (!bVirtual)
    {
        WinCheckWindow(hWin, pstWin);     
        nRet = WinCheckFixedAttr(&pstWin->stCfg.stAttr, pWinAttr);
        if (nRet)
        {
            return nRet;
        }

        nRet = WinCheckAttr(pWinAttr);
        if (nRet)
        {
            return nRet;
        }
        
        nRet = DISP_GetDisplayInfo(pstWin->enDisp, &stDispInfo);
        if (nRet)
        {
            WIN_ERROR("DISP_GetDisplayInfo failed in %s!\n", __FUNCTION__);
            return HI_ERR_VO_CREATE_ERR;
        }        

        atomic_set(&pstWin->stCfg.bNewAttrFlag, 0);        
        pstWin->stCfg.stAttrBuf = *pWinAttr;
        atomic_set(&pstWin->stCfg.bNewAttrFlag, 1);

        t = 0;
        while( atomic_read(&pstWin->stCfg.bNewAttrFlag) )
        {
            DISP_MSLEEP(5);
            t++;

            if (t > 10)
            {
                break;
            }
        }
        
        if (pstWin->hSlvWin)
        {
            WIN_GetAttr(pstWin->hSlvWin, &stSlvWinAttr);

            WinGetSlaveWinAttr2(pstWin, pWinAttr, &stSlvWinAttr);

            nRet = WIN_SetAttr(pstWin->hSlvWin, &stSlvWinAttr);
        }

        if (atomic_read(&pstWin->stCfg.bNewAttrFlag) )
        {
            atomic_set(&pstWin->stCfg.bNewAttrFlag, 0);
            WIN_ERROR("WIN Set Attr timeout in %s\n", __FUNCTION__);
            return HI_ERR_VO_TIMEOUT;
        }
    }
    else
    {
        //VIRTUAL_S *pstVirWindow;
        
        //WinCheckVirWindow(hWin, pstVirWindow);

        //nRet = WIN_VIR_SetAttr(pstVirWindow, pWinAttr);        
        return HI_ERR_VO_INVALID_OPT;
    }
    
    return HI_SUCCESS;
}



HI_S32 WIN_GetAttr(HI_HANDLE hWin, HI_DRV_WIN_ATTR_S *pWinAttr)
{
    HI_BOOL bVirtual;
    
    WinCheckDeviceOpen();
    WinCheckNullPointer(pWinAttr);
    bVirtual = WinCheckVirtual(hWin);

    if (!bVirtual)
    {
        WINDOW_S *pstWin; 

        WinCheckWindow(hWin, pstWin);
        *pWinAttr = pstWin->stCfg.stAttr;
    }
    else
    {
        VIRTUAL_S *pstVirWindow;
        
        WinCheckVirWindow(hWin, pstVirWindow);
        *pWinAttr = pstVirWindow->stAttrBuf;
    }
    
    return HI_SUCCESS;
}


//get info for source
HI_S32 WIN_GetInfo(HI_HANDLE hWin, HI_DRV_WIN_INFO_S * pstInfo)
{
    WINDOW_S *pstWin;
    HI_BOOL bVirtual;    
    
    WinCheckDeviceOpen();
    WinCheckNullPointer(pstInfo);

    bVirtual = WinCheckVirtual(hWin);
    if (!bVirtual)
    {
        WinCheckWindow(hWin, pstWin);
        pstInfo->eType = WinGetType(pstWin);
        pstInfo->hPrim = (HI_HANDLE)(pstWin->u32Index);
        pstInfo->hSec  = (HI_HANDLE)(pstWin->hSlvWin);
    }
    else
    {
        VIRTUAL_S *pstVirWin;
        WinCheckVirWindow(hWin, pstVirWin);

        pstInfo->eType = pstVirWin->enType;
        pstInfo->hPrim = (HI_HANDLE)(pstVirWin->u32Index);
        pstInfo->hSec  = HI_INVALID_HANDLE;
    }
    
    return HI_SUCCESS;
}

HI_S32 WIN_SetSource(HI_HANDLE hWin, HI_DRV_WIN_SRC_INFO_S *pstSrc)
{
    HI_DISP_DISPLAY_INFO_S stDispInfo;
    WB_SOURCE_INFO_S stSrc2Buf;
    WINDOW_S *pstWin;
    HI_BOOL bVirtual;
    
    HI_S32 nRet = HI_SUCCESS;

    WinCheckDeviceOpen();
    WinCheckNullPointer(pstSrc);

    bVirtual = WinCheckVirtual(hWin);
    if (!bVirtual)
    {
        WinCheckWindow(hWin, pstWin);
        
        nRet = DISP_GetDisplayInfo(WinGetDispID(pstWin), &stDispInfo);
        if (nRet)
        {
            return nRet;
        }

        pstWin->stCfg.stSource = *pstSrc;

        stSrc2Buf.hSrc = pstSrc->hSrc;
        stSrc2Buf.pfAcqFrame = pstSrc->pfAcqFrame;
        stSrc2Buf.pfRlsFrame = pstSrc->pfRlsFrame;
        nRet =  WinBuf_SetSource(&pstWin->stBuffer.stWinBP, &stSrc2Buf);
        if (nRet)
        {
            return nRet;
        }
        // send attr to source
        WinSendAttrToSource(pstWin, &stDispInfo);

        DISP_PRINT("WIN_SetSource :s=0x%x, info=0x%x, g=0x%x,==0x%x\n",
                (HI_U32)pstSrc->hSrc, (HI_U32)pstSrc->pfSendWinInfo,
                (HI_U32)pstSrc->pfAcqFrame, (HI_U32)pstSrc->pfRlsFrame);
    }
    else
    {
        VIRTUAL_S *pstVirWin;

        WinCheckVirWindow(hWin, pstVirWin);

        pstVirWin->stSrcInfo = *pstSrc;
        
        WIN_VIR_SendAttrToSource(pstVirWin);
    }
    return HI_SUCCESS;
}

HI_S32 WIN_GetSource(HI_HANDLE hWin, HI_DRV_WIN_SRC_INFO_S *pstSrc)
{
    WINDOW_S *pstWin;
    HI_S32 nRet = HI_SUCCESS;

    WinCheckDeviceOpen();
    WinCheckNullPointer(pstSrc);
    WinCheckWindow(hWin, pstWin);

    nRet = WinCheckSourceInfo(pstSrc);
    if (nRet)
    {
        return nRet;
    }

    *pstSrc = pstWin->stCfg.stSource;

    return HI_SUCCESS;
}

HI_S32 WIN_SetEnable(HI_HANDLE hWin, HI_BOOL bEnable)
{
    WINDOW_S *pstWin;
    HI_BOOL bVirtual;
    
    WinCheckDeviceOpen();
    
    bVirtual = WinCheckVirtual(hWin);
    if (!bVirtual)
    {
        WinCheckWindow(hWin, pstWin);
        pstWin->bEnable = bEnable;

        if (HI_DRV_WIN_ACTIVE_MAIN_AND_SLAVE == WinGetType(pstWin))
        {
            if (pstWin->hSlvWin)
            {
                WINDOW_S *pstSlvWin;
                WinCheckWindow(pstWin->hSlvWin, pstSlvWin);
                pstSlvWin->bEnable = bEnable;
            }
        }

        if (pstWin->bEnable == HI_FALSE)
        {
            if (pstWin->stDelayInfo.T)
            {
                DISP_MSLEEP(2*pstWin->stDelayInfo.T);
            }
            else
            {
                DISP_MSLEEP(2* 20);
            }
        }
    }
    else
    {
        VIRTUAL_S *pstVirWindow;
        
        WinCheckVirWindow(hWin, pstVirWindow);
        
        pstVirWindow->bEnable = bEnable;
        
    }
    return HI_SUCCESS;
}

HI_S32 WIN_GetEnable(HI_HANDLE hWin, HI_BOOL *pbEnable)
{
    WINDOW_S *pstWin;
    HI_BOOL bVirtual;
    
    WinCheckDeviceOpen();

    WinCheckNullPointer(pbEnable);
    
    bVirtual = WinCheckVirtual(hWin);
    if (!bVirtual)
    {
        WinCheckWindow(hWin, pstWin);
        *pbEnable = pstWin->bEnable;
    }
    else
    {
        VIRTUAL_S *pstVirWindow;
        
        WinCheckVirWindow(hWin, pstVirWindow);

        *pbEnable = pstVirWindow->bEnable;
        
    }
    return HI_SUCCESS;
}



#define WIN_PROCESS_CALC_TIME_THRESHOLD 10
#define WIN_DELAY_TIME_MAX_CIRCLE 50
HI_S32 WIN_CalcDelayTime(WINDOW_S *pstWin, HI_U32 *pu32BufNum, HI_U32 *pu32DelayMs)
{
    HI_U32 u32Num, u32NumNew, T, Dt, Ct, LstCt, Delay;
    HI_U32 L = 0;

__WIN_CALC_DELAY__:
    L++;
    if (L > WIN_PROCESS_CALC_TIME_THRESHOLD)
    {
        goto __WIN_CALC_DELAY_ERROR__;
    }
    
    LstCt = pstWin->stDelayInfo.u32CfgTime;
    T = pstWin->stDelayInfo.T;

    WinBuf_GetFullBufNum(&pstWin->stBuffer.stWinBP, &u32Num);
    Dt = pstWin->stDelayInfo.u32DisplayTime;

    HI_DRV_SYS_GetTimeStampMs(&Ct);
    
    if (Ct >= LstCt)
    {
        Delay = Ct - LstCt;
    }
    else
    {
        // circle happen, Ct across zero
        Delay = 0xFFFFFFFFul - LstCt + Ct;
    }

#if 0
    if (Delay > ((T*3)/2) )
    {
        printk("Delay=%d, T = %d\n", Delay, T);
        goto __WIN_CALC_DELAY__;
    }
#endif
    //printk("Delay=%d, Dt = %d\n", Delay, Dt);
    if (T <= (Dt + Delay))
    {
        Delay = 0;
    }
    else
    {
        Delay = T - Dt - Delay;
    }
    
    Delay = (u32Num + 1) * T + Delay;

    WinBuf_GetFullBufNum(&pstWin->stBuffer.stWinBP, &u32NumNew);
    if (  (LstCt != pstWin->stDelayInfo.u32CfgTime)
        ||(u32NumNew != u32Num)
        ||(HI_TRUE == pstWin->bInInterrupt)
       )
    {
        udelay(100);
        goto __WIN_CALC_DELAY__;
    }

    if (Delay > (T * WIN_DELAY_TIME_MAX_CIRCLE))
    {
        DISP_ASSERT(!Delay);
        goto __WIN_CALC_DELAY_ERROR__;
    }

    *pu32BufNum = u32Num;
    *pu32DelayMs= Delay;
    return HI_SUCCESS;

__WIN_CALC_DELAY_ERROR__:
    
    *pu32BufNum = 0;
    *pu32DelayMs= 0;

    return HI_SUCCESS;
}

HI_BOOL WinGetTBMatchInfo(HI_HANDLE hWin)
{
    WINDOW_S *pstWin;
    
    WinCheckWindow(hWin, pstWin);

    return pstWin->stDelayInfo.bTBMatch;
}

HI_S32 WIN_GetPlayInfo(HI_HANDLE hWin, HI_DRV_WIN_PLAY_INFO_S *pstInfo)
{
    WINDOW_S *pstWin;

    WinCheckDeviceOpen();
    WinCheckNullPointer(pstInfo);
    WinCheckWindow(hWin, pstWin);

    if (!pstWin->bEnable || !pstWin->stDelayInfo.u32DispRate)
    {
        WIN_ERROR("window is not ready!\n");
        return HI_ERR_VO_INVALID_OPT;
    }


    WIN_CalcDelayTime(pstWin, &(pstInfo->u32FrameNumInBufQn), &(pstInfo->u32DelayTime));
    pstInfo->u32DispRate = pstWin->stDelayInfo.u32DispRate;
    
    return HI_SUCCESS;
}

HI_S32 WinQueueFrame(HI_HANDLE hWin, HI_DRV_VIDEO_FRAME_S *pFrameInfo)
{
    WINDOW_S *pstWin;
    HI_DRV_WIN_SRC_INFO_S *pstSource;
    //HI_U32 u32BufId;
    HI_S32 nRet = HI_SUCCESS;
    HI_BOOL bVirtual;
    
    WinCheckDeviceOpen();

    WinCheckNullPointer(pFrameInfo);
    // s2 get state
    nRet = WinCheckFrame(pFrameInfo);
    if (nRet)
    {
        WIN_ERROR("win frame parameters invalid\n");
        return HI_ERR_VO_FRAME_INFO_ERROR;
    }
    bVirtual = WinCheckVirtual(hWin);
    if (!bVirtual)
    {
        WinCheckWindow(hWin, pstWin);
        pstSource = &pstWin->stCfg.stSource;
        
        nRet = WinBuf_PutNewFrame(&pstWin->stBuffer.stWinBP, pFrameInfo);
        if (nRet)
        {
            return HI_ERR_VO_BUFQUE_FULL;
        }        
    }
    else
    {
        VIRTUAL_S *pstVirWin;

        WinCheckVirWindow(hWin, pstVirWin);

        // update sink acquire frame count
        pstVirWin->stFrameStat.u32SrcQTry++;

        nRet = WIN_VIR_AddNewFrm(pstVirWin, pFrameInfo);
        if (nRet)
        {
            return HI_ERR_VO_BUFQUE_FULL;
        }

        pstVirWin->stFrameStat.u32SrcQOK++;
    }

    return HI_SUCCESS;
}

HI_S32 WIN_QueueFrame(HI_HANDLE hWin, HI_DRV_VIDEO_FRAME_S *pFrameInfo)
{
    WinCheckNullPointer(pFrameInfo);
    
    pFrameInfo->bStillFrame =  HI_FALSE;
    return WinQueueFrame(hWin, pFrameInfo);
}

HI_S32 WIN_QueueUselessFrame(HI_HANDLE hWin, HI_DRV_VIDEO_FRAME_S *pFrameInfo)
{
    WINDOW_S *pstWin;
    HI_S32 nRet = HI_SUCCESS;
    HI_BOOL bVirtual;

    
    WinCheckDeviceOpen();

    WinCheckNullPointer(pFrameInfo);

    bVirtual = WinCheckVirtual(hWin);

    if (!bVirtual)
    {
        WinCheckWindow(hWin, pstWin);
        nRet = WinCheckFrame(pFrameInfo);
        if (nRet)
        {
            WIN_ERROR("win frame parameters invalid\n");
            return HI_ERR_VO_FRAME_INFO_ERROR;
        }

        nRet = WinBufferPutULSFrame(&pstWin->stBuffer, pFrameInfo);
        if (nRet)
        {
            WIN_WARN("quls failed\n");
            return HI_ERR_VO_BUFQUE_FULL;
        }
    }
    else
    {
        VIRTUAL_S *pstVirWindow;
        WinCheckVirWindow(hWin, pstVirWindow);

        nRet = WinCheckFrame(pFrameInfo);
        if (nRet)
        {
            WIN_ERROR("win frame parameters invalid\n");
            return HI_ERR_VO_FRAME_INFO_ERROR;
        }

        nRet = WIN_VIR_AddUlsFrm(pstVirWindow, pFrameInfo);
        if (nRet)
        {
            WIN_WARN("quls failed\n");
            return HI_ERR_VO_BUFQUE_FULL;
        }
    }
    
    return HI_SUCCESS;
}

HI_S32 WIN_DequeueFrame(HI_HANDLE hWin, HI_DRV_VIDEO_FRAME_S *pFrameInfo)
{
    WINDOW_S *pstWin;

    WinCheckDeviceOpen();
    WinCheckNullPointer(pFrameInfo);
    WinCheckWindow(hWin, pstWin);

    return HI_SUCCESS;
}

HI_S32 WIN_SetZorder(HI_HANDLE hWin, HI_DRV_DISP_ZORDER_E enZFlag)
{
    WINDOW_S *pstWin;
    HI_S32 nRet = HI_SUCCESS;
    HI_BOOL bVirtual;
    
    WinCheckDeviceOpen();
    
    if (enZFlag >= HI_DRV_DISP_ZORDER_BUTT)
    {
        WIN_FATAL("HI_DRV_DISP_ZORDER_E invalid!\n");
        return HI_ERR_VO_INVALID_PARA;
    }
    
    bVirtual = WinCheckVirtual(hWin);
    if (!bVirtual)
    {
        // s1 检查句柄合法性
        WinCheckWindow(hWin, pstWin);

        switch(enZFlag)
        {
            case HI_DRV_DISP_ZORDER_MOVETOP:
                nRet = pstWin->stVLayerFunc.PF_MovTop(pstWin->u32VideoLayer);
                break;
            case HI_DRV_DISP_ZORDER_MOVEUP:
                nRet = pstWin->stVLayerFunc.PF_MovUp(pstWin->u32VideoLayer);
                break;
            case HI_DRV_DISP_ZORDER_MOVEBOTTOM:
                nRet = pstWin->stVLayerFunc.PF_MovBottom(pstWin->u32VideoLayer);
                break;
            case HI_DRV_DISP_ZORDER_MOVEDOWN:
                nRet = pstWin->stVLayerFunc.PF_MovDown(pstWin->u32VideoLayer);
                break;
            default :
                nRet = HI_ERR_VO_INVALID_OPT;
                break;
        }


        if (HI_DRV_WIN_ACTIVE_MAIN_AND_SLAVE == WinGetType(pstWin))
        {
            if (pstWin->hSlvWin)
            {
                WIN_SetZorder(pstWin->hSlvWin, enZFlag);
            }
        }  
    }
    else
    {
        return HI_ERR_VO_WIN_UNSUPPORT;
    }
    return nRet;
}

HI_S32 WIN_GetZorder(HI_HANDLE hWin, HI_U32 *pu32Zorder)
{
    WINDOW_S *pstWin;
    HI_S32 nRet = HI_SUCCESS;
    HI_BOOL bVirtual;
    WinCheckDeviceOpen();
    WinCheckNullPointer(pu32Zorder);

    bVirtual = WinCheckVirtual(hWin);
    if (!bVirtual)
    {
        // s1 检查句柄合法性
        WinCheckWindow(hWin, pstWin);

        nRet = pstWin->stVLayerFunc.PF_GetZorder(pstWin->u32VideoLayer, pu32Zorder);
    }
    else
    {
        return HI_ERR_VO_WIN_UNSUPPORT;
    }
   return nRet;
}


HI_S32 WIN_Freeze(HI_HANDLE hWin, HI_BOOL bEnable, HI_DRV_WIN_SWITCH_E enFrz)
{
    WINDOW_S *pstWin;
    HI_BOOL bVirtual;
    HI_U32  u = 0;

    
    WinCheckDeviceOpen();

    bVirtual = WinCheckVirtual(hWin);
    if (!bVirtual)
    {
        // s1 检查句柄合法性
        WinCheckWindow(hWin, pstWin);
        //WinCheckSlaveWindow(pstWin);

        if (enFrz >= HI_DRV_WIN_SWITCH_BUTT)
        {
            WIN_ERROR("Freeze mode is invalid!\n");
            return HI_ERR_VO_INVALID_PARA;
        }

        // s2 set enable
        if (pstWin->bUpState && pstWin->bEnable)
        {
            WIN_ERROR("Window is changing, can't set pause now!\n");
            return HI_ERR_VO_INVALID_OPT;
        }

        if (!pstWin->bEnable || pstWin->bReset)
        {
            WIN_ERROR("Window is DISABLE, can't set pause now!\n");
            return HI_ERR_VO_INVALID_OPT;
        }


        pstWin->bUpState = HI_FALSE;

        pstWin->enStateNew = bEnable ? WIN_STATE_FREEZE : WIN_STATE_UNFREEZE;
        if (bEnable)
        {
            pstWin->stFrz.enFreezeMode = enFrz;
        }
        pstWin->bUpState = HI_TRUE;

        if (HI_DRV_WIN_ACTIVE_MAIN_AND_SLAVE == WinGetType(pstWin))
        {
            if (pstWin->hSlvWin)
            {
                WIN_Freeze(pstWin->hSlvWin, bEnable, enFrz);
            }
        }
        
        u = 0;
        while(pstWin->bUpState && (u<10))
        {
            DISP_MSLEEP(5);
            u++;
        }
        
        if (u >= 10)
        {
            DISP_WARN("############ freeze TIMEOUT#########\n");
        }
    }
    else
    {
        return HI_ERR_VO_WIN_UNSUPPORT;
    }
    return HI_SUCCESS;
}

//HI_VOID ISR_WinReleaseFullFrame(WINDOW_S *pstWin);
HI_S32 WIN_Reset(HI_HANDLE hWin, HI_DRV_WIN_SWITCH_E enRst)
{
    WINDOW_S *pstWin;
    HI_DRV_VIDEO_FRAME_S *pstFrame;
    HI_U32 u = 0;
    HI_BOOL bVirtual;
    
    WinCheckDeviceOpen();
    bVirtual = WinCheckVirtual(hWin);
    if (!bVirtual)
    {
        // s1 检查句柄合法性
        WinCheckWindow(hWin, pstWin);
        //WinCheckSlaveWindow(pstWin);

        if (enRst >= HI_DRV_WIN_SWITCH_BUTT)
        {
            WIN_ERROR("Reset mode is invalid!\n");
            return HI_ERR_VO_INVALID_PARA;
        }

        // s2 set enable
        if (pstWin->bReset || pstWin->bUpState)
        {
            //WIN_ERROR("Last reset is not finished!\n");
            return HI_ERR_VO_INVALID_OPT;
        }

        if (pstWin->bEnable)
        {
            //printk("01 enter win reset........\n");
            pstWin->stRst.enResetMode = enRst;
            pstWin->bReset = HI_TRUE;

            while(pstWin->bReset && (u<10))
            {
                DISP_MSLEEP(20);
                u++;
            }

            if (u >= 5)
            {
                DISP_WARN("############ RESET TIMEOUT#########\n");
            }
            //printk("01 exit win reset\n");
        }
        else
        {
            //printk("02 enter win reset........\n");
            //ISR_WinReleaseDisplayedFrame(pstWin);
            WinBuf_RlsAndUpdateUsingFrame(&pstWin->stBuffer.stWinBP);
            
            ISR_WinReleaseUSLFrame(pstWin);

            // flush frame in full buffer pool
            pstFrame = WinBuf_GetDisplayedFrame(&pstWin->stBuffer.stWinBP);
            WinBuf_FlushWaitingFrame(&pstWin->stBuffer.stWinBP, pstFrame);

            pstWin->bReset = HI_FALSE;

            //printk("02 exit win reset\n");
        }
    }
    else
    {
        return HI_ERR_VO_WIN_UNSUPPORT;
    }
    return HI_SUCCESS;
}


HI_S32 WIN_Pause(HI_HANDLE hWin, HI_BOOL bEnable)
{
    WINDOW_S *pstWin;
    HI_U32 u;
    HI_BOOL bVirtual;
    
    WinCheckDeviceOpen();
    bVirtual = WinCheckVirtual(hWin);
    
    if (!bVirtual)
    {
        // s1 检查句柄合法性
        WinCheckWindow(hWin, pstWin);
        //WinCheckSlaveWindow(pstWin);

        // s2 set enable
        if (pstWin->bUpState && pstWin->bEnable)
        {
            WIN_ERROR("Window is changing, can't set pause now!\n");
            return HI_ERR_VO_INVALID_OPT;
        }

        pstWin->bUpState = HI_FALSE;

        pstWin->enStateNew = bEnable ? WIN_STATE_PAUSE : WIN_STATE_RESUME;

        pstWin->bUpState = HI_TRUE;

        u = 0;
        while(pstWin->bUpState && (u<10))
        {
            DISP_MSLEEP(5);
            u++;
        }

        if (u >= 10)
        {
            DISP_WARN("############ PAUSE TIMEOUT#########\n");
        }
    }
    else
    {
        return HI_ERR_VO_WIN_UNSUPPORT;
    }
    return HI_SUCCESS;
}

HI_S32 WIN_SetStepMode(HI_HANDLE hWin, HI_BOOL bStepMode)
{
    WINDOW_S *pstWin;
    HI_BOOL bVirtual;
    
    WinCheckDeviceOpen();
    
    bVirtual = WinCheckVirtual(hWin);
    
    if (!bVirtual)
    {
        // s1 检查句柄合法性
        WinCheckWindow(hWin, pstWin);

        // s2 set enable
        if (pstWin->bUpState && pstWin->bEnable)
        {
            WIN_ERROR("Window is changing, can't set pause now!\n");
            return HI_ERR_VO_INVALID_OPT;
        }

        // set stepmode flag
        pstWin->bStepMode = bStepMode;

        if (HI_DRV_WIN_ACTIVE_MAIN_AND_SLAVE == WinGetType(pstWin))
        {
            if (pstWin->hSlvWin)
            {
                WIN_SetStepMode(pstWin->hSlvWin, bStepMode);
            }
        }  
    }
    else
    {
        return HI_ERR_VO_WIN_UNSUPPORT;
    }
    return HI_SUCCESS;
}

HI_S32 WIN_SetStepPlay(HI_HANDLE hWin)
{


    return HI_SUCCESS;
}

HI_S32 WIN_SetQuick(HI_HANDLE hWin, HI_BOOL bEnable)
{
    WINDOW_S *pstWin;

    WinCheckDeviceOpen();

    // s1 检查句柄合法性
    WinCheckWindow(hWin, pstWin);
    //WinCheckSlaveWindow(pstWin);

    // s2 set enable
    if (pstWin->bUpState && pstWin->bEnable)
    {
        WIN_ERROR("Window is changing, can't set pause now!\n");
        return HI_ERR_VO_INVALID_OPT;
    }

    // initial quickmode
    pstWin->bQuickMode = bEnable;

    if (HI_DRV_WIN_ACTIVE_MAIN_AND_SLAVE == WinGetType(pstWin))
    {
        if (pstWin->hSlvWin)
        {
            WIN_SetQuick(pstWin->hSlvWin, bEnable);
        }
    }  

    return HI_SUCCESS;
}



/* only for virtual window */


HI_S32 WIN_SetExtBuffer(HI_HANDLE hWin, HI_DRV_VIDEO_BUFFER_POOL_S* pstBuf)
{


    return HI_SUCCESS;
}
HI_S32 WIN_AttachSink(HI_HANDLE hWin, HI_HANDLE hSink)
{
    VIRTUAL_S *pstVirWin;
    HI_S32 s32Ret;
    
    WinCheckVirWindow(hWin, pstVirWin);

    s32Ret = WIN_VIR_AttachSink(pstVirWin, hSink);
    
    return s32Ret;
}

HI_S32 WIN_DetachSink(HI_HANDLE hWin, HI_HANDLE hSink)
{
    VIRTUAL_S *pstVirWin;
    HI_S32 s32Ret;
    
    WinCheckVirWindow(hWin, pstVirWin);

    s32Ret = WIN_VIR_DetachSink(pstVirWin, hSink);
    
    return s32Ret;
}

HI_S32 WIN_SetVirtualAttr(HI_HANDLE hWin, HI_U32 u32Width,HI_U32 u32Height)
{
    VIRTUAL_S *pstVirWin;
    HI_S32 s32Ret;
    
    WinCheckVirWindow(hWin, pstVirWin);

    s32Ret = WIN_VIR_SetSize(pstVirWin, u32Width,u32Height);

    return s32Ret;
}
HI_S32 WIN_AcquireFrame(HI_HANDLE hWin, HI_DRV_VIDEO_FRAME_S *pFrameinfo)
{
    VIRTUAL_S *pstVirWin;
    HI_S32 s32Ret;
    
    WinCheckVirWindow(hWin, pstVirWin);

    s32Ret = WIN_VIR_GetFrm(pstVirWin, pFrameinfo);

    // update sink acquire frame count
    pstVirWin->stFrameStat.u32SinkAcqTry++;
    if (s32Ret == HI_SUCCESS)
    {
        pstVirWin->stFrameStat.u32SinkAcqOK++;
    }    

    return s32Ret;
}

HI_S32 WIN_ReleaseFrame(HI_HANDLE hWin, HI_DRV_VIDEO_FRAME_S *pFrameinfo)
{
    VIRTUAL_S *pstVirWin;
    HI_S32 s32Ret;
    
    WinCheckVirWindow(hWin, pstVirWin);

    s32Ret = WIN_VIR_RelFrm(pstVirWin, pFrameinfo);

    // update sink release frame count
    pstVirWin->stFrameStat.u32SinkRlsTry++;
    if (s32Ret == HI_SUCCESS)
    {
        pstVirWin->stFrameStat.u32SinkRlsOK++;
    }

    return s32Ret;
}

HI_S32 WIN_CreatStillFrame(HI_DRV_VIDEO_FRAME_S *pFrameinfo,HI_DRV_VIDEO_FRAME_S *pStillFrameInfo)
{
    HI_VDEC_PRIV_FRAMEINFO_S *pstPrivInfo = HI_NULL;
    HI_U32 datalen = 0, y_stride = 0, height = 0 ;
    DISP_MMZ_BUF_S  stMMZ_StillFrame;
    DISP_MMZ_BUF_S  stMMZ_Frame;
 
    pstPrivInfo = (HI_VDEC_PRIV_FRAMEINFO_S *)(pFrameinfo->u32Priv);    
    
    memset(&stMMZ_StillFrame,0,sizeof(DISP_MMZ_BUF_S));
    memset(&stMMZ_Frame,0,sizeof(DISP_MMZ_BUF_S));
    
    /*1:calculate alloc mem*/
    y_stride = pFrameinfo->stBufAddr[0].u32Stride_Y;
    height   = (HI_TRUE == pstPrivInfo->stCompressInfo.u32CompressFlag)
                     ? pstPrivInfo->stCompressInfo.s32CompFrameHeight : pFrameinfo->u32Height;        
    
    if ( HI_DRV_PIX_FMT_NV21 == pFrameinfo->ePixFormat) 
        datalen = height * y_stride * 3 / 2 + height * 4;
    else 
        datalen = height * y_stride * 2 + height * 4;        
    
    if(HI_SUCCESS != DISP_OS_MMZ_Alloc("VDP_StillFrame", HI_NULL, datalen, 16, &stMMZ_StillFrame))
    {
        WIN_ERROR(" Alloc StillFrame  failid(%x)\n",datalen);
        return HI_FAILURE;
    }

    /*2: creat still frame*/
    /*not support  Compress info*/
    stMMZ_Frame.u32StartPhyAddr = pFrameinfo->stBufAddr[0].u32PhyAddr_Y;
        
    DISP_OS_MMZ_Map(&stMMZ_StillFrame);
    DISP_OS_MMZ_Map(&stMMZ_Frame);
    



    memcpy(pStillFrameInfo,pFrameinfo,sizeof(HI_DRV_VIDEO_FRAME_S));
    memcpy((void *)stMMZ_StillFrame.u32StartVirAddr, (void *)stMMZ_Frame.u32StartVirAddr,datalen);


    /*3: calculate still frame addr*/
    pStillFrameInfo->stBufAddr[0].u32PhyAddr_YHead = stMMZ_StillFrame.u32StartPhyAddr +(pFrameinfo->stBufAddr[0].u32PhyAddr_YHead - stMMZ_Frame.u32StartPhyAddr );
    pStillFrameInfo->stBufAddr[0].u32Stride_Y =  pFrameinfo->stBufAddr[0].u32Stride_Y;
    
    pStillFrameInfo->stBufAddr[0].u32PhyAddr_Y = stMMZ_StillFrame.u32StartPhyAddr +(stMMZ_Frame.u32StartPhyAddr - pFrameinfo->stBufAddr[0].u32PhyAddr_Y );

    pStillFrameInfo->stBufAddr[0].u32PhyAddr_C = stMMZ_StillFrame.u32StartPhyAddr + (pStillFrameInfo->u32Height*pStillFrameInfo->stBufAddr[0].u32Stride_Y);

    pStillFrameInfo->stBufAddr[0].u32PhyAddr_CrHead = stMMZ_StillFrame.u32StartPhyAddr +( pFrameinfo->stBufAddr[0].u32PhyAddr_CrHead - stMMZ_Frame.u32StartPhyAddr);
    pStillFrameInfo->stBufAddr[0].u32PhyAddr_Cr = stMMZ_StillFrame.u32StartPhyAddr +( pFrameinfo->stBufAddr[0].u32PhyAddr_Cr - stMMZ_Frame.u32StartPhyAddr );
    pStillFrameInfo->stBufAddr[0].u32Stride_Cr = pFrameinfo->stBufAddr[0].u32Stride_Cr;

    pStillFrameInfo->bStillFrame = HI_TRUE;
    
    DISP_OS_MMZ_UnMap(&stMMZ_StillFrame);
    DISP_OS_MMZ_UnMap(&stMMZ_Frame);
    return HI_SUCCESS;
}

HI_S32 WinReleaseStillFrame(HI_DRV_VIDEO_FRAME_S *pStillFrameInfo)
{
    DISP_MMZ_BUF_S  stMMZ_StillFrame;
    HI_VDEC_PRIV_FRAMEINFO_S *pstPrivInfo = HI_NULL;
    
    WinCheckNullPointer(pStillFrameInfo);
    if (pStillFrameInfo->bStillFrame)
    {
        memset((void*)&stMMZ_StillFrame, 0, sizeof(DISP_MMZ_BUF_S));    
        pstPrivInfo = (HI_VDEC_PRIV_FRAMEINFO_S *)(pStillFrameInfo->u32Priv);    
        if (HI_TRUE == pstPrivInfo->stCompressInfo.u32CompressFlag)
        {
            stMMZ_StillFrame.u32StartPhyAddr = pStillFrameInfo->stBufAddr[0].u32PhyAddr_YHead;
        }
        else
        {
            stMMZ_StillFrame.u32StartPhyAddr = pStillFrameInfo->stBufAddr[0].u32PhyAddr_Y;
        }
        //printk("release 0x%x\n",stMMZ_StillFrame.u32StartPhyAddr);
         DISP_OS_MMZ_Release(&stMMZ_StillFrame);
         return HI_SUCCESS;
    }
   
    return HI_FAILURE;
}



HI_S32 WIN_SendFrame(HI_HANDLE hWin, HI_DRV_VIDEO_FRAME_S *pFrameInfo)
{
    HI_S32 nRet;
    HI_DRV_VIDEO_FRAME_S StillFrameInfo;
    WinCheckNullPointer(pFrameInfo);
    
    nRet = WIN_CreatStillFrame(pFrameInfo,&StillFrameInfo);
    if ( HI_SUCCESS != nRet )
    {
        WIN_ERROR(" WIN_CreatStillFrame  failid(%x)\n",nRet);
        return nRet;
    }
    
    return WinQueueFrame( hWin,  &StillFrameInfo);
}

HI_S32 Win_DebugGetHandle(HI_DRV_DISPLAY_E enDisp, WIN_HANDLE_ARRAY_S *pstWin)
{
    HI_S32 i;

    WinCheckDeviceOpen();

    // s1 检查句柄合法性
    WinCheckNullPointer(pstWin);

    DISP_MEMSET(pstWin, 0, sizeof(WIN_HANDLE_ARRAY_S));

    pstWin->u32WinNumber = 0;

    for(i=0; i<WINDOW_MAX_NUMBER; i++)
    {
        if (stDispWindow.pstWinArray[(HI_U32)enDisp][i])
        {
            pstWin->ahWinHandle[pstWin->u32WinNumber] = (HI_HANDLE)(stDispWindow.pstWinArray[(HI_U32)enDisp][i]->u32Index);
            pstWin->u32WinNumber++;
        }
    }

    return HI_SUCCESS;
}


/*
HI_S32 WinUpdatePlayInfo(HI_DRV_WIN_PLAY_INFO_S *ptPlay, HI_U32 u32Rate)
{
    ptPlay->u32DispRate = u32Rate;
    return HI_SUCCESS;
}
*/



HI_S32 WinAcquireFrame(WINDOW_S *pstWin)
{
#if 0
    HI_DRV_VIDEO_FRAME_S stNewFrame;
    HI_U32 u32BufId;
    HI_S32 nRet = HI_SUCCESS;

    nRet = BQ_GetWriteNode(&pstWin->stBuffer.stBP, &u32BufId);
    if (nRet)
    {
        return HI_ERR_VO_BUFQUE_FULL;
    }

    if (pstWin->stSource.pfAcqFrame)
    {
        nRet = pstWin->stSource.pfAcqFrame(pstWin->stSource.hSrc, &stNewFrame);
        if (nRet)
        {
            WIN_ERROR("WIN Release Frame failid\n");
            return HI_ERR_VO_FRAME_RELEASE_FAILED;
        }
    }

    // s2 get state
    nRet = WinCheckFrame(&stNewFrame);
    if (nRet)
    {
        return HI_ERR_VO_FRAME_INFO_ERROR;
    }

    if (!stNewFrame.u32PlayTime)
    {
        if (pstWin->stSource.pfRlsFrame && stNewFrame.bToRelease)
        {
            nRet = pstWin->stSource.pfRlsFrame(pstWin->stSource.hSrc, &stNewFrame);
            if (nRet)
            {
                WIN_ERROR("WIN Release Frame failid\n");
            }

            BQ_ReleaseRecoder(&pstWin->stBufQue, BQ_RELEASE_DISCARD);
        }
        else
        {
            // todo
        }
    }

    nRet = BQ_PutWriteNode(&pstWin->stBufQue, &stNewFrame, BUF_FRAME_SRC_NORMAL);
#endif

    return HI_SUCCESS;
}


//#define WIN_DEBUG_PRINT_RELEASE 1
HI_S32 s_ResetPrint = 0;

HI_VOID ISR_WinReleaseUSLFrame(WINDOW_S *pstWin)
{
    HI_DRV_WIN_SRC_INFO_S *pstSource = &pstWin->stCfg.stSource;
    HI_DRV_VIDEO_FRAME_S stRlsFrm;
    HI_S32 nRet;

     /* release useless frame */

    nRet = WinBufferGetULSFrame(&pstWin->stBuffer, &stRlsFrm);
    while(!nRet)
    {
        if (stRlsFrm.bStillFrame)
        {
            WIN_DestroyStillFrame(&stRlsFrm);
        }
        else if (pstSource->pfRlsFrame)
        {
            pstSource->pfRlsFrame(pstSource->hSrc, &stRlsFrm);
#ifdef WIN_DEBUG_PRINT_RELEASE
            if (s_ResetPrint)
            {
                printk("Rel 006 fid=%d, addr=0x%x\n", 
                        stRlsFrm.u32FrameIndex,
                        stRlsFrm.stBufAddr[0].u32PhyAddr_Y);
            }
#endif
        }

        nRet = WinBufferGetULSFrame(&pstWin->stBuffer, &stRlsFrm);
    }
    
    return;
}

HI_DRV_VIDEO_FRAME_S *ISR_SlaveWinGetConfigFrame(WINDOW_S *pstWin)
{
    WINDOW_S *pstMstWin = (WINDOW_S *)(pstWin->pstMstWin);
    HI_DRV_VIDEO_FRAME_S *pstDstF, *pstRefF, *pstNew;

    // get master window crrent configed frame
    pstDstF = WinBuf_GetConfigedFrame(&pstMstWin->stBuffer.stWinBP);
    if (!pstDstF)
    {
        return HI_NULL;
    }

    // get slave window displayed frame
    pstRefF = WinBuf_GetDisplayedFrame(&pstWin->stBuffer.stWinBP);

    //pstNew = WinBuf_GetFrameByMaxID(&pstWin->stBuffer.stWinBP, pstRefF, RefID, enDstField);
    pstNew = WinBuf_GetFrameByDstFrame(&pstWin->stBuffer.stWinBP, pstDstF, pstRefF);

#if 0
    if (pstNew)
    {
        pstPriv = (HI_DRV_VIDEO_PRIVATE_S *)&(pstNew->u32Priv[0]);
        printk("S:d=%d,f=%d,F:id =%d,f=%d\n",  RefID, enDstField, pstNew->u32FrameIndex, pstPriv->eOriginField);
    }
#endif

    return pstNew;
}

HI_VOID WinUpdateDispInfo(WINDOW_S *pstWin, HI_DISP_DISPLAY_INFO_S * pstDsipInfo)
{
    pstWin->stDispInfo.u32RefreshRate = pstDsipInfo->u32RefreshRate;
    pstWin->stDispInfo.bIsInterlace   = pstDsipInfo->bInterlace;
    pstWin->stDispInfo.bIsBtm         = pstDsipInfo->bIsBottomField;

    return;
}

WIN_DISP_INFO_S *WinGetDispInfoByHandle(HI_HANDLE hWin)
{
    WINDOW_S *pstWin;

    pstWin = WinGetWindow(hWin);

    if (pstWin)
    {
        return &pstWin->stDispInfo;
    }

    return HI_NULL;
}

HI_VOID WinTestFrameMatch(WINDOW_S *pstWin, HI_DRV_VIDEO_FRAME_S *pstFrame, HI_DISP_DISPLAY_INFO_S * pstDsipInfo)
{
    //HI_DRV_WIN_ATTR_S *pstAttr = &pstWin->stUsingAttr;
    HI_DRV_VIDEO_PRIVATE_S *pstPriv = (HI_DRV_VIDEO_PRIVATE_S *)&(pstFrame->u32Priv[0]);

    // if display work at interlace mode and frame rate equale to display refresh rate, 
    // dectect whether top-field video frame output at top-field time
    if (pstDsipInfo->bInterlace == HI_TRUE)
    {
         if(  (  (pstPriv->eOriginField == HI_DRV_FIELD_TOP) 
                &&(pstDsipInfo->bIsBottomField == HI_FALSE) )
            ||(   (pstPriv->eOriginField == HI_DRV_FIELD_BOTTOM) 
                 &&(pstDsipInfo->bIsBottomField == HI_TRUE) )
            )
        {
            pstWin->u32TBNotMatchCount++;
#if 0
            if (pstWin->enDisp == HI_DRV_DISPLAY_0)
            {
                printk(">>>>>>>Disp=%d, fid=%d, f=%d, btm=%d\n", 
                              pstWin->enDisp,
                              pstFrame->u32FrameIndex,
                              pstPriv->eOriginField, 
                              pstDsipInfo->bIsBottomField);
            }
#endif

        }
#if 0
        else
        {
            if (pstWin->enDisp == HI_DRV_DISPLAY_0)
            {
                printk("Disp=%d, fid=%d, f=%d, btm=%d\n", 
                                  pstWin->enDisp,
                                  pstFrame->u32FrameIndex,
                                  pstPriv->eOriginField, 
                                  pstDsipInfo->bIsBottomField);
            }
        }
#endif
    }

    return;
}

#define FIDELITY_033            1
#define FIDELITY_18             2
#define FIDELITY_576I_YPBPR     3
#define FIDELITY_480I_YPBPR     4 
#define FIDELITY_CBAR_75        8
#define FIDELITY_MX625          10
#define FIDELITY_BOWTIE         18
#define FIDELITY_SKN            20
#define FIDELITY_ZDN            22
#define FIDELITY_MOTO_CVBS      25

#define VIDEO_TEST_SATURATION_OFFSET   (3)
#define VIDEO_MOTO_SATURATION_OFFSET    (3)
HI_VOID ISR_WinConfigFrame(WINDOW_S *pstWin, HI_DRV_VIDEO_FRAME_S *pstFrame, const HI_DRV_DISP_CALLBACK_INFO_S *pstInfo)
{
    HI_DRV_WIN_ATTR_S *pstAttr = &pstWin->stUsingAttr;
    WIN_HAL_PARA_S stLayerPara;
    HI_DRV_DISP_COLOR_SETTING_S stColor;
    HI_U32 u32Fidelity;
    HI_S32 nRet;
    VIDEO_LAYER_CAPABILITY_S  stVideoLayerCap;
    DISP_INTF_OPERATION_S *pstDispOpt;

#if 0
    HI_U32 Ct = 0;
    HI_DRV_VIDEO_PRIVATE_S *pstPriv = (HI_DRV_VIDEO_PRIVATE_S *)&(pstFrame->u32Priv[0]);

    HI_DRV_SYS_GetTimeStampMs((HI_U32 *)&Ct);

    if (pstWin->enType == HI_DRV_WIN_ACTIVE_MAIN_AND_SLAVE)
    {
        printk("m id=0x%x, f=%d,Ct=%d\n", pstFrame->u32FrameIndex, pstPriv->eOriginField,Ct);
    }
    else
    {
        printk("s id=0x%x, f=%d,Ct=%d\n", pstFrame->u32FrameIndex, pstPriv->eOriginField,Ct);
        //mdelay(3);
    }
#endif

    pstWin->stCfg.eDispMode = pstInfo->stDispInfo.eDispMode;
    pstWin->stCfg.bRightEyeFirst = pstInfo->stDispInfo.bRightEyeFirst;
    
//printk("     cfg Id=%d\n", pstFrame->u32FrameIndex);
    //pstAttr->stInRect  = pstFrame->stDispRect;
    //printk("fw=%d, fh=%d\n", pstFrame->stDispRect.s32Width, pstFrame->stDispRect.s32Height);
    //pstWin->stUsingAttr.stInRect.s32Width = 720;
    //pstWin->stUsingAttr.stInRect.s32Height = 576;

    stLayerPara.en3Dmode = pstInfo->stDispInfo.eDispMode;
    stLayerPara.bRightEyeFirst = pstInfo->stDispInfo.bRightEyeFirst;
    stLayerPara.pstFrame = pstFrame;

    
    pstWin->stVLayerFunc.PF_GetCapability(pstWin->u32VideoLayer,&stVideoLayerCap);
    stLayerPara.bZmeUpdate = HI_TRUE;
    stLayerPara.bZmeSupport = stVideoLayerCap.bZme;
    
    if (stLayerPara.bZmeSupport)
    {
        stLayerPara.stDisp  = pstAttr->stOutRect;
        stLayerPara.stVideo = pstAttr->stOutRect;
    }
    else
    {
        /*if not support zme ,config frame as frame info*/
        stLayerPara.stDisp  = pstFrame->stDispRect;
        stLayerPara.stVideo = pstFrame->stDispRect;
        stLayerPara.stVideo.s32X = pstAttr->stOutRect.s32X;
        stLayerPara.stVideo.s32Y = pstAttr->stOutRect.s32Y;
        stLayerPara.stDisp.s32X = pstAttr->stOutRect.s32X;
        stLayerPara.stDisp.s32Y = pstAttr->stOutRect.s32Y;
    }
    
    stLayerPara.stIn    = pstFrame->stDispRect; //pstAttr->stInRect;
    stLayerPara.stInOrigin = pstFrame->stDispRect;

    stLayerPara.pstDispInfo = (HI_DISP_DISPLAY_INFO_S *)&(pstInfo->stDispInfo);
    //stLayerPara.eField = pstInfo->stDispInfo.bIsBottomField ? ;
//printk("$=%d.", pstWin->eLayer);
    nRet = pstWin->stVLayerFunc.PF_SetFramePara(pstWin->u32VideoLayer, &stLayerPara);
    if (nRet)
    {
        pstWin->stVLayerFunc.PF_SetEnable(pstWin->u32VideoLayer, HI_FALSE);
    }
    else
    {
        pstWin->stVLayerFunc.PF_SetEnable(pstWin->u32VideoLayer, HI_TRUE);
    }

    stColor.enInCS  = ((HI_DRV_VIDEO_PRIVATE_S *)&pstFrame->u32Priv[0])->eColorSpace;
    u32Fidelity     = ((HI_DRV_VIDEO_PRIVATE_S *)&pstFrame->u32Priv[0])->u32Fidelity;
    stColor.enOutCS = pstInfo->stDispInfo.eColorSpace;

    // special process for special pattern
    if (pstWin->enDisp == HI_DRV_DISPLAY_0)
    {
        if ((u32Fidelity == FIDELITY_SKN) || (u32Fidelity == FIDELITY_ZDN) )
        {
            stColor.enOutCS = HI_DRV_CS_BT709_YUV_LIMITED;
        }
    }

    if (  pstWin->bDispInfoChange
        ||(stColor.enOutCS != pstWin->stCfg.enOutCS)
        ||(stColor.enInCS  != pstWin->stCfg.enFrameCS)
        ||(u32Fidelity     != pstWin->stCfg.u32Fidelity)
        ||(stColor.enInCS   == HI_DRV_CS_UNKNOWN)
        ||(stColor.enOutCS  == HI_DRV_CS_UNKNOWN)
        )
    {
        if (stColor.enInCS == HI_DRV_CS_UNKNOWN)
        {
            stColor.enInCS = HI_DRV_CS_BT709_YUV_LIMITED;
        }

        if (stColor.enOutCS == HI_DRV_CS_UNKNOWN)
        {
            stColor.enOutCS = HI_DRV_CS_BT709_YUV_LIMITED;
        }        

        pstWin->stCfg.enFrameCS = stColor.enInCS;
        pstWin->stCfg.enOutCS   = stColor.enOutCS;
        pstWin->stCfg.u32Fidelity = u32Fidelity;
        
        stColor.u32Bright = pstInfo->stDispInfo.u32Bright;
        stColor.u32Hue    = pstInfo->stDispInfo.u32Hue;
#if 0
        if (  (pstWin->enDisp == HI_DRV_DISPLAY_0)
            &&(   (u32Fidelity == FIDELITY_033) 
                ||( (u32Fidelity >= FIDELITY_CBAR_75) && ((u32Fidelity <= FIDELITY_MX625)))
                ||(u32Fidelity >= FIDELITY_SKN)
               )
           )
#endif
        if (  (pstWin->enDisp == HI_DRV_DISPLAY_0)
            &&(u32Fidelity > 0)
            &&(u32Fidelity != FIDELITY_BOWTIE)
            &&(u32Fidelity != FIDELITY_576I_YPBPR)
            &&(u32Fidelity != FIDELITY_480I_YPBPR)
            &&(u32Fidelity != FIDELITY_MOTO_CVBS)
           )
        {
            stColor.u32Satur  = (HI_U32)(pstInfo->stDispInfo.u32Satur + VIDEO_TEST_SATURATION_OFFSET);
        }
        else if (u32Fidelity == FIDELITY_MOTO_CVBS)
        {
            stColor.u32Satur  = (HI_U32)(pstInfo->stDispInfo.u32Satur + VIDEO_MOTO_SATURATION_OFFSET);
        }
        else
        {
            stColor.u32Satur  = pstInfo->stDispInfo.u32Satur;
        }

        /*t00177539 for 480i/576i multiburst   */
        pstDispOpt = DISP_HAL_GetOperationPtr();
        if (pstWin->enDisp == HI_DRV_DISPLAY_0)
        {
            if ((u32Fidelity == FIDELITY_576I_YPBPR)
                    || (u32Fidelity == FIDELITY_480I_YPBPR))
            {
                pstDispOpt->PF_DATE_SetCoef(HI_DRV_DISPLAY_0, HI_FALSE);
            }
            else
            {
                pstDispOpt->PF_DATE_SetCoef(HI_DRV_DISPLAY_0, HI_TRUE);
            }
        }

        //stColor.u32Satur  = (HI_U32)(pstInfo->stDispInfo.u32Satur);
        stColor.u32Contrst = pstInfo->stDispInfo.u32Contrst;
        stColor.u32Kr = pstInfo->stDispInfo.u32Kr;
        stColor.u32Kg = pstInfo->stDispInfo.u32Kg;
        stColor.u32Kb = pstInfo->stDispInfo.u32Kb;
        stColor.bGammaEnable       = HI_FALSE;
        stColor.bUseCustGammaTable = HI_FALSE;

        pstWin->stVLayerFunc.PF_SetColor(pstWin->u32VideoLayer, &stColor);
        pstWin->bDispInfoChange = HI_FALSE;
    }

    pstWin->stVLayerFunc.PF_Update(pstWin->u32VideoLayer);

    WinTestFrameMatch(pstWin, pstFrame, (HI_DISP_DISPLAY_INFO_S *)&pstInfo->stDispInfo);

    WinUpdateDispInfo(pstWin, (HI_DISP_DISPLAY_INFO_S *)&pstInfo->stDispInfo);

    return;
}

HI_VOID ISR_WinUpdatePlayInfo(WINDOW_S *pstWin, const HI_DRV_DISP_CALLBACK_INFO_S *pstInfo)
{
    // calc delay time in buffer queue
    //HI_U32 u32Num;
    HI_U32 T;
    //struct timeval tv;

    //BP_GetFullBufNum(&pstWin->stBuffer.stBP, &u32Num);

    DISP_ASSERT(pstInfo->stDispInfo.u32RefreshRate);
    DISP_ASSERT(pstInfo->stDispInfo.stFmtResolution.s32Height);

    pstWin->stDelayInfo.u32DispRate = pstInfo->stDispInfo.u32RefreshRate;
    pstWin->stDelayInfo.T = (1*1000*100)/pstInfo->stDispInfo.u32RefreshRate;
    //pstWin->stDelayInfo.u32FrameNumber = u32Num;

    //pstWin->stDelayInfo.bTBMatch = HI_TRUE;
    pstWin->stDelayInfo.bInterlace = pstInfo->stDispInfo.bInterlace;

    T = pstWin->stDelayInfo.T;

    if (pstInfo->stDispInfo.bInterlace)
    {
        pstWin->stDelayInfo.u32DisplayTime = (pstInfo->stDispInfo.u32Vline *2*T)/pstInfo->stDispInfo.stPixelFmtResolution.s32Height;
    }
    else
    {
        pstWin->stDelayInfo.u32DisplayTime = (pstInfo->stDispInfo.u32Vline *T)/pstInfo->stDispInfo.stPixelFmtResolution.s32Height;
    }
    
    HI_DRV_SYS_GetTimeStampMs((HI_U32 *)&pstWin->stDelayInfo.u32CfgTime);
    return;
}


HI_VOID ISR_WinStateTransfer(WINDOW_S *pstWin)
{
    if (pstWin->enState == WIN_STATE_WORK)
    {
        switch(pstWin->enStateNew)
        {
            case WIN_STATE_PAUSE:
            case WIN_STATE_FREEZE:
            {
                pstWin->enState = pstWin->enStateNew;
                pstWin->bUpState = HI_FALSE;
                return;
            }
            case WIN_STATE_WORK:
            case WIN_STATE_RESUME:
            case WIN_STATE_UNFREEZE:
            default :
                pstWin->bUpState = HI_FALSE;
                return;
        }
    }
    else if(pstWin->enState == WIN_STATE_PAUSE)
    {
        switch(pstWin->enStateNew)
        {
            case WIN_STATE_RESUME:
            case WIN_STATE_FREEZE:
            {
                pstWin->enState = pstWin->enStateNew;
                pstWin->bUpState = HI_FALSE;
                return;
            }
            case WIN_STATE_PAUSE:
            case WIN_STATE_WORK:
            case WIN_STATE_UNFREEZE:
            default :
                pstWin->bUpState = HI_FALSE;
                return;
        }
    }
    else if(pstWin->enState == WIN_STATE_FREEZE)
    {
        switch(pstWin->enStateNew)
        {
            case WIN_STATE_UNFREEZE:
            {
                pstWin->enState = pstWin->enStateNew;
                pstWin->bUpState = HI_FALSE;
                return;
            }
            case WIN_STATE_PAUSE:
            case WIN_STATE_FREEZE:
            case WIN_STATE_WORK:
            case WIN_STATE_RESUME:
            default :
                pstWin->bUpState = HI_FALSE;
                return;
        }
    }
   
    return;
}

HI_VOID ISR_WinResetState(WINDOW_S *pstWin)
{
    pstWin->enState = WIN_STATE_WORK;
    pstWin->bReset  = HI_FALSE;
    
    return;
}


HI_DRV_VIDEO_FRAME_S *WinSearchMatchFrame(WINDOW_S *pstWin)
{
    WIN_DISP_INFO_S *pstDispInfo;
    HI_DRV_FIELD_MODE_E enDstField;
    HI_DRV_VIDEO_FRAME_S *pstDispFrame, *pstNewFrame;

    // if display work at interlace mode
    pstDispInfo = &pstWin->stDispInfo;
    if(!pstDispInfo)
    {
         WIN_ERROR("WIN  null pointer in %s!\n", __FUNCTION__);
         return HI_NULL;
    }

#if 0
    if (pstDispInfo->bIsInterlace != HI_TRUE)
    {
        if(pstWin->enType == HI_DRV_WIN_ACTIVE_MAIN_AND_SLAVE)
        {
            pstDispInfo = WinGetDispInfoByHandle(pstWin->hSlvWin);
        }
    }
#else

    if (pstDispInfo->bIsInterlace != HI_TRUE)
    {
        // if master window output progressive frame, get slave window info
        if(pstWin->enType == HI_DRV_WIN_ACTIVE_MAIN_AND_SLAVE)
        {
            WIN_DISP_INFO_S *pstSlvDispInfo;
            
            pstSlvDispInfo = WinGetDispInfoByHandle(pstWin->hSlvWin);
            if (pstSlvDispInfo)
            {
                if (   (pstSlvDispInfo->bIsInterlace == HI_TRUE)
                     &&(pstDispInfo->u32RefreshRate == pstSlvDispInfo->u32RefreshRate)
                    )
                {
                    pstDispInfo = pstSlvDispInfo;
                }
            }
         }
    }
#endif

    
    if (!pstDispInfo || (pstDispInfo->bIsInterlace != HI_TRUE) )
    {
        // if output progressive picture, need not to search top/bottom field
        return WinBuf_GetConfigFrame(&pstWin->stBuffer.stWinBP);;
    }

    // if display work at interlace mode and frame rate equale to display refresh rate, 
    // dectect whether top-field video frame output at top-field time
    pstDispFrame = WinBuf_GetDisplayedFrame(&pstWin->stBuffer.stWinBP);
    enDstField = pstDispInfo->bIsBtm ? HI_DRV_FIELD_BOTTOM : HI_DRV_FIELD_TOP;


    pstNewFrame = WinBuf_GetFrameByDisplayInfo(&pstWin->stBuffer.stWinBP,
                                            pstDispFrame,
                                            pstDispInfo->u32RefreshRate,
                                            enDstField);
    return pstNewFrame;
}


HI_DRV_VIDEO_FRAME_S * WinGetFrameToConfig(WINDOW_S *pstWin, const HI_DRV_DISP_CALLBACK_INFO_S *pstInfo)
{
    HI_DRV_VIDEO_FRAME_S *pstFrame;
    
    if (pstWin->enType == HI_DRV_WIN_ACTIVE_SLAVE)
    {
        pstFrame = ISR_SlaveWinGetConfigFrame(pstWin);
    }
    else
    {
        if (pstWin->bQuickMode)
        {
            HI_DRV_VIDEO_FRAME_S *pstDispFrame;
            
            pstDispFrame = WinBuf_GetDisplayedFrame(&pstWin->stBuffer.stWinBP);
            pstFrame = WinBuf_GetNewestFrame(&pstWin->stBuffer.stWinBP, pstDispFrame);
        }
        else
        {
            pstFrame = WinSearchMatchFrame(pstWin);
        }
    }

    
    if (!pstFrame)
    {
        /*if not  receive frame*/
        pstWin->stBuffer.u32UnderLoad++;

        if (WinTestBlackFrameFlag(pstWin) != HI_TRUE)
        {
            /*
            1 :  if first config frame ,config nothing.
            2 :  if last config is not black frame, repeat last config frame*/
            WinBuf_RepeatDisplayedFrame(&pstWin->stBuffer.stWinBP);
            pstFrame = WinBuf_GetConfigedFrame(&pstWin->stBuffer.stWinBP);
        }
        else
        {
            /*if last config is black frame, repeat black frame*/
            pstFrame = BP_GetBlackFrameInfo();
            WinSetBlackFrameFlag(pstWin);
        }
    }
    else
    {
        /*if received frame*/
        WinClearBlackFrameFlag(pstWin);
    }

/*
    if(!pstFrame)
    {
        pstFrame = BP_GetBlackFrameInfo();
        WinSetBlackFrameFlag(pstWin);
    }
*/

    return pstFrame;
}


HI_VOID ISR_CallbackForWinProcess(HI_HANDLE hDst, const HI_DRV_DISP_CALLBACK_INFO_S *pstInfo)
{
    WINDOW_S *pstWin;
    HI_BOOL bUpDispInof = HI_FALSE;

    if (!hDst || !pstInfo )
    {
        WIN_ERROR("WIN Input null pointer in %s!\n", __FUNCTION__);
        return;
    }
    
    pstWin = (WINDOW_S *)hDst;
    
    if (pstInfo->eEventType != HI_DRV_DISP_C_VT_INT)
    {
        DISP_PRINT("@@@@@@@ DISP HI_DRV_DISP_C_event= %d, disp=%d\n", pstInfo->eEventType, pstWin->enDisp);
    }

    if (  (WIN_DEVICE_STATE_SUSPEND == s_s32WindowGlobalFlag)
        ||(pstInfo->eEventType == HI_DRV_DISP_C_PREPARE_CLOSE)
        ||(pstInfo->eEventType == HI_DRV_DISP_C_PREPARE_TO_PEND)
        )
    {
        DISP_PRINT(">>>>>>>>> mask\n");
        pstWin->bMasked = HI_TRUE;
    }
    else
    {
        pstWin->bMasked = HI_FALSE;
    }

    if (pstInfo->eEventType == HI_DRV_DISP_C_DISPLAY_SETTING_CHANGE)
        
    {
        pstWin->bDispInfoChange = HI_TRUE;
        bUpDispInof = HI_TRUE;
    }


    if (  (pstInfo->eEventType == HI_DRV_DISP_C_OPEN)
        ||(pstInfo->eEventType == HI_DRV_DISP_C_RESUME)
       )
    {
        bUpDispInof = HI_TRUE;
        pstWin->bDispInfoChange = HI_TRUE;
    }

    if (atomic_read(&pstWin->stCfg.bNewAttrFlag))
    {
        pstWin->stCfg.stAttr = pstWin->stCfg.stAttrBuf;
        
        atomic_set(&pstWin->stCfg.bNewAttrFlag, 0);        

        bUpDispInof = HI_TRUE;
        pstWin->bDispInfoChange = HI_TRUE;
    }

    if (bUpDispInof)
    {
        pstWin->stUsingAttr = pstWin->stCfg.stAttr;
        
        if (!pstWin->stCfg.stAttr.stOutRect.s32Width || !pstWin->stCfg.stAttr.stOutRect.s32Height)
        {
            pstWin->stCfg.stAttr.stOutRect = pstInfo->stDispInfo.stVirtaulScreen;
        }

#if 0
        printk("VS=%d,%d, FR=%d,%d, PFR=%d,%d, O=%d,%d, U=%d,%d\n", 
                  pstInfo->stDispInfo.stVirtaulScreen.s32Width,
                  pstInfo->stDispInfo.stVirtaulScreen.s32Height,
                  pstInfo->stDispInfo.stFmtResolution.s32Width,
                  pstInfo->stDispInfo.stFmtResolution.s32Height,
                  pstInfo->stDispInfo.stPixelFmtResolution.s32Width,
                  pstInfo->stDispInfo.stPixelFmtResolution.s32Height,
                  pstWin->stCfg.stAttr.stOutRect.s32Width,
                  pstWin->stCfg.stAttr.stOutRect.s32Height,
                  pstWin->stUsingAttr.stOutRect.s32Width,
                  pstWin->stUsingAttr.stOutRect.s32Height);
#endif

        Win_ReviseOutRect(&pstInfo->stDispInfo.stVirtaulScreen,
                          &pstInfo->stDispInfo.stOffsetInfo,
                          &pstInfo->stDispInfo.stFmtResolution,
                          &pstWin->stCfg.stAttr.stOutRect,
                          &pstWin->stUsingAttr.stOutRect);  
#if 0
        printk("   VS=%d,%d, FR=%d,%d, PFR=%d,%d, O=%d,%d, U=%d,%d\n", 
                  pstInfo->stDispInfo.stVirtaulScreen.s32Width,
                  pstInfo->stDispInfo.stVirtaulScreen.s32Height,
                  pstInfo->stDispInfo.stFmtResolution.s32Width,
                  pstInfo->stDispInfo.stFmtResolution.s32Height,
                  pstInfo->stDispInfo.stPixelFmtResolution.s32Width,
                  pstInfo->stDispInfo.stPixelFmtResolution.s32Height,
                  pstWin->stCfg.stAttr.stOutRect.s32Width,
                  pstWin->stCfg.stAttr.stOutRect.s32Height,
                  pstWin->stUsingAttr.stOutRect.s32Width,
                  pstWin->stUsingAttr.stOutRect.s32Height);
#endif

        // TODO:  if outrect is outide screen, 
        if (!pstWin->stUsingAttr.stOutRect.s32Width || !pstWin->stUsingAttr.stOutRect.s32Height)
        {
            pstWin->stUsingAttr.stOutRect = pstInfo->stDispInfo.stFmtResolution;
        }

        WinSendAttrToSource(pstWin, (HI_DISP_DISPLAY_INFO_S *)&pstInfo->stDispInfo);

        // create window play info
        //WinUpdatePlayInfo(&pstWin->stPlay, pstInfo->stDispInfo.u32RefreshRate);

        DISP_PRINT("Display info>> M=%d,S=%d,att=%d, 3d=%d, R=%d, I=%d, w=%d, h=%d, %dvs%d, rate=%d, cs=%d\n", 
                pstInfo->stDispInfo.bIsMaster,
                pstInfo->stDispInfo.bIsSlave,
                pstInfo->stDispInfo.enAttachedDisp,
                pstInfo->stDispInfo.eDispMode,
                pstInfo->stDispInfo.bRightEyeFirst,
                pstInfo->stDispInfo.bInterlace, 
                pstInfo->stDispInfo.stPixelFmtResolution.s32Width,
                pstInfo->stDispInfo.stPixelFmtResolution.s32Height,
                pstInfo->stDispInfo.stAR.u8ARw,
                pstInfo->stDispInfo.stAR.u8ARh,
                pstInfo->stDispInfo.u32RefreshRate,
                pstInfo->stDispInfo.eColorSpace);
    }

    if (!pstWin->bEnable || pstWin->bMasked)
    {
        pstWin->stVLayerFunc.PF_SetEnable(pstWin->u32VideoLayer, HI_FALSE);
        pstWin->stVLayerFunc.PF_Update(pstWin->u32VideoLayer);
        return;
    }

    pstWin->bInInterrupt = HI_TRUE;

    // window process
    if ( (HI_DRV_DISP_C_VT_INT == pstInfo->eEventType) 
        || (HI_DRV_DISP_C_DISPLAY_SETTING_CHANGE == pstInfo->eEventType))
    {
        HI_DRV_VIDEO_FRAME_S *pstFrame = HI_NULL;


        //printk("win003,");
        if (pstWin->bReset)
        {
        //printk("win004,");
            // release displayed and configed frame
            s_ResetPrint = 1;

            //ISR_WinReleaseDisplayedFrame(pstWin);
            WinBuf_RlsAndUpdateUsingFrame(&pstWin->stBuffer.stWinBP);
            
            ISR_WinReleaseUSLFrame(pstWin);

            // flush frame in full buffer pool
            //ISR_WinReleaseFullFrame(pstWin);
            pstFrame = WinBuf_GetDisplayedFrame(&pstWin->stBuffer.stWinBP);
            WinBuf_FlushWaitingFrame(&pstWin->stBuffer.stWinBP, pstFrame);

            if (pstWin->stRst.enResetMode == HI_DRV_WIN_SWITCH_BLACK)
            {
                //ISR_WinReleaseDisplayedFrame2(pstWin);
                // get and config black frame
                pstFrame = BP_GetBlackFrameInfo();
                
                if (!pstFrame)
                {
                    DISP_ERROR("get black frame null \n");
                    return;
                }
                WinSetBlackFrameFlag(pstWin);
                
                ISR_WinConfigFrame(pstWin, pstFrame, pstInfo);
            }
            else
            {
                //pstFrame = ISR_WinGetLastFrame(&(pstWin->stBuffer));
                WinBuf_RepeatDisplayedFrame(&pstWin->stBuffer.stWinBP);
                pstFrame = WinBuf_GetConfigedFrame(&pstWin->stBuffer.stWinBP);

                //DISP_PRINT("RESET Frame addr=0x%x\n", pstFrame->stBufAddr[0].u32PhyAddr_Y);
                if (!pstFrame)
                {
                        pstFrame = BP_GetBlackFrameInfo();
                }
                
                if (!pstFrame)
                {
                    DISP_ERROR("get black frame null \n");
                    return;
                }

                ISR_WinConfigFrame(pstWin, pstFrame, pstInfo);
                WinClearBlackFrameFlag(pstWin);
            }

            s_ResetPrint = 0;

            pstWin->enState = WIN_STATE_WORK;
            pstWin->bReset  = HI_FALSE;
        }
        else
        {
            // window state transfer
            if (pstWin->bUpState)
            {
                ISR_WinStateTransfer(pstWin);
            }

            //printk("==win state=%d>>", pstWin->enState);

            switch(pstWin->enState)
            {
                case WIN_STATE_RESUME:
                case WIN_STATE_UNFREEZE:
                {
                    pstWin->enState = WIN_STATE_WORK;
                    // no break, enter case 'WIN_STATE_WORK'
                }
                case WIN_STATE_WORK:
                {
                    //ISR_WinReleaseDisplayedFrame(pstWin);
                    WinBuf_RlsAndUpdateUsingFrame(&pstWin->stBuffer.stWinBP);
                    
                    ISR_WinReleaseUSLFrame(pstWin);

                    pstFrame = WinGetFrameToConfig(pstWin, pstInfo);

                    if(pstFrame)
                    {
                        ISR_WinConfigFrame(pstWin, pstFrame, pstInfo);
                    }
                    
                    break;
                }
                case WIN_STATE_PAUSE:
                {
                    //ISR_WinReleaseDisplayedFrame(pstWin);
                    WinBuf_RlsAndUpdateUsingFrame(&pstWin->stBuffer.stWinBP);

                    ISR_WinReleaseUSLFrame(pstWin);
                    
                    //pstFrame = ISR_WinGetLastFrame(&(pstWin->stBuffer));
                    WinBuf_RepeatDisplayedFrame(&pstWin->stBuffer.stWinBP);
                    pstFrame = WinBuf_GetConfigedFrame(&pstWin->stBuffer.stWinBP);

                    if (!pstFrame)
                    {
                        pstFrame = BP_GetBlackFrameInfo();
                        WinSetBlackFrameFlag(pstWin);
                    }

                    if (pstFrame)
                    {
                        ISR_WinConfigFrame(pstWin, pstFrame, pstInfo);
                    }
                    
                    break;
                }
                case WIN_STATE_FREEZE:
                {
                    HI_S32 nRet;
                    
                    //ISR_WinReleaseDisplayedFrame(pstWin);
                    WinBuf_RlsAndUpdateUsingFrame(&pstWin->stBuffer.stWinBP);
                    
                    ISR_WinReleaseUSLFrame(pstWin);
                    
                    //pstFrame = ISR_WinGetFreezeFrame(pstWin);
                    WinBuf_RepeatDisplayedFrame(&pstWin->stBuffer.stWinBP);

                    pstFrame = WinBuf_GetConfigedFrame(&pstWin->stBuffer.stWinBP);
                    nRet = WinBuf_ReleaseOneFrame(&pstWin->stBuffer.stWinBP, pstFrame);
                    if (nRet != HI_SUCCESS)
                    {
                        // u32UnderLoad happened
                        pstWin->stBuffer.u32UnderLoad++;
                    }

                    if (pstWin->stFrz.enFreezeMode == HI_DRV_WIN_SWITCH_BLACK)
                    {
                        // find black frame and set flag
                        pstFrame = BP_GetBlackFrameInfo();
                        WinSetBlackFrameFlag(pstWin);
                    }
                    else if (pstFrame)
                    {
                        // if NO_BLACK_FREEZE and has frezen frame, clear flag
                        WinClearBlackFrameFlag(pstWin);
                    }

                    if (pstFrame)
                    {
                        ISR_WinConfigFrame(pstWin, pstFrame, pstInfo);
                    }
                    
                    break;
                }
                default:
                    break;
            }
        }
    }

    ISR_WinUpdatePlayInfo(pstWin, pstInfo);

    pstWin->bInInterrupt = HI_FALSE;

    return;
}


HI_S32 WinGetProcIndex(HI_HANDLE hWin, HI_U32 *p32Index)
{
    WINDOW_S *pstWin;

    WinCheckDeviceOpen();

    WinCheckNullPointer(p32Index);

    // s1 get window pointer
    pstWin = WinGetWindow(hWin);
    if (pstWin)
    {
        // return active windwo index
        *p32Index = pstWin->u32Index;
    }
    else
    {
        VIRTUAL_S *pstVirWin;
        
        pstVirWin = WinGetVirWindow(hWin);
        if (pstVirWin)
        {
            //return virtual window index
            *p32Index = pstVirWin->u32Index;
        }
        else
        {
            return HI_ERR_VO_WIN_NOT_EXIST;
        }
    }

    return HI_SUCCESS;
}

HI_S32 WinGetProcInfo(HI_HANDLE hWin, WIN_PROC_INFO_S *pstInfo)
{
    WINDOW_S *pstWin;

    WinCheckDeviceOpen();

    WinCheckNullPointer(pstInfo);

    DISP_MEMSET(pstInfo, 0, sizeof(WIN_PROC_INFO_S));

    // s1 get active window pointer
    pstWin = WinGetWindow(hWin);
    if (pstWin)
    {
        // get window proc info
        pstInfo->enType     = pstWin->enType;
        pstInfo->u32Index   = pstWin->u32Index;
        pstInfo->u32LayerId = (HI_U32)pstWin->u32VideoLayer;
        pstWin->stVLayerFunc.PF_GetZorder(pstWin->u32VideoLayer, &pstInfo->u32Zorder);
        pstInfo->bEnable   = (HI_U32)pstWin->bEnable;
        pstInfo->bMasked   = (HI_U32)pstWin->bMasked;
        pstInfo->u32WinState = (HI_U32)pstWin->enState;
        
        pstInfo->bReset = pstWin->bReset;
        pstInfo->enResetMode = pstWin->stRst.enResetMode;
        pstInfo->enFreezeMode = pstWin->stFrz.enFreezeMode;
        
        pstInfo->bQuickMode = pstWin->bQuickMode;
        pstInfo->bStepMode  = pstWin->bStepMode;

        pstInfo->hSrc          = (HI_U32)pstWin->stCfg.stSource.hSrc;
        pstInfo->pfAcqFrame    = (HI_U32)pstWin->stCfg.stSource.pfAcqFrame;
        pstInfo->pfRlsFrame    = (HI_U32)pstWin->stCfg.stSource.pfRlsFrame;
        pstInfo->pfSendWinInfo = (HI_U32)pstWin->stCfg.stSource.pfSendWinInfo;

        pstInfo->stAttr  = pstWin->stCfg.stAttr;

        pstInfo->u32TBNotMatchCount = pstWin->u32TBNotMatchCount;

        pstInfo->eDispMode = pstWin->stCfg.eDispMode;
        pstInfo->bRightEyeFirst = pstWin->stCfg.bRightEyeFirst;

        pstInfo->hSlvWin  = (HI_U32)pstWin->hSlvWin;
        pstInfo->bDebugEn = (HI_U32)pstWin->bDebugEn;

        pstInfo->u32ULSIn  = pstWin->stBuffer.u32ULSIn;
        pstInfo->u32ULSOut = pstWin->stBuffer.u32ULSOut;
        pstInfo->u32UnderLoad = pstWin->stBuffer.u32UnderLoad;

        //BP_GetBufState(&pstWin->stBuffer.stBP, (BUF_STT_S *)&(pstInfo->stBufState));
        WinBuf_GetStateInfo(&pstWin->stBuffer.stWinBP, (WB_STATE_S *)&(pstInfo->stBufState));
    }
    else
    {
        VIRTUAL_S *pstVirWin;
        
        pstVirWin = WinGetVirWindow(hWin);
        if (pstVirWin)
        {
            // get virtual proc info
            pstInfo->enType     = pstVirWin->enType;
            pstInfo->u32Index   = pstVirWin->u32Index;
            pstInfo->u32LayerId = 0xFFFFul;  // not use video layer
            pstInfo->bEnable   = (HI_U32)pstVirWin->bEnable;
            pstInfo->bMasked   = (HI_U32)pstVirWin->bMasked;
            pstInfo->u32WinState = 0;  // 0 means work

            pstInfo->hSrc          = (HI_U32)pstVirWin->stSrcInfo.hSrc;
            pstInfo->pfAcqFrame    = (HI_U32)pstVirWin->stSrcInfo.pfAcqFrame;
            pstInfo->pfRlsFrame    = (HI_U32)pstVirWin->stSrcInfo.pfRlsFrame;
            pstInfo->pfSendWinInfo = (HI_U32)pstVirWin->stSrcInfo.pfSendWinInfo;

            pstInfo->stAttr = pstVirWin->stAttrBuf;


            pstInfo->stBufState.stRecord.u32TryQueueFrame = pstVirWin->stFrameStat.u32SrcQTry;
            pstInfo->stBufState.stRecord.u32QueueFrame    = pstVirWin->stFrameStat.u32SrcQOK;

            pstInfo->stBufState.stRecord.u32Release = pstVirWin->stFrameStat.u32SinkRlsOK;
        }
        else
        {
            WIN_ERROR("WIN is not exist!\n");
            return HI_ERR_VO_WIN_NOT_EXIST;
        }
    } 

    return HI_SUCCESS;
}

HI_S32 WinGetCurrentImg(HI_HANDLE hWin, HI_DRV_VIDEO_FRAME_S *pstFrame)
{
    WINDOW_S *pstWin;
    HI_DRV_VIDEO_FRAME_S *pstFrmTmp = NULL;
    
    WinCheckDeviceOpen();
    WinCheckNullPointer(pstFrame);
    WinCheckWindow(hWin, pstWin);    
    DISP_MEMSET(pstFrame, 0, sizeof(HI_DRV_VIDEO_FRAME_S));
    
    pstFrmTmp = WinBuf_GetDisplayedFrame(&pstWin->stBuffer.stWinBP);    
    if (NULL == pstFrmTmp)
    {   
        if (pstWin->latest_display_frame_valid)
        {
            *pstFrame = pstWin->latest_display_frame;
        } 
        else
        {        
            return HI_FAILURE;
        }
    }
    else
    {
         *pstFrame = *(pstFrmTmp);
         pstWin->latest_display_frame = *(pstFrmTmp);
         pstWin->latest_display_frame_valid = 1;
    }
    
    return HI_SUCCESS;
}

HI_S32 WinCaptureFrame(HI_HANDLE hWin, HI_DRV_VIDEO_FRAME_S *pstFrame, HI_U32 *stMMZPhyAddr, HI_U32 *stMMZlen)
{
    WINDOW_S *pstWin;
    HI_DRV_VIDEO_FRAME_S *pstFrmTmp = NULL;
    HI_VDEC_PRIV_FRAMEINFO_S *pstPrivInfo = HI_NULL;
    HI_U32 datalen = 0, y_stride = 0, height = 0;
   
    WinCheckDeviceOpen();
    WinCheckNullPointer(pstFrame);
    WinCheckNullPointer(stMMZPhyAddr);
    WinCheckNullPointer(stMMZlen);
    WinCheckWindow(hWin, pstWin);    
    DISP_MEMSET(pstFrame, 0, sizeof(HI_DRV_VIDEO_FRAME_S));
    
    if (pstWin->enType == HI_DRV_WIN_VITUAL_SINGLE)
        return HI_FAILURE;

    if(HI_SUCCESS != WinBuf_SetCaptureFrame(&pstWin->stBuffer.stWinBP, pstWin->u32WinCapMMZvalid))    
        return HI_FAILURE;
    
    pstFrmTmp = WinBuf_GetCapturedFrame(&pstWin->stBuffer.stWinBP);    
    if (HI_NULL == pstFrmTmp)
    {
        pstFrmTmp = BP_GetBlackFrameInfo();
    }
    
    if (HI_NULL == pstFrmTmp)
    {
        return HI_FAILURE;
    }
    
    *pstFrame = *(pstFrmTmp);         
    
    pstPrivInfo = (HI_VDEC_PRIV_FRAMEINFO_S *)(pstFrame->u32Priv);    

    y_stride = pstFrame->stBufAddr[0].u32Stride_Y;
    height   = (HI_TRUE == pstPrivInfo->stCompressInfo.u32CompressFlag)
                     ? pstPrivInfo->stCompressInfo.s32CompFrameHeight : pstFrame->u32Height;        
    
    /*don't know why there exists these two branches, do we condidered gstreamer?*/
    if ( HI_DRV_PIX_FMT_NV21 == pstFrame->ePixFormat) 
        datalen = height * y_stride * 3 / 2 + height * 4;
    else 
        datalen = height * y_stride * 2 + height * 4;        
    
    
    if(HI_SUCCESS != DISP_OS_MMZ_Alloc("VDP_Capture", HI_NULL, datalen, 16, &pstWin->stWinCaptureMMZ))
    {
        WinBuf_ReleaseCaptureFrame(&pstWin->stBuffer.stWinBP,pstFrame,HI_FALSE);
        return HI_FAILURE;
    }
    
    pstWin->u32WinCapMMZvalid  = 1;
    *stMMZPhyAddr = pstWin->stWinCaptureMMZ.u32StartPhyAddr;
    *stMMZlen     = pstWin->stWinCaptureMMZ.u32Size;
    
    return HI_SUCCESS;
}

HI_S32 WinReleaseCaptureFrame(HI_HANDLE hWin, HI_DRV_VIDEO_FRAME_S *pstFrame)
{
    WINDOW_S *pstWin;
    
    WinCheckDeviceOpen();
    WinCheckNullPointer(pstFrame);
    WinCheckWindow(hWin, pstWin);    

    if (pstWin->enType == HI_DRV_WIN_VITUAL_SINGLE)
        return HI_FAILURE;

    if(HI_SUCCESS != WinBuf_ReleaseCaptureFrame(&pstWin->stBuffer.stWinBP,pstFrame, HI_FALSE))    
        return HI_FAILURE;
    
    return HI_SUCCESS;    
}


HI_S32 WinFreeCaptureMMZBuf(HI_HANDLE hWin, HI_DRV_VIDEO_FRAME_S *cap_frame)
{
    WINDOW_S *pstWin;    
    
    WinCheckDeviceOpen();
    WinCheckWindow(hWin, pstWin);

    if (cap_frame->stBufAddr[0].u32PhyAddr_Y 
        != pstWin->stWinCaptureMMZ.u32StartPhyAddr)
    {
        WIN_ERROR("capture release addr:%x,%x!\n", cap_frame->stBufAddr[0].u32PhyAddr_Y, 
            pstWin->stWinCaptureMMZ.u32StartPhyAddr);
        return HI_FAILURE;
    }   

    if (pstWin->u32WinCapMMZvalid == 1) {
        DISP_OS_MMZ_Release(&pstWin->stWinCaptureMMZ);
        pstWin->stWinCaptureMMZ.u32StartPhyAddr = 0;
        pstWin->u32WinCapMMZvalid = 0;
    }
    else {
        WIN_WARN("warning: when free mmz, null refcnt occurs.!\n");
        return HI_FAILURE;
    }
    
    return HI_SUCCESS;
}


HI_S32 WinFreeCaptureMMZBuf2(WINDOW_S *pstWin)
{   
    
    if (pstWin->u32WinCapMMZvalid == 1) {
        DISP_OS_MMZ_Release(&pstWin->stWinCaptureMMZ);
        pstWin->stWinCaptureMMZ.u32StartPhyAddr = 0;
        pstWin->u32WinCapMMZvalid = 0;
    }
    else {
        WIN_WARN("warning: when free mmz, null refcnt occurs.!\n");
    }
    
    return HI_SUCCESS;
}


HI_S32 WinForceClearCapture(HI_HANDLE hWin)
{
    HI_DRV_VIDEO_FRAME_S *pstFrmTmp = NULL;
    WINDOW_S *pstWin = HI_NULL;

    WinCheckDeviceOpen();     
    WinCheckWindow(hWin, pstWin);   

    pstFrmTmp = WinBuf_GetCapturedFrame(&pstWin->stBuffer.stWinBP);    
    if (NULL != pstFrmTmp)
    {  
        if(HI_SUCCESS != WinBuf_ReleaseCaptureFrame(&pstWin->stBuffer.stWinBP,pstFrmTmp, HI_TRUE))
            return HI_FAILURE;
    }    
    if( HI_SUCCESS != WinFreeCaptureMMZBuf2(pstWin))
        return HI_FAILURE;

    return HI_SUCCESS;
}


HI_S32 WinCapturePause(HI_HANDLE hWin, HI_BOOL bCaptureStart)
{
    WINDOW_S *pstWin = HI_NULL, *pstWinAttach = HI_NULL;
    HI_S32 Ret = 0;
    HI_HANDLE  attach_handle = 0;
    
    pstWin = WinGetWindow(hWin);     
    if (!pstWin)                      
    {
        return HI_FAILURE;
    }
    
    if (bCaptureStart && ((pstWin->enState == WIN_STATE_PAUSE)
        || (pstWin->enState == WIN_STATE_FREEZE)))
    {
        pstWin->bRestoreFlag = 0; 
        return HI_SUCCESS;
    }

    if (pstWin->enType == HI_DRV_WIN_ACTIVE_MAIN_AND_SLAVE) 
    {
        /*attach mode , need  pasue salve window too*/
        attach_handle = pstWin->hSlvWin;
        pstWinAttach = WinGetWindow(attach_handle);   
         
        if (!pstWinAttach)                      
        { 
            attach_handle = 0;
        }    
    }
    else if (pstWin->enType == HI_DRV_WIN_ACTIVE_SINGLE)
    {
        /*not attach mode only need  pasue self window*/
        attach_handle = 0;      
    }
    else if (pstWin->enType == HI_DRV_WIN_ACTIVE_SLAVE)
    {          
        /*attach mode only  mce capture slave window*/
        attach_handle = 0;    
    }
    
    if (bCaptureStart)
    {
        Ret = WIN_Pause(hWin, bCaptureStart);       
        if (Ret != HI_SUCCESS)
            return HI_FAILURE;
        
        if (attach_handle)
        {
            Ret = WIN_Pause(attach_handle, bCaptureStart);    
            if (Ret != HI_SUCCESS)             
            {
                (HI_VOID)WIN_Pause(hWin, !bCaptureStart);
                return HI_FAILURE;
            }   
        }

        pstWin->bRestoreFlag = 1;        
    }
    else
    {   
        if (pstWin->bRestoreFlag) {

            Ret = WIN_Pause(hWin, 0);
            if (attach_handle)
            {
                Ret |= WIN_Pause(attach_handle, 0); 
            }
            pstWin->bRestoreFlag = 0;        

            if (Ret != HI_SUCCESS) 
                return HI_FAILURE;

        }
    }
   
    return HI_SUCCESS;
}


#ifdef __cplusplus
 #if __cplusplus
}
 #endif
#endif /* __cplusplus */




