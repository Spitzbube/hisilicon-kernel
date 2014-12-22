#
# Makefile for the linux kernel.
#


KBUILD_AFLAGS :=$(KBUILD_AFLAGS:-msoft-float=-Wa,-mfpu=softvfp+vfp)
LDFLAGS +=--no-warn-mismatch

obj-y := core.o clock.o timer.o tags.o
obj-$(CONFIG_HW_PERF_EVENTS) += pmu.o
obj-$(CONFIG_CMA) += hi_cma.o
#obj-$(CONFIG_ION_CMA) += hi_ion.o
obj-$(CONFIG_PM) += hi_pm_sleep.o hipm.o
obj-$(CONFIG_SMP) += platsmp.o headsmp.o
#obj-$(CONFIG_HOTPLUG_CPU) += hotplug.o
obj-y += cpu/
