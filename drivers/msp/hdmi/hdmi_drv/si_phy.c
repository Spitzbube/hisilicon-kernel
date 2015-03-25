#include "si_phy.h"
#include "si_defstx.h"
#include "si_hdmitx.h"
#include "si_txapidefs.h"
#include "drv_reg_proc.h"
#include <linux/kernel.h>

#if defined(BOARD_TYPE_S40V2_fpga)    
//#include "drv_i2c_ext.h"
extern HI_S32 HI_DRV_I2C_Write(HI_U32 I2cNum, HI_U8 I2cDevAddr, HI_U32 I2cRegAddr, HI_U32 I2cRegAddrByteNum, HI_U8 *pData,
                        HI_U32 DataLen);

extern HI_S32 HI_DRV_I2C_Read(HI_U32 I2cNum, HI_U8 I2cDevAddr, HI_U32 I2cRegAddr, HI_U32 I2cRegAddrByteNum, HI_U8 *pData,
                       HI_U32 DataLen);

#define HDMI_TX_I2C_CHANNEL 1
#endif

HI_S32 SI_TX_PHY_WriteRegister(HI_U32 u32RegAddr, HI_U32 u32Value)
{
    HI_U32 u32Ret = HI_SUCCESS;
#if defined(BOARD_TYPE_S40V2_fpga)    
    //u32Ret = HI_DRV_GPIOI2C_Write(HDMI_TX_I2C_CHANNEL,0x60,u32RegAddr,u32Value);
    HI_INFO_HDMI("writing phy u32RegAddr 0x%x,u32Value 0x%x \n",u32RegAddr,u32Value);
    u32Ret = HI_DRV_I2C_Write(HDMI_TX_I2C_CHANNEL,0x60,u32RegAddr,1,(HI_U8 *)&u32Value,1);
#else
    u32Ret = DRV_HDMI_WriteRegister((HI_U32)(HDMI_TX_PHY_ADDR + u32RegAddr * 4),u32Value);
#endif
    return u32Ret;
}

HI_S32 SI_TX_PHY_ReadRegister(HI_U32 u32RegAddr, HI_U32 *pu32Value)
{
    HI_U32 u32Ret = HI_SUCCESS;
#if defined(BOARD_TYPE_S40V2_fpga)    
    u32Ret = HI_DRV_I2C_Read(HDMI_TX_I2C_CHANNEL,0x60,u32RegAddr,1,(HI_U8 *)pu32Value,1);
#else
    u32Ret = DRV_HDMI_ReadRegister((HI_U32)(HDMI_TX_PHY_ADDR + u32RegAddr * 4),pu32Value);
#endif
    return u32Ret;
}

HI_S32 SI_TX_PHY_HighBandwidth(HI_BOOL bTermEn)
{
    HI_U32 u32Ret = HI_SUCCESS;
    HI_U32 u32phyreg = 0;
        
#if defined(BOARD_TYPE_S40V2_fpga) 
    if(HI_TRUE == bTermEn)
    {
        HI_INFO_HDMI("From Jing:TMDS_CTL2[4]:term_en must be set to 1 for HDMI Eye-Diagram test\n");
        //set TMDS CNTL2 [4] to 1  
        SI_TX_PHY_ReadRegister(0x1,&u32phyreg);
        u32phyreg |= 0x10;
        HI_INFO_HDMI("writing phy 0x%x HighBandwidth 1\n",u32phyreg);
        SI_TX_PHY_WriteRegister(0x1, u32phyreg);
        SI_TX_PHY_ReadRegister(0x1, &u32phyreg);
        HI_INFO_HDMI("High Bandwith 0x10171804:0x%x\n", u32phyreg);    //0x10171804        
    }
    else
    {
        SI_TX_PHY_ReadRegister(0x1,&u32phyreg);
        u32phyreg &= 0xef;
        HI_INFO_HDMI("writing phy 0x%x HighBandwidth 0\n",u32phyreg);
        SI_TX_PHY_WriteRegister(0x1, u32phyreg);
        SI_TX_PHY_ReadRegister(0x1,&u32phyreg);
        HI_INFO_HDMI("low Bandwith 0x10171804:0x%x\n", u32phyreg);             
    }
#else  
    //新phy
    if(HI_TRUE == bTermEn)
    {
        HI_INFO_HDMI("From Jing:TMDS_CTL2[4]:term_en must be set to 1 for HDMI Eye-Diagram test\n");
        //set TMDS CNTL2 [4] to 1  
        SI_TX_PHY_ReadRegister(0x0e,&u32phyreg);
        u32phyreg |= 0x01;
        HI_INFO_HDMI("writing phy 0x%x HighBandwidth 1\n",u32phyreg);
        SI_TX_PHY_WriteRegister(0x0e, u32phyreg);
        SI_TX_PHY_ReadRegister(0x0e, &u32phyreg);
        HI_INFO_HDMI("High Bandwith 0x%x\n", u32phyreg);    //0x10171804        
    }
    else
    {
        SI_TX_PHY_ReadRegister(0x0e,&u32phyreg);
        u32phyreg &= ~0x01;
        HI_INFO_HDMI("writing phy 0x%x HighBandwidth 0\n",u32phyreg);
        SI_TX_PHY_WriteRegister(0x0e, u32phyreg);
        SI_TX_PHY_ReadRegister(0x0e,&u32phyreg);
        HI_INFO_HDMI("low Bandwith 0x%x\n", u32phyreg);             
    }
#endif
    return u32Ret;
}

