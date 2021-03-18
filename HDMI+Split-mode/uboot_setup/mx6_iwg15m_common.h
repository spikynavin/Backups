/*
 * Copyright (c) 2014-2015 iWave Systems Technologies Pvt. Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
 * @file mx6_iwg15m_common.h
 *
 * @brief Common Config File for iMx6x base iwave boards
 *
 * @ingroup High level Configuration
 */

#ifndef __MX6_IWG15M_COMMON_CONFIG_H
#define __MX6_IWG15M_COMMON_CONFIG_H

#define CONFIG_MX6
#define CONFIG_MX6_IWG15
#define CONFIG_SYS_I2C_MXC

/* uncomment for PLUGIN mode support */
#define CONFIG_USE_PLUGIN 

#define CONFIG_DEFAULT_FDT_FILE         "imx6qd-iwg15m-q7_pmic_ldobypass.dtb"

#define CONFIG_SYS_L2_PL310
#define CONFIG_SYS_CACHELINE_SIZE       32

/* uncomment for SECURE mode support */
/* #define CONFIG_SECURE_BOOT */

#include "mx6_common.h"
#include <linux/sizes.h>

#define CONFIG_SYS_GENERIC_BOARD
#define CONFIG_DISPLAY_CPUINFO
#define CONFIG_DISPLAY_BOARDINFO

#include <asm/arch/imx-regs.h>
#include <asm/imx-common/gpio.h>

#define CONFIG_CMDLINE_TAG
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_INITRD_TAG
#define CONFIG_REVISION_TAG

/* Size of malloc() pool */
#define CONFIG_SYS_MALLOC_LEN		(10 * SZ_1M)

#define CONFIG_BOARD_EARLY_INIT_F
#define CONFIG_BOARD_LATE_INIT
#define CONFIG_MXC_GPIO

#define CONFIG_MXC_UART

#define CONFIG_CMD_FUSE
#ifdef CONFIG_CMD_FUSE
#define CONFIG_MXC_OCOTP
#endif

/* MMC Configs */
#define CONFIG_FSL_ESDHC
#define CONFIG_FSL_USDHC
#define CONFIG_SYS_FSL_ESDHC_ADDR      0

#define CONFIG_MMC
#define CONFIG_CMD_MMC
#define CONFIG_DYNAMIC_MMC_DEVNO
#define CONFIG_GENERIC_MMC
#define CONFIG_BOUNCE_BUFFER
#define CONFIG_CMD_EXT2
#define CONFIG_CMD_FAT
#define CONFIG_DOS_PARTITION

#define CONFIG_SUPPORT_EMMC_BOOT /* eMMC specific */

#define CONFIG_CMD_PING
#define CONFIG_CMD_DHCP
#define CONFIG_CMD_MII
#define CONFIG_CMD_NET
#define CONFIG_FEC_MXC
#define CONFIG_MII
#define IMX_FEC_BASE			ENET_BASE_ADDR
#define CONFIG_FEC_XCV_TYPE		RGMII
#define CONFIG_ETHPRIME			"FEC"
#define CONFIG_FEC_MXC_PHYADDR		0xFF
#define CONFIG_PHY_MICREL
#define CONFIG_PHY_MICREL_KSZ9021
#define CONFIG_PHY_MICREL_KSZ9031

#define CONFIG_PHYLIB

/* allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE
#define CONFIG_CONS_INDEX              1
#define CONFIG_BAUDRATE                        115200

/* Command definition */
#include <config_cmd_default.h>

#define CONFIG_CMD_BMODE
#define CONFIG_CMD_BOOTZ
#undef CONFIG_CMD_SETEXPR
#undef CONFIG_CMD_IMLS

#define CONFIG_BOOTDELAY               1

#define CONFIG_LOADADDR                        0x12000000
#define CONFIG_SYS_TEXT_BASE           0x17800000
#define CONFIG_SYS_MMC_IMG_LOAD_PART	1

