#include "net.h"






void net_config(void) {
    net_interface_init();
    udp_init();
    tcp_init();
}





