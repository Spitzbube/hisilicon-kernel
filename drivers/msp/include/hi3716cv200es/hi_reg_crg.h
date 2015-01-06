//******************************************************************************
// Copyright     :  Copyright (C) 2013, Hisilicon Technologies Co., Ltd.
// File name     :  hi_reg_crg.h
// Version       :  1.0
// Author        :
// Date          :  2013-08-27
// Description   :  The C union definition file for the module CRG
// Others        :  Generated automatically by nManager V4.0
//******************************************************************************

#ifndef __HI_REG_CRG_H__
#define __HI_REG_CRG_H__

/* Define the union U_PERI_CRG0 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    cpu_pll_cfg0_apb      : 31  ; /* [30..0]  */
        unsigned int    reserved_0            : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG0;

/* Define the union U_PERI_CRG1 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    cpu_pll_cfg1_apb      : 27  ; /* [26..0]  */
        unsigned int    reserved_0            : 5   ; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG1;

/* Define the union U_PERI_CRG2 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    bpll_ctrl0            : 31  ; /* [30..0]  */
        unsigned int    reserved_0            : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG2;

/* Define the union U_PERI_CRG3 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    bpll_ctrl1            : 27  ; /* [26..0]  */
        unsigned int    reserved_0            : 5   ; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG3;

/* Define the union U_PERI_CRG4 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    ddr_pll_cfg0_apb      : 31  ; /* [30..0]  */
        unsigned int    reserved_0            : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG4;

/* Define the union U_PERI_CRG5 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    ddr_pll_cfg1_apb      : 27  ; /* [26..0]  */
        unsigned int    reserved_0            : 5   ; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG5;

/* Define the union U_PERI_CRG6 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    w_pll_cfg0_apb        : 31  ; /* [30..0]  */
        unsigned int    reserved_0            : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG6;

/* Define the union U_PERI_CRG7 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    w_pll_cfg1_apb        : 27  ; /* [26..0]  */
        unsigned int    reserved_0            : 5   ; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG7;

/* Define the union U_PERI_CRG8 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    vpll_ctrl0            : 31  ; /* [30..0]  */
        unsigned int    reserved_0            : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG8;

/* Define the union U_PERI_CRG9 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    vpll_ctrl1            : 27  ; /* [26..0]  */
        unsigned int    reserved_0            : 5   ; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG9;

/* Define the union U_PERI_CRG10 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    hpll_ctrl0            : 31  ; /* [30..0]  */
        unsigned int    reserved_0            : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG10;

/* Define the union U_PERI_CRG11 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    hpll_ctrl1            : 27  ; /* [26..0]  */
        unsigned int    reserved_0            : 5   ; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG11;

/* Define the union U_PERI_CRG12 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    epll_ctrl0            : 31  ; /* [30..0]  */
        unsigned int    reserved_0            : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG12;

/* Define the union U_PERI_CRG13 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    epll_ctrl1            : 27  ; /* [26..0]  */
        unsigned int    reserved_0            : 5   ; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG13;

/* Define the union U_PERI_CRG14 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    mpll_ctrl0            : 31  ; /* [30..0]  */
        unsigned int    reserved_0            : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG14;

/* Define the union U_PERI_CRG15 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    mpll_ctrl1            : 27  ; /* [26..0]  */
        unsigned int    reserved_0            : 5   ; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG15;

/* Define the union U_PERI_CRG16 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    qpll_ctrl0            : 31  ; /* [30..0]  */
        unsigned int    reserved_0            : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG16;

/* Define the union U_PERI_CRG17 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    qpll_ctrl1            : 27  ; /* [26..0]  */
        unsigned int    reserved_0            : 5   ; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG17;

/* Define the union U_PERI_CRG18 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    cpu_freq_sel_cfg_crg  : 3   ; /* [2..0]  */
        unsigned int    reserved_2            : 1   ; /* [3]  */
        unsigned int    cpu_freq_div_cfg_crg  : 2   ; /* [5..4]  */
        unsigned int    reserved_1            : 2   ; /* [7..6]  */
        unsigned int    cpu_div_cfg_bypass    : 1   ; /* [8]  */
        unsigned int    cpu_begin_cfg_bypass  : 1   ; /* [9]  */
        unsigned int    cpu_sw_begin_cfg      : 1   ; /* [10]  */
        unsigned int    reserved_0            : 21  ; /* [31..11]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG18;

/* Define the union U_PERI_CRG19 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    timeout_plllock       : 12  ; /* [11..0]  */
        unsigned int    lock_bypass           : 1   ; /* [12]  */
        unsigned int    pll_cfg_bypass        : 1   ; /* [13]  */
        unsigned int    reserved_0            : 18  ; /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG19;

/* Define the union U_PERI_CRG20 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    cpu_clkoff_sys        : 2   ; /* [1..0]  */
        unsigned int    neon_clkoff_sys       : 2   ; /* [3..2]  */
        unsigned int    l2_clkoff_sys         : 1   ; /* [4]  */
        unsigned int    reserved_2            : 3   ; /* [7..5]  */
        unsigned int    a9_atclk_cken         : 1   ; /* [8]  */
        unsigned int    a9_pclkdbg_cken       : 1   ; /* [9]  */
        unsigned int    a9_ctmclk_cken        : 1   ; /* [10]  */
        unsigned int    a9_peri_cken          : 1   ; /* [11]  */
        unsigned int    a9_trace_cken         : 1   ; /* [12]  */
        unsigned int    a9_acp_cken           : 1   ; /* [13]  */
        unsigned int    reserved_1            : 2   ; /* [15..14]  */
        unsigned int    arm_srst_req          : 2   ; /* [17..16]  */
        unsigned int    neon_srst_req         : 2   ; /* [19..18]  */
        unsigned int    cs_srst_req           : 1   ; /* [20]  */
        unsigned int    cluster_peri_srst_req : 1   ; /* [21]  */
        unsigned int    cluster_scu_srst_req  : 1   ; /* [22]  */
        unsigned int    cluster_ptm_srst_req  : 1   ; /* [23]  */
        unsigned int    cluster_dbg_srst_req  : 2   ; /* [25..24]  */
        unsigned int    sc_wd_srst_req        : 2   ; /* [27..26]  */
        unsigned int    reserved_0            : 4   ; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG20;

/* Define the union U_PERI_CRG21 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    l2_clk_div            : 2   ; /* [1..0]  */
        unsigned int    reserved_3            : 2   ; /* [3..2]  */
        unsigned int    acp_clk_div           : 2   ; /* [5..4]  */
        unsigned int    reserved_2            : 2   ; /* [7..6]  */
        unsigned int    peri_clk_div          : 3   ; /* [10..8]  */
        unsigned int    reserved_1            : 1   ; /* [11]  */
        unsigned int    trace_clk_div         : 4   ; /* [15..12]  */
        unsigned int    reserved_0            : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG21;

