//**********************************************************************
//                                                                          
// Copyright(c)2008,Huawei Technologies Co.,Ltd                            
// All rights reserved.                                                     
//                                                                          
// File Name   : vdp_driver.cpp
// Author      : s173141
// Email       : suyong7@huawei.com
// Data        : 2012/10/17
// Version     : v1.0
// Abstract    : header of vdp define
//                                                                         
// Modification history                                                    
//----------------------------------------------------------------------  
// Version       Data(yyyy/mm/dd)      name                                  
// Description                                                              
//                                                                          
// $Log: vdp_driver.cpp,v $
//
//
//
//
//**********************************************************************
//#include <linux/delay.h>
//#include <linux/timer.h>
//#include <linux/kernel.h>

#include "optm_hal.h"


#define STRIDE_ALIGN 16
#ifndef HI_BUILD_IN_BOOT
#define HI_PRINT printk
#else
#include "hifb_debug.h"
#endif
volatile OPTM_S_VDP_CV200_REGS_TYPE *pOptmVdpReg; //81141f5c
volatile OPTM_MDDRC_REGS_S *pMddrcReg; //81141f60 +4

#define OPTM_HAL_CHECK_NULL_POINT(p) do\
{\
 if (HI_NULL == p)\
    {\
        HI_PRINT("Error!can't handle null point.\n");\
    	return;\
    }\
}while(0)

#define OPTM_HAL_CHECK_LAYER_VALID(u32Data) do\
 {\
  if (u32Data >= OPTM_GFX_MAX)\
	 {\
		 HI_PRINT("Error,%s Select Wrong Graph Layer ID\n", __FUNCTION__);\
		 return;\
	 }\
 }while(0)


//-------------------------------------------------
// 
//-------------------------------------------------

HI_VOID  OPTM_VDP_DRIVER_Initial(HI_U32 virAddr)
{
    pOptmVdpReg = (OPTM_S_VDP_CV200_REGS_TYPE *)virAddr;

    //pOptmVdpReg->CBM_ATTR.u32 = 0x1Cul;
}

#ifndef HI_BUILD_IN_BOOT
HI_VOID  OPTM_MDDRC_DRIVER_Initial(HI_U32 virAddr)
{
	pMddrcReg = (OPTM_MDDRC_REGS_S*)virAddr;
}
#endif

HI_U32 OPTM_VDP_RegRead(HI_U32 a)
{
   //msleep(5);
   //printk("\nread---addr = 0x%x,date = 0x%x",a,*a);
   return (*((HI_U32 *)a));
}

HI_VOID OPTM_VDP_RegWrite(HI_U32 a, HI_U32 b)
{
    //msleep(5);
    //printk("\nwrite---addr = 0x%x,date = 0x%x",a,b);
    *(HI_U32 *)a = b;       // ENV cfg
}     




//--------------------------------------------------------------------
// Video Layer
//--------------------------------------------------------------------

//--------------------------------------------------------------------
// DHD Channel
//--------------------------------------------------------------------

//-------------------------------------------------------------------
//VDP_DISP_CLIP_END
//-------------------------------------------------------------------


//-------------------------------------------------------------------
//VDP_DISP_CSC_BEGIN
//-------------------------------------------------------------------

///GF AND VIDEO MIX CBM
HI_VOID OPTM_VDP_SetLayerConnect(HI_U32 u32Data)
{
#if 0
    volatile U_VOCTRL VOCTRL;
    
    VOCTRL.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->VOCTRL.u32)));

    VOCTRL.bits.g3_dhd1_sel = u32Data;

    OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->VOCTRL.u32)), VOCTRL.u32);
#endif
}

HI_VOID OPTM_VDP_OpenGFX3(HI_BOOL bOpen)
{
#if 0
    volatile U_CBM_ATTR CBM_ATTR;
    
    CBM_ATTR.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->CBM_ATTR.u32)));

	if (bOpen)
	{
		CBM_ATTR.bits.sur_attr5 = 1;  /*g3 to mixer2*/
    	CBM_ATTR.bits.sur_attr4 = 0;  /*v4 to mixer1*/
	}
	else
	{
		CBM_ATTR.bits.sur_attr5 = 0;  /*g3 to mixer1*/
    	CBM_ATTR.bits.sur_attr4 = 1;  /*v4 to mixer2*/
	}

    OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->CBM_ATTR.u32)), CBM_ATTR.u32);
#endif
}


HI_VOID OPTM_VDP_CBM_SetMixerBkg(OPTM_VDP_CBM_MIX_E u32mixer_id, OPTM_VDP_BKG_S stBkg)
{
    //volatile U_CBM_BKG1 CBM_BKG1;
    //volatile U_CBM_BKG2 CBM_BKG2;
    volatile U_MIXG0_BKG MIXG0_BKG;
    volatile U_MIXG0_BKALPHA MIXG0_BKALPHA;
    //volatile U_MIXV0_BKG MIXV0_BKG;

#if 0
    if(u32mixer_id == VDP_CBM_MIX0)
    {
        /* DHD0  mixer link */
        CBM_BKG1.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->CBM_BKG1.u32)));;
        CBM_BKG1.bits.cbm_bkgy1  = stBkg.u32BkgY;
        CBM_BKG1.bits.cbm_bkgcb1 = stBkg.u32BkgU;
        CBM_BKG1.bits.cbm_bkgcr1 = stBkg.u32BkgV;
        OPTM_VDP_RegWrite((HI_U32)&(pOptmVdpReg->CBM_BKG1.u32), CBM_BKG1.u32); 
    }
    else if(u32mixer_id == VDP_CBM_MIX1)
    {
        /* DHD1(DSD) mixer link */
        CBM_BKG2.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->CBM_BKG2.u32)));;
        CBM_BKG2.bits.cbm_bkgy2  = stBkg.u32BkgY;
        CBM_BKG2.bits.cbm_bkgcb2 = stBkg.u32BkgU;
        CBM_BKG2.bits.cbm_bkgcr2 = stBkg.u32BkgV;
        OPTM_VDP_RegWrite((HI_U32)&(pOptmVdpReg->CBM_BKG2.u32), CBM_BKG2.u32); 
    }
    else if(u32mixer_id == VDP_CBM_MIXG0)
#else
    if(u32mixer_id == VDP_CBM_MIXG0)
#endif
    {
        /* G0 mixer link */
        MIXG0_BKG.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->MIXG0_BKG.u32)));;
        MIXG0_BKG.bits.mixer_bkgy  = stBkg.u32BkgY;
        MIXG0_BKG.bits.mixer_bkgcb = stBkg.u32BkgU;
        MIXG0_BKG.bits.mixer_bkgcr = stBkg.u32BkgV;
        OPTM_VDP_RegWrite((HI_U32)&(pOptmVdpReg->MIXG0_BKG.u32), MIXG0_BKG.u32); 

        MIXG0_BKALPHA.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->MIXG0_BKALPHA.u32)));;
        MIXG0_BKALPHA.bits.mixer_alpha  = stBkg.u32BkgA;
        OPTM_VDP_RegWrite((HI_U32)&(pOptmVdpReg->MIXG0_BKALPHA.u32), MIXG0_BKALPHA.u32); 
    }
	else if(u32mixer_id == VDP_CBM_MIXG1)
	{
		return ;
	}
#if 0
    else if(u32mixer_id == VDP_CBM_MIXV0)
    {
        MIXV0_BKG.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->MIXV0_BKG.u32)));;
        MIXV0_BKG.bits.mixer_bkgy  = stBkg.u32BkgY;
        MIXV0_BKG.bits.mixer_bkgcb = stBkg.u32BkgU;
        MIXV0_BKG.bits.mixer_bkgcr = stBkg.u32BkgV;
        OPTM_VDP_RegWrite((HI_U32)&(pOptmVdpReg->MIXV0_BKG.u32), MIXV0_BKG.u32); 
    }
#endif
    else
    {
        HI_PRINT("Error! OPTM_VDP_CBM_SetMixerBkg() Select Wrong mixer ID\n");
    }

    return ;
}

HI_VOID OPTM_VDP_CBM_ResetMixerPrio(OPTM_VDP_CBM_MIX_E u32mixer_id)
{
    volatile U_MIXG0_MIX MIXG0_MIX;
    volatile U_MIXV0_MIX MIXV0_MIX;
    volatile U_CBM_MIX1 CBM_MIX1;
    volatile U_CBM_MIX2 CBM_MIX2;

    if (u32mixer_id == VDP_CBM_MIXG0)
    {
        MIXG0_MIX.u32 = 0;
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->MIXG0_MIX.u32)), MIXG0_MIX.u32);
    }
    else if (u32mixer_id == VDP_CBM_MIXV0)
    {
        MIXV0_MIX.u32 = 0;
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->MIXV0_MIX.u32)), MIXV0_MIX.u32);
    }
    else if (u32mixer_id == VDP_CBM_MIXV1)
    {
        //reserved
    }
    else if(u32mixer_id == VDP_CBM_MIX0)//DHD0
    {
        CBM_MIX1.u32 = 0;
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->CBM_MIX1.u32)), CBM_MIX1.u32);
    }
    else if(u32mixer_id == VDP_CBM_MIX1)//DHD1
    {
        CBM_MIX2.u32 = 0;
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->CBM_MIX2.u32)), CBM_MIX2.u32);
    }
    else
    {
        HI_PRINT("Error, OPTM_VDP_CBM_ResetMixerPrio() Set mixer  select wrong layer ID\n");
    }

    return ;
}

HI_VOID OPTM_VDP_CBM_GetMixerPrio(OPTM_VDP_CBM_MIX_E u32mixer_id, HI_U32 *pu32prio)
{	
    OPTM_HAL_CHECK_NULL_POINT(pu32prio);
	
    if (u32mixer_id == VDP_CBM_MIXG0)
    {
        *pu32prio = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->MIXG0_MIX.u32)));
    }
	else
	{
		*pu32prio= 0;
	}
}

HI_VOID OPTM_VDP_SetMixgPrio(OPTM_VDP_CBM_MIX_E u32mixer_id,HI_U32 u32prio)
{
    volatile U_MIXG0_MIX MIXG0_MIX;
    
    if (u32mixer_id == VDP_CBM_MIXG0)
    {
        MIXG0_MIX.u32 = u32prio;
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->MIXG0_MIX.u32)), MIXG0_MIX.u32);
    }
}


HI_VOID OPTM_VDP_CBM_SetMixerPrio(OPTM_VDP_CBM_MIX_E u32mixer_id,HI_U32 u32layer_id,HI_U32 u32prio)
{
    volatile U_MIXG0_MIX MIXG0_MIX;
    //volatile U_MIXV0_MIX MIXV0_MIX;
    //volatile U_CBM_MIX1 CBM_MIX1;
    //volatile U_CBM_MIX2 CBM_MIX2;
    
    if (u32mixer_id == VDP_CBM_MIXG0)
    {
        switch(u32prio)
        {
            case 0:
            {
                MIXG0_MIX.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->MIXG0_MIX.u32)));
                MIXG0_MIX.bits.mixer_prio0 = u32layer_id+1;
                OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->MIXG0_MIX.u32)), MIXG0_MIX.u32);
                break;
            }
            case 1:
            {
                MIXG0_MIX.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->MIXG0_MIX.u32)));
                MIXG0_MIX.bits.mixer_prio1 = u32layer_id+1;
                OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->MIXG0_MIX.u32)), MIXG0_MIX.u32);
                break;
            }
            case 2:
            {
                MIXG0_MIX.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->MIXG0_MIX.u32)));
                MIXG0_MIX.bits.mixer_prio2 = u32layer_id+1;
                OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->MIXG0_MIX.u32)), MIXG0_MIX.u32);
                break;
            }
            case 3:
            {
                MIXG0_MIX.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->MIXG0_MIX.u32)));
                MIXG0_MIX.bits.mixer_prio3 = u32layer_id+1;
                OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->MIXG0_MIX.u32)), MIXG0_MIX.u32);
                break;
            }
            default:
            {
                HI_PRINT("Error, Vou_SetCbmMixerPrio() Set mixer  select wrong layer ID\n");
                return ;
            }

        }
    }
    else if(u32mixer_id == VDP_CBM_MIXG1)
        ;
#if 0
    else if(u32mixer_id == VDP_CBM_MIXV0)
    {
        switch(u32prio)
        {
            case 0:
            {
                MIXV0_MIX.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->MIXV0_MIX.u32)));
                MIXV0_MIX.bits.mixer_prio0 = u32layer_id+1;
                OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->MIXV0_MIX.u32)), MIXV0_MIX.u32);
                break;
            }
            case 1:
            {
                MIXV0_MIX.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->MIXV0_MIX.u32)));
                MIXV0_MIX.bits.mixer_prio1 = u32layer_id+1;
                OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->MIXV0_MIX.u32)), MIXV0_MIX.u32);
                break;
            }
            default:
            {
                HI_PRINT("Error, Vou_SetCbmMixerPrio() Set mixer  select wrong layer ID\n");
                return ;
            }

        }
    }
    else if(u32mixer_id == VDP_CBM_MIXV1)
        ;
    else if(u32mixer_id == VDP_CBM_MIX0)//DHD0
    {
        switch(u32prio)
        {
            case 0:
            {
                CBM_MIX1.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->CBM_MIX1.u32)));
                CBM_MIX1.bits.mixer_prio0 = u32layer_id+1;
                OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->CBM_MIX1.u32)), CBM_MIX1.u32);
                break;
            }
            case 1:
            {
                CBM_MIX1.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->CBM_MIX1.u32)));
                CBM_MIX1.bits.mixer_prio1 = u32layer_id+1;
                OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->CBM_MIX1.u32)), CBM_MIX1.u32);
                break;
            }
            default:
            {
                HI_PRINT("Error, Vou_SetCbmMixerPrio() Set mixer  select wrong layer ID\n");
                return ;
            }
        }
    }
    else if(u32mixer_id == VDP_CBM_MIX1)//DHD1
    {
        switch(u32prio)
        {
            case 0:
            {
                CBM_MIX2.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->CBM_MIX2.u32)));
                CBM_MIX2.bits.mixer_prio0 = u32layer_id+1;
                OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->CBM_MIX2.u32)), CBM_MIX2.u32);
                break;
            }
            case 1:
            {
                CBM_MIX2.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->CBM_MIX2.u32)));
                CBM_MIX2.bits.mixer_prio1 = u32layer_id+1;
                OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->CBM_MIX2.u32)), CBM_MIX2.u32);
                break;
            }
            default:
            {
                HI_PRINT("Error, Vou_SetCbmMixerPrio() Set mixer  select wrong layer ID\n");
                return ;
            }


        }

    }
#endif
}
//-------------------------------------------------------------------
//GFX_BEGIN
//-------------------------------------------------------------------

HI_VOID  OPTM_VDP_GFX_GetLayerEnable(HI_U32 u32Data, HI_U32 *pu32Enable )
{
    volatile U_G0_CTRL GFX_CTRL;

	OPTM_HAL_CHECK_LAYER_VALID(u32Data);

	OPTM_HAL_CHECK_NULL_POINT(pu32Enable);
    
    GFX_CTRL.u32 = OPTM_VDP_RegRead(((HI_U32)&(pOptmVdpReg->G0_CTRL.u32) + u32Data * OPTM_GFX_OFFSET));

	*pu32Enable  = GFX_CTRL.bits.surface_en;
    return ;
}

HI_VOID  OPTM_VDP_GFX_SetLayerEnable(HI_U32 u32Data, HI_U32 u32bEnable )
{
    volatile U_G0_CTRL G0_CTRL;

	OPTM_HAL_CHECK_LAYER_VALID(u32Data);
    
    G0_CTRL.u32 = OPTM_VDP_RegRead(((HI_U32)&(pOptmVdpReg->G0_CTRL.u32) + u32Data * OPTM_GFX_OFFSET));
    G0_CTRL.bits.surface_en = u32bEnable;	
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->G0_CTRL.u32) + u32Data * OPTM_GFX_OFFSET), G0_CTRL.u32); 
	//printk("+++++++++++++layer enable %d.\n", u32bEnable);

    return ;
}

HI_VOID  OPTM_VDP_GFX_SetNoSecFlag    (HI_U32 u32Data, HI_U32 u32Enable )
{
    volatile U_G0_CTRL  G0_CTRL;
	
	OPTM_HAL_CHECK_LAYER_VALID(u32Data);

    G0_CTRL.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->G0_CTRL.u32) + u32Data * OPTM_GFX_OFFSET));
    G0_CTRL.bits.nosec_flag= u32Enable ;
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->G0_CTRL.u32) + u32Data * OPTM_GFX_OFFSET), G0_CTRL.u32);
   
    return ;
}

HI_VOID  OPTM_VDP_GFX_SetDcmpEnable(HI_U32 u32Data, HI_U32 u32bEnable)
{
    volatile U_G0_CTRL G0_CTRL;

	OPTM_HAL_CHECK_LAYER_VALID(u32Data);
    
    G0_CTRL.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->G0_CTRL.u32) + u32Data * OPTM_GFX_OFFSET));
    G0_CTRL.bits.dcmp_en = u32bEnable ;
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->G0_CTRL.u32) + u32Data * OPTM_GFX_OFFSET), G0_CTRL.u32); 

    return ;
}

HI_VOID OPTM_VDP_GFX_SetDcmpAddr(HI_U32 u32Data, HI_U32 u32LAddr)
{
    volatile U_G0_DCMP_ADDR G0_DCMP_ADDR;
	
	OPTM_HAL_CHECK_LAYER_VALID(u32Data);

    G0_DCMP_ADDR.u32 = u32LAddr;
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->G0_DCMP_ADDR.u32)+ u32Data * OPTM_GFX_OFFSET), G0_DCMP_ADDR.u32);
}

HI_VOID OPTM_VDP_GFX_SetDcmpNAddr(HI_U32 u32Data, HI_U32 u32NAddr)
{
    volatile U_G0_DCMP_NADDR G0_DCMP_NADDR;
	
	OPTM_HAL_CHECK_LAYER_VALID(u32Data);

    G0_DCMP_NADDR.u32 = u32NAddr;
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->G0_DCMP_NADDR.u32)+ u32Data * OPTM_GFX_OFFSET), G0_DCMP_NADDR.u32);
}

HI_VOID OPTM_VDP_GFX_SetDcmpOffset(HI_U32 u32Data, HI_U32 u32Offset)
{
    volatile U_G0_DCMP_OFFSET G0_DCMP_OFFSET;
	
	OPTM_HAL_CHECK_LAYER_VALID(u32Data);

    G0_DCMP_OFFSET.u32 = u32Offset;
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->G0_DCMP_OFFSET.u32)+ u32Data * OPTM_GFX_OFFSET), G0_DCMP_OFFSET.u32);
}


HI_VOID OPTM_VDP_GFX_GetLayerAddr(HI_U32 u32Data, HI_U32 *pu32Addr)
{
    volatile U_G0_ADDR GFX_ADDR;
	
	OPTM_HAL_CHECK_LAYER_VALID(u32Data);

	OPTM_HAL_CHECK_NULL_POINT(pu32Addr);
	
    GFX_ADDR.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->G0_ADDR.u32)+ u32Data * OPTM_GFX_OFFSET));
	
	*pu32Addr = GFX_ADDR.u32;
	
	return;    
}


HI_VOID OPTM_VDP_GFX_SetLayerAddr(HI_U32 u32Data, HI_U32 u32LAddr, HI_U32 u32Stride)
{
    volatile U_G0_ADDR G0_ADDR;
    volatile U_G0_STRIDE G0_STRIDE;
	
	OPTM_HAL_CHECK_LAYER_VALID(u32Data);

    G0_ADDR.u32 = u32LAddr;
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->G0_ADDR.u32)+ u32Data * OPTM_GFX_OFFSET), G0_ADDR.u32);

    G0_STRIDE.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->G0_STRIDE.u32)+ u32Data * OPTM_GFX_OFFSET));
    G0_STRIDE.bits.surface_stride = u32Stride;
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->G0_STRIDE.u32)+ u32Data * OPTM_GFX_OFFSET), G0_STRIDE.u32);
}

HI_VOID OPTM_VDP_GFX_SetLayerAddrEX(HI_U32 u32Data, HI_U32 u32LAddr)
{
    volatile U_G0_ADDR G0_ADDR;
	
	OPTM_HAL_CHECK_LAYER_VALID(u32Data);

    G0_ADDR.u32 = u32LAddr;
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->G0_ADDR.u32)+ u32Data * OPTM_GFX_OFFSET), G0_ADDR.u32);
	
}

HI_VOID OPTM_VDP_GFX_GetLayerStride(HI_U32 u32Data, HI_U32 *pu32Stride)
{
    volatile U_G0_STRIDE GFX_STRIDE;
	
	OPTM_HAL_CHECK_LAYER_VALID(u32Data);

    GFX_STRIDE.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->G0_STRIDE.u32)+ u32Data * OPTM_GFX_OFFSET));
    *pu32Stride = GFX_STRIDE.bits.surface_stride*STRIDE_ALIGN;

	return;
}

HI_VOID OPTM_VDP_GFX_SetLayerStride(HI_U32 u32Data, HI_U32 u32Stride)
{
    volatile U_G0_STRIDE G0_STRIDE;
	
	OPTM_HAL_CHECK_LAYER_VALID(u32Data);
    G0_STRIDE.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->G0_STRIDE.u32)+ u32Data * OPTM_GFX_OFFSET));
    G0_STRIDE.bits.surface_stride = u32Stride/STRIDE_ALIGN;
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->G0_STRIDE.u32)+ u32Data * OPTM_GFX_OFFSET), G0_STRIDE.u32);
}

HI_VOID OPTM_VDP_GFX_SetLutAddr(HI_U32 u32Data, HI_U32 u32LutAddr)
{
    volatile U_G0_PARAADDR G0_PARAADDR;
	
	OPTM_HAL_CHECK_LAYER_VALID(u32Data);

    G0_PARAADDR.u32 = u32LutAddr;
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->G0_PARAADDR.u32)+ u32Data * OPTM_GFX_OFFSET), G0_PARAADDR.u32);
}

HI_VOID OPTM_VDP_GFX_SetGammaEnable(HI_U32 u32Data, HI_U32 u32GmmEn)
{
#if 0
    volatile U_G0_CTRL G0_CTRL;
	
	OPTM_HAL_CHECK_LAYER_VALID(u32Data);
    
    G0_CTRL.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->G0_CTRL.u32)+u32Data*OPTM_GFX_MAX));
    G0_CTRL.bits.gmm_en = u32GmmEn;
    OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->G0_CTRL.u32)+u32Data*OPTM_GFX_MAX), G0_CTRL.u32);
#endif	
}

HI_VOID OPTM_VDP_GFX_GetInDataFmt(HI_U32 u32Data, OPTM_VDP_GFX_IFMT_E  *enDataFmt)
{
	volatile U_G0_CTRL G0_CTRL;

	OPTM_HAL_CHECK_LAYER_VALID(u32Data);
    
    G0_CTRL.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->G0_CTRL.u32) + u32Data * OPTM_GFX_OFFSET));
    *enDataFmt = G0_CTRL.bits.ifmt ;

    return ;
}

HI_VOID  OPTM_VDP_GFX_SetInDataFmt(HI_U32 u32Data, OPTM_VDP_GFX_IFMT_E  enDataFmt)
{
    volatile U_G0_CTRL G0_CTRL;

	OPTM_HAL_CHECK_LAYER_VALID(u32Data);
    
    G0_CTRL.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->G0_CTRL.u32) + u32Data * OPTM_GFX_OFFSET));
    G0_CTRL.bits.ifmt = enDataFmt;
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->G0_CTRL.u32) + u32Data * OPTM_GFX_OFFSET), G0_CTRL.u32);

    return ;
}


HI_VOID OPTM_VDP_GFX_SetBitExtend(HI_U32 u32Data, OPTM_VDP_GFX_BITEXTEND_E u32mode)
{
    volatile U_G0_CTRL G0_CTRL;
	
	OPTM_HAL_CHECK_LAYER_VALID(u32Data);

    G0_CTRL.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->G0_CTRL.u32)+ u32Data * OPTM_GFX_OFFSET));
    G0_CTRL.bits.bitext = u32mode;
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->G0_CTRL.u32)+ u32Data * OPTM_GFX_OFFSET), G0_CTRL.u32); 
}

//HI_VOID VDP_SetGfxPalpha(VDP_DISP_LAYER_E enLayer,HI_U32 bAlphaEn,HI_U32 bArange,HI_U32 u32Alpha0,HI_U32 u32Alpha1)//CBM

HI_VOID OPTM_VDP_GFX_SetColorKey(HI_U32 u32Data,HI_U32  bkeyEn,OPTM_VDP_GFX_CKEY_S stKey )
{
    volatile U_G0_CKEYMAX G0_CKEYMAX;
    volatile U_G0_CKEYMIN G0_CKEYMIN;
    volatile U_G0_CBMPARA G0_CBMPARA;
	
	OPTM_HAL_CHECK_LAYER_VALID(u32Data);

    G0_CKEYMAX.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->G0_CKEYMAX.u32)+ u32Data * OPTM_GFX_OFFSET));
    G0_CKEYMAX.bits.keyr_max = stKey.u32Key_r_max;
    G0_CKEYMAX.bits.keyg_max = stKey.u32Key_g_max;
    G0_CKEYMAX.bits.keyb_max = stKey.u32Key_b_max;
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->G0_CKEYMAX.u32)+ u32Data * OPTM_GFX_OFFSET), G0_CKEYMAX.u32);
	
    G0_CKEYMIN.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->G0_CKEYMIN.u32)+ u32Data * OPTM_GFX_OFFSET));
    G0_CKEYMIN.bits.keyr_min = stKey.u32Key_r_min;
    G0_CKEYMIN.bits.keyg_min = stKey.u32Key_g_min;
    G0_CKEYMIN.bits.keyb_min = stKey.u32Key_b_min;
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->G0_CKEYMIN.u32)+ u32Data * OPTM_GFX_OFFSET), G0_CKEYMIN.u32);

    G0_CBMPARA.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->G0_CBMPARA.u32)+ u32Data * OPTM_GFX_OFFSET));
    G0_CBMPARA.bits.key_en = bkeyEn;
    G0_CBMPARA.bits.key_mode = stKey.bKeyMode;            
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->G0_CBMPARA.u32)+ u32Data * OPTM_GFX_OFFSET), G0_CBMPARA.u32); 

}

