/***********************************************************************************/
/*  Copyright (c) 2002-2006, Silicon Image, Inc.  All rights reserved.             */
/*  No part of this work may be reproduced, modified, distributed, transmitted,    */
/*  transcribed, or translated into any language or computer format, in any form   */
/*  or by any means without written permission of: Silicon Image, Inc.,            */
/*  1060 East Arques Avenue, Sunnyvale, California 94085                           */
/***********************************************************************************/
#include "si_typedefs.h"
#include "si_txvptbl.h"
#include "si_defstx.h"
#include "si_hdmitx.h"
#include "si_txvidp.h"
#include "si_txapidefs.h"
#include "si_vmode.h"
#include "si_hdcp.h"
#include "si_eedefs.h"
#include "si_eeprom.h"
#include "si_phy.h"

HI_U8 SiIVidPathInIndex  = 2; /* inYCbCr24[] */
HI_U8 SiIVidPathOutIndex = 1; //YCbCr444

#define BYPASS 0x00
/* TODO */
/* Add code fo RBG to YCbCr Converter and Down Sampleing here */
/* tables to be modified                                      */
/* inRGB24[]                                                  */
/* inRGBDVO12[]                                               */
/* inYCbCr24[]                                                */

ROM const HI_U8 inRGB24[] = {
      BYPASS        // RGB24 out
};

ROM const HI_U8 inRGBDVO12[] = {
      TX_DVO                        // RGB DVO 12 out
};

ROM const HI_U8 inYCbCr24[] = {
      TX_YCbCrToRGB | TX_Dither,   // RGB24 out
      BYPASS                       // YCbCr24 Out
};
#if 0//lc change for Hisilicon
ROM const HI_U8 inYC24[] = {
      TX_422to444 | TX_YCbCrToRGB | TX_Dither, // RGB24 out
      TX_422to444 | TX_Dither,                 // YCbCr24 out
      BYPASS                                   // YC24 out
};
#else
ROM const HI_U8 inYC24[] = {
      TX_DeMux | TX_422to444 | TX_YCbCrToRGB | TX_Dither, // RGB24 out
      TX_DeMux | TX_422to444 | TX_Dither,                 // YCbCr24 out
      TX_DeMux | BYPASS                                   // YC24 out
};
#endif

ROM const HI_U8 inYCMuxed12[] = {
      TX_DeMux | TX_422to444 | TX_YCbCrToRGB | TX_Dither, // RGB24 out
      TX_DeMux | TX_422to444 | TX_Dither,                 // YCbCr24 out
      TX_DeMux                                            // YC24 out
};

ROM const HI_U8 inYCMuxed656_12[] = {
      TX_DeMux | TX_SyncExtr | TX_422to444 | TX_YCbCrToRGB | TX_Dither, // RGB24 out
      TX_DeMux | TX_SyncExtr | TX_422to444 | TX_Dither,                 // YCbCr24 out
      TX_DeMux | TX_SyncExtr                                            // YC24 out
};

ROM const HI_U8 inYC656_24[] = {
      TX_SyncExtr | TX_422to444 | TX_YCbCrToRGB | TX_Dither, // RGB24 out
      TX_SyncExtr | TX_422to444 | TX_Dither,                 // YCbCr24 out
      TX_SyncExtr                                            // YC24 out
};

ROM const HI_U8 * const inMode[] = {
      inRGB24, inRGBDVO12, inYCbCr24, inYC24, inYCMuxed12, inYCMuxed656_12, inYC656_24
};

