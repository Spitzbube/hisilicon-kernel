/*CNcomment:****************************************************************************/
/*                Copyright 2009 - 2019, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: wmalloc.h                                                       */
/* Version: 01a head file of mamage memery. CNcomment:内存管理头文件*/
/*                                                                           */
/* History:                                                                  */
/* 1. 01a,2009-2-25, w54130 Create this file.                                 */
/*****************************************************************************/
#include "hi_type.h"
#ifndef _TDE_WMALLOC_H_
#define _TDE_WMALLOC_H_

/*CNcomment:*****************************************************************************
*Function: wmeminit
*Description: init memery pool. CNcomment:TDE内存池初始化
*Input: none
*Output: none
*Return: success/error code. CNcomment:成功/错误码
others: none
*******************************************************************************/
HI_S32 wmeminit(void);

/******************************************************************************
*Function: wmalloc
*Description: request memery from memery pool. CNcomment:从内存池中申请内存
*Input: size request size. CNcomment:申请的内存大小
*Output: the pointer of memery that request memery. CNcomment:指向内存块的指针
*Return: none
others: none
*******************************************************************************/
HI_VOID *wmalloc(size_t size);

/******************************************************************************
*Function: wfree
*Description: free memery.CNcomment: 释放内存
*Input: ptr  the pointer of memery that need free. CNcomment:指向内存块的指针
*Output: none
*Return: success/error code. CNcomment:成功/错误码
others: none
*******************************************************************************/
HI_S32 wfree(HI_VOID *ptr);

/******************************************************************************
*Function: wmemterm
*Description: Deinit memery pool. CNcomment:TDE内存池去初始化
*Input: none
*Output: none
*Return: none
others: none
*******************************************************************************/
HI_VOID wmemterm(void);

/******************************************************************************
*Function: wgetphy
*Description: get the physics address of memery. CNcomment:获取内存的物理地址
*Input: ptr  the pointer of memery. CNcomment:指向内存的指针
*Output:  physics address. CNcomment:物理地址
*Return: none
others: none
*******************************************************************************/
HI_U32 wgetphy(HI_VOID *ptr);

/******************************************************************************
*Function: wgetphy
*Description: get the virtual address of memery. CNcomment:获取内存的虚拟地址
*Input: phyaddr  physics address. CNcomment:物理地址
*Output: virtual address. CNcomment:虚拟地址
*Return: none
others: none
*******************************************************************************/
HI_VOID * wgetvrt(HI_U32 phyaddr);

/******************************************************************************
*Function: wgetfreenum
*Description:get the number of the free unit. CNcomment:获取内存的剩余的Unit单元数
*Input: none
*Output: none
*Return: the number of the free unit. CNcomment:剩余的单元数
others: none
*******************************************************************************/
HI_U32 wgetfreenum(HI_VOID);

struct seq_file * wprintinfo(struct seq_file *page);
#endif
