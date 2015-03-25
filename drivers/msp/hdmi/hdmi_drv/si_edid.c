/***********************************************************************************/
/*  Copyright (c) 2002-2006, Silicon Image, Inc.  All rights reserved.             */
/*  No part of this work may be reproduced, modified, distributed, transmitted,    */
/*  transcribed, or translated into any language or computer format, in any form   */
/*  or by any means without written permission of: Silicon Image, Inc.,            */
/*  1060 East Arques Avenue, Sunnyvale, California 94085                           */
/***********************************************************************************/
#include <linux/string.h>
#include <linux/delay.h>
#include <linux/pm.h>
#include "si_cec.h"  
#include "si_amf.h"  
#include "si_config.h"  
#include "si_timer.h"  
#include "si_edid.h"
#include "si_crc16.h"
#include "si_eeprom.h"
#include "si_mddc.h"
#include "si_global.h"
#include "si_hdcpdefs.h"
#include "si_vmode.h"  
#include "hi_unf_hdmi.h"
#include "drv_hdmi.h"
#include "hi_drv_sys.h"
#include "si_phy.h"
#include "drv_gpio_ext.h"

#include "si_delay.h"

#include "hi_reg_common.h"
#include "hi_drv_module.h"


#if 0

#define  GPIO_0_ADDR             0xF8B20000
#define  GPIO_1_ADDR             0xF8B21000
#define  GPIO_2_ADDR             0xF8B22000
#define  GPIO_3_ADDR             0xF8B23000
#define  GPIO_4_ADDR             0xF8B24000
#define  GPIO_5_ADDR             0xF8B25000
#define  GPIO_6_ADDR             0xF8B26000
#define  GPIO_7_ADDR             0xF8B27000
#define  GPIO_8_ADDR             0xF8B28000
#define  GPIO_9_ADDR             0xF8B29000
#define  GPIO_10_ADDR            0xF8B2A000
#define  GPIO_11_ADDR            0xF8B2B000
#define  GPIO_12_ADDR            0xF8B2C000

//GPIO 13~17 �Ȳ���

#define  GPIO_DIR_REG            0x400
#define  GPIO_BIT_NUM            8
#define  GPIO_OUTPUT             1
#define	 GPIO_INPUT              0

#endif

struct SI_3D_Detail
{
    HI_U8 u82D_VIC_order;
    HI_U8 u83D_Structure;
    HI_U8 u83D_Detail;
};

typedef struct
{
    HI_U8 u8HDMI_Video_present;
    HI_U8 u83D_Present;
    HI_U8 u83D_Multi_present;
    HI_U8 u8HDMI_VIC_LEN;
    HI_U8 u8HDMI_3D_LEN;
    HI_U8 u8HDMI_VICNum;
    HI_U8 u8HDMI_VIC[8];
    HI_U8 u83D_Sturcture_ALL_15_8;
    HI_U8 u83D_Sturcture_ALL_7_0;
    HI_U8 u83D_MASK_15_8;
    HI_U8 u83D_MASK_7_0;
    HI_U8 SI_3D_DetailNum;
    struct SI_3D_Detail s3DDetail[16];
}SI_Vendor_3D_S;


static SI_Vendor_3D_S stVendor3D;
static HI_BOOL g_bHDMIFlag = HI_FALSE;

#if defined (DEBUG_EDID)

static HI_U32 __COUNT__ = 0;
//EDID Extarn Block >=2 test example
static HI_U8 EDID_4BLOCK[512] = {
0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x4D, 0x29, 0x48, 0x44, 0x01, 0x00, 0x00, 0x00,	
0x0A, 0x0D, 0x01, 0x03, 0x80, 0x50, 0x2D, 0x78, 0x0A, 0x0D, 0xC9, 0xA0, 0x57, 0x47, 0x98, 0x27, 0x12, 0x48, 0x4C, 0x20, 0x00,	
0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x1D, 0x80, 0x18,	
0x71, 0x1C, 0x16, 0x20, 0x58, 0x2C, 0x25, 0x00, 0x20, 0xC2, 0x31, 0x00, 0x00, 0x9E, 0x8C, 0x0A, 0xD0, 0x8A, 0x20, 0xE0, 0x2D,	
0x10, 0x10, 0x3E, 0x96, 0x00, 0x13, 0x8E, 0x21, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x48, 0x44, 0x4D, 0x49, 0x20,	
0x54, 0x56, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0xFD, 0x00, 0x3B, 0x3D, 0x0F, 0x2E, 0x08, 0x02, 0x00, 0x20,	
0x20, 0x20, 0x20, 0x20, 0x20, 0x03,	0xF1,

0xF0, 0x02, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	0x0C,

0x02, 0x03, 0x1E, 0xF1, 0x4A, 0x85, 0x04, 0x10, 0x02, 0x01, 0x06, 0x14, 0x12, 0x16, 0x13, 0x23,	
0x09, 0x07, 0x07, 0x83, 0x01, 0x00, 0x00, 0x66, 0x03, 0x0C, 0x00, 0x10, 0x00, 0x80, 0x01, 0x1D, 0x00, 0x72, 0x51, 0xD0, 0x1E,	
0x20, 0x6E, 0x28, 0x55, 0x00, 0xC4, 0x8E, 0x21, 0x00, 0x00, 0x1E, 0xD6, 0x09, 0x80, 0xA0, 0x20, 0xE0, 0x2D, 0x10, 0x10, 0x60,	
0x22, 0x00, 0x12, 0x8E, 0x21, 0x08, 0x08, 0x18, 0x8C, 0x0A, 0xD0, 0x90, 0x20, 0x40, 0x31, 0x20, 0x0C, 0x40, 0x55, 0x00, 0xC4,	
0x8E, 0x21, 0x00, 0x00, 0x18, 0x01, 0x1D, 0x80, 0xD0, 0x72, 0x1C, 0x16, 0x20, 0x10, 0x2C, 0x25, 0x80, 0xC4, 0x8E, 0x21, 0x00,	
0x00, 0x9E, 0x8C, 0x0A, 0xA0, 0x14, 0x51, 0xF0, 0x16, 0x00, 0x26, 0x7C, 0x43, 0x00, 0x13, 0x8E, 0x21, 0x00, 0x00, 0x98, 0x00,	
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	0xF5,

0x02, 0x03, 0x04, 0xF1, 0xF3, 0x39, 0x80, 0x18, 0x71, 0x38, 0x2D, 0x40, 0x58, 0x2C, 0x45, 0x00,	
0xC4, 0x8E, 0x21, 0x00, 0x00, 0x1E, 0x8C, 0x0A, 0xA0, 0x20, 0x51, 0x20, 0x18, 0x10, 0x18, 0x7E, 0x23, 0x00, 0xC4, 0x8E, 0x21,	
0x00, 0x00, 0x98, 0x01, 0x1D, 0x00, 0xBC, 0x52, 0xD0, 0x1E, 0x20, 0xB8, 0x28, 0x55, 0x40, 0xC4, 0x8E, 0x21, 0x00, 0x00, 0x1E,	
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	0xDF,
};

static HI_U8 EDID_3BLOCK0[384] = 
{
0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x4D,0x29,0x02,0x80,0x01,0x00,0x00,0x00,
0x0F,0x10,0x01,0x03,0x80,0x50,0x2D,0x78,0x0A,0x0D,0xC9,0xA0,0x57,0x47,0x98,0x27,
0x12,0x48,0x4C,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x1D,0x80,0x18,0x71,0x1C,0x16,0x20,0x58,0x2C,
0x25,0x00,0xC4,0x8E,0x21,0x00,0x00,0x9E,0x8C,0x0A,0xD0,0x8A,0x20,0xE0,0x2D,0x10,
0x10,0x3E,0x96,0x00,0xC4,0x8E,0x21,0x00,0x00,0x18,0x00,0x00,0x00,0xFC,0x00,0x34,
0x20,0x62,0x6C,0x6F,0x63,0x6B,0x0A,0x20,0x20,0x20,0x20,0x20,0x00,0x00,0x00,0xFD,
0x00,0x32,0x3D,0x1C,0x22,0x08,0x00,0x0A,0x20,0x20,0x20,0x20,0x20,0x20,0x02,0x90,
0xF0,0x02,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0C,
0x02,0x03,0x15,0x71,0x46,0x85,0x03,0x02,0x14,0x12,0x11,0x23,0x0D,0x7F,0x07,0x65,
0x03,0x0C,0x00,0x8C,0x00,0x8C,0x0A,0xD0,0x8A,0x20,0xE0,0x2D,0x10,0x10,0x3E,0x96,
0x00,0x13,0x8E,0x21,0x00,0x00,0x18,0x01,0x1D,0x80,0xD0,0x72,0x1C,0x16,0x20,0x10,
0x2C,0x25,0x80,0xC4,0x8E,0x21,0x00,0x00,0x9E,0x8C,0x0A,0xD0,0x90,0x20,0x40,0x31,
0x20,0x0C,0x40,0x55,0x00,0xC4,0x8E,0x21,0x00,0x00,0x18,0x8C,0x0A,0xD0,0x90,0x20,
0x40,0x31,0x20,0x0C,0x40,0x55,0x00,0x13,0x8E,0x21,0x00,0x00,0x18,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xB4,
};

static HI_U8 EDID_3BLOCK1[384] = 
{
0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x4D,0x29,0x0C,0x80,0x01,0x00,0x00,0x00,
0x0F,0x10,0x01,0x03,0x80,0x50,0x2D,0x78,0x0A,0x0D,0xC9,0xA0,0x57,0x47,0x98,0x27,
0x12,0x48,0x4C,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x1D,0x80,0x18,0x71,0x1C,0x16,0x20,0x58,0x2C,
0x25,0x00,0xC4,0x8E,0x21,0x00,0x00,0x9E,0x8C,0x0A,0xD0,0x8A,0x20,0xE0,0x2D,0x10,
0x10,0x3E,0x96,0x00,0xC4,0x8E,0x21,0x00,0x00,0x18,0x00,0x00,0x00,0xFC,0x00,0x34,
0x20,0x62,0x6C,0x6F,0x63,0x6B,0x23,0x32,0x0A,0x20,0x20,0x20,0x00,0x00,0x00,0xFD,
0x00,0x32,0x3D,0x1C,0x22,0x08,0x00,0x0A,0x20,0x20,0x20,0x20,0x20,0x20,0x02,0x71,
0xF0,0x02,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0C,
0x02,0x04,0x15,0x71,0x46,0x85,0x03,0x02,0x14,0x12,0x11,0x23,0x0D,0x7F,0x07,0x65,
0x03,0x0C,0x00,0x8C,0x00,0x8C,0x0A,0xD0,0x8A,0x20,0xE0,0x2D,0x10,0x10,0x3E,0x96,
0x00,0x13,0x8E,0x21,0x00,0x00,0x18,0x01,0x1D,0x80,0xD0,0x72,0x1C,0x16,0x20,0x10,
0x2C,0x25,0x80,0xC4,0x8E,0x21,0x00,0x00,0x9E,0x8C,0x0A,0xD0,0x90,0x20,0x40,0x31,
0x20,0x0C,0x40,0x55,0x00,0xC4,0x8E,0x21,0x00,0x00,0x18,0x8C,0x0A,0xD0,0x90,0x20,
0x40,0x31,0x20,0x0C,0x40,0x55,0x00,0x13,0x8E,0x21,0x00,0x00,0x18,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xB3,
};

//no natice format tag,so native is 1080P50
static HI_U8 EDID_2BLOCK_NONATIVE[256] = 
{
    0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x4D,0x29,0x00,0x00,0x01,0x01,0x01,0x01,
    0x00,0x00,0x01,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
    0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0A,0x20,0x20,0x20,0x20,0x20,0x20,0x01,0xAD,
    0x02,0x03,0x29,0xf0,0x50,0x1F,0x10,0x14,0x05,0x13,0x04,0x12,0x11,0x16,0x15,0x03,
    0x02,0x07,0x06,0x01,0x20,0x23,0x09,0x07,0x07,0x83,0x01,0x00,0x00,0x68,0x03,0x0c,
    0x00,0x20,0x00,0x80,0x00,0x0f,0x02,0x00,0x09,0x02,0x3a,0x80,0xd0,0x72,0x38,0x2d,
    0x40,0x10,0x2c,0x45,0x80,0x40,0x84,0x63,0x00,0x00,0x1e,0x01,0x1d,0x00,0xbc,0x52,
    0xd0,0x1e,0x20,0xb8,0x28,0x55,0x40,0x40,0x84,0x63,0x00,0x00,0x1e,0x01,0x1d,0x80,
    0x18,0x71,0x1c,0x16,0x20,0x58,0x2c,0x25,0x00,0x40,0x84,0x63,0x00,0x00,0x9e,0x01,
    0x1d,0x80,0xd0,0x72,0x1c,0x16,0x20,0x10,0x2c,0x25,0x80,0x40,0x84,0x63,0x00,0x00,
    0x9e,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x27,
};

//native format is 576P50
static HI_U8 EDID_2BLOCK_WITH_NATIVE[256] = 
{
    0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x4D,0x29,0x00,0x00,0x01,0x01,0x01,0x01,
    0x00,0x00,0x01,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
    0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0A,0x20,0x20,0x20,0x20,0x20,0x20,0x01,0xAD,
    0x02,0x03,0x29,0xf0,0x50,0x1F,0x10,0x14,0x05,0x13,0x04,0x92,0x11,0x16,0x15,0x03,
    0x02,0x07,0x06,0x01,0x20,0x23,0x09,0x07,0x07,0x83,0x01,0x00,0x00,0x68,0x03,0x0c,
    0x00,0x20,0x00,0x80,0x00,0x0f,0x02,0x00,0x09,0x02,0x3a,0x80,0xd0,0x72,0x38,0x2d,
    0x40,0x10,0x2c,0x45,0x80,0x40,0x84,0x63,0x00,0x00,0x1e,0x01,0x1d,0x00,0xbc,0x52,
    0xd0,0x1e,0x20,0xb8,0x28,0x55,0x40,0x40,0x84,0x63,0x00,0x00,0x1e,0x01,0x1d,0x80,
    0x18,0x71,0x1c,0x16,0x20,0x58,0x2c,0x25,0x00,0x40,0x84,0x63,0x00,0x00,0x9e,0x01,
    0x1d,0x80,0xd0,0x72,0x1c,0x16,0x20,0x10,0x2c,0x25,0x80,0x40,0x84,0x63,0x00,0x00,
    0x9e,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xA7,
};

