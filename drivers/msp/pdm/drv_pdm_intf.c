#include <linux/kernel.h>
#include <mach/hardware.h>
#include <asm/io.h>

#include "hi_module.h"
#include "hi_drv_module.h"
#include "hi_drv_dev.h"
#include "hi_drv_proc.h"
#include "hi_kernel_adapt.h"

#include "hi_drv_pdm.h"
#include "drv_pdm_ext.h"
#include "drv_pdm.h"
#include "hi_osal.h"


#define PDM_NAME                "HI_PDM"


DECLARE_MUTEX(g_PdmMutex);

#define DRV_PDM_Lock()      \
    do{         \
        if(down_interruptible(&g_PdmMutex))   \
        {       \
            HI_ERR_PDM("ERR: PDM intf lock error!\n");  \
        }       \
      }while(0)

#define DRV_PDM_UnLock()      \
    do{         \
        up(&g_PdmMutex);    \
      }while(0)


typedef struct tagPDM_REGISTER_PARAM_S{
    DRV_PROC_READ_FN        rdproc;
    DRV_PROC_WRITE_FN       wtproc;
}PDM_REGISTER_PARAM_S;


PDM_EXPORT_FUNC_S   g_PdmExportFuncs = 
{
    .pfnPDM_GetDispParam        = DRV_PDM_GetDispParam,
    .pfnPDM_GetMceParam         = DRV_PDM_GetMceParam,
    .pfnPDM_GetMceData          = DRV_PDM_GetMceData,
    .pfnPDM_ReleaseReserveMem   = DRV_PDM_ReleaseReserveMem,
    .pfnPDM_GetData				= DRV_PDM_GetData,
    .pfnPDM_GetSoundParam       = DRV_PDM_GetSoundParam
};


PDM_GLOBAL_S        g_PdmGlobal;

/*the function to get pdm tag data*/
extern int get_param_data(const char *name, char *buf, unsigned int buflen);


static void print_custom_timing(struct seq_file *p, HI_DISP_PARAM_S *pstDispParam)
{
	HI_CHAR *DataWidth[] =
	{
		"WIDTH8",
		"WIDTH16",
		"WIDTH24",
    };

	PROC_PRINT(p, " ############ Custom Timing begin ##############\n");
	PROC_PRINT(p, "VFB:      \011\011%d   \n", pstDispParam->stDispTiming.VFB);
	PROC_PRINT(p, "VBB:      \011\011%d   \n", pstDispParam->stDispTiming.VBB);
	PROC_PRINT(p, "VACT:      \011\011%d   \n", pstDispParam->stDispTiming.VACT);
	PROC_PRINT(p, "HFB:      \011\011%d   \n", pstDispParam->stDispTiming.HFB);
	PROC_PRINT(p, "HBB:      \011\011%d   \n", pstDispParam->stDispTiming.HBB);
	PROC_PRINT(p, "HACT:      \011\011%d   \n", pstDispParam->stDispTiming.HACT);
	PROC_PRINT(p, "VPW:      \011\011%d   \n", pstDispParam->stDispTiming.VPW);
	PROC_PRINT(p, "HPW:      \011\011%d   \n", pstDispParam->stDispTiming.HPW);

	if (pstDispParam->stDispTiming.IDV)
	{
		PROC_PRINT(p, "IDV:      \011\011%s   \n", "true");
	}
	else
	{
		PROC_PRINT(p, "IDV:      \011\011%s   \n", "false");
	}

	if (pstDispParam->stDispTiming.IHS)
	{
		PROC_PRINT(p, "IHS:      \011\011%s   \n", "true");
	}
	else
	{
		PROC_PRINT(p, "IHS:      \011\011%s   \n", "false");
	}

	if (pstDispParam->stDispTiming.IVS)
	{
		PROC_PRINT(p, "IVS:      \011\011%s   \n", "true");
	}
	else
	{
		PROC_PRINT(p, "IVS:      \011\011%s   \n", "false");
	}

	if (pstDispParam->stDispTiming.ClockReversal)
	{
		PROC_PRINT(p, "ClockReversal:      \011%s   \n", "true");
	}
	else
	{
		PROC_PRINT(p, "ClockReversal:      \011%s   \n", "false");
	}

	if (pstDispParam->stDispTiming.DataWidth < 3)
	{
		PROC_PRINT(p, "DataWidth:      \011%s   \n", DataWidth[pstDispParam->stDispTiming.DataWidth]);
	}
	else
	{
		PROC_PRINT(p, "DataWidth:      \011%d   \n", pstDispParam->stDispTiming.DataWidth);
	}

	PROC_PRINT(p, "ItfFormat:      \011%d   \n", pstDispParam->stDispTiming.ItfFormat);

	if (pstDispParam->stDispTiming.DitherEnable)
	{
		PROC_PRINT(p, "DitherEnable:      \011%s   \n", "true");
	}
	else
	{
		PROC_PRINT(p, "DitherEnable:      \011%s   \n", "false");
	}

	PROC_PRINT(p, "ClkPara0:      \011\0110x%x   \n", pstDispParam->stDispTiming.ClkPara0);
	PROC_PRINT(p, "ClkPara1:      \011\0110x%x   \n", pstDispParam->stDispTiming.ClkPara1);

	if (pstDispParam->stDispTiming.bInterlace)
	{
		PROC_PRINT(p, "bInterlace:      \011%s   \n", "true");
	}
	else
	{
		PROC_PRINT(p, "bInterlace:      \011%s   \n", "false");
	}

	PROC_PRINT(p, "PixFreq:      \011\011%d   \n", pstDispParam->stDispTiming.PixFreq);
	PROC_PRINT(p, "VertFreq:      \011\011%d   \n", pstDispParam->stDispTiming.VertFreq);
	PROC_PRINT(p, "AspectRatioW:      \011%d   \n", pstDispParam->stDispTiming.AspectRatioW);
	PROC_PRINT(p, "AspectRatioH:      \011%d   \n", pstDispParam->stDispTiming.AspectRatioH);

	if (pstDispParam->stDispTiming.bUseGamma)
	{
		PROC_PRINT(p, "UseGamma:      \011\011%s   \n", "true");
	}
	else
	{
		PROC_PRINT(p, "UseGamma:      \011\011%s   \n", "false");
	}

	PROC_PRINT(p, "Reserve0:      \011\011%d   \n", pstDispParam->stDispTiming.Reserve0);
	PROC_PRINT(p, "Reserve1:      \011\011%d   \n", pstDispParam->stDispTiming.Reserve1);

	PROC_PRINT(p, " ############ Custom Timing end ##############\n");
}



