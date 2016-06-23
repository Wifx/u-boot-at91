/*
 * Copyright (C) 2015 Wifx
 *		      Yannick Lanz <yannick.lanz@gmail.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/at91_common.h>
#include <asm/arch/at91_pmc.h>
#include <asm/arch/at91_rstc.h>
#include <asm/arch/atmel_mpddrc.h>
#include <asm/arch/atmel_usba_udc.h>
#include <asm/arch/gpio.h>
#include <asm/arch/clk.h>
#include <asm/arch/sama5d3_smc.h>
#include <asm/arch/sama5d4.h>
#include <atmel_hlcdc.h>
#include <atmel_mci.h>
#include <i2c.h>
#include <lcd.h>
#include <mmc.h>
#include <net.h>
#include <netdev.h>
#include <nand.h>
#include <spi.h>
#include <version.h>
#include <act8865.h>

DECLARE_GLOBAL_DATA_PTR;

#ifdef CONFIG_ATMEL_SPI
int spi_cs_is_valid(unsigned int bus, unsigned int cs)
{
	return bus == 0 && cs == 0;
}

void spi_cs_activate(struct spi_slave *slave)
{
	at91_set_pio_output(AT91_PIO_PORTC, 3, 0);
}

void spi_cs_deactivate(struct spi_slave *slave)
{
	at91_set_pio_output(AT91_PIO_PORTC, 3, 1);
}

static void sama5d4_lorix_one_spi0_hw_init(void)
{
	at91_set_a_periph(AT91_PIO_PORTC, 0, 0);	/* SPI0_MISO */
	at91_set_a_periph(AT91_PIO_PORTC, 1, 0);	/* SPI0_MOSI */
	at91_set_a_periph(AT91_PIO_PORTC, 2, 0);	/* SPI0_SPCK */

	at91_set_pio_output(AT91_PIO_PORTC, 3, 1);	/* SPI0_CS0 */

	/* Enable clock */
	at91_periph_clk_enable(ATMEL_ID_SPI0);
}
#endif /* CONFIG_ATMEL_SPI */

#ifdef CONFIG_NAND_ATMEL
static void sama5d4_lorix_one_nand_hw_init(void)
{
	struct at91_smc *smc = (struct at91_smc *)ATMEL_BASE_SMC;

	at91_periph_clk_enable(ATMEL_ID_SMC);

	/* Configure SMC CS3 for NAND */
	writel(AT91_SMC_SETUP_NWE(1) | AT91_SMC_SETUP_NCS_WR(1) |
	       AT91_SMC_SETUP_NRD(1) | AT91_SMC_SETUP_NCS_RD(1),
	       &smc->cs[3].setup);
	writel(AT91_SMC_PULSE_NWE(2) | AT91_SMC_PULSE_NCS_WR(3) |
	       AT91_SMC_PULSE_NRD(2) | AT91_SMC_PULSE_NCS_RD(3),
	       &smc->cs[3].pulse);
	writel(AT91_SMC_CYCLE_NWE(5) | AT91_SMC_CYCLE_NRD(5),
	       &smc->cs[3].cycle);
	writel(AT91_SMC_TIMINGS_TCLR(2) | AT91_SMC_TIMINGS_TADL(7) |
	       AT91_SMC_TIMINGS_TAR(2)  | AT91_SMC_TIMINGS_TRR(3)   |
	       AT91_SMC_TIMINGS_TWB(7)  | AT91_SMC_TIMINGS_RBNSEL(3)|
	       AT91_SMC_TIMINGS_NFSEL(1), &smc->cs[3].timings);
	writel(AT91_SMC_MODE_RM_NRD | AT91_SMC_MODE_WM_NWE |
	       AT91_SMC_MODE_EXNW_DISABLE |
	       AT91_SMC_MODE_DBW_8 |
	       AT91_SMC_MODE_TDF_CYCLE(3),
	       &smc->cs[3].mode);

	at91_set_a_periph(AT91_PIO_PORTC, 5, 0);	/* D0 */
	at91_set_a_periph(AT91_PIO_PORTC, 6, 0);	/* D1 */
	at91_set_a_periph(AT91_PIO_PORTC, 7, 0);	/* D2 */
	at91_set_a_periph(AT91_PIO_PORTC, 8, 0);	/* D3 */
	at91_set_a_periph(AT91_PIO_PORTC, 9, 0);	/* D4 */
	at91_set_a_periph(AT91_PIO_PORTC, 10, 0);	/* D5 */
	at91_set_a_periph(AT91_PIO_PORTC, 11, 0);	/* D6 */
	at91_set_a_periph(AT91_PIO_PORTC, 12, 0);	/* D7 */
	at91_set_a_periph(AT91_PIO_PORTC, 13, 0);	/* RE */
	at91_set_a_periph(AT91_PIO_PORTC, 14, 0);	/* WE */
	at91_set_a_periph(AT91_PIO_PORTC, 15, 1);	/* NCS */
	at91_set_a_periph(AT91_PIO_PORTC, 16, 1);	/* RDY */
	at91_set_a_periph(AT91_PIO_PORTC, 17, 1);	/* ALE */
	at91_set_a_periph(AT91_PIO_PORTC, 18, 1);	/* CLE */
}
#endif

