//#include <linux/list.h>
#include "drv_compatibility.h"
#include <linux/kernel.h>

#include <linux/device.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/delay.h>
#include <linux/poll.h>
#include <mach/hardware.h>
#include <linux/interrupt.h>
#include <linux/vmalloc.h>
#include <linux/slab.h>

#include "drv_global.h"
#include "drv_reg_proc.h"
#include "hi_osal.h"
#include "si_delay.h"
#include "si_phy.h"

//Panasonic_TH-L32CH3C
HDMI_DELAY_TIME_S MEI_TH_L32CH3C =
{
    "MEI",49924,600,200,"MEI_TH_L32CH3C",
};

//Skyworth 47E760A
HDMI_DELAY_TIME_S SKY_47E760A =
{
    "SKY",48,100,0,"SKY_47E760A",
};

HDMI_DELAY_TIME_S SKW_24E60HR =
{
	"SKW",48,1200,0,"24E60HR",
};

HDMI_DELAY_TIME_S LCD_32Z100AS =
{
	"SHP",4149,0,0,"LCD-32Z100AS",
};

HDMI_DELAY_TIME_S SAM_LA40B550K1F =
{
    "SAM",1289,100,0,"LA40B550K1F",
};

HDMI_DELAY_TIME_S *WholeList[] =
{
    &MEI_TH_L32CH3C,
    &SKY_47E760A,
    &SKW_24E60HR,
    &LCD_32Z100AS,
    &SAM_LA40B550K1F,
    NULL
};

Struct_80faed54 SNY_KLV_32J400A =
{
	"SNY",23809,"KLV-32J400A",
};

Struct_80faed54* Data_80faed54[] =
{
	&SNY_KLV_32J400A,
	NULL,
};

static HI_U32 u32GlobalFmtDelay; //81134398 +0
static HI_BOOL bForceFmtDelay; //8113439c +4
static HI_BOOL bForceMuteDelay; //811343a0 +8
static HI_U32 u32GlobalMuteDelay; //811343a4 +12
static HI_BOOL Data_811343a8; //811343a8 +16


void SetGlobalFmtDelay(HI_U32 a)
{
	u32GlobalFmtDelay = a;
}

HI_U32 GetGlobalFmtDelay(void)
{
	return u32GlobalFmtDelay;
}

void SetForceDelayMode(HI_BOOL bFmtDelay, HI_BOOL bMuteDelay)
{
	bForceFmtDelay = bFmtDelay;
	bForceMuteDelay = bMuteDelay;
}

HI_BOOL IsForceFmtDelay(void)
{
	return bForceFmtDelay;
}

HI_BOOL IsForceMuteDelay(void)
{
	return bForceMuteDelay;
}

void SetGlobalMuteDelay(HI_U32 a)
{
	u32GlobalMuteDelay = a;
}

HI_U32 GetGlobalsMuteDelay(void)
{
	return u32GlobalMuteDelay;
}

HI_S32 GetFormatDelay(HI_UNF_HDMI_ID_E enHdmi, HI_U32* pDelayTime)
{
    HI_U32 i = 0;
    HI_U32 delayTime = 0;
	HI_UNF_EDID_BASE_INFO_S* pSinkCap = DRV_Get_SinkCap(enHdmi);

	if (pDelayTime == NULL)
	{
		HI_ERR_HDMI("Null Point Error \n"); //86
		return HI_FAILURE;
	}

	if (bForceFmtDelay)
	{
		delayTime = u32GlobalFmtDelay;
		HI_INFO_HDMI("HDMI SetFormat Force Delay %dms \n", delayTime); //93
	}
	else
	{
		for (i = 0; WholeList[i] != NULL; i++)
		{
			if((pSinkCap->stMfrsInfo.u32ProductCode == WholeList[i]->u32IDProductCode) &&
					(!HI_OSAL_Strncmp(pSinkCap->stMfrsInfo.u8MfrsName,WholeList[i]->u8IDManufactureName,32)))
			{
				delayTime = WholeList[i]->u32DelayTimes;
				HI_INFO_HDMI("Sink %s need Delay %d ms \n",WholeList[i]->u8ProductType,delayTime); //104
			}
		}

		if (delayTime == 0)
		{
#warning TODO
			if (!HI_OSAL_Strncmp(pSinkCap->stMfrsInfo.u8MfrsName,"SKY",32) ||
					!HI_OSAL_Strncmp(pSinkCap->stMfrsInfo.u8MfrsName,"SKW",32))
			{
				delayTime = 600;
				HI_INFO_HDMI("Sink SKYWorth should Delay %d ms \n", delayTime); //115
			}
			else
			{
				delayTime = u32GlobalFmtDelay;
				HI_INFO_HDMI("HDMI SetFormat Global Delay %dms \n", delayTime); //122
			}
		}
	}

	*pDelayTime = delayTime;

	return HI_SUCCESS;
}

