#ifndef NET_H
#define NET_H

#include <stdint.h>

struct net_interface {
    uint8_t id;
    uint8_t mac[6];
    uint8_t ip[4];
    uint8_t gateway_mac[6];
    uint8_t gateway_ip[4];
};

uint32_t inet_addr(const char *cp); 
void net_config(void);

#endif
