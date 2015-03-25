/*  extdrv/interface/hdmi/hi_hdmi.c
 *
 * Copyright (c) 2006 Hisilicon Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program;
 *
 * History:
 *      19-April-2006 create this file
 *      hi_struct.h
 *      hi_debug.h

 */
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

//#include "common_dev.h" 头文件改名
#include "hi_drv_dev.h"
//#include "common_proc.h"
#include "hi_drv_proc.h"


//#include "mpi_priv_hdmi.h"
#include "hi_drv_hdmi.h"
#include "drv_hdmi.h"
#include "si_defstx.h"
#include "si_hdmitx.h"
#include "si_edid.h"
#include "si_phy.h"
#include "si_isr.h"
#include "si_delay.h"

//#include "hi_common_id.h"
#include "hi_module.h"
//#include "hi_common_log.h"
#include "hi_debug.h"
//#include "common_module_drv.h"
#include "hi_drv_module.h"
#include "drv_hdmi_ext.h"
#include "si_timer.h"
#include "drv_global.h"

#include "hi_osal.h"
#include "drv_reg_proc.h"
#include "si_defsmddc.h"

#ifdef ANDROID_SUPPORT

#include <linux/switch.h>

struct switch_dev hdmi_tx_sdev =
{     
    .name = "hdmi",  
};

HI_BOOL g_switchOk = HI_FALSE;


#endif


#define HDMI_NAME                      "HI_HDMI"
#if defined (SUPPORT_FPGA)
#include "hdmi_fpga.h"
#endif
HI_S32 DRV_HDMI_ReadPhy(void)
{
    HI_U32 u32Ret;
       
    u32Ret = SI_TX_PHY_GetOutPutEnable();
    
    return u32Ret;
}

//extern HI_U8 OutputState;
//extern HI_U8 AuthState;

extern HI_U32 unStableTimes;

/*****************************************************************************
 Prototype    : hdmi_Proc
 Description  : HDMI status in /proc/msp/hdmi
 Input        : None
 Output       : None
 Return Value :
 Calls        :
*****************************************************************************/