/* Define the union U_PERI_CRG22 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    core_bus_clk_sel      : 2   ; /* [1..0]  */
        unsigned int    mde0_clk_sel          : 2   ; /* [3..2]  */
        unsigned int    mde1_clk_sel          : 2   ; /* [5..4]  */
        unsigned int    reserved_1            : 2   ; /* [7..6]  */
        unsigned int    mde2_clk_sel          : 2   ; /* [9..8]  */
        unsigned int    mde3_clk_sel          : 2   ; /* [11..10]  */
        unsigned int    core_bus_clk_div      : 2   ; /* [13..12]  */
        unsigned int    reserved_0            : 18  ; /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG22;

/* Define the union U_PERI_CRG23 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    sfc_cken              : 1   ; /* [0]  */
        unsigned int    reserved_2            : 3   ; /* [3..1]  */
        unsigned int    sfc_srst_req          : 1   ; /* [4]  */
        unsigned int    reserved_1            : 3   ; /* [7..5]  */
        unsigned int    sfc_clk_sel           : 3   ; /* [10..8]  */
        unsigned int    reserved_0            : 21  ; /* [31..11]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG23;

/* Define the union U_PERI_CRG24 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    nf_cken               : 1   ; /* [0]  */
        unsigned int    reserved_2            : 3   ; /* [3..1]  */
        unsigned int    nf_srst_req           : 1   ; /* [4]  */
        unsigned int    reserved_1            : 3   ; /* [7..5]  */
        unsigned int    nf_clk_sel            : 3   ; /* [10..8]  */
        unsigned int    reserved_0            : 21  ; /* [31..11]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG24;

/* Define the union U_PERI_CRG25 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    ddr_cken              : 1   ; /* [0]  */
        unsigned int    ddrphy_cken           : 1   ; /* [1]  */
        unsigned int    reserved_1            : 2   ; /* [3..2]  */
        unsigned int    ddrphy_clk_sel        : 1   ; /* [4]  */
        unsigned int    reserved_0            : 27  ; /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG25;

/* Define the union U_PERI_CRG26 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    uart1_cken            : 1   ; /* [0]  */
        unsigned int    uart1_srst_req        : 1   ; /* [1]  */
        unsigned int    reserved_3            : 2   ; /* [3..2]  */
        unsigned int    uart2_cken            : 1   ; /* [4]  */
        unsigned int    uart2_srst_req        : 1   ; /* [5]  */
        unsigned int    reserved_2            : 2   ; /* [7..6]  */
        unsigned int    uart3_cken            : 1   ; /* [8]  */
        unsigned int    uart3_srst_req        : 1   ; /* [9]  */
        unsigned int    reserved_1            : 2   ; /* [11..10]  */
        unsigned int    uart4_cken            : 1   ; /* [12]  */
        unsigned int    uart4_srst_req        : 1   ; /* [13]  */
        unsigned int    reserved_0            : 18  ; /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG26;

/* Define the union U_PERI_CRG27 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    i2c_qam_cken          : 1   ; /* [0]  */
        unsigned int    i2c_qam_srst_req      : 1   ; /* [1]  */
        unsigned int    reserved_5            : 2   ; /* [3..2]  */
        unsigned int    i2c0_cken             : 1   ; /* [4]  */
        unsigned int    i2c0_srst_req         : 1   ; /* [5]  */
        unsigned int    reserved_4            : 2   ; /* [7..6]  */
        unsigned int    i2c1_cken             : 1   ; /* [8]  */
        unsigned int    i2c1_srst_req         : 1   ; /* [9]  */
        unsigned int    reserved_3            : 2   ; /* [11..10]  */
        unsigned int    i2c2_cken             : 1   ; /* [12]  */
        unsigned int    i2c2_srst_req         : 1   ; /* [13]  */
        unsigned int    reserved_2            : 2   ; /* [15..14]  */
        unsigned int    i2c3_cken             : 1   ; /* [16]  */
        unsigned int    i2c3_srst_req         : 1   ; /* [17]  */
        unsigned int    reserved_1            : 2   ; /* [19..18]  */
        unsigned int    i2c4_cken             : 1   ; /* [20]  */
        unsigned int    i2c4_srst_req         : 1   ; /* [21]  */
        unsigned int    reserved_0            : 10  ; /* [31..22]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG27;

/* Define the union U_PERI_CRG28 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    ssp0_cken             : 1   ; /* [0]  */
        unsigned int    ssp0_srst_req         : 1   ; /* [1]  */
        unsigned int    reserved_1            : 2   ; /* [3..2]  */
        unsigned int    ssp1_cken             : 1   ; /* [4]  */
        unsigned int    ssp1_srst_req         : 1   ; /* [5]  */
        unsigned int    reserved_0            : 26  ; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG28;

/* Define the union U_PERI_CRG29 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    sci0_cken             : 1   ; /* [0]  */
        unsigned int    sci0_srst_req         : 1   ; /* [1]  */
        unsigned int    reserved_1            : 2   ; /* [3..2]  */
        unsigned int    sci1_cken             : 1   ; /* [4]  */
        unsigned int    sci1_srst_req         : 1   ; /* [5]  */
        unsigned int    reserved_0            : 26  ; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG29;

/* Define the union U_PERI_CRG30 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    vdh_cken              : 1   ; /* [0]  */
        unsigned int    reserved_3            : 3   ; /* [3..1]  */
        unsigned int    vdh_srst_req          : 1   ; /* [4]  */
        unsigned int    reserved_2            : 3   ; /* [7..5]  */
        unsigned int    vdh_clk_sel           : 1   ; /* [8]  */
        unsigned int    reserved_1            : 3   ; /* [11..9]  */
        unsigned int    vdhclk_skipcfg        : 5   ; /* [16..12]  */
        unsigned int    vdhclk_loaden         : 1   ; /* [17]  */
        unsigned int    reserved_0            : 14  ; /* [31..18]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG30;

/* Define the union U_PERI_CRG31 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    jpgd0_cken            : 1   ; /* [0]  */
        unsigned int    reserved_2            : 3   ; /* [3..1]  */
        unsigned int    jpgd0_srst_req        : 1   ; /* [4]  */
        unsigned int    reserved_1            : 3   ; /* [7..5]  */
        unsigned int    jpgd0_clk_sel         : 2   ; /* [9..8]  */
        unsigned int    reserved_0            : 22  ; /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG31;

/* Define the union U_PERI_CRG32 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    jpgd1_cken            : 1   ; /* [0]  */
        unsigned int    reserved_2            : 3   ; /* [3..1]  */
        unsigned int    jpgd1_srst_req        : 1   ; /* [4]  */
        unsigned int    reserved_1            : 3   ; /* [7..5]  */
        unsigned int    jpgd1_clk_sel         : 2   ; /* [9..8]  */
        unsigned int    reserved_0            : 22  ; /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG32;

/* Define the union U_PERI_CRG33 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    pgd_cken              : 1   ; /* [0]  */
        unsigned int    reserved_1            : 3   ; /* [3..1]  */
        unsigned int    pgd_srst_req          : 1   ; /* [4]  */
        unsigned int    reserved_0            : 27  ; /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG33;

/* Define the union U_PERI_CRG34 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    bpd_cken              : 1   ; /* [0]  */
        unsigned int    reserved_1            : 3   ; /* [3..1]  */
        unsigned int    bpd_srst_req          : 1   ; /* [4]  */
        unsigned int    reserved_0            : 27  ; /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG34;

