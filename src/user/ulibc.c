#include "user.h"

char *strcpy(char *s, const char *t, unsigned int n) {
    
    char *ps;

    ps = s;
    if(n <= 0) {
        return ps;
    }
    while(--n > 0 && (*s++ = *t++) != 0) {
        ;
    }
    *s = 0;
    return ps;
}


int strncmp(const char *a, const char *b, unsigned int n) {
    while (n && *a && (*a == *b)) {
        n--;
        a++;
        b++;
    }
    if (n == 0) {
        return 0;
    }
    else {
        return (unsigned char)*a - (unsigned char)*b;
    }
}

void *memset(void *str, int c, size_t n) {
    unsigned char* buffer = (unsigned char*)str;
    for (unsigned int i=0; i<n; i++) {
        buffer[i] = (unsigned char)c;
    }
    return str;
}

// Returns a pointer to the first occurrence of character in the string
char *strchr(const char *c, char s) {
    for (; *c, c++) {
        if (*c == s) {
            return (char *)c;
        }
    }
    return 0;
}

char *gets(char *buf, int max) {
    int i, cc;
    char c;

    for (i=0; i+1 < max; ) {
        cc = read(0, &c, 1);
        if (cc < 1) {
            break;
        }
        buf[i++] = c;
        if (c == '\n' || c == '\r') {
            break;
    }
    buf[i] = '\0';
    return buf;
}

// used to list properties of a file identified by path
int stat(const char *n, struct stat *st) {
    int fd;
    int r;

    fd = open(n, O_RDONLY);
    if (fd < 0) {
        return -1;
    }
    r = fstat(fd, st);
    close(fd);
    return r;
}

int atoi(const char *s) {
    int n;

    n = 0;
    while ('0' <= *s && *s <= '9') {
        n = n*10 + *s++ - '0';
    }
    return n;
}

void* memmove(void *vdst, const void *vsrc, int n) {
    char *dst;
    const char *src;

    dst = vdst;
    src = vsrc;
    if (src > dst) {
        while (n-- > 0) {
            *dst++ = *src++;
        }
    } 
    else {
        dst += n;
        src += n;
        while (n-- > 0) {
            *--dst = *--src;
        }
    }
    return vdst;
}

int memcmp(const void *s1, const void *s2, uint n) {
    const char *p1 = s1, *p2 = s2;
    while (n-- > 0) {
        if (*p1 != *p2) {
            return *p1 - *p2;
        }
        p1++;
        p2++;
    }
    return 0;
}

void *memcpy(void *dst, const void *src, uint n) {
    return memmove(dst, src, n);
}