//HI_U32 setAttrChkCnt = 0;
#if 0 /*--NO MODIFY : COMMENT BY CODINGPARTNER--*/
static HI_S32 hdmi_Proc(struct seq_file *p, HI_VOID *v)
{
    HI_U32 Ret;
    HI_U32 u32Reg, index, offset;
    HI_UNF_HDMI_SINK_CAPABILITY_S sinkCap;
	HDMI_ATTR_S			          stHDMIAttr; 
    HDMI_VIDEO_ATTR_S            *pstVideoAttr;
    HDMI_AUDIO_ATTR_S            *pstAudioAttr;
    HDMI_APP_ATTR_S            *pstAppAttr;
#if defined (CEC_SUPPORT) 	
    HI_UNF_HDMI_CEC_STATUS_S      CECStatus;
#endif
    HI_U32 u32DefHDMIMode;

    memset(&sinkCap,0,sizeof(HI_UNF_HDMI_SINK_CAPABILITY_S));

    p += PROC_PRINT(p, "\n########### Hisi HDMI Dev Stat ###########\n");
    
    Ret = DRV_HDMI_GetAttr(HI_UNF_HDMI_ID_0, &stHDMIAttr);
    if(Ret != HI_SUCCESS)
    {
        p += PROC_PRINT(p, "HDMI driver do not Open\n" );
        p += PROC_PRINT(p, "\n#################### END ##################\n");
        return HI_SUCCESS;
    }
    pstVideoAttr = &stHDMIAttr.stVideoAttr;
    pstAudioAttr = &stHDMIAttr.stAudioAttr;
    pstAppAttr = &stHDMIAttr.stAppAttr;
    /* HPD Status */
    u32Reg = DRV_ReadByte_8BA(0, TX_SLV0, 0x09);// 0x72:0x09
    if ((u32Reg & 0x02) != 0x02)
    {
        p += PROC_PRINT(p, "HPD Status : Out\n");
    }
    else
    {
        p += PROC_PRINT(p, "HPD Status : IN\n");
    }
    /* HDMI Start mode */
    u32Reg = DRV_ReadByte_8BA(0, TX_SLV0, 0x08);// 0x72:0x08
    if ((u32Reg & 0x01) != 0x01)
    {
        p += PROC_PRINT(p, "HDMI do not Start!\n");
    }
    else
    {        
        /* HDMI Status */
        DRV_HDMI_GetSinkCapability(HI_UNF_HDMI_ID_0, &sinkCap);
        if (sinkCap.bIsSinkPowerOn == HI_TRUE)
        {
            p += PROC_PRINT(p, "TV : Power On\n");
        }
        else
        {
            p += PROC_PRINT(p, "TV : Power Off\n");
        }


        DRV_HDMI_GetPlayStatus(0,&u32Reg);
        if(u32Reg == HI_TRUE)
        {
            p += PROC_PRINT(p, "Play Status    : Start  \n");            
        }
        else
        {
            p += PROC_PRINT(p, "Play Status    : STOP  \n");
        }
        
        if (sinkCap.bIsRealEDID == HI_TRUE)
        {
            p += PROC_PRINT(p, "EDID Parse Status: Ok\n");
        }
        else
        {
            p += PROC_PRINT(p, "EDID Parse Status: Fail\n");
        }
        
        p += PROC_PRINT(p, "TV ManufactureName: %s\n", sinkCap.u8IDManufactureName);

        p += PROC_PRINT(p, "HDMI Sink video capability :\n");        
        for (index = 0; index < HI_DRV_DISP_FMT_BUTT; index++)
        {
            if(HI_TRUE == sinkCap.bVideoFmtSupported[hdmi_Disp2EncFmt(index)])
            {
                if (index == HI_DRV_DISP_FMT_1080P_60)
                {
                    p += PROC_PRINT(p, "Fmt: 1080P_60\n");
                }
                if (index == HI_DRV_DISP_FMT_1080P_50)
                {
                    p += PROC_PRINT(p, "Fmt: 1080P_50\n");
                }
                if (index == HI_DRV_DISP_FMT_1080P_30)
                {
                    p += PROC_PRINT(p, "Fmt: 1080P_30\n");
                }
                if (index == HI_DRV_DISP_FMT_1080P_25)
                {
                    p += PROC_PRINT(p, "Fmt: 1080P_25\n");
                }
                if (index == HI_DRV_DISP_FMT_1080P_24)
                {
                    p += PROC_PRINT(p, "Fmt: 1080P_24\n");
                }
                if (index == HI_DRV_DISP_FMT_1080i_60)
                {
                    p += PROC_PRINT(p, "Fmt: 1080i_60\n");
                }
                if (index == HI_DRV_DISP_FMT_1080i_50)
                {
                    p += PROC_PRINT(p, "Fmt: 1080i_50\n");
                }
                if (index == HI_DRV_DISP_FMT_720P_60)
                {
                    p += PROC_PRINT(p, "Fmt: 720P_60\n");
                }
                if (index == HI_DRV_DISP_FMT_720P_50)
                {
                    p += PROC_PRINT(p, "Fmt: 720P_50\n");
                }
                if (index == HI_DRV_DISP_FMT_576P_50)
                {
                    p += PROC_PRINT(p, "Fmt: 576P_50\n");
                }
                if (index == HI_DRV_DISP_FMT_480P_60)
                {
                    p += PROC_PRINT(p, "Fmt: 480P_60\n");
                }
                if (index == HI_DRV_DISP_FMT_PAL)
                {
                    p += PROC_PRINT(p, "Fmt: 576i_50\n");
                }
                if (index == HI_DRV_DISP_FMT_NTSC)
                {
                    p += PROC_PRINT(p, "Fmt: 480i_60\n");
                }
                if (index == HI_DRV_DISP_FMT_861D_640X480_60)
                {
                    p += PROC_PRINT(p, "Fmt: 640X480_60\n");
                }
#if defined (DVI_SUPPORT)
                if (index == HI_DRV_DISP_FMT_VESA_800X600_60)
                    p += PROC_PRINT(p, "Fmt: 800X600_60\n");

                if (index == HI_DRV_DISP_FMT_VESA_1024X768_60)
                    p += PROC_PRINT(p, "Fmt: 1024X768_60\n");

                if (index == HI_DRV_DISP_FMT_VESA_1366X768_60)
                    p += PROC_PRINT(p, "Fmt: 1366X768_60\n");

                if (index == HI_DRV_DISP_FMT_VESA_1440X900_60)
                    p += PROC_PRINT(p, "Fmt: 1400X900_60\n");

                if (index == HI_DRV_DISP_FMT_VESA_1440X900_60_RB)
                    p += PROC_PRINT(p, "Fmt: 1400X900_60_RB\n");

                if (index == HI_DRV_DISP_FMT_VESA_1600X1200_60)
                    p += PROC_PRINT(p, "Fmt: 1600X1200_60\n");

                 if (index == HI_DRV_DISP_FMT_VESA_1680X1050_60)
                    p += PROC_PRINT(p, "Fmt: 1680X1050_60\n");

                  if (index == HI_DRV_DISP_FMT_VESA_1680X1050_60_RB)
                    p += PROC_PRINT(p, "Fmt: 1680X1050_60_RB\n");

                  if (index == HI_DRV_DISP_FMT_VESA_1920X1200_60)
                    p += PROC_PRINT(p, "Fmt: 1920X1200_60\n");

                if (index == HI_DRV_DISP_FMT_VESA_1920X1080_60)
                    p += PROC_PRINT(p, "Fmt: 1920X1080_60\n");
                
                 if (index == HI_DRV_DISP_FMT_VESA_1920X1440_60)
                    p += PROC_PRINT(p, "Fmt: 1920X1440_60\n");

                if (index == HI_DRV_DISP_FMT_VESA_2048X1152_60)
                    p += PROC_PRINT(p, "Fmt: 2048X1152_60\n");
                
                if (index == HI_DRV_DISP_FMT_VESA_2560X1440_60_RB)
                    p += seq_printf(p, "Fmt: 2560X1440_60_RB\n");
                
                if (index == HI_DRV_DISP_FMT_VESA_2560X1600_60_RB)
                    p += seq_printf(p, "Fmt: 2560X1600_60_RB\n");
#endif

#if 0
                if(sinkCap.aspect_ratio[index][0] == 1)
                    p += PROC_PRINT(p, "aspect ratio:         4:3 \n");
                else if(sinkCap.aspect_ratio[index][1] == 2)
                    p += PROC_PRINT(p, "aspect ratio:               16:9\n");
#endif
            }
        }
        
        p += PROC_PRINT(p, "HDMI Sink Audio capability \n");    
        p += PROC_PRINT(p, "Audio Fmt support : ");    
        for (index = 0; index < HI_UNF_HDMI_MAX_AUDIO_CAP_COUNT; index++)
        {
            if (sinkCap.bAudioFmtSupported[index] == HI_TRUE)
            {
                 switch (index)
                {
                case 1:
                    p += PROC_PRINT(p, "LiniarPCM ");
                    break;
                case 2:
                    p += PROC_PRINT(p, "AC3 ");
                    break;
                case 3:
                    p += PROC_PRINT(p, "MPEG1 ");
                    break;
                case 4:
                    p += PROC_PRINT(p, "MP3 ");
                    break;
                case 5:
                    p += PROC_PRINT(p, "MPEG2 ");
                    break;
                case 6:
                    p += PROC_PRINT(p, "ACC ");
                    break;
                case 7:
                    p += PROC_PRINT(p, "DTS ");
                    break;
                case 8:
                    p += PROC_PRINT(p, "ATRAC ");
                    break;
                case 9:
                    p += PROC_PRINT(p, "OneBitAudio ");
                    break;
                case 10:
                    p += PROC_PRINT(p, "DD+ ");
                    break;
                case 11:
                    p += PROC_PRINT(p, "DTS_HD ");
                    break;
                case 12:
                    p += PROC_PRINT(p, "MAT ");
                    break;
                case 13:
                    p += PROC_PRINT(p, "DST ");
                    break;
                case 14:
                    p += PROC_PRINT(p, "WMA ");
                    break;
                default:
                    p += PROC_PRINT(p, "reserved "); 
                    break;
                }
            }
        }
        p += PROC_PRINT(p, "\n");

        p += PROC_PRINT(p, "Max Audio PCM channels: %d\n", sinkCap.u32MaxPcmChannels);
        p += PROC_PRINT(p, "Support Audio Sample Rates:");
        for (index = 0; index < HI_UNF_HDMI_MAX_AUDIO_SMPRATE_COUNT; index++)
        {
            if(sinkCap.u32AudioSampleRateSupported[index] != 0)
            {
                p += PROC_PRINT(p, " %d ", sinkCap.u32AudioSampleRateSupported[index]);
            }
        }
        p += PROC_PRINT(p, "\n");
        if(sinkCap.u8Speaker != HI_NULL)
        {
            p += PROC_PRINT(p, "Support Audio channels:");
            if(sinkCap.u8Speaker & 0x01)
                p += PROC_PRINT(p, " FL/FR ");
            if(sinkCap.u8Speaker & 0x02)
                p += PROC_PRINT(p, " LFE ");
            if(sinkCap.u8Speaker & 0x04)
                p += PROC_PRINT(p, " FC ");
            if(sinkCap.u8Speaker & 0x08)
                p += PROC_PRINT(p, " RL/RR ");
            if(sinkCap.u8Speaker & 0x10)
                p += PROC_PRINT(p, " RC ");
            if(sinkCap.u8Speaker & 0x20)
                p += PROC_PRINT(p, " FLC/FRC ");
            if(sinkCap.u8Speaker & 0x40)
                p += PROC_PRINT(p, " RLC/RRC ");    
            p += PROC_PRINT(p, "\n");
        }
     
        
#if defined (CEC_SUPPORT)    
        DRV_HDMI_CECStatus(HI_UNF_HDMI_ID_0, &CECStatus);
        if(CECStatus.bEnable == HI_TRUE)
        {
            p += PROC_PRINT(p, "CEC Status     : ON\n");
            p += PROC_PRINT(p, "CEC Phy Add    : %01d.%01d.%01d.%01d\n", CECStatus.u8PhysicalAddr[0],
                    CECStatus.u8PhysicalAddr[1], CECStatus.u8PhysicalAddr[2], CECStatus.u8PhysicalAddr[3]);
            p += PROC_PRINT(p, "CEC Logical Add: %01d\n", CECStatus.u8LogicalAddr);
        }
        else
        {
            p += PROC_PRINT(p, "CEC Status     : OFF\n");
        }
#endif
#if defined (HDCP_SUPPORT)       
        p += PROC_PRINT(p, "HDMI Output Attribute:\n");
        u32Reg = DRV_ReadByte_8BA(0, TX_SLV0, 0x0F);  // 0x72:0x0F  
        if (0X01 == (u32Reg & 0X01))
        {
            p += PROC_PRINT(p, "HDCP Encryption: ON\n");
        }
        else
        {
            p += PROC_PRINT(p, "HDCP Encryption: OFF\n");
        }

        if(pstAppAttr->bHDCPEnable == HI_TRUE)
        {
            p += PROC_PRINT(p, "HDCP Enable: ON\n");
        }
        else 
        {
            p += PROC_PRINT(p, "HDCP Enable: OFF\n");
        }
#endif

        p += PROC_PRINT(p, "PHY Output     : ");
        if (HI_TRUE == DRV_HDMI_ReadPhy())
        {
            p += PROC_PRINT(p, "Enable\n");
        }
        else
        {
            p += PROC_PRINT(p, "Disable\n");
            p += PROC_PRINT(p, "***HDMI is abnormal, You can not get HDMI Output***\n");
        }
                
        //DRV_HDMI_GetAttr(HI_UNF_HDMI_ID_0, &stHDMIAttr);
        
        if (pstAppAttr->bEnableVideo == HI_TRUE)
        {
            p += PROC_PRINT(p, "Video Output: Enable\n");
        }
        else
        {
            p += PROC_PRINT(p, "Video Output: Disable\n");
        }
        
        p+= PROC_PRINT(p, "Current Fmt    : ");
        if (pstVideoAttr->enVideoFmt == HI_DRV_DISP_FMT_1080P_60)
        {
            p += PROC_PRINT(p, "1080P_60\n");
        }
        else if (pstVideoAttr->enVideoFmt == HI_DRV_DISP_FMT_1080P_50)
        {
            p += PROC_PRINT(p, "1080P_50\n");
        }
        else if (pstVideoAttr->enVideoFmt == HI_DRV_DISP_FMT_1080P_30)
        {
            p += PROC_PRINT(p, "1080P_30\n");
        }
        else if (pstVideoAttr->enVideoFmt == HI_DRV_DISP_FMT_1080P_25)
        {
            p += PROC_PRINT(p, "1080P_25\n");
        }
        else if (pstVideoAttr->enVideoFmt == HI_DRV_DISP_FMT_1080P_24)
        {
            p += PROC_PRINT(p, "1080P_24\n");
        }
        else if (pstVideoAttr->enVideoFmt == HI_DRV_DISP_FMT_1080i_60)
        {
            p += PROC_PRINT(p, "1080i_60\n");
        }
        else if (pstVideoAttr->enVideoFmt == HI_DRV_DISP_FMT_1080i_50)
        {
            p += PROC_PRINT(p, "1080i_50\n");
        }
        else if (pstVideoAttr->enVideoFmt == HI_DRV_DISP_FMT_720P_60)
        {
            p += PROC_PRINT(p, "720P_60\n");
        }
        else if (pstVideoAttr->enVideoFmt == HI_DRV_DISP_FMT_720P_50)
        {
            p += PROC_PRINT(p, "720P_50\n");
        }
        else if (pstVideoAttr->enVideoFmt == HI_DRV_DISP_FMT_576P_50)
        {
            p += PROC_PRINT(p, "576P_50\n");
        }
        else if (pstVideoAttr->enVideoFmt == HI_DRV_DISP_FMT_480P_60)
        {
            p += PROC_PRINT(p, "480P_60\n");
        }
        else if (pstVideoAttr->enVideoFmt == HI_DRV_DISP_FMT_PAL)
        {
            p += PROC_PRINT(p, "576i_50\n");
        }
        else if (pstVideoAttr->enVideoFmt == HI_DRV_DISP_FMT_NTSC)
        {
            p += PROC_PRINT(p, "480i_60\n");
        }
        else if (pstVideoAttr->enVideoFmt == HI_DRV_DISP_FMT_861D_640X480_60)
        {
            p += PROC_PRINT(p, "640X480_60\n");
        }
#if defined (DVI_SUPPORT)
        else if (pstVideoAttr->enVideoFmt == HI_DRV_DISP_FMT_VESA_800X600_60)
        {
            p += PROC_PRINT(p, "800X600_60\n");
        }
        else if (pstVideoAttr->enVideoFmt == HI_DRV_DISP_FMT_VESA_1024X768_60)
        {
            p += PROC_PRINT(p, "1024X768_60\n");
        }
        else if (pstVideoAttr->enVideoFmt == HI_DRV_DISP_FMT_VESA_1280X1024_60)
        {
            p += PROC_PRINT(p, "1280X1024_60\n");
        }
        else if (pstVideoAttr->enVideoFmt == HI_DRV_DISP_FMT_VESA_1366X768_60)
        {
            p += PROC_PRINT(p, "1366X768_60\n");
        }
        else if (pstVideoAttr->enVideoFmt == HI_DRV_DISP_FMT_VESA_1440X900_60)
        {
            p += PROC_PRINT(p, "1440X900_60\n");
        }
        else if (pstVideoAttr->enVideoFmt == HI_DRV_DISP_FMT_VESA_1440X900_60_RB)
        {
            p += PROC_PRINT(p, "1440X900_60_RB\n");
        }
        else if (pstVideoAttr->enVideoFmt == HI_DRV_DISP_FMT_VESA_1600X1200_60)
        {
            p += PROC_PRINT(p, "1600X1200_60\n");
        }
        else if (pstVideoAttr->enVideoFmt == HI_DRV_DISP_FMT_VESA_1920X1200_60)
        {
            p += PROC_PRINT(p, "1920X1200_60\n");
        }
        else if (pstVideoAttr->enVideoFmt == HI_DRV_DISP_FMT_VESA_2048X1152_60)
        {
            p += PROC_PRINT(p, "2048X1152_60\n");
        }
#endif
        else
        {
            p += PROC_PRINT(p, "Unknown:%d\n", pstVideoAttr->enVideoFmt);
        }
        
        p += PROC_PRINT(p, "Color Space    : ");
        if (pstAppAttr->enVidOutMode == HI_UNF_HDMI_VIDEO_MODE_RGB444)
        {
            p += PROC_PRINT(p, "RGB444\n");
        }
        else if (pstAppAttr->enVidOutMode == HI_UNF_HDMI_VIDEO_MODE_YCBCR422)
        {
            p += PROC_PRINT(p, "YCbCr422\n");
        }
        else if (pstAppAttr->enVidOutMode == HI_UNF_HDMI_VIDEO_MODE_YCBCR444)
        {
            p += PROC_PRINT(p, "YCbCr444\n");
        }
        else 
		{
            p += PROC_PRINT(p, "Unknown:%d\n", pstAppAttr->enVidOutMode);
        }
        
        p += PROC_PRINT(p, "DeepColor      : ");
        if (pstAppAttr->enDeepColorMode == HI_UNF_HDMI_DEEP_COLOR_30BIT)
        {
            p += PROC_PRINT(p, "30bit\n");
        }
        else if (pstAppAttr->enDeepColorMode == HI_UNF_HDMI_DEEP_COLOR_36BIT)
        {
            p += PROC_PRINT(p, "36bit\n");
        }
        else
        {
            p += PROC_PRINT(p, "24bit\n");
        }
                
        p += PROC_PRINT(p, "AUD Output  : ");
        if (pstAppAttr->bEnableAudio == HI_TRUE)
        {
            p += PROC_PRINT(p, "Enable\n");
        }
        else
        {
            p += PROC_PRINT(p, "Disable\n");
        }
        
        p += PROC_PRINT(p, "AUD Input Type : ");
        if (pstAudioAttr->enSoundIntf == HDMI_AUDIO_INTERFACE_I2S)
        {
            p += PROC_PRINT(p, "I2S\n");
        }
        else if (pstAudioAttr->enSoundIntf == HDMI_AUDIO_INTERFACE_SPDIF)
        {
            p += PROC_PRINT(p, "SPDIF\n");
        }
        else if (pstAudioAttr->enSoundIntf == HDMI_AUDIO_INTERFACE_HBR)
        {
            p += PROC_PRINT(p, "HighBitRate\n");
        }
        else
        {
            p += PROC_PRINT(p, "Unknown:%d\n", pstAudioAttr->enSoundIntf);
        }
        p += PROC_PRINT(p, "AUD Sample Rate: %dHz\n", pstAudioAttr->enSampleRate);
        p += PROC_PRINT(p, "AUD Bit Depth  : %dbit\n", pstAudioAttr->enBitDepth);
        if(pstAudioAttr->bIsMultiChannel == HI_FALSE)
        {
           p += PROC_PRINT(p, "Aud Trace Mode: Stereo\n");
        }
        else
        {
           p += PROC_PRINT(p, "Aud Trace Mode: Multichannel(8)\n");
        }

        u32Reg = ReadByteHDMITXP1(0x05);  // 0x7A:0x05
        u32Reg = (u32Reg<<8) | ReadByteHDMITXP1(0x04);  // 0x7A:0x04
        u32Reg = (u32Reg<<8) | ReadByteHDMITXP1(0x03);  // 0x7A:0x03

        p += PROC_PRINT(p, "HDMI AUDIO N   : 0x%x(%d)\n",u32Reg,u32Reg);

        u32Reg = ReadByteHDMITXP1(0x0b);  // 0x7A:0x0b
        u32Reg = (u32Reg<<8) | ReadByteHDMITXP1(0x0a);  // 0x7A:0x0a
        u32Reg = (u32Reg<<8) | ReadByteHDMITXP1(0x09);  // 0x7A:0x09

        p += PROC_PRINT(p, "HDMI AUDIO CTS : 0x%x(%d)\n",u32Reg,u32Reg);

        u32Reg = ReadByteHDMITXP1(AUD_MODE_ADDR);
        p += PROC_PRINT(p, "AUD Mode       : 0x%02x\n",u32Reg);

		u32DefHDMIMode = DRV_Get_DefHDMIMode();
		if(HI_UNF_HDMI_DEFAULT_ACTION_HDMI == u32DefHDMIMode)
		{
			p += PROC_PRINT(p, "Default HDMI Mode: HDMI\n");
		}
		else if(HI_UNF_HDMI_DEFAULT_ACTION_DVI == u32DefHDMIMode)
		{
			p += PROC_PRINT(p, "Default HDMI Mode: DVI\n");
		}
		else if(HI_UNF_HDMI_DEFAULT_ACTION_NULL == u32DefHDMIMode)
		{
			p += PROC_PRINT(p, "Default HDMI Mode: NULL\n");
		}
		else
		{
    		p += PROC_PRINT(p, "Default HDMI Mode: Unknow\n");
		}
        
        /* HDMI Mode */
        p += PROC_PRINT(p, "Output Mode    : ");
        u32Reg = ReadByteHDMITXP1(0x2F);  // 0x7A:0x2F
        if ((u32Reg & 0x01) != 0x01)
        {
            p += PROC_PRINT(p, "DVI\n");
        }
        else
        {
            HI_BOOL VendorSpecInfoFlag = HI_FALSE;
            
            p += PROC_PRINT(p, "HDMI\n");
            
            u32Reg = DRV_ReadByte_8BA(0, TX_SLV1, 0x3E);  // 0x7A:0x3E
            if ( 0x03 == (u32Reg & 0x03))
            {
                p += PROC_PRINT(p, "AVI Infoframe  : Enable\n");
            }
            else
            {
                p += PROC_PRINT(p, "AVI Infoframe  : Disable\n");
            }
            
            if ( 0x30 == (u32Reg & 0x30))
            {
                p += PROC_PRINT(p, "AUD Infoframe  : Enable\n");
            }
            else
            {
                p += PROC_PRINT(p, "AUD Infoframe  : Disable\n");
            }
            
            if ( 0xc0 == (u32Reg & 0xc0))
            {
                p += PROC_PRINT(p, "MPG/VendorSpec Infoframe: Enable\n");
                VendorSpecInfoFlag = HI_TRUE;
            }
            else
            {
                p += PROC_PRINT(p, "MPG/VendorSpec Infoframe: Disable\n");
                VendorSpecInfoFlag = HI_FALSE;
            }
            
            u32Reg = DRV_ReadByte_8BA(0, TX_SLV1, 0x3F);  // 0x7A:0x3F
            if ( 0xC0 == (u32Reg & 0xC0))
            {
                p += PROC_PRINT(p, "Gamut Metadata Packet: Enable\n");
            }
            else
            {
                p += PROC_PRINT(p, "Gamut Metadata Packet: Disable\n");
            }
            if ( 0x03 == (u32Reg & 0x03))
            {
                p += PROC_PRINT(p, "Generic Packet Packet: Enable\n");
            }
            else
            {
                p += PROC_PRINT(p, "Generic Packet Packet: Disable\n");
            }
            
            u32Reg = DRV_ReadByte_8BA(0, TX_SLV1, 0xDF);  // 0x7A:0xDF
            if ( 0x10 == (u32Reg & 0x10))
            {
                p += PROC_PRINT(p, "AVMUTE         : Disable\n");
            }
            else if ( 0x01 == (u32Reg & 0x01))
            {
                p += PROC_PRINT(p, "AVMUTE         : Enable\n");
            }
            else
            {
                p += PROC_PRINT(p, "AVMUTE         : Unknown:%d\n", u32Reg);
            }

            /* AVI InfoFrame */
            p += PROC_PRINT(p, "AVI Inforframe:\n");
            for(index = 0; index < 17; index ++)
            {
                offset = 0x40 + index;//0x7A:0x40
                u32Reg = DRV_ReadByte_8BA(0, TX_SLV1, offset);
                p += PROC_PRINT(p, "0x%02x,", u32Reg);
            }
            p += PROC_PRINT(p, "\n");
            /* AUD InforFrame */
            p += PROC_PRINT(p, "AUD Inforframe:\n");
            for(index = 0; index < 9; index ++)
            {
                offset = 0x80 + index;//0x7A:0x80
                u32Reg = DRV_ReadByte_8BA(0, TX_SLV1, offset);
                p += PROC_PRINT(p, "0x%02x,", u32Reg);
            }
            p += PROC_PRINT(p, "\n");
            if (VendorSpecInfoFlag == HI_TRUE)
            {
                HI_U8 Packettype = 0, VideoFormat = 0, _3Dtype = 0;
                /* MPg/VendorSpec InforFrame */
                p += PROC_PRINT(p, "MPg/VendorSpec Inforframe:\n");
                for(index = 0; index < 12; index ++)
                {
                    offset = 0xa0 + index;//0x7A:0xA0
                    u32Reg = DRV_ReadByte_8BA(0, TX_SLV1, offset);
                    p += PROC_PRINT(p, "0x%02x,", u32Reg);
                    if (index == 0)
                    {
                        Packettype = u32Reg;
                    }
                    else if (index == 7)
                    {
                        VideoFormat = (u32Reg & 0xe0) >> 5;
                    }
                    else if (index == 8)
                    {
                        _3Dtype = (u32Reg & 0xf0) >> 4;
                    }
                }
                if ((Packettype == 0x81) && (VideoFormat == 0x02))
                {
                    //3D format
                    switch (_3Dtype)
                    {
                    case 0:
                        p += PROC_PRINT(p, " (3D:FramePacking) ");
                        break;
                    case 6:
                        p += PROC_PRINT(p, " (3D:Top-and-Bottom) ");
                        break;
                    case 8:
                        p += PROC_PRINT(p, " (3D:Side-By-Side half) ");
                        break;
                    }
                }
                p += PROC_PRINT(p, "\n");                
            }
        }      

        p += PROC_PRINT(p, "InitNum        : %d \n", DRV_HDMI_InitNum(0));
        p += PROC_PRINT(p, "ProcNum        : %d \n", DRV_HDMI_ProcNum(0));

        u32Reg = DRV_ReadByte_8BA(0, TX_SLV1, 0x48);
        u32Reg = u32Reg & 0x0f;
        if(u32Reg != 0x00)
        {
            p += PROC_PRINT(p, "Pixel Repeat   : 2x \n");
        }
        else
        {
            p += PROC_PRINT(p, "Pixel Repeat   : 1x(No repeat) \n");
        }

        u32Reg = ReadByteHDMITXP0(TX_STAT_ADDR);

        if((u32Reg & BIT_HDMI_PSTABLE)!=0)
        {
            p += PROC_PRINT(p, "Pixel Clk      : stable \n");            
        }
        else
        {
            p += PROC_PRINT(p, "Pixel Clk      : !!Warnning!! Clock Unstable\n");
        }     
        
        p += PROC_PRINT(p, "Unstable Times : %d \n",unStableTimes);

        u32Reg = ReadByteHDMITXP0(0x3b);
        u32Reg = (u32Reg << 8) | ReadByteHDMITXP0(0x3a);        
        p += PROC_PRINT(p, "H Total        : %d ( 0x%x )\n",u32Reg,u32Reg);

        u32Reg = ReadByteHDMITXP0(0x3d);
        u32Reg = (u32Reg << 8) | ReadByteHDMITXP0(0x3c);
        
        p += PROC_PRINT(p, "V Total        : %d ( 0x%x )\n",u32Reg,u32Reg);

        u32Reg = ReadByteHDMITXP0(INTERLACE_POL_DETECT);
        if((u32Reg & BIT_I_DETECTR)!=0)
        {
            p += PROC_PRINT(p, "InterlaceDetect: interlace\n");
        }
        else
        {
            p += PROC_PRINT(p, "InterlaceDetect: progress\n");
        }

        u32Reg = ReadByteHDMITXP1(DIAG_PD_ADDR);

        p += PROC_PRINT(p, "Power State    : 0x%02x\n",u32Reg);

        if(sinkCap.bHDMI_Video_Present == HI_TRUE)
        {
            p += PROC_PRINT(p, "sink 3D support: support \n");
        }
        else
        {
            p += PROC_PRINT(p, "sink 3D support: not support \n");
        }

        u32Reg = ReadByteHDMITXP0(0xf6);
        p += PROC_PRINT(p, "DDC Delay Count: 0x%02x(%d)\n",u32Reg,u32Reg);

        //60Mhz osc clk
        p += PROC_PRINT(p, "DDC Speed      : %dHz\n",(60000000 / (u32Reg * 30)));
	/* print EDID data */
        if (sinkCap.bIsRealEDID == HI_TRUE)
        {
            HI_U32 index,u32EdidLegth = 0;
            HI_U8  Data[512];

            p += PROC_PRINT(p, "EDID Raw Data:\n");
            memset(Data, 0, 512);
            u32EdidLegth = 128*(sinkCap.u8EDIDExternBlockNum + 1);

            if(u32EdidLegth > 512)
            {
                u32EdidLegth = 512;
            }
            
            SI_Proc_ReadEDIDBlock(Data, u32EdidLegth);
            for (index = 0; index < u32EdidLegth; index ++)
            {
                p += PROC_PRINT(p, "%02x ", Data[index]);
                if (0 == ((index + 1) % 16))
                {
                    p += PROC_PRINT(p, "\n");
                }
            }
            
        }
    }
    //p += PROC_PRINT(p, "setAttrChkCnt : %d \n",setAttrChkCnt);
    //p += PROC_PRINT(p, "OutputState : %d \n",OutputState);
    //p += PROC_PRINT(p, "AuthState : %d \n",AuthState);
    p += PROC_PRINT(p, "\n#################### END ##################\n" );
    return HI_SUCCESS;
}
#endif /*--NO MODIFY : COMMENT BY CODINGPARTNER--*/

