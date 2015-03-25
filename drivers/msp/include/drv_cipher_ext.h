/**
\file
\brief cipher osi
\copyright Shenzhen Hisilicon Co., Ltd.
\date 2008-2018
\version draft
\author QuYaxin 46153
\date 2009-11-8
*/

#ifndef __DRV_CIPHER_EXT_H__
#define __DRV_CIPHER_EXT_H__

/* add include here */
#include "hi_error_mpi.h"
#include "hi_unf_cipher.h"
#include "hi_drv_cipher.h"
#include "drv_cipher_ioctl.h"

#ifdef __cplusplus
extern "C" {
#endif


/*************************** Structure Definition ****************************/

/** */
typedef HI_VOID (*funcCipherCallback)(HI_U32);

/********************** Global Variable declaration **************************/



/******************************* API declaration *****************************/
typedef HI_S32 (*fp_DRV_Cipher_OpenChn)(HI_U32 softChnId);
typedef HI_S32 (*fp_DRV_Cipher_CloseChn)(HI_U32 softChnId);
typedef HI_S32 (*fp_DRV_Cipher_ConfigChn)(HI_U32 softChnId,  HI_UNF_CIPHER_CTRL_S *pConfig, funcCipherCallback fnCallBack);
typedef HI_S32 (*fp_DRV_Cipher_CreatTask)(HI_U32 softChnId, HI_DRV_CIPHER_TASK_S *pTask, HI_U32 *pKey, HI_U32 *pIV);
typedef HI_S32 (*fp_DRV_Cipher_HdcpParamConfig)(HI_DRV_CIPHER_HDCP_KEY_MODE_E enHdcpEnMode, HI_DRV_CIPHER_HDCP_KEY_RAM_MODE_E enRamMode, HI_DRV_CIPHER_HDCP_ROOT_KEY_TYPE_E enKeyType);
typedef HI_S32 (*fp_DRV_Cipher_ClearHdcpConfig)(HI_VOID);
typedef HI_S32 (*fp_DRV_Cipher_SoftReset)(HI_VOID);
typedef HI_S32 (*fp_DRV_Cipher_LoadHdcpKey)(HI_DRV_CIPHER_FLASH_ENCRYPT_HDCPKEY_S *pstFlashHdcpKey);
typedef HI_S32 (*fp_DRV_Cipher_CalcHashInit)(CIPHER_HASH_DATA_S *pCipherHashData);
typedef HI_S32 (*fp_DRV_Cipher_CalcHashUpdate)(CIPHER_HASH_DATA_S *pCipherHashData);
typedef HI_S32 (*fp_DRV_Cipher_CalcHashFinal)(CIPHER_HASH_DATA_S *pCipherHashData);
typedef HI_S32 (*fp_DRV_CIPHER_HashSoftRst)(HI_VOID);

typedef struct s_CIPHER_RegisterFunctionlist
{
	fp_DRV_Cipher_OpenChn DRV_Cipher_OpenChn;
	fp_DRV_Cipher_CloseChn DRV_Cipher_CloseChn;
	fp_DRV_Cipher_ConfigChn DRV_Cipher_ConfigChn;
	fp_DRV_Cipher_CreatTask DRV_Cipher_CreatTask;
	fp_DRV_Cipher_HdcpParamConfig DRV_Cipher_HdcpParamConfig;
	fp_DRV_Cipher_ClearHdcpConfig DRV_Cipher_ClearHdcpConfig;
	fp_DRV_Cipher_SoftReset DRV_Cipher_SoftReset;
	fp_DRV_Cipher_LoadHdcpKey DRV_Cipher_LoadHdcpKey;
    fp_DRV_Cipher_CalcHashInit DRV_Cipher_CalcHashInit;
    fp_DRV_Cipher_CalcHashUpdate DRV_Cipher_CalcHashUpdate;
    fp_DRV_Cipher_CalcHashFinal DRV_Cipher_CalcHashFinal;
    fp_DRV_CIPHER_HashSoftRst DRV_CIPHER_HashSoftRst;
}CIPHER_RegisterFunctionlist_S;

HI_S32	CIPHER_DRV_ModInit(HI_VOID);
HI_VOID	CIPHER_DRV_ModExit(HI_VOID);

#ifdef __cplusplus
}
#endif
#endif /* __DRV_CIPHER_EXT_H__ */

