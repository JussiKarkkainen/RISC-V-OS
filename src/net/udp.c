#include "udp.h"
#include "ipv4.h"
#include "net.h"
#include "../libc/include/string.h"
#include "../libc/include/stdio.h"
#include "../kernel/locks.h"
#include "socket.h"
#include <stddef.h>
#include <stdint.h>
#include "arpa/inet.h"

// udp_recvfrom and udp_sendto are used to interface with socket layer
// while udp_send/receive_packet actually sends the packet

#define UDP_CB_TABLE_SIZE 16

struct spinlock udplock;

struct udp_control_block cb_table[UDP_CB_TABLE_SIZE];

int udp_init() {
    initlock(&udplock, "udplock");
    return 0;
}


void udp_assign_desc(void) {

    struct udp_control_block *cb;
    acquire_lock(&udplock);

    for (i = 0; i < UDP_CB_TABLE_SIZE; i++) {
        cb = cb_table[i];
        if (!cb->used) {
            cb->used = 1;
            release_lock(&udplock);
            return i;
        }
    kprintf("no free udp_control_blocks");
    release_lock(&udplock);
    return -1;
}



void udp_recvfrom(int desc, uint8_t *buf, int n, struct sockaddr *addr, int *addrlen) {


}

void udp_sendto(int desc, uint8_t *buf, int n, struct sockaddr, *addr, int *addrlen) {

    struct sockaddr_in *sin;
    
    sin = (struct sockaddr_in *)addr;
    
    return udp_send_packet(netif, src_port, sin->sin_port, &sin->sin_addr, buf, len);
}



void udp_send_packet(struct net_interface *netif, uint8_t src_port, uint16_t dst_port, 
                     uint32_t ip_addr, uint8_t *data, int len) {

    struct udp_header udp_header;
    udp_header.src_port = src_port;
    udp_header.dst_port = dst_port;
    udp_header.udp_length = htons(sizeof(struct udp_header) + len);
    
    uint16_t *pseudo_header = kalloc(pseudo_hdr_len);
    // struct ipv4_pseudo_hdr pseudo_hdr;

    pseudo_header[0] = netif->ip[0] | (netif->ip[1] << 8);
    pseudo_header[1] = netif->ip[2] | (netif->ip[3] << 8);
    pseudo_header[2] = (uint16_t)(ip_addr >> 16);
    pseudo_header[3] = (uint16_t)(ip_addr);
    pseudo_header[4] = htons(PROTOCOL_TYPE_UDP);
    pseudo_header[5] = htons(len + sizeof(struct udp_header));
    pseudo_header[6] = htons(src_port);
    pseudo_header[7] = htons(dst_port);
    pseudo_header[8] = htons(len + sizeof(struct udp_header));
    
    uint32_t idx = 9;
    for (uint32_t i = 0; i < len; i += 2) {
        if (i + 1 < len) {
            pseudo_header[idx++] = data[i] | (data[i + 1] << 8);
        } else {
            pseudo_header[idx++] = data[i] | (0x0 << 8);
        }
    }

/*
    pseudo_hdr.dst_ipaddr = dst_ip;
    pseudo_hdr.protocol = PROTOCOL_TYPE_UDP;
    pseudo_hdr.udp_len = sizeof(struct udp_header) + len;
    pseudo_hdr.src_port = htons(src_port);
    pseudo_hdr.dst_port = htons(dst_port);
    pseudo_hdr.udp_length = sizeof(struct upd_header) + len;
*/   
 
    // memcpy(pseudo_header, &pseudo_hdr, sizeof(struct ipv4_pseudo_hdr));

    udp_header.checksum = ipv4_checksum(pseudo_header, pseudo_header_len);
   
    kfree(pseudo_header); 

    int data_len = sizeof(struct udp_header) + len;
    uint8_t *data = kalloc(data_len);
    memcpy(data, &udp_header, sizeof(struct udp_header);
    memcpy(data + sizeof(struct udp_header), data, len);

    ipv4_send_packet(netif, dst_addr, data, data_len, IPV4_DF_FLAG, PROTOCOL_TYPE_UDP);

    kfree(data);
}


void udp_receive_packet(struct net_interface netif, uint8_t *buf, uint32_t *src_addr, 
                        uint32_t *dst_addr, uint32_t payload_len) {

    struct udp_control_block *cb;
    void *data;
    struct udp_queue_head *udp_queue_hdr; 
    
    uint8_t *ip_data = packet + (4 * ipv4_header->ihl);
    
    struct udp_header udp_header = { 0 };
    
    memcpy(&udp_header, ip_data, sizeof(struct udp_header));
    udp_header.src_port = ntohs(udp_header.src_port);
    udp_header.dst_port = ntohs(udp_header.dst_port);
    udp_header.len = ntohs(usp_header.len);
    udp_header.checksum = ntohs(udp_header.checksum);

    uint8_t *udp_data = ip_data + sizeof(struct udp_header);  
    
    acquire_lock(&udplock);
    int i;
    for (i = 0; i < UDP_CB_TABLE_SIZE; i++) {
        cb = cb_table[i];
        if (cb->used && cb->port == hdr->dport) {
            if ((data = (void *)kalloc()) == 0) {
                release_lock(&udplock);
                return;
            }
        udp_queue_hdr = data;
        udp_queue_hdr->addr = *src_addr;
        udp_queue_hdr->port = udp_header.src_port;
        udp_queue_hdr->len = payload_len - sizeof(struct udp_header);
        
        memcpy(udp_queue_hdr + 1, udp_header + 1, len - sizeof(struct udp_header))
        push_to_queue(&cb->queue, data, sizeof(struct udp_queue_head) + (payload_len - sizeof(struct udp_header)));
        wakeup(cb);
        release_lock(&udplock);
        return; 
        }
    }
    release_lock(&udplock);
/*
    switch (udp_header.dst_port) {
        case DHCP_CLIENT_PORT:
            dhcp_receive_packet(udp_data);
    
        default:
            kprintf("Unrecognized udp_header.dst_port %d\n", udp_header.dst_port);
    }
*/

}