HI_U8 SI_ConfigTXVideoMode(HI_U8 In, HI_U8 Out)
{

    HI_U8 TblVal, RegVal;
    
    TblVal = *(inMode[In] + Out);
    RegVal = ReadByteHDMITXP0(TX_VID_MODE_ADDR);
    if( TblVal & TX_DeMux)
        RegVal |=BIT_TX_DEMUX_YC;
    else
        RegVal &= (~BIT_TX_DEMUX_YC);
        
    if( TblVal & TX_SyncExtr)
        RegVal |=BIT_TX_SYNC_EXTR;
    else
        RegVal &= (~BIT_TX_SYNC_EXTR);
        
    if( TblVal & TX_YCbCrToRGB)
    {
        RegVal |= BIT_TX_CSC;
        //RegVal |= BIT_TX_16_235_RANGE;  // expand range
        RegVal &= (~BIT_TX_16_235_RANGE);
    }
    else
    {
        RegVal &= (~BIT_TX_CSC);
        RegVal &= (~BIT_TX_16_235_RANGE);
    }
    
    if( TblVal & TX_Dither)
        RegVal |=BIT_TX_DITHER;
    else
        RegVal &= (~BIT_TX_DITHER);
        
    if( TblVal & TX_422to444)
        RegVal |= BIT_TX_422to444;
    else
        RegVal &= (~BIT_TX_422to444);

    WriteByteHDMITXP0(TX_VID_MODE_ADDR, RegVal);
    HI_INFO_HDMI("SI_ConfigTXVideoMode in:0x%02x: out:0x%02x, TblVal:0x%x, RegVal:0x%02x\n", In, Out, TblVal, RegVal);
    RegVal = ReadByteHDMITXP0(TX_SYS_CTRL1_ADDR);
    if( TblVal & TX_DVO)
        RegVal &= (~BIT_BSEL24BITS);
    else
        RegVal |= BIT_BSEL24BITS;
    WriteByteHDMITXP0(TX_SYS_CTRL1_ADDR, RegVal);

    /* TODO */
    /* Add code fo RBG to YCbCr Converter and Down Sampleing here */

    return TblVal;

}
//---------------------------------------------------------------------------
// Function Name: SI_SetDeepColor
// Function Description:  This function sets Deep Color
//---------------------------------------------------------------------------
void SI_SetDeepColor ( HI_U8 bDeepColor )
{
    HI_U8 bRegVal, bTmpDeepColor;
    HI_U8 bRegVal2;
    HI_U32 u32Value = 0;

    //pw down phy is similar to pll reset
    SI_TX_PHY_PowerDown(HI_TRUE);
    
    bTmpDeepColor = bDeepColor;
    if (bTmpDeepColor == SiI_DeepColor_Off)
    {
        bDeepColor = SiI_DeepColor_24bit;       // Setup everything as 24bpp but do not turn deep color on
    }

    // VID_ACEN_ADDR
    //  [7:6] - Wide Bus
    //          0b00 = 24 bits
    //          0b01 = 30 bits
    //          0b10 = 36 bits
    //bRegVal = (ReadByteHDMITXP0 ( VID_ACEN_ADDR ) & VID_ACEN_DEEP_COLOR_CLR);	
    bRegVal = (ReadByteHDMITXP0 ( VID_IN_MODE ) & VIN_MODE_CLR);	
    
    /* we give a fixed input bit widths value of 12bit for every channel*/
    bRegVal |= 0x06;

#ifndef _FPGA_FRED2_

    WriteByteHDMITXP0 (VID_IN_MODE,  bRegVal);
#else

    //lc change WriteByteHDMITXP0 (VID_ACEN_ADDR,  (bRegVal | 0x80));
    HI_INFO_HDMI("try to write VID_ACEN_ADDR:0x%02x, data:0x%02x\n", VID_IN_MODE, bRegVal);
    WriteByteHDMITXP0 (VID_IN_MODE,  bRegVal);
#endif

    HI_INFO_HDMI ("[TXVIDP.C](SI_SetDeepColor): Set Deep Color VID_ACEN_ADDR bDeepColor:%d\n", bDeepColor);

    // AUDP_TXCTRL_ADDR
    //  [5:3] ?PACKET_MODE
    //          0b100 = 24 bits
    //          0b101 = 30 bits
    //          0b110 = 36 bits
    // Set / Clear bit 5 separately below.
    bRegVal = (ReadByteHDMITXP1 ( AUDP_TXCTRL_ADDR ) & 0xE7);
    bRegVal |= (bDeepColor << 3);

    if ((bTmpDeepColor != SiI_DeepColor_Off) && (bTmpDeepColor != SiI_DeepColor_24bit))
    {
        bRegVal |= BIT_DEEPCOLOR_EN;
        bRegVal |= 0x20;

        // Enable dithering and set Dither Mode for Deep Color:
        bRegVal2 = (ReadByteHDMITXP0 ( TX_VID_MODE_ADDR ) & CLR_BITS_7_6_5);
        bRegVal2 |= (bDeepColor << 6);
        bRegVal2 |= BIT_DITHER_EN;
        u32Value = 0; //disable vid_dither 0x4F
    }
    else
    {
        bRegVal &= (~BIT_DEEPCOLOR_EN);
       // bRegVal &= (~0x20);
        bRegVal |= 0x20;
        bRegVal2 = (ReadByteHDMITXP0 ( TX_VID_MODE_ADDR ) & CLR_BITS_7_6_5);    // Disable dithering if not DC
        bRegVal2 |= BIT_DITHER_EN;
        u32Value = 0xe; //Enable D_RC_EN, D_GC_EN, D_BC_EN
    }
    /*set deep color */
    WriteByteHDMITXP0 (  TX_VID_DITHER, u32Value );
    WriteByteHDMITXP1 (  AUDP_TXCTRL_ADDR, bRegVal );
    HI_INFO_HDMI ("[TXVIDP.C](SI_SetDeepColor): Set Deep Color AUDP_TXCTRL_ADDR:0x%02x, data:0x%02x\n", AUDP_TXCTRL_ADDR, bRegVal);
    WriteByteHDMITXP0 (  TX_VID_MODE_ADDR, bRegVal2 );
    HI_INFO_HDMI ("[TXVIDP.C](SI_SetDeepColor): Set Deep Color AUDP_TXCTRL_ADDR:0x%02x, data:0x%02x\n", TX_VID_MODE_ADDR, bRegVal2);

    SI_TX_PHY_SetDeepColor(bDeepColor);

    SI_TX_PHY_PowerDown(HI_FALSE);
    #if 0
    /* Config rooG IP for DeepColor*/
#if defined(BOARD_TYPE_hi3716mv300_fpga)
u32Value = hdmi_gpio_i2c_read(0x60, 0x2);        
#else
DRV_HDMI_ReadRegister(0x10171808, &u32Value);    
#endif

    HI_INFO_HDMI("TMDS_CTL3 old walue:0x%x\n", u32Value);
    if (SiI_DeepColor_30bit == bDeepColor)
    {
        u32Value =  (u32Value & ~0x06) | 0x02;
        HI_INFO_HDMI("SiI_DeepColor_30bit\n");
    }
    else if (SiI_DeepColor_36bit == bDeepColor)
    {
        u32Value =  (u32Value & ~0x06) | 0x04;
        HI_INFO_HDMI("SiI_DeepColor_36bit\n");
    }
    else if (SiI_DeepColor_24bit == bDeepColor)
    {
        u32Value =  (u32Value & ~0x06) | 0x00;
        HI_INFO_HDMI("SiI_DeepColor_24bit(normal)\n");
    }
    else
    {
        u32Value =  (u32Value & ~0x06) | 0x00;
        HI_INFO_HDMI("SiI_DeepColor_Off\n");
    }
#if defined(BOARD_TYPE_hi3716mv300_fpga)
    hdmi_gpio_i2c_write(0x60, 0x2,u32Value);
    u32Value = 0;
    u32Value = hdmi_gpio_i2c_read(0x60, 0x2);
    HI_INFO_HDMI("TMDS_CTL3 new walue:0x%x\n", u32Value);    
#else    
    DRV_HDMI_WriteRegister((HI_U32)0x10171808, u32Value);
    u32Value = 0;
    DRV_HDMI_ReadRegister((HI_U32)0x10171808, &u32Value);
    HI_INFO_HDMI("TMDS_CTL3 new walue:0x%x\n", u32Value);
#endif
    #endif 
    
    return;  
}
//---------------------------------------------------------------------------
// Function Name: SetInputClockEdge
// Function Description:  This function sets Input Clock Edge
//---------------------------------------------------------------------------

