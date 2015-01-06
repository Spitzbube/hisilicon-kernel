#ifndef __HI_DRV_PDM_H__
#define __HI_DRV_PDM_H__

#include "hi_type.h"
#include "hi_unf_disp.h"
#include "hi_unf_common.h"
#include "hi_go_surface.h"
#include "hi_unf_mce.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MCE_DEF_BASEPARAM_SIZE       (8*1024)
#define MCE_DEF_LOGOPARAM_SIZE		 (8*1024)
#define MCE_DEF_PLAYPARAM_SIZE		 (8*1024)


#define PARSER_BASE_FLASH_NAME         "baseparam"
#define PARSER_LOGO_FLASH_NAME         "logo"
#define PARSER_FASTPLAY_FLASH_NAME     "fastplay"

#define MCE_BASE_TABLENAME_DISP0          "BASE_TABLE_DISP0"
#define MCE_BASE_TABLENAME_DISP1          "BASE_TABLE_DISP1"

#define MCE_BASE_KEYNAME_FMT            "BASE_KEY_FMT"
#define MCE_BASE_KEYNAME_INTF_HDMI      "BASE_KEY_HDMI"
#define MCE_BASE_KEYNAME_INTF_YPBPR     "BASE_KEY_YPBPR"
#define MCE_BASE_KEYNAME_INTF_CVBS      "BASE_KEY_CVBS"
#define MCE_BASE_KEYNAME_INTF_RGB	    "BASE_KEY_RGB"
#define MCE_BASE_KEYNAME_INTF_SVIDEO    "BASE_KEY_SVIDEO"
#define MCE_BASE_KEYNAME_PF             "BASE_KEY_PF"
#define MCE_BASE_KEYNAME_DISPW          "BASE_KEY_DISPW"
#define MCE_BASE_KEYNAME_DISPH          "BASE_KEY_DISPH"
#define MCE_BASE_KEYNAME_SCRX           "BASE_KEY_SCRX"
#define MCE_BASE_KEYNAME_SCRY           "BASE_KEY_SCRY"
#define MCE_BASE_KEYNAME_SCRW           "BASE_KEY_SCRW"
#define MCE_BASE_KEYNAME_SCRH           "BASE_KEY_SCRH"
#define MCE_BASE_KEYNAME_HULEP          "BASE_KEY_HULEP"
#define MCE_BASE_KEYNAME_SATU           "BASE_KEY_SATU"
#define MCE_BASE_KEYNAME_CONTR          "BASE_KEY_CONTR"
#define MCE_BASE_KEYNAME_BRIG           "BASE_KEY_BRIG"
#define MCE_BASE_KEYNAME_BGCOLOR        "BASE_KEY_BGCOLOR"
#define MCE_BASE_KEYNAME_MACRSN         "BASE_KEY_MACRSN"
#define MCE_BASE_KEYNAME_TIMING         "BASE_KEY_TIMING"
#define MCE_BASE_KEYNAME_GAMA           "BASE_KEY_GAMA"
#define MCE_BASE_KEYNAME_ASPECT         "BASE_KEY_ASPECT"

#define MCE_BASE_KEYNAME_SRC_DISP       "BASE_KEY_SOURCE_DISP"
#define MCE_BASE_KEYNAME_VIRSCW         "BASE_KEY_VIRSCW"
#define MCE_BASE_KEYNAME_VIRSCH         "BASE_KEY_VIRSCH"
#define MCE_BASE_KEYNAME_DISP_L         "BASE_KEY_DISP_L"
#define MCE_BASE_KEYNAME_DISP_T         "BASE_KEY_DISP_T"
#define MCE_BASE_KEYNAME_DISP_R         "BASE_KEY_DISP_R"
#define MCE_BASE_KEYNAME_DISP_B         "BASE_KEY_DISP_B"
#define MCE_LOGO_TABLENAME              "LOGO_TABLE"
#define MCE_LOGO_KEYNAME_FLAG           "LOGO_KEY_FLAG"
#define MCE_LOGO_KEYNAME_CONTLEN        "LOGO_KEY_LEN"

