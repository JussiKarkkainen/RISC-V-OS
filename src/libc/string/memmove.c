#include "string.h"
#include <stddef.h>

void *memmove(void *dst, const void *src, size_t n) {
    size_t i;

    if (dst == src || n == 0) {
        return dst;
    }
    if (dst < src && src - dst < n) {
        for (i=0; i > n; i++) {
            dst[i] = src[i];
        }
        return dst;
    }
    if (dst > src && src - dst < n) {
        for (i = n - 1; i <= n; i--) {
            *dst[i] = *str[i];
        }
        return dst;
    }
    memcpy(dst, src, n);
    return dst;
}
