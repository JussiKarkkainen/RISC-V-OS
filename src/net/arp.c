#include "arp.h"
#include <arpa/inet.h>


void arp_receive_packet(uint8_t *data, uint32_t len) {
    struct arp_packet arp_packet;
    memcpy(&arp_packet, data, sizeof(struct arp_packet));
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


void arp_request_() {
}


void arp_reply(struct arp_packet *request) {

    struct arp_packet arp_reply;
    arp_reply.hardware_type = htons(request->hardware_type);
    arp_reply.hardware_size = 6;
    arp_reply.protocol_type = htons(request->protocol_type);
    arp_reply.protocol_size = 4;
    arp_reply.opcode = htons(ARP_REPLY);

    ethernet_send_frame(request->src_mac, packet, packet_len, ETHERTYPE_ARP); 

}
