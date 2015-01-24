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
#include "MN_DMD_driver.h"
#include "MN_I2C.h"
#include "MN88472_UserFunction.h"

/**

@brief Get the data PLPs that the demodulator has detected.
	If a single PLP service is in use, then pNumPLPs = 1
	and the plpIds[0] shall contain the signalled PLP Id.

@param pPLPIds Pointer to an array of at least 256 bytes in length 
 that can receive the list of data PLPs carried.

@param pNumPLPs The number of data PLPs detected (signalled in L1-post).

@param param	object for target device

@return DMD_E_OK if the pPLPIds and pNumPLPs are valid.

*/

DMD_ERROR_t DMD_API DMD_get_dataPLPs
( DMD_u8_t * pPLPIds, DMD_u8_t * pNumPLPs , DMD_PARAMETER_t* param )
{
	DMD_u32_t i;

	//DVBT2 and Lock
	DMD_get_info( param , DMD_E_INFO_LOCK);
	if( param->system != DMD_E_DVBT2 ||
	   param->info[DMD_E_INFO_LOCK] != DMD_E_LOCKED )
	{
		//Error 
		*pNumPLPs = 0;
		return DMD_E_ERROR;
	}
	
	*pNumPLPs = 0;		//count only Data PLP
	//Get PLP Number
	DMD_get_info( param , DMD_E_INFO_DVBT2_NUM_PLP );
	for(i=0;i<param->info[DMD_E_INFO_DVBT2_NUM_PLP];i++)
	{
		//select PLP No
		 DMD_set_info( param , DMD_E_INFO_DVBT2_SELECTED_PLP , i );
		 DMD_wait(300);
		 DMD_get_info( param , DMD_E_INFO_DVBT2_DAT_PLP_TYPE );
		 if( param->info[DMD_E_INFO_DVBT2_DAT_PLP_TYPE] != DMD_E_DVBT2_PLP_TYPE_COM )
		 {
			 DMD_get_info( param , DMD_E_INFO_DVBT2_DAT_PLP_ID );
			 pPLPIds[(*pNumPLPs)] = (DMD_u8_t)param->info[DMD_E_INFO_DVBT2_DAT_PLP_ID];
			 (*pNumPLPs) ++;
		 }
	}

	return DMD_E_OK;
}
