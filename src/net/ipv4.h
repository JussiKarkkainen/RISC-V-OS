#ifndef IPV4_H
#define IPV4_H

#define ETHERTYPE_IPV4 0x0800
#define PROTOCOL_TYPE_UDP 17
#define PROTOCOL_TYPE_TCP 6

struct ipv4hdr {
    uint8_t version : 4;
    uint8_t ihl : 4;
    uint8_t tos;
    uint16_t len;
    uint16_t id;
    uint16_t flags : 3;
    uint16_t frag_offset : 13;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t csum;
    uint32_t srcaddr;
    uint32_t dstaddr;
} __attribute__((packed));


#endif
