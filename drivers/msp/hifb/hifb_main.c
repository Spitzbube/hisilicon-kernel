/******************************************************************************

  Copyright (C), 2001-2011, Huawei Tech. Co., Ltd.

******************************************************************************
  File Name     : hifb_main.c
  Version       : Initial Draft
  Author        : w54130
  Created       : 2007/5/25
  Last Modified :
  Description   : framebuffer external function
  History       :
  1.Date        : 2007/5/25
    Author      : w54130
    Modification: Created file

  2.Date        : 2007/11/07
    Author      : w54130
    Modification:AE6D02359(modified function hifb_check_var_xbpp and hifb_check_var_1632)

  3.Date        : 2008/01/23
    Author      : w54130
    Modification: AE5D02630(add check for xoffset and yoffset in hifb_check_var_xbpp and hifb_check_var_1632)

  4.Date        : 2008/01/23
    Author      : w54130
    Modification: AE5D02631(copy temporary varible to user point agrp in FBIOGET_DEFLICKER_HIFB)

  5.Date        : 2008/06/26
    Author      : s37678
    Modification: fix the defect AE5D02966
    

******************************************************************************/

#include <linux/ctype.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/delay.h>

#include <linux/slab.h>
#include <linux/mm.h>

#include <linux/fb.h>
#include <asm/uaccess.h>

#include <asm/types.h>
#include <asm/stat.h>
#include <asm/fcntl.h>

#include <linux/interrupt.h>

#include <staging/android/sw_sync.h>

#include "hi_module.h"
#include "hi_drv_module.h"

#include "hi_drv_dev.h"
#include "hi_debug.h"
#include "drv_disp_ext.h"
#include "hi_drv_proc.h"
#include "hi_drv_disp.h"


#include "hifb_drv.h"

#include "hifb.h"
#include "hifb_p.h"
#include "hifb_comm.h"
#include "drv_pdm_ext.h"

#ifdef CFG_HIFB_SCROLLTEXT_SUPPORT
#include "hifb_scrolltext.h"
#endif


/*************************** Macro Definition ****************************/
/** \addtogroup           HIFB_MAIN   */
/** @{ */  /** <!-- [HIFB_MAIN] */

#ifdef CFG_HIFB_STEREO3D_HW_SUPPORT
#define CFG_HIFB_3D_ENABLE   
#endif

#define CFG_HIFB_LOGO_SUPPORT
#define CFG_HIFB_VIRTUAL_COORDINATE_SUPPORT
//#define CFG_HIFB_PROC_SUPPORT
//#define CFG_HIFB_PROC_DEBUG
#define HIFB_MAX_WIDTH(u32LayerId)  g_pstCap[u32LayerId].u32MaxWidth
#define HIFB_MAX_HEIGHT(u32LayerId) g_pstCap[u32LayerId].u32MaxHeight
#define HIFB_MIN_WIDTH(u32LayerId)  g_pstCap[u32LayerId].u32MinWidth
#define HIFB_MIN_HEIGHT(u32LayerId) g_pstCap[u32LayerId].u32MinHeight

#define IS_STEREO_SBS(par)  ((par->st3DInfo.enOutStereoMode == HIFB_STEREO_SIDEBYSIDE_HALF))
#define IS_STEREO_TAB(par)  ((par->st3DInfo.enOutStereoMode == HIFB_STEREO_TOPANDBOTTOM))
#define IS_STEREO_FPK(par)  ((par->st3DInfo.enOutStereoMode == HIFB_STEREO_FRMPACKING))

#define IS_2BUF_MODE(par)  ((par->stExtendInfo.enBufMode == HIFB_LAYER_BUF_DOUBLE || par->stExtendInfo.enBufMode == HIFB_LAYER_BUF_DOUBLE_IMMEDIATE))
#define IS_1BUF_MODE(par)  ((par->stExtendInfo.enBufMode == HIFB_LAYER_BUF_ONE))

#define HIFB_LOGO_LAYER_ID HIFB_LAYER_HD_2
static HI_BOOL  g_bLogoShow = HI_FALSE;
static struct work_struct s_FreeLogoMemWork;
static HI_BOOL  g_bProcDebug = HI_FALSE;

/*mem size of layer.hifb will allocate mem:*/
static char* video = "";
//static char* video = "hifb:vram0_size:7200,vram1_size:3600,vram2_size:3600";
module_param(video, charp, S_IRUGO);

static char* tc_wbc = "off"; //80fb410c
module_param(tc_wbc, charp, S_IRUGO);

HIFB_DRV_OPS_S s_stDrvOps; //81143678 +212 = 8114374C
HI_U32 Data_8114374c = 0; //8114374c
typedef struct
{
	HI_BOOL bLogoShow; //0
	int Data_4; //0
	HI_U32 u32LayerId; //0
	//12
} Struct_81143750;

Struct_81143750 Data_81143750[2]; //81143750
HIFB_DRV_TDEOPS_S s_stDrvTdeOps; //8114376c

/* to save layer id and layer size */
HIFB_LAYER_S s_stLayer[HIFB_MAX_LAYER_NUM]; //81143610

const static HIFB_CAPABILITY_S *g_pstCap; //81143768

#ifdef CFG_HIFB_SCROLLTEXT_SUPPORT
/*define this array to save the private info of scrolltext layer*/
HIFB_SCROLLTEXT_INFO_S s_stTextLayer[HIFB_LAYER_ID_BUTT];
#endif

/*config layer size
	1: config layer memory size from video params
	2: config layer memory size from cfg.mak
	3: config layer memory size when usr opened layer*/
HI_U32 g_u32LayerSize[HIFB_MAX_LAYER_NUM] = 
{
/***********HD0**************/
#ifdef CFG_HI_HD0_FB_VRAM_SIZE
		CFG_HI_HD0_FB_VRAM_SIZE,
#else
		0,
#endif

/***********HD1**************/
#ifdef CFG_HI_HD1_FB_VRAM_SIZE
		CFG_HI_HD1_FB_VRAM_SIZE,
#else
		0,
#endif

/***********HD2**************/
#ifdef CFG_HI_HD2_FB_VRAM_SIZE
		CFG_HI_HD2_FB_VRAM_SIZE,
#else
		0,
#endif

/***********HD3**************/
#ifdef CFG_HI_HD3_FB_VRAM_SIZE
		CFG_HI_HD3_FB_VRAM_SIZE,
#else
		0,
#endif

/***********SD0**************/
#ifdef CFG_HI_SD0_FB_VRAM_SIZE
		CFG_HI_SD0_FB_VRAM_SIZE,
#else
        0,
#endif

/***********SD1**************/
#ifdef CFG_HI_SD1_FB_VRAM_SIZE
		CFG_HI_SD1_FB_VRAM_SIZE,
#else
		0,
#endif

/***********SD2**************/
#ifdef CFG_HI_SD2_FB_VRAM_SIZE
		CFG_HI_SD2_FB_VRAM_SIZE,
#else
		0,
#endif

/***********SD3**************/
#ifdef CFG_HI_SD3_FB_VRAM_SIZE
		CFG_HI_SD3_FB_VRAM_SIZE,
#else
		0,
#endif

/***********AD0**************/
#ifdef CFG_HI_AD0_FB_VRAM_SIZE
		CFG_HI_AD0_FB_VRAM_SIZE,
#else
		0,
#endif

/***********AD1**************/
#ifdef CFG_HI_AD1_FB_VRAM_SIZE
		CFG_HI_AD1_FB_VRAM_SIZE,
#else
		0,
#endif

/***********AD2**************/
#ifdef CFG_HI_AD2_FB_VRAM_SIZE
		CFG_HI_AD2_FB_VRAM_SIZE,
#else
		0,
#endif

/***********AD3**************/
#ifdef CFG_HI_AD3_FB_VRAM_SIZE
		CFG_HI_AD3_FB_VRAM_SIZE,
#else
		0,
#endif

/***********SOFT CURSOR******/
#ifdef CFG_HI_CURSOR_FB_VRAM_SIZE
		CFG_HI_CURSOR_FB_VRAM_SIZE,
#else
				0,
#endif

};

/* default fix information */
static struct fb_fix_screeninfo s_stDefFix[HIFB_LAYER_TYPE_BUTT] =
{
    {
        .id          = "hifb",
        .type        = FB_TYPE_PACKED_PIXELS,
        .visual      = FB_VISUAL_TRUECOLOR,
        .xpanstep    =                     1,
        .ypanstep    =                     1,
        .ywrapstep   =                     0,
        .line_length = HIFB_HD_DEF_STRIDE,
        .accel       = FB_ACCEL_NONE,
        .mmio_len    =                     0,
        .mmio_start  =                     0,
    },
    {
        .id          = "hifb",
        .type        = FB_TYPE_PACKED_PIXELS,
        .visual      = FB_VISUAL_TRUECOLOR,
        .xpanstep    =                     1,
        .ypanstep    =                     1,
        .ywrapstep   =                     0,
        .line_length = HIFB_SD_DEF_STRIDE,
        .accel       = FB_ACCEL_NONE,
        .mmio_len    =                     0,
        .mmio_start  =                     0,
    },
    {
        .id          = "hifb",
        .type        = FB_TYPE_PACKED_PIXELS,
        .visual      = FB_VISUAL_TRUECOLOR,
        .xpanstep    =                     1,
        .ypanstep    =                     1,
        .ywrapstep   =                     0,
        .line_length = HIFB_AD_DEF_STRIDE,
        .accel       = FB_ACCEL_NONE,
        .mmio_len    =                     0,
        .mmio_start  =                     0,
    },
    {
    .id          = "hifb",
    .type        = FB_TYPE_PACKED_PIXELS,
    .visual      = FB_VISUAL_TRUECOLOR,
    .xpanstep    =                     1,
    .ypanstep    =                     1,
    .ywrapstep   =                     0,
    .line_length = HIFB_AD_DEF_STRIDE,
    .accel       = FB_ACCEL_NONE,
    .mmio_len    =                     0,
    .mmio_start  =                     0,
    }
};

/* default variable information */
static struct fb_var_screeninfo s_stDefVar[HIFB_LAYER_TYPE_BUTT] =
{
    /*for HD layer*/
    {
        .xres			= HIFB_HD_DEF_WIDTH,
        .yres			= HIFB_HD_DEF_HEIGHT,
        .xres_virtual	= HIFB_HD_DEF_WIDTH,
        .yres_virtual	= HIFB_HD_DEF_HEIGHT,
        .xoffset        = 0,
        .yoffset        = 0,
        .bits_per_pixel = HIFB_DEF_DEPTH,
        .red			= {16, 8, 0},
        .green			= {8, 8, 0},
        .blue			= {0, 8, 0},
        .transp			= {24, 8, 0},
        .activate		= FB_ACTIVATE_NOW,
        .pixclock		= -1, /* pixel clock in ps (pico seconds) */
        .left_margin	= -1, /* time from sync to picture	*/
        .right_margin	= -1, /* time from picture to sync	*/
        .upper_margin	= -1, /* time from sync to picture	*/
        .lower_margin	= -1,
        .hsync_len		= -1, /* length of horizontal sync	*/
        .vsync_len		= -1, /* length of vertical sync	*/
    },
    /*for SD layer*/
    {
        .xres			= HIFB_SD_DEF_WIDTH,
        .yres			= HIFB_SD_DEF_HEIGHT,
        .xres_virtual	= HIFB_SD_DEF_WIDTH,
        .yres_virtual	= HIFB_SD_DEF_HEIGHT,
        .xoffset        = 0,
        .yoffset        = 0,
        .bits_per_pixel = HIFB_DEF_DEPTH,
        .red			= {16, 8, 0},
        .green			= {8, 8, 0},
        .blue			= {0, 8, 0},
        .transp			= {24, 8, 0},
        .activate		= FB_ACTIVATE_NOW,
        .pixclock		= -1, /* pixel clock in ps (pico seconds) */
        .left_margin	= -1, /* time from sync to picture	*/
        .right_margin	= -1, /* time from picture to sync	*/
        .upper_margin	= -1, /* time from sync to picture	*/
        .lower_margin	= -1,
        .hsync_len		= -1, /* length of horizontal sync	*/
        .vsync_len		= -1, /* length of vertical sync	*/
    },
    /*for AD layer*/
    {
        .xres			= HIFB_AD_DEF_WIDTH,
        .yres			= HIFB_AD_DEF_HEIGHT,
        .xres_virtual	= HIFB_AD_DEF_WIDTH,
        .yres_virtual	= HIFB_AD_DEF_HEIGHT,
        .xoffset        = 0,
        .yoffset        = 0,
        .bits_per_pixel = HIFB_DEF_DEPTH,
        .red			= {16, 8, 0},
        .green			= {8, 8, 0},
        .blue			= {0, 8, 0},
        .transp			= {24, 8, 0},
        .activate		= FB_ACTIVATE_NOW,
        .pixclock		= -1, /* pixel clock in ps (pico seconds) */
        .left_margin	= -1, /* time from sync to picture	*/
        .right_margin	= -1, /* time from picture to sync	*/
        .upper_margin	= -1, /* time from sync to picture	*/
        .lower_margin	= -1,
        .hsync_len		= -1, /* length of horizontal sync	*/
        .vsync_len		= -1, /* length of vertical sync	*/
    },
     /*for CURSOR layer*/
    {
        .xres			= HIFB_CURSOR_DEF_WIDTH,
        .yres			= HIFB_CURSOR_DEF_HEIGHT,
        .xres_virtual	= HIFB_CURSOR_DEF_WIDTH,
        .yres_virtual	= HIFB_CURSOR_DEF_HEIGHT,
        .xoffset        = 0,
        .yoffset        = 0,
        .bits_per_pixel = HIFB_DEF_DEPTH,
        .red			= {16, 8, 0},
        .green			= {8, 8, 0},
        .blue			= {0, 8, 0},
        .transp			= {24, 8, 0},
        .activate		= FB_ACTIVATE_NOW,
        .pixclock		= -1, /* pixel clock in ps (pico seconds) */
        .left_margin	= -1, /* time from sync to picture	*/
        .right_margin	= -1, /* time from picture to sync	*/
        .upper_margin	= -1, /* time from sync to picture	*/
        .lower_margin	= -1,
        .hsync_len		= -1, /* length of horizontal sync	*/
        .vsync_len		= -1, /* length of vertical sync	*/
    }
};

/* bit filed info of color fmt, the order must be the same as HIFB_COLOR_FMT_E */
static HIFB_ARGB_BITINFO_S s_stArgbBitField[] =
{   /*RGB565*/
    {
        .stRed    = {11, 5, 0},
        .stGreen  = {5, 6, 0},
        .stBlue   = {0, 5, 0},
        .stTransp = {0, 0, 0},
    },
    /*RGB888*/
    {
        .stRed    = {16, 8, 0},
        .stGreen  = {8, 8, 0},
        .stBlue   = {0, 8, 0},
        .stTransp = {0, 0, 0},
    },
    /*KRGB444*/
    {
        .stRed    = {8, 4, 0},
        .stGreen  = {4, 4, 0},
        .stBlue   = {0, 4, 0},
        .stTransp = {0, 0, 0},
    },
    /*KRGB555*/
    {
        .stRed    = {10, 5, 0},
        .stGreen  = {5, 5, 0},
        .stBlue   = {0, 5, 0},
        .stTransp = {0, 0, 0},
    },
    /*KRGB888*/
    {
        .stRed    = {16,8, 0},
        .stGreen  = {8, 8, 0},
        .stBlue   = {0, 8, 0},
        .stTransp = {0, 0, 0},
    },
    /*ARGB4444*/
    {
        .stRed    = {8, 4, 0},
        .stGreen  = {4, 4, 0},
        .stBlue   = {0, 4, 0},
        .stTransp = {12, 4, 0},
    },
    /*ARGB1555*/
    {
        .stRed    = {10, 5, 0},
        .stGreen  = {5, 5, 0},
        .stBlue   = {0, 5, 0},
        .stTransp = {15, 1, 0},
    },
    /*ARGB8888*/
    {
        .stRed    = {16, 8, 0},
        .stGreen  = {8, 8, 0},
        .stBlue   = {0, 8, 0},
        .stTransp = {24, 8, 0},
    },
    /*ARGB8565*/
    {
        .stRed    = {11, 5, 0},
        .stGreen  = {5, 6, 0},
        .stBlue   = {0, 5, 0},
        .stTransp = {16, 8, 0},
    },
    /*RGBA4444*/
    {
        .stRed    = {12, 4, 0},
        .stGreen  = {8, 4, 0},
        .stBlue   = {4, 4, 0},
        .stTransp = {0, 4, 0},
    },
    /*RGBA5551*/
    {
        .stRed    = {11, 5, 0},
        .stGreen  = {6, 5, 0},
        .stBlue   = {1, 5, 0},
        .stTransp = {0, 1, 0},
    },    
    /*RGBA5658*/
    {
        .stRed    = {19, 5, 0},
        .stGreen  = {13, 6, 0},
        .stBlue   = {8, 5, 0},
        .stTransp = {0, 8, 0},
    },  
    /*RGBA8888*/
    {
        .stRed    = {24, 8, 0},
        .stGreen  = {16, 8, 0},
        .stBlue   = {8, 8, 0},
        .stTransp = {0, 8, 0},
    },
    /*BGR565*/
    {
        .stRed    = {0, 5, 0},
        .stGreen  = {5, 6, 0},
        .stBlue   = {11, 5, 0},
        .stTransp = {0, 0, 0},
    },
    /*BGR888*/
    {
        .stRed    = {0, 8, 0},
        .stGreen  = {8, 8, 0},
        .stBlue   = {16, 8, 0},
        .stTransp = {0, 0, 0},
    },
    /*ABGR4444*/
    {
        .stRed    = {0, 4, 0},
        .stGreen  = {4, 4, 0},
        .stBlue   = {8, 4, 0},
        .stTransp = {12, 4, 0},
    },
    /*ABGR1555*/
    {
        .stRed    = {0, 5, 0},
        .stGreen  = {5, 5, 0},
        .stBlue   = {10, 5, 0},
        .stTransp = {15, 1, 0},
    },
    /*ABGR8888*/
    {
        .stRed    = {0, 8, 0},
        .stGreen  = {8, 8, 0},
        .stBlue   = {16, 8, 0},
        .stTransp = {24, 8, 0},
    },
    /*ABGR8565*/
    {
        .stRed    = {0, 5, 0},
        .stGreen  = {5, 6, 0},
        .stBlue   = {11, 5, 0},
        .stTransp = {16, 8, 0},
    },
    /*KBGR444 16bpp*/
    {
        .stRed    = {0, 4, 0},
        .stGreen  = {4, 4, 0},
        .stBlue   = {8, 4, 0},
        .stTransp = {0, 0, 0},
    },
    /*KBGR555 16bpp*/
    {
        .stRed    = {0, 5, 0},
        .stGreen  = {5, 5, 0},
        .stBlue   = {10, 5, 0},
        .stTransp = {0, 0, 0},
    },
    /*KBGR888 32bpp*/
    {
        .stRed    = {0, 8, 0},
        .stGreen  = {8, 8, 0},
        .stBlue   = {16, 8, 0},
        .stTransp = {0, 0, 0},
    },
    
    /*1bpp*/
    {
        .stRed = {0, 1, 0},
        .stGreen = {0, 1, 0},
        .stBlue = {0, 1, 0},
        .stTransp = {0, 0, 0},
    },
    /*2bpp*/
    {
        .stRed = {0, 2, 0},
        .stGreen = {0, 2, 0},
        .stBlue = {0, 2, 0},
        .stTransp = {0, 0, 0},
    },
    /*4bpp*/
    {
        .stRed = {0, 4, 0},
        .stGreen = {0, 4, 0},
        .stBlue = {0, 4, 0},
        .stTransp = {0, 0, 0},
    },
    /*8bpp*/
    {
        .stRed = {0, 8, 0},
        .stGreen = {0, 8, 0},
        .stBlue = {0, 8, 0},
        .stTransp = {0, 0, 0},
    },
    /*ACLUT44*/
    {
        .stRed = {4, 4, 0},
        .stGreen = {4, 4, 0},
        .stBlue = {4, 4, 0},
        .stTransp = {0, 4, 0},
    },
    /*ACLUT88*/
    {
        .stRed = {8, 8, 0},
        .stGreen = {8, 8, 0},
        .stBlue = {8, 8, 0},
        .stTransp = {0, 8, 0},
    }
};

/** @} */  /** <!-- ==== Macro Definition end ==== */


/******************************* API declaration *****************************/
/** \addtogroup      HIFB_MAIN */
/** @{ */  /** <!-- ��HIFB_MAIN �� */

static HI_S32 hifb_vo_callback(HI_VOID *pParaml, HI_VOID *pParamr);
static HI_S32 hifb_refresh(HI_U32 u32LayerId, HIFB_BUFFER_S *pstCanvasBuf, HIFB_LAYER_BUF_E enBufMode);
static HI_VOID hifb_select_antiflicker_mode(HIFB_PAR_S *pstPar);
static HI_S32 hifb_refreshall(struct fb_info *info);
static HI_S32 hifb_tde_callback(HI_VOID *pParaml, HI_VOID *pParamr);
static HI_S32 hifb_alloccanbuf(struct fb_info *info, HIFB_LAYER_INFO_S * pLayerInfo);
#ifdef CFG_HIFB_3D_ENABLE
static HI_S32 hifb_clearallstereobuf(struct fb_info *info);
static HI_S32 hifb_checkandalloc_3dmem(HIFB_LAYER_ID_E enLayerId, HI_U32 u32BufferSize);
#endif
static HI_VOID hifb_3DMode_callback(HI_VOID * pParaml,HI_VOID * pParamr);
static HI_VOID hifb_assign_dispbuf(HI_U32 u32LayerId);
static HI_S32 hifb_setcolreg(unsigned regno, unsigned red, unsigned green,
                unsigned blue, unsigned transp, struct fb_info *info);
static HI_S32 hifb_pan_display(struct fb_var_screeninfo *var, struct fb_info *info);

static HI_S32 hifb_frame_end_callback(HI_VOID *pParaml, HI_VOID *pParamr);

/** @} */  /** <!-- ==== API declaration end ==== */

#ifdef CFG_HIFB_LOGO_SUPPORT
static HI_VOID hifb_freelogomem_work(struct work_struct *work, HI_BOOL i)
{
    PDM_EXPORT_FUNC_S *ps_PdmExportFuncs = HI_NULL;

    if (HI_SUCCESS != HI_DRV_MODULE_GetFunction(HI_ID_PDM, (HI_VOID**)&ps_PdmExportFuncs))
    {
    	return;
    }
	
    if(HI_NULL == ps_PdmExportFuncs)
    {
        return;
    }
    
#if 0
    /*msleep 40ms to asure wbc closed*/
    msleep(40);

    ps_PdmExportFuncs->pfnPDM_ReleaseReserveMem(DISPLAY_BUFFER);
    ps_PdmExportFuncs->pfnPDM_ReleaseReserveMem(OPTM_GFX_WBC2_BUFFER);
    ps_PdmExportFuncs->pfnPDM_ReleaseReserveMem(HIFB_ZME_COEF_BUFFER);
#endif

	if (i == HI_FALSE)
	{
		ps_PdmExportFuncs->pfnPDM_ReleaseReserveMem(DISPLAY_BUFFER_HD);
		ps_PdmExportFuncs->pfnPDM_ReleaseReserveMem(OPTM_GFX_WBC2_BUFFER);
		HIFB_INFO("<<<<<<<<freen mem %s>>>>>>>>>\n", DISPLAY_BUFFER_HD); //651
		HIFB_INFO("<<<<<<<<freen mem %s>>>>>>>>>\n", OPTM_GFX_WBC2_BUFFER); //652
	}
	else
	{
		ps_PdmExportFuncs->pfnPDM_ReleaseReserveMem(DISPLAY_BUFFER_SD);
		HIFB_INFO("<<<<<<<<freen mem %s>>>>>>>>>\n", DISPLAY_BUFFER_SD); //657
	}

	if (Data_8114374c == 0)
	{
		ps_PdmExportFuncs->pfnPDM_ReleaseReserveMem(HIFB_ZME_COEF_BUFFER);
		HIFB_INFO("<<<<<<<<freen mem %s>>>>>>>>>\n", HIFB_ZME_COEF_BUFFER); //663
	}
    return;
}

static HI_S32 hifb_logo_init(HI_VOID)
{
    HI_S32 s32Ret;
    HIFB_OSD_DATA_S pstLogoLayerData; 
    HI_S32 i;
    
    Data_8114374c = 0;

    for (i = 0; i < 2; i++)
    {
		Data_81143750[i].Data_4 = 0;
		Data_81143750[i].u32LayerId = 2 + i*2; //HIFB_LOGO_LAYER_ID;

		/* judge whether has logo */
		s32Ret = s_stDrvOps.HIFB_DRV_GetOSDData(Data_81143750[i].u32LayerId, &pstLogoLayerData);
		if (s32Ret != HI_SUCCESS)
		{
			HIFB_ERROR("failed to Get OSDData%d !\n", Data_81143750[i].u32LayerId); //685
			return s32Ret;
		}

		if (pstLogoLayerData.eState == HIFB_LAYER_STATE_ENABLE)
		{
			Data_8114374c++;
			Data_81143750[i].bLogoShow = HI_TRUE;

			s_stDrvOps.HIFB_DRV_SetLayerMaskFlag(Data_81143750[i].u32LayerId, HI_TRUE);

			HIFB_INFO("<<<<<<<<<<<<<start with boot logo, ID:%d>>>>>>>>>>>>>>>\n", Data_81143750[i].u32LayerId); //696
		}
    }

    return HI_SUCCESS;
}

static HI_VOID hifb_clear_logo(HI_U32 layerId)
{        
	HI_BOOL i = (layerId > 3); //? 0: 1;

	if (Data_81143750[i].bLogoShow == HI_FALSE)
	{
		return;
	}

	layerId = Data_81143750[i].u32LayerId;

    s_stDrvOps.HIFB_DRV_SetLayerMaskFlag(layerId, HI_FALSE);
    s_stDrvOps.HIFB_DRV_ClearLogo(layerId);
    s_stDrvOps.HIFB_DRV_UpdataLayerReg(layerId);

    Data_8114374c--;

#if 0
	if (in_interrupt())
	{
		INIT_WORK(&s_FreeLogoMemWork, hifb_freelogomem_work);
    	schedule_work(&s_FreeLogoMemWork);    
	}
	else
#endif
	{
		hifb_freelogomem_work(HI_NULL, i);
	}
    
    Data_81143750[i].bLogoShow = HI_FALSE;
    Data_81143750[i].Data_4 = 1;
	
    HIFB_INFO("<<<<<<<<<<<<<hifb_clear_logo>>>>>>>>>>>>>>>\n"); //736
}
#endif

/*****************************************************************************
 Prototype    : hifb_getfmtbyargb
 Description  : get pixel format by argb bitfield
 Input        : struct fb_bitfield red
                struct fb_bitfield green
                struct fb_bitfield blue
                struct fb_bitfield transp
 Output       : None
 Return Value : static
 Calls        :
 Called By    :

  History        :
  1.Date         : 2006/11/9
    Author       : w54130
    Modification : Created function

*****************************************************************************/
static HIFB_COLOR_FMT_E hifb_getfmtbyargb(struct fb_bitfield *red, struct fb_bitfield *green,
                                          struct fb_bitfield *blue, struct fb_bitfield *transp, HI_U32 u32ColorDepth)
{
    HI_U32 i = 0;
    HI_U32 u32Bpp;

    /* not support color palette low than 8bit*/
    if (u32ColorDepth < 8)
    {
        return  HIFB_FMT_BUTT;   
    }
    
    if (u32ColorDepth == 8)
    {
        return HIFB_FMT_8BPP;
    }

    for (i = 0; i < sizeof(s_stArgbBitField)/sizeof(HIFB_ARGB_BITINFO_S); i++)
    {
        if ((hifb_bitfieldcmp(*red, s_stArgbBitField[i].stRed) == 0)
            && (hifb_bitfieldcmp(*green, s_stArgbBitField[i].stGreen) == 0)
            && (hifb_bitfieldcmp(*blue, s_stArgbBitField[i].stBlue) == 0)
            && (hifb_bitfieldcmp(*transp, s_stArgbBitField[i].stTransp) == 0))
        {
            u32Bpp = hifb_getbppbyfmt(i);
            if (u32Bpp == u32ColorDepth)
            {
                return i;
            }
        }
    } 
    i = HIFB_FMT_BUTT;
    switch(u32ColorDepth)
    {
        case 16:
            i = HIFB_FMT_RGB565;
            break;
        case 24:
            i = HIFB_FMT_RGB888;
            break;
        case 32:
            i = HIFB_FMT_ARGB8888;
            break;
        default:
            i = HIFB_FMT_BUTT;
            break;            
    }
    if(HIFB_FMT_BUTT != i)
    {
        *red = s_stArgbBitField[i].stRed;
        *green = s_stArgbBitField[i].stGreen;
        *blue = s_stArgbBitField[i].stBlue;
        *transp = s_stArgbBitField[i].stTransp;
    }
    return i;
}

HI_S32 hifb_realloc_layermem(struct fb_info *info,HI_U32 u32BufSize)
{
	HI_CHAR name[32];
	HIFB_PAR_S *par;

	if (HI_NULL == info)
	{
		return HI_FAILURE;
	}
	
	par = (HIFB_PAR_S *)info->par;
	
	if (0 == u32BufSize)
	{
		return HI_SUCCESS;
	}
	      
        if (info->screen_base != HI_NULL)
        {
            hifb_buf_ummap(info->screen_base);
        }

        if (info->fix.smem_start != 0)
        {
            hifb_buf_freemem(info->fix.smem_start);
			HIFB_INFO("free the video memory, phyaddr: 0x%lx!\n", info->fix.smem_start);
        }
	
    /*Modify 16 to 32, preventing out of bound.*/  
    /* initialize the fix screen info */
    
	snprintf(name, sizeof(name),"HIFB_Fb%d", par->stBaseInfo.u32LayerID);
    info->fix.smem_start = hifb_buf_allocmem(name, u32BufSize);
    if (0 == info->fix.smem_start)
    {
        HIFB_ERROR("%s:failed to malloc the video memory, size: %d KBtyes!\n", name, u32BufSize/1024);
        return HI_FAILURE;
    }
    else
    {
        info->fix.smem_len = u32BufSize;

        /* initialize the virtual address and clear memory */
        info->screen_base = hifb_buf_map(info->fix.smem_start);
        if (HI_NULL == info->screen_base)
        {
            HIFB_WARNING("Failed to call map video memory, "
                     "size:%d KBytes, start: 0x%lx\n",
                     info->fix.smem_len/1024, info->fix.smem_start);
        }
        else
        {
            memset(info->screen_base, 0x00, info->fix.smem_len);
        }
		
		HIFB_INFO("%s:success to malloc the video memory, size: %d KBtyes!\n", name, u32BufSize/1024);
    }

	return HI_SUCCESS;
}


