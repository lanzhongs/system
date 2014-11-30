/*
 * Copyright (C) 2009, Texas Instruments, Incorporated
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation version 2.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __CONFIG_TI816X_EVM_H
#define __CONFIG_TI816X_EVM_H

/* Unsupported features */
#undef CONFIG_USE_IRQ
#define CONFIG_VERSION_VARIABLE

/* By default, any image built will have MMC, NAND, SPI and I2C support */
//#define CONFIG_MMC			1
#define CONFIG_NAND				1
#define CONFIG_SPI				1
//#define CONFIG_I2C			1

/* U-Boot default commands */
#include <config_cmd_default.h>
#define CONFIG_CHAOYUAN_BOARD	1
/*
 * Size of malloc() pool
 */
#define CONFIG_SYS_MALLOC_LEN           0x4000000//(CONFIG_ENV_SIZE + (32 * 1024))
#define CONFIG_SYS_GBL_DATA_SIZE        128     /* size in bytes reserved for
						initial data */

/* SPI support */
#define CONFIG_OMAP3_SPI
#define CONFIG_MTD_DEVICE
#define CONFIG_SPI_FLASH
#define CONFIG_SPI_FLASH_WINBOND
#define CONFIG_CMD_SF
#define CONFIG_SF_DEFAULT_SPEED	(75000000)


#define CONFIG_CMDLINE_TAG             	1   /* enable passing of ATAGs  */
#define CONFIG_SETUP_MEMORY_TAGS       	1
#define CONFIG_INITRD_TAG              	1	/* for ramdisk support */
#define CONFIG_TI816X_TWO_EMIF			1
#define CONFIG_MISC_INIT_R				1
#define CONFIG_REVISION_TAG				1

/* Only one the following two options (DDR3/DDR2) should be enabled */
#define CONFIG_TI816X_EVM_DDR3                  /* Configure DDR3 in U-Boot */
/*#define CONFIG_TI816X_EVM_DDR2*/		/* Configure DDR2 in U-Boot */


#define CONFIG_BOOTDELAY		3		/* set to negative value for no autoboot */
#define CONFIG_VERSION_VARIABLE
#define CONFIG_BOOTARGS         		"console=ttyO2,115200n8 mem=160M"
#define CONFIG_BOOTCOMMAND				"yrdm /nand/uImage 0x8a000000;bootm 0x8a000000"
#define	CONFIG_EXTRA_ENV_SETTINGS		"verify=yes\0"
#define CONFIG_ETHADDR					"00:2d:54:48:47:b4" 
#define CONFIG_NETMASK					255.255.255.0
#define CONFIG_SERVERIP					192.0.0.128
#define CONFIG_IPADDR					192.0.0.64
#define CONFIG_BOOTFILE					"uImage"

/* Hardware related */

/**
 * Physical Memory Map
 */
#define CONFIG_NR_DRAM_BANKS		2		/* we have 2 banks of DRAM */
#define PHYS_DRAM_1			0x80000000	/* DRAM Bank #1 */
#define PHYS_DRAM_1_SIZE		0x20000000	/* 512 MB */
#define PHYS_DRAM_2			0xC0000000	/* DRAM Bank #2 */
#define PHYS_DRAM_2_SIZE		0x20000000	/* 512 MB */


/**
 * Platform/Board specific defs
 */
#define CONFIG_SYS_CLK_FREQ		27000000
#define CONFIG_SYS_TIMERBASE		0x4802E000

/*
 * NS16550 Configuration
 */
#define CONFIG_SYS_NS16550
#define CONFIG_SYS_NS16550_SERIAL
#define CONFIG_SYS_NS16550_REG_SIZE	(-4)
#define CONFIG_SYS_NS16550_CLK		(48000000)
#define CONFIG_SYS_NS16550_COM1		0x48024000	/* Base EVM has UART2 */

#define CONFIG_BAUDRATE		115200
#define CONFIG_SYS_BAUDRATE_TABLE	{ 110, 300, 600, 1200, 2400, \
4800, 9600, 14400, 19200, 28800, 38400, 56000, 57600, 115200 }

#define CONFIG_AUTO_COMPLETE		1
#define CONFIG_CMDLINE_EDITING

/*
 * Miscellaneous configurable options
 */
/* allow overwriting serial config and ethaddr */
#define CONFIG_ENV_OVERWRITE
/* Undef to save memory */
#define CONFIG_SYS_LONGHELP
/* Monitor Command Prompt */
#define CONFIG_SYS_PROMPT		"CYIT #"

