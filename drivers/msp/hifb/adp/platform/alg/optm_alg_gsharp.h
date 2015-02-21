#ifndef __OPTM_HIFB_ALG_GSHARP_H__
#define __OPTM_HIFB_ALG_GSHARP_H__

#include "hi_type.h" 

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#define OPTM_ALG_MIN2(x, y) (((x) < (y)) ? (x) : (y))
#define OPTM_ALG_MAX2(x, y) (((x) > (y)) ? (x) : (y))


typedef struct
{
    HI_U32 u32ZmeFrmWIn;    /*zme input frame width*/
    HI_U32 u32ZmeFrmHIn;    /*zme input frame height*/
    HI_U32 u32ZmeFrmWOut;   /*zme output frame width*/
    HI_U32 u32ZmeFrmHOut;   /*zme output frame height*/

}OPTM_ALG_GDTI_DRV_PARA_S;


typedef struct
{

    /*LTI*/
    HI_BOOL bEnLTI;      //lti_en
    HI_S32  s32LTIHPTmp[5];//lhpass_coef
    HI_S16  s16LTICompsatRatio;//lgain_ratio
    HI_U16  u16LTICoringThrsh;//lcoring_thd
    HI_U16  u16LTIUnderSwingThrsh;//lunder_swing
    HI_U16  u16LTIOverSwingThrsh;//lover_swing
    HI_U8   u8LTIMixingRatio;//lmixing_ratio

    HI_U32  u32LTIHFreqThrsh[2];//lhfreq_thd
    HI_U32   u32LTICompsatMuti[3];//lgain_coef

    /*CTI*/
    HI_BOOL bEnCTI;
    HI_S32   s32CTIHPTmp[3];
    HI_S16  s16CTICompsatRatio;
    HI_U16  u16CTICoringThrsh;
    HI_U16  u16CTIUnderSwingThrsh;
    HI_U16  u16CTIOverSwingThrsh;
    HI_U8   u8CTIMixingRatio;
}OPTM_ALG_GDTI_RTL_PARA_S;


HI_VOID OPTM_ALG_GDtiInit(OPTM_ALG_GDTI_DRV_PARA_S *pstDtiDrvPara, OPTM_ALG_GDTI_RTL_PARA_S *pstDtiRtlPara);
HI_VOID OPTM_ALG_GDtiSet(OPTM_ALG_GDTI_DRV_PARA_S *pstDtiDrvPara, OPTM_ALG_GDTI_RTL_PARA_S *pstDtiRtlPara);

#ifdef __cplusplus
#if __cplusplus
}
#endif	
#endif /* __cplusplus */

#endif	/* __OPTM_HIFB_H__ */


