//------------------------------------------------------------------------------
// Copyright ?2002-2005, Silicon Image, Inc.  All rights reserved.
//
// No part of this work may be reproduced, modified, distributed, transmitted,
// transcribed, or translated into any language or computer format, in any form
// or by any means without written permission of: Silicon Image, Inc.,
// 1060 East Arques Avenue, Sunnyvale, California 94085
//------------------------------------------------------------------------------

// Turn down the warning level for this C file to prevent the compiler 
// from complaining about the unused parameters in the stub functions below

#include <linux/delay.h>
#include <linux/sched.h>
#include <si_typedefs.h>
#include <si_cec_reg.h>
#include <si_cec.h>  
#include <si_amf.h>  
#include <si_config.h>  
#include "si_hdmitx.h"
#include "drv_hdmi.h"

#if defined (CEC_SUPPORT)
enum siCECMessage_e
{
    SI_CEC_MESSAGE_RETURN_OK_WITH_RESPONES = 0X00,
    SI_CEC_MESSAGE_RETURN_OK_WITHOUT_RESPONES,
    SI_CEC_MESSAGE_RETURN_ERROR_NOSUPPORT,
    SI_CEC_MESSAGE_RETURN_BUTT
};


static SiI_CEC_LogAddr_t CurLogAddr = SiI_CEC_LogAddr_STB1;
//------------------------------------------------------------------------------
// Function Name:
// Function Description:
//------------------------------------------------------------------------------
void cec_PrintLogAddr ( HI_U8 bLogAddr )
{

    if ( bLogAddr <= SiI_CEC_LogAddr_Unregister )
    {
        HI_CEC_HDMI (" [%02X] ", (int) bLogAddr );
        switch ( bLogAddr )
        {
            case SiI_CEC_LogAddr_TV:        
                HI_CEC_HDMI("TV");           break;
            case SiI_CEC_LogAddr_RecDev1:   
                HI_CEC_HDMI("RecDev1");      break;
            case SiI_CEC_LogAddr_RecDev2:   
                HI_CEC_HDMI("RecDev2");      break;
            case SiI_CEC_LogAddr_STB1:      
                HI_CEC_HDMI("STB1");         break;
            case SiI_CEC_LogAddr_DVD1:      
                HI_CEC_HDMI("DVD1");         break;
            case SiI_CEC_LogAddr_AudSys:    
                HI_CEC_HDMI("AudSys");       break;
            case SiI_CEC_LogAddr_STB2:      
                HI_CEC_HDMI("STB2");         break;
            case SiI_CEC_LogAddr_STB3:      
                HI_CEC_HDMI("STB3");         break;
            case SiI_CEC_LogAddr_DVD2:      
                HI_CEC_HDMI("DVD2");         break;
            case SiI_CEC_LogAddr_RecDev3:   
                HI_CEC_HDMI("RecDev3");      break;
            case SiI_CEC_LogAddr_Res1:      
                HI_CEC_HDMI("Res1");         break;
            case SiI_CEC_LogAddr_Res2:      
                HI_CEC_HDMI("Res2");         break;
            case SiI_CEC_LogAddr_Res3:      
                HI_CEC_HDMI("Res3");         break;
            case SiI_CEC_LogAddr_Res4:      
                HI_CEC_HDMI("Res4");         break;
            case SiI_CEC_LogAddr_FreeUse:   
                HI_CEC_HDMI("FreeUse");      break;
            case SiI_CEC_LogAddr_Unregister: 
                HI_CEC_HDMI("BrocastMSG");    break;
        }
    }
}
//------------------------------------------------------------------------------
// Function Name: PrintCommand
// Function Description:
//------------------------------------------------------------------------------
static void PrintCommand( SiI_CEC_t * SiI_CEC )
{

    HI_U8 i;
    HI_CEC_HDMI ("\n [FROM][TO][OPCODE]  (OperandLegth){OPERANDS}: ");
    cec_PrintLogAddr( (SiI_CEC->bDestOrRXHeader & 0xF0) >> 4 );
    cec_PrintLogAddr( SiI_CEC->bDestOrRXHeader & 0x0F );
    HI_CEC_HDMI (" [%02X]", (int) SiI_CEC->bOpcode);
        
    HI_CEC_HDMI ("   (%02X)", (int)(SiI_CEC->bCount & 0x0F));
    if (SiI_CEC->bCount & 0x0F)
    {
        for ( i = 0; i < ( SiI_CEC->bCount & 0x0F); i++ )
            HI_CEC_HDMI (" {%02X}", (int) SiI_CEC->bOperand[i]);
    }
    HI_CEC_HDMI ("\n");
}

#if 0
static void CEC_HDLR_ff_Abort( SiI_CEC_t * sii_cec )
{
    SiI_CEC_t cec_frame;
    //HI_CEC_HDMI("\n <abort> rcvd; send out feature abort");
    //
    // Unsupported opcode; send f e a t u r e   a b o r t
    //
    cec_frame.bOpcode         = 0x00;
    cec_frame.bDestOrRXHeader = (sii_cec->bDestOrRXHeader & 0xf0) >> 4 ;
    cec_frame.bOperand[0]     = 0xff;
    cec_frame.bOperand[1]     = 0;
    cec_frame.bCount          = 2;
    SiI_CEC_SetCommand( &cec_frame );
}

