#include "../net/net.h"
#include "../net/socket.h"
#include "../net/arpa/inet.h"
#include "user.h"
#include <stddef.h>
#include <stdint.h>

// Program to get the html of info.cern.ch, chosen since it uses http instead of https
// In the future this should be more automated and be able to get html of every html website

int main() {
    char buf[2056];
    
    char *header = "GET / HTTP/1.1\r\nHost: info.cern.ch\r\n\r\nConnection: close\r\n\r\n";

    int sockfd;
    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    addr.sin_addr.s_addr = inet_addr("188.184.21.108");
    memset(addr.sin_zero, '\0', sizeof(addr.sin_zero));

    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        printf("sockfd = %d\n", sockfd); 
        printf("socket() returned error\n");
        return -1;
    }
    if (connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        printf("connect returned error\n");
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
        printf("%s\n", buf);
    }

    return 0;
}
