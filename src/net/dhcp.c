#include "dhcp.h"


void dhcp_discover() {


    udp_send_packet();
}


void dhcp_request(uint8_t request_ip) {
    
    udp_send_packet();


}



void dhcp_receive_packet(dhcp_header *packet) {
    
    struct dhcp_header dhcp_header;
    memcpy(&dhcp_header, packet, sizeof(dhcp_packet));

    switch (dhcp_header.op) {
        case: DHCP_OFFER_OP
            dhcp_request(&dhcp_header->yiaddr);
        
        default:
            kprintf("Unrecognized dhcp.op %d\n", dhcp_header.op);
    }
}


void dhcp_offer() {
}

