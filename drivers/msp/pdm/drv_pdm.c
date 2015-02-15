#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/vmalloc.h>
#include <asm/memory.h>
#include <linux/bootmem.h>
#include <linux/gfp.h>
#include <linux/mm.h>
#include <linux/memblock.h>

#include "hi_drv_pdm.h"
#include "hi_db.h"
#include "drv_pdm_ext.h"
#include "drv_pdm.h"

extern int pdm_free_reserve_mem(HI_U32 PhyAddr, HI_U32 Len);

extern int get_param_data(const char *name, char *buf, unsigned int buflen);
extern int set_param_data(const char *name, char *buf, int buflen);

#define DRV_PDM_LOCK(pMutex)    \
    do{ \
        if(down_interruptible(pMutex)) \
        {       \
            HI_ERR_PDM("ERR: pdm lock error!\n");    \
        }   \
    }while(0)

#define DRV_PDM_UNLOCK(pMutex)    \
    do{ \
        up(pMutex); \
    }while(0)


extern PDM_GLOBAL_S        g_PdmGlobal;

int Data_8110aa1c; //8110aa1c

#if 0
static HI_UNF_DISP_TIMING_S   g_stDispTiming = 
{
    .VFB = 27,
    .VBB = 23,
    .VACT = 768,
    .HFB = 210, 
    .HBB = 46,  
    .HACT = 1366,
    .VPW = 4,    
    .HPW = 24,   
    .IDV = 0,    
    .IHS = 0,    
    .IVS = 0,    
    .ClockReversal = HI_FALSE,
    .DataWidth = HI_UNF_DISP_INTF_DATA_WIDTH24,
    .ItfFormat = HI_UNF_DISP_INTF_DATA_FMT_RGB888,
    .DitherEnable = HI_FALSE,                    
    .ClkPara0 = 0x912ccccc,       
    .ClkPara1 = 0x006d8157,       
    //.InRectWidth = 1366,
    //.InRectHeight = 768,
};
#endif

HI_CHAR *PDM_VmapByPhyaddr(HI_U32 phy_addr, HI_U32 size)
{
    HI_U32 i;
    HI_CHAR *vaddr = NULL;
    struct page **page_array = NULL;
    HI_U32 page_array_size = (size + PAGE_SIZE - 1)/PAGE_SIZE;

    page_array = vmalloc(page_array_size * sizeof(struct page *));
    if (!page_array) {
        HI_ERR_PDM("fail to vmalloc %u bytes\n", page_array_size * sizeof(struct page *)); //68
        goto error_out;
    }

    for (i=0; i < page_array_size; i++) {
        page_array[i] = phys_to_page(phy_addr + i*PAGE_SIZE);
    }

    if (i != page_array_size) {
        HI_ERR_PDM("incorrect page array for vmap, \
                array size is %u, but %u filled\n", page_array_size, i);
        goto error_out;
    }

    vaddr = (unsigned char *)vmap(page_array, page_array_size, VM_MAP, PAGE_KERNEL);
    if (!vaddr) {
        HI_ERR_PDM("fail to vmap %lu at 0x%lX\n", phy_addr, size); //84
        goto error_out;
    }

    if (page_array)
        vfree(page_array);

    return vaddr;

error_out:
    if (page_array)
        vfree(page_array);
	
    return NULL;
}

HI_VOID PDM_GetDefDispParam(HI_UNF_DISP_E enDisp, HI_DISP_PARAM_S *pstDispParam)
{
    HI_S32  i;
    
    for (i=0; i<HI_UNF_DISP_INTF_TYPE_BUTT; i++)
    {
        pstDispParam->stIntf[i].enIntfType = HI_UNF_DISP_INTF_TYPE_BUTT;
    }
	
    return;
}

HI_S32 PDM_GetBufByName(const HI_CHAR *BufName, HI_U32 *pu32BasePhyAddr, HI_U32 *pu32Len)
{
    HI_S32   i;
    
    for (i = 0; i < g_PdmGlobal.u32BufNum; i++)
    {
        if (0 == strncmp(g_PdmGlobal.stBufInfo[i].as8BufName, BufName, strlen(BufName)))
        {
            break;    
        }
    }

    if (i >= g_PdmGlobal.u32BufNum)
    {
        return HI_FAILURE;
    }

    *pu32BasePhyAddr = g_PdmGlobal.stBufInfo[i].u32PhyAddr;

    *pu32Len = g_PdmGlobal.stBufInfo[i].u32Lenth;
    
    return HI_SUCCESS;
}

HI_S32 PDM_SetVirAddrByName(const HI_CHAR *BufName, HI_U32 u32VirAddr)
{	
	HI_U32                  i;    

	DRV_PDM_LOCK(&g_PdmGlobal.PdmMutex); //141
	
	for (i = 0; i < g_PdmGlobal.u32BufNum; i++)
    {
        if (0 == strncmp(g_PdmGlobal.stBufInfo[i].as8BufName, BufName, strlen(BufName)))
        {
        	break;
        }
    }

	if (i >= g_PdmGlobal.u32BufNum)
	{
		DRV_PDM_UNLOCK(&g_PdmGlobal.PdmMutex);
		HI_INFO_PDM("can not find buffer:%s\n", BufName); //154
		return HI_FAILURE;
	}

	g_PdmGlobal.stBufInfo[i].u32VirAddr = u32VirAddr;
	DRV_PDM_UNLOCK(&g_PdmGlobal.PdmMutex);
	return HI_SUCCESS;
}

HI_S32 PDM_GetVirAddrByName(const HI_CHAR *BufName, HI_U32 *pu32VirAddr, HI_U32 *pu32Len)
{	
	HI_U32                  i;    

	DRV_PDM_LOCK(&g_PdmGlobal.PdmMutex); //167
	for (i = 0; i < g_PdmGlobal.u32BufNum; i++)
    {
        if (0 == strncmp(g_PdmGlobal.stBufInfo[i].as8BufName, BufName, strlen(BufName)))
        {
        	break;
        }
    }

	if (i >= g_PdmGlobal.u32BufNum)
	{
		DRV_PDM_UNLOCK(&g_PdmGlobal.PdmMutex);
		HI_INFO_PDM("can not find buffer:%s\n", BufName); //179
		return HI_FAILURE;
	}

	*pu32VirAddr = g_PdmGlobal.stBufInfo[i].u32VirAddr;
	*pu32Len = g_PdmGlobal.stBufInfo[i].u32Lenth;
	DRV_PDM_UNLOCK(&g_PdmGlobal.PdmMutex);
	return HI_SUCCESS;
}

