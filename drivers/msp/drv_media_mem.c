/* media-mem.c
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
#include <asm/cacheflush.h>
#include <linux/time.h>
#include <asm/setup.h>

#include <asm/memory.h>
#include <linux/dma-contiguous.h>

#include <linux/dma-mapping.h>
#include <asm/memory.h>
#include <asm/highmem.h>
#include <asm/tlbflush.h>
#include <asm/pgtable.h>


#include "drv_media_mem.h"
#include "hi_kernel_adapt.h"

#define DEFAULT_ALLOC 0
#define SLAB_ALLOC 1
#define EQ_BLOCK_ALLOC 2

#define LOW_TO_HIGH 0
#define HIGH_TO_LOW 1

#define MMZ_DBG_LEVEL 0x0
#define mmz_trace(level, s, params...) do { if (level & MMZ_DBG_LEVEL) \
		PRINTK_CA(KERN_INFO "[%s, %d]: " s "\n", \
			__FUNCTION__, __LINE__, params);\
} while (0)
#define mmz_trace_func() mmz_trace(0x02, "%s", __FILE__)

#define MMZ_GRAIN PAGE_SIZE
#define mmz_bitmap_size(p) (mmz_align2(mmz_length2grain((p)->nbytes), 8) / 8)

#define mmz_get_bit(p, n) (((p)->bitmap[(n) / 8] >> ((n) & 0x7)) & 0x1)
#define mmz_set_bit(p, n) (p)->bitmap[(n) / 8] |= 1 << ((n) & 0x7)
#define mmz_clr_bit(p, n) (p)->bitmap[(n) / 8] &= ~(1 << ((n) & 0x7))

#define mmz_pos2phy_addr(p, n) ((p)->phys_start + (n) * MMZ_GRAIN)
#define mmz_phy_addr2pos(p, a) (((a) - (p)->phys_start) / MMZ_GRAIN)

#define mmz_align2low(x, g) (((x) / (g)) * (g))
#define mmz_align2(x, g) ((((x) + (g) - 1) / (g)) * (g))
#define mmz_grain_align(x) mmz_align2(x, MMZ_GRAIN)
#define mmz_length2grain(len) (mmz_grain_align(len) / MMZ_GRAIN)

#define begin_list_for_each_mmz(p, gfp, mmz_name) list_for_each_entry(p, &mmz_list, list) {\
        if (gfp == 0 ? 0 : (p)->gfp != (gfp)) \
            continue;\
        if (mmz_name != NULL) { \
            if ((*mmz_name != '\0') && strcmp(mmz_name, p->name)) \
                continue;\
        } \
        if ((mmz_name == NULL) && (anony == 1)){ \
            if (strcmp("anonymous", p->name)) \
                continue;\
        } \
        mmz_trace(1, HIL_MMZ_FMT_S, hil_mmz_fmt_arg(p));

#define end_list_for_each_mmz() }

char line[COMMAND_LINE_SIZE];

static int mmz_total_size = 0;

int zone_number = 0;
int block_number = 0;

//the following 3 definations are duplicate from kernel 
//because no where to locate them

#define NAME_LEN_MAX	64

struct cma_zone{
	struct device pdev;
	char name[NAME_LEN_MAX];
	unsigned long gfp;
	unsigned long phys_start;
	unsigned long nbytes;
	unsigned int alloc_type;
	unsigned long block_align;
};
extern struct cma_zone*  hisi_get_cma_zone(const char* name);

static LIST_HEAD(mmz_list);
static HI_DECLARE_MUTEX(mmz_lock);

static int anony = 0;
static int mmz_info_phys_start = -1;

module_param(anony, int, S_IRUGO);


static void dump_mmz_mem(void);
static void __dma_clear_buffer(struct page *page, size_t size);

hil_mmz_t *hil_mmz_create(const char *name, unsigned long gfp, unsigned long phys_start, unsigned long nbytes)
{
    hil_mmz_t *p = NULL;

    mmz_trace_func();

    if (name == NULL)
    {
        PRINTK_CA(KERN_ERR "%s: 'name' can not be zero!", __FUNCTION__);
        return NULL;
    }

    p = kmalloc(sizeof(hil_mmz_t) + 1, GFP_KERNEL);
    if (p == NULL)
    {
        return NULL;
    }

    memset(p, 0, sizeof(hil_mmz_t) + 1);
    strlcpy(p->name, name, HIL_MMZ_NAME_LEN);
    p->gfp = gfp;
    p->phys_start = phys_start;
    p->nbytes = nbytes;

    INIT_LIST_HEAD(&p->list);
    INIT_LIST_HEAD(&p->mmb_list);

    p->destructor = kfree;

    return p;
}

#ifndef CFG_MMZ_MINI
hil_mmz_t *hil_mmz_create_v2(const char *name, unsigned long gfp, unsigned long phys_start,
                             unsigned long nbytes, unsigned int alloc_type, unsigned long block_align)
{
    hil_mmz_t *p;

    mmz_trace_func();

    if (name == NULL)
    {
        PRINTK_CA(KERN_ERR "%s: 'name' can not be zero!", __FUNCTION__);
        return NULL;
    }

    p = kmalloc(sizeof(hil_mmz_t), GFP_KERNEL);
    if (p == NULL)
    {
        return NULL;
    }

    memset(p, 0, sizeof(hil_mmz_t));
    strlcpy(p->name, name, HIL_MMZ_NAME_LEN);
    p->gfp = gfp;
    p->phys_start = phys_start;
    p->nbytes = nbytes;
    p->alloc_type  = alloc_type;
    p->block_align = block_align;

    INIT_LIST_HEAD(&p->list);
    INIT_LIST_HEAD(&p->mmb_list);

    p->destructor = kfree;

    return p;
}
#endif

int hil_mmz_destroy(hil_mmz_t *zone)
{
    if (zone == NULL)
    {
        return -1;
    }

    if (zone->destructor)
    {
        zone->destructor(zone);
    }

    return 0;
}


int hil_mmz_register(hil_mmz_t *zone)
{
   // int ret = 0;

    mmz_trace(1, HIL_MMZ_FMT_S, hil_mmz_fmt_arg(zone));

    if (zone == NULL)
    {
        return -1;
    }

    down(&mmz_lock);


    INIT_LIST_HEAD(&zone->mmb_list);

    list_add(&zone->list, &mmz_list);

    up(&mmz_lock);

    return 0;
}

int hil_mmz_unregister(hil_mmz_t *zone)
{
    int losts = 0;
    hil_mmb_t *p;

    if (zone == NULL)
    {
        return -1;
    }

    mmz_trace_func();

    down(&mmz_lock);
    list_for_each_entry(p, &zone->mmb_list, list)
    {
        PRINTK_CA(KERN_WARNING "          MB Lost: " HIL_MMB_FMT_S "\n", hil_mmb_fmt_arg(p));
        losts++;
    }

    if (losts)
    {
        PRINTK_CA(KERN_ERR "%d mmbs not free, mmz<%s> can not be deregistered!\n", losts, zone->name);
        up(&mmz_lock);
        return -1;
    }

    list_del(&zone->list);


    //hil_mmz_destroy(zone);

    up(&mmz_lock);

    return 0;
}


static int _do_mmb_alloc(hil_mmb_t *mmb)
{
    hil_mmb_t *p = NULL;

    mmz_trace_func();


    /* add mmb sorted */
    list_for_each_entry(p, &mmb->zone->mmb_list, list)
    {
        if (mmb->phys_addr < p->phys_addr)
        {
            break;
        }

        if (mmb->phys_addr == p->phys_addr)
        {
            PRINTK_CA(KERN_ERR "ERROR: media-mem allocator bad in %s! (%s, %d)",
                   mmb->zone->name, __FUNCTION__, __LINE__);
        }
    }
    list_add(&mmb->list, p->list.prev);

    mmz_trace(1, HIL_MMB_FMT_S, hil_mmb_fmt_arg(mmb));

    return 0;
}

