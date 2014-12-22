#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/pm.h>
#include <linux/suspend.h>
#include <asm/memory.h>
//#include <mach/system.h>
#include <mach/early-debug.h>
#include <mach/platform.h>
#include <linux/delay.h>
#include <linux/suspend.h>
#include <linux/syscalls.h>
#include <asm/mach/time.h>
#include <linux/slab.h>
#include <asm/hardware/gic.h>
#include <asm/hardware/arm_timer.h>
#include <linux/kmemleak.h>
#include <linux/device.h>
#include <linux/sram_memory.h>
#include <linux/io.h>

void __iomem *hi_sc_virtbase;

void __iomem *hi_crg_virtbase;
#if 0
void __iomem *hi_scu_virtbase;
#endif

/*ddr address for save cpu context*/
unsigned int hi_pm_ddrbase = (unsigned int) &hi_pm_ddrbase;
unsigned int hi_pm_phybase = (unsigned int) &hi_pm_phybase;
unsigned int hi_pm_cpu0_len = (unsigned int) &hi_pm_cpu0_len;
#define PM_CTX_BUF_SIZE  (1024) /*size of saved context*/

unsigned long saved_interrupt_mask[128];
unsigned long saved_cpu_target_mask[128];
asmlinkage void hi_pm_sleep(void);

#ifdef CONFIG_CACHE_L2X0
extern int hi_pm_disable_l2cache(void);
extern int hi_pm_enable_l2cache(void);
#endif

extern void __iomem *s40_gic_cpu_base_addr;
static void __iomem *gic_dist_base_addr = (void __iomem *)CFG_GIC_DIST_BASE;

static int hi_pm_save_gic(void)
{
	unsigned int max_irq, i;
	unsigned int intack;

	//gic_dist_base_addr = (void __iomem *)CFG_GIC_DIST_BASE;

	/* disable gic dist */
	writel(0, gic_dist_base_addr + GIC_DIST_CTRL);

	/*
	 * Find out how many interrupts are supported.
	 */
	max_irq = readl(gic_dist_base_addr + GIC_DIST_CTR) & 0x1f;
	max_irq = (max_irq + 1) * 32;

	/*
	 * The GIC only supports up to 1020 interrupt sources.
	 * Limit this to either the architected maximum, or the
	 * platform maximum.
	 */
#if 0
	if (max_irq > max(1020, NR_IRQS)) {
		max_irq = max(1020, NR_IRQS);
	}
#else
	if (max_irq < 1020)
		max_irq = 1020;
#endif

	/* save Dist target */
	for (i = 32; i < max_irq; i += 4) {
		saved_cpu_target_mask[i / 4] =
		    readl(gic_dist_base_addr + GIC_DIST_TARGET + i * 4 / 4);
	}

	/* save mask irq */
	for (i = 0; i < max_irq; i += 32) {
		saved_interrupt_mask[i / 32] =
		    readl(gic_dist_base_addr + GIC_DIST_ENABLE_SET +
			  i * 4 / 32);
	}

	/* clear all interrupt */
	for (i = 0; i < max_irq; i += 32) {
		writel(0xffffffff,
		       gic_dist_base_addr + GIC_DIST_ENABLE_CLEAR + i * 4 / 32);
	}

	/* read INT_ACK in CPU interface, until result is 1023 */
	for (i = 0; i < max_irq; i++) {
		intack = readl(s40_gic_cpu_base_addr + 0x0c);
		if (1023 == intack) {
			break;
		}
		writel(intack, s40_gic_cpu_base_addr + 0x10);
	}

#if 0   /* comment off wakeup intr, cause we will go directly to deepsleep */
	/* enable softinterrupt mask */
	writel(0xffff, gic_dist_base_addr + GIC_DIST_ENABLE_SET);

	/* enable KPC/TBC/RTC interrupt */
	writel(GET_IRQ_BIT(IRQ_KPC) | GET_IRQ_BIT(IRQ_TBC) |
	       GET_IRQ_BIT(IRQ_RTC),
	       gic_dist_base_addr + GIC_DIST_ENABLE_SET + 4);

	writel(0, gic_dist_base_addr + GIC_DIST_ENABLE_SET + 8);

	/* enable all gpio interrupt */
	writel(0x3fffff, gic_dist_base_addr + GIC_DIST_ENABLE_SET + 0xc);
#endif
	/* Enable GIC Dist Ctrl, SR needed */
	/* no intr response mode, need not enable dist ctrl */
	//writel(1, gic_dist_base_addr + GIC_DIST_CTRL);

	return 0;
}

