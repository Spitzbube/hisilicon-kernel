/*Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.
 ******************************************************************************
 File Name     : hi_tde_ioctl.h
Version       : Initial Draft
Author        : Hisilicon multimedia software group
Created       : 2006/7/10
Last Modified :
Description   : 
Function List :
History       :
 ******************************************************************************/

#ifndef __HI_TDE_IOCTL_H__
#define __HI_TDE_IOCTL_H__

#include <linux/ioctl.h>
#include "hi_tde_type.h"
#include "tde_config.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

/* Use 't' as magic number */
#define TDE_IOC_MAGIC 't'

/** blit info */
typedef struct hiTDE_BITBLIT_CMD_S
{
    TDE_HANDLE     s32Handle;        /**< TDE handle */
    TDE2_SURFACE_S stBackGround;     /**< background surface */
    TDE2_RECT_S    stBackGroundRect; /**< background surface operating rect */
    TDE2_SURFACE_S stForeGround;     /**< foreground surface*/
    TDE2_RECT_S    stForeGroundRect; /**< foreground surface operating rect */
    TDE2_SURFACE_S stDst;            /**< target surface*/
    TDE2_RECT_S    stDstRect;        /**< target surface operating rect */
    TDE2_OPT_S     stOpt;            /**< operating option*/
    HI_U32         u32NullIndicator; /**< flag of mask , 1: valid ,0: invalid */
} TDE_BITBLIT_CMD_S;

/** color fill info */
typedef struct hiTDE_SOLIDDRAW_CMD_S
{
    TDE_HANDLE       s32Handle;        /**< TDE handle */
    TDE2_SURFACE_S   stForeGround;     /**< background surface */
    TDE2_RECT_S      stForeGroundRect; /**< background surface operating rect */
    TDE2_SURFACE_S   stDst;            /**< foreground surface */
    TDE2_RECT_S      stDstRect;        /**< foreground surface operating rect */
    TDE2_FILLCOLOR_S stFillColor;      /**< fill color */
    TDE2_OPT_S       stOpt;            /**< operating option */
    HI_U32           u32NullIndicator; /**< flag of mask , 1: valid ,0: invalid */
} TDE_SOLIDDRAW_CMD_S;

/** quick copy info */
typedef struct hiTDE_QUICKCOPY_CMD_S
{
    TDE_HANDLE     s32Handle;   /**< TDE handle */
    TDE2_SURFACE_S stSrc;       /**< src surface */
    TDE2_RECT_S    stSrcRect;   /**< src surface rect */
    TDE2_SURFACE_S stDst;       /**< target surface */
    TDE2_RECT_S    stDstRect;   /**< target rect */
} TDE_QUICKCOPY_CMD_S;

/** quick fill info */
typedef struct hiTDE_QUICKFILL_CMD_S
{
    TDE_HANDLE     s32Handle;   /**< TDE handle */
    TDE2_SURFACE_S stDst;       /**< target surface */
    TDE2_RECT_S    stDstRect;   /**< target surface rect */
    HI_U32         u32FillData; /**< fill color*/
} TDE_QUICKFILL_CMD_S;

/** quick defilicker info */
typedef struct hiTDE_QUICKDEFLICKER_CMD_S
{
    TDE_HANDLE     s32Handle;   /**< TDE handle */
    TDE2_SURFACE_S stSrc;       /**< src surface*/
    TDE2_RECT_S    stSrcRect;   /**< src rect*/
    TDE2_SURFACE_S stDst;       /**< target surface */
    TDE2_RECT_S    stDstRect;   /**< target rect */
} TDE_QUICKDEFLICKER_CMD_S;

/** quick scale info */
typedef struct hiTDE_QUICKRESIZE_CMD_S
{
    TDE_HANDLE     s32Handle; /**< TDE handle */
    TDE2_SURFACE_S stSrc;     /**< src surface*/
    TDE2_RECT_S    stSrcRect; /**< src rect*/
    TDE2_SURFACE_S stDst;     /**< target surface */
    TDE2_RECT_S    stDstRect; /**< target rect */
} TDE_QUICKRESIZE_CMD_S;

/** semi-planar YUV to RGB info */
typedef struct hiTDE_MBBITBLT_CMD_S
{
    TDE_HANDLE     s32Handle;  /**< TDE handle */
    TDE2_MB_S      stMB;       /**< src surface*/    
    TDE2_RECT_S    stMbRect;   /**< src rect*/       
    TDE2_SURFACE_S stDst;      /**< target surface */
    TDE2_RECT_S    stDstRect;  /**< target rect */   
    TDE2_MBOPT_S   stMbOpt;    /**< operating option */
} TDE_MBBITBLT_CMD_S;

/** commond info */
typedef struct hiTDE_ENDJOB_CMD_S
{
    TDE_HANDLE s32Handle;  /**< TDE handle */
    HI_BOOL    bSync;      /**< weather sync */
    HI_BOOL    bBlock;     /**< weather block */
    HI_U32     u32TimeOut; /**< time out(ms) */
} TDE_ENDJOB_CMD_S;

#if HI_TDE_BITMAPMASK_SUPPORT
/** mask blit rop blend info */
typedef struct hiTDE_BITMAP_MASKROP_CMD_S
{
    TDE_HANDLE     s32Handle;         /**< TDE handle */
    TDE2_SURFACE_S stBackGround;      /**< bk surface */
    TDE2_RECT_S    stBackGroundRect;  /**< bk rect */
    TDE2_SURFACE_S stForeGround;      /**< fore surface*/ 
    TDE2_RECT_S    stForeGroundRect;  /**< fore rect*/    
    TDE2_SURFACE_S stMask;            /**< mask surface  */
    TDE2_RECT_S    stMaskRect;        /**< mask surface rect */
    TDE2_SURFACE_S stDst;             /**< target surface */ 
    TDE2_RECT_S    stDstRect;         /**< target rect */    
    TDE2_ROP_CODE_E enRopCode_Color;  /**< RGB rop type */
    TDE2_ROP_CODE_E enRopCode_Alpha;  /**< alpha rop type */
} TDE_BITMAP_MASKROP_CMD_S;