static hil_mmb_t *__mmb_alloc(const char *name, unsigned long size, unsigned long align,
                              unsigned long gfp, const char *mmz_name, hil_mmz_t *_user_mmz)
{
    hil_mmz_t *mmz;
    hil_mmb_t *mmb;

    unsigned long order = get_order(size);
    size_t count = size >> PAGE_SHIFT;
    struct page *page;

    unsigned long fixed_start = 0;
    //unsigned long fixed_len = -1;
    hil_mmz_t *fixed_mmz = NULL;

    mmz_trace_func();

    if ((size <= 0) || (size > 0x40000000UL))
    {
        return NULL;
    }

    if (align == 0)
    {
        align = MMZ_GRAIN;
    }

    size = mmz_grain_align(size); 
    order = get_order(size);	
    count = size>>PAGE_SHIFT; 	

    mmz_trace(1, "size=%luKB, align=%lu", size / SZ_1K, align);

    begin_list_for_each_mmz(mmz, gfp, mmz_name)
    if ((_user_mmz != NULL) && (_user_mmz != mmz))
    {
        continue;
    }

    page = dma_alloc_from_contiguous(mmz->cma_dev, count, order);
    if(!page)
		continue;
    fixed_mmz = mmz;
    fixed_start = page_to_phys(page);	
    break;

    end_list_for_each_mmz()

    if (fixed_mmz == NULL)
    {
		if (mmz_name == NULL)
		{
			PRINTK_CA(KERN_ERR" Alloc mmb '%s' failed, size %lu bytes.\n", name, size);
			dump_mmz_mem();
		}
		return NULL;
	}

    /** clear inner and outer cache data  **/
    if(page)
	__dma_clear_buffer(page,size);

    mmb = kmalloc(sizeof(hil_mmb_t), GFP_KERNEL);
    if (!mmb) {
        return NULL;
    }

    memset(mmb, 0, sizeof(hil_mmb_t));
    mmb->zone = fixed_mmz;
    mmb->phys_addr = fixed_start;
    mmb->length = size;
    if (name)
    {
        strlcpy(mmb->name, name, HIL_MMB_NAME_LEN);
    }
    else
    {
        strncpy(mmb->name, "<null>", sizeof(mmb->name)-1);
    }

    if (_do_mmb_alloc(mmb))
    {
        kfree(mmb);
        mmb = NULL;
    }

    return mmb;
}

