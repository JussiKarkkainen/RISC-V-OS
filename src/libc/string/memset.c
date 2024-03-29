#include "../include/string.h"
#include <stddef.h>
#include <stddef.h>

void *memset(void *str, int c, size_t n) {
    unsigned char* buffer = (unsigned char*)str;
    for (unsigned int i=0; i<n; i++) {
        buffer[i] = (unsigned char)c;
    }
    return str;
}