static HI_S32 PDM_ProcRead(struct seq_file *p, HI_VOID *v)
{
	HI_DISP_PARAM_S stDispParam;
	HI_SOUND_PARAM_S stSoundParam;
	HI_U32 u32Data;
	HI_S32 s32Ret = HI_SUCCESS;
	HI_U8  ii = 0;
	HI_U32 i = 0;
//	HI_UNF_DISP_E i;
//	HI_UNF_SND_E j;
	HI_CHAR *Aspect[HI_UNF_DISP_ASPECT_RATIO_BUTT] = 
	{
		"auto",
		"4to3",
		"16to9",
		"221to1",
		"user"
	};	
	HI_CHAR *MasterClock[] =
	{
		"128*fs",
		"256*fs",
		"384*fs",
		"512*fs",
		"768*fs",
		"1024*fs",
	};

	for (i = HI_UNF_DISPLAY0; i < HI_UNF_DISPLAY2/*HI_UNF_DISPLAY_BUTT*/; i++)
	{
		memset(&stDispParam, 0, sizeof(stDispParam));
		s32Ret = DRV_PDM_GetDispParam(i, &stDispParam);
		if (HI_SUCCESS == s32Ret)
		{
			PROC_PRINT(p,"---------------------------Dispaly%d------------------------------\n", i);

			if (stDispParam.enFormat == 260)
			{
				PROC_PRINT(p, "format:      \011\011%s   \n", "Custom timing");
				print_custom_timing(p, &stDispParam);
			}
			else
			{
				PROC_PRINT(p, "format:      \011\011%d   \n", stDispParam.enFormat);
			}

			PROC_PRINT(p,"source display:        \011%d\n", stDispParam.enSrcDisp);
			PROC_PRINT(p,"background color:      	0x%02x%02x%02x\n",
	                       stDispParam.stBgColor.u8Red, stDispParam.stBgColor.u8Green, stDispParam.stBgColor.u8Blue);
			PROC_PRINT(p,"HuePlus/Brightness/Contrast/Saturation: %d/%d/%d/%d\n",
	                       stDispParam.u32HuePlus, stDispParam.u32Brightness,
	                       stDispParam.u32Contrast, stDispParam.u32Saturation);
			PROC_PRINT(p,"virtual screen(Width/Height):      	%d/%d\n",
	                       stDispParam.u32VirtScreenWidth, stDispParam.u32VirtScreenHeight);
			PROC_PRINT(p,"offset(Left/Top/Right/Bottom):      	%d/%d/%d/%d\n",
	                       stDispParam.stOffsetInfo.u32Left, stDispParam.stOffsetInfo.u32Top,
	                       stDispParam.stOffsetInfo.u32Right, stDispParam.stOffsetInfo.u32Bottom);
			PROC_PRINT(p,"bGammaEnable:      	%d\n",   stDispParam.bGammaEnable);
			PROC_PRINT(p,"pixelformat:      	%d\n",   stDispParam.enPixelFormat);
			if ((stDispParam.stAspectRatio.enDispAspectRatio != HI_UNF_DISP_ASPECT_RATIO_USER)
				&& (stDispParam.stAspectRatio.enDispAspectRatio != HI_UNF_DISP_ASPECT_RATIO_BUTT))
			{
				PROC_PRINT(p,"aspectRatio:      	%s\n",   Aspect[stDispParam.stAspectRatio.enDispAspectRatio]);
			}
			else if (stDispParam.stAspectRatio.enDispAspectRatio == HI_UNF_DISP_ASPECT_RATIO_USER)
			{
				PROC_PRINT(p,"aspectRatio:      	%s(%dto%d)\n",   Aspect[stDispParam.stAspectRatio.enDispAspectRatio],
								stDispParam.stAspectRatio.u32UserAspectWidth, stDispParam.stAspectRatio.u32UserAspectHeight);
			}
			for (ii = HI_UNF_DISP_INTF_TYPE_HDMI; ii < HI_UNF_DISP_INTF_TYPE_BUTT; ii++)
			{
				if (stDispParam.stIntf[ii].enIntfType ==  HI_UNF_DISP_INTF_TYPE_HDMI)
				{
					PROC_PRINT(p,"HDMI:      		HDMI_%d\n",   stDispParam.stIntf[ii].unIntf.enHdmi);
				}
				else if (stDispParam.stIntf[ii].enIntfType ==  HI_UNF_DISP_INTF_TYPE_LCD)
				{
					PROC_PRINT(p,"LCD:      		LCD_%d\n",   stDispParam.stIntf[ii].unIntf.enLcd);
				}
				else if (stDispParam.stIntf[ii].enIntfType ==  HI_UNF_DISP_INTF_TYPE_BT1120)
				{
					PROC_PRINT(p,"BT1120:      	BT1120_%d\n",   stDispParam.stIntf[ii].unIntf.enHdmi);
				}
				else if (stDispParam.stIntf[ii].enIntfType ==  HI_UNF_DISP_INTF_TYPE_BT656)
				{
					PROC_PRINT(p,"BT656:      		BT656_%d\n",   stDispParam.stIntf[ii].unIntf.enHdmi);
				}
				else if (stDispParam.stIntf[ii].enIntfType ==  HI_UNF_DISP_INTF_TYPE_YPBPR)
				{
					PROC_PRINT(p,"YPbPr(Y/Pb/Pr):      	%d/%d/%d\n",   stDispParam.stIntf[ii].unIntf.stYPbPr.u8DacY,
									stDispParam.stIntf[ii].unIntf.stYPbPr.u8DacPb, stDispParam.stIntf[ii].unIntf.stYPbPr.u8DacPr);
				}
				else if (stDispParam.stIntf[ii].enIntfType ==  HI_UNF_DISP_INTF_TYPE_RGB)
				{
					PROC_PRINT(p,"RGB(R/G/B):      			%d/%d/%d\n",   stDispParam.stIntf[ii].unIntf.stRGB.u8DacR,
									stDispParam.stIntf[ii].unIntf.stRGB.u8DacG, stDispParam.stIntf[ii].unIntf.stRGB.u8DacB);
				}
				else if (stDispParam.stIntf[ii].enIntfType ==  HI_UNF_DISP_INTF_TYPE_CVBS)
				{
					PROC_PRINT(p,"CVBS:      		%d\n",   stDispParam.stIntf[ii].unIntf.stCVBS.u8Dac);
				}
				else if (stDispParam.stIntf[ii].enIntfType ==  HI_UNF_DISP_INTF_TYPE_SVIDEO)
				{
					PROC_PRINT(p,"SVIDEO(Y/C):      		%d/%d\n",   stDispParam.stIntf[ii].unIntf.stSVideo.u8DacY,
									stDispParam.stIntf[ii].unIntf.stSVideo.u8DacC);
				}
				else if (stDispParam.stIntf[ii].enIntfType == HI_UNF_DISP_INTF_TYPE_VGA)
				{
					PROC_PRINT(p, "VGA(R/G/B):      \011%d/%d/%d\n",
							stDispParam.stIntf[ii].unIntf.stRGB.u8DacR,
							stDispParam.stIntf[ii].unIntf.stRGB.u8DacG,
							stDispParam.stIntf[ii].unIntf.stRGB.u8DacB);
				}
			}
		}
	}

	for (i = HI_UNF_SND_0; i != HI_UNF_SND_BUTT; i++)
	{
		u32Data = 0;
		memset(&stSoundParam, 0, sizeof(stSoundParam));
		s32Ret = HI_DRV_PDM_GetSoundParamEx(i, &stSoundParam, &u32Data);
		if (HI_SUCCESS == s32Ret)
		{
			if (stSoundParam.u32PortNum < 17)
			{
				PROC_PRINT(p, "---------------------------Sound%d------------------------------\n", i);

				for (ii = 0; ii < stSoundParam.u32PortNum; ii++)
				{
					if (stSoundParam.stOutport[ii].enOutPort == HI_UNF_SND_OUTPUTPORT_DAC0)
					{
						if (u32Data != 0)
						{
							PROC_PRINT(p, "DAC:      \011\011\011%s       VOL:%3d\n", "DAC0", stSoundParam.Data_580[ii]);
						}
						else
						{
							PROC_PRINT(p, "DAC:      \011\011\011%s\n", "DAC0");
						}
					}

					if (stSoundParam.stOutport[ii].enOutPort == HI_UNF_SND_OUTPUTPORT_SPDIF0)
					{
						if (u32Data != 0)
						{
							PROC_PRINT(p, "SPDIF:      \011\011\011%s     VOL:%3d\n", "SPDIF0", stSoundParam.Data_580[ii]);
						}
						else
						{
							PROC_PRINT(p, "SPDIF:      \011\011\011%s\n", "SPDIF0");
						}
					}

					if (stSoundParam.stOutport[ii].enOutPort == HI_UNF_SND_OUTPUTPORT_HDMI0)
					{
						if (u32Data != 0)
						{
							PROC_PRINT(p, "HDMI:     \011\011\011%s      VOL:%3d\n", "HDMI0", stSoundParam.Data_580[ii]);
						}
						else
						{
							PROC_PRINT(p, "HDMI:      \011\011\011%s\n", "HDMI0");
						}
					}

					if (stSoundParam.stOutport[ii].enOutPort == HI_UNF_SND_OUTPUTPORT_ARC0)
					{
						if (u32Data != 0)
						{
							PROC_PRINT(p, "ARC:      \011\011\011%s       VOL:%3d\n", "ARC0", stSoundParam.Data_580[ii]);
						}
						else
						{
							PROC_PRINT(p, "ARC:      \011\011\011%s\n", "ARC0");
						}
					}

					if ((stSoundParam.stOutport[ii].enOutPort == HI_UNF_SND_OUTPUTPORT_I2S0) ||
							(stSoundParam.stOutport[ii].enOutPort == HI_UNF_SND_OUTPUTPORT_I2S1))
					{
						PROC_PRINT(p, "I2S%d attr:      \011\011\n",
								stSoundParam.stOutport[ii].enOutPort - HI_UNF_SND_OUTPUTPORT_I2S0);

						if (u32Data != 0)
						{
							PROC_PRINT(p, "\011VOL :                   %3d\n", stSoundParam.Data_580[ii]);
						}

						if (stSoundParam.stOutport[ii].unAttr.stI2sAttr.stAttr.bMaster)
						{
							PROC_PRINT(p, "\011Master:      \011\011TRUE\n");
						}
						else
						{
							PROC_PRINT(p, "\011Master:      \011\011FALSE\n");
						}

						if (stSoundParam.stOutport[ii].unAttr.stI2sAttr.stAttr.bPcmSampleRiseEdge)
						{
							PROC_PRINT(p, "\011PcmSampleRiseEdge:      TRUE\n");
						}
						else
						{
							PROC_PRINT(p, "\011PcmSampleRiseEdge:      FALSE\n");
						}

						PROC_PRINT(p, "\011Master colock:      \011%s\n", MasterClock[stSoundParam.stOutport[ii].unAttr.stI2sAttr.stAttr.enMclkSel]);
						PROC_PRINT(p, "\011Bit colock:      \011%d DIV\n", stSoundParam.stOutport[ii].unAttr.stI2sAttr.stAttr.enBclkSel);
						PROC_PRINT(p, "\011Channel:      \011\011%d\n", stSoundParam.stOutport[ii].unAttr.stI2sAttr.stAttr.enChannel);

						if (stSoundParam.stOutport[ii].unAttr.stI2sAttr.stAttr.enI2sMode == HI_UNF_I2S_STD_MODE)
						{
							PROC_PRINT(p, "\011Mode:      \011\011standard\n");
						}
						else
						{
							PROC_PRINT(p, "\011Mode:      \011\011pcm\n");
						}

						PROC_PRINT(p, "\011Bit Depth:      \011%d\n", stSoundParam.stOutport[ii].unAttr.stI2sAttr.stAttr.enBitDepth);
						PROC_PRINT(p, "\011PCM Delay Cycle:      \011%d\n", stSoundParam.stOutport[ii].unAttr.stI2sAttr.stAttr.enPcmDelayCycle);
					}
				} //for (ii = 0; ii < stSoundParam.Data_0; ii++)
			} //if (stSoundParam.Data_0 < 16)
		}
	} //for (i = HI_UNF_SND_0; i < HI_UNF_SND_BUTT; i++)

    return HI_SUCCESS;
}

