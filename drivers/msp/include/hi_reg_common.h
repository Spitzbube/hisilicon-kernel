
#ifndef __HI_REG_COMMON_H__
#define __HI_REG_COMMON_H__

#if    defined(CHIP_TYPE_hi3716cv200)   \
    || defined(CHIP_TYPE_hi3718cv100)   \
    || defined(CHIP_TYPE_hi3718mv100)   \
    || defined(CHIP_TYPE_hi3719cv100)   \
    || defined(CHIP_TYPE_hi3719mv100)   \
    || defined(CHIP_TYPE_hi3719mv100_a)

#include "./hi3716cv200/hi_reg_sys.h"
#include "./hi3716cv200/hi_reg_peri.h"
#include "./hi3716cv200/hi_reg_io.h"
#include "./hi3716cv200/hi_reg_crg.h"

#define HI_SYS_BASE_ADDR        0xF8000000
#define HI_PERI_BASE_ADDR       0xF8A20000
#define HI_IO_BASE_ADDR         0xF8A21000
#define HI_CRG_BASE_ADDR        0xF8A22000

#elif defined(CHIP_TYPE_hi3716cv200es)

#include "./hi3716cv200es/hi_reg_sys.h"
#include "./hi3716cv200es/hi_reg_peri.h"
#include "./hi3716cv200es/hi_reg_io.h"
#include "./hi3716cv200es/hi_reg_crg.h"

#define HI_SYS_BASE_ADDR        0xF8000000
#define HI_PERI_BASE_ADDR       0xF8A20000
#define HI_IO_BASE_ADDR         0xF8A21000
#define HI_CRG_BASE_ADDR        0xF8A22000

#endif

typedef S_SYSCTRL_REGS_TYPE     HI_REG_SYSCTRL_S;
typedef S_PERICTRL_REGS_TYPE    HI_REG_PERI_S;
typedef S_IO_REGS_TYPE          HI_REG_IO_S;
typedef S_CRG_REGS_TYPE         HI_REG_CRG_S;

extern volatile HI_REG_SYSCTRL_S    *g_pstRegSysCtrl;
extern volatile HI_REG_CRG_S        *g_pstRegCrg;
extern volatile HI_REG_IO_S         *g_pstRegIO;
extern volatile HI_REG_PERI_S       *g_pstRegPeri;

#endif  // __HI_REG_COMMON_H__