/* Console I/O Buffer Size */
#define CONFIG_SYS_CBSIZE		512
/* Print Buffer Size */
#define CONFIG_SYS_PBSIZE		(CONFIG_SYS_CBSIZE \
					+ sizeof(CONFIG_SYS_PROMPT) + 16)
/* max number of command args */
#define CONFIG_SYS_MAXARGS		32
/* Boot Argument Buffer Size */
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE
/* memtest works on 8 MB in DRAM after skipping 32MB from start addr of ram disk*/
#define CONFIG_SYS_MEMTEST_START	(PHYS_DRAM_1 + (64 *1024 *1024))
#define CONFIG_SYS_MEMTEST_END		(CONFIG_SYS_MEMTEST_START \
					+ (8 * 1024 * 1024))

#undef  CONFIG_SYS_CLKS_IN_HZ				/* everything, incl board info, in Hz */
#define CONFIG_SYS_LOAD_ADDR		0x8a000000	/* Default load address */
#define CONFIG_SYS_HZ			1000	/* 1ms clock */


/*
 * select serial console configuration
 */
#define CONFIG_SERIAL1			1
#define CONFIG_CONS_INDEX		1
#define CONFIG_SYS_CONSOLE_INFO_QUIET

#ifndef CONFIG_TI81XX_PCIE_BOOT
#if defined(CONFIG_NO_ETH)
# undef CONFIG_CMD_NET
#else
# define CONFIG_CMD_DHCP
# define CONFIG_CMD_PING
#endif

#if defined(CONFIG_CMD_NET)
# define CONFIG_DRIVER_TI_EMAC
# define CONFIG_MII
# define CONFIG_BOOTP_DEFAULT
# define CONFIG_BOOTP_DNS
# define CONFIG_BOOTP_DNS2
# define CONFIG_BOOTP_SEND_HOSTNAME
# define CONFIG_BOOTP_GATEWAY
# define CONFIG_BOOTP_SUBNETMASK
# define CONFIG_NET_RETRY_COUNT		10
# define CONFIG_NET_MULTI
#endif
#endif

#if defined(CONFIG_SYS_NO_FLASH)
#define CONFIG_ENV_IS_NOWHERE
#endif

/* NAND support */
#ifdef CONFIG_NAND
# define CONFIG_MTD_NAND_YAFFS2
# define CONFIG_YAFFS2
# define CONFIG_CMD_NAND
# define CONFIG_NAND_TI81XX
# define CONFIG_MTD_NAND_ECC_BCH
# define GPMC_NAND_ECC_LP_x16_LAYOUT	1
# define NAND_BASE			(0x08000000)	/* FIXME not sure */
# define CONFIG_SYS_NAND_ADDR		NAND_BASE	/* physical address */
							/* to access nand */
# define CONFIG_SYS_NAND_BASE		NAND_BASE	/* physical address */
							/* to access nand at */
							/* CS0 */
# define CONFIG_SYS_MAX_NAND_DEVICE	1		/* Max number of NAND */
#endif							/* devices */


/* ENV in SPI */
#if defined(CONFIG_SPI_ENV)
# undef CONFIG_ENV_IS_NOWHERE
# undef CONFIG_ENV_SIZE
# define CONFIG_ENV_IS_IN_SPI_FLASH	1
# ifdef CONFIG_ENV_IS_IN_SPI_FLASH
#define CONFIG_ENV_SIZE			0x2000 /*use a small env */
#  define CONFIG_SYS_FLASH_BASE		(0)
#  define SPI_FLASH_ERASE_SIZE		(4 * 1024) /* sector size of SPI flash */
#  define CONFIG_SYS_ENV_SECT_SIZE	(2 * SPI_FLASH_ERASE_SIZE) /* env size */
#  define CONFIG_ENV_SECT_SIZE		(CONFIG_SYS_ENV_SECT_SIZE)
#  define CONFIG_ENV_OFFSET		(64 * SPI_FLASH_ERASE_SIZE)
#  define CONFIG_ENV_ADDR		(CONFIG_ENV_OFFSET)
#  define CONFIG_SYS_MAX_FLASH_SECT	(1024) /* no of sectors in SPI flash */
#  define CONFIG_SYS_MAX_FLASH_BANKS	(1)
# endif
#endif /* SPI support */

#endif	  /* ! __CONFIG_TI816X_EVM_H */
