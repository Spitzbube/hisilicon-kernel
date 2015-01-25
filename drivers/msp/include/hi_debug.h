/******************************************************************************
Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.
******************************************************************************
File Name     : hi_debug.h
Version       : Initial Draft
Author        : Hisilicon multimedia software group
Created       : 2005/4/23
Last Modified :
Description   : Common debugging macro definition
Function List :
History       :
******************************************************************************/
#ifndef __HI_DEBUG_H__
#define __HI_DEBUG_H__


#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#include "hi_type.h"
#include "hi_module.h"

#if defined(HI_LOG_SUPPORT) && (0 == HI_LOG_SUPPORT)
#undef HI_DEBUG

#define HI_PANIC(fmt...)  do{\
}while(0)

#define HI_PRINT(fmt...) do{\
}while(0)
#else
#define HI_DEBUG

#ifdef __OS_LINUX__
#ifdef __KERNEL__
    #define HI_PRINT printk
#else
#include <stdio.h>
    #define HI_PRINT printf
#endif /* end of __KERNEL__ */
#else  /* __OS_ECOS__ */
#define HI_PRINT printf
#endif /* end of __OS_LINUX__ */

#ifdef __KERNEL__
    #define HI_PANIC printk
#else
    #define HI_PANIC printf
#endif

#endif

/*************************** Structure Definition ****************************/
/** \addtogroup     HI_DEBUG */
/** @{ */  /** <!-- [HI_DEBUG] */


/**Default level of the output debugging information*/
/**CNcomment: Ĭ�ϵĵ�����Ϣ�������*/
#define HI_LOG_LEVEL_DEFAULT HI_LOG_LEVEL_DBG //HI_LOG_LEVEL_ERROR

/**Level of the output debugging information*/
/**CNcomment: ������Ϣ�������*/
typedef enum hiLOG_LEVEL_E
{
    HI_LOG_LEVEL_FATAL   = 0,     /**<Fatal error. It indicates that a critical problem occurs in the system. Therefore, you must pay attention to it.*/
                                  /**<CNcomment: �������, ���������Ҫ�ر��ע��һ����ִ��������ϵͳ�������ش����� */
    HI_LOG_LEVEL_ERROR   = 1,     /**<Major error. It indicates that a major problem occurs in the system and the system cannot run.*/
                                  /**<CNcomment: һ�����, һ����ִ��������ϵͳ�����˱Ƚϴ�����⣬������������ */
    HI_LOG_LEVEL_WARNING = 2,     /**<Warning. It indicates that a minor problem occurs in the system, but the system still can run properly.*/
                                  /**<CNcomment: �澯��Ϣ, һ����ִ�����Ϣ���ϵͳ���ܳ������⣬���ǻ��ܼ������� */
    HI_LOG_LEVEL_INFO    = 3,     /**<Message. It is used to prompt users. Users can open the message when locating problems. It is recommended to disable this message in general.*/
                                  /**<CNcomment: ��ʾ��Ϣ, һ����Ϊ�����û���������ڶ�λ�����ʱ����Դ򿪣�һ������½���ر� */
    HI_LOG_LEVEL_DBG     = 4,     /**<Debug. It is used to prompt developers. Developers can open the message when locating problems. It is recommended to disable this message in general.*/
                                  /**<CNcomment: ��ʾ��Ϣ, һ����Ϊ������Ա����������趨�Ĵ�ӡ����һ������½���ر� */

    HI_LOG_LEVEL_BUTT
} HI_LOG_LEVEL_E;

/** @} */

/**Just only for fatal level print.   */   /**CNcomment: Ϊ�˴�ӡ������Ϣ���ƶ��ĺ��ӡ���� */
#define HI_TRACE_LEVEL_FATAL    (0)
/**Just only for error level print.   */   /**CNcomment: Ϊ�˴�ӡ������Ϣ���ƶ��ĺ��ӡ���� */
#define HI_TRACE_LEVEL_ERROR    (1)
/**Just only for warning level print. */   /**CNcomment: Ϊ�˴�ӡ������Ϣ���ƶ��ĺ��ӡ���� */
#define HI_TRACE_LEVEL_WARN     (2)
/**Just only for info level print.    */   /**CNcomment: Ϊ�˴�ӡ��Ϣ������ƶ��ĺ��ӡ���� */
#define HI_TRACE_LEVEL_INFO     (3)
/**Just only for debug level print.   */   /**CNcomment: Ϊ�˴�ӡ������Ϣ���ƶ��ĺ��ӡ���� */
#define HI_TRACE_LEVEL_DBG      (4)

