/*****************************************************************************
*             Copyright 2006 - 2050, Hisilicon Tech. Co., Ltd.
*                           ALL RIGHTS RESERVED
* FileName: tde_hal.c
* Description:TDE hal interface define
*
* History:
* Version   Date          Author        DefectNum       Description
*
*****************************************************************************/
#include <linux/mm.h>
#include <linux/slab.h>     
#include <linux/list.h> 
#include <asm/io.h>
#include <linux/delay.h>

#include "tde_hal.h"
#include "tde_define.h"
#include "wmalloc.h"

#include "hi_common.h"
#include "tde_filterPara.h"
#include "hi_reg_common.h"

#include <asm/barrier.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif  /* __cplusplus */
#endif  /* __cplusplus */
/****************************************************************************/
/*                   TDE hal inner macro definition                         */
/****************************************************************************/

/* compute struct member's offset */
#define TDE_OFFSET_OF(type, member) ((HI_U32) (&((type *)0)->member))

#ifdef TDE_VERSION_MPW
#define UPDATE_SIZE 32
#else
#define UPDATE_SIZE 64
#endif

/* Judge if Update flag is 1 or not on current configure */
#define TDE_MASK_UPDATE(member) (1 << ((TDE_OFFSET_OF(TDE_HWNode_S, member) >> 2) % (UPDATE_SIZE)))

/* According to setting's configue, update its update flag */
#define TDE_SET_UPDATE(member, updateflg) \
do{\
    updateflg |= ((HI_U64)1 << ((TDE_OFFSET_OF(TDE_HWNode_S, member))/4 % (UPDATE_SIZE)));\
}while(0)

#define TDE_FILL_DATA_BY_FMT(fill, data, fmt)\
do{\
    HI_U32 u32Bpp;\
    u32Bpp = TdeHalGetbppByFmt(fmt);\
    if (0xffffffff != u32Bpp)\
    {\
        HI_U32 i;\
        HI_U32 u32Cell;\
        u32Cell = data & (0xffffffff >> (32 - u32Bpp));\
        for (i = 0; i < (32 / u32Bpp); i++)\
        {\
            fill |= (u32Cell << (i * u32Bpp));\
        }\
    }\
    else\
    {\
        fill = data;\
    }\
}while(0)

/* Get member pointer in node buffer according to current member and update flag
   Return value pRet is null is to say current buffer has not needed member */
#define TDE_GET_MEMBER_IN_BUFF(pRet, member, pstVirBuf, update)\
do{\
    HI_S32 s32Offset;\
    \
    s32Offset = TdeHalGetOffsetInNode(TDE_MASK_UPDATE(member), update);\
    if (-1 != s32Offset )\
    {\
        pRet = pstVirBuf + s32Offset;\
    }\
    else\
    {\
        pRet = HI_NULL;\
    }\
}while(0)

#ifdef TDE_VERSION_MPW
#define TDE_ALPHA_ADJUST(u8Alpha)\
do{\
    if(255 == u8Alpha)\
    {\
        u8Alpha = 0x80;\
    }\
    else\
    {\
        u8Alpha = ((u8Alpha + 1)>>1); /*AI7D02798*/ \
    }\
}while(0)
#else
#define TDE_ALPHA_ADJUST(u8Alpha)
#endif


#define TDE_GET_MEMBER_IN_BUFNODE(pRet, member, pstNdBuf) \
    TDE_GET_MEMBER_IN_BUFF(pRet, member, (pstNdBuf)->pu32VirAddr, (pstNdBuf)->u32CurUpdate)

/* R/W register's encapsulation */
#ifndef HI311X_WRITE_REG
#define HI311X_WRITE_REG(base, offset, value) \
    (*(volatile unsigned int   *)((unsigned int)(base) + (offset)) = (value))
#endif

#ifndef HI311X_READ_REG
#define HI311X_READ_REG(base, offset) \
    (*(volatile unsigned int   *)((unsigned int)(base) + (offset)))
#endif

#define TDE_READ_REG(base, offset) HI311X_READ_REG((base), (offset))
#define TDE_WRITE_REG(base, offset, val) HI311X_WRITE_REG((base), (offset), (val))

/* TDE register's Address range */
#define TDE_REG_SIZE 0x1000

/* Step range's type by algorithm team */
#define TDE_RESIZE_PARA_AREA_0  3724     /*1.1*/
#define TDE_RESIZE_PARA_AREA_1  4096     /*1.0*/
#define TDE_RESIZE_PARA_AREA_2  4501     /*0.91*/
#define TDE_RESIZE_PARA_AREA_3  4935     /*0.83*/
#define TDE_RESIZE_PARA_AREA_4  5326     /*0.769*/
#define TDE_RESIZE_PARA_AREA_5  5729     /*0.715*/
#define TDE_RESIZE_PARA_AREA_6  6141     /*0.667*/
#define TDE_RESIZE_PARA_AREA_7  8192     /*0.5*/
#define TDE_RESIZE_PARA_AREA_8  12300    /*0.333*/
#define TDE_RESIZE_PARA_AREA_9  16384    /*0.25*/
#define TDE_RESIZE_PARA_AREA_10 20480    /*0.2*/
/* coefficient table rangle of 8*32 area */
#define TDE_RESIZE_8X32_AREA_0  4096     /*1.0*/
#define TDE_RESIZE_8X32_AREA_1  5936     /*0.69*/
#define TDE_RESIZE_8X32_AREA_2  8192     /*0.5*/
#define TDE_RESIZE_8X32_AREA_3  12412    /*0.33*/
#define TDE_RESIZE_8X32_AREA_4  204800   /*0.02*/

/*Colorkey mode of CMOS chip */
#define TDE_COLORKEY_IGNORE     2
#define TDE_COLORKEY_AREA_OUT   1
#define TDE_COLORKEY_AREA_IN    0

/* Aq control mode */
#define TDE_AQ_CTRL_COMP_LIST   0x0 /* start next AQ list, after complete current list's operations */
#define TDE_AQ_CTRL_COMP_LINE   0x4 /* start next AQ list, after complete current node and line */

/* Sq control mode */
#define TDE_SQ_CTRL_COMP_LIST   0x0 /* start next SQ list, after complete current list's operations */
#define TDE_SQ_CTRL_COMP_LINE   0x4 /* start next SQ list, after complete current node and line*/

/* Flag when current line is suspend */
#define TDE_SUSP_LINE_INVAL (-1)
#define TDE_SUSP_LINE_READY (-2)

#define TDE_MAX_READ_STATUS_TIME 10

#define TDE_SWVERSION_NUM (0x20080228)
/****************************************************************************/
/*                        TDE hal inner type definition                     */
/****************************************************************************/
/* source bitmap running mode */
typedef enum hiTDE_SRC_MODE_E
{
    TDE_SRC_MODE_DISA  = 0,         
    TDE_SRC_MODE_BMP   = 1,
    TDE_SRC_MODE_PATTERN = 2,
    TDE_SRC_MODE_FILL  = 3,
    TDE_SRC_QUICK_COPY = 5,
    TDE_SRC_QUICK_FILL = 7    
}TDE_SRC_MODE_E;

/* Sq control register's configured format, idiographic meanings can refer to register's handbook */
typedef union hiTDE_SQ_CTRL_U
{
    struct
    {
        HI_U32 u32TrigMode     :1;
        HI_U32 u32TrigCond     :1;
        HI_U32 u32Reserve1     :19;
        HI_U32 u32SqOperMode   :4;
        HI_U32 u32SqEn         :1;
        HI_U32 u32Reserve2     :5;
        HI_U32 u32SqUpdateFlag :1;
    }stBits;
    HI_U32 u32All;
}TDE_SQ_CTRL_U;

/* Aq control register's configured format, idiographic meanings can refer to register's handbook */
typedef union hiTDE_AQ_CTRL_U
{
    struct
    {
        HI_U32 u32Reserve1     :20; /* Reserve 1 */
        HI_U32 u32AqOperMode   :4;  /* Resume open mode in AQ list */
        HI_U32 u32AqEn         :1;  /* Enable AQ */
        HI_U32 u32Reserve2     :7;  /* Reserve 2 */
    }stBits;
    HI_U32 u32All;
}TDE_AQ_CTRL_U;

/* Operate manner register's configured format, idiographic meanings can refer to register's handbook */
typedef union hiTDE_INS_U
{
    struct
    {
        HI_U32 u32Src1Mod       :3;     /* source1 runnning mode */
        HI_U32 u32Src2Mod       :2;     /* source2 runnning mode */
        HI_U32 u32IcsConv       :1;     /* Enable to conversion of importing color zone */
        HI_U32 u32Resize        :1;     /* Enable to reszie on 2D*/
        HI_U32 u32DfeEn         :1;     /* Enable to Deflicker flicter */
#ifdef TDE_VERSION_MPW
        HI_U32 u32Reserve1      :3;     /* Reserve1 */
#else
	    HI_U32 u32Y1En		:1;		    /* Enable Y1 channel */	
	    HI_U32 u32Y2En		:1;		    /* Enable Y2 channel */
        HI_U32 u32Reserve1      :1;     /* Reserve 1 */
#endif
        HI_U32 u32Clut          :1;     /* Enable CLUT */
        HI_U32 u32ColorKey      :1;     /* Enable Colorkey */
        HI_U32 u32IcscSel      :1;      /* Select in importing color zone conversion */
        HI_U32 u32Clip          :1;     /* Enable Clip */
        HI_U32 u32MbMode        :3;     /* MB operate mode */
        HI_U32 u32MbEn          :1;     /* Enable to operate MB */
        HI_U32 u32AqIrqMask     :4;     /* Fake code of interrupt in AQ lsit */
        HI_U32 u32SqIrqMask     :5;     /* Fake code of interrupt in SQ lsit */
        HI_U32 u32OcsConv      :1;      /* Enable to conversion of exporting color zone */
        HI_U32 u32BlockFlag     :1;     /* Flag if block */
        HI_U32 u32FirstBlockFlag    :1; /* Flag that is block at first */
        HI_U32 u32LastBlockFlag     :1; /* Flag that is block last */
    }stBits;
    HI_U32 u32All;
}TDE_INS_U;

/* Source bitmap register's configured format, idiographic meanings can refer to register's handbook */
typedef union hiTDE_SRC_TYPE_U
{
    struct
    {
        HI_U32 u32Pitch        :16; /* line pitch */
        HI_U32 u32SrcColorFmt  :5;  /* pixel format */
        HI_U32 u32AlphaRange   :1;  /* 8bit alpha range */
#ifdef TDE_VERSION_MPW
        HI_U32 u32Reserve1     :2;  /* Reserve 1 */
#else
        HI_U32 u32ColorizeEnable:1; /* Enable Colorize,only effective for Src2, Src1 reserved */
        HI_U32 u32Reserve1     :1;  /* Reserve */
#endif
        HI_U32 u32HScanOrd     :1;  /* Horizontal scanning direction */
        HI_U32 u32VScanOrd     :1;  /* Vertical scanning direction */
        HI_U32 u32Reserve2     :3;  /* Reserve */
        HI_U32 u32RgbExp       :1;  /* RGB expand mode */
        HI_U32 u32Reserve3     :2;  /* Reserve */
    }stBits;
    HI_U32 u32All;
}TDE_SRC_TYPE_U;

/* Register's Config format at section start */
typedef union hiTDE_SUR_XY_U
{
    struct
    {
        HI_U32 u32X            :12; /* First X coordinate */
        HI_U32 u32Reserve1     :4;  /* Reserve */
        HI_U32 u32Y            :12; /* First Y coordinate */
        HI_U32 u32Reserve2     :4;  /* Reserve */
    }stBits;
    HI_U32 u32All;
}TDE_SUR_XY_U;

/* Bitmap size register's configured format, idiographic meanings can refer to register's handbook */
typedef union hiTDE_SUR_SIZE_U
{
    struct
    {
        HI_U32 u32Width        :12;             /* Image width */
        HI_U32 u32AlphaThresholdLow     :4;     /* The lowest 4 bit of alpha threshold, effective in size register of target, reserved in src2 size register */
        HI_U32 u32Height       :12;             /* Image height */
        HI_U32 u32AlphaThresholdHigh     :4;    /* The highest 4 bit of alpha threshold, effective in size register of target, reserved in src2 size register */
    }stBits;
    HI_U32 u32All;
}TDE_SUR_SIZE_U;

/* Target bitmap type register's configured format, idiographic meanings can refer to register's handbook */
typedef union hiTDE_TAR_TYPE_U
{
    struct
    {
        HI_U32 u32Pitch        :16;     /* line pitch */
        HI_U32 u32TarColorFmt  :5;      /* pixel format */
        HI_U32 u32AlphaRange   :1;      /* 8bit alpha range */
        HI_U32 u32AlphaFrom    :2;      /* Emporting alpha from */
        HI_U32 u32HScanOrd     :1;      /* Horizontal scanning direction */
        HI_U32 u32VScanOrd     :1;      /* Vertical scaning direction */
        HI_U32 u32RgbRound     :1;      /* RGB truncate bit mode */
        HI_U32 u32DfeLastlineOutEn  :1; /* Emporting enable to deflicker at last line */
        HI_U32 u32DfeFirstlineOutEn :1; /* Emporting enable to deflicker at first line */
        HI_U32 u32Reserve2     :3;      /* Reserve */
    }stBits;
    HI_U32 u32All;
}TDE_TAR_TYPE_U;

/* Color zone conversion register's configured format, idiographic meanings can refer to register's handbook */
typedef union hiTDE_COLOR_CONV_U
{
    struct
    {
        HI_U32 u32InColorImetry       :1;   /* Import color conversion standard  */
        HI_U32 u32InRgb2Yc       :1;        /* Import color conversion oreitation */
        HI_U32 u32InColorSpace  :1;         /* Import matrix of color conversion used */
        HI_U32 u32InChromaFmt   :1;         /* Import chroma format */
        HI_U32 u32OutColorImetry    :1;     /* Export color conversion standard */
        HI_U32 u32OutColorSpace     :1;     /* Export matrix of color conversion */
        HI_U32 u32OutChromaFmt      :1;     /* Export chroma format */
        HI_U32 u32Reserve1       :2;        /* Reserve */     
        HI_U32 u32ClutMod        :1;        /* clut mode */
        HI_U32 u32ClutReload     :1;        /* Enable to update clut table */
#ifdef TDE_VERSION_MPW
        HI_U32 u32Reserve2       :5;        /* Reserve */
#else
        HI_U32 u32Reserve2       :1;        /* Reserve */
        HI_U32 u32OcscReload	:1;	        /* Enable to reload exported CSC self-defined arguments */
        HI_U32 u32OcscCustomEn	:1;	        /* Enable to self-defined export CSC */
        HI_U32 u32IcscReload	:1;	        /* Enable to reload inported CSC self-defined arguments */
        HI_U32 u32IcscCustomEn	:1;	        /* Enable to self-defined inported CSC */
#endif
        HI_U32 u32Alpha0         :8;        /* alpha1555 format, 0:alpha value */
        HI_U32 u32Alpha1         :8;        /* alpha1555 format, 1:alpha value */
    }stBits;
    HI_U32 u32All;
}TDE_COLOR_CONV_U;

/* Zoom register's configured format, idiographic meanings can refer to register's handbook */
typedef union hiTDE_2D_RSZ_U
{
    struct
    {
        HI_U32 u32HfMod        :2;      /* Horizontal filter mode */
        HI_U32 u32Reserve1     :2;      /* Reserve */
        HI_U32 u32VfMod        :2;      /* Vertical filter mode */
        HI_U32 u32Reserve2     :2;      /* Reserve */
        HI_U32 u32DfeMod       :2;      /* Select in deflicker coefficient */
        HI_U32 u32Reserve3     :2;      /* Reserve */
        HI_U32 u32AlpBorder    :2;      /* alpha border handling mode */ 
        HI_U32 u32Reserve4     :2;      /* Reserve */
        HI_U32 u32VfRingEn     :1;      /* Enable to median filter,when vertical filter enable */
        HI_U32 u32HfRingEn     :1;      /* Enable to median filter,when horizontal filter enable */
        HI_U32 u32CoefSym      :1;      /* Property of filter coefficient */
        HI_U32 u32Reserve5     :5;      /* Reserve */
        HI_U32 u32VfCoefReload :1;      /* Enable to update vertical filter coefficient */
        HI_U32 u32HfCoefReload :1;      /* Enable to update horizontal filter coefficient */
        HI_U32 u32DfeAlphaDisable   :1; /* Unable to alpha deflicker */
        HI_U32 u32Reserve6     :5;      /* Reserve */
    }stBits;
    HI_U32 u32All;
}TDE_2D_RSZ_U;

/* Bitmap size register's configured format, idiographic meanings can refer to register's handbook */
typedef union hiTDE_ALU_U
{
    struct
    {
        HI_U32 u32AluMod        :4;     /* ALU running mode */      
        HI_U32 u32AlphaThreshodEn  :1;  /* Enable to threshold alpha */
        HI_U32 u32AlphaRopMod   :4;     /* Rop operate mode of alpha */
        HI_U32 u32RgbRopMod     :4;     /* Rop operate mode of RGB */
        HI_U32 u32GlobalAlpha   :8;     /* Global alpha value */
        HI_U32 u32CkBMod        :2;     /* Colorkey blue component operating mode */
        HI_U32 u32CkGMod        :2;     /* Colorkey green component operating mode */
        HI_U32 u32CkRMod        :2;     /* Colorkey red component operating mode */
        HI_U32 u32CkAMod        :2;     /* Colorkey alpha component operating mode */
        HI_U32 u32CkSel         :2;     /* Colorkey inporting selection */
        HI_U32 u32BlendReverse      :1; /* Enable to reserve blending fore and background */
    }stBits;
    HI_U32 u32All;
}TDE_ALU_U;

/* ARGB/AYUV component order register format */
typedef union hiTDE_ARGB_ORDER_U
{
    struct
    {
        HI_U32 u32Src1ArgbOrder :   5;  /* Src1's ARGB order */
        HI_U32 u32Reserved1 :   3;      /* Reserve */
        HI_U32 u32Src2ArgbOrder :   5;  /* Src2's ARGB order */
        HI_U32 u32Reserved2 :   3;      /* Reserve */
        HI_U32 u32TarArgbOrder  :   5;  /* Target's ARGB order */
        HI_U32 u32Reserved3 :   3;      /* Reserve */
    }stBits;
    HI_U32 u32All;
}TDE_ARGB_ORDER_U;

/* Colorkey Mask Register */
typedef union hiTDE_COLORKEY_MASK_U
{
    struct
    {
        HI_U32 u32BlueMask  :   8;  /* Blue component /clut/ V component mask */
        HI_U32 u32GreenMask :   8;  /* Green component / U component mask */
        HI_U32 u32RedMask   :   8;  /* Red component /Y component mask */
        HI_U32 u32AlphaMask :   8;  /* Alpha component mask */
    }stBits;
    HI_U32 u32All;
}TDE_COLORKEY_MASK_U;

/* alpha blend mode register */
typedef union hiTDE2_ALPHA_BLEND_U
{
    struct
    {
        HI_U32 u32Src1BlendMode :       4;      /* Src1's blending mode */
        HI_U32 u32Src1PremultiEn :      1;      /* Enable to pre-multiply alpha */
        HI_U32 u32Src1MultiGlobalEn :   1;      /* Enable to Src1 pre-multi global alpha */
        HI_U32 u32Src1PixelAlphaEn    :   1;    /* Enable to Src1 pixel alpha */
        HI_U32 u32Src1GlobalAlphaEn   :   1;    /* Enable to Src1 global alpha */
        HI_U32 u32Src2BlendMode :       4;      /* Blend mode in Src2 */
        HI_U32 u32Src2PremultiEn :      1;      /* Enable to Src2 pre-multi alpha */
        HI_U32 u32Src2MultiGlobalEn :   1;      /* Enable to Src2 pre-multi global alpha */
        HI_U32 u32Src2PixelAlphaEn    :   1;    /* Enable to Src2 pixel alpha */
        HI_U32 u32Src2GlobalAlphaEn   :   1;    /* Enable to Src2 global alpha*/
        HI_U32 u32AlphaRopEn    :   1;          /* Enable to alpha,when alpha blending */
        HI_U32 u32Reserved  :   15;             /* Reserve */
    }stBits;
    HI_U32 u32All;
}TDE_ALPHA_BLEND_U;

typedef union hiTDE_Y_PITCH
{
    struct
    {
        HI_U32 u32Pitch     :   16; /* line pitch */
        HI_U32 u32Reserved	:16;	/* Reserve */
    }stBits;
    HI_U32 u32All;
}TDE_Y_PITCH_U;

/* Filter argument table */
typedef union hiTDE_FILTER_PARA_U
{
    struct
    {
        HI_U32 u32Para0        :5;
        HI_U32 u32Para1        :6;
        HI_U32 u32Para2        :8;
        HI_U32 u32Para3        :7;
        HI_U32 u32Para4        :6;
    }stBits;
    HI_U32 u32All;
}TDE_FILTER_PARA_U;

typedef union hiTDE_BUSLIMITER_U
{
    struct
    {
        HI_U32 u32BwCnt :   10;
        HI_U32 u32CasRidSel :   1;
        HI_U32 u32Src1RidSel    :   1;
        HI_U32 u32Src2RidSel    :   1;
        HI_U32 u32Y1RidSel  :   1;
        HI_U32 u32Y2RidSel  :   1;
        HI_U32 u32Reserved1  :   1;
        HI_U32 u32WOutstdMax    :   4;
        HI_U32 u32ROutstdMax    :   4;
        HI_U32 u32Reserved2 :   7;
        HI_U32 u32ClkCtlEn  :   1;
    }stBits;
    HI_U32 u32All;
}TDE_BUSLIMITER_U;

