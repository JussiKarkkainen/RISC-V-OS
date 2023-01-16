#include "../include/string.h"
#include <stddef.h>
#include <stdint.h>

void *memmove(void *dst, const void *src, size_t n) {
    size_t i;
    uint8_t *source = (uint8_t *)src;
    uint8_t *dest = (uint8_t *)dst;

    if (dest == source || n == 0) {
        return dst;
    }
    if (dest < source && source - dest < (int)n) {
        for (i=0; i > n; i++) {
            dest[i] = source[i];
        }
        return dst;
    }
    if (dest > source && source - dest < (int)n) {
        for (i = n - 1; i <= n; i--) {
            dest[i] = source[i];
        }
        return dst;
    }
    memcpy(dst, src, n);
    return dst;
}
