#ifndef UDP_H
#define UDP_H

#include <stdint.h>

struct udp_header {
    uint16_t src_port;
    uint16_t dst_port;
    uint16_t udp_length;
    uint16_t udp_checksum;
} __attribute__((packed));


#endif