/** mask blit alpha blend info */
typedef struct hiTDE_BITMAP_MASKBLEND_CMD_S
{
    TDE_HANDLE     s32Handle;         /**< TDE handle */
    TDE2_SURFACE_S stBackGround;      /**< bk surface */      
    TDE2_RECT_S    stBackGroundRect;  /**< bk rect */         
    TDE2_SURFACE_S stForeGround;      /**< fore surface*/      
    TDE2_RECT_S    stForeGroundRect;  /**< fore rect*/         
    TDE2_SURFACE_S stMask;            /**< mask surface  */        
    TDE2_RECT_S    stMaskRect;        /**< mask surface rect */    
    TDE2_SURFACE_S stDst;             /**< target surface */       
    TDE2_RECT_S    stDstRect;         /**< target rect */          
    HI_U8          u8Alpha;           /**< global alpha */
    HI_U8          Reserved0;
    HI_U8          Reserved1;
    HI_U8          Reserved2;    
    TDE2_ALUCMD_E enBlendMode;        /**< blend opt */
}TDE_BITMAP_MASKBLEND_CMD_S;
#endif
/** pattern fill  */
typedef struct hiTDE_PATTERN_FILL_CMD_S
{
    TDE_HANDLE s32Handle;          /**< TDE handle */
    TDE2_SURFACE_S stBackGround;   /**< bk surface */
    TDE2_RECT_S stBackGroundRect;  /**< bk rect */
    TDE2_SURFACE_S stForeGround;   /**< fore surface */
    TDE2_RECT_S stForeGroundRect;  /**< fore rect */
    TDE2_SURFACE_S stDst;          /**< target surface */
    TDE2_RECT_S stDstRect;         /**< target rect */
    TDE2_PATTERN_FILL_OPT_S stOpt; /**< option */
    HI_U32 u32NullIndicator;       /**< flag of mask , 1: valid ,0: invalid */
}TDE_PATTERN_FILL_CMD_S;

#define TDE_BEGIN_JOB _IOW(TDE_IOC_MAGIC, 1, TDE_HANDLE)
#define TDE_BIT_BLIT _IOW(TDE_IOC_MAGIC, 2, TDE_BITBLIT_CMD_S)
#define TDE_SOLID_DRAW _IOW(TDE_IOC_MAGIC, 3, TDE_SOLIDDRAW_CMD_S)
#define TDE_QUICK_COPY _IOW(TDE_IOC_MAGIC, 4, TDE_QUICKCOPY_CMD_S)
#define TDE_QUICK_RESIZE _IOW(TDE_IOC_MAGIC, 5, TDE_QUICKRESIZE_CMD_S)
#define TDE_QUICK_FILL _IOW(TDE_IOC_MAGIC, 6, TDE_QUICKFILL_CMD_S)
#define TDE_QUICK_DEFLICKER _IOW(TDE_IOC_MAGIC, 7, TDE_QUICKDEFLICKER_CMD_S)
#define TDE_MB_BITBLT _IOW(TDE_IOC_MAGIC, 8, TDE_MBBITBLT_CMD_S)
#define TDE_END_JOB _IOW(TDE_IOC_MAGIC, 9, TDE_ENDJOB_CMD_S)
#define TDE_WAITFORDONE _IOW(TDE_IOC_MAGIC, 10, TDE_HANDLE)
#define TDE_CANCEL_JOB _IOW(TDE_IOC_MAGIC, 11, TDE_HANDLE)
#if HI_TDE_BITMAPMASK_SUPPORT
#define TDE_BITMAP_MASKROP _IOW(TDE_IOC_MAGIC, 12, TDE_BITMAP_MASKROP_CMD_S)
#define TDE_BITMAP_MASKBLEND _IOW(TDE_IOC_MAGIC, 13, TDE_BITMAP_MASKBLEND_CMD_S)
#endif
#define TDE_WAITALLDONE _IO(TDE_IOC_MAGIC, 14)
#define TDE_RESET _IO(TDE_IOC_MAGIC, 15)
#define TDE_TRIGGER_SEL _IOW(TDE_IOC_MAGIC, 16, TDE_TRIGGER_E)
#define TDE_SET_DEFLICKERLEVEL _IOW(TDE_IOC_MAGIC, 17, TDE_DEFLICKER_LEVEL_E)
#define TDE_GET_DEFLICKERLEVEL _IOR(TDE_IOC_MAGIC, 18, TDE_DEFLICKER_LEVEL_E)
#define TDE_SET_ALPHATHRESHOLD_VALUE _IOW(TDE_IOC_MAGIC, 19, HI_U8)
#define TDE_GET_ALPHATHRESHOLD_VALUE _IOR(TDE_IOC_MAGIC, 20, HI_U8)
#define TDE_SET_ALPHATHRESHOLD_STATE _IOW(TDE_IOC_MAGIC, 21, HI_BOOL)
#define TDE_GET_ALPHATHRESHOLD_STATE _IOW(TDE_IOC_MAGIC, 22, HI_BOOL)
#define TDE_PATTERN_FILL _IOW(TDE_IOC_MAGIC, 23, TDE_PATTERN_FILL_CMD_S)
#define TDE_ENABLE_REGIONDEFLICKER _IOW(TDE_IOC_MAGIC, 24, HI_BOOL)

//#pragma pack()
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* End of #ifndef __HI_TDE_IOCTL_H__ */