HI_S32 hifb_checkmem_enough(struct fb_info *info,HI_U32 u32Pitch,HI_U32 u32Height)
{
    HI_U32 u32BufferNum = 0;
    HI_U32 u32Buffersize = 0;
    HIFB_PAR_S *par = (HIFB_PAR_S *)info->par;
    
    switch(par->stExtendInfo.enBufMode)
    {
        case HIFB_LAYER_BUF_DOUBLE:
        case HIFB_LAYER_BUF_DOUBLE_IMMEDIATE:
        {
            u32BufferNum = 2;
            break;
        }
        case HIFB_LAYER_BUF_ONE:
		case HIFB_LAYER_BUF_STANDARD:
        {
            u32BufferNum = 1;
            break;
        }
        default:
            return HI_SUCCESS;
    }
	
    u32Buffersize = u32BufferNum * u32Pitch * u32Height;
	
    if(info->fix.smem_len >= u32Buffersize)
    {
        return HI_SUCCESS;
    }

	HIFB_ERROR("memory is not enough!  now is %d u32Pitch %d u32Height %d expect %d\n",info->fix.smem_len,u32Pitch, u32Height,u32Buffersize);
    
    return HI_FAILURE;
}

static HI_S32 hifb_check_fmt(struct fb_var_screeninfo *var, struct fb_info *info)
{
    HI_U32 u32MaxXRes = 0;
    HI_U32 u32MaxYRes = 0;
    HIFB_COLOR_FMT_E enFmt;
    HIFB_PAR_S *par;
	HI_U32 u32LayerID;

	par        = (HIFB_PAR_S *)info->par;
	u32LayerID = par->stBaseInfo.u32LayerID;
        
    enFmt = hifb_getfmtbyargb(&var->red, &var->green, &var->blue, &var->transp, var->bits_per_pixel);
    if (enFmt == HIFB_FMT_BUTT)
    {
        HIFB_ERROR("Unknown fmt(offset, length) r:(%d,%d,%d) ,g:(%d,%d,%d), b(%d,%d,%d), a(%d,%d,%d), bpp:%d!\n",
            var->red.offset, var->red.length, var->red.msb_right,
            var->green.offset, var->green.length, var->green.msb_right,
            var->blue.offset, var->blue.length, var->blue.msb_right,
            var->transp.offset, var->transp.length, var->transp.msb_right,
            var->bits_per_pixel);
        return -EINVAL;
    }

    if ((!g_pstCap[par->stBaseInfo.u32LayerID].bColFmt[enFmt])
        || (!s_stDrvTdeOps.HIFB_DRV_TdeSupportFmt(enFmt) &&
        	par->stExtendInfo.enBufMode != HIFB_LAYER_BUF_STANDARD))
    {
        HIFB_ERROR("Unsupported PIXEL FORMAT!\n");
        return -EINVAL;
    }

    /* virtual resolution must be no less than minimal resolution */
    if (var->xres_virtual < HIFB_MIN_WIDTH(u32LayerID))
    {
        var->xres_virtual = HIFB_MIN_WIDTH(u32LayerID);
    }

    if (var->yres_virtual < HIFB_MIN_HEIGHT(u32LayerID))
    {
        var->yres_virtual = HIFB_MIN_HEIGHT(u32LayerID);
    }

    /* just needed to campare display resolution with virtual resolution, because VO graphic layer can do scaler,
    display resolution >current standard resolution*/  
    u32MaxXRes = var->xres_virtual;
    if (var->xres > u32MaxXRes)
    {
        var->xres = u32MaxXRes;
    }
    else if (var->xres < HIFB_MIN_WIDTH(u32LayerID))
    {
        var->xres = HIFB_MIN_WIDTH(u32LayerID);
    }
    
    u32MaxYRes = var->yres_virtual;
    if (var->yres > u32MaxYRes)
    {
        var->yres = u32MaxYRes;
    }
    else if (var->yres < HIFB_MIN_HEIGHT(u32LayerID))
    {
        var->yres = HIFB_MIN_HEIGHT(u32LayerID);
    }

    HIFB_INFO("xres:%d, yres:%d, xres_virtual:%d, yres_virtual:%d\n",
        var->xres, var->yres, var->xres_virtual, var->yres_virtual);

    /*check if the offset is valid*/
    if ((var->xoffset > var->xres_virtual)
        || (var->yoffset > var->yres_virtual)
        || (var->xoffset + var->xres > var->xres_virtual)
        || (var->yoffset + var->yres > var->yres_virtual))
    {
        HIFB_ERROR("offset is invalid!xoffset:%d, yoffset:%d\n", var->xoffset, var->yoffset);
        return -EINVAL;
    }
	
	return HI_SUCCESS;
}


/******************************************************************************
 Function        : hifb_check_var
 Description     : check if the paramter for framebuffer is supported.
 Data Accessed   :
 Data Updated    :
 Output          : None
 Input           : struct fb_var_screeninfo *var
                   struct fb_info *info
 Return          : return 0, if the paramter is supported, otherwise,return error
                   code.
 Others          : 0
******************************************************************************/
static HI_S32 hifb_check_var(struct fb_var_screeninfo *var, struct fb_info *info)
{
#if 1
	printk("hifb_check_var\n");
#else
    HIFB_PAR_S *pstPar = (HIFB_PAR_S *)info->par;
    if (pstPar->stBaseInfo.u32LayerID == HIFB_LAYER_CURSOR)
    {
        HIFB_ERROR("cursor layer doesn't support this operation!\n");
        return HI_FAILURE;
    }
    return hifb_check_fmt(var, info);
#endif
}


/******************************************************************************
 Function             : hifb_3DData_Config
 Description         : config 3D Data.
 Data Accessed    :
 Data Updated     :
 Output               : None
 Input                 : HIFB_LAYER_ID_E enLayerId, HIFB_BUFFER_S *pstBuffer, HIFB_BLIT_OPT_S *pstBlitOpt
 Return               : return 0
 Others               : 0
******************************************************************************/
#ifdef CFG_HIFB_3D_ENABLE
static HI_S32 hifb_3DData_Config(HIFB_LAYER_ID_E enLayerId, HIFB_BUFFER_S *pstBuffer, HIFB_BLIT_OPT_S *pstBlitOpt)
{
	HIFB_PAR_S *pstPar;
	struct fb_info *info;
	HI_S32 s32Ret;
	unsigned long lockflag;
	
	HIFB_BUFFER_S st3DBuf;

	info   = s_stLayer[enLayerId].pstInfo;
	pstPar = (HIFB_PAR_S *)(info->par);

	spin_lock_irqsave(&pstPar->stBaseInfo.lock,lockflag);
	pstPar->stRunInfo.bNeedFlip        = HI_FALSE;
	pstPar->stRunInfo.s32RefreshHandle = 0;
	spin_unlock_irqrestore(&pstPar->stBaseInfo.lock,lockflag); 

	/**config 3D buffer which set to hardware*/
	memcpy(&st3DBuf.stCanvas, &pstPar->st3DInfo.st3DSurface, sizeof(HIFB_SURFACE_S));
#if 0
	memset(&st3DBuf, 0x0, sizeof(st3DBuf));
	st3DBuf.stCanvas.enFmt      = pstPar->st3DInfo.st3DSurface.enFmt;
	st3DBuf.stCanvas.u32Pitch   = pstPar->st3DInfo.st3DSurface.u32Pitch;	
	st3DBuf.stCanvas.u32Width   = pstPar->st3DInfo.st3DSurface.u32Width;
	st3DBuf.stCanvas.u32Height  = pstPar->st3DInfo.st3DSurface.u32Height;

	if (pstPar->stExtendInfo.enBufMode == HIFB_LAYER_BUF_NONE
		|| pstPar->stExtendInfo.enBufMode == HIFB_LAYER_BUF_ONE)
	{
		st3DBuf.stCanvas.u32PhyAddr = pstPar->st3DInfo.st3DSurface.u32PhyAddr;
	}
	else
	{
		st3DBuf.stCanvas.u32PhyAddr = pstPar->stDispInfo.u32DisplayAddr[pstPar->stRunInfo.u32IndexForInt];
	}	
#endif	
	/**end*/

	/*Left Eye Region*/   	   
	if (HIFB_STEREO_SIDEBYSIDE_HALF == pstPar->st3DInfo.enOutStereoMode)
	{
		st3DBuf.stCanvas.u32Width >>= 1;
	}
	else if (HIFB_STEREO_TOPANDBOTTOM == pstPar->st3DInfo.enOutStereoMode)
	{
		st3DBuf.stCanvas.u32Height >>= 1;
	}

	st3DBuf.UpdateRect.x = 0;
	st3DBuf.UpdateRect.y = 0;
	st3DBuf.UpdateRect.w = st3DBuf.stCanvas.u32Width;
	st3DBuf.UpdateRect.h = st3DBuf.stCanvas.u32Height;

    //printk("3D data blit ..\n");
	//printk("Disp Buffer, addr 0x%x, w %d, h %d, pitch %d fmt %d..\n",
	//		stDispBuf.stCanvas.u32PhyAddr,stDispBuf.stCanvas.u32Width,stDispBuf.stCanvas.u32Height,
	//		stDispBuf.stCanvas.u32Pitch,stDispBuf.stCanvas.enFmt);
	//printk("st3DBuf Buffer, addr 0x%x, w %d, h %d, pitch %d fmt %d..\n",
	//	st3DBuf.stCanvas.u32PhyAddr,st3DBuf.stCanvas.u32Width,st3DBuf.stCanvas.u32Height,
	//	st3DBuf.stCanvas.u32Pitch ,st3DBuf.stCanvas.enFmt);
	s32Ret = s_stDrvTdeOps.HIFB_DRV_Blit(pstBuffer, &st3DBuf, pstBlitOpt, HI_TRUE);
	if (s32Ret < 0)
	{
	    HIFB_ERROR("tde blit error!\n");
	    return HI_FAILURE;
	} 

	pstPar->stRunInfo.bModifying          = HI_TRUE;
	pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_STRIDE;
	pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_REFRESH;
	pstPar->stRunInfo.bModifying          = HI_FALSE;

	pstPar->stRunInfo.s32RefreshHandle = s32Ret;
	return HI_SUCCESS;
}
#endif

static HI_VOID hifb_assign_dispbuf(HI_U32 u32LayerId)
{
    struct fb_info *info = s_stLayer[u32LayerId].pstInfo;
    HIFB_PAR_S *pstPar = (HIFB_PAR_S *)(info->par);
    HI_U32 u32BufSize = 0;

#ifdef CFG_HIFB_3D_ENABLE
    //if ((IS_STEREO_SBS(pstPar) || IS_STEREO_TAB(pstPar)))
    if (pstPar->bSetStereoMode)
    {       
    	HI_U32 u32Stride;
        HI_U32 u32StartAddr;
        
        /* there's a limit from hardware that screen buf shoule be 16 bytes aligned,maybe it's proper
                  to get this info from drv adapter*/
        u32Stride  = ((info->var.xres * info->var.bits_per_pixel >> 3) + 0xf) & 0xfffffff0;
        u32BufSize = ((u32Stride * info->var.yres)+0xf)&0xfffffff0;

		/****in 2buf and 1buf refresh mode, we can use N3D buffer to save 3D data*******/
		if (IS_2BUF_MODE(pstPar) || IS_1BUF_MODE(pstPar))
		{
			u32StartAddr = info->fix.smem_start;
		}
		else if ((0 == pstPar->st3DInfo.st3DMemInfo.u32StereoMemStart) 
                || (0 == pstPar->st3DInfo.st3DMemInfo.u32StereoMemLen)
                || (0 == pstPar->stRunInfo.u32BufNum))
		{
			return;
		}
		else
		{
			u32StartAddr = pstPar->st3DInfo.st3DMemInfo.u32StereoMemStart;
		}

		if (1 == pstPar->stRunInfo.u32BufNum)
        {
            pstPar->st3DInfo.u32DisplayAddr[0] = u32StartAddr;
			pstPar->st3DInfo.u32DisplayAddr[1] = u32StartAddr;
        }
        else if (2 == pstPar->stRunInfo.u32BufNum)
        {
            pstPar->st3DInfo.u32DisplayAddr[0] = u32StartAddr;
            pstPar->st3DInfo.u32DisplayAddr[1] = u32StartAddr + u32BufSize;
        }
    }
    else
#endif		
    {
        /* there's a limit from hardware that screen buf shoule be 16 bytes aligned,maybe it's proper
             to get this info from drv adapter*/
        u32BufSize = ((info->fix.line_length * info->var.yres)+0xf)&0xfffffff0;

        if (info->fix.smem_len == 0)
        {
            return;
        }
        else if ((info->fix.smem_len >= u32BufSize) && (info->fix.smem_len < u32BufSize * 2))
        {
            pstPar->stDispInfo.u32DisplayAddr[0] = info->fix.smem_start;
            pstPar->stDispInfo.u32DisplayAddr[1] = info->fix.smem_start;			 
        }
        else if (info->fix.smem_len >= u32BufSize * 2)
        {
            pstPar->stDispInfo.u32DisplayAddr[0] = info->fix.smem_start;
            pstPar->stDispInfo.u32DisplayAddr[1] = info->fix.smem_start + u32BufSize;
        }
    }

    return;
}

/*1buf refresh*/
static HI_S32 hifb_refresh_1buf(HI_U32 u32LayerId, HIFB_BUFFER_S *pstCanvasBuf)
{
	HI_S32 s32Ret; 	
    HIFB_PAR_S *pstPar;              
    HIFB_BUFFER_S stDisplayBuf;
	struct fb_info *info;
	HIFB_OSD_DATA_S stOsdData;
	HIFB_BLIT_OPT_S stBlitOpt;

	info   = s_stLayer[u32LayerId].pstInfo;
	pstPar = (HIFB_PAR_S *)info->par;

    memset(&stBlitOpt,    0, sizeof(HIFB_BLIT_OPT_S));
    memset(&stDisplayBuf, 0, sizeof(HIFB_BUFFER_S));

    stDisplayBuf.stCanvas.enFmt      = pstPar->stExtendInfo.enColFmt;
    stDisplayBuf.stCanvas.u32Height  = pstPar->stExtendInfo.u32DisplayHeight;
    stDisplayBuf.stCanvas.u32Width   = pstPar->stExtendInfo.u32DisplayWidth;
    stDisplayBuf.stCanvas.u32Pitch   = info->fix.line_length;
    stDisplayBuf.stCanvas.u32PhyAddr = pstPar->stDispInfo.u32DisplayAddr[0];
	
    s_stDrvOps.HIFB_DRV_GetOSDData(u32LayerId, &stOsdData);

    /*if display address is not the same as inital address, please config it use old address*/
    if (stOsdData.u32RegPhyAddr != pstPar->stDispInfo.u32DisplayAddr[0] &&
        pstPar->stDispInfo.u32DisplayAddr[0]) 
    {
        pstPar->stRunInfo.bModifying = HI_TRUE;
        pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_DISPLAYADDR;
		pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_REFRESH;
        pstPar->stRunInfo.u32ScreenAddr = pstPar->stDispInfo.u32DisplayAddr[0];
        memset(info->screen_base, 0x00, info->fix.smem_len);
        pstPar->stRunInfo.bModifying = HI_FALSE;
    } 

    if (pstPar->stBaseInfo.enAntiflickerMode == HIFB_ANTIFLICKER_TDE)
    {
        stBlitOpt.enAntiflickerLevel = pstPar->stBaseInfo.enAntiflickerLevel;
    }

    if (pstCanvasBuf->stCanvas.u32Height != pstPar->stExtendInfo.u32DisplayHeight
        || pstCanvasBuf->stCanvas.u32Width != pstPar->stExtendInfo.u32DisplayWidth)
    {
        stBlitOpt.bScale = HI_TRUE;
        
        stDisplayBuf.UpdateRect.x = 0;
        stDisplayBuf.UpdateRect.y = 0;
        stDisplayBuf.UpdateRect.w = stDisplayBuf.stCanvas.u32Width;
        stDisplayBuf.UpdateRect.h = stDisplayBuf.stCanvas.u32Height;
    }
    else
    {
        stDisplayBuf.UpdateRect = pstCanvasBuf->UpdateRect;
    }

    stBlitOpt.bRegionDeflicker = HI_TRUE;
	
    s32Ret = s_stDrvTdeOps.HIFB_DRV_Blit(pstCanvasBuf, &stDisplayBuf, &stBlitOpt, HI_TRUE);
    if (s32Ret <= 0)
    {
        HIFB_ERROR("hifb_refresh_1buf blit err 5!\n");
        return HI_FAILURE;
    }
    
    memcpy(&(pstPar->stDispInfo.stUserBuffer), pstCanvasBuf, sizeof(HIFB_BUFFER_S));
	
    return HI_SUCCESS;
}

#ifdef CFG_HIFB_3D_ENABLE
static HI_S32 hifb_refresh_1buf_3D(HI_U32 u32LayerId, HIFB_BUFFER_S *pstCanvasBuf)
{
	HIFB_PAR_S *pstPar; 			 
	struct fb_info *info;
	HIFB_BLIT_OPT_S stBlitOpt;
	HIFB_OSD_DATA_S stOsdData;

	info   = s_stLayer[u32LayerId].pstInfo;
	pstPar = (HIFB_PAR_S *)info->par;
	
	s_stDrvOps.HIFB_DRV_GetOSDData(u32LayerId, &stOsdData);

	/*if display address is not the same as inital address, please config it use old address*/
	if (stOsdData.u32RegPhyAddr!= pstPar->stDispInfo.u32DisplayAddr[0] &&
		pstPar->stDispInfo.u32DisplayAddr[0]) 
	{
		pstPar->stRunInfo.bModifying = HI_TRUE;
		pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_DISPLAYADDR;
		pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_REFRESH;
		pstPar->stRunInfo.u32ScreenAddr = pstPar->stDispInfo.u32DisplayAddr[0];
		memset(info->screen_base, 0x00, info->fix.smem_len);
		pstPar->stRunInfo.bModifying = HI_FALSE;
	} 

	/*no need to allocate 3D buffer, displaybuf[0] will be setted to be 3D buffer*/
	/*config 3D surface par*/
	pstPar->st3DInfo.st3DSurface.enFmt     = pstPar->stExtendInfo.enColFmt;
	pstPar->st3DInfo.st3DSurface.u32Pitch  = ((((pstPar->stExtendInfo.u32DisplayWidth * info->var.bits_per_pixel) >> 3) + 0xf) & 0xfffffff0);
	pstPar->st3DInfo.st3DSurface.u32Width  = pstPar->stExtendInfo.u32DisplayWidth;
	pstPar->st3DInfo.st3DSurface.u32Height = pstPar->stExtendInfo.u32DisplayHeight;
	pstPar->st3DInfo.st3DSurface.u32PhyAddr= pstPar->st3DInfo.u32DisplayAddr[0];

	memset(&stBlitOpt, 0, sizeof(HIFB_BLIT_OPT_S));

	stBlitOpt.bRegionDeflicker = HI_TRUE;
	stBlitOpt.bScale           = HI_TRUE;
	if (pstPar->stBaseInfo.enAntiflickerMode == HIFB_ANTIFLICKER_TDE)
	{
		stBlitOpt.enAntiflickerLevel = pstPar->stBaseInfo.enAntiflickerLevel;
	}

	hifb_3DData_Config(u32LayerId, pstCanvasBuf, &stBlitOpt);

	/*backup usr buffer*/
	memcpy(&(pstPar->stDispInfo.stUserBuffer), pstCanvasBuf, sizeof(HIFB_BUFFER_S));
	
	return HI_SUCCESS;
}
#endif

/*get layer's update rect*/
static HI_S32 hifb_getupdate_rect(HI_U32 u32LayerId, HIFB_BUFFER_S *pstCanvasBuf, HIFB_RECT *pstUpdateRect)
{
	HIFB_PAR_S *pstPar;
    struct fb_info *info;    
	TDE2_RECT_S SrcRect ={0}, DstRect={0}, InSrcRect ={0}, InDstRect ={0};

	info     = s_stLayer[u32LayerId].pstInfo;
	pstPar   = (HIFB_PAR_S *)info->par;
	
	memset(&InDstRect, 0, sizeof(TDE2_RECT_S));

	SrcRect.u32Width  = pstCanvasBuf->stCanvas.u32Width;
	SrcRect.u32Height = pstCanvasBuf->stCanvas.u32Height;
	if (pstPar->st3DInfo.enOutStereoMode == HIFB_STEREO_SIDEBYSIDE_HALF)
	{
		DstRect.u32Width  = pstPar->stExtendInfo.u32DisplayWidth >> 1;
		DstRect.u32Height = pstPar->stExtendInfo.u32DisplayHeight;
	}
	else if (pstPar->st3DInfo.enOutStereoMode == HIFB_STEREO_TOPANDBOTTOM)
	{
		DstRect.u32Width  = pstPar->stExtendInfo.u32DisplayWidth;
		DstRect.u32Height = pstPar->stExtendInfo.u32DisplayHeight >> 1;
	}
	else
	{
		DstRect.u32Width  = pstPar->stExtendInfo.u32DisplayWidth;
		DstRect.u32Height = pstPar->stExtendInfo.u32DisplayHeight;
	}

	if (SrcRect.u32Width != DstRect.u32Width
		|| SrcRect.u32Height != DstRect.u32Height)
	{
		memcpy(&InSrcRect, &pstCanvasBuf->UpdateRect, sizeof(HIFB_RECT));
	    s_stDrvTdeOps.HIFB_DRV_CalScaleRect(&SrcRect, &DstRect, &InSrcRect, &InDstRect);
		memcpy(pstUpdateRect, &InDstRect, sizeof(HIFB_RECT));
	}
	else
	{
		memcpy(pstUpdateRect, &pstCanvasBuf->UpdateRect, sizeof(HIFB_RECT));
	}		

	return HI_SUCCESS;
}

/*backup fore buffer*/
static HI_S32 hifb_backup_forebuf(HI_U32 u32LayerId, HIFB_BUFFER_S *pstBackBuf)
{
	HI_S32 s32Ret;	
	HI_U32 u32ForePhyAddr;
	HIFB_PAR_S *pstPar;
	HIFB_RECT  *pstForeUpdateRect;
    struct fb_info *info;
	HIFB_BUFFER_S stForeBuf;
	HIFB_BUFFER_S stBackBuf;
	HIFB_BLIT_OPT_S stBlitTmp;

	info   = s_stLayer[u32LayerId].pstInfo;
	pstPar = (HIFB_PAR_S *)info->par;

	memcpy(&stBackBuf, pstBackBuf, sizeof(HIFB_BUFFER_S));

	if (pstPar->st3DInfo.enOutStereoMode != HIFB_STEREO_MONO
		&& pstPar->st3DInfo.enOutStereoMode != HIFB_STEREO_BUTT)
	{
		pstForeUpdateRect = &pstPar->st3DInfo.st3DUpdateRect;
		u32ForePhyAddr= pstPar->st3DInfo.u32DisplayAddr[1-pstPar->stRunInfo.u32IndexForInt];
	}
	else
	{
		pstForeUpdateRect = &pstPar->stDispInfo.stUpdateRect;
		u32ForePhyAddr= pstPar->stDispInfo.u32DisplayAddr[1-pstPar->stRunInfo.u32IndexForInt];
	}
	
	if (pstPar->st3DInfo.enOutStereoMode == HIFB_STEREO_SIDEBYSIDE_HALF)
	{
		stBackBuf.stCanvas.u32Width  = stBackBuf.stCanvas.u32Width >> 1;
	}
	else if (pstPar->st3DInfo.enOutStereoMode == HIFB_STEREO_TOPANDBOTTOM)
	{
		stBackBuf.stCanvas.u32Height = stBackBuf.stCanvas.u32Height >> 1;
	}
    
    /*backup fore buffer*/
    if (!hifb_iscontain(&stBackBuf.UpdateRect, pstForeUpdateRect))
    {
    	memcpy(&stForeBuf, &stBackBuf, sizeof(HIFB_BUFFER_S));
	    stForeBuf.stCanvas.u32PhyAddr = u32ForePhyAddr;
	    memcpy(&stForeBuf.UpdateRect, pstForeUpdateRect, sizeof(HIFB_RECT));        
	    memcpy(&stBackBuf.UpdateRect, &stForeBuf.UpdateRect , sizeof(HIFB_RECT));
	    memset(&stBlitTmp, 0x0, sizeof(stBlitTmp));
	
        s32Ret = s_stDrvTdeOps.HIFB_DRV_Blit(&stForeBuf, &stBackBuf, &stBlitTmp, HI_TRUE);
        if (s32Ret <= 0)
        {
            HIFB_ERROR("2buf  blit err 4!\n");
            return HI_FAILURE;
        }
    }

	return HI_SUCCESS;
}

/*accumulate rectangle from display buffer*/
/* 2 buf refresh */
static HI_S32 hifb_refresh_2buf(HI_U32 u32LayerId, HIFB_BUFFER_S *pstCanvasBuf)
{
	HI_S32 s32Ret;
	HI_U32 u32Index;
	HIFB_PAR_S *pstPar;
    struct fb_info *info;    
	unsigned long lockflag;   	
    HIFB_BUFFER_S stForeBuf;
    HIFB_BUFFER_S stBackBuf;      	
	HIFB_BLIT_OPT_S stBlitOpt;    
    HIFB_OSD_DATA_S stOsdData;

	s32Ret   = 0;
	info     = s_stLayer[u32LayerId].pstInfo;
	pstPar   = (HIFB_PAR_S *)info->par;
	u32Index = pstPar->stRunInfo.u32IndexForInt;
	
    memset(&stBlitOpt, 0, sizeof(HIFB_BLIT_OPT_S));
    memset(&stForeBuf, 0, sizeof(HIFB_BUFFER_S));
    memset(&stBackBuf, 0, sizeof(HIFB_BUFFER_S));
    
    stBlitOpt.bCallBack = HI_TRUE;
	stBlitOpt.pfnCallBack = hifb_tde_callback;
    stBlitOpt.pParam = &(pstPar->stBaseInfo.u32LayerID);

    spin_lock_irqsave(&pstPar->stBaseInfo.lock,lockflag);
    pstPar->stRunInfo.bNeedFlip = HI_FALSE;
    pstPar->stRunInfo.s32RefreshHandle = 0;
    spin_unlock_irqrestore(&pstPar->stBaseInfo.lock,lockflag);

    s_stDrvOps.HIFB_DRV_GetOSDData(u32LayerId, &stOsdData);
    
    stBackBuf.stCanvas.enFmt      = pstPar->stExtendInfo.enColFmt;
	stBackBuf.stCanvas.u32Width   = pstPar->stExtendInfo.u32DisplayWidth;
    stBackBuf.stCanvas.u32Height  = pstPar->stExtendInfo.u32DisplayHeight;    
    stBackBuf.stCanvas.u32Pitch   = info->fix.line_length;
    stBackBuf.stCanvas.u32PhyAddr = pstPar->stDispInfo.u32DisplayAddr[u32Index];

    /* according to the hw arithemetic, calculate  source and Dst fresh rectangle */	
    if ((pstCanvasBuf->stCanvas.u32Height != pstPar->stExtendInfo.u32DisplayHeight)
        || (pstCanvasBuf->stCanvas.u32Width != pstPar->stExtendInfo.u32DisplayWidth))
    {
		
        stBlitOpt.bScale = HI_TRUE;
    }

	hifb_getupdate_rect(u32LayerId, pstCanvasBuf, &stBackBuf.UpdateRect);


    /*We should check is address changed, for make sure that the address configed to the hw reigster is in effect*/	
    if (pstPar->stRunInfo.bFliped && 
            (stOsdData.u32RegPhyAddr == pstPar->stDispInfo.u32DisplayAddr[1-u32Index]))
    { 
    	/*when fill background buffer, we need to backup fore buffer first*/
		hifb_backup_forebuf(u32LayerId, &stBackBuf);		
        /*clear union rect*/
        memset(&(pstPar->stDispInfo.stUpdateRect), 0, sizeof(HIFB_RECT));
        pstPar->stRunInfo.bFliped = HI_FALSE; 
    }

    /* update union rect */
    if ((pstPar->stDispInfo.stUpdateRect.w == 0) || (pstPar->stDispInfo.stUpdateRect.h == 0))
    {
        memcpy(&pstPar->stDispInfo.stUpdateRect, &stBackBuf.UpdateRect, sizeof(HIFB_RECT));
    }
    else
    {
        HIFB_UNITE_RECT(pstPar->stDispInfo.stUpdateRect, stBackBuf.UpdateRect);
    }  

    if (pstPar->stBaseInfo.enAntiflickerMode == HIFB_ANTIFLICKER_TDE)
    {
        stBlitOpt.enAntiflickerLevel = pstPar->stBaseInfo.enAntiflickerLevel;
    }

    if (stBlitOpt.bScale == HI_TRUE)
    {
        /*actual area, calculate by TDE, here is just use for let pass the test */		
        stBackBuf.UpdateRect.x = 0;
        stBackBuf.UpdateRect.y = 0;
        stBackBuf.UpdateRect.w = stBackBuf.stCanvas.u32Width;
        stBackBuf.UpdateRect.h = stBackBuf.stCanvas.u32Height;
    }
    else
    {
        stBackBuf.UpdateRect = pstCanvasBuf->UpdateRect;
    }

    stBlitOpt.bRegionDeflicker = HI_TRUE;    
    /* blit with refresh rect */
    s32Ret = s_stDrvTdeOps.HIFB_DRV_Blit(pstCanvasBuf, &stBackBuf,&stBlitOpt, HI_TRUE);
    if (s32Ret <= 0)
    {
        HIFB_ERROR("2buf blit err7!\n");
        goto RET;
    }
    
    pstPar->stRunInfo.s32RefreshHandle = s32Ret;

    memcpy(&(pstPar->stDispInfo.stUserBuffer), pstCanvasBuf, sizeof(HIFB_BUFFER_S));

RET:    
    return HI_SUCCESS;
}

