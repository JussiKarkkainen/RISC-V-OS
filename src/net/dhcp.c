#include "dhcp.h"
#include "udp.h"
#include "../libc/include/string.h"
#include "../libc/include/stdio.h"
#include <stddef.h>
#include "arpa/inet.h"

static uint32_t xid = 1;

void dhcp_discover() {
    
    xid++;

    struct dhcp_header dhcp_header;
    dhcp_header.op = DHCP_DISCOVER;
    dhcp_header.hlen = DHCP_HLEN;
    dhcp_header.hops = 0;
    dhcp_header.xid = htonl(xid);
    dhcp_header.magic_cookie = htonl(DHCP_MAGIC_COOKIE);
    dhcp_header.options = {DHCP_MESSAGE_TYPE, 0x01, DHCP_DISCOVER, 0xff};

    uint8_t *packet = kalloc(sizeof(struct dhcp_header));
    memcpy(packet, &dhcp_header, sizeof(struct dhcp_header));

    udp_send_packet(DHCP_CLIENT_PORT, packet, sizeof(struct dhcp_header);

    kfree(packet);
}


void dhcp_request(uint8_t request_ip) {
    
    udp_send_packet();


}



void dhcp_receive_packet(struct dhcp_header *packet) {
    
    struct dhcp_header dhcp_header;
    memcpy(&dhcp_header, packet, sizeof(dhcp_packet));
    dhcp_header.xid = ntohl(dhcp_header.xid);

    if (dhcp_header.xid != xid) {
        kprintf("Transaction id doesn't match: dhcp_header.xid: %d should be: %d\n", dhcp_header.xid, xid);
        return;
    }

    switch (dhcp_header.op) {
        case: DHCP_OFFER_OP
            dhcp_request(&dhcp_header->yiaddr);
        
        default:
            kprintf("Unrecognized dhcp.op %d\n", dhcp_header.op);
    }
}


void dhcp_offer() {
}

