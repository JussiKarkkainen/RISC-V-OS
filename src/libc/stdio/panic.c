#include "printf.c"

void puts(char *string) {
    kprintf("panic: ");
    kprintf(string);
    kprintf("\n");
}
