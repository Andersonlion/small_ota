#include "ring_buffer.h"
#include <string.h>

// 初始化环形缓冲区
void ring_buffer_init(ring_buffer_t *rb, uint8_t *buf, uint32_t size) {
    rb->buffer = buf;
    rb->head = 0;
    rb->tail = 0;
    rb->size = size;
}

// 清空环形缓冲区
void ring_buffer_clear(ring_buffer_t *rb) {
    rb->head = 0;
    rb->tail = 0;
}

// 判断缓冲区是否为空
bool ring_buffer_is_empty(ring_buffer_t *rb) {
    return (rb->head == rb->tail);
}

// 获取可读数据量
uint32_t ring_buffer_available(ring_buffer_t *rb) {
    if (rb->head >= rb->tail) {
        return rb->head - rb->tail;
    } else {
        return rb->size - rb->tail + rb->head;
    }
}

// 写入任意个字节
// 返回值: 实际写入的字节数
uint32_t ring_buffer_write(ring_buffer_t *rb, const uint8_t *data, uint32_t len) {
    uint32_t free_space;
    uint32_t first_part_len;
    uint32_t write_len;
    
    // 计算可用空间（留一个空位）
    free_space = rb->size - ring_buffer_available(rb) - 1;
    
    // 限制写入长度
    write_len = (len < free_space) ? len : free_space;
    
    if (write_len == 0) {
        return 0;
    }
    
    // 分两段写入（处理环形回绕）
    if (rb->head + write_len <= rb->size) {
        // 不绕弯，直接拷贝
        memcpy(&rb->buffer[rb->head], data, write_len);
    } else {
        // 绕弯，分两段拷贝
        first_part_len = rb->size - rb->head;
        memcpy(&rb->buffer[rb->head], data, first_part_len);
        memcpy(rb->buffer, data + first_part_len, write_len - first_part_len);
    }
    
    // 更新写指针
    rb->head = (rb->head + write_len) % rb->size;
    
    return write_len;
}

// 一次性读取全部数据
// 返回值: 实际读取的字节数
uint32_t ring_buffer_read(ring_buffer_t *rb, uint8_t *out_buf, uint32_t max_len) {
    uint32_t available;
    uint32_t first_part_len;
    uint32_t read_len;
    
    // 计算可读数据量
    available = ring_buffer_available(rb);
    
    if (available == 0) {
        return 0;
    }
    
    // 限制读取长度
    read_len = (available < max_len) ? available : max_len;
    
    // 分两段拷贝（处理环形回绕）
    if (rb->tail + read_len <= rb->size) {
        // 不绕弯，直接拷贝
        memcpy(out_buf, &rb->buffer[rb->tail], read_len);
    } else {
        // 绕弯，分两段拷贝
        first_part_len = rb->size - rb->tail;
        memcpy(out_buf, &rb->buffer[rb->tail], first_part_len);
        memcpy(out_buf + first_part_len, rb->buffer, read_len - first_part_len);
    }
    
    // 更新读指针
    rb->tail = (rb->tail + read_len) % rb->size;
    
    return read_len;
}