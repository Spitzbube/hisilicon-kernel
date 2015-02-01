/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_common.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2009/12/15
  Description   : Common apis for hisilicon system.
  History       :
  1.Date        : 2010/01/25
    Author      : jianglei
    Modification: Created file

*******************************************************************************/

#ifndef __HI_COMMON_H__
#define __HI_COMMON_H__

#include "hi_type.h"
#include "hi_debug.h"

/*******************************Structure declaration *****************************/
/** @addtogroup     COMMON */
/** @{ */ /** <!--  [COMMON] */

/** Global config structure */
typedef struct hiSYS_CONF_S
{
    HI_U32 u32Reverse;  /**<Not used, reserved for extension*/ /**<CNcomment: ��ʱû��ʹ�ã�������չ*/

}HI_SYS_CONF_S;

/** Define the chip type. */
typedef enum hiCHIP_TYPE_E
{
    HI_CHIP_TYPE_HI3716M,
    HI_CHIP_TYPE_HI3716H,
    HI_CHIP_TYPE_HI3716C,
    HI_CHIP_TYPE_HI3716CES,

    HI_CHIP_TYPE_HI3720,
    HI_CHIP_TYPE_HI3712,
    HI_CHIP_TYPE_HI3715,

    HI_CHIP_TYPE_HI3718M,
    HI_CHIP_TYPE_HI3718C,
    HI_CHIP_TYPE_HI3719M,
    HI_CHIP_TYPE_HI3719C,
    HI_CHIP_TYPE_HI3719M_A, //11

    HI_CHIP_TYPE_HI3796C = 32,
    HI_CHIP_TYPE_HI3798C = 33,

    HI_CHIP_TYPE_HI3796C_A = 64,
    HI_CHIP_TYPE_HI3798C_A = 65,
    
    HI_CHIP_TYPE_BUTT
}HI_CHIP_TYPE_E;

/** Define the chip version. */
typedef enum hiCHIP_VERSION_E
{
    HI_CHIP_VERSION_V100 = 0x100,
    HI_CHIP_VERSION_V101 = 0x101,
    HI_CHIP_VERSION_V200 = 0x200,
    HI_CHIP_VERSION_V300 = 0x300,
    HI_CHIP_VERSION_V400 = 0x400,
    HI_CHIP_VERSION_BUTT
}HI_CHIP_VERSION_E;

/**System version, that is, the version of the software developer's kit (SDK)*/
typedef struct hiSYS_VERSION_S
{
    HI_CHIP_TYPE_E  enChipTypeSoft;      /**<Chip type corresponding to the SDK*/ /**<CNcomment:  SDK�����Ӧ��оƬ���� */
    HI_CHIP_TYPE_E  enChipTypeHardWare;  /**<Chip type that is detected when the SDK is running*/ /**<CNcomment:  SDK����ʱ��⵽��оƬ���� */
    HI_CHIP_VERSION_E enChipVersion;     /**<Chip version that is detected when the SDK is running*/ /**<CNcomment: SDK����ʱ��⵽оƬ�汾�� */
    HI_CHAR         aVersion[80];        /**<Version string of the SDK*/ /**<CNcomment:  SDK����汾���ַ� */
    HI_CHAR         BootVersion[80];     /**<Version string of the Boot*/ /**<CNcomment:  Boot�汾���ַ� */
}HI_SYS_VERSION_S;


/** Define the chip attributes */
typedef struct hiSYS_CHIP_ATTR_S
{
    HI_BOOL bDolbySupport;              /**<Whether this chip support dolby or not*//**<CNcomment:оƬ�Ƿ�֧�ֶű�*/
}HI_SYS_CHIP_ATTR_S;

/** Maximum bytes of a buffer name */
#define MAX_BUFFER_NAME_SIZE 16

/**Structure of an MMZ buffer*/
typedef struct hiMMZ_BUF_S
{
    HI_CHAR bufname[MAX_BUFFER_NAME_SIZE];  /**<Strings of an MMZ buffer name*/ /**<CNcomment:  MMZ buffer�����ַ� */
    HI_U32  phyaddr;                /**<Physical address of an MMZ buffer*/ /**<CNcomment:  MMZ buffer�����ַ */
    HI_U8  *kernel_viraddr;         /**<Kernel-state virtual address of an MMZ buffer*/ /**<CNcomment:  MMZ buffer�ں�̬�����ַ */
    HI_U8  *user_viraddr;           /**<User-state virtual address of an MMZ buffer*/ /**<CNcomment:  MMZ buffer�û�̬�����ַ */
    HI_U32  bufsize;                /**<Size of an MMZ buffer*/ /**<CNcomment:  MMZ buffer��С */
    HI_U32  overflow_threshold;     /**<Overflow threshold of an MMZ buffer, in percentage. For example, the value 100 indicates 100%.*/ /**<CNcomment:  MMZ buffer����ˮ�ߣ����ٷֱ����ã�����: 100 indicates 100%.*/
    HI_U32  underflow_threshold;    /**<Underflow threshold of an MMZ buffer, in percentage. For example, the value 0 indicates 0%.*/ /**<CNcomment:  MMZ buffer����ˮ�ߣ����ٷֱ����ã�����: 0 indicates 0%.*/
}HI_MMZ_BUF_S;