static void SetInputClockEdge( HI_U8 bOn )
{
    HI_U8 bRegVal;

    bRegVal = ReadByteHDMITXP0(TX_SYS_CTRL1_ADDR);
    if( bOn )
        bRegVal |= BIT_TX_CLOCK_RISING_EDGE;
    else
        bRegVal &= (~BIT_TX_CLOCK_RISING_EDGE);
    WriteByteHDMITXP0(TX_SYS_CTRL1_ADDR, bRegVal);

}
//------------------------------------------------------------------------------
// Function Name: SI_SetVideoPath
// Function Description: This functiom sets video path, for for different Input/Output
//                       Video formats, it doesn't include setting Resolution dependent parameters
// Accepts: poinetr on Video path parameters
// Returns: none
// Globals: none
//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////////////////
// abVideoPath[0] = Input Source : 0 - RGB24; 1 - RGB DVO 12; 2 - YCbCr24; 3 - YC24; 4 - YCbCr4:2:2
// abVideoPath[1] = Color Space Conversion : 0 - RGB out; 1 - YCbr (4:4:4) out
// abVideoPath[2] = Clock Edge : 0 - Falling edge; 1 - Rising Edge
// abVideoPath[3] = Deep Color Mode : 0 - 24 bit; 1 - 30 bits; 2 - 36 bits
////////////////////////////////////////////////////////////////////////////////////////////



