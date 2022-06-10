#include "net.h"
#include <stddef.h>
#include <stdint.h>


int getaddrinfo(const char *restrict node,
                const char *restrict service,
                const struct addrinfo *restrict hints,
                struct addrinfo **restrict res) {
}

int socket(int domain, int type, int protocol) {
}

int close(int socket) {
}

int sendto(int sockfd, const void *buf, size_t len, int flags,
               const struct sockaddr *dest_addr, socklen_t addrlen) {
}

int recvfrom(int sockfd, void *buf, size_t len, int flags,
                 struct sockaddr *src_addr, socklen_t *addrlen) {
}

int send(int socket, const void *buffer, size_t length, int flags) {
}

int recv(int socket, void *buffer, size_t length, int flags) {
}
