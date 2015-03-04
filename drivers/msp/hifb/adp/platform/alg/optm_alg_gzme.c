#include "optm_alg_gzme.h"
#include "optm_alg_zme_coef.h"

#ifdef HI_BUILD_IN_BOOT
#include "hi_drv_pdm.h"
#include "hifb_debug.h"
#else
#include "hifb_drv_common.h"
#endif

extern HI_S32 OPTM_AllocAndMap(const char *bufname, char *zone_name, HI_U32 size, int align, MMZ_BUFFER_S *psMBuf);
extern HI_VOID OPTM_UnmapAndRelease(MMZ_BUFFER_S *psMBuf);

const HI_S16 *g_pOPTMGfxZmeCoef[GZME_COEF_RATIO_BUTT][GZME_COEF_TYPE_BUTT] =
{    //HL                                          VL                                     HC                            VC                         
    {&OPTM_s16ZmeCoef_8T8P_Lanc3[0][0],  &OPTM_s16ZmeCoef_4T16P_6M_a15[0][0],         &OPTM_s16ZmeCoef_8T8P_Lanc3[0][0],  &OPTM_s16ZmeCoef_4T16P_6M_a15[0][0],         }, //>1
    {&OPTM_s16ZmeCoef_8T8P_Cubic[0][0],  &OPTM_s16ZmeCoef_4T16P_Cubic[0][0],          &OPTM_s16ZmeCoef_8T8P_Cubic[0][0],  &OPTM_s16ZmeCoef_4T16P_Cubic[0][0],          }, //==1
    {&OPTM_s16ZmeCoef_8T8P_Lanc3[0][0],  &OPTM_s16ZmeCoef_4T16P_Lanc2_6M_a15[0][0],   &OPTM_s16ZmeCoef_8T8P_Lanc3[0][0],  &OPTM_s16ZmeCoef_4T16P_Lanc2_6M_a15[0][0],   }, //>=0.75   
    {&OPTM_s16ZmeCoef_8T8P_Lanc2[0][0],  &OPTM_s16ZmeCoef_4T16P_Lanc2_5M_a15[0][0],   &OPTM_s16ZmeCoef_8T8P_Lanc2[0][0],  &OPTM_s16ZmeCoef_4T16P_Lanc2_5M_a15[0][0],   }, //>=0.5
    {&OPTM_s16ZmeCoef_8T8P_3M_a19[0][0], &OPTM_s16ZmeCoef_4T16P_Lanc2_4_5M_a15[0][0], &OPTM_s16ZmeCoef_8T8P_3M_a19[0][0], &OPTM_s16ZmeCoef_4T16P_Lanc2_4_5M_a15[0][0], }, //>=0.33 
    {&OPTM_s16ZmeCoef_8T8P_3M_a19[0][0], &OPTM_s16ZmeCoef_4T16P_Lanc2_4M_a15[0][0],   &OPTM_s16ZmeCoef_8T8P_3M_a19[0][0], &OPTM_s16ZmeCoef_4T16P_Lanc2_4M_a15[0][0],   }, //>=0.25 
    {&OPTM_s16ZmeCoef_8T8P_3M_a19[0][0], &OPTM_s16ZmeCoef_4T16P_Lanc2_3M_a13[0][0],   &OPTM_s16ZmeCoef_8T8P_3M_a19[0][0], &OPTM_s16ZmeCoef_4T16P_Lanc2_3M_a13[0][0],   }  //else
};


const HI_S16 *g_pOPTMGfxSDZmeCoef[GZME_COEF_RATIO_BUTT][GZME_COEF_TYPE_BUTT] =
{   //HL                                          VL                            HC                            VC 
    {&OPTM_s16ZmeCoef_8T8P_Lanc3[0][0],  &OPTM_s16ZmeCoef_4T16P_6M_a15[0][0], &OPTM_s16ZmeCoef_8T8P_Lanc3[0][0],  &OPTM_s16ZmeCoef_4T16P_6M_a15[0][0], }, //>1
    {&OPTM_s16ZmeCoef_8T8P_Cubic[0][0],  &OPTM_s16ZmeCoef_4T16P_3M_a13[0][0], &OPTM_s16ZmeCoef_8T8P_Cubic[0][0],  &OPTM_s16ZmeCoef_4T16P_3M_a13[0][0], }, //==1
    {&OPTM_s16ZmeCoef_8T8P_Lanc2[0][0],  &OPTM_s16ZmeCoef_4T16P_3M_a13[0][0], &OPTM_s16ZmeCoef_8T8P_Lanc2[0][0],  &OPTM_s16ZmeCoef_4T16P_3M_a13[0][0], }, //>=0.75   
    {&OPTM_s16ZmeCoef_8T8P_Lanc2[0][0],  &OPTM_s16ZmeCoef_4T16P_3M_a13[0][0], &OPTM_s16ZmeCoef_8T8P_Lanc2[0][0],  &OPTM_s16ZmeCoef_4T16P_3M_a13[0][0], }, //>=0.5
    {&OPTM_s16ZmeCoef_8T8P_3M_a19[0][0], &OPTM_s16ZmeCoef_4T16P_3M_a13[0][0], &OPTM_s16ZmeCoef_8T8P_3M_a19[0][0], &OPTM_s16ZmeCoef_4T16P_3M_a13[0][0], }, //>=0.33 
    {&OPTM_s16ZmeCoef_8T8P_3M_a19[0][0], &OPTM_s16ZmeCoef_4T16P_3M_a13[0][0], &OPTM_s16ZmeCoef_8T8P_3M_a19[0][0], &OPTM_s16ZmeCoef_4T16P_3M_a13[0][0], }, //>=0.25 
    {&OPTM_s16ZmeCoef_8T8P_3M_a19[0][0], &OPTM_s16ZmeCoef_4T16P_3M_a13[0][0], &OPTM_s16ZmeCoef_8T8P_3M_a19[0][0], &OPTM_s16ZmeCoef_4T16P_3M_a13[0][0], }  //else
};


