/* **************************************************** */
/*!
   @file	MN88472_UserFunction.c
   @brief	Functions for MN88472 customer
   @author	R.Mori
   @date	2011/10/27
   @param
		(c)	Panasonic
   */
/* **************************************************** */


#ifndef MN88472_USERFUNCTION_H
#define MN88472_USERFUNCTION_H

#ifdef __cplusplus
extern "C" {
#endif

#include "MN_DMD_driver.h"

//--------------------------------------
//	Get PLP information
//--------------------------------------
DMD_ERROR_t DMD_API DMD_get_dataPLPs
	( DMD_u8_t * pPLPIds, DMD_u8_t * pNumPLPs , DMD_PARAMETER_t* param  );;
#ifdef __cplusplus
}
#endif

#endif