#define DEF_FILE_NAMELENGTH 20

HI_S32 hdmi_GetProcArg(HI_CHAR*  chCmd,HI_CHAR*  chArg,HI_U32 u32ArgIdx)
{
    HI_U32 u32Count;
    HI_U32 u32CmdCount;
    HI_U32 u32LogCount;
    HI_U32 u32NewFlag;
    HI_CHAR chArg1[DEF_FILE_NAMELENGTH] = {0};
    HI_CHAR chArg2[DEF_FILE_NAMELENGTH] = {0};
    u32CmdCount = 0;

    /*清除前面的空格*/
    u32Count = 0;
    u32CmdCount = 0;
    u32LogCount = 1;
    u32NewFlag = 0;
    while(chCmd[u32Count] != 0 && chCmd[u32Count] != '\n' )
    {
        if (chCmd[u32Count] != ' ')
        {
            u32NewFlag = 1;
        }
        else
        {
            if(u32NewFlag == 1)
            {
                u32LogCount++;
                u32CmdCount= 0;
                u32NewFlag = 0;
            }
        }
        
        if (u32NewFlag == 1)
        {
            switch(u32LogCount)
            {
                case 1:
                    chArg1[u32CmdCount] = chCmd[u32Count];
                    u32CmdCount++;
                    break;
                case 2:
                    chArg2[u32CmdCount] = chCmd[u32Count];
                    u32CmdCount++;
                    break;
                default:
                    break;
            }
            
        }
        u32Count++;
    }
    
    switch(u32ArgIdx)
    {
        case 1:
            memcpy(chArg,chArg1,sizeof(HI_CHAR)*DEF_FILE_NAMELENGTH);
            break;
        case 2:
            memcpy(chArg,chArg2,sizeof(HI_CHAR)*DEF_FILE_NAMELENGTH);
            break;
        default:
            break;
    }
    return HI_SUCCESS;
}



extern HI_S32 hdmi_Open(struct inode *inode, struct file *filp);
extern HI_S32 hdmi_Close(struct inode *inode, struct file *filp);
extern HI_S32 hdmi_Ioctl(struct inode *inode, struct file *file,
                           unsigned int cmd, HI_VOID *arg);
extern HI_S32 hdmi_Suspend(PM_BASEDEV_S *pdev, pm_message_t state);
extern HI_S32 hdmi_Resume(PM_BASEDEV_S *pdev);