HI_VOID PDM_TransFomat(HI_UNF_ENC_FMT_E enSrcFmt, HI_UNF_ENC_FMT_E *penHdFmt, HI_UNF_ENC_FMT_E *penSdFmt)
{
    switch(enSrcFmt)
    {
        /* bellow are tv display formats */
        case HI_UNF_ENC_FMT_1080P_60:
        {
            *penHdFmt = HI_UNF_ENC_FMT_1080P_60;
            *penSdFmt = HI_UNF_ENC_FMT_NTSC;
            break;
        }
        case HI_UNF_ENC_FMT_1080P_50:
        {
            *penHdFmt = HI_UNF_ENC_FMT_1080P_50;
            *penSdFmt = HI_UNF_ENC_FMT_PAL; 
            break;
        }
        case HI_UNF_ENC_FMT_1080i_60:
        {
            *penHdFmt = HI_UNF_ENC_FMT_1080i_60;
            *penSdFmt = HI_UNF_ENC_FMT_NTSC;
            break;
        }
        case HI_UNF_ENC_FMT_1080i_50:
        {
            *penHdFmt = HI_UNF_ENC_FMT_1080i_50;
            *penSdFmt = HI_UNF_ENC_FMT_PAL; 
            break;
        }
        case HI_UNF_ENC_FMT_720P_60:
        {
            *penHdFmt = HI_UNF_ENC_FMT_720P_60;
            *penSdFmt = HI_UNF_ENC_FMT_NTSC;
            break;
        }
        case HI_UNF_ENC_FMT_720P_50:
        {
            *penHdFmt = HI_UNF_ENC_FMT_720P_50;
            *penSdFmt = HI_UNF_ENC_FMT_PAL; 
            break;
        } 
        case HI_UNF_ENC_FMT_576P_50:
        {
            *penHdFmt = HI_UNF_ENC_FMT_576P_50;
            *penSdFmt = HI_UNF_ENC_FMT_PAL; 
            break;        
        }
        case HI_UNF_ENC_FMT_480P_60:
        {
            *penHdFmt = HI_UNF_ENC_FMT_480P_60;
            *penSdFmt = HI_UNF_ENC_FMT_NTSC; 
            break;         
        }
        case HI_UNF_ENC_FMT_PAL:
        {
            *penHdFmt = HI_UNF_ENC_FMT_PAL;
            *penSdFmt = HI_UNF_ENC_FMT_PAL;
            break;
        }
        case HI_UNF_ENC_FMT_NTSC:
        {
            *penHdFmt = HI_UNF_ENC_FMT_NTSC;
            *penSdFmt = HI_UNF_ENC_FMT_NTSC; 
            break;
        } 
        
        /* bellow are vga display formats */
        case HI_UNF_ENC_FMT_861D_640X480_60:
        {
            *penHdFmt = HI_UNF_ENC_FMT_861D_640X480_60;
            *penSdFmt = HI_UNF_ENC_FMT_NTSC; 
            break;
        }
        case HI_UNF_ENC_FMT_VESA_800X600_60:
        {
            *penHdFmt = HI_UNF_ENC_FMT_VESA_800X600_60;
            *penSdFmt = HI_UNF_ENC_FMT_NTSC; 
            break;
        }
        case HI_UNF_ENC_FMT_VESA_1024X768_60:
        {
            *penHdFmt = HI_UNF_ENC_FMT_VESA_1024X768_60;
            *penSdFmt = HI_UNF_ENC_FMT_NTSC; 
            break;
        }
        case HI_UNF_ENC_FMT_VESA_1280X720_60:
        {
            *penHdFmt = HI_UNF_ENC_FMT_VESA_1280X720_60;
            *penSdFmt = HI_UNF_ENC_FMT_NTSC; 
            break;
        }
        case HI_UNF_ENC_FMT_VESA_1280X800_60:
        {
            *penHdFmt = HI_UNF_ENC_FMT_VESA_1280X800_60;
            *penSdFmt = HI_UNF_ENC_FMT_NTSC; 
            break;
        }
        case HI_UNF_ENC_FMT_VESA_1280X1024_60:
        {
            *penHdFmt = HI_UNF_ENC_FMT_VESA_1280X1024_60;
            *penSdFmt = HI_UNF_ENC_FMT_NTSC;  
            break;
        }
        case HI_UNF_ENC_FMT_VESA_1360X768_60:
        {
            *penHdFmt = HI_UNF_ENC_FMT_VESA_1360X768_60;
            *penSdFmt = HI_UNF_ENC_FMT_NTSC;
            break;
        }
        case HI_UNF_ENC_FMT_VESA_1366X768_60:
        {
            *penHdFmt = HI_UNF_ENC_FMT_VESA_1366X768_60;
            *penSdFmt = HI_UNF_ENC_FMT_NTSC;   
            break;
        }
        case HI_UNF_ENC_FMT_VESA_1400X1050_60:
        {
            *penHdFmt = HI_UNF_ENC_FMT_VESA_1400X1050_60;
            *penSdFmt = HI_UNF_ENC_FMT_NTSC;  
            break;
        }
        case HI_UNF_ENC_FMT_VESA_1440X900_60:
        {
            *penHdFmt = HI_UNF_ENC_FMT_VESA_1440X900_60;
            *penSdFmt = HI_UNF_ENC_FMT_NTSC;    
            break;
        }
        case HI_UNF_ENC_FMT_VESA_1440X900_60_RB:
        {
            *penHdFmt = HI_UNF_ENC_FMT_VESA_1440X900_60_RB;
            *penSdFmt = HI_UNF_ENC_FMT_NTSC;  
            break;
        }
        case HI_UNF_ENC_FMT_VESA_1600X900_60_RB:
        {
            *penHdFmt = HI_UNF_ENC_FMT_VESA_1600X900_60_RB;
            *penSdFmt = HI_UNF_ENC_FMT_NTSC; 
            break;
        }
        case HI_UNF_ENC_FMT_VESA_1600X1200_60:
        {
            *penHdFmt = HI_UNF_ENC_FMT_VESA_1600X1200_60;
            *penSdFmt = HI_UNF_ENC_FMT_NTSC;   
            break;
        }
        case HI_UNF_ENC_FMT_VESA_1680X1050_60:
        {
            *penHdFmt = HI_UNF_ENC_FMT_VESA_1680X1050_60;
            *penSdFmt = HI_UNF_ENC_FMT_NTSC;  
            break;
        } 
        
        case HI_UNF_ENC_FMT_VESA_1920X1080_60:
        {
            *penHdFmt = HI_UNF_ENC_FMT_VESA_1920X1080_60;
            *penSdFmt = HI_UNF_ENC_FMT_NTSC;   
            break;
        }
        case HI_UNF_ENC_FMT_VESA_1920X1200_60:
        {
            *penHdFmt = HI_UNF_ENC_FMT_VESA_1920X1200_60;
            *penSdFmt = HI_UNF_ENC_FMT_NTSC;            
            break;
        }
        case HI_UNF_ENC_FMT_VESA_2048X1152_60:
        {
            *penHdFmt = HI_UNF_ENC_FMT_VESA_2048X1152_60;
            *penSdFmt = HI_UNF_ENC_FMT_NTSC;            
            break;
        }
        default:
        {
            *penHdFmt = HI_UNF_ENC_FMT_1080i_50;
            *penSdFmt = HI_UNF_ENC_FMT_PAL;
            break;
        }
    }

    return;
}

