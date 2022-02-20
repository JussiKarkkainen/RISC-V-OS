#include "../../kernel/uart.h"

int putchar(char c) {
    //char s = (char) c;
    uart_putchar(c);
    return c;
}