/* List of each hardware node, using to maitain hardware node */
typedef struct hiTDE_BUF_NODE_S
{
    struct list_head stList;
    HI_U32 u32PhyAddr;              /* Point to physical address of hardware buffer */
    HI_U32* pu32VirAddr;            /* Virtual address responding to hardware buffer */
    HI_S32 s32Handle;               /* Current node's job handle */
    HI_VOID* pSwNode;               /* Software node pointer of current node */
    HI_U32 u32WordSize;             /* Size of current node config is needed, united by word */
    HI_U32 u32CurUpdate;            /* Update flag of current node */
    struct hiTDE_BUF_NODE_S* pstParentAddr; /* Parent node addr of configed information inherited */
}TDE_BUF_NODE_S;


/* State information when TDE operate is suspend */
typedef struct hiTDE_SUSP_STAT_S
{
    HI_S32 s32AqSuspLine;       /* Line executed while AQ is suspend ,if -1 , present no effection */
    TDE_BUF_NODE_S* pstSwBuf;
    HI_VOID *pSwNode;       	/* Pointer point to software node be suspend */
}TDE_SUSP_STAT_S;

/* Head address of argument table used as config */
typedef struct hiTDE_PARA_TABLE_S
{
    HI_U32 u32HfCoefAddr;
    HI_U32 u32VfCoefAddr;
}TDE_PARA_TABLE_S;

/****************************************************************************/
/*                         TDE hal inner variable definition                */
/****************************************************************************/
/* Base addr of register after mapping */
STATIC volatile HI_U32* s_pu32BaseVirAddr = HI_NULL;

/* State information when Aq is suspend */
STATIC TDE_SUSP_STAT_S s_stSuspStat = {0};

/* Head address of config argument table */
STATIC TDE_PARA_TABLE_S s_stParaTable = {0};

/* Deflicker level, default is auto */
STATIC TDE_DEFLICKER_LEVEL_E s_eDeflickerLevel = TDE_DEFLICKER_AUTO;

/* alpha threshold switch */
STATIC HI_BOOL s_bEnAlphaThreshold = HI_FALSE;

/*alpha threshold value */
STATIC HI_U8 s_u8AlphaThresholdValue = 0xff;

#define TDE_CSCTABLE_SIZE 7

STATIC HI_U32 s_u32Rgb2YuvCsc[TDE_CSCTABLE_SIZE] = 
    {0x0096004d, 0xffd5001d, 0x0080ffab, 0xff950080, 0x0000ffeb, 0x00000000, 0x02010000};
STATIC HI_U32 s_u32Yuv2RgbCsc[TDE_CSCTABLE_SIZE] = 
    {0x00000100, 0x01000167, 0xff49ffa8, 0x01c60100, 0x00000000, 0x06030000, 0x00000000};

STATIC HI_U32 s_u32Rgb2YuvCoefAddr = 0;
STATIC HI_U32 s_u32Yuv2RgbCoefAddr = 0;

//STATIC HI_CHIP_TYPE_E s_enChipType = HI_CHIP_TYPE_BUTT;
//STATIC HI_CHIP_VERSION_E s_enChipVersion = HI_CHIP_VERSION_BUTT;

/****************************************************************************/
/*                             TDE hal inner function definition            */
/****************************************************************************/
STATIC HI_S32 TdeHalInitParaTable(HI_VOID);
STATIC INLINE HI_S32 TdeHalGetOffsetInNode(HI_U64 u64MaskUpdt, HI_U64 u64Update);
STATIC INLINE HI_U32 TdeHalCurLine(HI_VOID);
STATIC HI_VOID TdeHalNodeSetSrc2Base(TDE_HWNode_S* pHWNode, TDE_DRV_SURFACE_S* pDrvSurface, HI_BOOL bS2Size);
STATIC HI_VOID TdeHalNodeSetSrc2BaseEx(TDE_HWNode_S* pHWNode, TDE_DRV_SURFACE_S* pDrvSurface, HI_BOOL bS2Size);
STATIC HI_S32  TdeHalGetbppByFmt(TDE_DRV_COLOR_FMT_E enFmt);
STATIC INLINE HI_U32 TdeHalGetResizeParaHTable(HI_U32 u32Step);
STATIC INLINE HI_U32 TdeHalGetResizeParaVTable(HI_U32 u32Step);
STATIC INLINE HI_VOID TdeHalSetChildToParent(HI_U32* pParent, HI_U32* pChild, HI_U32 u32ParentUpdt, HI_U32 ChildUpdt);
//STATIC INLINE TDE_NODE_BUF_S* TdeHalUpdateChildBuf(TDE_NODE_BUF_S* pNodeBuf);
STATIC INLINE HI_VOID TdeHalSetXyByAdjInfo(TDE_HWNode_S* pHWNode, TDE_CHILD_INFO* pChildInfo);
STATIC INLINE HI_VOID TdeHalInitQueue(HI_VOID);
HI_VOID TdeHalSetClock(HI_VOID);

extern HI_VOID HI_DRV_SYS_GetChipVersion(HI_CHIP_TYPE_E *penChipType, HI_CHIP_VERSION_E *penChipID);

/****************************************************************************/
/*                TDE hal ctl interface realization                         */
/****************************************************************************/

/*****************************************************************************
* Function:      TdeHalInit
* Description:   map the base address for tde
* Input:         u32BaseAddr: the base address of tde
* Output:        none
* Return:        success/fail
* Others:        none
*****************************************************************************/
HI_S32 TdeHalInit(HI_U32 u32BaseAddr)
{
    s_stSuspStat.s32AqSuspLine = TDE_SUSP_LINE_INVAL;
    s_stSuspStat.pSwNode = HI_NULL;

    /*init the pool memery of tde*//*CNcomment:初始化TDE内存池*/
    if (HI_SUCCESS != wmeminit())
    {
        goto TDE_INIT_ERR;
    }

    /*config start address for the parameter  *//*CNcomment: 配置参数表首地址 */
    if (HI_SUCCESS != TdeHalInitParaTable())
    {
        goto TDE_INIT_ERR;
    }

     /*map address for the register *//*CNcomment:寄存器映射*/
    s_pu32BaseVirAddr = (volatile HI_U32*)HI_GFX_REG_MAP(u32BaseAddr, TDE_REG_SIZE);
    if(HI_NULL == s_pu32BaseVirAddr)
    {
        goto TDE_INIT_ERR;
    }

    TdeHalSetClock();
		
    TdeHalCtlReset();

    TdeHalInitQueue(); /* init SQ/AQ *//*CNcomment: 初始化SQ/AQ */
    
    return HI_SUCCESS;

TDE_INIT_ERR:
    return HI_FAILURE;
}

HI_VOID TdeHalResumeInit(HI_VOID)
{
    TdeHalSetClock();

    TdeHalCtlReset();

    TdeHalInitQueue();

    return;
}

/*****************************************************************************
* Function:      TdeHalOpen
* Description:   open the tde
* Input:         none
* Output:        none
* Return:        none
* Others:        none
*****************************************************************************/
HI_S32 TdeHalOpen(HI_VOID)
{
    /* do sth ... */
    return HI_SUCCESS;
}

/*****************************************************************************
* Function:      TdeHalRelease
* Description:   release the address that had map
* Input:         none
* Output:        none
* Return:        success/fail
* Others:        none
*****************************************************************************/
HI_VOID TdeHalRelease(HI_VOID)
{
    HI_VOID *pBuf;
    if(0 != s_stParaTable.u32HfCoefAddr)
    {
        pBuf = (HI_VOID *)wgetvrt(s_stParaTable.u32HfCoefAddr);
        if (HI_NULL != pBuf)
        {
            TDE_FREE(pBuf);
            s_stParaTable.u32HfCoefAddr = 0;
        }
    }
    if (0 != s_stParaTable.u32VfCoefAddr)
    {
        pBuf = (HI_VOID *)wgetvrt(s_stParaTable.u32VfCoefAddr);
        if (HI_NULL != pBuf)
        {
            TDE_FREE(pBuf);
            s_stParaTable.u32VfCoefAddr = 0;
        }
    }

#ifdef TDE_VERSION_PILOT
    if (0 != s_u32Rgb2YuvCoefAddr)
    {
        pBuf = (HI_VOID *)wgetvrt(s_u32Rgb2YuvCoefAddr);
        if (HI_NULL != pBuf)
        {
            TDE_FREE(pBuf);
            s_u32Rgb2YuvCoefAddr = 0;
        }
    }

    if (0 != s_u32Yuv2RgbCoefAddr)
    {
        pBuf = (HI_VOID *)wgetvrt(s_u32Yuv2RgbCoefAddr);
        if (HI_NULL != pBuf)
        {
            TDE_FREE(pBuf);
            s_u32Yuv2RgbCoefAddr = 0;
        }
    }
#endif

    /* unmap the base address*//*CNcomment:  反映射基地址 */
    HI_GFX_REG_UNMAP(s_pu32BaseVirAddr);
    s_pu32BaseVirAddr = HI_NULL;

     /*free the pool of memery*//*CNcomment:TDE内存池去初始化*/
    wmemterm();
    
    return;
}

/*****************************************************************************
* Function:      TdeHalCtlIsIdle
* Description:   get the state of tde
* Input:         none
* Output:        none
* Return:        True: Idle/False: Busy
* Others:        none
*****************************************************************************/
HI_BOOL TdeHalCtlIsIdle(HI_VOID)
{
    return (HI_BOOL)(!(TDE_READ_REG(s_pu32BaseVirAddr, TDE_STA) & 0x1));
}

/*****************************************************************************
* Function:      TdeHalCtlIsIdleSafely
* Description:   get the state of tde one more time ,make sure it's idle
* Input:         none
* Output:        none
* Return:        True: Idle/False: Busy
* Others:        none
*****************************************************************************/
HI_BOOL TdeHalCtlIsIdleSafely(HI_VOID)
{
    HI_U32 i = 0;
    
    /*get the state of tde one more time ,make sure it's idle *//*CNcomment: 连续读取多次硬件状态,确保TDE完成 */
    for (i = 0; i < TDE_MAX_READ_STATUS_TIME; i++)
    {
        if (!TdeHalCtlIsIdle())
        {
            return HI_FALSE;
        }
    }
    return HI_TRUE;
}

/*****************************************************************************
* Function:      TdeHalCtlIntMask
* Description:   get the state of interrupt for Sq/Aq
* Input:         enListType: type of Sq/Aq
* Output:        none
* Return:        the interrupt state of Sq/Aq
* Others:        none
*****************************************************************************/
HI_U32 TdeHalCtlIntStats(HI_VOID)
{
    HI_U32 Value;

    Value = TDE_READ_REG(s_pu32BaseVirAddr, TDE_INT);
    /* clear all status */
    TDE_WRITE_REG(s_pu32BaseVirAddr, TDE_INT, (Value)/*0x800f001f*/);

    return Value;
}

/*****************************************************************************
* Function:      TdeHalResetStatus
* Description:   reset the hardware and clear all list
* Input:         none
* Output:       none
* Return:        none
* Others:       none
*****************************************************************************/
HI_VOID TdeHalResetStatus(HI_VOID)
{
    s_stSuspStat.s32AqSuspLine = TDE_SUSP_LINE_INVAL;
    //s_stSuspStat.pstSwBuf = HI_NULL;
    s_stSuspStat.pSwNode = HI_NULL;
    return;
}

/*****************************************************************************
* Function:      TdeHalCtlReset
* Description:   reset by software, this operation will clear the interrupt state
* Input:         none
* Output:       none
* Return:        none
* Others:        none
*****************************************************************************/
HI_VOID TdeHalCtlReset(HI_VOID)
{
    /*reset *//*CNcomment: 复位 */
    TDE_WRITE_REG(s_pu32BaseVirAddr, TDE_RST, 0xffffffff);

    /* clear the state of interrupt*//*CNcomment: 请中断状态 */
    TDE_WRITE_REG(s_pu32BaseVirAddr, TDE_INT, 0x800f001f);

    return;
}

HI_VOID TdeHalSetClock(HI_VOID)
{
    U_PERI_CRG37 unTempValue;

    unTempValue.u32 = g_pstRegCrg->PERI_CRG37.u32;
    
    /*cancel reset*/
    unTempValue.bits.tde_srst_req = 0x0;

    /*enable clock*/
    unTempValue.bits.tde_cken = 0x1;

    g_pstRegCrg->PERI_CRG37.u32 = unTempValue.u32;
    
    return;
}

/*****************************************************************************
* Function:      TdeHalCtlIntClear
* Description:   clear the state of interrupt
* Input:         enListType: type of Sq/Aq
*                u32Stats: state that need clear
* Output:        none
* Return:        none
* Others:        none
*****************************************************************************/
HI_VOID TdeHalCtlIntClear(TDE_LIST_TYPE_E enListType, HI_U32 u32Stats)
{
    HI_U32 u32ReadStats = 0;

    u32ReadStats = TDE_READ_REG(s_pu32BaseVirAddr, TDE_INT);
    if (TDE_LIST_AQ == enListType)
    {
        u32ReadStats = (u32ReadStats & 0x0000ffff) | ((u32Stats << 16) & 0xffff0000);
    }
    else
    {
        u32ReadStats = (u32ReadStats & 0xffff0000) | (u32Stats & 0x0000ffff);
    }
    TDE_WRITE_REG(s_pu32BaseVirAddr, TDE_INT, u32ReadStats);
}

/*get the queue of tde*//*CNcomment:判断TDE的工作队列是同步还是异步*/
HI_BOOL TdeHalIsSqWork()
{
    return (HI_BOOL)((TDE_READ_REG(s_pu32BaseVirAddr, TDE_STA) & 0x4) >> 2);
}

/****************************************************************************/
/*                             TDE hal node interface                    */
/****************************************************************************/

/*****************************************************************************
* Function:      TdeHalNodeInitNd
* Description:   init the software node struct for tde
* Input:         pHWNode: the pointer of software node struct
* Output:        none
* Return:        none
* Others:        none
*****************************************************************************/
HI_VOID TdeHalNodeInitNd(TDE_HWNode_S* pHWNode, HI_BOOL bChild)
{
    TDE_INS_U unIns;

    TDE_ASSERT(HI_NULL != pHWNode);

     /*it need set father's info and clear Update info when node is child*/
     /*CNcomment: 如果是子节点, 还需要父节点的设置信息,只把Update清空 */
    if (bChild)
    {
        TDE_2D_RSZ_U unRsz;
        HI_BOOL bClipOut = (pHWNode->u32TDE_CLIP_START >> 31);
	  
        unRsz.u32All = pHWNode->u32TDE_2D_RSZ;
        
        /*it's not need set parameter info when node is child*/
        /*CNcomment: 如果是子节点则不需要更新参数表 */
        unRsz.stBits.u32VfCoefReload = 0;
        unRsz.stBits.u32HfCoefReload = 0;
        pHWNode->u32TDE_2D_RSZ = unRsz.u32All;
        pHWNode->u64TDE_UPDATE = 0;
        TDE_SET_UPDATE(u32TDE_2D_RSZ, pHWNode->u64TDE_UPDATE);

	 if(bClipOut)
	 {
        TDE_SET_UPDATE(u32TDE_CLIP_START, pHWNode->u64TDE_UPDATE);
        TDE_SET_UPDATE(u32TDE_CLIP_STOP, pHWNode->u64TDE_UPDATE);
	 }
    }
    else /*clear all info*//*CNcomment:否则所有信息清空 */
    {
        memset(pHWNode, 0, sizeof(TDE_HWNode_S));
    }
    
    /*open interrupt*//*CNcomment:开启链表完成中断,
    同步链表可更新中断, 关闭节点完成中断 */
    unIns.u32All = pHWNode->u32TDE_INS;
    unIns.stBits.u32SqIrqMask = TDE_SQ_UPDATE_MASK_EN | TDE_SQ_COMP_LIST_MASK_EN | TDE_SQ_CUR_LINE_MASK_EN;
    unIns.stBits.u32AqIrqMask = TDE_AQ_SUSP_MASK_EN | TDE_AQ_COMP_LIST_MASK_EN;
    pHWNode->u32TDE_INS = unIns.u32All;
    TDE_SET_UPDATE(u32TDE_INS, pHWNode->u64TDE_UPDATE);
}

/*modify bug*//*CNcomment: AE5D03390:软件规避区域外clip bug */
HI_VOID TdeHalNodeInitChildNd(TDE_HWNode_S* pHWNode, HI_U32 u32TDE_CLIP_START, HI_U32 u32TDE_CLIP_STOP)
{
    TDE_INS_U unIns;
    TDE_2D_RSZ_U unRsz;
    HI_BOOL bClipOut;

    TDE_ASSERT(HI_NULL != pHWNode);

    bClipOut = pHWNode->u32TDE_CLIP_START >> 31;

     /*it need set father's info and clear Update info when node is child*/
     /*CNcomment:如果是子节点, 还需要父节点的设置信息,只把Update清空 */

    unRsz.u32All = pHWNode->u32TDE_2D_RSZ;

    /*it's not need set parameter info when node is child*/
    /*CNcomment: 如果是子节点则不需要更新参数表 */
    unRsz.stBits.u32VfCoefReload = 0;
    unRsz.stBits.u32HfCoefReload = 0;
    pHWNode->u32TDE_2D_RSZ = unRsz.u32All;
    pHWNode->u64TDE_UPDATE = 0;
    TDE_SET_UPDATE(u32TDE_2D_RSZ, pHWNode->u64TDE_UPDATE);

    if(bClipOut)
    {
        pHWNode->u32TDE_INS = pHWNode->u32TDE_INS | 0x4000; /*open clip function*//* CNcomment: 开启clip 功能*/
        pHWNode->u32TDE_CLIP_START = u32TDE_CLIP_START;
        pHWNode->u32TDE_CLIP_STOP = u32TDE_CLIP_STOP;
        TDE_SET_UPDATE(u32TDE_CLIP_START, pHWNode->u64TDE_UPDATE);
        TDE_SET_UPDATE(u32TDE_CLIP_STOP, pHWNode->u64TDE_UPDATE);
    }
    
    /*open interrupt*//* CNcomment:  开启链表完成中断,同步链表可更新中断, 关闭节点完成中断 */
    unIns.u32All = pHWNode->u32TDE_INS;
    unIns.stBits.u32SqIrqMask = TDE_SQ_UPDATE_MASK_EN | TDE_SQ_COMP_LIST_MASK_EN;
    unIns.stBits.u32AqIrqMask = TDE_AQ_SUSP_MASK_EN | TDE_AQ_COMP_LIST_MASK_EN;
    pHWNode->u32TDE_INS = unIns.u32All;
    TDE_SET_UPDATE(u32TDE_INS, pHWNode->u64TDE_UPDATE);
}

/*****************************************************************************
* Function:      TdeHalNodeGetNdSize
* Description:  get node size that is config the function
* Input:         pHWNode: the pointer of software node
* Output:        none
* Return:        return size of node
* Others:        none
*****************************************************************************/
HI_U32 TdeHalNodeGetNdSize(TDE_HWNode_S* pHWNode)
{
    HI_S32 u32Size = 0;
    HI_U32 i = 0;

    TDE_ASSERT(HI_NULL != pHWNode);

    /*calc the configer size that is set by u64TDE_UPDATE*/
    /* CNcomment:  统计u64TDE_UPDATE中的有效设置,
    计算当前配置下所需大小 */
    for (i = 0; i < sizeof(TDE_HWNode_S)/4 - 2; i++)
    {
        if ( 1 == ((pHWNode->u64TDE_UPDATE >> i) & 1) )
        {
            u32Size++;
        }
    }
    
    return (u32Size << 0x2); /*return the size according word*//* CNcomment:  根据Word返回字节数 */
}

/*****************************************************************************
* Function:      TdeHalNodeMakeNd
* Description:   set the config to node buf according node
* Input:         pBuf: the node buf
*                pHWNode:the pointer of node
* Output:        none
* Return:        none
* Others:        none
*****************************************************************************/
HI_S32 TdeHalNodeMakeNd(HI_VOID* pBuf, TDE_HWNode_S* pHWNode)
{
    HI_U32 *pNodeBuf = (HI_U32*)pBuf;
    HI_U32 *pu32Node = (HI_U32*)pHWNode;
    HI_U32 i = 0;
    
    TDE_ASSERT(HI_NULL != pBuf);
    TDE_ASSERT(HI_NULL != pHWNode);

    /*copy data from node to buf of node*//* CNcomment:  将pHWNode中的有效数据复制到保存节点的buffer中 */
    for (i = 0; i < (sizeof(TDE_HWNode_S) - sizeof(pHWNode->u64TDE_UPDATE))/4; i++)
    {
        if ( 1 == ((pHWNode->u64TDE_UPDATE >> i) & 1) )
        {
            *(pNodeBuf++) = *(pu32Node + i);
        }
    }

    return HI_SUCCESS;
}

