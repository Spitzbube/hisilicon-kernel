#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include "hi_type.h"
#include "hi_drv_stat.h"
#include "drv_stat_ioctl.h"

#ifdef __cplusplus
extern "C"
{
#endif /* End of #ifdef __cplusplus */

static  STAT_Event_Fun   stat_event_fun = NULL;

HI_S32 HI_DRV_STAT_EventFunc_Register(HI_VOID* pFunc)
{
    if (NULL == pFunc)
    {
        return HI_FAILURE;
    }
	stat_event_fun = (STAT_Event_Fun)pFunc;
	return HI_SUCCESS;
}

HI_VOID HI_DRV_STAT_EventFunc_UnRegister(HI_VOID)
{
	stat_event_fun = NULL;
	return;
}

HI_VOID HI_DRV_STAT_Event(STAT_EVENT_E enEvent, HI_U32 Value)
{
	if(stat_event_fun){
		stat_event_fun(enEvent, Value);
	}
	return;
}

HI_U32 HI_DRV_STAT_GetTick(HI_VOID)
{
    HI_U64 SysTime;

    SysTime = sched_clock();
    do_div(SysTime, 1000000);
    return (HI_U32)SysTime;
}


HI_S32 HI_DRV_STAT_KInit(void)
{
	stat_event_fun = NULL;
	return HI_SUCCESS;
}

HI_VOID HI_DRV_STAT_KExit(void)
{
	stat_event_fun = NULL;
    return ;
}

#ifndef MODULE
EXPORT_SYMBOL(HI_DRV_STAT_EventFunc_Register);
EXPORT_SYMBOL(HI_DRV_STAT_EventFunc_UnRegister);
#endif
EXPORT_SYMBOL(HI_DRV_STAT_Event);
EXPORT_SYMBOL(HI_DRV_STAT_GetTick);


#ifdef __cplusplus
}
#endif /* End of #ifdef __cplusplus */