/* Define the union U_PERI_CRG35 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    venc_cken             : 1   ; /* [0]  */
        unsigned int    reserved_3            : 3   ; /* [3..1]  */
        unsigned int    venc_srst_req         : 1   ; /* [4]  */
        unsigned int    reserved_2            : 3   ; /* [7..5]  */
        unsigned int    venc_clk_sel          : 1   ; /* [8]  */
        unsigned int    venc_clk_sel_bus      : 1   ; /* [9]  */
        unsigned int    reserved_1            : 2   ; /* [11..10]  */
        unsigned int    vencclk_skipcfg       : 5   ; /* [16..12]  */
        unsigned int    vencclk_loaden        : 1   ; /* [17]  */
        unsigned int    reserved_0            : 14  ; /* [31..18]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG35;

/* Define the union U_PERI_CRG36 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    jpge_cken             : 1   ; /* [0]  */
        unsigned int    reserved_2            : 3   ; /* [3..1]  */
        unsigned int    jpge_srst_req         : 1   ; /* [4]  */
        unsigned int    reserved_1            : 3   ; /* [7..5]  */
        unsigned int    jpge_clk_sel          : 2   ; /* [9..8]  */
        unsigned int    jpge_clk_sel_bus      : 1   ; /* [10]  */
        unsigned int    reserved_0            : 21  ; /* [31..11]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG36;

/* Define the union U_PERI_CRG37 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    tde_cken              : 1   ; /* [0]  */
        unsigned int    reserved_2            : 3   ; /* [3..1]  */
        unsigned int    tde_srst_req          : 1   ; /* [4]  */
        unsigned int    reserved_1            : 3   ; /* [7..5]  */
        unsigned int    tde_clk_sel           : 2   ; /* [9..8]  */
        unsigned int    reserved_0            : 22  ; /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG37;

/* Define the union U_PERI_CRG38 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    m3_cken               : 1   ; /* [0]  */
        unsigned int    reserved_2            : 3   ; /* [3..1]  */
        unsigned int    m3_dig_srst_req       : 1   ; /* [4]  */
        unsigned int    m3_core_srst_req      : 1   ; /* [5]  */
        unsigned int    reserved_1            : 2   ; /* [7..6]  */
        unsigned int    m3_clk_sel            : 2   ; /* [9..8]  */
        unsigned int    reserved_0            : 22  ; /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG38;

/* Define the union U_PERI_CRG39 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    sdio0_bus_cken        : 1   ; /* [0]  */
        unsigned int    sdio0_cken            : 1   ; /* [1]  */
        unsigned int    reserved_4            : 2   ; /* [3..2]  */
        unsigned int    sdio0_srst_req        : 1   ; /* [4]  */
        unsigned int    reserved_3            : 3   ; /* [7..5]  */
        unsigned int    sdio0_clk_sel         : 2   ; /* [9..8]  */
        unsigned int    reserved_2            : 2   ; /* [11..10]  */
        unsigned int    sdio0_sap_ps_sel      : 3   ; /* [14..12]  */
        unsigned int    reserved_1            : 1   ; /* [15]  */
        unsigned int    sdio0_drv_ps_sel      : 3   ; /* [18..16]  */
        unsigned int    reserved_0            : 13  ; /* [31..19]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG39;

/* Define the union U_PERI_CRG40 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    sdio1_bus_cken        : 1   ; /* [0]  */
        unsigned int    sdio1_cken            : 1   ; /* [1]  */
        unsigned int    reserved_4            : 2   ; /* [3..2]  */
        unsigned int    sdio1_srst_req        : 1   ; /* [4]  */
        unsigned int    reserved_3            : 3   ; /* [7..5]  */
        unsigned int    sdio1_clk_sel         : 2   ; /* [9..8]  */
        unsigned int    reserved_2            : 2   ; /* [11..10]  */
        unsigned int    sdio1_sap_ps_sel      : 3   ; /* [14..12]  */
        unsigned int    reserved_1            : 1   ; /* [15]  */
        unsigned int    sdio1_drv_ps_sel      : 3   ; /* [18..16]  */
        unsigned int    reserved_0            : 13  ; /* [31..19]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG40;

/* Define the union U_PERI_CRG41 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    dmac_cken             : 1   ; /* [0]  */
        unsigned int    reserved_1            : 3   ; /* [3..1]  */
        unsigned int    dmac_srst_req         : 1   ; /* [4]  */
        unsigned int    reserved_0            : 27  ; /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG41;

/* Define the union U_PERI_CRG42 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    sata_bus_cken         : 1   ; /* [0]  */
        unsigned int    sata_rx0_cken         : 1   ; /* [1]  */
        unsigned int    sata_cko_alive_cken   : 1   ; /* [2]  */
        unsigned int    sata_mpll_dword_cken  : 1   ; /* [3]  */
        unsigned int    sata_refclk_cken      : 1   ; /* [4]  */
        unsigned int    reserved_1            : 3   ; /* [7..5]  */
        unsigned int    sata_bus_srst_req     : 1   ; /* [8]  */
        unsigned int    sata_cko_alive_srst_req : 1   ; /* [9]  */
        unsigned int    reserved_0            : 22  ; /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG42;