const HI_S16 *g_pOPTMGfxDtiZmeCoef[GZME_COEF_RATIO_BUTT][GDTI_COEF_TYPE_BUTT] =
{   //HLTI                                          VLTI                                     HCTI                            VCTI 
    {&OPTM_s16ZmeCoef_2T8P_Gus2_6_75M_a0_5[0][0], &OPTM_s16ZmeCoef_2T16P_Gus2_6_75M_a0_5[0][0],  &OPTM_s16ZmeCoef_2T8P_4M_0_1[0][0], &OPTM_s16ZmeCoef_2T16P_4M_0_1[0][0],}, //>1
    {&OPTM_s16ZmeCoef_2T8P_Gus2_6_75M_a0_5[0][0], &OPTM_s16ZmeCoef_2T16P_Gus2_6M_a0_5[0][0],     &OPTM_s16ZmeCoef_2T8P_4M_0_1[0][0], &OPTM_s16ZmeCoef_2T16P_4M_0_1[0][0],}, //==1
    {&OPTM_s16ZmeCoef_2T8P_Gus2_6_75M_a0_5[0][0], &OPTM_s16ZmeCoef_2T16P_Gus2_6M_a0_5[0][0],     &OPTM_s16ZmeCoef_2T8P_4M_0_1[0][0], &OPTM_s16ZmeCoef_2T16P_4M_0_1[0][0],}, //>=0.75   
    {&OPTM_s16ZmeCoef_2T8P_Gus2_6_75M_a0_5[0][0], &OPTM_s16ZmeCoef_2T16P_Gus2_6M_a0_5[0][0],     &OPTM_s16ZmeCoef_2T8P_4M_0_1[0][0], &OPTM_s16ZmeCoef_2T16P_4M_0_1[0][0],}, //>=0.5
    {&OPTM_s16ZmeCoef_2T8P_Gus2_6_75M_a0_5[0][0], &OPTM_s16ZmeCoef_2T16P_Gus2_6M_a0_5[0][0],     &OPTM_s16ZmeCoef_2T8P_4M_0_1[0][0], &OPTM_s16ZmeCoef_2T16P_4M_0_1[0][0],}, //>=0.33 
    {&OPTM_s16ZmeCoef_2T8P_Gus2_6_75M_a0_5[0][0], &OPTM_s16ZmeCoef_2T16P_Gus2_6M_a0_5[0][0],     &OPTM_s16ZmeCoef_2T8P_4M_0_1[0][0], &OPTM_s16ZmeCoef_2T16P_4M_0_1[0][0],}, //>=0.25 
    {&OPTM_s16ZmeCoef_2T8P_Gus2_6_75M_a0_5[0][0], &OPTM_s16ZmeCoef_2T16P_Gus2_6M_a0_5[0][0],     &OPTM_s16ZmeCoef_2T8P_4M_0_1[0][0], &OPTM_s16ZmeCoef_2T16P_4M_0_1[0][0],}  //else
};


HI_U32 OPTM_GZmeTransCoefAlign(const HI_S16 *ps16Coef, HI_S16 s16CoefNumber, HI_S16 *pBitCoef)
{
    HI_U32 i;

    for (i = 0; i < s16CoefNumber; i++)
    {
        *pBitCoef++ = *ps16Coef++;
    }

    return (s16CoefNumber * 2);
}


static HI_U32 OPTM_GDtiTransCoefHor(const HI_S16 *ps16LtiCoef, const HI_S16 *ps16CtiCoef,
                               OPTM_ZME_COEF_BITARRAY_S *pBitCoef)
{
    HI_U32 i, u32Cnt,u32Tmp;

    u32Cnt = 3;
    for (i = 0; i < u32Cnt; i++)
    {
    	if(i < 2)
    	{
	    	pBitCoef->stBit[i].bits_0 = OPTM_VouBitValue(*ps16LtiCoef++);
	        pBitCoef->stBit[i].bits_1 = OPTM_VouBitValue(*ps16LtiCoef++);
	        pBitCoef->stBit[i].bits_2 = OPTM_VouBitValue(*ps16CtiCoef++);

	        u32Tmp = OPTM_VouBitValue(*ps16CtiCoef++);
	        pBitCoef->stBit[i].bits_32 = u32Tmp;
	        pBitCoef->stBit[i].bits_38 = (u32Tmp >> 2);

	        pBitCoef->stBit[i].bits_4 = OPTM_VouBitValue(*ps16LtiCoef++);
	        pBitCoef->stBit[i].bits_5 = OPTM_VouBitValue(*ps16LtiCoef++);

	        u32Tmp = OPTM_VouBitValue(*ps16CtiCoef++);
	        pBitCoef->stBit[i].bits_64 = u32Tmp;
	        pBitCoef->stBit[i].bits_66 = u32Tmp >> 4;

	        pBitCoef->stBit[i].bits_7 = OPTM_VouBitValue(*ps16CtiCoef++);
	        pBitCoef->stBit[i].bits_8 = OPTM_VouBitValue(*ps16LtiCoef++);

	        u32Tmp = OPTM_VouBitValue(*ps16LtiCoef++);
	        pBitCoef->stBit[i].bits_96 = u32Tmp;
	        pBitCoef->stBit[i].bits_94 = u32Tmp >> 6;

	        pBitCoef->stBit[i].bits_10 = OPTM_VouBitValue(*ps16CtiCoef++);
	        pBitCoef->stBit[i].bits_11 = OPTM_VouBitValue(*ps16CtiCoef++);
	        pBitCoef->stBit[i].bits_12 = 0;
    	}
    	else
    	{
	    	pBitCoef->stBit[i].bits_0 = OPTM_VouBitValue(*ps16LtiCoef++);
	        pBitCoef->stBit[i].bits_1 = OPTM_VouBitValue(*ps16LtiCoef++);
	        pBitCoef->stBit[i].bits_2 = OPTM_VouBitValue(*ps16CtiCoef++);

	        u32Tmp = OPTM_VouBitValue(*ps16CtiCoef++);
	        pBitCoef->stBit[i].bits_32 = u32Tmp;
	        pBitCoef->stBit[i].bits_38 = (u32Tmp >> 2);

	        pBitCoef->stBit[i].bits_4  = OPTM_VouBitValue(*ps16LtiCoef++);
	        pBitCoef->stBit[i].bits_5  = OPTM_VouBitValue(*ps16LtiCoef++);

	        u32Tmp = OPTM_VouBitValue(*ps16CtiCoef++);
	        pBitCoef->stBit[i].bits_64 = u32Tmp;
	        pBitCoef->stBit[i].bits_66 = u32Tmp >> 4;
	        
	        pBitCoef->stBit[i].bits_7  = OPTM_VouBitValue(*ps16CtiCoef++);
	        pBitCoef->stBit[i].bits_8  = 0;
	        pBitCoef->stBit[i].bits_96 = 0;
	        pBitCoef->stBit[i].bits_94 = 0;
	        pBitCoef->stBit[i].bits_10 = 0;
	        pBitCoef->stBit[i].bits_11 = 0;
	        pBitCoef->stBit[i].bits_12 = 0;
    	}
        
    }

    pBitCoef->u32Size = u32Cnt * sizeof(OPTM_ZME_COEF_BIT_S);

    return HI_SUCCESS;
}


