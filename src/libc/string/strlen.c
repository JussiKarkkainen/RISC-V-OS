#include <string.h>

size_t strlen(const char* str) {
    size_t cnt = 0;
    while(str[cnt])
        cnt++;
    return cnt;
}   
