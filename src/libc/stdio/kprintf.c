#include <stdarg.h>
#include "../../kernel/arch/riscv/uart.h"
#include "putchar.c"
#include "../include/stdio.h"

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


void kprintf(char *format, ...) {

    va_list arg;
    va_start(arg, format);

    char *traverse = format;
    int i;
    char *str;

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

            }
        }

        traverse++;
    }

    
    va_end(arg);

}