/*****************************************************************************
* Function:      TdeHalNodeExecute
* Description:  start list of tde
* Input:         enListType: type of list
*                u32NodePhyAddr: the start address of head node address
*                u64Update:the head node update set
*                bAqUseBuff: whether use temp buffer
* Output:        none
* Return:        none
* Others:        none
*****************************************************************************/
HI_S32 TdeHalNodeExecute(TDE_LIST_TYPE_E enListType, HI_U32 u32NodePhyAddr, HI_U64 u64Update, HI_BOOL bAqUseBuff)
{
    TDE_AQ_CTRL_U unAqCtrl;
    TDE_SQ_CTRL_U unSqCtrl;
    
    if (enListType == TDE_LIST_AQ)
    {
        /*tde is idle*//* CNcomment:TDE空闲*/
        if(TdeHalCtlIsIdleSafely())
        {
            /*write the first node address*//* CNcomment:写入首节点地址*/
            TDE_WRITE_REG(s_pu32BaseVirAddr, TDE_AQ_NADDR, u32NodePhyAddr);

            /*write the first node update area*//* CNcomment:写入首节点更新标识*/
            TDE_WRITE_REG(s_pu32BaseVirAddr, TDE_AQ_UPDATE, (HI_U32)(u64Update & 0xffffffff));
            
#if defined (TDE_VERSION_PILOT) || defined (TDE_VERSION_FPGA)
            TDE_WRITE_REG(s_pu32BaseVirAddr, TDE_AQ_UPDATE2, (HI_U32)((u64Update >> 32) & 0xffffffff));
#endif

            unAqCtrl.u32All = TDE_READ_REG(s_pu32BaseVirAddr, TDE_AQ_CTRL);

            /*modiyf the break off to finish current node when use the temp buffer */
            /* CNcomment:若需要使用临时buffer,
            则打断模式为当前节点完成打断,否则为当前节点
               当前行完成打断*/
            if (HI_TRUE ==  bAqUseBuff)
            {
                unAqCtrl.stBits.u32AqOperMode = TDE_AQ_CTRL_COMP_LIST;
                TDE_TRACE(TDE_KERN_DEBUG, "Aq Ctrl use comp list mode\n");
            }
            else
            {
                unAqCtrl.stBits.u32AqOperMode = TDE_AQ_CTRL_COMP_LINE;
                TDE_TRACE(TDE_KERN_DEBUG, "Aq Ctrl use comp node line mode\n");
            }

            TDE_WRITE_REG(s_pu32BaseVirAddr, TDE_AQ_CTRL, unAqCtrl.u32All);

            mb();

            /*start Aq list*//* CNcomment:启动Aq*/
            TDE_WRITE_REG(s_pu32BaseVirAddr, TDE_CTRL, 0x1);   
        }
        else
        {
            return HI_FAILURE;
        }
    }
    else
    {
        /*it can submit the Sq list when idle and Aq is running*//* CNcomment:TDE空闲或异步链表在工作，都可提交同步链表*/
        if(TdeHalCtlIsIdle() || ((TDE_READ_REG(s_pu32BaseVirAddr, TDE_STA) & 0x4) >> 2))
        {
            /*write the first node address*//* CNcomment:写入首节点地址*/
            TDE_WRITE_REG(s_pu32BaseVirAddr, TDE_SQ_NADDR, u32NodePhyAddr);

             /*write the first node need update area*//* CNcomment:写入首节点更新标识*/
            TDE_WRITE_REG(s_pu32BaseVirAddr, TDE_SQ_UPDATE, (HI_U32)(u64Update & 0xffffffff));
            
#if defined (TDE_VERSION_PILOT) || defined (TDE_VERSION_FPGA)
            TDE_WRITE_REG(s_pu32BaseVirAddr, TDE_SQ_UPDATE2, (HI_U32)((u64Update >> 32) & 0xffffffff));
#endif
             /*enable the Sq list*//* CNcomment:使能同步链表*/
            unSqCtrl.u32All = TDE_READ_REG(s_pu32BaseVirAddr, TDE_SQ_CTRL);

            unSqCtrl.stBits.u32SqEn = 0x1;
            unSqCtrl.stBits.u32SqUpdateFlag = 0x1;

            TDE_WRITE_REG(s_pu32BaseVirAddr, TDE_SQ_CTRL, unSqCtrl.u32All);

            /*start the Sq list*//* CNcomment: 启动Sq*/
            //TDE_WRITE_REG(s_pu32BaseVirAddr, TDE_CTRL, 0x101);
        }
        else
        {
            return HI_FAILURE;
        }
    }
    
    return HI_SUCCESS;
}

/*****************************************************************************
* Function:      TdeHalNodeEnableCompleteInt
* Description:   enable the finish interrupt of node
* Input:         pBuf: buffer of node
*                enType: type of list
* Output:        none
* Return:        none
* Others:        none
*****************************************************************************/
HI_VOID TdeHalNodeEnableCompleteInt(HI_VOID* pBuf, TDE_LIST_TYPE_E enType)
{
    HI_U32 *pu32Ins = (HI_U32*)pBuf;
    TDE_INS_U unIns;
    HI_U32 u32Mask;

    TDE_ASSERT(HI_NULL != pBuf);
    unIns.u32All = *pu32Ins;
    if (TDE_LIST_AQ == enType)
    {
        u32Mask = unIns.stBits.u32AqIrqMask;
        unIns.stBits.u32AqIrqMask = TDE_AQ_COMP_NODE_MASK_EN | u32Mask;
    }
    else
    {
        u32Mask = unIns.stBits.u32SqIrqMask;
        unIns.stBits.u32SqIrqMask = TDE_SQ_COMP_NODE_MASK_EN | u32Mask;
    }
    *pu32Ins = unIns.u32All;
}

HI_VOID TdeHalNodeComplteNd(TDE_LIST_TYPE_E enListType)
{
    return;
}

/*****************************************************************************
* Function:      TdeHalNodeSetSrc1
* Description:   set the info for source of bitmap 1
* Input:         pHWNode: pointer of node
*                pDrvSurface: bitmap info
* Output:        none
* Return:        none
* Others:        none
*****************************************************************************/
HI_VOID TdeHalNodeSetSrc1(TDE_HWNode_S* pHWNode, TDE_DRV_SURFACE_S* pDrvSurface)
{
    TDE_SRC_TYPE_U unSrcType;
    TDE_SUR_XY_U unXy;
    TDE_Y_PITCH_U unYPitch;
    TDE_ARGB_ORDER_U unArgbOrder;
    TDE_INS_U unIns;
    TDE_ASSERT(HI_NULL != pHWNode);
    TDE_ASSERT(HI_NULL != pDrvSurface);

    /*set the source bitmap attribute info*//*CNcomment:配置源位图属性信息*/
    unSrcType.u32All = pHWNode->u32TDE_S1_TYPE;
    unSrcType.stBits.u32SrcColorFmt = (HI_U32)pDrvSurface->enColorFmt;
    unSrcType.stBits.u32AlphaRange = (HI_U32)pDrvSurface->bAlphaMax255;
    unSrcType.stBits.u32HScanOrd = (HI_U32)pDrvSurface->enHScan;
    unSrcType.stBits.u32VScanOrd = (HI_U32)pDrvSurface->enVScan;
    /*file zero of low area and top area use low area extend*//*CNcomment:一直使用低位填充为0,高位使用低位的扩展方式*/
    unSrcType.stBits.u32RgbExp = 0; 
   
    unXy.u32All = pHWNode->u32TDE_S1_XY;
    unXy.stBits.u32X = pDrvSurface->u32Xpos;
    unXy.stBits.u32Y = pDrvSurface->u32Ypos;

#if defined (TDE_VERSION_PILOT) || defined (TDE_VERSION_FPGA)
    /*MB format*//*CNcomment:如果是宏块格式*/
    if (pDrvSurface->enColorFmt >= TDE_DRV_COLOR_FMT_YCbCr400MBP)
    {
        unSrcType.stBits.u32Pitch = pDrvSurface->u32CbCrPitch;
        unYPitch.stBits.u32Pitch = pDrvSurface->u32Pitch;

	 unIns.u32All = pHWNode->u32TDE_INS;
	 unIns.stBits.u32Y1En = HI_TRUE;

        pHWNode->u32TDE_S1_ADDR = pDrvSurface->u32CbCrPhyAddr;
        pHWNode->u32TDE_Y1_ADDR = pDrvSurface->u32PhyAddr;
        pHWNode->u32TDE_Y1_PITCH = unYPitch.u32All;
	 pHWNode->u32TDE_INS = unIns.u32All;

        TDE_SET_UPDATE(u32TDE_Y1_ADDR, pHWNode->u64TDE_UPDATE);
        TDE_SET_UPDATE(u32TDE_Y1_PITCH, pHWNode->u64TDE_UPDATE);
	 TDE_SET_UPDATE(u32TDE_INS, pHWNode->u64TDE_UPDATE);
    }
    else
#endif
    {
        unSrcType.stBits.u32Pitch = pDrvSurface->u32Pitch;
        pHWNode->u32TDE_S1_ADDR = pDrvSurface->u32PhyAddr;
        
#if defined (TDE_VERSION_PILOT) || defined (TDE_VERSION_FPGA)
        if (pDrvSurface->enColorFmt <= TDE_DRV_COLOR_FMT_ARGB8888)
        {
            unArgbOrder.u32All = pHWNode->u32TDE_ARGB_ORDER;
            unArgbOrder.stBits.u32Src1ArgbOrder = pDrvSurface->enRgbOrder;
            pHWNode->u32TDE_ARGB_ORDER = unArgbOrder.u32All;
            TDE_SET_UPDATE(u32TDE_ARGB_ORDER, pHWNode->u64TDE_UPDATE);
        }
#endif
    }

    /*target bitmapis same with source bitmap 1,so not need set*/
    /*CNcomment:源1位图宽高总是与Target位图一致,因此不设置源1的大小*/

    /*config the node*//*CNcomment:配置缓存节点*/
    pHWNode->u32TDE_S1_TYPE = unSrcType.u32All;
    pHWNode->u32TDE_S1_XY = unXy.u32All;

    /*set update area to 1*//*CNcomment:HWNode.TDE_UPDATE中有配置项对应位设1*/
    TDE_SET_UPDATE(u32TDE_S1_ADDR, pHWNode->u64TDE_UPDATE);
    TDE_SET_UPDATE(u32TDE_S1_TYPE, pHWNode->u64TDE_UPDATE);
    TDE_SET_UPDATE(u32TDE_S1_XY, pHWNode->u64TDE_UPDATE);

    return;
}

HI_VOID TdeHalNodeSetSrc1Ex(TDE_HWNode_S* pHWNode, TDE_DRV_SURFACE_S* pDrvSurface)
{
    TDE_SRC_TYPE_U unSrcType;
    TDE_SUR_XY_U unXy;
    
    TDE_ASSERT(HI_NULL != pHWNode);
    TDE_ASSERT(HI_NULL != pDrvSurface);

    /*set attribute for source bitmap *//*CNcomment:配置源位图属性信息*/
    unSrcType.u32All = pHWNode->u32TDE_S1_TYPE;
    unSrcType.stBits.u32Pitch = (HI_U32)pDrvSurface->u32Pitch;
    unSrcType.stBits.u32SrcColorFmt = (HI_U32)pDrvSurface->enColorFmt;
    unSrcType.stBits.u32AlphaRange = (HI_U32)pDrvSurface->bAlphaMax255;
    unSrcType.stBits.u32HScanOrd = (HI_U32)pDrvSurface->enHScan;
    unSrcType.stBits.u32VScanOrd = (HI_U32)pDrvSurface->enVScan;
    /*file zero of low area and top area use low area extend*//*CNcomment:一直使用低位填充为0,高位使用低位的扩展方式*/
    unSrcType.stBits.u32RgbExp = 0; 
    unXy.u32All = pHWNode->u32TDE_S1_XY;
    unXy.stBits.u32X = pDrvSurface->u32Xpos;
    unXy.stBits.u32Y = pDrvSurface->u32Ypos;

    /*target bitmapis same with source bitmap 1,so not need set*/
    /*CNcomment:源1位图宽高总是与Target位图一致,因此不设置源1的大小*/

     /*config the node*//*CNcomment:配置缓存节点*/
    pHWNode->u32TDE_S1_ADDR = pDrvSurface->u32PhyAddr;
    pHWNode->u32TDE_S1_TYPE = unSrcType.u32All;
    pHWNode->u32TDE_S1_XY = unXy.u32All;

   /*set update area to 1*//*CNcomment:HWNode.TDE_UPDATE中有配置项对应位设1*/
    TDE_SET_UPDATE(u32TDE_S1_ADDR, pHWNode->u64TDE_UPDATE);
    TDE_SET_UPDATE(u32TDE_S1_TYPE, pHWNode->u64TDE_UPDATE);
    TDE_SET_UPDATE(u32TDE_S1_XY, pHWNode->u64TDE_UPDATE);

    return;
}

/*****************************************************************************
* Function:      TdeHalNodeSetSrc2
* Description:   set the source bitmap 2
* Input:         pHWNode: pointer of node
*                pDrvSurface:  bitmap info
* Output:        none
* Return:        none
* Others:        none
*****************************************************************************/
HI_VOID TdeHalNodeSetSrc2(TDE_HWNode_S* pHWNode, TDE_DRV_SURFACE_S* pDrvSurface)
{
    TDE_ASSERT(HI_NULL != pHWNode);
    TDE_ASSERT(HI_NULL != pDrvSurface);

    TdeHalNodeSetSrc2Base(pHWNode, pDrvSurface, HI_TRUE);

    return;
}

/*****************************************************************************
* Function:      TdeHalNodeSetSrcMbY
* Description:   set the brightness info for source bitmap that is mb format 
* Input:         pHWNode: pointer of node
*                pDrvMbY: brightness info
*                enMbOpt: set info
* Output:       none
* Return:       none
* Others:        none
*****************************************************************************/
HI_VOID TdeHalNodeSetSrcMbY(TDE_HWNode_S* pHWNode, TDE_DRV_SURFACE_S* pDrvMbY, TDE_DRV_MB_OPT_MODE_E enMbOpt)
{
    TDE_ASSERT(HI_NULL != pHWNode);
    TDE_ASSERT(HI_NULL != pDrvMbY);

    if (TDE_MB_RASTER_OPT == enMbOpt)
    {
        /*CNcomment: 如果是宏块与光栅混合操作模式,亮度信息直接设置到Src2中 */
        TdeHalNodeSetSrc2BaseEx(pHWNode, pDrvMbY, HI_TRUE);

        return;
    }
    else
    {
        TdeHalNodeSetSrc1Ex(pHWNode, pDrvMbY);
    }

    if (TDE_MB_Y_FILTER == enMbOpt || TDE_MB_CONCA_FILTER == enMbOpt)
    {
        TDE_SUR_SIZE_U unSurSize;
        unSurSize.u32All = pHWNode->u32TDE_S2_SIZE;
        /*CNcomment:当为如上类型时,需要配置源2的大小,作为位图大小信息*/
        unSurSize.stBits.u32Width = (HI_U32)pDrvMbY->u32Width;
        unSurSize.stBits.u32Height = (HI_U32)pDrvMbY->u32Height;
        pHWNode->u32TDE_S2_SIZE = unSurSize.u32All;
        TDE_SET_UPDATE(u32TDE_S2_SIZE, pHWNode->u64TDE_UPDATE);
    }

    return;
}

/*****************************************************************************
* Function:      TdeHalNodeSetSrcMbCbCr
* Description:   set the chroma info for source bitmap that is mb format 
* Input:         pHWNode: pointer of node
*                pDrvMbCbCr: cbcr info
*                enMbOpt: set info
* Output:        none
* Return:        none
* Others:        none
*****************************************************************************/
HI_VOID TdeHalNodeSetSrcMbCbCr(TDE_HWNode_S* pHWNode, TDE_DRV_SURFACE_S* pDrvMbCbCr, TDE_DRV_MB_OPT_MODE_E enMbOpt)
{
    TDE_ASSERT(HI_NULL != pHWNode);
    TDE_ASSERT(HI_NULL != pDrvMbCbCr);
    
    if (TDE_MB_Y_FILTER == enMbOpt || TDE_MB_CONCA_FILTER == enMbOpt)
    {
        TdeHalNodeSetSrc2BaseEx(pHWNode, pDrvMbCbCr, HI_FALSE);
    }
    else
    {
        TdeHalNodeSetSrc2BaseEx(pHWNode, pDrvMbCbCr, HI_TRUE);
    }

    return;
}
/*****************************************************************************
* Function:      TdeHalNodeSetTgt
* Description:   set target bitmap info
* Input:         pHWNode: pointer of node
*                pDrvSurface: bitmap info
*                enAlphaFrom: alpha from
* Output:        none
* Return:        none
* Others:        none
*****************************************************************************/
HI_VOID TdeHalNodeSetTgt(TDE_HWNode_S* pHWNode, TDE_DRV_SURFACE_S* pDrvSurface, TDE_DRV_OUTALPHA_FROM_E enAlphaFrom)
{
    TDE_TAR_TYPE_U unTarType;
    TDE_SUR_SIZE_U unSurSize;
    TDE_SUR_XY_U unXy;
    TDE_ARGB_ORDER_U unArgbOrder;
    
    TDE_ASSERT(HI_NULL != pHWNode);

    /*set bitmap attribute info*//*CNcomment:配置源位图属性信息*/
    unTarType.u32All = pHWNode->u32TDE_TAR_TYPE;
    unTarType.stBits.u32Pitch = (HI_U32)pDrvSurface->u32Pitch;
    unTarType.stBits.u32TarColorFmt = (HI_U32)pDrvSurface->enColorFmt;
    unTarType.stBits.u32AlphaRange = (HI_U32)pDrvSurface->bAlphaMax255;
    unTarType.stBits.u32HScanOrd = (HI_U32)pDrvSurface->enHScan;
    unTarType.stBits.u32VScanOrd = (HI_U32)pDrvSurface->enVScan;
    unTarType.stBits.u32AlphaFrom = (HI_U32)enAlphaFrom;
    unTarType.stBits.u32RgbRound = 0; /*CNcomment:一直使用使用四舍无入的截位方式*/

    if (unTarType.stBits.u32TarColorFmt == TDE_DRV_COLOR_FMT_AYCbCr8888)
    {
        unTarType.stBits.u32TarColorFmt = TDE_DRV_COLOR_FMT_ARGB8888;
    }

    /*set bitmap size info*//*CNcomment:配置位图大小信息*/
    unSurSize.u32All = pHWNode->u32TDE_TS_SIZE;
    unSurSize.stBits.u32Width = (HI_U32)pDrvSurface->u32Width;
    unSurSize.stBits.u32Height = (HI_U32)pDrvSurface->u32Height;

    /*set alpha info*//*CNcomment:设置alpha判决阈值 */
#ifdef TDE_VERSION_MPW
    unSurSize.stBits.u32AlphaThresholdHigh = ((s_u8AlphaThresholdValue >>1) >> 4); //change the range 0-128
    unSurSize.stBits.u32AlphaThresholdLow = ((s_u8AlphaThresholdValue >>1) & 0x0f);
#else
    unSurSize.stBits.u32AlphaThresholdHigh = (s_u8AlphaThresholdValue >> 4); //change the range 0-255
    unSurSize.stBits.u32AlphaThresholdLow = (s_u8AlphaThresholdValue & 0x0f);
#endif
    unXy.u32All = pHWNode->u32TDE_TAR_XY;
    unXy.stBits.u32X = pDrvSurface->u32Xpos;
    unXy.stBits.u32Y = pDrvSurface->u32Ypos;

#if defined (TDE_VERSION_PILOT) || defined (TDE_VERSION_FPGA)
    if ((pDrvSurface->enColorFmt <= TDE_DRV_COLOR_FMT_ARGB8888) || (pDrvSurface->enColorFmt == TDE_DRV_COLOR_FMT_AYCbCr8888))
    {
        unArgbOrder.u32All = pHWNode->u32TDE_ARGB_ORDER;
        unArgbOrder.stBits.u32TarArgbOrder = pDrvSurface->enRgbOrder;

        if (pDrvSurface->enColorFmt == TDE_DRV_COLOR_FMT_AYCbCr8888)
        {
            unTarType.stBits.u32TarColorFmt = TDE_DRV_COLOR_FMT_ARGB8888;
    	    unArgbOrder.stBits.u32TarArgbOrder = 0x17;
        }
            
        pHWNode->u32TDE_ARGB_ORDER = unArgbOrder.u32All;

        TDE_SET_UPDATE(u32TDE_ARGB_ORDER, pHWNode->u64TDE_UPDATE);
    }
#endif	
    if(TDE_DRV_COLOR_FMT_RABG8888 == pDrvSurface->enColorFmt)
    {
        unArgbOrder.u32All = pHWNode->u32TDE_ARGB_ORDER;
        unArgbOrder.stBits.u32TarArgbOrder  = TDE_DRV_ORDER_RABG; //RABG
        pHWNode->u32TDE_ARGB_ORDER = unArgbOrder.u32All;
        TDE_SET_UPDATE(u32TDE_ARGB_ORDER, pHWNode->u64TDE_UPDATE);
        unTarType.stBits.u32TarColorFmt     = TDE_DRV_COLOR_FMT_ARGB8888;
    }

    /*set node info*//*CNcomment:配置缓存节点*/
    pHWNode->u32TDE_TAR_ADDR = pDrvSurface->u32PhyAddr;
    pHWNode->u32TDE_TAR_TYPE = unTarType.u32All;
    pHWNode->u32TDE_TAR_XY = unXy.u32All;
    pHWNode->u32TDE_TS_SIZE = unSurSize.u32All;

     /*set node update info to 1*//*CNcomment:HWNode.TDE_UPDATE中有配置项对应位设1*/
    TDE_SET_UPDATE(u32TDE_TAR_ADDR, pHWNode->u64TDE_UPDATE);
    TDE_SET_UPDATE(u32TDE_TAR_TYPE, pHWNode->u64TDE_UPDATE);
    TDE_SET_UPDATE(u32TDE_TAR_XY, pHWNode->u64TDE_UPDATE);
    TDE_SET_UPDATE(u32TDE_TS_SIZE, pHWNode->u64TDE_UPDATE);
    
    return;
}

