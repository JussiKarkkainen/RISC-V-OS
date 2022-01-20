#include <string.h>

void *memset(void *str, int c, size_t n) {
    unsigned char* buffer = (unsigned char*)str;
    for (int i=0; i<n; i++) {
        buf[i] = (unsigned char)c;
    return str;
}
