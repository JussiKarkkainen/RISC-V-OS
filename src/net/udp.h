#ifndef UDP_H
#define UDP_H

#include "net.h"
#include "queue_handler.h"
#include "socket.h"
#include <stdint.h>

#define DHCP_SERVER_PORT 67
#define DHCP_CLIENT_PORT 68

struct udp_header {
    uint16_t src_port;
    uint16_t dst_port;
    uint16_t udp_length;
    uint16_t udp_checksum;
} __attribute__((packed));

struct udp_queue_head {
    uint32_t addr;
    uint16_t port;
    uint16_t len;
    uint8_t data[0];
};

struct udp_control_block {
    int used;
    struct net_interface *netif;
    uint16_t port;
    struct queue_head *qhead;
};

void udp_send_packet(struct net_interface *netif, uint8_t src_port, uint16_t dst_port, 
                     uint32_t ip_addr, uint8_t *data, int len);
void udp_receive_packet(struct net_interface *netif, uint8_t *buf, uint32_t *src_addr,
                        uint32_t *dst_addr, uint32_t payload_len);
void udp_recvfrom(int desc, uint8_t *buf, int n, struct sockaddr *addr, int *addrlen);
void udp_sendto(int desc, uint8_t *buf, int n, struct sockaddr *addr, int *addrlen);
int udp_assign_desc(void);
void udp_init(void);

#endif
