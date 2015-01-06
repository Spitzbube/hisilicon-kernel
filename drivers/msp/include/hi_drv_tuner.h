#ifndef __HI_DRV_TUNER_H__
#define __HI_DRV_TUNER_H__

#include "hi_type.h"
#include "hi_drv_i2c.h"
#include "hi_debug.h"
#include "hi_drv_struct.h"
#include "drv_i2c_ext.h"
#include "hi_unf_frontend.h"

#define HI_FATAL_TUNER(fmt...) \
            HI_FATAL_PRINT(HI_ID_TUNER, fmt)

#define HI_ERR_TUNER(fmt...) \
            HI_ERR_PRINT(HI_ID_TUNER, fmt)

#define HI_WARN_TUNER(fmt...) \
            HI_WARN_PRINT(HI_ID_TUNER, fmt)

#define HI_INFO_TUNER(fmt...) \
            HI_INFO_PRINT(HI_ID_TUNER, fmt)

HI_S32 HI_DRV_TUNER_Init(HI_VOID);
HI_VOID HI_DRV_TUNER_DeInit(HI_VOID);
HI_S32 HI_DRV_TUNER_GetDeftAttr(HI_U32  u32tunerId , HI_UNF_TUNER_ATTR_S *pstTunerAttr );
HI_S32 HI_DRV_TUNER_SetAttr(HI_U32  u32tunerId , const HI_UNF_TUNER_ATTR_S *pstTunerAttr );
HI_S32 HI_DRV_TUNER_SetSatAttr(HI_U32	u32tunerId , const HI_UNF_TUNER_SAT_ATTR_S *pstSatTunerAttr);
HI_S32 HI_DRV_TUNER_Open (HI_U32    u32tunerId);
HI_S32 HI_DRV_TUNER_Close(HI_U32    u32tunerId);
HI_S32 HI_DRV_TUNER_Connect(HI_U32  u32tunerId , const HI_UNF_TUNER_CONNECT_PARA_S  *pstConnectPara,HI_U32 u32TimeOut);
HI_S32 HI_DRV_TUNER_GetStatus(HI_U32    u32tunerId , HI_UNF_TUNER_STATUS_S  *pstTunerStatus);

HI_S32 HI_DRV_TUNER_SetLNBConfig( HI_U32 u32TunerId, const HI_UNF_TUNER_FE_LNB_CONFIG_S *pstLNB);
HI_S32 HI_DRV_TUNER_SetLNBPower(HI_U32 u32TunerId, HI_UNF_TUNER_FE_LNB_POWER_E enLNBPower);
HI_S32 HI_DRV_TUNER_Switch22K(HI_U32 u32TunerId, HI_UNF_TUNER_SWITCH_22K_E enPort);
HI_S32 HI_DRV_TUNER_DISEQC_Switch4Port(HI_U32 u32TunerId, const HI_UNF_TUNER_DISEQC_SWITCH4PORT_S* pstPara);
HI_S32 HI_DRV_TUNER_DISEQC_Switch16Port(HI_U32 u32TunerId, const HI_UNF_TUNER_DISEQC_SWITCH16PORT_S* pstPara);

#endif

