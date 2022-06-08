#include "dns.h"
#include "../libc/include/string.h"
#include "..öibc/include/stdio.h"
#include "../kernel/pmm.h"
#include <arpa/inet.h>
#include <stddef.h>
#include <stdint.h>

static uint16_t dns_id = 1;

int dns_lookup(char *domain, uint8_t ip[4]) {


    // transmit
    struct dns_hdr dns_header;
    dns_header.id = htons(dns_id);
    dns_header.qdcount = htons(0x0001);
    dns_header.flags = htons(0x0100);


    uint16_t packet_len = sizeof(struct dns_hdr) + data_len;
    uint8_t *packet = kalloc(packet_len);
    memcpy(packet, &dns_header, sizeof(struct dns_hdr));
    memcpy(packet + sizeof(struct dns_hdr), data, data_len);

    kfree(data);

    kfree(packet);
    
    dns_id++;




    // Receive
    uint8_t buffer[128];
    int num_bytes_received = recv(dns, buf);            // receive dns message, maybe through sockets, copy to buffer
    
    struct dns_hdr dns_header;
    memcpy(&dns_header, buf, sizeof(struct dns_hdr);
    dns_header.id = ntohs(dns_header.id);
    dns_header.flags = ntohs(dns_header.flags);
    dns_header.qdcount = ntohs(dns_header.qdcount);
    dns_header.ancount = ntohs(dns_header.ancount);
    dns_header.nscount = ntohs(dns_header.nscount);
    dns_header.arcount = ntohs(dns_header.arcount);

    uint8_t dns_data = buf + sizeof(struct dns_hdr):





}



