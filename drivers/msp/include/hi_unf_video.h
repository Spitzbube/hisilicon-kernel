/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_unf_video.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2009/12/17
  Description   :
  History       :
  1.Date        : 2009/12/17
    Author      : w58735
    Modification: Created file

*******************************************************************************/

#ifndef __HI_UNF_VIDEO_H__
#define __HI_UNF_VIDEO_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "hi_common.h"

/*************************** Structure Definition ****************************/
/** \addtogroup      VIDEO */
/** @{ */  /** <!-- [VIDEO] */

/**Defines the video norm.*/
/**CNcomment: ������Ƶ��ʽö��*/
typedef enum hiUNF_ENC_FMT_E
{
    HI_UNF_ENC_FMT_1080P_60 = 0,     /**<1080p 60 Hz*/
    HI_UNF_ENC_FMT_1080P_50,         /**<1080p 50 Hz*/
    HI_UNF_ENC_FMT_1080P_30,         /**<1080p 30 Hz*/
    HI_UNF_ENC_FMT_1080P_25,         /**<1080p 25 Hz*/
    HI_UNF_ENC_FMT_1080P_24,         /**<1080p 24 Hz*/

    HI_UNF_ENC_FMT_1080i_60,         /**<1080i 60 Hz*/
    HI_UNF_ENC_FMT_1080i_50,         /**<1080i 60 Hz*/

    HI_UNF_ENC_FMT_720P_60,          /**<720p 60 Hz*/
    HI_UNF_ENC_FMT_720P_50,          /**<720p 50 Hz */

    HI_UNF_ENC_FMT_576P_50,          /**<576p 50 Hz*/
    HI_UNF_ENC_FMT_480P_60, //10         /**<480p 60 Hz*/

    HI_UNF_ENC_FMT_PAL,              /* B D G H I PAL */
    HI_UNF_ENC_FMT_PAL_N,            /* (N)PAL        */
    HI_UNF_ENC_FMT_PAL_Nc,           /* (Nc)PAL       */

    HI_UNF_ENC_FMT_NTSC,             /* (M)NTSC       */
    HI_UNF_ENC_FMT_NTSC_J,           /* NTSC-J        */
    HI_UNF_ENC_FMT_NTSC_PAL_M,       /* (M)PAL        */

    HI_UNF_ENC_FMT_SECAM_SIN,        /**< SECAM_SIN*/
    HI_UNF_ENC_FMT_SECAM_COS, //18       /**< SECAM_COS*/

    HI_UNF_ENC_FMT_1080P_24_FRAME_PACKING,
    HI_UNF_ENC_FMT_720P_60_FRAME_PACKING,
    HI_UNF_ENC_FMT_720P_50_FRAME_PACKING, //21

    HI_UNF_ENC_FMT_861D_640X480_60,
    HI_UNF_ENC_FMT_VESA_800X600_60,
    HI_UNF_ENC_FMT_VESA_1024X768_60,
    HI_UNF_ENC_FMT_VESA_1280X720_60,
    HI_UNF_ENC_FMT_VESA_1280X800_60,
    HI_UNF_ENC_FMT_VESA_1280X1024_60,
    HI_UNF_ENC_FMT_VESA_1360X768_60,
    HI_UNF_ENC_FMT_VESA_1366X768_60,
    HI_UNF_ENC_FMT_VESA_1400X1050_60,
    HI_UNF_ENC_FMT_VESA_1440X900_60,
    HI_UNF_ENC_FMT_VESA_1440X900_60_RB,
    HI_UNF_ENC_FMT_VESA_1600X900_60_RB,
    HI_UNF_ENC_FMT_VESA_1600X1200_60,
    HI_UNF_ENC_FMT_VESA_1680X1050_60,     
    HI_UNF_ENC_FMT_VESA_1680X1050_60_RB,
    HI_UNF_ENC_FMT_VESA_1920X1080_60,
    HI_UNF_ENC_FMT_VESA_1920X1200_60,
    HI_UNF_ENC_FMT_VESA_1920X1440_60,
    HI_UNF_ENC_FMT_VESA_2048X1152_60, //40
    HI_UNF_ENC_FMT_VESA_2560X1440_60_RB,
    HI_UNF_ENC_FMT_VESA_2560X1600_60_RB, //42

    HI_UNF_ENC_FMT_BUTT
}HI_UNF_ENC_FMT_E;

/**Defines the mode of the video decoder.*/
/**CNcomment: ������Ƶ������ģʽö��*/
typedef enum hiUNF_VCODEC_MODE_E
{
    HI_UNF_VCODEC_MODE_NORMAL = 0,   /**<Decode all frames.*/ /**<CNcomment: ������֡*/
    HI_UNF_VCODEC_MODE_IP,           /**<Decode only I frames and P frames.*/ /**<CNcomment: ֻ��IP֡*/
    HI_UNF_VCODEC_MODE_I,            /**<Decode only I frames.*/ /**<CNcomment: ֻ��I֡*/
    HI_UNF_VCODEC_MODE_DROP_INVALID_B,  /**<Decode all frames except the first B frame sequence behind I frames */ /**<CNcomment: ������֡�����˽�����I֡�����B֡*/
    HI_UNF_VCODEC_MODE_BUTT
}HI_UNF_VCODEC_MODE_E;

