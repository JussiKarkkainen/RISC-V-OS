#include <stdarg.h>
#include "../../kernel/uart.h"
#include "putchar.c"
#include "../include/stdio.h"
#include <stddef.h>

static char digits[] = "0123456789abcdef";

void intprint(int num, int base, int sign) {
  
    char buf[16];
    int i;
    unsigned int x;

    if(sign && (sign = num < 0)) {
        x = -num;
    }
    else {
        x = num;
    }

    i = 0;
    do {
        buf[i++] = digits[x % base];
    } while((x /= base) != 0);

    if(sign) {
        buf[i++] = '-';
    }

    while (--i >= 0) {
        uart_putc(buf[i]);
    }
}

void ptr_print(uint32_t x) {
    unsigned int i;
    uart_putchar('0');
    uart_putchar('x');
    for (i = 0; i < (sizeof(uint32_t) * 2); i++, x <<= 4) {
        uart_putchar(digits[x >> (sizeof(uint32_t) * 8 - 4)]);
    }
}

void kprintf(char *format, ...) {

    va_list arg;
    va_start(arg, format);

    char *traverse = format;
    int i;
    char *str;
    uint32_t p;

    while (*traverse != '\0') {
        if (*traverse != '%') {
            uart_putchar(*traverse);

        }
        else {
            traverse++;
            switch (*traverse) {
                
                case 'c' : i = va_arg(arg, int);
                    putchar(i);
                    break;
                
                case 's' : str = va_arg(arg, char *); // Returns pointer to beginning of string.
                    write_uart(str);
                    break;

                case 'd' : i = va_arg(arg, int);
                    intprint(i, 10, 1);
                    break;

                case 'x' : i = va_arg(arg, int);
                    intprint(i, 16, 1);
                    break;
                
                case 'p' : p = va_arg(arg, uint32_t);
                    ptr_print(p);
                    break;

            }
        }

        traverse++;
    }

    
    va_end(arg);

}
