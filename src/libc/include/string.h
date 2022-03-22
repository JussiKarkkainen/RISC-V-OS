#ifndef STRING_H
#define STRING_H

#include <stddef.h>

int memcmp(const void *str1, const void *str2, size_t n);
void *memcpy(void *s1, const void *s2, size_t n);
void *memset(void *str, int c, size_t n);
size_t strlen(const char* str);
void *memmove(void *dst, const void *src, size_t n);
int strncmp(const char *a, const char *b, unsigned int n);

#endif
