#include "ipv4.h"
#include "arp.h"
#include "../libc/include/stdio.h"
#include "../libc/include/string.h"
#include <stdef.h>
#include <arpa/inet.h>

uint16_t ipv4_checksum(void *addr, int size) {

    uint32_t sum = 0;
    uint16_t *ptr = addr;

    while (size > 1) {
        sum += *ptr++;
        size -= 2;
    }

    if (count > 0) {
        sum += *(uint8_t *)ptr;

    while (sum >> 16) {
        sum = (sum & 0xffff) + (sum >> 16);
    }

    return ~ret;
}

static uint16_t ipv4_id = 1;

void ipv4_send_packet(uint8_t *dst_ip_addr, uint8_t *data, int len, uint16_t flags, uint8_t protocol) {

    int packet_len = sizeof(struct ipv4hdr) + len;
    struct ipv4hdr ipv4_header;
    
    ipv4_header.version = IPV4_VERSION;
    ipv4_hedaer.ihl = 5;
    ipv4_header.service = 0;
    ipv4_header.len = sizeof(struct ipv4hdr) + len;
    ipv4_header.id = htons(ipv4_id);
    ipv4_header.flags = htons(flags);
    ipv4_header.ttl = IPV4_TTL;
    ipv4_header.protocol = protocol;
    ipv4_header.src_addr = src_addr;
    ipv4_header.dst_addr = dst_ip_addr;
    ipv4_header.checksum = ipv4_checksum(&ipv4_header, sizeof(struct ipv4hdr) 

    ipv4_id++;

    uint32_t *packet = kalloc(packet_len);
    memcpy(packet, &ipv4_header, sizeof(struct ipv4hdr));
    memcpy(packet + sizeof(struct ipv4hdr), data, len);

    ethernet_send_frame(dst_mac_addr, packet, packet_len, ETHERTYPE_IPV4);
    
    kfree(packet); 
}




void ipv4_handle_packet(struct ipv4hdr *ipv4_packet) {

    struct ipv4hdr ipv4_header;
    memcpy(&ipv4_header, data, sizeof(struct ipv4hdr));

    switch (ipv4_header.protocol) {
        case PROTOCOL_TYPE_UDP;
            udp_receive_packet(&ipv4_header, data);
            break;
        case PROTOCOL_TYPE_TCP;
            tcp_receive_packet(&ipv4_header, data);
            break;
        
        default:
            kprintf("Unrecognized ipv4 header protocol %p\n", ipv4_header.protocol);
    }
}






