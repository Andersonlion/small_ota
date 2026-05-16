#include "bl_main.h"
#include "bl_jump.h"
#include "ota_core.h"
#include "ota_config.h"
#include "net_dev.h"
#include "fal.h"
#include <string.h>

void bl_main(void)
{
    const struct fal_partition *app_part;
    ota_config_t cfg;

    /* 1. initialize FAL */
    if (fal_init() < 0) {
        goto jump_to_app;
    }

    /* 2. find partitions */
    app_part = fal_partition_find("app");
    if (app_part == NULL) {
        goto jump_to_app;
    }

    /* 3. load OTA config */
    if (ota_config_load(&cfg) < 0) {
        goto jump_to_app;
    }

    /* 4. URL redirect: if invalid, write default */
    if (!ota_config_url_is_valid(&cfg)) {
        memset(cfg.url, 0, sizeof(cfg.url));
        strncpy(cfg.url, CONFIG_DEFAULT_OTA_URL, CONFIG_OTA_URL_MAX - 1);
        ota_config_save(&cfg);
    }

    /* 5. check upgrade enable */
    if (cfg.enable == OTA_ENABLE_MAGIC) {
        if (cfg.total_size > 0 && cfg.total_size < OTA_SLICE_TOTAL_SIZE) {
            goto jump_to_app;
        }

        if (cfg.total_size > 0) {
            if (net_dev_open(cfg.url) == 0) {
                ota_core_run(app_part, &cfg);
                net_dev_close();
            }
        }
    }

jump_to_app:
    bl_jump_to_app(app_part->offset);

    while (1) {}
}
