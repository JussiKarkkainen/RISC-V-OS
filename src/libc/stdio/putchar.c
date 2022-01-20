#include <stdio.h>
#include <4uart.c>

int putchar(int c) {
    char s = (char) c;
    uart_putchar(&s);
    return c;
}
