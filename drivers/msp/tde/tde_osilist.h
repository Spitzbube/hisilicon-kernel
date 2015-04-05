/*****************************************************************************
*             Copyright 2006 - 2050, Hisilicon Tech. Co., Ltd.
*                           ALL RIGHTS RESERVED
* FileName: tde_os_listi.h
* Description:TDE osi list interface define
*
* History:
* Version   Date          Author        DefectNum       Description
*
*****************************************************************************/

#ifndef _TDE_OSILIST_H_
#define _TDE_OSILIST_H_

#ifdef __cplusplus
 #if __cplusplus
extern "C" {
 #endif /* __cplusplus */
#endif  /* __cplusplus */

#include <linux/list.h>
#include "tde_define.h"
#include "hi_tde_type.h"


/****************************************************************************/
/*                        TDE osi list types define                         */
/****************************************************************************/

/* Data struct of software list node */
typedef struct hiTDE_SWNODE_S
{
    struct list_head stList;    
    HI_S32 s32Handle;           		/* Job handle of the instruct */
    HI_S32 s32Index;            		/* Instruct serial number in job, form one on start, the same number is the same instruct */
    TDE_NODE_SUBM_TYPE_E enSubmType; 	/* current node type */
    TDE_NOTIFY_MODE_E enNotiType;   	/* Notice type after node completed */
    TDE_NODE_BUF_S stNodeBuf;       	/* Node of operate config */
    HI_U32 u32PhyBuffNum;           	/* Number of physical buffer distributed */
    struct hiTDE_SWNODE_S *pstParentNodeInCmd;	/* The head software node of this instruct */
} TDE_SWNODE_S;

/* Job definition */
typedef struct hiTDE_SWJOB_S
{
    struct list_head stList;    
    HI_S32 s32Handle;               /* Job handle */
    TDE_FUNC_CB       pFuncComplCB; /* Pointer of callback fuction */
    HI_VOID * pFuncPara;            /* Arguments of callback function */
    TDE_NOTIFY_MODE_E enNotiType;   /* Notice type after node completed */
    HI_U32 u32CmdNum;               /* Instruct number of job */
    HI_U32 u32NodeNum;              /* Node number of job */
    TDE_SWNODE_S *pstFirstCmd;      /* Software node of first instruct in job */
    TDE_SWNODE_S *pstLastCmd;       /* Software node of last instruct in job  */
    TDE_SWNODE_S *pstTailNode;      /* Last software node of job */
    wait_queue_head_t stQuery;      /* Wait queue used in query */
    HI_BOOL bInQuery;               /* If user is  quering or not */
    HI_BOOL bSubmitted;             /* If have submitted */
    HI_BOOL bAqUseBuff;             /* If using temporary buffer */
    HI_U8   u8WaitForDoneCount;     /* wait job count */    
    TDE_SWNODE_S *pstParentCmd;     /* Record parent node */
}TDE_SWJOB_S;

/****************************************************************************/
/*                             TDE osi list functions define                */
/****************************************************************************/

/*****************************************************************************
* Function:      TdeOsiSuspListInit
* Description:   Initialize all lists inside software to use as TDE operation cache
* Input:         null
* Output:        none
* Return:        Success/fail
* Others:        none
*****************************************************************************/
HI_S32      TdeOsiListInit(HI_VOID);

/*****************************************************************************
* Function:      TdeOsiListTerm
* Description:   Release all lists inside software to use as TDE operation cache
* Input:         NULL
* Output:        none 
* Return:        Success/fail
* Others:        none
*****************************************************************************/
HI_VOID     TdeOsiListTerm(HI_VOID);

/*****************************************************************************
* Function:      TdeOsiListBeginJob
* Description:   Create list, return head pointer of list
* Input:         none
* Output:        none
* Return:        create task handle 
* Others:        none
*****************************************************************************/
HI_S32      TdeOsiListBeginJob(TDE_HANDLE *pHandle );

/*****************************************************************************
* Function:      TdeOsiListCancelJob
* Description:   Delete list of s32Handle point
* Input:         s32Handle: the list wait for delete
* Output:        none
* Return:        none
* Others:        none
*****************************************************************************/
HI_S32      TdeOsiListCancelJob(TDE_HANDLE s32Handle);

/*****************************************************************************
* Function:      TdeOsiListCalcJobNum
* Description:   Get operate numbet of current task
* Input:         s32Handle:task handle
* Output:        none
* Return:        >0,node number in task; <0, wrong task
* Others:        AI7D02876 return number of current job command
*****************************************************************************/
HI_S32      TdeOsiListCalcJobNum(TDE_HANDLE s32Handle);

/*****************************************************************************
* Function:      TdeOsiListAddNode
* Description:   create list, return head pointer of list
* Input:         s32Handle: Head pointer of job need operate
*                pSwNode: Config arguments of TDE software node instruct
*                enSubmType: Submit type of current node 
* Output:        None
* Return:        Success/fail
* Others:        None
*****************************************************************************/
HI_S32      TdeOsiListAddNode(TDE_HANDLE s32Handle, TDE_NODE_BUF_S* pSwNode,
                              TDE_NODE_SUBM_TYPE_E enSubmType, HI_U32 u32PhyBuffNumInNode);

/*****************************************************************************
* Function:      TdeOsiListSubmitJob
* Description:   Create list, return head pointer of list
* Input:         s32Handle: Head pointer of job needing submit
*                enListType: submit operate is added into aq or sq?
*                bBlock: if block
*                u32TimeOut: time out
*                pFuncComplCB: callback fuction of complete operate
* Output:        none 
* Return:        success/fail/time out
* Others:        none
*****************************************************************************/
HI_S32      TdeOsiListSubmitJob(TDE_HANDLE s32Handle, TDE_LIST_TYPE_E enListType,
                                HI_U32 u32TimeOut, TDE_FUNC_CB pFuncComplCB, HI_VOID *pFuncPara,
                                TDE_NOTIFY_MODE_E enNotiType);

/*****************************************************************************
 Prototype    : TdeOsiListWaitAllDone
 Description  : wait for all TDE operate completed
 Input        : None
 Output       : NONE
 Return Value : HI_SUCCESS,TDE operate completed
 Calls        :
 Called By    :
*****************************************************************************/
HI_S32 TdeOsiListWaitAllDone(TDE_LIST_TYPE_E enListType);

/*****************************************************************************
 Prototype    : TdeOsiListReset
 Description  : Reset all state, release list space 
 Input        : none
 Output       : NONE
 Return Value : HI_SUCCESS/HI_FAILURE
 Calls        :
 Called By    :
*****************************************************************************/
HI_VOID TdeOsiListReset(HI_VOID);

/*****************************************************************************
 Prototype    : TdeOsiListIsCompleted
 Description  : Query if one instruct in job is completed
 Input        : s32Handle: job handle
                s32Id:instruct index
 Output       : NONE
 Return Value :
 Calls        :
 Called By    :

  History        :
  1.Date         : 2008/3/5
    Author       : w54130
    Modification : Created function

*****************************************************************************/
HI_BOOL TdeOsiListIsCompleted(TDE_HANDLE s32Handle, HI_U32 u32Id);

/*****************************************************************************
* Function:      TdeOsiListWaitForDone
* Description:   Query if submitted TDE operate is completed
* Input:         s32Handle: task handle 
*                u32TimeOut: if Time out 
* Output:        none
* Return:        Success/fail
* Others:        none
*****************************************************************************/
HI_S32 TdeOsiListWaitForDone(TDE_HANDLE s32Handle, HI_U32 u32TimeOut);

/*****************************************************************************
* Function:      TdeOsiListSqUpdateProc
* Description:   Interrupt for handling to update Sq list information
* Input:         none
* Output:        none
* Return:        task handle is created
* Others:        none
*****************************************************************************/
HI_VOID     TdeOsiListSqUpdateProc(HI_VOID);

/*****************************************************************************
* Function:      TdeOsiSqCompProc
* Description:   Interrupt to handling all list operate is completed
* Input:         enListType: ;list type:Sq/Aq
* Output:        none
* Return:        task handle is created
* Others:        none
*****************************************************************************/
HI_VOID     TdeOsiListCompProc(TDE_LIST_TYPE_E enListType);

/*****************************************************************************
* Function:      TdeOsiAqSuspProc
* Description:   Interrupt to handling current line in list is suspend
* Input:         enListType: list type:Sq/Aq
* Output:        none
* Return:        task handle is created
* Others:        none
*****************************************************************************/
HI_VOID     TdeOsiListSuspLineProc(TDE_LIST_TYPE_E enListType);

/*****************************************************************************
* Function:      TdeOsiNodeComp
* Description:
* Input:         TDE_LIST_TYPE_E enListType
* Output:        none
* Return:        task handle is created
* Others:        none
*****************************************************************************/
HI_VOID     TdeOsiListNodeComp(TDE_LIST_TYPE_E enListType);


/*****************************************************************************
* Function:      TdeOsiListGetPhyBuff
* Description:   Get one physical buffer, used in deflicker and zoom
* Input:         
* Output:        none
* Return:        physical address assigned
* Others:        none
*****************************************************************************/
HI_U32  TdeOsiListGetPhyBuff(HI_U32 u32CbCrOffset);


/*****************************************************************************
* Function:      TdeOsiListPutPhyBuff
* Description:   put back physical buffer
* Input:         u32BuffNum 
* Output:        none
* Return:        none
* Others:        none
*****************************************************************************/
HI_VOID  TdeOsiListPutPhyBuff(HI_U32 u32BuffNum);

#ifdef __cplusplus
 #if __cplusplus
}
 #endif /* __cplusplus */
#endif  /* __cplusplus */

#endif  /* _TDE_OSILIST_H_ */
