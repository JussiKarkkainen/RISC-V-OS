#include "arch/riscv/tty.c"
#include "arch/riscv/uart.h"
#include "../libc/stdio/kprintf.c"
//#include "../libc/stdio/putchar.c"

void enter(void) {
    //putchar('c');
    kprintf("hope no loop: %x\n", 123);
    console_init();

}
