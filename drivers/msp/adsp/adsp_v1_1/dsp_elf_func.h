/* ./edmac.h
 *
 * History: 
 *      
 */
 
 
#ifndef __DSP_ELF_H__
#define __DSP_ELF_H__

#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/fcntl.h>
#include <linux/mm.h>
#include <linux/miscdevice.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/system.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/string.h>
#include <linux/dma-mapping.h>
#include <linux/kthread.h>
#include "hi_type.h"


#if 0
#define ELF_PRINT printk
#else
#define ELF_PRINT printk_0
#endif

HI_S32 CopyELFSection(HI_UCHAR *pElfAddr);
HI_S32 CheckELFPaser(HI_UCHAR* pElfAddr);

#endif

