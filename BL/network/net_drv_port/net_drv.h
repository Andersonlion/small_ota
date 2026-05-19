/**
 * Low-level driver operations.
 * Implement these for your UART/SPI/I2C hardware.
 * All operations are synchronous (blocking).
 * you need to use send_bytes() to ring_buffer or ring_buffer() of net_dev in interrupt or other tasks to communicate with tcp server.
 * 
 */

#ifndef _NET_DRV_H_
#define _NET_DRV_H_

#include <stdint.h>
#include <stddef.h>
#include "lib.h"
extern ring_buffer_t rb;
#ifdef __cplusplus
extern "C" {
#endif


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
    int  (*send)(const uint8_t *data,size_t len);

    /**
     * Receive one byte from hardware (blocking with timeout).
     * @param timeout_ms  max wait time in milliseconds, 0 = no wait
     * @return byte value (0..255), -1 = timeout / no data
     */
    int  (*recv_byte)(uint32_t timeout_ms);

} net_drv_ops_t;

/*connect to server ,this func will be call in a while block*/
int connect_to_TCPServer();

/*send msg to TCPServer*/
int TCP_send_msg();

/*throw msg to ring buffer*/
int TCP_read_msg(ring_buffer_t* rb);



#ifdef __cplusplus
}
#endif

#endif /* _NET_DRV_H_ */
