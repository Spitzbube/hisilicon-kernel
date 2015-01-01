/* linux/include/asm-arm/arch-hi3510_v100_m01/media-mem.h
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

#ifndef __ASM_ARCH_MEDIA_MEM_H
#define __ASM_ARCH_MEDIA_MEM_H

#ifdef __cplusplus
extern "C" {
#endif

#define HIL_MMZ_NAME_LEN        32

#ifdef MMZ_V2_SUPPORT
#define MMZ_MODULE_OPTIMIZE     1

#if MMZ_MODULE_OPTIMIZE
#define MMB_ALLOC_OPTIMIZE      1   // New a MMB function
#define MMB_MAP_OPTIMIZE        1   // Map funtion
#define MMB_FLUSH_OPTIMIZE      1   // flush function
#if MMB_ALLOC_OPTIMIZE
#define MMB_LIST_OPTIMIZE       1   // must include MMB_ALLOC_OPTIMIZE
#endif

#define ALLOC_PAGES_MAX_NUM     9

struct mmb_free_node {
    unsigned long addr_start;
    unsigned long mmb_size;
    struct list_head list;
};
#endif
#endif

#ifndef MMB_ALLOC_OPTIMIZE
#define MMB_ALLOC_OPTIMIZE      0
#endif

#ifndef MMB_MAP_OPTIMIZE
#define MMB_MAP_OPTIMIZE        0
#endif

#ifndef MMB_FLUSH_OPTIMIZE
#define MMB_FLUSH_OPTIMIZE      0
#endif

#ifndef MMB_LIST_OPTIMIZE
#define MMB_LIST_OPTIMIZE       0
#endif

#ifndef PRINTK_CA
#ifdef  CONFIG_SUPPORT_CA_RELEASE
#define PRINTK_CA(fmt, args...)
#else
#define PRINTK_CA(fmt, args...) do{\
        printk("%s(%d): " fmt, __FILE__, __LINE__, ##args); \
} while (0)
#endif
#endif

#define CFG_MMZ_MINI   //CFG_MMZ_MINI not compile *_v2, code size is smaller.

/* just for inf */
struct hil_media_memory_zone {
    char name[HIL_MMZ_NAME_LEN];
    unsigned long gfp;
    unsigned long phys_start;
    unsigned long nbytes;
    struct list_head list;
  //  unsigned char *bitmap;
    struct device *cma_dev; 	
    struct list_head mmb_list;
    unsigned int alloc_type;
    unsigned long block_align;
    void (*destructor)(const void *);
#if MMB_ALLOC_OPTIMIZE
#if MMB_LIST_OPTIMIZE
    unsigned int mmb_nums;
    struct list_head *mmb_sec_area_start;
#endif
    unsigned int nodes_num;
    unsigned long alloc_start_addr[ALLOC_PAGES_MAX_NUM];
    struct list_head *alloc_start_node[ALLOC_PAGES_MAX_NUM];
    struct list_head free_list;
#endif
};
typedef struct hil_media_memory_zone hil_mmz_t;

#define HIL_MMZ_FMT_S "PHYS(0x%08lX, 0x%08lX), GFP=%lu, nBYTES=%luKB,   NAME=\"%s\""
#define hil_mmz_fmt_arg(p) (p)->phys_start,(p)->phys_start+(p)->nbytes-1,(p)->gfp,(p)->nbytes/SZ_1K,(p)->name


/* for inf & usr */
#define HIL_MMB_NAME_LEN 16
struct hil_media_memory_block {
    char name[HIL_MMB_NAME_LEN];
    struct hil_media_memory_zone *zone;
    struct list_head list;
    unsigned long phys_addr;
    void *kvirt;
    unsigned long length;
    unsigned long flags;
    unsigned int order;
    int phy_ref;
    int map_ref;
};
typedef struct hil_media_memory_block hil_mmb_t;

#define hil_mmb_kvirt(p)    ({hil_mmb_t *__mmb=(p); BUG_ON(__mmb==NULL); __mmb->kvirt;})
#define hil_mmb_phys(p)     ({hil_mmb_t *__mmb=(p); BUG_ON(__mmb==NULL); __mmb->phys_addr;})
#define hil_mmb_length(p)   ({hil_mmb_t *__mmb=(p); BUG_ON(__mmb==NULL); __mmb->length;})
#define hil_mmb_name(p)     ({hil_mmb_t *__mmb=(p); BUG_ON(__mmb==NULL); __mmb->name;})
#define hil_mmb_zone(p)     ({hil_mmb_t *__mmb=(p); BUG_ON(__mmb==NULL); __mmb->zone;})

#define HIL_MMB_MAP2KERN    (1<<0)
#define HIL_MMB_MAP2KERN_CACHED (1<<1)
#define HIL_MMB_RELEASED    (1<<2)

#define HIL_MMB_FMT_S "phys(0x%08lX, 0x%08lX), kvirt=0x%p, flags=0x%08lX, length=%luKB,    name=\"%s\""
#define hil_mmb_fmt_arg(p) (p)->phys_addr,mmz_grain_align((p)->phys_addr+(p)->length)-1,(p)->kvirt,(p)->flags,(p)->length/SZ_1K,(p)->name



/********** API_0 for inf *********/
extern hil_mmz_t *hil_mmz_create(const char *name, unsigned long gfp, unsigned long phys_start,
            unsigned long nbytes);
extern hil_mmz_t *hil_mmz_create_v2(const char *name, unsigned long gfp, unsigned long phys_start,
            unsigned long nbytes,  unsigned int alloc_type, unsigned long block_align);
extern int hil_mmz_destroy(hil_mmz_t *zone);
extern int hil_mmz_register(hil_mmz_t *zone);
extern int hil_mmz_unregister(hil_mmz_t *zone);
extern hil_mmb_t *hil_mmb_getby_phys(unsigned long addr);
extern hil_mmb_t *hil_mmb_getby_phys_2(unsigned long addr, unsigned long *Outoffset);
extern hil_mmb_t *hil_mmb_getby_kvirt(void *virt);
extern void* hil_mmb_map2kern(hil_mmb_t *mmb);
extern void* hil_mmb_map2kern_cached(hil_mmb_t *mmb);
extern int hil_mmb_unmap(hil_mmb_t *mmb);



/********** API_1 for inf & usr *********/

extern hil_mmb_t *hil_mmb_alloc(const char *name, unsigned long size, unsigned long align,
        unsigned long gfp, const char *mmz_name);
extern int hil_mmb_free(hil_mmb_t *mmb);
extern hil_mmb_t *hil_mmb_alloc_v2(const char *name, unsigned long size, unsigned long align,
        unsigned long gfp, const char *mmz_name, unsigned int order);

extern int hil_mmb_get(hil_mmb_t *mmb);
extern int hil_mmb_put(hil_mmb_t *mmb);
#ifdef MMZ_V2_SUPPORT
extern int hil_mmb_force_put(hil_mmb_t *mmb);
#endif
extern int get_mmz_info_phys_start(void);

extern int mmz_read_proc(char *page, char **start, off_t off,
                            int count, int *eof, void *data);

extern int mmz_write_proc(struct file *file, const char __user *buffer,
                                   unsigned long count, void *data);

int HI_DRV_MMZ_Init(void);
void HI_DRV_MMZ_Exit(void);

#ifdef __cplusplus
}
#endif

#endif