HI_VOID OPTM_VDP_GFX_GetColorKey(HI_U32 u32Data, HI_U32 *pbkeyEn,OPTM_VDP_GFX_CKEY_S *pstKey)
{
    volatile U_G0_CKEYMAX G0_CKEYMAX;
    volatile U_G0_CKEYMIN G0_CKEYMIN;
    volatile U_G0_CBMPARA G0_CBMPARA;
	
	OPTM_HAL_CHECK_LAYER_VALID(u32Data);

    G0_CKEYMAX.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->G0_CKEYMAX.u32)+ u32Data * OPTM_GFX_OFFSET));
	pstKey->u32Key_r_max = G0_CKEYMAX.bits.keyr_max;
	pstKey->u32Key_g_max = G0_CKEYMAX.bits.keyg_max;
	pstKey->u32Key_b_max = G0_CKEYMAX.bits.keyb_max;
	
    G0_CKEYMIN.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->G0_CKEYMIN.u32)+ u32Data * OPTM_GFX_OFFSET));
	pstKey->u32Key_r_min = G0_CKEYMIN.bits.keyr_min;
	pstKey->u32Key_g_min = G0_CKEYMIN.bits.keyg_min;
	pstKey->u32Key_b_min = G0_CKEYMIN.bits.keyb_min;

    G0_CBMPARA.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->G0_CBMPARA.u32)+ u32Data * OPTM_GFX_OFFSET));
	*pbkeyEn = G0_CBMPARA.bits.key_en;
	pstKey->bKeyMode = G0_CBMPARA.bits.key_mode;		 
}

HI_VOID OPTM_VDP_GFX_SetKeyMask(HI_U32 u32Data, OPTM_VDP_GFX_MASK_S stMsk)
{
    volatile U_G0_CMASK G0_CMASK;
	
	OPTM_HAL_CHECK_LAYER_VALID(u32Data);

    G0_CMASK.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->G0_CMASK.u32)+ u32Data * OPTM_GFX_OFFSET));
    G0_CMASK.bits.kmsk_r = stMsk.u32Mask_r;
    G0_CMASK.bits.kmsk_g = stMsk.u32Mask_g;
    G0_CMASK.bits.kmsk_b = stMsk.u32Mask_b;
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->G0_CMASK.u32)+ u32Data * OPTM_GFX_OFFSET), G0_CMASK.u32);

}

HI_VOID OPTM_VDP_GFX_GetKeyMask(HI_U32 u32Data, OPTM_VDP_GFX_MASK_S *pstMsk)
{
    volatile U_G0_CMASK G0_CMASK;
	
	OPTM_HAL_CHECK_LAYER_VALID(u32Data);

    G0_CMASK.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->G0_CMASK.u32)+ u32Data * OPTM_GFX_OFFSET));
	pstMsk->u32Mask_r = G0_CMASK.bits.kmsk_r;
	pstMsk->u32Mask_g = G0_CMASK.bits.kmsk_g;
	pstMsk->u32Mask_b = G0_CMASK.bits.kmsk_b;
}



HI_VOID OPTM_VDP_GFX_SetReadMode(HI_U32 u32Data, HI_U32 u32Mode)
{
    volatile U_G0_CTRL G0_CTRL;
	
	OPTM_HAL_CHECK_LAYER_VALID(u32Data);

    G0_CTRL.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->G0_CTRL.u32)+ u32Data * OPTM_GFX_OFFSET));
    G0_CTRL.bits.read_mode = u32Mode;
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->G0_CTRL.u32)+ u32Data * OPTM_GFX_OFFSET), G0_CTRL.u32); 


}

HI_VOID  OPTM_VDP_GFX_SetParaUpd  (HI_U32 u32Data, OPTM_VDP_DISP_COEFMODE_E enMode )
{
    volatile U_G0_PARAUP G0_PARAUP;
	
	OPTM_HAL_CHECK_LAYER_VALID(u32Data);

    if (enMode == VDP_DISP_COEFMODE_LUT || enMode == VDP_DISP_COEFMODE_ALL)
    {
        G0_PARAUP.bits.gdc_paraup = 0x1;
        OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->G0_PARAUP.u32)+ u32Data * OPTM_GFX_OFFSET), G0_PARAUP.u32);
    }



}
HI_VOID OPTM_VDP_GFX_SetDispMode(HI_U32 u32Data, OPTM_VDP_DISP_MODE_E enDispMode)
{
	return;
}
//3D MODE
HI_VOID  OPTM_VDP_GFX_SetThreeDimDofEnable    (HI_U32 u32Data, HI_U32 bEnable)
{
	volatile U_G0_DOF_CTRL  G0_DOF_CTRL;
	
	OPTM_HAL_CHECK_LAYER_VALID(u32Data);
    
    G0_DOF_CTRL.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->G0_DOF_CTRL.u32)+ u32Data * OPTM_GFX_OFFSET));
    G0_DOF_CTRL.bits.dof_en = bEnable;
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->G0_DOF_CTRL.u32)+ u32Data * OPTM_GFX_OFFSET), G0_DOF_CTRL.u32);
}

HI_VOID  OPTM_VDP_GFX_SetThreeDimDofStep(HI_U32 u32Data, HI_S32 s32LStep, HI_S32 s32RStep)
{
	volatile U_G0_DOF_STEP  G0_DOF_STEP;
	
	OPTM_HAL_CHECK_LAYER_VALID(u32Data);
    
    G0_DOF_STEP.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->G0_DOF_STEP.u32)+ u32Data * OPTM_GFX_OFFSET));
    G0_DOF_STEP.bits.right_step= s32RStep;
    G0_DOF_STEP.bits.left_step = s32LStep;
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->G0_DOF_STEP.u32)+ u32Data * OPTM_GFX_OFFSET), G0_DOF_STEP.u32);
}


HI_VOID OPTM_VDP_GFX_SetThreeDimEnable(HI_U32 u32Data,HI_U32 bTrue)
{
/*    
    volatile U_G0_CTRL G0_CTRL;
	
	OPTM_HAL_CHECK_LAYER_VALID(u32Data);

    G0_CTRL.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->G0_CTRL.u32)+ u32Data * OPTM_GFX_OFFSET));
    G0_CTRL.bits.trid_en = bTrue ;
    OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->G0_CTRL.u32)+ u32Data * OPTM_GFX_OFFSET), G0_CTRL.u32);
    */
}

HI_VOID  OPTM_VDP_GFX_GetLayerInRect(HI_U32 u32Data, OPTM_VDP_DISP_RECT_S *stInRect)
{
    U_G0_SFPOS G0_SFPOS;
    U_G0_VFPOS G0_VFPOS;
    U_G0_VLPOS G0_VLPOS;
    U_G0_DFPOS G0_DFPOS;
    U_G0_DLPOS G0_DLPOS;
    U_G0_IRESO G0_IRESO;
    
    G0_SFPOS.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->G0_SFPOS.u32)+ u32Data * OPTM_GFX_OFFSET));
    stInRect->u32SX = G0_SFPOS.bits.src_xfpos;

    G0_VFPOS.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->G0_VFPOS.u32) + u32Data * OPTM_VID_OFFSET));
    stInRect->u32VX = G0_VFPOS.bits.video_xfpos;
    stInRect->u32VY = G0_VFPOS.bits.video_yfpos;
    
    G0_DFPOS.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->G0_DFPOS.u32)+ u32Data * OPTM_GFX_OFFSET));
    stInRect->u32DXS = G0_DFPOS.bits.disp_xfpos;
    stInRect->u32DYS = G0_DFPOS.bits.disp_yfpos;
    
    G0_DLPOS.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->G0_DLPOS.u32)+ u32Data * OPTM_GFX_OFFSET));
    stInRect->u32DXL = G0_DLPOS.bits.disp_xlpos + 1;
    stInRect->u32DYL = G0_DLPOS.bits.disp_ylpos + 1;

    G0_VLPOS.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->G0_VLPOS.u32) + u32Data * OPTM_VID_OFFSET));
    stInRect->u32VXL = G0_VLPOS.bits.video_xlpos + 1;
    stInRect->u32VYL = G0_VLPOS.bits.video_ylpos + 1;

    G0_IRESO.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->G0_IRESO.u32)+ u32Data * OPTM_GFX_OFFSET));
    stInRect->u32IWth = G0_IRESO.bits.iw + 1;
    stInRect->u32IHgt = G0_IRESO.bits.ih + 1;
}
HI_VOID  OPTM_VDP_GFX_SetLayerReso     (HI_U32 u32Data, OPTM_VDP_DISP_RECT_S  stRect)
{
    volatile U_G0_SFPOS G0_SFPOS;
    volatile U_G0_VFPOS G0_VFPOS;
    volatile U_G0_VLPOS G0_VLPOS;
    volatile U_G0_DFPOS G0_DFPOS;
    volatile U_G0_DLPOS G0_DLPOS;
    volatile U_G0_IRESO G0_IRESO;
    //U_G0_ORESO G0_ORESO;

	OPTM_HAL_CHECK_LAYER_VALID(u32Data);

    // Read source position
    G0_SFPOS.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->G0_SFPOS.u32)+ u32Data * OPTM_GFX_OFFSET));
    G0_SFPOS.bits.src_xfpos = stRect.u32SX;
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->G0_SFPOS.u32)+ u32Data * OPTM_GFX_OFFSET), G0_SFPOS.u32);

    G0_VFPOS.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->G0_VFPOS.u32) + u32Data * OPTM_VID_OFFSET));
    G0_VFPOS.bits.video_xfpos = stRect.u32VX;
    G0_VFPOS.bits.video_yfpos = stRect.u32VY;
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->G0_VFPOS.u32) + u32Data * OPTM_VID_OFFSET), G0_VFPOS.u32); 

    G0_VLPOS.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->G0_VLPOS.u32) + u32Data * OPTM_VID_OFFSET));
    G0_VLPOS.bits.video_xlpos = stRect.u32VX + stRect.u32OWth-1;
    G0_VLPOS.bits.video_ylpos = stRect.u32VY + stRect.u32OHgt-1;
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->G0_VLPOS.u32) + u32Data * OPTM_VID_OFFSET), G0_VLPOS.u32); 

    // display position
    G0_DFPOS.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->G0_DFPOS.u32)+ u32Data * OPTM_GFX_OFFSET));
    G0_DFPOS.bits.disp_xfpos = stRect.u32DXS;
    G0_DFPOS.bits.disp_yfpos = stRect.u32DYS;
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->G0_DFPOS.u32)+ u32Data * OPTM_GFX_OFFSET), G0_DFPOS.u32);

    G0_DLPOS.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->G0_DLPOS.u32)+ u32Data * OPTM_GFX_OFFSET));
    G0_DLPOS.bits.disp_xlpos = stRect.u32DXL-1;
    G0_DLPOS.bits.disp_ylpos = stRect.u32DYL-1;
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->G0_DLPOS.u32)+ u32Data * OPTM_GFX_OFFSET), G0_DLPOS.u32);

    // input width and height
    G0_IRESO.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->G0_IRESO.u32)+ u32Data * OPTM_GFX_OFFSET));
    G0_IRESO.bits.iw = stRect.u32IWth - 1;
    G0_IRESO.bits.ih = stRect.u32IHgt - 1;
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->G0_IRESO.u32)+ u32Data * OPTM_GFX_OFFSET), G0_IRESO.u32);

    // output width and height
    //G0_ORESO.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->G0_ORESO.u32)+ u32Data * OPTM_GFX_OFFSET));
    //G0_ORESO.bits.ow = stRect.u32OWth - 1;
    //G0_ORESO.bits.oh = stRect.u32OHgt - 1;
    //OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->G0_ORESO.u32)+ u32Data * OPTM_GFX_OFFSET), G0_ORESO.u32);

	return ;
}

HI_VOID  OPTM_VDP_GFX_SetVideoPos     (HI_U32 u32Data, OPTM_VDP_RECT_S  stRect)
{
   volatile U_G0_VFPOS G0_VFPOS;
   volatile U_G0_VLPOS G0_VLPOS;

   OPTM_HAL_CHECK_LAYER_VALID(u32Data);  
  
   //grap position 
   G0_VFPOS.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->G0_VFPOS.u32) + u32Data * OPTM_GFX_OFFSET));
   G0_VFPOS.bits.video_xfpos = stRect.u32X;
   G0_VFPOS.bits.video_yfpos = stRect.u32Y;
   OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->G0_VFPOS.u32) + u32Data * OPTM_GFX_OFFSET), G0_VFPOS.u32); 
   G0_VLPOS.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->G0_VLPOS.u32) + u32Data * OPTM_GFX_OFFSET));
   G0_VLPOS.bits.video_xlpos = stRect.u32X + stRect.u32Wth - 1;
   G0_VLPOS.bits.video_ylpos = stRect.u32Y + stRect.u32Hgt - 1;
   OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->G0_VLPOS.u32) + u32Data * OPTM_GFX_OFFSET), G0_VLPOS.u32); 
   return ;
}   
    
HI_VOID  OPTM_VDP_GFX_SetDispPos     (HI_U32 u32Data, OPTM_VDP_RECT_S  stRect)
{
   volatile U_G0_DFPOS G0_DFPOS;
   volatile U_G0_DLPOS G0_DLPOS;

   OPTM_HAL_CHECK_LAYER_VALID(u32Data);
  
   /*video position */ 
   G0_DFPOS.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->G0_DFPOS.u32) + u32Data * OPTM_GFX_OFFSET));
   G0_DFPOS.bits.disp_xfpos = stRect.u32X;
   G0_DFPOS.bits.disp_yfpos = stRect.u32Y;
   OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->G0_DFPOS.u32) + u32Data * OPTM_GFX_OFFSET), G0_DFPOS.u32); 
   
   G0_DLPOS.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->G0_DLPOS.u32) + u32Data * OPTM_GFX_OFFSET));
   G0_DLPOS.bits.disp_xlpos = stRect.u32X + stRect.u32Wth - 1;
   G0_DLPOS.bits.disp_ylpos = stRect.u32Y + stRect.u32Hgt - 1;
   OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->G0_DLPOS.u32) + u32Data * OPTM_GFX_OFFSET), G0_DLPOS.u32); 
   return ;
}   
    


HI_VOID  OPTM_VDP_GFX_SetInReso(HI_U32 u32Data, OPTM_VDP_RECT_S  stRect)
{
   volatile U_G0_IRESO G0_IRESO;

   OPTM_HAL_CHECK_LAYER_VALID(u32Data);
 
   //grap position 
   G0_IRESO.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->G0_IRESO.u32) + u32Data * OPTM_GFX_OFFSET));
   G0_IRESO.bits.iw = stRect.u32Wth - 1;
   G0_IRESO.bits.ih = stRect.u32Hgt - 1;
   OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->G0_IRESO.u32) + u32Data * OPTM_GFX_OFFSET), G0_IRESO.u32); 
   return ;
}


//HI_VOID VDP_GFX_SetCmpAddr(HI_U32 u32Data, HI_U32 u32CmpAddr)
//{
//    U_G0_CMPADDR G0_CMPADDR;
//    if(u32Data >= OPTM_GFX_MAX)
//    {
//        HI_PRINT("Error,VDP_GFX_SetCmpAddr() Select Wrong Graph Layer ID\n");
//        return ;
//    }
//    G0_CMPADDR.u32 = u32CmpAddr;
//    OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->G0_CMPADDR.u32)+ u32Data * OPTM_GFX_OFFSET), G0_CMPADDR.u32);
//
//}

HI_VOID  OPTM_VDP_GFX_SetRegUp (HI_U32 u32Data)
{
    volatile U_G0_UPD G0_UPD;

    /* GO layer register update */
	OPTM_HAL_CHECK_LAYER_VALID(u32Data);

    G0_UPD.bits.regup = 0x1;
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->G0_UPD.u32) + u32Data * OPTM_GFX_OFFSET), G0_UPD.u32); 

    return ;
}

HI_VOID  OPTM_VDP_GFX_SetLayerBkg(HI_U32 u32Data, OPTM_VDP_BKG_S stBkg)
{
    volatile U_G0_BK    G0_BK;
    volatile U_G0_ALPHA G0_ALPHA;

	OPTM_HAL_CHECK_LAYER_VALID(u32Data);

    G0_BK.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->G0_BK.u32) + u32Data * OPTM_GFX_OFFSET));
    G0_BK.bits.vbk_y  = stBkg.u32BkgY ;
    G0_BK.bits.vbk_cb = stBkg.u32BkgU;
    G0_BK.bits.vbk_cr = stBkg.u32BkgV;
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->G0_BK.u32) + u32Data * OPTM_GFX_OFFSET), G0_BK.u32); 

    G0_ALPHA.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->G0_ALPHA.u32) + u32Data * OPTM_GFX_OFFSET));
    G0_ALPHA.bits.vbk_alpha = stBkg.u32BkgA;
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->G0_ALPHA.u32) + u32Data * OPTM_GFX_OFFSET), G0_ALPHA.u32); 

    return ;
}

HI_VOID  OPTM_VDP_GFX_SetLayerGalpha (HI_U32 u32Data, HI_U32 u32Alpha0)
{
    volatile U_G0_CBMPARA G0_CBMPARA;
    
	OPTM_HAL_CHECK_LAYER_VALID(u32Data);
    
    G0_CBMPARA.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->G0_CBMPARA.u32) + u32Data * OPTM_GFX_OFFSET));
    G0_CBMPARA.bits.galpha = u32Alpha0;
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->G0_CBMPARA.u32) + u32Data * OPTM_GFX_OFFSET), G0_CBMPARA.u32); 

    return ;
}
HI_VOID  OPTM_VDP_GFX_GetLayerGalpha(HI_U32 u32Data, HI_U8 *pu8Alpha0)
{
    volatile U_G0_CBMPARA G0_CBMPARA;
    
	OPTM_HAL_CHECK_LAYER_VALID(u32Data);
    
    G0_CBMPARA.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->G0_CBMPARA.u32) + u32Data * OPTM_GFX_OFFSET));
	*pu8Alpha0 = G0_CBMPARA.bits.galpha;

    return ;
}



HI_VOID OPTM_VDP_GFX_SetPalpha(HI_U32 u32Data, HI_U32 bAlphaEn,HI_U32 bArange,HI_U32 u32Alpha0,HI_U32 u32Alpha1)
{
    
    
    volatile U_G0_CBMPARA G0_CBMPARA;
    volatile U_G0_CKEYMIN G0_CKEYMIN;
    volatile U_G0_CKEYMAX G0_CKEYMAX;
	
	OPTM_HAL_CHECK_LAYER_VALID(u32Data);

    G0_CBMPARA.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->G0_CBMPARA.u32)+ u32Data * OPTM_GFX_OFFSET));
    G0_CBMPARA.bits.palpha_en = bAlphaEn;
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->G0_CBMPARA.u32)+ u32Data * OPTM_GFX_OFFSET), G0_CBMPARA.u32);

    G0_CKEYMIN.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->G0_CKEYMIN.u32)+ u32Data * OPTM_GFX_OFFSET));                
    G0_CKEYMIN.bits.va1 = u32Alpha1;
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->G0_CKEYMIN.u32)+ u32Data * OPTM_GFX_OFFSET), G0_CKEYMIN.u32);

    G0_CKEYMAX.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->G0_CKEYMAX.u32)+ u32Data * OPTM_GFX_OFFSET));
    G0_CKEYMAX.bits.va0 = u32Alpha0;                            
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->G0_CKEYMAX.u32)+ u32Data * OPTM_GFX_OFFSET), G0_CKEYMAX.u32); 
	
    G0_CBMPARA.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->G0_CBMPARA.u32)+ u32Data * OPTM_GFX_OFFSET));
    G0_CBMPARA.bits.palpha_range = bArange;            
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->G0_CBMPARA.u32)+ u32Data * OPTM_GFX_OFFSET), G0_CBMPARA.u32);
    
}

HI_VOID  OPTM_VDP_GFX_GetPalpha(HI_U32 u32Data, HI_U32 *pbAlphaEn,HI_U32 *pbArange,HI_U8 *pu8Alpha0,HI_U8 *pu8Alpha1)
{  
    volatile U_G0_CBMPARA G0_CBMPARA;
    volatile U_G0_CKEYMIN G0_CKEYMIN;
    volatile U_G0_CKEYMAX G0_CKEYMAX;
	
	OPTM_HAL_CHECK_LAYER_VALID(u32Data);

    G0_CBMPARA.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->G0_CBMPARA.u32)+ u32Data * OPTM_GFX_OFFSET));
	*pbAlphaEn = G0_CBMPARA.bits.palpha_en;

    G0_CKEYMIN.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->G0_CKEYMIN.u32)+ u32Data * OPTM_GFX_OFFSET));                
	*pu8Alpha1 = G0_CKEYMIN.bits.va1;

    G0_CKEYMAX.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->G0_CKEYMAX.u32)+ u32Data * OPTM_GFX_OFFSET));
	*pu8Alpha0 = G0_CKEYMAX.bits.va0;
	
    G0_CBMPARA.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->G0_CBMPARA.u32)+ u32Data * OPTM_GFX_OFFSET));
	*pbArange = G0_CBMPARA.bits.palpha_range;
    
}



HI_VOID OPTM_VDP_GFX_SetLayerNAddr(HI_U32 u32Data, HI_U32 u32NAddr)
{
    volatile U_G0_NADDR G0_NADDR;
	
	OPTM_HAL_CHECK_LAYER_VALID(u32Data);

    G0_NADDR.u32 = u32NAddr;
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->G0_NADDR.u32)+ u32Data * OPTM_GFX_OFFSET), G0_NADDR.u32);

}

HI_VOID  OPTM_VDP_GFX_SetMuteEnable(HI_U32 u32Data, HI_U32 bEnable)
{
    volatile U_G0_CTRL G0_CTRL;
	
	OPTM_HAL_CHECK_LAYER_VALID(u32Data);

    G0_CTRL.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->G0_CTRL.u32)+ u32Data * OPTM_GFX_OFFSET));
    G0_CTRL.bits.mute_en = bEnable;
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->G0_CTRL.u32) + u32Data * OPTM_GFX_OFFSET), G0_CTRL.u32); 
}

HI_VOID OPTM_VDP_GFX_SetPreMultEnable(HI_U32 u32Data, HI_U32 bEnable)
{
    volatile U_G0_CBMPARA G0_CBMPARA;
	
	OPTM_HAL_CHECK_LAYER_VALID(u32Data);

    G0_CBMPARA.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->G0_CBMPARA.u32)+ u32Data * OPTM_GFX_OFFSET));
    G0_CBMPARA.bits.premult_en = bEnable;
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->G0_CBMPARA.u32)+ u32Data * OPTM_GFX_OFFSET), G0_CBMPARA.u32); 
}

HI_VOID OPTM_VDP_GFX_GetPreMultEnable(HI_U32 u32Data, HI_U32 *pbEnable)
{
    volatile U_G0_CBMPARA G0_CBMPARA;
	
	OPTM_HAL_CHECK_LAYER_VALID(u32Data);

    G0_CBMPARA.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->G0_CBMPARA.u32)+ u32Data * OPTM_GFX_OFFSET));
	*pbEnable = G0_CBMPARA.bits.premult_en;
}


HI_VOID  OPTM_VDP_GFX_SetUpdMode(HI_U32 u32Data, HI_U32 u32Mode)
{
    volatile U_G0_CTRL G0_CTRL;
	
	OPTM_HAL_CHECK_LAYER_VALID(u32Data);

    /* G0 layer register update mode */
    G0_CTRL.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->G0_CTRL.u32)+ u32Data * OPTM_GFX_OFFSET));
    G0_CTRL.bits.upd_mode = u32Mode;
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->G0_CTRL.u32)+ u32Data * OPTM_GFX_OFFSET), G0_CTRL.u32); 
}


HI_VOID  OPTM_VDP_GFX_SetFlipEnable(HI_U32 u32Data, HI_U32 bEnable)
{
    volatile U_G0_CTRL G0_CTRL;
	
	OPTM_HAL_CHECK_LAYER_VALID(u32Data);

    G0_CTRL.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->G0_CTRL.u32)+ u32Data * OPTM_GFX_OFFSET));
    G0_CTRL.bits.flip_en = bEnable;
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->G0_CTRL.u32)+ u32Data * OPTM_GFX_OFFSET), G0_CTRL.u32); 
}

    


//-------------------------------------------------------------------
//GP_BEGIN
//-------------------------------------------------------------------
//
HI_VOID  OPTM_VDP_GP_SetParaUpd       (HI_U32 u32Data, OPTM_VDP_GP_PARA_E enMode)
{
    volatile U_GP0_PARAUP GP0_PARAUP;
    GP0_PARAUP.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_PARAUP.u32) + u32Data * OPTM_GP_OFFSET));
    
    if(u32Data >= OPTM_GP_MAX)
    {
        HI_PRINT("error,OPTM_VDP_GP_SetParaUpd() select wrong GP layer id\n");
        return ;
    }
    if(enMode == VDP_GP_PARA_ZME_HOR)
    {
        GP0_PARAUP.bits.gp0_hcoef_upd = 0x1;
    }
    else if(enMode == VDP_GP_PARA_ZME_VER)
    {
        GP0_PARAUP.bits.gp0_vcoef_upd = 0x1;
    }
    else
    {
        HI_PRINT("error,OPTM_VDP_GP_SetParaUpd() select wrong mode!\n");
    }
    
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_PARAUP.u32) + u32Data * OPTM_GP_OFFSET), GP0_PARAUP.u32); 
    return ;
}


