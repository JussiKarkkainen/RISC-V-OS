#include "socket.h"
#include <stdint.h>




struct *file socket_alloc(int domain, int type, int protocol) {
}


int socket_connect(struct socket *s, struct sockaddr *addr, int addrlen) {
}


int socket_read(struct socket *s, char *addr, int n) {

    if (s->type != SOCK_STREAM) {
        return -1;
    }
    return tcp_recv(s->desc, (uint8_t *)addr, n);
}


int socket_write(struct socket *s, char *addr, int n) {

    if (s->type != SOCK_STREAM) {
        return -1;
    }
    return tcp_send(s->desc, (uint8_t *)addr, n);
}


int socket_sendto(struct socket *s, char *buf, int n, struct sockaddr *addr, int *addrlen) {
}


int socket_recvfrom(struct socket *s, char *buf, int n, struct sockaddr *addr, int *addrlen) {
}