/* Define the union U_PERI_CRG43 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    sata_phy_ref_cken     : 1   ; /* [0]  */
        unsigned int    reserved_1            : 7   ; /* [7..1]  */
        unsigned int    sata_phy_refclk_sel   : 1   ; /* [8]  */
        unsigned int    reserved_0            : 23  ; /* [31..9]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG43;

/* Define the union U_PERI_CRG46 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    usb2_bus_cken         : 1   ; /* [0]  */
        unsigned int    usb2_ohci48m_cken     : 1   ; /* [1]  */
        unsigned int    usb2_ohci12m_cken     : 1   ; /* [2]  */
        unsigned int    usb2_otg_utmi_cken    : 1   ; /* [3]  */
        unsigned int    usb2_hst_phy_cken     : 1   ; /* [4]  */
        unsigned int    usb2_utmi0_cken       : 1   ; /* [5]  */
        unsigned int    usb2_utmi1_cken       : 1   ; /* [6]  */
        unsigned int    usb2_utmi2_cken       : 1   ; /* [7]  */
        unsigned int    usb2_adp_cken         : 1   ; /* [8]  */
        unsigned int    reserved_1            : 3   ; /* [11..9]  */
        unsigned int    usb2_bus_srst_req     : 1   ; /* [12]  */
        unsigned int    usb2_utmi0_srst_req   : 1   ; /* [13]  */
        unsigned int    usb2_utmi1_srst_req   : 1   ; /* [14]  */
        unsigned int    usb2_utmi2_srst_req   : 1   ; /* [15]  */
        unsigned int    usb2_hst_phy_srst_req : 1   ; /* [16]  */
        unsigned int    usb2_otg_phy_srst_req : 1   ; /* [17]  */
        unsigned int    usb2_adp_srst_req     : 1   ; /* [18]  */
        unsigned int    reserved_0            : 13  ; /* [31..19]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG46;

/* Define the union U_PERI_CRG47 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    usb2_phy0_ref_cken    : 1   ; /* [0]  */
        unsigned int    usb2_phy1_ref_cken    : 1   ; /* [1]  */
        unsigned int    usb2_phy2_ref_cken    : 1   ; /* [2]  */
        unsigned int    reserved_2            : 5   ; /* [7..3]  */
        unsigned int    usb2_phy0_srst_req    : 1   ; /* [8]  */
        unsigned int    usb2_phy0_srst_treq   : 1   ; /* [9]  */
        unsigned int    usb2_phy1_srst_req    : 1   ; /* [10]  */
        unsigned int    usb2_phy1_srst_treq   : 1   ; /* [11]  */
        unsigned int    usb2_phy2_srst_req    : 1   ; /* [12]  */
        unsigned int    usb2_phy2_srst_treq   : 1   ; /* [13]  */
        unsigned int    reserved_1            : 2   ; /* [15..14]  */
        unsigned int    usb2_phy0_refclk_sel  : 1   ; /* [16]  */
        unsigned int    usb2_phy1_refclk_sel  : 1   ; /* [17]  */
        unsigned int    usb2_phy2_refclk_sel  : 1   ; /* [18]  */
        unsigned int    reserved_0            : 13  ; /* [31..19]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG47;

/* Define the union U_PERI_CRG48 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    ca_kl_bus_cken        : 1   ; /* [0]  */
        unsigned int    ca_ci_bus_cken        : 1   ; /* [1]  */
        unsigned int    ca_ci_cken            : 1   ; /* [2]  */
        unsigned int    otp_bus_cken          : 1   ; /* [3]  */
        unsigned int    reserved_2            : 4   ; /* [7..4]  */
        unsigned int    ca_kl_srst_req        : 1   ; /* [8]  */
        unsigned int    ca_ci_srst_req        : 1   ; /* [9]  */
        unsigned int    otp_srst_req          : 1   ; /* [10]  */
        unsigned int    reserved_1            : 1   ; /* [11]  */
        unsigned int    ca_clk_sel            : 1   ; /* [12]  */
        unsigned int    reserved_0            : 19  ; /* [31..13]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG48;

/* Define the union U_PERI_CRG49 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    sha_cken              : 1   ; /* [0]  */
        unsigned int    reserved_1            : 3   ; /* [3..1]  */
        unsigned int    sha_srst_req          : 1   ; /* [4]  */
        unsigned int    reserved_0            : 27  ; /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG49;

/* Define the union U_PERI_CRG50 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    pmc_cken              : 1   ; /* [0]  */
        unsigned int    reserved_1            : 3   ; /* [3..1]  */
        unsigned int    pmc_srst_req          : 1   ; /* [4]  */
        unsigned int    reserved_0            : 27  ; /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG50;

/* Define the union U_PERI_CRG51 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    gsf_bus_s_cken        : 1   ; /* [0]  */
        unsigned int    gsf_bus_m0_cken       : 1   ; /* [1]  */
        unsigned int    gsf_bus_m1_cken       : 1   ; /* [2]  */
        unsigned int    gsf_ge0_cken          : 1   ; /* [3]  */
        unsigned int    gsf_ge1_cken          : 1   ; /* [4]  */
        unsigned int    gsf_pub_cken          : 1   ; /* [5]  */
        unsigned int    reserved_1            : 2   ; /* [7..6]  */
        unsigned int    gsf_p0_srst_req       : 1   ; /* [8]  */
        unsigned int    gsf_p1_srst_req       : 1   ; /* [9]  */
        unsigned int    gsf_mac0_srst_req     : 1   ; /* [10]  */
        unsigned int    gsf_mac1_srst_req     : 1   ; /* [11]  */
        unsigned int    gsf_rmii0_clk_sel     : 1   ; /* [12]  */
        unsigned int    gsf_rmii1_clk_sel     : 1   ; /* [13]  */
        unsigned int    gsf_rmii0_clk_oen     : 1   ; /* [14]  */
        unsigned int    gsf_rmii1_clk_oen     : 1   ; /* [15]  */
        unsigned int    gsf_rmii2_clk_sel     : 1   ; /* [16]  */
        unsigned int    gsf_rmii2_clk_oen     : 1   ; /* [17]  */
        unsigned int    reserved_0            : 14  ; /* [31..18]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG51;

/* Define the union U_PERI_CRG52 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    dsp_cken              : 1   ; /* [0]  */
        unsigned int    dsp0_cken             : 1   ; /* [1]  */
        unsigned int    reserved_2            : 2   ; /* [3..2]  */
        unsigned int    dsp_srst_req          : 1   ; /* [4]  */
        unsigned int    dsp0_srst_req         : 1   ; /* [5]  */
        unsigned int    reserved_1            : 2   ; /* [7..6]  */
        unsigned int    dsp_clk_sel           : 2   ; /* [9..8]  */
        unsigned int    reserved_0            : 22  ; /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG52;

/* Define the union U_PERI_CRG53 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    gpu_cken              : 1   ; /* [0]  */
        unsigned int    reserved_1            : 3   ; /* [3..1]  */
        unsigned int    gpu_srst_req          : 1   ; /* [4]  */
        unsigned int    reserved_0            : 27  ; /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG53;

/* Define the union U_PERI_CRG54 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    vo_bus_cken           : 1   ; /* [0]  */
        unsigned int    vo_cken               : 1   ; /* [1]  */
        unsigned int    vo_sd_cken            : 1   ; /* [2]  */
        unsigned int    vo_sdate_cken         : 1   ; /* [3]  */
        unsigned int    vo_hd_cken            : 1   ; /* [4]  */
        unsigned int    vo_hdate_cken         : 1   ; /* [5]  */
        unsigned int    vdac_ch0_cken         : 1   ; /* [6]  */
        unsigned int    vdac_ch1_cken         : 1   ; /* [7]  */
        unsigned int    vdac_ch2_cken         : 1   ; /* [8]  */
        unsigned int    vdac_ch3_cken         : 1   ; /* [9]  */
        unsigned int    vo_clkout_cken        : 1   ; /* [10]  */
        unsigned int    reserved_2            : 1   ; /* [11]  */
        unsigned int    vo_sd_clk_sel         : 2   ; /* [13..12]  */
        unsigned int    vo_sd_clk_div         : 2   ; /* [15..14]  */
        unsigned int    vo_hd_clk_sel         : 2   ; /* [17..16]  */
        unsigned int    vo_hd_clk_div         : 2   ; /* [19..18]  */
        unsigned int    vdac_ch0_clk_sel      : 1   ; /* [20]  */
        unsigned int    vdac_ch1_clk_sel      : 1   ; /* [21]  */
        unsigned int    vdac_ch2_clk_sel      : 1   ; /* [22]  */
        unsigned int    vdac_ch3_clk_sel      : 1   ; /* [23]  */
        unsigned int    vo_clkout_sel         : 1   ; /* [24]  */
        unsigned int    vo_clkout_pctrl       : 1   ; /* [25]  */
        unsigned int    hdmi_clk_sel          : 1   ; /* [26]  */
        unsigned int    reserved_1            : 1   ; /* [27]  */
        unsigned int    vdp_clk_sel           : 2   ; /* [29..28]  */
        unsigned int    vou_srst_req          : 1   ; /* [30]  */
        unsigned int    reserved_0            : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG54;

