#ifndef TCP_H
#define TCP_H

#include <stdint.h>

struct tcp_header {
    uint16_t src_port;
    uint16_t dst_port;
    uint32_t sequence_num;
    uint32_t acknowledge_num;
    uint8_t data_offset;
    uint8_t flags;
    uint16_t window_size;
    uint16_t tcp_checksum;
    uint16_t urgent_pointer;

} __attribute__((packed));

#endif
