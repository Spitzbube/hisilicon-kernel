//******************************************************************************
// Copyright     :  Copyright (C) 2013, Hisilicon Technologies Co., Ltd.
// File name     :  hi_reg_peri.h
// Author        :
// Version       :  1.0
// Date          :  2013-08-27
// Description   :  The C union definition file for the module perictrl
// Others        :  Generated automatically by nManager V4.0
//******************************************************************************

#ifndef __HI_REG_PERI_H__
#define __HI_REG_PERI_H__

/* Define the union U_START_MODE */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    reserved_7            : 7   ; /* [6..0]  */
        unsigned int    romboot_sel_lock      : 1   ; /* [7]  */
        unsigned int    reserved_6            : 1   ; /* [8]  */
        unsigned int    boot_sel              : 2   ; /* [10..9]  */
        unsigned int    sfc_addr_mode         : 1   ; /* [11]  */
        unsigned int    nf_bootbw             : 1   ; /* [12]  */
        unsigned int    reserved_5            : 1   ; /* [13]  */
        unsigned int    reserved_4            : 4   ; /* [17..14]  */
        unsigned int    reserved_3            : 1   ; /* [18]  */
        unsigned int    reserved_2            : 1   ; /* [19]  */
        unsigned int    romboot_sel           : 1   ; /* [20]  */
        unsigned int    reserved_1            : 1   ; /* [21]  */
        unsigned int    por_sel               : 1   ; /* [22]  */
        unsigned int    jtag_sel              : 1   ; /* [23]  */
        unsigned int    reserved_0            : 8   ; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_START_MODE;

/* Define the union U_PERI_STAT */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    reserved_1            : 1   ; /* [0]  */
        unsigned int    jtag_mux_io           : 2   ; /* [2..1]  */
        unsigned int    reserved_0            : 29  ; /* [31..3]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_STAT;

/* Define the union U_PERI_CTRL */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    sdio1_card_det_mode   : 1   ; /* [0]  */
        unsigned int    sdio0_card_det_mode   : 1   ; /* [1]  */
        unsigned int    reserved_2            : 2   ; /* [3..2]  */
        unsigned int    ssp0_cs_sel           : 1   ; /* [4]  */
        unsigned int    reserved_1            : 3   ; /* [7..5]  */
        unsigned int    net_if0_sel           : 1   ; /* [8]  */
        unsigned int    net_if1_sel           : 1   ; /* [9]  */
        unsigned int    sata0_pwren           : 1   ; /* [10]  */
        unsigned int    reserved_0            : 18  ; /* [28..11]  */
        unsigned int    peri_jtag_sel         : 3   ; /* [31..29]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CTRL;

/* Define the union U_CPU_STAT */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    core0_smpnamp         : 1   ; /* [0]  */
        unsigned int    core1_smpnamp         : 1   ; /* [1]  */
        unsigned int    reserved_0            : 30  ; /* [31..2]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_CPU_STAT;

/* Define the union U_CPU_SET */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    core0_cfgnmfi         : 1   ; /* [0]  */
        unsigned int    core1_cfgnmfi         : 1   ; /* [1]  */
        unsigned int    reserved_0            : 30  ; /* [31..2]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_CPU_SET;

/* Define the union U_NF_CFG */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    reserved_2            : 3   ; /* [2..0]  */
        unsigned int    nf_blksize            : 2   ; /* [4..3]  */
        unsigned int    nf_page               : 3   ; /* [7..5]  */
        unsigned int    nf_adnum              : 1   ; /* [8]  */
        unsigned int    nf_randomizer_pad     : 1   ; /* [9]  */
        unsigned int    sync_nand             : 1   ; /* [10]  */
        unsigned int    nf_ecc_type           : 4   ; /* [14..11]  */
        unsigned int    reserved_1            : 5   ; /* [19..15]  */
        unsigned int    nf_boot_cfg_lock      : 6   ; /* [25..20]  */
        unsigned int    reserved_0            : 2   ; /* [27..26]  */
        unsigned int    pmc_gpu_core0_mtcoms_ack : 1   ; /* [28]  */
        unsigned int    pmc_gpu_core1_mtcoms_ack : 1   ; /* [29]  */
        unsigned int    pmc_gpu_core2_mtcoms_ack : 1   ; /* [30]  */
        unsigned int    pmc_gpu_core3_mtcoms_ack : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_NF_CFG;