/* Define the union U_PERI_CRG55 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    vi_bus_cken           : 1   ; /* [0]  */
        unsigned int    vi0_cken              : 1   ; /* [1]  */
        unsigned int    reserved_2            : 2   ; /* [3..2]  */
        unsigned int    vi_bus_srst_req       : 1   ; /* [4]  */
        unsigned int    vi0_srst_req          : 1   ; /* [5]  */
        unsigned int    reserved_1            : 2   ; /* [7..6]  */
        unsigned int    vi0_pctrl             : 1   ; /* [8]  */
        unsigned int    vi0_clk_sel           : 1   ; /* [9]  */
        unsigned int    reserved_0            : 22  ; /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG55;

/* Define the union U_PERI_CRG56 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    qtc_bus_cken          : 1   ; /* [0]  */
        unsigned int    qtc_cken              : 1   ; /* [1]  */
        unsigned int    reserved_2            : 2   ; /* [3..2]  */
        unsigned int    qtc_srst_req          : 1   ; /* [4]  */
        unsigned int    reserved_1            : 3   ; /* [7..5]  */
        unsigned int    qtc_clk_pctrl         : 1   ; /* [8]  */
        unsigned int    reserved_0            : 23  ; /* [31..9]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG56;

/* Define the union U_PERI_CRG57 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    qam_2x_cken           : 1   ; /* [0]  */
        unsigned int    qam_1x_cken           : 1   ; /* [1]  */
        unsigned int    qam_j83b_cken         : 1   ; /* [2]  */
        unsigned int    qam_j83ac_cken        : 1   ; /* [3]  */
        unsigned int    qam_ads_cken          : 1   ; /* [4]  */
        unsigned int    qam_ts_cken           : 1   ; /* [5]  */
        unsigned int    qam_tsout_cken        : 1   ; /* [6]  */
        unsigned int    reserved_3            : 1   ; /* [7]  */
        unsigned int    qam_2x_srst_req       : 1   ; /* [8]  */
        unsigned int    qam_1x_srst_req       : 1   ; /* [9]  */
        unsigned int    reserved_2            : 2   ; /* [11..10]  */
        unsigned int    qam_ads_srst_req      : 1   ; /* [12]  */
        unsigned int    qam_ts_srst_req       : 1   ; /* [13]  */
        unsigned int    reserved_1            : 2   ; /* [15..14]  */
        unsigned int    qam_tsout_pctrl       : 1   ; /* [16]  */
        unsigned int    qam_adsclk_pctrl      : 1   ; /* [17]  */
        unsigned int    qam_ts_clk_sel        : 2   ; /* [19..18]  */
        unsigned int    qam_ts_clk_div        : 4   ; /* [23..20]  */
        unsigned int    qam2x_clk_sel         : 1   ; /* [24]  */
        unsigned int    reserved_0            : 7   ; /* [31..25]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG57;

/* Define the union U_PERI_CRG58 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    reserved_1            : 4   ; /* [3..0]  */
        unsigned int    qamadc_clk_div        : 4   ; /* [7..4]  */
        unsigned int    qamadc_clk_sel        : 2   ; /* [9..8]  */
        unsigned int    qamctrl_clk_sel       : 2   ; /* [11..10]  */
        unsigned int    qamctrl_clk_div       : 1   ; /* [12]  */
        unsigned int    reserved_0            : 19  ; /* [31..13]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG58;

/* Define the union U_PERI_CRG59 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    scd_cken              : 1   ; /* [0]  */
        unsigned int    reserved_1            : 3   ; /* [3..1]  */
        unsigned int    scd_srst_req          : 1   ; /* [4]  */
        unsigned int    reserved_0            : 27  ; /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG59;

/* Define the union U_PERI_CRG60 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    vpss_cken             : 1   ; /* [0]  */
        unsigned int    reserved_3            : 3   ; /* [3..1]  */
        unsigned int    vpss_srst_req         : 1   ; /* [4]  */
        unsigned int    reserved_2            : 3   ; /* [7..5]  */
        unsigned int    vpss_clk_sel          : 2   ; /* [9..8]  */
        unsigned int    reserved_1            : 2   ; /* [11..10]  */
        unsigned int    vpssclk_skipcfg       : 5   ; /* [16..12]  */
        unsigned int    vpssclk_loaden        : 1   ; /* [17]  */
        unsigned int    reserved_0            : 14  ; /* [31..18]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG60;

/* Define the union U_PERI_CRG61 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    hwc_cken              : 1   ; /* [0]  */
        unsigned int    reserved_2            : 3   ; /* [3..1]  */
        unsigned int    hwc_srst_req          : 1   ; /* [4]  */
        unsigned int    reserved_1            : 3   ; /* [7..5]  */
        unsigned int    hwc_clk_sel           : 2   ; /* [9..8]  */
        unsigned int    reserved_0            : 22  ; /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG61;

/* Define the union U_PERI_CRG62 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    bootmem_cken          : 1   ; /* [0]  */
        unsigned int    reserved_1            : 3   ; /* [3..1]  */
        unsigned int    bootmem_srst_req      : 1   ; /* [4]  */
        unsigned int    reserved_0            : 27  ; /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG62;

/* Define the union U_PERI_CRG63 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    pvr_bus_cken          : 1   ; /* [0]  */
        unsigned int    pvr_dmx_cken          : 1   ; /* [1]  */
        unsigned int    pvr_27m_cken          : 1   ; /* [2]  */
        unsigned int    pvr_tsi1_cken         : 1   ; /* [3]  */
        unsigned int    pvr_tsi2_cken         : 1   ; /* [4]  */
        unsigned int    pvr_tsi3_cken         : 1   ; /* [5]  */
        unsigned int    pvr_tsi4_cken         : 1   ; /* [6]  */
        unsigned int    pvr_tsi5_cken         : 1   ; /* [7]  */
        unsigned int    pvr_tsi6_cken         : 1   ; /* [8]  */
        unsigned int    pvr_tsi7_cken         : 1   ; /* [9]  */
        unsigned int    pvr_ts0_cken          : 1   ; /* [10]  */
        unsigned int    pvr_ts1_cken          : 1   ; /* [11]  */
        unsigned int    pvr_tsout0_cken       : 1   ; /* [12]  */
        unsigned int    pvr_tsout1_cken       : 1   ; /* [13]  */
        unsigned int    reserved_1            : 2   ; /* [15..14]  */
        unsigned int    pvr_tsi2_pctrl        : 1   ; /* [16]  */
        unsigned int    pvr_tsi3_pctrl        : 1   ; /* [17]  */
        unsigned int    pvr_tsi4_pctrl        : 1   ; /* [18]  */
        unsigned int    pvr_tsi5_pctrl        : 1   ; /* [19]  */
        unsigned int    pvr_tsi6_pctrl        : 1   ; /* [20]  */
        unsigned int    pvr_tsi7_pctrl        : 1   ; /* [21]  */
        unsigned int    pvr_srst_req          : 1   ; /* [22]  */
        unsigned int    reserved_0            : 9   ; /* [31..23]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG63;

/* Define the union U_PERI_CRG64 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    pvr_tsout0_pctrl      : 1   ; /* [0]  */
        unsigned int    pvr_tsout1_pctrl      : 1   ; /* [1]  */
        unsigned int    pvr_dmx_clk_sel       : 2   ; /* [3..2]  */
        unsigned int    sw_dmx_clk_div        : 5   ; /* [8..4]  */
        unsigned int    sw_dmxclk_loaden      : 1   ; /* [9]  */
        unsigned int    pvr_dmx_clkdiv_cfg    : 1   ; /* [10]  */
        unsigned int    reserved_1            : 1   ; /* [11]  */
        unsigned int    pvr_ts0_clk_sel       : 2   ; /* [13..12]  */
        unsigned int    pvr_ts1_clk_sel       : 2   ; /* [15..14]  */
        unsigned int    pvr_ts0_clk_div       : 4   ; /* [19..16]  */
        unsigned int    pvr_ts1_clk_div       : 4   ; /* [23..20]  */
        unsigned int    reserved_0            : 8   ; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG64;