static void CEC_HDLR_83_GivePhysicalAddr( SiI_CEC_t * sii_cec )
{
    SiI_CEC_t cec_frame;
    //HI_CEC_HDMI("\n CEC_HDLR_83_GivePhysicalAddr");
    //
    // Transmit Physical Address: 0.0.0.0
    //
    cec_frame.bOpcode         = 0x84;
    cec_frame.bDestOrRXHeader = 0x0F ;
    cec_frame.bOperand[0]     = 0x00 ; // [Physical Address]
    cec_frame.bOperand[1]     = 0x00 ; // [Physical Address]
    cec_frame.bOperand[2]     = 0x00 ; // [Device Type] = 0 = TV
    cec_frame.bCount          = 3 ;
    SiI_CEC_SetCommand( &cec_frame ) ;
}

#endif

static HI_U32 cec_RX_MSG_Operation(HI_U8 Init, HI_U8 Dest, HI_U8 opcode, HI_U8 *pOperand, HI_U8 OperandLength, SiI_CEC_t *pCECframe)
{
    HI_U32 Ret = SI_CEC_MESSAGE_RETURN_ERROR_NOSUPPORT;//Default Return is Error NoSupprot!
    
    switch(opcode)
    {
    /*General Protocol messages*/
    case CEC_OPCODE_FEATURE_ABORT:                   //0X00
        HI_CEC_HDMI("FEATURE ABORT Init:%d, Dest:%d, opcode:0x%x\n", Init, Dest, opcode);
        Ret = SI_CEC_MESSAGE_RETURN_OK_WITHOUT_RESPONES; //need not response
        break;
    case CEC_OPCODE_ABORT_MESSAGE:                   //0XFF
        HI_CEC_HDMI("ABORT MESSAGE\n");
        Ret = SI_CEC_MESSAGE_RETURN_OK_WITHOUT_RESPONES; //need not response
        break;
        
    /*One Touch Play Feature*/
    case CEC_OPCODE_ACTIVE_SOURCE:                   //0X82
        HI_CEC_HDMI("ACTIVE_SOURCE\n");
        HI_CEC_HDMI("It should be source cmd, ignore!\n");
        break;
    case CEC_OPCODE_IMAGE_VIEW_ON:                   //0X04
        HI_CEC_HDMI("IMAGE_VIEW_ON\n");
        HI_CEC_HDMI("It should be source cmd, ignore!\n");
        break;
    case CEC_OPCODE_TEXT_VIEW_ON:                    //0X0D
        HI_CEC_HDMI("TEXT_VIEW_ON\n");
        HI_CEC_HDMI("It should be source cmd, ignore!\n");
        break;
        
    /*Routing Control Feature*/
    case CEC_OPCODE_INACTIVE_SOURCE:                 //0X9D
        HI_CEC_HDMI("INACTIVE_SOURCE, No supprot\n");
        Ret = SI_CEC_MESSAGE_RETURN_ERROR_NOSUPPORT;
        break;
    case CEC_OPCODE_REQUEST_ACTIVE_SOURCE:           //0X85
    //When a device(TV) comes out of the standy state, it maybe broacast a
    //<Request Active Source> message to discover active source.
    //the active source device shall respond by broadcasting an <Active Source> message.
    {
        HI_U8 PhyAddr[4], LogAddr;
        HI_U32 Rec;
        Rec = DRV_HDMI_GetCECAddress(PhyAddr, &LogAddr);
        
        if(Rec != HI_SUCCESS)
        {
            break;
        }
        HI_CEC_HDMI("REQUEST_ACTIVE_SOURCE, return Active Source\n");
        //we should send back a message of ACTIVE_SOURCE
        pCECframe->bOpcode   = CEC_OPCODE_ACTIVE_SOURCE;
        pCECframe->bDestOrRXHeader = Init;
        pCECframe->bCount          = 2;
        pCECframe->bOperand[0]     = ((PhyAddr[0] << 4) & 0xf0) | (PhyAddr[1] & 0x0f); // [Physical Address(A.B.C.D):A B]
        pCECframe->bOperand[1]     = ((PhyAddr[2] << 4) & 0xf0) | (PhyAddr[3] & 0x0f) ; // [Physical Address(A.B.C.D):C D]
    }
        Ret = SI_CEC_MESSAGE_RETURN_OK_WITH_RESPONES;
        break;
    case CEC_OPCODE_ROUTING_CHANGE:                  //0X80
        HI_CEC_HDMI("ROUTING_CHANGE, it should be brocast message\n");
        break;
    case CEC_OPCODE_ROUTING_INFORMATION:             //0X81
        HI_CEC_HDMI("ROUTING_INFORMATION, it should be brocast message\n");
        break;
    case CEC_OPCODE_SET_STREAM_PATH:                 //0X86
    //the <Set Stream Path> is sent by the TV to the source device
    //to request it to broacst its path using an <Active Source> message.
        HI_CEC_HDMI("SET_STREAM_PATH\n");
    {
        HI_U8 PhyAddr[4], LogAddr;
        HI_U32 Rec;
        Rec = DRV_HDMI_GetCECAddress(PhyAddr, &LogAddr);
        
        if(Rec != HI_SUCCESS)
        {
            break;
        }
        HI_CEC_HDMI("REQUEST_ACTIVE_SOURCE, return Active Source\n");
        //we should send back a message of ACTIVE_SOURCE
        pCECframe->bOpcode   = CEC_OPCODE_ACTIVE_SOURCE;
        pCECframe->bDestOrRXHeader = Init;
        pCECframe->bCount          = 3;
        pCECframe->bOperand[0]     = ((PhyAddr[0] << 4) & 0xf0) | (PhyAddr[1] & 0x0f); // [Physical Address(A.B.C.D):A B]
        pCECframe->bOperand[1]     = ((PhyAddr[2] << 4) & 0xf0) | (PhyAddr[3] & 0x0f) ; // [Physical Address(A.B.C.D):C D]
        pCECframe->bOperand[2]     = LogAddr ; // [Device Type] = 3 Tuner1
    }
        Ret = SI_CEC_MESSAGE_RETURN_OK_WITH_RESPONES;
        break;
    /*Standby Feature*/
    case CEC_OPCODE_STANDBY:                         //0X36
        HI_CEC_HDMI("STANDBY! It should brocast message\n");
        break;
    /*One Touch Record Feature*/
    case CEC_OPCODE_RECORD_OFF:                      //0X0B
        break;
    case CEC_OPCODE_RECORD_ON:                       //0X09
        break;
    case CEC_OPCODE_RECORD_STATUS:                   //0X0A
        break;
    case CEC_OPCODE_RECORD_TV_SCREEN:                //0X0F
        break;
    /*Timer Programming Feature*/
    case CEC_OPCODE_CLEAR_ANALOGUE_TIMER:            //0X33
        break;
    case CEC_OPCODE_CLEAR_DIGITAL_TIMER:             //0X99
        break;
    case CEC_OPCODE_CLEAR_EXTERNAL_TIMER:            //0XA1
        break;
    case CEC_OPCODE_SET_ANALOGUE_TIMER:              //0X34
        break;
    case CEC_OPCODE_SET_DIGITAL_TIMER:               //0X97
        break;
    case CEC_OPCODE_SET_EXTERNAL_TIMER:              //0XA2
        break;
    case CEC_OPCODE_SET_TIMER_PROGRAM_TITLE:         //0X67
        break;
    case CEC_OPCODE_TIMER_CLEARED_STATUS:            //0X43
        break;
    case CEC_OPCODE_TIMER_STATUS:                    //0X35
        break;
        
    /*System Information Feature*/
    case CEC_OPCODE_CEC_VERSION:                     //0X9E
        HI_CEC_HDMI("CEC_VERSION:\n");
        {
            int index;
            for(index = 0; index < OperandLength; index ++)
            {
                if( ((pOperand[index] >= 'a') && (pOperand[index] <= 'z'))
                 || ((pOperand[index] >= 'A') && (pOperand[index] <= 'Z')) )
                {
                    HI_CEC_HDMI("%c", pOperand[index]);
                }
                else
                {
                    HI_CEC_HDMI("%02x", pOperand[index]);
                }
            }
        }
        HI_CEC_HDMI("\n");
        break;
    case CEC_OPCODE_GET_CEC_VERSION:                 //0X9F
        HI_CEC_HDMI("GET_CEC_VERSION, Response\n");
        //we should send back a message of ACTIVE_SOURCE
        pCECframe->bOpcode   = CEC_OPCODE_CEC_VERSION;
        pCECframe->bDestOrRXHeader = Init;
        pCECframe->bCount          = 3;
        pCECframe->bOperand[0]     = '1';
        pCECframe->bOperand[1]     = '3' ; 
        pCECframe->bOperand[2]     = 'a' ;
        
        Ret = SI_CEC_MESSAGE_RETURN_OK_WITH_RESPONES;
        break;
    case CEC_OPCODE_GIVE_PHYSICAL_ADDRESS:           //0X83
        HI_CEC_HDMI("GIVE_PHYSICAL_ADDRESS, Response\n");
    {
        HI_U8 PhyAddr[4], LogAddr;
        HI_U32 Rec;
        Rec = DRV_HDMI_GetCECAddress(PhyAddr, &LogAddr);
        
        if(Rec != HI_SUCCESS)
        {
            break;
        }
        HI_CEC_HDMI("return REPORT_PHYSICAL_ADDRESS\n");
        //we should send back a message of ACTIVE_SOURCE
        pCECframe->bOpcode   = CEC_OPCODE_REPORT_PHYSICAL_ADDRESS;
        pCECframe->bDestOrRXHeader = Init;
        pCECframe->bCount          = 3;
        pCECframe->bOperand[0]     = ((PhyAddr[0] << 4) & 0xf0) | (PhyAddr[1] & 0x0f); // [Physical Address(A.B.C.D):A B]
        pCECframe->bOperand[1]     = ((PhyAddr[2] << 4) & 0xf0) | (PhyAddr[3] & 0x0f) ; // [Physical Address(A.B.C.D):C D]
        pCECframe->bOperand[2]     = LogAddr;
    }
        Ret = SI_CEC_MESSAGE_RETURN_OK_WITH_RESPONES;
        break;
    case CEC_OPCODE_REPORT_PHYSICAL_ADDRESS:         //0X84
        HI_CEC_HDMI("REPORT_PHYSICAL_ADDRESS\n");
        if(OperandLength != 3)
        {
            HI_CEC_HDMI("Error oprand length:%d\n", OperandLength);
        }
        HI_CEC_HDMI("PhysicalAddr:%01d.%01d.%01d.%01d, LogicalAddr:%d\n", (pOperand[0] & 0xf0) >> 4, (pOperand[0] & 0x0f), \
         (pOperand[1] & 0xf0) >> 4, (pOperand[1] & 0x0f), pOperand[2]);
         
        Ret = SI_CEC_MESSAGE_RETURN_OK_WITHOUT_RESPONES;
        break;
    case CEC_OPCODE_GET_MENU_LANGUAGE:               //0X91
        HI_CEC_HDMI("GET_MENU_LANGUAGE, response\n");
        pCECframe->bOpcode   = CEC_OPCODE_SET_MENU_LANGUAGE;
        pCECframe->bDestOrRXHeader = Init;
        pCECframe->bCount          = 3;
        //Language: 3 ASCII Bytes as defined in ISO/FDIS 639-2
        pCECframe->bOperand[0]     = 'c';
        pCECframe->bOperand[1]     = 'h' ; 
        pCECframe->bOperand[2]     = 'i' ;
        
        Ret = SI_CEC_MESSAGE_RETURN_OK_WITH_RESPONES;
        break;
    case CEC_OPCODE_SET_MENU_LANGUAGE:               //0X32
        HI_CEC_HDMI("SET_MENU_LANGUAGE:");
        {
            int index;
            for(index = 0; index < OperandLength; index ++)
            {
                if( ((pOperand[index] >= 'a') && (pOperand[index] <= 'z'))
                 || ((pOperand[index] >= 'A') && (pOperand[index] <= 'Z')) )
                {
                    HI_CEC_HDMI("%c", pOperand[index]);
                }
                else
                {
                    HI_CEC_HDMI("%02x", pOperand[index]);
                }
            }
        }
        HI_CEC_HDMI("\n");
        break;
        
    /*Deck Control Feature*/
    case CEC_OPCODE_DECK_CONTROL:                    //0X42
        HI_CEC_HDMI("DECK_CONTROL\n");
        break;
    case CEC_OPCODE_DECK_STATUS:                     //0X1B
        HI_CEC_HDMI("DECK_STATUS\n");
        break;
    case CEC_OPCODE_GIVE_DECK_STATUS:                //0X1A
        HI_CEC_HDMI("GIVE_DECK_STATUS\n");
        break;
    case CEC_OPCODE_PLAY:                            //0X41
        HI_CEC_HDMI("PLAY\n");
        break;
        
    /*Tuner Control Feature*/
    case CEC_OPCODE_GIVE_TUNER_DEVICE_STATUS:        //0X08
        break;
    case CEC_OPCODE_SELECT_ANALOGUE_SERVICE:         //0X92
        break;
    case CEC_OPCODE_SELECT_DIGITAL_SERVICE:          //0X93
        break;
    case CEC_OPCODE_TUNER_DEVICE_STATUS:             //0X07
        break;
    case CEC_OPCODE_TUNER_STEP_DECREMENT:            //0X06
        break;
    case CEC_OPCODE_TUNER_STEP_INCREMENT:            //0X05
        break;
    /*Vendor Specific Command*/
    case CEC_OPCODE_DEVICE_VENDOR_ID:                //0X87
        HI_CEC_HDMI("DEVICE_VENDOR_ID:");
        {
            int index;
            for(index = 0; index < OperandLength; index ++)
            {
                HI_CEC_HDMI("%02x ", pOperand[index]);
            }
        }
        HI_CEC_HDMI("\n");
        break;
    case CEC_OPCODE_GIVE_DEVICE_VENDOR_ID:           //0X8C
        HI_CEC_HDMI("GIVE_DEVICE_VENDOR_ID, Response\n");
        pCECframe->bOpcode   = CEC_OPCODE_DEVICE_VENDOR_ID;
        pCECframe->bDestOrRXHeader = 0x0f;
        pCECframe->bCount          = 3;
        pCECframe->bOperand[0]     = 'h';
        pCECframe->bOperand[1]     = 'i' ; 
        pCECframe->bOperand[2]     = 's' ;
        
        Ret = SI_CEC_MESSAGE_RETURN_OK_WITH_RESPONES;
        break;
    case CEC_OPCODE_VENDOR_COMMAND:                  //0X89
        break;
    case CEC_OPCODE_VENDOR_COMMAND_WITH_ID:          //0XA0
        break;
    case CEC_OPCODE_VENDOR_REMOTE_BUTTON_DOWN:       //0X8A
        break;
    case CEC_OPCODE_VENDOR_REMOTE_BUTTON_UP:         //0X8B
        break;
        
    /*OSD Display Feature*/
    case CEC_OPCODE_SET_OSD_STRING:                  //0X64
        HI_CEC_HDMI("SET_OSD_NAME:%s\n", pOperand);
        Ret = SI_CEC_MESSAGE_RETURN_OK_WITHOUT_RESPONES;
        break;
    case CEC_OPCODE_GIVE_OSD_NAME:                   //0X46
        HI_CEC_HDMI("GIVE_DEVICE_VENDOR_ID, Response\n");
        pCECframe->bOpcode   = CEC_OPCODE_SET_OSD_STRING;
        pCECframe->bDestOrRXHeader = Init;
        pCECframe->bCount          = 3;
        pCECframe->bOperand[0]     = 's';
        pCECframe->bOperand[1]     = 't' ; 
        pCECframe->bOperand[2]     = 'b' ;
        
        Ret = SI_CEC_MESSAGE_RETURN_OK_WITH_RESPONES;
        break;
    case CEC_OPCODE_SET_OSD_NAME:                    //0X47
        HI_CEC_HDMI("SET_OSD_NAME:");
        {
            int index;
            for(index = 0; index < OperandLength; index ++)
            {
                if( ((pOperand[index] >= 'a') && (pOperand[index] <= 'z'))
                 || ((pOperand[index] >= 'A') && (pOperand[index] <= 'Z')) )
                {
                    HI_CEC_HDMI("%c", pOperand[index]);
                }
                else
                {
                    HI_CEC_HDMI("%02x", pOperand[index]);
                }
            }
        }
        HI_CEC_HDMI("\n");
        break;
        
    /*Device Menu Control Feature*/
    case CEC_OPCODE_MENU_REQUEST:                    //0X8D
        HI_CEC_HDMI("MENU_REQUEST\n");
        if(OperandLength < 1)
        {
            HI_CEC_HDMI("Invalid OperandLength:%d\n", OperandLength);
            break;
        }
        
        // Send back Menu STATUS
        pCECframe->bOpcode   = CEC_OPCODE_MENU_STATUS;
        pCECframe->bDestOrRXHeader = Init;
        pCECframe->bCount          = 1;
        
        if(pOperand[0] == 0x00)
        {
            HI_CEC_HDMI("Active\n");
            pCECframe->bOperand[0]     = 0X00;//Active
        }
        else if(pOperand[0] == 0x01)
        {
            HI_CEC_HDMI("Deactive\n");
            pCECframe->bOperand[0]     = 0X01;//Deactive
        }
        else if(pOperand[0] == 0x02)
        {
            HI_CEC_HDMI("Query\n");
        }
        else
        {
            HI_CEC_HDMI("Unknown, pOperand[0]:0x%x\n", pOperand[0]);
            break;
        }
        
        Ret = SI_CEC_MESSAGE_RETURN_OK_WITH_RESPONES;
        break;
    case CEC_OPCODE_MENU_STATUS:                     //0X8E
        HI_CEC_HDMI("MENU_STATUS:\n");
        if(OperandLength < 1)
        {
            HI_CEC_HDMI("Invalid OperandLength:%d\n", OperandLength);
            break;
        }
        if(pOperand[0] == 0x00)
        {
            HI_CEC_HDMI("Active\n");
        }
        else if(pOperand[0] == 0x01)
        {
            HI_CEC_HDMI("Deactive\n");
        }
        
        Ret = SI_CEC_MESSAGE_RETURN_OK_WITHOUT_RESPONES;
        break;
    case CEC_OPCODE_USER_CONTROL_PRESSED:            //0X44
        HI_CEC_HDMI("USER_CONTROL_PRESSED\n");
        HI_CEC_HDMI("Press RC:0x%02x\n", pOperand[0]);
        Ret = SI_CEC_MESSAGE_RETURN_OK_WITHOUT_RESPONES;
        break;
    case CEC_OPCODE_USER_CONTROL_RELEASED:           //0X45
        HI_CEC_HDMI("USER_CONTROL_RELEASED\n");
        HI_CEC_HDMI("Release RC:0x%02x\n", pOperand[0]);
        Ret = SI_CEC_MESSAGE_RETURN_OK_WITHOUT_RESPONES;
        break;
        
    /*Power Status Feature*/
    case CEC_OPCODE_GIVE_DEVICE_POWER_STATUS:        //0X8F
        HI_CEC_HDMI("GIVE_DEVICE_POWER_STATUS, response\n");
        pCECframe->bOpcode   = CEC_OPCODE_SET_OSD_STRING;
        pCECframe->bDestOrRXHeader = Init;
        pCECframe->bCount          = 1;
        pCECframe->bOperand[0]     = 0X00;//Power on
        
        Ret = SI_CEC_MESSAGE_RETURN_OK_WITH_RESPONES;
        break;
    case CEC_OPCODE_REPORT_POWER_STATUS:             //0X90
        HI_CEC_HDMI("REPORT_POWER_STATUS\n");
        if(OperandLength < 1)
        {
            HI_CEC_HDMI("Invalid OperandLength:%d\n", OperandLength);
            break;
        }
        if(pOperand[0] == 0x00)
        {
            HI_CEC_HDMI("Power On\n");
        }
        else if(pOperand[0] == 0x01)
        {
            HI_CEC_HDMI("Stand By\n");
        }
        else
        {
            HI_CEC_HDMI("Unknown, pOperand[0]:0x%x\n", pOperand[0]);
        }
        Ret = SI_CEC_MESSAGE_RETURN_OK_WITHOUT_RESPONES;
        break;
        
    /*System Audio Control Feature*/
    case CEC_OPCODE_GIVE_AUDIO_STATUS:               //0X71
        HI_CEC_HDMI("GIVE_AUDIO_STATUS\n");
        break;
    case CEC_OPCODE_GIVE_SYSTEM_AUDIO_MODE_STATUS:   //0x7D
        break;
    case CEC_OPCODE_REPORT_AUDIO_STATUS:             //0X7A
        break;
    case CEC_OPCODE_SET_SYSTEM_AUDIO_MODE:           //0X72
        break;
    case CEC_OPCODE_SYSTEM_AUDIO_MODE_REQUEST:       //0X70
        break;
    case CEC_OPCODE_SYSTEM_AUDIO_MODE_STATUS:        //0X7E
        break;
        
    /*Audio Rate Control Feature*/
    case CEC_OPCODE_SET_AUDIO_RATE:                  //0X9A
        break;
    default:
        return 3;
    }
    return Ret;
}

