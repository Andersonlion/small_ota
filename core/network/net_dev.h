#ifndef _NET_DEV_H_
#define _NET_DEV_H_

#include <stdint.h>
#include "net_drv.h"

#ifdef __cplusplus
extern "C" {
#endif

/* =============== driver registration (call before net_dev_open) =============== */

int net_drv_register(const net_drv_ops_t *ops);

/* =============== network device abstract API =============== */

/**
 * Open network device and connect to target server.
 * Extracts host from url, initializes hardware, connects to module.
 *
 * @param url   full firmware URL (e.g. http://server/firmware.bin)
 * @return 0 = success, -1 = error
 */
int net_dev_open(const char *url);

/**
 * Close network device and release hardware.
 */
void net_dev_close(void);

/**
 * HTTP Range GET — blocking.
 * Sends GET with Range: bytes=offset-(offset+len-1), waits for response,
 * extracts only the body data into buf.
 *
 * @param offset   byte offset of range start
 * @param len      number of bytes to request
 * @param buf      output buffer (size >= len)
 * @return  actual bytes received (>0), 0 = closed, -1 = error
 */
int net_dev_http_get_range(uint32_t offset, uint16_t len, uint8_t *buf);

#ifdef __cplusplus
}
#endif

#endif /* _NET_DEV_H_ */
