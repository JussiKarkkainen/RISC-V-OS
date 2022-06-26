#include "../../kernel/uart.h"

int putchar(char c) {
    //char s = (char) c;
    console_putc(c);
    return c;
}
