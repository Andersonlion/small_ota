#ifndef _NET_DRV_H_
#define _NET_DRV_H_

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Low-level driver operations.
 * Implement these for your UART/SPI/I2C hardware.
 * All operations are synchronous (blocking).
 */

/* ring buffer capacity (tune per platform) */
#ifndef NET_DRV_RING_BUF_SIZE
#define NET_DRV_RING_BUF_SIZE   1024
#endif

/* driver ops — implement these callbacks */
typedef struct {
    /**
     * Initialize hardware (UART/SPI/I2C), configure external network module.
     * @return 0 = success, -1 = error
     */
    int  (*init)(void);

    /**
     * Deinitialize hardware, put module to sleep.
     */
    void (*deinit)(void);

    /**
     * Send raw bytes to network module (blocking, all bytes sent).
     * @param data   bytes to send
     * @param len    number of bytes
     * @return actual bytes sent, -1 = error
     */
    int  (*send)(const uint8_t *data, size_t len);

    /**
     * Receive one byte from hardware (blocking with timeout).
     * @param timeout_ms  max wait time in milliseconds, 0 = no wait
     * @return byte value (0..255), -1 = timeout / no data
     */
    int  (*recv_byte)(uint32_t timeout_ms);
} net_drv_ops_t;

/* =============== ring buffer (internal use by net_dev) =============== */

typedef struct {
    uint8_t buf[NET_DRV_RING_BUF_SIZE];
    volatile uint16_t head;   /* write index (ISR or polling) */
    volatile uint16_t tail;   /* read index (consumer) */
} net_ring_buf_t;

void     net_ring_init(net_ring_buf_t *rb);
int      net_ring_is_empty(const net_ring_buf_t *rb);
int      net_ring_is_full(const net_ring_buf_t *rb);
uint16_t net_ring_free(const net_ring_buf_t *rb);
uint16_t net_ring_avail(const net_ring_buf_t *rb);
int      net_ring_put(net_ring_buf_t *rb, uint8_t byte);
int      net_ring_get(net_ring_buf_t *rb, uint8_t *byte);

#ifdef __cplusplus
}
#endif

#endif /* _NET_DRV_H_ */
