/*****************************************************************************/
/*                Copyright 2009 - 2019, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: wmalloc.c                                                       */
/* Version: 01a How to realize Memory management                                              */
/*                                                                           */
/* History:                                                                  */
/* 1. 01a,2009-2-25, w54130 Create this file.                                 */
/*****************************************************************************/
#include "tde_define.h"
#include "wmalloc.h"

typedef struct _MemoryBlock
{
    HI_U32          nSize;
    HI_U16          nFree;
    HI_U16          nFirst;
    HI_U16          nUnitSize;
#if HI_TDE_MEMCOUNT_SUPPORT    
    HI_U16          nMaxUsed;           /* Max used unit number             */
    HI_U16          nMaxNum;            /* Max unit number                  */
#endif    
    HI_U8*          pStartAddr;
    struct _MemoryBlock*    pNext;
}MemoryBlock;

typedef enum 
{
    UNIT_SIZE_64 = 0,
    UNIT_SIZE_128,
    UNIT_SIZE_1024,
    UNIT_SIZE_BUTT
}UNIT_SIZE_E;

STATIC MemoryBlock g_struMemBlock[UNIT_SIZE_BUTT]; 

#define PRINTMEMINFO() do\
{\
    TDE_TRACE(TDE_KERN_DEBUG, "-----------------------------------------------------\n");\
    TDE_TRACE(TDE_KERN_DEBUG, "\tMemBlock Info\ttotal\tfree\n");\
    TDE_TRACE(TDE_KERN_DEBUG, "\t64\t%d\t%d\n", HI_TDE_UNIT64_NUM, g_struMemBlock[UNIT_SIZE_64].nFree);\
    TDE_TRACE(TDE_KERN_DEBUG, "\t128\t%d\t%d\n", HI_TDE_UNIT128_NUM, g_struMemBlock[UNIT_SIZE_128].nFree);\
    TDE_TRACE(TDE_KERN_DEBUG, "\t1024\t%d\t%d\n", HI_TDE_UNIT1024_NUM, g_struMemBlock[UNIT_SIZE_1024].nFree);\
    TDE_TRACE(TDE_KERN_DEBUG, "------------------------------------------------------\n");\
}while(0)

HI_S32 MemoryBlockInit(UNIT_SIZE_E eUnitSize, HI_U32 nUnitNum, HI_U8 *pAddr) 
{
    HI_U16 i;
    HI_U8 *pData = pAddr;
    if((eUnitSize >= UNIT_SIZE_BUTT) || (NULL == pAddr ) || (0 == nUnitNum))
    {
        return HI_FAILURE;
    }
    if(UNIT_SIZE_1024 == eUnitSize)
    {
        g_struMemBlock[eUnitSize].nUnitSize = 1024;
    }
    else
    {
        g_struMemBlock[eUnitSize].nUnitSize = (eUnitSize + 1) * 64;
    }

    for(i = 1; i < nUnitNum; i++)
    {
        /* Don't flag for last unit,for last unit is ready for assigned, which is say no next unit can be assigned */
        *(HI_U16 *)pData = i;   
        
        pData += g_struMemBlock[eUnitSize].nUnitSize;

    }

    g_struMemBlock[eUnitSize].nFirst = 0;
    g_struMemBlock[eUnitSize].nFree = nUnitNum;
    g_struMemBlock[eUnitSize].nSize = nUnitNum * g_struMemBlock[eUnitSize].nUnitSize;
    g_struMemBlock[eUnitSize].pNext = NULL;
    g_struMemBlock[eUnitSize].pStartAddr = pAddr;

#if HI_TDE_MEMCOUNT_SUPPORT    
    g_struMemBlock[eUnitSize].nMaxNum = nUnitNum;
    g_struMemBlock[eUnitSize].nMaxUsed= 0;
#endif

    return HI_SUCCESS;
}

HI_VOID *mallocUnit(UNIT_SIZE_E eUnitSize)
{
    MemoryBlock *pBlock = &g_struMemBlock[eUnitSize];
    HI_U8* pFree = NULL;

    TDE_TRACE(TDE_KERN_DEBUG, "eUnitSize %d, free units:%d, first free unit:%d...\n", eUnitSize, pBlock->nFree, pBlock->nFirst);
    if(!pBlock->nFree)
    {
        return NULL;
    }

    pFree = pBlock->pStartAddr + pBlock->nFirst * pBlock->nUnitSize;
    pBlock->nFirst = *(HI_U16 *)pFree;

    pBlock->nFree--;
    memset(pFree, 0, pBlock->nUnitSize);

#if HI_TDE_MEMCOUNT_SUPPORT    
    if((g_struMemBlock[eUnitSize].nMaxNum - pBlock->nFree) > g_struMemBlock[eUnitSize].nMaxUsed)
    {
        g_struMemBlock[eUnitSize].nMaxUsed = g_struMemBlock[eUnitSize].nMaxNum - pBlock->nFree;
    }
#endif
    return pFree;
}