typedef struct hiRECT_S
{
    HI_S32 s32X;
    HI_S32 s32Y;
    HI_S32 s32Width;
    HI_S32 s32Height;
} HI_RECT_S;

typedef enum hiLAYER_ZORDER_E
{
    HI_LAYER_ZORDER_MOVETOP = 0,  /**<Move to the top*/ /**<CNcomment:  �Ƶ���� */
    HI_LAYER_ZORDER_MOVEUP,       /**<Move up*/ /**<CNcomment:  �����Ƶ� */
    HI_LAYER_ZORDER_MOVEBOTTOM,   /**<Move to the bottom*/ /**<CNcomment:  �Ƶ���ײ� */
    HI_LAYER_ZORDER_MOVEDOWN,     /**<Move down*/ /**<CNcomment:  �����Ƶ� */
    HI_LAYER_ZORDER_BUTT
} HI_LAYER_ZORDER_E;

/** Defines user mode proc show buffer */
/**CNcomment: �û�̬PROC buffer���� */
typedef struct hiPROC_SHOW_BUFFER_S
{
    HI_U8* pu8Buf;                  /**<Buffer address*/  /**<CNcomment: Buffer��ַ */
    HI_U32 u32Size;                 /**<Buffer size*/     /**<CNcomment: Buffer��С */
    HI_U32 u32Offset;               /**<Offset*/          /**<CNcomment: ��ӡƫ�Ƶ�ַ */
}HI_PROC_SHOW_BUFFER_S;

/** Proc show function */
/**CNcomment: Proc��Ϣ��ʾ�ص����� */
typedef HI_S32 (* HI_PROC_SHOW_FN)(HI_PROC_SHOW_BUFFER_S * pstBuf, HI_VOID *pPrivData);

/** Proc command function */
/**CNcomment: Proc���ƻص����� */
typedef HI_S32 (* HI_PROC_CMD_FN)(HI_PROC_SHOW_BUFFER_S * pstBuf, HI_U32 u32Argc, HI_U8 *pu8Argv[], HI_VOID *pPrivData);

/** Defines user mode proc entry */
/**CNcomment: �û�̬PROC��ڶ��� */
typedef struct hiPROC_ENTRY_S
{
    HI_CHAR *pszEntryName;          /**<Entry name*/            /**<CNcomment: ����ļ��� */
    HI_CHAR *pszDirectory;          /**<Directory name. If null, the entry will be added to /proc/hisi directory*/
                                    /**<CNcomment: Ŀ¼�����Ϊ�գ���������/proc/hisiĿ¼�� */
    HI_PROC_SHOW_FN pfnShowProc;    /**<Proc show function*/    /**<CNcomment: Proc��Ϣ��ʾ�ص����� */
    HI_PROC_CMD_FN pfnCmdProc;      /**<Proc command function*/ /**<CNcomment: Proc���ƻص����� */
    HI_VOID *pPrivData;             /**<Private data*/          /**<CNcomment: Buffer��ַ */
}HI_PROC_ENTRY_S;

#ifndef __KERNEL__
#include <time.h>

/** @} */  /** <!-- ==== Structure Definition end ==== */

/******************************* API declaration *****************************/
/** \addtogroup      COMMON */
/** @{ */  /** <!-- [COMMON] */

/**
@brief Initializes the system. CNcomment: ϵͳ��ʼ�� CNend
@attention \n
You must call this API to initialize the system before using the APIs of all modules.
Though you can call other APIs successfully before calling this API, the subsequent operations may fail.\n
CNcomment: ��ʹ������ģ��Ľӿ�֮ǰ����Ҫ�ȵ��ô˽ӿڶ�ϵͳ���г�ʼ��\n
�ڵ�������ӿ�֮ǰ��������ӿڣ����᷵��ʧ�ܣ����ǲ���ִ֤�е���ȷ�� CNend
@param N/A CNcomment: �� CNend
@retval ::HI_SUCCESS Success CNcomment: �ɹ� CNend
@retval ::HI_FAILURE Calling this API fails. CNcomment: APIϵͳ����ʧ�� CNend
@see \n
N/A CNcomment: �� CNend
*/
HI_S32 HI_SYS_Init(HI_VOID);

/**
@brief Deinitializes the system. CNcomment: ϵͳȥ��ʼ�� CNend
@attention \n
If all modules are not used, you need to call this API to deinitialize the system.\n
CNcomment: ����ģ�鶼����ʹ�ú���ô˽ӿ�ȥ��ʼ�� CNend
@param N/A CNcomment: �� CNend
@retval ::HI_SUCCESS Success CNcomment: �ɹ� CNend
@retval ::HI_FAILURE Calling this API fails. CNcomment: APIϵͳ����ʧ�� CNend
@see \n
N/A CNcomment: �� CNend
*/
HI_S32 HI_SYS_DeInit(HI_VOID);


