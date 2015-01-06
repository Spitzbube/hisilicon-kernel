#include <linux/slab.h>

#include "hi_type.h"
#include "drv_mutils.h"
#include "drv_module_ioctl.h"
#include "hi_drv_mem.h"

#ifdef USING_KMEM
#include "common_mem_mgr.h"
#include "mem_mgr_drv.h"
#include "common_mem_base.h"
#endif

#ifdef LOG_PRIVATE
#include "common_log_base.h"
#endif

#define KERNEL_MODE "kernel"

static HI_U8* g_szPoolName[] = {"Module_Mem_Pool", "Adp_Mem_Pool", "Usr_Mem_Pool", "MMZ_Pool", "UnknowType"};


HI_HANDLE KMem_Utils_Init(HI_U32 u32Count, KMEM_UTILS_S stMem)
{
    KMEM_UTILS_S* pUtils = NULL;
    HI_U32 u32ItemSize = 0;
    HI_HANDLE hResult = 0;
    
    pUtils = (KMEM_UTILS_S*)kmalloc(sizeof(KMEM_UTILS_S), GFP_KERNEL);

    if (NULL == pUtils)
    {
        HI_ERR_MEM("<%s:%s> malloc %d size failure!\n", KERNEL_MODE, __func__, u32Count * u32ItemSize);

        return 0;
    }

    memset(pUtils, 0, sizeof(KMEM_UTILS_S));
    pUtils->enType = stMem.enType;
    pUtils->u32ItemCount = u32Count;

    switch(stMem.enType)
    {
        case KMEM_POOL_TYPE_MODULE:
            u32ItemSize = sizeof(MODULE_POOL_S)*u32Count;            
        break;

#ifdef USING_KMEM
        case KMEM_POOL_TYPE_MODULE_MEMORY:
            u32ItemSize = sizeof(MODULE_MEM_POOL_S)*u32Count;
        break;
        case KMEM_POOL_TYPE_USR_MEMORY:
            u32ItemSize = sizeof(USR_MEM_POOL_S)*u32Count;
        break;
#endif

#ifdef LOG_PRIVATE
        case KMEM_POOL_TYPE_LOG:
            u32ItemSize = sizeof(MODULE_LOG_POOL_S)*u32Count;
        break;
#endif
        default:
            pUtils->enType = KMEM_POOL_TYPE_BUTT;
        break;;
    }

    pUtils->pMemBaseAddr = kmalloc(u32ItemSize, GFP_KERNEL);
    if (NULL == pUtils->pMemBaseAddr)
    {
        HI_ERR_MEM("<%s:%s> , failed to request %s memory size %d\n", KERNEL_MODE, __func__, g_szPoolName[pUtils->enType], u32ItemSize);
        kfree(pUtils);
    }
    else
    {
        memset(pUtils->pMemBaseAddr, 0, u32ItemSize);
        hResult = (HI_HANDLE)pUtils;
        
        HI_INFO_MEM("<%s:%s>  successfully, request %s, and base address is %p\n", KERNEL_MODE, __func__, g_szPoolName[pUtils->enType], pUtils->pMemBaseAddr);
    }
    
    return hResult;
}

HI_VOID KMem_Utils_DeInit(HI_HANDLE hUtils)
{
    KMEM_UTILS_S* pUtils = (KMEM_UTILS_S*)hUtils;
    
    if (NULL != pUtils)
    {
        if (NULL != pUtils->pMemBaseAddr)
        {
            HI_INFO_MEM("<%s:%s>  free %s memory, and address is %p\n", KERNEL_MODE, __func__, g_szPoolName[pUtils->enType], pUtils->pMemBaseAddr);
            kfree(pUtils->pMemBaseAddr);
        }
        
        kfree((HI_VOID*)hUtils);
    }
}

#if 0
HI_U32 KMem_Utils_GetItemNo(HI_HANDLE hUtils)
{
    KMEM_UTILS_S* pUtils = (KMEM_UTILS_S*)hUtils;
    
    if (NULL != pUtils)
    {
        return pUtils->u32HasCount;
    }

    return 0;
}
#endif