HI_S32 GetmuteDelay(HI_UNF_HDMI_ID_E enHdmi, HI_U32* Delay)
{
    HI_U32 i = 0;
	HI_U32 delayTime = 0;
	HDMI_VIDEO_ATTR_S* r9 = DRV_Get_VideoAttr(enHdmi);
	HI_UNF_EDID_BASE_INFO_S* pSinkCap = DRV_Get_SinkCap(enHdmi);

	if (Delay == NULL)
	{
		HI_ERR_HDMI("Null Point Error \n"); //169
		return HI_FAILURE;
	}

	if (bForceMuteDelay)
	{
		delayTime = u32GlobalMuteDelay;
		HI_INFO_HDMI("HDMI SetFormat Force Delay %dms \n", u32GlobalFmtDelay); //176
	}
	else
	{
		for (i = 0; WholeList[i] != NULL; i++)
		{
			if((pSinkCap->stMfrsInfo.u32ProductCode == WholeList[i]->u32IDProductCode) &&
					(!HI_OSAL_Strncmp(pSinkCap->stMfrsInfo.u8MfrsName,WholeList[i]->u8IDManufactureName,32)))
			{
				delayTime = WholeList[i]->Data_12;
				HI_INFO_HDMI("Sink %s need Delay %d ms \n",WholeList[i]->u8ProductType,delayTime); //186
			}
		}

		if (delayTime == 0)
		{
			if ((r9->enVideoFmt >= HI_DRV_DISP_FMT_1080P_30) && (r9->enVideoFmt <= HI_DRV_DISP_FMT_1080P_24))
			{
				delayTime = 120;
			}
			else
			{
				delayTime = 50;
			}
		}
	}

	*Delay = delayTime;

	return HI_SUCCESS;
}


#if 0

HI_S32 SetFormatDelay(HI_UNF_HDMI_SINK_CAPABILITY_S *sinkCap,HI_U32 *DelayTime)
{
    HI_U32 i = 0;

    if(sinkCap == NULL || DelayTime == NULL)
    {
        HI_ERR_HDMI("Null Point Error \n");
        return HI_FAILURE;
    }

    for (i = 0; WholeList[i] != NULL; i++)
    {
        if((sinkCap->u32IDProductCode == WholeList[i]->u32IDProductCode) && (!strcmp(sinkCap->u8IDManufactureName,WholeList[i]->u8IDManufactureName)))
        {
            *DelayTime = WholeList[i]->u32DelayTimes;
            HI_PRINT("Sink %s need Delay %d ms \n",WholeList[i]->u8ProductType,*DelayTime);
        }
    }

    return HI_SUCCESS;
};

#endif







void Check1stOE(HI_UNF_HDMI_ID_E enHdmi)
{
	HI_U32 i = 0;
	HI_UNF_EDID_BASE_INFO_S* pSinkCap = DRV_Get_SinkCap(enHdmi);

	if (!Data_811343a8)
	{
		Data_811343a8 = HI_TRUE;

		for (i = 0; Data_80faed54[i] != NULL; i++)
		{
			if((pSinkCap->stMfrsInfo.u32ProductCode == Data_80faed54[i]->u32IDProductCode) &&
					(!HI_OSAL_Strncmp(pSinkCap->stMfrsInfo.u8MfrsName,Data_80faed54[i]->u8IDManufactureName,32)))
			{
				HI_INFO_HDMI("Sink %s need Reopen OE \n",Data_80faed54[i]->u8ProductType); //261

				SI_TX_PHY_DisableHdmiOutput();
				DelayMS(100);
				SI_TX_PHY_EnableHdmiOutput();
				return;
			}
		}

		if (!HI_OSAL_Strncmp(pSinkCap->stMfrsInfo.u8MfrsName,"SNY",32) &&
				((pSinkCap->stMfrsInfo.u32ProductCode == 2008) ||
				(pSinkCap->stMfrsInfo.u32ProductCode == 2009)))
		{
			HI_INFO_HDMI("Sony 2008-2009 series need Reopen OE \n"); //274

			SI_TX_PHY_DisableHdmiOutput();
			DelayMS(100);
			SI_TX_PHY_EnableHdmiOutput();

		}
	}
}




HI_BOOL Is3716Cv200ECVer(void)
{
	HI_U32 fp16;

	DRV_HDMI_ReadRegister(EC_3716CV200_VER, &fp16);

	fp16 = (fp16 >> 8) & 0xFF;
	HI_INFO_HDMI("3716cv200 EC Version tag:0x%02x \n", fp16); //294

	if (fp16 == 0x41)
	{
		HI_INFO_HDMI("3716cv200 EC Version \n"); //298
		return HI_TRUE;
	}
	else if (fp16 == 0x40)
	{
		HI_INFO_HDMI("3716cv200 original Version \n"); //303
		return HI_FALSE;
	}
	else
	{
		HI_ERR_HDMI("unKnown 3716cv200 version,force to 3716cv200 default \n"); //308
		return HI_FALSE;
	}
}

