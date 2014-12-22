#include <linux/platform_device.h>
#include <asm/pmu.h>
#include <mach/irqs.h>
#include <mach/cpu-info.h>

static struct resource  A9_pmu_resources[] = {
	{
		.start = INTNR_A9_PMU_INT,
		.end   = INTNR_A9_PMU_INT,
		.flags = IORESOURCE_IRQ,
	},
	{
		.start = INTNR_A9_PMU_INT+1,
		.end   = INTNR_A9_PMU_INT+1,
		.flags = IORESOURCE_IRQ,
	},
	{
		.start = INTNR_A9_PMU_INT+2,
		.end   = INTNR_A9_PMU_INT+2,
		.flags = IORESOURCE_IRQ,
	},
	{
		.start = INTNR_A9_PMU_INT+3,
		.end   = INTNR_A9_PMU_INT+3,
		.flags = IORESOURCE_IRQ,
	},
};

static struct resource  A9_pmu_resources2[] = {
	{
		.start = INTNR_A9_PMU_INT,
		.end   = INTNR_A9_PMU_INT,
		.flags = IORESOURCE_IRQ,
	},
	{
		.start = INTNR_A9_PMU_INT+1,
		.end   = INTNR_A9_PMU_INT+1,
		.flags = IORESOURCE_IRQ,
	},
};

static struct platform_device A9_pmu_device = {
	.name = "arm-pmu",
	.id   = -1, //ARM_PMU_DEVICE_CPU,
	.resource = &A9_pmu_resources[0],
	.num_resources = ARRAY_SIZE(A9_pmu_resources),
};

static struct platform_device A9_pmu_device2 = {
	.name = "arm-pmu",
	.id   = -1, //ARM_PMU_DEVICE_CPU,
	.resource = &A9_pmu_resources2[0],
	.num_resources = ARRAY_SIZE(A9_pmu_resources2),
};

static int __init pmu_init(void)
{
	unsigned long long chipid = get_chipid();

	if ((chipid == _HI3798CV100A) ||
			(chipid == _HI3798CV100) ||
			(get_chipid() == _HI3796CV100))
	{
		platform_device_register(&A9_pmu_device);
	}
	else
	{
		platform_device_register(&A9_pmu_device2);
	}

	return 0;
};
arch_initcall(pmu_init);
