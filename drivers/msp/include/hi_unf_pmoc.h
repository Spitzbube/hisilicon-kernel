/******************************************************************************

  Copyright (C), 2001-2011, HiSilicon Technologies Co., Ltd.
 ******************************************************************************
File Name     : hi_unf_pmoc.h
Version       : Initial draft
Author        : HiSilicon multimedia software group
Created Date  : 2012-08-24
Last Modified by:
Description   : Application programming interfaces (APIs) of the PMOC
Function List :
Change History:
 ******************************************************************************/

#ifndef __HI_UNF_PMOC_H__
#define __HI_UNF_PMOC_H__

#include "hi_common.h"
#include "hi_error_mpi.h"

#ifdef HI_KEYLED_SUPPORT
#include "hi_unf_keyled.h"
#endif

#include "hi_unf_ir.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/*************************** Structure Definition ****************************/
/** \addtogroup      PMOC */
/** @{ */  /** <!-- [PMOC] */

/**ARM mode*/
/**CNcomment:ARM所处的模式*/
typedef enum hiUNF_PMOC_MODE_E
{
    HI_UNF_PMOC_MODE_NORMAL = 0,    /**<Normal*/     /**<CNcomment:普通模式 */
    HI_UNF_PMOC_MODE_SLOW,          /**<Slow*/       /**<CNcomment:慢速模式 */
    HI_UNF_PMOC_MODE_DOZE,          /**<Doze*/       /**<CNcomment:低速模式 */
    HI_UNF_PMOC_MODE_SLEEP,         /**<Sleep*/      /**<CNcomment:睡眠模式 */

    HI_UNF_PMOC_MODE_BUTT
}HI_UNF_PMOC_MODE_E;


/**standby woken type*/
/**CNcomment:待机被唤醒的类型 */
typedef enum hiUNF_PMOC_ACTUAL_WKUP_E
{
    HI_UNF_PMOC_WKUP_IR = 0,      /**<Woken by the IR module*/                /**<CNcomment:被IR唤醒 */
    HI_UNF_PMOC_WKUP_KEYLED,      /**<Woken by the keys on the front panel*/  /**<CNcomment:被前面板KEY唤醒 */
    HI_UNF_PMOC_WKUP_TIMEOUT,     /**<Woken by the timing interrupt*/         /**<CNcomment:被定时中断唤醒 */
    HI_UNF_PMOC_WKUP_ETH,         /**<Woken by the ethernet interrupt*/       /**<CNcomment:被网络中断唤醒 */
    HI_UNF_PMOC_WKUP_USB,         /**<Woken by the USB interrupt*/            /**<CNcomment:被USB鼠标键盘中断唤醒 */
    HI_UNF_PMOC_WKUP_BUTT
}HI_UNF_PMOC_ACTUAL_WKUP_E;

typedef enum hiUNF_PMOC_SCENE_E
{
    HI_UNF_PMOC_SCENE_STANDARD = 0,  /**<Standard scenario*/                    /**<CNcomment:标准场景 */
    HI_UNF_PMOC_SCENE_ETH,           /**<Forward scenario over the ETH port */  /**<CNcomment:网口转发场景 */
    HI_UNF_PMOC_SCENE_PHONE,         /**<Calling scenario*/                     /**<CNcomment:通话场景 */
    HI_UNF_PMOC_SCENE_BUTT
}HI_UNF_PMOC_SCENE_E;

typedef enum hiUNF_PMOC_ETH_E
{
    HI_UNF_PMOC_ETH_0 = 0x01,        /**<The first ETH.*/    /**<CNcomment:第1个网口*/
    HI_UNF_PMOC_ETH_1 = 0x02,        /**<The second ETH.*/   /**<CNcomment:第2个网口*/    
    HI_UNF_PMOC_ETH_BUTT = 0x04
}HI_UNF_PMOC_ETH_E;


#define PMOC_WKUP_IRKEY_MAXNUM 6
#define FILTER_VALUE_COUNT   (31)
#define FILTER_COUNT         (4)