HI_VOID *wmalloc(size_t size)
{
    UNIT_SIZE_E i;
    HI_VOID *pMalloc;

    if((size > 1024) || (0 == size))
    {
        return NULL;
    }

    if(size <= 64)
    {
        for(i = UNIT_SIZE_64; i < UNIT_SIZE_BUTT; i++)
        {
            pMalloc = mallocUnit(i);
            if(NULL != pMalloc)
            {
                return pMalloc;
            }
        }

        return NULL;
    }
    else if (size <= 128)
    {
        for(i = UNIT_SIZE_128; i < UNIT_SIZE_BUTT; i++)
        {
            pMalloc = mallocUnit(i);
            if(NULL != pMalloc)
            {
                return pMalloc;
            }
        }

        return NULL;
    }
    else
    {
        return mallocUnit(UNIT_SIZE_1024);
    }
}

HI_S32 freeUnit(UNIT_SIZE_E eUnitSize, HI_VOID *ptr)
{
    MemoryBlock *pBlock = &g_struMemBlock[eUnitSize];

    if(((HI_U32)ptr < (HI_U32)pBlock->pStartAddr) 
        || ((HI_U32)ptr >= ((HI_U32)pBlock->pStartAddr + pBlock->nSize)))
    {
        return HI_FAILURE;
    }

    pBlock->nFree++;
    *(HI_U16*)ptr = pBlock->nFirst; /* point to next unit can be assigned */
    pBlock->nFirst = ((HI_U32)ptr - (HI_U32)pBlock->pStartAddr)/pBlock->nUnitSize;
    TDE_TRACE(TDE_KERN_DEBUG, "eUnitSize:%d,first free unit:%d\n", pBlock->nUnitSize, pBlock->nFirst);
    TDE_TRACE(TDE_KERN_DEBUG, "eUnitSize:%d,first free unit:%d, free units:%d\n", pBlock->nUnitSize, pBlock->nFirst, pBlock->nFree);

    return HI_SUCCESS;
}

HI_S32 wfree(HI_VOID *ptr)
{
    UNIT_SIZE_E i;
    
    for(i = UNIT_SIZE_64; i < UNIT_SIZE_BUTT; i++)
    {
       if(HI_SUCCESS == freeUnit(i, ptr)) 
       {
            return HI_SUCCESS;
       }
    }

    TDE_TRACE(TDE_KERN_INFO, "Free mem failed!vir:%p, phy:%x\n", ptr, wgetphy(ptr));
    return HI_FAILURE;
}

/************************all before is unrelated with tde, can be transplant to other modules ************************/

/************************ The follow is related with tde encapsulation******************************************/


#define TDE_UNIT64_OFFSET   0
#define TDE_UNIT128_OFFSET  (HI_TDE_UNIT64_NUM * 64)
#define TDE_UNIT1024_OFFSET (TDE_UNIT128_OFFSET + HI_TDE_UNIT128_NUM * 128)

/* TDE memory pool size, default is 352K byte */
#define TDE_MEMPOOL_SIZE                (HI_TDE_UNIT64_NUM * 64 + HI_TDE_UNIT128_NUM * 128 + HI_TDE_UNIT1024_NUM * 1024)

STATIC HI_U32 g_u32MemPoolPhyAddr;
STATIC HI_U32 g_u32MemPoolVrtAddr;
STATIC HI_U32 g_u32MemPoolSize;

HI_S32 wmeminit(void)
{
    //TDE_NEW_MMB(g_pstruMemPool);
    //if (HI_NULL == g_pstruMemPool)
    //{
    //    TDE_TRACE(TDE_KERN_ERR, "new MMB failed!\n");
    //    return HI_FAILURE;
    //}
    g_u32MemPoolPhyAddr = HI_GFX_AllocMem("TDE_MemPool",NULL,TDE_MEMPOOL_SIZE + 0x200000);
    if(0 == g_u32MemPoolPhyAddr)
    {
        TDE_TRACE(TDE_KERN_INFO, "malloc mempool buffer failed!\n");
        HI_GFX_FreeMem(0);
        return HI_FAILURE;
    }
    g_u32MemPoolVrtAddr = (HI_U32 )HI_GFX_Map(g_u32MemPoolPhyAddr);
    
    g_u32MemPoolVrtAddr += 0x100000;
    g_u32MemPoolPhyAddr += 0x100000;

    MemoryBlockInit(UNIT_SIZE_64, HI_TDE_UNIT64_NUM, (HI_VOID *)(g_u32MemPoolVrtAddr));
    MemoryBlockInit(UNIT_SIZE_128, HI_TDE_UNIT128_NUM, (HI_VOID *)(g_u32MemPoolVrtAddr + TDE_UNIT128_OFFSET));

    MemoryBlockInit(UNIT_SIZE_1024, HI_TDE_UNIT1024_NUM, (HI_VOID *)(g_u32MemPoolVrtAddr + TDE_UNIT1024_OFFSET));

    g_u32MemPoolSize = TDE_UNIT1024_OFFSET + HI_TDE_UNIT1024_NUM * 1024;
    PRINTMEMINFO();
    
    return HI_SUCCESS;
}

