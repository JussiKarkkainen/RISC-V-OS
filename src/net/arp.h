#ifndef ARP_H
#define ARP_H

#include "stdint.h"

#define ARP_REQUEST 1
#define ARP_REPLY 2
#define ATHERTYPE_ARP 0x0806

struct arp_packet {
    uint16_t hardware_type;
    uint16_t protocol_type;
    uint8_t hardware_size;
    uint8_t protocol_size;
    uint16_t opcode;
    uint8_t src_mac[6];
    uint8_t src_ip[4];
    uint8_t dst_mac[6];
    uint8_t dst_ip[4];
} __attribute__((packed));


#endif