typedef struct hiUNF_PMOC_WAKEUP_FRAME
{
    HI_U32	u32MaskBytes;  /**<Mask byte, bitN to control u8Value[N]. 0: invalid, 1: valid*/  /**<CNcomment: 对应value的掩码, bitN对应u8Value[N], 0代表无效，1代表有效 */
    HI_U8	u8Offset;      /**<Filter offset, should be bigger than or equal to 12*/          /**<CNcomment: 过滤器偏移量。需要大于或等于12*/
    HI_U8	u8Value[FILTER_VALUE_COUNT];   /**<Filter value*/                                 /**<CNcomment: 过滤器的值*/
    HI_BOOL	bFilterValid;  /**<Valid filter, 0: invalid, 1: valid*/                           /**<CNcomment: 过滤器是否有效，0代表无效，1代表有效*/
}HI_UNF_PMOC_WAKEUP_FRAME_S, *HI_UNF_WAKEUP_FRAME_S_PTR;

typedef struct hiUNF_PMOC_NETWORK
{
    HI_UNF_PMOC_ETH_E		   enEthIndex;         /**<Eth index, can set several eth once*/  /**<CNcomment: 网口序号，可以同时设置多个网口*/
    HI_BOOL					   bUcPacketEnable;    /**<Single packet enable*/                 /**<CNcomment: 单播包使能 */ 
    HI_BOOL					   bMagicPacketEnable; /**<Magic packet enable*/                  /**<CNcomment: 魔法包使能*/
    HI_BOOL					   bWakeupFrameEnable; /**<Wakeup Frame enable*/                  /**<CNcomment: 唤醒帧使能 */
    HI_UNF_PMOC_WAKEUP_FRAME_S stFrame[FILTER_COUNT];         /**<Filter frame*/              /**<CNcomment: 唤醒帧数据 */
}HI_UNF_PMOC_NETWORK_S, *HI_UNF_PMOC_NETWORK_S_PTR;

/**Defines the standby wake-up conditions.*/
/**CNcomment:待机唤醒的条件定义*/
typedef struct hiUNF_PMOC_WKUP_S
{
    /**<Number of supported values of the power key. For the raw IR remote control, the number cannot be greater than the maximum key value 6 that is defined by the macro definition PMOC_WKUP_IRKEY_MAXNUM. Only one key value is supported for other remote controls.*/
    /**<CNcomment:能够支持power键值个数，raw 型红外遥控: 不能超越PMOC_WKUP_IRKEY_MAXNUM(<=6)宏定义所述个数键值，其余类型只能支持一个键值*/
    HI_U32 u32IrPmocNum;

    HI_U32 u32IrPowerKey0[PMOC_WKUP_IRKEY_MAXNUM];  /**<Lower-bit value of the power key on an IR remote control*/	/**<CNcomment:红外遥控 power 低位键值 */
    HI_U32 u32IrPowerKey1[PMOC_WKUP_IRKEY_MAXNUM];  /**<Upper-bit value of the power key on an IR remote control*/	/**<CNcomment:红外遥控 power 高位键值 */
	HI_U32 u32IrSymbol[64];                         /**<Value of the raw power key of IR*/                          /**<CNcomment:红外遥控唤醒原始电平对 */
	HI_U32 u32KeypadPowerKey;                           /**<Value of the power key*/                                    /**<CNcomment:按键 power键值 */
    HI_U32 u32WakeUpTime;                               /**<Preconfigured Wake-up time, in second*/                     /**<CNcomment:唤醒定时时间 ,单位为 秒  */
	HI_UNF_PMOC_NETWORK_S stNetwork;                    /**<Network parameter*/                                         /**<CNcomment:网络唤醒参数*/
    HI_BOOL  bMouseKeyboardEnable;                      /**<Mouse and key wake-up enable*/                              /**<CNcomment:鼠标键盘唤醒使能*/
}HI_UNF_PMOC_WKUP_S, *HI_UNF_PMOC_WKUP_S_PTR;

/**Configures the time displayed on the front panel in standby mode.*/
/**CNcomment:待机时的前面板显示的时间配置 */
typedef struct hiUNF_PMOC_TIME_S
{
    HI_U32 u32Hour;            /**<Hour*/     /**<CNcomment:时 */
    HI_U32 u32Minute;          /**<Minute*/   /**<CNcomment:分 */
    HI_U32 u32Second;          /**<Second*/   /**<CNcomment:秒 */
}HI_UNF_PMOC_TIME_S, *HI_UNF_PMOC_TIME_S_PTR;

