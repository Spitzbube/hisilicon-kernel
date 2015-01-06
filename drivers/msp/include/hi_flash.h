/******************************************************************************
  Copyright (C), 2004-2050, Hisilicon Tech. Co., Ltd.
******************************************************************************
  File Name     : hi_flash.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       :
  Last Modified :
  Description   : header file for hiflash component
  Function List :
  History       :
  1.Date        :
  Author        :
  Modification  : Created file
******************************************************************************/

/**
 * \file
 * \brief describle the information about hiflash component. CNcomment:提供hiflash组件相关接口、数据结构信息。
 * \attention \n
 * DO NOT write/erase flash too heavily using this hiflash interface because hiflash do not support "load balancing", use filesystems or implement "load balancing" instead if you have to. read is limitless.\n
 *       for example: if max write/erase times of MLC Nand chip is 3000, write/erase any fixed area must not exceed 3000 times, or read data may be wrong. \n\n
 * CNcomment:不要使用hiflash接口频繁write/erase flash，hiflash不支持读写均衡，如果必须要频繁write/erase操作，请使用文件系统，或者根据实际应用场景设计读写均衡算法。read操作无此限制。 \n
 *       比如: 如果MLC Nand的使用寿命为3000次，在产品整个生命周期中，对某个固定区域的write/erase操作不应该超过3000次，否则由于Nand失效读出数据可能错误。
 */

#ifndef __HI_FLASH__H__
#define __HI_FLASH__H__
#include "hi_type.h"

/*************************** Structure Definition ****************************/
/** \addtogroup      FLASH*/
/** @{ */  /** <!-- [FLASH] */

#define FLASH_NAME_LEN 32       /** Flash Name max length */

/** flash type */
typedef enum hiHI_FLASH_TYPE_E
{
    HI_FLASH_TYPE_SPI_0,    /**< SPI flash type */
    HI_FLASH_TYPE_NAND_0,   /**< NAND flash type */
    HI_FLASH_TYPE_EMMC_0,   /**< eMMC flash type */
    HI_FLASH_TYPE_BUTT      /**< Invalid flash type */
} HI_FLASH_TYPE_E;

#define  INVALID_FD -1

#define  HI_FLASH_RW_FLAG_RAW           0x0   /** read and write without OOB,for example: kernel/uboot/ubi/cramfs.. */
#define  HI_FLASH_RW_FLAG_WITH_OOB      0x1   /** read and write with OOB, example: yaffs2 filesystem image */
#define  HI_FLASH_RW_FLAG_ERASE_FIRST   0x2   /** erase before write */

/** this macro for return value when nand flash have bad block or valid length less partition length */
/** CNcomment:当nand有坏块时,读/写/擦除时,有效长度可能小于分区大小/打开长度,此时为了不越界,读/写/擦除完有效长度后,返回该值 */
#define  HI_FLASH_END_DUETO_BADBLOCK    -10

/** Flash partition access permission type */
typedef enum ACCESS_PERM
{
    ACCESS_NONE = 0,
    ACCESS_RD   = (1 << 1),
    ACCESS_WR   = (1 << 2),
    ACCESS_RDWR = (ACCESS_RD | ACCESS_WR),
    ACCESS_BUTT
} HI_FLASH_ACCESS_PERM_E;

/** Flash partition descriptions */
typedef struct hiFlash_PartInfo_S
{
    HI_U64  StartAddr;                  /**< Partiton start address */
    HI_U64  PartSize;                   /**< Partition size */
    HI_U32  BlockSize;                  /**< The Block size of the flash where this partition at */
    HI_FLASH_TYPE_E FlashType;          /**< The flash type where this partition at */
    HI_CHAR DevName[FLASH_NAME_LEN];    /**< The device node name where this partition relate to */
    HI_CHAR PartName[FLASH_NAME_LEN];   /**< The partition name of this partition */
    HI_FLASH_ACCESS_PERM_E perm;        /**< The partition access permission type */
} HI_Flash_PartInfo_S;

