/** 
 * \file
 * \brief Describes the information about HiSilicion frame buffer (HiFB). CNcomment:�ṩHIBF�������ϢCNend
 */
 
#ifndef __HIFB_H__
#define __HIFB_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */



#include <linux/fb.h>

#include "hi_type.h"
#include "hi_common.h"



/*************************** Macro Definition ****************************/
/** \addtogroup           HIFB   */
/** @{ */  /** <!-- [HIFB] */
#define IOC_TYPE_HIFB       'F'

/**Obtains the colorkey of a graphics layer.*/
/**CNcomment:��ȡͼ��colorkey��Ϣ */
#define FBIOGET_COLORKEY_HIFB       _IOR(IOC_TYPE_HIFB, 90, HIFB_COLORKEY_S)

/**Sets the colorkey of a graphics layer.*/
/**CNcomment:����ͼ��colorkey��Ϣ */
#define FBIOPUT_COLORKEY_HIFB       _IOW(IOC_TYPE_HIFB, 91, HIFB_COLORKEY_S)

/**Obtains the alpha values of a graphics layer, including the pixel alpha and global alpha.*/
/**CNcomment:��ȡͼ��alpha��Ϣ,�������غ�ȫ��alpha */
#define FBIOGET_ALPHA_HIFB          _IOR(IOC_TYPE_HIFB, 92, HIFB_ALPHA_S)

/**Sets the alpha values of a graphics layer, including the pixel alpha and global alpha.*/
/**CNcomment:����ͼ��alpha��Ϣ,�������غ�ȫ��alpha */
#define FBIOPUT_ALPHA_HIFB          _IOW(IOC_TYPE_HIFB, 93, HIFB_ALPHA_S)

/**Obtains the origin of a graphics layer.*/
/**CNcomment:��ȡͼ����Ļԭ����Ϣ */
#define FBIOGET_SCREEN_ORIGIN_HIFB  _IOR(IOC_TYPE_HIFB, 94, HIFB_POINT_S)

/**Sets the origin of a graphics layer.*/
/**CNcomment:����ͼ����Ļԭ����Ϣ */
#define FBIOPUT_SCREEN_ORIGIN_HIFB  _IOW(IOC_TYPE_HIFB, 95, HIFB_POINT_S)

/**Obtains the anti-flicker status of a graphics layer.*/
/**CNcomment:��ȡͼ�㿹��˸*/
#define FBIOGET_DEFLICKER_HIFB       _IOR(IOC_TYPE_HIFB, 98, HIFB_DEFLICKER_S)

/**Sets the anti-flicker status of a graphics layer.*/
/**CNcomment:����ͼ�㿹��˸ */
#define FBIOPUT_DEFLICKER_HIFB       _IOW(IOC_TYPE_HIFB, 99, HIFB_DEFLICKER_S)

/**Waits for the vertical blanking interrupt of a graphics layer.*/
/**CNcomment:�ȴ�ͼ�㴹ֱˮ���ж� */
#define FBIOGET_VBLANK_HIFB         _IO(IOC_TYPE_HIFB, 100)

/**Sets to display a graphics layer.*/
/**CNcomment:����ͼ����ʾ */
#define FBIOPUT_SHOW_HIFB           _IOW(IOC_TYPE_HIFB, 101, HI_BOOL)

/**Obtains the display information about a graphics layer.*/
/**CNcomment:��ȡͼ����ʾ��ʾ��Ϣ */
#define FBIOGET_SHOW_HIFB           _IOR(IOC_TYPE_HIFB, 102, HI_BOOL)

/**Obtains the capability of a graphics layer.*/
/**CNcomment:��ȡͼ������ */
#define FBIOGET_CAPABILITY_HIFB    _IOR(IOC_TYPE_HIFB, 103, HIFB_CAPABILITY_S)

/* crusor handle */
/* Attention:surface in cursor will be released by user*/




/** cursor will be separated from attached layer automatically if you attach cursor to another layer,that means
cursor can be attached to only one layer at any time*/


/**Sets the information about a graphics layer.*/
/**CNcomment:����ͼ����Ϣ */
#define FBIOPUT_LAYER_INFO                _IOW(IOC_TYPE_HIFB, 120, HIFB_LAYER_INFO_S*)

/**Obtains the information about a graphics layer.*/
/**CNcomment:��ȡͼ����Ϣ */
#define FBIOGET_LAYER_INFO                _IOR(IOC_TYPE_HIFB, 121, HIFB_LAYER_INFO_S*)

/**Obtains a canvas buffer.*/
/**CNcomment:��ȡcanvas buf */
#define FBIOGET_CANVAS_BUFFER             _IOR(IOC_TYPE_HIFB, 123, HIFB_BUFFER_S*) 

/**Refreshes a graphics layer.*/
/**CNcomment:ˢ��ͼ�� */
#define FBIO_REFRESH                      _IOW(IOC_TYPE_HIFB, 124, HIFB_BUFFER_S*) 

/**sync refresh*/
#define FBIO_WAITFOR_FREFRESH_DONE        _IO(IOC_TYPE_HIFB, 125)

/** set Encoder picture delivery method*/
#define FBIOPUT_ENCODER_PICTURE_FRAMING   _IOW(IOC_TYPE_HIFB, 126, HIFB_STEREO_MODE_E*)
/** get Encoder picture delivery method */
#define FBIOGET_ENCODER_PICTURE_FRAMING   _IOR(IOC_TYPE_HIFB, 127, HIFB_STEREO_MODE_E*)

/*set 3D Stereo work mode*/
#define FBIOPUT_STEREO_MODE _IOW(IOC_TYPE_HIFB, 128, HIFB_STEREO_WORKMODE_E*)
/*get 3D Stereo work mode*/
#define FBIOGET_STEREO_MODE _IOW(IOC_TYPE_HIFB, 129, HIFB_STEREO_WORKMODE_E*)


/**Sets the output range of the screen.*/
/**CNcomment:������Ļ�����Χ*/
#define FBIOPUT_SCREENSIZE          _IOW(IOC_TYPE_HIFB, 130, HIFB_SIZE_S*)

/**Obtains the output range of the screen.*/
/**CNcomment:��ȡ��Ļ�����Χ*/
#define FBIOGET_SCREENSIZE          _IOR(IOC_TYPE_HIFB, 131, HIFB_SIZE_S*)

/*set compression mode, only support pixel format argb8888, and stereo mode not support top and bottom*/
#define FBIOPUT_COMPRESSION  _IOW(IOC_TYPE_HIFB, 132, HI_BOOL*)
/*get compression mode */
#define FBIOGET_COMPRESSION  _IOR(IOC_TYPE_HIFB, 133, HI_BOOL*)

/**create scroll text*/
#define FBIO_SCROLLTEXT_CREATE   _IOW(IOC_TYPE_HIFB, 134, HIFB_SCROLLTEXT_CREATE_S*) 
/**send to scroll text buffer*/
#define FBIO_SCROLLTEXT_FILL     _IOW(IOC_TYPE_HIFB, 135, HIFB_SCROLLTEXT_DATA_S*) 
/**pause scroll text*/
#define FBIO_SCROLLTEXT_PAUSE    _IOW(IOC_TYPE_HIFB, 136, HI_U32*) 
/**resume scroll text*/
#define FBIO_SCROLLTEXT_RESUME   _IOW(IOC_TYPE_HIFB, 137, HI_U32*) 
/**destory scroll text*/
#define FBIO_SCROLLTEXT_DESTORY  _IOW(IOC_TYPE_HIFB, 138, HI_U32*)

/*set fb stereo depth*/
#define FBIOPUT_STEREO_DEPTH  _IOW(IOC_TYPE_HIFB, 139, HI_S32*)
/*get fb stereo depth*/
#define FBIOGET_STEREO_DEPTH  _IOR(IOC_TYPE_HIFB, 140, HI_S32*)