/* Define the union U_PERI_CRG67 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    hdmitx_ctrl_bus_cken  : 1   ; /* [0]  */
        unsigned int    hdmitx_ctrl_cec_cken  : 1   ; /* [1]  */
        unsigned int    hdmitx_ctrl_id_cken   : 1   ; /* [2]  */
        unsigned int    hdmitx_ctrl_mhl_cken  : 1   ; /* [3]  */
        unsigned int    hdmitx_ctrl_os_cken   : 1   ; /* [4]  */
        unsigned int    hdmitx_ctrl_as_cken   : 1   ; /* [5]  */
        unsigned int    reserved_2            : 2   ; /* [7..6]  */
        unsigned int    hdmitx_ctrl_bus_srst_req : 1   ; /* [8]  */
        unsigned int    hdmitx_ctrl_srst_req  : 1   ; /* [9]  */
        unsigned int    reserved_1            : 2   ; /* [11..10]  */
        unsigned int    hdmitx_ctrl_cec_clk_sel : 1   ; /* [12]  */
        unsigned int    hdmitx_ctrl_osclk_sel : 1   ; /* [13]  */
        unsigned int    hdmitx_ctrl_asclk_sel : 1   ; /* [14]  */
        unsigned int    reserved_0            : 17  ; /* [31..15]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG67;

/* Define the union U_PERI_CRG68 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    hdmitx_phy_bus_cken   : 1   ; /* [0]  */
        unsigned int    reserved_1            : 3   ; /* [3..1]  */
        unsigned int    hdmitx_phy_srst_req   : 1   ; /* [4]  */
        unsigned int    reserved_0            : 27  ; /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG68;

/* Define the union U_PERI_CRG69 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    adac_cken             : 1   ; /* [0]  */
        unsigned int    reserved_1            : 3   ; /* [3..1]  */
        unsigned int    adac_srst_req         : 1   ; /* [4]  */
        unsigned int    reserved_0            : 27  ; /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG69;

/* Define the union U_PERI_CRG70 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    aiao_cken             : 1   ; /* [0]  */
        unsigned int    reserved_3            : 3   ; /* [3..1]  */
        unsigned int    aiao_srst_req         : 1   ; /* [4]  */
        unsigned int    reserved_2            : 3   ; /* [7..5]  */
        unsigned int    aiao_clk_sel          : 1   ; /* [8]  */
        unsigned int    reserved_1            : 3   ; /* [11..9]  */
        unsigned int    aiaoclk_skipcfg       : 5   ; /* [16..12]  */
        unsigned int    aiaoclk_loaden        : 1   ; /* [17]  */
        unsigned int    reserved_0            : 14  ; /* [31..18]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG70;

/* Define the union U_PERI_CRG71 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    vdac_rct_cken         : 1   ; /* [0]  */
        unsigned int    vdac_bg_cken          : 1   ; /* [1]  */
        unsigned int    reserved_3            : 2   ; /* [3..2]  */
        unsigned int    vdac_c_srst_req       : 1   ; /* [4]  */
        unsigned int    vdac_r_srst_req       : 1   ; /* [5]  */
        unsigned int    vdac_g_srst_req       : 1   ; /* [6]  */
        unsigned int    vdac_b_srst_req       : 1   ; /* [7]  */
        unsigned int    reserved_2            : 4   ; /* [11..8]  */
        unsigned int    vdac_bg_clk_div       : 2   ; /* [13..12]  */
        unsigned int    reserved_1            : 2   ; /* [15..14]  */
        unsigned int    vdac_c_clk_pctrl      : 1   ; /* [16]  */
        unsigned int    vdac_r_clk_pctrl      : 1   ; /* [17]  */
        unsigned int    vdac_g_clk_pctrl      : 1   ; /* [18]  */
        unsigned int    vdac_b_clk_pctrl      : 1   ; /* [19]  */
        unsigned int    reserved_0            : 12  ; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG71;

/* Define the union U_PERI_CRG72 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    fephy_cken            : 1   ; /* [0]  */
        unsigned int    reserved_2            : 3   ; /* [3..1]  */
        unsigned int    fephy_srst_req        : 1   ; /* [4]  */
        unsigned int    reserved_1            : 3   ; /* [7..5]  */
        unsigned int    fephy_clk_sel         : 1   ; /* [8]  */
        unsigned int    reserved_0            : 23  ; /* [31..9]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG72;

/* Define the union U_PERI_CRG73 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    gpu_freq_sel_cfg_crg  : 3   ; /* [2..0]  */
        unsigned int    reserved_2            : 1   ; /* [3]  */
        unsigned int    gpu_freq_div_cfg_crg  : 2   ; /* [5..4]  */
        unsigned int    reserved_1            : 2   ; /* [7..6]  */
        unsigned int    gpu_div_cfg_bypass    : 1   ; /* [8]  */
        unsigned int    gpu_begin_cfg_bypass  : 1   ; /* [9]  */
        unsigned int    gpu_sw_begin_cfg      : 1   ; /* [10]  */
        unsigned int    reserved_0            : 21  ; /* [31..11]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG73;

/* Define the union U_PERI_CRG74 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    ddr_freq_sel_cfg_crg  : 3   ; /* [2..0]  */
        unsigned int    reserved_2            : 1   ; /* [3]  */
        unsigned int    ddr_freq_div_cfg_crg  : 2   ; /* [5..4]  */
        unsigned int    reserved_1            : 2   ; /* [7..6]  */
        unsigned int    ddr_div_cfg_bypass    : 1   ; /* [8]  */
        unsigned int    ddr_begin_cfg_bypass  : 1   ; /* [9]  */
        unsigned int    ddr_sw_begin_cfg      : 1   ; /* [10]  */
        unsigned int    reserved_0            : 21  ; /* [31..11]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG74;

/* Define the union U_PERI_CRG75 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    apll_ssmod_ctrl       : 12  ; /* [11..0]  */
        unsigned int    reserved_0            : 20  ; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG75;

