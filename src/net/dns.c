#include "dns.h"
#include "../libc/include/string.h"
#include "..öibc/include/stdio.h"
#include "../kernel/pmm.h"
#include <arpa/inet.h>
#include <stddef.h>
#include <stdint.h>

static uint16_t dns_id = 1;

int dns_lookup(char *domain, uint8_t ip[4]) {

    struct dns_hdr dns_header;
    dns_header.id = htons(dns_id);
    dns_header.qdcount = htons(0x0001);
    dns_header.flags = htons(0x0100);


    uint16_t packet_len = sizeof(dns_hdr) + data_len;
    uint8_t *packet = kalloc(packet_len);
    memcpy(packet, &dns_header, sizeof(dns_hdr));
    memcpy(packet + sizeof(dns_hdr), data, data_len);

    kfree(data);

    kfree(packet);

}



