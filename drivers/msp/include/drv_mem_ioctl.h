/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : drv_mem_ioctl.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2006/05/17
  Description   :
  History       :
  1.Date        : 2006/05/17
    Author      : g45345
    Modification: Created file

******************************************************************************/
#ifndef __DRV_MEM_IOCTL_H__
#define __DRV_MEM_IOCTL_H__

#include "hi_type.h"
#include "mpi_mmz.h"
#include "hi_debug.h"
#include "hi_drv_mem.h"

#ifdef __cplusplus
extern "C"{
#endif /* End of #ifdef __cplusplus */

#ifndef __KERNEL_
struct hiMEM_LLI_S
{
    HI_MMZ_BUF_S  buf;
    struct hiMEM_LLI_S  *next;
};

#else
struct hiMEM_LLI_S
{
    HI_MMZ_BUF_S  buf;
    atomic_t    cnt;
    struct hiMEM_LLI_S  *next;
};
#endif

typedef struct hiMEM_LLI_S MEM_LLI_S;

#define MEM_IOCTL(fd...)   ioctl(fd)

#define UMAPC_MEM_MALLOC                _IOWR(HI_ID_MEM,101, HI_MMZ_BUF_S)
#define UMAPC_MEM_FREE                  _IOW (HI_ID_MEM,102, HI_MMZ_BUF_S)

#ifdef __cplusplus
}
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __DRV_MEM_IOCTL_H__ */


