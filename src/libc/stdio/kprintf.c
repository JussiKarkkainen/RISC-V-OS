#include <stdarg.h>
#include "../../kernel/uart.h"
#include "../include/stdio.h"
#include <stddef.h>
#include "../../kernel/locks.h"

volatile int panicked = 0;

static char digits[] = "0123456789abcdef";

static struct {
  struct spinlock lock;
  int locking;
} pr;

void kprintf_init(void) {
  initlock(&pr.lock, "print lock");
  pr.locking = 1;
}

void panic(char *string) {
    pr.locking = 0;
    kprintf("panic: ");
    kprintf(string);
    kprintf("\n");
    panicked = 1;
    
    while (1) {
        ;
    }
}

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
    console_putc('0');
    console_putc('x');
    for (i = 0; i < (sizeof(uint32_t) * 2); i++, x <<= 4) {
        console_putc(digits[x >> (sizeof(uint32_t) * 8 - 4)]);
    }
}

void kprintf(char *format, ...) {

    va_list arg;
    va_start(arg, format);

    char *traverse = format;
    int i, locking;
    char *str;
    uint32_t p;
    
    locking = pr.locking;
    if (locking) {
        acquire_lock(&pr.lock);
    }

    while (*traverse != '\0') {
        if (*traverse != '%') {
            console_putc(*traverse);

        }
        else {
            traverse++;
            switch (*traverse) {
                
                case 'c' : i = va_arg(arg, int);
                    console_putc(i);
                    break;
                
                case 's' : 
                    if((str = va_arg(arg, char*)) == 0)
                        str = "(null)";
                    for(; *str; str++)
                        console_putc(*str);
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
    
    if (locking) {
        release_lock(&pr.lock);
    }
}