static int hi_pm_retore_gic(void)
{
	unsigned int max_irq, i;

	/* PRINT OUT the GIC Status */
	unsigned int irq_status[5];

	for (i = 0; i < 5; i++) {
		irq_status[i] = readl(gic_dist_base_addr + 0xd00 + i * 4);
	}

	writel(0, gic_dist_base_addr + GIC_DIST_CTRL);
	writel(0, s40_gic_cpu_base_addr + GIC_CPU_CTRL);

	/*
	 * Find out how many interrupts are supported.
	 */
	max_irq = readl(gic_dist_base_addr + GIC_DIST_CTR) & 0x1f;
	max_irq = (max_irq + 1) * 32;

	/*
	 * The GIC only supports up to 1020 interrupt sources.
	 * Limit this to either the architected maximum, or the
	 * platform maximum.
	 */
#if 0
	if (max_irq > max(1020, NR_IRQS)) {
		max_irq = max(1020, NR_IRQS);
	}
#else
	if (max_irq < 1020)
		max_irq = 1020;
#endif

	/*
	 * Set all global interrupts to be level triggered, active low.
	 */
	for (i = 32; i < max_irq; i += 16) {
		writel(0, gic_dist_base_addr + GIC_DIST_CONFIG + i * 4 / 16);
	}

	/*
	 * Set all global interrupts to this CPU only.
	 */
	for (i = 32; i < max_irq; i += 4) {
		writel(saved_cpu_target_mask[i / 4],
				gic_dist_base_addr + GIC_DIST_TARGET + i * 4 / 4);
	}

	/*
	 * Set priority on all interrupts.
	 */
	for (i = 0; i < max_irq; i += 4) {
		writel(0xa0a0a0a0,
				gic_dist_base_addr + GIC_DIST_PRI + i * 4 / 4);
	}

	/*
	 * Disable all interrupts.
	 */
	for (i = 0; i < max_irq; i += 32) {
		writel(0xffffffff,
				gic_dist_base_addr + GIC_DIST_ENABLE_CLEAR + i * 4 / 32);
	}

	for (i = 0; i < max_irq; i += 32) {
		writel(saved_interrupt_mask[i / 32], gic_dist_base_addr +
		       GIC_DIST_ENABLE_SET + i * 4 / 32);
	}

	writel(1, gic_dist_base_addr + GIC_DIST_CTRL);

	/* set the BASE priority 0xf0 */
	writel(0xf0, s40_gic_cpu_base_addr + GIC_CPU_PRIMASK);

	writel(1, s40_gic_cpu_base_addr + GIC_CPU_CTRL);

	return 0;
}

static unsigned long timer0_base_addr;

typedef struct __timer_register {
	unsigned long timer_load;
	unsigned long timer_value;
	unsigned long timer_ctrl;
	unsigned long timer_bgload;
} timer_register;

static timer_register timer0[2];

/* Protect and restore timer0_0 timer0_1 registert value */
static int protect_timer0_register(void)
{

	/* protect timer0_0 / timer0_1  timer_load value */
	timer0[0].timer_load = readl(timer0_base_addr + TIMER_LOAD);
	timer0[1].timer_load = readl(timer0_base_addr + 0x20 + TIMER_LOAD);

	/* protect timer0_0 / timer0_1  timer_load value */
	timer0[0].timer_value = readl(timer0_base_addr + TIMER_VALUE);
	timer0[1].timer_value = readl(timer0_base_addr + 0x20 + TIMER_VALUE);

	/* protect timer0_0 timer0_1 timer_ctrl value */
	timer0[0].timer_ctrl = readl(timer0_base_addr + TIMER_CTRL);
	timer0[1].timer_ctrl = readl(timer0_base_addr + 0x20 + TIMER_CTRL);

	/* protect timer0_0 timer0_1 timer_bgload value */
	timer0[0].timer_bgload = readl(timer0_base_addr + TIMER_BGLOAD);
	timer0[1].timer_bgload = readl(timer0_base_addr + 0x20 + TIMER_BGLOAD);

	/* disable timer0_0 timer0_1 */
	writel(0, timer0_base_addr + TIMER_CTRL);
	writel(0, timer0_base_addr + 0x20 + TIMER_CTRL);

	return 0;
}