// Check received MSG validation.
// return 0 if msg is 0K, return 1 if msg is 1nva1id
static HI_U8 cec_msg_validate( SiI_CEC_t * sii_cec )
{
    HI_U32 result = 0;
    SiI_CEC_t cec_frame;
    HI_U8 CEC_RX_INIT, CEC_RX_DEST;

    HI_U8 PhyAddr[4], LogAddr;
    HI_U32 Rec;
    Rec = DRV_HDMI_GetCECAddress(PhyAddr, &LogAddr);
    
    if(Rec != HI_SUCCESS)
    {
        return 1;
    }
    
    //Deal with CEC Rx message
    CEC_RX_DEST = (sii_cec->bDestOrRXHeader & 0x0f);
    CEC_RX_INIT = (sii_cec->bDestOrRXHeader & 0xf0) >> 4;
    
    if(CEC_RX_INIT == LogAddr)
    {
        HI_CEC_HDMI("Selft Message, ignore\n");
        return 1;
    }
    if ((CEC_RX_DEST != LogAddr) && (CEC_RX_DEST != SiI_CEC_LogAddr_Unregister))
    {
        HI_CEC_HDMI("Message is not sent to us CEC_RX_DEST:%d\n", CEC_RX_DEST);
        return 1;
    }
    
    memset(&cec_frame, 0, sizeof(SiI_CEC_t));
    result = cec_RX_MSG_Operation(CEC_RX_INIT, CEC_RX_DEST, sii_cec->bOpcode, sii_cec->bOperand, (sii_cec->bCount & 0x0F), &cec_frame);
    if(result == SI_CEC_MESSAGE_RETURN_OK_WITH_RESPONES)
    {
        HI_CEC_HDMI("response!\n");
        //SiI_CEC_SetCommand(&cec_frame);        
        return 0;
    }
    else if(result == SI_CEC_MESSAGE_RETURN_OK_WITHOUT_RESPONES)
    {
        return 0;//direct return!
    }
    else if(result == SI_CEC_MESSAGE_RETURN_ERROR_NOSUPPORT)
    {
        return 0;
    }

    //Deal with Unsupport Message!    //
    // Do not reply to Broadcast msgs, otherwise send Feature Abort
    // for all unsupported features.
    if( (sii_cec->bDestOrRXHeader & 0x0F) != 0x0F)
    {
        /*All devices shall support the message <Feature Abort>.
          It is used to allow devices to indicate if they do not support an opcode that has been
          sent to them, if it is unable to deal with this message.
          <Feature Abort> has 2 parameters: opcode and the reason of its reject of the frame.
          
          <Feature Abort> is used as a response to any failure!
        */
        // Unsupported opcode; send feature abort
        //<Feature Abort>has 2 parameters, the opcode and a reason for its rejection of the frame.
        memset(&cec_frame, 0, sizeof(SiI_CEC_t));
        cec_frame.bOpcode         = CEC_OPCODE_FEATURE_ABORT;//0x00;
        cec_frame.bDestOrRXHeader = (sii_cec->bDestOrRXHeader & 0xf0) >> 4 ;
        cec_frame.bOperand[0]     = sii_cec->bOpcode;
        cec_frame.bOperand[1]     = 0x00; //Unrecognized opcode:0x00
        cec_frame.bCount = 2;
        //SiI_CEC_SetCommand( &cec_frame );
        HI_CEC_HDMI("\n!Unsupported bOpcode:0x%x Feature!\n", sii_cec->bOpcode);
    }
    else
    {
        // Unsupported Broadcast Msg
        HI_CEC_HDMI("\n! Unsupported Brocast Msg!\n");
    }

    return 1;
}

