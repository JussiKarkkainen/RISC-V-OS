#ifndef SOCKET_H
#define SOCKET_H

#include "../kernel/file.h"
#include <stdint.h>
#include <stddef.h>

#define AF_INET 2
#define PF_INET 2

#define SOCK_STREAM 1
#define SOCK_DGRAM 2

typedef uint32_t socklen_t;

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


struct socket {
    int type;
    int desc;
};

struct file *socket_alloc(int domain, int type, int protocol);
int socket_connect(struct socket *s, struct sockaddr *addr, int addrlen);
int socket_read(struct socket *s, char *addr, int n);
int socket_write(struct socket *s, char *addr, int n);
int socket_sendto(struct socket *s, char *buf, int n, struct sockaddr *addr, int *addrlen);
int socket_recvfrom(struct socket *s, char *buf, int n, struct sockaddr *addr, int *addrlen);

#endif
