#ifndef ARPA_INET_H
#define ARPA_INET_H

#include <stdint.h>
#include "../socket.h"

#define HTONS(s) ((((s) & 0xff) << 8) | (((s) & 0xff00) >> 8))
#define NTOHS(s) htons(s)
#define HTONL(s) ((((s)&0x000000ff) << 24) | (((s)&0x0000ff00) << 8) |  \
                 (((s)&0x00ff0000) >> 8) | (((s)&0xff000000) >> 24))
#define NTOHL(s) htonl(s)


uint16_t htons(uint16_t s);
uint16_t ntohs(uint16_t s);
uint32_t htonl(uint32_t s);
uint32_t ntohl(uint32_t s);


#define htons(s) HTONS(s)
#define ntohs(s) NTOHS(s)
#define htonl(s) HTONL(s)
#define ntohl(s) NTOHL(s)

int inet_aton(const char *cp, struct in_addr *addr);
//uint32_t inet_addr(const char *cp);

#endif
