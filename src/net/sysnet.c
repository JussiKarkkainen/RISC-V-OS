#include "net.h"
#include <stddef.h>
#include <stdint.h>


int sys_getaddrinfo(const char *restrict node,
                const char *restrict service,
                const struct addrinfo *restrict hints,
                struct addrinfo **restrict res) {
}

int sys_socket(int domain, int type, int protocol) {

}

int sys_sendto(int sockfd, const void *buf, size_t len, int flags,
               const struct sockaddr *dest_addr, socklen_t addrlen) {

}

int sys_recvfrom(int sockfd, void *buf, size_t len, int flags,
                 struct sockaddr *src_addr, socklen_t *addrlen) {
}

int sys_send(int socket, const void *buffer, size_t length, int flags) {

    return sendto(socket, buffer, length, flags, NULL, 0);
}

int sys_recv(int socket, void *buffer, size_t length, int flags) {

    return recvfrom(socket, buf, length, flags, NULL, 9);
}