void SI_SetVideoPath ( HI_U8 bVMode, HI_U8 * abVideoPath )
{
    //HI_U8 bError;
    static HI_U8 InfCtrl1, InfCtrl2;
    HI_INFO_HDMI("SI_SetVideoPath bVMode:0x%02x\n", bVMode);
    SI_BlockWriteEEPROM( 4, EE_TX_VIDEOPATH_ADDR , abVideoPath);

    SiIVidPathInIndex = abVideoPath[0];
    SiIVidPathOutIndex = abVideoPath[1];
    HI_INFO_HDMI("SI_SetVideoPath setting video path:0x%02x,0x%02x,0x%02x,0x%02x\n", abVideoPath[0], abVideoPath[1], abVideoPath[2], abVideoPath[3]);
    SI_ConfigTXVideoMode( abVideoPath[0], abVideoPath[1]);
    SetInputClockEdge( abVideoPath[2] );
    SI_SetDeepColor ( abVideoPath[3] );

#if 0 /*--TMDS has been del in 1.4 ctrller--*/
    bError = SI_TMDS_setup(bVMode);

    if (bError == TMDS_SETUP_FAILED)
    {
        HI_INFO_HDMI ("[TXVIDP.C](SI_SetVideoPath): TMDS_SETUP_FAILED\n");
    }
#endif /*--NO MODIFY : COMMENT BY CODINGPARTNER--*/

    InfCtrl1 = ReadByteHDMITXP1(INF_CTRL1);    // save packet buffer control regs
    InfCtrl2 = ReadByteHDMITXP1(INF_CTRL2);
    //DEBUG_PRINTK("SI_SetVideoPath to do SI_SW_ResetHDMITX\n");
    //SI_SW_ResetHDMITX();               // Reset internal state machines and allow TCLK to Rx to stabilize

    WriteByteHDMITXP1(INF_CTRL1, InfCtrl1);        // Retrieve packet buffer control regs
    WriteByteHDMITXP1(INF_CTRL2, InfCtrl2);
    HI_INFO_HDMI("SI_SetVideoPath end\n");
#if defined (HDCP_SUPPORT)   
    SI_PrintAuthState();
#endif
}
//------------------------------------------------------------------------------
// Function Name: SI_GetVideoPath
// Function Description: This functiom gets video path, for for different Input/Output
//                       Video formats, it doesn't include setting Resolution dependent parameters
// Accepts: none
// Returns: poinetr on Video path parameters
// Globals: none
//------------------------------------------------------------------------------
void SI_GetVideoPath ( HI_U8 * abVideoPath )
{
    HI_U8 bRegVal;
    HI_INFO_HDMI("SI_GetVideoPath\n");
    abVideoPath[0] = SiIVidPathInIndex;
    abVideoPath[1] = SiIVidPathOutIndex;

    bRegVal = ReadByteHDMITXP0(TX_SYS_CTRL1_ADDR);
    abVideoPath[2] = 0;
    if( bRegVal & BIT_TX_CLOCK_RISING_EDGE )
        abVideoPath[2]|= SiI_RisingVideoClockEdge;

    bRegVal = ReadByteHDMITXP1 (  AUDP_TXCTRL_ADDR );
    if (!(bRegVal & BIT_DEEPCOLOR_EN))
    {
        abVideoPath[3] = 0xFF;
    }
    else
    {
        abVideoPath[3] = bRegVal >> 3;
        abVideoPath[3] &= 0x03;         // extract Deep Color  bits
    }
}