/**Defines special control operation of decoder*/
/**CNcomment: ���������������������ѡ�� */
typedef enum hiHI_UNF_VCODEC_CTRL_OPTION_E
{
    HI_UNF_VCODEC_CTRL_OPTION_SIMPLE_DPB = 0x1,
    
} HI_UNF_VCODEC_CTRL_OPTION_E;

/**Defines the decoding capability (resolution) of the decoder.*/
/**CNcomment: �����������������(�ֱ���) */
typedef enum hiUNF_VCODEC_CAP_LEVEL_E
{
    HI_UNF_VCODEC_CAP_LEVEL_NULL = 0, /**<Do not decode.*/ /**<CNcomment: ����Ҫ���� */
    HI_UNF_VCODEC_CAP_LEVEL_QCIF = 0, /**<The resolution of the picture to be decoded is less than or equal to 176x144.*/ /**<CNcomment: �����ͼ���С������176*144 */
    HI_UNF_VCODEC_CAP_LEVEL_CIF,      /**<The resolution of the picture to be decoded less than or equal to 352x288.*/ /**<CNcomment: �����ͼ���С������352*288 */
    HI_UNF_VCODEC_CAP_LEVEL_D1,       /**<The resolution of the picture to be decoded less than or equal to 720x576.*/ /**<CNcomment: �����ͼ���С������720*576 */  
    HI_UNF_VCODEC_CAP_LEVEL_720P,     /**<The resolution of the picture to be decoded is less than or equal to 1280x720.*/ /**<CNcomment: �����ͼ���С������1280*720 */
    HI_UNF_VCODEC_CAP_LEVEL_FULLHD,   /**<The resolution of the picture to be decoded is less than or equal to 1920x1080.*/ /**<CNcomment: �����ͼ���С������1920*1080 */ 

    HI_UNF_VCODEC_CAP_LEVEL_1280x800, /**<The resolution of the picture to be decoded is less than or equal to 1280x800.*/ /**<CNcomment: �����ͼ���С������1280x800*/ 
    HI_UNF_VCODEC_CAP_LEVEL_800x1280, /**<The resolution of the picture to be decoded is less than or equal to 800x1280.*/ /**<CNcomment: �����ͼ���С������800x1280*/ 
    HI_UNF_VCODEC_CAP_LEVEL_1488x1280, /**<The resolution of the picture to be decoded is less than or equal to 1488x1280.*/ /**<CNcomment: �����ͼ���С������1488x1280 */ 
    HI_UNF_VCODEC_CAP_LEVEL_1280x1488, /**<The resolution of the picture to be decoded is less than or equal to 1280x1488.*/ /**<CNcomment: �����ͼ���С������1280x1488 */ 
    HI_UNF_VCODEC_CAP_LEVEL_2160x1280, /**<The resolution of the picture to be decoded is less than or equal to 2160x1280.*/ /**<CNcomment: �����ͼ���С������2160x1280 */ 
    HI_UNF_VCODEC_CAP_LEVEL_1280x2160, /**<The resolution of the picture to be decoded is less than or equal to 1280x2160.*/ /**<CNcomment: �����ͼ���С������1280x2160 */ 
    HI_UNF_VCODEC_CAP_LEVEL_2160x2160, /**<The resolution of the picture to be decoded is less than or equal to 2160x2160.*/ /**<CNcomment: �����ͼ���С������2160x2160 */ 
    HI_UNF_VCODEC_CAP_LEVEL_4096x2160, /**<The resolution of the picture to be decoded is less than or equal to 4096x2160.*/ /**<CNcomment: �����ͼ���С������4096x2160 */ 
    HI_UNF_VCODEC_CAP_LEVEL_2160x4096, /**<The resolution of the picture to be decoded is less than or equal to 2160x4096.*/ /**<CNcomment: �����ͼ���С������2160x4096 */ 
    HI_UNF_VCODEC_CAP_LEVEL_4096x4096, /**<The resolution of the picture to be decoded is less than or equal to 4096x4096.*/ /**<CNcomment: �����ͼ���С������4096x4096 */ 
    HI_UNF_VCODEC_CAP_LEVEL_8192x4096, /**<The resolution of the picture to be decoded is less than or equal to 8192x4096.*/ /**<CNcomment: �����ͼ���С������8192x4096 */ 
    HI_UNF_VCODEC_CAP_LEVEL_4096x8192, /**<The resolution of the picture to be decoded is less than or equal to 4096x8192.*/ /**<CNcomment: �����ͼ���С������4096x8192 */ 
    HI_UNF_VCODEC_CAP_LEVEL_8192x8192, /**<The resolution of the picture to be decoded is less than or equal to 8192x8192.*/ /**<CNcomment: �����ͼ���С������8192x8192 */ 

    HI_UNF_VCODEC_CAP_LEVEL_BUTT    
} HI_UNF_VCODEC_CAP_LEVEL_E;