#ifdef CONFIG_GENERIC_ATMEL_MCI
void sama5d4_lorix_one_mci1_hw_init(void)
{
	at91_set_c_periph(AT91_PIO_PORTE, 19, 1);	/* MCI1 CDA */
	at91_set_c_periph(AT91_PIO_PORTE, 20, 1);	/* MCI1 DA0 */
	at91_set_c_periph(AT91_PIO_PORTE, 21, 1);	/* MCI1 DA1 */
	at91_set_c_periph(AT91_PIO_PORTE, 22, 1);	/* MCI1 DA2 */
	at91_set_c_periph(AT91_PIO_PORTE, 23, 1);	/* MCI1 DA3 */
	at91_set_c_periph(AT91_PIO_PORTE, 18, 0);	/* MCI1 CLK */

	/*
	 * As the mci io internal pull down is too strong, so if the io needs
	 * external pull up, the pull up resistor will be very small, if so
	 * the power consumption will increase, so disable the interanl pull
	 * down to save the power.
	 */
	at91_set_pio_pulldown(AT91_PIO_PORTE, 18, 0);
	at91_set_pio_pulldown(AT91_PIO_PORTE, 19, 0);
	at91_set_pio_pulldown(AT91_PIO_PORTE, 20, 0);
	at91_set_pio_pulldown(AT91_PIO_PORTE, 21, 0);
	at91_set_pio_pulldown(AT91_PIO_PORTE, 22, 0);
	at91_set_pio_pulldown(AT91_PIO_PORTE, 23, 0);

	/* Enable clock */
	at91_periph_clk_enable(ATMEL_ID_MCI1);
}

int board_mmc_init(bd_t *bis)
{
	/* Enable the power supply */
	//at91_set_pio_output(AT91_PIO_PORTE, 4, 0);

	return atmel_mci_init((void *)ATMEL_BASE_MCI1);
}
#endif /* CONFIG_GENERIC_ATMEL_MCI */

#ifdef CONFIG_MACB
void sama5d4_lorix_one_macb0_hw_init(void)
{
	at91_set_a_periph(AT91_PIO_PORTB, 0, 0);	/* ETXCK_EREFCK */
	at91_set_a_periph(AT91_PIO_PORTB, 6, 0);	/* ERXDV */
	at91_set_a_periph(AT91_PIO_PORTB, 8, 0);	/* ERX0 */
	at91_set_a_periph(AT91_PIO_PORTB, 9, 0);	/* ERX1 */
	at91_set_a_periph(AT91_PIO_PORTB, 7, 0);	/* ERXER */
	at91_set_a_periph(AT91_PIO_PORTB, 2, 0);	/* ETXEN */
	at91_set_a_periph(AT91_PIO_PORTB, 12, 0);	/* ETX0 */
	at91_set_a_periph(AT91_PIO_PORTB, 13, 0);	/* ETX1 */
	at91_set_a_periph(AT91_PIO_PORTB, 17, 0);	/* EMDIO */
	at91_set_a_periph(AT91_PIO_PORTB, 16, 0);	/* EMDC */

	/* Enable clock */
	at91_periph_clk_enable(ATMEL_ID_GMAC0);
}
#endif

