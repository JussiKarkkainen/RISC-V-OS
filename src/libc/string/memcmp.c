#include <stddef.h>

int memcmp(const void *str1, const void *str2, size_t n) {
    const unsigned char *a = (const unsigned char*)str1;
    const unsigned char *b = (const unsigned char*)str2;

    for (size_t i=0; i<n; i++) {
        if (a[i] < b[i]) 
            return 1;
        else if (a[i] > b[i])
            return -1;
    }
    return 0;
}