/**Defines the stream type supported by the decoder.*/
/**CNcomment: ���������֧�ֵ��������� */
typedef enum hiUNF_VCODEC_TYPE_E
{
    HI_UNF_VCODEC_TYPE_MPEG2,       /**<MPEG2*/
    HI_UNF_VCODEC_TYPE_MPEG4,       /**<MPEG4 DIVX4 DIVX5*/
    HI_UNF_VCODEC_TYPE_AVS,         /**<AVS*/
    HI_UNF_VCODEC_TYPE_H263,        /**<H263*/
    HI_UNF_VCODEC_TYPE_H264,        /**<H264*/
    HI_UNF_VCODEC_TYPE_REAL8,       /**<REAL*/
    HI_UNF_VCODEC_TYPE_REAL9,       /**<REAL*/
    HI_UNF_VCODEC_TYPE_VC1,         /**<VC-1*/
    HI_UNF_VCODEC_TYPE_VP6,         /**<VP6*/
    HI_UNF_VCODEC_TYPE_VP6F,        /**<VP6F*/
    HI_UNF_VCODEC_TYPE_VP6A,        /**<VP6A*/
    HI_UNF_VCODEC_TYPE_MJPEG,       /**<MJPEG*/
    HI_UNF_VCODEC_TYPE_SORENSON,    /**<SORENSON SPARK*/
    HI_UNF_VCODEC_TYPE_DIVX3,       /**<DIVX3*/
    HI_UNF_VCODEC_TYPE_RAW,         /**RAW*/
    HI_UNF_VCODEC_TYPE_JPEG,        /**JPEG,added for VENC*/
    HI_UNF_VCODEC_TYPE_VP8,         /**<VP8*/
    HI_UNF_VCODEC_TYPE_MSMPEG4V1,   /**< MS private MPEG4 */
    HI_UNF_VCODEC_TYPE_MSMPEG4V2,
    HI_UNF_VCODEC_TYPE_MSVIDEO1,    /**< MS video */
    HI_UNF_VCODEC_TYPE_WMV1,
    HI_UNF_VCODEC_TYPE_WMV2,
    HI_UNF_VCODEC_TYPE_RV10,
    HI_UNF_VCODEC_TYPE_RV20,
    HI_UNF_VCODEC_TYPE_SVQ1,        /**< Apple video */
    HI_UNF_VCODEC_TYPE_SVQ3,        /**< Apple video */
    HI_UNF_VCODEC_TYPE_H261,
    HI_UNF_VCODEC_TYPE_VP3,
    HI_UNF_VCODEC_TYPE_VP5,
    HI_UNF_VCODEC_TYPE_CINEPAK,
    HI_UNF_VCODEC_TYPE_INDEO2,
    HI_UNF_VCODEC_TYPE_INDEO3,
    HI_UNF_VCODEC_TYPE_INDEO4,
    HI_UNF_VCODEC_TYPE_INDEO5,
    HI_UNF_VCODEC_TYPE_MJPEGB,
    HI_UNF_VCODEC_TYPE_MVC,
    HI_UNF_VCODEC_TYPE_HEVC,     /**<HEVC, not supported*/
    HI_UNF_VCODEC_TYPE_DV,
    HI_UNF_VCODEC_TYPE_BUTT
}HI_UNF_VCODEC_TYPE_E;

/**Defines the H264 profile of the video encoder.*/
/**CNcomment: ������Ƶ������ H264Э�鵵��*/
typedef enum hiUNF_H264_PROFILE_E
{
    HI_UNF_H264_PROFILE_BASELINE = 0,   /**<Encode H264 stream by baseline profile.*/ /**<CNcomment: ���û��ߵ��α���H264����*/
    HI_UNF_H264_PROFILE_MAIN,           /**<Encode H264 stream by main profile.*/     /**<CNcomment:������Ҫ���α���H264����*/
    HI_UNF_H264_PROFILE_EXTENDED,       /**<Encode H264 stream by extended profile.*/ /**<CNcomment:������չ���α���H264����*/
    HI_UNF_H264_PROFILE_HIGH,           /**<Encode H264 stream by high profile.*/     /**<CNcomment: ���ø߼����α���H264����*/
    HI_UNF_H264_PROFILE_BUTT
}HI_UNF_H264_PROFILE_E;


typedef struct hiUNF_VCODEC_VC1_ATTR_S
{
	HI_BOOL  bAdvancedProfile;   /**<Whether the profile is an advanced profile*/ /**<CNcomment: �Ƿ�Advanced Profile*/
	HI_U32   u32CodecVersion;    /**<Version number*/ /**<CNcomment: �汾��*/
}HI_UNF_VCODEC_VC1_ATTR_S;

typedef struct hiUNF_VCODEC_VP6_ATTR_S
{
	HI_BOOL  bReversed;    /**<To reverse a picture, set this parameter to 1. In this cases, set it to 0.*/ /**<CNcomment: ͼ����Ҫ��תʱ��1��������0*/
}HI_UNF_VCODEC_VP6_ATTR_S;

typedef union hiUNF_VCODEC_EXTATTR_U
{
	HI_UNF_VCODEC_VC1_ATTR_S stVC1Attr;
	HI_UNF_VCODEC_VP6_ATTR_S stVP6Attr;
}HI_UNF_VCODEC_EXTATTR_U;