HI_S32 DRV_PDM_GetDispParam(HI_UNF_DISP_E enDisp, HI_DISP_PARAM_S *pstDispParam)
{
    HI_S32                      Ret;
    HI_DB_S                     stBaseDB;
    HI_DB_TABLE_S               stTable;
    HI_DB_KEY_S                 stKey;
    HI_U32                      u32BasePhyAddr;
    HI_U32                      u32BaseVirAddr;
    HI_U32                      u32BaseLen;
    HI_DB_KEY_S 				fp128;

    if ((HI_SUCCESS != PDM_GetVirAddrByName(PDM_BASEPARAM_BUFNAME, &u32BaseVirAddr, &u32BaseLen))
		|| (HI_NULL == u32BaseVirAddr))
	{
		Ret = PDM_GetBufByName(PDM_BASEPARAM_BUFNAME, &u32BasePhyAddr, &u32BaseLen);
	    if (HI_SUCCESS != Ret)
	    {
	        return Ret;
	    }
		
		u32BaseVirAddr = (HI_U32)PDM_VmapByPhyaddr(u32BasePhyAddr, u32BaseLen);
		PDM_SetVirAddrByName(PDM_BASEPARAM_BUFNAME, u32BaseVirAddr);
	}    

    PDM_GetDefDispParam(enDisp, pstDispParam);

    Ret = HI_DB_GetDBFromMem((HI_U8 *)u32BaseVirAddr, &stBaseDB);
    if(HI_SUCCESS != Ret)
    {
        HI_INFO_PDM("ERR: HI_DB_GetDBFromMem, use default baseparam!\n"); //556
        return HI_FAILURE;
    }
    
	if (HI_UNF_DISPLAY0 == enDisp)
	{
		Ret = HI_DB_GetTableByName(&stBaseDB, MCE_BASE_TABLENAME_DISP0, &stTable);
	}
	else
	{
		Ret = HI_DB_GetTableByName(&stBaseDB, MCE_BASE_TABLENAME_DISP1, &stTable);
	}

	if(HI_SUCCESS != Ret)
    {
        HI_INFO_PDM("ERR: HI_DB_GetTableByName, use default baseparam!\n"); //571
        return HI_SUCCESS;
    }
    
    Ret = HI_DB_GetKeyByName(&stTable, MCE_BASE_KEYNAME_FMT, &stKey);
    if(HI_SUCCESS == Ret)
    {
    	pstDispParam->enFormat = *(HI_UNF_ENC_FMT_E *)(stKey.pValue);

    	if (pstDispParam->enFormat == 260)
    	{
    		//80545600
    		Ret = HI_DB_GetKeyByName(&stTable, "BASE_KEY_TIMING_VFB", &fp128);
    	    if(HI_SUCCESS == Ret)
    	    {
    	        pstDispParam->stDispTiming.VFB = *(HI_U32 *)(fp128.pValue);
    	    }

    		Ret = HI_DB_GetKeyByName(&stTable, "BASE_KEY_TIMING_VBB", &fp128);
    	    if(HI_SUCCESS == Ret)
    	    {
    	        pstDispParam->stDispTiming.VBB = *(HI_U32 *)(fp128.pValue);
    	    }

    		Ret = HI_DB_GetKeyByName(&stTable, "BASE_KEY_TIMING_VACT", &fp128);
    	    if(HI_SUCCESS == Ret)
    	    {
    	        pstDispParam->stDispTiming.VACT = *(HI_U32 *)(fp128.pValue);
    	    }

    		Ret = HI_DB_GetKeyByName(&stTable, "BASE_KEY_TIMING_HFB", &fp128);
    	    if(HI_SUCCESS == Ret)
    	    {
    	        pstDispParam->stDispTiming.HFB = *(HI_U32 *)(fp128.pValue);
    	    }

    		Ret = HI_DB_GetKeyByName(&stTable, "BASE_KEY_TIMING_HBB", &fp128);
    	    if(HI_SUCCESS == Ret)
    	    {
    	        pstDispParam->stDispTiming.HBB = *(HI_U32 *)(fp128.pValue);
    	    }

    		Ret = HI_DB_GetKeyByName(&stTable, "BASE_KEY_TIMING_HACT", &fp128);
    	    if(HI_SUCCESS == Ret)
    	    {
    	        pstDispParam->stDispTiming.HACT = *(HI_U32 *)(fp128.pValue);
    	    }

    		Ret = HI_DB_GetKeyByName(&stTable, "BASE_KEY_TIMING_VPW", &fp128);
    	    if(HI_SUCCESS == Ret)
    	    {
    	        pstDispParam->stDispTiming.VPW = *(HI_U32 *)(fp128.pValue);
    	    }

    		Ret = HI_DB_GetKeyByName(&stTable, "BASE_KEY_TIMING_HPW", &fp128);
    	    if(HI_SUCCESS == Ret)
    	    {
    	        pstDispParam->stDispTiming.HPW = *(HI_U32 *)(fp128.pValue);
    	    }

    		Ret = HI_DB_GetKeyByName(&stTable, "BASE_KEY_TIMING_IDV", &fp128);
    	    if(HI_SUCCESS == Ret)
    	    {
    	        pstDispParam->stDispTiming.IDV = *(HI_BOOL *)(fp128.pValue);
    	    }

    		Ret = HI_DB_GetKeyByName(&stTable, "BASE_KEY_TIMING_IHS", &fp128);
    	    if(HI_SUCCESS == Ret)
    	    {
    	        pstDispParam->stDispTiming.IHS = *(HI_BOOL *)(fp128.pValue);
    	    }

    		Ret = HI_DB_GetKeyByName(&stTable, "BASE_KEY_TIMING_IVS", &fp128);
    	    if(HI_SUCCESS == Ret)
    	    {
    	        pstDispParam->stDispTiming.IVS = *(HI_BOOL *)(fp128.pValue);
    	    }

    		Ret = HI_DB_GetKeyByName(&stTable, "BASE_KEY_TIMING_CLKR", &fp128);
    	    if(HI_SUCCESS == Ret)
    	    {
    	        pstDispParam->stDispTiming.ClockReversal = *(HI_BOOL *)(fp128.pValue);
    	    }

    		Ret = HI_DB_GetKeyByName(&stTable, "BASE_KEY_TIMING_DW", &fp128);
    	    if(HI_SUCCESS == Ret)
    	    {
    	        pstDispParam->stDispTiming.DataWidth = *(HI_UNF_DISP_INTF_DATA_WIDTH_E *)(fp128.pValue);
    	    }

    		Ret = HI_DB_GetKeyByName(&stTable, "BASE_KEY_TIMING_ITFMT", &fp128);
    	    if(HI_SUCCESS == Ret)
    	    {
    	        pstDispParam->stDispTiming.ItfFormat = *(HI_UNF_DISP_INTF_DATA_FMT_E *)(fp128.pValue);
    	    }

    		Ret = HI_DB_GetKeyByName(&stTable, "BASE_KEY_TIMING_DITE", &fp128);
    	    if(HI_SUCCESS == Ret)
    	    {
    	        pstDispParam->stDispTiming.DitherEnable = *(HI_BOOL *)(fp128.pValue);
    	    }

    		Ret = HI_DB_GetKeyByName(&stTable, "BASE_KEY_TIMING_CLK0", &fp128);
    	    if(HI_SUCCESS == Ret)
    	    {
    	        pstDispParam->stDispTiming.ClkPara0 = *(HI_U32 *)(fp128.pValue);
    	    }

    		Ret = HI_DB_GetKeyByName(&stTable, "BASE_KEY_TIMING_CLK1", &fp128);
    	    if(HI_SUCCESS == Ret)
    	    {
    	        pstDispParam->stDispTiming.ClkPara1 = *(HI_U32 *)(fp128.pValue);
    	    }

    		Ret = HI_DB_GetKeyByName(&stTable, "BASE_KEY_TIMING_INTERL", &fp128);
    	    if(HI_SUCCESS == Ret)
    	    {
    	        pstDispParam->stDispTiming.bInterlace = *(HI_BOOL *)(fp128.pValue);
    	    }

    		Ret = HI_DB_GetKeyByName(&stTable, "BASE_KEY_TIMING_FIXF", &fp128);
    	    if(HI_SUCCESS == Ret)
    	    {
    	        pstDispParam->stDispTiming.PixFreq = *(HI_U32 *)(fp128.pValue);
    	    }

    		Ret = HI_DB_GetKeyByName(&stTable, "BASE_KEY_TIMING_VERTF", &fp128);
    	    if(HI_SUCCESS == Ret)
    	    {
    	        pstDispParam->stDispTiming.VertFreq = *(HI_U32 *)(fp128.pValue);
    	    }

    		Ret = HI_DB_GetKeyByName(&stTable, "BASE_KEY_TIMING_AW", &fp128);
    	    if(HI_SUCCESS == Ret)
    	    {
    	        pstDispParam->stDispTiming.AspectRatioW = *(HI_U32 *)(fp128.pValue);
    	    }

    		Ret = HI_DB_GetKeyByName(&stTable, "BASE_KEY_TIMING_AH", &fp128);
    	    if(HI_SUCCESS == Ret)
    	    {
    	        pstDispParam->stDispTiming.AspectRatioH = *(HI_U32 *)(fp128.pValue);
    	    }

    		Ret = HI_DB_GetKeyByName(&stTable, "BASE_KEY_TIMING_USEGAMMA", &fp128);
    	    if(HI_SUCCESS == Ret)
    	    {
    	        pstDispParam->stDispTiming.bUseGamma = *(HI_BOOL *)(fp128.pValue);
    	    }

    		Ret = HI_DB_GetKeyByName(&stTable, "BASE_KEY_TIMING_RSV0", &fp128);
    	    if(HI_SUCCESS == Ret)
    	    {
    	        pstDispParam->stDispTiming.Reserve0 = *(HI_U32 *)(fp128.pValue);
    	    }

    		Ret = HI_DB_GetKeyByName(&stTable, "BASE_KEY_TIMING_RSV1", &fp128);
    	    if(HI_SUCCESS == Ret)
    	    {
    	        pstDispParam->stDispTiming.Reserve1 = *(HI_U32 *)(fp128.pValue);
    	    }
    	}
    }

	Ret = HI_DB_GetKeyByName(&stTable, MCE_BASE_KEYNAME_INTF_HDMI, &stKey);
    if(HI_SUCCESS == Ret)
    {
        pstDispParam->stIntf[HI_UNF_DISP_INTF_TYPE_HDMI] = *(HI_UNF_DISP_INTF_S *)(stKey.pValue);
    }
    
    Ret = HI_DB_GetKeyByName(&stTable, MCE_BASE_KEYNAME_INTF_YPBPR, &stKey);
    if(HI_SUCCESS == Ret)
    {
        pstDispParam->stIntf[HI_UNF_DISP_INTF_TYPE_YPBPR] = *(HI_UNF_DISP_INTF_S *)(stKey.pValue);
    }
    
    Ret = HI_DB_GetKeyByName(&stTable, MCE_BASE_KEYNAME_INTF_CVBS, &stKey);
    if(HI_SUCCESS == Ret)
    {
        pstDispParam->stIntf[HI_UNF_DISP_INTF_TYPE_CVBS] = *(HI_UNF_DISP_INTF_S *)(stKey.pValue);
    }

    Ret = HI_DB_GetKeyByName(&stTable, MCE_BASE_KEYNAME_INTF_RGB, &stKey);
    if(HI_SUCCESS == Ret)
    {
        pstDispParam->stIntf[HI_UNF_DISP_INTF_TYPE_RGB] = *(HI_UNF_DISP_INTF_S *)(stKey.pValue);
    }

    Ret = HI_DB_GetKeyByName(&stTable, MCE_BASE_KEYNAME_INTF_SVIDEO, &stKey);
    if(HI_SUCCESS == Ret)
    {
        pstDispParam->stIntf[HI_UNF_DISP_INTF_TYPE_SVIDEO] = *(HI_UNF_DISP_INTF_S *)(stKey.pValue);
    }

    Ret = HI_DB_GetKeyByName(&stTable, "BASE_KEY_BT1120", &stKey);
    if(HI_SUCCESS == Ret)
    {
        pstDispParam->stIntf[HI_UNF_DISP_INTF_TYPE_BT1120] = *(HI_UNF_DISP_INTF_S *)(stKey.pValue);
    }

    Ret = HI_DB_GetKeyByName(&stTable, "BASE_KEY_BT656", &stKey);
    if(HI_SUCCESS == Ret)
    {
        pstDispParam->stIntf[HI_UNF_DISP_INTF_TYPE_BT656] = *(HI_UNF_DISP_INTF_S *)(stKey.pValue);
    }

    Ret = HI_DB_GetKeyByName(&stTable, "BASE_KEY_LCD", &stKey);
    if(HI_SUCCESS == Ret)
    {
        pstDispParam->stIntf[HI_UNF_DISP_INTF_TYPE_LCD] = *(HI_UNF_DISP_INTF_S *)(stKey.pValue);
    }

    Ret = HI_DB_GetKeyByName(&stTable, "BASE_KEY_VGA", &stKey);
    if(HI_SUCCESS == Ret)
    {
        pstDispParam->stIntf[HI_UNF_DISP_INTF_TYPE_VGA] = *(HI_UNF_DISP_INTF_S *)(stKey.pValue);
    }

    Ret = HI_DB_GetKeyByName(&stTable, MCE_BASE_KEYNAME_HULEP, &stKey);
    if(HI_SUCCESS == Ret)
    {
        pstDispParam->u32HuePlus = *(HI_U32 *)(stKey.pValue);
    }

    Ret = HI_DB_GetKeyByName(&stTable, MCE_BASE_KEYNAME_SATU, &stKey);
    if(HI_SUCCESS == Ret)
    {
        pstDispParam->u32Saturation = *(HI_U32 *)(stKey.pValue);
    }

    Ret = HI_DB_GetKeyByName(&stTable, MCE_BASE_KEYNAME_CONTR, &stKey);
    if(HI_SUCCESS == Ret)
    {
        pstDispParam->u32Contrast = *(HI_U32 *)(stKey.pValue);
    }

    Ret = HI_DB_GetKeyByName(&stTable, MCE_BASE_KEYNAME_BRIG, &stKey);
    if(HI_SUCCESS == Ret)
    {
        pstDispParam->u32Brightness = *(HI_U32 *)(stKey.pValue);
    }

    Ret = HI_DB_GetKeyByName(&stTable, MCE_BASE_KEYNAME_BGCOLOR, &stKey);
    if(HI_SUCCESS == Ret)
    {
        pstDispParam->stBgColor = *(HI_UNF_DISP_BG_COLOR_S *)(stKey.pValue);
    }

#if 0
    Ret = HI_DB_GetKeyByName(&stTable, MCE_BASE_KEYNAME_TIMING, &stKey);
    if(HI_SUCCESS == Ret)
    {
        pstDispParam->stDispTiming = *(HI_UNF_DISP_TIMING_S *)(stKey.pValue);
    }
#endif

    Ret = HI_DB_GetKeyByName(&stTable, MCE_BASE_KEYNAME_GAMA, &stKey);
    if(HI_SUCCESS == Ret)
    {
        pstDispParam->bGammaEnable = *(HI_BOOL *)(stKey.pValue);
    }

	Ret = HI_DB_GetKeyByName(&stTable, MCE_BASE_KEYNAME_ASPECT, &stKey);
    if(HI_SUCCESS == Ret)
    {
        pstDispParam->stAspectRatio = *(HI_UNF_DISP_ASPECT_RATIO_S *)(stKey.pValue);
    }
#if 0
    Ret = HI_DB_GetKeyByName(&stTable, MCE_BASE_KEYNAME_SCRX, &stKey);
    if(HI_SUCCESS == Ret)
    {
        pstDispParam->u32ScreenXpos = *(HI_U32 *)(stKey.pValue);
    }

    Ret = HI_DB_GetKeyByName(&stTable, MCE_BASE_KEYNAME_SCRY, &stKey);
    if(HI_SUCCESS == Ret)
    {
        pstDispParam->u32ScreenYpos = *(HI_U32 *)(stKey.pValue);
    }
    
    Ret = HI_DB_GetKeyByName(&stTable, MCE_BASE_KEYNAME_SCRW, &stKey);
    if(HI_SUCCESS == Ret)
    {
        pstDispParam->u32ScreenWidth = *(HI_U32 *)(stKey.pValue);
    }

    Ret = HI_DB_GetKeyByName(&stTable, MCE_BASE_KEYNAME_SCRH, &stKey);
    if(HI_SUCCESS == Ret)
    {
        pstDispParam->u32ScreenHeight = *(HI_U32 *)(stKey.pValue);
    }
#endif
	Ret = HI_DB_GetKeyByName(&stTable, MCE_BASE_KEYNAME_SRC_DISP, &stKey);
    if(HI_SUCCESS == Ret)
    {
        pstDispParam->enSrcDisp = *(HI_UNF_DISP_E *)(stKey.pValue);
    }
	
	Ret = HI_DB_GetKeyByName(&stTable, MCE_BASE_KEYNAME_VIRSCW, &stKey);
    if(HI_SUCCESS == Ret)
    {
        pstDispParam->u32VirtScreenWidth = *(HI_U32 *)(stKey.pValue);
    }
    Ret = HI_DB_GetKeyByName(&stTable, MCE_BASE_KEYNAME_VIRSCH, &stKey);
    if(HI_SUCCESS == Ret)
    {
        pstDispParam->u32VirtScreenHeight = *(HI_U32 *)(stKey.pValue);
    }
    Ret = HI_DB_GetKeyByName(&stTable, MCE_BASE_KEYNAME_DISP_L, &stKey);
    if(HI_SUCCESS == Ret)
    {
        pstDispParam->stOffsetInfo.u32Left = *(HI_U32 *)(stKey.pValue);
    }
    Ret = HI_DB_GetKeyByName(&stTable, MCE_BASE_KEYNAME_DISP_T, &stKey);
    if(HI_SUCCESS == Ret)
    {
        pstDispParam->stOffsetInfo.u32Top = *(HI_U32 *)(stKey.pValue);
    }
	Ret = HI_DB_GetKeyByName(&stTable, MCE_BASE_KEYNAME_DISP_R, &stKey);
    if(HI_SUCCESS == Ret)
    {
        pstDispParam->stOffsetInfo.u32Right = *(HI_U32 *)(stKey.pValue);
    }
    Ret = HI_DB_GetKeyByName(&stTable, MCE_BASE_KEYNAME_DISP_B, &stKey);
    if(HI_SUCCESS == Ret)
    {
        pstDispParam->stOffsetInfo.u32Bottom = *(HI_U32 *)(stKey.pValue);
    }

	Ret = HI_DB_GetKeyByName(&stTable, MCE_BASE_KEYNAME_PF, &stKey);
    if(HI_SUCCESS == Ret)
    {
        pstDispParam->enPixelFormat = *(HIGO_PF_E *)(stKey.pValue);
    }
    else
    {
        pstDispParam->enPixelFormat = HIGO_PF_8888;
    }


    return HI_SUCCESS;
}

