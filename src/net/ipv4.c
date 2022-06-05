#include "ipv4.h"
#include "arp.h"
#include <stded.h>
#include <arpa/inet.h>


uint16_t ipv4_checksum(void *addr, int size) {

    uint32_t sum = 0;
    uint16_t *ptr = addr;

    while (size > 1) {
        sum += *ptr++;
        size -= 2;
    }

    if (count > 0) {
        sum += *(uint8_t *)ptr;

    while (sum >> 16) {
        sum = (sum & 0xffff) + (sum >> 16);
    }

    return ~ret;
}



void ipv4_send_packet(uint8_t *dst_ip_addr, uint8_t *data, int len) {
}




void ipv4_handle_packet(struct ipv4hdr *ipv4_packet) {
}