#ifdef CFG_HIFB_3D_ENABLE
static HI_S32 hifb_refresh_2buf_3D(HI_U32 u32LayerId, HIFB_BUFFER_S *pstCanvasBuf)
{
	HI_S32 s32Ret;
	HI_U32 u32Index;
	HIFB_PAR_S *pstPar;
    struct fb_info *info;    
	unsigned long lockflag;   	
    HIFB_BUFFER_S stBackBuf;      	
	HIFB_BLIT_OPT_S stBlitOpt;    
    HIFB_OSD_DATA_S stOsdData;

	s32Ret   = 0;
	info     = s_stLayer[u32LayerId].pstInfo;
	pstPar   = (HIFB_PAR_S *)info->par;
	u32Index = pstPar->stRunInfo.u32IndexForInt;

    memset(&stBlitOpt, 0, sizeof(HIFB_BLIT_OPT_S));
    memset(&stBackBuf, 0, sizeof(HIFB_BUFFER_S));
    
    stBlitOpt.bCallBack = HI_TRUE;
	stBlitOpt.pfnCallBack = hifb_tde_callback;
    stBlitOpt.pParam = &(pstPar->stBaseInfo.u32LayerID);

    spin_lock_irqsave(&pstPar->stBaseInfo.lock,lockflag);
    pstPar->stRunInfo.bNeedFlip        = HI_FALSE;
    pstPar->stRunInfo.s32RefreshHandle = 0;
    spin_unlock_irqrestore(&pstPar->stBaseInfo.lock,lockflag);

    s_stDrvOps.HIFB_DRV_GetOSDData(u32LayerId, &stOsdData);
    
	/*no need to allocate 3D buffer, displaybuf[0] will be setted to be 3D buffer*/
	/*config 3D surface par*/
	pstPar->st3DInfo.st3DSurface.enFmt     = pstPar->stExtendInfo.enColFmt;
	pstPar->st3DInfo.st3DSurface.u32Pitch  = ((((pstPar->stExtendInfo.u32DisplayWidth * info->var.bits_per_pixel) >> 3) + 0xf) & 0xfffffff0);
	pstPar->st3DInfo.st3DSurface.u32Width  = pstPar->stExtendInfo.u32DisplayWidth;
	pstPar->st3DInfo.st3DSurface.u32Height = pstPar->stExtendInfo.u32DisplayHeight;
	pstPar->st3DInfo.st3DSurface.u32PhyAddr= pstPar->st3DInfo.u32DisplayAddr[u32Index];

	memcpy(&stBackBuf.stCanvas, &pstPar->st3DInfo.st3DSurface, sizeof(HIFB_SURFACE_S));

    /* according to the hw arithemetic, calculate  source and Dst fresh rectangle */	
    hifb_getupdate_rect(u32LayerId, pstCanvasBuf, &stBackBuf.UpdateRect);

    /*We should check is address changed, for make sure that the address configed to the hw reigster is in effect*/	
    if (pstPar->stRunInfo.bFliped && 
            (stOsdData.u32RegPhyAddr== pstPar->st3DInfo.u32DisplayAddr[1-u32Index]))
    { 
		/*when fill background buffer, we need to backup fore buffer first*/
		hifb_backup_forebuf(u32LayerId, &stBackBuf);	
        /*clear union rect*/
        memset(&(pstPar->st3DInfo.st3DUpdateRect), 0, sizeof(HIFB_RECT));
        pstPar->stRunInfo.bFliped = HI_FALSE; 
    }

    /* update union rect */
    if ((pstPar->st3DInfo.st3DUpdateRect.w == 0) || (pstPar->st3DInfo.st3DUpdateRect.h == 0))
    {

        memcpy(&pstPar->st3DInfo.st3DUpdateRect, &stBackBuf.UpdateRect, sizeof(HIFB_RECT));
    }
    else
    {	  
        HIFB_UNITE_RECT(pstPar->st3DInfo.st3DUpdateRect, stBackBuf.UpdateRect);
    }  

	stBlitOpt.bScale = HI_TRUE;
	stBlitOpt.bRegionDeflicker = HI_TRUE; 
	if (pstPar->stBaseInfo.enAntiflickerMode == HIFB_ANTIFLICKER_TDE)
	{
		stBlitOpt.enAntiflickerLevel = pstPar->stBaseInfo.enAntiflickerLevel;
	}
	
	hifb_3DData_Config(u32LayerId, pstCanvasBuf, &stBlitOpt);

    memcpy(&(pstPar->stDispInfo.stUserBuffer), pstCanvasBuf, sizeof(HIFB_BUFFER_S));
   
    return HI_SUCCESS;
}
#endif

static HI_S32 hifb_wait_regconfig_work(HIFB_LAYER_ID_E enLayerId)
{
	s_stDrvOps.HIFB_DRV_WaitVBlank(enLayerId);

	return HI_SUCCESS;
}


/*In this function we should wait the new contain has been show on the screen before return, 
and the operations such as address configuration no needed do in interrupt handle*/
static HI_S32 hifb_refresh_2buf_immediate_display(HI_U32 u32LayerId, HIFB_BUFFER_S *pstCanvasBuf)
{
	HI_S32 s32Ret;
	HI_U32 u32Index;
	HIFB_PAR_S *pstPar;
	struct fb_info *info;	 
	unsigned long lockflag; 	
	HIFB_BUFFER_S stForeBuf;
	HIFB_BUFFER_S stBackBuf;		
	HIFB_BLIT_OPT_S stBlitOpt;	  
	HIFB_OSD_DATA_S stOsdData;

	s32Ret	 = 0;
	info	 = s_stLayer[u32LayerId].pstInfo;
	pstPar	 = (HIFB_PAR_S *)info->par;
	u32Index = pstPar->stRunInfo.u32IndexForInt;
	
	memset(&stBlitOpt, 0, sizeof(HIFB_BLIT_OPT_S));
	memset(&stForeBuf, 0, sizeof(HIFB_BUFFER_S));
	memset(&stBackBuf, 0, sizeof(HIFB_BUFFER_S));
	
	stBlitOpt.bCallBack = HI_FALSE;
	stBlitOpt.pParam = &(pstPar->stBaseInfo.u32LayerID);

	spin_lock_irqsave(&pstPar->stBaseInfo.lock,lockflag);
	pstPar->stRunInfo.bNeedFlip = HI_FALSE;
	pstPar->stRunInfo.s32RefreshHandle = 0;
	spin_unlock_irqrestore(&pstPar->stBaseInfo.lock,lockflag);

	s_stDrvOps.HIFB_DRV_GetOSDData(u32LayerId, &stOsdData);
	
	stBackBuf.stCanvas.enFmt	  = pstPar->stExtendInfo.enColFmt;
	stBackBuf.stCanvas.u32Width   = pstPar->stExtendInfo.u32DisplayWidth;
	stBackBuf.stCanvas.u32Height  = pstPar->stExtendInfo.u32DisplayHeight;	  
	stBackBuf.stCanvas.u32Pitch   = info->fix.line_length;
	stBackBuf.stCanvas.u32PhyAddr = pstPar->stDispInfo.u32DisplayAddr[u32Index];

	/* according to the hw arithemetic, calculate  source and Dst fresh rectangle */	
	if ((pstCanvasBuf->stCanvas.u32Height != pstPar->stExtendInfo.u32DisplayHeight)
		|| (pstCanvasBuf->stCanvas.u32Width != pstPar->stExtendInfo.u32DisplayWidth))
	{
		
		stBlitOpt.bScale = HI_TRUE;
	}

	hifb_getupdate_rect(u32LayerId, pstCanvasBuf, &stBackBuf.UpdateRect);

	/*when fill background buffer, we need to backup fore buffer first*/
	hifb_backup_forebuf(u32LayerId, &stBackBuf); 	


	/* update union rect */
	memcpy(&pstPar->stDispInfo.stUpdateRect, &stBackBuf.UpdateRect, sizeof(HIFB_RECT));

	if (pstPar->stBaseInfo.enAntiflickerMode == HIFB_ANTIFLICKER_TDE)
	{
		stBlitOpt.enAntiflickerLevel = pstPar->stBaseInfo.enAntiflickerLevel;
	}

	if (stBlitOpt.bScale == HI_TRUE)
	{
		/*actual area, calculate by TDE, here is just use for let pass the test */		
		stBackBuf.UpdateRect.x = 0;
		stBackBuf.UpdateRect.y = 0;
		stBackBuf.UpdateRect.w = stBackBuf.stCanvas.u32Width;
		stBackBuf.UpdateRect.h = stBackBuf.stCanvas.u32Height;
	}
	else
	{
		stBackBuf.UpdateRect = pstCanvasBuf->UpdateRect;
	}

	stBlitOpt.bRegionDeflicker = HI_TRUE;	 
	/* blit with refresh rect */
	s32Ret = s_stDrvTdeOps.HIFB_DRV_Blit(pstCanvasBuf, &stBackBuf,&stBlitOpt, HI_TRUE);
	if (s32Ret <= 0)
	{
		HIFB_ERROR("2buf blit err7!\n");
		goto RET;
	}
	
    /*set the backup buffer to register and show it  */
    pstPar->stRunInfo.bModifying = HI_TRUE;            
    pstPar->stRunInfo.u32ScreenAddr       = pstPar->stDispInfo.u32DisplayAddr[u32Index];
	pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_DISPLAYADDR;
	pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_REFRESH;
    pstPar->stRunInfo.bModifying = HI_FALSE;

	pstPar->stRunInfo.u32IndexForInt = 1 - u32Index;
    
    memcpy(&(pstPar->stDispInfo.stUserBuffer), pstCanvasBuf, sizeof(HIFB_BUFFER_S));

    /*wait the address register's configuration take effect before return*/

    hifb_wait_regconfig_work(u32LayerId);


RET:	
	return HI_SUCCESS;
}


/*In this function we should wait the new contain has been show on the screen before return, 
and the operations such as address configuration no needed do in interrupt handle*/
#ifdef CFG_HIFB_3D_ENABLE
static HI_S32 hifb_refresh_2buf_immediate_display_3D(HI_U32 u32LayerId, HIFB_BUFFER_S *pstCanvasBuf)
{
	HI_S32 s32Ret;
	HI_U32 u32Index;
	HIFB_PAR_S *pstPar;
	struct fb_info *info;	 
	unsigned long lockflag; 	
	HIFB_BUFFER_S stBackBuf;		
	HIFB_BLIT_OPT_S stBlitOpt;	  

	s32Ret	 = 0;
	info	 = s_stLayer[u32LayerId].pstInfo;
	pstPar	 = (HIFB_PAR_S *)info->par;
	u32Index = pstPar->stRunInfo.u32IndexForInt;

	memset(&stBlitOpt, 0, sizeof(HIFB_BLIT_OPT_S));
	memset(&stBackBuf, 0, sizeof(HIFB_BUFFER_S));
	
	stBlitOpt.bCallBack = HI_FALSE;
	stBlitOpt.pParam = &(pstPar->stBaseInfo.u32LayerID);

	spin_lock_irqsave(&pstPar->stBaseInfo.lock,lockflag);
	pstPar->stRunInfo.bNeedFlip 	   = HI_FALSE;
	pstPar->stRunInfo.s32RefreshHandle = 0;
	spin_unlock_irqrestore(&pstPar->stBaseInfo.lock,lockflag);

	/*no need to allocate 3D buffer, displaybuf[0] will be setted to be 3D buffer*/
	/*config 3D surface par*/
	pstPar->st3DInfo.st3DSurface.enFmt	   = pstPar->stExtendInfo.enColFmt;
	pstPar->st3DInfo.st3DSurface.u32Pitch  = ((((pstPar->stExtendInfo.u32DisplayWidth * info->var.bits_per_pixel) >> 3) + 0xf) & 0xfffffff0);
	pstPar->st3DInfo.st3DSurface.u32Width  = pstPar->stExtendInfo.u32DisplayWidth;
	pstPar->st3DInfo.st3DSurface.u32Height = pstPar->stExtendInfo.u32DisplayHeight;
	pstPar->st3DInfo.st3DSurface.u32PhyAddr= pstPar->st3DInfo.u32DisplayAddr[u32Index];

	memcpy(&stBackBuf.stCanvas, &pstPar->st3DInfo.st3DSurface, sizeof(HIFB_SURFACE_S));

	/* according to the hw arithemetic, calculate  source and Dst fresh rectangle */	
	hifb_getupdate_rect(u32LayerId, pstCanvasBuf, &stBackBuf.UpdateRect);

	/*when fill background buffer, we need to backup fore buffer first*/
	hifb_backup_forebuf(u32LayerId, &stBackBuf); 

	/* update union rect */
	memcpy(&pstPar->st3DInfo.st3DUpdateRect, &stBackBuf.UpdateRect, sizeof(HIFB_RECT));


	stBlitOpt.bScale = HI_TRUE;
	stBlitOpt.bBlock = HI_TRUE;
	stBlitOpt.bRegionDeflicker = HI_TRUE; 
	if (pstPar->stBaseInfo.enAntiflickerMode == HIFB_ANTIFLICKER_TDE)
	{
		stBlitOpt.enAntiflickerLevel = pstPar->stBaseInfo.enAntiflickerLevel;
	}
	
	hifb_3DData_Config(u32LayerId, pstCanvasBuf, &stBlitOpt);

    pstPar->stRunInfo.bModifying = HI_TRUE;            
    pstPar->stRunInfo.u32ScreenAddr       = pstPar->st3DInfo.u32DisplayAddr[u32Index];
	pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_DISPLAYADDR;
	pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_REFRESH;
    pstPar->stRunInfo.bModifying = HI_FALSE;

	pstPar->stRunInfo.u32IndexForInt = 1 - u32Index;
    
    memcpy(&(pstPar->stDispInfo.stUserBuffer), pstCanvasBuf, sizeof(HIFB_BUFFER_S));

    /*wait the address register's configuration take effect before return*/

    hifb_wait_regconfig_work(u32LayerId);
	
	return HI_SUCCESS;
}
#endif

static HI_S32 hifb_disp_setdispsize(HI_U32 u32LayerId, HI_U32 u32Width, HI_U32 u32Height)
{
    struct fb_info *info = s_stLayer[u32LayerId].pstInfo;
    HIFB_PAR_S *pstPar = (HIFB_PAR_S *)info->par;   
    HI_U32 u32Pitch;
	
    if ((pstPar->stExtendInfo.u32DisplayWidth == u32Width) && (pstPar->stExtendInfo.u32DisplayHeight == u32Height))
    {
        return HI_SUCCESS;
    }    
	
    u32Pitch = u32Width * info->var.bits_per_pixel >> 3;
    u32Pitch = (u32Pitch + 0xf) & 0xfffffff0;
	
	if(HI_FAILURE == hifb_checkmem_enough(info, u32Pitch, u32Height))
	{
	   return HI_FAILURE;
	}		  
    
    pstPar->stExtendInfo.u32DisplayWidth  = u32Width;
    pstPar->stExtendInfo.u32DisplayHeight = u32Height;
    
    pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_INRECT;
  
    /* here we need to think about how to resist flicker again, we use VO do flicker resist before , but now if the display H size is the 
           same as the screen, VO will not do flicker resist, so should choose TDE to do flicker resist*/
    hifb_select_antiflicker_mode(pstPar);
    return HI_SUCCESS;
}

  /* we handle it by two case: 
      case 1 : if VO support Zoom, we only change screeen size, display size keep not change
      case 2: if VO can't support zoom, display size should keep the same as screen size*/
static HI_S32 hifb_disp_setscreensize(HI_U32 u32LayerId, HI_U32 u32Width, HI_U32 u32Height)
{
#ifndef CFG_HIFB_VIRTUAL_COORDINATE_SUPPORT
	HIFB_RECT stDispRect;
    struct fb_info *info = s_stLayer[u32LayerId].pstInfo;
    HIFB_PAR_S *pstPar = (HIFB_PAR_S *)info->par;

    if (0 == u32Width || 0 == u32Height)
    {
        return HI_FAILURE;
    }	

    pstPar->stExtendInfo.u32ScreenWidth  = u32Width;
    pstPar->stExtendInfo.u32ScreenHeight = u32Height;

    pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_OUTRECT;

    /* Here  we need to think about how to resist flicker again, we use VO do flicker resist before , but now if the display H size is the 
    	     same as the screen, VO will not do flicker resist, so should choose TDE to do flicker resist*/
    hifb_select_antiflicker_mode(pstPar);
#endif
    return HI_SUCCESS;
}

#if 0
static HI_S32 hifb_defscreensize(HI_U32 u32Width, HI_U32 u32Height, HIFB_SIZE_S *pstSize)
{
	if (u32Width <= 720)
	{
		u32Width  = 720;
		if (u32Height > 480)
		{
			u32Height = 576;
		}
		else
		{
			u32Height = 480;
		}
	}
	else if (u32Width <= 1280)
	{
		u32Width  = 1280;
		u32Height = 720;
	}
	else if (u32Width <= 1920)
	{
		u32Width  = 1920;
		u32Height = 1080;
	}

	pstSize->u32Width  = u32Width;
	pstSize->u32Height = u32Height;	
	
	return HI_SUCCESS;
}

/* we handle it under this principle: 
	case 1 : usr setting
	case 2 : layer init*/
static HI_S32 hifb_adapt_setscreensize(HI_U32 u32LayerId, HI_U32 u32Width, HI_U32 u32Height)
{	
	HIFB_SIZE_S stSize;
	/*screen size was init by usr, using usr's setting*/
	//printk("hifb_adapt_setscreensize w = %d, h = %d..\n",u32Width, u32Height);
	if (s_stDrvOps.HIFB_DRV_GetInitScreenFlag(u32LayerId))
	{
		return HI_SUCCESS;		
	}  

	hifb_defscreensize(u32Width, u32Height, &stSize);	

	//printk("after hifb_adapt_setscreensize w = %d, h = %d..\n",stSize.u32Width, stSize.u32Height);
	if (HI_SUCCESS == hifb_disp_setscreensize(u32LayerId, stSize.u32Width, stSize.u32Height))
	{
		s_stDrvOps.HIFB_DRV_SetInitScreenFlag(u32LayerId, HI_TRUE);
	}

	return HI_SUCCESS;
}
#endif

HI_S32 hifb_freeccanbuf(HIFB_PAR_S *par)
{
    if (HI_NULL != par->stDispInfo.stCanvasSur.u32PhyAddr)
    {
        hifb_buf_freemem(par->stDispInfo.stCanvasSur.u32PhyAddr);
    }
	
    par->stDispInfo.stCanvasSur.u32PhyAddr = 0;
   
    return HI_SUCCESS;
}

#ifdef CFG_HIFB_3D_ENABLE
HI_S32 hifb_freestereobuf(HIFB_PAR_S *par)
{
    if (HI_NULL != par->st3DInfo.st3DMemInfo.u32StereoMemStart)
    {
        hifb_buf_freemem(par->st3DInfo.st3DMemInfo.u32StereoMemStart);
    }
	
    par->st3DInfo.st3DMemInfo.u32StereoMemStart = 0;
    par->st3DInfo.st3DMemInfo.u32StereoMemLen   = 0;    
    par->st3DInfo.st3DSurface.u32PhyAddr        = 0;
    
    return HI_SUCCESS;
}
#endif

#ifdef CFG_HIFB_3D_ENABLE
static HI_VOID hifb_clearstereobuf(struct fb_info *info)
{
    HIFB_PAR_S *par = (HIFB_PAR_S *)info->par;
    
    if (par->st3DInfo.st3DMemInfo.u32StereoMemStart && par->st3DInfo.st3DMemInfo.u32StereoMemLen)
    {
        HIFB_BLIT_OPT_S stOpt;

        memset(&stOpt, 0x0, sizeof(stOpt));
        par->st3DInfo.st3DSurface.u32PhyAddr = par->st3DInfo.st3DMemInfo.u32StereoMemStart;
        s_stDrvTdeOps.HIFB_DRV_ClearRect(&(par->st3DInfo.st3DSurface), &stOpt);
    }
	
    return;
}
#endif

#ifdef CFG_HIFB_3D_ENABLE
static HI_S32 hifb_allocstereobuf(struct fb_info *info, HI_U32 u32BufSize)
{
    HIFB_PAR_S *par = (HIFB_PAR_S *)info->par;
    HI_CHAR name[32] = "";

    if (0 == u32BufSize)
    {
        return HI_FAILURE;
    }

    if (u32BufSize == par->st3DInfo.st3DMemInfo.u32StereoMemLen)
    {
        return HI_SUCCESS;
    }

    if (u32BufSize > info->fix.smem_len)
    {   
        HIFB_ERROR("u32BufSize:%d,  no enough mem(%d)\n", u32BufSize, info->fix.smem_len);
        return HI_FAILURE;
    }

    /** if  with old stereo buffer*/
    if (par->st3DInfo.st3DMemInfo.u32StereoMemStart)
    {
        /** free old buffer*/
        HIFB_INFO("free old stereo buffer\n");        
        hifb_freestereobuf(par);
    }


    /** alloc new stereo buffer*/
    snprintf(name, sizeof(name), "HIFB_StereoBuf%d", par->stBaseInfo.u32LayerID);
    par->st3DInfo.st3DMemInfo.u32StereoMemStart = hifb_buf_allocmem(name, u32BufSize);
	
    if (0 == par->st3DInfo.st3DMemInfo.u32StereoMemStart)
    {   
        HIFB_ERROR("alloc stereo buffer no mem, u32BufSize:%d\n", u32BufSize);
        return HI_FAILURE;
    }
    
    par->st3DInfo.st3DMemInfo.u32StereoMemLen = u32BufSize;    

    HIFB_INFO("alloc new memory for stereo buffer success\n"); 

    return HI_SUCCESS;
}
#endif


/******************************************************************************
 Function        : hifb_set_par
 Description     : set the variable parmater and make it use
 Data Accessed   :
 Data Updated    :
 Output          : None
 Input           : struct fb_info *info
 Return          : return 0
 Others          : 0
******************************************************************************/
static HI_S32 hifb_set_par(struct fb_info *info)
{
#if 1
	printk("hifb_set_par\n");
#else
	HI_S32 s32Ret;
	HI_U32 u32Stride;
	HI_U32 u32BufSize;
    HIFB_PAR_S *pstPar;        
    HIFB_COLOR_FMT_E enFmt;
    HI_U32 u32StartAddr = 0;
    
    pstPar = (HIFB_PAR_S *)info->par;    	
    enFmt = hifb_getfmtbyargb(&info->var.red, &info->var.green, &info->var.blue, &info->var.transp, info->var.bits_per_pixel);
	u32Stride  = ((((info->var.xres_virtual * info->var.bits_per_pixel) >> 3) + 0xf) & 0xfffffff0);
	
	if (!pstPar->bPanFlag)
	{				
		u32BufSize = u32Stride * info->var.yres_virtual;
    	if (u32BufSize > info->fix.smem_len)
    	{
    		s32Ret = hifb_realloc_layermem(info, u32BufSize);
			if (HI_FAILURE == s32Ret)
			{
				return HI_FAILURE;
			}
			
			pstPar->stRunInfo.u32IndexForInt = 0;
			
			hifb_assign_dispbuf(pstPar->stBaseInfo.u32LayerID);

			pstPar->stRunInfo.bModifying          = HI_TRUE;
			pstPar->stRunInfo.u32ScreenAddr       = info->fix.smem_start;
        	pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_DISPLAYADDR;        
        	pstPar->stRunInfo.bModifying          = HI_FALSE;
    	}	
	}
	else
	{		
		s32Ret = hifb_checkmem_enough(info, u32Stride, info->var.yres_virtual);
		if (HI_FAILURE == s32Ret)
		{			
			return HI_FAILURE;
		}
	}

	pstPar->stExtendInfo.enBufMode = HIFB_LAYER_BUF_STANDARD;
	
#ifdef CFG_HIFB_3D_ENABLE
    if (//(IS_STEREO_SBS(pstPar)|| IS_STEREO_TAB(pstPar))
    	pstPar->bSetStereoMode
            && (HIFB_LAYER_BUF_STANDARD == pstPar->stExtendInfo.enBufMode))
    {
    	HI_U32 u32BufferSize;
		/*the stride of 3D buffer*/
    	u32Stride = ((info->var.xres * info->var.bits_per_pixel >> 3) + 0xf) & 0xfffffff0;

		u32BufferSize = u32Stride * info->var.yres; 
		u32BufferSize *= pstPar->stRunInfo.u32BufNum;
				
		/*config 3D surface par*/
		pstPar->st3DInfo.st3DSurface.enFmt    = enFmt;		
		pstPar->st3DInfo.st3DSurface.u32Width = info->var.xres;
		pstPar->st3DInfo.st3DSurface.u32Height= info->var.yres;

		pstPar->stRunInfo.bModifying          = HI_TRUE;
		pstPar->st3DInfo.st3DSurface.u32Pitch = u32Stride;
	    pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_STRIDE;
		pstPar->stRunInfo.bModifying          = HI_FALSE;

		s32Ret = hifb_checkandalloc_3dmem(pstPar->stBaseInfo.u32LayerID, u32BufferSize);
		if (HI_SUCCESS != s32Ret)
		{
			HIFB_INFO("fail to alloc 3d memory, set_fb_par failure.\n ");
			return s32Ret;
		}
		
		/*the stride of N3D buffer*/
        u32Stride = info->var.xres_virtual * info->var.bits_per_pixel >> 3;
        u32Stride = (u32Stride + 0xf) & 0xfffffff0;
        info->fix.line_length = u32Stride;        
    }	
    else	
#endif		
    {        
        /* set the stride if stride change */
        u32Stride = info->var.xres_virtual * info->var.bits_per_pixel >> 3;

        /* 16 byte aligned */
        u32Stride = (u32Stride + 0xf) & 0xfffffff0;
        
        u32StartAddr = info->fix.smem_start ;
        
        if (u32Stride != info->fix.line_length  ||
           (info->var.yres != pstPar->stExtendInfo.u32DisplayHeight))
        {
        	pstPar->stRunInfo.bModifying          = HI_TRUE;
			
            info->fix.line_length = u32Stride;
            hifb_assign_dispbuf(pstPar->stBaseInfo.u32LayerID);
			
            pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_STRIDE;
			pstPar->stRunInfo.bModifying          = HI_FALSE;			
        }               
    }
        
    if ((pstPar->stExtendInfo.enColFmt != enFmt))
    {   			
		hifb_freeccanbuf(pstPar);
        
#ifdef CFG_HIFB_SCROLLTEXT_SUPPORT
		if (s_stTextLayer[pstPar->stBaseInfo.u32LayerID].bAvailable)
		{
			HI_U32 i;
			for (i = 0; i < SCROLLTEXT_NUM; i++)
			{
				if (s_stTextLayer[pstPar->stBaseInfo.u32LayerID].stScrollText[i].bAvailable)
				{
					hifb_freescrolltext_cachebuf(&(s_stTextLayer[pstPar->stBaseInfo.u32LayerID].stScrollText[i]));
					memset(&s_stTextLayer[pstPar->stBaseInfo.u32LayerID].stScrollText[i],0,sizeof(HIFB_SCROLLTEXT_S));
				}
			}
		
			s_stTextLayer[pstPar->stBaseInfo.u32LayerID].bAvailable      = HI_FALSE;
			s_stTextLayer[pstPar->stBaseInfo.u32LayerID].u32textnum      = 0;
			s_stTextLayer[pstPar->stBaseInfo.u32LayerID].u32ScrollTextId = 0;
		}
#endif        
		pstPar->stRunInfo.bModifying  = HI_TRUE;
        pstPar->stExtendInfo.enColFmt = enFmt;
        pstPar->stRunInfo.u32ParamModifyMask  |= HIFB_LAYER_PARAMODIFY_FMT;
		pstPar->stRunInfo.bModifying = HI_FALSE;
    }   
    
    /* If xres or yres change */
    if (info->var.xres != pstPar->stExtendInfo.u32DisplayWidth
        || info->var.yres != pstPar->stExtendInfo.u32DisplayHeight)
    {
        if ((0 == info->var.xres) || (0 == info->var.yres))
        {
            if (HI_TRUE == pstPar->stExtendInfo.bShow)
            {
            	pstPar->stRunInfo.bModifying          = HI_TRUE;				
                pstPar->stExtendInfo.bShow            = HI_FALSE;				
                pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_SHOW;
				pstPar->stRunInfo.bModifying          = HI_FALSE;
            }
        }
		
        hifb_disp_setdispsize  (pstPar->stBaseInfo.u32LayerID, info->var.xres, info->var.yres);
		//hifb_disp_setscreensize(pstPar->stBaseInfo.u32LayerID, info->var.xres, info->var.yres);
		hifb_assign_dispbuf(pstPar->stBaseInfo.u32LayerID);
#if 0		
		/*screen size was setted by usr, using usr's setting*/
		if (/*!(g_u32HifbState & HIFB_STATE_LOGO_IN) && */!s_stDrvOps.HIFB_DRV_GetScreenFlag(pstPar->stBaseInfo.u32LayerID))
		{
			hifb_adapt_setscreensize(pstPar->stBaseInfo.u32LayerID, info->var.xres, info->var.yres);
		}              
#endif		
    }
#endif

    return 0;
}



