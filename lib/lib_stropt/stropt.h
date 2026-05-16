#ifndef _STROPT_H_
#define _STROPT_H_

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Parse Content-Length from HTTP header string.
 * Scans for "Content-Length: " (case-insensitive prefix) and extracts value.
 *
 * @param header     HTTP header buffer
 * @param header_len length of buffer
 * @return Content-Length value, 0 if not found
 */
uint32_t stropt_parse_content_length(const char *header, size_t header_len);

/**
 * Find end of HTTP header (\r\n\r\n).
 *
 * @param data     buffer to search
 * @param len      buffer length
 * @return offset to first body byte (after \r\n\r\n), or -1 if not found
 */
int stropt_find_body_start(const uint8_t *data, size_t len);

/**
 * Check if flash-stored string is valid (not erased, not empty).
 *
 * @param str   null-terminated string from flash
 * @return 1 = valid, 0 = erased or empty
 */
int stropt_is_valid(const char *str);

#ifdef __cplusplus
}
#endif

#endif /* _STROPT_H_ */