/** Flash operation descriptions */
typedef struct tagFLASH_OPT_S
{
    int (*raw_read)(int fd, unsigned long long *startaddr, unsigned char *buffer,
        unsigned long length, unsigned long long openaddr, unsigned long long limit_leng, int read_oob, int skip_badblock);
    int (*raw_write)(int fd, unsigned long long *startaddr, unsigned char *buffer,
        unsigned long length, unsigned long long openaddr, unsigned long long limit_leng, int write_oob);
    int (*raw_erase)(int fd, unsigned long long startaddr,
        unsigned long long length, unsigned long long openaddr, unsigned long long limit_leng);
} FLASH_OPT_S;

/** Flash Infomation */
typedef struct hiFlash_InterInfo_S
{
    HI_U64  TotalSize;                  /**< flash total size */
    HI_U64  PartSize;                   /**< flash partition size */
    HI_U32  BlockSize;                  /**< flash block size */
    HI_U32  PageSize;                   /**< flash page size */
    HI_U32  OobSize;                    /**< flash OOB size */
    HI_S32  fd;                         /**< file handle */ /**<CNcomment:文件句柄(按地址打开不能得到真实句柄)*/
    HI_U64  OpenAddr;                   /**< flash open address */
    HI_U64  OpenLeng;                   /**< flash open length */
    HI_FLASH_TYPE_E FlashType;          /**< flash type */
    FLASH_OPT_S *pFlashopt;             /**< operation callbacks on this flash */
    HI_Flash_PartInfo_S *pPartInfo;     /**< parition descriptions on this flash */
} HI_Flash_InterInfo_S;

/** @} */  /** <!-- ==== Structure Definition end ==== */

/******************************* API declaration *****************************/
/** \addtogroup      FLASH */
/** @{ */  /** <!-- [FLASH] */


/**
\brief: open flash partiton
\attention \n
\param[in] enFlashType      Flash type
\param[in] pPartitionName   CNcomment: 非EMMC器件(如SPI/NAND),只能用/dev/mtdx作为分区名。EMMC器件只能用/dev/mmcblk0px作为分区名。
\param[in] u64Address       open address CNcomment:打开地址(当分区名无效即pPartitionName为空时使用)
\param[in] u64Len           open length CNcomment:打开长度(当分区名无效即pPartitionName为空时使用)
\retval    fd               Flash handle
\retval    INVALID_FD       invaild fd
\see \n
*/
HI_HANDLE HI_Flash_Open(HI_FLASH_TYPE_E enFlashType, HI_CHAR *pPartitionName, HI_U64 u64Address, HI_U64 u64Len);

/**
\brief:  open flash patition by name
\attention \n
\param[in] pPartitionName   patition name CNcomment:非EMMC器件(如SPI/NAND),只能用/dev/mtdx作为分区名。EMMC器件只能用bootargs里blkdevparts=mmcblk0:中设定的分区名字，不能用/dev/mmcblk0px作为分区名
\retval    fd               Flash hande
\retval    INVALID_FD       invaild fd
\see \n
*/
HI_HANDLE HI_Flash_OpenByName(HI_CHAR *pPartitionName);

/**
\brief: open flash patition by  type and name
\attention \n
\param[in] enFlashType      flash type
\param[in] pPartitionName   patition name CNcomment: 非EMMC器件(如SPI/NAND),只能用/dev/mtdx作为分区名。EMMC器件只能用/dev/mmcblk0px作为分区名。
\retval    fd               Flash hande
\retval    INVALID_FD       invaild fd
\see \n
*/
HI_HANDLE HI_Flash_OpenByTypeAndName(HI_FLASH_TYPE_E enFlashType, HI_CHAR *pPartitionName);

/**
\brief: open flash patition by  address
\attention \n
\param[in] enFlashType       flash type
\param[in] u64Address        open address
\param[in] u64Len            open length
\retval    fd                Flash hande
\retval    INVALID_FD        invaild fd
\see \n
*/
HI_HANDLE HI_Flash_OpenByTypeAndAddr(HI_FLASH_TYPE_E enFlashType, HI_U64 u64Address, HI_U64 u64Len);


