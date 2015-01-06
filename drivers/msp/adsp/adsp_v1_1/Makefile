#===============================================================================
# export variables
#===============================================================================
ifeq ($(CFG_HI_EXPORT_FLAG),)
    ifneq ($(KERNELRELEASE),)
    KERNEL_DIR := $(srctree)

    SDK_DIR := $(KERNEL_DIR)/../../..
    else
    SDK_DIR := $(CURDIR)/../../../../..
    endif

include $(SDK_DIR)/base.mak
endif

#===============================================================================
# local variables
#===============================================================================
ADSP_DRV_INCLUDE =$(SDK_DIR)/source/msp/drv/adsp/adsp_v1_1/include 

EXTRA_CFLAGS += $(CFG_HI_KMOD_CFLAGS)

EXTRA_CFLAGS += -I$(COMMON_UNF_INCLUDE)                 \
                -I$(COMMON_API_INCLUDE)                 \
                -I$(COMMON_DRV_INCLUDE)                 \
                -I$(MSP_UNF_INCLUDE)                    \
                -I$(MSP_API_INCLUDE)                    \
                -I$(MSP_DRV_INCLUDE)        \
                -I$(ADSP_DRV_INCLUDE)                    

#SIMULATE or DSP HW
#CFG_HI_SND_AOE_HW_SUPPORT = y	  do not enable ,include in base.mak

ifneq ($(CFG_HI_SND_AOE_HW_SUPPORT),y)
#simulate support
HIFIDIR := ./hifi
HIFIDIR2 := $(SDK_DIR)/source/msp/drv/adsp/adsp_v1_1/hifi

EXTRA_CFLAGS +=  \
		-I$(HIFIDIR2)/common/include \
		-I$(HIFIDIR2)/aoe/include \
		-I$(HIFIDIR2)/aoe/src/include 

EXTRA_CFLAGS += -DHI_SND_AOE_SWSIMULATE_SUPPORT
else
EXTRA_CFLAGS += -DHI_SND_DSP_HW_SUPPORT
endif

KBUILD_EXTRA_SYMBOLS += $(COMMON_DIR)/drv/Module.symvers

MOD_NAME := hi_adsp

obj-$(CFG_MSP_BUILDTYPE) += $(MOD_NAME).o

$(MOD_NAME)-y   := drv_adsp_intf.o \
		   drv_adsp.o 
		   

ifneq ($(CFG_HI_SND_AOE_HW_SUPPORT),y)
#simulate support
$(MOD_NAME)-y += $(HIFIDIR)/common/linear/linear.o
$(MOD_NAME)-y += $(HIFIDIR)/common/mem/dsp_mem.o
$(MOD_NAME)-y += $(HIFIDIR)/common/resampler/resample_rom.o
$(MOD_NAME)-y += $(HIFIDIR)/common/resampler/resample.o
$(MOD_NAME)-y += $(HIFIDIR)/common/util/dsp_util.o
$(MOD_NAME)-y += $(HIFIDIR)/common/ringbuf/dsp_ringbuf.o
$(MOD_NAME)-y += $(HIFIDIR)/common/basicop/basicop.o

$(MOD_NAME)-y += $(HIFIDIR)/sw_main.o
$(MOD_NAME)-y += $(HIFIDIR)/aoe/src/aop/aoe_aop.o
$(MOD_NAME)-y += $(HIFIDIR)/aoe/src/aip/aoe_aip.o
$(MOD_NAME)-y += $(HIFIDIR)/aoe/src/engine/aoe_engine.o
$(MOD_NAME)-y += $(HIFIDIR)/aoe/src/aoe_main.o
$(MOD_NAME)-y += $(HIFIDIR)/aoe/src/aoe_reg.o
else
$(MOD_NAME)-y += kfile_ops_func.o dsp_elf_func.o
endif
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

modules_install:
	$(AT)make -C $(LINUX_DIR) M=$(CURDIR) modules_install