/**set the priority of layer in gp*/
#define FBIOPUT_ZORDER          _IOW(IOC_TYPE_HIFB, 141, HIFB_ZORDER_E *)
/**gett the priority of layer in gp*/
/**default setting:
	HD0: G0(hifb0) ---- zorder == 1  -->bottom
	HD0: G1(hifb1) ---- zorder == 2
	HD0: G2(hifb2) ---- zorder == 3 -->top
	--------------------------
	HD1: G4(hifb4) ---- zorder == 1*/
#define FBIOGET_ZORDER          _IOW(IOC_TYPE_HIFB, 142, HI_U32 *)

/**free logo*/
#define FBIO_FREE_LOGO          _IO(IOC_TYPE_HIFB, 143)

/** @} */  /** <!-- ==== Macro Definition end ==== */



/*************************** Structure Definition ****************************/
/** \addtogroup           HIFB   */
/** @{ */  /** <!-- [HIFB] */


/**Definition of the range type*/
/**CNcomment:��Χ���Ͷ���*/
typedef struct
{
    HI_U32  u32Width;         /**<Output width of the screen*//**<CNcomment:��Ļ������Ŀ�� */
    HI_U32  u32Height;        /**<Output height of the screen*//**<CNcomment:��Ļ������ĸ߶� */
}HIFB_SIZE_S;

/**layer ID */
/**CNcomment:ͼ��ID����*/
typedef enum 
{
    HIFB_LAYER_HD_0 = 0x0,
    HIFB_LAYER_HD_1,
    HIFB_LAYER_HD_2,
    HIFB_LAYER_HD_3,
    HIFB_LAYER_SD_0, //4
    HIFB_LAYER_SD_1,    
    HIFB_LAYER_SD_2,     
    HIFB_LAYER_SD_3, 
    HIFB_LAYER_AD_0,  
    HIFB_LAYER_AD_1,
    HIFB_LAYER_AD_2,
    HIFB_LAYER_AD_3,
    HIFB_LAYER_CURSOR,
    HIFB_LAYER_ID_BUTT //13
} HIFB_LAYER_ID_E;


/**Obtains the extended 32-bit color value based on color components. The upper bits are padded with lower bits during color extension.*/
/**CNcomment:�����ɫ��������ȡ����չ��32λ��ֵ����չ����:��λ�����λ*/
static inline HI_U8  hifb_rgb(const struct fb_bitfield* pBit, HI_S32 color)
{
    return ((HI_U8)((((HI_U32)color)>>pBit->offset) << (8-pBit->length)) +
             ((HI_U8)(((HI_U32)(color)>>pBit->offset) << (8-pBit->length)) >> pBit->length));
}

/**Converts a colorkey to a 32-bit colorkey based on the pixel format. A lower-bit color (such as 16 bits) is extended to an upper-bit color (such as 32 bits) as follows:
   For each chip, upper bits can be padded with lower bits in multiple mode. The padding mode of the chip must be consistent with the preceding extension mode. Otherwise, the colorkey of the graphics layer is invalid.*/
/**CNcomment:������ظ�ʽ��Ϣ����һ����ɫֵ��ת����һ��32λ����ɫkeyֵ,����ɫ����16bit��չΪ��ɫ����32bit�Ĳ�����:
   ��λ���߼�λ��оƬ�ж��ַ�ʽ��оƬ�ķ�ʽ�����������ʽһ�£�����ͼ��colorkey��Ч*/
static inline HI_S32 hifb_color2key(const struct fb_var_screeninfo* pVar, HI_S32 color)
{
   if (pVar->bits_per_pixel <= 8)
   {
       return color; 
   }
   else
   {
      HI_U8 r, g, b;
      r = hifb_rgb(&pVar->red, color);
      g = hifb_rgb(&pVar->green, color);      
      b = hifb_rgb(&pVar->blue, color);
      return (r<<16) + (g<<8) + b;
   }
}


typedef struct
{
    HI_BOOL bKeyEnable;         /**<Colorkey enable*//**<CNcomment:colorkey �Ƿ�ʹ�� */
    HI_U32 u32Key;              /**<The value is obtained by calling ::hifb_color2key.*//**<CNcomment:��ֵӦ��ͨ��::hifb_color2key�õ�*/
}HIFB_COLORKEY_S;

/**Rectangle information*/
/**CNcomment:������Ϣ */
typedef struct
{
    HI_S32 x, y;    /**<x: horizontal coordinate of the upper left point of the rectangle; y: vertical coordinate of the upper left point of the rectangle*//**<CNcomment: x:�������ϵ����� y:�������ϵ������*/
    HI_S32 w, h;    /**< w: rectangle width; h: rectangle height*//**<CNcomment: w:���ο�� h:���θ߶�*/
} HIFB_RECT;

typedef struct
{
    HI_S32 l;
    HI_S32 t;
    HI_S32 r;
    HI_S32 b;
}HIFB_REGION;

/**Coordinate of a point*/
/**CNcomment:�������Ϣ */
typedef struct
{
    HI_S32 s32XPos;         /**<  horizontal position *//**<CNcomment:ˮƽλ�� */
    HI_S32 s32YPos;         /**<  vertical position *//**<CNcomment:��ֱλ�� */
}HIFB_POINT_S;

/**Anti-flicker information*/
/**CNcomment:����˸��Ϣ */
typedef struct hiHIFB_DEFLICKER_S
{
    HI_U32  u32HDfLevel;    /**<  horizontal deflicker level *//**<CNcomment:ˮƽ����˸���� */     
    HI_U32  u32VDfLevel;    /**<  vertical deflicker level *//**<CNcomment:��ֱ����˸���� */
    HI_U8   *pu8HDfCoef;    /**<  horizontal deflicker coefficient *//**<CNcomment:ˮƽ����˸ϵ�� */
    HI_U8   *pu8VDfCoef;    /**<  vertical deflicker coefficient *//**<CNcomment:��ֱ����˸ϵ�� */
}HIFB_DEFLICKER_S;

/**Alpha information*/
/**CNcomment:Alpha��Ϣ */
typedef struct
{
    HI_BOOL bAlphaEnable;   /**<  alpha enable flag *//**<CNcomment:alphaʹ�ܱ�ʶ*/
    HI_BOOL bAlphaChannel;  /**<  alpha channel enable flag *//**<CNcomment:alphaͨ��ʹ��*/
    HI_U8 u8Alpha0;         /**<  alpha0 value, used in ARGB1555 *//**CNcomment:alpha0ȡֵ��ARGB1555��ʽʱʹ��*/
    HI_U8 u8Alpha1;         /**<  alpha1 value, used in ARGB1555 *//**CNcomment:alpha1ȡֵ��ARGB1555��ʽʱʹ��*/
    HI_U8 u8GlobalAlpha;    /**<  global alpha value *//**<CNcomment:ȫ��alphaȡֵ*/
    HI_U8 u8Reserved;
}HIFB_ALPHA_S;