static void sama5d4_lorix_one_serial3_hw_init(void)
{
	at91_set_b_periph(AT91_PIO_PORTE, 17, 1);	/* TXD3 */
	at91_set_b_periph(AT91_PIO_PORTE, 16, 0);	/* RXD3 */

	/* Enable clock */
	at91_periph_clk_enable(ATMEL_ID_USART3);
}

int board_early_init_f(void)
{
	at91_periph_clk_enable(ATMEL_ID_PIOA);
	at91_periph_clk_enable(ATMEL_ID_PIOB);
	at91_periph_clk_enable(ATMEL_ID_PIOC);
	at91_periph_clk_enable(ATMEL_ID_PIOD);
	at91_periph_clk_enable(ATMEL_ID_PIOE);

	sama5d4_lorix_one_serial3_hw_init();

	return 0;
}

int board_init(void)
{
	/* adress of boot parameters */
	gd->bd->bi_boot_params = CONFIG_SYS_SDRAM_BASE + 0x100;

#ifdef CONFIG_ATMEL_SPI
	sama5d4_lorix_one_spi0_hw_init();
#endif
#ifdef CONFIG_NAND_ATMEL
	sama5d4_lorix_one_nand_hw_init();
#endif
#ifdef CONFIG_GENERIC_ATMEL_MCI
	sama5d4_lorix_one_mci1_hw_init();
#endif
#ifdef CONFIG_MACB
	sama5d4_lorix_one_macb0_hw_init();
#endif
#ifdef CONFIG_USB_GADGET_ATMEL_USBA
	at91_udp_hw_init();
#endif
#ifdef CONFIG_ACT8865_POWER
	if (i2c_set_bus_num(1)) {
		printf("I2C:   failed to switch bus 1\n");
	} else {
		if (act8865_i2c_interface_accessable())
			act8865_enable_ldo_output(ACT8865_LDO_REG5,
						  ACT8865_3V3_VOLT);
	}
#endif

	return 0;
}

int dram_init(void)
{
	gd->ram_size = get_ram_size((void *)CONFIG_SYS_SDRAM_BASE,
				    CONFIG_SYS_SDRAM_SIZE);
	return 0;
}

int board_eth_init(bd_t *bis)
{
	int rc = 0;
	unsigned char buf[6];

#ifdef CONFIG_MACB
	rc = macb_eth_initialize(0, (void *)ATMEL_BASE_GMAC0, 0x00);
	if (rc) {
		printf("GMAC0 register failed\n");
	} else {
		if (!getenv("ethaddr")) {
			/* Set I2C bus to 0 */
			if (i2c_set_bus_num(0)) {
				printf("i2c bus 0 is not valid\n");
			} else {
				/* Read MAC address */
				if (i2c_read(0x58, 0x9a, 1, buf, 6)) {
					printf("MAC address read failed\n");
				} else {
					if (is_valid_ether_addr(buf)) {
						eth_setenv_enetaddr("ethaddr",
								    buf);
					} else {
						printf("MAC address is not valid\n");
					}
				}
			}
		}
	}
#endif

#ifdef CONFIG_USB_GADGET_ATMEL_USBA
	usba_udc_probe(&pdata);
#ifdef CONFIG_USB_ETH_RNDIS
	usb_eth_initialize(bis);
#endif
#endif

	return rc;
}

#ifdef CONFIG_SYS_I2C_SOFT
int get_soft_i2c_scl_pin(void)
{
	switch (I2C_ADAP_HWNR) {
	case 0:
		return GPIO_PIN_PA(31);
	case 3:
		return GPIO_PIN_PC(26);
	default:
		return -1;
	}
}

int get_soft_i2c_sda_pin(void)
{
	switch (I2C_ADAP_HWNR) {
	case 0:
		return GPIO_PIN_PA(30);
	case 3:
		return GPIO_PIN_PC(25);
	default:
		return -1;
	}
}
#endif

