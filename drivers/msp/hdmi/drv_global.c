
#include <linux/kernel.h>
//include memset
#include <linux/interrupt.h>

#include "drv_global.h"
#include "drv_disp_ext.h"

#include "hi_unf_audio.h"
#include "hi_unf_hdmi.h"
#include "hi_drv_hdmi.h"
#include "hi_drv_module.h"


//#include "si_timer.h"

static HDMI_COMM_ATTR_S g_stHdmiCommParam;

static HDMI_CHN_ATTR_S  g_stHdmiChnParam[HI_UNF_HDMI_ID_BUTT];


#if 0 /*--NO MODIFY : COMMENT BY CODINGPARTNER--*/
void DRV_Get_CommAttr(HDMI_COMM_ATTR_S **pstCommAttr)
{
    printk("Get_CommAttr \n");
    //printk("g_stHdmiCommParam : 0x%x \n",&g_stHdmiCommParam);
    *pstCommAttr = &g_stHdmiCommParam;
#if 0 /*--NO MODIFY : COMMENT BY CODINGPARTNER--*/
    printk("pstCommAttr : 0x%x \n",pstCommAttr);
    printk("&pstCommAttr : 0x%x \n",&pstCommAttr);
    printk("*pstCommAttr : 0x%x \n",*pstCommAttr);
#endif /*--NO MODIFY : COMMENT BY CODINGPARTNER--*/
}
#endif /*--NO MODIFY : COMMENT BY CODINGPARTNER--*/

HDMI_COMM_ATTR_S *DRV_Get_CommAttr()
{
    HI_INFO_HDMI("Get_CommAttr \n");
    return &g_stHdmiCommParam;
}


void DRV_PrintCommAttr()
{
    printk("g_stHdmiCommParam \n"
        "bOpenGreenChannel :%d \n"
        "bOpenedInBoot :%d \n"
        "enDefaultMode :%d \n"
        "kThreadTimerStop :%d \n"
        "enVidInMode :%d \n",
        g_stHdmiCommParam.bOpenGreenChannel,
        g_stHdmiCommParam.bOpenedInBoot,
        g_stHdmiCommParam.enDefaultMode,
        g_stHdmiCommParam.kThreadTimerStop,
        g_stHdmiCommParam.enVidInMode);
}

HI_S32 DRV_Get_IsGreenChannel(HI_VOID)
{
    return g_stHdmiCommParam.bOpenGreenChannel;
}

void DRV_Set_GreenChannel(HI_BOOL bGreen)
{
    HI_INFO_HDMI("Set_GreenChannel bGreen : %d \n",bGreen);
    g_stHdmiCommParam.bOpenGreenChannel = bGreen;
}

HI_S32 DRV_Get_IsOpenedInBoot(HI_VOID)
{
    return g_stHdmiCommParam.bOpenedInBoot;
}

void DRV_Set_OpenedInBoot(HI_BOOL bOpened)
{
    HI_INFO_HDMI("Set_OpenedInBoot bOpend : %d \n",bOpened);
    g_stHdmiCommParam.bOpenedInBoot = bOpened;
}

HI_S32 DRV_Get_DefHDMIMode(HI_VOID)
{
    return g_stHdmiCommParam.enDefaultMode; 
}

void DRV_Set_DefHDMIMode(HI_S32 defMod)
{
    HI_INFO_HDMI("Set_DefHDMIMode defMod : %d \n",defMod);
    g_stHdmiCommParam.enDefaultMode = defMod;
}


HI_S32 DRV_Get_IsThreadStoped(HI_VOID)
{
    return g_stHdmiCommParam.kThreadTimerStop; 
}

void DRV_Set_ThreadStop(HI_BOOL bStop)
{
    HI_INFO_HDMI("Set_ThreadStatus bStop : %d \n",bStop);
    g_stHdmiCommParam.kThreadTimerStop = bStop;
}

HI_UNF_HDMI_VIDEO_MODE_E DRV_Get_VIDMode(HI_VOID)
{
    return g_stHdmiCommParam.enVidInMode;
}

void DRV_Set_VIDMode(HI_UNF_HDMI_VIDEO_MODE_E enVInMode)
{
    g_stHdmiCommParam.enVidInMode = enVInMode;
}



HDMI_CHN_ATTR_S *DRV_Get_ChnAttr()
{
    return g_stHdmiChnParam;
}

HDMI_ATTR_S *DRV_Get_HDMIAttr(HI_UNF_HDMI_ID_E enHdmi)
{
    return &g_stHdmiChnParam[enHdmi].stHDMIAttr;
}