/**Pixel format*/
/**CNcomment:���ظ�ʽö�� */
typedef enum
{        
    HIFB_FMT_RGB565 = 0,        
    HIFB_FMT_RGB888,		      /**<  RGB888 24bpp */

    HIFB_FMT_KRGB444,       /**<  RGB444 16bpp */
    HIFB_FMT_KRGB555,       /**<  RGB555 16bpp */
    HIFB_FMT_KRGB888,       /**<  RGB888 32bpp */

    HIFB_FMT_ARGB4444,      /**< ARGB4444 */     
    HIFB_FMT_ARGB1555,      /**< ARGB1555 */  
    HIFB_FMT_ARGB8888,      /**< ARGB8888 */   
    HIFB_FMT_ARGB8565,      /**< ARGB8565 */

    HIFB_FMT_RGBA4444,      /**< ARGB4444 */
    HIFB_FMT_RGBA5551,      /**< RGBA5551 */
    HIFB_FMT_RGBA5658,      /**< RGBA5658 */
    HIFB_FMT_RGBA8888,      /**< RGBA8888 */

    HIFB_FMT_BGR565,        /**< BGR565 */   
    HIFB_FMT_BGR888,        /**< BGR888 */   
    HIFB_FMT_ABGR4444,      /**< ABGR4444 */   
    HIFB_FMT_ABGR1555,      /**< ABGR1555 */   
    HIFB_FMT_ABGR8888,      /**< ABGR8888 */   
    HIFB_FMT_ABGR8565,      /**< ABGR8565 */      
    HIFB_FMT_KBGR444,       /**< BGR444 16bpp */
    HIFB_FMT_KBGR555,       /**< BGR555 16bpp */
    HIFB_FMT_KBGR888,       /**< BGR888 32bpp */

    HIFB_FMT_1BPP,          /**<  clut1 */
    HIFB_FMT_2BPP,          /**<  clut2 */    
    HIFB_FMT_4BPP,          /**<  clut4 */ 
    HIFB_FMT_8BPP,          /**< clut8 */
    HIFB_FMT_ACLUT44,       /**< AClUT44*/
    HIFB_FMT_ACLUT88,         /**< ACLUT88 */
    HIFB_FMT_PUYVY,         /**< UYVY */
    HIFB_FMT_PYUYV,         /**< YUYV */
    HIFB_FMT_PYVYU,         /**< YVYU */
    HIFB_FMT_YUV888,        /**< YUV888 */
    HIFB_FMT_AYUV8888, //32     /**< AYUV8888 */
    HIFB_FMT_YUVA8888,      /**< YUVA8888 */
    HIFB_FMT_BUTT //34
}HIFB_COLOR_FMT_E;

/**HiFB capability set*/
/**CNcomment:hifb������ */
typedef struct
{
    HI_BOOL bKeyAlpha;      /**< whether support colorkey alpha */
    HI_BOOL bGlobalAlpha;   /**< whether support global alpha */
    HI_BOOL bCmap;          /**< whether support color map */
    HI_BOOL bHasCmapReg; //12   /**< whether has color map register*/
    HI_BOOL bColFmt[HIFB_FMT_BUTT]; /**< support which color format */
    HI_BOOL bVoScale;       /**< support vo scale*/
    HI_BOOL bLayerSupported; //156   /**< whether support a certain layer, for example:x5 HD support HIFB_SD_0 not support HIFB_SD_1*/
    HI_BOOL bCompression;    /**< whether support compression */
    HI_BOOL bStereo;        /**< whether support 3D Stereo*/
    HI_U32  u32MaxWidth;    /**< the max pixels per line */
    HI_U32  u32MaxHeight;   /**< the max lines */
    HI_U32  u32MinWidth;    /**< the min pixels per line */
    HI_U32  u32MinHeight;   /**< the min lines */ 
    HI_U32  u32VDefLevel;   /**< vertical deflicker level, 0 means vertical deflicker is unsupported */
    HI_U32  u32HDefLevel;   /**< horizontal deflicker level, 0 means horizontal deflicker is unsupported  */
}HIFB_CAPABILITY_S;

/*refresh mode*/
typedef enum 
{
    HIFB_LAYER_BUF_DOUBLE = 0x0,         /**< 2 display buf in fb */
    HIFB_LAYER_BUF_ONE    = 0x1,         /**< 1 display buf in fb */
    HIFB_LAYER_BUF_NONE   = 0x2,         /**< no display buf in fb,the buf user refreshed will be directly set to VO*/    
    HIFB_LAYER_BUF_DOUBLE_IMMEDIATE=0x3, /**< 2 display buf in fb, each refresh will be displayed*/
    HIFB_LAYER_BUF_STANDARD = 0x4,       /**< standard refresh*/
    HIFB_LAYER_BUF_BUTT
} HIFB_LAYER_BUF_E;

/* surface info */
typedef struct
{
    HI_U32  u32PhyAddr;     /**<  start physical address */
    HI_U32  u32Width;       /**<  width pixels */
    HI_U32  u32Height;      /**<  height pixels */   
    HI_U32  u32Pitch;       /**<  line pixels */         
    HIFB_COLOR_FMT_E enFmt; /**<  color format */     
}HIFB_SURFACE_S;


/* refresh surface info */
typedef struct
{
    HIFB_SURFACE_S stCanvas;    
    HIFB_RECT UpdateRect;       /**< refresh region*/
}HIFB_BUFFER_S;

/* cursor info */
typedef struct
{
    HIFB_SURFACE_S stCursor;
    HIFB_POINT_S stHotPos;
} HIFB_CURSOR_S;

/** Encoder picture delivery method */
typedef enum
{
    HIFB_STEREO_MONO   = 0x0,             /**< Normal output to non-stereoscopic (3D) TV. No L/R content provided to TV*/
    HIFB_STEREO_SIDEBYSIDE_HALF,          /**< L/R frames are downscaled horizontally by 2 andpacked side-by-side into a single frame, left on lefthalf of frame*/
    HIFB_STEREO_TOPANDBOTTOM,             /**< L/R frames are downscaled vertically by 2 andpacked into a single frame, left on top*/
    HIFB_STEREO_FRMPACKING,               /**< one frames are copyed */    
    HIFB_STEREO_BUTT
}HIFB_STEREO_MODE_E;

/** 3D stereo mode*/
typedef enum
{
    HIFB_STEREO_WORKMODE_HW_FULL = 0x0,      /**< 3d stereo function use hardware and transfer full frame to vo, note: hardware doesn't support this mode if encoder picture delivery method is top and bottom */   
    HIFB_STEREO_WORKMODE_HW_HALF,            /**< 3d stereo function use hardware and transfer half frame to vo*/
    HIFB_STEREO_WORKMODE_SW_EMUL,            /**< 3d stereo function use software emulation */
    HIFB_STEREO_WORKMODE_BUTT
}HIFB_STEREO_WORKMODE_E;

/**antiflicker level*/
/**Auto means fb will choose a appropriate antiflicker level automatically according to the color info of map*/
typedef enum
{
    HIFB_LAYER_ANTIFLICKER_NONE = 0x0,	/**< no antiflicker*/
    HIFB_LAYER_ANTIFLICKER_LOW = 0x1,	/**< low level*/
    HIFB_LAYER_ANTIFLICKER_MIDDLE = 0x2,/**< middle level*/
    HIFB_LAYER_ANTIFLICKER_HIGH = 0x3,  /**< high level*/
    HIFB_LAYER_ANTIFLICKER_AUTO = 0x4,  /**< auto*/
    HIFB_LAYER_ANTIFLICKER_BUTT
}HIFB_LAYER_ANTIFLICKER_LEVEL_E;

/*layer info maskbit*/
typedef enum
{
    HIFB_LAYERMASK_BUFMODE = 0x1,           /**< Whether the buffer mode in HIFB_LAYER_INFO_S is masked when the graphics layer information is set.*//**<CNcomment:����ͼ����Ϣʱ��HIFB_LAYER_INFO_S��bufģʽ�Ƿ���Ч���� */
    HIFB_LAYERMASK_ANTIFLICKER_MODE = 0x2,  /**< Whether the anti-flicker mode is masked.*//**<CNcomment:����˸ģʽ�Ƿ���Ч���� */
    HIFB_LAYERMASK_POS = 0x4,               /**< Whether the graphics layer position is masked.*//**<CNcomment:ͼ��λ���Ƿ���Ч���� */
    HIFB_LAYERMASK_CANVASSIZE = 0x8,        /**< Whether the canvas size is masked.*//**<CNcomment:canvassize�Ƿ���Ч���� */
    HIFB_LAYERMASK_DISPSIZE = 0x10,         /**< Whether the display size is masked.*//**<CNcomment:displaysize�Ƿ���Ч���� */
    HIFB_LAYERMASK_SCREENSIZE = 0x20,       /**< Whether the screen size is masked.*//**<CNcomment:screensize�Ƿ���Ч���� */
    HIFB_LAYERMASK_BMUL = 0x40,             /**< Whether the premultiplexed mode is masked.*//**<CNcomment:�Ƿ�Ԥ���Ƿ���Ч���� */
    HIFB_LAYERMASK_BUTT
}HIFB_LAYER_INFO_MASKBIT;