/**
@brief Obtains the compiled time of a version. CNcomment: ��ȡ�汾�ı���ʱ�� CNend
@attention \n
The compiled time is the time during which the common module is made again.
CNcomment: ʱ��Ϊ����commonģ������make��ʱ�� CNend
@param[out] pstTime Pointer to the compiled time of a version (output). CNcomment: ָ�����ͣ�����汾�����ʱ�䡣 CNend
@retval ::HI_SUCCESS Success CNcomment: �ɹ� CNend
@retval ::HI_FAILURE Calling this API fails. CNcomment: APIϵͳ����ʧ�� CNend
@see \n
N/A CNcomment: �� CNend
*/
HI_S32 HI_SYS_GetBuildTime(struct tm * pstTime);


/**
@brief Obtains the version number. CNcomment: ��ȡ�汾�� CNend
@attention \n
N/A CNcomment: �� CNend
@param[out] pstVersion Pointer to the version number (output). CNcomment: ָ�����ͣ�����汾�š� CNend
@retval ::HI_SUCCESS Success CNcomment: �ɹ� CNend
@retval ::HI_FAILURE Calling this API fails. CNcomment: APIϵͳ����ʧ�� CNend
@see \n
N/A CNcomment: �� CNend
*/
HI_S32 HI_SYS_GetVersion(HI_SYS_VERSION_S *pstVersion);


/**
@brief Obtains the chip attributes. CNcomment: ��ȡоƬ���� CNend
@attention \n
N/A CNcomment: �� CNend
@param[out] pstChipAttr Pointer to the chip attributes(output). CNcomment: ָ�����ͣ����оƬ���� CNend
@retval ::HI_SUCCESS Success CNcomment: �ɹ� CNend
@retval ::HI_FAILURE Calling this API fails. CNcomment: APIϵͳ����ʧ�� CNend
@see \n
N/A CNcomment: �� CNend
*/

HI_S32 HI_SYS_GetChipAttr(HI_SYS_CHIP_ATTR_S *pstChipAttr);

/**
@brief Performs global system configuration. CNcomment: ����ϵͳ��ȫ������ CNend
@attention \n
N/A CNcomment: �� CNend
@param[in] pstSysConf Pointer to the address for system configuration CNcomment: ָ�����ͣ�ϵͳ����ָ���ַ�� CNend
@retval ::HI_SUCCESS Success CNcomment: �ɹ� CNend
@retval ::HI_FAILURE Calling this API fails. CNcomment: APIϵͳ����ʧ�� CNend
@see \n
N/A CNcomment: �� CNend
*/
HI_S32 HI_SYS_SetConf(const HI_SYS_CONF_S *pstSysConf);

/**
@brief Obtains global system configuration. CNcomment: ��ȡϵͳ��ȫ������ CNend
@attention \n
N/A CNcomment: �� CNend
@param[out] pstSysConf Pointer to the system configuration (output). CNcomment: ָ�����ͣ����ϵͳ���á� CNend
@retval ::HI_SUCCESS Success CNcomment: �ɹ� CNend
@retval ::HI_FAILURE Calling this API fails. CNcomment: APIϵͳ����ʧ�� CNend
@see \n
N/A CNcomment: �� CNend
*/
HI_S32 HI_SYS_GetConf(HI_SYS_CONF_S *pstSysConf);

/**
@brief Sets the debugging information level of a module. CNcomment: ����ģ��ĵ�����Ϣ���� CNend
@attention \n
N/A CNcomment: �� CNend
@param[in] enModId Debugging ID of a module CNcomment: ģ��ĵ���ID�� CNend
@param[in] enLogLevel Debugging information level of a module CNcomment: ģ��ĵ�����Ϣ���� CNend
@retval ::HI_SUCCESS Success CNcomment: �ɹ� CNend
@retval ::HI_FAILURE Calling this API fails. CNcomment: APIϵͳ����ʧ�� CNend
@see \n
*/
HI_S32 HI_SYS_SetLogLevel(HI_MOD_ID_E enModId,  HI_LOG_LEVEL_E enLogLevel);

/**
@brief Sets the debugging information file path for U-disk. CNcomment: ������־�洢·�� CNend
@attention \n
N/A CNcomment: �� CNend
@param[in]  pszLogPath Debugging information file path. CNcomment: ��־�Ĵ洢·�� CNend
@retval ::HI_SUCCESS Success CNcomment: �ɹ� CNend
@retval ::HI_FAILURE Calling this API fails. CNcomment: APIϵͳ����ʧ�� CNend
@see \n
*/
HI_S32 HI_SYS_SetLogPath(const HI_CHAR* pszLogPath);

/**
@brief Sets the debugging files(may be stream, YUV data, image...) save path. CNcomment: ���õ����ļ��洢·�� CNend
@attention \n
N/A CNcomment: �� CNend
@param[in]  pszPath Debugging files path. CNcomment: �����ļ��Ĵ洢·�� CNend
@retval ::HI_SUCCESS Success CNcomment: �ɹ� CNend
@retval ::HI_FAILURE Calling this API fails. CNcomment: APIϵͳ����ʧ�� CNend
@see \n
*/
HI_S32 HI_SYS_SetStorePath(const HI_CHAR* pszPath);

