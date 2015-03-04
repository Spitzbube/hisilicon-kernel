
/******************************************************************************

  Copyright (C), 2001-2013, Hisilicon Tech. Co., Ltd.

******************************************************************************
  File Name       	       :      optm_hifb.c
  Version        		: 	Initial Draft
  Author         		: 	Hisilicon multimedia software group
  Created       		:
  Description  	       :      optimus prime 
  History       		:
  1.Date        		       :
  Author      		       :
  Modification   	       :	Created file

******************************************************************************/
#ifndef HI_BUILD_IN_BOOT
#include <linux/string.h>
#include <linux/fb.h>

#include <linux/time.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/fcntl.h>
#include <linux/mm.h>
#include <linux/miscdevice.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>
#include <linux/slab.h>
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 36))
#include <linux/smp_lock.h>
#endif 
#include <linux/init.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/system.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/string.h>
#include <linux/workqueue.h>

#include "hi_module.h"
#include "hi_drv_module.h"
#include "drv_disp_ext.h"
#else
#include "drv_display.h"
#include "hifb_debug.h"
#endif
#include "optm_hal.h"
#include "optm_hifb.h"
#include "optm_alg_csc.h"
#include "optm_alg_gzme.h"
#include "optm_alg_gsharp.h"
#include "hi_drv_disp.h"

/******************************************************************
                    definitions of functional switches
******************************************************************/
#ifndef HI_BUILD_IN_BOOT
static DISP_EXPORT_FUNC_S *ps_DispExportFuncs = HI_NULL; //81143418
#else
static HI_U32             *ps_DispExportFuncs = HI_NULL;
#endif

#define CFG_HIFB_S40V2_PALNTSC_BUG

#ifdef HI_BUILD_IN_BOOT
#define HIFB_ERROR   printf
#define HIFB_WARNING printf
#define HIFB_INFO(fmt...)
#define HIFB_FATAL   printf
#endif

/* wait v blanking */
#define OPTM_HIFB_WVM_ENABLE 1

/*  call-back after registers' update */
#define OPTM_HIFB_GFXRR_ENABLE 1


#define OPTM_MAX_LOGIC_HIFB_LAYER ((HI_U32)HIFB_LAYER_ID_BUTT) //13

OPTM_ALG_GZME_MEM_S  GfxZmeModule; //81143454

#define DispWidth_HD  1280
#define DispHeight_HD 720

#define DispWidth_SD  720
#define DispHeight_SD 576

#define OPTM_ENABLE  1
#define OPTM_DISABLE 0

#define OPTM_COLOR_DEFVALUE 50

#define OPTM_EXTRACTLINE_RATIO 4
#define OPTM_EXTRACTLINE_WIDTH 1080
#define OPTM_MASTER_GPID OPTM_GFX_GP_0
#define OPTM_SLAVER_GPID OPTM_GFX_GP_1
#define OPTM_SLAVER_LAYERID HIFB_LAYER_SD_0
#define OPTM_CURSOR_LAYERID HIFB_LAYER_SD_1 //HIFB_LAYER_HD_3????



/******************************************************************
                    definitions of data format
******************************************************************/
typedef union _OPTM_GFX_UP_FLAG_U
{
    /*  Define the struct bits */
    struct
    {
        unsigned int    RegUp       : 1 ; /*  [0] */
        unsigned int    Enable      : 1 ; /*  [1] */
        unsigned int    InRect      : 1 ; /*  [2] */
        unsigned int    OutRect     : 1 ; /*  [3] */
        unsigned int    Alpha       : 1 ; /*  [4] */
        unsigned int    PreMute     : 1 ; /*  [5] */
		unsigned int    WbcMode     : 1 ; /*  [6] */
        unsigned int    Reserved    : 25; /*  [31...7] */
    } bits;

    /*  Define an unsigned member */
    unsigned int    u32;
}OPTM_GFX_UP_FLAG_U;


typedef struct tagOPTM_GFX_LAYER_S{
    HI_BOOL              bOpened; //0 (2560)
    HI_BOOL              bMaskFlag;		
	HI_BOOL              bSharpEnable;
	HI_BOOL              bExtractLine;
	/*******backup hardware data of gfx*********/
	HI_BOOL              bEnable; //16
	HI_BOOL              b3DEnable;
	HI_BOOL              bPreMute;
    int fill1[1]; //????
	HI_U32               u32ZOrder; //32
	HI_BOOL              bCampEnable; //36
    HI_BOOL              bBufferChged;    
    HI_U32               s32BufferChgCount;        
    HI_U32               NoCmpBufAddr;
	HI_U32               u32TriDimAddr;
    int fill2[1]; //????
    HI_U16               Stride; //60         /* no compression mode stride*/
    HI_U16               CmpStride;       /* compression mode stride     */
	HIFB_COLOR_FMT_E     enDataFmt;
	HIFB_RECT            stInRect;        /*Inres of gfx*/
    HIFB_ALPHA_S         stAlpha;
	HIFB_COLORKEYEX_S    stColorkey;
	HIFB_STEREO_MODE_E   enTriDimMode;
	OPTM_VDP_BKG_S       stBkg; //128
	OPTM_VDP_GFX_BITEXTEND_E enBitExtend; //148
	OPTM_VDP_DATA_RMODE_E    enReadMode; //152
	OPTM_VDP_DATA_RMODE_E    enUpDateMode; //156
	/*****************end**********************/	
	
	OPTM_VDP_LAYER_GFX_E      enGfxHalId; //160   /*the gfx's hal id*/
	OPTM_GFX_GP_E        enGPId; //164 /*which gp the gfx belong to*/
   
    OPTM_CSC_STATE_E     CscState; //168

	volatile HI_U32      vblflag;
	wait_queue_head_t    vblEvent; //176
    MMZ_BUFFER_S         stCluptTable; //188
    //200
}OPTM_GFX_LAYER_S;

typedef enum tagOPTM_WBC_MODE_E{
	OPTM_WBC_MODE_MONO      = 0x0,
	OPTM_WBC_MODE_LFET_EYE  = 0x10,
	OPTM_WBC_MODE_RIGHT_EYE = 0x11,
	OPTM_WBC_MODE_BUTT,
}OPTM_WBC_MODE_E;

typedef enum tagOPTM_VDP_CONNECT_E
{
    OPTM_VDP_CONNECT_G3_DHD0 = 0, 
    OPTM_VDP_CONNECT_G3_DHD1    ,

    OPTM_VDP_CONNECT_BUTT       ,
}OPTM_VDP_CONNECT_E;



typedef struct tagOPTM_GFX_WBC_S{
    HI_BOOL                bOpened; //0
    HI_BOOL                bEnable; //4

    OPTM_VDP_LAYER_WBC_E        enWbcHalId; //8
    /* setting */
    HI_S32                 s32BufferWidth; //12
    HI_S32                 s32BufferHeight; //16
    HI_U32                 u32BufferStride; //20
    int 				   Data_24; //24
    int					   Data_28; //28
    int 				   Data_32; //32
    int					   Data_36; //36
    int 				   Data_40; //40
    MMZ_BUFFER_S           stFrameBuffer; //44

    HI_U32                 u32DataPoint; //56 /* 0, feeder; others, reserve */

    HIFB_COLOR_FMT_E       enDataFmt; //60

    HIFB_RECT              stInRect; //64???
    HI_BOOL                bInProgressive; //80???
    HIFB_RECT              stOutRect; //84???
    HI_BOOL                bOutProgressive; //100???
    HI_U32                 u32BtmOffset; //104???
    HI_BOOL                bHdDispProgressive; //108???
	OPTM_VDP_DITHER_E      enDitherMode; //112
	OPTM_VDP_WBC_OFMT_E    stWBCFmt; //116
	OPTM_VDP_DATA_RMODE_E  enReadMode; //120
	OPTM_WBC_MODE_E        enWbcMode; //124
	HI_U32 Data_128; //128
	//132
}OPTM_GFX_WBC_S;

/* display ID */
typedef enum tagOPTM_DISPCHANNEL_E
{
    OPTM_DISPCHANNEL_0 = 0,//gfx4,gfx5
    OPTM_DISPCHANNEL_1,    //gfx0,gfx1,gfx2,gfx3
    OPTM_DISPCHANNEL_BUTT
}OPTM_DISPCHANNEL_E;

#ifndef HI_BUILD_IN_BOOT
typedef struct tagOPTM_GFX_WORK_S
{
	HI_U32                   u32Data;
	struct work_struct       work; 
}OPTM_GFX_WORK_S;
#endif

typedef struct tagOPTM_GFX_GP_S
{
	/*Frame format of Output: 0-field; 1-frame*/
	HI_BOOL bOpen;     //the flag of gp initial
	HI_BOOL b3DEnable;   // 3D flag
	HI_BOOL bMaskFlag;
	HI_BOOL bBGRState; //12
	HI_BOOL bInterface;
	HI_BOOL bGpClose; //20
	HI_BOOL bRecoveryInNextVT; //24
	/*wether need to extract line or not*/
	HI_BOOL bNeedExtractLine;
	/*gp_in size setted by usr*/
	HI_BOOL bGPInSetbyusr;
	/*gp_in size got initial by the first opened layer */
	HI_BOOL bGPInInitial;
	/*disp initial*/
	HI_BOOL bDispInitial; //40

	HI_RECT_S stInRect;
	HI_RECT_S stOutRect;

	HIFB_STEREO_MODE_E        enTriDimMode;

	int fill; //???
	/*  about color  */
	OPTM_COLOR_SPACE_E        enInputCsc; //84
	OPTM_COLOR_SPACE_E        enOutputCsc; //88
	OPTM_GFX_CSC_PARA_S       stCscPara;

	OPTM_VDP_LAYER_GP_E       enGpHalId; //120
	OPTM_DISPCHANNEL_E        enDispCh; //124

    OPTM_GFX_UP_FLAG_U        unUpFlag;
	/*declare work queue to open slv layer*/
#ifndef HI_BUILD_IN_BOOT
	struct workqueue_struct   *queue; //132
	OPTM_GFX_WORK_S           stOpenSlvWork;
	OPTM_GFX_WORK_S           st3DModeChgWork;
#endif

	/***save for disp change and suspend***/
	HI_U32 u32Prior; //176
	HI_U32 u32Alpha; //180
	OPTM_VDP_DATA_RMODE_E enReadMode; //184
	OPTM_VDP_BKG_S        stBkg; //188
	OPTM_VDP_CBM_MIX_E 	  enMixg; //208
	//212
}OPTM_GFX_GP_S;

typedef struct tagOPTM_GFX_IRQ_S
{    
    HI_U32 u32Param0;
    HI_U32 u32Param1;
	IntCallBack pFunc;
}OPTM_GFX_IRQ_S;

typedef struct tagOPTM_GFX_CALLBACK_S
{   
	/*each bit: 0---no irq,1---irq
	0---HIFB_CALLBACK_TYPE_VO
	1---HIFB_CALLBACK_TYPE_3DMode_CHG
	2---HIFB_CALLBACK_TYPE_REGUP
	*/
	HI_U32  u32CTypeFlag; 
	OPTM_GFX_IRQ_S stGfxIrq[HIFB_CALLBACK_TYPE_BUTT];
}OPTM_GFX_CALLBACK_S;


#define OPTM_GP_MAXGFXCOUNT 3
typedef struct tagOPTM_GP_IRQ_S
{
	/*Gp only need to register callback func to disp once*/
    HI_BOOL bRegistered[111]/*[HI_DRV_DISP_C_TYPE_BUTT]*/; //0
	
	OPTM_GFX_CALLBACK_S stGfxCallBack[OPTM_GP_MAXGFXCOUNT];
//	int fill[110]; //?????
	//1068
}OPTM_GP_IRQ_S;

/******************************************************************
               capacity set definitions
******************************************************************/
const HIFB_CAPABILITY_S g_stGfxCap[OPTM_MAX_LOGIC_HIFB_LAYER] = //80baf65c
{
    /* HD0 */
    {
    	.bKeyAlpha = 1,
		.bGlobalAlpha = 1,
		.bCmap = 1,
		.bHasCmapReg = 1,

		.bColFmt = {1,1,  1,1,1,  1,1,1,1,  1,1,1,1,  1,1,1,1,1,1,1,1,1,  0,0,0,1,0,0,  0,0,0,0,  0,0}, // 27

		.bVoScale   = HI_TRUE,		
		.bLayerSupported = HI_TRUE,

		.bCompression = 1,
		.bStereo      = 1,
		
		.u32MaxWidth  = 3840,
		.u32MaxHeight = 2160,
		.u32MinWidth  = 0,
		.u32MinHeight = 0,

		.u32VDefLevel = 0,  /* not surpport */
		.u32HDefLevel = 0,  /* not surpport */
    },
    
    /* HD1 */
    {
    	.bKeyAlpha = 1,
		.bGlobalAlpha = 1,
		.bCmap = 0,
		.bHasCmapReg = 0,

		.bColFmt = {1,1,  1,1,1,  1,1,1,1,  1,1,1,1,  1,1,1,1,1,1,1,1,1,  0,0,0,0,0,0,  0,0,0,0,  0,0}, // 27

		.bVoScale   = HI_TRUE,		
		.bLayerSupported = HI_TRUE,

		.bCompression = 0,
		.bStereo      = 0,
		
		.u32MaxWidth  = 3840,
		.u32MaxHeight = 2160,
		.u32MinWidth  = 0,
		.u32MinHeight = 0,

		.u32VDefLevel = 0,  /* not surpport */
		.u32HDefLevel = 0,  /* not surpport */
    },

	/* HD2 */
    {
    	.bKeyAlpha = 1,
		.bGlobalAlpha = 1,
		.bCmap = 0,
		.bHasCmapReg = 0,

		.bColFmt = {1,1,1,1,  1,1,1,1,  1,1,1,1,   1,1,1,1,   1,1,1,1,   1,1,0,0,  0,0,0,0,  0,0,0,0,  0,0}, // 27

		.bVoScale   = HI_TRUE,		
		.bLayerSupported = HI_TRUE,

		.bCompression = 0,
		.bStereo      = 0,
		
		.u32MaxWidth  = 3840,
		.u32MaxHeight = 2160,
		.u32MinWidth  = 0,
		.u32MinHeight = 0,

		.u32VDefLevel = 0,  /* not surpport */
		.u32HDefLevel = 0,  /* not surpport */
    },

    /* HD3 */
    {
    	.bKeyAlpha = 1,
		.bGlobalAlpha = 1,
		.bCmap = 0,
		.bHasCmapReg = 0,

		.bColFmt = {1,1,1,1,  1,1,1,1,  1,1,1,1,   1,1,1,1,   1,1,1,1,   1,1,0,0,  0,0,0,0,  0,0,0,0,  0,0}, // 27

		.bVoScale   = HI_TRUE,		
		.bLayerSupported = HI_TRUE,

		.bCompression = 0,
		.bStereo      = 0,
		
		.u32MaxWidth  = 2560,
		.u32MaxHeight = 1600,
		.u32MinWidth  = 0,
		.u32MinHeight = 0,

		.u32VDefLevel = 0,  /* not surpport */
		.u32HDefLevel = 0,  /* not surpport */
    }, 

    /* SD0 */
    {
    	.bKeyAlpha = 1,
		.bGlobalAlpha = 1,
		.bCmap = 1,
		.bHasCmapReg = 1,

		.bColFmt = {1,1,1,1,  1,1,1,1,  1,1,1,1,   1,1,1,1,   1,1,1,1,   1,1,0,0,  0,1,0,0,  0,0,0,0,  0,0}, // 27

		.bVoScale   = HI_TRUE,		
		.bLayerSupported = HI_TRUE,

		.bCompression = 1,
		.bStereo      = 1,
		
		.u32MaxWidth  = 2560,
		.u32MaxHeight = 1600,
		.u32MinWidth  = 0,
		.u32MinHeight = 0,

		.u32VDefLevel = 0,  /* not surpport */
		.u32HDefLevel = 0,  /* not surpport */
    },

    /* SD1 */
    {
    	.bKeyAlpha = 1,
		.bGlobalAlpha = 1,
		.bCmap = 0,
		.bHasCmapReg = 1,

		.bColFmt = {1,1,1,1,  1,1,1,1,  1,1,1,1,   1,1,1,1,   1,1,1,1,   1,1,0,0,  0,1,0,0,  0,0,0,0,  0,0}, // 27

		.bVoScale   = HI_FALSE,		
		.bLayerSupported = HI_TRUE,

		.bCompression = 0,
		.bStereo      = 1,
		
		.u32MaxWidth  = 2560,
		.u32MaxHeight = 1600,
		.u32MinWidth  = 0,
		.u32MinHeight = 0,

		.u32VDefLevel = 0,  /* not surpport */
		.u32HDefLevel = 0,  /* not surpport */
    },

    /* SD2 */
    {
    	.bKeyAlpha = 1,
		.bGlobalAlpha = 1,
		.bCmap = 0,
		.bHasCmapReg = 0,

		.bColFmt = {1,1,1,1,  1,1,1,1,  1,1,1,1,   1,1,1,1,   1,1,1,1,   1,1,0,0,  0,0,0,0,  0,0,0,0,  0,0}, // 27

		.bVoScale   = HI_FALSE,		
		.bLayerSupported = HI_FALSE,

		.bCompression = 0,
		.bStereo      = 0,
		
		.u32MaxWidth  = 1920,
		.u32MaxHeight = 1080,
		.u32MinWidth  = 0,
		.u32MinHeight = 0,

		.u32VDefLevel = 0,  /* not surpport */
		.u32HDefLevel = 0,  /* not surpport */
    },

    /* SD3 */
    {
    	.bKeyAlpha = 1,
		.bGlobalAlpha = 1,
		.bCmap = 0,
		.bHasCmapReg = 0,

		.bColFmt = {1,1,1,1,  1,1,1,1,  1,1,1,1,   1,1,1,1,   1,1,1,1,   1,1,0,0,  0,0,0,0,  0,0,0,0,  0,0}, // 27

		.bVoScale   = HI_FALSE,		
		.bLayerSupported = HI_FALSE,

		.bCompression = 0,
		.bStereo      = 0,
		
		.u32MaxWidth  = 1920,
		.u32MaxHeight = 1080,
		.u32MinWidth  = 0,
		.u32MinHeight = 0,

		.u32VDefLevel = 0,  /* not surpport */
		.u32HDefLevel = 0,  /* not surpport */
    },

	/* AD0 */
    {
    	.bKeyAlpha = 1,
		.bGlobalAlpha = 1,
		.bCmap = 0,
		.bHasCmapReg = 0,

		.bColFmt = {1,1,1,1,  1,1,1,1,  1,1,1,1,   1,1,1,1,   1,1,1,1,   1,1,0,0,  0,0,0,0,  0,0,0,0,  0,0}, // 27

		.bVoScale   = HI_TRUE,		
		.bLayerSupported = HI_FALSE,

		.bCompression = 1,
		.bStereo      = 1,
		
		.u32MaxWidth  = 1920,
		.u32MaxHeight = 1080,
		.u32MinWidth  = 0,
		.u32MinHeight = 0,

		.u32VDefLevel = 0,  /* not surpport */
		.u32HDefLevel = 0,  /* not surpport */
    },

    /* AD1 */
    {
    	.bKeyAlpha = 1,
		.bGlobalAlpha = 1,
		.bCmap = 0,
		.bHasCmapReg = 0,

		.bColFmt = {1,1,1,1,  1,1,1,1,  1,1,1,1,   1,1,1,1,   1,1,1,1,   1,1,0,0,  0,0,0,0,  0,0,0,0,  0,0}, // 27

		.bVoScale   = HI_TRUE,		
		.bLayerSupported = HI_FALSE,

		.bCompression = 1,
		.bStereo      = 1,
		
		.u32MaxWidth  = 1920,
		.u32MaxHeight = 1080,
		.u32MinWidth  = 0,
		.u32MinHeight = 0,

		.u32VDefLevel = 0,  /* not surpport */
		.u32HDefLevel = 0,  /* not surpport */
    },

    /* AD2 */
    {
    	.bKeyAlpha = 1,
		.bGlobalAlpha = 1,
		.bCmap = 0,
		.bHasCmapReg = 0,

		.bColFmt = {1,1,1,1,  1,1,1,1,  1,1,1,1,   1,1,1,1,   1,1,1,1,   1,1,0,0,  0,0,0,0,  0,0,0,0,  0,0}, // 27

		.bVoScale   = HI_TRUE,		
		.bLayerSupported = HI_FALSE,

		.bCompression = 1,
		.bStereo      = 1,
		
		.u32MaxWidth  = 1920,
		.u32MaxHeight = 1080,
		.u32MinWidth  = 0,
		.u32MinHeight = 0,

		.u32VDefLevel = 0,  /* not surpport */
		.u32HDefLevel = 0,  /* not surpport */
    },

	/* AD3 */
    {
    	.bKeyAlpha = 1,
		.bGlobalAlpha = 1,
		.bCmap = 0,
		.bHasCmapReg = 0,

		.bColFmt = {1,1,1,1,  1,1,1,1,  1,1,1,1,   1,1,1,1,   1,1,1,1,   1,1,0,0,  0,0,0,0,  0,0,0,0,  0,0}, // 27

		.bVoScale   = HI_TRUE,		
		.bLayerSupported = HI_FALSE,

		.bCompression = 1,
		.bStereo      = 1,
		
		.u32MaxWidth  = 1920,
		.u32MaxHeight = 1080,
		.u32MinWidth  = 0,
		.u32MinHeight = 0,

		.u32VDefLevel = 0,  /* not surpport */
		.u32HDefLevel = 0,  /* not surpport */
    },

	/* CURSOR */
    {
    	.bKeyAlpha = 1,
		.bGlobalAlpha = 1,
		.bCmap = 0,
		.bHasCmapReg = 0,

		.bColFmt = {1,1,1,1,  1,1,1,1,  1,1,1,1,   1,1,1,1,   1,1,1,1,   1,1,0,0,  0,0,0,0,  0,0,0,0,  0,0}, // 27

		.bVoScale   = HI_TRUE,		
		.bLayerSupported = HI_FALSE,

		.bCompression = 1,
		.bStereo      = 1,
		
		.u32MaxWidth  = 1920,
		.u32MaxHeight = 1080,
		.u32MinWidth  = 0,
		.u32MinHeight = 0,

		.u32VDefLevel = 0,  /* not surpport */
		.u32HDefLevel = 0,  /* not surpport */
    },
};

/******************************************************************
                   definitions of global variables
******************************************************************/
static HI_U32 g_u32GFXInitFlag      = 0; //81143540
static HI_U32 g_u32SlvLayerInitFlag = 0; //8114341c
//static HI_U32 g_u32DispInitFlag[OPTM_GFX_GP_BUTT];


/* WORKMODE */
static HIFB_GFX_MODE_EN g_enOptmGfxWorkMode = HIFB_GFX_MODE_NORMAL; //8114338c

/* gfx0,gfx1,gfx2,gfx3,gfx4,gfx5 */
static OPTM_GFX_LAYER_S g_stGfxDevice[OPTM_MAX_LOGIC_HIFB_LAYER]; //81142964 +13*200 = 8114338c

/*graphics process device gp0 and gp1*/
/*gp0: process gfx0,gfx1,gfx2,gfx3*/
/*gp1: process --------gfx4,gfx5*/
static OPTM_GFX_GP_S g_stGfxGPDevice[OPTM_GFX_GP_BUTT]; //81141f64
/*save irq info of each gfx*/
static OPTM_GP_IRQ_S g_stGfxGPIrq[OPTM_GFX_GP_BUTT]; //8114210c
static OPTM_GFX_WBC_S  g_stGfxWbc2; //81143390
//static OPTM_GFX_WBC_S  g_stGfxWbc3;

/*static*/ HI_BOOL Data_81143414; //81143414

#define OPTM_GP0_GFX_COUNT 3
#define OPTM_GP1_GFX_COUNT 2

#define OPTM_GFX_WBC_WIDTH  720
#define OPTM_GFX_WBC_HEIGHT 576

#define OPTM_GFXCLUT_LENGTH 256
#define OPTM_GFXDATA_DEFAULTBYTES 4

#define OPTM_CMAP_SIZE  0x400    /*unit:KB 256*4*/


/******************************************************************
                     macro definitions
******************************************************************/

#define D_OPTM_HIFB_CheckGfxOpen(enLayerId)  \
do{ if (enLayerId >= HIFB_LAYER_ID_BUTT){  \
      HIFB_ERROR("no suppout Gfx%d!\n",enLayerId); \
      return HI_FAILURE;}    \
	if (g_stGfxDevice[enLayerId].bOpened != HI_TRUE){     \
      HIFB_ERROR("Error! Gfx%d not open!\n",enLayerId); \
      return HI_FAILURE;}    \
}while(0)

/**check gp mask flag,return success when it's true, or else continue*/
#define OPTM_CheckGPMask_BYLayerID(u32LayerID)  do\
{\
	if (g_stGfxGPDevice[g_stGfxDevice[u32LayerID].enGPId].bMaskFlag)\
	{\
		return HI_SUCCESS;\
	}\
}while(0)	

#define OPTM_CheckGPMask_BYGPID(enGPId)  do\
{\
	if (g_stGfxGPDevice[enGPId].bMaskFlag)\
	{\
		return HI_SUCCESS;\
	}\
}while(0)


#define IS_MASTER_GP(enGpId) ((g_enOptmGfxWorkMode == HIFB_GFX_MODE_HD_WBC)&&(enGpId == OPTM_GFX_GP_0))
#define IS_SLAVER_GP(enGpId) ((g_enOptmGfxWorkMode == HIFB_GFX_MODE_HD_WBC)&&(enGpId == OPTM_GFX_GP_1))



/******************************************************************
                      function definitions
******************************************************************/
HI_VOID OPTM_GfxWVBCallBack(HI_U32 enLayerId, HI_U32 u32Param1);
HI_VOID OPTM_GfxWaitRRCallBack(HI_U32 enLayerId, HI_U32 u32Param1);
HI_VOID OPTM_Wbc2Isr(HI_VOID* pParam0, HI_VOID *pParam1);
HI_S32 OPTM_GFX_OpenWbc2(OPTM_GFX_WBC_S *pstWbc2);
HI_S32 OPTM_GFX_CloseWbc2(OPTM_GFX_WBC_S *pstWbc2);


