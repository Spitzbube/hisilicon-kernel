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

EXTRA_CFLAGS += -I$(COMMON_UNF_INCLUDE)                                         \
                -I$(COMMON_API_INCLUDE)                                         \
                -I$(COMMON_DRV_INCLUDE)                                         \
                -I$(MSP_UNF_INCLUDE)                                            \
                -I$(MSP_API_INCLUDE)                                            \
                -I$(MSP_DRV_INCLUDE)                                            \
                -I$(MSP_DIR)/drv/frontend/include                               \
                -I$(MSP_DIR)/drv/frontend/demod/AVL_DVBSx_SDK/AVL_DVBSx/include \
                -I$(MSP_DIR)/drv/frontend/demod/AVL_DVBSx_SDK/BSP

EXTRA_CFLAGS += -I$(SOURCE_DIR)/common/include/
EXTRA_CFLAGS += -I$(SOURCE_DIR)/common/drv/include/
EXTRA_CFLAGS += -I$(SOURCE_DIR)/common/api/stat/

KBUILD_EXTRA_SYMBOLS += $(COMMON_DIR)/drv/Module.symvers

#################### base #####################
MOD_NAME := hi_tuner

obj-$(CFG_MSP_BUILDTYPE) += $(MOD_NAME).o

 
$(MOD_NAME)-y := drv_tuner.o
$(MOD_NAME)-y += tuner_intf.o

ifeq ($(CFG_HI_DEMOD_TYPE_HI3130),y)
hi_tuner-y +=	demod/hi3130/hi3130.o
hi_tuner-y +=	demod/hi3130v200/dvb_c/hi3130v200.o
EXTRA_CFLAGS += -DDEMOD_DEV_TYPE_HI3130
endif

ifeq ($(CFG_HI_DEMOD_TYPE_HI3136),y)
hi_tuner-y +=	demod/hi3136/hi3136.o
EXTRA_CFLAGS += -DDEMOD_DEV_TYPE_HI3136
endif

ifeq ($(CFG_HI_DEMOD_TYPE_X5HDQAM),y)
hi_tuner-y +=	demod/x5hdqam/x5hdqam.o
EXTRA_CFLAGS += -DDEMOD_DEV_TYPE_X5HDQAM
endif

ifeq ($(CFG_HI_DEMOD_TYPE_X5HDV2QAM),y)
hi_tuner-y +=	demod/x5hdqamv200/x5hdqamv200.o
hi_tuner-y +=	demod/hi3716cv200/dvb_c/hi3716cv200.o
EXTRA_CFLAGS += -DDEMOD_DEV_TYPE_X5HDV2QAM
endif

ifeq ($(CFG_HI_DEMOD_TYPE_J83B),y)
hi_tuner-y +=	demod/j83b/j83b.o
hi_tuner-y +=	demod/hi3716cv200/j83b/hi3716cv200_j83b.o
EXTRA_CFLAGS += -DDEMOD_DEV_TYPE_J83B
	ifeq ($(CFG_HI_DEMOD_TYPE_HI3130),y)
	hi_tuner-y +=	demod/hi3130v200/j83b/hi3130v200_j83b.o
	EXTRA_CFLAGS += -DDEMOD_DEV_TYPE_HI3130
	endif
endif

ifeq ($(CFG_HI_DEMOD_TYPE_AVL6211),y)
hi_tuner-y += demod/AVL_DVBSx_SDK/AVL_DVBSx/src/avl.o \
                demod/AVL_DVBSx_SDK/AVL_DVBSx/src/avl_dvbsx.o \
                demod/AVL_DVBSx_SDK/AVL_DVBSx/src/IBase.o \
                demod/AVL_DVBSx_SDK/AVL_DVBSx/src/IBlindScan.o \
                demod/AVL_DVBSx_SDK/AVL_DVBSx/src/IBlindScanProcess.o \
                demod/AVL_DVBSx_SDK/AVL_DVBSx/src/IDiseqc.o \
                demod/AVL_DVBSx_SDK/AVL_DVBSx/src/II2C.o \
                demod/AVL_DVBSx_SDK/AVL_DVBSx/src/II2CRepeater.o \
                demod/AVL_DVBSx_SDK/AVL_DVBSx/src/IRx.o \
                demod/AVL_DVBSx_SDK/AVL_DVBSx/src/ITuner.o \
                demod/AVL_DVBSx_SDK/BSP/IBSP.o \
                demod/AVL_DVBSx_SDK/AVL6211/AVL6211.o \
                demod/AVL_DVBSx_SDK/AVL6211/AVL6211_patch.dat.o