/* Define the union U_PERI_SEC_STAT */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    a9mp_pmusecure        : 2   ; /* [1..0]  */
        unsigned int    reserved_1            : 2   ; /* [3..2]  */
        unsigned int    a9mp_pmupriv          : 2   ; /* [5..4]  */
        unsigned int    reserved_0            : 26  ; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_SEC_STAT;

/* Define the union U_PERI_QOS_GLOB_CTRL */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    ddrc_qos_way          : 1   ; /* [0]  */
        unsigned int    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_QOS_GLOB_CTRL;

/* Define the union U_PERI_USB_RESUME_INT_MASK */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    usb_phy0_suspend_int_mask : 1   ; /* [0]  */
        unsigned int    reserved_1            : 1   ; /* [1]  */
        unsigned int    usb_phy2_suspend_int_mask : 1   ; /* [2]  */
        unsigned int    usb_phy1_suspend_int_mask : 1   ; /* [3]  */
        unsigned int    reserved_0            : 28  ; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_USB_RESUME_INT_MASK;

/* Define the union U_PERI_USB_RESUME_INT_RAWSTAT */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    usb_phy0_suspend_int_rawstat : 1   ; /* [0]  */
        unsigned int    reserved_1            : 1   ; /* [1]  */
        unsigned int    usb_phy2_suspend_int_rawstat : 1   ; /* [2]  */
        unsigned int    usb_phy1_suspend_int_rawstat : 1   ; /* [3]  */
        unsigned int    reserved_0            : 28  ; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_USB_RESUME_INT_RAWSTAT;

/* Define the union U_PERI_USB_RESUME_INT_STAT */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    usb_phy0_suspend_int_stat : 1   ; /* [0]  */
        unsigned int    reserved_1            : 1   ; /* [1]  */
        unsigned int    usb_phy2_suspend_int_stat : 1   ; /* [2]  */
        unsigned int    usb_phy1_suspend_int_stat : 1   ; /* [3]  */
        unsigned int    reserved_0            : 28  ; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_USB_RESUME_INT_STAT;

/* Define the union U_PERI_INT_A9TOMCE */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    int_a9tomce           : 1   ; /* [0]  */
        unsigned int    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_INT_A9TOMCE;

/* Define the union U_PERI_INT_A9TODSP0 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    int_swi_a9todsp0      : 1   ; /* [0]  */
        unsigned int    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_INT_A9TODSP0;

/* Define the union U_PERI_INT_DSP0TOA9 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    int_swi_dsp0toa9      : 1   ; /* [0]  */
        unsigned int    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_INT_DSP0TOA9;

/* Define the union U_PERI_INT_SWI0 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    int_swi0              : 1   ; /* [0]  */
        unsigned int    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_INT_SWI0;

/* Define the union U_PERI_INT_SWI1 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    int_swi1              : 1   ; /* [0]  */
        unsigned int    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_INT_SWI1;

/* Define the union U_PERI_INT_SWI2 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    int_swi2              : 1   ; /* [0]  */
        unsigned int    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_INT_SWI2;

