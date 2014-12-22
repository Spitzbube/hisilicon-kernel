/* linux/arch/arm/mach-godnet/platsmp.c
 *
 * clone form linux/arch/arm/mach-realview/platsmp.c
 *
 *  Copyright (C) 2002 ARM Ltd.
 *  All Rights Reserved
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/jiffies.h>
#include <linux/smp.h>
#include <linux/io.h>
#include <asm/hardware/gic.h>
#include <asm/cacheflush.h>
#include <mach/hardware.h>
#include <asm/mach-types.h>
#include <asm/unified.h>
#include <asm/smp_scu.h>
#include <mach/early-debug.h>

#include <mach/cpu-info.h>

#include "platsmp.h"

unsigned long scureg_base = (unsigned long) &scureg_base;


/*
 * control for which core is the next to come out of the secondary
 * boot "holding pen"
 */
//int __cpuinitdata pen_release = -1;

/* copy startup code to sram, and flash cache. */
/*static*/ void set_scu_boot_addr(unsigned int start_addr, /* slave start phy address */
				     unsigned int jump_addr)  /* slave jump phy address */
{
	unsigned int *virtaddr;
	unsigned int *p_virtaddr;

	p_virtaddr = virtaddr = ioremap(start_addr, PAGE_SIZE);

	*p_virtaddr++ = 0xe51ff004; /* ldr  pc, [pc, #-4] */
	*p_virtaddr++ = jump_addr;  /* pc jump phy address */

	smp_wmb();
	__cpuc_flush_dcache_area((void *)virtaddr,
		(size_t)((char *)p_virtaddr - (char *)virtaddr));
	outer_clean_range(__pa(virtaddr), __pa(p_virtaddr));

	iounmap(virtaddr);
}

/*
 * Write pen_release in a way that is guaranteed to be visible to all
 * observers, irrespective of whether they're taking part in coherency
 * or not.  This is necessary for the hotplug code to work reliably.
 */
/*static*/ void __cpuinit write_pen_release(int val)
{
	pen_release = val;
	smp_wmb();
	__cpuc_flush_dcache_area((void *)&pen_release, sizeof(pen_release));
	outer_clean_range(__pa(&pen_release), __pa(&pen_release + 1));
}

static void __iomem *scu_base_addr(void)
{
	return (void __iomem *)IO_ADDRESS(REG_BASE_A9_PERI + REG_A9_PERI_SCU);
}

static DEFINE_SPINLOCK(boot_lock);

void __cpuinit s40_secondary_init(unsigned int cpu)
{
	/*
	 * 1. enable L1 prefetch                       [2]
	 * 2. enable L2 prefetch hint                  [1]a
	 * 3. enable write full line of zeros mode.    [3]a
	 * 4. enable allocation in one cache way only. [8]
	 *   a: This feature must be enabled only when the slaves
	 *      connected on the Cortex-A9 AXI master port support it.
	 */
	asm volatile (
	"	mrc	p15, 0, r0, c1, c0, 1\n"
	"	orr	r0, r0, #0x0104\n"
	"	orr	r0, r0, #0x02\n"
	"	mcr	p15, 0, r0, c1, c0, 1\n"
	  :
	  :
	  : "r0", "cc");

	/*
	 * if any interrupts are already enabled for the primary
	 * core (e.g. timer irq), then they will not have been enabled
	 * for us: do so
	 */
//	gic_secondary_init(0);

	/*
	 * let the primary processor know we're out of the
	 * pen, then head off into the C entry point
	 */
	write_pen_release(-1);

	/*
	 * Synchronise with the boot thread.
	 */
	spin_lock(&boot_lock);
	spin_unlock(&boot_lock);
}

/* relase pen and then the slave core run into our world */
int __cpuinit s40_boot_secondary(unsigned int cpu, struct task_struct *idle)
{
	unsigned long timeout;

	set_scu_boot_addr(0xFFFF0000,
		(unsigned int)virt_to_phys(s40_secondary_startup));

	/*
	 * set synchronisation state between this boot processor
	 * and the secondary one
	 */
	spin_lock(&boot_lock);

	if ((get_chipid() == _HI3798CV100A) ||
			(get_chipid() == _HI3798CV100) ||
			(get_chipid() == _HI3796CV100))
	{
		s5_scu_power_up(cpu);
	}
	else
	{
		s40_scu_power_up(cpu);
	}

	/*
	 * The secondary processor is waiting to be released from
	 * the holding pen - release it, then wait for it to flag
	 * that it has been released by resetting pen_release.
	 *
	 * Note that "pen_release" is the hardware CPU ID, whereas
	 * "cpu" is Linux's internal ID.
	 */
	write_pen_release(cpu);

	arch_send_wakeup_ipi_mask(cpumask_of(cpu));

	/*
	 * Send the secondary CPU a soft interrupt, thereby causing
	 * the boot monitor to read the system wide flags register,
	 * and branch to the address found there.
	 */
	timeout = jiffies + (5 * HZ);
	while (time_before(jiffies, timeout)) {
		smp_rmb();
		if (pen_release == -1)
			break;

		udelay(10);
	}

	/*
	 * now the secondary core is starting up let it run its
	 * calibrations, then wait for it to finish
	 */
	spin_unlock(&boot_lock);

	return pen_release != -1 ? -ENOSYS : 0;
}