/*  in WBC mode, call-back function of switching for SD display system */
HI_VOID OPTM_DispInfoCallbackUnderWbc(HI_U32 u32Param0, HI_U32 u32Param1);
HI_S32 OPTM_SetCallbackToDisp(OPTM_GFX_GP_E enGPId, IntCallBack pCallBack, HI_DRV_DISP_CALLBACK_TYPE_E eType, HI_BOOL bFlag);
HI_VOID OPTM_DispCallBack(HI_VOID* u32Param0, HI_VOID* u32Param1);
HI_VOID OPTM_FrameEndCallBack(HI_VOID* u32Param0, HI_VOID* u32Param1);
HI_S32 OPTM_Distribute_Callback(HI_VOID* u32Param0, HI_VOID* u32Param1);
HI_S32 OPTM_GfxSetSrcFromWbc2(HI_BOOL bFromWbc2);
HI_S32 OPTM_GfxChgCmp2Ncmp(HIFB_LAYER_ID_E enLayerId);
HI_S32 OPTM_GPMask(OPTM_VDP_LAYER_GP_E enGPId, HI_BOOL bFlag);

HI_S32 OPTM_GPRecovery(OPTM_VDP_LAYER_GP_E enGPId);
HI_S32 OPTM_GfxCloseLayer(HIFB_LAYER_ID_E enLayerId);
HI_S32 OPTM_GfxSetDispFMTSize(OPTM_GFX_GP_E enGpId, const HI_RECT_S *pstOutRect);
HI_S32 OPTM_GfxSetLayerDeCmpEnable(HIFB_LAYER_ID_E enLayerId, HI_BOOL bEnable);
HI_S32 OPTM_GfxSetEnable(HIFB_LAYER_ID_E enLayerId, HI_BOOL bEnable);
HI_S32 OPTM_GfxSetGpRect(OPTM_GFX_GP_E enGpId, const HIFB_RECT * pstInputRect);
OPTM_VDP_GFX_IFMT_E OPTM_PixerFmtTransferToHalFmt(HIFB_COLOR_FMT_E enDataFmt);
HI_S32 OPTM_GfxConfigSlvLayer(HIFB_LAYER_ID_E enLayerId, HI_RECT_S *pstRect);
HI_S32 OPTM_GfxSetCsc(OPTM_GFX_GP_E enGfxGpId, OPTM_GFX_CSC_PARA_S *pstCscPara, HI_BOOL bIsBGRIn);
HI_S32 OPTM_GfxOpenSlvLayer(HIFB_LAYER_ID_E enLayerId);
HI_S32 OPTM_AllocAndMap(const char *bufname, char *zone_name, HI_U32 size, int align, MMZ_BUFFER_S *psMBuf);
HI_VOID OPTM_UnmapAndRelease(MMZ_BUFFER_S *psMBuf);
HI_S32 OPTM_Adapt_AllocAndMap(const char *bufname, char *zone_name, HI_U32 size, int align, MMZ_BUFFER_S *psMBuf);
HI_S32 OPTM_GfxSetLayerAddr(HIFB_LAYER_ID_E enLayerId, HI_U32 u32Addr);
HI_S32 OPTM_GfxSetWbcAddr(OPTM_VDP_LAYER_WBC_E enWbcHalId, HI_U32 u32Addr, HI_U32 u32BufferStride);
HI_S32 OPTM_GfxSetLayerStride(HIFB_LAYER_ID_E enLayerId, HI_U32 u32Stride);
OPTM_COLOR_SPACE_E OPTM_AdaptCscTypeFromDisp(HI_DRV_COLOR_SPACE_E enHiDrvCsc);
HI_S32 OPTM_JudgeWbcEnable(HI_VOID);
/*static*/ HI_S32 OPTM_GfxCloseSlvLayer(HIFB_LAYER_ID_E enLayerId);
HI_S32 OPTM_GfxWaitVBlank(HIFB_LAYER_ID_E enLayerId);















/******************************************************************
                   function definitions
******************************************************************/
HI_U32 OPTM_AlignUp(HI_U32 x, HI_U32 a)
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

#ifdef HI_BUILD_IN_BOOT
extern HI_S32 Win_ReviseOutRect(const HI_RECT_S *tmp_virtscreen, 
                         const HI_DRV_DISP_OFFSET_S *stOffsetInfo,
                         const HI_RECT_S *stFmtResolution,
                         const HI_RECT_S *stPixelFmtResolution,
                         HI_RECT_S *stToBeRevisedRect, 
                         HI_RECT_S *stRevisedRect);
#define IO_ADDRESS(addr) (addr)

HIFB_GFX_MODE_EN OPTM_Get_GfxWorkMode(HI_VOID)
{
    return g_enOptmGfxWorkMode;
}

HI_S32 OPTM_GpInitFromDisp(OPTM_GFX_GP_E enGPId)
{
    HI_DRV_DISPLAY_E enDisp;
    HI_DISP_DISPLAY_INFO_S pstInfo;
    OPTM_COLOR_SPACE_E          enGpCsc;

	if (!g_stGfxGPDevice[enGPId].bOpen)
	{
		return HI_FAILURE;
	}
    
    if (enGPId == OPTM_GFX_GP_0)
    {
        enDisp = HI_DRV_DISPLAY_1;
    }
    else
    {
        enDisp = HI_DRV_DISPLAY_0;
    }

	memset(&pstInfo, 0, sizeof(HI_DISP_DISPLAY_INFO_S));
    
    DISP_GetDisplayInfo(enDisp, &pstInfo);
    g_stGfxGPDevice[enGPId].bInterface = pstInfo.bInterlace;
    
    Win_ReviseOutRect(&pstInfo.stVirtaulScreen, &pstInfo.stOffsetInfo, 
                      &pstInfo.stFmtResolution, &pstInfo.stPixelFmtResolution,
                      &pstInfo.stVirtaulScreen, &g_stGfxGPDevice[enGPId].stOutRect);
	
    g_stGfxGPDevice[enGPId].stCscPara.u32Bright     = pstInfo.u32Bright;
    g_stGfxGPDevice[enGPId].stCscPara.u32Contrast   = pstInfo.u32Contrst;
    g_stGfxGPDevice[enGPId].stCscPara.u32Hue        = pstInfo.u32Hue;
	g_stGfxGPDevice[enGPId].stCscPara.u32Kb         = pstInfo.u32Kb;
	g_stGfxGPDevice[enGPId].stCscPara.u32Kg         = pstInfo.u32Kg;
	g_stGfxGPDevice[enGPId].stCscPara.u32Kr         = pstInfo.u32Kr;
    g_stGfxGPDevice[enGPId].stCscPara.u32Saturation = pstInfo.u32Satur;
	

    if (pstInfo.bIsMaster && enGPId == OPTM_GFX_GP_0)
    {
        g_enOptmGfxWorkMode = HIFB_GFX_MODE_HD_WBC;
    }
    
   // Debug("DISP_GetDisplayInfo:GpId=%d,DisplayWidth=%d,DisplayHeight=%d\n",enGPId,pstInfo.stOrgRect.s32Width,pstInfo.stOrgRect.s32Height);
    OPTM_GPRecovery(enGPId);
    
	enGpCsc = OPTM_AdaptCscTypeFromDisp(pstInfo.eColorSpace);
	if (g_stGfxGPDevice[enGPId].enOutputCsc != enGpCsc)
	{
		g_stGfxGPDevice[enGPId].enOutputCsc = enGpCsc;
		OPTM_GfxSetCsc(enGPId, &g_stGfxGPDevice[enGPId].stCscPara, g_stGfxGPDevice[enGPId].bBGRState);
	}	

    return HI_SUCCESS;
}
#endif

HI_S32	OPTM_GFX_GetDevCap(const HIFB_CAPABILITY_S **pstCap)
{
	*pstCap = &g_stGfxCap[0];

	return HI_SUCCESS;
}

OPTM_VDP_LAYER_GFX_E OPTM_GetGfxHalId(HIFB_LAYER_ID_E enLayerId)
{
#if 0
	if (HIFB_LAYER_SD_1 == enLayerId)
	{
		return OPTM_VDP_LAYER_GFX3;
	}
	else if (HIFB_LAYER_SD_1 > enLayerId)
	{
		return (OPTM_VDP_LAYER_GFX_E)enLayerId;
	}
	else
	{
		return OPTM_VDP_LAYER_GFX_BUTT;
	}
#else
	if (enLayerId < 5)
	{
		return (OPTM_VDP_LAYER_GFX_E)enLayerId;
	}
	else
	{
		return 6;
	}
#endif
}

OPTM_COLOR_SPACE_E OPTM_AdaptCscTypeFromDisp(HI_DRV_COLOR_SPACE_E enHiDrvCsc)
{
	switch(enHiDrvCsc)
	{
		case HI_DRV_CS_BT601_YUV_LIMITED:
			return OPTM_CS_BT601_YUV_LIMITED;
		case HI_DRV_CS_BT601_YUV_FULL:
			return OPTM_CS_BT601_YUV_FULL;
		case HI_DRV_CS_BT709_YUV_LIMITED:
			return OPTM_CS_BT709_YUV_LIMITED;
		case HI_DRV_CS_BT709_YUV_FULL:
			return OPTM_CS_BT709_YUV_FULL;
		case HI_DRV_CS_BT709_RGB_FULL:
			return OPTM_CS_BT601_RGB_FULL;
		default:
			return OPTM_CS_BUTT;
	}
}

HIFB_STEREO_MODE_E OPTM_AdaptTriDimModeFromDisp(HI_DRV_DISP_STEREO_E enDispStereo)
{
	switch(enDispStereo)
	{
		case DISP_STEREO_NONE:
			return HIFB_STEREO_MONO;
		case DISP_STEREO_SBS_HALF:
			return HIFB_STEREO_SIDEBYSIDE_HALF;
		case DISP_STEREO_TAB:
			return HIFB_STEREO_TOPANDBOTTOM;
		case DISP_STEREO_FPK:
			return HIFB_STEREO_FRMPACKING;
		default:
			return HIFB_STEREO_BUTT;
	}

	return HIFB_STEREO_BUTT;
}


HI_VOID OPTM_GPDATA_Init(HI_VOID)
{
    memset(&(g_stGfxDevice[0])    , 0, sizeof(OPTM_GFX_LAYER_S)*OPTM_MAX_LOGIC_HIFB_LAYER);
	memset(&(g_stGfxGPDevice[0])  , 0, sizeof(OPTM_GFX_GP_S)*OPTM_GFX_GP_BUTT);
	memset(&(g_stGfxGPIrq[0])     , 0, sizeof(OPTM_GP_IRQ_S)*OPTM_GFX_GP_BUTT);
	memset(&g_stGfxWbc2           , 0, sizeof(OPTM_GFX_WBC_S));
}

/* physical base address of VOU registers' list */
#define OPTM_REGS_BASE_ADDR   0xfa120000

HI_S32 OPTM_Aapt_Module_GetFunction(HI_U32 u32ModuleID, HI_VOID** ppFunc)
{
#ifndef HI_BUILD_IN_BOOT
	if (HI_NULL == ppFunc)
	{
		return HI_FAILURE;
	}
	
	if (HI_SUCCESS != HI_DRV_MODULE_GetFunction(u32ModuleID, ppFunc))
	{
		return HI_FAILURE;
	}
	
	if (HI_NULL == *ppFunc)
	{
		return HI_FAILURE;
	}
	else
	{
		return HI_SUCCESS;
	}
#else
	return HI_SUCCESS;
#endif
}


HI_S32 OPTM_GfxInit(HI_VOID)
{
    HI_S32 s32ret;
	HI_U32 u32Phyaddr;
	
    PRINT_IN;

    if (OPTM_ENABLE == g_u32GFXInitFlag)
    {
        return HI_SUCCESS;
    }

	u32Phyaddr = IO_ADDRESS(OPTM_REGS_BASE_ADDR);
	if (HI_NULL == u32Phyaddr)
	{
		HIFB_ERROR("fail to init hal register!\n");
		return HI_FAILURE;
	}
	
    OPTM_VDP_DRIVER_Initial(u32Phyaddr);
	
	OPTM_GPDATA_Init();
    
    /* TODO: load Gfx zoom coefficients */
    s32ret = OPTM_ALG_GZmeVdpComnInit(&GfxZmeModule);
    if (HI_SUCCESS != s32ret)
    {
        HIFB_ERROR("Malloc Gfxzme coeff buffer failed\n"); //1073
        return s32ret;
    }
	
    g_u32GFXInitFlag = OPTM_ENABLE;
    
    PRINT_OUT;

    return HI_SUCCESS;
}

HI_S32 OPTM_GfxDeInit(HI_VOID)
{
#if 1
#warning TODO
#else
    HI_S32 i;
	
    PRINT_IN;

    if (OPTM_DISABLE == g_u32GFXInitFlag)
    {
        return HI_SUCCESS;
    }

    for (i=HIFB_LAYER_HD_0; i<HIFB_LAYER_ID_BUTT; i++)
    {
        if (g_stGfxDevice[i].bOpened != HI_FALSE)
        {
            OPTM_GfxCloseLayer(i);
        }        
    }
    
    /* TODO: load Gfx zoom coefficients */
    OPTM_ALG_GZmeVdpComnDeInit(&GfxZmeModule);

	if (g_stGfxWbc2.stFrameBuffer.u32StartVirAddr != 0)
	{
		OPTM_UnmapAndRelease(&(g_stGfxWbc2.stFrameBuffer));
		g_stGfxWbc2.stFrameBuffer.u32StartVirAddr = 0;
		g_stGfxWbc2.stFrameBuffer.u32StartPhyAddr = 0;
	}	

    g_u32GFXInitFlag = OPTM_DISABLE;
	ps_DispExportFuncs = HI_NULL;
    
    PRINT_OUT;
#endif

    return HI_SUCCESS;
}

#ifndef HI_BUILD_IN_BOOT
static HI_VOID OPTM_WorkQueueToOpenWbc(struct work_struct *data)
{
	HIFB_LAYER_ID_E u32LayerID;
	OPTM_GFX_WORK_S *pstOpenSlvWork = container_of(data, OPTM_GFX_WORK_S, work); 
	u32LayerID = (HIFB_LAYER_ID_E)(pstOpenSlvWork->u32Data);
	//printk("get data from slv work %d.\n",u32LayerID);
	OPTM_GfxOpenSlvLayer(u32LayerID);
}
/*
#else
static HI_VOID OPTM_WorkQueueToOpenWbc(HI_VOID)
{
	return;
}*/
#endif

#ifndef HI_BUILD_IN_BOOT
static HI_VOID OPTM_3DMode_Callback(struct work_struct *data)
{
	HI_U32						i;
	HI_U32						u32CTypeFlag;
	HI_U32						u32LayerCount;
	OPTM_GFX_GP_E 		        enGpHalId;
	OPTM_GFX_WORK_S             *pst3DModeWork;

	pst3DModeWork = container_of(data, OPTM_GFX_WORK_S, work);
	enGpHalId	  = (OPTM_GFX_GP_E)(pst3DModeWork->u32Data);

	if (enGpHalId >= OPTM_GFX_GP_BUTT)
	{
		return;
	}

	u32LayerCount = (OPTM_VDP_LAYER_GP0 == enGpHalId) ? OPTM_GP0_GFX_COUNT : OPTM_GP1_GFX_COUNT;
	
	for (i = 0; i < u32LayerCount;i++)
	{
		u32CTypeFlag = g_stGfxGPIrq[enGpHalId].stGfxCallBack[i].u32CTypeFlag;
		
		if (!u32CTypeFlag)
		{
			continue;
		}		

		if (u32CTypeFlag & HIFB_CALLBACK_TYPE_3DMode_CHG)
		{
			/*callback function*/
			g_stGfxGPIrq[enGpHalId].stGfxCallBack[i].stGfxIrq[HIFB_CALLBACK_TYPE_3DMode_CHG].pFunc(
				(HI_VOID*)g_stGfxGPIrq[enGpHalId].stGfxCallBack[i].stGfxIrq[HIFB_CALLBACK_TYPE_3DMode_CHG].u32Param0, 
				(HI_VOID*)&g_stGfxGPDevice[enGpHalId].enTriDimMode);			
		}
	}
}
#endif

HI_VOID OPTM_ALG_Init(OPTM_GFX_GP_E enGPId)
{
	OPTM_ALG_GDTI_RTL_PARA_S stDtiRtlPara;

	memset(&stDtiRtlPara, 0, sizeof(OPTM_ALG_GDTI_RTL_PARA_S));

	OPTM_ALG_GDtiInit(HI_NULL, &stDtiRtlPara);

        OPTM_VDP_GP_SetTiHpCoef(enGPId, VDP_TI_MODE_CHM, (HI_S32 *)stDtiRtlPara.s32CTIHPTmp);
	OPTM_VDP_GP_SetTiHpCoef(enGPId, VDP_TI_MODE_LUM, (HI_S32 *)stDtiRtlPara.s32LTIHPTmp);
	
	OPTM_VDP_GP_SetTiGainRatio(enGPId, VDP_TI_MODE_CHM, (HI_S32)stDtiRtlPara.s16CTICompsatRatio);
	OPTM_VDP_GP_SetTiGainRatio(enGPId, VDP_TI_MODE_LUM, (HI_S32)stDtiRtlPara.s16LTICompsatRatio);

	OPTM_VDP_GP_SetTiCoringThd(enGPId, VDP_TI_MODE_CHM, (HI_U32)stDtiRtlPara.u16CTICoringThrsh);
	OPTM_VDP_GP_SetTiCoringThd(enGPId, VDP_TI_MODE_LUM, (HI_U32)stDtiRtlPara.u16LTICoringThrsh);

	OPTM_VDP_GP_SetTiSwingThd(enGPId, VDP_TI_MODE_CHM, (HI_U32)stDtiRtlPara.u16CTIOverSwingThrsh, (HI_U32)stDtiRtlPara.u16CTIUnderSwingThrsh);
	OPTM_VDP_GP_SetTiSwingThd(enGPId, VDP_TI_MODE_LUM, (HI_U32)stDtiRtlPara.u16LTIOverSwingThrsh, (HI_U32)stDtiRtlPara.u16LTIUnderSwingThrsh);

	OPTM_VDP_GP_SetTiMixRatio(enGPId, VDP_TI_MODE_CHM, (HI_U32)stDtiRtlPara.u8CTIMixingRatio);
	OPTM_VDP_GP_SetTiMixRatio(enGPId, VDP_TI_MODE_LUM, (HI_U32)stDtiRtlPara.u8LTIMixingRatio);

	OPTM_VDP_GP_SetTiHfThd(enGPId, VDP_TI_MODE_LUM, (HI_U32 *)stDtiRtlPara.u32LTIHFreqThrsh);
	OPTM_VDP_GP_SetTiGainCoef(enGPId, VDP_TI_MODE_LUM, (HI_U32 *)stDtiRtlPara.u32LTICompsatMuti);
}

/*static*/ HI_S32 OPTM_GPOpen(OPTM_GFX_GP_E enGPId)
{
	HI_U32 i;
	OPTM_VDP_BKG_S     stBkg;
	HI_U32 u32InitLayerID;
	HI_U32 u32MaxLayerCount;

	PRINT_IN;

	if (g_stGfxGPDevice[enGPId].bOpen)
	{
		return HI_SUCCESS;
	}

	if (HI_NULL == ps_DispExportFuncs)
	{
#warning module HI_ID_DISP not yet implemented!
		if(HI_SUCCESS != OPTM_Aapt_Module_GetFunction(HI_ID_DISP, (HI_VOID**)&ps_DispExportFuncs))
	    {
	        HIFB_ERROR("Fail to get disp export functions!\n"); //1241
	        return HI_FAILURE;
	    }
	}

	memset(&g_stGfxGPIrq[enGPId], 0, sizeof(OPTM_GP_IRQ_S));
	if (OPTM_GFX_GP_0 == enGPId)
	{
	#ifndef HI_BUILD_IN_BOOT
		g_stGfxGPDevice[enGPId].queue = create_workqueue(HIFB_WORK_QUEUE);
		if (HI_NULL != g_stGfxGPDevice[enGPId].queue)
		{			 
			INIT_WORK(&g_stGfxGPDevice[enGPId].stOpenSlvWork.work,   OPTM_WorkQueueToOpenWbc);
			INIT_WORK(&g_stGfxGPDevice[enGPId].st3DModeChgWork.work, OPTM_3DMode_Callback);
			HIFB_INFO("create_workqueue success.\n"); //1255
		}
    #endif    
	}	
	
	memset(&stBkg,0,sizeof(OPTM_VDP_BKG_S));
	stBkg.u32BkgA = 0x0;

	g_stGfxGPDevice[enGPId].u32Alpha    = 0xff;
	g_stGfxGPDevice[enGPId].enReadMode  = VDP_RMODE_PROGRESSIVE;
	g_stGfxGPDevice[enGPId].stBkg       = stBkg;
	g_stGfxGPDevice[enGPId].enInputCsc  = OPTM_CS_BT709_RGB_FULL;
	g_stGfxGPDevice[enGPId].enOutputCsc = OPTM_CS_UNKNOWN;
	g_stGfxGPDevice[enGPId].bBGRState   = HI_FALSE;
	/*set recovery true to make sure when gp was open initially,
    	    dispsize will be set to hardware*/
	g_stGfxGPDevice[enGPId].bGpClose          = HI_FALSE;
	g_stGfxGPDevice[enGPId].bRecoveryInNextVT = HI_TRUE;
	g_stGfxGPDevice[enGPId].bDispInitial      = HI_FALSE;
	
	/*0:HIFB_LAYER_HD_0;1:HIFB_LAYER_HD_1;2:HIFB_LAYER_HD_2;3:HIFB_LAYER_HD_3
		0:HIFB_LAYER_SD_0;1:HIFB_LAYER_SD_1*/
	if (OPTM_GFX_GP_0 == enGPId)
	{
		g_stGfxGPDevice[enGPId].enMixg       = VDP_CBM_MIXG0;
		g_stGfxGPDevice[enGPId].enGpHalId    = OPTM_VDP_LAYER_GP0;
		g_stGfxGPDevice[enGPId].enDispCh     = OPTM_DISPCHANNEL_1;

		OPTM_VDP_GP_SetLayerGalpha(enGPId, g_stGfxGPDevice[enGPId].u32Alpha);
		OPTM_VDP_GP_SetReadMode   (enGPId, g_stGfxGPDevice[enGPId].enReadMode);
		OPTM_VDP_CBM_SetMixerBkg  (g_stGfxGPDevice[enGPId].enMixg, g_stGfxGPDevice[enGPId].stBkg);		
		u32InitLayerID   = (HI_U32)HIFB_LAYER_HD_0;
		u32MaxLayerCount = (HI_U32)HIFB_LAYER_HD_3;
	}
	else if (OPTM_GFX_GP_1 == enGPId)
	{
		g_stGfxGPDevice[enGPId].enMixg       = VDP_CBM_MIXG1;
		g_stGfxGPDevice[enGPId].enGpHalId    = OPTM_VDP_LAYER_GP1;
		g_stGfxGPDevice[enGPId].enDispCh     = OPTM_DISPCHANNEL_0;

		OPTM_VDP_GP_SetLayerGalpha(enGPId, g_stGfxGPDevice[enGPId].u32Alpha);
		OPTM_VDP_GP_SetReadMode   (enGPId, g_stGfxGPDevice[enGPId].enReadMode);
		OPTM_VDP_CBM_SetMixerBkg  (g_stGfxGPDevice[enGPId].enMixg, g_stGfxGPDevice[enGPId].stBkg);		
		u32InitLayerID   = (HI_U32)HIFB_LAYER_SD_0;
		u32MaxLayerCount = (HI_U32)HIFB_LAYER_SD_0; //HIFB_LAYER_SD_1;
	}
	else
	{
		return HI_SUCCESS;
	}

	g_stGfxGPDevice[enGPId].u32Prior = 0x0;
	for (i = u32InitLayerID; i <= u32MaxLayerCount; i++)
	{
		g_stGfxDevice[i].u32ZOrder   = i;
		OPTM_VDP_CBM_SetMixerPrio(g_stGfxGPDevice[enGPId].enMixg, i, g_stGfxDevice[i].u32ZOrder);
		g_stGfxDevice[i].enGfxHalId = OPTM_GetGfxHalId(i);
		g_stGfxGPDevice[enGPId].u32Prior |= ((i+1)<<(i*4));
	}

	OPTM_ALG_Init(enGPId);
	
	g_stGfxGPDevice[enGPId].bOpen = HI_TRUE;

	PRINT_OUT;

	return HI_SUCCESS;
}

static HI_S32 OPTM_GPClose(OPTM_GFX_GP_E enGPId)
{
	PRINT_IN;
	if (HI_FALSE == g_stGfxGPDevice[enGPId].bOpen)
	{
		return HI_SUCCESS;
	}
	
	OPTM_SetCallbackToDisp(enGPId, (IntCallBack)OPTM_DispCallBack, HI_DRV_DISP_C_INTPOS_90_PERCENT, HI_FALSE);
#ifndef HI_BUILD_IN_BOOT
    if (g_stGfxGPDevice[enGPId].queue)
	{
		destroy_workqueue(g_stGfxGPDevice[enGPId].queue);
		g_stGfxGPDevice[enGPId].queue = HI_NULL;
	}
#endif	

	g_stGfxGPDevice[enGPId].bOpen         = HI_FALSE;
	g_stGfxGPDevice[enGPId].bGPInInitial  = HI_FALSE;
	g_stGfxGPDevice[enGPId].bGPInSetbyusr = HI_FALSE;
	g_stGfxGPDevice[enGPId].bDispInitial  = HI_FALSE;
	g_stGfxGPDevice[enGPId].bNeedExtractLine = HI_FALSE;
	g_stGfxGPDevice[enGPId].bMaskFlag     = HI_FALSE;
	
	g_enOptmGfxWorkMode = HIFB_GFX_MODE_NORMAL;

	PRINT_OUT;
	return HI_SUCCESS;
}

/*static*/ HI_S32 OPTM_GfxSetLayerReadMode(HIFB_LAYER_ID_E enLayerId, OPTM_VDP_DATA_RMODE_E enReadMode)
{
	OPTM_GFX_GP_E        enGPId;

	enGPId = g_stGfxDevice[enLayerId].enGPId;

	OPTM_VDP_GFX_SetReadMode(g_stGfxDevice[enLayerId].enGfxHalId, enReadMode);
	OPTM_VDP_GP_SetReadMode (g_stGfxGPDevice[enGPId].enGpHalId,   enReadMode);
	
	return HI_SUCCESS;
}