#define CEC_CMD_POOL_SIZE 512
SiI_CEC_t cec_cmd_pool[CEC_CMD_POOL_SIZE];
unsigned int cmd_pool_rptr = 0;
unsigned int cmd_pool_wptr = 0;
unsigned int can_read_num = 0;
unsigned int  get_cmd_counter = 0;
wait_queue_head_t hdmi_cec_wait_queue_head ;

void get_buf_status(void)
{
    unsigned int i = 0;
    if(cmd_pool_rptr == cmd_pool_wptr){
        cmd_pool_rptr = 0;
        cmd_pool_wptr = 0;
        for(i=0; i < CEC_CMD_POOL_SIZE; i++)
            memset(&cec_cmd_pool[i], 0, sizeof(SiI_CEC_t));
    }
}

void store_cec_cmd(SiI_CEC_t *rx_cmd)
{
    get_buf_status();
    memcpy(&cec_cmd_pool[cmd_pool_wptr], rx_cmd, sizeof(SiI_CEC_t));

    if(cmd_pool_wptr == CEC_CMD_POOL_SIZE - 1){
        cmd_pool_wptr = 0;
    }else {
        cmd_pool_wptr++;
    }

    if(cmd_pool_rptr == cmd_pool_wptr){
        can_read_num = CEC_CMD_POOL_SIZE;
    }else if(cmd_pool_rptr <  cmd_pool_wptr){
        can_read_num = cmd_pool_wptr - cmd_pool_rptr;
    }else{
        can_read_num = CEC_CMD_POOL_SIZE -(cmd_pool_rptr - cmd_pool_wptr);
    }

    if((can_read_num > 0) && (get_cmd_counter > 0)){
        wake_up_interruptible(&hdmi_cec_wait_queue_head);
    }
}