/**Defines the attributes of a video decoder.*/
/**CNcomment: ������Ƶ���������Խṹ*/
typedef struct hiUNF_VCODEC_ATTR_S
{
    HI_UNF_VCODEC_TYPE_E        enType;          /**<Video encoding type*/ /**<CNcomment: ��Ƶ��������*/
	HI_UNF_VCODEC_EXTATTR_U     unExtAttr;       /**<Extra attributes related to the video encoding type*/ /**<CNcomment: ��Ƶ����������صĶ�������*/
    HI_UNF_VCODEC_MODE_E  	    enMode;          /**<Mode of a video decoder*/ /**<CNcomment: ��Ƶ������ģʽģʽ*/
    HI_U32                	    u32ErrCover;     /**<Error concealment threshold of the output frames of a video decoder. The value 0 indicates that no frames are output if an error occurs; the value 100 indicates that all frames are output no matter whether errors occur.*/
                                                 /**<CNcomment: ��Ƶ�����������֡�����������ޣ�0:���ִ��󼴲������100:���ܴ������ȫ�����*/
    HI_U32                	    u32Priority;     /**<Priority of a video decoder. The value range is [1, HI_UNF_VCODEC_MAX_PRIORITY]. The value 0 is a reserved value. If you set the value to 0, no error message is displayed, but the value 1 is used automatically. The smaller the value, the lower the priority.*/
                                                 /**<CNcomment: ��Ƶ���������ȼ�, ȡֵ��Χ: ���ڵ���1��С�ڵ���HI_UNF_VCODEC_MAX_PRIORITY, 0�Ǳ���ֵ������Ϊ0���ᱨ�?���ǻ��Զ�ȡֵΪ1,��ֵԽС���ȼ�Խ�� */
    HI_BOOL                     bOrderOutput;    /**<Whether the videos are output by the decoding sequence. You are advised to set this parameter to HI_TRUE in VP mode, and HI_FALSE in other modes.*/
                                                 /**<CNcomment: �Ƿ񰴽����������VPģʽ���Ƽ�����ΪHI_TRUE, һ��ģʽ������ΪHI_FALSE */	
    HI_S32                      s32CtrlOptions;  /**<The value is HI_UNF_VCODEC_CTRL_OPTION_E, or the 'or' value of several enum type*/
                                                 /**<CNcomment: ȡֵΪHI_UNF_VCODEC_CTRL_OPTION_E�����߼���ö�ٵ�'��'  */
    HI_VOID*                    pCodecContext;   /**<Private codec context */
}HI_UNF_VCODEC_ATTR_S;


/**Defines the sub stream protocol.*/
/**CNcomment: ����������Э���֧ö��*/
typedef enum hiUNF_VIDEO_SUB_STANDARD_E
{
    HI_UNF_VIDEO_SUB_STANDARD_UNKNOWN,       /**<Unknown*/ /**<CNcomment: δ֪Э���֧*/
    HI_UNF_VIDEO_SUB_STANDARD_MPEG2_MPEG1,   /**<The MPEG2 protocol family is compatible with MPEG1.*/ /**<CNcomment: MPEG2Э������Լ���MPEG1 */
    HI_UNF_VIDEO_SUB_STANDARD_MPEG4_SHV,     /**<The MPEG4 protocol family is compatible with SHV.*/ /**<CNcomment: MPEG4Э������Լ���SHV*/
    HI_UNF_VIDEO_SUB_STANDARD_MPEG4_XVID,    /**<The MPEG4 protocol family includes the sub protocol XVID.*/ /**<CNcomment: MPEG4Э���XVID��֧*/
    HI_UNF_VIDEO_SUB_STANDARD_MPEG4_DIVX,    /**<The MPEG4 protocol family includes the sub protocol DIVX.*/ /**<CNcomment: MPEG4Э���DIVX��֧*/

    HI_UNF_VIDEO_SUB_STANDARD_BUTT
}HI_UNF_VIDEO_SUB_STANDARD_E;

/**Defines the frame rate of the video stream.*/
/**CNcomment: ������Ƶ����֡�ʽṹ*/
typedef struct hiUNF_VCODEC_FRMRATE_S
{
    HI_U32 u32fpsInteger;     /**<Integral part of the frame rate (in frame/s)*/ /**<CNcomment: ������֡�ʵ������, fps */
    HI_U32 u32fpsDecimal;     /**<Fractional part (calculated to three decimal places) of the frame rate (in frame/s)*/
                              /**<CNcomment: ������֡�ʵ�С��֣�����3λ��, fps */
}HI_UNF_VCODEC_FRMRATE_S;

