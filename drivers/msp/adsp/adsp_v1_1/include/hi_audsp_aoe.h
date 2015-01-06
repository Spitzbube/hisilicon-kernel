/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_audsp_aoe.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2013/02/28
  Description   :
  History       :
  1.Date        : 2013/02/28
    Author      : zgjie
    Modification: Created file

 *******************************************************************************/

#ifndef  __HI_AUDSP_AOE__H__
#define  __HI_AUDSP_AOE__H__

#include "hi_type.h"
#include "hi_audsp_common.h"

#ifdef __cplusplus
 #if __cplusplus
extern "C" {
 #endif
#endif /* __cplusplus */

/***************************** Macro Definition ******************************/

#define AOE_RUNNING_FLAG 0xa5a5a5a5


/**Defines the  status of an ADSP.*/
typedef enum 
{
    ADSP_SYSCOM_CMD_START = 0, /**<Stop*//**<CNcomment: 停止 */
    ADSP_SYSCOM_CMD_STOP,     /**<Start*//**<CNcomment: 运行 */
  
    ADSP_SYSCOM_CMD_BUTT
}ADSP_SYSCOM_CMD_E;

typedef enum 
{
    ADSP_SYSCOM_CMD_DONE = 0, 
    ADSP_SYSCOM_CMD_ERR_START_FAIL = 1, 
    ADSP_SYSCOM_CMD_ERR_STOP_FAIL = 2, 
    ADSP_SYSCOM_CMD_ERR_CREATE_FAIL = 3, 
    ADSP_SYSCOM_CMD_ERR_UNKNOWCMD =  4,
    ADSP_SYSCOM_CMD_ERR_TIMEOUT = 5,
    ADSP_SYSCOM_CMD_ERR_BUTT
}ADSP_SYSCOM_CMD_RET_E;

typedef enum 
{
    ADSP_AOE_CMD_START = 0, /**<Stop*//**<CNcomment: 停止 */
    ADSP_AOE_CMD_STOP,     /**<Start*//**<CNcomment: 运行 */
  
    ADSP_AOE_CMD_BUTT
}ADSP_AOESYS_CMD_E;

typedef enum 
{
    ADSP_AOE_CMD_DONE = 0, 
    ADSP_AOE_CMD_ERR_START_FAIL = 1, 
    ADSP_AOE_CMD_ERR_STOP_FAIL = 2, 
    ADSP_AOE_CMD_ERR_CREATE_FAIL = 3, 
    ADSP_AOE_CMD_ERR_UNKNOWCMD =  4,
    ADSP_AOE_CMD_ERR_TIMEOUT = 5,
    ADSP_AOE_CMD_ERR_BUTT
}ADSP_AOESYS_CMD_RET_E;




#define AOE_REG_LENGTH  (2048)    //2k reg for aoe

#define AOE_MAX_ENGINE_NUM  6    
#define AOE_MAX_AOP_NUM    8    
#define AOE_MAX_AIP_NUM    8    

#define AOE_COM_REG_OFFSET      0x0000
#define AOE_ENGINE_REG_OFFSET   0x0040
#define AOE_AIP_REG_OFFSET      0x0100
#define AOE_AOP_REG_OFFSET      0x0300

#define AOE_COM_REG_BANDSIZE    sizeof(S_AOE_REGS_TYPE)
#define AOE_AIP_REG_BANDSIZE    sizeof(S_AIP_REGS_TYPE)
#define AOE_ENGINE_REG_BANDSIZE sizeof(S_MIXER_REGS_TYPE)
#define AOE_AOP_REG_BANDSIZE    sizeof(S_AOP_REGS_TYPE)

#define AOE_COM_REG_BASE (DSP0_SHARESRAM_BASEADDR + AOE_COM_REG_OFFSET)  

#define AOE_AIP_VOL_6dB (0x7f)
#define AOE_AIP_VOL_0dB (0x79)
#define AOE_AIP_VOL_infdB (0x28)
#define AOE_AIP_VOL_MAX_dB (AOE_AIP_VOL_6dB)
#define AOE_AIP_VOL_MIN_dB (AOE_AIP_VOL_infdB)
#define AIP_FIFO_LATENCYMS_DEFAULT    20

typedef enum
{
    AOE_AIP_CMD_START = 0, /**<Stop*//**<CNcomment: 停止 */
    AOE_AIP_CMD_STOP,    /**<Start*//**<CNcomment: 运行 */
    AOE_AIP_CMD_PAUSE,
    AOE_AIP_CMD_FLUSH,

    AOE_AIP_CMD_BUTT
} AOE_AIP_CMD_E;

typedef enum
{
    AOE_AIP_CMD_DONE = 0,
    AOE_AIP_CMD_ERR_START_FAIL = 1,
    AOE_AIP_CMD_ERR_STOP_FAIL   = 2,
    AOE_AIP_CMD_ERR_CREATE_FAIL = 3,
    AOE_AIP_CMD_ERR_UNKNOWCMD = 4,
    AOE_AIP_CMD_ERR_TIMEOUT = 5,
        

    AOE_AIP_CMD_ERR_BUTT
} AOE_AIP_CMD_RET_E;

/**Defines the  status of an ENGINE.*/
typedef enum 
{
    AOE_ENGINE_CMD_START = 0, /**<Stop*//**<CNcomment: 停止 */
    AOE_ENGINE_CMD_STOP,     /**<Start*//**<CNcomment: 运行 */
  
    AOE_ENGINE_CMD_BUTT
}AOE_ENGINE_CMD_E;


typedef enum 
{
    AOE_ENGINE_CMD_DONE = 0, 
    AOE_ENGINE_CMD_ERR_START_FAIL = 1, 
    AOE_ENGINE_CMD_ERR_STOP_FAIL = 2, 
    AOE_ENGINE_CMD_ERR_CREATE_FAIL = 3, 
    AOE_ENGINE_CMD_ERR_UNKNOWCMD =  4,
    AOE_ENGINE_CMD_ERR_TIMEOUT = 5,
    AOE_ENGINE_CMD_ERR_BUTT
}AOE_ENGINE_CMD_RET_E;


/**Defines the  status of an AOP.*/
typedef enum
{
    AOE_AOP_CMD_START = 0, /**<Stop*//**<CNcomment: 停止 */
    AOE_AOP_CMD_STOP, /**<Start*//**<CNcomment: 运行 */

    AOE_AOP_CMD_BUTT
} AOE_AOP_CMD_E;




typedef enum
{
    AOE_AOP_CMD_DONE = 0,
    AOE_AOP_CMD_ERR_START_FAIL = 1,
    AOE_AOP_CMD_ERR_STOP_FAIL   = 2,
    AOE_AOP_CMD_ERR_CREATE_FAIL = 3,
    AOE_AOP_CMD_ERR_UNKNOWCMD = 4,
    AOE_AOP_CMD_ERR_TIMEOUT = 5,
    AOE_AOP_CMD_ERR_BUTT
} AOE_AOP_CMD_RET_E;


/* Define the union U_AOE_STATUS1 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    overflow_cnt_0        : 16  ; /* [15..0]  */
        unsigned int    overflow_cnt_1        : 8   ; /* [23..16]  */
        unsigned int    overflow_cnt_2        : 8   ; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_AOE_STATUS1;

/* Define the union U_ENGINE_ATTR */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    precision             : 2   ; /* [1..0]  */
        unsigned int    ch                    : 2   ; /* [3..2]  */
        unsigned int    fs                    : 4   ; /* [7..4]  */
        unsigned int    format                : 2   ; /* [9..8]  */
        unsigned int    Reserved_2            : 22  ; /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_ENGINE_ATTR;

/* Define the union U_ENGINE_CTRL */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    cmd                   : 2   ; /* [1..0]  */
        unsigned int    Reserved_4            : 1   ; /* [2]  */
        unsigned int    cmd_done              : 1   ; /* [3]  */
        unsigned int    cmd_return_value      : 4   ; /* [7..4]  */
        unsigned int    ena_profile           : 1   ; /* [8]  */
        unsigned int    Reserved_3            : 23  ; /* [31..9]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_ENGINE_CTRL;

/* Define the union U_ENGINE_MIX_SRC */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    aip_fifo_ena          : 16  ; /* [15..0]  */
        unsigned int    Reserved_5            : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_ENGINE_MIX_SRC;

/* Define the union U_ENGINE_ROU_DST */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    aop_buf_ena           : 16  ; /* [15..0]  */
        unsigned int    Reserved_6            : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_ENGINE_ROU_DST;

/* Define the union U_ENGINE_PP_CTRL */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    pp_cmd                : 2   ; /* [1..0]  */
        unsigned int    Reserved_8            : 1   ; /* [2]  */
        unsigned int    pp_cmd_done           : 1   ; /* [3]  */
        unsigned int    dst_pp_enable_flag    : 16  ; /* [19..4]  */
        unsigned int    Reserved_7            : 12  ; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_ENGINE_PP_CTRL;

/* Define the union U_ENGINE_STATUS0 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    aver_mcps             : 8   ; /* [7..0]  */
        unsigned int    peak_mcps             : 8   ; /* [15..8]  */
        unsigned int    Reserved_10           : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_ENGINE_STATUS0;

/* Define the union U_AIP_BUFF_ATTR */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    aip_precision         : 2   ; /* [1..0]  */
        unsigned int    aip_ch                : 2   ; /* [3..2]  */
        unsigned int    aip_fs                : 4   ; /* [7..4]  */
        unsigned int    aip_format            : 2   ; /* [9..8]  */
        unsigned int    aip_alsa              : 1   ; /* [10]  */
        unsigned int    Reserved_12           : 21  ; /* [31..11]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_AIP_BUFF_ATTR;

/* Define the union U_AIP_FIFO_ATTR */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    fifo_precision        : 2   ; /* [1..0]  */
        unsigned int    fifo_ch               : 2   ; /* [3..2]  */
        unsigned int    fifo_fs               : 4   ; /* [7..4]  */
        unsigned int    fifo_format           : 2   ; /* [9..8]  */
        unsigned int    fifo_latency          : 6   ; /* [15..10]  */
        unsigned int    fifo_latency_real     : 6   ; /* [21..16]  */
        unsigned int    Reserved_13           : 10  ; /* [31..22]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_AIP_FIFO_ATTR;

/* Define the union U_AIP_CTRL */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    cmd                   : 2   ; /* [1..0]  */
        unsigned int    Reserved_14           : 1   ; /* [2]  */
        unsigned int    cmd_done              : 1   ; /* [3]  */
        unsigned int    cmd_return_value      : 4   ; /* [7..4]  */
        unsigned int    volume                : 7   ; /* [14..8]  */
        unsigned int    fade_en               : 1   ; /* [15]  */
        unsigned int    fade_in_rate          : 4   ; /* [19..16]  */
        unsigned int    fade_out_rate         : 4   ; /* [23..20]  */
        unsigned int    dst_fs_adj_step       : 5   ; /* [28..24]  */
        unsigned int    dst_fs_adj_dir        : 2   ; /* [30..29]  */
        unsigned int    ena_profile           : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_AIP_CTRL;

/* Define the union U_AIP_SRC_ATTR_EXT */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    fs_ext                : 24  ; /* [23..0]  */
        unsigned int    Reserved_15           : 8   ; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_AIP_SRC_ATTR_EXT;

/* Define the union U_AIP_BUF_SIZE */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    buff_size             : 24  ; /* [23..0]  */
        unsigned int    buff_flag             : 1   ; /* [24]  */
        unsigned int    Reserved_16           : 7   ; /* [31..25]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_AIP_BUF_SIZE;

/* Define the union U_AIP_BUF_TRANS_SIZE */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    tx_trans_size         : 24  ; /* [23..0]  */
        unsigned int    Reserved_19           : 8   ; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_AIP_BUF_TRANS_SIZE;

/* Define the union U_AIP_PP_CTRL */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    pp_cmd                : 2   ; /* [1..0]  */
        unsigned int    Reserved_21           : 1   ; /* [2]  */
        unsigned int    pp_cmd_done           : 1   ; /* [3]  */
        unsigned int    pp_enable             : 1   ; /* [4]  */
        unsigned int    Reserved_20           : 27  ; /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_AIP_PP_CTRL;

/* Define the union U_AIP_STATUS0 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    aver_mcps             : 8   ; /* [7..0]  */
        unsigned int    peak_mcps             : 8   ; /* [15..8]  */
        unsigned int    Reserved_23           : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_AIP_STATUS0;

/* Define the union U_AIP_FIFO_SIZE */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    aip_fifo_size         : 16  ; /* [15..0]  */
        unsigned int    aip_fifo_underflow_cnt  : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_AIP_FIFO_SIZE;

/* Define the union U_AOP_BUFF_ATTR */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    buf_precision         : 2   ; /* [1..0]  */
        unsigned int    buf_ch                : 2   ; /* [3..2]  */
        unsigned int    buf_fs                : 4   ; /* [7..4]  */
        unsigned int    buf_format            : 2   ; /* [9..8]  */
        unsigned int    buf_priority          : 1   ; /* [10]  */
        unsigned int    buf_delay             : 9   ; /* [19..11]  */
        unsigned int    buf_latency           : 6   ; /* [25..20]  */
        unsigned int    buf_latency_real      : 6   ; /* [31..26]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_AOP_BUFF_ATTR;

/* Define the union U_AOP_CTRL */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    cmd                   : 2   ; /* [1..0]  */
        unsigned int    Reserved_25           : 1   ; /* [2]  */
        unsigned int    cmd_done              : 1   ; /* [3]  */
        unsigned int    cmd_return_value      : 4   ; /* [7..4]  */
        unsigned int    ena_profile           : 1   ; /* [8]  */
        unsigned int    Reserved_24           : 23  ; /* [31..9]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_AOP_CTRL;

/* Define the union U_AOP_STATUS0 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    aver_mcps             : 8   ; /* [7..0]  */
        unsigned int    peak_mcps             : 8   ; /* [15..8]  */
        unsigned int    Reserved_26           : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_AOP_STATUS0;

/* Define the union U_AOP_BUF_SIZE */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    buff_size             : 24  ; /* [23..0]  */
        unsigned int    buff_flag             : 1   ; /* [24]  */
        unsigned int    buff_underflow_cnt    : 7   ; /* [31..25]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_AOP_BUF_SIZE;

//==============================================================================
/* Define the global struct */
typedef struct
{
	volatile unsigned int         AOE_MAGIC;
	volatile unsigned int         AOE_VERSION;
    volatile unsigned int         AOE_STATUS0;
    volatile U_AOE_STATUS1        AOE_STATUS1;
	volatile unsigned int         AOE_RESERVED[12];
} S_AOE_REGS_TYPE;

//AIP struct 
typedef struct
{
    volatile U_AIP_BUFF_ATTR      AIP_BUFF_ATTR;
    volatile U_AIP_FIFO_ATTR      AIP_FIFO_ATTR;
    volatile U_AIP_CTRL           AIP_CTRL;
    volatile U_AIP_SRC_ATTR_EXT   AIP_SRC_ATTR_EXT;
    volatile unsigned int         AIP_BUF_ADDR;
    volatile U_AIP_BUF_SIZE       AIP_BUF_SIZE;
    volatile unsigned int         AIP_BUF_WPTR;
    volatile unsigned int         AIP_BUF_RPTR;
    volatile U_AIP_BUF_TRANS_SIZE AIP_BUF_TRANS_SIZE;
    volatile U_AIP_PP_CTRL        AIP_PP_CTRL;
    volatile unsigned int         AIP_PP_ADDR;
    volatile U_AIP_STATUS0        AIP_STATUS0;
    volatile unsigned int         AIP_FIFO_ADDR;
    volatile U_AIP_FIFO_SIZE      AIP_FIFO_SIZE;
    volatile unsigned int         AIP_FIFO_WPTR;
    volatile unsigned int         AIP_FIFO_RPTR;

} S_AIP_REGS_TYPE;

//MIXER struct 
typedef struct
{
    volatile U_ENGINE_ATTR        ENGINE_ATTR;
    volatile U_ENGINE_CTRL        ENGINE_CTRL;
    volatile U_ENGINE_MIX_SRC     ENGINE_MIX_SRC;
    volatile U_ENGINE_ROU_DST     ENGINE_ROU_DST;
    volatile U_ENGINE_PP_CTRL     ENGINE_PP_CTRL;
    volatile unsigned int         ENGINE_PP_ADDR;
    volatile U_ENGINE_STATUS0     ENGINE_STATUS0;
    volatile unsigned int         ENGINE_RESERVED1;

} S_MIXER_REGS_TYPE;

//AOP Struct 
typedef struct
{
    volatile U_AOP_BUFF_ATTR      AOP_BUFF_ATTR;
    volatile U_AOP_CTRL           AOP_CTRL;
    volatile U_AOP_STATUS0        AOP_STATUS0;
    volatile unsigned int         AOP_RESERVED1;
    volatile unsigned int         AOP_BUF_ADDR;
    volatile U_AOP_BUF_SIZE       AOP_BUF_SIZE;
    volatile unsigned int         AOP_BUF_WPTR;
    volatile unsigned int         AOP_BUF_RPTR;

} S_AOP_REGS_TYPE;
















#ifdef __cplusplus
 #if __cplusplus
}
 #endif
#endif /* __cplusplus */

#endif 