/*****************************************************************************
* Function:      TdeHalNodeSetBaseOperate
* Description:   set base operation
* Input:         pHWNode: pointer of node
*                enMode: base operation mode
*                enAlu: ALU mode
*                pstColorFill:fill color value
* Output:        none
* Return:        none
* Others:        none
*****************************************************************************/
HI_VOID TdeHalNodeSetBaseOperate(TDE_HWNode_S* pHWNode, TDE_DRV_BASEOPT_MODE_E enMode,
                                 TDE_DRV_ALU_MODE_E enAlu, TDE_DRV_COLORFILL_S *pstColorFill)
{
    TDE_ALU_U unAluMode;
    TDE_INS_U unIns;
    
    TDE_ASSERT(HI_NULL != pHWNode);
    
    unAluMode.u32All = pHWNode->u32TDE_ALU;
    unIns.u32All = pHWNode->u32TDE_INS;
    unAluMode.stBits.u32AlphaThreshodEn = (s_bEnAlphaThreshold)?1:0;

    switch (enMode)
    {
    case TDE_QUIKE_FILL:/*quick file*//*CNcomment:快速填充*/
        {
            TDE_ASSERT(HI_NULL != pstColorFill);
            TDE_FILL_DATA_BY_FMT(pHWNode->u32TDE_S1_FILL, 
                pstColorFill->u32FillData, pstColorFill->enDrvColorFmt);
            TDE_SET_UPDATE(u32TDE_S1_FILL, pHWNode->u64TDE_UPDATE);

            unAluMode.stBits.u32AluMod = TDE_SRC1_BYPASS;
            unIns.stBits.u32Src1Mod = TDE_SRC_QUICK_FILL;
            unIns.stBits.u32Src2Mod = TDE_SRC_MODE_DISA;
        }
        break;
    case TDE_QUIKE_COPY:/*quick copy*//*CNcomment:快速拷贝*/
        {
            unAluMode.stBits.u32AluMod = TDE_SRC1_BYPASS;
            unIns.stBits.u32Src1Mod = TDE_SRC_QUICK_COPY;
            unIns.stBits.u32Src2Mod = TDE_SRC_MODE_DISA;
        }
        break;
    case TDE_NORM_FILL_1OPT:/*signal fill*//*CNcomment:普通单源填充*/
        {
            TDE_ASSERT(HI_NULL != pstColorFill);
            TDE_FILL_DATA_BY_FMT(pHWNode->u32TDE_S2_FILL, 
                pstColorFill->u32FillData, pstColorFill->enDrvColorFmt);
            
            TDE_SET_UPDATE(u32TDE_S2_FILL, pHWNode->u64TDE_UPDATE);
            
            unIns.stBits.u32Src1Mod = TDE_SRC_MODE_DISA;
            unIns.stBits.u32Src2Mod = TDE_SRC_MODE_FILL;
            if (TDE_ALU_NONE == enAlu)
            {
                unAluMode.stBits.u32AluMod = TDE_SRC2_BYPASS;
            }
            else
            {
                unAluMode.stBits.u32AluMod = (HI_U32)enAlu;
            }
        }
        break;
    case TDE_NORM_BLIT_1OPT:/*signal blit*//*CNcomment:普通单源操作搬移*/
        {
            unIns.stBits.u32Src1Mod = TDE_SRC_MODE_DISA;
            unIns.stBits.u32Src2Mod = TDE_SRC_MODE_BMP;
            if (TDE_ALU_NONE == enAlu)
            {
                unAluMode.stBits.u32AluMod = TDE_SRC2_BYPASS;
            }
            else
            {
                unAluMode.stBits.u32AluMod = (HI_U32)enAlu;
            }
        }
        break;
    case TDE_NORM_FILL_2OPT:/*signal color with bitmap operation and blit*//*CNcomment:单色和位图运算操作搬移*/
        {
            TDE_ASSERT(HI_NULL != pstColorFill);
            TDE_FILL_DATA_BY_FMT(pHWNode->u32TDE_S2_FILL, 
                pstColorFill->u32FillData, pstColorFill->enDrvColorFmt);
            TDE_SET_UPDATE(u32TDE_S2_FILL, pHWNode->u64TDE_UPDATE);
            
            unIns.stBits.u32Src1Mod = TDE_SRC_MODE_BMP;
            unIns.stBits.u32Src2Mod = TDE_SRC_MODE_FILL;
            if (TDE_ALU_NONE == enAlu)
            {
                unAluMode.stBits.u32AluMod = TDE_SRC2_BYPASS;
            }
            else
            {
                unAluMode.stBits.u32AluMod = (HI_U32)enAlu;
            }
        }
        break;
    case TDE_NORM_BLIT_2OPT:/*double blit*//*CNcomment:普通双源操作搬移 */
        {
            unIns.stBits.u32Src1Mod = TDE_SRC_MODE_BMP;
            unIns.stBits.u32Src2Mod = TDE_SRC_MODE_BMP;
            if(TDE_ALU_NONE == enAlu)
            {
                unAluMode.stBits.u32AluMod = TDE_SRC2_BYPASS;
            }
            else
            {
                unAluMode.stBits.u32AluMod = (HI_U32)enAlu;
            }
        }
        break;
    case TDE_MB_2OPT:/*mb combination operation*//*CNcomment:宏块合并操作*/
        {
            unIns.stBits.u32Src1Mod = TDE_SRC_MODE_BMP;
            unIns.stBits.u32Src2Mod = TDE_SRC_MODE_BMP;
            unIns.stBits.u32MbEn = 1;
            if (TDE_ALU_CONCA == enAlu)
            {
                unAluMode.stBits.u32AluMod = TDE_ALU_CONCA;
            }
            else if (TDE_ALU_NONE == enAlu)
            {
                unAluMode.stBits.u32AluMod = TDE_SRC2_BYPASS;

            }
            else
            {
                unAluMode.stBits.u32AluMod = enAlu;
            }
        }
        break;
    case TDE_MB_C_OPT:/*mb cbcr sampling operation*//*CNcomment:宏块色度上采样操作*/
        {
            unIns.stBits.u32Src1Mod = TDE_SRC_MODE_DISA;
            unIns.stBits.u32Src2Mod = TDE_SRC_MODE_BMP;
            unIns.stBits.u32MbEn = 1;
            unAluMode.stBits.u32AluMod = TDE_SRC2_BYPASS;
        }
        break;
    case TDE_MB_Y_OPT:/*mb ligthness resize*//*CNcomment:宏块亮度缩放*/
        {
            unIns.stBits.u32Src1Mod = TDE_SRC_MODE_BMP;
            unIns.stBits.u32Src2Mod = TDE_SRC_MODE_DISA;
            unIns.stBits.u32MbEn = 1;
            unAluMode.stBits.u32AluMod = TDE_SRC2_BYPASS;
        }
        break;
    case TDE_SINGLE_SRC_PATTERN_FILL_OPT:
    {
        unIns.stBits.u32Src1Mod = TDE_SRC_MODE_DISA;
        unIns.stBits.u32Src2Mod = TDE_SRC_MODE_PATTERN;
        if (TDE_ALU_NONE != enAlu)
        {
            unAluMode.stBits.u32AluMod = enAlu;
        }
        else
        {
            unAluMode.stBits.u32AluMod = TDE_SRC2_BYPASS;
        }
        break;
    }
    case TDE_DOUBLE_SRC_PATTERN_FILL_OPT:
        {
            unIns.stBits.u32Src1Mod = TDE_SRC_MODE_BMP;
            unIns.stBits.u32Src2Mod = TDE_SRC_MODE_PATTERN;
            if (TDE_ALU_NONE != enAlu)
            {
                unAluMode.stBits.u32AluMod = enAlu;
            }
            else
            {
                unAluMode.stBits.u32AluMod = TDE_SRC2_BYPASS;
            }
            break;
        }
    default:
        break;
    }

    /*set node*//*CNcomment:配置缓存节点*/
    pHWNode->u32TDE_ALU = unAluMode.u32All;
    pHWNode->u32TDE_INS = unIns.u32All;

    /*set node update area to 1*//*CNcomment:WNode.TDE_UPDATE中有配置项对应位设1*/
    TDE_SET_UPDATE(u32TDE_ALU, pHWNode->u64TDE_UPDATE);
    TDE_SET_UPDATE(u32TDE_INS, pHWNode->u64TDE_UPDATE);

    return;
}

/*****************************************************************************
* Function:      TdeHalNodeSetGlobalAlpha
* Description:   set alpha blend
* Input:         pHWNode: pointer of node
*                u8Alpha: Alpha value
* Output:        none
* Return:        none
* Others:        none
*****************************************************************************/
HI_VOID TdeHalNodeSetGlobalAlpha(TDE_HWNode_S* pHWNode, HI_U8 u8Alpha, HI_BOOL bEnable)
{
    TDE_ALU_U unAluMode;

    TDE_ASSERT(HI_NULL != pHWNode);

#if 0
    if (!bEnable)
    {
#ifdef TDE_VERSION_MPW
        u8Alpha = 0xff;
#else
        return;
#endif
    }
#endif    

    TDE_ALPHA_ADJUST(u8Alpha);
    
    /*set node*//*CNcomment:配置缓存节点*/
    unAluMode.u32All = pHWNode->u32TDE_ALU;
    unAluMode.stBits.u32GlobalAlpha = u8Alpha;
    pHWNode->u32TDE_ALU = unAluMode.u32All;

    /*set node update area to 1*//*CNcomment:HWNode.TDE_UPDATE中有配置项对应位设1*/
    TDE_SET_UPDATE(u32TDE_ALU, pHWNode->u64TDE_UPDATE);

    return;
}

/*****************************************************************************
* Function:      TdeHalNodeSetExpAlpha
* Description:   extend to alpha0 and alpha1 operation when extend alpha for RGB5551
* Input:         pHWNode:pointer of node
*                u8Alpha0: Alpha0 value
*                u8Alpha1: Alpha1 value
* Output:        none
* Return:        none
* Others:        none
*****************************************************************************/
HI_VOID TdeHalNodeSetExpAlpha(TDE_HWNode_S* pHWNode, TDE_DRV_SRC_E enSrc, HI_U8 u8Alpha0, HI_U8 u8Alpha1)
{
    TDE_COLOR_CONV_U unConv;
    
    TDE_ASSERT(HI_NULL != pHWNode);

    TDE_ALPHA_ADJUST(u8Alpha0);
    TDE_ALPHA_ADJUST(u8Alpha1);
    
    /*set alpha0 alpha1*//*CNcomment:配置alpha0, alpha1*/
    unConv.u32All = pHWNode->u32TDE_COLOR_CONV;
    unConv.stBits.u32Alpha0 = u8Alpha0;
    unConv.stBits.u32Alpha1 = u8Alpha1;
    pHWNode->u32TDE_COLOR_CONV = unConv.u32All;

    /*set node update area to 1*//*CNcomment:HWNode.TDE_UPDATE中有配置项对应位设1*/
    TDE_SET_UPDATE(u32TDE_COLOR_CONV, pHWNode->u64TDE_UPDATE);

    if(TDE_DRV_SRC_S1 & enSrc)
    {
        pHWNode->u32TDE_S1_TYPE |= 0x20000000;
        
        /*set node update area to 1*//*CNcomment:HWNode.TDE_UPDATE中有配置项对应位设1*/
        TDE_SET_UPDATE(u32TDE_S1_TYPE, pHWNode->u64TDE_UPDATE);
    }

    if(TDE_DRV_SRC_S2 & enSrc)
    {
        pHWNode->u32TDE_S2_TYPE |= 0x20000000;
        
        /*set node update area to 1*//*CNcomment:HWNode.TDE_UPDATE中有配置项对应位设1*/
        TDE_SET_UPDATE(u32TDE_S2_TYPE, pHWNode->u64TDE_UPDATE);
    }

    return;
}

/*****************************************************************************
* Function:      TdeHalNodeSetAlphaBorder
* Description:   set deal with the Alpha side to enable or disable
* Input:         pHWNode: pointer of node
*                bEnable: enable true or false
* Output:        none
* Return:        none
* Others:        none
*****************************************************************************/
HI_VOID TdeHalNodeSetAlphaBorder(TDE_HWNode_S* pHWNode, HI_BOOL bVEnable, HI_BOOL bHEnable)
{
    TDE_2D_RSZ_U unRsz;
    
    TDE_ASSERT(HI_NULL != pHWNode);
    unRsz.u32All = pHWNode->u32TDE_2D_RSZ;
    unRsz.stBits.u32AlpBorder = (((HI_U32)bVEnable << 1) | (HI_U32)bHEnable);
    pHWNode->u32TDE_2D_RSZ = unRsz.u32All;
    
    TDE_SET_UPDATE(u32TDE_2D_RSZ, pHWNode->u64TDE_UPDATE);

    return;
}

/*****************************************************************************
* Function:      TdeHalNodeSetRop
* Description:   set rop operation parameter 
* Input:         pHWNode: pointer of node
*                enRopCode: ROP code
* Output:        none
* Return:        none
* Others:        none
*****************************************************************************/
HI_VOID TdeHalNodeSetRop(TDE_HWNode_S* pHWNode, TDE2_ROP_CODE_E enRgbRop, TDE2_ROP_CODE_E enAlphaRop)
{
    TDE_ALU_U unAluMode;
    
    TDE_ASSERT(HI_NULL != pHWNode);
    /*set node*//*CNcomment:配置缓存节点*/
    unAluMode.u32All = pHWNode->u32TDE_ALU;
    unAluMode.stBits.u32RgbRopMod = (HI_U32)enRgbRop;
    unAluMode.stBits.u32AlphaRopMod = (HI_U32)enAlphaRop;
    pHWNode->u32TDE_ALU = unAluMode.u32All;

    /*set node update area to 1*//*CNcomment:HWNode.TDE_UPDATE中有配置项对应位设1*/
    TDE_SET_UPDATE(u32TDE_ALU, pHWNode->u64TDE_UPDATE);
}

/*****************************************************************************
* Function:      TdeHalNodeSetBlend
* Description:   set blend operation parameter 
* Input:         pHWNode: pointer of node
*                stBlendOpt:blend operation
* Output:        none
* Return:        none
* Others:        none
*****************************************************************************/
HI_VOID TdeHalNodeSetBlend(TDE_HWNode_S *pHWNode, TDE2_BLEND_OPT_S *pstBlendOpt)
{
#if defined(TDE_VERSION_PILOT) || defined(TDE_VERSION_FPGA)
    TDE_ALPHA_BLEND_U unAlphaBlend;

    TDE_ASSERT(HI_NULL != pHWNode);

    unAlphaBlend.u32All = pHWNode->u32TDE_ALPHA_BLEND;
    /*those paramter is fix ,app cann't view*//*CNcomment: 这四个参数驱动固定，对外不可见 */
    unAlphaBlend.stBits.u32Src1PixelAlphaEn = HI_TRUE;
    unAlphaBlend.stBits.u32Src1GlobalAlphaEn = HI_FALSE;
    unAlphaBlend.stBits.u32Src1MultiGlobalEn = HI_FALSE;
    unAlphaBlend.stBits.u32Src2MultiGlobalEn = HI_FALSE;

    unAlphaBlend.stBits.u32Src1PremultiEn = pstBlendOpt->bSrc1AlphaPremulti;
    unAlphaBlend.stBits.u32Src2PremultiEn = pstBlendOpt->bSrc2AlphaPremulti;
    unAlphaBlend.stBits.u32Src2PixelAlphaEn = pstBlendOpt->bPixelAlphaEnable;
    unAlphaBlend.stBits.u32Src2GlobalAlphaEn = pstBlendOpt->bGlobalAlphaEnable;

    /*set mode for src1 and src2*//*CNcomment:  配置Src1、Src2模式 */
    switch(pstBlendOpt->eBlendCmd)
    {
        /**< fs: sa      fd: 1.0-sa */
        case TDE2_BLENDCMD_NONE:
        {
            unAlphaBlend.stBits.u32Src1BlendMode = TDE2_BLEND_INVSRC2ALPHA;
            unAlphaBlend.stBits.u32Src2BlendMode = TDE2_BLEND_SRC2ALPHA;
            break;
        }
        /**< fs: 0.0     fd: 0.0 */
        case TDE2_BLENDCMD_CLEAR:
        {
            unAlphaBlend.stBits.u32Src1BlendMode = TDE2_BLEND_ZERO;
            unAlphaBlend.stBits.u32Src2BlendMode = TDE2_BLEND_ZERO;
            break;
        }
        /**< fs: 1.0     fd: 0.0 */
        case TDE2_BLENDCMD_SRC:
        {
            unAlphaBlend.stBits.u32Src1BlendMode = TDE2_BLEND_ZERO;
            unAlphaBlend.stBits.u32Src2BlendMode = TDE2_BLEND_ONE;
            break;
        }
        /**< fs: 1.0     fd: 1.0-sa */
        case TDE2_BLENDCMD_SRCOVER:
        {
            unAlphaBlend.stBits.u32Src1BlendMode = TDE2_BLEND_INVSRC2ALPHA;
            unAlphaBlend.stBits.u32Src2BlendMode = TDE2_BLEND_ONE;
            break;
        }
        /**< fs: 1.0-da  fd: 1.0 */
        case TDE2_BLENDCMD_DSTOVER:
        {
            unAlphaBlend.stBits.u32Src1BlendMode = TDE2_BLEND_ONE;
            unAlphaBlend.stBits.u32Src2BlendMode = TDE2_BLEND_INVSRC1ALPHA;
            break;
        }
        /**< fs: da      fd: 0.0 */
        case TDE2_BLENDCMD_SRCIN:
        {
            unAlphaBlend.stBits.u32Src1BlendMode = TDE2_BLEND_ZERO;
            unAlphaBlend.stBits.u32Src2BlendMode = TDE2_BLEND_SRC1ALPHA;
            break;
        }
        /**< fs: 0.0     fd: sa */
        case TDE2_BLENDCMD_DSTIN:
        {
            unAlphaBlend.stBits.u32Src1BlendMode = TDE2_BLEND_SRC2ALPHA;
            unAlphaBlend.stBits.u32Src2BlendMode = TDE2_BLEND_ZERO;
            break;
        }
        /**< fs: 1.0-da  fd: 0.0 */
        case TDE2_BLENDCMD_SRCOUT:
        {
            unAlphaBlend.stBits.u32Src1BlendMode = TDE2_BLEND_ZERO;
            unAlphaBlend.stBits.u32Src2BlendMode = TDE2_BLEND_INVSRC1ALPHA;
            break;
        }
        /**< fs: 0.0     fd: 1.0-sa */
        case TDE2_BLENDCMD_DSTOUT:
        {
            unAlphaBlend.stBits.u32Src1BlendMode = TDE2_BLEND_INVSRC2ALPHA;
            unAlphaBlend.stBits.u32Src2BlendMode = TDE2_BLEND_ZERO;
            break;
        }
        /**< fs: da      fd: 1.0-sa */
        case TDE2_BLENDCMD_SRCATOP:
        {
            unAlphaBlend.stBits.u32Src1BlendMode = TDE2_BLEND_INVSRC2ALPHA;
            unAlphaBlend.stBits.u32Src2BlendMode = TDE2_BLEND_SRC1ALPHA;
            break;
        }
        /**< fs: 1.0-da  fd: sa */
        case TDE2_BLENDCMD_DSTATOP:
        {
            unAlphaBlend.stBits.u32Src1BlendMode = TDE2_BLEND_SRC2ALPHA;
            unAlphaBlend.stBits.u32Src2BlendMode = TDE2_BLEND_INVSRC1ALPHA;
            break;
        }
        /**< fs: 1.0     fd: 1.0 */
        case TDE2_BLENDCMD_ADD:
        {
            unAlphaBlend.stBits.u32Src1BlendMode = TDE2_BLEND_ONE;
            unAlphaBlend.stBits.u32Src2BlendMode = TDE2_BLEND_ONE;
            break;
        }
        /**< fs: 1.0-da  fd: 1.0-sa */
        case TDE2_BLENDCMD_XOR:
        {
            unAlphaBlend.stBits.u32Src1BlendMode = TDE2_BLEND_INVSRC2ALPHA;
            unAlphaBlend.stBits.u32Src2BlendMode = TDE2_BLEND_INVSRC1ALPHA;
            break;
        }
        /**< fs: 0.0  fd: 1.0*/
        case TDE2_BLENDCMD_DST:
        {
            unAlphaBlend.stBits.u32Src1BlendMode = TDE2_BLEND_ONE;
            unAlphaBlend.stBits.u32Src2BlendMode = TDE2_BLEND_ZERO;
            break;
        }
        /*user parameter*//*CNcomment:  用户自己配置参数 */
        case TDE2_BLENDCMD_CONFIG:
        default:
        {
            unAlphaBlend.stBits.u32Src1BlendMode = pstBlendOpt->eSrc1BlendMode;
            unAlphaBlend.stBits.u32Src2BlendMode = pstBlendOpt->eSrc2BlendMode;
            break;
        }
    }
    pHWNode->u32TDE_ALPHA_BLEND = unAlphaBlend.u32All;

    TDE_SET_UPDATE(u32TDE_ALPHA_BLEND, pHWNode->u64TDE_UPDATE);
#endif

    return;
}