HI_S32 DRV_PDM_GetMceParam(HI_MCE_PARAM_S *pstMceParam)
{
    HI_S32                      Ret;
    HI_DB_S                     stBaseDB;
    HI_DB_TABLE_S               stTable;
    HI_DB_KEY_S                 stKey;
    HI_U32                      u32MceParaPhyAddr;
    HI_U32                      u32MceParaVirAddr;
    HI_U32                      u32MceParaLen;

	if ((HI_SUCCESS != PDM_GetVirAddrByName(PDM_PLAYPARAM_BUFNAME, &u32MceParaVirAddr, &u32MceParaLen))
		|| (HI_NULL == u32MceParaVirAddr))
	{
		Ret = PDM_GetBufByName(PDM_PLAYPARAM_BUFNAME, &u32MceParaPhyAddr, &u32MceParaLen);
	    if (HI_SUCCESS != Ret)
	    {
	        return Ret;
	    }

		u32MceParaVirAddr = (HI_U32)PDM_VmapByPhyaddr(u32MceParaPhyAddr, u32MceParaLen);
		PDM_SetVirAddrByName(PDM_PLAYPARAM_BUFNAME, u32MceParaVirAddr);
	}    

    Ret = HI_DB_GetDBFromMem((HI_U8 *)u32MceParaVirAddr, &stBaseDB);
    if(HI_SUCCESS != Ret)
    {
        HI_INFO_PDM("ERR: HI_DB_GetDBFromMem!\n"); //762
        return HI_FAILURE;
    }

    Ret = HI_DB_GetTableByName(&stBaseDB, MCE_PLAY_TABLENAME, &stTable);
    if(HI_SUCCESS != Ret)
    {
        HI_INFO_PDM("ERR: HI_DB_GetTableByName!\n"); //769
        return HI_FAILURE;        
    }

    Ret = HI_DB_GetKeyByName(&stTable, MCE_PLAY_KEYNAME_FLAG, &stKey);
    if(HI_SUCCESS == Ret)
    {
        pstMceParam->u32CheckFlag = *(HI_U32 *)(stKey.pValue);
    }
    else
    {
        pstMceParam->u32CheckFlag = 0;
    }    

    Ret = HI_DB_GetKeyByName(&stTable, MCE_PLAY_KEYNAME_DATALEN, &stKey);
    if(HI_SUCCESS == Ret)
    {
        pstMceParam->u32PlayDataLen = *(HI_U32 *)(stKey.pValue);
    }
    else
    {
        pstMceParam->u32PlayDataLen = 0;
    }

    Ret = HI_DB_GetKeyByName(&stTable, MCE_PLAY_KEYNAME_PARAM, &stKey);
    if(HI_SUCCESS == Ret)
    {
        pstMceParam->stPlayParam = *(HI_UNF_MCE_PLAY_PARAM_S *)(stKey.pValue);
    }
    else
    {
        memset(&(pstMceParam->stPlayParam), 0x00, sizeof(HI_UNF_MCE_PLAY_PARAM_S));
    }

    return HI_SUCCESS;

}

