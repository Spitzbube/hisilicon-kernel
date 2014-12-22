#include <linux/init.h>
#include <linux/device.h>
#include <linux/dma-mapping.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/amba/bus.h>
#include <linux/amba/clcd.h>
#include <linux/clocksource.h>
#include <linux/clockchips.h>
#include <linux/cnt32_to_63.h>
#include <linux/io.h>

#include <linux/clkdev.h>
#include <asm/system.h>
#include <asm/irq.h>
//#include <asm/leds.h>
#include <asm/hardware/arm_timer.h>
#include <asm/hardware/gic.h>
//#include <asm/hardware/vic.h>
#include <asm/mach-types.h>

#include <asm/mach/arch.h>
#include <asm/mach/flash.h>
#include <asm/mach/irq.h>
#include <asm/mach/time.h>
#include <asm/mach/map.h>
#include <asm/sched_clock.h>
#include <mach/hardware.h>
#include <mach/early-debug.h>
#include <mach/irqs.h>
#include "mach/clock.h"

#include <linux/bootmem.h>
#include <mach/cpu-info.h>
#include <linux/delay.h>
#include <asm/hardware/cache-l2x0.h>
#include <asm/smp_twd.h>
#include <linux/memblock.h>
#include <linux/tags.h>

#include <asm/smp_scu.h>

#if 0
+#ifdef CONFIG_PM
+extern unsigned long hi_pm_phybase;
+extern unsigned long hi_pm_ddrbase;
+#endif
#endif

#ifdef CONFIG_CMA
extern int hisi_declare_heap_memory(void);
#endif

#if 0
+#ifdef CONFIG_ION_CMA
+extern int hisi_declare_ion_memory(void);
+extern int hisi_register_ion_device(void);
+#endif
+
+#ifdef CONFIG_SUPPORT_SRAM_MANAGER
+extern int init_sram_manager(void);
+#endif
#endif

extern void arch_cpu_init(void);

void __iomem *s40_gic_cpu_base_addr = (void __iomem *)CFG_GIC_CPU_BASE;

void __init s40_gic_init_irq(void)
{
	edb_trace();
#if 0
	s40_gic_cpu_base_addr = (void __iomem *)CFG_GIC_CPU_BASE;
#endif

#ifndef CONFIG_LOCAL_TIMERS
	gic_init(0, HISI_GIC_IRQ_START, (void __iomem *)
		 CFG_GIC_DIST_BASE, s40_gic_cpu_base_addr);
#else
	/* git initialed include Local timer.
	* IRQ_LOCALTIMER is settled IRQ number for local timer interrupt.
	* It is set to 29 by ARM.
	*/
	gic_init(0, IRQ_LOCALTIMER, (void __iomem *)(CFG_GIC_DIST_BASE),
		 (void __iomem *)(CFG_GIC_CPU_BASE));
#endif
}

static struct map_desc s40_io_desc[] __initdata = {
	/* S40_IOCH1 */
	{
		.virtual	= S40_IOCH1_VIRT,
		.pfn		= __phys_to_pfn(S40_IOCH1_PHYS),
		.length		= S40_IOCH1_SIZE,
		.type		= MT_DEVICE
	},
	/* ? */
	{
		.virtual	= S40_IOCH2_VIRT,
		.pfn		= __phys_to_pfn(S40_IOCH2_PHYS),
		.length		= S40_IOCH2_SIZE,
		.type		= MT_DEVICE
	},
};

/*
 * pdm_tag format:
 * version=1.0.0.0 baseparam=0x86000000,0x1000 logdata=0x870000000,0x2000
 */
