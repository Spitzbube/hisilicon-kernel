
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


typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    Bit0            : 1  ; // [0..0]
        unsigned int    Bit1            : 1  ; // [1..1]
        unsigned int    Bit2            : 1  ; // [2..2]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_0x601c;

typedef struct
{
	int fill_0[6148]; //0
	int Data_0x6010; //0x6010
	int Data_0x6014; //0x6014
	int Data_0x6018; //0x6018
	U_DATE_0x601c Data_0x601c; //0x601c
	int fill_0x6020[56]; //0x6020
	int Data_0x6100; //0x6100
	int fill_0x6104[39]; //0x6104
	int Data_0x61a0; //0x61a0
	int Data_0x61a4; //0x61a4
	int fill_0x61a8[22]; //0x61a8
	int Data_0x6200; //0x6200
	int fill_0x6204[6239]; //0x6204
	int Data_0xc380; //0xc380
	//0xd6ac???
} Struct_8110b108;

typedef struct
{
	int fill_0[4140]; //0
	//0x40b0
} Struct_8110b178;


typedef struct
{
	int fill_0[3]; //0
	//12
} Struct_8110b114;

volatile Struct_8110b108* Data_8110b108 = HI_NULL; //8110b108
volatile Struct_8110b108* Data_8110b10c; //8110b10c +4
Struct_8110b114* Data_8110b114; //8110b114 +12

Struct_8110b178* Data_8110b178; //8110b178 +112
HI_U32 Data_8110b17c; //8110b17c +116








HI_S32 PQ_HAL_Init(HI_VOID)
{
	if (Data_8110b17c == 1)
	{
		return HI_SUCCESS;
	}

	Data_8110b178 = (Struct_8110b178*) kmalloc_large(sizeof(Struct_8110b178), GFP_KERNEL);
	if (Data_8110b178 == HI_NULL)
	{
		HI_ERR_PQ("sg_pstVPSSRegLocal can not kmalloc!\n"); //95
		return HI_FAILURE;
	}
	memset(Data_8110b178, 0, sizeof(Struct_8110b178));

	Data_8110b10c = (Struct_8110b108*) kmalloc_large(0xd6ac, GFP_KERNEL);
	if (Data_8110b10c == HI_NULL)
	{
		HI_ERR_PQ("sg_pstVDPRegLocal can not kmalloc!\n"); //103
		return HI_FAILURE;
	}
	memset(Data_8110b10c, 0, 0xd6ac);

	Data_8110b114 = (Struct_8110b114*) kmalloc(sizeof(Struct_8110b114), GFP_KERNEL);
	if (Data_8110b114 == HI_NULL)
	{
		HI_ERR_PQ("sg_pstPQModuleLocal can not kmalloc!\n"); //111
		return HI_FAILURE;
	}

	memset(Data_8110b114, 0, sizeof(Struct_8110b114));

	Data_8110b17c = 1;

	return HI_SUCCESS;
}


volatile Struct_8110b108* PQ_HAL_GetVdpReg(HI_VOID)
{
	if (Data_8110b108) return Data_8110b108;
	else if (Data_8110b10c) return Data_8110b10c;
	else return HI_NULL;
}


HI_VOID PQ_HAL_UpdateVdpPQ(volatile Struct_8110b108* a, volatile Struct_8110b108* b)
{
	U_DATE_0x601c Data_0x601c;

    memcpy((HI_VOID *)(&(a->Data_0x6200)), (HI_VOID *)(&(b->Data_0x6200)), 0x17c);

    a->Data_0x6018 = b->Data_0x6018;
    a->Data_0x601c.bits.Bit2 = b->Data_0x601c.bits.Bit2;

    memcpy((HI_VOID *)(&(a->Data_0x61a0)), (HI_VOID *)(&(b->Data_0x61a0)), 8);

    a->Data_0x6014 = b->Data_0x6014;
    a->Data_0x601c.bits.Bit0 = b->Data_0x601c.bits.Bit0;

    memcpy((HI_VOID *)(&(a->Data_0x6100)), (HI_VOID *)(&(b->Data_0x6100)), 0x94);

    a->Data_0x6010 = b->Data_0x6010;
    a->Data_0x601c.bits.Bit1 = b->Data_0x601c.bits.Bit1;

    memcpy((HI_VOID *)(&(a->Data_0xc380)), (HI_VOID *)(&(b->Data_0xc380)), 0x20);
}


HI_S32 PQ_HAL_UpdateVdpReg(HI_U32 addr)
{
	if (Data_8110b108 != (volatile Struct_8110b108*)(addr))
	{
		Data_8110b108 = (volatile Struct_8110b108*)(addr);

		if (Data_8110b108 != HI_NULL)
		{
			PQ_HAL_UpdateVdpPQ(Data_8110b108, Data_8110b10c);
		}
	}

	return HI_SUCCESS;
}


HI_VOID PQ_HAL_UpdateDCICoef(int a)
{
#warning TODO: PQ_HAL_UpdateDCICoef

	printk("PQ_HAL_UpdateDCICoef: TODO\n");

}


HI_VOID PQ_HAL_UpdatACMCoef(int a)
{
#warning TODO: PQ_HAL_UpdatACMCoef

	printk("PQ_HAL_UpdatACMCoef: TODO\n");

}


HI_VOID PQ_HAL_UpdateSharpCofe(int a)
{
#warning TODO: PQ_HAL_UpdateSharpCofe

	printk("PQ_HAL_UpdateSharpCofe: TODO\n");

}