#ifndef CFG_MMZ_MINI
static hil_mmb_t *__mmb_alloc_v2(const char *name, unsigned long size, unsigned long align, unsigned long gfp,
                                 const char *mmz_name, hil_mmz_t *_user_mmz, unsigned int order)
{
    hil_mmz_t *mmz;
    hil_mmb_t *mmb;
    int i;

    unsigned long cma_order ;
    size_t count = size >> PAGE_SHIFT;
    struct page *page;

    unsigned long start = 0;
    unsigned long region_len = 0;

    unsigned long fixed_start = 0;
    unsigned long fixed_len = ~1;
    hil_mmz_t *fixed_mmz = NULL;

    mmz_trace_func();

    if ((size == 0) || (size > 0x40000000UL))
    {
        return NULL;
    }

    if (align == 0)
    {
        align = 1;
    }

    size = mmz_grain_align(size);

    mmz_trace(1, "size=%luKB, align=%lu", size / SZ_1K, align);

    begin_list_for_each_mmz(mmz, gfp, mmz_name)
    if ((_user_mmz != NULL) && (_user_mmz != mmz))
    {
        continue;
    }

    if (mmz->alloc_type == SLAB_ALLOC)
    {
        if ((size - 1) & size)
        {
            for (i = 1; i <= 32; i++)
            {
                if (!((size >> i) & ~0))
                {
                    size = 1 << i;
                    break;
                }
            }
        }
    }
    else if (mmz->alloc_type == EQ_BLOCK_ALLOC)
    {
        size = mmz_align2(size, mmz->block_align);
    }

    cma_order = get_order(size);		     	

    page = dma_alloc_from_contiguous(mmz.cma_dev, count, cma_order);
    if(!page)
		return NULL;
    fixed_mmz = mmz;
    fixed_start = page_to_phys(page);	
    break;


    end_list_for_each_mmz()

    if (fixed_mmz == NULL)
    {
        return NULL;
    }

    mmb = kmalloc(sizeof(hil_mmb_t), GFP_KERNEL);

    memset(mmb, 0, sizeof(hil_mmb_t));
    mmb->zone = fixed_mmz;
    mmb->phys_addr = fixed_start;
    mmb->length = size;
    mmb->order = order;
    if (name)
    {
        strlcpy(mmb->name, name, HIL_MMB_NAME_LEN);
    }
    else
    {
        strcpy(mmb->name, "<null>");
    }

    if (_do_mmb_alloc(mmb))
    {
        kfree(mmb);
        mmb = NULL;
    }

    return mmb;
}

#endif

hil_mmb_t *hil_mmb_alloc(const char *name, unsigned long size, unsigned long align,
                         unsigned long gfp, const char *mmz_name)
{
    hil_mmb_t *mmb;

    down(&mmz_lock);
    mmb = __mmb_alloc(name, size, align, gfp, mmz_name, NULL);
    up(&mmz_lock);

    return mmb;
}

#ifndef CFG_MMZ_MINI
hil_mmb_t *hil_mmb_alloc_v2(const char *name, unsigned long size, unsigned long align,
                            unsigned long gfp, const char *mmz_name, unsigned int order)
{
    hil_mmb_t *mmb;

    down(&mmz_lock);
    mmb = __mmb_alloc_v2(name, size, align, gfp, mmz_name, NULL, order);
    up(&mmz_lock);

    return mmb;
}
#endif

hil_mmb_t *hil_mmb_alloc_in(const char *name, unsigned long size, unsigned long align,
                            hil_mmz_t *_user_mmz)
{
    hil_mmb_t *mmb;

    if (_user_mmz == NULL)
    {
        return NULL;
    }

    down(&mmz_lock);
    mmb = __mmb_alloc(name, size, align, _user_mmz->gfp, _user_mmz->name, _user_mmz);
    up(&mmz_lock);

    return mmb;
}

