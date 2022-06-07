#include "udp.h"
#include "ipv4.h"
#include "../libc/include/string.h"
#include "../libc/include/stdio.h"
#include <stddef.h>
#include <stdint.h>
#include <arpa/inet.h>


uint16_t udp_checksum(struct udp_header *udp_header) {
}


void udp_send_packet(uint8_t dst_ip, uint16_t src_port, uint16_t dst_port, void *data, int len) {

    struct udp_header udp_header;
    udp_header.src_port = src_port;
    udp_header.dst_port = dst_port;
    udp_header.udp_length = htons(sizeof(struct udp_header) + len);
    
    uint8_t *pseudo_header = kalloc(pseudo_hdr_len);
    struct ipv4_pseudo_hdr pseudo_hdr;

    pseudo_hdr.dst_ipaddr = dst_ip;
    pseudo_hdr.protocol = PROTOCOL_TYPE_UDP;
    pseudo_hdr.udp_len = sizeof(struct udp_header) + len;
    pseudo_hdr.src_port = htons(src_port);
    pseudo_hdr.dst_port = htons(dst_port);
    pseudo_hdr.udp_length = sizeof(struct upd_header) + len;
     
    memcpy(pseudo_header, &pseudo_hdr, sizeof(struct ipv4_pseudo_hdr));

    usp_header.checksum = ipv4_checksum(pseudo_header, pseudo_header_len);
   
    kfree(pseudo_header); 

    int data_len = sizeof(struct udp_header) + len;
    uint8_t *data = kalloc(data_len);
    memcpy(data, &udp_header, sizeof(struct udp_header);
    
    ipv4_send_packet(dst_addr, data, data_len, IPV4_DF_FLAG, PROTOCOL_TYPE_UDP);

    kfree(data);
}


void udp_receive_packet(struct udp_header packet) {

    void *udp_data = (void *)packet + sizeof(struct udp_header); 

    switch (udp_header.dst_port) {
        case DHCP_CLIENT_PORT:
            dhcp_receive_packet(udp_data);
    
        default:
            kprintf("Unrecognized udp_header.dst_port %d\n", udp_header.dst_port);
    }
}


