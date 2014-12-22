#ifndef __ASM_ARM_ARCH_IO_H
#define __ASM_ARM_ARCH_IO_H

#define IO_SPACE_LIMIT 0xffffffff

#define __io(a) __typesafe_io(a)
#define __mem_pci(a) (a)

#if 0

/*  phys_addr		virt_addr
 * 0xF800_0000 <-----> 0xFD00_0000
 */
#define S40_IOCH1_VIRT	(0xFD000000)
#define S40_IOCH1_PHYS	(0xF8000000)
#define S40_IOCH1_SIZE	(0x02000000)

#define IO_ADDRESS(x) (x + 0x5000000)

#else

/* phys_addr virt_addr
 * 0xF800_0000 <-----> 0xF900_0000
 */
#define S40_IOCH1_VIRT (0xF9000000)
#define S40_IOCH1_PHYS (0xF8000000)
#define S40_IOCH1_SIZE (0x02000000)

#define IO_ADDRESS(x) (x + 0x1000000)

/* phys_addr virt_addr
 * 0xFF00_0000 <-----> 0xFB00_0000
 */
#define S40_IOCH2_VIRT (0xFB000000)
#define S40_IOCH2_PHYS (0xFF000000)
#define S40_IOCH2_SIZE (0x00430000)

#define IO_ADDRESS2(x) (x - 0x4000000)

#endif

#endif
