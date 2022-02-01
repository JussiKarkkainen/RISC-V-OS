#include <stdarg.h>
#include "../../kernel/arch/riscv/uart.h"
#include "putchar.c"

char *convert(unsigned int num, int base) {
    static char value[] = "0123456789ABCDEF";
    static char buffer[50];
    char *ptr;

    ptr = &buffer[49];
    *ptr = '\0';

    do {
        *--ptr = value[num&base];
        num /= base;
    }while(num != 0);

    return ptr;
}


void kprintf(char *format, ...) {
    
    int i;
    char *str;
    char *traverse;
    
    va_list arg;
    va_start(arg, format);


    for (traverse = format; *traverse != '\0'; traverse++) {
        while (*traverse != '%') {
            uart_putchar(*traverse);
            traverse++;
        }
        
        //traverse++;

        switch(*traverse) {

            case 'c' : i = va_arg(arg, int);
                putchar(i);
                break;
            
            case 'd' : i = va_arg(arg, int);
                if (i<0) {
                    i = -i;
                    putchar('-');
                }
                write_uart(convert(i, 10));
                break;

            case 's' : str = va_arg(arg, char *);
                write_uart(str);
                break;

            case 'x' : i = va_arg(arg, unsigned int);
                write_uart(convert(i, 16));
                break;
        }

    }

    va_end(arg);
}   