#endif

HI_BOOL g_bHdmiEdidPrepareOk = HI_FALSE;
static HI_U8 EDIDData[512];
static HI_UNF_HDMI_SINK_CAPABILITY_S g_stSinkCap;

static HI_BOOL g_forceDVIMode  = HI_FALSE;
static HI_BOOL g_forceHDMIMode = HI_FALSE;
Bool CEAOnFisrtPage;  

int g_s32VmodeOfUNFFormat[HI_UNF_ENC_FMT_BUTT] = {0};

static void SaveEdidToEeprom(HI_U8 offset, HI_U8 ucLen, HI_U8 *pucData)
{
    HI_U16 uwAddr = EE_SINK_CAP_ADDR + offset;
	
    SI_BlockWriteEEPROM(ucLen, uwAddr, pucData);    
    return ;
}

#if 0 /*--NO MODIFY : COMMENT BY CODINGPARTNER--*/
static HI_U32 SI_GPIO_SetDirBit(HI_U32 u32GpioNo, HI_U8 GpioBitNum, HI_BOOL	bInput)
{
    //��Ҫ��
    HI_U32 Ret;
    HI_U32 GpioUsrAddr = 0, value;
    
    switch (u32GpioNo)
    {
    case 0:
        GpioUsrAddr = GPIO_0_ADDR;
        break;
    case 1:
        GpioUsrAddr = GPIO_1_ADDR;
        break;
    case 2:
        GpioUsrAddr = GPIO_2_ADDR;
        break;
    case 3:
        GpioUsrAddr = GPIO_3_ADDR;
        break;
    case 4:
        GpioUsrAddr = GPIO_4_ADDR;
        break;
    case 5:
        GpioUsrAddr = GPIO_5_ADDR;
        break;
    case 6:
        GpioUsrAddr = GPIO_6_ADDR;
        break;
    case 7:
        GpioUsrAddr = GPIO_7_ADDR;
        break;
    case 8:
        GpioUsrAddr = GPIO_8_ADDR;
        break;
    default:
        HI_ERR_HDMI("Do not support GPIO:%d\n", u32GpioNo);
        return HI_FAILURE;
    }
    
    GpioUsrAddr += GPIO_DIR_REG;
    
    DRV_HDMI_ReadRegister(GpioUsrAddr, &value);

	if (!bInput)
	{
        value |= (GPIO_OUTPUT << GpioBitNum);
        Ret = DRV_HDMI_WriteRegister(GpioUsrAddr, value);
	}
	else
	{
        value &= (~(1 << GpioBitNum));
        Ret = DRV_HDMI_WriteRegister(GpioUsrAddr, value);
	}
	
	return HI_SUCCESS;    
}

static HI_U32 SI_GPIO_WriteBit(HI_U32 u32GpioNo, HI_U8 GpioBitNum, HI_BOOL bHighVolt)
{
    HI_U32 Ret;
    HI_U32 GpioUsrAddr = 0, value;
    
    switch (u32GpioNo)
    {
    case 0:
        GpioUsrAddr = GPIO_0_ADDR;
        break;
    case 1:
        GpioUsrAddr = GPIO_1_ADDR;
        break;
    case 2:
        GpioUsrAddr = GPIO_2_ADDR;
        break;
    case 3:
        GpioUsrAddr = GPIO_3_ADDR;
        break;
    case 4:
        GpioUsrAddr = GPIO_4_ADDR;
        break;
    case 5:
        GpioUsrAddr = GPIO_5_ADDR;
        break;
    case 6:
        GpioUsrAddr = GPIO_6_ADDR;
        break;
    case 7:
        GpioUsrAddr = GPIO_7_ADDR;
        break;
    case 8:
        GpioUsrAddr = GPIO_8_ADDR;
        break;
    default:
        HI_ERR_HDMI("Do not support GPIO:%d\n", u32GpioNo);
        return HI_FAILURE;
    }
    
    //Check GPIO Direct
    DRV_HDMI_ReadRegister((GpioUsrAddr + GPIO_DIR_REG), &value);

    value &= (1 << GpioBitNum);
    value = value >> GpioBitNum;
	if (value != GPIO_OUTPUT)
	{
	    HI_ERR_HDMI("GPIO Direction is input, write can not operate.\n");
	    return HI_FAILURE;
	}

    if (bHighVolt)
		Ret = DRV_HDMI_WriteRegister((GpioUsrAddr + (4 << GpioBitNum)), (1 << GpioBitNum));    
    else
		Ret = DRV_HDMI_WriteRegister((GpioUsrAddr + (4 << GpioBitNum)), 0);

	return HI_SUCCESS;    
}

static HI_U32 SI_GPIO_ReadBit(HI_U32 u32GpioNo, HI_U8 GpioBitNum, HI_BOOL *pbHighVolt)
{
    HI_U32 GpioUsrAddr = 0, value;
    
    switch (u32GpioNo)
    {
    case 0:
        GpioUsrAddr = GPIO_0_ADDR;
        break;
    case 1:
        GpioUsrAddr = GPIO_1_ADDR;
        break;
    case 2:
        GpioUsrAddr = GPIO_2_ADDR;
        break;
    case 3:
        GpioUsrAddr = GPIO_3_ADDR;
        break;
    case 4:
        GpioUsrAddr = GPIO_4_ADDR;
        break;
    case 5:
        GpioUsrAddr = GPIO_5_ADDR;
        break;
    case 6:
        GpioUsrAddr = GPIO_6_ADDR;
        break;
    case 7:
        GpioUsrAddr = GPIO_7_ADDR;
        break;
    case 8:
        GpioUsrAddr = GPIO_8_ADDR;
        break;
    default:
        HI_ERR_HDMI("Do not support GPIO:%d\n", u32GpioNo);
        return HI_FAILURE;
    }
    
    //Check GPIO Direct
    DRV_HDMI_ReadRegister((GpioUsrAddr + GPIO_DIR_REG), &value);
    
    value &= (1 << GpioBitNum);
    value = value >> GpioBitNum;
	if (value != GPIO_INPUT)
	{
	    HI_ERR_HDMI("GPIO Direction is input, write can not operate.\n");
	    return HI_FAILURE;
	}
    
    DRV_HDMI_ReadRegister((GpioUsrAddr + (4 << GpioBitNum)), &value);
    *pbHighVolt = value >> GpioBitNum;

	return HI_SUCCESS;    
}
#endif /*--NO MODIFY : COMMENT BY CODINGPARTNER--*/



static HI_U32 SI_DDC_Adjust(void)
{
    HI_U32 ret;
    HI_U32 value;
    HI_S32 count = 0;
    GPIO_EXT_FUNC_S *gpio_func_ops = HI_NULL;
    
    HI_INFO_HDMI("Enter SI_DDC_Adjust \n");

    ret = HI_DRV_MODULE_GetFunction(HI_ID_GPIO, (HI_VOID**)&gpio_func_ops);    
    if((NULL == gpio_func_ops) || (ret != HI_SUCCESS))
    {
        HI_FATAL_HDMI("can't get gpio funcs!\n");
        return HI_FAILURE;
    }
    
    /*if func ops and func ptr is null, then return ;*/
    if(!gpio_func_ops || !gpio_func_ops->pfnGpioDirSetBit ||
        !gpio_func_ops->pfnGpioReadBit || !gpio_func_ops->pfnGpioWriteBit)
    {
        HI_INFO_HDMI("gpio hdmi init err\n");
        return HI_FAILURE;
    }

    //when DDC_DELAY_CNT = 0x14  oscclk   dscl = oscclk / (DDC_DELAY_CNT * 30) = 100khz
    // dscl should in range 48Khz ~ 72Khz
    //50khz
    //DRV_HDMI_WriteRegister(0xf8ce03d8, 0x28); //DDC_DELAY_CNT
    //100khz
    //DRV_HDMI_WriteRegister(0xf8ce03d8, 0x14); //DDC_DELAY_CNT

    // 75Khz DDC_DELAY_CNT
    WriteByteHDMITXP0(DDC_DELAY_CNT,0x1a);
    // 63Khz
    //DRV_HDMI_WriteRegister(0xf8ce03d8, 0x1f); //DDC_DELAY_CNT
    // phy reg3  osc clk : 57.56 ~ 72.23 Mhz  delay = 0x60  ===> I2C clk 75 ~ 94khz
    //why 21khz
    //DRV_HDMI_WriteRegister(0xf8ce03d8, 0x60); //DDC_DELAY_CNT

    DelayMS(1);
    
    //bit 6  bus_low
    value =ReadByteHDMITXP0(DDC_STATUS);
    HI_INFO_HDMI("DDC Status(0x72:0xf2):0x%x\n", value);

    //gpio 
    if(g_pstRegIO->ioshare_reg0.u32 == 0x01)
    {
        //PIO Mode
        //ioshare reg0 0x00 GPIO0_0 // HDMI_TX_SCL
        //ioshare reg1 0x00 GPIO0_1 // HDMI_TX_SDA  
        g_pstRegIO->ioshare_reg0.u32 = 0x00;
        g_pstRegIO->ioshare_reg1.u32 = 0x00;

        
        //if SDA is low, need to create clock in SCL
        //SDA is high
        
        //Set GPIO0_1:HDMI_SDA input
        //  u32GpioNo = ���*8 + ������ı��
        //Ret = HI_UNF_GPIO_SetDirBit(0*8 + 1, HI_TRUE);//GPIO0_1:HDMI_SDA
        //SI_GPIO_SetDirBit(0, 1, HI_TRUE);
        gpio_func_ops->pfnGpioDirSetBit(1,HI_TRUE);
        
        
        //Ret = HI_UNF_GPIO_SetDirBit(0, HI_FALSE);//GPIO0_0:HDMI_SCL
        //SI_GPIO_SetDirBit(0, 0, HI_FALSE);
        gpio_func_ops->pfnGpioDirSetBit(0,HI_FALSE);
        
        //Ret = HI_UNF_GPIO_ReadBit(24, &value);//GPIO0_1:HDMI_SDA
        //SI_GPIO_ReadBit(0, 1, &value);
        gpio_func_ops->pfnGpioReadBit(1,&value);
        
        HI_INFO_HDMI("Current Value:%d\n", value);
        //in order to pull up HDMI_SDA,try to set HDMI_SCL repeat
        while ((value == 0) && ((count ++) < 20))
        {
            HI_INFO_HDMI("***Pull up SCL Voltage for DDC, count:%d\n", count);
            //Ret = HI_UNF_GPIO_WriteBit(25, HI_TRUE);//GPIO0_0:HDMI_SCL
            //SI_GPIO_WriteBit(0, 0, HI_TRUE);
            gpio_func_ops->pfnGpioWriteBit(0,HI_TRUE);
            mdelay(1);//msleep(1);
            //Ret = HI_UNF_GPIO_WriteBit(25, HI_FALSE);//GPIO0_0:HDMI_SCL
            //SI_GPIO_WriteBit(0, 0, HI_FALSE);
            gpio_func_ops->pfnGpioWriteBit(0,HI_FALSE);
            mdelay(1);//msleep(1);
            
            //Ret = HI_UNF_GPIO_ReadBit(24, &value);//GPIO0_1:HDMI_SDA
            //SI_GPIO_ReadBit(0, 1, &value);
            gpio_func_ops->pfnGpioReadBit(1,&value);
        }

        if (value == 0)
        {
            HI_ERR_HDMI("Can not pull up SCL Voltage\n");
        }
        
        //SDA/SCL Mode
        //ioshare reg0 0x01 HDMI_TX_SCL
        //ioshare reg1 0x01 HDMI_TX_SDA  //DDC 

        g_pstRegIO->ioshare_reg0.u32 = 0x01;
        g_pstRegIO->ioshare_reg1.u32 = 0x01;
    }

    return HI_SUCCESS;
}

HI_U8 SI_ResetEDIDBlock(void)
{
    int index;
    HI_U8  ucData[16];
    memset(ucData, 0, 16);
    for (index = 0; index < 512; index += 16/*8*/)
    {
	/* Write EDID into EE_EDID_DATA buffer */
       SI_BlockWriteEEPROM(16/*8*/, EE_EDID_DATA + index, ucData);
    }
    memset(EDIDData, 0, 512);
    return HI_SUCCESS;
}

HI_U8 SI_Proc_writeEDIDBlock(HI_U32 offset, HI_U8 *DataBlock, HI_U32 size)
{
    if ((offset + size) > 512)
    {
        DEBUG_PRINTK("error data size\n");
        return HI_FAILURE;
    }
    memcpy((EDIDData + offset), DataBlock, size);
    return HI_SUCCESS;
}

HI_U8 SI_Proc_ReadEDIDBlock(HI_U8 *DataBlock, HI_U32 size)
{
    if (size > 512)
    {
        size = 512;
    }

    memcpy(DataBlock, EDIDData, size);
    return HI_SUCCESS;
}

HI_U8 SI_ByteReadEDID(HI_U8 Block_Offset, HI_U8 Addr, HI_U8 * Data)
{
    SI_BlockReadEEPROM(1, (EE_EDID_DATA + Block_Offset * 256 + Addr), Data);
    return HI_SUCCESS;
}