#ifndef CFG_MMZ_MINI
hil_mmb_t *hil_mmb_alloc_in_v2(const char *name, unsigned long size, unsigned long align,
                               hil_mmz_t *_user_mmz, unsigned int order)
{
    hil_mmb_t *mmb;

    if (_user_mmz == NULL)
    {
        return NULL;
    }

    down(&mmz_lock);
    mmb = __mmb_alloc_v2(name, size, align, _user_mmz->gfp, _user_mmz->name, _user_mmz, order);
    up(&mmz_lock);

    return mmb;
}
#endif

static int __dma_update_pte(pte_t *pte, pgtable_t token, unsigned long addr,
			    void *data)
{
	struct page *page = virt_to_page(addr);
	pgprot_t prot = *(pgprot_t *)data;

	set_pte_ext(pte, mk_pte(page, prot), 0);
	return 0;
}

static void __dma_remap(struct page *page, size_t size, pgprot_t prot)
{
	unsigned long start = (unsigned long) page_address(page);
	unsigned end = start + size;

	apply_to_page_range(&init_mm, start, size, __dma_update_pte, &prot);
	dsb();
	hisi_flush_tlb_kernel_range(start, end);
}

static void __dma_clear_buffer(struct page *page, size_t size)
{
	void *ptr;
#define L2_CACHE_SIZE	(512 * 1024)
	/*
	 * Ensure that the allocated pages are zeroed, and that any data
	 * lurking in the kernel direct-mapped region is invalidated.
	 */
	ptr = page_address(page);
	if (ptr) {
		/*  remove memset for speed
		memset(ptr, 0, size);
		*/
		if (size < L2_CACHE_SIZE) {
			dmac_flush_range(ptr, ptr + size);
			outer_flush_range(__pa(ptr), __pa(ptr) + size);
		} else {
#ifdef CONFIG_SMP
			on_each_cpu(__cpuc_flush_kern_all, NULL, 1);
#else
			__cpuc_flush_kern_all();
#endif
			outer_flush_all();
		}
	}
}

static void *_mmb_map2kern(hil_mmb_t *mmb, int cached)
{
    pgprot_t prot;	
    struct page* page = phys_to_page(mmb->phys_addr);	

    if (mmb->flags & HIL_MMB_MAP2KERN)
    {
        if ((cached * HIL_MMB_MAP2KERN_CACHED) != (mmb->flags & HIL_MMB_MAP2KERN_CACHED))
        {
            PRINTK_CA(KERN_ERR "mmb<%s> already kernel-mapped %s, can not be re-mapped as %s.",
                   mmb->name,
                   (mmb->flags & HIL_MMB_MAP2KERN_CACHED) ? "cached" : "non-cached",
                   (cached) ? "cached" : "non-cached" );
            return NULL;
        }

        mmb->map_ref++;

        return mmb->kvirt;
    }

    if (cached) {
	    mmb->flags |= HIL_MMB_MAP2KERN_CACHED;
	    prot = pgprot_kernel;
    } else {
	    mmb->flags &= ~HIL_MMB_MAP2KERN_CACHED;
	    prot = pgprot_writecombine(pgprot_kernel);
    }

    /* FIXME: invalid all the cache here? */
    __dma_remap(page, mmb->length, prot);
    mmb->kvirt = __va(mmb->phys_addr);

    if (mmb->kvirt)
    {
        mmb->flags |= HIL_MMB_MAP2KERN;
        mmb->map_ref++;
    }

    return mmb->kvirt;
}

void *hil_mmb_map2kern(hil_mmb_t *mmb)
{
    void *p;

    if (mmb == NULL)
    {
        return NULL;
    }

    down(&mmz_lock);
    p = _mmb_map2kern(mmb, 0);
    up(&mmz_lock);

    return p;
}

void *hil_mmb_map2kern_cached(hil_mmb_t *mmb)
{
    void *p;

    if (mmb == NULL)
    {
        return NULL;
    }

    down(&mmz_lock);
    p = _mmb_map2kern(mmb, 1);
    up(&mmz_lock);

    return p;
}

static int _mmb_free(hil_mmb_t *mmb);

int hil_mmb_unmap(hil_mmb_t *mmb)
{
    int ref;
    struct page* page = NULL;

    if (mmb == NULL)
    {
        return -1;
    }

    page = phys_to_page(mmb->phys_addr);

    down(&mmz_lock);

    if (mmb->flags & HIL_MMB_MAP2KERN_CACHED)
    {
	__cpuc_flush_dcache_area((void *)mmb->kvirt, (size_t)mmb->length); // flush l1cache
	outer_flush_range(mmb->phys_addr, mmb->phys_addr + mmb->length); // flush l2cache
    }

    if (mmb->flags & HIL_MMB_MAP2KERN)
    {
        ref = --mmb->map_ref;
        if (mmb->map_ref != 0)
        {
            up(&mmz_lock);
            return ref;
        }
	__dma_remap(page, mmb->length, pgprot_kernel);
       // iounmap(mmb->kvirt);
    }

    mmb->kvirt  = NULL;
    mmb->flags &= ~HIL_MMB_MAP2KERN;
    mmb->flags &= ~HIL_MMB_MAP2KERN_CACHED;

    if ((mmb->flags & HIL_MMB_RELEASED) && (mmb->phy_ref == 0))
    {
        _mmb_free(mmb);
    }

    up(&mmz_lock);

    return 0;
}

