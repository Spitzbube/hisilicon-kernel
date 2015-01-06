//**********************************************************************
//
// Copyright(c)2008,Huawei Technologies Co.,Ltd
// All rights reserved.
//
// File Name   : drv_pq_define.h
// Author      : l00206803
// Version     : v0.1
// Abstract    : header of pq define
//**********************************************************************

#ifndef _DRV_PQ_DEFINE_H_
#define _DRV_PQ_DEFINE_H_

#include "hi_type.h"
#include "drv_pq_alg.h"

#define PQ_VERSION "1.0.0.0"
#define PQ_DEF_NAME         "pqparam"
#define PQ_CMD_VIRTUAL_DEI_CTRL                                                           0xffff1000
#define PQ_CMD_VIRTUAL_DEI_GLOBAL_MOTION_CTRL                                0xffff1001
#define PQ_CMD_VIRTUAL_DEI_DIR_CHECK                                                 0xffff1002
#define PQ_CMD_VIRTUAL_DEI_DIR_MULTI                                                  0xffff1003
#define PQ_CMD_VIRTUAL_DEI_INTP_SCALE_RATIO                                    0xffff1004
#define PQ_CMD_VIRTUAL_DEI_INTP_CTRL                                                 0xffff1005
#define PQ_CMD_VIRTUAL_DEI_JITTER_MOTION                                         0xffff1006
#define PQ_CMD_VIRTUAL_DEI_FIELD_MOTION                                          0xffff1007
#define PQ_CMD_VIRTUAL_DEI_MOTION_RATIO_CURVE                              0xffff1008
#define PQ_CMD_VIRTUAL_DEI_IIR_MOTION_CURVE                                   0xffff1009
#define PQ_CMD_VIRTUAL_DEI_REC_MODE                                                 0xffff100a
#define PQ_CMD_VIRTUAL_DEI_HIST_MOTION                                             0xffff100b
#define PQ_CMD_VIRTUAL_DEI_MOR_FLT                                                    0xffff100c
#define PQ_CMD_VIRTUAL_DEI_OPTM_MODULE                                           0xffff100d
#define PQ_CMD_VIRTUAL_DEI_COMB_CHK                                                 0xffff100e
#define PQ_CMD_VIRTUAL_DEI_CRS_CLR                                                     0xffff100f
#define PQ_CMD_VIRTUAL_FMD_CTRL                                                           0xffff1800
#define PQ_CMD_VIRTUAL_FMD_HISTBIN                                                      0xffff1801
#define PQ_CMD_VIRTUAL_FMD_PCCTHD                                                      0xffff1802
#define PQ_CMD_VIRTUAL_FMD_PCCBLK                                                       0xffff1803
#define PQ_CMD_VIRTUAL_FMD_UMTHD                                                        0xffff1804
#define PQ_CMD_VIRTUAL_FMD_ITDIFF                                                         0xffff1805
#define PQ_CMD_VIRTUAL_FMD_LAST                                                           0xffff1806
#define PQ_CMD_VIRTUAL_DNR_CTRL                       0xffff1100
#define PQ_CMD_VIRTUAL_DNR_DR_FILTER              0xffff1101
#define PQ_CMD_VIRTUAL_DNR_DB_FILTER              0xffff1102
#define PQ_CMD_VIRTUAL_DNR_INFO                      0xffff1103
#define PQ_CMD_VIRTUAL_SHARP_VHD_LUMA                   0xffff1200
#define PQ_CMD_VIRTUAL_SHARP_VHD_CHROMA              0xffff1201
#define PQ_CMD_VIRTUAL_SHARP_VSD_LUMA                   0xffff1202
#define PQ_CMD_VIRTUAL_SHARP_VSD_CHROMA              0xffff1203
#define PQ_CMD_VIRTUAL_SHARP_STR_LUMA                   0xffff1204
#define PQ_CMD_VIRTUAL_SHARP_STR_CHROMA              0xffff1205
#define PQ_CMD_VIRTUAL_SHARP_GP0_LUMA                   0xffff1206
#define PQ_CMD_VIRTUAL_SHARP_GP0_CHROMA              0xffff1207
#define PQ_CMD_VIRTUAL_SHARP_GP1_LUMA                   0xffff1208
#define PQ_CMD_VIRTUAL_SHARP_GP1_CHROMA              0xffff1209
#define PQ_CMD_VIRTUAL_CSC_VDP_V0              0xffff1900
#define PQ_CMD_VIRTUAL_CSC_VDP_V1              0xffff1901
#define PQ_CMD_VIRTUAL_CSC_VDP_V3              0xffff1902
#define PQ_CMD_VIRTUAL_CSC_VDP_V4              0xffff1903
#define PQ_CMD_VIRTUAL_CSC_VDP_G0              0xffff1904
#define PQ_CMD_VIRTUAL_CSC_VDP_G1              0xffff1905
#define PQ_CMD_VIRTUAL_CSC_VDP_WBC_DHD0              0xffff1906
#define PQ_CMD_VIRTUAL_CSC_VDP_INTER_DHD0              0xffff1907
#define PQ_CMD_VIRTUAL_CSC_VDP_INTER_DHD1              0xffff1908
#define PQ_CMD_VIRTUAL_ACC_MOD_GENTLE                      0xffff1300
#define PQ_CMD_VIRTUAL_ACC_MOD_MIDDLE                      0xffff1301
#define PQ_CMD_VIRTUAL_ACC_MOD_STRONG                     0xffff1302
#define PQ_CMD_VIRTUAL_ACC_CTRL                                   0xffff1303
#define PQ_CMD_VIRTUAL_ACM_MOD_BLUE                      0xffff1400
#define PQ_CMD_VIRTUAL_ACM_MOD_GREEN                      0xffff1401
#define PQ_CMD_VIRTUAL_ACM_MOD_BG                      0xffff1402
#define PQ_CMD_VIRTUAL_ACM_MOD_SKIN                      0xffff1403
#define PQ_CMD_VIRTUAL_ACM_MOD_VIVID                      0xffff1404
#define PQ_CMD_VIRTUAL_ACM_CTRL                                0xffff1405
#define PQ_CMD_VIRTUAL_GAMMA_RGB_MOD0                  0xffff1500
#define PQ_CMD_VIRTUAL_GAMMA_RGB_MOD1                  0xffff1501
#define PQ_CMD_VIRTUAL_GAMMA_RGB_MOD2                  0xffff1502
#define PQ_CMD_VIRTUAL_GAMMA_RGB_MOD3                  0xffff1503
#define PQ_CMD_VIRTUAL_GAMMA_YUV_MOD0                  0xffff1504
#define PQ_CMD_VIRTUAL_GAMMA_YUV_MOD1                  0xffff1505
#define PQ_CMD_VIRTUAL_GAMMA_YUV_MOD2                  0xffff1506
#define PQ_CMD_VIRTUAL_GAMMA_YUV_MOD3                  0xffff1507
#define PQ_CMD_VIRTUAL_GAMMA_CTRL_DISPLAY0                   0xffff1508
#define PQ_CMD_VIRTUAL_GAMMA_CTRL_DISPLAY1                   0xffff1509
#define PQ_CMD_VIRTUAL_DITHER_COEF                              0xffff1600
#define PQ_CMD_VIRTUAL_OPTION_DISP0                             0xffff1700
#define PQ_CMD_VIRTUAL_OPTION_DISP1                             0xffff1704
#define PQ_CMD_VIRTUAL_OPTION_DISP2                             0xffff1708
#define PQ_CMD_VIRTUAL_DEBUG_WINDOW_CHAN0              0xffff2000
#define PQ_CMD_VIRTUAL_DEBUG_WINDOW_CHAN1              0xffff2004
#define PQ_CMD_VIRTUAL_DEBUG_WINDOW_CHAN2              0xffff2008
#define PQ_CMD_VIRTUAL_CAPTURE_DISP0                           0xffff2100
#define PQ_CMD_VIRTUAL_CAPTURE_DISP1                           0xffff2104
#define PQ_CMD_VIRTUAL_CAPTURE_DISP2                           0xffff2108
#define PQ_CMD_VIRTUAL_DEBUG_DISP0                              0xffff2200
#define PQ_CMD_VIRTUAL_DEBUG_DISP1                              0xffff2204
#define PQ_CMD_VIRTUAL_DEBUG_DISP2                              0xffff2208
#define PQ_CMD_VIRTUAL_BIN_EXPORT                                0xffffff00
#define PQ_CMD_VIRTUAL_BIN_IMPORT                                0xffffff04
#define PQ_CMD_VIRTUAL_BIN_FIXED                                  0xffffff08
#define PQ_CMD_VIRTUAL_DRIVER_VERSION                       0xffffffff

