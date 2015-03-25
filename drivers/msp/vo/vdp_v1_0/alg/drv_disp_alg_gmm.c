
/******************************************************************************
  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.
******************************************************************************
File Name     : drv_disp_da.h
Version       : Initial Draft
Author        : Hisilicon multimedia software group
Created       : 2012/12/30
Last Modified :
Description   :
Function List :
History       :
******************************************************************************/

#include "drv_disp_com.h"
#include "drv_disp_da.h"

#ifdef __cplusplus
 #if __cplusplus
extern "C" {
 #endif
#endif /* __cplusplus */


typedef struct
{
	HI_U32 Data_0; //0
	int fill_4[1248]; //4
	//4996
} Struct_8113d708;


Struct_8113d708 Data_8113d708; //8113d708 -> 8113ea8c
Struct_8113ea8c Data_8113ea8c; //8113ea8c -> 811402a4
Struct_8113ea8c Data_811402a4; //811402a4 -> 81141abc


HI_VOID ALG_GmmThdSet(Struct_81141acc* a, int b, int c)
{
#warning TODO: ALG_GmmThdSet

	printk("ALG_GmmThdSet: TODO\n");
	printk("ALG_GmmThdSet: TODO\n");
	printk("ALG_GmmThdSet: TODO\n");
	printk("ALG_GmmThdSet: TODO\n");
	printk("ALG_GmmThdSet: TODO\n");
	printk("ALG_GmmThdSet: TODO\n");
	printk("ALG_GmmThdSet: TODO\n");
	printk("ALG_GmmThdSet: TODO\n");
	printk("ALG_GmmThdSet: TODO\n");
	printk("ALG_GmmThdSet: TODO\n");
	printk("ALG_GmmThdSet: TODO\n");
	printk("ALG_GmmThdSet: TODO\n");
	printk("ALG_GmmThdSet: TODO\n");
	printk("ALG_GmmThdSet: TODO\n");
	printk("ALG_GmmThdSet: TODO\n");
	printk("ALG_GmmThdSet: TODO\n");
}


HI_VOID UpdateGmmCoef(Struct_81141acc* a, Struct_8113ea8c* b, HI_U32 c)
{
#warning TODO: UpdateGmmCoef

	printk("UpdateGmmCoef: TODO\n");
	printk("UpdateGmmCoef: TODO\n");
	printk("UpdateGmmCoef: TODO\n");
	printk("UpdateGmmCoef: TODO\n");
	printk("UpdateGmmCoef: TODO\n");
	printk("UpdateGmmCoef: TODO\n");
	printk("UpdateGmmCoef: TODO\n");
	printk("UpdateGmmCoef: TODO\n");
	printk("UpdateGmmCoef: TODO\n");
	printk("UpdateGmmCoef: TODO\n");
	printk("UpdateGmmCoef: TODO\n");
	printk("UpdateGmmCoef: TODO\n");
	printk("UpdateGmmCoef: TODO\n");
	printk("UpdateGmmCoef: TODO\n");
	printk("UpdateGmmCoef: TODO\n");
	printk("UpdateGmmCoef: TODO\n");
}

HI_S32 ALG_GmmInit(Struct_81141acc* a)
{
	memset(&Data_8113d708, 0, sizeof(Struct_8113d708));

	Data_8113d708.Data_0 = 4992;
	a->Data_24 = a->Data_0.u32StartPhyAddr;
	a->Data_28 = a->Data_24 + Data_8113d708.Data_0/4;
	a->Data_32 = a->Data_24 + ((Data_8113d708.Data_0 & 0x7FFFFFFF) >> 1);
	a->Data_36 = a->Data_24 + ((Data_8113d708.Data_0*3)/4);

	UpdateGmmCoef(a, &Data_8113ea8c, 0);

	a->Data_40 = a->Data_12.u32StartPhyAddr;
	a->Data_44 = a->Data_40 + Data_8113d708.Data_0/4;
	a->Data_48 = a->Data_40 + ((Data_8113d708.Data_0 & 0x7FFFFFFF) >> 1);
	a->Data_52 = a->Data_40 + ((Data_8113d708.Data_0*3)/4);

	UpdateGmmCoef(a, &Data_811402a4, 1);

	return HI_SUCCESS;
}

HI_S32 ALG_GmmDeInit(Struct_81141acc* a)
{
	if (a->Data_0.u32StartVirAddr != 0)
	{
		a->Data_0.u32StartVirAddr = 0;
	}

	if (a->Data_12.u32StartVirAddr != 0)
	{
		a->Data_12.u32StartVirAddr = 0;
	}

	return HI_SUCCESS;
}


#ifdef __cplusplus
 #if __cplusplus
}
 #endif
#endif /* __cplusplus */









