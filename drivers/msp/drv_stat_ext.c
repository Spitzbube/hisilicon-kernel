/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : stat.c
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2008/12/16
  Last Modified :
  Description   : interrupt and thread timing measurement tool
  Function List :
  History       :
  1.Date        : 2008/12/16
    Author      : z42136
    Modification: Created file

******************************************************************************/
#include <linux/dma-mapping.h>
#include <linux/sched.h>
#include "hi_type.h"
#include "hi_debug.h"
#include "hi_osal.h"
#include "hi_drv_dev.h"
#include "hi_drv_proc.h"
#include "hi_drv_stat.h"
#include "drv_stat_ioctl.h"


typedef char THREAD_NAME[64];

/* for userspace stat */
typedef struct
{
    HI_BOOL bUsed;
    HI_U32 min_time;    // us
    HI_U32 max_time;    // us
    HI_U32 avg_time;    // us

    THREAD_NAME name;
}STAT_USERSPACE_S;


typedef struct
{
    struct timeval tv;
    HI_U32 time;
}STAT_ISR_TIME_S;

typedef struct
{
    HI_U32 EventMs;
    HI_U32 Value1;
    HI_U32 Value2;
}STAT_EVENT_TIME_S;


#define STAT_MAX_THREAD_USERSPACE   32
/* make sure STAT_USERSPACE_TOTAL_SIZE < 4096 */
#define STAT_USERSPACE_TOTAL_SIZE (sizeof(STAT_USERSPACE_S)*STAT_MAX_THREAD_USERSPACE)


static STAT_ISR_TIME_S g_IsrTime[STAT_ISR_BUTT];
static HI_BOOL g_stat_isr_enable = HI_FALSE;
static HI_BOOL g_stat_thread_enable = HI_TRUE;

static STAT_EVENT_TIME_S g_EventTime[STAT_EVENT_BUTT];


static UMAP_DEVICE_S    g_srtuStatDev;

static HI_U32 g_stat_thread_phyaddr_base = 0;

/* g_stat_userspace point to STAT_MAX_THREAD_USERSPACE * STAT_USERSPACE_S */
static STAT_USERSPACE_S * g_stat_thread_kvirt_base = NULL;

static HI_U64 DIV64(HI_U64 Dividend, HI_U64 Divisor)
{
    do_div(Dividend, Divisor);
    
    return Dividend;
}

static HI_S32 CMPI_STAT_Ioctl(struct inode *inode,
                            struct file *file,
                            HI_U32 cmd,
                            HI_VOID * arg)
{
    HI_U32 tmp;
    int i;

       switch(cmd)
       {
            case UMAPC_CMPI_STAT_REGISTER:
            {
                HI_U32 * p_stat_thread_addr = (HI_U32 * )arg;

                for(i=0; i<STAT_MAX_THREAD_USERSPACE; i++)
                {
                    if(g_stat_thread_kvirt_base[i].bUsed == HI_FALSE)
                    {
                        tmp = g_stat_thread_phyaddr_base + i*sizeof(STAT_USERSPACE_S);
                        HI_WARN_STAT("get free slab(%d)\n", i); //107
                        HI_WARN_STAT("g_stat_thread_phyaddr_base = %x, tmp = %x\n", g_stat_thread_phyaddr_base, tmp); //108

                        *p_stat_thread_addr = tmp;
                        g_stat_thread_kvirt_base[i].bUsed = HI_TRUE;

                        return 0;
                    }
                }

                HI_ERR_STAT("no available stat resource\n"); //117
                /* no available stat resource */
                *p_stat_thread_addr = 0;
                return -1;
            }

            case UMAPC_CMPI_STAT_RESETALL:
            {
                for(i=0; i<STAT_MAX_THREAD_USERSPACE; i++)
                {
                    if(g_stat_thread_kvirt_base[i].bUsed == HI_TRUE)
                    {
                        g_stat_thread_kvirt_base[i].avg_time = 0;
                        g_stat_thread_kvirt_base[i].min_time = 0;
                        g_stat_thread_kvirt_base[i].max_time = 0;
                    }
                }

                break;
            }

            case UMAPC_CMPI_STAT_EVENT:
            {
                STAT_EVENT_S   *pStatEvent;

                pStatEvent = (STAT_EVENT_S *)arg;

                HI_DRV_STAT_Event(pStatEvent->enEvent, pStatEvent->Value);

                break;
            }

            case UMAPC_CMPI_STAT_GETTICK:
            {
                HI_U32 *pTick = (HI_U32*)arg;
                *pTick = HI_DRV_STAT_GetTick();
                break;
            }

            default:
                return -1;
       }

    return 0;
}

