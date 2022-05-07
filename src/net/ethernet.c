#include "ethernet.h"
#include <stdint.h>
#include <stddef.h>
#include <arpa/inet.h>

void ethernet_send_frame(uint8_t dst_mac_addr, uint8_t *data, uint32_t len, uint16_t protocol) {
    uint8_t src_mac_addr[6];

    struct ethernet_hdr eth_hdr = kalloc();

    virtio_net_send(frame, sizeof(ethernet_hdr) + len);
}


void ethernet_receive_frame(uint8_t *data, uint32_t len) {

    struct ethernet_hdr eth_hdr;

    switch (eth_hdr.ethertype) {
        case ETHERTYPE_ARP:
            arp_receive_packet();
            break;
        
        case ETHERTYPE IPV4:
            ipv4_receive_packet();
            break;

        default:
            kprintf("Unsupported ethernet frame type=%p\n", eth_hdr.ethertype);
    }

    kfree(data);
}







