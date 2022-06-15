#include "../net/net.h"
#include "../libc/include/string.h"
#include "../libc/include/stdio.h"
#include <stddef.h>
#include <stdint.h>

int main() {

    char buf[2056];
    
    char *header = "GET / HTTP/1.1\r\nHost: info.cern.ch\r\n\r\nConnection: close\r\n\r\n";

    int sockfd, cnt;
    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    addr.sin_addr.s_addr = inet_addr("188.184.21.108");
    memset(addr.sin_zero, '\0', sizeof(addr.sin_zero));

    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        kprintf("sockfd = %d\n", sockfd); 
        kprintf("socket() returned error\n");
        return -1;
    }
    
    if (connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
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
