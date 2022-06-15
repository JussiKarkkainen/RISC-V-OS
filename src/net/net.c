#include "net.h"

#define MAX_NET_INTERFACE 1     // For now...

static struct net_interface net_interface[MAX_NET_INTERFACE];

void net_interface_init(void) {
           
    struct net_interface *netif = kalloc(); 
    netif->id = 0;
    memcpy(netif->mac, get_driver_mac_addr(), 6);
    memcpy(netif->ip, ip, 6);
    memcpy(netif->gateway_ip, gateway_ip, 6);

    net_interface[id] = netif; 



void net_config(void) {
    net_interface_init();
    udp_init();
    tcp_init();
}





