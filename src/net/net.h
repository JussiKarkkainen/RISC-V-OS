#ifndef NET_H
#define NET_H

#include <stdint.h>
#include <stddef.h>

struct sockaddr {
    unsigned short sa_family;
    char sa_data[14];
};

struct in_addr {
    unsigned long s_addr;
};

struct sockaddr_in {
    short int sin_family;
    unsigned short int sin_port;
    struct in_addr sin_addr;
    unsigned char sin_zero[8];
};



struct addrinfo {
    int ai_flags;
    int ai_family;
    int ai_socket;
    int ai_protocol;
    size_t ai_addrlen;
    struct sockaddr *ai_addr;
    char *ai_canonname;

    struct addrinfo *ai_next;
};

int getaddrinfo(const char *restrict node, 
                const char *restrict service,
                const struct addrinfo *restrict hints,
                struct addrinfo **restrict res);
int socket(int domain, int type, int protocol);
int close(int socket);
int sendto(int sockfd, void *buf, size_t len, int flags,
                const struct sockaddr *dst_addr, uint32_t addrlen);
int recvfrom(int sockfd, void *buf, size_t len, int flags,
                struct sockaddr *src_addr, uint32_t *addrlen);
int send(int socket, const void *buffer, size_t length, int flags);
int recv(int socket, void *buffer, size_t length, int flags);

#endif
