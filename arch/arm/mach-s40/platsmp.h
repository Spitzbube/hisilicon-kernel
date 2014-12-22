#ifndef __PLATSMP__H__
#define __PLATSMP__H__

//extern int __cpuinitdata pen_release;

void s40_secondary_startup(void);

void s5_scu_power_up(int cpu);
void s40_scu_power_up(int cpu);

//void slave_cores_power_off(int cpu);

#endif