/* Define the union U_PERI_INT_SWI0_MASK */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    int_swi0_mask_a9      : 1   ; /* [0]  */
        unsigned int    int_swi0_mask_dsp0    : 1   ; /* [1]  */
        unsigned int    reserved_0            : 30  ; /* [31..2]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_INT_SWI0_MASK;

/* Define the union U_PERI_INT_SWI1_MASK */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    int_swi1_mask_a9      : 1   ; /* [0]  */
        unsigned int    int_swi1_mask_dsp0    : 1   ; /* [1]  */
        unsigned int    reserved_0            : 30  ; /* [31..2]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_INT_SWI1_MASK;

/* Define the union U_PERI_INT_SWI2_MASK */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    int_swi2_mask_a9      : 1   ; /* [0]  */
        unsigned int    int_swi2_mask_dsp0    : 1   ; /* [1]  */
        unsigned int    reserved_0            : 30  ; /* [31..2]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_INT_SWI2_MASK;

/* Define the union U_PERI_QAM */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    qam_i2c_devaddr       : 7   ; /* [6..0]  */
        unsigned int    reserved_0            : 25  ; /* [31..7]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_QAM;

/* Define the union U_PERI_QAM_ADC0 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    opm                   : 2   ; /* [1..0]  */
        unsigned int    use_prev_f            : 1   ; /* [2]  */
        unsigned int    ppsel                 : 1   ; /* [3]  */
        unsigned int    chsel                 : 1   ; /* [4]  */
        unsigned int    endcr                 : 1   ; /* [5]  */
        unsigned int    bctrl                 : 5   ; /* [10..6]  */
        unsigned int    endoutz               : 1   ; /* [11]  */
        unsigned int    selof                 : 1   ; /* [12]  */
        unsigned int    startcal              : 1   ; /* [13]  */
        unsigned int    bcal                  : 2   ; /* [15..14]  */
        unsigned int    fsctrl                : 8   ; /* [23..16]  */
        unsigned int    adcrdy                : 1   ; /* [24]  */
        unsigned int    bitosync              : 1   ; /* [25]  */
        unsigned int    rfalgi                : 3   ; /* [28..26]  */
        unsigned int    rfalgq                : 3   ; /* [31..29]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_QAM_ADC0;

/* Define the union U_PERI_QAM_ADC1 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    qamadc_i2c_devaddr    : 7   ; /* [6..0]  */
        unsigned int    enavcmin              : 1   ; /* [7]  */
        unsigned int    i2c_resetz            : 1   ; /* [8]  */
        unsigned int    reserved_0            : 23  ; /* [31..9]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_QAM_ADC1;

/* Define the union U_PERI_TIANLA_ADAC0 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    dacr_vol              : 7   ; /* [6..0]  */
        unsigned int    reserved_1            : 1   ; /* [7]  */
        unsigned int    dacl_vol              : 7   ; /* [14..8]  */
        unsigned int    reserved_0            : 3   ; /* [17..15]  */
        unsigned int    deemphasis_fs         : 2   ; /* [19..18]  */
        unsigned int    dacr_deemph           : 1   ; /* [20]  */
        unsigned int    dacl_deemph           : 1   ; /* [21]  */
        unsigned int    dacr_path             : 1   ; /* [22]  */
        unsigned int    dacl_path             : 1   ; /* [23]  */
        unsigned int    popfreer              : 1   ; /* [24]  */
        unsigned int    popfreel              : 1   ; /* [25]  */
        unsigned int    fs                    : 1   ; /* [26]  */
        unsigned int    pd_vref               : 1   ; /* [27]  */
        unsigned int    mute_dacr             : 1   ; /* [28]  */
        unsigned int    mute_dacl             : 1   ; /* [29]  */
        unsigned int    pd_dacr               : 1   ; /* [30]  */
        unsigned int    pd_dacl               : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_TIANLA_ADAC0;

/* Define the union U_PERI_TIANLA_ADAC1 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    reserved_2            : 8   ; /* [7..0]  */
        unsigned int    clkdgesel             : 1   ; /* [8]  */
        unsigned int    clksel2               : 1   ; /* [9]  */
        unsigned int    adj_refbf             : 2   ; /* [11..10]  */
        unsigned int    rst                   : 1   ; /* [12]  */
        unsigned int    adj_ctcm              : 1   ; /* [13]  */
        unsigned int    adj_dac               : 2   ; /* [15..14]  */
        unsigned int    reserved_1            : 3   ; /* [18..16]  */
        unsigned int    sample_sel            : 3   ; /* [21..19]  */
        unsigned int    data_bits             : 2   ; /* [23..22]  */
        unsigned int    reserved_0            : 1   ; /* [24]  */
        unsigned int    mute_rate             : 2   ; /* [26..25]  */
        unsigned int    dacvu                 : 1   ; /* [27]  */
        unsigned int    sunmuter              : 1   ; /* [28]  */
        unsigned int    sunmutel              : 1   ; /* [29]  */
        unsigned int    smuter                : 1   ; /* [30]  */
        unsigned int    smutel                : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_TIANLA_ADAC1;

/* Define the union U_PERI_FEPHY */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    fephy_phy_addr        : 5   ; /* [4..0]  */
        unsigned int    reserved_4            : 3   ; /* [7..5]  */
        unsigned int    fephy_tclk_enable     : 1   ; /* [8]  */
        unsigned int    reserved_3            : 1   ; /* [9]  */
        unsigned int    fephy_patch_enable    : 1   ; /* [10]  */
        unsigned int    reserved_2            : 1   ; /* [11]  */
        unsigned int    soft_fephy_mdio_mdc   : 1   ; /* [12]  */
        unsigned int    reserved_1            : 1   ; /* [13]  */
        unsigned int    soft_fephy_mdio_i     : 1   ; /* [14]  */
        unsigned int    reserved_0            : 1   ; /* [15]  */
        unsigned int    soft_fephy_gp_i       : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_FEPHY;