/******************************************************************************
 Function        : hifb_pan_display
 Description     : pan display.
 Data Accessed   :
 Data Updated    :
 Output          : None
 Input           : struct fb_var_screeninfo *var
 Return          : return 0
 Others          : 0
******************************************************************************/
static HI_S32 hifb_pan_display(struct fb_var_screeninfo *var, struct fb_info *info)
{
#if 1
	printk("hifb_pan_display\n");
#else
    HIFB_PAR_S *par = (HIFB_PAR_S *)info->par;
    HI_U32 u32DisplayAddr = 0;
    HI_U32 u32StartAddr = info->fix.smem_start ;    

	if (HIFB_LAYER_BUF_STANDARD != par->stExtendInfo.enBufMode)
	{
		return HI_SUCCESS;
	}

    /*stereo 3d  mode*/
#ifdef CFG_HIFB_3D_ENABLE
    if (//(IS_STEREO_SBS(par) || IS_STEREO_TAB(par)) 
    	par->bSetStereoMode
           && par->stExtendInfo.enBufMode == HIFB_LAYER_BUF_STANDARD)
    {
    	HI_S32 s32Ret = 0;
    	HI_U32 u32TmpAddr;
		HI_U32 u32Stride;
		HIFB_BLIT_OPT_S stBlitOpt;
		HIFB_BUFFER_S stDispBuf;
		HI_U32 u32BufferSize;
		/*the stride of 3D buffer*/
    	u32Stride = ((info->var.xres * info->var.bits_per_pixel >> 3) + 0xf) & 0xfffffff0;

		u32BufferSize = u32Stride * info->var.yres; 
		u32BufferSize *= par->stRunInfo.u32BufNum;
		
	    /*N3D display buffer address*/
		if (info->var.bits_per_pixel >= 8)
		{
			u32TmpAddr = info->fix.smem_start + info->fix.line_length * info->var.yoffset
		               + info->var.xoffset* (info->var.bits_per_pixel >> 3);
		}
		else
		{
			u32TmpAddr = (info->fix.smem_start + info->fix.line_length * info->var.yoffset
		               + info->var.xoffset * info->var.bits_per_pixel / 8);
		}

	    if((info->var.bits_per_pixel == 24)&&((info->var.xoffset !=0)||(info->var.yoffset !=0)))
	    {
	        HI_U32 TmpData;

	        TmpData = (info->fix.smem_start + info->fix.line_length * info->var.yoffset
	                       + info->var.xoffset * (info->var.bits_per_pixel >> 3))/16/3;
	        u32TmpAddr = TmpData*16*3;
	    }

		/**config N3D display buffer that we blit to 3D buffer*/
		memset(&stDispBuf, 0x0, sizeof(stDispBuf));
		stDispBuf.stCanvas.enFmt      = par->stExtendInfo.enColFmt;
		stDispBuf.stCanvas.u32Pitch   = info->fix.line_length;
		stDispBuf.stCanvas.u32PhyAddr = u32TmpAddr;
		stDispBuf.stCanvas.u32Width   = info->var.xres;
		stDispBuf.stCanvas.u32Height  = info->var.yres;
		
		stDispBuf.UpdateRect.x = 0;
		stDispBuf.UpdateRect.y = 0;
		stDispBuf.UpdateRect.w = info->var.xres;
		stDispBuf.UpdateRect.h = info->var.yres;
		/**end*/

		/**config N3D display buffer that we blit to 3D buffer*/
		memcpy(&par->st3DInfo.st3DSurface, &stDispBuf.stCanvas, sizeof(HIFB_SURFACE_S));
		par->st3DInfo.st3DSurface.u32Pitch = u32Stride;

		s32Ret = hifb_checkandalloc_3dmem(par->stBaseInfo.u32LayerID, u32BufferSize);
		if (HI_SUCCESS != s32Ret)
		{
			HIFB_INFO("fail to alloc 3d memory, pandisplay failure.\n ");
			return s32Ret;
		}
		
		par->st3DInfo.st3DSurface.u32PhyAddr = par->st3DInfo.u32DisplayAddr[par->stRunInfo.u32IndexForInt];
		/**end*/
		
		memset(&stBlitOpt, 0, sizeof(HIFB_BLIT_OPT_S));

		stBlitOpt.bScale = HI_TRUE;
		stBlitOpt.bRegionDeflicker = HI_TRUE; 
		if (par->stBaseInfo.enAntiflickerMode == HIFB_ANTIFLICKER_TDE)
		{
			stBlitOpt.enAntiflickerLevel = par->stBaseInfo.enAntiflickerLevel;
		}
    
		stBlitOpt.bCallBack = HI_TRUE;
		stBlitOpt.pfnCallBack = hifb_tde_callback;
		stBlitOpt.pParam = &(par->stBaseInfo.u32LayerID);
		
		/*blit display buffer to 3D buffer*/
        s32Ret = hifb_3DData_Config(par->stBaseInfo.u32LayerID, &stDispBuf, &stBlitOpt);
		if (HI_SUCCESS != s32Ret)
		{
			HIFB_ERROR("pandisplay config stereo data failure!");
			return HI_FAILURE;
		}
    }	
    else//mono mode
 #endif
    {
        /*set the stride and display start address*/
        if (var->bits_per_pixel >= 8)
        {
            u32DisplayAddr = (u32StartAddr + info->fix.line_length * var->yoffset
                           + var->xoffset * (var->bits_per_pixel >> 3))&0xfffffff0;
        }
        else
        {
            u32DisplayAddr = (u32StartAddr + info->fix.line_length * var->yoffset
                           + var->xoffset * var->bits_per_pixel / 8) & 0xfffffff0;
        }

        if((info->var.bits_per_pixel == 24)&&((info->var.xoffset !=0)||(info->var.yoffset !=0)))
        {
            HI_U32 TmpData;
    
            TmpData = (u32StartAddr + info->fix.line_length * var->yoffset
                           + var->xoffset * (var->bits_per_pixel >> 3))/16/3;
            u32DisplayAddr = TmpData*16*3;
    
        }
        
        par->stRunInfo.bModifying          = HI_TRUE;
		par->stRunInfo.u32ScreenAddr       = u32DisplayAddr;
		par->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_REFRESH;
        par->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_DISPLAYADDR;        
        par->stRunInfo.bModifying          = HI_FALSE;
        
        /*if the flag "FB_ACTIVATE_VBL" has been set, we should wait forregister update finish*/
        if (var->activate & FB_ACTIVATE_VBL)
        {
            hifb_wait_regconfig_work(par->stBaseInfo.u32LayerID);
        }
    }

 #ifdef CFG_HIFB_LOGO_SUPPORT		
	if (g_bLogoShow)
	{	
		HIFB_INFO("<<<<<<has logo,show app>>>>>>>>\n");
		hifb_wait_regconfig_work(par->stBaseInfo.u32LayerID);
		hifb_clear_logo(0);
	}
#endif	

 	if (!par->bPanFlag)
 	{
 		par->bPanFlag = HI_TRUE;
 	}
#endif
    
    return HI_SUCCESS;
}


/******************************************************************************
 Function             : hifb_checkandalloc_3dmem
 Description         : config 3D par.
 Data Accessed    :
 Data Updated     :
 Output               : None
 Input                 : HIFB_LAYER_ID_E enLayerId
 Return               : return 0
 Others               : 0
******************************************************************************/
#ifdef CFG_HIFB_3D_ENABLE
static HI_S32 hifb_checkandalloc_3dmem(HIFB_LAYER_ID_E enLayerId, HI_U32 u32BufferSize)
{
	HI_S32 s32Ret;
    HIFB_PAR_S *pstPar;        
	struct fb_info *info;

	info = s_stLayer[enLayerId].pstInfo;
    pstPar = (HIFB_PAR_S *)info->par;    

	if (//HIFB_STEREO_FRMPACKING == pstPar->st3DInfo.enOutStereoMode
		 HIFB_STEREO_MONO == pstPar->st3DInfo.enOutStereoMode)
	{
		return HI_SUCCESS;
	}

	if (pstPar->stExtendInfo.enBufMode != HIFB_LAYER_BUF_NONE
		&& pstPar->stExtendInfo.enBufMode != HIFB_LAYER_BUF_STANDARD)
	{
		return HI_SUCCESS;
	}

	/*1: allocate 3D buffer*/
	if (u32BufferSize > pstPar->st3DInfo.st3DMemInfo.u32StereoMemLen)
	{
		s32Ret = hifb_allocstereobuf(info, u32BufferSize);
        if (s32Ret != HI_SUCCESS)
        {
            HIFB_ERROR("alloc 3D buffer failure!, expect mem size: %d\n", u32BufferSize);
            return s32Ret;
        }
		
		pstPar->st3DInfo.st3DSurface.u32PhyAddr = pstPar->st3DInfo.st3DMemInfo.u32StereoMemStart;
		pstPar->st3DInfo.u32rightEyeAddr        = pstPar->st3DInfo.st3DSurface.u32PhyAddr;		
        pstPar->stRunInfo.u32IndexForInt        = 0;

		hifb_clearstereobuf  (info);
		hifb_assign_dispbuf(pstPar->stBaseInfo.u32LayerID);
	}

	return HI_SUCCESS;
}
#endif

static HI_S32 hifb_tde_callback(HI_VOID *pParaml, HI_VOID *pParamr)
{
    HI_U32 u32LayerId = *(HI_U32 *)pParaml;
    HI_S32 s32TdeFinishHandle = *(HI_S32 *)pParamr;
    HIFB_PAR_S *pstPar = (HIFB_PAR_S *)(s_stLayer[u32LayerId].pstInfo->par);

    if (pstPar->bSetStereoMode)
    {
        HI_U32 u32Index = pstPar->stRunInfo.u32IndexForInt;

		s_stDrvOps.HIFB_DRV_SetLayerAddr(u32LayerId, pstPar->st3DInfo.u32DisplayAddr[u32Index]);
		pstPar->stRunInfo.u32ScreenAddr  = pstPar->st3DInfo.u32DisplayAddr[u32Index];
		pstPar->st3DInfo.u32rightEyeAddr = pstPar->stRunInfo.u32ScreenAddr;
		s_stDrvOps.HIFB_DRV_SetTriDimAddr(u32LayerId, pstPar->st3DInfo.u32rightEyeAddr);

		s_stDrvOps.HIFB_DRV_UpdataLayerReg(pstPar->stBaseInfo.u32LayerID);

	    pstPar->stRunInfo.u32IndexForInt = (++u32Index) % pstPar->stRunInfo.u32BufNum;
    }
    return HI_SUCCESS;
}

static HI_VOID hifb_disp_setlayerpos(HI_U32 u32LayerId, HI_S32 s32XPos, HI_S32 s32YPos)
{
    struct fb_info *info = s_stLayer[u32LayerId].pstInfo;
    HIFB_PAR_S *pstPar = (HIFB_PAR_S *)info->par;

	pstPar->stRunInfo.bModifying          = HI_TRUE;

	pstPar->stExtendInfo.stPos.s32XPos = s32XPos;
    pstPar->stExtendInfo.stPos.s32YPos = s32YPos;

    pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_INRECT;
	pstPar->stRunInfo.bModifying          = HI_FALSE;
    return;
}

static HI_VOID hifb_buf_setbufmode(HI_U32 u32LayerId, HIFB_LAYER_BUF_E enLayerBufMode)
{
    struct fb_info *info = s_stLayer[u32LayerId].pstInfo;
    HIFB_PAR_S *pstPar = (HIFB_PAR_S *)info->par;

    /* in 0 buf mode ,maybe the stride or fmt will be changed! */
    if ((pstPar->stExtendInfo.enBufMode == HIFB_LAYER_BUF_NONE)
        && (pstPar->stExtendInfo.enBufMode != enLayerBufMode))
    {
        pstPar->stRunInfo.bModifying = HI_TRUE;
        
        pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_STRIDE;

        pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_FMT;

        pstPar->stRunInfo.bModifying = HI_FALSE;
    }

    pstPar->stExtendInfo.enBufMode = enLayerBufMode;

}


/*choose the module to do  flicker resiting, TDE or VOU ? the rule is as this ,the moudle should do flicker resisting who has do scaling*/
static HI_VOID hifb_select_antiflicker_mode(HIFB_PAR_S *pstPar)
{
	HIFB_RECT stOutputRect;
    /*if the usr's configuration is no needed to do flicker resisting, so no needed to do it  */
   if (pstPar->stBaseInfo.enAntiflickerLevel == HIFB_LAYER_ANTIFLICKER_NONE)
   {
       pstPar->stBaseInfo.enAntiflickerMode = HIFB_ANTIFLICKER_NONE;
   }
   else
   {
       /*current standard no needed to do flicker resisting */
       if (!pstPar->stBaseInfo.bNeedAntiflicker)
       {
           pstPar->stBaseInfo.enAntiflickerMode = HIFB_ANTIFLICKER_NONE;
       }
       else
       {
       		s_stDrvOps.HIFB_DRV_GetLayerOutRect(pstPar->stBaseInfo.u32LayerID, &stOutputRect);
           /*VO has don scaling , so should do flicker resisting at the same time*/
           if ((pstPar->stExtendInfo.u32DisplayWidth != stOutputRect.w)
             || (pstPar->stExtendInfo.u32DisplayHeight != stOutputRect.h))
           {                
               pstPar->stBaseInfo.enAntiflickerMode = HIFB_ANTIFLICKER_VO; 
           }
           else
           {
               pstPar->stBaseInfo.enAntiflickerMode = HIFB_ANTIFLICKER_TDE; 
           }
       }
   }
    
}
   

static HI_VOID hifb_disp_setantiflickerlevel(HI_U32 u32LayerId, HIFB_LAYER_ANTIFLICKER_LEVEL_E enAntiflickerLevel)
{
    struct fb_info *info = s_stLayer[u32LayerId].pstInfo;
    HIFB_PAR_S *pstPar = (HIFB_PAR_S *)info->par;

    pstPar->stBaseInfo.enAntiflickerLevel = enAntiflickerLevel;
    hifb_select_antiflicker_mode(pstPar);

    return;
}

#define HIFB_CHECK_LAYERID(u32LayerId) do\
{\
    if (!g_pstCap[u32LayerId].bLayerSupported)\
    {\
        HIFB_ERROR("not support layer %d\n", u32LayerId);\
        return HI_FAILURE;\
    }\
}while(0);

#ifdef CFG_HIFB_CURSOR_SUPPORT
#define HIFB_CHECK_CURSOR_LAYERID(u32LayerId) do\
{\
 if (u32LayerId != HIFB_LAYER_CURSOR)\
    {\
        HIFB_ERROR("layer %d is not cursor layer!\n", u32LayerId);\
    	return HI_FAILURE;\
    }\
}while(0)
#endif

static HI_S32 hifb_flip_screenaddr(HI_U32 u32LayerId)
{
	HI_U32 u32Index;
    struct fb_info *info;
    HIFB_PAR_S *pstPar;    

	info   = s_stLayer[u32LayerId].pstInfo;
	pstPar = (HIFB_PAR_S *)(info->par);

    u32Index = pstPar->stRunInfo.u32IndexForInt;

	if (//(IS_STEREO_SBS(pstPar) || IS_STEREO_TAB(pstPar)))
		pstPar->bSetStereoMode)
	{
		s_stDrvOps.HIFB_DRV_SetLayerAddr(u32LayerId, pstPar->st3DInfo.u32DisplayAddr[u32Index]);
		pstPar->stRunInfo.u32ScreenAddr  = pstPar->st3DInfo.u32DisplayAddr[u32Index];
		pstPar->st3DInfo.u32rightEyeAddr = pstPar->stRunInfo.u32ScreenAddr;
		s_stDrvOps.HIFB_DRV_SetTriDimAddr(u32LayerId, pstPar->st3DInfo.u32rightEyeAddr);
	}
	else
	{
		s_stDrvOps.HIFB_DRV_SetLayerAddr(u32LayerId, pstPar->stDispInfo.u32DisplayAddr[u32Index]);
		pstPar->stRunInfo.u32ScreenAddr  = pstPar->stDispInfo.u32DisplayAddr[u32Index];
	}
	
    pstPar->stRunInfo.u32IndexForInt = (++u32Index) % pstPar->stRunInfo.u32BufNum;
    pstPar->stRunInfo.bFliped   = HI_TRUE;
    pstPar->stRunInfo.bNeedFlip = HI_FALSE;

	return HI_SUCCESS;
}
static HI_S32 hifb_vo_callback(HI_VOID *pParaml, HI_VOID *pParamr)
{
    HI_U32 *pu32LayerId = (HI_U32 *)pParaml;
    struct fb_info *info = s_stLayer[*pu32LayerId].pstInfo;
    HIFB_PAR_S *pstPar = (HIFB_PAR_S *)(info->par);

    if (!pstPar->stRunInfo.bModifying)
    {
        if (pstPar->stRunInfo.u32ParamModifyMask & HIFB_LAYER_PARAMODIFY_SHOW)
        {
            s_stDrvOps.HIFB_DRV_EnableLayer(*pu32LayerId, pstPar->stExtendInfo.bShow);
            pstPar->stRunInfo.u32ParamModifyMask &= ~HIFB_LAYER_PARAMODIFY_SHOW;       
        }
        
        if (pstPar->stRunInfo.u32ParamModifyMask & HIFB_LAYER_PARAMODIFY_ALPHA)
        {
            s_stDrvOps.HIFB_DRV_SetLayerAlpha(*pu32LayerId, &pstPar->stExtendInfo.stAlpha);
			pstPar->stRunInfo.u32ParamModifyMask &= ~HIFB_LAYER_PARAMODIFY_ALPHA;
        }

        if (pstPar->stRunInfo.u32ParamModifyMask & HIFB_LAYER_PARAMODIFY_COLORKEY)
        {
            s_stDrvOps.HIFB_DRV_SetLayerKeyMask(*pu32LayerId, &pstPar->stExtendInfo.stCkey);
			pstPar->stRunInfo.u32ParamModifyMask &= ~HIFB_LAYER_PARAMODIFY_COLORKEY;
        }

        if (pstPar->stRunInfo.u32ParamModifyMask & HIFB_LAYER_PARAMODIFY_BMUL)
        {
            s_stDrvOps.HIFB_DRV_SetLayerPreMult(*pu32LayerId, pstPar->stBaseInfo.bPreMul);
			pstPar->stRunInfo.u32ParamModifyMask &= ~HIFB_LAYER_PARAMODIFY_BMUL;
        }

        if (pstPar->stRunInfo.u32ParamModifyMask & HIFB_LAYER_PARAMODIFY_ANTIFLICKERLEVEL)
        {
            HIFB_DEFLICKER_S stDeflicker;

            stDeflicker.pu8HDfCoef = pstPar->stBaseInfo.ucHDfcoef;
            stDeflicker.pu8VDfCoef = pstPar->stBaseInfo.ucVDfcoef;
            stDeflicker.u32HDfLevel = pstPar->stBaseInfo.u32HDflevel;
            stDeflicker.u32VDfLevel = pstPar->stBaseInfo.u32VDflevel;

            s_stDrvOps.HIFB_DRV_SetLayerDeFlicker(*pu32LayerId, &stDeflicker);

			pstPar->stRunInfo.u32ParamModifyMask &= ~HIFB_LAYER_PARAMODIFY_ANTIFLICKERLEVEL;
        }
		
        if (pstPar->stRunInfo.u32ParamModifyMask & HIFB_LAYER_PARAMODIFY_INRECT
			|| pstPar->stRunInfo.u32ParamModifyMask & HIFB_LAYER_PARAMODIFY_OUTRECT)
        {
            HIFB_RECT stInRect   = {0};

        	/*******the position of layer in the graphics processor*****/
        	stInRect.x = pstPar->stExtendInfo.stPos.s32XPos;
            stInRect.y = pstPar->stExtendInfo.stPos.s32YPos;
			
            stInRect.w = (HI_S32)pstPar->stExtendInfo.u32DisplayWidth;
            stInRect.h = (HI_S32)pstPar->stExtendInfo.u32DisplayHeight;                												 				

			/************set the input rect of layer*************/
            s_stDrvOps.HIFB_DRV_SetLayerInRect(*pu32LayerId, &stInRect);
			pstPar->stRunInfo.u32ParamModifyMask &= ~HIFB_LAYER_PARAMODIFY_INRECT;
			pstPar->stRunInfo.u32ParamModifyMask &= ~HIFB_LAYER_PARAMODIFY_OUTRECT;
		}

		/***color format,stride,display address take effect only when user refreshing**/
		if (pstPar->stRunInfo.u32ParamModifyMask & HIFB_LAYER_PARAMODIFY_REFRESH)
		{
#if 0		
#ifdef CFG_HIFB_LOGO_SUPPORT		
			if (g_bLogoShow)
            {   
                HIFB_INFO("<<<<<<has logo,show app>>>>>>>>\n");
                hifb_clear_logo();
            }
#endif	
#endif						
			if (pstPar->stRunInfo.u32ParamModifyMask & HIFB_LAYER_PARAMODIFY_FMT)
	        {
	            if ((pstPar->stExtendInfo.enBufMode == HIFB_LAYER_BUF_NONE)
	                && pstPar->stDispInfo.stUserBuffer.stCanvas.u32PhyAddr)
	            {
	                s_stDrvOps.HIFB_DRV_SetLayerDataFmt(*pu32LayerId, pstPar->stDispInfo.stUserBuffer.stCanvas.enFmt);
	            }
	            else
	            {
	                s_stDrvOps.HIFB_DRV_SetLayerDataFmt(*pu32LayerId, pstPar->stExtendInfo.enColFmt);
	            }

				pstPar->stRunInfo.u32ParamModifyMask &= ~HIFB_LAYER_PARAMODIFY_FMT;
	        }

	        if (pstPar->stRunInfo.u32ParamModifyMask & HIFB_LAYER_PARAMODIFY_STRIDE)
	        {
#ifdef CFG_HIFB_3D_ENABLE	        
	        	if (//(IS_STEREO_SBS(pstPar) || IS_STEREO_TAB(pstPar)))
	        		pstPar->bSetStereoMode)
	        	{	        					
	                s_stDrvOps.HIFB_DRV_SetLayerStride(*pu32LayerId, pstPar->st3DInfo.st3DSurface.u32Pitch);			
	        	}
				else
#endif					
				{
					if ((pstPar->stExtendInfo.enBufMode == HIFB_LAYER_BUF_NONE)
	                	&& pstPar->stDispInfo.stUserBuffer.stCanvas.u32PhyAddr)
		            {
		                s_stDrvOps.HIFB_DRV_SetLayerStride(*pu32LayerId, pstPar->stDispInfo.stUserBuffer.stCanvas.u32Pitch);
		            }				
		            else				
		            {

						s_stDrvOps.HIFB_DRV_SetLayerStride(*pu32LayerId, info->fix.line_length);	                
		            }
				}	            

				pstPar->stRunInfo.u32ParamModifyMask &= ~HIFB_LAYER_PARAMODIFY_STRIDE;

	        }
			
	        if (pstPar->stRunInfo.u32ParamModifyMask & HIFB_LAYER_PARAMODIFY_DISPLAYADDR)
	        {
	            s_stDrvOps.HIFB_DRV_SetLayerAddr(*pu32LayerId, pstPar->stRunInfo.u32ScreenAddr);
				pstPar->stRunInfo.u32ParamModifyMask &= ~HIFB_LAYER_PARAMODIFY_DISPLAYADDR;

				if (pstPar->bSetStereoMode)
				{
					pstPar->st3DInfo.u32rightEyeAddr = pstPar->stRunInfo.u32ScreenAddr;
					s_stDrvOps.HIFB_DRV_SetTriDimAddr(*pu32LayerId, pstPar->st3DInfo.u32rightEyeAddr);
				}
	        }

			pstPar->stRunInfo.u32ParamModifyMask &= ~HIFB_LAYER_PARAMODIFY_REFRESH;
		}		
    }
    
    if ((pstPar->stExtendInfo.enBufMode == HIFB_LAYER_BUF_DOUBLE)
        && (pstPar->stRunInfo.bNeedFlip == HI_TRUE))
    {
		hifb_flip_screenaddr(*pu32LayerId);
    } 

#ifdef CFG_HIFB_3D_ENABLE
    if ((pstPar->stExtendInfo.enBufMode == HIFB_LAYER_BUF_STANDARD)
        	&& pstPar->st3DInfo.st3DMemInfo.u32StereoMemStart
        	&& pstPar->bSetStereoMode//(IS_STEREO_SBS(pstPar) || IS_STEREO_TAB(pstPar))
        	&& (pstPar->stRunInfo.bNeedFlip == HI_TRUE))
    {
		hifb_flip_screenaddr(*pu32LayerId);
    }
#endif

    s_stDrvOps.HIFB_DRV_UpdataLayerReg(*pu32LayerId);

#ifdef CFG_HIFB_SCROLLTEXT_SUPPORT
	hifb_scrolltext_blit(*pu32LayerId);
#endif

    return HI_SUCCESS;
}

static HI_S32 hifb_frame_end_callback(HI_VOID *pParaml, HI_VOID *pParamr)
{
#warning TODO: hifb_frame_end_callback
	printk("hifb_frame_end_callback: TODO\n");
}


/* 0buf refresh */
static HI_S32 hifb_refresh_0buf(HI_U32 u32LayerId, HIFB_BUFFER_S *pstCanvasBuf)
{
	HI_U32 u32StartAddr;
	HIFB_PAR_S *pstPar; 
    struct fb_info *info;    

	info   = s_stLayer[u32LayerId].pstInfo;
	pstPar = (HIFB_PAR_S *)info->par;	
    u32StartAddr = pstCanvasBuf->stCanvas.u32PhyAddr;

    pstPar->stRunInfo.bModifying = HI_TRUE;

	pstPar->stRunInfo.u32ScreenAddr = u32StartAddr;
    pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_DISPLAYADDR;        
    
	pstPar->stDispInfo.stUserBuffer.stCanvas.u32Pitch = pstCanvasBuf->stCanvas.u32Pitch;
    pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_STRIDE;    

	pstPar->stDispInfo.stUserBuffer.stCanvas.enFmt = pstCanvasBuf->stCanvas.enFmt;
    pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_FMT;    

    hifb_disp_setdispsize(u32LayerId, pstCanvasBuf->stCanvas.u32Width, 
                              pstCanvasBuf->stCanvas.u32Height);
	memcpy(&(pstPar->stDispInfo.stUserBuffer), pstCanvasBuf, sizeof(HIFB_BUFFER_S));
	
	pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_REFRESH;
    pstPar->stRunInfo.bModifying = HI_FALSE;    

    hifb_wait_regconfig_work(u32LayerId);
    
    return HI_SUCCESS;
}

#ifdef CFG_HIFB_3D_ENABLE
static HI_S32 hifb_refresh_0buf_3D(HI_U32 u32LayerId, HIFB_BUFFER_S *pstCanvasBuf)
{
	HI_S32 s32Ret;
	HIFB_PAR_S *pstPar;
	HI_U32 u32BufferSize;	 
    struct fb_info *info; 
	HIFB_BLIT_OPT_S stBlitOpt;

	info   = s_stLayer[u32LayerId].pstInfo;
	pstPar = (HIFB_PAR_S *)info->par;	

	/*config 3D surface par*/
	pstPar->st3DInfo.st3DSurface.enFmt     = pstCanvasBuf->stCanvas.enFmt;
	pstPar->st3DInfo.st3DSurface.u32Pitch  = pstCanvasBuf->stCanvas.u32Pitch;
	pstPar->st3DInfo.st3DSurface.u32Width  = pstCanvasBuf->stCanvas.u32Width;
	pstPar->st3DInfo.st3DSurface.u32Height = pstCanvasBuf->stCanvas .u32Height;

	/*allocate 3D memory*/
	u32BufferSize = pstCanvasBuf->stCanvas.u32Height * ((pstCanvasBuf->stCanvas.u32Pitch + 0xf) & 0xfffffff0);

	s32Ret = hifb_checkandalloc_3dmem(u32LayerId, u32BufferSize);
	if (HI_SUCCESS != s32Ret)
	{
		HIFB_INFO("fail to alloc 3d memory, refresh failure.\n ");
		return s32Ret;
	}

	/*config 3D surface par*/
	pstPar->st3DInfo.st3DSurface.u32PhyAddr= pstPar->st3DInfo.u32DisplayAddr[0];
	
	/*config 3D buffer*/
	memset(&stBlitOpt, 0, sizeof(HIFB_BLIT_OPT_S));
	stBlitOpt.bRegionDeflicker = HI_TRUE;
	stBlitOpt.bScale           = HI_TRUE;
	if (pstPar->stBaseInfo.enAntiflickerMode == HIFB_ANTIFLICKER_TDE)
	{
		stBlitOpt.enAntiflickerLevel = pstPar->stBaseInfo.enAntiflickerLevel;
	}
	
	hifb_3DData_Config(u32LayerId, pstCanvasBuf, &stBlitOpt);
	
    pstPar->stRunInfo.bModifying = HI_TRUE;
	
	pstPar->stRunInfo.u32ScreenAddr       = pstPar->st3DInfo.st3DMemInfo.u32StereoMemStart;
    pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_DISPLAYADDR;
	
	pstPar->stDispInfo.stUserBuffer.stCanvas.u32Pitch = pstCanvasBuf->stCanvas.u32Pitch;
    pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_STRIDE;
	
	pstPar->stDispInfo.stUserBuffer.stCanvas.enFmt = pstCanvasBuf->stCanvas.enFmt;
    pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_FMT;    

    hifb_disp_setdispsize(u32LayerId, pstCanvasBuf->stCanvas.u32Width, 
                              pstCanvasBuf->stCanvas.u32Height);

	pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_REFRESH;
	
    pstPar->stRunInfo.bModifying = HI_FALSE;

    memcpy(&(pstPar->stDispInfo.stUserBuffer), pstCanvasBuf, sizeof(HIFB_BUFFER_S));

    hifb_wait_regconfig_work(u32LayerId);
    
    return HI_SUCCESS;
}
#endif

