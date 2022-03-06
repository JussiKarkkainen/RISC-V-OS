#include <stdarg.h>
#include "../../kernel/uart.h"
#include "putchar.c"
#include "../include/stdio.h"
#include <stddef.h>

void hexprint(int num) {
    
    int rem; 
    int i = 0;
    char buffer[50];

    while(num != 0) {
        rem = num % 16;  

        if (rem < 10) {
            buffer[i++] = 48 + rem;
        }
        else {
            buffer[i++] = 55 + rem;
        }

        num /= 16;  
    }
    
    char *str = "0x";
    write_uart(str);

    for(int j = i - 1; j >= 0 ; j--) {
        putchar(buffer[j]);
    } 
}

static char digits[] = "0123456789abcdef";

void ptr_print(uint32_t x) {
    int i;
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
                    if (i<0) {
                        i = -i;
                        putchar('-');
                    }
                    putchar(i);
                    break;

                case 'x' : i = va_arg(arg, int);
                    hexprint(i);
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
