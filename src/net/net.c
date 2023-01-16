#include "net.h"
#include "tcp.h"
#include "udp.h"
#include "arpa/inet.h"
#include "../libc/include/string.h"

struct net_interface net_interface_table[MAX_NET_INTERFACE];

void net_interface_init(void) {
    uint8_t net_f[21];
    memset(&net_f, 0, sizeof(net_f));
    struct net_interface *netif = (struct net_interface *)net_f; 
    netif->id = 0;      
    netif->ip = inet_addr("10.0.2.0");
    //netif->gateway_mac = {0}; 
    net_interface_table[netif->id] = *netif; 
/*
    memcpy(netif->mac, 52:54:00:12:34:56, 6);
    memcpy(netif->ip, ip, 6);
    memcpy(netif->gateway_ip, gateway_ip, 6);

*/
}

void net_config(void) {
    net_interface_init();
    udp_init();
    tcp_init();
}

