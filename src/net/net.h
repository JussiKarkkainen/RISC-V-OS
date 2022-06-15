#ifndef NET_H
#define NET_H


struct net_interface {
    uint8_t id;
    uint8_t mac[6];
    uint8_t ip[4];
    uint8_t gateway_mac[6];
    uint8_t gateway_ip[4];
};


#endif