#ifdef CFG_HIFB_3D_ENABLE
static HI_S32 hifb_refresh_panbuf(HI_U32 u32LayerId, HIFB_BUFFER_S *pstCanvasBuf)
{
    HI_S32 s32Ret;
	HI_U32 u32Stride;
	HI_U32 u32TmpAddr;    
	HIFB_RECT UpdateRect;
	HIFB_BLIT_OPT_S stBlitOpt;
    HIFB_BUFFER_S stCanvasBuf;
    HIFB_BUFFER_S stDisplayBuf;   

	HIFB_PAR_S *par;
    struct fb_info *info;    
    struct fb_var_screeninfo *var;
	
	info = s_stLayer[u32LayerId].pstInfo;
	par = (HIFB_PAR_S *)info->par;
	var = &(info->var);

	UpdateRect = pstCanvasBuf->UpdateRect;
        
    if ((UpdateRect.x >=  par->stExtendInfo.u32DisplayWidth)
        || (UpdateRect.y >= par->stExtendInfo.u32DisplayHeight)
        || (UpdateRect.w == 0) || (UpdateRect.h == 0))
    {
        HIFB_ERROR("hifb_refresh_panbuf upate rect invalid\n");
        return HI_FAILURE;
    }
	
    if (par->bSetStereoMode)//(IS_STEREO_SBS(par) || IS_STEREO_TAB(par))
    {
        if (HI_NULL == par->st3DInfo.st3DMemInfo.u32StereoMemStart)
        {
            HIFB_ERROR("you should pan first\n");
            return HI_FAILURE;
        }

        u32Stride = par->st3DInfo.st3DSurface.u32Pitch;
                
        memset(&stBlitOpt, 0, sizeof(HIFB_BLIT_OPT_S));
        stBlitOpt.bScale = HI_TRUE;
		
        if (HIFB_ANTIFLICKER_TDE == par->stBaseInfo.enAntiflickerMode)
        {
            stBlitOpt.enAntiflickerLevel = par->stBaseInfo.enAntiflickerLevel;
        }
		
        stBlitOpt.bBlock = HI_TRUE;
        stBlitOpt.bRegionDeflicker = HI_TRUE;

        if (var->bits_per_pixel >= 8)
        {
            u32TmpAddr = info->fix.smem_start + info->fix.line_length * var->yoffset
                           + var->xoffset* (var->bits_per_pixel >> 3);
        }
        else
        {
            u32TmpAddr = (info->fix.smem_start + info->fix.line_length * var->yoffset
                           + var->xoffset * var->bits_per_pixel / 8);
        }

		if((var->bits_per_pixel == 24)&&((var->xoffset !=0)||(var->yoffset !=0)))
	    {
	        HI_U32 TmpData;

	        TmpData = (info->fix.smem_start + info->fix.line_length * var->yoffset
	                       + var->xoffset * (var->bits_per_pixel >> 3))/16/3;
	        u32TmpAddr = TmpData*16*3;
	    }

		/********************config pan buffer*******************/
        memset(&stCanvasBuf, 0, sizeof(HIFB_BUFFER_S));
        stCanvasBuf.stCanvas.enFmt      = par->stExtendInfo.enColFmt;
        stCanvasBuf.stCanvas.u32Pitch   = info->fix.line_length;
        stCanvasBuf.stCanvas.u32PhyAddr = u32TmpAddr;
		stCanvasBuf.stCanvas.u32Width   = par->stExtendInfo.u32DisplayWidth;
        stCanvasBuf.stCanvas.u32Height  = par->stExtendInfo.u32DisplayHeight;
        stCanvasBuf.UpdateRect          = UpdateRect;
		/***********************end**************************/

		/*******************config 3D buffer********************/
        memset(&stDisplayBuf, 0, sizeof(HIFB_BUFFER_S));
        stDisplayBuf.stCanvas.enFmt      = par->st3DInfo.st3DSurface.enFmt;           
        stDisplayBuf.stCanvas.u32Pitch   = par->st3DInfo.st3DSurface.u32Pitch;
        stDisplayBuf.stCanvas.u32PhyAddr = par->stRunInfo.u32ScreenAddr;
        stDisplayBuf.stCanvas.u32Width   = par->st3DInfo.st3DSurface.u32Width; 
        stDisplayBuf.stCanvas.u32Height  = par->st3DInfo.st3DSurface.u32Height;
		/***********************end**************************/   
        
        if (HIFB_STEREO_SIDEBYSIDE_HALF == par->st3DInfo.enOutStereoMode)
        {
            stDisplayBuf.stCanvas.u32Width >>= 1;  
        }
        else if (HIFB_STEREO_TOPANDBOTTOM == par->st3DInfo.enOutStereoMode)
        {
            stDisplayBuf.stCanvas.u32Height >>= 1;  
        }
		
		stDisplayBuf.UpdateRect.x = 0;	   
		stDisplayBuf.UpdateRect.y = 0;			   
		stDisplayBuf.UpdateRect.w = stDisplayBuf.stCanvas.u32Width;
		stDisplayBuf.UpdateRect.h = stDisplayBuf.stCanvas.u32Height; 


        s32Ret = s_stDrvTdeOps.HIFB_DRV_Blit(&stCanvasBuf, &stDisplayBuf, &stBlitOpt, HI_TRUE);
        if (s32Ret < 0)
        {
            HIFB_ERROR("stereo blit error!\n");
            return HI_FAILURE;
        } 
		
    }

    return HI_SUCCESS;
}
#endif  



static HI_S32 hifb_refresh(HI_U32 u32LayerId, HIFB_BUFFER_S *pstCanvasBuf, HIFB_LAYER_BUF_E enBufMode)
{
    HI_S32 s32Ret;
	HIFB_PAR_S *par;
	struct fb_info *info;

	s32Ret = HI_FAILURE;
	info   = s_stLayer[u32LayerId].pstInfo;
	par    = (HIFB_PAR_S *)(info->par);
	
#ifdef CFG_HIFB_3D_ENABLE
	if (par->bSetStereoMode)//(IS_STEREO_SBS(par) || IS_STEREO_TAB(par)))
	{
		switch (enBufMode)
	    {    
	        case HIFB_LAYER_BUF_DOUBLE:
	            s32Ret = hifb_refresh_2buf_3D(u32LayerId, pstCanvasBuf);
	            break;
					
	        case HIFB_LAYER_BUF_ONE:
	            s32Ret = hifb_refresh_1buf_3D(u32LayerId, pstCanvasBuf);
	            break;

	        case HIFB_LAYER_BUF_NONE:
	           s32Ret = hifb_refresh_0buf_3D(u32LayerId, pstCanvasBuf);
	           break;
	           
	        case HIFB_LAYER_BUF_DOUBLE_IMMEDIATE:
	            s32Ret = hifb_refresh_2buf_immediate_display_3D(u32LayerId, pstCanvasBuf);
	            break;

	        default:
	            break;
	    }
	}
	else
#endif		
	{
		switch (enBufMode)
	    {    
	        case HIFB_LAYER_BUF_DOUBLE:
	            s32Ret = hifb_refresh_2buf(u32LayerId, pstCanvasBuf);
	            break;
					
	        case HIFB_LAYER_BUF_ONE:
	            s32Ret = hifb_refresh_1buf(u32LayerId, pstCanvasBuf);
	            break;

	        case HIFB_LAYER_BUF_NONE:
	           s32Ret = hifb_refresh_0buf(u32LayerId, pstCanvasBuf);
	           break;
	           
	        case HIFB_LAYER_BUF_DOUBLE_IMMEDIATE:
	            s32Ret = hifb_refresh_2buf_immediate_display(u32LayerId, pstCanvasBuf);
	            break;

	        default:
	            break;
	    }
	}

#ifdef CFG_HIFB_LOGO_SUPPORT		
	if (g_bLogoShow)
	{	
		HIFB_INFO("<<<<<<has logo,show app>>>>>>>>\n");
		hifb_wait_regconfig_work(u32LayerId);
		hifb_clear_logo(0);
	}
#endif	

	par->stRunInfo.bModifying = HI_TRUE;
	par->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_REFRESH;
	par->stRunInfo.bModifying = HI_FALSE;
	
    return s32Ret;
}

static HI_S32 hifb_alloccanbuf(struct fb_info *info, HIFB_LAYER_INFO_S * pLayerInfo)
{
    HIFB_PAR_S *par = (HIFB_PAR_S *)info->par;
    
    if (!(pLayerInfo->u32Mask & HIFB_LAYERMASK_CANVASSIZE))
    {
        //printk("============0x%x\n", pLayerInfo->u32Mask);
        return HI_SUCCESS;
    }
    
    /** if  with old canvas buffer*/
    if (par->stDispInfo.stCanvasSur.u32PhyAddr)
    {
        /* if  old is the sampe with new , then return, else free the old buffer*/
        if ((pLayerInfo->u32CanvasWidth == par->stDispInfo.stCanvasSur.u32Width) &&
             (pLayerInfo->u32CanvasHeight == par->stDispInfo.stCanvasSur.u32Height))
        {
            HIFB_INFO("mem is the sampe , no need alloc new memory");
            return HI_SUCCESS;
        }

        /** free new old buffer*/
        HIFB_INFO("free old canvas buffer\n");        
        hifb_freeccanbuf(par);
    }
    
    /** new canvas buffer*/
    if ((pLayerInfo->u32CanvasWidth >=  0) && (pLayerInfo->u32CanvasHeight >=  0))
    {
        HI_U32 u32LayerSize;
        HI_U32 u32Pitch;
        HI_CHAR *pBuf;

        /*Modify 16 to 32, preventing out of bound.*/
        HI_CHAR name[32];

        /*16 bytes aligmn*/
        u32Pitch = ((pLayerInfo->u32CanvasWidth * info->var.bits_per_pixel >> 3) + 15)>>4;
        u32Pitch = u32Pitch << 4;
        
        u32LayerSize = u32Pitch * pLayerInfo->u32CanvasHeight;
        /** alloc new buffer*/
		snprintf(name, sizeof(name), "HIFB_Canvas%d", par->stBaseInfo.u32LayerID);
        par->stDispInfo.stCanvasSur.u32PhyAddr = hifb_buf_allocmem(name, u32LayerSize);
        //HIFB_ERROR("canvas surface addr:0x%x\n", par->CanvasSur.u32PhyAddr);
        if (par->stDispInfo.stCanvasSur.u32PhyAddr == 0)
        {   
            HIFB_ERROR("alloc canvas buffer no mem, expect size: 0x%x, cavh:%d\n", u32LayerSize, pLayerInfo->u32CanvasHeight);
            return HI_FAILURE;
        }

        pBuf = (HI_CHAR *)hifb_buf_map(par->stDispInfo.stCanvasSur.u32PhyAddr);
        if (pBuf == HI_NULL)
        {
            HIFB_ERROR("map canvas buffer failed!\n");
            hifb_buf_freemem(par->stDispInfo.stCanvasSur.u32PhyAddr);
            return HI_FAILURE;
        }
        memset(pBuf, 0, u32LayerSize);
        hifb_buf_ummap(pBuf);

        HIFB_INFO("alloc new memory for canvas buffer success\n"); 
        par->stDispInfo.stCanvasSur.u32Width  = pLayerInfo->u32CanvasWidth;
        par->stDispInfo.stCanvasSur.u32Height = pLayerInfo->u32CanvasHeight;
        par->stDispInfo.stCanvasSur.enFmt     =  hifb_getfmtbyargb(&info->var.red, &info->var.green, &info->var.blue, &info->var.transp, info->var.bits_per_pixel);
        par->stDispInfo.stCanvasSur.u32Pitch  = u32Pitch;

        return HI_SUCCESS;
    }
	return HI_SUCCESS;
}



static HI_S32 hifb_onrefresh(HIFB_PAR_S* par, HI_VOID __user *argp)
{
    HI_S32 s32Ret;
    HIFB_BUFFER_S stCanvasBuf;
    
    if (par->stBaseInfo.u32LayerID == HIFB_LAYER_CURSOR)
    {
        HIFB_WARNING("you shouldn't refresh cursor layer!");
        return HI_SUCCESS;
    }
    
    if (copy_from_user(&stCanvasBuf, argp, sizeof(HIFB_BUFFER_S)))
    {
        return -EFAULT;
    }
    
    /*when user data  update in 3d mode , 
    	    blit pan buffer to 3D buffer to config 3d data*/  
#ifdef CFG_HIFB_3D_ENABLE
    if ((par->stExtendInfo.enBufMode == HIFB_LAYER_BUF_STANDARD) 
            &&  (((par->st3DInfo.st3DMemInfo.u32StereoMemStart != 0) && par->bSetStereoMode/*(IS_STEREO_SBS(par) || IS_STEREO_TAB(par))*/) ))
    {
        return hifb_refresh_panbuf(par->stBaseInfo.u32LayerID, &stCanvasBuf);
    }

	/**when user refresh in pan display , just return**/
    if (par->stExtendInfo.enBufMode == HIFB_LAYER_BUF_STANDARD)
    {
        return HI_FAILURE;
    }
#endif

	if (0 == stCanvasBuf.stCanvas.u32Width
		|| 0 == stCanvasBuf.stCanvas.u32Height)
	{
		HIFB_ERROR("canvas buffer's width or height can't be zero.\n");
		return HI_FAILURE;
	}

	if (stCanvasBuf.stCanvas.enFmt >= HIFB_FMT_BUTT)
	{
		HIFB_ERROR("color format of canvas buffer unsupported.\n");
		return HI_FAILURE;
	}
    
    if ((stCanvasBuf.UpdateRect.x >=  stCanvasBuf.stCanvas.u32Width)
        || (stCanvasBuf.UpdateRect.y >= stCanvasBuf.stCanvas.u32Height)
        || (stCanvasBuf.UpdateRect.w == 0) || (stCanvasBuf.UpdateRect.h == 0))
    {
        HIFB_ERROR("rect error: update rect:(%d,%d,%d,%d), canvas range:(%d,%d)\n", 
                  stCanvasBuf.UpdateRect.x, stCanvasBuf.UpdateRect.y,
                  stCanvasBuf.UpdateRect.w, stCanvasBuf.UpdateRect.h,
                  stCanvasBuf.stCanvas.u32Width, stCanvasBuf.stCanvas.u32Height);
        return HI_FAILURE;
    }
    
    if (stCanvasBuf.UpdateRect.x + stCanvasBuf.UpdateRect.w > stCanvasBuf.stCanvas.u32Width)
    {
        stCanvasBuf.UpdateRect.w = stCanvasBuf.stCanvas.u32Width - stCanvasBuf.UpdateRect.x;
    }
    
    if (stCanvasBuf.UpdateRect.y + stCanvasBuf.UpdateRect.h > stCanvasBuf.stCanvas.u32Height)
    {
        stCanvasBuf.UpdateRect.h =  stCanvasBuf.stCanvas.u32Height - stCanvasBuf.UpdateRect.y;
    }
    
    if (par->stExtendInfo.enBufMode == HIFB_LAYER_BUF_NONE)
    {
        /* there's a limit from hardware that the start address of screen buf 
              should be 16byte aligned! */
        if ((stCanvasBuf.stCanvas.u32PhyAddr & 0xf) || (stCanvasBuf.stCanvas.u32Pitch & 0xf))
        {
            HIFB_ERROR("addr 0x%x or pitch: 0x%x is not 16 bytes align !\n", 
                stCanvasBuf.stCanvas.u32PhyAddr,
                stCanvasBuf.stCanvas.u32Pitch);
            return HI_FAILURE;
        }
    }

    s32Ret = hifb_refresh(par->stBaseInfo.u32LayerID, &stCanvasBuf, par->stExtendInfo.enBufMode);
    
    return s32Ret;
}

static HI_S32 hifb_onputlayerinfo(struct fb_info *info, HIFB_PAR_S* par, HI_VOID __user *argp)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HIFB_LAYER_INFO_S stLayerInfo;
    HI_U32 u32Pitch;
    if (par->stBaseInfo.u32LayerID == HIFB_LAYER_CURSOR)
    {
       HIFB_WARNING("you shouldn't put cursor layer info!");
       return HI_SUCCESS;
    }
    
    if (copy_from_user(&stLayerInfo, argp, sizeof(HIFB_LAYER_INFO_S)))
    {
       return -EFAULT;
    }
    
    s32Ret = hifb_alloccanbuf(info, &stLayerInfo);
    if (s32Ret != HI_SUCCESS)
    {
       HIFB_ERROR("alloc canvas buffer failed\n");
       return HI_FAILURE;
    }
    
    
    if (stLayerInfo.u32Mask & HIFB_LAYERMASK_DISPSIZE)
    {
        u32Pitch = stLayerInfo.u32DisplayWidth* info->var.bits_per_pixel >> 3;
        u32Pitch = (u32Pitch + 0xf) & 0xfffffff0;

		if (stLayerInfo.u32DisplayWidth == 0 || stLayerInfo.u32DisplayHeight == 0)
        {
            HIFB_ERROR("display witdh/height shouldn't be 0!\n");
            return HI_FAILURE;
        } 
			   
        if(HI_FAILURE == hifb_checkmem_enough(info, u32Pitch, stLayerInfo.u32DisplayHeight))
        {
            return HI_FAILURE;
        }
    }        
    
    if (stLayerInfo.u32Mask & HIFB_LAYERMASK_SCREENSIZE)
    {
       if ((stLayerInfo.u32ScreenWidth == 0) || (stLayerInfo.u32ScreenHeight == 0))
       {
           HIFB_ERROR("screen width/height shouldn't be 0\n");
           return HI_FAILURE;
       }
    }

#ifdef CFG_HIFB_3D_ENABLE
     if ( ((stLayerInfo.u32Mask & HIFB_LAYERMASK_DISPSIZE) 
	 		&& par->bSetStereoMode))
     {
        hifb_clearallstereobuf(info);
     }
#endif	 
    
	if (stLayerInfo.u32Mask & HIFB_LAYERMASK_BUFMODE)
	{
        HI_U32 u32LayerSize;
     
		if (stLayerInfo.BufMode == HIFB_LAYER_BUF_ONE)
		{
		   u32LayerSize = info->fix.line_length * info->var.yres;
		}
		else if ((stLayerInfo.BufMode == HIFB_LAYER_BUF_DOUBLE) 
		    || (stLayerInfo.BufMode == HIFB_LAYER_BUF_DOUBLE_IMMEDIATE))
		{
		   u32LayerSize = 2 * info->fix.line_length * info->var.yres;
		}
		else
		{
		   u32LayerSize = 0;
		}

		if (u32LayerSize > info->fix.smem_len)
		{
		   HIFB_ERROR("No enough mem! layer real memory size:%d KBytes, expected:%d KBtyes\n",
		       info->fix.smem_len/1024, u32LayerSize/1024);
		   return HI_FAILURE;
		}
    }
    
    /*if x>width or y>height ,how to deal with: see nothing in screen or return failure?*/
    if ((stLayerInfo.u32Mask & HIFB_LAYERMASK_POS) 
       && ((stLayerInfo.s32XPos < 0) || (stLayerInfo.s32YPos < 0)))
    {
       HIFB_ERROR("Pos err!\n");
       return HI_FAILURE;
    }
    
    if ((stLayerInfo.u32Mask & HIFB_LAYERMASK_BMUL) && par->stExtendInfo.stCkey.bKeyEnable)
    {
       HIFB_ERROR("Colorkey and premul couldn't take effect at same time!\n");
       return HI_FAILURE;
    }
    
    /*avoid modifying register in vo isr before all params has benn recorded! In vo irq,
       flag bModifying will be checked.*/
    par->stRunInfo.bModifying = HI_TRUE;
    
    if (stLayerInfo.u32Mask & HIFB_LAYERMASK_BMUL)
    {
        par->stBaseInfo.bPreMul            = stLayerInfo.bPreMul;
        par->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_BMUL;
    }
    
    
    if (stLayerInfo.u32Mask & HIFB_LAYERMASK_BUFMODE)
    {
        hifb_buf_setbufmode(par->stBaseInfo.u32LayerID, stLayerInfo.BufMode);
    }

	if (stLayerInfo.u32Mask & HIFB_LAYERMASK_POS)
	{                
	    hifb_disp_setlayerpos(par->stBaseInfo.u32LayerID, stLayerInfo.s32XPos, stLayerInfo.s32YPos);
	}

	if (stLayerInfo.u32Mask & HIFB_LAYERMASK_ANTIFLICKER_MODE)
	{
	    hifb_disp_setantiflickerlevel(par->stBaseInfo.u32LayerID, stLayerInfo.eAntiflickerLevel);
	}

	if (stLayerInfo.u32Mask & HIFB_LAYERMASK_SCREENSIZE)
	{
	    s32Ret = hifb_disp_setscreensize(par->stBaseInfo.u32LayerID, stLayerInfo.u32ScreenWidth, stLayerInfo.u32ScreenHeight);
		if (HI_SUCCESS == s32Ret)
		{
			s_stDrvOps.HIFB_DRV_SetScreenFlag(par->stBaseInfo.u32LayerID, HI_TRUE);
		}
	}

	if (stLayerInfo.u32Mask & HIFB_LAYERMASK_DISPSIZE)
	{				
		if (stLayerInfo.u32DisplayWidth <= info->var.xres_virtual
			&& stLayerInfo.u32DisplayHeight <= info->var.yres_virtual)
		{
			s32Ret = hifb_disp_setdispsize(par->stBaseInfo.u32LayerID, stLayerInfo.u32DisplayWidth, stLayerInfo.u32DisplayHeight);
			if (s32Ret == HI_SUCCESS)
			{
			    info->var.xres = stLayerInfo.u32DisplayWidth;
				info->var.yres = stLayerInfo.u32DisplayHeight;
				hifb_assign_dispbuf(par->stBaseInfo.u32LayerID);
			}
			
			hifb_refreshall(info);	
		}	
	}
    
    par->stRunInfo.bModifying = HI_FALSE;
    return s32Ret;	
}

#ifdef CFG_HIFB_3D_ENABLE
static HI_S32 hifb_clearallstereobuf(struct fb_info *info)
{
#if 1
    HIFB_PAR_S *par = (HIFB_PAR_S *)info->par;

    if (par->stExtendInfo.enBufMode == HIFB_LAYER_BUF_STANDARD || par->stExtendInfo.enBufMode == HIFB_LAYER_BUF_NONE) 
    {
        hifb_clearstereobuf(info);
    }
    else
    {
        HIFB_BLIT_OPT_S stOpt;
        HIFB_SURFACE_S Surface;
        memset(&stOpt, 0x0, sizeof(stOpt));

        Surface.enFmt     = par->stExtendInfo.enColFmt;
        Surface.u32Height = par->stExtendInfo.u32DisplayHeight;
        Surface.u32Width  = par->stExtendInfo.u32DisplayWidth;
        Surface.u32Pitch  = ((((par->stExtendInfo.u32DisplayWidth * info->var.bits_per_pixel) >> 3) + 0xf) & 0xfffffff0);
        
        if (par->stExtendInfo.enBufMode == HIFB_LAYER_BUF_DOUBLE 
                ||par->stExtendInfo.enBufMode == HIFB_LAYER_BUF_DOUBLE_IMMEDIATE)
        {
            Surface.u32PhyAddr = par->st3DInfo.u32DisplayAddr[par->stRunInfo.u32IndexForInt];
        }
        else
        {
            Surface.u32PhyAddr = par->st3DInfo.u32DisplayAddr[0];
        }

		if (HI_NULL == Surface.u32PhyAddr)
		{
			HIFB_ERROR("fail to clear stereo rect.\n");
			return HI_FAILURE;
		}
        
        s_stDrvTdeOps.HIFB_DRV_ClearRect(&Surface, &stOpt);
    }
#endif
    return HI_SUCCESS;
}
#endif

static HI_S32 hifb_refreshuserbuffer(HI_U32 u32LayerId)
{
	HIFB_PAR_S *par;
	struct fb_info *info;    

	info = s_stLayer[u32LayerId].pstInfo;
	par = (HIFB_PAR_S *)info->par;

	if (par->stDispInfo.stUserBuffer.stCanvas.u32PhyAddr)
    {                       
        HIFB_BUFFER_S stCanvas;
        stCanvas = par->stDispInfo.stUserBuffer;
        stCanvas.UpdateRect.x = 0;
        stCanvas.UpdateRect.y = 0;
        stCanvas.UpdateRect.w = stCanvas.stCanvas.u32Width;
        stCanvas.UpdateRect.h = stCanvas.stCanvas.u32Height;
        
        hifb_refresh(par->stBaseInfo.u32LayerID, &stCanvas, par->stExtendInfo.enBufMode);
    }

	return HI_SUCCESS;
	
}

static HI_S32 hifb_refreshall(struct fb_info *info)
{
    HIFB_PAR_S *par = (HIFB_PAR_S *)info->par;
	
#ifdef CFG_HIFB_3D_ENABLE
    if (par->bSetStereoMode)//(IS_STEREO_SBS(par) || IS_STEREO_TAB(par))
    {      
    	if (HIFB_LAYER_BUF_STANDARD == par->stExtendInfo.enBufMode)
    	{
        	hifb_pan_display(&info->var, info);	
    	}

		if (HIFB_LAYER_BUF_NONE == par->stExtendInfo.enBufMode)
		{
			hifb_refreshuserbuffer(par->stBaseInfo.u32LayerID);
		}
    }
#endif

    if (HIFB_LAYER_BUF_STANDARD != par->stExtendInfo.enBufMode
		 && HIFB_LAYER_BUF_NONE != par->stExtendInfo.enBufMode)
    {                       
		hifb_refreshuserbuffer(par->stBaseInfo.u32LayerID);
    }

    return HI_SUCCESS;
}



/******************************************************************************
 Function        : hifb_ioctl
 Description     : set the colorkey or alpha for overlay
 Data Accessed   :
 Data Updated    :
 Output          : None
 Input           : struct inode *inode
                   struct file *file
                   unsigned HI_S32 cmd
                   unsigned long arg
                   struct fb_info *info
 Return          : return 0 if succeed, otherwise return error code
 Others          : 0
******************************************************************************/

