#ifndef ETHERNET_H
#define ETHERNET_H

#include <stdint.h>

struct ethernet_hdr {
    unsigned char dst_mac[6];
    unsigned char src_mac[6];
    uint16_t ethertype;
    unsigned char payload[];
}__attribute__((packed));


void ethernet_send_frame();
void ethernet_receive_frame();


#endif