#ifndef HI_LOG_LEVEL
#define HI_LOG_LEVEL         (HI_TRACE_LEVEL_INFO)
#endif


/**Just only debug output,MUST BE NOT calling it. */
/**CNcomment: ���������Ϣ�ӿڣ����Ƽ�ֱ�ӵ��ô˽ӿ� */
extern HI_VOID HI_LogOut(HI_U32 u32Level, HI_MOD_ID_E enModId,
            HI_U8 *pFuncName, HI_U32 u32LineNum, const char *format, ...);

#ifdef HI_DEBUG

#define HI_TRACE(level, module_id, fmt...)                      \
    do{                                                         \
        HI_LogOut(level, module_id, (HI_U8*)__FUNCTION__,__LINE__,fmt);  \
    }while(0)

#define HI_ASSERT(expr)                                     \
    do{                                                     \
        if (!(expr)) {                                      \
            HI_PANIC("\nASSERT failed at:\n  >File name: %s\n  >Function : %s\n  >Line No. : %d\n  >Condition: %s\n", \
                __FILE__,__FUNCTION__, __LINE__, #expr);    \
        }                                                   \
    }while(0)


#define HI_ASSERT_RET(expr)                                     \
    do{                                                         \
        if (!(expr)) {                                          \
            HI_PRINT("\n<%s %d>: ASSERT Failure{" #expr "}\n",  \
                          __FUNCTION__, __LINE__);              \
            return HI_FAILURE;                                  \
        }                                                       \
    }while(0)

#define HI_DEBUG_LOG(fmt...) \
            HI_TRACE(HI_LOG_LEVEL_INFO, HI_ID_SYS, fmt)


/**Supported for debug output to serial/network/u-disk. */
/**CNcomment: ����ģ����Ҫ�������º�������������Ϣ������������ڡ���ڡ�U�̴洢�� */
/**Just only reserve the fatal level output. */
/**CNcomment: ������������ĵ�����Ϣ */
#if (HI_LOG_LEVEL == HI_TRACE_LEVEL_FATAL)
#define HI_FATAL_PRINT(module_id, fmt...)   HI_TRACE(HI_TRACE_LEVEL_FATAL,    module_id, fmt)
#define HI_ERR_PRINT(  module_id, fmt...)
#define HI_WARN_PRINT( module_id, fmt...)
#define HI_INFO_PRINT( module_id, fmt...)
#define HI_DBG_PRINT(  module_id, fmt...)
/**Just only reserve the fatal/error level output. */
/**CNcomment: ������������ĺʹ��󼶱�ĵ�����Ϣ */
#elif (HI_LOG_LEVEL == HI_TRACE_LEVEL_ERROR)
#define HI_FATAL_PRINT(module_id, fmt...)   HI_TRACE(HI_TRACE_LEVEL_FATAL,    module_id, fmt)
#define HI_ERR_PRINT(  module_id, fmt...)   HI_TRACE(HI_TRACE_LEVEL_ERROR,    module_id, fmt)
#define HI_WARN_PRINT( module_id, fmt...)
#define HI_INFO_PRINT( module_id, fmt...)
#define HI_DBG_PRINT(  module_id, fmt...)
/**Just only reserve the fatal/error/warning level output. */
/**CNcomment: ������������ġ�����ġ����漶��ĵ�����Ϣ */
#elif (HI_LOG_LEVEL == HI_TRACE_LEVEL_WARN)
#define HI_FATAL_PRINT(module_id, fmt...)   HI_TRACE(HI_TRACE_LEVEL_FATAL,    module_id, fmt)
#define HI_ERR_PRINT(  module_id, fmt...)   HI_TRACE(HI_TRACE_LEVEL_ERROR,    module_id, fmt)
#define HI_WARN_PRINT( module_id, fmt...)   HI_TRACE(HI_TRACE_LEVEL_WARN,     module_id, fmt)
#define HI_INFO_PRINT( module_id, fmt...)
#define HI_DBG_PRINT(  module_id, fmt...)
/**Just only reserve the fatal/error/warning/info level output. */
/**CNcomment: ������������ġ�����ġ��������Ϣ����ĵ�����Ϣ */
#elif (HI_LOG_LEVEL == HI_TRACE_LEVEL_INFO)
#define HI_FATAL_PRINT(module_id, fmt...)   HI_TRACE(HI_TRACE_LEVEL_FATAL,    module_id, fmt)
#define HI_ERR_PRINT(  module_id, fmt...)   HI_TRACE(HI_TRACE_LEVEL_ERROR,    module_id, fmt)
#define HI_WARN_PRINT( module_id, fmt...)   HI_TRACE(HI_TRACE_LEVEL_WARN,     module_id, fmt)
#define HI_INFO_PRINT( module_id, fmt...)   HI_TRACE(HI_TRACE_LEVEL_INFO,     module_id, fmt)
#define HI_DBG_PRINT(  module_id, fmt...)
#else
/**Reserve all the levels output. */
/**CNcomment: �������м��������Ϣ */
#define HI_FATAL_PRINT(module_id, fmt...)   HI_TRACE(HI_TRACE_LEVEL_FATAL,    module_id, fmt)
#define HI_ERR_PRINT(  module_id, fmt...)   HI_TRACE(HI_TRACE_LEVEL_ERROR,    module_id, fmt)
#define HI_WARN_PRINT( module_id, fmt...)   HI_TRACE(HI_TRACE_LEVEL_WARN,     module_id, fmt)
#define HI_INFO_PRINT( module_id, fmt...)   HI_TRACE(HI_TRACE_LEVEL_INFO,     module_id, fmt)
#define HI_DBG_PRINT(  module_id, fmt...)   HI_TRACE(HI_TRACE_LEVEL_DBG,      module_id, fmt)
#endif

#else
#define HI_FATAL_PRINT(module_id, fmt...)
#define HI_ERR_PRINT(  module_id, fmt...)
#define HI_WARN_PRINT( module_id, fmt...)
#define HI_INFO_PRINT( module_id, fmt...)
#define HI_DBG_PRINT(  module_id, fmt...)
#define HI_TRACE(level, module_id, fmt...)
#define HI_ASSERT(expr)
#define HI_ASSERT_RET(expr)
#define HI_DEBUG_LOG(fmt...)
#endif /* endif HI_DEBUG */

/** @} */  /** <!-- ==== Structure Definition End ==== */


#define MKSTR(exp) # exp
#define MKMARCOTOSTR(exp) MKSTR(exp)
#define VERSION_STRING ("SDK_VERSION:["MKMARCOTOSTR(SDK_VERSION)"] Build Time:["__DATE__", "__TIME__"]")


/**Initialize a user module.*/
/**CNcomment: �û�ģ���ʼ�� */
#define HI_MODULE_DECLARE(MODULE_NAME)	\
	static HI_U32 g_u32ModuleId = HI_INVALID_MODULE_ID;	\
	static HI_S32 __attribute__((constructor(200))) init_module_id() \
	{	 \
		return HI_MODULE_RegisterByName(MODULE_NAME, &g_u32ModuleId); \
	} \
    static HI_S32 __attribute__((destructor(200))) deinit_module_id() \
    { \
        return HI_MODULE_UnRegister(g_u32ModuleId); \
    }

/**Defines a user module ID.*/
/**CNcomment: �û�ģ��ID�궨�� */
#define MODULE_ID (g_u32ModuleId)

/**Defines the command of the user module different level log print.*/
/**CNcomment: �û�ģ����־����궨�� */
#define HI_MODULE_FATAL(pszFormat...)  HI_FATAL_PRINT(MODULE_ID,pszFormat)
#define HI_MODULE_ERROR(pszFormat...)  HI_ERR_PRINT(MODULE_ID,pszFormat)
#define HI_MODULE_WARN(pszFormat...)   HI_WARN_PRINT(MODULE_ID,pszFormat)
#define HI_MODULE_DEBUG(pszFormat...)  HI_DBG_PRINT(MODULE_ID,pszFormat)
#define HI_MODULE_INFO(pszFormat...)   HI_INFO_PRINT(MODULE_ID,pszFormat)

/**Defines the memory allocate and free command used by user module.*/
/**CNcomment: �û�ģ���ڴ����궨�� */
#define HI_MODULE_MALLOC(u32Size)               HI_MEM_Malloc(MODULE_ID, u32Size)
#define HI_MODULE_FREE(pMemAddr)                HI_MEM_Free(MODULE_ID, pMemAddr)
#define HI_MODULE_CALLOC(u32MemBlock, u32Size)  HI_MEM_Calloc(MODULE_ID, u32MemBlock, u32Size)
#define HI_MODULE_REALLOC(pMemAddr, u32Size)    HI_MEM_Realloc(MODULE_ID, pMemAddr, u32Size)


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __HI_DEBUG_H__ */


