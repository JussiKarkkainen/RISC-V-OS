#ifndef DHCP_H
#define DHCP_H


struct dhcp_header {
    uint8_t op;
    uint8_t hw_type;
    uint8_t hw_len;
    uint8_t hops;
    uint32_t transaction_id;
    uint16_t sec;
    uint16_t flags;
    uint32_t ciaddr;
    uint32_t yiaddr;
    uint32_t siaddr;
    uint32_t giaddr;
    uint8_t chaddr[16];
    uint8_t sname[64];
    uint8_t boot_file_name[128];
    uint8_t magic_cookie[4];
} __attribute__((packed));


void dhcp_discover();
void dhcp_request(uint8_t request_ip);
void dhcp_receive_packet();
void dhcp_offer();

#endif