unsigned int  get_cec_cmd(HI_UNF_HDMI_CEC_CMD_S *rx_cmd, unsigned int num, HI_U32 timeout)
{
    unsigned int i = 0;
    if(get_cmd_counter == 0)
        init_waitqueue_head(&hdmi_cec_wait_queue_head);
    get_cmd_counter ++;

    wait_event_interruptible_timeout(hdmi_cec_wait_queue_head, (can_read_num > 0) , timeout);

    if(can_read_num > 0){
        rx_cmd[i].enSrcAdd = cec_cmd_pool[cmd_pool_rptr].bDestOrRXHeader & 0xf0;
        rx_cmd[i].enDstAdd = cec_cmd_pool[cmd_pool_rptr].bDestOrRXHeader & 0x0f;
        rx_cmd[i].u8Opcode = cec_cmd_pool[cmd_pool_rptr].bOpcode;
        memcpy(&rx_cmd[i].unOperand.stRawData, cec_cmd_pool[cmd_pool_rptr].bOperand, 16);

        if(cmd_pool_rptr == CEC_CMD_POOL_SIZE - 1){
            cmd_pool_rptr = 0;
        }else {
            cmd_pool_rptr ++;
        }
        i = 1;
    }

    if(cmd_pool_rptr == cmd_pool_wptr){
        can_read_num = 0;
    }else if(cmd_pool_rptr <  cmd_pool_wptr){
        can_read_num = cmd_pool_wptr - cmd_pool_rptr;
    }else{
        can_read_num = CEC_CMD_POOL_SIZE -(cmd_pool_rptr - cmd_pool_wptr);
    }

    return  i;
}