#define    PQ_IOC_SET_PQ        128
#define    PQ_IOC_GET_PQ        129



///////////////////////////////////////////////define gamma//////////////////////

typedef struct hi_PQ_COMM_GAMMA_RGB_S
{
    HI_U32 au32R[GAMMA_NUM + 1];   /**<  R Y*/
    HI_U32 au32G[GAMMA_NUM + 1];   /**<  G U*/
    HI_U32 au32B[GAMMA_NUM + 1];   /**<  B V*/
} PQ_COMM_GAMMA_RGB_S;

typedef struct hi_PQ_TOP_OFST_TABLE_S
{
    HI_U32      u32FileHeaderOfst;
    HI_U32      u32DefaltOptOfst;
    HI_U32      u32CoefOfst;
} PQ_TOP_OFST_TABLE_S;

/////////////////////////////////////////////////define header///////////////////////////////////////////////////////////////
typedef struct hi_PQ_FILE_HEADER_S
{
    HI_U32       u32FileCheckSum;  /* 参数文件的校验和，File Header（除此变量外）和data的所有逐字节校验和，用于检验参数正确性和判断是否dirty */
    HI_U32       u32ParamSize;  /* 参数文件大小，包括File header和data */
    HI_CHAR        u8Version[STR_LEN_32];   /* 版本号，字符串表示 */
    HI_CHAR        u8Author[STR_LEN_32];    /* 参数调试者签名，字符串表示 */
    HI_CHAR        u8Desc[STR_LEN_1024];   /* 版本描述，字符串表示 */
    HI_CHAR       u8Time[STR_LEN_32];       /* 参数文件生成（烧写）时间，[0] ~ [5]：yy mm dd hh mm ss，[6]~[7]保留。该时间由PQ工具从PC上自动获取，无需用户输入 */
    /* 以下是参数文件生成时的Chip、SDK版本，当这个参数文件被下载到其它盒子时，接受参数的盒子根据这两个参数判断是否兼容 */
    HI_CHAR        u8ChipName[STR_LEN_32];
    HI_CHAR        u8SDKVersion[STR_LEN_80];
} PQ_FILE_HEADER_S;

