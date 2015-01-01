#ifndef __MEDIA_MEM_INTF_H__
#define __MEDIA_MEM_INTF_H__

typedef void kcom_mmz_t;
typedef unsigned long mmb_addr_t;

#define MMB_ADDR_INVALID (~0)


mmb_addr_t 	new_mmb(const char *name, int size, unsigned long align, const char *zone_name);


void 		delete_mmb(mmb_addr_t addr);

void *		remap_mmb(mmb_addr_t addr);
void *		remap_mmb_cached(mmb_addr_t addr);
void *		refer_mapped_mmb(void *mapped_addr);
int 		unmap_mmb(void *mapped_addr);

int		get_mmb(mmb_addr_t addr);
int		put_mmb(mmb_addr_t addr);

void *remap_mmb_2(mmb_addr_t phyaddr, unsigned long *VBaddr, unsigned long *Outoffset);
int unmap_mmb_2(void *mapped_addr, unsigned long offset);

#endif