HI_VOID OPTM_VDP_GP_SetIpOrder (HI_U32 u32Data, HI_U32 u32Chn, OPTM_VDP_GP_ORDER_E enIpOrder)
{
    volatile U_GP0_CTRL GP0_CTRL ;

    if(u32Data >= OPTM_GP_MAX)
    {
        HI_PRINT("Error,OPTM_VDP_GP_SetIpOrder() Select Wrong GP Layer ID\n");
        return ;
    }

    GP0_CTRL.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_CTRL.u32)+ u32Data * OPTM_GP_OFFSET));

    if(u32Chn == 0)//channel A
    {
        if(u32Data == OPTM_VDP_LAYER_GP1)
        {
            //for GP0
            GP0_CTRL.bits.ffc_sel = 0;
        }
        switch(enIpOrder)
        {
            case VDP_GP_ORDER_NULL:
            {
                GP0_CTRL.bits.mux1_sel = 2;
                GP0_CTRL.bits.mux2_sel = 3;
                GP0_CTRL.bits.aout_sel = 0;
                GP0_CTRL.bits.bout_sel = 1;

                break;
            }
            case VDP_GP_ORDER_CSC:
            {
                GP0_CTRL.bits.mux1_sel = 0;
                GP0_CTRL.bits.mux2_sel = 3;
                GP0_CTRL.bits.aout_sel = 2;
                GP0_CTRL.bits.bout_sel = 1;

                break;
            }
            case VDP_GP_ORDER_ZME:
            {
                GP0_CTRL.bits.mux1_sel = 2;
                GP0_CTRL.bits.mux2_sel = 0;
                GP0_CTRL.bits.aout_sel = 3;
                GP0_CTRL.bits.bout_sel = 1;

                break;
            }
            case VDP_GP_ORDER_CSC_ZME:
            {
                GP0_CTRL.bits.mux1_sel = 0;
                GP0_CTRL.bits.mux2_sel = 2;
                GP0_CTRL.bits.aout_sel = 3;
                GP0_CTRL.bits.bout_sel = 1;

                break;
            }
            case VDP_GP_ORDER_ZME_CSC:
            {
                GP0_CTRL.bits.mux1_sel = 3;
                GP0_CTRL.bits.mux2_sel = 0;
                GP0_CTRL.bits.aout_sel = 2;
                GP0_CTRL.bits.bout_sel = 1;

                break;
            }
            default://null
            {
                GP0_CTRL.bits.mux1_sel = 2;
                GP0_CTRL.bits.mux2_sel = 3;
                GP0_CTRL.bits.aout_sel = 0;
                GP0_CTRL.bits.bout_sel = 1;

                break;
            }
        }
    }
    else if(u32Chn == 1)//channel B
    {
        if(u32Data == OPTM_VDP_LAYER_GP1)
        {
            //for WBC_GP0
            GP0_CTRL.bits.ffc_sel = 1;
        }
        switch(enIpOrder)
        {
            case VDP_GP_ORDER_NULL:
            {
                GP0_CTRL.bits.mux1_sel = 1;
                GP0_CTRL.bits.mux2_sel = 0;
                GP0_CTRL.bits.aout_sel = 3;
                GP0_CTRL.bits.bout_sel = 2;

                break;
            }
            case VDP_GP_ORDER_CSC:
            {
                GP0_CTRL.bits.mux1_sel = 1;
                GP0_CTRL.bits.mux2_sel = 3;
                GP0_CTRL.bits.aout_sel = 0;
                GP0_CTRL.bits.bout_sel = 2;

                break;
            }
            case VDP_GP_ORDER_ZME:
            {
                GP0_CTRL.bits.mux1_sel = 2;
                GP0_CTRL.bits.mux2_sel = 1;
                GP0_CTRL.bits.aout_sel = 0;
                GP0_CTRL.bits.bout_sel = 3;

                break;
            }
            case VDP_GP_ORDER_CSC_ZME:
            {
                GP0_CTRL.bits.mux1_sel = 1;
                GP0_CTRL.bits.mux2_sel = 2;
                GP0_CTRL.bits.aout_sel = 0;
                GP0_CTRL.bits.bout_sel = 3;

                break;
            }
            case VDP_GP_ORDER_ZME_CSC:
            {
                GP0_CTRL.bits.mux1_sel = 3;
                GP0_CTRL.bits.mux2_sel = 1;
                GP0_CTRL.bits.aout_sel = 0;
                GP0_CTRL.bits.bout_sel = 2;

                break;
            }
            default://null
            {
                GP0_CTRL.bits.mux1_sel = 1;
                GP0_CTRL.bits.mux2_sel = 0;
                GP0_CTRL.bits.aout_sel = 3;
                GP0_CTRL.bits.bout_sel = 2;

                break;
            }
        }
    }
    else
    {
        HI_PRINT("Error,OPTM_VDP_GP_SetIpOrder() Select Wrong GP Channel\n");
    }

    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_CTRL.u32)+ u32Data * OPTM_GP_OFFSET), GP0_CTRL.u32);
}

HI_VOID OPTM_VDP_GP_SetReadMode(HI_U32 u32Data, HI_U32 u32Mode)
{
    volatile U_GP0_CTRL GP0_CTRL;
    if(u32Data >= OPTM_GP_MAX)
    {
        HI_PRINT("Error,OPTM_VDP_GP_SetReadMode Select Wrong Graph Layer ID\n");
        return ;
    }

    GP0_CTRL.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_CTRL.u32)+ u32Data * OPTM_GP_OFFSET));
    GP0_CTRL.bits.read_mode = u32Mode;
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_CTRL.u32)+ u32Data * OPTM_GP_OFFSET), GP0_CTRL.u32); 


}

HI_VOID  OPTM_VDP_GP_GetRect  (HI_U32 u32Data, OPTM_VDP_DISP_RECT_S  *pstRect)
{
    volatile U_GP0_IRESO GP0_IRESO;
    volatile U_GP0_ORESO GP0_ORESO;

    if(u32Data >= OPTM_GP_MAX)
    {
        HI_PRINT("Error,OPTM_VDP_GP_SetRect() Select Wrong GP Layer ID\n");
        return ;
    }

    GP0_IRESO.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_IRESO.u32) + u32Data * OPTM_GP_OFFSET));
    pstRect->u32IWth = GP0_IRESO.bits.iw + 1;
    pstRect->u32IHgt = GP0_IRESO.bits.ih + 1;  

    GP0_ORESO.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ORESO.u32) + u32Data * OPTM_GP_OFFSET));
    pstRect->u32OWth = GP0_ORESO.bits.ow + 1; 
    pstRect->u32OHgt = GP0_ORESO.bits.oh + 1;

    return ;
}

HI_VOID  OPTM_VDP_GP_SetRect  (HI_U32 u32Data, OPTM_VDP_DISP_RECT_S  stRect)
{
    volatile U_GP0_IRESO GP0_IRESO;
    volatile U_GP0_ORESO GP0_ORESO;

    if(u32Data >= OPTM_GP_MAX)
    {
        HI_PRINT("Error,OPTM_VDP_GP_SetRect() Select Wrong GP Layer ID\n");
        return ;
    }

    GP0_IRESO.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_IRESO.u32) + u32Data * OPTM_GP_OFFSET));
    GP0_IRESO.bits.iw = stRect.u32IWth - 1;
    GP0_IRESO.bits.ih = stRect.u32IHgt - 1;
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_IRESO.u32) + u32Data * OPTM_GP_OFFSET), GP0_IRESO.u32); 

    GP0_ORESO.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ORESO.u32) + u32Data * OPTM_GP_OFFSET));
    GP0_ORESO.bits.ow = stRect.u32OWth - 1;
    GP0_ORESO.bits.oh = stRect.u32OHgt - 1;
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ORESO.u32) + u32Data * OPTM_GP_OFFSET), GP0_ORESO.u32); 

    return ;
}

HI_VOID  OPTM_VDP_GP_SetLayerReso (HI_U32 u32Data, OPTM_VDP_DISP_RECT_S  stRect)
{
    volatile U_GP0_VFPOS GP0_VFPOS;
    volatile U_GP0_VLPOS GP0_VLPOS;
    volatile U_GP0_DFPOS GP0_DFPOS;
    volatile U_GP0_DLPOS GP0_DLPOS;
    volatile U_GP0_IRESO GP0_IRESO;
    volatile U_GP0_ORESO GP0_ORESO;

    if(u32Data >= OPTM_GP_MAX)
    {
        HI_PRINT("Error,OPTM_VDP_GP_SetLayerReso() Select Wrong GP Layer ID\n");
        return ;
    }

    /*video position */ 
    GP0_VFPOS.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_VFPOS.u32) + u32Data * OPTM_GP_OFFSET));
    GP0_VFPOS.bits.video_xfpos = stRect.u32VX;
    GP0_VFPOS.bits.video_yfpos = stRect.u32VY;
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_VFPOS.u32) + u32Data * OPTM_GP_OFFSET), GP0_VFPOS.u32); 

    GP0_VLPOS.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_VLPOS.u32) + u32Data * OPTM_GP_OFFSET));
    GP0_VLPOS.bits.video_xlpos = stRect.u32VX + stRect.u32OWth - 1;
    GP0_VLPOS.bits.video_ylpos = stRect.u32VY + stRect.u32OHgt - 1;
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_VLPOS.u32) + u32Data * OPTM_GP_OFFSET), GP0_VLPOS.u32); 

    GP0_DFPOS.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_DFPOS.u32) + u32Data * OPTM_GP_OFFSET));
    GP0_DFPOS.bits.disp_xfpos = stRect.u32DXS;
    GP0_DFPOS.bits.disp_yfpos = stRect.u32DYS;
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_DFPOS.u32) + u32Data * OPTM_GP_OFFSET), GP0_DFPOS.u32); 

    GP0_DLPOS.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_DLPOS.u32) + u32Data * OPTM_GP_OFFSET));
    GP0_DLPOS.bits.disp_xlpos = stRect.u32DXL-1;
    GP0_DLPOS.bits.disp_ylpos = stRect.u32DYL-1;
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_DLPOS.u32) + u32Data * OPTM_GP_OFFSET), GP0_DLPOS.u32); 

    GP0_IRESO.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_IRESO.u32) + u32Data * OPTM_GP_OFFSET));
    GP0_IRESO.bits.iw = stRect.u32IWth - 1;
    GP0_IRESO.bits.ih = stRect.u32IHgt - 1;
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_IRESO.u32) + u32Data * OPTM_GP_OFFSET), GP0_IRESO.u32); 

    GP0_ORESO.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ORESO.u32) + u32Data * OPTM_GP_OFFSET));
    GP0_ORESO.bits.ow = stRect.u32OWth - 1;
    GP0_ORESO.bits.oh = stRect.u32OHgt - 1;
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ORESO.u32) + u32Data * OPTM_GP_OFFSET), GP0_ORESO.u32); 

    return ;
}   

HI_VOID  OPTM_VDP_GP_SetVideoPos     (HI_U32 u32Data, OPTM_VDP_RECT_S  stRect)
{
   volatile U_GP0_VFPOS GP0_VFPOS;
   volatile U_GP0_VLPOS GP0_VLPOS;

   if(u32Data >= OPTM_GP_MAX)
   {
       HI_PRINT("Error,OPTM_VDP_GP_SetVideoPos() Select Wrong Video Layer ID\n");
       return ;
   }
   
  
   /*video position */ 
   GP0_VFPOS.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_VFPOS.u32) + u32Data * OPTM_GP_OFFSET));
   GP0_VFPOS.bits.video_xfpos = stRect.u32X;
   GP0_VFPOS.bits.video_yfpos = stRect.u32Y;
   OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_VFPOS.u32) + u32Data * OPTM_GP_OFFSET), GP0_VFPOS.u32); 
   
   GP0_VLPOS.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_VLPOS.u32) + u32Data * OPTM_GP_OFFSET));
   GP0_VLPOS.bits.video_xlpos = stRect.u32X + stRect.u32Wth - 1;
   GP0_VLPOS.bits.video_ylpos = stRect.u32Y + stRect.u32Hgt - 1;
   OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_VLPOS.u32) + u32Data * OPTM_GP_OFFSET), GP0_VLPOS.u32); 
   return ;
}   
    
HI_VOID  OPTM_VDP_GP_SetDispPos     (HI_U32 u32Data, OPTM_VDP_RECT_S  stRect)
{
   volatile U_GP0_DFPOS GP0_DFPOS;
   volatile U_GP0_DLPOS GP0_DLPOS;

   if(u32Data >= OPTM_GP_MAX)
   {
       HI_PRINT("Error,OPTM_VDP_GP_SetDispPos() Select Wrong Video Layer ID\n");
       return ;
   }
   
  
   /*video position */ 
   GP0_DFPOS.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_DFPOS.u32) + u32Data * OPTM_GP_OFFSET));
   GP0_DFPOS.bits.disp_xfpos = stRect.u32X;
   GP0_DFPOS.bits.disp_yfpos = stRect.u32Y;
   OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_DFPOS.u32) + u32Data * OPTM_GP_OFFSET), GP0_DFPOS.u32); 
   
   GP0_DLPOS.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_DLPOS.u32) + u32Data * OPTM_GP_OFFSET));
   GP0_DLPOS.bits.disp_xlpos = stRect.u32X + stRect.u32Wth - 1;
   GP0_DLPOS.bits.disp_ylpos = stRect.u32Y + stRect.u32Hgt - 1;
   OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_DLPOS.u32) + u32Data * OPTM_GP_OFFSET), GP0_DLPOS.u32); 
   return ;
}   

HI_VOID  OPTM_VDP_GP_SetInReso (HI_U32 u32Data, OPTM_VDP_RECT_S  stRect)
{
    volatile U_GP0_IRESO GP0_IRESO;

    if(u32Data >= OPTM_GP_MAX)
    {
        HI_PRINT("Error,OPTM_VDP_GP_SetInReso() Select Wrong GP Layer ID\n");
        return ;
    }

    GP0_IRESO.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_IRESO.u32) + u32Data * OPTM_GP_OFFSET));
    GP0_IRESO.bits.iw = stRect.u32Wth - 1;
    GP0_IRESO.bits.ih = stRect.u32Hgt - 1;
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_IRESO.u32) + u32Data * OPTM_GP_OFFSET), GP0_IRESO.u32); 

    return ;
}  

HI_VOID  OPTM_VDP_GP_SetOutReso (HI_U32 u32Data, OPTM_VDP_RECT_S  stRect)
{
    volatile U_GP0_ORESO GP0_ORESO;

    if(u32Data >= OPTM_GP_MAX)
    {
        HI_PRINT("Error,OPTM_VDP_GP_SetOutReso() Select Wrong GP Layer ID\n");
        return ;
    }


    GP0_ORESO.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ORESO.u32) + u32Data * OPTM_GP_OFFSET));
    GP0_ORESO.bits.ow = stRect.u32Wth - 1;
    GP0_ORESO.bits.oh = stRect.u32Hgt - 1;
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ORESO.u32) + u32Data * OPTM_GP_OFFSET), GP0_ORESO.u32); 

    return ;
}  

HI_VOID  OPTM_VDP_GP_SetLayerGalpha (HI_U32 u32Data, HI_U32 u32Alpha)
{
    volatile U_GP0_GALPHA GP0_GALPHA;
    
    if(u32Data >= OPTM_GP_MAX)
    {
        HI_PRINT("Error,OPTM_VDP_GP_SetLayerGalpha() Select Wrong gp Layer ID\n");
        return ;
    }

    
    GP0_GALPHA.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_GALPHA.u32) + u32Data * OPTM_GP_OFFSET));
    GP0_GALPHA.bits.galpha = u32Alpha;
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_GALPHA.u32) + u32Data * OPTM_GP_OFFSET), GP0_GALPHA.u32); 

    return ;
}

HI_VOID  OPTM_VDP_GP_SetLayerBkg(HI_U32 u32Data, OPTM_VDP_BKG_S stBkg)
{
    volatile U_GP0_BK GP0_BK;
    volatile U_GP0_ALPHA     GP0_ALPHA;

    if(u32Data >= OPTM_GP_MAX)
    {
        HI_PRINT("Error,OPTM_VDP_GP_SetLayerBkg() Select Wrong GPeo Layer ID\n");
        return ;
    }

    GP0_BK.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_BK.u32) + u32Data * OPTM_GP_OFFSET));
    GP0_BK.bits.vbk_y  = stBkg.u32BkgY;
    GP0_BK.bits.vbk_cb = stBkg.u32BkgU;
    GP0_BK.bits.vbk_cr = stBkg.u32BkgV;
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_BK.u32) + u32Data * OPTM_GP_OFFSET), GP0_BK.u32); 

    GP0_ALPHA.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ALPHA.u32) + u32Data * OPTM_GP_OFFSET));
    GP0_ALPHA.bits.vbk_alpha = stBkg.u32BkgA;
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ALPHA.u32) + u32Data * OPTM_GP_OFFSET), GP0_ALPHA.u32); 

    return ;
}

HI_VOID  OPTM_VDP_GP_SetRegUp  (HI_U32 u32Data)
{
    volatile U_GP0_UPD GP0_UPD;

    /* GP layer register update */
    if(u32Data >= OPTM_GP_MAX)
    {
        HI_PRINT("Error,OPTM_VDP_GP_SetRegUp() Select Wrong GP Layer ID\n");
        return ;
    }

    GP0_UPD.bits.regup = 0x1;
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_UPD.u32) + u32Data * OPTM_GP_OFFSET), GP0_UPD.u32); 

    return ;
}

//-------------------------------------------------------------------
// GP.CSC begin
//-------------------------------------------------------------------

HI_VOID  OPTM_VDP_GP_SetCscDcCoef   (HI_U32 u32Data, OPTM_VDP_CSC_DC_COEF_S pstCscCoef)
{
    volatile U_GP0_CSC_IDC  GP0_CSC_IDC;
    volatile U_GP0_CSC_ODC  GP0_CSC_ODC;
    volatile U_GP0_CSC_IODC GP0_CSC_IODC;
    
    if(u32Data >= OPTM_GP_MAX)
    {
        HI_PRINT("Error,OPTM_VDP_GP_SetCscDcCoef() Select Wrong GPeo Layer ID\n");
        return ;
    }
    
    GP0_CSC_IDC.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_CSC_IDC.u32) + u32Data * OPTM_GP_OFFSET));
    GP0_CSC_IDC.bits.cscidc1  = pstCscCoef.csc_in_dc1;
    GP0_CSC_IDC.bits.cscidc0  = pstCscCoef.csc_in_dc0;
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_CSC_IDC.u32) + u32Data * OPTM_GP_OFFSET), GP0_CSC_IDC.u32); 

    GP0_CSC_ODC.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_CSC_ODC.u32) + u32Data * OPTM_GP_OFFSET));
    GP0_CSC_ODC.bits.cscodc1 = pstCscCoef.csc_out_dc1;
    GP0_CSC_ODC.bits.cscodc0 = pstCscCoef.csc_out_dc0;
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_CSC_ODC.u32) + u32Data * OPTM_GP_OFFSET), GP0_CSC_ODC.u32); 

    GP0_CSC_IODC.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_CSC_IODC.u32) + u32Data * OPTM_GP_OFFSET));
    GP0_CSC_IODC.bits.cscodc2 = pstCscCoef.csc_out_dc2;
    GP0_CSC_IODC.bits.cscidc2 = pstCscCoef.csc_in_dc2;
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_CSC_IODC.u32) + u32Data * OPTM_GP_OFFSET), GP0_CSC_IODC.u32); 
    return ;
}

HI_VOID   OPTM_VDP_GP_SetCscCoef(HI_U32 u32Data, OPTM_VDP_CSC_COEF_S stCscCoef)
{   
    volatile U_GP0_CSC_P0 GP0_CSC_P0;
    volatile U_GP0_CSC_P1 GP0_CSC_P1;
    volatile U_GP0_CSC_P2 GP0_CSC_P2;
    volatile U_GP0_CSC_P3 GP0_CSC_P3;
    volatile U_GP0_CSC_P4 GP0_CSC_P4;
    if(u32Data >= OPTM_GP_MAX)
    {
        HI_PRINT("Error,OPTM_VDP_GP_SetCscCoef Select Wrong GPeo ID\n");
        return ;
    }


    GP0_CSC_P0.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_CSC_P0.u32)+u32Data*OPTM_GP_OFFSET));
    GP0_CSC_P0.bits.cscp00 = stCscCoef.csc_coef00;
    GP0_CSC_P0.bits.cscp01 = stCscCoef.csc_coef01;
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_CSC_P0.u32)+u32Data*OPTM_GP_OFFSET), GP0_CSC_P0.u32);

    GP0_CSC_P1.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_CSC_P1.u32)+u32Data*OPTM_GP_OFFSET));
    GP0_CSC_P1.bits.cscp02 = stCscCoef.csc_coef02;
    GP0_CSC_P1.bits.cscp10 = stCscCoef.csc_coef10;
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_CSC_P1.u32)+u32Data*OPTM_GP_OFFSET), GP0_CSC_P1.u32);

    GP0_CSC_P2.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_CSC_P2.u32)+u32Data*OPTM_GP_OFFSET));
    GP0_CSC_P2.bits.cscp11 = stCscCoef.csc_coef11;
    GP0_CSC_P2.bits.cscp12 = stCscCoef.csc_coef12;
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_CSC_P2.u32)+u32Data*OPTM_GP_OFFSET), GP0_CSC_P2.u32);

    GP0_CSC_P3.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_CSC_P3.u32)+u32Data*OPTM_GP_OFFSET));
    GP0_CSC_P3.bits.cscp20 = stCscCoef.csc_coef20;
    GP0_CSC_P3.bits.cscp21 = stCscCoef.csc_coef21;
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_CSC_P3.u32)+u32Data*OPTM_GP_OFFSET), GP0_CSC_P3.u32);

    GP0_CSC_P4.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_CSC_P4.u32)+u32Data*OPTM_GP_OFFSET));
    GP0_CSC_P4.bits.cscp22 = stCscCoef.csc_coef22;
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_CSC_P4.u32)+u32Data*OPTM_GP_OFFSET), GP0_CSC_P4.u32);
	
}

    
HI_VOID  OPTM_VDP_GP_SetCscEnable   (HI_U32 u32Data, HI_U32 u32bCscEn)
{
    volatile U_GP0_CSC_IDC GP0_CSC_IDC;
    
    if(u32Data >= OPTM_GP_MAX)
    {
        HI_PRINT("Error,OPTM_VDP_GP_SetCscEnable() Select Wrong GPeo Layer ID\n");
        return ;
    }

    GP0_CSC_IDC.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_CSC_IDC.u32) + u32Data * OPTM_GP_OFFSET));
    GP0_CSC_IDC.bits.csc_en = u32bCscEn;
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_CSC_IDC.u32) + u32Data * OPTM_GP_OFFSET), GP0_CSC_IDC.u32); 

    return ;
}

HI_VOID OPTM_VDP_GP_SetCscMode(HI_U32 u32Data, OPTM_VDP_CSC_MODE_E enCscMode)
                                                           
