#include "socket.h"
#include "udp.h"
#include "tcp.h"
#include "../kernel/paging.h"
#include "../kernel/file.h"
#include "../kernel/filesys.h"
#include "../libc/include/stdio.h"
#include <stdint.h>
#include <stddef.h>

struct file *socket_alloc(int domain, int type, int protocol) {
    
    struct file *file;
    struct socket *socket;

    if (domain != AF_INET || (type != SOCK_DGRAM && type != SOCK_STREAM) || protocol != 0) {
        return NULL;
    }

    if (!(file = file_alloc())) {
        return NULL;
    }
    
    socket = (struct socket *)kalloc();
    if (!socket) {
        return NULL;
    }
    socket->type = type;
    socket->desc = (type == SOCK_STREAM ? tcp_assign_desc() : udp_assign_desc());
    file->type = FD_SOCKET;
    file->readable = 1;
    file->writable = 1;
    file->socket = socket;
    return file;
}

int socket_connect(struct socket *s, struct sockaddr *addr, int addrlen) {
    if (s->type != SOCK_STREAM) {
        return -1;
    }
    kprintf("s->desc %d addrlen %d\n", s->desc, addrlen);
    return tcp_connect(s->desc, addr, addrlen);
}

int socket_read(struct socket *s, char *addr, int n) {
    return 0;
    /*
    if (s->type != SOCK_STREAM) {
        return -1;
    }
    return tcp_recv(s->desc, (uint8_t *)addr, n);
*/
}

int socket_write(struct socket *s, char *addr, int n) {
    return 0;
    /*
    if (s->type != SOCK_STREAM) {
        return -1;
    }
    return tcp_send(s->desc, (uint8_t *)addr, n);
*/
}

int socket_sendto(struct socket *s, char *buf, int n, struct sockaddr *addr, int *addrlen) {
    return 0;
    /*
    if (s->type != SOCK_DGRAM) {
        return -1;
    }
    return udp_sendto(s->desc, (uint8_t *)buf, n, addr, addrlen);
    */
}

int socket_recvfrom(struct socket *s, char *buf, int n, struct sockaddr *addr, int *addrlen) {
    return 0;
    /*
    if (s->type != SOCK_DGRAM) {
        return -1;
    }
    return udp_recvfrom(s->desc, (uint8_t *)buf, n, addr, addrlen);
    */
}