/**
@brief Writes to a register or a memory. CNcomment:  д�Ĵ������ڴ� CNend
@attention \n
N/A CNcomment: �� CNend
@param[in] u32RegAddr Physical address of a register or a memory CNcomment: �Ĵ������ڴ�������ַ�� CNend
@param[in] u32Value Value of a register CNcomment:  �Ĵ�����ֵ�� CNend
@retval ::HI_SUCCESS Success CNcomment: �ɹ� CNend
@retval ::HI_FAILURE Calling this API fails. CNcomment: APIϵͳ����ʧ�� CNend
@see \n
N/A CNcomment: �� CNend
*/
HI_S32 HI_SYS_WriteRegister(HI_U32 u32RegAddr, HI_U32 u32Value);

/**
@brief Reads a register or a memory. CNcomment: ���Ĵ������ڴ� CNend
@attention \n
N/A CNcomment: �� CNend
@param[in] u32RegAddr Physical address of a register or a memory CNcomment: �Ĵ������ڴ�������ַ�� CNend
@param[out] pu32Value Pointer to the register value (output) CNcomment:  ָ�����ͣ�����Ĵ�����ֵ�� CNend
@retval ::HI_SUCCESS Success CNcomment: �ɹ� CNend
@retval ::HI_FAILURE Calling this API fails. CNcomment: APIϵͳ����ʧ�� CNend
@see \n
N/A CNcomment: �� CNend
*/
HI_S32 HI_SYS_ReadRegister(HI_U32 u32RegAddr, HI_U32 *pu32Value);

/**
@brief Map registers address. CNcomment:  ӳ��Ĵ�����ַ CNend
@attention \n
N/A CNcomment: �� CNend
@param[in] u32RegAddr The start physical address of registers. CNcomment: �Ĵ�������ʼ�����ַ�� CNend
@param[in] u32Length  Length of the registers want to map CNcomment:   �Ĵ������ȡ� CNend
@param[out] pVirAddr  Virtual address CNcomment:   ӳ���������ַ�� CNend
@retval ::HI_SUCCESS Success CNcomment: �ɹ� CNend
@retval ::HI_FAILURE Calling this API fails. CNcomment: APIϵͳ����ʧ�� CNend
@see \n
N/A CNcomment: �� CNend
*/
HI_S32 HI_SYS_MapRegister(HI_U32 u32RegAddr, HI_U32 u32Length, HI_VOID *pVirAddr);

/**
@brief Unmap registers address. CNcomment:  ���Ĵ�����ַӳ�� CNend
@attention \n
N/A CNcomment: �� CNend
@param[in] pVirAddr The virtual address to be unmapped CNcomment: Ҫ���ӳ��������ַ�� CNend
@retval ::HI_SUCCESS Success CNcomment: �ɹ� CNend
@retval ::HI_FAILURE Calling this API fails. CNcomment: APIϵͳ����ʧ�� CNend
@see \n
N/A CNcomment: �� CNend
*/
HI_S32 HI_SYS_UnmapRegister(HI_VOID *pVirAddr);

/**
@brief Get timestamp. CNcomment: ��ȡʱ����� CNend
@attention \n
N/A CNcomment: �� CNend
@param[out] pu32TimeMs Pointer to the timestamp value (output) CNcomment: ���ʱ����� CNend
@retval ::HI_SUCCESS Success CNcomment: �ɹ� CNend
@retval ::HI_FAILURE Calling this API fails. CNcomment: APIϵͳ����ʧ�� CNend
@see \n
N/A CNcomment: �� CNend
*/
HI_S32 HI_SYS_GetTimeStampMs(HI_U32 *pu32TimeMs);

/**
@brief Applies for a media memory zone (MMZ) and maps the user-state address.
CNcomment:  ����mmz�ڴ棬��ӳ���û�̬��ַ CNend
@attention \n
N/A CNcomment: �� CNend
@param[in/out] pstBuf Structure of the buffer information. Bufname and bufsize are inputs, and the physical address and user-state virtual address are outputs.
                    CNcomment: buffer��Ϣ�ṹ��bufname��bufsize��Ϊ����,�����ַ���û�̬�����ַ��Ϊ��� CNend
@retval ::HI_SUCCESS Success CNcomment: �ɹ� CNend
@retval ::HI_FAILURE Calling this API fails. CNcomment: APIϵͳ����ʧ�� CNend
@see \n
N/A CNcomment: �� CNend
*/
HI_S32 HI_MMZ_Malloc(HI_MMZ_BUF_S *pstBuf);

/**
@brief Unmaps the user-state address and releases the MMZ. CNcomment: ����û�̬��ַ��ӳ�䣬���ͷ�mmz�ڴ� CNend
@attention \n
Ensure that the lengths of the transferred physical address and user-state virtual address are correct.
CNcomment: ��֤����������ַ���û�̬�����ַ�ͳ�����ȷ CNend
@param[in] pstBuf Structure of the buffer information CNcomment: buffer��Ϣ�ṹ CNend
@retval ::HI_SUCCESS Success CNcomment: �ɹ� CNend
@retval ::HI_FAILURE Calling this API fails. CNcomment: APIϵͳ����ʧ�� CNend
@see \n
N/A CNcomment: �� CNend
*/
HI_S32 HI_MMZ_Free(HI_MMZ_BUF_S *pstBuf);

