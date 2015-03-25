#include "hi_drv_hdmi.h"

typedef struct hiHDMI_DELAY_TIME_S{
    HI_U8           u8IDManufactureName[4];   /**<Manufacture name*//**<CNcomment:设备厂商标识 */
    HI_U32          u32IDProductCode;         /**<Product code*//**<CNcomment:设备ID */
    HI_U32          u32DelayTimes;
    HI_U8           u8ProductType[32];    /**<Product Type*//**<CNcomment:产品型号 */
}HDMI_DELAY_TIME_S;

HI_S32 SetFormatDelay(HI_UNF_HDMI_SINK_CAPABILITY_S *sinkCap,HI_U32 *DelayTime);