/*****************************************************************************
* Function:      TdeHalNodeSetColorize
* Description:  set blend operation parmeter
* Input:         pHWNode: point of node
*                u32Colorize:Co
* Output:        none
* Return:        none
* Others:        none
*****************************************************************************/
HI_VOID TdeHalNodeSetColorize(TDE_HWNode_S *pHWNode, HI_U32 u32Colorize)
{
#if defined(TDE_VERSION_PILOT) || defined(TDE_VERSION_FPGA)
    TDE_SRC_TYPE_U unSrcType;    
    TDE_ASSERT(pHWNode != HI_NULL);

    unSrcType.u32All = pHWNode->u32TDE_S2_TYPE;
    unSrcType.stBits.u32ColorizeEnable = HI_TRUE;
    pHWNode->u32TDE_S2_TYPE = unSrcType.u32All;
    
    pHWNode->u32TDE_COLORIZE = u32Colorize;

    TDE_SET_UPDATE(u32TDE_S2_TYPE, pHWNode->u64TDE_UPDATE);
    TDE_SET_UPDATE(u32TDE_COLORIZE, pHWNode->u64TDE_UPDATE);
#endif

    return;
}

HI_VOID TdeHalNodeEnableAlphaRop(TDE_HWNode_S *pHWNode)
{
#if defined(TDE_VERSION_PILOT) || defined(TDE_VERSION_FPGA)
    TDE_ALPHA_BLEND_U unAlphaBlend;

    unAlphaBlend.u32All = pHWNode->u32TDE_ALPHA_BLEND;

    unAlphaBlend.stBits.u32AlphaRopEn = HI_TRUE;

    pHWNode->u32TDE_ALPHA_BLEND = unAlphaBlend.u32All;

    TDE_SET_UPDATE(u32TDE_ALPHA_BLEND, pHWNode->u64TDE_UPDATE);
#endif

    return;
}

/*****************************************************************************
* Function:      TdeHalNodeSetColorExp
* Description:   set color extend or color revise parameter
* Input:         pHWNode: pointer of node
*                pClutCmd: Clut operation parameter
* Output:        none
* Return:        none
* Others:        none
*****************************************************************************/
HI_VOID TdeHalNodeSetClutOpt(TDE_HWNode_S* pHWNode, TDE_DRV_CLUT_CMD_S* pClutCmd, HI_BOOL bReload)
{
    TDE_INS_U unIns;
    TDE_COLOR_CONV_U unConv;
    
    TDE_ASSERT(HI_NULL != pHWNode);
    TDE_ASSERT(HI_NULL != pClutCmd);
    
    unIns.u32All = pHWNode->u32TDE_INS;
    unIns.stBits.u32Clut = 1; /*enable clut operation*//*CNcomment: 使能Clut操作*/

    unConv.u32All = pHWNode->u32TDE_COLOR_CONV;
    unConv.stBits.u32ClutMod = (HI_U32)pClutCmd->enClutMode;
    unConv.stBits.u32ClutReload = (HI_U32)bReload; /*reload clut to register*//*CNcomment: reload clut 到寄存器 */

     /*set node*//*CNcomment:配置缓存节点*/
    pHWNode->u32TDE_CLUT_ADDR = (HI_U32)pClutCmd->pu8PhyClutAddr;
    pHWNode->u32TDE_INS = unIns.u32All;
    pHWNode->u32TDE_COLOR_CONV = unConv.u32All;

    /*set node update area to 1*//*CNcomment:HWNode.TDE_UPDATE中有配置项对应位设1*/
    TDE_SET_UPDATE(u32TDE_CLUT_ADDR, pHWNode->u64TDE_UPDATE);
    TDE_SET_UPDATE(u32TDE_INS, pHWNode->u64TDE_UPDATE);
    TDE_SET_UPDATE(u32TDE_COLOR_CONV, pHWNode->u64TDE_UPDATE);
}

/*****************************************************************************
* Function:      TdeHalNodeSetColorKey
* Description:  set parameter for color key operation  according color format
* Input:         pHWNode:pointer of node
*                enFmt: color format
*                pColorKey: pointer of color key value
* Output:        none
* Return:        none
* Others:        none
*****************************************************************************/
HI_VOID TdeHalNodeSetColorKey(TDE_HWNode_S* pHWNode, TDE_COLORFMT_CATEGORY_E enFmtCat, 
                              TDE_DRV_COLORKEY_CMD_S* pColorKey)
{
    TDE_INS_U unIns;
    TDE_ALU_U unAluMode;

    TDE_ASSERT(HI_NULL != pHWNode);
    TDE_ASSERT(HI_NULL != pColorKey);
    
    unIns.u32All = pHWNode->u32TDE_INS;
    unIns.stBits.u32ColorKey = 1; /*enable color key operation*//*CNcomment:使能Color Key操作*/
    unAluMode.u32All = pHWNode->u32TDE_ALU;
    unAluMode.stBits.u32CkSel = (HI_U32)pColorKey->enColorKeyMode;

    if (TDE_COLORFMT_CATEGORY_ARGB == enFmtCat)
    {
        TDE_ALPHA_ADJUST(pColorKey->unColorKeyValue.struCkARGB.stAlpha.u8CompMin);
        TDE_ALPHA_ADJUST(pColorKey->unColorKeyValue.struCkARGB.stAlpha.u8CompMax);
        
        pHWNode->u32TDE_CK_MIN = pColorKey->unColorKeyValue.struCkARGB.stBlue.u8CompMin
            | (pColorKey->unColorKeyValue.struCkARGB.stGreen.u8CompMin << 8)
            | (pColorKey->unColorKeyValue.struCkARGB.stRed.u8CompMin << 16)
            | (pColorKey->unColorKeyValue.struCkARGB.stAlpha.u8CompMin << 24);
        pHWNode->u32TDE_CK_MAX = pColorKey->unColorKeyValue.struCkARGB.stBlue.u8CompMax
            | (pColorKey->unColorKeyValue.struCkARGB.stGreen.u8CompMax << 8)
            | (pColorKey->unColorKeyValue.struCkARGB.stRed.u8CompMax << 16)
            | (pColorKey->unColorKeyValue.struCkARGB.stAlpha.u8CompMax << 24);
#if defined(TDE_VERSION_PILOT) || defined(TDE_VERSION_FPGA)
        pHWNode->u32TDE_CK_MASK = pColorKey->unColorKeyValue.struCkARGB.stBlue.u8CompMask
            | (pColorKey->unColorKeyValue.struCkARGB.stGreen.u8CompMask << 8)
            | (pColorKey->unColorKeyValue.struCkARGB.stRed.u8CompMask << 16)
            | (pColorKey->unColorKeyValue.struCkARGB.stAlpha.u8CompMask << 24);
#endif

        if (pColorKey->unColorKeyValue.struCkARGB.stBlue.bCompIgnore)
        {
            unAluMode.stBits.u32CkBMod = TDE_COLORKEY_IGNORE;
        }
        else if (pColorKey->unColorKeyValue.struCkARGB.stBlue.bCompOut)
        {
            unAluMode.stBits.u32CkBMod = TDE_COLORKEY_AREA_OUT;
        }
        else
        {
            unAluMode.stBits.u32CkBMod = TDE_COLORKEY_AREA_IN;
        }

        if (pColorKey->unColorKeyValue.struCkARGB.stGreen.bCompIgnore)
        {
            unAluMode.stBits.u32CkGMod = TDE_COLORKEY_IGNORE;
        }
        else if (pColorKey->unColorKeyValue.struCkARGB.stGreen.bCompOut)
        {
            unAluMode.stBits.u32CkGMod = TDE_COLORKEY_AREA_OUT;
        }
        else
        {
            unAluMode.stBits.u32CkGMod = TDE_COLORKEY_AREA_IN;
        }

        if (pColorKey->unColorKeyValue.struCkARGB.stRed.bCompIgnore)
        {
            unAluMode.stBits.u32CkRMod = TDE_COLORKEY_IGNORE;
        }
        else if (pColorKey->unColorKeyValue.struCkARGB.stRed.bCompOut)
        {
            unAluMode.stBits.u32CkRMod = TDE_COLORKEY_AREA_OUT;
        }
        else
        {
            unAluMode.stBits.u32CkRMod = TDE_COLORKEY_AREA_IN;
        }

        if (pColorKey->unColorKeyValue.struCkARGB.stAlpha.bCompIgnore)
        {
            unAluMode.stBits.u32CkAMod = TDE_COLORKEY_IGNORE;
        }
        else if (pColorKey->unColorKeyValue.struCkARGB.stAlpha.bCompOut)
        {
            unAluMode.stBits.u32CkAMod = TDE_COLORKEY_AREA_OUT;
        }
        else
        {
            unAluMode.stBits.u32CkAMod = TDE_COLORKEY_AREA_IN;
        }
    }
    else if (TDE_COLORFMT_CATEGORY_CLUT == enFmtCat)/*clut format use index*//*CNcomment:CLUT格式只用索引*/
    {
        TDE_ALPHA_ADJUST(pColorKey->unColorKeyValue.struCkClut.stAlpha.u8CompMin);
        TDE_ALPHA_ADJUST(pColorKey->unColorKeyValue.struCkClut.stAlpha.u8CompMax);

        pHWNode->u32TDE_CK_MIN = pColorKey->unColorKeyValue.struCkClut.stClut.u8CompMin
            | (pColorKey->unColorKeyValue.struCkClut.stAlpha.u8CompMin << 24);
        pHWNode->u32TDE_CK_MAX = pColorKey->unColorKeyValue.struCkClut.stClut.u8CompMax
            | (pColorKey->unColorKeyValue.struCkClut.stAlpha.u8CompMax << 24);
#if defined(TDE_VERSION_PILOT) || defined(TDE_VERSION_FPGA)
        pHWNode->u32TDE_CK_MASK = pColorKey->unColorKeyValue.struCkClut.stClut.u8CompMask
            | (pColorKey->unColorKeyValue.struCkClut.stAlpha.u8CompMask << 24);
#endif
        
        if (pColorKey->unColorKeyValue.struCkClut.stClut.bCompIgnore)
        {
            unAluMode.stBits.u32CkBMod = TDE_COLORKEY_IGNORE;
        }
        else if (pColorKey->unColorKeyValue.struCkClut.stClut.bCompOut)
        {
            unAluMode.stBits.u32CkBMod = TDE_COLORKEY_AREA_OUT;
        }
        else
        {
            unAluMode.stBits.u32CkBMod = TDE_COLORKEY_AREA_IN;
        }

        if (pColorKey->unColorKeyValue.struCkClut.stAlpha.bCompIgnore)
        {
            unAluMode.stBits.u32CkAMod = TDE_COLORKEY_IGNORE;
        }
        else if (pColorKey->unColorKeyValue.struCkClut.stAlpha.bCompOut)
        {
            unAluMode.stBits.u32CkAMod = TDE_COLORKEY_AREA_OUT;
        }
        else
        {
            unAluMode.stBits.u32CkAMod = TDE_COLORKEY_AREA_IN;
        }
    }
    else if (TDE_COLORFMT_CATEGORY_YCbCr == enFmtCat)/*YCbCr format*//*CNcomment:YCbCr格式*/
    {
        TDE_ALPHA_ADJUST(pColorKey->unColorKeyValue.struCkYCbCr.stAlpha.u8CompMin);
        TDE_ALPHA_ADJUST(pColorKey->unColorKeyValue.struCkYCbCr.stAlpha.u8CompMax);
        
        pHWNode->u32TDE_CK_MIN = pColorKey->unColorKeyValue.struCkYCbCr.stCr.u8CompMin
            | (pColorKey->unColorKeyValue.struCkYCbCr.stCb.u8CompMin << 8)
            | (pColorKey->unColorKeyValue.struCkYCbCr.stY.u8CompMin << 16)
            | (pColorKey->unColorKeyValue.struCkYCbCr.stAlpha.u8CompMin << 24);
        pHWNode->u32TDE_CK_MAX = pColorKey->unColorKeyValue.struCkYCbCr.stCr.u8CompMax
            | (pColorKey->unColorKeyValue.struCkYCbCr.stCb.u8CompMax << 8)
            | (pColorKey->unColorKeyValue.struCkYCbCr.stY.u8CompMax << 16)
            | (pColorKey->unColorKeyValue.struCkYCbCr.stAlpha.u8CompMax << 24);
#if defined(TDE_VERSION_PILOT) || defined(TDE_VERSION_FPGA)
        pHWNode->u32TDE_CK_MASK = pColorKey->unColorKeyValue.struCkYCbCr.stCr.u8CompMask
            | (pColorKey->unColorKeyValue.struCkYCbCr.stCb.u8CompMask << 8)
            | (pColorKey->unColorKeyValue.struCkYCbCr.stY.u8CompMask << 16)
            | (pColorKey->unColorKeyValue.struCkYCbCr.stAlpha.u8CompMask << 24);
#endif

        if (pColorKey->unColorKeyValue.struCkYCbCr.stCr.bCompIgnore)
        {
            unAluMode.stBits.u32CkBMod = TDE_COLORKEY_IGNORE;
        }
        else if (pColorKey->unColorKeyValue.struCkYCbCr.stCr.bCompOut)
        {
            unAluMode.stBits.u32CkBMod = TDE_COLORKEY_AREA_OUT;
        }
        else
        {
            unAluMode.stBits.u32CkBMod = TDE_COLORKEY_AREA_IN;
        }

        if (pColorKey->unColorKeyValue.struCkYCbCr.stCb.bCompIgnore)
        {
            unAluMode.stBits.u32CkGMod = TDE_COLORKEY_IGNORE;
        }
        else if (pColorKey->unColorKeyValue.struCkYCbCr.stCb.bCompOut)
        {
            unAluMode.stBits.u32CkGMod = TDE_COLORKEY_AREA_OUT;
        }
        else
        {
            unAluMode.stBits.u32CkGMod = TDE_COLORKEY_AREA_IN;
        }

        if (pColorKey->unColorKeyValue.struCkYCbCr.stY.bCompIgnore)
        {
            unAluMode.stBits.u32CkRMod = TDE_COLORKEY_IGNORE;
        }
        else if (pColorKey->unColorKeyValue.struCkYCbCr.stY.bCompOut)
        {
            unAluMode.stBits.u32CkRMod = TDE_COLORKEY_AREA_OUT;
        }
        else
        {
            unAluMode.stBits.u32CkRMod = TDE_COLORKEY_AREA_IN;
        }

        if (pColorKey->unColorKeyValue.struCkYCbCr.stAlpha.bCompIgnore)
        {
            unAluMode.stBits.u32CkAMod = TDE_COLORKEY_IGNORE;
        }
        else if (pColorKey->unColorKeyValue.struCkYCbCr.stAlpha.bCompOut)
        {
            unAluMode.stBits.u32CkAMod = TDE_COLORKEY_AREA_OUT;
        }
        else
        {
            unAluMode.stBits.u32CkAMod = TDE_COLORKEY_AREA_IN;
        }
    }
    else
    {
        return;
    }
    pHWNode->u32TDE_ALU = unAluMode.u32All;
    pHWNode->u32TDE_INS = unIns.u32All;
    
    /*set node update area to 1*//*CNcomment:HWNode.TDE_UPDATE中有配置项对应位设1*/
    TDE_SET_UPDATE(u32TDE_ALU, pHWNode->u64TDE_UPDATE);
    TDE_SET_UPDATE(u32TDE_INS, pHWNode->u64TDE_UPDATE);
    TDE_SET_UPDATE(u32TDE_CK_MIN, pHWNode->u64TDE_UPDATE);
    TDE_SET_UPDATE(u32TDE_CK_MAX, pHWNode->u64TDE_UPDATE);
#if defined (TDE_VERSION_PILOT) || defined (TDE_VERSION_FPGA)
    TDE_SET_UPDATE(u32TDE_CK_MASK, pHWNode->u64TDE_UPDATE);
#endif

    return;
}
/*****************************************************************************
* Function:      TdeHalNodeSetClipping
* Description:  set clip operation parameter
* Input:         pHWNode: pointer of node
*                pClip: clip range
* Output:        none
* Return:        none
* Others:        none
*****************************************************************************/
HI_VOID TdeHalNodeSetClipping(TDE_HWNode_S* pHWNode, TDE_DRV_CLIP_CMD_S* pClip)
{
    TDE_INS_U unIns;
    TDE_SUR_SIZE_U unClipPos;
    
    TDE_ASSERT(HI_NULL != pHWNode);
    TDE_ASSERT(HI_NULL != pClip);
    
    unIns.u32All = pHWNode->u32TDE_INS;
    unIns.stBits.u32Clip = 1; /*enable clip operation*//*CNcomment:启用clip操作*/

    if(pClip->bInsideClip)
    {
        unClipPos.u32All = 0;
    }
    else
    {
        /*over clip*//*CNcomment: 区域外clip指示 */
        unClipPos.u32All = 0x80000000; 
    }
    unClipPos.stBits.u32Width = pClip->u16ClipStartX;
    unClipPos.stBits.u32Height = pClip->u16ClipStartY;
    pHWNode->u32TDE_CLIP_START = unClipPos.u32All;

    unClipPos.u32All = 0; /*reset for next set*//*CNcomment:  重新复位,准备下一设置 AI7D02798 */
    unClipPos.stBits.u32Width = pClip->u16ClipEndX;
    unClipPos.stBits.u32Height = pClip->u16ClipEndY;
    pHWNode->u32TDE_CLIP_STOP = unClipPos.u32All;

    pHWNode->u32TDE_INS = unIns.u32All;

    /*set node update area to 1*//*CNcomment:HWNode.TDE_UPDATE中有配置项对应位设1*/
    TDE_SET_UPDATE(u32TDE_CLIP_START, pHWNode->u64TDE_UPDATE);
    TDE_SET_UPDATE(u32TDE_CLIP_STOP, pHWNode->u64TDE_UPDATE);
    TDE_SET_UPDATE(u32TDE_INS, pHWNode->u64TDE_UPDATE);

    return;
}

