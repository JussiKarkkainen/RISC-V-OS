#include <string.h>
#include <stddef..h>

void *memcpy(void *s1, const void *s2, size_t n) {
    unsigned char *dst = (unsigned char *)s1;
    unsigned char *src = (const unsigned char *)s2;
    for (int i=0; i<=n; i++) 
        dst[i] = src[i];
    return s1;
}
