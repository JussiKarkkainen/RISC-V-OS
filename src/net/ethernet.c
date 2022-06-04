#include "ethernet.h"
#include "arp.h"
#include "ipv4.h"
#include "../libc/include/string.h"
#include "../libc/include/stdio.h"
#include <stdint.h>
#include <stddef.h>
#include <arpa/inet.h>


void ethernet_send_frame(uint8_t dst_mac_addr, uint8_t *data, uint32_t len, uint16_t protocol) {
    uint8_t src_mac_addr[6];

    struct ethernet_hdr eth_hdr = kalloc(sizeof(ethernet_hdr) + len);
    void *hdr_data = (void *)eth_hdr + sizeof(ethernet_hdr);

    get_mac_addr(src_mac_addr);

    memcpy(eth_hdr->src_mac_addr, src_mac_addr, 6);
    memcpy(eth_hdr->dst_mac_addr, dst_mac_addr, 6);

    memcpy(hdr_data, data, len);

    virtio_net_send_packet(frame, sizeof(ethernet_hdr) + len);
    kfree(frame);
}


void ethernet_receive_frame(uint8_t *data, uint32_t len) {

    struct ethernet_hdr eth_hdr;
    memcpy(&eth_hdr, data, len);
    eth_hdr.ethertype = ntohs(eth_hdr.ethertype);
    int data_len = len - sizeof(ethernet_hdr);

    switch (eth_hdr.ethertype) {
        case ETHERTYPE_ARP:
            arp_receive_packet(data, data_len);
            break;
        
        case ETHERTYPE_IPV4:
            ipv4_receive_packet(data, data_len);
            break;
        
        case ETHERTYPE_IPV6:
            ipv6_receive_packet(data, data_len);
            break;

        default:
            kprintf("Unsupported ethernet frame type=%p\n", eth_hdr.ethertype);
    }

    kfree(data);
}