HI_U8 SI_BlockReadEDID(HI_U8 NBytes, HI_U8 Addr, HI_U8 * Data)
{
    SI_BlockReadEEPROM(NBytes, EE_EDID_DATA + Addr, Data);
    return HI_SUCCESS;
}

HI_U8 SI_BlockReadEDID_Real(HI_U8 NBytes, HI_U16 Addr, HI_U8 * Data)
{
    HI_U8 ret = 0;
    TmpDType TmpD;
    memset((void*)&TmpD, 0, sizeof(TmpD));

    TmpD.MDDC.SlaveAddr = EDID_SLV;
    TmpD.MDDC.Offset = Addr/256;
    TmpD.MDDC.RegAddr = Addr%256;
    TmpD.MDDC.NBytesLSB = NBytes;
    TmpD.MDDC.NBytesMSB = 0;
    TmpD.MDDC.Dummy = 0;

    if(TmpD.MDDC.Offset)
        TmpD.MDDC.Cmd = MASTER_CMD_ENH_RD;
    else
        TmpD.MDDC.Cmd = MASTER_CMD_SEQ_RD;
    TmpD.MDDC.PData = Data;

    MDDCWriteOffset(Addr/256);     
    ret = SI_BlockRead_MDDC(&TmpD.MDDC);
	
    MDDCWriteOffset(0); //reset Segment offset to 0   
#if defined (DEBUG_EDID)
#if 0  
{
    int index = 0;
    for(index = 0; index < NBytes; index ++)
    {
        Data[index] = EDID_4BLOCK[__count__ + index];
        DEBUG_PRINTK("index:%d, data:0x%02x\n", __count__ + index, Data[index]);
    }  
    __count__ += NBytes; 
} 
#endif

#if 0  
{
    int index = 0;
    for(index = 0; index < NBytes; index ++)
    {
        Data[index] = EDID_2BLOCK_WITH_NATIVE[__COUNT__ + index];
        DEBUG_PRINTK("index:%d, data:0x%02x\n", __COUNT__ + index, Data[index]);
    }  
    __COUNT__ += NBytes; 
} 
#endif

#if 1  
    {
        int index = 0;
        for(index = 0; index < NBytes; index ++)
        {
            Data[index] = EDID_2BLOCK_NONATIVE[__COUNT__ + index];
            DEBUG_PRINTK("index:%d, data:0x%02x\n", __COUNT__ + index, Data[index]);
        }  
        __COUNT__ += NBytes; 
    } 
#endif

    
#if 0  //__HDMI_SIMPLYHD_TEST__
{
    int index = 0;
    for(index = 0; index < NBytes; index ++)
    {
        Data[index] = EDID_3BLOCK0[__COUNT__ + index];
        DEBUG_PRINTK("index:%d, data:0x%02x\n", __COUNT__ + index, Data[index]);
    }  
    __COUNT__ += NBytes; 
} 
#endif
    
#if 0  //__HDMI_SIMPLYHD_TEST__
{
    int index = 0;
    for(index = 0; index < NBytes; index ++)
    {
        Data[index] = EDID_3BLOCK1[__COUNT__ + index];
        DEBUG_PRINTK("index:%d, data:0x%02x\n", __COUNT__ + index, Data[index]);
    }  
    __COUNT__ += NBytes; 
} 
#endif
#endif /*--Debug Edid end--*/
    
    return ret;
}


//-----------------------------------------------------------
static HI_U8 DecodeHeader(HI_U8 * HeaderError, HI_U16 * pCRC16)
{
    HI_U8 Error = HI_FAILURE;
    IRAM HI_U8 Data[8];

    Error = SI_BlockReadEDID(8, 0, Data);
    if(HI_SUCCESS != Error)
        return Error;

    SI_UpdateCRC16WithBlock( pCRC16, 8, Data);   
   
    
    return Error;
}
//-----------------------------------------------------------
static HI_U8 CheckEDIDVersion(HI_U8 * VersionError, HI_U16 * pCRC16)
{
    IRAM HI_U8 Data[2] = {0};
    HI_U8  Error;

    * VersionError = 0;
    /* EDID Version: offset:0x12: Vesrion:1, Revision:3 */
    Error = SI_BlockReadEDID(2, VER_ADDR, Data);
    if(HI_SUCCESS != Error)
        return Error;
    SI_UpdateCRC16WithBlock( pCRC16, 2, Data);
   // if(! ((Data[0] == 1) &&  (Data[1] >= 3)) )
   //     * VersionError  = VER_DONT_SUPPORT_861B;
   HI_INFO_HDMI("version:Data[0]:%d,Data[1]:%d\n",Data[0],Data[1]);
    return Error;
}
//-----------------------------------------------------------
static HI_U8 CheckCRC(HI_U8 Addr, HI_U8 * CRC, HI_U16 * pCRC16)
{
    HI_U8  Error, i, Base;
    IRAM HI_U8  Data[8] = {0};


    Base = Addr;
    Addr = 0;

    for(*CRC = 0; Addr < 127; Addr = Addr + 8)
    {
        Error = SI_BlockReadEDID(8, Base + Addr, Data);
        if(Error)
            break;
        for( i = 0; i < 8; i++)
        {
            *CRC += Data[i];
            SI_UpdateCRC16WithBlock( pCRC16, 8, Data);
        }

    }

    return Error;
}
//-----------------------------------------------------------
static HI_U8 CheckFor861BExtension(HI_U8 * NExtensions, HI_U16 * pCRC16)
{
    HI_U8 Error;
    /* Extension: offset:0x7e, indicate Num of Externsion of EDID */
    Error = SI_BlockReadEDID(1, NUM_EXTENSIONS_ADDR, NExtensions);
    if(!Error)
        SI_UpdateCRC16WithBlock( pCRC16, 1, NExtensions);
    HI_INFO_HDMI("Read first block extension flag add offset:0x%x, data:0x%02x\n", NUM_EXTENSIONS_ADDR, NExtensions[0]);
    return Error;

}

//-----------------------------------------------------------
static HI_U8 NewSearchForVendorSpecificBlock(HI_U8 * SearchRes, HI_U16 * pCRC16)
{

    HI_U8 Error, VSpecificBoundary, BlockAddr;
    IRAM HI_U8  Data[2] = {0};

    *SearchRes = 0;

    if(CEAOnFisrtPage)
    {
        Error = SI_BlockReadEDID(2, EXTENSION_ADDR_1StP + 2, Data);
        if(HI_SUCCESS != Error)
            return Error;
    }
    else
    {
        Error = SI_BlockReadEDID(2, EXTENSION_ADDR + 2, Data);
        if(HI_SUCCESS != Error)
            return Error;
    }
    SI_UpdateCRC16WithBlock( pCRC16, 2, Data);
    if(Data[0] < 4)
        return Error;

    //    VSpecificBoundary = CEA_DATA_BLOCK_COLLECTION_ADDR + Data[0] - 1;
    VSpecificBoundary = EXTENSION_ADDR + Data[0];

    BlockAddr = CEA_DATA_BLOCK_COLLECTION_ADDR;
    if(CEAOnFisrtPage)
    {
        VSpecificBoundary = CEA_DATA_BLOCK_COLLECTION_ADDR_1StP + Data[0] - 1;
        BlockAddr = CEA_DATA_BLOCK_COLLECTION_ADDR_1StP;
    }

    while( BlockAddr < VSpecificBoundary)
    {

        Error = SI_BlockReadEDID(1, BlockAddr, Data);
        if(HI_SUCCESS != Error)
            return Error;
        SI_UpdateCRC16WithBlock( pCRC16, 1, Data);
        if((Data[0] & 0xE0)== TAG_VENDOR_DATA_BLOCK)
        {
            * SearchRes = BlockAddr;
            break;
        }
        BlockAddr = BlockAddr + (Data[0] & 0x1F) + 1;
    }

    return Error;
}
//-----------------------------------------------------------
static HI_U8 NewCheckHDMISignature(HI_U8 * pSysError, HI_U16 * pCRC16)
{
    HI_U8 Error = HI_FAILURE, Addr;
    IRAM HI_U8  Data[3] = {0};

    NewSearchForVendorSpecificBlock(&Addr, pCRC16);

    if (!Addr)             /// HDMI Signature block not found
        *pSysError = NO_HDMI_SIGNATURE;
    else
    {

        Error = SI_BlockReadEDID(3, ++Addr, Data);
        if(! Error)
        {
            SI_UpdateCRC16WithBlock( pCRC16, 3, Data);

            ///         if((Data[0] != 0x03)&&(Data[1] != 0x0C)&&(Data[2] != 0x0))
            if((Data[0] != 0x03)||(Data[1] != 0x0C)||(Data[2] != 0x0))

                *pSysError = NO_HDMI_SIGNATURE;
        }
    }
    return Error;
}
//-----------------------------------------------------------
static HI_U8 GetCEA861BBlockTag( HI_U16 * pCRC16, HI_U8 * Addr)
{
    HI_U8 Error;
    HI_U8  Data = 0;

    Error = SI_BlockReadEDID(1, *Addr, &Data);
    if(!Error)
    {
        SI_UpdateCRC16WithBlock( pCRC16, 1, &Data);
        *Addr = Data;
    }
    return Error;
}
#if defined (DVI_SUPPORT)
static void DecodeEstablishTiming(HI_U8 * Data)
{

    if(Data[0]& 0x80)
        HI_INFO_HDMI("720 x 400 @ 70Hz\n");
    if(Data[0]& 0x40)
        HI_INFO_HDMI("720 x 400 @ 88Hz\n");
    if(Data[0]& 0x20)
        HI_INFO_HDMI("640 x 480 @ 60Hz\n");
    if(Data[0]& 0x10)
        HI_INFO_HDMI("640 x 480 @ 67Hz\n");
    if(Data[0]& 0x08)
        HI_INFO_HDMI("640 x 480 @ 72Hz\n");
    if(Data[0]& 0x04)
        HI_INFO_HDMI("640 x 480 @ 75Hz\n");
    if(Data[0]& 0x02)
        HI_INFO_HDMI("800 x 600 @ 56Hz\n");

    if(Data[0]& 0x01){
        g_stSinkCap.bVideoFmtSupported[HI_UNF_ENC_FMT_VESA_800X600_60] = HI_TRUE;
        HI_INFO_HDMI("800 x 400 @ 60Hz\n");
    }


    if(Data[1]& 0x80)
        HI_INFO_HDMI("800 x 600 @ 72Hz\n");
    if(Data[1]& 0x40)
        HI_INFO_HDMI("800 x 600 @ 75Hz\n");
    if(Data[1]& 0x20)
        HI_INFO_HDMI("832 x 624 @ 75Hz\n");
    if(Data[1]& 0x10)
        HI_INFO_HDMI("1024 x 768 @ 87Hz\n");

    if(Data[1]& 0x08){
        g_stSinkCap.bVideoFmtSupported[HI_UNF_ENC_FMT_VESA_1024X768_60] = HI_TRUE;
        HI_INFO_HDMI("1024 x 768 @ 60Hz\n");
    }

    if(Data[1]& 0x04)
        HI_INFO_HDMI("1024 x 768 @ 70Hz\n");
    if(Data[1]& 0x02)
        HI_INFO_HDMI("1024 x 768 @ 75Hz\n");
    if(Data[1]& 0x01)
        HI_INFO_HDMI("1280 x 1024 @ 75Hz\n");

    if(Data[2]& 0x80)
        HI_INFO_HDMI("1152 x 870 @ 75Hz\n");

    if((!Data[0])&&(!Data[1])&&(!Data[2]))
        HI_INFO_HDMI("No established video modes\n");
}

//-----------------------------------------------------------
static void DecodeStandardTiming(HI_U8 * Data, HI_U8 Mode)
{
    HI_U8 TmpVal, i;

    Mode*=4;
    TmpVal = Mode + 4;
    for(i = 0; Mode < TmpVal; Mode++)
    {
        i++;
        HI_INFO_HDMI(" Mode %i \n",(int)Mode);
        if((Data[i] == 0x01)&&(Data[i + 1]==0x01))
            HI_INFO_HDMI("Mode %d wasn't defined! \n", (int)Mode);
        else
        {
            HI_INFO_HDMI(" Hor Act pixels %i \n", (int)((Data[Mode]+31)*8));
            HI_INFO_HDMI(" Aspect ratio: \n");
            TmpVal = Data[Mode + 1] & 0xC0;
            if(TmpVal==0x00)
                HI_INFO_HDMI("16:10\n");
            else  if(TmpVal==0x40)
                HI_INFO_HDMI("4:3\n");
            else  if(TmpVal==0x80)
                HI_INFO_HDMI("5:4\n");
            else
                HI_INFO_HDMI("16:9\n");
            HI_INFO_HDMI(" Refresh rate %i Hz \n", (int)((Data[Mode + 1])& 0x3F) + 60);
        }
    }
}

//----------------------------------------------------------
static HI_U8 TestHeaderDetailedTimingDescr(HI_U8 * Data)
{
    HI_U8 Res = 0;

    if(Data[0]||Data[1])
        Res = 1; // timing descriptor
    return Res;
}

