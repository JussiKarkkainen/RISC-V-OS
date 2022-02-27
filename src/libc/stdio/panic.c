#include "../include/stdio.h"

void panic(char *string) {
    kprintf("panic: ");
    kprintf(string);
    kprintf("\n");
}