/**Defines the information about video streams.*/
/**CNcomment: ������Ƶ������Ϣ�ṹ*/
typedef struct hiUNF_VCODEC_STREAMINFO_S
{
    HI_UNF_VCODEC_TYPE_E        enVCodecType;   /**<Stream type*/ /**<CNcomment: �������� */
    HI_UNF_VIDEO_SUB_STANDARD_E enSubStandard;  /**<Sub stream protocol*/ /**<CNcomment: ������Э���֧ */
    HI_U32                      u32SubVersion;  /**<Version of the sub stream protocol*/ /**<CNcomment: ������Э��汾�� */
    HI_U32                      u32Profile;     /**<Stream profile*/ /**<CNcomment: ������profile */
    HI_U32                      u32Level;       /**<Stream level*/ /**<CNcomment: ������level */
    HI_UNF_ENC_FMT_E            enDisplayNorm;  /**<Display norm (PAL or NTSC)*/ /**<CNcomment: ��ʾ��׼(P/N) */
    HI_BOOL                     bProgressive;   /**<Sampling type (progressive or interlaced)*/ /**<CNcomment: ����ʽ(����/����) */
    HI_U32                      u32AspectWidth; /**<Output aspect ratio: width*/ /**<CNcomment: �����߱�֮��ֵ */
    HI_U32                      u32AspectHeight;/**<Output aspect ratio: height*/ /**<CNcomment: �����߱�֮��ֵ */

    HI_U32 u32bps;            /**<Bit rate, in kbit/s*/ /**<CNcomment: ����������, Kbps */
    HI_U32 u32fpsInteger;     /**<Integral part of the frame rate (in frame/s)*/ /**<CNcomment: ������֡�ʵ������, fps */
    HI_U32 u32fpsDecimal;     /**<Fractional part (calculated to three decimal places) of the frame rate (in frame/s)*/ /**<CNcomment: ������֡�ʵ�С��֣�����3λ��, fps */
    HI_U32 u32Width;          /**<Width of the decoded picture*/ /**<CNcomment: ����ͼ��� */
    HI_U32 u32Height;         /**<Height of the decoded picture*/ /**<CNcomment: ����ͼ��� */
    HI_U32 u32DisplayWidth;   /**<Width of the displayed picture*/ /**<CNcomment: ��ʾͼ��� */
    HI_U32 u32DisplayHeight;  /**<Height of the displayed picture*/ /**<CNcomment: ��ʾͼ��� */
    HI_U32 u32DisplayCenterX; /**<Horizontal coordinate of the center of the displayed picture (the upper left point of the source picture serves as the coordinate origin)*/
                              /**<CNcomment: ��ʾͼ�����ĺ���꣬��ԭʼͼ������Ͻ�Ϊ���ԭ�� */
    HI_U32 u32DisplayCenterY; /**<Vertical coordinate of the center of the displayed picture (the upper left point of the source picture serves as the coordinate origin)*/
                              /**<CNcomment: ��ʾͼ����������꣬��ԭʼͼ������Ͻ�Ϊ���ԭ�� */
}HI_UNF_VCODEC_STREAMINFO_S;


/**Defines the types of the user data in the video information.*/
/**CNcomment: ������Ƶ��Ϣ�е��û�������� */
typedef enum hiHI_UNF_VIDEO_USERDATA_TYPE_E
{
    HI_UNF_VIDEO_USERDATA_UNKNOWN = 0,      /**<Unknown type*/ /**<CNcomment: δ֪����*/
    HI_UNF_VIDEO_USERDATA_DVB1_CC = 0x1,    /**<Closed Caption Data*/ /**<CNcomment: ��Ļ���*/
    HI_UNF_VIDEO_USERDATA_DVB1_BAR = 0x2,   /**<Bar Data*/ /**<CNcomment: Bar���*/
    HI_UNF_VIDEO_USERDATA_AFD = 0x10000,    /**<Active Format Description*/ /**<CNcomment: ģʽ�������*/
    HI_UNF_VIDEO_USERDATA_BUTT
}HI_UNF_VIDEO_USERDATA_TYPE_E;

/**Defines the profile of video broadcasting.*/
/**CNcomment: ������Ƶ�㲥��profileö��*/
typedef enum hiUNF_VIDEO_BROADCAST_PROFILE_E
{
    HI_UNF_VIDEO_BROADCAST_DVB,        /**<Digital video broadcasting (DVB)*/ /**<CNcomment: ������Ƶ�㲥DVB*/
    HI_UNF_VIDEO_BROADCAST_DIRECTV,    /**<American live broadcast operator DirecTV*/ /**<CNcomment: ����ֱ����Ӫ��DirecTV*/
    HI_UNF_VIDEO_BROADCAST_ATSC,       /**<Advanced Television Systems Committee (ATSC)*/ /**<CNcomment: �Ƚ������ʽίԱ��ATSC��Advanced Television Systems Committee��*/
    HI_UNF_VIDEO_BROADCAST_DVD,        /**<Digital video disc (DVD)*/ /**<CNcomment: ������Ƶ����*/
    HI_UNF_VIDEO_BROADCAST_ARIB,       /**<Association of Radio Industries and Businesses (ARIB)*/ /**<CNcomment: ���ߵ繤ҵ����ҵЭ����*/
    HI_UNF_VIDEO_BROADCAST_BUTT
}HI_UNF_VIDEO_BROADCAST_PROFILE_E;

