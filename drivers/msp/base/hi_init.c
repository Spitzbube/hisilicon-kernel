#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/semaphore.h>

#include "hi_type.h"

#if 0
#include "drv_vdec_ext.h"
#include "drv_omxvdec_ext.h"
#include "drv_pmoc_ext.h"
#include "drv_gpio_ext.h"
#include "drv_gpioi2c_ext.h"
#include "drv_i2c_ext.h"
#include "drv_sci_ext.h"
#include "drv_otp_ext.h"
#include "drv_hdcp_ext.h"
#include "drv_sci_ext.h"
#include "drv_demux_ext.h"
#include "drv_pvr_ext.h"
#include "drv_sync_ext.h"
#include "drv_adec_ext.h"
#include "hi_drv_sci.h"
#include "hi_drv_wdg.h"
#include "drv_keyled_ext.h"
#include "drv_ir_ext.h"
#include "drv_jpeg_ext.h"
#include "drv_jpge_ext.h"
#include "drv_hifb_ext.h"
#include "drv_tde_ext.h"
#include "drv_tuner_ext.h"
#include "drv_png_ext.h"
#include "drv_avplay_ext.h"
#include "drv_disp_ext.h"
#include "drv_hdmi_ext.h"
#include "drv_cipher_ext.h"
#include "drv_vfmw_ext.h"
#include "drv_mce_ext.h"
#include "drv_vpss_ext.h"
#include "drv_venc_ext.h"
#include "drv_pdm_ext.h"
#include "drv_aenc_ext.h"
#include "drv_ao_ext.h"
#include "drv_adsp_ext.h"
#include "drv_vi_ext.h"
#include "drv_hdmirx_ext.h"
#include "drv_pq_ext.h"
#endif
#ifndef MODULE
HI_S32 HI_DRV_LoadModules(HI_VOID)
{
	printk("--> HI_DRV_LoadModules");

#if 0
    GPIO_DRV_ModInit();

#ifdef HI_GPIOI2C_SUPPORT
    GPIOI2C_DRV_ModInit();
#endif

    IR_DRV_ModInit();

    CIPHER_DRV_ModInit();

    PDM_DRV_ModInit();
	
    PQ_DRV_ModInit();	

    HDMI_DRV_ModInit();

    VDP_DRV_ModInit();

    TDE_DRV_ModInit();

    HIFB_DRV_ModInit();

    JPEG_DRV_ModInit();

#ifdef HI_LOADER_RECOVERY
    PNG_DRV_ModInit();
#else
    I2C_DRV_ModInit();

    TUNER_DRV_ModInit();

    DMX_DRV_ModInit();

    OTP_DRV_ModInit();
#endif

#if !defined(HI_LOADER_APPLOADER) && !defined(HI_LOADER_RECOVERY)
    PMOC_DRV_ModInit();

    AVPLAY_DRV_ModInit();

    PVR_DRV_ModInit();

    SYNC_DRV_ModInit();

    ADSP_DRV_ModInit();

    AIAO_DRV_ModInit();

    ADEC_DRV_ModInit();

    VFMW_DRV_ModInit();
    SVDEC_ModeInit();

    VDEC_DRV_ModInit();
	
#if 1 //def ANDROID
    OMXVDEC_DRV_ModInit();
#endif

    VPSS_DRV_ModInit();

    WDG_DRV_ModInit();

    MCE_DRV_ModInit();

    PNG_DRV_ModInit();

    JPGE_DRV_ModInit();
#endif

#ifdef HI_HDCP_SUPPORT
    HDCP_DRV_ModInit();
#endif

#ifdef HI_KEYLED_SUPPORT
    KEYLED_DRV_ModInit();
#endif

#ifdef HI_SCI_SUPPORT
    SCI_DRV_ModInit();
#endif

#ifdef HI_AENC_SUPPORT
    AENC_DRV_ModInit();
#endif

#ifdef HI_VENC_SUPPORT
    VENC_DRV_ModInit();
#endif

#ifdef HI_VI_SUPPORT
    VI_DRV_ModInit();
#endif
#endif

    printk("<-- HI_DRV_LoadModules");

    return HI_SUCCESS;
}

late_initcall(HI_DRV_LoadModules);
#endif

