#ifndef _OTA_CORE_H_
#define _OTA_CORE_H_

#include <stdint.h>
#include "ota_types.h"
#include "fal.h"

/**
 * Run OTA download sequence.
 * Precondition: net_dev already open, config loaded with valid enable.
 * On success, clears enable/total_size and returns 0.
 * On failure, keeps current_offset for resume and returns -1.
 *
 * @param app_part   pointer to "app" partition (from fal_partition_find)
 * @param cfg        loaded config (will be updated in flash as download progresses)
 * @return 0 = success, -1 = error (resumable)
 */
int ota_core_run(const struct fal_partition *app_part, ota_config_t *cfg);

#endif /* _OTA_CORE_H_ */