#ifdef CONFIG_MFG
#define CONFIG_MFG_ENV_SETTINGS \
       "mfgtool_args=setenv bootargs console=" CONFIG_CONSOLE_DEV ",115200 " \
               "rdinit=/linuxrc " \
               "g_mass_storage.stall=0 g_mass_storage.removable=1 " \
               "g_mass_storage.idVendor=0x066F g_mass_storage.idProduct=0x37FF "\
               "g_mass_storage.iSerialNumber=\"\" "\
               "enable_wait_mode=off "\
                "\0" \
               "initrd_addr=0x12C00000\0" \
               "initrd_high=0xffffffff\0" \
                "bootcmd_mfg=run mfgtool_args;bootm ${loadaddr} ${initrd_addr} ${fdt_addr};\0" \
                "bootcmd_mfg_pmic=run mfgtool_args;bootm ${loadaddr} ${initrd_addr} ${fdt_addr_pmic};\0" \

#define CONFIG_EXTRA_ENV_SETTINGS \
               CONFIG_MFG_ENV_SETTINGS \
               "fdt_addr=0x18000000\0" \
               "fdt_addr_pmic=0x1A000000\0" \
               "fdt_high=0xffffffff\0" \
               "uboot_ldo_bypass=1\0" \
                "ethaddr=00:01:02:03:04:05\0" \
               "\0" 
#else

#define COMMON_CONFIG_EXTRA_ENV_SETTINGS \
        "fdt_file=" CONFIG_DEFAULT_FDT_FILE "\0" \
        "uboot_ldo_bypass=1\0" \
	"mxc_hdmi.only_cea=0\0" \
        "fdtforce=0\0" \
        "fdt_check=if test ${fdtforce} = \"0\"; then dynamicfdt;fi;\0" \
        "fdt_addr=0x18000000\0" \
        "fdt_high=0xffffffff\0"   \
               "ethaddr=00:01:02:03:04:05\0" \
                "kernel=uImage\0" \
                "bootargs_base=console=ttymxc1,115200n8\0" \
                "bootcmd_net=dhcp;run fdt_check;tftpboot ${loadaddr} ${serverip}:${kernel};tftpboot ${fdt_addr} ${serverip}:${fdt_file};" \
                        "run bootargs_net;bootm ${loadaddr} - ${fdt_addr}\0" \
                "bootargs_net=setenv bootargs ${bootargs_base} " \
                        "root=/dev/nfs ip=dhcp nfsroot=${serverip}:${nfsroot},v3,tcp ${lcd} ${hdmi}\0" \

#define CONFIG_EXTRA_ENV_SETTINGS \
                COMMON_CONFIG_EXTRA_ENV_SETTINGS \
                "bootargs_mmc=setenv bootargs ${bootargs_base} " \
                        "root=/dev/mmcblk2p2 rootwait rw ${lcd} ${hdmi}\0" \
                "bootcmd_mmc=run bootargs_mmc;run fdt_check;mmc dev 2;" \
                        "fatload mmc 2 ${loadaddr} ${kernel};fatload mmc 2 ${fdt_addr} ${fdt_file};bootm ${loadaddr} - ${fdt_addr}\0" \
                "bootcmd=run bootcmd_mmc\0" \
        "\0" 

#define CONFIG_EXTRA_ENV_SETTINGS_SATA \
                COMMON_CONFIG_EXTRA_ENV_SETTINGS \
                "bootargs_sata=setenv bootargs ${bootargs_base} " \
                        "root=/dev/sda2 rootwait rw ${lcd} ${hdmi}\0" \
                "bootcmd_sata=run bootargs_sata;run fdt_check;sata init;" \
                        "fatload sata 0 ${loadaddr} ${kernel};fatload sata 0 ${fdt_addr} ${fdt_file};bootm ${loadaddr} - ${fdt_addr}\0" \
                "bootcmd=run bootcmd_sata\0" \
        "\0" 