/**Defines the position of the user data in the video information.*/
/**CNcomment: ������Ƶ��Ϣ���û���ݵ�λ��ö��*/
typedef enum hiUNF_VIDEO_USER_DATA_POSITION_E
{
    HI_UNF_VIDEO_USER_DATA_POSITION_UNKNOWN,       /**<Unknown*/ /**<CNcomment: δ֪λ��*/
    HI_UNF_VIDEO_USER_DATA_POSITION_MPEG2_SEQ,     /**<The data is parsed from sequences under the MPEG2 protocol.*/ /**<CNcomment: MPEG2Э���£��������н��*/
    HI_UNF_VIDEO_USER_DATA_POSITION_MPEG2_GOP,     /**<The data is parsed from the group of pictures (GOP) under the MPEG2 protocol.*/ /**<CNcomment: MPEG2Э���£���GOP��Group Of Pictures���н��*/
    HI_UNF_VIDEO_USER_DATA_POSITION_MPEG2_FRAME,   /**<The data is parsed from picture frames under the MPEG2 protocol.*/ /**<CNcomment: MPEG2Э���£���ͼ��֡�н��*/
    HI_UNF_VIDEO_USER_DATA_POSITION_MPEG4_VSOS,    /**<The data is parsed from the sequences of visible objects under the MPEG4 protocol.*/ /**<CNcomment: MPEG4Э���£��ӿ��Ӷ��������н��*/
    HI_UNF_VIDEO_USER_DATA_POSITION_MPEG4_VSO,     /**<The data is parsed from visible objects under the MPEG4 protocol.*/ /**<CNcomment: MPEG4Э���£��ӿ��Ӷ����н��*/
    HI_UNF_VIDEO_USER_DATA_POSITION_MPEG4_VOL,     /**<The data is parsed from the video object layer under the MPEG4 protocol.*/ /**<CNcomment: MPEG4Э���£�����Ƶ������н��*/
    HI_UNF_VIDEO_USER_DATA_POSITION_MPEG4_GOP,     /**<The data is parsed from the GOP under the MPEG4 protocol.*/ /**<CNcomment: MPEG4Э���£���GOP�н��*/
    HI_UNF_VIDEO_USER_DATA_POSITION_H264_REG,      /**<The data is parsed from the user_data_regestered_itu_t_t35() syntax under the H.264 protocol.*/ /**<CNcomment: ��H.264Э���user_data_regestered_itu_t_t35()�﷨�н��*/
    HI_UNF_VIDEO_USER_DATA_POSITION_H264_UNREG,    /**<The data is parsed from the user_data_unregestered() syntax under the H.264 protocol.*/ /**<CNcomment: ��H.264Э���user_data_unregestered()�﷨�н��*/
    HI_UNF_VIDEO_USER_DATA_POSITION_BUTT
}HI_UNF_VIDEO_USER_DATA_POSITION_E;

/**Defines the structure of the user data in the video information.*/
/**CNcomment: ������Ƶ��Ϣ�е��û���ݽṹ*/
typedef struct hiUNF_VIDEO_USERDATA_S
{
    HI_UNF_VIDEO_BROADCAST_PROFILE_E   enBroadcastProfile;   /**<Broadcasting profile of the user data*/ /**<CNcomment: �û���ݵĹ㲥profile*/
    HI_UNF_VIDEO_USER_DATA_POSITION_E  enPositionInStream;   /**<Position of the user data in video streams*/ /**<CNcomment: �û��������Ƶ���е�λ��*/
    HI_U32                             u32Pts;               /**<PTS corresponding to the user data*/ /**<CNcomment: �û���ݶ�Ӧ��ʱ���*/
    HI_U32                             u32SeqCnt;            /**<Sequence ID of the user data*/ /**<CNcomment: �û���ݵ�ǰһ������*/
    HI_U32                             u32SeqFrameCnt;       /**<Frame number of the user data*/ /**<CNcomment: �û���ݵ�ǰһ֡��*/
    HI_U8                              *pu8Buffer;           /**<Initial address of the user data memory, output parameter*/ /**<CNcomment: �û�����ڴ���ĳ�ʼ��ַ,�������*/
    HI_U32                             u32Length;            /**<User data size (a multiple of 1 byte)*/ /**<CNcomment: �û���ݵĴ�С����1byteΪ��λ*/
    HI_BOOL                            bBufferOverflow;      /**<Indicate that whether the user data size exceeds the maximum size defined by MAX_USER_DATA_LEN.*/ /**<CNcomment: ��־�û���ݵĳ����Ƿ񳬹���MAX_USER_DATA_LEN��������ֵ*/
}HI_UNF_VIDEO_USERDATA_S;

/**Defines the parameters of format changing */
/**CNcomment: ������ʽ�л�ʱ����ز���*/
typedef struct hiUNF_NORMCHANGE_PARAM_S
{
    HI_UNF_ENC_FMT_E            enNewFormat;			/**<New format*/ /**<CNcomment: �µ���ʽ*/
    HI_U32                      u32ImageWidth;			/**<Width of image*/ /**<CNcomment: ͼ����*/
    HI_U32                      u32ImageHeight;			/**<Height of image*/ /**<CNcomment: ͼ��߶�*/
    HI_BOOL                     bProgressive;           /**<Sampling type (progressive or interlaced)*/ /**<CNcomment: ����ʽ(����/����) */
    HI_U32                      u32FrameRate;			/**<Frame rate*//**<CNcomment:֡��*/
}HI_UNF_NORMCHANGE_PARAM_S;