{
#if 0
    volatile OPTM_VDP_CSC_COEF_S    st_csc_coef;
    volatile OPTM_VDP_CSC_DC_COEF_S st_csc_dc_coef;

    HI_U32 u32Pre   = 1 << 10;
    HI_U32 u32DcPre = 4;//1:8bit; 4:10bit

    if(enCscMode == VDP_CSC_RGB2YUV_601)
    {
        st_csc_coef.csc_coef00     = (HI_S32)(0.299  * u32Pre);
        st_csc_coef.csc_coef01     = (HI_S32)(0.587  * u32Pre);
        st_csc_coef.csc_coef02     = (HI_S32)(0.114  * u32Pre);

        st_csc_coef.csc_coef10     = (HI_S32)(-0.172 * u32Pre);
        st_csc_coef.csc_coef11     = (HI_S32)(-0.339 * u32Pre);
        st_csc_coef.csc_coef12     = (HI_S32)(0.511  * u32Pre);

        st_csc_coef.csc_coef20     = (HI_S32)(0.511  * u32Pre);
        st_csc_coef.csc_coef21     = (HI_S32)(-0.428 * u32Pre);
        st_csc_coef.csc_coef22     = (HI_S32)(-0.083 * u32Pre);

        st_csc_dc_coef.csc_in_dc2  = (HI_S32)(0 * u32DcPre);
        st_csc_dc_coef.csc_in_dc1  = (HI_S32)(0 * u32DcPre);
        st_csc_dc_coef.csc_in_dc0  = (HI_S32)(0 * u32DcPre);

        st_csc_dc_coef.csc_out_dc2 = (HI_S32)( 16 * u32DcPre);
        st_csc_dc_coef.csc_out_dc1 = (HI_S32)(128 * u32DcPre);
        st_csc_dc_coef.csc_out_dc0 = (HI_S32)(128 * u32DcPre);
    }
    else if(enCscMode == VDP_CSC_YUV2RGB_601)
    {
        st_csc_coef.csc_coef00     = (HI_S32)(    1  * u32Pre);
        st_csc_coef.csc_coef01     = (HI_S32)(    0  * u32Pre);
        st_csc_coef.csc_coef02     = (HI_S32)(1.371  * u32Pre);

        st_csc_coef.csc_coef10     = (HI_S32)(     1 * u32Pre);
        st_csc_coef.csc_coef11     = (HI_S32)(-0.698 * u32Pre);
        st_csc_coef.csc_coef12     = (HI_S32)(-0.336 * u32Pre);

        st_csc_coef.csc_coef20     = (HI_S32)(    1  * u32Pre);
        st_csc_coef.csc_coef21     = (HI_S32)(1.732  * u32Pre);
        st_csc_coef.csc_coef22     = (HI_S32)(    0  * u32Pre);

        st_csc_dc_coef.csc_in_dc2  = (HI_S32)(-16  * u32DcPre);
        st_csc_dc_coef.csc_in_dc1  = (HI_S32)(-128 * u32DcPre);
        st_csc_dc_coef.csc_in_dc0  = (HI_S32)(-128 * u32DcPre);

        st_csc_dc_coef.csc_out_dc2 = (HI_S32)(0 * u32DcPre);
        st_csc_dc_coef.csc_out_dc1 = (HI_S32)(0 * u32DcPre);
        st_csc_dc_coef.csc_out_dc0 = (HI_S32)(0 * u32DcPre);
    }
    else if(enCscMode == VDP_CSC_RGB2YUV_709)
    {
        st_csc_coef.csc_coef00     = (HI_S32)(0.213  * u32Pre);
        st_csc_coef.csc_coef01     = (HI_S32)(0.715  * u32Pre);
        st_csc_coef.csc_coef02     = (HI_S32)(0.072  * u32Pre);

        st_csc_coef.csc_coef10     = (HI_S32)(-0.117 * u32Pre);
        st_csc_coef.csc_coef11     = (HI_S32)(-0.394 * u32Pre);
        st_csc_coef.csc_coef12     = (HI_S32)( 0.511 * u32Pre);

        st_csc_coef.csc_coef20     = (HI_S32)( 0.511 * u32Pre);
        st_csc_coef.csc_coef21     = (HI_S32)(-0.464 * u32Pre);
        st_csc_coef.csc_coef22     = (HI_S32)(-0.047 * u32Pre);

        st_csc_dc_coef.csc_in_dc2  = (HI_S32)(0 * u32DcPre);
        st_csc_dc_coef.csc_in_dc1  = (HI_S32)(0 * u32DcPre);
        st_csc_dc_coef.csc_in_dc0  = (HI_S32)(0 * u32DcPre);

        st_csc_dc_coef.csc_out_dc2 = (HI_S32)(16  * u32DcPre);
        st_csc_dc_coef.csc_out_dc1 = (HI_S32)(128 * u32DcPre);
        st_csc_dc_coef.csc_out_dc0 = (HI_S32)(128 * u32DcPre);
    }
    else if(enCscMode == VDP_CSC_YUV2RGB_709)
    {
        st_csc_coef.csc_coef00     = (HI_S32)(    1  * u32Pre);
        st_csc_coef.csc_coef01     = (HI_S32)(    0  * u32Pre);
        st_csc_coef.csc_coef02     = (HI_S32)(1.540  * u32Pre);

        st_csc_coef.csc_coef10     = (HI_S32)(     1 * u32Pre);
        st_csc_coef.csc_coef11     = (HI_S32)(-0.183 * u32Pre);
        st_csc_coef.csc_coef12     = (HI_S32)(-0.459 * u32Pre);

        st_csc_coef.csc_coef20     = (HI_S32)(    1  * u32Pre);
        st_csc_coef.csc_coef21     = (HI_S32)(1.816  * u32Pre);
        st_csc_coef.csc_coef22     = (HI_S32)(    0  * u32Pre);

        st_csc_dc_coef.csc_in_dc2  = (HI_S32)(-16  * u32DcPre);
        st_csc_dc_coef.csc_in_dc1  = (HI_S32)(-128 * u32DcPre);
        st_csc_dc_coef.csc_in_dc0  = (HI_S32)(-128 * u32DcPre);

        st_csc_dc_coef.csc_out_dc2 = (HI_S32)(0 * u32DcPre);
        st_csc_dc_coef.csc_out_dc1 = (HI_S32)(0 * u32DcPre);
        st_csc_dc_coef.csc_out_dc0 = (HI_S32)(0 * u32DcPre);
    }
    else if(enCscMode == VDP_CSC_YUV2YUV_709_601)
    {
        st_csc_coef.csc_coef00     = (HI_S32)(     1 * u32Pre);
        st_csc_coef.csc_coef01     = (HI_S32)(-0.116 * u32Pre);
        st_csc_coef.csc_coef02     = (HI_S32)( 0.208 * u32Pre);

        st_csc_coef.csc_coef10     = (HI_S32)(     0 * u32Pre);
        st_csc_coef.csc_coef11     = (HI_S32)( 1.017 * u32Pre);
        st_csc_coef.csc_coef12     = (HI_S32)( 0.114 * u32Pre);

        st_csc_coef.csc_coef20     = (HI_S32)(     0 * u32Pre);
        st_csc_coef.csc_coef21     = (HI_S32)( 0.075 * u32Pre);
        st_csc_coef.csc_coef22     = (HI_S32)( 1.025 * u32Pre);

        st_csc_dc_coef.csc_in_dc2  = (HI_S32)(-16  * u32DcPre);
        st_csc_dc_coef.csc_in_dc1  = (HI_S32)(-128 * u32DcPre);
        st_csc_dc_coef.csc_in_dc0  = (HI_S32)(-128 * u32DcPre);

        st_csc_dc_coef.csc_out_dc2 = (HI_S32)(16 * u32DcPre);
        st_csc_dc_coef.csc_out_dc1 = (HI_S32)(128 * u32DcPre);
        st_csc_dc_coef.csc_out_dc0 = (HI_S32)(128 * u32DcPre);
    }
    else if(enCscMode == VDP_CSC_YUV2YUV_601_709)
    {
        st_csc_coef.csc_coef00     = (HI_S32)(     1 * u32Pre);
        st_csc_coef.csc_coef01     = (HI_S32)(-0.116 * u32Pre);
        st_csc_coef.csc_coef02     = (HI_S32)( 0.208 * u32Pre);

        st_csc_coef.csc_coef10     = (HI_S32)(     0 * u32Pre);
        st_csc_coef.csc_coef11     = (HI_S32)( 1.017 * u32Pre);
        st_csc_coef.csc_coef12     = (HI_S32)( 0.114 * u32Pre);

        st_csc_coef.csc_coef20     = (HI_S32)(     0 * u32Pre);
        st_csc_coef.csc_coef21     = (HI_S32)( 0.075 * u32Pre);
        st_csc_coef.csc_coef22     = (HI_S32)( 1.025 * u32Pre);

        st_csc_dc_coef.csc_in_dc2  = (HI_S32)(-16  * u32DcPre);
        st_csc_dc_coef.csc_in_dc1  = (HI_S32)(-128 * u32DcPre);
        st_csc_dc_coef.csc_in_dc0  = (HI_S32)(-128 * u32DcPre);

        st_csc_dc_coef.csc_out_dc2 = (HI_S32)(16 * u32DcPre) ;
        st_csc_dc_coef.csc_out_dc1 = (HI_S32)(128 * u32DcPre);
        st_csc_dc_coef.csc_out_dc0 = (HI_S32)(128 * u32DcPre);
    }
    else
    {
        st_csc_coef.csc_coef00     = (HI_S32)1 * u32Pre;
        st_csc_coef.csc_coef01     = (HI_S32)0 * u32Pre;
        st_csc_coef.csc_coef02     = (HI_S32)0 * u32Pre;

        st_csc_coef.csc_coef10     = (HI_S32)0 * u32Pre;
        st_csc_coef.csc_coef11     = (HI_S32)1 * u32Pre;
        st_csc_coef.csc_coef12     = (HI_S32)0 * u32Pre;

        st_csc_coef.csc_coef20     = (HI_S32)0 * u32Pre;
        st_csc_coef.csc_coef21     = (HI_S32)0 * u32Pre;
        st_csc_coef.csc_coef22     = (HI_S32)1 * u32Pre;

        st_csc_dc_coef.csc_in_dc2  = (HI_S32)(-16  * u32DcPre);
        st_csc_dc_coef.csc_in_dc1  = (HI_S32)(-128 * u32DcPre);
        st_csc_dc_coef.csc_in_dc0  = (HI_S32)(-128 * u32DcPre);

        st_csc_dc_coef.csc_out_dc2 = (HI_S32)( 16 * u32DcPre);
        st_csc_dc_coef.csc_out_dc1 = (HI_S32)(128 * u32DcPre);
        st_csc_dc_coef.csc_out_dc0 = (HI_S32)(128 * u32DcPre);
    }
    
    OPTM_VDP_GP_SetCscCoef  (u32Data,st_csc_coef);
    OPTM_VDP_GP_SetCscDcCoef(u32Data,st_csc_dc_coef);
#endif
    return ;
}    

HI_VOID OPTM_VDP_GP_SetDispMode(HI_U32 u32Data, OPTM_VDP_DISP_MODE_E enDispMode)
{
#if 0
    volatile U_GP0_CTRL GP0_CTRL;
    
    if(u32Data >= OPTM_GP_MAX)
    {
        HI_PRINT("Error,OPTM_VDP_GP_SetDispMode() Select Wrong Video Layer ID\n");
        return ;
    }

    
    GP0_CTRL.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->GP0_CTRL.u32) + u32Data * OPTM_GP_OFFSET));
    GP0_CTRL.bits.disp_mode = enDispMode;
    OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->GP0_CTRL.u32) + u32Data * OPTM_VID_OFFSET), GP0_CTRL.u32); 
#endif
    return ;
}

//-------------------------------------------------------------------
// GP.ZME begin
//-------------------------------------------------------------------

HI_VOID OPTM_VDP_GP_SetZmeEnable  (HI_U32 u32Data, OPTM_VDP_ZME_MODE_E enMode, HI_U32 u32bEnable)
{
    volatile U_GP0_ZME_HSP GP0_ZME_HSP;
    volatile U_GP0_ZME_VSP GP0_ZME_VSP;
  
    if(u32Data >= OPTM_GP_MAX)
    {
        HI_PRINT("Error,OPTM_VDP_GP_SetZmeEnable() Select Wrong Video Layer ID\n");
        return ;
    }
    
    /*GP zoom enable */
    if((enMode == VDP_ZME_MODE_HOR)||(enMode == VDP_ZME_MODE_ALL))
    {
        GP0_ZME_HSP.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_HSP.u32) + u32Data * OPTM_GP_OFFSET));
        GP0_ZME_HSP.bits.hsc_en = u32bEnable;
        OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_HSP.u32) + u32Data * OPTM_GP_OFFSET), GP0_ZME_HSP.u32); 
    }
    
    if((enMode == VDP_ZME_MODE_VER)||(enMode == VDP_ZME_MODE_ALL))
    {
        GP0_ZME_VSP.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_VSP.u32) + u32Data * OPTM_GP_OFFSET));
        GP0_ZME_VSP.bits.vsc_en = u32bEnable;
        OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_VSP.u32) + u32Data * OPTM_GP_OFFSET), GP0_ZME_VSP.u32); 
    }
    
    return ;
}


HI_VOID OPTM_VDP_GP_SetZmePhase   (HI_U32 u32Data, OPTM_VDP_ZME_MODE_E enMode, HI_S32 s32Phase)
{
    volatile U_GP0_ZME_HOFFSET  GP0_ZME_HOFFSET;
    volatile U_GP0_ZME_VOFFSET  GP0_ZME_VOFFSET;
    
    if(u32Data >= OPTM_GP_MAX)
    {
        HI_PRINT("Error,OPTM_VDP_GP_SetZmePhase() Select Wrong Video Layer ID\n");
        return ;
    }

    /* GP zoom enable */
    if((enMode == VDP_ZME_MODE_HORL) || (enMode == VDP_ZME_MODE_HOR)||(enMode == VDP_ZME_MODE_ALL))
    {
        GP0_ZME_HOFFSET.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_HOFFSET.u32) + u32Data * OPTM_GP_OFFSET));
        GP0_ZME_HOFFSET.bits.hor_loffset = s32Phase;
        OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_HOFFSET.u32) + u32Data * OPTM_GP_OFFSET), GP0_ZME_HOFFSET.u32); 
    }

    if((enMode == VDP_ZME_MODE_HORC) || (enMode == VDP_ZME_MODE_HOR)||(enMode == VDP_ZME_MODE_ALL))
    {
        GP0_ZME_HOFFSET.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_HOFFSET.u32) + u32Data * OPTM_GP_OFFSET));
        GP0_ZME_HOFFSET.bits.hor_coffset = s32Phase;
        OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_HOFFSET.u32) + u32Data * OPTM_GP_OFFSET), GP0_ZME_HOFFSET.u32); 
    }

    if((enMode == VDP_ZME_MODE_VERL)||(enMode == VDP_ZME_MODE_VER)||(enMode == VDP_ZME_MODE_ALL))
    {
        GP0_ZME_VOFFSET.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_VOFFSET.u32) + u32Data * OPTM_GP_OFFSET));
        GP0_ZME_VOFFSET.bits.vbtm_offset = s32Phase;
        OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_VOFFSET.u32) + u32Data * OPTM_GP_OFFSET), GP0_ZME_VOFFSET.u32); 
    }

    if((enMode == VDP_ZME_MODE_VERC)||(enMode == VDP_ZME_MODE_VER)||(enMode == VDP_ZME_MODE_ALL))
    {
        GP0_ZME_VOFFSET.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_VOFFSET.u32) + u32Data * OPTM_GP_OFFSET));
        GP0_ZME_VOFFSET.bits.vtp_offset = s32Phase;
        OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_VOFFSET.u32) + u32Data * OPTM_GP_OFFSET), GP0_ZME_VOFFSET.u32); 
    }

    return ;
}

HI_VOID OPTM_VDP_GP_SetZmeFirEnable   (HI_U32 u32Data, OPTM_VDP_ZME_MODE_E enMode, HI_U32 u32bEnable)
{
    volatile U_GP0_ZME_HSP        GP0_ZME_HSP;
    volatile U_GP0_ZME_VSP        GP0_ZME_VSP;
    
    if(u32Data >= OPTM_GP_MAX)
    {
        HI_PRINT("Error,OPTM_VDP_GP_SetZmeFirEnable() Select Wrong Video Layer ID\n");
        return ;
    }
            
    if((enMode == VDP_ZME_MODE_ALPHA)||(enMode == VDP_ZME_MODE_HOR)||(enMode == VDP_ZME_MODE_ALL))
    {
        GP0_ZME_HSP.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_HSP.u32) + u32Data * OPTM_GP_OFFSET));
        GP0_ZME_HSP.bits.hafir_en = u32bEnable;
        OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_HSP.u32) + u32Data * OPTM_GP_OFFSET), GP0_ZME_HSP.u32);
    }
    
    if((enMode == VDP_ZME_MODE_HOR)||(enMode == VDP_ZME_MODE_ALL))
    {
        GP0_ZME_HSP.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_HSP.u32) + u32Data * OPTM_GP_OFFSET));
        GP0_ZME_HSP.bits.hfir_en = u32bEnable;
        OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_HSP.u32) + u32Data * OPTM_GP_OFFSET), GP0_ZME_HSP.u32); 
    }
     
    if((enMode == VDP_ZME_MODE_ALPHAV)||(enMode == VDP_ZME_MODE_VER)||(enMode == VDP_ZME_MODE_ALL))
    {
        GP0_ZME_VSP.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_VSP.u32) + u32Data * OPTM_GP_OFFSET));
        GP0_ZME_VSP.bits.vafir_en = u32bEnable;
        OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_VSP.u32) + u32Data * OPTM_GP_OFFSET), GP0_ZME_VSP.u32); 
    }
    
    if((enMode == VDP_ZME_MODE_VER)||(enMode == VDP_ZME_MODE_ALL))
    {
        GP0_ZME_VSP.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_VSP.u32) + u32Data * OPTM_GP_OFFSET));
        GP0_ZME_VSP.bits.vfir_en = u32bEnable;
        OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_VSP.u32) + u32Data * OPTM_GP_OFFSET), GP0_ZME_VSP.u32); 
    }
     
    return ;
}

HI_VOID OPTM_VDP_GP_SetZmeMidEnable   (HI_U32 u32Data, OPTM_VDP_ZME_MODE_E enMode, HI_U32 u32bEnable)
{
    volatile U_GP0_ZME_HSP        GP0_ZME_HSP;
    volatile U_GP0_ZME_VSP        GP0_ZME_VSP;
    
    if(u32Data >= OPTM_GP_MAX)
    {
        HI_PRINT("Error,OPTM_VDP_GP_SetZmeMidEnable() Select Wrong Video Layer ID\n");
        return ;
    }
    
    /* VHD layer zoom enable */
    if((enMode == VDP_ZME_MODE_ALPHA)||(enMode == VDP_ZME_MODE_HOR)||(enMode == VDP_ZME_MODE_ALL))
    {
        GP0_ZME_HSP.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_HSP.u32) + u32Data * OPTM_GP_OFFSET));
        GP0_ZME_HSP.bits.hamid_en = u32bEnable;
        OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_HSP.u32) + u32Data * OPTM_GP_OFFSET), GP0_ZME_HSP.u32); 
    }
    if((enMode == VDP_ZME_MODE_HORL)||(enMode == VDP_ZME_MODE_HOR)||(enMode == VDP_ZME_MODE_ALL))
    {
        GP0_ZME_HSP.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_HSP.u32) + u32Data * OPTM_GP_OFFSET));
        GP0_ZME_HSP.bits.hlmid_en = u32bEnable;
        OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_HSP.u32) + u32Data * OPTM_GP_OFFSET), GP0_ZME_HSP.u32); 
    }
    
    if((enMode == VDP_ZME_MODE_HORC)||(enMode == VDP_ZME_MODE_HOR)||(enMode == VDP_ZME_MODE_ALL))
    {
        GP0_ZME_HSP.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_HSP.u32) + u32Data * OPTM_GP_OFFSET));
        GP0_ZME_HSP.bits.hchmid_en = u32bEnable;
        OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_HSP.u32) + u32Data * OPTM_GP_OFFSET), GP0_ZME_HSP.u32); 
    }
    
    if((enMode == VDP_ZME_MODE_ALPHAV)||(enMode == VDP_ZME_MODE_VER)||(enMode == VDP_ZME_MODE_ALL))
    {
        GP0_ZME_VSP.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_VSP.u32) + u32Data * OPTM_GP_OFFSET));
        GP0_ZME_VSP.bits.vamid_en = u32bEnable;
        OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_VSP.u32) + u32Data * OPTM_GP_OFFSET), GP0_ZME_VSP.u32); 
    }
    if((enMode == VDP_ZME_MODE_VERL)||(enMode == VDP_ZME_MODE_VER)||(enMode == VDP_ZME_MODE_ALL))
    {
        GP0_ZME_VSP.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_VSP.u32) + u32Data * OPTM_GP_OFFSET));
        GP0_ZME_VSP.bits.vlmid_en = u32bEnable;
        OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_VSP.u32) + u32Data * OPTM_GP_OFFSET), GP0_ZME_VSP.u32); 
    }
    
    if((enMode == VDP_ZME_MODE_VERC)||(enMode == VDP_ZME_MODE_VER)||(enMode == VDP_ZME_MODE_ALL))
    {
        GP0_ZME_VSP.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_VSP.u32) + u32Data * OPTM_GP_OFFSET));
        GP0_ZME_VSP.bits.vchmid_en = u32bEnable;
        OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_VSP.u32) + u32Data * OPTM_GP_OFFSET), GP0_ZME_VSP.u32); 
    }

    return ;
}


HI_VOID OPTM_VDP_GP_SetZmeHorRatio  (HI_U32 u32Data, HI_U32 u32Ratio)
{
    volatile U_GP0_ZME_HSP        GP0_ZME_HSP;
    
    if(u32Data >= OPTM_GP_MAX)
    {
        HI_PRINT("Error,OPTM_VDP_GP_SetZmeHorRatio() Select Wrong Video Layer ID\n");
        return ;
    }
    
    GP0_ZME_HSP.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_HSP.u32) + u32Data * OPTM_GP_OFFSET));
    GP0_ZME_HSP.bits.hratio = u32Ratio;	
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_HSP.u32) + u32Data * OPTM_GP_OFFSET), GP0_ZME_HSP.u32); 

    return ;
}

HI_VOID OPTM_VDP_GP_SetZmeVerRatio  (HI_U32 u32Data, HI_U32 u32Ratio)
{
    volatile U_GP0_ZME_VSR        GP0_ZME_VSR;
    
    if(u32Data >= OPTM_GP_MAX)
    {
        HI_PRINT("Error,OPTM_VDP_GP_SetZmeVerRatio() Select Wrong Video Layer ID\n");
        return ;
    }
    
    GP0_ZME_VSR.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_VSR.u32) + u32Data * OPTM_GP_OFFSET));
    GP0_ZME_VSR.bits.vratio = u32Ratio;
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_VSR.u32) + u32Data * OPTM_GP_OFFSET), GP0_ZME_VSR.u32); 

    return ;
}



HI_VOID OPTM_VDP_GP_SetZmeHfirOrder        (HI_U32 u32Data, HI_U32 u32HfirOrder)
{
    volatile U_GP0_ZME_HSP        GP0_ZME_HSP;
    
    if(u32Data >= OPTM_GP_MAX)
    {
        HI_PRINT("Error,OPTM_VDP_GP_SetZmeHfirOrder() Select Wrong Video Layer ID\n");
        return ;
    }
    
    GP0_ZME_HSP.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_HSP.u32) + u32Data * OPTM_GP_OFFSET));
    GP0_ZME_HSP.bits.hfir_order = u32HfirOrder;
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_HSP.u32) + u32Data * OPTM_GP_OFFSET), GP0_ZME_HSP.u32); 

    return ;
}

HI_VOID OPTM_VDP_GP_SetZmeCoefAddr  (HI_U32 u32Data, HI_U32 u32Mode, HI_U32 u32Addr)
{
    volatile U_GP0_HCOEFAD    GP0_HCOEFAD;
    volatile U_GP0_VCOEFAD    GP0_VCOEFAD;
    
    if(u32Data >= OPTM_GP_MAX)
    {
        HI_PRINT("Error,OPTM_VDP_GP_SetZmeCoefAddr() Select Wrong Video Layer ID\n");
        return ;
    }
    
    if(u32Mode == VDP_GP_PARA_ZME_HOR)
    {
        GP0_HCOEFAD.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_HCOEFAD.u32) + u32Data * OPTM_GP_OFFSET));
        GP0_HCOEFAD.bits.coef_addr = u32Addr;
        OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_HCOEFAD.u32) + u32Data * OPTM_GP_OFFSET), GP0_HCOEFAD.u32); 
    }
    else if(u32Mode == VDP_GP_PARA_ZME_VER)
    {
        GP0_VCOEFAD.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_VCOEFAD.u32) + u32Data * OPTM_GP_OFFSET));
        GP0_VCOEFAD.bits.coef_addr = u32Addr;
        OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_VCOEFAD.u32) + u32Data * OPTM_GP_OFFSET), GP0_VCOEFAD.u32); 
    }
    else
    {
        HI_PRINT("Error,OPTM_VDP_GP_SetZmeCoefAddr() Select a Wrong Mode!\n");
    }

    return ;
}


HI_VOID OPTM_VDP_GP_SetParaRd   (HI_U32 u32Data, OPTM_VDP_GP_PARA_E enMode)
{
    volatile U_GP0_PARARD GP0_PARARD;

    GP0_PARARD.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_PARARD.u32) + u32Data * OPTM_GP_OFFSET));

    if(u32Data >= OPTM_GP_MAX)
    {
        HI_PRINT("Error,OPTM_VDP_GP_SetParaRd() Select Wrong Video Layer ID\n");
        return ;
    }
    if(enMode == VDP_GP_PARA_ZME_HORL)
    {
        GP0_PARARD.bits.gp0_hlcoef_rd = 0x1;
    }
    
    if(enMode == VDP_GP_PARA_ZME_HORC)
    {
        GP0_PARARD.bits.gp0_hccoef_rd = 0x1;
    }
    
    if(enMode == VDP_GP_PARA_ZME_VERL)
    {
        GP0_PARARD.bits.gp0_vlcoef_rd = 0x1;
    }
    
    if(enMode == VDP_GP_PARA_ZME_VERC)
    {
        GP0_PARARD.bits.gp0_vccoef_rd = 0x1;
    }
    
    if(enMode == VDP_GP_GTI_PARA_ZME_HORL)
    {
        GP0_PARARD.bits.gp0_gti_hlcoef_rd = 0x1;
    }
    
    if(enMode == VDP_GP_GTI_PARA_ZME_HORC)
    {
        GP0_PARARD.bits.gp0_gti_hccoef_rd = 0x1;
    }
    
    if(enMode == VDP_GP_GTI_PARA_ZME_VERL)
    {
        GP0_PARARD.bits.gp0_gti_vlcoef_rd = 0x1;
    }
    
    if(enMode == VDP_GP_PARA_ZME_VERC)
    {
        GP0_PARARD.bits.gp0_gti_vccoef_rd = 0x1;
    }
    
    else
    {
        HI_PRINT("error,VDP_VID_SetParaUpd() select wrong mode!\n");
    }
    
    OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_PARARD.u32) + u32Data * OPTM_GP_OFFSET), GP0_PARARD.u32); 
    return ;
}

//-------------------------------------------------------------------
// GP.ZME.GTI(LTI/CTI) begin
//-------------------------------------------------------------------

HI_VOID  OPTM_VDP_GP_SetTiEnable(HI_U32 u32Data, HI_U32 u32Md,HI_U32 u32Data1)
{
    volatile U_GP0_ZME_LTICTRL GP0_ZME_LTICTRL;
    volatile U_GP0_ZME_CTICTRL GP0_ZME_CTICTRL;

    if(u32Data >= OPTM_GP_MAX)
    {
        HI_PRINT("Error,OPTM_VDP_GP_SetTiEnable() Select Wrong Graph Layer ID\n");
        return ;
    }
    if((u32Md == VDP_TI_MODE_LUM)||(u32Md == VDP_TI_MODE_ALL))
    {
        GP0_ZME_LTICTRL.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_LTICTRL.u32)+ u32Data * OPTM_GP_OFFSET));
        GP0_ZME_LTICTRL.bits.lti_en = u32Data1;
        OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_LTICTRL.u32)+ u32Data * OPTM_GP_OFFSET), GP0_ZME_LTICTRL.u32);

    }
    if((u32Md == VDP_TI_MODE_CHM)||(u32Md == VDP_TI_MODE_ALL))
    {
        GP0_ZME_CTICTRL.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_CTICTRL.u32)+ u32Data * OPTM_GP_OFFSET));
        GP0_ZME_CTICTRL.bits.cti_en = u32Data1;
        OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_CTICTRL.u32)+ u32Data * OPTM_GP_OFFSET), GP0_ZME_CTICTRL.u32);
    }

}

HI_VOID  OPTM_VDP_GP_SetTiGainRatio(HI_U32 u32Data, HI_U32 u32Md, HI_S32 s32Data)

{
    volatile U_GP0_ZME_LTICTRL GP0_ZME_LTICTRL;
    volatile U_GP0_ZME_CTICTRL GP0_ZME_CTICTRL;

    if(u32Data >= OPTM_GP_MAX)
    {
        HI_PRINT("Error,VDP_SetTiGainRatio() Select Wrong Graph Layer ID\n");
        return ;
    }

    if((u32Md == VDP_TI_MODE_LUM)||(u32Md == VDP_TI_MODE_ALL))
    {
        GP0_ZME_LTICTRL.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_LTICTRL.u32)+ u32Data * OPTM_GP_OFFSET));
        GP0_ZME_LTICTRL.bits.lgain_ratio = s32Data;
        OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_LTICTRL.u32)+ u32Data * OPTM_GP_OFFSET), GP0_ZME_LTICTRL.u32); 
    }

    if((u32Md == VDP_TI_MODE_CHM)||(u32Md == VDP_TI_MODE_ALL))
    {
        GP0_ZME_CTICTRL.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_CTICTRL.u32)+ u32Data * OPTM_GP_OFFSET));
        GP0_ZME_CTICTRL.bits.cgain_ratio = s32Data;
        OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_CTICTRL.u32)+ u32Data * OPTM_GP_OFFSET), GP0_ZME_CTICTRL.u32); 
    }

}