/* Define the union U_PERI_SD_LDO */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    fuse                  : 4   ; /* [3..0]  */
        unsigned int    vset                  : 1   ; /* [4]  */
        unsigned int    en                    : 1   ; /* [5]  */
        unsigned int    bypass                : 1   ; /* [6]  */
        unsigned int    reserved_0            : 25  ; /* [31..7]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_SD_LDO;

/* Define the union U_PERI_USB0 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    ss_word_if_i          : 1   ; /* [0]  */
        unsigned int    ohci_susp_lgcy_i      : 1   ; /* [1]  */
        unsigned int    app_start_clk_i       : 1   ; /* [2]  */
        unsigned int    ulpi_bypass_en_i      : 1   ; /* [3]  */
        unsigned int    reserved_2            : 1   ; /* [4]  */
        unsigned int    ss_autoppd_on_overcur_en_i : 1   ; /* [5]  */
        unsigned int    ss_ena_incr_align_i   : 1   ; /* [6]  */
        unsigned int    ss_ena_incr4_i        : 1   ; /* [7]  */
        unsigned int    ss_ena_incr8_i        : 1   ; /* [8]  */
        unsigned int    ss_ena_incr16_i       : 1   ; /* [9]  */
        unsigned int    reserved_1            : 2   ; /* [11..10]  */
        unsigned int    port0_pwr_en          : 1   ; /* [12]  */
        unsigned int    port1_pwr_en          : 1   ; /* [13]  */
        unsigned int    port2_pwr_en          : 1   ; /* [14]  */
        unsigned int    port0_ovr_en          : 1   ; /* [15]  */
        unsigned int    port1_ovr_en          : 1   ; /* [16]  */
        unsigned int    port2_ovr_en          : 1   ; /* [17]  */
        unsigned int    usbpwr_pctrl          : 1   ; /* [18]  */
        unsigned int    usbovr_p_ctrl         : 1   ; /* [19]  */
        unsigned int    usbpwr_merge_en       : 1   ; /* [20]  */
        unsigned int    usbovr_merge_en       : 1   ; /* [21]  */
        unsigned int    ss_hubsetup_min_i     : 1   ; /* [22]  */
        unsigned int    reserved_0            : 5   ; /* [27..23]  */
        unsigned int    chipid                : 1   ; /* [28]  */
        unsigned int    ss_scaledown_mode     : 2   ; /* [30..29]  */
        unsigned int    ohci_0_cntsel_i_n     : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_USB0;

/* Define the union U_PERI_USB1 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    phy0_txvreftune       : 4   ; /* [3..0]  */
        unsigned int    phy0_txrisetune       : 2   ; /* [5..4]  */
        unsigned int    reserved_2            : 1   ; /* [6]  */
        unsigned int    phy0_txfslstune       : 4   ; /* [10..7]  */
        unsigned int    phy0_sqrxtune         : 3   ; /* [13..11]  */
        unsigned int    phy0_compdistune      : 3   ; /* [16..14]  */
        unsigned int    phy0_loopbackenb      : 1   ; /* [17]  */
        unsigned int    phy0_sleepm           : 1   ; /* [18]  */
        unsigned int    phy0_txhsxvtune       : 2   ; /* [20..19]  */
        unsigned int    phy0_commononn        : 1   ; /* [21]  */
        unsigned int    phy0_siddq            : 1   ; /* [22]  */
        unsigned int    reserved_1            : 1   ; /* [23]  */
        unsigned int    phy0_vbusvldext       : 1   ; /* [24]  */
        unsigned int    phy0_vbusvldextsel    : 1   ; /* [25]  */
        unsigned int    phy0_txpreemppulsetune : 1   ; /* [26]  */
        unsigned int    phy0_txpreempamptune  : 2   ; /* [28..27]  */
        unsigned int    phy0_txrestune        : 2   ; /* [30..29]  */
        unsigned int    reserved_0            : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_USB1;