int hil_mmb_get(hil_mmb_t *mmb)
{
    int ref;

    if (mmb == NULL)
    {
        return -1;
    }

    down(&mmz_lock);

    if (mmb->flags & HIL_MMB_RELEASED)
    {
        PRINTK_CA(KERN_WARNING "hil_mmb_get: amazing, mmb<%s> is released!\n", mmb->name);
    }

    ref = ++mmb->phy_ref;

    up(&mmz_lock);

    return ref;
}

static int _mmb_free(hil_mmb_t *mmb)
{
    size_t count = mmb->length >> PAGE_SHIFT;
    struct page *page = phys_to_page(mmb->phys_addr);	

    hil_mmz_t *mmz = mmb->zone;	

    if (mmb->flags & HIL_MMB_MAP2KERN_CACHED)
    {
	__cpuc_flush_dcache_area((void *)mmb->kvirt, (size_t)mmb->length); // flush l1cache
	outer_flush_range(mmb->phys_addr, mmb->phys_addr + mmb->length); // flush l2cache
    }

    dma_release_from_contiguous(mmz->cma_dev,page,count);

    list_del(&mmb->list);
    kfree(mmb);

    return 0;
}

int hil_mmb_put(hil_mmb_t *mmb)
{
    int ref;

    if (mmb == NULL)
    {
        return -1;
    }

    down(&mmz_lock);

    if (mmb->flags & HIL_MMB_RELEASED)
    {
        PRINTK_CA(KERN_WARNING "hil_mmb_put: amazing, mmb<%s> is released!\n", mmb->name);
    }

    ref = --mmb->phy_ref;

    if ((mmb->flags & HIL_MMB_RELEASED) && (mmb->phy_ref == 0) && (mmb->map_ref == 0))
    {
        _mmb_free(mmb);
    }

    up(&mmz_lock);

    return ref;
}

int hil_mmb_free(hil_mmb_t *mmb)
{
    mmz_trace_func();

    if (mmb == NULL)
    {
        return -1;
    }

    mmz_trace(1, HIL_MMB_FMT_S, hil_mmb_fmt_arg(mmb));

    down(&mmz_lock);

    if (mmb->flags & HIL_MMB_RELEASED)
    {
        PRINTK_CA(KERN_WARNING "hil_mmb_free: amazing, mmb<%s> is released before, but still used!\n", mmb->name);

        up(&mmz_lock);

        return 0;
    }

    if (mmb->phy_ref > 0)
    {
        PRINTK_CA(KERN_WARNING "hil_mmb_free: free mmb<%s> delayed for which ref-count is %d!\n",
               mmb->name, mmb->map_ref);
        mmb->flags |= HIL_MMB_RELEASED;
        up(&mmz_lock);

        return 0;
    }

    if (mmb->flags & HIL_MMB_MAP2KERN)
    {
        PRINTK_CA(KERN_WARNING "hil_mmb_free: free mmb<%s> delayed for which is kernel-mapped to 0x%p with map_ref %d!\n",
               mmb->name, mmb->kvirt, mmb->map_ref);
        mmb->flags |= HIL_MMB_RELEASED;
        up(&mmz_lock);

        return 0;
    }

    _mmb_free(mmb);

    up(&mmz_lock);

    return 0;
}

#define MACH_MMB(p, val, member) do {\
        hil_mmz_t *__mach_mmb_zone__; \
        (p) = NULL; \
        list_for_each_entry(__mach_mmb_zone__, &mmz_list, list) { \
            hil_mmb_t *__mach_mmb__; \
            list_for_each_entry(__mach_mmb__, &__mach_mmb_zone__->mmb_list, list) { \
                if (__mach_mmb__->member == (val)){ \
                    (p) = __mach_mmb__; \
                    break; \
                } \
            } \
            if (p) break;\
        } \
    } while (0)

hil_mmb_t *hil_mmb_getby_phys(unsigned long addr)
{
    hil_mmb_t *p;

    down(&mmz_lock);
    MACH_MMB(p, addr, phys_addr);
    up(&mmz_lock);

    return p;
}