/**Defines the video format.*/
/**CNcomment: ������Ƶ��ʽö��*/
typedef enum hiUNF_VIDEO_FORMAT_E
{
    /* Semi-Planner */
    HI_UNF_FORMAT_YUV_SEMIPLANAR_422,       /**<The YUV spatial sampling format is 4:2:2.*/ /**<CNcomment: YUV�ռ�����ʽΪ4:2:2*/   
    HI_UNF_FORMAT_YUV_SEMIPLANAR_420,       /**<The YUV spatial sampling format is 4:2:0, V first.*/ /**<CNcomment: YUV�ռ�����ʽΪ4:2:0��V�ڵ�λ*/
    HI_UNF_FORMAT_YUV_SEMIPLANAR_400,
    HI_UNF_FORMAT_YUV_SEMIPLANAR_411,
    HI_UNF_FORMAT_YUV_SEMIPLANAR_422_1X2,
    HI_UNF_FORMAT_YUV_SEMIPLANAR_444,
    HI_UNF_FORMAT_YUV_SEMIPLANAR_420_UV,   /**<The YUV spatial sampling format is 4:2:0,U first.*/ /**<CNcomment: YUV�ռ�����ʽΪ4:2:0, U�ڵ�λ*/


    /* Package */
    HI_UNF_FORMAT_YUV_PACKAGE_UYVY,         /**<The YUV spatial sampling format is package, and the pixel arrangement sequence in the memory is UYVY.*/ /**<CNcomment: YUV�ռ�����ʽΪpackage,�ڴ�����ΪUYVY*/
    HI_UNF_FORMAT_YUV_PACKAGE_YUYV,         /**<The YUV spatial sampling format is package, and the pixel arrangement sequence in the memory is YUYV.*/ /**<CNcomment: YUV�ռ�����ʽΪpackage,�ڴ�����ΪYUYV*/
    HI_UNF_FORMAT_YUV_PACKAGE_YVYU,         /**<The YUV spatial sampling format is package, and the pixel arrangement sequence in the memory is YVYU.*/ /**<CNcomment: YUV�ռ�����ʽΪpackage,�ڴ�����ΪYVYU*/

    /* Planner */
    HI_UNF_FORMAT_YUV_PLANAR_400,
    HI_UNF_FORMAT_YUV_PLANAR_411,
    HI_UNF_FORMAT_YUV_PLANAR_420,
    HI_UNF_FORMAT_YUV_PLANAR_422_1X2,
    HI_UNF_FORMAT_YUV_PLANAR_422_2X1,
    HI_UNF_FORMAT_YUV_PLANAR_444,
    HI_UNF_FORMAT_YUV_PLANAR_410,
    HI_UNF_FORMAT_YUV_BUTT
}HI_UNF_VIDEO_FORMAT_E;

/**Defines the type of the video frame.*/
/**CNcomment: ������Ƶ֡������ö��*/
typedef enum hiUNF_VIDEO_FRAME_TYPE_E
{
    HI_UNF_FRAME_TYPE_UNKNOWN,   /**<Unknown*/ /**<CNcomment: δ֪��֡����*/
    HI_UNF_FRAME_TYPE_I,         /**<I frame*/ /**<CNcomment: I֡*/
    HI_UNF_FRAME_TYPE_P,         /**<P frame*/ /**<CNcomment: P֡*/
    HI_UNF_FRAME_TYPE_B,         /**<B frame*/ /**<CNcomment: B֡*/
    HI_UNF_FRAME_TYPE_BUTT
}HI_UNF_VIDEO_FRAME_TYPE_E;

/**Defines the video frame/field mode.*/
/**CNcomment: ������Ƶ֡��ģʽö��*/
typedef enum hiUNF_VIDEO_FIELD_MODE_E
{
    HI_UNF_VIDEO_FIELD_ALL,        /**<Frame mode*/ /**<CNcomment: ֡ģʽ*/
    HI_UNF_VIDEO_FIELD_TOP,        /**<Top field mode*/ /**<CNcomment: ����ģʽ*/
    HI_UNF_VIDEO_FIELD_BOTTOM,     /**<Bottom field mode*/ /**<CNcomment: �׳�ģʽ*/
    HI_UNF_VIDEO_FIELD_BUTT
}HI_UNF_VIDEO_FIELD_MODE_E;


typedef struct hiUNF_CAPTURE_MEM_MODE_S
{
    HI_U32			    u32StartPhyAddr;	    /**<start phy addr*/ /**<CNcomment: ��ʼ�����ַ*/
    HI_U32			    u32StartUserAddr;       /**<start user addr*/ /**<CNcomment: ��ʼ�û���ַ*/
    HI_U32			    u32DataLen;		        /**<len of databuf*/ /**<CNcomment: ������*/
}HI_UNF_CAPTURE_MEM_MODE_S;


