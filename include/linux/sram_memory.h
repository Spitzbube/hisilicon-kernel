/******************************************************************************
 *    SRAM memory manager
 *    Copyright (c) 2013-2023 by Hisilicon.
 *    All rights reserved.
 * ***
 *    Create By Lai Yingjun.
 *
******************************************************************************/

#ifndef SRAM_MEMORYH
#define SRAM_MEMORYH
void *sram_alloc(unsigned long size, unsigned int *phys_addr,
		 unsigned long flags);
void sram_free(void *vaddr);
int sram_pm_save(void);
int sram_pm_resume(void);

#endif /* SRAM_MEMORYH */
