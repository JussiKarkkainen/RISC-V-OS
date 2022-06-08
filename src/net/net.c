#include "net.h"
#include <stddef.h>
#include <stdint.h>


int socket(int domain, int type, int protocol) {
}

int sendto(int sockfd, const void *buf, size_t len, int flags,
               const struct sockaddr *dest_addr, socklen_t addrlen) {
}

int recvfrom(int sockfd, void *buf, size_t len, int flags,
                 struct sockaddr *src_addr, socklen_t *addrlen) {
}