HI_S32 DRV_PDM_GetMceData(HI_U32 u32Size, HI_U32 *pAddr)
{
    HI_S32                      Ret;
    HI_U32                      u32MceDataPhyAddr;
    HI_U32                      u32MceDataLen;
	HI_U32						u32MceDataVirAddr;

	if ((HI_SUCCESS != PDM_GetVirAddrByName(PDM_PLAYDATA_BUFNAME, &u32MceDataVirAddr, &u32MceDataLen))
		|| (HI_NULL == u32MceDataVirAddr))
	{
		Ret = PDM_GetBufByName(PDM_PLAYDATA_BUFNAME, &u32MceDataPhyAddr, &u32MceDataLen);
	    if (HI_SUCCESS != Ret)
	    {
	        return Ret;
	    }

		u32MceDataVirAddr = (HI_U32)PDM_VmapByPhyaddr(u32MceDataPhyAddr, u32MceDataLen);
		PDM_SetVirAddrByName(PDM_PLAYDATA_BUFNAME, u32MceDataVirAddr);
	}
	
    *pAddr = u32MceDataVirAddr;

    return HI_SUCCESS;
}

HI_S32 DRV_PDM_GetData(const HI_CHAR *BufName, HI_U32 *pu32DataAddr, HI_U32 *pu32DataLen)
{
	HI_S32 Ret = HI_SUCCESS;
	HI_U32 u32PhyAddr = 0;
	
	if ((HI_NULL != BufName)
		&& (HI_NULL != pu32DataAddr)
		&& (HI_NULL != pu32DataLen))
	{
		if ((HI_SUCCESS != PDM_GetVirAddrByName(BufName, pu32DataAddr, pu32DataLen))
		    || (HI_NULL == *pu32DataAddr))
		{
			Ret = PDM_GetBufByName(BufName, &u32PhyAddr, pu32DataLen);
		    if (HI_SUCCESS != Ret)
		    {
		        return Ret;
		    }
			
			*pu32DataAddr = (HI_U32)PDM_VmapByPhyaddr(u32PhyAddr, *pu32DataLen);

			return PDM_SetVirAddrByName(BufName, *pu32DataAddr);
		}
		else
		{
			return HI_SUCCESS;
		}
	}
	else
	{
		return HI_FAILURE;
	}
}

