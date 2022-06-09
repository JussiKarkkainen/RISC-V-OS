#ifndef ARPA_INET_H
#define ARPA_INET_H

#define htons(s) ((((s) & 0xff) << 8) | (((s) & 0xff00) >> 8))
#define ntohs(s) (htons(s))
#define htonl(s) ((((s)&0x000000ff) << 24) | (((s)&0x0000ff00) << 8) |  \
                 (((s)&0x00ff0000) >> 8) | (((s)&0xff000000) >> 24))
#define ntohl(s) (htonl(s))


uint16_t htons(uint16_t s);
uint16_t ntohs(uint16_t s);
uint32_t htonl(uint32_t s);
uint32_t ntohl(uint32_t s);


#endif
