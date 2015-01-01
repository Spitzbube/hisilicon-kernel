/* kcom.c
*
* Copyright (c) 2006 Hisilicon Co., Ltd. 
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*
*/

#include <linux/kernel.h>

#include <linux/version.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/fcntl.h>
#include <linux/mm.h>
#include <linux/mman.h>
#include <linux/miscdevice.h>
#include <linux/proc_fs.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/slab.h>
//#include <linux/devfs_fs_kernel.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <mach/hardware.h>
#include <asm/io.h>
#include <asm/system.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/spinlock.h>
#include <linux/vmalloc.h>

#include <linux/string.h>
#include <linux/list.h>

#include <linux/time.h>

#include "drv_media_mem.h"
#include "drv_mmz.h"


// mmz over cma can't  be created runtime 
#if 0
kcom_mmz_t *new_zone(const char *name, unsigned long phys_start, unsigned long size)
{
	hil_mmz_t *mmz;
	
	mmz = hil_mmz_create(name, 0, phys_start, size);
	if(mmz ==NULL)
		return NULL;
	if(hil_mmz_register(mmz) !=0) {
		hil_mmz_destroy(mmz);
		return NULL;
	}

	return (kcom_mmz_t *)mmz;
}

#ifndef CFG_MMZ_MINI
kcom_mmz_t *new_zone_v2(const char *name, unsigned long phys_start, unsigned long size,
			unsigned int alloc_type, unsigned long block_align)
{
	hil_mmz_t *mmz;
	
	mmz = hil_mmz_create_v2(name, 0, phys_start, size, alloc_type, block_align);
	if(mmz ==NULL)
		return NULL;
	if(hil_mmz_register(mmz) !=0) {
		hil_mmz_destroy(mmz);
		return NULL;
	}

	return (kcom_mmz_t *)mmz;
}
#endif

void delete_zone(kcom_mmz_t *zone)
{
	hil_mmz_unregister((kcom_mmz_t *)zone);
	hil_mmz_destroy((kcom_mmz_t *)zone);
}
#endif 
mmb_addr_t new_mmb(const char *name, int size, unsigned long align, 
		const char *zone_name)
{
	hil_mmb_t *mmb;
	
	mmb = hil_mmb_alloc(name, size, align, 0, zone_name);
	if(mmb ==NULL)
		return MMB_ADDR_INVALID;

	return (mmb_addr_t)hil_mmb_phys(mmb);
}

#ifndef CFG_MMZ_MINI
mmb_addr_t new_mmb_v2(const char *name, int size, unsigned long align, 
		const char *zone_name, unsigned int order)
{
	hil_mmb_t *mmb;
	
	mmb = hil_mmb_alloc_v2(name, size, align, 0, zone_name, order);
	if(mmb ==NULL)
		return MMB_ADDR_INVALID;

	return (mmb_addr_t)hil_mmb_phys(mmb);
}
#endif

void delete_mmb(mmb_addr_t addr)
{
	hil_mmb_t *mmb;

	mmb = hil_mmb_getby_phys((unsigned long)addr);
	if(mmb ==NULL)
		return;

	hil_mmb_free(mmb);
}

void *remap_mmb(mmb_addr_t addr)
{
	hil_mmb_t *mmb;

	mmb = hil_mmb_getby_phys((unsigned long)addr);
	if(mmb ==NULL)
		return NULL;

	return hil_mmb_map2kern(mmb);
}

void *remap_mmb_cached(mmb_addr_t addr)
{
	hil_mmb_t *mmb;

	mmb = hil_mmb_getby_phys((unsigned long)addr);
	if(mmb ==NULL)
		return NULL;

	return hil_mmb_map2kern_cached(mmb);
}

void *	refer_mapped_mmb(void *mapped_addr)
{
	hil_mmb_t *mmb;

	mmb = hil_mmb_getby_kvirt(mapped_addr);
	if(mmb ==NULL)
		return NULL;

	if(mmb->flags & HIL_MMB_MAP2KERN_CACHED)
		return hil_mmb_map2kern_cached(mmb);
	else
		return hil_mmb_map2kern(mmb);
}

int unmap_mmb(void *mapped_addr)
{
	hil_mmb_t *mmb;

	mmb = hil_mmb_getby_kvirt(mapped_addr);
	if(mmb ==NULL)
		return -1;

	return hil_mmb_unmap(mmb);
}

#if 0
mmb_addr_t mapped_to_mmb(void *mapped_addr)
{
	hil_mmb_t *mmb;

	mmb = hil_mmb_getby_kvirt(mapped_addr);
	if(mmb ==NULL)
		return MMB_ADDR_INVALID;

	return hil_mmb_phys(mmb);
}
#endif

int get_mmb(mmb_addr_t addr)
{
	hil_mmb_t *mmb;

	mmb = hil_mmb_getby_phys((unsigned long)addr);
	if(mmb ==NULL)
		return -1;

	return hil_mmb_get(mmb);
}

int put_mmb(mmb_addr_t addr)
{
	hil_mmb_t *mmb;

	mmb = hil_mmb_getby_phys((unsigned long)addr);
	if(mmb ==NULL)
		return -1;

	return hil_mmb_put(mmb);
}

/*
** Input is physaddr, is allocated by MMZ. 
** Maybe this addr is not the original base physical address.
** return value is the real virsual addres
** the original base visual address can be get from VBaddr.
** *Outoffset is offset between VBaddr and 
**User should use "viraddr + *Outoffset" as the real virsual address.
*/
void *remap_mmb_2(mmb_addr_t phyaddr, unsigned long *VBaddr, unsigned long *Outoffset)
{
    void *viraddr = 0, *virrealaddr;
	hil_mmb_t *mmb;
    
//    printk("remap_mmb_2:0x%x\n", phyaddr);
    *Outoffset = 0;
	mmb = hil_mmb_getby_phys_2((unsigned long)phyaddr, Outoffset);
	if(mmb ==NULL)
		return NULL;

	viraddr = hil_mmb_map2kern(mmb);
	if(viraddr ==NULL)
		return NULL;  

    VBaddr      = viraddr;
    virrealaddr = viraddr + *Outoffset;
    return virrealaddr;    
}
/*
**mapped_addr may not be original base visual address.
**original base visual address should be:(mapped_VBaddr - offset)
*/
int unmap_mmb_2(void *mapped_addr, unsigned long offset)
{
	hil_mmb_t *mmb;

    if (mapped_addr == NULL)
        return -1;
	mmb = hil_mmb_getby_kvirt((mapped_addr - offset));
	if(mmb ==NULL)
		return -1;

	return hil_mmb_unmap(mmb);
}


#if 0
EXPORT_SYMBOL(new_zone);
EXPORT_SYMBOL(delete_zone);
#endif
EXPORT_SYMBOL(new_mmb);
EXPORT_SYMBOL(delete_mmb);
EXPORT_SYMBOL(remap_mmb);
EXPORT_SYMBOL(remap_mmb_cached);
EXPORT_SYMBOL(refer_mapped_mmb);
EXPORT_SYMBOL(unmap_mmb);

EXPORT_SYMBOL(get_mmb);
EXPORT_SYMBOL(put_mmb);

EXPORT_SYMBOL(remap_mmb_2);
EXPORT_SYMBOL(unmap_mmb_2);