hil_mmb_t *hil_mmb_getby_kvirt(void *virt)
{
    hil_mmb_t *p;

    if (virt == NULL)
    {
        return NULL;
    }

    down(&mmz_lock);
    MACH_MMB(p, virt, kvirt);
    up(&mmz_lock);

    return p;
}

#define MACH_MMB_2(p, val, Outoffset) do {\
        hil_mmz_t *__mach_mmb_zone__; \
        (p) = NULL; \
        list_for_each_entry(__mach_mmb_zone__, &mmz_list, list) { \
            hil_mmb_t *__mach_mmb__; \
            list_for_each_entry(__mach_mmb__, &__mach_mmb_zone__->mmb_list, list) { \
                if ((__mach_mmb__->phys_addr <= (val)) && ((__mach_mmb__->length + __mach_mmb__->phys_addr) > (val))){ \
                    (p) = __mach_mmb__; \
                    Outoffset = val - __mach_mmb__->phys_addr; \
                    break; \
                } \
            } \
            if (p) break;\
        } \
    } while (0)

hil_mmb_t *hil_mmb_getby_phys_2(unsigned long addr, unsigned long *Outoffset)
{
    hil_mmb_t *p;

    down(&mmz_lock);
    MACH_MMB_2(p, addr, *Outoffset);
    up(&mmz_lock);
    return p;
}

hil_mmz_t *hil_mmz_find(unsigned long gfp, const char *mmz_name)
{
    hil_mmz_t *p;

    down(&mmz_lock);
    begin_list_for_each_mmz(p, gfp, mmz_name)
    up(&mmz_lock);
    return p;
    end_list_for_each_mmz()
    up(&mmz_lock);

    return NULL;
}

/*
 * name,gfp,phys_start,nbytes,alloc_type;...
 * All param in hex mode, except name.
 */
static int media_mem_parse_cmdline(char *s)
{
    hil_mmz_t *zone = NULL;
    char *line;
    struct cma_zone * cma_zone;

    while ((line = strsep(&s, ":")) != NULL)
    {
        int i;
        char *argv[6];

        /*
         * FIXME: We got 4 args in "line", formated "argv[0],argv[1],argv[2],argv[3],argv[4]".
         * eg: "<mmz_name>,<gfp>,<phys_start_addr>,<size>,<alloc_type>"
         * For more convenient, "hard code" are used such as "arg[0]", i.e.
         */
        for (i = 0; (argv[i] = strsep(&line, ",")) != NULL;)
        {
            if (++i == ARRAY_SIZE(argv))
            {
                break;
            }
        }

	cma_zone = hisi_get_cma_zone(argv[0]);
	if(!cma_zone)
	{
		PRINTK_CA(KERN_ERR"can't get cma zone info:%s\n",argv[0]);
		continue;
	}

        if (i == 4)
        {
            zone = hil_mmz_create("null", 0, 0, 0);
	     if(NULL == zone)
	     {
	     	   continue;
	     }
            strlcpy(zone->name, argv[0], HIL_MMZ_NAME_LEN);
            //zone->gfp = _strtoul_ex(argv[1], NULL, 0);
            //zone->phys_start = _strtoul_ex(argv[2], NULL, 0);
            //zone->nbytes = _strtoul_ex(argv[3], NULL, 0);
		zone->gfp = cma_zone->gfp;
		zone->phys_start = cma_zone->phys_start;
		zone->nbytes = cma_zone->nbytes;
		zone->cma_dev = &cma_zone->pdev;
	}
#ifndef CFG_MMZ_MINI
        else if (i == 6)
        {
            zone = hil_mmz_create_v2("null", 0, 0, 0, 0, 0);
	     if(NULL == zone)
	     {
	     	   continue;
	     }		
            strlcpy(zone->name, argv[0], HIL_MMZ_NAME_LEN);
            //zone->gfp = _strtoul_ex(argv[1], NULL, 0);
            //zone->phys_start = _strtoul_ex(argv[2], NULL, 0);
            //zone->nbytes = _strtoul_ex(argv[3], NULL, 0);
            //zone->alloc_type  = _strtoul_ex(argv[4], NULL, 0);
            //zone->block_align = _strtoul_ex(argv[5], NULL, 0);
		zone->gfp = cma_zone->gfp;
		zone->phys_start = cma_zone->phys_start;
		zone->nbytes = cma_zone->nbytes;
		zone->alloc_type = cma_zone->alloc_type;
		zone->block_align = cma_zone->block_align;
		zone->cma_dev = &cma_zone->pdev;
	}
#endif
        else
        {
            PRINTK_CA(KERN_ERR "MMZ: your parameter num is not correct!\n");
            continue;
        }

        mmz_info_phys_start = zone->phys_start + zone->nbytes - 0x2000;
        if (hil_mmz_register(zone))
        {
            PRINTK_CA(KERN_WARNING "Add MMZ failed: " HIL_MMZ_FMT_S "\n", hil_mmz_fmt_arg(zone));
            hil_mmz_destroy(zone);
        }

        zone = NULL;
    }

    return 0;
}