HI_S32 SI_TX_PHY_GetOutPutEnable(void)
{
    HI_U32 u32Value = 0;
           
#if defined(BOARD_TYPE_S40V2_fpga) 
    SI_TX_PHY_ReadRegister(0x1,&u32Value);
    if ((u32Value & 0x20) != 0x20)
    {
        return HI_FALSE;
    }
    return HI_TRUE;
#else  
    //新phy
    SI_TX_PHY_ReadRegister(0x05,&u32Value);
    if ((u32Value & 0x20) != 0x20)
    {
        return HI_FALSE;
    }
    return HI_TRUE;
#endif
}

HI_S32 SI_TX_PHY_DisableHdmiOutput(void)
{
    HI_U32 u32Reg = 0;
    //HI_CHIP_TYPE_E enChip;
    //HI_CHIP_VERSION_E enChipVersion;
    //HI_U32 Ret;

    //HI_DRV_SYS_GetChipVersion(&enChip, &enChipVersion);
    
#if defined(BOARD_TYPE_S40V2_fpga) 
    /* disable HDMI PHY Output:TMDS CNTL2 Register:oe */
    SI_TX_PHY_ReadRegister(0x1,&u32Reg);
    u32Reg &= ~0x20;
    HI_INFO_HDMI("writing phy 0x%x DisableHdmiOutput\n",u32Reg);
    SI_TX_PHY_WriteRegister(0x1, u32Reg);
#else  
    //新phy
    /* disable HDMI PHY Output:TMDS CNTL2 Register:oe */
    SI_TX_PHY_ReadRegister(0x05,&u32Reg);
    u32Reg &= ~0x20;
    HI_INFO_HDMI("writing phy 0x%x DisableHdmiOutput\n",u32Reg);
    SI_TX_PHY_WriteRegister(0x05, u32Reg);
#endif

    return HI_SUCCESS;
}

HI_S32 SI_TX_PHY_EnableHdmiOutput(void)
{
    HI_U32 u32Reg = 0;
    //HI_CHIP_TYPE_E enChip;
    //HI_CHIP_VERSION_E enChipVersion;
    //HI_U32 Ret;

    //HI_DRV_SYS_GetChipVersion(&enChip, &enChipVersion);

    /* enable HDMI PHY Output:TMDS CNTL2 Register:oe */
#if defined(BOARD_TYPE_S40V2_fpga) 
    SI_TX_PHY_ReadRegister(0x1,&u32Reg);
    HI_INFO_HDMI("writing phy 0x%x EnableHdmiOutput\n",u32Reg);
    u32Reg |= 0x20;
    HI_INFO_HDMI("writing phy 0x%x EnableHdmiOutput\n",u32Reg);
    SI_TX_PHY_WriteRegister(0x1, u32Reg);
#else  
    //新phy
    SI_TX_PHY_ReadRegister(0x05,&u32Reg);
    HI_INFO_HDMI("writing phy 0x%x EnableHdmiOutput\n",u32Reg);
    u32Reg |= 0x20;
    HI_INFO_HDMI("writing phy 0x%x EnableHdmiOutput\n",u32Reg);
    SI_TX_PHY_WriteRegister(0x05, u32Reg);
#endif

    return HI_SUCCESS;
}

