/***********************************************************************************/
/*  Copyright (c) 2002-2006, Silicon Image, Inc.  All rights reserved.             */
/*  No part of this work may be reproduced, modified, distributed, transmitted,    */
/*  transcribed, or translated into any language or computer format, in any form   */
/*  or by any means without written permission of: Silicon Image, Inc.,            */
/*  1060 East Arques Avenue, Sunnyvale, California 94085                           */
/***********************************************************************************/
#ifndef __EDID_H_2010_0517__
#define __EDID_H_2010_0517__

#include "si_typedefs.h"
#include "si_hlviic.h"
#include "si_globdefs.h"

#define EDID_SLV 0xA0
#define OFFSET_SLV 0x60

typedef struct {
 HI_U8 ErrorCode;
 HI_U8 DisplayType;
 HI_U16 CRC16;
} EDIDParsedDataType;

typedef struct
{
	HI_U8 pixel_clk[2]; //0
	HI_U8 h_active; //2
	HI_U8 h_blank; //3
	HI_U8 h_active_blank; //4
	HI_U8 v_active; //5
	HI_U8 v_blank; //6
	HI_U8 v_active_blank; //7
	HI_U8 h_sync_offset; //8
	HI_U8 h_sync_pulse_width; //9
	HI_U8 vs_offset_pulse_width; //10
	HI_U8 hs_offset_vs_offset; //11
	HI_U8 h_image_size; //12
	HI_U8 v_image_size; //13
	HI_U8 h_v_image_size; //14
	HI_U8 h_border; //15
	HI_U8 v_border; //16
	HI_U8 flags; //17

} DETAILED_TIMING_BLOCK;

typedef struct
{
	HI_U8 fill_0[8];
	HI_U8 mfg_id[2]; //8
	HI_U8 prod_code[2]; //10
	HI_U8 serial[4]; //12
	HI_U8 mfg_week; //16
	HI_U8 mfg_year; //17
	HI_U8 version; //18
	HI_U8 revision; //19
	char fill_20[15]; //20
	HI_U8 est_timing[3]; //35
	HI_U8 std_timing[16]; //38
#define FIRST_DETAILED_TIMING_ADDR 54
} EDID_FIRST_BLOCK_INFO;

HI_S32 SI_PrepareEDID(HI_BOOL bForceMode);
#if 0
HI_U8 SI_ParseEDID(EDIDParsedDataType *);
#else
HI_U8 SI_ParseEDID(HI_U8 *DisplayType);
#endif
HI_U8 ShortParseEDID(EDIDParsedDataType *);

extern int g_s32VmodeOfUNFFormat[];
HI_U8 Transfer_VideoTimingFromat_to_VModeTablesIndex(HI_UNF_ENC_FMT_E unfFmt);

// EDID Addresses

#define VER_ADDR 0x12
#define NUM_EXTENSIONS_ADDR 0x7E

#define EXTENSION_ADDR 0x80
#define CEA_DATA_BLOCK_COLLECTION_ADDR 0x84

#define EXTENSION_ADDR_1StP 0x00
#define CEA_DATA_BLOCK_COLLECTION_ADDR_1StP 0x04


#define TAG_AUDIO_DATA_BLOCK   0x20
#define TAG_VIDEO_DATA_BLOCK   0x40
#define TAG_VENDOR_DATA_BLOCK  0x60
#define TAG_SPEAKER_DATA_BLOCK  0x80

// Codes of EDID Errors

#define NO_ERR 0
// 1-7 reserved for I2C Errors
#define BAD_HEADER 8
#define VER_DONT_SUPPORT_861B 9
#define _1ST_BLOCK_CRC_ERROR 10
#define _2ND_BLOCK_CRC_ERROR 11
#define EXTENSION_BLOCK_CRC_ERROR 11
#define NO_861B_EXTENSION 12
#define NO_HDMI_SIGNATURE 13
#define BLOCK_MAP_ERROR 14
#define CRC_CEA861EXTENSION_ERROR 15
//#define NO_HDMI_SIGNATURE 16

