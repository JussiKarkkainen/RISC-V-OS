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



#endif
