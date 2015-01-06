//******************************************************************************
// Copyright     :  Copyright (C) 2013, Hisilicon Technologies Co., Ltd.
// File name     :  hi_reg_sys.h
// Author        :
// Version       :  1.0
// Date          :  2013-08-27
// Description   :  The C union definition file for the module sysctrl
// Others        :  Generated automatically by nManager V4.0
//******************************************************************************

#ifndef __HI_REG_SYS_H__
#define __HI_REG_SYS_H__

/* Define the union U_SC_CTRL */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    mcu_bus_clk_sel       : 2   ; /* [1..0]  */
        unsigned int    mcu_bus_clk_sel_stat  : 2   ; /* [3..2]  */
        unsigned int    mcu_bus_clk_div       : 2   ; /* [5..4]  */
        unsigned int    reserved_1            : 2   ; /* [7..6]  */
        unsigned int    remapclear            : 1   ; /* [8]  */
        unsigned int    remapstat             : 1   ; /* [9]  */
        unsigned int    reserved_0            : 22  ; /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_SC_CTRL;

/* Define the union U_SC_LOW_POWER_CTRL */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    reserved_3            : 1   ; /* [0]  */
        unsigned int    stb_poweroff          : 1   ; /* [1]  */
        unsigned int    reserved_2            : 1   ; /* [2]  */
        unsigned int    mcu_lp_subsys_iso     : 1   ; /* [3]  */
        unsigned int    reserved_1            : 2   ; /* [5..4]  */
        unsigned int    bus_core_pd_idlereq   : 1   ; /* [6]  */
        unsigned int    bus_core_pd_idle      : 1   ; /* [7]  */
        unsigned int    bus_core_pd_idleack   : 1   ; /* [8]  */
        unsigned int    reserved_0            : 23  ; /* [31..9]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_SC_LOW_POWER_CTRL;

/* Define the union U_SC_IO_REUSE_SEL */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    key0_gpio_sel         : 2   ; /* [1..0]  */
        unsigned int    reserved_1            : 2   ; /* [3..2]  */
        unsigned int    csn1_gpio_sel         : 3   ; /* [6..4]  */
        unsigned int    csn2_gpio_sel         : 2   ; /* [8..7]  */
        unsigned int    csn3_gpio_sel         : 2   ; /* [10..9]  */
        unsigned int    clk_gpio_sel          : 2   ; /* [12..11]  */
        unsigned int    data_gpio_sel         : 2   ; /* [14..13]  */
        unsigned int    ir_gpio_sel           : 1   ; /* [15]  */
        unsigned int    reserved_0            : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_SC_IO_REUSE_SEL;

/* Define the union U_SC_CLKGATE_SRST_CTRL */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    mce_cken              : 1   ; /* [0]  */
        unsigned int    mce_srst_req          : 1   ; /* [1]  */
        unsigned int    reserved_5            : 2   ; /* [3..2]  */
        unsigned int    ir_cken               : 1   ; /* [4]  */
        unsigned int    ir_srst_req           : 1   ; /* [5]  */
        unsigned int    reserved_4            : 2   ; /* [7..6]  */
        unsigned int    led_cken              : 1   ; /* [8]  */
        unsigned int    led_srst_req          : 1   ; /* [9]  */
        unsigned int    reserved_3            : 2   ; /* [11..10]  */
        unsigned int    uart_cken             : 1   ; /* [12]  */
        unsigned int    uart_srst_req         : 1   ; /* [13]  */
        unsigned int    reserved_2            : 2   ; /* [15..14]  */
        unsigned int    ssp_cken              : 1   ; /* [16]  */
        unsigned int    ssp_srst_req          : 1   ; /* [17]  */
        unsigned int    reserved_1            : 10  ; /* [27..18]  */
        unsigned int    pd_rst_req            : 1   ; /* [28]  */
        unsigned int    reserved_0            : 3   ; /* [31..29]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_SC_CLKGATE_SRST_CTRL;

/* Define the union U_SC_CA_RST_CTRL */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    ca_rst_ctrl           : 1   ; /* [0]  */
        unsigned int    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_SC_CA_RST_CTRL;

/* Define the union U_SC_WDG_RST_CTRL */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    wdg_rst_ctrl          : 1   ; /* [0]  */
        unsigned int    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_SC_WDG_RST_CTRL;

/* Define the union U_SC_DDRPHY_LP_EN */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    ddrphy_lp_en          : 2   ; /* [1..0]  */
        unsigned int    reserved_0            : 30  ; /* [31..2]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_SC_DDRPHY_LP_EN;

/* Define the union U_SC_MCU_HPM_CTRL */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    mcu_hpm_div           : 6   ; /* [5..0]  */
        unsigned int    mcu_hpm_en            : 1   ; /* [6]  */
        unsigned int    mcu_hpm_rst_req       : 1   ; /* [7]  */
        unsigned int    reserved_0            : 24  ; /* [31..8]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_SC_MCU_HPM_CTRL;

/* Define the union U_SC_MCU_HPM_STAT */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    mcu_hpm_pc_org        : 10  ; /* [9..0]  */
        unsigned int    mcu_hpm_valid         : 1   ; /* [10]  */
        unsigned int    reserved_0            : 21  ; /* [31..11]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_SC_MCU_HPM_STAT;

/* Define the union U_SC_MCU_LDO_CTRL */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    mcu_ldo_vset          : 4   ; /* [3..0]  */
        unsigned int    reserved_0            : 28  ; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_SC_MCU_LDO_CTRL;

