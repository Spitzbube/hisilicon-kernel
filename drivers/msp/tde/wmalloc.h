/*CNcomment:****************************************************************************/
/*                Copyright 2009 - 2019, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: wmalloc.h                                                       */
/* Version: 01a head file of mamage memery. CNcomment:�ڴ����ͷ�ļ�*/
/*                                                                           */
/* History:                                                                  */
/* 1. 01a,2009-2-25, w54130 Create this file.                                 */
/*****************************************************************************/
#include "hi_type.h"
#ifndef _TDE_WMALLOC_H_
#define _TDE_WMALLOC_H_

/*CNcomment:*****************************************************************************
*Function: wmeminit
*Description: init memery pool. CNcomment:TDE�ڴ�س�ʼ��
*Input: none
*Output: none
*Return: success/error code. CNcomment:�ɹ�/������
others: none
*******************************************************************************/
HI_S32 wmeminit(void);

/******************************************************************************
*Function: wmalloc
*Description: request memery from memery pool. CNcomment:���ڴ���������ڴ�
*Input: size request size. CNcomment:������ڴ��С
*Output: the pointer of memery that request memery. CNcomment:ָ���ڴ���ָ��
*Return: none
others: none
*******************************************************************************/
HI_VOID *wmalloc(HI_SIZE_T size);

/******************************************************************************
*Function: wfree
*Description: free memery.CNcomment: �ͷ��ڴ�
*Input: ptr  the pointer of memery that need free. CNcomment:ָ���ڴ���ָ��
*Output: none
*Return: success/error code. CNcomment:�ɹ�/������
others: none
*******************************************************************************/
HI_S32 wfree(HI_VOID *ptr);

/******************************************************************************
*Function: wmemterm
*Description: Deinit memery pool. CNcomment:TDE�ڴ��ȥ��ʼ��
*Input: none
*Output: none
*Return: none
others: none
*******************************************************************************/
HI_VOID wmemterm(void);

/******************************************************************************
*Function: wgetphy
*Description: get the physics address of memery. CNcomment:��ȡ�ڴ�������ַ
*Input: ptr  the pointer of memery. CNcomment:ָ���ڴ��ָ��
*Output:  physics address. CNcomment:�����ַ
*Return: none
others: none
*******************************************************************************/
HI_U32 wgetphy(HI_VOID *ptr);

/******************************************************************************
*Function: wgetphy
*Description: get the virtual address of memery. CNcomment:��ȡ�ڴ�������ַ
*Input: phyaddr  physics address. CNcomment:�����ַ
*Output: virtual address. CNcomment:�����ַ
*Return: none
others: none
*******************************************************************************/
HI_VOID * wgetvrt(HI_U32 phyaddr);

/******************************************************************************
*Function: wgetfreenum
*Description:get the number of the free unit. CNcomment:��ȡ�ڴ��ʣ���Unit��Ԫ��
*Input: none
*Output: none
*Return: the number of the free unit. CNcomment:ʣ��ĵ�Ԫ��
others: none
*******************************************************************************/
HI_U32 wgetfreenum(HI_VOID);

struct seq_file * wprintinfo(struct seq_file *page);
#endif
