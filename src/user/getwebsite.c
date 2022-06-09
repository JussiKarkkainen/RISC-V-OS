#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define MAXLINE 100 
#define MAXSUB 100

int main(int argc, char *argv[]) {

    if (argc < 2) {
        printf("Usage: getwebsite domain\n");
        return -1;
    }

    char *web_domain = argv[1];
    char *page = "/";

    char sendline[MAXLINE + 1], recvline[MAXLINE + 1];

    // Form request
    snprintf(sendline, MAXSUB,
     "GET %s HTTP/1.0\r\n"
     "Host: %s\r\n", page, web_domain);


    struct addrinfo hints, *res; 
    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    printf("web_domain %s\n", web_domain);
    getaddrinfo(web_domain, "80", &hints, &res);
    
    int sockfd;
    
    if (sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol) < 0) {
        printf("socket() returned error\n");
        return -1;
    }
    printf("sockfd = %d\n", sockfd); 
    int cnt;
     
    if (cnt = connect(sockfd, res->ai_addr, res->ai_addrlen) < 0) {
        printf("count %d\n", cnt);
        printf("connect() returned error %d\n", cnt); 
        return -1;
    }
    
    size_t n;

    // Send request
    if (send(sockfd, sendline, strlen(sendline), 0) >= 0) {
        // read response
        while (n = recv(sockfd, recvline, MAXLINE, 0) > 0) {
            recvline[n] = '\0';

            printf("hello %s\n", recvline);
        }
    }

    return 0;
}
