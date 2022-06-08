#include "dns.h"
#include "net.h"
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
    
    // Tansform domain name into DNS query name, by replacing dots with numbers
    uint32_t domain_len = strlen(domain);
    uint32_t data_len = (1 + domain_len + 5) * sizeof(uint8_t);

    char *tmp_domain = kalloc((domain_len + 1) * sizeof(char));
    strcat(tmp_domain, domain);
    strcat(tmp_domain, ".");

    uint32_t j = 0, pos = 0;    
    for (uint32_t i = 0; i < domain_len + 1; i++) {
        if (tmp_domain[i] == '.') {
            data[j++] = i - pos;
            
            while (pos < i) {
                data[j++] = tmp_domain[pos++];
            }
            pos++;
        }
    }

    // END
    data[j++] = 0;
    // Type
    data[j++] = (uint8_t)(DNS_TYPE_A << 8);
    data[j++] = (uint8_t)(DNS_TYPE_A);
    // Class
    data[j++] = (uint8_t)(DNS_CLASS_IN << 8);
    data[j++] = (uint8_t)(DNS_CLASS_IN);

    kfree(tmp_domain);

    uint16_t packet_len = sizeof(struct dns_hdr) + data_len;
    uint8_t *packet = kalloc(packet_len);
    memcpy(packet, &dns_header, sizeof(struct dns_hdr));
    memcpy(packet + sizeof(struct dns_hdr), data, data_len);

    kfree(data);

    int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT_DNS);
    server_addr.sin_addr.s_addr = inet_addr2(interface->dns_ip);
    
    socklen_t server_addr_len = sizeof(struct sockaddr_in);

    if (sendto(sockfd,
             packet,
             packet_len,
             0,
             (struct sockaddr*)&server_addr,
             server_addr_len) < 0) {
        
        return -1;
    }
    
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