static HI_S32 PDM_ProcWrite(struct file * file,
    const char __user * buf, size_t count, loff_t *ppos)
{
    return count;
}

HI_S32 PDM_DRV_Open(struct inode *finode, struct file  *ffile)
{
    return HI_SUCCESS;
}

HI_S32 PDM_DRV_Close(struct inode *finode, struct file  *ffile)
{
    return HI_SUCCESS;
}

HI_S32 PDM_Ioctl(struct inode *inode, struct file *file, unsigned int cmd, HI_VOID *arg)
{
    HI_S32          Ret = HI_SUCCESS;

    DRV_PDM_Lock(); //420


    DRV_PDM_UnLock();
    
    return Ret;
}

static long PDM_DRV_Ioctl(struct file *ffile, unsigned int cmd, unsigned long arg)
{
    HI_S32 Ret;

    Ret = HI_DRV_UserCopy(ffile->f_dentry->d_inode, ffile, cmd, arg, PDM_Ioctl);

    return Ret;
}

HI_S32 PDM_Suspend(PM_BASEDEV_S *pdev, pm_message_t state)
{
    return HI_SUCCESS;
}

HI_S32 PDM_Resume(PM_BASEDEV_S *pdev)
{
    return HI_SUCCESS;
}

static PDM_REGISTER_PARAM_S g_PdmProcPara = {
    .rdproc = PDM_ProcRead,
    .wtproc = PDM_ProcWrite,
};