//------------------------------------------------------------------------------
// Function Name: ParsingReceivedData
//
// Function Description:
// 1. CEC7.3 Frame Validation
// 2. Protocol Extension
// 3. CEC12.3 Feature Abort
// 4. Amber Alert i.e. call to function specified by the handler that is expecting
//    a reply from the specified device, LA, LogicalAddress.
//------------------------------------------------------------------------------
static void cec_ParsingRecevedData ( HI_U8 bRXState )
{
    HI_U8 bAuxData;
    HI_U8 i;
    SiI_CEC_t SiI_CEC;
    
    //
    // CEC_RX_COUNT Register:  RX_ERROR | CEC_RX_CMD_CNT | CEC_RX_byte_CNT
    // See CPI document for details.
    //
    bAuxData = bRXState & 0xF0;
    if( bAuxData )
    {
        memset(&SiI_CEC, 0, sizeof(SiI_CEC_t));
        SiI_CEC.bCount = bRXState;
        bAuxData >>=4;
        HI_CEC_HDMI ("\n %i frames in RX FIFO\n", (int) bAuxData );
        /* Get each Frame from RX FIFO */
        for ( i = 0; i < bAuxData; i++ )
        {
            HI_CEC_HDMI ("Deal with %i frames in RX FIFO\n", (int) bAuxData );
            SI_CEC_GetCommand( &SiI_CEC );
            
            if(((SiI_CEC.bDestOrRXHeader & 0xf0) >> 4) == CurLogAddr)
            {
                HI_CEC_HDMI("We should ignore this message, it is sent from ourself\n");
            }
            else
            {
                PrintCommand( &SiI_CEC );
                // Check BC msg && init==0x00
                // Validation Message
                if(0 == cec_msg_validate( &SiI_CEC )){
                    store_cec_cmd(&SiI_CEC);
                }
            }
            /* Get the next message */
            memset(&SiI_CEC, 0, sizeof(SiI_CEC_t));
            if ( SiI_CEC.bRXNextCount)
                SiI_CEC.bCount = SiI_CEC.bRXNextCount;
        }
    }
    return;
}

