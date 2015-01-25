#===============================================================================
# export variables
#===============================================================================
ifeq ($(CFG_HI_EXPORT_FLAG),)
    ifneq ($(KERNELRELEASE),)
    KERNEL_DIR := $(srctree)

    SDK_DIR := $(KERNEL_DIR)/../../..
    else
    SDK_DIR := $(CURDIR)/../../../..
    endif

include $(SDK_DIR)/base.mak
endif

#===============================================================================
# local variables
#===============================================================================
EXTRA_CFLAGS += $(CFG_HI_KMOD_CFLAGS)

EXTRA_CFLAGS += -I$(COMMON_UNF_INCLUDE)                 \
                -I$(COMMON_API_INCLUDE)                 \
                -I$(COMMON_DRV_INCLUDE)                 \
                -I$(MSP_UNF_INCLUDE)                    \
                -I$(MSP_API_INCLUDE)                    \
                -I$(MSP_DRV_INCLUDE)                    \
                -I$(MSP_DIR)/api/demux                  \
                -I$(MSP_DIR)/drv/demux                  \
                -I$(MSP_DIR)/drv/demux/descrambler      \
                -I$(COMPONENT_DIR)/ha_codec/include

EXTRA_CFLAGS += -DCFG_HI_DEMUX_POOLBUF_SIZE=$(CFG_HI_DEMUX_POOLBUF_SIZE)

EXTRA_CFLAGS += -DHI_DEMUX_PROC_SUPPORT

EXTRA_CFLAGS += -DDMX_DESCRAMBLER_SUPPORT

KBUILD_EXTRA_SYMBOLS += $(COMMON_DIR)/drv/Module.symvers

MOD_NAME := hi_demux

obj-$(CFG_MSP_BUILDTYPE) += $(MOD_NAME).o

$(MOD_NAME)-y   :=  drv_demux_intf.o        \
                    drv_demux.o             \
                    drv_demux_func.o        \
                    hal_demux.o             \
                    drv_demux_osal.o        \
                    drv_demux_scd.o         \
                    drv_demux_bitstream.o   \
                    drv_demux_index.o       \
                    drv_demux_sw.o

$(MOD_NAME)-y   +=  descrambler/drv_descrambler.o       \
                    descrambler/drv_descrambler_func.o  \
                    descrambler/hal_descrambler.o

#===============================================================================
#   rules
#===============================================================================
.PHONY: all clean modules_install

all:
	$(AT)make -C $(LINUX_DIR) M=$(CURDIR) modules

clean:
	$(AT)make -C $(LINUX_DIR) M=$(CURDIR) clean
	$(AT)rm -rf  *.o
	$(AT)rm -rf  .*.o.cmd
	$(AT)rm -rf .tmp_versions
	$(AT)rm -rf  Module.symvers
	$(AT)rm -rf  modules.order
	$(AT)rm -rf  descrambler/*.o
	$(AT)rm -rf  descrambler/.*.o.cmd

modules_install:
	$(AT)make -C $(LINUX_DIR) M=$(CURDIR) modules_install

