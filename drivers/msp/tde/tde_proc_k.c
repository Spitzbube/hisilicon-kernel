#ifdef __cplusplus
 #if __cplusplus
extern "C" {
 #endif  /* __cplusplus */
#endif  /* __cplusplus */

#include "tde_proc.h"
#include "tde_config.h"
typedef struct _hiTDE_PROCINFO_S
{
    HI_U32          u32CurNode;
    TDE_HWNode_S    stTdeHwNode[TDE_MAX_PROC_NUM];
    HI_BOOL         bProcEnable;
    HI_BOOL         bTraceEnbale;
}TDE_PROCINFO_S;

TDE_PROCINFO_S g_stTdeProcInfo;

HI_VOID TDEProcEnable(HI_BOOL bEnable)
{
    g_stTdeProcInfo.bProcEnable = bEnable;
}

HI_VOID TDEProcRecordNode(TDE_HWNode_S* pHWNode)
{
    if ((!g_stTdeProcInfo.bProcEnable) || (HI_NULL == pHWNode))
    {
        return;
    }
    
    memcpy(&g_stTdeProcInfo.stTdeHwNode[g_stTdeProcInfo.u32CurNode], pHWNode, sizeof(TDE_HWNode_S));

    g_stTdeProcInfo.u32CurNode++;
    g_stTdeProcInfo.u32CurNode = (g_stTdeProcInfo.u32CurNode)%TDE_MAX_PROC_NUM;
}

HI_VOID TDEProcClearNode(HI_VOID)
{
    memset(&g_stTdeProcInfo.stTdeHwNode[0], 0, sizeof(g_stTdeProcInfo.stTdeHwNode));
    g_stTdeProcInfo.u32CurNode = 0;
}


HI_VOID TDEProcEnableTrace(HI_BOOL bEnable)
{
    g_stTdeProcInfo.bTraceEnbale = bEnable;
}


int tde_write_proc(struct file * file,
    const char __user * buf, size_t count, loff_t *ppos)  
{
    char buffer[128] = {0};

    if(count > sizeof(buffer))
    {
        TDE_TRACE(TDE_KERN_INFO, "The command string is out of buf space :%d bytes !\n", sizeof(buffer));
        return 0;
    }
    
    if(copy_from_user(buffer, buf, count))
    {
        TDE_TRACE(TDE_KERN_INFO, "<%s> Line %d:copy_from_user failed!\n", __FUNCTION__, __LINE__);
        return 0;
    }

    if (strstr(buffer, "trace on"))
    {
        TDEProcEnableTrace(HI_TRUE);
        TDE_TRACE(TDE_KERN_INFO, "tde trace on\n");
    }
    else if (strstr(buffer, "trace off"))
    {
        TDEProcEnableTrace(HI_FALSE);
        TDE_TRACE(TDE_KERN_INFO, "tde trace off\n");
    }
    else if (strstr(buffer, "proc on"))
    {
        TDEProcEnable(HI_TRUE);
        TDE_TRACE(TDE_KERN_INFO, "tde proc on\n");
    }
    else if (strstr(buffer, "proc off"))
    {
        TDEProcEnable(HI_FALSE);
        TDE_TRACE(TDE_KERN_INFO, "tde proc off\n");
    }
    else if (strstr(buffer, "node clear"))
    {
        TDEProcClearNode();
        TDE_TRACE(TDE_KERN_INFO, "node buffer was cleared\n");
    }

    return count;
}

int tde_read_proc(struct seq_file *p, HI_VOID *v)
{
    HI_S32 len = 0;
    HI_S32 i, j;
    HI_U32* pu32Cur;

    /* see define of TDE_HWNode_S */
    HI_U8*  chUpdate[] =
    {
        "INS         ",
        "S1_ADDR     ",
        "S1_TYPE     ",
        "S1_XY       ",
        "S1_FILL     ",
        "S2_ADDR     ",
        "S2_TYPE     ",
        "S2_XY       ",
        "S2_SIZE     ",
        "S2_FILL     ",
        "TAR_ADDR    ",
        "TAR_TYPE    ",
        "TAR_XY      ",
        "TS_SIZE     ",
        "COLOR_CONV  ",
        "CLUT_ADDR   ",
        "2D_RSZ      ",
        "HF_COEF_ADDR",
        "VF_COEF_ADDR",
        "RSZ_STEP    ",
        "RSZ_Y_OFST  ",
        "RSZ_X_OFST  ",
        "DFE_COEF0   ",
        "DFE_COEF1   ",
        "DFE_COEF2   ",
        "DFE_COEF3   ",
        "ALU         ",
        "CK_MIN      ",
        "CK_MAX      ",
        "CLIP_START  ",
        "CLIP_STOP   ",
        "Y1_ADDR     ",
        "Y1_PITCH    ",
        "Y2_ADDR     ",
        "Y2_PITCH    ",
        "RSZ_VSTEP   ",
        "ARGB_ORDER  ",
        "CK_MASK     ",
        "COLORIZE    ",
        "ALPHA_BLEND ",
        "UPDATE1     ",
        "UPDATE2     "
    };

    TDE_HWNode_S *pstHwNode = g_stTdeProcInfo.stTdeHwNode;
    p = wprintinfo(p+len);
     #ifndef CONFIG_TDE_STR_DISABLE
    /* print node information */
    PROC_PRINT(p,"\n--------- Hisilicon TDE Node params Info ---------\n");

    for (j = 0 ; j < g_stTdeProcInfo.u32CurNode; j++)
    {
        if (0 != pstHwNode[j].u64TDE_UPDATE)
        {
            PROC_PRINT(p, "\n--- HWNode update: 0x%08x %08x ---\n", 
                (HI_U32)(pstHwNode[j].u64TDE_UPDATE >> 32),
                (HI_U32)pstHwNode[j].u64TDE_UPDATE);

            pu32Cur = (HI_U32*)&pstHwNode[j];
            for (i = 0; i < sizeof(TDE_HWNode_S) / 4 - 2; i++)
            {
                if (((pstHwNode[j].u64TDE_UPDATE >> i) & 1) != 0)
                    PROC_PRINT(p, "(%s):\t0x%08x\n", chUpdate[i], *(pu32Cur + i));
            }
        }
    }
    #endif
    return 0;
}

#ifdef __cplusplus
 #if __cplusplus
}
 #endif /* __cplusplus */
#endif  /* __cplusplus */