//-----------------------------------------------------------
static void ParsingDetailedTDPart1(HI_U8 * Data)
{
    HI_U16 TmpVal;

    TmpVal = (Data[2] << 4)& 0x0F00;
    TmpVal |= Data[0];
    HI_INFO_HDMI(" H Active %i \n", TmpVal );
    TmpVal = (Data[2]<<8)& 0x0F00;
    TmpVal |= Data[1];
    HI_INFO_HDMI(" H Blank %i \n", TmpVal );
    TmpVal = (Data[5] << 4)& 0x0F00;
    TmpVal |= Data[3];
    HI_INFO_HDMI(" V Active %i \n", TmpVal );
    TmpVal = (Data[5]<<8)& 0x0F00;
    TmpVal |= Data[4];
    HI_INFO_HDMI(" V Blank %i \n", TmpVal );
}
//-----------------------------------------------------------
static void ParsingDetailedTDPart2(HI_U8 * Data)
{
    HI_U16 TmpVal;

    TmpVal = (Data[3] << 2)& 0x0300;
    TmpVal |= Data[0];
    HI_INFO_HDMI(" H Sync Offset %i\n", TmpVal );
    TmpVal = (Data[3] << 4)& 0x0300;
    TmpVal |= Data[1];
    HI_INFO_HDMI(" H Sync Pulse W %i\n", TmpVal );
    TmpVal = (Data[3] << 2 ) & 0x30;
    TmpVal |= (Data[2] >> 4);
    HI_INFO_HDMI(" V Sync Offset %i\n",   TmpVal  );
    TmpVal = (Data[3] << 4 ) & 0x30;
    TmpVal |= (Data[2]& 0x0F);
    HI_INFO_HDMI(" V Sync Pulse W %i\n", TmpVal );
}
//-----------------------------------------------------------
static void ParsingDetailedTDPart3(HI_U8 * Data)
{
    HI_U16 TmpVal;

    TmpVal = Data[0] |( Data[2] << 4);
    HI_INFO_HDMI(" H Image Size %i\n", TmpVal );
    TmpVal = Data[1] |(( Data[2] << 8)&0x0F00);
    HI_INFO_HDMI(" V Image Size %i\n", TmpVal );
    HI_INFO_HDMI(" H Border %i\n", (int)Data[3]);
    HI_INFO_HDMI(" V Border %i\n", (int)Data[4]);
    if(Data[5] & 0x80)
        HI_INFO_HDMI("Interlaced\n");
    else
        HI_INFO_HDMI("Non-interlaced\n");
    if(Data[5] & 0x60)
        HI_INFO_HDMI("Table 3.17 for defenition\n");
    else
        HI_INFO_HDMI("Normal Display:\n");
    if(Data[5] & 0x10)
        HI_INFO_HDMI(" Digital\n");
    else
        HI_INFO_HDMI(" Analog\n");
}
#endif
//-----------------------------------------------------------
static HI_U8 MonitorCapable861(HI_U16 * pCRC16)
{
    HI_U8 CapN = 0, Error;
    
    Error = SI_BlockReadEDID(1, 0x83, &CapN);
    if(!Error)
    {
        SI_UpdateCRC16WithBlock( pCRC16, 1, &CapN);
        HI_INFO_HDMI(" DTV monitor supports: \n");
        if(CapN & 0x80)
            HI_INFO_HDMI(" Underscan\n");
        if(CapN & 0x40)
            HI_INFO_HDMI(" Basic audio\n");
        if(CapN & 0x20)
        {
            g_stSinkCap.bSupportYCbCr = HI_TRUE;
            HI_INFO_HDMI(" YCbCr 4:4:4\n");
        }
        if(CapN & 0x10)
        {
            g_stSinkCap.bSupportYCbCr = HI_TRUE;
            HI_INFO_HDMI(" YCbCr 4:2:2\n");
        }
        HI_INFO_HDMI(" Native formats in detail descriptors %i\n",(int)(CapN & 0x0F));
    }
    
    return Error;
}

static void ParsingShortTimeDescriptorBlock(HI_U8 * Data, HI_U8 Size)
{
    HI_U8 cea816Mode;
    HI_S32 hdmiModeIdx, i;
    HI_UNF_ENC_FMT_E unfFmt;
    HI_UNF_ENC_FMT_E FirstVideoFmtSupported = HI_UNF_ENC_FMT_BUTT;

    for(i = 0; i < Size; i++)
    {           
        cea816Mode = (Data[i]&0x7F);
        HI_INFO_HDMI("ID %i \n",cea816Mode);
  
        for(hdmiModeIdx = 0; hdmiModeIdx < NMODES; hdmiModeIdx++)
        {
          
            if (VModeTables[hdmiModeIdx].ModeId.Mode_C1 == cea816Mode) 
            {
                if(VModeTables[hdmiModeIdx].ModeId.enUNFFmt_C1 >= HI_UNF_ENC_FMT_BUTT)
                {
                    continue;
                }
                unfFmt = VModeTables[hdmiModeIdx].ModeId.enUNFFmt_C1;
                g_stSinkCap.bVideoFmtSupported[unfFmt] = HI_TRUE;  /* support timing foramt */
                if(FirstVideoFmtSupported == HI_UNF_ENC_FMT_BUTT)
                {
                    FirstVideoFmtSupported = unfFmt;
                }

                if(Data[i]&0x80)
                {
                    HI_INFO_HDMI(" <--[Native mode]\n");
                    g_stSinkCap.enNativeVideoFormat = unfFmt;
                }
                g_s32VmodeOfUNFFormat[unfFmt] = hdmiModeIdx;
            }
            else if (VModeTables[hdmiModeIdx].ModeId.Mode_C2 == cea816Mode) 
            {
                if(VModeTables[hdmiModeIdx].ModeId.enUNFFmt_C2 >= HI_UNF_ENC_FMT_BUTT)
                {
                    continue;
                }
                unfFmt = VModeTables[hdmiModeIdx].ModeId.enUNFFmt_C2;
                g_stSinkCap.bVideoFmtSupported[unfFmt] = HI_TRUE;

                if(Data[i]&0x80)
                {
                    HI_INFO_HDMI(" <--[Native mode]\n");
                    g_stSinkCap.enNativeVideoFormat = unfFmt;
                }
                g_s32VmodeOfUNFFormat[unfFmt] = hdmiModeIdx;
            }
            else
            {
                continue;
            }
        }
        //We need to add default value:640x480p@60Hz.
        g_stSinkCap.bVideoFmtSupported[HI_UNF_ENC_FMT_861D_640X480_60] = HI_TRUE;
        g_s32VmodeOfUNFFormat[HI_UNF_ENC_FMT_861D_640X480_60] = 0x01;
    }
    
    if(g_stSinkCap.enNativeVideoFormat == HI_UNF_ENC_FMT_BUTT)
    {
        g_stSinkCap.enNativeVideoFormat = FirstVideoFmtSupported;
    }
}

HI_U8 Transfer_VideoTimingFromat_to_VModeTablesIndex(HI_UNF_ENC_FMT_E unfFmt)
{

    if (HI_UNF_ENC_FMT_BUTT <= unfFmt)   
        return 0; /* 640x480 format index in VModeTables*/
    else
        return(g_s32VmodeOfUNFFormat[unfFmt]);	
}

//-----------------------------------------------------------
static HI_U8 ParsingVideoDATABlock(HI_U16 * pCRC16, HI_U8 BaseAddr)
{
    IRAM HI_U8  Data[32];
    HI_U8 Error;
    HI_U8 NBytes = 0;

    HI_INFO_HDMI("%s BaseAddr:%x\n", __FUNCTION__, BaseAddr);
    
    Error = SI_BlockReadEDID(1, BaseAddr, &NBytes);
    if(!Error)
    {
        SaveEdidToEeprom(BaseAddr++, 1, &NBytes);
        SI_UpdateCRC16WithBlock( pCRC16, 1, &NBytes);
        NBytes&=0x1F;
        HI_INFO_HDMI("ShortTimeDescriptor CEA861 Modes:\n");
#if 0 /*--NO MODIFY : COMMENT BY CODINGPARTNER--*/
        for(i = 0 ; i < 4; i++)
        {
#endif /*--NO MODIFY : COMMENT BY CODINGPARTNER--*/
#if 0 /*--NO MODIFY : COMMENT BY CODINGPARTNER--*/
            if(NBytes <= 8)
            {
#endif /*--NO MODIFY : COMMENT BY CODINGPARTNER--*/
                Error = SI_BlockReadEDID(NBytes, BaseAddr, Data);
                if(!Error)
                {
                    //SaveEdidToEeprom(BaseAddr, NBytes, Data);                    
                    //SI_UpdateCRC16WithBlock( pCRC16, NBytes, Data);
                    ParsingShortTimeDescriptorBlock(Data, NBytes);
                }
#if 0 /*--NO MODIFY : COMMENT BY CODINGPARTNER--*/
                break;
            }
#endif /*--NO MODIFY : COMMENT BY CODINGPARTNER--*/
#if 0 /*--NO MODIFY : COMMENT BY CODINGPARTNER--*/
            else
            {
                Error = SI_BlockReadEDID(8, BaseAddr + i * 8, Data);
                if(!Error)
                {
                    SaveEdidToEeprom(BaseAddr  + i*8, 8, Data);
                    
                    SI_UpdateCRC16WithBlock( pCRC16, 8, Data);
                    ParsingShortTimeDescriptorBlock(Data, 8);
                }
                else
                    break;
                NBytes-=8;
            }
        }
#endif /*--NO MODIFY : COMMENT BY CODINGPARTNER--*/       


    }
    return Error;
}
//-----------------------------------------------------------
static void ParsingShortAudioDescriptor(HI_U8 * Data)
{
    HI_U8 AudioFormatCode;

    AudioFormatCode = (Data[0]&0xF8)>>3;

    if (AudioFormatCode < HI_UNF_HDMI_MAX_AUDIO_CAP_COUNT)
    {
        g_stSinkCap.bAudioFmtSupported[AudioFormatCode] = HI_TRUE;
    }
    
    HI_INFO_HDMI("Audio Format Code %i: \n",(int)AudioFormatCode);
    switch (AudioFormatCode)
    {
    case 1:
        HI_INFO_HDMI("Liniar PCM\n");
        break;
    case 2:
        HI_INFO_HDMI("AC-3\n");
        break;
    case 3:
        HI_INFO_HDMI("MPEG-1\n");
        break;
    case 4:
        HI_INFO_HDMI("MP3\n");
        break;
    case 5:
        HI_INFO_HDMI("MPEG2\n");
        break;
    case 6:
        HI_INFO_HDMI("AAC\n");
        break;
    case 7:
        HI_INFO_HDMI("DTS\n");
        break;
    case 8:
        HI_INFO_HDMI("ATRAC\n");
        break;
    default:
        HI_INFO_HDMI("reserved\n");
    }
    HI_INFO_HDMI("Max N of channels %i\n",(int)((Data[0]&0x07)+1));
    HI_INFO_HDMI("Fs: \n");
    if(Data[1] & 0x01)
    {
        HI_INFO_HDMI(" 32 KHz\n");
        g_stSinkCap.u32AudioSampleRateSupported[0] = HI_UNF_SAMPLE_RATE_32K;
    }
    if(Data[1] & 0x02)
    {
        HI_INFO_HDMI(" 44 KHz\n");
        g_stSinkCap.u32AudioSampleRateSupported[1] = HI_UNF_SAMPLE_RATE_44K;
    }
    if(Data[1] & 0x04)
    {
        HI_INFO_HDMI(" 48 KHz\n");
        g_stSinkCap.u32AudioSampleRateSupported[2] = HI_UNF_SAMPLE_RATE_48K;
    }
    if(Data[1] & 0x08)
    {
        HI_INFO_HDMI(" 88 KHz\n");
        g_stSinkCap.u32AudioSampleRateSupported[3] = HI_UNF_SAMPLE_RATE_88K;
    }
    if(Data[1] & 0x10)
    {
        HI_INFO_HDMI(" 96 KHz\n");
        g_stSinkCap.u32AudioSampleRateSupported[4] = HI_UNF_SAMPLE_RATE_96K;
    }
    if(Data[1] & 0x20)
    {
        HI_INFO_HDMI(" 176 KHz\n");
    }
    if(Data[1] & 0x40)
    {
        HI_INFO_HDMI(" 192 KHz\n");
        g_stSinkCap.u32AudioSampleRateSupported[5] = HI_UNF_SAMPLE_RATE_192K;
    }
    if(AudioFormatCode == 1)
    {
    	if(g_stSinkCap.u32MaxPcmChannels < ((Data[0]&0x07)+1))
    	{
            g_stSinkCap.u32MaxPcmChannels = ((Data[0]&0x07)+1);
        }
        
        HI_INFO_HDMI("Supported length: \n");
        if(Data[2]&0x01)
            HI_INFO_HDMI("16 bits\n");
        if(Data[2]&0x02)
            HI_INFO_HDMI("20 bits\n");
        if(Data[2]&0x04)
            HI_INFO_HDMI("24 bits\n");
    }
    else
        HI_INFO_HDMI(" Maximum bit rate %i KHz\n", (int)(Data[2]<<3));


}
//-----------------------------------------------------------
static HI_U8 ParsingAudioDATABlock(HI_U16 * pCRC16, HI_U8 BaseAddr)
{
    IRAM HI_U8  Data[3] = {0};
    HI_U8 Error, i = 0;
    HI_U8 NBytes = 0;

    Error = SI_BlockReadEDID(1, BaseAddr, &NBytes);
    if(!Error)
    {
        SaveEdidToEeprom(BaseAddr++, 1, &NBytes);
        
        SI_UpdateCRC16WithBlock( pCRC16, 1, &NBytes);
        NBytes&=0x1F;

        do
        {
            Error = SI_BlockReadEDID(3, BaseAddr, Data);
            if(!Error)
            {
                SaveEdidToEeprom(BaseAddr, 3, Data);
                
                SI_UpdateCRC16WithBlock( pCRC16, 3, Data);
                ParsingShortAudioDescriptor(Data);
            }
            else
                break;
                
            i+=3;
            BaseAddr+=3;
        }
        while (i < NBytes);
    }
    return Error;
}
//-----------------------------------------------------------
static HI_U8 ParsingSpeakerDATABlock(HI_U16 * pCRC16, HI_U8 BaseAddr)
{
    HI_U8  Data = 0;
    HI_U8 Error;


    Error = SI_BlockReadEDID(1, ++BaseAddr, &Data);
    if(!Error)
    {
        SI_UpdateCRC16WithBlock( pCRC16, 1, &Data);
        g_stSinkCap.u8Speaker = Data & 0x7f;
        HI_INFO_HDMI("Speakers' allocation: \n");
        if(Data & 0x01)
            HI_INFO_HDMI("FL/FR\n");
        if(Data & 0x02)
            HI_INFO_HDMI("LFE\n");
        if(Data & 0x04)
            HI_INFO_HDMI("FC\n");
        if(Data & 0x08)
            HI_INFO_HDMI("RL/RR\n");
        if(Data & 0x10)
            HI_INFO_HDMI("RC\n");
        if(Data & 0x20)
            HI_INFO_HDMI("FLC/FRC\n");
        if(Data & 0x40)
            HI_INFO_HDMI("RLC/RRC\n");
    }
    return Error;
}