int get_mmz_info_phys_start(void)
{
    return mmz_info_phys_start;
}

#define MAX_MMZ_INFO_LEN 20*1024
#define CHECK_BUFF_OVERFLOW() \
do{\
	if (p_mmz_info_buf + len - mmz_info_buf > MAX_MMZ_INFO_LEN){\
		PRINTK_CA(KERN_ERR"mmz_info_buff overflow(%d), more than 20k data!\n", p_mmz_info_buf + len - mmz_info_buf);\
		break;\
	};\
}while(0)

#define SPLIT_LINE "-------------------------------------------------------------------------------------------------------\n"

static void dump_mmz_mem(void)
{
	hil_mmz_t *p;

	int nZoneCnt = 0;
	unsigned int u32Number = 0;
	unsigned int used_size = 0, free_size = 0;

	printk( "|                   PHYS           |  ID  |   KVIRT   |    FLAGS    |  LENGTH(KB)  |       NAME        |\n");

	list_for_each_entry(p, &mmz_list, list){
		hil_mmb_t *mmb;

		list_for_each_entry(mmb, &p->mmb_list, list){
			u32Number++;
		}

		printk("|ZONE[%d]: (0x%08lx, 0x%08lx)   %d                 0x%08lx      %-10lu   \"%s%-14s|\n",nZoneCnt, \
			(p)->phys_start,(p)->phys_start+(p)->nbytes-1, u32Number, (p)->gfp,(p)->nbytes/SZ_1K,(p)->name, "\"");

		nZoneCnt++;

		mmz_total_size += p->nbytes / 1024;
		zone_number++;

		list_for_each_entry(mmb, &p->mmb_list, list){
			printk("|" HIL_MMB_FMT_S "|\n", hil_mmb_fmt_arg(mmb));
			used_size += mmb->length/1024;
			block_number++;
		}
	}
	printk(SPLIT_LINE);
	printk("|%-102s|\n", "Summary:");
	printk(SPLIT_LINE);
	printk("|  MMZ Total Size  |      Used      |     Idle        |  Zone Number  |   BLock Number                 |\n");
	printk(SPLIT_LINE);

	if(0 != mmz_total_size){
		free_size = mmz_total_size - used_size;
		printk("|       %d%-8s       %d%-8s      %d%-6s          %d                  %d                      |\n",
			 mmz_total_size/1024, "MB", used_size/1024, "MB", free_size/1024, "MB", zone_number, block_number);
		mmz_total_size = 0;
		zone_number = 0;
		block_number = 0;
	}

	printk(SPLIT_LINE);
}