static HI_U32 OPTM_GDtiTransCoefVert(const HI_S16 *ps16LtiCoef, const HI_S16 *ps16CtiCoef,
                                OPTM_ZME_COEF_BITARRAY_S *pBitCoef)
{
    HI_U32 i, u32Cnt,u32Tmp;

    u32Cnt = 6;
    for (i = 0; i < u32Cnt; i++)
    {
    	if(i < 5)
    	{
	    	pBitCoef->stBit[i].bits_0 = OPTM_VouBitValue(*ps16LtiCoef++);
	        pBitCoef->stBit[i].bits_1 = OPTM_VouBitValue(*ps16LtiCoef++);
	        pBitCoef->stBit[i].bits_2 = OPTM_VouBitValue(*ps16CtiCoef++);

	        u32Tmp = OPTM_VouBitValue(*ps16CtiCoef++);
	        pBitCoef->stBit[i].bits_32 = u32Tmp;
	        pBitCoef->stBit[i].bits_38 = (u32Tmp >> 2);

	        pBitCoef->stBit[i].bits_4 = OPTM_VouBitValue(*ps16LtiCoef++);
	        pBitCoef->stBit[i].bits_5 = OPTM_VouBitValue(*ps16LtiCoef++);

	        u32Tmp = OPTM_VouBitValue(*ps16CtiCoef++);
	        pBitCoef->stBit[i].bits_64 = u32Tmp;
	        pBitCoef->stBit[i].bits_66 = u32Tmp >> 4;

	        pBitCoef->stBit[i].bits_7 = OPTM_VouBitValue(*ps16CtiCoef++);
	        pBitCoef->stBit[i].bits_8 = OPTM_VouBitValue(*ps16LtiCoef++);

	        u32Tmp = OPTM_VouBitValue(*ps16LtiCoef++);
	        pBitCoef->stBit[i].bits_96 = u32Tmp;
	        pBitCoef->stBit[i].bits_94 = u32Tmp >> 6;

	        pBitCoef->stBit[i].bits_10 = OPTM_VouBitValue(*ps16CtiCoef++);
	        pBitCoef->stBit[i].bits_11 = OPTM_VouBitValue(*ps16CtiCoef++);
	        pBitCoef->stBit[i].bits_12 = 0;
    	}
    	else
    	{
	    	pBitCoef->stBit[i].bits_0 = OPTM_VouBitValue(*ps16LtiCoef++);
	        pBitCoef->stBit[i].bits_1 = OPTM_VouBitValue(*ps16LtiCoef++);
	        pBitCoef->stBit[i].bits_2 = OPTM_VouBitValue(*ps16CtiCoef++);

	        u32Tmp = OPTM_VouBitValue(*ps16CtiCoef++);
	        pBitCoef->stBit[i].bits_32 = u32Tmp;
	        pBitCoef->stBit[i].bits_38 = (u32Tmp >> 2);

	        pBitCoef->stBit[i].bits_4  = 0;
	        pBitCoef->stBit[i].bits_5  = 0;
	        pBitCoef->stBit[i].bits_64 = 0;
	        pBitCoef->stBit[i].bits_66 = 0;
	        pBitCoef->stBit[i].bits_7  = 0;
	        pBitCoef->stBit[i].bits_8  = 0;
	        pBitCoef->stBit[i].bits_96 = 0;
	        pBitCoef->stBit[i].bits_94 = 0;
	        pBitCoef->stBit[i].bits_10 = 0;
	        pBitCoef->stBit[i].bits_11 = 0;
	        pBitCoef->stBit[i].bits_12 = 0;
    	}
        
    }

    pBitCoef->u32Size = u32Cnt * sizeof(OPTM_ZME_COEF_BIT_S);

    return HI_SUCCESS;
}



static HI_U32 OPTM_GZmeLoadCoefH(OPTM_GZME_COEF_RATIO_E enCoefRatio, HI_U8 *pu8Addr)
{
    HI_S16 u16TableTmp[8 * 8];
    HI_U32 u32TableTmpSize;
    OPTM_ZME_COEF_BITARRAY_S stArray;   /*for DTI coefficient*/

#warning TODO: OPTM_GZmeLoadCoefH

	memset(u16TableTmp, 0, sizeof(u16TableTmp));
    //Luma
    u32TableTmpSize = OPTM_GZmeTransCoefAlign(g_pOPTMGfxZmeCoef[enCoefRatio][GZME_COEF_8T8P_LH],(HI_S16)(8 * 8), u16TableTmp);
    memcpy(pu8Addr, u16TableTmp, u32TableTmpSize);
    pu8Addr += u32TableTmpSize;

    //Chroma
    u32TableTmpSize = OPTM_GZmeTransCoefAlign(g_pOPTMGfxZmeCoef[enCoefRatio][GZME_COEF_8T8P_CH], (HI_S16)(8 * 8), u16TableTmp);
    memcpy(pu8Addr, u16TableTmp, u32TableTmpSize);
    pu8Addr += u32TableTmpSize;

	//for LTI&CTI horizontal
    OPTM_GDtiTransCoefHor(g_pOPTMGfxDtiZmeCoef[enCoefRatio][GDTI_COEF_2T8P_LH], g_pOPTMGfxDtiZmeCoef[enCoefRatio][GDTI_COEF_2T8P_CH], &stArray);
    memcpy(pu8Addr, stArray.stBit, stArray.u32Size);
    
    return HI_SUCCESS;
}


static HI_S32 OPTM_GZmeLoadCoefV(OPTM_GZME_COEF_RATIO_E enCoefRatio, HI_U8 *pu8Addr)
{
    HI_S16 u16TableTmp[4 * 16 * 2];
    HI_U32 u32TableTmpSize = 4 * 16 * 2;
    const HI_S16 *ps16L, *ps16Ch, *ps16Lti, *ps16Cti;
    HI_S16 *ps16Dst;
    HI_S32 i;
    OPTM_ZME_COEF_BITARRAY_S stArray;

#warning TODO: OPTM_GZmeLoadCoefV

    ps16L   = g_pOPTMGfxZmeCoef[enCoefRatio][GZME_COEF_4T16P_LV];
    ps16Ch  = g_pOPTMGfxZmeCoef[enCoefRatio][GZME_COEF_4T16P_CV];
    ps16Lti = g_pOPTMGfxDtiZmeCoef[enCoefRatio][GDTI_COEF_2T16P_LV];
    ps16Cti = g_pOPTMGfxDtiZmeCoef[enCoefRatio][GDTI_COEF_2T16P_CV];

    ps16Dst = u16TableTmp;

    /*load vertical zme(luma and chroma) coef*/
    for (i=0; i<64; i+=4)
    {
        *ps16Dst++ = *ps16Ch++;
        *ps16Dst++ = *ps16Ch++;
        *ps16Dst++ = *ps16Ch++;
        *ps16Dst++ = *ps16Ch++;

        *ps16Dst++ = *ps16L++;
        *ps16Dst++ = *ps16L++;
        *ps16Dst++ = *ps16L++;
        *ps16Dst++ = *ps16L++;        
    }
    memcpy(pu8Addr, u16TableTmp, u32TableTmpSize*2);
    pu8Addr += u32TableTmpSize*2;

    OPTM_GDtiTransCoefVert(ps16Lti, ps16Cti, &stArray);
    memcpy(pu8Addr, stArray.stBit, stArray.u32Size);


    return HI_SUCCESS;
}