static long DRV_STAT_Ioctl(struct file *file,
                           HI_U32 cmd,
                           unsigned long arg)
{
    return (long)HI_DRV_UserCopy(file->f_dentry->d_inode, file, cmd, arg, CMPI_STAT_Ioctl);
}

static HI_S32 DRV_STAT_Release(struct inode * inode, struct file * file)
{
    return 0;
}

static HI_S32 DRV_STAT_Open(struct inode * inode, struct file * file)
{
    return 0;
}

static struct file_operations DRV_stat_Fops=
{
    .owner      = THIS_MODULE,
    .open       = DRV_STAT_Open,
	.unlocked_ioctl      = DRV_STAT_Ioctl,
    .release    = DRV_STAT_Release,
};

static HI_S32 stat_proc_read(struct seq_file *s, HI_VOID *pArg)
{
    int i;

    PROC_PRINT(s, "----------- host isr stat -----------\n");
    if(g_stat_isr_enable == HI_FALSE)
    {
        PROC_PRINT(s, "isr stat is disabled!\n");
    }
    else
    {
        PROC_PRINT(s, "audio isr time = %u us\n", g_IsrTime[STAT_ISR_AUDIO].time);
        PROC_PRINT(s, "video isr time = %u us\n", g_IsrTime[STAT_ISR_VIDEO].time);
        PROC_PRINT(s, "demux isr time = %u us\n", g_IsrTime[STAT_ISR_DEMUX].time);
        PROC_PRINT(s, "sync isr time  = %u us\n", g_IsrTime[STAT_ISR_SYNC].time);
        PROC_PRINT(s, "vo isr time    = %u us\n", g_IsrTime[STAT_ISR_VO].time);
        PROC_PRINT(s, "tde isr time   = %u us\n", g_IsrTime[STAT_ISR_TDE].time);
    }

    PROC_PRINT(s, "----------- host thread stat ----------\n");
    if(g_stat_thread_enable == HI_FALSE)
    {
        PROC_PRINT(s, "thread stat is disabled!\n");
    }
    else
    {
        for(i=0; i<STAT_MAX_THREAD_USERSPACE; i++)
        {
            if(g_stat_thread_kvirt_base[i].bUsed == HI_TRUE)
            {
                PROC_PRINT(s, "thread(%s) min=%08dus, avg=%08dus, max=%08d\n",
                                g_stat_thread_kvirt_base[i].name,
                                g_stat_thread_kvirt_base[i].min_time,
                                g_stat_thread_kvirt_base[i].avg_time,
                                g_stat_thread_kvirt_base[i].max_time);
            }
        }
    }

    PROC_PRINT(s, "----------- host event stat ----------\n");
    
    PROC_PRINT(s, "KEYIN          = %-10u (keyvalue 0x%x)\n", g_EventTime[STAT_EVENT_KEYIN].EventMs, g_EventTime[STAT_EVENT_KEYIN].Value1);
    PROC_PRINT(s, "KEYOUT         = %-10u (keyvalue 0x%x)\n", g_EventTime[STAT_EVENT_KEYOUT].EventMs, g_EventTime[STAT_EVENT_KEYOUT].Value1);
    PROC_PRINT(s, "ASTOP          = %-10u\n", g_EventTime[STAT_EVENT_ASTOP].EventMs);
    PROC_PRINT(s, "VSTOP          = %-10u\n", g_EventTime[STAT_EVENT_VSTOP].EventMs);
    PROC_PRINT(s, "CONNECT        = %-10u\n", g_EventTime[STAT_EVENT_CONNECT].EventMs);
    PROC_PRINT(s, "LOCKED         = %-10u\n", g_EventTime[STAT_EVENT_LOCKED].EventMs);
    PROC_PRINT(s, "ASTART         = %-10u\n", g_EventTime[STAT_EVENT_ASTART].EventMs);
    PROC_PRINT(s, "VSTART         = %-10u\n", g_EventTime[STAT_EVENT_VSTART].EventMs);
    PROC_PRINT(s, "CWSET          = %-10u\n", g_EventTime[STAT_EVENT_CWSET].EventMs);
    PROC_PRINT(s, "STREAMIN       = %-10u\n", g_EventTime[STAT_EVENT_STREAMIN].EventMs);
    PROC_PRINT(s, "ISTREAMGET     = %-10u (size %d)\n", g_EventTime[STAT_EVENT_ISTREAMGET].EventMs,g_EventTime[STAT_EVENT_ISTREAMGET].Value1);
    PROC_PRINT(s, "FRAMEDECED     = %-10u\n", g_EventTime[STAT_EVENT_IFRAMEOUT].EventMs);
    
	PROC_PRINT(s, "VPSSGET        = %-10u\n", g_EventTime[STAT_EVENT_VPSSGETFRM].EventMs);
    PROC_PRINT(s, "VPSSOUT        = %-10u\n", g_EventTime[STAT_EVENT_VPSSOUTFRM].EventMs);
    PROC_PRINT(s, "AVPLAYGET      = %-10u\n", g_EventTime[STAT_EVENT_AVPLAYGETFRM].EventMs);
    
	PROC_PRINT(s, "PRESYNC        = %-10u\n", g_EventTime[STAT_EVENT_PRESYNC].EventMs);
    PROC_PRINT(s, "BUFREADY       = %-10u (type %d)\n", g_EventTime[STAT_EVENT_BUFREADY].EventMs,g_EventTime[STAT_EVENT_BUFREADY].Value1);
    PROC_PRINT(s, "FRAMESYNCOK    = %-10u\n", g_EventTime[STAT_EVENT_FRAMESYNCOK].EventMs);

    PROC_PRINT(s, "VOGET          = %-10u\n", g_EventTime[STAT_EVENT_VOGETFRM].EventMs);
        
    PROC_PRINT(s, "IFRAMEINTER    = %-10u\n", g_EventTime[STAT_EVENT_IFRAMEINTER].Value1);
    PROC_PRINT(s, "TOTAL          = %-10u\n", g_EventTime[STAT_EVENT_VOGETFRM].EventMs - g_EventTime[STAT_EVENT_KEYIN].EventMs);

    return 0;
}