static HDMI_EXPORT_FUNC_S s_stHdmiExportFuncs = {
    .pfnHdmiInit = HI_DRV_HDMI_Init,
    .pfnHdmiDeinit = HI_DRV_HDMI_Deinit,
    .pfnHdmiOpen  = HI_DRV_HDMI_Open,
    .pfnHdmiClose = HI_DRV_HDMI_Close,
    .pfnHdmiGetPlayStus = HI_DRV_HDMI_PlayStus,
    .pfnHdmiGetAoAttr = HI_DRV_AO_HDMI_GetAttr,
    .pfnHdmiGetSinkCapability = HI_DRV_HDMI_GetSinkCapability,
    .pfnHdmiGetAudioCapability = HI_DRV_HDMI_GetAudioCapability,
    .pfnHdmiAudioChange = HI_DRV_HDMI_AudioChange,
    .pfnHdmiPreFormat = HI_DRV_HDMI_PreFormat,
    .pfnHdmiSetFormat = HI_DRV_HDMI_SetFormat,
//  .pfnHdmiSet3DMode = HI_DRV_HDMI_Set3DMode,
};

static long  hdmi_Drv_Ioctl(struct file *file,unsigned int cmd, unsigned long arg) 
{
	return (long)HI_DRV_UserCopy(file->f_dentry->d_inode, file, cmd, arg, hdmi_Ioctl);
}

static struct file_operations hdmi_FOPS =
{
    owner   : THIS_MODULE,
    open    : hdmi_Open,
    unlocked_ioctl   : hdmi_Drv_Ioctl,
    release : hdmi_Close,
};

static /*struct*/ PM_BASEOPS_S  hdmi_DRVOPS = {
	.probe        = NULL,
	.remove       = NULL,
	.shutdown     = NULL,
	.prepare      = NULL,
	.complete     = NULL,
	.suspend      = hdmi_Suspend,
	.suspend_late = NULL,
	.resume_early = NULL,
	.resume       = hdmi_Resume,
};

static UMAP_DEVICE_S   g_hdmiRegisterData;
static HI_U8 *g_pDefHDMIMode[] = {"NULL","HDMI","DVI","BUTT"};

static HI_U8 *g_pDispFmtString[] = 
{
    "1080P60", 
    "1080P50", 
    "1080P30", 
    "1080P25",
    "1080P24",        
    "1080i60",        
    "1080i50",        
    "720P60",         
    "720P50",         

    "576P50",         
    "480P60",         

    "PAL",
    "PAL_B",
    "PAL_B1",
    "PAL_D",
    "PAL_D1",
    "PAL_G", 
    "PAL_H",
    "PAL_K",
    "PAL_I",
    "PAL_N",
    "PAL_Nc",
    "PAL_M",
    "PAL_60",

    "NTSC",
    "NTSC_J",
    "NTSC_443",

    "SECAM_SIN",
    "SECAM_COS", 
    "SECAM_L",      
    "SECAM_B",      
    "SECAM_G",      
    "SECAM_D",      
    "SECAM_K",      
    "SECAM_H",      
                                        
    "1440x576i_50",                        
    "1440x480i_60",                        
                                     
    "1080P_24_FP",                         
    "720P_60_FP",                          
    "720P_50_FP",                          
                                         
    "640X480_60",                     
    "800X600_60",                     
    "1024X768_60",                    
    "1280X720_60",                    
    "1280X800_60",                    
    "1280X1024_60",         
    "1360X768_60",          
    "1366X768_60",          
    "1400X1050_60",         
    "1440X900_60",          
    "1440X900_60_RB",       
    "1600X900_60_RB",       
    "1600X1200_60",         
    "1680X1050_60",         
    "1680X1050_60_RB",      
    "1920X1080_60",                   
    "1920X1200_60",                   
    "1920X1440_60",                   
    "2048X1152_60",                   
    "2560X1440_60_RB",                
    "2560X1600_60_RB",                
                        
    "Customer Timing" 
};

static HI_U8 *g_pUnfFmtString[] = 
{
    "1080P60", 
    "1080P50", 
    "1080P30", 
    "1080P25",
    "1080P24",        
    "1080i60",        
    "1080i50",        
    "720P60",         
    "720P50",         

    "576P50",         
    "480P60",         

    "PAL",
    "PAL_N",
    "PAL_Nc",

    "NTSC",
    "NTSC_J",
    "NTSC_PAL_M",

    "SECAM_SIN",
    "SECAM_COS",
  
    "1080P24_FP",
    "720P60_FP",
    "720P50_FP",

    "640x480", 
    "800x600", 
    "1024x768", 
    "1280x720",
    "1280x800",
    "1280x1024",
    "1360x768",
    "1366x768",
    "1400x1050",
    "1440x900",        
    "1440x900_RB",

    "1600x900_RB",
    "1600x1200",
    "1680x1050",
    "1680x1050_RB",
    "1920x1080",
    "1920x1200",
    "1920x1440",
    "2048x1152",
    "2560x1440_RB",
    "2560x1600_RB",
    "CustomerTiming"
};

#if 0 /*--NO MODIFY : COMMENT BY CODINGPARTNER--*/
static HI_U8 *g_pAudioFmtCode[]= 
{
    "Reserved", "PCM",  "AC3",     "MPEG1", "MP3",   "MPEG2", "AAC",
    "DTS",     "ATRAC", "ONE_BIT", "DDP",   "DTS_HD", "MAT",  "DST",
    "WMA_PRO"
};
#endif /*--NO MODIFY : COMMENT BY CODINGPARTNER--*/
#if 0 /*--NO MODIFY : COMMENT BY CODINGPARTNER--*/
static HI_U8 *g_pSampleRate[] = 
{
    "32", "44.1", "48", "88.2","96","176.4","192","BUTT"
};
#endif /*--NO MODIFY : COMMENT BY CODINGPARTNER--*/
static HI_U8 *g_pSpeaker[] = 
{
    "FL/FR", "LFE", "FC", "RL/RR","RC","FLC/FRC","RLC/RRC",
    "FLW/FRW","FLH/FRH","TC","FCH"
}; 


static HI_U8 *g_pAudInputType[] = { "I2S","SPDIF","HBR","BUTT"};
static HI_U8 *g_pColorSpace[] = {"RGB444","YCbCr422","YCbCr444","Future"};
static HI_U8 *g_pDeepColor[] = {"24bit","30bit","36bit","OFF","BUTT"};
#if 0 /*-- no used now--*/
static HI_U8 *g_p3DMode[] = {
    "2D", 
    "FPK", 
    "SBS_HALF",
    "TAB",
    "FILED_ALTE",
    "LINE_ALTE",
    "SBS_FULL", 
    "L_DEPTH",
    "L_DEPTH_G_DEPTH",
};
#endif /*--NO MODIFY : COMMENT BY CODINGPARTNER--*/
static HI_U8 *g_pScanInfo[] = {"No Data","OverScan","UnderScan","Future"};
static HI_U8 *g_pPixelRep[] = {"1x(No Repeat)","2x","3x","4x","5x","6x","7x","8x","9x","10x","Reserved"};