/**layer info*/
typedef struct
{
    HIFB_LAYER_BUF_E BufMode;
    HIFB_LAYER_ANTIFLICKER_LEVEL_E eAntiflickerLevel;	
    HI_S32 s32XPos;           /**<  the x pos of origion point in screen */
    HI_S32 s32YPos;	          /**<  the y pos of origion point in screen */
    HI_S32 u32CanvasWidth;    /**<  the width of canvas buffer */
    HI_S32 u32CanvasHeight;   /**<  the height of canvas buffer */
    HI_U32 u32DisplayWidth;	  /**<  the width of display buf in fb.for 0 buf ,there is no display buf in fb, so it's effectless*/
    HI_U32 u32DisplayHeight;  /**<  the height of display buf in fb. */
    HI_U32 u32ScreenWidth;    /**<  the width of screen */
    HI_U32 u32ScreenHeight;   /**<  the height of screen */
    HI_BOOL bPreMul;          /**<  The data drawed in buf is premul data or not*/
    HI_BOOL bUseNewScreen;    /**<  whether use new screen*/
    HI_U32 u32Mask;			  /**<  param modify mask bit*/
}HIFB_LAYER_INFO_S;


typedef struct 
{
    HIFB_RECT           stRect;        /**< the position you wanted to show on the screen */
    HIFB_COLOR_FMT_E    ePixelFmt;     /**< the color format of scrolltext content */
    HI_U16              u16CacheNum;   /**< The cached buffer number for store scrolltext content */
    HI_U16              u16RefreshFreq;/**< The refresh frequency you wanted */
    HI_BOOL             bDeflicker;    /**< Whether enable antificker */
}HIFB_SCROLLTEXT_ATTR_S;

typedef struct
{
    HIFB_SCROLLTEXT_ATTR_S stAttr;/**< The scrolltext attribution */
    HI_U32  u32Handle;            /**< The output ID for the scrolltext */
}HIFB_SCROLLTEXT_CREATE_S;

typedef struct
{
    HI_U32  u32Handle;     /**< ID of the scrolltext */
    HI_U32  u32PhyAddr;    /**< The physical address of the scrolltext content buffer */
    HI_U8   *pu8VirAddr;   /**< The virtual address of the scrolltext content buffer */
    HI_U32  u32Stride;     /**< The stride of the scrolltext content buffer */
}HIFB_SCROLLTEXT_DATA_S;


#define GP_LAYER_NUM 4
/*virtual window info*/
typedef enum
{
    HIFB_ZORDER_MOVETOP = 0,  /**< Move to the top*//**<CNcomment:�Ƶ����*/
    HIFB_ZORDER_MOVEUP,       /**< Move upwards*//**<CNcomment:������*/
    HIFB_ZORDER_MOVEBOTTOM,   /**< Move to the bottom*//**<CNcomment:�Ƶ���ײ�*/
    HIFB_ZORDER_MOVEDOWN,     /**< Move downwards*//**<CNcomment:������*/
    HIFB_ZORDER_BUTT
}HIFB_ZORDER_E;

typedef struct
{
	int fill_0[25]; //0
	HI_U32 u32LayerId; //100
} HIFB_SLV_LAYER_INFO_S;



/** @} */  /** <!-- ==== Structure Definition end ==== */


#if 0
/******************************* API declaration *****************************/
/** \addtogroup     HiFB */
/** @{ */  /** <!-- ��HiFB�� */

/**
\brief Obtains the variable information about the screen. CNcomment:��ȡ��Ļ�Ŀɱ���Ϣ.CNend
\attention \n
By default, the resolution and pixel format of the standard-definition (SD) graphics layer are 720x576 and ARGB1555 respectively; the resolution and pixel format of the high-definition (HD) graphics layer are 1280x720 and ARGB888 respectively;
CNcomment:ϵͳĬ�ϵı���ͼ��ֱ���Ϊ720��576�����ظ�ʽΪARGB1555��ϵͳĬ�ϵĸ���ͼ��ֱ���Ϊ1280��720�����ظ�ʽΪARGB888��CNend

\param[in] fd ID of an FB device. CNcomment:Framebuffer�豸��CNend
\param[in] FBIOGET_VSCREENINFO  ioctl number. CNcomment:ioctl��CNend
\param[out] var  Pointer to the variable information. CNcomment:�ɱ���Ϣ�ṹ��ָ��CNend
\retval 0  SUCCESS Success. CNcomment:�ɹ�CNend
\retval ::EPERM  1. The operation is not supported. CNcomment:1,��֧�ָò���CNend
\retval ::ENOMEM  12. The memory is insufficient. CNcomment:12,�ڴ治��CNend
\retval ::EFAULT  14. The input pointer is invalid. CNcomment:14,�������ָ���ַ��ЧCNend
\retval ::EINVAL  22. The input parameter is invalid. CNcomment:22,���������ЧCNend
\see \n
::FBIOPUT_VSCREENINFO
\par example
\code
struct fb_var_screeninfo vinfo;
if (ioctl(fd, FBIOGET_VSCREENINFO, &vinfo) < 0)
{
    return -1;
}

\endcode
*/
int ioctl (int fd, FBIOGET_VSCREENINFO, struct fb_var_screeninfo *var);


/**
\brief Sets the screen resolution and pixel format of the FB. CNcomment:����Framebuffer����Ļ�ֱ��ʺ����ظ�ʽ�ȡ�CNend
\attention \n
The resolution must be supported by each overlay layer. You can query the maximum resolution and minimum resolution supported by each overlay layer by calling FBIOGET_CAPABILITY_HIFB.\n
The sum of the actual resolution and offset value must be within the range of the virtual resolution. Otherwise, the system automatically adjusts the actual resolution.
CNcomment:�ֱ��ʵĴ�С�����ڸ����Ӳ�֧�ֵķֱ��ʷ�Χ�ڣ������Ӳ�֧�ֵ����ֱ��ʺ���С�ֱ��ʿ�ͨ��FBIOGET_CAPABILITY_HIFB��ȡ��CNend
���뱣֤ʵ�ʷֱ�����ƫ�Ƶĺ�������ֱ��ʷ�Χ�ڣ�����ϵͳ���Զ�����ʵ�ʷֱ��ʵĴ�С����������ֱ��ʷ�Χ�ڡ�CNend

\param[in] fd ID of an FB device. CNcomment:Framebuffer�豸��CNend
\param[in] FBIOPUT_VSCREENINFO ioctl number. CNcomment:ioctl��CNend
\param[in] var Pointer to the variable information. CNcomment:�ɱ���Ϣ�ṹ��ָ��CNend
\retval  0 SUCCESS Success. CNcomment:�ɹ�CNend
\retval ::EPERM  1. The operation is not supported. CNcomment:1,��֧�ָò���CNend
\retval ::ENOMEM  12. The memory is insufficient. CNcomment:12,�ڴ治��CNend
\retval ::EFAULT  14. The input pointer is invalid. CNcomment:14,�������ָ���ַ��ЧCNend
\retval ::EINVAL  22. The input parameter is invalid. CNcomment:22,���������ЧCNend
\see \n
::FBIOGET_VSCREENINFO
\par example
\code
//Assume that the actual resolution is 720x576, the virtual resolution is 720x576, the offset value is (0, 0), and the pixel format is ARGB8888. In this case, the sample code is as follows:
//CNcommnet:����ʵ�ʷֱ���Ϊ720��576������ֱ���Ϊ720��576��ƫ��Ϊ��0��0�������ظ�ʽΪARGB8888��ʾ��������£�CNend
struct fb_bitfield r32 = {16, 8, 0};
struct fb_bitfield g32 = {8, 8, 0};
struct fb_bitfield b32 = {0, 8, 0}; 
struct fb_bitfield a32 = {24, 8, 0};
struct fb_var_screeninfo vinfo;
if (ioctl(fd, FBIOGET_VSCREENINFO, &vinfo) < 0)
{
    return -1;
}
vinfo.xres_virtual = 720;
vinfo.yres_virtual = 576;
vinfo.xres = 720;
vinfo.yres = 576;
vinfo.activate = FB_ACTIVATE_NOW;
vinfo.bits_per_pixel = 32;
vinfo.xoffset = 0;
vinfo.yoffset = 0;
vinfo.red = r32;
vinfo.green = g32;
vinfo.blue = b32;
vinfo.transp= a32;
if (ioctl(fd, FBIOPUT_VSCREENINFO, &vinfo) < 0)
{
    return -1;
}

\endcode
*/
int ioctl (int fd, FBIOPUT_VSCREENINFO, struct fb_var_screeninfo *var);