static HI_U32 OPTM_GZmeLoadSDCoefH(OPTM_GZME_COEF_RATIO_E enCoefRatio, HI_U8 *pu8Addr)
{
    HI_S16 u16TableTmp[8 * 8];
    HI_U32 u32TableTmpSize;

#warning TODO: OPTM_GZmeLoadSDCoefH

	memset(u16TableTmp,0,sizeof(u16TableTmp));
    //����
    u32TableTmpSize = OPTM_GZmeTransCoefAlign(g_pOPTMGfxSDZmeCoef[enCoefRatio][GZME_COEF_8T8P_LH],(HI_S16)(8 * 8), u16TableTmp);
    memcpy(pu8Addr, u16TableTmp, u32TableTmpSize);
    pu8Addr += u32TableTmpSize;

    //ɫ��
    u32TableTmpSize = OPTM_GZmeTransCoefAlign(g_pOPTMGfxSDZmeCoef[enCoefRatio][GZME_COEF_8T8P_CH], (HI_S16)(8 * 8), u16TableTmp);
    memcpy(pu8Addr, u16TableTmp, u32TableTmpSize);
    pu8Addr += u32TableTmpSize;

    return HI_SUCCESS;
}


static HI_S32 OPTM_GZmeLoadSDCoefV(OPTM_GZME_COEF_RATIO_E enCoefRatio, HI_U8 *pu8Addr)
{
    HI_S16 u16TableTmp[4 * 16 * 2];
    const HI_S16 *ps16L, *ps16Ch;
    HI_S16 *ps16Dst;
    HI_S32 i;

#warning TODO: OPTM_GZmeLoadSDCoefV

    ps16L   = g_pOPTMGfxSDZmeCoef[enCoefRatio][GZME_COEF_4T16P_LV];
    ps16Ch  = g_pOPTMGfxSDZmeCoef[enCoefRatio][GZME_COEF_4T16P_CV];
    ps16Dst = u16TableTmp;

    for (i=0; i<64; i+=4)
    {
        *ps16Dst++ = *ps16Ch++;
        *ps16Dst++ = *ps16Ch++;
        *ps16Dst++ = *ps16Ch++;
        *ps16Dst++ = *ps16Ch++;

        *ps16Dst++ = *ps16L++;
        *ps16Dst++ = *ps16L++;
        *ps16Dst++ = *ps16L++;
        *ps16Dst++ = *ps16L++;        
    }

    memcpy(pu8Addr, u16TableTmp, 16*4*2*2);

    return HI_SUCCESS;
}



