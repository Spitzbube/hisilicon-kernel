
/******************************************************************************
  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.
******************************************************************************
File Name     : drv_win_hal.h
Version       : Initial Draft
Author        : Hisilicon multimedia software group
Created       : 2012/12/30
Last Modified :
Description   :
Function List :
History       :
******************************************************************************/
#ifndef __DRV_WIN_HAL_H__
#define __DRV_WIN_HAL_H__


#include "hi_type.h"
#include "drv_disp_com.h"
#include "vdp_define.h"
#include "vdp_driver.h"


#ifdef __cplusplus
 #if __cplusplus
extern "C" {
 #endif
#endif /* __cplusplus */


#define HI_WIN_IN_RECT_X_ALIGN      0xFFFFFFF0ul
#define HI_WIN_IN_RECT_WIDTH_ALIGN  0xFFFFFFFCul
#define HI_WIN_IN_RECT_Y_ALIGN      0xFFFFFFFCul
#define HI_WIN_IN_RECT_HEIGHT_ALIGN 0xFFFFFFFCul

#define HI_WIN_OUT_RECT_X_ALIGN      0xFFFFFFFEul
#define HI_WIN_OUT_RECT_WIDTH_ALIGN  0xFFFFFFFEul
#define HI_WIN_OUT_RECT_Y_ALIGN      0xFFFFFFFCul
#define HI_WIN_OUT_RECT_HEIGHT_ALIGN 0xFFFFFFFCul


#define VIDEO_ZOOM_IN_VERTICAL_MAX   4
#define VIDEO_ZOOM_IN_HORIZONTAL_MAX 4

#define DEF_VIDEO_LAYER_MAX_NUMBER 6
#define DEF_VIDEO_LAYER_INVALID_ID 0xff


typedef enum tagVIDEO_LAYER_ACC_MODE_E
{
    VIDEO_LAYER_ACC_DISABLE = 0,
    VIDEO_LAYER_ACC_AUTO,
    VIDEO_LAYER_ACC_MODE_BUTT
}VIDEO_LAYER_ACC_MODE_E;


typedef enum tagVIDEO_LAYER_ACM_MODE_E
{
    VIDEO_LAYER_ACM_DISABLE = 0,
    VIDEO_LAYER_ACM_AUTO,
    VIDEO_LAYER_ACM_MODE_BUTT
}VIDEO_LAYER_ACM_MODE_E;

typedef struct tagVIDEO_LAYER_CAPABILITY_S
{
    HI_BOOL bSupport;
    HI_U32  eId;
    HI_BOOL bZme;
    HI_BOOL bACC;
    HI_BOOL bACM;
    HI_BOOL bLTICTI;

    HI_BOOL bHDIn;
    HI_BOOL bHDOut;
}VIDEO_LAYER_CAPABILITY_S;


typedef struct tagVIDEO_LAYER_S
{
    HI_BOOL bWorking;
}VIDEO_LAYER_S;



typedef struct tagVIDEO_LAYER_FRAME_PARA_S
{
    HI_DRV_FRAME_TYPE_E  eFrmType;
    HI_DRV_PIX_FORMAT_E  eVideoFormat;
   
    HI_BOOL bInterlaced;
    HI_BOOL bTopFirst;

    HI_RECT_S stIn;
    HI_RECT_S stDisp;
    HI_RECT_S stVideo;
    
    HI_DRV_COLOR_SPACE_E eSrcCS;
    HI_DRV_COLOR_SPACE_E eDstCs;

    HI_U32 u32AddrNumber;
    HI_DRV_VID_FRAME_ADDR_S stAddr[2];

}VIDEO_LAYER_FRAME_PARA_S;


typedef struct tagWIN_HAL_PARA_S
{
    HI_DRV_DISP_STEREO_E en3Dmode;
    HI_BOOL bRightEyeFirst;

    HI_DRV_VIDEO_FRAME_S *pstFrame;

    HI_BOOL bZmeUpdate;
    HI_BOOL bZmeSupport;
    HI_RECT_S stIn;
    
    /* since some platform use complete width , 
     * some use cropped width,so we add a original one.
     */
    HI_RECT_S stInOrigin;
    HI_RECT_S stVideo;
    HI_RECT_S stDisp;
    
    HI_DISP_DISPLAY_INFO_S *pstDispInfo;
    HI_DRV_DISP_FIELD_FLAG_E eField;
}WIN_HAL_PARA_S;


typedef struct tagVIDEO_LAYER_FUNCTIONG_S
{
    HI_S32 (*PF_GetCapability)(HI_U32 u32Layer, VIDEO_LAYER_CAPABILITY_S *pstSurf);

    HI_S32 (*PF_AcquireLayer)(HI_U32 u32Layer);
    HI_S32 (*PF_AcquireLayerByDisplay)(HI_DRV_DISPLAY_E eDisp, HI_U32 *pu32Layer);
    HI_S32 (*PF_ReleaseLayer)(HI_U32 u32Layer);

    HI_S32 (*PF_SetEnable)(HI_U32 u32Layer, HI_BOOL bEnable);
    HI_S32 (*PF_Update)(HI_U32 u32Layer);

    HI_S32 (*PF_SetDefault)(HI_U32 u32Layer);
    HI_S32 (*PF_SetAllLayerDefault)(HI_VOID);

    HI_S32 (*PF_SetDispMode)(HI_U32 u32Layer, HI_DRV_DISP_STEREO_MODE_E eMode);
    HI_S32 (*PF_SetIORect)(HI_U32 u32Layer, HI_RECT_S *in, HI_RECT_S *disp, HI_RECT_S *video);
    HI_S32 (*PF_SetColor)(HI_U32 u32Layer, HI_DRV_DISP_COLOR_SETTING_S *pstColor);
    HI_S32 (*PF_SetPixFmt)(HI_U32 u32Layer, HI_DRV_PIX_FORMAT_E eFmt);
    HI_S32 (*PF_SetAddr)(HI_U32 u32Layer, HI_DRV_VID_FRAME_ADDR_S *pstAddr, HI_U32 u32Num);

    HI_S32 (*PF_MovUp)(HI_U32 u32Layer);
    HI_S32 (*PF_MovTop)(HI_U32 u32Layer);
    HI_S32 (*PF_MovDown)(HI_U32 u32Layer);
    HI_S32 (*PF_MovBottom)(HI_U32 u32Layer);

    HI_S32 (*PF_GetZorder)(HI_U32 u32Layer, HI_U32 *pZOrder);

    HI_S32 (*PF_SetACC)(HI_U32 u32Layer, VIDEO_LAYER_ACC_MODE_E eMode);
    HI_S32 (*PF_SetACM)(HI_U32 u32Layer, VIDEO_LAYER_ACM_MODE_E eMode);

    HI_S32 (*PF_SetDebug)(HI_U32 u32Layer, HI_BOOL bEnable);

    HI_S32 (*PF_SetFramePara)(HI_U32 u32Layer, WIN_HAL_PARA_S *pstPara);
    HI_S32 (*PF_Get3DOutRect)(HI_DRV_DISP_STEREO_E en3DMode, HI_RECT_S *pstOutRect, HI_RECT_S *pstReviseOutRect);
}VIDEO_LAYER_FUNCTIONG_S;

HI_S32 VideoLayer_Init(HI_DRV_DISP_VERSION_S *pstVersion);
HI_S32 VideoLayer_DeInit(HI_VOID);

HI_S32 VideoLayer_GetFunction(VIDEO_LAYER_FUNCTIONG_S *pstFunc);

VIDEO_LAYER_FUNCTIONG_S *VideoLayer_GetFunctionPtr(HI_VOID);



#ifdef __cplusplus
 #if __cplusplus
}
 #endif
#endif /* __cplusplus */

#endif /*  __DRV_VO_HAL_H__  */










