#ifndef __OPTM_HIFB_H__
#define __OPTM_HIFB_H__


#include <linux/fb.h>
#include "hi_tde_type.h"
#include "hifb.h"
#include "hifb_drv_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */


typedef struct
{
    HI_BOOL   bInRegionClip;
    HI_BOOL   bClip;
    HIFB_RECT stClipRect;
}HIFB_CLIP_S;

typedef struct
{
	HI_BOOL bScale;
    HI_BOOL bBlock;
    HI_BOOL bCallBack;
	HI_BOOL bRegionDeflicker;
	HI_VOID *pParam;
	HI_U32 u32CmapAddr;
	HIFB_LAYER_ANTIFLICKER_LEVEL_E enAntiflickerLevel;
	IntCallBack pfnCallBack; 
    HIFB_ALPHA_S stAlpha;
    HIFB_COLORKEYEX_S stCKey;
    HIFB_CLIP_S stClip;    	   
}HIFB_BLIT_OPT_S;


/*mask bit*/
typedef enum
{
    /*Color format*/
    /*CNcomment:颜色格式*/
    HIFB_LAYER_PARAMODIFY_FMT = 0x1,

    /*Line length*/
    /*CNcomment:行间距*/
    HIFB_LAYER_PARAMODIFY_STRIDE = 0x2,

    /*Alpha value*/
    /*CNcomment:alpha值*/
    HIFB_LAYER_PARAMODIFY_ALPHA = 0x4,

    /*Colorkey value*/
    /*CNcomment:colorkey值*/
    HIFB_LAYER_PARAMODIFY_COLORKEY = 0x8,

    /*Input rectangle*/
    /*CNcomment:输入矩形*/
    HIFB_LAYER_PARAMODIFY_INRECT = 0x10,

    /*Output rectangle*/
    /*CNcomment:输出矩形*/
    HIFB_LAYER_PARAMODIFY_OUTRECT = 0x20,

    /*Display buf address*/
    /*CNcomment:显示buffer地址*/
    HIFB_LAYER_PARAMODIFY_DISPLAYADDR = 0x40,

    /*State of show or hide*/
    /*CNcomment:显示隐藏状态*/
    HIFB_LAYER_PARAMODIFY_SHOW = 0x80,

    /*Whether premultiply data or not*/
    /*CNcomment:是否为预存数据*/
    HIFB_LAYER_PARAMODIFY_BMUL = 0x100,

    /*Anti deflicker level*/
    /*CNcomment:抗闪烁级别*/
    HIFB_LAYER_PARAMODIFY_ANTIFLICKERLEVEL = 0x200,

	/*refresh usr data*/
    /*CNcomment:是否刷新*/
    HIFB_LAYER_PARAMODIFY_REFRESH = 0x400,  /**color format ,stride,display address only take effect when 
    												usr data was refreshed*/

	/*param modify all*/
	/*CNcomment:所有修改项*/
	HIFB_LAYER_PARAMODIFY_ALL = 0x1 | 0x2 | 0x4 | 0x8 | 0x10 | 0x20 | 0x40 | 0x80 | 0x100 | 0x200 | 0x400,
	
    HIFB_LAYER_PARAMODIFY_BUTT
}HIFB_LAYER_PARAMODIFY_MASKBIT_E;