/**
\brief Obtains the fixed information about the FB. CNcomment:��ȡFramebuffer�Ĺ̶���Ϣ��CNend
\attention \n
None
\param[in] fd ID of an FB device. CNcomment:Framebuffer�豸��CNend
\param[in] FBIOGET_FSCREENINFO  ioctl number. CNcomment:ioctl��CNend
\param[out] fix Pointer to the fixed information. CNcomment:�̶���Ϣ�ṹ��ָ��CNend
\retval  0 SUCCESS Success. CNcomment:�ɹ�CNend
\retval ::EPERM  1. The operation is not supported. CNcomment:1,��֧�ָò���CNend
\retval ::ENOMEM  12. The memory is insufficient. CNcomment:12,�ڴ治��CNend
\retval ::EFAULT  14. The input pointer is invalid. CNcomment:14,�������ָ���ַ��ЧCNend
\retval ::EINVAL  22. The input parameter is invalid. CNcomment:22,���������ЧCNend
\see \n
N/A
*/
int ioctl (int fd, FBIOGET_FSCREENINFO, struct fb_fix_screeninfo *fix);



/**
\brief Sets to display contents at different offset positions of the virtual resolution. CNcommnet:���ô�����ֱ����еĲ�ͬƫ�ƴ���ʼ��ʾ��CNend
\attention \n
The sum of the actual resolution and offset value must be within the range of the virtual resolution. Otherwise, the setting is unsuccessful.
CNcomment:���뱣֤ʵ�ʷֱ�����ƫ�Ƶĺ�������ֱ��ʷ�Χ�ڣ��������ò��ɹ���CNend

\param[in] fd ID of an FB device. CNcomment:Framebuffer�豸��CNend
\param[in] FBIOPAN_DISPLAY  ioctl number. CNcomment:ioctl��CNend
\param[out] var  Pointer to the variable information. CNcomment:�ɱ���Ϣ�ṹ��ָ��CNend
\retval  0 SUCCESS Success. CNcomment:�ɹ�CNend
\retval ::EPERM  1. The operation is not supported. CNcomment:1,��֧�ָò���CNend
\retval ::ENOMEM  12. The memory is insufficient. CNcomment:12,�ڴ治��CNend
\retval ::EFAULT  14. The input pointer is invalid.  CNcomment:14,�������ָ���ַ��ЧCNend
\retval ::EINVAL  22. The input parameter is invalid. CNcomment:22,���������ЧCNend
\see \n
N/A
\par example
\code
//Assume that the actual resolution is 300x300, the virtual resolution is 720x576, the start offset value is (50, 50), and the display position is (300, 0). In this case, the code for the PAN setting is as follows:
//CNcomment:����ʵ�ʷֱ���Ϊ300��300������ֱ���Ϊ720��576����ʼƫ��Ϊ��50��50����Ȼ��ƫ�Ƶ���300��0������ʼ��ʾ��PAN���ô������£�CNend
struct fb_bitfield r32 = {16, 8, 0};
struct fb_bitfield g32 = {8, 8, 0};
struct fb_bitfield b32 = {0, 8, 0};
struct fb_bitfield a32 = {24, 8, 0};
struct fb_var_screeninfo vinfo;

vinfo.xres_virtual = 720;
vinfo.yres_virtual = 576;
vinfo.xres = 300;
vinfo.yres = 300;
vinfo.activate = FB_ACTIVATE_NOW;
vinfo.bits_per_pixel = 32;
vinfo.xoffset = 50;
vinfo.yoffset = 50;
vinfo.red = r32;
vinfo.green = g32;
vinfo.blue = b32;
vinfo.transp= a32;
if (ioctl(fd, FBIOPUT_VSCREENINFO, &vinfo) < 0)
{
    return -1;
}
vinfo.xoffset = 300;
vinfo.yoffset = 0;
if (ioctl(fd, FBIOPAN_DISPLAY, &vinfo) < 0)
{
    return -1;
}

\endcode
*/
int ioctl (int fd, FBIOPAN_DISPLAY, struct fb_var_screeninfo *var);



/**
\brief Obtains the information about the palette. CNcommnet:��ȡ��ɫ����Ϣ��CNend
\attention \n
None

\param[in] fd ID of an FB device. CNcomment:Framebuffer�豸��CNend
\param[in] FBIOGETCMAP  ioctl number. CNcomment:ioctl��CNend
\param[out] cmap Pointer to the palette. CNcomment:��ɫ��ṹ��ָ��CNend
\retval  0 SUCCESS Success. CNcomment:�ɹ�CNend
\retval ::EPERM  1. The operation is not supported. CNcomment:1,��֧�ָò���CNend
\retval ::ENOMEM  12. The memory is insufficient. CNcomment:12,�ڴ治��CNend
\retval ::EFAULT  14. The input pointer is invalid. CNcomment:14,�������ָ���ַ��ЧCNend
\retval ::EINVAL  22. The input parameter is invalid. CNcomment:22,���������ЧCNend
\see \n
::FBIOPUTCMAP
\par example
\code
//only low 8bits valid 
unsigned short Red256[256];   
//only low 8bits valid 
unsigned short Green256[256];  
// only low 8bits valid 
unsigned short Blue256[256];   
struct fb_cmap cmap;

cmap.start = 0;
cmap.len = 256;
cmap.red = Red256;
cmap.green = Green256;
cmap.blue = Blue256;
cmap.transp = 0;

if (ioctl(fd, FBIOGETCMAP, &cmap) < 0)
{
    printf("fb ioctl get cmap err!\n");
    return -1;
}

\endcode
*/
int ioctl (int fd, FBIOGETCMAP, struct fb_cmap *cmap);



/**
\brief Sets the information about the palette. CNcomment:���õ�ɫ����Ϣ��CNend
\attention \n
None
\param[in] fd ID of an FB device. CNcomment:Framebuffer�豸��CNend
\param[in] FBIOPUTCMAP  ioctl number. CNcomment:ioctl��CNend
\param[in] cmap Pointer to the palette. CNcomment:��ɫ��ṹ��ָ��CNend
\retval  0 SUCCESS Success. CNcomment:�ɹ�CNend
\retval ::EPERM  1. The operation is not supported. CNcomment:1,��֧�ָò���CNend
\retval ::ENOMEM  12. The memory is insufficient. CNcomment:12,�ڴ治��CNend
\retval ::EFAULT  14. The input pointer is invalid. CNcomment:14,�������ָ���ַ��ЧCNend
\retval ::EINVAL  22. The input parameter is invalid. CNcomment:22,���������ЧCNend
\see \n
::FBIOGETCMAP
\par example
\code
//only low 8bits valid 
unsigned short Red256[256];     
//only low 8bits valid 
unsigned short Green256[256]; 
//only low 8bits valid 
unsigned short Blue256[256];    
struct fb_cmap cmap;

//create a palette which contains 256 color 
Palette_Create(Red256, Green256, Blue256);

cmap.start = 0;
cmap.len = 256;
cmap.red = Red256;
cmap.green = Green256;
cmap.blue = Blue256;
cmap.transp = 0;

if (ioctl(fd, FBIOPUTCMAP, &cmap) < 0)
{
    printf("fb ioctl put cmap err!\n");
    return -1;
}

\endcode
*/
int ioctl (int fd, FBIOPUTCMAP, struct fb_cmap *cmap);



