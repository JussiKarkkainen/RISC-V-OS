#include "ipv4.h"
#include "arp.h"
#include "net.h"
#include "ethernet.h"
#include "arpa/inet.h"
#include "udp.h"
#include "tcp.h"
#include "../kernel/paging.h"
#include "../libc/include/stdio.h"
#include "../libc/include/string.h"
#include <stddef.h>

// https://www.rfc-editor.org/rfc/rfc1071
uint16_t ipv4_checksum(void *addr, int count, uint32_t init) {

    uint32_t sum = init;
    uint16_t *ptr = addr;

    while (count > 1) {
        sum += *ptr++;
        count -= 2;
    }

    if (count > 0) {
        sum += *(uint8_t *)ptr;
    }
    while (sum >> 16) {
        sum = (sum & 0xffff) + (sum >> 16);
    }

    return ~sum;
}

static uint16_t ipv4_id = 1;

void ipv4_send_packet(struct net_interface *netif, uint32_t dst_ip_addr, uint8_t *data, 
                      int len, uint16_t flags, uint8_t protocol) {

   // int packet_len = sizeof(struct ipv4hdr) + len;
    struct ipv4hdr ipv4_header;
    
    ipv4_header.version = IPV4_VERSION;
    ipv4_header.ihl = 5;
    ipv4_header.tos = 0;
    ipv4_header.len = sizeof(struct ipv4hdr) + len;
    ipv4_header.id = htons(ipv4_id);
    ipv4_header.flags = htons(flags);
    ipv4_header.ttl = IPV4_TTL;
    ipv4_header.protocol = protocol;
    ipv4_header.src_addr = netif->ip;
    ipv4_header.dst_addr = dst_ip_addr;
    ipv4_header.csum = ipv4_checksum(&ipv4_header, sizeof(struct ipv4hdr), 0); 

    ipv4_id++;

    uint32_t *packet = kalloc();
    memcpy(packet, &ipv4_header, sizeof(struct ipv4hdr));
    memcpy(packet + sizeof(struct ipv4hdr), data, len);
    
 //   uint8_t dst_mac_addr[6] = {0};    // isn't actually 0 

//    ethernet_send_frame(dst_mac_addr, packet, packet_len, ETHERTYPE_IPV4);
    
    kfree(packet); 
}


void ipv4_handle_packet(struct net_interface *netif, uint8_t *data, uint32_t data_len) {
    
    struct ipv4hdr *ipv4_header;
//    uint16_t hdr_len;

    ipv4_header = (struct ipv4hdr *)data;
//    hdr_len = ipv4_header->ihl << 2;          // hdr len in bytes
    
//    uint8_t *payload = ((uint8_t *)ipv4_header) + hdr_len;
//    uint32_t payload_len = ntohs(ipv4_header->len) - hdr_len;
    
    if (!ipv4_header->ttl) {
        kprintf("IPV4 time to live = 0");
        return;
    }

    if ((ntohs(ipv4_header->fragment_offset) & 0x2000) || (ntohs(ipv4_header->fragment_offset) & 0x1fff)) {
        kprintf("IP fragments not supported\n");
        return;
    }

    switch (ipv4_header->protocol) {
/*
        case PROTOCOL_TYPE_UDP:
            udp_receive_packet(netif, payload, (uint32_t*)&ipv4_header->src_addr, 
                               (uint32_t*)&ipv4_header->dst_addr, payload_len);
            break;
        case PROTOCOL_TYPE_TCP:
            tcp_receive_packet(netif, payload, (uint32_t*)&ipv4_header->src_addr, 
                               (uint32_t*)&ipv4_header->dst_addr, payload_len);
            break;
        
*/
        default:
            kprintf("Unrecognized ipv4 header protocol %p\n", ipv4_header->protocol);
    }


}