/* Define the union U_PERI_CRG76 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    bpll_ssmod_ctrl       : 12  ; /* [11..0]  */
        unsigned int    reserved_0            : 20  ; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG76;

/* Define the union U_PERI_CRG77 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    dpll_ssmod_ctrl       : 12  ; /* [11..0]  */
        unsigned int    reserved_0            : 20  ; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG77;

/* Define the union U_PERI_CRG78 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    wpll_ssmod_ctrl       : 12  ; /* [11..0]  */
        unsigned int    reserved_0            : 20  ; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG78;

/* Define the union U_PERI_CRG79 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    vpll_ssmod_ctrl       : 12  ; /* [11..0]  */
        unsigned int    reserved_0            : 20  ; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG79;

/* Define the union U_PERI_CRG80 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    hpll_ssmod_ctrl       : 12  ; /* [11..0]  */
        unsigned int    reserved_0            : 20  ; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG80;

/* Define the union U_PERI_CRG81 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    epll_ssmod_ctrl       : 12  ; /* [11..0]  */
        unsigned int    reserved_0            : 20  ; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG81;

/* Define the union U_PERI_CRG82 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    mpll_ssmod_ctrl       : 12  ; /* [11..0]  */
        unsigned int    reserved_0            : 20  ; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG82;

/* Define the union U_PERI_CRG83 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    qpll_ssmod_ctrl       : 12  ; /* [11..0]  */
        unsigned int    reserved_0            : 20  ; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG83;

/* Define the union U_PERI_CRG84 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    pll_lock              : 9   ; /* [8..0]  */
        unsigned int    reserved_0            : 23  ; /* [31..9]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG84;

/* Define the union U_PERI_CRG85 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    cpu_clk_sw_ok_crg     : 1   ; /* [0]  */
        unsigned int    cpu_clk_mux           : 3   ; /* [3..1]  */
        unsigned int    gpu_clk_sw_ok_crg     : 1   ; /* [4]  */
        unsigned int    gpu_clk_mux           : 3   ; /* [7..5]  */
        unsigned int    ddr_clk_sw_ok_crg     : 1   ; /* [8]  */
        unsigned int    ddr_clk_mux           : 3   ; /* [11..9]  */
        unsigned int    core_bus_clk_seled    : 2   ; /* [13..12]  */
        unsigned int    sfc_clk_seled         : 1   ; /* [14]  */
        unsigned int    nf_clk_seled          : 1   ; /* [15]  */
        unsigned int    sdio0_clk_seled       : 2   ; /* [17..16]  */
        unsigned int    sdio1_clk_seled       : 2   ; /* [19..18]  */
        unsigned int    mde0_clk_seled        : 2   ; /* [21..20]  */
        unsigned int    mde1_clk_seled        : 2   ; /* [23..22]  */
        unsigned int    mde2_clk_seled        : 2   ; /* [25..24]  */
        unsigned int    mde3_clk_seled        : 2   ; /* [27..26]  */
        unsigned int    ddrphy_clk_seled      : 1   ; /* [28]  */
        unsigned int    reserved_0            : 3   ; /* [31..29]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG85;

/* Define the union U_PERI_CRG86 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    cpu_pll_cfg0_sw       : 31  ; /* [30..0]  */
        unsigned int    reserved_0            : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG86;

/* Define the union U_PERI_CRG87 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    cpu_pll_cfg1_sw       : 27  ; /* [26..0]  */
        unsigned int    reserved_0            : 5   ; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG87;

/* Define the union U_PERI_CRG88 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    ddr_pll_cfg0_sw       : 31  ; /* [30..0]  */
        unsigned int    reserved_0            : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG88;

/* Define the union U_PERI_CRG89 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    ddr_pll_cfg1_sw       : 27  ; /* [26..0]  */
        unsigned int    reserved_0            : 5   ; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG89;

/* Define the union U_PERI_CRG90 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    slic_rst_out          : 1   ; /* [0]  */
        unsigned int    fe_rstn_out           : 1   ; /* [1]  */
        unsigned int    dem_rst0_out          : 1   ; /* [2]  */
        unsigned int    dem_rst2_out          : 1   ; /* [3]  */
        unsigned int    reserved_0            : 28  ; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG90;

/* Define the union U_PERI_CRG91 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    usb2_phy0_cnt_out     : 1   ; /* [0]  */
        unsigned int    usb2_phy1_cnt_out     : 1   ; /* [1]  */
        unsigned int    usb2_phy2_cnt_out     : 1   ; /* [2]  */
        unsigned int    reserved_0            : 29  ; /* [31..3]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG91;

/* Define the union U_PERI_CRG92 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    btl_cken              : 1   ; /* [0]  */
        unsigned int    reserved_1            : 3   ; /* [3..1]  */
        unsigned int    btl_srst_req          : 1   ; /* [4]  */
        unsigned int    reserved_0            : 27  ; /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG92;

/* Define the union U_PERI_CRG93 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    vdh_srst_ok           : 1   ; /* [0]  */
        unsigned int    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG93;

/* Define the union U_PERI_CRG94 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    wdg0_cken             : 1   ; /* [0]  */
        unsigned int    wdg1_cken             : 1   ; /* [1]  */
        unsigned int    wdg2_cken             : 1   ; /* [2]  */
        unsigned int    wdg3_cken             : 1   ; /* [3]  */
        unsigned int    wdg0_srst_req         : 1   ; /* [4]  */
        unsigned int    wdg1_srst_req         : 1   ; /* [5]  */
        unsigned int    wdg2_srst_req         : 1   ; /* [6]  */
        unsigned int    wdg3_srst_req         : 1   ; /* [7]  */
        unsigned int    reserved_0            : 24  ; /* [31..8]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG94;

/* Define the union U_PERI_CRG95 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    pll_test_en           : 1   ; /* [0]  */
        unsigned int    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CRG95;