/*****************************************************************************
* Function:      TdeHalNodeSetFlicker
* Description:  set flicker operation parameter
* Input:         pHWNode: pointer of node
*                pFlicker: flicker coefficient
* Output:        none
* Return:        none
* Others:        none
*****************************************************************************/
HI_VOID TdeHalNodeSetFlicker(TDE_HWNode_S* pHWNode, TDE_DRV_FLICKER_CMD_S* pFlicker)
{
    TDE_INS_U unIns;
    TDE_2D_RSZ_U unRsz;
    
    TDE_ASSERT(HI_NULL != pHWNode);
    TDE_ASSERT(HI_NULL != pFlicker);
    
    unIns.u32All = pHWNode->u32TDE_INS;
    unIns.stBits.u32DfeEn = 1; /*enbale Flicker*//*CNcomment:使能抗闪烁*/
    unRsz.u32All = pHWNode->u32TDE_2D_RSZ;
    if (pFlicker->enDeflickerMode == TDE2_DEFLICKER_MODE_RGB)
    {
        unRsz.stBits.u32DfeAlphaDisable = 1;
    }
    else
    {
        unRsz.stBits.u32DfeAlphaDisable = 0;
    }
    unRsz.stBits.u32DfeMod = (HI_U32)pFlicker->enDfeMode;

    /*set node*//*CNcomment:配置缓存节点*/
    switch(s_eDeflickerLevel)
    {
        case TDE_DEFLICKER_LOW:
        {
            unRsz.stBits.u32DfeMod = 0;
            pHWNode->u32TDE_DFE_COEF0 = 0x0f003307;
            break;
        }
        case TDE_DEFLICKER_MIDDLE:
        {
            unRsz.stBits.u32DfeMod = 0;
            pHWNode->u32TDE_DFE_COEF0 = 0x0f002010;
            break;
        }
        case TDE_DEFLICKER_HIGH:
        {
            unRsz.stBits.u32DfeMod = 0;
            pHWNode->u32TDE_DFE_COEF0 = 0x0f001a13;
            break;
        }
        default:   
        {
            pHWNode->u32TDE_DFE_COEF0 = (HI_U32)pFlicker->u8Coef0NextLine 
                | ((HI_U32)pFlicker->u8Coef0CurLine << 8)
                | ((HI_U32)pFlicker->u8Coef0LastLine << 16)
                | ((HI_U32)pFlicker->u8Threshold0 << 24);
            pHWNode->u32TDE_DFE_COEF1 = (HI_U32)pFlicker->u8Coef1NextLine 
                | ((HI_U32)pFlicker->u8Coef1CurLine << 8)
                | ((HI_U32)pFlicker->u8Coef1LastLine << 16)
                | ((HI_U32)pFlicker->u8Threshold1 << 24);
            pHWNode->u32TDE_DFE_COEF2 = (HI_U32)pFlicker->u8Coef2NextLine 
                | ((HI_U32)pFlicker->u8Coef2CurLine << 8)
                | ((HI_U32)pFlicker->u8Coef2LastLine << 16)
                | ((HI_U32)pFlicker->u8Threshold2 << 24);
            pHWNode->u32TDE_DFE_COEF3 = (HI_U32)pFlicker->u8Coef3NextLine 
                | ((HI_U32)pFlicker->u8Coef3CurLine << 8)
                | ((HI_U32)pFlicker->u8Coef3LastLine << 16);
                    break;
        }
    }

    pHWNode->u32TDE_INS = unIns.u32All;
    pHWNode->u32TDE_2D_RSZ = unRsz.u32All;
    
    /*set node update area to 1*//*CNcomment:HWNode.TDE_UPDATE中有配置项对应位设1*/
    TDE_SET_UPDATE(u32TDE_INS, pHWNode->u64TDE_UPDATE);
    TDE_SET_UPDATE(u32TDE_2D_RSZ, pHWNode->u64TDE_UPDATE);
    TDE_SET_UPDATE(u32TDE_DFE_COEF0, pHWNode->u64TDE_UPDATE);
    TDE_SET_UPDATE(u32TDE_DFE_COEF1, pHWNode->u64TDE_UPDATE);
    TDE_SET_UPDATE(u32TDE_DFE_COEF2, pHWNode->u64TDE_UPDATE);
    TDE_SET_UPDATE(u32TDE_DFE_COEF3, pHWNode->u64TDE_UPDATE);

    return;
}
/*****************************************************************************
* Function:      TdeHalNodeSetResize
* Description:   set resize operation parameter
* Input:         pHWNode: pointer of node
*                pResize: resize coefficient
* Output:        none
* Return:        none
* Others:        none
*****************************************************************************/
HI_VOID TdeHalNodeSetResize(TDE_HWNode_S* pHWNode, TDE_DRV_RESIZE_CMD_S* pResize, 
TDE_NODE_SUBM_TYPE_E enNodeType)
{
    TDE_INS_U unIns;
    TDE_2D_RSZ_U unRsz;
    HI_U32 u32Sign = 0;
#ifdef TDE_VERSION_MPW
    HI_U16 u16VStep;
    HI_U16 u16HStep;
#endif
    TDE_TAR_TYPE_U unTarType;
    
    TDE_ASSERT(HI_NULL != pHWNode);
    TDE_ASSERT(HI_NULL != pResize);

    unIns.u32All = pHWNode->u32TDE_INS;
    
    unRsz.u32All = pHWNode->u32TDE_2D_RSZ;
    unRsz.stBits.u32HfMod = pResize->enFilterH;
    unRsz.stBits.u32VfMod = pResize->enFilterV;

    /*plumb Flicker*//*CNcomment: 垂直滤波 */
    if (TDE_DRV_FILTER_NONE != pResize->enFilterV)
    {
        unRsz.stBits.u32VfRingEn = pResize->bVfRing;  /*enable middle Flicker*//*CNcomment: 使能中值滤波*/
        unRsz.stBits.u32VfCoefReload = 0x1; /*load parameter*//*CNcomment:载入垂直参数*/
        /*set v resize parameter of start address*//*CNcomment:根据步长配置垂直缩放参数表首地址 */
        pHWNode->u32TDE_VF_COEF_ADDR = s_stParaTable.u32VfCoefAddr
                      + TdeHalGetResizeParaVTable(pResize->u32StepV) * TDE_PARA_VTABLE_SIZE;
        TDE_SET_UPDATE(u32TDE_VF_COEF_ADDR, pHWNode->u64TDE_UPDATE);

        if (TDE_NODE_SUBM_PARENT == enNodeType)
        {
       /*set plumb offset*//*CNcomment:  设置垂直偏移 */
        u32Sign = pResize->u32OffsetY >> 31; /*get symbol*//*CNcomment: 提取符号*/
        pHWNode->u32TDE_RSZ_Y_OFST = (u32Sign << 16) | (pResize->u32OffsetY & 0xffff);
        TDE_SET_UPDATE(u32TDE_RSZ_Y_OFST, pHWNode->u64TDE_UPDATE);
        unIns.stBits.u32Resize = 0x1; /*enable resize*//*CNcomment: 使能缩放*/
        }
    }
    /*h Flicker*//*CNcomment: 水平滤波 */
    if (TDE_DRV_FILTER_NONE != pResize->enFilterH)
    {
        unRsz.stBits.u32HfRingEn = pResize->bHfRing; /*enable middle Flicker*//*CNcomment: 使能中值滤波*/
        unRsz.stBits.u32HfCoefReload = 0x1;/*load parameter*//*CNcomment:载入垂直参数*/
        /*set h resize parameter of start address*//*CNcomment:根据步长配置垂直缩放参数表首地址 */
        pHWNode->u32TDE_HF_COEF_ADDR = s_stParaTable.u32HfCoefAddr
                    + TdeHalGetResizeParaHTable(pResize->u32StepH) * TDE_PARA_HTABLE_SIZE;
        TDE_SET_UPDATE(u32TDE_HF_COEF_ADDR, pHWNode->u64TDE_UPDATE);

        if (TDE_NODE_SUBM_PARENT == enNodeType)
        {
        /*set h offset*//*CNcomment:  设置水平偏移 */
        u32Sign = pResize->u32OffsetX >> 31; /*get symbol*//*CNcomment: 提取符号*/
        pHWNode->u32TDE_RSZ_X_OFST = (u32Sign << 16) | (pResize->u32OffsetX & 0xffff);
        TDE_SET_UPDATE(u32TDE_RSZ_X_OFST, pHWNode->u64TDE_UPDATE);
        unIns.stBits.u32Resize = 0x1; /*enable resize*//*CNcomment:使能缩放*/
        }
    }

    unRsz.stBits.u32CoefSym = pResize->bCoefSym;/*Flicker parameter*//*CNcomment:对称滤波系数*/
    pHWNode->u32TDE_2D_RSZ = unRsz.u32All;
    if (TDE_NODE_SUBM_PARENT == enNodeType)
    {
        pHWNode->u32TDE_INS = unIns.u32All;

        unTarType.u32All = pHWNode->u32TDE_TAR_TYPE;
        unTarType.stBits.u32DfeFirstlineOutEn = pResize->bFirstLineOut;
        unTarType.stBits.u32DfeLastlineOutEn = pResize->bLastLineOut;
        pHWNode->u32TDE_TAR_TYPE = unTarType.u32All;

        /*set flicker step*//*CNcomment:   设置滤波步长 */
#ifdef TDE_VERSION_MPW
        u16HStep = pResize->u32StepH & 0xffff;
        u16VStep = pResize->u32StepV & 0xffff;
        pHWNode->u32TDE_RSZ_STEP = (HI_U32)u16HStep | ((HI_U32)u16VStep << 16);
        TDE_SET_UPDATE(u32TDE_RSZ_STEP, pHWNode->u64TDE_UPDATE);
#else
    pHWNode->u32TDE_RSZ_HSTEP = pResize->u32StepH;
    pHWNode->u32TDE_RSZ_VSTEP = pResize->u32StepV;
    TDE_SET_UPDATE(u32TDE_RSZ_HSTEP, pHWNode->u64TDE_UPDATE);
    TDE_SET_UPDATE(u32TDE_RSZ_VSTEP, pHWNode->u64TDE_UPDATE);
#endif
    
        TDE_SET_UPDATE(u32TDE_INS, pHWNode->u64TDE_UPDATE);
        TDE_SET_UPDATE(u32TDE_TAR_TYPE , pHWNode->u64TDE_UPDATE);
    }

    TDE_SET_UPDATE(u32TDE_2D_RSZ, pHWNode->u64TDE_UPDATE);

    return;
}

/*****************************************************************************
* Function:      TdeHalNodeSetColorConvert
* Description:  set parameter for color space change
* Input:         pHWNode:pointer of node
*                pConv: parameter of color space change
* Output:        none
* Return:        none
* Others:        none
*****************************************************************************/
HI_VOID TdeHalNodeSetColorConvert(TDE_HWNode_S* pHWNode, TDE_DRV_CONV_MODE_CMD_S* pConv)
{
    TDE_INS_U unIns;
    TDE_COLOR_CONV_U unConv;
    
    TDE_ASSERT(HI_NULL != pHWNode);
    TDE_ASSERT(HI_NULL != pConv);

    unIns.u32All = pHWNode->u32TDE_INS;
    unConv.u32All = pHWNode->u32TDE_COLOR_CONV;

    unIns.stBits.u32IcsConv = (HI_U32)pConv->bInConv;
    unIns.stBits.u32OcsConv = (HI_U32)pConv->bOutConv;
    unIns.stBits.u32IcscSel = (HI_U32)pConv->bInSrc1Conv;
    
    unConv.stBits.u32InRgb2Yc = (HI_U32)pConv->bInRGB2YC;
    unConv.stBits.u32InColorSpace = (HI_U32)pConv->bInMetrixVid;
    unConv.stBits.u32InColorImetry = (HI_U32)pConv->bInMetrix709;
    unConv.stBits.u32OutColorSpace = (HI_U32)pConv->bOutMetrixVid;
    unConv.stBits.u32OutColorImetry = (HI_U32)pConv->bOutMetrix709;

    /*set node*//*CNcomment:配置缓存节点*/
    pHWNode->u32TDE_COLOR_CONV = unConv.u32All;
    pHWNode->u32TDE_INS = unIns.u32All;

    /*set node update area to 1*//*CNcomment:HWNode.TDE_UPDATE中有配置项对应位设1*/
    TDE_SET_UPDATE(u32TDE_COLOR_CONV, pHWNode->u64TDE_UPDATE);
    TDE_SET_UPDATE(u32TDE_INS, pHWNode->u64TDE_UPDATE);

    return;
}

/*****************************************************************************
* Function:      TdeHalNodeAddChild
* Description:   add child node on Flicker
* Input:         pHWNode: pointer of node
*                pChildInfo: child info
* Output:        none
* Return:        none
* Others:        none
*****************************************************************************/
HI_VOID TdeHalNodeAddChild(TDE_HWNode_S* pHWNode, TDE_CHILD_INFO* pChildInfo)
{
    TDE_SUR_XY_U unXy;
    TDE_INS_U unIns;
    
    TDE_ASSERT(HI_NULL != pHWNode);
    TDE_ASSERT(HI_NULL != pChildInfo);

    /* config register according update info
       u32TDE_RSZ_OFST
       u32TDE_2D_RSZ
       TDE_S2_XY
       TDE_TAR_XY
       TDE_S2_SIZE
       TDE_TS_SIZE
       */

    /*CNcomment: 根据Update信息配置寄存器:
       u32TDE_RSZ_OFST
       u32TDE_2D_RSZ
       TDE_S2_XY
       TDE_TAR_XY
       TDE_S2_SIZE
       TDE_TS_SIZE
       */
    if (pChildInfo->u64Update & 0x1)
    {
        TdeHalSetXyByAdjInfo(pHWNode, pChildInfo);
    }

    if (pChildInfo->u64Update & 0x2)
    {
        TDE_SUR_SIZE_U unSz;
        unSz.u32All = pHWNode->u32TDE_S2_SIZE;
        unSz.stBits.u32Width = pChildInfo->u32Wi;
        unSz.stBits.u32Height = pChildInfo->u32Hi;
        pHWNode->u32TDE_S2_SIZE = unSz.u32All;
        TDE_SET_UPDATE(u32TDE_S2_SIZE, pHWNode->u64TDE_UPDATE);
    }

    if (pChildInfo->u64Update & 0x4)
    {
        HI_U32 u32Sign;
        
        /*set h offset*//* CNcomment: 设置水平偏移 */
        u32Sign = pChildInfo->u32HOfst >> 31;/*get symbol*/ /*CNcomment:提取符号*/
        pHWNode->u32TDE_RSZ_X_OFST = (u32Sign << 16) | (pChildInfo->u32HOfst & 0xffff);

        TDE_SET_UPDATE(u32TDE_RSZ_X_OFST, pHWNode->u64TDE_UPDATE);

        /*set v offset*//* CNcomment: 设置垂直偏移 */
        u32Sign = pChildInfo->u32VOfst >> 31; /*get symbol*/ /*CNcomment:提取符号*/
        pHWNode->u32TDE_RSZ_Y_OFST = (u32Sign << 16) | (pChildInfo->u32VOfst & 0xffff);

        TDE_SET_UPDATE(u32TDE_RSZ_Y_OFST, pHWNode->u64TDE_UPDATE);
    }

    if (pChildInfo->u64Update & 0x8)
    {
        unXy.u32All = pHWNode->u32TDE_TAR_XY;
        unXy.stBits.u32X = pChildInfo->u32Xo;
        unXy.stBits.u32Y = pChildInfo->u32Yo;
        pHWNode->u32TDE_TAR_XY = unXy.u32All;
        TDE_SET_UPDATE(u32TDE_TAR_XY, pHWNode->u64TDE_UPDATE);
        if(pChildInfo->stDSAdjInfo.bDoubleSource)
        {
            unXy.u32All = pHWNode->u32TDE_S1_XY;
            unXy.stBits.u32X = (HI_U32)(pChildInfo->u32Xo + pChildInfo->stDSAdjInfo.s32DiffX);
            unXy.stBits.u32Y = (HI_U32)(pChildInfo->u32Yo + pChildInfo->stDSAdjInfo.s32DiffY);
            pHWNode->u32TDE_S1_XY = unXy.u32All;
            TDE_SET_UPDATE(u32TDE_S1_XY, pHWNode->u64TDE_UPDATE);
        }
    }

    if (pChildInfo->u64Update & 0x10)
    {
        TDE_SUR_SIZE_U unSz;
        unSz.u32All = pHWNode->u32TDE_TS_SIZE;
        unSz.stBits.u32Width = pChildInfo->u32Wo;
        unSz.stBits.u32Height = pChildInfo->u32Ho;
        pHWNode->u32TDE_TS_SIZE = unSz.u32All;
        TDE_SET_UPDATE(u32TDE_TS_SIZE, pHWNode->u64TDE_UPDATE);
    }
    /*fill block info*/ /*CNcomment: 填充块类型信息 */
    unIns.u32All = pHWNode->u32TDE_INS;
    unIns.stBits.u32BlockFlag = (HI_U32)pChildInfo->enSliceType;
    if (pChildInfo->enSliceType == TDE_FIRST_BLOCK_SLICE_TYPE)
    {
        unIns.stBits.u32FirstBlockFlag = 1;
        unIns.stBits.u32LastBlockFlag = 0;
    }
    else if (pChildInfo->enSliceType == TDE_LAST_BLOCK_SLICE_TYPE)
    {
        unIns.stBits.u32FirstBlockFlag = 0;
        unIns.stBits.u32LastBlockFlag = 1;
    }
    else
    {
        unIns.stBits.u32FirstBlockFlag = 0;
        unIns.stBits.u32LastBlockFlag = 0;
    }
    pHWNode->u32TDE_INS = unIns.u32All;

    TDE_SET_UPDATE(u32TDE_INS, pHWNode->u64TDE_UPDATE);

    return;
}

/*****************************************************************************
* Function:      TdeHalNodeSetMbMode
* Description:   set parameter for deal with mb 
* Input:         pHWNode: pointer of node
*                pMbCmd: parameter
* Output:        none
* Return:        none
* Others:        none
*****************************************************************************/
HI_VOID TdeHalNodeSetMbMode(TDE_HWNode_S* pHWNode, TDE_DRV_MB_CMD_S* pMbCmd)
{
    TDE_INS_U unIns;
    
    TDE_ASSERT(HI_NULL != pHWNode);
    TDE_ASSERT(HI_NULL != pMbCmd);

    unIns.u32All = pHWNode->u32TDE_INS;
    unIns.stBits.u32MbMode = (HI_U32)pMbCmd->enMbMode;
    pHWNode->u32TDE_INS = unIns.u32All;

    /*set node update area to 1*//*CNcomment:HWNode.TDE_UPDATE中有配置项对应位设1*/
    TDE_SET_UPDATE(u32TDE_INS, pHWNode->u64TDE_UPDATE);

    return;
}

HI_VOID TDeHalNodeSetCsc(TDE_HWNode_S* pHWNode, TDE2_CSC_OPT_S stCscOpt)
{
#if defined (TDE_VERSION_PILOT) || defined (TDE_VERSION_FPGA)

	TDE_COLOR_CONV_U unColorConv;
        TDE_INS_U unIns;

	TDE_ASSERT(HI_NULL != pHWNode);

	unColorConv.u32All = pHWNode->u32TDE_COLOR_CONV;
        unIns.u32All = pHWNode->u32TDE_INS;

	unColorConv.stBits.u32IcscCustomEn = stCscOpt.bICSCUserEnable;
	unColorConv.stBits.u32IcscReload = stCscOpt.bICSCParamReload;
	unColorConv.stBits.u32OcscCustomEn = stCscOpt.bOCSCUserEnable;
	unColorConv.stBits.u32OcscReload = stCscOpt.bOCSCParamReload;

	pHWNode->u32TDE_ICSC_ADDR = stCscOpt.u32ICSCParamAddr;
	pHWNode->u32TDE_OCSC_ADDR = stCscOpt.u32OCSCParamAddr;

    	/*use correct parameter when user not use user parameter and enable ICSC function*/
        /*CNcomment:若用户未开启自定义系数且ICSC功能使能,
        则强制使用TDE的修正系数(TDE默认系数存在问题)*/
        if (!stCscOpt.bICSCUserEnable && unIns.stBits.u32IcsConv)
        {
            unColorConv.stBits.u32IcscCustomEn = 1;
	    unColorConv.stBits.u32IcscReload = 1;
	    if (unColorConv.stBits.u32InRgb2Yc)
            {
                pHWNode->u32TDE_ICSC_ADDR = s_u32Rgb2YuvCoefAddr;
            }
	    else
            {
                pHWNode->u32TDE_ICSC_ADDR = s_u32Yuv2RgbCoefAddr;
            }
        }

        if (!stCscOpt.bOCSCUserEnable && unIns.stBits.u32OcsConv)
        {
            unColorConv.stBits.u32OcscCustomEn = 1;
	    unColorConv.stBits.u32OcscReload = 1;
	    if (unColorConv.stBits.u32InRgb2Yc)
            {
                pHWNode->u32TDE_OCSC_ADDR = s_u32Yuv2RgbCoefAddr;
            }
	    else
            {
                pHWNode->u32TDE_OCSC_ADDR = s_u32Rgb2YuvCoefAddr;
            }
        }

        pHWNode->u32TDE_COLOR_CONV = unColorConv.u32All;

	TDE_SET_UPDATE(u32TDE_COLOR_CONV, pHWNode->u64TDE_UPDATE);
	TDE_SET_UPDATE(u32TDE_ICSC_ADDR, pHWNode->u64TDE_UPDATE);
	TDE_SET_UPDATE(u32TDE_OCSC_ADDR, pHWNode->u64TDE_UPDATE);
#endif

	return;
}


#if 0
STATIC HI_VOID TdeHalRestoreAq(HI_VOID)
{
    if (TDE_SUSP_LINE_READY == s_stSuspStat.s32AqSuspLine /* && 0 == s_stSuspStat.u32CurUpdate */
        && HI_NULL != s_stSuspStat.pstSwBuf )
    {
        if (TdeHalEnsureStartAq())
        {
            s_stSuspStat.pstSwBuf = HI_NULL;
        }
        return;
    }

    if (s_stQueueInfo.bAqReady)
    {
        if (TdeHalEnsureStartAq())
        {
            s_stQueueInfo.bAqReady = HI_FALSE;
        }
        return;
    }

    if (TDE_SUSP_LINE_INVAL == s_stSuspStat.s32AqSuspLine /* && 0 == s_stSuspStat.u32CurUpdate */
        && HI_NULL == s_stSuspStat.pstSwBuf)
    {
        return;
    }
    
    if (-1 == s_stSuspStat.s32AqSuspLine)
    {
        if (HI_SUCCESS != TdeHalRestoreChildNode())
        {
            return;
        }
    }
    else
    {
        if (HI_SUCCESS != TdeHalRestoreAloneNode())
        {
            return;
        }
    }
    if (!TdeHalCtlIsIdle())
    {
        s_stSuspStat.s32AqSuspLine = TDE_SUSP_LINE_READY;
        //s_stSuspStat.u32CurUpdate = 0;
        return;
    }
    TDE_WRITE_REG(s_pu32BaseVirAddr, TDE_CTRL, 1);

    s_stSuspStat.pstSwBuf = HI_NULL;
    s_stSuspStat.s32AqSuspLine = TDE_SUSP_LINE_INVAL;
    //s_stSuspStat.u32CurUpdate = 0;
    return;
    
}

#endif

/*****************************************************************************
* Function:      TdeHalNodeSuspend
* Description:   save current running node when execute suspend operation so that can resume
* Input:         none
* Output:       none
* Return:        none
* Others:        none
*****************************************************************************/
HI_VOID TdeHalNodeSuspend(HI_VOID)
{
    HI_U32 u32SuspNodePhyAddr = 0;
    //HI_U32* pu32SwNodeVirAddr = HI_NULL;
    //HI_U32* pu32AqSuspVirAddr = HI_NULL;
    //HI_BOOL bSaveLine = HI_TRUE;
    //TDE_BUF_NODE_S* pstBufNode;

    /*read current node physics address and save physics address and virtual address*/
    /*CNcomment:读取当前节点物理地址,并保存其对应的物理/虚拟地址*/
    u32SuspNodePhyAddr = TdeHalCurNode(TDE_LIST_AQ);
    if (0 == u32SuspNodePhyAddr)
    {
        s_stSuspStat.pSwNode = HI_NULL;
        s_stSuspStat.s32AqSuspLine = TDE_SUSP_LINE_INVAL;
        return;
    }

    s_stSuspStat.pSwNode = (HI_VOID *)wgetvrt(u32SuspNodePhyAddr - 4);

    #if 0
    pu32AqSuspVirAddr = (HI_U32 *)wgetvrt(u32SuspNodePhyAddr);
    if (HI_NULL == pu32AqSuspVirAddr)
    {
        s_stSuspStat.pstSwBuf = HI_NULL;
        s_stSuspStat.s32AqSuspLine = TDE_SUSP_LINE_INVAL;
        return;
    }
    pu32SwNodeVirAddr = TDE_GET_TAIL_INFO_IN_BUFF(pu32AqSuspVirAddr, u32SwBufAddr);
    s_stSuspStat.pstSwBuf = (TDE_BUF_NODE_S*)(*pu32SwNodeVirAddr);
    
    if (HI_NULL != s_stSuspStat.pstSwBuf->pstParentAddr)
    {
        bSaveLine = HI_FALSE;
    }

    if (bSaveLine)
    {
        s_stSuspStat.s32AqSuspLine = (HI_S32)TdeHalCurLine();
    }
    else
    {
        s_stSuspStat.s32AqSuspLine = -1;
    }
    #endif

    /*save current line infor*//*CNcomment:保存当前行信息*/
    s_stSuspStat.s32AqSuspLine = (HI_S32)TdeHalCurLine();
}