static HI_U8 ParsingVendorspecificDATABlock(HI_U16 * pCRC16, HI_U8 BaseAddr, HI_U8 length)
{
    HI_U8  Data[32] = {0};
    HI_U8 Error = 0;
    HI_U8 u8HDMI_3D_LEN_Offset;
    HI_U8 offset, index;

    Error = SI_BlockReadEDID(length, ++BaseAddr, Data);
    if(!Error)
    {
        SI_UpdateCRC16WithBlock( pCRC16, length, Data);

        HI_INFO_HDMI("IEERegId:0x%02x, 0x%02x, 0x%02x\n", Data[0], Data[1], Data[2]);
         if ((Data[0] == 0x03) && (Data[1] == 0x0c)
          && (Data[2] == 0x00))
        {
            HI_INFO_HDMI("This is HDMI Device\n");
        }
        else
        {
            HI_INFO_HDMI("This is DVI Device, we don't parse it\n");
            return 0;
        }
        g_stSinkCap.u8IEERegId[0] =Data[0];
        g_stSinkCap.u8IEERegId[1] =Data[1];
        g_stSinkCap.u8IEERegId[2] =Data[2];
                
        /* Vendor Specific Data Block */
        g_stSinkCap.u8PhyAddr_A = (Data[3] & 0xF0)>>4;
        g_stSinkCap.u8PhyAddr_B = (Data[3] & 0x0F);
        g_stSinkCap.u8PhyAddr_C = (Data[4] & 0xF0)>>4;
        g_stSinkCap.u8PhyAddr_D = (Data[4] & 0x0F);
    
        if((g_stSinkCap.u8PhyAddr_A != 0xF )&&(g_stSinkCap.u8PhyAddr_B != 0xF )&&
           (g_stSinkCap.u8PhyAddr_C != 0xF )&&(g_stSinkCap.u8PhyAddr_D != 0xF ))
        {
            g_stSinkCap.bIsPhyAddrValid = TRUE;
        }
        else
        {
            g_stSinkCap.bIsPhyAddrValid = FALSE;
        }
        HI_INFO_HDMI("PhyAddr %02x.%02x.%02x.%02x\n", g_stSinkCap.u8PhyAddr_A, g_stSinkCap.u8PhyAddr_B, g_stSinkCap.u8PhyAddr_C, g_stSinkCap.u8PhyAddr_D);
            
        g_stSinkCap.bSupportDVIDual    = (Data[5] & 0x01);
        g_stSinkCap.bSupportDeepColorYCBCR444     = (Data[5] & 0x08) >> 3;
        g_stSinkCap.bSupportDeepColor30Bit    = (Data[5] & 0x10) >> 4;
        g_stSinkCap.bSupportDeepColor36Bit    = (Data[5] & 0x20) >> 5;
        g_stSinkCap.bSupportDeepColor48Bit    = (Data[5] & 0x40) >> 6;
        g_stSinkCap.bSupportAI                = (Data[5] & 0x80) >> 7;
        g_stSinkCap.u8MaxTMDSClock            = (Data[6] & 0xff) * 5;
        g_stSinkCap.bLatency_Fields_Present   = (Data[7] & 0x80) >> 7;
        g_stSinkCap.bI_Latency_Fields_Present = (Data[7] & 0x40) >> 6;
        g_stSinkCap.bHDMI_Video_Present       = (Data[7] & 0x20) >> 5;
        g_stSinkCap.u8Video_Latency           = (Data[8] & 0xff);
        g_stSinkCap.u8Audio_Latency           = (Data[9] & 0xff);
        g_stSinkCap.u8Interlaced_Video_Latency= (Data[10]& 0xff);
        g_stSinkCap.u8Interlaced_Audio_Latency= (Data[11]& 0xff);

        memset(&stVendor3D, 0, sizeof(SI_Vendor_3D_S));
        stVendor3D.u8HDMI_Video_present = (Data[7] & 0x20) >> 5;
        if ((stVendor3D.u8HDMI_Video_present != 0) && (length > 15))
        {
            stVendor3D.u83D_Present = (Data[12]& 0x80) >> 7;
            stVendor3D.u83D_Multi_present = (Data[12]& 0x60) >> 5;
            stVendor3D.u8HDMI_3D_LEN = (Data[13]& 0x1F);
            stVendor3D.u8HDMI_VIC_LEN = (Data[13]& 0xE0) >> 5;
        
            /*if 3D_present is 1, HMDI Sink shall support 3D Video format.
              if Sink support at lease one 59.94/60Hz 2D video format, it should support
              1920x1080p@23.98/24Hz; 1280x720p@59.94/60Hz: Frame packing
              if Sink support at lease one 50Hz 2D video format, it should support
              1920x1080p@23.98/24Hz; 1280x720p@50Hz: Frame packing              
            */
            HI_INFO_HDMI("u8HDMI_Video_present:0x%x, u83D_Present:0x%x, u83D_Multi_present:0x%x\n", 
                stVendor3D.u8HDMI_Video_present, stVendor3D.u83D_Present, stVendor3D.u83D_Multi_present);
            HI_INFO_HDMI("u8HDMI_3D_LEN:0x%x, u8HDMI_VIC_LEN:0x%x\n", stVendor3D.u8HDMI_3D_LEN, stVendor3D.u8HDMI_VIC_LEN);
            
            offset = 14;
            stVendor3D.u8HDMI_VICNum = stVendor3D.u8HDMI_VIC_LEN;
            if(stVendor3D.u8HDMI_VIC_LEN > 0)
            {
                memset(stVendor3D.u8HDMI_VIC, 0, 8);
                for(index = 0; index < stVendor3D.u8HDMI_VIC_LEN; index ++)
                {
                    stVendor3D.u8HDMI_VIC[index] = Data[offset + index];
                }
            }
            
            offset ++;
            u8HDMI_3D_LEN_Offset = 0;
            if (stVendor3D.u83D_Multi_present == 0)
            {
                //3D_Structure_ALL_15...0 and 3D_MASK_15...0 are not present;
            }
            if ((stVendor3D.u83D_Multi_present == 1) || (stVendor3D.u83D_Multi_present == 2))
            {
                stVendor3D.u83D_Sturcture_ALL_15_8 = (Data[offset]& 0xFF);
                offset ++;
                stVendor3D.u83D_Sturcture_ALL_7_0  = (Data[offset]& 0xFF);
                u8HDMI_3D_LEN_Offset = 2;
            }
            if (stVendor3D.u83D_Multi_present == 2)
            {
                offset ++;
                stVendor3D.u83D_MASK_15_8 = (Data[offset]& 0xFF);
                offset ++;
                stVendor3D.u83D_MASK_7_0  = (Data[offset]& 0xFF);
                u8HDMI_3D_LEN_Offset = 4;
            }
            
            stVendor3D.SI_3D_DetailNum = 0;
            while (stVendor3D.u8HDMI_3D_LEN > u8HDMI_3D_LEN_Offset)
            {
                offset ++;
                u8HDMI_3D_LEN_Offset ++;
                stVendor3D.s3DDetail[stVendor3D.SI_3D_DetailNum].u82D_VIC_order = (Data[offset] & 0xf0) >> 4;
                stVendor3D.s3DDetail[stVendor3D.SI_3D_DetailNum].u83D_Structure = (Data[offset] & 0x0f);
                
                if(stVendor3D.s3DDetail[stVendor3D.SI_3D_DetailNum].u83D_Structure >= 0x08)
                {
                    offset ++;
                    u8HDMI_3D_LEN_Offset ++;
                    stVendor3D.s3DDetail[stVendor3D.SI_3D_DetailNum].u83D_Structure = (Data[offset] & 0xf0) >> 4;
                }
                
                stVendor3D.SI_3D_DetailNum ++;                
            }
        }   

    }
    return Error;
}

static HI_U8 ParsingUseExtenedDATABlock(HI_U16 * pCRC16, HI_U8 BaseAddr)
{
    HI_U8  Data[8] = {0};
    HI_U8 Error = 0;

    Error = SI_BlockReadEDID(1, ++BaseAddr, Data);
    if  (! Error)
    {
        SI_UpdateCRC16WithBlock( pCRC16, 1, Data);

        switch(Data[0])
        {
#if defined (DVI_SUPPORT)
        case USE_EXTENDED_VIDEO_CAPABILITY_DATA_BLOCK:
            HI_INFO_HDMI("UseExtened Include Video Capability Data Block\n");
            break;
        case USE_EXTENDED_VENDOR_SPECIFIC_VIDEO_DATA_BLOCK:
            HI_INFO_HDMI("UseExtened Include Vendor-Specific Video Data Block\n");
            break;
        case 2:
            HI_INFO_HDMI("UseExtened Include Reserved for VESA Video Display Device Information Data Block\n");
            break;
        case 3:
            HI_INFO_HDMI("UseExtened Include Reserved for VESA Video Data Block\n");
            break;
        case 4:
            HI_INFO_HDMI("UseExtened Include Reserved for HDMI Video Data Block\n");
            break;
#endif
        case USE_EXTENDED_COLORIMETRY_DATA_BLOCK:
            HI_INFO_HDMI("UseExtened Include Colorimetry Data Block\n");
            Error = SI_BlockReadEDID(2, ++BaseAddr, Data);
            if  (! Error)
            {
                if (Data[0] & 0x01)
                {
                    g_stSinkCap.bSupportxvYCC601 = HI_TRUE;
                }
                if (Data[0] & 0x02)
                {
                    g_stSinkCap.bSupportxvYCC709 = HI_TRUE;
                }
                g_stSinkCap.u8MDBit = Data[1] & 0x07;
            }
            break;
#if defined (DVI_SUPPORT)
        case USE_EXTENDED_CEA_MIS_AUDIO_FIELD_DATA_BLOCK:
            HI_INFO_HDMI("UseExtened Include CEA Miscellaneous Audio Fields\n");
            break;
        case USE_EXTENDED_VENDOR_SPECIFIC_AUIDO_DATA_BLOCK:
            HI_INFO_HDMI("UseExtened Include Vendor-Specific Audio Data Block\n");
            break;
        case 18:
            HI_INFO_HDMI("UseExtened Include Reserved for HDMI Audio Data Block\n");
            break;
#endif
        default:
            HI_INFO_HDMI("UseExtened reserve\n");
            break;
        }
    }
    return Error;
}


static HI_U8 ParsingCEADataBlockCollection(HI_U16 * pCRC16)
{
    HI_U8 Error;
    HI_U8 AddrTag, Addr, length, D = 0;
    HI_INFO_HDMI("ParsingCEADataBlockCollection\n");
    // 0. get block boundaries boundary
    // 1. Get TAG is OK if calc next Tag Address
    //    decode tag, call tag parser, if not TAG exit, if address exceed boundary exit too, otherwise go to 1
    if(CEAOnFisrtPage)
    {
        Error = SI_BlockReadEDID(1, 0x02, &D);
        Addr = 0x04;
    }
    else
    {
        Error = SI_BlockReadEDID(1, 0x82, &D);
        Addr = 0x84;
    }
    
    if(!Error)
    {
        D+=Addr;
        AddrTag = Addr;
        do
        {
            Error = GetCEA861BBlockTag(pCRC16, &AddrTag);
            if(!Error)
            {
                HI_INFO_HDMI("EDID AddrTag:0x%02x-->tag:0x%x\n", AddrTag, ((AddrTag&0xE0)>> 5));
                length = AddrTag & 0x1f;
                switch(AddrTag&0xE0)
                {
                case VIDEO_TAG:
                    HI_INFO_HDMI("ParsingVideoDATABlock\n");
                    Error = ParsingVideoDATABlock(pCRC16, Addr);
                    break;
                case AUDIO_TAG:
                    HI_INFO_HDMI("ParsingAudioDATABlock\n");
                    Error = ParsingAudioDATABlock(pCRC16, Addr);
                    break;
                case SPEAKER_TAG:
                    HI_INFO_HDMI("ParsingSpeakerDATABlock\n");
                    Error = ParsingSpeakerDATABlock(pCRC16, Addr);
                    break;
                case VENDOR_TAG:
                    HI_INFO_HDMI("VENDOR_TAG in EDID\n");
                    Error = ParsingVendorspecificDATABlock(pCRC16, Addr, length);
                    break;
                case USE_EXTENDED_TAG:
                    HI_INFO_HDMI("Use Extended Tag\n");
                    Error = ParsingUseExtenedDATABlock(pCRC16, Addr);
                    break;
                }
                Addr += ( AddrTag & 0x1F ) ;   // next Tag Address
                AddrTag = ++Addr;
            }
            if(Error)
                break;
        }
        while (Addr < D);
    }
    return Error;
}
#if 0 /* no used now ?? need check */
//-----------------------------------------------------------
static HI_U8 ParseCEADetailedTimingDescriptors(HI_U16 * pCRC16)
{
    IRAM HI_U8  Data[6];
    HI_U8 BaseAddr, Error;
    memset((void*)Data, 0, 6);
    HI_INFO_HDMI("ParseCEADetailedTimingDescriptors\n");
    // Get Start Address
    if(CEAOnFisrtPage)
        BaseAddr = 0x02;
    else
        BaseAddr = 0x82;
    Error = SI_BlockReadEDID(1, BaseAddr, Data);
    if(!Error)
    {
        SI_UpdateCRC16WithBlock( pCRC16, 1, Data);
        if(CEAOnFisrtPage)
            BaseAddr = Data[0];
        else
            BaseAddr = Data[0] + 0x80;

        Error = SI_BlockReadEDID(2, BaseAddr, Data);
        if(!Error)
        {
            SI_UpdateCRC16WithBlock( pCRC16, 2, Data);
            while(TestHeaderDetailedTimingDescr(Data))
            {

                Error = SI_BlockReadEDID(6, BaseAddr + 2, Data);
                if(!Error)
                {
                    SI_UpdateCRC16WithBlock( pCRC16, 6, Data);
                    ParsingDetailedTDPart1(Data);
                    Error = SI_BlockReadEDID(4, BaseAddr + 8, Data);
                    if(!Error)
                    {
                        SI_UpdateCRC16WithBlock( pCRC16, 4, Data);
                        ParsingDetailedTDPart2(Data);
                        Error = SI_BlockReadEDID(6, BaseAddr + 12, Data);
                        if(!Error)
                        {
                            SI_UpdateCRC16WithBlock( pCRC16, 6, Data);
                            ParsingDetailedTDPart3(Data);
                        }
                    }
                }
                if(Error)
                    break;
                else if(CEAOnFisrtPage && (BaseAddr > 0x6C))
                {
                    break;
                }
                else if (BaseAddr > 0xEC)
                    break;
                BaseAddr+=18;
                Error = SI_BlockReadEDID(2, BaseAddr, Data);
                if(!Error)
                    SI_UpdateCRC16WithBlock( pCRC16, 2, Data);
                else
                    break;

            }
        }
    }
    return Error;
}
#endif