//------------------------------------------------------------------------------
// Function Name: userSWTtask
// Function Description:
// This is function handler event from the CEC RX buffer.
// CEC Software Connection #5.
//------------------------------------------------------------------------------

static HI_U8 bCECTask = 0;
static HI_U8 bNewTask = SiI_CEC_Idle;
static HI_U8 result[16] = {0};
HI_U8 bTXState = SI_CEC_TX_WAITCMD;

void SI_CEC_Event_Handler( void )
{
    static HI_U8 i = 0;
    SiI_CEC_Int_t CEC_Int;
    
    if( bNewTask != bCECTask )
    {
        bNewTask = bCECTask;
        i = 0;
    }
    //HI_CEC_HDMI("xxx ping bTXState:%d, SI_CEC_TX_WAITCMD:%d\n", bTXState, SI_CEC_TX_WAITCMD);
    if (bCECTask)
    {
        //HI_CEC_HDMI ("\nCEC_Event_Handler CECTask %x %x\n",             bCECTask, bTXState);
    }
    
    // Process CEC Events; at this point doesn't do much
    SI_CEC_Interrupt_Processing(&CEC_Int);

    if( bCECTask == SiI_CEC_ReqPing )
    {
        HI_CEC_HDMI("ping bTXState:%d, SI_CEC_TX_WAITCMD:%d\n", bTXState, SI_CEC_TX_WAITCMD);
        //
        // Test function 1: Ping Button Pressed
        //
        if( bTXState == SI_CEC_TX_WAITCMD )
        {
            HI_CEC_HDMI("request ping\n");
            HI_CEC_HDMI("\n %X", (int)i );
            SI_SiI_CEC_SendPing( i );
            
            bTXState = SI_CEC_TX_SENDING;
        }
        else if(bTXState == SI_CEC_TX_SENDING )
        {
            if( CEC_Int.bTXState == SI_CEC_TX_SENDFAILURE )
            {
                result[i] = 0;
                
                HI_CEC_HDMI (" NoAck \n");
                cec_PrintLogAddr(i);
                i++;
                bTXState = SI_CEC_TX_WAITCMD;              
            }
            if( CEC_Int.bTXState == SI_CEC_TX_SENDACKED )
            {
                result[i] = 1;
                
                HI_CEC_HDMI (" Ack \n"); cec_PrintLogAddr(i);
                i++;
                bTXState = SI_CEC_TX_WAITCMD;
            
            }
            if( i >= SiI_CEC_LogAddr_Unregister )
            {
                bCECTask = SiI_CEC_Idle;
                HI_CEC_HDMI("\nend of send command\n");
                {
                    int index;
                    for(index = 0;index < 16; index ++)
                    {
                        HI_CEC_HDMI("index:%d, result:%d\n", index, result[index]);
                    }
                    HI_CEC_HDMI("end\n");
                }
                i = 0;
            }
        }
    }
    else if(bCECTask == SiI_CEC_ReqCmd1)
    {
        //  Test function 2: Send test message
        if ( CEC_Int.bTXState == SI_CEC_TX_SENDFAILURE )
        {
            HI_CEC_HDMI (" NoAck ");
            bTXState = SI_CEC_TX_WAITCMD;
            bCECTask = SiI_CEC_Idle;
        }
        if ( CEC_Int.bTXState == SI_CEC_TX_SENDACKED )
        {
            HI_CEC_HDMI (" Ack ");
            bTXState = SI_CEC_TX_WAITCMD;
            bCECTask = SiI_CEC_Idle;
        }
    }
    
    // Check for incoming CEC frames in the Rx Fifo.
    if( CEC_Int.bRXState )
    {
        cec_ParsingRecevedData( CEC_Int.bRXState );
    }
}