/**Defines 3D frame packing type*/
typedef enum hiUNF_VIDEO_FRAME_PACKING_TYPE_E
{
    HI_UNF_FRAME_PACKING_TYPE_NONE,             /**< Normal frame, not a 3D frame */
    HI_UNF_FRAME_PACKING_TYPE_SIDE_BY_SIDE,     /**< Side by side */
    HI_UNF_FRAME_PACKING_TYPE_TOP_AND_BOTTOM,   /**< Top and bottom */
    HI_UNF_FRAME_PACKING_TYPE_TIME_INTERLACED,  /**< Time interlaced: one frame for left eye, the next frame for right eye */
    HI_UNF_FRAME_PACKING_TYPE_BUTT             
}HI_UNF_VIDEO_FRAME_PACKING_TYPE_E;

typedef struct hiUNF_VIDEO_FRAME_ADDR_S
{
    HI_U32             u32YAddr;    /**<Address of the Y component in the current frame*/ /**<CNcomment: ��ǰ֡Y������ݵĵ�ַ*/
    HI_U32             u32CAddr;    /**<Address of the C component in the current frame*/ /**<CNcomment: ��ǰ֡C������ݵĵ�ַ*/
    HI_U32             u32CrAddr;   /**<Address of the Cr component in the current frame*/ /**<CNcomment: ��ǰ֡Cr������ݵĵ�ַ*/

    HI_U32             u32YStride;  /**<Stride of the Y component*/ /**<CNcomment: Y������ݵĿ��*/
    HI_U32             u32CStride;  /**<Stride of the C component*/ /**<CNcomment: C������ݵĿ��*/
    HI_U32             u32CrStride; /**<Stride of the Cr component*/ /**<CNcomment: Cr������ݵĿ��*/
}HI_UNF_VIDEO_FRAME_ADDR_S;

typedef struct hiUNF_VIDEO_FRAME_INFO_S
{
    HI_U32                              u32FrameIndex;      /**<Frame index ID of a video sequence*/ /**<CNcomment: ��Ƶ�����е�֡�����*/
    HI_UNF_VIDEO_FRAME_ADDR_S           stVideoFrameAddr[2];
    HI_U32                              u32Width;           /**<Width of the source picture*/ /**<CNcomment: ԭʼͼ���*/
    HI_U32                              u32Height;          /**<Height of the source picture*/ /**<CNcomment: ԭʼͼ���*/
    HI_U32                              u32SrcPts;          /**<Original PTS of a video frame*/ /**<CNcomment: ��Ƶ֡��ԭʼʱ���*/
    HI_U32                              u32Pts;             /**<PTS of a video frame*/ /**<CNcomment: ��Ƶ֡��ʱ���*/
    HI_U32                              u32AspectWidth;
    HI_U32                              u32AspectHeight; 
    HI_UNF_VCODEC_FRMRATE_S             stFrameRate;

    HI_UNF_VIDEO_FORMAT_E               enVideoFormat;      /**<Video YUV format*/ /**<CNcomment: ��ƵYUV��ʽ*/
    HI_BOOL                             bProgressive;       /**<Sampling type (progressive or interlaced)*/ /**<CNcomment: ����ʽ(����/����) */
    HI_UNF_VIDEO_FIELD_MODE_E           enFieldMode;        /**<Frame or field encoding mode*/ /**<CNcomment: ֡�򳡱���ģʽ*/
    HI_BOOL                             bTopFieldFirst;     /**<Top field first flag*/ /**<CNcomment: �������ȱ�־*/
    HI_UNF_VIDEO_FRAME_PACKING_TYPE_E   enFramePackingType; /**<3D frame packing type*/
    HI_U32                              u32Circumrotate;    /**<Need circumrotate, 1 need */ 
    HI_BOOL                             bVerticalMirror;
    HI_BOOL                             bHorizontalMirror;
    HI_U32                              u32DisplayWidth;    /**<Width of the displayed picture*/ /**<CNcomment: ��ʾͼ���*/
    HI_U32                              u32DisplayHeight;   /**<Height of the displayed picture*/ /**<CNcomment: ��ʾͼ���*/
    HI_U32                              u32DisplayCenterX;  /**<Horizontal coordinate of the center of the displayed picture (the upper left point of the source picture serves as the coordinate origin)*/ /**<CNcomment: ��ʾ����x��꣬ԭʼͼ�����Ͻ�Ϊ���ԭ��*/
    HI_U32                              u32DisplayCenterY;  /**<Vertical coordinate of the center of the displayed picture (the upper left point of the source picture serves as the coordinate origin)*/ /**<CNcomment: ��ʾ����y��꣬ԭʼͼ�����Ͻ�Ϊ���ԭ��*/
    HI_U32                              u32ErrorLevel;      /**<Error percentage of a decoded picture, ranging from 0% to 100%*/ /**<CNcomment: һ�����ͼ���еĴ������ȡֵΪ0��100*/
    HI_U32                              u32Private[64];
}HI_UNF_VIDEO_FRAME_INFO_S;

/** @} */  /** <!-- ==== Structure Definition End ==== */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HI_UNF_VIDEO_ H*/
