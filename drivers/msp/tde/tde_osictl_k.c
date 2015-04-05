/*****************************************************************************
*             Copyright 2006 - 2050, Hisilicon Tech. Co., Ltd.
*                           ALL RIGHTS RESERVED
* FileName: tde_osictl.c
* Description:  TDE osi ctl
*
* History:
* Version   Date          Author        DefectNum       Description
*
*****************************************************************************/
#include <linux/module.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/slab.h>     
#include <asm/irq.h>

#include "tde_define.h"
#include "tde_osictl.h"
#include "tde_osilist.h"
#include "tde_hal.h"
#include "wmalloc.h"
#include "tde_proc.h"
#include "tde_config.h"


#ifdef __cplusplus
 #if __cplusplus
extern "C" {
 #endif  /* __cplusplus */
#endif  /* __cplusplus */

//STATIC int s_bSrc1Conv = 0;

/****************************************************************************/
/*                             TDE osi ctl macro definition					*/
/****************************************************************************/
#define TDE_NO_SCALE_STEP 0x1000
#define TDE_FLOAT_BITLEN 12
#define TDE_MAX_SLICE_WIDTH 256
#define TDE_MAX_SLICE_NUM 20
#define TDE_MAX_RECT_WIDTH 0xfff
#define TDE_MAX_RECT_HEIGHT 0xfff
#define TDE_MAX_SURFACE_PITCH 0xffff
#define TDE_MAX_ZOOM_OUT_STEP 8

#ifdef TDE_VERSION_MPW
#define TDE_MAX_MINIFICATION_H  15
#define TDE_MAX_MINIFICATION_V  15
#else
#define TDE_MAX_MINIFICATION_H  255
#define TDE_MAX_MINIFICATION_V  255
#endif
#if 0
#define TDE_MAX_TMPBUFF_WIDTH   720
#define TDE_MAX_TMPBUFF_HEIGHT  576
#endif
#define TDE_MIN(a, b) \
    (((a) > (b)) ? (b) : (a))
        
#define TDE_CALC_MAX_SLICE_NUM(bFF, vstep, in_w) \
    ((HI_FALSE == (bFF) && TDE_NO_SCALE_STEP == (vstep)) ? (1) : ((in_w) / 118 + 5)) /*AI7D02711*/

#define TDE_FILLUP_RECT_BY_DRVSURFACE(rect, drvSur) \
    do {\
        (rect).s32Xpos   = (HI_S32)(drvSur).u32Xpos; \
        (rect).s32Ypos   = (HI_S32)(drvSur).u32Ypos; \
        (rect).u32Width  = (drvSur).u32Width; \
        (rect).u32Height = (drvSur).u32Height; \
    } while (0)

/*AI7D02579 beg*/
#define TDE_CHANGE_DIR_BY_MIRROR(pDirect, enMirror) \
    do{\
        switch (enMirror)\
        {\
        case TDE2_MIRROR_HORIZONTAL:\
            (pDirect)->enHScan = !((pDirect)->enHScan);\
            break;\
        case TDE2_MIRROR_VERTICAL:\
            (pDirect)->enVScan = !((pDirect)->enVScan);\
            break;\
        case TDE2_MIRROR_BOTH:\
            (pDirect)->enHScan = !((pDirect)->enHScan);\
            (pDirect)->enVScan = !((pDirect)->enVScan);\
            break;\
        default:\
            break;\
        }\
    } while(0)
/*AI7D02579 end*/

#define TDE_SET_CLIP(phwNode, pstOpt) \
    do {\
        if (TDE2_CLIPMODE_NONE != (pstOpt)->enClipMode)\
        {\
            TDE_DRV_CLIP_CMD_S stClipCmd; \
            stClipCmd.u16ClipStartX = (HI_U16)(pstOpt)->stClipRect.s32Xpos; \
            stClipCmd.u16ClipStartY = (HI_U16)(pstOpt)->stClipRect.s32Ypos; \
            stClipCmd.u16ClipEndX = ((pstOpt)->stClipRect.s32Xpos \
                                     + (pstOpt)->stClipRect.u32Width - 1); \
            stClipCmd.u16ClipEndY = (pstOpt)->stClipRect.s32Ypos \
                                    + (pstOpt)->stClipRect.u32Height - 1; \
            stClipCmd.bInsideClip = (TDE2_CLIPMODE_INSIDE == (pstOpt)->enClipMode) \
                                    ? HI_TRUE : HI_FALSE; \
            TdeHalNodeSetClipping(phwNode, &stClipCmd); \
        } \
    } while (0)

/*
start % 4 = 0: (h-1)/4 + 1
start % 4 = 1: (h/4) + 1
start % 4 = 2: (h+1)/4 + 1
start % 4 = 3: (h+2)/4 + 1
 */
#define TDE_ADJ_SIZE_BY_START_I(start, size) \
    (((size) + (((start) % 4) - 1)) / 4 + 1)

/*
when bottom filed, adjust height, only according by 4 integal-multiple of start 
address.so, start reserve is not used
*/
#define TDE_ADJ_B_SIZE_BY_START_I(start, size) \
    ((size >> 2) + ((size % 4) >> 1))
/*
start even number:  (w-1)/2 +1
start odd number: w/2 + 1
 */
#define TDE_ADJ_SIZE_BY_START_P(start, size) \
    (((size) + (((start) % 2) - 1)) / 2 + 1)

/*
    if ((0 == (y) % 2) && (0 == (h) % 2))\
    {\
        (retH) = (h) / 2; \
    } \
    else \
    {\
        (retH) = (h) / 2 + 1; \
    } \
*/
#define TDE_ADJ_FIELD_HEIGHT_BY_START(y, h) \
    ((h) / 2 + (((y) & 1) | ((h) & 1)))
/*
    if ((1 == (y) % 2) && (1 == (h) % 2))\
    {\
        (retH) = (h) / 2 + 1; \
    } \
    else \
    {\
        (retH) = (h) / 2; \
    } \
*/
#define TDE_ADJ_B_FIELD_HEIGHT_BY_START(y, h) \
    ((h) / 2 + (((y) & 1) & ((h) & 1)))

/* AI7D02880
    return CbCr422R fill value
      ------------
      |Y'|Cb|Y|Cr|
      ------------
       ^_____|
      copy Y to Y'
*/
#define TDE_GET_YC422R_FILLVALUE(value) \
        ((value) & 0xffffff) | ((((value) >> 8) & 0xff) << 24)

#define TDE_UNIFY_RECT(psrcRect, pdstRect)\
    do{\
        if ((psrcRect)->u32Height != (pdstRect)->u32Height)\
        {\
            (psrcRect)->u32Height = TDE_MIN((psrcRect)->u32Height, (pdstRect)->u32Height);\
            (pdstRect)->u32Height = (psrcRect)->u32Height;\
        }\
        if((psrcRect)->u32Width != (pdstRect)->u32Width)\
        {\
            (psrcRect)->u32Width = TDE_MIN((psrcRect)->u32Width, (pdstRect)->u32Width);\
            (pdstRect)->u32Width = (psrcRect)->u32Width;\
        }\
    }while(0)

#define TDE_CHECK_COLORFMT(enColorFmt) do{\
        if(TDE2_COLOR_FMT_BUTT <= enColorFmt)\
        {\
            TDE_TRACE(TDE_KERN_INFO, "color format error!\n");\
            return HI_ERR_TDE_INVALID_PARA;\
        }\
    }while(0)

#define TDE_CHECK_MBCOLORFMT(enMbColorFmt) \
    do{\
        if(TDE2_MB_COLOR_FMT_BUTT <= enMbColorFmt)\
        {\
            TDE_TRACE(TDE_KERN_INFO, "mb color format error!\n");\
            return HI_ERR_TDE_INVALID_PARA;\
        }\
    }while(0)

#define TDE_CHECK_OUTALPHAFROM(enOutAlphaFrom) \
    do{\
        if(TDE2_OUTALPHA_FROM_BUTT <= enOutAlphaFrom)\
        {\
            TDE_TRACE(TDE_KERN_INFO, "enOutAlphaFrom error!\n");\
            return HI_ERR_TDE_INVALID_PARA;\
        }\
    }while(0)

#define TDE_CHECK_ROPCODE(enRopCode) \
    do{\
        if(TDE2_ROP_BUTT <= enRopCode)\
        {\
            TDE_TRACE(TDE_KERN_INFO, "enRopCode error!\n");\
            return HI_ERR_TDE_INVALID_PARA;\
        }\
    }while(0)


#define TDE_CHECK_ALUCMD(enAluCmd) \
    do{\
            if(TDE2_ALUCMD_BUTT <= enAluCmd)\
            {\
                TDE_TRACE(TDE_KERN_INFO, "enAluCmd error!\n");\
                return HI_ERR_TDE_INVALID_PARA;\
            }\
    }while(0)

#define TDE_CHECK_MIRROR(enMirror) \
    do{\
        if(TDE2_MIRROR_BUTT <= enMirror)\
        {\
            TDE_TRACE(TDE_KERN_INFO, "enMirror error!\n");\
            return HI_ERR_TDE_INVALID_PARA;\
        }\
    }while(0)

#define TDE_CHECK_CLIPMODE(enClipMode) \
    do{\
        if(TDE2_CLIPMODE_BUTT <= enClipMode)\
        {\
            TDE_TRACE(TDE_KERN_INFO, "enClipMode error!\n");\
            return HI_ERR_TDE_INVALID_PARA;\
        }\
    }while(0)

#define TDE_CHECK_MBRESIZE(enMbResize) \
    do{\
        if(TDE2_MBRESIZE_BUTT <= enMbResize)\
        {\
            TDE_TRACE(TDE_KERN_INFO, "enMbResize error!\n");\
            return HI_ERR_TDE_INVALID_PARA;\
        }\
    }while(0)

#define TDE_CHECK_MBPICMODE(enMbPicMode) \
    do{\
        if(TDE_PIC_MODE_BUTT <= enMbPicMode)\
        {\
            TDE_TRACE(TDE_KERN_INFO, "enMbPicMode error!\n");\
            return HI_ERR_TDE_INVALID_PARA;\
        }\
    }while(0)

#define TDE_CHECK_COLORKEYMODE(enColorKeyMode) \
    do{\
        if(TDE2_COLORKEY_MODE_BUTT <= enColorKeyMode)\
        {\
            TDE_TRACE(TDE_KERN_INFO, "enColorKeyMode error!\n");\
            return HI_ERR_TDE_INVALID_PARA;\
        }\
    }while(0)

#define TDE_CHECK_FILTERMODE(enFilterMode) \
    do{\
        if(TDE2_FILTER_MODE_BUTT <= enFilterMode)\
        {\
            TDE_TRACE(TDE_KERN_INFO, "enFilterMode error:%d!\n",enFilterMode);\
            return HI_ERR_TDE_INVALID_PARA;\
        }\
    }while(0)

#define TDE_CHECK_ROTATE(enRotate) \
    do{\
        if(TDE_ROTATE_BUTT <= enRotate)\
        {\
            TDE_TRACE(TDE_KERN_INFO, "enRotate error!\n");\
            return HI_ERR_TDE_INVALID_PARA;\
        }\
    }while(0)

#define TDE_CHECK_SUBBYTE_STARTX(startx, w, outFmt) \
    do {\
        HI_S32 s32Bpp;\
        s32Bpp = TdeOsiGetbppByFmt(outFmt);\
        if (0 > s32Bpp)\
        {\
            TDE_TRACE(TDE_KERN_INFO, "Unkown color format!\n");\
            return HI_ERR_TDE_INVALID_PARA;\
        }\
        \
        if (s32Bpp < 8)\
        {\
            /* when writing, subbyte format align ask start point byte align */\
            if (((startx) * s32Bpp % 8) || ((w) * s32Bpp % 8))\
            {\
                TDE_TRACE(TDE_KERN_INFO, "Subbyte's start or width for writing is not aligned!\n");\
                return HI_ERR_TDE_INVALID_PARA;\
            }\
        }\
    }while(0)

#define TDE_CHECK_BLENDCMD(enBlendCmd) \
    do{\
            if (enBlendCmd >= TDE2_BLENDCMD_BUTT)\
            {\
                TDE_TRACE(TDE_KERN_INFO, "Unknown blend cmd!\n");\
                return HI_ERR_TDE_INVALID_PARA;\
            }\
    }while(0)
            
#define TDE_CHECK_BLENDMODE(enBlendMode) \
	do{\
		if (enBlendMode >= TDE2_BLEND_BUTT)\
		{\
			TDE_TRACE(TDE_KERN_INFO, "Unknown blend mode!\n");\
			return HI_ERR_TDE_INVALID_PARA;\
		}\
	}while(0)


#define TDE_CHECK_NOT_MB(enFmt) do\
        {\
            if (enFmt >= TDE2_COLOR_FMT_JPG_YCbCr400MBP)\
            {\
                TDE_TRACE(TDE_KERN_INFO, "This operation doesn't support Semi-plannar!\n");\
                return HI_ERR_TDE_INVALID_PARA;\
            }\
        }while(0)
        
/****************************************************************************/
/*                         TDE osi ctl struct definition					*/
/****************************************************************************/

/* pixel format transform type */
typedef enum hiTDE_COLORFMT_TRANSFORM_E
{
    TDE_COLORFMT_TRANSFORM_ARGB_ARGB = 0,
    TDE_COLORFMT_TRANSFORM_ARGB_YCbCr,
    TDE_COLORFMT_TRANSFORM_CLUT_ARGB,
    TDE_COLORFMT_TRANSFORM_CLUT_YCbCr,
    TDE_COLORFMT_TRANSFORM_CLUT_CLUT,
    TDE_COLORFMT_TRANSFORM_YCbCr_ARGB,
    TDE_COLORFMT_TRANSFORM_YCbCr_YCbCr,
    TDE_COLORFMT_TRANSFORM_An_An,
    TDE_COLORFMT_TRANSFORM_ARGB_An,
    TDE_COLORFMT_TRANSFORM_YCbCr_An,
    TDE_COLORFMT_TRANSFORM_BUTT,
} TDE_COLORFMT_TRANSFORM_E;

/* CLUT table use */
typedef enum hiTDE_CLUT_USAGE_E
{
    TDE_CLUT_COLOREXPENDING = 0,    /* color expend */
    TDE_CLUT_COLORCORRECT,          /* color correct */
    TDE_CLUT_CLUT_BYPASS,
    TDE_CLUT_USAGE_BUTT
} TDE_CLUT_USAGE_E;

/* frame/filed operate mode */
typedef enum hiTDE_PIC_MODE_E
{
    TDE_FRAME_PIC_MODE = 0,     /* frame operate mode */
    TDE_BOTTOM_FIELD_PIC_MODE,  /* bottom filed operate mode */
    TDE_TOP_FIELD_PIC_MODE,     /* top filed operate mode */
    TDE_PIC_MODE_BUTT
} TDE_PIC_MODE_E;

typedef struct hiTDE_SLICE_INFO
{
    HI_U32 u32SliceXi;
    HI_U32 u32SliceWi;
    HI_U32 s32SliceHOfst;
    HI_U32 u32SliceXo;
    HI_U32 u32SliceWo;
    TDE_SLICE_TYPE_E enSliceType;
} TDE_SLICE_INFO;

typedef struct hiTDE_FILTER_OPT
{
    HI_U32  u32HStep;
    HI_U32  u32VStep;
    HI_S32  s32HOffset;
    HI_S32  s32VOffset;
    HI_U32  u32Bppi;
    HI_U32  u32WorkBufNum;
    HI_U32  bBadLastPix;        /* while blocking, last point of each block is if effective*/
    HI_BOOL bVRing;
    HI_BOOL bHRing;
    HI_BOOL bEvenStartInX;      /* when input need up_sample, bEvenStartInX is set to HI_TRUE */
    HI_BOOL bEvenStartOutX;     /* when input need drop_sample, bEvenStartInX is set to HI_TRUE */
    HI_BOOL bCoefSym;           /* coefficient is if symmetrical, use filed ground */
    HI_BOOL b2OptCbCr;
    TDE_SCANDIRECTION_S stSrcDire;
    TDE_SCANDIRECTION_S stDstDire;
    TDE_MBSTART_ADJ_INFO_S stAdjInfo;
    TDE_DOUBLESRC_ADJ_INFO_S stDSAdjInfo;
    TDE_DRV_FILTER_MODE_E enFilterMode;
    HI_BOOL bFirstLineOut;
    HI_BOOL bLastLineOut;
} TDE_FILTER_OPT;

typedef enum hiTDE_OPERATION_CATEGORY_E
{
    TDE_OPERATION_SINGLE_SRC1 = 0,
    TDE_OPERATION_SINGLE_SRC2,
    TDE_OPERATION_DOUBLE_SRC,
    TDE_OPERATION_BUTT
} TDE_OPERATION_CATEGORY_E;
typedef enum hiTDE_PATTERN_OPERATION_CATEGORY_E
{
    TDE_PATTERN_OPERATION_SINGLE_SRC = 0,
    TDE_PATTERN_OPERATION_DOUBLE_SRC,
    TDE_PATTERN_OPERATION_BUTT
}TDE_PATTERN_OPERATION_CATEGORY_E;

/****************************************************************************/
/*                     TDE osi ctl inner variables definition   			*/
/****************************************************************************/

#if 0
STATIC TDE_DRV_COLOR_FMT_E g_enTdeCommonDrvColorFmt[TDE2_COLOR_FMT_BUTT + 1] = {
    TDE_DRV_COLOR_FMT_RGB444,   TDE_DRV_COLOR_FMT_RGB555,   TDE_DRV_COLOR_FMT_RGB565,
    TDE_DRV_COLOR_FMT_RGB888,
    TDE_DRV_COLOR_FMT_ARGB4444,
    TDE_DRV_COLOR_FMT_ARGB1555, TDE_DRV_COLOR_FMT_ARGB8565, TDE_DRV_COLOR_FMT_ARGB8888,
    TDE_DRV_COLOR_FMT_CLUT1,
    TDE_DRV_COLOR_FMT_CLUT2,
    TDE_DRV_COLOR_FMT_CLUT4,    TDE_DRV_COLOR_FMT_CLUT8,    TDE_DRV_COLOR_FMT_ACLUT44,
    TDE_DRV_COLOR_FMT_ACLUT88,  TDE_DRV_COLOR_FMT_A1,
    TDE_DRV_COLOR_FMT_A8,       TDE_DRV_COLOR_FMT_YCbCr888, TDE_DRV_COLOR_FMT_AYCbCr8888,
    TDE_DRV_COLOR_FMT_YCbCr422, TDE_DRV_COLOR_FMT_byte,
    TDE_DRV_COLOR_FMT_halfword, TDE_DRV_COLOR_FMT_BUTT
};
#endif
STATIC TDE_DRV_COLOR_FMT_E g_enTdeCommonDrvColorFmt[TDE2_COLOR_FMT_BUTT + 1] = {
    TDE_DRV_COLOR_FMT_RGB444,   TDE_DRV_COLOR_FMT_RGB444,
    TDE_DRV_COLOR_FMT_RGB555,   TDE_DRV_COLOR_FMT_RGB555,
    TDE_DRV_COLOR_FMT_RGB565,   TDE_DRV_COLOR_FMT_RGB565,
    TDE_DRV_COLOR_FMT_RGB888,   TDE_DRV_COLOR_FMT_RGB888,
    TDE_DRV_COLOR_FMT_ARGB4444, TDE_DRV_COLOR_FMT_ARGB4444, TDE_DRV_COLOR_FMT_ARGB4444, TDE_DRV_COLOR_FMT_ARGB4444,
    TDE_DRV_COLOR_FMT_ARGB1555, TDE_DRV_COLOR_FMT_ARGB1555, TDE_DRV_COLOR_FMT_ARGB1555, TDE_DRV_COLOR_FMT_ARGB1555,
    TDE_DRV_COLOR_FMT_ARGB8565, TDE_DRV_COLOR_FMT_ARGB8565, TDE_DRV_COLOR_FMT_ARGB8565, TDE_DRV_COLOR_FMT_ARGB8565,
    TDE_DRV_COLOR_FMT_ARGB8888, TDE_DRV_COLOR_FMT_ARGB8888, TDE_DRV_COLOR_FMT_ARGB8888, TDE_DRV_COLOR_FMT_ARGB8888,
    TDE_DRV_COLOR_FMT_RABG8888,
    TDE_DRV_COLOR_FMT_CLUT1,    TDE_DRV_COLOR_FMT_CLUT2,    TDE_DRV_COLOR_FMT_CLUT4,    
    TDE_DRV_COLOR_FMT_CLUT8,    TDE_DRV_COLOR_FMT_ACLUT44,  TDE_DRV_COLOR_FMT_ACLUT88,  
    TDE_DRV_COLOR_FMT_A1,       TDE_DRV_COLOR_FMT_A8,       
    TDE_DRV_COLOR_FMT_YCbCr888, TDE_DRV_COLOR_FMT_AYCbCr8888,TDE_DRV_COLOR_FMT_YCbCr422, 
    TDE_DRV_COLOR_FMT_byte,     TDE_DRV_COLOR_FMT_halfword,
    TDE_DRV_COLOR_FMT_YCbCr400MBP,
    TDE_DRV_COLOR_FMT_YCbCr422MBH, TDE_DRV_COLOR_FMT_YCbCr422MBV,
    TDE_DRV_COLOR_FMT_YCbCr420MB,  TDE_DRV_COLOR_FMT_YCbCr420MB,  TDE_DRV_COLOR_FMT_YCbCr420MB,
    TDE_DRV_COLOR_FMT_YCbCr420MB,
    TDE_DRV_COLOR_FMT_YCbCr444MB, TDE_DRV_COLOR_FMT_BUTT
};

STATIC TDE_DRV_ARGB_ORDER_E g_enTdeArgbOrder[TDE2_COLOR_FMT_BUTT + 1] = {
    TDE_DRV_ORDER_ARGB, TDE_DRV_ORDER_ABGR,
    TDE_DRV_ORDER_ARGB, TDE_DRV_ORDER_ABGR,
    TDE_DRV_ORDER_ARGB, TDE_DRV_ORDER_ABGR,
    TDE_DRV_ORDER_ARGB, TDE_DRV_ORDER_ABGR,
    TDE_DRV_ORDER_ARGB, TDE_DRV_ORDER_ABGR, TDE_DRV_ORDER_RGBA, TDE_DRV_ORDER_BGRA,
    TDE_DRV_ORDER_ARGB, TDE_DRV_ORDER_ABGR, TDE_DRV_ORDER_RGBA, TDE_DRV_ORDER_BGRA,
    TDE_DRV_ORDER_ARGB, TDE_DRV_ORDER_ABGR, TDE_DRV_ORDER_RGBA, TDE_DRV_ORDER_BGRA,
    TDE_DRV_ORDER_ARGB, TDE_DRV_ORDER_ABGR, TDE_DRV_ORDER_RGBA, TDE_DRV_ORDER_BGRA,
    TDE_DRV_ORDER_RABG,
    TDE_DRV_ORDER_BUTT, TDE_DRV_ORDER_BUTT, TDE_DRV_ORDER_BUTT,    
    TDE_DRV_ORDER_BUTT, TDE_DRV_ORDER_BUTT, TDE_DRV_ORDER_BUTT,  
    TDE_DRV_ORDER_BUTT, TDE_DRV_ORDER_BUTT,       
    TDE_DRV_ORDER_BUTT, TDE_DRV_ORDER_BUTT, TDE_DRV_ORDER_BUTT, 
    TDE_DRV_ORDER_BUTT, TDE_DRV_ORDER_BUTT,
    TDE_DRV_ORDER_BUTT,
    TDE_DRV_ORDER_BUTT, TDE_DRV_ORDER_BUTT,
    TDE_DRV_ORDER_BUTT, TDE_DRV_ORDER_BUTT, TDE_DRV_ORDER_BUTT,
    TDE_DRV_ORDER_BUTT,
    TDE_DRV_ORDER_BUTT
};

STATIC TDE_DRV_COLOR_FMT_E g_enTdeMbDrvColorFmt[TDE2_MB_COLOR_FMT_BUTT + 1] = {
    TDE_DRV_COLOR_FMT_YCbCr400MBP,
    TDE_DRV_COLOR_FMT_YCbCr422MBH, TDE_DRV_COLOR_FMT_YCbCr422MBV,
    TDE_DRV_COLOR_FMT_YCbCr420MB,  TDE_DRV_COLOR_FMT_YCbCr420MB,  TDE_DRV_COLOR_FMT_YCbCr420MB,
    TDE_DRV_COLOR_FMT_YCbCr420MB,
    TDE_DRV_COLOR_FMT_YCbCr444MB,  TDE_DRV_COLOR_FMT_BUTT
};

STATIC TDE_DRV_FILTER_MODE_E g_enTdeFilterMode[TDE2_FILTER_MODE_BUTT + 1] = {
    TDE_DRV_FILTER_COLOR, TDE_DRV_FILTER_ALPHA, TDE_DRV_FILTER_ALL, TDE_DRV_FILTER_ALL
};

/* if local deflicker flag */
STATIC HI_BOOL s_bRegionDeflicker = HI_FALSE;
/****************************************************************************/
/*                 TDE osi ctl inner interface definition                   */
/****************************************************************************/
STATIC TDE_COLORFMT_CATEGORY_E   TdeOsiGetFmtCategory(TDE2_COLOR_FMT_E enFmt);

STATIC TDE_COLORFMT_TRANSFORM_E  TdeOsiGetFmtTransType(TDE2_COLOR_FMT_E enSrc2Fmt, TDE2_COLOR_FMT_E enDstFmt);

/* AI7D02649 beg */
STATIC INLINE HI_S32             TdeOsiSetClutOpt(TDE2_SURFACE_S * pClutSur, TDE2_SURFACE_S * pOutSur, TDE_CLUT_USAGE_E* penClutUsage, 
                                                  HI_BOOL bClutReload, TDE_HWNode_S* pstHWNode);

STATIC INLINE TDE_CLUT_USAGE_E   TdeOsiGetClutUsage(TDE2_COLOR_FMT_E enSrcFmt, TDE2_COLOR_FMT_E enDstFmt);
/* AI7D02649 end */

STATIC HI_S32                    TdeOsiGetScanInfo(TDE2_SURFACE_S *pSrc, TDE2_RECT_S *pstSrcRect,
                                                   TDE2_SURFACE_S *pstDst,
                                                   TDE2_RECT_S *pstDstRect, TDE2_OPT_S* pstOpt,
                                                   TDE_SCANDIRECTION_S *pstSrcDirection,
                                                   TDE_SCANDIRECTION_S *pstDstDirection);

STATIC HI_S32                    TdeOsiGetInterRect(TDE2_RECT_S *pRect1, TDE2_RECT_S *pRect2, TDE2_RECT_S *pInterRect);

STATIC INLINE HI_S32             TdeOsiSetMbPara(TDE_HANDLE s32Handle, TDE_HWNode_S* pHWNode,
                                                 TDE2_MB_S* pstMB, TDE2_RECT_S  *pstMbRect,
                                                 TDE2_SURFACE_S* pstDst, TDE2_RECT_S  *pstDstRect,
                                                 TDE2_MBOPT_S* pMbOpt);

STATIC INLINE HI_S32             TdeOsiAdjQuality(TDE_DRV_SURFACE_S* pS1Tmp, TDE_DRV_SURFACE_S* pS2Tmp,
                                                  TDE2_MB_S* pstMB, TDE2_RECT_S* pstMbRect, 
                                                  TDE2_SURFACE_S* pstDst, TDE2_RECT_S* pstDstRect,
                                                  TDE_PIC_MODE_E enPicMode, TDE2_MBOPT_S* pMbOpt);

STATIC INLINE HI_U32             TdeOsiMbCalcHStep(HI_U32 u32Wi, HI_U32 u32Wo,
                                                   TDE2_MB_COLOR_FMT_E enInFmt, TDE2_MB_COLOR_FMT_E enOutFmt, HI_BOOL bCbCr);

STATIC INLINE HI_U32             TdeOsiMbCalcVStep(HI_U32 u32Hi, HI_U32 u32Ho,
                                                   TDE2_MB_COLOR_FMT_E enInFmt, TDE2_MB_COLOR_FMT_E enOutFmt, HI_BOOL bCbCr);

STATIC INLINE HI_S32             TdeOsiMbCalcHOffset(HI_U32 u32Xi, TDE2_MB_COLOR_FMT_E enFmt, HI_BOOL bCbCr);

STATIC INLINE HI_S32             TdeOsiMbCalcVOffset(HI_U32 u32Yi, TDE2_MB_COLOR_FMT_E enFmt,
                                                     TDE_PIC_MODE_E enPicMode, HI_BOOL bCbCr);

STATIC INLINE HI_U32             TdeOsiCalcNoLapSlice(TDE_SLICE_INFO *pSliceInfo, HI_U32 u32MaxNum, HI_U32 u32Xi, 
                                                      HI_U32 u32Xo, HI_U32 u32BmpW, /*AI7D02711*/
                                                      TDE_FILTER_OPT* pstFilterOpt);

STATIC INLINE HI_U32             TdeOsiCalcOverLapSlice(TDE_SLICE_INFO *pSliceInfo, HI_U32 u32MaxNum, HI_U32 u32BmpXi, HI_U32 u32BmpWi, 
                                                        HI_U32 u32BmpXo, HI_U32 u32BmpWo, /*AI7D02711*/
                                                        TDE_FILTER_OPT* pstFilterOpt);

STATIC HI_S32                    TdeOsiSetFilterChildNode(TDE_HANDLE s32Handle, TDE_HWNode_S* pNode,
                                                          TDE2_RECT_S* pInRect,
                                                          TDE2_RECT_S*  pOutRect,
                                                          TDE2_DEFLICKER_MODE_E enDeflickerMode,
                                                          TDE_FILTER_OPT* pstFilterOpt);

STATIC HI_S32                    TdeOsiSetFilterNode(TDE_HANDLE s32Handle, TDE_HWNode_S* pNode,
                                                     TDE2_SURFACE_S *pstForeGround, TDE2_SURFACE_S *pstDst,
                                                     TDE2_RECT_S* pInRect, TDE2_RECT_S*  pOutRect,
                                                     TDE2_DEFLICKER_MODE_E enDeflickerMode, TDE_FILTER_OPT *pstFilterOpt);

STATIC HI_S32                    TdeOsi1SourceFill(TDE_HANDLE s32Handle, TDE2_SURFACE_S *pstDst,
                                                   TDE2_RECT_S  *pstDstRect, TDE2_FILLCOLOR_S *pstFillColor,
                                                   TDE2_OPT_S *pstOpt);

STATIC HI_S32                    TdeOsi2SourceFill(TDE_HANDLE s32Handle, TDE2_SURFACE_S* pstSrc,
                                                   TDE2_RECT_S  *pstSrcRect,
                                                   TDE2_SURFACE_S *pstDst,
                                                   TDE2_RECT_S  *pstDstRect, TDE2_FILLCOLOR_S *pstFillColor,
                                                   TDE2_OPT_S *pstOpt);

STATIC INLINE HI_S32             TdeOsiCheckSurface(TDE2_SURFACE_S* pstSurface, TDE2_RECT_S  *pstRect);
STATIC INLINE HI_S32             TdeOsiCheckMbSurface(TDE2_MB_S* pstMbSurface, TDE2_RECT_S  *pstRect);
STATIC INLINE HI_S32             TdeOsiCheckYc422RPara(TDE2_SURFACE_S* pstSrcSurface, TDE2_RECT_S *pstSrcRect, 
                                                       TDE2_SURFACE_S* pstDstSurface, TDE2_RECT_S *pstDstRect, 
                                                       HI_BOOL bDeflicker, HI_BOOL bResize);

STATIC TDE_OPERATION_CATEGORY_E  TdeOsiGetOptCategory(TDE2_SURFACE_S * pstBackGround, TDE2_RECT_S *pstBackGroundRect,
                                                      TDE2_SURFACE_S * pstForeGround, TDE2_RECT_S *pstForeGroundRect,
                                                      TDE2_SURFACE_S *pstDst,
                                                      TDE2_RECT_S *pstDstRect,
                                                      TDE2_OPT_S * pstOpt);

STATIC INLINE HI_VOID            TdeOsiConvertSurface(TDE2_SURFACE_S *pstSur, TDE2_RECT_S *pstRect,
                                                      TDE_SCANDIRECTION_S *pstScanInfo,
                                                      TDE_DRV_SURFACE_S *pstDrvSur,
                                                      TDE2_RECT_S *pstOperationArea);

STATIC HI_S32                    TdeOsiConvertMbSurface(TDE2_MB_S* pstMB, TDE2_RECT_S  *pstMbRect,
                                                        TDE_DRV_SURFACE_S *pstDrvY,
                                                        TDE_DRV_SURFACE_S *pstDrvCbCr,
                                                        TDE_PIC_MODE_E enPicMode, HI_BOOL bCbCr2Opt);

STATIC INLINE HI_VOID            TdeOsiCalcMbFilterOpt(TDE_FILTER_OPT* pstFilterOpt, TDE2_MB_COLOR_FMT_E enInFmt,
                                                       TDE2_MB_COLOR_FMT_E enOutFmt,
                                                       TDE2_RECT_S* pInRect, TDE2_RECT_S*  pOutRect, HI_BOOL bCbCr,
                                                       HI_BOOL bCus, TDE_PIC_MODE_E enPicMode);

STATIC HI_S32                   TdeOsiSetClipPara(TDE2_SURFACE_S * pstBackGround, TDE2_RECT_S *pstBGRect,
                                                   TDE2_SURFACE_S * pstForeGround,
                                                   TDE2_RECT_S *pstFGRect, TDE2_SURFACE_S * pstDst,
                                                   TDE2_RECT_S *pstDstRect,
                                                   TDE2_OPT_S * pstOpt,
                                                   TDE_HWNode_S *pstHwNode);

STATIC HI_S32                    TdeOsiMbSetClipPara(TDE2_RECT_S * pstDstRect, TDE2_MBOPT_S *pstMbOpt,
                                                     TDE_HWNode_S *pstHwNode);

STATIC HI_VOID                   TdeOsiSetBaseOptParaForBlit(TDE2_OPT_S * pstOpt, TDE2_SURFACE_S* pstSrc1, TDE2_SURFACE_S* pstSrc2,
                                                             TDE_OPERATION_CATEGORY_E enOptCategory, TDE_HWNode_S *pstHwNode);

STATIC INLINE HI_VOID            TdeOsiSetResizePara(TDE_HWNode_S* pstHwNode, TDE_FILTER_OPT* pstFilterOpt,TDE_NODE_SUBM_TYPE_E enNodeType);
STATIC        HI_S32             TdeOsiAdjClipPara(TDE_HWNode_S* pHWNode);
STATIC INLINE HI_VOID            TdeOsiSetDeflickerPara(TDE_HWNode_S* pstHwNode, TDE2_DEFLICKER_MODE_E enDeflickerMode, TDE_FILTER_OPT* pstFilterOpt);

STATIC HI_S32                    TdeOsiSetNodeFinish(TDE_HANDLE s32Handle, TDE_HWNode_S* pHWNode,
                                                     HI_U32 u32WorkBufNum, TDE_NODE_SUBM_TYPE_E enSubmType);

STATIC INLINE HI_S32             TdeOsiCheckResizePara(HI_U32 u32InWidth, HI_U32 u32InHeight,
                                                       HI_U32 u32OutWidth, HI_U32 u32OutHeight);
STATIC INLINE HI_BOOL            TdeOsiWhetherContainAlpha(TDE2_COLOR_FMT_E enColorFmt);
STATIC INLINE HI_VOID            TdeOsiSetFilterOptAdjInfo(TDE_DRV_SURFACE_S *pInSur, TDE_DRV_SURFACE_S* pOutSur, 
                                                           TDE_FILTER_OPT* pstFilterOpt, TDE_CHILD_SCALE_MODE_E enScaleMode);
STATIC INLINE HI_VOID            TdeOsiSetExtAlpha(TDE2_SURFACE_S *pstBackGround, TDE2_SURFACE_S *pstForeGround,
                                                   TDE_HWNode_S *pstHwNode);
STATIC HI_S32                    TdeOsiYC422TmpOpt(TDE_HANDLE s32Handle, TDE2_SURFACE_S* pstForeGround, TDE2_RECT_S  *pstForeGroundRect,
                                                   TDE2_SURFACE_S* pstDst, TDE2_RECT_S  *pstDstRect, TDE2_OPT_S* pstOpt);

#if 0
/* DEBUG functions for Node params */
STATIC INLINE HI_VOID TdeOsiPrintNodeInfo(TDE_HWNode_S* pHWNode)
{
    TDE_TRACE(TDE_KERN_DEBUG, "\n");
    if(pHWNode->u64TDE_UPDATE & 0x1)
        TDE_TRACE(TDE_KERN_DEBUG, "01 u32TDE_INS          \t0x%x\n" ,pHWNode->u32TDE_INS);
    if((pHWNode->u64TDE_UPDATE >> 1) & 0x1)
        TDE_TRACE(TDE_KERN_DEBUG, "02 u32TDE_S1_ADDR      \t0x%x\n" , pHWNode->u32TDE_S1_ADDR);
    if((pHWNode->u64TDE_UPDATE >> 2) & 0x1)
        TDE_TRACE(TDE_KERN_DEBUG, "03 u32TDE_S1_TYPE      \t0x%x\n" , pHWNode->u32TDE_S1_TYPE);
    if((pHWNode->u64TDE_UPDATE >> 3) & 0x1)
        TDE_TRACE(TDE_KERN_DEBUG, "04 u32TDE_S1_XY        \t0x%x\n" , pHWNode->u32TDE_S1_XY);
    if((pHWNode->u64TDE_UPDATE >> 4) & 0x1)
        TDE_TRACE(TDE_KERN_DEBUG, "05 u32TDE_S1_FILL      \t0x%x\n" , pHWNode->u32TDE_S1_FILL);
    if((pHWNode->u64TDE_UPDATE >> 5) & 0x1)
        TDE_TRACE(TDE_KERN_DEBUG, "06 u32TDE_S2_ADDR      \t0x%x\n" , pHWNode->u32TDE_S2_ADDR);
    if((pHWNode->u64TDE_UPDATE >> 6) & 0x1)
        TDE_TRACE(TDE_KERN_DEBUG, "07 u32TDE_S2_TYPE      \t0x%x\n" , pHWNode->u32TDE_S2_TYPE);
    if((pHWNode->u64TDE_UPDATE >> 7) & 0x1)
        TDE_TRACE(TDE_KERN_DEBUG, "08 u32TDE_S2_XY        \t0x%x\n" , pHWNode->u32TDE_S2_XY);
    if((pHWNode->u64TDE_UPDATE >> 8) & 0x1)
        TDE_TRACE(TDE_KERN_DEBUG, "09 u32TDE_S2_SIZE      \t0x%x\n" , pHWNode->u32TDE_S2_SIZE);
    if((pHWNode->u64TDE_UPDATE >> 9) & 0x1)
        TDE_TRACE(TDE_KERN_DEBUG, "10 u32TDE_S2_FILL      \t0x%x\n" , pHWNode->u32TDE_S2_FILL);
    if((pHWNode->u64TDE_UPDATE >> 10) & 0x1)
        TDE_TRACE(TDE_KERN_DEBUG, "11 u32TDE_TAR_ADDR     \t0x%x\n" , pHWNode->u32TDE_TAR_ADDR);
    if((pHWNode->u64TDE_UPDATE >> 11) & 0x1)
        TDE_TRACE(TDE_KERN_DEBUG, "12 u32TDE_TAR_TYPE     \t0x%x\n" , pHWNode->u32TDE_TAR_TYPE);
    if((pHWNode->u64TDE_UPDATE >> 12) & 0x1)
        TDE_TRACE(TDE_KERN_DEBUG, "13 u32TDE_TAR_XY       \t0x%x\n" , pHWNode->u32TDE_TAR_XY);
    if((pHWNode->u64TDE_UPDATE >> 13) & 0x1)
        TDE_TRACE(TDE_KERN_DEBUG, "14 u32TDE_TS_SIZE      \t0x%x\n" , pHWNode->u32TDE_TS_SIZE);
    if((pHWNode->u64TDE_UPDATE >> 14) & 0x1)
        TDE_TRACE(TDE_KERN_DEBUG, "15 u32TDE_COLOR_CONV   \t0x%x\n" , pHWNode->u32TDE_COLOR_CONV);
    if((pHWNode->u64TDE_UPDATE >> 15) & 0x1)
        TDE_TRACE(TDE_KERN_DEBUG, "16 u32TDE_CLUT_ADDR    \t0x%x\n" , pHWNode->u32TDE_CLUT_ADDR);
    if((pHWNode->u64TDE_UPDATE >> 16) & 0x1)
        TDE_TRACE(TDE_KERN_DEBUG, "17 u32TDE_2D_RSZ       \t0x%x\n" , pHWNode->u32TDE_2D_RSZ);
    if((pHWNode->u64TDE_UPDATE >> 17) & 0x1)
        TDE_TRACE(TDE_KERN_DEBUG, "18 u32TDE_HF_COEF_ADDR \t0x%x\n" , pHWNode->u32TDE_HF_COEF_ADDR);
    if((pHWNode->u64TDE_UPDATE >> 18) & 0x1)
        TDE_TRACE(TDE_KERN_DEBUG, "19 u32TDE_VF_COEF_ADDR \t0x%x\n" , pHWNode->u32TDE_VF_COEF_ADDR);
    if((pHWNode->u64TDE_UPDATE >> 19) & 0x1)
#ifdef TDE_VERSION_MPW
        TDE_TRACE(TDE_KERN_DEBUG, "20 u32TDE_RSZ_STEP     \t0x%x\n" , pHWNode->u32TDE_RSZ_STEP);
#else
        TDE_TRACE(TDE_KERN_DEBUG, "20 u32TDE_RSZ_STEP     \t0x%x\n" , pHWNode->u32TDE_RSZ_HSTEP);
#endif
    if((pHWNode->u64TDE_UPDATE >> 20) & 0x1)
        TDE_TRACE(TDE_KERN_DEBUG, "21 u32TDE_RSZ_Y_OFST   \t0x%x\n" , pHWNode->u32TDE_RSZ_Y_OFST);
    if((pHWNode->u64TDE_UPDATE >> 21) & 0x1)
        TDE_TRACE(TDE_KERN_DEBUG, "22 u32TDE_RSZ_X_OFST   \t0x%x\n" , pHWNode->u32TDE_RSZ_X_OFST);
    if((pHWNode->u64TDE_UPDATE >> 22) & 0x1)
        TDE_TRACE(TDE_KERN_DEBUG, "23 u32TDE_DFE_COEF0    \t0x%x\n" , pHWNode->u32TDE_DFE_COEF0);
    if((pHWNode->u64TDE_UPDATE >> 23) & 0x1)
        TDE_TRACE(TDE_KERN_DEBUG, "24 u32TDE_DFE_COEF1    \t0x%x\n" , pHWNode->u32TDE_DFE_COEF1);
    if((pHWNode->u64TDE_UPDATE >> 24) & 0x1)
        TDE_TRACE(TDE_KERN_DEBUG, "25 u32TDE_DFE_COEF2    \t0x%x\n" , pHWNode->u32TDE_DFE_COEF2);
    if((pHWNode->u64TDE_UPDATE >> 25) & 0x1)
        TDE_TRACE(TDE_KERN_DEBUG, "26 u32TDE_DFE_COEF3    \t0x%x\n" , pHWNode->u32TDE_DFE_COEF3);
    if((pHWNode->u64TDE_UPDATE >> 26) & 0x1)
        TDE_TRACE(TDE_KERN_DEBUG, "27 u32TDE_ALU          \t0x%x\n" , pHWNode->u32TDE_ALU);
    if((pHWNode->u64TDE_UPDATE >> 27) & 0x1)
        TDE_TRACE(TDE_KERN_DEBUG, "28 u32TDE_CK_MIN       \t0x%x\n" , pHWNode->u32TDE_CK_MIN);
    if((pHWNode->u64TDE_UPDATE >> 28) & 0x1)
        TDE_TRACE(TDE_KERN_DEBUG, "29 u32TDE_CK_MAX       \t0x%x\n" , pHWNode->u32TDE_CK_MAX);
    if((pHWNode->u64TDE_UPDATE >> 29) & 0x1)
        TDE_TRACE(TDE_KERN_DEBUG, "30 u32TDE_CLIP_START   \t0x%x\n" , pHWNode->u32TDE_CLIP_START);
    if((pHWNode->u64TDE_UPDATE >> 30) & 0x1)
        TDE_TRACE(TDE_KERN_DEBUG, "31 u32TDE_CLIP_STOP    \t0x%x\n" , pHWNode->u32TDE_CLIP_STOP);
#ifdef TDE_VERSION_MPW
    TDE_TRACE(TDE_KERN_DEBUG, "32 u64TDE_UPDATE       \t0x%x\n" , (HI_U32)(pHWNode->u64TDE_UPDATE&0xffffffff));
#else
    if((pHWNode->u64TDE_UPDATE >> 31) & 0x1)
        TDE_TRACE(TDE_KERN_DEBUG, "32 u32TDE_Y1_ADDR    \t0x%x\n" , pHWNode->u32TDE_Y1_ADDR);
    if((pHWNode->u64TDE_UPDATE >> 32) & 0x1)
        TDE_TRACE(TDE_KERN_DEBUG, "33 u32TDE_Y1_PITCH    \t0x%x\n" , pHWNode->u32TDE_Y1_PITCH);
    if((pHWNode->u64TDE_UPDATE >> 33) & 0x1)
        TDE_TRACE(TDE_KERN_DEBUG, "34 u32TDE_Y2_ADDR    \t0x%x\n" , pHWNode->u32TDE_Y2_ADDR);
    if((pHWNode->u64TDE_UPDATE >> 34) & 0x1)
        TDE_TRACE(TDE_KERN_DEBUG, "35 u32TDE_Y2_PITCH    \t0x%x\n" , pHWNode->u32TDE_Y2_PITCH);
    if((pHWNode->u64TDE_UPDATE >> 35) & 0x1)
        TDE_TRACE(TDE_KERN_DEBUG, "36 u32TDE_RSZ_VSTEP    \t0x%x\n" , pHWNode->u32TDE_RSZ_VSTEP);
    if((pHWNode->u64TDE_UPDATE >> 36) & 0x1)
        TDE_TRACE(TDE_KERN_DEBUG, "37 u32TDE_ARGB_ORDER    \t0x%x\n" , pHWNode->u32TDE_ARGB_ORDER);
    if((pHWNode->u64TDE_UPDATE >> 37) & 0x1)
        TDE_TRACE(TDE_KERN_DEBUG, "38 u32TDE_CK_MASK    \t0x%x\n" , pHWNode->u32TDE_CK_MASK);
    if((pHWNode->u64TDE_UPDATE >> 38) & 0x1)
        TDE_TRACE(TDE_KERN_DEBUG, "39 u32TDE_COLORIZE    \t0x%x\n" , pHWNode->u32TDE_COLORIZE);
    if((pHWNode->u64TDE_UPDATE >> 39) & 0x1)
        TDE_TRACE(TDE_KERN_DEBUG, "40 u32TDE_ALPHA_BLEND    \t0x%x\n" , pHWNode->u32TDE_ALPHA_BLEND);
 if((pHWNode->u64TDE_UPDATE >> 40) & 0x1)
        TDE_TRACE(TDE_KERN_DEBUG, "41 u32TDE_ICSC_ADDR    \t0x%x\n" , pHWNode->u32TDE_ICSC_ADDR);
 if((pHWNode->u64TDE_UPDATE >> 41) & 0x1)
        TDE_TRACE(TDE_KERN_DEBUG, "42 u32TDE_OCSC_ADDR    \t0x%x\n" , pHWNode->u32TDE_OCSC_ADDR);
    TDE_TRACE(TDE_KERN_DEBUG, "43 u64TDE_UPDATE       \t0x%x%x\n" , 
        (HI_U32)((pHWNode->u64TDE_UPDATE >> 32)&0xffffffff), (HI_U32)(pHWNode->u64TDE_UPDATE&0xffffffff));
#endif
    TDE_TRACE(TDE_KERN_DEBUG, "\n");
}
#endif


/****************************************************************************/
/*                      TDE osi ctl inner interface realization				*/
/****************************************************************************/

/*  Y  =  (263 * R + 516 * G + 100 * B) >> 10 + 16 */
/*  Cb =  (-152 * R - 298 * G + 450 * B) >> 10  + 128 */
/*  Cr =  (450 * R - 377 * G - 73 * B) >> 10  + 128 */
STATIC HI_S32 rgb2ycc(HI_U8 r, HI_U8 g, HI_U8 b, HI_U8 *y, HI_U8 *cb, HI_U8 *cr)
{
    *y = ((((263 * r + 516 * g + 100 * b) >> 9) + 1) >> 1) + 16;
    *cb = ((((-152 * r - 298 * g+ 450 * b) >> 9) + 1) >> 1)  + 128;
    *cr = ((((450 * r - 377 * g - 73 * b) >> 9) + 1) >> 1)  + 128;

    return 0;
}

/*  R  =  ((298 * (Y-16) + 0 * (Cb-128) + 409 * (Cr-128)) >> 8 */
/*  G =  ((298 * (Y-16) - 100 * (Cb-128) - 208 * (Cr-128)) >> 8 */
/*  B = ((298 * (Y-16) + 517 * (Cb-128) - 0 * (Cr-128)) >> 8 */
STATIC HI_S32 ycc2rgb(HI_U8 y, HI_U8 cb, HI_U8 cr, HI_U8 *r, HI_U8 *g, HI_U8 *b)
{
    *r = (((298 *(y -16) + 409 * (cr - 128)) >> 7) + 1) >> 1;
    *g = (((298 * (y - 16) - 100 * (cb - 128) - 208 * (cr - 128)) >> 7) + 1) >> 1;
    *b = (((298 *(y -16) + 517 * (cb - 128)) >> 7) + 1) >> 1;

    return 0;
}

typedef struct tagTDE2_FMT_BITOFFSET_S
{
    HI_U8 u8AlphaBits;
    HI_U8 u8RedBits;
    HI_U8 u8GreenBits;
    HI_U8 u8BlueBits;
    HI_U8 u8AlphaOffset;
    HI_U8 u8RedOffset;
    HI_U8 u8GreenOffset;
    HI_U8 u8BlueOffset;
}TDE2_FMT_BITOFFSET_S;

STATIC TDE2_FMT_BITOFFSET_S s_u8FmtBitAndOffsetArray[TDE2_COLOR_FMT_AYCbCr8888 + 1] = 
{
    {0, 4, 4, 4, 12, 8, 4, 0},      /* TDE2_COLOR_FMT_RGB444 */
    {0, 4, 4, 4, 12, 0, 4, 8},      /* TDE2_COLOR_FMT_BGR444 */
    {0, 5, 5, 5, 15, 10, 5, 0},     /* TDE2_COLOR_FMT_RGB555 */
    {0, 5, 5, 5, 15, 0, 5, 10},     /* TDE2_COLOR_FMT_BGR555 */
    {0, 5, 6, 5, 16, 11, 5, 0},     /* TDE2_COLOR_FMT_RGB565 */
    {0, 5, 6, 6, 16, 0, 5, 11},     /* TDE2_COLOR_FMT_BGR565 */
    {0, 8, 8, 8, 24, 16, 8, 0},     /* TDE2_COLOR_FMT_RGB888 */
    {0, 8, 8, 8, 24, 0, 8, 16},     /* TDE2_COLOR_FMT_BGR888 */
    {4, 4, 4, 4, 12, 8, 4, 0},      /* TDE2_COLOR_FMT_ARGB4444 */
    {4, 4, 4, 4, 12, 0, 4, 8},      /* TDE2_COLOR_FMT_ABGR4444 */
    {4, 4, 4, 4, 0, 12, 8, 4},      /* TDE2_COLOR_FMT_RGBA4444 */
    {4, 4, 4, 4, 0, 4, 8, 12},      /* TDE2_COLOR_FMT_BGRA4444 */
    {1, 5, 5, 5, 15, 10, 5, 0},     /* TDE2_COLOR_FMT_ARGB1555 */
    {1, 5, 5, 5, 15, 0, 5, 10},     /* TDE2_COLOR_FMT_ABGR1555 */
    {1, 5, 5, 5, 0, 11, 6, 1},      /* TDE2_COLOR_FMT_RGBA1555 */
    {1, 5, 5, 5, 0, 1, 6, 11},      /* TDE2_COLOR_FMT_BGRA1555 */
    {8, 5, 6, 5, 16, 11, 5, 0},     /* TDE2_COLOR_FMT_ARGB8565 */
    {8, 5, 6, 5, 16, 0, 5, 11},     /* TDE2_COLOR_FMT_ABGR8565 */
    {8, 5, 6, 5, 0, 19, 13, 8},     /* TDE2_COLOR_FMT_RGBA8565 */
    {8, 5, 6, 6, 0, 8, 13, 19},     /* TDE2_COLOR_FMT_BGRA8565 */
    {8, 8, 8, 8, 24, 16, 8, 0},     /* TDE2_COLOR_FMT_ARGB8888 */
    {8, 8, 8, 8, 24, 0, 8, 16},     /* TDE2_COLOR_FMT_ABGR8888 */
    {8, 8, 8, 8, 0, 24, 16, 8},     /* TDE2_COLOR_FMT_RGBA8888 */
    {8, 8, 8, 8, 0, 8, 16, 24},     /* TDE2_COLOR_FMT_BGRA8888 */
    {8, 8, 8, 8, 16, 24, 0, 8},     /* TDE2_COLOR_FMT_RABG8888 */
    
    {0, 0, 0, 0, 0, 0, 0, 0},       
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},       
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 8, 8, 8, 24, 16, 8, 0},     /* TDE2_COLOR_FMT_YCbCr888 */
    {8, 8, 8, 8, 24, 16, 8, 0},     /* TDE2_COLOR_FMT_AYCbCr8888 */
};

/*****************************************************************************
 Prototype       : TdeOsiColorConvert
 Description     : translate other RGB  pixel format to ARGB8888's
 Input           : enColorFmt    **
                   u32InColor    **
                   pu32OutColor  **
 Output          : None
 Return Value    : 
 Global Variable   
    Read Only    : 
    Read & Write : 
  History         
  1.Date         : 2008/5/21
    Author       : wming
    Modification : Created function

*****************************************************************************/
STATIC HI_S32 TdeOsiColorConvert(TDE2_FILLCOLOR_S *pstFillColor, TDE2_SURFACE_S *pstSur, HI_U32 *pu32OutColor)
{
    HI_U8 a, r, g, b, y, cb, cr;
    //HI_U8 bits_a, bits_r, bits_g, bits_b;
    TDE_COLORFMT_TRANSFORM_E enColorTrans;

    TDE_ASSERT(NULL != pstFillColor);
    TDE_ASSERT(NULL != pstSur);
    TDE_ASSERT(NULL != pu32OutColor);
    enColorTrans = TdeOsiGetFmtTransType(pstFillColor->enColorFmt, pstSur->enColorFmt);

#if 0
    switch(pstFillColor->enColorFmt)
    {
        case TDE2_COLOR_FMT_RGB444:
        {
            bits_a = 0;
            bits_r = 4;
            bits_g = 4;
            bits_b = 4;
            pstFillColor->u32FillColor &= 0xfff;
            break;
        }
        case TDE2_COLOR_FMT_RGB555:
        {
            bits_a = 0;
            bits_r = 5;
            bits_g = 5;
            bits_b = 5;
            pstFillColor->u32FillColor &= 0x7fff;
            break;
        }
        case TDE2_COLOR_FMT_RGB565:
        {
            bits_a = 0;
            bits_r = 5;
            bits_g = 6;
            bits_b = 5;
            pstFillColor->u32FillColor &= 0xffff;
            break;
        }
        case TDE2_COLOR_FMT_RGB888:
        {
            bits_a = 0;
            bits_r = 8;
            bits_g = 8;
            bits_b = 8;
            pstFillColor->u32FillColor &= 0xffffff;
            break;
        }
        case TDE2_COLOR_FMT_ARGB4444:
        {
            bits_a = 4;
            bits_r = 4;
            bits_g = 4;
            bits_b = 4;
            pstFillColor->u32FillColor &= 0xffff;
            break;
        }
        case TDE2_COLOR_FMT_ARGB1555:
        {
            bits_a = 1;
            bits_r = 5;
            bits_g = 5;
            bits_b = 5;
            pstFillColor->u32FillColor &= 0xffff;
            break;
        }
        case TDE2_COLOR_FMT_ARGB8565:
        {
            bits_a = 8;
            bits_r = 5;
            bits_g = 6;
            bits_b = 5;
            pstFillColor->u32FillColor &= 0xffffff;
            break;
        }
        case TDE2_COLOR_FMT_ARGB8888:
        {
            bits_a = 8;
            bits_r = 8;
            bits_g = 8;
            bits_b = 8;
            pstFillColor->u32FillColor &= 0xffffffff;
            break;
        }
        case TDE2_COLOR_FMT_YCbCr888:
        {
            bits_a = 0;
            bits_r = 8;
            bits_g = 8;
            bits_b = 8;
            pstFillColor->u32FillColor &= 0xffffff;
            break;
        }
        case TDE2_COLOR_FMT_AYCbCr8888:
        {
            bits_a = 8;
            bits_r = 8;
            bits_g = 8;
            bits_b = 8;
            break;
        }
        default:
            TDE_TRACE(TDE_KERN_INFO, "Unsupported color!\n");
            return -1;
    }
#endif
    if (((TDE2_COLOR_FMT_CLUT1 <= pstFillColor->enColorFmt) 
        && (pstFillColor->enColorFmt <= TDE2_COLOR_FMT_A8))
        || (pstFillColor->enColorFmt >= TDE2_COLOR_FMT_YCbCr422))
    {
        TDE_TRACE(TDE_KERN_INFO, "Unsupported color!\n");
        return -1;
    }

    a = (pstFillColor->u32FillColor >> s_u8FmtBitAndOffsetArray[pstFillColor->enColorFmt].u8AlphaOffset)
            & (0xff >> (8 - s_u8FmtBitAndOffsetArray[pstFillColor->enColorFmt].u8AlphaBits));
    r = (pstFillColor->u32FillColor >> s_u8FmtBitAndOffsetArray[pstFillColor->enColorFmt].u8RedOffset)
            & (0xff >> (8 - s_u8FmtBitAndOffsetArray[pstFillColor->enColorFmt].u8RedBits));
    g = (pstFillColor->u32FillColor >> s_u8FmtBitAndOffsetArray[pstFillColor->enColorFmt].u8GreenOffset)
            & (0xff >> (8 - s_u8FmtBitAndOffsetArray[pstFillColor->enColorFmt].u8GreenBits));
    b = (pstFillColor->u32FillColor >> s_u8FmtBitAndOffsetArray[pstFillColor->enColorFmt].u8BlueOffset)
            & (0xff >> (8 - s_u8FmtBitAndOffsetArray[pstFillColor->enColorFmt].u8BlueBits));

    if ((TDE2_COLOR_FMT_ARGB1555 <= pstFillColor->enColorFmt)
        && (pstFillColor->enColorFmt <= TDE2_COLOR_FMT_BGRA1555))
    {
        if(a)
        {
            a = pstSur->u8Alpha1;
        }
        else
        {
            a = pstSur->u8Alpha0;
        }
    }
    else
    {
        a = a << (8 - s_u8FmtBitAndOffsetArray[pstFillColor->enColorFmt].u8AlphaBits);
    }

#if 0
    if((TDE2_COLOR_FMT_ARGB1555 != pstFillColor->enColorFmt))
    {
        //a = (pstFillColor->u32FillColor >> (bits_b + bits_g + bits_r)) << (8-bits_a) ;
        a = a << (8 - s_u8FmtBitAndOffsetArray[pstFillColor->enColorFmt].u8AlphaBits);
    }
    else
    {       
        if(a)
        {
            a = pstSur->u8Alpha1;
        }
        else
        {
            a = pstSur->u8Alpha0;
        }
    }
#endif

#ifdef TDE_VERSION_MPW
    if(255 == a )
    {
        a  = 0x80;
    }
    else
    {
        a = a >> 1;
    }
#endif
#if 0
    r = (pstFillColor->u32FillColor >> (bits_b + bits_g)) << (8-bits_r) ; 
    g = (pstFillColor->u32FillColor >> bits_b) << (8-bits_g) ;
    b = pstFillColor->u32FillColor << (8-bits_b) ;
#endif

    r = r << (8 - s_u8FmtBitAndOffsetArray[pstFillColor->enColorFmt].u8RedBits); 
    g = g << (8 - s_u8FmtBitAndOffsetArray[pstFillColor->enColorFmt].u8GreenBits);
    b = b << (8 - s_u8FmtBitAndOffsetArray[pstFillColor->enColorFmt].u8BlueBits);

    switch(enColorTrans)
    {
        case TDE_COLORFMT_TRANSFORM_ARGB_ARGB:
        case TDE_COLORFMT_TRANSFORM_YCbCr_YCbCr:
        {
            *pu32OutColor = (a << 24) + (r << 16) + (g << 8) + b;
            return 0;
        }

        case TDE_COLORFMT_TRANSFORM_ARGB_YCbCr:
        {
            rgb2ycc(r, g, b, &y, &cb, &cr);
            *pu32OutColor = (a << 24) + (y << 16) + (cb << 8) + cr;
            return 0;
        }
        
        case TDE_COLORFMT_TRANSFORM_YCbCr_ARGB:
        {
            ycc2rgb(r, g, b, &y, &cb, &cr);
            *pu32OutColor = (a << 24) + (y << 16) + (cb << 8) + cr;
            return 0;
        }
        default:
            TDE_TRACE(TDE_KERN_INFO, "Unsupported color transport!\n");
            return -1;
    }
}

/*****************************************************************************
* Function:      TdeOsiCheckResizePara
* Description:   check zoom ratio limit
* Input:         pstInRect: rect before zoom
*                pstOutRect: rect after zoom
* Output:        none
* Return:        TDE_COLORFMT_CATEGORY_E   pixel format category
* Others:        none
*****************************************************************************/
STATIC INLINE HI_S32 TdeOsiCheckResizePara(HI_U32 u32InWidth, HI_U32 u32InHeight,
                                           HI_U32 u32OutWidth, HI_U32 u32OutHeight)
{
    if (((u32OutWidth * TDE_MAX_MINIFICATION_H) < u32InWidth)
        || ((u32OutHeight * TDE_MAX_MINIFICATION_V) < u32InHeight))
    {
        TDE_TRACE(TDE_KERN_INFO, "Resize parameter error!\n");
        return -1;
    }
    else
    {
        return 0;
    }
}

/*****************************************************************************
* Function:      TdeOsiGetFmtCategory
* Description:   get pixel format category info
* Input:         enFmt: pixel format
* Output:        none
* Return:        TDE_COLORFMT_CATEGORY_E  pixel format category
* Others:        none
*****************************************************************************/
STATIC TDE_COLORFMT_CATEGORY_E TdeOsiGetFmtCategory(TDE2_COLOR_FMT_E enFmt)
{
    /* target is ARGB format */
#ifdef TDE_VERSION_MPW
    if (enFmt <= TDE2_COLOR_FMT_ARGB8888)
#else
    if (enFmt <= TDE2_COLOR_FMT_RABG8888) 
#endif
    {
        return TDE_COLORFMT_CATEGORY_ARGB;
    }
    /* target is CLUT table format */
    else if (enFmt <= TDE2_COLOR_FMT_ACLUT88)
    {
        return TDE_COLORFMT_CATEGORY_CLUT;
    }
    /* target is alpha CLUT table format */
    else if (enFmt <= TDE2_COLOR_FMT_A8)
    {
        return TDE_COLORFMT_CATEGORY_An;
    }
    /* target is YCbCr format */
    else if (enFmt <= TDE2_COLOR_FMT_YCbCr422)
    {
        return TDE_COLORFMT_CATEGORY_YCbCr;
    }
    /* byte format */
    else if (enFmt == TDE2_COLOR_FMT_byte)
    {
        return TDE_COLORFMT_CATEGORY_BYTE;
    }
    /* halfword  format */
    else if (enFmt == TDE2_COLOR_FMT_halfword)
    {
        return TDE_COLORFMT_CATEGORY_HALFWORD;
    }
#if defined (TDE_VERSION_PILOT) || defined (TDE_VERSION_FPGA)
    else if (enFmt <= TDE2_COLOR_FMT_JPG_YCbCr444MBP)
    {
        return TDE_COLORFMT_CATEGORY_YCbCr;
    }
#endif
    /* error format */
    else
    {
        return TDE_COLORFMT_CATEGORY_BUTT;
    }
}

/*****************************************************************************
* Function:      TdeOsiGetFmtTransType
* Description:   get pixel format transform type
* Input:         enSrc2Fmt: foreground pixel format
                 enDstFmt: target pixel format 
* Output:        none
* Return:        TDE_COLORFMT_TRANSFORM_E pixel format transform type
* Others:        none
*****************************************************************************/
STATIC TDE_COLORFMT_TRANSFORM_E TdeOsiGetFmtTransType(TDE2_COLOR_FMT_E enSrc2Fmt, TDE2_COLOR_FMT_E enDstFmt)
{
    TDE_COLORFMT_CATEGORY_E enSrcCategory;
    TDE_COLORFMT_CATEGORY_E enDstCategory;

    /* get foreground pixel format category */
    enSrcCategory = TdeOsiGetFmtCategory(enSrc2Fmt);

    /* get target pixel format category*/
    enDstCategory = TdeOsiGetFmtCategory(enDstFmt);

    switch (enSrcCategory)
    {
    case TDE_COLORFMT_CATEGORY_ARGB:
    {
        if (TDE_COLORFMT_CATEGORY_ARGB == enDstCategory)
        {
            return TDE_COLORFMT_TRANSFORM_ARGB_ARGB;
        }

        if (TDE_COLORFMT_CATEGORY_YCbCr == enDstCategory)
        {
            return TDE_COLORFMT_TRANSFORM_ARGB_YCbCr;
        }

        #if 0
        if (TdeOsiWhetherContainAlpha(enDstFmt))
        {
            return TDE_COLORFMT_TRANSFORM_ARGB_An;
        }
        #else
        if (TDE_COLORFMT_CATEGORY_An == enDstCategory)
        {
            return TDE_COLORFMT_TRANSFORM_ARGB_An;
        }
        #endif

        return TDE_COLORFMT_TRANSFORM_BUTT;
    }

    case TDE_COLORFMT_CATEGORY_CLUT:
    {
        if (TDE_COLORFMT_CATEGORY_ARGB == enDstCategory)
        {
            return TDE_COLORFMT_TRANSFORM_CLUT_ARGB;
        }

        if (TDE_COLORFMT_CATEGORY_YCbCr == enDstCategory)
        {
            return TDE_COLORFMT_TRANSFORM_CLUT_YCbCr;
        }

        if (TDE_COLORFMT_CATEGORY_CLUT == enDstCategory)
        {
            return TDE_COLORFMT_TRANSFORM_CLUT_CLUT;
        }

        return TDE_COLORFMT_TRANSFORM_BUTT;
    }

    case TDE_COLORFMT_CATEGORY_YCbCr:
    {
        if (TDE_COLORFMT_CATEGORY_ARGB == enDstCategory)
        {
            return TDE_COLORFMT_TRANSFORM_YCbCr_ARGB;
        }

        if (TDE_COLORFMT_CATEGORY_YCbCr == enDstCategory)
        {
            return TDE_COLORFMT_TRANSFORM_YCbCr_YCbCr;
        }

        #if 0
        if (TdeOsiWhetherContainAlpha(enDstFmt))
        {
            return TDE_COLORFMT_TRANSFORM_YCbCr_An;
        }
        #else
        if (TDE_COLORFMT_CATEGORY_An == enDstCategory)
        {
            return TDE_COLORFMT_TRANSFORM_ARGB_An;
        }
        #endif

        return TDE_COLORFMT_TRANSFORM_BUTT;
    }

    case TDE_COLORFMT_CATEGORY_An:
    {
        if (TDE_COLORFMT_CATEGORY_An == enDstCategory)
        {
            return TDE_COLORFMT_TRANSFORM_An_An;
        }

        return TDE_COLORFMT_TRANSFORM_BUTT;
    }

    default:
        return TDE_COLORFMT_TRANSFORM_BUTT;
    }
}

/*****************************************************************************
* Function:      TdeOsiIsSingleSrc2Rop
* Description:   query if ROP operate is if single source2 operate
* Input:         enRop: rop operate type
* Output:        none
* Return:        HI_TRUE: single ROP;HI_FALSE: non single ROP
* Others:        none
*****************************************************************************/
STATIC INLINE HI_BOOL TdeOsiIsSingleSrc2Rop(TDE2_ROP_CODE_E enRop)
{
    switch (enRop)
    {
    case TDE2_ROP_BLACK:
    case TDE2_ROP_NOTCOPYPEN:
    case TDE2_ROP_COPYPEN:
    case TDE2_ROP_WHITE:
        return HI_TRUE;

    default:
        return HI_FALSE;
    }
}

/*****************************************************************************
* Function:      TdeOsiGetClutUsage
* Description:   get CLUT table usage
* Input:         enSrc2Fmt foreground pixel format
                 enDstFmt  target pixel format
* Output:        none
* Return:        TDE_CLUT_USAGE_E:  clut  usage
* Others:        none
*****************************************************************************/
STATIC INLINE TDE_CLUT_USAGE_E TdeOsiGetClutUsage(TDE2_COLOR_FMT_E enSrcFmt, TDE2_COLOR_FMT_E enDstFmt)
{
    TDE_COLORFMT_TRANSFORM_E enColorTransType;

    enColorTransType = TdeOsiGetFmtTransType(enSrcFmt, enDstFmt);

    switch (enColorTransType)
    {
    case TDE_COLORFMT_TRANSFORM_CLUT_ARGB:
    case TDE_COLORFMT_TRANSFORM_CLUT_YCbCr:
        return TDE_CLUT_COLOREXPENDING;         /* color expand */

    case TDE_COLORFMT_TRANSFORM_ARGB_ARGB:
    case TDE_COLORFMT_TRANSFORM_YCbCr_YCbCr:
        return TDE_CLUT_COLORCORRECT;           /* color adjust */

    case TDE_COLORFMT_TRANSFORM_CLUT_CLUT:
        return TDE_CLUT_CLUT_BYPASS;

    default:
        return TDE_CLUT_USAGE_BUTT;
    }
}

STATIC INLINE HI_BOOL TdeOsiWhetherContainAlpha(TDE2_COLOR_FMT_E enColorFmt)
{
    switch (enColorFmt)
    {
        case TDE2_COLOR_FMT_ARGB4444:
        case TDE2_COLOR_FMT_ABGR4444:
        case TDE2_COLOR_FMT_RGBA4444:
        case TDE2_COLOR_FMT_BGRA4444:
        case TDE2_COLOR_FMT_ARGB1555:
        case TDE2_COLOR_FMT_ABGR1555:
        case TDE2_COLOR_FMT_RGBA1555:
        case TDE2_COLOR_FMT_BGRA1555:    
        case TDE2_COLOR_FMT_ARGB8565:
        case TDE2_COLOR_FMT_ABGR8565:
        case TDE2_COLOR_FMT_RGBA8565:
        case TDE2_COLOR_FMT_BGRA8565:    
        case TDE2_COLOR_FMT_ARGB8888:
        case TDE2_COLOR_FMT_ABGR8888:
        case TDE2_COLOR_FMT_RGBA8888:
        case TDE2_COLOR_FMT_BGRA8888:    
        case TDE2_COLOR_FMT_AYCbCr8888:
        case TDE2_COLOR_FMT_RABG8888:    
            return HI_TRUE;
        default:
            return HI_FALSE;
    }
}

/*****************************************************************************
* Function:      TdeOsiSetClutOpt
* Description:   set clut parameter
* Input:         pClutSur Clut bitmap info
*                penClutUsage  return clut usage type: expand/adjust
*                bClutReload   if reload clut
*                pstHWNode     hardware node information
* Output:        none
* Return:        TDE_CLUT_USAGE_E: clut usage 
* Others:        modify AI7D02649 add
*****************************************************************************/
STATIC INLINE HI_S32 TdeOsiSetClutOpt(TDE2_SURFACE_S * pClutSur, TDE2_SURFACE_S * pOutSur, TDE_CLUT_USAGE_E* penClutUsage, 
                                      HI_BOOL bClutReload, TDE_HWNode_S* pstHWNode)
{
    if (((HI_U8*)-1 != pClutSur->pu8ClutPhyAddr)&&(HI_NULL != pClutSur->pu8ClutPhyAddr))
    {
        TDE_COLORFMT_CATEGORY_E enFmtCate;
            
        enFmtCate = TdeOsiGetFmtCategory(pOutSur->enColorFmt);

        /* when user input the type of clut is not consistent with output format,rerurn error */
        if ( (!pClutSur->bYCbCrClut && TDE_COLORFMT_CATEGORY_YCbCr == enFmtCate) 
             || (pClutSur->bYCbCrClut && TDE_COLORFMT_CATEGORY_ARGB == enFmtCate) )
        {
            TDE_TRACE(TDE_KERN_INFO, "clut fmt not same\n");
            return HI_ERR_TDE_UNSUPPORTED_OPERATION;
        }
        
        /* Because of unsupport output CSC, input and background color zone is consistent.
			In trine sources operation,clut need use background, so output color zone instand of background's */
        *penClutUsage = TdeOsiGetClutUsage(pClutSur->enColorFmt, pOutSur->enColorFmt);/*AI7D02757*/
        if (TDE_CLUT_CLUT_BYPASS > *penClutUsage)
        {
            TDE_DRV_CLUT_CMD_S stClutCmd;
            if (TDE_CLUT_COLOREXPENDING == *penClutUsage)
            {
                stClutCmd.enClutMode = TDE_COLOR_EXP_CLUT_MODE;
            }
            else
            {
                stClutCmd.enClutMode = TDE_COLOR_CORRCT_CLUT_MODE;
            }

            if(((HI_U32)pClutSur->pu8ClutPhyAddr)%4)
            {
                TDE_TRACE(TDE_KERN_INFO, "pClutSur->pu8ClutPhyAddr=%p\n", pClutSur->pu8ClutPhyAddr);
                return HI_ERR_TDE_NOT_ALIGNED;
            }

            stClutCmd.pu8PhyClutAddr = pClutSur->pu8ClutPhyAddr;
            TdeHalNodeSetClutOpt(pstHWNode, &stClutCmd, bClutReload);
        }
    }
    return HI_SUCCESS;
}

/*****************************************************************************
* Function:      TdeOsiSetFilterOptAdjInfo
* Description:   Set adjust info when each block operate in MB bitmap 
* Input:         pInSur:  input brightness info
*                pOutSur: output bitmap info
*                pstFilterOpt:    filter info
*                enScaleMode:     child node zoom node
* Output:        none
* Return:        none
* Others:        none
*****************************************************************************/
STATIC INLINE HI_VOID TdeOsiSetFilterOptAdjInfo(TDE_DRV_SURFACE_S *pInSur, TDE_DRV_SURFACE_S* pOutSur, 
                                                TDE_FILTER_OPT* pstFilterOpt, TDE_CHILD_SCALE_MODE_E enScaleMode)
{
    pstFilterOpt->stAdjInfo.enScaleMode = enScaleMode;
    pstFilterOpt->stAdjInfo.u32StartInX = pInSur->u32Xpos;
    pstFilterOpt->stAdjInfo.u32StartInY = pInSur->u32Ypos;
    pstFilterOpt->stAdjInfo.u32StartOutX = pOutSur->u32Xpos;
    pstFilterOpt->stAdjInfo.u32StartOutY = pOutSur->u32Ypos;
    pstFilterOpt->stAdjInfo.enFmt = pInSur->enColorFmt;

    if ((TDE_CHILD_SCALE_MB_CONCA_L == enScaleMode)
        && (TDE_DRV_COLOR_FMT_YCbCr420MB == pInSur->enColorFmt 
            || TDE_DRV_COLOR_FMT_YCbCr422MBH == pInSur->enColorFmt))
    {
        pstFilterOpt->bEvenStartInX = HI_TRUE;
        pstFilterOpt->bBadLastPix = HI_TRUE;
    }

    if (TDE_DRV_COLOR_FMT_YCbCr422 == pOutSur->enColorFmt)
    {
        pstFilterOpt->bEvenStartOutX = HI_TRUE;
    }
}

/*****************************************************************************
* Function:      TdeOsiCheckSingleSrc2Opt
* Description:   check if valid of foreground single source operate
* Input:         enSrc2Fmt foreground pixel format
                 enDstFmt  target pixel format
                 pstOpt     operate attribute pointer
* Output:        none
* Return:        0  valid parameter;
                 -1 invalid parameter;
* Others:        none
*****************************************************************************/
STATIC HI_S32  TdeOsiCheckSingleSrc2Opt(TDE2_COLOR_FMT_E enSrc2Fmt, TDE2_COLOR_FMT_E enDstFmt,
                                        TDE2_OPT_S* pstOpt)
{
    TDE_COLORFMT_TRANSFORM_E enColorTransType;

    /* return error, if enable color key */
    if (TDE2_COLORKEY_MODE_NONE != pstOpt->enColorKeyMode)
    {
        TDE_TRACE(TDE_KERN_INFO, "It doesn't support colorkey in single source mode!\n");
        return -1;
    }

    TDE_CHECK_ALUCMD(pstOpt->enAluCmd);

    /* if operate type is ROP and it is not single operate,return error */
    if (pstOpt->enAluCmd & TDE2_ALUCMD_ROP)
    {
        TDE_CHECK_ROPCODE(pstOpt->enRopCode_Alpha);
        TDE_CHECK_ROPCODE(pstOpt->enRopCode_Color);
        if ((!TdeOsiIsSingleSrc2Rop(pstOpt->enRopCode_Alpha))
            || (!TdeOsiIsSingleSrc2Rop(pstOpt->enRopCode_Color)))
        {
            TDE_TRACE(TDE_KERN_INFO, "Only support single s2 rop!\n");
            return -1;
        }
    }
    
    /* single source can not do blend operate */
    if(pstOpt->enAluCmd & TDE2_ALUCMD_BLEND)
    {
        TDE_TRACE(TDE_KERN_INFO, "Alu mode error!\n");
        return -1;
    }

    enColorTransType = TdeOsiGetFmtTransType(enSrc2Fmt, enDstFmt);
    if(TDE_COLORFMT_TRANSFORM_BUTT == enColorTransType)
    {
        TDE_TRACE(TDE_KERN_INFO, "Unkown color transport type!\n");
        return -1;
    }

    if(TDE_COLORFMT_TRANSFORM_CLUT_CLUT == enColorTransType)
    {
        /* unsupport deflicker,zoom, Rop, mirror,colorize */
        if ((pstOpt->enDeflickerMode != TDE2_DEFLICKER_MODE_NONE)
            || (pstOpt->bResize)
            || (pstOpt->enAluCmd & TDE2_ALUCMD_ROP)
            || (pstOpt->enAluCmd & TDE2_ALUCMD_COLORIZE)
            /*|| (TDE2_MIRROR_NONE != pstOpt->enMirror)*/) /* AI7D02892 undo mirror */
        {
            TDE_TRACE(TDE_KERN_INFO, "It doesn't support deflicker or ROP or mirror!\n");
            return -1;
        }
    }
#if 0
    else if(TDE_COLORFMT_TRANSFORM_An_An == enColorTransType)
    {
        if (enSrc2Fmt != enDstFmt)
        {
            TDE_TRACE(TDE_KERN_INFO, "s2 color format and target color format are not equal!\n");
            return -1;
        }
    }
#endif

    return 0;
}

/*****************************************************************************
* Function:      TdeOsiCheckDoubleSrcOpt
* Description:   check if valid of dual source operate
* Input:         enSrc1Fmt background pixel format
                 enSrc2Fmt foreground pixel format
                 enDstFmt  target pixel format
                 pstOpt    operate attribute operate
* Output:        none
* Return:        0  valid parameter;
                 -1 invalid parameter;
* Others:        none
*****************************************************************************/
STATIC HI_S32  TdeOsiCheckDoubleSrcOpt(TDE2_COLOR_FMT_E enSrc1Fmt, TDE2_COLOR_FMT_E enSrc2Fmt,
                                       TDE2_COLOR_FMT_E enDstFmt, TDE2_OPT_S* pstOpt)
{
    TDE_COLORFMT_CATEGORY_E enSrc1Category;
    TDE_COLORFMT_CATEGORY_E enSrc2Category;
    TDE_COLORFMT_CATEGORY_E enDstCategory;

    TDE_CHECK_ALUCMD(pstOpt->enAluCmd);

    /* get background pixel format category */
    enSrc1Category = TdeOsiGetFmtCategory(enSrc1Fmt);

    /* get foreground pixel format category  */
    enSrc2Category = TdeOsiGetFmtCategory(enSrc2Fmt);

    /* get target pixel format category  */
    enDstCategory = TdeOsiGetFmtCategory(enDstFmt);

    if ((TDE_COLORFMT_CATEGORY_BYTE <= enSrc1Category)
        || (TDE_COLORFMT_CATEGORY_BYTE <= enSrc2Category)
        || (TDE_COLORFMT_CATEGORY_BYTE <= enDstCategory))
    {
        TDE_TRACE(TDE_KERN_INFO, "unkown format!\n");
        return -1;
    }

#ifdef TDE_VERSION_MPW
    if (TDE_COLORFMT_CATEGORY_ARGB == enSrc1Category)
#else
    if (TDE_COLORFMT_CATEGORY_ARGB == enSrc1Category 
        || TDE_COLORFMT_CATEGORY_YCbCr == enSrc1Category)
#endif
    {
        if (TDE_COLORFMT_CATEGORY_An == enSrc2Category)
        {
            /* target must have alpha bit */
            if (!TdeOsiWhetherContainAlpha(enDstFmt))
            {
                TDE_TRACE(TDE_KERN_INFO, "Target must have alpha component!\n");
                return -1;
            }
        }
    }
    #if 0
    else if ((TDE2_COLOR_FMT_YCbCr888 == enSrc1Fmt)
             || (TDE2_COLOR_FMT_AYCbCr8888 == enSrc1Fmt))
    {
        if (TDE_COLORFMT_CATEGORY_An == enSrc2Category)
        {
            if (!TdeOsiWhetherContainAlpha(enDstFmt))
            {
                TDE_TRACE(TDE_KERN_INFO, "Target must have alpha component!\n");
                return -1;
            }
        }
    }
    #endif
    else if (TDE_COLORFMT_CATEGORY_CLUT == enSrc1Category)
    {
        if ((TDE_COLORFMT_CATEGORY_CLUT != enSrc2Category)
            || (TDE_COLORFMT_CATEGORY_CLUT != enDstCategory))
        {
            TDE_TRACE(TDE_KERN_INFO, "Unsupported operation!\n");
            return -1;
        }

        /* unspport deflicker, zoom, ROP/Blend/Colorize, support mirror */
        if ((pstOpt->enDeflickerMode != TDE2_DEFLICKER_MODE_NONE)|| (pstOpt->bResize)
            || (TDE2_ALUCMD_NONE != pstOpt->enAluCmd)
            /*|| (TDE2_MIRROR_NONE != pstOpt->enMirror)*/) /*AI7D02892 undo mirror limit */
        {
            TDE_TRACE(TDE_KERN_INFO, "It doesn't support deflicker or ROP or mirror!\n");
            return -1;
        }
    }
    else if((TDE_COLORFMT_CATEGORY_An == enSrc1Category)
        && (TDE_COLORFMT_CATEGORY_An == enSrc2Category)
        && (TDE_COLORFMT_CATEGORY_An == enDstCategory))
    {
        /* unsupport ROP/Blend/Colorize, support mirror */
        if ((TDE2_ALUCMD_NONE != pstOpt->enAluCmd)
            /*|| (TDE2_MIRROR_NONE != pstOpt->enMirror)*/) /*AI7D02892 undo mirror limit */
        {
            TDE_TRACE(TDE_KERN_INFO, "It doesn't support Blend/ROP/Colorize!\n");
            return -1;
        }
    }

    if (pstOpt->enAluCmd & TDE2_ALUCMD_ROP)
    {
        TDE_CHECK_ROPCODE(pstOpt->enRopCode_Alpha);
        TDE_CHECK_ROPCODE(pstOpt->enRopCode_Color);
    }	

    if (pstOpt->enAluCmd & TDE2_ALUCMD_BLEND)
    {
        TDE_CHECK_BLENDCMD(pstOpt->stBlendOpt.eBlendCmd);
        if (pstOpt->stBlendOpt.eBlendCmd == TDE2_BLENDCMD_CONFIG)
        {
             TDE_CHECK_BLENDMODE(pstOpt->stBlendOpt.eSrc1BlendMode);
   	      TDE_CHECK_BLENDMODE(pstOpt->stBlendOpt.eSrc2BlendMode);	  
        }    	
    }
 
	return HI_SUCCESS;
}

/*****************************************************************************
* Function:      TdeOsiDoubleSrcGetOptCode
* Description:   get dual source operate encode 
* Input:         enSrc1Fmt background pixel format
                 enSrc2Fmt foreground pixel format
                 enDstFmt: target pixel format 
* Output:        none
* Return:        code value
* Others:        none
*****************************************************************************/
STATIC HI_U16 TdeOsiDoubleSrcGetOptCode(TDE2_COLOR_FMT_E enSrc1Fmt, TDE2_COLOR_FMT_E enSrc2Fmt,
                                        TDE2_COLOR_FMT_E enDstFmt)
{
    HI_U16 u16Code1 = 0x0;
    HI_U16 u16Code2 = 0x0;
    TDE_COLORFMT_TRANSFORM_E enColorTransType;

    enColorTransType = TdeOsiGetFmtTransType(enSrc2Fmt, enSrc1Fmt);
    switch (enColorTransType)
    {
    case TDE_COLORFMT_TRANSFORM_ARGB_ARGB:
        u16Code1 = 0x0;
        break;

    case TDE_COLORFMT_TRANSFORM_ARGB_YCbCr:
        u16Code1 = 0x5;
        break;

    case TDE_COLORFMT_TRANSFORM_CLUT_ARGB:
        u16Code1 = 0x8;
        break;
    case TDE_COLORFMT_TRANSFORM_CLUT_YCbCr:
	    u16Code1 = 0x8 | 0x10 | 0x1;
	    break;
    case TDE_COLORFMT_TRANSFORM_YCbCr_ARGB:
        u16Code1 = 0x1;
        break;

    case TDE_COLORFMT_TRANSFORM_YCbCr_YCbCr:
        u16Code1 = 0x0;
        break;

    default:
        u16Code1 = 0x8000;
        return u16Code1;
    }

    enColorTransType = TdeOsiGetFmtTransType(enSrc1Fmt, enDstFmt);
    switch (enColorTransType)
    {
    case TDE_COLORFMT_TRANSFORM_ARGB_ARGB:
        u16Code2 = 0x0;
        break;

    case TDE_COLORFMT_TRANSFORM_ARGB_YCbCr:
        u16Code2 = 0x2;
        break;

    case TDE_COLORFMT_TRANSFORM_CLUT_ARGB:
        u16Code2 = 0x8;
        break;

    case TDE_COLORFMT_TRANSFORM_CLUT_YCbCr:
        u16Code2 = 0xa;
        break;

    case TDE_COLORFMT_TRANSFORM_YCbCr_ARGB:
        u16Code2 = 0x2 | 0x4;
	 if (enSrc2Fmt >= TDE2_COLOR_FMT_CLUT1 && enSrc2Fmt <= TDE2_COLOR_FMT_ACLUT88)
        {
        	u16Code2 = 0;
        }
        break;

    case TDE_COLORFMT_TRANSFORM_YCbCr_YCbCr:
        u16Code2 = 0x0;
        if (enSrc2Fmt >= TDE2_COLOR_FMT_CLUT1 && enSrc2Fmt <= TDE2_COLOR_FMT_ACLUT88)
        {
        	u16Code2 = 0x2;
        }	
        break;

    default:
        u16Code2 = 0x8000;
        return u16Code2;
    }

    return (u16Code1 | u16Code2);
}

/*****************************************************************************
* Function:      TdeOsiSingleSrc2GetOptCode
* Description:   get dual source operate encode 
* Input:         enSrc2Fmt foreground pixel format
                 enDstFmt  target pixel format
* Output:        none
* Return:        encode value, expression is fllowinf:

|------0-----|------1-----|------2-------|------3-----|
|-----ICSC---|-----OCSC---|(in)RGB->YCbCr|-----CLUT---|

* Others:       none
*****************************************************************************/
STATIC HI_U16 TdeOsiSingleSrc2GetOptCode(TDE2_COLOR_FMT_E enSrc2Fmt, TDE2_COLOR_FMT_E enDstFmt)
{
    HI_U16 u16Code = 0;
    TDE_COLORFMT_TRANSFORM_E enColorTransType;

    enColorTransType = TdeOsiGetFmtTransType(enSrc2Fmt, enDstFmt);

    switch (enColorTransType)
    {
    case TDE_COLORFMT_TRANSFORM_ARGB_ARGB:
        u16Code = 0x0;
        break;

    case TDE_COLORFMT_TRANSFORM_ARGB_YCbCr:
        u16Code = 0x5;
        break;

    case TDE_COLORFMT_TRANSFORM_CLUT_ARGB:
        u16Code = 0x8;
        break;

    case TDE_COLORFMT_TRANSFORM_CLUT_CLUT:
        u16Code = 0x0;
        break;

    case TDE_COLORFMT_TRANSFORM_CLUT_YCbCr:
        u16Code = 0xA;
        break;

    case TDE_COLORFMT_TRANSFORM_YCbCr_ARGB:
        u16Code = 0x1;
        break;

    case TDE_COLORFMT_TRANSFORM_YCbCr_YCbCr:
        u16Code = 0x0;
        break;

    default:
        u16Code = 0x8000;
    }

    return u16Code;
}

/*****************************************************************************
* Function:      TdeOsiGetConvbyCode
* Description:   get format conversion manner by format conversion code
* Input:         u16Code  format conversion code
*                pstConv  format conversion struct
* Output:        none
* Return:        encode value
* Others:        none
*****************************************************************************/
STATIC HI_VOID TdeOsiGetConvbyCode(HI_U16 u16Code, TDE_DRV_CONV_MODE_CMD_S *pstConv)
{
    pstConv->bInConv   = u16Code & 0x1;
    pstConv->bOutConv  = (u16Code >> 1) & 0x1;
    pstConv->bInRGB2YC = ((u16Code >> 2) & 0x1);
    pstConv->bInSrc1Conv = ((u16Code >> 4) & 0x1);
    //pstConv->bInSrc1Conv = s_bSrc1Conv;

    return;
}

/*****************************************************************************
* Function:      TdeOsiGetbppByFmt
* Description:   get pixel bit of pixel format
* Input:         enFmt  target pixel format
* Output:        none
* Return:        -1 fail; other:pixel bit
* Others:        none
*****************************************************************************/
STATIC HI_S32 TdeOsiGetbppByFmt(TDE2_COLOR_FMT_E enFmt)
{
    switch (enFmt)
    {
        case TDE2_COLOR_FMT_RGB444:
        case TDE2_COLOR_FMT_BGR444:    
        case TDE2_COLOR_FMT_RGB555:
        case TDE2_COLOR_FMT_BGR555:
        case TDE2_COLOR_FMT_RGB565:
        case TDE2_COLOR_FMT_BGR565:
        case TDE2_COLOR_FMT_ARGB4444:
        case TDE2_COLOR_FMT_ABGR4444:
        case TDE2_COLOR_FMT_RGBA4444:
        case TDE2_COLOR_FMT_BGRA4444:    
        case TDE2_COLOR_FMT_ARGB1555:
        case TDE2_COLOR_FMT_ABGR1555:
        case TDE2_COLOR_FMT_RGBA1555:
        case TDE2_COLOR_FMT_BGRA1555:    
        case TDE2_COLOR_FMT_ACLUT88:
        case TDE2_COLOR_FMT_YCbCr422:     
        case TDE2_COLOR_FMT_halfword:
            return 16;

        case TDE2_COLOR_FMT_RGB888:
        case TDE2_COLOR_FMT_BGR888:
        case TDE2_COLOR_FMT_ARGB8565:
        case TDE2_COLOR_FMT_ABGR8565:
        case TDE2_COLOR_FMT_RGBA8565:
        case TDE2_COLOR_FMT_BGRA8565:    
        case TDE2_COLOR_FMT_YCbCr888:    
            return 24;
        case TDE2_COLOR_FMT_ARGB8888:           
        case TDE2_COLOR_FMT_ABGR8888:            
        case TDE2_COLOR_FMT_RGBA8888:            
        case TDE2_COLOR_FMT_BGRA8888:        
        case TDE2_COLOR_FMT_AYCbCr8888:
        case TDE2_COLOR_FMT_RABG8888:        
            return 32;

        case TDE2_COLOR_FMT_CLUT1:
        case TDE2_COLOR_FMT_A1:
            return 1;

        case TDE2_COLOR_FMT_CLUT2:
            return 2;

        case TDE2_COLOR_FMT_CLUT4:
            return 4;

        case TDE2_COLOR_FMT_CLUT8:
        case TDE2_COLOR_FMT_ACLUT44:
        case TDE2_COLOR_FMT_A8:
        case TDE2_COLOR_FMT_byte:
            return 8;

        default:
            return -1;
    }
}

/*****************************************************************************
* Function:      TdeOsiGetScanInfo
* Description:   get scanning direction, avoid lap
* Input:         pSrc source bitmap
                 pstDst target bitmap
                 enMirror mirror type
* Output:        pstSrcDirection source scanning information
                 pstDstDirection target scanning information
* Return:        0  success
                 -1 fail
* Others:        AI7D02579 add  antiscan handle to YCbCr422R
*****************************************************************************/
STATIC HI_S32 TdeOsiGetScanInfo(TDE2_SURFACE_S *pSrc, TDE2_RECT_S *pstSrcRect, TDE2_SURFACE_S *pstDst,
                                TDE2_RECT_S *pstDstRect, TDE2_OPT_S* pstOpt,
                                TDE_SCANDIRECTION_S *pstSrcDirection,
                                TDE_SCANDIRECTION_S *pstDstDirection)
{
    HI_U32 u32SrcAddr;
    HI_U32 u32DstAddr;
    //HI_U32 u32AddrDiff;
    HI_BOOL bFilter = HI_FALSE;
    TDE2_MIRROR_E enMirror = TDE2_MIRROR_NONE;
    HI_S32 s32SrcdBpp, s32DstBpp;

    TDE_ASSERT(NULL != pSrc);
    TDE_ASSERT(NULL != pstSrcRect);
    TDE_ASSERT(NULL != pstDst);
    TDE_ASSERT(NULL != pstDstRect);
    TDE_ASSERT(NULL != pstSrcDirection);
    TDE_ASSERT(NULL != pstDstDirection);

    /* when writing, subbyte format align ask for byte align of start point */
    TDE_CHECK_SUBBYTE_STARTX(pstDstRect->s32Xpos, pstDstRect->u32Width, pstDst->enColorFmt);

    /* default scanning direction */
    pstSrcDirection->enHScan = TDE_SCAN_LEFT_RIGHT;
    pstSrcDirection->enVScan = TDE_SCAN_UP_DOWN;
    pstDstDirection->enHScan = TDE_SCAN_LEFT_RIGHT;
    pstDstDirection->enVScan = TDE_SCAN_UP_DOWN;

    if (HI_NULL != pstOpt)
    {
        if ((pstOpt->enDeflickerMode != TDE2_DEFLICKER_MODE_NONE) || pstOpt->bResize)
        {
            bFilter = HI_TRUE;
        }
        #if 0
        if ((pstOpt->enDeflickerMode != TDE2_DEFLICKER_MODE_NONE)
            || (pstOpt->bResize && pstSrcRect->u32Height != pstDstRect->u32Height))
        {
            if (TDE2_COLOR_FMT_YCbCr422 == pstDst->enColorFmt)
            {
                pstSrcDirection->enHScan = TDE_SCAN_RIGHT_LEFT;
                pstDstDirection->enHScan = TDE_SCAN_RIGHT_LEFT;
            }
        }
        #endif
        enMirror = pstOpt->enMirror;
    }

    if (enMirror && bFilter)
    {
        TDE_TRACE(TDE_KERN_INFO, "Could not support VF/FF/HF and Mirror\n!");
        return -1;
    }
#if 0
    if (TDE2_COLOR_FMT_YCbCr422 == pstDst->enColorFmt)
    {
        pstSrcDirection->enHScan = TDE_SCAN_RIGHT_LEFT;
        pstDstDirection->enHScan = TDE_SCAN_RIGHT_LEFT;
    }
    else
    {
        pstSrcDirection->enHScan = TDE_SCAN_LEFT_RIGHT;
        pstDstDirection->enHScan = TDE_SCAN_LEFT_RIGHT;
    }
#endif

    if (bFilter) /* if use filter, use default scanning direction */
    {
        return 0;
    }

    if (TDE2_MIRROR_NONE != enMirror)
    {
        TDE_CHANGE_DIR_BY_MIRROR(pstSrcDirection, enMirror);
    }
    else
    {
        /* only if stride is the same, can be do conversion */
        if (pSrc->u32Stride != pstDst->u32Stride)
        {
            return 0;
        }

        s32SrcdBpp = TdeOsiGetbppByFmt(pSrc->enColorFmt);
        s32DstBpp = TdeOsiGetbppByFmt(pstDst->enColorFmt);

        if ((s32DstBpp < 0) || (s32SrcdBpp < 0))
        {
            TDE_TRACE(TDE_KERN_INFO, "bits per pixel less than 0!\n");
            return -1;
        }

        u32SrcAddr = pSrc->u32PhyAddr + pstSrcRect->s32Ypos * pSrc->u32Stride
                     + ((pstSrcRect->s32Xpos * s32SrcdBpp) >> 3);
        u32DstAddr = pstDst->u32PhyAddr + pstDstRect->s32Ypos * pstDst->u32Stride \
                     + ((pstDstRect->s32Xpos * s32DstBpp) >> 3);

        /* source is above of target or on the left of the same direction */
        if (u32SrcAddr <= u32DstAddr)
        {
            #if 0
            u32AddrDiff = u32DstAddr - u32SrcAddr;

            if (u32AddrDiff > pSrc->u32Stride)
            {
                pstSrcDirection->enVScan = TDE_SCAN_DOWN_UP;
                pstDstDirection->enVScan = TDE_SCAN_DOWN_UP;
            }
            else
            {
                pstSrcDirection->enHScan = TDE_SCAN_RIGHT_LEFT;
                pstDstDirection->enHScan = TDE_SCAN_RIGHT_LEFT;
            }
            #endif
            pstSrcDirection->enVScan = TDE_SCAN_DOWN_UP;
            pstDstDirection->enVScan = TDE_SCAN_DOWN_UP;

            pstSrcDirection->enHScan = TDE_SCAN_RIGHT_LEFT;
            pstDstDirection->enHScan = TDE_SCAN_RIGHT_LEFT;
        }
    }

    return 0;
}

/*****************************************************************************
* Function:      TdeOsiGetInterRect
* Description:   get inter rect of two rectangles
* Input:         pRect1  input rectangle1
                 pRect2  input rectangle2
* Output:        pInterRect output inter rectangle
* Return:        0  have inter zone
                 -1 no inter zone
* Others:        none
*****************************************************************************/
STATIC HI_S32 TdeOsiGetInterRect(TDE2_RECT_S *pRect1, TDE2_RECT_S *pRect2, TDE2_RECT_S *pInterRect)
{
    HI_S32 s32Left, s32Top, s32Right, s32Bottom;
    HI_S32 s32Right1, s32Bottom1, s32Right2, s32Bottom2;

    s32Left = (pRect1->s32Xpos > pRect2->s32Xpos) ? pRect1->s32Xpos : pRect2->s32Xpos;
    s32Top = (pRect1->s32Ypos > pRect2->s32Ypos) ? pRect1->s32Ypos : pRect2->s32Ypos;

    s32Right1 = pRect1->s32Xpos + pRect1->u32Width - 1;  /* AI7D02635 */
    s32Right2 = pRect2->s32Xpos + pRect2->u32Width - 1;  /* AI7D02635 */
    s32Right = (s32Right1 > s32Right2) ? s32Right2 : s32Right1;

    s32Bottom1 = pRect1->s32Ypos + pRect1->u32Height - 1; /* AI7D02635 */
    s32Bottom2 = pRect2->s32Ypos + pRect2->u32Height - 1; /* AI7D02635 */
    s32Bottom = (s32Bottom1 > s32Bottom2) ? s32Bottom2 : s32Bottom1;

    if ((s32Left > s32Right) || (s32Top > s32Bottom))
    {
        return -1;
    }

    pInterRect->s32Xpos   = s32Left;
    pInterRect->s32Ypos   = s32Top;
    pInterRect->u32Width  = s32Right - s32Left + 1;
    pInterRect->u32Height = s32Bottom - s32Top + 1;

    return 0;
}

/*****************************************************************************
* Function:      TdeOsiIsRect1InRect2
* Description:   Rect1 is if inside Rect2
* Input:         pRect1  input rectangle1
                 pRect2  input rectangle2
* Output:        none
* Return:        1  have inter zone
                 0  no inter zone
* Others:        none
*****************************************************************************/
STATIC INLINE HI_BOOL TdeOsiIsRect1InRect2(TDE2_RECT_S *pRect1, TDE2_RECT_S *pRect2)
{
    HI_S32 s32Right1, s32Bottom1, s32Right2, s32Bottom2;

    s32Right1 = pRect1->s32Xpos + pRect1->u32Width - 1; /* AI7D02635 */
    s32Right2 = pRect2->s32Xpos + pRect2->u32Width - 1; /* AI7D02635 */

    s32Bottom1 = pRect1->s32Ypos + pRect1->u32Height - 1; /* AI7D02635 */
    s32Bottom2 = pRect2->s32Ypos + pRect2->u32Height - 1; /* AI7D02635 */

    if((pRect1->s32Xpos >= pRect2->s32Xpos)
        &&(pRect1->s32Ypos >= pRect2->s32Ypos)
        &&(s32Right1 <= s32Right2)
        &&(s32Bottom1 <= s32Bottom2))
    {
        return HI_TRUE;
    }
    
    return HI_FALSE;
}

/*****************************************************************************
* Function:      TdeOsiSetMbPara
* Description:   MB operate setting parameter interface
* Input:         s32Handle: task handle
*                pHWNode: config node
*                pstMB:    brightness block information struct
*                pstMbRect: MB bitmap operate zone
*                pstDst:  target bitmap information struct
*                pstDstRect: target bitmap operate zone
*                pMbOpt:  operate parameter setting struct
*                enPicMode: picture top filed/ bottom filed/frame
* Output:        none
* Return:        none
* Others:        none
*****************************************************************************/
STATIC INLINE HI_S32 TdeOsiSetMbPara(TDE_HANDLE s32Handle, TDE_HWNode_S* pHWNode,
                                     TDE2_MB_S* pstMB, TDE2_RECT_S  *pstMbRect,
                                     TDE2_SURFACE_S* pstDst, TDE2_RECT_S  *pstDstRect,
                                     TDE2_MBOPT_S* pMbOpt)
{
    TDE_DRV_SURFACE_S stDrvS1;
    TDE_DRV_SURFACE_S stDrvS2;
    TDE_DRV_SURFACE_S stDrvS1Tmp = {0};
    TDE_DRV_SURFACE_S stDrvS2Tmp = {0};
    TDE_DRV_CONV_MODE_CMD_S stConv = {0};
    TDE_DRV_SURFACE_S stDrvDst;
    TDE2_RECT_S stInRect;
    TDE2_RECT_S stOutRect;
    HI_U16 u16Code;

    TDE2_RECT_S stAdjFieldRect;
    TDE_DRV_MB_CMD_S stMbCmd;
    HI_S32 s32Ret;
    TDE_SCANDIRECTION_S stScan = {0};    /* MB mode always use up->down,left->right */
    TDE_FILTER_OPT stFilterOpt = {0};
    TDE_PIC_MODE_E enPicMode = TDE_FRAME_PIC_MODE;
    HI_U32 u32WorkBufNum = 0;
    TDE2_OUTALPHA_FROM_E enAlphaFrom = TDE2_OUTALPHA_FROM_NORM;
    TDE2_CSC_OPT_S stCscOpt = {0};

    /* AI7D02555 */
    if (pMbOpt->bSetOutAlpha)
    {
        enAlphaFrom = TDE2_OUTALPHA_FROM_GLOBALALPHA;
    }

    /*judge if needing calculate by filed */
    if ((TDE2_MB_COLOR_FMT_MP1_YCbCr420MBP == pstMB->enMbFmt)
        || (TDE2_MB_COLOR_FMT_MP2_YCbCr420MBP == pstMB->enMbFmt)
        || (TDE2_MB_COLOR_FMT_MP2_YCbCr420MBI == pstMB->enMbFmt))
    {
        enPicMode = TDE_TOP_FIELD_PIC_MODE;
    }
#if 1
    if((pstDstRect->u32Height == pstMbRect->u32Height)
        &&(pstDstRect->u32Width == pstMbRect->u32Width))
    {
        pMbOpt->enResize = TDE2_MBRESIZE_NONE;
    }
    else if(TDE2_MBRESIZE_NONE == pMbOpt->enResize)
    {
        TDE_UNIFY_RECT(pstMbRect, pstDstRect);
    }
#endif

    /* set target bitmap format which driver layer need */
    TdeOsiConvertSurface(pstDst, pstDstRect, &stScan, &stDrvDst, NULL);

    /* get encode by call TdeOsiSingleSrc2GetOptCode */
    u16Code = TdeOsiDoubleSrcGetOptCode(TDE2_COLOR_FMT_YCbCr888, TDE2_COLOR_FMT_YCbCr888,
                                        pstDst->enColorFmt);

    TdeOsiGetConvbyCode(u16Code, &stConv);
    /* if format is 400MB, operate on brightness directly */
    if (TDE2_MB_COLOR_FMT_JPG_YCbCr400MBP == pstMB->enMbFmt)
    {
        /* set brightness bitmap format which driver layer need */
        if (TdeOsiConvertMbSurface(pstMB, pstMbRect, &stDrvS1, HI_NULL, TDE_FRAME_PIC_MODE, HI_FALSE) < 0)
        {
            TDE_TRACE(TDE_KERN_INFO, "TdeOsiConvertMbSurface error!\n");\
            return HI_ERR_TDE_INVALID_PARA;
        }

        /* 0nly operate on brightness */
        TdeHalNodeInitNd(pHWNode, HI_FALSE);

        /* foreground's size need set to brightness's size */
        TdeHalNodeSetSrcMbY(pHWNode, &stDrvS1, TDE_MB_Y_FILTER);
        TdeHalNodeSetSrcMbCbCr(pHWNode, &stDrvS1, TDE_MB_Y_FILTER);
        TdeHalNodeSetTgt(pHWNode, &stDrvDst, enAlphaFrom);
        TdeHalNodeSetBaseOperate(pHWNode, TDE_MB_Y_OPT, TDE_ALU_NONE, 0);
        stMbCmd.enMbMode = TDE_MB_CONCA_FILTER;
        TdeHalNodeSetMbMode(pHWNode, &stMbCmd);
        TdeHalNodeSetGlobalAlpha(pHWNode, pMbOpt->u8OutAlpha, HI_TRUE);

        if ((s32Ret = TdeOsiMbSetClipPara(pstDstRect, pMbOpt, pHWNode)) < 0)
        {
            return s32Ret;
        }

        TdeHalNodeSetColorConvert(pHWNode, &stConv);
        TDeHalNodeSetCsc(pHWNode, stCscOpt);
        /* if no filter operate,complete node setting directly */
        if ((TDE2_MBRESIZE_NONE == pMbOpt->enResize) && (HI_FALSE == pMbOpt->bDeflicker))
        {
            return TdeOsiSetNodeFinish(s32Handle, pHWNode, 0, TDE_NODE_SUBM_ALONE);
        }
        else
        {
            TDE2_DEFLICKER_MODE_E enDeflickerMode = ((pMbOpt->bDeflicker)?(TDE2_DEFLICKER_MODE_RGB):(TDE2_DEFLICKER_MODE_NONE));
            TDE_FILLUP_RECT_BY_DRVSURFACE(stInRect, stDrvS1);
            TdeOsiCalcMbFilterOpt(&stFilterOpt, pstMB->enMbFmt, TDE2_MB_COLOR_FMT_JPG_YCbCr444MBP, 
                                  pstMbRect, pstDstRect, HI_FALSE, HI_FALSE, TDE_FRAME_PIC_MODE);
            stFilterOpt.stAdjInfo.enScaleMode = TDE_CHILD_SCALE_MBY;
            return TdeOsiSetFilterChildNode(s32Handle, pHWNode, &stInRect, pstDstRect, enDeflickerMode,
                                            &stFilterOpt);
        }
    }

    if (TdeOsiConvertMbSurface(pstMB, pstMbRect, &stDrvS1, &stDrvS2, enPicMode, HI_FALSE) < 0)
    {
        TDE_TRACE(TDE_KERN_INFO, "could not transfer mb paras!\n");
        return HI_ERR_TDE_INVALID_PARA;
    }

    /* adjust width and  height, by if field zoom */
    stAdjFieldRect.s32Xpos   = pstMbRect->s32Xpos;
    stAdjFieldRect.s32Ypos   = pstMbRect->s32Ypos;
    stAdjFieldRect.u32Width  = stDrvS1.u32Width;
    stAdjFieldRect.u32Height = stDrvS1.u32Height;

    /* adjust zoom quality by color format and if getting temporay buffer */
    s32Ret = TdeOsiAdjQuality(&stDrvS1Tmp, &stDrvS2Tmp, pstMB, pstMbRect, 
                              pstDst, pstDstRect, enPicMode, pMbOpt);
    if (-1 == s32Ret)
    {
        return HI_ERR_TDE_UNSUPPORTED_OPERATION;
    }
    u32WorkBufNum = (HI_U32)s32Ret;
    TDE_TRACE(TDE_KERN_DEBUG, "u32WorkBufNum:%d\n", u32WorkBufNum);

    switch (pMbOpt->enResize)
    {
        case TDE2_MBRESIZE_QUALITY_HIGH:
        {
            TDE2_DEFLICKER_MODE_E enDeflickerMode = ((pMbOpt->bDeflicker)?(TDE2_DEFLICKER_MODE_RGB):(TDE2_DEFLICKER_MODE_NONE));
            TDE_TRACE(TDE_KERN_DEBUG, "High Quality Mb Resize!\n");
            /* pass1. brightness zoom at first(deflicker) */
            TdeHalNodeInitNd(pHWNode, HI_FALSE);
            TdeHalNodeSetSrcMbY(pHWNode, &stDrvS1, TDE_MB_Y_FILTER);
            TdeHalNodeSetTgt(pHWNode, &stDrvS1Tmp, enAlphaFrom);
            TdeHalNodeSetBaseOperate(pHWNode, TDE_MB_Y_OPT, TDE_ALU_NONE, 0);
            stMbCmd.enMbMode = TDE_MB_Y_FILTER;
            TdeHalNodeSetMbMode(pHWNode, &stMbCmd);
            TdeOsiCalcMbFilterOpt(&stFilterOpt, pstMB->enMbFmt, TDE2_MB_COLOR_FMT_JPG_YCbCr444MBP, 
                                  &stAdjFieldRect, pstDstRect, HI_FALSE, HI_FALSE, enPicMode);
            TDE_FILLUP_RECT_BY_DRVSURFACE(stInRect, stDrvS1);
            TDE_FILLUP_RECT_BY_DRVSURFACE(stOutRect, stDrvS1Tmp);
            TdeOsiSetFilterOptAdjInfo(&stDrvS1, &stDrvS1Tmp, &stFilterOpt, TDE_CHILD_SCALE_MBY);
            if ((s32Ret = TdeOsiSetFilterChildNode(s32Handle, pHWNode, &stInRect, &stOutRect, enDeflickerMode,
                                              &stFilterOpt)) < 0)
            {
                tde_osr_disableirq();
                TdeOsiListPutPhyBuff(u32WorkBufNum);
                tde_osr_enableirq();
                TDE_TRACE(TDE_KERN_INFO, "mb high quality pass1 fail\n!");
                return s32Ret;
            }

            /* pass2. chroma zoom at then */
            TdeHalNodeInitNd(pHWNode, HI_FALSE);

            /* foreground size restore to chroma size */
            TdeHalNodeSetSrcMbCbCr(pHWNode, &stDrvS2, TDE_MB_CbCr_FILTER);
            TdeHalNodeSetTgt(pHWNode, &stDrvS2Tmp, enAlphaFrom);
            TdeHalNodeSetBaseOperate(pHWNode, TDE_MB_C_OPT, TDE_ALU_NONE, 0);
            stMbCmd.enMbMode = TDE_MB_CbCr_FILTER;
            TdeHalNodeSetMbMode(pHWNode, &stMbCmd);

            /* calculate chroma step and offset,
            	note:step calculation need use chroma width and height,
            		offset calculation need start address that user input at first */
            TdeOsiCalcMbFilterOpt(&stFilterOpt, pstMB->enMbFmt, TDE2_MB_COLOR_FMT_JPG_YCbCr444MBP, 
                                  &stAdjFieldRect, pstDstRect, HI_TRUE, HI_FALSE, enPicMode);
            TDE_FILLUP_RECT_BY_DRVSURFACE(stInRect, stDrvS2);
            TDE_FILLUP_RECT_BY_DRVSURFACE(stOutRect, stDrvS2Tmp);
            TdeOsiSetFilterOptAdjInfo(&stDrvS2, &stDrvS2Tmp, &stFilterOpt, TDE_CHILD_SCALE_MBC);
            if ((s32Ret = TdeOsiSetFilterChildNode(s32Handle, pHWNode, &stInRect, &stOutRect, HI_FALSE /*pMbOpt->bDeflicker*/,
                                              &stFilterOpt)) < 0)
            {
                tde_osr_disableirq();
                TdeOsiListPutPhyBuff(u32WorkBufNum);
                tde_osr_enableirq();
                TDE_TRACE(TDE_KERN_INFO, "mb high quality pass2 fail\n!");
                return s32Ret;
            }

            /* pass3. brightness and chroma combination */
            TdeHalNodeInitNd(pHWNode, HI_FALSE);
            TdeHalNodeSetColorConvert(pHWNode, &stConv);
            TDeHalNodeSetCsc(pHWNode, stCscOpt);
            TdeHalNodeSetSrcMbY(pHWNode, &stDrvS1Tmp, TDE_MB_CONCA_FILTER);
            TdeHalNodeSetSrcMbCbCr(pHWNode, &stDrvS2Tmp, TDE_MB_CONCA_FILTER);
            TdeHalNodeSetTgt(pHWNode, &stDrvDst, enAlphaFrom);
            TdeHalNodeSetGlobalAlpha(pHWNode, pMbOpt->u8OutAlpha, HI_TRUE);
            TdeHalNodeSetBaseOperate(pHWNode, TDE_MB_2OPT, TDE_ALU_NONE, 0);
            stMbCmd.enMbMode = TDE_MB_CONCA_FILTER;
            TdeHalNodeSetMbMode(pHWNode, &stMbCmd);
            TdeOsiSetFilterOptAdjInfo(&stDrvS1Tmp, &stDrvDst, &stFilterOpt, TDE_CHILD_SCALE_MB_CONCA_H);

            if ((s32Ret = TdeOsiMbSetClipPara(pstDstRect, pMbOpt, pHWNode)) < 0)
            {
                tde_osr_disableirq();
                TdeOsiListPutPhyBuff(u32WorkBufNum);
                tde_osr_enableirq();
                return s32Ret;
            }
            if ((s32Ret = TdeOsiSetNodeFinish(s32Handle, pHWNode, u32WorkBufNum, TDE_NODE_SUBM_ALONE)) < 0)
            {
                tde_osr_disableirq();
                TdeOsiListPutPhyBuff(u32WorkBufNum);
                tde_osr_enableirq();
                return s32Ret;
            }

            break;
        }
        case TDE2_MBRESIZE_QUALITY_MIDDLE:
        {
            TDE2_DEFLICKER_MODE_E enDeflickerMode = ((pMbOpt->bDeflicker)?(TDE2_DEFLICKER_MODE_RGB):(TDE2_DEFLICKER_MODE_NONE));

            TDE_TRACE(TDE_KERN_DEBUG, "Middle Quality Mb Resize!\n");
            /* pass1. chroma up-sample */
            TdeHalNodeInitNd(pHWNode, HI_FALSE);
            TdeHalNodeSetSrcMbCbCr(pHWNode, &stDrvS2, TDE_MB_CbCr_FILTER);
            TdeHalNodeSetTgt(pHWNode, &stDrvS1Tmp, enAlphaFrom);
            TdeHalNodeSetBaseOperate(pHWNode, TDE_MB_C_OPT, TDE_ALU_NONE, 0);
            stMbCmd.enMbMode = TDE_MB_CbCr_FILTER;
            TdeHalNodeSetMbMode(pHWNode, &stMbCmd);

            /* calculate chroma step and offset.
            	notes: for chroma up-sample, zoom by brightness size, and input and output is the same size */
            TdeOsiCalcMbFilterOpt(&stFilterOpt, pstMB->enMbFmt, TDE2_MB_COLOR_FMT_JPG_YCbCr444MBP, 
                                  &stAdjFieldRect, &stAdjFieldRect, HI_TRUE, HI_TRUE, enPicMode);
            TDE_FILLUP_RECT_BY_DRVSURFACE(stInRect, stDrvS2);
            TDE_FILLUP_RECT_BY_DRVSURFACE(stOutRect, stDrvS1Tmp);
            stFilterOpt.stAdjInfo.enScaleMode = TDE_CHILD_SCALE_MBC;
            TdeOsiSetFilterOptAdjInfo(&stDrvS2, &stDrvS1Tmp, &stFilterOpt, TDE_CHILD_SCALE_MBC);
            if ((s32Ret = TdeOsiSetFilterChildNode(s32Handle, pHWNode, &stInRect, &stOutRect, HI_FALSE,
                                              &stFilterOpt)) < 0)
            {
                tde_osr_disableirq();
                TdeOsiListPutPhyBuff(u32WorkBufNum);
                tde_osr_enableirq();
                TDE_TRACE(TDE_KERN_INFO, "mb middle quality pass1 fail\n!");
                return s32Ret;
            }

            /* pass2. combinate brightness and chroma, then zoom */
            TdeHalNodeInitNd(pHWNode, HI_FALSE);
            /* set encode by TdeHalNodeSetColorConvert */
            TdeHalNodeSetColorConvert(pHWNode, &stConv);
            TDeHalNodeSetCsc(pHWNode, stCscOpt);
            stDrvS1.enColorFmt = TDE_DRV_COLOR_FMT_YCbCr444MB;
            TdeHalNodeSetSrcMbY(pHWNode, &stDrvS1, TDE_MB_CONCA_FILTER);
            TdeHalNodeSetSrcMbCbCr(pHWNode, &stDrvS1Tmp, TDE_MB_CONCA_FILTER);
            TdeHalNodeSetTgt(pHWNode, &stDrvDst, enAlphaFrom);
            TdeHalNodeSetGlobalAlpha(pHWNode, pMbOpt->u8OutAlpha, HI_TRUE);
            TdeHalNodeSetBaseOperate(pHWNode, TDE_MB_2OPT, TDE_ALU_NONE, 0);
            stMbCmd.enMbMode = TDE_MB_CONCA_FILTER;
            TdeHalNodeSetMbMode(pHWNode, &stMbCmd);

            if ((s32Ret = TdeOsiMbSetClipPara(pstDstRect, pMbOpt, pHWNode)) < 0)
            {
                tde_osr_disableirq();
                TdeOsiListPutPhyBuff(u32WorkBufNum);
                tde_osr_enableirq();
                return s32Ret;
            }
            TdeOsiCalcMbFilterOpt(&stFilterOpt, pstMB->enMbFmt, TDE2_MB_COLOR_FMT_JPG_YCbCr444MBP, 
                                  &stAdjFieldRect, pstDstRect, HI_FALSE, HI_FALSE, TDE_FRAME_PIC_MODE);
            TDE_FILLUP_RECT_BY_DRVSURFACE(stInRect, stDrvS1);
            stFilterOpt.u32WorkBufNum = u32WorkBufNum;
            TdeOsiSetFilterOptAdjInfo(&stDrvS1, &stDrvDst, &stFilterOpt, TDE_CHILD_SCALE_MB_CONCA_M);
            if((s32Ret = TdeOsiSetFilterChildNode(s32Handle, pHWNode, &stInRect, pstDstRect, 
                                            enDeflickerMode, &stFilterOpt)) < 0)
            {
                tde_osr_disableirq();
                TdeOsiListPutPhyBuff(u32WorkBufNum);
                tde_osr_enableirq();
                return s32Ret;
            }
            
            break;
        }
        case TDE2_MBRESIZE_QUALITY_LOW:
        {
            TDE2_DEFLICKER_MODE_E enDeflickerMode = ((pMbOpt->bDeflicker)?(TDE2_DEFLICKER_MODE_RGB):(TDE2_DEFLICKER_MODE_NONE));
            TDE_TRACE(TDE_KERN_DEBUG, "Low Quality Mb Resize!\n");
            /* pass1. first connection and then zoom */
            TdeHalNodeInitNd(pHWNode, HI_FALSE);
            /* set encode by call TdeHalNodeSetColorConvert */
            TdeHalNodeSetColorConvert(pHWNode, &stConv);
            TDeHalNodeSetCsc(pHWNode, stCscOpt);
            TdeHalNodeSetSrcMbY(pHWNode, &stDrvS1, TDE_MB_CONCA_FILTER);

            /* foreground size need set to brightness size */
            TdeHalNodeSetSrcMbCbCr(pHWNode, &stDrvS2, TDE_MB_CONCA_FILTER);
            TdeHalNodeSetTgt(pHWNode, &stDrvDst, enAlphaFrom);
            TdeHalNodeSetGlobalAlpha(pHWNode, pMbOpt->u8OutAlpha, HI_TRUE);
            TdeHalNodeSetBaseOperate(pHWNode, TDE_MB_2OPT, TDE_ALU_NONE, 0);
            stMbCmd.enMbMode = TDE_MB_CONCA_FILTER;
            TdeHalNodeSetMbMode(pHWNode, &stMbCmd);
            if ((s32Ret = TdeOsiMbSetClipPara(pstDstRect, pMbOpt, pHWNode)) < 0)
            {
                tde_osr_disableirq();
                TdeOsiListPutPhyBuff(u32WorkBufNum);
                tde_osr_enableirq();
                return s32Ret;
            }
            TdeOsiCalcMbFilterOpt(&stFilterOpt, pstMB->enMbFmt, TDE2_MB_COLOR_FMT_JPG_YCbCr444MBP, 
                                  &stAdjFieldRect, pstDstRect, HI_FALSE, HI_FALSE, TDE_FRAME_PIC_MODE);
            TDE_FILLUP_RECT_BY_DRVSURFACE(stInRect, stDrvS1);
            TdeOsiSetFilterOptAdjInfo(&stDrvS1, &stDrvDst, &stFilterOpt, TDE_CHILD_SCALE_MB_CONCA_L);
            if((s32Ret = TdeOsiSetFilterChildNode(s32Handle, pHWNode, &stInRect, pstDstRect, 
                                            enDeflickerMode, &stFilterOpt)) < 0)
            {
                tde_osr_disableirq();
                TdeOsiListPutPhyBuff(u32WorkBufNum);
                tde_osr_enableirq();
                return s32Ret;
            }
            
            break;
        }
        default:     /*TDE_MBRESIZE_NONE*/
        {
            TDE2_DEFLICKER_MODE_E enDeflickerMode = ((pMbOpt->bDeflicker)?(TDE2_DEFLICKER_MODE_RGB):(TDE2_DEFLICKER_MODE_NONE));
            TDE_TRACE(TDE_KERN_DEBUG, "Mb CUS by Filter!\n");
            TDE_TRACE(TDE_KERN_DEBUG, "Mb CUS by Filter!\n");
            /* first up-sample, then connection */
            TdeHalNodeInitNd(pHWNode, HI_FALSE);
            /* set encode by call TdeHalNodeSetColorConvert */
            TdeHalNodeSetColorConvert(pHWNode, &stConv);
            TDeHalNodeSetCsc(pHWNode, stCscOpt);
            TdeHalNodeSetSrcMbY(pHWNode, &stDrvS1, TDE_MB_UPSAMP_CONCA);
            TdeHalNodeSetSrcMbCbCr(pHWNode, &stDrvS2, TDE_MB_UPSAMP_CONCA);
            TdeHalNodeSetTgt(pHWNode, &stDrvDst, enAlphaFrom);
            TdeHalNodeSetGlobalAlpha(pHWNode, pMbOpt->u8OutAlpha, HI_TRUE);
            TdeHalNodeSetBaseOperate(pHWNode, TDE_MB_2OPT, TDE_ALU_CONCA, 0);
            stMbCmd.enMbMode = TDE_MB_UPSAMP_CONCA;
            TdeHalNodeSetMbMode(pHWNode, &stMbCmd);
            if ((s32Ret = TdeOsiMbSetClipPara(pstDstRect, pMbOpt, pHWNode)) < 0)
            {
                tde_osr_disableirq();
                TdeOsiListPutPhyBuff(u32WorkBufNum);
                tde_osr_enableirq();
                return s32Ret;
            }
            TdeOsiCalcMbFilterOpt(&stFilterOpt, pstMB->enMbFmt, TDE2_MB_COLOR_FMT_JPG_YCbCr444MBP, 
                                  &stAdjFieldRect, &stAdjFieldRect, HI_TRUE, HI_TRUE, enPicMode);
            TDE_FILLUP_RECT_BY_DRVSURFACE(stInRect, stDrvS2);
            TdeOsiSetFilterOptAdjInfo(&stDrvS1, &stDrvDst, &stFilterOpt, TDE_CHILD_SCALE_MB_CONCA_CUS);
            if((s32Ret = TdeOsiSetFilterChildNode(s32Handle, pHWNode, &stInRect, pstDstRect, 
                                            enDeflickerMode, &stFilterOpt)) < 0)
            {
                tde_osr_disableirq();
                TdeOsiListPutPhyBuff(u32WorkBufNum);
                tde_osr_enableirq();
                return s32Ret;
            }
        }
    }

    return HI_SUCCESS;
}

/*****************************************************************************
* Function:      TdeOsiAdjQuality
* Description:   set filter type by input argument settings 
* Input:         pS1Tmp: buffer struct of foreground temporary bitmap
*                pS2Tmp: buffer struct of foreground temporary bitmap
*                pstMB:  MB bitmap information
*                pstMbRect: Mb bitmap operate zone
*                pstDstRect:  target bitmap operate zone
*                pMbOpt:  struct of operate parameter setting
* Output:        none
* Return:        number of temporary buffer/    -1: fail
* Others:        none
*****************************************************************************/
STATIC INLINE HI_S32 TdeOsiAdjQuality(TDE_DRV_SURFACE_S* pS1Tmp, TDE_DRV_SURFACE_S* pS2Tmp,
                                      TDE2_MB_S* pstMB, TDE2_RECT_S* pstMbRect, 
                                      TDE2_SURFACE_S* pstDst, TDE2_RECT_S* pstDstRect,
                                      TDE_PIC_MODE_E enPicMode, TDE2_MBOPT_S* pMbOpt)
{
    HI_U32 u32WorkBufferNum = 0;
   // HI_BOOL bMustUseBuf = HI_FALSE;
#if 0
    if (TDE2_MB_COLOR_FMT_JPG_YCbCr444MBP == pstMB->enMbFmt)
    {
        pMbOpt->enResize = TDE2_MBRESIZE_QUALITY_LOW;
        return 0;
    }

    if (TDE2_COLOR_FMT_YCbCr422 == pstDst->enColorFmt)
    {
        pMbOpt->enResize = TDE2_MBRESIZE_QUALITY_HIGH;
    }
#endif
    /* needn't zoom */
    if ((TDE2_MBRESIZE_NONE == pMbOpt->enResize)
        ||((pstMbRect->u32Height == pstDstRect->u32Height)
            &&(pstMbRect->u32Width == pstDstRect->u32Width)))
    {
        /* if it is YCbCr444MB, needn't up-sample, use TDE2_MBRESIZE_QUALITY_LOW directly */
        if (TDE2_MB_COLOR_FMT_JPG_YCbCr444MBP == pstMB->enMbFmt)
        {
            pMbOpt->enResize = TDE2_MBRESIZE_QUALITY_LOW;
            return 0;
        }
        /* if input is not YCbCr444MB and output is YCbCr422R(output:YC422R,unsupport block), it need be completed by quality_high zoom */
        else if (TDE2_COLOR_FMT_YCbCr422 == pstDst->enColorFmt)
        {
            pMbOpt->enResize = TDE2_MBRESIZE_QUALITY_HIGH;
        }
        else
        {
            /* if no zoom but open deflicker, or no zoom but use field mode, use quality_high filter */
            if (pMbOpt->bDeflicker || TDE_FRAME_PIC_MODE != enPicMode)
            {
                pMbOpt->enResize = TDE2_MBRESIZE_QUALITY_MIDDLE;
            }
            else
            {
                /* if no zoom and no deflicker, no use fliter */
                pMbOpt->enResize = TDE2_MBRESIZE_NONE;
                return 0;
            }
        }
    }
    else /* need zoom */
    {
        /* if it is YCbCr444MB and output is non YC422R, no need to up-sample and use TDE2_MBRESIZE_QUALITY_LOW  directly */
        if (TDE2_MB_COLOR_FMT_JPG_YCbCr444MBP == pstMB->enMbFmt
            && TDE2_COLOR_FMT_YCbCr422 != pstDst->enColorFmt)
        {
            pMbOpt->enResize = TDE2_MBRESIZE_QUALITY_LOW;
            return 0;
        }
        /* when zoom, output is YCbCr422R(output YC422R unspport block), need to use quality_high zoom */
        else if (TDE2_COLOR_FMT_YCbCr422 == pstDst->enColorFmt)
        {
            pMbOpt->enResize = TDE2_MBRESIZE_QUALITY_HIGH;
        }
    }

    /* get temporary bitmap size, if fail, use quality_low zoom */
    if (TDE2_MBRESIZE_QUALITY_HIGH == pMbOpt->enResize)
    {
#if 0    
        if((pstDstRect->u32Height * pstDstRect->u32Width) > (TDE_MAX_TMPBUFF_HEIGHT*TDE_MAX_TMPBUFF_WIDTH))
        {
            /* temporary buffer memory is not enough, use quality_low zoom */
            pMbOpt->enResize = TDE2_MBRESIZE_QUALITY_LOW;
            return 0;
        }
#endif        
        /* brightness and chroma size is the same after quality_high filter */
        pS1Tmp->u32Width  = pS2Tmp->u32Width = pstDstRect->u32Width;
        pS1Tmp->u32Pitch  = (pS1Tmp->u32Width + 3) & (0xfffffffc); /*pitch need align by 4 bytes */
        pS2Tmp->u32Pitch  = (pS2Tmp->u32Width * 2 + 3) & (0xfffffffc);
        pS1Tmp->u32Height = pS2Tmp->u32Height = pstDstRect->u32Height;

        pS1Tmp->u32PhyAddr = TdeOsiListGetPhyBuff(0);
        if (0 == pS1Tmp->u32PhyAddr)
        {
            pMbOpt->enResize = TDE2_MBRESIZE_QUALITY_LOW;
            return 0;
        }
        else
        {
            /* only if get successfully first, it must be successful at second time */
            pS2Tmp->u32PhyAddr = TdeOsiListGetPhyBuff(pS1Tmp->u32Pitch * pS1Tmp->u32Height);
            TDE_ASSERT(HI_NULL != pS2Tmp->u32PhyAddr);
        }

        pS1Tmp->enColorFmt = TDE_DRV_COLOR_FMT_YCbCr444MB;/* the result of quality_high is always 444MB */
        pS2Tmp->enColorFmt = TDE_DRV_COLOR_FMT_YCbCr444MB;

        u32WorkBufferNum = 2;
    }
    else if (TDE2_MBRESIZE_QUALITY_MIDDLE == pMbOpt->enResize)
    {
#if 0    
        if((pstMbRect->u32Height * pstMbRect->u32Width) > (TDE_MAX_TMPBUFF_HEIGHT*TDE_MAX_TMPBUFF_WIDTH))
        {
            /* when temporary buffer memory is not enough, use quality_high zoom */
            pMbOpt->enResize = TDE2_MBRESIZE_QUALITY_LOW;
            return 0;
        }
#endif        
        pS1Tmp->enColorFmt = TDE_DRV_COLOR_FMT_YCbCr444MB;/* quality_mid zoom result is always 444MB */
        pS1Tmp->u32Width  = pstMbRect->u32Width;
        if (TDE_FRAME_PIC_MODE != enPicMode)
        {
            pS1Tmp->u32Height = TDE_ADJ_FIELD_HEIGHT_BY_START(pstMbRect->s32Ypos, pstMbRect->u32Height);
        }
        else
        {
            pS1Tmp->u32Height = pstMbRect->u32Height;
        }
        pS1Tmp->u32Pitch   = (pS1Tmp->u32Width * 2 + 3) & (0xfffffffc);/*pitch need align by 4 bytes */
        pS1Tmp->u32PhyAddr = TdeOsiListGetPhyBuff(0);
        if (0 == pS1Tmp->u32PhyAddr)
        {
            pMbOpt->enResize = TDE2_MBRESIZE_QUALITY_LOW;
            return 0;
        }

        u32WorkBufferNum = 1;
    }
    else
    {
        pMbOpt->enResize = TDE2_MBRESIZE_QUALITY_LOW;
    }

    return u32WorkBufferNum;
}

/*****************************************************************************
* Function:      TdeOsiMbCalcHStep
* Description:   calculate horizontal stride zoomed
* Input:         u32Wi: input bitmap width
*                u32Wo: output bitmap width
*                enFmt: bitmap format
*                bCbCr: calculate chroma/brightness horizontal zoomed stide
* Output:        none
* Return:        calculate back step, premultiply is 2*12
* Others:        none
*****************************************************************************/
STATIC INLINE HI_U32 TdeOsiMbCalcHStep(HI_U32 u32Wi, HI_U32 u32Wo,
                                       TDE2_MB_COLOR_FMT_E enInFmt, TDE2_MB_COLOR_FMT_E enOutFmt, HI_BOOL bCbCr)
{
    if (1 >= u32Wo)
    {
        return 0;
    }
    if (enInFmt != enOutFmt && TDE2_MB_COLOR_FMT_JPG_YCbCr444MBP != enOutFmt)
    {
        TDE_TRACE(TDE_KERN_INFO, "Can't calc Hstep from format:%d to format:%d\n", enInFmt, enOutFmt);
        //return 0;
    }

    /* calculate horizontal zoomed step of brightness, need round for pointing stide */
    if (bCbCr && TDE2_MB_COLOR_FMT_JPG_YCbCr444MBP == enOutFmt)
    {
        if ((TDE2_MB_COLOR_FMT_JPG_YCbCr422MBVP != enInFmt) && (TDE2_MB_COLOR_FMT_JPG_YCbCr444MBP != enInFmt))
        {
            return (((u32Wi - 1) << TDE_FLOAT_BITLEN) / (u32Wo - 1) + 1) >> 1;
        }
    }

    if (bCbCr && TDE2_MB_COLOR_FMT_JPG_YCbCr444MBP == enInFmt)
    {
        if ((TDE2_MB_COLOR_FMT_JPG_YCbCr422MBVP != enOutFmt) && (TDE2_MB_COLOR_FMT_JPG_YCbCr444MBP != enOutFmt))
        {
            return (((u32Wi - 1) << TDE_FLOAT_BITLEN) / (u32Wo - 1) ) << 1;
        }
    }

    return (((u32Wi - 1) << (TDE_FLOAT_BITLEN + 1)) / (u32Wo - 1) + 1) >> 1;
}

/*****************************************************************************
* Function:      TdeOsiMbCalcVStep
* Description:   calculate vertical zoomed step
* Input:         u32Hi: input bitmap width
*                u32Ho: output bitmap width
*                enFmt: bitmap format
*                bCbCr: calculate vertical zoomed step of brightness/chroma
* Output:        none
* Return:        calculate back stride, premultiply is 2^12
* Others:        none
*****************************************************************************/
STATIC INLINE HI_U32 TdeOsiMbCalcVStep(HI_U32 u32Hi, HI_U32 u32Ho,
                                       TDE2_MB_COLOR_FMT_E enInFmt, TDE2_MB_COLOR_FMT_E enOutFmt, HI_BOOL bCbCr)
{
    if (1 >= u32Ho)
    {
        return 0;
    }
    if (enInFmt != enOutFmt && TDE2_MB_COLOR_FMT_JPG_YCbCr444MBP != enOutFmt)
    {
        TDE_TRACE(TDE_KERN_INFO, "Can't calc Vstep from format:%d to format:%d\n", enInFmt, enOutFmt);
        //return 0;
    }

    /* calculate vertical zoomed step of brightness, need round for pointing stide */
    if (bCbCr && TDE2_MB_COLOR_FMT_JPG_YCbCr444MBP == enOutFmt)
    {
        if ((TDE2_MB_COLOR_FMT_JPG_YCbCr422MBHP != enInFmt) && (TDE2_MB_COLOR_FMT_JPG_YCbCr444MBP != enInFmt))
        {
            return (((u32Hi - 1) << TDE_FLOAT_BITLEN) / (u32Ho - 1) + 1) >> 1;  /* 0.5 * step */
        }
    }

    if (bCbCr && TDE2_MB_COLOR_FMT_JPG_YCbCr444MBP == enInFmt)
    {
        if ((TDE2_MB_COLOR_FMT_JPG_YCbCr422MBHP != enOutFmt) && (TDE2_MB_COLOR_FMT_JPG_YCbCr444MBP != enOutFmt))
        {
            return (((u32Hi - 1) << (TDE_FLOAT_BITLEN)) / (u32Ho - 1) ) << 1;  /* 2 * step */
        }
    }
    if (bCbCr && TDE2_MB_COLOR_FMT_JPG_YCbCr422MBHP == enOutFmt)
    {
        if (TDE2_MB_COLOR_FMT_JPG_YCbCr420MBP == enInFmt)
        {
            return (((u32Hi - 1) << TDE_FLOAT_BITLEN) / (u32Ho - 1) + 1) >> 1;  /* 0.5 * step */
        }
    }
    if (bCbCr && TDE2_MB_COLOR_FMT_JPG_YCbCr420MBP == enOutFmt)
    {
        if ((TDE2_MB_COLOR_FMT_JPG_YCbCr422MBHP == enInFmt)
        || (TDE2_MB_COLOR_FMT_JPG_YCbCr444MBP == enInFmt))
        {
            return (((u32Hi - 1) << (TDE_FLOAT_BITLEN)) / (u32Ho - 1) ) << 1;  /* 2 * step */
        }
    }
    return (((u32Hi - 1) << (TDE_FLOAT_BITLEN + 1)) / (u32Ho - 1) + 1) >> 1;    /* step */
}

/*****************************************************************************
* Function:      TdeOsiMbCalcHOffset
* Description:   calculate vertical zoomed stride
* Input:         u32Xi: input bitmap start horizontal position 
*                enFmt: bitmap format
*                bCbCr: if calculate chroma's offset
* Output:        none
* Return:        calculate back Offset,premultiply:2^12
* Others:        none
*****************************************************************************/
STATIC INLINE HI_S32 TdeOsiMbCalcHOffset(HI_U32 u32Xi, TDE2_MB_COLOR_FMT_E enFmt, HI_BOOL bCbCr)
{
    if (!bCbCr)
    {
        return 0;
    }

    if ((TDE2_MB_COLOR_FMT_MP1_YCbCr420MBP == enFmt)
        || (TDE2_MB_COLOR_FMT_JPG_YCbCr420MBP == enFmt))
    {
        if ( 1 == (u32Xi & 0x1) )
        {
            return (HI_S32)(1 << (TDE_FLOAT_BITLEN - 2));    /* 0.25 * 2^12 */
        }

        return (HI_S32)((-1) << (TDE_FLOAT_BITLEN - 2));    /* -0.25 * 2^12 */
    }

    if ((TDE2_MB_COLOR_FMT_MP2_YCbCr420MBP == enFmt)
             || (TDE2_MB_COLOR_FMT_MP2_YCbCr420MBI == enFmt)
             || (TDE2_MB_COLOR_FMT_JPG_YCbCr422MBHP == enFmt))
    {
        if( 1 == (u32Xi & 0x1))
        {
            return (HI_S32)(1 << (TDE_FLOAT_BITLEN - 1));    /* 0.5 * 2^12 */
        }
    }

    return 0;

}

/*****************************************************************************
* Function:      TdeOsiMbCalcVOffset
* Description:   calculate vertical zoomed step
* Input:         u32Yi: input bitmap start height
*                enFmt: bitmap format
*                enPicMode: frame picture/ bottom field/ top field
*                bCbCr: if calculate chroma's offset
* Output:        none
* Return:        calculate back Offset,premultiply:2^12
* Others:        none
*****************************************************************************/
STATIC INLINE HI_S32 TdeOsiMbCalcVOffset(HI_U32 u32Yi, TDE2_MB_COLOR_FMT_E enFmt,
                                         TDE_PIC_MODE_E enPicMode, HI_BOOL bCbCr)
{
    if (bCbCr)
    {
        if (TDE2_MB_COLOR_FMT_JPG_YCbCr420MBP == enFmt)
        {
            switch (u32Yi & 0x1)
            {
            case 1:
                return (HI_S32)(1 << (TDE_FLOAT_BITLEN - 2));    /* 0.25 * 2^12 */
            default:
                return (HI_S32)((-1) << (TDE_FLOAT_BITLEN - 2));    /* -0.25 * 2^12 */
            }
        }
        else if ((TDE2_MB_COLOR_FMT_MP2_YCbCr420MBI == enFmt)
                || (TDE2_MB_COLOR_FMT_MP1_YCbCr420MBP == enFmt)
                || (TDE2_MB_COLOR_FMT_MP2_YCbCr420MBP == enFmt))
        {
            switch (u32Yi & 0x3)
            {
            case 0:
            {
                if (TDE_BOTTOM_FIELD_PIC_MODE == enPicMode)
                {
                    return (HI_S32)((-5) << (TDE_FLOAT_BITLEN - 3));    /* -0.625 * 2^12 */
                }
                else
                {
                    return (HI_S32)((-1) << (TDE_FLOAT_BITLEN - 3));    /* -0.125 * 2^12 */
                }
            }
            case 1:
            {
                if (TDE_BOTTOM_FIELD_PIC_MODE == enPicMode)
                {
                    return (HI_S32)((-3) << (TDE_FLOAT_BITLEN - 3));    /* -0.375 * 2^12 */
                }
                else
                {
                    return (HI_S32)(1 << (TDE_FLOAT_BITLEN - 3));    /* 0.125 * 2^12 */
                }
            }
            case 2:
            {
                if (TDE_BOTTOM_FIELD_PIC_MODE == enPicMode)
                {
                    return (HI_S32)((-1) << (TDE_FLOAT_BITLEN - 3));    /* -0.125 * 2^12 */
                }
                else
                {
                    return (HI_S32)(3 << (TDE_FLOAT_BITLEN - 3));    /* 0.375 * 2^12 */
                }
            }
            case 3:
            {
                if (TDE_BOTTOM_FIELD_PIC_MODE == enPicMode)
                {
                    return (HI_S32)(1 << (TDE_FLOAT_BITLEN - 3));    /* 0.125 * 2^12 */
                }
                else
                {
                    return (HI_S32)(5 << (TDE_FLOAT_BITLEN - 3));    /* 0.625 * 2^12 */
                }
            }
            default:
                return 0;
            }
        }
        else if (TDE2_MB_COLOR_FMT_JPG_YCbCr422MBVP == enFmt)
        {
            switch (u32Yi & 0x1)
            {
            case 1:
                return (HI_S32)(1 << (TDE_FLOAT_BITLEN - 1));    /* 0.5 * 2^12 */
            default:
                return 0;
            }
        }
        else
        {
            return 0;
        }
    }
    else
    {
        if ((TDE2_MB_COLOR_FMT_MP2_YCbCr420MBI == enFmt)
            || (TDE2_MB_COLOR_FMT_MP1_YCbCr420MBP == enFmt)
            || (TDE2_MB_COLOR_FMT_MP2_YCbCr420MBP == enFmt))
        {
            switch (u32Yi & 0x3)
            {
            case 0:
            case 2:
            {
                if (TDE_BOTTOM_FIELD_PIC_MODE == enPicMode)
                {
                    return (HI_S32)((-1) << (TDE_FLOAT_BITLEN - 1));    /* -0.5 * 2^12 */
                }
                else
                {
                    return 0;
                }
            }
            case 1:
            case 3:
            {
                if (TDE_BOTTOM_FIELD_PIC_MODE == enPicMode)
                {
                    return 0;
                }
                else
                {
                    return (HI_S32)(1 << (TDE_FLOAT_BITLEN - 1));    /* 0.5 * 2^12 */
                }
            }
            default:
                return 0;
            }
        }
        else
        {
            return 0;
        }
    }
}

/*****************************************************************************
* Function:      TdeOsiCalcNoLapSlice
* Description:   calculate value when block,it is no lap
* Input:         pu32WFirst: return first block's size
*                pu32WLast: return last block's size
*                u32Xi: input bitmap start horizontal  coordinate 
*                u32BmpW: bitmap width(no horizontal zoom, input and output is consistent)
*                u32Bppi: pixel bit width/ each pixel in input bitmap
* Output:        none
* Return:        return block number
* Others:        AI7D02711 alloc dynamicly
                 AI7D02579 add antiscaning for YC422R and operate on last point
*****************************************************************************/
STATIC INLINE HI_U32 TdeOsiCalcNoLapSlice(TDE_SLICE_INFO *pSliceInfo, HI_U32 u32MaxNum, HI_U32 u32Xi, HI_U32 u32Xo,
                                          HI_U32 u32BmpW, TDE_FILTER_OPT* pstFilterOpt)
{
#define TDE_GET_STARTX_BY_SCANINFO(hScan, sliceW, x) \
        ((hScan) ? (HI_U32)((x) + (sliceW) - 1) : ((HI_U32)(x)))

    /* HI_U32 u32SliceNum; */
    HI_U32 u32BmpXo = u32Xo;
    HI_U32 u32OverLapPix = (pstFilterOpt->bEvenStartInX && pstFilterOpt->bEvenStartOutX) ? 4 : 2;
    HI_U32 n = 1;
        
    if (TDE_MAX_SLICE_WIDTH >= u32BmpW)
    {
        pSliceInfo[0].u32SliceWi = u32BmpW;
        pSliceInfo[0].u32SliceXi = TDE_GET_STARTX_BY_SCANINFO(pstFilterOpt->stSrcDire.enHScan, pSliceInfo[0].u32SliceWi, u32Xi);;
        pSliceInfo[0].s32SliceHOfst = pstFilterOpt->s32HOffset;
        pSliceInfo[0].u32SliceWo = u32BmpW;
        pSliceInfo[0].u32SliceXo = TDE_GET_STARTX_BY_SCANINFO(pstFilterOpt->stDstDire.enHScan, pSliceInfo[0].u32SliceWo, u32Xo);;
        
        pSliceInfo[0].enSliceType = TDE_NO_BLOCK_SLICE_TYPE;
        return 1;
    }
    else
    {
        HI_U32 u32OfstWord;
        /* HI_U32 u32WFirst; */
        /* HI_U32 u32W64SliceNum;*/    /* not include first block */

        /* calculate start position and the offset of word */
        u32OfstWord = (u32Xi * pstFilterOpt->u32Bppi % 128) / pstFilterOpt->u32Bppi;
        if (pstFilterOpt->bEvenStartInX)
        {
            u32OfstWord &= (~0x1);
        }

        pSliceInfo[0].enSliceType = TDE_FIRST_BLOCK_SLICE_TYPE;

        pSliceInfo[0].u32SliceWi = TDE_MAX_SLICE_WIDTH - u32OfstWord;
        pSliceInfo[0].u32SliceXi = 
            TDE_GET_STARTX_BY_SCANINFO(pstFilterOpt->stSrcDire.enHScan, pSliceInfo[0].u32SliceWi, u32Xi);
        pSliceInfo[0].s32SliceHOfst = pstFilterOpt->s32HOffset;
        pSliceInfo[0].u32SliceWo = pSliceInfo[0].u32SliceWi;
        pSliceInfo[0].u32SliceXo = 
            TDE_GET_STARTX_BY_SCANINFO(pstFilterOpt->stDstDire.enHScan, pSliceInfo[0].u32SliceWo, u32Xo);
        u32Xo += (pstFilterOpt->bBadLastPix) ? (pSliceInfo[0].u32SliceWo - u32OverLapPix) : (pSliceInfo[0].u32SliceWo);
        u32Xi += (pstFilterOpt->bBadLastPix) ? (pSliceInfo[0].u32SliceWi - u32OverLapPix) : (pSliceInfo[0].u32SliceWi);
        
        for (n = 1; n <= u32MaxNum + 1; n++) /* u32MaxNum protect form memory is not outside */
        {
            if ((HI_S32)(u32BmpW - u32Xo + u32BmpXo) <= TDE_MAX_SLICE_WIDTH)
            {
                break;
            }
            pSliceInfo[n].enSliceType = TDE_MID_BLOCK_SLICE_TYPE;
            pSliceInfo[n].u32SliceWi = TDE_MAX_SLICE_WIDTH;
            pSliceInfo[n].u32SliceXi = 
                TDE_GET_STARTX_BY_SCANINFO(pstFilterOpt->stSrcDire.enHScan, pSliceInfo[n].u32SliceWi, u32Xi);
            pSliceInfo[n].s32SliceHOfst = pstFilterOpt->s32HOffset;
            pSliceInfo[n].u32SliceWo = pSliceInfo[n].u32SliceWi;
            pSliceInfo[n].u32SliceXo = 
                TDE_GET_STARTX_BY_SCANINFO(pstFilterOpt->stDstDire.enHScan, pSliceInfo[n].u32SliceWo, u32Xo);
            u32Xo += (pstFilterOpt->bBadLastPix) ? (pSliceInfo[n].u32SliceWo - u32OverLapPix) : (pSliceInfo[n].u32SliceWo);
            u32Xi += (pstFilterOpt->bBadLastPix) ? (pSliceInfo[n].u32SliceWi - u32OverLapPix) : (pSliceInfo[n].u32SliceWi);
        }
        
        if ((HI_S32)(u32BmpW - u32Xo + u32BmpXo) > 0)
        {
            /* recalculate to leaving width on last block */
            pSliceInfo[n].enSliceType = TDE_LAST_BLOCK_SLICE_TYPE;
            pSliceInfo[n].u32SliceWi = u32BmpW - u32Xo + u32BmpXo;
            pSliceInfo[n].u32SliceXi = 
                TDE_GET_STARTX_BY_SCANINFO(pstFilterOpt->stDstDire.enHScan, pSliceInfo[n].u32SliceWi, u32Xi);
            pSliceInfo[n].s32SliceHOfst = pstFilterOpt->s32HOffset;
            pSliceInfo[n].u32SliceWo = pSliceInfo[n].u32SliceWi;
            
            pSliceInfo[n].u32SliceXo = 
                TDE_GET_STARTX_BY_SCANINFO(pstFilterOpt->stDstDire.enHScan, pSliceInfo[n].u32SliceWo, u32Xo);
        }
    }
    return n + 1;
}

/*****************************************************************************
* Function:      TdeOsiCalcOverLapSlice
* Description:   calcute value when block,it overlap
* Input:         pSliceInfo: slice information
*                u32BmpXi: input start horizontal position of bitmap
*                u32BmpWi: input bitmap width
*                u32BmpHStep: horizontal stride
*                s32HOffset: horizontal offset
*                u32BmpXo: output start horizontal position of bitmap
*                u32BmpWo: output bitmap width
*                u32Bppi: input bit width each pixel
* Output:        none
* Return:        return slice number
* Others:        AI7D02711 change to alloc dynamicly
*****************************************************************************/
STATIC INLINE HI_U32 TdeOsiCalcOverLapSlice(TDE_SLICE_INFO *pSliceInfo, HI_U32 u32MaxNum, HI_U32 u32BmpXi, HI_U32 u32BmpWi, 
                                            HI_U32 u32BmpXo, HI_U32 u32BmpWo, TDE_FILTER_OPT* pstFilterOpt)
{
    HI_U32 u32Xi = u32BmpXi;
    HI_U32 u32Wi;
    HI_U32 s32HOfst = pstFilterOpt->s32HOffset;
    HI_U32 u32Xo = u32BmpXo;
    HI_U32 u32Wo;
    HI_U32 u32OfstWord = 0;
    HI_U32 u32LeftWi = u32BmpWi;
    HI_U32 u32LeftWo = u32BmpWo;
    HI_U32 u32CalcWi = 0;
    //HI_U32 u32CalcWo = 0;
    HI_U32 u32BackPix = (pstFilterOpt->bEvenStartInX) ? 2 : 0; /* u32BackPix is 2 at h2 version */
    /* asymmetry offset is 0, otherwise offset is 0.5 * 2^12 */
    //HI_U32 u32HOfstDiff = 0;//(pstFilterOpt->bCoefSym) ? (0) : (1 << (TDE_FLOAT_BITLEN - 1));
    HI_U32 n;

    if (0 == pstFilterOpt->u32HStep)
    {
        pSliceInfo[0].u32SliceXi = u32Xi;
        pSliceInfo[0].u32SliceWi = u32BmpWi;
        pSliceInfo[0].s32SliceHOfst = pstFilterOpt->s32HOffset;
        pSliceInfo[0].u32SliceXo = u32Xo;
        pSliceInfo[0].u32SliceWo = u32BmpWo;
        pSliceInfo[0].enSliceType = TDE_NO_BLOCK_SLICE_TYPE;
        return 1;
    }

    /* u32MaxNum: protect from memory is inside */
    for (n = 0; n < u32MaxNum; n++) /*AI7D02711*/
    {
	/* step1. from Xi, HOfst, Xo calculate into Wi */
        if (TDE_MAX_SLICE_WIDTH >= u32LeftWi)
        {
            u32Wi = u32LeftWi;

	     /* last slice,update back_pix*/
	    u32BackPix = 0;	 
        }
        else
        {
            /* calculate start position and word offset */
            u32OfstWord = (u32Xi * pstFilterOpt->u32Bppi % 128) / pstFilterOpt->u32Bppi;
            u32Wi = TDE_MAX_SLICE_WIDTH - u32OfstWord;
        }

	/* step2  calculate Wo by Wi */
	if (0 > (HI_S32)((u32Wi - 5 - u32BackPix) * TDE_NO_SCALE_STEP - s32HOfst))
	{
	     u32Wo = u32LeftWo;
	}
	else
	{
	     if (TDE_MAX_SLICE_WIDTH >= u32LeftWi)
            {
                u32Wo = u32LeftWo;
            }
	     else
            {
                u32Wo = ((u32Wi - 5 - u32BackPix) * TDE_NO_SCALE_STEP - s32HOfst ) / pstFilterOpt->u32HStep + 1;
                /* if output need drop-sample, need ensure output is even */
                u32Wo = (pstFilterOpt->bEvenStartOutX) ? (u32Wo & (~0x1)) : (u32Wo);
            }
	     	 
	}

	if (u32Wo > u32LeftWo)
        {
            u32Wo = u32LeftWo;
        }

	/* if drop-sample */
	if (pstFilterOpt->bEvenStartOutX)
	{
		u32CalcWi = ((s32HOfst + pstFilterOpt->u32HStep*(u32Wo - 1)) >> TDE_FLOAT_BITLEN) + 5 + u32BackPix;
	}
	else
	{
		u32CalcWi = u32Wi;
	}

	/* protect judge, output w is inside of bitmap W */
        if (u32CalcWi > u32LeftWi)
        {
            u32CalcWi = u32LeftWi;
        }

        /* step2. get the nth array of coecifients: Xi, Wi, HOfst, Xo, Wo */
        pSliceInfo[n].s32SliceHOfst = s32HOfst;
	 pSliceInfo[n].u32SliceWi = (pstFilterOpt->bEvenStartInX) ? ((u32CalcWi + 1) & (~0x1)) : (u32CalcWi);	
        pSliceInfo[n].u32SliceWo = u32Wo;
        pSliceInfo[n].u32SliceXi = u32Xi;
        pSliceInfo[n].u32SliceXo = u32Xo;
        pSliceInfo[n].enSliceType = TDE_MID_BLOCK_SLICE_TYPE;	

	/* if the leaving width is 0, break */
        if (((u32BmpXi + u32BmpWi) <= (u32Xi + pSliceInfo[n].u32SliceWi))
            || ((u32BmpXo + u32BmpWo) <= (u32Xo + u32Wo)))
        {
            n++; 
            break;
        }
	
	if (!pstFilterOpt->bEvenStartOutX)
	{
		u32Xi = ((s32HOfst + pstFilterOpt->u32HStep * u32Wo) >> TDE_FLOAT_BITLEN) - 3 + u32Xi;
		s32HOfst = ((s32HOfst + pstFilterOpt->u32HStep * u32Wo) & 0x0fff) + 3 * TDE_NO_SCALE_STEP;
		if (pstFilterOpt->bEvenStartInX)
		{
			s32HOfst += (u32Xi & 0x1)?(TDE_NO_SCALE_STEP):(0);
			u32Xi = u32Xi & ~0x1;
		}
		u32Xo = u32Xo + u32Wo;
	}
	else
	{
		u32Xi = ((s32HOfst + pstFilterOpt->u32HStep * (u32Wo - 2)) >> TDE_FLOAT_BITLEN) - 3 + u32Xi;
		s32HOfst = ((s32HOfst + pstFilterOpt->u32HStep * (u32Wo - 2)) & 0x0fff) + 3 * TDE_NO_SCALE_STEP;
		if (pstFilterOpt->bEvenStartInX)
		{
			s32HOfst += (u32Xi & 0x1)?(TDE_NO_SCALE_STEP):(0);
			u32Xi = u32Xi & ~0x1;
		}
		u32Xo = u32Xo + u32Wo - 2;
	}


	

	#if 0	
        if (TDE_MAX_SLICE_WIDTH >= u32LeftWi)
        {
            u32Wi = u32LeftWi;
        }
        else
        {
            u32OfstWord = (u32Xi * pstFilterOpt->u32Bppi % 32) / pstFilterOpt->u32Bppi;
            u32Wi = TDE_MAX_SLICE_WIDTH - u32OfstWord;
        }

        if (0 > (HI_S32)((u32Wi - 5 - u32BackPix) * TDE_NO_SCALE_STEP - s32HOfst - u32HOfstDiff))
        {
            u32Wo = u32LeftWo;
        }
        else
        {
            if (TDE_MAX_SLICE_WIDTH >= u32LeftWi)
            {
                u32Wo = u32LeftWo;
            }
            else
            {
                u32Wo = ((u32Wi - 5 - u32BackPix) * TDE_NO_SCALE_STEP - s32HOfst - u32HOfstDiff) / pstFilterOpt->u32HStep + 1;
                u32Wo = (pstFilterOpt->bEvenStartOutX) ? (u32Wo & (~0x1)) : (u32Wo);
            }
        }

        if (u32Wo > u32LeftWo)
        {
            u32Wo = u32LeftWo;
        }

        if (!pstFilterOpt->bEvenStartInX)
        {
            u32CalcWi = u32Wi;
        }
        else
        {
            u32CalcWi = ((s32HOfst + u32HOfstDiff + pstFilterOpt->u32HStep*(u32Wo - 1)) >> TDE_FLOAT_BITLEN) + 5;
        }

        if (u32CalcWi > u32LeftWi)
        {
            u32CalcWi = u32LeftWi;
        }

        pSliceInfo[n].s32SliceHOfst = s32HOfst;
       //pSliceInfo[n].u32SliceWi = (pstFilterOpt->bEvenStartInX) ? ((u32CalcWi + 3) & (~0x1)) : (u32CalcWi);
		pSliceInfo[n].u32SliceWi = (pstFilterOpt->bEvenStartInX) ? ((u32CalcWi) & (~0x1)) : (u32CalcWi);	
        pSliceInfo[n].u32SliceWo = u32Wo;
        pSliceInfo[n].u32SliceXi = u32Xi;
        pSliceInfo[n].u32SliceXo = u32Xo;
        pSliceInfo[n].enSliceType = TDE_MID_BLOCK_SLICE_TYPE;

        if (((u32BmpXi + u32BmpWi) <= (u32Xi + u32CalcWi/*u32Wi*/))
            || ((u32BmpXo + u32BmpWo) <= (u32Xo + u32Wo)))
        {
            n++; /*AI7D02711*/
            break;
        }

        u32CalcWo = (pstFilterOpt->bEvenStartOutX) ? (u32Wo - 2) : u32Wo;
        u32Xi = ((s32HOfst + pstFilterOpt->u32HStep * u32CalcWo) >> TDE_FLOAT_BITLEN) - 3 + u32Xi;
        s32HOfst = ((s32HOfst + pstFilterOpt->u32HStep * u32CalcWo) & 0x0fff) + 3 * TDE_NO_SCALE_STEP;
        
        if (pstFilterOpt->bEvenStartInX && (u32Xi & 0x1))
        {
            u32Xi -= 1;
            s32HOfst += TDE_NO_SCALE_STEP;
        }

        u32Xo = u32Xo + u32CalcWo;
#endif
        /* update LeftWi,u32LeftWo */
        u32LeftWi = u32BmpXi + u32BmpWi - u32Xi;
        u32LeftWo = u32BmpXo + u32BmpWo - u32Xo;
		
    }
    pSliceInfo[0].enSliceType = TDE_FIRST_BLOCK_SLICE_TYPE;
    pSliceInfo[n-1].enSliceType = TDE_LAST_BLOCK_SLICE_TYPE;
    if (n == 1)
    {
        pSliceInfo[0].enSliceType = TDE_NO_BLOCK_SLICE_TYPE;
    }

    return n;
}

/*****************************************************************************
* Function:      TdeOsiSetFilterChildNode
* Description:   calculate child nodes of slice and add into task list
* Input:         s32Handle: task list handle
*                pNode: node config parameter
*                pInRect: input bitmap zone
*                pOutRect: output bitmap zone
*                bDeflicker: if deflicker
*                pstFilterOpt: filter config parameter
* Output:        none
* Return:        return slice number
* Others:        none
*****************************************************************************/
STATIC HI_S32 TdeOsiSetFilterChildNode(TDE_HANDLE s32Handle, TDE_HWNode_S* pNode, TDE2_RECT_S* pInRect,
                                       TDE2_RECT_S*  pOutRect,
                                       TDE2_DEFLICKER_MODE_E enDeflickerMode,
                                       TDE_FILTER_OPT* pstFilterOpt)
{
    TDE_SLICE_INFO* sliceInfo = HI_NULL;
    TDE_CHILD_INFO stChildInfo = {0};
    HI_U32 u32SliceNum = 1;
    HI_BOOL bDeflicker = (TDE2_DEFLICKER_MODE_NONE == enDeflickerMode)?HI_FALSE:HI_TRUE;
    HI_U32 u32MaxSliceNum = 
           TDE_CALC_MAX_SLICE_NUM(bDeflicker, pstFilterOpt->u32VStep, pInRect->u32Width);
    HI_U32 u32FirstNum;
    HI_S32 s32Ret;
    HI_S32 i;
    HI_U32 u32TDE_CLIP_START = pNode->u32TDE_CLIP_START;
    HI_U32 u32TDE_CLIP_STOP = pNode->u32TDE_CLIP_STOP;

    TDE_TRACE(TDE_KERN_DEBUG, "slice calc u32MaxSliceNum: %d!\n", u32MaxSliceNum);

    tde_osr_disableirq();    
    sliceInfo = TDE_MALLOC(u32MaxSliceNum * sizeof(TDE_SLICE_INFO));
    tde_osr_enableirq();
    if (HI_NULL == sliceInfo)
    {
        return HI_ERR_TDE_NO_MEM;
    }

    memset(sliceInfo, 0, u32MaxSliceNum * sizeof(TDE_SLICE_INFO));

    /* set child node zoom mode */
    memcpy(&stChildInfo.stAdjInfo, &pstFilterOpt->stAdjInfo, sizeof(TDE_MBSTART_ADJ_INFO_S));
    memcpy(&stChildInfo.stDSAdjInfo, &pstFilterOpt->stDSAdjInfo, sizeof(TDE_DOUBLESRC_ADJ_INFO_S));
    /*  u32ChildUpdate :
     * _________________________________________
     * |    |    |    |    |    |    |    |    |
     * | ...| 0  | 0  | 1  | 1  | 1  | 1  |  1 |
     * |____|____|____|____|____|____|____|____|
     *                   |    |    |    |    |
     *                  \/   \/   \/   \/   \/
     *                u32Wo u32Xo HOfst u32Wi u32Xi
     *                u32Ho u32Yo VOfst u32Hi u32Yi
     */

    /* if block/how to block */
    /* only zoom for horizontal, not do deflicker and don't zoom when vertical zoom */
    if ((HI_FALSE == bDeflicker)
        && (TDE_NO_SCALE_STEP == pstFilterOpt->u32VStep))
    {
        TDE_TRACE(TDE_KERN_DEBUG, "one slice block for filter!\n");
        u32SliceNum = 1;
        sliceInfo[0].s32SliceHOfst = pstFilterOpt->s32HOffset;
        sliceInfo[0].u32SliceWi = pInRect->u32Width;
        sliceInfo[0].u32SliceWo = pOutRect->u32Width;
        sliceInfo[0].u32SliceXi = pInRect->s32Xpos;
        sliceInfo[0].u32SliceXo = pOutRect->s32Xpos;
        sliceInfo[0].enSliceType = TDE_NO_BLOCK_SLICE_TYPE;
        stChildInfo.u64Update = 0x1F;
    }
    /* donot horizonal zoom, do deflicker or use nonoverlap blocj when vertical zoom */
    else if ((TDE_NO_SCALE_STEP == pstFilterOpt->u32HStep) 
             && ((bDeflicker) || (TDE_NO_SCALE_STEP != pstFilterOpt->u32VStep)))
    {
        TDE_TRACE(TDE_KERN_DEBUG, "NoLap slice block for filter!\n");
        u32SliceNum = TdeOsiCalcNoLapSlice(sliceInfo, u32MaxSliceNum, (HI_U32)(pInRect->s32Xpos), 
                                           (HI_U32)(pOutRect->s32Xpos),
                                           pInRect->u32Width, pstFilterOpt);
        stChildInfo.u64Update = 0x1B;

    }
    /* do horizontal zoom, and do vertical zoom or overlap block when  deflicker */
    else if ((TDE_NO_SCALE_STEP != pstFilterOpt->u32HStep) 
             && ((bDeflicker) || (TDE_NO_SCALE_STEP != pstFilterOpt->u32VStep)))
    {
        TDE_TRACE(TDE_KERN_DEBUG, "OverLap slice block for filter!\n");
        u32SliceNum = TdeOsiCalcOverLapSlice(sliceInfo, u32MaxSliceNum, pInRect->s32Xpos,
                                             pInRect->u32Width, (HI_U32)pOutRect->s32Xpos, 
                                             pOutRect->u32Width, pstFilterOpt);
        TDE_TRACE(TDE_KERN_DEBUG, "slice real u32SliceNum: %d!\n", u32SliceNum);
        stChildInfo.u64Update = 0x1F;
    }

    /* set filter parameter information of parent node */
    TdeOsiSetResizePara(pNode, pstFilterOpt, TDE_NODE_SUBM_PARENT);

    if (bDeflicker)
    {
        TdeOsiSetDeflickerPara(pNode, enDeflickerMode, pstFilterOpt);
    }

    /* if horizontal zoom out, do filter form right to left for slice */
    if ((TDE_SCAN_RIGHT_LEFT == pstFilterOpt->stDstDire.enHScan) || (pInRect->u32Width < pOutRect->u32Width))
    {
        u32FirstNum = u32SliceNum - 1;
    }
    else     /* othewise, fliter form left to right */
    {
        u32FirstNum = 0;
    }

    stChildInfo.u32Xi = sliceInfo[u32FirstNum].u32SliceXi;
    stChildInfo.u32Yi = (HI_U32)pInRect->s32Ypos;
    stChildInfo.u32Wi = sliceInfo[u32FirstNum].u32SliceWi;
    stChildInfo.u32Hi = pInRect->u32Height;
    stChildInfo.u32HOfst = sliceInfo[u32FirstNum].s32SliceHOfst;
    stChildInfo.u32VOfst = pstFilterOpt->s32VOffset;
    stChildInfo.u32Xo = sliceInfo[u32FirstNum].u32SliceXo;
    stChildInfo.u32Yo = (HI_U32)pOutRect->s32Ypos;
    stChildInfo.u32Wo = sliceInfo[u32FirstNum].u32SliceWo;
    stChildInfo.u32Ho = pOutRect->u32Height;
    stChildInfo.enSliceType = sliceInfo[u32FirstNum].enSliceType;
    TdeHalNodeAddChild(pNode, &stChildInfo);
    s32Ret = TdeOsiSetNodeFinish(s32Handle, pNode, pstFilterOpt->u32WorkBufNum, TDE_NODE_SUBM_PARENT);
    if (HI_SUCCESS != s32Ret)
    {
        TDE_FREE(sliceInfo);
        return s32Ret;
    }

    /* accoding to each child node information, set register.
    the first is set as parent node,so begin form the second node */
    if (0 == u32FirstNum)    /* from left to right */
    {
        for (i = u32FirstNum + 1; i < (HI_S32)u32SliceNum; i++)
        {
            //TdeHalNodeInitNd(pNode, HI_TRUE);
            /* AE5D03390: outside software evade zone, clip bug */
            TdeHalNodeInitChildNd(pNode,u32TDE_CLIP_START, u32TDE_CLIP_STOP);
            /* set child node fliter parameter information */
            TdeOsiSetResizePara(pNode, pstFilterOpt, TDE_NODE_SUBM_CHILD);
            stChildInfo.u32Xi = sliceInfo[i].u32SliceXi;
            stChildInfo.u32Yi = (HI_U32)pInRect->s32Ypos;
            stChildInfo.u32Wi = sliceInfo[i].u32SliceWi;
            stChildInfo.u32Hi = pInRect->u32Height;
            stChildInfo.u32HOfst = sliceInfo[i].s32SliceHOfst;
            stChildInfo.u32VOfst = pstFilterOpt->s32VOffset;
            stChildInfo.u32Xo = sliceInfo[i].u32SliceXo;
            stChildInfo.u32Yo = (HI_U32)pOutRect->s32Ypos;
            stChildInfo.u32Wo = sliceInfo[i].u32SliceWo;
            stChildInfo.u32Ho = pOutRect->u32Height;
            stChildInfo.enSliceType = sliceInfo[i].enSliceType;
            TdeHalNodeAddChild(pNode, &stChildInfo);
            s32Ret = TdeOsiSetNodeFinish(s32Handle, pNode, 0, TDE_NODE_SUBM_CHILD);
            if (HI_SUCCESS != s32Ret)
            {
                tde_osr_disableirq();
                TDE_FREE(sliceInfo);
                tde_osr_enableirq();
                return s32Ret;
            }
        }
    }
    else     /* from right to left */
    {
        for (i = u32FirstNum - 1; i >= 0; i--)
        {
            //TdeHalNodeInitNd(pNode, HI_TRUE);
            /* AE5D03390:outside software evade zone, clip bug */
            TdeHalNodeInitChildNd(pNode,u32TDE_CLIP_START, u32TDE_CLIP_STOP);
            TdeOsiSetResizePara(pNode, pstFilterOpt, TDE_NODE_SUBM_CHILD);
            stChildInfo.u32Xi = sliceInfo[i].u32SliceXi;
            stChildInfo.u32Yi = (HI_U32)pInRect->s32Ypos;
            stChildInfo.u32Wi = sliceInfo[i].u32SliceWi;
            stChildInfo.u32Hi = pInRect->u32Height;
            stChildInfo.u32HOfst = sliceInfo[i].s32SliceHOfst;
            stChildInfo.u32VOfst = pstFilterOpt->s32VOffset;
            stChildInfo.u32Xo = sliceInfo[i].u32SliceXo;
            stChildInfo.u32Yo = (HI_U32)pOutRect->s32Ypos;
            stChildInfo.u32Wo = sliceInfo[i].u32SliceWo;
            stChildInfo.u32Ho = pOutRect->u32Height;
            stChildInfo.enSliceType = sliceInfo[i].enSliceType;
            TdeHalNodeAddChild(pNode, &stChildInfo);
            s32Ret = TdeOsiSetNodeFinish(s32Handle, pNode, 0, TDE_NODE_SUBM_CHILD);
            if (HI_SUCCESS != s32Ret)
            {
                tde_osr_disableirq();
                TDE_FREE(sliceInfo);
                tde_osr_enableirq();
                return s32Ret;
            }
        }
    }

    tde_osr_disableirq();
    TDE_FREE(sliceInfo);
    tde_osr_enableirq();
    
    return HI_SUCCESS;
}

typedef struct tagUpdateConfig
{
    HI_S32 ori_in_width; //original image width
    HI_S32 ori_in_height; //original image height 
	HI_S32 zme_out_width; //output full image width
	HI_S32 zme_out_height; //output full image height

	HI_S32 update_instart_w; //the start_x of update area in original image 
	HI_S32 update_instart_h; //the start_y of update area in original image
	HI_S32 update_in_width;  //the width of update area in original image
	HI_S32 update_in_height;  //the height of update area in original image 
}UpdateConfig;

typedef struct tagUpdateInfo
{
    HI_S32 zme_instart_w;  //the start_x of needed readin area in original image 
	HI_S32 zme_instart_h;  //the start_y of needed readin area in original image
	HI_S32 zme_in_width;   //the width of needed readin area in original image
	HI_S32 zme_in_height;  //the height of needed readin area in original image

	HI_S32 zme_outstart_w; //the start_x of needed update area in output image  
	HI_S32 zme_outstart_h; //the start_y of needed update area in output image
	HI_S32 zme_out_width;  //the width of needed update area in output image
	HI_S32 zme_out_height; //the height of needed update area in output image

	HI_S32 zme_hphase; //the start phase of horizontal scale 
	HI_S32 zme_vphase; //the start phase of vertical scale
	HI_S32 def_offsetup; //the up offset of deflicker
	HI_S32 def_offsetdown; //the down offset of deflicker
}UpdateInfo;

STATIC void TdeOsiGetHUpdateInfo(UpdateConfig *reg, UpdateInfo *info, int scaler_en)
{
	HI_S32 zme_hinstart=0, zme_hinstop=0;
	HI_S32 zme_houtstart=0, zme_houtstop=0;
	HI_S32 update_hstart=reg->update_instart_w;
	HI_S32 update_hstop=update_hstart+reg->update_in_width-1;
	HI_S32 zme_hphase=0;
    //HI_S32 ratio=(HI_S32)(4096*(reg->ori_in_width-1.0)/(reg->zme_out_width-1.0)+0.5);
    HI_S32 ratio=(HI_S32)(4096*(reg->ori_in_width-1)/(reg->zme_out_width-1)+1/2);
	HI_S32 dratio=4096*(reg->zme_out_width-1)/(reg->ori_in_width-1); 
	//TDE_ASSERT(update_hstart>=0 && update_hstop<reg->ori_in_width);
	TDE_TRACE(TDE_KERN_DEBUG, "update_start:%d, update_hstop:%d, ori_in_width:%d\n", \
	 update_hstart, update_hstop, reg->ori_in_width);

	if(1 != scaler_en)  /*hor_scaler not enable*/
    {
        info->zme_instart_w = reg->update_instart_w;
        info->zme_outstart_w = reg->update_instart_w;
        info->zme_in_width = reg->update_in_width;
        info->zme_out_width = reg->update_in_width;
        info->zme_hphase = 0;

        return;
    }

	//hor_scaler enable
	if(update_hstart>=0 && update_hstart<4) 
	{
        /*update outstretched area exceed left limit bordline*/
		zme_hinstart = 0;
		zme_houtstart = 0;
		zme_hphase = 0;
	}
	else  
	{
        /*update outstretched area didn't exceed the left limit bordline*/
		zme_hinstart = (update_hstart-4)*dratio;
		zme_houtstart = (zme_hinstart%4096)==0 ? (zme_hinstart>>12): ((zme_hinstart>>12)+1);
		zme_hinstart = zme_houtstart*ratio>>12;	
		if(zme_hinstart-3<0)  
		{
            /*the left few point need mirror pixels when scale*/
			zme_hphase = (zme_houtstart*ratio)%4096+zme_hinstart*4096;
			zme_hinstart = 0;
		}
		else 
		{
            /*the left few point not need mirror pixels when scale*/
            zme_hphase = (zme_houtstart*ratio)%4096+3*4096;
			zme_hinstart = zme_hinstart - 3;
		}	
	}

	if( update_hstop>(reg->ori_in_width-4) && update_hstop<reg->ori_in_width)
	{
        /*update outstretched area exceed the right limit bordline*/
		zme_hinstop = reg->ori_in_width - 1;
		zme_houtstop = reg->zme_out_width - 1;
	}
	else 
	{
        /*update outstretched area didn't exceed the right limit bordline*/
		zme_hinstop = (update_hstop+3+1)*dratio;
		zme_houtstop = (zme_hinstop%4096)==0 ? ((zme_hinstop>>12)-1): (zme_hinstop>>12);
		zme_hinstop = zme_houtstop*ratio>>12;
		if(zme_hinstop+4>(reg->ori_in_width-1)) 
		{
            /*the right few point need mirror pixels when scale*/
			zme_hinstop = reg->ori_in_width - 1;
		}
		else
		{
            /*the right few point need mirror pixels when scale*/
			zme_hinstop = zme_hinstop + 4;
		}
	}
				
	info->zme_instart_w = zme_hinstart;
	info->zme_outstart_w = zme_houtstart;
	info->zme_in_width = zme_hinstop - zme_hinstart +1;
	info->zme_out_width = zme_houtstop - zme_houtstart +1;
	info->zme_hphase = zme_hphase;

    return ;
}

STATIC void TdeOsiGetVUpdateInfo(UpdateConfig *reg, UpdateInfo *info, int scaler_en, int deflicker_en)
{
	HI_S32 zme_vinstart=0, zme_vinstop=0;
	HI_S32 zme_voutstart=0, zme_voutstop=0;
	HI_S32 update_vstart=reg->update_instart_h;
	HI_S32 update_vstop=update_vstart+reg->update_in_height-1;
	HI_S32 zme_vphase=0;
	//HI_S32 ratio=(HI_S32)(4096*(reg->ori_in_height-1.0)/(reg->zme_out_height-1.0)+0.5);
	HI_S32 ratio=(HI_S32)(4096*(reg->ori_in_height-1)/(reg->zme_out_height-1)+1/2);
	HI_S32 dratio=4096*(reg->zme_out_height-1)/(reg->ori_in_height-1);
	//TDE_ASSERT(update_vstart>=0 && update_vstop<reg->ori_in_height);
	TDE_TRACE(TDE_KERN_DEBUG, "update_vstart:%d, update_vstop:%d, ori_in_height:%d, ratio:%d£¬dratio:%d\n",\
	update_vstart, update_vstop, reg->ori_in_height, ratio, dratio);

	if(scaler_en==0 && deflicker_en==1)
	{
        /*ver_scale not enable & deflicker enable*/
		if(update_vstart<2)
		{
            /*the update outstreatched area exceed the up limit bordline*/
			zme_vinstart = 0;
			zme_voutstart = 0;
			info->def_offsetup = 0;
		}
		else
		{
            /*the update outstreatched area didn't exceed the up limit bordline*/
			zme_vinstart = update_vstart - 2;
			zme_voutstart = reg->update_instart_h -1;
			info->def_offsetup = 1;
		}
		
		if(update_vstop>(reg->ori_in_height-3))
		{
            /*the update outstreatched area exceed the down limit bordline*/
            zme_vinstop = reg->ori_in_height - 1;
			zme_voutstop = reg->ori_in_height - 1; 
			info->def_offsetdown = 0;
		}
		else
		{
            /*the update outstreatched area didn't exceed the down limit bordline*/
			zme_vinstop = update_vstop + 2;
			zme_voutstop = zme_vinstop - 1;
			info->def_offsetdown = 1;
		}

		info->zme_in_height = zme_vinstop - zme_vinstart +1;
		info->zme_instart_h = zme_vinstart;		
		info->zme_outstart_h = zme_voutstart;
		info->zme_out_height = zme_voutstop - zme_voutstart + 1;
		info->zme_vphase = 0;

	}
	else if(scaler_en==1 && deflicker_en==0)
	{
        /*ver_scale enable & deflicker not enable*/
		if(update_vstart>=0 && update_vstart<2)
		{
            /*the update outstreatched area exceed the up limit bordline*/
			zme_vinstart = 0;
			zme_voutstart = 0;
			zme_vphase = 0;
		}
		else 
		{
            /*the update outstreatched area didn't exceed the up limit bordline*/
            zme_vinstart = (update_vstart-2)*dratio;
			zme_voutstart = (zme_vinstart%4096)==0 ? (zme_vinstart>>12): ((zme_vinstart>>12)+1);
			zme_vinstart = zme_voutstart*ratio>>12;
			if(zme_vinstart-2<0)
			{
                /*the up few point need mirror pixels when scale*/
				zme_vphase = (zme_voutstart*ratio)%4096+zme_vinstart*4096; 
				zme_vinstart = 0;
			}
			else
			{
                /*the up few point not need mirror pixels when scale*/
				zme_vphase = (zme_voutstart*ratio)%4096+2*4096; 
				zme_vinstart = zme_vinstart - 2;
			}	
		}
		
		if(update_vstop>(reg->ori_in_height-3) && update_vstop<reg->ori_in_height)
		{
            /*the update outstreatched area exceed the down limit bordline*/
			zme_vinstop = reg->ori_in_height - 1;
			zme_voutstop = reg->zme_out_height - 1;
            TDE_TRACE(TDE_KERN_DEBUG, "update_vstop:%d zme_voutstop:%d\n", update_vstop, zme_voutstop);
		}
		else
		{
            /*the update outstreatched area didn't exceed the down limit bordline*/
			zme_vinstop = (update_vstop+2+1)*dratio;
            zme_voutstop = (zme_vinstop%4096)==0 ? (zme_vinstop>>12): (zme_vinstop>>12) + 1; 
            TDE_TRACE(TDE_KERN_DEBUG, "update_vstop:%d zme_voutstop:%d\n", update_vstop, zme_voutstop);
			zme_vinstop = zme_voutstop*ratio>>12;
			if(zme_vinstop+2>(reg->ori_in_height-1))
			{
                /*the down few point need mirror pixels when scale*/
				zme_vinstop = reg->ori_in_height - 1;
			}
			else
			{
                /*the down few point not need mirror pixels when scale*/
				zme_vinstop = zme_vinstop + 2;
			}
            if(zme_voutstop>=reg->zme_out_height)
            {
            //    zme_voutstop = reg->zme_out_height -1;
            }		
		}
				
		info->zme_in_height = zme_vinstop - zme_vinstart +1;
		info->zme_instart_h = zme_vinstart;
        TDE_TRACE(TDE_KERN_DEBUG, "zme_voutstart:%d, zme_voutstop:%d\n", zme_voutstart, zme_voutstop);
		info->zme_outstart_h = zme_voutstart;
		info->zme_out_height = zme_voutstop - zme_voutstart +1;
		info->zme_vphase = zme_vphase;
		info->def_offsetup = 0;
		info->def_offsetdown = 0;
	}
	else if(scaler_en==1 && deflicker_en==1)
	{
        /*ver_scale enable & deflicker enable*/
        if(update_vstart>=0 && update_vstart<=2)
		{
            /*the update outstreatched area exceed the up limit bordline*/
			zme_vinstart = 0;
			zme_voutstart = 0;
			zme_vphase = 0;
			info->def_offsetup = 0;
		}
		else 
		{
            /*the update outstreatched area didn't exceed the up limit bordline*/
            zme_vinstart = (update_vstart-2)*dratio;
			zme_voutstart = ((zme_vinstart%4096)==0 ? (zme_vinstart>>12): ((zme_vinstart>>12)+1)) - 1;
			if(zme_voutstart<=1)
			{
                /*the update outstreatched deflicker area exceed the up limit bordline*/
				zme_vinstart = 0;
				zme_vphase = 0;
				info->def_offsetup = (zme_voutstart==0)?0: 1;
			}
			else
			{
                /*the update outstreatched deflicker area didn't exceed the up limit bordline*/
				zme_vinstart = (zme_voutstart-1)*ratio>>12;
				if(zme_vinstart<2)
				{
                    /*the up few point need mirror pixels when scale*/
					zme_vphase = ( (zme_voutstart-1)*ratio )%4096+zme_vinstart*4096; 
					zme_vinstart = 0;
				}
				else
				{
                    /*the up few point not need mirror pixels when scale*/
					zme_vphase = ( (zme_voutstart-1)*ratio )%4096+2*4096; 
					zme_vinstart = zme_vinstart - 2;
				}
				info->def_offsetup = 1;
			}		
		}
 
		if(update_vstop>(reg->ori_in_height-3) && update_vstop<reg->ori_in_height)
		{
            /*the update outstreatched area exceed the down limit bordline*/
			zme_vinstop = reg->ori_in_height - 1;
			zme_voutstop = reg->zme_out_height - 1;
			info->def_offsetdown = 0;
		}
		else
		{
            /*the update outstreatched area didn't exceed the down limit bordline*/
			zme_vinstop = (update_vstop+2+1)*dratio;
            zme_voutstop = ((zme_vinstop%4096)==0 ? (zme_vinstop>>12): (zme_vinstop>>12)) + 2; 
		    if(zme_voutstop>(reg->zme_out_height-3))
			{
                /*the update outstreatched deflicker area exceed the down limit bordline*/
				zme_vinstop = reg->ori_in_height - 1;
				info->def_offsetdown = (zme_voutstop>=(reg->zme_out_height-1))?0: 1;
			}
			else
			{
				zme_vinstop = (zme_voutstop+1)*ratio>>12;
				zme_vinstop = (zme_vinstop>(reg->ori_in_height-3))?(reg->ori_in_height-1): (zme_vinstop + 2);
			    info->def_offsetdown = 1;
			}

            if(zme_voutstop>=reg->zme_out_height)
            {
                zme_voutstop = reg->zme_out_height -1;
            }	
		}

				
		info->zme_instart_h = zme_vinstart;		
		info->zme_outstart_h = zme_voutstart;
		info->zme_in_height = zme_vinstop - zme_vinstart +1;
		info->zme_out_height = zme_voutstop - zme_voutstart +1;
		info->zme_vphase = zme_vphase;
	}
	else
	{
		info->zme_instart_h = reg->update_instart_h;
		info->zme_in_height = reg->update_in_height;
		info->zme_outstart_h = reg->update_instart_h;
		info->zme_out_height = reg->update_in_height;
		info->zme_vphase = 0;
		info->def_offsetup = 0;
		info->def_offsetdown = 0;
	}
	
}


/*****************************************************************************
* Function:      TdeOsiSetFilterNode
* Description:   do deflicker(include VF/HF/FF)
* Input:         s32Handle:Job head node pointer of needing operate 
*                pNode: set parent node fliter parameter information
*                pInSurface: input bitmap information
*                pOutSurface: output bitmap information
*                bDeflicker: if deflicker
*                pstOpt: config option of filter operate needings
* Output:        none
* Return:        none
* Others:        none
*****************************************************************************/
STATIC HI_S32 TdeOsiSetFilterNode(TDE_HANDLE s32Handle, TDE_HWNode_S* pNode, TDE2_SURFACE_S *pstForeGround, TDE2_SURFACE_S *pstDst, 
                                  TDE2_RECT_S* pInRect, TDE2_RECT_S*  pOutRect, TDE2_DEFLICKER_MODE_E enDeflickerMode, TDE_FILTER_OPT *pstFilterOpt)
{
    TDE2_COLOR_FMT_E enInFmt;
    TDE2_COLOR_FMT_E enOutFmt;

    UpdateConfig reg;
    UpdateInfo info;

    TDE2_RECT_S stUpdateInRect;
    TDE2_RECT_S stUpdateOutRect;

    HI_BOOL bDeflicker;
    HI_BOOL bScale = HI_FALSE;
    
    enInFmt = pstForeGround->enColorFmt;
    enOutFmt = pstDst->enColorFmt;
    bDeflicker = (TDE2_DEFLICKER_MODE_NONE == enDeflickerMode)?HI_FALSE:HI_TRUE;
    
    /* calculate vertical/horizontal stride and offset */
    if (1 >= pOutRect->u32Width)
    {
        pstFilterOpt->u32HStep = 0;
    }
    else
    {
        pstFilterOpt->u32HStep = ((pInRect->u32Width - 1) << TDE_FLOAT_BITLEN) / (pOutRect->u32Width - 1);
    }

    if (1 >= pOutRect->u32Height)
    {
        pstFilterOpt->u32VStep = 0;
    }
    else
    {
        pstFilterOpt->u32VStep = ((pInRect->u32Height - 1) << TDE_FLOAT_BITLEN) / (pOutRect->u32Height - 1);
    }

    pstFilterOpt->s32HOffset = 0;
    pstFilterOpt->s32VOffset = 0;
    pstFilterOpt->stAdjInfo.enScaleMode = TDE_CHILD_SCALE_NORM;
    pstFilterOpt->u32Bppi = TdeOsiGetbppByFmt(enInFmt);
    if (TDE2_COLOR_FMT_YCbCr422 == enInFmt || TDE2_COLOR_FMT_YCbCr422 == enOutFmt)
    {
        pstFilterOpt->bEvenStartInX = HI_TRUE; /* YCbCr422 ask for start width is even of each slice */
        pstFilterOpt->bBadLastPix = HI_TRUE;  /* YCbCr422 ask for conver 2 noneffective pixel points for each slice when blocking */
    }
    if (TDE2_COLOR_FMT_YCbCr422 == enOutFmt)
    {
        pstFilterOpt->bEvenStartOutX = HI_TRUE;
        pstFilterOpt->bBadLastPix = HI_TRUE;   /* YCbCr422 ask for conver 2 noneffective pixel points for each slice when blocking  */
    }


    /* judge middle value filter is enable, enable manner is judged by arithmetic team */
    if (TDE_NO_SCALE_STEP > pstFilterOpt->u32VStep)
    {
        pstFilterOpt->bVRing = HI_TRUE;
    }

    if (TDE_NO_SCALE_STEP > pstFilterOpt->u32HStep)
    {
        pstFilterOpt->bHRing = HI_TRUE;
    }

    pstFilterOpt->bCoefSym = HI_TRUE; /* arithmetic suply field, keeping open asymmetry */

    reg.ori_in_height = pstForeGround->u32Height;
    reg.ori_in_width = pstForeGround->u32Width;
    reg.zme_out_height = pstDst->u32Height;
    reg.zme_out_width = pstDst->u32Width;
    reg.update_instart_w = pInRect->s32Xpos;
    reg.update_instart_h = pInRect->s32Ypos;
    reg.update_in_width = pInRect->u32Width;
    reg.update_in_height = pInRect->u32Height;
    
    TDE_TRACE(TDE_KERN_DEBUG, "InHeight:%d, InWidth:%d, OutHeight:%d, OutWidth:%d,UpdateX:%d, UpdateY:%d,\
        UpdateW:%d, UpdateH:%d\n", reg.ori_in_height, reg.ori_in_width, reg.zme_out_height, reg.zme_out_width,\
        reg.update_instart_w, reg.update_instart_h, reg.update_in_width, reg.update_in_height);
    /* local deflicker */
    if (((pstForeGround->u32Width != pInRect->u32Width) || (pstForeGround->u32Height != pInRect->u32Height))
        && s_bRegionDeflicker)
    {
        if (1 >= pstDst->u32Width)
        {
            pstFilterOpt->u32HStep = 0;
        }
        else
        {
            pstFilterOpt->u32HStep = ((pstForeGround->u32Width - 1) << TDE_FLOAT_BITLEN) / (pstDst->u32Width - 1);
        }

        if (1 >= pstDst->u32Height)
        {
            pstFilterOpt->u32VStep = 0;
        }
        else
        {
            pstFilterOpt->u32VStep = ((pstForeGround->u32Height - 1) << TDE_FLOAT_BITLEN) / (pstDst->u32Height - 1);
        }

        if (TDE_NO_SCALE_STEP > pstFilterOpt->u32VStep)
        {
            pstFilterOpt->bVRing = HI_TRUE;
        }

        if (TDE_NO_SCALE_STEP > pstFilterOpt->u32HStep)
        {
            pstFilterOpt->bHRing = HI_TRUE;
        }

        if (TDE_NO_SCALE_STEP != pstFilterOpt->u32HStep)
        {
            bScale = HI_TRUE;
        }
        
        TDE_TRACE(TDE_KERN_DEBUG, "bHScale:%x\n", bScale);
        TdeOsiGetHUpdateInfo(&reg, &info, bScale);

        bScale = HI_FALSE;
        if (TDE_NO_SCALE_STEP != pstFilterOpt->u32VStep)
        {
            bScale = HI_TRUE;
        }
        TDE_TRACE(TDE_KERN_DEBUG, "bVScale:%x\n", bScale);
        TdeOsiGetVUpdateInfo(&reg, &info, bScale, bDeflicker);
        TDE_TRACE(TDE_KERN_DEBUG, "zme_instart_w:%d, zme_instart_h:%d, zme_in_width:%d, \
            zme_in_height:%d, zme_outstart_w:%d, zme_outstart_h:%d, zme_out_width:%d, \
            zme_out_height:%d, zme_hphase:%d, zme_vphase:%d, def_offsetup:%d, def_offsetdown:%d\n",
            info.zme_instart_w, info.zme_instart_h, info.zme_in_width, info.zme_in_height, \
            info.zme_outstart_w, info.zme_outstart_h, info.zme_out_width, info.zme_out_height, \
            info.zme_hphase, info.zme_vphase, info.def_offsetup, info.def_offsetdown);

        stUpdateInRect.s32Xpos = info.zme_instart_w;
        stUpdateInRect.s32Ypos = info.zme_instart_h;
        stUpdateInRect.u32Width = info.zme_in_width;
        stUpdateInRect.u32Height = info.zme_in_height;

        stUpdateOutRect.s32Xpos = info.zme_outstart_w;
        stUpdateOutRect.s32Ypos = info.zme_outstart_h;
        stUpdateOutRect.u32Width = info.zme_out_width;
        stUpdateOutRect.u32Height = info.zme_out_height;

        pstFilterOpt->s32HOffset = info.zme_hphase;
        pstFilterOpt->s32VOffset = info.zme_vphase;
        pstFilterOpt->bFirstLineOut = (info.def_offsetup & 0x1);
        pstFilterOpt->bLastLineOut = (info.def_offsetdown & 0x1);

        return TdeOsiSetFilterChildNode(s32Handle, pNode, &stUpdateInRect, &stUpdateOutRect, enDeflickerMode, pstFilterOpt);
    }
    else
    {
        return TdeOsiSetFilterChildNode(s32Handle, pNode, pInRect, pOutRect, enDeflickerMode, pstFilterOpt);
    }
    
}

/*****************************************************************************
* Function:      TdeOsiCalcMbFilterOpt
* Description:   calculate filter coefficient when mb blit
* Input:         pstFilterOpt: filter coefficient struct
*                enInFmt: input bitmap format info
*                pInRect: input bitmap operate zone
*                pOutRect: output bitmap operate zone
*                bCbCr: if chroma operate
*                bCus:  if sample in chroma
*                enPicMode: bitmap operate  mode: frame/ top filed/ bottom field
* Output:        none
* Return:        none
* Others:        none
*****************************************************************************/
STATIC INLINE HI_VOID TdeOsiCalcMbFilterOpt(TDE_FILTER_OPT* pstFilterOpt, TDE2_MB_COLOR_FMT_E enInFmt, TDE2_MB_COLOR_FMT_E enOutFmt,
                                            TDE2_RECT_S* pInRect, TDE2_RECT_S*  pOutRect, HI_BOOL bCbCr,
                                            HI_BOOL bCus, TDE_PIC_MODE_E enPicMode)
{
    //(HI_VOID)memset((HI_VOID*)pstFilterOpt, 0, sizeof(TDE_FILTER_OPT));

    pstFilterOpt->enFilterMode = TDE_DRV_FILTER_ALL;

    /* calculate vertiacal/ horizontal stride and offset, brightness step calculate need its width and height */
    if (bCbCr && bCus)    /* chroma sample need calculate step by indepedant */
    {
        switch (enInFmt)
        {
        case TDE2_MB_COLOR_FMT_JPG_YCbCr422MBHP:
            pstFilterOpt->u32HStep = (1 << (TDE_FLOAT_BITLEN - 1));    /* 0.5 */
            pstFilterOpt->u32VStep = TDE_NO_SCALE_STEP;    /* 1 */
            break;
        case TDE2_MB_COLOR_FMT_JPG_YCbCr422MBVP:
            pstFilterOpt->u32HStep = TDE_NO_SCALE_STEP;    /* 1 */
            pstFilterOpt->u32VStep = (1 << (TDE_FLOAT_BITLEN - 1));    /* 0.5 */
            break;
        case TDE2_MB_COLOR_FMT_MP1_YCbCr420MBP:
        case TDE2_MB_COLOR_FMT_MP2_YCbCr420MBP:
        case TDE2_MB_COLOR_FMT_MP2_YCbCr420MBI:
        case TDE2_MB_COLOR_FMT_JPG_YCbCr420MBP:
            pstFilterOpt->u32HStep = (1 << (TDE_FLOAT_BITLEN - 1));    /* 0.5 */
            pstFilterOpt->u32VStep = (1 << (TDE_FLOAT_BITLEN - 1));    /* 0.5 */
            break;
        default:
            pstFilterOpt->u32HStep = TDE_NO_SCALE_STEP;    /* 1 */
            pstFilterOpt->u32VStep = TDE_NO_SCALE_STEP;    /* 1 */
            break;
        }
    }
    else
    {
        if (pstFilterOpt->b2OptCbCr)
        {
            pstFilterOpt->u32HStep = TdeOsiMbCalcHStep(pInRect->u32Width, pOutRect->u32Width, enInFmt, enOutFmt, HI_FALSE);
            pstFilterOpt->u32VStep = TdeOsiMbCalcVStep(pInRect->u32Height, pOutRect->u32Height, enInFmt, enOutFmt, HI_FALSE);
        }
        else
        {
            pstFilterOpt->u32HStep = TdeOsiMbCalcHStep(pInRect->u32Width, pOutRect->u32Width, enInFmt, enOutFmt, bCbCr);
            pstFilterOpt->u32VStep = TdeOsiMbCalcVStep(pInRect->u32Height, pOutRect->u32Height, enInFmt, enOutFmt, bCbCr);
        }
    }

    if (TDE2_MB_COLOR_FMT_JPG_YCbCr444MBP == enOutFmt && TDE2_MB_COLOR_FMT_JPG_YCbCr444MBP != enInFmt)
    {
    pstFilterOpt->s32HOffset = TdeOsiMbCalcHOffset(pInRect->s32Xpos, enInFmt, bCbCr);
    pstFilterOpt->s32VOffset = TdeOsiMbCalcVOffset(pInRect->s32Ypos, enInFmt, enPicMode, bCbCr);
    }
    else if (TDE2_MB_COLOR_FMT_JPG_YCbCr444MBP != enOutFmt && TDE2_MB_COLOR_FMT_JPG_YCbCr444MBP != enInFmt
             && enOutFmt == enInFmt)
    {
        pstFilterOpt->s32HOffset = 0;
        pstFilterOpt->s32VOffset = (TDE_BOTTOM_FIELD_PIC_MODE == enPicMode) ? 
                                   /*            step / 2         -  0.5  */
                                   ((pstFilterOpt->u32VStep >> 1) - (1 << (TDE_FLOAT_BITLEN - 1))) : (0);
    }
    else
    {
        /* other situation do not handle, calculate by 0 */
        pstFilterOpt->s32HOffset = 0;
        pstFilterOpt->s32VOffset = 0;
    }
#if 0
    if (bCus && (TDE_FRAME_PIC_MODE == enPicMode) && (1 == (pInRect->s32Ypos & 1)))
    {
        pstFilterOpt->s32VOffset += (pstFilterOpt->u32VStep >> 1);
    }
#endif
    /* MB format chroma Bpp is half word, brightness Bpp is the width of Byte */
    if (bCbCr)
    {
        pstFilterOpt->u32Bppi = 16;

        /* judge if median filter is enable, enable mannar is supplied by arithmetic team */
        if (TDE_NO_SCALE_STEP > pstFilterOpt->u32VStep)
        {
            pstFilterOpt->bVRing = HI_TRUE;
        }

        if (TDE_NO_SCALE_STEP > pstFilterOpt->u32HStep)
        {
            pstFilterOpt->bHRing = HI_TRUE;
        }
    }
    else
    {
        pstFilterOpt->u32Bppi = 8;

        /*  judge if median filter is enable, enable mannar is supplied by arithmetic team  */
        if (TDE_NO_SCALE_STEP > pstFilterOpt->u32HStep)
        {
            pstFilterOpt->bHRing = HI_TRUE;
        }
    }
    
    pstFilterOpt->bCoefSym = HI_TRUE; 
    pstFilterOpt->enFilterMode = TDE_DRV_FILTER_COLOR;
    TDE_TRACE(TDE_KERN_DEBUG, "\n");
}

/*****************************************************************************
* Function:      TdeOsiAdjPara4YCbCr422R
* Description:   when fill color is YCbCr422R, fill by word
* Input:         pstDst: target bitmap info struct
*                pstDstRect: target operate zone
*                pstFillColor: fill color
*                
* Output:        none
* Return:        none
* Others:        AI7D02880 add software fill, when YCbCr422, fill function by word
*****************************************************************************/
STATIC INLINE HI_VOID TdeOsiAdjPara4YCbCr422R(TDE2_SURFACE_S *pstDst, TDE2_RECT_S *pstDstRect, TDE2_FILLCOLOR_S *pstFillColor)
{
    if (TDE2_COLOR_FMT_YCbCr422 != pstDst->enColorFmt)
    {
        return;
    }

    
    pstDst->enColorFmt = TDE2_COLOR_FMT_AYCbCr8888;
    pstDst->bAlphaMax255 = HI_TRUE;
    pstFillColor->enColorFmt = TDE2_COLOR_FMT_AYCbCr8888;

    
    pstDst->u32Width /= 2;

   
    pstDstRect->u32Width /= 2;
    pstDstRect->s32Xpos /= 2;

    
    pstFillColor->u32FillColor = TDE_GET_YC422R_FILLVALUE(pstFillColor->u32FillColor);
}

/*****************************************************************************
* Function:      TdeOsi1SourceFill
* Description:   single source fill operate,source1 is fill color,target bitmap is pstDst,support source1 and fill color do ROP or alpha blending to target bitmap, unsupport mirror,colorkey
*                if src bitmap is MB, only support single source operate, just is set pstBackGround or pstForeGround
* Input:         s32Handle: task handle
*                pSrc: background bitmap info struct
*                pstDst: foreground bitmap info struct
*                pstFillColor:  target bitmap info struct
*                pstOpt: operate parameter setting struct
* Output:        none
* Return:        HI_SUCCESS/HI_FAILURE
* Others:        none
*****************************************************************************/
STATIC HI_S32 TdeOsi1SourceFill(TDE_HANDLE s32Handle, TDE2_SURFACE_S *pstDst,
                                TDE2_RECT_S  *pstDstRect, TDE2_FILLCOLOR_S *pstFillColor, TDE2_OPT_S *pstOpt)
{
    TDE_DRV_BASEOPT_MODE_E enBaseMode = {0};
    TDE_DRV_ALU_MODE_E enAluMode = {0};
    TDE_HWNode_S stHWNode = {0};
    TDE_DRV_SURFACE_S stDrvSurface = {0};
    TDE_DRV_COLORFILL_S stDrvColorFill = {0};
    TDE_SCANDIRECTION_S stScanInfo = {0};
    TDE_DRV_OUTALPHA_FROM_E enOutAlphaFrom = TDE2_OUTALPHA_FROM_NORM;
    TDE2_RECT_S stDstRect = {0};    

    if ((HI_NULL == pstDst) || (HI_NULL == pstDstRect) || (HI_NULL == pstFillColor))
    {
        TDE_TRACE(TDE_KERN_INFO, "NULL pointer!\n");
        return HI_ERR_TDE_NULL_PTR;
    }

    memcpy(&stDstRect, pstDstRect, sizeof(TDE2_RECT_S));
    
    TDE_CHECK_NOT_MB(pstDst->enColorFmt);
    TDE_CHECK_NOT_MB(pstFillColor->enColorFmt);

    if (TdeOsiCheckSurface(pstDst, &stDstRect) < 0)
    {
        return HI_ERR_TDE_INVALID_PARA;
    }

   
    TDE_CHECK_SUBBYTE_STARTX(stDstRect.s32Xpos, stDstRect.u32Width, pstDst->enColorFmt);

    TdeOsiAdjPara4YCbCr422R(pstDst, &stDstRect, pstFillColor);

    
    TdeHalNodeInitNd(&stHWNode, HI_FALSE);

    //TdeOsiSetExtAlpha(pstDst, NULL, &stHWNode); /* AI7D02681 */

    enAluMode = TDE_ALU_NONE;

    if (HI_NULL == pstOpt)
    {
        if (pstFillColor->enColorFmt == pstDst->enColorFmt)
        {
            enBaseMode = TDE_QUIKE_FILL;
            enOutAlphaFrom = TDE2_OUTALPHA_FROM_NORM;
        }
        else
        {
            enBaseMode = TDE_NORM_FILL_1OPT;
            enOutAlphaFrom = TDE2_OUTALPHA_FROM_NORM;
        }
    }
    else
    {
        TDE_CHECK_ALUCMD(pstOpt->enAluCmd);
        enBaseMode = TDE_NORM_FILL_1OPT;
        if (pstOpt->enAluCmd & TDE2_ALUCMD_ROP)
        {        
            TDE_CHECK_ROPCODE(pstOpt->enRopCode_Color);
            TDE_CHECK_ROPCODE(pstOpt->enRopCode_Alpha);
            enAluMode = TDE_ALU_ROP;

            
            TdeHalNodeSetRop(&stHWNode, pstOpt->enRopCode_Color, pstOpt->enRopCode_Alpha);
        }

        if (pstOpt->enAluCmd & TDE2_ALUCMD_BLEND)
        {
            TDE_CHECK_BLENDCMD(pstOpt->stBlendOpt.eBlendCmd);
	     if (TDE2_BLENDCMD_CONFIG == pstOpt->stBlendOpt.eBlendCmd)
	     {
	          TDE_CHECK_BLENDMODE(pstOpt->stBlendOpt.eSrc1BlendMode);
		   TDE_CHECK_BLENDMODE(pstOpt->stBlendOpt.eSrc2BlendMode);	  
	     }
            enAluMode = TDE_ALU_BLEND;

#if 0
            if (!pstOpt->stBlendOpt.bSrc2AlphaPremulti)
            {
                enAluMode = TDE_ALU_BLEND_SRC2;
            }
#else
            TdeHalNodeSetBlend(&stHWNode, &pstOpt->stBlendOpt);
#endif

            if (pstOpt->enAluCmd & TDE2_ALUCMD_ROP)
            {
               
                TdeHalNodeEnableAlphaRop(&stHWNode);
            }
        }

        if (pstOpt->enAluCmd & TDE2_ALUCMD_COLORIZE)
        {
            TdeHalNodeSetColorize(&stHWNode, pstOpt->u32Colorize);
        }
        
        if(TDE2_ALUCMD_NONE == pstOpt->enAluCmd)
        {
            if (pstFillColor->enColorFmt == pstDst->enColorFmt)
            {
                enBaseMode = TDE_QUIKE_FILL;

            }
            else
            {
                enBaseMode = TDE_NORM_FILL_1OPT;
            }
        }
        else
        {
            TDE_TRACE(TDE_KERN_INFO, "invalid alu command!\n");
            return HI_ERR_TDE_INVALID_PARA;
        }

        enOutAlphaFrom = pstOpt->enOutAlphaFrom;
        TDE_CHECK_OUTALPHAFROM(enOutAlphaFrom);
        if(TDE2_OUTALPHA_FROM_FOREGROUND == enOutAlphaFrom)
        {
            TDE_TRACE(TDE_KERN_INFO, "enOutAlphaFrom error!\n");
            return HI_ERR_TDE_INVALID_PARA;
            
        }

        
        TdeHalNodeSetGlobalAlpha(&stHWNode, pstOpt->u8GlobalAlpha, pstOpt->stBlendOpt.bGlobalAlphaEnable);
        
        
        if(TdeOsiSetClipPara(NULL, NULL, pstDst, &stDstRect, pstDst, &stDstRect, pstOpt, &stHWNode) < 0)
        {
            return HI_ERR_TDE_CLIP_AREA;
        }
    }

    
    if (TDE_NORM_FILL_1OPT == enBaseMode)
    {
        if(TdeOsiColorConvert(pstFillColor, pstDst, &stDrvColorFill.u32FillData) < 0)
        {
            return HI_ERR_TDE_INVALID_PARA;
        }
	
	stDrvColorFill.enDrvColorFmt = TDE_DRV_COLOR_FMT_ARGB8888;
    }
    else
    {
        stDrvColorFill.u32FillData = pstFillColor->u32FillColor;
	
	stDrvColorFill.enDrvColorFmt = g_enTdeCommonDrvColorFmt[pstFillColor->enColorFmt];

    }


   
    TdeHalNodeSetBaseOperate(&stHWNode, enBaseMode, enAluMode, &stDrvColorFill);

  
    stScanInfo.enHScan = TDE_SCAN_LEFT_RIGHT;
    stScanInfo.enVScan = TDE_SCAN_UP_DOWN;

    
    TdeOsiConvertSurface(pstDst, &stDstRect, &stScanInfo, &stDrvSurface, NULL);

    if (TDE_NORM_FILL_1OPT == enBaseMode)
    {
        
        TdeHalNodeSetSrc1(&stHWNode, &stDrvSurface);
    }

    
    TdeHalNodeSetTgt(&stHWNode, &stDrvSurface, enOutAlphaFrom);

    TdeOsiSetExtAlpha(pstDst, NULL, &stHWNode); /* AI7D02681 */
    
    return TdeOsiSetNodeFinish(s32Handle, &stHWNode, 0, TDE_NODE_SUBM_ALONE);
}

/*****************************************************************************
* Function:      TdeOsi2SourceFill
* Description:   double sources fill operate, source1 is fill color, source2 is pstSrc,bitmap is pstDst,after source2 zoom or deflicker,
*                Rop with fill color or alpha blending to target bitmap, unsupport mirror,colorkey
*                fi src bitmap is mb format, only support single source mode, just set pstBackGround or pstForeGround
* Input:         s32Handle: task handle 
*                pSrc: background bitmap info struct
*                pstDst: foreground bitmap info struct
*                pstFillColor:  target bitmap info struct
*                pstOpt:  operate parameter setting struct
* Output:        none
* Return:        HI_SUCCESS/HI_FAILURE
* Others:        none
*****************************************************************************/
STATIC HI_S32 TdeOsi2SourceFill(TDE_HANDLE s32Handle, TDE2_SURFACE_S* pstSrc, TDE2_RECT_S  *pstSrcRect,
                                TDE2_SURFACE_S *pstDst,
                                TDE2_RECT_S  *pstDstRect, TDE2_FILLCOLOR_S *pstFillColor,
                                TDE2_OPT_S *pstOpt)
{
    TDE_DRV_BASEOPT_MODE_E enBaseMode = TDE_QUIKE_FILL;
    TDE_DRV_ALU_MODE_E enAluMode = TDE_SRC1_BYPASS;
    TDE_HWNode_S stHWNode = {0};
    TDE_DRV_SURFACE_S stDrvSurface = {0};
    TDE_DRV_COLORFILL_S stDrvColorFill = {0};
    TDE_SCANDIRECTION_S stSrcScanInfo = {0};
    TDE_SCANDIRECTION_S stDstScanInfo = {0};
    TDE2_RECT_S stSrcOptArea = {0};
    TDE2_RECT_S stDstOptArea = {0};
    TDE_FILTER_OPT stFilterOpt = {0};
    TDE_CLUT_USAGE_E enClutUsage = TDE_CLUT_USAGE_BUTT;
    TDE2_RECT_S stSrcRect = {0};
    TDE2_RECT_S stDstRect = {0};
    HI_S32 s32Ret = HI_FAILURE;

    if ((NULL == pstDst) || (NULL == pstDstRect) || (NULL == pstFillColor) || (NULL == pstOpt)
        || (NULL == pstSrc) || (NULL == pstSrcRect))
    {
        return HI_ERR_TDE_NULL_PTR;
    }

    memcpy(&stSrcRect, pstSrcRect, sizeof(TDE2_RECT_S));
    memcpy(&stDstRect, pstDstRect, sizeof(TDE2_RECT_S));

    
    TDE_CHECK_COLORFMT(pstFillColor->enColorFmt);

   
    TDE_CHECK_OUTALPHAFROM(pstOpt->enOutAlphaFrom);

   
    TDE_CHECK_ALUCMD(pstOpt->enAluCmd);
    
    if (TdeOsiCheckSurface(pstDst, &stDstRect) < 0)
    {
        return HI_ERR_TDE_INVALID_PARA;
    }

    if (TdeOsiCheckSurface(pstSrc, &stSrcRect) < 0)
    {
        return HI_ERR_TDE_INVALID_PARA;
    }

    if (!pstOpt->bResize)
    {
        TDE_UNIFY_RECT(&stSrcRect, &stDstRect);
    }

    TdeHalNodeInitNd(&stHWNode, HI_FALSE);

    enBaseMode = TDE_NORM_FILL_2OPT;
    enAluMode = TDE_ALU_NONE;

    if (pstOpt->enAluCmd & TDE2_ALUCMD_ROP)
    {        
        TDE_CHECK_ROPCODE(pstOpt->enRopCode_Color);
        TDE_CHECK_ROPCODE(pstOpt->enRopCode_Alpha);
        
        enAluMode = TDE_ALU_ROP;

       
        TdeHalNodeSetRop(&stHWNode, pstOpt->enRopCode_Color, pstOpt->enRopCode_Alpha);
    }
    
    if (pstOpt->enAluCmd & TDE2_ALUCMD_BLEND)
    {
        TDE_CHECK_BLENDCMD(pstOpt->stBlendOpt.eBlendCmd);
        if (pstOpt->stBlendOpt.eBlendCmd == TDE2_BLENDCMD_CONFIG)
        {
             TDE_CHECK_BLENDMODE(pstOpt->stBlendOpt.eSrc1BlendMode);
	      TDE_CHECK_BLENDMODE(pstOpt->stBlendOpt.eSrc2BlendMode);	  
         }
        enAluMode = TDE_ALU_BLEND;

#if 0
        if (!pstOpt->stBlendOpt.bSrc2AlphaPremulti)
        {
            enAluMode = TDE_ALU_BLEND_SRC2;
        }
#else
        TdeHalNodeSetBlend(&stHWNode, &pstOpt->stBlendOpt);
#endif

        if (pstOpt->enAluCmd & TDE2_ALUCMD_ROP)
        {
           
            TdeHalNodeEnableAlphaRop(&stHWNode);
        }
    }
    
    if (pstOpt->enAluCmd & TDE2_ALUCMD_COLORIZE)
    {
        TdeHalNodeSetColorize(&stHWNode, pstOpt->u32Colorize);
    }

    
    TdeHalNodeSetGlobalAlpha(&stHWNode, pstOpt->u8GlobalAlpha, pstOpt->stBlendOpt.bGlobalAlphaEnable);

    
    if(TdeOsiSetClipPara(NULL, NULL, pstSrc, &stSrcRect, pstDst, &stDstRect, pstOpt, &stHWNode) < 0)
    {
        return HI_ERR_TDE_CLIP_AREA;
    }
    
    if(TdeOsiColorConvert(pstFillColor, pstSrc, &stDrvColorFill.u32FillData) < 0)
    {
        return HI_ERR_TDE_INVALID_PARA;
    }

    stDrvColorFill.enDrvColorFmt = TDE_DRV_COLOR_FMT_ARGB8888;//TDE2_COLOR_FMT_ARGB8888;

    
    TdeHalNodeSetBaseOperate(&stHWNode, enBaseMode, enAluMode, &stDrvColorFill);
    TdeOsiSetExtAlpha(NULL, pstSrc, &stHWNode);
    
    
    if (TdeOsiGetScanInfo(pstSrc, &stSrcRect, pstDst, &stDstRect, pstOpt, &stSrcScanInfo,
                          &stDstScanInfo) < 0)
    {
        return HI_ERR_TDE_INVALID_PARA;
    }

   
    TdeOsiConvertSurface(pstSrc, &stSrcRect, &stSrcScanInfo, &stDrvSurface, &stSrcOptArea);

    
    TdeHalNodeSetSrc1(&stHWNode, &stDrvSurface);

    
    TdeOsiConvertSurface(pstDst, &stDstRect, &stDstScanInfo, &stDrvSurface, &stDstOptArea);

  
    TdeHalNodeSetTgt(&stHWNode, &stDrvSurface, pstOpt->enOutAlphaFrom);

   
    if((s32Ret = TdeOsiSetClutOpt(pstSrc, pstDst, &enClutUsage, pstOpt->bClutReload, &stHWNode)) < 0)
    {
        return s32Ret;
    }

   
    if (TDE2_COLORKEY_MODE_FOREGROUND == pstOpt->enColorKeyMode)
    {
        TDE_DRV_COLORKEY_CMD_S stColorkey;
        TDE_COLORFMT_CATEGORY_E enFmtCategory;
        stColorkey.unColorKeyValue = pstOpt->unColorKeyValue;

        if (TDE_CLUT_COLOREXPENDING != enClutUsage
            && TDE_CLUT_CLUT_BYPASS != enClutUsage)
        {
           
            stColorkey.enColorKeyMode = TDE_DRV_COLORKEY_FOREGROUND_AFTER_CLUT;
        }
        else
        {
            stColorkey.enColorKeyMode = TDE_DRV_COLORKEY_FOREGROUND_BEFORE_CLUT;
        }

       
        enFmtCategory = TdeOsiGetFmtCategory(pstSrc->enColorFmt);

        
        TdeHalNodeSetColorKey(&stHWNode, enFmtCategory, &stColorkey);
    }
    else if (TDE2_COLORKEY_MODE_BACKGROUND == pstOpt->enColorKeyMode)
    {
        TDE_TRACE(TDE_KERN_INFO, "Unsupported solidraw colorkey in background mode!\n");
        return HI_ERR_TDE_INVALID_PARA;
    }

    if ((pstOpt->bResize) || (pstOpt->enDeflickerMode != TDE2_DEFLICKER_MODE_NONE))
    {
        
        stFilterOpt.u32WorkBufNum = 0;

       
        memcpy(&stFilterOpt.stSrcDire, &stSrcScanInfo, sizeof(TDE_SCANDIRECTION_S));
        memcpy(&stFilterOpt.stDstDire, &stDstScanInfo, sizeof(TDE_SCANDIRECTION_S));

        stFilterOpt.enFilterMode = g_enTdeFilterMode[pstOpt->enFilterMode];

       
        return TdeOsiSetFilterNode(s32Handle, &stHWNode, pstSrc, pstDst,
                                   &stSrcOptArea, &stDstOptArea, pstOpt->enDeflickerMode, &stFilterOpt);
    }
    else
    {
        
        return TdeOsiSetNodeFinish(s32Handle, &stHWNode, 0, TDE_NODE_SUBM_ALONE);
    }
}

/*****************************************************************************
* Function:      TdeOsiCheckYc422RPara
* Description:   check by uniform, for Yc422R has many limits
* Input:         pstSurface: bitmap info
*                pstRect: bitmap operate zone
* Output:        none
* Return:        success/fail
* Others:        none
*****************************************************************************/
STATIC INLINE HI_S32 TdeOsiCheckYc422RPara(TDE2_SURFACE_S* pstSrcSurface, TDE2_RECT_S *pstSrcRect, 
                                           TDE2_SURFACE_S* pstDstSurface, TDE2_RECT_S *pstDstRect, HI_BOOL bDeflicker, HI_BOOL bResize)
{
    
    if(TDE2_COLOR_FMT_YCbCr422 == pstSrcSurface->enColorFmt 
       && TDE2_COLOR_FMT_YCbCr422 == pstDstSurface->enColorFmt)
    {
        
        if (bDeflicker || (bResize && pstSrcRect->u32Height != pstDstRect->u32Height))
        {
            return -1;
        }
    }
    
    else if (TDE2_COLOR_FMT_YCbCr422 != pstSrcSurface->enColorFmt 
             && TDE2_COLOR_FMT_YCbCr422 == pstDstSurface->enColorFmt)
    {
        
        if ( (bDeflicker && bResize && pstSrcRect->u32Width != pstDstRect->u32Width)
             || (bResize && pstSrcRect->u32Width != pstDstRect->u32Width && pstSrcRect->u32Height != pstDstRect->u32Height))
        {
            return -1;
        }
    }
    

    return 0;
}

/*****************************************************************************
* Function:      TdeOsiCheckSurface
* Description:   adjust right operate zone, according by the size of bitmap and operate zone from user upload 
* Input:         pstSurface: bitmap info
*                pstRect: bitmap operate zone
* Output:        none
* Return:        success/fail
* Others:        none
*****************************************************************************/
STATIC INLINE HI_S32 TdeOsiCheckSurface(TDE2_SURFACE_S* pstSurface, TDE2_RECT_S  *pstRect)
{
    HI_S32 s32Bpp;
    HI_U32 u32BytePerPixel;

    TDE_CHECK_COLORFMT(pstSurface->enColorFmt);

    if ((TDE_MAX_SURFACE_PITCH < pstSurface->u32Stride)
        || (0 == pstSurface->u32Stride)
        || (0 == pstRect->u32Height)
        || (0 == pstRect->u32Width)
        || (pstRect->s32Xpos < 0)
        || ((HI_U32)pstRect->s32Xpos >= pstSurface->u32Width)
        || (pstRect->s32Ypos < 0)
        || ((HI_U32)pstRect->s32Ypos >= pstSurface->u32Height))
    {
        TDE_TRACE(TDE_KERN_INFO, "sw:%d, sh:%d, stride:%x, x:%d, y:%d, w:%d, h:%d\n", pstSurface->u32Width,
            pstSurface->u32Height, pstSurface->u32Stride, pstRect->s32Xpos, pstRect->s32Ypos,
            pstRect->u32Width, pstRect->u32Height);
        TDE_TRACE(TDE_KERN_INFO, "invalid surface size or operation area!");
        return -1;
    }

    if (TDE2_COLOR_FMT_BUTT <= pstSurface->enColorFmt)
    {
        TDE_TRACE(TDE_KERN_INFO, "Unkown color format!\n");
        return -1;
    }

    if ((TDE2_COLOR_FMT_YCbCr422 == pstSurface->enColorFmt)
        && ((pstRect->s32Xpos & 0x1) || (pstRect->u32Width & 0x1)))
    {
        TDE_TRACE(TDE_KERN_INFO, "x, width of YCbCr422R couldn't be odd!\n");
        return -1;
    }

    s32Bpp = TdeOsiGetbppByFmt(pstSurface->enColorFmt);

    if ( pstSurface->enColorFmt <= TDE2_COLOR_FMT_halfword )
    {
       
        if ((s32Bpp >= 8) && (s32Bpp != 24))
        {
            u32BytePerPixel = (s32Bpp >> 3);

           
            if (pstSurface->u32PhyAddr % u32BytePerPixel)
            {
                TDE_TRACE(TDE_KERN_INFO, "Bitmap address is not aligned!\n");
                return -1;
            }

            
            if (pstSurface->u32Stride % u32BytePerPixel)
            {
                TDE_TRACE(TDE_KERN_INFO, "stride is not aligned!\n");

                return -1;
            }
        }
        else if (s32Bpp == 24)
        {        
           
            if (pstSurface->u32PhyAddr % 4)
            {
                TDE_TRACE(TDE_KERN_INFO, "Bitmap address is not aligned!\n");
                return -1;
            }

            
            if (pstSurface->u32Stride % 4)
            {
                TDE_TRACE(TDE_KERN_INFO, "stride is not aligned!\n");

                return -1;
            }
        }
    }
    else
    {
        if (((pstSurface->u32CbCrStride > TDE_MAX_SURFACE_PITCH) || (pstSurface->u32CbCrStride == 0)) 
            && (pstSurface->enColorFmt != TDE2_COLOR_FMT_JPG_YCbCr400MBP))
        {
            TDE_TRACE(TDE_KERN_INFO, "Invalid CbCr stride!\n");
            return -1;
        }
    }

    if (pstRect->s32Xpos + pstRect->u32Width > pstSurface->u32Width)
    {
        /* AI7D02547 */
        pstRect->u32Width = pstSurface->u32Width - pstRect->s32Xpos;
    }

    if (pstRect->s32Ypos + pstRect->u32Height > pstSurface->u32Height)
    {
        /* AI7D02547 */
        pstRect->u32Height = pstSurface->u32Height - pstRect->s32Ypos;
    }

    
    if ((TDE_MAX_RECT_WIDTH < pstRect->u32Width)
        || (TDE_MAX_RECT_HEIGHT < pstRect->u32Height))
    {
        TDE_TRACE(TDE_KERN_INFO, "invalid operation area!");
        return -1;
    }

    return 0;
}

/*****************************************************************************
* Function:      TdeOsiCheckMbSurface
* Description:   adjust right operate zone, according by the size of bitmap and operate zone from user upload 
* Input:         pstSurface: bitmap info
*                pstRect: bitmap operate zone
* Output:        none
* Return:        success/fail
* Others:        none
*****************************************************************************/
STATIC INLINE HI_S32 TdeOsiCheckMbSurface(TDE2_MB_S* pstMbSurface, TDE2_RECT_S  *pstRect)
{
    TDE_CHECK_MBCOLORFMT(pstMbSurface->enMbFmt);
    
    if ((pstRect->s32Xpos < 0)
        || (TDE_MAX_RECT_WIDTH < pstRect->u32Width)
        || (TDE_MAX_RECT_HEIGHT < pstRect->u32Height)
        || (0 == pstRect->u32Height)
        || (0 == pstRect->u32Width)
        || (TDE_MAX_SURFACE_PITCH < pstMbSurface->u32YStride)
        || (TDE_MAX_SURFACE_PITCH < pstMbSurface->u32CbCrStride)
        || ((HI_U32)pstRect->s32Xpos >= pstMbSurface->u32YWidth)
        || (pstRect->s32Ypos < 0)
        || ((HI_U32)pstRect->s32Ypos >= pstMbSurface->u32YHeight))
    {
        TDE_TRACE(TDE_KERN_INFO, "invalid parameter in operation rect!\n");
        return -1;
    }

    if (pstRect->s32Xpos + pstRect->u32Width > pstMbSurface->u32YWidth)
    {
        pstRect->u32Width = pstMbSurface->u32YWidth - pstRect->s32Xpos;
    }

    if (pstRect->s32Ypos + pstRect->u32Height > pstMbSurface->u32YHeight)
    {
        pstRect->u32Height = pstMbSurface->u32YHeight - pstRect->s32Ypos;
    }

    return 0;
}

/*****************************************************************************
* Function:      TdeOsiGetOptCategory
* Description:   analyze TDE operate type
* Input:         pstBackGround: background bitmap info
*                pstBackGroundRect: background bitmap operate zone
*                pstForeGround: foreground bitmap info
*                pstForeGroundRect: foreground bitmap operate zone
*                pstDst: target bitmap info
*                pstDstRect: target bitmap operate zone
*                pstOpt: operate option
* Output:        none
* Return:        TDE operate type
* Others:        none
*****************************************************************************/
STATIC TDE_OPERATION_CATEGORY_E  TdeOsiGetOptCategory(TDE2_SURFACE_S * pstBackGround,
                                                      TDE2_RECT_S *    pstBackGroundRect,
                                                      TDE2_SURFACE_S * pstForeGround,
                                                      TDE2_RECT_S *    pstForeGroundRect,
                                                      TDE2_SURFACE_S * pstDst,
                                                      TDE2_RECT_S *    pstDstRect,
                                                      TDE2_OPT_S *     pstOpt)
{
    TDE2_SURFACE_S *pTmpSrc2 = HI_NULL;
    TDE2_RECT_S *pTmpSrc2Rect = HI_NULL;

    if ((HI_NULL == pstDst) || (HI_NULL == pstDstRect))
    {
        TDE_TRACE(TDE_KERN_INFO, "\n");
        return TDE_OPERATION_BUTT;
    }

    
    TDE_CHECK_NOT_MB(pstDst->enColorFmt);

    
    if (TdeOsiCheckSurface(pstDst, pstDstRect) < 0)
    {
        TDE_TRACE(TDE_KERN_INFO, "\n");
        return TDE_OPERATION_BUTT;
    }

    
    if ((HI_NULL != pstBackGround) && (HI_NULL != pstForeGround))
    {
        if ((HI_NULL == pstBackGroundRect) || (HI_NULL == pstForeGroundRect) || (HI_NULL == pstOpt))
        {
            TDE_TRACE(TDE_KERN_INFO, "\n");
            return TDE_OPERATION_BUTT;
        }

        if (TdeOsiCheckSurface(pstBackGround, pstBackGroundRect) < 0)
        {
            TDE_TRACE(TDE_KERN_INFO, "\n");
            return TDE_OPERATION_BUTT;
        }

        if (TdeOsiCheckSurface(pstForeGround, pstForeGroundRect) < 0)
        {
            TDE_TRACE(TDE_KERN_INFO, "\n");
            return TDE_OPERATION_BUTT;
        }

        if ((pstBackGroundRect->u32Height != pstDstRect->u32Height)
            || (pstBackGroundRect->u32Width != pstDstRect->u32Width))
        {
            TDE_TRACE(TDE_KERN_INFO, "\n");
            return TDE_OPERATION_BUTT;
        }

        if (!pstOpt->bResize)
        {
            
            if (pstBackGroundRect->u32Height != pstForeGroundRect->u32Height)
            {
                pstBackGroundRect->u32Height = TDE_MIN(pstBackGroundRect->u32Height, pstForeGroundRect->u32Height);
                pstDstRect->u32Height = pstBackGroundRect->u32Height;
                pstForeGroundRect->u32Height = pstBackGroundRect->u32Height;
            }

           
            if (pstBackGroundRect->u32Width != pstForeGroundRect->u32Width)
            {
                pstBackGroundRect->u32Width = TDE_MIN(pstBackGroundRect->u32Width, pstForeGroundRect->u32Width);
                pstDstRect->u32Width = pstBackGroundRect->u32Width;
                pstForeGroundRect->u32Width = pstBackGroundRect->u32Width;
            }
        }
        return TDE_OPERATION_DOUBLE_SRC;
    }

    if ((HI_NULL != pstBackGround) && (HI_NULL == pstForeGround))
    {
        pTmpSrc2 = pstBackGround;
        pTmpSrc2Rect = pstBackGroundRect;
    }
    else if ((HI_NULL == pstBackGround) && (HI_NULL != pstForeGround))
    {
        pTmpSrc2 = pstForeGround;
        pTmpSrc2Rect = pstForeGroundRect;
    }
    else
    {
        TDE_TRACE(TDE_KERN_INFO, "null pointer in single source 2 operation!");
        return TDE_OPERATION_BUTT;
    }

    if (HI_NULL == pTmpSrc2Rect)
    {
        TDE_TRACE(TDE_KERN_INFO, "\n");
        return TDE_OPERATION_BUTT;
    }

    
    if (TdeOsiCheckSurface(pTmpSrc2, pTmpSrc2Rect) < 0)
    {
        TDE_TRACE(TDE_KERN_INFO, "\n");
        return TDE_OPERATION_BUTT;
    }

    if ((HI_NULL == pstOpt) || (!pstOpt->bResize))
    {
        TDE_UNIFY_RECT(pTmpSrc2Rect, pstDstRect);
    }

    if ((HI_NULL == pstOpt)
        && (pTmpSrc2->enColorFmt == pstDst->enColorFmt))
    {
        return TDE_OPERATION_SINGLE_SRC1;
    }
    
    
    if (HI_NULL == pstOpt)
    {
        TDE_TRACE(TDE_KERN_INFO, "pstOpt can not be NULL in single source 2 operation!\n");
    
        return TDE_OPERATION_BUTT;
    }
    return TDE_OPERATION_SINGLE_SRC2;
}

/*****************************************************************************
* Function:      TdeOsiConvertSurface
* Description:   raster bitmap info by user upload translate to bitmap info which driver and hardware need
* Input:         pstSur: raster bitmap info by user upload
*                pstRect: raster bitmap operate zone by user upload
*                pstScanInfo: scanning direction info
*                pstDrvSur: bitmap info which driver and hardware need
* Output:        pstOperationArea: new operate zone fixed by scannning direction 
* Return:        none
* Others:        none
*****************************************************************************/
STATIC INLINE HI_VOID TdeOsiConvertSurface(TDE2_SURFACE_S *pstSur, TDE2_RECT_S *pstRect,
                                           TDE_SCANDIRECTION_S *pstScanInfo,
                                           TDE_DRV_SURFACE_S *pstDrvSur,
                                           TDE2_RECT_S *pstOperationArea)
{
    pstDrvSur->enColorFmt = g_enTdeCommonDrvColorFmt[pstSur->enColorFmt];
    pstDrvSur->u32Width  = pstRect->u32Width;
    pstDrvSur->u32Height = pstRect->u32Height;
    pstDrvSur->u32Pitch  = pstSur->u32Stride;
    pstDrvSur->bAlphaMax255 = pstSur->bAlphaMax255;
    pstDrvSur->enHScan = pstScanInfo->enHScan;
    pstDrvSur->enVScan = pstScanInfo->enVScan;
    pstDrvSur->u32PhyAddr = pstSur->u32PhyAddr;
#if defined (TDE_VERSION_PILOT) || defined (TDE_VERSION_FPGA)
    pstDrvSur->u32CbCrPhyAddr = pstSur->u32CbCrPhyAddr;
    pstDrvSur->u32CbCrPitch = pstSur->u32CbCrStride;
    pstDrvSur->enRgbOrder = g_enTdeArgbOrder[pstSur->enColorFmt];
#endif
    
    
    if(TDE_SCAN_RIGHT_LEFT == pstScanInfo->enHScan)
    {
        pstDrvSur->u32Xpos = (HI_U32)pstRect->s32Xpos + pstRect->u32Width - 1;
    }
    else
    {
        pstDrvSur->u32Xpos   = (HI_U32)pstRect->s32Xpos;
    }
    
    
    if(TDE_SCAN_DOWN_UP == pstScanInfo->enVScan)
    {
        pstDrvSur->u32Ypos = (HI_U32)pstRect->s32Ypos + pstRect->u32Height - 1;
    }
    else
    {
        pstDrvSur->u32Ypos   = (HI_U32)pstRect->s32Ypos;
    }

    if(NULL != pstOperationArea)
    {
        
        pstOperationArea->s32Xpos = pstRect->s32Xpos;//(HI_S32)pstDrvSur->u32Xpos;
        pstOperationArea->s32Ypos = (HI_S32)pstDrvSur->u32Ypos;
        pstOperationArea->u32Width = pstDrvSur->u32Width;
        pstOperationArea->u32Height = pstDrvSur->u32Height;
    }

    return;
}

/*****************************************************************************
* Function:      TdeOsiConvertMbSurface
* Description:   convert MB bitmap info to bitmap info which driver and hardware need
* Input:         pstMB: MB bitmap info
*                pstMbRect: MB bitmap operate zone
*                pstDrvY: brightness bitmap info
*                pstDrvCbCr: chroma bitmap info
*                enPicMode: picture mode: frame/ bottom field/ top field
* Output:        none
* Return:        none
* Others:        none
*****************************************************************************/
STATIC HI_S32 TdeOsiConvertMbSurface(TDE2_MB_S* pstMB, TDE2_RECT_S  *pstMbRect, TDE_DRV_SURFACE_S *pstDrvY,
                                     TDE_DRV_SURFACE_S *pstDrvCbCr, TDE_PIC_MODE_E enPicMode, HI_BOOL bCbCr2Opt)
{
    TDE_DRV_COLOR_FMT_E enAdjColorFmt;

    if ((HI_NULL == pstMB) || (HI_NULL == pstMbRect))
    {
        TDE_TRACE(TDE_KERN_INFO, "null ptr!\n");
        return -1;
    }

    if (HI_NULL != pstDrvY)
    {
        (HI_VOID)memset((HI_VOID*)pstDrvY, 0, sizeof(TDE_DRV_SURFACE_S));
        pstDrvY->enColorFmt = g_enTdeMbDrvColorFmt[pstMB->enMbFmt];
        pstDrvY->u32Width = pstMbRect->u32Width;
        pstDrvY->u32Xpos = (HI_U32)pstMbRect->s32Xpos;
        pstDrvY->u32PhyAddr = (TDE_BOTTOM_FIELD_PIC_MODE == enPicMode) ? 
                              (pstMB->u32YPhyAddr + pstMB->u32YStride) : (pstMB->u32YPhyAddr);
       
        if (TDE_FRAME_PIC_MODE == enPicMode)
        {
            pstDrvY->u32Height = pstMbRect->u32Height;
            pstDrvY->u32Pitch = pstMB->u32YStride;
            pstDrvY->u32Ypos = (HI_U32)pstMbRect->s32Ypos;
        }
        else     
        {
            pstDrvY->u32Height = (TDE_BOTTOM_FIELD_PIC_MODE == enPicMode) ? 
                TDE_ADJ_B_FIELD_HEIGHT_BY_START(pstMbRect->s32Ypos, pstMbRect->u32Height) :
                TDE_ADJ_FIELD_HEIGHT_BY_START(pstMbRect->s32Ypos, pstMbRect->u32Height);
            pstDrvY->u32Pitch = pstMB->u32YStride * 2;
            pstDrvY->u32Ypos = ((HI_U32)(pstMbRect->s32Ypos / 2));
        }
    }

    if (HI_NULL == pstDrvCbCr)
    {
        return 0;
    }

    enAdjColorFmt = g_enTdeMbDrvColorFmt[pstMB->enMbFmt];
    (HI_VOID)memset((HI_VOID*)pstDrvCbCr, 0, sizeof(TDE_DRV_SURFACE_S));
    pstDrvCbCr->enColorFmt = g_enTdeMbDrvColorFmt[pstMB->enMbFmt];
    pstDrvCbCr->u32PhyAddr = (TDE_BOTTOM_FIELD_PIC_MODE == enPicMode) ? 
                             (pstMB->u32CbCrPhyAddr + pstMB->u32CbCrStride) : (pstMB->u32CbCrPhyAddr);

    if (bCbCr2Opt)
    {
        enAdjColorFmt = TDE_DRV_COLOR_FMT_YCbCr444MB;
    }
    
    if (TDE_FRAME_PIC_MODE == enPicMode)
    {
        pstDrvCbCr->u32Pitch = pstMB->u32CbCrStride;

        
        switch (enAdjColorFmt)
        {
        case TDE_DRV_COLOR_FMT_YCbCr400MBP:
            return -1;
        case TDE_DRV_COLOR_FMT_YCbCr422MBH:
        {
            pstDrvCbCr->u32Xpos  = (HI_U32)pstMbRect->s32Xpos / 2;
            pstDrvCbCr->u32Ypos  = (HI_U32)pstMbRect->s32Ypos;
            pstDrvCbCr->u32Width =
                TDE_ADJ_SIZE_BY_START_P(pstMbRect->s32Xpos, pstMbRect->u32Width);
            pstDrvCbCr->u32Height = pstMbRect->u32Height;
        }
            break;
        case TDE_DRV_COLOR_FMT_YCbCr422MBV:
        {
            pstDrvCbCr->u32Xpos   = (HI_U32)pstMbRect->s32Xpos;
            pstDrvCbCr->u32Ypos   = (HI_U32)pstMbRect->s32Ypos / 2;
            pstDrvCbCr->u32Width  = pstMbRect->u32Width;
            pstDrvCbCr->u32Height =
                TDE_ADJ_SIZE_BY_START_P(pstMbRect->s32Ypos, pstMbRect->u32Height);
        }
            break;
        case TDE_DRV_COLOR_FMT_YCbCr420MB:
        {
            pstDrvCbCr->u32Xpos  = (HI_U32)pstMbRect->s32Xpos / 2;
            pstDrvCbCr->u32Ypos  = (HI_U32)pstMbRect->s32Ypos / 2;
            pstDrvCbCr->u32Width =
                TDE_ADJ_SIZE_BY_START_P(pstMbRect->s32Xpos, pstMbRect->u32Width);
            pstDrvCbCr->u32Height =
                TDE_ADJ_SIZE_BY_START_P(pstMbRect->s32Ypos, pstMbRect->u32Height);
            TDE_TRACE(TDE_KERN_DEBUG, "W:0x%x, H:0x%x\n", pstDrvCbCr->u32Width, 
            pstDrvCbCr->u32Height);    
        }
            break;
        case TDE_DRV_COLOR_FMT_YCbCr444MB:
        {
            pstDrvCbCr->u32Xpos   = (HI_U32)pstMbRect->s32Xpos;
            pstDrvCbCr->u32Ypos   = (HI_U32)pstMbRect->s32Ypos;
            pstDrvCbCr->u32Width  = pstMB->u32YWidth;
            pstDrvCbCr->u32Height = pstMbRect->u32Height;
        }
            break;
        default:
            TDE_TRACE(TDE_KERN_INFO, "Unspport color format!\n");
            return -1;
        }

        return 0;
    }
    
    pstDrvCbCr->u32Pitch = pstMB->u32CbCrStride * 2;

    
    switch (enAdjColorFmt)
    {
    case TDE_DRV_COLOR_FMT_YCbCr400MBP:
        return -1;
    case TDE_DRV_COLOR_FMT_YCbCr422MBH:
    {
        pstDrvCbCr->u32Xpos  = (HI_U32)pstMbRect->s32Xpos / 2;
        pstDrvCbCr->u32Ypos  = ((HI_U32)pstMbRect->s32Ypos / 2);
        pstDrvCbCr->u32Width =
            TDE_ADJ_SIZE_BY_START_P(pstMbRect->s32Xpos, pstMbRect->u32Width);
        pstDrvCbCr->u32Height = (TDE_BOTTOM_FIELD_PIC_MODE == enPicMode) ?
            TDE_ADJ_B_FIELD_HEIGHT_BY_START(pstMbRect->s32Ypos, pstMbRect->u32Height) :
            TDE_ADJ_FIELD_HEIGHT_BY_START(pstMbRect->s32Ypos, pstMbRect->u32Height);
    }
        break;
    case TDE_DRV_COLOR_FMT_YCbCr422MBV:
    {
        pstDrvCbCr->u32Xpos   = (HI_U32)pstMbRect->s32Xpos;
        pstDrvCbCr->u32Ypos   = ((HI_U32)pstMbRect->s32Ypos / 4);
        pstDrvCbCr->u32Width  = pstMbRect->u32Width;
        pstDrvCbCr->u32Height = (TDE_BOTTOM_FIELD_PIC_MODE == enPicMode) ?
            TDE_ADJ_B_SIZE_BY_START_I(pstMbRect->s32Ypos, pstMbRect->u32Height) :
            TDE_ADJ_SIZE_BY_START_I(pstMbRect->s32Ypos, pstMbRect->u32Height);
    }
        break;
    case TDE_DRV_COLOR_FMT_YCbCr420MB:
    {
        pstDrvCbCr->u32Xpos  = (HI_U32)pstMbRect->s32Xpos / 2;
        pstDrvCbCr->u32Ypos  = ((HI_U32)pstMbRect->s32Ypos / 4);
        pstDrvCbCr->u32Width =
            TDE_ADJ_SIZE_BY_START_P(pstMbRect->s32Xpos, pstMbRect->u32Width);
        pstDrvCbCr->u32Height = (TDE_BOTTOM_FIELD_PIC_MODE == enPicMode) ?
            TDE_ADJ_B_SIZE_BY_START_I(pstMbRect->s32Ypos, pstMbRect->u32Height) :
            TDE_ADJ_SIZE_BY_START_I(pstMbRect->s32Ypos, pstMbRect->u32Height);
        TDE_TRACE(TDE_KERN_DEBUG, "W:0x%x, H:0x%x\n", pstDrvCbCr->u32Width, 
        pstDrvCbCr->u32Height);    
    }
        break;
    case TDE_DRV_COLOR_FMT_YCbCr444MB:
    {
        pstDrvCbCr->u32Xpos   = (HI_U32)pstMbRect->s32Xpos;
        pstDrvCbCr->u32Ypos  = (TDE_BOTTOM_FIELD_PIC_MODE == enPicMode) ?
                               ((HI_U32)(pstMbRect->s32Ypos / 2 + 1)) : ((HI_U32)pstMbRect->s32Ypos / 2);
        pstDrvCbCr->u32Width = pstMB->u32YWidth;
        pstDrvCbCr->u32Height = (TDE_BOTTOM_FIELD_PIC_MODE == enPicMode) ?
            TDE_ADJ_B_FIELD_HEIGHT_BY_START(pstMbRect->s32Ypos, pstMbRect->u32Height) :
            TDE_ADJ_FIELD_HEIGHT_BY_START(pstMbRect->s32Ypos, pstMbRect->u32Height);
    }
        break;
    default:
        TDE_TRACE(TDE_KERN_INFO, "Unspport color format!\n");
        return -1;
    }

    return 0;
}

/*****************************************************************************
* Function:      TdeOsiSetClipPara
* Description:   set clip zone parameter
* Input:         pstBackGround: background bitmap info
*                pstBGRect: background bitmap operate zone
*                pstForeGround: foreground bitmap info
*                pstFGRect: foreground bitmap operate zone
*                pstDst:  target bitmap info
*                pstDstRect: target bitmap operate zone
*                pstOpt: operate option
*                pstHwNode: hardware operate node
* Output:        none
* Return:        none
* Others:        none
*****************************************************************************/
STATIC HI_S32 TdeOsiSetClipPara(TDE2_SURFACE_S * pstBackGround, TDE2_RECT_S *pstBGRect,
                                 TDE2_SURFACE_S * pstForeGround,
                                 TDE2_RECT_S *pstFGRect, TDE2_SURFACE_S * pstDst, TDE2_RECT_S *pstDstRect,
                                 TDE2_OPT_S * pstOpt,
                                 TDE_HWNode_S *pstHwNode)
{
    TDE_DRV_CLIP_CMD_S stClip = {0};
    TDE2_RECT_S stInterRect = {0};
    
    TDE_ASSERT(NULL != pstForeGround);
    TDE_ASSERT(NULL != pstDst);
    TDE_ASSERT(NULL != pstOpt);
    TDE_ASSERT(NULL != pstHwNode);

    
    if ((TDE2_CLIPMODE_INSIDE == pstOpt->enClipMode) && (!pstOpt->bResize))
    {
       
        if (TdeOsiGetInterRect(pstDstRect, &pstOpt->stClipRect, &stInterRect) < 0)
        {
            TDE_TRACE(TDE_KERN_INFO, "clip and operation area have no inrerrect!\n");
            return HI_ERR_TDE_CLIP_AREA;
        }

        if (NULL != pstBackGround)
        {
            
            pstBGRect->s32Xpos += stInterRect.s32Xpos - pstDstRect->s32Xpos;
            pstBGRect->s32Ypos  += stInterRect.s32Ypos - pstDstRect->s32Ypos;
            pstBGRect->u32Height = stInterRect.u32Height;
            pstBGRect->u32Width = stInterRect.u32Width;
        }

       
        pstFGRect->s32Xpos += stInterRect.s32Xpos - pstDstRect->s32Xpos;
        pstFGRect->s32Ypos  += stInterRect.s32Ypos - pstDstRect->s32Ypos;
        pstFGRect->u32Height = stInterRect.u32Height;
        pstFGRect->u32Width = stInterRect.u32Width;

       
        *pstDstRect = stInterRect;
    }
    else if (TDE2_CLIPMODE_INSIDE == pstOpt->enClipMode)
    {
        
        if (TdeOsiGetInterRect(pstDstRect, &pstOpt->stClipRect, &stInterRect) < 0)
        {
            TDE_TRACE(TDE_KERN_INFO, "clip and operation area have no inter-rect!\n");
            return HI_ERR_TDE_CLIP_AREA;
        }
        
        stClip.bInsideClip   = HI_TRUE ;
        stClip.u16ClipStartX = pstOpt->stClipRect.s32Xpos;
        stClip.u16ClipStartY = pstOpt->stClipRect.s32Ypos;
        stClip.u16ClipEndX = pstOpt->stClipRect.s32Xpos + pstOpt->stClipRect.u32Width - 1;
        stClip.u16ClipEndY = pstOpt->stClipRect.s32Ypos + pstOpt->stClipRect.u32Height - 1;

        
        TdeHalNodeSetClipping(pstHwNode, &stClip);
    }
    else if(TDE2_CLIPMODE_OUTSIDE == pstOpt->enClipMode) 
    {
        if (TdeOsiIsRect1InRect2(pstDstRect, &pstOpt->stClipRect))
        {
            TDE_TRACE(TDE_KERN_INFO, "clip and operation area have no inter-rect!\n");
            return HI_ERR_TDE_CLIP_AREA;
        }

        
        if (TdeOsiGetInterRect(pstDstRect, &pstOpt->stClipRect, &stInterRect) < 0)
        {
            return 0;
        }

        stClip.bInsideClip   = HI_FALSE;
        stClip.u16ClipStartX = pstOpt->stClipRect.s32Xpos;
        stClip.u16ClipStartY = pstOpt->stClipRect.s32Ypos;
        stClip.u16ClipEndX = pstOpt->stClipRect.s32Xpos + pstOpt->stClipRect.u32Width - 1;
        stClip.u16ClipEndY = pstOpt->stClipRect.s32Ypos + pstOpt->stClipRect.u32Height - 1;

        
        TdeHalNodeSetClipping(pstHwNode, &stClip);
    }
    else if(pstOpt->enClipMode >= TDE2_CLIPMODE_BUTT)
    {
        TDE_TRACE(TDE_KERN_INFO, "error clip mode!\n");
        return HI_ERR_TDE_INVALID_PARA;
    }

    return 0;
}

STATIC HI_S32 TdeOsiSetPatternClipPara(TDE2_SURFACE_S * pstBackGround, TDE2_RECT_S *pstBGRect,
                                 TDE2_SURFACE_S * pstForeGround,
                                 TDE2_RECT_S *pstFGRect, TDE2_SURFACE_S * pstDst, TDE2_RECT_S *pstDstRect,
                                 TDE2_PATTERN_FILL_OPT_S * pstOpt,
                                 TDE_HWNode_S *pstHwNode)
{
    TDE_DRV_CLIP_CMD_S stClip = {0};
    TDE2_RECT_S stInterRect = {0};
    
    TDE_ASSERT(NULL != pstForeGround);
    TDE_ASSERT(NULL != pstDst);
    TDE_ASSERT(NULL != pstOpt);
    TDE_ASSERT(NULL != pstHwNode);

    TDE_CHECK_CLIPMODE(pstOpt->enClipMode);
  
    if (TDE2_CLIPMODE_INSIDE == pstOpt->enClipMode)
    {
        
        if (TdeOsiGetInterRect(pstDstRect, &pstOpt->stClipRect, &stInterRect) < 0)
        {
            TDE_TRACE(TDE_KERN_INFO, "clip and operation area have no inrerrect!\n");
            return HI_ERR_TDE_CLIP_AREA;
        }

#if 0
        if (NULL != pstBackGround)
        {
            pstBGRect->s32Xpos += stInterRect.s32Xpos - pstDstRect->s32Xpos;
            pstBGRect->s32Ypos  += stInterRect.s32Ypos - pstDstRect->s32Ypos;
            pstBGRect->u32Height = stInterRect.u32Height;
            pstBGRect->u32Width = stInterRect.u32Width;
        }
#if 1
        pstFGRect->s32Xpos += stInterRect.s32Xpos - pstDstRect->s32Xpos;
        pstFGRect->s32Ypos  += stInterRect.s32Ypos - pstDstRect->s32Ypos;
        pstFGRect->u32Height = stInterRect.u32Height;
        pstFGRect->u32Width = stInterRect.u32Width;
#endif
        *pstDstRect = stInterRect;
#endif

#if 1
            stClip.bInsideClip   = HI_TRUE;
            stClip.u16ClipStartX = pstOpt->stClipRect.s32Xpos;
            stClip.u16ClipStartY = pstOpt->stClipRect.s32Ypos;
            stClip.u16ClipEndX = pstOpt->stClipRect.s32Xpos + pstOpt->stClipRect.u32Width - 1;
            stClip.u16ClipEndY = pstOpt->stClipRect.s32Ypos + pstOpt->stClipRect.u32Height - 1;
    
            
            TdeHalNodeSetClipping(pstHwNode, &stClip);
#endif            


        
    }
    else if(TDE2_CLIPMODE_OUTSIDE == pstOpt->enClipMode) 
    {
        if (TdeOsiIsRect1InRect2(pstDstRect, &pstOpt->stClipRect))
        {
            TDE_TRACE(TDE_KERN_INFO, "clip and operation area have no inter-rect!\n");
            return HI_ERR_TDE_CLIP_AREA;
        }

        
        if (TdeOsiGetInterRect(pstDstRect, &pstOpt->stClipRect, &stInterRect) < 0)
        {
            return 0;
        }

        stClip.bInsideClip   = HI_FALSE;
        stClip.u16ClipStartX = pstOpt->stClipRect.s32Xpos;
        stClip.u16ClipStartY = pstOpt->stClipRect.s32Ypos;
        stClip.u16ClipEndX = pstOpt->stClipRect.s32Xpos + pstOpt->stClipRect.u32Width - 1;
        stClip.u16ClipEndY = pstOpt->stClipRect.s32Ypos + pstOpt->stClipRect.u32Height - 1;

        
        TdeHalNodeSetClipping(pstHwNode, &stClip);
    }
    else if(pstOpt->enClipMode >= TDE2_CLIPMODE_BUTT)
    {
        TDE_TRACE(TDE_KERN_INFO, "error clip mode!\n");
        return HI_ERR_TDE_INVALID_PARA;
    }

    return 0;
}

/*****************************************************************************
* Function:      TdeOsiMbSetClipPara
* Description:   set clip zone parameter by MB operate 
* Input:         pstDstRect:background bitmap operate zone
*                pstMbOpt: set parameter
*                pstHwNode: hardware operate node
* Output:        none
* Return:        none
* Others:        note: MB operate is not refer to software optimization of clip zone
*****************************************************************************/
STATIC HI_S32 TdeOsiMbSetClipPara(TDE2_RECT_S * pstDstRect, TDE2_MBOPT_S *pstMbOpt,
                                 TDE_HWNode_S *pstHwNode)
{
    TDE2_RECT_S stInterRect = {0};
    
    TDE_ASSERT(NULL != pstDstRect);
    TDE_ASSERT(NULL != pstMbOpt);
    TDE_ASSERT(NULL != pstHwNode);

    if (TDE2_CLIPMODE_INSIDE == pstMbOpt->enClipMode)
    {
       
        if (TdeOsiGetInterRect(pstDstRect, &pstMbOpt->stClipRect, &stInterRect) < 0)
        {
            TDE_TRACE(TDE_KERN_INFO, "clip and operation area have no inter-rect!\n");
            return HI_ERR_TDE_CLIP_AREA;
        }
        TDE_SET_CLIP(pstHwNode, pstMbOpt);
    }
    else if(TDE2_CLIPMODE_OUTSIDE == pstMbOpt->enClipMode) 
    {
        if (TdeOsiIsRect1InRect2(pstDstRect, &pstMbOpt->stClipRect))
        {
            TDE_TRACE(TDE_KERN_INFO, "clip and operation area have no inter-rect!\n");
            return HI_ERR_TDE_CLIP_AREA;
        }
        
        TDE_SET_CLIP(pstHwNode, pstMbOpt);
    }
    else if(pstMbOpt->enClipMode >= TDE2_CLIPMODE_BUTT)
    {
        TDE_TRACE(TDE_KERN_INFO, "error clip mode!\n");
        return HI_ERR_TDE_INVALID_PARA;
    }
    return 0;
}
/*****************************************************************************
 Prototype       : TdeOsiSetExtAlpha
 Description     : two alpha values when set ARGB1555 format
 Input           : pstBackGround  **
                   pstForeGround  **
                   pstHwNode      **
 Output          : None
 Return Value    : 
 Global Variable   
    Read Only    : 
    Read & Write : 
  History         
  1.Date         : 2008/6/10
    Author       : executable frame by single
    Modification : Created function

*****************************************************************************/
STATIC INLINE HI_VOID TdeOsiSetExtAlpha(TDE2_SURFACE_S *pstBackGround, TDE2_SURFACE_S *pstForeGround,
                                        TDE_HWNode_S *pstHwNode)
{
    TDE_DRV_SRC_E enSrc = TDE_DRV_SRC_NONE;

    if((NULL != pstForeGround)
        && (pstForeGround->bAlphaExt1555)
        && (TDE2_COLOR_FMT_ARGB1555 <= pstForeGround->enColorFmt)
        && (pstForeGround->enColorFmt <= TDE2_COLOR_FMT_BGRA1555))
    {
        enSrc |= TDE_DRV_SRC_S2;
    }
    
    if((NULL != pstBackGround)
        && (pstBackGround->bAlphaExt1555)
        && (TDE2_COLOR_FMT_ARGB1555 <= pstBackGround->enColorFmt)
        && (pstBackGround->enColorFmt <= TDE2_COLOR_FMT_BGRA1555))
    {
        enSrc |= TDE_DRV_SRC_S1;
    }

   
    if(enSrc & TDE_DRV_SRC_S1)
    {
        TdeHalNodeSetExpAlpha(pstHwNode, enSrc, pstBackGround->u8Alpha0, pstBackGround->u8Alpha1);
    }
    else if(enSrc & TDE_DRV_SRC_S2)
    {
        TdeHalNodeSetExpAlpha(pstHwNode, enSrc, pstForeGround->u8Alpha0, pstForeGround->u8Alpha1);
    }
}

/*****************************************************************************
* Function:      TdeOsiSetBaseOptParaForBlit
* Description:   encapsulation function used to set operate type
* Input:         pstOpt: operate option
*                enOptCategory: operate category
*                pstHwNode: nareware operate node
* Output:        none
* Return:        none
* Others:        none
*****************************************************************************/
STATIC HI_VOID TdeOsiSetBaseOptParaForBlit(TDE2_OPT_S * pstOpt, TDE2_SURFACE_S* pstSrc1, TDE2_SURFACE_S* pstSrc2,
                                           TDE_OPERATION_CATEGORY_E enOptCategory, TDE_HWNode_S *pstHwNode)
{
    TDE_DRV_BASEOPT_MODE_E enBaseOpt = {0};
    TDE_DRV_ALU_MODE_E enAlu = TDE_ALU_NONE;

    if ((HI_NULL == pstHwNode) || (HI_NULL == pstOpt))
    {
        return;
    }

    if ((HI_NULL != pstSrc1) && (HI_NULL != pstSrc2))
    {
        if ( (TDE_COLORFMT_CATEGORY_An == TdeOsiGetFmtCategory(pstSrc2->enColorFmt))
              && (TDE_COLORFMT_CATEGORY_ARGB == TdeOsiGetFmtCategory(pstSrc1->enColorFmt)
                  || TDE_COLORFMT_CATEGORY_YCbCr == TdeOsiGetFmtCategory(pstSrc1->enColorFmt)) )
        {
            enAlu = TDE_SRC1_BYPASS;
        }
    }

    if (pstOpt->enAluCmd & TDE2_ALUCMD_ROP)
    {
        enAlu = TDE_ALU_ROP;
        
       
        TdeHalNodeSetRop(pstHwNode, pstOpt->enRopCode_Color, pstOpt->enRopCode_Alpha);
    }
    
    if (pstOpt->enAluCmd & TDE2_ALUCMD_BLEND)
    {
        enAlu = TDE_ALU_BLEND;

#if 0
        if (!pstOpt->stBlendOpt.bSrc2AlphaPremulti)
        {
            enAlu = TDE_ALU_BLEND_SRC2;
        }
#else
        TdeHalNodeSetBlend(pstHwNode, &pstOpt->stBlendOpt);
#endif

        if (pstOpt->enAluCmd & TDE2_ALUCMD_ROP)
        {
            TdeHalNodeEnableAlphaRop(pstHwNode);
        }
    }
    
    if (pstOpt->enAluCmd & TDE2_ALUCMD_COLORIZE)
    {
        TdeHalNodeSetColorize(pstHwNode, pstOpt->u32Colorize);
    }

    if (TDE_OPERATION_DOUBLE_SRC == enOptCategory)
    {
        enBaseOpt = TDE_NORM_BLIT_2OPT;
    }
    else
    {
        enBaseOpt = TDE_NORM_BLIT_1OPT;
    }

    
    TdeHalNodeSetGlobalAlpha(pstHwNode, pstOpt->u8GlobalAlpha, pstOpt->stBlendOpt.bGlobalAlphaEnable);
    
    TdeHalNodeSetBaseOperate(pstHwNode, enBaseOpt, enAlu, 0);

    TDeHalNodeSetCsc(pstHwNode, pstOpt->stCscOpt);	

    return;
}

/*****************************************************************************
* Function:      TdeOsiSetResizePara
* Description:   set zoom coefficient to hardware node
* Input:         pstHwNode: node need to set
*                pstFilterOpt: filter parameter
* Output:        none
* Return:        none
* Others:        none
*****************************************************************************/
STATIC INLINE HI_VOID TdeOsiSetResizePara(TDE_HWNode_S* pstHwNode, TDE_FILTER_OPT* pstFilterOpt, TDE_NODE_SUBM_TYPE_E enNodeType)
{
    TDE_DRV_RESIZE_CMD_S stResizeCmd = {0};

    
    stResizeCmd.bCoefSym = pstFilterOpt->bCoefSym;
    stResizeCmd.bHfRing = pstFilterOpt->bHRing;
    stResizeCmd.bVfRing = pstFilterOpt->bVRing;
    if (TDE_NO_SCALE_STEP == pstFilterOpt->u32HStep)
    {
        stResizeCmd.enFilterH = TDE_DRV_FILTER_NONE;
    }
    else
    {
        stResizeCmd.enFilterH = pstFilterOpt->enFilterMode;//TDE_DRV_FILTER_ALL;
    }

    if (TDE_NO_SCALE_STEP == pstFilterOpt->u32VStep)
    {
        stResizeCmd.enFilterV = TDE_DRV_FILTER_NONE;
    }
    else
    {
        stResizeCmd.enFilterV = pstFilterOpt->enFilterMode;//TDE_DRV_FILTER_ALL;
    }

   
    stResizeCmd.u32OffsetX = (HI_U32)pstFilterOpt->s32HOffset;
    stResizeCmd.u32OffsetY = (HI_U32)pstFilterOpt->s32VOffset;
    stResizeCmd.u32StepH = pstFilterOpt->u32HStep;
    stResizeCmd.u32StepV = pstFilterOpt->u32VStep;

    stResizeCmd.bFirstLineOut = pstFilterOpt->bFirstLineOut;
    stResizeCmd.bLastLineOut = pstFilterOpt->bLastLineOut;
    TdeHalNodeSetResize(pstHwNode, &stResizeCmd, enNodeType);
}

/*****************************************************************************
* Function:      TdeOsiSetDeflickerPara
* Description:   set deflicker coefficient to hardware node
* Input:         pstHwNode: node need to set 
*
* Output:        none
* Return:        none
* Others:        none
*****************************************************************************/
STATIC INLINE HI_VOID TdeOsiSetDeflickerPara(TDE_HWNode_S* pstHwNode, TDE2_DEFLICKER_MODE_E enDeflickerMode, TDE_FILTER_OPT* pstFilterOpt)
{
    TDE_DRV_FLICKER_CMD_S stFlickerCmd = {0};
    HI_BOOL bDeflicker = (TDE2_DEFLICKER_MODE_NONE == enDeflickerMode)?HI_FALSE:HI_TRUE;

//#define __TDE_CHECK_SLICE_DATA__

#ifndef __TDE_CHECK_SLICE_DATA__
    HI_U8 u8DefTable[] = {0, 64, 0, 12, 40, 12, 15, 37, 15, 17, 34, 17,
                          2, 4, 7};
#else
    HI_U8 u8DefTable[] = {0, 64, 0, 0, 64, 0, 0, 64, 0, 0, 64, 0,
                          0, 64, 0};
#endif

    
    if (HI_TRUE == bDeflicker)
    {
        stFlickerCmd.enDfeMode = TDE_DRV_AUTO_FILTER;
    }
    
    memcpy((HI_VOID*)&stFlickerCmd.u8Coef0LastLine, (const HI_VOID *)u8DefTable, sizeof(u8DefTable));

    stFlickerCmd.enFilterV = pstFilterOpt->enFilterMode;
    stFlickerCmd.enDeflickerMode = enDeflickerMode;
    TdeHalNodeSetFlicker(pstHwNode, &stFlickerCmd);
}


STATIC HI_S32 TdeOsiAdjClipPara(TDE_HWNode_S* pHWNode)
{
	
	if(pHWNode->u32TDE_CLIP_START >> 31)
	{
		HI_U32 u32ClipStartX, u32ClipStartY, u32ClipEndX, u32ClipEndY;
		HI_U32 u32OutStartX, u32OutStartY, u32OutEndX, u32OutEndY;
		HI_U32 u32StartX, u32StartY, u32EndX, u32EndY;

		u32ClipStartX = pHWNode->u32TDE_CLIP_START & 0xfff;
		u32ClipStartY = (pHWNode->u32TDE_CLIP_START >> 16) & 0xfff;
		u32ClipEndX = pHWNode->u32TDE_CLIP_STOP & 0xfff;
		u32ClipEndY = (pHWNode->u32TDE_CLIP_STOP >> 16) & 0xfff;

		u32OutStartX = pHWNode->u32TDE_TAR_XY & 0xfff;
		u32OutStartY = (pHWNode->u32TDE_TAR_XY >> 16) & 0xfff;
		u32OutEndX = u32OutStartX + (pHWNode->u32TDE_TS_SIZE & 0xfff) - 1;
		u32OutEndY = u32OutStartY + ((pHWNode->u32TDE_TS_SIZE >> 16) & 0xfff) - 1;

		u32StartX = (u32ClipStartX > u32OutStartX) ? u32ClipStartX : u32OutStartX;
		u32StartY = (u32ClipStartY > u32OutStartY) ? u32ClipStartY : u32OutStartY;
		u32EndX = (u32ClipEndX < u32OutEndX) ? u32ClipEndX : u32OutEndX;
		u32EndY = (u32ClipEndY < u32OutEndY) ? u32ClipEndY : u32OutEndY;


		if((u32StartX > u32EndX) || (u32StartY > u32EndY))
		{
			pHWNode->u32TDE_INS = pHWNode->u32TDE_INS & 0xffffbfff; 
		}
		else
		{
			pHWNode->u32TDE_CLIP_START = ((0x80000000) | (u32StartX & 0xfff) | ((u32StartY & 0xfff) << 16));
			pHWNode->u32TDE_CLIP_STOP = ( (u32EndX & 0xfff) | ((u32EndY & 0xfff) << 16));
		}
	
	}

	return HI_SUCCESS;

}

/*****************************************************************************
* Function:      TdeOsiSetNodeFinish
* Description:   complete node config, add node to list signed by handle
* Input:         s32Handle: task handle, which is sumbit list  
*                pHWNode: set node
*                u32WorkBufNum: temporary buffer number
*                enSubmType: submit node type
* Output:        none
* Return:        return slice number
* Others:        node struct is following:
*                 ----------------------
*                 |  software node pointer(4)   |
*                 ----------------------
*                 |  config parameter          |
*                 ----------------------
*                 |  physical address of next node(4) |
*                 ----------------------
*                 |  update flag of next node(4) |
*                 ----------------------
*****************************************************************************/
STATIC HI_S32 TdeOsiSetNodeFinish(TDE_HANDLE s32Handle, TDE_HWNode_S* pHWNode,
                                  HI_U32 u32WorkBufNum, TDE_NODE_SUBM_TYPE_E enSubmType)
{
    HI_U32 u32Size;
    HI_VOID* pBuf = HI_NULL;
    TDE_NODE_BUF_S stSwNode = {0};
    HI_S32 s32Ret;

    
    TdeOsiAdjClipPara(pHWNode);

    u32Size = TdeHalNodeGetNdSize(pHWNode);
    if (0 == u32Size)
    {
        TDE_TRACE(TDE_KERN_INFO, "error buffer size!\n");
    
        return HI_FAILURE;
    }

    tde_osr_disableirq();
    pBuf = (HI_VOID *)TDE_MALLOC(u32Size + TDE_NODE_HEAD_BYTE + TDE_NODE_TAIL_BYTE);
    tde_osr_enableirq();
    if (HI_NULL == pBuf)
    {
        TDE_TRACE(TDE_KERN_INFO, "malloc (%d) failed, wgetfreenum(%d)!\n", (u32Size + TDE_NODE_HEAD_BYTE + TDE_NODE_TAIL_BYTE), wgetfreenum());
        return HI_ERR_TDE_NO_MEM;
    }

    
    if (TdeHalNodeMakeNd((HI_CHAR *)pBuf + TDE_NODE_HEAD_BYTE, pHWNode) < 0)
    {
        TDE_TRACE(TDE_KERN_INFO, "TdeHalNodeMakeNd fail!\n");
        TDE_FREE(pBuf);
        return HI_ERR_TDE_INVALID_PARA;
    }

    TDEProcRecordNode(pHWNode);
    
    stSwNode.pBuf = pBuf;
    stSwNode.u32NodeSz = u32Size;
    stSwNode.u64Update = pHWNode->u64TDE_UPDATE;
    stSwNode.u32PhyAddr = wgetphy(pBuf) + TDE_NODE_HEAD_BYTE;

    if((s32Ret = TdeOsiListAddNode(s32Handle, &stSwNode, enSubmType, u32WorkBufNum)) < 0)
    {
        TDE_TRACE(TDE_KERN_INFO, "TdeOsiListAddNode fail!\n");
        TDE_FREE(pBuf);
        return s32Ret;
    }
    return HI_SUCCESS;
}

/****************************************************************************
                             TDE osi ctl interface initialization
****************************************************************************/

/*****************************************************************************
* Function:      TdeOsiBeginJob
* Description:   get TDE task handle
* Input:         none
* Output:        none
* Return:        created task handle
* Others:        none
*****************************************************************************/
HI_S32 TdeOsiBeginJob(TDE_HANDLE *ps32Handle)
{
    if (NULL == ps32Handle)
    {
        TDE_TRACE(TDE_KERN_INFO, "Null ptr!\n");
        return HI_ERR_TDE_NULL_PTR;
    }

    return TdeOsiListBeginJob(ps32Handle);
}
EXPORT_SYMBOL(TdeOsiBeginJob);

/*****************************************************************************
* Function:      TdeOsiEndJob
* Description:   submit TDE task
* Input:         s32Handle: task handle
*                bBlock: if block
*                u32TimeOut: timeout value(unit by 10ms) 
* Output:        none
* Return:        success / fail
* Others:        none
*****************************************************************************/
HI_S32     TdeOsiEndJob(TDE_HANDLE s32Handle, HI_BOOL bBlock, HI_U32 u32TimeOut,
                        HI_BOOL bSync, TDE_FUNC_CB pFuncComplCB, HI_VOID *pFuncPara)
{
    TDE_LIST_TYPE_E enListType;
    TDE_NOTIFY_MODE_E enNotiType;
#if HI_TDE_SQ_SUPPORT
    if (bSync)
    {
        enListType = TDE_LIST_SQ;
    }
    else
#endif        
    {
        enListType = TDE_LIST_AQ;
    }

    if (bBlock)
    {
        if(in_interrupt())
        {
            TDE_TRACE(TDE_KERN_INFO, "can not be block in interrupt!\n");
            return HI_ERR_TDE_UNSUPPORTED_OPERATION;
        }
        enNotiType = TDE_JOB_WAKE_NOTIFY;
    }
    else
    {
        enNotiType = TDE_JOB_COMPL_NOTIFY;
    }

    return TdeOsiListSubmitJob(s32Handle, enListType, u32TimeOut, pFuncComplCB, pFuncPara, enNotiType);
}
EXPORT_SYMBOL(TdeOsiEndJob);

/*****************************************************************************
* Function:      TdeOsiDelJob
* Description:   delete created TDE task, only effective for call before endjob
*                use to release software resource of list of task
* Input:         s32Handle: task handle
* Output:        none
* Return:        success/fail
* Others:        none
*****************************************************************************/
HI_S32     TdeOsiCancelJob(TDE_HANDLE s32Handle)
{
    return TdeOsiListCancelJob(s32Handle);
}
EXPORT_SYMBOL(TdeOsiCancelJob);

/*****************************************************************************
* Function:      TdeOsiWaitForDone
* Description:   wait for completion of submit TDE operate
* Input:         s32Handle: task handle
* Output:        none
* Return:        success/fail
* Others:        none
*****************************************************************************/
HI_S32 TdeOsiWaitForDone(TDE_HANDLE s32Handle, HI_U32 u32TimeOut)
{
    if(in_interrupt())
    {
        TDE_TRACE(TDE_KERN_INFO, "can not be block in interrupt!\n");
        return HI_ERR_TDE_UNSUPPORTED_OPERATION;
    }

    return TdeOsiListWaitForDone(s32Handle, u32TimeOut);
}
EXPORT_SYMBOL(TdeOsiWaitForDone);

/*****************************************************************************
* Function:      TdeOsiWaitAllDone
* Description:   wait for all TDE operate completion
* Input:         none
* Output:        none
* Return:        success/fail
* Others:        none
*****************************************************************************/
HI_S32 TdeOsiWaitAllDone(HI_BOOL bSync)
{
#if HI_TDE_SQ_SUPPORT
    TDE_LIST_TYPE_E enListType = (bSync) ? (TDE_LIST_SQ) : (TDE_LIST_AQ);
#else
    TDE_LIST_TYPE_E enListType = TDE_LIST_AQ;
#endif
    if(in_interrupt())
    {
        TDE_TRACE(TDE_KERN_INFO, "can not wait in interrupt!\n");
        return HI_ERR_TDE_UNSUPPORTED_OPERATION;
    }

    return TdeOsiListWaitAllDone(enListType);
}
EXPORT_SYMBOL(TdeOsiWaitAllDone);

/*****************************************************************************
* Function:      TdeOsiReset
* Description:   reset all state, release all list
* Input:         none
* Output:        none
* Return:        success/fail
* Others:        none
*****************************************************************************/
HI_VOID TdeOsiReset(HI_VOID)
{
    return TdeOsiListReset();
}
EXPORT_SYMBOL(TdeOsiReset);

/*****************************************************************************
* Function:      TdeOsiQuickBlit
* Description:   quick blit source to target, no any functional operate, the size of source and target are the same
*                format is not MB format
* Input:         pSrc: source bitmap info
*                pstDst:  target bitmap info
*                pFuncComplCB: callback function pointer when operate is completed
				when it is null, it say to need not notice operate is over
* Output:        none
* Return:        none
* Others:        none
*****************************************************************************/
HI_S32     TdeOsiQuickBitblit(TDE_HANDLE s32Handle, TDE2_SURFACE_S* pstSrc, TDE2_RECT_S *pstSrcRect,
                              TDE2_SURFACE_S* pstDst,
                              TDE2_RECT_S *pstDstRect,
                              HI_BOOL bDeflicker)
{
    TDE_HWNode_S stHWNode = {0};
    TDE_DRV_SURFACE_S stSrcDrvSurface = {0};
    TDE_DRV_SURFACE_S stDstDrvSurface = {0};
    TDE_SCANDIRECTION_S stSrcScanInfo = {0};
    TDE_SCANDIRECTION_S stDstScanInfo = {0};
    HI_U16 u16Code = 0;
    TDE_DRV_CONV_MODE_CMD_S stConv = {0};
    TDE_FILTER_OPT stFilterOpt = {0};
    TDE2_RECT_S stSrcOptArea = {0};
    TDE2_RECT_S stDstOptArea = {0};
    TDE2_OPT_S stOpt = {0};

    HI_BOOL bResize = HI_FALSE;

    if ((NULL == pstSrc) || (NULL == pstDst) || (NULL == pstSrcRect) || (NULL == pstDstRect))
    {
        TDE_TRACE(TDE_KERN_INFO, "Null ptr!\n");
        return HI_ERR_TDE_NULL_PTR;
    }

   
    TDE_CHECK_NOT_MB(pstSrc->enColorFmt);
    TDE_CHECK_NOT_MB(pstDst->enColorFmt);

    if (TdeOsiCheckSurface(pstSrc, pstSrcRect) < 0)
    {
        return HI_ERR_TDE_INVALID_PARA;
    }

    if (TdeOsiCheckSurface(pstDst, pstDstRect) < 0)
    {
        return HI_ERR_TDE_INVALID_PARA;
    }

    if ((pstSrcRect->u32Width != pstDstRect->u32Width)
        || (pstSrcRect->u32Height != pstDstRect->u32Height))
    {
        bResize = HI_TRUE;
        if (TdeOsiCheckResizePara(pstSrcRect->u32Width, pstSrcRect->u32Height,
                                  pstDstRect->u32Width, pstDstRect->u32Height) < 0)
        {
            return HI_ERR_TDE_MINIFICATION;
        }
    }
    
    
#if 0
    if(TDE2_COLOR_FMT_YCbCr422 == pstSrc->enColorFmt || TDE2_COLOR_FMT_YCbCr422 == pstDst->enColorFmt)
    {
        if( ((bDeflicker) && (pstSrcRect->u32Width != pstDstRect->u32Width))
            || ((pstSrcRect->u32Height != pstDstRect->u32Height) && ((pstSrcRect->u32Width != pstDstRect->u32Width))) )
        {
            TDE_TRACE(TDE_KERN_INFO, "It deos not support hf/vf/ff in YCbCr422 format!\n");
            return TDE_OPERATION_BUTT;
        }
    }
    if (TdeOsiCheckYc422RPara(pstSrc, pstSrcRect, pstDst, pstDstRect, bDeflicker, bResize) < 0)
    {
        TDE_TRACE(TDE_KERN_INFO, "It deos not support hf/vf/ff in YCbCr422 format!\n");
        return HI_ERR_TDE_UNSUPPORTED_OPERATION;
    }
#endif

   
    TdeHalNodeInitNd(&stHWNode, HI_FALSE);

    stOpt.bResize = bResize;
    stOpt.enDeflickerMode = (bDeflicker)?TDE2_DEFLICKER_MODE_BOTH:TDE2_DEFLICKER_MODE_NONE;
    
    if (TdeOsiGetScanInfo(pstSrc, pstSrcRect, pstDst, pstDstRect, &stOpt, &stSrcScanInfo,
                          &stDstScanInfo) < 0)
    {
        return HI_ERR_TDE_INVALID_PARA;
    }

   
    TdeOsiConvertSurface(pstSrc, pstSrcRect, &stSrcScanInfo, &stSrcDrvSurface, &stSrcOptArea);
    TDE_TRACE(TDE_KERN_DEBUG, "stSrcScanInfo.enVScan:%d, stSrcDrvSurface.enHScan:%d\n",\
        stSrcScanInfo.enVScan, stSrcDrvSurface.enHScan);
    TDE_TRACE(TDE_KERN_DEBUG, "stSrcOptArea.s32XPos:%d, stSrcOptArea.s32YPos:%d, \
        stSrcOptArea.u32Width:%d, stSrcOptArea.u32Height:%d\n", stSrcOptArea.s32Xpos,\
        stSrcOptArea.s32Ypos, stSrcOptArea.u32Width, stSrcOptArea.u32Height);

   
    TdeOsiConvertSurface(pstDst, pstDstRect, &stDstScanInfo, &stDstDrvSurface, &stDstOptArea);
    TDE_TRACE(TDE_KERN_DEBUG, "stDstScanInfo.enVScan:%d, stDstScanInfo.enHScan:%d\n",\
        stDstScanInfo.enVScan, stDstScanInfo.enHScan);
    TDE_TRACE(TDE_KERN_DEBUG, "stDstOptArea.s32XPos:%d, stDstOptArea.s32YPos:%d, \
        stDstOptArea.u32Width:%d, stDstOptArea.u32Height:%d\n", stDstOptArea.s32Xpos,\
        stDstOptArea.s32Ypos, stDstOptArea.u32Width, stDstOptArea.u32Height);

   
    TdeHalNodeSetTgt(&stHWNode, &stDstDrvSurface, TDE2_OUTALPHA_FROM_NORM);

    TdeOsiSetExtAlpha(pstDst, pstSrc, &stHWNode); /* AI7D02681 */

    if (!bDeflicker && !bResize && (pstSrc->enColorFmt == pstDst->enColorFmt))   
    {
       
        TdeHalNodeSetSrc1(&stHWNode, &stSrcDrvSurface);

        
        TdeHalNodeSetBaseOperate(&stHWNode, TDE_QUIKE_COPY, TDE_SRC1_BYPASS, 0);

        
        return TdeOsiSetNodeFinish(s32Handle, &stHWNode, 0, TDE_NODE_SUBM_ALONE);
    }
    else    
    {
        
        TdeHalNodeSetSrc2(&stHWNode, &stSrcDrvSurface);

        
        u16Code = TdeOsiSingleSrc2GetOptCode(pstSrc->enColorFmt, pstDst->enColorFmt);

        TdeOsiGetConvbyCode(u16Code, &stConv);

       
        TdeHalNodeSetColorConvert(&stHWNode, &stConv);

        
        TdeHalNodeSetBaseOperate(&stHWNode, TDE_NORM_BLIT_1OPT, TDE_ALU_NONE, 0);

       
        if (bDeflicker || bResize)
	    {
        	
        	memcpy(&stFilterOpt.stSrcDire, &stSrcScanInfo, sizeof(TDE_SCANDIRECTION_S));
       	    memcpy(&stFilterOpt.stDstDire, &stDstScanInfo, sizeof(TDE_SCANDIRECTION_S));
        	stFilterOpt.enFilterMode = TDE_DRV_FILTER_ALL;
        
        	return TdeOsiSetFilterNode(s32Handle, &stHWNode, pstSrc, pstDst, 
                                   &stSrcOptArea, &stDstOptArea, stOpt.enDeflickerMode, &stFilterOpt);
    	}

	return TdeOsiSetNodeFinish(s32Handle, &stHWNode, 0, TDE_NODE_SUBM_ALONE);
    }
}

/*****************************************************************************
* Function:      TdeOsiQuickCopy
* Description:    quick blit source to target, no any functional operate, the size of source and target are the same
*                format is not MB format
* Input:         pSrc: source bitmap info
*                pstDst:  target bitmap info
*                pFuncComplCB: callback function pointer when operate is completed
				when it is null, it say to need not notice operate is over
* Output:        none
* Return:        none
* Others:        none
*****************************************************************************/
HI_S32     TdeOsiQuickCopy(TDE_HANDLE s32Handle, TDE2_SURFACE_S* pstSrc, TDE2_RECT_S  *pstSrcRect,
                           TDE2_SURFACE_S* pstDst, TDE2_RECT_S *pstDstRect)
{
    TDE_HWNode_S stHWNode = {0};
    TDE_DRV_SURFACE_S stSrcDrvSurface = {0};
    TDE_DRV_SURFACE_S stDstDrvSurface = {0};
    TDE_SCANDIRECTION_S stSrcScanInfo = {0};
    TDE_SCANDIRECTION_S stDstScanInfo = {0};
    
    if ((NULL == pstSrc) || (NULL == pstDst) || (NULL == pstSrcRect) || (NULL == pstDstRect))
    {
        TDE_TRACE(TDE_KERN_INFO, "NULL ptr!\n");
        return HI_ERR_TDE_NULL_PTR;
    }

    
    TDE_CHECK_NOT_MB(pstSrc->enColorFmt);
    TDE_CHECK_NOT_MB(pstDst->enColorFmt);

    if(pstSrc->enColorFmt != pstDst->enColorFmt)
    {
        TDE_TRACE(TDE_KERN_INFO, "color format is different!\n");
        return HI_ERR_TDE_INVALID_PARA;
    }
   
    if (TdeOsiCheckSurface(pstSrc, pstSrcRect) < 0)
    {
        return HI_ERR_TDE_INVALID_PARA;
    }

    
    if (TdeOsiCheckSurface(pstDst, pstDstRect) < 0)
    {
        return HI_ERR_TDE_INVALID_PARA;
    }


  
    TDE_UNIFY_RECT(pstSrcRect, pstDstRect);

   
    TdeHalNodeInitNd(&stHWNode, HI_FALSE);

    
    if (TdeOsiGetScanInfo(pstSrc, pstSrcRect, pstDst, pstDstRect, HI_NULL, &stSrcScanInfo,
                          &stDstScanInfo) < 0)
    {
        return HI_ERR_TDE_INVALID_PARA;
    }

    
    TdeOsiConvertSurface(pstSrc, pstSrcRect, &stSrcScanInfo, &stSrcDrvSurface, NULL);

   
    TdeOsiConvertSurface(pstDst, pstDstRect, &stDstScanInfo, &stDstDrvSurface, NULL);

    
    TdeHalNodeSetTgt(&stHWNode, &stDstDrvSurface, TDE2_OUTALPHA_FROM_BACKGROUND);

    
    TdeHalNodeSetSrc1(&stHWNode, &stSrcDrvSurface);

    
    TdeHalNodeSetBaseOperate(&stHWNode, TDE_QUIKE_COPY, TDE_SRC1_BYPASS, 0);

    
    return TdeOsiSetNodeFinish(s32Handle, &stHWNode, 0, TDE_NODE_SUBM_ALONE);
}
EXPORT_SYMBOL(TdeOsiQuickCopy);

/*****************************************************************************
* Function:      TdeOsiQuickResize
* Description:   zoom the size of source bitmap to the size aasigned by target bitmap, of which source and target can be the same
* Input:         pSrc: source bitmap info struct
*                pDst: target bitmap info struct
*                pFuncComplCB: callback function pointer when operate is over;if null, to say to no need to notice
* Output:        none
* Return:        none
* Others:        AI7D02579  add support for YCbCr422
*****************************************************************************/
HI_S32     TdeOsiQuickResize(TDE_HANDLE s32Handle, TDE2_SURFACE_S* pstSrc, TDE2_RECT_S  *pstSrcRect,
                             TDE2_SURFACE_S* pstDst, TDE2_RECT_S  *pstDstRect)
{
    HI_S32 s32Ret = HI_FAILURE;
    TDE2_OPT_S stOpt = {0};

    stOpt.bResize = HI_TRUE;

    if ((s32Ret = TdeOsiYC422TmpOpt(s32Handle, pstSrc, pstSrcRect,
                                    pstDst, pstDstRect, &stOpt)) < 0 )
    {
        return s32Ret;
    }
		
    if ((s32Ret = TdeOsiQuickBitblit(s32Handle, pstSrc, pstSrcRect, 
                                     pstDst, pstDstRect, HI_FALSE)) < 0)
    {
        return s32Ret;
    }
    return HI_SUCCESS;
}
EXPORT_SYMBOL(TdeOsiQuickResize);

/*****************************************************************************
* Function:      TdeOsiQuickFlicker
* Description:   deflicker source bitmap,output to target bitmap,source and target can be the same
* Input:         pSrc: source bitmap info struct
*                pDst: terget bitmap info struct
*                pFuncComplCB: callback function pointer when operate is over;if null, to say to no need to notice
* Output:        none
* Return:        none
* Others:        none
*****************************************************************************/
HI_S32     TdeOsiQuickFlicker(TDE_HANDLE s32Handle, TDE2_SURFACE_S* pstSrc, TDE2_RECT_S  *pstSrcRect,
                              TDE2_SURFACE_S* pstDst, TDE2_RECT_S  *pstDstRect)
{
    HI_S32 s32Ret = HI_FAILURE;
    TDE2_OPT_S stOpt = {0};
    HI_BOOL bDeflicker;

    stOpt.enDeflickerMode = TDE2_DEFLICKER_MODE_BOTH;

    if ((s32Ret = TdeOsiYC422TmpOpt(s32Handle, pstSrc, pstSrcRect,
                                    pstDst, pstDstRect, &stOpt)) < 0 )
    {
        return s32Ret;
    }

    bDeflicker = (stOpt.enDeflickerMode == TDE2_DEFLICKER_MODE_NONE)?HI_FALSE:HI_TRUE;

    if ((s32Ret = TdeOsiQuickBitblit(s32Handle, pstSrc, pstSrcRect, 
                                     pstDst, pstDstRect, bDeflicker)) < 0)
    {
        return s32Ret;
    }
    return HI_SUCCESS;
}
EXPORT_SYMBOL(TdeOsiQuickFlicker);

/*****************************************************************************
* Function:      TdeOsiQuickFill
* Description:   quick fill fixed value to target bitmap, fill value is refered to target bitmap
* Input:         pstDst: target bitmap info struct
*                u32FillData: fill value
*                pFuncComplCB: callback function pointer when operate is over;if null, to say to no need to notice
* Output:        none
* Return:        none
* Others:        none
*****************************************************************************/
HI_S32     TdeOsiQuickFill(TDE_HANDLE s32Handle, TDE2_SURFACE_S* pstDst, TDE2_RECT_S *pstDstRect,
                           HI_U32 u32FillData)
{
    TDE2_FILLCOLOR_S stFillColor;

    stFillColor.enColorFmt   = pstDst->enColorFmt;
    stFillColor.u32FillColor = u32FillData;

    return TdeOsi1SourceFill(s32Handle, pstDst, pstDstRect, &stFillColor, NULL);
}
EXPORT_SYMBOL(TdeOsiQuickFill);

/*****************************************************************************
* Function:      TdeOsiSingleSrc1Blit
* Description:   source1 operate realization
* Input:         pstBackGround: source1 bitmap info struct
*                pstBackGroundRect: source1 bitmap operate zone
*                pstDst: target bitmap info struct
*                pstDstRect: target bitmap operate zone
* Output:        none
* Return:        success/fail
* Others:        none
*****************************************************************************/
STATIC HI_S32 TdeOsiSingleSrc1Blit(TDE_HANDLE s32Handle, TDE2_SURFACE_S* pstBackGround, TDE2_RECT_S  *pstBackGroundRect,
                                   TDE2_SURFACE_S* pstDst, TDE2_RECT_S  *pstDstRect)
{
    TDE_HWNode_S stHWNode = {0};
    TDE_DRV_SURFACE_S stSrcDrvSurface = {0};
    TDE_DRV_SURFACE_S stDstDrvSurface = {0};
    TDE_SCANDIRECTION_S stSrcScanInfo = {0};
    TDE_SCANDIRECTION_S stDstScanInfo = {0};

    if ((NULL == pstBackGround) || (NULL == pstBackGroundRect)
        || (NULL == pstDst) || (NULL == pstDstRect))
    {
        return HI_ERR_TDE_NULL_PTR;
    }
   
    TdeHalNodeInitNd(&stHWNode, HI_FALSE);

    
    TdeHalNodeSetBaseOperate(&stHWNode, TDE_QUIKE_COPY, TDE_SRC1_BYPASS, 0);

    
    if (TdeOsiGetScanInfo(pstBackGround, pstBackGroundRect, pstDst, pstDstRect, HI_NULL,
                          &stSrcScanInfo,
                          &stDstScanInfo) < 0)
    {
        return HI_ERR_TDE_INVALID_PARA;
    }

   
    TdeOsiConvertSurface(pstBackGround, pstBackGroundRect, &stSrcScanInfo, &stSrcDrvSurface, NULL);

   
    TdeHalNodeSetSrc1(&stHWNode, &stSrcDrvSurface);

    TdeOsiSetExtAlpha(pstBackGround, HI_NULL, &stHWNode); /* AI7D02681 */

    
    TdeOsiConvertSurface(pstDst, pstDstRect, &stDstScanInfo, &stDstDrvSurface, NULL);

    
    TdeHalNodeSetTgt(&stHWNode, &stDstDrvSurface, TDE2_OUTALPHA_FROM_NORM);

    
    return TdeOsiSetNodeFinish(s32Handle, &stHWNode, 0, TDE_NODE_SUBM_ALONE);
}

/*****************************************************************************
* Function:      TdeOsiSingleSrc2Blit
* Description:   source2 operate realization
* Input:         pstBackGround: source2 bitmap info struct
*                pstBackGroundRect: source2 bitmap operate zone
*                pstDst: target bitmap info struct
*                pstDstRect: target bitmap operate zone
* Output:        none
* Return:        success/fail
* Others:        none
*****************************************************************************/
STATIC HI_S32 TdeOsiSingleSrc2Blit(TDE_HANDLE s32Handle,
                                                                TDE2_SURFACE_S* pstForeGround,
                                                                TDE2_RECT_S  *pstForeGroundRect,
                                                                TDE2_SURFACE_S* pstDst,
                                                                TDE2_RECT_S  *pstDstRect,
                                                                TDE2_OPT_S* pstOpt)
{
    TDE_HWNode_S stHWNode = {0};
    TDE_DRV_SURFACE_S stSrcDrvSurface = {0};
    TDE_DRV_SURFACE_S stDstDrvSurface = {0};
    TDE_SCANDIRECTION_S stSrcScanInfo = {0};
    TDE_SCANDIRECTION_S stDstScanInfo = {0};
    HI_U16 u16Code = 0;
    TDE_DRV_CONV_MODE_CMD_S stConv = {0};
    TDE2_RECT_S stFGOptArea = {0};
    TDE2_RECT_S stDstOptArea = {0};
    TDE_FILTER_OPT stFilterOpt = {0};
    TDE2_RECT_S stForeRect = {0};
    TDE2_RECT_S stDstRect = {0};
    HI_S32 s32Ret = HI_FAILURE;

    TDE_CLUT_USAGE_E enClutUsage = TDE_CLUT_USAGE_BUTT;

#if defined (TDE_VERSION_PILOT) || defined (TDE_VERSION_FPGA) 
	TDE2_SURFACE_S stTempForeGround;
#endif
    TDE_ASSERT(NULL != pstOpt);

    if ((NULL == pstDst) || (NULL == pstDstRect) || (NULL == pstOpt)
        || (NULL == pstForeGround) || (NULL == pstForeGroundRect))
    {
        return HI_ERR_TDE_NULL_PTR;
    }
    
    if (TdeOsiCheckSingleSrc2Opt(pstForeGround->enColorFmt, pstDst->enColorFmt, pstOpt) < 0)
    {
        return HI_ERR_TDE_INVALID_PARA;
    }

    if(pstOpt->bResize)
    {
        if (TdeOsiCheckResizePara(pstForeGroundRect->u32Width, pstForeGroundRect->u32Height,
                                  pstDstRect->u32Width, pstDstRect->u32Height) < 0)
        {
            return HI_ERR_TDE_MINIFICATION;
        }
    }

    memcpy(&stForeRect, pstForeGroundRect, sizeof(TDE2_RECT_S));
    memcpy(&stDstRect, pstDstRect, sizeof(TDE2_RECT_S));
   
    TdeHalNodeInitNd(&stHWNode, HI_FALSE);

    if(TdeOsiSetClipPara(NULL, NULL, pstForeGround, &stForeRect, pstDst,
                      &stDstRect, pstOpt, &stHWNode) < 0)
    {
        return HI_ERR_TDE_CLIP_AREA;
    }

    
    if (TdeOsiGetScanInfo(pstForeGround, &stForeRect, pstDst, &stDstRect, pstOpt,
                          &stSrcScanInfo, &stDstScanInfo) < 0)
    {
        return HI_ERR_TDE_INVALID_PARA;
    }

   
    TdeOsiConvertSurface(pstForeGround, &stForeRect, &stSrcScanInfo, &stSrcDrvSurface, &stFGOptArea);

   
    TdeHalNodeSetSrc2(&stHWNode, &stSrcDrvSurface);

   
    TdeOsiConvertSurface(pstDst, &stDstRect, &stDstScanInfo, &stDstDrvSurface, &stDstOptArea);

    
    TDE_CHECK_OUTALPHAFROM(pstOpt->enOutAlphaFrom);

    if(TDE2_OUTALPHA_FROM_BACKGROUND == pstOpt->enOutAlphaFrom)
    {
        TDE_TRACE(TDE_KERN_INFO, "enOutAlphaFrom error!\n");
        return HI_ERR_TDE_INVALID_PARA;
        
    }

   
    TdeHalNodeSetTgt(&stHWNode, &stDstDrvSurface, pstOpt->enOutAlphaFrom);

   
    u16Code = TdeOsiSingleSrc2GetOptCode(pstForeGround->enColorFmt, pstDst->enColorFmt);

    TdeOsiGetConvbyCode(u16Code, &stConv);

    TdeHalNodeSetColorConvert(&stHWNode, &stConv);

   
    if((s32Ret = TdeOsiSetClutOpt(pstForeGround, pstDst, &enClutUsage, pstOpt->bClutReload, &stHWNode)) < 0)
    {
        return s32Ret;
    }

    TdeOsiSetBaseOptParaForBlit(pstOpt, HI_NULL, pstForeGround,TDE_OPERATION_SINGLE_SRC2, &stHWNode);

    TdeOsiSetExtAlpha(HI_NULL, pstForeGround, &stHWNode); /* AI7D02681 */

    if ((pstOpt->bResize) || (pstOpt->enDeflickerMode != TDE2_DEFLICKER_MODE_NONE))
    {
        
        memcpy(&stFilterOpt.stSrcDire, &stSrcScanInfo, sizeof(TDE_SCANDIRECTION_S));
        memcpy(&stFilterOpt.stDstDire, &stDstScanInfo, sizeof(TDE_SCANDIRECTION_S));

        stFilterOpt.enFilterMode = g_enTdeFilterMode[pstOpt->enFilterMode];

#if defined (TDE_VERSION_PILOT) || defined (TDE_VERSION_FPGA) 
	memcpy(&stTempForeGround, pstForeGround, sizeof(TDE2_SURFACE_S));
	if (pstForeGround->enColorFmt == TDE2_COLOR_FMT_JPG_YCbCr422MBHP ||
		(pstForeGround->enColorFmt >= TDE2_COLOR_FMT_MP1_YCbCr420MBP && pstForeGround->enColorFmt <= TDE2_COLOR_FMT_JPG_YCbCr420MBP)
		|| (pstForeGround->enColorFmt == TDE2_COLOR_FMT_JPG_YCbCr400MBP))
	{
		stTempForeGround.enColorFmt = TDE2_COLOR_FMT_YCbCr422;
	}
	else if (pstForeGround->enColorFmt == TDE2_COLOR_FMT_JPG_YCbCr422MBVP
		|| pstForeGround->enColorFmt == TDE2_COLOR_FMT_JPG_YCbCr444MBP)
	{
		stTempForeGround.enColorFmt = TDE2_COLOR_FMT_YCbCr888;
	}

        
        return TdeOsiSetFilterNode(s32Handle, &stHWNode, &stTempForeGround, pstDst,
                                   &stFGOptArea, &stDstOptArea, pstOpt->enDeflickerMode, &stFilterOpt);
#else
       
        return TdeOsiSetFilterNode(s32Handle, &stHWNode, pstForeGround, pstDst,
               &stFGOptArea, &stDstOptArea, pstOpt->enDeflickerMode, &stFilterOpt);
#endif
    }
    else
    {
       
        return TdeOsiSetNodeFinish(s32Handle, &stHWNode, 0, TDE_NODE_SUBM_ALONE);
    }
    return HI_SUCCESS;
}

/*****************************************************************************
* Function:      TdeOsiDoubleSrcBlit
* Description:   dual source operate realization
* Input:         pstBackGround: source1 bitmap info struct
*                pstBackGroundRect: source1 bitmap operate zone
*                pstForeGround: source2 bitmap info struct
*                pstForeGroundRect: source2 bitmap operate zone
*                pstDst: target bitmap info struct
*                pstDstRect: target bitmap operate zone
*                pstOpt:config parameter
* Output:        none
* Return:        success/fail
* Others:        AI7D02579 add support for YCbCr422
*****************************************************************************/
STATIC HI_S32 TdeOsiDoubleSrcBlit(TDE_HANDLE s32Handle, TDE2_SURFACE_S* pstBackGround, TDE2_RECT_S  *pstBackGroundRect,
                                  TDE2_SURFACE_S* pstForeGround, TDE2_RECT_S  *pstForeGroundRect,
                                  TDE2_SURFACE_S* pstDst,
                                  TDE2_RECT_S  *pstDstRect,
                                  TDE2_OPT_S* pstOpt)
{
    TDE_HWNode_S stHWNode = {0};
    TDE_DRV_SURFACE_S stSrcDrvSurface = {0};
    TDE_DRV_SURFACE_S stDstDrvSurface = {0};
    TDE_SCANDIRECTION_S stSrcScanInfo = {0};
    TDE_SCANDIRECTION_S stDstScanInfo = {0};
    HI_U16 u16Code = 0;
    TDE2_RECT_S stBGOptArea = {0};
    TDE2_RECT_S stFGOptArea = {0};
    TDE2_RECT_S stDstOptArea = {0};
    TDE_FILTER_OPT stFilterOpt = {0};
    HI_S32 s32Ret = HI_FAILURE;
    TDE_DRV_CONV_MODE_CMD_S stConv = {0};
    TDE2_SURFACE_S stTempSur = {0};
    TDE2_RECT_S stForeRect = {0};
    TDE2_RECT_S stDstRect = {0};
    TDE2_RECT_S stBackRect = {0};

    TDE_CLUT_USAGE_E enClutUsage = TDE_CLUT_USAGE_BUTT;
    
#if defined (TDE_VERSION_PILOT) || defined (TDE_VERSION_FPGA) 
	TDE2_SURFACE_S stTempForeGround;
#endif

    if ((NULL == pstBackGround) || (NULL == pstBackGroundRect)
        || (NULL == pstForeGround) || (NULL == pstForeGroundRect)
        || (NULL == pstDst) || (NULL == pstDstRect)
        || (NULL == pstOpt))
    {
        return HI_ERR_TDE_NULL_PTR;
    }

    memcpy(&stForeRect, pstForeGroundRect, sizeof(TDE2_RECT_S));
    memcpy(&stBackRect, pstBackGroundRect, sizeof(TDE2_RECT_S));
    memcpy(&stDstRect, pstDstRect, sizeof(TDE2_RECT_S));
    if (TdeOsiCheckDoubleSrcOpt(pstBackGround->enColorFmt, pstForeGround->enColorFmt, pstDst->enColorFmt,
                                pstOpt) < 0)
    {
        return HI_ERR_TDE_INVALID_PARA;
    }
    
    if(pstOpt->bResize)
    {
        if (TdeOsiCheckResizePara(stForeRect.u32Width, stForeRect.u32Height,
                                  stDstRect.u32Width, stDstRect.u32Height) < 0)
        {
            return HI_ERR_TDE_MINIFICATION;
        }
    }

     if (bTdeHalSwVersion()
        && (TDE2_COLOR_FMT_YCbCr888 <= pstBackGround->enColorFmt)
        && (pstBackGround->enColorFmt <= TDE2_COLOR_FMT_YCbCr422)
        && (TDE2_COLOR_FMT_CLUT1 <= pstForeGround->enColorFmt)
        && (pstForeGround->enColorFmt <= TDE2_COLOR_FMT_ACLUT88)
        && (TDE2_COLORKEY_MODE_FOREGROUND == pstOpt->enColorKeyMode))
    {
        TDE2_OPT_S stOpt = {0};

        memcpy(&stOpt.stClipRect, &pstOpt->stClipRect, sizeof(TDE2_RECT_S));
        stOpt.enClipMode = pstOpt->enClipMode;
        
        memcpy(&stTempSur, pstDst, sizeof(TDE2_SURFACE_S));
        if (pstDst->enColorFmt == TDE2_COLOR_FMT_YCbCr888)
        {
            stTempSur.enColorFmt = TDE2_COLOR_FMT_RGB888;
        }
        else if (pstDst->enColorFmt == TDE2_COLOR_FMT_AYCbCr8888)
        {
            stTempSur.enColorFmt = TDE2_COLOR_FMT_ARGB8888;
        }
        else if (pstDst->enColorFmt == TDE2_COLOR_FMT_YCbCr422)
        {
            stTempSur.enColorFmt = TDE2_COLOR_FMT_ARGB4444;
        }

        memcpy(&stOpt.stCscOpt, &pstOpt->stCscOpt, sizeof(TDE2_CSC_OPT_S));
        
        s32Ret = TdeOsiSingleSrc2Blit(s32Handle, pstBackGround, &stBackRect, &stTempSur, &stDstRect, &stOpt);
        if (s32Ret < 0)
        {
            return s32Ret;
        }
    }
    
    
    TdeHalNodeInitNd(&stHWNode, HI_FALSE);

   
    if(TdeOsiSetClipPara(pstBackGround, &stBackRect, pstForeGround, &stForeRect, pstDst,
                      &stDstRect, pstOpt, &stHWNode) < 0)
    {
        return HI_ERR_TDE_CLIP_AREA;
    }

   
    if (TdeOsiGetScanInfo(pstForeGround, &stForeRect, pstDst, &stDstRect, pstOpt,
                          &stSrcScanInfo,
                          &stDstScanInfo) < 0)
    {
        return HI_ERR_TDE_INVALID_PARA;
    }

   
    TdeOsiConvertSurface(pstBackGround, &stBackRect, &stSrcScanInfo, &stSrcDrvSurface, &stBGOptArea);

    if (bTdeHalSwVersion()
        && (TDE2_COLOR_FMT_YCbCr888 <= pstBackGround->enColorFmt)
        && (pstBackGround->enColorFmt <= TDE2_COLOR_FMT_YCbCr422)
        && (TDE2_COLOR_FMT_CLUT1 <= pstForeGround->enColorFmt)
        && (pstForeGround->enColorFmt <= TDE2_COLOR_FMT_ACLUT88)
        && (TDE2_COLORKEY_MODE_FOREGROUND == pstOpt->enColorKeyMode))
    {
       
        memcpy(&stBackRect, &stDstRect, sizeof(TDE2_RECT_S));
        TdeOsiConvertSurface(&stTempSur, &stBackRect, &stSrcScanInfo, &stSrcDrvSurface, &stBGOptArea);
    }
    else
    {
        memcpy(&stTempSur, pstBackGround, sizeof(TDE2_SURFACE_S));
    }

    
    TdeHalNodeSetSrc1(&stHWNode, &stSrcDrvSurface);

    
    TdeOsiConvertSurface(pstForeGround, &stForeRect, &stSrcScanInfo, &stSrcDrvSurface, &stFGOptArea);

   
    TdeHalNodeSetSrc2(&stHWNode, &stSrcDrvSurface);

    
    TdeOsiConvertSurface(pstDst, &stDstRect, &stDstScanInfo, &stDstDrvSurface, &stDstOptArea);

    
    TDE_CHECK_OUTALPHAFROM(pstOpt->enOutAlphaFrom);

    
    TdeHalNodeSetTgt(&stHWNode, &stDstDrvSurface, pstOpt->enOutAlphaFrom);

    
    u16Code = TdeOsiDoubleSrcGetOptCode(stTempSur.enColorFmt, pstForeGround->enColorFmt,
                                        pstDst->enColorFmt);

    TdeOsiGetConvbyCode(u16Code, &stConv);

    
    TdeHalNodeSetColorConvert(&stHWNode, &stConv);

    
    if((s32Ret = TdeOsiSetClutOpt(pstForeGround, pstDst, &enClutUsage, pstOpt->bClutReload, &stHWNode)) < 0)
    {
        return s32Ret;
    }

   
    if (TDE2_COLORKEY_MODE_NONE != pstOpt->enColorKeyMode)
    {
        TDE_DRV_COLORKEY_CMD_S stColorkey;
        TDE_COLORFMT_CATEGORY_E enFmtCategory;
        stColorkey.unColorKeyValue = pstOpt->unColorKeyValue;

        switch (pstOpt->enColorKeyMode)
        {
        case TDE2_COLORKEY_MODE_BACKGROUND:
            stColorkey.enColorKeyMode = TDE_DRV_COLORKEY_BACKGROUND;

            
            enFmtCategory = TdeOsiGetFmtCategory(pstBackGround->enColorFmt);
            if (enFmtCategory >= TDE_COLORFMT_CATEGORY_BUTT)
            {
                TDE_TRACE(TDE_KERN_ERR, "Unknown fmt category!\n");
                return HI_ERR_TDE_INVALID_PARA;
            }
            
            TdeHalNodeSetColorKey(&stHWNode, enFmtCategory, &stColorkey);

            break;

        case TDE2_COLORKEY_MODE_FOREGROUND:
            if (TDE_CLUT_COLOREXPENDING != enClutUsage
                && TDE_CLUT_CLUT_BYPASS != enClutUsage)
            {
               
                stColorkey.enColorKeyMode = TDE_DRV_COLORKEY_FOREGROUND_AFTER_CLUT;
            }
            else
            {
                stColorkey.enColorKeyMode = TDE_DRV_COLORKEY_FOREGROUND_BEFORE_CLUT;
            }

            
            enFmtCategory = TdeOsiGetFmtCategory(pstForeGround->enColorFmt);
            if (enFmtCategory >= TDE_COLORFMT_CATEGORY_BUTT)
            {
                TDE_TRACE(TDE_KERN_ERR, "Unknown fmt category!\n");
                return HI_ERR_TDE_INVALID_PARA;
            }

            TdeHalNodeSetColorKey(&stHWNode, enFmtCategory, &stColorkey);
            break;

        default:
            TDE_TRACE(TDE_KERN_INFO, "\n");
            return HI_ERR_TDE_INVALID_PARA;
        }
    }

    TdeOsiSetBaseOptParaForBlit(pstOpt, &stTempSur, pstForeGround, TDE_OPERATION_DOUBLE_SRC, &stHWNode);
    TdeOsiSetExtAlpha(&stTempSur, pstForeGround, &stHWNode);
    
    if ((pstOpt->bResize) || (TDE2_DEFLICKER_MODE_NONE != pstOpt->enDeflickerMode))
    {
        TDE_DOUBLESRC_ADJ_INFO_S stDSAdjInfo;
        stDSAdjInfo.bDoubleSource = HI_TRUE;
        stDSAdjInfo.s32DiffX = stBGOptArea.s32Xpos - stDstOptArea.s32Xpos;
        stDSAdjInfo.s32DiffY = stBGOptArea.s32Ypos - stDstOptArea.s32Ypos;

        
        memcpy(&stFilterOpt.stSrcDire, &stSrcScanInfo, sizeof(TDE_SCANDIRECTION_S));
        memcpy(&stFilterOpt.stDstDire, &stDstScanInfo, sizeof(TDE_SCANDIRECTION_S));

        
        memcpy(&stFilterOpt.stDSAdjInfo, &stDSAdjInfo, sizeof(TDE_DOUBLESRC_ADJ_INFO_S));

        stFilterOpt.enFilterMode = g_enTdeFilterMode[pstOpt->enFilterMode];     

#if defined (TDE_VERSION_PILOT) || defined (TDE_VERSION_FPGA) 
	memcpy(&stTempForeGround, pstForeGround, sizeof(TDE2_SURFACE_S));
	if (pstForeGround->enColorFmt == TDE2_COLOR_FMT_JPG_YCbCr422MBHP ||
		(pstForeGround->enColorFmt >= TDE2_COLOR_FMT_MP1_YCbCr420MBP && pstForeGround->enColorFmt <= TDE2_COLOR_FMT_JPG_YCbCr420MBP)
		|| (pstForeGround->enColorFmt == TDE2_COLOR_FMT_JPG_YCbCr400MBP))
	{
		stTempForeGround.enColorFmt = TDE2_COLOR_FMT_YCbCr422;
	}
	else if (pstForeGround->enColorFmt == TDE2_COLOR_FMT_JPG_YCbCr422MBVP
		|| pstForeGround->enColorFmt == TDE2_COLOR_FMT_JPG_YCbCr444MBP)
	{
		stTempForeGround.enColorFmt = TDE2_COLOR_FMT_YCbCr888;
	}
	
        
        return TdeOsiSetFilterNode(s32Handle, &stHWNode, &stTempForeGround, pstDst,
                                   &stFGOptArea, &stDstOptArea, pstOpt->enDeflickerMode, &stFilterOpt);
#else
        
        return TdeOsiSetFilterNode(s32Handle, &stHWNode, pstForeGround, pstDst,
               &stFGOptArea, &stDstOptArea, pstOpt->enDeflickerMode, &stFilterOpt);
#endif
    }
    else
    {
       
        return TdeOsiSetNodeFinish(s32Handle, &stHWNode, 0, TDE_NODE_SUBM_ALONE);
    }
}

/*****************************************************************************
* Function:      TdeOsiYC422TmpOpt
* Description:   if operate for YCbCr422 bitmap, add a "pass" temporary handle
* Input:         s32Handle: task handle
*                pstForeGround: foreground bitmap info struct
*                pstForeGroundRect: foreground bitmap operate zone
*                pstDst:  target bitmap info struct
*                pstDstRect: target bitmap operate zone
*                pstOpt:  operate parameter setting struct
* Output:        none
* Return:        =0: success, <0: error
* Others:        AI7D02579 add support for YCbCr422
*****************************************************************************/
STATIC HI_S32 TdeOsiYC422TmpOpt(TDE_HANDLE s32Handle, TDE2_SURFACE_S* pstForeGround, TDE2_RECT_S  *pstForeGroundRect,
                                TDE2_SURFACE_S* pstDst, TDE2_RECT_S  *pstDstRect, TDE2_OPT_S* pstOpt)
{
    TDE2_OPT_S stTmpOpt = {0};
    TDE2_SURFACE_S stTmpDst = {0};
    TDE2_RECT_S stTmpDstRect = {0};
    HI_S32 s32Ret = HI_FAILURE;
    HI_BOOL bDeflicker;

    if ((NULL == pstDst) || (NULL == pstDstRect) || (NULL == pstOpt)
        || (NULL == pstForeGround) || (NULL == pstForeGroundRect))
    {
        return HI_ERR_TDE_NULL_PTR;
    }
        
    if (((pstOpt->enDeflickerMode != TDE2_DEFLICKER_MODE_NONE)|| (pstOpt->bResize)) && (pstOpt->enMirror))
    {
        TDE_TRACE(TDE_KERN_INFO, "Could not support VF/FF/HF and Mirror\n!");
        return HI_ERR_TDE_UNSUPPORTED_OPERATION;
    }

    bDeflicker = (pstOpt->enDeflickerMode == TDE2_DEFLICKER_MODE_NONE)?HI_FALSE:HI_TRUE;
    
    if (0 == TdeOsiCheckYc422RPara(pstForeGround, pstForeGroundRect, pstDst, pstDstRect, 
                                   bDeflicker, pstOpt->bResize))
    {
        TDE_TRACE(TDE_KERN_DEBUG, "One Pass For Tmp Operation!\n");
        return 0;
    }
    TDE_TRACE(TDE_KERN_DEBUG, "Two Pass For Tmp Operation!\n");

   
    if (TdeOsiGetbppByFmt(pstDst->enColorFmt) < 16)
    {
        TDE_TRACE(TDE_KERN_INFO, "Could not output the format For YCbCr422R!\n");
        return HI_ERR_TDE_UNSUPPORTED_OPERATION;
    }

    s32Ret = TdeOsiCheckSurface(pstForeGround, pstForeGroundRect);
    if (s32Ret < 0)
    {
        return HI_ERR_TDE_INVALID_PARA;
    }

    s32Ret = TdeOsiCheckSurface(pstDst, pstDstRect);
    if (s32Ret < 0)
    {
        return HI_ERR_TDE_INVALID_PARA;
    }

    if (!pstOpt->bResize)
    {
        TDE_UNIFY_RECT(pstForeGroundRect, pstDstRect);
    }

    stTmpOpt.bResize = pstOpt->bResize;
    stTmpOpt.enDeflickerMode = pstOpt->enDeflickerMode;

    memcpy(&stTmpDst, pstDst, sizeof(TDE2_SURFACE_S));
    memcpy(&stTmpDstRect, pstDstRect, sizeof(TDE2_RECT_S));
    stTmpDst.enColorFmt = TDE2_COLOR_FMT_RGB565;
    stTmpDst.u32Stride = stTmpDst.u32Width * 2;

    TDE_TRACE(TDE_KERN_DEBUG, "tmp x:%d, y:%d, w:%d, h:%d\n", 
              stTmpDstRect.s32Xpos, stTmpDstRect.s32Ypos, stTmpDstRect.u32Width, stTmpDstRect.u32Height);

    if ((s32Ret = TdeOsiSingleSrc2Blit(s32Handle, pstForeGround, 
                  pstForeGroundRect, &stTmpDst, &stTmpDstRect, &stTmpOpt)) < 0)
    {
        return s32Ret;
    }

    memcpy(pstForeGround, &stTmpDst, sizeof(TDE2_SURFACE_S));
    memcpy(pstForeGroundRect, &stTmpDstRect, sizeof(TDE2_RECT_S));

    
    pstOpt->enDeflickerMode = TDE2_DEFLICKER_MODE_NONE;
    pstOpt->bResize = HI_FALSE;

    return 0;
}

/*****************************************************************************
* Function:      TdeOsiBlit
* Description:   operate pstBackGround with pstForeGround,which result output to pstDst,operate setting is in pstOpt
* Input:         enCmd: TDE operate command type
*                pstBackGround: background bitmap info struct
*                pstForeGround: foreground bitmap info struct
*                pstDst:  target bitmap info struct
*                pstOpt:  operate parameter setting struct
* Output:        none
* Return:        none
* Others:        none
*****************************************************************************/
HI_S32     TdeOsiBlit(TDE_HANDLE s32Handle, TDE2_SURFACE_S* pstBackGround, TDE2_RECT_S  *pstBackGroundRect,
                      TDE2_SURFACE_S* pstForeGround, TDE2_RECT_S  *pstForeGroundRect, TDE2_SURFACE_S* pstDst,
                      TDE2_RECT_S  *pstDstRect, TDE2_OPT_S* pstOpt)
{
    TDE_OPERATION_CATEGORY_E enOptCategory;
    HI_S32 s32Ret = 0;

    enOptCategory = TdeOsiGetOptCategory(pstBackGround, pstBackGroundRect, pstForeGround, pstForeGroundRect, pstDst,
                                         pstDstRect, pstOpt);

    switch (enOptCategory)
    {
    case TDE_OPERATION_SINGLE_SRC1:
    {
        if (NULL == pstBackGround)
        {
            return TdeOsiSingleSrc1Blit(s32Handle, pstForeGround, pstForeGroundRect,
                                        pstDst, pstDstRect);
        }
        else
        {         
            return TdeOsiSingleSrc1Blit(s32Handle, pstBackGround, pstBackGroundRect,
                                        pstDst, pstDstRect);
        }
        break;
    }
    case TDE_OPERATION_SINGLE_SRC2:
    {
        if (NULL == pstBackGround)
        {
            if ((s32Ret = TdeOsiYC422TmpOpt(s32Handle, pstForeGround, pstForeGroundRect,
                                        pstDst, pstDstRect, pstOpt)) < 0)
            {
                return s32Ret;
            }

            if ((s32Ret = TdeOsiSingleSrc2Blit(s32Handle, pstForeGround, pstForeGroundRect,
                                        pstDst, pstDstRect, pstOpt)) < 0)
            {
                return s32Ret;
            }
        }
        else
        {
            if ((s32Ret = TdeOsiYC422TmpOpt(s32Handle, pstBackGround, pstBackGroundRect,
                                        pstDst, pstDstRect, pstOpt)) < 0 )
            {
                return s32Ret;
            }

            if ((s32Ret = TdeOsiSingleSrc2Blit(s32Handle, pstBackGround, pstBackGroundRect,
                                        pstDst, pstDstRect, pstOpt)) < 0 ) /*AI7D02940*/
            {
                return s32Ret;
            }
        }
        break;
    }
    case TDE_OPERATION_DOUBLE_SRC:
    {
        if ((s32Ret = TdeOsiYC422TmpOpt(s32Handle, pstForeGround, pstForeGroundRect,
                                        pstDst, pstDstRect, pstOpt)) < 0)
        {
            return s32Ret;
        }
        
        if ((s32Ret = TdeOsiDoubleSrcBlit(s32Handle, pstBackGround, pstBackGroundRect, pstForeGround, pstForeGroundRect,
                                   pstDst, pstDstRect, pstOpt)) < 0)
        {
            return s32Ret;
        }
        break;
    }
    default:
        return HI_ERR_TDE_INVALID_PARA;
    }
    return HI_SUCCESS;
}
EXPORT_SYMBOL(TdeOsiBlit);

/*****************************************************************************
* Function:      TdeOsiMbBlit
* Description:   Mb blit 
* Input:         s32Handle: task handle
*                pstMB: MB bitmap information struct
*                pstMbRect: MB bitmap operate zone
*                pstDst: target bitmap information struct
*                pstDstRect:  target bitmap operate zone
*                pstMbOpt:  operate parameter setting struct
* Output:        none
* Return:        >0: return task id of current operate; <0: fail
* Others:        none
*****************************************************************************/
HI_S32     TdeOsiMbBlit(TDE_HANDLE s32Handle, TDE2_MB_S* pstMB, TDE2_RECT_S  *pstMbRect,
                        TDE2_SURFACE_S* pstDst, TDE2_RECT_S  *pstDstRect, TDE2_MBOPT_S* pstMbOpt)
{
    TDE_HWNode_S stHwNode = {0};

    if ((HI_NULL == pstMB) || (HI_NULL == pstMbRect) 
        || (HI_NULL == pstDst) || (HI_NULL == pstDstRect) || (HI_NULL == pstMbOpt))
    {
        TDE_TRACE(TDE_KERN_INFO, "Contains NULL ptr!\n");
        return HI_ERR_TDE_NULL_PTR;
    }

    
    #if 0
    if (TDE2_COLOR_FMT_YCbCr888 != pstDst->enColorFmt
        && TDE2_COLOR_FMT_AYCbCr8888 != pstDst->enColorFmt
        && TDE2_COLOR_FMT_YCbCr422 != pstDst->enColorFmt)
    {
        TDE_TRACE(TDE_KERN_INFO, "MacroBlock does not support CSC!\n");
        return HI_ERR_TDE_INVALID_PARA;
    }
    #endif

    TDE_CHECK_NOT_MB(pstDst->enColorFmt);
    TDE_CHECK_MBCOLORFMT(pstMB->enMbFmt);
    
    if (TdeOsiCheckSurface(pstDst, pstDstRect) < 0
        || TdeOsiCheckMbSurface(pstMB, pstMbRect) < 0)
    {
        return HI_ERR_TDE_INVALID_PARA;
    }

    if((pstDstRect->u32Height == pstMbRect->u32Height)
        &&(pstDstRect->u32Width == pstMbRect->u32Width))
    {
        pstMbOpt->enResize = TDE2_MBRESIZE_NONE;
    }

    if (TDE2_MBRESIZE_NONE == pstMbOpt->enResize)
    {
        TDE_UNIFY_RECT(pstMbRect, pstDstRect);
    }

    if (TdeOsiCheckResizePara(pstMbRect->u32Width, pstMbRect->u32Height,
                              pstDstRect->u32Width, pstDstRect->u32Height) < 0)
    {
        TDE_TRACE(TDE_KERN_INFO, "The Scale is too large!\n");
        return HI_ERR_TDE_MINIFICATION;
    }

    return TdeOsiSetMbPara(s32Handle, &stHwNode, pstMB, pstMbRect,
                           pstDst, pstDstRect, pstMbOpt);
}
EXPORT_SYMBOL(TdeOsiMbBlit);
#if HI_TDE_BITMAPMASK_SUPPORT
/*****************************************************************************
* Function:      TdeOsiBitmapMaskCheckPara
* Description:   check for trinal source operate parameter
* Input:         pstBackGround: background bitmap
*                pstBackGroundRect: background operate zone
*                pstForeGround: foreground bitmap
*                pstForeGroundRect: foreground operate zone
*                pstMask:  Mask bitmap
*                pstMaskRect:  Mask bitmap operate zone
*                pstDst:  target bitmap
*                pstDstRect: target bitmap operate zone
* Output:        none
* Return:        =0: success; <0: fail
* Others:        none
*****************************************************************************/
STATIC INLINE HI_S32 TdeOsiBitmapMaskCheckPara(TDE2_SURFACE_S* pstBackGround, TDE2_RECT_S  *pstBackGroundRect,
                                               TDE2_SURFACE_S* pstForeGround, TDE2_RECT_S  *pstForeGroundRect,
                                               TDE2_SURFACE_S* pstMask,
                                               TDE2_RECT_S  *pstMaskRect, TDE2_SURFACE_S* pstDst,
                                               TDE2_RECT_S  *pstDstRect)
{
    TDE_COLORFMT_CATEGORY_E enSrc1Category;
    TDE_COLORFMT_CATEGORY_E enDstCategory;

    if ((NULL == pstBackGround) || (NULL == pstBackGroundRect) || (NULL == pstForeGround)
        || (NULL == pstForeGroundRect)
        || (NULL == pstMask) || (NULL == pstMaskRect) || (NULL == pstDst) || (NULL == pstDstRect))
    {
        TDE_TRACE(TDE_KERN_INFO, "Contains NULL ptr!\n");
        return HI_ERR_TDE_NULL_PTR;
    }	

   
    TDE_CHECK_NOT_MB(pstForeGround->enColorFmt);
    TDE_CHECK_NOT_MB(pstBackGround->enColorFmt);
    TDE_CHECK_NOT_MB(pstMask->enColorFmt);
    TDE_CHECK_NOT_MB(pstDst->enColorFmt);

#ifdef TDE_VERSION_MPW
    if(TDE2_COLOR_FMT_YCbCr422 == pstBackGround->enColorFmt)
    {
        TDE_TRACE(TDE_KERN_INFO, "BackGround bitmap does not support YCbCr422 format!\n");
        return HI_ERR_TDE_INVALID_PARA;
    }
#endif

    
    enSrc1Category = TdeOsiGetFmtCategory(pstBackGround->enColorFmt);

    
    enDstCategory = TdeOsiGetFmtCategory(pstDst->enColorFmt);

    #if 0
    if (enSrc1Category != enDstCategory)
    {
        TDE_TRACE(TDE_KERN_INFO, "does not support csc!\n");
        return HI_ERR_TDE_INVALID_PARA;
    }
    #endif
    
   
    if (TdeOsiCheckSurface(pstBackGround, pstBackGroundRect) < 0)
    {
        TDE_TRACE(TDE_KERN_INFO, "pstBackGroundRect does not correct!\n");
        return HI_ERR_TDE_INVALID_PARA;
    }

   
    if (TdeOsiCheckSurface(pstForeGround, pstForeGroundRect) < 0)
    {
        TDE_TRACE(TDE_KERN_INFO, "pstForeGroundRect does not correct!\n");
        return HI_ERR_TDE_INVALID_PARA;
    }

    
    if (TdeOsiCheckSurface(pstMask, pstMaskRect) < 0)
    {
        TDE_TRACE(TDE_KERN_INFO, "pstMaskRect does not correct!\n");

        return HI_ERR_TDE_INVALID_PARA;
    }

   
    if (TdeOsiCheckSurface(pstDst, pstDstRect) < 0)
    {
        TDE_TRACE(TDE_KERN_INFO, "pstDstRect does not correct!\n");

        return HI_ERR_TDE_INVALID_PARA;
    }

    if ((pstBackGroundRect->u32Width != pstForeGroundRect->u32Width)
        || (pstForeGroundRect->u32Width != pstMaskRect->u32Width)
        || (pstMaskRect->u32Width != pstDstRect->u32Width)
        || (pstBackGroundRect->u32Height != pstForeGroundRect->u32Height)
        || (pstForeGroundRect->u32Height != pstMaskRect->u32Height)
        || (pstMaskRect->u32Height != pstDstRect->u32Height))
    {
        TDE_TRACE(TDE_KERN_INFO, "Don't support scale!\n");
        return HI_ERR_TDE_INVALID_PARA;
    }

    return HI_SUCCESS;
}

/*****************************************************************************
* Function:      TdeOsiBitmapMaskRop
* Description:   Ropmask foreground and mask at firs, and then ropmask background and middle bitmap
*                output result to target bitmap
* Input:         pstBackGround: background bitmap info struct
*                pstForeGround: foreground bitmap info struct
*                pMask: fask code operate bitmap info
*                pstDst:  target bitmap info
*                pFuncComplCB: callback function pointer when operate is over;if null, to say to no need to notice
* Output:        none
* Return:        none
* Others:        none
*****************************************************************************/
HI_S32     TdeOsiBitmapMaskRop(TDE_HANDLE s32Handle, 
                    TDE2_SURFACE_S* pstBackGround, TDE2_RECT_S  *pstBackGroundRect,
                    TDE2_SURFACE_S* pstForeGround, TDE2_RECT_S  *pstForeGroundRect,
                    TDE2_SURFACE_S* pstMask, TDE2_RECT_S  *pstMaskRect, 
                    TDE2_SURFACE_S* pstDst, TDE2_RECT_S  *pstDstRect,
                    TDE2_ROP_CODE_E enRopCode_Color, TDE2_ROP_CODE_E enRopCode_Alpha)
{
    TDE_HWNode_S stHWNode = {0};
    TDE_DRV_SURFACE_S stSrcDrvSurface = {0};
    TDE_DRV_SURFACE_S stDstDrvSurface = {0};
    TDE_SCANDIRECTION_S stScanInfo= {0};

    TDE2_SURFACE_S stMidSurface = {0};
    TDE2_RECT_S stMidRect = {0};
    HI_U16 u16Code = 0;
    TDE_DRV_CONV_MODE_CMD_S stConv = {0};
    
    HI_S32 ret;

    if ((ret = TdeOsiBitmapMaskCheckPara(pstBackGround, pstBackGroundRect,
                                         pstForeGround, pstForeGroundRect, pstMask,
                                         pstMaskRect, pstDst, pstDstRect)) < 0)
    {
        return ret;
    }

    if(!TdeOsiWhetherContainAlpha(pstForeGround->enColorFmt))
    {
        TDE_TRACE(TDE_KERN_INFO, "ForeGround bitmap must contains alpha component!\n");
        return HI_ERR_TDE_INVALID_PARA;
    }

    if (TDE2_COLOR_FMT_A1 != pstMask->enColorFmt)
    {
        TDE_TRACE(TDE_KERN_INFO, "Maskbitmap's colorformat can only be A1 in TdeOsiBitmapMaskRop!\n");
        return HI_ERR_TDE_INVALID_PARA;
    }

    
    TdeHalNodeInitNd(&stHWNode, HI_FALSE);

    
    stScanInfo.enHScan = TDE_SCAN_LEFT_RIGHT;
    stScanInfo.enVScan = TDE_SCAN_UP_DOWN;

   
    TdeOsiConvertSurface(pstForeGround, pstForeGroundRect, &stScanInfo, &stSrcDrvSurface, NULL);

   
    TdeHalNodeSetSrc1(&stHWNode, &stSrcDrvSurface);

    
    TdeOsiConvertSurface(pstMask, pstMaskRect, &stScanInfo, &stSrcDrvSurface, NULL);

    
    TdeHalNodeSetSrc2(&stHWNode, &stSrcDrvSurface);

    
    memcpy(&stMidSurface, pstForeGround, sizeof(TDE2_SURFACE_S));

    stMidSurface.u32PhyAddr = TdeOsiListGetPhyBuff(0);
    stMidSurface.pu8ClutPhyAddr = NULL;
    
    stMidRect.s32Xpos   = 0;
    stMidRect.s32Ypos   = 0;
    stMidRect.u32Height = pstForeGroundRect->u32Height;
    stMidRect.u32Width  = pstForeGroundRect->u32Width;

   
    TdeOsiConvertSurface(&stMidSurface, &stMidRect, &stScanInfo, &stDstDrvSurface, NULL);

    
    TdeHalNodeSetTgt(&stHWNode, &stDstDrvSurface, TDE2_OUTALPHA_FROM_NORM);

    TdeOsiSetExtAlpha(pstForeGround, &stMidSurface, &stHWNode);


    /* logical operation first passs */
    TdeHalNodeSetBaseOperate(&stHWNode, TDE_NORM_BLIT_2OPT, TDE_ALU_MASK_ROP1, 0);

    
    if ((ret = TdeOsiSetNodeFinish(s32Handle, &stHWNode, 0, TDE_NODE_SUBM_ALONE)) < 0)
    {
        tde_osr_disableirq();
        TdeOsiListPutPhyBuff(1);
        tde_osr_enableirq();
        return ret;
    }

    
    TdeHalNodeInitNd(&stHWNode, HI_FALSE);

    
    TdeOsiConvertSurface(pstBackGround, pstBackGroundRect, &stScanInfo, &stSrcDrvSurface, NULL);

    
    TdeHalNodeSetSrc1(&stHWNode, &stSrcDrvSurface);

    
    TdeHalNodeSetSrc2(&stHWNode, &stDstDrvSurface);

   
    TdeOsiConvertSurface(pstDst, pstDstRect, &stScanInfo, &stDstDrvSurface, NULL);

   
    TdeHalNodeSetTgt(&stHWNode, &stDstDrvSurface, TDE2_OUTALPHA_FROM_NORM);

   
    u16Code = TdeOsiSingleSrc2GetOptCode(pstForeGround->enColorFmt, pstDst->enColorFmt);

    TdeOsiGetConvbyCode(u16Code, &stConv);

    
    TdeHalNodeSetColorConvert(&stHWNode, &stConv);

    TdeHalNodeSetRop(&stHWNode, enRopCode_Color, enRopCode_Alpha);

    TdeOsiSetExtAlpha(pstBackGround, &stMidSurface, &stHWNode);

    /* logical operation second passs */
    TdeHalNodeSetBaseOperate(&stHWNode, TDE_NORM_BLIT_2OPT, TDE_ALU_MASK_ROP2, 0);

    
    ret = TdeOsiSetNodeFinish(s32Handle, &stHWNode, 1, TDE_NODE_SUBM_ALONE);
    if (ret < 0)
    {
        tde_osr_disableirq();
        TdeOsiListPutPhyBuff(1);
        tde_osr_enableirq();
        return ret;
    }
    
    return HI_SUCCESS;
}

EXPORT_SYMBOL(TdeOsiBitmapMaskRop);

/*****************************************************************************
* Function:      TdeOsiBitmapMaskBlend
* Description:   blendmask foreground and mask bitmap at first,and blend background with middle bitmap
*                output reslut to target bitmap
* Input:         pstBackGround: background bitmap info struct
*                pstForeGround: foreground bitmap info struct
*                pMask: fask code operate bitmap info
*                pstDst:  target bitmap info
*                u8Alpha:  alpha value operated
*                pFuncComplCB: callback function pointer when operate is over;if null, to say to no need to notice
* Output:        none
* Return:        none
* Others:        none
*****************************************************************************/
HI_S32     TdeOsiBitmapMaskBlend(TDE_HANDLE s32Handle, TDE2_SURFACE_S* pstBackGround,
                                 TDE2_RECT_S  *pstBackGroundRect,
                                 TDE2_SURFACE_S* pstForeGround, TDE2_RECT_S  *pstForeGroundRect,
                                 TDE2_SURFACE_S* pstMask,
                                 TDE2_RECT_S  *pstMaskRect, TDE2_SURFACE_S* pstDst, TDE2_RECT_S  *pstDstRect,
                                 HI_U8 u8Alpha,
                                 TDE2_ALUCMD_E enBlendMode)
{
    TDE_HWNode_S stHWNode = {0};
    TDE_DRV_SURFACE_S stSrcDrvSurface = {0};
    TDE_DRV_SURFACE_S stDstDrvSurface = {0};
    TDE_SCANDIRECTION_S stScanInfo = {0};
    TDE2_SURFACE_S stMidSurface = {0};
    TDE2_RECT_S stMidRect = {0};
    TDE_DRV_ALU_MODE_E enDrvAluMode = TDE_SRC1_BYPASS;
    HI_U16 u16Code = 0;
    TDE_DRV_CONV_MODE_CMD_S stConv = {0};
    TDE2_BLEND_OPT_S stBlendOpt = {0};

    HI_S32 ret;

    if ((ret = TdeOsiBitmapMaskCheckPara(pstBackGround, pstBackGroundRect,
                                         pstForeGround, pstForeGroundRect, pstMask,
                                         pstMaskRect, pstDst, pstDstRect)) < 0)
    {
        return ret;
    }

    if ((TDE2_COLOR_FMT_A1 != pstMask->enColorFmt)
        &&(TDE2_COLOR_FMT_A8 != pstMask->enColorFmt))
    {
        TDE_TRACE(TDE_KERN_INFO, "Maskbitmap's colorformat can only be An in TdeOsiBitmapMaskBlend!\n");
        return HI_ERR_TDE_INVALID_PARA;
    }

    if (TDE2_ALUCMD_BLEND == enBlendMode)
    {
        enDrvAluMode = TDE_ALU_BLEND;
    }
#if 0
    else if (TDE2_ALUCMD_BLEND_PREMUL == enBlendMode)
    {
        enDrvAluMode = TDE_ALU_BLEND_SRC2;
    }
#endif
    else
    {
        TDE_TRACE(TDE_KERN_INFO, "Alum mode can only be blending in TdeOsiBitmapMaskBlend!\n");
        return HI_ERR_TDE_INVALID_PARA;
    }

    	
    TdeHalNodeInitNd(&stHWNode, HI_FALSE);

    
    stScanInfo.enHScan = TDE_SCAN_LEFT_RIGHT;
    stScanInfo.enVScan = TDE_SCAN_UP_DOWN;

    
    TdeOsiConvertSurface(pstForeGround, pstForeGroundRect, &stScanInfo, &stSrcDrvSurface, NULL);

    
    TdeHalNodeSetSrc1(&stHWNode, &stSrcDrvSurface);

   
    TdeOsiConvertSurface(pstMask, pstMaskRect, &stScanInfo, &stSrcDrvSurface, NULL);

    
    TdeHalNodeSetSrc2(&stHWNode, &stSrcDrvSurface);

    
    memcpy(&stMidSurface, pstForeGround, sizeof(TDE2_SURFACE_S));

    stMidSurface.u32PhyAddr = TdeOsiListGetPhyBuff(0);

    stMidRect.s32Xpos   = 0;
    stMidRect.s32Ypos   = 0;
    stMidRect.u32Height = pstForeGroundRect->u32Height;
    stMidRect.u32Width  = pstForeGroundRect->u32Width;

    
    TdeOsiConvertSurface(&stMidSurface, &stMidRect, &stScanInfo, &stDstDrvSurface, NULL);

    
    TdeHalNodeSetTgt(&stHWNode, &stDstDrvSurface, TDE2_OUTALPHA_FROM_NORM);


    TdeOsiSetExtAlpha(pstForeGround, &stMidSurface, &stHWNode);

    
    TdeHalNodeSetBaseOperate(&stHWNode, TDE_NORM_BLIT_2OPT, TDE_ALU_MASK_BLEND, 0);

    
    TdeHalNodeSetGlobalAlpha(&stHWNode, 0xff, HI_TRUE);

    
    if ((ret = TdeOsiSetNodeFinish(s32Handle, &stHWNode, 0, TDE_NODE_SUBM_ALONE)) < 0)
    {
        tde_osr_disableirq();
        TdeOsiListPutPhyBuff(1);
        tde_osr_enableirq();
        return ret;
    }

    
    TdeHalNodeInitNd(&stHWNode, HI_FALSE);


    
    TdeOsiConvertSurface(pstBackGround, pstBackGroundRect, &stScanInfo, &stSrcDrvSurface, NULL);

    
    TdeHalNodeSetSrc1(&stHWNode, &stSrcDrvSurface);

    
    TdeHalNodeSetSrc2(&stHWNode, &stDstDrvSurface);

    
    TdeOsiConvertSurface(pstDst, pstDstRect, &stScanInfo, &stDstDrvSurface, NULL);

    
    TdeHalNodeSetTgt(&stHWNode, &stDstDrvSurface, TDE2_OUTALPHA_FROM_NORM);

    
    u16Code = TdeOsiSingleSrc2GetOptCode(pstForeGround->enColorFmt, pstDst->enColorFmt);

    TdeOsiGetConvbyCode(u16Code, &stConv);

    
    TdeHalNodeSetColorConvert(&stHWNode, &stConv);

    
    TdeHalNodeSetGlobalAlpha(&stHWNode, u8Alpha, HI_TRUE);

   
    TdeHalNodeSetBaseOperate(&stHWNode, TDE_NORM_BLIT_2OPT, enDrvAluMode, 0);

#if defined(TDE_VERSION_PILOT) || defined(TDE_VERSION_FPGA)
    stBlendOpt.bGlobalAlphaEnable = HI_TRUE;
    stBlendOpt.bPixelAlphaEnable = HI_TRUE;
    stBlendOpt.eBlendCmd = TDE2_BLENDCMD_NONE;
    TdeHalNodeSetBlend(&stHWNode, &stBlendOpt);
#endif
    
    TdeOsiSetExtAlpha(pstBackGround, &stMidSurface, &stHWNode);

    
    ret = TdeOsiSetNodeFinish(s32Handle, &stHWNode, 1, TDE_NODE_SUBM_ALONE);
    if (ret < 0)
    {
        TdeOsiListPutPhyBuff(1);
        return ret;
    }
    
    return HI_SUCCESS;
}
EXPORT_SYMBOL(TdeOsiBitmapMaskBlend);
#endif
/*****************************************************************************
* Function:      TdeOsiSolidDraw
* Description:   operate src1 with src2, which result to pstDst,operate setting is in pstOpt
*                if src is MB, only support single source operate, just to say to only support pstBackGround or pstForeGround
* Input:         s32Handle: task handle 
*                pSrc: background bitmap info struct
*                pstDst: foreground bitmap info struct
*                pstFillColor:  fill  color
*                pstOpt: operate parameter setting struct
* Output:        none
* Return:        HI_SUCCESS/HI_FAILURE
* Others:        none
*****************************************************************************/
HI_S32  TdeOsiSolidDraw(TDE_HANDLE s32Handle, TDE2_SURFACE_S* pstSrc, TDE2_RECT_S  *pstSrcRect,
                        TDE2_SURFACE_S *pstDst,
                        TDE2_RECT_S  *pstDstRect, TDE2_FILLCOLOR_S *pstFillColor,
                        TDE2_OPT_S *pstOpt)
{
    if (NULL == pstSrc)
    {
        return TdeOsi1SourceFill(s32Handle, pstDst, pstDstRect, pstFillColor, pstOpt);
    }
    else
    {
        HI_S32 s32Ret;

        if ((NULL == pstSrc) || (NULL == pstDst) || (NULL == pstFillColor))
        {
            return HI_ERR_TDE_NULL_PTR;
        }
        
        TDE_CHECK_NOT_MB(pstSrc->enColorFmt);
        TDE_CHECK_NOT_MB(pstDst->enColorFmt);
        TDE_CHECK_NOT_MB(pstFillColor->enColorFmt);
        if ((s32Ret = TdeOsiYC422TmpOpt(s32Handle, pstSrc, pstSrcRect,
                                        pstDst, pstDstRect, pstOpt)) < 0 )
        {
            return s32Ret;
        }
        return TdeOsi2SourceFill(s32Handle, pstSrc, pstSrcRect, pstDst, pstDstRect, pstFillColor, pstOpt);
    }
}
EXPORT_SYMBOL(TdeOsiSolidDraw);
#if 0
STATIC TDE_TRIG_SEL s_pFuncTrigSelCb = NULL;

HI_S32 TdeOsiTriggerSel(HI_U32 u32TrigSel)
{
    if(NULL != s_pFuncTrigSelCb)
    {
        s_pFuncTrigSelCb(u32TrigSel);
        return HI_SUCCESS;
    }
    else
    {
        return HI_FAILURE;
    }
}
EXPORT_SYMBOL(TdeOsiTriggerSel);

HI_S32 TdeOsiRegisterTrigFunc(TDE_TRIG_SEL pFuncTrigSelCb)
{
    s_pFuncTrigSelCb = pFuncTrigSelCb;
    return HI_SUCCESS;
}
EXPORT_SYMBOL(TdeOsiRegisterTrigFunc);
#endif

HI_S32 TdeOsiSetDeflickerLevel(TDE_DEFLICKER_LEVEL_E eDeflickerLevel)
{
    return TdeHalSetDeflicerLevel(eDeflickerLevel);
}
EXPORT_SYMBOL(TdeOsiSetDeflickerLevel);

HI_S32 TdeOsiGetDeflickerLevel(TDE_DEFLICKER_LEVEL_E *pDeflickerLevel)
{
    if (HI_NULL == pDeflickerLevel)
    {
        return HI_FAILURE;
    }
    return TdeHalGetDeflicerLevel(pDeflickerLevel);
}
EXPORT_SYMBOL(TdeOsiGetDeflickerLevel);

HI_S32 TdeOsiSetAlphaThresholdValue(HI_U8 u8ThresholdValue)
{
    return TdeHalSetAlphaThreshold(u8ThresholdValue);
}
EXPORT_SYMBOL(TdeOsiSetAlphaThresholdValue);

HI_S32 TdeOsiGetAlphaThresholdValue(HI_U8 *pu8ThresholdValue)
{
    if (HI_NULL == pu8ThresholdValue)
    {
        return HI_FAILURE;
    }
    return TdeHalGetAlphaThreshold(pu8ThresholdValue);
}
EXPORT_SYMBOL(TdeOsiGetAlphaThresholdValue);

HI_S32 TdeOsiSetAlphaThresholdState(HI_BOOL bEnAlphaThreshold)
{
    return TdeHalSetAlphaThresholdState(bEnAlphaThreshold);
}
EXPORT_SYMBOL(TdeOsiSetAlphaThresholdState);
HI_S32 TdeOsiGetAlphaThresholdState(HI_BOOL *pbEnAlphaThreshold)
{
    if (HI_NULL == pbEnAlphaThreshold)
    {
        return HI_FAILURE;
    }
    
    return TdeHalGetAlphaThresholdState(pbEnAlphaThreshold);
}
/*****************************************************************************
* Function:      TdeOsiCheckSingleSrcPatternOpt
* Description:   check if single source mode fill operate is valid
* Input:         enSrcFmt foreground pixel format
                 enDstFmt  target pixel format
                 pstOpt    operate attibute pointer
* Output:        none
* Return:        0  valid parameter
                 -1 invalid parameter
* Others:        none
*****************************************************************************/
HI_S32 TdeOsiCheckSingleSrcPatternOpt(TDE2_COLOR_FMT_E enSrcFmt,
        TDE2_COLOR_FMT_E enDstFmt, TDE2_PATTERN_FILL_OPT_S *pstOpt)
{
    TDE_COLORFMT_TRANSFORM_E enColorTransType;
    
    enColorTransType = TdeOsiGetFmtTransType(enSrcFmt, enDstFmt);
    if(TDE_COLORFMT_TRANSFORM_BUTT == enColorTransType)
    {
        TDE_TRACE(TDE_KERN_INFO, "Unkown color transport type!\n");
        return -1;
    }

    if (pstOpt != HI_NULL)
    {
        TDE_CHECK_COLORKEYMODE(pstOpt->enColorKeyMode);
        
        if (TDE2_COLORKEY_MODE_NONE != pstOpt->enColorKeyMode)
        {
            TDE_TRACE(TDE_KERN_INFO, "It doesn't support colorkey in single source pattern mode!\n");
            return -1;
        }
        
        TDE_CHECK_ALUCMD(pstOpt->enAluCmd);
        
        if (pstOpt->enAluCmd & TDE2_ALUCMD_ROP)
        {
            if ((!TdeOsiIsSingleSrc2Rop(pstOpt->enRopCode_Alpha))
                || (!TdeOsiIsSingleSrc2Rop(pstOpt->enRopCode_Color)))
            {
                TDE_TRACE(TDE_KERN_INFO, "Only support single s2 rop!\n");
                return -1;
            }
        }
        else if(TDE2_ALUCMD_NONE != pstOpt->enAluCmd)
        {
            TDE_TRACE(TDE_KERN_INFO, "Alu mode error!\n");
            return -1;
        }
        if(TDE_COLORFMT_TRANSFORM_CLUT_CLUT == enColorTransType)
        {
            if ((pstOpt->enAluCmd != TDE2_ALUCMD_NONE))
            {
                TDE_TRACE(TDE_KERN_INFO, "It doesn't ROP/Blend/Colorize!\n");
                return -1;
            }

            if (enSrcFmt != enDstFmt)
            {
                TDE_TRACE(TDE_KERN_INFO, "If src fmt and dst fmt are clut, they shoulod be the same fmt!\n");
                return -1;
            }
        }
    }  
    return HI_SUCCESS;
}
/*****************************************************************************
* Function:      TdeOsiCheckDoubleSrcPatternOpt
* Description:   check if doul source mode fill operate parameter is valid
* Input:         enBackGroundFmt background pixel format
                 enForeGroundFmt foreground pixel format 
                 enDstFmt  target pixel format
                 pstOpt     operate attribute pointer
* Output:        none
* Return:        0  valid parameter
                 -1 invalid parameter
* Others:        none
*****************************************************************************/
HI_S32 TdeOsiCheckDoubleSrcPatternOpt(TDE2_COLOR_FMT_E enBackGroundFmt, TDE2_COLOR_FMT_E enForeGroundFmt,
                                        TDE2_COLOR_FMT_E enDstFmt, TDE2_PATTERN_FILL_OPT_S *pstOpt)
{
    TDE_COLORFMT_CATEGORY_E enBackGroundCategory;
    TDE_COLORFMT_CATEGORY_E enForeGroundCategory;
    TDE_COLORFMT_CATEGORY_E enDstCategory;
 
    TDE_CHECK_ALUCMD(pstOpt->enAluCmd);

    
    enBackGroundCategory = TdeOsiGetFmtCategory(enBackGroundFmt);

    
    enForeGroundCategory = TdeOsiGetFmtCategory(enForeGroundFmt);

   
    enDstCategory = TdeOsiGetFmtCategory(enDstFmt);

    if ((TDE_COLORFMT_CATEGORY_BYTE <= enBackGroundCategory)
        || (TDE_COLORFMT_CATEGORY_BYTE <= enForeGroundCategory)
        || (TDE_COLORFMT_CATEGORY_BYTE <= enDstCategory))
    {
        TDE_TRACE(TDE_KERN_INFO, "Unkown color format!\n");
        return -1;
    }

    if (TDE_COLORFMT_CATEGORY_ARGB == enBackGroundCategory)
    {
        if (TDE_COLORFMT_CATEGORY_An == enForeGroundCategory)
        {
            
            if (!TdeOsiWhetherContainAlpha(enDstFmt))
            {
                TDE_TRACE(TDE_KERN_INFO, "Target must have alpha component!\n");
                return -1;
            }
        }
    }
    else if ((TDE2_COLOR_FMT_YCbCr888 == enBackGroundFmt)
             || (TDE2_COLOR_FMT_AYCbCr8888 == enForeGroundFmt))
    {
        if (TDE_COLORFMT_CATEGORY_An == enForeGroundCategory)
        {
            
            if (!TdeOsiWhetherContainAlpha(enDstFmt))
            {
                TDE_TRACE(TDE_KERN_INFO, "Target must have alpha component!\n");
                return -1;
            }
        }
    }
    else if (TDE_COLORFMT_CATEGORY_CLUT == enBackGroundCategory)
    {
        if ((TDE_COLORFMT_CATEGORY_CLUT != enForeGroundCategory)
            || (TDE_COLORFMT_CATEGORY_CLUT != enDstCategory))
        {
            TDE_TRACE(TDE_KERN_INFO, "Unsupported operation!\n");
            return -1;
        }

        
        if (TDE2_ALUCMD_NONE != pstOpt->enAluCmd)
        {
            TDE_TRACE(TDE_KERN_INFO, "It doesn't support ROP!\n");
            return -1;
        }

        if ((enBackGroundFmt != enForeGroundFmt)|| (enBackGroundFmt != enDstFmt))
        {
            TDE_TRACE(TDE_KERN_INFO, "If background, foreground , dst are clut, they \
                should be the same fmt!\n");
            return -1;
        }
    }
    else if((TDE_COLORFMT_CATEGORY_An == enBackGroundCategory)
        && (TDE_COLORFMT_CATEGORY_An == enForeGroundCategory)
        && (TDE_COLORFMT_CATEGORY_An == enDstCategory))
    {
       
        if (TDE2_ALUCMD_NONE != pstOpt->enAluCmd)
        {
            TDE_TRACE(TDE_KERN_INFO, "It doesn't support ROP or mirror!\n");
            return -1;
        }
    }  

    if (enDstCategory == TDE_COLORFMT_CATEGORY_CLUT)
    {
        if ((TDE_COLORFMT_CATEGORY_CLUT != enForeGroundCategory)
            || (TDE_COLORFMT_CATEGORY_CLUT != enBackGroundCategory))
        {
            TDE_TRACE(TDE_KERN_INFO, "Unsupported operation!\n");
            return -1;
        }
    }
    return HI_SUCCESS;
}

/*****************************************************************************
* Function:      TdeOsiGetPatternOptCategory
* Description:   analyze pattern fill operate type
* Input:         pstBackGround: background bitmap info
*                pstBackGroundRect: background bitmap operate zone
*                pstForeGround: foreground bitmap info
*                pstForeGroundRect: foreground bitmap operate zone
*                pstDst: target bitmap info
*                pstDstRect: target bitmap operate zone
*                pstOpt: operate option
* Output:        none
* Return:        TDE operate type
* Others:        none
*****************************************************************************/
TDE_PATTERN_OPERATION_CATEGORY_E TdeOsiGetPatternOptCategory(TDE2_SURFACE_S* pstBackGround, 
    TDE2_RECT_S  *pstBackGroundRect, TDE2_SURFACE_S* pstForeGround, TDE2_RECT_S  *pstForeGroundRect, 
    TDE2_SURFACE_S* pstDst, TDE2_RECT_S  *pstDstRect, TDE2_PATTERN_FILL_OPT_S* pstOpt)
{
    TDE2_SURFACE_S *pTmpSrc2 = HI_NULL;
    TDE2_RECT_S *pTmpSrc2Rect = HI_NULL;

    if ((NULL == pstDst) || (NULL == pstDstRect))
    {
        TDE_TRACE(TDE_KERN_INFO, "Dst/Dst Rect should not be null!\n");
        return TDE_PATTERN_OPERATION_BUTT;
    }
    
    TDE_CHECK_NOT_MB(pstDst->enColorFmt);
    
   
    if (TdeOsiCheckSurface(pstDst, pstDstRect) < 0)
    {
        TDE_TRACE(TDE_KERN_INFO, "\n");
        return TDE_PATTERN_OPERATION_BUTT;
    }

    
    if ((NULL != pstBackGround) && (NULL != pstForeGround))
    {
        if ((NULL == pstBackGroundRect) || (NULL == pstForeGroundRect) || (NULL == pstOpt))
        {
            TDE_TRACE(TDE_KERN_INFO, "Background Rect/Foreground Rect/Opt should not be null in two src pattern fill!\n");
            return TDE_PATTERN_OPERATION_BUTT;
        }

	 TDE_CHECK_NOT_MB(pstBackGround->enColorFmt);
	 TDE_CHECK_NOT_MB(pstForeGround->enColorFmt);
		
        
        if (TdeOsiCheckSurface(pstBackGround, pstBackGroundRect) < 0)
        {
            TDE_TRACE(TDE_KERN_INFO, "\n");
            return TDE_PATTERN_OPERATION_BUTT;
        }

        
        if (TdeOsiCheckSurface(pstForeGround, pstForeGroundRect) < 0)
        {
            TDE_TRACE(TDE_KERN_INFO, "\n");
            return TDE_PATTERN_OPERATION_BUTT;
        }

        
        if ((pstBackGroundRect->u32Height != pstDstRect->u32Height)
            || (pstBackGroundRect->u32Width != pstDstRect->u32Width))
        {
            TDE_TRACE(TDE_KERN_INFO, "Size of background rect and Dst rect should be the same in two src pattern fill.\
                background x:%d, y:%d, w:%d, h:%d;dst x:%d, y:%d, w:%d, h:%d\n", pstBackGroundRect->s32Xpos,\
                pstBackGroundRect->s32Ypos, pstBackGroundRect->u32Width, pstBackGroundRect->u32Height,\
                pstDstRect->s32Xpos, pstDstRect->s32Ypos, pstDstRect->u32Width, pstDstRect->u32Height);
            return TDE_PATTERN_OPERATION_BUTT;
        }

        if (pstForeGroundRect->u32Width > 256)
        {
            TDE_TRACE(TDE_KERN_INFO, "Max pattern width is 256!\n");
            return TDE_PATTERN_OPERATION_BUTT;
        }
        return TDE_PATTERN_OPERATION_DOUBLE_SRC;
    }
    else if (pstBackGround != NULL)
    {
        if (HI_NULL == pstBackGroundRect)
        {
            TDE_TRACE(TDE_KERN_INFO, "Background rect shouldn't be NULL!\n");
            return TDE_PATTERN_OPERATION_BUTT;
        }
        pTmpSrc2 = pstBackGround;
        pTmpSrc2Rect = pstBackGroundRect;
    }
    else if (pstForeGround != NULL)
    {
        if (HI_NULL == pstForeGroundRect)
        {
            TDE_TRACE(TDE_KERN_INFO, "Foreground rect shouldn't be NULL!\n");
            return TDE_PATTERN_OPERATION_BUTT;
        }
        pTmpSrc2 = pstForeGround;
        pTmpSrc2Rect = pstForeGroundRect;
    }
    else
    {
        TDE_TRACE(TDE_KERN_INFO, "No src!");
        return TDE_PATTERN_OPERATION_BUTT;
    }

    if (pTmpSrc2 != HI_NULL)
    {
	 TDE_CHECK_NOT_MB(pTmpSrc2->enColorFmt);
	
        
        if (TdeOsiCheckSurface(pTmpSrc2, pTmpSrc2Rect) < 0)
        {
            TDE_TRACE(TDE_KERN_INFO, "\n");
            return TDE_PATTERN_OPERATION_BUTT;
        }
        if (pTmpSrc2Rect->u32Width > 256)
        {
            TDE_TRACE(TDE_KERN_INFO, "Max pattern width is 256!\n");
            return TDE_PATTERN_OPERATION_BUTT;
        }
    }

    return TDE_PATTERN_OPERATION_SINGLE_SRC;
}

HI_BOOL TdeOsiCheckOverlap(TDE2_SURFACE_S *pstSur1, TDE2_RECT_S *pstRect1, TDE2_SURFACE_S *pstSur2, TDE2_RECT_S *pstRect2)
{
    HI_U32 u32Rect1StartPhy;
    HI_U32 u32Rect1EndPhy;
    HI_U32 u32Rect2StartPhy;
    HI_U32 u32Rect2EndPhy;
    HI_U32 u32Bpp1;
    HI_U32 u32Bpp2;

    u32Bpp1 = TdeOsiGetbppByFmt(pstSur1->enColorFmt) / 8;
    u32Bpp2 = TdeOsiGetbppByFmt(pstSur2->enColorFmt) / 8;
    
    u32Rect1StartPhy = pstSur1->u32PhyAddr + (pstRect1->s32Ypos * pstSur1->u32Stride) \
        + pstRect1->s32Xpos * u32Bpp1;
    u32Rect1EndPhy = u32Rect1StartPhy + (pstRect1->u32Height - 1) * pstSur1->u32Stride \
        + (pstRect1->u32Width - 1) * u32Bpp1;

    u32Rect2StartPhy = pstSur2->u32PhyAddr + (pstRect2->s32Ypos * pstSur2->u32Stride) \
        + pstRect2->s32Xpos * u32Bpp2;
    u32Rect2EndPhy = u32Rect2StartPhy + (pstRect2->u32Height - 1) * pstSur2->u32Stride \
        + (pstRect2->u32Width - 1) * u32Bpp2;

    TDE_TRACE(TDE_KERN_DEBUG, "u32Rect1StartPhy:%x, u32Rect1EndPhy:%x, u32Rect2StartPhy:%x, \
        u32Rect2EndPhy:%x\n", u32Rect1StartPhy, u32Rect1EndPhy, u32Rect2StartPhy, u32Rect2EndPhy);
    if (((u32Rect1StartPhy >= u32Rect2StartPhy) && (u32Rect1StartPhy <= u32Rect2EndPhy)) \
        || ((u32Rect1EndPhy >= u32Rect2StartPhy) && (u32Rect1EndPhy <= u32Rect2EndPhy)))
    {
        return HI_TRUE;
    }

    return HI_FALSE;
}

HI_S32 TdeOsiPatternBlit(TDE_HANDLE s32Handle, TDE2_SURFACE_S *pstBackGround, 
    TDE2_RECT_S *pstBackGroundRect, TDE2_SURFACE_S *pstForeGround, TDE2_RECT_S *pstForeGroundRect,
    TDE2_SURFACE_S *pstDst, TDE2_RECT_S *pstDstRect, TDE2_PATTERN_FILL_OPT_S *pstOpt)
{
    TDE2_OPT_S stOpt = {0};
    TDE2_RECT_S stBackRect = {0};
    TDE2_RECT_S stDstRect = {0};
    TDE2_RECT_S stForeRect = {0};

    HI_S32 s32Ret = HI_FAILURE;
    //TDE2_RECT_S stTempBackRect = {0};
    //TDE2_RECT_S stTempDstRect = {0};
    HI_S32 s32Result = HI_FAILURE;
    
    stOpt.enAluCmd = pstOpt->enAluCmd;
    stOpt.enRopCode_Alpha = pstOpt->enRopCode_Alpha;
    stOpt.enRopCode_Color = pstOpt->enRopCode_Color;
    stOpt.enColorKeyMode = pstOpt->enColorKeyMode;
    stOpt.unColorKeyValue = pstOpt->unColorKeyValue;
    stOpt.enClipMode = pstOpt->enClipMode;
    memcpy(&stOpt.stClipRect, &pstOpt->stClipRect, sizeof(TDE2_RECT_S));
    stOpt.bClutReload = pstOpt->bClutReload;
    stOpt.u8GlobalAlpha = pstOpt->u8GlobalAlpha;
    stOpt.enOutAlphaFrom = pstOpt->enOutAlphaFrom;
    stOpt.u32Colorize = pstOpt->u32Colorize;
    memcpy(&stOpt.stBlendOpt, &pstOpt->stBlendOpt, sizeof(TDE2_BLEND_OPT_S));
    memcpy(&stOpt.stCscOpt, &pstOpt->stCscOpt, sizeof(TDE2_CSC_OPT_S));

    stBackRect.s32Xpos = pstBackGroundRect->s32Xpos;
    stDstRect.s32Xpos = pstDstRect->s32Xpos;

    memcpy(&stForeRect, pstForeGroundRect, sizeof(TDE2_RECT_S));

    while((HI_U32)stDstRect.s32Xpos < ((HI_U32)pstDstRect->s32Xpos + pstDstRect->u32Width))
    {
        stDstRect.s32Ypos = pstDstRect->s32Ypos;
        stDstRect.u32Width = pstForeGroundRect->u32Width;
        stDstRect.u32Height = pstForeGroundRect->u32Height;
        if (((HI_U32)stDstRect.s32Xpos + stDstRect.u32Width) >
            ((HI_U32)pstDstRect->s32Xpos + pstDstRect->u32Width))
        {
            stDstRect.u32Width = (HI_U32)pstDstRect->s32Xpos + pstDstRect->u32Width - (HI_U32)stDstRect.s32Xpos;
        }
        stBackRect.s32Ypos = pstBackGroundRect->s32Ypos;
        stBackRect.u32Width = stDstRect.u32Width;
        stBackRect.u32Height = stDstRect.u32Height;
        while((HI_U32)stDstRect.s32Ypos < ((HI_U32)pstDstRect->s32Ypos + pstDstRect->u32Height))
        {
            if (((HI_U32)stDstRect.s32Ypos + stDstRect.u32Height)
                > ((HI_U32)pstDstRect->s32Ypos + pstDstRect->u32Height) )
            {
                stDstRect.u32Height = (HI_U32)pstDstRect->s32Ypos + pstDstRect->u32Height - (HI_U32)stDstRect.s32Ypos;
                stBackRect.u32Height = stDstRect.u32Height;
            }
            if (pstBackGround)
            {
                stForeRect.u32Width = stBackRect.u32Width;
                stForeRect.u32Height = stBackRect.u32Height;
                s32Ret = TdeOsiDoubleSrcBlit(s32Handle, pstBackGround, &stBackRect, pstForeGround, &stForeRect, pstDst, &stDstRect, &stOpt);
            }
            else
            {
                s32Ret = TdeOsiSingleSrc2Blit(s32Handle, pstForeGround, &stForeRect, pstDst, &stDstRect, &stOpt);
            }

            if (HI_ERR_TDE_NO_MEM == s32Ret)
            {
                return HI_ERR_TDE_NO_MEM;
            }
            else
            {
                s32Result &= s32Ret;
            }
            
            stDstRect.s32Ypos += stDstRect.u32Height;
            stBackRect.s32Ypos += stDstRect.u32Height;
        }
        stDstRect.s32Xpos += stDstRect.u32Width;
        stBackRect.s32Xpos += stDstRect.u32Width;
    }

    return s32Result;    
}

/*****************************************************************************
* Function:      TdeOsiSingleSrcPatternFill
* Description:   single resource pattern fill
* Input:         s32Handle:task handle
                pstBackGround: background bitmap info
*                pstBackGroundRect: background bitmap operate zone
*                pstForeGround: foreground bitmap info
*                pstForeGroundRect: foreground bitmap operate zone
*                pstDst: target bitmap info
*                pstDstRect: target bitmap operate zone
*                pstOpt: operate option
* Output:        none 
* Return:        success/fail
* Others:        none
*****************************************************************************/
HI_S32 TdeOsiSingleSrcPatternFill(TDE_HANDLE s32Handle, TDE2_SURFACE_S *pstSrc, 
    TDE2_RECT_S *pstSrcRect, TDE2_SURFACE_S *pstDst, TDE2_RECT_S *pstDstRect, 
    TDE2_PATTERN_FILL_OPT_S *pstOpt)
{
    TDE_HWNode_S stHWNode = {0};
    TDE_DRV_BASEOPT_MODE_E enBaseMode = {0};
    TDE_DRV_ALU_MODE_E enAluMode = {0};
    TDE_SCANDIRECTION_S stSrcScanInfo = {0};
    TDE_SCANDIRECTION_S stDstScanInfo = {0};
    TDE_DRV_SURFACE_S stSrcDrvSurface = {0};
    TDE_DRV_SURFACE_S stDstDrvSurface = {0};
    HI_U16 u16Code = 0;
    TDE_DRV_CONV_MODE_CMD_S stConv = {0};
    TDE_CLUT_USAGE_E enClutUsage = TDE_CLUT_USAGE_BUTT;
    HI_S32 s32Ret;

    if ((NULL == pstSrc) || (NULL == pstSrcRect)
        || (NULL == pstDst) || (NULL == pstDstRect))
    {
        return HI_ERR_TDE_NULL_PTR;
    }

    if (TdeOsiCheckSingleSrcPatternOpt(pstSrc->enColorFmt, pstDst->enColorFmt, pstOpt) < 0)
    {
        return HI_ERR_TDE_INVALID_PARA;
    }

    
    if (TdeOsiCheckOverlap(pstSrc, pstSrcRect, pstDst, pstDstRect))
    {
        TDE_TRACE(TDE_KERN_INFO, "Surface overlap!\n");
        return HI_ERR_TDE_INVALID_PARA;
    }
    
    TdeHalNodeInitNd(&stHWNode, HI_FALSE);

    enBaseMode = TDE_SINGLE_SRC_PATTERN_FILL_OPT;
    enAluMode = TDE_ALU_NONE;

    if(pstOpt != HI_NULL)
    {
        if (pstOpt->enAluCmd & TDE2_ALUCMD_ROP)
        {
            TDE_CHECK_ROPCODE(pstOpt->enRopCode_Alpha);
            TDE_CHECK_ROPCODE(pstOpt->enRopCode_Color);
            if ((!TdeOsiIsSingleSrc2Rop(pstOpt->enRopCode_Alpha))
                || (!TdeOsiIsSingleSrc2Rop(pstOpt->enRopCode_Color)))
            {
                TDE_TRACE(TDE_KERN_INFO, "Only support single s2 rop!\n");
                return -1;
            }
            enAluMode = TDE_ALU_ROP;
            
            
            TdeHalNodeSetRop(&stHWNode, pstOpt->enRopCode_Color, pstOpt->enRopCode_Alpha);
        }
    
        if (pstOpt->enAluCmd & TDE2_ALUCMD_BLEND)
        {
            enAluMode = TDE_ALU_BLEND;

#if 0
            if (!pstOpt->stBlendOpt.bSrc2AlphaPremulti)
            {
                enAluMode = TDE_ALU_BLEND_SRC2;
            }
#else
            TdeHalNodeSetBlend(&stHWNode, &pstOpt->stBlendOpt);
#endif

            if (pstOpt->enAluCmd & TDE2_ALUCMD_ROP)
            {
                TdeHalNodeEnableAlphaRop(&stHWNode);
            }
        }
    
        if (pstOpt->enAluCmd & TDE2_ALUCMD_COLORIZE)
        {
            TdeHalNodeSetColorize(&stHWNode, pstOpt->u32Colorize);
        }

        
        TdeHalNodeSetGlobalAlpha(&stHWNode, pstOpt->u8GlobalAlpha, pstOpt->stBlendOpt.bGlobalAlphaEnable);

       
        if(TdeOsiSetPatternClipPara(HI_NULL, HI_NULL, pstSrc, pstSrcRect, pstDst, pstDstRect, pstOpt, &stHWNode) < 0)
        {
            return HI_ERR_TDE_CLIP_AREA;
        }
    }
    
    TdeHalNodeSetBaseOperate(&stHWNode, enBaseMode, enAluMode, HI_NULL);

    TdeOsiSetExtAlpha(HI_NULL, pstSrc, &stHWNode);
    
    
    stSrcScanInfo.enHScan = TDE_SCAN_LEFT_RIGHT;
    stSrcScanInfo.enVScan = TDE_SCAN_UP_DOWN;
    stDstScanInfo.enHScan = TDE_SCAN_LEFT_RIGHT;
    stDstScanInfo.enVScan = TDE_SCAN_UP_DOWN;

   
    TdeOsiConvertSurface(pstSrc, pstSrcRect, &stSrcScanInfo, &stSrcDrvSurface, NULL);

    
    TdeHalNodeSetSrc2(&stHWNode, &stSrcDrvSurface);

    
    TdeOsiConvertSurface(pstDst, pstDstRect, &stDstScanInfo, &stDstDrvSurface, NULL);

    
    if (pstOpt != HI_NULL)
    {
        TDE_CHECK_OUTALPHAFROM(pstOpt->enOutAlphaFrom);
        if (pstOpt->enOutAlphaFrom == TDE2_OUTALPHA_FROM_BACKGROUND)
        {
            TDE_TRACE(TDE_KERN_INFO, "Single src pattern fill dosen't support out alpha form background!\n");
            return HI_ERR_TDE_INVALID_PARA;
        }
        TdeHalNodeSetTgt(&stHWNode, &stDstDrvSurface, pstOpt->enOutAlphaFrom);
    }
    else
    {
        TdeHalNodeSetTgt(&stHWNode, &stDstDrvSurface, TDE2_OUTALPHA_FROM_FOREGROUND);
    }

   
    u16Code = TdeOsiSingleSrc2GetOptCode(pstSrc->enColorFmt, pstDst->enColorFmt);

    TdeOsiGetConvbyCode(u16Code, &stConv);

    TdeHalNodeSetColorConvert(&stHWNode, &stConv);
   
    if (pstOpt != HI_NULL)
    {
        s32Ret = TdeOsiSetClutOpt(pstSrc, pstDst, &enClutUsage, pstOpt->bClutReload, &stHWNode);
    }
    else
    {
        s32Ret = TdeOsiSetClutOpt(pstSrc, pstDst, &enClutUsage, HI_TRUE, &stHWNode);
    }

    if (s32Ret != HI_SUCCESS)
    {
        return s32Ret;
    }

    
    if (pstOpt != HI_NULL)
    {
    	TDeHalNodeSetCsc(&stHWNode, pstOpt->stCscOpt);
    }

    return TdeOsiSetNodeFinish(s32Handle, &stHWNode, 0, TDE_NODE_SUBM_ALONE); 
}
/*****************************************************************************
* Function:      TdeOsiDoubleSrcPatternFill
* Description:   dual resource pattern fill 
* Input:         s32Handle: task handle 
*                pstBackGround: background bitmap info
*                pstBackGroundRect: background bitmap operate zone
*                pstForeGround: foreground bitmap info
*                pstForeGroundRect: foreground bitmap operate zone
*                pstDst: target bitmap info
*                pstDstRect: target bitmap operate zone
*                pstOpt: operate option
* Output:        none 
* Return:        success/fail
* Others:        none
*****************************************************************************/
HI_S32 TdeOsiDoubleSrcPatternFill(TDE_HANDLE s32Handle, TDE2_SURFACE_S *pstBackGround, 
    TDE2_RECT_S *pstBackGroundRect, TDE2_SURFACE_S *pstForeGround, TDE2_RECT_S *pstForeGroundRect,
    TDE2_SURFACE_S *pstDst, TDE2_RECT_S *pstDstRect, TDE2_PATTERN_FILL_OPT_S *pstOpt)
{
    TDE_HWNode_S stHWNode = {0};
    TDE_DRV_SURFACE_S stSrcDrvSurface = {0};
    TDE_DRV_SURFACE_S stDstDrvSurface = {0};
    TDE_SCANDIRECTION_S stSrcScanInfo = {0};
    TDE_SCANDIRECTION_S stDstScanInfo = {0};
    TDE_DRV_ALU_MODE_E enAluMode = TDE_ALU_NONE;
    TDE_CLUT_USAGE_E enClutUsage = TDE_CLUT_USAGE_BUTT;
    HI_U16 u16Code = 0;
    TDE_DRV_CONV_MODE_CMD_S stConv = {0};
    HI_S32 s32Ret;

    if ((HI_NULL == pstBackGround) || (HI_NULL == pstBackGroundRect)
        || (HI_NULL == pstForeGround) || (HI_NULL == pstForeGroundRect)
        || (HI_NULL == pstDst) || (HI_NULL == pstDstRect)
        || (HI_NULL == pstOpt))
    {
        return HI_ERR_TDE_INVALID_PARA;
    }
    
    if (TdeOsiCheckDoubleSrcPatternOpt(pstBackGround->enColorFmt, pstForeGround->enColorFmt, 
                            pstDst->enColorFmt, pstOpt) < 0)
    {
        return HI_ERR_TDE_INVALID_PARA;
    }

    
    if (TdeOsiCheckOverlap(pstForeGround, pstForeGroundRect, pstBackGround, pstBackGroundRect)\
        || TdeOsiCheckOverlap(pstForeGround, pstForeGroundRect, pstDst, pstDstRect))
    {
        TDE_TRACE(TDE_KERN_INFO, "Surface overlap!\n");
        return HI_ERR_TDE_INVALID_PARA;
    }

    if (bTdeHalSwVersion())
    {
        TDE_COLORFMT_CATEGORY_E eSrcCategory;
        TDE_COLORFMT_CATEGORY_E eDstCategory;
        eSrcCategory = TdeOsiGetFmtCategory(pstForeGround->enColorFmt);
        eDstCategory = TdeOsiGetFmtCategory(pstDst->enColorFmt);
        if ((TDE_COLORFMT_CATEGORY_CLUT == eSrcCategory)
            && (eDstCategory != TDE_COLORFMT_CATEGORY_CLUT)
            && pstOpt && (pstOpt->enColorKeyMode == TDE2_COLORKEY_MODE_FOREGROUND))
        {
             return TdeOsiPatternBlit(s32Handle, pstBackGround, pstBackGroundRect, pstForeGround, pstForeGroundRect, pstDst, pstDstRect,pstOpt); 
        }
    }

    
    TdeHalNodeInitNd(&stHWNode, HI_FALSE);

    if (pstOpt->enAluCmd & TDE2_ALUCMD_ROP)
    {
        TDE_CHECK_ROPCODE(pstOpt->enRopCode_Color);
        TDE_CHECK_ROPCODE(pstOpt->enRopCode_Alpha);
        enAluMode = TDE_ALU_ROP;

        
        TdeHalNodeSetRop(&stHWNode, pstOpt->enRopCode_Color, pstOpt->enRopCode_Alpha);
    }

    if (pstOpt->enAluCmd & TDE2_ALUCMD_BLEND)
    {
        TDE_CHECK_BLENDCMD(pstOpt->stBlendOpt.eBlendCmd);
	     if (pstOpt->stBlendOpt.eBlendCmd == TDE2_BLENDCMD_CONFIG)
	     {
	          TDE_CHECK_BLENDMODE(pstOpt->stBlendOpt.eSrc1BlendMode);
		   TDE_CHECK_BLENDMODE(pstOpt->stBlendOpt.eSrc2BlendMode);	  
	     }
        enAluMode = TDE_ALU_BLEND;

#if 0
        if (!pstOpt->stBlendOpt.bSrc2AlphaPremulti)
        {
            enAluMode = TDE_ALU_BLEND_SRC2;
        }
#else
        TdeHalNodeSetBlend(&stHWNode, &pstOpt->stBlendOpt);
#endif
        
        if (pstOpt->enAluCmd & TDE2_ALUCMD_ROP)
        {
            TdeHalNodeEnableAlphaRop(&stHWNode);
        }
    }    
    
    if (pstOpt->enAluCmd & TDE2_ALUCMD_COLORIZE)
    {
        TdeHalNodeSetColorize(&stHWNode, pstOpt->u32Colorize);
    }
    
    
    TdeHalNodeSetGlobalAlpha(&stHWNode, pstOpt->u8GlobalAlpha, pstOpt->stBlendOpt.bGlobalAlphaEnable);

    
    if(TdeOsiSetPatternClipPara(pstBackGround, pstBackGroundRect, pstForeGround, pstForeGroundRect, pstDst, pstDstRect, pstOpt, &stHWNode) < 0)
    {
        return HI_ERR_TDE_CLIP_AREA;
    }

    TdeHalNodeSetBaseOperate(&stHWNode, TDE_DOUBLE_SRC_PATTERN_FILL_OPT, enAluMode, HI_NULL);

    TdeOsiSetExtAlpha(pstBackGround, pstForeGround, &stHWNode);

   
    stSrcScanInfo.enHScan = TDE_SCAN_LEFT_RIGHT;
    stSrcScanInfo.enVScan = TDE_SCAN_UP_DOWN;
    stDstScanInfo.enHScan = TDE_SCAN_LEFT_RIGHT;
    stDstScanInfo.enVScan = TDE_SCAN_UP_DOWN;

    
    TdeOsiConvertSurface(pstBackGround, pstBackGroundRect, &stSrcScanInfo, &stSrcDrvSurface, NULL);

    
    TdeHalNodeSetSrc1(&stHWNode, &stSrcDrvSurface);

    
    TdeOsiConvertSurface(pstForeGround, pstForeGroundRect, &stSrcScanInfo, &stSrcDrvSurface, NULL);

    
    TdeHalNodeSetSrc2(&stHWNode, &stSrcDrvSurface);

   
    TdeOsiConvertSurface(pstDst, pstDstRect, &stDstScanInfo, &stDstDrvSurface, NULL);

    
    TdeHalNodeSetTgt(&stHWNode, &stDstDrvSurface, pstOpt->enOutAlphaFrom);

    
    u16Code = TdeOsiDoubleSrcGetOptCode(pstBackGround->enColorFmt, pstForeGround->enColorFmt,
                                        pstDst->enColorFmt);

    TdeOsiGetConvbyCode(u16Code, &stConv);

    
    TdeHalNodeSetColorConvert(&stHWNode, &stConv);

    
    if((s32Ret = TdeOsiSetClutOpt(pstForeGround, pstBackGround, &enClutUsage, pstOpt->bClutReload, &stHWNode)) < 0)
    {
        return s32Ret;
    }

    
    TDE_CHECK_COLORKEYMODE(pstOpt->enColorKeyMode);
    if (TDE2_COLORKEY_MODE_NONE != pstOpt->enColorKeyMode)
    {
        TDE_DRV_COLORKEY_CMD_S stColorkey;
        TDE_COLORFMT_CATEGORY_E enFmtCategory;
        stColorkey.unColorKeyValue = pstOpt->unColorKeyValue;

        switch (pstOpt->enColorKeyMode)
        {
        case TDE2_COLORKEY_MODE_BACKGROUND:
            stColorkey.enColorKeyMode = TDE_DRV_COLORKEY_BACKGROUND;

            
            enFmtCategory = TdeOsiGetFmtCategory(pstBackGround->enColorFmt);
            if (enFmtCategory >= TDE_COLORFMT_CATEGORY_BUTT)
            {
                TDE_TRACE(TDE_KERN_ERR, "Unknown fmt category!\n");
                return HI_ERR_TDE_INVALID_PARA;
            }
            
            TdeHalNodeSetColorKey(&stHWNode, enFmtCategory, &stColorkey);

            break;

        case TDE2_COLORKEY_MODE_FOREGROUND:
            if (TDE_CLUT_COLOREXPENDING != enClutUsage
                && TDE_CLUT_CLUT_BYPASS != enClutUsage)
            {
                
                stColorkey.enColorKeyMode = TDE_DRV_COLORKEY_FOREGROUND_AFTER_CLUT;
            }
            else
            {
                stColorkey.enColorKeyMode = TDE_DRV_COLORKEY_FOREGROUND_BEFORE_CLUT;
            }

            
            enFmtCategory = TdeOsiGetFmtCategory(pstForeGround->enColorFmt);
            if (enFmtCategory >= TDE_COLORFMT_CATEGORY_BUTT)
            {
                TDE_TRACE(TDE_KERN_ERR, "Unknown fmt category!\n");
                return HI_ERR_TDE_INVALID_PARA;
            }

            TdeHalNodeSetColorKey(&stHWNode, enFmtCategory, &stColorkey);
            break;

        default:
            TDE_TRACE(TDE_KERN_INFO, "\n");
            return HI_ERR_TDE_INVALID_PARA;
        }
    }

    
    TDeHalNodeSetCsc(&stHWNode, pstOpt->stCscOpt);
    
    
    return TdeOsiSetNodeFinish(s32Handle, &stHWNode, 0, TDE_NODE_SUBM_ALONE);
}
/*****************************************************************************
* Function:      TdeOsiPatternFill
* Description:   pattern fill
* Input:         s32Handle:task handle
                 pstBackGround: background bitmap info
*                pstBackGroundRect: background bitmap operate zone
*                pstForeGround: foreground bitmap info
*                pstForeGroundRect: foreground bitmap operate zone
*                pstDst: target bitmap info
*                pstDstRect: target bitmap operate zone
*                pstOpt: operate option
* Output:        none 
* Return:        success/fail
* Others:        none
*****************************************************************************/
HI_S32 TdeOsiPatternFill(TDE_HANDLE s32Handle, TDE2_SURFACE_S * pstBackGround, 
    TDE2_RECT_S * pstBackGroundRect, TDE2_SURFACE_S * pstForeGround, 
    TDE2_RECT_S * pstForeGroundRect, TDE2_SURFACE_S * pstDst, 
    TDE2_RECT_S * pstDstRect, TDE2_PATTERN_FILL_OPT_S *pstOpt)
{
    TDE_PATTERN_OPERATION_CATEGORY_E enOptCategory;
    
    enOptCategory = TdeOsiGetPatternOptCategory(pstBackGround, pstBackGroundRect, pstForeGround, pstForeGroundRect, pstDst,
                                         pstDstRect, pstOpt);
    switch(enOptCategory)
    { 
        case TDE_PATTERN_OPERATION_SINGLE_SRC:
        {
            if (HI_NULL != pstBackGround)
            {
                return TdeOsiSingleSrcPatternFill(s32Handle, pstBackGround, pstBackGroundRect, pstDst, pstDstRect, pstOpt);
            }
            else
            {
                return TdeOsiSingleSrcPatternFill(s32Handle, pstForeGround, pstForeGroundRect, pstDst, pstDstRect, pstOpt);
            }
        }
        case TDE_PATTERN_OPERATION_DOUBLE_SRC:
        {
            return TdeOsiDoubleSrcPatternFill(s32Handle, pstBackGround, pstBackGroundRect, pstForeGround, pstForeGroundRect,
                                   pstDst, pstDstRect, pstOpt);
        }
        default:
            return HI_ERR_TDE_INVALID_PARA;     
    }
}

HI_S32 TdeCalScaleRect(const TDE2_RECT_S* pstSrcRect, const TDE2_RECT_S* pstDstRect,
                                TDE2_RECT_S* pstRectInSrc, TDE2_RECT_S* pstRectInDst)
{
    UpdateConfig reg;
    UpdateInfo info;

    reg.ori_in_height = pstSrcRect->u32Height;
    reg.ori_in_width = pstSrcRect->u32Width;
    reg.zme_out_height = pstDstRect->u32Height;
    reg.zme_out_width = pstDstRect->u32Width;
    
    reg.update_instart_w = pstRectInSrc->s32Xpos;
    reg.update_instart_h = pstRectInSrc->s32Ypos;
    reg.update_in_width = pstRectInSrc->u32Width;
    reg.update_in_height = pstRectInSrc->u32Height;
    
    TdeOsiGetHUpdateInfo(&reg, &info, HI_TRUE);
    TdeOsiGetVUpdateInfo(&reg, &info, HI_TRUE, HI_TRUE);

    pstRectInSrc->s32Xpos = info.zme_instart_w;
    pstRectInSrc->s32Ypos = info.zme_instart_h;
    pstRectInSrc->u32Width = info.zme_in_width;
    pstRectInSrc->u32Height = info.zme_in_height;

    pstRectInDst->s32Xpos = info.zme_outstart_w;
    pstRectInDst->s32Ypos = info.zme_outstart_h;
    pstRectInDst->u32Width = info.zme_out_width;
    pstRectInDst->u32Height = info.zme_out_height;

    return HI_SUCCESS;
}


HI_S32 TdeOsiEnableRegionDeflicker(HI_BOOL bRegionDeflicker)
{
    s_bRegionDeflicker = bRegionDeflicker;
    return HI_SUCCESS;
}


EXPORT_SYMBOL(TdeOsiPatternFill);
EXPORT_SYMBOL(TdeOsiEnableRegionDeflicker);
EXPORT_SYMBOL(TdeCalScaleRect);


#ifdef __cplusplus
 #if __cplusplus
}
 #endif /* __cplusplus */
#endif  /* __cplusplus */