//-----------------------------------------------------------
static HI_U8 ParseTimingDescriptors(HI_U16 * pCRC16)
{
    IRAM HI_U8  Data[8];
    HI_U8 Error, i, BaseAddr;


    memset((void*)Data, 0, 8);
    Error = SI_BlockReadEDID(3, 0x23, Data);

    if(!Error)
    {
        SI_UpdateCRC16WithBlock( pCRC16, 3, Data);
        DecodeEstablishTiming(Data);
        
        Error = SI_BlockReadEDID(8, 0x26, Data);
        if(!Error)
        {
            HI_INFO_HDMI("Standard Timing:");
            DecodeStandardTiming(Data, 0);
            SI_UpdateCRC16WithBlock( pCRC16, 8, Data);
            Error = SI_BlockReadEDID(8, 0x2E, Data);
            if(!Error)
            {
                DecodeStandardTiming(Data, 1);
                SI_UpdateCRC16WithBlock( pCRC16, 8, Data);
                BaseAddr = 0x36;
                HI_INFO_HDMI(" Detailed Timing: ");
                for(i = 0; i < 4;i ++)
                {
                    HI_INFO_HDMI("Descriptor %i:\n", (int)i);
                    Error = SI_BlockReadEDID(4, BaseAddr, Data);
                    if(!Error)
                    {
                        SI_UpdateCRC16WithBlock( pCRC16, 4, Data);
                        if(TestHeaderDetailedTimingDescr(Data))
                        {
                            Error = SI_BlockReadEDID(6, BaseAddr + 2, Data);
                            if(!Error)
                            {
                                SI_UpdateCRC16WithBlock( pCRC16, 6, Data);
                                ParsingDetailedTDPart1(Data);
                                Error = SI_BlockReadEDID(4, BaseAddr + 8, Data);
                                if(!Error)
                                {
                                    SI_UpdateCRC16WithBlock( pCRC16, 4, Data);
                                    ParsingDetailedTDPart2(Data);
                                    Error = SI_BlockReadEDID(6, BaseAddr + 12, Data);
                                    if(!Error)
                                    {
                                        SI_UpdateCRC16WithBlock( pCRC16, 6, Data);
                                        ParsingDetailedTDPart3(Data);
                                    }
                                }
                            }
                        }
                    }
                    if(Error)
                        break;
                    BaseAddr+=18;
                }

            }
        }
    }

    return Error;
}


typedef struct {
     HI_U8  IdManufacterChar;
     HI_U8  Code;
 }hdmi_IdManufacterCode_t;

typedef struct
{
     HI_U8 HexNumber;
     HI_U8 DecNumber;
}hdmi_HextoDecConverter_t;

static hdmi_IdManufacterCode_t IdManufacterCharCode[26]=
{
    {'A',0x01},
    {'B',0x02},
    {'C',0x03},
    {'D',0x04},
    {'E',0x05},
    {'F',0x06},
    {'G',0x07},
    {'H',0x08},
    {'I',0x09},
    {'J',0x0a},
    {'K',0x0b},
    {'L',0x0c},
    {'M',0x0d},
    {'N',0x0e},
    {'O',0x0f},
    {'P',0x10},
    {'Q',0x11},
    {'R',0x12},
    {'S',0x13},
    {'T',0x14},
    {'U',0x15},
    {'V',0x16},
    {'W',0x17},
    {'X',0x18},
    {'Y',0x19},
    {'Z',0x1a}
 };

static hdmi_HextoDecConverter_t  HextoDecConverter[6]=
{
    {'A', 10},
    {'B', 11},
    {'C', 12},
    {'D', 13},
    {'E', 14},
    {'F', 15}
};
static HI_U32 hdmi_LinuxPow(HI_U32 X , HI_U32 Y)
{
  HI_U32 Result = 1;
  HI_U32 i=0;
  for ( i=0; i<Y; i++)
  {
     Result = Result*X;
  }
  return(Result);
}

static HI_U32 sthdmi_HextoDecConverter(HI_U8 * Name_p, HI_U32 Size)
{
      HI_U32 Index;
      HI_U32 DecNumber=0;
      HI_BOOL Found=FALSE;

      while ((Name_p!=NULL)&&(Size!=0))
      {
        for (Index=0;Index<6; Index++)
        {
           if (HextoDecConverter[Index].HexNumber== (char)*Name_p)
           {
             Found=TRUE;
             break; /* found */
           }
        }

        (DecNumber +=(HI_U32)((*Name_p))*(hdmi_LinuxPow(16,(Size-1))));

        Found =FALSE;/*pos++;*/Name_p++;Size--;
     }
  return(DecNumber);
}
  
//-----------------------------------------------------------
static HI_U8 BasicParse(HI_U8 * pSysError, HI_U16 * pCRC16)
{    
    HI_U8 Error;
    HI_U8 CRC;
    HI_U32 Code, Index, IdSerialNumberCode;
    HI_U8 EDID_Basic_Block[128] = {0};
    HI_U8 IdSerialCode[2];
    HI_U8 Buffer[2];
    HI_BOOL CharacterFound;

    Error = DecodeHeader(pSysError, pCRC16);
    if(!Error)
    {
        if(! (*pSysError))
        {
            Error = CheckCRC(0, &CRC, pCRC16);
            if(!Error)
            {
                if(CRC)
                {
                    *pSysError = _1ST_BLOCK_CRC_ERROR;
                    Error = CheckEDIDVersion(pSysError, pCRC16);

                }
            }
        }
    }
    /* Fill basic data to global variable */
    Error |= SI_BlockReadEDID(128, 0, EDID_Basic_Block);
    if(Error != HI_SUCCESS)
    {
        HI_WARN_HDMI("BasicParse CheckVersion and BlockRead error ! %d",Error);
    }
    
    Code = (HI_U32)(EDID_Basic_Block[8]<< 8) + (HI_U32)(EDID_Basic_Block[9]);
    HI_INFO_HDMI("xx code:0x%08x<--0x%02x, 0x%02x\n", Code, EDID_Basic_Block[8], EDID_Basic_Block[9]);
    /* Manufacter Name */
    /* Retrieve the first character of the Id Manufacter Name.. */
    CharacterFound= FALSE;
    for (Index=0; Index<26; Index ++)
    {
        if ((Code&0x7C00)>>10 == IdManufacterCharCode[Index].Code)
        {
            CharacterFound= TRUE;
            break; /* found */
        }
    }
    HI_INFO_HDMI("a:0x%02x,w found:%d\n", ((Code&0x7C00)>>10), CharacterFound);
    (CharacterFound)?(g_stSinkCap.u8IDManufactureName[0]=(char)IdManufacterCharCode[Index].IdManufacterChar):(g_stSinkCap.u8IDManufactureName[0]='?');
    
    CharacterFound= FALSE;
    /* Retrieve the Second character of the Id Manufacter Name.. */
    for (Index=0; Index<26; Index ++)
    {
        if ((Code&0x03E0)>>5 == IdManufacterCharCode[Index].Code)
        {
            CharacterFound= TRUE;
            break; /* found */
        }
    }
    (CharacterFound)?(g_stSinkCap.u8IDManufactureName[1]=(char)IdManufacterCharCode[Index].IdManufacterChar):(g_stSinkCap.u8IDManufactureName[1]='?');

    CharacterFound= FALSE;
    /* Retrieve the third character of the Id Manufacter Name.. */
    for (Index=0;Index<26 ; Index++)
    {
        if ((Code&0x001F)== IdManufacterCharCode[Index].Code)
        {
            CharacterFound= TRUE;
            break; /* found */
        }
    }
    (CharacterFound)?(g_stSinkCap.u8IDManufactureName[2]=(char)IdManufacterCharCode[Index].IdManufacterChar):(g_stSinkCap.u8IDManufactureName[2]='?');
    g_stSinkCap.u8IDManufactureName[3] = '\0';
    /* ProductCode ID */
    g_stSinkCap.u32IDProductCode = (EDID_Basic_Block[8] << 8) + EDID_Basic_Block[9];
    /* SerialNumber ID */    
    IdSerialNumberCode = (EDID_Basic_Block[0x0c] << 24) + (EDID_Basic_Block[0x0d] << 16)
             + (EDID_Basic_Block[0x0e] << 8) + (EDID_Basic_Block[0x0f]);
    
    g_stSinkCap.u32IDSerialNumber=0;
    
    IdSerialCode[0]= (IdSerialNumberCode&0xF0000000)>>28;
    IdSerialCode[1]= (IdSerialNumberCode&0x0F000000)>>24;
    
    g_stSinkCap.u32IDSerialNumber += sthdmi_HextoDecConverter(IdSerialCode,2);              /* 2^0 */
    
    IdSerialCode[0]= (IdSerialNumberCode&0x00F00000)>>20;
    IdSerialCode[1]= (IdSerialNumberCode&0x000F0000)>>16;
    g_stSinkCap.u32IDSerialNumber += sthdmi_HextoDecConverter(IdSerialCode,2)*256;        /* 1*(2^8) */
    
    IdSerialCode[0]= (IdSerialNumberCode&0x0000F000)>>12;
    IdSerialCode[1]= (IdSerialNumberCode&0x00000F00)>>8;
    g_stSinkCap.u32IDSerialNumber += sthdmi_HextoDecConverter(IdSerialCode,2)*512;        /* 2*(2^8) */
    
    IdSerialCode[0]= (IdSerialNumberCode&0x000000F0)>>4;
    IdSerialCode[1]= (IdSerialNumberCode&0x0000000F);
    g_stSinkCap.u32IDSerialNumber += sthdmi_HextoDecConverter(IdSerialCode,2)*768;   /* 3*(2^8) */
    /* Week of Manufacture */
    g_stSinkCap.u32WeekOfManufacture = EDID_Basic_Block[0x10];
    /* Year of Manufacture */
    Buffer[0] = (EDID_Basic_Block[0x11]&0xF0)>>4;
    Buffer[1] = (EDID_Basic_Block[0x11]&0x0F);
    g_stSinkCap.u32YearOfManufacture = sthdmi_HextoDecConverter(Buffer,2)+1990;
    
    g_stSinkCap.u8Version    = EDID_Basic_Block[0x12];
    g_stSinkCap.u8Revision   = EDID_Basic_Block[0x13];
    g_stSinkCap.u8EDIDExternBlockNum = EDID_Basic_Block[0x7e]; /*extern flag*//*CNcomment:������־ */
    
    return Error;
}