/* Define the union U_PERI_USB2 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    phy1_txvreftune       : 4   ; /* [3..0]  */
        unsigned int    phy1_txrisetune       : 2   ; /* [5..4]  */
        unsigned int    reserved_2            : 1   ; /* [6]  */
        unsigned int    phy1_txfslstune       : 4   ; /* [10..7]  */
        unsigned int    phy1_sqrxtune         : 3   ; /* [13..11]  */
        unsigned int    phy1_compdistune      : 3   ; /* [16..14]  */
        unsigned int    phy1_loopbackenb      : 1   ; /* [17]  */
        unsigned int    phy1_sleepm           : 1   ; /* [18]  */
        unsigned int    phy1_txhsxvtune       : 2   ; /* [20..19]  */
        unsigned int    phy1_commononn        : 1   ; /* [21]  */
        unsigned int    phy1_siddq            : 1   ; /* [22]  */
        unsigned int    reserved_1            : 1   ; /* [23]  */
        unsigned int    phy1_vbusvldext       : 1   ; /* [24]  */
        unsigned int    phy1_vbusvldextsel    : 1   ; /* [25]  */
        unsigned int    phy1_txpreemppulsetune : 1   ; /* [26]  */
        unsigned int    phy1_txpreempamptune  : 2   ; /* [28..27]  */
        unsigned int    phy1_txrestune        : 2   ; /* [30..29]  */
        unsigned int    reserved_0            : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_USB2;

/* Define the union U_PERI_USB3 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    phy0_phy1_phy2_testdatain : 8   ; /* [7..0]  */
        unsigned int    phy0_phy1_phy2__testclk : 1   ; /* [8]  */
        unsigned int    phy0_phy1_phy2_testaddr : 4   ; /* [12..9]  */
        unsigned int    phy0_phy1_phy2_testdataoutsel : 1   ; /* [13]  */
        unsigned int    reserved_1            : 2   ; /* [15..14]  */
        unsigned int    phy0_testdataout      : 4   ; /* [19..16]  */
        unsigned int    phy1_testdataout      : 4   ; /* [23..20]  */
        unsigned int    phy2_testdataout      : 4   ; /* [27..24]  */
        unsigned int    reserved_0            : 4   ; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_USB3;

/* Define the union U_PERI_USB4 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    phy2_txvreftune       : 4   ; /* [3..0]  */
        unsigned int    phy2_txrisetune       : 2   ; /* [5..4]  */
        unsigned int    reserved_2            : 1   ; /* [6]  */
        unsigned int    phy2_txfslstune       : 4   ; /* [10..7]  */
        unsigned int    phy2_sqrxtune         : 3   ; /* [13..11]  */
        unsigned int    phy2_compdistune      : 3   ; /* [16..14]  */
        unsigned int    phy2_loopbackenb      : 1   ; /* [17]  */
        unsigned int    phy2_sleepm           : 1   ; /* [18]  */
        unsigned int    phy2_txhsxvtune       : 2   ; /* [20..19]  */
        unsigned int    phy2_commononn        : 1   ; /* [21]  */
        unsigned int    phy2_siddq            : 1   ; /* [22]  */
        unsigned int    reserved_1            : 1   ; /* [23]  */
        unsigned int    phy2_vbusvldext       : 1   ; /* [24]  */
        unsigned int    phy2_vbusvldextsel    : 1   ; /* [25]  */
        unsigned int    phy2_txpreemppulsetune : 1   ; /* [26]  */
        unsigned int    phy2_txpreempamptune  : 2   ; /* [28..27]  */
        unsigned int    phy2_txrestune        : 2   ; /* [30..29]  */
        unsigned int    reserved_0            : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_USB4;

/* Define the union U_DSP0_CTRL */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    runstall_dsp0         : 1   ; /* [0]  */
        unsigned int    statvectorsel_dsp0    : 1   ; /* [1]  */
        unsigned int    ocdhaltonreset_dsp0   : 1   ; /* [2]  */
        unsigned int    reserved_2            : 1   ; /* [3]  */
        unsigned int    wdg1_en_dsp0          : 1   ; /* [4]  */
        unsigned int    reserved_1            : 1   ; /* [5]  */
        unsigned int    reserved_0            : 26  ; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_DSP0_CTRL;