HI_VOID* KMem_Utils_MALLOC(HI_HANDLE hUtils)
{
    HI_U32 u32Index = 0;

    KMEM_UTILS_S* pUtils = (KMEM_UTILS_S*)hUtils;
    HI_VOID* pResult = NULL;

    MODULE_POOL_S * pModuleBase = NULL;
    
#ifdef USING_KMEM
    MODULE_MEM_POOL_S*     pMemAdpBase = NULL;
    USR_MEM_POOL_S*     pMemBase    = NULL;
#endif

#ifdef LOG_PRIVATE
    MODULE_LOG_POOL_S*  pLogBase    = NULL;
#endif

    if (NULL != pUtils && pUtils->pMemBaseAddr != NULL)
    {
        HI_INFO_MEM("pool type:%d, maxsize:%u, size:%u, membase:0x%x.\n", pUtils->enType, pUtils->u32ItemCount, pUtils->u32HasCount,  pUtils->pMemBaseAddr);
                
        switch(pUtils->enType)
        {
            case KMEM_POOL_TYPE_MODULE:
            {
                pModuleBase = (MODULE_POOL_S*)pUtils->pMemBaseAddr;
            }
            break;

#ifdef USING_KMEM
            case KMEM_POOL_TYPE_MODULE_MEMORY:
            {
                pMemAdpBase = (MODULE_MEM_POOL_S*)pUtils->pMemBaseAddr;
            }
            break;
            case KMEM_POOL_TYPE_USR_MEMORY:
            {
                pMemBase = (USR_MEM_POOL_S*)pUtils->pMemBaseAddr;
            }
            break;
#endif

#ifdef LOG_PRIVATE
            case KMEM_POOL_TYPE_LOG:
            {
                pLogBase = (MODULE_LOG_POOL_S*)pUtils->pMemBaseAddr;
            }
            break;
#endif
            default:
            break;
        }
        
        for (u32Index=0; u32Index<pUtils->u32ItemCount; u32Index++)
        {            
            if (NULL != pModuleBase && pModuleBase[u32Index].u32Idle == 0)
            {
                pModuleBase[u32Index].u32Idle = 1;

                pResult = &pModuleBase[u32Index];

                pUtils->u32HasCount++;
                
                HI_INFO_MEM("<%s:%s> ... request %s and address[%d] is %p!\n", KERNEL_MODE, __func__, g_szPoolName[pUtils->enType], u32Index, pResult);
                break;
            }
            
#ifdef USING_KMEM

            if (NULL != pMemAdpBase && pMemAdpBase[u32Index].u32Idle == 0)
            {
                pMemAdpBase[u32Index].u32Idle = 1;

                pResult = &pMemAdpBase[u32Index];

                pUtils->u32HasCount++;
                
                HI_INFO_MEM("<%s:%s> ... request %s and address[%d] is %p!\n", KERNEL_MODE, __func__, g_szPoolName[pUtils->enType], u32Index, pResult);
                break;
            }

            if (NULL != pMemBase && pMemBase[u32Index].u32Idle == 0)
            {
                pMemBase[u32Index].u32Idle = 1;

                pResult = &pMemBase[u32Index];

                pUtils->u32HasCount++;
                
                HI_INFO_MEM("<%s:%s> ... request %s and address[%d] is %p!\n", KERNEL_MODE, __func__, g_szPoolName[pUtils->enType], u32Index, pResult);
                break;
            }
#endif

#ifdef LOG_PRIVATE
            if (NULL != pLogBase && pLogBase[u32Index].u32Idle == 0)
            {
                pLogBase[u32Index].u32Idle = 1;

                pResult = &pLogBase[u32Index];

                pUtils->u32HasCount++;
                
                HI_INFO_MEM("<%s:%s> ... request %s and address[%d] is %p!\n", KERNEL_MODE, __func__, g_szPoolName[pUtils->enType], u32Index, pResult);
                break;
            }
#endif
        }

        return pResult;
    }


    HI_ERR_MEM("<%s:%s> ... param is invalid!\n",KERNEL_MODE, __func__);
    
    return NULL;
}