/*static*/ HI_S32 OPTM_GfxInitLayer(HIFB_LAYER_ID_E enLayerId)
{
	OPTM_VDP_BKG_S stBkg;
		
	PRINT_IN;

	init_waitqueue_head(&(g_stGfxDevice[enLayerId].vblEvent));	

	g_stGfxDevice[enLayerId].enGfxHalId = OPTM_GetGfxHalId(enLayerId);
#if 0
	g_stGfxDevice[enLayerId].enGPId     = (g_stGfxDevice[enLayerId].enGfxHalId > OPTM_VDP_LAYER_GFX2) ? OPTM_GFX_GP_1 : OPTM_GFX_GP_0;
#else
	g_stGfxDevice[enLayerId].enGPId     = (g_stGfxDevice[enLayerId].enGfxHalId > OPTM_VDP_LAYER_GFX3) ? OPTM_GFX_GP_1: OPTM_GFX_GP_0;
#endif
	g_stGfxDevice[enLayerId].CscState   = OPTM_CSC_SET_PARA_RGB;

	memset(&stBkg, 0, sizeof(stBkg));
	stBkg.u32BkgA = 0x0;
	g_stGfxDevice[enLayerId].stBkg = stBkg;
	g_stGfxDevice[enLayerId].enBitExtend = VDP_GFX_BITEXTEND_3RD;
	g_stGfxDevice[enLayerId].enReadMode  = VDP_RMODE_PROGRESSIVE;
	g_stGfxDevice[enLayerId].enUpDateMode= VDP_RMODE_PROGRESSIVE;

	OPTM_VDP_GFX_SetNoSecFlag(g_stGfxDevice[enLayerId].enGfxHalId, HI_TRUE); //HI_FALSE);
	OPTM_VDP_GFX_SetDcmpEnable(g_stGfxDevice[enLayerId].enGfxHalId, HI_FALSE);
	
	OPTM_VDP_GFX_SetLayerBkg(g_stGfxDevice[enLayerId].enGfxHalId, g_stGfxDevice[enLayerId].stBkg);
	/*  set bit-extension mode  */
	OPTM_VDP_GFX_SetBitExtend(g_stGfxDevice[enLayerId].enGfxHalId, g_stGfxDevice[enLayerId].enBitExtend);
    /*  set progressive read */
	OPTM_GfxSetLayerReadMode(enLayerId, g_stGfxDevice[enLayerId].enReadMode);

#if 0
	if (OPTM_VDP_LAYER_GFX3 == g_stGfxDevice[enLayerId].enGfxHalId)
	{
		OPTM_VDP_SetLayerConnect(OPTM_VDP_CONNECT_G3_DHD1);
		OPTM_VDP_OpenGFX3(HI_TRUE);
		OPTM_VDP_GFX_SetReadMode(g_stGfxDevice[enLayerId].enGfxHalId, VDP_RMODE_INTERLACE);
	}
#endif
	/*  set the mode of field update */
	OPTM_VDP_GFX_SetUpdMode (g_stGfxDevice[enLayerId].enGfxHalId, g_stGfxDevice[enLayerId].enUpDateMode);
	OPTM_VDP_GFX_SetRegUp   (g_stGfxDevice[enLayerId].enGfxHalId);
    PRINT_OUT;
	
    return HI_SUCCESS;
}

static HI_S32 OPTM_GfxDeInitLayer(HIFB_LAYER_ID_E enLayerId)
{
	PRINT_IN;	
    if (g_stGfxCap[enLayerId].bHasCmapReg != HI_FALSE)
    {
       /*  release CLUT TABLE buffer */
       if (g_stGfxDevice[enLayerId].stCluptTable.u32StartVirAddr != 0)
       {
	        OPTM_UnmapAndRelease(&(g_stGfxDevice[enLayerId].stCluptTable));     	
            g_stGfxDevice[enLayerId].stCluptTable.u32StartVirAddr = 0;
            g_stGfxDevice[enLayerId].stCluptTable.u32StartPhyAddr = 0;
       }
    }

	PRINT_OUT;
	
    return HI_SUCCESS;
}

HI_S32 OPTM_GfxSetCsc(OPTM_GFX_GP_E enGfxGpId, OPTM_GFX_CSC_PARA_S *pstCscPara, HI_BOOL bIsBGRIn)
{ 
#if 1
#warning TODO
#else
	OPTM_ALG_CSC_DRV_PARA_S stCscDrvPara;
	OPTM_ALG_CSC_RTL_PARA_S stCscRtlPara;
	OPTM_VDP_CSC_COEF_S     stVDPCscCoef;
	OPTM_VDP_CSC_DC_COEF_S  stVDPCscDcCoef;
	
    PRINT_IN;

	g_stGfxGPDevice[enGfxGpId].enInputCsc  = OPTM_CS_BT709_RGB_FULL;

	OPTM_CheckGPMask_BYGPID(enGfxGpId);	

	stCscDrvPara.eInputCS   = g_stGfxGPDevice[enGfxGpId].enInputCsc;
	stCscDrvPara.eOutputCS  = g_stGfxGPDevice[enGfxGpId].enOutputCsc;
	stCscDrvPara.bIsBGRIn   = bIsBGRIn;
	
	stCscDrvPara.u32Bright  = pstCscPara->u32Bright;
	stCscDrvPara.u32Contrst = pstCscPara->u32Contrast;
	stCscDrvPara.u32Hue     = pstCscPara->u32Hue;
	stCscDrvPara.u32Kb      = pstCscPara->u32Kb;
	stCscDrvPara.u32Kg      = pstCscPara->u32Kg;
	stCscDrvPara.u32Kr      = pstCscPara->u32Kr;
	stCscDrvPara.u32Satur   = pstCscPara->u32Saturation;

	HIFB_INFO("<<<<<gpid %d csc in %d, out%d, BGR %d.\n",enGfxGpId,stCscDrvPara.eInputCS,stCscDrvPara.eOutputCS,stCscDrvPara.bIsBGRIn);
	HIFB_INFO("parameter %d,%d,%d,%d,%d,%d,%d\n>>>",pstCscPara->u32Bright,pstCscPara->u32Contrast,pstCscPara->u32Hue,
				pstCscPara->u32Kb,pstCscPara->u32Kg,pstCscPara->u32Kr,pstCscPara->u32Saturation);
	OPTM_ALG_CscCoefSet(&stCscDrvPara, &stCscRtlPara);

	stVDPCscCoef.csc_coef00 = stCscRtlPara.s32CscCoef_00;
	stVDPCscCoef.csc_coef01 = stCscRtlPara.s32CscCoef_01;
	stVDPCscCoef.csc_coef02 = stCscRtlPara.s32CscCoef_02;
	stVDPCscCoef.csc_coef10 = stCscRtlPara.s32CscCoef_10;
	stVDPCscCoef.csc_coef11 = stCscRtlPara.s32CscCoef_11;
	stVDPCscCoef.csc_coef12 = stCscRtlPara.s32CscCoef_12;
	stVDPCscCoef.csc_coef20 = stCscRtlPara.s32CscCoef_20;
	stVDPCscCoef.csc_coef21 = stCscRtlPara.s32CscCoef_21;
	stVDPCscCoef.csc_coef22 = stCscRtlPara.s32CscCoef_22;

	stVDPCscDcCoef.csc_in_dc0 = stCscRtlPara.s32CscDcIn_0;
	stVDPCscDcCoef.csc_in_dc1 = stCscRtlPara.s32CscDcIn_1;
	stVDPCscDcCoef.csc_in_dc2 = stCscRtlPara.s32CscDcIn_2;
	
	stVDPCscDcCoef.csc_out_dc0 = stCscRtlPara.s32CscDcOut_0;
	stVDPCscDcCoef.csc_out_dc1 = stCscRtlPara.s32CscDcOut_1;
	stVDPCscDcCoef.csc_out_dc2 = stCscRtlPara.s32CscDcOut_2;
	
	OPTM_VDP_GP_SetCscCoef  (g_stGfxGPDevice[enGfxGpId].enGpHalId, stVDPCscCoef);	
	OPTM_VDP_GP_SetCscDcCoef(g_stGfxGPDevice[enGfxGpId].enGpHalId, stVDPCscDcCoef);
	OPTM_VDP_GP_SetCscEnable(g_stGfxGPDevice[enGfxGpId].enGpHalId, HI_TRUE);

	/*** current CSC mode is RGB ***/
    //g_stGfxDevice[enLayerId].CscState = OPTM_CSC_SET_PARA_RGB;
	
    PRINT_OUT;
#endif
 
    return HI_SUCCESS;
}

#ifndef HI_BUILD_IN_BOOT
static HIFB_LAYER_ID_E CallBackLayerId[HIFB_LAYER_ID_BUTT];
#endif
HI_S32 OPTM_GfxSetCallback(HIFB_LAYER_ID_E enLayerId, IntCallBack pCallBack, HIFB_CALLBACK_TPYE_E eCallbackType)

{
#ifndef HI_BUILD_IN_BOOT

	HI_U32                      u32GfxIndex;
	OPTM_GFX_GP_E               enGPId;

    PRINT_IN;	
    
    if (eCallbackType >= HIFB_CALLBACK_TYPE_BUTT)
    {
    	HIFB_ERROR("Fail to set callback func!\n");
        return HI_FAILURE;
    }    

	/***back up layer's id in the global array***/
	CallBackLayerId[enLayerId] = enLayerId;
	
	enGPId = g_stGfxDevice[enLayerId].enGPId;
	u32GfxIndex = (enLayerId > HIFB_LAYER_HD_3) ? (enLayerId - HIFB_LAYER_HD_3 - 1) : enLayerId;

	if (u32GfxIndex >= OPTM_GP_MAXGFXCOUNT)
	{
		HIFB_ERROR("Fail to set callback func!\n");
		return HI_FAILURE;
	}

	if (HI_NULL != pCallBack)
	{
		g_stGfxGPIrq[enGPId].stGfxCallBack[u32GfxIndex].u32CTypeFlag |= eCallbackType;
		g_stGfxGPIrq[enGPId].stGfxCallBack[u32GfxIndex].stGfxIrq[eCallbackType].pFunc     = pCallBack;
		g_stGfxGPIrq[enGPId].stGfxCallBack[u32GfxIndex].stGfxIrq[eCallbackType].u32Param0 = (HI_U32)&CallBackLayerId[enLayerId];	    
	}
	else
	{
		g_stGfxGPIrq[enGPId].stGfxCallBack[u32GfxIndex].u32CTypeFlag &= ~((HI_U32)eCallbackType);
		g_stGfxGPIrq[enGPId].stGfxCallBack[u32GfxIndex].stGfxIrq[eCallbackType].pFunc     = HI_NULL;
	}

    PRINT_OUT;

    return HI_SUCCESS;
#else
	return HI_SUCCESS;
#endif
}

/*Open  sd0 layer when working int wbc mode*/
HI_S32 OPTM_GfxOpenSlvLayer(HIFB_LAYER_ID_E enLayerId)
{
	HI_S32        s32Ret;
	OPTM_GFX_GP_E enGPId;
	PRINT_IN;

	if (OPTM_ENABLE == g_u32SlvLayerInitFlag)
	{
		return HI_SUCCESS;
	}

	if (HIFB_GFX_MODE_NORMAL == g_enOptmGfxWorkMode)
	{
		return HI_SUCCESS;
	}

	/*  1 open WBC2 */
	s32Ret = OPTM_GFX_OpenWbc2(&g_stGfxWbc2);
	if (s32Ret != HI_SUCCESS)
	{
		HIFB_ERROR("Fail to open Wbc2!\n"); //1668
		goto ERR;
	}

	/*  2 synchronization of G1 */
	s32Ret = OPTM_GfxInitLayer(enLayerId);
	if (s32Ret != HI_SUCCESS)
	{
		HIFB_ERROR("failed to init slvLayer gfx%d!\n",enLayerId); //1676
		goto ERR;
	}      

	enGPId  = g_stGfxDevice[enLayerId].enGPId;

	s32Ret = OPTM_GPOpen(enGPId);
	if (HI_SUCCESS != s32Ret)
	{
		return HI_FAILURE;
	}

	/*  3 set slv layer */  
	g_stGfxDevice[enLayerId].enReadMode = VDP_RMODE_INTERLACE;
	OPTM_VDP_GFX_SetInDataFmt  (g_stGfxDevice[enLayerId].enGfxHalId, OPTM_PixerFmtTransferToHalFmt(g_stGfxWbc2.enDataFmt));
	OPTM_GfxSetLayerReadMode   (enLayerId, g_stGfxDevice[enLayerId].enReadMode);
	OPTM_VDP_GFX_SetPalpha   (g_stGfxDevice[enLayerId].enGfxHalId, HI_TRUE,HI_TRUE,0xff,0xff);
	OPTM_VDP_GFX_SetPreMultEnable(g_stGfxDevice[enLayerId].enGfxHalId, HI_TRUE);
	OPTM_VDP_GFX_SetRegUp      (g_stGfxDevice[enLayerId].enGfxHalId);	

	g_stGfxDevice[enLayerId].bCampEnable = HI_FALSE;	
	 
	s32Ret = OPTM_SetCallbackToDisp(enGPId, (IntCallBack)OPTM_DispCallBack, HI_DRV_DISP_C_INTPOS_90_PERCENT, HI_TRUE);
	if (HI_SUCCESS != s32Ret)
	{
		HIFB_ERROR("unable to register to disp for slv layer!\n"); //1701
		goto ERR;
	}


	if (g_stGfxCap[enLayerId].bHasCmapReg != HI_FALSE)
    {
    	HI_CHAR name[32];
		snprintf(name, sizeof(name), "HIFB_Fb%d_Clut", enLayerId);
        /*  apply clut table buffer */   
        if (OPTM_Adapt_AllocAndMap(name, HI_NULL, OPTM_CMAP_SIZE, 0, &g_stGfxDevice[enLayerId].stCluptTable) != HI_SUCCESS)
        {
            HIFB_ERROR("GFX Get clut buffer failed!\n"); //1713
            goto ERR;
        }		

        OPTM_VDP_GFX_SetLutAddr(g_stGfxDevice[enLayerId].enGfxHalId, g_stGfxDevice[enLayerId].stCluptTable.u32StartPhyAddr);
    }	

	//OPTM_GfxSetLayerAddr  (enLayerId, g_stGfxWbc2.stFrameBuffer.u32StartPhyAddr);
	OPTM_GfxSetWbcAddr(g_stGfxWbc2.enWbcHalId, g_stGfxWbc2.Data_36, g_stGfxWbc2.u32BufferStride);
	OPTM_GfxSetLayerStride(enLayerId, g_stGfxWbc2.u32BufferStride);

	g_stGfxDevice[enLayerId].bEnable = OPTM_JudgeWbcEnable();
	g_stGfxGPDevice[OPTM_SLAVER_GPID].unUpFlag.bits.Enable = 1;
	g_stGfxGPDevice[OPTM_SLAVER_GPID].unUpFlag.bits.RegUp  = 1;

	g_stGfxDevice[enLayerId].bOpened     = HI_TRUE;
	g_u32SlvLayerInitFlag = OPTM_ENABLE;

	/*maybe master layer was closed, when we starting work queue to open slv layer*/
	if (HIFB_GFX_MODE_NORMAL == g_enOptmGfxWorkMode)
	{
		goto ERR;
	}

	PRINT_OUT;

	return s32Ret;
ERR:
	OPTM_GfxCloseSlvLayer(enLayerId);
	g_stGfxDevice[enLayerId].bOpened               = HI_FALSE;
	g_u32SlvLayerInitFlag = OPTM_DISABLE;
	return HI_FAILURE;
}

/*Close  sd0 layer when working int wbc mode*/
/*static*/ HI_S32 OPTM_GfxCloseSlvLayer(HIFB_LAYER_ID_E enLayerId)
{
	HI_S32        s32Ret;
	OPTM_GFX_GP_E enGPId;
	PRINT_IN;

	s32Ret = HI_SUCCESS;
	
	if (OPTM_DISABLE== g_u32SlvLayerInitFlag)
	{
		return HI_SUCCESS;
	}

	/*unregister slavery layer's callback function*/	
	enGPId  = g_stGfxDevice[enLayerId].enGPId;

	/* close slavery layer, confirm hardware close */
    OPTM_VDP_GFX_SetLayerEnable(g_stGfxDevice[enLayerId].enGfxHalId, HI_FALSE);
    OPTM_VDP_GFX_SetRegUp (g_stGfxDevice[enLayerId].enGfxHalId);
	
#if 0
	OPTM_VDP_WBC_SetEnable(g_stGfxWbc2.enWbcHalId, HI_FALSE);
	OPTM_VDP_WBC_SetRegUp (g_stGfxWbc2.enWbcHalId);

	/*wait v blank to make sure hardware closed*/
	OPTM_GfxWaitVBlank(enLayerId);
#endif
	
//#ifndef HI_BUILD_IN_BOOT    
	//s32Ret |= OPTM_GfxSetCallback(enLayerId,HI_NULL,HIFB_CALLBACK_TYPE_VO);
	s32Ret |= OPTM_SetCallbackToDisp(enGPId, (IntCallBack)OPTM_DispCallBack, HI_DRV_DISP_C_INTPOS_90_PERCENT, HI_FALSE);
//#endif
	/*close wbc2*/
	s32Ret |= OPTM_GFX_CloseWbc2(&g_stGfxWbc2);

	/*release resource applied for slavery layer*/
    s32Ret |= OPTM_GfxDeInitLayer(enLayerId);	

	g_stGfxDevice[enLayerId].bExtractLine = HI_FALSE;
	g_stGfxDevice[enLayerId].bOpened      = HI_FALSE;

	g_u32SlvLayerInitFlag = OPTM_DISABLE;

	PRINT_OUT;

	return s32Ret;
}


/*this func is for both graphics and video  virtual screen deal, it's a common function.*/
HI_S32 OPTM_ReviseOutRect(const HI_RECT_S *tmp_virtscreen, 
                         const OPTM_GFX_OFFSET_S *stOffsetInfo,
                         const HI_RECT_S *stFmtResolution,
                         const HI_RECT_S *stPixelFmtResolution,
                         HI_RECT_S *stToBeRevisedRect, 
                         HI_RECT_S *stRevisedRect)
{    
    HI_U32 width_ratio = 0,  height_ratio = 0;
    HI_U32 zmeDestWidth = 0, zmeDestHeight = 0;
    OPTM_GFX_OFFSET_S tmp_offsetInfo;

    tmp_offsetInfo = *stOffsetInfo;

	if (tmp_virtscreen->s32Width == 0 || tmp_virtscreen->s32Height == 0)
	{
		return HI_FAILURE;
	}

    if ( (stFmtResolution->s32Width * 2) == stPixelFmtResolution->s32Width)
    {
        tmp_offsetInfo.u32Left  *= 2;
        tmp_offsetInfo.u32Right *= 2;        
    }

    zmeDestWidth = (stPixelFmtResolution->s32Width - tmp_offsetInfo.u32Left - tmp_offsetInfo.u32Right);
    zmeDestHeight = (stPixelFmtResolution->s32Height - tmp_offsetInfo.u32Top - tmp_offsetInfo.u32Bottom);
    /*pay attention ,we must care about that  u32 overflow.....*/
    width_ratio  = zmeDestWidth  * 100 /(tmp_virtscreen->s32Width);
    height_ratio = zmeDestHeight * 100 /(tmp_virtscreen->s32Height);
   
    stRevisedRect->s32X = (stToBeRevisedRect->s32X * width_ratio) /100 + tmp_offsetInfo.u32Left;
    stRevisedRect->s32Y= (stToBeRevisedRect->s32Y * height_ratio) /100 + tmp_offsetInfo.u32Top;
    
    stRevisedRect->s32Width= (stToBeRevisedRect->s32Width * width_ratio) / 100;
    stRevisedRect->s32Height= (stToBeRevisedRect->s32Height * height_ratio) / 100;  

    stRevisedRect->s32X = OPTM_AlignUp(stRevisedRect->s32X, 2);
    stRevisedRect->s32Y = OPTM_AlignUp(stRevisedRect->s32Y, 2);
    stRevisedRect->s32Width  = OPTM_AlignUp(stRevisedRect->s32Width, 2);
    stRevisedRect->s32Height = OPTM_AlignUp(stRevisedRect->s32Height, 2);

    return HI_SUCCESS;
}

HI_S32 OPTM_GetScreenRectFromDispInfo(const HI_RECT_S *tmp_virtscreen, 
                         const OPTM_GFX_OFFSET_S *stOffsetInfo,
                         const HI_RECT_S *stFmtResolution,
                         const HI_RECT_S *stPixelFmtResolution,
                         HI_RECT_S *stScreenRect)
{    
    OPTM_GFX_OFFSET_S tmp_offsetInfo;

    tmp_offsetInfo = *stOffsetInfo;

	if (tmp_virtscreen->s32Width == 0 || tmp_virtscreen->s32Height == 0)
	{
		return HI_FAILURE;
	}

    if ( (stFmtResolution->s32Width * 2) == stPixelFmtResolution->s32Width)
    {
        tmp_offsetInfo.u32Left  *= 2;
        tmp_offsetInfo.u32Right *= 2;        
    }
   
    stScreenRect->s32X = tmp_offsetInfo.u32Left;
    stScreenRect->s32Y = tmp_offsetInfo.u32Top;
    
    stScreenRect->s32Width= (stPixelFmtResolution->s32Width - tmp_offsetInfo.u32Left - tmp_offsetInfo.u32Right);
    stScreenRect->s32Height= (stPixelFmtResolution->s32Height - tmp_offsetInfo.u32Top - tmp_offsetInfo.u32Bottom);  

    stScreenRect->s32X = OPTM_AlignUp(stScreenRect->s32X, 2);
    stScreenRect->s32Y = OPTM_AlignUp(stScreenRect->s32Y, 2);
    stScreenRect->s32Width  = OPTM_AlignUp(stScreenRect->s32Width, 2);
    stScreenRect->s32Height = OPTM_AlignUp(stScreenRect->s32Height, 2);

    return HI_SUCCESS;
}