HI_U32 DRV_HDMI_SetDefaultAttr(HI_VOID)
{
    HI_DRV_DISP_FMT_E   enEncFmt = HI_DRV_DISP_FMT_1080i_50;
    HDMI_VIDEO_ATTR_S   *pstVidAttr = DRV_Get_VideoAttr(HI_UNF_HDMI_ID_0);
    HDMI_AUDIO_ATTR_S   *pstAudAttr = DRV_Get_AudioAttr(HI_UNF_HDMI_ID_0);
    HDMI_APP_ATTR_S     *pstAppAttr = DRV_Get_AppAttr(HI_UNF_HDMI_ID_0);   
    DISP_EXPORT_FUNC_S  *disp_func_ops = HI_NULL;
    HI_S32              ret = HI_SUCCESS;

    memset(pstVidAttr,0,sizeof(HDMI_VIDEO_ATTR_S));
    memset(pstAudAttr,0,sizeof(HDMI_AUDIO_ATTR_S));
    memset(pstAppAttr,0,sizeof(HDMI_APP_ATTR_S));
    
    ret = HI_DRV_MODULE_GetFunction(HI_ID_DISP, (HI_VOID**)&disp_func_ops);
    if((NULL == disp_func_ops) || (ret != HI_SUCCESS))
    {
        HI_FATAL_HDMI("can't get disp funcs!\n");
        //return HI_FAILURE;
    }

    // GetFormat need add 3d mode 
    if(disp_func_ops && disp_func_ops->pfnDispGetFormat)
    {
        disp_func_ops->pfnDispGetFormat(HI_DRV_DISPLAY_1, &enEncFmt);
    }
    
    // if don't get disp fmt,then use default fmt
    pstVidAttr->enVideoFmt = enEncFmt;
    if (enEncFmt < HI_DRV_DISP_FMT_861D_640X480_60)
    {
        pstAppAttr->enVidOutMode = HI_UNF_HDMI_VIDEO_MODE_YCBCR444;
    }
    else
    {
        pstAppAttr->enVidOutMode = HI_UNF_HDMI_VIDEO_MODE_RGB444;
    }

    pstVidAttr->b3DEnable = HI_FALSE;
    pstVidAttr->u83DParam = HI_UNF_3D_MAX_BUTT;
    
    pstAudAttr->enBitDepth = HI_UNF_BIT_DEPTH_16;
    pstAudAttr->enSoundIntf = HDMI_AUDIO_INTERFACE_I2S;
    pstAudAttr->enSampleRate = HI_UNF_SAMPLE_RATE_48K;
    pstAudAttr->u32Channels = 2;
    pstAudAttr->bIsMultiChannel = HI_FALSE;
    
    return HI_SUCCESS;
}

HDMI_APP_ATTR_S   *DRV_Get_AppAttr(HI_UNF_HDMI_ID_E enHdmi)
{
    return &g_stHdmiChnParam[enHdmi].stHDMIAttr.stAppAttr;
}

HDMI_VIDEO_ATTR_S *DRV_Get_VideoAttr(HI_UNF_HDMI_ID_E enHdmi)
{
    return &g_stHdmiChnParam[enHdmi].stHDMIAttr.stVideoAttr;
}

HDMI_AUDIO_ATTR_S *DRV_Get_AudioAttr(HI_UNF_HDMI_ID_E enHdmi)
{
    return &g_stHdmiChnParam[enHdmi].stHDMIAttr.stAudioAttr;
}

HI_BOOL DRV_Get_IsNeedForceUpdate(HI_UNF_HDMI_ID_E enHdmi)
{
    return g_stHdmiChnParam[enHdmi].ForceUpdateFlag;
}


void DRV_Set_ForceUpdateFlag(HI_UNF_HDMI_ID_E enHdmi,HI_BOOL bupdate)
{
    g_stHdmiChnParam[enHdmi].ForceUpdateFlag = bupdate;
}

HI_BOOL DRV_Get_IsNeedPartUpdate(HI_UNF_HDMI_ID_E enHdmi)
{
    return g_stHdmiChnParam[enHdmi].partUpdateFlag;
}

void DRV_Set_PartUpdateFlag(HI_UNF_HDMI_ID_E enHdmi,HI_BOOL bupdate)
{
    g_stHdmiChnParam[enHdmi].partUpdateFlag = bupdate;
}



HDMI_PROC_EVENT_S *DRV_Get_EventList(HI_UNF_HDMI_ID_E enHdmi)
{
    //eventlist is not associateed to channel,so force return  HI_UNF_HDMI_ID_0
    return g_stHdmiChnParam[HI_UNF_HDMI_ID_0].eventList;
}

HI_UNF_HDMI_CEC_STATUS_S *DRV_Get_CecStatus(HI_UNF_HDMI_ID_E enHdmi)
{
    return &g_stHdmiChnParam[enHdmi].stCECStatus;
}

HI_UNF_HDMI_AVI_INFOFRAME_VER2_S *DRV_Get_AviInfoFrm(HI_UNF_HDMI_ID_E enHdmi)
{
    return &g_stHdmiChnParam[enHdmi].stAVIInfoFrame;
}

HI_UNF_HDMI_AUD_INFOFRAME_VER1_S *DRV_Get_AudInfoFrm(HI_UNF_HDMI_ID_E enHdmi)
{
    return &g_stHdmiChnParam[enHdmi].stAUDInfoFrame;
}

HI_BOOL DRV_Get_IsChnOpened(HI_UNF_HDMI_ID_E enHdmi)
{
    return g_stHdmiChnParam[enHdmi].bOpen;
}

void DRV_Set_ChnOpen(HI_UNF_HDMI_ID_E enHdmi,HI_BOOL bChnOpen)
{
    g_stHdmiChnParam[enHdmi].bOpen = bChnOpen;
}

HI_BOOL DRV_Get_IsChnStart(HI_UNF_HDMI_ID_E enHdmi)
{
    return g_stHdmiChnParam[enHdmi].bStart;
}

void DRV_Set_ChnStart(HI_UNF_HDMI_ID_E enHdmi,HI_BOOL bChnStart)
{
    g_stHdmiChnParam[enHdmi].bStart = bChnStart;
}

HI_BOOL DRV_Get_IsCECStart(HI_UNF_HDMI_ID_E enHdmi)
{
    return g_stHdmiChnParam[enHdmi].bCECStart;
}

void DRV_Set_CECStart(HI_UNF_HDMI_ID_E enHdmi,HI_BOOL bCecStart)
{
    g_stHdmiChnParam[enHdmi].bCECStart = bCecStart;
}