HI_VOID OPTM_GZmeLoadCoefHV(OPTM_ALG_GZME_MEM_S *pstGZmeCoefMem)
{
    HI_U8 *pu8CurAddr;
    HI_U32 u32NumSize;
    HI_U32 u32PhyAddr;
    OPTM_ALG_GZME_COEF_ADDR_S *pstAddrTmp;

    pu8CurAddr = (HI_U8 *)(pstGZmeCoefMem->stMBuf.u32StartVirAddr);
    u32PhyAddr = pstGZmeCoefMem->stMBuf.u32StartPhyAddr;
    pstAddrTmp = &(pstGZmeCoefMem->stZmeCoefAddr);

	//GFX0 
    // 8 tap 8 phase for horizontal
    u32NumSize = 256 + 48;    //182030   256 + (2*8*2/12+1)*16
    OPTM_GZmeLoadCoefH(GZME_COEF_1, pu8CurAddr);
    pstAddrTmp->u32ZmeCoefAddrHL_1 = u32PhyAddr;
    u32PhyAddr  += u32NumSize;
    pu8CurAddr += u32NumSize;

    OPTM_GZmeLoadCoefH(GZME_COEF_E1, pu8CurAddr);
    pstAddrTmp->u32ZmeCoefAddrHL_E1 = u32PhyAddr;
    u32PhyAddr  += u32NumSize;
    pu8CurAddr += u32NumSize;

    OPTM_GZmeLoadCoefH(GZME_COEF_075, pu8CurAddr);
    pstAddrTmp->u32ZmeCoefAddrHL_075 = u32PhyAddr;
    u32PhyAddr  += u32NumSize;
    pu8CurAddr += u32NumSize;

    OPTM_GZmeLoadCoefH(GZME_COEF_05, pu8CurAddr);
    pstAddrTmp->u32ZmeCoefAddrHL_05 = u32PhyAddr;
    u32PhyAddr  += u32NumSize;
    pu8CurAddr += u32NumSize;

    OPTM_GZmeLoadCoefH(GZME_COEF_033, pu8CurAddr);
    pstAddrTmp->u32ZmeCoefAddrHL_033 = u32PhyAddr;
    u32PhyAddr  += u32NumSize;
    pu8CurAddr += u32NumSize;

    OPTM_GZmeLoadCoefH(GZME_COEF_025, pu8CurAddr);
    pstAddrTmp->u32ZmeCoefAddrHL_025 = u32PhyAddr;
    u32PhyAddr  += u32NumSize;
    pu8CurAddr += u32NumSize;

    OPTM_GZmeLoadCoefH(GZME_COEF_0, pu8CurAddr);
    pstAddrTmp->u32ZmeCoefAddrHL_0 = u32PhyAddr;
    u32PhyAddr  += u32NumSize;
    pu8CurAddr += u32NumSize;

    // 4 tap 16 phase for vertical 
    u32NumSize = 256 + 96;            //182030   256+(2*16*2/12+1)*16    
    OPTM_GZmeLoadCoefV(GZME_COEF_1,  pu8CurAddr);
    pstAddrTmp->u32ZmeCoefAddrVL_1 = u32PhyAddr;
    u32PhyAddr   += u32NumSize;
    pu8CurAddr += u32NumSize;

    OPTM_GZmeLoadCoefV(GZME_COEF_E1, pu8CurAddr);
    pstAddrTmp->u32ZmeCoefAddrVL_E1 = u32PhyAddr;
    u32PhyAddr   += u32NumSize;
    pu8CurAddr += u32NumSize;

    OPTM_GZmeLoadCoefV(GZME_COEF_075,  pu8CurAddr);
    pstAddrTmp->u32ZmeCoefAddrVL_075 = u32PhyAddr;
    u32PhyAddr   += u32NumSize;
    pu8CurAddr += u32NumSize;

    OPTM_GZmeLoadCoefV(GZME_COEF_05,  pu8CurAddr);
    pstAddrTmp->u32ZmeCoefAddrVL_05 = u32PhyAddr;
    u32PhyAddr   += u32NumSize;
    pu8CurAddr += u32NumSize;

    OPTM_GZmeLoadCoefV(GZME_COEF_033,  pu8CurAddr);
    pstAddrTmp->u32ZmeCoefAddrVL_033 = u32PhyAddr;
    u32PhyAddr   += u32NumSize;
    pu8CurAddr += u32NumSize;

    OPTM_GZmeLoadCoefV(GZME_COEF_025,  pu8CurAddr);
    pstAddrTmp->u32ZmeCoefAddrVL_025 = u32PhyAddr;
    u32PhyAddr   += u32NumSize;
    pu8CurAddr += u32NumSize;

    OPTM_GZmeLoadCoefV(GZME_COEF_0,  pu8CurAddr);
    pstAddrTmp->u32ZmeCoefAddrVL_0 = u32PhyAddr;
    u32PhyAddr   += u32NumSize;
    pu8CurAddr += u32NumSize;

    //GFX1 or WBC2//added by t59295
	// 8 tap 8 phase for horizontal
    u32NumSize = 256 + 48;
    OPTM_GZmeLoadSDCoefH(GZME_COEF_1, pu8CurAddr);
    pstAddrTmp->u32ZmeSDCoefAddrHL_1 = u32PhyAddr;
    u32PhyAddr  += u32NumSize;
    pu8CurAddr += u32NumSize;

    OPTM_GZmeLoadSDCoefH(GZME_COEF_E1, pu8CurAddr);
    pstAddrTmp->u32ZmeSDCoefAddrHL_E1 = u32PhyAddr;
    u32PhyAddr  += u32NumSize;
    pu8CurAddr += u32NumSize;

    OPTM_GZmeLoadSDCoefH(GZME_COEF_075, pu8CurAddr);
    pstAddrTmp->u32ZmeSDCoefAddrHL_075 = u32PhyAddr;
    u32PhyAddr  += u32NumSize;
    pu8CurAddr += u32NumSize;

    OPTM_GZmeLoadSDCoefH(GZME_COEF_05, pu8CurAddr);
    pstAddrTmp->u32ZmeSDCoefAddrHL_05 = u32PhyAddr;
    u32PhyAddr  += u32NumSize;
    pu8CurAddr += u32NumSize;

    OPTM_GZmeLoadSDCoefH(GZME_COEF_033, pu8CurAddr);
    pstAddrTmp->u32ZmeSDCoefAddrHL_033 = u32PhyAddr;
    u32PhyAddr  += u32NumSize;
    pu8CurAddr += u32NumSize;

    OPTM_GZmeLoadSDCoefH(GZME_COEF_025, pu8CurAddr);
    pstAddrTmp->u32ZmeSDCoefAddrHL_025 = u32PhyAddr;
    u32PhyAddr  += u32NumSize;
    pu8CurAddr += u32NumSize;

    OPTM_GZmeLoadSDCoefH(GZME_COEF_0, pu8CurAddr);
    pstAddrTmp->u32ZmeSDCoefAddrHL_0 = u32PhyAddr;
    u32PhyAddr  += u32NumSize;
    pu8CurAddr += u32NumSize;

    // 4 tap 16 phase for vertical 
    u32NumSize = 256 + 96;
    OPTM_GZmeLoadSDCoefV(GZME_COEF_1,  pu8CurAddr);
    pstAddrTmp->u32ZmeSDCoefAddrVL_1 = u32PhyAddr;
    u32PhyAddr   += u32NumSize;
    pu8CurAddr += u32NumSize;

    OPTM_GZmeLoadSDCoefV(GZME_COEF_E1, pu8CurAddr);
    pstAddrTmp->u32ZmeSDCoefAddrVL_E1 = u32PhyAddr;
    u32PhyAddr   += u32NumSize;
    pu8CurAddr += u32NumSize;

    OPTM_GZmeLoadSDCoefV(GZME_COEF_075,  pu8CurAddr);
    pstAddrTmp->u32ZmeSDCoefAddrVL_075 = u32PhyAddr;
    u32PhyAddr   += u32NumSize;
    pu8CurAddr += u32NumSize;

    OPTM_GZmeLoadSDCoefV(GZME_COEF_05,  pu8CurAddr);
    pstAddrTmp->u32ZmeSDCoefAddrVL_05 = u32PhyAddr;
    u32PhyAddr   += u32NumSize;
    pu8CurAddr += u32NumSize;

    OPTM_GZmeLoadSDCoefV(GZME_COEF_033,  pu8CurAddr);
    pstAddrTmp->u32ZmeSDCoefAddrVL_033 = u32PhyAddr;
    u32PhyAddr   += u32NumSize;
    pu8CurAddr += u32NumSize;

    OPTM_GZmeLoadSDCoefV(GZME_COEF_025,  pu8CurAddr);
    pstAddrTmp->u32ZmeSDCoefAddrVL_025 = u32PhyAddr;
    u32PhyAddr   += u32NumSize;
    pu8CurAddr += u32NumSize;

    OPTM_GZmeLoadSDCoefV(GZME_COEF_0,  pu8CurAddr);
    pstAddrTmp->u32ZmeSDCoefAddrVL_0 = u32PhyAddr;
    u32PhyAddr   += u32NumSize;
    pu8CurAddr += u32NumSize;
}


HI_S32 OPTM_ALG_GZmeVdpComnInit(OPTM_ALG_GZME_MEM_S *pstGZmeCoefMem)
{
	//apply memory for zme coefficient, and get the address.
	if (OPTM_AllocAndMap(HIFB_ZME_COEF_BUFFER, HI_NULL, 
			9184, //8176 = (256*4 + 48 + 96) * 7,
		    0, &pstGZmeCoefMem->stMBuf) != HI_SUCCESS)
	{
	    HI_PRINT("GFX Get wbc2 buffer failed!\n");
	    return HI_FAILURE;
	}	
	
    //load zme coefficient into the memory
    OPTM_GZmeLoadCoefHV(pstGZmeCoefMem);

    return HI_SUCCESS;
}


HI_VOID OPTM_ALG_GZmeVdpComnDeInit(OPTM_ALG_GZME_MEM_S *pstGZmeCoefMem)
{
    //release zme coefficient memory 
    if (pstGZmeCoefMem->stMBuf.u32StartVirAddr != 0) 
    {
        //release memory
        OPTM_UnmapAndRelease(&(pstGZmeCoefMem->stMBuf));
        pstGZmeCoefMem->stMBuf.u32StartVirAddr = 0;
        pstGZmeCoefMem->stMBuf.u32StartPhyAddr = 0;
    }
}




