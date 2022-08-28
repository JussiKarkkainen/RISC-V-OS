#ifndef NET_H
#define NET_H

#include <stdint.h>

struct net_interface {
    uint8_t id;
    uint8_t mac[6];
    uint32_t ip;
    uint8_t gateway_mac[6];
    uint8_t gateway_ip[4];
};

#define MAX_NET_INTERFACE 1     // For now...
static const struct net_interface net_interface_table[MAX_NET_INTERFACE];

uint32_t inet_addr(const char *cp); 
void net_config(void);

#endif
