#include "arp.h"
#include "ipv4.h"
#include "ethernet.h"
#include "../libc/include/string.h"
#include "arpa/inet.h"
#include <stddef.h>


static uint8_t last_arp_mac_addr[6] = {0};


void arp_receive_packet(uint8_t *data, uint32_t len) {
    struct arp_packet arp_packet;
    memcpy(&arp_packet, data, sizeof(struct arp_packet));
    arp_packet.hardware_type = ntohs(arp_packet.hardware_type);
    arp_protocol_type = ntohs(arp_packet.protocol_type);
    arp_packet.opcode = ntohs(arp_packet.opcode);


    switch (arp_packet.opcode) {
        case ARP_REQUEST:
            // Respond to arp request
            arp_reply(&arp_packet);
            break;

        case ARP_REPLY:
            memcpy(arp_reply_mac_addr, arp_packet.src_mac, 6); 
            break;

        default:
            kprintf("unrecognized arp opcode\n");
            break;
    }
}

// Send arp request
void arp_request(uint8_t ip_addr[4]) {
    
    uint8_t broadcast_mac[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

    struct arp_packet arp_request;
    arp_request.hardware_type = 
    arp_request.protocol_type = htons(ETHERTYPE_IPV4);
    arp_request.hardware_size
   
    uint8_t packet_len = sizeof(struct ap_packet);
    
    arp_request.src_mac = get_mac_addr(); 
    arp_packet.src_ip = ip_addr;
    memcpy(arp_request.dst_mac, broadcast_mac, 6);
    memcpy(arp_request.dst_ip, ip_addr, 4);

    ethernet_send_frame(broadcast_mac, arp_request, packet_len, ETHERTYPE_ARP);

}

// Send arp reply
void arp_reply(struct arp_packet *request) {

    struct arp_packet arp_reply;
    arp_reply.hardware_type = htons(reply->hardware_type);
    arp_reply.hardware_size = 6;
    arp_reply.protocol_type = htons(reply->protocol_type);
    arp_reply.protocol_size = 4;
    arp_reply.opcode = htons(ARP_REPLY);

    arp_reply.src_mac = get_mac_addr();
    arp_reply.src_ip = ip_addr;
    memcpy(arp_reply.dst_mac, request->src_mac, 6);
    memcpy(arp_reply.dst_ip, request->src_ip, 4); 

    uint8_t packet_len = sizeof(struct arp_packet);

    ethernet_send_frame(reply->src_mac, packet, packet_len, ETHERTYPE_ARP); 

}
