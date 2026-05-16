#include "net_dev.h"
#include "net_drv.h"
#include <string.h>
#include <stdio.h>

/* =============== ring buffer implementation =============== */

void net_ring_init(net_ring_buf_t *rb)
{
    rb->head = 0;
    rb->tail = 0;
}

int net_ring_is_empty(const net_ring_buf_t *rb)
{
    return (rb->head == rb->tail);
}

int net_ring_is_full(const net_ring_buf_t *rb)
{
    return ((rb->head + 1) % NET_DRV_RING_BUF_SIZE) == rb->tail;
}

uint16_t net_ring_free(const net_ring_buf_t *rb)
{
    if (rb->head >= rb->tail) {
        return NET_DRV_RING_BUF_SIZE - 1 - (rb->head - rb->tail);
    } else {
        return (rb->tail - rb->head) - 1;
    }
}

uint16_t net_ring_avail(const net_ring_buf_t *rb)
{
    if (rb->head >= rb->tail) {
        return rb->head - rb->tail;
    } else {
        return NET_DRV_RING_BUF_SIZE - (rb->tail - rb->head);
    }
}

int net_ring_put(net_ring_buf_t *rb, uint8_t byte)
{
    uint16_t next = (rb->head + 1) % NET_DRV_RING_BUF_SIZE;
    if (next == rb->tail) {
        return -1;  /* full */
    }
    rb->buf[rb->head] = byte;
    rb->head = next;
    return 0;
}

int net_ring_get(net_ring_buf_t *rb, uint8_t *byte)
{
    if (rb->head == rb->tail) {
        return -1;  /* empty */
    }
    *byte = rb->buf[rb->tail];
    rb->tail = (rb->tail + 1) % NET_DRV_RING_BUF_SIZE;
    return 0;
}

/* =============== module state =============== */

static net_drv_ops_t drv_ops;
static net_ring_buf_t ring_buf;
static int net_opened = 0;

/* =============== low-level helpers =============== */

/* flush ring buffer */
static void ring_flush(void)
{
    net_ring_init(&ring_buf);
}

/* read one byte from ring with poll — calls drv.recv_byte to fill */
static int ring_read_byte(uint32_t timeout_ms)
{
    uint8_t b;
    uint32_t poll_ms = (timeout_ms > 100) ? 100 : timeout_ms;

    while (timeout_ms > 0) {
        if (net_ring_get(&ring_buf, &b) == 0) {
            return b;
        }
        /* poll one byte from driver, feed into ring */
        int c = drv_ops.recv_byte(poll_ms);
        if (c >= 0) {
            net_ring_put(&ring_buf, (uint8_t)c);
        }
        if (timeout_ms > poll_ms) {
            timeout_ms -= poll_ms;
        } else {
            break;
        }
    }
    return -1;
}

/* skip HTTP headers and return pointer to body start in ring.
 * reads from ring buffer dynamically until \r\n\r\n found.
 * Returns body length, -1 on error. */
static int skip_http_header(void)
{
    int state = 0;  /* 0=idle, 1=\r, 2=\r\n, 3=\r\n\r, 4=done (\r\n\r\n) */
    int c;
    while (state != 4) {
        c = ring_read_byte(5000);
        if (c < 0) {
            return -1;
        }
        switch (state) {
        case 0: state = (c == '\r') ? 1 : 0; break;
        case 1: state = (c == '\n') ? 2 : ((c == '\r') ? 1 : 0); break;
        case 2: state = (c == '\r') ? 3 : 0; break;
        case 3: state = (c == '\n') ? 4 : ((c == '\r') ? 1 : 0); break;
        }
    }
    return 0;
}

/* =============== public API =============== */

int net_dev_open(const char *url)
{
    if (net_opened) {
        return -1;
    }

    /* init ring buffer */
    ring_flush();

    /* initialize hardware */
    if (drv_ops.init) {
        if (drv_ops.init() != 0) {
            return -1;
        }
    }

    net_opened = 1;
    (void)url;
    return 0;
}

void net_dev_close(void)
{
    if (!net_opened) {
        return;
    }

    if (drv_ops.deinit) {
        drv_ops.deinit();
    }

    net_opened = 0;
}

int net_dev_http_get_range(uint32_t offset, uint16_t len, uint8_t *buf)
{
    char request[256];
    int req_len;
    int c;
    uint16_t i;

    if (!net_opened) {
        return -1;
    }

    /* flush ring buffer before new request */
    ring_flush();

    /* build HTTP Range GET request */
    req_len = snprintf(request, sizeof(request),
        "GET / HTTP/1.1\r\n"
        "Range: bytes=%lu-%lu\r\n"
        "\r\n",
        (unsigned long)offset,
        (unsigned long)(offset + len - 1));
    if (req_len < 0 || req_len >= (int)sizeof(request)) {
        return -1;
    }

    /* send request */
    if (drv_ops.send == NULL) {
        return -1;
    }
    if (drv_ops.send((const uint8_t *)request, (size_t)req_len) < 0) {
        return -1;
    }

    /* skip HTTP response headers (\r\n\r\n) */
    if (skip_http_header() != 0) {
        return -1;
    }

    /* read body: exactly len bytes */
    for (i = 0; i < len; i++) {
        c = ring_read_byte(10000);
        if (c < 0) {
            return (int)i;  /* partial read */
        }
        buf[i] = (uint8_t)c;
    }

    return len;
}

/* =============== driver registration =============== */

/**
 * Set the driver operations. Must be called before net_dev_open().
 * Typically called by the porting layer.
 */
int net_drv_register(const net_drv_ops_t *ops)
{
    if (ops == NULL) {
        return -1;
    }
    memcpy(&drv_ops, ops, sizeof(net_drv_ops_t));
    return 0;
}