void SI_TX_InvertSyncPol(HI_BOOL bInvert)
{
    HI_U8 RegVal = 0;
    
    RegVal = ReadByteHDMITXP0(DE_CNTRL_ADDR);
    HI_INFO_HDMI("DE_CNTRL_ADDR:0x%x, Before RegVal:0x%x\n", DE_CNTRL_ADDR, RegVal);
    if(bInvert)
    {
        RegVal |= 0x30;
        HI_INFO_HDMI("Neagtiv Polarity DE_CNTRL_ADDR:0x%x, change RegVal:0x%x\n", DE_CNTRL_ADDR, RegVal);
    }
    else
    {
        RegVal &= (~0x30);
        HI_INFO_HDMI("Positive Polarity DE_CNTRL_ADDR:0x%x, change RegVal:0x%x\n", DE_CNTRL_ADDR, RegVal);
    }
    WriteByteHDMITXP0(DE_CNTRL_ADDR,RegVal);    
}

void SI_TX_CSC709Select(HI_BOOL bSpace709)
{
    HI_U8 RegVal = 0;

    RegVal = ReadByteHDMITXP0(TX_VID_CTRL_ADDR);
    HI_INFO_HDMI("TX_VID_CTRL_ADDR:0x%x, Before RegVal:0x%x\n", TX_VID_CTRL_ADDR, RegVal);

    if(bSpace709)
    {
        RegVal |= BIT_VID_CTRL_CSCSEL;
        HI_INFO_HDMI("709 Standard TX_VID_CTRL_ADDR:0x%x, change RegVal:0x%x\n", TX_VID_CTRL_ADDR, RegVal);
    }
    else
    {
        RegVal &= (~BIT_VID_CTRL_CSCSEL);
        HI_INFO_HDMI("601 Standard TX_VID_CTRL_ADDR:0x%x, change RegVal:0x%x\n", TX_VID_CTRL_ADDR, RegVal);
    }
    WriteByteHDMITXP0(TX_VID_CTRL_ADDR,RegVal);    
    
}