static UMAP_DEVICE_S g_PdmRegisterData;


static struct file_operations g_PdmFops =
{
    .owner          =    THIS_MODULE,
    .open           =     PDM_DRV_Open,
    .unlocked_ioctl =    PDM_DRV_Ioctl,
    .release        =  PDM_DRV_Close,
};

static PM_BASEOPS_S g_PdmDrvOps = {
    .probe        = NULL,
    .remove       = NULL,
    .shutdown     = NULL,
    .prepare      = NULL,
    .complete     = NULL,
    .suspend      = PDM_Suspend,
    .suspend_late = NULL,
    .resume_early = NULL,
    .resume       = PDM_Resume,
};

/*******************************************
tag format is version=1.0.0.0  fb=0x85000000,0x10000  baseparam=0x86000000,0x2000 ����
*******************************************/
HI_S32 PDM_GetTagBuf(HI_VOID)
{
    HI_CHAR                 PdmTag[512];
    HI_U32                  PdmLen;
    HI_CHAR                 TmpBuf[32];
    HI_CHAR                 *p, *q;
    
    memset(PdmTag, 0x0, 512);

    PdmLen = get_param_data("pdm_tag", PdmTag, 512);
    if (PdmLen >= 512)
    {
        return HI_FAILURE;
    }

    PdmTag[511] = '\0';

    p = PdmTag;

    g_PdmGlobal.u32BufNum = 0;
    
    while (*p != '\0')
    {
        p = strstr(p, " ");
        if (0 == p)
        {
            return HI_SUCCESS;
        }

        q = strstr(p, "=");
        if (0 == q)
        {
            return HI_SUCCESS;
        }

        p++;

        memcpy(g_PdmGlobal.stBufInfo[g_PdmGlobal.u32BufNum].as8BufName, p, q-p);

        p = q+1;
        q = strstr(p, ",");
        if (0 == q)
        {
            return HI_FAILURE;
        }

        memset(TmpBuf, 0x0, sizeof(TmpBuf));
        memcpy(TmpBuf, p, q-p);

        g_PdmGlobal.stBufInfo[g_PdmGlobal.u32BufNum].u32PhyAddr = simple_strtoul(TmpBuf, NULL, 16);

        p = q+1;
        q = strstr(p, " ");
        if (0 == q)
        {
            q = PdmTag + PdmLen;
        }

        memset(TmpBuf, 0x0, sizeof(TmpBuf));
        memcpy(TmpBuf, p, q-p);
        
        g_PdmGlobal.stBufInfo[g_PdmGlobal.u32BufNum].u32Lenth = simple_strtoul(TmpBuf, NULL, 16);

        g_PdmGlobal.u32BufNum++;
    }

    return HI_SUCCESS;
}