#define CONFIG_EXTRA_ENV_SETTINGS_MSD \
                COMMON_CONFIG_EXTRA_ENV_SETTINGS \
                "bootargs_msd=setenv bootargs ${bootargs_base} " \
                        "root=/dev/mmcblk1p2 rootwait rw ${lcd} ${hdmi}\0" \
                "bootcmd_msd=run bootargs_msd;run fdt_check;mmc dev 1;" \
                        "fatload mmc 1 ${loadaddr} ${kernel};fatload mmc 1 ${fdt_addr} ${fdt_file};bootm ${loadaddr} - ${fdt_addr}\0" \
                "bootcmd=run bootcmd_msd\0" \
        "\0" 

#define CONFIG_EXTRA_ENV_SETTINGS_SSD \
                COMMON_CONFIG_EXTRA_ENV_SETTINGS \
                "bootargs_ssd=setenv bootargs ${bootargs_base} " \
                        "root=/dev/mmcblk0p2 rootwait rw ${lcd} ${hdmi}\0" \
                "bootcmd_ssd=run bootargs_ssd;run fdt_check;mmc dev 0;" \
                        "fatload mmc 0 ${loadaddr} ${kernel};fatload mmc 0 ${fdt_addr} ${fdt_file};bootm ${loadaddr} - ${fdt_addr}\0" \
                "bootcmd=run bootcmd_ssd\0" \
        "\0" 

#define CONFIG_EXTRA_ENV_SETTINGS_EMMC \
                COMMON_CONFIG_EXTRA_ENV_SETTINGS \
                "bootargs_mmc=setenv bootargs ${bootargs_base} " \
                        "root=/dev/mmcblk2p2 rootwait rw ${lcd} ${hdmi}\0" \
                "bootcmd_mmc=run bootargs_mmc;run fdt_check;mmc dev 2;" \
                        "fatload mmc 2 ${loadaddr} ${kernel};fatload mmc 2 ${fdt_addr} ${fdt_file};bootm ${loadaddr} - ${fdt_addr}\0" \
                "bootcmd=run bootcmd_mmc\0" \
        "\0" 

#endif

#define CONFIG_ARP_TIMEOUT     200UL

/* Miscellaneous configurable options */
#define CONFIG_SYS_LONGHELP
#define CONFIG_SYS_HUSH_PARSER
#define CONFIG_SYS_PROMPT_HUSH_PS2     "> "
#define CONFIG_SYS_PROMPT              "iWave-G15M > "

#define CONFIG_AUTO_COMPLETE
#define CONFIG_SYS_CBSIZE              2048

/* Print Buffer Size */
#define CONFIG_SYS_PBSIZE (CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)
#define CONFIG_SYS_MAXARGS             256
#define CONFIG_SYS_BARGSIZE CONFIG_SYS_CBSIZE

#define CONFIG_CMD_MEMTEST
#define CONFIG_SYS_MEMTEST_START       0x10000000
#define CONFIG_SYS_MEMTEST_END         0x10010000
#define CONFIG_SYS_MEMTEST_SCRATCH     0x10800000

#define CONFIG_SYS_LOAD_ADDR           CONFIG_LOADADDR
#define CONFIG_SYS_HZ                  1000

#define CONFIG_CMDLINE_EDITING
#define CONFIG_STACKSIZE               (128 * 1024)

/* Physical Memory Map */
#define CONFIG_NR_DRAM_BANKS           1
#define PHYS_SDRAM                     MMDC0_ARB_BASE_ADDR

#define PHYS_SDRAM_SIZE                (1u * 1024 * 1024 * 1024)
#define PHYS_SDRAM_SIZE_512MB          (512 * 1024 * 1024)
#define CONFIG_SYS_SDRAM_BASE          PHYS_SDRAM
#define CONFIG_SYS_INIT_RAM_ADDR       IRAM_BASE_ADDR
#define CONFIG_SYS_INIT_RAM_SIZE       IRAM_SIZE

