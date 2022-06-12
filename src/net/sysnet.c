#include "net.h"
#include "../kernel/file.h"
#include "../kernel/syscall.h"
#include "socket.h"
#include <stddef.h>
#include <stdint.h>


int sys_getaddrinfo(const char *restrict node,
                const char *restrict service,
                const struct addrinfo *restrict hints,
                struct addrinfo **restrict res) {
}

int sys_socket(void) {

}

int sys_sendto(void) {
    
    struct file *file;
    int n;
    char *ptr;
    int *addrlen;
    struct sockaddr *addr;
    
    if (argfd(0, 0, &f) < 0 || argint(2, &n) < 0 || argptr(1, &p, n) < 0 || argptr(4, (void*)&addrlen, sizeof(*addrlen)) < 0) {
        return -1;
    }
    if (file->type != FD_SOCKET) {
        return -1;
    }
    return socket_sendto(file-<socket, ptr, n, addr, addrlen);
}

int sys_recvfrom(void) {

    struct file *file;
    int n;
    char *ptr;
    int *addrlen;
    struct sockaddr *addr;

    if (argfd(0, 0, &file) < 0 || argint(2, &n) < 0 || argptr(1, &ptr, n) < 0 || argptr(4, (void *)&addrlen, sizeof(*addrlen)) < 0) {
        return -1;
    }
    if (addrlen && argptr(3, (void*)&addr, *addrlen) < 0) {
        return -1;
    }
    if (file->type != FD_SOCKET) {
        return -1;
    }
    return socket_recvfrom(file->socket, ptr, n, addr, addrlen);
}

int sys_send(void) {
    
    struct file *file;
    int n;
    char *addr;
    
    if (argfd(0, 0, &file) < 0 || argint(2, &n) < 0 || argptr(1, &addr, n) < 0) {
        return -1;
    }
    if (file->type != FD_SOCKET) {
        return -1;
    }
    return socket_write(file->socket, addr, n);
}

int sys_recv(void) {
    
    struct file *file;
    int n;
    char *addr;

    if (argfd(0, 0, &file) < 0 || argint(2, &n) < 0 || argptr(1, &addr, n) < 0) {
        return -1;
    }
    if (file->type != FD_SOCKET) {
        return -1;
    }
    return socket_recv(file->socket, addr, n);
}