/**
\brief: close flash partition
\attention \n
\param[in] hFlash    flash handle
\retval ::HI_SUCCESS
\retval ::HI_FAILURE
\see \n
*/
HI_S32 HI_Flash_Close(HI_HANDLE hFlash);

/**
\brief: erase Flash partiton
\attention \n
\param[in] hFlash      flash handle
\param[in] u64Address  erase start address, must align with blocksize  CNcomment:擦写地址，字节为单位，块对齐
\param[in] u32Len      data length, must align with blocksize CNcomment:数据长度，字节为单位，块对齐
\retval ::TotalErase   erase total length
\retval ::HI_FLASH_END_DUETO_BADBLOCK     return value when nand flash have bad block or valid length less partition length CNcomment:擦除Flash遇到坏块结束(擦除完有效长度数据后,返回该值,一般不认为该操作失败)
\retval ::HI_FAILURE   failure
\see \n
*/
HI_S32 HI_Flash_Erase(HI_HANDLE hFlash, HI_U64 u64Address, HI_U64 u64Len);

/**
\brief:  read data from flash
\attention \n
\param[in] hFlash       flash handle
\param[in] u64Address   read start address, for nand, must align with pagesize.
\param[in] pBuf         destination buffer pointer
\param[in] u32Len       destination data length
\param[in] u32Flags     OOB flag  CNcomment:可取值 HI_FLASH_RW_FLAG_WITH_OOB，表示数据内容是否带 OOB 区
\retval ::TotalRead     read flash vaild length
\retval ::HI_FLASH_END_DUETO_BADBLOCK    CNcomment:读Flash遇到坏块结束(读完有效长度数据后,返回该值,由用户判断此种情况成功与否)
\retval ::HI_FAILURE    failure
\see \n
*/
HI_S32 HI_Flash_Read(HI_HANDLE hFlash, HI_U64 u64Address, HI_U8 *pBuf,
                     HI_U32 u32Len, HI_U32 u32Flags);

/**
\brief: write data to flash
\attention \n
1) forbidden used the function when yaffs filesystem is using
2) can use HI_FLASH_RW_FLAG_ERASE_FIRST manner write flah , can write over all partition one time or write with block
CNcomment:1) 不能调用该接口更新当前正使用的yaffs文件系统
CNcomment:2) 调用该接口写Flash时，可以使用HI_FLASH_RW_FLAG_ERASE_FIRST，可以一次写完也可以一块一块的写，但是当写yaffs2时，
    必须先调用HI_Flash_Erase把要写的分区完全擦除
\param[in] hFlash       flash handle
\param[in] u64Address   data start address, for nand, must align with pagesize
\param[in] pBuf         destination buffer pointer
\param[in] u32Len       destination data length, for nand, if write with oob, must align with (pagesize + oobsize)
\param[in] u32Flags     OOB flag CNcomment:可取值 HI_FLASH_RW_FLAG_WITH_OOB，表示数据内容是否带 OOB 区
\retval ::TotalWrite    write flash vaild length
\retval ::HI_FLASH_END_DUETO_BADBLOCK   have bad block CNcomment:写Flash遇到坏块结束(写完有效长度数据后,返回该值,由用户判断此种情况成功与否)
\retval ::HI_FAILURE
\see \n
*/
HI_S32 HI_Flash_Write(HI_HANDLE hFlash, HI_U64 u64Address,
                      HI_U8 *pBuf, HI_U32 u32Len, HI_U32 u32Flags);

/**
\brief: get flash partition info
\attention \n
info content: TotalSize,PartSize,BlockSize,PageSize,OobSize,fd
\param[in] hFlash        flash handle
\param[in] pInterInfo    info struct pointer
\retval ::HI_SUCCESS
\retval ::HI_FAILURE
\see \n
*/
HI_S32 HI_Flash_GetInfo(HI_HANDLE hFlash, HI_Flash_InterInfo_S *pFlashInfo);

/** @} */  /** <!-- ==== API declaration end ==== */

#endif // __HI_FLASH__H__