HI_VOID OPTM_DispCallBack(HI_VOID* u32Param0, HI_VOID* u32Param1)
{
#ifndef HI_BUILD_IN_BOOT
	OPTM_COLOR_SPACE_E          enGpCsc;
	OPTM_VDP_LAYER_GP_E         *pEnGpHalId;
	HI_DRV_DISP_CALLBACK_INFO_S *pstDispInfo;

	pEnGpHalId  = (OPTM_VDP_LAYER_GP_E *)u32Param0;
	pstDispInfo = (HI_DRV_DISP_CALLBACK_INFO_S *)u32Param1;

	if (HI_NULL == pEnGpHalId || HI_NULL == pstDispInfo)
	{
		HIFB_WARNING("unable to handle null point in dispcallback\n");
		return;
	}
	
	if (g_enOptmGfxWorkMode == HIFB_GFX_MODE_NORMAL
		&& pstDispInfo->stDispInfo.bIsMaster
		&& OPTM_VDP_LAYER_GP0 == *pEnGpHalId)
	{
		g_enOptmGfxWorkMode = HIFB_GFX_MODE_HD_WBC;
		
		if (g_stGfxGPDevice[*pEnGpHalId].queue)
		{
			g_stGfxGPDevice[*pEnGpHalId].stOpenSlvWork.u32Data = HIFB_LAYER_SD_0;
			queue_work(g_stGfxGPDevice[*pEnGpHalId].queue, &g_stGfxGPDevice[*pEnGpHalId].stOpenSlvWork.work);
		}
		
		HIFB_INFO("set gfx work mode wbc!\n");
	}

	if (HI_DRV_DISP_C_PREPARE_TO_PEND == pstDispInfo->eEventType
		|| HI_DRV_DISP_C_PREPARE_CLOSE == pstDispInfo->eEventType)
	{		
		/**when disp format changed or ready to suspend, 
			we should diable layer and save layer data **/
		if (!g_stGfxGPDevice[*pEnGpHalId].bDispInitial)
		{
			return;
		}
		
		g_stGfxGPDevice[*pEnGpHalId].bGpClose = HI_TRUE;
        OPTM_GPMask(*pEnGpHalId, HI_TRUE);
		HIFB_INFO("======disp%d close======\n",*pEnGpHalId);
		return;
	}
	else if (HI_DRV_DISP_C_RESUME == pstDispInfo->eEventType
			|| HI_DRV_DISP_C_OPEN == pstDispInfo->eEventType)
	{	
		/*accept disp callback info first time , no need to handle
		    close&open event, disp size info would be saved automatic*/
		if (!g_stGfxGPDevice[*pEnGpHalId].bDispInitial)
		{
			return;
		}
				
		if (HI_TRUE == g_stGfxGPDevice[*pEnGpHalId].bGpClose)
		{	
			g_stGfxGPDevice[*pEnGpHalId].bRecoveryInNextVT = HI_TRUE;
			g_stGfxGPDevice[*pEnGpHalId].bGpClose          = HI_FALSE;
		}
		
		OPTM_GPMask(*pEnGpHalId, HI_FALSE);
		HIFB_INFO("======disp%d open======\n",*pEnGpHalId);
		return;
	}
	else if (HI_DRV_DISP_C_DISPLAY_SETTING_CHANGE == pstDispInfo->eEventType)
	{
		g_stGfxGPDevice[*pEnGpHalId].stCscPara.u32Bright     = pstDispInfo->stDispInfo.u32Bright;
		g_stGfxGPDevice[*pEnGpHalId].stCscPara.u32Contrast   = pstDispInfo->stDispInfo.u32Contrst;
		g_stGfxGPDevice[*pEnGpHalId].stCscPara.u32Hue        = pstDispInfo->stDispInfo.u32Hue;
		g_stGfxGPDevice[*pEnGpHalId].stCscPara.u32Kb         = pstDispInfo->stDispInfo.u32Kb;
		g_stGfxGPDevice[*pEnGpHalId].stCscPara.u32Kg         = pstDispInfo->stDispInfo.u32Kg;
		g_stGfxGPDevice[*pEnGpHalId].stCscPara.u32Kr         = pstDispInfo->stDispInfo.u32Kr;
		g_stGfxGPDevice[*pEnGpHalId].stCscPara.u32Saturation = pstDispInfo->stDispInfo.u32Satur;

		HIFB_INFO("====u32Bright %d,u32Contrast %d,u32Hue %d,u32Kb %d,u32Kg %d,u32Kr %d,u32Saturation %d !===\n"
				,pstDispInfo->stDispInfo.u32Bright,pstDispInfo->stDispInfo.u32Contrst,pstDispInfo->stDispInfo.u32Hue
				,pstDispInfo->stDispInfo.u32Kb,pstDispInfo->stDispInfo.u32Kg,pstDispInfo->stDispInfo.u32Kr
				,pstDispInfo->stDispInfo.u32Satur);
			
		
		OPTM_GfxSetCsc(*pEnGpHalId, &g_stGfxGPDevice[*pEnGpHalId].stCscPara, 
						g_stGfxGPDevice[*pEnGpHalId].bBGRState);
	
		memcpy(&g_stGfxGPDevice[*pEnGpHalId].stInRect, &pstDispInfo->stDispInfo.stVirtaulScreen, sizeof(HI_RECT_S));
		
		OPTM_GetScreenRectFromDispInfo(&pstDispInfo->stDispInfo.stVirtaulScreen, (OPTM_GFX_OFFSET_S *)&pstDispInfo->stDispInfo.stOffsetInfo,
						&pstDispInfo->stDispInfo.stFmtResolution,&pstDispInfo->stDispInfo.stPixelFmtResolution,
						&g_stGfxGPDevice[*pEnGpHalId].stOutRect);	

#if 0
		printk("===vir_screen(%d,%d,%d,%d),offset(%d,%d,%d,%d),FMT(%d,%d,%d,%d),PixFMt(%d,%d,%d,%d),OUT_RECT(%d,%d,%d,%d)\n",
				pstDispInfo->stDispInfo.stVirtaulScreen.s32X,pstDispInfo->stDispInfo.stVirtaulScreen.s32Y,
				pstDispInfo->stDispInfo.stVirtaulScreen.s32Width,pstDispInfo->stDispInfo.stVirtaulScreen.s32Height,
				pstDispInfo->stDispInfo.stOffsetInfo.u32Top,pstDispInfo->stDispInfo.stOffsetInfo.u32Bottom,
				pstDispInfo->stDispInfo.stOffsetInfo.u32Left,pstDispInfo->stDispInfo.stOffsetInfo.u32Right,
				pstDispInfo->stDispInfo.stFmtResolution.s32X,pstDispInfo->stDispInfo.stFmtResolution.s32Y,
				pstDispInfo->stDispInfo.stFmtResolution.s32Width,pstDispInfo->stDispInfo.stFmtResolution.s32Height,
				pstDispInfo->stDispInfo.stPixelFmtResolution.s32X,pstDispInfo->stDispInfo.stPixelFmtResolution.s32Y,
				pstDispInfo->stDispInfo.stPixelFmtResolution.s32Width,pstDispInfo->stDispInfo.stPixelFmtResolution.s32Height,
				g_stGfxGPDevice[*pEnGpHalId].stOutRect.s32X,g_stGfxGPDevice[*pEnGpHalId].stOutRect.s32Y,
				g_stGfxGPDevice[*pEnGpHalId].stOutRect.s32Width,g_stGfxGPDevice[*pEnGpHalId].stOutRect.s32Height);
#endif
		OPTM_GPRecovery(*pEnGpHalId);

		//printk("====DISP_SETTING_EVENT_END====\n");
		return;
	}
	else if (HI_DRV_DISP_C_VT_INT == pstDispInfo->eEventType)
	{
		/*ensure Gp would be opened when disp changed, even without disp open event*/	
		if (HI_TRUE == g_stGfxGPDevice[*pEnGpHalId].bGpClose
			&& HI_TRUE ==g_stGfxGPDevice[*pEnGpHalId].bDispInitial)
		{	
			g_stGfxGPDevice[*pEnGpHalId].bRecoveryInNextVT = HI_TRUE;
			OPTM_GPMask(*pEnGpHalId, HI_FALSE);
			g_stGfxGPDevice[*pEnGpHalId].bGpClose          = HI_FALSE;
		}

		/*
		    1: when recovery flag was true, config each gfx in this VT.
		    2: distribute VT_INT signal to each gfx.
		*/						
		if (HI_TRUE == g_stGfxGPDevice[*pEnGpHalId].bRecoveryInNextVT)
		{		
			g_stGfxGPDevice[*pEnGpHalId].bInterface = pstDispInfo->stDispInfo.bInterlace;
			HIFB_INFO("disp call back bInterface %d.\n", g_stGfxGPDevice[*pEnGpHalId].bInterface);
			
			memcpy(&g_stGfxGPDevice[*pEnGpHalId].stInRect, &pstDispInfo->stDispInfo.stVirtaulScreen, sizeof(HI_RECT_S));
			
			OPTM_GetScreenRectFromDispInfo(&pstDispInfo->stDispInfo.stVirtaulScreen, (OPTM_GFX_OFFSET_S *)&pstDispInfo->stDispInfo.stOffsetInfo,
						&pstDispInfo->stDispInfo.stFmtResolution,&pstDispInfo->stDispInfo.stPixelFmtResolution,
						&g_stGfxGPDevice[*pEnGpHalId].stOutRect);		

			//HIFB_INFO("disp change gpid %d, w %d, h %d.\n",*pEnGpHalId,pstDispRect->s32Width,pstDispRect->s32Height);
#if 0
			printk("===DISP_CHANGE_START===\n");
			printk("===vir_screen(%d,%d,%d,%d),offset(%d,%d,%d,%d),FMT(%d,%d,%d,%d),PixFMt(%d,%d,%d,%d),OUT_RECT(%d,%d,%d,%d)\n",
			pstDispInfo->stDispInfo.stVirtaulScreen.s32X,pstDispInfo->stDispInfo.stVirtaulScreen.s32Y,
			pstDispInfo->stDispInfo.stVirtaulScreen.s32Width,pstDispInfo->stDispInfo.stVirtaulScreen.s32Height,
			pstDispInfo->stDispInfo.stOffsetInfo.u32Top,pstDispInfo->stDispInfo.stOffsetInfo.u32Bottom,
			pstDispInfo->stDispInfo.stOffsetInfo.u32Left,pstDispInfo->stDispInfo.stOffsetInfo.u32Right,
			pstDispInfo->stDispInfo.stFmtResolution.s32X,pstDispInfo->stDispInfo.stFmtResolution.s32Y,
			pstDispInfo->stDispInfo.stFmtResolution.s32Width,pstDispInfo->stDispInfo.stFmtResolution.s32Height,
			pstDispInfo->stDispInfo.stPixelFmtResolution.s32X,pstDispInfo->stDispInfo.stPixelFmtResolution.s32Y,
			pstDispInfo->stDispInfo.stPixelFmtResolution.s32Width,pstDispInfo->stDispInfo.stPixelFmtResolution.s32Height,
			g_stGfxGPDevice[*pEnGpHalId].stOutRect.s32X,g_stGfxGPDevice[*pEnGpHalId].stOutRect.s32Y,
			g_stGfxGPDevice[*pEnGpHalId].stOutRect.s32Width,g_stGfxGPDevice[*pEnGpHalId].stOutRect.s32Height);
#endif
			OPTM_GPRecovery(*pEnGpHalId);
			
			//printk("===DISP_CHANGE_END===\n");
			
			g_stGfxGPDevice[*pEnGpHalId].bRecoveryInNextVT = HI_FALSE;
			g_stGfxGPDevice[*pEnGpHalId].bDispInitial      = HI_TRUE;
		}

		enGpCsc = OPTM_AdaptCscTypeFromDisp(pstDispInfo->stDispInfo.eColorSpace);		
		if (g_stGfxGPDevice[*pEnGpHalId].enOutputCsc != enGpCsc)
		{
			g_stGfxGPDevice[*pEnGpHalId].enOutputCsc = enGpCsc;
			g_stGfxGPDevice[*pEnGpHalId].stCscPara.u32Bright     = pstDispInfo->stDispInfo.u32Bright;
    		g_stGfxGPDevice[*pEnGpHalId].stCscPara.u32Contrast   = pstDispInfo->stDispInfo.u32Contrst;
    		g_stGfxGPDevice[*pEnGpHalId].stCscPara.u32Hue        = pstDispInfo->stDispInfo.u32Hue;
    		g_stGfxGPDevice[*pEnGpHalId].stCscPara.u32Kb         = pstDispInfo->stDispInfo.u32Kb;
    		g_stGfxGPDevice[*pEnGpHalId].stCscPara.u32Kg         = pstDispInfo->stDispInfo.u32Kg;
    		g_stGfxGPDevice[*pEnGpHalId].stCscPara.u32Kr         = pstDispInfo->stDispInfo.u32Kr;
    		g_stGfxGPDevice[*pEnGpHalId].stCscPara.u32Saturation = pstDispInfo->stDispInfo.u32Satur;
			OPTM_GfxSetCsc(*pEnGpHalId, &g_stGfxGPDevice[*pEnGpHalId].stCscPara, 
							g_stGfxGPDevice[*pEnGpHalId].bBGRState);
		}		

		if (IS_SLAVER_GP(*pEnGpHalId))
		{
			OPTM_Wbc2Isr(u32Param0, u32Param1);
		}
		else
		{
			OPTM_Distribute_Callback(u32Param0, u32Param1);
		}		

		return;
	}

	return;
#else
	return;
#endif
}

HI_VOID OPTM_FrameEndCallBack(HI_VOID* u32Param0, HI_VOID* u32Param1)
{
#warning TODO: OPTM_FrameEndCallBack
	printk("OPTM_FrameEndCallBack: TODO\n");
}

HI_DRV_DISPLAY_E OPTM_GfxChn2DispChn(OPTM_DISPCHANNEL_E enDispCh)
{
	if (OPTM_DISPCHANNEL_0 == enDispCh)
	{
		return HI_DRV_DISPLAY_0;
	}
	else if (OPTM_DISPCHANNEL_1 == enDispCh)
	{
		return HI_DRV_DISPLAY_1;
	}
	else
	{
		return HI_DRV_DISPLAY_BUTT;
	}
}

HI_S32 OPTM_SetCallbackToDisp(OPTM_GFX_GP_E enGPId, IntCallBack pCallBack, HI_DRV_DISP_CALLBACK_TYPE_E eType, HI_BOOL bFlag)
{
#ifndef HI_BUILD_IN_BOOT	
	HI_S32 s32Ret;
	HI_DRV_DISPLAY_E            enDisp;
	HI_DRV_DISP_CALLBACK_S      stCallback;
	
    PRINT_IN;
	
	if (bFlag == g_stGfxGPIrq[enGPId].bRegistered[eType])
	{
		return HI_SUCCESS;
	}
    
    if (eType >= HI_DRV_DISP_C_TYPE_BUTT)
    {
    	HIFB_ERROR("Fail to set callback func!\n"); //2149
        return HI_FAILURE;
    }    

	if (HI_NULL == pCallBack)
	{
		HIFB_ERROR("Unable to handle the null func point!\n"); //2155
		return HI_FAILURE;
	}
	
	s32Ret = HI_FAILURE;
	enDisp = OPTM_GfxChn2DispChn(g_stGfxGPDevice[enGPId].enDispCh);
	
	stCallback.hDst            = (HI_HANDLE)(&g_stGfxGPDevice[enGPId].enGpHalId);
	stCallback.pfDISP_Callback = (HI_VOID*)pCallBack;

	if (bFlag)
	{
		s32Ret = ps_DispExportFuncs->pfnDispRegCallback(enDisp,eType,&stCallback);
	}		
	else
	{
		s32Ret = ps_DispExportFuncs->pfnDispUnRegCallback(enDisp,eType,&stCallback);
	}

	if (HI_SUCCESS == s32Ret)
	{
		g_stGfxGPIrq[enGPId].bRegistered[eType] = bFlag;
	}
    
    PRINT_OUT;

    return s32Ret;
#else	
	return HI_SUCCESS;
#endif
}

HI_S32 OPTM_GfxOpenLayer(HIFB_LAYER_ID_E enLayerId)
{
    HI_S32        s32Ret;
	OPTM_GFX_GP_E enGPId; 
	
 	PRINT_IN;

    if (g_stGfxCap[enLayerId].bLayerSupported != HI_TRUE)
    {
    	HIFB_ERROR("Gfx%d was not supported!\n",enLayerId); //2193
        return HI_FAILURE;
    }

    if (g_stGfxDevice[enLayerId].bOpened == HI_TRUE)
    {
    	HIFB_WARNING("info:Gfx%d was opened!\n",enLayerId); //2199
        return HI_SUCCESS;
    }

    if ((HIFB_GFX_MODE_HD_WBC == g_enOptmGfxWorkMode)
        &&(HIFB_LAYER_SD_0 ==  enLayerId)) //4
    {
        HIFB_WARNING("GFX work at wbc mode, gfx%d is working!\n", enLayerId); //2206
        return HI_FAILURE;
    }
	
    s32Ret = OPTM_GfxInitLayer(enLayerId);
    if (s32Ret != HI_SUCCESS)
    {
        HIFB_ERROR("fail to init GFX%d!\n", enLayerId); //2213
        return HI_FAILURE;
    }
	
	enGPId = g_stGfxDevice[enLayerId].enGPId;

	s32Ret = OPTM_GPOpen(enGPId);
	if (HI_SUCCESS != s32Ret)
	{
		return HI_FAILURE;
	}
	
	s32Ret = OPTM_SetCallbackToDisp(enGPId, (IntCallBack)OPTM_DispCallBack, HI_DRV_DISP_C_INTPOS_90_PERCENT, HI_TRUE);
	if (HI_SUCCESS != s32Ret)
	{
		HIFB_ERROR("Disp was not ready, open gfx%d failure!\n", enLayerId); //2228
		return HI_FAILURE;
	}


	s32Ret = OPTM_SetCallbackToDisp(enGPId, (IntCallBack)OPTM_FrameEndCallBack, HI_DRV_DISP_C_GFX_WBC, HI_TRUE);
	if (HI_SUCCESS != s32Ret)
	{
		HIFB_ERROR("fail to register FrameEndCallBack\n", enLayerId); //2236
		return HI_FAILURE;
	}



	if (g_stGfxCap[enLayerId].bHasCmapReg != HI_FALSE)
    {
    	HI_CHAR name[32];
		snprintf(name, sizeof(name), "HIFB_Fb%d_Clut", enLayerId);
        /*  apply clut table buffer */  
        if (OPTM_Adapt_AllocAndMap(name, HI_NULL, OPTM_CMAP_SIZE, 0, &g_stGfxDevice[enLayerId].stCluptTable) != HI_SUCCESS)
        {
            HIFB_ERROR("GFX Get clut buffer failed!\n"); //2249
            return HI_FAILURE;
        }		

        OPTM_VDP_GFX_SetLutAddr(g_stGfxDevice[enLayerId].enGfxHalId, g_stGfxDevice[enLayerId].stCluptTable.u32StartPhyAddr);
    }	

	/*  set layer open flag true*/
    g_stGfxDevice[enLayerId].bOpened = HI_TRUE;

	PRINT_OUT;
    
    return HI_SUCCESS;
}

static HI_S32 OPTM_CheckGpState(OPTM_GFX_GP_E enGPId)
{
	HI_U32 i;
	HI_U32 u32LayerIdSta, u32LayerIdEnd;
	
	if (OPTM_GFX_GP_0 == enGPId)
	{
		u32LayerIdSta = HIFB_LAYER_HD_0;
		u32LayerIdEnd = HIFB_LAYER_HD_3;
	}
	else if (OPTM_GFX_GP_1 == enGPId)
	{
		u32LayerIdSta = HIFB_LAYER_SD_0;
		u32LayerIdEnd = HIFB_LAYER_SD_1;
	}
	else
	{
		return OPTM_DISABLE;
	}

	for (i = u32LayerIdSta; i <= u32LayerIdEnd; i++)
	{
		if (g_stGfxDevice[i].bOpened)
		{
			return OPTM_ENABLE;
		}
	}

	return OPTM_DISABLE;
}

HI_S32 OPTM_GfxCloseLayer(HIFB_LAYER_ID_E enLayerId)
{   
#if 1
#warning TODO
#else
	OPTM_GFX_GP_E enGPId;
	
    PRINT_IN;
    
    if (g_stGfxDevice[enLayerId].bOpened == HI_FALSE)
    {
        return HI_SUCCESS;
    }

	enGPId = g_stGfxDevice[enLayerId].enGPId;
	
	/*set layer decompression false*/
    if(g_stGfxDevice[enLayerId].bCampEnable)
    {
        OPTM_GfxSetLayerDeCmpEnable(enLayerId, HI_FALSE);
    }

	if (OPTM_VDP_LAYER_GFX3 == g_stGfxDevice[enLayerId].enGfxHalId)
	{
		OPTM_VDP_OpenGFX3(HI_FALSE);
		OPTM_VDP_SetLayerConnect(OPTM_VDP_CONNECT_G3_DHD0);
	}

    /* set layer disenable, confirm hardware close */
	OPTM_GfxSetEnable(enLayerId, HI_FALSE);
	OPTM_VDP_GFX_SetRegUp(g_stGfxDevice[enLayerId].enGfxHalId);
	OPTM_GfxWaitVBlank(enLayerId);
	OPTM_GfxDeInitLayer(enLayerId);
	
	g_stGfxDevice[enLayerId].bExtractLine = HI_FALSE;
	g_stGfxDevice[enLayerId].bOpened      = HI_FALSE;

	/*if gp closed ,set gp disable and close wbc*/
	if (!OPTM_CheckGpState(enGPId))
	{	
		if (g_enOptmGfxWorkMode == HIFB_GFX_MODE_HD_WBC)
		{
			OPTM_GfxCloseSlvLayer(OPTM_SLAVER_LAYERID);
		    OPTM_GPClose         (OPTM_SLAVER_GPID);
		}

		OPTM_GPClose(enGPId);
	}

    PRINT_OUT;
#endif
    
    return HI_SUCCESS;
}

HI_S32 OPTM_JudgeWbcEnable(HI_VOID)
{
	HI_U32 i;
	for (i = 0; i < HIFB_LAYER_SD_0; i++)
	{
		if (g_stGfxDevice[i].bEnable)
		{
			return HI_TRUE;
		}
	}
	
	return HI_FALSE;
}

HI_S32 OPTM_GfxSetEnable(HIFB_LAYER_ID_E enLayerId, HI_BOOL bEnable)
{
#if 1
#warning TODO
#else
	OPTM_GFX_GP_E enGpId;
	
    PRINT_IN;

    g_stGfxDevice[enLayerId].bEnable = bEnable;

	enGpId  = g_stGfxDevice[enLayerId].enGPId;

	OPTM_CheckGPMask_BYGPID(enGpId);
	
    OPTM_VDP_GFX_SetLayerEnable(g_stGfxDevice[enLayerId].enGfxHalId, bEnable);		
	OPTM_VDP_GFX_SetRegUp      (g_stGfxDevice[enLayerId].enGfxHalId);

	if (IS_MASTER_GP(enGpId))
	{	
		if (bEnable)
		{
			g_stGfxDevice[OPTM_SLAVER_LAYERID].bEnable = HI_TRUE;
		}
		else
		{
			g_stGfxDevice[OPTM_SLAVER_LAYERID].bEnable = OPTM_JudgeWbcEnable();
		}

	   	g_stGfxGPDevice[OPTM_SLAVER_GPID].unUpFlag.bits.Enable = 1;
		g_stGfxGPDevice[OPTM_SLAVER_GPID].unUpFlag.bits.RegUp  = 1;		
    }

    PRINT_OUT;
#endif
    
    return HI_SUCCESS;
}

HI_S32 OPTM_GfxSetLayerAddr(HIFB_LAYER_ID_E enLayerId, HI_U32 u32Addr)
{
#if 1
#warning TODO
#else
	PRINT_IN;
    g_stGfxDevice[enLayerId].NoCmpBufAddr = u32Addr;

	OPTM_CheckGPMask_BYLayerID(enLayerId);

    OPTM_VDP_GFX_SetLayerAddrEX(g_stGfxDevice[enLayerId].enGfxHalId, u32Addr);

	if (IS_SLAVER_GP(g_stGfxDevice[enLayerId].enGPId))
	{	
		OPTM_GFX_WBC_S   *pstWbc2;
		
		pstWbc2      = &g_stGfxWbc2;
		
		OPTM_VDP_WBC_SetLayerAddr(pstWbc2->enWbcHalId, u32Addr,
        							0x0, pstWbc2->u32BufferStride, 0x0);
	}
	
    PRINT_OUT;
#endif
	
    return HI_SUCCESS;
}

HI_S32 OPTM_GfxSetWbcAddr(OPTM_VDP_LAYER_WBC_E enWbcHalId, HI_U32 u32Addr, HI_U32 u32BufferStride)
{
	if (g_stGfxGPDevice[g_stGfxDevice[HIFB_LAYER_SD_0/*4*/].enGPId].bMaskFlag == HI_FALSE)
	{
		OPTM_VDP_WBC_SetLayerAddr(enWbcHalId, u32Addr, 0x0, u32BufferStride, 0x0);
	}

    return HI_SUCCESS;
}

HI_S32 OPTM_GfxSetLayerStride(HIFB_LAYER_ID_E enLayerId, HI_U32 u32Stride)
{   
	HI_U16	u16StridePre;

	PRINT_IN;
		 
    g_stGfxDevice[enLayerId].Stride = (HI_U16)u32Stride;

	OPTM_CheckGPMask_BYLayerID(enLayerId);

	if (g_stGfxGPDevice[g_stGfxDevice[enLayerId].enGPId].bNeedExtractLine
		&& /*OPTM_CURSOR_LAYERID*/HIFB_LAYER_HD_3 != enLayerId)
	{
		OPTM_VDP_GFX_SetLayerStride(g_stGfxDevice[enLayerId].enGfxHalId, u32Stride*2);
	}
	else
	{
    	OPTM_VDP_GFX_SetLayerStride(g_stGfxDevice[enLayerId].enGfxHalId, u32Stride);
	}
        

    /* when set G0 stride we should change current mode from cmp mode to no cmp mode*/ 
    u16StridePre = g_stGfxDevice[enLayerId].Stride;
    g_stGfxDevice[enLayerId].Stride = (HI_U16)u32Stride;
#if 0
	/* cmp mode, stride is" width*2 + head size" algin with 16 bytes*/
    g_stGfxDevice[enLayerId].CmpStride = ((HI_U16)u32Stride/2 + (u32Stride/2/128/8 +1)  +0xf)&0xfffffff0;
#endif

    if(g_stGfxDevice[enLayerId].bCampEnable)
    {
		if(enLayerId == HIFB_LAYER_HD_0)
		{ 
		    if(u16StridePre !=  (HI_U16)u32Stride)
		    {
			    OPTM_GfxChgCmp2Ncmp(HIFB_LAYER_HD_0);          
		    }
		}          
    }
    else
    {
	    //OPTM_VDP_GFX_SetLayerStride(g_stGfxDevice[enLayerId].enGfxHalId, u32Stride);
    }

	PRINT_OUT;
	
    return HI_SUCCESS;
}

HIFB_COLOR_FMT_E OPTM_HalFmtTransferToPixerFmt(OPTM_VDP_GFX_IFMT_E enDataFmt)
{
    if (enDataFmt >= VDP_GFX_IFMT_BUTT)
    {
        return HIFB_FMT_BUTT;
    }

    switch(enDataFmt)
    {
        case VDP_GFX_IFMT_CLUT_1BPP:
            return HIFB_FMT_1BPP;
        case VDP_GFX_IFMT_CLUT_2BPP:
            return HIFB_FMT_2BPP;
        case VDP_GFX_IFMT_CLUT_4BPP:
            return HIFB_FMT_4BPP;
        case VDP_GFX_IFMT_CLUT_8BPP:
            return HIFB_FMT_8BPP;
		case VDP_GFX_IFMT_ACLUT_44:
       		return HIFB_FMT_ACLUT44;
		case VDP_GFX_IFMT_RGB_444:
           return HIFB_FMT_KRGB444; 	   
		case VDP_GFX_IFMT_RGB_555:
           return HIFB_FMT_KRGB555;		   
        case VDP_GFX_IFMT_RGB_565:
            return HIFB_FMT_RGB565;
        case VDP_GFX_IFMT_PKG_UYVY:
            return HIFB_FMT_PUYVY;
        case VDP_GFX_IFMT_PKG_YUYV:
            return HIFB_FMT_PYUYV;
        case VDP_GFX_IFMT_PKG_YVYU:
            return HIFB_FMT_PYVYU;	
        case VDP_GFX_IFMT_ACLUT_88:
           return HIFB_FMT_ACLUT88;
        case VDP_GFX_IFMT_RGB_888:
           return HIFB_FMT_RGB888; 
        case VDP_GFX_IFMT_YCBCR_888:
           return HIFB_FMT_YUV888;	
        case VDP_GFX_IFMT_ARGB_8565:
           return HIFB_FMT_ARGB8565;	
        case VDP_GFX_IFMT_KRGB_888:
           return HIFB_FMT_KRGB888;
        case VDP_GFX_IFMT_ARGB_8888:
            return HIFB_FMT_ARGB8888;		   
        case VDP_GFX_IFMT_ARGB_4444:
            return HIFB_FMT_ARGB4444;
        case VDP_GFX_IFMT_ARGB_1555:
            return HIFB_FMT_ARGB1555;
		case VDP_GFX_IFMT_AYCBCR_8888:
			return HIFB_FMT_AYUV8888;
		case VDP_GFX_IFMT_RGBA_4444:
			return HIFB_FMT_RGBA4444;
		case VDP_GFX_IFMT_RGBA_5551:
			return HIFB_FMT_RGBA5551;
		case VDP_GFX_IFMT_RGBA_5658:
			return HIFB_FMT_RGBA5658;
		case VDP_GFX_IFMT_RGBA_8888:
			return HIFB_FMT_RGBA8888;
		case VDP_GFX_IFMT_YCBCRA_8888:
			return HIFB_FMT_YUVA8888;  
			
        default:
            return HIFB_FMT_BUTT;
    }
	
	return VDP_GFX_IFMT_BUTT;
}