typedef struct hi_PQ_OPT_OFST_TABLE_S
{
    HI_U32 u32OptCoefDispp0OfSt;
    HI_U32 u32OptCoefDispp1OfSt;
    HI_U32 u32OptCoefDispp2OfSt;
} PQ_OPT_OFST_TABLE_S;
typedef struct hi_PQ_OPT_S
{
    PQ_OPT_OFST_TABLE_S stPqOptOfstTable;
    PQ_OPT_COEF_S stPqOptDisp0Coef;
    PQ_OPT_COEF_S stPqOptDisp1Coef;
    PQ_OPT_COEF_S stPqOptDisp2Coef;
} PQ_OPT_S;
typedef struct hi_PQ_COEF_OFST_TABLE_S
{
    HI_U32      u32DeiOfst;
    HI_U32      u32FmdOfst;
    HI_U32      u32DnrOfst;
    HI_U32      u32SharpOfst;
    HI_U32      u32CscOfst;
    HI_U32      u32AccOfst;
    HI_U32      u32AcmOfst;
    HI_U32      u32GammaOfst;
    HI_U32      u32DitherOfst;
} PQ_COEF_OFST_TABLE_S;
typedef struct hi_PQ_COEF_S
{
    PQ_COEF_OFST_TABLE_S      stCoefOfstTable;
    PQ_DEI_COEF_S        stDeiCoef;
    PQ_FMD_COEF_S    stFmdCoef;
    PQ_DNR_COEF_S       stDnrCoef;
    PQ_SHARP_COEF_S     stSharpCoef;
    PQ_CSC_COEF_S        stCscCoef;
    PQ_ACC_COEF_S       stAccCoef;
    PQ_ACM_COEF_S       stAcmCoef;
    PQ_GAMMA_COEF_S    stGammaCoef;
    PQ_DITHER_COEF_S    stDitherCoef;

} PQ_COEF_S;
typedef struct hi_PQ_PARAM_S
{
    PQ_TOP_OFST_TABLE_S         stTopOfstTable;
    PQ_FILE_HEADER_S               stPQFileHeader;
    PQ_OPT_S                           stPqOption;
    PQ_COEF_S                         stPQCoef;
} PQ_PARAM_S;