static HI_S32 HDMI0_Proc(struct seq_file *p, HI_VOID *v)
{
    HI_U32 u32Reg, index, offset,Ret,u32DefHDMIMode;
	HDMI_ATTR_S			          stHDMIAttr; 
    HDMI_VIDEO_ATTR_S            *pstVideoAttr;
    HDMI_AUDIO_ATTR_S            *pstAudioAttr;
    HDMI_APP_ATTR_S              *pstAppAttr;
    HI_UNF_HDMI_SINK_CAPABILITY_S sinkCap;
    HI_UNF_HDMI_CEC_STATUS_S      CECStatus;
    HI_U32                        u32PlayStatus = 0;
    HI_S32 s32Temp;

    memset(&sinkCap,0,sizeof(HI_UNF_HDMI_SINK_CAPABILITY_S));
   // memset(u8DefHDMI, 0, sizeof(u8DefHDMI));
    p += PROC_PRINT(p, "--------------------------------- Hisi HDMI Dev Stat --------------------------------\n");
    Ret = DRV_HDMI_GetAttr(HI_UNF_HDMI_ID_0, &stHDMIAttr);
    if(Ret != HI_SUCCESS)
    {
        p += PROC_PRINT(p, "HDMI driver do not Open\n" );
        p += PROC_PRINT(p, "----------------------------------------- END -----------------------------------------\n");
        return HI_SUCCESS;
    }
    u32Reg = DRV_ReadByte_8BA(0, TX_SLV0, 0x08);// 0x72:0x08
    if ((u32Reg & 0x01) != 0x01)
    {
        p += PROC_PRINT(p, "HDMI do not Start!\n");
        p += PROC_PRINT(p, "----------------------------------------- END -----------------------------------------\n");
        return HI_SUCCESS;
    }
    pstVideoAttr = &stHDMIAttr.stVideoAttr;
    pstAudioAttr = &stHDMIAttr.stAudioAttr;
    pstAppAttr = &stHDMIAttr.stAppAttr;
    DRV_HDMI_GetSinkCapability(HI_UNF_HDMI_ID_0, &sinkCap);
    p += PROC_PRINT(p, "%-20s: ","Hotplug");
    u32Reg = DRV_ReadByte_8BA(0, TX_SLV0, 0x09);
    if(0x02&u32Reg)
    {
        p += PROC_PRINT(p, "%-20s| ", "Enable");
    }
    else
    { 
        p += PROC_PRINT(p, "%-20s| ", "Disable");
    }
    p += PROC_PRINT(p, "%-20s: ","Thread");
    s32Temp = DRV_Get_IsThreadStoped();
    if(s32Temp)
    {
        p += PROC_PRINT(p, "%s\n", "Enable");
    }
    else
    {
        p += PROC_PRINT(p, "%s\n", "Disable");
    }
    
    p += PROC_PRINT(p, "%-20s: ","Sink");
    if(sinkCap.bIsSinkPowerOn)
    {
        p += PROC_PRINT(p, "%-20s| ", "Active");
    }
    else
    {
        p += PROC_PRINT(p, "%-20s| ", "Deactive");
    }
    p += PROC_PRINT(p, "%-20s: ","HDCP Enable");
    if(pstAppAttr->bHDCPEnable)
    {
        p += PROC_PRINT(p, "%s\n", "ON");
    }
    else
    {
        p += PROC_PRINT(p, "%s\n", "OFF");
    }

    p += PROC_PRINT(p, "%-20s: ","PHY Output");
    s32Temp = DRV_HDMI_ReadPhy();
    if(s32Temp)
    {
        p += PROC_PRINT(p, "%-20s| ", "Enable");
    }
    else
    {
        p += PROC_PRINT(p, "%-20s| ", "Disable");
    }
    p += PROC_PRINT(p, "%-20s: ","HDCP Encryption");
    s32Temp = DRV_ReadByte_8BA(0, TX_SLV0, 0x0F);
    if(s32Temp&0x01)
    {
        p += PROC_PRINT(p, "%s\n", "ON");
    }
    else
    {
        p += PROC_PRINT(p, "%s\n", "OFF");
    }

    p += PROC_PRINT(p, "%-20s: ","Play Status");
    DRV_HDMI_GetPlayStatus(0,&u32PlayStatus);
    if(u32PlayStatus)
    {
        p += PROC_PRINT(p, "%-20s| ", "Start");
    }
    else
    {
        p += PROC_PRINT(p, "%-20s| ", "Stop");
    }
    p += PROC_PRINT(p, "%-20s: ","CEC Status");
    memset(&CECStatus, 0, sizeof(HI_UNF_HDMI_CEC_STATUS_S));
#ifdef CEC_SUPPORT
    DRV_HDMI_CECStatus(HI_UNF_HDMI_ID_0, &CECStatus);
#endif
    if(CECStatus.bEnable)
    {
        p += PROC_PRINT(p, "%s\n", "Enable");
    }
    else
    {
        p += PROC_PRINT(p, "%s\n", "Disable");
    }

    p += PROC_PRINT(p, "%-20s: ","EDID Status");
    //s32Temp = DRV_HDMI_ReadPhy();
    if(sinkCap.bIsRealEDID)
    {
        p += PROC_PRINT(p, "%-20s| ", "OK");
    }
    else
    {
        p += PROC_PRINT(p, "%-20s| ", "NO");
    }
    p += PROC_PRINT(p, "%-20s: ","CEC Phy Addr");
   
    p += PROC_PRINT(p, "%02d.%02d.%02d.%02d\n", CECStatus.u8PhysicalAddr[0],
        CECStatus.u8PhysicalAddr[1],CECStatus.u8PhysicalAddr[2],CECStatus.u8PhysicalAddr[3]);


    p += PROC_PRINT(p, "%-20s: ","Default Mode");
    u32DefHDMIMode = DRV_Get_DefHDMIMode();
    p += PROC_PRINT(p, "%-20s| ", g_pDefHDMIMode[u32DefHDMIMode]);
    p += PROC_PRINT(p, "%-20s: ","CEC Logical Addr");
    p += PROC_PRINT(p, "%d\n", CECStatus.u8LogicalAddr);


    p += PROC_PRINT(p, "%-20s: ","Output Mode");
    u32Reg = ReadByteHDMITXP1(0x2F);
    if(u32Reg & 0x01)
    {
        p += PROC_PRINT(p, "%-20s| ", "HDMI");
    }
    else
    {
        p += PROC_PRINT(p, "%-20s| ", "DVI");
    }
    p += PROC_PRINT(p, "\n");
    
    p += PROC_PRINT(p, "---------------- Video -------------------|---------------- Audio -------------------\n");
    
    p += PROC_PRINT(p, "%-20s: ","Video Output ");
    if(pstAppAttr->bEnableVideo)
    {
        p += PROC_PRINT(p, "%-20s| ", "Enable");
    }
    else
    {
        p += PROC_PRINT(p, "%-20s| ", "Disable");
    }
    p += PROC_PRINT(p, "%-20s: ","AUD Output");
    if(pstAppAttr->bEnableAudio)
    {
        p += PROC_PRINT(p, "%s\n", "Enable");
    }
    else
    {
        p += PROC_PRINT(p, "%s\n", "Disable");
    }

    p += PROC_PRINT(p, "%-20s: ","Current Fmt");
    p += PROC_PRINT(p, "%-20s| ", g_pDispFmtString[pstVideoAttr->enVideoFmt]);
    p += PROC_PRINT(p, "%-20s: ","Input Type");
    p += PROC_PRINT(p, "%s\n", g_pAudInputType[pstAudioAttr->enSoundIntf]);

    p += PROC_PRINT(p, "%-20s: ","Color Space");
    p += PROC_PRINT(p, "%-20s| ", g_pColorSpace[pstAppAttr->enVidOutMode]);
    p += PROC_PRINT(p, "%-20s: ","Sample Rate");
    p += PROC_PRINT(p, "%dHZ\n", pstAudioAttr->enSampleRate);

    p += PROC_PRINT(p, "%-20s: ","DeepColor");
    p += PROC_PRINT(p, "%-20s| ", g_pDeepColor[pstAppAttr->enDeepColorMode]);
    p += PROC_PRINT(p, "%-20s: ","Bit Depth");
    p += PROC_PRINT(p, "%dbit\n", pstAudioAttr->enBitDepth);


    p += PROC_PRINT(p, "%-20s: ","xvYCC");
    if(pstAppAttr->bxvYCCMode)
    {
        p += PROC_PRINT(p, "%-20s| ", "Enable");
    }
    else
    {
        p += PROC_PRINT(p, "%-20s| ", "Disable");
    }
    p += PROC_PRINT(p, "%-20s: ","Trace Mode");
    if(pstAudioAttr->bIsMultiChannel)
    {
        p += PROC_PRINT(p, "%s\n", "Multichannel(8)");        
    }
    else
    {
        p += PROC_PRINT(p, "%s\n", "Stereo");
    }
    
  //  pstVideoAttr->u83DParam = 0;
    p += PROC_PRINT(p, "%-20s: ","3D Mode");
   // p += PROC_PRINT(p, "%-20s| ", g_p3DMode[pstVideoAttr->u83DParam]);
    if(0 == pstVideoAttr->u83DParam)
    {
        p += PROC_PRINT(p, "%-20s| ", "FPK");
    }
    else if (8 == pstVideoAttr->u83DParam)
    {
        p += PROC_PRINT(p, "%-20s| ", "SBS HALF");
    }
    else if (6 == pstVideoAttr->u83DParam)
    {
        p += PROC_PRINT(p, "%-20s| ", "TAB");
    }
    else
    {
        p += PROC_PRINT(p, "%-20s| ", "2D");
    }
    
    p += PROC_PRINT(p, "%-20s: ","N Value");
    u32Reg = ReadByteHDMITXP1(0x05);  // 0x7A:0x05
    u32Reg = (u32Reg<<8) | ReadByteHDMITXP1(0x04);  // 0x7A:0x04
    u32Reg = (u32Reg<<8) | ReadByteHDMITXP1(0x03);  // 0x7A:0x03
    p += PROC_PRINT(p, "0x%x(%d)\n",u32Reg,u32Reg);
    
    p += PROC_PRINT(p, "%-20s: ","AVMUTE");
    u32Reg = DRV_ReadByte_8BA(0, TX_SLV1, 0xDF);  // 0x7A:0xDF
    if ( 0x10 == (u32Reg & 0x10))
    {
        p += PROC_PRINT(p, "%-20s| ", "Disable");
    }
    else if ( 0x01 == (u32Reg & 0x01))
    {
        p += PROC_PRINT(p, "%-20s| ", "Enable");
    }
    else
    {
        p += PROC_PRINT(p, "%-20s| ", "Unknown");
    }
    p += PROC_PRINT(p, "%-20s: ","CTS");
    u32Reg = ReadByteHDMITXP1(0x0b);  // 0x7A:0x0b
    u32Reg = (u32Reg<<8) | ReadByteHDMITXP1(0x0a);  // 0x7A:0x0a
    u32Reg = (u32Reg<<8) | ReadByteHDMITXP1(0x09);  // 0x7A:0x09
    p += PROC_PRINT(p, "0x%x(%d)\n",u32Reg,u32Reg);
    
    p += PROC_PRINT(p, "---------------------------------- Info Frame status --------------------------------\n");
    p += PROC_PRINT(p, "%-25s: ","AVI InfoFrame");
    u32Reg = DRV_ReadByte_8BA(0, TX_SLV1, 0x3E);  // 0x7A:0x3E
    if ( 0x03 == (u32Reg & 0x03))
    {
        p += PROC_PRINT(p, "%-15s| ", "Enable");
    }
    else
    {
        p += PROC_PRINT(p, "%-15s| ", "Disable");
    }
    p += PROC_PRINT(p, "%-23s: ","Gamut Metadata Packet");
    u32Reg = DRV_ReadByte_8BA(0, TX_SLV1, 0x3F);  // 0x7A:0x3F
    if(0xC0 == (u32Reg & 0xC0))
    {
        p += PROC_PRINT(p, "%s\n", "Enable");
    }
    else
    {
        p += PROC_PRINT(p, "%s\n", "Disable");
    }

    p += PROC_PRINT(p, "%-25s: ","AUD InfoFrame");
    u32Reg = DRV_ReadByte_8BA(0, TX_SLV1, 0x3E);  // 0x7A:0x3E
    if ( 0x30 == (u32Reg & 0x30))
    {
        p += PROC_PRINT(p, "%-15s| ", "Enable");
    }
    else
    {
        p += PROC_PRINT(p, "%-15s| ", "Disable");
    }
    p += PROC_PRINT(p, "%-23s: ","Generic Packet");
    u32Reg = DRV_ReadByte_8BA(0, TX_SLV1, 0x3F);  // 0x7A:0x3F
    if(0x03 == (u32Reg & 0x03))
    {
        p += PROC_PRINT(p, "%s\n", "Enable");
    }
    else
    {
        p += PROC_PRINT(p, "%s\n", "Disable");
    }

    p += PROC_PRINT(p, "%-25s: ","MPg/VendorSpec InfoFrame");
    u32Reg = DRV_ReadByte_8BA(0, TX_SLV1, 0x3E);  // 0x7A:0x3E
    if ( 0xC0 == (u32Reg & 0xC0))
    {
        p += PROC_PRINT(p, "%-15s| ", "Enable");
    }
    else
    {
        p += PROC_PRINT(p, "%-15s| ", "Disable");
    }
    p += PROC_PRINT(p, "\n");
    p += PROC_PRINT(p, "-------------------------------------- Raw Data -------------------------------------\n");
    p += PROC_PRINT(p, "AVI InfoFrame :\n");
    for(index = 0; index < 17; index ++)
    {
        offset = 0x40 + index;//0x7A:0x40
        u32Reg = DRV_ReadByte_8BA(0, TX_SLV1, offset);
        p += PROC_PRINT(p, "0x%02x,", u32Reg);
    }
    p += PROC_PRINT(p, "\n");
    p += PROC_PRINT(p, "AUD InfoFrame :\n");
    for(index = 0; index < 9; index ++)
    {
        offset = 0x80 + index;//0x7A:0x80
        u32Reg = DRV_ReadByte_8BA(0, TX_SLV1, offset);
        p += PROC_PRINT(p, "0x%02x,", u32Reg);
    }
    p += PROC_PRINT(p, "\n");
    p += PROC_PRINT(p, "MPg/VendorSpec Inforframe :\n");
    for(index = 0; index < 12; index ++)
    {
        offset = 0xa0 + index;//0x7A:0xA0
        u32Reg = DRV_ReadByte_8BA(0, TX_SLV1, offset);
        p += PROC_PRINT(p, "0x%02x,", u32Reg);
    }
    p += PROC_PRINT(p, "\n");
    p += PROC_PRINT(p, "------------------------------------ Parsed InfoFrame -------------------------------\n");
    p += PROC_PRINT(p, "%-20s: ","Video ID Code(VIC)");
    u32Reg = DRV_ReadByte_8BA(0, TX_SLV1, 0x47);
    p += PROC_PRINT(p, "0x%-18x| ", u32Reg);
    p += PROC_PRINT(p, "%-20s: ","Colorimetry");
    u32Reg = DRV_ReadByte_8BA(0, TX_SLV1, 0x45);
    u32Reg = u32Reg>>6;
    if(!u32Reg)
    {
        p += PROC_PRINT(p, "%s\n", "No Data");
    }
    else if (0x1 == u32Reg)
    {
        p += PROC_PRINT(p, "%s\n", "ITU601");
    }
    else if (0x2 == u32Reg)
    {
        p += PROC_PRINT(p, "%s\n", "ITU709");
    }
    else
    {
        u32Reg = DRV_ReadByte_8BA(0, TX_SLV1, 0x46);
        u32Reg &= 0x70;
        if(u32Reg == 0)
        {
            p += PROC_PRINT(p, "%s\n", "xvYCC601");
        }
        else if(u32Reg == 0x10)
        {
            p += PROC_PRINT(p, "%s\n", "xvYCC709");
        }
        else
        {
            p += PROC_PRINT(p, "%s\n", "reserve");
        }
    }
    p += PROC_PRINT(p, "%-20s: ","Pixel Repetition");
    u32Reg = DRV_ReadByte_8BA(0, TX_SLV1, 0x48);
    u32Reg &= 0x0F;
    if(u32Reg > 10)
    {
        //array overflow, set to reserved
        u32Reg = 10;
    }
    p += PROC_PRINT(p, "%-20s| ", g_pPixelRep[u32Reg]);
    p += PROC_PRINT(p, "%-20s: ","ScanInfo");
    u32Reg = DRV_ReadByte_8BA(0, TX_SLV1, 0x44);
    u32Reg &= 0x03;
    p += PROC_PRINT(p, "%s\n", g_pScanInfo[u32Reg]);

    p += PROC_PRINT(p, "%-20s: ","Output Color Space");
    u32Reg = DRV_ReadByte_8BA(0, TX_SLV1, 0x44);
    u32Reg &= 0x60;
    u32Reg >>=5;
    p += PROC_PRINT(p, "%-20s| ", g_pColorSpace[u32Reg]);
    p += PROC_PRINT(p, "%-20s: ","AspectRatio");
    u32Reg = DRV_ReadByte_8BA(0, TX_SLV1, 0x45);
    u32Reg &= 0x30;
    if(0x00 == u32Reg)
    {
        p += PROC_PRINT(p, "%s\n", "No Data");
    }
    else if (0x10 == u32Reg)
    {
        p += PROC_PRINT(p, "%s\n", "4:3");
    }
    else if (0x20 == u32Reg)
    {
        p += PROC_PRINT(p, "%s\n", "16:9");
    }
    else
    {
        p += PROC_PRINT(p, "%s\n", "Future");
    }
 
    p += PROC_PRINT(p, "---------------------------------------- END ----------------------------------------\n");
  
    return HI_SUCCESS;
}