static int restore_timer0_register(void)
{

	/* disable timer0_0 timer0_1 */
	writel(0, timer0_base_addr + TIMER_CTRL);
	writel(0, timer0_base_addr + 0x20 + TIMER_CTRL);

	/* clear timer0_0 timer0_1 intr */
	writel(1, timer0_base_addr + TIMER_INTCLR);
	writel(1, timer0_base_addr + 0x20 + TIMER_INTCLR);

	/* restore timer0_0 timer0_1 load value to load before enable */
	writel(timer0[0].timer_value, timer0_base_addr + TIMER_LOAD);
	writel(timer0[1].timer_value, timer0_base_addr + 0x20 + TIMER_LOAD);

	/* restore timer0_0 timer0_1 ctrl value */
	writel(timer0[0].timer_ctrl, timer0_base_addr + TIMER_CTRL);
	writel(timer0[1].timer_ctrl, timer0_base_addr + 0x20 + TIMER_CTRL);

	/* restore timer0_0 timer0_1 bgload value. when reg value to zero then to 
	   bgload */
	writel(timer0[0].timer_bgload, timer0_base_addr + TIMER_BGLOAD);
	writel(timer0[1].timer_bgload, timer0_base_addr + 0x20 + TIMER_BGLOAD);

	return 0;
}

static int hi_pm_suspend(void)
{
	/* int ret = 0; */
	unsigned long flage = 0;

	/* disable irq */
	local_irq_save(flage);

	/* save gic */
	hi_pm_save_gic();

	/*save & disable l2 cache */
#ifdef CONFIG_CACHE_L2X0
	hi_pm_disable_l2cache();
#endif

#ifdef CONFIG_SUPPORT_SRAM_MANAGER
	sram_pm_save();
#endif

#if 0
	/* save & diable timer0_1 */
	protect_timer0_register();
#endif

	hi_pm_sleep();

#if 0
	/* restore & enable timer0_1 */
	restore_timer0_register();
#endif

#ifdef CONFIG_SUPPORT_SRAM_MANAGER
	sram_pm_resume();
#endif

	/*restore & enable l2 cache */
#ifdef CONFIG_CACHE_L2X0
	hi_pm_enable_l2cache();
#endif

	/* restore gic */
	hi_pm_retore_gic();

	/* enable irq */
	local_irq_restore(flage);

	return 0;
}

/* TODO */
static int hi_pm_enter(suspend_state_t state)
{
	int ret = 0;
	switch (state) {
	case PM_SUSPEND_STANDBY:
	case PM_SUSPEND_MEM:
		ret = hi_pm_suspend();
		break;
	default:
		ret = -EINVAL;
	}

	return ret;
}

int hi_pm_valid(suspend_state_t state)
{
	return 1;
}

static const struct platform_suspend_ops hi_pm_ops = {
	.enter = hi_pm_enter,
	.valid = hi_pm_valid,
};

static int __init hi_pm_init(void)
{
	hi_sc_virtbase = (void __iomem *)IO_ADDRESS(REG_BASE_SCTL);
	hi_crg_virtbase = (void __iomem *)IO_ADDRESS(REG_BASE_CRG);

	hi_pm_ddrbase =
	    (unsigned int)kzalloc((PM_CTX_BUF_SIZE), GFP_DMA | GFP_KERNEL);
	hi_pm_phybase = __pa(hi_pm_ddrbase);
	/*
	 * Because hi_pm_ddrbase is saved in .text of hi_pm_sleep.S, the kmemleak,
	 * which not check the .text, reports a mem leak here ,
	 * so we suppress kmemleak messages.
	 */
	kmemleak_not_leak(hi_pm_ddrbase);

#if 0
	/* get the base address of timer0 */
	timer0_base_addr =
	    (unsigned long)IO_ADDRESS(REG_BASE_A9_PERI) +
	    REG_A9_PERI_GLOBAL_TIMER;
#endif

	suspend_set_ops(&hi_pm_ops);
	return 0;
}

module_init(hi_pm_init);

