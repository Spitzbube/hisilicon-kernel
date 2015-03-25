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

HI_S32 SI_PrepareEDID(HI_BOOL bForceMode);
HI_U8 SI_ParseEDID(EDIDParsedDataType *);
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

HI_U8 SI_EDID_Force_Setting(void);
HI_U8 SI_Set_Force_OutputMode(HI_BOOL bForce, HI_BOOL bHDMIMode);
HI_U8 SI_Get_Force_OutputMode(HI_BOOL *bForce, HI_BOOL *bHDMIMode);
HI_U32 SI_Set_DEBUG_Flag(HI_U32 DebugFlag);
HI_U8 SI_ResetEDIDBlock(void);
HI_U8 SI_Proc_ReadEDIDBlock(HI_U8 *DataBlock, HI_U32 size);
HI_U8 SI_Force_GetEDID(HI_U8 *datablock, HI_U32 *length);
#endif