OPTM_VDP_GFX_IFMT_E OPTM_PixerFmtTransferToHalFmt(HIFB_COLOR_FMT_E enDataFmt)
{
    if (enDataFmt >= HIFB_FMT_BUTT)
    {
        return VDP_GFX_IFMT_BUTT;
    }

    switch(enDataFmt)
    {
        case HIFB_FMT_1BPP:
            return VDP_GFX_IFMT_CLUT_1BPP;
        case HIFB_FMT_2BPP:
            return VDP_GFX_IFMT_CLUT_2BPP;
        case HIFB_FMT_4BPP:
            return VDP_GFX_IFMT_CLUT_4BPP;
        case HIFB_FMT_8BPP:
            return VDP_GFX_IFMT_CLUT_8BPP;
		case HIFB_FMT_ACLUT44:
       		return VDP_GFX_IFMT_ACLUT_44;
		case HIFB_FMT_KRGB444:
           return VDP_GFX_IFMT_RGB_444; 	   
		case HIFB_FMT_KRGB555:
           return VDP_GFX_IFMT_RGB_555;		   
        case HIFB_FMT_RGB565:
            return VDP_GFX_IFMT_RGB_565;
        case HIFB_FMT_PUYVY:
            return VDP_GFX_IFMT_PKG_UYVY;
        case HIFB_FMT_PYUYV:
            return VDP_GFX_IFMT_PKG_YUYV;
        case HIFB_FMT_PYVYU:
            return VDP_GFX_IFMT_PKG_YVYU;	
        case HIFB_FMT_ACLUT88:
           return VDP_GFX_IFMT_ACLUT_88;
        case HIFB_FMT_RGB888:
           return VDP_GFX_IFMT_RGB_888; 
        case HIFB_FMT_YUV888:
           return VDP_GFX_IFMT_YCBCR_888;	
        case HIFB_FMT_ARGB8565:
           return VDP_GFX_IFMT_ARGB_8565;	
        case HIFB_FMT_KRGB888:
           return VDP_GFX_IFMT_KRGB_888;
        case HIFB_FMT_ARGB8888:
            return VDP_GFX_IFMT_ARGB_8888;		   
        case HIFB_FMT_ARGB4444:
            return VDP_GFX_IFMT_ARGB_4444;
        case HIFB_FMT_ARGB1555:
            return VDP_GFX_IFMT_ARGB_1555;
		case HIFB_FMT_AYUV8888:
			return VDP_GFX_IFMT_AYCBCR_8888;
		case HIFB_FMT_RGBA4444:
			return VDP_GFX_IFMT_RGBA_4444;
		case HIFB_FMT_RGBA5551:
			return VDP_GFX_IFMT_RGBA_5551;
		case HIFB_FMT_RGBA5658:
			return VDP_GFX_IFMT_RGBA_5658;
		case HIFB_FMT_RGBA8888:
			return VDP_GFX_IFMT_RGBA_8888;
		case HIFB_FMT_YUVA8888:
			return VDP_GFX_IFMT_YCBCRA_8888;  

		case HIFB_FMT_BGR565:
			return VDP_GFX_IFMT_RGB_565; 
		case HIFB_FMT_BGR888:
			return VDP_GFX_IFMT_RGB_888; 
		case HIFB_FMT_ABGR4444:
			return VDP_GFX_IFMT_ARGB_4444; 
		case HIFB_FMT_ABGR1555:
			return VDP_GFX_IFMT_ARGB_1555; 
		case HIFB_FMT_ABGR8888:
			return VDP_GFX_IFMT_ABGR_8888; 
		case HIFB_FMT_ABGR8565:
			return VDP_GFX_IFMT_ARGB_8565; 
		case HIFB_FMT_KBGR444:
			return VDP_GFX_IFMT_RGB_444; 
		case HIFB_FMT_KBGR555:
			return VDP_GFX_IFMT_RGB_555; 
		case HIFB_FMT_KBGR888:
			return VDP_GFX_IFMT_KRGB_888; 
			
        default:
            return VDP_GFX_IFMT_BUTT;
    }
	
	return VDP_GFX_IFMT_BUTT;
}

HI_S32 OPTM_GfxSetLayerDataFmt(HIFB_LAYER_ID_E enLayerId, HIFB_COLOR_FMT_E enDataFmt)
{
#if 1
#warning TODO
#else
	HI_U32 i;
	HI_U32 u32LayerCount;
	OPTM_GFX_GP_E enGPId;
	PRINT_IN;

	if (!g_stGfxCap[enLayerId].bColFmt[enDataFmt])
	{
		HIFB_ERROR("unSupport color format.\n");
		return HI_FAILURE;
	}
	
	enGPId = g_stGfxDevice[enLayerId].enGPId;
	g_stGfxDevice[enLayerId].enDataFmt = enDataFmt;

	//printk("===OPTM_GfxSetLayerDataFmt===ID %d,fmt %d===.\n",enLayerId,enDataFmt);

	OPTM_CheckGPMask_BYGPID(enGPId);	

    if(HIFB_FMT_ABGR8888 != enDataFmt && (enDataFmt >= HIFB_FMT_BGR565 && HIFB_FMT_KBGR888 >= enDataFmt)
		&& g_stGfxDevice[enLayerId].CscState == OPTM_CSC_SET_PARA_RGB)
    {
    	u32LayerCount = (HIFB_LAYER_HD_3 >= enLayerId) ? OPTM_GP0_GFX_COUNT : OPTM_GP1_GFX_COUNT;
		for (i = 0; i < u32LayerCount; i++)
		{
			if ((i != enLayerId) && g_stGfxDevice[i].bEnable
				&&(g_stGfxDevice[i].CscState != OPTM_CSC_SET_PARA_BGR))
			{
				//printk("enLayerId %d, enable %d.\n",enLayerId,g_stGfxDevice[enLayerId].bEnable);
				HIFB_ERROR("fail to set color format.\n");
				return HI_FAILURE;
			}
		}
	    g_stGfxDevice[enLayerId].CscState = OPTM_CSC_SET_PARA_BGR;
		g_stGfxGPDevice[enGPId].bBGRState = HI_TRUE;
		OPTM_GfxSetCsc(enGPId,  &g_stGfxGPDevice[enGPId].stCscPara, HI_TRUE);
		if (IS_MASTER_GP(enGPId))
		{
			OPTM_GfxSetCsc(OPTM_SLAVER_GPID,  &g_stGfxGPDevice[OPTM_SLAVER_GPID].stCscPara, HI_TRUE);
			g_stGfxGPDevice[OPTM_SLAVER_GPID].bBGRState = HI_TRUE;
		}
    }
    else if(((HIFB_FMT_ABGR8888 == enDataFmt) || (HIFB_FMT_BGR565 > enDataFmt || enDataFmt > HIFB_FMT_KBGR888))
		&& g_stGfxDevice[enLayerId].CscState == OPTM_CSC_SET_PARA_BGR)
    {
    	u32LayerCount = (HIFB_LAYER_HD_3 >= enLayerId) ? OPTM_GP0_GFX_COUNT : OPTM_GP1_GFX_COUNT;
		for (i = 0; i < u32LayerCount; i++)
		{
			if ((i != enLayerId) && g_stGfxDevice[i].bEnable
				&&(g_stGfxDevice[i].CscState != OPTM_CSC_SET_PARA_RGB))
			{
				HIFB_ERROR("fail to set color format.\n");
				return HI_FAILURE;
			}
		}
    	g_stGfxDevice[enLayerId].CscState = OPTM_CSC_SET_PARA_RGB;
		g_stGfxGPDevice[enGPId].bBGRState = HI_FALSE;
        OPTM_GfxSetCsc(enGPId,  &g_stGfxGPDevice[enGPId].stCscPara, HI_FALSE);
		if (IS_MASTER_GP(enGPId))
		{
			OPTM_GfxSetCsc(OPTM_SLAVER_GPID,  &g_stGfxGPDevice[OPTM_SLAVER_GPID].stCscPara, HI_FALSE);
			g_stGfxGPDevice[OPTM_SLAVER_GPID].bBGRState = HI_FALSE;
		}
    }

	OPTM_VDP_GFX_SetInDataFmt(g_stGfxDevice[enLayerId].enGfxHalId,OPTM_PixerFmtTransferToHalFmt(enDataFmt));

	PRINT_OUT;
#endif
    return HI_SUCCESS;
}

HI_S32 OPTM_GfxSetColorReg(HIFB_LAYER_ID_E enLayerId, HI_U32 u32OffSet, HI_U32 u32Color, HI_S32 UpFlag)
{
#if 1
#warning TODO
#else
	HI_U32 *pCTab;

	pCTab = (HI_U32 *)(g_stGfxDevice[enLayerId].stCluptTable.u32StartVirAddr);
	if (HI_NULL == pCTab)
	{
		HIFB_ERROR("Unable to handle null virtual address!\n");
		return HI_FAILURE;
	}
	
	pCTab[u32OffSet] = u32Color;
	if (UpFlag != 0)
    {
        OPTM_VDP_GFX_SetParaUpd(g_stGfxDevice[enLayerId].enGfxHalId,VDP_DISP_COEFMODE_LUT);
    }
#endif
	
    return HI_SUCCESS;
}

#ifdef OPTM_HIFB_WVM_ENABLE
HI_VOID OPTM_GfxWVBCallBack(HI_U32 enLayerId, HI_U32 u32Param1)
{
  
    g_stGfxDevice[enLayerId].vblflag = 1;
    wake_up_interruptible(&(g_stGfxDevice[enLayerId].vblEvent));

    return;
}

HI_S32 OPTM_GfxWaitVBlank(HIFB_LAYER_ID_E enLayerId)
{
	HI_S32 ret;
    HI_U32 u32TimeOutMs;

	PRINT_IN;
	
    D_OPTM_HIFB_CheckGfxOpen(enLayerId);

    u32TimeOutMs = (200 * HZ)/1000;


    g_stGfxDevice[enLayerId].vblflag = 0;
    ret = wait_event_interruptible_timeout(g_stGfxDevice[enLayerId].vblEvent, g_stGfxDevice[enLayerId].vblflag, u32TimeOutMs);

	PRINT_OUT;
	
    return ret;
}

#else
HI_S32 OPTM_GfxWaitVBlank(HIFB_LAYER_ID_E enLayerId)
{
	HIFB_ERROR("GFX ERROR! NOT enable wait v blank\n");
    return HI_FAILURE;
}
#endif


HI_S32 OPTM_GfxSetLayerDeFlicker(HIFB_LAYER_ID_E enLayerId, HIFB_DEFLICKER_S *pstDeFlicker)
{
    return HI_SUCCESS;
}

HI_S32 OPTM_GfxSetLayerAlpha(HIFB_LAYER_ID_E enLayerId, HIFB_ALPHA_S *pstAlpha)
{
#if 1
#warning TODO
#else
	OPTM_GFX_GP_E enGpId;
	
    PRINT_IN;

	enGpId = g_stGfxDevice[enLayerId].enGPId;

	memcpy(&g_stGfxDevice[enLayerId].stAlpha, pstAlpha, sizeof(HIFB_ALPHA_S));

	OPTM_CheckGPMask_BYLayerID(enLayerId);

	OPTM_VDP_GFX_SetPalpha(g_stGfxDevice[enLayerId].enGfxHalId,pstAlpha->bAlphaEnable,HI_TRUE,pstAlpha->u8Alpha0,pstAlpha->u8Alpha1);
	OPTM_VDP_GFX_SetLayerGalpha(g_stGfxDevice[enLayerId].enGfxHalId, pstAlpha->u8GlobalAlpha);
#if 0
	if (IS_MASTER_GP(enGpId))
	{
		HIFB_LAYER_ID_E enSlvGfxId = HIFB_LAYER_SD_0;
		
		g_stGfxDevice  [enSlvGfxId].stAlpha                    = *pstAlpha;	    
		g_stGfxGPDevice[OPTM_SLAVER_GPID].unUpFlag.bits.Alpha  = 1;		
		g_stGfxGPDevice[OPTM_SLAVER_GPID].unUpFlag.bits.RegUp  = 1;
	}
#endif    

	PRINT_OUT;
#endif
	 
    return HI_SUCCESS;
}

HI_S32 OPTM_GfxGetLayerRect(HIFB_LAYER_ID_E enLayerId, HIFB_RECT *pstRect)
{
	PRINT_IN;

	memcpy(pstRect, &g_stGfxDevice[enLayerId].stInRect, sizeof(HIFB_RECT));
	
	PRINT_OUT;

	return HI_SUCCESS;
}

HI_S32 OPTM_GfxConfigCursorRect(HIFB_LAYER_ID_E enLayerId, const HIFB_RECT *pstRect)
{
#if 1
#warning TODO
#else
	OPTM_GFX_GP_E enGpId;
	OPTM_VDP_DISP_RECT_S stGfxRect;
	
	if (OPTM_CURSOR_LAYERID != enLayerId)
	{
		return HI_FAILURE;
	}

	enGpId = g_stGfxDevice[enLayerId].enGPId;
	memset(&stGfxRect, 0, sizeof(OPTM_VDP_DISP_RECT_S));

	stGfxRect.u32DXS = g_stGfxGPDevice[enGpId].stOutRect.s32X & 0xfffffffe; 
	stGfxRect.u32DYS = g_stGfxGPDevice[enGpId].stOutRect.s32Y & 0xfffffffc;
	stGfxRect.u32DXL = g_stGfxGPDevice[enGpId].stOutRect.s32Width & 0xfffffffe;
    stGfxRect.u32DYL = g_stGfxGPDevice[enGpId].stOutRect.s32Height & 0xfffffffc;
	
	stGfxRect.u32VX  = g_stGfxDevice[enLayerId].stInRect.x & 0xfffffffe;	
	stGfxRect.u32VY  = g_stGfxDevice[enLayerId].stInRect.y & 0xfffffffc;	
	stGfxRect.u32IWth= g_stGfxDevice[enLayerId].stInRect.w & 0xfffffffe;
    stGfxRect.u32IHgt= g_stGfxDevice[enLayerId].stInRect.h & 0xfffffffc;

	if (stGfxRect.u32VX > stGfxRect.u32DXL)
	{
		stGfxRect.u32VX = stGfxRect.u32DXL;
	}

	if (stGfxRect.u32VY > stGfxRect.u32DYL)
	{
		stGfxRect.u32VY = stGfxRect.u32DYL;
	}

	if ((stGfxRect.u32VX + stGfxRect.u32IWth) > stGfxRect.u32DXL)
	{
		stGfxRect.u32IWth = stGfxRect.u32DXL - stGfxRect.u32VX;
	}

	if ((stGfxRect.u32VY + stGfxRect.u32IHgt) > stGfxRect.u32DYL)
	{
		stGfxRect.u32IHgt = stGfxRect.u32DYL - stGfxRect.u32VY;
	}

	stGfxRect.u32OWth= stGfxRect.u32IWth;
    stGfxRect.u32OHgt= stGfxRect.u32IHgt;
	
	OPTM_VDP_GFX_SetLayerReso(g_stGfxDevice[enLayerId].enGfxHalId, stGfxRect);
    OPTM_VDP_GFX_SetRegUp    (g_stGfxDevice[enLayerId].enGfxHalId);
#endif

	
	return HI_SUCCESS;
}

HI_S32 OPTM_GfxSetLayerRect(HIFB_LAYER_ID_E enLayerId, const HIFB_RECT *pstRect)
{
#if 1
#warning TODO
#else
	OPTM_GFX_GP_E enGpId;
	OPTM_VDP_DISP_RECT_S stGfxRect;	

	PRINT_IN;

    g_stGfxDevice[enLayerId].stInRect.x = pstRect->x;
	g_stGfxDevice[enLayerId].stInRect.y = pstRect->y;
	g_stGfxDevice[enLayerId].stInRect.w = pstRect->w;
	g_stGfxDevice[enLayerId].stInRect.h = pstRect->h;	

	//printk("===OPTM_GfxSetLayerRect==ID %d,w %d,h %d===\n",enLayerId,pstRect->w,pstRect->h);

	OPTM_CheckGPMask_BYLayerID(enLayerId);

	if (OPTM_CURSOR_LAYERID == enLayerId)
	{
		return OPTM_GfxConfigCursorRect(enLayerId, pstRect);
	}

	enGpId = g_stGfxDevice[enLayerId].enGPId;
	
    memset(&stGfxRect, 0, sizeof(OPTM_VDP_DISP_RECT_S));
	stGfxRect.u32VX  = g_stGfxDevice[enLayerId].stInRect.x;
	stGfxRect.u32DXS = g_stGfxDevice[enLayerId].stInRect.x; 
	stGfxRect.u32VY  = g_stGfxDevice[enLayerId].stInRect.y;
	stGfxRect.u32DYS = g_stGfxDevice[enLayerId].stInRect.y;
	stGfxRect.u32IWth= g_stGfxDevice[enLayerId].stInRect.w;
    stGfxRect.u32IHgt= g_stGfxDevice[enLayerId].stInRect.h;

	/*when gfx's disprect beyond the region of gp_inrect, clip disprect before set it to hal*/
	if (g_stGfxDevice[enLayerId].stInRect.x > g_stGfxGPDevice[enGpId].stInRect.s32Width - g_stGfxCap[enLayerId].u32MinWidth)
	{
		g_stGfxDevice[enLayerId].stInRect.x = g_stGfxGPDevice[enGpId].stInRect.s32Width - g_stGfxCap[enLayerId].u32MinWidth;
	}

	if (g_stGfxDevice[enLayerId].stInRect.y > g_stGfxGPDevice[enGpId].stInRect.s32Height - g_stGfxCap[enLayerId].u32MinHeight)
	{
		g_stGfxDevice[enLayerId].stInRect.y = g_stGfxGPDevice[enGpId].stInRect.s32Height - g_stGfxCap[enLayerId].u32MinHeight;
	}

	if (!IS_SLAVER_GP(enGpId))
	{
		if ((g_stGfxDevice[enLayerId].stInRect.x + g_stGfxDevice[enLayerId].stInRect.w)
			> g_stGfxGPDevice[enGpId].stInRect.s32Width)
		{
			stGfxRect.u32IWth = g_stGfxGPDevice[enGpId].stInRect.s32Width - g_stGfxDevice[enLayerId].stInRect.x;
		}	

		if ((g_stGfxDevice[enLayerId].stInRect.y + g_stGfxDevice[enLayerId].stInRect.h)
			> g_stGfxGPDevice[enGpId].stInRect.s32Height)
		{
			stGfxRect.u32IHgt = g_stGfxGPDevice[enGpId].stInRect.s32Height - g_stGfxDevice[enLayerId].stInRect.y;
		}
	}	

	if (g_stGfxGPDevice[enGpId].bNeedExtractLine)
	{
		stGfxRect.u32IHgt /= 2;
		stGfxRect.u32VY   /= 2;
		stGfxRect.u32DYS  /= 2;
		g_stGfxDevice[enLayerId].bExtractLine = HI_TRUE;
		OPTM_VDP_GFX_SetLayerStride(g_stGfxDevice[enLayerId].enGfxHalId, g_stGfxDevice[enLayerId].Stride*2);
	}
	else if (HI_TRUE == g_stGfxDevice[enLayerId].bExtractLine)
	{		
		OPTM_GfxSetLayerStride(enLayerId, g_stGfxDevice[enLayerId].Stride);
		g_stGfxDevice[enLayerId].bExtractLine = HI_FALSE;
	}
	
	
	stGfxRect.u32OWth= stGfxRect.u32IWth;
    stGfxRect.u32OHgt= stGfxRect.u32IHgt;
	stGfxRect.u32DXL = g_stGfxDevice[enLayerId].stInRect.x + stGfxRect.u32OWth;
    stGfxRect.u32DYL = g_stGfxDevice[enLayerId].stInRect.y + stGfxRect.u32OHgt;

	/*************************************/
	if ((g_enOptmGfxWorkMode == HIFB_GFX_MODE_NORMAL)
		&&(enGpId == OPTM_GFX_GP_1))
	{
		stGfxRect.u32DXS = 0;
		stGfxRect.u32DYS = 0;
		stGfxRect.u32DXL = g_stGfxGPDevice[g_stGfxDevice[enLayerId].enGPId].stInRect.s32Width;
		stGfxRect.u32DYL = g_stGfxGPDevice[g_stGfxDevice[enLayerId].enGPId].stInRect.s32Height;

		if (g_stGfxGPDevice[enGpId].bNeedExtractLine)
		{
			stGfxRect.u32DYL /= 2;
		}
	}

	/*************************************/
	if (HIFB_STEREO_SIDEBYSIDE_HALF == g_stGfxDevice[enLayerId].enTriDimMode
		|| HIFB_STEREO_TOPANDBOTTOM == g_stGfxDevice[enLayerId].enTriDimMode)
	{
		stGfxRect.u32DXS = 0;
		stGfxRect.u32DYS = 0;
		stGfxRect.u32DXL = g_stGfxGPDevice[g_stGfxDevice[enLayerId].enGPId].stInRect.s32Width;
		stGfxRect.u32DYL = g_stGfxGPDevice[g_stGfxDevice[enLayerId].enGPId].stInRect.s32Height;
	}

	if (HIFB_STEREO_SIDEBYSIDE_HALF == g_stGfxDevice[enLayerId].enTriDimMode)
	{
		stGfxRect.u32IWth = stGfxRect.u32IWth/2;
		stGfxRect.u32IWth &= 0xfffffffe;
		
		stGfxRect.u32OWth = (stGfxRect.u32IWth)*2;
	}
	else if (HIFB_STEREO_TOPANDBOTTOM == g_stGfxDevice[enLayerId].enTriDimMode)
	{
		stGfxRect.u32IHgt = stGfxRect.u32IHgt/2;
		stGfxRect.u32IHgt &= 0xfffffffe;
		stGfxRect.u32OHgt = (stGfxRect.u32IHgt)*2;
	}

	stGfxRect.u32VX   &= 0xfffffffe;
	stGfxRect.u32VY   &= 0xfffffffc;
	
	stGfxRect.u32DXS  &= 0xfffffffe; 	
	stGfxRect.u32DYS  &= 0xfffffffc;
	
	stGfxRect.u32DXL  &= 0xfffffffe;
	stGfxRect.u32DYL  &= 0xfffffffc;

	stGfxRect.u32IWth &= 0xfffffffe;
	stGfxRect.u32IHgt &= 0xfffffffc;

	stGfxRect.u32OWth &= 0xfffffffe;	
	stGfxRect.u32OHgt &= 0xfffffffc;

	OPTM_VDP_GFX_SetLayerReso(g_stGfxDevice[enLayerId].enGfxHalId, stGfxRect);
    OPTM_VDP_GFX_SetRegUp    (g_stGfxDevice[enLayerId].enGfxHalId);

	PRINT_OUT;
#endif
		
    return HI_SUCCESS;
}

HI_S32 OPTM_GfxSetGpInPutSize(OPTM_GFX_GP_E enGpId, HI_U32 u32Width, HI_U32 u32Height)
{
	g_stGfxGPDevice[enGpId].stInRect.s32Width  = u32Width;
	g_stGfxGPDevice[enGpId].stInRect.s32Height = u32Height;
	return HI_SUCCESS;
}

HI_S32 OPTM_GfxGetDispFMTSize(OPTM_GFX_GP_E enGpId, HIFB_RECT *pstOutRect)
{
	pstOutRect->x = g_stGfxGPDevice[enGpId].stOutRect.s32X;
	pstOutRect->y = g_stGfxGPDevice[enGpId].stOutRect.s32Y;
	pstOutRect->w = g_stGfxGPDevice[enGpId].stOutRect.s32Width;
	pstOutRect->h = g_stGfxGPDevice[enGpId].stOutRect.s32Height;

	if (pstOutRect->w == 0 || pstOutRect->h == 0)
	{
		pstOutRect->x = 0;	
		pstOutRect->y = 0;
		pstOutRect->w = 1280;
		pstOutRect->h = 720;
	}
	
#ifdef	CFG_HIFB_S40V2_PALNTSC_BUG
	if (pstOutRect->w == 1440 &&
		(pstOutRect->h == 576 || pstOutRect->h == 480))
	{
		pstOutRect->w /= 2;
	}
#endif

	return HI_SUCCESS;
}

HI_S32 OPTM_GFX_GetSlvLayerInfo(HIFB_SLV_LAYER_INFO_S* a)
{
	printk("OPTM_GFX_GetSlvLayerInfo: TODO\n");
}

HI_S32 OPTM_GFX_SetTCFlag(HI_U32 a)
{
//	printk("OPTM_GFX_SetTCFlag: TODO\n");
	Data_81143414 = a;

	return HI_SUCCESS;
}

HI_S32 OPTM_GfxSetGpDeflicker(int a, int b)
{
	printk("OPTM_GfxSetGpDeflicker: TODO\n");
}



