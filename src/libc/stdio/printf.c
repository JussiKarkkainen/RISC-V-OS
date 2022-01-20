#include <uart.h>

int printf(const char* format, ...) {
    
    va_list arg;
    va_start(arg, format);
    char* traverse;
    int idx = 0;

    for (traverse = format; *traverse != '\0'; traverse++) {
        while (*traverse != '%') {
            uart_putchar(*traverse);
            traverse++;
        }
        
        traverse++;

        switch(*traverse) {

            case 'c' : i = va_arg(arg, int);
                uart_putchar(i);
                break;
            
            case 'd' : i = va_srg(arg, int);
                if (i<0) {
                    i = -i;
                    uart_putchar('-');
                }
                puts(i);
                break;

            case 's' : s = va_arg(char *);
                uart_putchar(s);
                break;

            case 'x' : i = va_arg(unsigned int);
                uart_putchar(i);
                break;
        }

        va_end(arg);
        

        