/*
 * Initialise the CPU possible map early - this describes the CPUs
 * which may be present or become present in the system.
 */
void __init s40_smp_init_cpus(void)
{
	void __iomem *scu_base = scu_base_addr();
	unsigned int i, ncores;

	scureg_base = (unsigned long)scu_base;
	ncores = scu_base ? scu_get_core_count(scu_base) : 1;

	/* sanity check */
	if (ncores > NR_CPUS) {
		printk(KERN_WARNING
		       "Realview: no. of cores (%d) greater than configured "
		       "maximum of %d - clipping\n",
		       ncores, NR_CPUS);
		ncores = NR_CPUS;
	}

	for (i = 0; i < ncores; i++)
		set_cpu_possible(i, true);

#if 0
	set_smp_cross_call(gic_raise_softirq);
#endif
}

void s40_scu_power_up(int cpu)
{
	unsigned int regval;
	static int init_flags = 0;

	if (!init_flags) {
		writel(0x3, (volatile void*)(IO_ADDRESS(REG_PERI_PMC3)));
		writel(0xffff88, (volatile void*)(IO_ADDRESS(REG_PERI_PMC1)));
		init_flags++;
	}

	regval = readl((volatile void*)(IO_ADDRESS(REG_BASE_PMC)));
	/* a9_core1_pd_req=0, enable core1 power*/
	regval &= ~(1 << 3);
	/* a9_core1_wait_mtcoms_ack=0, no wait ack */
	regval &= ~(1 << 8);
	/* a9_core1_pmc_enable=1, PMC control power up cycle */
#if 0
	regval |= (1 << 7);
#else
	regval |= (1 << 0);
#endif

//	printk(KERN_DEBUG "CPU%u: powerup\n", cpu);

	writel(regval, (volatile void*)(IO_ADDRESS(REG_BASE_PMC)));

	/* clear the slave cpu reset */
	regval = readl((volatile void*)(IO_ADDRESS(A9_REG_BASE_RST)));
	regval &= ~(1 << 17);
	writel(regval, (volatile void*)(IO_ADDRESS(A9_REG_BASE_RST)));
}

void s5_scu_power_up(int cpu)
{
	unsigned int regval;
	static int init_flags = 0;

	if (!init_flags) {
		writel(0x3, (volatile void*)(IO_ADDRESS(REG_PERI_PMC3)));
		writel(0xffff88, (volatile void*)(IO_ADDRESS(REG_PERI_PMC1)));
		init_flags++;
	}

	regval = readl((volatile void*)(IO_ADDRESS(REG_BASE_PMC)));

	switch (cpu)
	{
	case 0:
	case 1:
		break;
	case 2:
		regval &= ~0x188;
		regval |= 0x80;
		break;
	default:
		regval &= ~0x188000;
		regval |= 0x80000;
		break;
	}

//	printk(KERN_DEBUG "CPU%u: powerup\n", cpu);

	writel(regval, (volatile void*)(IO_ADDRESS(REG_BASE_PMC)));

	/* clear the slave cpu reset */
	regval = readl((volatile void*)(IO_ADDRESS(A9_REG_BASE_RST)));
	regval &= ~(1 << (cpu+12));
	writel(regval, (volatile void*)(IO_ADDRESS(A9_REG_BASE_RST)));
}

/* TODO */
void s40_cpu_die(unsigned int cpu)
{
	unsigned int regval;

	v7_flush_kern_cache_all();

	if ((get_chipid() == _HI3798CV100A) ||
			(get_chipid() == _HI3798CV100) ||
			(get_chipid() == _HI3796CV100))
	{
		switch (cpu)
		{
		case 0:
		case 1:
			regval = readl((volatile void*)(IO_ADDRESS(REG_BASE_PMC)));
			regval |= (1 << (cpu+12));
			writel(regval, (volatile void*)(IO_ADDRESS(REG_BASE_PMC)));
			__asm volatile("wfi");
			break;
		case 2:
			regval = readl((volatile void*)(IO_ADDRESS(REG_BASE_PMC)));
			regval |= 0x18;
			writel(regval, (volatile void*)(IO_ADDRESS(REG_BASE_PMC)));
			__asm volatile("wfi");
			break;
		default:
			regval = readl((volatile void*)(IO_ADDRESS(REG_BASE_PMC)));
			regval |= 0x18000;
			writel(regval, (volatile void*)(IO_ADDRESS(REG_BASE_PMC)));
			__asm volatile("wfi");
			break;
		}
	}
	else
	{
		/* a9_core1_pd_req=0, enable core1 power*/
		regval = readl((volatile void*)(IO_ADDRESS(REG_BASE_PMC)));
		regval |= (1 << 3);
		writel(regval, (volatile void*)(IO_ADDRESS(REG_BASE_PMC)));
		__asm volatile("wfi");
	}

	BUG();
}

/* send  start addr to slave cores */
void __init s40_smp_prepare_cpus(unsigned int max_cpus)
{
#if 0
	int i;

	/*
	 * Initialise the present map, which describes the set of CPUs
	 * actually populated at the present time.
	 */
	for (i = 0; i < max_cpus; i++)
		set_cpu_present(i, true);
#endif

	scu_enable(scu_base_addr());
}