/* Define the union U_SC_GPIO_OD_CTRL */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    gpio5_0_od_sel        : 1   ; /* [0]  */
        unsigned int    gpio5_1_od_sel        : 1   ; /* [1]  */
        unsigned int    gpio5_2_od_sel        : 1   ; /* [2]  */
        unsigned int    gpio5_3_od_sel        : 1   ; /* [3]  */
        unsigned int    gpio5_4_od_sel        : 1   ; /* [4]  */
        unsigned int    reserved_1            : 2   ; /* [6..5]  */
        unsigned int    gpio5_7_od_sel        : 1   ; /* [7]  */
        unsigned int    core_pwr_active       : 1   ; /* [8]  */
        unsigned int    reserved_0            : 23  ; /* [31..9]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_SC_GPIO_OD_CTRL;

//==============================================================================
/* Define the global struct */
typedef struct
{
    volatile U_SC_CTRL              SC_CTRL                  ; /* 0x0 */
    volatile unsigned int           SC_SYSRES                ; /* 0x4 */
    volatile unsigned int           reserved_0[14]           ; /* 0x8~0x3c */
    volatile U_SC_LOW_POWER_CTRL    SC_LOW_POWER_CTRL        ; /* 0x40 */
    volatile U_SC_IO_REUSE_SEL      SC_IO_REUSE_SEL          ; /* 0x44 */
    volatile U_SC_CLKGATE_SRST_CTRL   SC_CLKGATE_SRST_CTRL   ; /* 0x48 */
    volatile U_SC_CA_RST_CTRL       SC_CA_RST_CTRL           ; /* 0x4c */
    volatile U_SC_WDG_RST_CTRL      SC_WDG_RST_CTRL          ; /* 0x50 */
    volatile unsigned int           reserved_1               ; /* 0x54 */
    volatile U_SC_DDRPHY_LP_EN      SC_DDRPHY_LP_EN          ; /* 0x58 */
    volatile unsigned int           reserved_2               ; /* 0x5c */
    volatile U_SC_MCU_HPM_CTRL      SC_MCU_HPM_CTRL          ; /* 0x60 */
    volatile U_SC_MCU_HPM_STAT      SC_MCU_HPM_STAT          ; /* 0x64 */
    volatile U_SC_MCU_LDO_CTRL      SC_MCU_LDO_CTRL          ; /* 0x68 */
    volatile unsigned int           reserved_3[5]            ; /* 0x6c~0x7c */
    volatile unsigned int           SC_GEN0                  ; /* 0x80 */
    volatile unsigned int           SC_GEN1                  ; /* 0x84 */
    volatile unsigned int           SC_GEN2                  ; /* 0x88 */
    volatile unsigned int           SC_GEN3                  ; /* 0x8c */
    volatile unsigned int           SC_GEN4                  ; /* 0x90 */
    volatile unsigned int           SC_GEN5                  ; /* 0x94 */
    volatile unsigned int           SC_GEN6                  ; /* 0x98 */
    volatile unsigned int           SC_GEN7                  ; /* 0x9c */
    volatile unsigned int           SC_GEN8                  ; /* 0xa0 */
    volatile unsigned int           SC_GEN9                  ; /* 0xa4 */
    volatile unsigned int           SC_GEN10                 ; /* 0xa8 */
    volatile unsigned int           SC_GEN11                 ; /* 0xac */
    volatile unsigned int           SC_GEN12                 ; /* 0xb0 */
    volatile unsigned int           SC_GEN13                 ; /* 0xb4 */
    volatile unsigned int           SC_GEN14                 ; /* 0xb8 */
    volatile unsigned int           SC_GEN15                 ; /* 0xbc */
    volatile unsigned int           SC_GEN16                 ; /* 0xc0 */
    volatile unsigned int           SC_GEN17                 ; /* 0xc4 */
    volatile unsigned int           SC_GEN18                 ; /* 0xc8 */
    volatile unsigned int           SC_GEN19                 ; /* 0xcc */
    volatile unsigned int           SC_GEN20                 ; /* 0xd0 */
    volatile unsigned int           SC_GEN21                 ; /* 0xd4 */
    volatile unsigned int           SC_GEN22                 ; /* 0xd8 */
    volatile unsigned int           SC_GEN23                 ; /* 0xdc */
    volatile unsigned int           SC_GEN24                 ; /* 0xe0 */
    volatile unsigned int           SC_GEN25                 ; /* 0xe4 */
    volatile unsigned int           SC_GEN26                 ; /* 0xe8 */
    volatile unsigned int           SC_GEN27                 ; /* 0xec */
    volatile unsigned int           SC_GEN28                 ; /* 0xf0 */
    volatile unsigned int           SC_GEN29                 ; /* 0xf4 */
    volatile unsigned int           SC_GEN30                 ; /* 0xf8 */
    volatile unsigned int           SC_GEN31                 ; /* 0xfc */
    volatile U_SC_GPIO_OD_CTRL      SC_GPIO_OD_CTRL          ; /* 0x100 */
    volatile unsigned int           reserved_4[66]           ; /* 0x104~0x208 */
    volatile unsigned int           SC_LOCKEN                ; /* 0x20c */
    volatile unsigned int           reserved_5[820]          ; /* 0x210~0xedc */
    volatile unsigned int           SC_SYSID                 ; /* 0xee0 */
} S_SYSCTRL_REGS_TYPE;

#endif /* __HI_REG_SYS_H__ */

