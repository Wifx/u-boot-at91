/*
 * Configuration settings for the SAMA5D4 LORIX One Wifx gateway.
 *
 * Copyright (C) 2016 Wifx
 *		      Yannick Lanz <yannick.lanz@gmail.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CONFIG_H
#define __CONFIG_H

/* No NOR flash, this definition should put before common header */
#define CONFIG_SYS_NO_FLASH

#include "at91-sama5_common.h"

/* serial console */
#define CONFIG_ATMEL_USART
#define CONFIG_USART_BASE		ATMEL_BASE_USART3
#define CONFIG_USART_ID			ATMEL_ID_USART3

/* SDRAM */
#define CONFIG_NR_DRAM_BANKS		1
#define CONFIG_SYS_SDRAM_BASE       ATMEL_BASE_DDRCS
#define CONFIG_SYS_SDRAM_SIZE		0x08000000

#ifdef CONFIG_SPL_BUILD
#define CONFIG_SYS_INIT_SP_ADDR		0x210000
#else
#define CONFIG_SYS_INIT_SP_ADDR \
	(CONFIG_SYS_SDRAM_BASE + 4 * 1024 - GENERATED_GBL_DATA_SIZE)
#endif

#define CONFIG_SYS_LOAD_ADDR		0x22000000 /* load address */

/* NAND flash */
#define CONFIG_CMD_NAND

#ifdef CONFIG_CMD_NAND
#define CONFIG_NAND_ATMEL
#define CONFIG_SYS_MAX_NAND_DEVICE	1
#define CONFIG_SYS_NAND_BASE		ATMEL_BASE_CS3
/* our ALE is AD21 */
#define CONFIG_SYS_NAND_MASK_ALE	(1 << 21)
/* our CLE is AD22 */
#define CONFIG_SYS_NAND_MASK_CLE	(1 << 22)
#define CONFIG_SYS_NAND_ONFI_DETECTION
/* PMECC & PMERRLOC */
#define CONFIG_ATMEL_NAND_HWECC
#define CONFIG_ATMEL_NAND_HW_PMECC
#endif

/* MMC */
#define CONFIG_CMD_MMC

#ifdef CONFIG_CMD_MMC
#define CONFIG_MMC
#define CONFIG_GENERIC_MMC
#define CONFIG_GENERIC_ATMEL_MCI
#define ATMEL_BASE_MMCI			ATMEL_BASE_MCI1
#endif

/* USB */
#define CONFIG_CMD_USB

#ifdef CONFIG_CMD_USB
#define CONFIG_USB_EHCI
#define CONFIG_USB_EHCI_ATMEL
#define CONFIG_SYS_USB_EHCI_MAX_ROOT_PORTS	3
#define CONFIG_USB_STORAGE
#endif

/* USB device */
#define CONFIG_USB_GADGET
#define CONFIG_USB_GADGET_DUALSPEED
#define CONFIG_USB_GADGET_ATMEL_USBA
#define CONFIG_USB_ETHER
#define CONFIG_USB_ETH_RNDIS
#define CONFIG_USBNET_MANUFACTURER      "Wifx LORIX One"

#if defined(CONFIG_CMD_USB) || defined(CONFIG_CMD_MMC)
#define CONFIG_CMD_FAT
#define CONFIG_DOS_PARTITION
#endif

/* Ethernet Hardware */
#define CONFIG_MACB
#define CONFIG_RMII
#define CONFIG_NET_RETRY_COUNT		20
#define CONFIG_MACB_SEARCH_PHY

/* Software (bit-bang) I2C */
#define CONFIG_CMD_I2C

#define CONFIG_SYS_I2C
#define CONFIG_SYS_I2C_SOFT
#define CONFIG_SYS_I2C_SOFT_SPEED	50000
#define CONFIG_SYS_I2C_SOFT_SLAVE	0x00
#define I2C_SOFT_DECLARATIONS4
#define CONFIG_SYS_I2C_SOFT_SPEED_4     50000
#define CONFIG_SYS_I2C_SOFT_SLAVE_4     0x00

#define CONFIG_ACT8865_POWER

#ifndef __ASSEMBLY__
int get_soft_i2c_scl_pin(void);
int get_soft_i2c_sda_pin(void);
#endif
#define CONFIG_SOFT_I2C_GPIO_SCL	get_soft_i2c_scl_pin()
#define CONFIG_SOFT_I2C_GPIO_SDA	get_soft_i2c_sda_pin()

#ifdef CONFIG_SYS_USE_SERIALFLASH
/* override the bootcmd, bootargs and other configuration for spi flash env */
#elif CONFIG_SYS_USE_NANDFLASH
/* override the bootcmd, bootargs and other configuration for nandflash env */
#elif CONFIG_SYS_USE_MMC
/* override the bootcmd, bootargs and other configuration for sd/mmc env */
#endif

/* SPL */
#define CONFIG_SPL_FRAMEWORK
#define CONFIG_SPL_TEXT_BASE		0x200000
#define CONFIG_SPL_MAX_SIZE		0x10000
#define CONFIG_SPL_BSS_START_ADDR	0x20000000
#define CONFIG_SPL_BSS_MAX_SIZE		0x80000
#define CONFIG_SYS_SPL_MALLOC_START	0x20080000
#define CONFIG_SYS_SPL_MALLOC_SIZE	0x80000

#define CONFIG_SPL_LIBCOMMON_SUPPORT
#define CONFIG_SPL_LIBGENERIC_SUPPORT
#define CONFIG_SPL_GPIO_SUPPORT
#define CONFIG_SPL_SERIAL_SUPPORT

#define CONFIG_SPL_BOARD_INIT
#define CONFIG_SYS_MONITOR_LEN		(512 << 10)

#ifdef CONFIG_SYS_USE_MMC
#define CONFIG_SPL_LDSCRIPT		arch/arm/cpu/at91-common/u-boot-spl.lds
#define CONFIG_SPL_MMC_SUPPORT
#define CONFIG_SYS_U_BOOT_MAX_SIZE_SECTORS	0x400
#define CONFIG_SYS_MMCSD_RAW_MODE_U_BOOT_SECTOR 0x200
#define CONFIG_SYS_MMCSD_FS_BOOT_PARTITION	1
#define CONFIG_SPL_FS_LOAD_PAYLOAD_NAME		"u-boot.img"
#define CONFIG_SPL_FAT_SUPPORT
#define CONFIG_SPL_LIBDISK_SUPPORT

#elif CONFIG_SYS_USE_NANDFLASH
#define CONFIG_SPL_NAND_SUPPORT
#define CONFIG_SPL_NAND_DRIVERS
#define CONFIG_SPL_NAND_BASE
#define CONFIG_PMECC_CAP			4
#define CONFIG_PMECC_SECTOR_SIZE	512
#define CONFIG_SYS_NAND_U_BOOT_OFFS	0x40000
#define CONFIG_SYS_NAND_5_ADDR_CYCLE
#define CONFIG_SYS_NAND_PAGE_SIZE	0x0800
#define CONFIG_SYS_NAND_PAGE_COUNT	64
#define CONFIG_SYS_NAND_OOBSIZE		64
#define CONFIG_SYS_NAND_BLOCK_SIZE	0x20000
#define CONFIG_SYS_NAND_BAD_BLOCK_POS	0x0
#define CONFIG_SPL_GENERATE_ATMEL_PMECC_HEADER

#endif
#endif