HI_VOID  OPTM_VDP_GP_SetTiMixRatio(HI_U32 u32Data, HI_U32 u32Md, HI_U32 u32mixing_ratio)

{
    volatile U_GP0_ZME_LTICTRL GP0_ZME_LTICTRL;
    volatile U_GP0_ZME_CTICTRL GP0_ZME_CTICTRL;
    if(u32Data >= OPTM_GP_MAX)
    {
        HI_PRINT("Error,OPTM_VDP_GP_SetTiMixRatio() Select Wrong Graph Layer ID\n");
        return ;
    }


    if((u32Md == VDP_TI_MODE_LUM)||(u32Md == VDP_TI_MODE_ALL))
    {
        GP0_ZME_LTICTRL.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_LTICTRL.u32)+ u32Data * OPTM_GP_OFFSET));
        GP0_ZME_LTICTRL.bits.lmixing_ratio = u32mixing_ratio;
        OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_LTICTRL.u32)+ u32Data * OPTM_GP_OFFSET), GP0_ZME_LTICTRL.u32); 
    }

    if((u32Md == VDP_TI_MODE_CHM)||(u32Md == VDP_TI_MODE_ALL))
    {
        GP0_ZME_CTICTRL.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_CTICTRL.u32)+ u32Data * OPTM_GP_OFFSET));
        GP0_ZME_CTICTRL.bits.cmixing_ratio = u32mixing_ratio;
        OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_CTICTRL.u32)+ u32Data * OPTM_GP_OFFSET), GP0_ZME_CTICTRL.u32); 
    }

}

HI_VOID  OPTM_VDP_GP_SetTiHfThd(HI_U32 u32Data, HI_U32 u32Md, HI_U32 * u32TiHfThd)
{
    volatile U_GP0_ZME_LHFREQTHD GP0_ZME_LHFREQTHD;
    if(u32Data >= OPTM_GP_MAX)
    {
        HI_PRINT("Error,OPTM_VDP_GP_SetTiHfThd() Select Wrong Graph Layer ID\n");
        return ;
    }

    if((u32Md == VDP_TI_MODE_LUM)||(u32Md == VDP_TI_MODE_ALL))
    {
        GP0_ZME_LHFREQTHD.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_LHFREQTHD.u32)+ u32Data * OPTM_GP_OFFSET));
        GP0_ZME_LHFREQTHD.bits.lhfreq_thd0 = u32TiHfThd[0];
        GP0_ZME_LHFREQTHD.bits.lhfreq_thd1 = u32TiHfThd[1];
        OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_LHFREQTHD.u32)+ u32Data * OPTM_GP_OFFSET), GP0_ZME_LHFREQTHD.u32); 
    }


}

HI_VOID  OPTM_VDP_GP_SetTiHpCoef(HI_U32 u32Data, HI_U32 u32Md, HI_S32 * s32Data)
{

    volatile U_GP0_ZME_LTICTRL GP0_ZME_LTICTRL;
    volatile U_GP0_ZME_CTICTRL GP0_ZME_CTICTRL;
    volatile U_GP0_ZME_LHPASSCOEF GP0_ZME_LHPASSCOEF;
    volatile U_GP0_ZME_CHPASSCOEF GP0_ZME_CHPASSCOEF;
    if(u32Data >= OPTM_GP_MAX)
    {
        HI_PRINT("Error,OPTM_VDP_GP_SetTiHpCoef() Select Wrong Graph Layer ID\n");
        return ;
    }

    if((u32Md == VDP_TI_MODE_LUM)||(u32Md == VDP_TI_MODE_ALL))
    {
        GP0_ZME_LTICTRL.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_LTICTRL.u32)+ u32Data * OPTM_GP_OFFSET));
        GP0_ZME_LHPASSCOEF.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_LHPASSCOEF.u32)+ u32Data * OPTM_GP_OFFSET));
        GP0_ZME_LHPASSCOEF.bits.lhpass_coef0 = s32Data[0];
        GP0_ZME_LHPASSCOEF.bits.lhpass_coef1 = s32Data[1];
        GP0_ZME_LHPASSCOEF.bits.lhpass_coef2 = s32Data[2];
        GP0_ZME_LHPASSCOEF.bits.lhpass_coef3 = s32Data[3];
        GP0_ZME_LTICTRL.bits.lhpass_coef4    = s32Data[4];
        OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_LTICTRL.u32)+ u32Data * OPTM_GP_OFFSET), GP0_ZME_LTICTRL.u32); 
        OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_LHPASSCOEF.u32)+ u32Data * OPTM_GP_OFFSET), GP0_ZME_LHPASSCOEF.u32); 
    }

    if((u32Md == VDP_TI_MODE_CHM)||(u32Md == VDP_TI_MODE_ALL))
    {
        GP0_ZME_CTICTRL.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_CTICTRL.u32)+ u32Data * OPTM_GP_OFFSET));
        GP0_ZME_CHPASSCOEF.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_CHPASSCOEF.u32)+ u32Data * OPTM_GP_OFFSET));
        GP0_ZME_CHPASSCOEF.bits.chpass_coef0 = s32Data[0];
        GP0_ZME_CHPASSCOEF.bits.chpass_coef1 = s32Data[1];
        GP0_ZME_CHPASSCOEF.bits.chpass_coef2 = s32Data[2];
        OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_CTICTRL.u32)+ u32Data * OPTM_GP_OFFSET), GP0_ZME_CTICTRL.u32); 
        OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_CHPASSCOEF.u32)+ u32Data * OPTM_GP_OFFSET), GP0_ZME_CHPASSCOEF.u32); 
    }
}

HI_VOID  OPTM_VDP_GP_SetTiCoringThd(HI_U32 u32Data, HI_U32 u32Md, HI_U32 u32thd)
{
    volatile U_GP0_ZME_LTITHD GP0_ZME_LTITHD;
    volatile U_GP0_ZME_CTITHD GP0_ZME_CTITHD;
    if(u32Data >= OPTM_GP_MAX)
    {
        HI_PRINT("Error,OPTM_VDP_GP_SetTiCoringThd() Select Wrong Graph Layer ID\n");
        return ;
    }

    if((u32Md == VDP_TI_MODE_LUM)||(u32Md == VDP_TI_MODE_ALL))
    {
        GP0_ZME_LTITHD.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_LTITHD.u32)+ u32Data * OPTM_GP_OFFSET));
        GP0_ZME_LTITHD.bits.lcoring_thd = u32thd;
        OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_LTITHD.u32)+ u32Data * OPTM_GP_OFFSET), GP0_ZME_LTITHD.u32); 
    }

    if((u32Md == VDP_TI_MODE_CHM)||(u32Md == VDP_TI_MODE_ALL))
    {
        GP0_ZME_CTITHD.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_CTITHD.u32)+ u32Data * OPTM_GP_OFFSET));
        GP0_ZME_CTITHD.bits.ccoring_thd = u32thd;
        OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_CTITHD.u32)+ u32Data * OPTM_GP_OFFSET), GP0_ZME_CTITHD.u32); 
    }

}

//***********************have problem !!!*****************************************************
HI_VOID  OPTM_VDP_GP_SetTiSwingThd(HI_U32 u32Data, HI_U32 u32Md, HI_U32 u32thd, HI_U32 u32thd1)//have problem
{
    volatile U_GP0_ZME_LTITHD GP0_ZME_LTITHD;
    volatile U_GP0_ZME_CTITHD GP0_ZME_CTITHD;
    if(u32Data >= OPTM_GP_MAX)
    {
        HI_PRINT("Error,Vou_SetTiSwingThd() Select Wrong Graph Layer ID\n");
        return ;
    }

    if((u32Md == VDP_TI_MODE_LUM)||(u32Md == VDP_TI_MODE_ALL))
    {
        GP0_ZME_LTITHD.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_LTITHD.u32)+ u32Data * OPTM_GP_OFFSET));
        GP0_ZME_LTITHD.bits.lover_swing  = u32thd;
        GP0_ZME_LTITHD.bits.lunder_swing = u32thd1;
        OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_LTITHD.u32)+ u32Data * OPTM_GP_OFFSET), GP0_ZME_LTITHD.u32); 
    }

    if((u32Md == VDP_TI_MODE_CHM)||(u32Md == VDP_TI_MODE_ALL))
    {
        GP0_ZME_CTITHD.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_CTITHD.u32)+ u32Data * OPTM_GP_OFFSET));
        GP0_ZME_CTITHD.bits.cover_swing  = u32thd;
        GP0_ZME_CTITHD.bits.cunder_swing = u32thd1;
        OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_CTITHD.u32)+ u32Data * OPTM_GP_OFFSET), GP0_ZME_CTITHD.u32); 
    }

}

//***********************************************************************************************
HI_VOID  OPTM_VDP_GP_SetTiGainCoef(HI_U32 u32Data, HI_U32 u32Md, HI_U32 * u32coef)
{
    volatile U_GP0_ZME_LGAINCOEF GP0_ZME_LGAINCOEF;
    if(u32Data >= OPTM_GP_MAX)
    {
        HI_PRINT("Error,OPTM_VDP_GP_SetTiGainCoef() Select Wrong Graph Layer ID\n");
        return ;
    }

    if((u32Md == VDP_TI_MODE_LUM)||(u32Md == VDP_TI_MODE_ALL))
    {
        GP0_ZME_LGAINCOEF.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_LGAINCOEF.u32)+ u32Data * OPTM_GP_OFFSET));
        GP0_ZME_LGAINCOEF.bits.lgain_coef0 = u32coef[0];
        GP0_ZME_LGAINCOEF.bits.lgain_coef1 = u32coef[1];
        GP0_ZME_LGAINCOEF.bits.lgain_coef2 = u32coef[2];
        OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_LGAINCOEF.u32)+ u32Data * OPTM_GP_OFFSET), GP0_ZME_LGAINCOEF.u32); 
    }
}

HI_VOID  OPTM_VDP_GP_SetTiDefThd(HI_U32 u32Data, HI_U32 u32Md)
{
    HI_U32 u32HfThd[2]    = {20,512};
    HI_U32 u32GainCoef[3] = {152,118,108};
//    HI_S32  s32LtiHpCoef[5] = {-19,-8,-2,-1,-1};
//    HI_S32  s32CtiHpCoef[5] = {-21,-21,-11,0,0};

    HI_S32  s32CtiHpCoef[5] = {-21,-21,-11,0,0};
    HI_S32  s32LtiHpCoef[5] = {-21,-21,-11,0,0};

    if(u32Data >= OPTM_GP_MAX)
    {
        HI_PRINT("Error,OPTM_VDP_GP_SetTiDefThd() Select Wrong Graph Layer ID\n");
        return ;
    }

    if(u32Md == VDP_TI_MODE_LUM)
    {
        OPTM_VDP_GP_SetTiGainRatio(u32Data, u32Md, 255);
        OPTM_VDP_GP_SetTiMixRatio (u32Data, u32Md, 127);
        OPTM_VDP_GP_SetTiCoringThd(u32Data, u32Md, 0);
        OPTM_VDP_GP_SetTiSwingThd (u32Data, u32Md, 6, 6);
        OPTM_VDP_GP_SetTiHfThd    (u32Data, u32Md, u32HfThd);
        OPTM_VDP_GP_SetTiGainCoef (u32Data, u32Md, u32GainCoef);
        OPTM_VDP_GP_SetTiHpCoef   (u32Data, u32Md, s32LtiHpCoef);
    }

    if(u32Md == VDP_TI_MODE_CHM)
    {
        OPTM_VDP_GP_SetTiGainRatio(u32Data, u32Md, 255);
        OPTM_VDP_GP_SetTiMixRatio (u32Data, u32Md, 127);
        OPTM_VDP_GP_SetTiCoringThd(u32Data, u32Md, 4);
        OPTM_VDP_GP_SetTiSwingThd (u32Data, u32Md, 6, 6);
        OPTM_VDP_GP_SetTiHfThd    (u32Data, u32Md, u32HfThd);
        OPTM_VDP_GP_SetTiGainCoef (u32Data, u32Md, u32GainCoef);
        OPTM_VDP_GP_SetTiHpCoef   (u32Data, u32Md, s32CtiHpCoef);
    }
}



#if 0
///////////////////////GO ZME  BEGIN///
/* graphic0 layer zoom enable */
HI_VOID  VDP_GFX_SetZmeEnable(HI_U32 u32Data,OPTM_VDP_ZME_MODE_E enMode,HI_U32 bEnable)
{
    U_G0_HSP G0_HSP;
    U_G0_VSP G0_VSP;

    if(u32Data >= OPTM_GFX_MAX)
    {
        HI_PRINT("Error,VDP_GFX_SetZmeEnable() Select Wrong Graph Layer ID\n");
        return ;
    }

    if((enMode == VDP_ZME_MODE_HOR)||(enMode == VDP_ZME_MODE_ALL))
    {


        G0_HSP.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->G0_HSP.u32)+ u32Data * OPTM_GFX_OFFSET));
        G0_HSP.bits.hsc_en = bEnable;
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->G0_HSP.u32)+ u32Data * OPTM_GFX_OFFSET), G0_HSP.u32); 
    }

    if((enMode == VDP_ZME_MODE_VER)||(enMode == VDP_ZME_MODE_ALL))
    {
        G0_VSP.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->G0_VSP.u32)+ u32Data * OPTM_GFX_OFFSET));
        G0_VSP.bits.vsc_en = bEnable;
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->G0_VSP.u32)+ u32Data * OPTM_GFX_OFFSET), G0_VSP.u32); 
    }
}

HI_VOID VDP_GFX_SetZmeFirEnable(HI_U32 u32Data, OPTM_VDP_ZME_MODE_E enMode, HI_U32 bEnable)
{
    
    U_G0_VSP G0_VSP;
    U_G0_HSP G0_HSP;

    
    if(u32Data >= OPTM_GFX_MAX)
    {
        HI_PRINT("Error,VDP_SetZmeFirEnable() Select Wrong Graph Layer ID\n");
        return ;
    }

    /* g0 layer zoom enable */
    if((enMode == VDP_ZME_MODE_HORL)||(enMode == VDP_ZME_MODE_HOR)||(enMode == VDP_ZME_MODE_ALL))
    {
        G0_HSP.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->G0_HSP.u32)+ u32Data * OPTM_GFX_OFFSET));
        G0_HSP.bits.hfir_en = bEnable;
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->G0_HSP.u32)+ u32Data * OPTM_GFX_OFFSET), G0_HSP.u32); 
    }

    if((enMode == VDP_ZME_MODE_ALPHA)||(enMode == VDP_ZME_MODE_HOR)||(enMode == VDP_ZME_MODE_ALL))
    {
        G0_HSP.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->G0_HSP.u32)+ u32Data * OPTM_GFX_OFFSET));
        G0_HSP.bits.hafir_en = bEnable;
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->G0_HSP.u32)+ u32Data * OPTM_GFX_OFFSET), G0_HSP.u32); 
    }

    if((enMode == VDP_ZME_MODE_VERL)||(enMode == VDP_ZME_MODE_VER)||(enMode == VDP_ZME_MODE_ALL))
    {
        G0_VSP.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->G0_VSP.u32)+ u32Data * OPTM_GFX_OFFSET));
        G0_VSP.bits.vfir_en = bEnable;
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->G0_VSP.u32)+ u32Data * OPTM_GFX_OFFSET), G0_VSP.u32); 
    }

    if((enMode == VDP_ZME_MODE_ALPHAV)||(enMode == VDP_ZME_MODE_VER)||(enMode == VDP_ZME_MODE_ALL))
    {
        G0_VSP.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->G0_VSP.u32)+ u32Data * OPTM_GFX_OFFSET));
        G0_VSP.bits.vafir_en = bEnable;
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->G0_VSP.u32)+ u32Data * OPTM_GFX_OFFSET), G0_VSP.u32); 
    }

}

/* Vou set Median filter enable */
HI_VOID  VDP_GFX_SetZmeMidEnable(HI_U32 u32Data,OPTM_VDP_ZME_MODE_E enMode, HI_U32 bEnable)
{
    U_G0_HSP G0_HSP;
    U_G0_VSP G0_VSP;

    if(u32Data >= OPTM_GFX_MAX)
    {
        HI_PRINT("Error,VDP_GFX_SetZmeMidEnable() Select Wrong Graph Layer ID\n");
        return ;
    }
    /* G0 layer zoom enable */
    if((enMode == VDP_ZME_MODE_HORL) || (enMode == VDP_ZME_MODE_HOR) || (enMode == VDP_ZME_MODE_ALL))
    {
        G0_HSP.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->G0_HSP.u32)+ u32Data * OPTM_GFX_OFFSET));
        G0_HSP.bits.hlmid_en = bEnable;
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->G0_HSP.u32)+ u32Data * OPTM_GFX_OFFSET), G0_HSP.u32);  
    }
    if((enMode == VDP_ZME_MODE_HORC) || (enMode == VDP_ZME_MODE_HOR) || (enMode == VDP_ZME_MODE_ALL))
    {
        G0_HSP.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->G0_HSP.u32)+ u32Data * OPTM_GFX_OFFSET));
        G0_HSP.bits.hchmid_en = bEnable;
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->G0_HSP.u32)+ u32Data * OPTM_GFX_OFFSET), G0_HSP.u32);  
    }
    if((enMode == VDP_ZME_MODE_ALPHA) || (enMode == VDP_ZME_MODE_HOR) || (enMode == VDP_ZME_MODE_ALL))
    {
        G0_HSP.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->G0_HSP.u32)+ u32Data * OPTM_GFX_OFFSET));
        G0_HSP.bits.hamid_en = bEnable;
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->G0_HSP.u32)+ u32Data * OPTM_GFX_OFFSET), G0_HSP.u32);  
    }
    if((enMode == VDP_ZME_MODE_VERL) || (enMode == VDP_ZME_MODE_VER) || (enMode == VDP_ZME_MODE_ALL))
    {
        G0_VSP.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->G0_VSP.u32)+ u32Data * OPTM_GFX_OFFSET));
        G0_VSP.bits.vlmid_en = bEnable;
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->G0_VSP.u32)+ u32Data * OPTM_GFX_OFFSET), G0_VSP.u32);  
    }
    if((enMode == VDP_ZME_MODE_VERC) || (enMode == VDP_ZME_MODE_VER) || (enMode == VDP_ZME_MODE_ALL))
    {
        G0_VSP.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->G0_VSP.u32)+ u32Data * OPTM_GFX_OFFSET));
        G0_VSP.bits.vchmid_en = bEnable;
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->G0_VSP.u32)+ u32Data * OPTM_GFX_OFFSET), G0_VSP.u32);
    }
    if((enMode == VDP_ZME_MODE_ALPHAV) || (enMode == VDP_ZME_MODE_VER) || (enMode == VDP_ZME_MODE_ALL))
    {
        G0_VSP.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->G0_VSP.u32)+ u32Data * OPTM_GFX_OFFSET));
        G0_VSP.bits.vamid_en = bEnable;
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->G0_VSP.u32)+ u32Data * OPTM_GFX_OFFSET), G0_VSP.u32);
    }



}

HI_VOID VDP_GFX_SetZmeHfirOrder(HI_U32 u32Data, HI_U32 uHfirOrder)
{
    U_G0_HSP G0_HSP;
    if(u32Data >= OPTM_GFX_MAX)
    {
        HI_PRINT("Error,VDP_GFX_SetZmeHfirOrder() Select Wrong Graph Layer ID\n");
        return ;
    }

    G0_HSP.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->G0_HSP.u32)+ u32Data * OPTM_GFX_OFFSET));
    G0_HSP.bits.hfir_order = uHfirOrder;
    OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->G0_HSP.u32)+ u32Data * OPTM_GFX_OFFSET), G0_HSP.u32);


}
HI_VOID VDP_GFX_SetZmeVerTap(HI_U32 u32Data, OPTM_VDP_ZME_MODE_E enMode, HI_U32 u32VerTap)
{
    U_G0_VSP G0_VSP;
    if(u32Data >= OPTM_GFX_MAX)
    {
        HI_PRINT("Error,VDP_GFX_SetZmeVerTap() Select Wrong Graph Layer ID\n");
        return ;
    }

    G0_VSP.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->G0_VSP.u32)+ u32Data * OPTM_GFX_OFFSET));
    G0_VSP.bits.vsc_luma_tap = u32VerTap;
    OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->G0_VSP.u32)+ u32Data * OPTM_GFX_OFFSET), G0_VSP.u32);
}

/* Vou set zoom inital phase */
HI_VOID  VDP_GFX_SetZmePhase(HI_U32 u32Data, OPTM_VDP_ZME_MODE_E enMode,HI_S32 s32Phase)
{
    U_G0_HOFFSET G0_HOFFSET;
    U_G0_VOFFSET G0_VOFFSET;
    if(u32Data >= OPTM_GFX_MAX)
    {
        HI_PRINT("Error,VDP_GFX_SetZmePhase() Select Wrong Graph Layer ID\n");
        return ;
    }

    if((enMode == VDP_ZME_MODE_HORL)||(enMode == VDP_ZME_MODE_HOR)||(enMode == VDP_ZME_MODE_ALL))
    {
        G0_HOFFSET.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->G0_HOFFSET.u32)+ u32Data * OPTM_GFX_OFFSET));
        G0_HOFFSET.bits.hor_loffset = (s32Phase);
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->G0_HOFFSET.u32)+ u32Data * OPTM_GFX_OFFSET), G0_HOFFSET.u32); 
    }
    if((enMode == VDP_ZME_MODE_HORC)||(enMode == VDP_ZME_MODE_HOR)||(enMode == VDP_ZME_MODE_ALL))
    {
        G0_HOFFSET.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->G0_HOFFSET.u32)+ u32Data * OPTM_GFX_OFFSET));
        G0_HOFFSET.bits.hor_coffset = (s32Phase);
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->G0_HOFFSET.u32)+ u32Data * OPTM_GFX_OFFSET), G0_HOFFSET.u32); 
    }

    if((enMode == VDP_ZME_MODE_VERT)||(enMode == VDP_ZME_MODE_VER)||(enMode == VDP_ZME_MODE_ALL))
    {
        G0_VOFFSET.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->G0_HOFFSET.u32)+ u32Data * OPTM_GFX_OFFSET));
        G0_VOFFSET.bits.vtp_offset = (s32Phase);
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->G0_VOFFSET.u32)+ u32Data * OPTM_GFX_OFFSET), G0_VOFFSET.u32); 
    }
    if((enMode == VDP_ZME_MODE_VERB)||(enMode == VDP_ZME_MODE_VER)||(enMode == VDP_ZME_MODE_ALL))
    {
        G0_VOFFSET.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->G0_VOFFSET.u32)+ u32Data * OPTM_GFX_OFFSET));
        G0_VOFFSET.bits.vbtm_offset = (s32Phase);
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->G0_VOFFSET.u32)+ u32Data * OPTM_GFX_OFFSET), G0_VOFFSET.u32); 
    }


}
//GFX END
#endif
//-------------------------------------------------------------------
//WBC_DHD0_BEGIN
//-------------------------------------------------------------------
HI_VOID OPTM_VDP_WBC_SetAutoSt(OPTM_VDP_LAYER_WBC_E enLayer, HI_U32 bEnable)
{
    volatile U_WBC_GP0_CTRL  WBC_GP0_CTRL;
    volatile U_WBC_DHD0_CTRL  WBC_DHD0_CTRL;

    if(enLayer == OPTM_VDP_LAYER_WBC_GP0 )
    {
        WBC_GP0_CTRL.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_GP0_CTRL.u32)));
        WBC_GP0_CTRL.bits.auto_stop_en=  bEnable;
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_GP0_CTRL.u32)), WBC_GP0_CTRL.u32);
    }
    if(enLayer == OPTM_VDP_LAYER_WBC_HD0)
    {
        WBC_DHD0_CTRL.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_DHD0_CTRL.u32)));
        WBC_DHD0_CTRL.bits.auto_stop_en=  bEnable;
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_DHD0_CTRL.u32)), WBC_DHD0_CTRL.u32);
    }


}


HI_VOID OPTM_VDP_WBC_SetThreeMd(OPTM_VDP_LAYER_WBC_E enLayer, HI_U32 bMode)
{
    volatile    U_WBC_GP0_CTRL  WBC_GP0_CTRL;
//    volatile    U_WBC_DHD0_CTRL    WBC_DHD0_CTRL;

    if(enLayer == OPTM_VDP_LAYER_WBC_GP0)
    {
        WBC_GP0_CTRL.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_GP0_CTRL.u32)));
        WBC_GP0_CTRL.bits.three_d_mode =  bMode;
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_GP0_CTRL.u32)), WBC_GP0_CTRL.u32);
    }
#if 0
    else if(enLayer == OPTM_VDP_LAYER_WBC_HD0)
    {
        WBC_DHD0_CTRL.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_DHD0_CTRL.u32)));
        WBC_DHD0_CTRL.bits.three_d_mode =  bMode;
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_DHD0_CTRL.u32)), WBC_DHD0_CTRL.u32);
    }
#endif
}

HI_VOID OPTM_VDP_WBC_SetEnable( OPTM_VDP_LAYER_WBC_E enLayer, HI_U32 bEnable)
{
//    volatile U_WBC_DHD0_CTRL WBC_DHD0_CTRL;
    volatile U_WBC_GP0_CTRL  WBC_GP0_CTRL;
    volatile U_WBC_G0_CTRL   WBC_G0_CTRL;

#if 0
    if( enLayer == OPTM_VDP_LAYER_WBC_HD0)
    {
        WBC_DHD0_CTRL.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_DHD0_CTRL.u32)));
        WBC_DHD0_CTRL.bits.wbc_en = bEnable;
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_DHD0_CTRL.u32)), WBC_DHD0_CTRL.u32);
    }
    else if(enLayer == OPTM_VDP_LAYER_WBC_GP0)
#else
    if(enLayer == OPTM_VDP_LAYER_WBC_GP0)
#endif
    {
        WBC_GP0_CTRL.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_GP0_CTRL.u32)));
        WBC_GP0_CTRL.bits.wbc_en = bEnable;
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_GP0_CTRL.u32)), WBC_GP0_CTRL.u32);


    }
    else if(enLayer == OPTM_VDP_LAYER_WBC_2)
    {
        WBC_G0_CTRL.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_G0_CTRL.u32)));
        WBC_G0_CTRL.bits.wbc_en = bEnable;
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_G0_CTRL.u32)), WBC_G0_CTRL.u32);
    }
    else if(enLayer == OPTM_VDP_LAYER_WBC_3)
    {
        WBC_G0_CTRL.u32 = OPTM_VDP_RegRead(((HI_U32)(&(pOptmVdpReg->WBC_G0_CTRL.u32)))+0x400);
        WBC_G0_CTRL.bits.wbc_en = bEnable;
        OPTM_VDP_RegWrite(((HI_U32)(&(pOptmVdpReg->WBC_G0_CTRL.u32)))+0x400, WBC_G0_CTRL.u32);
    }
}

