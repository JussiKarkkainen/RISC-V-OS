#ifndef IPV4_H
#define IPV4_H

#include "net.h"
#include <stdint.h>

#define ETHERTYPE_IPV4 0x0800
#define PROTOCOL_TYPE_UDP 17
#define PROTOCOL_TYPE_TCP 6
#define IPV4_VERSION 4
#define IPV4_TTL 64

struct ipv4hdr {
    uint8_t version : 4;
    uint8_t ihl : 4;
    uint8_t tos;
    uint16_t len;
    uint16_t id;
    uint16_t flags : 3;
    uint16_t fragment_offset : 13;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t csum;
    uint32_t src_addr;
    uint32_t dst_addr;
};

struct ipv4_pseudo_hdr {
    uint32_t src_ipaddr;
    uint32_t dst_ipaddr;
    uint8_t zeroes;
    uint8_t protocol;
    uint16_t udp_len;
    uint16_t src_port;
    uint16_t dst_port;
    uint16_t udp_length;
    uint16_t checksum;
};

uint16_t ipv4_checksum(void *addr, int size, uint32_t init);
void ipv4_send_packet(struct net_interface *netif, uint32_t dst_ip_addr, uint8_t *data, int len,
                      uint16_t flags, uint8_t protocol);
void ipv4_handle_packet(struct net_interface *netif, uint8_t *data, uint32_t data_len);

#endif
