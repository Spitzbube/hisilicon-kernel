/*****************************************************************************
*             Copyright 2006 - 2050, Hisilicon Tech. Co., Ltd.
*                           ALL RIGHTS RESERVED
* FileName: tde_define.h
* Description:TDE types define for all module
*
* History:
* Version   Date          Author        DefectNum       Description
*
*****************************************************************************/

#ifndef _TDE_DEFINE_H_
#define _TDE_DEFINE_H_

#include <asm/uaccess.h>
#include <linux/time.h>
#include <linux/mm.h>
#include <linux/hardirq.h>

#include "hi_type.h"
#include "hi_drv_mmz.h"
#include "hi_debug.h"
#include "hi_drv_dev.h"
#include "wmalloc.h"
#include "tde_osr.h"
#include "hi_kernel_adapt.h"

#include "hi_gfx_comm_k.h"
#include "tde_config.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif  /* __cplusplus */
#endif  /* __cplusplus */

/****************************************************************************/
/*                             TDE types define                             */
/****************************************************************************/
/* Pixel format classify */
typedef enum hiTDE_COLORFMT_CATEGORY_E
{
    TDE_COLORFMT_CATEGORY_ARGB,
    TDE_COLORFMT_CATEGORY_CLUT,
    TDE_COLORFMT_CATEGORY_An,
    TDE_COLORFMT_CATEGORY_YCbCr,
    TDE_COLORFMT_CATEGORY_BYTE,
    TDE_COLORFMT_CATEGORY_HALFWORD,
    TDE_COLORFMT_CATEGORY_MB,
    TDE_COLORFMT_CATEGORY_BUTT
} TDE_COLORFMT_CATEGORY_E;


/* Node submit type, Independant operation node, Operated head node, Operated middle node */
typedef enum hiTDE_NODE_SUBM_TYPE_E
{
    TDE_NODE_SUBM_ALONE = 0,    /* current node submit as independant operated node */
    TDE_NODE_SUBM_PARENT = 1,   /* current node submit as operated parent node */
    TDE_NODE_SUBM_CHILD = 2     /* current node submit as operated child node */
}TDE_NODE_SUBM_TYPE_E;

/* List type , synchronized list(SQ)/asynchroned list(AQ) */
typedef enum hiTDE_LIST_TYPE_E
{
    TDE_LIST_AQ = 0,    /* AQ */
#if HI_TDE_SQ_SUPPORT
    TDE_LIST_SQ,        /* SQ */
#endif    
    TDE_LIST_BUTT   
} TDE_LIST_TYPE_E;

/* Operation setting information node */
typedef struct hiTDE_NODE_BUF_S
{
    HI_VOID *pBuf; 		/* setting information node buffer*/
    HI_U32   u32NodeSz;	/* current node occupied size, united in byte*/
    HI_U64   u64Update;	/* current node update flag */
    HI_U32   u32PhyAddr;/* current node addr in hardware list */
} TDE_NODE_BUF_S;

/* Notified mode  after current node's job end */
typedef enum hiTDE_NOTIFY_MODE_E
{
    TDE_JOB_NONE_NOTIFY = 0,    /* none notify after job end */
    TDE_JOB_COMPL_NOTIFY,   	/* notify after job end */
    TDE_JOB_WAKE_NOTIFY,    	/* wake noytify after job end */
    TDE_JOB_NOTIFY_BUTT
} TDE_NOTIFY_MODE_E;

#define TDE_VERSION_PILOT 

#define STATIC static
#define INLINE inline
#define TDE_MAX_WAIT_TIMEOUT 2000
#define TDE_USE_SPINLOCK

//#define TDE_TIME_COUNT


#define TDE_KERN_ERR        1    
#define TDE_KERN_WARNING    2
#define TDE_KERN_INFO       3   
#define TDE_KERN_DEBUG      4  

#ifndef CONFIG_TDE_DEBUG_DISABLE  
#define TDE_TRACE( level, fmt...)    \
 do {                                       \
        if(TDE_KERN_ERR == level)           \
            HI_GFX_COMM_LOG_FATAL(HIGFX_TDE_ID, fmt); \
        else if(TDE_KERN_WARNING == level)  \
            HI_GFX_COMM_LOG_ERROR(HIGFX_TDE_ID, fmt);   \
        else if(TDE_KERN_INFO == level)     \
            HI_GFX_COMM_LOG_WARNING(HIGFX_TDE_ID, fmt);  \
        else                                \
            HI_GFX_COMM_LOG_INFO(HIGFX_TDE_ID, fmt);  \
    } while (0)
#else 
#define TDE_TRACE( level, fmt... )  
#endif

#ifndef CONFIG_TDE_DEBUG_DISABLE  
#define TDE_ASSERT(expr)  do {      \
            if(!(expr)) { \
                TDE_TRACE(TDE_KERN_ERR, "Assertion [%s] failed! %s:%s(line=%d)\n",\
#expr,__FILE__,__FUNCTION__,__LINE__); \
                panic("Assertion panic\n");     \
            } } while(0)
#else
#define TDE_ASSERT(expr)
#endif

STATIC INLINE HI_VOID * TDE_MALLOC(HI_U32 size) 
{
    HI_VOID* ptr;

    ptr = (HI_VOID *)wmalloc(size);
    return ptr;
    
}

STATIC INLINE HI_VOID TDE_FREE(HI_VOID* ptr)
{
    wfree(ptr);
}


#define TDE_INIT_WAITQUEUE_HEAD(pqueue) init_waitqueue_head(pqueue)

#define TDE_WAIT_EVENT_INTERRUPTIBLE_TIMEOUT(queue, condition, timeout) wait_event_interruptible_timeout(queue, condition, timeout)

#define TDE_WAKEUP_INTERRUPTIBLE(pqueue) wake_up_interruptible(pqueue)

#ifdef TDE_USE_SPINLOCK
#define TDE_DOWN_INTERRUPTIBLE(pmutex)
#define TDE_UP(pmutex)
#else
#define TDE_DOWN_INTERRUPTIBLE(pmutex) \
    do{\
        if(!in_interrupt())\
        {\
            if(down_interruptible(pmutex) < 0)\
            {\
                TDE_TRACE(TDE_KERN_ERR, "<%s> Line %d:down_interruptible failed!\n", __FUNCTION__, __LINE__);\
            }\
        }\
    }while(0)

#define TDE_UP(pmutex)  \
    do{\
        if(!in_interrupt())\
        {\
            up(pmutex);\
        }\
    }while(0)
#endif

typedef struct timeval TDE_timeval_s;
#define TDE_gettimeofday do_gettimeofday

#ifdef __cplusplus
#if __cplusplus
}
#endif  /* __cplusplus */
#endif  /* __cplusplus */

#endif  /* _TDE_DEFINE_H_ */