EXTRA_CFLAGS += -DDEMOD_DEV_TYPE_AVL6211
endif              

ifeq ($(CFG_HI_DEMOD_TYPE_MXL101),y)            
hi_tuner-y += demod/MXL101/mxl101.o \
                demod/MXL101/MxL101SF_OEM_Drv.o \
                demod/MXL101/MxL101SF_PhyCfg.o \
                demod/MXL101/MxL101SF_PhyCtrlApi.o
EXTRA_CFLAGS += -DDEMOD_DEV_TYPE_MXL101
endif                
     
ifeq ($(CFG_HI_DEMOD_TYPE_MN88472),y)        
hi_tuner-y += demod/mn88472/MN88472_autoctrl.o \
                demod/mn88472/MN88472_register.o \
                demod/mn88472/MN_DMD_device.o \
                demod/mn88472/MN_DMD_driver.o \
                demod/mn88472/MN_I2C.o \
                demod/mn88472/MN_Tuner.o \
                demod/mn88472/MN88472.o \
                demod/mn88472/MN_DMD_common.o \
                demod/mn88472/MN88472_UserFunction.o
EXTRA_CFLAGS += -DDEMOD_DEV_TYPE_MN88472
endif
               
ifeq ($(CFG_HI_DEMOD_TYPE_IT9170),y)
hi_tuner-y += demod/IT9170/cmd.o \
                demod/IT9170/IT9170.o \
                demod/IT9170/IT9173.o \
                demod/IT9170/samba.o \
                demod/IT9170/standard.o \
                demod/IT9170/tuner.o \
                demod/IT9170/user.o                              
EXTRA_CFLAGS += -DDEMOD_DEV_TYPE_IT9170
endif                
                
ifeq ($(CFG_HI_DEMOD_TYPE_IT9133),y)                
hi_tuner-y += demod/IT9133/cmd.o \
                demod/IT9133/IT9133.o \
                demod/IT9133/Omega.o \
                demod/IT9133/standard.o \
                demod/IT9133/tuner.o \
                demod/IT9133/user.o \
                demod/IT9133/ite9133.o    
EXTRA_CFLAGS += -DDEMOD_DEV_TYPE_IT9133                              
endif                
                
DRIV_DIR=$(KECS_DIR)/drv/tuner_pilot
                
EXTRA_CFLAGS += $(KCFG_CFLAGS) 
EXTRA_CFLAGS += -I$(DRV_DIR)/include  -I$(MSP_DIR)/include -I$(FE_DIR)/include
EXTRA_CFLAGS += -I$(FE_DIR)/demod/AVL_DVBSx_SDK/AVL_DVBSx/include \
                -I$(FE_DIR)/demod/AVL_DVBSx_SDK/BSP 
EXTRA_CFLAGS += -I$(KECS_DIR)/drv/gpio/


############## tuner type config #############
ifeq ($(CFG_HI_TUNER_TYPE_XG_3BL),y)
hi_tuner-y += tuner/xg_3bl.o
EXTRA_CFLAGS += -DTUNER_DEV_TYPE_XG_3BL
endif

ifeq ($(CFG_HI_TUNER_TYPE_ALPS_TDAE),y)
hi_tuner-y += tuner/alps_tdae.o
EXTRA_CFLAGS += -DTUNER_DEV_TYPE_ALPS_TDAE
endif

ifeq ($(CFG_HI_TUNER_TYPE_CD1616_LF_GIH_4),y)
hi_tuner-y += tuner/cd1616_double.o
EXTRA_CFLAGS += -DTUNER_DEV_TYPE_CD1616_LF_GIH_4
endif

ifeq ($(CFG_HI_TUNER_TYPE_CD1616),y)
hi_tuner-y += tuner/cd1616.o
EXTRA_CFLAGS += -DTUNER_DEV_TYPE_CD1616
endif

ifeq ($(CFG_HI_TUNER_TYPE_TDA18250),y)
hi_tuner-y += tuner/tda18250/tmbslTDA18250.o \
			  tuner/tda18250/tmbslTDA18250Instance.o \
			  tuner/tda18250/tmddTDA18250.o \
			  tuner/tda18250/tmddTDA18250Instance.o \
			  tuner/tda18250/tda18250.o
EXTRA_CFLAGS += -DTUNER_DEV_TYPE_TDA18250
endif			  

ifeq ($(CFG_HI_TUNER_TYPE_TDA18250B),y)
hi_tuner-y += tuner/tda18250b/tmbslTDA18250A.o \
			  tuner/tda18250b/tmbslTDA18250A_Advanced.o \
			  tuner/tda18250b/tda18250b.o
