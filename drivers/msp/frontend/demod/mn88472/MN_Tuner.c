
/* **************************************************** */
/*!
   @file	MN_DMD_Tuner_template.c
   @brief	Tuner API wrapper (sample)
   @author	R.Mori
   @date	2011/6/30
   @param
		(c)	Panasonic
   */
/* **************************************************** */
//---------- this is sample Tuner API  ----------------
#include "MN_DMD_driver.h"
#include "MN_DMD_common.h"
#include "MN_DMD_device.h"
#include "MN_I2C.h"
#include "MN88472.h"

/* **************************************************** */
/* **************************************************** */

/*! Tuner Initialize Function*/
DMD_ERROR_t DMD_API DMD_Tuner_init(DMD_PARAMETER_t *param)
{
	//TODO:	Please call tuner initialize API here
	//this function is called by DMD_init
	
#if 0	
	DMD_ERROR_t	ret;
       DMD_u32_t     i2cnum;
       DMD_u32_t     i2caddr;
       TUNER_OPS_S* stTunerOps = (TUNER_OPS_S*)(param->userdata);
       
       i2cnum = stTunerOps->enI2cChannel;
       i2caddr = stTunerOps->u32DemodAddress;
	ret = DMD_I2C_Write(i2cnum, DMD_BANK_1st + i2caddr, DMD_TCBSET , 0x53);
	DMD_DBG_TRACE("Tuner is not implemeted\n");
#endif
	return DMD_E_OK;
}
/*! Tuner Tune Function */
DMD_ERROR_t DMD_API DMD_Tuner_set_system(DMD_PARAMETER_t *param)
{
	//TODO:	Please call tune  API here
	//this function is called by DMD_tune
	DMD_DBG_TRACE("Tuner is not implemeted\n");
	return DMD_E_OK;
}
