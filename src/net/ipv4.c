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



void ipv4_send_packet(uint8_t *dst_ip_addr, uint8_t *data, int len) {

    int packet_len = sizeof(ipv4hdr) + len;
    struct ipv4hdr ipv4_header;
    

    uint32_t *packet = kalloc(packet_len);
    memcpy(packet, &ipv4_header, sizeof(ipv4hdr));
    memcpy(packet + sizeof(ipv4hdr), data, len);

    ethernet_send_frame(dst_mac_addr, packet, packet_len, ETHERTYPE_IPV4);
    kfree(packet); 
}




void ipv4_handle_packet(struct ipv4hdr *ipv4_packet) {

    struct ipv4hdr ipv4_header;
    memcpy(&ipv4_header, data, sizeof(ipv4hdr));

    switch (ipv4_header.protocol) {
        case PROTOCOL_TYPE_UDP;
            udp_receive_packet(&ipv4_header, data);
            break;
        case PROTOCOL_TYPE_TCP;
            tcp_receive_packet(&ipv4_header, data);
            break;
        
        default:
            kprintf("Unrecognized ipv4 header protocol");
    }
}






