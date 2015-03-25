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


//Panasonic_TH-L32CH3C
HDMI_DELAY_TIME_S MEI_TH_L32CH3C =
{
    "MEI",13481,800,"MEI_TH_L32CH3C",
};

//Skyworth 47E760A
HDMI_DELAY_TIME_S SKY_47E760A =
{
    "SKY",19833,800,"SKY_47E760A",
};

HDMI_DELAY_TIME_S *WholeList[] =
{
    &MEI_TH_L32CH3C,&SKY_47E760A,NULL
};

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