static HI_S32 hifb_ioctl(struct fb_info *info, HI_U32 cmd, unsigned long arg)
{
#if 1
	printk("hifb_ioctl\n");
#else
    HIFB_PAR_S *par = (HIFB_PAR_S *)info->par;
    HI_VOID __user *argp = (HI_VOID __user *)arg;

    HI_S32 s32Ret = HI_SUCCESS;
    
    if ((argp == NULL) && (cmd != FBIOGET_VBLANK_HIFB) && (cmd != FBIO_WAITFOR_FREFRESH_DONE)
		&& (cmd != FBIO_FREE_LOGO))
    {
        return -EINVAL;
    }

    if ((!g_pstCap[par->stBaseInfo.u32LayerID].bLayerSupported) 
        && par->stBaseInfo.u32LayerID != HIFB_LAYER_CURSOR)
    {
        HIFB_ERROR("not supprot layer %d!\n", par->stBaseInfo.u32LayerID);
        return HI_FAILURE;
    }

    switch (cmd)
    {
        case FBIO_REFRESH:
        {
            s32Ret = hifb_onrefresh(par, argp);
            break;
        }
        
        case FBIOGET_CANVAS_BUFFER:
        {
            if (copy_to_user(argp, &(par->stDispInfo.stCanvasSur), sizeof(HIFB_BUFFER_S)))
            {
                return -EFAULT;
            }
            return HI_SUCCESS;
        }
    	case FBIOPUT_LAYER_INFO:
    	{
            s32Ret= hifb_onputlayerinfo(info, par, argp);
            break;
    	}
    	case FBIOGET_LAYER_INFO:
    	{
            HIFB_LAYER_INFO_S stLayerInfo = {0};

			hifb_wait_regconfig_work(par->stBaseInfo.u32LayerID);
			
            stLayerInfo.bPreMul           = par->stBaseInfo.bPreMul;
            stLayerInfo.BufMode           = par->stExtendInfo.enBufMode;
            stLayerInfo.eAntiflickerLevel = par->stBaseInfo.enAntiflickerLevel;
            stLayerInfo.s32XPos           = par->stExtendInfo.stPos.s32XPos;
            stLayerInfo.s32YPos           = par->stExtendInfo.stPos.s32YPos;
            stLayerInfo.u32DisplayWidth   = par->stExtendInfo.u32DisplayWidth;
            stLayerInfo.u32DisplayHeight  = par->stExtendInfo.u32DisplayHeight;
			stLayerInfo.u32ScreenWidth    = par->stExtendInfo.u32ScreenWidth;
			stLayerInfo.u32ScreenHeight   = par->stExtendInfo.u32ScreenHeight;			

            return copy_to_user(argp, &stLayerInfo, sizeof(HIFB_LAYER_INFO_S));
    	} 
#ifdef CFG_HIFB_3D_ENABLE
        case FBIOGET_ENCODER_PICTURE_FRAMING:
        {	
#ifdef HIFB_FOR_TEST				
            if (copy_to_user(argp, &par->st3DInfo.enOutStereoMode, sizeof(HIFB_STEREO_MODE_E)))
            {
                return -EFAULT;
            }
#endif
            break;
        }
        case FBIOPUT_ENCODER_PICTURE_FRAMING:
        {
#ifdef HIFB_FOR_TEST
			HIFB_STEREO_MODE_E epftmp;
            if (copy_from_user(&epftmp, argp, sizeof(HIFB_STEREO_MODE_E)))
            {
                return -EFAULT;
            }

			hifb_3DMode_callback(&par->stBaseInfo.u32LayerID, &epftmp);
#endif			
            break;
        }        
        case FBIOPUT_STEREO_MODE:
        {	
			break;
        }
        case FBIOGET_STEREO_MODE:
        { 
            break;
        }
#endif

#if 0
#ifdef CFG_HIFB_COMPRESSION_SUPPORT
        case FBIOPUT_COMPRESSION:
        {
            HI_BOOL bComp = HI_FALSE;
            
            if (copy_from_user(&bComp, argp, sizeof(HI_BOOL)))
            {
                return -EFAULT;
            }


            if (bComp == HI_TRUE)
            { 
                if (par->stDisplayInfo.enEncPicFraming == HIFB_STEREO_TOPANDBOTTOM)
                {
                    HIFB_ERROR("compression mode is not  support  3d stereo(top and bottom)\n");
                    return HI_FAILURE;
                } 			
                
                if (par->enColFmt != HIFB_FMT_ARGB8888)
                {
                    HIFB_ERROR("compression only support pixel format (ARGB8888)\n");
                    return HI_FAILURE;
                }
            }

            if (!g_pstCap[par->u32LayerID].bCompression)
            {
                //HIFB_ERROR("==hardware doesn't support compression\n");
                return HI_SUCCESS;//xx
            }
            

            if (!IS_HD_LAYER(par->u32LayerID))
            {
                HIFB_ERROR("compression only support hd layer\n");
                return HI_FAILURE;
            }

            if (!bComp)
            {
                par->bCompNeedOpen = HI_FALSE;
            }
			
            if (par->bCompression != bComp)
            {
                hifb_wait_regconfig_work(par->u32LayerID);         // wait color fmt set finish
                s_stDrvOps.HIFB_DRV_EnableCompression(par->u32LayerID, bComp); 
                s_stDrvOps.HIFB_DRV_UpdataLayerReg(par->u32LayerID);
                par->bCompression = bComp;

                hifb_refreshall(info);
            }

            break;
        }
        case FBIOGET_COMPRESSION:
        {
            if (copy_to_user(argp, &par->bCompression, sizeof(HI_BOOL)))
            {
                return -EFAULT;
            }
          
            break;
        }
#endif
#endif
        case FBIOGET_ALPHA_HIFB:
        {
            if (copy_to_user(argp, &par->stExtendInfo.stAlpha, sizeof(HIFB_ALPHA_S)))
            {
                return -EFAULT;
            }

            break;
        }

        case FBIOPUT_ALPHA_HIFB:
        {
            HIFB_ALPHA_S stAlpha = {0};
            
            if (copy_from_user(&par->stExtendInfo.stAlpha, argp, sizeof(HIFB_ALPHA_S)))
            {
                return -EFAULT;
            }

            stAlpha = par->stExtendInfo.stAlpha;
            if (!par->stExtendInfo.stAlpha.bAlphaChannel)
            {
                stAlpha.u8GlobalAlpha |= 0xff;
                par->stExtendInfo.stAlpha.u8GlobalAlpha |= 0xff;
            }

            s_stDrvOps.HIFB_DRV_SetLayerAlpha(par->stBaseInfo.u32LayerID, &stAlpha);
            break;
        }

        case FBIOGET_DEFLICKER_HIFB:
        {
            HIFB_DEFLICKER_S deflicker;

            if (!g_pstCap[par->stBaseInfo.u32LayerID].u32HDefLevel
                && !g_pstCap[par->stBaseInfo.u32LayerID].u32VDefLevel)
            {
                HIFB_WARNING("deflicker is not supported!\n");
                return -EPERM;
            }

            if (copy_from_user(&deflicker, argp, sizeof(HIFB_DEFLICKER_S)))
            {
                return -EFAULT;
            }

            deflicker.u32HDfLevel = par->stBaseInfo.u32HDflevel;
            deflicker.u32VDfLevel = par->stBaseInfo.u32VDflevel;
            if (par->stBaseInfo.u32HDflevel > 1)
            {
                if (NULL == deflicker.pu8HDfCoef)
                {
                    return -EFAULT;
                }

                if (copy_to_user(deflicker.pu8HDfCoef, par->stBaseInfo.ucHDfcoef, par->stBaseInfo.u32HDflevel - 1))
                {
                    return -EFAULT;
                }
            }

            if (par->stBaseInfo.u32VDflevel > 1)
            {
                if (NULL == deflicker.pu8VDfCoef)
                {
                    return -EFAULT;
                }

                if (copy_to_user(deflicker.pu8VDfCoef, par->stBaseInfo.ucVDfcoef, par->stBaseInfo.u32VDflevel - 1))
                {
                    return -EFAULT;
                }
            }

            if (copy_to_user(argp, &deflicker, sizeof(deflicker)))
            {
                return -EFAULT;
            }

            break;
        }

        case FBIOPUT_DEFLICKER_HIFB:
        {
            HIFB_DEFLICKER_S deflicker;

            if (!g_pstCap[par->stBaseInfo.u32LayerID].u32HDefLevel
                && !g_pstCap[par->stBaseInfo.u32LayerID].u32VDefLevel)
            {
                HIFB_WARNING("deflicker is not supported!\n");
                return -EPERM;
            }

            par->stRunInfo.bModifying = HI_TRUE;

            if (copy_from_user(&deflicker, argp, sizeof(HIFB_DEFLICKER_S)))
            {
                return -EFAULT;
            }

            par->stBaseInfo.u32HDflevel = HIFB_MIN(deflicker.u32HDfLevel, g_pstCap[par->stBaseInfo.u32LayerID].u32HDefLevel);
            if ((par->stBaseInfo.u32HDflevel > 1))
            {
                if (NULL == deflicker.pu8HDfCoef)
                {
                    return -EFAULT;
                }

                if (copy_from_user(par->stBaseInfo.ucHDfcoef, deflicker.pu8HDfCoef, par->stBaseInfo.u32HDflevel - 1))
                {
                    return -EFAULT;
                }
            }

            par->stBaseInfo.u32VDflevel = HIFB_MIN(deflicker.u32VDfLevel, g_pstCap[par->stBaseInfo.u32LayerID].u32VDefLevel);
            if (par->stBaseInfo.u32VDflevel > 1)
            {
                if (NULL == deflicker.pu8VDfCoef)
                {
                    return -EFAULT;
                }

                if (copy_from_user(par->stBaseInfo.ucVDfcoef, deflicker.pu8VDfCoef, par->stBaseInfo.u32VDflevel - 1))
                {
                    return -EFAULT;
                }
            }

            par->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_ANTIFLICKERLEVEL;
            
            par->stRunInfo.bModifying = HI_FALSE;

            break;
        }

        case FBIOGET_COLORKEY_HIFB:
        {
            HIFB_COLORKEY_S ck;
            ck.bKeyEnable = par->stExtendInfo.stCkey.bKeyEnable;
            ck.u32Key = par->stExtendInfo.stCkey.u32Key;
            if (copy_to_user(argp, &ck, sizeof(HIFB_COLORKEY_S)))
            {
                return -EFAULT;
            }

            break;
        }

        case FBIOPUT_COLORKEY_HIFB:
        {
            HIFB_COLORKEY_S ckey;

            if (copy_from_user(&ckey, argp, sizeof(HIFB_COLORKEY_S)))
            {
                return -EFAULT;
            }

            if (ckey.bKeyEnable && par->stBaseInfo.bPreMul)
            {
                HIFB_ERROR("colorkey and premul couldn't take effect at the same time!\n");
                return HI_FAILURE;
            }

			par->stRunInfo.bModifying = HI_TRUE;
			
            par->stExtendInfo.stCkey.u32Key = ckey.u32Key;
            par->stExtendInfo.stCkey.bKeyEnable = ckey.bKeyEnable;

            
            if (info->var.bits_per_pixel <= 8)
            {
                if (ckey.u32Key >= (2 << info->var.bits_per_pixel))
                {
                    HIFB_ERROR("The key :%d is out of range the palette: %d!\n",
                                ckey.u32Key, 2 << info->var.bits_per_pixel);
                    return HI_FAILURE;
                }

                par->stExtendInfo.stCkey.u8BlueMax  = par->stExtendInfo.stCkey.u8BlueMin = info->cmap.blue[ckey.u32Key];
                par->stExtendInfo.stCkey.u8GreenMax = par->stExtendInfo.stCkey.u8GreenMin = info->cmap.green[ckey.u32Key];
                par->stExtendInfo.stCkey.u8RedMax   = par->stExtendInfo.stCkey.u8RedMin = info->cmap.red[ckey.u32Key];
            }
            else
            {
                HI_U8 u8RMask, u8GMask, u8BMask;
                
                s_stDrvOps.HIFB_DRV_ColorConvert(&info->var, &par->stExtendInfo.stCkey);
				
                u8BMask  = (0xff >> s_stArgbBitField[par->stExtendInfo.enColFmt].stBlue.length);  
                u8GMask  = (0xff >> s_stArgbBitField[par->stExtendInfo.enColFmt].stGreen.length);    
                u8RMask  = (0xff >> s_stArgbBitField[par->stExtendInfo.enColFmt].stRed.length);
				
                par->stExtendInfo.stCkey.u8BlueMin  = (par->stExtendInfo.stCkey.u32Key & (~u8BMask));
                par->stExtendInfo.stCkey.u8GreenMin = ((par->stExtendInfo.stCkey.u32Key >> 8) & (~u8GMask));
                par->stExtendInfo.stCkey.u8RedMin   = ((par->stExtendInfo.stCkey.u32Key >> 16) & (~u8RMask));
            
                par->stExtendInfo.stCkey.u8BlueMax  = par->stExtendInfo.stCkey.u8BlueMin | u8BMask;
                par->stExtendInfo.stCkey.u8GreenMax = par->stExtendInfo.stCkey.u8GreenMin | u8GMask;
                par->stExtendInfo.stCkey.u8RedMax   = par->stExtendInfo.stCkey.u8RedMin | u8RMask;   
            }

			par->stExtendInfo.stCkey.u8RedMask   = 0xff;
			par->stExtendInfo.stCkey.u8BlueMask  = 0xff;
			par->stExtendInfo.stCkey.u8GreenMask = 0xff;
			
            par->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_COLORKEY;
            par->stRunInfo.bModifying          = HI_FALSE;
            break;
        }

        case FBIOPUT_SCREENSIZE:
        {
#ifndef CFG_HIFB_VIRTUAL_COORDINATE_SUPPORT			
            HIFB_SIZE_S stScreenSize;
            
            if (par->stBaseInfo.u32LayerID == HIFB_LAYER_CURSOR)
            {
                HIFB_WARNING("you shouldn't set cursor origion!");
                return HI_SUCCESS;
            }
        
            if (copy_from_user(&stScreenSize, argp, sizeof(HIFB_SIZE_S)))
            {
                return -EFAULT;
            }

            s32Ret = hifb_disp_setscreensize(par->stBaseInfo.u32LayerID, stScreenSize.u32Width, stScreenSize.u32Height);

			if (HI_SUCCESS == s32Ret)
			{
				s_stDrvOps.HIFB_DRV_SetScreenFlag(par->stBaseInfo.u32LayerID, HI_TRUE);
			}
#endif            
            break;
        }
            
        case FBIOGET_SCREENSIZE:    
        {
#ifndef CFG_HIFB_VIRTUAL_COORDINATE_SUPPORT				
            HIFB_SIZE_S stScreenSize;

			hifb_wait_regconfig_work(par->stBaseInfo.u32LayerID);
						
			stScreenSize.u32Width  = par->stExtendInfo.u32ScreenWidth;
			stScreenSize.u32Height = par->stExtendInfo.u32ScreenHeight;
            
            if (copy_to_user(argp, &stScreenSize, sizeof(HIFB_SIZE_S)))
            {
                return -EFAULT;
            }
#endif			
            break;
        }
        
        case FBIOGET_SCREEN_ORIGIN_HIFB:
        {            			
            if (copy_to_user(argp, &par->stExtendInfo.stPos, sizeof(HIFB_POINT_S)))
            {
                return -EFAULT;
            }
            
            break;
        }

        case FBIOPUT_SCREEN_ORIGIN_HIFB:
        {
			HIFB_RECT   stOutputRect;			
            HIFB_POINT_S origin;
			HI_U32 u32LayerId = par->stBaseInfo.u32LayerID;
                
            if (par->stBaseInfo.u32LayerID == HIFB_LAYER_CURSOR)
            {
                HIFB_WARNING("you shouldn't set cursor origion!");
                return HI_SUCCESS;
            }

            if (copy_from_user(&origin, argp, sizeof(HIFB_POINT_S)))
            {
                return -EFAULT;
            }

            if (origin.s32XPos < 0 || origin.s32YPos < 0)
            {
                HIFB_ERROR("It's not supported to set start pos of layer to negative!\n");
                return HI_FAILURE;
            }

			s_stDrvOps.HIFB_DRV_GetLayerOutRect(u32LayerId, &stOutputRect);
	           
            par->stRunInfo.bModifying = HI_TRUE; 
            par->stExtendInfo.stPos.s32XPos = origin.s32XPos;
            par->stExtendInfo.stPos.s32YPos = origin.s32YPos;        
			
            if (origin.s32XPos > stOutputRect.w- HIFB_MIN_WIDTH(u32LayerId))
            {
                par->stExtendInfo.stPos.s32XPos = stOutputRect.w - HIFB_MIN_WIDTH(u32LayerId);
            }

            if (origin.s32YPos > stOutputRect.h - HIFB_MIN_HEIGHT(u32LayerId))
            {
                par->stExtendInfo.stPos.s32YPos = stOutputRect.h - HIFB_MIN_HEIGHT(u32LayerId);
            }

            par->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_INRECT;
            par->stRunInfo.bModifying = HI_FALSE;
            
            break;
        }

        case FBIOGET_VBLANK_HIFB:
        {
            if (s_stDrvOps.HIFB_DRV_WaitVBlank(par->stBaseInfo.u32LayerID) < 0)
            {
                HIFB_WARNING("It is not support VBL!\n");
                return -EPERM;
            }

            break;
        }

        case FBIOPUT_SHOW_HIFB:
        {
            HI_BOOL bShow;

            if (par->stBaseInfo.u32LayerID == HIFB_LAYER_CURSOR)
            {
                HIFB_WARNING("you shouldn't show cursor by this cmd!");
                return HI_SUCCESS;
            }

            if (copy_from_user(&bShow, argp, sizeof(HI_BOOL)))
            {
                return -EFAULT;
            }

            /* reset the same status */
            if (bShow == par->stExtendInfo.bShow)
            {
                HIFB_INFO("The layer is show(%d) now!\n", par->stExtendInfo.bShow);
                return 0;
            }

            par->stRunInfo.bModifying          = HI_TRUE;
            par->stExtendInfo.bShow            = bShow;
            par->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_SHOW;
            par->stRunInfo.bModifying          = HI_FALSE;

            break;
        }

        case FBIOGET_SHOW_HIFB:
        {
            if (copy_to_user(argp, &par->stExtendInfo.bShow, sizeof(HI_BOOL)))
            {
                return -EFAULT;
            }

            break;
        }

        case FBIO_WAITFOR_FREFRESH_DONE:
        {
            if (par->stRunInfo.s32RefreshHandle 
               && par->stExtendInfo.enBufMode != HIFB_LAYER_BUF_ONE)
            {
                s32Ret = s_stDrvTdeOps.HIFB_DRV_WaitForDone(par->stRunInfo.s32RefreshHandle, 1000);
                if (s32Ret < 0)
                {
                    HIFB_ERROR("HIFB_DRV_WaitForDone failed!ret=%x\n", s32Ret);
                    return HI_FAILURE;
                }
            }

            break;
        }

        case FBIOGET_CAPABILITY_HIFB:
        {
            if (copy_to_user(argp, (HI_VOID *)&g_pstCap[par->stBaseInfo.u32LayerID], sizeof(HIFB_CAPABILITY_S)))
            {
                HIFB_ERROR("FBIOGET_CAPABILITY_HIFB error\n");
                return -EFAULT;
            }

            break;
        }
#ifdef CFG_HIFB_SCROLLTEXT_SUPPORT
        case FBIO_SCROLLTEXT_CREATE:
        {
            HIFB_SCROLLTEXT_CREATE_S stScrollText;
            
            if (copy_from_user(&stScrollText, argp, sizeof(HIFB_SCROLLTEXT_CREATE_S)))
            {
                return -EFAULT;
            }

			if (stScrollText.stAttr.ePixelFmt >= HIFB_FMT_BUTT)
			{
				HIFB_ERROR("Invalid attributes.\n");
				return HI_FAILURE;
			}

			if (stScrollText.stAttr.stRect.w < 0 || stScrollText.stAttr.stRect.h < 0)
			{
				HIFB_ERROR("Invalid attributes.\n");
				return HI_FAILURE;
			}
			
			s32Ret = hifb_create_scrolltext(par->stBaseInfo.u32LayerID, &stScrollText);
            if (HI_SUCCESS != s32Ret)
            {
                return -EFAULT;
            }
            
            return copy_to_user(argp, &stScrollText, sizeof(HIFB_SCROLLTEXT_CREATE_S));
                     
        }
        case FBIO_SCROLLTEXT_FILL:
        {
            HIFB_SCROLLTEXT_DATA_S stScrollTextData;
            
            if (copy_from_user(&stScrollTextData, argp, sizeof(HIFB_SCROLLTEXT_DATA_S)))
            {
                return -EFAULT;
            }

            if (HI_NULL == stScrollTextData.u32PhyAddr
                && HI_NULL == stScrollTextData.pu8VirAddr)
            {
                HIFB_ERROR("invalid usr data!\n");
                return -EFAULT;
            }


			s32Ret = hifb_fill_scrolltext(&stScrollTextData);
            if (HI_SUCCESS != s32Ret)
            {
                HIFB_ERROR("failed to fill data to scroll text !\n");
                return -EFAULT;
            }         
            
            break;
        }
		case FBIO_SCROLLTEXT_DESTORY:
        {
            HI_U32 u32LayerId, u32ScrollTextID, u32Handle;
            
            if (copy_from_user(&u32Handle, argp, sizeof(HI_U32)))
            {
                return -EFAULT;
            }

			s32Ret = hifb_parse_scrolltexthandle(u32Handle,&u32LayerId,&u32ScrollTextID);
			if (HI_SUCCESS != s32Ret)
			{
				HIFB_ERROR("invalid scrolltext handle!\n");
                return -EFAULT;
			}

			s32Ret = hifb_destroy_scrolltext(u32LayerId,u32ScrollTextID);
			if (HI_SUCCESS != s32Ret)
			{
				HIFB_ERROR("failed to destroy scrolltext!\n");
                return -EFAULT;	
			}
            
            break;
        } 
        case FBIO_SCROLLTEXT_PAUSE:
        {
            HI_U32 u32LayerId, u32ScrollTextID, u32Handle;
			HIFB_SCROLLTEXT_S  *pstScrollText;
            
            if (copy_from_user(&u32Handle, argp, sizeof(HI_U32)))
            {
                return -EFAULT;
            }
				
			s32Ret = hifb_parse_scrolltexthandle(u32Handle,&u32LayerId,&u32ScrollTextID);
			if (HI_SUCCESS != s32Ret)
			{
				HIFB_ERROR("invalid scrolltext handle!\n");
                return -EFAULT;
			}

			pstScrollText = &(s_stTextLayer[u32LayerId].stScrollText[u32ScrollTextID]);
			pstScrollText->bPause = HI_TRUE;			
            
            break;
        }
		case FBIO_SCROLLTEXT_RESUME:
        {
            HI_U32 u32LayerId, u32ScrollTextID, u32Handle;
			HIFB_SCROLLTEXT_S  *pstScrollText;
            
            if (copy_from_user(&u32Handle, argp, sizeof(HI_U32)))
            {
                return -EFAULT;
            }

			s32Ret = hifb_parse_scrolltexthandle(u32Handle,&u32LayerId,&u32ScrollTextID);
			if (HI_SUCCESS != s32Ret)
			{
				HIFB_ERROR("invalid scrolltext handle!\n");
                return -EFAULT;
			}

			pstScrollText = &(s_stTextLayer[u32LayerId].stScrollText[u32ScrollTextID]);
			pstScrollText->bPause = HI_FALSE;			
            
            break;
        }
#endif        
		case FBIOGET_ZORDER:
		{
			HI_U32  u32Zorder;
			s_stDrvOps.HIFB_DRV_GetLayerPriority(par->stBaseInfo.u32LayerID, &u32Zorder);
            return copy_to_user(argp, &(u32Zorder), sizeof(HI_U32));				
		}
		case FBIOPUT_ZORDER:
		{
            HIFB_ZORDER_E enZorder;

            if (copy_from_user(&enZorder, argp, sizeof(HIFB_ZORDER_E)))
            {
                return -EFAULT;
            }			

			if (enZorder >= HIFB_ZORDER_BUTT)
			{
				HIFB_ERROR("invalid operation.\n");
				return HI_FAILURE;
			}

			s_stDrvOps.HIFB_DRV_SetLayerPriority(par->stBaseInfo.u32LayerID, enZorder);
			break;
		}
#ifdef CFG_HIFB_LOGO_SUPPORT		
        case FBIO_FREE_LOGO:
        {
            if (g_bLogoShow)
            {
                hifb_clear_logo(0);
            }
            break;
        }
#endif		
#ifdef CFG_HIFB_3D_ENABLE
	    case FBIOPUT_STEREO_DEPTH:
        {
			HI_S32 s32StereoDepth;

			if (copy_from_user(&s32StereoDepth, argp, sizeof(HI_S32)))
            {
                return -EFAULT;
            }

			if (!par->bSetStereoMode)
			{
				HIFB_ERROR("u need to set disp stereo mode first.\n");
				return HI_FAILURE;
			}
			
			s_stDrvOps.HIFB_DRV_SetStereoDepth(par->stBaseInfo.u32LayerID, s32StereoDepth);

			par->st3DInfo.s32StereoDepth = s32StereoDepth;
			
            break;
        }

		case FBIOGET_STEREO_DEPTH:
        {
			if (!par->bSetStereoMode)
			{
				HIFB_ERROR("u need to set disp stereo mode first.\n");
				return HI_FAILURE;
			}

			if (copy_to_user(argp, &(par->st3DInfo.s32StereoDepth), sizeof(HI_S32)))
            {
                return -EFAULT;
            }
			
            break;
        }
#endif		
        default:
        {
            HIFB_ERROR("the command:0x%x is unsupported!\n", cmd);
            return -EINVAL;
        }
    }

    return s32Ret;
#endif
}

static HI_VOID hifb_3DMode_callback(HI_VOID * pParaml,HI_VOID * pParamr)
{
#ifdef CFG_HIFB_3D_ENABLE
	HI_U32 *pu32LayerId;
	HIFB_STEREO_MODE_E *penStereoMode;
	struct fb_info *info;
        HIFB_PAR_S *pstPar;

	pu32LayerId   = (HI_U32 *)pParaml;
	penStereoMode = (HIFB_STEREO_MODE_E *)pParamr;

	info   = s_stLayer[*pu32LayerId].pstInfo;
	pstPar = (HIFB_PAR_S *)(info->par);

	if (HIFB_STEREO_MONO == *penStereoMode)
	{
		pstPar->bSetStereoMode = HI_FALSE;
		pstPar->st3DInfo.enInStereoMode  = HIFB_STEREO_MONO;
	}
	else
	{
		pstPar->bSetStereoMode = HI_TRUE; 
		s_stDrvOps.HIFB_DRV_SetTriDimAddr(*pu32LayerId, pstPar->st3DInfo.u32rightEyeAddr);
	}

	pstPar->st3DInfo.enOutStereoMode = *penStereoMode;
	s_stDrvOps.HIFB_DRV_SetTriDimMode(*pu32LayerId, *penStereoMode);
	//hifb_checkandalloc_3dmem(*pu32LayerId);

	/**********these parameters will take effect after refresh*************/
    pstPar->stRunInfo.bModifying          = HI_TRUE;
	pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_STRIDE;
	pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_INRECT;
	pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_DISPLAYADDR;
	pstPar->stRunInfo.bModifying          = HI_FALSE;
	
    hifb_assign_dispbuf(pstPar->stBaseInfo.u32LayerID); 
    hifb_clearallstereobuf(info);

    if (HIFB_LAYER_BUF_STANDARD == pstPar->stExtendInfo.enBufMode)
	{
    	hifb_pan_display(&info->var, info);	
	}
	else
	{
		hifb_refreshuserbuffer(pstPar->stBaseInfo.u32LayerID);
	}
#endif	
	return;
}

struct
{
	HI_U32 Data_384; //384
	HI_U32 Data_388; //388
	HI_U32 Data_392; //392
	wait_queue_head_t    FrameEndEvent; //396
	struct sw_sync_timeline *Data_408; //408
	//->412
	//???
} s_SyncInfo; //81143790???

static HI_S32 hifb_layer_init(HI_U32 u32LayerID)
{
	struct fb_info *info;
    HIFB_PAR_S *par;
	HIFB_COLOR_FMT_E enColorFmt;
	HIFB_RECT stInRect;

	info   = s_stLayer[u32LayerID].pstInfo;
	par = (HIFB_PAR_S *)(info->par);

#if 0
	if (IS_HD_LAYER(u32LayerID))
    {
        info->var = s_stDefVar[HIFB_LAYER_TYPE_HD];
    }
    else if (IS_SD_LAYER(u32LayerID))
    {
        info->var = s_stDefVar[HIFB_LAYER_TYPE_SD];
    }
    else if (IS_AD_LAYER(u32LayerID))
    {
        info->var = s_stDefVar[HIFB_LAYER_TYPE_AD];
    }
#else
    if (u32LayerID == HIFB_LAYER_HD_0) //0
    {
        info->var = s_stDefVar[HIFB_LAYER_TYPE_HD];
    }
    else if (u32LayerID == HIFB_LAYER_SD_0) //4
    {
        info->var = s_stDefVar[HIFB_LAYER_TYPE_SD];
    }
    else if (((u32LayerID >= 8) && (u32LayerID <= 11)) ||
    		((u32LayerID >= 1) && (u32LayerID <= 3)))
    {
        info->var = s_stDefVar[HIFB_LAYER_TYPE_AD];
    }
    else if ((u32LayerID >= 5) && (u32LayerID <= 7))
    {
        info->var = s_stDefVar[HIFB_LAYER_TYPE_AD];
    }
#endif
    else
    {
		return HI_FAILURE;
    }

	enColorFmt = hifb_getfmtbyargb(&info->var.red, &info->var.green, &info->var.blue, &info->var.transp, info->var.bits_per_pixel);
	
	memset(&(par->stDispInfo.stUserBuffer), 0, sizeof(HIFB_BUFFER_S));
	memset(&(par->stDispInfo.stCanvasSur),  0, sizeof(HIFB_SURFACE_S));

	par->stBaseInfo.bNeedAntiflicker = HI_FALSE;
	hifb_disp_setantiflickerlevel(par->stBaseInfo.u32LayerID, HIFB_LAYER_ANTIFLICKER_AUTO);    

	par->stRunInfo.bModifying = HI_FALSE;
	par->stRunInfo.u32ParamModifyMask = 0;

	info->var.xoffset = 0;
	info->var.yoffset = 0;        

	par->stExtendInfo.stAlpha.bAlphaEnable  = HI_TRUE;
	par->stExtendInfo.stAlpha.bAlphaChannel = HI_FALSE;
	par->stExtendInfo.stAlpha.u8Alpha0      = HIFB_ALPHA_TRANSPARENT;
	par->stExtendInfo.stAlpha.u8Alpha1      = HIFB_ALPHA_OPAQUE;
	par->stExtendInfo.stAlpha.u8GlobalAlpha = HIFB_ALPHA_OPAQUE;
	s_stDrvOps.HIFB_DRV_SetLayerAlpha(par->stBaseInfo.u32LayerID, &par->stExtendInfo.stAlpha);

	memset(&(par->stExtendInfo.stCkey), 0, sizeof(HIFB_COLORKEYEX_S));
	par->stExtendInfo.stCkey.u8RedMask   = 0xff;
	par->stExtendInfo.stCkey.u8GreenMask = 0xff;
	par->stExtendInfo.stCkey.u8BlueMask  = 0xff;
	s_stDrvOps.HIFB_DRV_SetLayerKeyMask(par->stBaseInfo.u32LayerID, &par->stExtendInfo.stCkey);

	par->stExtendInfo.enColFmt = enColorFmt;
	s_stDrvOps.HIFB_DRV_SetLayerDataFmt(par->stBaseInfo.u32LayerID, par->stExtendInfo.enColFmt);

	memset(&par->stExtendInfo.stPos, 0, sizeof(HIFB_POINT_S));

	info->fix.line_length = info->var.xres_virtual*(info->var.bits_per_pixel >> 3);

	par->stExtendInfo.u32DisplayWidth       = info->var.xres;
	par->stExtendInfo.u32DisplayHeight      = info->var.yres;
	par->st3DInfo.st3DSurface.u32Pitch      = info->fix.line_length;
	par->st3DInfo.st3DSurface.enFmt         = par->stExtendInfo.enColFmt;
	par->st3DInfo.st3DSurface.u32Width      = info->var.xres;
	par->st3DInfo.st3DSurface.u32Height     = info->var.yres;
	par->st3DInfo.st3DMemInfo.u32StereoMemLen   = HI_NULL;
	par->st3DInfo.st3DMemInfo.u32StereoMemStart = HI_NULL;
	    
	stInRect.x = 0;
	stInRect.y = 0;
	stInRect.w = info->var.xres;
	stInRect.h = info->var.yres;        
	/*******set layer's inrect the same as outrect when initial********/
	s_stDrvOps.HIFB_DRV_SetLayerInRect  (par->stBaseInfo.u32LayerID, &stInRect);
#if 0
	s_stDrvOps.HIFB_DRV_SetLayerOutRect(par->stBaseInfo.u32LayerID, &stInRect);
	s_stDrvOps.HIFB_DRV_GetDispSize(par->stBaseInfo.u32LayerID, &stDispRect);
	par->stExtendInfo.u32ScreenWidth  = stDispRect.w;
	par->stExtendInfo.u32ScreenHeight = stDispRect.h;
		
	/*******init layer's outrect********/
	if (/*!(g_u32HifbState & HIFB_STATE_LOGO_IN) && */!s_stDrvOps.HIFB_DRV_GetScreenFlag(par->stBaseInfo.u32LayerID))
	{
		if (!s_stDrvOps.HIFB_DRV_GetInitScreenFlag(par->stBaseInfo.u32LayerID))
		{
			HIFB_SIZE_S stSize;
			HIFB_RECT   stOutRect;
			hifb_defscreensize(info->var.xres, info->var.yres, &stSize);
			
			stOutRect.x = 0;
			stOutRect.y = 0;
			stOutRect.w = stSize.u32Width;
			stOutRect.h = stSize.u32Height;
			
			s_stDrvOps.HIFB_DRV_SetLayerOutRect(par->stBaseInfo.u32LayerID, &stOutRect);
			
			par->stExtendInfo.u32ScreenWidth  = stSize.u32Width;
			par->stExtendInfo.u32ScreenHeight = stSize.u32Height;
		}
	}
#endif	
	/***********************end***************************/
	s_stDrvOps.HIFB_DRV_SetLayerStride  (par->stBaseInfo.u32LayerID, info->fix.line_length);

	mutex_init(&par->st3DInfo.st3DMemInfo.stStereoMemLock);

	s_stDrvOps.HIFB_DRV_SetTriDimMode(par->stBaseInfo.u32LayerID, HIFB_STEREO_MONO); 

	par->stExtendInfo.enBufMode               = HIFB_LAYER_BUF_STANDARD;

	par->stRunInfo.u32BufNum = HIFB_MAX_FLIPBUF_NUM;

	par->bPanFlag = HI_FALSE;

	par->Data_8 = 0;
	par->Data_12 = 1;

	spin_lock_init(&par->stBaseInfo.lock);

	return HI_SUCCESS;
}