HI_S32 OPTM_GfxSetDispFMTSize(OPTM_GFX_GP_E enGpId, const HI_RECT_S *pstOutRect)
{
	HI_U32    u32Ratio;
	HIFB_RECT     stInputRect;

	PRINT_IN;	

	g_stGfxGPDevice[enGpId].stOutRect.s32X = pstOutRect->s32X; 
	g_stGfxGPDevice[enGpId].stOutRect.s32Y = pstOutRect->s32Y;

	g_stGfxGPDevice[enGpId].stOutRect.s32Width  = pstOutRect->s32Width;
	g_stGfxGPDevice[enGpId].stOutRect.s32Height = pstOutRect->s32Height;

	//printk("===OPTM_GfxSetDispFMTSize==ID %d,w %d,h %d===.\n",enGpId,pstOutRect->s32Width,pstOutRect->s32Height);

	OPTM_CheckGPMask_BYGPID(enGpId);

	stInputRect.x = g_stGfxGPDevice[enGpId].stInRect.s32X;
	stInputRect.y = g_stGfxGPDevice[enGpId].stInRect.s32Y;
	stInputRect.w = g_stGfxGPDevice[enGpId].stInRect.s32Width;
	stInputRect.h = g_stGfxGPDevice[enGpId].stInRect.s32Height;

	/*judge wether need to extract line for layer or not*/
	u32Ratio =  g_stGfxGPDevice[enGpId].stInRect.s32Height*2;
	u32Ratio /= g_stGfxGPDevice[enGpId].stOutRect.s32Height;
	if (g_stGfxGPDevice[enGpId].bInterface)
	{
		//u32Ratio *= 2;
	}
	
	if (u32Ratio >= OPTM_EXTRACTLINE_RATIO
		&& !IS_SLAVER_GP(enGpId))
	{
		g_stGfxGPDevice[enGpId].bNeedExtractLine = HI_TRUE;
	}
	else
	{
		g_stGfxGPDevice[enGpId].bNeedExtractLine = HI_FALSE;
	}

	if (stInputRect.w && stInputRect.h)
	{
		OPTM_GfxSetGpRect(enGpId, &stInputRect);
	}	

	PRINT_OUT;

	return HI_SUCCESS;
	
}
#define SHARPEN_RATIO 3
#define SHARPEN_MAX_WIDTH 1920
HI_S32 OPTM_GfxSetGpRect(OPTM_GFX_GP_E enGpId, const HIFB_RECT * pstInputRect)
{
#if 1
#warning TODO
#else
	HI_U32          i;
	HI_U32          u32LayerCount;
	HIFB_LAYER_ID_E enLayerId;
	HIFB_LAYER_ID_E enInitLayerId;
	HI_BOOL         bSlvGp;
	HI_BOOL         bGfxSharpen;
	OPTM_VDP_LAYER_GP_E  enGpHalId;
	OPTM_VDP_DISP_RECT_S stGfxRect;

	OPTM_ALG_GZME_DRV_PARA_S stZmeDrvPara;
	OPTM_ALG_GZME_RTL_PARA_S stZmeRtlPara;

	OPTM_ALG_GDTI_DRV_PARA_S stDtiDrvPara;
	OPTM_ALG_GDTI_RTL_PARA_S stDtiRtlPara;

	PRINT_IN;
	bSlvGp     = HI_FALSE;
	enGpHalId  = g_stGfxGPDevice[enGpId].enGpHalId;

	g_stGfxGPDevice[enGpId].stInRect.s32Width  = pstInputRect->w;
	g_stGfxGPDevice[enGpId].stInRect.s32Height = pstInputRect->h;

	OPTM_CheckGPMask_BYGPID(enGpId);

	memset(&stZmeDrvPara, 0, sizeof(stZmeDrvPara));
	memset(&stZmeRtlPara, 0, sizeof(stZmeRtlPara));
	memset(&stDtiDrvPara, 0, sizeof(stDtiDrvPara));
	memset(&stDtiRtlPara, 0, sizeof(stDtiRtlPara));

	HIFB_INFO("set gp%d in rect %d,%d.\n",enGpId,g_stGfxGPDevice[enGpId].stInRect.s32Width,g_stGfxGPDevice[enGpId].stInRect.s32Height);
	
    memset(&stGfxRect, 0, sizeof(OPTM_VDP_DISP_RECT_S));

    stGfxRect.u32IWth = g_stGfxGPDevice[enGpId].stInRect.s32Width & 0xfffffffe;
    stGfxRect.u32IHgt = g_stGfxGPDevice[enGpId].stInRect.s32Height & 0xfffffffc;
    stGfxRect.u32OWth = g_stGfxGPDevice[enGpId].stOutRect.s32Width & 0xfffffffe;
    stGfxRect.u32OHgt = g_stGfxGPDevice[enGpId].stOutRect.s32Height & 0xfffffffc;

	//printk("GP %d %d %d %d \n",stGfxRect.u32IWth,stGfxRect.u32IHgt,stGfxRect.u32OWth,stGfxRect.u32OHgt);

	stGfxRect.u32DXS  = g_stGfxGPDevice[enGpId].stOutRect.s32X & 0xfffffffe;
	stGfxRect.u32DYS  = g_stGfxGPDevice[enGpId].stOutRect.s32Y & 0xfffffffc;
	stGfxRect.u32DXL  = stGfxRect.u32OWth + stGfxRect.u32DXS;
	stGfxRect.u32DYL  = stGfxRect.u32OHgt + stGfxRect.u32DYS;
	stGfxRect.u32VX   = stGfxRect.u32DXS;
	stGfxRect.u32VY   = stGfxRect.u32DYS;
	stGfxRect.u32VXL  = stGfxRect.u32OWth + stGfxRect.u32VX;
	stGfxRect.u32VYL  = stGfxRect.u32OHgt + stGfxRect.u32VY;

	if (stGfxRect.u32IWth == 0 || stGfxRect.u32IHgt == 0 ||
		stGfxRect.u32OWth == 0 || stGfxRect.u32OHgt== 0)
	{		
		return HI_SUCCESS;
	}

	//printk("~~~~~~~~~~~~~~~%d,%d\n",g_stGfxGPDevice[enGpId].u32InWidth,g_stGfxGPDevice[enGpId].u32OutWidth);
	if (g_stGfxGPDevice[enGpId].bNeedExtractLine == HI_TRUE)
	{
		stGfxRect.u32IHgt /= 2;
	}
	
	OPTM_VDP_GP_SetLayerReso(enGpHalId, stGfxRect);    

	/****set scale coefficient****/
	/*Frame format for zme : 0-field; 1-frame*/
	stZmeDrvPara.bZmeFrmFmtIn  = HI_TRUE;
	stZmeDrvPara.bZmeFrmFmtOut = g_stGfxGPDevice[enGpId].bInterface ? HI_FALSE : HI_TRUE;

	//printk("++++++++++++gp out interface :%d..\n",g_stGfxGPDevice[enGpId].bInterface);

	if (IS_SLAVER_GP(enGpId))
	{	
		OPTM_VDP_DISP_RECT_S stWbcRect;
		stGfxRect.u32IWth  = g_stGfxWbc2.stInRect.w & 0xfffffffe;
		stGfxRect.u32IHgt  = g_stGfxWbc2.stInRect.h & 0xfffffffc;

		if (stGfxRect.u32IWth == 0 ||
			stGfxRect.u32IHgt == 0)
		{
			return HI_SUCCESS;
		}

	    /*in wbc work mode, slavery channel show 2D image*/
		if (HIFB_STEREO_SIDEBYSIDE_HALF == 
			g_stGfxGPDevice[OPTM_MASTER_GPID].enTriDimMode)
		{
			stGfxRect.u32IWth /= 2;
		}
		else if (HIFB_STEREO_TOPANDBOTTOM == 
			g_stGfxGPDevice[OPTM_MASTER_GPID].enTriDimMode)
		{
			stGfxRect.u32IHgt /= 2;
		}
	
		if ((g_stGfxGPDevice[OPTM_MASTER_GPID].bNeedExtractLine == HI_TRUE))
		{
			stGfxRect.u32IHgt /= 2;
		}
		
		memset(&stWbcRect, 0, sizeof(OPTM_VDP_DISP_RECT_S));
		
		stWbcRect.u32DXL = stGfxRect.u32IWth;
		stWbcRect.u32DYL = stGfxRect.u32IHgt;
		OPTM_VDP_WBC_SetCropReso (g_stGfxWbc2.enWbcHalId, stWbcRect);
	}
	
	stZmeDrvPara.u32ZmeFrmWIn  = stGfxRect.u32IWth;
	stZmeDrvPara.u32ZmeFrmHIn  = stGfxRect.u32IHgt;
#if 0
	if (g_stGfxGPDevice[enGpId].bNeedExtractLine == HI_TRUE)
	{
		stZmeDrvPara.u32ZmeFrmHIn /= 2;
	}

	if (IS_SLAVER_GP(enGpId)
		&& (g_stGfxGPDevice[OPTM_MASTER_GPID].bNeedExtractLine == HI_TRUE))
	{
		stZmeDrvPara.u32ZmeFrmHIn /= 2;
	}
#endif
	//stZmeDrvPara.u32ZmeFrmWOut = g_stGfxGPDevice[enGpId].stOutRect.s32Width;
	//stZmeDrvPara.u32ZmeFrmHOut = g_stGfxGPDevice[enGpId].stOutRect.s32Height;
	stZmeDrvPara.u32ZmeFrmWOut = stGfxRect.u32OWth;
	stZmeDrvPara.u32ZmeFrmHOut = stGfxRect.u32OHgt;
	
	if (OPTM_DISPCHANNEL_1 == g_stGfxGPDevice[enGpId].enDispCh)
	{
		//printf("====OPTM_ALG_GZmeHDSet====");
		OPTM_ALG_GZmeHDSet(&GfxZmeModule, &stZmeDrvPara, &stZmeRtlPara);
	}
	else if (OPTM_DISPCHANNEL_0 == g_stGfxGPDevice[enGpId].enDispCh)
	{
		OPTM_ALG_GZmeSDSet(&GfxZmeModule, &stZmeDrvPara, &stZmeRtlPara);
	}
	else
	{
		return HI_FAILURE;
	}

	stDtiDrvPara.u32ZmeFrmWIn  = stGfxRect.u32IWth;
	stDtiDrvPara.u32ZmeFrmHIn  = stGfxRect.u32IHgt;
	stDtiDrvPara.u32ZmeFrmWOut = stGfxRect.u32OWth;
	stDtiDrvPara.u32ZmeFrmHOut = stGfxRect.u32OHgt;
	
	OPTM_ALG_GDtiSet(&stDtiDrvPara, &stDtiRtlPara);

	if (stZmeDrvPara.u32ZmeFrmWIn > SHARPEN_MAX_WIDTH)
	{
		stZmeRtlPara.bZmeEnH = HI_FALSE;
		stZmeRtlPara.bZmeEnV = HI_FALSE;
	}

	/*zme enable horizontal*/	
	OPTM_VDP_GP_SetZmeEnable(enGpHalId, VDP_ZME_MODE_HOR, stZmeRtlPara.bZmeEnH);
	OPTM_VDP_GP_SetZmeEnable(enGpHalId, VDP_ZME_MODE_VER, stZmeRtlPara.bZmeEnV);

	if (IS_SLAVER_GP(enGpId))
	{
		bSlvGp     = HI_TRUE;
		//printf("==bSlvGp HI_TRUE==");
	}

	bGfxSharpen = HI_TRUE;
	
	if (stDtiDrvPara.u32ZmeFrmWIn*2/stDtiDrvPara.u32ZmeFrmWOut > SHARPEN_RATIO
		|| stDtiDrvPara.u32ZmeFrmHIn*2/stDtiDrvPara.u32ZmeFrmHOut > SHARPEN_RATIO)
	{
		bGfxSharpen = HI_FALSE;
	}

	if (stZmeRtlPara.bZmeEnH || stZmeRtlPara.bZmeEnV)
	{
		//printf("==stZmeRtlPara.bZmeEnH || stZmeRtlPara.bZmeEnV==");
		OPTM_VDP_GP_SetIpOrder(enGpHalId, bSlvGp, VDP_GP_ORDER_CSC_ZME);
		/*GP0 sharpen is forced to open,  set hfir_order V_H */
		OPTM_VDP_GP_SetZmeHfirOrder(enGpHalId, VDP_ZME_ORDER_HV);

		OPTM_VDP_GP_SetZmeCoefAddr (enGpId, VDP_GP_PARA_ZME_HOR, stZmeRtlPara.u32ZmeCoefAddrHL);
		/*set zme mode of horizontal luma and chroma*/
		OPTM_VDP_GP_SetZmeFirEnable(enGpId, VDP_ZME_MODE_HOR, stZmeRtlPara.bZmeMdHLC);
		/*set zme mode of horizontal alpha*/
		OPTM_VDP_GP_SetZmeFirEnable(enGpId, VDP_ZME_MODE_ALPHA, stZmeRtlPara.bZmeMdHA);		

		OPTM_VDP_GP_SetZmeMidEnable(enGpId, VDP_ZME_MODE_ALPHA, stZmeRtlPara.bZmeMedHA);
		OPTM_VDP_GP_SetZmeMidEnable(enGpId, VDP_ZME_MODE_HORL, stZmeRtlPara.bZmeMedHL);
		OPTM_VDP_GP_SetZmeMidEnable(enGpId, VDP_ZME_MODE_HORC, stZmeRtlPara.bZmeMedHC);

		OPTM_VDP_GP_SetZmePhase    (enGpId, VDP_ZME_MODE_HORL, stZmeRtlPara.s32ZmeOffsetHL);
		OPTM_VDP_GP_SetZmePhase    (enGpId, VDP_ZME_MODE_HORC, stZmeRtlPara.s32ZmeOffsetHC);

		OPTM_VDP_GP_SetZmeHorRatio(enGpId, stZmeRtlPara.u32ZmeRatioHL);

		OPTM_VDP_GP_SetZmeCoefAddr (enGpId, VDP_GP_PARA_ZME_VER, stZmeRtlPara.u32ZmeCoefAddrVL);
		/*set zme mode of horizontal luma and chroma*/
		OPTM_VDP_GP_SetZmeFirEnable(enGpId, VDP_ZME_MODE_VER, stZmeRtlPara.bZmeMdVLC);
		/*set zme mode of horizontal alpha*/
		OPTM_VDP_GP_SetZmeFirEnable(enGpId, VDP_ZME_MODE_ALPHAV, stZmeRtlPara.bZmeMdVA);

		OPTM_VDP_GP_SetZmeMidEnable(enGpId, VDP_ZME_MODE_ALPHAV, stZmeRtlPara.bZmeMedVA);
		OPTM_VDP_GP_SetZmeMidEnable(enGpId, VDP_ZME_MODE_VERL, stZmeRtlPara.bZmeMedVL);
		OPTM_VDP_GP_SetZmeMidEnable(enGpId, VDP_ZME_MODE_VERC, stZmeRtlPara.bZmeMedVC);

		OPTM_VDP_GP_SetZmePhase    (enGpId, VDP_ZME_MODE_VERL, stZmeRtlPara.s32ZmeOffsetVBtm);
		OPTM_VDP_GP_SetZmePhase    (enGpId, VDP_ZME_MODE_VERC, stZmeRtlPara.s32ZmeOffsetVTop);

		OPTM_VDP_GP_SetZmeVerRatio(enGpId, stZmeRtlPara.u32ZmeRatioVL);

		if (OPTM_DISPCHANNEL_1 == g_stGfxGPDevice[enGpId].enDispCh
			&& bGfxSharpen)
		{ 
			/*GP0 sharpen is forced to open */
			OPTM_VDP_GP_SetTiEnable(enGpId, VDP_TI_MODE_CHM, stDtiRtlPara.bEnCTI);
			OPTM_VDP_GP_SetTiEnable(enGpId, VDP_TI_MODE_LUM, stDtiRtlPara.bEnLTI);
			/*GP0 sharpen is forced to open,  set hfir_order V_H */
			OPTM_VDP_GP_SetZmeHfirOrder(enGpHalId, VDP_ZME_ORDER_VH);
		}
		else
		{  
			/*GP1 sharpen is forced to close */
			OPTM_VDP_GP_SetTiEnable(enGpId, VDP_TI_MODE_CHM, HI_FALSE);
			OPTM_VDP_GP_SetTiEnable(enGpId, VDP_TI_MODE_LUM, HI_FALSE);
		}
	 	
		OPTM_VDP_GP_SetTiHpCoef(enGpId, VDP_TI_MODE_CHM, (HI_S32 *)stDtiRtlPara.s32CTIHPTmp);
		OPTM_VDP_GP_SetTiHpCoef(enGpId, VDP_TI_MODE_LUM, (HI_S32 *)stDtiRtlPara.s32LTIHPTmp);
		
		OPTM_VDP_GP_SetTiGainRatio(enGpId, VDP_TI_MODE_CHM, (HI_S32)stDtiRtlPara.s16CTICompsatRatio);
		OPTM_VDP_GP_SetTiGainRatio(enGpId, VDP_TI_MODE_LUM, (HI_S32)stDtiRtlPara.s16LTICompsatRatio);
		
		OPTM_VDP_GP_SetTiCoringThd(enGpId, VDP_TI_MODE_CHM, (HI_U32)stDtiRtlPara.u16CTICoringThrsh);
		OPTM_VDP_GP_SetTiCoringThd(enGpId, VDP_TI_MODE_LUM, (HI_U32)stDtiRtlPara.u16LTICoringThrsh);
		
		OPTM_VDP_GP_SetTiSwingThd(enGpId, VDP_TI_MODE_CHM, (HI_U32)stDtiRtlPara.u16CTIOverSwingThrsh, (HI_U32)stDtiRtlPara.u16CTIUnderSwingThrsh);
		OPTM_VDP_GP_SetTiSwingThd(enGpId, VDP_TI_MODE_LUM, (HI_U32)stDtiRtlPara.u16LTIOverSwingThrsh, (HI_U32)stDtiRtlPara.u16LTIUnderSwingThrsh);
		
		OPTM_VDP_GP_SetTiMixRatio(enGpId, VDP_TI_MODE_CHM, (HI_U32)stDtiRtlPara.u8CTIMixingRatio);
		OPTM_VDP_GP_SetTiMixRatio(enGpId, VDP_TI_MODE_LUM, (HI_U32)stDtiRtlPara.u8LTIMixingRatio);
		
		OPTM_VDP_GP_SetTiHfThd(enGpId, VDP_TI_MODE_LUM, (HI_U32 *)stDtiRtlPara.u32LTIHFreqThrsh);
		OPTM_VDP_GP_SetTiGainCoef(enGpId, VDP_TI_MODE_LUM, (HI_U32 *)stDtiRtlPara.u32LTICompsatMuti);	
	}
	else
	{
		OPTM_VDP_GP_SetIpOrder(enGpHalId, bSlvGp, VDP_GP_ORDER_CSC);
	}	
	/************************/

	OPTM_VDP_GP_SetRegUp  (enGpHalId);
	OPTM_VDP_GP_SetParaUpd(enGpHalId,VDP_ZME_MODE_HOR);
    OPTM_VDP_GP_SetParaUpd(enGpHalId,VDP_ZME_MODE_VER);

	/**************************************/
	u32LayerCount = (OPTM_GFX_GP_0 == enGpId) ? OPTM_GP0_GFX_COUNT : OPTM_GP1_GFX_COUNT;
	
	if (OPTM_GFX_GP_0 == enGpId)
	{
		enInitLayerId     = HIFB_LAYER_HD_0;
	}
	else
	{
		enInitLayerId    = HIFB_LAYER_SD_0;
	}

	/****when gp_inrect changed, reset all gfx's inrect******/
	for (i = 0; i < u32LayerCount;i++)
	{
		enLayerId = enInitLayerId + i;
		
		if (!g_stGfxDevice[enLayerId].bOpened)
		{
			continue;
		}
				
		OPTM_GfxSetLayerRect(enLayerId, &g_stGfxDevice[enLayerId].stInRect);
	}

	/**************WBC_GP_INRECT == GP_INRECT******/
	if (IS_MASTER_GP(enGpId))
	{
		//printk("set slv inrect flag true..\n");
		g_stGfxGPDevice[OPTM_SLAVER_GPID].stInRect.s32Width  = g_stGfxGPDevice[enGpId].stInRect.s32Width;
		g_stGfxGPDevice[OPTM_SLAVER_GPID].stInRect.s32Height = g_stGfxGPDevice[enGpId].stInRect.s32Height;
	   
		g_stGfxGPDevice[OPTM_SLAVER_GPID].unUpFlag.bits.InRect = 1;
		g_stGfxGPDevice[OPTM_SLAVER_GPID].unUpFlag.bits.RegUp  = 1;
	}
	
	PRINT_OUT;
#endif

	return HI_SUCCESS;
}

HI_S32 OPTM_GfxGetOutRect(OPTM_GFX_GP_E enGpId, HIFB_RECT * pstOutputRect)
{
	PRINT_IN;

	pstOutputRect->x = 0;
	pstOutputRect->y = 0;
	pstOutputRect->w = g_stGfxGPDevice[enGpId].stInRect.s32Width;
	pstOutputRect->h = g_stGfxGPDevice[enGpId].stInRect.s32Height;
	
	PRINT_OUT;

	return HI_SUCCESS;
}

HI_S32 OPTM_GfxSetLayKeyMask(HIFB_LAYER_ID_E enLayerId, const HIFB_COLORKEYEX_S *pstColorkey)
{
#if 1
#warning TODO
#else
    OPTM_VDP_GFX_CKEY_S ckey_info;
    OPTM_VDP_GFX_MASK_S ckey_mask;  

    PRINT_IN;

	D_OPTM_HIFB_CheckGfxOpen(enLayerId);

	memset(&ckey_info, 0, sizeof(ckey_info));
	memset(&ckey_mask, 0, sizeof(ckey_mask));

	memcpy(&g_stGfxDevice[enLayerId].stColorkey, pstColorkey, sizeof(HIFB_COLORKEYEX_S));

	OPTM_CheckGPMask_BYLayerID(enLayerId);

    ckey_info.bKeyMode  = pstColorkey->u32KeyMode;
   
	if (g_stGfxDevice[enLayerId].CscState == OPTM_CSC_SET_PARA_BGR)
	{
		ckey_info.u32Key_r_min = pstColorkey->u8BlueMin;
		ckey_info.u32Key_g_min = pstColorkey->u8GreenMin;
		ckey_info.u32Key_b_min = pstColorkey->u8RedMin;

		ckey_info.u32Key_r_max = pstColorkey->u8BlueMax;
		ckey_info.u32Key_g_max = pstColorkey->u8GreenMax;
		ckey_info.u32Key_b_max = pstColorkey->u8RedMax;

		ckey_mask.u32Mask_r = pstColorkey->u8BlueMask;
		ckey_mask.u32Mask_g = pstColorkey->u8GreenMask;
		ckey_mask.u32Mask_b = pstColorkey->u8RedMask;
	}
	else
	{
		ckey_info.u32Key_r_min = pstColorkey->u8RedMin;
		ckey_info.u32Key_g_min = pstColorkey->u8GreenMin;
		ckey_info.u32Key_b_min = pstColorkey->u8BlueMin;

		ckey_info.u32Key_r_max = pstColorkey->u8RedMax;
		ckey_info.u32Key_g_max = pstColorkey->u8GreenMax;
		ckey_info.u32Key_b_max = pstColorkey->u8BlueMax;

		ckey_mask.u32Mask_r = pstColorkey->u8RedMask;
		ckey_mask.u32Mask_g = pstColorkey->u8GreenMask;
		ckey_mask.u32Mask_b = pstColorkey->u8BlueMask;
	}

	/*  set key threshold / mode / enable of graphic layer */
	OPTM_VDP_GFX_SetKeyMask(g_stGfxDevice[enLayerId].enGfxHalId, ckey_mask);
	OPTM_VDP_GFX_SetColorKey(g_stGfxDevice[enLayerId].enGfxHalId, pstColorkey->bKeyEnable, ckey_info);
	OPTM_VDP_GFX_SetRegUp(g_stGfxDevice[enLayerId].enGfxHalId);

	PRINT_OUT;
#endif

	return HI_SUCCESS;
}

/*  superposition */
HI_S32 OPTM_GfxSetLayerPreMult(HIFB_LAYER_ID_E enLayerId, HI_BOOL bEnable)
{	
#if 1
#warning TODO
#else
	OPTM_GFX_GP_E enGpId;
    PRINT_IN;
	
	enGpId = g_stGfxDevice[enLayerId].enGPId;
	g_stGfxDevice[enLayerId].bPreMute = bEnable;

	OPTM_CheckGPMask_BYGPID(enGpId);
	
	OPTM_VDP_GFX_SetPreMultEnable(g_stGfxDevice[enLayerId].enGfxHalId, bEnable);
#if 0	
	if (IS_MASTER_GP(enGpId))
	{
		HIFB_LAYER_ID_E enSlvGfxId = HIFB_LAYER_SD_0;
		
		g_stGfxDevice  [enSlvGfxId].bPreMute                 = bEnable;	    
		g_stGfxGPDevice[OPTM_SLAVER_GPID].unUpFlag.bits.PreMute = 1;
		g_stGfxGPDevice[OPTM_SLAVER_GPID].unUpFlag.bits.RegUp   = 1;
	}
#endif
	PRINT_OUT;
#endif
	
    return HI_SUCCESS;
}

HI_S32  OPTM_GfxSetClutAddr(HIFB_LAYER_ID_E enLayerId, HI_U32 u32PhyAddr)
{
#if 1
#warning TODO
#else
	PRINT_IN;

    if (HI_NULL == u32PhyAddr)
    {
        return HI_FAILURE;
    }
	
    OPTM_VDP_GFX_SetLutAddr(g_stGfxDevice[enLayerId].enGfxHalId, u32PhyAddr);
    OPTM_VDP_GFX_SetParaUpd(g_stGfxDevice[enLayerId].enGfxHalId,VDP_DISP_COEFMODE_LUT);

	PRINT_OUT;
#endif
	
    return HI_SUCCESS;
}

HI_S32 OPTM_GfxGetOSDData(HIFB_LAYER_ID_E enLayerId, HIFB_OSD_DATA_S *pstLayerData)
{
#if 1
#warning TODO
#else
	HI_U32 u32Enable;
	HI_U32 u32KeyEnable;
	HI_U32 alpharange;
    OPTM_GFX_GP_E enGPId;
    OPTM_VDP_DISP_RECT_S  stRect;    
	OPTM_VDP_DISP_RECT_S stInRect;
	OPTM_VDP_GFX_MASK_S  stckey_mask;
	OPTM_VDP_GFX_CKEY_S  stKey;
	OPTM_VDP_GFX_IFMT_E  enDataFmt;
	
	PRINT_IN;

	u32Enable = HI_FALSE;
    enGPId    = OPTM_GFX_GP_1;	
	enDataFmt = VDP_GFX_IFMT_BUTT;
	u32KeyEnable= HI_FALSE;
	
    if(HIFB_LAYER_SD_0 > enLayerId)
	{
		enGPId = OPTM_GFX_GP_0;
	}
	
	OPTM_VDP_GFX_GetLayerEnable(enLayerId, &u32Enable);

	if (u32Enable)
	{
		pstLayerData->eState = HIFB_LAYER_STATE_ENABLE;
	}
	else
	{
		pstLayerData->eState = HIFB_LAYER_STATE_DISABLE;
	}
	
	pstLayerData->u32BufferPhyAddr = g_stGfxDevice[enLayerId].NoCmpBufAddr;

	OPTM_VDP_GFX_GetLayerAddr(enLayerId, &pstLayerData->u32RegPhyAddr);
	
	OPTM_VDP_GFX_GetLayerStride(enLayerId, &pstLayerData->u32Stride);

    OPTM_VDP_GFX_GetLayerInRect(OPTM_GetGfxHalId(enLayerId), &stInRect);
    pstLayerData->stInRect.x = stInRect.u32DXS; 
    pstLayerData->stInRect.y = stInRect.u32DYS; 
    pstLayerData->stInRect.w = stInRect.u32IWth;
    pstLayerData->stInRect.h = stInRect.u32IHgt;

    OPTM_VDP_GP_GetRect(enGPId, &stRect);

	pstLayerData->stOutRect.x = 0;
	pstLayerData->stOutRect.y = 0;
	pstLayerData->stOutRect.w = stRect.u32IWth;
	pstLayerData->stOutRect.h = stRect.u32IHgt;

	pstLayerData->u32ScreenWidth = stRect.u32OWth;
	pstLayerData->u32ScreenHeight= stRect.u32OHgt;

	OPTM_VDP_WBC_GetEnable(OPTM_VDP_LAYER_WBC_GP0, &u32Enable);

	if (u32Enable)
	{
		pstLayerData->eGfxWorkMode = HIFB_GFX_MODE_HD_WBC;
	}
	else
	{
		pstLayerData->eGfxWorkMode = HIFB_GFX_MODE_NORMAL;
	}
	
	OPTM_VDP_GFX_GetPalpha(enLayerId, &pstLayerData->stAlpha.bAlphaEnable, 
							&alpharange,&pstLayerData->stAlpha.u8Alpha0,&pstLayerData->stAlpha.u8Alpha1);
	OPTM_VDP_GFX_GetLayerGalpha(enLayerId, &pstLayerData->stAlpha.u8GlobalAlpha);

	OPTM_VDP_GFX_GetKeyMask (enLayerId, &stckey_mask);
	OPTM_VDP_GFX_GetColorKey(enLayerId, &u32KeyEnable,&stKey);
	
	pstLayerData->stColorKey.u8RedMask   = stckey_mask .u32Mask_r;
	pstLayerData->stColorKey.u8GreenMask = stckey_mask .u32Mask_g;
	pstLayerData->stColorKey.u8BlueMask  = stckey_mask .u32Mask_b;

	pstLayerData->stColorKey.bMaskEnable= HI_TRUE;
	pstLayerData->stColorKey.bKeyEnable = u32KeyEnable;
	pstLayerData->stColorKey.u32KeyMode = stKey.bKeyMode;
	
	pstLayerData->stColorKey.u8RedMax   = stKey.u32Key_r_max;
	pstLayerData->stColorKey.u8GreenMax = stKey.u32Key_g_max;
	pstLayerData->stColorKey.u8BlueMax  = stKey.u32Key_b_max;

	pstLayerData->stColorKey.u8RedMin   = stKey.u32Key_r_min;
	pstLayerData->stColorKey.u8GreenMin = stKey.u32Key_g_min;
	pstLayerData->stColorKey.u8BlueMin  = stKey.u32Key_b_min;

	OPTM_VDP_GFX_GetPreMultEnable(enLayerId, &pstLayerData->bPreMul);

	OPTM_VDP_GFX_GetInDataFmt(enLayerId, &enDataFmt);

	pstLayerData->eFmt = OPTM_HalFmtTransferToPixerFmt(enDataFmt);
	
	PRINT_OUT;
#endif
    return HI_SUCCESS;
}

