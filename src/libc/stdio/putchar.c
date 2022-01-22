#include <stdio.h>
#include <uart.h>

int putchar(int c) {
    char s = (char) c;
    write_uart(&s);
    return c;
}
