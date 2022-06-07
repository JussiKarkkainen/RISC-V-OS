#ifndef DNS_H
#define DNS_H

#include <stdint.h>

struct dns_hdr {
    uint16_t id;
    uint16_t flags;
    uint16_t qdcount;
    uint16_t ancount;
    uint16_t nscount;
    uint16_t arcount;
} __attribute__((packed));

int dns_lookup(char *domain, uint8_t ip[4]);


#endif