HI_VOID TdeHalGetSuspendNode(HI_VOID **ppstSuspendNode)
{
    *ppstSuspendNode = s_stSuspStat.pSwNode;
    return;
}


/****************************************************************************/
/*                             TDE hal function inner                         */
/****************************************************************************/

/*****************************************************************************
* Function:      TdeHalInitParaTable
* Description:  init config parameter
* Input:         none
* Output:        none
* Return:        success/fail
* Others:        none
*****************************************************************************/
STATIC HI_S32 TdeHalInitParaTable(HI_VOID)
{
    #define OrgHfCoef Tde8x32Coef
    HI_U32 i = 0;
    HI_U32 j = 0;
    TDE_FILTER_PARA_U unPara;
    HI_U32 *pHfCoef = HI_NULL;
    HI_U32 *pVfCoef = HI_NULL;
    HI_U32 *pRgb2YuvCsc = HI_NULL;
    HI_U32 *pYuv2RgbCsc = HI_NULL;
    
    pHfCoef = (HI_U32 *)TDE_MALLOC(TDE_PARA_HTABLE_SIZE * TDE_PARA_HTABLE_NUM);
    if (HI_NULL == pHfCoef)
    {
        TDE_TRACE(TDE_KERN_INFO, "Alloc horizontal coef failed!\n");
        return HI_FAILURE;
    }
    
    pVfCoef = (HI_U32 *)TDE_MALLOC(TDE_PARA_VTABLE_SIZE * TDE_PARA_VTABLE_NUM);
    if (HI_NULL == pVfCoef)
    {
        TDE_TRACE(TDE_KERN_INFO, "Alloc vertical coef failed\n");
        TDE_FREE(pHfCoef);
        return HI_FAILURE;
    }

    memset(pHfCoef, 0, (TDE_PARA_HTABLE_SIZE * TDE_PARA_HTABLE_NUM));
    memset(pVfCoef, 0, (TDE_PARA_VTABLE_SIZE * TDE_PARA_VTABLE_NUM));
    /*copy parameter according other offer way*//* CNcomment:按照算法组提供的结构拷贝参数表 */
    for (i = 0; i < TDE_PARA_HTABLE_NUM; i++)
    {
        memcpy(pHfCoef + i*(TDE_PARA_HTABLE_SIZE/4), OrgHfCoef + i*(TDE_PARA_HTABLE_ORG_SIZE/4), (TDE_PARA_HTABLE_ORG_SIZE));
    }
    for (i = 0, j = 0; i < TDE_PARA_VTABLE_ORG_SIZE * TDE_PARA_VTABLE_NUM - 4; j++)
    {
        unPara.stBits.u32Para0 = OrgVfCoef[i++];
        unPara.stBits.u32Para1 = OrgVfCoef[i++];
        unPara.stBits.u32Para2 = OrgVfCoef[i++];
        unPara.stBits.u32Para3 = OrgVfCoef[i++];
        unPara.stBits.u32Para4 = OrgVfCoef[i++];
        *(pVfCoef + j) = unPara.u32All;
    }

    memset(&s_stParaTable, 0, sizeof(s_stParaTable));

    s_stParaTable.u32HfCoefAddr = wgetphy((HI_VOID *)pHfCoef);
    s_stParaTable.u32VfCoefAddr = wgetphy((HI_VOID *)pVfCoef);

#ifdef TDE_VERSION_PILOT
    pRgb2YuvCsc = (HI_U32 *)TDE_MALLOC(TDE_CSCTABLE_SIZE * sizeof(HI_U32));
    if (HI_NULL == pRgb2YuvCsc)
    {
        TDE_FREE(pHfCoef);
        TDE_FREE(pVfCoef);
        TDE_TRACE(TDE_KERN_INFO, "Alloc csc coef failed!\n");
        return HI_FAILURE;
    }
    
    pYuv2RgbCsc = (HI_U32 *)TDE_MALLOC(TDE_CSCTABLE_SIZE * sizeof(HI_U32));
    if (HI_NULL == pYuv2RgbCsc)
    {
        TDE_TRACE(TDE_KERN_INFO, "Alloc csc coef failed\n");
        TDE_FREE(pHfCoef);
        TDE_FREE(pVfCoef);
        TDE_FREE(pRgb2YuvCsc);
        return HI_FAILURE;
    }

    memcpy(pRgb2YuvCsc, s_u32Rgb2YuvCsc, TDE_CSCTABLE_SIZE * sizeof(HI_U32));
    memcpy(pYuv2RgbCsc, s_u32Yuv2RgbCsc, TDE_CSCTABLE_SIZE * sizeof(HI_U32));

    s_u32Rgb2YuvCoefAddr = wgetphy((HI_VOID *)pRgb2YuvCsc);
    s_u32Yuv2RgbCoefAddr = wgetphy((HI_VOID *)pYuv2RgbCsc);
#endif

    return HI_SUCCESS;
}

/*****************************************************************************
* Function:      TdeHalCurLine
* Description:   get the node physics address that is suspended
* Input:         none
* Output:        none
* Return:        line of suspend
* Others:        none
*****************************************************************************/
STATIC INLINE HI_U32 TdeHalCurLine(HI_VOID)
{
    return ((TDE_READ_REG(s_pu32BaseVirAddr, TDE_STA) & 0x0fff0000) >> 16);
}

/*****************************************************************************
* Function:      TdeHalCurNode
* Description:   get the node physics address that is suspended
* Input:         enListType: type of Sq/Aq
* Output:        none
* Return:       the address of current running node
* Others:        none
*****************************************************************************/
HI_U32 TdeHalCurNode(TDE_LIST_TYPE_E enListType)
{
    HI_U32 u32Addr = 0;
    
    if (TDE_LIST_AQ == enListType)
    {
        u32Addr = TDE_READ_REG(s_pu32BaseVirAddr, TDE_AQ_ADDR);
    }
    else
    {
        u32Addr = TDE_READ_REG(s_pu32BaseVirAddr, TDE_SQ_ADDR);
    }
    return u32Addr;
}
/*****************************************************************************
* Function:      TdeHalNodeSetSrc2Base
* Description:   set base info of source bitmap 2 
* Input:         pHWNode: pointer of hardware list
*                pDrvSurface: info of bitmap
*                bS2Size: whether set the source size
* Output:        none
* Return:       physics address of current node on list
* Others:       none
*****************************************************************************/
STATIC HI_VOID TdeHalNodeSetSrc2Base(TDE_HWNode_S* pHWNode, TDE_DRV_SURFACE_S* pDrvSurface, HI_BOOL bS2Size)
{
    TDE_SRC_TYPE_U unSrcType;
    TDE_SUR_XY_U unXy;
    TDE_Y_PITCH_U unYPitch;
    TDE_ARGB_ORDER_U unArgbOrder;
    TDE_INS_U unIns;	
    
    TDE_ASSERT(HI_NULL != pHWNode);
    TDE_ASSERT(HI_NULL != pDrvSurface);

    /*set attribute info for source bitmap*//*CNcomment:配置源位图属性信息*/
    unSrcType.u32All = 0;
    unSrcType.stBits.u32SrcColorFmt = (HI_U32)pDrvSurface->enColorFmt;
    unSrcType.stBits.u32AlphaRange = (HI_U32)pDrvSurface->bAlphaMax255;
    unSrcType.stBits.u32HScanOrd = (HI_U32)pDrvSurface->enHScan;
    unSrcType.stBits.u32VScanOrd = (HI_U32)pDrvSurface->enVScan;
    unSrcType.stBits.u32RgbExp = 0; 

    unXy.u32All = pHWNode->u32TDE_S2_XY;
    unXy.stBits.u32X = pDrvSurface->u32Xpos;
    unXy.stBits.u32Y = pDrvSurface->u32Ypos;

#if defined(TDE_VERSION_PILOT) || defined(TDE_VERSION_FPGA)
    if (pDrvSurface->enColorFmt >= TDE_DRV_COLOR_FMT_YCbCr400MBP)
    {
        unSrcType.stBits.u32Pitch = pDrvSurface->u32CbCrPitch;
        unYPitch.stBits.u32Pitch = pDrvSurface->u32Pitch;
        
	unIns.u32All = pHWNode->u32TDE_INS;
	unIns.stBits.u32Y2En = HI_TRUE;

        pHWNode->u32TDE_S2_ADDR = pDrvSurface->u32CbCrPhyAddr;
        pHWNode->u32TDE_Y2_ADDR = pDrvSurface->u32PhyAddr;
        pHWNode->u32TDE_Y2_PITCH = unYPitch.u32All;
	 pHWNode->u32TDE_INS = unIns.u32All;

        TDE_SET_UPDATE(u32TDE_Y2_ADDR, pHWNode->u64TDE_UPDATE);
        TDE_SET_UPDATE(u32TDE_Y2_PITCH, pHWNode->u64TDE_UPDATE);
	TDE_SET_UPDATE(u32TDE_INS, pHWNode->u64TDE_UPDATE);	
    }
    else
#endif
    {
        unSrcType.stBits.u32Pitch = pDrvSurface->u32Pitch;
        pHWNode->u32TDE_S2_ADDR = pDrvSurface->u32PhyAddr;
#if defined(TDE_VERSION_PILOT) || defined(TDE_VERSION_FPGA)
        if (pDrvSurface->enColorFmt <= TDE_DRV_COLOR_FMT_ARGB8888)
        {
            unArgbOrder.u32All = pHWNode->u32TDE_ARGB_ORDER;
            unArgbOrder.stBits.u32Src2ArgbOrder = pDrvSurface->enRgbOrder;
            pHWNode->u32TDE_ARGB_ORDER = unArgbOrder.u32All;
            TDE_SET_UPDATE(u32TDE_ARGB_ORDER, pHWNode->u64TDE_UPDATE);
        }
#endif
    }

    /*set node*//*CNcomment:配置缓存节点*/
    pHWNode->u32TDE_S2_TYPE = unSrcType.u32All;
    pHWNode->u32TDE_S2_XY = unXy.u32All;

    /*set node update area to 1*//*CNcomment:HWNode.TDE_UPDATE中有配置项对应位设1*/
    TDE_SET_UPDATE(u32TDE_S2_ADDR, pHWNode->u64TDE_UPDATE);
    TDE_SET_UPDATE(u32TDE_S2_TYPE, pHWNode->u64TDE_UPDATE);
    TDE_SET_UPDATE(u32TDE_S2_XY, pHWNode->u64TDE_UPDATE);
    
    if (bS2Size)
    {
        TDE_SUR_SIZE_U unSurSize;
        unSurSize.u32All = 0;
        /*set size info of bitmap*//*CNcomment:配置位图大小信息*/
        unSurSize.stBits.u32Width = (HI_U32)pDrvSurface->u32Width;
        unSurSize.stBits.u32Height = (HI_U32)pDrvSurface->u32Height;
        pHWNode->u32TDE_S2_SIZE = unSurSize.u32All;
        TDE_SET_UPDATE(u32TDE_S2_SIZE, pHWNode->u64TDE_UPDATE);
    }

    return;
}

STATIC HI_VOID TdeHalNodeSetSrc2BaseEx(TDE_HWNode_S* pHWNode, TDE_DRV_SURFACE_S* pDrvSurface, HI_BOOL bS2Size)
{
    TDE_SRC_TYPE_U unSrcType;
    TDE_SUR_XY_U unXy;
    
    TDE_ASSERT(HI_NULL != pHWNode);
    TDE_ASSERT(HI_NULL != pDrvSurface);

    /*set attribute info for source bitmap *//*CNcomment:配置源位图属性信息*/
    unSrcType.u32All = 0;
    unSrcType.stBits.u32Pitch = pDrvSurface->u32Pitch;
    unSrcType.stBits.u32SrcColorFmt = (HI_U32)pDrvSurface->enColorFmt;
    unSrcType.stBits.u32AlphaRange = (HI_U32)pDrvSurface->bAlphaMax255;
    unSrcType.stBits.u32HScanOrd = (HI_U32)pDrvSurface->enHScan;
    unSrcType.stBits.u32VScanOrd = (HI_U32)pDrvSurface->enVScan;
    unSrcType.stBits.u32RgbExp = 0; 
    
    unXy.u32All = pHWNode->u32TDE_S2_XY;
    unXy.stBits.u32X = pDrvSurface->u32Xpos;
    unXy.stBits.u32Y = pDrvSurface->u32Ypos;

    /*set node*//*CNcomment:配置缓存节点*/
    pHWNode->u32TDE_S2_ADDR = pDrvSurface->u32PhyAddr;
    pHWNode->u32TDE_S2_TYPE = unSrcType.u32All;
    pHWNode->u32TDE_S2_XY = unXy.u32All;

    /*set node update area to 1*//*CNcomment:HWNode.TDE_UPDATE中有配置项对应位设1*/
    TDE_SET_UPDATE(u32TDE_S2_ADDR, pHWNode->u64TDE_UPDATE);
    TDE_SET_UPDATE(u32TDE_S2_TYPE, pHWNode->u64TDE_UPDATE);
    TDE_SET_UPDATE(u32TDE_S2_XY, pHWNode->u64TDE_UPDATE);
    
    if (bS2Size)
    {
        TDE_SUR_SIZE_U unSurSize;
        unSurSize.u32All = 0;
        /*set size info of bitmap*//*CNcomment:配置位图大小信息*/
        unSurSize.stBits.u32Width = (HI_U32)pDrvSurface->u32Width;
        unSurSize.stBits.u32Height = (HI_U32)pDrvSurface->u32Height;
        pHWNode->u32TDE_S2_SIZE = unSurSize.u32All;
        TDE_SET_UPDATE(u32TDE_S2_SIZE, pHWNode->u64TDE_UPDATE);
    }

    return;
}

/*****************************************************************************
* Function:      TdeHalGetbppByFmt
* Description:   get bpp according color of driver
* Input:         enFmt: color type
* Output:        pitch width
* Return:        -1: wrong format
* Others:        none
*****************************************************************************/
STATIC HI_S32  TdeHalGetbppByFmt(TDE_DRV_COLOR_FMT_E enFmt)
{
    switch (enFmt)
    {
    case TDE_DRV_COLOR_FMT_RGB444:
    case TDE_DRV_COLOR_FMT_RGB555:
    case TDE_DRV_COLOR_FMT_RGB565:
    case TDE_DRV_COLOR_FMT_ARGB4444:
    case TDE_DRV_COLOR_FMT_ARGB1555:
    case TDE_DRV_COLOR_FMT_ACLUT88:
    case TDE_DRV_COLOR_FMT_YCbCr422:
    case TDE_DRV_COLOR_FMT_halfword:
        return 16;

    case TDE_DRV_COLOR_FMT_RGB888:
    case TDE_DRV_COLOR_FMT_ARGB8565:
    case TDE_DRV_COLOR_FMT_YCbCr888:    
        return 24;    
    case TDE_DRV_COLOR_FMT_ARGB8888:
    case TDE_DRV_COLOR_FMT_AYCbCr8888:
    case TDE_DRV_COLOR_FMT_RABG8888:
        return 32;

    case TDE_DRV_COLOR_FMT_CLUT1:
    case TDE_DRV_COLOR_FMT_A1:
        return 1;

    case TDE_DRV_COLOR_FMT_CLUT2:
        return 2;

    case TDE_DRV_COLOR_FMT_CLUT4:
        return 4;

    case TDE_DRV_COLOR_FMT_CLUT8:
    case TDE_DRV_COLOR_FMT_ACLUT44:
    case TDE_DRV_COLOR_FMT_A8:
    case TDE_DRV_COLOR_FMT_byte:
        return 8;

    case TDE_DRV_COLOR_FMT_YCbCr400MBP:
    case TDE_DRV_COLOR_FMT_YCbCr422MBH:
    case TDE_DRV_COLOR_FMT_YCbCr422MBV:
    case TDE_DRV_COLOR_FMT_YCbCr420MB:
    case TDE_DRV_COLOR_FMT_YCbCr444MB:
    case TDE_DRV_COLOR_FMT_BUTT:
    default:
        return -1;
    }
}

/*****************************************************************************
* Function:      TdeHalGetResizeParaTable
* Description:   get index table according the step
* Input:         u32Step: input step
* Output:        none
* Return:        index table address
* Others:        none
*****************************************************************************/
STATIC INLINE HI_U32 TdeHalGetResizeParaHTable(HI_U32 u32Step)
{
    HI_U32 u32Index;

    if (TDE_RESIZE_8X32_AREA_0 > u32Step)
    {
        u32Index = 0;
    }
    else if (TDE_RESIZE_8X32_AREA_0 <= u32Step && TDE_RESIZE_8X32_AREA_1 > u32Step)
    {
        u32Index = 1;
    }
    else if (TDE_RESIZE_8X32_AREA_1 <= u32Step && TDE_RESIZE_8X32_AREA_2 > u32Step)
    {
        u32Index = 2;
    }
    else if (TDE_RESIZE_8X32_AREA_2 <= u32Step && TDE_RESIZE_8X32_AREA_3 > u32Step)
    {
        u32Index = 3;
    }
    else if (TDE_RESIZE_8X32_AREA_3 <= u32Step && TDE_RESIZE_8X32_AREA_4 > u32Step)
    {
        u32Index = 4;
    }
    else /* TDE_RESIZE_8X32_AREA_4 <= u32Step */
    {
        u32Index = 5;
    }

    return u32Index;
}

