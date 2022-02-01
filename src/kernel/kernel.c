#include "arch/riscv/tty.c"
#include "../libc/stdio/printf.c"

void enter() {

    kprintf("val: %s\n", 'c');
    console_init();

}