HI_U32 OPTM_GetGfxHLfirCoef(OPTM_ALG_GZME_COEF_ADDR_S *pstZmeCoefAddr, HI_U32 u32Ratio)
{
    HI_U32 u32CoefAddr;
    HI_U32 u32TmpRatio = 4096 * 4096 / u32Ratio;

    if      (u32TmpRatio > 4096 ) { u32CoefAddr = pstZmeCoefAddr->u32ZmeCoefAddrHL_1;   }
    //else if (u32TmpRatio == 4096) { u32CoefAddr = pstZmeCoefAddr->u32ZmeCoefAddrHL_E1;  }
    else if (u32TmpRatio >= 3686) { u32CoefAddr = pstZmeCoefAddr->u32ZmeCoefAddrHL_E1;  }
    else if (u32TmpRatio >= 3072) { u32CoefAddr = pstZmeCoefAddr->u32ZmeCoefAddrHL_075; }
    else if (u32TmpRatio >= 2048) { u32CoefAddr = pstZmeCoefAddr->u32ZmeCoefAddrHL_05;  }
    else if (u32TmpRatio >= 1365) { u32CoefAddr = pstZmeCoefAddr->u32ZmeCoefAddrHL_033; }
    else if (u32TmpRatio >= 1024) { u32CoefAddr = pstZmeCoefAddr->u32ZmeCoefAddrHL_025; }
    else                          { u32CoefAddr = pstZmeCoefAddr->u32ZmeCoefAddrHL_0;   }


    return u32CoefAddr;
}


HI_U32 OPTM_GetGfxHCfirCoef(OPTM_ALG_GZME_COEF_ADDR_S *pstZmeCoefAddr, HI_U32 u32Ratio)
{
    HI_U32 u32CoefAddr;
    HI_U32 u32TmpRatio = 4096 * 4096 / u32Ratio;

     if     (u32TmpRatio > 4096 ) { u32CoefAddr = pstZmeCoefAddr->u32ZmeCoefAddrHC_1;   }
    else if (u32TmpRatio == 4096) { u32CoefAddr = pstZmeCoefAddr->u32ZmeCoefAddrHC_E1;  }                                                
    else if (u32TmpRatio >= 3072) { u32CoefAddr = pstZmeCoefAddr->u32ZmeCoefAddrHC_075; }                                                
    else if (u32TmpRatio >= 2048) { u32CoefAddr = pstZmeCoefAddr->u32ZmeCoefAddrHC_05;  }                                                
    else if (u32TmpRatio >= 1365) { u32CoefAddr = pstZmeCoefAddr->u32ZmeCoefAddrHC_033; }                                                
    else if (u32TmpRatio >= 1024) { u32CoefAddr = pstZmeCoefAddr->u32ZmeCoefAddrHC_025; }                                                
    else                          { u32CoefAddr = pstZmeCoefAddr->u32ZmeCoefAddrHC_0;   }


    return u32CoefAddr;
}



HI_U32 OPTM_GetGfxVLfirCoef(OPTM_ALG_GZME_COEF_ADDR_S *pstZmeCoefAddr, HI_U32 u32Ratio)
{
    HI_U32 u32CoefAddr;
    HI_U32 u32TmpRatio = 4096 * 4096 / u32Ratio;

    if     (u32TmpRatio > 4096 ) { u32CoefAddr = pstZmeCoefAddr->u32ZmeCoefAddrVL_1;   }
    //else if(u32TmpRatio == 4096) { u32CoefAddr = pstZmeCoefAddr->u32ZmeCoefAddrVL_E1;  }
	else if(u32TmpRatio >= 3686) { u32CoefAddr = pstZmeCoefAddr->u32ZmeCoefAddrVL_E1;  }
    else if(u32TmpRatio >= 3072) { u32CoefAddr = pstZmeCoefAddr->u32ZmeCoefAddrVL_075; }
    else if(u32TmpRatio >= 2048) { u32CoefAddr = pstZmeCoefAddr->u32ZmeCoefAddrVL_05;  }
    else if(u32TmpRatio >= 1365) { u32CoefAddr = pstZmeCoefAddr->u32ZmeCoefAddrVL_033; }
    else if(u32TmpRatio >= 1024) { u32CoefAddr = pstZmeCoefAddr->u32ZmeCoefAddrVL_025; }
    else                         { u32CoefAddr = pstZmeCoefAddr->u32ZmeCoefAddrVL_0;   }


    return u32CoefAddr;
}


HI_U32 OPTM_GetGfxVCfirCoef(OPTM_ALG_GZME_COEF_ADDR_S *pstZmeCoefAddr, HI_U32 u32Ratio)
{
    HI_U32 u32CoefAddr;
    HI_U32 u32TmpRatio = 4096 * 4096 / u32Ratio;

    if     (u32TmpRatio > 4096 ) { u32CoefAddr = pstZmeCoefAddr->u32ZmeCoefAddrVC_1;   }
    else if(u32TmpRatio == 4096) { u32CoefAddr = pstZmeCoefAddr->u32ZmeCoefAddrVC_E1;  }
    else if(u32TmpRatio >= 3072) { u32CoefAddr = pstZmeCoefAddr->u32ZmeCoefAddrVC_075; }
    else if(u32TmpRatio >= 2048) { u32CoefAddr = pstZmeCoefAddr->u32ZmeCoefAddrVC_05;  }
    else if(u32TmpRatio >= 1365) { u32CoefAddr = pstZmeCoefAddr->u32ZmeCoefAddrVC_033; }
    else if(u32TmpRatio >= 1024) { u32CoefAddr = pstZmeCoefAddr->u32ZmeCoefAddrVC_025; }
    else                         { u32CoefAddr = pstZmeCoefAddr->u32ZmeCoefAddrVC_0;   }


    return u32CoefAddr;
}


HI_U32 OPTM_GetGfxHLfirSDCoef(OPTM_ALG_GZME_COEF_ADDR_S *pstZmeCoefAddr, HI_U32 u32Ratio)
{
    HI_U32 u32CoefAddr;
    HI_U32 u32TmpRatio = 4096 * 4096 / u32Ratio;

    if      (u32TmpRatio > 4096 ) { u32CoefAddr = pstZmeCoefAddr->u32ZmeSDCoefAddrHL_1;   }
    else if (u32TmpRatio == 4096) { u32CoefAddr = pstZmeCoefAddr->u32ZmeSDCoefAddrHL_E1;  }
    else if (u32TmpRatio >= 3072) { u32CoefAddr = pstZmeCoefAddr->u32ZmeSDCoefAddrHL_075; }
    else if (u32TmpRatio >= 2048) { u32CoefAddr = pstZmeCoefAddr->u32ZmeSDCoefAddrHL_05;  }
    else if (u32TmpRatio >= 1365) { u32CoefAddr = pstZmeCoefAddr->u32ZmeSDCoefAddrHL_033; }
    else if (u32TmpRatio >= 1024) { u32CoefAddr = pstZmeCoefAddr->u32ZmeSDCoefAddrHL_025; }
    else                          { u32CoefAddr = pstZmeCoefAddr->u32ZmeSDCoefAddrHL_0;   }


    return u32CoefAddr;
}