HI_S32 HI_DRV_PDM_Init(HI_VOID)
{
    HI_S32  ret;
    
    ret = HI_DRV_MODULE_Register(HI_ID_PDM, PDM_NAME, (HI_VOID*)&g_PdmExportFuncs);
    if (HI_SUCCESS != ret)
    {
        HI_FATAL_PDM("ERR: HI_DRV_MODULE_Register!\n"); //552
        return ret;
    }    

    memset(&g_PdmGlobal, 0x0, sizeof(PDM_GLOBAL_S));

    HI_INIT_MUTEX(&g_PdmGlobal.PdmMutex);

    ret = PDM_GetTagBuf();
    if (HI_SUCCESS != ret)
    {
        memset(g_PdmGlobal.stBufInfo, 0x0, sizeof(PDM_BUF_INFO_S)*PDM_MAX_BUF_NUM);
    }
    
    return ret;
}

HI_S32 HI_DRV_PDM_DeInit(HI_VOID)
{
    HI_DRV_MODULE_UnRegister(HI_ID_PDM);

    return HI_SUCCESS;
}

HI_S32 PDM_DRV_ModInit(HI_VOID)
{
    HI_CHAR             ProcName[16];
    DRV_PROC_ITEM_S     *pProcItem = HI_NULL;

#if 0
#ifndef HI_MCE_SUPPORT
    HI_DRV_PDM_Init();
#endif
#endif

    HI_OSAL_Snprintf(ProcName, sizeof(ProcName), "%s", HI_MOD_PDM);

    pProcItem = HI_DRV_PROC_AddModule(ProcName, HI_NULL, HI_NULL);
    if(HI_NULL != pProcItem)
    {
        pProcItem->read = g_PdmProcPara.rdproc;
        pProcItem->write = g_PdmProcPara.wtproc;
    }
        
    HI_OSAL_Snprintf(g_PdmRegisterData.devfs_name, sizeof(g_PdmRegisterData.devfs_name), UMAP_DEVNAME_PDM);
    g_PdmRegisterData.fops = &g_PdmFops;
    g_PdmRegisterData.minor = UMAP_MIN_MINOR_PDM;
    g_PdmRegisterData.owner  = THIS_MODULE;
    g_PdmRegisterData.drvops = &g_PdmDrvOps;
    if (HI_DRV_DEV_Register(&g_PdmRegisterData) < 0)
    {
        HI_FATAL_PDM("register PDM failed.\n"); //601
        return HI_FAILURE;
    }
    
    return  0;
}

HI_VOID PDM_DRV_ModExit(HI_VOID)
{
    HI_CHAR             ProcName[16];
    
    HI_DRV_DEV_UnRegister(&g_PdmRegisterData);

    HI_OSAL_Snprintf(ProcName, sizeof(ProcName), "%s", HI_MOD_PDM);
    HI_DRV_PROC_RemoveModule(ProcName);
    
#if 0
#ifndef HI_MCE_SUPPORT
    HI_DRV_PDM_DeInit();
#endif
#endif

    return;
}

#ifdef MODULE
module_init(PDM_DRV_ModInit);
module_exit(PDM_DRV_ModExit);
#endif

MODULE_AUTHOR("HISILICON");
MODULE_LICENSE("GPL");




