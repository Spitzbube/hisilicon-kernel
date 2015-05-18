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

#include "drv_global.h"
#include "hi_drv_module.h"

extern struct
{
	HI_U32 fmt;
}
VICModeTables[5]; /* =
{
		{HI_UNF_ENC_FMT_BUTT},
		{HI_UNF_ENC_FMT_3840X2160_30},
		{HI_UNF_ENC_FMT_3840X2160_25},
		{HI_UNF_ENC_FMT_3840X2160_24},
		{HI_UNF_ENC_FMT_4096X2160_24}
}; */

static HI_U8 g_EdidMen[EDID_SIZE];
static HI_BOOL g_bErrList[EDID_MAX_ERR_NUM] = {0}; //811349C4

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

#if 0
static SI_Vendor_3D_S stVendor3D;
static HI_BOOL g_bHDMIFlag = HI_FALSE;
#endif

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
#if 0
static HI_UNF_HDMI_SINK_CAPABILITY_S g_stSinkCap;

static HI_BOOL g_forceDVIMode  = HI_FALSE;
static HI_BOOL g_forceHDMIMode = HI_FALSE;
Bool CEAOnFisrtPage;  
#endif

int g_s32VmodeOfUNFFormat[HI_UNF_ENC_FMT_BUTT] = {0}; //811343b4

#if 0
static void SaveEdidToEeprom(HI_U8 offset, HI_U8 ucLen, HI_U8 *pucData)
{
    HI_U16 uwAddr = EE_SINK_CAP_ADDR + offset;
	
    SI_BlockWriteEEPROM(ucLen, uwAddr, pucData);    
    return ;
}
#endif

