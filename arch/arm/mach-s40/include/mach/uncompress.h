#ifndef __HISI_UNCOMPRESS_H__
#define __HISI_UNCOMPRESS_H__
#include <linux/io.h>
#include <mach/platform.h>

#define AMBA_UART_DR (REG_BASE_UART0 + 0x0)
#define AMBA_UART_LCRH (REG_BASE_UART0 + 0x2c)
#define AMBA_UART_CR (REG_BASE_UART0 + 0x30)
#define AMBA_UART_FR (REG_BASE_UART0 + 0x18)

/*
 * This does not append a newline
 */
static inline void putc(int c)
{
#if 0
	unsigned int val;
	val = __raw_readl(AMBA_UART_FR);
	while (val & (1 << 5)) {
		val = __raw_readl(AMBA_UART_FR);
		barrier();
	}

	__raw_writel(c, AMBA_UART_DR);
#else
	while (((volatile char*)AMBA_UART_FR)[0] & (1 << 5)) barrier();

	((volatile char*)AMBA_UART_DR)[0] = c;
#endif
}

static inline void flush(void)
{
#if 0
	unsigned int val;
	val = __raw_readl(AMBA_UART_FR);
	while (val & (1 << 3)) {
		val = __raw_readl(AMBA_UART_FR);
		barrier();
	}
#else
	while (((volatile char*)AMBA_UART_FR)[0] & (1 << 3)) barrier();
#endif
}

/*
 * nothing to do
 */
#define arch_decomp_setup()
#define arch_decomp_wdog()

#endif
