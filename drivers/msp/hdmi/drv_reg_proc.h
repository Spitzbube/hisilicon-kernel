
#ifndef __DRV_REG_PROC_H__
#define __DRV_REG_PROC_H__
#include "hi_unf_hdmi.h"
#include "hi_drv_hdmi.h"
#include "hi_drv_disp.h"
#include "si_hlviic.h"
#include "si_defstx.h"
#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */



HI_S32 DRV_HDMI_WriteRegister(HI_U32 u32RegAddr, HI_U32 u32Value);
HI_S32 DRV_HDMI_ReadRegister(HI_U32 u32RegAddr, HI_U32 *pu32Value);

HI_U8 DRV_ReadByte_8BA(HI_U8 Bus, HI_U8 SlaveAddr, HI_U8 RegAddr);
void DRV_WriteByte_8BA(HI_U8 Bus, HI_U8 SlaveAddr, HI_U8 RegAddr, HI_U8 Data);
HI_U16 DRV_ReadWord_8BA(HI_U8 Bus, HI_U8 SlaveAddr, HI_U8 RegAddr);
HI_U8 DRV_ReadByte_16BA(HI_U8 Bus, HI_U8 SlaveAddr, HI_U16 RegAddr);
HI_U16 DRV_ReadWord_16BA(HI_U8 Bus, HI_U8 SlaveAddr, HI_U16 RegAddr);
void DRV_WriteWord_8BA(HI_U8 Bus, HI_U8 SlaveAddr, HI_U8 RegAddr, HI_U16 Data);
void DRV_WriteWord_16BA(HI_U8 Bus, HI_U8 SlaveAddr, HI_U16 RegAddr, HI_U16 Data);
void DRV_WriteByte_16BA(HI_U8 Bus, HI_U8 SlaveAddr, HI_U16 RegAddr, HI_U8 Data);
HI_S32 DRV_BlockRead_8BAS(I2CShortCommandType * I2CCommand, HI_U8 * Data);
HI_U8 DRV_BlockWrite_8BAS( I2CShortCommandType * I2CCommand, HI_U8 * Data );
//HI_U8 DRV_HDMI_HWReset(HI_U32 u32Enable);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __DRV_REG_PROC_H__ */