#if 0 /*--TMDS Reg has been Del in 1.4 ctrller--*/
//------------------------------------------------------------------------------
// SI_TMDS_setup
//------------------------------------------------------------------------------
HI_U8 SI_TMDS_setup(HI_U8 bVMode)
{
    int idclk_freq = 0;
    int iLowRange = 0;
    int iMidRange1 = 0;
    int iMidRange2 = 0;
    int iHghRange = 0;
    TCLK_SEL tclk = 0;
    HI_U8 bRegVal = 0;
    HI_U8 bRegVal2 = 0;
    HI_U8 nIPLLF = 0;
    HI_U8 nFFRCOUNT = 0;
    HI_U8 nFFBCOUNT = 0;
    HI_U8 nFPOSTCOUNT = 0;

    HI_INFO_HDMI("Start...\n");
    idclk_freq = (int) VModeTables[bVMode].PixClk / 100;

    bRegVal = ReadByteHDMITXP0 ( TX_TMDS_CTRL_ADDR ) & 0x60;
    switch (bRegVal)
    {
    case 0x00:
        tclk = x0_5;
        HI_INFO_HDMI ("[TXVIDP.C](SI_TMDS_setup): 0.5x tclk\n");
        break;
    default:
    case 0x20:
        tclk = x1;
        HI_INFO_HDMI ("[TXVIDP.C](SI_TMDS_setup): 1.0x tclk\n");
        break;
    case 0x40:
        tclk = x2;
        HI_INFO_HDMI ("[TXVIDP.C](SI_TMDS_setup): 2.0x tclk\n");
        break;
    case 0x60:
        tclk = x4;
        HI_INFO_HDMI ("[TXVIDP.C](SI_TMDS_setup): 4.0x tclk\n");
        break;
    }

    bRegVal = ReadByteHDMITXP0 ( VID_ACEN_ADDR );
    bRegVal = bRegVal & (~VID_ACEN_DEEP_COLOR_CLR);
    bRegVal = bRegVal >> 6;

    iLowRange = 25;
    switch (bRegVal)
    {
    case SiI_DeepColor_24bit:
        nFFBCOUNT = 0x03;
        iMidRange1 =  64;
        iMidRange2 = 126;
        iHghRange  = 270;
        break;
    case SiI_DeepColor_30bit:
        nFFBCOUNT = 0x04;
        iMidRange1 =  53;
        iMidRange2 = 104;
        iHghRange  = 203;
        break;
    case SiI_DeepColor_36bit:
        nFFBCOUNT = 0x05;
        iMidRange1 =  44;
        iMidRange2 =  86;
        iHghRange  = 168;
        break;
    }
    // Set FFBCount field in 0x7A:0x83:
    // NOTE: UP TO BUILD 14, nFFBCOUNT value caldulted in the switch statement above was not used.
    // Instead, the code forced it to 0x03 (line 303 below).This line sets it to the calculated value.
    // Line 303 was modified accordingly. Consult DD.
    bRegVal2 = ReadByteHDMITXP0 ( TX_TMDS_CTRL2_ADDR );
    bRegVal2 &= CLR_BITS_5_4_3;
    bRegVal2 |= (nFFBCOUNT << 3);
    WriteByteHDMITXP0 ( TX_TMDS_CTRL2_ADDR, bRegVal2 );

    nIPLLF = 0x01;
    switch (tclk)
    {
    case x0_5:
        nFPOSTCOUNT = 0x07;
        break;
    case x1:
        nFPOSTCOUNT = 0x03;
        break;
    case x2:
        nFPOSTCOUNT = 0x01;
        break;
    case x4:
        nFPOSTCOUNT = 0x00;
        break;
    }

    // Out of Range
    if ((idclk_freq < iLowRange) || (idclk_freq > iHghRange))
    {
        return TMDS_SETUP_FAILED;
    }

    
    if ((idclk_freq >= iLowRange) && (idclk_freq <= iMidRange1))
    {// Blue range
        nFFRCOUNT = 0x00;
        SI_Mpll_setup(blue);
        SI_FApost_setup(blue, idclk_freq, bRegVal);
    }
    else if ((idclk_freq > iMidRange1) && (idclk_freq <= iMidRange2))
    {// Yellow range
        if (tclk == x4)
        {
            return TMDS_SETUP_FAILED;
        }
        nFFRCOUNT = 0x01;
        nFPOSTCOUNT >>= 1;
        SI_Mpll_setup(yellow);
        SI_FApost_setup(yellow, idclk_freq, bRegVal);
    }
    else if ((idclk_freq > iMidRange2) && (idclk_freq <= iHghRange))
    {// Orange range
        if ((tclk == x4) || (tclk == x2))
        {
            return TMDS_SETUP_FAILED;
        }
        nFFRCOUNT = 0x03;
        nFPOSTCOUNT >>= 2;
        SI_Mpll_setup(orange);
        SI_FApost_setup(orange, idclk_freq, bRegVal);
    }

    // TX_SYS_CTRL4_ADDR
    //  [7:5]   reserved
    //  [4:1]   IPLLF = 0x01*
    //  [0]     reserved
    WriteByteHDMITXP0 (TX_SYS_CTRL4_ADDR, ((ReadByteHDMITXP0(TX_SYS_CTRL4_ADDR) & 0xE1 ) | (0x01 << 1)));

    // TX_TMDS_CTRL2_ADDR
    //  [7:6]   TPOSTCOUNT
    //  [5:3]   FFBCOUNT = 0x03*

    //  [2:0]   FFRCOUNT*

    ///    WriteByteHDMITXP0 (TX_TMDS_CTRL2_ADDR, ((ReadByteHDMITXP0(TX_TMDS_CTRL2_ADDR) & 0xC0) | ((0x03 << 3) | (nFFRCOUNT))));
    WriteByteHDMITXP0 (TX_TMDS_CTRL2_ADDR, ((ReadByteHDMITXP0(TX_TMDS_CTRL2_ADDR) & 0xF8) | (nFFRCOUNT)));  // Use calculated nFFBCOUNT value rather
    // than 0x03. Value set after the
    // "switch (bRegVal)" statement in this function.
    //Consult DD.
    // TX_TMDS_CTRL3_ADDR
    //  [7]     reserved
    //  [6:3]   ITPLL
    //  [2:0]   FPOSTCOUNT*
    WriteByteHDMITXP0 (TX_TMDS_CTRL3_ADDR, 
        ((ReadByteHDMITXP0(TX_TMDS_CTRL3_ADDR) & 0xF8)  | (nFPOSTCOUNT)));

    return TMDS_SETUP_PASSED;
}

