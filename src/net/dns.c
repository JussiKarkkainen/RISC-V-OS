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
    
    uint32_t server_addr_len = sizeof(struct sockaddr_in);

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
    int num_bytes_received = recvfrom(sockfd, buf, sizeof(buf), 0, 
                                     (struct sockaddr*)&server_addr, 
                                     &server_addr_len);            // receive dns message, maybe through sockets, copy to buffer
   
    close(sockfd);

    struct dns_hdr dns_header;
    memcpy(&dns_header, buf, sizeof(struct dns_hdr));
    dns_header.id = ntohs(dns_header.id);
    dns_header.flags = ntohs(dns_header.flags);
    dns_header.qdcount = ntohs(dns_header.qdcount);
    dns_header.ancount = ntohs(dns_header.ancount);
    dns_header.nscount = ntohs(dns_header.nscount);
    dns_header.arcount = ntohs(dns_header.arcount);

    uint8_t dns_data = buf + sizeof(struct dns_hdr):


    uint16_t query_len = 0;
    for (uint16_t query = 0; query < dns_header.qdcount; query++) {
        while (dns_data[query_len++] != 0x00) {
            ;
        }
        query_len += 2; // type
        query_len += 2; // class
    }

    if (dns_ans_hdr.ancount > 0) {
        struct dns_ans_hdr ans_header;
        memcpy(&ans_header, dns_data + query_len + sizeof(struct dns_ans_hdr));
        ans_header.name = ntohs(ans_header.name);
        ans_header.type = ntohs(ans_header.type);
        ans_header.class = ntohs(ans_header.class);
        ans_header.ttl = ntohs(ans_header.ttl);
        ans_header.data_len = ntohs(ans_header.data_len);

        if (ans_header.class == DNS_CLASS_IN && answer_header.data_len == 4) {
             memcpy(ip, dns_data + query_len + sizeof(struct dns_ans_hdr),
             ans_header.data_len);
        } else {
            kprintf("wrong class type");
            return -1;
        }
    } else {
        kprintf("no answer"),
        return -1;
    }

    }

    return 0;
}