/**Configures the display mode of the front panel in standby mode.*/
/**CNcomment:待机时的前面板显示配置 */
typedef struct hiUNF_PMOC_STANDBY_MODE_S
{
    HI_U32			   u32Mode;      /**<0: no display; 1: display the digits represented by u32DispCod; 2: display the time represented by stTimeInfo*/     /**<CNcomment:0 : 无显示 1 : 显示u32DispCode代表的数字; 2 : 显示stTimeInfo代表的时间. */
    HI_U32			   u32DispCode;  /**<Digits displayed on the front panel when u32Mode is 1, value range from 0000 to 9999, do not support hex */         /**<CNcomment:u32Mode为1时，前面板显示的数字，取值范围从0000到9999，不支持16进制数 */
    HI_UNF_PMOC_TIME_S stTimeInfo; /**<Time displayed on the front panel when u32Mode is 2*/	                                                             /**<CNcomment:u32Mode为2时，前面板显示的时间*/
}HI_UNF_PMOC_STANDBY_MODE_S, *HI_UNF_PMOC_STANDBY_MODE_S_PTR;

/**Type of the device that is woken in standby mode*/
/**CNcomment:待机唤醒的设备类型 */
typedef struct hiUNF_PMOC_DEV_TYPE_S
{
    HI_UNF_IR_CODE_E	 irtype;    /**<Type of the IR remote control*/	 /**<CNcomment:红外遥控类型 */
#ifdef HI_KEYLED_SUPPORT
    HI_UNF_KEYLED_TYPE_E kltype;  /**<Type of the front panel*/        /**<CNcomment:前面板类型 */
#endif
}HI_UNF_PMOC_DEV_TYPE_S, *HI_UNF_PMOC_DEV_TYPE_S_PTR;

/** @} */  /** <!-- ==== Structure Definition End ==== */

/******************************* API Declaration *****************************/
/** \addtogroup      PMOC */
/** @{ */  /** <!-- [PMOC] */
/*---- pm ----*/

/**
\brief Starts the power management on chip (PMoC) device.
CNcomment:\brief 打开低功耗模块设备。CNend

\attention \n
The ARM can be switched to another operating mode and then to low-power mode only after the PMoC device is started.\n
CNcomment:在打开低功耗模块设备后，才能切换ARM的工作模式，继而进入低功耗状态。CNend

\param N/A                                                               CNcomment:无 CNend
\retval HI_SUCCESS Success                                               CNcomment:成功 CNend
\retval ::HI_ERR_PMOC_FAILED_INIT  The PMoC device fails to open.        CNcomment:PMoC设备打开失败 CNend
\see \n
N/A
*/
HI_S32 HI_UNF_PMOC_Init(HI_VOID);

/**
\brief Stops the PMoC device.
CNcomment:\brief 关闭低功耗设备。CNend

\attention \n
N/A
\param  N/A                                                             CNcomment:无 CNend
\retval HI_SUCCESS Success                                              CNcomment:成功 CNend
\retval ::HI_ERR_PMOC_NOT_INIT  The PMoC device is not started.         CNcomment:设备未打开 CNend
\see \n
N/A
*/
HI_S32 HI_UNF_PMOC_DeInit(HI_VOID);

/**
\brief Sets the mode to which the ARM is switched and obtains the wake-up mode.
CNcomment:\brief 设置ARM要切换到的模式，同时获取ARM唤醒的方式。CNend

\attention \n
If the ARM enters the HI_UNF_PMOC_MODE_SLEEP mode, programs stop running at once. The subsequent code can be executed only after the ARM is woken up.\n
Before enabling the standby mode, you must start the IR device, key device on the front panel; otherwise, the interrupts of these devices are masked. As a result, the corresponding wake-up modes cannot be achieved.\n
Do not access DDRs after the ARM enters the sleep mode. Therefore, the functions of display output, video output, DEMUX input, and audio/video decoding must be disabled.\n
CNcomment:ARM进入HI_UNF_PMOC_MODE_SLEEP真待机模式后程序立即停止运行，必须在唤醒后才能继续执行后面的代码\n 
进入待机模式前，必须打开IR（遥控器）设备、前面板KEY设备，否则对应模块的中断被屏蔽，不能实现该种模式的唤醒\n 
进入真待机时，必须确保不对DDR进行访问。所以必须关闭显示输出、视频输出、DEMUX输入、音视频解码等。CNend

\param[in] enSystemMode   Mode of the ARM in low-power mode                CNcomment: 置低功耗下ARM所处模式。CNend

\param[out]  penWakeUpStatus Wake-up mode returned from the HI_UNF_PMOC_MODE_SLEEP mode. For details about the definition, see the description of ::HI_UNF_PMOC_ACTUAL_WKUP_E. \n
                         CNcomment: 从HI_UNF_PMOC_MODE_SLEEP状态返回时的唤醒方式。具体含义请参考::HI_UNF_PMOC_ACTUAL_WKUP_E CNend
\retval HI_SUCCESS Success                                                 CNcomment:成功 CNend
\retval ::HI_ERR_PMOC_NOT_INIT   The PMoC device is not started.           CNcomment:PMoC设备未打开 CNend
\retval ::HI_ERR_PMOC_INVALID_PARA  The parameter is invalid.              CNcomment:非法参数 CNend
\retval ::HI_ERR_PMOC_FAILED_STANDBY  It fails to enter standby            CNcomment:进入待机失败 CNend
\see \n
N/A
*/
HI_S32 HI_UNF_PMOC_SwitchSystemMode(HI_UNF_PMOC_MODE_E enSystemMode, HI_UNF_PMOC_ACTUAL_WKUP_E * penWakeUpStatus);

