#include "ota_core.h"
#include "ota_config.h"
#include "net_dev.h"
#include "crc16.h"

int ota_core_run(const struct fal_partition *app_part, ota_config_t *cfg)
{
    uint32_t total_size    = cfg->total_size;
    uint32_t current_off   = cfg->current_offset;
    uint32_t total_slices  = total_size / OTA_SLICE_TOTAL_SIZE;
    uint32_t current_slice;
    uint32_t http_offset;
    uint32_t flash_offset;
    uint8_t  buf[OTA_SLICE_TOTAL_SIZE];  /* 514 bytes receive buffer */
    uint16_t crc_calc;
    uint16_t crc_expected;
    int      ret;

    /* validate total_size is integral multiple of slice size */
    if (total_size % OTA_SLICE_TOTAL_SIZE != 0) {
        return -1;
    }

    /* erase app partition at download start (once) */
    if (current_off == 0) {
        ret = fal_partition_erase_all(app_part);
        if (ret < 0) {
            return -1;
        }
    }

    /* calculate starting position */
    current_slice = current_off / OTA_SLICE_DATA_SIZE;

    for (; current_slice < total_slices; current_slice++) {
        http_offset   = current_slice * OTA_SLICE_TOTAL_SIZE;
        flash_offset  = current_slice * OTA_SLICE_DATA_SIZE;

        /* request one slice (514 bytes) from server */
        ret = net_dev_http_get_range(http_offset, OTA_SLICE_TOTAL_SIZE, buf);
        if (ret != OTA_SLICE_TOTAL_SIZE) {
            return -1;
        }

        /* CRC16 over first 512 bytes */
        crc_calc = crc16_modbus(buf, OTA_SLICE_DATA_SIZE);

        /* expected CRC from last 2 bytes (little-endian) */
        crc_expected = (uint16_t)buf[OTA_SLICE_DATA_SIZE]
                     | ((uint16_t)buf[OTA_SLICE_DATA_SIZE + 1] << 8);

        if (crc_calc != crc_expected) {
            return -1;
        }

        /* write 512 data bytes to app partition */
        ret = fal_partition_write(app_part, flash_offset, buf, OTA_SLICE_DATA_SIZE);
        if (ret < 0) {
            return -1;
        }

        /* update progress */
        current_off += OTA_SLICE_DATA_SIZE;
        cfg->current_offset = current_off;
        ota_config_update_offset(current_off);
    }

    /* download complete — clear config */
    ota_config_reset();

    return 0;
}