//////////////////////////////////////////////////define import///////////////////////////////////////////////////
typedef struct hi_PQ_IMPORT_S
{
    HI_CHAR        u8Author[STR_LEN_32];    /* 参数调试者签名，字符串表示 */
    HI_CHAR        u8Desc[STR_LEN_1024];   /* 版本描述，字符串表示 */
    HI_CHAR       u8Time[STR_LEN_32];       /* 参数文件生成（烧写）时间，[0] ~ [5]：yy mm dd hh mm ss，[6]~[7]保留。该时间由PQ工具从PC上自动获取，无需用户输入 */
} PQ_IMPORT_S;


//////////////////////////////////////define io ctrl//////////////////////////////////////////////////////////////

typedef struct hi_PQ_IO_S
{
    HI_U32 u32PqCmd;
    union
    {
        PQ_OPT_COEF_S                          stPqOptCoef;
        PQ_DEI_CTRL                               stPqDeiCtrl;
        PQ_DEI_GLOBAL_MOTION_CTRL       stPqDeiGlobalMotionCtrl;
        PQ_DEI_DIR_CHECK                       stPqDeiDirCheck;
        PQ_DEI_DIR_MULTI                        stPqDeiDirMulti;
        PQ_DEI_INTP_SCALE_RATIO            stPqDeiIntpScaleRatio;
        PQ_DEI_INTP_CTRL                        stPqDeiIntpCtrl;
        PQ_DEI_JITTER_MOTION                 stPqDeiJitterMotion;
        PQ_DEI_FIELD_MOTION                   stPqDeiFieldMotion;
        PQ_DEI_MOTION_RATIO_CURVE        stPqDeiMotionRatioCurve;
        PQ_DEI_IIR_MOTION_CURVE             stPqDeiIirMotionCurve;
        PQ_DEI_REC_MODE                         stPqDeiRecMode;
        PQ_DEI_HIST_MOTION                     stPqDeiHistMotion;
        PQ_DEI_MOR_FLT	                           stPqDeiMorFlt;
        PQ_DEI_OPTM_MODULE                    stPqDeiOptmModule;
        PQ_DEI_COMB_CHK                          stPqDeiCombChk;
        PQ_DEI_CRS_CLR                             stPqDeiCrsClr;
        PQ_FMD_CTRL_S                             stPqFmdCtrl;
        PQ_FMD_HISTBIN_THD_S                stPqFmdHistbinThd;
        PQ_FMD_PCC_THD_S                      stPqFmdPccThd;
        PQ_FMD_PCC_BLK_S                      stPqFmdPccBlk;
        PQ_FMD_UM_THD_S	                  stPqFmdUmThd;
        PQ_FMD_ITDIFF_THD_S                 stPqFmdItdiffThd;
        PQ_FMD_LASI_THD_S                    stPqFmdLashThd;
        PQ_DNR_CTRL_S                          stPqDnrCtrl;
        PQ_DNR_DB_FILTER_S                  stPqDnrDbFilter;
        PQ_DNR_DR_FILTER_S                  stPqDnrDrFilter;
        PQ_DNR_INFO_S                          stPqDnrInfo;
        PQ_SHARP_LUMA_S                      stPqSharpLuma;
        PQ_SHARP_CHROMA_S                  stPqSharpChroma;
        PQ_CSC_DATA_S                          stPqCscData;
        PQ_ACC_CRV_S                            stAccCrv;
        PQ_ACC_CTRL_S                           stAccCtrl;
        PQ_ACM_LUT_S                            stAcmLut;
        PQ_ACM_CTRL_S                          stAcmCtrl;
        PQ_GAMMA_CTRL_S                      stPqGammaCtrl;
        PQ_COMM_GAMMA_RGB_S             stPqGammaMode;
        PQ_DITHER_COEF_S                     stDitherCoef;
        HI_U32                                       u32PqDriverBinPhyAddr;
    } u_Data;
} PQ_IO_S;


#define CMD_IOC_GET_PQ            _IOWR(HI_ID_PQ, PQ_IOC_GET_PQ, PQ_IO_S)
#define CMD_IOC_SET_PQ            _IOWR(HI_ID_PQ, PQ_IOC_SET_PQ, PQ_IO_S)



#endif

