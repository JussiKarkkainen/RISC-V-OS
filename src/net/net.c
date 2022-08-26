#include "net.h"
#include "tcp.h"
#include "udp.h"
#include "arpa/inet.h"

void net_interface_init(void) {
    uint8_t net_f[21];
    struct net_interface *netif = (struct net_interface *)net_f; 
    netif->id = 0;      
    netif->ip = inet_addr("10.0.2.0");
    net_interface[netif->id] = *netif; 
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