static HI_S32 HDMI0_Sink_Proc(struct seq_file *p, HI_VOID *v)
{
    HI_UNF_HDMI_SINK_CAPABILITY_S sinkCap;
    HI_U32 u32Index,u32Count;

    memset(&sinkCap,0,sizeof(HI_UNF_HDMI_SINK_CAPABILITY_S));
    p += PROC_PRINT(p, "--------------------------------- Hisi HDMI Sink Capability -------------------------\n");
    
    DRV_HDMI_GetSinkCapability(HI_UNF_HDMI_ID_0, &sinkCap);
    
    p += PROC_PRINT(p, "%-20s: ","EDID Status");
    if(sinkCap.bIsRealEDID)
    {
        p += PROC_PRINT(p, "%-20s| ", "OK");
    }
    else
    {
        p += PROC_PRINT(p, "%-20s| ", "Failed");
    }
    p += PROC_PRINT(p, "%-20s: ","TV Manufacture Name");
    p += PROC_PRINT(p, "%s\n", sinkCap.u8IDManufactureName);

    p += PROC_PRINT(p, "%-22s","");
    p += PROC_PRINT(p, "%-20s| ","");
    p += PROC_PRINT(p, "%-20s: ","ProductCode");
    p += PROC_PRINT(p, "%d\n", sinkCap.u32IDProductCode);

    p += PROC_PRINT(p, "%-20s: ","Hdmi Support");
    if(sinkCap.bIsRealEDID)
    {
        p += PROC_PRINT(p, "%-20s| ", "TRUE");
    }
    else
    {
        p += PROC_PRINT(p, "%-20s| ", "FALSE");
    }
    p += PROC_PRINT(p, "%-20s: ","SerialNumber");
    p += PROC_PRINT(p, "%d\n", sinkCap.u32IDSerialNumber);

    p += PROC_PRINT(p, "%-20s: ","EDID Version");
    p += PROC_PRINT(p, "%d.%-18d| ", sinkCap.u8Version,sinkCap.u8Revision);
    p += PROC_PRINT(p, "%-20s: ","Week Of Manufacture");
    p += PROC_PRINT(p, "%d\n", sinkCap.u32WeekOfManufacture);

    p += PROC_PRINT(p, "%-20s: ","Extend Block Num");
    p += PROC_PRINT(p, "%-20d| ", sinkCap.u8EDIDExternBlockNum);
    p += PROC_PRINT(p, "%-20s: ","Year Of Manufacture");
    p += PROC_PRINT(p, "%d\n", sinkCap.u32YearOfManufacture);

    p += PROC_PRINT(p, "%-20s: ","DVI Dual");
    if(sinkCap.bSupportDVIDual)
    {
        p += PROC_PRINT(p, "%-20s| ", "TRUE");
    }
    else
    {
        p += PROC_PRINT(p, "%-20s| ", "FALSE");
    }
    p += PROC_PRINT(p, "%-20s: ","CEC PhyAddr Valid");
    if(sinkCap.bIsPhyAddrValid)
    {
        p += PROC_PRINT(p, "%s\n", "TRUE");
    }
    else
    {
        p += PROC_PRINT(p, "%s\n", "FALSE");
    }

    p += PROC_PRINT(p, "%-20s: ","Supports AI");
    if(sinkCap.bSupportAI)
    {
        p += PROC_PRINT(p, "%-20s| ", "TRUE");
    }
    else
    {
        p += PROC_PRINT(p, "%-20s| ", "FALSE");
    }
    p += PROC_PRINT(p, "%-20s: ","CEC Phy Add");
    p += PROC_PRINT(p, "%02x.%02x.%02x.%02x\n", sinkCap.u8PhyAddr_A, 
        sinkCap.u8PhyAddr_B, sinkCap.u8PhyAddr_C, sinkCap.u8PhyAddr_D);
    p += PROC_PRINT(p, "-------------------------------------- Video ----------------------------------------\n");
    p += PROC_PRINT(p, "%-20s: ","Video Timing");
    for(u32Index = 0,u32Count = 0; u32Index < HI_UNF_ENC_FMT_BUTT; u32Index++)
    {
        if(sinkCap.bVideoFmtSupported[u32Index])
        {
            p += PROC_PRINT(p, "%s / ", g_pUnfFmtString[u32Index]);
            u32Count++;
            if(0 == u32Count%6)
            {
                p += PROC_PRINT(p, "\n%-22s","");
            }
        }
    }
    p += PROC_PRINT(p, "\n");
    p += PROC_PRINT(p, "%-20s: ","Native Format");
    p += PROC_PRINT(p, "%s\n", g_pUnfFmtString[sinkCap.enNativeVideoFormat]);
    p += PROC_PRINT(p, "%-20s: ","Colorimetry");
    if(sinkCap.bSupportxvYCC601)
    {
        p += PROC_PRINT(p, " / %s", "xvYCC601");
    }
    if(sinkCap.bSupportxvYCC709)
    {
        p += PROC_PRINT(p, " / %s", "xvYCC709");
    }
    p += PROC_PRINT(p, "\n");
    
    p += PROC_PRINT(p, "%-20s: ","Color Space");
    p += PROC_PRINT(p, "%s", "RGB");
    if(sinkCap.bSupportYCbCr)
    {
        p += PROC_PRINT(p, " / %s", "YCbCr");
    }
    p += PROC_PRINT(p, "\n");

    p += PROC_PRINT(p, "%-20s: ","Deep Color");
    p += PROC_PRINT(p, "%s", "24");
    if(sinkCap.bSupportDeepColor30Bit)
    {
        p += PROC_PRINT(p, " / %s", "30");
    }
    if(sinkCap.bSupportDeepColor30Bit)
    {
        p += PROC_PRINT(p, " / %s", "36");
    }
    if(sinkCap.bSupportDeepColor48Bit)
    {
        p += PROC_PRINT(p, " / %s", "48");
    }
    p += PROC_PRINT(p, "bit\n");
    
    p += PROC_PRINT(p, "%-20s: ","3D Support");
    if(sinkCap.bHDMI_Video_Present)
    {
        p += PROC_PRINT(p, "%s\n","Support");
    }
    else
    {
        p += PROC_PRINT(p, "%s\n","Not Support");
    }

    p += PROC_PRINT(p, "%-20s: ","3D Type");
    p += PROC_PRINT(p, "%s\n","None");

    p += PROC_PRINT(p, "-------------------------------------- Audio ----------------------------------------\n");
    p += PROC_PRINT(p, "Audio Fmt support : ");    
    for (u32Index = 0; u32Index < HI_UNF_HDMI_MAX_AUDIO_CAP_COUNT; u32Index++)
    {
        if (sinkCap.bAudioFmtSupported[u32Index] == HI_TRUE)
        {
             switch (u32Index)
            {
            case 1:
                p += PROC_PRINT(p, "PCM ");
                break;
            case 2:
                p += PROC_PRINT(p, "AC3 ");
                break;
            case 3:
                p += PROC_PRINT(p, "MPEG1 ");
                break;
            case 4:
                p += PROC_PRINT(p, "MP3 ");
                break;
            case 5:
                p += PROC_PRINT(p, "MPEG2 ");
                break;
            case 6:
                p += PROC_PRINT(p, "ACC ");
                break;
            case 7:
                p += PROC_PRINT(p, "DTS ");
                break;
            case 8:
                p += PROC_PRINT(p, "ATRAC ");
                break;
            case 9:
                p += PROC_PRINT(p, "OneBitAudio ");
                break;
            case 10:
                p += PROC_PRINT(p, "DD+ ");
                break;
            case 11:
                p += PROC_PRINT(p, "DTS_HD ");
                break;
            case 12:
                p += PROC_PRINT(p, "MAT ");
                break;
            case 13:
                p += PROC_PRINT(p, "DST ");
                break;
            case 14:
                p += PROC_PRINT(p, "WMA ");
                break;
            default:
                p += PROC_PRINT(p, "reserved "); 
                break;
            }
        }
    }
    p += PROC_PRINT(p, "\n");

    p += PROC_PRINT(p, "Max Audio PCM channels: %d\n", sinkCap.u32MaxPcmChannels);
    p += PROC_PRINT(p, "Support Audio Sample Rates:");
    for (u32Index = 0; u32Index < HI_UNF_HDMI_MAX_AUDIO_SMPRATE_COUNT; u32Index++)
    {
        if(sinkCap.u32AudioSampleRateSupported[u32Index] != 0)
        {
            p += PROC_PRINT(p, " %d ", sinkCap.u32AudioSampleRateSupported[u32Index]);
        }
    }
    p += PROC_PRINT(p, "\n");

    if(sinkCap.u8Speaker != 0x00)
    {
        p += PROC_PRINT(p, "\n%-10s : ","Speaker");        

        for(u32Index = 0; u32Index < 8; u32Index++)
        {
            if(sinkCap.u8Speaker & 0x1)
            {
                p += PROC_PRINT(p, "%s ",g_pSpeaker[u32Index]);
            }
            sinkCap.u8Speaker >>= 1;
        }
                
        p += PROC_PRINT(p, "\n");
    }

    p += PROC_PRINT(p, "------------------------------------ EDID Raw Data ---------------------------------- \n");
    if (sinkCap.bIsRealEDID == HI_TRUE)
    {
        HI_U32 index,u32EdidLegth = 0;
        HI_U8  Data[512];

        memset(Data, 0, 512);
        u32EdidLegth = 128*(sinkCap.u8EDIDExternBlockNum + 1);

        if(u32EdidLegth > 512)
        {
            u32EdidLegth = 512;
        }
        
        SI_Proc_ReadEDIDBlock(Data, u32EdidLegth);
        for (index = 0; index < u32EdidLegth; index ++)
        {
            p += PROC_PRINT(p, "%02x ", Data[index]);
            if (0 == ((index + 1) % 16))
            {
                p += PROC_PRINT(p, "\n");
            }
        }
        
    }
    
    p += PROC_PRINT(p, "---------------------------------------- END ----------------------------------------\n");
       
    return HI_SUCCESS;
}

extern HI_U32 unStableTimes;

