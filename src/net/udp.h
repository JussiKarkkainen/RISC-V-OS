#ifndef UDP_H
#define UDP_H

#include <stdint.h>

#define DHCP_SERVER_PORT 67
#define DHCP_CLIENT_PORT 68

struct udp_header {
    uint16_t src_port;
    uint16_t dst_port;
    uint16_t udp_length;
    uint16_t udp_checksum;
} __attribute__((packed));

udp_send_packet(uint8_t dst_ip, uint16_t src_port, uint16_t dst_port, void *data, int len);
udp_receive_packet(struct *udp_header udp_header);

#endif
