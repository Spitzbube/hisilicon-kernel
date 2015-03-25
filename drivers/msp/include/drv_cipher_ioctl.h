/******************************************************************************

  Copyright (C), 2011-2021, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : drv_cipher_ioctl.h
  Version       : Initial Draft
  Author        : Hisilicon hisecurity team
  Created       : 
  Last Modified :
  Description   : 
  Function List :
  History       :
******************************************************************************/

#ifndef __DRV_CIPHER_IOCTL_H__
#define __DRV_CIPHER_IOCTL_H__

#include "hi_type.h"
#include "hi_unf_cipher.h"
#include "hi_drv_cipher.h"
#include "hi_debug.h"

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */


/* special handle creation parameter */
typedef struct  hiCIPHER_HANDLE_S
{
    HI_HANDLE hCIHandle;
    HI_UNF_CIPHER_ATTS_S stCipherAtts;
}CIPHER_HANDLE_S;

/* device parameter */
typedef struct  hiCIPHER_DATA_S
{
    HI_HANDLE CIHandle;
    HI_U32 ScrPhyAddr;
    HI_U32 DestPhyAddr;
    HI_U32 u32PkgNum;
}CIPHER_DATA_S;

typedef struct hiCIPHER_Config_CTRL
{
    HI_HANDLE CIHandle;
    HI_UNF_CIPHER_CTRL_S CIpstCtrl;
}CIPHER_Config_CTRL;


typedef struct hiCIPHER_PROC_ITEM_S
{
    HI_U32    u32Resv;
}CIPHER_PROC_ITEM_S;

typedef enum
{
    HI_CIPHER_HMAC_KEY_FROM_CA  = 0,
    HI_CIPHER_HMAC_KEY_FROM_CPU = 1,
}CIPHER_HMAC_KEY_FROM_E;

typedef struct hiCIPHER_HASH_DATA_S
{
    HI_HANDLE hHandle;
    HI_UNF_CIPHER_HASH_TYPE_E enShaType;
    HI_U32 u32TotalDataLen;
    HI_U8  *pu8InputData;
    HI_U32 u32InputDataLen;
    /* max padding data (64bytes) | fix data length(8bytes) */
    HI_U8  u8Padding[64+8];
    HI_U32 u32PaddingLen;
    HI_U8  u8HMACKey[16];
    CIPHER_HMAC_KEY_FROM_E enHMACKeyFrom;
    HI_U8  *pu8Output;
}CIPHER_HASH_DATA_S;

/* Ioctl definitions */

#define    CMD_CIPHER_CREATEHANDLE           _IOWR(HI_ID_CIPHER, 0x1, CIPHER_HANDLE_S)
#define    CMD_CIPHER_DESTROYHANDLE          _IOW(HI_ID_CIPHER,  0x2, HI_U32)
#define    CMD_CIPHER_CONFIGHANDLE           _IOW(HI_ID_CIPHER,  0x3, CIPHER_Config_CTRL)
#define    CMD_CIPHER_ENCRYPT                _IOW(HI_ID_CIPHER,  0x4, CIPHER_DATA_S)
#define    CMD_CIPHER_DECRYPT                _IOW(HI_ID_CIPHER,  0x5, CIPHER_DATA_S)
#define    CMD_CIPHER_DECRYPTMULTI           _IOW(HI_ID_CIPHER,  0x6, CIPHER_DATA_S)
#define    CMD_CIPHER_ENCRYPTMULTI           _IOW(HI_ID_CIPHER,  0x7, CIPHER_DATA_S)
#define    CMD_CIPHER_GETRANDOMNUMBER        _IOR(HI_ID_CIPHER,  0x8, HI_U32)
#define    CMD_CIPHER_GETHANDLECONFIG        _IOWR(HI_ID_CIPHER, 0x9, CIPHER_Config_CTRL)
#define    CMD_CIPHER_CALCHASHINIT         _IOWR(HI_ID_CIPHER, 0xa, CIPHER_HASH_DATA_S)
#define    CMD_CIPHER_CALCHASHUPDATE       _IOWR(HI_ID_CIPHER, 0xb, CIPHER_HASH_DATA_S)
#define    CMD_CIPHER_CALCHASHFINAL        _IOWR(HI_ID_CIPHER, 0xc, CIPHER_HASH_DATA_S)
#define    CMD_CIPHER_LOADHDCPKEY            _IOWR(HI_ID_CIPHER, 0xd, HI_DRV_CIPHER_FLASH_ENCRYPT_HDCPKEY_S)


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* End of #ifndef __DRV_CIPHER_IOCTL_H__*/
