#ifndef __DRV_PQ_H__
#define __DRV_PQ_H__

#include "hi_type.h"
#include "drv_pq_ext.h"
#include "drv_pq_define.h"

#ifdef __cplusplus
extern "C" {
#endif


HI_VOID PQ_DRV_GetOption(PQ_IO_S* arg);
HI_VOID PQ_DRV_SetOption(PQ_IO_S* arg);
HI_VOID PQ_DRV_GetDeiParam(PQ_IO_S* arg);
HI_VOID PQ_DRV_SetDeiParam(PQ_IO_S* arg);
HI_VOID PQ_DRV_GetFmdParam(PQ_IO_S* arg);
HI_VOID PQ_DRV_SetFmdParam(PQ_IO_S* arg);
HI_VOID PQ_DRV_GetDnrParam(PQ_IO_S* arg);
HI_VOID PQ_DRV_SetDnrParam(PQ_IO_S* arg);
HI_VOID PQ_DRV_GetSharpParam(PQ_IO_S* arg);
HI_VOID PQ_DRV_SetVpssSharpParam(PQ_IO_S* arg);
HI_VOID PQ_DRV_SetGfxSharpParam(PQ_IO_S* arg);
HI_VOID PQ_DRV_GetCscParam(PQ_IO_S* arg);
HI_VOID PQ_DRV_SetVoCscParam(PQ_IO_S* arg);
HI_VOID PQ_DRV_SetDispCscParam(PQ_IO_S* arg);
HI_VOID PQ_DRV_SetGfxCscParam(PQ_IO_S* arg);
HI_VOID PQ_DRV_GetAccParam(PQ_IO_S* arg);
HI_VOID PQ_DRV_SetAccParam(PQ_IO_S* arg);
HI_VOID PQ_DRV_GetAcmParam(PQ_IO_S* arg);
HI_VOID PQ_DRV_SetAcmParam(PQ_IO_S* arg);
HI_VOID PQ_DRV_GetGammaParam(PQ_IO_S* arg);
HI_VOID PQ_DRV_SetGammaParam(PQ_IO_S* arg);
HI_VOID PQ_DRV_GetGammaCtrlParam(PQ_IO_S* arg);
HI_VOID PQ_DRV_SetGammaCtrlParam(PQ_IO_S* arg);
HI_VOID PQ_DRV_GetDitherParam(PQ_IO_S* arg);
HI_VOID PQ_DRV_SetDitherParam(PQ_IO_S* arg);
HI_S32 PQ_DRV_FixParaToFlash(PQ_IO_S* arg);
HI_S32 PQ_DRV_UpdateVpss(HI_U32 u32UpdateType, PQ_PARAM_S* pstPqParam);
HI_S32 PQ_DRV_UpdateVo(HI_U32 u32UpdateType, PQ_PARAM_S* pstPqParam);
HI_S32 PQ_DRV_UpdateDisp(HI_U32 u32UpdateType, PQ_PARAM_S* pstPqParam);
HI_S32 PQ_DRV_UpdateGfx(HI_U32 u32UpdateType, PQ_PARAM_S* pstPqParam);
HI_S32 PQ_DRV_GetPqParam(PQ_PARAM_S** pstPqParam);
HI_S32 PQ_DRV_SetPqParam(PQ_PARAM_S* pstPqParam);
//HI_S32 PQ_DRV_GetFlashPqBin(PQ_PARAM_S* pstPqParam);
HI_S32 PQ_DRV_CopyPqBinToPqApp(PQ_PARAM_S* pstPqParam);


typedef struct
{
	HI_U16 wData_0; //0
	HI_U16 wData_2; //2
	HI_U16 wData_4; //4
	HI_U16 wData_6; //6
	HI_U16 wData_8; //8
	HI_U16 wData_10; //10
	HI_U16 wData_12; //12
	HI_U16 wData_14; //14
	HI_U16 wData_16; //16
	HI_U16 wData_18; //18
	//20??
} DRV_PQ_GetPictureSettingStruct;

HI_S32 DRV_PQ_GetHDPictureSetting(DRV_PQ_GetPictureSettingStruct* a);
HI_S32 DRV_PQ_GetSDPictureSetting(DRV_PQ_GetPictureSettingStruct* a);

typedef struct
{
	HI_U16 wData_0; //0
	HI_U16 wData_2; //2
	HI_U16 wData_4; //4
	HI_U16 wData_6; //6

} PQ_MNG_GetPictureSettingStruct1;

typedef struct
{
	HI_S16 wData_0; //0
	HI_S16 wData_2; //2
	HI_S16 wData_4; //4
	HI_S16 wData_6; //6
	HI_S16 wData_8; //8
	HI_S16 wData_10; //10

} PQ_MNG_GetPictureSettingStruct2;

HI_S32 PQ_MNG_SetSDPictureSetting(PQ_MNG_GetPictureSettingStruct1* a, PQ_MNG_GetPictureSettingStruct2* b);
HI_S32 PQ_MNG_SetHDPictureSetting(PQ_MNG_GetPictureSettingStruct1* a, PQ_MNG_GetPictureSettingStruct2* b);
HI_S32 PQ_MNG_GetHDPictureSetting(PQ_MNG_GetPictureSettingStruct1* a, PQ_MNG_GetPictureSettingStruct2* b);
HI_S32 PQ_MNG_GetSDPictureSetting(PQ_MNG_GetPictureSettingStruct1* a, PQ_MNG_GetPictureSettingStruct2* b);
HI_U32 PQ_MNG_CheckSourceMode(int a, int b);
HI_U32 PQ_MNG_CheckOutputMode(int a);
HI_S32 PQ_MNG_LoadPhyList(int a, int b, int c, int d);
HI_VOID PQ_MNG_EnableSharpen(HI_U8 a);
HI_VOID PQ_MNG_EnableDCI(HI_U32 a);
HI_VOID PQ_MNG_EnableColorEnhance(HI_U32 a);
HI_VOID DRV_PQ_SetDemoMode(int a, int b);
HI_VOID PQ_MNG_EnableSharpenDemo(int a);
HI_VOID PQ_MNG_ColorDemoMode(int a);
HI_VOID PQ_MNG_DCIDemoMode(int a);
HI_S32 PQ_MNG_SetDCILevelGain(int a, int b);
HI_S32 PQ_MNG_SetFleshToneLevel(int a, int b);
HI_S32 PQ_MNG_SetSixBaseColorLevel(int* a, int b);
HI_S32 PQ_MNG_SetColorEnhanceMode(int a, int b);

typedef struct
{
	int fill_0; //0
	int Data_4; //4
	int Data_8; //8
	int fill_12[5]; //12
	int Data_32; //32
	int Data_36; //36
	int Data_40; //40
} DRV_PQ_UpdateVdpPQ_Struct;

HI_S32 DRV_PQ_UpdateVdpPQ(int a, DRV_PQ_UpdateVdpPQ_Struct* b, int c);


HI_S32 PQ_HAL_Init(HI_VOID);
HI_S32 PQ_HAL_UpdateVdpReg(HI_U32 addr);
HI_VOID PQ_HAL_UpdateDCICoef(int a);
HI_VOID PQ_HAL_UpdatACMCoef(int a);
HI_VOID PQ_HAL_UpdateSharpCofe(int a);


#ifdef __cplusplus
}
#endif

#endif