HI_VOID KMem_Utils_FREE(HI_HANDLE hUtils, HI_VOID* pAddr)
{
    KMEM_UTILS_S* pUtils = (KMEM_UTILS_S*)hUtils;
    HI_U32 u32Index = 0;

    MODULE_POOL_S* pModuleBase = NULL;

#ifdef USING_KMEM
    MODULE_MEM_POOL_S*    pMemAdpBase = NULL;
    USR_MEM_POOL_S*    pMemBase    = NULL;
#endif

#ifdef LOG_PRIVATE
    MODULE_LOG_POOL_S* pLogBase    = NULL;
#endif

    if (NULL != pUtils && pUtils->pMemBaseAddr != NULL)
    {
        HI_INFO_MEM("pool type:%d, maxsize:%u, size:%u, addr:0x%x.\n", pUtils->enType, pUtils->u32ItemCount, pUtils->u32HasCount,  pAddr);
        
        switch(pUtils->enType)
        {
            case KMEM_POOL_TYPE_MODULE:
            {
                pModuleBase = (MODULE_POOL_S*)pUtils->pMemBaseAddr;
            }
            break;

#ifdef USING_KMEM
            case KMEM_POOL_TYPE_MODULE_MEMORY:
            {
                pMemAdpBase = (MODULE_MEM_POOL_S*)pUtils->pMemBaseAddr;
            }
            break;
            case KMEM_POOL_TYPE_USR_MEMORY:
            {
                pMemBase = (USR_MEM_POOL_S*)pUtils->pMemBaseAddr;
            }
            break;
#endif

#ifdef LOG_PRIVATE
            case KMEM_POOL_TYPE_LOG:
            {
                pLogBase = (MODULE_LOG_POOL_S*)pUtils->pMemBaseAddr;
            }
            break;
#endif
            default:
            break;
        }

        
        for (u32Index=0; u32Index<pUtils->u32ItemCount; u32Index++)
        {            
            if (NULL != pModuleBase && (&pModuleBase[u32Index] == pAddr) )
            {
                pModuleBase[u32Index].u32Idle = 0;

                memset(&pModuleBase[u32Index], 0, sizeof(pModuleBase[u32Index]));

                pUtils->u32HasCount--;
                
                HI_INFO_MEM("<%s:%s>... idle %s and address is %p, successfully!\n", KERNEL_MODE, __func__, g_szPoolName[pUtils->enType], pAddr);
                break;
            }

#ifdef USING_KMEM
            if (NULL != pMemAdpBase && (&pMemAdpBase[u32Index] == pAddr) )
            {
                pMemAdpBase[u32Index].u32Idle = 0;

                memset(&pMemAdpBase[u32Index], 0, sizeof(pMemAdpBase[u32Index]));

                pUtils->u32HasCount--;
                
                HI_INFO_MEM("<%s:%s>... idle %s and address is %p, successfully!\n", KERNEL_MODE, __func__, g_szPoolName[pUtils->enType], pAddr);
                break;
            }

            if (NULL != pMemBase && (&pMemBase[u32Index] == pAddr) )
            {
                pMemBase[u32Index].u32Idle = 0;

                memset(&pMemBase[u32Index], 0, sizeof(pMemBase[u32Index]));

                pUtils->u32HasCount--;
                
                HI_INFO_MEM("<%s:%s>... idle %s and address is %p, successfully!\n", KERNEL_MODE, __func__, g_szPoolName[pUtils->enType], pAddr);
                break;
            }
#endif

#ifdef LOG_PRIVATE
            if (NULL != pLogBase && (&pLogBase[u32Index] == pAddr) )
            {
                pLogBase[u32Index].u32Idle = 0;

                memset(&pLogBase[u32Index], 0, sizeof(pLogBase[u32Index]));

                pUtils->u32HasCount--;
                
                HI_INFO_MEM("<%s:%s>... idle %s and address is %p, successfully!\n", KERNEL_MODE, __func__, g_szPoolName[pUtils->enType], pAddr);
                break;
            }
#endif
        }

        return ;
    }


    HI_ERR_MEM("<%s:%s>... idle %s and address is %p, failure!!!\n", KERNEL_MODE, __func__, pUtils ? g_szPoolName[pUtils->enType] : g_szPoolName[KMEM_POOL_TYPE_BUTT], pAddr);
    
    return;

}