void __init pdm_reserve_mem(void)
{
	char pdm_tag[512];
	int  taglen;
	char  *p, *str;
	unsigned int phyaddr;
	unsigned int addrlen;
	int ret = -EINVAL;

#define EQUAL_MARK "="

	memset(pdm_tag, 0x0, sizeof(pdm_tag));

	taglen = get_param_data("pdm_tag", pdm_tag, sizeof(pdm_tag));
	if (taglen <= 0)
		return;

	if (taglen >= sizeof(pdm_tag))
		goto error;

	/* Skip the first "=" */
	p = strstr(pdm_tag, EQUAL_MARK);
	if (!p)
		goto error;

	p += sizeof(EQUAL_MARK) - 1;
	if (p >= pdm_tag + taglen)
		goto error;

	while (p < pdm_tag + taglen) {
		p = strstr(p, EQUAL_MARK);
		if (!p)
			goto error;

		p += sizeof(EQUAL_MARK) - 1;

		str = p;
		while (p < (pdm_tag + taglen) && *p != ',')
			p++;

		if (p >= (pdm_tag + taglen))
			goto error;

		*p = '\0';
		phyaddr = simple_strtoul(str, NULL, 16);
		*p++ = ',';

		if (phyaddr & (SZ_4K - 1))
			goto error;

		str = p;
		while (p < (pdm_tag + taglen) && *p != ' ')
			p++;

		if (p >= (pdm_tag + taglen))
			goto error;

		*p = '\0';
		addrlen = simple_strtoul(str, NULL, 16);
		*p = ' ';

		if (addrlen & (SZ_4K - 1))
			goto error;

		ret = memblock_reserve(phyaddr, addrlen);
		if (ret)
			goto error;
	}

	return;
error:
	printk(KERN_ERR "Invalid PDM tag, errno:%d\n", ret);
#undef EQUAL_MARK
}

void __init s40_map_io(void)
{
	int i;

	iotable_init(s40_io_desc, ARRAY_SIZE(s40_io_desc));

	for (i = 0; i < ARRAY_SIZE(s40_io_desc); i++) {
		edb_putstr(" V: ");	edb_puthex(s40_io_desc[i].virtual);
		edb_putstr(" P: ");	edb_puthex(s40_io_desc[i].pfn);
		edb_putstr(" S: ");	edb_puthex(s40_io_desc[i].length);
		edb_putstr(" T: ");	edb_putul(s40_io_desc[i].type);
		edb_putstr("\n");
	}

	edb_trace();
}
/*****************************************************************************/

#define HIL_AMBADEV_NAME(name) hil_ambadevice_##name

