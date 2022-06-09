#ifndef TCP_H
#define TCP_H

#include <stdint.h>

struct tcp_header {
    uint16_t src_port;
    uint16_t dst_port;
    uint32_t sequence_num;
    uint32_t acknowledge_num;
    uint8_t header_length : 4;
    uint8_t resv : 4;
    uint8_t flags;
    uint16_t window_size;
    uint16_t tcp_checksum;
    uint16_t urgent_pointer;
    uint32_t options[10];
} __attribute__((packed));

#endif
