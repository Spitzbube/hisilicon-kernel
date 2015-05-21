#include "hi_drv_hdmi.h"

typedef struct hiHDMI_DELAY_TIME_S{
    HI_U8           u8IDManufactureName[4];   /**<Manufacture name*//**<CNcomment:�豸���̱�ʶ */
    HI_U32          u32IDProductCode; //4        /**<Product code*//**<CNcomment:�豸ID */
    HI_U32          u32DelayTimes; //8
    HI_U32          Data_12; //12
    HI_U8           u8ProductType[32]; //16   /**<Product Type*//**<CNcomment:��Ʒ�ͺ� */
}HDMI_DELAY_TIME_S;

typedef struct
{
    HI_U8           u8IDManufactureName[4];   /**<Manufacture name*//**<CNcomment:�豸���̱�ʶ */
    HI_U32          u32IDProductCode; //4        /**<Product code*//**<CNcomment:�豸ID */
    HI_U8           u8ProductType[32]; //8   /**<Product Type*//**<CNcomment:��Ʒ�ͺ� */
} Struct_80faed54;


//HI_S32 SetFormatDelay(HI_UNF_HDMI_SINK_CAPABILITY_S *sinkCap,HI_U32 *DelayTime);

void SetGlobalFmtDelay(HI_U32);
HI_U32 GetGlobalFmtDelay(void);

void SetForceDelayMode(HI_BOOL bForceFmtDelay, HI_BOOL bForceMuteDelay);
HI_BOOL IsForceFmtDelay(void);
HI_BOOL IsForceMuteDelay(void);

void SetGlobalMuteDelay(HI_U32);
HI_U32 GetGlobalsMuteDelay(void);

HI_S32 GetFormatDelay(HI_UNF_HDMI_ID_E enHdmi, HI_U32* delayTime);

HI_S32 GetmuteDelay(HI_UNF_HDMI_ID_E enHdmi, HI_U32* Delay);

void Check1stOE(HI_UNF_HDMI_ID_E enHdmi);