HI_VOID OPTM_VDP_WBC_GetEnable( OPTM_VDP_LAYER_WBC_E enLayer, HI_U32 *pU32Enable)
{
    volatile U_WBC_DHD0_CTRL WBC_DHD0_CTRL;
    volatile U_WBC_GP0_CTRL  WBC_GP0_CTRL;


    if( enLayer == OPTM_VDP_LAYER_WBC_HD0)
    {
        WBC_DHD0_CTRL.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_DHD0_CTRL.u32)));
		*pU32Enable = WBC_DHD0_CTRL.bits.wbc_en;
    }
    else if(enLayer == OPTM_VDP_LAYER_WBC_GP0)
    {
        WBC_GP0_CTRL.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_GP0_CTRL.u32)));
		*pU32Enable = WBC_GP0_CTRL.bits.wbc_en;
    }


}


HI_VOID OPTM_VDP_WBC_SetOutIntf (OPTM_VDP_LAYER_WBC_E enLayer, OPTM_VDP_DATA_RMODE_E u32RdMode)
{
//    volatile U_WBC_DHD0_CTRL WBC_DHD0_CTRL;
    volatile U_WBC_GP0_CTRL  WBC_GP0_CTRL;
#if 0
    if( enLayer == OPTM_VDP_LAYER_WBC_HD0) 
    {
        WBC_DHD0_CTRL.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_DHD0_CTRL.u32)));
        WBC_DHD0_CTRL.bits.mode_out = u32RdMode;            
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_DHD0_CTRL.u32)), WBC_DHD0_CTRL.u32);
    }
    else if( enLayer == OPTM_VDP_LAYER_WBC_GP0) 
#else
    if( enLayer == OPTM_VDP_LAYER_WBC_GP0)
#endif
    {
        WBC_GP0_CTRL.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_GP0_CTRL.u32)));
        WBC_GP0_CTRL.bits.mode_out = u32RdMode;            
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_GP0_CTRL.u32)), WBC_GP0_CTRL.u32);
    }
    else
    {
        HI_PRINT("Error! OPTM_VDP_WBC_SetOutIntf enRdMode error!\n");
    }
}

HI_VOID  OPTM_VDP_WBC_SetRegUp (OPTM_VDP_LAYER_WBC_E enLayer)
{
//    volatile U_WBC_DHD0_UPD WBC_DHD0_UPD;
    volatile U_WBC_GP0_UPD WBC_GP0_UPD;
    volatile U_WBC_G0_UPD WBC_G0_UPD;

#if 0
    if(enLayer == OPTM_VDP_LAYER_WBC_HD0)
    {
        WBC_DHD0_UPD.bits.regup = 0x1;
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_DHD0_UPD.u32)), WBC_DHD0_UPD.u32); 

        return ;
    }
    else if(enLayer == OPTM_VDP_LAYER_WBC_GP0)
#else
    if(enLayer == OPTM_VDP_LAYER_WBC_GP0)
#endif
    {
        WBC_GP0_UPD.bits.regup = 0x1;
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_GP0_UPD.u32)), WBC_GP0_UPD.u32); 

        return ;
    }
    else if(enLayer == OPTM_VDP_LAYER_WBC_2)
    {
    	WBC_G0_UPD.bits.regup = 0x1;
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_G0_UPD.u32)), WBC_G0_UPD.u32);
    }
    else if(enLayer == OPTM_VDP_LAYER_WBC_3)
    {
    	WBC_G0_UPD.bits.regup = 0x1;
        OPTM_VDP_RegWrite(((HI_U32)(&(pOptmVdpReg->WBC_G0_UPD.u32)))+0x400, WBC_G0_UPD.u32);
    }

}

HI_VOID OPTM_VDP_WBC_SetOutFmt(OPTM_VDP_LAYER_WBC_E enLayer, OPTM_VDP_WBC_OFMT_E stIntfFmt)

{
    volatile U_WBC_GP0_CTRL   WBC_GP0_CTRL;
    volatile U_WBC_G0_CTRL WBC_G0_CTRL;

#if 0
    if(enLayer == OPTM_VDP_LAYER_WBC_HD0)
    {
        WBC_DHD0_CTRL.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_DHD0_CTRL.u32)));
        WBC_DHD0_CTRL.bits.format_out = stIntfFmt;
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_DHD0_CTRL.u32)), WBC_DHD0_CTRL.u32);
    }
    else if(enLayer == OPTM_VDP_LAYER_WBC_GP0)
#else
    if(enLayer == OPTM_VDP_LAYER_WBC_GP0)
#endif
    {
        WBC_GP0_CTRL.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_GP0_CTRL.u32)));
        WBC_GP0_CTRL.bits.format_out = stIntfFmt;
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_GP0_CTRL.u32)), WBC_GP0_CTRL.u32);
    }
    else if(enLayer == OPTM_VDP_LAYER_WBC_2)
    {
    	WBC_G0_CTRL.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_G0_CTRL.u32)));
    	WBC_G0_CTRL.bits.format_out = stIntfFmt;
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_G0_CTRL.u32)), WBC_G0_CTRL.u32);
    }
    else if(enLayer == OPTM_VDP_LAYER_WBC_3)
    {
    	WBC_G0_CTRL.u32 = OPTM_VDP_RegRead(((HI_U32)(&(pOptmVdpReg->WBC_G0_CTRL.u32)))+0x400);
    	WBC_G0_CTRL.bits.format_out = stIntfFmt;
        OPTM_VDP_RegWrite(((HI_U32)(&(pOptmVdpReg->WBC_G0_CTRL.u32)))+0x400, WBC_G0_CTRL.u32);
    }

}

HI_VOID OPTM_VDP_WBC_SetSpd(OPTM_VDP_LAYER_WBC_E enLayer, HI_U32 u32ReqSpd)
{
    volatile U_WBC_DHD0_CTRL WBC_DHD0_CTRL;
    volatile U_WBC_GP0_CTRL WBC_GP0_CTRL;

    if(enLayer == OPTM_VDP_LAYER_WBC_HD0)
    {
        WBC_DHD0_CTRL.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_DHD0_CTRL.u32)));
        WBC_DHD0_CTRL.bits.req_interval = u32ReqSpd;
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_DHD0_CTRL.u32)), WBC_DHD0_CTRL.u32);
    }
    else if(enLayer == OPTM_VDP_LAYER_WBC_GP0)
    {
        WBC_GP0_CTRL.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_GP0_CTRL.u32)));
        WBC_GP0_CTRL.bits.req_interval = u32ReqSpd;
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_GP0_CTRL.u32)), WBC_GP0_CTRL.u32);
    }

}

HI_VOID  OPTM_VDP_WBC_SetLayerAddr   (OPTM_VDP_LAYER_WBC_E enLayer, HI_U32 u32LAddr,HI_U32 u32CAddr,HI_U32 u32LStr, HI_U32 u32CStr)
{
//    volatile U_WBC_DHD0_STRIDE WBC_DHD0_STRIDE;
//    U_WBC_DHD0_YADDR  WBC_DHD0_YADDR ;
//    U_WBC_DHD0_CADDR  WBC_DHD0_CADDR ;

    volatile U_WBC_GP0_STRIDE WBC_GP0_STRIDE;
//    U_WBC_GP0_YADDR  WBC_GP0_YADDR ;
//    U_WBC_GP0_CADDR  WBC_GP0_CADDR ;

    volatile U_WBC_G0_STRIDE WBC_G0_STRIDE;

#if 0
    if (enLayer == OPTM_VDP_LAYER_WBC_HD0)
    {
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_DHD0_YADDR.u32)), u32LAddr); 
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_DHD0_CADDR.u32)), u32CAddr); 
        WBC_DHD0_STRIDE.bits.wbclstride = u32LStr;
        WBC_DHD0_STRIDE.bits.wbccstride = u32CStr;
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_DHD0_STRIDE.u32)), WBC_DHD0_STRIDE.u32);

        return ;
    }

    else if (enLayer == OPTM_VDP_LAYER_WBC_GP0)
#else
    if (enLayer == OPTM_VDP_LAYER_WBC_GP0)
#endif
    {
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_GP0_YADDR.u32)), u32LAddr); 
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_GP0_CADDR.u32)), u32CAddr); 
        WBC_GP0_STRIDE.bits.wbclstride = u32LStr;
        WBC_GP0_STRIDE.bits.wbccstride = u32CStr;
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_GP0_STRIDE.u32)), WBC_GP0_STRIDE.u32);

        return ;
    }
    else if(enLayer == OPTM_VDP_LAYER_WBC_2)
    {
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_G0_YADDR.u32)), u32LAddr);
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_G0_CADDR.u32)), u32CAddr);
        WBC_G0_STRIDE.bits.wbcstride = u32LStr;
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_G0_STRIDE.u32)), WBC_G0_STRIDE.u32);
    }
    else if(enLayer == OPTM_VDP_LAYER_WBC_3)
    {
        OPTM_VDP_RegWrite(((HI_U32)(&(pOptmVdpReg->WBC_G0_YADDR.u32)))+0x400, u32LAddr);
        OPTM_VDP_RegWrite(((HI_U32)(&(pOptmVdpReg->WBC_G0_CADDR.u32)))+0x400, u32CAddr);
        WBC_G0_STRIDE.bits.wbcstride = u32LStr;
        OPTM_VDP_RegWrite(((HI_U32)(&(pOptmVdpReg->WBC_G0_STRIDE.u32)))+0x400, WBC_G0_STRIDE.u32);
    }
}

HI_VOID  OPTM_VDP_WBC_SetLayerReso     (OPTM_VDP_LAYER_WBC_E enLayer, OPTM_VDP_DISP_RECT_S  stRect)
{
    volatile U_WBC_DHD0_ORESO WBC_DHD0_ORESO;
    volatile U_WBC_GP0_ORESO WBC_GP0_ORESO;
	//volatile U_WBC_GP0_IRESO WBC_GP0_IRESO;
    if(enLayer == OPTM_VDP_LAYER_WBC_HD0)
    {
        WBC_DHD0_ORESO.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_DHD0_ORESO.u32)));
        WBC_DHD0_ORESO.bits.ow = stRect.u32OWth - 1;
        WBC_DHD0_ORESO.bits.oh = stRect.u32OHgt - 1;
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_DHD0_ORESO.u32)), WBC_DHD0_ORESO.u32); 
    }
    else if(enLayer == OPTM_VDP_LAYER_WBC_GP0)
    {
    	//WBC_GP0_IRESO.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_GP0_IRESO.u32)));
        //WBC_GP0_IRESO.bits.iw = stRect.u32IWth - 1;
        //WBC_GP0_IRESO.bits.ih = stRect.u32IHgt - 1;
        //OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_GP0_IRESO.u32)), WBC_GP0_IRESO.u32); 
		
        WBC_GP0_ORESO.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_GP0_ORESO.u32)));
        WBC_GP0_ORESO.bits.ow = stRect.u32OWth - 1;
        WBC_GP0_ORESO.bits.oh = stRect.u32OHgt - 1;
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_GP0_ORESO.u32)), WBC_GP0_ORESO.u32); 
    }




}

HI_VOID OPTM_VDP_WBC_SetDitherMode  (OPTM_VDP_LAYER_WBC_E enLayer, OPTM_VDP_DITHER_E enDitherMode)

{
//    volatile U_WBC_DHD0_DITHER_CTRL WBC_DHD0_DITHER_CTRL;
    volatile U_WBC_GP0_DITHER_CTRL WBC_GP0_DITHER_CTRL;

#if 0
    if(enLayer == OPTM_VDP_LAYER_WBC_HD0)
    {

        WBC_DHD0_DITHER_CTRL.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_DHD0_DITHER_CTRL.u32)));
        WBC_DHD0_DITHER_CTRL.bits.dither_md = enDitherMode;
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_DHD0_DITHER_CTRL.u32)), WBC_DHD0_DITHER_CTRL.u32);

    }
    else if(enLayer == OPTM_VDP_LAYER_WBC_GP0)
#else
     if(enLayer == OPTM_VDP_LAYER_WBC_GP0)
#endif
    {
#warning TODO: OPTM_VDP_WBC_SetDitherMode
        WBC_GP0_DITHER_CTRL.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_GP0_DITHER_CTRL.u32)));
        WBC_GP0_DITHER_CTRL.bits.dither_md = enDitherMode;
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_GP0_DITHER_CTRL.u32)), WBC_GP0_DITHER_CTRL.u32);

    }


}

HI_VOID OPTM_VDP_WBC_SetDitherCoef  (OPTM_VDP_LAYER_WBC_E enLayer, OPTM_VDP_DITHER_COEF_S dither_coef)

{
    
    volatile U_WBC_DHD0_DITHER_COEF0 WBC_DHD0_DITHER_COEF0;
    volatile U_WBC_DHD0_DITHER_COEF1 WBC_DHD0_DITHER_COEF1;

    volatile U_WBC_GP0_DITHER_COEF0 WBC_GP0_DITHER_COEF0;
    volatile U_WBC_GP0_DITHER_COEF1 WBC_GP0_DITHER_COEF1;



    if(enLayer == OPTM_VDP_LAYER_WBC_HD0)
    {

        WBC_DHD0_DITHER_COEF0.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_DHD0_DITHER_COEF0.u32)));
        WBC_DHD0_DITHER_COEF0.bits.dither_coef0 = dither_coef.dither_coef0 ;
        WBC_DHD0_DITHER_COEF0.bits.dither_coef1 = dither_coef.dither_coef1 ;
        WBC_DHD0_DITHER_COEF0.bits.dither_coef2 = dither_coef.dither_coef2 ;
        WBC_DHD0_DITHER_COEF0.bits.dither_coef3 = dither_coef.dither_coef3 ;
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_DHD0_DITHER_COEF0.u32)), WBC_DHD0_DITHER_COEF0.u32);

        WBC_DHD0_DITHER_COEF1.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_DHD0_DITHER_COEF1.u32)));
        WBC_DHD0_DITHER_COEF1.bits.dither_coef4 = dither_coef.dither_coef4 ;
        WBC_DHD0_DITHER_COEF1.bits.dither_coef5 = dither_coef.dither_coef5 ;
        WBC_DHD0_DITHER_COEF1.bits.dither_coef6 = dither_coef.dither_coef6 ;
        WBC_DHD0_DITHER_COEF1.bits.dither_coef7 = dither_coef.dither_coef7 ;
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_DHD0_DITHER_COEF1.u32)), WBC_DHD0_DITHER_COEF1.u32);

    }
    else if(enLayer == OPTM_VDP_LAYER_WBC_GP0)
    {

        WBC_GP0_DITHER_COEF0.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_GP0_DITHER_COEF0.u32)));
        WBC_GP0_DITHER_COEF0.bits.dither_coef0 = dither_coef.dither_coef0 ;
        WBC_GP0_DITHER_COEF0.bits.dither_coef1 = dither_coef.dither_coef1 ;
        WBC_GP0_DITHER_COEF0.bits.dither_coef2 = dither_coef.dither_coef2 ;
        WBC_GP0_DITHER_COEF0.bits.dither_coef3 = dither_coef.dither_coef3 ;
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_GP0_DITHER_COEF0.u32)), WBC_GP0_DITHER_COEF0.u32);

        WBC_GP0_DITHER_COEF1.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_GP0_DITHER_COEF1.u32)));
        WBC_GP0_DITHER_COEF1.bits.dither_coef4 = dither_coef.dither_coef4 ;
        WBC_GP0_DITHER_COEF1.bits.dither_coef5 = dither_coef.dither_coef5 ;
        WBC_GP0_DITHER_COEF1.bits.dither_coef6 = dither_coef.dither_coef6 ;
        WBC_GP0_DITHER_COEF1.bits.dither_coef7 = dither_coef.dither_coef7 ;
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_GP0_DITHER_COEF1.u32)), WBC_GP0_DITHER_COEF1.u32);

    }


}

HI_VOID  OPTM_VDP_WBC_SetCropReso (OPTM_VDP_LAYER_WBC_E enLayer, OPTM_VDP_DISP_RECT_S stRect)
{
    volatile U_WBC_DHD0_FCROP WBC_DHD0_FCROP;
    volatile U_WBC_DHD0_LCROP WBC_DHD0_LCROP;

    volatile U_WBC_GP0_FCROP WBC_GP0_FCROP;
    volatile U_WBC_GP0_LCROP WBC_GP0_LCROP;

    if(enLayer == OPTM_VDP_LAYER_WBC_HD0)
    {
        WBC_DHD0_FCROP.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_DHD0_FCROP.u32)));
        WBC_DHD0_FCROP.bits.wfcrop = stRect.u32DXS;
        WBC_DHD0_FCROP.bits.hfcrop = stRect.u32DYS;
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_DHD0_FCROP.u32)), WBC_DHD0_FCROP.u32); 

        WBC_DHD0_LCROP.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_DHD0_LCROP.u32)));
        WBC_DHD0_LCROP.bits.wlcrop = stRect.u32DXL-1;
        WBC_DHD0_LCROP.bits.hlcrop = stRect.u32DYL-1;
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_DHD0_LCROP.u32)), WBC_DHD0_LCROP.u32); 
    }
    else if(enLayer == OPTM_VDP_LAYER_WBC_GP0)
    {
        WBC_GP0_FCROP.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_GP0_FCROP.u32)));
        WBC_GP0_FCROP.bits.wfcrop = stRect.u32DXS;
        WBC_GP0_FCROP.bits.hfcrop = stRect.u32DYS;
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_GP0_FCROP.u32)), WBC_GP0_FCROP.u32); 

        WBC_GP0_LCROP.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_GP0_LCROP.u32)));
        WBC_GP0_LCROP.bits.wlcrop = stRect.u32DXL-1;
        WBC_GP0_LCROP.bits.hlcrop = stRect.u32DYL-1;
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_GP0_LCROP.u32)), WBC_GP0_LCROP.u32); 
    }
    



    return ;
}

HI_VOID OPTM_VDP_WBC_SetZmeCoefAddr(OPTM_VDP_LAYER_WBC_E enLayer, OPTM_VDP_WBC_PARA_E u32Mode, HI_U32 u32Addr)
{
    //volatile U_WBC_DHD0_HCOEFAD WBC_DHD0_HCOEFAD;
    //volatile U_WBC_DHD0_VCOEFAD WBC_DHD0_VCOEFAD;
    volatile U_GP0_HCOEFAD      GP0_HCOEFAD;
    volatile U_GP0_VCOEFAD      GP0_VCOEFAD;

    if( enLayer == OPTM_VDP_LAYER_WBC_HD0)
    {
#if 0    
        if(u32Mode == VDP_WBC_PARA_ZME_HOR)
        {
            WBC_DHD0_HCOEFAD.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_DHD0_HCOEFAD.u32)));
            WBC_DHD0_HCOEFAD.bits.coef_addr = u32Addr;
            OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_DHD0_HCOEFAD.u32)), WBC_DHD0_HCOEFAD.u32); 
        }
        else if(u32Mode == VDP_WBC_PARA_ZME_VER)
        {
            WBC_DHD0_VCOEFAD.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_DHD0_VCOEFAD.u32)));
            WBC_DHD0_VCOEFAD.bits.coef_addr = u32Addr;
            OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_DHD0_VCOEFAD.u32)), WBC_DHD0_VCOEFAD.u32); 
        }
        else
        {
            HI_PRINT("Error,VDP_WBC_DHD0_SetZmeCoefAddr() Select a Wrong Mode!\n");
        }
#endif
    }
    else if(enLayer == OPTM_VDP_LAYER_WBC_GP0)
    {

        if(u32Mode == VDP_WBC_PARA_ZME_HOR)
        {
            GP0_HCOEFAD.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_HCOEFAD.u32) + OPTM_WBC_GP0_SEL * OPTM_GP_OFFSET));
            GP0_HCOEFAD.bits.coef_addr = u32Addr;
            OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_HCOEFAD.u32) + OPTM_WBC_GP0_SEL * OPTM_GP_OFFSET), GP0_HCOEFAD.u32); 
        }
        else if(u32Mode == VDP_WBC_PARA_ZME_VER)
        {
            GP0_VCOEFAD.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_VCOEFAD.u32) + OPTM_WBC_GP0_SEL * OPTM_GP_OFFSET));
            GP0_VCOEFAD.bits.coef_addr = u32Addr;
            OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_VCOEFAD.u32) + OPTM_WBC_GP0_SEL * OPTM_GP_OFFSET), GP0_VCOEFAD.u32); 
        }
        else
        {
            HI_PRINT("Error,OPTM_VDP_GP_SetZmeCoefAddr() Select a Wrong Mode!\n");
        }
    }
    return ;

}

HI_VOID  OPTM_VDP_WBC_SetParaUpd (OPTM_VDP_LAYER_WBC_E enLayer, OPTM_VDP_WBC_PARA_E enMode)
{
    volatile U_WBC_DHD0_PARAUP WBC_DHD0_PARAUP;
    volatile U_GP0_PARAUP GP0_PARAUP;

    WBC_DHD0_PARAUP.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->WBC_DHD0_PARAUP.u32)));

    GP0_PARAUP.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_PARAUP.u32) + OPTM_WBC_GP0_SEL * OPTM_GP_OFFSET));

    if( enLayer == OPTM_VDP_LAYER_WBC_HD0)
    {
#if 0
        if(enMode == VDP_WBC_PARA_ZME_HOR)
        {
            WBC_DHD0_PARAUP.bits.wbc_hcoef_upd = 0x1;
        }
        else if(enMode == VDP_WBC_PARA_ZME_VER)
        {
            WBC_DHD0_PARAUP.bits.wbc_vcoef_upd = 0x1;
        }
        else
        {
            HI_PRINT("error,VDP_WBC_DHD0_SetParaUpd() select wrong mode!\n");
        }
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_DHD0_PARAUP.u32)), WBC_DHD0_PARAUP.u32); 
#endif
        return ;
    }
    else if(enLayer == OPTM_VDP_LAYER_WBC_GP0)
    {
        if(enMode == VDP_WBC_PARA_ZME_HOR)
        {
            GP0_PARAUP.bits.gp0_hcoef_upd = 0x1;
        }
        else if(enMode == VDP_WBC_PARA_ZME_VER)
        {
            GP0_PARAUP.bits.gp0_vcoef_upd = 0x1;
        }
        else
        {
            HI_PRINT("error,OPTM_VDP_GP_SetParaUpd() select wrong mode!\n");
        }

        OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_PARAUP.u32) + OPTM_WBC_GP0_SEL * OPTM_GP_OFFSET), GP0_PARAUP.u32); 
        return ;
    }
}


HI_VOID OPTM_VDP_WBC_SetZmeEnable  (OPTM_VDP_LAYER_WBC_E enLayer, OPTM_VDP_ZME_MODE_E enMode, HI_U32 u32bEnable)
{
    volatile U_WBC_DHD0_ZME_HSP WBC_DHD0_ZME_HSP;
    volatile U_WBC_DHD0_ZME_VSP WBC_DHD0_ZME_VSP;
    volatile U_GP0_ZME_HSP      GP0_ZME_HSP;
    volatile U_GP0_ZME_VSP      GP0_ZME_VSP;

    /*WBC zoom enable */
    if(enLayer == OPTM_VDP_LAYER_WBC_HD0 )
    {
        if((enMode == VDP_ZME_MODE_HORL)||(enMode == VDP_ZME_MODE_HOR)||(enMode == VDP_ZME_MODE_ALL))
        {
            WBC_DHD0_ZME_HSP.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_DHD0_ZME_HSP.u32)));
            WBC_DHD0_ZME_HSP.bits.hlmsc_en = u32bEnable;
            WBC_DHD0_ZME_HSP.bits.hlfir_en = 1;
            OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_DHD0_ZME_HSP.u32)), WBC_DHD0_ZME_HSP.u32); 
        }

        if((enMode == VDP_ZME_MODE_HORC)||(enMode == VDP_ZME_MODE_HOR)||(enMode == VDP_ZME_MODE_ALL))
        {
            WBC_DHD0_ZME_VSP.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_DHD0_ZME_VSP.u32)));
            WBC_DHD0_ZME_HSP.bits.hchmsc_en = u32bEnable;
            WBC_DHD0_ZME_HSP.bits.hchfir_en = 1;
            OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_DHD0_ZME_HSP.u32)), WBC_DHD0_ZME_HSP.u32); 
        }

        if((enMode == VDP_ZME_MODE_NONL)||(enMode == VDP_ZME_MODE_ALL))
        {
            WBC_DHD0_ZME_HSP.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_DHD0_ZME_HSP.u32)));
            WBC_DHD0_ZME_HSP.bits.non_lnr_en = u32bEnable;
            OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_DHD0_ZME_HSP.u32)), WBC_DHD0_ZME_HSP.u32);  
        }


        if((enMode == VDP_ZME_MODE_VERL)||(enMode == VDP_ZME_MODE_VER)||(enMode == VDP_ZME_MODE_ALL))
        {
            WBC_DHD0_ZME_VSP.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_DHD0_ZME_VSP.u32)));
            WBC_DHD0_ZME_VSP.bits.vlmsc_en = u32bEnable;
            WBC_DHD0_ZME_VSP.bits.vlfir_en = 1;
            OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_DHD0_ZME_VSP.u32)), WBC_DHD0_ZME_VSP.u32); 
        }

        if((enMode == VDP_ZME_MODE_VERC)||(enMode == VDP_ZME_MODE_VER)||(enMode == VDP_ZME_MODE_ALL))
        {
            WBC_DHD0_ZME_VSP.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_DHD0_ZME_VSP.u32)));
            WBC_DHD0_ZME_VSP.bits.vchmsc_en = u32bEnable;
            WBC_DHD0_ZME_VSP.bits.vchfir_en = 1;
            OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_DHD0_ZME_VSP.u32)), WBC_DHD0_ZME_VSP.u32); 
        }

    }
    else if(enLayer == OPTM_VDP_LAYER_WBC_GP0)
    {
        if((enMode == VDP_ZME_MODE_HOR)||(enMode == VDP_ZME_MODE_ALL))
        {
            GP0_ZME_HSP.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_HSP.u32) + OPTM_WBC_GP0_SEL * OPTM_GP_OFFSET));
            GP0_ZME_HSP.bits.hsc_en = u32bEnable;
            OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_HSP.u32) + OPTM_WBC_GP0_SEL * OPTM_GP_OFFSET), GP0_ZME_HSP.u32); 
        }

        if((enMode == VDP_ZME_MODE_VER)||(enMode == VDP_ZME_MODE_ALL))
        {
            GP0_ZME_VSP.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_VSP.u32) + OPTM_WBC_GP0_SEL * OPTM_GP_OFFSET));
            GP0_ZME_VSP.bits.vsc_en = u32bEnable;
            OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_VSP.u32) + OPTM_WBC_GP0_SEL * OPTM_GP_OFFSET), GP0_ZME_VSP.u32); 
        }

        return ;
    }

    return ;

}

