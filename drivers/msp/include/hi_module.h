/******************************************************************************
Copyright (C), 2012-2062, Hisilicon Tech. Co., Ltd.
******************************************************************************
File Name     : hi_module.h
Version       : V1.0 Initial Draft
Author        : g00182102
Created       : 2012/6/19
Last Modified :
Description   : The module name definition for all the MODULES
Function List : None.
History       :
******************************************************************************/
/** @addtogroup   MODULES  */
/** @{ */  /** <!-- [MODULES] */
#ifndef __HI_MODULE_H__
#define __HI_MODULE_H__

#define HI_INVALID_MODULE_ID 	    (0xffffffff)
#define HI_MAX_USER_MODULE_NUMBER   (256)

/** Module ID flags */
typedef enum hiMOD_ID_E
{
    HI_ID_STB         = 0,

    /**< common. */ /**< CNcomment: 系统通用枚举数据常量 */
    HI_ID_SYS         = 1,
    HI_ID_MODULE      ,
    HI_ID_LOG         ,
    HI_ID_PROC        ,
    HI_ID_MEM         ,
    HI_ID_STAT        ,
    HI_ID_PDM         ,
    HI_ID_MEMDEV      ,

    HI_ID_DEMUX       = 0x0A,

    /**< audio. */ /**< CNcomment: 音频部分常量区 */
    HI_ID_ADEC        = 0x10,
    HI_ID_AO          ,
    HI_ID_SIO_AI      ,
    HI_ID_SIO_AO      ,
    HI_ID_SIO         ,
    HI_ID_AI          ,
    HI_ID_AENC        ,
    HI_ID_SRC         ,
    HI_ID_AIAO        ,
    HI_ID_ADE         ,
    HI_ID_ADSP        ,

    /**< video. */ /**< CNcomment: 视频部分常量区 */
    HI_ID_VFMW        = 0x20,
    HI_ID_SVDEC       ,
    HI_ID_DISP        ,
    HI_ID_HDMI        ,
    HI_ID_VO          ,
    HI_ID_VPSS        ,
    HI_ID_VDEC        ,
    HI_ID_VI          ,
    HI_ID_VENC        ,
    HI_ID_PQ         ,
    /**< graphics. */ /**< CNcomment: 图形部分常量区 */
    HI_ID_TDE         = 0x30,
    HI_ID_JPGDEC      ,
    HI_ID_JPGENC      ,
    HI_ID_FB          ,
    HI_ID_PNG         ,
    HI_ID_HIGO        ,

    /**< player. */ /**< CNcomment: 播放相关部分常量区 */
    HI_ID_PVR         = 0x40,
    HI_ID_AVPLAY      ,
    HI_ID_SYNC        ,
    HI_ID_VSYNC       ,
    HI_ID_ASYNC       ,
    HI_ID_FASTPLAY ,

    /**< ecs. */ /**< CNcomment: 外设部分常量区 */
    HI_ID_FLASH       = 0x50,
    HI_ID_IR          ,
    HI_ID_RTC         ,
    HI_ID_I2C         ,
    HI_ID_SCI         ,
    HI_ID_ETH         ,
    HI_ID_USB_PROTECT ,
    HI_ID_WDG         = 0x57,   /* watch dog used 'W' */
    HI_ID_GPIO        ,
    HI_ID_GPIO_I2C    ,
    HI_ID_DMAC        ,
    HI_ID_PMOC        ,
    HI_ID_TUNER       ,
    HI_ID_KEYLED      ,
    HI_ID_E2PROM      ,
    HI_ID_CIPHER      ,
    HI_ID_OTP         = 0x60,
    HI_ID_CA          ,
    HI_ID_PM          ,
    HI_ID_CI          ,
    HI_ID_CIMAXPLUS   ,
    HI_ID_TVP5150     ,
    HI_ID_SIL9293     ,

    /**< voip, bluetooth,alsa. */ /**<  CNcomment: VOIP、蓝牙部分常量区*/
    HI_ID_VOIP_HME    = 0x80,
    HI_ID_NDPT        ,
    HI_ID_AUDADP      ,
    HI_ID_BT          ,
    HI_ID_ALSA        ,

    /**< vp. */ /**<  CNcomment: VP常量区*/
    HI_ID_VP          = 0x90,
    HI_ID_HDCP        ,

    /**< subtitle. */ /**<  CNcomment: 字幕常量区*/
    HI_ID_SUBT        = 0x98,
    HI_ID_TTX         ,
    HI_ID_CC          ,

    /**< loader. */ /**< CNcomment: loader */
    HI_ID_LOADER      = 0xA0,

    /**< user definition. */ /**< CNcomment: 为整机保留的自定义区 */
    HI_ID_USR_START   = 0xB0,
    HI_ID_USR_END     = 0xFE,

    HI_ID_BUTT        = 0xFF
} HI_MOD_ID_E;

#endif

/** @} */  /** <!-- ==== group Definition end ==== */