HI_U32 OPTM_GetGfxHCfirSDCoef(OPTM_ALG_GZME_COEF_ADDR_S *pstZmeCoefAddr, HI_U32 u32Ratio)
{
    HI_U32 u32CoefAddr;
    HI_U32 u32TmpRatio = 4096 * 4096 / u32Ratio;

    if      (u32TmpRatio > 4096 ) { u32CoefAddr = pstZmeCoefAddr->u32ZmeSDCoefAddrHC_1;   }
    else if (u32TmpRatio == 4096) { u32CoefAddr = pstZmeCoefAddr->u32ZmeSDCoefAddrHC_E1;  }                                                
    else if (u32TmpRatio >= 3072) { u32CoefAddr = pstZmeCoefAddr->u32ZmeSDCoefAddrHC_075; }                                                
    else if (u32TmpRatio >= 2048) { u32CoefAddr = pstZmeCoefAddr->u32ZmeSDCoefAddrHC_05;  }                                                
    else if (u32TmpRatio >= 1365) { u32CoefAddr = pstZmeCoefAddr->u32ZmeSDCoefAddrHC_033; }                                                
    else if (u32TmpRatio >= 1024) { u32CoefAddr = pstZmeCoefAddr->u32ZmeSDCoefAddrHC_025; }                                                
    else                          { u32CoefAddr = pstZmeCoefAddr->u32ZmeSDCoefAddrHC_0;   }


    return u32CoefAddr;
}



HI_U32 OPTM_GetGfxVLfirSDCoef(OPTM_ALG_GZME_COEF_ADDR_S *pstZmeCoefAddr, HI_U32 u32Ratio)
{
    HI_U32 u32CoefAddr;
    HI_U32 u32TmpRatio = 4096 * 4096 / u32Ratio;

    if     (u32TmpRatio > 4096 ) { u32CoefAddr = pstZmeCoefAddr->u32ZmeSDCoefAddrVL_1;   }
    else if(u32TmpRatio == 4096) { u32CoefAddr = pstZmeCoefAddr->u32ZmeSDCoefAddrVL_E1;  }
    else if(u32TmpRatio >= 3072) { u32CoefAddr = pstZmeCoefAddr->u32ZmeSDCoefAddrVL_075; }
    else if(u32TmpRatio >= 2048) { u32CoefAddr = pstZmeCoefAddr->u32ZmeSDCoefAddrVL_05;  }
    else if(u32TmpRatio >= 1365) { u32CoefAddr = pstZmeCoefAddr->u32ZmeSDCoefAddrVL_033; }
    else if(u32TmpRatio >= 1024) { u32CoefAddr = pstZmeCoefAddr->u32ZmeSDCoefAddrVL_025; }
    else                         { u32CoefAddr = pstZmeCoefAddr->u32ZmeSDCoefAddrVL_0;   }


    return u32CoefAddr;
}


HI_U32 OPTM_GetGfxVCfirSDCoef(OPTM_ALG_GZME_COEF_ADDR_S *pstZmeCoefAddr, HI_U32 u32Ratio)
{
    HI_U32 u32CoefAddr;
    HI_U32 u32TmpRatio = 4096 * 4096 / u32Ratio;

    if     (u32TmpRatio > 4096 ) { u32CoefAddr = pstZmeCoefAddr->u32ZmeSDCoefAddrVC_1;   }
    else if(u32TmpRatio == 4096) { u32CoefAddr = pstZmeCoefAddr->u32ZmeSDCoefAddrVC_E1;  }
    else if(u32TmpRatio >= 3072) { u32CoefAddr = pstZmeCoefAddr->u32ZmeSDCoefAddrVC_075; }
    else if(u32TmpRatio >= 2048) { u32CoefAddr = pstZmeCoefAddr->u32ZmeSDCoefAddrVC_05;  }
    else if(u32TmpRatio >= 1365) { u32CoefAddr = pstZmeCoefAddr->u32ZmeSDCoefAddrVC_033; }
    else if(u32TmpRatio >= 1024) { u32CoefAddr = pstZmeCoefAddr->u32ZmeSDCoefAddrVC_025; }
    else                         { u32CoefAddr = pstZmeCoefAddr->u32ZmeSDCoefAddrVC_0;   }


    return u32CoefAddr;
}