// DisplayType

#define UNKNOWN_DISPLAY 0
#define DVI_DISPLAY 1
#define HDMI_DISPLAY 2

#define VIDEO_TAG 0x40
#define AUDIO_TAG 0x20
#define VENDOR_TAG 0x60
#define SPEAKER_TAG 0x80

#define USE_EXTENDED_TAG                              0xe0
#define USE_EXTENDED_VIDEO_CAPABILITY_DATA_BLOCK      0x00
#define USE_EXTENDED_VENDOR_SPECIFIC_VIDEO_DATA_BLOCK 0x01
#define USE_EXTENDED_COLORIMETRY_DATA_BLOCK           0x05
#define USE_EXTENDED_CEA_MIS_AUDIO_FIELD_DATA_BLOCK   0x10
#define USE_EXTENDED_VENDOR_SPECIFIC_AUIDO_DATA_BLOCK 0x11

#define BLOCK_HEADER	0x02
#define VERSION_THREE	0x03
#define THREE_MSB		0xE0
#define FIVE_LSB		0x1F
#define SVDB_TAG		0x03

#define DDC_ADDR 0xA0
#define EDID_SIZE 512
#define EDID_BLOCK_SIZE 128
#define EXT_BLOCK_ADDR 126
#define EDID_MAX_ERR_NUM 16
#define EDID_READ_FIRST_BLOCK_ERR 1
#define STANDARDTIMING_SIZE 12

#define VIDEO_CAPABILITY_DATA_BLOCK 0
#define VENDOR_SPECIFIC_VIDEO_DATA_BLOCK 1
#define RESERVED_VESA_DISPLAY_DEVICE 2
#define RESERVED_VESA_VIDEO_DATA_BLOCK 3
#define RESERVED_HDMI_VIDEO_DATA_BLOCK 4
#define COLORIMETRY_DATA_BLOCK  5
#define CEA_MISCELLANENOUS_AUDIO_FIELDS 16
#define VENDOR_SPECIFIC_AUDIO_DATA_BLOCK 17
#define RESERVED_HDMI_AUDIO_DATA_BLOCK 18

#define XVYCC601      1
#define XVYCC709      2
#define SYCC601       4
#define ADOBE_XYCC601 8
#define ADOBE_RGB     16

#define EXT_VER_TAG 0x02
#define EXT_REVISION 0x02

#define AUDIO_FORMAT_CODE (0xf << 3)
#define AUDIO_MAX_CHANNEL 0x07

#define EDID_VIC 0x7f

#define DATA_BLOCK_LENGTH 31
#define DATA_BLOCK_TAG_CODE 0xff
#define AUDIO_DATA_BLOCK 1
#define VIDEO_DATA_BLOCK 2
#define VENDOR_DATA_BLOCK 3
#define SPEAKER_DATA_BLOCK 4
#define VESA_DTC_DATA_BLOCK 5
#define USE_EXT_DATA_BLOCK 7

#define BIT4 (1 << 4)
#define BIT5 (1 << 5)
#define BIT6 (1 << 6)
#define BIT7 (1 << 7)


HI_U8 SI_EDID_Force_Setting(void);
HI_U8 SI_Set_Force_OutputMode(HI_BOOL bForce, HI_BOOL bHDMIMode);
HI_U8 SI_Get_Force_OutputMode(HI_BOOL *bForce, HI_BOOL *bHDMIMode);
HI_U32 SI_Set_DEBUG_Flag(HI_U32 DebugFlag);
HI_U8 SI_ResetEDIDBlock(void);
HI_U8 SI_Proc_ReadEDIDBlock(HI_U8 *DataBlock, HI_U32 size);
HI_U8 SI_Force_GetEDID(HI_U8 *datablock, HI_U32 *length);
#endif