/**
\brief Sets the wake-up mode of the ARM.
CNcomment:\brief 设置ARM唤醒方式。CNend

\attention \n
You can query the original IR code values by referring to IR remote manuals.\n
In addition, you can query the relationships between the keys and key IDs by checking the connection between hardware and chips.\n
CNcomment:IR原始码值可以通过遥控器的使用手册得到\n
按键与键值的对应关系可以通过硬件和芯片的连接关系得到。CNend

\param[in] pstAttr  Wake-up mode of the ARM                             CNcomment:ARM唤醒方式。CNend
\retval HI_SUCCESS Success                                              CNcomment:成功 CNend
\retval ::HI_ERR_PMOC_NOT_INIT  The PMoC device is not started.         CNcomment:PMoC设备未打开 CNend
\retval ::HI_ERR_PMOC_INVALID_POINT  The pointer is invalid.            CNcomment:非法指针 CNend
\retval ::HI_ERR_PMOC_INVALID_PARA  The parameter is invalid.           CNcomment:非法参数 CNend
\retval ::HI_ERR_PMOC_FAILED_IRPOWERVAL It fails to set power val.      CNcomment:设置唤醒方式失败 CNend
\see \n
N/A
*/
HI_S32 HI_UNF_PMOC_SetWakeUpAttr(HI_UNF_PMOC_WKUP_S_PTR pstAttr);

/**
\brief set display content when wake up.
CNcomment:\brief 设置唤醒显示内容。CNend

\attention \n
content that the panel display when standby
CNcomment:待机时面板显示指定内容\n CNend

\param[in] pstStandbyMode  display content:time or channel or no display   CNcomment:显示内容:时间 or 频道 or 无显示。CNend
\retval HI_SUCCESS  success                                                CNcomment:成功 CNend 
\retval ::HI_ERR_PMOC_NOT_INIT  The PMoC device is not started.            CNcomment:PMoC设备未打开 CNend
\retval ::HI_ERR_PMOC_INVALID_POINT  The pointer is invalid.               CNcomment:非法指针 CNend
\retval ::HI_ERR_PMOC_INVALID_PARA  The parameter is invalid.              CNcomment:非法参数 CNend
\retval ::HI_ERR_PMOC_FAILED_STANDBY  It fails to set standby display      CNcomment:设置唤醒内容失败 CNend
\see \n
N/A
*/
HI_S32 HI_UNF_PMOC_SetStandbyDispMode(HI_UNF_PMOC_STANDBY_MODE_S_PTR pstStandbyMode);

/**
\brief Obtains the current system mode.
CNcomment:\brief 获取系统当前模式。CNend

\attention \n
This API is abandoned and only used for forward compatible. It always returns normal state.\n
CNcomment:此接口已废弃，始终返回普通模式，仅为兼容之前版本使用。\n CNend

\param[in] penSystemMode  Mode to be obtained                   CNcomment:需要获取的模式。CNend
\retval HI_SUCCESS Success                                      CNcomment:成功 CNend
\retval ::HI_ERR_PMOC_NOT_INIT  The PMoC device is not started. CNcomment:PMoC设备未打开 CNend
\retval ::HI_ERR_PMOC_INVALID_POINT  The pointer is invalid.    CNcomment:非法指针 CNend
\see \n
N/A
*/
HI_S32 HI_UNF_PMOC_ReadSystemMode(HI_UNF_PMOC_MODE_E * penSystemMode);