#if 0
static HI_S32 stat_proc_write(struct file * file,
    const char __user * buf, size_t count, loff_t *ppos)
{
    return 0;
}
#endif

static HI_S32 func_80539d84(struct seq_file *s, HI_VOID *pArg)
{
#warning func_80539d84: TODO
	printk("func_80539d84: TODO\n");
}

HI_VOID stat_event_fun(STAT_EVENT_E enEvent, HI_U32 Value);

HI_S32 HI_DRV_STAT_Init(HI_VOID)
{
    DRV_PROC_ITEM_S *item ;
    DRV_PROC_ITEM_S *item2 ;
    int i;

    memset((void *)g_IsrTime, 0, sizeof(g_IsrTime));

    g_stat_thread_kvirt_base =  dma_alloc_coherent(NULL, STAT_USERSPACE_TOTAL_SIZE, &g_stat_thread_phyaddr_base, GFP_DMA);
    if (NULL == g_stat_thread_kvirt_base)
    {
        return -1;
    }
    memset((void *)g_stat_thread_kvirt_base, 0, STAT_USERSPACE_TOTAL_SIZE);

    for(i=0; i<STAT_MAX_THREAD_USERSPACE; i++)
    {
        g_stat_thread_kvirt_base[i].bUsed = HI_FALSE;
    }

    HI_OSAL_Snprintf(g_srtuStatDev.devfs_name, sizeof(g_srtuStatDev.devfs_name), "%s", UMAP_DEVNAME_STAT);
    g_srtuStatDev.fops = &DRV_stat_Fops;
    g_srtuStatDev.minor = UMAP_MIN_MINOR_STAT;
	g_srtuStatDev.owner  = THIS_MODULE;
	g_srtuStatDev.drvops = NULL;
    if(HI_DRV_DEV_Register(&g_srtuStatDev) < 0)
    {
        HI_ERR_STAT("cann't register stat dev\n"); //672
        dma_free_coherent(NULL, STAT_USERSPACE_TOTAL_SIZE, g_stat_thread_kvirt_base, g_stat_thread_phyaddr_base);
        return -1;
    }

    item = HI_DRV_PROC_AddModule(HI_MOD_STAT, NULL, NULL);
    if (! item)
    {
        HI_DRV_DEV_UnRegister(&g_srtuStatDev);
        dma_free_coherent(NULL, STAT_USERSPACE_TOTAL_SIZE, g_stat_thread_kvirt_base, g_stat_thread_phyaddr_base);
        return -1;
    }
    item->read = stat_proc_read;
    item->write = NULL;

	HI_DRV_STAT_EventFunc_Register(stat_event_fun);

	item2 = HI_DRV_PROC_AddModule("low_delay_statistics", NULL, NULL);
    if (!item2)
    {
        HI_ERR_STAT("cann't register low delay statistics interface.\n"); //690
        HI_DRV_DEV_UnRegister(&g_srtuStatDev);
        dma_free_coherent(NULL, STAT_USERSPACE_TOTAL_SIZE, g_stat_thread_kvirt_base, g_stat_thread_phyaddr_base);
        return -1;
    }

    item2->read = func_80539d84; //stat_proc_read;
    item2->write = NULL;

    return 0;

}


