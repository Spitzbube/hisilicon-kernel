#ifndef __DISP_HAL_ADP_H
#define __DISP_HAL_ADP_H

HI_S32 Get_FormatCfgPara(HI_U32 index,DISP_FMT_CFG_S *pstDispFormatParam);
#ifndef __DISP_PLATFORM_BOOT__

HI_S32 Chip_Specific_WbcZmeFunc(DISP_WBC_E eWbc,
                            ALG_VZME_DRV_PARA_S *stZmeI,
                            ALG_VZME_RTL_PARA_S *stZmeO); 
#endif
HI_S32 Chip_Specific_Set3DMode(HI_DRV_DISPLAY_E eChn, HI_U32 u32DispId, DISP_FMT_CFG_S *pstCfg);
HI_S32 Chip_Specific_SetWbc3DInfo(DISP_WBC_E eWbc, HI_DISP_DISPLAY_INFO_S *pstDispInfo, HI_RECT_S *in);
HI_S32 Chip_Specific_ReviseWbcZmeInput(DISP_WBC_E eWbc,HI_DISP_DISPLAY_INFO_S *pstDispInfo, HI_U32 *u32Width,HI_U32 *u32Height);                            
HI_S32 Chip_Specific_DispSetMSChnEnable(HI_U32 u32DispIdM, HI_U32 u32DispIdS, HI_U32 u32DelayMs, HI_BOOL bEnalbe);
HI_S32 Chip_Specific_DispSelectPll(HI_DRV_DISPLAY_E eChn, HI_U32 uPllIndex);
HI_VOID  Chip_Specific_DispSetPll(DISP_PLL_SOURCE_E enPllIndex,HI_U32 u32PllxReg0,HI_U32 u32PllxReg1);
HI_S32 Chip_Specific_CBM_MovTOP(HI_DRV_DISPLAY_E enDisp, HI_DRV_DISP_LAYER_E enLayer);
HI_VOID Chip_Specific_DispSetChanClk(HI_DRV_DISPLAY_E eChn, DISP_FMT_CFG_S *pstCfg);


#endif