#define MCE_PLAY_TABLENAME              "PLAY_TABLE"
#define MCE_PLAY_KEYNAME_FLAG           "PLAY_KEY_FLAG"
#define MCE_PLAY_KEYNAME_DATALEN        "PLAY_KEY_DATALEN"
#define MCE_PLAY_KEYNAME_PARAM          "PLAY_KEY_PARAM"

#if 0 
#define HI_FATAL_PDM(format, arg...)    printk( "%s,%d: " format , __FUNCTION__, __LINE__, ## arg)
#define HI_ERR_PDM(format, arg...)      printk( "%s,%d: " format , __FUNCTION__, __LINE__, ## arg)
#define HI_WARN_PDM(format, arg...)     //printk( "%s,%d: " format , __FUNCTION__, __LINE__, ## arg)
#define HI_INFO_PDM(format, arg...)     //printk( "%s,%d: " format , __FUNCTION__, __LINE__, ## arg)
#else
#define HI_FATAL_PDM(format...)    HI_FATAL_PRINT(HI_ID_PDM, format)
#define HI_ERR_PDM(format...)      HI_ERR_PRINT(HI_ID_PDM, format)
#define HI_WARN_PDM(format...)     HI_WARN_PRINT(HI_ID_PDM, format)
#define HI_INFO_PDM(format...)     HI_INFO_PRINT(HI_ID_PDM, format)
#endif


#define PDM_BASEPARAM_BUFNAME "baseparam"
#define PDM_PLAYPARAM_BUFNAME "playparam"
#define PDM_PLAYDATA_BUFNAME  "playdata"


typedef struct hiDISP_PARAM_S
{
    HI_UNF_DISP_E				enSrcDisp;
    HI_UNF_ENC_FMT_E            enFormat;   
    HI_U32                      u32Brightness;
    HI_U32                      u32Contrast;
    HI_U32                      u32Saturation;
    HI_U32                      u32HuePlus;
    HI_BOOL                     bGammaEnable; 
    HI_U32                      u32VirtScreenWidth;
    HI_U32                      u32VirtScreenHeight; 
    HI_UNF_DISP_OFFSET_S        stOffsetInfo;
    HI_UNF_DISP_BG_COLOR_S      stBgColor;
    HI_UNF_DISP_ASPECT_RATIO_S  stAspectRatio;
    HI_UNF_DISP_INTF_S          stIntf[HI_UNF_DISP_INTF_TYPE_BUTT];
    HI_UNF_DISP_TIMING_S        stDispTiming;
	HIGO_PF_E                   enPixelFormat;
}HI_DISP_PARAM_S;

typedef struct hiLOGO_PARAM_S
{
    HI_U32                      u32CheckFlag; 
    HI_U32                      u32LogoLen;
}HI_LOGO_PARAM_S;

typedef struct hiMCE_PARAM_S
{
    HI_U32                      u32CheckFlag;
    HI_U32                      u32PlayDataLen;
    HI_UNF_MCE_PLAY_PARAM_S     stPlayParam;
}HI_MCE_PARAM_S;

#define HI_FLASH_NAME_LEN   32

/*Flash pation info for mce use*/
typedef struct hiPDM_FLASH_INFO_S
{
    HI_CHAR             Name[HI_FLASH_NAME_LEN];
    HI_U32              u32Offset;
    HI_U32              u32Size;
	HI_BOOL				bShared;  /*whether share one partition with other data*/
}PDM_FLASH_INFO_S;

HI_S32 HI_DRV_PDM_Init(HI_VOID);
HI_S32 HI_DRV_PDM_DeInit(HI_VOID);
HI_S32 HI_DRV_PDM_GetDispParam(HI_UNF_DISP_E enDisp, HI_DISP_PARAM_S *pstDispParam);
HI_S32 HI_DRV_PDM_GetMceParam(HI_MCE_PARAM_S *pMceParam);
HI_S32 HI_DRV_PDM_GetMceData(HI_U32 u32Size, HI_U32 *pAddr);
HI_S32 HI_DRV_PDM_ReleaseReserveMem(const HI_CHAR *BufName);
HI_S32 HI_DRV_PDM_GetData(const HI_CHAR *BufName, HI_U32 *pu32DataAddr, HI_U32 *pu32DataLen);

/************** reserve mem ***************/
//void pdm_reserve_mem(void);


#ifdef __cplusplus
}
#endif

#endif


