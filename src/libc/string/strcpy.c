#include "../include/string.h"

char *strcpy(char *s, const char *t unsigned int n) {
    
    char *ps;

    os = s;
    if(n <= 0) {
        return os;
    }
    while(--n > 0 && (*s++ = *t++) != 0) {
        ;
    }
    *s = 0;
    return os;
}
