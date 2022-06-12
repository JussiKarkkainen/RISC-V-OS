#include "../net/sysnet.h"
#include "../libc/include/string.h"
#include "../libc/include/stdio.h"
#include <stddef.h>
#include <stdint.h>

int main() {

    char buf[2056];
    
    char *header = "GET / HTTP/1.1\r\nHost: info.cern.ch\r\n\r\nConnection: close\r\n\r\n";

    struct addrinfo hints, *res; 
    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    getaddrinfo("info.cern.ch", "80", &hints, &res);
    
    int sockfd, cnt;
    
    if ((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0) {
        kprintf("sockfd = %d\n", sockfd); 
        kprintf("socket() returned error\n");
        return -1;
    }
    
    if (connect(sockfd, res->ai_addr, res->ai_addrlen) < 0) {
        kprintf("connect returned error");
        return -1;
    }  

    size_t n;

    // Send request
    send(sockfd, header, strlen(header), 0);
        // read response
    while (1) {
        n = recv(sockfd, buf, sizeof(buf)-1, 0);
        if (n == 0) {
            break;
        }
        buf[n] = 0;
        kprintf("%s\n", buf);
    }

    return 0;
}
