#ifndef __DRV_DISP_ALG_GMM_H__
#define __DRV_DISP_ALG_GMM_H__


typedef struct
{
	MMZ_BUFFER_S Data_0; //0 = 81141acc
	MMZ_BUFFER_S Data_12; //12 = 81141ad8
	HI_U32 Data_24; //24
	HI_U32 Data_28; //28
	HI_U32 Data_32; //32
	HI_U32 Data_36; //36
	HI_U32 Data_40; //40
	HI_U32 Data_44; //44
	HI_U32 Data_48; //48
	HI_U32 Data_52; //52
	//56???
} Struct_81141acc;


typedef struct
{
	int fill_0[1542]; //0
	//6168????
} Struct_8113ea8c;

HI_S32 ALG_GmmInit(Struct_81141acc* a);
HI_VOID UpdateGmmCoef(Struct_81141acc* a, Struct_8113ea8c* b, HI_U32 c);
HI_VOID ALG_GmmThdSet(Struct_81141acc* a, int b, int c);

#endif /*__DRV_DISP_ALG_GMM_H__*/