/* Define the union U_DSP0_PRID */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    prid_dsp0             : 2   ; /* [1..0]  */
        unsigned int    reserved_0            : 30  ; /* [31..2]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_DSP0_PRID;

/* Define the union U_DSP0_STATUS */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    pwaitmode_dsp0        : 1   ; /* [0]  */
        unsigned int    xocdmode_dsp0         : 1   ; /* [1]  */
        unsigned int    reserved_0            : 30  ; /* [31..2]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_DSP0_STATUS;

/* Define the union U_PERI_CHIP_INFO4 */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    dolby_flag            : 1   ; /* [0]  */
        unsigned int    reserved_0            : 1   ; /* [1]  */
        unsigned int    dts_flag              : 1   ; /* [2]  */
        unsigned int    peri_chip_info4       : 29  ; /* [31..3]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_CHIP_INFO4;

/* Define the union U_PERI_SIM_OD_CTRL */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    sim1_pwren_od_sel     : 1   ; /* [0]  */
        unsigned int    sim0_pwren_od_sel     : 1   ; /* [1]  */
        unsigned int    sim0_rst_od_sel       : 1   ; /* [2]  */
        unsigned int    sim1_rst_od_sel       : 1   ; /* [3]  */
        unsigned int    reserved_0            : 28  ; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_SIM_OD_CTRL;

/* Define the union U_PERI_SOC_FUSE */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    otp_control_cpu       : 1   ; /* [0]  */
        unsigned int    reserved_3            : 2   ; /* [2..1]  */
        unsigned int    otp_control_gpu       : 2   ; /* [4..3]  */
        unsigned int    otp_ddr_st            : 2   ; /* [6..5]  */
        unsigned int    otp_vd_st             : 1   ; /* [7]  */
        unsigned int    reserved_2            : 1   ; /* [8]  */
        unsigned int    otp_pvr_tsi2_cken     : 1   ; /* [9]  */
        unsigned int    otp_spdif_ctrl        : 1   ; /* [10]  */
        unsigned int    reserved_1            : 1   ; /* [11]  */
        unsigned int    otp_ctrl_vdac         : 4   ; /* [15..12]  */
        unsigned int    chip_id               : 5   ; /* [20..16]  */
        unsigned int    reserved_0            : 3   ; /* [23..21]  */
        unsigned int    mven                  : 1   ; /* [24]  */
        unsigned int    vedu_chip_id          : 2   ; /* [26..25]  */
        unsigned int    vdac_bgtrim_sel       : 1   ; /* [27]  */
        unsigned int    vdac_bgtrim           : 2   ; /* [29..28]  */
        unsigned int    caboot_en             : 1   ; /* [30]  */
        unsigned int    gpu_pwractive         : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_SOC_FUSE;

