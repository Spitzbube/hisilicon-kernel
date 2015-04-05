#ifndef __TDE_OSR_H__
#define __TDE_OSR_H__

#include <linux/ioctl.h> /* needed for the _IOW etc stuff used later */

void tde_osr_enableirq(void);
void tde_osr_disableirq(void);
void tde_osr_hsr(void* pstFunc, void* data);
HI_BOOL tde_osr_isirqdisabled(void);

#endif /* _TDE_OSR_H_ */