static HI_U8 CheckCRCof2ndEDIDBlock( HI_U8 * pSysError, HI_U16 * pCRC16)
{
    HI_U8 Error;
    HI_U8 CRC;

    Error = CheckCRC(128, &CRC, pCRC16);
    if(!Error)
    {
        if(CRC)
        {
            *pSysError = _1ST_BLOCK_CRC_ERROR;
            Error = CheckEDIDVersion(pSysError, pCRC16);
        }
    }
    return Error;
}
#if 0 /* no used now ??  */
//-------------------------------------------------------------
static HI_U8 Parse861BExtension(HI_U16 * pCRC16)
{
    HI_U8 Error;
    HI_INFO_HDMI("MonitorCapable861\n");
    Error = MonitorCapable861(pCRC16);
    
    if(!Error)
        Error = ParsingCEADataBlockCollection(pCRC16);
        
    if(!Error)
        Error = ParseCEADetailedTimingDescriptors(pCRC16);
        
    return Error;
}
#endif
HI_S32 SI_PrepareEDID(HI_BOOL bForceMode)
{
    HI_U8  ret, NExtensions;
    HI_U8  ucData[16];
    HI_U16 index;
#if defined (DEBUG_SUPPORT) 
	HI_U16 tmpIdx;
#endif
    HI_U8  ByteperTime = 16;
    HI_BOOL bDVIMode, bHDMIMode;

    memset((void*)g_s32VmodeOfUNFFormat, 0, sizeof(g_s32VmodeOfUNFFormat));
    /* Refill g_stSinkCap */
    memset(&g_stSinkCap, 0, sizeof(HI_UNF_HDMI_SINK_CAPABILITY_S));
    memset((void*)(g_stSinkCap.bAudioFmtSupported), HI_FALSE, HI_UNF_HDMI_MAX_AUDIO_CAP_COUNT * sizeof(HI_BOOL));
    memset((void*)(g_stSinkCap.u32AudioSampleRateSupported), HI_FALSE, HI_UNF_HDMI_MAX_AUDIO_SMPRATE_COUNT * sizeof(HI_BOOL));
    g_stSinkCap.u32MaxPcmChannels = 2;
    g_stSinkCap.enNativeVideoFormat = HI_UNF_ENC_FMT_BUTT;
    memset((void*)(g_stSinkCap.bVideoFmtSupported), HI_FALSE, sizeof(HI_BOOL)*HI_UNF_ENC_FMT_BUTT);

    if(bForceMode == HI_FALSE)
    {
    #if 0
        if (BIT_HPD_PIN != (GetSysStat() & BIT_HPD_PIN))
    	{
		HI_ERR_HDMI("check BIT_HPD_PIN:0x%x\n", GetSysStat());
		return HI_FAILURE;
        }
    #endif    
        if (!SI_HPD_Status())
    	{
		    HI_ERR_HDMI("check BIT_HPD_PIN: no hot plug \n");
		    return HI_FAILURE;
        }
    }
    HI_INFO_HDMI("SI_PrepareEDID Read EDID\n");

    g_stSinkCap.bConnected   = HI_TRUE;   /* HPD is set, we should set connect TRUE */
    
    g_bHdmiEdidPrepareOk = HI_FALSE;
    HI_INFO_HDMI("1 set g_bHdmiEdidPrepareOk: to %d\n", g_bHdmiEdidPrepareOk);
    SI_Get_Force_OutputMode(&bDVIMode, &bHDMIMode);
    /*We will check DDC SDA/SCL Status */

    SI_DDC_Adjust();
    /* Clear EDID first before write.EEPROM must clear too. */
    SI_ResetEDIDBlock();
   
    /* Block 0 ofset:0x00~0x7f */
    SI_timer_count();
    
#if defined (DEBUG_EDID)
    __COUNT__ = 0;
#endif

    HI_INFO_HDMI("------- EDID Info -------\n");
    for (index = 0; index < 128; index += ByteperTime/*8*/)
    {
        memset(ucData, 0, ByteperTime/*8*/);
		
        /* Really to read EDID from DDC */
        ret = SI_BlockReadEDID_Real(ByteperTime/*8*/, index, ucData);
        if (ret)
        {
            //if can't read edid,default hdmi when force to set DVI 
            HI_WARN_HDMI("Can NOT read EDID This times.\n");
            HI_INFO_HDMI("NO EDID:Turn to Defalut Mode\n");
            if((bHDMIMode == bDVIMode) || (HI_TRUE == bHDMIMode))            
               g_bHDMIFlag = HI_TRUE;

            return HI_FAILURE;
        }

        /* Write EDID into EE_EDID_DATA buffer */
        SI_BlockWriteEEPROM( ByteperTime/*8*/, EE_EDID_DATA + index, ucData);
#if defined (DEBUG_SUPPORT) 
        for (tmpIdx = 0; tmpIdx < ByteperTime/*8*/; tmpIdx++)
        {
            HI_INFO_HDMI("%02x ", ucData[tmpIdx]);
        }
        HI_INFO_HDMI("\n");
#endif
    }

     /* Read NExtensions in first block */
    ret = SI_BlockReadEDID(1, NUM_EXTENSIONS_ADDR, (HI_U8 *)&NExtensions);

    if (! ret)
        HI_INFO_HDMI("We need to read %d * 128Bytes of extern E-EDID\n", NExtensions);

    /* Block 1 ofset:0x80~0xff */
    if(NExtensions >= 1)
    {    
        HI_INFO_HDMI("\n-------EDID Extern Info index:0--------\n");
        for (index = 128; index < 256; index += ByteperTime/*8*/)
        {
            memset(ucData, 0, ByteperTime/*8*/);
            /* Really to read EDID from DDC */
            ret = SI_BlockReadEDID_Real(ByteperTime/*8*/, index, ucData);
            if (ret)
            {
                HI_ERR_HDMI("Can NOT read EDID This times..\n");
                if((bHDMIMode == bDVIMode) || (HI_TRUE == bHDMIMode))
            	{
                    g_bHDMIFlag = HI_TRUE;
            	}
                return HI_FAILURE;
            }
            /* Write EDID into EE_EDID_DATA buffer */
            SI_BlockWriteEEPROM(ByteperTime/*8*/, EE_EDID_DATA + index, ucData);
#if defined (DEBUG_SUPPORT) 
            for (tmpIdx = 0; tmpIdx < ByteperTime/*8*/; tmpIdx++)
            {
                HI_INFO_HDMI("%02x ", ucData[tmpIdx]);
            }
            
            HI_INFO_HDMI("\n");
#endif
        }
        HI_INFO_HDMI("-----------end of EDID Extern Info:%d--------\n\n", index);
    }
/*Currently,more than 256 Bytes are not uesed,we only reserve in future*/
#if 0
    /* Block n ofset:128 (n+1 ~ n+2) */
    if(NExtensions > 1)
    {
        HI_U32 loop;
        
        if(NExtensions > 4)
        {
            HI_ERR_HDMI("Error EDID Extension No:%d\n", NExtensions);
            NExtensions = 4;
        }
        for (loop = 1; loop < NExtensions; loop ++)
        {
            HI_INFO_HDMI("\n------- other EDID Extern Info index:%d--------\n", index);
            for (index = (128 * loop + 128); index < (loop * 128 + 256); index += ByteperTime/*8*/)
            {
                memset(ucData, 0, ByteperTime/*8*/);
                /* Really to read EDID from DDC */
                ret = SI_BlockReadEDID_Real(ByteperTime/*8*/, index, ucData);
                if (ret)
                {
                    HI_ERR_HDMI("Can NOT read EDID.\n");
                    if((bHDMIMode == bDVIMode) || (HI_TRUE == bHDMIMode))
                	{
                        g_bHDMIFlag = HI_TRUE;
                	}
                    return HI_FAILURE;
                }
                /* Write EDID into EE_EDID_DATA buffer */
                SI_BlockWriteEEPROM(ByteperTime/*8*/, EE_EDID_DATA + index, ucData);
                for (tmpIdx = 0; tmpIdx < ByteperTime/*8*/; tmpIdx++)
                {
                    HI_INFO_HDMI("%02x ", ucData[tmpIdx]);
                }
                
                HI_INFO_HDMI("\n");
            }
            HI_INFO_HDMI("-----------EDID Extern Infoindex:%d--------\n\n", index);         
        }
    }
    /* Print Debug EDID */
    HI_INFO_HDMI("\nEDID Debug Begin, Block_Num:%d(*128bytes):\n", (NExtensions + 1));
    for (index = 0; index < (NExtensions + 1) * 128; index +=ByteperTime)
    {
        SI_BlockReadEEPROM(16, EE_EDID_DATA + index, ucData);
        HI_INFO_HDMI("%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X\n",
        ucData[0], ucData[1], ucData[2], ucData[3], ucData[4], ucData[5], ucData[6], ucData[7],
        ucData[8], ucData[9], ucData[10], ucData[11], ucData[12], ucData[13], ucData[14], ucData[15]);
    }
    HI_INFO_HDMI("EDID Debug End\n");
#endif	
    for (index = 0; index < 256; index +=ByteperTime)
    {
        SI_BlockReadEEPROM(ByteperTime, EE_EDID_DATA + index, ucData);
        SI_Proc_writeEDIDBlock(index, ucData, ByteperTime);
    }    
    
    HI_INFO_HDMI("SI_IsHDMICompatible Get HDMI mode\n"); 
    g_stSinkCap.bSupportHdmi = SI_IsHDMICompatible();
    g_stSinkCap.bIsRealEDID  = HI_TRUE;
    HI_INFO_HDMI("1 set g_bHdmiEdidPrepareOk:%d to true\n", g_bHdmiEdidPrepareOk);
    g_bHdmiEdidPrepareOk = HI_TRUE;
    
    return HI_SUCCESS;
}

HI_U8 SI_Set_Force_OutputMode(HI_BOOL bDVIMode, HI_BOOL bHDMIMode)
{
    g_forceDVIMode  = bDVIMode;
    g_forceHDMIMode = bHDMIMode;
    
    return HI_SUCCESS;
}

HI_U8 SI_Get_Force_OutputMode(HI_BOOL *bDVIMode, HI_BOOL *bHDMIMode)
{
    *bDVIMode    = g_forceDVIMode;
    *bHDMIMode   = g_forceHDMIMode;
    
    return HI_SUCCESS;
}

HI_U8 SI_EDID_Force_Setting(void)
{
    HI_U8 tempOutputState = SI_GetHDMIOutputStatus();
    
   // memset(&g_stSinkCap, 0, sizeof(HI_UNF_HDMI_SINK_CAPABILITY_S));


    /* Force to any timing */
    if(CABLE_UNPLUG_ != tempOutputState)
        g_stSinkCap.bConnected          = HI_TRUE;
    else
        g_stSinkCap.bConnected          = HI_FALSE;
   
    g_forceHDMIMode = g_bHDMIFlag;
    HI_INFO_HDMI("g_forceHDMIMode:%d,g_bHDMIFlag:%d\n", g_forceHDMIMode,g_bHDMIFlag);
    if (HI_TRUE == g_forceHDMIMode)
    {
        HI_INFO_HDMI("EDID Force to HDMI Mode\n");
        g_stSinkCap.bSupportHdmi        = HI_TRUE;/* HDMI Mode */

        g_stSinkCap.bAudioFmtSupported[1] = HI_TRUE; /* Only support PCM */
        /* Only support Basical Sample rate */
        g_stSinkCap.u32AudioSampleRateSupported[0] = HI_UNF_SAMPLE_RATE_32K;
        g_stSinkCap.u32AudioSampleRateSupported[1] = HI_UNF_SAMPLE_RATE_44K;
        g_stSinkCap.u32AudioSampleRateSupported[2] = HI_UNF_SAMPLE_RATE_48K;
        g_stSinkCap.bSupportYCbCr       = HI_TRUE;
        
        tempOutputState = CABLE_PLUGIN_HDMI_OUT; 
        SI_SetHDMIOutputStatus(tempOutputState);
    }
#if defined (DVI_SUPPORT)
    else
    {
        DEBUG_PRINTK("EDID Force to DVI Mode\n");
        g_stSinkCap.bSupportHdmi        = HI_FALSE;/* DVI Mode */
        g_stSinkCap.bSupportYCbCr       = HI_FALSE; /* DVI Only support RGB444 */
        tempOutputState = CABLE_PLUGIN_DVI_OUT; 
        SI_SetHDMIOutputStatus(tempOutputState);
        g_stSinkCap.bVideoFmtSupported[HI_UNF_ENC_FMT_VESA_800X600_60] = HI_TRUE;
        g_stSinkCap.bVideoFmtSupported[HI_UNF_ENC_FMT_VESA_1024X768_60] = HI_TRUE;
        return 0;
    }
#endif
    g_stSinkCap.enNativeVideoFormat = HI_UNF_ENC_FMT_720P_50;

#ifdef __HDMI_SIMPLYHD_TEST__
    /* Support Basecial DTV format in HDMI1.3 */
    g_stSinkCap.bVideoFmtSupported[HI_UNF_ENC_FMT_1080i_60] = HI_TRUE;
    g_stSinkCap.bVideoFmtSupported[HI_UNF_ENC_FMT_1080i_50] = HI_TRUE;
    g_stSinkCap.bVideoFmtSupported[HI_UNF_ENC_FMT_576P_50]  = HI_TRUE;
    g_stSinkCap.bVideoFmtSupported[HI_UNF_ENC_FMT_480P_60]  = HI_TRUE;
    g_stSinkCap.bVideoFmtSupported[HI_UNF_ENC_FMT_PAL]      = HI_FALSE;//SimplyHD HI_TRUE;
    g_stSinkCap.bVideoFmtSupported[HI_UNF_ENC_FMT_NTSC]     = HI_FALSE;//SimplyHD HI_TRUE;
    g_stSinkCap.bVideoFmtSupported[HI_UNF_ENC_FMT_861D_640X480_60] = HI_TRUE;
    /* Support the other DTV format in HDMI1.3 */
    if(tempOutputState == CABLE_PLUGIN_DVI_OUT)
    {
		DEBUG_PRINTK("DVI remove 1080p,720p\n");
		g_stSinkCap.bVideoFmtSupported[HI_UNF_ENC_FMT_1080P_60] = HI_FALSE;
		g_stSinkCap.bVideoFmtSupported[HI_UNF_ENC_FMT_1080P_50] = HI_FALSE;
		g_stSinkCap.bVideoFmtSupported[HI_UNF_ENC_FMT_1080P_30] = HI_FALSE;
		g_stSinkCap.bVideoFmtSupported[HI_UNF_ENC_FMT_1080P_25] = HI_FALSE;
		g_stSinkCap.bVideoFmtSupported[HI_UNF_ENC_FMT_1080P_24] = HI_FALSE;
		g_stSinkCap.bVideoFmtSupported[HI_UNF_ENC_FMT_720P_60]  = HI_FALSE;
		g_stSinkCap.bVideoFmtSupported[HI_UNF_ENC_FMT_720P_50]  = HI_FALSE;
    }
    else if(EDID_3BlockFlag == HI_TRUE)    
	{
		DEBUG_PRINTK("3 block remove 1080p\n");
		g_stSinkCap.bVideoFmtSupported[HI_UNF_ENC_FMT_1080P_60] = HI_FALSE;
		g_stSinkCap.bVideoFmtSupported[HI_UNF_ENC_FMT_1080P_50] = HI_FALSE;
		g_stSinkCap.bVideoFmtSupported[HI_UNF_ENC_FMT_1080P_30] = HI_FALSE;
		g_stSinkCap.bVideoFmtSupported[HI_UNF_ENC_FMT_1080P_25] = HI_FALSE;
		g_stSinkCap.bVideoFmtSupported[HI_UNF_ENC_FMT_1080P_24] = HI_FALSE;
		g_stSinkCap.bVideoFmtSupported[HI_UNF_ENC_FMT_720P_60]  = HI_FALSE;
		g_stSinkCap.bVideoFmtSupported[HI_UNF_ENC_FMT_720P_50]  = HI_FALSE;
	}
#else
    /* Support Basecial DTV format in HDMI1.3 */
    g_stSinkCap.bVideoFmtSupported[HI_UNF_ENC_FMT_1080i_60] = HI_TRUE;
    g_stSinkCap.bVideoFmtSupported[HI_UNF_ENC_FMT_1080i_50] = HI_TRUE;
    g_stSinkCap.bVideoFmtSupported[HI_UNF_ENC_FMT_720P_60]  = HI_TRUE;
    g_stSinkCap.bVideoFmtSupported[HI_UNF_ENC_FMT_720P_50]  = HI_TRUE;
    g_stSinkCap.bVideoFmtSupported[HI_UNF_ENC_FMT_576P_50]  = HI_TRUE;
    g_stSinkCap.bVideoFmtSupported[HI_UNF_ENC_FMT_480P_60]  = HI_TRUE;
    /* Support the other DTV format in HDMI1.3 */
    g_stSinkCap.bVideoFmtSupported[HI_UNF_ENC_FMT_1080P_60] = HI_TRUE;
    g_stSinkCap.bVideoFmtSupported[HI_UNF_ENC_FMT_1080P_50] = HI_TRUE;
    g_stSinkCap.bVideoFmtSupported[HI_UNF_ENC_FMT_1080P_30] = HI_TRUE;
    g_stSinkCap.bVideoFmtSupported[HI_UNF_ENC_FMT_1080P_25] = HI_TRUE;
    g_stSinkCap.bVideoFmtSupported[HI_UNF_ENC_FMT_1080P_24] = HI_TRUE;
    g_stSinkCap.bVideoFmtSupported[HI_UNF_ENC_FMT_PAL]      = HI_TRUE;
    g_stSinkCap.bVideoFmtSupported[HI_UNF_ENC_FMT_NTSC]     = HI_TRUE;
    g_stSinkCap.bVideoFmtSupported[HI_UNF_ENC_FMT_861D_640X480_60] = HI_TRUE;
#endif
   
    /*Set VIC*/
    g_s32VmodeOfUNFFormat[HI_UNF_ENC_FMT_1080P_60] = 0x0b; //0
    g_s32VmodeOfUNFFormat[HI_UNF_ENC_FMT_1080P_50] = 0x18; //1
    g_s32VmodeOfUNFFormat[HI_UNF_ENC_FMT_1080P_30] = 0x1b; //2
    g_s32VmodeOfUNFFormat[HI_UNF_ENC_FMT_1080P_25] = 0x1a; //3
    g_s32VmodeOfUNFFormat[HI_UNF_ENC_FMT_1080P_24] = 0x19; //4
    g_s32VmodeOfUNFFormat[HI_UNF_ENC_FMT_1080i_60] = 0x03; //5
    g_s32VmodeOfUNFFormat[HI_UNF_ENC_FMT_1080i_50] = 0x0e; //6
    g_s32VmodeOfUNFFormat[HI_UNF_ENC_FMT_720P_60]  = 0x02; //7
    g_s32VmodeOfUNFFormat[HI_UNF_ENC_FMT_720P_50]  = 0x0d; //8
    g_s32VmodeOfUNFFormat[HI_UNF_ENC_FMT_576P_50]  = 0x0c; //9
    g_s32VmodeOfUNFFormat[HI_UNF_ENC_FMT_480P_60]  = 0x01; //10
    g_s32VmodeOfUNFFormat[HI_UNF_ENC_FMT_PAL]      = 0x15; 
    g_s32VmodeOfUNFFormat[HI_UNF_ENC_FMT_NTSC]     = 0x06; 
    g_s32VmodeOfUNFFormat[HI_UNF_ENC_FMT_861D_640X480_60]  = 0x01;
    

    //g_bHdmiEdidPrepareOk = HI_TRUE;  //no setting g_bHdmiEdidPrepareOk
    HI_INFO_HDMI("2 set g_bHdmiEdidPrepareOk:%d to true\n", g_bHdmiEdidPrepareOk);
    
    return 0;
}