typedef struct 
{
	 /* support how many layers*/
	/*CNcomment:返回支持的图层数 */
	//HI_U32 HIFB_DRV_GetSupportLayerCount();

	/*set layer the default bit extenal mode*/
	/*CNcomment:设置默认的扩展bit 模式*/
	//HI_S32 (*HIFB_DRV_SetLayerBitExtMode)(HIFB_LAYER_ID_E enLayerID);

	/*enable/disable the layer*/
	/*CNcomment:使能图层*/
	HI_S32 (*HIFB_DRV_EnableLayer)(HIFB_LAYER_ID_E enLayerId,HI_BOOL bEnable);

	/*set the address of layer*/
	/*CNcomment:设置图层的显示地址*/
	HI_S32 (*HIFB_DRV_SetLayerAddr)(HIFB_LAYER_ID_E enLayerId,HI_U32 u32Addr);

	/*set layer stride*/
	/*CNcomment:设置图层行间距*/
	HI_S32 (*HIFB_DRV_SetLayerStride)(HIFB_LAYER_ID_E enLayerId,HI_U32 u32Stride);

	/*set layer pixel format*/
	/*CNcomment:设置图层像素格式*/
	HI_S32 (*HIFB_DRV_SetLayerDataFmt)(HIFB_LAYER_ID_E enLayerId,HIFB_COLOR_FMT_E enDataFmt);

	/*set color register*/
	/*CNcomment:设置CLUT 数组值*/
	HI_S32 (*HIFB_DRV_SetColorReg)(HIFB_LAYER_ID_E enLayerId, HI_U32 u32OffSet, HI_U32 u32Color, HI_S32 UpFlag);

	/*wait until vblank, it's a block interface*/
	/*CNcomment:等待垂直时序中断，阻塞型接口*/
	HI_S32 (*HIFB_DRV_WaitVBlank)(HIFB_LAYER_ID_E enLayerId);

	/* set layer deflicker */
	/*CNcomment:设置图层抗闪参数*/
	HI_S32 (*HIFB_DRV_SetLayerDeFlicker)(HIFB_LAYER_ID_E enLayerId, HIFB_DEFLICKER_S *pstDeFlicker);

	/*set layer alpha*/
	/*CNcomment:设置图层alpha*/
	HI_S32 (*HIFB_DRV_SetLayerAlpha)(HIFB_LAYER_ID_E enLayerId, HIFB_ALPHA_S *pstAlpha);

	/*set layer start position and size*/
	/*CNcomment:设置图层起始位置和大小*/
	HI_S32 (*HIFB_DRV_SetLayerRect)(HIFB_LAYER_ID_E enLayerId, const HIFB_RECT *pstInputRect, const HIFB_RECT *pstOutputRect);
	
	/*set layer start position and size*/
	/*CNcomment:设置图层起始位置和大小*/
	HI_S32 (*HIFB_DRV_SetLayerInRect)(HIFB_LAYER_ID_E enLayerId, const HIFB_RECT *pstInputRect);

	/*set layer output size*/
	/*CNcomment:设置图层的输出区域*/
	HI_S32 (*HIFB_DRV_SetLayerOutRect)(HIFB_LAYER_ID_E enLayerId, const HIFB_RECT *pstOutputRect);


	/* set layer colorkey */
	/* CNcomment:设置图层colorkey*/
	HI_S32 (*HIFB_DRV_SetLayerKeyMask)(HIFB_LAYER_ID_E enLayerId, const HIFB_COLORKEYEX_S* pstColorkey);

	/* update layer register */
	/*CNcomment:寄存器更新*/
	HI_S32 (*HIFB_DRV_UpdataLayerReg)(HIFB_LAYER_ID_E enLayerId);

	/*wait for the config register completed*/
	/*CNcomment:等待寄存器配置完成*/
	HI_S32 (*HIFB_DRV_WaitRegUpdateFinished)(HIFB_LAYER_ID_E enLayerId);

	/*set premul data*/
	/*CNcomment:设置预乘*/
	HI_S32 (*HIFB_DRV_SetLayerPreMult)(HIFB_LAYER_ID_E enLayerId, HI_BOOL bPreMul);

	/*set clut address*/
	/*CNcomment:设置CLUT 地址*/
	HI_S32 (*HIFB_DRV_SetClutAddr)(HIFB_LAYER_ID_E enLayerId, HI_U32 u32PhyAddr);

	/* get osd data */
	/*CNcomment:获取硬件数据*/
	HI_S32 (*HIFB_DRV_GetOSDData)(HIFB_LAYER_ID_E enLayerId, HIFB_OSD_DATA_S *pstLayerData);
  
	/* register call back function*/
	/*CNcomment:注册中断回调函数*/
	HI_S32 (*HIFB_DRV_SetIntCallback)(HIFB_CALLBACK_TPYE_E enCType, IntCallBack pCallback, HIFB_LAYER_ID_E enLayerId);

	/*open layer*/
	/*CNcomment:打开图层*/
	HI_S32 (*HIFB_DRV_OpenLayer)(HIFB_LAYER_ID_E enLayerId);

	/*close layer*/
	/*CNcomment:关闭图层*/
	HI_S32 (*HIFB_DRV_CloseLayer)(HIFB_LAYER_ID_E enLayerId);

	/*get status of disp*/
	/*CNcomment:获取DISP 打开状态*/
	HI_S32 (*HIFB_DRV_GetHaltDispStatus)(HIFB_LAYER_ID_E enLayerId,HI_BOOL *pbDispInit);

	/*enable/disable compression*/
	/*CNcomment:使能/ 非使能压缩*/
	HI_S32 (*HIFB_DRV_EnableCompression)(HIFB_LAYER_ID_E enLayerId, HI_BOOL bCompressionEnable);

	/*set update rect of compression*/
	/*CNcomment:设置压缩更新矩形*/
	HI_S32 (*HIFB_DRV_SetCompressionRect)(HIFB_LAYER_ID_E enLayerId, const HIFB_RECT *pstRect);

	/*handle the compression job*/
	/*CNcomment:处理压缩更新*/
	HI_S32 (*HIFB_DRV_CompressionHandle)(HIFB_LAYER_ID_E enLayerId);

	/*set 3D mode*/
	/*CNcomment:设置3D 模式*/
	HI_S32 (*HIFB_DRV_SetTriDimMode)(HIFB_LAYER_ID_E enLayerId, HIFB_STEREO_MODE_E enStereoMode);  

	
	/*set 3D address*/
	/*CNcomment:设置3D 数据地址*/
	HI_S32 (*HIFB_DRV_SetTriDimAddr)(HIFB_LAYER_ID_E enLayerId, HI_U32 u32StereoAddr);  

	/*get capability of gfx*/
	/*CNcomment:获取图层能力集*/
	HI_S32 (*HIFB_DRV_GetGFXCap)(const HIFB_CAPABILITY_S **pstCap);

	/*pause compression*/
	/*CNcomment:暂停压缩*/
	HI_S32 (*HIFB_DRV_PauseCompression)(HIFB_LAYER_ID_E enLayerId);

	/*resume compression*/
	/*CNcomment:恢复压缩*/
	HI_S32 (*HIFB_DRV_ResumeCompression)(HIFB_LAYER_ID_E enLayerId);

	/*set the priority of layer in gp*/
	/*CNcomment:设置图层在GP 中的优先级*/
	HI_S32 (*HIFB_DRV_SetLayerPriority)(HIFB_LAYER_ID_E enLayerId, HIFB_ZORDER_E enZOrder);

	/*get the priority of layer in gp*/
	/*CNcomment:获取图层在GP 中的优先级*/
	HI_S32 (*HIFB_DRV_GetLayerPriority)(HIFB_LAYER_ID_E enLayerId, HI_U32 *pU32Priority);

	/*mask layer,  prevent user to operating the layer in  the period of display format changing*/
	/*CNcomment:屏蔽图层，阻止用户对图层的硬件设置操作*/
	HI_S32 (*HIFB_DRV_MaskLayer)(HIFB_LAYER_ID_E enLayerId, HI_BOOL bFlag);	
  
	HI_S32 (*HIFB_DRV_ColorConvert)(const struct fb_var_screeninfo *pstVar, HIFB_COLORKEYEX_S *pCkey);	

	HI_S32 (*HIFB_DRV_GfxInit)(HI_VOID);
	HI_S32 (*HIFB_DRV_GfxDeInit)(HI_VOID);

	HI_S32 (*HIFB_DRV_GetLayerOutRect)(HIFB_LAYER_ID_E enLayerId, HIFB_RECT *pstOutputRect);
	HI_S32 (*HIFB_DRV_GetLayerInRect)(HIFB_LAYER_ID_E enLayerId, HIFB_RECT *pstOutputRect);  
	/*set layer screen size*/
	/*CNcomment:设置图层的输出区域*/
	HI_S32 (*HIFB_DRV_SetLayerScreenSize)(HIFB_LAYER_ID_E enLayerId, HI_U32 u32Width, HI_U32 u32Height);
	/*set flag of screensize modified by usr*/
	/*CNcomment:设置用户修改SCREEN 的标志*/
	HI_S32 (*HIFB_DRV_SetScreenFlag)(HIFB_LAYER_ID_E enLayerId, HI_BOOL bFlag);
	/*Get flag of screensize modified by usr*/
	/*CNcomment:获取用户修改SCREEN 的标志*/
	HI_S32 (*HIFB_DRV_GetScreenFlag)(HIFB_LAYER_ID_E enLayerId);

	/*set flag of screensize modified by usr*/
	/*CNcomment:设置用户修改SCREEN 的标志*/
	HI_S32 (*HIFB_DRV_SetInitScreenFlag)(HIFB_LAYER_ID_E enLayerId, HI_BOOL bFlag);
	/*Get flag of screensize modified by usr*/
	/*CNcomment:获取用户修改SCREEN 的标志*/
	HI_S32 (*HIFB_DRV_GetInitScreenFlag)(HIFB_LAYER_ID_E enLayerId);
	/*set gfx mask flag*/
	/*CNcomment:设置图形屏蔽标志*/
	HI_S32 (*HIFB_DRV_SetLayerMaskFlag)(HIFB_LAYER_ID_E enLayerId, HI_BOOL bFlag);
	/*Get gfx mask flag*/
	/*CNcomment:获取图形屏蔽标志*/
	HI_S32 (*HIFB_DRV_GetLayerMaskFlag)(HIFB_LAYER_ID_E enLayerId);

	HI_S32 (*HIFB_DRV_GetDispSize)(HIFB_LAYER_ID_E enLayerId, HIFB_RECT *pstOutputRect);
	HI_S32 (*HIFB_DRV_ClearLogo)(HIFB_LAYER_ID_E enLayerId);
	HI_S32 (*HIFB_DRV_SetStereoDepth)(HIFB_LAYER_ID_E enLayerId, HI_S32 s32Depth);
}HIFB_DRV_OPS_S;

