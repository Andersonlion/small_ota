#ifndef _CRC16_H_
#define _CRC16_H_

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * CRC16-MODBUS (polynomial 0x8005)
 * Checksum over data[0..len-1], returns 16-bit CRC value.
 */
uint16_t crc16_modbus(const uint8_t *data, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* _CRC16_H_ */