HI_S32 hdmi_ProcWrite(struct file * file,
    const char __user * buf, size_t count, loff_t *ppos)
{
    //struct seq_file   *p = file->private_data;
    //DRV_PROC_ITEM_S  *pProcItem = s->private;
    HI_CHAR  chCmd[60] = {0};
    HI_CHAR  chArg1[DEF_FILE_NAMELENGTH] = {0};
    HI_CHAR  chArg2[DEF_FILE_NAMELENGTH] = {0};

    
    if(count > 40)
    {   
        printk("Error:Echo too long.\n");
        return HI_FAILURE;
    }
    
    if(copy_from_user(chCmd,buf,count))
    {
        printk("copy from user failed\n");
        return HI_FAILURE;
    }

    hdmi_GetProcArg(chCmd, chArg1, 1);
    hdmi_GetProcArg(chCmd, chArg2, 2);

    //sw reset
    if(!strcmp(chArg1,"swrst"))
    {
        printk("hdmi resetting... ... ... \n");
        SI_SW_ResetHDMITX();
    }
    //invert tmds clock
    else if(!strcmp(chArg1,"tclk"))
    {
        if(chArg2[0] == '1')
        {
            printk("hdmi TmdsClk invert...  \n");
            WriteByteHDMITXP1(0x3d,0x1f);
        }
        else if(chArg2[0] == '0')
        {
            printk("hdmi TmdsClk not invert... \n");
            WriteByteHDMITXP1(0x3d,0x17);
        }
    }
    else if(!strcmp(chArg1,"mute"))
    {
        if(chArg2[0] == '1')
        {
            printk("mute...  \n");
            DRV_HDMI_SetAVMute(0,HI_TRUE);
            //WriteByteHDMITXP1(0x3d,0x1f);
        }
        else if(chArg2[0] == '0')
        {
            printk("unmute... \n");
            DRV_HDMI_SetAVMute(0,HI_FALSE);
            //WriteByteHDMITXP1(0x3d,0x17);
        }
    }
    //else if(chArg1[0] == '3'&& chArg1[1] == 'd')
    else if(!strcmp(chArg1,"3d"))
    {
        HDMI_ATTR_S stAttr;
        DRV_HDMI_GetAttr(0,&stAttr);
        if(chArg2[0] == '0')
        {
            printk("3d mode disable...  \n");
            //HI_DRV_HDMI_Set3DMode(0,HI_FALSE,HI_UNF_3D_MAX_BUTT);
            stAttr.stVideoAttr.b3DEnable = HI_FALSE;
            stAttr.stVideoAttr.u83DParam = HI_UNF_3D_MAX_BUTT;
            //WriteByteHDMITXP1(0x3d,0x1f);
        }
        else if(!strcmp(chArg2,"fp"))
        {
            printk("Frame Packing... \n");
            //HI_DRV_HDMI_Set3DMode(0,HI_TRUE,HI_UNF_3D_FRAME_PACKETING);
            stAttr.stVideoAttr.b3DEnable = HI_TRUE;
            stAttr.stVideoAttr.u83DParam = HI_UNF_3D_FRAME_PACKETING;
            //WriteByteHDMITXP1(0x3d,0x17);
        }
        else if(!strcmp(chArg2,"sbs"))
        {
            printk("Side by side(half)... \n");
            //HI_DRV_HDMI_Set3DMode(0,HI_TRUE,HI_UNF_3D_SIDE_BY_SIDE_HALF);
            stAttr.stVideoAttr.b3DEnable = HI_TRUE;
            stAttr.stVideoAttr.u83DParam = HI_UNF_3D_SIDE_BY_SIDE_HALF;
            //WriteByteHDMITXP1(0x3d,0x17);
        }
        else if(!strcmp(chArg2,"tab"))
        {
            printk("Top and bottom... \n");
            //HI_DRV_HDMI_Set3DMode(0,HI_TRUE,HI_UNF_3D_TOP_AND_BOTTOM);
            stAttr.stVideoAttr.b3DEnable = HI_TRUE;
            stAttr.stVideoAttr.u83DParam = HI_UNF_3D_TOP_AND_BOTTOM;
            //WriteByteHDMITXP1(0x3d,0x17);
        }
        DRV_HDMI_SetAttr(0,&stAttr);
    }
    else if(!strcmp(chArg1,"cbar"))
    {
        HI_U32 u32Reg = 0;
        if(chArg2[0] == '0')
        {
            printk("colorbar disable...  \n");
            DRV_HDMI_ReadRegister(0xf8ccc000,&u32Reg);
            u32Reg = u32Reg & (~0x70000000);
            DRV_HDMI_WriteRegister(0xf8ccc000,(u32Reg | 0x1));
        }
        else if(chArg2[0] == '1')
        {
            printk("colorbar enable.. \n");
            DRV_HDMI_ReadRegister(0xf8ccc000,&u32Reg);
            u32Reg = u32Reg | 0x70000000;
            DRV_HDMI_WriteRegister(0xf8ccc000,(u32Reg | 0x1));
        }
    }
    // 0x00 0xff 0xff yellow
    // 0x00 0xff 0x00 green
    // 0xff 0x00 0x00 blue
    // 0x00 0x00 0xff red
    // 0x80 0x10 0x80 black
    //  white
    else if(!strcmp(chArg1,"vblank"))
    {
        if(chArg2[0] == '0')
        {
            printk("vblank disable...  \n");
            WriteByteHDMITXP0(0x0d,0x00);
        }
        else if(!strcmp(chArg2,"black"))
        {
            printk("vblank black.. \n");
            WriteByteHDMITXP0(0x0d,0x04);
            WriteByteHDMITXP0(0x4b,0x80);
            WriteByteHDMITXP0(0x4c,0x10);
            WriteByteHDMITXP0(0x4d,0x80);
        }
        else if(!strcmp(chArg2,"red"))
        {
            printk("vblank red.. \n");
            WriteByteHDMITXP0(0x0d,0x04);
            WriteByteHDMITXP0(0x4b,0x00);
            WriteByteHDMITXP0(0x4c,0x00);
            WriteByteHDMITXP0(0x4d,0xff);
        }
        else if(!strcmp(chArg2,"green"))
        {
            printk("vblank green.. \n");
            WriteByteHDMITXP0(0x0d,0x04);
            WriteByteHDMITXP0(0x4b,0x00);
            WriteByteHDMITXP0(0x4c,0xff);
            WriteByteHDMITXP0(0x4d,0x00);
        }
        else if(!strcmp(chArg2,"blue"))
        {
            printk("vblank blue.. \n");
            WriteByteHDMITXP0(0x0d,0x04);
            WriteByteHDMITXP0(0x4b,0xff);
            WriteByteHDMITXP0(0x4c,0x00);
            WriteByteHDMITXP0(0x4d,0x00);
        }
    }
    else if(!strcmp(chArg1,"enc"))
    {
        if(!strcmp(chArg2,"phy"))
        {
            printk("encode by phy \n");
            WriteByteHDMITXP1(0x3c,0x08);
            SI_TX_PHY_WriteRegister(0x0d,0x00);
        }
        else if(!strcmp(chArg2,"ctrl"))
        {
            printk("encode by ctrl \n");
            WriteByteHDMITXP1(0x3c,0x00);
            SI_TX_PHY_WriteRegister(0x0d,0x01);
        }
    }
    else if(!strcmp(chArg1,"pclk"))
    {
        if(chArg2[0] == '0')
        {
            printk("pclk nobypass \n");
            WriteByteHDMITXP1(0x3c,0x08);
        }
        else if(chArg2[0] == '1')
        {
            printk("pclk bypass \n");
            WriteByteHDMITXP1(0x3c,0x28);
        }
    }    
    else if(!strcmp(chArg1,"audio"))
    {
        HDMI_AUDIO_ATTR_S stHDMIAOAttr;
        memset((void*)&stHDMIAOAttr, 0, sizeof(HDMI_AUDIO_ATTR_S));
        DRV_HDMI_GetAOAttr(0,&stHDMIAOAttr);
        
        if(chArg2[0] == '0')
        {
            printk("audio I2S \n");
            stHDMIAOAttr.enSoundIntf = HDMI_AUDIO_INTERFACE_I2S;
            DRV_HDMI_AudioChange(0,&stHDMIAOAttr);
        }
        else if(chArg2[0] == '1')
        {
            printk("audio SPDIF \n");
            stHDMIAOAttr.enSoundIntf = HDMI_AUDIO_INTERFACE_SPDIF;
            DRV_HDMI_AudioChange(0,&stHDMIAOAttr);
        }
        else if(chArg2[0] == '2')
        {
            printk("audio HBR \n");
            stHDMIAOAttr.enSoundIntf = HDMI_AUDIO_INTERFACE_HBR;
            DRV_HDMI_AudioChange(0,&stHDMIAOAttr);
        }
        else
        {
            printk("not supported \n");
        }
    } 
    else if(!strcmp(chArg1,"thread"))
    {
        if(chArg2[0] == '0')
        {
            printk("thread stop \n");
            DRV_Set_ThreadStop(HI_TRUE);
        }
        else if(chArg2[0] == '1')
        {
            printk("thread start \n");          
            DRV_Set_ThreadStop(HI_FALSE);
        }
    }
    else if(!strcmp(chArg1,"ceccnt"))
    {
        if(chArg2[0] == '0')
        {
#if defined (CEC_SUPPORT)
            HDMI_CHN_ATTR_S *pstChnAttr = DRV_Get_ChnAttr();
            pstChnAttr[HI_UNF_HDMI_ID_0].u8CECCheckCount = 0;
#else
            printk("do not support cec \n");
#endif

        }
    }
    else if (!strcmp(chArg1,"setAttr"))
    {
        HDMI_ATTR_S *pstHDMIAttr = DRV_Get_HDMIAttr(HI_UNF_HDMI_ID_0);
        DRV_Set_ForceUpdateFlag(HI_UNF_HDMI_ID_0,HI_TRUE);
        DRV_HDMI_SetAttr(HI_UNF_HDMI_ID_0, pstHDMIAttr);
    }
#if 0 /*--NO MODIFY : COMMENT BY CODINGPARTNER--*/
    else if (!strcmp(chArg1,"chkSet"))
    {
        HDMI_ATTR_S *pstHDMIAttr = DRV_Get_HDMIAttr(HI_UNF_HDMI_ID_0);
                  
        HI_INFO_HDMI("--> chkhot plug SetAttr \n");

        while(1)
        {
             HI_U32                 j = 0,RegDate = 0,stable = 0;
            DRV_Set_ForceUpdateFlag(HI_UNF_HDMI_ID_0,HI_TRUE);                   
            SI_SW_ResetHDMITX();
            DelayMS(1000);

            printk(" %d times sw reset \n",setAttrChkCnt);
            setAttrChkCnt++;
            printk("check stable 15s \n"); 
            for (j=0 ;j<10000;j++)   
            {
                DelayMS(1);/*10ms*/
                RegDate= ReadByteHDMITXP0(0x09);
                //printf("check (0x%x)!********\n",RegDate);
                stable = (RegDate & 0x1);

                if (!stable  )
                    break;
            }

            if (!stable )
            {
                printk("*************stable*************(%x)******************\n",RegDate);
                break;
            }  
        }
        
        DRV_HDMI_SetAttr(HI_UNF_HDMI_ID_0, pstHDMIAttr);
    }
#endif /*--NO MODIFY : COMMENT BY CODINGPARTNER--*/
    else if (!strcmp(chArg1,"check"))
    {
        HI_U32 u32Reg = 0;
        if(!strcmp(chArg2,"timing"))
        {
            HI_PRINT("Check timing... \n");
            u32Reg = ReadByteHDMITXP0(TX_STAT_ADDR);

            if((u32Reg & BIT_HDMI_PSTABLE)!=0)
            {
                HI_PRINT("Pixel Clk      : stable \n");            
            }
            else
            {
                HI_PRINT("Pixel Clk      : !!Warnning!! Clock Unstable\n");
            }     
            
            HI_PRINT("Unstable Times : %d \n",unStableTimes);

            u32Reg = ReadByteHDMITXP0(0x3b);
            u32Reg = (u32Reg << 8) | ReadByteHDMITXP0(0x3a);        
            HI_PRINT("H Total        : %d ( 0x%x )\n",u32Reg,u32Reg);

            u32Reg = ReadByteHDMITXP0(0x3d);
            u32Reg = (u32Reg << 8) | ReadByteHDMITXP0(0x3c);
            
            HI_PRINT("V Total        : %d ( 0x%x )\n",u32Reg,u32Reg);

            u32Reg = ReadByteHDMITXP0(INTERLACE_POL_DETECT);
            if((u32Reg & BIT_I_DETECTR)!=0)
            {
                HI_PRINT("InterlaceDetect: interlace\n");
            }
            else
            {
                HI_PRINT("InterlaceDetect: progress\n");
            }

            u32Reg = ReadByteHDMITXP1(DIAG_PD_ADDR);

            HI_PRINT("Power State    : 0x%02x\n",u32Reg);


        }
        else if(!strcmp(chArg2,"ddc"))
        {
            HI_PRINT("check ddc... \n");

            u32Reg = ReadByteHDMITXP0(DDC_DELAY_CNT);
            HI_PRINT("DDC Delay Count : 0x%02x(%d)\n",u32Reg,u32Reg);

            //60Mhz osc clk
            HI_PRINT("DDC Speed in calc : %dHz\n",(60000000 / (u32Reg * 30)));

            u32Reg = ReadByteHDMITXP0(DDC_STATUS);

            if(u32Reg & BIT_MDDC_ST_I2C_LOW)
            {
                HI_PRINT("I2C transaction did not start because I2C bus is pulled LOW by an external device \n");
            }

            if(u32Reg & BIT_MDDC_ST_NO_ACK)
            {
                HI_PRINT("HDMI Transmitter did not receive an ACK from slave device during address or data write \n");
            }
            if(u32Reg & BIT_MDDC_ST_IN_PROGR)
            {
                HI_PRINT("DDC operation in progress \n");
            }

            if(u32Reg & BIT_MDDC_ST_FIFO_FULL)
            {
                HI_PRINT("DDC FIFO Full \n");
            }

            if(u32Reg & BIT_MDDC_ST_FIFO_EMPL)
            {
                HI_PRINT("DDC FIFO Empty \n");
            }

            if(u32Reg & BIT_MDDC_ST_FRD_USE)
            {
                HI_PRINT("DDC FIFO Read In Use \n");
            }

            if(u32Reg & BIT_MDDC_ST_FWT_USE)
            {
                HI_PRINT("DDC FIFO Write In Use \n");
            }
        }
        else if(!strcmp(chArg2,"color"))
        {
            HI_PRINT("not supported... \n");          
        }
        else if(!strcmp(chArg2,"ro"))
        {
            HI_PRINT("not supported... \n");          
        }
        else if(!strcmp(chArg2,"phy"))
        {
            // unknown DM_TX_CTRL2
            SI_TX_PHY_ReadRegister(0x06,&u32Reg);
            if(u32Reg != 0x89)
            {
                HI_ERR_HDMI("Phy Reg 0x06 : 0x%02x(0x89) \n",u32Reg);
            }

            // unknown DM_TX_CTRL3
            SI_TX_PHY_ReadRegister(0x07,&u32Reg);
            if(u32Reg != 0x81)
            {
                HI_ERR_HDMI("Phy Reg 0x07 : 0x%02x(0x81) \n",u32Reg);
            }

            // unknown DM_TX_CTRL5
            SI_TX_PHY_ReadRegister(0x09,&u32Reg);
            if(u32Reg != 0x1a)
            {
                HI_ERR_HDMI("Phy Reg 0x09 : 0x%02x(0x1a) \n",u32Reg);
            }

            // unknown BIAS_GEN_CTRL1 
            SI_TX_PHY_ReadRegister(0x0a,&u32Reg);
            if(u32Reg != 0x07)
            {
                HI_ERR_HDMI("Phy Reg 0x0a : 0x%02x(0x07) \n",u32Reg);
            }

            // unknown BIAS_GEN_CTRL2
            SI_TX_PHY_ReadRegister(0x0b,&u32Reg);
            if(u32Reg != 0x51)
            {
                HI_ERR_HDMI("Phy Reg 0x0b : 0x%02x(0x51) \n",u32Reg);
            }

            //unknown DM_TX_CTRL4 
            SI_TX_PHY_ReadRegister(0x08,&u32Reg);
            if(u32Reg != 0x40)
            {
                HI_ERR_HDMI("Phy Reg 0x08 : 0x%02x(0x40) \n",u32Reg);
            }

            // enc_bypass == nobypass
            SI_TX_PHY_ReadRegister(0x0d,&u32Reg);
            if(u32Reg != 0x00)
            {
                HI_ERR_HDMI("Phy Reg 0x0d : 0x%02x(0x00) \n",u32Reg);
            }

                     
            // term_en && cap_ctl  // term_en 先关掉
            SI_TX_PHY_ReadRegister(0x0e,&u32Reg);
            if(u32Reg & 0x01)
            {
                HI_PRINT("term_en : Enable  \n");
            }
            else
            {
                HI_PRINT("term_en : Disable  \n");
            }
            
            if(u32Reg & 0x02)
            {
                HI_PRINT("cap_ctl : Enable (recommond to disable)  \n");
            }
            else
            {
                HI_PRINT("cap_ctl : Disable  \n");
            }

            // pll ctrl -deep color
            SI_TX_PHY_ReadRegister(0x02,&u32Reg);
            if((u32Reg & 0x03) == 0x00)
            {
                HI_PRINT("dpcolor_ctl  : 8bit  \n");
            }            
            else if((u32Reg & 0x03) == 0x01)
            {
                HI_PRINT("dpcolor_ctl  : 10bit  \n");
            }            
            else if((u32Reg & 0x03) == 0x02)
            {
                HI_PRINT("dpcolor_ctl  : 12bit  \n");
            }
            else
            {
                HI_PRINT("dpcolor_ctl  : invalid  \n");
            }

            // oe && pwr_down
            SI_TX_PHY_ReadRegister(0x05,&u32Reg);
            if(u32Reg & 0x10)
            {
                HI_PRINT("Phy No power Down \n");
            }
            else
            {
                HI_PRINT("Phy will be power down \n");
            }

            if(u32Reg & 0x20)
            {
                HI_PRINT("Phy Outputs enable\n");
            }
            else
            {
                HI_PRINT("Phy Outputs Disable\n");
            }         

            SI_TX_PHY_ReadRegister(0x0c,&u32Reg);
            if(u32Reg & 0x01)
            {
                HI_PRINT("receiver is connected \n");
            }
            else
            {
                HI_PRINT("receiver is dis-connected \n");
            }

            if(u32Reg & 0x02)
            {
                HI_PRINT("Clock detected > 2.5Mhz \n");
            }
            else
            {
                HI_PRINT("No clock detected \n");
            }      
            
            
        }
    }

    else 
    {
        HI_PRINT("--------------------------------- HDMI debug options --------------------------------\n");                                                     
        HI_PRINT("you can perform HDMI debug with such commond:\n");                                                                      
        HI_PRINT("echo [arg1] [arg2] > /proc/msp/hdmi \n\n");                                                                             
        HI_PRINT("debug action                      arg1         arg2\n");                                                                
        HI_PRINT("------------------------------    --------    ---------------------------------------\n");                                                
        HI_PRINT("colorbar                          cbar        0 disable / 1 enable \n");
        HI_PRINT("vblank(yuv data from hdmi)        vblank      0 /red / green/ blue/ black \n");
        HI_PRINT("DVI encoder                       enc         phy(default)/ctrl  \n");
        HI_PRINT("pixel clk bypass                  pclk        0 nobypass(default) 1 bypass  \n");
        HI_PRINT("software reset                    swrst       no param \n"); 
        HI_PRINT("invert Tmds clk                   tclk        0 not invert(default) / 1 invert \n");  
        HI_PRINT("Avmute                            mute        0 unmute/ 1 mute \n");
        HI_PRINT("Set 3D Fmt                        3d          0 disable3D /fp/sbs/tab  \n");
        HI_PRINT("Debug audio Change                audio       0 I2S /1 SPdif /2 HBR   \n");
        HI_PRINT("Thread stop/start                 thread      0 stop /1 start  \n");
        HI_PRINT("cec count                         ceccnt      0 clear cec count \n");
        HI_PRINT("Force set attr                    setAttr     no param \n"); 
        HI_PRINT("check                             check       timing / ddc / color /ro (read only regs) / phy\n"); 
        HI_PRINT("-------------------------------------------------------------------------------------\n"); 
    }
    return count;
}


