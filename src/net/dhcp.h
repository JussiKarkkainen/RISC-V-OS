#ifndef DHCP_H
#define DHCP_H


void dhcp_discover();
void dhcp_request(uint8_t request_ip);
void dhcp_receive_packet();
void dhcp_offer();

#endif