HI_S32 OPTM_GfxUpLayerReg(HIFB_LAYER_ID_E enLayerId)
{	
#if 1
#warning TODO
#else
	OPTM_GFX_GP_E enGpId;
	
	enGpId = g_stGfxDevice[enLayerId].enGPId;

    OPTM_VDP_GFX_SetRegUp(g_stGfxDevice[enLayerId].enGfxHalId);
	OPTM_VDP_GP_SetRegUp (g_stGfxGPDevice[g_stGfxDevice[enLayerId].enGPId].enGpHalId);

	
    if (IS_MASTER_GP(enGpId))
    {
        g_stGfxGPDevice[OPTM_SLAVER_GPID].unUpFlag.bits.RegUp = 1;
    }
#endif
	
    return HI_SUCCESS;
}

HI_S32 OPTM_GFX_OpenWbc2(OPTM_GFX_WBC_S *pstWbc2)
{
#warning TODO: OPTM_GFX_OpenWbc2
	OPTM_VDP_DISP_RECT_S stWbcGpRect;
	
	PRINT_IN;
	
    if (HI_TRUE == pstWbc2->bOpened)
    {
	    return HI_SUCCESS;
    }

#if 0
    if (Data_81143414 == 0)
    {
    	//->805e1ea0
        pstWbc2->s32BufferWidth  = OPTM_GFX_WBC_WIDTH;
        pstWbc2->s32BufferHeight = OPTM_GFX_WBC_HEIGHT;
        pstWbc2->u32BufferStride = pstWbc2->s32BufferWidth * OPTM_GFXDATA_DEFAULTBYTES;
    }
    else
    {
		/* apply clut table buffer */
		pstWbc2->s32BufferWidth  = 1920; //OPTM_GFX_WBC_WIDTH;
		pstWbc2->s32BufferHeight = 1200; //OPTM_GFX_WBC_HEIGHT;
		pstWbc2->u32BufferStride = pstWbc2->s32BufferWidth * OPTM_GFXDATA_DEFAULTBYTES;
    }
#else
    switch (Data_81143414)
    {
    case 0:
        pstWbc2->s32BufferWidth  = OPTM_GFX_WBC_WIDTH;
        pstWbc2->s32BufferHeight = OPTM_GFX_WBC_HEIGHT;
        pstWbc2->u32BufferStride = pstWbc2->s32BufferWidth * OPTM_GFXDATA_DEFAULTBYTES;
    	break;
    default:
		pstWbc2->s32BufferWidth  = 1920; //OPTM_GFX_WBC_WIDTH;
		pstWbc2->s32BufferHeight = 1200; //OPTM_GFX_WBC_HEIGHT;
		pstWbc2->u32BufferStride = pstWbc2->s32BufferWidth * OPTM_GFXDATA_DEFAULTBYTES;
    	break;
    }
#endif

#if 0
	if (pstWbc2->stFrameBuffer.u32StartVirAddr == 0 &&
		pstWbc2->stFrameBuffer.u32StartPhyAddr == 0)
#else
	if (pstWbc2->stFrameBuffer.u32StartVirAddr == 0)
#endif
	{
		if (OPTM_AllocAndMap(OPTM_GFX_WBC2_BUFFER, HI_NULL, 
		    pstWbc2->u32BufferStride * pstWbc2->s32BufferHeight * 2,
		    0, &(pstWbc2->stFrameBuffer)) != 0)
		{
		    HIFB_ERROR("GFX Get wbc2 buffer failed!\n"); //3845
		    return HI_FAILURE;
		}
	}

	pstWbc2->Data_24 = 0;
	pstWbc2->Data_28 = pstWbc2->Data_36 = pstWbc2->stFrameBuffer.u32StartPhyAddr;
	pstWbc2->Data_32 = pstWbc2->Data_40 = pstWbc2->stFrameBuffer.u32StartPhyAddr + pstWbc2->u32BufferStride * pstWbc2->s32BufferHeight;

	memset((HI_U8 *)(pstWbc2->stFrameBuffer.u32StartVirAddr), 0, pstWbc2->u32BufferStride * pstWbc2->s32BufferHeight * 2);

    pstWbc2->bEnable = HI_FALSE;
    pstWbc2->enWbcHalId = OPTM_VDP_LAYER_WBC_GP0;
	/* 0, feeder; others, reserve */
    pstWbc2->u32DataPoint = 0; 
	/*the data fmt set for gfx4*/
    pstWbc2->enDataFmt = HIFB_FMT_AYUV8888;    
    
	memset(&stWbcGpRect, 0, sizeof(stWbcGpRect));

	if (Data_81143414 == 0)
	{
#if 0
		stWbcGpRect.u32IWth= OPTM_GFX_WBC_WIDTH;
		stWbcGpRect.u32IHgt= OPTM_GFX_WBC_HEIGHT;
		stWbcGpRect.u32OWth= OPTM_GFX_WBC_WIDTH;
		stWbcGpRect.u32OHgt= OPTM_GFX_WBC_HEIGHT;
#else
		printk("help\n");
		stWbcGpRect.u32DXL = OPTM_GFX_WBC_WIDTH;
		stWbcGpRect.u32DYL = OPTM_GFX_WBC_HEIGHT;

		stWbcGpRect.u32IWth= OPTM_GFX_WBC_WIDTH;
		stWbcGpRect.u32IHgt= OPTM_GFX_WBC_HEIGHT;
		stWbcGpRect.u32OWth= stWbcGpRect.u32DXL;
#endif
	}
	else
	{
		stWbcGpRect.u32DXL = 1920;
		stWbcGpRect.u32DYL = 1200;

		stWbcGpRect.u32IWth= 1920;
		stWbcGpRect.u32IHgt= 1200;
		stWbcGpRect.u32OWth= stWbcGpRect.u32DXL;
	}
	stWbcGpRect.u32OHgt= stWbcGpRect.u32DYL;

    pstWbc2->enDitherMode = VDP_DITHER_TMP_SPA_8;
	pstWbc2->stWBCFmt     = VDP_WBC_OFMT_ARGB8888;
	pstWbc2->enReadMode   = VDP_RMODE_PROGRESSIVE;
	pstWbc2->enWbcMode    = OPTM_WBC_MODE_MONO;

	pstWbc2->Data_128 = 256;

	OPTM_VDP_WBC_SetThreeMd   (pstWbc2->enWbcHalId, pstWbc2->enWbcMode);
    OPTM_VDP_WBC_SetDitherMode(pstWbc2->enWbcHalId, pstWbc2->enDitherMode);
    OPTM_VDP_WBC_SetOutFmt    (pstWbc2->enWbcHalId, pstWbc2->stWBCFmt);   
	OPTM_VDP_WBC_SetOutIntf   (pstWbc2->enWbcHalId, pstWbc2->enReadMode);
    OPTM_VDP_WBC_SetRegUp     (pstWbc2->enWbcHalId);

    pstWbc2->bOpened = HI_TRUE;

    PRINT_OUT;
	
    return HI_SUCCESS;
}

HI_S32 OPTM_GFX_CloseWbc2(OPTM_GFX_WBC_S *pstWbc2)
{
#if 0
#warning TODO
#else
	PRINT_IN;
	
    if (pstWbc2->bOpened == HI_FALSE)
    {
        return HI_SUCCESS;
    }
    
    pstWbc2->bOpened = HI_FALSE;

	
	OPTM_VDP_WBC_SetEnable(pstWbc2->enWbcHalId, HI_FALSE);
	OPTM_VDP_WBC_SetRegUp (pstWbc2->enWbcHalId);
#if 0	
	if (pstWbc2->stFrameBuffer.u32StartVirAddr != 0)
	{
		OPTM_UnmapAndRelease(&(pstWbc2->stFrameBuffer));
		pstWbc2->stFrameBuffer.u32StartVirAddr = 0;
		pstWbc2->stFrameBuffer.u32StartPhyAddr = 0;
	}
#endif	
	PRINT_OUT;
#endif
	
    return HI_SUCCESS;
}

HI_S32 OPTM_GfxConfigSlvLayer(HIFB_LAYER_ID_E enLayerId,
                                        HI_RECT_S *pstRect)
{
#if 1
#warning TODO
#else
	HIFB_RECT        stRect;
	OPTM_GFX_GP_E    enGfxGpId;
	OPTM_GFX_GP_E    enMstGfxGpId;			
	OPTM_GFX_WBC_S   *pstWbc2;
	OPTM_VDP_DISP_RECT_S  stLayerRect;

	PRINT_IN;
	
	//stRect.x = pstRect->s32X;
	//stRect.y = pstRect->s32Y;
	stRect.x = 0;
	stRect.y = 0;
	stRect.w = pstRect->s32Width;
	stRect.h = pstRect->s32Height;

	pstWbc2      = &g_stGfxWbc2;
	enGfxGpId    = g_stGfxDevice[enLayerId].enGPId;	
	enMstGfxGpId = OPTM_MASTER_GPID;

	HIFB_INFO("enMstGfxGpId %d, in_w %d, in_h %d \n",enMstGfxGpId,g_stGfxGPDevice[enMstGfxGpId].stInRect.s32Width,
				g_stGfxGPDevice[enMstGfxGpId].stInRect.s32Height);
	/*when layer work in wbc mode, wbc_inRect=MstGP_inRect*/
	pstWbc2->stInRect.x = 0;
	pstWbc2->stInRect.y = 0;
	pstWbc2->stInRect.w = g_stGfxGPDevice[enMstGfxGpId].stInRect.s32Width;
	pstWbc2->stInRect.h = g_stGfxGPDevice[enMstGfxGpId].stInRect.s32Height;

	/*wbc_outRect was decided by the disp format size or user's adjustRect*/
	pstWbc2->stOutRect.x = pstRect->s32X;
	pstWbc2->stOutRect.y = pstRect->s32Y;
	pstWbc2->stOutRect.w = pstRect->s32Width;
	pstWbc2->stOutRect.h = pstRect->s32Height;

	g_stGfxGPDevice[enGfxGpId].stOutRect.s32X      = pstRect->s32X;
	g_stGfxGPDevice[enGfxGpId].stOutRect.s32Y      = pstRect->s32Y;
	g_stGfxGPDevice[enGfxGpId].stOutRect.s32Width  = pstRect->s32Width;
	g_stGfxGPDevice[enGfxGpId].stOutRect.s32Height = pstRect->s32Height;	

	memset(&stLayerRect, 0, sizeof(stLayerRect));

    stLayerRect.u32IWth= pstWbc2->stInRect.w & 0xfffffffe;
    stLayerRect.u32IHgt= pstWbc2->stInRect.h & 0xfffffffc;
	/*crop for source of gp0 data*/
    stLayerRect.u32DXL = stLayerRect.u32DXS + stLayerRect.u32IWth;
    stLayerRect.u32DYL = stLayerRect.u32DXS + stLayerRect.u32IHgt;

    stLayerRect.u32OWth= pstRect->s32Width & 0xfffffffe;
    stLayerRect.u32OHgt= pstRect->s32Height & 0xfffffffc;
	
	OPTM_CheckGPMask_BYGPID(enGfxGpId);

	OPTM_VDP_WBC_SetLayerReso(pstWbc2->enWbcHalId, stLayerRect);   
    OPTM_VDP_WBC_SetCropReso (pstWbc2->enWbcHalId, stLayerRect);	

	OPTM_GfxSetLayerRect(enLayerId, &stRect);
	
	//OPTM_GfxSetGpRect(enGfxGpId, &pstWbc2->stInRect);
	OPTM_GfxSetGpRect(enGfxGpId, &stRect);
	
	OPTM_VDP_GFX_SetRegUp(g_stGfxDevice[enLayerId].enGfxHalId);
	OPTM_VDP_GP_SetRegUp (enGfxGpId);
	OPTM_VDP_WBC_SetRegUp(pstWbc2->enWbcHalId);

	PRINT_OUT;
#endif
	
    return HI_SUCCESS;
}

HI_VOID OPTM_Wbc2Isr(HI_VOID* u32Param0, HI_VOID* u32Param1)
{    
#if 1
#warning TODO
#else
	HIFB_LAYER_ID_E enLayerId;
	OPTM_GFX_GP_E   enGfxGpId;
	OPTM_GFX_GP_S   *pGfxGp;
	OPTM_GFX_WBC_S   *pstWbc2;

	pstWbc2     = &g_stGfxWbc2;
	enGfxGpId   = *((OPTM_GFX_GP_E*)u32Param0);
	enLayerId   = HIFB_LAYER_SD_0;
	pGfxGp      = &g_stGfxGPDevice[enGfxGpId];

	if (pGfxGp->unUpFlag.bits.RegUp && !g_stGfxGPDevice[enGfxGpId].bMaskFlag)
	{
        if (pGfxGp->unUpFlag.bits.Enable)
        {
        	OPTM_VDP_GFX_SetLayerEnable(g_stGfxDevice[enLayerId].enGfxHalId, g_stGfxDevice[enLayerId].bEnable);
			OPTM_VDP_WBC_SetEnable     (pstWbc2->enWbcHalId,                 g_stGfxDevice[enLayerId].bEnable);
            //OPTM_VDP_GFX_SetLayerAddrEX(g_stGfxDevice[enLayerId].enGfxHalId, g_stGfxWbc2.stFrameBuffer.u32StartPhyAddr);
			pGfxGp->unUpFlag.bits.Enable = 0;
        }

		if (pGfxGp->unUpFlag.bits.InRect)
        {
			OPTM_GfxConfigSlvLayer(enLayerId, &pGfxGp->stOutRect);
			pGfxGp->unUpFlag.bits.InRect = 0;
        }

		if (pGfxGp->unUpFlag.bits.Alpha)
        {
        	HIFB_ALPHA_S *pstAlpha;
			pstAlpha = &g_stGfxDevice[enLayerId].stAlpha;
			OPTM_VDP_GFX_SetPalpha(g_stGfxDevice[enLayerId].enGfxHalId,pstAlpha->bAlphaEnable,HI_TRUE,pstAlpha->u8Alpha0,pstAlpha->u8Alpha1);
			OPTM_VDP_GFX_SetLayerGalpha(g_stGfxDevice[enLayerId].enGfxHalId, pstAlpha->u8GlobalAlpha);
			pGfxGp->unUpFlag.bits.Alpha = 0;
        }

		if (pGfxGp->unUpFlag.bits.PreMute)
        {
			OPTM_VDP_GFX_SetPreMultEnable(g_stGfxDevice[enLayerId].enGfxHalId, g_stGfxDevice[enLayerId].bPreMute);
			pGfxGp->unUpFlag.bits.PreMute = 0;
        }

		if (pGfxGp->unUpFlag.bits.WbcMode)
        {
			OPTM_VDP_WBC_SetThreeMd(pstWbc2->enWbcHalId, (HI_U32)pstWbc2->enWbcMode);
			pGfxGp->unUpFlag.bits.WbcMode = 0;
        }

		OPTM_VDP_GFX_SetRegUp(g_stGfxDevice[enLayerId].enGfxHalId);
		OPTM_VDP_GP_SetRegUp (enGfxGpId);
		OPTM_VDP_WBC_SetRegUp(pstWbc2->enWbcHalId);
		
		pGfxGp->unUpFlag.bits.RegUp = 0;
	}

	OPTM_GfxWVBCallBack(enLayerId, HI_NULL);
#endif
	
	return;
}

HI_S32 OPTM_GfxSetTriDimEnable(HIFB_LAYER_ID_E enLayerId, HI_U32 bEnable)
{
#if 1
#warning TODO
#else
	PRINT_IN;

	g_stGfxDevice[enLayerId].b3DEnable = bEnable;

	OPTM_CheckGPMask_BYLayerID(enLayerId);

	OPTM_VDP_GFX_SetThreeDimEnable(enLayerId, bEnable);
		
	PRINT_OUT;
#endif
	
    return HI_SUCCESS;
}

OPTM_VDP_DISP_MODE_E OPTM_GfxGetHalTriDimMode(HIFB_STEREO_MODE_E enMode)
{
	switch(enMode)
	{
		case HIFB_STEREO_MONO:
			return VDP_DISP_MODE_2D;
		case HIFB_STEREO_SIDEBYSIDE_HALF:
			return VDP_DISP_MODE_SBS;
		case HIFB_STEREO_TOPANDBOTTOM:
			return VDP_DISP_MODE_TAB;
		case HIFB_STEREO_FRMPACKING:
			return VDP_DISP_MODE_FP;
		default:
			return VDP_DISP_MODE_BUTT;
	}

	return VDP_DISP_MODE_BUTT;
}

HI_S32 OPTM_GfxSetTriDimMode(HIFB_LAYER_ID_E enLayerId, HIFB_STEREO_MODE_E enMode)
{	
#if 1
#warning TODO
#else
	HI_U32 u32GpId;
    PRINT_IN;

	u32GpId = (HI_U32)(g_stGfxDevice[enLayerId].enGPId);

	if (u32GpId >= (HI_U32)OPTM_GFX_GP_BUTT)
	{
		return HI_FAILURE;
	}
	
	g_stGfxDevice[enLayerId].enTriDimMode = enMode;

	OPTM_CheckGPMask_BYGPID(u32GpId);

	OPTM_VDP_GFX_SetDispMode(enLayerId, OPTM_GfxGetHalTriDimMode(enMode));
	
	/*for test*/

	g_stGfxGPDevice[u32GpId].enTriDimMode = enMode;
	
	OPTM_VDP_GP_SetDispMode(g_stGfxDevice[enLayerId].enGPId, OPTM_GfxGetHalTriDimMode(enMode));
	OPTM_VDP_GP_SetRegUp   (g_stGfxDevice[enLayerId].enGPId);
		
	PRINT_OUT;
#endif
	
    return HI_SUCCESS;
}

HI_S32 OPTM_GfxSetTriDimAddr(HIFB_LAYER_ID_E enLayerId, HI_U32 u32TriDimAddr)
{	
#if 1
#warning TODO
#else
    PRINT_IN;

	g_stGfxDevice[enLayerId].u32TriDimAddr= u32TriDimAddr;

	OPTM_CheckGPMask_BYLayerID(enLayerId);

	OPTM_VDP_GFX_SetLayerNAddr(enLayerId, u32TriDimAddr);
		
	PRINT_OUT;
#endif
	
    return HI_SUCCESS;
}



HI_S32 OPTM_GfxSetLayerDeCmpEnable(HIFB_LAYER_ID_E enLayerId, HI_BOOL bEnable)
{
    return HI_SUCCESS;
}


HI_S32 OPTM_GfxSetGfxCmpAddr(HIFB_LAYER_ID_E enLayerId, HI_U32 u32CmpAddr)
{	
    return HI_SUCCESS;
}


HI_S32 OPTM_GfxSetGfxCmpRect(HIFB_LAYER_ID_E enLayerId, const HIFB_RECT *pstRect)
{
    return HI_SUCCESS;
}

/*this function called in the vysnc isr, handle cmp and nocmp buffer changing.*/
HI_S32 OPTM_GfxGfxCmpHandle(HIFB_LAYER_ID_E enLayerId)
{
    return HI_SUCCESS;
}


/********************************************* 
	 when change resolution or stride or address value,  
         we should change from cmp mode to no cmp mode,
         and WBC3 do a over scape update write
*********************************************/

HI_S32 OPTM_GfxChgCmp2Ncmp(HIFB_LAYER_ID_E enLayerId)
{
    return HI_SUCCESS;
}


/*set the priority of layer in gp*/
/*CNcomment:����ͼ����GP �е����ȼ�*/
HI_S32 OPTM_GfxGetLayerPriority(HIFB_LAYER_ID_E enLayerId, HI_U32 *pU32Priority)
{
#if 1
#warning TODO
#else
	HI_U32 i;
	HI_U32 u32prio;
	HI_U32 u32Index;
	HI_U32 u32LayerPrio;
	HI_U32 u32LayerIdIndex;
	OPTM_VDP_CBM_MIX_E eCbmMixg;
	
	u32Index = (g_stGfxDevice[enLayerId].enGPId == OPTM_GFX_GP_0) ? OPTM_GP0_GFX_COUNT : OPTM_GP1_GFX_COUNT;
	eCbmMixg = (g_stGfxDevice[enLayerId].enGPId == OPTM_GFX_GP_0) ? VDP_CBM_MIXG0 : VDP_CBM_MIXG1;
	OPTM_VDP_CBM_GetMixerPrio(eCbmMixg, &u32prio);

	/**HIFB_LAYER_HD_0 -> 0
		HIFB_LAYER_HD_1 -> 1
		HIFB_LAYER_SD_0 -> 0
		HIFB_LAYER_SD_1 -> 1*/
	u32LayerIdIndex =  enLayerId - OPTM_GP0_GFX_COUNT * g_stGfxDevice[enLayerId].enGPId;

	for (i = 0; i < u32Index; i++)
	{
		u32LayerPrio = u32prio & 0xf;
		u32prio = u32prio >> 4;		
		if ((u32LayerPrio-1) == u32LayerIdIndex)
		{
			*pU32Priority = i+1;
			return HI_SUCCESS;
		}		
	}	

	HIFB_ERROR("info:fail to get z_order of gfx%d!\n",enLayerId);
#endif
	return HI_FAILURE;
}

/*set the priority of layer in gp*/
/*CNcomment:����ͼ����GP �е����ȼ�*/
HI_S32 OPTM_GfxSetLayerPriority(HIFB_LAYER_ID_E enLayerId, HIFB_ZORDER_E enZOrder)
{
#if 1
#warning TODO
#else
	HI_S32  i;
	HI_S32  j;
	HI_CHAR prio[4];
	HI_CHAR priotemp;
	HI_U32  m,n;
	HI_U32  u32Prio;
	HI_U32  u32ExcIndex;
	HI_U32  u32LayerPrio;
	OPTM_VDP_CBM_MIX_E eCbmMixg;
	
	PRINT_IN;
	
	u32LayerPrio = 0;
	OPTM_GfxGetLayerPriority(enLayerId, &u32LayerPrio);

	if (u32LayerPrio > 4 || u32LayerPrio < 1)
	{
		return HI_SUCCESS;
	}

	switch (enZOrder)
	{
		case HIFB_ZORDER_MOVEBOTTOM:
		{
			u32ExcIndex = 1;
			break;
		}	
		case HIFB_ZORDER_MOVEDOWN:
		{
			u32ExcIndex = u32LayerPrio - 1;
			break;
		}	
		case HIFB_ZORDER_MOVETOP:
		{
			u32ExcIndex = 4;
			break;
		}	
		case HIFB_ZORDER_MOVEUP:
		{
			u32ExcIndex = u32LayerPrio + 1;
			break;
		}	
		default:
		{
				return HI_FAILURE;
		}
	}
	
	if (u32ExcIndex > 4 || u32ExcIndex < 1)
	{
		return HI_SUCCESS;
	}

	eCbmMixg = (g_stGfxDevice[enLayerId].enGPId == OPTM_GFX_GP_0) ? VDP_CBM_MIXG0 : VDP_CBM_MIXG1;	
	OPTM_VDP_CBM_GetMixerPrio(eCbmMixg, &u32Prio);

	for (i = 0; i < 4; i++)
	{
		prio[i] = (u32Prio&0xf);
		u32Prio = u32Prio >> 4;
	}

	if (u32LayerPrio > u32ExcIndex)
	{
		j = -1;
	}
	else if (u32ExcIndex > u32LayerPrio)
	{
		j = 1;
	}
	else
	{	
		return HI_SUCCESS;
	}

	priotemp = prio[u32LayerPrio-1];
	m = u32LayerPrio-1;
	n = u32ExcIndex-1;

	if (m >=4 || n >= 4)
	{
		return HI_SUCCESS;
	}
	
	for (i = m; i != n;)
	{
		if ((i >= 4 || i < 0) || ((i+j) >= 4 || (i+j) < 0))
		{
			return HI_FAILURE;
		}
		
		prio[i] = prio[i+j];
		i += j;
	}

	prio[i] = priotemp;


	u32Prio = 0;
	for (i = 3; i >= 0; i--)
	{
		u32Prio = u32Prio << 4;
		u32Prio |= prio[i];		
	}

	u32Prio &= 0xffff;

	OPTM_VDP_SetMixgPrio(eCbmMixg, u32Prio);

	g_stGfxGPDevice[g_stGfxDevice[enLayerId].enGPId].u32Prior = u32Prio;
	
	PRINT_OUT;
#endif

	return HI_SUCCESS;
}

HI_S32 OPTM_GPMask(OPTM_VDP_LAYER_GP_E enGPId, HI_BOOL bFlag)
{
#if 1
#warning TODO
#else
	HI_U32                      i;
	HI_BOOL                     bEnable;
	HI_U32                      u32LayerCount;
	HIFB_LAYER_ID_E             enLayerId;
	OPTM_GFX_WBC_S              *pstWbc2;

	PRINT_IN;

	if (!g_stGfxGPDevice[enGPId].bOpen)
	{
		return HI_SUCCESS;
	}

	u32LayerCount = (OPTM_VDP_LAYER_GP0 == enGPId) ? OPTM_GP0_GFX_COUNT : OPTM_GP1_GFX_COUNT;
	
	if (OPTM_VDP_LAYER_GP0 == enGPId)
	{
		enLayerId   = HIFB_LAYER_HD_0;
	}
	else if (OPTM_VDP_LAYER_GP1 == enGPId)
	{
		enLayerId  = HIFB_LAYER_SD_0;
	}
	else
	{
		return HI_FAILURE;
	}

	g_stGfxGPDevice[enGPId].bMaskFlag = bFlag;
	
	if (IS_MASTER_GP(enGPId))
	{
		g_stGfxGPDevice[OPTM_SLAVER_GPID].bMaskFlag = bFlag;
	}
	
	if (IS_SLAVER_GP(enGPId))
	{
		/*****disable/enable WBC_GP0, Gfx4, Gp1*****/
		pstWbc2   = &g_stGfxWbc2;
		enLayerId = HIFB_LAYER_SD_0;
		bEnable   = bFlag ? HI_FALSE : g_stGfxDevice[enLayerId].bEnable;
		
		OPTM_VDP_GFX_SetLayerEnable(g_stGfxDevice[enLayerId].enGfxHalId, bEnable);
        OPTM_VDP_GFX_SetRegUp      (g_stGfxDevice[enLayerId].enGfxHalId);

		OPTM_VDP_WBC_SetEnable(pstWbc2->enWbcHalId, bEnable);
		OPTM_VDP_WBC_SetRegUp (pstWbc2->enWbcHalId);
	}	
	else
	{
		for (i = 0; i < u32LayerCount;i++)
		{
			if(!g_stGfxDevice[enLayerId+i].bOpened)
			{
				continue;
			}
			
			/*before disable layer, we need to save data.
	              when set layer mask true, we save operations of user in global variable. 
	              until layer mask become false, these operations will be setted to hardware.*/
			bEnable = bFlag ? HI_FALSE : g_stGfxDevice[enLayerId+i].bEnable;		        	
			OPTM_VDP_GFX_SetLayerEnable(g_stGfxDevice[enLayerId+i].enGfxHalId, bEnable);				
			OPTM_VDP_GFX_SetRegUp      (g_stGfxDevice[enLayerId+i].enGfxHalId);
		}

		if (IS_MASTER_GP(enGPId))
		{
			HIFB_LAYER_ID_E enSlvGfxId = HIFB_LAYER_SD_0;
			g_stGfxDevice[enSlvGfxId].bEnable = bFlag ? HI_FALSE : g_stGfxDevice[enSlvGfxId].bEnable;
			g_stGfxGPDevice[OPTM_SLAVER_GPID].unUpFlag.bits.RegUp  = 1;
			g_stGfxGPDevice[OPTM_SLAVER_GPID].unUpFlag.bits.Enable = 1;
		}
	}
	
	PRINT_OUT;
#endif

	return HI_SUCCESS;
}