HI_VOID hifb_getmaxscreensize(HI_UNF_ENC_FMT_E enFmt, HI_U32 *pMaxW, HI_U32 *pMaxH)
{
    switch(enFmt)
    {
        case HI_UNF_ENC_FMT_1080P_60:
        case HI_UNF_ENC_FMT_1080P_50:
        case HI_UNF_ENC_FMT_1080i_60:
        case HI_UNF_ENC_FMT_1080i_50:
        {
            *pMaxW = 1920;
            *pMaxH = 1080;
            break;
        }
        case HI_UNF_ENC_FMT_720P_60:
        case HI_UNF_ENC_FMT_720P_50:
        {
            *pMaxW = 1280;
            *pMaxH = 720;
            break;
        }
        case HI_UNF_ENC_FMT_576P_50:
        case HI_UNF_ENC_FMT_PAL:
        {
            *pMaxW = 720;
            *pMaxH = 576;
            break;        
        }
        case HI_UNF_ENC_FMT_480P_60:
        case HI_UNF_ENC_FMT_NTSC:
        {
            *pMaxW = 720;
            *pMaxH = 480; 
            break;         
        }
        /* bellow are vga display formats */
        case HI_UNF_ENC_FMT_861D_640X480_60:
        {
            *pMaxW = 640;
            *pMaxH = 480; 
            break;
        }
        case HI_UNF_ENC_FMT_VESA_800X600_60:
        {
            *pMaxW = 800;
            *pMaxH = 600; 
            break;
        }
        case HI_UNF_ENC_FMT_VESA_1024X768_60:
        {
            *pMaxW = 1024;
            *pMaxH = 768; 
            break;
        }
        case HI_UNF_ENC_FMT_VESA_1280X720_60:
        {
            *pMaxW = 1280;
            *pMaxH = 720; 
            break;
        }
        case HI_UNF_ENC_FMT_VESA_1280X800_60:
        {
            *pMaxW = 1280;
            *pMaxH = 800; 
            break;
        }
        case HI_UNF_ENC_FMT_VESA_1280X1024_60:
        {
            *pMaxW = 1280;
            *pMaxH = 1024; 
            break;
        }
        case HI_UNF_ENC_FMT_VESA_1360X768_60:
        {
            *pMaxW = 1360;
            *pMaxH = 768; 
            break;
        }
        case HI_UNF_ENC_FMT_VESA_1366X768_60:
        {
            *pMaxW = 1366;
            *pMaxH = 768;  
            break;
        }
        case HI_UNF_ENC_FMT_VESA_1400X1050_60:
        {
            *pMaxW = 1400;
            *pMaxH = 1050; 
            break;
        }
        case HI_UNF_ENC_FMT_VESA_1440X900_60:
        {
            *pMaxW = 1440;
            *pMaxH = 900; 
            break;
        }
        case HI_UNF_ENC_FMT_VESA_1440X900_60_RB:
        {
            *pMaxW = 1440;
            *pMaxH = 900; 
            break;
        }
        case HI_UNF_ENC_FMT_VESA_1600X900_60_RB:
        {
            *pMaxW = 1600;
            *pMaxH = 900; 
            break;
        }
        case HI_UNF_ENC_FMT_VESA_1600X1200_60:
        {
            *pMaxW = 1600;
            *pMaxH = 1200;  
            break;
        }
        case HI_UNF_ENC_FMT_VESA_1680X1050_60:
        {
            *pMaxW = 1680;
            *pMaxH = 1050; 
            break;
        } 
        
        case HI_UNF_ENC_FMT_VESA_1920X1080_60:
        {
            *pMaxW = 1920;
            *pMaxH = 1080; 
            break;
        }
        case HI_UNF_ENC_FMT_VESA_1920X1200_60:
        {
            *pMaxW = 1920;
            *pMaxH = 1200; 
            break;
        }
        case HI_UNF_ENC_FMT_VESA_2048X1152_60:
        {
            *pMaxW = 2048;
            *pMaxH = 1152;      
            break;
        }
        default:
        {
            *pMaxW = 1920;
            *pMaxH = 1080; 
            break;
        }
    }

    return;    
}

#ifdef CFG_HIFB_LOGO_SUPPORT
static HI_S32 hifb_set_logosd(HI_U32 u32LayerID)
{
	HI_S32 s32Ret;     	
	HI_U32 u32Stride;
    HIFB_PAR_S *par;
	struct fb_info *info;
	HIFB_OSD_DATA_S stLogoLayerData;

	info   = s_stLayer[u32LayerID].pstInfo;
	par = (HIFB_PAR_S *)(info->par);	

	if (!g_bLogoShow)
	{
		return HI_SUCCESS;
	}

	if (!IS_HD_LAYER(u32LayerID))
	{
		return HI_SUCCESS;
	}

    s32Ret = s_stDrvOps.HIFB_DRV_GetOSDData(HIFB_LOGO_LAYER_ID, &stLogoLayerData);
    if (s32Ret != HI_SUCCESS)
    {
        HIFB_ERROR("failed to Get OSDData from layer%d !\n",HIFB_LOGO_LAYER_ID);                
        return s32Ret;
    }

	info->var.bits_per_pixel = hifb_getbppbyfmt(stLogoLayerData.eFmt);
	if (info->var.bits_per_pixel == 0)
	{
		HIFB_ERROR("unsupported fmt!\n");
		return HI_FAILURE;
	}	

    info->var.red    = s_stArgbBitField[stLogoLayerData.eFmt].stRed;
    info->var.green  = s_stArgbBitField[stLogoLayerData.eFmt].stGreen;
    info->var.blue   = s_stArgbBitField[stLogoLayerData.eFmt].stBlue;
    info->var.transp = s_stArgbBitField[stLogoLayerData.eFmt].stTransp;

	par->stExtendInfo.stAlpha = stLogoLayerData.stAlpha;
	s_stDrvOps.HIFB_DRV_SetLayerAlpha(u32LayerID, &par->stExtendInfo.stAlpha);

	u32Stride = stLogoLayerData.stInRect.w * (info->var.bits_per_pixel/8);
	/*detect extracted line*/
	if (stLogoLayerData.u32Stride >= u32Stride*2)
	{
		stLogoLayerData.stInRect.h *= 2;
	}
		
	info->var.xres = stLogoLayerData.stInRect.w;
	info->var.yres = stLogoLayerData.stInRect.h;
	info->var.xres_virtual = info->var.xres;
	info->var.yres_virtual = info->var.yres;
	info->fix.line_length  = ((((info->var.xres_virtual * info->var.bits_per_pixel) >> 3) + 0xf) & 0xfffffff0);

    par->stExtendInfo.enColFmt = stLogoLayerData.eFmt;
	par->stExtendInfo.stPos.s32XPos    = stLogoLayerData.stInRect.x;
	par->stExtendInfo.stPos.s32YPos    = stLogoLayerData.stInRect.y;
	par->stExtendInfo.u32DisplayWidth  = info->var.xres;
	par->stExtendInfo.u32DisplayHeight = info->var.yres;




	/************set the input rect of layer*************/
    s_stDrvOps.HIFB_DRV_SetLayerInRect (u32LayerID, &stLogoLayerData.stInRect);
	s_stDrvOps.HIFB_DRV_SetLayerStride (u32LayerID, info->fix.line_length);
	s_stDrvOps.HIFB_DRV_SetLayerDataFmt(u32LayerID, stLogoLayerData.eFmt);

	return HI_SUCCESS;
}
#endif

HI_S32 hifb_read_proc(struct seq_file *p, HI_VOID *v)
{
#warning TODO: hifb_read_proc
	printk("hifb_read_proc: TODO\n");
}

HI_S32 hifb_write_proc(struct file * file,
    const char __user * buf, size_t count, loff_t *ppos)
{
#warning TODO: hifb_write_proc
	printk("hifb_write_proc: TODO\n");
}

static HI_VOID hifb_createproc(HI_U32 u32LayerID)
{
	HIFB_PAR_S * par = (HIFB_PAR_S *)(s_stLayer[u32LayerID].pstInfo->par);

	HI_CHAR entry_name[16];
	GFX_PROC_ITEM_S item;

	snprintf(entry_name, sizeof(entry_name), "hifb%d", u32LayerID);
	item.fnRead = hifb_read_proc;
	item.fnWrite= hifb_write_proc;
	item.fnIoctl= HI_NULL;
	HI_GFX_PROC_AddModule(entry_name, &item, (HI_VOID *)s_stLayer[u32LayerID].pstInfo);

	HIFB_INFO("success to create %s proc!\n", entry_name); //4844

	par->Data_380 = 0;
	par->Data_384 = 1;
	par->Data_392 = 13;
}

static HI_S32 hifb_createwbcproc(HIFB_PAR_S * par)
{
    HI_S32 s32Ret;
    HIFB_PAR_S * par1;
    HIFB_SLV_LAYER_INFO_S fp160;

	s32Ret = s_stDrvOps.HIFB_DRV_GetSlvLayerInfo(&fp160);
	if (s32Ret == HI_FAILURE)
	{
		return HI_FAILURE;
	}

	if (fp160.u32LayerId >= HIFB_LAYER_ID_BUTT+1)
	{
		return HI_FAILURE;
	}

	par1 = (HIFB_PAR_S *)(s_stLayer[fp160.u32LayerId].pstInfo->par);

	par->Data_392 = fp160.u32LayerId;
	par1->Data_388++;

	HIFB_INFO("create wbc proc, master layerid %d, masterlayernum %d\n", par->stBaseInfo.u32LayerID, par1->Data_388); //4880

	if (par1->Data_384 == 0)
	{
		HI_CHAR entry_name[16];
		GFX_PROC_ITEM_S item;

		snprintf(entry_name, sizeof(entry_name), "hifb%d", fp160.u32LayerId);
		item.fnRead = hifb_read_proc;
		item.fnWrite= hifb_write_proc;
		item.fnIoctl= HI_NULL;
		HI_GFX_PROC_AddModule(entry_name, &item, (HI_VOID *)s_stLayer[fp160.u32LayerId].pstInfo);

		HIFB_INFO("success to create %s proc!\n", entry_name); //4893

		par1->Data_384 = 1;
		par1->Data_380 = 1;
	}

	return HI_SUCCESS;
}

static HI_S32 hifb_open (struct fb_info *info, HI_S32 user)
{     
#if 1
	printk("hifb_open\n");
#else
    HI_S32 cnt; 
    HI_S32 s32Ret;
	HIFB_PAR_S *par;
	HI_U32 u32BufSize, u32Pitch; 
	par = (HIFB_PAR_S *)info->par;
	cnt = atomic_read(&par->stBaseInfo.ref_count);
	
    /****************assure layer is legal*********************/
    if (!g_pstCap[par->stBaseInfo.u32LayerID].bLayerSupported)
    {
        HIFB_ERROR("gfx%d is not supported!\n", par->stBaseInfo.u32LayerID); //5107
        return HI_FAILURE;
    }
	
    /****************** open the layer first time **************/
    if (!cnt)
    {		
		s32Ret = s_stDrvTdeOps.HIFB_DRV_TdeOpen();
		if (s32Ret != HI_SUCCESS)
        {
            HIFB_INFO("tde was not avaliable!\n"); //5117
        }
		
        s32Ret = s_stDrvOps.HIFB_DRV_OpenLayer(par->stBaseInfo.u32LayerID);
        if (s32Ret != HI_SUCCESS)
        {
            HIFB_ERROR("failed to open layer%d !\n", par->stBaseInfo.u32LayerID); //5123
            return s32Ret;
        }	

        if (par->stBaseInfo.u32LayerID == HIFB_LAYER_HD_0) //0
        {
        	s_SyncInfo.Data_384 = s_SyncInfo.Data_392 = par->stBaseInfo.u32LayerID;
        	s_SyncInfo.Data_388 = 1;
        	init_waitqueue_head(&(s_SyncInfo.FrameEndEvent));
        	s_SyncInfo.Data_408 = sw_sync_timeline_create("hifb");
        }

		/***********layer parameters initial***************/
		hifb_layer_init(par->stBaseInfo.u32LayerID);
		
#ifdef CFG_HIFB_LOGO_SUPPORT
		/***********config layer with osd data***************/
		hifb_set_logosd(par->stBaseInfo.u32LayerID);
#endif		

		/***********alloc disp buffer, set disp address******/		           
//		u32Pitch = ((((info->var.xres_virtual * info->var.bits_per_pixel) >> 3) + 0xf) & 0xfffffff0);
		u32Pitch = ((info->var.xres_virtual * info->var.bits_per_pixel) >> 3);
		if (u32Pitch & 0xFF)
		{
			u32Pitch >>= 8;
			if (!(u32Pitch % 2)) u32Pitch += 1;
			else u32Pitch += 2;
			u32Pitch <<= 8;
		}
		u32BufSize  = info->var.yres_virtual*u32Pitch;
		//u32BufSize *= par->stRunInfo.u32BufNum;
		
        if(info->fix.smem_len < u32BufSize)
        {
			hifb_realloc_layermem(info, u32BufSize);			
		}

		par->stRunInfo.u32IndexForInt = 0;

		hifb_assign_dispbuf(par->stBaseInfo.u32LayerID);
		/*clear fb memory if it's the first time to open layer*/
		memset(info->screen_base, 0, info->fix.smem_len);

		s_stDrvOps.HIFB_DRV_SetLayerAddr(par->stBaseInfo.u32LayerID, info->fix.smem_start);
		par->stRunInfo.u32ScreenAddr = info->fix.smem_start;
		par->st3DInfo.u32rightEyeAddr = par->stRunInfo.u32ScreenAddr;

		/***********set callback function to hard ware*********/
		s32Ret = s_stDrvOps.HIFB_DRV_SetIntCallback(HIFB_CALLBACK_TYPE_VO, (IntCallBack)hifb_vo_callback, par->stBaseInfo.u32LayerID);
		if (s32Ret != HI_SUCCESS)
		{
			HIFB_ERROR("failed to set vo callback function, open layer%d failure\n", par->stBaseInfo.u32LayerID); //5165
			return s32Ret;
		}

		s32Ret = s_stDrvOps.HIFB_DRV_SetIntCallback(HIFB_CALLBACK_TYPE_3DMode_CHG, (IntCallBack)hifb_3DMode_callback, par->stBaseInfo.u32LayerID);
		if (s32Ret != HI_SUCCESS)
		{
			HIFB_ERROR("failed to set stereo mode change callback function, open layer%d failure\n", par->stBaseInfo.u32LayerID); //5172
			return s32Ret;
		}

		if (par->stBaseInfo.u32LayerID == 0)
		{
			//805f2ccc
			s32Ret = s_stDrvOps.HIFB_DRV_SetIntCallback(HIFB_CALLBACK_TYPE_0x10, (IntCallBack)hifb_frame_end_callback, par->stBaseInfo.u32LayerID);
			if (s32Ret != HI_SUCCESS)
			{
				HIFB_ERROR("failed to set frame end callback function, open layer%d failure\n", par->stBaseInfo.u32LayerID); //5182
				return s32Ret;
			}
		}
		//805f2b90
		if (((HIFB_PAR_S *)(s_stLayer[par->stBaseInfo.u32LayerID].pstInfo->par))->Data_384 == 0)
		{
			hifb_createproc(par->stBaseInfo.u32LayerID);
			hifb_createwbcproc(par);
		}
        //805f2bac
        s_stDrvOps.HIFB_DRV_EnableLayer(par->stBaseInfo.u32LayerID, HI_TRUE);

        par->stExtendInfo.bShow = HI_TRUE;
        par->Data_16 = 1;
    }

    /* increase reference count */
    atomic_inc(&par->stBaseInfo.ref_count);
    par->stExtendInfo.bOpen = HI_TRUE;
#endif

    return HI_SUCCESS;
}

/******************************************************************************
 Function        : hifb_release
 Description     : open the framebuffer and disable the layer
 Data Accessed   :
 Data Updated    :
 Output          : None
                   struct fb_info *info
 Return          : return 0 if succeed, otherwise return -EINVAL
 Others          : 0
******************************************************************************/
static HI_S32 hifb_release (struct fb_info *info, HI_S32 user)
{
#if 1
	printk("hifb_release\n");
#else
    HIFB_PAR_S *par = (HIFB_PAR_S *)info->par;
    HI_U32 cnt = atomic_read(&par->stBaseInfo.ref_count);

    if (!cnt)
    {
        return -EINVAL;
    }

    /* only one user */
    if (cnt == 1 )
    {
        par->stExtendInfo.bShow = HI_FALSE;
        if (par->stBaseInfo.u32LayerID != HIFB_LAYER_CURSOR)
        {
            /* disable the layer */
#if 0
            s_stDrvOps.HIFB_DRV_EnableCompression(par->stBaseInfo.u32LayerID, 0);
#endif
            s_stDrvOps.HIFB_DRV_EnableLayer(par->stBaseInfo.u32LayerID, HI_FALSE);
            s_stDrvOps.HIFB_DRV_UpdataLayerReg(par->stBaseInfo.u32LayerID);

			/*************unRegister callback function************************/
	        s_stDrvOps.HIFB_DRV_SetIntCallback(HIFB_CALLBACK_TYPE_VO,         HI_NULL, par->stBaseInfo.u32LayerID);
			s_stDrvOps.HIFB_DRV_SetIntCallback(HIFB_CALLBACK_TYPE_3DMode_CHG, HI_NULL, par->stBaseInfo.u32LayerID);


            memset(info->screen_base, 0, info->fix.smem_len);
            hifb_freeccanbuf(par);
#ifdef CFG_HIFB_SCROLLTEXT_SUPPORT
			if (s_stTextLayer[par->stBaseInfo.u32LayerID].bAvailable)
			{
			    HI_U32 i;
				for (i = 0; i < SCROLLTEXT_NUM; i++)
				{
				    if (s_stTextLayer[par->stBaseInfo.u32LayerID].stScrollText[i].bAvailable)
				    {
				        hifb_freescrolltext_cachebuf(&(s_stTextLayer[par->stBaseInfo.u32LayerID].stScrollText[i]));
						memset(&s_stTextLayer[par->stBaseInfo.u32LayerID].stScrollText[i],0,sizeof(HIFB_SCROLLTEXT_S));
				    }
				}

				s_stTextLayer[par->stBaseInfo.u32LayerID].bAvailable = HI_FALSE;
				s_stTextLayer[par->stBaseInfo.u32LayerID].u32textnum = 0;
				s_stTextLayer[par->stBaseInfo.u32LayerID].u32ScrollTextId = 0;
			}
#endif
			
#ifdef CFG_HIFB_3D_ENABLE
            hifb_freestereobuf(par);
#endif
            par->st3DInfo.enInStereoMode  = HIFB_STEREO_MONO;
			par->st3DInfo.enOutStereoMode = HIFB_STEREO_MONO;
            par->bSetStereoMode           = HI_FALSE;

			s_stDrvOps.HIFB_DRV_SetTriDimMode(par->stBaseInfo.u32LayerID, HIFB_STEREO_MONO);
			s_stDrvOps.HIFB_DRV_SetTriDimAddr(par->stBaseInfo.u32LayerID, HI_NULL);
			s_stDrvOps.HIFB_DRV_SetLayerAddr (par->stBaseInfo.u32LayerID, HI_NULL);

            s_stDrvOps.HIFB_DRV_CloseLayer(par->stBaseInfo.u32LayerID);
        }
      
      par->stExtendInfo.bOpen = HI_FALSE;
    }

#ifdef CFG_HIFB_LOGO_SUPPORT    
    if (g_bLogoShow)
    {
        hifb_clear_logo(0);
    }
#endif

    /* decrease the reference count */
    atomic_dec(&par->stBaseInfo.ref_count);
#endif

    return 0;
}


static HI_S32 hifb_dosetcolreg(unsigned regno, unsigned red, unsigned green,
                          unsigned blue, unsigned transp, struct fb_info *info, HI_BOOL bUpdateReg)
{
    HIFB_PAR_S *par = (HIFB_PAR_S *)info->par;
    //HI_U32 *pCmap;

    HI_U32 argb = ((transp & 0xff) << 24) | ((red & 0xff) << 16) | ((green & 0xff) << 8) | (blue & 0xff);

    if (regno > 255)
    {
        HIFB_WARNING("regno: %d, larger than 255!\n", regno);
        return HI_FAILURE;
    }

    s_stDrvOps.HIFB_DRV_SetColorReg(par->stBaseInfo.u32LayerID, regno, argb, bUpdateReg);
    return HI_SUCCESS;
}

static HI_S32 hifb_setcolreg(unsigned regno, unsigned red, unsigned green,
                          unsigned blue, unsigned transp, struct fb_info *info)
{
#if 1
	printk("hifb_setcolreg\n");
#else
    return hifb_dosetcolreg(regno, red, green, blue, transp, info, HI_TRUE);
#endif
}

static HI_S32 hifb_setcmap(struct fb_cmap *cmap, struct fb_info *info)
{
#if 1
	printk("hifb_setcmap\n");
#else
    HI_S32 i, start;
    unsigned short *red, *green, *blue, *transp;
    unsigned short hred, hgreen, hblue, htransp = 0xffff;
    HIFB_PAR_S *par = (HIFB_PAR_S *)info->par;

    if (par->stBaseInfo.u32LayerID == HIFB_LAYER_CURSOR)
    {
        return -EINVAL;
    }

/*
    if (!HIFB_IS_CLUTFMT(par->enColFmt))
    {
        HIFB_ERROR("failed to set color clut map! The layer is not clut format!\n");
        return -EINVAL;
    }
  */
  
    if (!g_pstCap[par->stBaseInfo.u32LayerID].bCmap)
    {
        /* AE6D03519, delete this color map warning! */
        HIFB_INFO("Layer%d is not support color map!\n", par->stBaseInfo.u32LayerID);
        return -EPERM;
    }

    red    = cmap->red;
    green  = cmap->green;
    blue   = cmap->blue;
    transp = cmap->transp;
    start  = cmap->start;

    for (i = 0; i < cmap->len; i++)
    {
        hred   = *red++;
        hgreen = *green++;
        hblue  = *blue++;
        if (transp)
        {
            htransp = *transp++;
        }

        if (i < cmap->len - 1)
        {
            if (hifb_dosetcolreg(start++, hred, hgreen, hblue, htransp, info, HI_FALSE))
            {
                break;
            }
        }
        else
        {   /*the last time update register*/
            if (hifb_dosetcolreg(start++, hred, hgreen, hblue, htransp, info, HI_TRUE))
            {
                break;
            }
        }

    }
#endif

    return 0;
}

#if 0
void hifb_fillrect(struct fb_info *p, const struct fb_fillrect *rect)
{
    //HIFB_PAR_S *par = (HIFB_PAR_S *)p->par;
    //par->fillcount++;
    printk("fun %s, line %d.\n",__FUNCTION__,__LINE__);
    cfb_fillrect(p, rect);
    printk("fun %s, line %d.\n",__FUNCTION__,__LINE__);
}
void hifb_copyarea(struct fb_info *p, const struct fb_copyarea *area)
{
    //HIFB_PAR_S *par = (HIFB_PAR_S *)p->par;
    //par->copyareacount++;
    printk("fun %s, line %d.\n",__FUNCTION__,__LINE__);
    cfb_copyarea(p, area);
    printk("fun %s, line %d.\n",__FUNCTION__,__LINE__);
}
void hifb_imageblit(struct fb_info *p, const struct fb_image *image)
{
    //HIFB_PAR_S *par = (HIFB_PAR_S *)p->par;
    //par->blitcount++;
    printk("fun %s, line %d.\n",__FUNCTION__,__LINE__);
    cfb_imageblit(p, image);
	printk("fun %s, line %d.\n",__FUNCTION__,__LINE__);
}
#endif

static struct fb_ops s_sthifbops =
{
    .owner			= THIS_MODULE,
    .fb_open		= hifb_open,
    .fb_release		= hifb_release,
    .fb_check_var	= hifb_check_var,
    .fb_set_par		= hifb_set_par,
    .fb_pan_display = hifb_pan_display,
    .fb_ioctl		= hifb_ioctl,
    .fb_setcolreg	= hifb_setcolreg,
    .fb_setcmap		= hifb_setcmap,
};

/******************************************************************************
 Function        : hifb_overlay_cleanup
 Description     : releae the resource for certain framebuffer
 Data Accessed   :
 Data Updated    :
 Output          : None
 Input           : HI_S32 which_layer
                   HI_S32 need_unregister
 Return          : static
 Others          : 0
******************************************************************************/
#ifdef MODULE
static __EXIT__ HI_VOID hifb_overlay_cleanup(HI_U32 u32LayerId, HI_BOOL bUnregister)
#else
static HI_VOID hifb_overlay_cleanup(HI_U32 u32LayerId, HI_BOOL bUnregister)
#endif
{
    struct fb_info* info = NULL;
    struct fb_cmap* cmap = NULL;

    /* get framebuffer info structure pointer */
    info = s_stLayer[u32LayerId].pstInfo;
    if (info != NULL)
    {
        cmap = &info->cmap;

        if (cmap->len != 0)
        {
            /* free color map */
            fb_dealloc_cmap(cmap);
        }

        if (info->screen_base != HI_NULL)
        {
            hifb_buf_ummap(info->screen_base);
        }

        if (info->fix.smem_start != 0)
        {
            hifb_buf_freemem(info->fix.smem_start);
        }
        
        if (bUnregister)
        {
            unregister_framebuffer(info);
        }
        
        s_stLayer[u32LayerId].pstInfo = NULL;
        
#ifdef CFG_HIFB_SCROLLTEXT_SUPPORT
		if (s_stTextLayer[u32LayerId].bAvailable)
		{
		    HI_U32 i;
			for (i = 0; i < SCROLLTEXT_NUM; i++)
			{
			    if (s_stTextLayer[u32LayerId].stScrollText[i].bAvailable)
			    {
			        hifb_freescrolltext_cachebuf(&(s_stTextLayer[u32LayerId].stScrollText[i]));
					memset(&s_stTextLayer[u32LayerId].stScrollText[i],0,sizeof(HIFB_SCROLLTEXT_S));
			    }
			}

			s_stTextLayer[u32LayerId].bAvailable = HI_FALSE;
			s_stTextLayer[u32LayerId].u32textnum = 0;
			s_stTextLayer[u32LayerId].u32ScrollTextId = 0;
		}
#endif        
    }
}