/**
\brief Sets the operating scenario.
CNcomment:\brief 设置工作场景。CNend

\attention \n
Only the standby scenario and the forward scenario are supported currently.\n
CNcomment:目前仅支持标准待机场景和网口转发场景\n CNend

\param[in] eScene  Standby scenario                               CNcomment:待机场景。CNend
\retval HI_SUCCESS Success                                        CNcomment:成功 CNend
\retval ::HI_ERR_PMOC_NOT_INIT    The PMoC device is not started. CNcomment:PMoC设备未打开 CNend
\retval ::HI_ERR_PMOC_INVALID_POINT  The parameter is invalid.    CNcomment:非法参数 CNend
\see \n
N/A
*/
HI_S32 HI_UNF_PMOC_SetScene(HI_UNF_PMOC_SCENE_E eScene);

/**
\brief Sets the types of the devices related to standby such as front panel and IR remote control.
CNcomment:\brief 设置目前前面板同红外遥控等待机相关设备的类型。CNend

\attention \n

\param[in] pdevType  Standby device that needs to be supported in standby       CNcomment:待机时需要支持的待机设备。CNend
\retval HI_SUCCESS  success                                                CNcomment:成功 CNend
\retval ::HI_ERR_PMOC_NOT_INIT  The PMoC device is not started.            CNcomment:PMoC设备未打开 CNend
\retval ::HI_ERR_PMOC_INVALID_POINT  The pointer is invalid.               CNcomment:非法指针 CNend
\retval ::HI_ERR_PMOC_INVALID_PARA  The parameter is invalid.              CNcomment:非法参数 CNend
\retval ::HI_ERR_PMOC_FAILED_STANDBY  It fails to set standby devices.     CNcomment:设置唤醒设备失败 CNend
\see \n
N/A
*/
HI_S32 HI_UNF_PMOC_SetDevType(HI_UNF_PMOC_DEV_TYPE_S_PTR pdevType);

/**
\brief when standby, GPIO pin control 12v and 5v power,relate to hardware.
CNcomment:\brief 用于待机时,GPIO管脚控制12V0和5V0电源，与硬件相关。CNend

\attention \n
when standby, the outside power controled by STANDBY_PWROFF pin, the function can be ignored.\n
when some part of outside power controled by GPIO pin, use this function will decrease standby power.\n
CNcomment:待机时外围电源全部由管脚STANDBY_PWROFF控制时，不用理会该接口；\n
当部分外围电源由GPIO管脚去控制时,合理调用该接口用于降低待机功耗。CNend

\param[in] u32GpioNo  GPIO pin index, choose the fifth[40,47] CNcomment:GPIO管脚索引号,取第5组为[40, 47] CNend
\param[in] bHighOrLow GPIO output control                     CNcomment:GPIO管脚输出控制 CNend
\retval HI_SUCCESS  success                                                CNcomment:成功 CNend
\retval ::HI_ERR_PMOC_NOT_INIT  The PMoC device is not started.            CNcomment:PMoC设备未打开 CNend
\retval ::HI_ERR_PMOC_INVALID_POINT  The pointer is invalid.               CNcomment:非法指针 CNend
\retval ::HI_ERR_PMOC_INVALID_PARA  The parameter is invalid.              CNcomment:非法参数 CNend
\retval ::HI_ERR_PMOC_FAILED_STANDBY  It fails to set standby Gpio.        CNcomment:设置待机Gpio管脚失败 CNend
\see \n
N/A
*/
HI_S32 HI_UNF_PMOC_SetPwrOffGpio(HI_U32 u32GpioNo, HI_BOOL bHighOrLow);

/**
\brief Get the period during standby status.
CNcomment:\brief 获取从待机到唤醒持续的时间。CNend

\attention \n

\param[out] pu32Period  The period of standby. CNcomment:待机持续的时间 CNend
\retval ::HI_ERR_PMOC_NOT_INIT  The PMoC device is not started.            CNcomment:PMoC设备未打开 CNend
\retval ::HI_ERR_PMOC_INVALID_POINT  The pointer is invalid.               CNcomment:非法指针 CNend
\retval ::HI_ERR_PMOC_INVALID_PARA  The parameter is invalid.              CNcomment:非法参数 CNend
\retval ::HI_ERR_PMOC_FAILED_GETPERIOD  It fails to get standby period.    CNcomment:获取待机时间失败 CNend
\see \n
N/A
*/
HI_S32 HI_UNF_PMOC_GetStandbyPeriod(HI_U32 *pu32Period);


/** @} */  /** <!-- ==== API declaration end ==== */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HI_UNF_PMOC_H__ */