HI_VOID wmemterm(void)
{
    PRINTMEMINFO();
    HI_GFX_Unmap ((HI_VOID *)(g_u32MemPoolVrtAddr - 0x100000));
    HI_GFX_FreeMem( g_u32MemPoolPhyAddr - 0x100000);

    g_u32MemPoolPhyAddr = 0;
    g_u32MemPoolVrtAddr = 0;
    g_u32MemPoolSize = 0;
}

HI_U32 wgetphy(HI_VOID *ptr)
{
    HI_U32 u32MemVrt = (HI_U32)ptr;  

    if((u32MemVrt < g_u32MemPoolVrtAddr)
        || (u32MemVrt >= (g_u32MemPoolVrtAddr + g_u32MemPoolSize)))
    {
        return 0;
    }

    return (g_u32MemPoolPhyAddr + (u32MemVrt - g_u32MemPoolVrtAddr));
}

HI_VOID * wgetvrt(HI_U32 phyaddr)
{
    if((phyaddr < g_u32MemPoolPhyAddr)
        || (phyaddr >= (g_u32MemPoolPhyAddr + g_u32MemPoolSize)))
    {
        return NULL;
    }

    return (HI_VOID *)(g_u32MemPoolVrtAddr + (phyaddr - g_u32MemPoolPhyAddr));
}

HI_U32 wgetfreenum(HI_VOID)
{
    UNIT_SIZE_E eUnitSize = 0;
    HI_U32 u32FreeUnitNum = g_struMemBlock[eUnitSize].nFree;
    

    for(eUnitSize = UNIT_SIZE_64; eUnitSize < UNIT_SIZE_BUTT; eUnitSize++)
    {
        u32FreeUnitNum = (u32FreeUnitNum > g_struMemBlock[eUnitSize].nFree)?g_struMemBlock[eUnitSize].nFree:u32FreeUnitNum;
    }


    return u32FreeUnitNum;
}

struct seq_file * wprintinfo(struct seq_file *page)
{
#if HI_TDE_MEMCOUNT_SUPPORT
    HI_U32 u32MaxUsed64    = g_struMemBlock[UNIT_SIZE_64].nMaxUsed;
    HI_U32 u32MaxUsed128   = g_struMemBlock[UNIT_SIZE_128].nMaxUsed;
    HI_U32 u32MaxUsed1024  = g_struMemBlock[UNIT_SIZE_1024].nMaxUsed;
#else
    HI_U32 u32Free64    = g_struMemBlock[UNIT_SIZE_64].nFree;
    HI_U32 u32Free128   = g_struMemBlock[UNIT_SIZE_128].nFree;
    HI_U32 u32Free1024  = g_struMemBlock[UNIT_SIZE_1024].nFree;
 #endif

 #ifndef CONFIG_TDE_STR_DISABLE
    PROC_PRINT(page, "--------- Hisilicon TDE Memory Pool Info ---------\n");
    #if HI_TDE_MEMCOUNT_SUPPORT
    PROC_PRINT(page, "     Type         Total       MaxUsed\n");
    PROC_PRINT(page, "[Unit 64  ]   %8u  %8u\n", HI_TDE_UNIT64_NUM, u32MaxUsed64);
    PROC_PRINT(page, "[Unit 128 ]   %8u  %8u\n", HI_TDE_UNIT128_NUM, u32MaxUsed128);
    PROC_PRINT(page, "[Unit 1024]   %8u  %8u\n", HI_TDE_UNIT1024_NUM, u32MaxUsed1024);
    PROC_PRINT(page, "[Total    ]   %8uK %8uK\n", TDE_MEMPOOL_SIZE/1024, (64 * u32MaxUsed64 + 128 * u32MaxUsed128 + 1024 * u32MaxUsed1024)/1024);
    #else
    PROC_PRINT(page, "     Type         Total       Used\n");
    PROC_PRINT(page, "[Unit 64  ]   %8u  %8u\n", HI_TDE_UNIT64_NUM, HI_TDE_UNIT64_NUM - u32Free64);
    PROC_PRINT(page, "[Unit 128 ]   %8u  %8u\n", HI_TDE_UNIT128_NUM, HI_TDE_UNIT128_NUM - u32Free128);
    PROC_PRINT(page, "[Unit 1024]   %8u  %8u\n", HI_TDE_UNIT1024_NUM, HI_TDE_UNIT1024_NUM - u32Free1024);
    PROC_PRINT(page, "[Total    ]   %8uK %8uK\n", TDE_MEMPOOL_SIZE/1024, (TDE_MEMPOOL_SIZE - (64 * u32Free64 + 128 * u32Free128 + 1024 * u32Free1024))/1024);
    #endif
#endif
    return page;
}