//==============================================================================
/* Define the global struct */
typedef struct
{
    volatile U_PERI_CRG0            PERI_CRG0                ; /* 0x0 */
    volatile U_PERI_CRG1            PERI_CRG1                ; /* 0x4 */
    volatile U_PERI_CRG2            PERI_CRG2                ; /* 0x8 */
    volatile U_PERI_CRG3            PERI_CRG3                ; /* 0xc */
    volatile U_PERI_CRG4            PERI_CRG4                ; /* 0x10 */
    volatile U_PERI_CRG5            PERI_CRG5                ; /* 0x14 */
    volatile U_PERI_CRG6            PERI_CRG6                ; /* 0x18 */
    volatile U_PERI_CRG7            PERI_CRG7                ; /* 0x1c */
    volatile U_PERI_CRG8            PERI_CRG8                ; /* 0x20 */
    volatile U_PERI_CRG9            PERI_CRG9                ; /* 0x24 */
    volatile U_PERI_CRG10           PERI_CRG10               ; /* 0x28 */
    volatile U_PERI_CRG11           PERI_CRG11               ; /* 0x2c */
    volatile U_PERI_CRG12           PERI_CRG12               ; /* 0x30 */
    volatile U_PERI_CRG13           PERI_CRG13               ; /* 0x34 */
    volatile U_PERI_CRG14           PERI_CRG14               ; /* 0x38 */
    volatile U_PERI_CRG15           PERI_CRG15               ; /* 0x3c */
    volatile U_PERI_CRG16           PERI_CRG16               ; /* 0x40 */
    volatile U_PERI_CRG17           PERI_CRG17               ; /* 0x44 */
    volatile U_PERI_CRG18           PERI_CRG18               ; /* 0x48 */
    volatile U_PERI_CRG19           PERI_CRG19               ; /* 0x4c */
    volatile U_PERI_CRG20           PERI_CRG20               ; /* 0x50 */
    volatile U_PERI_CRG21           PERI_CRG21               ; /* 0x54 */
    volatile U_PERI_CRG22           PERI_CRG22               ; /* 0x58 */
    volatile U_PERI_CRG23           PERI_CRG23               ; /* 0x5c */
    volatile U_PERI_CRG24           PERI_CRG24               ; /* 0x60 */
    volatile U_PERI_CRG25           PERI_CRG25               ; /* 0x64 */
    volatile U_PERI_CRG26           PERI_CRG26               ; /* 0x68 */
    volatile U_PERI_CRG27           PERI_CRG27               ; /* 0x6c */
    volatile U_PERI_CRG28           PERI_CRG28               ; /* 0x70 */
    volatile U_PERI_CRG29           PERI_CRG29               ; /* 0x74 */
    volatile U_PERI_CRG30           PERI_CRG30               ; /* 0x78 */
    volatile U_PERI_CRG31           PERI_CRG31               ; /* 0x7c */
    volatile U_PERI_CRG32           PERI_CRG32               ; /* 0x80 */
    volatile U_PERI_CRG33           PERI_CRG33               ; /* 0x84 */
    volatile U_PERI_CRG34           PERI_CRG34               ; /* 0x88 */
    volatile U_PERI_CRG35           PERI_CRG35               ; /* 0x8c */
    volatile U_PERI_CRG36           PERI_CRG36               ; /* 0x90 */
    volatile U_PERI_CRG37           PERI_CRG37               ; /* 0x94 */
    volatile U_PERI_CRG38           PERI_CRG38               ; /* 0x98 */
    volatile U_PERI_CRG39           PERI_CRG39               ; /* 0x9c */
    volatile U_PERI_CRG40           PERI_CRG40               ; /* 0xa0 */
    volatile U_PERI_CRG41           PERI_CRG41               ; /* 0xa4 */
    volatile U_PERI_CRG42           PERI_CRG42               ; /* 0xa8 */
    volatile U_PERI_CRG43           PERI_CRG43               ; /* 0xac */
    volatile unsigned int           reserved_0[2]            ; /* 0xb0~0xb4 */
    volatile U_PERI_CRG46           PERI_CRG46               ; /* 0xb8 */
    volatile U_PERI_CRG47           PERI_CRG47               ; /* 0xbc */
    volatile U_PERI_CRG48           PERI_CRG48               ; /* 0xc0 */
    volatile U_PERI_CRG49           PERI_CRG49               ; /* 0xc4 */
    volatile U_PERI_CRG50           PERI_CRG50               ; /* 0xc8 */
    volatile U_PERI_CRG51           PERI_CRG51               ; /* 0xcc */
    volatile U_PERI_CRG52           PERI_CRG52               ; /* 0xd0 */
    volatile U_PERI_CRG53           PERI_CRG53               ; /* 0xd4 */
    volatile U_PERI_CRG54           PERI_CRG54               ; /* 0xd8 */
    volatile U_PERI_CRG55           PERI_CRG55               ; /* 0xdc */
    volatile U_PERI_CRG56           PERI_CRG56               ; /* 0xe0 */
    volatile U_PERI_CRG57           PERI_CRG57               ; /* 0xe4 */
    volatile U_PERI_CRG58           PERI_CRG58               ; /* 0xe8 */
    volatile U_PERI_CRG59           PERI_CRG59               ; /* 0xec */
    volatile U_PERI_CRG60           PERI_CRG60               ; /* 0xf0 */
    volatile U_PERI_CRG61           PERI_CRG61               ; /* 0xf4 */
    volatile U_PERI_CRG62           PERI_CRG62               ; /* 0xf8 */
    volatile U_PERI_CRG63           PERI_CRG63               ; /* 0xfc */
    volatile U_PERI_CRG64           PERI_CRG64               ; /* 0x100 */
    volatile unsigned int           reserved_1[2]            ; /* 0x104~0x108 */
    volatile U_PERI_CRG67           PERI_CRG67               ; /* 0x10c */
    volatile U_PERI_CRG68           PERI_CRG68               ; /* 0x110 */
    volatile U_PERI_CRG69           PERI_CRG69               ; /* 0x114 */
    volatile U_PERI_CRG70           PERI_CRG70               ; /* 0x118 */
    volatile U_PERI_CRG71           PERI_CRG71               ; /* 0x11c */
    volatile U_PERI_CRG72           PERI_CRG72               ; /* 0x120 */
    volatile U_PERI_CRG73           PERI_CRG73               ; /* 0x124 */
    volatile U_PERI_CRG74           PERI_CRG74               ; /* 0x128 */
    volatile U_PERI_CRG75           PERI_CRG75               ; /* 0x12c */
    volatile U_PERI_CRG76           PERI_CRG76               ; /* 0x130 */
    volatile U_PERI_CRG77           PERI_CRG77               ; /* 0x134 */
    volatile U_PERI_CRG78           PERI_CRG78               ; /* 0x138 */
    volatile U_PERI_CRG79           PERI_CRG79               ; /* 0x13c */
    volatile U_PERI_CRG80           PERI_CRG80               ; /* 0x140 */
    volatile U_PERI_CRG81           PERI_CRG81               ; /* 0x144 */
    volatile U_PERI_CRG82           PERI_CRG82               ; /* 0x148 */
    volatile U_PERI_CRG83           PERI_CRG83               ; /* 0x14c */
    volatile U_PERI_CRG84           PERI_CRG84               ; /* 0x150 */
    volatile U_PERI_CRG85           PERI_CRG85               ; /* 0x154 */
    volatile U_PERI_CRG86           PERI_CRG86               ; /* 0x158 */
    volatile U_PERI_CRG87           PERI_CRG87               ; /* 0x15c */
    volatile U_PERI_CRG88           PERI_CRG88               ; /* 0x160 */
    volatile U_PERI_CRG89           PERI_CRG89               ; /* 0x164 */
    volatile U_PERI_CRG90           PERI_CRG90               ; /* 0x168 */
    volatile U_PERI_CRG91           PERI_CRG91               ; /* 0x16c */
    volatile U_PERI_CRG92           PERI_CRG92               ; /* 0x170 */
    volatile U_PERI_CRG93           PERI_CRG93               ; /* 0x174 */
    volatile U_PERI_CRG94           PERI_CRG94               ; /* 0x178 */
    volatile U_PERI_CRG95           PERI_CRG95               ; /* 0x17c */
} S_CRG_REGS_TYPE;

#endif /* __HI_REG_CRG_H__ */