/**
@brief pplies for an MMZ with a specified name and obtains its physical address. CNcomment: ָ��mmz����������mmz�ڴ棬���������ַ CNend
@attention \n
N/A CNcomment: �� CNend
@param[in] u32Size Buffer size CNcomment: buffer��С CNend
@param[in] u32Align Alignment mode CNcomment: ���뷽ʽ CNend
@param[in] ps8MMZName Name of an MMZ in the buffer. If the MMZ name is set to NULL, an MMZ is anonymously applied for. CNcomment: buffer��������֣�����NULL�������� CNend
@param[in] ps8MMBName Buffer name CNcomment: buffer������� CNend
@retval ::NULL The application fails. CNcomment: ����ʧ�� CNend
@retval Physical address CNcomment: �����ַ CNend
@see \n
N/A CNcomment: �� CNend
*/
HI_VOID *HI_MMZ_New(HI_U32 u32Size , HI_U32 u32Align, HI_CHAR *ps8MMZName, HI_CHAR *ps8MMBName);


/**
@brief Releases an MMZ based on its physical address. CNcomment: ͨ�������ַ�ͷ�mmz�ڴ� CNend
@attention \n
N/A CNcomment: �� CNend
@param[in] u32PhysAddr Physical address of a buffer CNcomment: buffer�����ַ CNend
@retval ::HI_SUCCESS Success CNcomment: �ɹ� CNend
@retval ::HI_FAILURE Calling this API fails. CNcomment: APIϵͳ����ʧ�� CNend
@see \n
N/A CNcomment: �� CNend
*/
HI_S32 HI_MMZ_Delete(HI_U32 u32PhysAddr);

/**
@brief Maps the physical address of an MMZ applied for to a user-state virtual address. You can determine whether to cache the address.
CNcomment: ��mmz����������ַӳ����û�̬�����ַ������ָ���Ƿ�cached CNend
@attention \n
N/A CNcomment: �� CNend
@param[in] u32PhysAddr Physical address of a buffer CNcomment: buffer�����ַ CNend
@param[in] u32Cached Whether to cache the address. 0: no; 1: yes CNcomment: �Ƿ�ʹ��cache��0��ʹ�ã�1ʹ�� CNend
@retval ::NULL The application fails. CNcomment: ����ʧ�� CNend
@retval User-state virtual address CNcomment: �û�̬���ַ CNend
@see \n
N/A CNcomment: �� CNend
*/
HI_VOID *HI_MMZ_Map(HI_U32 u32PhysAddr, HI_U32 u32Cached);

/**
@brief Unmaps the user-state address of an MMZ. CNcomment: ���mmz�ڴ��û�̬��ַ��ӳ�� CNend
@attention \n
N/A CNcomment: �� CNend
@param[in] u32PhysAddr Physical address of a buffer CNcomment: buffer�����ַ CNend
@retval ::HI_SUCCESS Success CNcomment: �ɹ� CNend
@retval ::HI_FAILURE Calling this API fails. CNcomment: APIϵͳ����ʧ�� CNend
@see \n
N/A CNcomment: �� CNend
*/
HI_S32 HI_MMZ_Unmap(HI_U32 u32PhysAddr);


/**
@brief Flushes D-cache to the cached MMZ. CNcomment: ����cached����MMZ��ˢDcache���ڴ� CNend
@attention \n
If the value 0 is transferred, all D-caches are refreshed; otherwise, only the transferred memory is refreshed.
CNcomment: �����0����ˢ�����е�Dcache������ֻˢ������ǿ��ڴ� CNend
@param[in] u32PhysAddr Physical address of a buffer CNcomment: buffer�����ַ CNend
@retval ::HI_SUCCESS Success CNcomment: �ɹ� CNend
@retval ::HI_FAILURE Calling this API fails. CNcomment: APIϵͳ����ʧ�� CNend
@see \n
N/A CNcomment: �� CNend
*/
HI_S32 HI_MMZ_Flush(HI_U32 u32PhysAddr);


/**
@brief Maps a physical address to a user-state virtual address. CNcomment: �������ַӳ����û�̬�����ַ CNend
@attention \n
N/A CNcomment: �� CNend
@param[in] u32PhyAddr Physical address of a buffer CNcomment: buffer�����ַ CNend
@param[in] u32Size Buffer size CNcomment: buffer�Ĵ�С CNend
@retval ::NULL The application fails. CNcomment: ����ʧ�� CNend
@retval User-state virtual address CNcomment: �û�̬���ַ CNend
@see \n
N/A CNcomment: �� CNend
*/
HI_VOID *HI_MEM_Map(HI_U32 u32PhyAddr, HI_U32 u32Size);


/**
@brief Unmaps a user-state address. CNcomment: ����û�̬��ַ��ӳ�� CNend
@attention \n
N/A CNcomment: �� CNend
@param[in] pAddrMapped User-state virtual address of a buffer. CNcomment: buffer���û�̬���ַ CNend
@retval ::HI_SUCCESS Success CNcomment: �ɹ� CNend
@retval ::HI_FAILURE Calling this API fails. CNcomment: APIϵͳ����ʧ�� CNend
@see \n
N/A CNcomment: �� CNend
*/
HI_S32 HI_MEM_Unmap(HI_VOID *pAddrMapped);

