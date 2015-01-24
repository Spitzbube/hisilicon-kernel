/* **************************************************** */
/*!
   @file	MN_DMD_Device.h
   @brief	Panasonic Demodulator Driver
   @author	R.Mori
   @date	2011/6/30
   @param
		(c)	Panasonic
   */
/* **************************************************** */
/*!
   this file defines common interface for each demodulator device
   */

#include "MN_DMD_driver.h"

#ifndef MN_DMD_DEVICE_H
#define MN_DMD_DEVICE_H

#ifdef __cplusplus
extern "C" {
#endif

//return DMD_E_OK , if device support the system & bandwidth
extern DMD_ERROR_t	DMD_system_support( DMD_SYSTEM_t sys );

/* **************************************************** */
/*  Demodulator dependence functions (not exported)*/
/* **************************************************** */
//these functions is defined by each device (device_name.c)
extern DMD_ERROR_t	DMD_device_open( DMD_PARAMETER_t *param );
extern DMD_ERROR_t	DMD_device_term( DMD_PARAMETER_t *param );
extern DMD_ERROR_t	DMD_device_close( DMD_PARAMETER_t *param );
extern DMD_ERROR_t	DMD_device_init( DMD_PARAMETER_t *param );
extern DMD_ERROR_t	DMD_device_load_pseq ( DMD_PARAMETER_t *param );
extern DMD_ERROR_t	DMD_device_pre_tune ( DMD_PARAMETER_t *param );
extern DMD_ERROR_t	DMD_device_post_tune ( DMD_PARAMETER_t *param );
//extern DMD_ERROR_t DMD_device_set_sig_type(DMD_PARAMETER_t* param);
extern DMD_ERROR_t	DMD_device_set_system( DMD_PARAMETER_t *param ); 
extern DMD_ERROR_t	DMD_device_reset( DMD_PARAMETER_t *param );
extern DMD_ERROR_t	DMD_device_scan( DMD_PARAMETER_t *param );
extern DMD_ERROR_t	DMD_device_get_info( DMD_PARAMETER_t *param , DMD_u32_t id);
extern DMD_ERROR_t	DMD_device_set_info( DMD_PARAMETER_t *param , DMD_u32_t id ,DMD_u32_t val);
extern DMD_u32_t	DMD_BER( DMD_PARAMETER_t *param, DMD_u32_t* err , DMD_u32_t* sum );
extern DMD_u32_t	DMD_CNR( DMD_PARAMETER_t *param, DMD_u32_t* cnr_i , DMD_u32_t* cnr_d );
extern DMD_u32_t	DMD_AGC(DMD_PARAMETER_t *param);
extern DMD_u32_t	DMD_RegSet_Rev;

#ifdef __cplusplus
}
#endif

#endif