typedef struct
{
	HI_S32 (*HIFB_DRV_Blit)(HIFB_BUFFER_S *pSrcImg, HIFB_BUFFER_S *pDstImg,  HIFB_BLIT_OPT_S *pstOpt,HI_BOOL bScreenRefresh);
	HI_S32 (*HIFB_DRV_ClearRect)(HIFB_SURFACE_S* pDstImg, HIFB_BLIT_OPT_S* pstOpt);
	HI_S32 (*HIFB_DRV_SetTdeCallBack)(IntCallBack pTdeCallBack);
	HI_VOID (*HIFB_DRV_WaitAllTdeDone)(HI_BOOL bSync);
	HI_S32 (*HIFB_DRV_TdeSupportFmt)(HIFB_COLOR_FMT_E fmt);
	
	HI_S32 (*HIFB_DRV_CalScaleRect)(const TDE2_RECT_S* pstSrcRect, const TDE2_RECT_S* pstDstRect,
								TDE2_RECT_S* pstRectInSrc, TDE2_RECT_S* pstRectInDst);
	HI_S32 (*HIFB_DRV_WaitForDone)(TDE_HANDLE s32Handle, HI_U32 u32TimeOut);
	
	HI_S32 (*HIFB_DRV_TdeOpen)(HI_VOID);
	HI_S32 (*HIFB_DRV_TdeClose)(HI_VOID);

}HIFB_DRV_TDEOPS_S;

HI_VOID HIFB_DRV_GetTdeOps(HIFB_DRV_TDEOPS_S *Ops);
HI_VOID HIFB_DRV_GetDevOps(HIFB_DRV_OPS_S    *Ops);




#ifdef __cplusplus
#if __cplusplus
}
#endif	/* __HIFB_DRV_H__ */
#endif /* __cplusplus */

#endif	/* __OPTM_HIFB_H__ */