static HI_S32 BlockCheckSum(HI_U8 *pEDID)
{
    HI_U8 u8Index;
    HI_U32 u32CheckSum = 0;
    //HI_U8 u8CheckSum = 0;

    if(pEDID == HI_NULL)
    {
        HI_ERR_HDMI("null point checksum\n"); //38
        return HI_FAILURE;
    }

    for(u8Index = 0; u8Index < EDID_BLOCK_SIZE; u8Index++)
    {
        u32CheckSum += pEDID[u8Index];
    }

    //u8CheckSum = (256 - (u32Sum%256))%256;
    if((u32CheckSum & 0xff) != 0x00)
    {
         HI_INFO_HDMI("addr(0x7F):0x%02x,checksum:0x%02x \n",
                    pEDID[EDID_BLOCK_SIZE - 1],u32CheckSum); //51

        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

HI_S32 SI_ReadEDID (HI_U8 NBytes, HI_U16 Addr, HI_U8 * Data)
{

    HI_S32 s32Ret = 0;

#ifndef DEBUG_EDID
    if(!DRV_Get_IsUserEdid(HI_UNF_HDMI_ID_0))
    {
        TmpDType TmpD;
        memset((void*)&TmpD, 0, sizeof(TmpD));

        TmpD.MDDC.SlaveAddr = DDC_ADDR;
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
        s32Ret = SI_BlockRead_MDDC(&TmpD.MDDC);

        MDDCWriteOffset(0); //reset Segment offset to 0
    }
    else
    {
        int index = 0;
        HDMI_EDID_S *pEDID = DRV_Get_UserEdid(HI_UNF_HDMI_ID_0);

        if(Addr >= pEDID->u32Edidlength)
        {
        	HI_ERR_HDMI("analog Err block edid device,can not read any more \n"); //94
            return HI_FAILURE;
        }

        for(index = 0; index < NBytes; index ++)
        {
            Data[index] = pEDID->u8Edid[Addr + index];
        }
    }
#else
    int index = 0;
    if(Addr >= sizeof(EDID_BLOCK_TEST15))
    {
        HI_ERR_HDMI("analog block edid device,can not read any more \n");
        return HI_FAILURE;
    }

    for(index = 0; index < NBytes; index ++)
    {
        Data[index] = EDID_BLOCK_TEST15[Addr + index];
    }
#endif
    return s32Ret;
}


static HI_U32 SI_DDC_Adjust(void)
{
    HI_U32 ret;
    HI_U32 value;
    HI_U32 gpio_scl,gpio_sda;
    HI_S32 count = 0;
    GPIO_EXT_FUNC_S *gpio_func_ops = HI_NULL;
    
    HI_INFO_HDMI("Enter SI_DDC_Adjust \n"); //127

    ret = HI_DRV_MODULE_GetFunction(HI_ID_GPIO, (HI_VOID**)&gpio_func_ops);    
    if((NULL == gpio_func_ops) || (ret != HI_SUCCESS))
    {
        HI_FATAL_HDMI("can't get gpio funcs!\n"); //132
        return HI_FAILURE;
    }
    
    /*if func ops and func ptr is null, then return ;*/
    if(!gpio_func_ops || !gpio_func_ops->pfnGpioDirSetBit ||
        !gpio_func_ops->pfnGpioReadBit || !gpio_func_ops->pfnGpioWriteBit)
    {
        HI_INFO_HDMI("gpio hdmi init err\n"); //140
        return HI_FAILURE;
    }

    //when DDC_DELAY_CNT = 0x14  oscclk   dscl = oscclk / (DDC_DELAY_CNT * 30) = 100khz
    // dscl should in range 48Khz ~ 72Khz
    //50khz
    //DRV_HDMI_WriteRegister(0xf8ce03d8, 0x28); //DDC_DELAY_CNT
    //100khz
    //DRV_HDMI_WriteRegister(0xf8ce03d8, 0x14); //DDC_DELAY_CNT

    // 75Khz DDC_DELAY_CNT
    //WriteByteHDMITXP0(DDC_DELAY_CNT,0x1a);
    // 63Khz
    //DRV_HDMI_WriteRegister(0xf8ce03d8, 0x1f); //DDC_DELAY_CNT
    // phy reg3  osc clk : 57.56 ~ 72.23 Mhz  delay = 0x60  ===> I2C clk 75 ~ 94khz
    //why 21khz
    //DRV_HDMI_WriteRegister(0xf8ce03d8, 0x60); //DDC_DELAY_CNT

    WriteByteHDMITXP0(DDC_DELAY_CNT, DRV_Get_DDCSpeed());

    DelayMS(1);
    
    //bit 6  bus_low
    value =ReadByteHDMITXP0(DDC_STATUS);
    HI_INFO_HDMI("DDC Status(0x72:0xf2):0x%x\n", value); //165

#if 1

    //HI3716Cv200 Series
    #if    defined(CHIP_TYPE_hi3716cv200)   \
        || defined(CHIP_TYPE_hi3716mv400)   \
        || defined(CHIP_TYPE_hi3718cv100)   \
        || defined(CHIP_TYPE_hi3719cv100)

        HI_INFO_HDMI("Adjust in 3716Cv200 Series\n");
        //gpio
        if(g_pstRegIO->ioshare_reg0.u32 == 0x01)
        {
            //PIO Mode
            //ioshare reg0 0x00 GPIO0_0 // HDMI_TX_SCL
            //ioshare reg1 0x00 GPIO0_1 // HDMI_TX_SDA
            g_pstRegIO->ioshare_reg0.u32 = 0x00;
            g_pstRegIO->ioshare_reg1.u32 = 0x00;

            //  u32GpioNo = ×éºÅ*8 + ÔÚ×éÀïµÄ±àºÅ

            //HDMI_TX_SCL GPIO0_0
            //0*8 + 0
            gpio_scl = 0;
            //Set GPIO0_1:HDMI_SDA input
            //Ret = HI_UNF_GPIO_SetDirBit(GPIO_ID*8 + GPIO_Reg, HI_TRUE);//HDMI_SDA
            gpio_sda = 1;

            SI_DDC_ResetTV(gpio_scl,gpio_sda);
            //SDA/SCL Mode
            //ioshare reg0 0x01 HDMI_TX_SCL
            //ioshare reg1 0x01 HDMI_TX_SDA  //DDC
            g_pstRegIO->ioshare_reg0.u32 = 0x01;
            g_pstRegIO->ioshare_reg1.u32 = 0x01;
        }

    //HI3719Mv100 Series
    #elif defined(CHIP_TYPE_hi3718mv100) || defined (CHIP_TYPE_hi3719mv100)

         HI_INFO_HDMI("Adjust in 3719Mv100 Series\n");
         //gpio
         if(g_pstRegIO->ioshare_reg13.u32 == 0x01)
         {
             //PIO Mode
             //ioshare reg13 0x00 GPIO1_5 // HDMI_TX_SCL
             //ioshare reg14 0x00 GPIO1_6 // HDMI_TX_SDA
             g_pstRegIO->ioshare_reg13.u32 = 0x00;
             g_pstRegIO->ioshare_reg14.u32 = 0x00;


             //HDMI_TX_SCL GPIO1_5
             gpio_scl = 13;
             //Set GPIO1_6:HDMI_SDA input
             gpio_sda = 14;

             SI_DDC_ResetTV(gpio_scl,gpio_sda);

             //SDA/SCL Mode
             //ioshare reg0 0x01 HDMI_TX_SCL
             //ioshare reg1 0x01 HDMI_TX_SDA  //DDC
             g_pstRegIO->ioshare_reg13.u32 = 0x01;
             g_pstRegIO->ioshare_reg14.u32 = 0x01;
         }

    #elif  defined(CHIP_TYPE_hi3751v100)    \
        || defined(CHIP_TYPE_hi3796cv100)   \
        || defined(CHIP_TYPE_hi3798cv100)

        HI_INFO_HDMI("Adjust in 3798Cv100 Series\n"); //306

        if (g_pstRegIO->ioshare_reg124.u32 == 0x00)
        {
            // PIO Mode
            // ioshare reg124 0x02 GPIO2_4 // HDMI_TX_SCL
            // ioshare reg125 0x02 GPIO2_5 // HDMI_TX_SDA

            // 0xf8a211f0Åä0 £»
            // 0xf8a211f4Åä0 £»
            g_pstRegIO->ioshare_reg124.u32 = 0x02;
            g_pstRegIO->ioshare_reg125.u32 = 0x02;

            //  u32GpioNo = ×éºÅ*8 + ÔÚ×éÀïµÄ±àºÅ

            //HDMI_TX_SCL GPIO2_4
            //2*8 + 4
            gpio_scl = 20;
            //Set GPIO2_5:HDMI_SDA input
            //Ret = HI_UNF_GPIO_SetDirBit(GPIO_ID*8 + GPIO_Reg, HI_TRUE);//HDMI_SDA
            //SI_GPIO_SetDirBit(2, 5, HI_TRUE);
            gpio_sda = 21;

#if 0
            SI_DDC_ResetTV(gpio_scl,gpio_sda);
#else
            //if SDA is low, need to create clock in SCL
            //SDA is high

            //HDMI_SDA
            //Set GPIO0_1:HDMI_SDA input
            //  u32GpioNo = ×éºÅ*8 + ÔÚ×éÀïµÄ±àºÅ
            //Ret = HI_UNF_GPIO_SetDirBit(0*8 + 1, HI_TRUE);//GPIO0_1:HDMI_SDA
            //SI_GPIO_SetDirBit(0, 1, HI_TRUE);
            gpio_func_ops->pfnGpioDirSetBit(gpio_sda,HI_TRUE);

            //HDMI_SCL
            gpio_func_ops->pfnGpioDirSetBit(gpio_scl,HI_FALSE);

            //HDMI_SDA
            gpio_func_ops->pfnGpioReadBit(gpio_sda,&value);

            HI_INFO_HDMI("Current Value:%d\n", value); //342
            //in order to pull up HDMI_SDA,try to set HDMI_SCL repeat
            while ((value == 0) && ((count ++) < 20))
            {
                HI_INFO_HDMI("***Pull up SCL Voltage for DDC, count:%d\n", count); //346
                //HDMI_SCL
                gpio_func_ops->pfnGpioWriteBit(gpio_scl,HI_TRUE);
                msleep(1);//msleep(1);
                //HDMI_SCL
                gpio_func_ops->pfnGpioWriteBit(gpio_scl,HI_FALSE);
                msleep(1);//msleep(1);

                //HDMI_SDA
                gpio_func_ops->pfnGpioReadBit(gpio_sda,&value);
            }

            if (value == 0)
            {
            	HI_ERR_HDMI("Can not pull up SCL Voltage\n"); //360
            }
#endif

            //SDA/SCL Mode
            //ioshare reg124 0x00 HDMI_TX_SCL
            //ioshare reg125 0x00 HDMI_TX_SDA  //DDC
            g_pstRegIO->ioshare_reg124.u32 = 0x00;
            g_pstRegIO->ioshare_reg125.u32 = 0x00;
        }

    #elif  defined(CHIP_TYPE_hi3798mv100)   \
        || defined(CHIP_TYPE_hi3796mv100)

        HI_INFO_HDMI("Adjust in 3798mv100 Series\n");
        //gpio
    #if 1 /*--NO MODIFY : COMMENT BY CODINGPARTNER--*/
        if(g_pstRegIO->ioshare_46.u32 == 0x01)
        {
            //PIO Mode
            //ioshare reg46 0x00 GPIO4_5 // HDMI_TX_SCL
            //ioshare reg45 0x00 GPIO4_4 // HDMI_TX_SDA
            g_pstRegIO->ioshare_46.u32 = 0x02;
            g_pstRegIO->ioshare_45.u32 = 0x02;


            //HDMI_TX_SCL GPIO4_5
            gpio_scl = 37;
            //Set GPIO4_4:HDMI_SDA input
            gpio_sda = 36;

            SI_DDC_ResetTV(gpio_scl,gpio_sda);

            //SDA/SCL Mode
            //ioshare reg0 0x01 HDMI_TX_SCL
            //ioshare reg1 0x01 HDMI_TX_SDA  //DDC
            g_pstRegIO->ioshare_46.u32 = 0x01;
            g_pstRegIO->ioshare_45.u32 = 0x01;
        }
    #endif /*--NO MODIFY : COMMENT BY CODINGPARTNER--*/

    #endif

#else
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
#endif

    return HI_SUCCESS;
}

static HI_S32 GetExtBlockNum(HI_U8 ExtBlockNum)
{
    if (ExtBlockNum > 3)
    {
        ExtBlockNum = 3;
        HI_ERR_HDMI("Ext Block num is too big\n"); //382
    }
    return ExtBlockNum;
}

HI_S32 SI_ReadSinkEDID(HI_VOID)
{
#warning TODO: SI_ReadSinkEDID
    HI_U32 u32Index, u32ExtBlockNum, s32Ret, i;
    HI_U8  u8Data[16];
    HI_U16 u16Byte = 16;
    HI_UNF_EDID_BASE_INFO_S *pSinkCap = DRV_Get_SinkCap(HI_UNF_HDMI_ID_0);
    HDMI_PRIVATE_EDID_S *pPriSinkCap = DRV_Get_PriSinkCap(HI_UNF_HDMI_ID_0);

    DRV_Set_SinkCapValid(HI_UNF_HDMI_ID_0,HI_FALSE);
    memset(g_EdidMen, 0, EDID_SIZE);
    memset(pSinkCap, 0,sizeof(HI_UNF_EDID_BASE_INFO_S));
    memset((void*)g_s32VmodeOfUNFFormat, 0, sizeof(g_s32VmodeOfUNFFormat));
    memset(g_bErrList,HI_FALSE,EDID_MAX_ERR_NUM * sizeof(HI_BOOL));
    memset(pPriSinkCap,0,sizeof(HDMI_PRIVATE_EDID_S));
    //init native fmt
    pSinkCap->enNativeFormat = HI_UNF_ENC_FMT_BUTT;

    SI_DDC_Adjust();
    HI_INFO_HDMI("read block one edid\n"); //405
    //read first block edid
    for(u32Index = 0; u32Index < EDID_BLOCK_SIZE; u32Index += u16Byte)
    {
        s32Ret = SI_ReadEDID(u16Byte, u32Index, u8Data);
        if(HI_SUCCESS != s32Ret)
        {
            HI_ERR_HDMI("can't read edid block 1\n"); //412
            return EDID_READ_FIRST_BLOCK_ERR;
        }

        for(i = 0; i < u16Byte; i++)
        {
            g_EdidMen[u32Index + i] = u8Data[i];//save mem
        }

        if((u32Index)%EDID_BLOCK_SIZE == 0)
        {
            HI_INFO_HDMI("EDID Debug Begin, Block_Num:%d(*128bytes):\n", (u32Index/EDID_BLOCK_SIZE + 1)); //423
        }
        HI_INFO_HDMI("%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X\n",
        u8Data[0], u8Data[1], u8Data[2], u8Data[3], u8Data[4], u8Data[5], u8Data[6], u8Data[7],
        u8Data[8], u8Data[9], u8Data[10], u8Data[11], u8Data[12], u8Data[13], u8Data[14], u8Data[15]); //427
        //HI_INFO_HDMI("\n");
    }

    u32ExtBlockNum = GetExtBlockNum(g_EdidMen[EXT_BLOCK_ADDR]);
    pSinkCap->u8ExtBlockNum = u32ExtBlockNum;

    if(u32ExtBlockNum > 3)
    {
        HI_WARN_HDMI("Extern block too big, correct to 3 \n");
        u32ExtBlockNum = 3;
    }

    //check first block sum
    s32Ret = BlockCheckSum(g_EdidMen);
    if(s32Ret != HI_SUCCESS)
    {
        HI_WARN_HDMI("First Block Crc Error! \n"); //444
        g_bErrList[_1ST_BLOCK_CRC_ERROR] = HI_TRUE;
    }

    //read extern block
    HI_INFO_HDMI("read extern block edid\n"); //449
    for(u32Index = EDID_BLOCK_SIZE; u32Index < EDID_BLOCK_SIZE *(u32ExtBlockNum + 1); u32Index += u16Byte)
    {
        s32Ret = SI_ReadEDID(u16Byte, u32Index, u8Data);
        if(HI_SUCCESS != s32Ret)
        {
            HI_ERR_HDMI("can't read edid block %d \n",(u32Index/EDID_BLOCK_SIZE + 1)); //455
            return (u32Index/EDID_BLOCK_SIZE + 1); // return err block no
        }

        for(i = 0; i < u16Byte; i++)
        {
            g_EdidMen[u32Index + i] = u8Data[i];//save mem
            //HI_INFO_HDMI("0x%02x,",u8Data[i]);
        }

        if((u32Index)%EDID_BLOCK_SIZE == 0)
        {
            HI_INFO_HDMI("EDID Debug Begin, Block_Num:%d(*128bytes):\n", (u32Index/EDID_BLOCK_SIZE + 1)); //467
        }
        HI_INFO_HDMI("%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X\n",
        u8Data[0], u8Data[1], u8Data[2], u8Data[3], u8Data[4], u8Data[5], u8Data[6], u8Data[7],
        u8Data[8], u8Data[9], u8Data[10], u8Data[11], u8Data[12], u8Data[13], u8Data[14], u8Data[15]); //471
        //HI_INFO_HDMI("\n");
    }

    return HI_SUCCESS;
}


static HI_S32 CheckHeader(HI_U8 *pData)
{
    HI_U32 u32Index;
    const HI_U8 Block_Zero_header[] ={0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00};

    for (u32Index = 0; u32Index < 8; u32Index ++)
    {
        if(pData[u32Index] != Block_Zero_header[u32Index])
        {
            HI_WARN_HDMI("BAD_HEADER Index:%d, 0x%02x\n",u32Index, pData[u32Index]); //488
            g_bErrList[BAD_HEADER] = HI_TRUE;
        }
    }
    return HI_SUCCESS;
}

static HI_S32 ParseVendorInfo(EDID_FIRST_BLOCK_INFO *pData)
{
    HI_U16 u16Index,u16Data;
    HI_UNF_EDID_BASE_INFO_S *pSinkCap = DRV_Get_SinkCap(HI_UNF_HDMI_ID_0);
    HI_UNF_EDID_MANUFACTURE_INFO_S *pstVendor = &pSinkCap->stMfrsInfo;

   	if(pData == NULL)
	{
		return HI_FAILURE;
    }

	u16Data = (pData->mfg_id[0]<<8) | (pData->mfg_id[1]);

	for(u16Index = 0; u16Index < 3; u16Index++)
	{
	    pstVendor->u8MfrsName[2 - u16Index]= ((u16Data & (0x1F << (5*u16Index))) >> (5*u16Index));
		if((0 < pstVendor->u8MfrsName[2 - u16Index])&&\
				(27 > pstVendor->u8MfrsName[2 - u16Index]))
		{
			pstVendor->u8MfrsName[2 - u16Index] += 'A' - 1;
		}
		else
		{
		    HI_INFO_HDMI("can't parse manufacture name\n");
			//return HI_FAILURE; //no must return
		}
	}
	pstVendor->u32ProductCode       = (pData->prod_code[1] << 8) | pData->prod_code[0];
	pstVendor->u32SerialNumber      = (pData->serial[3] << 24) | (pData->serial[2] << 16) | (pData->serial[1] << 8) | (pData->serial[0]);
	pstVendor->u32Week = pData->mfg_week;
	pstVendor->u32Year = pData->mfg_year + 1990;

	HI_INFO_HDMI("mfg name[%s]\n",pstVendor->u8MfrsName); //527
	HI_INFO_HDMI("code:%d\n",pstVendor->u32ProductCode);
	HI_INFO_HDMI("serial:%d\n",pstVendor->u32SerialNumber);
	HI_INFO_HDMI("year:%d,week:%d\n",pstVendor->u32Year, pstVendor->u32Week); //530

    return HI_SUCCESS;
}

HI_S32 ParseVersion(HI_U8 *pData1,HI_U8 *pData2)
{
    HI_UNF_EDID_BASE_INFO_S *pSinkCap = DRV_Get_SinkCap(HI_UNF_HDMI_ID_0);

    pSinkCap->u8Version  = *pData1;
    pSinkCap->u8Revision = *pData2;

    HI_INFO_HDMI("ver:%02d,rever:%02d\n",pSinkCap->u8Version,pSinkCap->u8Revision); //542
    return HI_SUCCESS;
}

HI_S32 ParseStandardTiming(HI_U8 *pData)
{
    HI_U8 TmpVal, i;
    HI_U32 Hor, Ver,aspect_ratio,freq;

    for(i = 0; i < STANDARDTIMING_SIZE; i += 2)
    {
        if((pData[i] == 0x01)&&(pData[i + 1]==0x01))
        {
            HI_INFO_HDMI("Mode %d wasn't defined! \n", (int)pData[i]); //555
        }
        else
        {
            HI_UNF_EDID_BASE_INFO_S *pSinkCap = DRV_Get_SinkCap(HI_UNF_HDMI_ID_0);
            Hor = (pData[i]+31)*8;
            HI_INFO_HDMI(" Hor Act pixels %d \n", Hor); //561
            TmpVal = pData[i + 1] & 0xC0;
            if(TmpVal==0x00)
            {
                HI_INFO_HDMI("Aspect ratio:16:10\n"); //565
                aspect_ratio = _16_10;
                Ver = Hor *10/16;
            }
            else  if(TmpVal==0x40)
            {
                HI_INFO_HDMI("Aspect ratio:4:3\n"); //571
                aspect_ratio = _4;
                Ver = Hor *3/4;
            }
            else  if(TmpVal==0x80)
            {
                HI_INFO_HDMI("Aspect ratio:5:4\n"); //577
                aspect_ratio = _5_4;
                Ver = Hor *4/5;
            }
            else //0xc0
            {
                HI_INFO_HDMI("Aspect ratio:16:9\n"); //583
                aspect_ratio = _16;
                Ver = Hor *9/16;
            }
            freq = ((pData[i + 1])& 0x3F) + 60;
            HI_INFO_HDMI(" Refresh rate %d Hz \n", freq); //588

            if(freq == 60)
            {
                if((Hor == 1280) && (Ver == 720))
                {
                    HI_INFO_HDMI("1280X720 \n"); //594
                    pSinkCap->bSupportFormat[HI_UNF_ENC_FMT_VESA_1280X720_60] = HI_TRUE;
                }
                else if ((Hor == 1280) && (Ver == 800))
                {
                    HI_INFO_HDMI("1280X800_RB \n"); //599
                    //pSinkCap->bSupportFormat[HI_UNF_ENC_FMT_VESA_1280X800_60] = HI_TRUE;
                }
                else if ((Hor == 1280) && (Ver == 1024))
                {
                    HI_INFO_HDMI("1280X1024 \n");
                    pSinkCap->bSupportFormat[HI_UNF_ENC_FMT_VESA_1280X1024_60] = HI_TRUE;
                }
                else if ((Hor == 1360) && (Ver == 768))
                {
                    HI_INFO_HDMI("1360X768 \n");
                    pSinkCap->bSupportFormat[HI_UNF_ENC_FMT_VESA_1360X768_60] = HI_TRUE;
                }
                else if ((Hor == 1366) && (Ver == 768))
                {
                    HI_INFO_HDMI("1366X768 \n");
                    pSinkCap->bSupportFormat[HI_UNF_ENC_FMT_VESA_1366X768_60] = HI_TRUE;
                }
                else if ((Hor == 1400) && (Ver == 1050))
                {
                    HI_INFO_HDMI("1400X1050_RB \n");
                    //pSinkCap->bSupportFormat[HI_UNF_ENC_FMT_VESA_1400X1050_60] = HI_TRUE;
                }
                else if ((Hor == 1440) && (Ver == 900))
                {
                    HI_INFO_HDMI("1440X900_RB \n");
                    pSinkCap->bSupportFormat[HI_UNF_ENC_FMT_VESA_1440X900_60_RB] = HI_TRUE;
                }
                else if ((Hor == 1600) && (Ver == 900))
                {
                    HI_INFO_HDMI("1600X900_RB \n");
                    pSinkCap->bSupportFormat[HI_UNF_ENC_FMT_VESA_1600X900_60_RB] = HI_TRUE;
                }
                else if ((Hor == 1600) && (Ver == 1200))
                {
                    HI_INFO_HDMI("1600X1200 \n"); //634
                    pSinkCap->bSupportFormat[HI_UNF_ENC_FMT_VESA_1600X1200_60] = HI_TRUE;
                }
                else if ((Hor == 1680) && (Ver == 1050))
                {
                    HI_INFO_HDMI("1680X1050_RB \n"); //639
                    pSinkCap->bSupportFormat[HI_UNF_ENC_FMT_VESA_1680X1050_60_RB] = HI_TRUE;
                }
                else if ((Hor == 1920) && (Ver == 1080))
                {
                    HI_INFO_HDMI("1920X1080 \n");
                    pSinkCap->bSupportFormat[HI_UNF_ENC_FMT_VESA_1920X1080_60] = HI_TRUE;
                }
                else if ((Hor == 1920) && (Ver == 1200))
                {
                    HI_INFO_HDMI("1920X1200_RB \n"); //649
                    //pSinkCap->bSupportFormat[HI_UNF_ENC_FMT_VESA_1920X1200_60] = HI_TRUE;
                }
                else if ((Hor == 1920) && (Ver == 1440))
                {
                    HI_INFO_HDMI("1920X1440 \n"); //654
                    pSinkCap->bSupportFormat[HI_UNF_ENC_FMT_VESA_1920X1440_60] = HI_TRUE;
                }
                else if ((Hor == 2048) && (Ver == 1152))
                {
                    HI_INFO_HDMI("2048X1152_RB \n"); //659
                    //pSinkCap->bSupportFormat[HI_UNF_ENC_FMT_VESA_2048X1152_60] = HI_TRUE;
                }
                else if ((Hor == 2560) && (Ver == 1440))
                {
                    HI_INFO_HDMI("2560X1440_RB \n");
                    pSinkCap->bSupportFormat[HI_UNF_ENC_FMT_VESA_2560X1440_60_RB] = HI_TRUE;
                }
                else if ((Hor == 2560) && (Ver == 1600))
                {
                    HI_INFO_HDMI("2560X1600_RB \n");
                    pSinkCap->bSupportFormat[HI_UNF_ENC_FMT_VESA_2560X1600_60_RB] = HI_TRUE;
                }
            }
        }
    }
    return HI_SUCCESS;
}

static HI_S32 ParseEstablishTiming(HI_U8 * pData)
{
    HI_UNF_EDID_BASE_INFO_S *pSinkCap = DRV_Get_SinkCap(HI_UNF_HDMI_ID_0);

    if(pData[0]& 0x80)
        HI_INFO_HDMI("720 x 400 @ 70Hz\n");
    if(pData[0]& 0x40)
        HI_INFO_HDMI("720 x 400 @ 88Hz\n");
    if(pData[0]& 0x20)
    {
        HI_INFO_HDMI("640 x 480 @ 60Hz\n");
        pSinkCap->bSupportFormat[HI_UNF_ENC_FMT_861D_640X480_60] = HI_TRUE;
    }
    if(pData[0]& 0x10)
        HI_INFO_HDMI("640 x 480 @ 67Hz\n");
    if(pData[0]& 0x08)
        HI_INFO_HDMI("640 x 480 @ 72Hz\n");
    if(pData[0]& 0x04)
        HI_INFO_HDMI("640 x 480 @ 75Hz\n");
    if(pData[0]& 0x02)
        HI_INFO_HDMI("800 x 600 @ 56Hz\n");

    if(pData[0]& 0x01)
    {
        pSinkCap->bSupportFormat[HI_UNF_ENC_FMT_VESA_800X600_60] = HI_TRUE;
        HI_INFO_HDMI("800 x 400 @ 60Hz\n");
    }

    if(pData[1]& 0x80)
        HI_INFO_HDMI("800 x 600 @ 72Hz\n");
    if(pData[1]& 0x40)
        HI_INFO_HDMI("800 x 600 @ 75Hz\n");
    if(pData[1]& 0x20)
        HI_INFO_HDMI("832 x 624 @ 75Hz\n");
    if(pData[1]& 0x10)
        HI_INFO_HDMI("1024 x 768 @ 87Hz\n"); //713

    if(pData[1]& 0x08)
    {
        pSinkCap->bSupportFormat[HI_UNF_ENC_FMT_VESA_1024X768_60] = HI_TRUE;
        HI_INFO_HDMI("1024 x 768 @ 60Hz\n");
    }
    if(pData[1]& 0x04)
        HI_INFO_HDMI("1024 x 768 @ 70Hz\n");
    if(pData[1]& 0x02)
        HI_INFO_HDMI("1024 x 768 @ 75Hz\n");
    if(pData[1]& 0x01)
        HI_INFO_HDMI("1280 x 1024 @ 75Hz\n"); //725

    if(pData[2]& 0x80)
        HI_INFO_HDMI("1152 x 870 @ 75Hz\n"); //728

    if((!pData[0])&&(!pData[1])&&(!pData[2]))
        HI_INFO_HDMI("No established video modes\n");

    return HI_SUCCESS;
}

HI_S32 ParsePreferredTiming(HI_U8 * pData)
{
    HI_U32 u32Temp;
    DETAILED_TIMING_BLOCK *pDetailed = (DETAILED_TIMING_BLOCK*)pData;
    HI_UNF_EDID_BASE_INFO_S *pSinkCap = DRV_Get_SinkCap(HI_UNF_HDMI_ID_0);
    HI_UNF_EDID_TIMING_S *pEdidTiming = &pSinkCap->stPerferTiming;

     //pixel clock
    u32Temp = (pDetailed->pixel_clk[0]|(pDetailed->pixel_clk[1]<<8))*10; //unit HZ
    HI_INFO_HDMI("pixel clock :%d\n",u32Temp); //745
    pEdidTiming->u32PixelClk = u32Temp;

     //vfb
    u32Temp = pDetailed->v_border ;
    u32Temp += (pDetailed->vs_offset_pulse_width >> 4)| ((pDetailed->hs_offset_vs_offset&0x0C) << 4);
    HI_INFO_HDMI("VFB :%d\n",u32Temp); //751
    pEdidTiming->u32VFB = u32Temp;

    //vbb
    //v_active_blank == vblack == vfront + vback + vsync = VFB(vfront) + VBB(vback + vsync)
    u32Temp = ((pDetailed->v_active_blank & 0x0F)<<8)|pDetailed->v_blank;
    HI_INFO_HDMI("VBB :%d\n",u32Temp); //757
    pEdidTiming->u32VBB = u32Temp - pEdidTiming->u32VFB;

    //vact
    u32Temp = ((pDetailed->v_active_blank & 0xF0) << 4)|pDetailed->v_active;
    HI_INFO_HDMI("VACT :%d\n",u32Temp); //762
    pEdidTiming->u32VACT = u32Temp;

    //HFB
    u32Temp = pDetailed->h_border;
    u32Temp += pDetailed->h_sync_offset|((pDetailed->hs_offset_vs_offset & 0xC0)<<2);
    HI_INFO_HDMI("HFB :%d\n",u32Temp); //768
    pEdidTiming->u32HFB = u32Temp;
    //HBB
    u32Temp = pDetailed->h_blank;
    u32Temp += (pDetailed->h_active_blank&0x0F) << 8;
    HI_INFO_HDMI("HBB :%d\n",u32Temp); //773
    //h_active_blank == hblack == hfront + hback + hsync = HFB(hfront) + HBB(hback + hsync)
    pEdidTiming->u32HBB = u32Temp - pEdidTiming->u32HFB;

    //HACT
    u32Temp = ((pDetailed->h_active_blank & 0xF0) << 4)| pDetailed->h_active;
    HI_INFO_HDMI("HACT :%d\n",u32Temp); //779
    pEdidTiming->u32HACT = u32Temp;

	//VPW
    u32Temp = (pDetailed->hs_offset_vs_offset & 0x03) << 4;
    u32Temp |= (pDetailed->vs_offset_pulse_width &0x0F);
    HI_INFO_HDMI("VPW :%d\n",u32Temp); //785
    pEdidTiming->u32VPW = u32Temp;
    //HPW
    u32Temp = (pDetailed->hs_offset_vs_offset & 0x30) << 4;
    u32Temp |= pDetailed->h_sync_pulse_width;
    HI_INFO_HDMI("HPW :%d\n",u32Temp); //790
    pEdidTiming->u32HPW = u32Temp;

    // H image size
    u32Temp = pDetailed->h_image_size;
    u32Temp |= (pDetailed->h_v_image_size & 0xF0) << 4;
    HI_INFO_HDMI("H image size :%d\n",u32Temp); //796
    pEdidTiming->u32ImageWidth = u32Temp;

    //V image size
    u32Temp = (pDetailed->h_v_image_size & 0x0F) << 8;
    u32Temp  |= pDetailed->v_image_size ;
    HI_INFO_HDMI("V image size :%d\n",u32Temp); //802
    pEdidTiming->u32ImageHeight = u32Temp;
    if(pDetailed->flags & 0x80)         /*Interlaced flag*/
	{
        HI_INFO_HDMI("Output mode: interlaced\n"); //806
        pEdidTiming->bInterlace = HI_TRUE;
	}
	else
	{
        HI_INFO_HDMI("Output mode: progressive\n");
        pEdidTiming->bInterlace = HI_FALSE;
	}

    /*Sync Signal Definitions Type*/
    if(0 == (pDetailed->flags & 0x10))     /*Analog Sync Signal Definitions*/
	{
		switch((pDetailed->flags & 0x0E) >> 1)
		{
			case 0x00:          /*Analog Composite Sync - Without Serrations - Sync On Green Signal only*/
                HI_INFO_HDMI("sync acs ws green\n");
				break;

			case 0x01:                  /*Analog Composite Sync - Without Serrations - Sync On all three (RGB) video signals*/
                HI_INFO_HDMI("sync acs ws all\n");
				break;

			case 0x02:                  /*Analog Composite Sync - With Serrations (H-sync during V-sync); - Sync On Green Signal only*/
                HI_INFO_HDMI("sync acs ds green\n"); //829
				break;

			case 0x03:                  /*Analog Composite Sync - With Serrations (H-sync during V-sync); - Sync On all three (RGB) video signals*/
                HI_INFO_HDMI("sync acs ds all\n");
                break;

			case 0x04:                  /*Bipolar Analog Composite Sync - Without Serrations; - Sync On Green Signal only*/
                HI_INFO_HDMI("sync bacs ws green\n");
                break;

			case 0x05:                  /*Bipolar Analog Composite Sync - Without Serrations; - Sync On all three (RGB) video signals*/
                HI_INFO_HDMI("sync bacs ws all\n");
                break;

			case 0x06:                  /*Bipolar Analog Composite Sync - With Serrations (H-sync during V-sync); - Sync On Green Signal only*/
                HI_INFO_HDMI("sync bacs ds green\n");
                break;

			case 0x07:                  /*Bipolar Analog Composite Sync - With Serrations (H-sync during V-sync); - Sync On all three (RGB) video signals*/
                HI_INFO_HDMI("sync bacs ds all\n");
                break;
			default:
				break;

          }
    }
    else                /*Digital Sync Signal Definitions*/
    {
 		switch((pDetailed->flags & 0x0E) >> 1)
		{
			case 0x01:
            case 0x00:                  /*Digital Composite Sync - Without Serrations*/
				//new_customer->Sync_type = HI_UNF_EDID_SYNC_DCS_WS;
                pEdidTiming->bIHS = 0;
                pEdidTiming->bIVS = 0;
                break;

			case 0x02:                   /*Digital Composite Sync - With Serrations (H-sync during V-sync)*/
			case 0x03:
				//new_customer->Sync_type = HI_UNF_EDID_SYNC_DCS_DS;
                break;

			case 0x04:                  /*Digital Separate Sync Vsync(-) Hsync(-)*/
				//new_customer->Sync_type = HI_UNF_EDID_SYNC_DSS_VN_HN;
                pEdidTiming->bIHS = 0;
                pEdidTiming->bIVS = 0;
                break;

			case 0x05:                  /*Digital Separate Sync Vsync(-) Hsync(+)*/
				//new_customer->Sync_type = HI_UNF_EDID_SYNC_DSS_VN_HP;
                pEdidTiming->bIHS = 1;
                pEdidTiming->bIVS = 0;
                break;

			case 0x06:                  /*Digital Separate Sync Vsync(+) Hsync(-)*/
				//new_customer->Sync_type = HI_UNF_EDID_SYNC_DSS_VP_HN;
                pEdidTiming->bIHS = 0;
                pEdidTiming->bIVS = 1;
                break;

			case 0x07:                  /*Digital Separate Sync Vsync(+) Hsync(+)*/
			//	new_customer->Sync_type = HI_UNF_EDID_SYNC_DSS_VP_HP;
                pEdidTiming->bIHS = 1;
                pEdidTiming->bIVS = 1;
                break;

			default:
				break;

		}
    }
    pEdidTiming->bIDV = 0;
    /*Stereo Viewing Support*/
    switch(((pDetailed->flags & 0x60) >> 4)|(pDetailed->flags & 0x01))
	{
		case 0x02:
            HI_INFO_HDMI("stereo sequential R\n");
			break;

		case 0x04:
            HI_INFO_HDMI("stereo sequential L\n");
			break;

		case 0x03:
            HI_INFO_HDMI("stereo interleaved 2R\n");
			break;

		case 0x05:
            HI_INFO_HDMI("stereo interleaved 2L\n");
			break;

		case 0x06:
             HI_INFO_HDMI("stereo interleaved 4\n");
			break;

		case 0x07:
            HI_INFO_HDMI("stereo interleaved SBS\n");
			break;

		default:
            HI_INFO_HDMI("stereo no\n"); //930
			break;

	}
     return HI_SUCCESS;
}

HI_S32 ParseFirstBlock(HI_U8 *pData)
{
    EDID_FIRST_BLOCK_INFO *pEdidInfo = (EDID_FIRST_BLOCK_INFO*)pData;

    CheckHeader(pData);
    ParseVendorInfo(pEdidInfo);
    ParseVersion(&pEdidInfo->version,&pEdidInfo->revision);
    ParseEstablishTiming(pEdidInfo->est_timing);
    ParseStandardTiming(pEdidInfo->std_timing);
    ParsePreferredTiming(&pData[FIRST_DETAILED_TIMING_ADDR]);
    return HI_SUCCESS;
}


static HI_VOID ParseDTVMonitorAttr(HI_U8 *pEDID)
{
    HI_UNF_EDID_BASE_INFO_S *pSinkCap = DRV_Get_SinkCap(HI_UNF_HDMI_ID_0);
    HI_UNF_EDID_COLOR_SPACE_S *pColorSpace = &pSinkCap->stColorSpace;
    HDMI_PRIVATE_EDID_S *pPriSinkCap = DRV_Get_PriSinkCap(HI_UNF_HDMI_ID_0);

    pColorSpace->bRGB444   = HI_TRUE;
    pColorSpace->bYCbCr422 = (pEDID[0] & BIT4 ? HI_TRUE : HI_FALSE) ;
    pColorSpace->bYCbCr444 = (pEDID[0] & BIT5 ? HI_TRUE : HI_FALSE);
    HI_INFO_HDMI("IT Underscan:0x%02x\n", ((pEDID[0] & BIT7)>>7)); //959

    pPriSinkCap->bUnderScanDev = (((pEDID[0] & BIT7)>>7)? HI_TRUE : HI_FALSE);
    //pColorSpace->unScanInfo.u32ScanInfo = (pEDID[0] & BIT7) >> 4;
    HI_INFO_HDMI("basic audio:0x%02x\n",  (pEDID[0] & BIT6)); //963
    HI_INFO_HDMI("bYCbCr444:%d\n", pColorSpace->bYCbCr444); //964
    HI_INFO_HDMI("bYCbCr422:%d\n", pColorSpace->bYCbCr422); //965
    HI_INFO_HDMI("DTDS num:0x%02x\n",  (pEDID[0] & 0x0F)); //966
}

static HI_S32 ParseAudioBlock(HI_U8 *pData, HI_U8 u8Len)
{
    HI_UNF_EDID_BASE_INFO_S *pSinkCap = DRV_Get_SinkCap(HI_UNF_HDMI_ID_0);
    HI_U8 Index, format_code, u8bit, u8Count;
    HI_UNF_EDID_AUDIO_INFO_S *pstAudioInfo = pSinkCap->stAudioInfo;
    /*one short audio descriptors length is 3 Bytes*/
    for(Index = 0; Index < (u8Len / 3); Index ++)
    {
        HI_U8 cur = pSinkCap->u32AudioInfoNum;
        u8Count = 0;

        if(cur >= HI_UNF_EDID_MAX_AUDIO_CAP_COUNT)
        {
            HI_WARN_HDMI("Audio Capability count over max number %d \n",HI_UNF_EDID_MAX_AUDIO_CAP_COUNT); //982
            break;
        }

        format_code = (pData[Index*3] & AUDIO_FORMAT_CODE)>>3;
        HI_INFO_HDMI("audio format:0x%02x, data:0x%02x\n", format_code,pData[Index*3]); //987
        pstAudioInfo[cur].u8AudChannel = (pData[Index*3] & AUDIO_MAX_CHANNEL) + 1;
        u8bit = pData[Index*3 + 1];
        HI_INFO_HDMI("sample rate:0x%02x\n",u8bit); //990
        if(u8bit&0x01)
        {
            pstAudioInfo[cur].enSupportSampleRate[u8Count] = HI_UNF_SAMPLE_RATE_32K;
            u8Count++;
        }
        if(u8bit&0x02)
        {
            pstAudioInfo[cur].enSupportSampleRate[u8Count] = HI_UNF_SAMPLE_RATE_44K;
            u8Count++;
        }
        if(u8bit&0x04)
        {
            pstAudioInfo[cur].enSupportSampleRate[u8Count] = HI_UNF_SAMPLE_RATE_48K;
            u8Count++;
        }
        if(u8bit&0x08)
        {
            pstAudioInfo[cur].enSupportSampleRate[u8Count] = HI_UNF_SAMPLE_RATE_88K;
            u8Count++;
        }
        if(u8bit&0x10)
        {
            pstAudioInfo[cur].enSupportSampleRate[u8Count] = HI_UNF_SAMPLE_RATE_96K;
            u8Count++;
        }
        if(u8bit&0x20)
        {
            pstAudioInfo[cur].enSupportSampleRate[u8Count] = HI_UNF_SAMPLE_RATE_176K;
            u8Count++;
        }
        if(u8bit&0x40)
        {
            pstAudioInfo[cur].enSupportSampleRate[u8Count] = HI_UNF_SAMPLE_RATE_192K;
            u8Count++;
        }
        HI_INFO_HDMI("sample rate num:%d\n",u8Count); //1026
        pstAudioInfo[cur].u32SupportSampleRateNum = u8Count;

        pstAudioInfo[cur].enAudFmtCode = (HI_UNF_EDID_AUDIO_FORMAT_CODE_E)format_code;
        if(format_code > HI_UNF_EDID_AUDIO_FORMAT_CODE_RESERVED && format_code < HI_UNF_EDID_AUDIO_FORMAT_CODE_BUTT)
        {
            pSinkCap->u32AudioInfoNum++;
        }

        if(1 == format_code)
        {
            u8Count = 0;
            u8bit = pData[Index*3 + 2];
            HI_INFO_HDMI("Bit Depth:0x%02x\n",u8bit); //1039
            if(u8bit&0x01)
            {
                pstAudioInfo[cur].bSupportBitDepth[u8Count] = HI_UNF_BIT_DEPTH_16;
                u8Count++;
            }
            if(u8bit&0x02)
            {
                pstAudioInfo[cur].bSupportBitDepth[u8Count] = HI_UNF_BIT_DEPTH_20;
                u8Count++;
            }
            if(u8bit&0x04)
            {
                pstAudioInfo[cur].bSupportBitDepth[u8Count] = HI_UNF_BIT_DEPTH_24;
                u8Count++;
            }
            HI_INFO_HDMI("Bit Depth num:%d\n",u8Count); //1055
            pstAudioInfo[cur].u32SupportBitDepthNum = u8Count;
        }
        else if ((format_code > 1) && (format_code < 9))
        {
            pstAudioInfo[cur].u32MaxBitRate = pData[Index*3 + 2]/8;
            HI_INFO_HDMI("Max Bit Rate:%d\n",pstAudioInfo[Index].u32MaxBitRate); //1061
        }
        else //9 -15 reserve
        {
            //g_stEdidInfo.stAudInfo[format_code].u32Reserve = pData[Index*3 + 2];
        }
    }
    return HI_SUCCESS;
}

static HI_S32 ParseVideoDataBlock(HI_U8 *pData, HI_U8 u8Len)
{
    HI_U8 Index;
    HI_UNF_ENC_FMT_E enFmt, enNativeFmt = HI_UNF_ENC_FMT_BUTT;
    HI_U32 vic,hdmiModeIdx;
    HI_UNF_EDID_BASE_INFO_S *pSinkCap = DRV_Get_SinkCap(HI_UNF_HDMI_ID_0);

    if(pData == HI_NULL)
    {
        HI_ERR_HDMI("hdmi video point null!\n"); //1081
        return HI_FAILURE;
    }

    for(Index = 0; Index < u8Len; Index++)
    {
        vic = pData[Index] & EDID_VIC;
        HI_INFO_HDMI("vic:0x%02x\n",vic); //1089
        for(hdmiModeIdx = 0; hdmiModeIdx < NMODES; hdmiModeIdx++)
        {
            if (VModeTables[hdmiModeIdx].ModeId.Mode_C1 == vic)
            {
                if(VModeTables[hdmiModeIdx].ModeId.enUNFFmt_C1 >= HI_UNF_ENC_FMT_BUTT)
                {
                    continue;
                }
                enFmt = VModeTables[hdmiModeIdx].ModeId.enUNFFmt_C1;
                pSinkCap->bSupportFormat[enFmt] = HI_TRUE;  /* support timing foramt */
                if(enNativeFmt == HI_UNF_ENC_FMT_BUTT)
                {
                    enNativeFmt = enFmt; //if no native flag set first define format
                }

                if(pData[Index]&0x80)
                {
                    if(HI_UNF_ENC_FMT_BUTT == pSinkCap->enNativeFormat)
                    {
                        pSinkCap->enNativeFormat = enFmt;
                    }
                    HI_INFO_HDMI("Native fmt :0x%02x\n",pData[Index]); //1111
                }
                g_s32VmodeOfUNFFormat[enFmt] = hdmiModeIdx;
            }
        }
    }

    if(HI_UNF_ENC_FMT_BUTT == pSinkCap->enNativeFormat)
    {
        pSinkCap->enNativeFormat = enNativeFmt; //set The first order fmt
    }
    HI_INFO_HDMI("*****Native fmt :0x%02x\n",pSinkCap->enNativeFormat); //1122

    //We need to add default value:640x480p@60Hz.
    pSinkCap->bSupportFormat[HI_UNF_ENC_FMT_861D_640X480_60] = HI_TRUE;
    g_s32VmodeOfUNFFormat[HI_UNF_ENC_FMT_861D_640X480_60] = 0x01;
    return HI_SUCCESS;
}

static HI_S32 ParseVendorSpecificDataBlock(HI_U8 *pData, HI_U8 u8Len )
{
    HI_U8 index ,HDMI_VIC_LEN, HDMI_3D_LEN,u83D_present, u83D_Multi_present,u83D_Structure;
    HI_U8 u8Temp;
    HI_UNF_EDID_BASE_INFO_S *pSinkCap = DRV_Get_SinkCap(HI_UNF_HDMI_ID_0);
    HI_U8 offset = 0;// for dynamic offset

    HI_INFO_HDMI("IEERegId:0x%02x, 0x%02x, 0x%02x\n", pData[0], pData[1], pData[2]); //1137
    if ((pData[0] == 0x03) && (pData[1] == 0x0c)
      && (pData[2] == 0x00))
    {
        HI_INFO_HDMI("This is HDMI Device\n"); //1141
        pSinkCap->bSupportHdmi = HI_TRUE;
    }
    else
    {
        //pSinkCap->bSupportHdmi = HI_FALSE;
        //HI_INFO_HDMI("This is DVI Device, we don't parse it\n");
        //return HI_FAILURE;
        HI_INFO_HDMI("unknown Device, we don't parse it\n"); //1149
        return HI_SUCCESS;
    }

    if(u8Len < 4)
    {
        HI_INFO_HDMI("len:%d\n",u8Len); //1155
        return HI_SUCCESS;
    }
        /* Vendor Specific Data Block */
    pSinkCap->stCECAddr.u8PhyAddrA = (pData[3] & 0xF0)>>4;
    pSinkCap->stCECAddr.u8PhyAddrB = (pData[3] & 0x0F);
    pSinkCap->stCECAddr.u8PhyAddrC = (pData[4] & 0xF0)>>4;
    pSinkCap->stCECAddr.u8PhyAddrD = (pData[4] & 0x0F);

    if((pSinkCap->stCECAddr.u8PhyAddrA != 0xF )&&(pSinkCap->stCECAddr.u8PhyAddrB != 0xF )&&
       (pSinkCap->stCECAddr.u8PhyAddrC != 0xF )&&(pSinkCap->stCECAddr.u8PhyAddrD != 0xF ))
    {
        pSinkCap->stCECAddr.bPhyAddrValid = TRUE;
    }
    else
    {
        pSinkCap->stCECAddr.bPhyAddrValid = FALSE;
    }

    //HI_INFO_HDMI("PhyAddr %02x.%02x.%02x.%02x\n", g_stEdidInfo.u8PhyAddrA, g_stEdidInfo.u8PhyAddrB, g_stEdidInfo.u8PhyAddrC, g_stEdidInfo.u8PhyAddrD);

    if(u8Len < 6)
    {
        HI_INFO_HDMI("len:%d\n",u8Len); //1178
        return HI_SUCCESS;
    }

    pSinkCap->bSupportDVIDual    = (pData[5] & 0x01);
    pSinkCap->stDeepColor.bDeepColorY444  = (pData[5] & 0x08) >> 3;
    pSinkCap->stDeepColor.bDeepColor30Bit = (pData[5] & 0x10) >> 4;
    pSinkCap->stDeepColor.bDeepColor36Bit = (pData[5] & 0x20) >> 5;
    pSinkCap->stDeepColor.bDeepColor48Bit = (pData[5] & 0x40) >> 6;
    pSinkCap->bSupportsAI = (pData[5] & 0x80) >> 7;
    HI_INFO_HDMI("DVI_Dual:%d,Y444:%d,30bit:%d,48bit:%d,AI:%d\n",pSinkCap->bSupportDVIDual,
                pSinkCap->stDeepColor.bDeepColorY444, pSinkCap->stDeepColor.bDeepColor30Bit,
                pSinkCap->stDeepColor.bDeepColor48Bit,pSinkCap->bSupportsAI); //1190
    if(u8Len < 7)
    {
        HI_INFO_HDMI("len:%d\n",u8Len); //1193
        return HI_SUCCESS;
    }
   // g_stEdidInfo.u32MaxTMDSClk   = (pData[6] & 0xff) * 5;
    HI_INFO_HDMI("Max TMDS Colock:%d\n", (pData[6] & 0xff) * 5); //1197

    if(u8Len < 8)
    {
        HI_INFO_HDMI("len:%d\n", u8Len); //1201
        return HI_SUCCESS;
    }

    //HDMI_Video_present is used as 3d support tag now. we don't use 3D_present to judge 3dsupport
    pSinkCap->st3DInfo.bSupport3D = (pData[7] & 0x20) >> 5;
    HI_INFO_HDMI("support 3d:%d\n",pSinkCap->st3DInfo.bSupport3D); //1207
    HI_INFO_HDMI("bLatency_Fields_Present:%d\n",(pData[7] & 0x80) >> 7);
    HI_INFO_HDMI("bI_Latency_Fields_Present:%d\n",(pData[7] & 0x40) >> 6); //1209
    HI_INFO_HDMI("CNC:%d\n",(pData[7] & 0x0F)); //1210
    if(u8Len < 9)
    {
        HI_INFO_HDMI("len:%d\n", u8Len); //1213
        return HI_SUCCESS;
    }

    offset = 8;
    if(((pData[7] & 0x80) >> 7) != 0)
    {
        HI_INFO_HDMI("u8Video_Latency:%d\n",(pData[offset++] & 0xff));//if valid,always in 8 //1220
        HI_INFO_HDMI("u8Audio_Latency:%d\n",(pData[offset++] & 0xff));// 9 //1221

    }

    if(((pData[7] & 0x40) >> 6) != 0)
    {
        HI_INFO_HDMI("u8Interlaced_Video_Latency:%d\n",(pData[offset++] & 0xff)); //1227
        HI_INFO_HDMI("u8Interlaced_Audio_Latency:%d\n",(pData[offset++] & 0xff)); //1228
    }

    if(u8Len < (offset+1))
    {
        HI_INFO_HDMI("len:%d\n", u8Len); //1233
        return HI_SUCCESS;
    }

    HI_INFO_HDMI("u8ImagSize:%d\n",((pData[offset]& 0x18)>>2)); //1237
    u83D_present = (pData[offset]& 0x80) >> 7;
    u83D_Multi_present = (pData[offset]& 0x60) >> 5;

    HI_INFO_HDMI("3D present:%d ,u83D_Multi_present:%d \n",u83D_present,u83D_Multi_present); //1241
    offset++;

    if(u8Len < (offset+1))
    {
        HI_INFO_HDMI("len:%d\n", u8Len); //1246
        return HI_SUCCESS;
    }

    HDMI_3D_LEN = (pData[offset]& 0x1F);
    HDMI_VIC_LEN = (pData[offset]& 0xE0) >> 5;
    offset++; // 14

    index = 0;

    HI_INFO_HDMI("3D_LEN:%d,VIC:%d\n",HDMI_3D_LEN, HDMI_VIC_LEN); //1256
    if ((HDMI_VIC_LEN > 0) && (u8Len >= (HDMI_VIC_LEN + offset)))
    {
        for(index = 0; index < HDMI_VIC_LEN; index ++)
        {
            //g_stEdidInfo.bSupportFmt4K_2K[index] = HI_TRUE;
            u8Temp = pData[offset++];
            if(u8Temp <= 4)
            {
                HI_U32 u32temp = VICModeTables[u8Temp].fmt;
                if(u32temp < HI_UNF_ENC_FMT_BUTT)
                {
                    pSinkCap->bSupportFormat[u32temp] = HI_TRUE;
                }
                else
                {
                    HI_WARN_HDMI("Getted Butt fmt when parse 4k cap \n"); //1272
                }
            }
            HI_INFO_HDMI("4k*2k VIC:%d\n",u8Temp); //1275
        }
    }

    if((HDMI_3D_LEN > 0) && (u8Len >= (HDMI_3D_LEN + offset)))
    {
        //index = 0;
        if(u83D_present)
        {
            if ((u83D_Multi_present == 0x1) || (u83D_Multi_present == 0x2))
            {

                //3d structure_All_15...8 resever
                u8Temp = pData[offset++] & 0xFF;
                HI_INFO_HDMI("3D_Structure_ALL_15¡­8 :0x%x\n",u8Temp); //1289
                if(0x01 == u8Temp)
                {
                   pSinkCap->st3DInfo.bSupport3DType[HI_UNF_EDID_3D_SIDE_BY_SIDE_HALF] = HI_TRUE;
                }
                u8Temp = pData[offset++] & 0xFF;
                HI_INFO_HDMI("3D_Structure_ALL_7¡­0 :0x%x\n",u8Temp); //1295


                if(0x01&u8Temp)
                {
                   pSinkCap->st3DInfo.bSupport3DType[HI_UNF_EDID_3D_FRAME_PACKETING] = HI_TRUE;
                }
                if(0x02&u8Temp)
                {
                   pSinkCap->st3DInfo.bSupport3DType[HI_UNF_EDID_3D_FIELD_ALTERNATIVE] = HI_TRUE;
                }
                if(0x04&u8Temp)
                {
                   pSinkCap->st3DInfo.bSupport3DType[HI_UNF_EDID_3D_LINE_ALTERNATIVE] = HI_TRUE;
                }
                if(0x08&u8Temp)
                {
                   pSinkCap->st3DInfo.bSupport3DType[HI_UNF_EDID_3D_SIDE_BY_SIDE_FULL] = HI_TRUE;
                }
                if(0x10&u8Temp)
                {
                   pSinkCap->st3DInfo.bSupport3DType[HI_UNF_EDID_3D_L_DEPTH] = HI_TRUE;
                }
                if(0x20&u8Temp)
                {
                   pSinkCap->st3DInfo.bSupport3DType[HI_UNF_EDID_3D_L_DEPTH_GRAPHICS_GRAPHICS_DEPTH] = HI_TRUE;
                }
                if(0x40&u8Temp)
                {
                   pSinkCap->st3DInfo.bSupport3DType[HI_UNF_EDID_3D_TOP_AND_BOTTOM] = HI_TRUE;
                }
                //index += 2;
            }

            if(u83D_Multi_present == 0x1)
            {
                //3D_MASK_0...15 resever
                u8Temp = pData[offset++] & 0xFF;
                HI_INFO_HDMI("3D_MASK_15¡­8 :0x%x\n",u8Temp); //1333

                u8Temp = pData[offset++] & 0xFF;
                HI_INFO_HDMI("3D_MASK_7¡­0 :0x%x\n",u8Temp); //1336
            }
        }
    }

    for(index = offset ; index < u8Len; index ++)
    {
        u83D_Structure = (pData[offset]&0xF0)>>4;
        HI_INFO_HDMI("2D_VIC_order:0x%02x\n", u83D_Structure); //1344
        u83D_Structure = (pData[offset]&0x0F);
        if(u83D_Structure >= 0x08)
        {
            HI_INFO_HDMI("3D_Detailed > 0x08 :0x%02x\n", u83D_Structure); //1348
            u83D_Structure = (pData[offset]&0xF0)>>4;
        }

        offset++;
        HI_INFO_HDMI("3D_Detailed :0x%02x\n", u83D_Structure); //1353
    }


    return HI_SUCCESS;
}

HI_S32 ParseSpeakerDataBlock(HI_U8 *pData, HI_U8 u8Len)
{
    HI_U8 u32Bit = pData[0];

    HI_UNF_EDID_BASE_INFO_S *pSinkCap = DRV_Get_SinkCap(HI_UNF_HDMI_ID_0);

    HI_INFO_HDMI("Speaker0:0x%02x\n",u32Bit); //1365
    if(u32Bit & 0x01)
    {
        pSinkCap->bSupportAudioSpeaker[HI_UNF_EDID_AUDIO_SPEAKER_FL_FR] = HI_TRUE;
    }
    if(u32Bit & 0x02)
    {
        pSinkCap->bSupportAudioSpeaker[HI_UNF_EDID_AUDIO_SPEAKER_LFE] = HI_TRUE;
    }
    if(u32Bit & 0x04)
    {
        pSinkCap->bSupportAudioSpeaker[HI_UNF_EDID_AUDIO_SPEAKER_FC] = HI_TRUE;
    }
    if(u32Bit & 0x08)
    {
        pSinkCap->bSupportAudioSpeaker[HI_UNF_EDID_AUDIO_SPEAKER_RL_RR] = HI_TRUE;
    }
    if(u32Bit & 0x10)
    {
        pSinkCap->bSupportAudioSpeaker[HI_UNF_EDID_AUDIO_SPEAKER_RC] = HI_TRUE;
    }
    if(u32Bit & 0x20)
    {
        pSinkCap->bSupportAudioSpeaker[HI_UNF_EDID_AUDIO_SPEAKER_FLC_FRC] = HI_TRUE;
    }
    if(u32Bit & 0x40)
    {
        pSinkCap->bSupportAudioSpeaker[HI_UNF_EDID_AUDIO_SPEAKER_FLW_FRW] = HI_TRUE;
    }
    if(u32Bit & 0x80)
    {
        pSinkCap->bSupportAudioSpeaker[HI_UNF_EDID_AUDIO_SPEAKER_FLH_FRH] = HI_TRUE;
    }
    u32Bit = pData[1];
    HI_INFO_HDMI("Speaker1:0x%02x\n",u32Bit); //1399
    if(u32Bit & 0x01)
    {
        pSinkCap->bSupportAudioSpeaker[HI_UNF_EDID_AUDIO_SPEAKER_TC] = HI_TRUE;
    }
    if(u32Bit & 0x02)
    {
        pSinkCap->bSupportAudioSpeaker[HI_UNF_EDID_AUDIO_SPEAKER_FCH] = HI_TRUE;
    }

    return HI_SUCCESS;
}

HI_S32 ParseExtDataBlock(HI_U8 *pData, HI_U8 u8Len)
{
    HI_UNF_EDID_BASE_INFO_S *pSinkCap = DRV_Get_SinkCap(HI_UNF_HDMI_ID_0);

    switch(pData[0])
    {
    case VIDEO_CAPABILITY_DATA_BLOCK:
        HI_INFO_HDMI("Video Capability Data Block\n"); //1419
        //g_stEdidInfo.unScanInfo.u32ScanInfo |= pData[1]&0xFF;
        HI_INFO_HDMI("ScanType:%d\n",pData[1]); //1421
        break;
    case VENDOR_SPECIFIC_VIDEO_DATA_BLOCK:
        HI_INFO_HDMI("vendor specific data block\n"); //1424
        break;
    case RESERVED_VESA_DISPLAY_DEVICE:
        HI_INFO_HDMI("reserved vesa display device\n");
        break;
    case RESERVED_VESA_VIDEO_DATA_BLOCK:
        HI_INFO_HDMI("reserved vesa video data block\n"); //1430
        break;
    case RESERVED_HDMI_VIDEO_DATA_BLOCK:
        HI_INFO_HDMI("reserved hdmi video data block\n"); //1433
        break;
    case COLORIMETRY_DATA_BLOCK:
       // g_stEdidInfo.unColorimetry.u32Colorimetry = (pData[1]|(pData[2]<<8));
        if(XVYCC601&pData[1])
        {
            pSinkCap->stColorMetry.bxvYCC601 = HI_TRUE;
        }
        if(XVYCC709&pData[1])
        {
            pSinkCap->stColorMetry.bxvYCC709 = HI_TRUE;
        }
        if(SYCC601&pData[1])
        {
            pSinkCap->stColorMetry.bsYCC601 = HI_TRUE;
        }
        if(ADOBE_XYCC601&pData[1])
        {
            pSinkCap->stColorMetry.bAdobleYCC601 = HI_TRUE;
        }
        if(ADOBE_RGB&pData[1])
        {
            pSinkCap->stColorMetry.bAdobleRGB = HI_TRUE;
        }
        HI_INFO_HDMI("Colorimetry:0x%02x\n", pData[1]); //1457
        break;
    case CEA_MISCELLANENOUS_AUDIO_FIELDS:
        HI_INFO_HDMI("CEA miscellanenous audio data fileds\n"); //1460
        break;
    case VENDOR_SPECIFIC_AUDIO_DATA_BLOCK:
        HI_INFO_HDMI("vendor specific audio data block\n"); //1463
        break;
    case RESERVED_HDMI_AUDIO_DATA_BLOCK:
        HI_INFO_HDMI("reserved hdmi audio data block\n"); //1466
        break;
     default:
        break;
    }
    return HI_SUCCESS;
}

HI_S32 ParseDTVBlock(HI_U8 *pData)
{
    HI_U8 offset, length, len;
    HI_S32 s32Ret;

    if(pData == HI_NULL)
    {
        HI_ERR_HDMI("null ponit\n"); //1481
        return HI_FAILURE;
    }

    HI_INFO_HDMI("checksum\n"); //1485
    s32Ret = BlockCheckSum(pData);

    if(s32Ret != HI_SUCCESS)
    {
        HI_WARN_HDMI("Extend Block Crc Error! \n"); //1490
        g_bErrList[EXTENSION_BLOCK_CRC_ERROR] = HI_TRUE;
    }


    if(pData[0] != EXT_VER_TAG)//0x02
    {
        HI_ERR_HDMI("extern block version err:0x%02x\n",pData[0]); //1497
        return HI_FAILURE;
    }
    if (pData[1] < EXT_REVISION)//0x03
    {
        HI_ERR_HDMI("extern block revison err: 0x%02x\n", pData[1]); //1502
        return HI_FAILURE;
    }

    length = pData[2];
    HI_INFO_HDMI("data block length:0x%x\n",length); //1507
    if(length == 0)
    {
        HI_ERR_HDMI("no detailed timing data and no reserved provided!\n"); //1510
        return HI_FAILURE;
    }
    if(length <= 4)
    {
        HI_ERR_HDMI("no reserved provided! len:%d\n",length); //1515
        return HI_FAILURE;
    }
    offset = 4;

    ParseDTVMonitorAttr(&pData[3]);

    while(offset < length)
    {
        len = pData[offset] & DATA_BLOCK_LENGTH;
        HI_INFO_HDMI("data ID:%d,len:%d,data:0x%02x\n",((pData[offset] & DATA_BLOCK_TAG_CODE) >> 5),len,pData[offset]); //1525
        switch((pData[offset] & DATA_BLOCK_TAG_CODE) >> 5)
        {
            case AUDIO_DATA_BLOCK:
                ParseAudioBlock(&pData[offset + 1], len);
                break;
            case VIDEO_DATA_BLOCK:
                ParseVideoDataBlock(&pData[offset + 1], len);
                break;
            case VENDOR_DATA_BLOCK:
                s32Ret = ParseVendorSpecificDataBlock(&pData[offset + 1], len);
                if(HI_SUCCESS != s32Ret)
                {
                    HI_ERR_HDMI("Vendor parase error!\n");
                    //return s32Ret;
                }
                break;
            case SPEAKER_DATA_BLOCK:
                ParseSpeakerDataBlock(&pData[offset + 1], len);
                break;
            case VESA_DTC_DATA_BLOCK:
                HI_INFO_HDMI("VESA_DTC parase\n"); //1546
                break;
            case USE_EXT_DATA_BLOCK:
                ParseExtDataBlock(&pData[offset + 1], len);
                break;
             default:
                HI_INFO_HDMI("resvered block tag code define"); //1552
                break;
        }
        offset += len + 1;
    }

    return HI_SUCCESS;

}

static HI_BOOL g_bDefHDMIMode = HI_TRUE;
HI_U8 SI_SetDefaultOutputMode(HI_BOOL bDefHDMIMode)
{
    g_bDefHDMIMode = bDefHDMIMode;

    return HI_SUCCESS;
}

HI_U8 SI_GetDefaultOutputMode(HI_BOOL *pDefHDMIMode)
{
    *pDefHDMIMode = g_bDefHDMIMode;
    return HI_SUCCESS;
}

void SI_OldAudioAdjust(void)
{
    HI_DRV_HDMI_AUDIO_CAPABILITY_S *tempAudioCap = DRV_Get_OldAudioCap();
    HI_UNF_EDID_BASE_INFO_S *pSinkCap = DRV_Get_SinkCap(HI_UNF_HDMI_ID_0);
    HI_U32 i,j;

    memset(tempAudioCap,0,sizeof(HI_DRV_HDMI_AUDIO_CAPABILITY_S));

    for(i = 0; i < HI_UNF_EDID_MAX_AUDIO_CAP_COUNT; i++)
    {
        if(pSinkCap->stAudioInfo[i].enAudFmtCode)
        {
            tempAudioCap->bAudioFmtSupported[pSinkCap->stAudioInfo[i].enAudFmtCode] = HI_TRUE;

            //p += PROC_PRINT(p, "%-3d %-12s| ",pSinkCap->stAudioInfo[i].enAudFmtCode, g_pAudioFmtCode[pSinkCap->stAudioInfo[i].enAudFmtCode]);
            //p += PROC_PRINT(p, "%-15d| ",pSinkCap->stAudioInfo[i].u8AudChannel);
            if((pSinkCap->stAudioInfo[i].enAudFmtCode == HI_UNF_EDID_AUDIO_FORMAT_CODE_PCM)
                && (pSinkCap->stAudioInfo[i].u8AudChannel > tempAudioCap->u32MaxPcmChannels))
            {
                tempAudioCap->u32MaxPcmChannels = pSinkCap->stAudioInfo[i].u8AudChannel;
            }

            for (j = 0; j < MAX_SAMPE_RATE_NUM; j++)
            {
                if(pSinkCap->stAudioInfo[i].enSupportSampleRate[j] == HI_UNF_SAMPLE_RATE_32K)
                {
                    tempAudioCap->u32AudioSampleRateSupported[0] = HI_UNF_SAMPLE_RATE_32K;
                }
                if(pSinkCap->stAudioInfo[i].enSupportSampleRate[j] == HI_UNF_SAMPLE_RATE_44K)
                {
                    tempAudioCap->u32AudioSampleRateSupported[1] = HI_UNF_SAMPLE_RATE_44K;
                }
                if(pSinkCap->stAudioInfo[i].enSupportSampleRate[j] == HI_UNF_SAMPLE_RATE_48K)
                {
                    tempAudioCap->u32AudioSampleRateSupported[2] = HI_UNF_SAMPLE_RATE_48K;
                }
                if(pSinkCap->stAudioInfo[i].enSupportSampleRate[j] == HI_UNF_SAMPLE_RATE_88K)
                {
                    tempAudioCap->u32AudioSampleRateSupported[3] = HI_UNF_SAMPLE_RATE_88K;
                }
                if(pSinkCap->stAudioInfo[i].enSupportSampleRate[j] == HI_UNF_SAMPLE_RATE_96K)
                {
                    tempAudioCap->u32AudioSampleRateSupported[4] = HI_UNF_SAMPLE_RATE_96K;
                }
                if(pSinkCap->stAudioInfo[i].enSupportSampleRate[j] == HI_UNF_SAMPLE_RATE_176K)
                {
                    tempAudioCap->u32AudioSampleRateSupported[5] = HI_UNF_SAMPLE_RATE_176K;
                }
                if(pSinkCap->stAudioInfo[i].enSupportSampleRate[j] == HI_UNF_SAMPLE_RATE_192K)
                {
                    tempAudioCap->u32AudioSampleRateSupported[6] = HI_UNF_SAMPLE_RATE_192K;
                }
            }
        }
    }
}

// when we found 0x03 0x0c 0x00 in edid, hdmi mode
// 0 extend block, dvi mode
// no vsdb flag(030c00) && crc ok && 1st head ok && no pcm capability, dvi mode
// else user decision default mode
HI_U8 SI_ParseEDID(HI_U8 *DisplayType)
{
    HI_S32 s32Ret,Index;
    HI_U8  *pData = g_EdidMen; // Parse sink cap
    HI_UNF_EDID_BASE_INFO_S *pSinkCap = DRV_Get_SinkCap(HI_UNF_HDMI_ID_0);
    HI_DRV_HDMI_AUDIO_CAPABILITY_S *pOldAudioCap;

#warning TODO
    ParseFirstBlock(pData);

    if(pSinkCap->u8ExtBlockNum == 0)
    {
        pSinkCap->bSupportHdmi = HI_FALSE;
        DRV_Set_SinkCapValid(HI_UNF_HDMI_ID_0,HI_TRUE);
        return HI_SUCCESS;
    }

    if(pSinkCap->u8ExtBlockNum > 3)
    {
        HI_INFO_HDMI("Extern block too big: 0x%02x \n",pSinkCap->u8ExtBlockNum); //1655
        HI_INFO_HDMI("Setting to 3 Block \n"); //1656
        pSinkCap->u8ExtBlockNum = 3;
    }

    //g_stEdidInfo.bRealEDID = HI_TRUE;
    //g_stEdidInfo.bRGB444 = HI_TRUE;//always HI_TRUE
    for(Index = 1; Index <= pSinkCap->u8ExtBlockNum; Index++)
    {
        s32Ret = ParseDTVBlock(&pData[EDID_BLOCK_SIZE*Index]);
        if(HI_SUCCESS == s32Ret)
        {
            HI_INFO_HDMI("Successfully resolved ext block NO:0x%x\n",Index); //1667
            break;
        }
    }

    SI_OldAudioAdjust();

    pOldAudioCap = DRV_Get_OldAudioCap();

    if(pSinkCap->bSupportHdmi == HI_FALSE)
    {
        if(g_bErrList[BAD_HEADER] || g_bErrList[_1ST_BLOCK_CRC_ERROR] || g_bErrList[EXTENSION_BLOCK_CRC_ERROR]
				|| pOldAudioCap->bAudioFmtSupported[HI_UNF_EDID_AUDIO_FORMAT_CODE_PCM])
        {
            HI_INFO_HDMI("Unknown mode, Go to Force Mode"); //1681
            HI_INFO_HDMI("BAD_HEADER:%d,_1ST_BLOCK_CRC_ERROR:%d,EXTENSION_BLOCK_CRC_ERROR:%d,PCM:%d\n",
                g_bErrList[BAD_HEADER],g_bErrList[_1ST_BLOCK_CRC_ERROR],g_bErrList[EXTENSION_BLOCK_CRC_ERROR],pOldAudioCap->bAudioFmtSupported[HI_UNF_EDID_AUDIO_FORMAT_CODE_PCM]); //1683

            return HI_FAILURE;
        }
        HI_INFO_HDMI("DVI DEVICE\n"); //1687
        *DisplayType = DVI_DISPLAY;
    }
    else
    {
        HI_INFO_HDMI("HDMI DEVICE\n"); //1692
        *DisplayType = HDMI_DISPLAY;
    }

    DRV_Set_SinkCapValid(HI_UNF_HDMI_ID_0,HI_TRUE);

    return HI_SUCCESS;
}

#if 0
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
#endif

HI_U8 SI_Proc_ReadEDIDBlock(HI_U8 *DataBlock, HI_U32 size)
{
    if (size > 512)
    {
        size = 512;
    }

    memcpy(DataBlock, EDIDData, size);
    return HI_SUCCESS;
}

//This function is used in enjoy project.
HI_U8 SI_Force_GetEDID(HI_U8 *datablock, HI_U32 *length)
{
    HI_U32 ret;
    HI_U32 BlockNum = 0;
    HI_U8  DisplayType = 0;

    ret = SI_ReadSinkEDID();
    if(ret != HI_SUCCESS)
    {
        HI_ERR_HDMI("ReadSinkEDID fail!\n"); //1722
        return ret;
    }

    SI_Proc_ReadEDIDBlock(datablock, EDID_SIZE);
    BlockNum = GetExtBlockNum(g_EdidMen[EXT_BLOCK_ADDR]);

    if(BlockNum > 3)
    {
        BlockNum = 3;
    }

    *length = 128 + 128 * BlockNum;

    if(!SI_ParseEDID(&DisplayType))
    {
        HI_INFO_HDMI("finish SI_EDIDProcessing\n"); //1738
    }
    else
    {
        HI_ERR_HDMI("edid parse error! \b"); //1742
    }

    return 0;
}

#if 0
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
#endif

HI_U8 Transfer_VideoTimingFromat_to_VModeTablesIndex(HI_UNF_ENC_FMT_E unfFmt)
{

    if (HI_UNF_ENC_FMT_BUTT <= unfFmt)   
        return 0; /* 640x480 format index in VModeTables*/
    else
        return(g_s32VmodeOfUNFFormat[unfFmt]);	
}

#if 0
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

#if 0 //TODO
    SI_DDC_Adjust();
    /* Clear EDID first before write.EEPROM must clear too. */
    SI_ResetEDIDBlock();
#endif
   
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
#if 0 //TODO
        SI_Proc_writeEDIDBlock(index, ucData, ByteperTime);
#endif
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

#if 0
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
#endif

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

#endif

#if 0
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
#endif

#if 0
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
#endif

HI_S32 SI_GetHdmiSinkCaps(HI_UNF_EDID_BASE_INFO_S *pCapability)
{
    HI_UNF_EDID_BASE_INFO_S *pSinkCap = DRV_Get_SinkCap(HI_UNF_HDMI_ID_0);

    if(DRV_Get_IsValidSinkCap(HI_UNF_HDMI_ID_0))
    {
        memcpy(pCapability,pSinkCap,sizeof(HI_UNF_EDID_BASE_INFO_S));
        return HI_SUCCESS;
    }
    else
    {
        return HI_FAILURE;
    }
}

