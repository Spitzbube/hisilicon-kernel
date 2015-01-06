
#ifndef TDE_EXT
#define TDE_EXT

#include "hi_type.h"
#include "hi_tde_type.h"

typedef HI_S32  (*FN_TDE_Open)(HI_VOID);
typedef HI_S32  (*FN_TDE_Close)(HI_VOID);
typedef HI_S32  (*FN_TDE_BeginJob)(TDE_HANDLE *);
typedef HI_S32 (*FN_TDE_EndJob)(TDE_HANDLE, HI_BOOL, HI_U32, HI_BOOL, TDE_FUNC_CB, HI_VOID*);
typedef HI_S32  (*FN_TDE_CancelJob)(TDE_HANDLE);
typedef HI_S32  (*FN_TDE_WaitForDone)(TDE_HANDLE, HI_U32);
typedef HI_S32  (*FN_TDE_WaitAllDone)(HI_BOOL);
typedef HI_S32  (*FN_TDE_QuickCopy)(TDE_HANDLE, TDE2_SURFACE_S*, TDE2_RECT_S*, TDE2_SURFACE_S*, TDE2_RECT_S*);
typedef HI_S32  (*FN_TDE_QuickFill)(TDE_HANDLE, TDE2_SURFACE_S*, TDE2_RECT_S*, HI_U32);
typedef HI_S32  (*FN_TDE_QuickResize)(TDE_HANDLE, TDE2_SURFACE_S*, TDE2_RECT_S*, TDE2_SURFACE_S*, TDE2_RECT_S*);
typedef HI_S32  (*FN_TDE_QuickFlicker)(TDE_HANDLE, TDE2_SURFACE_S*, TDE2_RECT_S*, TDE2_SURFACE_S*, TDE2_RECT_S*);
typedef HI_S32  (*FN_TDE_Blit)(TDE_HANDLE, TDE2_SURFACE_S*, TDE2_RECT_S*, TDE2_SURFACE_S*, TDE2_RECT_S*, TDE2_SURFACE_S*,
                          TDE2_RECT_S*, TDE2_OPT_S*);
typedef HI_S32  (*FN_TDE_MbBlit)(TDE_HANDLE, TDE2_MB_S*, TDE2_RECT_S*, TDE2_SURFACE_S*, TDE2_RECT_S*, TDE2_MBOPT_S*);
typedef HI_S32  (*FN_TDE_SolidDraw)(TDE_HANDLE, TDE2_SURFACE_S*, TDE2_RECT_S*, TDE2_SURFACE_S*, TDE2_RECT_S*, TDE2_FILLCOLOR_S*, TDE2_OPT_S*);
typedef HI_S32  (*FN_TDE_SetDeflickerLevel)(TDE_DEFLICKER_LEVEL_E);
typedef HI_S32  (*FN_TDE_EnableRegionDeflicker)(HI_BOOL);
typedef HI_S32  (*FN_TDE_CalScaleRect)(const TDE2_RECT_S*, const TDE2_RECT_S*, TDE2_RECT_S*, TDE2_RECT_S*);


typedef struct
{
    FN_TDE_Open             pfnTdeOpen;
    FN_TDE_Close            pfnTdeClose;
    FN_TDE_BeginJob         pfnTdeBeginJob;
    FN_TDE_EndJob           pfnTdeEndJob;
    FN_TDE_CancelJob        pfnTdeCancelJob;
    FN_TDE_WaitForDone      pfnTdeWaitForDone;
    FN_TDE_WaitAllDone      pfnTdeWaitAllDone;
    FN_TDE_QuickCopy        pfnTdeQuickCopy;    
    FN_TDE_QuickFill        pfnTdeQuickFill;
    FN_TDE_QuickResize      pfnTdeQuickResize;
    FN_TDE_QuickFlicker     pfnTdeQuickFlicker;
    FN_TDE_Blit             pfnTdeBlit;
    FN_TDE_MbBlit           pfnTdeMbBlit;
    FN_TDE_SolidDraw        pfnTdeSolidDraw;
    FN_TDE_SetDeflickerLevel        pfnTdeSetDeflickerLevel;
    FN_TDE_EnableRegionDeflicker    pfnTdeEnableRegionDeflicker;    
    FN_TDE_CalScaleRect     pfnTdeCalScaleRect;
} TDE_EXPORT_FUNC_S;

HI_S32 TDE_DRV_ModInit(HI_VOID);
HI_VOID  TDE_DRV_ModExit(HI_VOID);
HI_S32 tde_init_module_k(HI_VOID);
HI_VOID tde_cleanup_module_k(HI_VOID);

#endif


