
ccflags-y += -DSDK_VERSION="HiSTBAndroidV600R001C00SPC020_v2014072118"
ccflags-y += -DCHIP_TYPE_hi3798cv100

ccflags-y += -DCFG_HI_HD0_FB_VRAM_SIZE=16200
ccflags-y += -DCFG_HIFB_STEREO3D_HW_SUPPORT
ccflags-y += -DCFG_HIFB_FENCE_SUPPORT
ccflags-y += -DHI_PROC_SUPPORT=1

ccflags-y += -Idrivers/staging/android
ccflags-y += -Idrivers/msp/include
ccflags-y += -Idrivers/msp/hifb/adp/platform/alg

obj-y += adp/platform/hi3798cv100/optm_hal.o
obj-y += adp/platform/cv200/optm_hifb.o
obj-y += adp/hifb_blit.o
obj-y += adp/hifb_drv_adpt.o
obj-y += hifb_comm.o
obj-y += hifb_fence.o
obj-y += hifb_main.o
