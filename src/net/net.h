#ifndef NET_H
#define NET_H

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

int socket(int domain, int type, int protocol);
int close(int socket);
int sendto(int sockfd, void *buf, size_t len, int flags,
                const struct sockaddr *dst_addr, uint32_t addrlen);
int recvfrom(int sockfd, void *buf, size_t len, int flags,
                struct sockaddr *src_addr, uint32_t *addrlen);


#endif