/* SPL */
#ifdef CONFIG_SPL_BUILD
void spl_board_init(void)
{
#ifdef CONFIG_SYS_USE_MMC
	sama5d4_lorix_one_mci1_hw_init();
#elif CONFIG_SYS_USE_NANDFLASH
	sama5d4_lorix_one_nand_hw_init();
#endif
}

static void ddr2_conf(struct atmel_mpddrc_config *ddr2)
{
	ddr2->md = (ATMEL_MPDDRC_MD_DBW_16_BITS | ATMEL_MPDDRC_MD_DDR2_SDRAM);

	ddr2->cr = (ATMEL_MPDDRC_CR_NC_COL_10 |
		    ATMEL_MPDDRC_CR_NR_ROW_14 |
		    ATMEL_MPDDRC_CR_CAS_DDR_CAS3 |
		    ATMEL_MPDDRC_CR_NB_8BANKS |
		    ATMEL_MPDDRC_CR_NDQS_DISABLED |
		    ATMEL_MPDDRC_CR_DECOD_INTERLEAVED |
		    ATMEL_MPDDRC_CR_UNAL_SUPPORTED);

	ddr2->rtr = 0x2b0;

	ddr2->tpr0 = (8 << ATMEL_MPDDRC_TPR0_TRAS_OFFSET |
		      3 << ATMEL_MPDDRC_TPR0_TRCD_OFFSET |
		      3 << ATMEL_MPDDRC_TPR0_TWR_OFFSET |
		      10 << ATMEL_MPDDRC_TPR0_TRC_OFFSET |
		      3 << ATMEL_MPDDRC_TPR0_TRP_OFFSET |
		      2 << ATMEL_MPDDRC_TPR0_TRRD_OFFSET |
		      2 << ATMEL_MPDDRC_TPR0_TWTR_OFFSET |
		      2 << ATMEL_MPDDRC_TPR0_TMRD_OFFSET);

	ddr2->tpr1 = (2 << ATMEL_MPDDRC_TPR1_TXP_OFFSET |
		      200 << ATMEL_MPDDRC_TPR1_TXSRD_OFFSET |
		      25 << ATMEL_MPDDRC_TPR1_TXSNR_OFFSET |
		      23 << ATMEL_MPDDRC_TPR1_TRFC_OFFSET);

	ddr2->tpr2 = (7 << ATMEL_MPDDRC_TPR2_TFAW_OFFSET |
		      2 << ATMEL_MPDDRC_TPR2_TRTP_OFFSET |
		      3 << ATMEL_MPDDRC_TPR2_TRPA_OFFSET |
		      2 << ATMEL_MPDDRC_TPR2_TXARDS_OFFSET |
		      8 << ATMEL_MPDDRC_TPR2_TXARD_OFFSET);
}

void mem_init(void)
{
	struct atmel_mpddrc_config ddr2;

	ddr2_conf(&ddr2);

	/* enable MPDDR clock */
	at91_periph_clk_enable(ATMEL_ID_MPDDRC);
	at91_system_clk_enable(AT91_PMC_SYS_CLK_DDRCK);

	/* DDRAM2 Controller initialize */
	ddr2_init(ATMEL_BASE_DDRCS, &ddr2);
}

void at91_pmc_init(void)
{
	struct at91_pmc *pmc = (struct at91_pmc *)ATMEL_BASE_PMC;
	u32 tmp;

	tmp = AT91_PMC_PLLAR_29 |
	      AT91_PMC_PLLXR_PLLCOUNT(0x3f) |
	      AT91_PMC_PLLXR_MUL(87) |
	      AT91_PMC_PLLXR_DIV(1);
	at91_plla_init(tmp);

	writel(0x0 << 8, &pmc->pllicpr);

	tmp = AT91_PMC_MCKR_H32MXDIV |
	      AT91_PMC_MCKR_PLLADIV_2 |
	      AT91_PMC_MCKR_MDIV_3 |
	      AT91_PMC_MCKR_CSS_PLLA;
	at91_mck_init(tmp);
}
#endif