/* WBC set Median filter enable */
HI_VOID  OPTM_VDP_WBC_SetMidEnable(OPTM_VDP_LAYER_WBC_E enLayer, OPTM_VDP_ZME_MODE_E enMode,HI_U32 bEnable)

{

    volatile U_WBC_DHD0_ZME_HSP WBC_DHD0_ZME_HSP;
    volatile U_WBC_DHD0_ZME_VSP WBC_DHD0_ZME_VSP;
    volatile U_GP0_ZME_HSP      GP0_ZME_HSP;
    volatile U_GP0_ZME_VSP      GP0_ZME_VSP;
    if(enLayer == OPTM_VDP_LAYER_WBC_HD0 )
    {
        if((enMode == VDP_ZME_MODE_HORL)||(enMode == VDP_ZME_MODE_HOR)||(enMode == VDP_ZME_MODE_ALL))
        {
            WBC_DHD0_ZME_HSP.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_DHD0_ZME_HSP.u32)));
            WBC_DHD0_ZME_HSP.bits.hlmid_en = bEnable;
            OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_DHD0_ZME_HSP.u32)), WBC_DHD0_ZME_HSP.u32);

        }

        if((enMode == VDP_ZME_MODE_HORC)||(enMode == VDP_ZME_MODE_HOR)||(enMode == VDP_ZME_MODE_ALL))
        {
            WBC_DHD0_ZME_HSP.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_DHD0_ZME_HSP.u32)));
            WBC_DHD0_ZME_HSP.bits.hchmid_en = bEnable;
            OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_DHD0_ZME_HSP.u32)), WBC_DHD0_ZME_HSP.u32);  
        }

        if((enMode == VDP_ZME_MODE_VERL)||(enMode == VDP_ZME_MODE_VER)||(enMode == VDP_ZME_MODE_ALL))
        {
            WBC_DHD0_ZME_VSP.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_DHD0_ZME_VSP.u32)));
            WBC_DHD0_ZME_VSP.bits.vlmid_en = bEnable;
            OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_DHD0_ZME_VSP.u32)), WBC_DHD0_ZME_VSP.u32);  
        }

        if((enMode == VDP_ZME_MODE_VERC)||(enMode == VDP_ZME_MODE_VER)||(enMode == VDP_ZME_MODE_ALL))
        {
            WBC_DHD0_ZME_VSP.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_DHD0_ZME_VSP.u32)));
            WBC_DHD0_ZME_VSP.bits.vchmid_en = bEnable;
            OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_DHD0_ZME_VSP.u32)), WBC_DHD0_ZME_VSP.u32);  
        }
    }
    else if(enLayer == OPTM_VDP_LAYER_WBC_GP0)
    {
        if((enMode == VDP_ZME_MODE_ALPHA)||(enMode == VDP_ZME_MODE_HOR)||(enMode == VDP_ZME_MODE_ALL))
        {
            GP0_ZME_HSP.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_HSP.u32) + OPTM_WBC_GP0_SEL * OPTM_GP_OFFSET));
            GP0_ZME_HSP.bits.hamid_en = bEnable;
            OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_HSP.u32) + OPTM_WBC_GP0_SEL * OPTM_GP_OFFSET), GP0_ZME_HSP.u32); 
        }
        if((enMode == VDP_ZME_MODE_HORL)||(enMode == VDP_ZME_MODE_HOR)||(enMode == VDP_ZME_MODE_ALL))
        {
            GP0_ZME_HSP.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_HSP.u32) + OPTM_WBC_GP0_SEL * OPTM_GP_OFFSET));
            GP0_ZME_HSP.bits.hlmid_en = bEnable;
            OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_HSP.u32) + OPTM_WBC_GP0_SEL * OPTM_GP_OFFSET), GP0_ZME_HSP.u32); 
        }

        if((enMode == VDP_ZME_MODE_HORC)||(enMode == VDP_ZME_MODE_HOR)||(enMode == VDP_ZME_MODE_ALL))
        {
            GP0_ZME_HSP.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_HSP.u32) + OPTM_WBC_GP0_SEL * OPTM_GP_OFFSET));
            GP0_ZME_HSP.bits.hchmid_en = bEnable;
            OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_HSP.u32) + OPTM_WBC_GP0_SEL * OPTM_GP_OFFSET), GP0_ZME_HSP.u32); 
        }

        if((enMode == VDP_ZME_MODE_ALPHAV)||(enMode == VDP_ZME_MODE_VER)||(enMode == VDP_ZME_MODE_ALL))
        {
            GP0_ZME_VSP.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_VSP.u32) + OPTM_WBC_GP0_SEL * OPTM_GP_OFFSET));
            GP0_ZME_VSP.bits.vamid_en = bEnable;
            OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_VSP.u32) + OPTM_WBC_GP0_SEL * OPTM_GP_OFFSET), GP0_ZME_VSP.u32); 
        }
        if((enMode == VDP_ZME_MODE_VERL)||(enMode == VDP_ZME_MODE_VER)||(enMode == VDP_ZME_MODE_ALL))
        {
            GP0_ZME_VSP.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_VSP.u32) + OPTM_WBC_GP0_SEL * OPTM_GP_OFFSET));
            GP0_ZME_VSP.bits.vlmid_en = bEnable;
            OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_VSP.u32) + OPTM_WBC_GP0_SEL * OPTM_GP_OFFSET), GP0_ZME_VSP.u32); 
        }

        if((enMode == VDP_ZME_MODE_VERC)||(enMode == VDP_ZME_MODE_VER)||(enMode == VDP_ZME_MODE_ALL))
        {
            GP0_ZME_VSP.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_VSP.u32) + OPTM_WBC_GP0_SEL * OPTM_GP_OFFSET));
            GP0_ZME_VSP.bits.vchmid_en = bEnable;
            OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_VSP.u32) + OPTM_WBC_GP0_SEL * OPTM_GP_OFFSET), GP0_ZME_VSP.u32); 
        }
    }
    return;

}

HI_VOID OPTM_VDP_WBC_SetFirEnable(OPTM_VDP_LAYER_WBC_E enLayer, OPTM_VDP_ZME_MODE_E enMode, HI_U32 bEnable)
{
    volatile U_WBC_DHD0_ZME_HSP WBC_DHD0_ZME_HSP;
    volatile U_WBC_DHD0_ZME_VSP WBC_DHD0_ZME_VSP;
    volatile U_GP0_ZME_HSP      GP0_ZME_HSP;
    volatile U_GP0_ZME_VSP      GP0_ZME_VSP;
    
    if(enLayer == OPTM_VDP_LAYER_WBC_HD0 )
    {
        if((enMode == VDP_ZME_MODE_HORL)||(enMode == VDP_ZME_MODE_HOR)||(enMode == VDP_ZME_MODE_ALL))
        {
            WBC_DHD0_ZME_HSP.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_DHD0_ZME_HSP.u32)));
            WBC_DHD0_ZME_HSP.bits.hlfir_en = bEnable;
            OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_DHD0_ZME_HSP.u32)), WBC_DHD0_ZME_HSP.u32);

        }

        if((enMode == VDP_ZME_MODE_HORC)||(enMode == VDP_ZME_MODE_HOR)||(enMode == VDP_ZME_MODE_ALL))
        {
            WBC_DHD0_ZME_HSP.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_DHD0_ZME_HSP.u32)));
            WBC_DHD0_ZME_HSP.bits.hchfir_en = bEnable;
            OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_DHD0_ZME_HSP.u32)), WBC_DHD0_ZME_HSP.u32);  
        }

        if((enMode == VDP_ZME_MODE_VERL)||(enMode == VDP_ZME_MODE_VER)||(enMode == VDP_ZME_MODE_ALL))
        {
            WBC_DHD0_ZME_VSP.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_DHD0_ZME_VSP.u32)));
            WBC_DHD0_ZME_VSP.bits.vlfir_en = bEnable;
            OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_DHD0_ZME_VSP.u32)), WBC_DHD0_ZME_VSP.u32);  
        }

        if((enMode == VDP_ZME_MODE_VERC)||(enMode == VDP_ZME_MODE_VER)||(enMode == VDP_ZME_MODE_ALL))
        {
            WBC_DHD0_ZME_VSP.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_DHD0_ZME_VSP.u32)));
            WBC_DHD0_ZME_VSP.bits.vchfir_en = bEnable;
            OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_DHD0_ZME_VSP.u32)), WBC_DHD0_ZME_VSP.u32);  
        }
    }
    else if(enLayer == OPTM_VDP_LAYER_WBC_GP0)
    {
        if((enMode == VDP_ZME_MODE_ALPHA)||(enMode == VDP_ZME_MODE_HOR)||(enMode == VDP_ZME_MODE_ALL))
        {
            GP0_ZME_HSP.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_HSP.u32) + OPTM_WBC_GP0_SEL * OPTM_GP_OFFSET));
            GP0_ZME_HSP.bits.hafir_en = bEnable;
            OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_HSP.u32) + OPTM_WBC_GP0_SEL * OPTM_GP_OFFSET), GP0_ZME_HSP.u32); 
        }

        if((enMode == VDP_ZME_MODE_HOR)||(enMode == VDP_ZME_MODE_ALL))
        {
            GP0_ZME_HSP.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_HSP.u32) + OPTM_WBC_GP0_SEL * OPTM_GP_OFFSET));
            GP0_ZME_HSP.bits.hfir_en = bEnable;
            OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_HSP.u32) + OPTM_WBC_GP0_SEL * OPTM_GP_OFFSET), GP0_ZME_HSP.u32); 
        }

        if((enMode == VDP_ZME_MODE_ALPHAV)||(enMode == VDP_ZME_MODE_VER)||(enMode == VDP_ZME_MODE_ALL))
        {
            GP0_ZME_VSP.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_VSP.u32) + OPTM_WBC_GP0_SEL * OPTM_GP_OFFSET));
            GP0_ZME_VSP.bits.vafir_en = bEnable;
            OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_VSP.u32) + OPTM_WBC_GP0_SEL * OPTM_GP_OFFSET), GP0_ZME_VSP.u32); 
        }

        if((enMode == VDP_ZME_MODE_VER)||(enMode == VDP_ZME_MODE_ALL))
        {
            GP0_ZME_VSP.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_VSP.u32) + OPTM_WBC_GP0_SEL * OPTM_GP_OFFSET));
            GP0_ZME_VSP.bits.vfir_en = bEnable;
            OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_VSP.u32) + OPTM_WBC_GP0_SEL * OPTM_GP_OFFSET), GP0_ZME_VSP.u32); 
        }
    }
     
     
    return ;
    
}


HI_VOID OPTM_VDP_WBC_SetZmeVerTap(OPTM_VDP_LAYER_WBC_E enLayer, OPTM_VDP_ZME_MODE_E enMode, HI_U32 u32VerTap)

{
    volatile U_WBC_DHD0_ZME_VSP WBC_DHD0_ZME_VSP;
    if(enLayer == OPTM_VDP_LAYER_WBC_HD0 )
    {
        /*
        if ((enMode == VDP_ZME_MODE_VERL) || (enMode == VDP_ZME_MODE_VER) || (enMode == VDP_ZME_MODE_ALL))
        {
            WBC_DHD0_ZME_VSP.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_DHD0_ZME_VSP.u32)));
            WBC_DHD0_ZME_VSP.bits.vsc_luma_tap = u32VerTap;
            OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_DHD0_ZME_VSP.u32)), WBC_DHD0_ZME_VSP.u32);
        }
        */


        if ((enMode == VDP_ZME_MODE_VERC) || (enMode == VDP_ZME_MODE_VER) || (enMode == VDP_ZME_MODE_ALL))
        {
            WBC_DHD0_ZME_VSP.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_DHD0_ZME_VSP.u32)));
            WBC_DHD0_ZME_VSP.bits.vsc_chroma_tap = u32VerTap;
            OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_DHD0_ZME_VSP.u32)), WBC_DHD0_ZME_VSP.u32);
        }
    }

}

HI_VOID OPTM_VDP_WBC_SetZmeHfirOrder(OPTM_VDP_LAYER_WBC_E enLayer, HI_U32 u32HfirOrder)
{
    volatile U_WBC_DHD0_ZME_HSP WBC_DHD0_ZME_HSP;    
    volatile U_GP0_ZME_HSP      GP0_ZME_HSP;
    if(enLayer == OPTM_VDP_LAYER_WBC_HD0 )
    {

        WBC_DHD0_ZME_HSP.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_DHD0_ZME_HSP.u32)));
        WBC_DHD0_ZME_HSP.bits.hfir_order = u32HfirOrder;
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_DHD0_ZME_HSP.u32)), WBC_DHD0_ZME_HSP.u32); 
    }
    else if(enLayer == OPTM_VDP_LAYER_WBC_GP0)
    {
        GP0_ZME_HSP.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_HSP.u32) + OPTM_WBC_GP0_SEL * OPTM_GP_OFFSET));
        GP0_ZME_HSP.bits.hfir_order = u32HfirOrder;
        OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_HSP.u32) + OPTM_WBC_GP0_SEL * OPTM_GP_OFFSET), GP0_ZME_HSP.u32); 
    }
    return ;
}

HI_VOID OPTM_VDP_WBC_SetZmeHorRatio(OPTM_VDP_LAYER_WBC_E enLayer, HI_U32 u32Ratio)

{
    volatile U_WBC_DHD0_ZME_HSP WBC_DHD0_ZME_HSP;
    volatile U_GP0_ZME_HSP      GP0_ZME_HSP;
    if(enLayer == OPTM_VDP_LAYER_WBC_HD0 )
    {


        WBC_DHD0_ZME_HSP.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_DHD0_ZME_HSP.u32)));
        WBC_DHD0_ZME_HSP.bits.hratio = u32Ratio;
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_DHD0_ZME_HSP.u32)), WBC_DHD0_ZME_HSP.u32); 
    }
    else if(enLayer == OPTM_VDP_LAYER_WBC_GP0)
    {
        GP0_ZME_HSP.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_HSP.u32) + OPTM_WBC_GP0_SEL * OPTM_GP_OFFSET));
        GP0_ZME_HSP.bits.hratio = u32Ratio;
        OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_HSP.u32) + OPTM_WBC_GP0_SEL * OPTM_GP_OFFSET), GP0_ZME_HSP.u32); 
    }
    return ;
}

HI_VOID  OPTM_VDP_WBC_SetZmeInFmt(OPTM_VDP_LAYER_WBC_E enLayer, OPTM_VDP_PROC_FMT_E u32Fmt)
{
    volatile U_WBC_DHD0_ZME_VSP WBC_DHD0_ZME_VSP;

    if(enLayer == OPTM_VDP_LAYER_WBC_HD0 )
    {

        WBC_DHD0_ZME_VSP.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_DHD0_ZME_VSP.u32)));
        WBC_DHD0_ZME_VSP.bits.zme_in_fmt = u32Fmt;
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_DHD0_ZME_VSP.u32)), WBC_DHD0_ZME_VSP.u32); 
    }

    return ;
}


HI_VOID  OPTM_VDP_WBC_SetZmeOutFmt(OPTM_VDP_LAYER_WBC_E enLayer, OPTM_VDP_PROC_FMT_E u32Fmt)
{
   volatile U_WBC_DHD0_ZME_VSP WBC_DHD0_ZME_VSP;

    if(enLayer == OPTM_VDP_LAYER_WBC_HD0 )
    {
        WBC_DHD0_ZME_VSP.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_DHD0_ZME_VSP.u32)));
        WBC_DHD0_ZME_VSP.bits.zme_out_fmt = u32Fmt;
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_DHD0_ZME_VSP.u32)), WBC_DHD0_ZME_VSP.u32); 
    }

    return ;
}

HI_VOID  OPTM_VDP_WBC_SetZmeVerRatio(OPTM_VDP_LAYER_WBC_E enLayer, HI_U32 u32Ratio)
{
    volatile U_WBC_DHD0_ZME_VSR WBC_DHD0_ZME_VSR;
    volatile U_GP0_ZME_VSR        GP0_ZME_VSR;
    
    
    if(enLayer == OPTM_VDP_LAYER_WBC_HD0 )
    {
        WBC_DHD0_ZME_VSR.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_DHD0_ZME_VSR.u32)));
        WBC_DHD0_ZME_VSR.bits.vratio = u32Ratio;
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_DHD0_ZME_VSR.u32)), WBC_DHD0_ZME_VSR.u32); 
    }
    else if(enLayer == OPTM_VDP_LAYER_WBC_GP0)
    {
        GP0_ZME_VSR.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_VSR.u32) + OPTM_WBC_GP0_SEL * OPTM_GP_OFFSET));
        GP0_ZME_VSR.bits.vratio = u32Ratio;
        OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_VSR.u32) + OPTM_WBC_GP0_SEL * OPTM_GP_OFFSET), GP0_ZME_VSR.u32); 
    }
    return ;
}

HI_VOID  OPTM_VDP_WBC_SetZmePhase    (OPTM_VDP_LAYER_WBC_E enLayer, OPTM_VDP_ZME_MODE_E enMode,HI_S32 s32Phase)
{
    volatile U_WBC_DHD0_ZME_VOFFSET   WBC_DHD0_ZME_VOFFSET;
    volatile U_WBC_DHD0_ZME_VBOFFSET  WBC_DHD0_ZME_VBOFFSET;
    volatile U_GP0_ZME_HOFFSET        GP0_ZME_HOFFSET;
    volatile U_GP0_ZME_VOFFSET        GP0_ZME_VOFFSET;
    
    if(enLayer == OPTM_VDP_LAYER_WBC_HD0)
    {

        if((enMode == VDP_ZME_MODE_VERL)||(enMode == VDP_ZME_MODE_VER)||(enMode == VDP_ZME_MODE_ALL))
        {
            WBC_DHD0_ZME_VOFFSET.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_DHD0_ZME_VOFFSET.u32)));
            WBC_DHD0_ZME_VOFFSET.bits.vluma_offset = s32Phase;
            OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_DHD0_ZME_VOFFSET.u32)), WBC_DHD0_ZME_VOFFSET.u32); 
        }

        if((enMode == VDP_ZME_MODE_VERC)||(enMode == VDP_ZME_MODE_VER)||(enMode == VDP_ZME_MODE_ALL))
        {
            WBC_DHD0_ZME_VOFFSET.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_DHD0_ZME_VOFFSET.u32)));
            WBC_DHD0_ZME_VOFFSET.bits.vchroma_offset = s32Phase;
            OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_DHD0_ZME_VOFFSET.u32)), WBC_DHD0_ZME_VOFFSET.u32); 
        }

        if((enMode == VDP_ZME_MODE_VERB)||(enMode == VDP_ZME_MODE_VERL)||(enMode == VDP_ZME_MODE_ALL))
        {
            WBC_DHD0_ZME_VBOFFSET.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_DHD0_ZME_VBOFFSET.u32)));
            WBC_DHD0_ZME_VBOFFSET.bits.vbluma_offset = s32Phase;
            OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_DHD0_ZME_VBOFFSET.u32)), WBC_DHD0_ZME_VBOFFSET.u32); 
        }

        if((enMode == VDP_ZME_MODE_VERB)||(enMode == VDP_ZME_MODE_VERC)||(enMode == VDP_ZME_MODE_ALL))
        {
            WBC_DHD0_ZME_VBOFFSET.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_DHD0_ZME_VBOFFSET.u32)));
            WBC_DHD0_ZME_VBOFFSET.bits.vbchroma_offset = s32Phase;
            OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_DHD0_ZME_VBOFFSET.u32)), WBC_DHD0_ZME_VBOFFSET.u32); 
        }
    }
    else if(enLayer == OPTM_VDP_LAYER_WBC_GP0)
    {
        if((enMode == VDP_ZME_MODE_HORL) || (enMode == VDP_ZME_MODE_HOR)||(enMode == VDP_ZME_MODE_ALL))
        {
            GP0_ZME_HOFFSET.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_HOFFSET.u32) + OPTM_WBC_GP0_SEL * OPTM_GP_OFFSET));
            GP0_ZME_HOFFSET.bits.hor_loffset = s32Phase;
            OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_HOFFSET.u32) + OPTM_WBC_GP0_SEL * OPTM_GP_OFFSET), GP0_ZME_HOFFSET.u32); 
        }

        if((enMode == VDP_ZME_MODE_HORC) || (enMode == VDP_ZME_MODE_HOR)||(enMode == VDP_ZME_MODE_ALL))
        {
            GP0_ZME_HOFFSET.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_HOFFSET.u32) + OPTM_WBC_GP0_SEL * OPTM_GP_OFFSET));
            GP0_ZME_HOFFSET.bits.hor_coffset = s32Phase;
            OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_HOFFSET.u32) + OPTM_WBC_GP0_SEL * OPTM_GP_OFFSET), GP0_ZME_HOFFSET.u32); 
        }

        if((enMode == VDP_ZME_MODE_VERL)||(enMode == VDP_ZME_MODE_VER)||(enMode == VDP_ZME_MODE_ALL))
        {
            GP0_ZME_VOFFSET.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_VOFFSET.u32) + OPTM_WBC_GP0_SEL * OPTM_GP_OFFSET));
            GP0_ZME_VOFFSET.bits.vbtm_offset = s32Phase;
            OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_VOFFSET.u32) + OPTM_WBC_GP0_SEL * OPTM_GP_OFFSET), GP0_ZME_VOFFSET.u32); 
        }

        if((enMode == VDP_ZME_MODE_VERC)||(enMode == VDP_ZME_MODE_VER)||(enMode == VDP_ZME_MODE_ALL))
        {
            GP0_ZME_VOFFSET.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_VOFFSET.u32) + OPTM_WBC_GP0_SEL * OPTM_GP_OFFSET));
            GP0_ZME_VOFFSET.bits.vtp_offset = s32Phase;
            OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_ZME_VOFFSET.u32) + OPTM_WBC_GP0_SEL * OPTM_GP_OFFSET), GP0_ZME_VOFFSET.u32); 
        }
    }

    return ;
}

HI_VOID  OPTM_VDP_WBC_SetCscEnable  (OPTM_VDP_LAYER_WBC_E enLayer, HI_U32 enCSC)
{   
    volatile U_WBC_DHD0_CSCIDC WBC_DHD0_CSCIDC;
    volatile U_GP0_CSC_IDC     GP0_CSC_IDC;

    if(enLayer == OPTM_VDP_LAYER_WBC_HD0)
    {

        WBC_DHD0_CSCIDC.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_DHD0_CSCIDC.u32)));        
        WBC_DHD0_CSCIDC.bits.csc_en = enCSC;
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_DHD0_CSCIDC.u32)), WBC_DHD0_CSCIDC.u32);
    }
    else if(enLayer == OPTM_VDP_LAYER_WBC_GP0)
    {

        GP0_CSC_IDC.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_CSC_IDC.u32) + OPTM_WBC_GP0_SEL * OPTM_GP_OFFSET));
        GP0_CSC_IDC.bits.csc_en = enCSC;
        OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_CSC_IDC.u32) + OPTM_WBC_GP0_SEL * OPTM_GP_OFFSET), GP0_CSC_IDC.u32); 

    }
    return ;
}

HI_VOID   OPTM_VDP_WBC_SetCscDcCoef(OPTM_VDP_LAYER_WBC_E enLayer,OPTM_VDP_CSC_DC_COEF_S stCscCoef)
{   
    volatile U_WBC_DHD0_CSCIDC WBC_DHD0_CSCIDC;
    volatile U_WBC_DHD0_CSCODC WBC_DHD0_CSCODC;
    volatile U_GP0_CSC_IDC  GP0_CSC_IDC;
    volatile U_GP0_CSC_ODC  GP0_CSC_ODC;
    volatile U_GP0_CSC_IODC GP0_CSC_IODC;
    
    if(enLayer == OPTM_VDP_LAYER_WBC_HD0)
    {
        WBC_DHD0_CSCIDC.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_DHD0_CSCIDC.u32)));
        
        WBC_DHD0_CSCIDC.bits.cscidc2 = stCscCoef.csc_in_dc2;
        WBC_DHD0_CSCIDC.bits.cscidc1 = stCscCoef.csc_in_dc1;
        WBC_DHD0_CSCIDC.bits.cscidc0 = stCscCoef.csc_in_dc0;
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_DHD0_CSCIDC.u32)), WBC_DHD0_CSCIDC.u32);

        WBC_DHD0_CSCODC.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_DHD0_CSCODC.u32)));
        WBC_DHD0_CSCODC.bits.cscodc2 = stCscCoef.csc_out_dc2;
        WBC_DHD0_CSCODC.bits.cscodc1 = stCscCoef.csc_out_dc1;
        WBC_DHD0_CSCODC.bits.cscodc0 = stCscCoef.csc_out_dc0;
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_DHD0_CSCODC.u32)), WBC_DHD0_CSCODC.u32);
    }

    else if(enLayer == OPTM_VDP_LAYER_WBC_GP0)
    {
        GP0_CSC_IDC.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_CSC_IDC.u32) + OPTM_WBC_GP0_SEL * OPTM_GP_OFFSET));
        GP0_CSC_IDC.bits.cscidc1  = stCscCoef.csc_in_dc1;
        GP0_CSC_IDC.bits.cscidc0  = stCscCoef.csc_in_dc0;
        OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_CSC_IDC.u32) + OPTM_WBC_GP0_SEL * OPTM_GP_OFFSET), GP0_CSC_IDC.u32); 

        GP0_CSC_ODC.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_CSC_ODC.u32) + OPTM_WBC_GP0_SEL * OPTM_GP_OFFSET));
        GP0_CSC_ODC.bits.cscodc1 = stCscCoef.csc_out_dc1;
        GP0_CSC_ODC.bits.cscodc0 = stCscCoef.csc_out_dc0;
        OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_CSC_ODC.u32) + OPTM_WBC_GP0_SEL * OPTM_GP_OFFSET), GP0_CSC_ODC.u32); 

        GP0_CSC_IODC.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_CSC_IODC.u32) + OPTM_WBC_GP0_SEL * OPTM_GP_OFFSET));
        GP0_CSC_IODC.bits.cscodc2 = stCscCoef.csc_out_dc2;
        GP0_CSC_IODC.bits.cscidc2 = stCscCoef.csc_in_dc2;
        OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_CSC_IODC.u32) + OPTM_WBC_GP0_SEL * OPTM_GP_OFFSET), GP0_CSC_IODC.u32); 
    }
    return ;
}

