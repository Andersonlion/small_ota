/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _FAL_CFG_H_
#define _FAL_CFG_H_

#define FAL_DEBUG      0
#define FAL_PART_HAS_TABLE_CFG

/* ===================== Flash device Configuration ========================= */
extern const struct fal_flash_dev onchip_flash;

/* flash device table */
#define FAL_FLASH_DEV_TABLE                                          \
{                                                                    \
    &onchip_flash,                                                   \
}

/* ====================== Partition Configuration ========================== */

/*
 * Flash layout:
 * +---------------+ 0x00000000
 * |  bootloader   |  64 KB
 * +---------------+ 0x00010000
 * |  config       |   4 KB
 * +---------------+ 0x00011000
 * |  app          |  remaining
 * +---------------+
 */

#define _f_boot_start     0
#define _f_boot_size       ( 64 * 1024)

#define _f_conf_start      ( 64 * 1024)
#define _f_conf_size       (  4 * 1024)

#define _f_app_start       ( 68 * 1024)
#define _f_app_size        (444 * 1024)   /* adjust to your actual flash size */

/*----------------------------------------------------------------------------------*/

#ifdef FAL_PART_HAS_TABLE_CFG
/* partition table */
#define FAL_PART_TABLE                                                               \
{                                                                                    \
    {FAL_PART_MAGIC_WORD, "boot",   "onchip",  _f_boot_start,  _f_boot_size, 0}, \
    {FAL_PART_MAGIC_WORD, "config", "onchip",  _f_conf_start,  _f_conf_size, 0}, \
    {FAL_PART_MAGIC_WORD, "app",    "onchip",  _f_app_start,   _f_app_size,  0}, \
}
#endif /* FAL_PART_HAS_TABLE_CFG */

#endif /* _FAL_CFG_H_ */