/**-----Extended Functions--------*/
/**
\brief Obtains the capability of an overlay layer. CNcomment:��ȡ���Ӳ��֧��������CNend
\attention \n
None
\param[in] fd ID of an FB device. CNcomment:Framebuffer�豸��CNend
\param[in] FBIOGET_CAPABILITY_HIFB  ioctl number. CNcomment:ioctl��CNend
\param[out] pstCap Pointer to the capability. CNcomment:֧�������ṹ��ָ��CNend
\retval  0 SUCCESS Success. CNcomment:�ɹ�CNend
\retval ::EPERM  1. The operation is not supported. CNcomment:1,��֧�ָò���CNend
\retval ::ENOMEM  12. The memory is insufficient. CNcomment:12,�ڴ治��CNend
\retval ::EFAULT  14. The input pointer is invalid. CNcomment:14,�������ָ���ַ��ЧCNend
\retval ::EINVAL  22. The input parameter is invalid. CNcomment:22,���������ЧCNend
\see \n
N/A
*/
int ioctl (int fd, FBIOGET_CAPABILITY_HIFB, HIFB_CAPABILITY_S *pstCap);



/**
\brief Obtains the coordinates of the start display point of an overlay layer on the screen. CNcomment:��ȡ���Ӳ�����Ļ����ʾ����ʼ����ꡣCNend
\attention \n
None
\param[in] fd ID of an FB device. CNcomment:Framebuffer�豸��CNend
\param[in] FBIOGET_SCREEN_ORIGIN_HIFB  ioctl number. CNcomment:ioctl��CNend
\param[out] pstPoint Pointer to the coordinate origin. CNcomment:���ԭ��ṹ��ָ��CNend
\retval  0 SUCCESS Success. CNcomment:�ɹ�CNend
\retval ::EPERM  1. The operation is not supported. CNcomment:1,��֧�ָò���CNend
\retval ::ENOMEM  12. The memory is insufficient. CNcomment:12,�ڴ治��CNend
\retval ::EFAULT  14. The input pointer is invalid. CNcomment:14,�������ָ���ַ��ЧCNend
\retval ::EINVAL  22. The input parameter is invalid. CNcomment:22,���������ЧCNend
\see \n
::FBIOPUT_SCREEN_ORIGIN_HIFB
*/
int ioctl (int fd, FBIOGET_SCREEN_ORIGIN_HIFB, HIFB_POINT_S *pstPoint);



/**
\brief Sets the coordinates of the start display point of an overlay layer on the screen. CNcomment:���õ��Ӳ�����Ļ����ʾ����ʼ����ꡣCNend
\attention \n
If the coordinate origin of an overlay layer is beyond the range, the coordinate origin is set to (u32MaxWidth - u32MinWidth, u32MaxHeight - u32MinHeight) by default.
CNcomment:�����Ӳ����ԭ�㳬���˷�Χ��Ĭ�Ͻ����ԭ������Ϊ��u32MaxWidth �Cu32MinWidth,��u32MaxHeight �Cu32MinHeight����CNend
\param[in] fd ID of an FB device. CNcomment:Framebuffer�豸��CNend
\param[in] FBIOPUT_SCREEN_ORIGIN_HIFB  ioctl number. CNcomment:ioctl��CNend
\param[in] pstPoint Pointer to the coordinate origin. CNcomment:���ԭ��ṹ��ָ��CNend
\retval  0 SUCCESS Success. CNcomment:�ɹ�CNend
\retval ::EPERM  1. The operation is not supported. CNcomment:1,��֧�ָò���CNend
\retval ::ENOMEM  12. The memory is insufficient. CNcomment:12,�ڴ治��CNend
\retval ::EFAULT  14. The input pointer is invalid. CNcomment:14,�������ָ���ַ��ЧCNend
\retval ::EINVAL  22. The input parameter is invalid. CNcomment:22,���������ЧCNend
\see \n
::FBIOGET_SCREEN_ORIGIN_HIFB
*/
int ioctl (int fd, FBIOPUT_SCREEN_ORIGIN_HIFB, HIFB_POINT_S *pstPoint);



/**
\brief Obtains the display status of the current overlay layer. CNcomment:��ȡ��ǰ���Ӳ����ʾ״̬��CNend
\attention \n
None
\param[in] fd ID of an FB device. CNcomment:Framebuffer�豸��CNend
\param[in] FBIOGET_SHOW_HIFB  ioctl number. CNcomment:ioctl��CNend
\param[out] bShow Status of the current overlay layer. If *bShow is set to HI_TRUE, it indicates that the current overlay layer is shown; if *bShow is set to HI_FALSE, it indicates that the current overlay layer is hidden. CNcomment:ָʾ��ǰ���Ӳ��״̬��*bShow = HI_TRUE����ǰ���Ӳ㴦����ʾ״̬��*bShow = HI_FALSE����ǰ���Ӳ㴦������״̬CNend
\retval  0 SUCCESS Success. CNcomment:�ɹ�CNend
\retval ::EPERM  1. The operation is not supported. CNcomment:1,��֧�ָò���CNend
\retval ::ENOMEM  12. The memory is insufficient. CNcomment:12,�ڴ治��CNend
\retval ::EFAULT  14. The input pointer is invalid. CNcomment:14,�������ָ���ַ��ЧCNend
\retval ::EINVAL  22. The input parameter is invalid. CNcomment:22,���������ЧCNend
\see \n
::FBIOPUT_SHOW_HIFB
*/
int ioctl (int fd, FBIOGET_SHOW_HIFB, HI_BOOL *bShow);



/**
\brief Shows or hides the current overlay layer. CNcomment:��ʾ�����ظõ��Ӳ㡣CNend
\attention \n
None
\param[in] fd ID of an FB device. CNcomment:Framebuffer�豸��CNend
\param[in] FBIOPUT_SHOW_HIFB  ioctl number. CNcomment:ioctl��CNend
\param[in] bShow Display status of the current overlay layer. If *bShow is set to HI_TRUE, it indicates that the current overlay layer is shown; if *bShow is set to HI_FALSE, it indicates that the current overlay layer is hidden. CNcomment:�õ��Ӳ����ʾ״̬��*bShow = HI_TRUE����ʾ��ǰ���Ӳ㣻*bShow = HI_FALSE�����ص�ǰ���Ӳ�CNend
\retval  0 SUCCESS Success. CNcomment:�ɹ�CNend
\retval ::EPERM  1. The operation is not supported. CNcomment:1,��֧�ָò���CNend
\retval ::ENOMEM  12. The memory is insufficient. CNcomment:12,�ڴ治��CNend
\retval ::EFAULT  14. The input pointer is invalid. CNcomment:14,�������ָ���ַ��ЧCNend
\retval ::EINVAL  22. The input parameter is invalid. CNcomment:22,���������ЧCNend
\see \n
::FBIOGET_SHOW_HIFB
*/
int ioctl (int fd, FBIOPUT_SHOW_HIFB, HI_BOOL *bShow);