#define CONFIG_SYS_INIT_SP_OFFSET \
	(CONFIG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_ADDR \
	(CONFIG_SYS_INIT_RAM_ADDR + CONFIG_SYS_INIT_SP_OFFSET)

/* FLASH and environment organization */
#define CONFIG_SYS_NO_FLASH

#define CONFIG_FSL_ENV_IN_DYNAMIC
#define CONFIG_ENV_SIZE			(8 * 1024)
#define CONFIG_ENV_SECT_SIZE            (64 * 1024)
#ifndef CONFIG_MFG 
#if defined(CONFIG_FSL_ENV_IN_DYNAMIC)
        #define CONFIG_ENV_IS_IN_DYNAMIC        1
        #define CONFIG_SATA_ENV_DEV             0
        #define CONFIG_ENV_OFFSET               (768 * 1024)
        #define CONFIG_ENV_SECT_SIZE            (64 * 1024)
        #define CONFIG_SYS_USE_SPINOR
        #define CONFIG_ENV_SPI_BUS              CONFIG_SF_DEFAULT_BUS
        #define CONFIG_ENV_SPI_CS               CONFIG_SF_DEFAULT_CS
        #define CONFIG_ENV_SPI_MODE             CONFIG_SF_DEFAULT_MODE
        #define CONFIG_ENV_SPI_MAX_HZ           CONFIG_SF_DEFAULT_SPEED
        #define CONFIG_SYS_DCACHE_OFF
#else
        #define CONFIG_ENV_IS_NOWHERE   1
#endif
#endif

#ifndef CONFIG_SYS_NOSMP
#define CONFIG_SYS_NOSMP
#endif

#define CONFIG_CMD_SATA

#ifdef CONFIG_CMD_SATA
#define CONFIG_DWC_AHSATA
#define CONFIG_SYS_SATA_MAX_DEVICE	1
#define CONFIG_DWC_AHSATA_PORT_ID	0
#define CONFIG_DWC_AHSATA_BASE_ADDR	SATA_ARB_BASE_ADDR
#define CONFIG_LBA48
#define CONFIG_LIBATA
#endif

#ifdef CONFIG_SYS_USE_SPINOR
#define CONFIG_CMD_SF
#define CONFIG_SPI_FLASH
#define CONFIG_SPI_FLASH_SST
#define CONFIG_MXC_SPI
#define CONFIG_SF_DEFAULT_BUS  0
#define CONFIG_SF_DEFAULT_SPEED 20000000
#define CONFIG_SF_DEFAULT_MODE (SPI_MODE_0)
#endif

#define CONFIG_OF_LIBFDT

#ifndef CONFIG_SYS_DCACHE_OFF
#define CONFIG_CMD_CACHE
#endif

/*
 * I2C configs
 */
#define CONFIG_CMD_I2C
#define CONFIG_SYS_I2C
#define CONFIG_HARD_I2C         1
#define CONFIG_I2C_MXC  1
#define CONFIG_SYS_I2C_BASE             I2C2_BASE_ADDR
#define CONFIG_SYS_I2C_SPEED            100000
#define CONFIG_SYS_I2C_SLAVE            0x8

/* Framebuffer */
#undef CONFIG_VIDEO
#undef CONFIG_VIDEO_IPUV3
#undef CONFIG_CFB_CONSOLE
#undef CONFIG_VGA_AS_SINGLE_DEVICE
#undef CONFIG_SYS_CONSOLE_IS_IN_ENV
#undef CONFIG_SYS_CONSOLE_OVERWRITE_ROUTINE
#undef CONFIG_VIDEO_BMP_RLE8
#undef CONFIG_SPLASH_SCREEN
#undef CONFIG_SPLASH_SCREEN_ALIGN
#undef CONFIG_BMP_16BPP
#undef CONFIG_VIDEO_LOGO
#undef CONFIG_VIDEO_BMP_LOGO
#undef CONFIG_IMX_HDMI
#endif                         /* __MX6_IWG15M_COMMON_CONFIG_H */