void SI_TX_PHY_INIT(void)
{
     //HI_CHIP_TYPE_E enChip;
    //HI_CHIP_VERSION_E enChipVersion;
    
    //HI_DRV_SYS_GetChipVersion(&enChip, &enChipVersion);

#if defined(BOARD_TYPE_S40V2_fpga)  
    HI_U32 u32Value = 0;  
    /* hdmi PHY */
    SI_TX_PHY_WriteRegister(0x1, (HI_U32)0x00000008);
    //u32Value = hdmi_gpio_i2c_read(0x60, 0x1);
    SI_TX_PHY_ReadRegister(0x1,&u32Value);
    HI_INFO_HDMI("TMDS_CTL2 set to:0x%x\n", u32Value);
    SI_TX_PHY_DisableHdmiOutput();
    //SI_TX_PHY_EnableHdmiOutput();//temp 等out put通了再关掉
    SI_TX_PHY_WriteRegister(0x8, (HI_U32)0x00000060);
    SI_TX_PHY_WriteRegister(0x2, (HI_U32)0x000000a9);
    SI_TX_PHY_WriteRegister(0x3, (HI_U32)0x00000040);
    SI_TX_PHY_WriteRegister(0x6, (HI_U32)0x00000010);    
#else      
    // unknown DM_TX_CTRL2
    SI_TX_PHY_WriteRegister(0x06,0x89);
    // term_en && cap_ctl  // term_en 先关掉
    SI_TX_PHY_WriteRegister(0x0e,0x00);
    // unknown DM_TX_CTRL3
    SI_TX_PHY_WriteRegister(0x07,0x81);
    // unknown DM_TX_CTRL5
    SI_TX_PHY_WriteRegister(0x09,0x1a);
    // unknown BIAS_GEN_CTRL1 
    SI_TX_PHY_WriteRegister(0x0a,0x07);
    // unknown BIAS_GEN_CTRL2
    SI_TX_PHY_WriteRegister(0x0b,0x51);
    // pll ctrl -deep color
    SI_TX_PHY_WriteRegister(0x02,0x24);
    // oe && pwr_down 
    // 初始化时不打开oe
    SI_TX_PHY_WriteRegister(0x05,0x12);
    //unknown DM_TX_CTRL4 
    SI_TX_PHY_WriteRegister(0x08,0x40);
    // enc_bypass == nobypass
    SI_TX_PHY_WriteRegister(0x0d,0x00);
#endif
}

HI_S32 SI_TX_PHY_PowerDown(HI_BOOL bPwdown)
{
    HI_U32 u32Value = 0;
#if defined(BOARD_TYPE_S40V2_fpga)  
    /* hdmi PHY */
    SI_TX_PHY_ReadRegister(0x2,&u32Value);
    if(bPwdown)
    {
        u32Value &= ~0x01;
    }
    else
    {
        u32Value |= 0x01;
    }
    HI_INFO_HDMI("writing phy 0x%x PowerDown\n",u32Value);
    SI_TX_PHY_WriteRegister(0x2,u32Value);
#else  
    //新phy
    /* hdmi PHY */
    SI_TX_PHY_ReadRegister(0x05,&u32Value);
    if(bPwdown)
    {
        u32Value &= ~0x10;
    }
    else
    {
        u32Value |= 0x10;
    }
    HI_INFO_HDMI("writing phy 0x%x PowerDown\n",u32Value);
    SI_TX_PHY_WriteRegister(0x05,u32Value);
#endif

   return HI_SUCCESS;
}

HI_S32 SI_TX_PHY_SetDeepColor(HI_U8 bDeepColor)
{
    HI_U32 u32Value = 0;
#if defined(BOARD_TYPE_S40V2_fpga)  
    /* Config rooG IP for DeepColor*/
    SI_TX_PHY_ReadRegister(0x02,&u32Value);

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
    
    HI_INFO_HDMI("writing phy 0x%x SetDeepColor\n",u32Value);
    
    SI_TX_PHY_WriteRegister(0x02,u32Value);
    u32Value = 0;
    SI_TX_PHY_ReadRegister(0x02,&u32Value);
    HI_INFO_HDMI("TMDS_CTL3 new walue:0x%x\n", u32Value);
#else  
    //新phy
    /* Config kudu IP for DeepColor*/
    SI_TX_PHY_ReadRegister(0x02,&u32Value);

    HI_INFO_HDMI("PLL_CTRL  old walue:0x%x\n", u32Value);
    
    if (SiI_DeepColor_30bit == bDeepColor)
    {
        u32Value =  (u32Value & ~0x03) | 0x01;
        HI_INFO_HDMI("SiI_DeepColor_30bit\n");
    }
    else if (SiI_DeepColor_36bit == bDeepColor)
    {
        u32Value =  (u32Value & ~0x03) | 0x02;
        HI_INFO_HDMI("SiI_DeepColor_36bit\n");
    }
    else if (SiI_DeepColor_24bit == bDeepColor)
    {
        u32Value =  (u32Value & ~0x03) | 0x00;
        HI_INFO_HDMI("SiI_DeepColor_24bit(normal)\n");
    }
    else
    {
        u32Value =  (u32Value & ~0x03) | 0x00;
        HI_INFO_HDMI("SiI_DeepColor_Off\n");
    } 
    
    HI_INFO_HDMI("writing phy 0x%x SetDeepColor\n",u32Value);
    
    SI_TX_PHY_WriteRegister(0x02,u32Value);
    u32Value = 0;
    SI_TX_PHY_ReadRegister(0x02,&u32Value);
    HI_INFO_HDMI("PLL_CTRL  new walue:0x%x\n", u32Value);
#endif
    return HI_SUCCESS;
}