/**
\brief Obtains the colorkey of an overlay layer. CNcomment:��ȡ���Ӳ��colorkey��CNend
\attention \n
None
\param[in] fd ID of an FB device. CNcomment:Framebuffer�豸��CNend
\param[in] FBIOGET_COLORKEY_HIFB  ioctl number. CNcomment:ioctl��CNend
\param[out] pstColorKey Pointer to the colorkey. CNcomment:colorkey�ṹ��ָ��CNend
\retval  0 SUCCESS Success. CNcomment:�ɹ�CNend
\retval ::EPERM  1. The operation is not supported. CNcomment:1,��֧�ָò���CNend
\retval ::ENOMEM  12. The memory is insufficient. CNcomment:12,�ڴ治��CNend
\retval ::EFAULT  14. The input pointer is invalid. CNcomment:14,�������ָ���ַ��ЧCNend
\retval ::EINVAL  22. The input parameter is invalid. CNcomment:22,���������ЧCNend
\see \n
::FBIOPUT_COLORKEY_HIFB
*/
int ioctl (int fd, FBIOGET_COLORKEY_HIFB, HIFB_COLORKEY_S *pstColorKey);



/**
\brief Sets the colorkey of an overlay layer. CNcomment:���õ��Ӳ��colorkey��CNend
\attention \n
None
\param[in] fd  ID of an FB device. CNcomment:Framebuffer�豸��CNend
\param[in] FBIOPUT_COLORKEY_HIFB  ioctl number. CNcomment:ioctl��CNend
\param[in] pstColorKey  Pointer to the colorkey. CNcomment:colorkey�ṹ��ָ��CNend
\retval  0 SUCCESS Success. CNcomment:�ɹ�CNend
\retval ::EPERM  1. The operation is not supported. CNcomment:1,��֧�ָò���CNend
\retval ::ENOMEM  12. The memory is insufficient. CNcomment:12,�ڴ治��CNend
\retval ::EFAULT  14. The input pointer is invalid. CNcomment:14,�������ָ���ַ��ЧCNend
\retval ::EINVAL  22. The input parameter is invalid. CNcomment:22,���������ЧCNend
\see \n
::FBIOGET_COLORKEY_HIFB
\par example
\code
//Assume that the pixel format is ARGB8888. To filter the red color, perform the following setting:
//CNcomment:���赱ǰ���ظ�ʽΪARGB8888����Ҫ���˵���ɫ,�����������£�CNend
HIFB_COLORKEY_S stColorKey;

stColorKey.bKeyEnable = HI_TRUE;
stColorKey.u32Key = 0xFF0000;
if (ioctl(fd, FBIOPUT_COLORKEY_HIFB, &stColorKey) < 0)
{
    return -1;
}

\endcode
*/
int ioctl (int fd, FBIOPUT_COLORKEY_HIFB, HIFB_COLORKEY_S *pstColorKey);



/**
\brief Obtains the alpha value of an overlay layer. CNcomment:��ȡ���Ӳ�Alpha��CNend
\attention \n
For details, see the description of ::HIFB_ALPHA_S.
CNcomment:��μ�::HIFB_ALPHA_S��˵����CNend

\param[in] fd  ID of an FB device. CNcomment:Framebuffer�豸��CNend
\param[in] FBIOGET_ALPHA_HIFB  ioctl number. CNcomment:ioctl��CNend
\param[out] pstAlpha  Pointer to the alpha value. CNcomment:Alpha�ṹ��ָ��CNend
\retval  0 SUCCESS Success. CNcomment:�ɹ�CNend
\retval ::EPERM  1. The operation is not supported. CNcomment:1,��֧�ָò���CNend
\retval ::ENOMEM  12. The memory is insufficient. CNcomment:12,�ڴ治��CNend
\retval ::EFAULT  14. The input pointer is invalid. CNcomment:14,�������ָ���ַ��ЧCNend
\retval ::EINVAL  22. The input parameter is invalid. CNcomment:22,���������ЧCNend
\see \n
::FBIOPUT_ALPHA_HIFB
*/
int ioctl (int fd, FBIOGET_ALPHA_HIFB, HIFB_ALPHA_S *pstAlpha);




/**
\brief Sets the alpha value of an overlay layer. CNcomment:���õ��Ӳ��Alpha��CNend
\attention \n
For details, see the description of ::HIFB_ALPHA_S.
CNcomment:��μ�::HIFB_ALPHA_S��˵��CNend

\param[in] fd  ID of an FB device. CNcomment:Framebuffer�豸��CNend
\param[in] FBIOPUT_ALPHA_HIFB  ioctl number. CNcomment:ioctl��CNend
\param[in] pstAlpha  Pointer to the alpha value. CNcomment:Alpha�ṹ��ָ��CNend
\retval  0 SUCCESS Success. CNcomment:�ɹ�CNend
\retval ::EPERM  1. The operation is not supported. CNcomment:1,��֧�ָò���CNend
\retval ::ENOMEM  12. The memory is insufficient. CNcomment:12,�ڴ治��CNend
\retval ::EFAULT  14. The input pointer is invalid. CNcomment:14,�������ָ���ַ��ЧCNend
\retval ::EINVAL  22. The input parameter is invalid. CNcomment:22,���������ЧCNend
\see \n
::FBIOGET_ALPHA_HIFB
*/
int ioctl (int fd, FBIOPUT_ALPHA_HIFB, HIFB_ALPHA_S *pstAlpha);



/**
\brief Obtains the anti-flicker setting of an overlay layer. The HD platform does not support this function. CNcomment:��ȡ���Ӳ�Ŀ���˸���á�����ƽ̨��֧�����á�CNend
\attention \n
Before obtaining the values of anti-flicker parameters, you must set the maximum anti-flicker level that can be obtained, and allocate sufficient memory for storing anti-flicker coefficients. This API is invalid for the HD platform currently.
CNcomment:�ڻ�ȡ����˸����ʱ�����������ܹ���ȡ����˸����󼶱���Ϊ����˸ϵ������㹻���ڴ档Ŀǰ�ýӿ��ڸ�����Ч��CNend

\param[in] fd  ID of an FB device. CNcomment:Framebuffer�豸��CNend
\param[in] FBIOGET_DEFLICKER_HIFB  ioctl number. CNcomment:ioctl��CNend
\param[out] pstDeflicker  Pointer to the anti-flicker setting. CNcomment:����˸�ṹ��ָ��CNend
\retval  0 SUCCESS Success. CNcomment:�ɹ�CNend
\retval ::EPERM  1. The operation is not supported. CNcomment:1,��֧�ָò���CNend
\retval ::ENOMEM  12. The memory is insufficient. CNcomment:12,�ڴ治��CNend
\retval ::EFAULT  14. The input pointer is invalid. CNcomment:14,�������ָ���ַ��ЧCNend
\retval ::EINVAL  22. The input parameter is invalid. CNcomment:22,���������ЧCNend
\see \n
::FBIOPUT_DEFLICKER_HIFB
\par example
\code
//Assume that the maximum horizontal and vertical anti-flicker levels that can be obtained are 2. In this case, the sample code is as follows:
//CNcomment:��ȡˮƽ�ʹ�ֱ����˸��󼶱�Ϊ2��ʾ��������£�CNend
HI_U8 u8HDefCoef;
HI_U8 u8VDefCoef;
HIFB_DEFLICKER_S stDeflicker;

stDeflicker.u32HDfLevel = 2;
stDeflicker.u32VDfLevel = 2;
stDeflicker.pu8HDfCoef = &u8HDefCoef;
stDeflicker.pu8VDfCoef = &u8VDefCoef;

if (ioctl(fd, FBIOGET_DEFLICKER_HIFB, &stDeflicker) < 0)
{
    return -1;
}

\endcode
*/
int ioctl (int fd, FBIOGET_DEFLICKER_HIFB, HIFB_DEFLICKER_S *pstDeflicker);



