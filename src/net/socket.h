#ifndef SOCKET_H
#define SOCKET_H

#include "../kernel/file.h"

struct socket {
    int type;
    int desc;
}

struct *file socket_alloc(int domain, int type, int protocol);
int socket_connect(struct socket *s, struct sockaddr *addr, int addrlen);
int socket_read(struct socket *s, char *addr, int n);
int socket_write(struct socket *s, char *addr, int n);
int socket_sendto(struct socket *s, char *buf, int n, struct sockaddr *addr, int *addrlen);
int socket_recvfrom(struct socket *s, char *buf, int n, struct sockaddr *addr, int *addrlen);

#endif