HI_S32 DRV_PDM_GetSoundParam(HI_UNF_SND_E enSound, HI_SOUND_PARAM_S* pSoundParam)
{
    HI_S32                      Ret;
    HI_DB_S                     stBaseDB;
    HI_DB_TABLE_S               stTable;
    HI_DB_KEY_S                 stKey;
    HI_U32                      u32MceParaPhyAddr;
    HI_U32                      u32MceParaVirAddr;
    HI_U32                      u32MceParaLen;
    HI_U32 u32Val;

    if ((enSound >= HI_UNF_SND_BUTT) || (pSoundParam == NULL))
	{
    	return HI_FAILURE;
	}

	if ((HI_SUCCESS != PDM_GetVirAddrByName(PDM_BASEPARAM_BUFNAME, &u32MceParaVirAddr, &u32MceParaLen))
		|| (HI_NULL == u32MceParaVirAddr))
	{
		Ret = PDM_GetBufByName(PDM_BASEPARAM_BUFNAME, &u32MceParaPhyAddr, &u32MceParaLen);
	    if (HI_SUCCESS != Ret)
	    {
	        return Ret;
	    }

		u32MceParaVirAddr = (HI_U32)PDM_VmapByPhyaddr(u32MceParaPhyAddr, u32MceParaLen);
		PDM_SetVirAddrByName(PDM_BASEPARAM_BUFNAME, u32MceParaVirAddr);
	}

    Ret = HI_DB_GetDBFromMem((HI_U8 *)u32MceParaVirAddr, &stBaseDB);
    if(HI_SUCCESS != Ret)
    {
        HI_INFO_PDM("ERR: HI_DB_GetDBFromMem, use default baseparam!\n"); //896
        return HI_FAILURE;
    }

    if (enSound == HI_UNF_SND_0)
    {
        Ret = HI_DB_GetTableByName(&stBaseDB, "BASE_TABLE_SOUND0", &stTable);
    }
    else if (enSound == HI_UNF_SND_1)
    {
        Ret = HI_DB_GetTableByName(&stBaseDB, "BASE_TABLE_SOUND1", &stTable);
    }
    else
    {
        Ret = HI_DB_GetTableByName(&stBaseDB, "BASE_TABLE_SOUND2", &stTable);
    }

    if(HI_SUCCESS != Ret)
    {
        HI_INFO_PDM("ERR: HI_DB_GetTableByName, use default baseparam!\n"); //915
        return HI_FAILURE;
    }

    pSoundParam->Data_0 = 0;

    Ret = HI_DB_GetKeyByName(&stTable, "BASE_KEY_SND_DAC0", &stKey);
    if(HI_SUCCESS == Ret)
    {
    	if (*(HI_U32 *)(stKey.pValue) == 0)
    	{
    		pSoundParam->Data_4[pSoundParam->Data_0].Data_0 = 0;

    	    Ret = HI_DB_GetKeyByName(&stTable, "BASE_KEY_SND_DAC_VOLUME0", &stKey);
    	    if(HI_SUCCESS == Ret)
    	    {
    	    	Data_8110aa1c = 1;
    	    	u32Val = *(HI_U32 *)(stKey.pValue);
    	    	if (u32Val > 100) u32Val = 50;
    	    }
    	    else
    	    {
    	    	Data_8110aa1c = 0;
    	    	u32Val = 50;
    	    }
    	    pSoundParam->Data_580[pSoundParam->Data_0++] = u32Val;
    	}
    }

    Ret = HI_DB_GetKeyByName(&stTable, "BASE_KEY_SND_SPDIF0", &stKey);
    if(HI_SUCCESS == Ret)
    {
    	if (*(HI_U32 *)(stKey.pValue) == 3)
    	{
    		pSoundParam->Data_4[pSoundParam->Data_0].Data_0 = 3;

    	    Ret = HI_DB_GetKeyByName(&stTable, "BASE_KEY_SND_SPDIF_VOLUME0", &stKey);
    	    if(HI_SUCCESS == Ret)
    	    {
    	    	Data_8110aa1c = 1;
    	    	u32Val = *(HI_U32 *)(stKey.pValue);
    	    	if (u32Val > 100) u32Val = 50;
    	    }
    	    else
    	    {
    	    	Data_8110aa1c = 0;
    	    	u32Val = 50;
    	    }
    	    pSoundParam->Data_580[pSoundParam->Data_0++] = u32Val;
    	}
    }

    Ret = HI_DB_GetKeyByName(&stTable, "BASE_KEY_SND_HDMI0", &stKey);
    if(HI_SUCCESS == Ret)
    {
    	if (*(HI_U32 *)(stKey.pValue) == 4)
    	{
    		pSoundParam->Data_4[pSoundParam->Data_0].Data_0 = 4;

    	    Ret = HI_DB_GetKeyByName(&stTable, "BASE_KEY_SND_HDMI_VOLUME0", &stKey);
    	    if(HI_SUCCESS == Ret)
    	    {
    	    	Data_8110aa1c = 1;
    	    	u32Val = *(HI_U32 *)(stKey.pValue);
    	    	if (u32Val > 100) u32Val = 50;
    	    }
    	    else
    	    {
    	    	Data_8110aa1c = 0;
    	    	u32Val = 50;
    	    }
    	    pSoundParam->Data_580[pSoundParam->Data_0++] = u32Val;
    	}
    }

    Ret = HI_DB_GetKeyByName(&stTable, "BASE_KEY_SND_ARC0", &stKey);
    if(HI_SUCCESS == Ret)
    {
    	if (*(HI_U32 *)(stKey.pValue) == 5)
    	{
    		pSoundParam->Data_4[pSoundParam->Data_0].Data_0 = 5;

    	    Ret = HI_DB_GetKeyByName(&stTable, "BASE_KEY_SND_ARC_VOLUME0", &stKey);
    	    if(HI_SUCCESS == Ret)
    	    {
    	    	Data_8110aa1c = 1;
    	    	u32Val = *(HI_U32 *)(stKey.pValue);
    	    	if (u32Val > 100) u32Val = 50;
    	    }
    	    else
    	    {
    	    	Data_8110aa1c = 0;
    	    	u32Val = 50;
    	    }
    	    pSoundParam->Data_580[pSoundParam->Data_0++] = u32Val;
    	}
    }

    Ret = HI_DB_GetKeyByName(&stTable, "BASE_KEY_SND_I2S0", &stKey);
    if(HI_SUCCESS == Ret)
    {
    	if (*(HI_U32 *)(stKey.pValue) == 1)
    	{
    		pSoundParam->Data_4[pSoundParam->Data_0].Data_0 = 1;
    		pSoundParam->Data_4[pSoundParam->Data_0].Data_4 = ((struct hiSOUND_PARAM_S_Inner2*)(stKey.pValue))->Data_4;

    	    Ret = HI_DB_GetKeyByName(&stTable, "BASE_KEY_SND_I2S_VOLUME0", &stKey);
    	    if(HI_SUCCESS == Ret)
    	    {
    	    	Data_8110aa1c = 1;
    	    	u32Val = *(HI_U32 *)(stKey.pValue);
    	    	if (u32Val > 100) u32Val = 50;
    	    }
    	    else
    	    {
    	    	Data_8110aa1c = 0;
    	    	u32Val = 50;
    	    }
    	    pSoundParam->Data_580[pSoundParam->Data_0++] = u32Val;
    	}
    }

    Ret = HI_DB_GetKeyByName(&stTable, "BASE_KEY_SND_I2S1", &stKey);
    if(HI_SUCCESS == Ret)
    {
    	if (*(HI_U32 *)(stKey.pValue) == 2)
    	{
    		pSoundParam->Data_4[pSoundParam->Data_0].Data_0 = 2;
    		pSoundParam->Data_4[pSoundParam->Data_0].Data_4 = ((struct hiSOUND_PARAM_S_Inner2*)(stKey.pValue))->Data_4;

    	    Ret = HI_DB_GetKeyByName(&stTable, "BASE_KEY_SND_I2S_VOLUME1", &stKey);
    	    if(HI_SUCCESS == Ret)
    	    {
    	    	Data_8110aa1c = 1;
    	    	u32Val = *(HI_U32 *)(stKey.pValue);
    	    	if (u32Val > 100) u32Val = 50;
    	    }
    	    else
    	    {
    	    	Data_8110aa1c = 0;
    	    	u32Val = 50;
    	    }
    	    pSoundParam->Data_580[pSoundParam->Data_0++] = u32Val;
    	}
    }

    return HI_SUCCESS;
}

