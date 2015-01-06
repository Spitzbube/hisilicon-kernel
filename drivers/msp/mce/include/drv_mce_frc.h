#ifndef __MCE_FRC_H__
#define __MCE_FRC_H__

#include "hi_type.h"
#include "drv_mce_avplay.h"
//#include "drv_avplay_ioctl.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AVPLAY_ALG_FRC_PRECISION  32
#define AVPLAY_ALG_FRC_BASE_PLAY_RATIO  256

/* maximum display rate:16 */
#define AVPLAY_ALG_FRC_MAX_PLAY_RATIO   AVPLAY_ALG_FRC_BASE_PLAY_RATIO * 256

/* minimum display rate: 1/16 */
#define AVPLAY_ALG_FRC_MIN_PLAY_RATIO   1

#define ABS(x) (((x) < 0) ? -(x) : (x))
#define AVPLAY_ALG_ABS(x)     (((x) < 0) ? -(x) : (x))

HI_S32 MCE_FrcCreate(MCE_AVPLAY_S *pAvplay);
HI_S32 MCE_FrcDestroy(MCE_AVPLAY_S *pAvplay);
HI_S32 MCE_FrcReset(AVPLAY_ALG_FRC_S *hFrc);
HI_VOID MCE_FrcCalculate(AVPLAY_ALG_FRC_S *hFrc, AVPLAY_FRC_CFG_S *pstFrcCfg, AVPLAY_FRC_CTRL_S *pstFrcCtrl);

#ifdef __cplusplus
}
#endif

#endif