//-----------------------------------------------------------
HI_U8 SI_ParseEDID(EDIDParsedDataType * ParsedData)
{
    //HI_U8 CapN;
    HI_U8 Error, NExtensions = 0;
#if 0 /* no used */
    HI_U8 bEdidParseFull = 0;
#endif
    CEAOnFisrtPage = 0;
    ParsedData->ErrorCode = NO_ERR;
    ParsedData->DisplayType = UNKNOWN_DISPLAY;
    ParsedData->CRC16 = 0x0000;
    HI_INFO_HDMI("Enter SI_ParseEDID\n");
    Error = BasicParse(&ParsedData->ErrorCode, &ParsedData->CRC16);
    if(Error||ParsedData->ErrorCode)
    {
        HI_ERR_HDMI("BasicParse ERR=%d/%d\n", Error, ParsedData->ErrorCode);
        return Error;
    }    
    // Check 861B extension
    Error = CheckFor861BExtension(&NExtensions, &ParsedData->CRC16);
    if(HI_SUCCESS != Error)
    {
        HI_ERR_HDMI("CheckFor861BExtension ERR=%d\n", Error);
        return Error;
    }
    if(NExtensions > 3)
    {
        HI_ERR_HDMI("NExtensions=%d is too big, force to 3\n", NExtensions);
        NExtensions = 3;
    }
    
    HI_INFO_HDMI("SI_ParseEDID NExtensions=%d, there are %d E-EDID need to receive!!!!\n", NExtensions, NExtensions);
        
    if(0 == NExtensions)
    {
        ParsedData->ErrorCode =  NO_861B_EXTENSION;
        return Error;
    }
    
    // Check CRC, sum of extension (128 bytes) must be Zero (0)
    Error = CheckCRCof2ndEDIDBlock(&ParsedData->ErrorCode, &ParsedData->CRC16);
    if(Error || ParsedData->ErrorCode)
    {
        HI_ERR_HDMI("CheckCRCof2ndEDIDBlock error:%#x.\n", Error);
        return Error;
    }
#if defined (DVI_SUPPORT)
    Error = ParseTimingDescriptors(&ParsedData->CRC16);        // First 128 bytes
    if(Error)
        return Error;
#endif
    if(NExtensions >= 1) //deal with extension 1
    {
       HI_INFO_HDMI("Only one Externsion EDID occurs\n");
#if 0 /* no used */
        if(0 != bEdidParseFull)
        {
            Error = Parse861BExtension(&ParsedData->CRC16);
            if(Error)
            {
                HI_ERR_HDMI("Parse861BExtension error:%#x.\n", Error);
                return Error;
            }
        }
        else
#endif
        {
            Error = ParsingCEADataBlockCollection(&ParsedData->CRC16);
            if(Error)
            {
                HI_ERR_HDMI("ParsingCEADataBlockCollection error:%#x.\n", Error);
                return Error;
            }
        }
         
        Error = NewCheckHDMISignature(&ParsedData->ErrorCode, &ParsedData->CRC16);
        if(Error||ParsedData->ErrorCode)
            return Error;
            
        ParsedData->DisplayType = HDMI_DISPLAY;
		Error = MonitorCapable861(&ParsedData->CRC16);
		if(Error)
        {
            HI_ERR_HDMI("MonitorCapable861 error:%#x.\n", Error);
            return Error;
        }
#if 0 
        g_stSinkCap.bSupportYCbCr = HI_FALSE;
        Error = SI_BlockReadEDID(1, 0x83, &CapN);   /* E-EDID block 1:0x83 */
        if(!Error)
        {
            HI_INFO_HDMI("DTV monitor supports: \n");
            if(CapN & 0x80)
                HI_INFO_HDMI(" Underscan\n");
            if(CapN & 0x40)
                HI_INFO_HDMI(" Basic audio\n");
            if(CapN & 0x20)
            {
                g_stSinkCap.bSupportYCbCr = HI_TRUE;
                HI_INFO_HDMI(" YCbCr 4:4:4\n");
            }
            if(CapN & 0x10)
            {
                g_stSinkCap.bSupportYCbCr = HI_TRUE;
                HI_INFO_HDMI(" YCbCr 4:2:2\n");
            }
            HI_INFO_HDMI("\n Native formats in detail descriptors %i\n\n",(int)(CapN & 0x0F));
        }        
#endif
    }

    HI_INFO_HDMI("\nend of SI_ParseEDID\n");
    MDDCWriteOffset(0);

    {
        HI_U32 index;
        
        HI_INFO_HDMI("setting index****\n");
        for(index = 0; index <= HI_UNF_ENC_FMT_480P_60; index ++)
        {
            HI_INFO_HDMI("index:%d, Format:0x%02x\n", index, g_s32VmodeOfUNFFormat[index]);
        }
        HI_INFO_HDMI("****\n");
    } 
           
    return Error;
}
#if defined (HDCP_SUPPORT)
static HI_U32 HDMIDebugOpen = HI_FALSE;//HI_TRUE;//HI_FALSE;

HI_U32 SI_Set_DEBUG_Flag(HI_U32 DebugFlag)
{
    HDMIDebugOpen = DebugFlag;
    
    if(DebugFlag == 0x3fc)
    {
        DEBUG_PRINTK("***Specail doing for No HPD test\n");
        /*force output mode*/
        SI_Set_Force_OutputMode(HI_FALSE, HI_TRUE);
    }
    
    return 0;
}

HI_U32 HDMI_DEBUG(const char *format, ...)
{
    #if 0
    va_list args;
    
    memset((void*)&va_list, 0, sizeof(va_list));
    if (HI_TRUE == HDMIDebugOpen)
    {
    	va_start(args, format);
    	DEBUG_PRINTK(format, args);
    }
    #endif

    return 0;
}
#endif

HI_S32 SI_GetHdmiSinkCaps(HI_UNF_HDMI_SINK_CAPABILITY_S *pCapability)
{
    HI_U8 tempOutputState = SI_GetHDMIOutputStatus();
    
    memset(pCapability, 0, sizeof(HI_UNF_HDMI_SINK_CAPABILITY_S));
    
#ifdef __HDMI_SIMPLYHD_TEST__        
    g_stSinkCap.bVideoFmtSupported[HI_UNF_ENC_FMT_PAL]   = HI_FALSE;
    g_stSinkCap.bVideoFmtSupported[HI_UNF_ENC_FMT_NTSC] = HI_FALSE;
#endif

    /* whether Sink device power on */
    
    #if 0
    if(BIT_RSEN == (GetSysStat() & BIT_RSEN))
        g_stSinkCap.bIsSinkPowerOn = HI_TRUE;
    else
        g_stSinkCap.bIsSinkPowerOn = HI_FALSE;
    #endif 
        g_stSinkCap.bIsSinkPowerOn = SI_RSEN_Status();
    
    if ((tempOutputState == CABLE_UNPLUG_) || (HI_TRUE != SI_HPD_Status()))
    {
        //memset(&g_stSinkCap, 0, sizeof(HI_UNF_HDMI_SINK_CAPABILITY_S));
                
        HI_INFO_HDMI("Cable Out, g_stSinkCap.bConnected:%d!\n", g_stSinkCap.bConnected);
        memcpy(pCapability, &g_stSinkCap, sizeof(HI_UNF_HDMI_SINK_CAPABILITY_S));
        pCapability->bConnected = HI_FALSE;
#if defined (HDCP_SUPPORT)
        if (HDMIDebugOpen == 0x3fc)
        {
            DEBUG_PRINTK("Green channel\n");
            return HI_SUCCESS;
        }
#endif        
        return HI_FAILURE;
    }
    else
    {
        HI_INFO_HDMI("SI_GetHdmiSinkCaps\n");
        g_stSinkCap.bConnected      = HI_TRUE;   /* HPD is set, we should set connect TRUE */
    }
    
    HI_INFO_HDMI("g_bHdmiEdidPrepareOk:0x%x\n", g_bHdmiEdidPrepareOk);
    if (HI_TRUE != g_bHdmiEdidPrepareOk)
    {
        HI_ERR_HDMI("EDID Parse Error, Need to force setting\n");
        SI_EDID_Force_Setting();
    }
    else
    {
        g_stSinkCap.bSupportHdmi = SI_IsHDMICompatible();        
        if (HI_FALSE == g_stSinkCap.bSupportHdmi)
        {
            SI_EDID_Force_Setting();
        }
    }
        
    memcpy(pCapability, &g_stSinkCap, sizeof(HI_UNF_HDMI_SINK_CAPABILITY_S));
    
    return HI_SUCCESS;
}

//This function is used in enjoy project.
HI_U8 SI_Force_GetEDID(HI_U8 *datablock, HI_U32 *length)
{
    HI_U32 ret;
    ret = SI_PrepareEDID(HI_TRUE);
    if(ret != HI_SUCCESS) 
    {
        HI_ERR_HDMI("SI_PrepareEDID fail!\n");
        return ret;
    }
    SI_Proc_ReadEDIDBlock(datablock, 256);
    *length = 256;

    return 0;
}
/*------------------------END--------------------------*/
