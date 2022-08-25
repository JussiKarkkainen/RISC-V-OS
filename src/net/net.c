#include "net.h"
#include "tcp.h"
#include "udp.h"
#include "arpa/inet.h"

#define MAX_NET_INTERFACE 1     // For now...

static struct net_interface net_interface[MAX_NET_INTERFACE];

void net_interface_init(void) {
           
    struct net_interface *netif; 
    netif->id = 0;      
    netif->ip = inet_addr("10.0.2.0");
    net_interface[id] = netif; 
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

