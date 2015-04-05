#ifndef __TDE_PROC_H__
#define __TDE_PROC_H__

#include "tde_hal.h"

#define TDE_MAX_PROC_NUM 8

HI_VOID TDEProcRecordNode(TDE_HWNode_S* pHWNode);

int tde_read_proc(struct seq_file *p, HI_VOID *v);

int tde_write_proc(struct file * file,
    const char __user * buf, size_t count, loff_t *ppos);  


#endif