/**
\brief Sets the anti-flicker function of an overlay layer. CNcomment:���õ��Ӳ�Ŀ���˸���ܡ�CNend
\attention \n
For details, see the description of ::HIFB_DEFLICKER_S. This API is invalid for the HD platform currently.
CNcomment:��μ�::HIFB_DEFLICKER_S��˵����Ŀǰ�ýӿ��ڸ�����Ч��CNend

\param[in] fd  ID of an FB device. CNcomment:Framebuffer�豸��CNend
\param[in] FBIOPUT_DEFLICKER_HIFB  ioctl number. CNcomment:ioctl��CNend
\param[in] pstDeflicker  Pointer to the anti-flicker setting. CNcomment:����˸�ṹ��ָ��CNend
\retval  0 SUCCESS Success. CNcomment:�ɹ�CNend
\retval ::EPERM  1. The operation is not supported. CNcomment:1,��֧�ָò���CNend
\retval ::ENOMEM  12. The memory is insufficient. CNcomment:12,�ڴ治��CNend
\retval ::EFAULT  14. The input pointer is invalid. CNcomment:14,�������ָ���ַ��ЧCNend
\retval ::EINVAL  22. The input parameter is invalid. CNcomment:22,���������ЧCNend
\see \n
::FBIOGET_DEFLICKER_HIFB
\par example
\code
//Assume that the 2-tap horizontal and vertical anti-flicker functions are required. In this case, the sample code is as follows:
//CNcomment:����ˮƽ�ʹ�ֱ2�׿���˸�Ĵ������£�CNend
HI_U8 u8HDefCoef = 0x80;
HI_U8 u8VDefCoef = 0x80;
HIFB_DEFLICKER_S stDeflicker;

stDeflicker.u32HDfLevel = 2;
stDeflicker.u32VDfLevel = 2;
stDeflicker.pu8HDfCoef = &u8HDefCoef;
stDeflicker.pu8VDfCoef = &u8VDefCoef;

if (ioctl(fd, FBIOPUT_DEFLICKER_HIFB, &stDeflicker) < 0)
{
    return -1;
}
\endcode
*/
int ioctl (int fd, FBIOPUT_DEFLICKER_HIFB, HIFB_DEFLICKER_S *pstDeflicker);



/**
\brief Waits for the vertical blanking region of an overlay layer. To operate the display buffer without tearing, you can operate it in the vertical blanking region. CNcomment:Ϊ�˲����Դ�ʱ������˺������һ������ڸõ��Ӳ�Ĵ�ֱ��������Դ���в�����ͨ��ýӿڿ��Եȴ�õ��Ӳ㴹ֱ������ĵ�����CNend
\attention \n
None
\param[in] fd ID of an FB device. CNcomment:Framebuffer�豸��CNend
\param[in] FBIOGET_VBLANK_HIFB  ioctl number. CNcomment:ioctl��CNend
\retval  0 SUCCESS Success. CNcomment:�ɹ�CNend
\retval ::EPERM  1. The operation is not supported. CNcomment:1,��֧�ָò���CNend
\retval ::ENOMEM  12. The memory is insufficient. CNcomment:12,�ڴ治��CNend
\retval ::EFAULT  14. The input pointer is invalid. CNcomment:14,�������ָ���ַ��ЧCNend
\retval ::EINVAL  22. The input parameter is invalid. CNcomment:22,���������ЧCNend
\see \n
N/A
*/
int ioctl (int fd, FBIOGET_VBLANK_HIFB);


/**
\brief Obtains the stereo frame encode format. CNcomment:��ȡStereo֡�����ʽ���ܡ�CNend
\attention \n
For details, see the description of ::HIFB_STEREO_MODE_E. 

\param[in] fd  Framebuffer�豸��CNend
\param[in] FBIOGET_STEREO_MODE  ioctl��CNend
\param[out] penStereoMode  stereoģʽö��ָ��CNend
\retval  0 SUCCESS �ɹ�CNend
\retval ::EPERM  1,��֧�ָò���CNend
\retval ::ENOMEM  12,�ڴ治��CNend
\retval ::EFAULT  14,�������ָ���ַ��ЧCNend
\retval ::EINVAL  22,���������ЧCNend
\see \n
::FBIOPUT_STEREO_MODE
*/
int ioctl (int fd, FBIOGET_STEREO_MODE, HIFB_STEREO_MODE_E *penStereoMode);


/**
\brief ����Stereoģʽ���ܡ�CNend
\attention \n
��μ�::HIFB_STEREO_MODE_E��˵����CNend

\param[in] fd  Framebuffer�豸��CNend
\param[in] FBIOPUT_STEREO_MODE  ioctl��CNend
\param[in] penStereoMode  stereoģʽö��ָ��CNend
\retval  0 SUCCESS �ɹ�CNend
\retval ::EPERM  1,��֧�ָò���CNend
\retval ::ENOMEM  12,�ڴ治��CNend
\retval ::EFAULT  14,�������ָ���ַ��ЧCNend
\retval ::EINVAL  22,���������ЧCNend
\see \n
::FBIOGET_STEREO_MODE
*/
int ioctl (int fd, FBIOPUT_STEREO_MODE, HIFB_STEREO_MODE_E *penStereoMode);


/**
\brief ��ȡStereo֡�����ʽ���ܡ�CNend
\attention \n
CNcomment:��μ�::HIFB_ENCODER_PICTURE_FRAMING_E��˵����CNend

\param[in] fd  ID of an FB device. CNcomment:Framebuffer�豸��CNend
\param[in] FBIOGET_STEREO_MODE  ioctl number. CNcomment:ioctl��CNend
\param[out] penStereoMode  Pointer to the frame encode format. CNcomment:֡�����ʽö��ָ��CNend
\retval  0 SUCCESS Success. CNcomment:�ɹ�CNend
\retval ::EPERM  1. The operation is not supported. CNcomment:1,��֧�ָò���CNend
\retval ::ENOMEM  12. The memory is insufficient. CNcomment:12,�ڴ治��CNend
\retval ::EFAULT  14. The input pointer is invalid. CNcomment:14,�������ָ���ַ��ЧCNend
\retval ::EINVAL  22. The input parameter is invalid. CNcomment:22,���������ЧCNend
\see \n
::FBIOPUT_ENCODER_PICTURE_FRAMING
*/
int ioctl (int fd, FBIOGET_ENCODER_PICTURE_FRAMING, HIFB_STEREO_MODE_E *penEncPicFrm);


/**
\brief Sets the stereo frame encode format. CNcomment:����Stereo֡�����ʽ���ܡ�CNend
\attention \n
For details, see the description of ::HIFB_STEREO_MODE_E. 
CNcomment:��μ�::HIFB_ENCODER_PICTURE_FRAMING_E��˵����CNend

\param[in] fd  ID of an FB device. CNcomment:Framebuffer�豸��CNend
\param[in] FBIOPUT_STEREO_MODE  ioctl number. CNcomment:ioctl��CNend
\param[in] penStereoMode  Pointer to the frame encode format. CNcomment:֡�����ʽö��ָ��CNend
\retval  0 SUCCESS Success. CNcomment:�ɹ�CNend
\retval ::EPERM  1. The operation is not supported. CNcomment:1,��֧�ָò���CNend
\retval ::ENOMEM  12. The memory is insufficient. CNcomment:12,�ڴ治��CNend
\retval ::EFAULT  14. The input pointer is invalid. CNcomment:14,�������ָ���ַ��ЧCNend
\retval ::EINVAL  22. The input parameter is invalid. CNcomment:22,���������ЧCNend
\see \n
::FBIOGET_ENCODER_PICTURE_FRAMING
*/
int ioctl (int fd, FBIOPUT_ENCODER_PICTURE_FRAMING, HIFB_STEREO_MODE_E *penEncPicFrm);


/** @} */  /** <!-- ==== API declaration end ==== */
#endif


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __HIFB_H__ */

