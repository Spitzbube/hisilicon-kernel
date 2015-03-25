#include "si_typedefs.h"
#include "hi_drv_hdmi.h"


#ifndef __TX_PHY_H__
#define __TX_PHY_H__


//#define BOARD_TYPE_S40V2_fpga

#define HDMI_TX_PHY_ADDR 0xf8ce1800L

HI_S32 SI_TX_PHY_WriteRegister(HI_U32 u32RegAddr, HI_U32 u32Value);
HI_S32 SI_TX_PHY_ReadRegister(HI_U32 u32RegAddr, HI_U32 *pu32Value);

HI_S32 SI_TX_PHY_HighBandwidth(HI_BOOL bTermEn);

HI_S32 SI_TX_PHY_GetOutPutEnable(void);

HI_S32 SI_TX_PHY_DisableHdmiOutput(void);
HI_S32 SI_TX_PHY_EnableHdmiOutput(void);

void SI_TX_PHY_INIT(void);

HI_S32 SI_TX_PHY_PowerDown(HI_BOOL bPwdown);

HI_S32 SI_TX_PHY_SetDeepColor(HI_U8 bDeepColor);

#endif 