HI_VOID   OPTM_VDP_WBC_SetCscCoef(OPTM_VDP_LAYER_WBC_E enLayer,OPTM_VDP_CSC_COEF_S stCscCoef)
{   
    volatile U_WBC_DHD0_CSCP0        WBC_DHD0_CSCP0;
    volatile U_WBC_DHD0_CSCP1        WBC_DHD0_CSCP1;
    volatile U_WBC_DHD0_CSCP2        WBC_DHD0_CSCP2;
    volatile U_WBC_DHD0_CSCP3        WBC_DHD0_CSCP3;
    volatile U_WBC_DHD0_CSCP4        WBC_DHD0_CSCP4;
    volatile U_GP0_CSC_P0            GP0_CSC_P0;
    volatile U_GP0_CSC_P1            GP0_CSC_P1;
    volatile U_GP0_CSC_P2            GP0_CSC_P2;
    volatile U_GP0_CSC_P3            GP0_CSC_P3;
    volatile U_GP0_CSC_P4            GP0_CSC_P4;


    if(enLayer == OPTM_VDP_LAYER_WBC_HD0)
    {
        WBC_DHD0_CSCP0.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_DHD0_CSCP0.u32)));
        WBC_DHD0_CSCP0.bits.cscp00 = stCscCoef.csc_coef00;
        WBC_DHD0_CSCP0.bits.cscp01 = stCscCoef.csc_coef01;
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_DHD0_CSCP0.u32)), WBC_DHD0_CSCP0.u32);

        WBC_DHD0_CSCP1.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_DHD0_CSCP1.u32)));
        WBC_DHD0_CSCP1.bits.cscp02 = stCscCoef.csc_coef02;
        WBC_DHD0_CSCP1.bits.cscp10 = stCscCoef.csc_coef10;
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_DHD0_CSCP1.u32)), WBC_DHD0_CSCP1.u32);

        WBC_DHD0_CSCP2.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_DHD0_CSCP2.u32)));
        WBC_DHD0_CSCP2.bits.cscp11 = stCscCoef.csc_coef11;
        WBC_DHD0_CSCP2.bits.cscp12 = stCscCoef.csc_coef12;
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_DHD0_CSCP2.u32)), WBC_DHD0_CSCP2.u32);

        WBC_DHD0_CSCP3.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_DHD0_CSCP3.u32)));
        WBC_DHD0_CSCP3.bits.cscp20 = stCscCoef.csc_coef20;
        WBC_DHD0_CSCP3.bits.cscp21 = stCscCoef.csc_coef21;
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_DHD0_CSCP3.u32)), WBC_DHD0_CSCP3.u32);

        WBC_DHD0_CSCP4.u32 = OPTM_VDP_RegRead((HI_U32)(&(pOptmVdpReg->WBC_DHD0_CSCP4.u32)));
        WBC_DHD0_CSCP4.bits.cscp22 = stCscCoef.csc_coef22;
        OPTM_VDP_RegWrite((HI_U32)(&(pOptmVdpReg->WBC_DHD0_CSCP4.u32)), WBC_DHD0_CSCP4.u32);
    }
    else if(enLayer == OPTM_VDP_LAYER_WBC_GP0)
    {
        GP0_CSC_P0.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_CSC_P0.u32)+OPTM_WBC_GP0_SEL*OPTM_GP_OFFSET));
        GP0_CSC_P0.bits.cscp00 = stCscCoef.csc_coef00;
        GP0_CSC_P0.bits.cscp01 = stCscCoef.csc_coef01;
        OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_CSC_P0.u32)+OPTM_WBC_GP0_SEL*OPTM_GP_OFFSET), GP0_CSC_P0.u32);

        GP0_CSC_P1.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_CSC_P1.u32)+OPTM_WBC_GP0_SEL*OPTM_GP_OFFSET));
        GP0_CSC_P1.bits.cscp02 = stCscCoef.csc_coef02;
        GP0_CSC_P1.bits.cscp10 = stCscCoef.csc_coef10;
        OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_CSC_P1.u32)+OPTM_WBC_GP0_SEL*OPTM_GP_OFFSET), GP0_CSC_P1.u32);

        GP0_CSC_P2.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_CSC_P2.u32)+OPTM_WBC_GP0_SEL*OPTM_GP_OFFSET));
        GP0_CSC_P2.bits.cscp11 = stCscCoef.csc_coef11;
        GP0_CSC_P2.bits.cscp12 = stCscCoef.csc_coef12;
        OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_CSC_P2.u32)+OPTM_WBC_GP0_SEL*OPTM_GP_OFFSET), GP0_CSC_P2.u32);

        GP0_CSC_P3.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_CSC_P3.u32)+OPTM_WBC_GP0_SEL*OPTM_GP_OFFSET));
        GP0_CSC_P3.bits.cscp20 = stCscCoef.csc_coef20;
        GP0_CSC_P3.bits.cscp21 = stCscCoef.csc_coef21;
        OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_CSC_P3.u32)+OPTM_WBC_GP0_SEL*OPTM_GP_OFFSET), GP0_CSC_P3.u32);

        GP0_CSC_P4.u32 = OPTM_VDP_RegRead((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_CSC_P4.u32)+OPTM_WBC_GP0_SEL*OPTM_GP_OFFSET));
        GP0_CSC_P4.bits.cscp22 = stCscCoef.csc_coef22;
        OPTM_VDP_RegWrite((HI_U32)((HI_U32)&(pOptmVdpReg->GP0_CSC_P4.u32)+OPTM_WBC_GP0_SEL*OPTM_GP_OFFSET), GP0_CSC_P4.u32);
    }    

}

HI_VOID OPTM_VDP_WBC_SetCscMode( OPTM_VDP_LAYER_WBC_E enLayer, OPTM_VDP_CSC_MODE_E enCscMode)
{
#if 0
    OPTM_VDP_CSC_COEF_S    st_csc_coef;
    OPTM_VDP_CSC_DC_COEF_S st_csc_dc_coef;

    HI_U32 u32Pre   ;//= 1 << 10;
    HI_U32 u32DcPre ;//= 4;//1:8bit; 4:10bit

    if(enLayer == OPTM_VDP_LAYER_WBC_HD0)
    {
        u32Pre   = 1 << 8;
        u32DcPre = 1;//1:8bit; 4:10bit
        if(enCscMode == VDP_CSC_RGB2YUV_601)
        {
            st_csc_coef.csc_coef00     = (HI_S32)(0.299  * u32Pre);
            st_csc_coef.csc_coef01     = (HI_S32)(0.587  * u32Pre);
            st_csc_coef.csc_coef02     = (HI_S32)(0.114  * u32Pre);

        st_csc_coef.csc_coef10     = (HI_S32)(-0.172 * u32Pre);
        st_csc_coef.csc_coef11     = (HI_S32)(-0.339 * u32Pre);
        st_csc_coef.csc_coef12     = (HI_S32)(0.511  * u32Pre);

        st_csc_coef.csc_coef20     = (HI_S32)(0.511  * u32Pre);
        st_csc_coef.csc_coef21     = (HI_S32)(-0.428 * u32Pre);
        st_csc_coef.csc_coef22     = (HI_S32)(-0.083 * u32Pre);

        st_csc_dc_coef.csc_in_dc2  = 0 * u32DcPre;
        st_csc_dc_coef.csc_in_dc1  = 0 * u32DcPre;
        st_csc_dc_coef.csc_in_dc0  = 0 * u32DcPre;

        st_csc_dc_coef.csc_out_dc2 =  16 * u32DcPre;
        st_csc_dc_coef.csc_out_dc1 = 128 * u32DcPre;
        st_csc_dc_coef.csc_out_dc0 = 128 * u32DcPre;
    }
    else if(enCscMode == VDP_CSC_YUV2RGB_601)
    {
        st_csc_coef.csc_coef00     = (HI_S32)(    1  * u32Pre);
        st_csc_coef.csc_coef01     = (HI_S32)(    0  * u32Pre);
        st_csc_coef.csc_coef02     = (HI_S32)(1.371  * u32Pre);

            st_csc_coef.csc_coef10     = (HI_S32)(     1 * u32Pre);
            st_csc_coef.csc_coef11     = (HI_S32)(-0.698 * u32Pre);
            st_csc_coef.csc_coef12     = (HI_S32)(-0.336 * u32Pre);

            st_csc_coef.csc_coef20     = (HI_S32)(    1  * u32Pre);
            st_csc_coef.csc_coef21     = (HI_S32)(1.732  * u32Pre);
            st_csc_coef.csc_coef22     = (HI_S32)(    0  * u32Pre);

            st_csc_dc_coef.csc_in_dc2  = -16  * u32DcPre;
            st_csc_dc_coef.csc_in_dc1  = -128 * u32DcPre;
            st_csc_dc_coef.csc_in_dc0  = -128 * u32DcPre;

        st_csc_dc_coef.csc_out_dc2 =  0 * u32DcPre;
        st_csc_dc_coef.csc_out_dc1 =  0 * u32DcPre;
        st_csc_dc_coef.csc_out_dc0 =  0 * u32DcPre;
    }
    else if(enCscMode == VDP_CSC_RGB2YUV_709)
    {
        st_csc_coef.csc_coef00     = (HI_S32)(0.213  * u32Pre);
        st_csc_coef.csc_coef01     = (HI_S32)(0.715  * u32Pre);
        st_csc_coef.csc_coef02     = (HI_S32)(0.072  * u32Pre);

        st_csc_coef.csc_coef10     = (HI_S32)(-0.117 * u32Pre);
        st_csc_coef.csc_coef11     = (HI_S32)(-0.394 * u32Pre);
        st_csc_coef.csc_coef12     = (HI_S32)( 0.511 * u32Pre);

        st_csc_coef.csc_coef20     = (HI_S32)( 0.511 * u32Pre);
        st_csc_coef.csc_coef21     = (HI_S32)(-0.464 * u32Pre);
        st_csc_coef.csc_coef22     = (HI_S32)(-0.047 * u32Pre);

        st_csc_dc_coef.csc_in_dc2  = 0 * u32DcPre;
        st_csc_dc_coef.csc_in_dc1  = 0 * u32DcPre;
        st_csc_dc_coef.csc_in_dc0  = 0 * u32DcPre;

        st_csc_dc_coef.csc_out_dc2 = 16  * u32DcPre;
        st_csc_dc_coef.csc_out_dc1 = 128 * u32DcPre;
        st_csc_dc_coef.csc_out_dc0 = 128 * u32DcPre;
    }
    else if(enCscMode == VDP_CSC_YUV2RGB_709)
    {
        st_csc_coef.csc_coef00     = (HI_S32)(    1  * u32Pre);
        st_csc_coef.csc_coef01     = (HI_S32)(    0  * u32Pre);
        st_csc_coef.csc_coef02     = (HI_S32)(1.540  * u32Pre);

        st_csc_coef.csc_coef10     = (HI_S32)(     1 * u32Pre);
        st_csc_coef.csc_coef11     = (HI_S32)(-0.183 * u32Pre);
        st_csc_coef.csc_coef12     = (HI_S32)(-0.459 * u32Pre);

        st_csc_coef.csc_coef20     = (HI_S32)(    1  * u32Pre);
        st_csc_coef.csc_coef21     = (HI_S32)(1.816  * u32Pre);
        st_csc_coef.csc_coef22     = (HI_S32)(    0  * u32Pre);

        st_csc_dc_coef.csc_in_dc2  = -16  * u32DcPre;
        st_csc_dc_coef.csc_in_dc1  = -128 * u32DcPre;
        st_csc_dc_coef.csc_in_dc0  = -128 * u32DcPre;

        st_csc_dc_coef.csc_out_dc2 = 0 * u32DcPre;
        st_csc_dc_coef.csc_out_dc1 = 0 * u32DcPre;
        st_csc_dc_coef.csc_out_dc0 = 0 * u32DcPre;
    }
    else if(enCscMode == VDP_CSC_YUV2YUV_709_601)
    {
        st_csc_coef.csc_coef00     = (HI_S32)(     1 * u32Pre);
        st_csc_coef.csc_coef01     = (HI_S32)(-0.116 * u32Pre);
        st_csc_coef.csc_coef02     = (HI_S32)( 0.208 * u32Pre);

        st_csc_coef.csc_coef10     = (HI_S32)(     0 * u32Pre);
        st_csc_coef.csc_coef11     = (HI_S32)( 1.017 * u32Pre);
        st_csc_coef.csc_coef12     = (HI_S32)( 0.114 * u32Pre);

        st_csc_coef.csc_coef20     = (HI_S32)(     0 * u32Pre);
        st_csc_coef.csc_coef21     = (HI_S32)( 0.075 * u32Pre);
        st_csc_coef.csc_coef22     = (HI_S32)( 1.025 * u32Pre);

        st_csc_dc_coef.csc_in_dc2  = -16  * u32DcPre;
        st_csc_dc_coef.csc_in_dc1  = -128 * u32DcPre;
        st_csc_dc_coef.csc_in_dc0  = -128 * u32DcPre;

        st_csc_dc_coef.csc_out_dc2 =   16 * u32DcPre;
        st_csc_dc_coef.csc_out_dc1 =  128 * u32DcPre;
        st_csc_dc_coef.csc_out_dc0 =  128 * u32DcPre;
    }
    else if(enCscMode == VDP_CSC_YUV2YUV_601_709)
    {
        st_csc_coef.csc_coef00     = (HI_S32)(     1 * u32Pre);
        st_csc_coef.csc_coef01     = (HI_S32)(-0.116 * u32Pre);
        st_csc_coef.csc_coef02     = (HI_S32)( 0.208 * u32Pre);

            st_csc_coef.csc_coef10     = (HI_S32)(     0 * u32Pre);
            st_csc_coef.csc_coef11     = (HI_S32)( 1.017 * u32Pre);
            st_csc_coef.csc_coef12     = (HI_S32)( 0.114 * u32Pre);

            st_csc_coef.csc_coef20     = (HI_S32)(     0 * u32Pre);
            st_csc_coef.csc_coef21     = (HI_S32)( 0.075 * u32Pre);
            st_csc_coef.csc_coef22     = (HI_S32)( 1.025 * u32Pre);

            st_csc_dc_coef.csc_in_dc2  = -16  * u32DcPre;
            st_csc_dc_coef.csc_in_dc1  = -128 * u32DcPre;
            st_csc_dc_coef.csc_in_dc0  = -128 * u32DcPre;

            st_csc_dc_coef.csc_out_dc2 =   16 * u32DcPre;
            st_csc_dc_coef.csc_out_dc1 =  128 * u32DcPre;
            st_csc_dc_coef.csc_out_dc0 =  128 * u32DcPre;
        }
        OPTM_VDP_WBC_SetCscCoef  ( enLayer,st_csc_coef);
        OPTM_VDP_WBC_SetCscDcCoef( enLayer,st_csc_dc_coef);
    } 
    else if(enLayer == OPTM_VDP_LAYER_WBC_GP0)
    {
        u32Pre   = 1 << 10;
        u32DcPre = 4;//1:8bit; 4:10bit
        if(enCscMode == VDP_CSC_RGB2YUV_601)
        {
            st_csc_coef.csc_coef00     = (HI_S32)(0.299  * u32Pre);
            st_csc_coef.csc_coef01     = (HI_S32)(0.587  * u32Pre);
            st_csc_coef.csc_coef02     = (HI_S32)(0.114  * u32Pre);

            st_csc_coef.csc_coef10     = (HI_S32)(-0.172 * u32Pre);
            st_csc_coef.csc_coef11     = (HI_S32)(-0.339 * u32Pre);
            st_csc_coef.csc_coef12     = (HI_S32)(0.511  * u32Pre);

            st_csc_coef.csc_coef20     = (HI_S32)(0.511  * u32Pre);
            st_csc_coef.csc_coef21     = (HI_S32)(-0.428 * u32Pre);
            st_csc_coef.csc_coef22     = (HI_S32)(-0.083 * u32Pre);

            st_csc_dc_coef.csc_in_dc2  = 0 * u32DcPre;
            st_csc_dc_coef.csc_in_dc1  = 0 * u32DcPre;
            st_csc_dc_coef.csc_in_dc0  = 0 * u32DcPre;

            st_csc_dc_coef.csc_out_dc2 =  16 * u32DcPre;
            st_csc_dc_coef.csc_out_dc1 = 128 * u32DcPre;
            st_csc_dc_coef.csc_out_dc0 = 128 * u32DcPre;
        }
        else if(enCscMode == VDP_CSC_YUV2RGB_601)
        {
            st_csc_coef.csc_coef00     = (HI_S32)(    1  * u32Pre);
            st_csc_coef.csc_coef01     = (HI_S32)(    0  * u32Pre);
            st_csc_coef.csc_coef02     = (HI_S32)(1.371  * u32Pre);

            st_csc_coef.csc_coef10     = (HI_S32)(     1 * u32Pre);
            st_csc_coef.csc_coef11     = (HI_S32)(-0.698 * u32Pre);
            st_csc_coef.csc_coef12     = (HI_S32)(-0.336 * u32Pre);

            st_csc_coef.csc_coef20     = (HI_S32)(    1  * u32Pre);
            st_csc_coef.csc_coef21     = (HI_S32)(1.732  * u32Pre);
            st_csc_coef.csc_coef22     = (HI_S32)(    0  * u32Pre);

            st_csc_dc_coef.csc_in_dc2  = -16  * u32DcPre;
            st_csc_dc_coef.csc_in_dc1  = -128 * u32DcPre;
            st_csc_dc_coef.csc_in_dc0  = -128 * u32DcPre;

            st_csc_dc_coef.csc_out_dc2 =  0 * u32DcPre;
            st_csc_dc_coef.csc_out_dc1 =  0 * u32DcPre;
            st_csc_dc_coef.csc_out_dc0 =  0 * u32DcPre;
        }
        else if(enCscMode == VDP_CSC_RGB2YUV_709)
        {
            st_csc_coef.csc_coef00     = (HI_S32)(0.213  * u32Pre);
            st_csc_coef.csc_coef01     = (HI_S32)(0.715  * u32Pre);
            st_csc_coef.csc_coef02     = (HI_S32)(0.072  * u32Pre);

            st_csc_coef.csc_coef10     = (HI_S32)(-0.117 * u32Pre);
            st_csc_coef.csc_coef11     = (HI_S32)(-0.394 * u32Pre);
            st_csc_coef.csc_coef12     = (HI_S32)( 0.511 * u32Pre);

            st_csc_coef.csc_coef20     = (HI_S32)( 0.511 * u32Pre);
            st_csc_coef.csc_coef21     = (HI_S32)(-0.464 * u32Pre);
            st_csc_coef.csc_coef22     = (HI_S32)(-0.047 * u32Pre);

            st_csc_dc_coef.csc_in_dc2  = 0 * u32DcPre;
            st_csc_dc_coef.csc_in_dc1  = 0 * u32DcPre;
            st_csc_dc_coef.csc_in_dc0  = 0 * u32DcPre;

            st_csc_dc_coef.csc_out_dc2 = 16  * u32DcPre;
            st_csc_dc_coef.csc_out_dc1 = 128 * u32DcPre;
            st_csc_dc_coef.csc_out_dc0 = 128 * u32DcPre;
        }
        else if(enCscMode == VDP_CSC_YUV2RGB_709)
        {
            st_csc_coef.csc_coef00     = (HI_S32)(    1  * u32Pre);
            st_csc_coef.csc_coef01     = (HI_S32)(    0  * u32Pre);
            st_csc_coef.csc_coef02     = (HI_S32)(1.540  * u32Pre);

            st_csc_coef.csc_coef10     = (HI_S32)(     1 * u32Pre);
            st_csc_coef.csc_coef11     = (HI_S32)(-0.183 * u32Pre);
            st_csc_coef.csc_coef12     = (HI_S32)(-0.459 * u32Pre);

            st_csc_coef.csc_coef20     = (HI_S32)(    1  * u32Pre);
            st_csc_coef.csc_coef21     = (HI_S32)(1.816  * u32Pre);
            st_csc_coef.csc_coef22     = (HI_S32)(    0  * u32Pre);

            st_csc_dc_coef.csc_in_dc2  = -16  * u32DcPre;
            st_csc_dc_coef.csc_in_dc1  = -128 * u32DcPre;
            st_csc_dc_coef.csc_in_dc0  = -128 * u32DcPre;

            st_csc_dc_coef.csc_out_dc2 = 0 * u32DcPre;
            st_csc_dc_coef.csc_out_dc1 = 0 * u32DcPre;
            st_csc_dc_coef.csc_out_dc0 = 0 * u32DcPre;
        }
        else if(enCscMode == VDP_CSC_YUV2YUV_709_601)
        {
            st_csc_coef.csc_coef00     = (HI_S32)(     1 * u32Pre);
            st_csc_coef.csc_coef01     = (HI_S32)(-0.116 * u32Pre);
            st_csc_coef.csc_coef02     = (HI_S32)( 0.208 * u32Pre);

            st_csc_coef.csc_coef10     = (HI_S32)(     0 * u32Pre);
            st_csc_coef.csc_coef11     = (HI_S32)( 1.017 * u32Pre);
            st_csc_coef.csc_coef12     = (HI_S32)( 0.114 * u32Pre);

            st_csc_coef.csc_coef20     = (HI_S32)(     0 * u32Pre);
            st_csc_coef.csc_coef21     = (HI_S32)( 0.075 * u32Pre);
            st_csc_coef.csc_coef22     = (HI_S32)( 1.025 * u32Pre);

            st_csc_dc_coef.csc_in_dc2  = -16  * u32DcPre;
            st_csc_dc_coef.csc_in_dc1  = -128 * u32DcPre;
            st_csc_dc_coef.csc_in_dc0  = -128 * u32DcPre;

            st_csc_dc_coef.csc_out_dc2 =   16 * u32DcPre;
            st_csc_dc_coef.csc_out_dc1 =  128 * u32DcPre;
            st_csc_dc_coef.csc_out_dc0 =  128 * u32DcPre;
        }
        else if(enCscMode == VDP_CSC_YUV2YUV_601_709)
        {
            st_csc_coef.csc_coef00     = (HI_S32)(     1 * u32Pre);
            st_csc_coef.csc_coef01     = (HI_S32)(-0.116 * u32Pre);
            st_csc_coef.csc_coef02     = (HI_S32)( 0.208 * u32Pre);

            st_csc_coef.csc_coef10     = (HI_S32)(     0 * u32Pre);
            st_csc_coef.csc_coef11     = (HI_S32)( 1.017 * u32Pre);
            st_csc_coef.csc_coef12     = (HI_S32)( 0.114 * u32Pre);

            st_csc_coef.csc_coef20     = (HI_S32)(     0 * u32Pre);
            st_csc_coef.csc_coef21     = (HI_S32)( 0.075 * u32Pre);
            st_csc_coef.csc_coef22     = (HI_S32)( 1.025 * u32Pre);

            st_csc_dc_coef.csc_in_dc2  = -16  * u32DcPre;
            st_csc_dc_coef.csc_in_dc1  = -128 * u32DcPre;
            st_csc_dc_coef.csc_in_dc0  = -128 * u32DcPre;

            st_csc_dc_coef.csc_out_dc2 =   16 * u32DcPre;
            st_csc_dc_coef.csc_out_dc1 =  128 * u32DcPre;
            st_csc_dc_coef.csc_out_dc0 =  128 * u32DcPre;
        }
        else
        {
            st_csc_coef.csc_coef00     = 1 * u32Pre;
            st_csc_coef.csc_coef01     = 0 * u32Pre;
            st_csc_coef.csc_coef02     = 0 * u32Pre;

            st_csc_coef.csc_coef10     = 0 * u32Pre;
            st_csc_coef.csc_coef11     = 1 * u32Pre;
            st_csc_coef.csc_coef12     = 0 * u32Pre;

            st_csc_coef.csc_coef20     = 0 * u32Pre;
            st_csc_coef.csc_coef21     = 0 * u32Pre;
            st_csc_coef.csc_coef22     = 1 * u32Pre;

            st_csc_dc_coef.csc_in_dc2  = -16  * u32DcPre;
            st_csc_dc_coef.csc_in_dc1  = -128 * u32DcPre;
            st_csc_dc_coef.csc_in_dc0  = -128 * u32DcPre;

            st_csc_dc_coef.csc_out_dc2 =  16 * u32DcPre;
            st_csc_dc_coef.csc_out_dc1 = 128 * u32DcPre;
            st_csc_dc_coef.csc_out_dc0 = 128 * u32DcPre;
        }
        OPTM_VDP_GP_SetCscCoef  (OPTM_WBC_GP0_SEL,st_csc_coef);
        OPTM_VDP_GP_SetCscDcCoef(OPTM_WBC_GP0_SEL,st_csc_dc_coef);
    }
#endif

    return ;
}

int OPTM_VDP_DISP_GetIntSignal(int a)
{
#warning TODO: OPTM_VDP_DISP_GetIntSignal
	printk("OPTM_VDP_DISP_GetIntSignal: TODO\n");
}


void OPTM_ClearIntSignal(int a)
{
#warning TODO: OPTM_ClearIntSignal
	printk("OPTM_ClearIntSignal: TODO\n");
}

OPTM_VDP_DISP_MODE_E OPTM_DISP_GetDispMode(OPTM_VDP_LAYER_GP_E enGpHalId)
{
#warning TODO: OPTM_DISP_GetDispMode
	printk("OPTM_DISP_GetDispMode: TODO\n");
}



//-------------------------------------------------------------------
//WBC_DHD0_END