HI_VOID OPTM_ALG_GZmeComnSet(OPTM_ALG_GZME_MEM_S *pstMem, OPTM_ALG_GZME_DRV_PARA_S *pstZmeDrvPara, OPTM_ALG_GZME_RTL_PARA_S *pstZmeRtlPara)
{
     HI_U32 u32VZmeRealRatio;

    
    //config zme enable
    pstZmeRtlPara->bZmeEnH = (pstZmeDrvPara->u32ZmeFrmWIn == pstZmeDrvPara->u32ZmeFrmWOut) ? HI_FALSE: HI_TRUE;
	
    if( (pstZmeDrvPara->u32ZmeFrmHIn == pstZmeDrvPara->u32ZmeFrmHOut)
       && (pstZmeDrvPara->bZmeFrmFmtIn == pstZmeDrvPara->bZmeFrmFmtOut))
    {
        pstZmeRtlPara->bZmeEnV = HI_FALSE;
    }
    else
    {
        pstZmeRtlPara->bZmeEnV = HI_TRUE;
    }
    
	//modified by z00128410 for opening sharp module 2013/6/22 Himedia problem
	pstZmeRtlPara->bZmeEnH = HI_TRUE;
	pstZmeRtlPara->bZmeEnV = HI_TRUE;
	
    //config vertical chroma zme tap and zme order
    pstZmeRtlPara->bZmeTapVC = 0;
    pstZmeRtlPara->bZmeOrder = (pstZmeRtlPara->u32ZmeRatioHL>=OPTM_ALG_G_HZME_PRECISION) ? 0 : 1;


    //config zme ratio, pay attention: this vertical ratio may not be the real ratio
    //logic would calculate the real ratio according to the input and output frame format.
    pstZmeRtlPara->u32ZmeRatioHL = pstZmeDrvPara->u32ZmeFrmWIn * OPTM_ALG_G_HZME_PRECISION / pstZmeDrvPara->u32ZmeFrmWOut;
    pstZmeRtlPara->u32ZmeRatioVL = pstZmeDrvPara->u32ZmeFrmHIn * OPTM_ALG_G_VZME_PRECISION / pstZmeDrvPara->u32ZmeFrmHOut;
    pstZmeRtlPara->u32ZmeRatioHC = pstZmeRtlPara->u32ZmeRatioHL;
    pstZmeRtlPara->u32ZmeRatioVC = pstZmeRtlPara->u32ZmeRatioVL;
    
    //calculate luma real zme resolution of input and output 
    if( (pstZmeDrvPara->bZmeFrmFmtIn == 1) && (pstZmeDrvPara->bZmeFrmFmtOut == 0) )
    {
        u32VZmeRealRatio = pstZmeRtlPara->u32ZmeRatioVL * 2;
    }
    else
    {
    	//return;
        u32VZmeRealRatio = pstZmeRtlPara->u32ZmeRatioVL;
    }
    

    //calculate horizontal zme offset
    pstZmeRtlPara->s32ZmeOffsetHL = 0;
    pstZmeRtlPara->s32ZmeOffsetHC = 0;

    //calculate vertical zme offset: offset is the real offset!
    pstZmeRtlPara->s32ZmeOffsetVTop = 0;
    if( (pstZmeDrvPara->bZmeFrmFmtIn == 0) && (pstZmeDrvPara->bZmeFrmFmtOut == 0) )
    {
        pstZmeRtlPara->s32ZmeOffsetVBtm = ( (HI_S32)u32VZmeRealRatio - OPTM_ALG_G_VZME_PRECISION ) / 2;
    }
    else if( (pstZmeDrvPara->bZmeFrmFmtIn == 1) && (pstZmeDrvPara->bZmeFrmFmtOut == 0) )
    {
        pstZmeRtlPara->s32ZmeOffsetVBtm = (HI_S32)u32VZmeRealRatio / 2;
    }
    else
    {
        pstZmeRtlPara->s32ZmeOffsetVBtm = 0;
    }

    
    //config zme mode
    if( pstZmeRtlPara->u32ZmeRatioHL == OPTM_ALG_G_HZME_PRECISION ) //when 1:1 horizontal scaler, directly copy
    {
        pstZmeRtlPara->bZmeMdHA = 0;
        pstZmeRtlPara->bZmeMdHLC = 0;
    }
    else
    {
        pstZmeRtlPara->bZmeMdHA = 1;
        pstZmeRtlPara->bZmeMdHLC = 1;
    }

	//modified by z00128410 for opening sharp module 2013/6/22 Himedia problem
	pstZmeRtlPara->bZmeMdHA = 1;
    pstZmeRtlPara->bZmeMdHLC = 1;
	
    //considering anti-flicker for SD, the situation of 1:1 vertical scaler also need low-pass filter
    pstZmeRtlPara->bZmeMdVA = 1;
    pstZmeRtlPara->bZmeMdVLC = 1;

    //config zme median filter enable: if filter tap > 2 and upscaler, median filter enable
    if(pstZmeRtlPara->u32ZmeRatioHL < OPTM_ALG_G_HZME_PRECISION)
    {
        pstZmeRtlPara->bZmeMedHA = 1;
        pstZmeRtlPara->bZmeMedHL = 1;
        pstZmeRtlPara->bZmeMedHC = 1;
    }
    else
    {
        pstZmeRtlPara->bZmeMedHA = 0;
        pstZmeRtlPara->bZmeMedHL = 0;
        pstZmeRtlPara->bZmeMedHC = 0;
    }

    if(pstZmeRtlPara->u32ZmeRatioVL < OPTM_ALG_G_VZME_PRECISION && pstZmeRtlPara->bZmeTapVC == 0)
    {
        pstZmeRtlPara->bZmeMedVA = 1;
        pstZmeRtlPara->bZmeMedVL = 1;
        pstZmeRtlPara->bZmeMedVC = 1;
    }
    else
    {
        pstZmeRtlPara->bZmeMedVA = 0;
        pstZmeRtlPara->bZmeMedVL = 0;
        pstZmeRtlPara->bZmeMedVC = 0;
    }
}


HI_VOID OPTM_ALG_GZmeHDSet(OPTM_ALG_GZME_MEM_S *pstMem, OPTM_ALG_GZME_DRV_PARA_S *pstZmeDrvPara, OPTM_ALG_GZME_RTL_PARA_S *pstZmeRtlPara)
{
    OPTM_ALG_GZmeComnSet(pstMem, pstZmeDrvPara, pstZmeRtlPara);

    pstZmeRtlPara->u32ZmeCoefAddrHL = OPTM_GetGfxHLfirCoef( &(pstMem->stZmeCoefAddr), pstZmeRtlPara->u32ZmeRatioHL);
    pstZmeRtlPara->u32ZmeCoefAddrHC = OPTM_GetGfxHCfirCoef( &(pstMem->stZmeCoefAddr), pstZmeRtlPara->u32ZmeRatioHC);
    pstZmeRtlPara->u32ZmeCoefAddrVL = OPTM_GetGfxVLfirCoef( &(pstMem->stZmeCoefAddr), pstZmeRtlPara->u32ZmeRatioVL);
    pstZmeRtlPara->u32ZmeCoefAddrVC = OPTM_GetGfxVCfirCoef( &(pstMem->stZmeCoefAddr), pstZmeRtlPara->u32ZmeRatioVC);
}


HI_VOID OPTM_ALG_GZmeSDSet(OPTM_ALG_GZME_MEM_S *pstMem, OPTM_ALG_GZME_DRV_PARA_S *pstZmeDrvPara, OPTM_ALG_GZME_RTL_PARA_S *pstZmeRtlPara)
{
    OPTM_ALG_GZmeComnSet(pstMem, pstZmeDrvPara, pstZmeRtlPara);

    pstZmeRtlPara->u32ZmeCoefAddrHL = OPTM_GetGfxHLfirSDCoef( &(pstMem->stZmeCoefAddr), pstZmeRtlPara->u32ZmeRatioHL);
    pstZmeRtlPara->u32ZmeCoefAddrHC = OPTM_GetGfxHCfirSDCoef( &(pstMem->stZmeCoefAddr), pstZmeRtlPara->u32ZmeRatioHC);
    pstZmeRtlPara->u32ZmeCoefAddrVL = OPTM_GetGfxVLfirSDCoef( &(pstMem->stZmeCoefAddr), pstZmeRtlPara->u32ZmeRatioVL);
    pstZmeRtlPara->u32ZmeCoefAddrVC = OPTM_GetGfxVCfirSDCoef( &(pstMem->stZmeCoefAddr), pstZmeRtlPara->u32ZmeRatioVC);
}