//------------------------------------------------------------------------------
// SI_Mpll_setup
//------------------------------------------------------------------------------
void SI_Mpll_setup(HI_U8 MpllSet)
{
    HI_U8 itpll, tpostcount, tfrcount;

    itpll = 0x06;
    switch (MpllSet)
    {
    default:
    case blue:
        tpostcount = 0x02;
        tfrcount   = 0x00;
        break;
    case yellow:
        tpostcount = 0x01;
        tfrcount   = 0x01;
        break;
    case orange:
        tpostcount = 0x00;
        tfrcount   = 0x02;
        break;
    }
    // TX_TMDS_CTRL2_ADDR
    //  [7:6]   TPOSTCOUNT*
    //  [5:3]   FFBCOUNT
    //  [2:0]   FFRCOUNT
    WriteByteHDMITXP0 (TX_TMDS_CTRL2_ADDR, ((ReadByteHDMITXP0(TX_TMDS_CTRL2_ADDR) & 0x3F) | (tpostcount << 6)));

    // TX_TMDS_CTRL3_ADDR
    //  [7]     reserved
    //  [6:3]   ITPLL*
    //  [2:0]   FPOSTCOUNT
    WriteByteHDMITXP0 (TX_TMDS_CTRL3_ADDR, ((ReadByteHDMITXP0(TX_TMDS_CTRL3_ADDR) & 0x87)  | (itpll << 3)));

    // TX_TMDS_CTRL4_ADDR
    //  [7:2]   reserved
    //  [1:0]   TFRPOSTCOUNT*
    WriteByteHDMITXP0 (TX_TMDS_CTRL4_ADDR, ((ReadByteHDMITXP0(TX_TMDS_CTRL4_ADDR) & 0xFC) | (tfrcount)));
}

//------------------------------------------------------------------------------
// SI_FApost_setup
//------------------------------------------------------------------------------
void SI_FApost_setup(HI_U8 RangeSet, int idclk_freq, HI_U8 bpp)
{
    HI_U8 nFAPOSTCOUNT = 0;
    switch (RangeSet)
    {
    default:
    case blue:
        switch (bpp)
        {
        default:
        case SiI_DeepColor_Off:
        case SiI_DeepColor_24bit:
            if (idclk_freq >= 44)
                nFAPOSTCOUNT = 1;
            break;
        case SiI_DeepColor_30bit:
            if (idclk_freq >= 33)
                nFAPOSTCOUNT = 1;
            break;
        case SiI_DeepColor_36bit:
            if (idclk_freq >= 30)
                nFAPOSTCOUNT = 1;
            break;
        }
        break;
    case yellow:
        switch (bpp)
        {
        default:
        case SiI_DeepColor_Off:
        case SiI_DeepColor_24bit:
            if (idclk_freq >= 86)
                nFAPOSTCOUNT = 1;
            break;
        case SiI_DeepColor_30bit:
            if (idclk_freq >= 71)
                nFAPOSTCOUNT = 1;
            break;
        case SiI_DeepColor_36bit:
            if (idclk_freq >= 58)
                nFAPOSTCOUNT = 1;
            break;
        }
        break;
    case orange:
        switch (bpp)
        {
        default:
        case SiI_DeepColor_Off:
        case SiI_DeepColor_24bit:
            if (idclk_freq >= 168)
                nFAPOSTCOUNT = 1;
            break;
        case SiI_DeepColor_30bit:
            if (idclk_freq >= 139)
                nFAPOSTCOUNT = 1;
            break;
        case SiI_DeepColor_36bit:
            if (idclk_freq >= 114)
                nFAPOSTCOUNT = 1;
            break;
        }
        break;
    }
    // TX_TMDS_CCTRL_ADDR
    //  [7:6]   reserved
    //  [5]     FAPOSTCOUNT*
    //  [4:0]   reserved
    WriteByteHDMITXP0 (TX_TMDS_CCTRL_ADDR, ((ReadByteHDMITXP0(TX_TMDS_CCTRL_ADDR) & 0xDF) | (nFAPOSTCOUNT << 5)));
}
#endif /*--NO MODIFY : COMMENT BY CODINGPARTNER--*/
