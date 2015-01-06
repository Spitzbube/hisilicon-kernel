#ifndef __DRV_HIFB_EXT_H__
#define __DRV_HIFB_EXT_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "hi_type.h"

typedef HI_VOID  (*FN_HIFB_SetLogoLayerEnable)(HI_BOOL);


typedef struct
{
    FN_HIFB_SetLogoLayerEnable             pfnHifbSetLogoLayerEnable;
} HIFB_EXPORT_FUNC_S;

HI_S32 HIFB_DRV_ModInit(HI_VOID);
HI_VOID HIFB_DRV_ModExit(HI_VOID);
HI_S32 hifb_init_module_k(HI_VOID);
HI_VOID hifb_cleanup_module_k(HI_VOID);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /*__DRV_HIFB_EXT_H__*/