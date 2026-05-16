#include "ota_types.h"
#include "fal.h"
#include <string.h>

/* cache for the config partition */
static const struct fal_partition *cfg_part = NULL;

/* =============== internal helpers =============== */

static int cfg_part_ensure(void)
{
    if (cfg_part == NULL) {
        cfg_part = fal_partition_find("config");
        if (cfg_part == NULL) {
            return -1;
        }
    }
    return 0;
}

/* check if a flash region is in erased state (all 0xFF) */
static int flash_is_erased(const uint8_t *data, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        if (data[i] != 0xFF) {
            return 0;
        }
    }
    return 1;
}

/* =============== public API =============== */

int ota_config_load(ota_config_t *cfg)
{
    if (cfg_part_ensure() != 0) {
        return -1;
    }
    return fal_partition_read(cfg_part, 0, (uint8_t *)cfg, sizeof(ota_config_t));
}

int ota_config_save(const ota_config_t *cfg)
{
    if (cfg_part_ensure() != 0) {
        return -1;
    }
    return fal_partition_write(cfg_part, 0, (const uint8_t *)cfg, sizeof(ota_config_t));
}

int ota_config_update_offset(uint32_t offset)
{
    if (cfg_part_ensure() != 0) {
        return -1;
    }
    return fal_partition_write(cfg_part, offsetof(ota_config_t, current_offset),
                               (const uint8_t *)&offset, sizeof(offset));
}

int ota_config_reset(void)
{
    ota_config_t cfg;
    memset(&cfg, 0, sizeof(cfg));
    return ota_config_save(&cfg);
}

int ota_config_set_enable(void)
{
    if (cfg_part_ensure() != 0) {
        return -1;
    }
    uint16_t magic = OTA_ENABLE_MAGIC;
    return fal_partition_write(cfg_part, offsetof(ota_config_t, enable),
                               (const uint8_t *)&magic, sizeof(magic));
}

int ota_config_set_url(const char *url)
{
    if (cfg_part_ensure() != 0) {
        return -1;
    }
    size_t len = strlen(url);
    if (len >= CONFIG_OTA_URL_MAX) {
        len = CONFIG_OTA_URL_MAX - 1;
    }
    /* first write the url, then set enable separately */
    char buf[CONFIG_OTA_URL_MAX];
    memset(buf, 0, sizeof(buf));
    memcpy(buf, url, len);
    return fal_partition_write(cfg_part, offsetof(ota_config_t, url),
                               (const uint8_t *)buf, sizeof(buf));
}

int ota_config_set_total_size(uint32_t size)
{
    if (cfg_part_ensure() != 0) {
        return -1;
    }
    return fal_partition_write(cfg_part, offsetof(ota_config_t, total_size),
                               (const uint8_t *)&size, sizeof(size));
}

int ota_config_url_is_valid(const ota_config_t *cfg)
{
    if (cfg->url[0] == '\0' || (unsigned char)cfg->url[0] == 0xFF) {
        return 0;
    }
    return 1;
}

int ota_config_erase_all(void)
{
    if (cfg_part_ensure() != 0) {
        return -1;
    }
    return fal_partition_erase_all(cfg_part);
}