HI_S32 DRV_PDM_TagRemoveBuff(const HI_CHAR *BufName)
{
    HI_CHAR                 PdmTag[512];
    HI_U32                  PdmLen;
    HI_CHAR                 *p, *q;
    int i, j=0;

    memset(PdmTag, 0x0, 512);

    PdmLen = get_param_data("pdm_tag", PdmTag, 512);
    if (PdmLen >= 512)
    {
        return HI_FAILURE;
    }

    PdmTag[511] = '\0';

    p = strstr(PdmTag, BufName);
    if (p == 0)
    {
        return HI_FAILURE;
    }

    q = strchr(p, ' ');
    if (q == 0)
    {
    	*p = '\0';
    }
    else
    {
    	for (i = 0; i < (strlen(PdmTag) + (PdmTag - (q+1))); i++)
    	{
    		p[i] = q[++j];
    	}
    	p[i] = '\0';
    }

    set_param_data("pdm_tag", PdmTag, strlen(PdmTag)+1);

	return HI_SUCCESS;
}

HI_S32 DRV_PDM_ReleaseReserveMem(const HI_CHAR *BufName)
{
    HI_U32                      i;    
	
    DRV_PDM_LOCK(&g_PdmGlobal.PdmMutex); //1128

    for (i = 0; i < g_PdmGlobal.u32BufNum; i++)
    {
        if (0 == strncmp(g_PdmGlobal.stBufInfo[i].as8BufName, BufName, strlen(BufName)))
        {
            break;    
        }
    }

    if (i >= g_PdmGlobal.u32BufNum)
    {
        DRV_PDM_UNLOCK(&g_PdmGlobal.PdmMutex);
        return HI_FAILURE;
    }

    if (g_PdmGlobal.stBufInfo[i].bRelease)
    {
        DRV_PDM_UNLOCK(&g_PdmGlobal.PdmMutex);
        return HI_SUCCESS;
    }
    
    g_PdmGlobal.stBufInfo[i].bRelease = HI_TRUE;

	if (HI_NULL != g_PdmGlobal.stBufInfo[i].u32VirAddr)
	{
		vunmap((HI_VOID *)(g_PdmGlobal.stBufInfo[i].u32VirAddr));

		g_PdmGlobal.stBufInfo[i].u32VirAddr = HI_NULL;	
	}

    DRV_PDM_UNLOCK(&g_PdmGlobal.PdmMutex);

    pdm_free_reserve_mem(g_PdmGlobal.stBufInfo[i].u32PhyAddr, g_PdmGlobal.stBufInfo[i].u32Lenth);
   
    DRV_PDM_TagRemoveBuff(BufName);

    return HI_SUCCESS;
}