#define HIL_AMBA_DEVICE(name, busid, base, platdata)		\
static struct amba_device HIL_AMBADEV_NAME(name) =		\
{								\
	.dev		= {					\
		.coherent_dma_mask = ~0,			\
		.init_name = busid,				\
		.platform_data = platdata,			\
	},							\
	.res		= {					\
		.start	= REG_BASE_##base,			\
		.end	= REG_BASE_##base + 0x1000 - 1,		\
		.flags	= IORESOURCE_IO,			\
	},							\
	.dma_mask	= ~0,					\
	.irq		= { INTNR_##base, INTNR_##base }	\
}

HIL_AMBA_DEVICE(uart0, "uart:0",  UART0,    NULL);
HIL_AMBA_DEVICE(uart1, "uart:1",  UART1,    NULL);
HIL_AMBA_DEVICE(uart2, "uart:2",  UART2,    NULL);
HIL_AMBA_DEVICE(uart3, "uart:3",  UART3,    NULL);
HIL_AMBA_DEVICE(uart4, "uart:4",  UART4,    NULL);

static struct amba_device *amba_devs1[] __initdata = {
	&HIL_AMBADEV_NAME(uart0),
	&HIL_AMBADEV_NAME(uart1),
	&HIL_AMBADEV_NAME(uart2),
};

static struct amba_device *amba_devs2[] __initdata = {
	&HIL_AMBADEV_NAME(uart0),
	&HIL_AMBADEV_NAME(uart1),
	&HIL_AMBADEV_NAME(uart2),
	&HIL_AMBADEV_NAME(uart3),
	&HIL_AMBADEV_NAME(uart4),
};

/*
 * These are fixed clocks.
 */

static struct clk sp804_clk = {
	.rate	= 24000000, /* TODO: XXX */
};

static struct clk_lookup lookups[] = {
	{ /* SP804 timers */
		.dev_id		= "sp804",
		.clk		= &sp804_clk,
	},
};

static void __init s40_reserve(void)
{
	/* Reserve memory for PDM modoule*/
	pdm_reserve_mem();

#ifdef CONFIG_SUPPORT_DSP_RUN_MEM
	/* Reserve memory for DSP */
	BUG_ON(memblock_reserve(CONFIG_DSP_RUN_MEM_ADDR,
		CONFIG_DSP_RUN_MEM_SIZE));

	printk(KERN_NOTICE "DSP run memory space at 0x%08X, size: 0x%08x Bytes.\n",
		CONFIG_DSP_RUN_MEM_ADDR,
		CONFIG_DSP_RUN_MEM_SIZE);
#endif

#ifdef CONFIG_CMA
	hisi_declare_heap_memory();
#endif

#ifdef CONFIG_ION_CMA
	hisi_declare_ion_memory();
#endif
}
/*****************************************************************************/

void __init s40_init(void)
{
	unsigned long i;

	edb_trace();

	if ((get_chipid() == _HI3719MV100) ||
			(get_chipid() == _HI3718MV100) ||
			(get_chipid() == _HI3798CV100A) ||
			(get_chipid() == _HI3798CV100) ||
			(get_chipid() == _HI3796CV100))
	{
		for (i = 0; i < ARRAY_SIZE(amba_devs1); i++) {
			edb_trace();
			amba_device_register(amba_devs1[i], &iomem_resource);
		}
	}
	else
	{
		for (i = 0; i < ARRAY_SIZE(amba_devs2); i++) {
			edb_trace();
			amba_device_register(amba_devs2[i], &iomem_resource);
		}
	}

#ifdef CONFIG_ION_CMA
	hisi_register_ion_device();
#endif

#ifdef CONFIG_SUPPORT_SRAM_MANAGER
//	init_sram_manager();
#endif
}
#if 0
+/*****************************************************************************/
+
+#ifdef CONFIG_CACHE_L2X0
+static int __init l2_cache_init(void)
+{
+ void __iomem *l2x0_base = (void __iomem *)IO_ADDRESS(REG_BASE_L2CACHE);
+ /* L2 icache pre-fetch is ON, dcache pre-fetch is OFF */
+ /* FIXME: 32way, 16 Associativity */
+
+ l2x0_init(l2x0_base, 0x00450000, 0xFFB0FFFF);
+
+ return 0;
+}
+
+early_initcall(l2_cache_init);
+#endif
+
+/*****************************************************************************/
#endif

static void __init s40_init_early(void)
{
	clkdev_add_table(lookups, ARRAY_SIZE(lookups));

	arch_cpu_init();

	/*
	 * 1. enable L1 prefetch                       [2]
	 * 4. enable allocation in one cache way only. [8]
	 */
	asm volatile (
	"	mrc	p15, 0, r0, c1, c0, 1\n"
	"	orr	r0, r0, #0x104\n"
	"	mcr	p15, 0, r0, c1, c0, 1\n"
	  :
	  :
	  : "r0", "cc");

	edb_trace();
}
/*****************************************************************************/

void s40_restart(char mode, const char *cmd)
{
	u32 data;

	printk(KERN_INFO "Cpu will restart.");

	mdelay(200);

	local_irq_disable();

	data = readl((volatile void *)0xf9a22178);
	data &= ~(1 << 4);
	data |= 1;
	writel(data, (volatile void *)0xf9a22178);

	udelay(1);

	/* unclock wdg */
	writel(0x1ACCE551,  (volatile void *)(IO_ADDRESS(REG_BASE_WDG0 + 0xc00)));
	/* wdg load value */
	writel(0x00000100,  (volatile void *)(IO_ADDRESS(REG_BASE_WDG0 + 0x0)));
	/* bit0: int enable bit1: reboot enable */
	writel(0x00000003,  (volatile void *)(IO_ADDRESS(REG_BASE_WDG0 + 0x8)));

	while (1);

	BUG();
}
/*****************************************************************************/

extern void __init s40_smp_init_cpus(void);
extern void __init s40_smp_prepare_cpus(unsigned int max_cpus);
extern void __cpuinit s40_secondary_init(unsigned int cpu);
extern int __cpuinit s40_boot_secondary(unsigned int cpu, struct task_struct *idle);
extern void s40_cpu_die(unsigned int cpu);

static struct smp_operations s40_smp /*__initdata*/ =
{
	.smp_init_cpus = s40_smp_init_cpus,
	.smp_prepare_cpus = s40_smp_prepare_cpus,
	.smp_secondary_init = s40_secondary_init,
	.smp_boot_secondary = s40_boot_secondary,
	.cpu_die = s40_cpu_die,
};

/*****************************************************************************/

#if 0
extern struct sys_timer s40_sys_timer;
#else
extern void __init s40_timer_init(void);
#endif

MACHINE_START(/*S40*/GODBOX, "bigfish")
 .atag_offset = 0x100,
 .map_io = s40_map_io,
 .init_early = s40_init_early,
 .init_irq = s40_gic_init_irq,
#if 0
 .handle_irq = gic_handle_irq,
 .timer = &s40_sys_timer,
#else
 .init_time = s40_timer_init,
#endif
 .init_machine = s40_init,
 .reserve = s40_reserve,
 .restart = s40_restart,
 .smp = smp_ops(s40_smp),
MACHINE_END