/******************************************************************************
 Function        : hifb_overlay_probe
 Description     : initialze the framebuffer for the overlay and set
                   the register .
 Data Accessed   :
 Data Updated    :
 Output          : None
 Input           : HI_S32 which_layer
                   unsigned long vram_size
 Return          : 0 if success; otherwise return error code
 Others          : 0
******************************************************************************/
static HI_S32 __INIT__ hifb_overlay_probe(HI_U32 u32LayerId, HI_U32 u32VramSize)
{
    HI_S32 s32Ret = 0;
    struct fb_info * info = NULL;
    HIFB_PAR_S *pstPar = NULL;

    /* Creates a new frame buffer info structure. reserves hifb_par for driver private data (info->par) */
    info = framebuffer_alloc(sizeof(HIFB_PAR_S), NULL);
    if (!info)
    {
        HIFB_ERROR("failed to malloc the fb_info!\n"); //5565
        return -ENOMEM;
    }

    info->pseudo_palette = ((HIFB_PAR_S *)(info->par))->Data_396;

    /* save the info pointer in global pointer array, otherwise the info will be lost in cleanup if the following code has error */
    s_stLayer[u32LayerId].pstInfo = info;
    
	snprintf(info->fix.id,sizeof(info->fix.id),"ovl%d", u32LayerId);
    info->flags = FBINFO_FLAG_DEFAULT | FBINFO_HWACCEL_YPAN | FBINFO_HWACCEL_XPAN;
    
    /* initialize file operations */
    info->fbops = &s_sthifbops;

    pstPar                         = (HIFB_PAR_S *)(info->par);
    pstPar->stBaseInfo.u32LayerID  = u32LayerId;
    pstPar->stDispInfo.stCanvasSur.u32PhyAddr = 0;

#if 0
    if (IS_HD_LAYER(u32LayerId))
    {
        info->fix = s_stDefFix[HIFB_LAYER_TYPE_HD];
        info->var = s_stDefVar[HIFB_LAYER_TYPE_HD];
    }
    else if (IS_SD_LAYER(u32LayerId))
    {
        info->fix = s_stDefFix[HIFB_LAYER_TYPE_SD];
        info->var = s_stDefVar[HIFB_LAYER_TYPE_SD];
    }
    else if (IS_AD_LAYER(u32LayerId))
    {
        info->fix = s_stDefFix[HIFB_LAYER_TYPE_AD];
        info->var = s_stDefVar[HIFB_LAYER_TYPE_AD];
    }
    else
    {
		return HI_FAILURE;
    }
#else
    if (u32LayerId == HIFB_LAYER_HD_0) //0
    {
        info->fix = s_stDefFix[HIFB_LAYER_TYPE_HD];
        info->var = s_stDefVar[HIFB_LAYER_TYPE_HD];
    }
    else if (u32LayerId == HIFB_LAYER_SD_0) //4
    {
        info->fix = s_stDefFix[HIFB_LAYER_TYPE_SD];
        info->var = s_stDefVar[HIFB_LAYER_TYPE_SD];
    }
    else if ((u32LayerId >= 1) && (u32LayerId <= 3))
    {
        info->fix = s_stDefFix[HIFB_LAYER_TYPE_AD];
        info->var = s_stDefVar[HIFB_LAYER_TYPE_AD];
    }
    else if ((u32LayerId >= 5) && (u32LayerId <= 7))
    {
        info->fix = s_stDefFix[HIFB_LAYER_TYPE_AD];
        info->var = s_stDefVar[HIFB_LAYER_TYPE_AD];
    }
    else
    {
		return HI_FAILURE;
    }
#endif
    
    /*it's not need to alloc mem for cursor layer*/
    if (u32VramSize != 0)
    {
        /*Modify 16 to 32, preventing out of bound.*/
        HI_CHAR name[32];
        /* initialize the fix screen info */
        
        snprintf(name, sizeof(name), "HIFB_Fb%d", u32LayerId);
        info->fix.smem_start = hifb_buf_allocmem(name, (u32VramSize)* 1024);
        if (0 == info->fix.smem_start)
        {
            HIFB_ERROR("%s:failed to malloc the video memory, size: %d KBtyes!\n", name, (u32VramSize)); //5615
            goto ERR;
        }
        else
        {
            info->fix.smem_len = u32VramSize * 1024;

            /* initialize the virtual address and clear memory */
            info->screen_base = hifb_buf_map(info->fix.smem_start);
            if (HI_NULL == info->screen_base)
            {
                HIFB_WARNING("Failed to call map video memory, "
                         "size:0x%x, start: 0x%lx\n",
                         info->fix.smem_len, info->fix.smem_start);
            }
            else
            {
                memset(info->screen_base, 0x00, info->fix.smem_len);
            }
        }

        /* alloc color map */
        if (g_pstCap[u32LayerId].bCmap)
        {
            if (fb_alloc_cmap(&info->cmap, 256, 1) < 0)
            {
                HIFB_WARNING("fb_alloc_cmap failed!\n"); //5641
            }
            else
            {
                info->cmap.len = 256;
            }
        }
    }

#if 0
    printk("info->count = %d\n", info->count);
    printk("info->node = %d\n", info->node);
    printk("info->flags = %d\n", info->flags);
    printk("info->var.xres = %d\n", info->var.xres);
    printk("info->var.yres = %d\n", info->var.yres);
    printk("info->var.xres_virtual = %d\n", info->var.xres_virtual);
    printk("info->var.yres_virtual = %d\n", info->var.yres_virtual);
    printk("info->var.xoffset = %d\n", info->var.xoffset);
    printk("info->var.yoffset = %d\n", info->var.yoffset);
    printk("info->var.bits_per_pixel = %d\n", info->var.bits_per_pixel);
    printk("info->var.grayscale = %d\n", info->var.grayscale);

    printk("info->fix.smem_start = %d\n", info->fix.smem_start);	/* Start of frame buffer mem */
    printk("info->fix.smem_len = %d\n", info->fix.smem_len);			/* Length of frame buffer mem */
    printk("info->fix.type = %d\n", info->fix.type);			/* see FB_TYPE_*		*/
    printk("info->fix.type_aux = %d\n", info->fix.type_aux);			/* Interleave for interleaved Planes */
    printk("info->fix.visual = %d\n", info->fix.visual);			/* see FB_VISUAL_*		*/
    printk("info->fix.xpanstep = %d\n", info->fix.xpanstep);			/* zero if no hardware panning  */
    printk("info->fix.ypanstep = %d\n", info->fix.ypanstep);			/* zero if no hardware panning  */
    printk("info->fix.ywrapstep = %d\n", info->fix.ywrapstep);		/* zero if no hardware ywrap    */
    printk("info->fix.line_length = %d\n", info->fix.line_length);		/* length of a line in bytes    */
    printk("info->fix.mmio_start = %d\n", info->fix.mmio_start);	/* Start of Memory Mapped I/O   */
    printk("info->fix.mmio_len = %d\n", info->fix.mmio_len);			/* Length of Memory Mapped I/O  */
    printk("info->fix.accel = %d\n", info->fix.accel);			/* Indicate to driver which	*/

    printk("info->screen_base = %d\n", info->screen_base);
    printk("info->screen_size = %d\n", info->screen_size);
#endif

    if ((s32Ret = register_framebuffer(info)) < 0)
    {
        HIFB_ERROR("failed to register_framebuffer!\n"); //5652
        s32Ret = -EINVAL;
        goto ERR;
    }

    HIFB_INFO("succeed in registering the fb%d: %s frame buffer device\n",
              info->node, info->fix.id); //5659

    return HI_SUCCESS;
    
ERR:
    hifb_overlay_cleanup(u32LayerId, HI_FALSE);
    
    return s32Ret;
}

/******************************************************************************
 Function        : hifb_get_vram_size
 Description     : parse the parameter string and get the size. if
                   the parameter is invalid, the size is default value.
 Data Accessed   :
 Data Updated    :
 Output          : None
 Input           : const char* pstr  the string for the vram size
 Return          : the video memory size
 Others          : 0
******************************************************************************/
static unsigned long hifb_get_vram_size(char* pstr)
{
    HI_S32 str_is_valid = HI_TRUE;
    unsigned long vram_size = 0;
    char* ptr = pstr;

    if ((ptr == NULL) || (*ptr == '\0'))
    {
        return 0;
    }

    /*check if the string is valid*/
    while (*ptr != '\0')
    {
        if (*ptr == ',')
        {
            //*ptr = '\0';
            break;
        }
        else if ((!isdigit(*ptr)) && ('X' != *ptr) && ('x' != *ptr)
            && ((*ptr > 'f' && *ptr <= 'z') || (*ptr > 'F' && *ptr <= 'Z')))
        {
            str_is_valid = HI_FALSE;
            break;
        }

        ptr++;
    }

    if (str_is_valid)
    {
        vram_size = simple_strtoul(pstr, (char **)NULL, 0);

        /*make the size PAGE_SIZE align*/
        vram_size = ((vram_size * 1024 + PAGE_SIZE - 1) & PAGE_MASK)/1024;
    }

    return vram_size;
}

static HI_S32 hifb_parse_cfg(HI_VOID)
{
    HI_CHAR *pscstr = NULL;
    HI_CHAR number[4] = {0};
    HI_U32 i = 0;
    HI_U32 u32LayerId;
    HI_U32 u32LayerSize;

    /* find the first 'varm' position in \arg video */
    //video += sizeof("hifb");

    /* get the string before next varm */
    pscstr = strstr(video, "vram");
        
    HIFB_INFO("video:%s\n", video); //5734

    while (pscstr != NULL)
    {
        /* parse the layer id and save it in a string */
        i = 0;
        /*skip "vram"*/
        pscstr += 4;
        while (*pscstr != '_')
        {
            /* i>1 means layer id is bigger than 100, it's obviously out of range!*/
            if (i > 1)
            {
                HIFB_ERROR("layer id is out of range!\n"); //5747
                return -1;
            }

            number[i] = *pscstr;
            i++;
            pscstr++;
        }

        number[i] = '\0';

        /* change the layer id string into digital and assure it's legal*/
        u32LayerId = simple_strtoul(number, (char **)NULL, 10);
        if (u32LayerId > HIFB_MAX_LAYER_ID)
        {
            HIFB_ERROR("layer id is out of range!\n"); //5762
            return HI_FAILURE;
        }

        if ((!g_pstCap[u32LayerId].bLayerSupported)
            &&(u32LayerId != HIFB_LAYER_CURSOR))
        {
            HIFB_ERROR("chip doesn't support layer %d!\n", u32LayerId); //5769
            return HI_FAILURE;
        }

        /* get the layer size string and change it to digital */
        pscstr += sizeof("size") + i;
        u32LayerSize = hifb_get_vram_size(pscstr);
        if ((u32LayerSize < s_stLayer[u32LayerId].u32LayerSize / 2) && (u32LayerId != HIFB_LAYER_CURSOR)
            && (u32LayerSize > 0))
        {
            u32LayerSize = s_stLayer[u32LayerId].u32LayerSize / 2;
        }

        s_stLayer[u32LayerId].u32LayerSize = u32LayerSize;

        //HIFB_ERROR("p:%s\n", pscstr);
        /* get next layer string */
        pscstr = strstr(pscstr, "vram");
        //HIFB_ERROR("p:%s\n", pscstr);
    }

    return 0;
}

#ifdef CFG_HIFB_PROC_SUPPORT
static const HI_CHAR* s_pszFmtName[] = {
    "RGB565",        
    "RGB888",		
    "KRGB444",       
    "KRGB555",       
    "KRGB888",       
    "ARGB4444",         
    "ARGB1555",      
    "ARGB8888",      
    "ARGB8565",
    "RGBA4444",
    "RGBA5551",
    "RGBA5658",
    "RGBA8888",
    "BGR565",
    "BGR888",
    "ABGR4444",
    "ABGR1555",
    "ABGR8888",
    "ABGR8565",
    "KBGR444",
    "KBGR555",
    "KBGR888",
    "1BPP",         
    "2BPP",         
    "4BPP",         
    "8BPP",
    "ACLUT44",
    "ACLUT88",
    "PUYVY",
    "PYUYV",
    "PYVYU",
    "YUV888",
    "AYUV8888",
    "YUVA8888",  
    "BUTT"};

const static HI_CHAR* s_pszLayerName[] = {"layer_hd_0", "layer_hd_1", "layer_hd_2", "layer_hd_3",
                                          "layer_sd_0", "layer_sd_1", "layer_sd_2", "layer_sd_3",
                                          "layer_ad_0", "layer_ad_1", "layer_ad_2", "layer_ad_3",
                                          "layer_cursor"};

HI_S32 hifb_print_layer_proc(struct fb_info * info, struct seq_file *p, HI_VOID *v)
{
	HI_U32 u32Stride;
    HIFB_PAR_S *par;
	HIFB_RECT   stOutputRect;
	HIFB_RECT   stDispRect;
    const HI_CHAR* pszBufMode[] = {"triple", "double ", "single", "triple( no dicard frame)", "standard", "unknow"};
    const HI_CHAR* pszAntiflicerLevel[] =  {"NONE", "LOW" , "MIDDLE", "HIGH", "AUTO" ,"ERROR"};
    const HI_CHAR* pszAntiMode[] =  {"NONE", "TDE" , "VOU" , "BUTT"};
    const HI_CHAR* pszStereoMode[] =  {"Mono", "Side by Side" , "Top and Bottom", "Frame packing", "unknow mode"}; 	    
    const HI_CHAR* pLayerName = NULL;

    par = (HIFB_PAR_S *)info->par; 

	if (par->stBaseInfo.u32LayerID >= HIFB_LAYER_ID_BUTT)
	{
		HIFB_ERROR("wrong layer id.\n");
		return HI_FAILURE;
	}
	
	s_stDrvOps.HIFB_DRV_GetLayerOutRect(par->stBaseInfo.u32LayerID, &stOutputRect);   
	s_stDrvOps.HIFB_DRV_GetDispSize(par->stBaseInfo.u32LayerID, &stDispRect);
    if (par->stBaseInfo.u32LayerID >= sizeof(s_pszLayerName)/sizeof(*s_pszLayerName))
    {
        pLayerName = "unknow layer";
    }
    else
    {
        pLayerName = s_pszLayerName[par->stBaseInfo.u32LayerID];
    }

    if (par->stBaseInfo.enAntiflickerMode > HIFB_ANTIFLICKER_BUTT)
    {
        par->stBaseInfo.enAntiflickerMode = HIFB_ANTIFLICKER_BUTT;
    }

    if (par->stBaseInfo.enAntiflickerLevel > HIFB_LAYER_ANTIFLICKER_BUTT)
    {
        par->stBaseInfo.enAntiflickerLevel = HIFB_LAYER_ANTIFLICKER_BUTT;
    }

	if (par->bSetStereoMode)
	{
		u32Stride = par->st3DInfo.st3DSurface.u32Pitch;
	}
	else
	{
		if ((par->stExtendInfo.enBufMode == HIFB_LAYER_BUF_NONE)
	         && par->stDispInfo.stUserBuffer.stCanvas.u32PhyAddr)
		{
			u32Stride = par->stDispInfo.stUserBuffer.stCanvas.u32Pitch;
		}
		else
		{
			u32Stride = info->fix.line_length;
		}
	}	 
    
	PROC_PRINT(p,  "LayerId                     \t :%s\n", pLayerName);
	PROC_PRINT(p,  "ShowState                   \t :%s\n", par->stExtendInfo.bShow ? "ON" : "OFF");	
	PROC_PRINT(p,  "ColorFormat:                \t :%s\n", s_pszFmtName[par->stExtendInfo.enColFmt]);
	PROC_PRINT(p,  "Stride                      \t :%d\n", u32Stride);
	PROC_PRINT(p,  "Offset                      \t :(%d, %d)\n", info->var.xoffset, info->var.yoffset);
	PROC_PRINT(p,  "Resolution(real/virtual/max)\t :(%d, %d)/(%d, %d)/(%d, %d)\n", info->var.xres, info->var.yres,
				info->var.xres_virtual, info->var.yres_virtual,stOutputRect.w,  stOutputRect.h);
	PROC_PRINT(p,  "Mem size:                   \t :%d KB\n\n",info->fix.smem_len / 1024);
	
	PROC_PRINT(p,  "StartPosition               \t :(%d, %d)\n", par->stExtendInfo.stPos.s32XPos, par->stExtendInfo.stPos.s32YPos);	
	PROC_PRINT(p,  "BufferMode                  \t :%s\n",pszBufMode[par->stExtendInfo.enBufMode]);
	PROC_PRINT(p,  "PixelAlpha                  \t :enable(%s), alpha0(0x%x), alpha1(0x%x)\n", 
				par->stExtendInfo.stAlpha.bAlphaEnable ? "true" : "false",
				par->stExtendInfo.stAlpha.u8Alpha0, par->stExtendInfo.stAlpha.u8Alpha1);	
	PROC_PRINT(p,  "Global Alpha                \t :0x%x\n", par->stExtendInfo.stAlpha.u8GlobalAlpha);
	PROC_PRINT(p,  "Colorkey                    \t :enable(%s), value(0x%x)\n", par->stExtendInfo.stCkey.bKeyEnable ? "true" : "false", 
				par->stExtendInfo.stCkey.u32Key);
	PROC_PRINT(p,  "Deflicker                   \t :enable(%s) mode(%s) level(%s)\n",par->stBaseInfo.bNeedAntiflicker ? "true" : "false",
					pszAntiMode[par->stBaseInfo.enAntiflickerMode], pszAntiflicerLevel[par->stBaseInfo.enAntiflickerLevel]);
	PROC_PRINT(p,  "3DMode                      \t :input(%s) output(%s)\n", pszStereoMode[par->st3DInfo.enInStereoMode], pszStereoMode[par->st3DInfo.enOutStereoMode]);
	PROC_PRINT(p,  "DisplayResolution           \t :(%d, %d)\n",par->stExtendInfo.u32DisplayWidth, par->stExtendInfo.u32DisplayHeight);		

	PROC_PRINT(p,  "CanavasAddr                 \t :0x%x\n",par->stDispInfo.stUserBuffer.stCanvas.u32PhyAddr);
	PROC_PRINT(p,  "CanavasUpdateRect           \t :(%d,%d,%d,%d) \n", par->stDispInfo.stUserBuffer.UpdateRect.x, par->stDispInfo.stUserBuffer.UpdateRect.y,
					par->stDispInfo.stUserBuffer.UpdateRect.w, par->stDispInfo.stUserBuffer.UpdateRect.h);
	PROC_PRINT(p,  "CanvasResolution            \t :(%d,%d)\n",par->stDispInfo.stCanvasSur.u32Width, par->stDispInfo.stCanvasSur.u32Height);
	PROC_PRINT(p,  "CanvasPitch                 \t :%d\n",par->stDispInfo.stCanvasSur.u32Pitch);
	PROC_PRINT(p,  "CanvasFormat                \t :%s\n",s_pszFmtName[par->stDispInfo.stCanvasSur.enFmt]);

	if (g_bProcDebug)
	{
		PROC_PRINT(p,  "\nReferecceCount              \t :%d\n", atomic_read(&par->stBaseInfo.ref_count));
		PROC_PRINT(p,  "DeviceMaxResolution         \t :%d, %d\n",stDispRect.w,	stDispRect.h);
		PROC_PRINT(p,  "DisplayingAddr(register)    \t :0x%x\n",par->stRunInfo.u32ScreenAddr);
		PROC_PRINT(p,  "DisplayBuf[0] addr          \t :0x%x\n",par->stDispInfo.u32DisplayAddr[0]);
		PROC_PRINT(p,  "DisplayBuf[1] addr          \t :0x%x\n",par->stDispInfo.u32DisplayAddr[1]);	  
		PROC_PRINT(p,  "IsNeedFlip(2buf)            \t :%s\n",par->stRunInfo.bNeedFlip? "YES" : "NO");
		PROC_PRINT(p,  "BufferIndexDisplaying(2buf) \t :%d\n",1-par->stRunInfo.u32IndexForInt);
		PROC_PRINT(p,  "UnionRect(2buf)             \t :(%d,%d,%d,%d)\n",par->stDispInfo.stUpdateRect.x, par->stDispInfo.stUpdateRect.y, par->stDispInfo.stUpdateRect.w, par->stDispInfo.stUpdateRect.h);
	}

    return 0; 
}

HI_S32 hifb_read_proc(struct seq_file *p, HI_VOID *v)
{
    DRV_PROC_ITEM_S * item = (DRV_PROC_ITEM_S *)(p->private);
    struct fb_info* info    = (struct fb_info *)(item->data);
    
 	return hifb_print_layer_proc(info, p, v);
}
extern HI_VOID hifb_captureimage_fromdevice(HI_U32 u32LayerID);

static HI_VOID hifb_parse_proccmd(struct seq_file* p, HI_U32 u32LayerId, HI_CHAR *pCmd)
{
    struct fb_info *info = s_stLayer[u32LayerId].pstInfo;
    HIFB_PAR_S *pstPar = (HIFB_PAR_S *)info->par;
    HI_S32 cnt = atomic_read(&pstPar->stBaseInfo.ref_count);

    
    if (strncmp("show", pCmd, 4) == 0)
    {
        if (cnt == 0)
        {
            HIFB_INFO("err:device no open!\n");
            return;
        }

        if (pstPar->stBaseInfo.u32LayerID == HIFB_LAYER_CURSOR)
        {
            HIFB_INFO("cursor layer doesn't support this cmd!\n");
            return;
        }
        
        if (!pstPar->stExtendInfo.bShow)
        {
            pstPar->stRunInfo.bModifying = HI_TRUE;
            pstPar->stExtendInfo.bShow = HI_TRUE;
            pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_SHOW;
            pstPar->stRunInfo.bModifying = HI_FALSE;
        }
    }     
    else if (strncmp("hide", pCmd, 4) == 0)
    {
        if (cnt == 0)
        {
            HIFB_INFO("err:device not open!\n");
            return;
        }

        if (pstPar->stBaseInfo.u32LayerID == HIFB_LAYER_CURSOR)
        {
            PROC_PRINT(p, "cursor layer doesn't support this cmd!\n");
            return;
        }
        
        if (pstPar->stExtendInfo.bShow)
        {
            pstPar->stRunInfo.bModifying = HI_TRUE;
            pstPar->stExtendInfo.bShow = HI_FALSE;
            pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_SHOW;
            pstPar->stRunInfo.bModifying = HI_FALSE;
        }
    }
    else if (strncmp("help", pCmd, 4) == 0)
    {
        HI_PRINT("help info:\n");
        HI_PRINT("echo cmd > proc file\n");
        HI_PRINT("hifb support cmd:\n");
        HI_PRINT("show:show layer\n");
        HI_PRINT("hide:hide layer\n");
		HI_PRINT("alpha=255:set layer's global alpha\n");
		HI_PRINT("capture:capture image from frame buffer\n");
		HI_PRINT("debug on :debug on\n");
		HI_PRINT("debug off:debug off\n");
        HI_PRINT("For example, if you want to hide layer 1,you can input:\n");   
        HI_PRINT("echo hide > /proc/msp/hifb1\n");   
    }
	else if (strncmp("debug on", pCmd, 8) == 0)
    {
		g_bProcDebug = HI_TRUE;
		HI_PRINT("set proc debug on.\n");
    }
	else if (strncmp("debug off", pCmd, 9) == 0)
    {
		g_bProcDebug = HI_FALSE;
		HI_PRINT("set proc debug off.\n");
    }
	else if (strncmp("alpha", pCmd, 5) == 0)
	{
		HI_U32  u32Alpha;
		HI_CHAR TmpCmd[HIFB_FILE_NAME_MAX_LEN]={0};
		HI_BOOL bIsStrValid = HI_FALSE;
		HI_CHAR *pStr = HI_NULL;
		HI_CHAR *pStrTmp = HI_NULL;

		strncpy(TmpCmd,pCmd,(HIFB_FILE_NAME_MAX_LEN-1));
		TmpCmd[HIFB_FILE_NAME_MAX_LEN-1] = '\0';
		
		pStr = strstr(TmpCmd, "=");

		if (HI_NULL == pStr)
		{
			return;
		}
		
		pStr++;

		while(pStr != '\0')
		{
			if (HI_FALSE == bIsStrValid)
			{
				if (isdigit(*pStr) || ('X' == *pStr) || ('x' == *pStr)
					|| (*pStr >= 'a' && *pStr <= 'f') || (*pStr >= 'A' && *pStr <= 'F'))
				{
					bIsStrValid = HI_TRUE;
					pStrTmp = pStr;
				}
			}
			else
			{
				if ((!isdigit(*pStr)) && ('X' != *pStr) && ('x' != *pStr)
						&& (!(*pStr >= 'a' && *pStr <= 'f') && !(*pStr >= 'A' && *pStr <= 'F')))
				{
					*pStr = '\0';
					break;
				}
			}
			
			pStr++;
		}

		if (!bIsStrValid)
		{
			HIFB_ERROR("cmd is invalid\n");
			return;
		}

		u32Alpha = simple_strtoul(pStrTmp, (char **)NULL, 0);

		if (u32Alpha > 255)
		{
			u32Alpha = 255;
		}

	   	pstPar->stExtendInfo.stAlpha.bAlphaChannel = HI_TRUE;
        pstPar->stExtendInfo.stAlpha.u8GlobalAlpha = u32Alpha;

        s_stDrvOps.HIFB_DRV_SetLayerAlpha(pstPar->stBaseInfo.u32LayerID, &pstPar->stExtendInfo.stAlpha);
		HI_PRINT("set gfx global alpha 0x%x.\n", u32Alpha);
	}
	else if (strncmp("capture", pCmd, 7) == 0)
	{
		HI_S32 cnt; 
		cnt = atomic_read(&pstPar->stBaseInfo.ref_count);
		if (cnt < 1)
		{
			HIFB_ERROR("Unsupported to capture a closed layer.\n");
			return;
		}

		hifb_captureimage_fromdevice(pstPar->stBaseInfo.u32LayerID);
	}
    else
    {
        HIFB_ERROR("unsupported cmd:%s ", pCmd);
        HIFB_ERROR("you can use help cmd to show help info!\n");
    }

    return;
}

HI_S32 hifb_write_proc(struct file * file,
    const char __user * buf, size_t count, loff_t *ppos)
{
    struct fb_info *info;
    HIFB_PAR_S *pstPar;
    HI_CHAR buffer[128];
    
    struct seq_file *seq = file->private_data;
    DRV_PROC_ITEM_S *item = seq->private;
    info = (struct fb_info *)(item->data);
    pstPar = (HIFB_PAR_S *)(info->par);

    
    if (count > sizeof(buffer))
    {
        HIFB_ERROR("The command string is out of buf space :%d bytes !\n", sizeof(buffer));
        return 0;
    }

    if (copy_from_user(buffer, buf, count))
    {
        HIFB_ERROR("failed to call copy_from_user !\n");
        return 0;
    }
    
    hifb_parse_proccmd(seq, pstPar->stBaseInfo.u32LayerID, (HI_CHAR*)buffer);

    return count;
}
#endif
HI_VOID HIFB_DRV_ModExit(HI_VOID);


/******************************************************************************
 Function        : HIFB_DRV_ModInit
 Description     : initialize framebuffer. the function is called when
                   loading the moudel
 Data Accessed   :
 Data Updated    :
 Output          : None
 Input           : HI_VOID
 Return          : 0, if success; otherwise, return error code
 Others          : 0
******************************************************************************/

#define FBNAME "HI_FB"
extern HI_S32 hifb_init_module_k(HI_VOID);

HI_S32 HIFB_DRV_ModInit(HI_VOID)
{
    HI_U32 i = 0;    
    HI_S32 s32Ret;

    memset(&s_stLayer, 0x00, sizeof(s_stLayer));

	HIFB_DRV_GetDevOps(&s_stDrvOps);
	HIFB_DRV_GetTdeOps(&s_stDrvTdeOps);
	
    /* inital adoption layer */
    if (HI_SUCCESS != s_stDrvOps.HIFB_DRV_GfxInit())
    {
        HIFB_ERROR("drv init failed\n"); //6269
        goto ERR;
    }

#ifdef CFG_HIFB_LOGO_SUPPORT
    s32Ret = hifb_logo_init();
    if (s32Ret != HI_SUCCESS)
    {
        HIFB_ERROR("hifb logo init failed\n"); //6277
    }        
#endif

	if (s_stDrvOps.HIFB_DRV_GetGFXCap(&g_pstCap) < 0)
	{
		HIFB_ERROR("Gfx get device capability failed!\n"); //6283
		return HI_FAILURE;
	}

    if (!strncmp("on", tc_wbc, 2))
    {
    	s_stDrvOps.HIFB_DRV_SetTCFlag(1);
    }

    /* parse the \arg video string */
    if (hifb_parse_cfg() < 0)
    {
        /* hint info */
        HIFB_INFO("Usage:insmod hifb.ko video=\"hifb:vrami_size:xxx,vramj_size:xxx,...\"\n"); //6296
        HIFB_INFO("i,j means layer id, xxx means layer size in kbytes!\n"); //6297
        HIFB_INFO("example:insmod hifb.ko video=\"hifb:vram0_size:810,vram1_size:810\"\n\n"); //6298
        return HI_FAILURE;
    }

    /*inital fb file according the config*/
    for(i = 0; i <= 5/*HIFB_MAX_LAYER_ID*/; i++)
    {
        /*if hw not support, we modify memory to 0, so in the path of /dev/ there is only one device name */
        if (i== HIFB_LAYER_CURSOR/* || 
            !g_pstCap[i].bLayerSupported*/)
        {
            s_stLayer[i].u32LayerSize = 0;
        }
#if 0		
		else if (!g_pstCap[i].bLayerSupported)
        {
            continue;
        }
#endif
		if (!strcmp("", video))
		{
			s_stLayer[i].u32LayerSize = g_u32LayerSize[i];
		}
        
        /* register the layer */
        if (hifb_overlay_probe(i, s_stLayer[i].u32LayerSize) == HI_SUCCESS)
        {
#ifdef CFG_HIFB_PROC_SUPPORT 
			HI_CHAR entry_name[16];
            GFX_PROC_ITEM_S item;

            /* create a proc entry in 'hifb' for the layer */
            snprintf(entry_name, sizeof(entry_name), "hifb%d", i);
			item.fnRead = hifb_read_proc;
			item.fnWrite= hifb_write_proc;
			item.fnIoctl= HI_NULL;
            HI_GFX_PROC_AddModule(entry_name, &item, (HI_VOID *)s_stLayer[i].pstInfo);
#endif			
        }
        else
        {
        	return HI_FAILURE;
        }
#ifdef CFG_HIFB_SCROLLTEXT_SUPPORT
		memset(&s_stTextLayer[i], 0, sizeof(HIFB_SCROLLTEXT_INFO_S));
#endif
    }
 
    HIFB_INFO("layersize hifb0:%d, hifb1:%d, hifb2:%d, hifb3:%d, hifb4:%d, hifb5:%d, hifb_cursor:%d\n",
        s_stLayer[HIFB_LAYER_HD_0].u32LayerSize, s_stLayer[HIFB_LAYER_HD_1].u32LayerSize,
        s_stLayer[HIFB_LAYER_HD_2].u32LayerSize, s_stLayer[HIFB_LAYER_HD_3].u32LayerSize,
        s_stLayer[HIFB_LAYER_SD_0].u32LayerSize, s_stLayer[HIFB_LAYER_SD_1].u32LayerSize,
        s_stLayer[HIFB_LAYER_CURSOR].u32LayerSize); //6337

    s_stDrvTdeOps.HIFB_DRV_SetTdeCallBack(hifb_tde_callback);

#if 0
#ifndef HI_MCE_SUPPORT	
    hifb_init_module_k();
#endif
#endif

	/* show version */
    hifb_version();   
    
    return 0;
    
ERR:
    
    for (i = 0; i < 6/*HIFB_LAYER_ID_BUTT*/; i++)
    {
        hifb_overlay_cleanup(i, HI_TRUE);
    }

    return HI_FAILURE;
}

/******************************************************************************
 Function        : HIFB_DRV_ModExit
 Description     : cleanup the resource when exiting the framebuffer
                   module
 Data Accessed   :
 Data Updated    :
 Output          : None
 Input           : HI_VOID
 Return          : static
 Others          : 0
******************************************************************************/

HI_VOID HIFB_DRV_ModExit(HI_VOID)
{
    HI_S32 i;
	HI_GFX_MODULE_UnRegister(HIGFX_FB_ID);

    s_stDrvTdeOps.HIFB_DRV_SetTdeCallBack(NULL);

#if 0
#ifdef CFG_HIFB_LOGO_SUPPORT
    if (g_bLogoShow)
    {
        hifb_clear_logo();
    }
#endif
#endif

    s_stDrvOps.HIFB_DRV_GfxDeInit();

    for (i = 0; i < 6/*HIFB_LAYER_ID_BUTT*/; i++)
    {
#ifdef CFG_HIFB_PROC_SUPPORT
        HI_CHAR entry_name[16];
        /* create a proc entry in 'hifb' for the layer */
        snprintf(entry_name, sizeof(entry_name), "hifb%d", i);            
        HI_GFX_PROC_RemoveModule(entry_name);
#endif

#ifdef CFG_HIFB_LOGO_SUPPORT
        hifb_clear_logo(i);
#endif

        hifb_overlay_cleanup(i, HI_TRUE);
#ifdef CFG_HIFB_COMPRESSION_SUPPORT
        s_stDrvOps.HIFB_DRV_EnableCompression(i, HI_FALSE);
#endif
    }

	s_stDrvTdeOps.HIFB_DRV_TdeClose();

}

#ifdef MODULE
module_init(HIFB_DRV_ModInit);
module_exit(HIFB_DRV_ModExit);
MODULE_LICENSE("GPL");
#endif