HI_U32 SI_CEC_SendCommand(HI_UNF_HDMI_CEC_CMD_S  *pCECCmd)
{
    SiI_CEC_t cec_frame;
    memset(&cec_frame, 0, sizeof(SiI_CEC_t));
    
    SI_CEC_RegisterWrite(REG__CEC_TX_INIT, pCECCmd->enSrcAdd);
    
    CurLogAddr = pCECCmd->enSrcAdd;

    if(pCECCmd->u8Opcode == CEC_OPCODE_POLLING_MESSAGE)
    {
        SI_SiI_CEC_SendPing(pCECCmd->enDstAdd);
        bCECTask = SiI_CEC_ReqPing;
        
        HI_CEC_HDMI("\nSend Ping cmd\n");
        return 0;
    }
    
    cec_frame.bOpcode         = pCECCmd->u8Opcode;
    cec_frame.bDestOrRXHeader = pCECCmd->enDstAdd;
    cec_frame.bCount          = pCECCmd->unOperand.stRawData.u8Length;
    memcpy((cec_frame.bOperand), (pCECCmd->unOperand.stRawData.u8Data), pCECCmd->unOperand.stRawData.u8Length);
    
    bCECTask = SiI_CEC_ReqCmd1;
    
    SiI_CEC_SetCommand( &cec_frame );
    
    return HI_SUCCESS;
}

HI_U32 SI_CEC_AudioPing(HI_U32 *pu32Status)
{
    HI_U8 status, timeout = 0;
    HI_U32 Error = HI_SUCCESS;

    /* CEC Set Up Register */
    WriteByteHDMICEC(REG__CEC_AUTO_PING_CTRL, 0x02); //0xcc 0xe0

    WriteByteHDMICEC(REG__CEC_AUTO_PING_CTRL, 0x01);  //start cec ping

    while(timeout ++ < 10)
    {
        msleep(100);
        status = ReadByteHDMICEC(REG__CEC_AUTO_PING_CTRL);
        if(0x80 == (status & 0x80))
        {
            HI_CEC_HDMI("REG__CEC_AUTO_PING_CTRL:0x%x\n", status);
            break;
        }
    }

    if(timeout >= 100)
    {
        HI_ERR_HDMI("AutoPing timeout\n");
        return HI_FAILURE;
    }
    
    status = ReadByteHDMICEC(REG__CEC_AUTO_PING_MAP0);          //0xcc 0xe1
    //HI_CEC_HDMI("AUTO_PING_MAP0:0x%x\n",status);
    status = ReadByteHDMICEC(REG__CEC_AUTO_PING_MAP1) + status; //0xcc 0xe2

    HI_INFO_HDMI("CEC Auto Ping Result:0x%x\n", status);
    WriteByteHDMICEC(REG__CEC_AUTO_PING_CTRL, 0x00);
    
    *pu32Status = status;

    return Error;
}

HI_U32 SI_CEC_Open(void)
{
    //Set CEC Interrupt bit
    SI_CEC_RegisterWrite( REG__CEC_INT_ENABLE_0, 0xff );
    SI_CEC_RegisterWrite( REG__CEC_INT_ENABLE_1, 0xff );
    //Set Logical Address and Caputure address
    SI_CEC_CAPTURE_ID_Set(0X00);
    
    return 0;
}

HI_U32 SI_CEC_Close(void)
{
    //Clean CEC Interrupt bit
    SI_CEC_RegisterWrite( REG__CEC_INT_ENABLE_0, 0x00 );
    SI_CEC_RegisterWrite( REG__CEC_INT_ENABLE_1, 0x00 );

    return 0;
}
#endif

/*---------------------------END-----------------------*/

