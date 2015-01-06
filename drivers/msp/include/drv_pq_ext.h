#ifndef __DRV_PQ_EXT_H__
#define __DRV_PQ_EXT_H__

#include "hi_type.h"
#include "drv_pq_define.h"
#ifdef __cplusplus
extern "C" {
#endif


#define PQ_DEF_NAME         "pqparam"
#define MCE_DEF_PQPARAM_SIZE       (2*1024*1024)

#if 0
#define HI_FATAL_PQ(format, arg...)    printk( "%s,%d: " format , __FUNCTION__, __LINE__, ## arg)
#define HI_ERR_PQ(format, arg...)      printk( "%s,%d: " format , __FUNCTION__, __LINE__, ## arg)
#define HI_WARN_PQ(format, arg...)     printk( "%s,%d: " format , __FUNCTION__, __LINE__, ## arg)
#define HI_INFO_PQ(format, arg...)     printk( "%s,%d: " format , __FUNCTION__, __LINE__, ## arg)
#else
#define HI_FATAL_PQ(format...)    HI_FATAL_PRINT(HI_ID_PQ, format)
#define HI_ERR_PQ(format...)      HI_ERR_PRINT(HI_ID_PQ, format)
#define HI_WARN_PQ(format...)     HI_WARN_PRINT(HI_ID_PQ, format)
#define HI_INFO_PQ(format...)     HI_INFO_PRINT(HI_ID_PQ, format)
#endif

typedef HI_S32 (*FN_PQ_GetPqParam)(PQ_PARAM_S ** pstPqParam);

typedef struct tagPQ_EXPORT_FUNC_S
{
    FN_PQ_GetPqParam               pfnPQ_GetPqParam;    
}PQ_EXPORT_FUNC_S;

HI_S32 PQ_DRV_ModInit(HI_VOID);
HI_VOID PQ_DRV_ModExit(HI_VOID);
HI_S32 HI_DRV_PQ_Init(HI_VOID);

#ifdef __cplusplus
}
#endif

#endif