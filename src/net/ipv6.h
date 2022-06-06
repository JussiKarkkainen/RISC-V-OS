#ifndef IPV6_H
#define IPV6_H

struct ipv6hdr {
    uint8_t version : 4;
    uint8_t dsfield : 6;
    uint8_t ecn : 2;
    uint32_t flow_label : 20;
    uint16_t payload_length;
    uint8_t next_header;
    uint8_t hop_limit;
    uint8_t src_ip[16];
    uint8_t dst_ip[16];
} __attribute__((packed));

#endif
