/*****************************************************************************
*             Copyright 2006 - 2050, Hisilicon Tech. Co., Ltd.
*                           ALL RIGHTS RESERVED
* FileName: tde_hal.h
* Description:TDE hal interface define
*
* History:
* Version   Date          Author        DefectNum       Description
*
*****************************************************************************/

#ifndef _TDE_HAL_H_
#define _TDE_HAL_H_

#include "hi_tde_type.h"

#include "tde_define.h"

#ifdef __cplusplus
 #if __cplusplus
extern "C" {
 #endif /* __cplusplus */
#endif  /* __cplusplus */

#define TDE_INTNUM  123//68
#define TDE_REG_BASEADDR 0xf8c10000//0x10120000
/****************************************************************************/
/*                             TDE hal types define                         */
/****************************************************************************/

/* TDE pixel format, accord with register, value cannot modified freely */
typedef enum hiTDE_DRV_COLOR_FMT_E
{
    TDE_DRV_COLOR_FMT_RGB444   = 0,
    TDE_DRV_COLOR_FMT_RGB555   = 1,
    TDE_DRV_COLOR_FMT_RGB565   = 2,
    TDE_DRV_COLOR_FMT_RGB888   = 3,
    TDE_DRV_COLOR_FMT_ARGB4444 = 4,
    TDE_DRV_COLOR_FMT_ARGB1555 = 5,
    TDE_DRV_COLOR_FMT_ARGB8565 = 6,
    TDE_DRV_COLOR_FMT_ARGB8888 = 7,
    TDE_DRV_COLOR_FMT_CLUT1   = 8,
    TDE_DRV_COLOR_FMT_CLUT2   = 9,
    TDE_DRV_COLOR_FMT_CLUT4   = 10,
    TDE_DRV_COLOR_FMT_CLUT8   = 11,
    TDE_DRV_COLOR_FMT_ACLUT44 = 12,
    TDE_DRV_COLOR_FMT_ACLUT88 = 13,
    TDE_DRV_COLOR_FMT_A1 = 16,
    TDE_DRV_COLOR_FMT_A8 = 17,
    TDE_DRV_COLOR_FMT_YCbCr888   = 18,
    TDE_DRV_COLOR_FMT_AYCbCr8888 = 19,
    TDE_DRV_COLOR_FMT_YCbCr422 = 20,
    TDE_DRV_COLOR_FMT_byte = 22,
    TDE_DRV_COLOR_FMT_halfword = 23,
    TDE_DRV_COLOR_FMT_YCbCr400MBP = 24,
    TDE_DRV_COLOR_FMT_YCbCr422MBH = 25,
    TDE_DRV_COLOR_FMT_YCbCr422MBV = 26,
    TDE_DRV_COLOR_FMT_YCbCr420MB = 27,
    TDE_DRV_COLOR_FMT_YCbCr444MB = 28,
    TDE_DRV_COLOR_FMT_RABG8888,
    TDE_DRV_COLOR_FMT_BUTT
} TDE_DRV_COLOR_FMT_E;

/* branch order*/
/* support 24 orders in ARGB, TDE driver can only expose 4 usual orders; if needed, can be added */
typedef enum hiTDE_DRV_ARGB_ORDER_E
{
    TDE_DRV_ORDER_ARGB = 0x0,
    TDE_DRV_ORDER_ABGR = 0x5,
	TDE_DRV_ORDER_RABG = 0x7,
    TDE_DRV_ORDER_RGBA = 0x9,
    TDE_DRV_ORDER_BGRA = 0x14,
    TDE_DRV_ORDER_BUTT
}TDE_DRV_ARGB_ORDER_E;

/* TDE basic operate mode */
typedef enum hiTDE_DRV_BASEOPT_MODE_E
{
    /* Quick fill */
    TDE_QUIKE_FILL,

    /* Quick copy */
    TDE_QUIKE_COPY,

    /* Normal fill in single source */
    TDE_NORM_FILL_1OPT,

    /* Normal bilit in single source */
    TDE_NORM_BLIT_1OPT,

    /* Fill and Rop */
    TDE_NORM_FILL_2OPT,

    /* Normal bilit in double source */
    TDE_NORM_BLIT_2OPT,

    /* MB operation */
    TDE_MB_C_OPT,    /* MB chroma zoom */
    TDE_MB_Y_OPT,    /* MB brightness zoom */
    TDE_MB_2OPT,     /* MB combinate operation */

    /* Fill operate in single source mode  */
    TDE_SINGLE_SRC_PATTERN_FILL_OPT,

    /* Fill operate in double source mode */
    TDE_DOUBLE_SRC_PATTERN_FILL_OPT
} TDE_DRV_BASEOPT_MODE_E;

/* Type definition in interrupted state */
typedef enum hiTDE_DRV_INT_STATS_E
{
    TDE_DRV_LINK_COMPLD_STATS = 0x1,
    TDE_DRV_NODE_COMPLD_STATS = 0x2,
    TDE_DRV_LINE_SUSP_STATS = 0x4,
    TDE_DRV_RDY_START_STATS = 0x8,
    TDE_DRV_SQ_UPDATE_STATS = 0x10,
    TDE_DRV_INT_ALL_STATS = 0x800F001F
} TDE_DRV_INT_STATS_E;

/* ColorKey mode is needed by hardware */
typedef enum hiTDE_DRV_COLORKEY_MODE_E
{
    TDE_DRV_COLORKEY_BACKGROUND = 0,          		/* color key in bkground bitmap */
    TDE_DRV_COLORKEY_FOREGROUND_BEFORE_CLUT = 2,  	/* color key in foreground bitmap,before CLUT */
    TDE_DRV_COLORKEY_FOREGROUND_AFTER_CLUT = 3    	/* color key in bkground bitmap, after CLUT */
} TDE_DRV_COLORKEY_MODE_E;

/* color key setting arguments*/
typedef struct hiTDE_DRV_COLORKEY_CMD_S
{
    TDE_DRV_COLORKEY_MODE_E enColorKeyMode;        	/* color key mode */
    TDE2_COLORKEY_U        unColorKeyValue;       	/* color key value */
} TDE_DRV_COLORKEY_CMD_S;

/* Deficker filting mode */
typedef enum hiTDE_DRV_FLICKER_MODE
{
    TDE_DRV_FIXED_COEF0 = 0,   /* Deficker by fixed coefficient: 0 */
    TDE_DRV_AUTO_FILTER,       /* Deficker by auto filter */
    TDE_DRV_TEST_FILTER        /* Deficker by test filter */
} TDE_DRV_FLICKER_MODE;

/* Block type, equipped register note in numerical value reference */
typedef enum hiTDE_SLICE_TYPE_E
{
    TDE_NO_BLOCK_SLICE_TYPE = 0,         /* No block */
    TDE_FIRST_BLOCK_SLICE_TYPE = 0x3,    /* First block */
    TDE_LAST_BLOCK_SLICE_TYPE = 0x5,     /* Last block */
    TDE_MID_BLOCK_SLICE_TYPE = 0x1       /* Middle block */
} TDE_SLICE_TYPE_E;

/* vertical/horizontal filt mode: available for zoom */
typedef enum hiTDE_DRV_FILTER_MODE_E
{
    TDE_DRV_FILTER_NONE = 0,    /* none filt*/
    TDE_DRV_FILTER_COLOR,       /* filt on color parameter */
    TDE_DRV_FILTER_ALPHA,       /* filt on Alpha value */
    TDE_DRV_FILTER_ALL          /* filt on Alpha and color value */
} TDE_DRV_FILTER_MODE_E;

/* Deflicker operate setting */
typedef struct hiTDE_DRV_FLICKER_CMD_S
{
    TDE_DRV_FLICKER_MODE enDfeMode;
    TDE_DRV_FILTER_MODE_E enFilterV;
    HI_U8            u8Coef0LastLine;
    HI_U8            u8Coef0CurLine;
    HI_U8            u8Coef0NextLine;
    HI_U8            u8Coef1LastLine;
    HI_U8            u8Coef1CurLine;
    HI_U8            u8Coef1NextLine;
    HI_U8            u8Coef2LastLine;
    HI_U8            u8Coef2CurLine;
    HI_U8            u8Coef2NextLine;
    HI_U8            u8Coef3LastLine;
    HI_U8            u8Coef3CurLine;
    HI_U8            u8Coef3NextLine;
    HI_U8            u8Threshold0;
    HI_U8            u8Threshold1;
    HI_U8            u8Threshold2;
    TDE2_DEFLICKER_MODE_E enDeflickerMode;
} TDE_DRV_FLICKER_CMD_S;


/* Zoom operate settings */
typedef struct hiTDE_DRV_RESIZE_CMD_S
{
    HI_U32            u32OffsetX;
    HI_U32            u32OffsetY;
    HI_U32            u32StepH;
    HI_U32            u32StepV;
    HI_BOOL           bCoefSym;
    HI_BOOL           bVfRing;
    HI_BOOL           bHfRing;
    TDE_DRV_FILTER_MODE_E enFilterV;
    TDE_DRV_FILTER_MODE_E enFilterH;
    HI_BOOL           bFirstLineOut;
    HI_BOOL           bLastLineOut;   
} TDE_DRV_RESIZE_CMD_S;

/* Clip Setting */
typedef struct hiTDE_DRV_CLIP_CMD_S
{
    HI_U16  u16ClipStartX;
    HI_U16  u16ClipStartY;
    HI_U16  u16ClipEndX;
    HI_U16  u16ClipEndY;
    HI_BOOL bInsideClip;
} TDE_DRV_CLIP_CMD_S;

/* clut mode */
typedef enum hiTDE_DRV_CLUT_MODE_E
{
    /* color expand */
    TDE_COLOR_EXP_CLUT_MODE = 0,

    /* color correct */
    TDE_COLOR_CORRCT_CLUT_MODE
} TDE_DRV_CLUT_MODE_E;

/* clut setting */
typedef struct hiTDE_DRV_CLUT_CMD_S
{
    TDE_DRV_CLUT_MODE_E enClutMode;
    HI_U8*          pu8PhyClutAddr;
} TDE_DRV_CLUT_CMD_S;

/* MB Setting */
typedef enum hiTDE_DRV_MB_OPT_MODE_E
{
    TDE_MB_Y_FILTER = 0, 		/* brightness filt */
    TDE_MB_CbCr_FILTER = 2, 	/* chroma filt*/
    TDE_MB_UPSAMP_CONCA = 4,	/* first upsample then contact in chroma and brightness */
    TDE_MB_CONCA_FILTER = 6, 	/* first contact in chroma and brightness and then filt */
    TDE_MB_RASTER_OPT = 1    	/* MB Raster Operate */
} TDE_DRV_MB_OPT_MODE_E;

/* MB Command Setting */
typedef struct hiTDE_DRV_MB_CMD_S
{
    TDE_DRV_MB_OPT_MODE_E enMbMode;        /* MB Operate Mode */
} TDE_DRV_MB_CMD_S;

/* plane mask command setting */
typedef struct hiTDE_DRV_PLMASK_CMD_S
{
    HI_U32 u32Mask;
} TDE_DRV_PLMASK_CMD_S;

/* Color zone convert setting */
typedef struct hiTDE_DRV_CONV_MODE_CMD_S
{
    /* Import Metrix used by color converted:graphic:0/video:1 */
    HI_U8 bInMetrixVid;

    /* Import standard in color convertion:IT-U601:0/ITU-709:1 */
    HI_U8 bInMetrix709;

    /* Export Metrix used by color converted:graphic:0/video:1 */
    HI_U8 bOutMetrixVid;

    /* Import standard in color conversion:IT-U601:0/ITU-709:1 */
    HI_U8 bOutMetrix709;

    /* Enable or unable conversion on importing color zone */
    HI_U8 bInConv;

    /* Enable or unable conversion on exporting color zone */
    HI_U8 bOutConv;
    HI_U8 bInSrc1Conv;

    /* import color conversion direction */
    HI_U8 bInRGB2YC;
} TDE_DRV_CONV_MODE_CMD_S;

/* vertical scanning direction */
typedef enum hiTDE_DRV_VSCAN_E
{
    TDE_SCAN_UP_DOWN = 0,	/* form up to down */
    TDE_SCAN_DOWN_UP = 1 	/* form down to up */
} TDE_DRV_VSCAN_E;

/* horizontal scanning direction */
typedef enum hiTDE_DRV_HSCAN_E
{
    TDE_SCAN_LEFT_RIGHT = 0,	/* form left to right */
    TDE_SCAN_RIGHT_LEFT = 1 	/* form right to left */
} TDE_DRV_HSCAN_E;

/* Definition on scanning direction */
typedef struct hiTDE_SCANDIRECTION_S
{
    /* vertical scanning direction */
    TDE_DRV_VSCAN_E enVScan;

    /* horizontal scanning direction */
    TDE_DRV_HSCAN_E enHScan;
} TDE_SCANDIRECTION_S;

/*  Between bitmap info struct setted by driver , by user and hardware info is not all
	the same. eg, bitmap info can be divided into two bitmap info: src1 and src2, 
	which is hardware needs, when user set for MB.

	In MB mode(refer to TDE_INS register), pu8PhyCbCr is not used in driver, but divided into 
	head addr of src1 and src2.
*/

/* TDEV240 version:
1. In nonMB mod, you can support MB. Because adding two membet variables:u32CbCrPhyAddr¡¢u32CbCrPitch,
which for Src1 and Src2 add one assistant channel by each.

2.Support component order in ARGB/RGB format(24 kinds in toal)
,add component order register and member variables:enRgbOrder.
*/

typedef struct hiTDE_DRV_SURFACE_S
{
    /* Bitmap head addr */
    HI_U32 u32PhyAddr;

    /* color format */
    TDE_DRV_COLOR_FMT_E enColorFmt;

    /* ARGB component order */
    TDE_DRV_ARGB_ORDER_E enRgbOrder;

    /* Position X at first */
    HI_U32 u32Xpos;

    /* Position Y at first */
    HI_U32 u32Ypos;

    /* Bitmap Height */
    HI_U32 u32Height;

    /* Bitmap Width */
    HI_U32 u32Width;

    /* Bitmap Pitch */
    HI_U32 u32Pitch;

    /* CbCr component addr */
    HI_U32 u32CbCrPhyAddr;

    /* CbCr pitch*/
    HI_U32 u32CbCrPitch;

    /* alpha max value is 255?or 128? */
    HI_BOOL bAlphaMax255;

    /* Vertical scanning direction */
    TDE_DRV_VSCAN_E enVScan;

    /* Horizontal scanning direction */
    TDE_DRV_HSCAN_E enHScan;
} TDE_DRV_SURFACE_S;

/* MB bitmap info */
typedef struct hiTDE_DRV_MB_S
{
    TDE_DRV_COLOR_FMT_E enMbFmt;
    HI_U32              u32YPhyAddr;
    HI_U32              u32YWidth;
    HI_U32              u32YHeight;
    HI_U32              u32YStride;
    HI_U32              u32CbCrPhyAddr;
    HI_U32              u32CbCrStride;
} TDE_DRV_MB_S;

/* ALU mode*/
typedef enum hiTDE_DRV_ALU_MODE_E
{
    TDE_SRC1_BYPASS = 0,
    TDE_ALU_ROP,
    TDE_ALU_BLEND,
    TDE_ALU_BLEND_SRC2,
    TDE_ALU_MASK_ROP1,
    TDE_ALU_MASK_BLEND,
    TDE_ALU_CONCA,
    TDE_SRC2_BYPASS,
    TDE_ALU_MASK_ROP2,
    TDE_ALU_NONE 		/* register has no setting, used in flag */
} TDE_DRV_ALU_MODE_E;


/* cofigure info of node,by TDE_UPDATE order */
typedef struct hiTDE_HWNode_S
{
    HI_U32 u32TDE_INS;
    HI_U32 u32TDE_S1_ADDR;
    HI_U32 u32TDE_S1_TYPE;
    HI_U32 u32TDE_S1_XY;
    HI_U32 u32TDE_S1_FILL;
    HI_U32 u32TDE_S2_ADDR;
    HI_U32 u32TDE_S2_TYPE;
    HI_U32 u32TDE_S2_XY;
    HI_U32 u32TDE_S2_SIZE;
    HI_U32 u32TDE_S2_FILL;
    HI_U32 u32TDE_TAR_ADDR;
    HI_U32 u32TDE_TAR_TYPE;
    HI_U32 u32TDE_TAR_XY;
    HI_U32 u32TDE_TS_SIZE;
    HI_U32 u32TDE_COLOR_CONV;
    HI_U32 u32TDE_CLUT_ADDR;
    HI_U32 u32TDE_2D_RSZ;
    HI_U32 u32TDE_HF_COEF_ADDR;
    HI_U32 u32TDE_VF_COEF_ADDR;
#ifdef TDE_VERSION_MPW
    HI_U32 u32TDE_RSZ_STEP;
#else
    HI_U32 u32TDE_RSZ_HSTEP;
#endif
    HI_U32 u32TDE_RSZ_Y_OFST;
    HI_U32 u32TDE_RSZ_X_OFST;
    HI_U32 u32TDE_DFE_COEF0;
    HI_U32 u32TDE_DFE_COEF1;
    HI_U32 u32TDE_DFE_COEF2;
    HI_U32 u32TDE_DFE_COEF3;
    HI_U32 u32TDE_ALU;
    HI_U32 u32TDE_CK_MIN;
    HI_U32 u32TDE_CK_MAX;
    HI_U32 u32TDE_CLIP_START;
    HI_U32 u32TDE_CLIP_STOP;
#if defined (TDE_VERSION_PILOT) || defined (TDE_VERSION_FPGA)
    HI_U32 u32TDE_Y1_ADDR;
    HI_U32 u32TDE_Y1_PITCH;
    HI_U32 u32TDE_Y2_ADDR;
    HI_U32 u32TDE_Y2_PITCH;
    HI_U32 u32TDE_RSZ_VSTEP;
    HI_U32 u32TDE_ARGB_ORDER;
    HI_U32 u32TDE_CK_MASK;
    HI_U32 u32TDE_COLORIZE;
    HI_U32 u32TDE_ALPHA_BLEND;
    HI_U32 u32TDE_ICSC_ADDR;
    HI_U32 u32TDE_OCSC_ADDR;	
#endif
    HI_U64 u64TDE_UPDATE; /* record update settings */
} TDE_HWNode_S;

/* Zoom mode in subnode*/
typedef enum hiTDE_CHILD_SCALE_MODE_E
{
    TDE_CHILD_SCALE_NORM = 0,
    TDE_CHILD_SCALE_MBY,
    TDE_CHILD_SCALE_MBC,
    TDE_CHILD_SCALE_MB_CONCA_H,
    TDE_CHILD_SCALE_MB_CONCA_M,
    TDE_CHILD_SCALE_MB_CONCA_L,
    TDE_CHILD_SCALE_MB_CONCA_CUS,
} TDE_CHILD_SCALE_MODE_E;

/*  Info needed in MB format when Y/CbCr change */
typedef struct hiTDE_MBSTART_ADJ_INFO_S
{
    HI_U32 u32StartInX;     /* Start X,Y imported after MB adjust */
    HI_U32 u32StartInY;
    HI_U32 u32StartOutX;    /* Start X,Y exported after MB adjust */
    HI_U32 u32StartOutY;
    TDE_DRV_COLOR_FMT_E enFmt; /* color format, MB use it to renew position of Y and CbCr */
    TDE_CHILD_SCALE_MODE_E enScaleMode;
} TDE_MBSTART_ADJ_INFO_S;

/* Adjusting info when double source dispart */
typedef struct hiTDE_DOUBLESRC_ADJ_INFO_S
{
    HI_BOOL bDoubleSource;   
    HI_S32 s32DiffX;    /*  s32DiffX = S1x - Tx         */
    HI_S32 s32DiffY;    /*  s32DiffY = S1y - Ty         */ 
}TDE_DOUBLESRC_ADJ_INFO_S;
/*
 * Configure info when set child node 
 * u64Update :
 * _________________________________________
 * |    |    |    |    |    |    |    |    |
 * | ...| 0  | 0  | 1  | 1  | 1  | 1  |  1 |
 * |____|____|____|____|____|____|____|____|
 *                   |    |    |    |    |
 *                  \/   \/   \/   \/   \/
 *                u32Wo u32Xo HOfst u32Wi u32Xi
 *                u32Ho u32Yo VOfst u32Hi u32Yi
 */
typedef struct hiTDE_CHILD_INFO
{
    HI_U32 u32Xi;
    HI_U32 u32Yi;
    HI_U32 u32Wi;
    HI_U32 u32Hi;
    HI_U32 u32HOfst;
    HI_U32 u32VOfst;
    HI_U32 u32Xo;
    HI_U32 u32Yo;
    HI_U32 u32Wo;
    HI_U32 u32Ho;
    HI_U64 u64Update;
    TDE_MBSTART_ADJ_INFO_S stAdjInfo;
    TDE_DOUBLESRC_ADJ_INFO_S stDSAdjInfo;
    TDE_SLICE_TYPE_E enSliceType;
} TDE_CHILD_INFO;

typedef struct hiTDE_DRV_COLORFILL_S
{
    TDE_DRV_COLOR_FMT_E enDrvColorFmt;
    HI_U32              u32FillData;
} TDE_DRV_COLORFILL_S;

typedef enum hiTDE_DRV_INT_E
{
	TDE_DRV_INT_lIST_COMP_AQ = 0x10000,
	TDE_DRV_INT_lIST_COMP_SQ = 0x1,
	TDE_DRV_INT_NODE_COMP_AQ = 0x20000,
	TDE_DRV_INT_NODE_COMP_SQ = 0x2,
	TDE_DRV_INT_SUSPEND_LINE_AQ = 0x40000,
	TDE_DRV_INT_SUSPEND_LINE_SQ = 0x4,
 	TDE_DRV_INT_HEAD_UPDATE_SQ = 0x10,
 	TDE_DRV_INT_ERROR = 0x80000000,
}TDE_DRV_INT_E;

typedef TDE2_OUTALPHA_FROM_E TDE_DRV_OUTALPHA_FROM_E;

typedef enum hiTDE_DRV_SRC_E
{
    TDE_DRV_SRC_NONE = 0,
    TDE_DRV_SRC_S1 = 0x1,
    TDE_DRV_SRC_S2 = 0x2,
    TDE_DRV_SRC_T = 0x4,
}TDE_DRV_SRC_E;

/****************************************************************************/
/*                             TDE register macro definition                            */
/****************************************************************************/
#define TDE_BASE_ADDR 0x1013
#define TDE_VER 0x0800
#define TDE_RST 0x0804
#define TDE_CTRL 0x0808
#define TDE_SQ_ADDR 0x080C
#define TDE_AQ_ADDR 0x0810
#define TDE_STA 0x0814
#define TDE_INT 0x0818
#define TDE_SQ_CTRL 0x081C
#define TDE_SQ_NADDR 0x0820
#define TDE_SQ_UPDATE 0x0824
#define TDE_SQ_OFFSET 0x0828
#define TDE_AQ_CTRL 0x082C
#define TDE_AQ_NADDR 0x0830
#define TDE_AQ_UPDATE 0x0834
#define TDE_NADDR 0x0838
#define TDE_UPDATE 0x083C
#define TDE_SQ_RPT_ADDR 0x0840
#define TDE_BUS_LIMITER 0x0844
#define TDE_BUS_MONITOR 0x0848

#define TDE_SQ_UPDATE2 0x0858
#define TDE_AQ_UPDATE2 0x085c
#define TDE_REQ_TH 0x0868

#define TDE_INS 0x0880
#define TDE_S1_ADDR 0x0884
#define TDE_S1_TYPE 0x0888
#define TDE_S1_XY 0x088C
#define TDE_S1_FILL 0x0890
#define TDE_S2_ADDR 0x0894
#define TDE_S2_TYPE 0x0898
#define TDE_S2_XY 0x089C
#define TDE_S2_SIZE 0x08A0
#define TDE_S2_FILL 0x08A4
#define TDE_TAR_ADDR 0x08A8
#define TDE_TAR_TYPE 0x08AC
#define TDE_TAR_XY 0x08B0
#define TDE_TS_SIZE 0x08B4
#define TDE_COLOR_CONV 0x08B8
#define TDE_CLUT_ADDR 0x08BC
#define TDE_2D_RSZ 0x08C0
#define TDE_HCOEF_ADDR 0x08C4
#define TDE_VCOEF_ADDR 0x08C8
#define TDE_RSZ_STEP 0x08CC
#define TDE_RSZ_Y_OFST 0x08D0
#define TDE_RSZ_X_OFST 0x08D4
#define TDE_DFE_COEF0 0x08D8
#define TDE_DFE_COEF1 0x08DC
#define TDE_DFE_COEF2 0x08E0
#define TDE_DFE_COEF3 0x08E4
#define TDE_ALU 0x08E8
#define TDE_CK_MIN 0x08EC
#define TDE_CK_MAX 0x08F0
#define TDE_CLIP_START 0x08F4
#define TDE_CLIP_STOP 0x08F8
#define TDE_Y1_ADDR 0x08FC
#define TDE_Y1_PITCH 0x0900
#define TDE_Y2_ADDR 0x0904
#define TDE_Y2_PITCH 0x0908
#define TDE_RSZ_VSTEP 0x090C
#define TDE_ARGB_ORDER 0x0910
#define TDE_CK_MASK 0x0914
#define TDE_COLORIZE 0x0918
#define TDE_ALPHA_BLEND 0x091C
#define TDE_ICSC_ADDR 0x0920
#define TDE_OCSC_ADDR 0x0924

#define TDE_SQ_MASK_DISABLE_ALL 0  /*00000£ºinterrupt shield */
#define TDE_SQ_CUR_LINE_MASK_EN 1  /*00001£ºEnable to complete interrupt of current node and line in SQ */
#define TDE_SQ_RD_NODE_MASK_EN 2   /*00010£ºEable, complete interrupt in R/W new node instruct info in SQ, but not begin graphic operation */
#define TDE_SQ_COMP_NODE_MASK_EN 4 /*00100£ºEnable to complete interrupt of current node in SQ */
#define TDE_SQ_COMP_LIST_MASK_EN 8 /*01000£ºEnable to complete interrupt in SQ */
#define TDE_SQ_UPDATE_MASK_EN 16   /*10000£ºEnable to complete interrupt for update on list head in SQ */

#define TDE_AQ_MASK_DISABLE_ALL 0  /*0000£ºinterrupt shiled */
#define TDE_AQ_SUSP_MASK_EN 1      /*0001£ºEnable to interrupt during suspend on execute in one line in AQ */
#define TDE_AQ_RD_NODE_MASK_EN 2   /*0010£ºEnable to interrupt in completing R/W new node instruct info,but not begin graphic operation in AQ*/
#define TDE_AQ_COMP_NODE_MASK_EN 4 /*0100£ºEnable to interrupt when complete current node in AQ */
#define TDE_AQ_COMP_LIST_MASK_EN 8 /*1000£ºEnable to interrupt  in complete AQ */

/*Handle responsed with node */
/*
	Add 4 byte pointer in physical buffer header,to save software node;
	For need to consult current executing software node,but register can only
	give the physical addr of it.
*/
#define TDE_NODE_HEAD_BYTE 16

/* Next node addr¡¢update info¡¢occupied bytes */
#ifdef TDE_VERSION_MPW
#define TDE_NODE_TAIL_BYTE 8
#else
#define TDE_NODE_TAIL_BYTE 12
#endif

/****************************************************************************/
/*                             TDE hal ctl functions define                 */
/****************************************************************************/

/*****************************************************************************
* Function:      TdeHalInit
* Description:   main used in mapping TDE basic addr
* Input:         u32BaseAddr:Register basic addr
* Output:        None
* Return:        Success/Failure
* Others:        None
*****************************************************************************/
HI_S32  TdeHalInit(HI_U32 u32BaseAddr);

/*****************************************************************************
* Function:      TdeHalOpen
* Description:   main used in initialize needed register 
* Input:         None
* Output:        None
* Return:        None
* Others:        None
*****************************************************************************/
HI_S32 TdeHalOpen(HI_VOID);

/*****************************************************************************
* Function:      TdeHalRelease
* Description:   main used in release TDE basic addr by mapping
* Input:         None
* Output:        None
* Return:        Success/Failure
* Others:        None
*****************************************************************************/
HI_VOID TdeHalRelease(HI_VOID);

/*****************************************************************************
* Function:      TdeHalCtlIsIdle
* Description:   Query if TDE is in IDLE state or not
* Input:         None
* Output:        None
* Return:        True: Idle/False: Busy
* Others:        None
*****************************************************************************/
HI_BOOL TdeHalCtlIsIdle(HI_VOID);

/*****************************************************************************
* Function:      TdeHalCtlIsIdleSafely
* Description:   cycle many times, to make sure TDE is in IDLE state
* Input:         None
* Output:        None
* Return:        True: Idle/False: Busy
* Others:        None
*****************************************************************************/
HI_BOOL TdeHalCtlIsIdleSafely(HI_VOID);

/*****************************************************************************
* Function:      TdeHalCtlCurNode
* Description:   Get physical node addr when operate is suspend
* Input:         enListType: set Sq/Aq type
* Output:        None
* Return:        Physical address current node
* Others:        None
*****************************************************************************/

//HI_U32 TdeHalCtlCurNode(TDE_LIST_TYPE_E enListType);

/*****************************************************************************
* Function:      TdeHalCtlIntMask
* Description:   set Sq/Aq interrupt shield code
* Input:         enListType: set Sq/Aq type
*                u32Mask: interrupt shield code
* Output:        None
* Return:        None
* Others:        None
*****************************************************************************/

//HI_VOID TdeHalCtlIntMask(TDE_LIST_TYPE_E enListType, HI_U32 u32Mask);

/*****************************************************************************
* Function:      TdeHalCtlIntMask
* Description:   Get Sq/Aq interrupt state
* Input:         enListType: set Sq/Aq type
* Output:        None
* Return:        Sq/Aq interrupt state
* Others:        None
*****************************************************************************/
HI_U32  TdeHalCtlIntStats(HI_VOID);

/*****************************************************************************
* Function:      TdeHalResetStatus
* Description:   replace all state in TDE HAL, clean all list
* Input:         None
* Output:        None
* Return:        None
* Others:        None
*****************************************************************************/
HI_VOID TdeHalResetStatus(HI_VOID);

/*****************************************************************************
* Function:      TdeHalCtlReset
* Description:   soft replace, reset interrupt state
* Input:         None
* Output:        None
* Return:        None
* Others:        None
*****************************************************************************/
HI_VOID TdeHalCtlReset(HI_VOID);

/*****************************************************************************
* Function:      TdeHalCtlIntClear
* Description:   Reset relevant interrupt state
* Input:         enListType: set Sq/Aq type
*                u32Stats: Reset relevant interrupt state
* Output:        None
* Return:        None
* Others:        None
*****************************************************************************/
HI_VOID TdeHalCtlIntClear(TDE_LIST_TYPE_E enListType, HI_U32 u32Stats);

/*****************************************************************************
*Description: Query working queue if is synchronous or asynchronous  
*****************************************************************************/
HI_BOOL TdeHalIsSqWork(HI_VOID);

/****************************************************************************/
/*                             TDE hal node functions define                */
/****************************************************************************/

/*****************************************************************************
* Function:      TdeHalNodeInitNd
* Description:   Initialize struct, TDE operate node is needed,which in software cache
* Input:         pHWNode:Node struct pointer. used in cache by software
* Output:        None
* Return:        Mone
* Others:        None
*****************************************************************************/
HI_VOID TdeHalNodeInitNd(TDE_HWNode_S* pHWNode, HI_BOOL bChild);

HI_VOID TdeHalNodeInitChildNd(TDE_HWNode_S* pHWNode, HI_U32 u32TDE_CLIP_START, HI_U32 u32TDE_CLIP_STOP);


/*****************************************************************************
* Function:      TdeHalNodeGetNdSize
* Description:   Get needed bytes by current nodes (4 byete) 
* Input:         pHWNode: Struct pointer of node used by software in cache
* Output:        None
* Return:        Bytes
* Others:        None
*****************************************************************************/
HI_U32  TdeHalNodeGetNdSize(TDE_HWNode_S* pHWNode);

/*****************************************************************************
* Function:      TdeHalNodeMakeNd
* Description:    make pHWNode by current configure into the node in pBuf  can be configured straightly
* Input:         pBuf: Buffer of the node be make into 
*                pHWNode: Node struct pointer used in cache by software
* Output:        None
* Return:        None
* Others:        None
*****************************************************************************/
HI_S32  TdeHalNodeMakeNd(HI_VOID* pBuf, TDE_HWNode_S* pHWNode);

/*****************************************************************************
* Function:      TdeHalNodeEnableCompleteInt
* Description:   Complete interrupt by using node's operate
* Input:         pBuf: Buffer need node be operated
* Output:        None
* Return:        None
* Others:        None
*****************************************************************************/
HI_VOID TdeHalNodeEnableCompleteInt(HI_VOID* pBuf, TDE_LIST_TYPE_E enType);

/*****************************************************************************
* Function:      TdeHalNodeComplteAqList
* Description:   Notice hardware AQ list have been completed
* Input:         None
* Output:        None
* Return:        None
* Others:        None
*****************************************************************************/
HI_VOID TdeHalNodeComplteAqList(HI_VOID);

HI_VOID TdeHalNodeComplteNd(TDE_LIST_TYPE_E enListType);

/*****************************************************************************
* Function:      TdeHalNodeSetSrc1
* Description:   Set Src1 bitmap info
* Input:         pHWNode: Node struct pointer used in cache by software
*                pDrvSurface: bitmap info used in setting
* Output:        None
* Return:        None
* Others:        None
*****************************************************************************/
HI_VOID TdeHalNodeSetSrc1(TDE_HWNode_S* pHWNode, TDE_DRV_SURFACE_S* pDrvSurface);

/*****************************************************************************
* Function:      TdeHalNodeSetSrc2
* Description:   Set Src2 bitmap info
* Input:         pHWNode: Node struct pointer used in cache by software
*                pDrvSurface: bitmap info used in setting
* Output:        None
* Return:        None
* Others:        None
*****************************************************************************/
HI_VOID TdeHalNodeSetSrc2(TDE_HWNode_S* pHWNode, TDE_DRV_SURFACE_S* pDrvSurface);

/*****************************************************************************
* Function:      TdeHalNodeSetSrcMbY
* Description:   Set brightness information in MB source bitmap
* Input:         pHWNode: Node struct pointer used in cache by software
*                pDrvMbY: bitmap brightness information used in setting
*                enMbOpt: MB mode option
* Output:        None
* Return:        None
* Others:        None
*****************************************************************************/
HI_VOID TdeHalNodeSetSrcMbY(TDE_HWNode_S* pHWNode, TDE_DRV_SURFACE_S* pDrvMbY, TDE_DRV_MB_OPT_MODE_E enMbOpt);

/*****************************************************************************
* Function:      TdeHalNodeSetSrcMbCbCr
* Description:   set chroma info in MB source bitmap
* Input:         pHWNode: Node struct pointer used in cache by software
*                pDrvMbCbCr: CbCr info
*                enMbOpt: MB mode option
* Output:        None
* Return:        None
* Others:        None
*****************************************************************************/
HI_VOID TdeHalNodeSetSrcMbCbCr(TDE_HWNode_S* pHWNode, TDE_DRV_SURFACE_S* pDrvMbCbCr, TDE_DRV_MB_OPT_MODE_E enMbOpt);

/*****************************************************************************
* Function:      TdeHalNodeSetTgt
* Description:   Set target bitmap information
* Input:         pHWNode: Node struct pointer used in cache by software
*                pDrvSurface: bitmap information used in setting
* Output:        None
* Return:        None
* Others:        None
*****************************************************************************/
HI_VOID TdeHalNodeSetTgt(TDE_HWNode_S* pHWNode, TDE_DRV_SURFACE_S* pDrvSurface, TDE_DRV_OUTALPHA_FROM_E enAlphaFrom);

/*****************************************************************************
* Function:      TdeHalNodeSetBaseOperate
* Description:   Set basic operate type
* Input:         pHWNode: Node struct pointer used in cache by software
*                enMode: basic operate mode
*                enAlu: ALU mode
*                u32FillData: if basic mode have fill operate ,read this value
* Output:        None
* Return:        None
* Others:        None
*****************************************************************************/
HI_VOID TdeHalNodeSetBaseOperate(TDE_HWNode_S* pHWNode, TDE_DRV_BASEOPT_MODE_E enMode,
                                 TDE_DRV_ALU_MODE_E enAlu, TDE_DRV_COLORFILL_S *pstColorFill);

/*****************************************************************************
* Function:      TdeHalNodeSetGlobalAlpha
* Description:   Set Alpha mixed arguments
* Input:         pHWNode: Node struct pointer used in cache by software
*                u8Alpha: Alpha mixed setting value
*                       bEnable: Enable to use global alpha
* Output:        None
* Return:        None
* Others:        None
*****************************************************************************/
HI_VOID TdeHalNodeSetGlobalAlpha(TDE_HWNode_S* pHWNode, HI_U8 u8Alpha, HI_BOOL bEnable);

/*****************************************************************************
* Function:      TdeHalNodeSetExpAlpha
* Description:   When expand Alpha in RGB5551. to alpha0 and alpha1
* Input:         pHWNode: Node struct pointer used in cache by software
*                u8Alpha: Alpha mixed setting value
* Output:        None
* Return:        None
* Others:        None
*****************************************************************************/
HI_VOID TdeHalNodeSetExpAlpha(TDE_HWNode_S* pHWNode, TDE_DRV_SRC_E enSrc, HI_U8 u8Alpha0, HI_U8 u8Alpha1);
/*****************************************************************************
* Function:      TdeHalNodeSetAlphaBorder
* Description:   Enable to set Alpha can be bordered
* Input:         pHWNode: Node struct pointer used in cache by software
*                bEnable: Enanle to border Alpha
* Output:        None
* Return:        None
* Others:        None
*****************************************************************************/
HI_VOID TdeHalNodeSetAlphaBorder(TDE_HWNode_S* pHWNode, HI_BOOL bVEnable, HI_BOOL bHEnable);

/*****************************************************************************
* Function:      TdeHalNodeSetRop
* Description:   Set ROP arguments
* Input:         pHWNode: Node struct pointer used in cache by software
*                enRopCode: ROP operator
* Output:        None
* Return:        None
* Others:        None
*****************************************************************************/
HI_VOID TdeHalNodeSetRop(TDE_HWNode_S* pHWNode, TDE2_ROP_CODE_E enRgbRop, TDE2_ROP_CODE_E enAlphaRop);

/*****************************************************************************
* Function:      TdeHalNodeSetBlend
* Description:   Set blend operate arguments
* Input:         pHWNode: Node struct pointer used in cache by software
*                stBlendOpt:blend operate option
* Output:        None
* Return:        None
* Others:        None
*****************************************************************************/
HI_VOID TdeHalNodeSetBlend(TDE_HWNode_S *pHWNode, TDE2_BLEND_OPT_S *pstBlendOpt);

/*****************************************************************************
* Function:      TdeHalNodeSetColorize
* Description:   Set blend operate arguments
* Input:         pHWNode: Node struct pointer used in cache by software
*                u32Colorize:Co
* Output:        None
* Return:        None
* Others:        None
*****************************************************************************/
HI_VOID TdeHalNodeSetColorize(TDE_HWNode_S *pHWNode, HI_U32 u32Colorize);

/*****************************************************************************
* Function:      TdeHalNodeEnableAlphaRop
* Description:   Enable to blend Rop operate
* Input:         pHWNode: Node struct pointer used in cache by software
* Output:        None
* Return:        None
* Others:        None
*****************************************************************************/
HI_VOID TdeHalNodeEnableAlphaRop(TDE_HWNode_S *pHWNode);

/*****************************************************************************
* Function:      TdeHalNodeSetColorExp
* Description:   Set color expand or adjust argument
* Input:         pHWNode: Node struct pointer used in cache by software
*                pClutCmd: Clut operate atguments
* Output:        None
* Return:        None
* Others:        None
*****************************************************************************/
HI_VOID TdeHalNodeSetClutOpt(TDE_HWNode_S* pHWNode, TDE_DRV_CLUT_CMD_S* pClutCmd, HI_BOOL bReload);

/*****************************************************************************
* Function:      TdeHalNodeSetColorKey
* Description:   Set arguments needed by color key,according current color format
* Input:         pHWNode: Node struct pointer used in cache by software
*                enFmt: color format
*                pColorKey: color key pointer
* Output:        None
* Return:        None
* Others:        None
*****************************************************************************/
HI_VOID TdeHalNodeSetColorKey(TDE_HWNode_S* pHWNode, TDE_COLORFMT_CATEGORY_E enFmtCat, 
                              TDE_DRV_COLORKEY_CMD_S* pColorKey);

/*****************************************************************************
* Function:      TdeHalNodeSetClipping
* Description:   Set rectangle's clip operated arguments
* Input:         pHWNode: Node struct pointer used in cache by software
*                pClip: Clip rectangle range
* Output:        None
* Return:        None
* Others:        None
*****************************************************************************/
HI_VOID TdeHalNodeSetClipping(TDE_HWNode_S* pHWNode, TDE_DRV_CLIP_CMD_S* pClip);

/*****************************************************************************
* Function:      TdeHalNodeSetFlicker
* Description:   set deflicker fliter operate arguments
* Input:         pHWNode: Node struct pointer used in cache by software
*                pFlicker: Deflicker coefficient
* Output:        None
* Return:        None
* Others:        None
*****************************************************************************/
HI_VOID TdeHalNodeSetFlicker(TDE_HWNode_S* pHWNode, TDE_DRV_FLICKER_CMD_S* pFlicker);

/*****************************************************************************
* Function:      TdeHalNodeSetResize
* Description:   set zoom fliter operate arguments
* Input:         pHWNode: Node struct pointer used in cache by software
*                pResize: zoom coefficient
* Output:        None
* Return:        None
* Others:        None
*****************************************************************************/
HI_VOID TdeHalNodeSetResize(TDE_HWNode_S* pHWNode, TDE_DRV_RESIZE_CMD_S* pResize,TDE_NODE_SUBM_TYPE_E enNodeType);

/*****************************************************************************
* Function:      TdeHalNodeSetResize
* Description:   set color zone conversion arguments
* Input:         pHWNode: Node struct pointer used in cache by software
*                pConv: color zone conversion argument
* Output:        None
* Return:        None
* Others:        None
*****************************************************************************/
HI_VOID TdeHalNodeSetColorConvert(TDE_HWNode_S* pHWNode, TDE_DRV_CONV_MODE_CMD_S* pConv);

/*****************************************************************************
* Function:      TdeHalNodeAddChild
* Description:   Add child node when filter operate
* Input:         pHWNode: Node struct pointer used in cache by software
*                pChildInfo: add child node's configue information
* Output:        None
* Return:        None
* Others:        None
*****************************************************************************/
HI_VOID TdeHalNodeAddChild(TDE_HWNode_S* pHWNode, TDE_CHILD_INFO* pChildInfo);

/*****************************************************************************
* Function:      TdeHalNodeSetMbMode
* Description:   set MB operate arguments
* Input:         pHWNode: Node struct pointer used in cache by software
*                pMbCmd: MB operate arguments
* Output:        None
* Return:        None
* Others:        None
*****************************************************************************/
HI_VOID TdeHalNodeSetMbMode(TDE_HWNode_S* pHWNode, TDE_DRV_MB_CMD_S* pMbCmd);

/*****************************************************************************
* Function:      TdeHalNodeSuspend
* Description:   Save current node and line to ensure operate normally,when suspend list operation
* Input:         None
* Output:        None
* Return:        None
* Others:        None
*****************************************************************************/
HI_VOID TdeHalNodeSuspend(HI_VOID);

/*****************************************************************************
* Function:      TDeHalNodeSetCsc
* Description:  Set CSC's first optional argument
* Input:         pHWNode:Node struct pointer used in cache by software
			stCscOpt:CSC first optional argument
* Output:        None
* Return:        None
* Others:        None
*****************************************************************************/
HI_VOID TDeHalNodeSetCsc(TDE_HWNode_S* pHWNode, TDE2_CSC_OPT_S stCscOpt);

/* Get software node address be suspended */
HI_VOID TdeHalGetSuspendNode(HI_VOID **ppstSuspendNode);

/*****************************************************************************
* Function:      TdeHalSetDeflicerLevel
* Description:   set deflicker level
* Input:         eDeflickerLevel:deflicker level
* Output:        None
* Return:        Success / Fail
* Others:        None
*****************************************************************************/
HI_S32 TdeHalSetDeflicerLevel(TDE_DEFLICKER_LEVEL_E eDeflickerLevel);

/*****************************************************************************
* Function:      TdeHalGetDeflicerLevel
* Description:   Get deflicker level
* Input:        
* Output:        pDeflicerLevel:deflicker level
* Return:        Success / Fail
* Others:        None
*****************************************************************************/
HI_S32 TdeHalGetDeflicerLevel(TDE_DEFLICKER_LEVEL_E *pDeflicerLevel);

/*****************************************************************************
* Function:      TdeHalSetAlphaThreshold
* Description:   Set alpha threshold 
* Input:         u8ThresholdValue:alpha threshold
* Output:        
* Return:        Success / Fail
* Others:        None
*****************************************************************************/
HI_S32 TdeHalSetAlphaThreshold(HI_U8 u8ThresholdValue);

/*****************************************************************************
* Function:      TdeHalGetAlphaThreshold
* Description:   Get alpha threshold 
* Input:         
* Output:        pu8ThresholdValue:alpha threshold
* Return:        Success / Fail
* Others:        None
*****************************************************************************/
HI_S32 TdeHalGetAlphaThreshold(HI_U8 * pu8ThresholdValue);

/*****************************************************************************
* Function:      TdeHalGetAlphaThresholdState
* Description:   Set alpha threshold to judge if open or close
* Input:         bEnAlphaThreshold:alpha switch status
* Output:        None
* Return:        Success / Fail
* Others:        None
*****************************************************************************/
HI_S32 TdeHalSetAlphaThresholdState(HI_BOOL bEnAlphaThreshold);

/*****************************************************************************
* Function:      TdeHalGetAlphaThresholdState
* Description:   Get alpha threshold to judge if open or close
* Input:         None
* Output:        pbEnAlphaThreshold:alpha switch status
* Return:        Success / Fail
* Others:        None
*****************************************************************************/
HI_S32 TdeHalGetAlphaThresholdState(HI_BOOL *pbEnAlphaThreshold);

/*****************************************************************************
* Function:      TdeHalNodeExecute
* Description:   Start TDE list
* Input:         enListType: List type
*                u32NodePhyAddr: list first node address
*                u64Update: first node updating flag
*                bAqUseBuff: if use temporary buffer
* Output:        None
* Return:        Success / Fail
* Others:        None
*****************************************************************************/
HI_S32 TdeHalNodeExecute(TDE_LIST_TYPE_E enListType, HI_U32 u32NodePhyAddr, HI_U64 u64Update, HI_BOOL bAqUseBuff);

/*****************************************************************************
* Function:      TdeHalRestoreAloneNode
* Description:   Resume node operation, this node is independant
* Input:         None
* Output:        None
* Return:        Success / Fail
* Others:        None
*****************************************************************************/
HI_S32 TdeHalRestoreAloneNode(HI_VOID);

/*****************************************************************************
* Function:      TdeHalCurNode
* Description:   Get current node in register
* Input:         enListType: List type
* Output:        node physical address
* Return:        None
* Others:        None
*****************************************************************************/
HI_U32 TdeHalCurNode(TDE_LIST_TYPE_E enListType);

HI_VOID TdeHalResumeInit(HI_VOID);

HI_VOID TdeHalSystemInit(HI_VOID);

HI_BOOL bTdeHalSwVersion(HI_VOID);

#ifdef __cplusplus
 #if __cplusplus
}
 #endif /* __cplusplus */
#endif  /* __cplusplus */

#endif  /* _TDE_HAL_H_ */
