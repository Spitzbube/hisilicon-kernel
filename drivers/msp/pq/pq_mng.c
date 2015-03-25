
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/vmalloc.h>
#include <asm/memory.h>
#include <linux/bootmem.h>
#include <linux/gfp.h>
#include <linux/mm.h>
#include <linux/memblock.h>

#include "hi_drv_module.h"
#include "drv_pdm_ext.h"
#include "drv_pq_ext.h"
#include "drv_vpss_ext.h"
#include "drv_win_ext.h"
#include "drv_disp_ext.h"
#include "hi_flash.h"
#include "hi_drv_mem.h"
#include "hi_drv_mmz.h"
#include "drv_pq.h"
#include "hi_osal.h"


typedef struct
{
	PQ_MNG_GetPictureSettingStruct1 Data_0; //0
	PQ_MNG_GetPictureSettingStruct2 Data_8; //8
	PQ_MNG_GetPictureSettingStruct1 Data_20; //20

} Struct_80faaa8c;

Struct_80faaa8c Data_80faaa8c = //80faaa8c
{
		{128,128,128,128},
		{128,128,128,128,128,128},
		{128,128,128,128},
#warning TODO: further initializers
};


HI_S32 PQ_MNG_GetSDPictureSetting(PQ_MNG_GetPictureSettingStruct1* a, PQ_MNG_GetPictureSettingStruct2* b)
{
	a->wData_0 = Data_80faaa8c.Data_0.wData_0;
	a->wData_2 = Data_80faaa8c.Data_0.wData_2;
	a->wData_4 = Data_80faaa8c.Data_0.wData_4;
	a->wData_6 = Data_80faaa8c.Data_0.wData_6;

	b->wData_0 = Data_80faaa8c.Data_8.wData_0;
	b->wData_2 = Data_80faaa8c.Data_8.wData_2;
	b->wData_4 = Data_80faaa8c.Data_8.wData_4;
	b->wData_6 = Data_80faaa8c.Data_8.wData_6;
	b->wData_8 = Data_80faaa8c.Data_8.wData_8;
	b->wData_10 = Data_80faaa8c.Data_8.wData_10;

	return HI_SUCCESS;
}

HI_S32 PQ_MNG_GetHDPictureSetting(PQ_MNG_GetPictureSettingStruct1* a, PQ_MNG_GetPictureSettingStruct2* b)
{
	a->wData_0 = Data_80faaa8c.Data_20.wData_0;
	a->wData_2 = Data_80faaa8c.Data_20.wData_2;
	a->wData_4 = Data_80faaa8c.Data_20.wData_4;
	a->wData_6 = Data_80faaa8c.Data_20.wData_6;

	b->wData_0 = Data_80faaa8c.Data_8.wData_0;
	b->wData_2 = Data_80faaa8c.Data_8.wData_2;
	b->wData_4 = Data_80faaa8c.Data_8.wData_4;
	b->wData_6 = Data_80faaa8c.Data_8.wData_6;
	b->wData_8 = Data_80faaa8c.Data_8.wData_8;
	b->wData_10 = Data_80faaa8c.Data_8.wData_10;

	return HI_SUCCESS;
}


HI_S32 PQ_MNG_SetSDPictureSetting(PQ_MNG_GetPictureSettingStruct1* a, PQ_MNG_GetPictureSettingStruct2* b)
{
#warning TODO: PQ_MNG_SetSDPictureSetting

	printk("PQ_MNG_SetSDPictureSetting: TODO\n");

}


HI_S32 PQ_MNG_SetHDPictureSetting(PQ_MNG_GetPictureSettingStruct1* a, PQ_MNG_GetPictureSettingStruct2* b)
{
#warning TODO: PQ_MNG_SetHDPictureSetting

	printk("PQ_MNG_SetHDPictureSetting: TODO\n");

}


HI_U32 PQ_MNG_CheckSourceMode(int a, int b)
{
#warning TODO: PQ_MNG_CheckSourceMode

	printk("PQ_MNG_CheckSourceMode: TODO\n");
}


HI_U32 PQ_MNG_CheckOutputMode(int a)
{
#warning TODO: PQ_MNG_CheckOutputMode

	printk("PQ_MNG_CheckOutputMode: TODO\n");

}


HI_S32 PQ_MNG_LoadPhyList(int a, int b, int c, int d)
{
#warning TODO: PQ_MNG_LoadPhyList

	printk("PQ_MNG_LoadPhyList: TODO\n");
}


HI_VOID PQ_MNG_EnableSharpen(HI_U8 a)
{
#warning TODO: PQ_MNG_EnableSharpen

	printk("PQ_MNG_EnableSharpen: TODO\n");

}

HI_VOID PQ_MNG_EnableDCI(HI_U32 a)
{
#warning TODO: PQ_MNG_EnableDCI

	printk("PQ_MNG_EnableDCI: TODO\n");

}


HI_VOID PQ_MNG_EnableColorEnhance(HI_U32 a)
{
#warning TODO: PQ_MNG_EnableColorEnhance

	printk("PQ_MNG_EnableColorEnhance: TODO\n");

}


HI_VOID DRV_PQ_SetDemoMode(int a, int b)
{
#warning TODO: DRV_PQ_SetDemoMode

	printk("DRV_PQ_SetDemoMode: TODO\n");

}


HI_VOID PQ_MNG_EnableSharpenDemo(int a)
{
#warning TODO: PQ_MNG_EnableSharpenDemo

	printk("PQ_MNG_EnableSharpenDemo: TODO\n");

}


HI_VOID PQ_MNG_ColorDemoMode(int a)
{
#warning TODO: PQ_MNG_ColorDemoMode

	printk("PQ_MNG_ColorDemoMode: TODO\n");

}

HI_VOID PQ_MNG_DCIDemoMode(int a)
{
#warning TODO: PQ_MNG_DCIDemoMode

	printk("PQ_MNG_DCIDemoMode: TODO\n");

}


HI_S32 PQ_MNG_SetDCILevelGain(int a, int b)
{
#warning TODO: PQ_MNG_SetDCILevelGain

	printk("PQ_MNG_SetDCILevelGain: TODO\n");

}


HI_S32 PQ_MNG_SetFleshToneLevel(int a, int b)
{
#warning TODO: PQ_MNG_SetFleshToneLevel

	printk("PQ_MNG_SetFleshToneLevel: TODO\n");

}

HI_S32 PQ_MNG_SetSixBaseColorLevel(int* a, int b)
{
#warning TODO: PQ_MNG_SetSixBaseColorLevel

	printk("PQ_MNG_SetSixBaseColorLevel: TODO\n");

}


HI_S32 PQ_MNG_SetColorEnhanceMode(int a, int b)
{
#warning TODO: PQ_MNG_SetColorEnhanceMode

	printk("PQ_MNG_SetColorEnhanceMode: TODO\n");

}