HI_S32 HDMI_ModeInit(HI_VOID)
{
    DRV_PROC_ITEM_S  *pProcItem;
    #if 1
    DRV_PROC_EX_S stFnOpt =
    {
         .fnRead = HDMI0_Proc,
    };
#if 0 /*--NO MODIFY : COMMENT BY CODINGPARTNER--*/
    DRV_PROC_EX_S stFnOpt =
    {
         .fnRead = hdmi_Proc,
    };
#endif /*--NO MODIFY : COMMENT BY CODINGPARTNER--*/

    DRV_PROC_EX_S stFnSinkOpt =
    {
         .fnRead = HDMI0_Sink_Proc,
    };
    #else
    DRV_PROC_EX_S stFnOpt;
    stFnOpt.fnRead = HDMI0_Proc;
    #endif

    /* Register hdmi device */
    //sprintf(g_hdmiRegisterData.devfs_name, UMAP_DEVNAME_HDMI);
    HI_OSAL_Snprintf(g_hdmiRegisterData.devfs_name, 64, UMAP_DEVNAME_HDMI);
    g_hdmiRegisterData.fops   = &hdmi_FOPS;
    g_hdmiRegisterData.drvops = &hdmi_DRVOPS;
    g_hdmiRegisterData.minor  = UMAP_MIN_MINOR_HDMI;
    g_hdmiRegisterData.owner  = THIS_MODULE;
    if (HI_DRV_DEV_Register(&g_hdmiRegisterData) < 0)
    {
        HI_FATAL_HDMI("register hdmi failed.\n");
        return HI_FAILURE;
    }
    /* Register Proc hdmi Status */
    pProcItem = HI_DRV_PROC_AddModule("hdmi0", &stFnOpt, NULL);
    if(pProcItem != HI_NULL)
    {
        pProcItem->write = hdmi_ProcWrite;
    }

    //pProcItem = HI_DRV_PROC_AddModule("hdmi0_sink", &stFnSinkOpt, NULL);
    HI_DRV_PROC_AddModule("hdmi0_sink", &stFnSinkOpt, NULL);
    return HI_SUCCESS;
}

extern HI_S32  HDMI_DRV_Init(HI_VOID);

#if 0 /*--NO MODIFY : COMMENT BY CODINGPARTNER--*/
extern HI_S32  HI_DRV_HDMI_Init(HI_VOID);
extern HI_S32  HI_DRV_HDMI_Open(HI_UNF_HDMI_ID_E enHdmi);
extern HI_U32  HI_DRV_HDMI_DeInit(HI_U32 FromUserSpace);
extern HI_U32  HI_DRV_HDMI_Close(HI_UNF_HDMI_ID_E enHdmi);
#endif /*--NO MODIFY : COMMENT BY CODINGPARTNER--*/




HI_VOID HDMI_ModeExit(HI_VOID)
{
    /* Unregister hdmi device */
    HI_DRV_PROC_RemoveModule("hdmi0");
    HI_DRV_PROC_RemoveModule("hdmi0_sink");
    HI_DRV_DEV_UnRegister(&g_hdmiRegisterData);
    return;
}

HI_S32 DRV_HDMI_Register(HI_VOID)
{
	HI_S32 ret;
	ret = HI_DRV_MODULE_Register((HI_U32)HI_ID_HDMI,HDMI_NAME,(HI_VOID *)&s_stHdmiExportFuncs);
    if (HI_SUCCESS != ret)
    {
        HI_FATAL_HDMI("HI_DRV_MODULE_Register failed\n");
        return ret;
    }
	return HI_SUCCESS;
}
HI_S32 DRV_HDMI_UnRegister(HI_VOID)
{
	HI_S32 ret;
	ret = HI_DRV_MODULE_UnRegister((HI_U32)HI_ID_HDMI);
    if (HI_SUCCESS != ret)
    {        
        HI_FATAL_HDMI("HI_DRV_MODULE_UnRegister failed\n");
        return ret;
    }
	return HI_SUCCESS;
}
int HDMI_DRV_ModInit(void)
{
    HI_S32 ret;
    
#if defined (SUPPORT_FPGA)
    SocHdmiInit();
#endif
    HDMI_ModeInit();

    ret = HDMI_DRV_Init();


    //ret = DRV_HDMI_Register();
    
#ifdef ANDROID_SUPPORT
    //android 特有
	if (switch_dev_register(&hdmi_tx_sdev))
    {
		HI_WARN_HDMI("\n Warning:! registering HDMI switch device Failed \n");		
		g_switchOk = HI_FALSE;
        //return -EINVAL;
	}
    else
    {
        g_switchOk = HI_TRUE;
    }
#endif

#ifdef MODULE
	HI_PRINT("Load hi_hdmi.ko success.\t(%s)\n", VERSION_STRING);
#endif

    return ret;
}

extern HI_VOID  HDMI_DRV_EXIT(HI_VOID);
void HDMI_DRV_ModExit(void)
{
    HI_U32 hdmiStatus;
    hdmiStatus = DRV_HDMI_InitNum(HI_UNF_HDMI_ID_0);
    if(hdmiStatus > 0)
    {
        HI_DRV_HDMI_Close(HI_UNF_HDMI_ID_0);
    }

    HDMI_ModeExit();
         
    HDMI_DRV_EXIT();
    //HI_DRV_HDMI_PlayStus(HI_UNF_HDMI_ID_0,&temp);
    //if(temp == HI_TRUE)
    //{
        //HI_DRV_HDMI_Close(HI_UNF_HDMI_ID_0);
    //}
#ifdef ANDROID_SUPPORT
    //android 特有
    if(g_switchOk == HI_TRUE)
    {
    	switch_dev_unregister(&hdmi_tx_sdev);
    }
#endif

#ifdef MODULE
    HI_PRINT("remove hi_hdmi.ko success.\t(%s)\n", VERSION_STRING);
#endif

    return;
}

#ifdef MODULE
module_init(HDMI_DRV_ModInit);
module_exit(HDMI_DRV_ModExit);
#endif
MODULE_LICENSE("GPL");