int mmz_read_proc(char *page, char **start, off_t off,
                  int count, int *eof, void *data)
{
	unsigned long ret = 0;
#if !(0 == HI_PROC_SUPPORT)
    int nZoneCnt = 0;
	hil_mmz_t *p;
	unsigned int used_size = 0, free_size = 0;
	unsigned long len = 0;
	char * mmz_info_buf = (char*)__get_free_pages(GFP_TEMPORARY, get_order(MAX_MMZ_INFO_LEN));
	char * p_mmz_info_buf = mmz_info_buf;
    unsigned int u32Number = 0;
    
	memset(mmz_info_buf, 0, MAX_MMZ_INFO_LEN);
	
	down(&mmz_lock);

    len = snprintf(page , count, SPLIT_LINE);
    memcpy(p_mmz_info_buf, page, len);
    p_mmz_info_buf += len;

    
    len = snprintf(page , count, "|                   PHYS           |  ID  |   KVIRT   |    FLAGS    |  LENGTH(KB)  |       NAME        |\n");
    memcpy(p_mmz_info_buf, page, len);
    p_mmz_info_buf += len;
    
    len = snprintf(page , count, SPLIT_LINE);
    memcpy(p_mmz_info_buf, page, len);
    p_mmz_info_buf += len;

	// Collect all mmb info into mmz_info_buff
	list_for_each_entry(p, &mmz_list, list){
		hil_mmb_t *mmb;

        list_for_each_entry(mmb, &p->mmb_list, list){
			u32Number++;
		}

		len = snprintf(page , count, "|ZONE[%d]: (0x%08lx, 0x%08lx)   %d                 0x%08lx      %-10lu   \"%s%-14s|\n",nZoneCnt, \
                       (p)->phys_start,(p)->phys_start+(p)->nbytes-1, u32Number, (p)->gfp,(p)->nbytes/SZ_1K,(p)->name, "\"");
		CHECK_BUFF_OVERFLOW();
		memcpy(p_mmz_info_buf, page, len);
		p_mmz_info_buf += len;

        nZoneCnt++;

		mmz_total_size += p->nbytes / 1024;
		zone_number++;

		list_for_each_entry(mmb, &p->mmb_list, list){
			len = snprintf(page, count, "|" HIL_MMB_FMT_S "|\n", hil_mmb_fmt_arg(mmb));
			CHECK_BUFF_OVERFLOW();
			memcpy(p_mmz_info_buf, page, len);
			p_mmz_info_buf += len;
		
			used_size += mmb->length/1024;
			block_number++;
		}
	}

    len = snprintf(page , count, SPLIT_LINE);
    memcpy(p_mmz_info_buf, page, len);
    p_mmz_info_buf += len;

    len = snprintf(page , count, "|%-102s|\n", "Summary:");
    memcpy(p_mmz_info_buf, page, len);
    p_mmz_info_buf += len;

    len = snprintf(page , count, SPLIT_LINE);
    memcpy(p_mmz_info_buf, page, len);
    p_mmz_info_buf += len;

    
    len = snprintf(page , count, "|  MMZ Total Size  |      Used      |     Idle        |  Zone Number  |   BLock Number                 |\n");
    memcpy(p_mmz_info_buf, page, len);
    p_mmz_info_buf += len;

    len = snprintf(page , count, SPLIT_LINE);
    memcpy(p_mmz_info_buf, page, len);
    p_mmz_info_buf += len;
	
	if(0 != mmz_total_size){
		free_size = mmz_total_size - used_size;
	    len = snprintf(page, count, "|       %d%-8s       %d%-8s      %d%-6s          %d                  %d                      |\n",
				 mmz_total_size/1024, "MB", used_size/1024, "MB", free_size/1024, "MB", zone_number, block_number);
		CHECK_BUFF_OVERFLOW();
		memcpy(p_mmz_info_buf, page, len);
		p_mmz_info_buf += len;
		
		mmz_total_size = 0;
		zone_number = 0;
		block_number = 0;
	}

    len = snprintf(page , count, SPLIT_LINE);
    memcpy(p_mmz_info_buf, page, len);
    p_mmz_info_buf += len;

	// transfer info to proc buff page
	if (off +  count > p_mmz_info_buf - mmz_info_buf){
		memcpy(page, mmz_info_buf + off, p_mmz_info_buf - mmz_info_buf - off);
		*eof = 1;
		ret = p_mmz_info_buf - mmz_info_buf - off;
	}else{
		memcpy(page, mmz_info_buf + off, count);
		ret = count;
	}
	*start = page;

	up(&mmz_lock);

	free_pages((unsigned long)mmz_info_buf, get_order(MAX_MMZ_INFO_LEN) );
#endif

	return ret;
}


#define MMZ_SETUP_CMDLINE_LEN 256
static char __initdata setup_zones[MMZ_SETUP_CMDLINE_LEN] = "ddr,0,0,160M";

static void mmz_exit_check(void)
{
    hil_mmz_t *p;

    mmz_trace_func();
    for (p = hil_mmz_find(0, NULL); p != NULL; p = hil_mmz_find(0, NULL))
    {
        //PRINTK_CA(KERN_WARNING "MMZ force removed: " HIL_MMZ_FMT_S "\n", hil_mmz_fmt_arg(p));
        hil_mmz_unregister(p);
    }
}

int HI_DRV_MMZ_Init(void)
{
    char *s;
    char *p = NULL;
    char *q;
 //   int len;

    //PRINTK_CA(KERN_INFO "Hisilicon Media Memory Zone Manager state 0\n");

 //   len = strlen(setup_zones);
 //   if (len == 0)
    {
        strlcpy(line, saved_command_line, COMMAND_LINE_SIZE);
        q = strstr(line, "mmz=");
        if (q)
        {
            s = strsep(&q, "=");
            if (s)
            {
                p = strsep(&q, " ");
            }
	     if (p)
	     {
            	   strlcpy(setup_zones, p, MMZ_SETUP_CMDLINE_LEN);
	     }
        }
    }

    media_mem_parse_cmdline(setup_zones);
    return 0;
}

void HI_DRV_MMZ_Exit(void)
{
    mmz_exit_check();
    return;
}

//EXPORT_SYMBOL(hil_mmb_alloc);
#ifndef CFG_MMZ_MINI
//EXPORT_SYMBOL(hil_mmb_alloc_v2);
#endif
//EXPORT_SYMBOL(hil_mmb_free);
//EXPORT_SYMBOL(hil_mmb_get);
//EXPORT_SYMBOL(hil_mmb_put);
//EXPORT_SYMBOL(mmz_read_proc);
//EXPORT_SYMBOL(mmz_write_proc);
//EXPORT_SYMBOL(HI_DRV_MMZ_Init);
//EXPORT_SYMBOL(HI_DRV_MMZ_Exit);
