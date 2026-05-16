#ifndef _OTA_CONFIG_H_
#define _OTA_CONFIG_H_

#include "ota_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Read full ota_config_t from config partition.
 * @return >= 0 on success, -1 on error
 */
int ota_config_load(ota_config_t *cfg);

/**
 * Write full ota_config_t to config partition.
 * @return >= 0 on success, -1 on error
 */
int ota_config_save(const ota_config_t *cfg);

/**
 * Update only the current_offset field (used during download).
 * @return >= 0 on success, -1 on error
 */
int ota_config_update_offset(uint32_t offset);

/**
 * Reset config to zero (clear all fields, including enable).
 * @return >= 0 on success, -1 on error
 */
int ota_config_reset(void);

/**
 * Set enable flag to OTA_ENABLE_MAGIC (called by app before reboot).
 * @return >= 0 on success, -1 on error
 */
int ota_config_set_enable(void);

/**
 * Write upgrade URL to config (called by app).
 * @return >= 0 on success, -1 on error
 */
int ota_config_set_url(const char *url);

/**
 * Write total_size to config (called by app).
 * @return >= 0 on success, -1 on error
 */
int ota_config_set_total_size(uint32_t size);

/**
 * Check if url field in loaded config is valid.
 * @return 1 = valid, 0 = erased or empty
 */
int ota_config_url_is_valid(const ota_config_t *cfg);

/**
 * Erase entire config partition.
 * @return >= 0 on success, -1 on error
 */
int ota_config_erase_all(void);

#ifdef __cplusplus
}
#endif

#endif /* _OTA_CONFIG_H_ */