/*****************************************************************************
* Function:      TdeHalGetResizeParaVTable
* Description:   get table of parameter for resize
* Input:         u32Step:input step
* Output:        none
* Return:        address of table
* Others:        none
*****************************************************************************/
STATIC INLINE HI_U32 TdeHalGetResizeParaVTable(HI_U32 u32Step)
{
    HI_U32 u32Index;

    /*get index table according step*//*CNcomment: 根据步长找到表的索引 */
    if (u32Step < TDE_RESIZE_PARA_AREA_0)
    {
        u32Index = 0;
    }
    else if (u32Step >= TDE_RESIZE_PARA_AREA_0 && u32Step < TDE_RESIZE_PARA_AREA_1)
    {
        u32Index = 1;
    }
    else if (u32Step >= TDE_RESIZE_PARA_AREA_1 && u32Step < TDE_RESIZE_PARA_AREA_2)
    {
        u32Index = 2;
    }
    else if (u32Step >= TDE_RESIZE_PARA_AREA_2 && u32Step < TDE_RESIZE_PARA_AREA_3)
    {
        u32Index = 3;
    }
    else if (u32Step >= TDE_RESIZE_PARA_AREA_3 && u32Step < TDE_RESIZE_PARA_AREA_4)
    {
        u32Index = 4;
    }
    else if (u32Step >= TDE_RESIZE_PARA_AREA_4 && u32Step < TDE_RESIZE_PARA_AREA_5)
    {
        u32Index = 5;
    }
    else if (u32Step >= TDE_RESIZE_PARA_AREA_5 && u32Step < TDE_RESIZE_PARA_AREA_6)
    {
        u32Index = 6;
    }
    else if (u32Step >= TDE_RESIZE_PARA_AREA_6 && u32Step < TDE_RESIZE_PARA_AREA_7)
    {
        u32Index = 7;
    }
    else if (u32Step >= TDE_RESIZE_PARA_AREA_7 && u32Step < TDE_RESIZE_PARA_AREA_8)
    {
        u32Index = 8;
    }
    else if (u32Step >= TDE_RESIZE_PARA_AREA_8 && u32Step < TDE_RESIZE_PARA_AREA_9)
    {
        u32Index = 9;
    }
    else if (u32Step >= TDE_RESIZE_PARA_AREA_9 && u32Step < TDE_RESIZE_PARA_AREA_10)
    {
        u32Index = 10;
    }
    else /* u32Step >= TDE_RESIZE_PARA_AREA_10 */
    {
        u32Index = 11;
    }

    return u32Index;
}
/*****************************************************************************
* Function:      TdeHalGetOffsetInNode
* Description:   get offset of operation according u32MaskUpdt
* Input:         none
* Output:        none
* Return:        -1:offse
* Others:        none
*****************************************************************************/
STATIC INLINE HI_S32 TdeHalGetOffsetInNode(HI_U64 u64MaskUpdt, HI_U64 u64Update)
{
    HI_U32 i = 0;
    HI_S32 s32Ret = 0;

    if (0 == (u64MaskUpdt & u64Update))
    {
        return -1;
    }
    
    for (i = 0; i < 64; i++)
    {
        if (1 == ((u64MaskUpdt >> i) & 0x1))
        {
            break;
        }
        if (1 == ((u64Update >> i) & 0x1))
        {
            s32Ret++;
        }
    }
    return s32Ret;
}
/*****************************************************************************
* Function:      TdeHalRestoreAloneNode
* Description:   resume task that is singal node
* Input:         none
* Output:        none
* Return:        success/fail
* Others:       none
*****************************************************************************/
HI_S32 TdeHalRestoreAloneNode(HI_VOID)
{
    HI_U32* pu32CurVirAddr = HI_NULL;
    HI_U16 u16CurLine = (HI_U16)s_stSuspStat.s32AqSuspLine;
    TDE_SUR_XY_U unXy;
    TDE_SUR_SIZE_U unSize;
    TDE_SRC_TYPE_U unSrcType;
    TDE_TAR_TYPE_U unTgtType;

    unSize.u32All = 0;
    unXy.u32All = 0;
    unSrcType.u32All = 0;
    unTgtType.u32All = 0;

    /*find the set of current node start position and according line number modify start position*/
    /*CNcomment:找到当前节点起始位置的设置,根据保存的行号修改起始位置 */

    /*resume source 1 info*/
    /*CNcomment: -1- 恢复s1信息-- */
    /*not need get/set bitmap size info,source size is same with target size */
    /*CNcomment:不需获取/设置位图尺寸信息, S1尺寸和Tgt尺寸一致 */
    /*CNcomment: 获取S1扫描方向信息 */
    TDE_GET_MEMBER_IN_BUFNODE(pu32CurVirAddr, u32TDE_S1_TYPE, s_stSuspStat.pstSwBuf);
    if (HI_NULL != pu32CurVirAddr)
    {
        unSrcType.u32All = *pu32CurVirAddr;
    }
    /*get/set source 1 bitmap position info*/
    /*CNcomment: 获取/设置S1位图位置信息 */
    TDE_GET_MEMBER_IN_BUFNODE(pu32CurVirAddr, u32TDE_S1_XY, s_stSuspStat.pstSwBuf);
    if (HI_NULL != pu32CurVirAddr)
    {
        unXy.u32All = *pu32CurVirAddr;

        if (1 == unSrcType.stBits.u32VScanOrd) /*in reverse scan*//*CNcomment:反扫描*/
        {
            unXy.stBits.u32Y -= u16CurLine;
        }
        else
        {
            unXy.stBits.u32Y += u16CurLine;
        }
        *pu32CurVirAddr = unXy.u32All;
    }

    /*resume source 2 info*//*CNcomment: -2- 恢复s2信息-- */
    unXy.u32All = 0;
    unSrcType.u32All = 0;
    
    /*get/set source 2 bitmap position info*/
    /*CNcomment: 获取/设置S2位图尺寸信息 */
    TDE_GET_MEMBER_IN_BUFNODE(pu32CurVirAddr, u32TDE_S2_SIZE, s_stSuspStat.pstSwBuf);
    if (HI_NULL != pu32CurVirAddr)
    {
        unSize.u32All = *pu32CurVirAddr;
        unSize.stBits.u32Height -= u16CurLine;
        *pu32CurVirAddr = unSize.u32All;
    }
    /*CNcomment: 获取S2扫描方向信息 */
    TDE_GET_MEMBER_IN_BUFNODE(pu32CurVirAddr, u32TDE_S2_TYPE, s_stSuspStat.pstSwBuf);
    if (HI_NULL != pu32CurVirAddr)
    {
        unSrcType.u32All = *pu32CurVirAddr;
    }
    /*get/set source 2 bitmap position info*/
    /*CNcomment: 获取/设置S2位图位置信息 */
    TDE_GET_MEMBER_IN_BUFNODE(pu32CurVirAddr, u32TDE_S2_XY, s_stSuspStat.pstSwBuf);
    if (HI_NULL != pu32CurVirAddr)
    {
        unXy.u32All = *pu32CurVirAddr;
        if (1 == unSrcType.stBits.u32VScanOrd) /*CNcomment:反扫描*/
        {
            unXy.stBits.u32Y -= u16CurLine;
        }
        else
        {
            unXy.stBits.u32Y += u16CurLine;
        }
        *pu32CurVirAddr = unXy.u32All;
    }

    /*resume source 3 info*//*CNcomment: -3- 恢复s3信息-- */
    unSize.u32All = 0;
    unSrcType.u32All = 0;
    unXy.u32All = 0;

    /*get/set source target bitmap size info*/
    /*CNcomment: 获取/设置Tgt位图尺寸信息 */
    TDE_GET_MEMBER_IN_BUFNODE(pu32CurVirAddr, u32TDE_TS_SIZE, s_stSuspStat.pstSwBuf);
    if (HI_NULL != pu32CurVirAddr)
    {
        unSize.u32All = *pu32CurVirAddr;
        unSize.stBits.u32Height -= u16CurLine;
        *pu32CurVirAddr = unSize.u32All;
    }
    /*CNcomment: 获取Tgt扫描方向信息 */
    TDE_GET_MEMBER_IN_BUFNODE(pu32CurVirAddr, u32TDE_TAR_TYPE, s_stSuspStat.pstSwBuf);
    if (HI_NULL != pu32CurVirAddr)
    {
        unTgtType.u32All = *pu32CurVirAddr;
    }
    /*CNcomment: 获取/设置Tgt位图位置信息 */
    TDE_GET_MEMBER_IN_BUFNODE(pu32CurVirAddr, u32TDE_TAR_XY, s_stSuspStat.pstSwBuf);
    if (HI_NULL != pu32CurVirAddr)
    {
        unXy.u32All = *pu32CurVirAddr;
        if (1 == unTgtType.stBits.u32VScanOrd) /*CNcomment:反扫描*/
        {
            unXy.stBits.u32Y -= u16CurLine;
        }
        else
        {
            unXy.stBits.u32Y += u16CurLine;
        }
        *pu32CurVirAddr = unXy.u32All;
    }

    /*CNcomment:写入当前AQ链表物理地址*/
    TDE_WRITE_REG(s_pu32BaseVirAddr, TDE_AQ_NADDR, s_stSuspStat.pstSwBuf->u32PhyAddr);
    TDE_WRITE_REG(s_pu32BaseVirAddr, TDE_AQ_UPDATE, s_stSuspStat.pstSwBuf->u32CurUpdate);
    return HI_SUCCESS;
}

#if 0
STATIC INLINE HI_S32 TdeHalRestoreChildNode(HI_VOID)
{
    HI_U32 *pu32ParentNAddr = HI_NULL;
    HI_U32 *pu32CurNAddr = HI_NULL;
    TDE_BUF_NODE_S* pstParentBuf = HI_NULL;
    TDE_BUF_NODE_S* pstSuspBuf = HI_NULL;
    struct list_head *pstHeadList = HI_NULL;
    
    if (s_stSuspStat.pstSwBuf->u32PhyAddr > s_stQueueInfo.stQuAddr[TDE_SUBMIT_AQ1].u32NodeStartPhyAddr
        && s_stSuspStat.pstSwBuf->u32PhyAddr < s_stQueueInfo.stQuAddr[TDE_SUBMIT_AQ1].u32NodeEndPhyAddr)
    {
        pstHeadList = &s_stQueueInfo.stQuAddr[TDE_SUBMIT_AQ1].stList;
    }
    else if (s_stSuspStat.pstSwBuf->u32PhyAddr > s_stQueueInfo.stQuAddr[TDE_SUBMIT_AQ2].u32NodeStartPhyAddr
        && s_stSuspStat.pstSwBuf->u32PhyAddr < s_stQueueInfo.stQuAddr[TDE_SUBMIT_AQ2].u32NodeEndPhyAddr)
    {
        pstHeadList = &s_stQueueInfo.stQuAddr[TDE_SUBMIT_AQ2].stList;
    }
    else
    {
        return HI_FAILURE;
    }

    if (s_stSuspStat.pstSwBuf->stList.next == pstHeadList)
    {
        return HI_SUCCESS;
    }

    pstSuspBuf = s_stSuspStat.pstSwBuf;
    
    pstParentBuf = pstSuspBuf->pstParentAddr;

    if (HI_NULL == pstParentBuf)
    {
        TDE_WRITE_REG(s_pu32BaseVirAddr, TDE_AQ_NADDR, pstSuspBuf->u32PhyAddr);
        TDE_WRITE_REG(s_pu32BaseVirAddr, TDE_AQ_UPDATE, pstSuspBuf->u32CurUpdate);
        return HI_SUCCESS;
    }

    pu32ParentNAddr = TDE_GET_CUR_NADDR_VADDR(pstParentBuf);
    pu32CurNAddr = TDE_GET_CUR_NADDR_VADDR(pstSuspBuf);
    *pu32ParentNAddr = *pu32CurNAddr;

    TdeHalSetChildToParent(pstParentBuf->pu32VirAddr, pstSuspBuf->pu32VirAddr, 
                           pstParentBuf->u32CurUpdate, pstSuspBuf->u32CurUpdate);

    TDE_WRITE_REG(s_pu32BaseVirAddr, TDE_AQ_NADDR, pstParentBuf->u32PhyAddr);
    TDE_WRITE_REG(s_pu32BaseVirAddr, TDE_AQ_UPDATE, pstParentBuf->u32CurUpdate);

    return HI_SUCCESS;
}
#endif
/*****************************************************************************
* Function:      TdeHalSetChildToParent
* Description:   copy info of child  to parent 
* Input:         pParent: parent info
*                pChild:  child info
*                u32ParentUpdt: parent update area
*                ChildUpdt      child update area
* Output:        none
* Return:        success/fail
* Others:        none
*****************************************************************************/
STATIC INLINE HI_VOID TdeHalSetChildToParent(HI_U32* pParent, HI_U32* pChild, HI_U32 u32ParentUpdt, HI_U32 ChildUpdt)
{
    HI_U32 *pu32ParentInBuf = HI_NULL;
    HI_U32 *pu32ChildInBuf = HI_NULL;

    /* AE5D03314 beg */
    TDE_GET_MEMBER_IN_BUFF(pu32ParentInBuf, u32TDE_INS, pParent, u32ParentUpdt);
    TDE_GET_MEMBER_IN_BUFF(pu32ChildInBuf, u32TDE_INS, pChild, ChildUpdt);
    if (HI_NULL != pu32ParentInBuf && HI_NULL != pu32ChildInBuf)
    {
        *pu32ParentInBuf = *pu32ChildInBuf;
    }    
    /*CNcomment: AE5D03314 end */
    
    /*CNcomment: 将当前节点的TDE_S1_XY, TDE_S2_XY, TDE_TAR_XY, TDE_S2_SIZE, 
       TDE_TS_SIZE, TDE_RSZ_X_OFST,TDE_RSZ_X_OFST 配置到父节点的相应项中 */
    TDE_GET_MEMBER_IN_BUFF(pu32ParentInBuf, u32TDE_S1_XY, pParent, u32ParentUpdt);
    TDE_GET_MEMBER_IN_BUFF(pu32ChildInBuf, u32TDE_S1_XY, pChild, ChildUpdt);
    if (HI_NULL != pu32ParentInBuf && HI_NULL != pu32ChildInBuf)
    {
        *pu32ParentInBuf = *pu32ChildInBuf;
    }
    TDE_GET_MEMBER_IN_BUFF(pu32ParentInBuf, u32TDE_S2_XY, pParent, u32ParentUpdt);
    TDE_GET_MEMBER_IN_BUFF(pu32ChildInBuf, u32TDE_S2_XY, pChild, ChildUpdt);
    if (HI_NULL != pu32ParentInBuf && HI_NULL != pu32ChildInBuf)
    {
        *pu32ParentInBuf = *pu32ChildInBuf;
    }
    TDE_GET_MEMBER_IN_BUFF(pu32ParentInBuf, u32TDE_TAR_XY, pParent, u32ParentUpdt);
    TDE_GET_MEMBER_IN_BUFF(pu32ChildInBuf, u32TDE_TAR_XY, pChild, ChildUpdt);
    if (HI_NULL != pu32ParentInBuf && HI_NULL != pu32ChildInBuf)
    {
        *pu32ParentInBuf = *pu32ChildInBuf;
    }
    TDE_GET_MEMBER_IN_BUFF(pu32ParentInBuf, u32TDE_S2_SIZE, pParent, u32ParentUpdt);
    TDE_GET_MEMBER_IN_BUFF(pu32ChildInBuf, u32TDE_S2_SIZE, pChild, ChildUpdt);
    if (HI_NULL != pu32ParentInBuf && HI_NULL != pu32ChildInBuf)
    {
        *pu32ParentInBuf = *pu32ChildInBuf;
    }
    TDE_GET_MEMBER_IN_BUFF(pu32ParentInBuf, u32TDE_TS_SIZE, pParent, u32ParentUpdt);
    TDE_GET_MEMBER_IN_BUFF(pu32ChildInBuf, u32TDE_TS_SIZE, pChild, ChildUpdt);
    if (HI_NULL != pu32ParentInBuf && HI_NULL != pu32ChildInBuf)
    {
        *pu32ParentInBuf = *pu32ChildInBuf;
    }
    TDE_GET_MEMBER_IN_BUFF(pu32ParentInBuf, u32TDE_RSZ_Y_OFST, pParent, u32ParentUpdt);
    TDE_GET_MEMBER_IN_BUFF(pu32ChildInBuf, u32TDE_RSZ_Y_OFST, pChild, ChildUpdt);
    if (HI_NULL != pu32ParentInBuf && HI_NULL != pu32ChildInBuf)
    {
        *pu32ParentInBuf = *pu32ChildInBuf;
    }
    TDE_GET_MEMBER_IN_BUFF(pu32ParentInBuf, u32TDE_RSZ_X_OFST, pParent, u32ParentUpdt);
    TDE_GET_MEMBER_IN_BUFF(pu32ChildInBuf, u32TDE_RSZ_X_OFST, pChild, ChildUpdt);
    if (HI_NULL != pu32ParentInBuf && HI_NULL != pu32ChildInBuf)
    {
        *pu32ParentInBuf = *pu32ChildInBuf;
    }
}

/*****************************************************************************
* Function:      TdeHalSetXyByAdjInfo
* Description:   pHWNode:   hardware node info
*                pChildInfo: child info
* Input:         none
* Output:        none
* Return:        none
* Others:        none
*****************************************************************************/
STATIC INLINE HI_VOID TdeHalSetXyByAdjInfo(TDE_HWNode_S* pHWNode, TDE_CHILD_INFO* pChildInfo)
{
    TDE_SUR_XY_U unXy;
    
    switch (pChildInfo->stAdjInfo.enScaleMode)
    {
    case TDE_CHILD_SCALE_NORM:
    case TDE_CHILD_SCALE_MBC:
        {
            /*CNcomment: 直接按照S2分块 */
            unXy.u32All = pHWNode->u32TDE_S2_XY;
            unXy.stBits.u32X = pChildInfo->u32Xi;
            unXy.stBits.u32Y = pChildInfo->u32Yi;
            pHWNode->u32TDE_S2_XY = unXy.u32All;
            TDE_SET_UPDATE(u32TDE_S2_XY, pHWNode->u64TDE_UPDATE);
            break;
        }
    case TDE_CHILD_SCALE_MBY:
        {
            /*CNcomment: 直接按照S1分块 */
            unXy.u32All = pHWNode->u32TDE_S1_XY;
            unXy.stBits.u32X = pChildInfo->u32Xi;
            unXy.stBits.u32Y = pChildInfo->u32Yi;
            pHWNode->u32TDE_S1_XY = unXy.u32All;
            TDE_SET_UPDATE(u32TDE_S1_XY, pHWNode->u64TDE_UPDATE);
            break;
        }
    case TDE_CHILD_SCALE_MB_CONCA_H:
    case TDE_CHILD_SCALE_MB_CONCA_M:
        {
            /*CNcomment: 按照亮度S1分块, 调整色度S2起始位置 */
            unXy.u32All = pHWNode->u32TDE_S1_XY;
            unXy.stBits.u32X = pChildInfo->u32Xi;
            unXy.stBits.u32Y = pChildInfo->u32Yi;
            pHWNode->u32TDE_S1_XY = unXy.u32All;
            TDE_SET_UPDATE(u32TDE_S1_XY, pHWNode->u64TDE_UPDATE);

            unXy.u32All = pHWNode->u32TDE_S2_XY;
            unXy.stBits.u32X = pChildInfo->u32Xi - pChildInfo->stAdjInfo.u32StartInX;
            unXy.stBits.u32Y = 0;
            pHWNode->u32TDE_S2_XY = unXy.u32All;
            TDE_SET_UPDATE(u32TDE_S2_XY, pHWNode->u64TDE_UPDATE);
            break;
        }
    case TDE_CHILD_SCALE_MB_CONCA_L:
        {
            /*CNcomment: 按照亮度S1分块, 调整色度S2起始位置 */
            unXy.u32All = pHWNode->u32TDE_S1_XY;
            unXy.stBits.u32X = pChildInfo->u32Xi;
            unXy.stBits.u32Y = pChildInfo->u32Yi;
            pHWNode->u32TDE_S1_XY = unXy.u32All;
            TDE_SET_UPDATE(u32TDE_S1_XY, pHWNode->u64TDE_UPDATE);

            unXy.u32All = pHWNode->u32TDE_S2_XY;
            unXy.stBits.u32X = pChildInfo->u32Xi;
            unXy.stBits.u32Y = pChildInfo->u32Yi;
			
            if (TDE_DRV_COLOR_FMT_YCbCr420MB == pChildInfo->stAdjInfo.enFmt
                || TDE_DRV_COLOR_FMT_YCbCr422MBH == pChildInfo->stAdjInfo.enFmt)
            {
                unXy.stBits.u32X = (pChildInfo->u32Xi >> 1);
            }
            if (TDE_DRV_COLOR_FMT_YCbCr420MB == pChildInfo->stAdjInfo.enFmt
                || TDE_DRV_COLOR_FMT_YCbCr422MBV == pChildInfo->stAdjInfo.enFmt)
            {
                unXy.stBits.u32Y = (pChildInfo->u32Yi >> 1);
            }
 
            pHWNode->u32TDE_S2_XY = unXy.u32All;
            TDE_SET_UPDATE(u32TDE_S2_XY, pHWNode->u64TDE_UPDATE);
            break;
        }
    case TDE_CHILD_SCALE_MB_CONCA_CUS:
        {
            /*CNcomment: 按照色度S2分块, 调整亮度S1起始位置 */
            unXy.u32All = pHWNode->u32TDE_S2_XY;
            unXy.stBits.u32X = pChildInfo->u32Xi;
            unXy.stBits.u32Y = pChildInfo->u32Yi;
            pHWNode->u32TDE_S2_XY = unXy.u32All;
            TDE_SET_UPDATE(u32TDE_S2_XY, pHWNode->u64TDE_UPDATE);

            unXy.u32All = pHWNode->u32TDE_S1_XY;
            unXy.stBits.u32X = pChildInfo->u32Xo - pChildInfo->stAdjInfo.u32StartOutX 
                               + pChildInfo->stAdjInfo.u32StartInX;
            unXy.stBits.u32Y = pChildInfo->stAdjInfo.u32StartInY;
            pHWNode->u32TDE_S1_XY = unXy.u32All;
            TDE_SET_UPDATE(u32TDE_S1_XY, pHWNode->u64TDE_UPDATE);
			break;
	 }
    default:
        break;
    }
}

STATIC INLINE HI_VOID TdeHalInitQueue(HI_VOID)
{
    TDE_AQ_CTRL_U unAqCtrl;
    TDE_SQ_CTRL_U unSqCtrl;

    /*write 0 to Aq/Sq list start address register*/
    /*CNcomment: 将Aq/Sq链表首地址寄存器写0 */
    TDE_WRITE_REG(s_pu32BaseVirAddr, TDE_AQ_NADDR, 0);
    TDE_WRITE_REG(s_pu32BaseVirAddr, TDE_SQ_NADDR, 0);

    unAqCtrl.u32All = TDE_READ_REG(s_pu32BaseVirAddr, TDE_AQ_CTRL);
    unSqCtrl.u32All = TDE_READ_REG(s_pu32BaseVirAddr, TDE_SQ_CTRL);

    
    /*enable Aq/Sq list*//*CNcomment: 使能Aq/Sq链表 */
    unAqCtrl.stBits.u32AqEn = 1;
    unSqCtrl.stBits.u32SqEn = 0;   /*hardware disable*//*CNcomment: 硬件暂时屏蔽*/

    /*CNcomment:  设置SQ的触发条件和同步模式 */
    unSqCtrl.stBits.u32TrigMode = 0;
    unSqCtrl.stBits.u32TrigCond = 0;

    /*set Sq/Aq operation mode*//*CNcomment:  配置Sq/Aq操作模式 */
    unSqCtrl.stBits.u32SqOperMode = TDE_SQ_CTRL_COMP_LIST; 
    unAqCtrl.stBits.u32AqOperMode = TDE_AQ_CTRL_COMP_LINE;

    TDE_WRITE_REG(s_pu32BaseVirAddr, TDE_AQ_CTRL, unAqCtrl.u32All);
    TDE_WRITE_REG(s_pu32BaseVirAddr, TDE_SQ_CTRL, unSqCtrl.u32All);
}

HI_S32 TdeHalSetDeflicerLevel(TDE_DEFLICKER_LEVEL_E eDeflickerLevel)
{
    s_eDeflickerLevel = eDeflickerLevel;
    return HI_SUCCESS;
}

HI_S32 TdeHalGetDeflicerLevel(TDE_DEFLICKER_LEVEL_E *pDeflicerLevel)
{
    *pDeflicerLevel = s_eDeflickerLevel;
    return HI_SUCCESS;
}

HI_S32 TdeHalSetAlphaThreshold(HI_U8 u8ThresholdValue)
{
    s_u8AlphaThresholdValue = u8ThresholdValue;
    
    return HI_SUCCESS;
    
}

HI_S32 TdeHalGetAlphaThreshold(HI_U8 *pu8ThresholdValue)
{
    *pu8ThresholdValue = s_u8AlphaThresholdValue;

    return HI_SUCCESS;
}

HI_S32 TdeHalSetAlphaThresholdState(HI_BOOL bEnAlphaThreshold)
{
    s_bEnAlphaThreshold = bEnAlphaThreshold;

    return HI_SUCCESS;
}

HI_S32 TdeHalGetAlphaThresholdState(HI_BOOL *pbEnAlphaThreshold)
{
    *pbEnAlphaThreshold = s_bEnAlphaThreshold;

    return HI_SUCCESS;
}

HI_BOOL bTdeHalSwVersion(HI_VOID)
{
    HI_U32 u32Version;

    u32Version = TDE_READ_REG(s_pu32BaseVirAddr, TDE_VER);

    return (u32Version == TDE_SWVERSION_NUM);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif  /* __cplusplus */
#endif  /* __cplusplus */

