/******************************************************************************
 *    SRAM memory manager
 *    Copyright (c) 2013-2023 by Hisilicon.
 *    All rights reserved.
 * ***
 *    Create By Lai Yingjun.
 *
******************************************************************************/

#include <linux/types.h>
#include <linux/list.h>
#include <linux/vmalloc.h>
#include <asm/io.h>
#include <linux/string.h>
#include <linux/sram_memory.h>

#if 1
#define SRAM_DEBUG(x) printk("SRAM: %s, line:%d\n", x, __LINE__);
#else
#define SRAM_DEBUG(x)
#endif

#ifndef CONFIG_SRAM_PHYS_START
#define CONFIG_SRAM_PHYS_START             0xFFFF2800
#endif
#ifndef CONFIG_SRAM_SIZE
#define CONFIG_SRAM_SIZE                   0x2800
#endif

#define SRAM_VIRT_TO_PHYS(VADDR) (VADDR - sram_manager_info.sram_virt_start + CONFIG_SRAM_PHYS_START)

typedef struct {
	unsigned long addr;
	unsigned long size;
	struct list_head list;
} sram_mem_head;

typedef struct {
	unsigned long sram_virt_start;
	unsigned long sram_virt_end;
	unsigned long total_free_bytes;
	unsigned long total_free_blocks;
	spinlock_t lock;
	struct list_head sram_free_list;
	struct list_head sram_alloc_list;
	struct list_head sram_blocks_list;
	unsigned char *backup;
} sram_manager;

sram_manager sram_manager_info = { 0, 0 };

int init_sram_manager(void)
{
	sram_mem_head *p = NULL;
	unsigned long lock_flags = 0;

	SRAM_DEBUG("SRAM Manager initializing ...");
	sram_manager_info.sram_virt_start =
	    (unsigned long)ioremap_nocache(CONFIG_SRAM_PHYS_START,
					   CONFIG_SRAM_SIZE);
	if (!sram_manager_info.sram_virt_start) {
		printk(KERN_ERR "Failed to remap SRAM!\n");
		goto error;
	}

	sram_manager_info.sram_virt_end =
	    sram_manager_info.sram_virt_start + CONFIG_SRAM_SIZE;

	p = (sram_mem_head *) vmalloc(sizeof(sram_mem_head));
	if (!p) {
		printk(KERN_ERR "Failed to allocate the first head of SRAM!\n");
		goto error;
	}

	p->addr = sram_manager_info.sram_virt_start;
	p->size = CONFIG_SRAM_SIZE;

	spin_lock_init(&sram_manager_info.lock);
	spin_lock_irqsave(&sram_manager_info.lock, lock_flags);
	INIT_LIST_HEAD(&sram_manager_info.sram_free_list);
	INIT_LIST_HEAD(&sram_manager_info.sram_alloc_list);
	list_add(&p->list, &(sram_manager_info.sram_free_list));
	sram_manager_info.total_free_bytes = CONFIG_SRAM_SIZE;
	sram_manager_info.total_free_blocks = 1;
	spin_unlock_irqrestore(&sram_manager_info.lock, lock_flags);

	sram_manager_info.backup = (unsigned char *)vmalloc(CONFIG_SRAM_SIZE);
	if (!sram_manager_info.backup) {
		printk(KERN_ERR
		       "Failed to allocate SRAM backup region for power manager!\n");
		goto error;
	}
	SRAM_DEBUG("SRAM Manager initialized");
	return 0;

error:
	spin_unlock_irqrestore(&sram_manager_info.lock, lock_flags);
	if (p)
		vfree(p);

	if (sram_manager_info.backup) {
		vfree(sram_manager_info.backup);
	}

	if (sram_manager_info.sram_virt_start)
		iounmap((void *)sram_manager_info.sram_virt_start);

	printk(KERN_ERR "SRAM Manager initialize failed!");
	return -1;
}

void *sram_alloc(unsigned long size, unsigned int *phys_addr,
		 unsigned long flags)
{
	struct list_head *loop = NULL;
	sram_mem_head *tmp = NULL;
	unsigned long i = 0;
	unsigned long alloc_addr = 0;
	unsigned long lock_flags = 0;

	if (size > sram_manager_info.total_free_bytes) {
		SRAM_DEBUG("No mem left!");
		goto error;
	}

	list_for_each(loop, &sram_manager_info.sram_free_list) {
		tmp = list_entry(loop, sram_mem_head, list);
		if (tmp->size > size)
			break;

		if (++i >= sram_manager_info.total_free_blocks)
			break;
	}

	if (i >= sram_manager_info.total_free_blocks) {
		SRAM_DEBUG("No mem blocks fit!");
		goto error;
	}

	spin_lock_irqsave(&sram_manager_info.lock, lock_flags);
	alloc_addr = tmp->addr;
	tmp->addr += size;
	tmp->size -= size;
	if (!tmp->size) {
		list_del(&tmp->list);
		vfree(tmp);
		sram_manager_info.total_free_blocks--;
	}

	sram_manager_info.total_free_bytes -= size;
	spin_unlock_irqrestore(&sram_manager_info.lock, lock_flags);

	if (phys_addr)
		*phys_addr = SRAM_VIRT_TO_PHYS(alloc_addr);

	return (void *)alloc_addr;

error:
	spin_unlock_irqrestore(&sram_manager_info.lock, lock_flags);
	return NULL;
}

void sram_free(void *vaddr)
{
	return;
}

int sram_pm_save(void)
{
	if (!sram_manager_info.backup || !sram_manager_info.sram_virt_start) {
		return -1;
	}

	memcpy(sram_manager_info.backup,
	       (unsigned char *)sram_manager_info.sram_virt_start,
	       CONFIG_SRAM_SIZE);
	return 0;
}

int sram_pm_resume(void)
{
	if (!sram_manager_info.backup || !sram_manager_info.sram_virt_start) {
		return -1;
	}

	memcpy((unsigned char *)sram_manager_info.sram_virt_start,
	       sram_manager_info.backup, CONFIG_SRAM_SIZE);
	return 0;
}