/**
@brief Obtains the physical address and size based on the virtual address. CNcomment: ��������ַ��ȡ�����ַ���Լ���С CNend
@attention \n
N/A CNcomment: �� CNend
@param[in] pVir User-state virtual address CNcomment: �û�̬���ַ CNend
@param[out] pu32Phyaddr Physical address  CNcomment: �����ַ CNend
@param[out] pu32Size Size CNcomment: ��С CNend
@retval ::HI_SUCCESS Success CNcomment: �ɹ� CNend
@retval ::HI_FAILURE Calling this API fails. CNcomment: APIϵͳ����ʧ�� CNend
@see \n
N/A CNcomment: �� CNend
*/
HI_S32 HI_MMZ_GetPhyaddr(HI_VOID * pVir, HI_U32 *pu32Phyaddr, HI_U32 *pu32Size);

/**
@brief Register one module to manager. CNcomment:ģ��ע�ᣬ���ڹ��� CNend
@attention Before manager someone module, calling this interface. CNcomment:�������ģ�飬�ô˽ӿ���ע�� CNend
@param[in] pszModuleName The module name CNcomment:ģ����� CNend
@param[in] u32ModuleID   The module ID. CNcomment:ģ��ID CNend
@retval ::HI_SUCCESS Success CNcomment:�ɹ� CNend
@retval ::HI_FAILURE Failure CNcomment:ʧ�� CNend
@see \n
N/A CNcomment:�� CNend
*/
HI_S32 HI_MODULE_Register(HI_U32 u32ModuleID, const HI_CHAR * pszModuleName);

/**
@brief Register one moudle by name. CNcomment:ģ��ע�ᣬID��ϵͳ���� CNend
@attention Before manager someone module, calling this interface. CNcomment:�������ģ�飬�ô˽ӿ���ע�� CNend
@param[in] pszModuleName The module name CNcomment:ģ����� CNend
@param[out] pu32ModuleID The module id allocated by system. CNcomment:ϵͳ�����ģ��ID CNend
@retval ::HI_SUCCESS Success CNcomment:�ɹ� CNend
@retval ::HI_FAILURE Failure CNcomment:ʧ�� CNend
@see \n
N/A CNcomment:�� CNend
*/
HI_S32 HI_MODULE_RegisterByName(const HI_CHAR * pszModuleName, HI_U32* pu32ModuleID);

/**
@brief UnRegister one module to trace. CNcomment:ģ���Ƴ� CNend
@attention Before stopping to manage someone module, calling this interface. CNcomment:����Ҫ�����ģ��ʱ��ʹ�ô˽ӿ��Ƴ�ģ�� CNend
@param[in] u32ModuleID The module ID. CNcomment:ģ��ID CNend
@param[out] None CNcomment:�� CNend
@retval ::HI_SUCCESS Success CNcomment:�ɹ� CNend
@retval ::HI_FAILURE Failure CNcomment:ʧ�� CNend
@see \n
N/A CNcomment:�� CNend
*/
HI_S32 HI_MODULE_UnRegister(HI_U32 u32ModuleID);

/**
@brief User mode proc cretea directory. CNcomment:�û�̬proc����Ŀ¼ CNend
@attention You need register module before calling this API. Only support create one level directory. CNcomment:��Ҫ��ע��ģ�飬ֻ֧�ִ���һ��Ŀ¼ CNend
@param[in] pszName The directory name. CNcomment:Ŀ¼�� CNend
@param[out] None CNcomment:�� CNend
@retval ::HI_SUCCESS Success CNcomment:�ɹ� CNend
@retval ::HI_FAILURE Failure CNcomment:ʧ�� CNend
@see \n
N/A CNcomment:�� CNend
*/
HI_S32 HI_PROC_AddDir(const HI_CHAR *pszName);

/**
@brief User mode proc remove directory. CNcomment:�û�̬procɾ��Ŀ¼ CNend
@attention It will return fail if there are entries in the directory. CNcomment:���Ŀ¼�»�������ļ�,����ɾ��ʧ�� CNend
@param[in] pszName The directory name. CNcomment:Ŀ¼�� CNend
@retval ::HI_SUCCESS Success CNcomment:�ɹ� CNend
@retval ::HI_FAILURE Failure CNcomment:ʧ�� CNend
@see \n
N/A CNcomment:�� CNend
*/
HI_S32 HI_PROC_RemoveDir(const HI_CHAR *pszName);

/**
@brief User mode proc add entry. CNcomment:�û�̬proc������� CNend
@attention \n
N/A CNcomment: �� CNend
@param[in] u32ModuleID Module ID. CNcomment:ģ��ID CNend
@param[in] pstEntry Parameter of entry. CNcomment:������ڲ��� CNend
@retval ::HI_SUCCESS Success CNcomment:�ɹ� CNend
@retval ::HI_FAILURE Failure CNcomment:ʧ�� CNend
@see \n
N/A CNcomment:�� CNend
*/
HI_S32 HI_PROC_AddEntry(HI_U32 u32ModuleID, const HI_PROC_ENTRY_S* pstEntry);

