#include "arp.h"



void arp_receive_packet(uint8_t *data, uint32_t len) {
    struct arp_packet arp_packet;
    memcpy(&arp_packet, data, sizeof(struct arp_packet);
    arp_packet.hardware_type = ntohs(arp_packet.hardware_type);
    arp_protocol_type = ntohs(arp_packet.protocol_type);
    arp_packet.opcode = ntohs(arp_packet.opcode);


    switch (arp_packet.opcode) {
        case ARP_REQUEST:
            break;

        case ARP_REPLY:
            break;

        default:
            kprintf("unrecognized arp opcode\n");
            break;
    }
}


void arp_request_mac_addr() {
}


void arp_reply() {
}
