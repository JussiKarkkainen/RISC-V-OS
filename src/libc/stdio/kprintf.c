#include <stdarg.h>
#include "../../kernel/arch/riscv/uart.h"
#include "putchar.c"




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

                case 'x' : i = va_arg(arg int);
                    putchar(convert(i, 16);
                    break

            }
        }
        traverse++;
    }

    
    va_end(arg);

}