/**
@brief User mode proc remove entry. CNcomment:�û�̬procɾ����� CNend
@attention \n
N/A CNcomment: �� CNend
@param[in] u32ModuleID Module ID. CNcomment:ģ��ID CNend
@param[in] pstEntry Parameter of entry. CNcomment:ɾ����ڲ��� CNend
@retval ::HI_SUCCESS Success CNcomment:�ɹ� CNend
@retval ::HI_FAILURE Failure CNcomment:ʧ�� CNend
@see \n
N/A CNcomment:�� CNend
*/
HI_S32 HI_PROC_RemoveEntry(HI_U32 u32ModuleID, const HI_PROC_ENTRY_S* pstEntry);

/**
@brief User mode proc print function. CNcomment:�û�̬proc��ӡ���ݵĺ��� CNend
@attention \n
N/A CNcomment: �� CNend
@param[in] pstBuf Output buffer parameter. CNcomment:���buffer���� CNend
@param[in] pFmt   Format parameter. CNcomment:��ӡ��ʽ������ CNend
@retval ::HI_SUCCESS Success CNcomment:�ɹ� CNend
@retval ::HI_FAILURE Failure CNcomment:ʧ�� CNend
@see \n
N/A CNcomment:�� CNend
*/
HI_S32 HI_PROC_Printf(HI_PROC_SHOW_BUFFER_S *pstBuf, const HI_CHAR *pFmt, ...);

/**
@brief malloc the pointed size from system heap. CNcomment:��ϵͳ�з���ָ����С���ڴ� CNend
@attention None CNcomment:�� CNend
@param[in] u32ModuleID The module ID, who need to request memory. CNcomment:ģ��ID CNend
@param[in] u32Size The size of requesting. CNcomment:�������Ĵ�С����λ���ֽ� CNend
@param[out] None CNcomment:�� CNend
@retval ::Valid memory address Success CNcomment:�ɹ����ط��䵽�Ŀռ��׵�ַ CNend
@retval ::NULL Failure CNcomment:ʧ�ܷ���NULL CNend
@see \n
N/A CNcomment:�� CNend
*/
HI_VOID* HI_MEM_Malloc(HI_U32 u32ModuleID, HI_U32 u32Size);


/**
@brief Free the requsted memory by hi_malloc. CNcomment:�ͷŷ�����ڴ� CNend
@attention when stopping to use the memory, calling this interface. CNcomment:������Ҫ����ڴ�ʱ��ʹ�ô˽ӿڽ����ͷ� CNend
@param[in] u32ModuleID The module ID, who need to free memory. CNcomment:ģ��ID CNend
@param[in] pMemAddr The memory address to free CNcomment:�ͷſռ���׵�ַ CNend
@param[out] None CNcomment:�� CNend
@retval ::None CNcomment:�� CNend
@see \n
N/A CNcomment:�� CNend
*/
HI_VOID HI_MEM_Free(HI_U32 u32ModuleID, HI_VOID* pMemAddr);

/**
@brief Calloc memory, with u32MemBlock blocks and u32Size size per. CNcomment:���������С���ڴ�� CNend
@attention None CNcomment:�� CNend
@param[in] u32ModuleID The module id, who need to calloc memory. CNcomment:ģ��ID CNend
@param[in] u32MemBlock The requesting block number. CNcomment:����Ŀ��� CNend
@param[in] u32Size The requesting size per block. CNcomment:ÿ��Ĵ�С����λ���ֽ� CNend
@param[out] None CNcomment:�� CNend
@retval ::Valid memory address Success CNcomment:�ɹ��򷵻ط��䵽���ڴ��׵�ַ CNend
@retval ::NULL Failure CNcomment:ʧ�ܷ���NULL CNend
@see \n
N/A CNcomment: �� CNend
*/
HI_VOID* HI_MEM_Calloc(HI_U32 u32ModuleID, HI_U32 u32MemBlock, HI_U32 u32Size);

/**
@brief Realloc memory CNcomment:���·���ռ� CNend
@attention None CNcomment:�� CNend
@param[in] pMemAddr The memory address, which has been requested from system heap. CNcomment:Ҫ�ı���ڴ��С�ĵ�ַ CNend
@param[in] u32Size The newer memory size to request. CNcomment:�µ��ڴ�ռ��С����λ�ֽ� CNend
@param[out] None CNcomment:�� CNend
@retval ::Valid memory address Success CNcomment:�ɹ��򷵻��µ��ڴ�ռ��׵�ַ CNend
@retval ::NULL Failure CNcomment:ʧ���򷵻�NULL. CNend
@see \n
N/A CNcomment:�� CNend
*/
HI_VOID* HI_MEM_Realloc(HI_U32 u32ModuleID, HI_VOID *pMemAddr, HI_U32 u32Size);


