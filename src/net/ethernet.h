#ifndef ETHERNET_H
#define ETHERNET_H

#include <stdint.h>
#include "net.h"

struct ethernet_hdr {
    unsigned char dst_mac[6];
    unsigned char src_mac[6];
    uint16_t ethertype;
    unsigned char payload[];
}__attribute__((packed));


void ethernet_send_frame(struct net_interface *netif, uint8_t *dst_mac_addr, uint8_t *data, 
                         uint32_t len, uint16_t protocol);
void ethernet_receive_frame(uint8_t *data, uint32_t len);

#endif
