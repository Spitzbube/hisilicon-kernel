#ifndef __DRV_TUNER_EXT_H__
#define __DRV_TUNER_EXT_H__

#include "hi_type.h"

#include "hi_unf_frontend.h"
#include "hi_drv_dev.h"

typedef HI_S32 (*FN_TUNER_OPEN)(struct inode *inode, struct file *filp);
typedef long (*FN_TUNER_IOCTL)(struct file *filp, HI_U32 cmd, unsigned long arg);
typedef HI_S32 (*FN_TUNER_RELEASE)(struct inode *inode, struct file *filp);
typedef HI_S32 (*FN_TUNER_SUSPEND)(PM_BASEDEV_S *pdev, pm_message_t state);
typedef HI_S32 (*FN_TUNER_RESUME)(PM_BASEDEV_S *pdev);

typedef HI_S32 (*FN_TUNER_PROC_READ)(struct seq_file *p, HI_VOID *v);
typedef HI_S32 (*FN_TUNER_PROC_READ_REG)(struct seq_file * p, HI_VOID * v);
typedef HI_S32 (*FN_TUNER_DRV_Init)(HI_VOID);
typedef HI_VOID  (*FN_TUNER_DRV_Exit)(HI_VOID);

typedef struct
{
    FN_TUNER_OPEN      pfnTunerOpen;
    FN_TUNER_IOCTL      pfnTunerIoctl;
    FN_TUNER_RELEASE     pfnTunerRelease;
    FN_TUNER_SUSPEND   pfnTunerSuspend;
    FN_TUNER_RESUME    pfnTunerResume;
    FN_TUNER_PROC_READ pfnTunerProcRead;
    FN_TUNER_PROC_READ_REG pfnTunerProcReadReg;
    FN_TUNER_DRV_Init pfnTunerDrvInit;
    FN_TUNER_DRV_Exit pfnTunerDrvExit;
} TUNER_EXPORT_FUNC_S;

HI_S32 TUNER_DRV_ModInit(HI_VOID);
HI_VOID TUNER_DRV_ModExit(HI_VOID);

#endif