HI_S32 HI_DRV_PDM_GetDispParam(HI_UNF_DISP_E enDisp, HI_DISP_PARAM_S *pstDispParam)
{
    return DRV_PDM_GetDispParam(enDisp, pstDispParam);
}

HI_S32 HI_DRV_PDM_GetMceParam(HI_MCE_PARAM_S *pMceParam)
{
    return DRV_PDM_GetMceParam(pMceParam);
}

HI_S32 HI_DRV_PDM_GetMceData(HI_U32 u32Size, HI_U32 *pAddr)
{
    return DRV_PDM_GetMceData(u32Size, pAddr);
}

HI_S32 HI_DRV_PDM_ReleaseReserveMem(const HI_CHAR *BufName)
{
    return DRV_PDM_ReleaseReserveMem(BufName);
}

HI_S32 HI_DRV_PDM_GetData(const HI_CHAR *BufName, HI_U32 *pu32DataAddr, HI_U32 *pu32DataLen)
{
	return DRV_PDM_GetData(BufName, pu32DataAddr, pu32DataLen);
}

HI_S32 HI_DRV_PDM_GetSoundParam(HI_UNF_SND_E enSound, HI_SOUND_PARAM_S* pSoundParam)
{
	return DRV_PDM_GetSoundParam(enSound, pSoundParam);
}

HI_S32 HI_DRV_PDM_GetSoundParamEx(HI_UNF_SND_E enSound, HI_SOUND_PARAM_S* pSoundParam, HI_U32 *pu32Data)
{
	HI_S32 res;

	if (pu32Data == 0)
	{
		return 0x80520005;
	}

	res = DRV_PDM_GetSoundParam(enSound, pSoundParam);

	*pu32Data = Data_8110aa1c;

	return res;
}

