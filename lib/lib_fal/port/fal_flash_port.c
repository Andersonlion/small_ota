/*
 * Flash device port template.
 * Copy this file, implement the ops callbacks for your target MCU,
 * and define the onchip_flash structure.
 */

#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "fal.h"

/* =============== sysprintf stub =============== */
/* Override this with your platform's printf if needed. */
void sysprintf(char *format, ...)
{
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

/* =============== flash device ops =============== */

static int init(void)
{
    /* TODO: initialize your MCU's flash controller here */
    return 0;
}

static int read(long offset, uint8_t *buf, size_t size)
{
    /* TODO: read from flash at addr (onchip_flash.addr + offset) */
    (void)offset;
    (void)buf;
    (void)size;
    return 0;
}

static int write(long offset, const uint8_t *buf, size_t size)
{
    /* TODO: write to flash at (onchip_flash.addr + offset).
     *       Must handle alignment (this MCU's write_gran).
     *       Return size on success, -1 on error. */
    (void)offset;
    (void)buf;
    (void)size;
    return 0;
}

static int erase(long offset, size_t size)
{
    /* TODO: erase flash from (onchip_flash.addr + offset).
     *       Align to blk_size before erasing.
     *       Return size on success, -1 on error. */
    (void)offset;
    (void)size;
    return 0;
}

/* =============== flash device definition =============== */

const struct fal_flash_dev onchip_flash =
{
    .name       = "onchip",
    .addr       = 0,                /* TODO: set your flash base address */
    .len        = 512 * 1024,       /* TODO: set your total flash size */
    .blk_size   = 4 * 1024,         /* TODO: set minimum erase block size */
    .ops        = {init, read, write, erase},
    .write_gran = 8,                /* TODO: set write granularity in bits
                                     *   1 = NOR, 8 = STM32F2/F4,
                                     *   32 = STM32F1, 64 = STM32L4 */
};