HI_S32 OPTM_GPRecovery(OPTM_VDP_LAYER_GP_E enGPId)
{
#if 1
#warning TODO
#else
	HI_U32                      i;
	HI_U32                      u32LayerCount;
	HI_RECT_S                   *pstDispRect;
	HIFB_LAYER_ID_E             enInitLayerId;
	HIFB_LAYER_ID_E             enLayerId;
	
	PRINT_IN;

	if (!g_stGfxGPDevice[enGPId].bOpen)
	{
		return HI_SUCCESS;
	}
	
	OPTM_CheckGPMask_BYGPID(enGPId);
	
	u32LayerCount = (OPTM_VDP_LAYER_GP0 == enGPId) ? OPTM_GP0_GFX_COUNT : OPTM_GP1_GFX_COUNT;
	
	if (OPTM_VDP_LAYER_GP0 == enGPId)
	{
		enInitLayerId     = HIFB_LAYER_HD_0;
	}
	else if (OPTM_VDP_LAYER_GP1 == enGPId)
	{
		enInitLayerId    = HIFB_LAYER_SD_0;
	}
	else
	{
		return HI_FAILURE;
	}

	pstDispRect = &g_stGfxGPDevice[enGPId].stOutRect;

	OPTM_VDP_GP_SetLayerGalpha(enGPId, g_stGfxGPDevice[enGPId].u32Alpha);
	OPTM_VDP_CBM_SetMixerBkg  (g_stGfxGPDevice[enGPId].enMixg, g_stGfxGPDevice[enGPId].stBkg);
	OPTM_VDP_SetMixgPrio      (g_stGfxGPDevice[enGPId].enMixg, g_stGfxGPDevice[enGPId].u32Prior);
	
	if (IS_SLAVER_GP(enGPId))
	{
		OPTM_GFX_WBC_S *pstWbc2;
		pstWbc2 = &g_stGfxWbc2;
		/*****Recover WBC_GP0, Gfx4, Gp1*****/
		OPTM_VDP_WBC_SetDitherMode(pstWbc2->enWbcHalId, pstWbc2->enDitherMode);
    	OPTM_VDP_WBC_SetOutFmt    (pstWbc2->enWbcHalId, pstWbc2->stWBCFmt);   
		OPTM_VDP_WBC_SetOutIntf   (pstWbc2->enWbcHalId, pstWbc2->enReadMode);
		OPTM_GfxSetLayerAddr      (HIFB_LAYER_SD_0, pstWbc2->stFrameBuffer.u32StartPhyAddr);
		OPTM_GfxSetLayerStride    (HIFB_LAYER_SD_0, pstWbc2->u32BufferStride);
		OPTM_VDP_GFX_SetInDataFmt(g_stGfxDevice[HIFB_LAYER_SD_0].enGfxHalId, 
					OPTM_PixerFmtTransferToHalFmt(pstWbc2->enDataFmt));

		OPTM_VDP_GFX_SetPalpha   (g_stGfxDevice[HIFB_LAYER_SD_0].enGfxHalId, HI_TRUE,HI_TRUE,0xff,0xff);
		OPTM_VDP_GFX_SetPreMultEnable(g_stGfxDevice[HIFB_LAYER_SD_0].enGfxHalId, HI_TRUE);
		OPTM_VDP_GFX_SetLutAddr(g_stGfxDevice[HIFB_LAYER_SD_0].enGfxHalId, g_stGfxDevice[HIFB_LAYER_SD_0].stCluptTable.u32StartPhyAddr);
		OPTM_VDP_GFX_SetLayerBkg(g_stGfxDevice[HIFB_LAYER_SD_0].enGfxHalId, g_stGfxDevice[HIFB_LAYER_SD_0].stBkg);
		OPTM_VDP_GFX_SetBitExtend(g_stGfxDevice[HIFB_LAYER_SD_0].enGfxHalId, g_stGfxDevice[HIFB_LAYER_SD_0].enBitExtend);
		OPTM_GfxSetLayerReadMode(HIFB_LAYER_SD_0, g_stGfxDevice[HIFB_LAYER_SD_0].enReadMode);
		OPTM_VDP_GFX_SetUpdMode (g_stGfxDevice[HIFB_LAYER_SD_0].enGfxHalId, g_stGfxDevice[HIFB_LAYER_SD_0].enUpDateMode);
		
		OPTM_GfxSetCsc(enGPId, &g_stGfxGPDevice[enGPId].stCscPara, 
				g_stGfxGPDevice[enGPId].bBGRState);
		OPTM_GfxConfigSlvLayer(HIFB_LAYER_SD_0, pstDispRect);
	}
	else
	{
		/*when recovery gp , set disp size to hardware first*/	
		OPTM_GfxSetDispFMTSize((OPTM_GFX_GP_E)enGPId, pstDispRect);
		OPTM_GfxSetCsc(enGPId, &g_stGfxGPDevice[enGPId].stCscPara, 
						g_stGfxGPDevice[enGPId].bBGRState);
		OPTM_VDP_GP_SetDispMode(enGPId, OPTM_GfxGetHalTriDimMode(g_stGfxGPDevice[enGPId].enTriDimMode));
		OPTM_VDP_GP_SetRegUp   (enGPId);
		/****recover all gfx in the gp******/
		for (i = 0; i < u32LayerCount;i++)
		{
			enLayerId = enInitLayerId + i;

			if(!g_stGfxDevice[enLayerId].bOpened)
			{
				continue;
			}	
			
			OPTM_GfxSetEnable(enLayerId,g_stGfxDevice[enLayerId].bEnable);
			//printk("===gp recovery enable %d=====.\n",g_stGfxDevice[enLayerId].bEnable);
			OPTM_GfxSetLayerAlpha(enLayerId, &g_stGfxDevice[enLayerId].stAlpha);
			OPTM_GfxSetLayKeyMask(enLayerId, &g_stGfxDevice[enLayerId].stColorkey);
			OPTM_GfxSetTriDimEnable(enLayerId, g_stGfxDevice[enLayerId].b3DEnable);
			OPTM_GfxSetTriDimMode(enLayerId, g_stGfxDevice[enLayerId].enTriDimMode);
			OPTM_GfxSetTriDimAddr(enLayerId, g_stGfxDevice[enLayerId].u32TriDimAddr);
			OPTM_GfxSetLayerPreMult(enLayerId, g_stGfxDevice[enLayerId].bPreMute);
			OPTM_GfxSetLayerDataFmt(enLayerId, g_stGfxDevice[enLayerId].enDataFmt);
			//printk("====gp recovery layer datafmt %d======.\n",g_stGfxDevice[enLayerId].enDataFmt);
			OPTM_GfxSetLayerStride(enLayerId, g_stGfxDevice[enLayerId].Stride);
			//printk("====gp recovery layer stride %d======.\n",g_stGfxDevice[enLayerId].Stride);
			OPTM_GfxSetLayerRect(enLayerId, &g_stGfxDevice[enLayerId].stInRect);
			//printk("===gp recovery layer rect w %d,h%d=====.\n",g_stGfxDevice[enLayerId].stInRect.w,g_stGfxDevice[enLayerId].stInRect.h);
			OPTM_GfxSetLayerAddr(enLayerId, g_stGfxDevice[enLayerId].NoCmpBufAddr);
			//printk("====gp recovery layer addr 0x%x======.\n",g_stGfxDevice[enLayerId].NoCmpBufAddr);
			
			OPTM_VDP_GFX_SetLutAddr(g_stGfxDevice[enLayerId].enGfxHalId, g_stGfxDevice[enLayerId].stCluptTable.u32StartPhyAddr);
			OPTM_VDP_GFX_SetLayerBkg(g_stGfxDevice[enLayerId].enGfxHalId, g_stGfxDevice[enLayerId].stBkg);
			OPTM_VDP_GFX_SetBitExtend(g_stGfxDevice[enLayerId].enGfxHalId, g_stGfxDevice[enLayerId].enBitExtend);
			OPTM_GfxSetLayerReadMode(enLayerId, g_stGfxDevice[enLayerId].enReadMode);
			OPTM_VDP_GFX_SetUpdMode (g_stGfxDevice[enLayerId].enGfxHalId, g_stGfxDevice[enLayerId].enUpDateMode);
			}
	}
	
	PRINT_OUT;
#endif

	return HI_SUCCESS;
}

HI_S32 OPTM_VO_Callback(HI_VOID* u32Param0, HI_VOID* u32Param1)
{
	HI_U32                      i;
	HI_U32                      u32CTypeFlag;
	HI_U32                      u32LayerCount;
	HIFB_LAYER_ID_E             enInitLayerId;
	HIFB_LAYER_ID_E             enLayerId;	
	OPTM_VDP_LAYER_GP_E              *pEnGpHalId;
	HI_DRV_DISP_CALLBACK_INFO_S *pstDispInfo;		
	
	pEnGpHalId  = (OPTM_VDP_LAYER_GP_E *)u32Param0;
	pstDispInfo = (HI_DRV_DISP_CALLBACK_INFO_S *)u32Param1;

	if (pEnGpHalId == HI_NULL || pstDispInfo == HI_NULL)
	{
		return HI_FAILURE;
	}

	u32LayerCount = (OPTM_VDP_LAYER_GP0 == *pEnGpHalId) ? OPTM_GP0_GFX_COUNT : OPTM_GP1_GFX_COUNT;
	
	if (OPTM_VDP_LAYER_GP0 == *pEnGpHalId)
	{
		enInitLayerId     = HIFB_LAYER_HD_0;
	}
	else if (OPTM_VDP_LAYER_GP1 == *pEnGpHalId)
	{
		enInitLayerId    = HIFB_LAYER_SD_0;
	}
	else
	{
		return HI_SUCCESS;
	}
	
	for (i = 0; i < u32LayerCount;i++)
	{
		u32CTypeFlag = g_stGfxGPIrq[*pEnGpHalId].stGfxCallBack[i].u32CTypeFlag;
		
		if (!u32CTypeFlag)
		{
			continue;
		}
			
		enLayerId = enInitLayerId + i;			
		
		if (u32CTypeFlag & HIFB_CALLBACK_TYPE_VO)
		{
			if (HI_DRV_DISP_FIELD_PROGRESSIVE == pstDispInfo->eField
				|| HI_DRV_DISP_FIELD_BOTTOM == pstDispInfo->eField)
			{
				g_stGfxGPIrq[*pEnGpHalId].stGfxCallBack[i].stGfxIrq[HIFB_CALLBACK_TYPE_VO].pFunc(
					(HI_VOID*)g_stGfxGPIrq[*pEnGpHalId].stGfxCallBack[i].stGfxIrq[HIFB_CALLBACK_TYPE_VO].u32Param0, 
					HI_NULL);
				/**when v sync , wake up V Block*/
				OPTM_GfxWVBCallBack(enLayerId, HI_NULL);
			}
			else if ((HIFB_LAYER_HD_1 == (enLayerId) || HIFB_LAYER_HD_2 == (enLayerId))
				&& (HI_DRV_DISP_FIELD_TOP == pstDispInfo->eField))
			{
				g_stGfxGPIrq[*pEnGpHalId].stGfxCallBack[i].stGfxIrq[HIFB_CALLBACK_TYPE_VO].pFunc(
					(HI_VOID*)g_stGfxGPIrq[*pEnGpHalId].stGfxCallBack[i].stGfxIrq[HIFB_CALLBACK_TYPE_VO].u32Param0, 
					HI_NULL);
				/**when v sync , wake up V Block*/
				OPTM_GfxWVBCallBack(enLayerId, HI_NULL);
			}
		}

		if (u32CTypeFlag & HIFB_CALLBACK_TYPE_REGUP)
		{
			/*callback function*/
			/*define here for extending*/ 
		}
	}

	return HI_SUCCESS;
}

HI_S32 OPTM_Distribute_Callback(HI_VOID* u32Param0, HI_VOID* u32Param1)
{
#if 1
#warning TODO
#else
	HIFB_STEREO_MODE_E          enTriDimMode;
	OPTM_VDP_LAYER_GP_E         *pEnGpHalId;
	HI_DRV_DISP_CALLBACK_INFO_S *pstDispInfo;		

	
	
	pEnGpHalId  = (OPTM_VDP_LAYER_GP_E *)u32Param0;
	pstDispInfo = (HI_DRV_DISP_CALLBACK_INFO_S *)u32Param1;

	if (pEnGpHalId == HI_NULL || pstDispInfo == HI_NULL)
	{
		return HI_FAILURE;
	}

	OPTM_VO_Callback(u32Param0, u32Param1);

	/************************3D Mode change*************************/
	enTriDimMode = OPTM_AdaptTriDimModeFromDisp(pstDispInfo->stDispInfo.eDispMode);
#if 1	
	if (enTriDimMode != g_stGfxGPDevice[*pEnGpHalId].enTriDimMode
		&& (HIFB_STEREO_BUTT > enTriDimMode))
	{
		g_stGfxGPDevice[*pEnGpHalId].enTriDimMode = enTriDimMode;
		OPTM_VDP_GP_SetDispMode(*pEnGpHalId, OPTM_GfxGetHalTriDimMode(enTriDimMode));
		OPTM_VDP_GP_SetRegUp   (*pEnGpHalId);
     #ifndef HI_BUILD_IN_BOOT   
		g_stGfxGPDevice[*pEnGpHalId].st3DModeChgWork.u32Data = *pEnGpHalId;
	    queue_work(g_stGfxGPDevice[*pEnGpHalId].queue, &g_stGfxGPDevice[*pEnGpHalId].st3DModeChgWork.work);
     #endif

	 	/**************WBC_GP_INRECT == GP_INRECT******/
		if (IS_MASTER_GP(*pEnGpHalId))
		{
			g_stGfxGPDevice[OPTM_SLAVER_GPID].stInRect.s32Width  = g_stGfxGPDevice[*pEnGpHalId].stInRect.s32Width;
			g_stGfxGPDevice[OPTM_SLAVER_GPID].stInRect.s32Height = g_stGfxGPDevice[*pEnGpHalId].stInRect.s32Height;

			g_stGfxWbc2.enWbcMode = OPTM_WBC_MODE_LFET_EYE;
			g_stGfxGPDevice[OPTM_SLAVER_GPID].unUpFlag.bits.WbcMode  = 1;
			g_stGfxGPDevice[OPTM_SLAVER_GPID].unUpFlag.bits.InRect = 1;
			g_stGfxGPDevice[OPTM_SLAVER_GPID].unUpFlag.bits.RegUp  = 1;
		}
    }
#endif	
#endif

	return HI_SUCCESS;
}

HI_S32 OPTM_GFX_SetGpInUsrFlag(OPTM_GFX_GP_E enGpId, HI_BOOL bFlag)
{
	g_stGfxGPDevice[enGpId].bGPInSetbyusr = bFlag;
	return HI_SUCCESS;
}

HI_S32 OPTM_GFX_GetGpInUsrFlag(OPTM_GFX_GP_E enGpId)
{
	
	return g_stGfxGPDevice[enGpId].bGPInSetbyusr;
}

HI_S32 OPTM_GFX_SetGpInInitFlag(OPTM_GFX_GP_E enGpId, HI_BOOL bFlag)
{
	g_stGfxGPDevice[enGpId].bGPInInitial= bFlag;
	return HI_SUCCESS;
}

HI_S32 OPTM_GFX_GetGpInInitFlag(OPTM_GFX_GP_E enGpId)
{
	
	return g_stGfxGPDevice[enGpId].bGPInInitial;
}

HI_S32 OPTM_GFX_SetGfxMask(OPTM_GFX_GP_E enGpId, HI_BOOL bFlag)
{
	HIFB_OSD_DATA_S stLayerData;
	PRINT_IN;
	
	g_stGfxGPDevice[enGpId].bMaskFlag = bFlag;

	/*when set gp enable, recovery all properties of gp*/
	if (!bFlag)
	{
	    OPTM_GPRecovery(enGpId);
	}

	if (OPTM_MASTER_GPID == enGpId)
	{
		/*work in wbc mode
		     1: user app
		     2: boot logo*/
		OPTM_GfxGetOSDData(HIFB_LAYER_SD_0, &stLayerData);

		if (IS_MASTER_GP(enGpId) ||
			HIFB_LAYER_STATE_ENABLE == stLayerData.eState)
		{
			/*set wbc mask and recovery*/
			g_stGfxGPDevice[OPTM_SLAVER_GPID].bMaskFlag = bFlag;			
			if (!bFlag
				&& g_stGfxWbc2.bOpened)
			{							
				OPTM_GPRecovery(OPTM_SLAVER_GPID);
			}
		}
	}

	PRINT_OUT;
	
	return HI_SUCCESS;
}

HI_S32 OPTM_GFX_GetGfxMask(OPTM_GFX_GP_E enGpId)
{	
	return g_stGfxGPDevice[enGpId].bMaskFlag;
}

HI_S32 OPTM_GFX_ClearLogoOsd(HIFB_LAYER_ID_E enLayerId)
{
#if 1
#warning TODO
#else
	if (!g_stGfxGPDevice[OPTM_MASTER_GPID].bOpen &&
		!g_stGfxGPDevice[OPTM_SLAVER_GPID].bOpen)
	{
		OPTM_VDP_WBC_SetEnable(g_stGfxWbc2.enWbcHalId, HI_FALSE);		
		OPTM_VDP_WBC_SetLayerAddr(g_stGfxWbc2.enWbcHalId, HI_NULL,
        							HI_NULL, HI_NULL, HI_NULL);
		OPTM_VDP_WBC_SetRegUp (g_stGfxWbc2.enWbcHalId);
		
		OPTM_VDP_GFX_SetLayerEnable(OPTM_GetGfxHalId(OPTM_SLAVER_LAYERID), HI_FALSE);		
		OPTM_VDP_GFX_SetLayerAddrEX(OPTM_GetGfxHalId(OPTM_SLAVER_LAYERID), HI_NULL);
		OPTM_VDP_GFX_SetRegUp(OPTM_GetGfxHalId(OPTM_SLAVER_LAYERID));
	}

	OPTM_VDP_GFX_SetLayerEnable(OPTM_GetGfxHalId(enLayerId), HI_FALSE);
	OPTM_VDP_GFX_SetLayerAddrEX(OPTM_GetGfxHalId(enLayerId), HI_NULL);
	OPTM_VDP_GFX_SetRegUp(OPTM_GetGfxHalId(enLayerId));
#endif

	return HI_SUCCESS;
}

HI_S32 OPTM_GFX_SetStereoDepth(HIFB_LAYER_ID_E enLayerId, HI_S32 s32Depth)
{
#if 1
#warning TODO
#else
	OPTM_VDP_GFX_SetThreeDimDofEnable(OPTM_GetGfxHalId(enLayerId), HI_TRUE);
	OPTM_VDP_GFX_SetThreeDimDofStep(OPTM_GetGfxHalId(enLayerId), s32Depth, (0-s32Depth));
#endif
	return HI_SUCCESS;
}

HI_VOID OPTM_GFX_GetOps(OPTM_GFX_OPS_S *ops)
{
	ops->OPTM_GfxCloseLayer         = OPTM_GfxCloseLayer;
	ops->OPTM_GfxDeInit             = OPTM_GfxDeInit;
	ops->OPTM_GfxGetLayerPriority   = OPTM_GfxGetLayerPriority;
	ops->OPTM_GfxGetOSDData         = OPTM_GfxGetOSDData;
//	ops->OPTM_GfxGfxCmpHandle       = OPTM_GfxGfxCmpHandle;
	ops->OPTM_GfxInit               = OPTM_GfxInit;
	ops->OPTM_GfxOpenLayer          = OPTM_GfxOpenLayer;
      
	ops->OPTM_GfxSetCallback        = OPTM_GfxSetCallback;

	ops->OPTM_GfxSetClutAddr        = OPTM_GfxSetClutAddr;
	ops->OPTM_GfxSetColorReg        = OPTM_GfxSetColorReg;
	ops->OPTM_GfxSetEnable          = OPTM_GfxSetEnable;
//	ops->OPTM_GfxSetGfxCmpAddr      = OPTM_GfxSetGfxCmpAddr;
//	ops->OPTM_GfxSetGfxCmpRect      = OPTM_GfxSetGfxCmpRect;
	ops->OPTM_GfxSetGpRect          = OPTM_GfxSetGpRect;
	ops->OPTM_GfxSetGpInPutSize     = OPTM_GfxSetGpInPutSize;
	ops->OPTM_GfxSetLayerAddr       = OPTM_GfxSetLayerAddr;
	ops->OPTM_GfxSetLayerAlpha      = OPTM_GfxSetLayerAlpha;
	ops->OPTM_GfxSetLayerDataFmt    = OPTM_GfxSetLayerDataFmt;
//	ops->OPTM_GfxSetLayerDeCmpEnable= OPTM_GfxSetLayerDeCmpEnable;
	ops->OPTM_GfxSetLayerDeFlicker  = OPTM_GfxSetLayerDeFlicker;
	ops->OPTM_GfxSetLayerPreMult    = OPTM_GfxSetLayerPreMult;
	ops->OPTM_GfxSetLayerPriority   = OPTM_GfxSetLayerPriority;
	ops->OPTM_GfxSetLayerRect       = OPTM_GfxSetLayerRect;
	ops->OPTM_GfxSetLayerStride     = OPTM_GfxSetLayerStride;
	ops->OPTM_GfxSetLayKeyMask      = OPTM_GfxSetLayKeyMask;
	ops->OPTM_GfxUpLayerReg         = OPTM_GfxUpLayerReg;
	ops->OPTM_GfxWaitVBlank         = OPTM_GfxWaitVBlank;
	ops->OPTM_GFX_GetDevCap         = OPTM_GFX_GetDevCap;
	ops->OPTM_GfxGetOutRect         = OPTM_GfxGetOutRect;
	ops->OPTM_GfxGetLayerRect       = OPTM_GfxGetLayerRect;
	ops->OPTM_GFX_SetGpInUsrFlag    = OPTM_GFX_SetGpInUsrFlag;
	ops->OPTM_GFX_GetGpInUsrFlag    = OPTM_GFX_GetGpInUsrFlag;
	ops->OPTM_GFX_SetGpInInitFlag   = OPTM_GFX_SetGpInInitFlag;
	ops->OPTM_GFX_GetGpInInitFlag   = OPTM_GFX_GetGpInInitFlag;
	ops->OPTM_GFX_SetGfxMask        = OPTM_GFX_SetGfxMask;
	ops->OPTM_GFX_GetGfxMask        = OPTM_GFX_GetGfxMask;
	ops->OPTM_GfxGetDispFMTSize     = OPTM_GfxGetDispFMTSize;

	ops->OPTM_GFX_GetSlvLayerInfo	= OPTM_GFX_GetSlvLayerInfo;
	ops->OPTM_GFX_SetTCFlag 		= OPTM_GFX_SetTCFlag;

	/***3D****/
	ops->OPTM_GfxSetTriDimEnable    = OPTM_GfxSetTriDimEnable;
	ops->OPTM_GfxSetTriDimMode      = OPTM_GfxSetTriDimMode;
	ops->OPTM_GfxSetTriDimAddr      = OPTM_GfxSetTriDimAddr;
	ops->OPTM_GFX_SetStereoDepth    = OPTM_GFX_SetStereoDepth;
	ops->OPTM_GFX_ClearLogoOsd      = OPTM_GFX_ClearLogoOsd;
		
	ops->OPTM_GFX_SetGpDeflicker 	= OPTM_GfxSetGpDeflicker;
}

/***********************************************************/
/*			               adapt system function                                                 */
/***********************************************************/
HI_S32 OPTM_AllocAndMap(const char *bufname, char *zone_name, HI_U32 size, int align, MMZ_BUFFER_S *psMBuf)
{
#ifndef HI_BUILD_IN_BOOT
	return HI_DRV_MMZ_AllocAndMap(bufname, zone_name, size, align, psMBuf);	
#else
	if (HI_SUCCESS == HI_DRV_PDM_AllocReserveMem(bufname, size, &psMBuf->u32StartPhyAddr))
	{
		psMBuf->u32StartVirAddr= psMBuf->u32StartPhyAddr;
		return HI_SUCCESS;
	}
	else
	{
		return HI_FAILURE;
	}	
#endif
}

HI_VOID OPTM_UnmapAndRelease(MMZ_BUFFER_S *psMBuf)
{
#ifdef HI_BUILD_IN_BOOT
	return;
#else
	HI_DRV_MMZ_UnmapAndRelease(psMBuf);
#endif

}

HI_S32 OPTM_Adapt_AllocAndMap(const char *bufname, char *zone_name, HI_U32 size, int align, MMZ_BUFFER_S *psMBuf)
{
#ifndef HI_BUILD_IN_BOOT
	return HI_DRV_MMZ_AllocAndMap(bufname, zone_name, size, align, psMBuf);	
#else
	psMBuf->u32StartPhyAddr = (HI_U32)malloc(size);
	if (HI_NULL == psMBuf->u32StartPhyAddr)
	{
		HIFB_ERROR("fail to alloc buffer.\n");
		return HI_FAILURE;
	}
	
	psMBuf->u32StartVirAddr = psMBuf->u32StartPhyAddr;
	return HI_SUCCESS;
#endif
}