HI_VOID HI_DRV_STAT_Exit(HI_VOID)
{
	HI_DRV_PROC_RemoveModule("low_delay_statistics");
	HI_DRV_STAT_EventFunc_UnRegister();
    HI_DRV_PROC_RemoveModule(HI_MOD_STAT);
    HI_DRV_DEV_UnRegister(&g_srtuStatDev);
    dma_free_coherent(NULL, STAT_USERSPACE_TOTAL_SIZE, g_stat_thread_kvirt_base, g_stat_thread_phyaddr_base);
	return;
}

#if defined(HI_STAT_ISR_SUPPORTED)
HI_VOID HI_DRV_STAT_IsrReset(HI_VOID)
{
    memset((void *)g_IsrTime, 0, sizeof(g_IsrTime));
}

HI_VOID HI_DRV_STAT_IsrEnable(HI_VOID)
{
    g_stat_isr_enable = HI_TRUE;
}

HI_VOID HI_DRV_STAT_IsrDisable(HI_VOID)
{
    g_stat_isr_enable = HI_FALSE;
}

HI_VOID HI_DRV_STAT_IsrBegin(STAT_ISR_E isr)
{
    if(g_stat_isr_enable == HI_FALSE)
        return ;

    if(isr < STAT_ISR_BUTT)
        do_gettimeofday(&(g_IsrTime[isr].tv));
}

HI_VOID HI_DRV_STAT_IsrEnd(STAT_ISR_E isr)
{
    if(g_stat_isr_enable == HI_FALSE)
        return ;

    if(isr < STAT_ISR_BUTT)
    {
        struct timeval tv, tv2;
        HI_U32 time = 0;

        tv = g_IsrTime[isr].tv;

        do_gettimeofday(&tv2);

        if(tv2.tv_sec > tv.tv_sec)
        {
            time = (tv2.tv_sec - tv.tv_sec)*1000000 + tv2.tv_usec - tv.tv_usec;
        }
        else
        {
            time = tv2.tv_usec - tv.tv_usec;
        }

        if(time > g_IsrTime[isr].time)
            g_IsrTime[isr].time = time;
    }
}
#endif

HI_VOID stat_event_fun(STAT_EVENT_E enEvent, HI_U32 Value)
{
    HI_U64   SysTime;

    if ( enEvent >= STAT_EVENT_BUTT )
    {
        return;
    }

    SysTime = sched_clock();

    SysTime = DIV64(SysTime, 1000000);

    switch ( enEvent )
    {
        case STAT_EVENT_KEYIN :
        case STAT_EVENT_KEYOUT:    
        case STAT_EVENT_ISTREAMGET:
        case STAT_EVENT_BUFREADY:
        case STAT_EVENT_FRAMESYNCOK:
            {
                g_EventTime[enEvent].Value1 = Value;
            }
            break;
        case STAT_EVENT_CONNECT:
            {
                g_EventTime[STAT_EVENT_LOCKED].EventMs = 0;
            }
            break;
        case STAT_EVENT_VSTART:
            {
                HI_S32 Index;
                for (Index = STAT_EVENT_CWSET; Index < STAT_EVENT_BUTT; Index++ )
                {
                    g_EventTime[Index].EventMs = 0;
                    g_EventTime[Index].Value1 = 0;
                    g_EventTime[Index].Value2 = 0;                    
                }
            }
            break;
        case STAT_EVENT_CWSET :
        case STAT_EVENT_LOCKED :
            {
                if (0 != g_EventTime[enEvent].EventMs)
                {
                    return;
                }
            }
            break;
        case STAT_EVENT_IFRAMEINTER:
            {
                g_EventTime[enEvent].Value1 = Value - g_EventTime[enEvent].Value2;
                g_EventTime[enEvent].Value2 = Value;
            }
            break;
        default:
            break;
    }

    g_EventTime[enEvent].EventMs = (HI_U32)SysTime;     

    return;
}

#if defined(HI_STAT_ISR_SUPPORTED)
EXPORT_SYMBOL(HI_DRV_STAT_IsrBegin);
EXPORT_SYMBOL(HI_DRV_STAT_IsrEnd);
#endif

