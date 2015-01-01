
#define PERI_CRG32		IO_ADDRESS(0x20030080)
#define SDIO0CLK_PCTRL		(1U << 3)
#define SDIO0CLK_SEL_50M	(1U << 2)	/* 50MHz */
#define SDIO0_CKEN		(1U << 1)
#define SYS_PERIPHCTRL14	IO_ADDRESS(0x2005003c)
#define SDIO0_DET_MODE		(1U << 2)

static void hi_mci_sys_ctrl_init(struct himci_host *host)
{
	unsigned int reg_value;
	/* set detect polarity */
	reg_value = himci_readl(SYS_PERIPHCTRL14);
	reg_value &= ~SDIO0_DET_MODE;
	himci_writel(reg_value, SYS_PERIPHCTRL14);
	/* set clk polarity, mmc clk */
	reg_value = 0;
	reg_value = himci_readl(PERI_CRG32);
	reg_value &= ~(SDIO0CLK_SEL_50M | SDIO0CLK_PCTRL);
	reg_value |= SDIO0_CKEN;
	himci_writel(reg_value, PERI_CRG32);
}

static void hi_mci_sys_ctrl_suspend(struct himci_host *host)
{
}
