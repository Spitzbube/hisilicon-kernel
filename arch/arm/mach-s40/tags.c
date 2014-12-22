
#include <linux/init.h>
#include <linux/kernel.h>
#include <asm/setup.h>


static char boot_sdkversion[64] = "0.0.0.0";
#define ATAG_SDKVERSION	0x726d6d75

static int __init parse_tag_sdkversion(const struct tag *tag)
{
	memcpy(boot_sdkversion, &tag->u, sizeof(boot_sdkversion));
	return 0;
}
__tagtable(ATAG_SDKVERSION, parse_tag_sdkversion);

const char * get_sdkversion(void)
{
	return boot_sdkversion;
}
EXPORT_SYMBOL(get_sdkversion);


unsigned long get_dram_size(void)
{
	unsigned long board_dram_size = 0;
	int i;

	for (i = 0; i < meminfo.nr_banks; i++)
	{
		struct membank *bank = &meminfo.bank[i];

		board_dram_size += bank->size;
	}

	return board_dram_size;
}
EXPORT_SYMBOL(get_dram_size);