EXTRA_CFLAGS += -DTUNER_DEV_TYPE_TDA18250B
endif			  

ifeq ($(CFG_HI_TUNER_TYPE_MXL203),y)			  
hi_tuner-y += tuner/mxl203/mxl203.o \
			  tuner/mxl203/MxL203RF.o \
			  tuner/mxl203/MxL_User_Define.o \
			  tuner/mxl203/MxL203RF_API.o
EXTRA_CFLAGS += -DTUNER_DEV_TYPE_MXL203
endif

ifeq ($(CFG_HI_TUNER_TYPE_TMX7070X),y)
hi_tuner-y += tuner/tmx7070x.o 
EXTRA_CFLAGS += -DTUNER_DEV_TYPE_TMX7070X
endif

ifeq ($(CFG_HI_TUNER_TYPE_MT2081),y)
hi_tuner-y += tuner/mt2081/mt2081.o \
			  tuner/mt2081/mt2081_userdef.o
EXTRA_CFLAGS += -DTUNER_DEV_TYPE_MT2081
endif
			  
ifeq ($(CFG_HI_TUNER_TYPE_TDCC),y)			  
hi_tuner-y += tuner/tdcc.o
EXTRA_CFLAGS += -DTUNER_DEV_TYPE_TDCC
endif

ifeq ($(CFG_HI_TUNER_TYPE_R820C),y)	      
hi_tuner-y += tuner/R820C/I2C_Sys.o \
		          tuner/R820C/R820C.o
EXTRA_CFLAGS += -DTUNER_DEV_TYPE_R820C
endif

ifeq ($(CFG_HI_TUNER_TYPE_AV2011),y)              
hi_tuner-y += tuner/av2011.o
EXTRA_CFLAGS += -DTUNER_DEV_TYPE_AV2011
endif

ifeq ($(CFG_HI_TUNER_TYPE_SHARP7903),y)
hi_tuner-y += tuner/sharp_qm1d1c004x.o
EXTRA_CFLAGS += -DTUNER_DEV_TYPE_SHARP7903
endif

ifeq ($(CFG_HI_TUNER_TYPE_RDA5815),y)
hi_tuner-y += tuner/RDA5815.o
EXTRA_CFLAGS += -DTUNER_DEV_TYPE_RDA5815
endif

ifeq ($(CFG_HI_TUNER_TYPE_M88TS2022),y)
hi_tuner-y += tuner/M88TS2022.o
EXTRA_CFLAGS += -DTUNER_DEV_TYPE_M88TS2022
endif

ifeq ($(CFG_HI_TUNER_TYPE_MXL603),y)
hi_tuner-y += tuner/MXL603/MxL603_OEM_Drv.o \
			        tuner/MXL603/MxL603_TunerApi.o \
			        tuner/MXL603/MxL603_TunerCfg.o \
			        tuner/MXL603/MxL603.o
EXTRA_CFLAGS += -DTUNER_DEV_TYPE_MXL603
endif

ifeq ($(CFG_HI_TUNER_TYPE_TDA6651),y)			        
hi_tuner-y += tuner/tda6651.o             
EXTRA_CFLAGS += -DTUNER_DEV_TYPE_TDA6651
endif

hi_tuner-y += lnbctrl/lnbctrl_common.o

ifeq ($(CFG_HI_LNB_CTRL_ISL9492),y)
hi_tuner-y += lnbctrl/isl9492.o 
EXTRA_CFLAGS += -DLNB_CTRL_ISL9492
endif

ifeq ($(CFG_HI_LNB_CTRL_MPS8125),y) 
hi_tuner-y += lnbctrl/mps8125.o
EXTRA_CFLAGS += -DLNB_CTRL_MPS8125

ifeq ($(CFG_HI_LNB_CTRL_GPIO),y)
EXTRA_CFLAGS += -DLNB_CTRL_GPIO

ifneq (${CFG_HI_LNB_EN_GPIO_NO},)
EXTRA_CFLAGS += -DHI_LNB_EN_GPIO_NO=${CFG_HI_LNB_EN_GPIO_NO}
endif

ifneq (${CFG_HI_LNB_HV_SEL_GPIO_NO},)
EXTRA_CFLAGS += -DHI_LNB_HV_SEL_GPIO_NO=${CFG_HI_LNB_HV_SEL_GPIO_NO}
endif
endif

ifeq ($(CFG_HI_LNB_CTRL_DEMOD),y)
EXTRA_CFLAGS += -DLNB_CTRL_DEMOD
endif
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