/* Define the union U_PERI_FEPHY_LDO_CTRL */
typedef union
{
    /* Define the struct bits */
    struct
    {
        unsigned int    fephy_ldo_vset        : 4   ; /* [3..0]  */
        unsigned int    reserved_0            : 28  ; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;

} U_PERI_FEPHY_LDO_CTRL;

//==============================================================================
/* Define the global struct */
typedef struct
{
    volatile U_START_MODE           START_MODE               ; /* 0x0 */
    volatile U_PERI_STAT            PERI_STAT                ; /* 0x4 */
    volatile U_PERI_CTRL            PERI_CTRL                ; /* 0x8 */
    volatile U_CPU_STAT             CPU_STAT                 ; /* 0xc */
    volatile U_CPU_SET              CPU_SET                  ; /* 0x10 */
    volatile U_NF_CFG               NF_CFG                   ; /* 0x14 */
    volatile U_PERI_SEC_STAT        PERI_SEC_STAT            ; /* 0x18 */
    volatile unsigned int           reserved_0[9]            ; /* 0x1c~0x3c */
    volatile U_PERI_QOS_GLOB_CTRL   PERI_QOS_GLOB_CTRL       ; /* 0x40 */
    volatile unsigned int           reserved_1[28]           ; /* 0x44~0xb0 */
    volatile U_PERI_USB_RESUME_INT_MASK   PERI_USB_RESUME_INT_MASK ; /* 0xb4 */
    volatile U_PERI_USB_RESUME_INT_RAWSTAT   PERI_USB_RESUME_INT_RAWSTAT ; /* 0xb8 */
    volatile U_PERI_USB_RESUME_INT_STAT   PERI_USB_RESUME_INT_STAT ; /* 0xbc */
    volatile U_PERI_INT_A9TOMCE     PERI_INT_A9TOMCE         ; /* 0xc0 */
    volatile U_PERI_INT_A9TODSP0    PERI_INT_A9TODSP0        ; /* 0xc4 */
    volatile U_PERI_INT_DSP0TOA9    PERI_INT_DSP0TOA9        ; /* 0xc8 */
    volatile unsigned int           reserved_2[6]            ; /* 0xcc~0xe0 */
    volatile U_PERI_INT_SWI0        PERI_INT_SWI0            ; /* 0xe4 */
    volatile U_PERI_INT_SWI1        PERI_INT_SWI1            ; /* 0xe8 */
    volatile U_PERI_INT_SWI2        PERI_INT_SWI2            ; /* 0xec */
    volatile U_PERI_INT_SWI0_MASK   PERI_INT_SWI0_MASK       ; /* 0xf0 */
    volatile U_PERI_INT_SWI1_MASK   PERI_INT_SWI1_MASK       ; /* 0xf4 */
    volatile U_PERI_INT_SWI2_MASK   PERI_INT_SWI2_MASK       ; /* 0xf8 */
    volatile unsigned int           reserved_3               ; /* 0xfc */
    volatile U_PERI_QAM             PERI_QAM                 ; /* 0x100 */
    volatile U_PERI_QAM_ADC0        PERI_QAM_ADC0            ; /* 0x104 */
    volatile U_PERI_QAM_ADC1        PERI_QAM_ADC1            ; /* 0x108 */
    volatile unsigned int           reserved_4               ; /* 0x10c */
    volatile U_PERI_TIANLA_ADAC0    PERI_TIANLA_ADAC0        ; /* 0x110 */
    volatile U_PERI_TIANLA_ADAC1    PERI_TIANLA_ADAC1        ; /* 0x114 */
    volatile U_PERI_FEPHY           PERI_FEPHY               ; /* 0x118 */
    volatile U_PERI_SD_LDO          PERI_SD_LDO              ; /* 0x11c */
    volatile U_PERI_USB0            PERI_USB0                ; /* 0x120 */
    volatile U_PERI_USB1            PERI_USB1                ; /* 0x124 */
    volatile U_PERI_USB2            PERI_USB2                ; /* 0x128 */
    volatile U_PERI_USB3            PERI_USB3                ; /* 0x12c */
    volatile U_PERI_USB4            PERI_USB4                ; /* 0x130 */
    volatile unsigned int           reserved_5[19]           ; /* 0x134~0x17c */
    volatile U_DSP0_CTRL            DSP0_CTRL                ; /* 0x180 */
    volatile U_DSP0_PRID            DSP0_PRID                ; /* 0x184 */
    volatile U_DSP0_STATUS          DSP0_STATUS              ; /* 0x188 */
    volatile unsigned int           reserved_6[21]           ; /* 0x18c~0x1dc */
    volatile U_PERI_CHIP_INFO4      PERI_CHIP_INFO4          ; /* 0x1e0 */
    volatile unsigned int           reserved_7[3]            ; /* 0x1e4~0x1ec */
    volatile unsigned int           PERI_SW_SET              ; /* 0x1f0 */
    volatile unsigned int           reserved_8[131]          ; /* 0x1f4~0x3fc */
    volatile unsigned int           PERI_DSP0_0              ; /* 0x400 */
    volatile unsigned int           PERI_DSP0_1              ; /* 0x404 */
    volatile unsigned int           PERI_DSP0_2              ; /* 0x408 */
    volatile unsigned int           PERI_DSP0_3              ; /* 0x40c */
    volatile unsigned int           PERI_DSP0_4              ; /* 0x410 */
    volatile unsigned int           PERI_DSP0_5              ; /* 0x414 */
    volatile unsigned int           PERI_DSP0_6              ; /* 0x418 */
    volatile unsigned int           PERI_DSP0_7              ; /* 0x41c */
    volatile unsigned int           reserved_9[263]          ; /* 0x420~0x838 */
    volatile U_PERI_SIM_OD_CTRL     PERI_SIM_OD_CTRL         ; /* 0x83c */
    volatile U_PERI_SOC_FUSE        PERI_SOC_FUSE            ; /* 0x840 */
    volatile U_PERI_FEPHY_LDO_CTRL   PERI_FEPHY_LDO_CTRL     ; /* 0x844 */
} S_PERICTRL_REGS_TYPE;

#endif /* __HI_REG_PERI_H__ */

