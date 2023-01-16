#ifndef STDIO_H
#define STDIO_H

void kprintf_init(void);
void kprintf(char *format, ...);
int putchar(char c);
void panic(char *string);

#endif