#ifdef MMZ_V2_SUPPORT
/**
@brief pplies for an MMZ with a specified name and obtains its physical address. CNcomment: ָ��mmz����������mmz�ڴ棬���������ַ CNend
@attention \n
N/A CNcomment: �� CNend
@param[in] u32Size Buffer size CNcomment: buffer��С CNend
@param[in] u32Align Alignment mode CNcomment: ���뷽ʽ CNend
@param[in] ps8MMZName Name of an MMZ in the buffer. If the MMZ name is set to NULL, an MMZ is anonymously applied for. CNcomment: buffer��������֣�����NULL�������� CNend
@param[in] ps8MMBName Buffer name CNcomment: buffer������� CNend
@retval ::NULL The application fails. CNcomment: ����ʧ�� CNend
@retval Physical address CNcomment: �����ַ CNend
@see \n
N/A CNcomment: �� CNend
*/
HI_VOID *HI_MMZ_New_Share(HI_U32 u32Size , HI_U32 u32Align, HI_CHAR *ps8MMZName, HI_CHAR *ps8MMBName);


/**
@brief pplies for an MMZ with a specified name and obtains its physical address. CNcomment: ָ��mmz����������mmz�ڴ棬���������ַ CNend
@attention \n
N/A CNcomment: �� CNend
@param[in] u32Size Buffer size CNcomment: buffer��С CNend
@param[in] u32Align Alignment mode CNcomment: ���뷽ʽ CNend
@param[in] ps8MMZName Name of an MMZ in the buffer. If the MMZ name is set to NULL, an MMZ is anonymously applied for. CNcomment: buffer��������֣�����NULL�������� CNend
@param[in] ps8MMBName Buffer name CNcomment: buffer������� CNend
@retval ::NULL The application fails. CNcomment: ����ʧ�� CNend
@retval Physical address CNcomment: �����ַ CNend
@see \n
N/A CNcomment: �� CNend
*/
HI_VOID *HI_MMZ_New_Shm_Com(HI_U32 u32Size , HI_U32 u32Align, HI_CHAR *ps8MMZName, HI_CHAR *ps8MMBName);


/**
@brief Get physical address and size of chm or com type MMZ. CNcomment: ��ȡshm��com��MMZ�����ַ�ʹ�С CNend
@attention \n
N/A CNcomment: �� CNend
@param[out] pu32PhysAddr Physical address of the buffer CNcomment: buffer�����ַ CNend
@param[out] pu32Size     Size of the buffer             CNcomment: buffer��С CNend
@retval ::HI_SUCCESS Success CNcomment: �ɹ� CNend
@retval ::HI_FAILURE Calling this API fails. CNcomment: APIϵͳ����ʧ�� CNend
@see \n
N/A CNcomment: �� CNend
*/
HI_S32 HI_MMZ_Get_Shm_Com(HI_U32 *pu32PhysAddr, HI_U32 *pu32Size);


/**
@brief Force releases an MMZ based on its physical address. CNcomment: ͨ�������ַǿ���ͷ�mmz�ڴ� CNend
@attention \n
N/A CNcomment: �� CNend
@param[in] u32PhysAddr Physical address of a buffer CNcomment: buffer�����ַ CNend
@retval ::HI_SUCCESS Success CNcomment: �ɹ� CNend
@retval ::HI_FAILURE Calling this API fails. CNcomment: APIϵͳ����ʧ�� CNend
@see \n
N/A CNcomment: �� CNend
*/
HI_S32 HI_MMZ_Force_Delete(HI_U32 u32PhysAddr);


/**
@brief Flushes MMZ. CNcomment: Flush MMZ��� CNend
@attention \n
@param[in] u32PhysAddr Physical address of a buffer CNcomment: buffer�����ַ CNend
@param[in] u32VirtAddr Virtual address of a buffer  CNcomment: buffer�����ַ CNend
@param[in] u32Size     Size of a buffer             CNcomment: buffer�ռ��С CNend
@retval ::HI_SUCCESS Success CNcomment: �ɹ� CNend
@retval ::HI_FAILURE Fail. CNcomment: ʧ�� CNend
@see \n
N/A CNcomment: �� CNend
*/
HI_S32 HI_MMZ_Flush_Dirty(HI_U32 u32PhysAddr, HI_U32 u32VirtAddr, HI_U32 u32Size);


/**
@brief Open MMZ. CNcomment: ��MMZ CNend
@attention \n
@param  None CNcomment: �� CNend
@retval ::HI_SUCCESS Success CNcomment: �ɹ� CNend
@retval ::HI_FAILURE Fail. CNcomment: ʧ�� CNend
@see \n
N/A CNcomment: �� CNend
*/
HI_S32 HI_MMZ_open(HI_VOID);


/**
@brief Close MMZ. CNcomment: �ر�MMZ CNend
@attention \n
@param  None CNcomment: �� CNend
@retval ::HI_SUCCESS Success CNcomment: �ɹ� CNend
@retval ::HI_FAILURE Fail. CNcomment: ʧ�� CNend
@see \n
N/A CNcomment: �� CNend
*/
HI_S32 HI_MMZ_close(HI_VOID);
#endif /* endif MMZ_V2_SUPPORT */


/** @} */ /** <!-- ==== API declaration end ==== */

#endif /* endif __KERNEL__ */

#endif /* __HI_COMMON_H__ */

